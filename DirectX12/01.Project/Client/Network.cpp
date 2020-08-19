#include "stdafx.h"
#include "Network.h"
#include "LoginScene.h"
#include "IngameScene.h"

#include <Engine/Device.h>

#include <Engine/Ptr.h>

#include <Engine/Texture.h>

#include <Engine/ResMgr.h>
#include <Engine/CollisionMgr.h>
#include <Engine/FontMgr.h>

#include <Engine/GameObject.h>
#include <Engine/MeshRender.h>
#include <Engine/Transform.h>
#include <Engine/Camera.h>
#include <Engine/Light3D.h>
#include <Engine/LandScape.h>
#include <Engine/GridScript.h>

#include <Engine/PlayerScript.h>
#include <Engine/FPSCamScript.h>
#include <Engine/ToolCamScript.h>
#include <Engine/MonsterScript.h>
#include <Engine/StatusScript.h>
#include <Engine/QuickSlotScript.h>

#include "PlayerCamScript.h"
#include "InventoryScript.h"
#include "ChatScript.h"
#include <Engine/TestScript.h>
#include "InputScript.h"
#include "AnimalScript.h"

#include <Engine/NaviMgr.h>
#include <Engine/Layer.h>

unsigned int CNetwork::m_usID = 0;
concurrent_unordered_map<unsigned int, CGameObject*> CNetwork::m_cumPlayer;
concurrent_unordered_map<unsigned int, CGameObject*> CNetwork::m_cumAnimal;

CNetwork::CNetwork()
{
	Init();
}

CNetwork::~CNetwork()
{
	DisConnect();
}

void CNetwork::Err_quit(const char * msg, int err_no)
{
	WCHAR *lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCWSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(-1);
}

void CNetwork::Err_display(const char * msg, int err_no)
{
	WCHAR *lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	cout << "[ " << msg << " ]";
	wcout << L"���� " << lpMsgBuf << endl;
	while (true);
	LocalFree(lpMsgBuf);
}

void CNetwork::Init()
{
	for (auto i = 0; i < LT_END; ++i)
		m_bLoginState[i] = false;
	m_bPushKey = false;
	m_bClientClose = false;
	m_bCollision = false;

	m_SendWsaBuf.buf = m_cSendBuf;
	m_SendWsaBuf.len = BUF_SIZE;

	m_RecvWsaBuf.buf = m_cRecvbuf;
	m_RecvWsaBuf.len = BUF_SIZE;

	m_in_packet_size = 0;
	m_saved_packet_size = 0;
}

bool CNetwork::ConnectServer(string ipAddr)
{
	char IP[15];
	strcpy_s(IP, ipAddr.c_str());

	WSADATA	wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	m_sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(SOCKADDR_IN));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVER_PORT);
	serveraddr.sin_addr.s_addr = inet_addr(IP);
	int retval = WSAConnect(m_sock, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr), NULL, NULL, NULL, NULL);
	if (retval == SOCKET_ERROR) {
		int err_no = WSAGetLastError();
		//Err_display("socket err", err_no);
		return false;
	}

	// IP �ּҷ� ����Ǿ��� ���
	if (CreateEventSelect()) {
		RunRecvThread();
		return true;
	}
	else
		return false;
}

void CNetwork::RunRecvThread()
{
	// Network Run
	m_tNetworkThread = make_shared<thread>([]() {CNetwork::GetInst()->RecvPacket(); });

}

bool CNetwork::CreateEventSelect()
{
	m_hEvent = WSACreateEvent();

	int retval = WSAEventSelect(m_sock, m_hEvent, FD_READ || FD_CLOSE);
	if (retval != SOCKET_ERROR) {
		cout << "CreateSelectEvent Success" << endl;
		return true;
	}
	else {
		int err_no = WSAGetLastError();
		Err_display("CreateSelectEvent err", err_no);
		return false;
	}
}

void CNetwork::DisConnect()
{
	m_tNetworkThread->join();

	closesocket(m_sock);
	WSACloseEvent(m_hEvent);
	WSACleanup();
	cout << "DisConnect" << endl;
}

