#include "stdafx.h"
#include "IngameScene.h"

#include <Engine/Device.h>

#include <Engine/Ptr.h>

#include <Engine/Texture.h>

#include <Engine/ResMgr.h>
#include <Engine/CollisionMgr.h>
#include <Engine/RenderMgr.h>
#include <Engine/FontMgr.h>

#include <Engine/Layer.h>
#include <Engine/Scene.h>

#include <Engine/GameObject.h>
#include <Engine/MeshRender.h>
#include <Engine/Transform.h>
#include <Engine/Camera.h>
#include <Engine/Font.h>
#include <Engine/Light3D.h>
#include <Engine/LandScape.h>
#include <Engine/GridScript.h>
#include <Engine/Animator3D.h>

#include <Engine/ToolCamScript.h>
#include <Engine/MonsterScript.h>

#include "StatusScript.h"
#include "QuickSlotScript.h"
#include "PlayerCamScript.h"
#include "InventoryScript.h"
#include "SunshineScript.h"
#include "PlayerScript.h"
#include "FPSCamScript.h"
#include "ItemLootScript.h"

#include "InputScript.h"
#include "ChatScript.h"

#include "AnimalScript.h"
#include "AnimalSpawner.h"
#include "NaturalScript.h"

#include "ItemScript.h"
#include "StuffScript.h"
#include "ToolItemScript.h"
#include "UsableScript.h"

#include "CheatMgr.h"
#include "ShowFPSScript.h"

#include <Engine/TestScript.h>

#include <Engine/ParticleSystem.h>

#include <Engine/NaviMgr.h>

#include "PacketMgr.h"
#include "Network.h"
#include "RainScript.h"
#include "Housing.h"
#include "HousingMgr.h"
#include "BuildScript.h"
#include "HumanScript.h"

CIngameScene::CIngameScene() 
	: m_pChat(NULL)
	, m_pPlayer(NULL)
	, m_pFPSInfo(NULL)
{
}


CIngameScene::~CIngameScene()
{
}

void CIngameScene::Init()
{
	// MeshData 로드
	//Ptr<CMeshData> pBearTex = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\bear.mdat", L"MeshData\\bear.mdat");
	//Ptr<CMeshData> pWolfTex = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\wolf.mdat", L"MeshData\\wolf.mdat");
	//Ptr<CMeshData> pBoarTex = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\boar.mdat", L"MeshData\\boar.mdat");
	//Ptr<CMeshData> pDeerTex = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\deer.mdat", L"MeshData\\deer.mdat");
	//Ptr<CMeshData> pTreeATex = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\sprucea.mdat", L"MeshData\\sprucea.mdat");
	//Ptr<CMeshData> pTreeBTex = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\spruceb.mdat", L"MeshData\\spruceb.mdat");
	//Ptr<CMeshData> pTreeCTex = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\sprucec.mdat", L"MeshData\\sprucec.mdat");

	CGameObject * pTestObject = nullptr;
	  
	Ptr<CTexture> pTex = CResMgr::GetInst()->Load<CTexture>( L"TestTex", L"Texture\\Health.png" );
	Ptr<CTexture> pExplosionTex = CResMgr::GetInst()->Load<CTexture>( L"Explosion", L"Texture\\Explosion\\Explosion80.png" );
	Ptr<CTexture> pBlackTex = CResMgr::GetInst()->Load<CTexture>( L"Black", L"Texture\\asd.png" );
	Ptr<CTexture> pSky01 = CResMgr::GetInst()->Load<CTexture>( L"Sky01", L"Texture\\Skybox\\Sky01.png" );

	Ptr<CTexture> pColor = CResMgr::GetInst()->Load<CTexture>( L"Tile", L"Texture\\Tile\\TILE_03.tga" );
	Ptr<CTexture> pNormal = CResMgr::GetInst()->Load<CTexture>( L"Tile_n", L"Texture\\Tile\\TILE_03_N.tga" );

	Ptr<CTexture> pDiffuseTargetTex = CResMgr::GetInst()->FindRes<CTexture>( L"DiffuseTargetTex" );
	Ptr<CTexture> pNormalTargetTex = CResMgr::GetInst()->FindRes<CTexture>( L"NormalTargetTex" );
	Ptr<CTexture> pPositionTargetTex = CResMgr::GetInst()->FindRes<CTexture>( L"PositionTargetTex" );
	// Compute Shader Test
	// UAV 용 Texture 생성
	Ptr<CTexture> pTestUAVTexture = CResMgr::GetInst()->CreateTexture(L"UAVTexture", 1024, 1024
		, DXGI_FORMAT_R8G8B8A8_UNORM, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
		, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	Ptr<CMaterial> pPM = CResMgr::GetInst()->FindRes<CMaterial>(L"MergeLightMtrl");
	pPM->SetData(SHADER_PARAM::TEX_3, pSky01.GetPointer());

	pPM = CResMgr::GetInst()->FindRes<CMaterial>(L"PointLightMtrl");
	pPM->SetData(SHADER_PARAM::TEX_2, pSky01.GetPointer());

	CGameObject* pObject = nullptr;
	CGameObject* pChildObject = nullptr;

	// ===================
	// Player 오브젝트 생성
	// ===================
	Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\islandplayer.mdat", L"MeshData\\islandplayer.mdat");

	CGameObject* pPlayer = pMeshData->Instantiate();
	// Script 설정
	pPlayer->AddComponent( new CPlayerScript );

#ifdef CHECK_COLLISION
	pPlayer->AddComponent( new CCollider2D );
	pPlayer->Collider2D()->SetOffsetScale( Vec3( 150.f, 150.f, 150.f ) );
	pPlayer->Collider2D()->SetCollider2DType( COLLIDER2D_TYPE::SPHERE );
#endif 

	CPlayerScript* pPlayerScript = pPlayer->GetScript<CPlayerScript>();
	pPlayerScript->AnimationInfo( pPlayer->Animator3D() );

	pPlayer->SetName( L"Player Object" );
	pPlayer->FrustumCheck( false );
	pPlayer->Transform()->SetLocalPos( Vec3( 18000.f, 200.f, 2000.f ) );
	pPlayer->Transform()->SetLocalScale( Vec3( 1.5f, 1.5f, 1.5f ) );
	//pPlayer->Transform()->SetLocalRot(Vec3(0.f, 0.f, 0.f));
	m_pScene->FindLayer( L"Player" )->AddGameObject( pPlayer );
	m_pPlayer = pPlayer;
	m_pPlayer->MeshRender()->SetDynamicShadow( true );

#ifdef NETWORK_ON
	CPacketMgr::GetInst()->SetPlayerObj(m_pPlayer);
	CNetwork::GetInst()->SetPlayerObj( m_pPlayer );
#else
#endif NETWORK_ON

	CCheatMgr::GetInst()->SetPlayer( m_pPlayer );

	// ==================
	// Camera Object 생성
	// ==================
	CGameObject* pMainCam = new CGameObject;
	pMainCam->SetName( L"MainCam" );
	pMainCam->AddComponent( new CTransform );
	pMainCam->AddComponent( new CCamera );
	pMainCam->AddComponent( new CFPSCamScript );

	pMainCam->Transform()->SetLocalPos( Vec3( 0.f, 100.f, 0.f ) );
	//pMainCam->Transform()->SetLocalRot(Vec3(0.f, XM_PI, 0.f));

	pMainCam->Camera()->SetProjType( PROJ_TYPE::PERSPECTIVE );
	pMainCam->Camera()->SetFar( 10000.f );
	pMainCam->Camera()->SetLayerAllCheck();
	pMainCam->Camera()->SetLayerCheck( 30, false );
	pMainCam->Camera()->SetLayerCheck( 29, false );
	pMainCam->GetScript<CFPSCamScript>()->SetPlayer(pPlayer);
	m_pScene->FindLayer( L"Default" )->AddGameObject( pMainCam );
	m_pPlayer->GetScript<CPlayerScript>()->SetMainCamera(pMainCam->Camera());

	// ====================
	// Player Camera
	// ====================
	CGameObject* pPlayerCam = new CGameObject;
	pPlayerCam->AddComponent(new CTransform);
	pPlayerCam->AddComponent(new CCamera);
	pPlayerCam->AddComponent(new CPlayerCamScript);
	pPlayerCam->GetScript<CPlayerCamScript>()->SetPlayer(pPlayer);
	
	//pPlayerCam->Transform()->SetLocalPos(Vec3(0.f, 25.f, 150.f));
	pPlayerCam->Transform()->SetLocalRot(Vec3(0.f, XM_PI, 0.f));

	pPlayerCam->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
	pPlayerCam->Camera()->SetFar(10000.f);
	pPlayerCam->Camera()->SetLayerCheck(1, true);
	pPlayerCam->Camera()->SetCamType(CAM_TYPE::INVENTORY);

	m_pScene->FindLayer(L"Default")->AddGameObject(pPlayerCam);

	// ====================
	// UI Camera
	// ====================
	CGameObject* pUICam = new CGameObject;
	pUICam->AddComponent( new CTransform );
	pUICam->AddComponent( new CCamera );

	pUICam->Camera()->SetProjType( PROJ_TYPE::ORTHGRAPHIC );
	pUICam->Camera()->SetFar( 10000.f );
	pUICam->Camera()->SetLayerCheck( 30, true );
	pUICam->Camera()->SetWidth(CRenderMgr::GetInst()->GetResolution().fWidth);
	pUICam->Camera()->SetHeight(CRenderMgr::GetInst()->GetResolution().fHeight);

	m_pScene->FindLayer(L"Default")->AddGameObject(pUICam);

	CreatePlayerStatusUI();
	CreateInventoryUI();
	CreateChatUI();
	CreateItemUI();
	m_pMRT = CSceneMgr::GetInst()->CreateMRTUI();
	tEvent evt = {};
	evt.eType = EVENT_TYPE::TRANSFER_LAYER;
	evt.wParam = (DWORD_PTR)m_pMRT;
	evt.lParam = ((DWORD_PTR)29 << 16 | (DWORD_PTR)true);
	CEventMgr::GetInst()->AddEvent(evt);

	// ====================
	// 3D Light Object 추가
	// ====================
	pObject = new CGameObject;
	pObject->SetName( L"Sunshine" );
	pObject->AddComponent( new CTransform );
	pObject->AddComponent( new CLight3D );
	pObject->AddComponent( new CSunshineScript );

	pObject->Light3D()->SetLightPos(Vec3(0.f, 500.f, 0.f));
	pObject->Light3D()->SetLightType(LIGHT_TYPE::DIR);
	pObject->Light3D()->SetDiffuseColor(Vec3(1.f, 1.f, 1.f));
	pObject->Light3D()->SetSpecular(Vec3(0.3f, 0.3f, 0.3f));
	pObject->Light3D()->SetAmbient(Vec3(0.1f, 0.1f, 0.1f));
	pObject->Light3D()->SetLightDir(Vec3(1.f, -1.f, 1.f));
	pObject->Light3D()->SetLightRange(1000.f);

	pObject->Transform()->SetLocalPos(Vec3(-1000.f, 1000.f, 1000.f));

	pObject->GetScript<CSunshineScript>()->SetPlayer(m_pPlayer);
	pObject->GetScript<CSunshineScript>()->Init();

	m_pScene->FindLayer( L"Default" )->AddGameObject( pObject );

	CGameObject* pSun = pObject;
	CCheatMgr::GetInst()->SetClock(pSun);

	// ====================
	// Skybox 오브젝트 생성
	// ====================
	pObject = new CGameObject;
	pObject->SetName( L"SkyBox" );
	pObject->FrustumCheck( false );
	pObject->AddComponent( new CTransform );
	pObject->AddComponent( new CMeshRender );

	// MeshRender 설정
	pObject->MeshRender()->SetMesh( CResMgr::GetInst()->FindRes<CMesh>( L"SphereMesh" ) );
	pObject->MeshRender()->SetMaterial( CResMgr::GetInst()->FindRes<CMaterial>( L"SkyboxMtrl" ) );
	pObject->MeshRender()->GetSharedMaterial()->SetData( SHADER_PARAM::TEX_0, pSky01.GetPointer() );

	// AddGameObject
	m_pScene->FindLayer( L"Default" )->AddGameObject( pObject );
	pSun->GetScript<CSunshineScript>()->SetSkybox(pObject);

	// =======================
	// LandScape 오브젝트 생성
	// =======================
	Ptr<CTexture> pLandScape = CResMgr::GetInst()->Load<CTexture>( L"Grass", L"Texture\\LandScape\\mapTexture.png" );
	Ptr<CTexture> pLandScapeNormal = CResMgr::GetInst()->Load<CTexture>(L"LandScapeNormal", L"Texture\\ISLAND_NORMAL.png");

	pObject = new CGameObject;
	pObject->SetName( L"LandScape Object" );
	pObject->AddComponent( new CTransform );
	pObject->AddComponent( new CMeshRender );
	pObject->AddComponent( new CLandScape );
	pObject->LandScape()->CreateLandScape( L"Texture/ISLAND_110.bmp", 110, 110 );
	pObject->MeshRender()->SetMesh( CResMgr::GetInst()->FindRes<CMesh>( L"LandScapeMesh" ) );
	pObject->MeshRender()->SetMaterial( CResMgr::GetInst()->FindRes<CMaterial>( L"LandScapeMtrl" ) );
	pObject->MeshRender()->GetSharedMaterial()->SetData( SHADER_PARAM::TEX_0, pLandScape.GetPointer() );
	pObject->MeshRender()->GetSharedMaterial()->SetData( SHADER_PARAM::TEX_1, pLandScapeNormal.GetPointer() );
	pObject->Transform()->SetLocalPos( Vec3( 0.f, 0.f, 0.f ) );
	pObject->Transform()->SetLocalScale( Vec3( 200.f, 400.f, 200.f ) );
	pObject->FrustumCheck( false );
	CNaviMgr::GetInst()->SetLandScape( pObject->LandScape() );
	m_pScene->FindLayer( L"Default" )->AddGameObject( pObject );
	
	CreateNatural();
#ifdef NETWORK_ON
#else
	CreateAnimalSpawner();
#endif // NETWORK_ON
	CreateShowFPS();

	// ==========================
	// Distortion Object 만들기
	// ==========================
	pObject = new CGameObject;
	pObject->SetName(L"Sea");
	pObject->FrustumCheck(false);

	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);

	// Material 값 셋팅
	Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"AdvancedWaterMtrl");

	float tessellation = 1024.f;

	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMaterial(pMtrl);
	pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::FLOAT_0, &tessellation);

	float fHeight = 80.f;
	pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::FLOAT_1, &fHeight);
	float fCull = 0.2f;
	pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::FLOAT_2, &fCull);

	pObject->Transform()->SetLocalPos(Vec3(10000.f, 80.f, 10000.f));
	pObject->Transform()->SetLocalScale(Vec3(50000.f, 50000.f, 1.f));
	pObject->Transform()->SetLocalRot(Vec3(XM_PI / 2.f, 0.f, 0.f));

	m_pScene->FindLayer(L"Environment")->AddGameObject(pObject);
	pSun->GetScript<CSunshineScript>()->SetSea(pObject);

	//====================
	//Particle Object 생성
	//====================
	pObject = new CGameObject;
	pObject->SetName( L"Rain" );
	pObject->AddComponent( new CTransform );
	pObject->AddComponent( new CParticleSystem );
	pObject->SetActive( false );

	pObject->FrustumCheck( false );
	pObject->ParticleSystem()->SetKind( 1 );
	pObject->ParticleSystem()->SetStartColor( Vec4( 0.7f, 0.7f, 0.7f, 0.7f ) );
	pObject->ParticleSystem()->SetEndColor( Vec4( 0.5f, 0.6f, 0.7f, 0.8f ) );
	pObject->ParticleSystem()->SetStartScale( 10.f );
	pObject->ParticleSystem()->SetEndScale( 20.f );
	pObject->ParticleSystem()->SetStartSpeed( 200.f );
	pObject->ParticleSystem()->SetEndSpeed( 200.f );
	pObject->ParticleSystem()->SetTexture( L"Rain", L"Texture\\Particle\\HardRain.png" );
	
	m_pScene->FindLayer( L"Default" )->AddGameObject( pObject );
	pSun->GetScript<CSunshineScript>()->SetRain( pObject );

	//==========================
	//비올 때 카메라에 빗물 생성
	//==========================
	Ptr<CTexture> pRainDrop = CResMgr::GetInst()->Load<CTexture>( L"RainDrop", L"Texture\\raindrop3.png" );
	pObject = new CGameObject;
	pObject->SetName( L"Particle" );
	pObject->AddComponent( new CTransform );
	pObject->AddComponent( new CRainScript );
	pObject->AddComponent( new CMeshRender );

	pObject->MeshRender()->SetMesh( CResMgr::GetInst()->FindRes<CMesh>( L"RectMesh" ) );
	pObject->MeshRender()->SetMaterial( CResMgr::GetInst()->FindRes<CMaterial>( L"RainDropMtrl" ) );
	pObject->Transform()->SetLocalPos( Vec3( 0.f, 0.f, 3000.f ) );
	tResolution vResolution = CRenderMgr::GetInst()->GetResolution();
	pObject->Transform()->SetLocalScale( Vec3( vResolution.fWidth, vResolution.fHeight, 1.f ) );

	pObject->MeshRender()->GetCloneMaterial()->SetData( SHADER_PARAM::TEX_0, pRainDrop.GetPointer() );
	m_pScene->FindLayer( L"UI" )->AddGameObject( pObject );

	// =============
	// FBX 파일 로드
	// =============
	//Ptr<CMeshData> pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\monster.fbx");
	//pMeshData->Save(pMeshData->GetPath());

	//// MeshData 로드
	/*Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\monster.mdat", L"MeshData\\monster.mdat");

	pObject = pMeshData->Instantiate();
	pObject->SetName(L"House");
	pObject->FrustumCheck(false);
	pObject->Transform()->SetLocalPos(Vec3(0.f, 600.f, 0.f));
	pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	m_pScene->FindLayer(L"Monster")->AddGameObject(pObject);*/

	// =================================
	// CollisionMgr 충돌 그룹(Layer) 지정
	// =================================
	// Player Layer 와 Monster Layer 는 충돌 검사 진행
