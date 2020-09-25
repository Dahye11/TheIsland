#pragma once
#include <Engine/Script.h>
#include "Animal.h"

#define ANIMAL_ANIMATION_COOLTIME 2.f

class CAnimalSpawner;

class CAnimalScript :
	public CScript
{
	tAnimalStatus	m_tStatus;

	Vec3			m_vOffsetScale;

	Vec3			m_vPrevPos;

	bool			m_bBehavior;
	float			m_fCurrentTime;
	Vec3			m_vMoveDir;
	CGameObject*	m_pTarget;

	float			m_fAttackCoolTime;
	float			m_fAttackTime;

	bool			m_bIdleBehavior;
	float			m_fIdleBehaviorTime;

	bool			m_bAttack;
	float			m_fAnimationCoolTime;

	bool			m_bAnimalDead;
	float			m_fLivingTime;

	CGameObject*	m_pParticleObj;
	float			m_fParticleTime;

	bool			m_bWakeUp;

	CAnimalSpawner*	m_pSpawner;

	UINT			m_iIndex;

public:
	CAnimalScript();
	virtual ~CAnimalScript();

	// CScript을(를) 통해 상속됨
	CLONE(CAnimalScript);
	virtual void Update() override;

	virtual void OnCollision(CCollider2D* _pOther) override;
	virtual void OnCollisionEnter(CCollider2D* _pOther) override;
	virtual void OnCollisionExit(CCollider2D* _pOther) override;

public:
	void SetWakeUp(bool bWakeUp) { m_bWakeUp = bWakeUp; }
	bool GetWakeUp() { return m_bWakeUp; }

public:
	bool CollisionSphere(Vec3 vOffsetScale, CCollider2D* _pOther, float fOffset = 1.f);
	bool CollisionHouse(Vec3 vOffsetScale, CCollider2D* _pOther, Vec3 vHouseOffsetScale, UINT iType);

	void SetAnimalStatus(tAnimalStatus tStatus);
	tAnimalStatus GetAnimalStatus();

	void SetOffsetScale(Vec3 vScale);
	Vec3 GetOffsetScale();

	void SetBehaviorType(BEHAVIOR_TYPE eType);
	void SetAnimalType(ANIMAL_TYPE eKind);

	void SetAttackCoolTime(float fTime);

	void Damage(CGameObject* _pOther, float fDamage);
	bool GetAnimalDead();

	void SetAnimalSpawner(CAnimalSpawner* pSpawner);

	void SetAnimation( CAnimator3D* pAnimation );		// 항상 SetAnimalStatus()호출 후 호출하기

	void SetIndex(UINT index) { m_iIndex = index; }
	UINT GetIndex() { return m_iIndex; }
};