void CNetwork::RecvPacket()
{
	cout << "Network Run" << endl;
	while (!m_bClientClose)
	{
		m_iIndex = WSAWaitForMultipleEvents(1, &m_hEvent, FALSE, WSA_INFINITE, FALSE);

		if ((m_iIndex == WSA_WAIT_FAILED) || (m_iIndex == WSA_WAIT_TIMEOUT)) {
			int err_no = WSAGetLastError();
			Err_display("WSAWaitForMultipleEvents Err", err_no);
			break;
		}
		int retval = WSAEnumNetworkEvents(m_sock, m_hEvent, &m_weEvent);
		if (retval == SOCKET_ERROR) {
			int err_no = WSAGetLastError();
			Err_display("WSAEnumNetworkEvents Err", err_no);
			break;
		}

		if (m_weEvent.lNetworkEvents & FD_READ) {
			if (m_weEvent.iErrorCode[FD_READ_BIT] != 0) {
				int err_no = GetLastError();
				Err_display("m_weEvent.iErrorCode Err", err_no);
				break;
			}
			DWORD	size, flag = 0;
			int ret = WSARecv(m_sock, &m_RecvWsaBuf, 1, &size, &flag, NULL, NULL);
			if (ret == SOCKET_ERROR) {
				int err_no = GetLastError();
				Err_display("WSARecv SOCKET_ERROR", err_no);
				break;
			}

			BYTE* packet = reinterpret_cast<BYTE*>(m_cRecvbuf);

			while (size != 0) {
				if (0 == m_in_packet_size) m_in_packet_size = packet[0];
				if (size + m_saved_packet_size >= m_in_packet_size) {
					memcpy(m_cPacketBuf + m_saved_packet_size, packet, m_in_packet_size - m_saved_packet_size);
					ProcessPacket(m_cPacketBuf);
					packet += m_in_packet_size - m_saved_packet_size;
					size -= m_in_packet_size - m_saved_packet_size;
					m_in_packet_size = 0;
					m_saved_packet_size = 0;
				}
				else {
					memcpy(m_cPacketBuf + m_saved_packet_size, packet, size);
					m_saved_packet_size += size;
					size = 0;
				}
			}
		}
		if (m_weEvent.lNetworkEvents & FD_CLOSE)
		{
			cout << "FD_CLOSE" << endl;
			m_bClientClose = true;
			break;
		}
	}
	//cout << "Network Close" << endl;
}

void CNetwork::ProcessPacket(char* packet)
{
	switch (packet[1])
	{
	case SC_LOGIN_OK:
	{
		SetLogin(true);
		Recv_Login_OK_Packet(packet);
		break;
	}
	case SC_LOGIN_FAIL:
	{
		break;
	}
	case SC_STATUS_PLAYER:
	{
		Recv_Status_Player_Packet(packet);
		break;
	}
	case SC_PUT_PLAYER: {
		SetLoopStart(true);
		Recv_Put_Player_Packet(packet);
		break;
	}
	case SC_REMOVE_PLAYER: {
		Recv_Remove_Player_Packet(packet);
		break;
	}
	case SC_POS: {
		Recv_Pos_Player_Packet(packet);
		break;
	}
	case SC_ROT: {
		break;
	}
	case SC_CHAT: {
		Recv_Chat_Packet(packet);
		break;
	}
	// NPC, MONSTER ����
	case SC_PUT_NPC: {
		Recv_Put_Npc_Packet(packet);
		break;
	}
	case SC_POS_NPC:
	{
		Recv_Pos_Npc_Packet(packet);
		break;
	}
	case SC_REMOVE_NPC:
	{
		Recv_Remove_Npc_Packet(packet);
		break;
	}
	// Natural
	case SC_PUT_NATURAL:
	{
		Recv_Put_Natural_Packet(packet);
		break;
	}
	case SC_DESTROY_NATURAL:
	{
		Recv_Destroy_Natural_Packet(packet);
		break;
	}
	// Housing
	case SC_INSTALL_HOUSING:
	{
		Recv_Install_Housing_Packet(packet);
		break;
	}
	
	// etc
	case SC_WEATHER:
	{
		Recv_Weather_Packet(packet);
		break;
	}
	case SC_TIME:
	{
		Recv_Time_Packet(packet);
		break;
	}
	}
}