#ifdef NETWORK_ON
	CCollisionMgr::GetInst()->CheckCollisionLayer(L"Player", L"Animal");
	CCollisionMgr::GetInst()->CheckCollisionLayer(L"Player", L"Environment");
	CCollisionMgr::GetInst()->CheckCollisionLayer(L"Player", L"House");
	CCollisionMgr::GetInst()->CheckCollisionLayer(L"Player", L"Human");

	//
	CCollisionMgr::GetInst()->CheckCollisionLayer(L"Arrow", L"Animal");
	CCollisionMgr::GetInst()->CheckCollisionLayer(L"Arrow", L"House");
#else
	CCollisionMgr::GetInst()->CheckCollisionLayer(L"Player", L"Animal");
	CCollisionMgr::GetInst()->CheckCollisionLayer(L"Player", L"Environment");
	CCollisionMgr::GetInst()->CheckCollisionLayer(L"Player", L"House");
	CCollisionMgr::GetInst()->CheckCollisionLayer(L"Player", L"Human");
	//
	CCollisionMgr::GetInst()->CheckCollisionLayer(L"Player", L"Animal");
	CCollisionMgr::GetInst()->CheckCollisionLayer(L"Player", L"Environment");
	CCollisionMgr::GetInst()->CheckCollisionLayer(L"Player", L"House");
	CCollisionMgr::GetInst()->CheckCollisionLayer(L"Player", L"Human");
	//
	CCollisionMgr::GetInst()->CheckCollisionLayer( L"Animal", L"Environment" );
	//
	CCollisionMgr::GetInst()->CheckCollisionLayer( L"Arrow", L"Animal" );
	CCollisionMgr::GetInst()->CheckCollisionLayer( L"Arrow", L"House" );
	//
	CCollisionMgr::GetInst()->CheckCollisionLayer(L"Build", L"Animal");
	CCollisionMgr::GetInst()->CheckCollisionLayer(L"Build", L"Environment");
	CCollisionMgr::GetInst()->CheckCollisionLayer(L"Build", L"House");
	CCollisionMgr::GetInst()->CheckCollisionLayer(L"Build", L"Human");
#endif // NETWORK_ON
	

	GiveStartItem();
	ShowCursor(m_bShowCursor);
}

void CIngameScene::GiveStartItem()
{
	CItemScript* pItem = new CStuffScript(ITEM_TYPE::ITEM_WOOD);
	m_pInventory->GetScript<CInventoryScript>()->AddItem(pItem, 100);
	pItem = new CStuffScript(ITEM_TYPE::ITEM_STONE);
	m_pInventory->GetScript<CInventoryScript>()->AddItem(pItem, 100);
	pItem = new CStuffScript(ITEM_TYPE::ITEM_BONE);
	m_pInventory->GetScript<CInventoryScript>()->AddItem(pItem, 100);
	pItem = new CStuffScript(ITEM_TYPE::ITEM_LEATHER);
	m_pInventory->GetScript<CInventoryScript>()->AddItem(pItem, 100);
	pItem = new CUsableScript(ITEM_TYPE::ITEM_WATER_BOTTLE);
	m_pInventory->GetScript<CInventoryScript>()->AddItem(pItem, 5);
	pItem = new CUsableScript(ITEM_TYPE::ITEM_EMPTY_BOTTLE);
	m_pInventory->GetScript<CInventoryScript>()->AddItem(pItem, 5);
	pItem = new CToolItemScript(ITEM_TYPE::ITEM_WOODCLUB);
	m_pInventory->GetScript<CInventoryScript>()->AddItem(pItem, 1);
	pItem = new CUsableScript(ITEM_TYPE::ITEM_CHOCOLATEBAR);
	m_pInventory->GetScript<CInventoryScript>()->AddItem(pItem, 5);
	pItem = new CUsableScript(ITEM_TYPE::ITEM_MEDICKIT);
	m_pInventory->GetScript<CInventoryScript>()->AddItem(pItem, 5);
	pItem = new CStuffScript(ITEM_TYPE::ITEM_CLOTH);
	m_pInventory->GetScript<CInventoryScript>()->AddItem(pItem, 100);
}

