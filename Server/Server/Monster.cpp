#include "Monster.h"

CMonster::CMonster() :
	m_uiTarget(NO_TARGET),
	m_bWakeUp(false),
	m_bHit(false),
	m_bIdle(true),
	m_iBehaviorCount(0),
	m_bExit(false),
	m_iExitCount(0),
	m_uiAnimation((UINT)ANIMAL_ANIMATION_TYPE::IDLE)
{
}


CMonster::~CMonster()
{
}

void CMonster::SetAnimalStatus(tAnimalStatus & animalStatus)
{
	unique_lock<shared_mutex> lock(m_smAnimalSharedMutex);
	m_tStatus.eKind = animalStatus.eKind;
	m_tStatus.eType = animalStatus.eType;
	m_tStatus.fDamage = animalStatus.fDamage;
	m_tStatus.fHealth = animalStatus.fHealth;
	m_tStatus.fSpeed = animalStatus.fSpeed;
	m_tStatus.fStamina = animalStatus.fStamina;
}

void CMonster::SetHealth(float & fHealth)
{
	unique_lock<shared_mutex> lock(m_smAnimalSharedMutex);
	m_tStatus.fHealth = fHealth;
}

void CMonster::SetSpeed(float & fSpeed)
{
	unique_lock<shared_mutex> lock(m_smAnimalSharedMutex);
	m_tStatus.fSpeed = fSpeed;
}

void CMonster::SetDamage(float & fDamage)
{
	unique_lock<shared_mutex> lock(m_smAnimalSharedMutex);
	m_tStatus.fDamage = fDamage;
}

void CMonster::SetType(BEHAVIOR_TYPE & eType)
{
	unique_lock<shared_mutex> lock(m_smAnimalSharedMutex);
	m_tStatus.eType = eType;
}

void CMonster::SetKind(ANIMAL_TYPE & eKind)
{
	unique_lock<shared_mutex> lock(m_smAnimalSharedMutex);
	m_tStatus.eKind = eKind;
}

void CMonster::SetTarget(USHORT playerId)
{
	unique_lock<shared_mutex> lock(m_smAnimalSharedMutex);
	m_uiTarget = playerId;
}

void CMonster::SetWakeUp(bool bWakeUp)
{
	unique_lock<shared_mutex>lock(m_smAnimalSharedMutex);
	m_bWakeUp = bWakeUp;
}

void CMonster::SetDir(Vec3 & vDir)
{
	unique_lock<shared_mutex> lock(m_smAnimalSharedMutex);
	m_vMoveDir = vDir;
}

void CMonster::SetPrevPos(Vec3 & vPrevPos)
{
	unique_lock<shared_mutex>	lock(m_smAnimalSharedMutex);
	m_vPrevPos = vPrevPos;
}

void CMonster::SetHit(bool bHit)
{
	unique_lock<shared_mutex>	lock(m_smAnimalSharedMutex);
	m_bHit = bHit;
}

const tAnimalStatus & CMonster::GetAnimalStatus()
{
	shared_lock<shared_mutex> lock(m_smAnimalSharedMutex);
	return m_tStatus;
}

const float & CMonster::GetHealth()
{
	shared_lock<shared_mutex> lock(m_smAnimalSharedMutex);
	return m_tStatus.fHealth;
}

const float & CMonster::GetSpeed()
{
	shared_lock<shared_mutex> lock(m_smAnimalSharedMutex);
	return m_tStatus.fSpeed;
}

const float & CMonster::GetDamage()
{
	shared_lock<shared_mutex> lock(m_smAnimalSharedMutex);
	return m_tStatus.fDamage;
}

const BEHAVIOR_TYPE & CMonster::GetType()
{
	shared_lock<shared_mutex> lock(m_smAnimalSharedMutex);
	return m_tStatus.eType;
}

const ANIMAL_TYPE & CMonster::GetKind()
{
	shared_lock<shared_mutex> lock(m_smAnimalSharedMutex);
	return m_tStatus.eKind;
}

const USHORT & CMonster::GetTarget()
{
	shared_lock<shared_mutex> lock(m_smAnimalSharedMutex);
	return m_uiTarget;
}

const bool CMonster::GetWakeUp()
{
	shared_lock<shared_mutex> lock(m_smAnimalSharedMutex);
	return m_bWakeUp;
}

const Vec3 & CMonster::GetDir()
{
	shared_lock<shared_mutex> lock(m_smAnimalSharedMutex);
	return m_vMoveDir;
}

const Vec3 & CMonster::GetPrevPos()
{
	shared_lock<shared_mutex>	lock(m_smAnimalSharedMutex);
	return m_vPrevPos;
}

const bool CMonster::GetHit()
{
	shared_lock<shared_mutex>	lock(m_smAnimalSharedMutex);
	return m_bHit;
}