void CNetwork::Send_Login_Packet(wstring playerID)
{
	DWORD size{ 0 }, flag{ 0 };

	cs_login_packet*	login_packet = reinterpret_cast<cs_login_packet*>(m_cSendBuf);

	login_packet->size = sizeof(cs_login_packet);
	login_packet->type = CS_LOGIN;
	wcscpy_s(login_packet->player_id, playerID.c_str());

	m_SendWsaBuf.len = sizeof(cs_login_packet);

	int ret = WSASend(m_sock, &m_SendWsaBuf, 1, &size, flag, NULL, NULL);

	if (ret != 0) {
		int err_no = WSAGetLastError();
		Err_display("Err while sending packet - ", err_no);
	}
}

void CNetwork::Send_Move_Packet()
{
	DWORD size, flag = 0;

	cs_move_packet* packet = reinterpret_cast<cs_move_packet*>(m_cSendBuf);

	//Vec3 vRot = m_pPlayer->Transform()->GetLocalRot();
	packet->size = sizeof(cs_move_packet);
	packet->type = CS_MOVE;
	packet->id = m_usID;
	packet->vLocalPos = m_pPlayer->Transform()->GetLocalPos();
	//packet->vLocalRot = m_pPlayer->Transform()->GetLocalRot();
	//packet->bRun = bRun;
	//packet->vWorldDir = vWorldDir;
	//packet->fHeight = CNaviMgr::GetInst()->GetY(m_pPlayer->Transform()->GetLocalPos());

	m_SendWsaBuf.len = sizeof(cs_move_packet);

	int retval = WSASend(m_sock, &m_SendWsaBuf, 1, &size, flag, NULL, NULL);

	if (retval != 0)
	{
		int err_no = WSAGetLastError();
		Err_display("Err while sending packet - ", err_no);
	}
}

//void CNetwork::Send_Move_Packet(Vec3 vWorldDir, bool bRun)
//{
//	DWORD size, flag = 0;
//
//	cs_move_packet* packet = reinterpret_cast<cs_move_packet*>(m_cSendBuf);
//
//	//Vec3 vRot = m_pPlayer->Transform()->GetLocalRot();
//	packet->size = sizeof(cs_move_packet);
//	packet->type = CS_MOVE;
//	packet->id = m_usID;
//	packet->bRun = bRun;
//	packet->vWorldDir = vWorldDir;
//	packet->fHeight = CNaviMgr::GetInst()->GetY(m_pPlayer->Transform()->GetLocalPos());
//
//	m_SendWsaBuf.len = sizeof(cs_move_packet);
//
//	int retval = WSASend(m_sock, &m_SendWsaBuf, 1, &size, flag, NULL, NULL);
//
//	if (retval != 0)
//	{
//		int err_no = WSAGetLastError();
//		Err_display("Err while sending packet - ", err_no);
//	}
//}

void CNetwork::Send_Chat_Packet(string message)
{
	DWORD size, flag = 0;

	cs_chat_packet* chat_packet = reinterpret_cast<cs_chat_packet*>(m_cSendBuf);

	chat_packet->size = sizeof(cs_chat_packet);
	chat_packet->type = CS_CHAT;
	chat_packet->id = m_usID;
	strcpy_s(chat_packet->meesage, message.c_str());

	m_SendWsaBuf.len = sizeof(cs_chat_packet);

	int retval = WSASend(m_sock, &m_SendWsaBuf, 1, &size, flag, NULL, NULL);

	if (retval != 0)
	{
		int err_no = WSAGetLastError();
		Err_display("Err while sending packet - ", err_no);
	}
}

void CNetwork::Send_Collision_Player_Packet(UINT Collision_type, USHORT Collision_Id, bool bRun)
{
	DWORD	size, flag = 0;

	cs_collision_packet* collision_packet = reinterpret_cast<cs_collision_packet*>(m_cSendBuf);

	collision_packet->type = CS_COLLISION;
	collision_packet->collision_uitype = Collision_type;
	collision_packet->collision_id = Collision_Id;
	collision_packet->bRun = bRun;

	int retval = WSASend(m_sock, &m_SendWsaBuf, 1, &size, flag, NULL, NULL);

	if (retval != 0)
	{
		int err_no = WSAGetLastError();
		Err_display("Err while sending packet - ", err_no);
	}
}

