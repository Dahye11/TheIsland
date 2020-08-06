#include "MonsterProcess.h"
#include "PacketMgr.h"
#include "TimerMgr.h"
#include <DirectXMath.h>

CMonsterProcess::CMonsterProcess()
{
	if (!CProcess::m_pMonsterPool)
		CProcess::m_pMonsterPool = new class CMonsterpool();
}


CMonsterProcess::~CMonsterProcess()
{
	if (m_pMonsterPool)
	{
		delete m_pMonsterPool;
		m_pMonsterPool = nullptr;
	}
}

void CMonsterProcess::AttackEvent(unsigned short uiMonster, unsigned short uiTarget)
{
	if (CProcess::m_pMonsterPool->m_cumMonsterPool[uiMonster]->GetState() == OBJ_STATE_DIE) return;

	concurrent_unordered_set<unsigned short> login_list;
	concurrent_unordered_set<unsigned short> range_list;

	CopyBeforeLoginList(login_list);

	InRangePlayer(login_list, range_list, uiMonster);

	// Ÿ���� ������
	if (uiTarget == NO_TARGET) {
		m_pMonsterPool->m_cumMonsterPool[uiMonster]->SetState(OBJ_STATE_IDLE);
		m_pMonsterPool->m_cumMonsterPool[uiMonster]->SetTarget(NO_TARGET);

		Update_Event ev;
		ev.m_EventType = EVENT_TYPE::EV_MONSTER_UPDATE;
		ev.m_ObjState = OBJ_STATE_TYPE::OBJ_STATE_IDLE;
		ev.m_Do_Object = uiMonster;
		ev.m_From_Object = NO_TARGET;
		ev.wakeup_time = high_resolution_clock::now() + 1s;
		CProcess::PushEventQueue(ev);
	}
	else // Ÿ���� ������
	{
		float player_HP = m_pPlayerPool->m_cumPlayerPool[uiTarget]->GetHealth();
		Vec3 player_pos = m_pPlayerPool->m_cumPlayerPool[uiTarget]->GetLocalPos();
		float player_AfterHP = player_HP;

		Vec3 monster_pos = m_pMonsterPool->m_cumMonsterPool[uiMonster]->GetLocalPos();
		float monster_Damage = m_pMonsterPool->m_cumMonsterPool[uiMonster]->GetDamage();
		// �÷��̾�� �������� ������.
		player_AfterHP -= monster_Damage;
		
		// �÷��̾� ü���� 0�� �Ǿ�����
		if (player_AfterHP <= 0) 
		{
			// �÷��̾� ó�� (�÷��̾� ���)
			// �÷��̾� ��� ��Ŷ ������
			CPacketMgr::GetInst()->Send_Death_Player_Packet(uiTarget);

			// ANIMAL IDLE ���·�
			m_pMonsterPool->m_cumMonsterPool[uiMonster]->SetState(OBJ_STATE_IDLE);
			m_pMonsterPool->m_cumMonsterPool[uiMonster]->SetTarget(NO_TARGET);
			Update_Event ev;
			ev.m_EventType = EVENT_TYPE::EV_MONSTER_UPDATE;
			ev.m_ObjState = OBJ_STATE_TYPE::OBJ_STATE_IDLE;
			ev.m_Do_Object = uiMonster;
			ev.m_From_Object = NO_TARGET;
			ev.wakeup_time = high_resolution_clock::now() + 1s;
			CProcess::PushEventQueue(ev);

			//
			for (auto& au : range_list) {
				bool isConnect = m_pPlayerPool->m_cumPlayerPool[au]->GetConnect();
				if (!isConnect) continue;
				// Player Status ��Ŷ ������
				// Animation - �÷��̾ ������
			}
		}
		else { // �÷��̾� ü���� 0�� �ȵǾ��� ���
			m_pPlayerPool->m_cumPlayerPool[uiTarget]->SetHealth(player_AfterHP);
			// �÷��̾� ��Ŷ ������

			for (auto& au : range_list) {
				bool isConnect = CProcess::m_pPlayerPool->m_cumPlayerPool[au]->GetConnect();
				if (!isConnect) continue;
				// 
			}
		}
	}
}