void CIngameScene::Update()
{
	if (KEY_TAB(KEY_TYPE::KEY_ESC))
	{
		for (UINT i = 0; i < MAX_LAYER; ++i)
		{
			CSceneMgr::GetInst()->GetCurScene()->GetLayer(i)->RemoveAll();
		}
		PostQuitMessage(0);
		return;
	}

	if (KEY_TAB(KEY_TYPE::KEY_ENTER))
	{
		if (m_pChat)
		{
			if (m_pChat->GetScript<CInputScript>()->GetEnable() && !m_pInventory->GetScript<CInventoryScript>()->GetInventoryActive()) 
			{
				
				string str = m_pChat->GetScript<CInputScript>()->GetString();
#ifdef NETWORK_ON
				CPacketMgr::GetInst()->Send_Chat_Packet(str);
				m_pChat->GetScript<CInputScript>()->SetEnable(false);
#else
				string strPlayerName = "Player";
				m_pChat->GetScript<CChatScript>()->AddChat(strPlayerName, str);
				m_pChat->GetScript<CInputScript>()->SetEnable(false);
				m_pChat->GetScript<CInputScript>()->Clear();
#endif // NETWORK_ON
			}
			else if(!m_pChat->GetScript<CInputScript>()->GetEnable() && !m_pInventory->GetScript<CInventoryScript>()->GetInventoryActive())
				m_pChat->GetScript<CInputScript>()->SetEnable(true);
		}
	}

	if (KEY_TAB(KEY_TYPE::KEY_I))
	{
		if (!m_pChat->GetScript<CInputScript>()->GetEnable())
		{
			m_pInventory->GetScript<CInventoryScript>()->Show();
			m_bShowCursor = !m_bShowCursor;
			ShowCursor(m_bShowCursor);
			if (!m_bShowCursor)
			{
				tResolution vResolution = CRenderMgr::GetInst()->GetResolution();
				Vec2 vCenter = Vec2(vResolution.fWidth / 2.f, vResolution.fHeight / 2.f);
				SetCursorPos(vCenter.x, vCenter.y);
			}
		}
	}

	if (KEY_TAB(KEY_TYPE::KEY_1))
	{
		if (!m_pChat->GetScript<CInputScript>()->GetEnable() && !m_pInventory->GetScript<CInventoryScript>()->GetInventoryActive())
		{
			m_pQuickSlot->GetScript<CQuickSlotScript>()->KeyInput(1);
			if (m_iSelect != 1)
			{
				m_pPlayer->GetScript<CPlayerScript>()->DisableItem(m_iSelect - 1);
				m_iSelect = 1;
				m_pPlayer->GetScript<CPlayerScript>()->EnableItem(m_iSelect - 1);
				m_bOverlapItem = false;
			}
			else
			{
				if (m_bOverlapItem)
				{
					m_pPlayer->GetScript<CPlayerScript>()->EnableItem(m_iSelect - 1);
					m_bOverlapItem = false;
				}
				else
				{
					m_pPlayer->GetScript<CPlayerScript>()->DisableItem(m_iSelect - 1);
					m_bOverlapItem = true;
				}
			}
		}
	}
	else if (KEY_TAB(KEY_TYPE::KEY_2))
	{
		if (!m_pChat->GetScript<CInputScript>()->GetEnable() && !m_pInventory->GetScript<CInventoryScript>()->GetInventoryActive())
		{
			m_pQuickSlot->GetScript<CQuickSlotScript>()->KeyInput(2);
			if (m_iSelect != 2)
			{
				m_pPlayer->GetScript<CPlayerScript>()->DisableItem(m_iSelect - 1);
				m_iSelect = 2;
				m_pPlayer->GetScript<CPlayerScript>()->EnableItem(m_iSelect - 1);
				m_bOverlapItem = false;
			}
			else
			{
				if (m_bOverlapItem)
				{
					m_pPlayer->GetScript<CPlayerScript>()->EnableItem(m_iSelect - 1);
					m_bOverlapItem = false;
				}
				else
				{
					m_pPlayer->GetScript<CPlayerScript>()->DisableItem(m_iSelect - 1);
					m_bOverlapItem = true;
				}
			}
		}
	}
	else if (KEY_TAB(KEY_TYPE::KEY_3))
	{
		if (!m_pChat->GetScript<CInputScript>()->GetEnable() && !m_pInventory->GetScript<CInventoryScript>()->GetInventoryActive())
		{
			m_pQuickSlot->GetScript<CQuickSlotScript>()->KeyInput(3);
			if (m_iSelect != 3)
			{
				m_pPlayer->GetScript<CPlayerScript>()->DisableItem(m_iSelect - 1);
				m_iSelect = 3;
				m_pPlayer->GetScript<CPlayerScript>()->EnableItem(m_iSelect - 1);
				m_bOverlapItem = false;
			}
			else
			{
				if (m_bOverlapItem)
				{
					m_pPlayer->GetScript<CPlayerScript>()->EnableItem(m_iSelect - 1);
					m_bOverlapItem = false;
				}
				else
				{
					m_pPlayer->GetScript<CPlayerScript>()->DisableItem(m_iSelect - 1);
					m_bOverlapItem = true;
				}
			}
		}
	}
	else if (KEY_TAB(KEY_TYPE::KEY_4))
	{
		if (!m_pChat->GetScript<CInputScript>()->GetEnable() && !m_pInventory->GetScript<CInventoryScript>()->GetInventoryActive())
		{
			m_pQuickSlot->GetScript<CQuickSlotScript>()->KeyInput(4);
			if (m_iSelect != 4)
			{
				m_pPlayer->GetScript<CPlayerScript>()->DisableItem(m_iSelect - 1);
				m_iSelect = 4;
				m_pPlayer->GetScript<CPlayerScript>()->EnableItem(m_iSelect - 1);
				m_bOverlapItem = false;
			}
			else
			{
				if (m_bOverlapItem)
				{
					m_pPlayer->GetScript<CPlayerScript>()->EnableItem(m_iSelect - 1);
					m_bOverlapItem = false;
				}
				else
				{
					m_pPlayer->GetScript<CPlayerScript>()->DisableItem(m_iSelect - 1);
					m_bOverlapItem = true;
				}
			}
		}
	}
	else if (KEY_TAB(KEY_TYPE::KEY_5))
	{
		if (!m_pChat->GetScript<CInputScript>()->GetEnable() && !m_pInventory->GetScript<CInventoryScript>()->GetInventoryActive())
		{
			m_pQuickSlot->GetScript<CQuickSlotScript>()->KeyInput(5);
			if (m_iSelect != 5)
			{
				m_pPlayer->GetScript<CPlayerScript>()->DisableItem(m_iSelect - 1);
				m_iSelect = 5;
				m_pPlayer->GetScript<CPlayerScript>()->EnableItem(m_iSelect - 1);
				m_bOverlapItem = false;
			}
			else
			{
				if (m_bOverlapItem)
				{
					m_pPlayer->GetScript<CPlayerScript>()->EnableItem(m_iSelect - 1);
					m_bOverlapItem = false;
				}
				else
				{
					m_pPlayer->GetScript<CPlayerScript>()->DisableItem(m_iSelect - 1);
					m_bOverlapItem = true;
				}
			}
		}
	}

	if (KEY_TAB(KEY_TYPE::KEY_P))
	{
		if (m_bShowMRT)
		{
			tEvent evt = {};

			evt.eType = EVENT_TYPE::TRANSFER_LAYER;
			evt.wParam = (DWORD_PTR)m_pMRT;
			evt.lParam = ((DWORD_PTR)30 << 16 | (DWORD_PTR)true);

			CEventMgr::GetInst()->AddEvent(evt);
		}
		else
		{
			tEvent evt = {};

			evt.eType = EVENT_TYPE::TRANSFER_LAYER;
			evt.wParam = (DWORD_PTR)m_pMRT;
			evt.lParam = ((DWORD_PTR)29 << 16 | (DWORD_PTR)true);

			CEventMgr::GetInst()->AddEvent(evt);
		}
		m_bShowMRT = !m_bShowMRT;
	}

	if (KEY_TAB(KEY_TYPE::KEY_O))
	{
		if (m_bShowFPS)
		{
			tEvent evt = {};

			evt.eType = EVENT_TYPE::TRANSFER_LAYER;
			evt.wParam = (DWORD_PTR)m_pFPSInfo;
			evt.lParam = ((DWORD_PTR)30 << 16 | (DWORD_PTR)true);

			CEventMgr::GetInst()->AddEvent(evt);
		}
		else
		{
			tEvent evt = {};

			evt.eType = EVENT_TYPE::TRANSFER_LAYER;
			evt.wParam = (DWORD_PTR)m_pFPSInfo;
			evt.lParam = ((DWORD_PTR)29 << 16 | (DWORD_PTR)true);

			CEventMgr::GetInst()->AddEvent(evt);
		}
		m_bShowFPS = !m_bShowFPS;
	}
}