void CNetwork::Send_Attack_Player_Packet(UINT attack_type, USHORT attack_Id)
{
	DWORD	size, flag = 0;

	cs_attack_packet* attack_packet = reinterpret_cast<cs_attack_packet*>(m_cSendBuf);

	attack_packet->type = CS_ATTACK;
	attack_packet->attack_uiType = attack_type;
	attack_packet->attack_id = attack_Id;

	int retval = WSASend(m_sock, &m_SendWsaBuf, 1, &size, flag, NULL, NULL);

	if (retval != 0)
	{
		int err_no = WSAGetLastError();
		Err_display("Err while sending packet - ", err_no);
	}
}

void CNetwork::Send_Install_Housing_Packet(UINT uiType, Vec3 vLocalPos, Vec3 vLocalRot, Vec3 vLocalScale, Vec3 vOffsetPos, Vec3 vOffsetScale)
{
	DWORD size, flag = 0;

	cs_install_housing_packet* install_housing_packet = reinterpret_cast<cs_install_housing_packet*>(m_cSendBuf);
	install_housing_packet->type = CS_HOUSING_INSTALL;
	install_housing_packet->housing_type = uiType;
	install_housing_packet->vLocalPos = vLocalPos;
	install_housing_packet->vLocalRot = vLocalRot;
	install_housing_packet->vLocalScale = vLocalScale;
	install_housing_packet->vOffsetPos = vOffsetPos;
	install_housing_packet->vOffsetScale = vOffsetScale;

	int retval = WSASend(m_sock, &m_SendWsaBuf, 1, &size, flag, NULL, NULL);

	if (retval != 0)
	{
		int err_no = WSAGetLastError();
		Err_display("Err while sending packet - ", err_no);
	}
}

void CNetwork::Send_Remove_Housing_Packet(USHORT house_id)
{
	DWORD size, flag = 0;

	cs_remove_housing_packet* remove_housing_packet = reinterpret_cast<cs_remove_housing_packet*>(m_cSendBuf);
	remove_housing_packet->type = CS_HOUSING_REMOVE;
	remove_housing_packet->house_id = house_id;

	int retval = WSASend(m_sock, &m_SendWsaBuf, 1, &size, flag, NULL, NULL);

	if (retval != 0)
	{
		int err_no = WSAGetLastError();
		Err_display("Err while sending packet - ", err_no);
	}
}

void CNetwork::Send_Get_Item_Packet(UINT uiItemType, UINT uiInvenNum)
{
	DWORD size, flag = 0;

	cs_item_get_packet* item_get_packet = reinterpret_cast<cs_item_get_packet*>(m_cSendBuf);
	item_get_packet->type = CS_GET_ITEM;
	item_get_packet->uiType = uiItemType;
	item_get_packet->uiInvenNum = uiInvenNum;

	int retval = WSASend(m_sock, &m_SendWsaBuf, 1, &size, flag, NULL, NULL);

	if (retval != 0)
	{
		int err_no = WSAGetLastError();
		Err_display("Err while sending packet - ", err_no);
	}
}

void CNetwork::Send_Remove_Item_Packet(UINT uiItemType, UINT uiInvenNum)
{
	DWORD size, flag = 0;

	cs_item_remove_packet* item_remove_packet = reinterpret_cast<cs_item_remove_packet*>(m_cSendBuf);
	item_remove_packet->type = CS_GET_ITEM;
	item_remove_packet->uiType = uiItemType;
	item_remove_packet->uiInvenNum = uiInvenNum;

	int retval = WSASend(m_sock, &m_SendWsaBuf, 1, &size, flag, NULL, NULL);

	if (retval != 0)
	{
		int err_no = WSAGetLastError();
		Err_display("Err while sending packet - ", err_no);
	}
}