void CMonsterProcess::FollowEvent(unsigned short uiID, unsigned short uiTarget)
{
	if (m_pMonsterPool->m_cumMonsterPool[uiID]->GetState() == OBJ_STATE_DIE) return;

	concurrent_unordered_set<unsigned short> before_loginList;
	concurrent_unordered_set<unsigned short> after_loginList;
	concurrent_unordered_set<unsigned short> before_rangeList;
	concurrent_unordered_set<unsigned short> after_rangeList;

	unsigned short monster_id = uiID + MAX_USER; // Player Login List�� �������� MAX_USER�� + ���ش�.
	
	CopyBeforeLoginList(before_loginList);
	InRangePlayer(before_loginList, before_rangeList, uiID);
	
	// ���� Ÿ���� ���ų� �׾��ٸ�
	if (uiTarget == NO_TARGET || (uiTarget != NO_TARGET && m_pPlayerPool->m_cumPlayerPool[uiTarget]->GetState() == OBJ_STATE_DIE))
	{
		m_pMonsterPool->m_cumMonsterPool[uiID]->SetState(OBJ_STATE_IDLE);
		m_pMonsterPool->m_cumMonsterPool[uiID]->SetTarget(NO_TARGET);
		Update_Event ev;
		ev.m_Do_Object = uiID;
		ev.m_EventType = EV_MONSTER_UPDATE;
		ev.m_From_Object = NO_TARGET;
		ev.m_ObjState = OBJ_STATE_IDLE;
		ev.wakeup_time = high_resolution_clock::now() + 16ms;
		PushEventQueue(ev);
		return;
	}
	// Ÿ���� �����Ѵٸ�
	// Monster Follow ���
	Vec3 target_pos = m_pPlayerPool->m_cumPlayerPool[uiTarget]->GetLocalPos();
	Vec3 monster_pos = m_pMonsterPool->m_cumMonsterPool[uiID]->GetLocalPos();
	float monster_speed = m_pMonsterPool->m_cumMonsterPool[uiID]->GetSpeed();

	Vec3 vDir = XMVector3Normalize(target_pos - monster_pos);
	vDir.y = 0.f;

	monster_pos = vDir * monster_speed * 0.02f;

	Vec3 vRot = m_pPlayerPool->m_cumPlayerPool[uiTarget]->GetLocalRot();

	m_pMonsterPool->m_cumMonsterPool[uiID]->SetLocalRot(Vec3(-3.141592654f / 2.f, atan2(vDir.x, vDir.z) + 3.141592f, 0.f));
	m_pMonsterPool->m_cumMonsterPool[uiID]->SetLocalPos(monster_pos);

	CopyBeforeLoginList(after_loginList);
	InRangePlayer(after_loginList, after_rangeList, uiID);

	for (auto& au : before_rangeList)
	{
		if (0 == after_rangeList.count(au)) // after�� ������
		{
			bool bConnect = m_pPlayerPool->m_cumPlayerPool[au]->GetConnect();
			if (!bConnect) continue;
			CPacketMgr::GetInst()->Send_Remove_Npc_Packet(au, uiID);
		}
		else
		{
			CPacketMgr::GetInst()->Send_Pos_Npc_Packet(au, uiID);
		}
	}

	if (m_pPlayerPool->m_cumPlayerPool[uiTarget]->GetState() == OBJ_STATE_DIE)
	{
		m_pMonsterPool->m_cumMonsterPool[uiID]->SetState(OBJ_STATE_IDLE);
		m_pMonsterPool->m_cumMonsterPool[uiID]->SetTarget(NO_TARGET);
		//
		Update_Event ev;
		ev.m_Do_Object = uiID;
		ev.m_EventType = EV_MONSTER_UPDATE;
		ev.m_From_Object = NO_TARGET;
		ev.m_ObjState = OBJ_STATE_IDLE;
		ev.wakeup_time = high_resolution_clock::now() + 1s;
		PushEventQueue(ev);
	}
	else
	{
		if (ObjectRangeCheck(monster_pos, target_pos, 100.f)) // ���� ����
		{
			m_pMonsterPool->m_cumMonsterPool[uiID]->SetState(OBJ_STATE_ATTACK);
			Update_Event ev;
			ev.m_Do_Object = uiID;
			ev.m_EventType = EV_MONSTER_UPDATE;
			ev.m_From_Object = uiTarget;
			ev.m_ObjState = OBJ_STATE_ATTACK;
			ev.wakeup_time = high_resolution_clock::now() + 4s;
			PushEventQueue(ev);
		}
		else if (ObjectRangeCheck(monster_pos, target_pos, MONSTER_BETWEEN_RANGE))
		{
			m_pMonsterPool->m_cumMonsterPool[uiID]->SetState(OBJ_STATE_FOLLOW);
			Update_Event ev;
			ev.m_Do_Object = uiID;
			ev.m_EventType = EV_MONSTER_UPDATE;
			ev.m_From_Object = uiTarget;
			ev.m_ObjState = OBJ_STATE_FOLLOW;
			ev.wakeup_time = high_resolution_clock::now() + 16ms;
			PushEventQueue(ev);
		}
		else 
		{
			m_pMonsterPool->m_cumMonsterPool[uiID]->SetState(OBJ_STATE_IDLE);
			m_pMonsterPool->m_cumMonsterPool[uiID]->SetTarget(NO_TARGET);
			Update_Event ev;
			ev.m_Do_Object = uiID;
			ev.m_EventType = EV_MONSTER_UPDATE;
			ev.m_From_Object = NO_TARGET;
			ev.m_ObjState = OBJ_STATE_IDLE;
			ev.wakeup_time = high_resolution_clock::now() + 16ms;
			PushEventQueue(ev);
		}
	}
}