void CIngameScene::CreateQuickSlotUI(CGameObject* _pInventory)
{
	// ===================
	// UI 오브젝트 생성
	// ===================
	// UI QuickSlot BackGround
	CGameObject* pObject = new CGameObject;
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);

	tResolution vResolution = CRenderMgr::GetInst()->GetResolution();
	Vec3 vPos = Vec3(0.f, vResolution.fHeight / -2.f + 80.f, 1000.f);

	pObject->Transform()->SetLocalPos(vPos);
	pObject->Transform()->SetLocalScale(Vec3(500.f, 80.f, 1.f));

	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIMtrl"));

	int a = 1;
	pObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::INT_0, &a);
	pObject->AddComponent(new CQuickSlotScript);

	m_pScene->FindLayer(L"UI")->AddGameObject(pObject);

	// UI QuickSlot slot
	for (int i = 0; i < 5; ++i)
	{
		CGameObject* pChildObject = new CGameObject;
		pChildObject->AddComponent(new CTransform);
		pChildObject->AddComponent(new CMeshRender);

		pChildObject->Transform()->SetLocalPos(Vec3(-200.f + (i * 100.f), vPos.y, 800.f));
		pChildObject->Transform()->SetLocalScale(Vec3(75.f, 75.f, 1.f));

		pChildObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		pChildObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"HighUIMtrl"));

		Vec4 vColor = Vec4(0.7f, 0.7f, 0.7f, 1.f);
		pChildObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::VEC4_0, &vColor);

		pObject->GetScript<CQuickSlotScript>()->AddQuickSlot(pChildObject);
		m_pScene->FindLayer(L"UI")->AddGameObject(pChildObject);
		_pInventory->GetScript<CInventoryScript>()->AddSlot(pChildObject);
	}


	Ptr<CTexture> pFont = CFontMgr::GetInst()->GetFontTex();
	for (int i = 0; i < 5; ++i)
	{
		CGameObject* pChildObject = new CGameObject;
		pChildObject->AddComponent(new CTransform);
		pChildObject->AddComponent(new CMeshRender);

		//pChildObject->Transform()->SetLocalPos(Vec3(-190.f + (i * 95.f), -330.f, 500.f));
		pChildObject->Transform()->SetLocalPos(Vec3(-0.4f + (i * 0.2f), -0.6f, -500.f));
		pChildObject->Transform()->SetLocalScale(Vec3(20.f / 500.f, 20.f / 80.f, 1.f));

		pChildObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		pChildObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"FontMtrl"));

		pChildObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, pFont.GetPointer());

		// 추후에 폰트 정보를 파일입출력으로 받아온 후
		// 각 글자에 따라서 정보를 찍어주면 됨
		// 49
		CharInfo tInfo = CFontMgr::GetInst()->GetFontInfo().mCharInfo[(char)i + 49];
		float sizeX = (float)CFontMgr::GetInst()->GetFontInfo().iScaleX;
		float sizeY = (float)CFontMgr::GetInst()->GetFontInfo().iScaleY;
		float startU = tInfo.ix / sizeX;
		float startV = tInfo.iy / sizeY;
		float widthU = tInfo.iWidth / sizeX;
		float heightV = tInfo.iHeight / sizeY;
		Vec4 fontColor = Vec4(1.f, 1.f, 1.f, 1.f);
		Vec4 fontBackColor = Vec4(0.4f, 0.4f, 0.8f, 1.f);

		pChildObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::FLOAT_0, &startU);
		pChildObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::FLOAT_1, &widthU);
		pChildObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::FLOAT_2, &startV);
		pChildObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::FLOAT_3, &heightV);

		pChildObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::VEC4_0, &fontColor);
		pChildObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::VEC4_1, &fontBackColor);

		pObject->AddChild(pChildObject);
		m_pScene->FindLayer(L"UI")->AddGameObject(pChildObject);
	}
	m_pQuickSlot = pObject;
	m_pPlayer->GetScript<CPlayerScript>()->SetQuickSlot(m_pQuickSlot->GetScript<CQuickSlotScript>());
}

void CIngameScene::CreatePlayerStatusUI()
{
	Ptr<CTexture> pUITex = CResMgr::GetInst()->Load<CTexture>(L"UITex", L"Texture\\Image2.png");
	Ptr<CTexture> pTexHealth = CResMgr::GetInst()->Load<CTexture>(L"HealthIconTex", L"Texture\\Health01.png");
	Ptr<CTexture> pTexHungry = CResMgr::GetInst()->Load<CTexture>(L"HungryIconTex", L"Texture\\Hungry01.png");
	Ptr<CTexture> pTexThirst = CResMgr::GetInst()->Load<CTexture>(L"ThirstIconTex", L"Texture\\Thirst01.png");

	//
	// PlayerStatus BackGround
	CGameObject* pObject = new CGameObject;
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->SetName(L"Player Status");

	tResolution vResolution = CRenderMgr::GetInst()->GetResolution();
	Vec3 vPos = Vec3(vResolution.fWidth / 2.f - 150.f, vResolution.fHeight / -2.f + 100.f, 2000.f);

	pObject->Transform()->SetLocalPos(vPos);
	pObject->Transform()->SetLocalScale(Vec3(250.f, 135.f, 1.f));

	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"HighUIMtrl");
	pObject->MeshRender()->SetMaterial(pMtrl->Clone());
	Vec4 vColor = Vec4(0.5f, 0.5f, 0.5f, 1.f);
	pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::VEC4_0, &vColor);

	pObject->AddComponent(new CStatusScript);

	m_pScene->FindLayer(L"UI")->AddGameObject(pObject);
	m_pPlayer->GetScript<CPlayerScript>()->SetStatusObject(pObject);

	// PlayerStatus HealthBar
	CGameObject* pChildObject = new CGameObject;
	pChildObject->AddComponent(new CTransform);
	pChildObject->AddComponent(new CMeshRender);
	//pChildObject->Transform()->SetLocalPos(Vec3(510.f, -260.f, 1000.f));
	pChildObject->Transform()->SetLocalPos(Vec3(0.075f, 0.3f, -1000.f));
	pChildObject->Transform()->SetLocalScale(Vec3(190.f / 250.f, 30.f / 135.f, 1.f));
	pChildObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pChildObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIMtrl"));
	int a = 5; // Green
	pChildObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::INT_0, &a);
	pObject->AddChild(pChildObject);
	m_pScene->FindLayer(L"UI")->AddGameObject(pChildObject);

	// PlayerStatus HungryBar
	pChildObject = new CGameObject;
	pChildObject->AddComponent(new CTransform);
	pChildObject->AddComponent(new CMeshRender);
	//pChildObject->Transform()->SetLocalPos(Vec3(510.f, -300.f, 1000.f));
	pChildObject->Transform()->SetLocalPos(Vec3(0.075f, 0.f, -1000.f));
	pChildObject->Transform()->SetLocalScale(Vec3(190.f / 250.f, 30.f / 135.f, 1.f));
	pChildObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pChildObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIMtrl"));
	a = 6; // Brown
	pChildObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::INT_0, &a);
	pObject->AddChild(pChildObject);
	m_pScene->FindLayer(L"UI")->AddGameObject(pChildObject);
	// PlayerStatus ThirstBar
	pChildObject = new CGameObject;
	pChildObject->AddComponent(new CTransform);
	pChildObject->AddComponent(new CMeshRender);
	pChildObject->Transform()->SetLocalPos(Vec3(0.075f, -0.3f, -1000.f));
	pChildObject->Transform()->SetLocalScale(Vec3(190.f / 250.f, 30.f / 135.f, 1.f));
	pChildObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pChildObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIMtrl"));
	a = 3; // Blue
	pChildObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::INT_0, &a);
	pObject->AddChild(pChildObject);
	m_pScene->FindLayer(L"UI")->AddGameObject(pChildObject);
	// PlayerStatus ArmorBar
	pChildObject = new CGameObject;
	pChildObject->AddComponent(new CTransform);
	pChildObject->AddComponent(new CMeshRender);
	pChildObject->Transform()->SetLocalPos(Vec3(0.075f, 0.3f, -1000.f));
	pChildObject->Transform()->SetLocalScale(Vec3(190.f / 250.f, 30.f / 135.f, 1.f));
	pChildObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pChildObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"HighUIMtrl")->Clone());
	Vec4 vColor2 = Vec4(0.25f, 0.85f, 0.7f, 1.f);
	pChildObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::VEC4_0, &vColor2);
	pObject->AddChild(pChildObject);
	m_pScene->FindLayer(L"UI")->AddGameObject(pChildObject);

	// =================================
	pChildObject = new CGameObject;
	pChildObject->SetName(L"Screen Damage");
	pChildObject->AddComponent(new CTransform);
	pChildObject->AddComponent(new CMeshRender);

	// Transform 설정
	pChildObject->Transform()->SetLocalPos(Vec3(0.f, 0.f, 2000.f));
	pChildObject->Transform()->SetLocalScale(Vec3(vResolution.fWidth, vResolution.fHeight, 1.f));

	// MeshRender 설정
	pChildObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pChildObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIMtrl"));
	pChildObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, pUITex.GetPointer());
	float health = 1.f;
	pChildObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::FLOAT_0, &health);
	// AddGameObject
	pObject->GetScript<CStatusScript>()->SetScreenDamage(pChildObject);
	m_pScene->FindLayer(L"UI")->AddGameObject(pChildObject);

	//
	// Health Icon
	pObject = new CGameObject;
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);

	vPos = Vec3(vResolution.fWidth / 2.f - 250.f, vResolution.fHeight / -2.f + 140.f, 1.f);

	pObject->Transform()->SetLocalPos(vPos);
	pObject->Transform()->SetLocalScale(Vec3(30.f, 30.f, 1.f));

	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"IconMtrl"));

	pObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, pTexHealth.GetPointer());
	float fa = 1.f;

	pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::FLOAT_0, &fa);

	m_pScene->FindLayer(L"UI")->AddGameObject(pObject);
	//
	// Hungry Icon
	pObject = new CGameObject;
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);

	vPos.y -= 40.f;

	pObject->Transform()->SetLocalPos(vPos);
	pObject->Transform()->SetLocalScale(Vec3(30.f, 30.f, 1.f));

	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"IconMtrl"));

	pObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, pTexHungry.GetPointer());

	pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::FLOAT_0, &fa);

	m_pScene->FindLayer(L"UI")->AddGameObject(pObject);
	//
	// Thirst Icon
	pObject = new CGameObject;
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);

	vPos.y -= 40.f;

	pObject->Transform()->SetLocalPos(vPos);
	pObject->Transform()->SetLocalScale(Vec3(30.f, 30.f, 1.f));

	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"IconMtrl"));

	pObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, pTexThirst.GetPointer());

	pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::FLOAT_0, &fa);

	m_pScene->FindLayer(L"UI")->AddGameObject(pObject);
}

