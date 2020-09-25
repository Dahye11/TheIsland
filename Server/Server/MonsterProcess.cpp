#include "MonsterProcess.h"
#include "PacketMgr.h"
#include "TimerMgr.h"
#include <DirectXMath.h>


random_device	rd;

uniform_real_distribution<float>	urd(0.0, 1.0);

CMonsterProcess::CMonsterProcess()
{
	BindMonsterUpdate();
}


CMonsterProcess::~CMonsterProcess()
{
}

void CMonsterProcess::AttackEvent(USHORT Animal_Id, USHORT usTarget)
{
	auto& Animal = m_pObjectPool->m_cumAnimalPool[Animal_Id];
	auto& Target = m_pObjectPool->m_cumPlayerPool[usTarget];

	USHORT&	Animal_Index = Animal_Id;
	USHORT& Target_Index = usTarget;

	// ========================== 예외 처리 ==========================
	bool bWakeUp = Animal->GetWakeUp();
	if (!bWakeUp) return;

	char Animal_State = Animal->GetState();
	if (Animal_State == OBJ_STATE_TYPE::OST_DIE)	return;

	concurrent_unordered_set<USHORT> loginList;
	concurrent_unordered_set<USHORT> rangeList;
	CopyBeforeLoginList(loginList);
	InRangePlayer(loginList, rangeList, Animal_Index);

	if (rangeList.empty()) {
		Animal->SetExit(false);
		Animal->SetWakeUp(false);
		Animal->SetTarget(NO_TARGET);
		return;
	}
	// ==============================================================
	// Animation
	Animal->SetAnimation((UINT)ANIMAL_ANIMATION_TYPE::ATTACK);
	

	Vec3 vAnimalPos = Animal->GetLocalPos();
	Animal->SetPrevPos(vAnimalPos);

	Vec3 vTargetPos = Target->GetLocalPos();
	float fAnimalSpeed = Animal->GetSpeed();
	Vec3 vTargetRot = Target->GetLocalRot();

	Vec3 vDir = XMVector3Normalize(vTargetPos - vAnimalPos);
	vDir.y = 0.f;
	vAnimalPos += vDir * fAnimalSpeed * 0.05f;


	Animal->SetLocalRot(Vec3(-3.141592654f / 2.f, atan2(vDir.x, vDir.z) + 3.141592f, 0.f));

	for (auto& user : rangeList) {
		bool bConnect = m_pObjectPool->m_cumPlayerPool[user]->GetConnect();
		if (!bConnect) continue;
		bool bExist = m_pObjectPool->m_cumPlayerPool[user]->ExistList(Animal_Index);
		if (bExist) {
			CPacketMgr::Send_Pos_Packet(user, Animal_Index);
			CPacketMgr::Send_Animation_Packet(user, Animal_Index, (UINT)ANIMAL_ANIMATION_TYPE::ATTACK);
		}
	}

	// Attack
	float fTarget_CurrHp = Target->GetHealth();
	float fAnimalDamage = Animal->GetDamage();
	fTarget_CurrHp -= fAnimalDamage;
	float fTarget_AfterHp = fTarget_CurrHp;


	if (fTarget_AfterHp <= 0.f)
	{
		// - Player
		fTarget_AfterHp = 0.f;
		Target->SetHealth(fTarget_AfterHp);
		Target->SetState(OST_DIE);
		CPacketMgr::Send_Status_Player_Packet(Target_Index);
		CPacketMgr::Send_Death_Player_Packet(Target_Index);
	}
	else
	{
		// - Player
		Target->SetHealth(fTarget_AfterHp);
		CPacketMgr::Send_Status_Player_Packet(Target_Index);
	}
	// ========================================================
	// New Target
	USHORT NewTarget = NO_TARGET;
	float fDist = ANIMAL_VIEW_RANGE;

	InRangePlayer(loginList, rangeList, Animal_Index);
	for (auto user : rangeList) {
		if (!m_pObjectPool->m_cumPlayerPool[user]->GetConnect()) continue;
		if (m_pObjectPool->m_cumPlayerPool[user]->GetState() == OST_DIE) continue;

		Vec3 vPos1 = Animal->GetLocalPos();
		Vec3 vPos2 = m_pObjectPool->m_cumPlayerPool[user]->GetLocalPos();
		if (ObjectRangeCheck(vPos1, vPos2, 2000.f)) {
			float currDist = CalculationDistance(vPos1, vPos2);
			if (fDist >= currDist) {
				fDist = currDist;
				NewTarget = user;
			}
		}
	}
	// ==========================================================
	if (NewTarget == NO_TARGET) {
		bool bExit = Animal->GetExit();
		if (bExit) {
			int iCount = Animal->GetExitCount();
			if (iCount == 0) {
				Animal->SetExit(false);
				Animal->SetWakeUp(false);
				Animal->SetTarget(NO_TARGET);
				return;
			}
			else {
				Animal->MinusExitCount();
			}
		}
		else {
			Animal->SetExit(true);
			Animal->SetExitCount(60);
		}

		//Animal->SetTarget(NO_TARGET);
		PushEvent_Animal_Behavior(Animal_Index);
	}
	else {
		Animal->SetExit(false);
		Animal->SetExitCount(0);
		Animal->SetTarget(NewTarget);
		PushEvent_Animal_Behavior(Animal_Index);
	}
}

