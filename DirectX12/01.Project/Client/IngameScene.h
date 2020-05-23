#pragma once
#include <Engine/SceneScript.h>

class CGameObject;

class CIngameScene :
	public CSceneScript
{
	CGameObject* m_pChat;
	CGameObject* m_pPlayer;
public:
	CIngameScene();
	virtual ~CIngameScene();

	// CSceneScript��(��) ���� ��ӵ�
	virtual void Init() override;
	virtual void Update() override;


	void CreateQuickSlotUI(CGameObject* _pInventory);
	void CreatePlayerStatusUI();
	void CreateInventoryUI();
	void CreateChatUI();
};