void CIngameScene::CreateInventoryUI()
{
	Ptr<CTexture> PlayerTex = CResMgr::GetInst()->FindRes<CTexture>(L"PlayerTex");

	CGameObject* pInventory = new CGameObject;
	pInventory->SetName(L"Inventory Object");

	pInventory->AddComponent(new CTransform);
	pInventory->AddComponent(new CInventoryScript);

	pInventory->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));

	pInventory->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));

	// AddGameObject
	m_pScene->FindLayer(L"UI")->AddGameObject(pInventory);

	CreateQuickSlotUI(pInventory);

	{
		CGameObject* pObject = new CGameObject;
		pObject->SetName(L"Inventory Player Object");

		pObject->AddComponent(new CTransform);
		pObject->AddComponent(new CMeshRender);

		pObject->Transform()->SetLocalPos(Vec3(-450.f, 0.f, 500.f));

		Vec3 vScale(450.f, 450.f, 1.f);
		pObject->Transform()->SetLocalScale(vScale);

		// MeshRender 설정
		pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"AlphaTexMtrl");
		pObject->MeshRender()->SetMaterial(pMtrl);
		pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, PlayerTex.GetPointer());

		// AddGameObject
		pInventory->AddChild(pObject);
		m_pScene->FindLayer(L"Invisible")->AddGameObject(pObject);
	}

	{
		CGameObject* pObject = new CGameObject;
		pObject->SetName(L"Inventory BackGround");

		pObject->AddComponent(new CTransform);
		pObject->AddComponent(new CMeshRender);

		pObject->Transform()->SetLocalPos(Vec3(0.f, 0.f, 2000.f));

		tResolution vResolution = CRenderMgr::GetInst()->GetResolution();
		Vec3 vScale(vResolution.fWidth, vResolution.fHeight, 1.f);
		pObject->Transform()->SetLocalScale(vScale);

		// MeshRender 설정
		pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"HighUIMtrl");
		pObject->MeshRender()->SetMaterial(pMtrl->Clone());
		Vec4 vColor = Vec4(0.f, 0.f, 0.f, 0.8f);
		pObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::VEC4_0, &vColor);

		// AddGameObject
		pInventory->AddChild(pObject);
		m_pScene->FindLayer(L"Invisible")->AddGameObject(pObject);
	}

	{
		for (int j = 0; j < 4; ++j)
		{
			for(int i = 0; i < 5; ++i)
			{
				CGameObject* pItemSlot = new CGameObject;
				wstring strItem = L"ItemSlot_" + char(i) + char(j);

				pItemSlot->SetName(strItem);

				pItemSlot->AddComponent(new CTransform);
				pItemSlot->AddComponent(new CMeshRender);

				pItemSlot->Transform()->SetLocalPos(Vec3(-120.f + i * 90.f, 100.f - (j * 90.f), 200.f));

				Vec3 vScale(80.f, 80.f, 1.f);
				pItemSlot->Transform()->SetLocalScale(vScale);

				// MeshRender 설정
				pItemSlot->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
				Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"UIMtrl");
				pItemSlot->MeshRender()->SetMaterial(pMtrl);
				int color = 2; // White
				pItemSlot->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::INT_0, &color);

				// AddGameObject
				pInventory->AddChild(pItemSlot);
				m_pScene->FindLayer(L"Invisible")->AddGameObject(pItemSlot);
				pInventory->GetScript<CInventoryScript>()->AddSlot(pItemSlot);
			}
		}

		CGameObject* pObject = new CGameObject;
		pObject->SetName(L"Inventory ItemSlot BackGround");

		pObject->AddComponent(new CTransform);
		pObject->AddComponent(new CMeshRender);

		pObject->Transform()->SetLocalPos(Vec3(0.f, 0.f, 1000.f));

		Vec3 vScale(600.f, 450.f, 1.f);
		pObject->Transform()->SetLocalScale(vScale);

		// MeshRender 설정
		pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"HighUIMtrl");
		pObject->MeshRender()->SetMaterial(pMtrl->Clone());
		Vec4 vColor = Vec4(0.2f, 0.2f, 0.2f, 1.f);
		pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::VEC4_0, &vColor);

		// AddGameObject
		pInventory->AddChild(pObject);
		m_pScene->FindLayer(L"Invisible")->AddGameObject(pObject);
	}

	{
		CGameObject* pObject = new CGameObject;
		pObject->SetName(L"Crafting Item");

		pObject->AddComponent(new CTransform);
		pObject->AddComponent(new CMeshRender);

		pObject->Transform()->SetLocalPos(Vec3(470.f, 50.f, 800.f));

		Vec3 vScale(300.f, 550.f, 1.f);
		pObject->Transform()->SetLocalScale(vScale);

		// MeshRender 설정
		pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"HighUIMtrl");
		pObject->MeshRender()->SetMaterial(pMtrl);
		
		Vec4 vColor = Vec4(0.2f, 0.2f, 0.2f, 1.f);
		pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::VEC4_0, &vColor);

		// AddGameObject
		pInventory->AddChild(pObject);
		m_pScene->FindLayer(L"Invisible")->AddGameObject(pObject);
	}

	{
		CGameObject* pObject = new CGameObject;
		pObject->SetName(L"Inventory Player Background");

		pObject->AddComponent(new CTransform);
		pObject->AddComponent(new CMeshRender);

		pObject->Transform()->SetLocalPos(Vec3(-450.f, 0.f, 600.f));

		Vec3 vScale(250.f, 450.f, 1.f);
		pObject->Transform()->SetLocalScale(vScale);

		// MeshRender 설정
		pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"HighUIMtrl");
		pObject->MeshRender()->SetMaterial(pMtrl->Clone());

		Vec4 vColor = Vec4(0.35f, 0.35f, 0.35f, 1.f);
		pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::VEC4_0, &vColor);

		// AddGameObject
		pInventory->AddChild(pObject);
		m_pScene->FindLayer(L"Invisible")->AddGameObject(pObject);
	}

	pInventory->GetScript<CInventoryScript>()->Init();
	m_pInventory = pInventory;
	m_pInventory->GetScript<CInventoryScript>()->SetPlayer(m_pPlayer);
	m_pPlayer->GetScript<CPlayerScript>()->SetInventoryObject(m_pInventory);
}

void CIngameScene::CreateItemUI()
{
	CGameObject* pLootObject = new CGameObject;
	pLootObject->SetName(L"Item Loot Object");
	pLootObject->AddComponent(new CTransform);
	pLootObject->AddComponent(new CItemLootScript);
	pLootObject->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pLootObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	m_pScene->FindLayer(L"Invisible")->AddGameObject(pLootObject);

	CGameObject* pObject = new CGameObject;
	pObject->SetName(L"Item Loot Background");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);

	tResolution vResolution = CRenderMgr::GetInst()->GetResolution();
	Vec3 vPos = Vec3(vResolution.fWidth / 2.f - 150.f, vResolution.fHeight / -2.f + 200.f, 2000.f);

	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"HighUIMtrl");
	pObject->MeshRender()->SetMaterial(pMtrl->Clone());

	Vec4 vColor = Vec4(0.4f, 0.8f, 0.4f, 0.8f);
	pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::VEC4_0, &vColor);

	pObject->Transform()->SetLocalPos(vPos);
	pObject->Transform()->SetLocalScale(Vec3(250.f, 40.f, 1.f));

	pLootObject->GetScript<CItemLootScript>()->SetBackgroundObject(pObject);
	pLootObject->AddChild(pObject);
	m_pScene->FindLayer(L"Invisible")->AddGameObject(pObject);

	pObject = new CGameObject;
	pObject->SetName(L"Item Loot Icon");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);

	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"ItemMtrl");
	pObject->MeshRender()->SetMaterial(pMtrl->Clone());

	vPos.x -= 100.f;

	pObject->Transform()->SetLocalPos(Vec3(vPos.x, vPos.y, 1900.f));
	pObject->Transform()->SetLocalScale(Vec3(35.f, 35.f, 1.f));

	pLootObject->AddChild(pObject);
	pLootObject->GetScript<CItemLootScript>()->SetItemLootIcon(pObject->MeshRender()->GetSharedMaterial());
	m_pScene->FindLayer(L"Invisible")->AddGameObject(pObject);

	pObject = new CGameObject;
	pObject->SetName(L"Item Loot Name");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CFont);

	vPos.x += 120.f;
	pObject->Transform()->SetLocalPos(Vec3(vPos.x, vPos.y, 1900.f));
	pObject->Transform()->SetLocalScale(Vec3(180.f, 35.f, 1.f));

	pObject->Font()->SetFontColor(Vec4(1.f, 1.f, 1.f, 1.f));
	pObject->Font()->SetBackColor(Vec4(0.f, 0.f, 0.f, 0.f));

	pLootObject->AddChild(pObject);
	pLootObject->GetScript<CItemLootScript>()->SetItemLootFont(pObject->Font());
	m_pScene->FindLayer(L"Invisible")->AddGameObject(pObject);

	m_pInventory->GetScript<CInventoryScript>()->SetItemLootScript(pLootObject->GetScript<CItemLootScript>());
}

void CIngameScene::CreateChatUI()
{
	m_pChat = new CGameObject;
	m_pChat->SetName(L"Chat Object");
	m_pChat->AddComponent(new CTransform);
	m_pChat->AddComponent(new CFont);
	m_pChat->AddComponent(new CInputScript);
	m_pChat->AddComponent(new CChatScript);

	tResolution vResolution = CRenderMgr::GetInst()->GetResolution();
	Vec3 vPos = Vec3(vResolution.fWidth / -2.f + 180.f, vResolution.fHeight / -2.f + 60.f, 500.f);

	//m_pChat->Transform()->SetLocalPos(Vec3(-480.f, -355.f, 500.f));
	m_pChat->Transform()->SetLocalPos(vPos);
	m_pChat->Transform()->SetLocalScale(Vec3(300.f, 40.f, 1.f));
	m_pScene->FindLayer(L"UI")->AddGameObject(m_pChat);

	m_pChat->Font()->SetString(" ");
	m_pChat->GetScript<CInputScript>()->SetMaxCount(20);
	m_pChat->GetScript<CInputScript>()->SetEnable(false);
	
	vPos.y += 40.f;

	for (int i = 0; i < MAX_CHAT_LINE; ++i)
	{
		CGameObject* pObject = new CGameObject;
		pObject->SetName(L"ChatLog Bar");
		pObject->AddComponent(new CTransform);
		pObject->AddComponent(new CFont);
		pObject->Transform()->SetLocalPos(Vec3(vPos.x, vPos.y + (i * 30.f), 900.f));
		pObject->Transform()->SetLocalScale(Vec3(300.f, 30.f, 1000.f));
		pObject->Font()->SetBackColor(Vec4(0.5f, 0.5f, 0.5f, 0.3f));
		pObject->Font()->SetString(" ");
		m_pScene->FindLayer(L"UI")->AddGameObject(pObject);

		m_pChat->GetScript<CChatScript>()->AddChatObject(pObject, i);
	}
	m_pPlayer->GetScript<CPlayerScript>()->SetChatObject(m_pChat);
}

void CIngameScene::CreateNatural()
{
#ifdef NETWORK_ON
	for (int i = BEGIN_NATURAL; i < BEGIN_NATURAL + 306; ++i) {
		m_mapNatural.insert(make_pair(i, new CGameObject()));
	}
	FILE* pFile = NULL;

	wstring ResPath = CPathMgr::GetResPath();
	ResPath += L"Data\\Map.dat";
	string FullPath{ ResPath.begin(), ResPath.end() };

	fopen_s(&pFile, FullPath.c_str(), "r");

	int iSize = 0;
	fread(&iSize, sizeof(int), 1, pFile);

	for (int i = BEGIN_NATURAL; i < BEGIN_NATURAL + iSize; ++i)
	{
		wchar_t strName[MAX_PATH]{};
		size_t iLength = 0;
		fread(&iLength, sizeof(size_t), 1, pFile);
		fread(strName, sizeof(wchar_t), iLength, pFile);

		bool bNaturalScript;
		fread(&bNaturalScript, sizeof(bool), 1, pFile);

		if (bNaturalScript)
		{
			wchar_t strPath[MAX_PATH]{};
			fread(&iLength, sizeof(size_t), 1, pFile);
			fread(strPath, sizeof(wchar_t), iLength, pFile);

			if (iLength == 0)
				continue;

			Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(strPath, strPath);

			m_mapNatural[i] = pMeshData->Instantiate();
			m_mapNatural[i]->AddComponent(new CNaturalScript(NATURAL_TREE));
			m_mapNatural[i]->GetScript<CNaturalScript>()->LoadFromScene(pFile);
			m_mapNatural[i]->GetScript<CNaturalScript>()->SetIndex(i);

#ifdef CHECK_COLLISION
			m_mapNatural[i]->AddComponent(new CCollider2D);
			m_mapNatural[i]->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::SPHERE);
			m_mapNatural[i]->Collider2D()->SetOffsetPos(Vec3(0.f, 0.f, 60.f));
			m_mapNatural[i]->Collider2D()->SetOffsetScale(Vec3(1.7f, 1.7f, 1.7f));
#endif
		}

		m_mapNatural[i]->Transform()->LoadFromScene(pFile);
		m_mapNatural[i]->MeshRender()->SetDynamicShadow(true);

		string str1;
		wstring str2 = strName;
		str1.assign(str2.begin(), str2.end());

		m_mapNatural[i]->SetName(str2);
		if (str1 == "sprucea" || str1 == "sprucec")
		{
			m_mapNatural[i]->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"TreeMtrl"), 0);
		}
		else if (str1 == "spruceb")
		{
			m_mapNatural[i]->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"TreeMtrl"), 1);
		}
		else if (str1 == "plainsgrass")
		{
			m_mapNatural[i]->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"BushMtrl"), 0);
