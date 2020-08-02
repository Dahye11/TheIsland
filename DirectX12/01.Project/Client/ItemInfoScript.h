#pragma once
#include <Engine/Script.h>

class CItemInfoScript :
	public CScript
{
	CGameObject*	m_pBackground;
	CGameObject*	m_pItemName;
	CGameObject*	m_pItemInfo;
	string			m_strPrevItemName;
public:
	CItemInfoScript();
	virtual ~CItemInfoScript();

	// CScript��(��) ���� ��ӵ�
	CLONE(CItemInfoScript);
	virtual void Update() override;

public:
	void SetObject(CGameObject* pBackground, CGameObject* pItemName, CGameObject* pItemInfo);

	void SetItemName(string strName);
	void SetItemInfo(string strInfo);
	void SetItem(UINT eType);
};

