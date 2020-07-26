#pragma once
#include "ItemScript.h"

#define STUFF_MAX_COUNT 100

class CStuffScript :
	public CItemScript
{
public:
	CStuffScript(ITEM_TYPE eType, int iCount = 1);
	virtual ~CStuffScript();

public:
	// CItemScript��(��) ���� ��ӵ�
	CLONE(CStuffScript);

	virtual void Update() override;
	virtual void Use_Right(CGameObject* pHost, CGameObject* pObj, int num) override;
	virtual void Use_Left(CGameObject* pHost, CGameObject* pObj, int num) override;
	virtual void Use_Highlight(CGameObject* pHost, CGameObject* pObj, int num) override;
	virtual void EnableItem(CGameObject* pHost, int num) override {};
	virtual void DisableItem(CGameObject* pHost, int num) override {};
};
