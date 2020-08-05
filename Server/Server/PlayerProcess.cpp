#include "stdafx.h"
#include "PlayerProcess.h"
#include "Network.h"
#include "PacketMgr.h"
#include "Player.h"
#include "DataBase.h"
#include "TimerMgr.h"

CPlayerProcess::CPlayerProcess()
{
	if (!CProcess::m_pPlayerPool)
		CProcess::m_pPlayerPool = new class CPlayerpool();
	BindPacketProcess();
}


CPlayerProcess::~CPlayerProcess()
{
	if (m_pPlayerPool)
	{
		delete m_pPlayerPool;
		m_pPlayerPool = nullptr;
	}
}

void CPlayerProcess::AcceptClient(const SOCKET& sSocket, unsigned short playerId)
{
	CProcess::m_pPlayerPool->m_cumPlayerPool[playerId]->SetSocket(sSocket);
	CProcess::m_pPlayerPool->m_cumPlayerPool[playerId]->SetNumID(playerId);
	CProcess::m_pPlayerPool->m_cumPlayerPool[playerId]->SetRecvState();
}

void CPlayerProcess::RecvPacket(unsigned short playerId, char * packet, DWORD bytesize)
{
	char* cPacket = CProcess::m_pPlayerPool->m_cumPlayerPool[playerId]->RecvEvent(bytesize, packet);

	if (cPacket != nullptr) {
		m_fpPacketProcess[packet[1]](playerId, packet);
		CProcess::m_pPlayerPool->m_cumPlayerPool[playerId]->SetRecvState();
	}
}

void CPlayerProcess::PlayerLogin(unsigned short playerId, char * packet)
{
	cs_login_packet*	login_packet = reinterpret_cast<cs_login_packet*>(packet);
	
	if (!ExistLoginList(playerId))
	{
		InsertLoginList(playerId);
		CPacketMgr::Send_Login_OK_Packet(playerId);
	}
	else {
		CPacketMgr::Send_Login_Fail_Packet(playerId);
		return;
	}

	// Player Init
	{
		tPlayerStatus tStatus;
		tStatus.fHP = 100.f;
		tStatus.fHungry = 100.f;
		tStatus.fStamina = 100.f;
		tStatus.fThirst = 100.f;
		tStatus.fSpeed = 200.f;

		m_pPlayerPool->m_cumPlayerPool[playerId]->SetPlayerStatus(tStatus);
	}
	m_pPlayerPool->m_cumPlayerPool[playerId]->SetNumID(login_packet->id);
	m_pPlayerPool->m_cumPlayerPool[playerId]->SetWcID(login_packet->player_id);
	m_pPlayerPool->m_cumPlayerPool[playerId]->SetLocalPos(Vec3(10000.f, 273.f, 10000.f));
	m_pPlayerPool->m_cumPlayerPool[playerId]->SetLocalScale(Vec3(1.5f, 1.5f, 1.5f));
	m_pPlayerPool->m_cumPlayerPool[playerId]->SetLocalRot(Vec3(0.f, 0.f, 0.f));
	m_pPlayerPool->m_cumPlayerPool[playerId]->SetConnect(true);

	// Server -> Client�� �ʱ� �÷��̾� �� ��Ŷ ����
	sc_status_player_packet status_packet;
	status_packet.id = m_pPlayerPool->m_cumPlayerPool[playerId]->GetNumID();
	status_packet.size = sizeof(sc_status_player_packet);
	status_packet.type = SC_STATUS_PLAYER;
	status_packet.fHP = m_pPlayerPool->m_cumPlayerPool[playerId]->GetHP();
	status_packet.fHungry = m_pPlayerPool->m_cumPlayerPool[playerId]->GetHungry();
	status_packet.fStamina = m_pPlayerPool->m_cumPlayerPool[playerId]->GetStamina();
	status_packet.vLocalPos = m_pPlayerPool->m_cumPlayerPool[playerId]->GetLocalPos();
	//status_packet.vLocalRot = m_pPlayerPool->m_cumPlayerPool[playerId]->GetLocalRot();

	// Player ViewList Update
	Vec3 player_pos = m_pPlayerPool->m_cumPlayerPool[playerId]->GetLocalPos();

	concurrent_unordered_set<unsigned short> list;
	CopyBeforeLoginList(list);

	// [ Add Player List ]
	for (auto& au : list) {
		if (playerId == au)
			continue;

		bool bConnect = m_pPlayerPool->m_cumPlayerPool[au]->GetConnect();
		if (false == bConnect)
			continue;
		else
		{
			Vec3 other_pos = m_pPlayerPool->m_cumPlayerPool[au]->GetLocalPos();

			if (ObjectRangeCheck(player_pos, other_pos, PLAYER_BETWEEN_RANGE))
			{
				CPacketMgr::Send_Put_Player_Packet(au, playerId);
				CPacketMgr::Send_Put_Player_Packet(playerId, au);
			}
		}
	}

	// [ Add Monster List ]
	for (auto& au : m_pMonsterPool->m_cumMonsterPool) {
		if (au.second->GetState() == OBJ_STATE_DIE) continue;
		Vec3 monster_pos = au.second->GetLocalPos();

		if (true == ObjectRangeCheck(player_pos, monster_pos, PLAYER_BETWEEN_RANGE))
		{
			if (false == au.second->GetWakeUp())
			{
				CPacketMgr::Send_Wakeup_Npc_Packet(playerId, au.first);

				Update_Event ev;
				ev.m_Do_Object = au.first;
				ev.m_EventType = EVENT_TYPE::EV_MONSTER_UPDATE;
				ev.m_From_Object = NO_TARGET;
				ev.m_ObjState = OBJ_STATE_IDLE;
				ev.wakeup_time = high_resolution_clock::now() + 1s;
				PushEventQueue(ev);
			}
			CPacketMgr::Send_Put_Npc_Packet(playerId, au.first);
		}
	}
}

