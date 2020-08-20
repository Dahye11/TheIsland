#pragma once
#include <Engine/SceneScript.h>

#include <concurrent_unordered_map.h>
using namespace concurrency;

class CGameObject;

class CIngameScene :
	public CSceneScript
{
	CGameObject* m_pChat;
	CGameObject* m_pPlayer;
	CGameObject* m_pInventory;
	CGameObject* m_pQuickSlot;

	int m_iSelect = -1;
	bool m_bShowCursor = false;
	bool m_bShowMRT = false;
	bool m_bOverlapItem = false;
	CGameObject* m_pMRT;

	CGameObject* m_pFPSInfo;
	bool m_bShowFPS = false;

	map<UINT, CGameObject*> m_mapPlayers;
	map<UINT, CGameObject*> m_mapAnimals;
	map<UINT, CGameObject*> m_mapHousing;
	map<UINT, CGameObject*> m_mapNatural;

public:
	concurrent_unordered_map<unsigned int, CGameObject*> m_cumPlayer;
	concurrent_unordered_map<unsigned int, CGameObject*> m_cumAnimal;

public:
	CIngameScene();
	virtual ~CIngameScene();

	// CSceneScript��(��) ���� ��ӵ�
	virtual void Init() override;
	virtual void Update() override;

	void GiveStartItem();

	void CreateQuickSlotUI(CGameObject* _pInventory);
	void CreatePlayerStatusUI();
	void CreateInventoryUI();
	void CreateItemUI();
	void CreateChatUI();

	void CreateNatural();
	void CreateAnimalSpawner();

	void CreateShowFPS();

	void AnimalUpdate( USHORT uiId, Vec3 vPos, Vec3 vRot, UINT uiType );
	void AnimalDestory( USHORT uiId );
	void AnimalAnimationUpdate( USHORT uiId, UINT uiType );

	void InstallHousing( UINT uiType, USHORT uiId, Vec3 vPos, Vec3 vRot, Vec3 vScale );
	void DestroyHousing( USHORT uiId );

	void InstallNatural( UINT uiType, USHORT uiId, Vec3 vPos, Vec3 vRot, Vec3 vScale, Vec3 vOffsetPos, Vec3 vOffsetScale, float fHealth, bool bDestroy );
	void DestroyNatural( USHORT uiId );
};