void CMonsterProcess::FollowEvent(USHORT AnimalId, USHORT usTarget)
{
	auto& Animal = m_pObjectPool->m_cumAnimalPool[AnimalId];
	auto& Target = m_pObjectPool->m_cumPlayerPool[usTarget];

	USHORT&	Animal_Index = AnimalId;
	USHORT& Target_Index = usTarget;

	// ========================== 예외 처리 ==========================
	bool bWakeUp = Animal->GetWakeUp();
	if (!bWakeUp) return;

	char Animal_State = Animal->GetState();
	if (Animal_State == OBJ_STATE_TYPE::OST_DIE)	return;

	concurrent_unordered_set<USHORT> loginList;
	concurrent_unordered_set<USHORT> rangeList;
	CopyBeforeLoginList(loginList);
	InRangePlayer(loginList, rangeList, Animal_Index);

	if (rangeList.empty()) {
		Animal->SetExit(false); 
		Animal->SetWakeUp(false);
		Animal->SetTarget(NO_TARGET);
		return;
	}
	// ==============================================================
	// Animation
	Animal->SetAnimation((UINT)ANIMAL_ANIMATION_TYPE::WALK);
	// Follow
	Vec3 vAnimalPos = Animal->GetLocalPos();
	Animal->SetPrevPos(vAnimalPos);

	Vec3 vTargetPos = Target->GetLocalPos();
	float fAnimalSpeed = Animal->GetSpeed();
	Vec3 vTargetRot = Target->GetLocalRot();

	m_pObjectPool->m_cumAnimalPool[AnimalId]->SetPrevPos(vAnimalPos);

	Vec3 vDir = XMVector3Normalize(vTargetPos - vAnimalPos);
	vDir.y = 0.f;
	vAnimalPos += vDir * fAnimalSpeed * 0.05f;


	Animal->SetLocalRot(Vec3(-3.141592654f / 2.f, atan2(vDir.x, vDir.z) + 3.141592f, 0.f));
	Animal->SetLocalPos(vAnimalPos);
	// Collision
	m_pObjectPool->Animal_Collision(Animal_Index);
	// ==========================================================
	// New Target
	USHORT NewTarget = NO_TARGET;
	float fDist = ANIMAL_VIEW_RANGE;

	InRangePlayer(loginList, rangeList, Animal_Index);
	for (auto user : rangeList) {
		if (!m_pObjectPool->m_cumPlayerPool[user]->GetConnect()) continue;
		if (m_pObjectPool->m_cumPlayerPool[user]->GetState() == OST_DIE) continue;

		CPacketMgr::Send_Pos_Packet(user, Animal_Index);
		CPacketMgr::Send_Animation_Packet(user, Animal_Index, (UINT)ANIMAL_ANIMATION_TYPE::WALK);
		
		Vec3 vPos1 = Animal->GetLocalPos();
		Vec3 vPos2 = m_pObjectPool->m_cumPlayerPool[user]->GetLocalPos();

		if (ObjectRangeCheck(vPos1, vPos2, 2000.f)) {
			float currDist = CalculationDistance(vPos1, vPos2);
			if (fDist >= currDist) {
				fDist = currDist;
				NewTarget = user;
			}
		}
	}
	// ==========================================================
	if (NewTarget == NO_TARGET) {
		bool bExit = Animal->GetExit();
		if (bExit) {
			int iCount = Animal->GetExitCount();
			if (iCount == 0) {
				Animal->SetExit(false);
				Animal->SetWakeUp(false);
				Animal->SetTarget(NO_TARGET);
				return;
			}
			else {
				Animal->MinusExitCount();
			}
		}
		else {
			Animal->SetExit(true);
			Animal->SetExitCount(60);
		}
		
		//Animal->SetTarget(NO_TARGET);
		PushEvent_Animal_Behavior(Animal_Index);
	}
	else {
		Animal->SetExit(false);
		Animal->SetExitCount(0);
		Animal->SetTarget(NewTarget);
		PushEvent_Animal_Behavior(Animal_Index);
	}
}

