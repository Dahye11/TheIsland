#pragma once
#include "ItemScript.h"

#define TOOL_MAX_COUNT 1

class CToolItemScript :
	public CItemScript
{
public:
	CToolItemScript(ITEM_TYPE eType, int iCount = 1);
	virtual ~CToolItemScript();

public:
	// CItemScript��(��) ���� ��ӵ�
	CLONE(CToolItemScript);

	virtual void Update() override;
	virtual void Use() override;
};

