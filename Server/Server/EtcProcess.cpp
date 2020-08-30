#include "EtcProcess.h"
#include "PacketMgr.h"
#include "TimerMgr.h"


CEtcProcess::CEtcProcess()
{
	PushEvent_Etc_Animal_Collision();
	PushEvent_Rot();
	PushEvent_Etc_Weather();
	PushEvent_Etc_Time();
}


CEtcProcess::~CEtcProcess()
{
	
}

void CEtcProcess::Animal_Collision_Event()
{
	for (auto& animal : m_pObjectPool->m_cumAnimalPool) {
		bool bWakeUp = animal.second->GetWakeUp();
		if (!bWakeUp) continue;
		// - Player Collision
		// - Natural Collision
		for (auto& natural : m_pObjectPool->m_cumNaturalPool) {
			char eType = natural.second->GetType();

			if (NATURAL_TYPE::N_BUSH == eType)
				continue;

			if (CollisionSphere(animal, natural)) {
				Vec3 vNaturalPos = natural.second->GetLocalPos();
				Vec3 vAnimalPos = animal.second->GetLocalPos();
				Vec3 vDir = XMVector3Normalize(vAnimalPos - vNaturalPos);
				float fSpeed = animal.second->GetSpeed();
				vDir.y = 0.f;
				vAnimalPos += vDir * fSpeed * 0.05f;
				animal.second->SetLocalPos(vAnimalPos);
			}
		}
		// - House Collision
		for (auto& house : m_pObjectPool->m_cumHousingPool) {
			char eType = house.second->GetType();
			if (eType >= HOUSING_TYPE::HOUSING_FOUNDATION && eType < HOUSING_TYPE::HOUSING_END) {
				bool bCollision = false;
				bCollision = CollisionHouse(animal, house, eType);
				if (bCollision){
					Vec3 vPrevPos = animal.second->GetPrevPos();
					animal.second->SetLocalPos(vPrevPos);
				}
			}
		}
	}
}

void CEtcProcess::Rot_Event()
{
	concurrent_unordered_set<USHORT>	loginList;
	CProcess::CopyBeforeLoginList(loginList);
	for (auto user : loginList) {
		bool bConnect = m_pObjectPool->m_cumPlayerPool[user]->GetConnect();
		if (!bConnect) continue;
		
		concurrent_unordered_set<USHORT>	ViewList;
		m_pObjectPool->m_cumPlayerPool[user]->CopyViewList(ViewList);
		for (auto& other : ViewList)
			CPacketMgr::Send_Rot_Packet(user, other);
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
	CTimerMgr::GetInst()->Tick();
	float fTime = CTimerMgr::GetInst()->GetTotalTime();

	concurrent_unordered_set<USHORT> loginList;
	CopyBeforeLoginList(loginList);

	for (auto& playerId : loginList)
	{
		bool bConnect = m_pObjectPool->m_cumPlayerPool[playerId]->GetConnect();
		if (!bConnect) continue;
		CPacketMgr::Send_Time_Packet(playerId, fTime);
	}
	// Add Event
	PushEvent_Etc_Time();
}
