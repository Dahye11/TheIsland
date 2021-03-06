#pragma once
#include <Engine/Script.h>

#define ARMOR_SLOT 25
class CItemScript;
class CItemLootScript;

class CInventoryScript :
	public CScript
{
	vector<CGameObject*>	m_vecItemSlot;
	vector<CItemScript*>	m_vecItem;

	vector<CGameObject*>	m_vecRecipe;
	UINT					m_iRecipePage;
	CGameObject*			m_pNextPage;
	CGameObject*			m_pPrevPage;

	CGameObject*			m_pItemInfo;

	CGameObject*			m_pArmorSlot;
	CGameObject*			m_pArmor;

	bool					m_bActive;
	bool					m_bAddable;

	bool					m_bClick;
	int						m_iClickIdx;
	CGameObject*			m_pClickObj;

	CItemLootScript*		m_pItemLootScript;

	CGameObject*			m_pPlayer;
public:
	CInventoryScript();
	virtual ~CInventoryScript();

	CLONE(CInventoryScript);

	virtual void Update() override;

public:
	void AddSlot(CGameObject* pObject);
	void Init();
	void AddItem(CItemScript* pItem, int iCount = 1);

	bool GetInventoryActive() { return m_bActive; }
	
	void SetItemLootScript(CItemLootScript* pScript);
	void Show();
	void OnAddable(int index);

	int CheckItem(UINT eType, int iCount = 1);
	bool DecreaseItem(int idx, int iCount = 1);
	void DestroyArmor();

	void SetPlayer(CGameObject* pPlayer);

	UINT Use_Left(CGameObject* pHost, CGameObject* pObj, int index);
	UINT Use_Right(CGameObject* pHost, CGameObject* pObj, int index);
	void Use_Highlight(CGameObject* pHost, CGameObject* pObj, int index);
	void DisableItem(CGameObject* pHost, int index);
	UINT EnableItem(CGameObject* pHost, int index);

private:
	void AddItemFunc(CItemScript* pItem, int iCount);
	bool CheckClickUI(Vec2 vMousePos, CGameObject* pObj);

	void CheckAddable();

	void RecipeInit();
	void ShowRecipe();
	void HideRecipe();

};