void CMonsterProcess::EvastionEvent(unsigned short uiMonster, unsigned short uiTarget)
{
	concurrent_unordered_set<unsigned short> login_list;
	concurrent_unordered_set<unsigned short> range_list;

	CopyBeforeLoginList(login_list);
	InRangePlayer(login_list, range_list, uiMonster);

	Vec3 monster_pos = m_pMonsterPool->m_cumMonsterPool[uiMonster]->GetLocalPos();
	Vec3 monster_dir = m_pMonsterPool->m_cumMonsterPool[uiMonster]->GetDir();
	float monster_speed = m_pMonsterPool->m_cumMonsterPool[uiMonster]->GetSpeed();
	monster_pos += monster_dir * monster_speed * 0.02f;
	m_pMonsterPool->m_cumMonsterPool[uiMonster]->SetLocalPos(monster_pos);

	for (auto& au : range_list)
	{
		bool bConnect = m_pPlayerPool->m_cumPlayerPool[au]->GetConnect();
		if (!bConnect) continue;
		CPacketMgr::GetInst()->Send_Pos_Npc_Packet(au, uiMonster);
	}
}

void CMonsterProcess::IdleEvent(unsigned short monsterId)
{
	concurrent_unordered_set<unsigned short> login_list;
	concurrent_unordered_set<unsigned short> range_list;

	CopyBeforeLoginList(login_list);
	InRangePlayer(login_list, range_list, monsterId);

	if (range_list.empty()) {
		//CProcess::m_pMonsterPool->m_cumMonsterPool[monsterId]->ResPawn();
		return;
	}

	Vec3 monster_pos = m_pMonsterPool->m_cumMonsterPool[monsterId]->GetLocalPos();
	BEHAVIOR_TYPE animal_type = m_pMonsterPool->m_cumMonsterPool[monsterId]->GetType();

	unsigned short target_id = m_pMonsterPool->m_cumMonsterPool[monsterId]->GetTarget();
	
	for (auto& au : range_list) {
		bool isConnect = m_pPlayerPool->m_cumPlayerPool[au]->GetConnect();
		if (!isConnect)continue;
		//CPacketMgr::Send_Packet(au, &) // IDLE Animation ��Ŷ ������
		Vec3 player_pos = m_pPlayerPool->m_cumPlayerPool[au]->GetLocalPos();
		if (ObjectRangeCheck(player_pos, monster_pos, MONSTER_BETWEEN_RANGE) && target_id == NO_TARGET)
		{
			m_pMonsterPool->m_cumMonsterPool[monsterId]->SetTarget(au);
			target_id = au;
		}
	}

	// Animal�� Target�� ���� ���
	if (NO_TARGET == target_id)
	{
		m_pMonsterPool->m_cumMonsterPool[monsterId]->SetState(OBJ_STATE_IDLE);
		Update_Event ev;
		ev.m_Do_Object = monsterId;
		ev.m_EventType = EV_MONSTER_UPDATE;
		ev.m_ObjState = OBJ_STATE_IDLE;
		ev.m_From_Object = NO_TARGET;
		ev.wakeup_time = high_resolution_clock::now() + 10s;
		PushEventQueue(ev);
	}
	else if(target_id < MAX_USER)
	{
		Update_Event ev;
		ev.m_Do_Object = monsterId;
		ev.m_EventType = EV_MONSTER_UPDATE;
		if (B_WARLIKE == animal_type) // ����
		{
			m_pMonsterPool->m_cumMonsterPool[monsterId]->SetState(OBJ_STATE_FOLLOW);
	
			ev.m_ObjState = OBJ_STATE_FOLLOW;
			ev.m_From_Object = target_id;
			ev.wakeup_time = high_resolution_clock::now() + 1s;
		}
		else if (B_PASSIVE == animal_type) // �񼱰�
		{
			m_pMonsterPool->m_cumMonsterPool[monsterId]->SetState(OBJ_STATE_IDLE);
			
			ev.m_ObjState = OBJ_STATE_IDLE;
			ev.m_From_Object = NO_TARGET;
			ev.wakeup_time = high_resolution_clock::now() + 1s;
		}
		else if (B_EVASION == animal_type) // ȸ��(����)
		{
			m_pMonsterPool->m_cumMonsterPool[monsterId]->SetState(OBJ_STATE_EVASION);
			ev.m_ObjState = OBJ_STATE_EVASION;
			ev.m_From_Object = target_id;
			ev.wakeup_time = high_resolution_clock::now() + 1s;
		}
		PushEventQueue(ev);
	}
}

