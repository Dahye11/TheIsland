#include "Process.h"
#include "PacketMgr.h"
#include "TimerMgr.h"
#include "DataBase.h"

int		CProcess::m_Minute = 0;
int		CProcess::m_Hour = 11;
int		CProcess::m_Day = 0;

CObjectpool*	CProcess::m_pObjectPool = nullptr;
USHORT	CProcess::m_houseNum = 0;
concurrent_unordered_set<USHORT> CProcess::m_cusLoginList;
concurrent_priority_queue<Update_Event>	CProcess::m_cpqEventQueue;

CProcess::CProcess()
{
	
}

CProcess::~CProcess()
{
}

void CProcess::Initalize()
{
	m_cusLoginList.clear();
	m_cpqEventQueue.clear();

	if (!m_pObjectPool) {
		m_pObjectPool = new CObjectpool();
	}
}

void CProcess::Release()
{
	m_cusLoginList.clear();
	m_cpqEventQueue.clear();

	if (m_pObjectPool) {
		delete m_pObjectPool;
		m_pObjectPool = nullptr;
	}
}

bool CProcess::ObjectRangeCheck(Vec3 & vFirst, Vec3 & vSecond, float fDistance)
{
	float fDist = powf(vSecond.x - vFirst.x, 2) + powf(vSecond.z - vFirst.z, 2);
	fDist = sqrtf(fDist);
	if (fDistance > fDist)
		return true;
	return false;
}

void CProcess::PushEvent_Animal_Behavior(USHORT AnimalId)
{
	USHORT&	Animal_Index = AnimalId;
	auto& Animal = m_pObjectPool->m_cumAnimalPool[AnimalId];
	
	USHORT Target_Index = Animal->GetTarget();
	auto& Target = m_pObjectPool->m_cumPlayerPool[Target_Index];


	// ========================== 예외 처리 ==========================
	bool bWakeUp = Animal->GetWakeUp();
	if (!bWakeUp) return;

	char cState = Animal->GetState();
	if (cState == OST_DIE) return;
	// ==============================================================

	if (Target_Index != NO_TARGET) {
		Vec3 vPos1 = Animal->GetLocalPos();
		Vec3 vPos2 = Target->GetLocalPos();

		bool bExit = Animal->GetExit();

		if (ObjectRangeCheck(vPos1, vPos2, 2000.f) || bExit == true)
		{
			UINT uiType = Animal->GetType();

			switch (uiType) {
			case (UINT)BEHAVIOR_TYPE::B_WARLIKE: // 선공
			{
				if (ObjectRangeCheck(vPos1, vPos2, ANIMAL_VIEW_RANGE) || bExit == true) {
					if (CollisionSphere(Animal, Target, 0.3f)) {
						PushEvent_Animal_Attack(Animal_Index, Target_Index);
					}
					else {
						PushEvent_Animal_Follow(Animal_Index, Target_Index);
					}
				}
				else {
					PushEvent_Animal_Idle(Animal_Index, Target_Index);
				}
			}
			break;
			case (UINT)BEHAVIOR_TYPE::B_PASSIVE: // 비선공
			{
				if (ObjectRangeCheck(vPos1, vPos2, ANIMAL_VIEW_RANGE) || bExit == true) {
					if (Animal->GetHit() == true) {
						if (CollisionSphere(Animal, Target, 0.3f)) {
							PushEvent_Animal_Attack(Animal_Index, Target_Index);
						}
						else {
							PushEvent_Animal_Follow(Animal_Index, Target_Index);
						}
					}
					else {
						PushEvent_Animal_Idle(Animal_Index, Target_Index);
					}
				}
				else {
					PushEvent_Animal_Idle(Animal_Index, Target_Index);
				}
			}
			break;
			case (UINT)BEHAVIOR_TYPE::B_EVASION: // 회피
			{
				if (ObjectRangeCheck(vPos1, vPos2, ANIMAL_VIEW_RANGE) || bExit == true) {
					PushEvent_Animal_Evastion(Animal_Index, Target_Index);
				}
				else {
					PushEvent_Animal_Idle(Animal_Index, Target_Index);
				}
			}
			break;
			}
		}
		else {
			PushEvent_Animal_Idle(Animal_Index, Target_Index);
		}
	}
	else {
		PushEvent_Animal_Idle(Animal_Index, NO_TARGET);
	}
	
}