void CMonsterProcess::EvastionEvent(USHORT AnimalId, USHORT usTarget)
{
	auto& Animal = m_pObjectPool->m_cumAnimalPool[AnimalId];
	auto& Target = m_pObjectPool->m_cumPlayerPool[usTarget];

	USHORT&	Animal_Index = AnimalId;
	USHORT& Target_Index = usTarget;

	// ========================== 예외 처리 ==========================
	bool bWakeUp = Animal->GetWakeUp();
	if (!bWakeUp) return;

	char Animal_State = Animal->GetState();
	if (Animal_State == OBJ_STATE_TYPE::OST_DIE)	return;

	concurrent_unordered_set<USHORT> loginList;
	concurrent_unordered_set<USHORT> rangeList;
	CopyBeforeLoginList(loginList);
	InRangePlayer(loginList, rangeList, Animal_Index);

	if (rangeList.empty()) {
		Animal->SetExit(false); 
		Animal->SetWakeUp(false);
		Animal->SetTarget(NO_TARGET);
		return;
	}
	// ==============================================================
	// Animation
	Animal->SetAnimation((UINT)ANIMAL_ANIMATION_TYPE::RUN);
	// Evastion
	Vec3 vAnimalPos = Animal->GetLocalPos();
	Animal->SetPrevPos(vAnimalPos);

	Vec3 vTargetPos = Target->GetLocalPos();

	float fAnimalSpeed = Animal->GetSpeed();
	Vec3 vDir = XMVector3Normalize(vTargetPos - vAnimalPos);
	vDir.y = 0.f;
	vAnimalPos += - vDir * fAnimalSpeed * 0.05f;
	Animal->SetLocalRot(Vec3(0.f, atan2( - vDir.x, - vDir.z) + 3.141592f, 0.f));
	Animal->SetLocalPos(vAnimalPos);
	// Collision
	m_pObjectPool->Animal_Collision(Animal_Index);
	// ==========================================================
	// New Target
	USHORT NewTarget = NO_TARGET;
	float fDist = ANIMAL_VIEW_RANGE;

	InRangePlayer(loginList, rangeList, Animal_Index);
	for (auto user : rangeList) {
		if (!m_pObjectPool->m_cumPlayerPool[user]->GetConnect()) continue;
		if (m_pObjectPool->m_cumPlayerPool[user]->GetState() == OST_DIE) continue;

		CPacketMgr::Send_Pos_Packet(user, Animal_Index);
		CPacketMgr::Send_Animation_Packet(user, Animal_Index, (UINT)ANIMAL_ANIMATION_TYPE::RUN);

		Vec3 vPos1 = Animal->GetLocalPos();
		Vec3 vPos2 = m_pObjectPool->m_cumPlayerPool[user]->GetLocalPos();

		if (ObjectRangeCheck(vPos1, vPos2, 2000.f)) {
			float currDist = CalculationDistance(vPos1, vPos2);
			if (fDist >= currDist) {
				fDist = currDist;
				NewTarget = user;
			}
		}
	}

	if (NewTarget == NO_TARGET) {
		bool bExit = Animal->GetExit();
		if (bExit) {
			int iCount = Animal->GetExitCount();
			if (iCount == 0) {
				Animal->SetExit(false);
				Animal->SetWakeUp(false);
				Animal->SetTarget(NO_TARGET);
				return;
			}
			else {
				Animal->MinusExitCount();
			}
		}
		else {
			Animal->SetExit(true);
			Animal->SetExitCount(60);
		}

		//Animal->SetTarget(NO_TARGET);
		PushEvent_Animal_Behavior(Animal_Index);
	}
	else {
		Animal->SetExit(false);
		Animal->SetExitCount(0);
		Animal->SetTarget(NewTarget);
		PushEvent_Animal_Behavior(Animal_Index);
	}
}