void CPlayerProcess::PlayerMove(unsigned short playerId, char * packet)
{
	cs_move_packet* move_packet = reinterpret_cast<cs_move_packet*>(packet);

	bool bRun = move_packet->bRun;
	Vec3 vWorldDir = move_packet->vWorldDir;

	Vec3 vPos = CProcess::m_pPlayerPool->m_cumPlayerPool[playerId]->GetLocalPos();
	Vec3 vOriginPos = CProcess::m_pPlayerPool->m_cumPlayerPool[playerId]->GetLocalPos();
	float fSpeed = CProcess::m_pPlayerPool->m_cumPlayerPool[playerId]->GetSpeed();
	float fHeight = move_packet->fHeight;
	cout << fHeight << endl;
	float fMaxHeight = max(0, fHeight);
	// Walk or Run
	if (bRun)
	{
		fSpeed *= 5.f;
	}
	vPos += vWorldDir * fSpeed * 0.2f;
	// Height
	/*if (vOriginPos.y > fMaxHeight + 5.f)
	{
		vOriginPos.y -= fSpeed * 0.2f * 5.f;
	}*/
	cout << "aft)Pos : " << vPos.x << ", " << vOriginPos.y << ", " << vPos.z << endl;
	CProcess::m_pPlayerPool->m_cumPlayerPool[playerId]->SetLocalPos(Vec3(vPos.x, fHeight, vPos.z));

	//CPacketMgr::GetInst()->Send_Pos_Player_Packet(playerId, playerId);
	UpdateViewList(playerId);
}

void CPlayerProcess::PlayerLogout(unsigned short playerId)
{
	// �̹� Disconnect�� ������ ���
	if (!m_pPlayerPool->m_cumPlayerPool[playerId]->GetConnect()) 
		return;
	// ���� Diconnect�� ���°� �ƴ� ���
	m_pPlayerPool->m_cumPlayerPool[playerId]->SetConnect(false);

	if (ExistLoginList(playerId))
		DeleteLoginList(playerId);

	concurrent_unordered_set<unsigned short>list;
	CopyBeforeLoginList(list);
	for(auto& au : list)
		CPacketMgr::Send_Remove_Player_Packet(au, playerId);
}