void CProcess::PushEvent_Animal_Attack(USHORT AnimalId, USHORT PlayerId)
{
	Update_Event ev;
	ev.m_Do_Object = AnimalId;
	ev.m_EventType = EV_MONSTER_UPDATE;
	ev.m_From_Object = PlayerId;
	ev.m_eObjUpdate = AUT_ATTACK;
	ev.wakeup_time = high_resolution_clock::now() + 1s;
	PushEventQueue(ev);
}

void CProcess::PushEvent_Animal_Follow(USHORT AnimalId, USHORT PlayerId)
{
	Update_Event ev;
	ev.m_Do_Object = AnimalId;
	ev.m_EventType = EV_MONSTER_UPDATE;
	ev.m_From_Object = PlayerId;
	ev.m_eObjUpdate = AUT_FOLLOW;
	ev.wakeup_time = high_resolution_clock::now() + milliseconds(40);
	PushEventQueue(ev);
}

void CProcess::PushEvent_Animal_Evastion(USHORT AnimalId, USHORT PlayerId)
{
	m_pObjectPool->m_cumAnimalPool[AnimalId]->SetTarget(PlayerId);
	Update_Event ev;
	ev.m_Do_Object = AnimalId;
	ev.m_EventType = EV_MONSTER_UPDATE;
	ev.m_From_Object = PlayerId;
	ev.m_eObjUpdate = AUT_EVASION;
	ev.wakeup_time = high_resolution_clock::now() + milliseconds(40);
	PushEventQueue(ev);
}

void CProcess::PushEvent_Animal_Idle(USHORT AnimalId, USHORT PlayerId)
{
	m_pObjectPool->m_cumAnimalPool[AnimalId]->SetTarget(NO_TARGET);
	Update_Event ev;
	ev.m_Do_Object = AnimalId;
	ev.m_EventType = EV_MONSTER_UPDATE;
	ev.m_From_Object = NO_TARGET;
	ev.m_eObjUpdate = AUT_IDLE;
	ev.wakeup_time = high_resolution_clock::now() + milliseconds(40);
	PushEventQueue(ev);
}

void CProcess::PushEvent_Animal_Die(USHORT AnimalId, USHORT PlayerId)
{
	m_pObjectPool->m_cumAnimalPool[AnimalId]->SetTarget(NO_TARGET);
	Update_Event ev;
	ev.m_Do_Object = AnimalId;
	ev.m_EventType = EV_MONSTER_UPDATE;
	ev.m_From_Object = NO_TARGET;
	ev.m_eObjUpdate = AUT_DIE;
	ev.wakeup_time = high_resolution_clock::now() + 1s;
	PushEventQueue(ev);
}

void CProcess::PushEvent_Animal_Respawn(USHORT AnimalId)
{
	m_pObjectPool->m_cumAnimalPool[AnimalId]->SetState(OBJ_STATE_TYPE::OST_DIE);
	m_pObjectPool->m_cumAnimalPool[AnimalId]->SetTarget(NO_TARGET);
	BEHAVIOR_TYPE eType = m_pObjectPool->m_cumAnimalPool[AnimalId]->GetType();
	Update_Event ev;
	ev.m_Do_Object = AnimalId;
	ev.m_EventType = EV_MONSTER_UPDATE;
	ev.m_From_Object = NO_TARGET;
	ev.m_eObjUpdate = AUT_RESPAWN;

	if (eType == BEHAVIOR_TYPE::B_WARLIKE)
		ev.wakeup_time = high_resolution_clock::now() + 60s;
	else if (eType == BEHAVIOR_TYPE::B_PASSIVE)
		ev.wakeup_time = high_resolution_clock::now() + 50s;
	else if (eType == BEHAVIOR_TYPE::B_EVASION)
		ev.wakeup_time = high_resolution_clock::now() + 40s;
	PushEventQueue(ev);
}