void CMonsterProcess::IdleEvent(USHORT AnimalId)
{	
	auto& Animal = m_pObjectPool->m_cumAnimalPool[AnimalId];

	USHORT& Animal_Index = AnimalId;

	// ========================== 예외 처리 ==========================
	bool bWakeUp = Animal->GetWakeUp();
	if (!bWakeUp) return;

	char Animal_State = Animal->GetState();
	if (Animal_State == OBJ_STATE_TYPE::OST_DIE)	return;

	concurrent_unordered_set<USHORT> loginList;
	concurrent_unordered_set<USHORT> rangeList;
	CopyBeforeLoginList(loginList);
	InRangePlayer(loginList, rangeList, Animal_Index);

	bool bExit = Animal->GetExit();

	if (bExit) {
		int iCount = Animal->GetExitCount();
		if (iCount == 0) {
			Animal->SetExit(false);
			Animal->SetWakeUp(false);
			Animal->SetTarget(NO_TARGET);
			return;
		}
		else {
			Animal->MinusBehaviorCount();
		}
	}
	else {
		if (rangeList.empty()) {
			Animal->SetWakeUp(false);
			Animal->SetTarget(NO_TARGET);
			return;
		}
	}
	// ==============================================================

	int iCount = Animal->GetBehaviorCount();

	if (iCount == 0) {
		int random = rand() % 2;

		if (random == 0) {
			Animal->SetIdle(false);
			Animal->SetBehaviorCount(rand() % 50);
			Vec3 vDir = Vec3(urd(rd), 0.f, urd(rd));
			vDir = XMVector3Normalize(vDir);
			Animal->SetDir(vDir);
		}
		else if (random == 1) {
			Animal->SetIdle(true);
			Animal->SetBehaviorCount(rand() % 50);
		}
	}
	else {
		Animal->MinusBehaviorCount();

		bool bIdle = Animal->GetIdle();

		if (bIdle) {
			// Animation
			Animal->SetAnimation((UINT)ANIMAL_ANIMATION_TYPE::IDLE);
		}
		else {
			// Animation
			Animal->SetAnimation((UINT)ANIMAL_ANIMATION_TYPE::WALK);

			Vec3	vPos = Animal->GetLocalPos();
			Animal->SetPrevPos(vPos);

			/*Vec3 vDir = Vec3(rand() / (float)RAND_MAX, 0.f, rand() / (float)RAND_MAX);
			vDir = XMVector3Normalize(vDir);
			Animal->SetDir(vDir);*/
			Vec3 vDir = Animal->GetDir();
			float fSpeed = Animal->GetSpeed();
			vPos += vDir * fSpeed * 0.05f;

			char eType = Animal->GetKind();

			if (A_BEAR == eType || A_BOAR == eType)
				Animal->SetLocalRot(Vec3(-3.141592654f / 2.f, atan2(vDir.x, vDir.z) + 3.141592f, 0.f));
			else {
				if (A_WOLF == eType)
					Animal->SetLocalRot(Vec3(0.f, atan2(vDir.x, vDir.z), 0.f));
				else
					Animal->SetLocalRot(Vec3(0.f, atan2(vDir.x, vDir.z) + 3.141592f, 0.f));
			}

			Animal->SetLocalPos(vPos);

			// Collision
			m_pObjectPool->Animal_Collision(Animal_Index);
		}
	}

	// Animation 보내기, 주변 타겟 찾기

	USHORT NewTarget = NO_TARGET;
	float fDist = PLAYER_VIEW_RANGE;
	for (auto user : loginList) {
		auto& Player = m_pObjectPool->m_cumPlayerPool[user];

		Vec3 vPos1 = Animal->GetLocalPos();
		Vec3 vPos2 = Player->GetLocalPos();
		if (ObjectRangeCheck(vPos1, vPos2, PLAYER_VIEW_RANGE)) {
			bool bIdle = Animal->GetIdle();
			if (bIdle) {

			}
			else {
				CPacketMgr::Send_Pos_Packet(user, AnimalId);
				CPacketMgr::Send_Animation_Packet(user, AnimalId, (UINT)ANIMAL_ANIMATION_TYPE::WALK);
			}
			float currDist = CalculationDistance(vPos1, vPos2);
			if (fDist >= currDist) {
				fDist = currDist;
				NewTarget = user;
			}
		}
	}

	//USHORT usNewTarget = FindTarget(AnimalId);
	if (NewTarget == NO_TARGET) {
		m_pObjectPool->m_cumAnimalPool[AnimalId]->SetTarget(NO_TARGET);
		PushEvent_Animal_Idle(AnimalId, NO_TARGET);
	}
	else {
		m_pObjectPool->m_cumAnimalPool[AnimalId]->SetTarget(NewTarget);
		PushEvent_Animal_Behavior(AnimalId);
	}
}