// ============================== RECV ============================
void CNetwork::Recv_Login_OK_Packet(char * packet)
{
	pScene = CSceneMgr::GetInst()->GetCurScene();
	sc_login_ok_packet* login_packet = reinterpret_cast<sc_login_ok_packet*>(packet);
	m_usID = login_packet->id;
	//pScene->FindLayer(L"Player")->AddGameObject(m_pPlayer);
}

void CNetwork::Recv_Login_Fail_Packet(char * packet)
{
	SetLogin(false);
}

void CNetwork::Recv_Status_Player_Packet(char * packet)
{
	sc_status_player_packet* status_player_packet = reinterpret_cast<sc_status_player_packet*>(packet);
	USHORT player_id = status_player_packet->id;
	float fHealth = status_player_packet->fHealth;
	float fHungry = status_player_packet->fHungry;
	float fThrist = status_player_packet->fThrist;

	if (m_usID == player_id)
	{
		// �߰�
	}
	else
	{
		return;
	}
}

void CNetwork::Recv_Put_Player_Packet(char * packet)
{
	sc_put_player_packet* put_player_packet = reinterpret_cast<sc_put_player_packet*>(packet);
	unsigned int player_id = put_player_packet->id;

	Vec3 vPos = put_player_packet->vPos;
	Vec3 vRot = put_player_packet->vRot;

	if (m_usID == player_id) {
		m_pPlayer->Transform()->SetLocalPos(vPos);
	}
	else {
		m_cumPlayer[player_id]->Transform()->SetLocalPos(vPos);
		m_cumPlayer[player_id]->Transform()->SetLocalRot(vRot);
		pScene->FindLayer(L"Human")->AddGameObject(CNetwork::m_cumPlayer[player_id]);
	}
}

void CNetwork::Recv_Remove_Player_Packet(char * packet)
{
	sc_remove_player_packet* remove_player_packet = reinterpret_cast<sc_remove_player_packet*>(packet);
	unsigned int player_id = remove_player_packet->id;

	tEvent tEv;
	tEv.eType = EVENT_TYPE::DELETE_OBJECT;

	if (m_usID == player_id) {
		tEv.wParam = (DWORD_PTR)m_pPlayer;
		CEventMgr::GetInst()->AddEvent(tEv);
	}
	else {
		tEv.wParam = (DWORD_PTR)m_cumPlayer[player_id];
		CEventMgr::GetInst()->AddEvent(tEv);
	}
}

void CNetwork::Recv_Pos_Player_Packet(char * packet)
{
	sc_pos_player_packet*	pos_packet = reinterpret_cast<sc_pos_player_packet*>(packet);
	unsigned int player_id = pos_packet->id;
	Vec3 vPos = pos_packet->vPos;
	Vec3 vRot = pos_packet->vRot;
	if (m_usID == player_id) {
		m_pPlayer->Transform()->SetLocalPos(vPos);
		//m_pPlayer->Transform()->SetLocalRot(vRot);
	}
	else {
		m_cumPlayer[player_id]->Transform()->SetLocalPos(vPos);
		m_cumPlayer[player_id]->Transform()->SetLocalRot(vRot);
	}
}

void CNetwork::Recv_Chat_Packet(char * packet)
{
	sc_chat_packet* chat_packet = reinterpret_cast<sc_chat_packet*>(packet);

	wstring wname = chat_packet->wcid;
	string name;
	name.assign(wname.begin(), wname.end());
	string Msg(chat_packet->meesage);
	cout << "[" << name << "]" << Msg << endl;
	m_pChat->GetScript<CChatScript>()->AddChat(name, Msg);
	m_pChat->GetScript<CInputScript>()->SetEnable(false);
	m_pChat->GetScript<CInputScript>()->Clear();
}