void CPlayerProcess::PlayerRot(unsigned short playerId, char * packet)
{
	cs_rot_packet* rot_packet = reinterpret_cast<cs_rot_packet*>(packet);
	unsigned short id = rot_packet->id;
	Vec3 vRot = rot_packet->vRot;

	m_pPlayerPool->m_cumPlayerPool[playerId]->SetLocalRot(vRot);

	concurrent_unordered_set<unsigned short> viewList;

	m_pPlayerPool->m_cumPlayerPool[playerId]->CopyPlayerList(viewList);

	for (auto& au : viewList)
	{
		CPacketMgr::Send_Rot_Player_Packet(au, playerId);
	}
}

void CPlayerProcess::PlayerCollisionAnimal(unsigned short playerId, char * packet)
{
	cs_collision_packet* collision_packet = reinterpret_cast<cs_collision_packet*>(packet);

	bool AnimalId = collision_packet->id;
	bool bRun = collision_packet->bRun;

	Vec3 vAnimalPos = m_pMonsterPool->m_cumMonsterPool[AnimalId]->GetLocalPos();
	Vec3 vPlayerPos = m_pPlayerPool->m_cumPlayerPool[playerId]->GetLocalPos();
	float fPlayerSpeed = m_pPlayerPool->m_cumPlayerPool[playerId]->GetSpeed();
	Vec3 vDir = XMVector3Normalize(vPlayerPos - vAnimalPos);
	vDir.y = 0.f;

	if (bRun)
	{
		vPlayerPos += vDir * fPlayerSpeed * 0.02f * 5.f;
	}
	else
	{
		vPlayerPos += vDir * fPlayerSpeed * 0.2f;
	}

	m_pPlayerPool->m_cumPlayerPool[playerId]->SetLocalPos(vPlayerPos);

	UpdateViewList(playerId);
}

void CPlayerProcess::PlayerCollisionNatural(unsigned short playerId, char * packet)
{
	/*cs_collision_packet* collision_packet = reinterpret_cast<cs_collision_packet*>(packet);

	bool AnimalId = collision_packet->id;
	bool bRun = collision_packet->bRun;

	Vec3 vNatural = m_pMonsterPool->m_cumMonsterPool[AnimalId]->GetLocalPos();
	Vec3 vPlayerPos = m_pPlayerPool->m_cumPlayerPool[playerId]->GetLocalPos();
	float fPlayerSpeed = m_pPlayerPool->m_cumPlayerPool[playerId]->GetSpeed();
	Vec3 vDir = XMVector3Normalize(vPlayerPos - vAnimalPos);
	vDir.y = 0.f;

	if (bRun)
	{
		vPlayerPos += vDir * fPlayerSpeed * 0.2f * 5.f;
	}
	else
	{
		vPlayerPos += vDir * fPlayerSpeed * 0.2f;
	}

	m_pPlayerPool->m_cumPlayerPool[playerId]->SetLocalPos(vPlayerPos);

	UpdateViewList(playerId);*/
}

void CPlayerProcess::PlayerChat(unsigned short _usID, char * _packet)
{
	cs_chat_packet* chat_packet = reinterpret_cast<cs_chat_packet*>(_packet);

	concurrent_unordered_set<unsigned short> loginList;

	CopyBeforeLoginList(loginList);

	for (auto& player : loginList) {
		CPacketMgr::GetInst()->Send_Chat_Packet(_usID, player, chat_packet->meesage);
	}
}