#ifdef CHECK_COLLISION
			m_mapNatural[i]->Collider2D()->SetOffsetPos(Vec3(0.f, 0.f, 20.f));
			m_mapNatural[i]->Collider2D()->SetOffsetScale(Vec3(100.f, 100.f, 100.f));
			//pObject->FrustumCheck(false);
#endif
		}
#ifdef CHECK_COLLISION
		else if (str1 == "mountainsrocks01")
		{
			m_mapNatural[i]->Collider2D()->SetOffsetPos(Vec3(20.f, 20.f, -40.f));
			m_mapNatural[i]->Collider2D()->SetOffsetScale(Vec3(350.f, 350.f, 350.f));
		}
		else if (str1 == "mountainsrocks02")
		{
			m_mapNatural[i]->Collider2D()->SetOffsetPos(Vec3(20.f, 0.f, 0.f));
			m_mapNatural[i]->Collider2D()->SetOffsetScale(Vec3(220.f, 220.f, 220.f));
		}
		else if (str1 == "genericcliffa")
		{
			m_mapNatural[i]->Collider2D()->SetOffsetPos(Vec3(0.f, 0.f, 20.f));
			m_mapNatural[i]->Collider2D()->SetOffsetScale(Vec3(10.f, 10.f, 10.f));
		}
		else if (str1 == "mountainsrocks01_a")
		{
			m_mapNatural[i]->Collider2D()->SetOffsetPos(Vec3(0.f, 0.f, 0.f));
			m_mapNatural[i]->Collider2D()->SetOffsetScale(Vec3(200.f, 200.f, 200.f));
		}
		else if (str1 == "mountainsrocks01_b")
		{
			m_mapNatural[i]->Collider2D()->SetOffsetPos(Vec3(0.f, 0.f, -60.f));
			m_mapNatural[i]->Collider2D()->SetOffsetScale(Vec3(220.f, 220.f, 220.f));
		}
		else if (str1 == "mountainsrocks01_c")
		{
			m_mapNatural[i]->Collider2D()->SetOffsetPos(Vec3(0.f, 0.f, 0.f));
			m_mapNatural[i]->Collider2D()->SetOffsetScale(Vec3(100.f, 100.f, 100.f));
		}
		else if (str1 == "mountainsrocks01_d")
		{
			m_mapNatural[i]->Collider2D()->SetOffsetPos(Vec3(0.f, 0.f, 0.f));
			m_mapNatural[i]->Collider2D()->SetOffsetScale(Vec3(100.f, 100.f, 100.f));
		}
		else
		{
			m_mapNatural[i]->Collider2D()->SetOffsetPos(Vec3(0.f, 0.f, 20.f));
			m_mapNatural[i]->Collider2D()->SetOffsetScale(Vec3(4.f, 4.f, 4.f));
		}
#endif
		/*CScene* pScene = CSceneMgr::GetInst()->GetCurScene();
		pScene->AddGameObject(L"Environment", m_mapNatural[i], false);*/
	}

	fclose(pFile);
#else
	FILE* pFile = NULL;

	wstring ResPath = CPathMgr::GetResPath();
	ResPath += L"Data\\Map.dat";
	string FullPath{ ResPath.begin(), ResPath.end() };

	fopen_s(&pFile, FullPath.c_str(), "r");

	int iSize = 0;
	fread(&iSize, sizeof(int), 1, pFile);

	for (int i = BEGIN_NATURAL; i < BEGIN_NATURAL + iSize; ++i)
	{
		CGameObject* pObject = nullptr;

		wchar_t strName[MAX_PATH]{};
		size_t iLength = 0;
		fread(&iLength, sizeof(size_t), 1, pFile);
		fread(strName, sizeof(wchar_t), iLength, pFile);

		bool bNaturalScript;
		fread(&bNaturalScript, sizeof(bool), 1, pFile);

		if (bNaturalScript)
		{
			wchar_t strPath[MAX_PATH]{};
			fread(&iLength, sizeof(size_t), 1, pFile);
			fread(strPath, sizeof(wchar_t), iLength, pFile);

			if (iLength == 0)
				continue;

			Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(strPath, strPath);

			pObject = pMeshData->Instantiate();
			pObject->AddComponent(new CNaturalScript(NATURAL_TREE));
			pObject->GetScript<CNaturalScript>()->LoadFromScene(pFile);
			pObject->GetScript<CNaturalScript>()->SetIndex(i);

#ifdef CHECK_COLLISION
			pObject->AddComponent(new CCollider2D);
			pObject->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::SPHERE);
			pObject->Collider2D()->SetOffsetPos(Vec3(0.f, 0.f, 60.f));
			pObject->Collider2D()->SetOffsetScale(Vec3(1.7f, 1.7f, 1.7f));
#endif
		}

		pObject->Transform()->LoadFromScene(pFile);
		pObject->MeshRender()->SetDynamicShadow(true);

		string str1;
		wstring str2 = strName;
		str1.assign(str2.begin(), str2.end());

		pObject->SetName(str2);
		if (str1 == "sprucea" || str1 == "sprucec")
		{
			pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"TreeMtrl"), 0);
		}
		else if (str1 == "spruceb")
		{
			pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"TreeMtrl"), 1);
		}
		else if (str1 == "plainsgrass")
		{
			pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"BushMtrl"), 0);
#ifdef CHECK_COLLISION
			pObject->Collider2D()->SetOffsetPos(Vec3(0.f, 0.f, 20.f));
			pObject->Collider2D()->SetOffsetScale(Vec3(100.f, 100.f, 100.f));
			//pObject->FrustumCheck(false);
#endif
		}
#ifdef CHECK_COLLISION
		else if (str1 == "mountainsrocks01")
		{
			pObject->Collider2D()->SetOffsetPos(Vec3(20.f, 20.f, -40.f));
			pObject->Collider2D()->SetOffsetScale(Vec3(350.f, 350.f, 350.f));
		}
		else if (str1 == "mountainsrocks02")
		{
			pObject->Collider2D()->SetOffsetPos(Vec3(20.f, 0.f, 0.f));
			pObject->Collider2D()->SetOffsetScale(Vec3(220.f, 220.f, 220.f));
		}
		else if (str1 == "genericcliffa")
		{
			pObject->Collider2D()->SetOffsetPos(Vec3(0.f, 0.f, 20.f));
			pObject->Collider2D()->SetOffsetScale(Vec3(10.f, 10.f, 10.f));
		}
		else if (str1 == "mountainsrocks01_a")
		{
			pObject->Collider2D()->SetOffsetPos(Vec3(0.f, 0.f, 0.f));
			pObject->Collider2D()->SetOffsetScale(Vec3(200.f, 200.f, 200.f));
		}
		else if (str1 == "mountainsrocks01_b")
		{
			pObject->Collider2D()->SetOffsetPos(Vec3(0.f, 0.f, -60.f));
			pObject->Collider2D()->SetOffsetScale(Vec3(220.f, 220.f, 220.f));
		}
		else if (str1 == "mountainsrocks01_c")
		{
			pObject->Collider2D()->SetOffsetPos(Vec3(0.f, 0.f, 0.f));
			pObject->Collider2D()->SetOffsetScale(Vec3(100.f, 100.f, 100.f));
		}
		else if (str1 == "mountainsrocks01_d")
		{
			pObject->Collider2D()->SetOffsetPos(Vec3(0.f, 0.f, 0.f));
			pObject->Collider2D()->SetOffsetScale(Vec3(100.f, 100.f, 100.f));
		}
		else
		{
			pObject->Collider2D()->SetOffsetPos(Vec3(0.f, 0.f, 20.f));
			pObject->Collider2D()->SetOffsetScale(Vec3(4.f, 4.f, 4.f));
		}
#endif
		CScene* pScene = CSceneMgr::GetInst()->GetCurScene();
		pScene->AddGameObject(L"Environment", pObject, false);
	}

	fclose(pFile);
#endif // NETWORK_ON
}

void CIngameScene::CreateAnimalSpawner()
{
	CGameObject* pSpawner1 = new CGameObject;
	pSpawner1->AddComponent(new CTransform);
	pSpawner1->AddComponent(new CAnimalSpawner(BEHAVIOR_TYPE::B_EVASION));

	pSpawner1->Transform()->SetLocalPos(Vec3(10000.f, 0.f, 10000.f));
	pSpawner1->GetScript<CAnimalSpawner>()->SpawnStartAnimal();
	m_pScene->FindLayer(L"Default")->AddGameObject(pSpawner1);

	// =========================================================================

	CGameObject* pSpawner3 = new CGameObject;
	pSpawner3->AddComponent(new CTransform);
	pSpawner3->AddComponent(new CAnimalSpawner(BEHAVIOR_TYPE::B_WARLIKE));

	pSpawner3->Transform()->SetLocalPos(Vec3(8100.f, 0.f, 16200.f));
	pSpawner3->GetScript<CAnimalSpawner>()->SpawnStartAnimal();
	m_pScene->FindLayer(L"Default")->AddGameObject(pSpawner3);

	// =========================================================================

	CGameObject* pSpawner2 = new CGameObject;
	pSpawner2->AddComponent(new CTransform);
	pSpawner2->AddComponent(new CAnimalSpawner(BEHAVIOR_TYPE::B_PASSIVE));

	pSpawner2->Transform()->SetLocalPos(Vec3(6900.f, 0.f, 5285.f));
	pSpawner2->GetScript<CAnimalSpawner>()->SpawnStartAnimal();
	m_pScene->FindLayer(L"Default")->AddGameObject(pSpawner2);

	CCheatMgr::GetInst()->SetSpawner(pSpawner1, pSpawner2, pSpawner3);
}

