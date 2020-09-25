#pragma once
#include <Engine/Script.h>
#include "Housing.h"

class CBuildScript :
	public CScript
{
	bool m_bBuild;
	bool m_bCollision;

	HOUSING_TYPE	m_eType;
	UINT			m_iGrade;

	UINT			m_iIndex;
public:
	CBuildScript(HOUSING_TYPE eType, UINT iGrade = 0);
	virtual ~CBuildScript();

public:
	virtual void Update();
	CLONE(CBuildScript);

	virtual void OnCollision(CCollider2D* _pOther) override;
	virtual void OnCollisionEnter(CCollider2D* _pOther) override;
	virtual void OnCollisionExit(CCollider2D* _pOther) override;

public:
	void Init();
	bool Build(bool bSendPacket = true);
	void MustBuild();

	bool Upgrade();

	HOUSING_TYPE GetHousingType();
	Vec3 GetOffsetScale();

	void SetIndex(UINT index) { m_iIndex = index; }
	UINT GetIndex() { return m_iIndex; }
};