void CMonsterProcess::DieEvent(USHORT Animal_Id)
{
	auto& Animal = m_pObjectPool->m_cumAnimalPool[Animal_Id];

	if (Animal->GetState() == OST_LIVE)
		return;

	// Animation
	Animal->SetAnimation((UINT)ANIMAL_ANIMATION_TYPE::DIE);

	Vec3 AnimalPos = Animal->GetLocalPos();
	Animal->SetPrevPos(AnimalPos);

	concurrent_unordered_set<USHORT> login_list;
	CopyBeforeLoginList(login_list);
	for (auto& user : login_list) {
		Vec3 PlayerPos = m_pObjectPool->m_cumPlayerPool[user]->GetLocalPos();
		if (ObjectRangeCheck(PlayerPos, AnimalPos, PLAYER_VIEW_RANGE)) {
			CPacketMgr::Send_Animation_Packet(user, Animal_Id, (UINT)ANIMAL_ANIMATION_TYPE::DIE);
		}
	}

	PushEvent_Animal_Remove(Animal_Id);
}

void CMonsterProcess::RemoveEvent(USHORT AnimalId)
{
	auto& Animal = m_pObjectPool->m_cumAnimalPool[AnimalId];

	char eState = Animal->GetState();
	if (eState == OST_LIVE || eState == OST_REMOVE)
		return;

	Vec3 AnimalPos = Animal->GetLocalPos();
	Animal->SetState(OST_REMOVE);

	concurrent_unordered_set<USHORT> login_list;
	CopyBeforeLoginList(login_list);
	for (auto& user : login_list) {
		bool bConnect = m_pObjectPool->m_cumPlayerPool[user]->GetConnect();
		if (!bConnect) continue;
		Vec3 PlayerPos = m_pObjectPool->m_cumPlayerPool[user]->GetLocalPos();
		if (ObjectRangeCheck(PlayerPos, AnimalPos, PLAYER_VIEW_RANGE)) {
			CPacketMgr::Send_Remove_Packet(user, AnimalId);
		}
	}
	PushEvent_Animal_Respawn(AnimalId);
}

void CMonsterProcess::RespawnEvent(USHORT Animal_Id)
{
	auto& Animal = m_pObjectPool->m_cumAnimalPool[Animal_Id];
	USHORT& Animal_Index = Animal_Id;

	m_pObjectPool->Init_Animal(Animal_Index);

	concurrent_unordered_set<USHORT> login_list;
	concurrent_unordered_set<USHORT> range_list;

	CopyBeforeLoginList(login_list);
	InRangePlayer(login_list, range_list, Animal_Index);

	if (range_list.empty()) {
		return;
	}

	Animal->SetWakeUp(true);

	Vec3 monster_pos = Animal->GetLocalPos();

	for (auto& au : range_list) {
		bool isConnect = CProcess::m_pObjectPool->m_cumPlayerPool[au]->GetConnect();
		if (!isConnect) continue;
		CPacketMgr::GetInst()->Send_Pos_Packet(au, Animal_Index);
	}

	PushEvent_Animal_Behavior(Animal_Index);
}

void CMonsterProcess::InRangePlayer(concurrent_unordered_set<USHORT>& cusLogin, concurrent_unordered_set<USHORT>& cusList, USHORT uiMonster)
{
	Vec3 monster_pos = CProcess::m_pObjectPool->m_cumAnimalPool[uiMonster]->GetLocalPos();
	for (auto& au : cusLogin) {
		bool isConnect = CProcess::m_pObjectPool->m_cumPlayerPool[au]->GetConnect();
		if (!isConnect) continue;
		Vec3 player_pos = CProcess::m_pObjectPool->m_cumPlayerPool[au]->GetLocalPos();
		if(ObjectRangeCheck(player_pos, monster_pos, PLAYER_VIEW_RANGE))
			cusList.insert(au);
	}
}

USHORT CMonsterProcess::FindTarget(USHORT Animal_Id)
{
	auto& Animal = m_pObjectPool->m_cumAnimalPool[Animal_Id];

	concurrent_unordered_set<USHORT>	loginList;
	CopyBeforeLoginList(loginList);

	USHORT NewTarget = NO_TARGET;
	float fDist = PLAYER_VIEW_RANGE;
	for (auto user : loginList) {
		auto& Player = m_pObjectPool->m_cumPlayerPool[user];

		Vec3 vPos1 = Animal->GetLocalPos();
		Vec3 vPos2 = Player->GetLocalPos();
		if (ObjectRangeCheck(vPos1, vPos2, 2000.f)) {
			float currDist = CalculationDistance(vPos1, vPos2);
			if (fDist >= currDist) {
				fDist = currDist;
				NewTarget = user;
			}
		}
	}
	return NewTarget;
}