void CPlayerProcess::UpdateViewList(unsigned short playerId)
{
	concurrent_unordered_set<unsigned short> loginList; // ���� �α��� ����Ʈ
	concurrent_unordered_set<unsigned short> beforeList; // ���� �� ����Ʈ
	concurrent_unordered_set<unsigned short> afterList; // ���� �� ����Ʈ

	// ������ ����Ʈ�� �޾ƿɴϴ�.
	CopyBeforeLoginList(loginList);
	// ���� �÷��̾��� �þ�ó�� ���� ����Ʈ�� �޾ƿɴϴ�.
	CProcess::m_pPlayerPool->m_cumPlayerPool[playerId]->CopyPlayerList(beforeList);
	// ���� �÷��̾� ��ġ�� �޾ƿɴϴ�.
	Vec3 player_pos = CProcess::m_pPlayerPool->m_cumPlayerPool[playerId]->GetLocalPos();

	// After List�� �þ�ó�� ����Ʈ �߰� �۾� [ Player ] 
	for (auto& au : loginList) 
	{
		if (au == playerId) continue;
		if (false == CProcess::m_pPlayerPool->m_cumPlayerPool[au]->GetConnect()) continue;
		Vec3 other_pos = CProcess::m_pPlayerPool->m_cumPlayerPool[au]->GetLocalPos();
		if (ObjectRangeCheck(player_pos, other_pos, PLAYER_BETWEEN_RANGE)) {
			afterList.insert(au);
			continue;
		}
	}

	//After List�� �þ�ó�� ����Ʈ �߰� �۾� [ Monster ]
	for (auto& au : m_pMonsterPool->m_cumMonsterPool) {
		// ���� ���Ͱ� �׾������� continue;
		if (au.second->GetState() == OBJ_STATE_DIE) continue;
		
		Vec3 monster_pos = au.second->GetLocalPos();
		// ���� ���Ͱ� �÷��̾��� ���� ���� ���� �ִٸ� �þ�ó���� �־��ش�.
		if (ObjectRangeCheck(player_pos, monster_pos, PLAYER_BETWEEN_RANGE)) {
			afterList.insert(au.first + MAX_USER);
			continue;
		}
	}

	CPacketMgr::Send_Pos_Player_Packet(playerId, playerId);

	for (auto& au : afterList) {
		if (0 == beforeList.count(au)) // ViewList ������Ʈ ���� �������� �ʾҴ� ���
		{
			if (au < MAX_USER) // Player
			{
				CPacketMgr::GetInst()->Send_Put_Player_Packet(playerId, au);
				CPacketMgr::GetInst()->Send_Put_Player_Packet(au, playerId);
			}
			else // Animal
			{
				unsigned short monster_id = au - MAX_USER;
				if (false == m_pMonsterPool->m_cumMonsterPool[monster_id]->GetWakeUp())
				{
					Update_Event ev;
					ev.m_Do_Object = monster_id;
					ev.m_EventType = EVENT_TYPE::EV_MONSTER_UPDATE;
					ev.m_From_Object = NO_TARGET;
					ev.m_ObjState = OBJ_STATE_IDLE;
					ev.wakeup_time = high_resolution_clock::now() + 1s;
					PushEventQueue(ev);
				}
				else {
					char monster_type = m_pMonsterPool->m_cumMonsterPool[monster_id]->GetType();
					Update_Event ev;
					ev.m_Do_Object = monster_id;
					ev.m_EventType = EV_MONSTER_UPDATE;
					ev.m_From_Object = playerId;
					if (B_WARLIKE == monster_type)
						ev.m_ObjState = OBJ_STATE_FOLLOW;
					else if (B_PASSIVE == monster_type)
						ev.m_ObjState = OBJ_STATE_IDLE;
					else if (B_EVASION == monster_type)
						ev.m_ObjState = OBJ_STATE_EVASION;
					ev.wakeup_time = high_resolution_clock::now() + 1s;
					PushEventQueue(ev);
				}
				CPacketMgr::GetInst()->Send_Put_Npc_Packet(playerId, au - MAX_USER);
			}
		}
		else // VIewList ������Ʈ ���� �������� ���
		{
			CPacketMgr::GetInst()->Send_Pos_Player_Packet(playerId, au);
			CPacketMgr::GetInst()->Send_Pos_Player_Packet(au, playerId);
		}
	}

	for (auto& au : beforeList) {
		if (0 == afterList.count(au)) // ViewList ������Ʈ �Ŀ��� ������ ���,
		{
			if (au < MAX_USER)
			{
				CPacketMgr::Send_Remove_Player_Packet(playerId, au);
				CPacketMgr::Send_Remove_Player_Packet(au, playerId);
			}
			else
			{
				CPacketMgr::Send_Remove_Npc_Packet(playerId, au - MAX_USER);
			}
		}
	}
}