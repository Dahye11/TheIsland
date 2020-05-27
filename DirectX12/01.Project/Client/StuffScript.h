#pragma once
#include "ItemScript.h"

enum STUFF_TYPE {
	STUFF_STONE,
	STUFF_WOOD,
	STUFF_END
};

class CStuffScript :
	public CItemScript
{
	STUFF_TYPE		m_eStuff;

public:
	CStuffScript(STUFF_TYPE eType);
	virtual ~CStuffScript();

public:
	// CItemScript��(��) ���� ��ӵ�
	CLONE(CStuffScript);

	virtual void Update() override;
	virtual void Use() override;
};

