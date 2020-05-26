#pragma once
#include <Engine/Script.h>

enum ANIMAL_TYPE
{
	A_BEAR,
	A_BOAR,
	A_DEER,
	A_WOLF,
	A_END
};

enum BEHAVIOR_TYPE
{
	B_WARLIKE,
	B_PASSIVE,
	B_EVASION,
	B_END
};

struct tAnimalStatus
{
	float fHp;
	float fStamina;

	float fSpeed;
	float fDamage;

	float fBehaviorTime;

	BEHAVIOR_TYPE eType;
	ANIMAL_TYPE eKind;
};

class CAnimalScript :
	public CScript
{
	tAnimalStatus	m_tStatus;

	Vec3			m_vOffsetScale;

	bool			m_bBehavior;
	float			m_fCurrentTime;
	Vec3			m_vMoveDir;
	CGameObject*	m_pTarget;

	float			m_fAttackCoolTime;
	float			m_fAttackTime;

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
	bool CollisionSphere(Vec3 vOffsetScale, CCollider2D* _pOther);

	void SetAnimalStatus(tAnimalStatus tStatus);
	tAnimalStatus GetAnimalStatus();

	void SetOffsetScale(Vec3 vScale);

	void SetBehaviorType(BEHAVIOR_TYPE eType);
	void SetAnimalType(ANIMAL_TYPE eKind);

	void SetAttackCoolTime(float fTime);
};

