#include "EtcProcess.h"
#include "PacketMgr.h"
#include "TimerMgr.h"
#include "DataBase.h"

CEtcProcess::CEtcProcess()
{
	//PushEvent_Etc_DB_Update();
	//PushEvent_Etc_Animal_Collision();
	PushEvent_Rot();
	PushEvent_Etc_Weather();
	PushEvent_Etc_Time();
	PushEvent_Etc_Player_Update();
}


CEtcProcess::~CEtcProcess()
{
	
}

//void CEtcProcess::Animal_Collision_Event()
//{
//	for (auto& animal : m_pObjectPool->m_cumAnimalPool) {
//		bool bWakeUp = animal.second->GetWakeUp();
//		if (!bWakeUp) continue;
//		// - Player Collision
//		// - Natural Collision
//		for (auto& natural : m_pObjectPool->m_cumNaturalPool) {
//			char eType = natural.second->GetType();
//
//			if (NATURAL_TYPE::N_BUSH == eType)
//				continue;
//
//			if (CollisionSphere(animal, natural)) {
//				Vec3 vNaturalPos = natural.second->GetLocalPos();
//				Vec3 vAnimalPos = animal.second->GetLocalPos();
//				Vec3 vDir = XMVector3Normalize(vAnimalPos - vNaturalPos);
//				float fSpeed = animal.second->GetSpeed();
//				vDir.y = 0.f;
//				vAnimalPos += vDir * fSpeed * 0.05f;
//				animal.second->SetLocalPos(vAnimalPos);
//			}
//		}
//		// - House Collision
//		for (auto& house : m_pObjectPool->m_cumHousingPool) {
//			char eType = house.second->GetType();
//			if (eType >= HOUSING_TYPE::HOUSING_FOUNDATION && eType < HOUSING_TYPE::HOUSING_END) {
//				bool bCollision = false;
//				bCollision = CollisionHouse(animal, house, eType);
//				if (bCollision){
//					Vec3 vPrevPos = animal.second->GetPrevPos();
//					animal.second->SetLocalPos(vPrevPos);
//				}
//			}
//		}
//	}
//	PushEvent_Etc_Animal_Collision();
//}

void CEtcProcess::Rot_Event()
{
	concurrent_unordered_set<USHORT>	loginList;
	CProcess::CopyBeforeLoginList(loginList);
	for (auto user : loginList) {
		bool bConnect = m_pObjectPool->m_cumPlayerPool[user]->GetConnect();
		if (!bConnect) continue;
		
		concurrent_unordered_set<USHORT>	ViewList;
		m_pObjectPool->m_cumPlayerPool[user]->CopyViewList(ViewList);
		for (auto& other : ViewList) {
			if (MAX_USER <= other && other < END_ANIMAL)
			{
				if (m_pObjectPool->m_cumAnimalPool[other]->GetState() == OST_DIE || !m_pObjectPool->m_cumAnimalPool[other]->GetWakeUp())
					continue;
			}
			CPacketMgr::Send_Rot_Packet(user, other);
		}
	}

	PushEvent_Rot();
}

void CEtcProcess::WeatherEvent()
{
	bool bRain = rand() % 2;

	concurrent_unordered_set<USHORT> loginList;
	CopyBeforeLoginList(loginList);

	for (auto& playerId : loginList) {
		bool bConnect = m_pObjectPool->m_cumPlayerPool[playerId]->GetConnect();
		if (!bConnect) continue;
		CPacketMgr::Send_Weather_Packet(playerId, bRain);
	}
	// Add Event
	PushEvent_Etc_Weather();
}

void CEtcProcess::TimerEvent()
{
	m_Minute += 1;

	if (((int)m_Minute % 60) == 0) {
		m_Minute++;
		if (m_Minute >= 60) {
			m_Hour++;
			m_Minute = 0;
			if (m_Hour >= 24) {
				m_Hour = 0.f;
				m_Day++;
			}
		}
	}
	concurrent_unordered_set<USHORT> loginList;
	CopyBeforeLoginList(loginList);

	for (auto& playerId : loginList)
	{
		bool bConnect = m_pObjectPool->m_cumPlayerPool[playerId]->GetConnect();
		if (!bConnect) continue;
		CPacketMgr::Send_Time_Packet(playerId, m_Day, m_Hour, m_Minute);
	}
	// Add Event
	PushEvent_Etc_Time();
}

void CEtcProcess::UserInfo_Save_Event()
{
	for (auto& user : m_pObjectPool->m_cumPlayerPool) {
		bool bConnect = user.second->GetConnect();
		if (!bConnect) continue;

		DB_Event ev;
		ev.inum = user.second->GetDbNum();
		ev.fHealth = user.second->GetHealth();
		ev.fHungry = user.second->GetHungry();
		ev.fThirst = user.second->GetThirst();
		Vec3 vPos = user.second->GetLocalPos();
		ev.fX = vPos.x;
		ev.fY = vPos.y;
		ev.fZ = vPos.z;
		CDataBase::GetInst()->UpdateUserInfo(ev);
	}
	PushEvent_Etc_DB_Update();
}

void CEtcProcess::PlayerUpdate_Event()
{
	float HealthValueByHungry = 0.5f;
	float HealthValueByThirst = 0.25f;
	float HungryValue = 0.2f;
	float ThirstValue = 0.35f;

	for (auto& user : m_pObjectPool->m_cumPlayerPool) {
		bool bConnect = user.second->GetConnect();
		if (!bConnect) continue;

		user.second->SetDecreaseHungry(HungryValue);
		user.second->SetDecreaseThirst(ThirstValue);

		float Hungry = user.second->GetHungry();
		float Thirst = user.second->GetThirst();

		if (Hungry <= 0.f) {
			user.second->SetDecreaseHealth(HealthValueByHungry);
		}
		if (Thirst <= 0.f) {
			user.second->SetDecreaseHealth(HealthValueByThirst);
		}

		CPacketMgr::Send_Status_Player_Packet(user.first);
	}

	PushEvent_Etc_Player_Update();
}