void CMonsterProcess::DieEvent(unsigned short uiMonster)
{
	concurrent_unordered_set<unsigned short> login_list;
	concurrent_unordered_set<unsigned short> range_list;

	CopyBeforeLoginList(login_list);
	InRangePlayer(login_list, range_list, uiMonster);

	// ���� ���� DIE�� ��ȯ
	m_pMonsterPool->m_cumMonsterPool[uiMonster]->SetState(OBJ_STATE_DIE);
	// ���� ������ 
	Update_Event ev;
	ev.m_Do_Object = uiMonster;
	ev.m_EventType = EV_MONSTER_UPDATE;
	ev.m_From_Object = NO_TARGET;
	ev.m_ObjState = OBJ_STATE_RESPAWN;
	ev.wakeup_time = high_resolution_clock::now() + 30s;
	PushEventQueue(ev);

	for (auto& au : range_list)
	{
		bool bConnect = m_pPlayerPool->m_cumPlayerPool[au]->GetConnect();
		if (!bConnect) continue;
		CPacketMgr::GetInst()->Send_Remove_Npc_Packet(au, uiMonster);
	}
}

void CMonsterProcess::RespawnEvent(unsigned short uiMonster)
{
	concurrent_unordered_set<unsigned short> login_list;
	concurrent_unordered_set<unsigned short> range_list;

	//CProcess::m_pMonsterPool->m_cumMonsterPool[uiMonster]->ResPawn();

	CopyBeforeLoginList(login_list);
	InRangePlayer(login_list, range_list, uiMonster);

	if (range_list.empty())
		return;

	CProcess::m_pMonsterPool->m_cumMonsterPool[uiMonster]->SetWakeUp(true);
	unsigned short monster_id = MAX_USER + uiMonster;

	Vec3 monster_pos = CProcess::m_pMonsterPool->m_cumMonsterPool[uiMonster]->GetLocalPos();

	Update_Event ev;
	ev.m_Do_Object = uiMonster;
	ev.m_From_Object = NO_TARGET;
	ev.m_EventType = EV_MONSTER_UPDATE;
	ev.m_ObjState = OBJ_STATE_IDLE;
	ev.wakeup_time = high_resolution_clock::now() + 1s;

	CProcess::PushEventQueue(ev);

	for (auto& au : range_list) {
		bool isConnect = CProcess::m_pPlayerPool->m_cumPlayerPool[au]->GetConnect();
		if (!isConnect) continue;
		CPacketMgr::GetInst()->Send_Put_Npc_Packet(au, uiMonster);
	}
}