void CNetwork::Recv_Animation_Player_Packet(char * packet)
{
	sc_animation_player_packet* animation_player_packet = reinterpret_cast<sc_animation_player_packet*>(packet);
	USHORT player_id = animation_player_packet->id;
	UINT uiType = animation_player_packet->animation_uiType;

	if (m_usID == player_id)
	{
		if (uiType == (UINT)PLAYER_ANIMATION_TYPE::WALK) {

		}
		else if (uiType == (UINT)PLAYER_ANIMATION_TYPE::RUN) {

		}
		else if (uiType == (UINT)PLAYER_ANIMATION_TYPE::IDLE1) {

		}
		else if (uiType == (UINT)PLAYER_ANIMATION_TYPE::IDLE2) {

		}
		else if (uiType == (UINT)PLAYER_ANIMATION_TYPE::DIE) {

		}
		else if (uiType == (UINT)PLAYER_ANIMATION_TYPE::TAKE_WEAPON) {

		}
		else if (uiType == (UINT)PLAYER_ANIMATION_TYPE::ATTACK1) {

		}
		else if (uiType == (UINT)PLAYER_ANIMATION_TYPE::ATTACK2) {

		}
		else if (uiType == (UINT)PLAYER_ANIMATION_TYPE::ATTACK3) {

		}
		else if (uiType == (UINT)PLAYER_ANIMATION_TYPE::HIT1) {

		}
		else if (uiType == (UINT)PLAYER_ANIMATION_TYPE::HIT2) {

		}
		else if (uiType == (UINT)PLAYER_ANIMATION_TYPE::JUMP) {

		}
		else
			return;
	}
	else
	{
		if (uiType == (UINT)PLAYER_ANIMATION_TYPE::WALK) {

		}
		else if (uiType == (UINT)PLAYER_ANIMATION_TYPE::RUN) {

		}
		else if (uiType == (UINT)PLAYER_ANIMATION_TYPE::IDLE1) {

		}
		else if (uiType == (UINT)PLAYER_ANIMATION_TYPE::IDLE2) {

		}
		else if (uiType == (UINT)PLAYER_ANIMATION_TYPE::DIE) {

		}
		else if (uiType == (UINT)PLAYER_ANIMATION_TYPE::TAKE_WEAPON) {

		}
		else if (uiType == (UINT)PLAYER_ANIMATION_TYPE::ATTACK1) {

		}
		else if (uiType == (UINT)PLAYER_ANIMATION_TYPE::ATTACK2) {

		}
		else if (uiType == (UINT)PLAYER_ANIMATION_TYPE::ATTACK3) {

		}
		else if (uiType == (UINT)PLAYER_ANIMATION_TYPE::HIT1) {

		}
		else if (uiType == (UINT)PLAYER_ANIMATION_TYPE::HIT2) {

		}
		else if (uiType == (UINT)PLAYER_ANIMATION_TYPE::JUMP) {

		}
		else
			return;
	}
}

void CNetwork::Recv_WakeUp_Npc_Packet(char * packet)
{
	sc_wake_up_packet* wakeup_npc_packet = reinterpret_cast<sc_wake_up_packet*>(packet);

	unsigned int monster_id = wakeup_npc_packet->id;

	m_cumAnimal[monster_id]->GetScript<CAnimalScript>()->SetWakeUp(true);
}

void CNetwork::Recv_Put_Npc_Packet(char * packet)
{
	sc_put_npc_packet* put_npc_packet = reinterpret_cast<sc_put_npc_packet*>(packet);
	USHORT monster_id = put_npc_packet->id;

	UINT eType = put_npc_packet->eType;
	Vec3 vPos = put_npc_packet->vPos;
	Vec3 vRot = put_npc_packet->vRot;

	dynamic_cast<CIngameScene*>(pScene->GetSceneScript())->AnimalUpdate(monster_id, vPos, vRot, eType);
}

void CNetwork::Recv_Remove_Npc_Packet(char * packet)
{
	sc_remove_npc_packet* remove_npc_packet = reinterpret_cast<sc_remove_npc_packet*>(packet);
	USHORT monster_id = remove_npc_packet->id;

	dynamic_cast<CIngameScene*>(pScene->GetSceneScript())->AnimalDestory(monster_id);
}

void CNetwork::Recv_Pos_Npc_Packet(char * packet)
{
	sc_pos_npc_packet* pos_npc_packet = reinterpret_cast<sc_pos_npc_packet*>(packet);
	unsigned int monster_id = pos_npc_packet->id;
	UINT eType = pos_npc_packet->eType;
	Vec3 vPos = pos_npc_packet->vPos;
	Vec3 vRot = pos_npc_packet->vRot;

	dynamic_cast<CIngameScene*>(pScene->GetSceneScript())->AnimalUpdate(monster_id, vPos, vRot, eType);
}

