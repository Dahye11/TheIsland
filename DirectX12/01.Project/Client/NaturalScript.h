#pragma once
#include <Engine/Script.h>

#define NATURAL_RESPAWN_TIME 10.f

enum NATURAL_TYPE {
	NATURAL_TREE,
	NATURAL_STONE,
	NATURAL_BUSH,
	NATURAL_NONE,
	NATURAL_END
};

class CNaturalScript :
	public CScript
{
	float			m_fHealth;
	float			m_fTime;

	float			m_fAngle;

	NATURAL_TYPE	m_eType;

	bool			m_bDestroy;
	bool			m_bRotate;

	Vec3			m_vOrginRot;
	Vec3			m_vTargetRot;

	CGameObject*	m_pParticleObj;
	float			m_fParticleTime;

	UINT			m_iIndex;
public:
	CNaturalScript(NATURAL_TYPE eType);
	virtual ~CNaturalScript();

	CLONE(CNaturalScript);
	// CScript을(를) 통해 상속됨
	virtual void Update() override;

	virtual void OnCollision(CCollider2D* _pOther) override;
	virtual void OnCollisionEnter(CCollider2D* _pOther) override;
	virtual void OnCollisionExit(CCollider2D* _pOther) override;

	virtual void SaveToScene( FILE* _pFile );
	virtual void LoadFromScene( FILE* _pFile );

public:
	bool Damage(CGameObject* pObj, float fDamage);
	bool GetDestroy();
	NATURAL_TYPE GetNaturalType();

	void Respawn();

	void SetType( int iType );
	void SetType( NATURAL_TYPE eType );

	void SetHealth(float fHealth);
	void SetDestroy(bool bDestroy);

	void SetIndex(UINT index) { m_iIndex = index; }
	UINT GetIndex() { return m_iIndex; }
};

