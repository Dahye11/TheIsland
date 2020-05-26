#pragma once
#include <Engine/SceneScript.h>

class CGameObject;

class CIngameScene :
	public CSceneScript
{
public:
	CIngameScene();
	virtual ~CIngameScene();

	// CSceneScript��(��) ���� ��ӵ�
	virtual void Init() override;


	void CreateQuickSlotUI(CGameObject* _pInventory);
	void CreatePlayerStatusUI();
	void CreateInventoryUI();
};