void CNetwork::Recv_Animation_Npc_Packet(char * packet)
{
	sc_animation_npc_packet* animation_npc_packet = reinterpret_cast<sc_animation_npc_packet*>(packet);
	unsigned int monster_id = animation_npc_packet->id;
	UINT		uiType = animation_npc_packet->animation;

	// ���� �ִϸ��̼� Ű�� �ٲٱ�
	if (uiType == (UINT)ANIMAL_ANIMATION_TYPE::WALK) {

	}
	else if (uiType == (UINT)ANIMAL_ANIMATION_TYPE::RUN) {

	}
	else if (uiType == (UINT)ANIMAL_ANIMATION_TYPE::IDLE) {

	}
	else if (uiType == (UINT)ANIMAL_ANIMATION_TYPE::EAT) {

	}
	else if (uiType == (UINT)ANIMAL_ANIMATION_TYPE::DIE) {

	}
	else if (uiType == (UINT)ANIMAL_ANIMATION_TYPE::ATTACK) {

	}
	else
		return;
}

void CNetwork::Recv_Put_Natural_Packet(char * packet)
{
	sc_put_natural_packet* put_natural_packet = reinterpret_cast<sc_put_natural_packet*>(packet);
	float fHealth = put_natural_packet->fHealth;
	bool bDestroy = put_natural_packet->bDestroy;
	USHORT natural_id = put_natural_packet->natural_id;
	Vec3 vLocalPos = put_natural_packet->vLocalPos;
	Vec3 vLocalScale = put_natural_packet->vLocalScale;
	Vec3 vOffsetPos = put_natural_packet->vOffsetPos;
	Vec3 vOffsetScale = put_natural_packet->vOffsetScale;

	// �߰�
}

void CNetwork::Recv_Destroy_Natural_Packet(char * packet)
{
	sc_destroy_natural_packet* destroy_natural_packet = reinterpret_cast<sc_destroy_natural_packet*>(packet);
	USHORT natural_id = destroy_natural_packet->natural_id;

	// ����
}

void CNetwork::Recv_Install_Housing_Packet(char * packet)
{
	sc_install_housing_packet* install_housing_packet = reinterpret_cast<sc_install_housing_packet*>(packet);
	USHORT house_id = install_housing_packet->house_id;
	UINT type = install_housing_packet->housing_type;
	Vec3 vPos = install_housing_packet->vLocalPos;
	Vec3 vRot = install_housing_packet->vLocalRot;
	Vec3 vScale = install_housing_packet->vLocalScale;

	// INSTALL ����
	dynamic_cast<CIngameScene*>(pScene->GetSceneScript())->InstallHousing(type, vPos, vRot, vScale);
}

void CNetwork::Recv_Remove_Housing_Packet(char * packet)
{
	sc_remove_housing_packet* remove_housing_packet = reinterpret_cast<sc_remove_housing_packet*>(packet);
	USHORT house_id = remove_housing_packet->house_id;

	// REMOVE 
}

void CNetwork::Recv_Add_Item_Packet(char * packet)
{
	sc_add_item_packet* add_item_packet = reinterpret_cast<sc_add_item_packet*>(packet);
	UINT uiType = add_item_packet->uiItemType;
	UINT uiInvenNum = add_item_packet->uiInven_num;
}

void CNetwork::Recv_Weather_Packet(char * packet)
{
	sc_weather_packet* weather_packet = reinterpret_cast<sc_weather_packet*>(packet);
	bool bRain = weather_packet->bRain;

	// �ޱ�
	//cout << bRain << endl;

	CGameObject* pRainObj = pScene->GetLayer( 0 )->FindObject( L"Rain" );
	if ( pRainObj )
		pRainObj->SetActive( bRain );
}

void CNetwork::Recv_Time_Packet(char * packet)
{
	sc_time_packet* time_packet = reinterpret_cast<sc_time_packet*>(packet);
	float fTime = time_packet->fTime;

	cout << fTime << endl;
}
