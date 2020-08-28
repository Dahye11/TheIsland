#pragma once

#include "Component.h"

class CGameObject;

class CBoneSocket : 
	public CComponent
{
public:
	CBoneSocket();
	~CBoneSocket();

	// CComponent��(��) ���� ��ӵ�
	virtual void SaveToScene( FILE * _pFile ) override;
	virtual void LoadFromScene( FILE * _pFile ) override;
	CLONE( CBoneSocket );

private:
	Vec3			m_vOffset;
	CGameObject*	m_pTarget;
	wstring			m_strBoneName;

public:
	void SetTarget( CGameObject* pTarget );
	void SetTarget( CGameObject* pTarget, const wstring& strBoneName );

public:
	virtual void Update();
};