void CIngameScene::CreateShowFPS()
{
	m_pFPSInfo = new CGameObject;
	m_pFPSInfo->AddComponent(new CTransform);
	m_pFPSInfo->AddComponent(new CShowFPSScript);
	m_pFPSInfo->AddComponent(new CFont);

	tResolution vResolution = CRenderMgr::GetInst()->GetResolution();
	Vec3 vPos = Vec3(vResolution.fWidth / -2.f + 100.f, vResolution.fHeight / 2.f - 40.f, 10.f);

	m_pFPSInfo->Transform()->SetLocalPos(vPos);
	m_pFPSInfo->Transform()->SetLocalScale(Vec3(100.f, 80.f, 1.f));

	m_pFPSInfo->Font()->SetFontColor(Vec4(1.f, 0.5f, 0.f, 1.f));

	m_pScene->FindLayer(L"Invisible")->AddGameObject(m_pFPSInfo);
}

void CIngameScene::PlayerUpdate(USHORT usId, Vec3 vPos, Vec3 vRot)
{
	if (CNetwork::m_usID == usId) {
	}
	else {
		auto p = m_mapPlayers.find(usId);
		if (p == m_mapPlayers.end()) {
			Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\islandplayer.mdat", L"MeshData\\islandplayer.mdat");
			CGameObject* pObject = pMeshData->Instantiate();
			pObject->AddComponent(new CHumanScript);
			CHumanScript* pScript = pObject->GetScript<CHumanScript>();
			pScript->AnimationInfo(pObject->Animator3D());
			SetName(L"Other Object");
			pObject->FrustumCheck(false);



			pObject->MeshRender()->SetDynamicShadow(true);

			Vec3 vNewPos = vPos;
			vNewPos.y = CNaviMgr::GetInst()->GetY(vPos);
			pObject->Transform()->SetLocalPos(vNewPos);
			pObject->Transform()->SetLocalRot(vRot);
			pObject->Transform()->SetLocalScale(Vec3(1.5f, 1.5f, 1.5f));

			m_pScene->FindLayer(L"Human")->AddGameObject(pObject);
			m_mapPlayers.insert(make_pair(usId, pObject));
		}
		else {
			float fHeight = CNaviMgr::GetInst()->GetY(m_mapPlayers[usId]->Transform()->GetLocalPos());
			m_mapPlayers[usId]->Transform()->SetLocalPos(Vec3(vPos.x, fHeight, vPos.z));
			m_mapPlayers[usId]->Transform()->SetLocalRot(vRot);
		}
	}
}

void CIngameScene::PlayerRotUpdate(USHORT usId, Vec3 vRot)
{
	if (usId == CNetwork::m_usID) {
		return;
	}
	auto p = m_mapPlayers.find(usId);
	if (p == m_mapPlayers.end()) {
		
	}
	else {
		m_mapPlayers[usId]->Transform()->SetLocalRot(vRot);
	}
}

void CIngameScene::PlayerStatusUpdate(float fHealth, float fHungry, float fThirst)
{
	m_pPlayer->GetScript<CStatusScript>()->SetHealth(fHealth);
	m_pPlayer->GetScript<CStatusScript>()->SetHungry(fHungry);
	m_pPlayer->GetScript<CStatusScript>()->SetThirst(fThirst);
}

void CIngameScene::PlayerDestroy(USHORT usId)
{
	if (usId == CNetwork::m_usID)
		return;
	auto p = m_mapPlayers.find(usId);
	if (p == m_mapPlayers.end())
	{
		// 없는거 삭제하는 경우
		return;
	}
	else {
		tEvent tEv;
		tEv.eType = EVENT_TYPE::DELETE_OBJECT;
		tEv.wParam = (DWORD_PTR)(m_mapPlayers[usId]);
		CEventMgr::GetInst()->AddEvent(tEv);
		m_mapPlayers.erase(usId);
	}
}

void CIngameScene::PlayerAnimationUpdate(USHORT usId, UINT uiType)
{
	auto p = m_mapPlayers.find(usId);
	if (p == m_mapPlayers.end())
	{
		// 없는거
		return;
	}
	if (usId == CNetwork::m_usID)
		return;
	else {
		switch (uiType) {
		case (UINT)PLAYER_ANIMATION_TYPE::WALK:
			m_mapPlayers[usId]->Animator3D()->ChangeAnimation(L"Walk");
			break;
		case (UINT)PLAYER_ANIMATION_TYPE::RUN:
			m_mapPlayers[usId]->Animator3D()->ChangeAnimation(L"Run");
			break;
		case (UINT)PLAYER_ANIMATION_TYPE::IDLE1:
			m_mapPlayers[usId]->Animator3D()->ChangeAnimation(L"Idle1");
			break;
		case (UINT)PLAYER_ANIMATION_TYPE::IDLE2:
			m_mapPlayers[usId]->Animator3D()->ChangeAnimation(L"Idle2");
			break;
		case (UINT)PLAYER_ANIMATION_TYPE::DIE:
			m_mapPlayers[usId]->Animator3D()->ChangeAnimation(L"Die");
			break;
		case (UINT)PLAYER_ANIMATION_TYPE::TAKE_WEAPON:
			m_mapPlayers[usId]->Animator3D()->ChangeAnimation(L"TakeWeapon");
			break;
		case (UINT)PLAYER_ANIMATION_TYPE::ATTACK1:
			m_mapPlayers[usId]->Animator3D()->ChangeAnimation(L"Attack1");
			break;
		case (UINT)PLAYER_ANIMATION_TYPE::ATTACK2:
			m_mapPlayers[usId]->Animator3D()->ChangeAnimation(L"Attack2");
			break;
		case (UINT)PLAYER_ANIMATION_TYPE::ATTACK3:
			m_mapPlayers[usId]->Animator3D()->ChangeAnimation(L"Attack3");
			break;
		case (UINT)PLAYER_ANIMATION_TYPE::HIT1:
			m_mapPlayers[usId]->Animator3D()->ChangeAnimation(L"Hit1");
			break;
		case (UINT)PLAYER_ANIMATION_TYPE::HIT2:
			m_mapPlayers[usId]->Animator3D()->ChangeAnimation(L"Hit2");
			break;
		case (UINT)PLAYER_ANIMATION_TYPE::JUMP:
			m_mapPlayers[usId]->Animator3D()->ChangeAnimation(L"Jump");
			break;
		}
	}
}

void CIngameScene::AnimalUpdate(USHORT usId, Vec3 vPos, Vec3 vRot)
{
	auto p = m_mapAnimals.find(usId);
	if (p == m_mapAnimals.end())
	{// 생성
		CGameObject* pObject = NULL;
		if (usId < BEGIN_ANIMAL + ANIMAL_BEAR)
		{
			// 곰
			Ptr<CMeshData> pBearTex = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\bear.mdat", L"MeshData\\bear.mdat");
			pObject = pBearTex->Instantiate();
#ifdef CHECK_COLLISION
			pObject->AddComponent(new CCollider2D);
			pObject->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::SPHERE);
			pObject->Collider2D()->SetOffsetPos(Vec3(0.f, 50.f, 0.f));
			pObject->Collider2D()->SetOffsetScale(Vec3(30.f, 30.f, 30.f));
#endif
			pObject->AddComponent(new CAnimalScript);

			{
				tAnimalStatus tStatus;
				tStatus.fHp = 200.f;
				tStatus.fStamina = 100.f;
				tStatus.fDamage = 20.f;
				tStatus.fSpeed = 150.f;
				tStatus.fBehaviorTime = 4.f;
				tStatus.uiAnimalId = usId;
				tStatus.eType = BEHAVIOR_TYPE::B_WARLIKE;
				tStatus.eKind = ANIMAL_TYPE::A_BEAR;

				Vec3 vOffsetScale = Vec3(2.f, 2.f, 2.f);

				pObject->GetScript<CAnimalScript>()->SetAnimalStatus(tStatus);
				pObject->GetScript<CAnimalScript>()->SetOffsetScale(vOffsetScale);
			}

			pObject->SetName(L"Bear");

			pObject->MeshRender()->SetDynamicShadow(true);

			//pObject->Transform()->SetLocalPos(Vec3(1500.f, 20.f, 2000.f));
			pObject->Transform()->SetLocalRot(Vec3(-XM_PI / 2.f, 0.f, 0.f));
			pObject->Transform()->SetLocalScale(Vec3(20.f, 20.f, 20.f));
		}
		else if (usId < BEGIN_ANIMAL + ANIMAL_BEAR + ANIMAL_BOAR)
		{
			Ptr<CMeshData> pBoarTex = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\boar.mdat", L"MeshData\\boar.mdat");
			pObject = pBoarTex->Instantiate();
#ifdef CHECK_COLLISION
			pObject->AddComponent(new CCollider2D);
			pObject->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::SPHERE);
			pObject->Collider2D()->SetOffsetPos(Vec3(0.f, 50.f, 0.f));
			pObject->Collider2D()->SetOffsetScale(Vec3(600.f, 600.f, 600.f));
#endif
			pObject->AddComponent(new CAnimalScript);

			{
				tAnimalStatus tStatus;
				tStatus.fHp = 200.f;
				tStatus.fStamina = 100.f;
				tStatus.fDamage = 20.f;
				tStatus.fSpeed = 150.f;
				tStatus.fBehaviorTime = 4.f;
				tStatus.uiAnimalId = usId;
				tStatus.eType = BEHAVIOR_TYPE::B_PASSIVE;
				tStatus.eKind = ANIMAL_TYPE::A_BOAR;

				Vec3 vOffsetScale = Vec3(60.f, 60.f, 60.f);

				pObject->GetScript<CAnimalScript>()->SetAnimalStatus(tStatus);
				pObject->GetScript<CAnimalScript>()->SetOffsetScale(vOffsetScale);
			}

			pObject->SetName(L"Boar");

			pObject->MeshRender()->SetDynamicShadow(true);

			pObject->Transform()->SetLocalRot(Vec3(-XM_PI / 2.f, 0.f, 0.f));
			pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
		}
		else if (usId < BEGIN_ANIMAL + ANIMAL_BEAR + ANIMAL_BOAR + ANIMAL_DEER)
		{
			Ptr<CMeshData> pDeerTex = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\deer.mdat", L"MeshData\\deer.mdat");
			pObject = pDeerTex->Instantiate();
#ifdef CHECK_COLLISION
			pObject->AddComponent(new CCollider2D);

			pObject->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::SPHERE);
			pObject->Collider2D()->SetOffsetPos(Vec3(0.f, 50.f, 0.f));
			pObject->Collider2D()->SetOffsetScale(Vec3(300.f, 300.f, 300.f));