void CProcess::PushEvent_Animal_Remove(USHORT AnimalId)
{
	Update_Event ev;
	ev.m_Do_Object = AnimalId;
	ev.m_EventType = EV_MONSTER_UPDATE;
	ev.m_From_Object = NO_TARGET;
	ev.m_eObjUpdate = AUT_REMOVE;
	ev.wakeup_time = high_resolution_clock::now() + 10s;
	PushEventQueue(ev);
}

void CProcess::PushEvent_Natural_Respawn(USHORT NaturalId)
{
	bool Destroy = m_pObjectPool->m_cumNaturalPool[NaturalId]->GetDestroy();
	if (!Destroy) return;

	Update_Event ev;
	ev.m_Do_Object = NaturalId;
	ev.m_EventType = EV_NATURAL_UPDATE;
	ev.m_From_Object = NO_TARGET;
	ev.m_eObjUpdate = NUT_RESPAWN;
	ev.wakeup_time = high_resolution_clock::now() + 10s;
	PushEventQueue(ev);
}

void CProcess::PushEvent_Etc_DB_Update()
{
	Update_Event ev;
	ev.m_Do_Object = 9995;
	ev.m_EventType = EV_ETC;
	ev.m_From_Object = NO_TARGET;
	ev.m_eObjUpdate = EUT_USERINFO_SAVE;
	ev.wakeup_time = high_resolution_clock::now() + 15s;
	PushEventQueue(ev);
}

void CProcess::PushEvent_Etc_Animal_Collision()
{
	Update_Event ev;
	ev.m_Do_Object = 9996;
	ev.m_EventType = EV_ETC;
	ev.m_From_Object = NO_TARGET;
	ev.m_eObjUpdate = EUT_ANIMAL_COLLISION;
	ev.wakeup_time = high_resolution_clock::now() + milliseconds(500);
	PushEventQueue(ev);
}

void CProcess::PushEvent_Rot()
{
	Update_Event ev;
	ev.m_Do_Object = 9997;
	ev.m_EventType = EV_ETC;
	ev.m_From_Object = NO_TARGET;
	ev.m_eObjUpdate = EUT_ROT;
	ev.wakeup_time = high_resolution_clock::now() + 1s;
	PushEventQueue(ev);
}

void CProcess::PushEvent_Etc_Weather()
{
	Update_Event ev;
	ev.m_Do_Object = 9998;
	ev.m_EventType = EV_ETC;
	ev.m_From_Object = NO_TARGET;
	ev.m_eObjUpdate = EUT_WEATHER;
	ev.wakeup_time = high_resolution_clock::now() + 30s;
	PushEventQueue(ev);
}

void CProcess::PushEvent_Etc_Time()
{
	Update_Event ev;
	ev.m_Do_Object = 9999;
	ev.m_EventType = EV_ETC;
	ev.m_From_Object = NO_TARGET;
	ev.m_eObjUpdate = EUT_TIMER;
	ev.wakeup_time = high_resolution_clock::now() + 1s;
	PushEventQueue(ev);
}

void CProcess::PushEvent_Etc_Player_Update()
{
	Update_Event ev;
	ev.m_Do_Object = 10000;
	ev.m_EventType = EV_ETC;
	ev.m_From_Object = NO_TARGET;
	ev.m_eObjUpdate = EUT_PLAYER_UPDATE;
	ev.wakeup_time = high_resolution_clock::now() + 1s;
	PushEventQueue(ev);
}