void CMonsterProcess::HealEvent(unsigned short uiMonster)
{
	char monster_state = m_pMonsterPool->m_cumMonsterPool[uiMonster]->GetState();
	if (monster_state == OBJ_STATE_ATTACK || monster_state == OBJ_STATE_FOLLOW || monster_state == OBJ_STATE_FOLLOW) return;

	concurrent_unordered_set<unsigned short> login_list;
	concurrent_unordered_set<unsigned short> range_list;

	unsigned short monster_id = uiMonster + MAX_USER;

	CopyBeforeLoginList(login_list);
	InRangePlayer(login_list, range_list, monster_id);
	float monster_hp = m_pMonsterPool->m_cumMonsterPool[uiMonster]->GetHealth();
	float limit_hp = 100.f;

	if (monster_hp < limit_hp)
	{
		float percent_hp = limit_hp * 0.01f;
		monster_hp += percent_hp;
	}
	else if (monster_hp >= limit_hp)
	{
		monster_hp = limit_hp;
	}
	m_pMonsterPool->m_cumMonsterPool[uiMonster]->SetHealth(monster_hp);

	// ���� ���� �Ѱ�����մϴ�
	if (range_list.empty())
	{
		return;
	}
		//m_pMonsterPool->m_cumMonsterPool[uiMonster]->ResPawn();
	else
	{
		if (monster_hp != limit_hp)
		{
			Update_Event ev;
			ev.m_Do_Object = uiMonster;
			ev.m_EventType = EV_MONSTER_UPDATE;
			ev.m_From_Object = NO_TARGET;
			ev.m_ObjState = OBJ_STATE_HEAL;
			ev.wakeup_time = high_resolution_clock::now() + 1s;
			PushEventQueue(ev);
		}
		for (auto& au : range_list)
		{
			bool bConnect = m_pPlayerPool->m_cumPlayerPool[au]->GetConnect();
			if (!bConnect) continue;
			// Status ��Ŷ ������
		}
	}
}

void CMonsterProcess::InRangePlayer(concurrent_unordered_set<unsigned short>& cusLogin, concurrent_unordered_set<unsigned short>& cusList, unsigned short uiMonster)
{
	Vec3 monster_pos = CProcess::m_pMonsterPool->m_cumMonsterPool[uiMonster]->GetLocalPos();
	for (auto& au : cusLogin) {
		bool isConnect = CProcess::m_pPlayerPool->m_cumPlayerPool[au]->GetConnect();
		if (!isConnect) continue;
		Vec3 player_pos = CProcess::m_pPlayerPool->m_cumPlayerPool[au]->GetLocalPos();
		if(ObjectRangeCheck(player_pos, monster_pos, PLAYER_BETWEEN_RANGE))
			cusList.insert(au);
	}
}