#endif
			pObject->AddComponent(new CAnimalScript);

			{
				tAnimalStatus tStatus;
				tStatus.fHp = 100.f;
				tStatus.fStamina = 100.f;
				tStatus.fDamage = 0.f;
				tStatus.fSpeed = 250.f;
				tStatus.fBehaviorTime = 4.f;
				tStatus.uiAnimalId = usId;
				tStatus.eType = BEHAVIOR_TYPE::B_EVASION;
				tStatus.eKind = ANIMAL_TYPE::A_DEER;

				Vec3 vOffsetScale = Vec3(30.f, 30.f, 30.f);

				pObject->GetScript<CAnimalScript>()->SetAnimalStatus(tStatus);
				pObject->GetScript<CAnimalScript>()->SetOffsetScale(vOffsetScale);
			}

			pObject->SetName(L"Deer");

			pObject->MeshRender()->SetDynamicShadow(true);

			pObject->Transform()->SetLocalRot(Vec3(0.f, 0.f, 0.f));
			pObject->Transform()->SetLocalScale(Vec3(2.f, 2.f, 2.f));
		}
		else if (usId < END_ANIMAL)
		{
			Ptr<CMeshData> pWolfTex = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\wolf.mdat", L"MeshData\\wolf.mdat");
			pObject = pWolfTex->Instantiate();
#ifdef CHECK_COLLISION
			pObject->AddComponent(new CCollider2D);
			pObject->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::SPHERE);
			pObject->Collider2D()->SetOffsetPos(Vec3(0.f, 50.f, 0.f));
			pObject->Collider2D()->SetOffsetScale(Vec3(30.f, 30.f, 30.f));
#endif
			pObject->AddComponent(new CAnimalScript);

			{
				tAnimalStatus tStatus;
				tStatus.fHp = 200.f;
				tStatus.fStamina = 100.f;
				tStatus.fDamage = 20.f;
				tStatus.fSpeed = 200.f;
				tStatus.fBehaviorTime = 4.f;
				tStatus.uiAnimalId = usId;
				tStatus.eType = BEHAVIOR_TYPE::B_PASSIVE;
				tStatus.eKind = ANIMAL_TYPE::A_WOLF;

				Vec3 vOffsetScale = Vec3(2.f, 2.f, 2.f);

				pObject->GetScript<CAnimalScript>()->SetAnimalStatus(tStatus);
				pObject->GetScript<CAnimalScript>()->SetOffsetScale(vOffsetScale);
			}
			pObject->SetName(L"Wolf");
			pObject->MeshRender()->SetDynamicShadow(true);
			pObject->Transform()->SetLocalRot(Vec3(0.f, 0.f, 0.f));
			pObject->Transform()->SetLocalScale(Vec3(20.f, 20.f, 20.f));
		}
		CAnimalScript* pAnimalScript = pObject->GetScript<CAnimalScript>();
		pAnimalScript->SetAnimation(pObject->Animator3D());
		pAnimalScript->SetIndex(usId);

		m_pScene->FindLayer(L"Animal")->AddGameObject(pObject);

		float fHeight = CNaviMgr::GetInst()->GetY(vPos);
		pObject->Transform()->SetLocalPos(Vec3(vPos.x, fHeight, vPos.z));
		pObject->Transform()->SetLocalRot(vRot);
		m_mapAnimals.insert(make_pair(usId, pObject));
	}
	// 업데이트
	else
	{
		float fHeight = CNaviMgr::GetInst()->GetY( vPos );
		m_mapAnimals[usId]->Transform()->SetLocalPos( Vec3( vPos.x, fHeight, vPos.z ) );
		m_mapAnimals[usId]->Transform()->SetLocalRot( vRot );
	}
}

void CIngameScene::AnimalRotUpdate(USHORT usId, Vec3 vRot)
{
	auto p = m_mapAnimals.find(usId);
	if (p == m_mapAnimals.end())
	{
		return;
	}
	else
	{
		//m_mapAnimals[usId]->Transform()->SetLocalRot(vRot);
	}
}

void CIngameScene::AnimalDestory( USHORT uiId )
{
	auto p = m_mapAnimals.find( uiId );
	if ( p == m_mapAnimals.end() )
	{
		// 없는거 삭제하는 경우
		return;
	}
	tEvent tEv;
	tEv.eType = EVENT_TYPE::DELETE_OBJECT;
	tEv.wParam = ( DWORD_PTR )( p->second );
	// p->second = m_mapAnimals[uiId]
	CEventMgr::GetInst()->AddEvent( tEv );
	m_mapAnimals.erase( p );
}

void CIngameScene::AnimalAnimationUpdate( USHORT uiId, UINT uiType )
{
	auto p = m_mapAnimals.find( uiId );
	if ( p == m_mapAnimals.end() )
	{
		// 없는거
		return;
	}

	if ( uiType == ( UINT )ANIMAL_ANIMATION_TYPE::WALK ) {
		m_mapAnimals[uiId]->Animator3D()->ChangeAnimation( L"Walk" );
	}
	else if ( uiType == ( UINT )ANIMAL_ANIMATION_TYPE::RUN ) {
		m_mapAnimals[uiId]->Animator3D()->ChangeAnimation( L"Run" );
	}
	else if ( uiType == ( UINT )ANIMAL_ANIMATION_TYPE::IDLE ) {
		m_mapAnimals[uiId]->Animator3D()->ChangeAnimation( L"Idle" );
	}
	else if ( uiType == ( UINT )ANIMAL_ANIMATION_TYPE::EAT ) {
		m_mapAnimals[uiId]->Animator3D()->ChangeAnimation( L"Eat" );
	}
	else if ( uiType == ( UINT )ANIMAL_ANIMATION_TYPE::DIE ) {
		m_mapAnimals[uiId]->Animator3D()->ChangeAnimation( L"Die" );
		m_mapAnimals[uiId]->GetScript<CAnimalScript>()->SetAnimalDead(true);
	}
	else if ( uiType == ( UINT )ANIMAL_ANIMATION_TYPE::ATTACK ) {
		m_mapAnimals[uiId]->Animator3D()->ChangeAnimation( L"Attack" );
	}
}

void CIngameScene::InstallHousing( UINT uiType, UINT uiGrade, USHORT uiId, Vec3 vPos, Vec3 vRot, Vec3 vScale )
{
	CGameObject* pObject = nullptr;

	if ( ( HOUSING_TYPE )uiType != HOUSING_ETC )
	{
		pObject = CHousingMgr::GetInst()->GetHousingMeshData( ( HOUSING_TYPE )uiType, uiGrade )->Instantiate();
	}
	else
	{
		Ptr<CMeshData> pTex = CResMgr::GetInst()->Load<CMeshData>( L"Campfire.mdat", L"MeshData\\campfire.mdat" );
		pObject = pTex->Instantiate();
	}
	pObject->AddComponent( new CBuildScript( ( HOUSING_TYPE )uiType, uiGrade ) );

#ifdef CHECK_COLLISION
	pObject->AddComponent( new CCollider2D );
	pObject->Collider2D()->SetOffsetScale( Vec3( 195.f, 195.f, 195.f ) );

	if ( uiType >= HOUSING_WALL && uiType < HOUSING_FLOOR )
		pObject->Collider2D()->SetOffsetPos( Vec3( 0.f, 0.f, 120.f ) );

	pObject->Collider2D()->SetCollider2DType( COLLIDER2D_TYPE::SPHERE );

#endif
	pObject->SetName( L"House" );

	//Vec3 vNewPos = vPos;
	//vNewPos.y = CNaviMgr::GetInst()->GetY( vPos );
	pObject->Transform()->SetLocalPos(vPos);

	pObject->Transform()->SetLocalRot( vRot );
	pObject->Transform()->SetLocalScale( vScale );

	pObject->MeshRender()->SetDynamicShadow( true );

	pObject->GetScript<CBuildScript>()->Init();
	pObject->GetScript<CBuildScript>()->SetIndex( uiId );
	pObject->GetScript<CBuildScript>()->MustBuild();

	m_pScene->FindLayer(L"House")->AddGameObject(pObject);
}

void CIngameScene::DestroyHousing( USHORT uiId )
{
	auto p = m_mapHousing.find( uiId );
	if ( p == m_mapHousing.end() )
	{
		// 없는거 삭제하는 경우
		return;
	}

	tEvent tEv;
	tEv.eType = EVENT_TYPE::DELETE_OBJECT;
	tEv.wParam = ( DWORD_PTR )( m_mapHousing[uiId] );
	CEventMgr::GetInst()->AddEvent( tEv );

	m_mapHousing.erase( uiId );
}

void CIngameScene::InstallNatural(UINT uiType, USHORT uiId, Vec3 vPos, Vec3 vRot, Vec3 vScale, Vec3 vOffsetPos, Vec3 vOffsetScale, float fHealth, bool bDestroy )
{
	CScene* pScene = CSceneMgr::GetInst()->GetCurScene();
	pScene->AddGameObject(L"Environment", m_mapNatural[uiId], false);
	auto p = m_mapNatural.find( uiId );
	if ( p == m_mapNatural.end() )
	{
		//// 생성하려면 MeshData 종류를 알아야 함
		//Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(strPath, strPath);
		//// 생성
		//CGameObject* pObject = NULL;

		//pObject = pMeshData->Instantiate();
		//pObject->AddComponent(new CNaturalScript((NATURAL_TYPE)uiType));
		//pObject->AddComponent(new CCollider2D);

		//pObject->MeshRender()->SetDynamicShadow(true);

		//tEvent tEv;
		//tEv.eType = EVENT_TYPE::CREATE_OBJECT;
		//tEv.lParam = 2;
		//tEv.wParam = (DWORD_PTR)(pObject);
		//CEventMgr::GetInst()->AddEvent(tEv);

		//float fHeight = CNaviMgr::GetInst()->GetY(pObject->Transform()->GetLocalPos());
		//pObject->Transform()->SetLocalPos(Vec3(vPos.x, fHeight, vPos.z));
		//pObject->Transform()->SetLocalRot(vRot);
		//pObject->Transform()->SetLocalScale(vScale);

		//m_mapNatural.insert(make_pair(uiId, pObject));
	}
	else
	{
		Vec3 vNewPos = vPos;
		vNewPos.y = CNaviMgr::GetInst()->GetY( vPos );

		m_mapNatural[uiId]->Transform()->SetLocalPos( vNewPos );
		//m_mapNatural[uiId]->Transform()->SetLocalRot( vRot );
		m_mapNatural[uiId]->Transform()->SetLocalScale( vScale );

		m_mapNatural[uiId]->Collider2D()->SetOffsetPos( vOffsetPos );
		m_mapNatural[uiId]->Collider2D()->SetOffsetScale( vOffsetScale );

		m_mapNatural[uiId]->GetScript<CNaturalScript>()->SetHealth( fHealth );
		m_mapNatural[uiId]->GetScript<CNaturalScript>()->SetDestroy( bDestroy );
	}
}

void CIngameScene::DestroyNatural( USHORT uiId )
{
	auto p = m_mapNatural.find( uiId );
	if ( p == m_mapNatural.end() )
	{
		// 없는거 삭제하는 경우
		return;
	}
	cout << "Natural Destroy : " << uiId << endl;
	tEvent tEv;
	tEv.eType = EVENT_TYPE::DELETE_OBJECT;
	tEv.wParam = ( DWORD_PTR )( m_mapNatural[uiId] );
	CEventMgr::GetInst()->AddEvent( tEv );

	m_mapNatural.erase( uiId );
}

void CIngameScene::ChatUpdate(string name, string Msg)
{
	m_pChat->GetScript<CChatScript>()->AddChat(name, Msg);
	m_pChat->GetScript<CInputScript>()->SetEnable(false);
	m_pChat->GetScript<CInputScript>()->Clear();
}
