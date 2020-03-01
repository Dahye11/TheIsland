#include "stdafx.h"
#include "IngameScene.h"

#include <Engine/Ptr.h>

#include <Engine/Texture.h>

#include <Engine/ResMgr.h>
#include <Engine/CollisionMgr.h>

#include <Engine/Layer.h>
#include <Engine/Scene.h>

#include <Engine/GameObject.h>
#include <Engine/MeshRender.h>
#include <Engine/Transform.h>
#include <Engine/Camera.h>
#include <Engine/Light3D.h>
#include <Engine/LandScape.h>
#include <Engine/GridScript.h>

#include <Engine/PlayerScript.h>
#include <Engine/FPSCamScript.h>
#include <Engine/MonsterScript.h>
#include <Engine/StatusScript.h>
#include <Engine/QuickSlotScript.h>

#include "PlayerCamScript.h"

CIngameScene::CIngameScene()
{
}


CIngameScene::~CIngameScene()
{
}

void CIngameScene::Init()
{
	Ptr<CTexture> pTex = CResMgr::GetInst()->Load<CTexture>( L"TestTex", L"Texture\\Health.png" );
	Ptr<CTexture> pExplosionTex = CResMgr::GetInst()->Load<CTexture>( L"Explosion", L"Texture\\Explosion\\Explosion80.png" );
	Ptr<CTexture> pBlackTex = CResMgr::GetInst()->Load<CTexture>( L"Black", L"Texture\\asd.png" );
	Ptr<CTexture> pSky01 = CResMgr::GetInst()->Load<CTexture>( L"Sky01", L"Texture\\Skybox\\Sky01.png" );
	Ptr<CTexture> pSky02 = CResMgr::GetInst()->Load<CTexture>( L"Sky02", L"Texture\\Skybox\\Sky02.jpg" );

	Ptr<CTexture> pColor = CResMgr::GetInst()->Load<CTexture>( L"Tile", L"Texture\\Tile\\TILE_03.tga" );
	Ptr<CTexture> pNormal = CResMgr::GetInst()->Load<CTexture>( L"Tile_n", L"Texture\\Tile\\TILE_03_N.tga" );

	CGameObject* pObject = nullptr;
	CGameObject* pChildObject = nullptr;

	// ===================
	// Player 오브젝트 생성
	// ===================
	pObject = new CGameObject;
	pObject->SetName( L"Player Object" );
	pObject->AddComponent( new CTransform );
	pObject->AddComponent( new CMeshRender );

	// Transform 설정
	pObject->Transform()->SetLocalPos( Vec3( 0.f, 25.f, 0.f ) );
	pObject->Transform()->SetLocalScale( Vec3( 50.f, 50.f, 50.f ) );
	//pObject->Transform()->SetLocalRot(Vec3(XM_PI / 2.f, 0.f, 0.f));

	// MeshRender 설정
	pObject->MeshRender()->SetMesh( CResMgr::GetInst()->FindRes<CMesh>( L"SphereMesh" ) );
	pObject->MeshRender()->SetMaterial( CResMgr::GetInst()->FindRes<CMaterial>( L"Std3DMtrl" ) );
	pObject->MeshRender()->GetSharedMaterial()->SetData( SHADER_PARAM::TEX_0, pColor.GetPointer() );
	pObject->MeshRender()->GetSharedMaterial()->SetData( SHADER_PARAM::TEX_1, pNormal.GetPointer() );

	// Script 설정
	pObject->AddComponent( new CPlayerScript );

	// AddGameObject
	m_pScene->FindLayer( L"Player" )->AddGameObject( pObject );

	// ==================
	// Camera Object 생성
	// ==================
	CGameObject* pMainCam = new CGameObject;
	pMainCam->SetName( L"MainCam" );
	pMainCam->AddComponent( new CTransform );
	pMainCam->AddComponent( new CCamera );
	pMainCam->AddComponent( new CFPSCamScript );

	pMainCam->Transform()->SetLocalPos( Vec3( 0.f, 100.f, 0.f ) );
	pMainCam->Camera()->SetProjType( PROJ_TYPE::PERSPECTIVE );
	pMainCam->Camera()->SetFar( 100000.f );
	pMainCam->Camera()->SetLayerAllCheck();
	pMainCam->Camera()->SetLayerCheck( 30, false );
	pObject->GetScript<CPlayerScript>()->SetCamera( pMainCam->Camera() );
	m_pScene->FindLayer( L"Default" )->AddGameObject( pMainCam );

	// ====================
	// Player Camera
	// ====================
	CGameObject* pPlayerCam = new CGameObject;
	pPlayerCam->AddComponent(new CTransform);
	pPlayerCam->AddComponent(new CCamera);
	pPlayerCam->AddComponent(new CPlayerCamScript);
	pPlayerCam->GetScript<CPlayerCamScript>()->SetPlayer(pObject);
	
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

	m_pScene->FindLayer(L"Default")->AddGameObject(pUICam);


	CreateQuickSlotUI();
	CreatePlayerStatusUI();
	CreateInventoryUI();

	// ====================
	// 3D Light Object 추가
	// ====================
	pObject = new CGameObject;
	pObject->AddComponent( new CTransform );
	pObject->AddComponent( new CLight3D );

	pObject->Light3D()->SetLightPos( Vec3( 0.f, 200.f, 1000.f ) );
	pObject->Light3D()->SetLightType( LIGHT_TYPE::DIR );
	pObject->Light3D()->SetDiffuseColor( Vec3( 1.f, 1.f, 1.f ) );
	pObject->Light3D()->SetSpecular( Vec3( 0.3f, 0.3f, 0.3f ) );
	pObject->Light3D()->SetAmbient( Vec3( 0.1f, 0.1f, 0.1f ) );
	pObject->Light3D()->SetLightDir( Vec3( 1.f, -1.f, 1.f ) );
	pObject->Light3D()->SetLightRange( 500.f );

	m_pScene->FindLayer( L"Default" )->AddGameObject( pObject );

	// ===================
	// Test 오브젝트 생성
	// ===================
	pObject = new CGameObject;
	pObject->SetName( L"Test Object" );
	pObject->AddComponent( new CTransform );
	pObject->AddComponent( new CMeshRender );

	// Transform 설정
	pObject->Transform()->SetLocalPos( Vec3( 0.f, 200.f, 1000.f ) );
	pObject->Transform()->SetLocalScale( Vec3( 100.f, 100.f, 100.f ) );
	//pObject->Transform()->SetLocalRot(Vec3(XM_PI / 2.f, 0.f, 0.f));

	// MeshRender 설정
	pObject->MeshRender()->SetMesh( CResMgr::GetInst()->FindRes<CMesh>( L"SphereMesh" ) );
	pObject->MeshRender()->SetMaterial( CResMgr::GetInst()->FindRes<CMaterial>( L"Std3DMtrl" ) );
	pObject->MeshRender()->GetSharedMaterial()->SetData( SHADER_PARAM::TEX_0, pColor.GetPointer() );
	pObject->MeshRender()->GetSharedMaterial()->SetData( SHADER_PARAM::TEX_1, pNormal.GetPointer() );


	// AddGameObject
	m_pScene->FindLayer( L"Monster" )->AddGameObject( pObject );


	//// Script 설정	
	//pObject->GetScript<CGridScript>()->SetToolCamera(pMainCam);
	//pObject->GetScript<CGridScript>()->SetGridColor(Vec3(0.8f, 0.2f, 0.2f));

	//// AddGameObject
	//m_pScene->FindLayer(L"Tool")->AddGameObject(pObject);
	//	

	//// =================================
	//// CollisionMgr 충돌 그룹(Layer) 지정
	//// =================================
	//// Player Layer 와 Monster Layer 는 충돌 검사 진행
	//CCollisionMgr::GetInst()->CheckCollisionLayer(L"Player", L"Monster");
	//CCollisionMgr::GetInst()->CheckCollisionLayer(L"Bullet", L"Monster");

	// Script 설정
	pObject->AddComponent( new CMonsterScript );

	// AddGameObject
	m_pScene->FindLayer( L"Monster" )->AddGameObject( pObject );

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

	// =======================
	// LandScape 오브젝트 생성
	// =======================
	pObject = new CGameObject;
	pObject->SetName( L"LandScape Object" );
	pObject->AddComponent( new CTransform );
	pObject->AddComponent( new CMeshRender );
	pObject->AddComponent( new CLandScape );
	pObject->LandScape()->CreateLandScape( L"Texture/TestLandScape.bmp", 219, 219 );
	pObject->MeshRender()->SetMesh( CResMgr::GetInst()->FindRes<CMesh>( L"LandScapeMesh" ) );
	pObject->MeshRender()->SetMaterial( CResMgr::GetInst()->FindRes<CMaterial>( L"LandScapeMtrl" ) );
	//pObject->MeshRender()->GetSharedMaterial()->SetData( SHADER_PARAM::TEX_0, pSky01.GetPointer() );
	pObject->Transform()->SetLocalPos( Vec3( 0.f, 0.f, 0.f ) );
	pObject->Transform()->SetLocalScale( Vec3( 15.f, 15.f, 15.f ) );
	pObject->FrustumCheck( false );

	m_pScene->FindLayer( L"Default" )->AddGameObject( pObject );


	// ====================
	// Grid 오브젝트 생성
	// ====================
	pObject = new CGameObject;
	pObject->SetName( L"Grid" );
	pObject->FrustumCheck( false );
	pObject->AddComponent( new CTransform );
	pObject->AddComponent( new CMeshRender );
	pObject->AddComponent( new CGridScript );

	// Transform 설정
	pObject->Transform()->SetLocalScale( Vec3( 100000.f, 100000.f, 1.f ) );
	pObject->Transform()->SetLocalRot( Vec3( XM_PI / 2.f, 0.f, 0.f ) );

	// MeshRender 설정
	pObject->MeshRender()->SetMesh( CResMgr::GetInst()->FindRes<CMesh>( L"RectMesh" ) );
	pObject->MeshRender()->SetMaterial( CResMgr::GetInst()->FindRes<CMaterial>( L"GridMtrl" ) );

	// Script 설정	
	pObject->GetScript<CGridScript>()->SetToolCamera( pMainCam );
	pObject->GetScript<CGridScript>()->SetGridColor( Vec3( 0.8f, 0.2f, 0.2f ) );

	// AddGameObject
	m_pScene->FindLayer( L"Tool" )->AddGameObject( pObject );


	// =================================
	// CollisionMgr 충돌 그룹(Layer) 지정
	// =================================
	// Player Layer 와 Monster Layer 는 충돌 검사 진행
	CCollisionMgr::GetInst()->CheckCollisionLayer( L"Player", L"Monster" );
	CCollisionMgr::GetInst()->CheckCollisionLayer( L"Bullet", L"Monster" );
}

void CIngameScene::CreateQuickSlotUI()
{
	// ===================
	// UI 오브젝트 생성
	// ===================
	// UI QuickSlot BackGround
	CGameObject* pObject = new CGameObject;
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);

	pObject->Transform()->SetLocalPos(Vec3(0.f, -320.f, 1000.f));
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

		//pChildObject->Transform()->SetLocalPos(Vec3(-190.f + (i * 95.f), -330.f, 500.f));
		pChildObject->Transform()->SetLocalPos(Vec3(-0.4f + (i * 0.2f), -0.1f, -100.f));
		pChildObject->Transform()->SetLocalScale(Vec3(75.f / 500.f, 75.f / 80.f, 1.f));

		pChildObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		pChildObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIMtrl"));

		a = 2;
		pChildObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::INT_0, &a);

		pObject->AddChild(pChildObject);
		m_pScene->FindLayer(L"UI")->AddGameObject(pChildObject);
	}
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
	pObject->Transform()->SetLocalPos(Vec3(490.f, -300.f, 1500.f));
	pObject->Transform()->SetLocalScale(Vec3(250.f, 135.f, 1.f));

	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIMtrl"));

	int a = 4; // White
	pObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::INT_0, &a);
	pObject->AddComponent(new CStatusScript);

	m_pScene->FindLayer(L"UI")->AddGameObject(pObject);

	// PlayerStatus HealthBar
	CGameObject* pChildObject = new CGameObject;
	pChildObject->AddComponent(new CTransform);
	pChildObject->AddComponent(new CMeshRender);
	//pChildObject->Transform()->SetLocalPos(Vec3(510.f, -260.f, 1000.f));
	pChildObject->Transform()->SetLocalPos(Vec3(0.075f, 0.3f, -100.f));
	pChildObject->Transform()->SetLocalScale(Vec3(190.f / 250.f, 30.f / 135.f, 1.f));
	pChildObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pChildObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIMtrl"));
	a = 5; // Green
	pChildObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::INT_0, &a);
	pObject->AddChild(pChildObject);
	m_pScene->FindLayer(L"UI")->AddGameObject(pChildObject);

	// PlayerStatus HungryBar
	pChildObject = new CGameObject;
	pChildObject->AddComponent(new CTransform);
	pChildObject->AddComponent(new CMeshRender);
	//pChildObject->Transform()->SetLocalPos(Vec3(510.f, -300.f, 1000.f));
	pChildObject->Transform()->SetLocalPos(Vec3(0.075f, 0.f, -100.f));
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
	pChildObject->Transform()->SetLocalPos(Vec3(0.075f, -0.3f, -100.f));
	pChildObject->Transform()->SetLocalScale(Vec3(190.f / 250.f, 30.f / 135.f, 1.f));
	pChildObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pChildObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIMtrl"));
	a = 3; // Blue
	pChildObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::INT_0, &a);
	pObject->AddChild(pChildObject);
	m_pScene->FindLayer(L"UI")->AddGameObject(pChildObject);

	// =================================
	pChildObject = new CGameObject;
	pChildObject->SetName(L"Screen Damage");
	pChildObject->AddComponent(new CTransform);
	pChildObject->AddComponent(new CMeshRender);

	// Transform 설정
	pChildObject->Transform()->SetLocalPos(Vec3(-1.95f, 2.25f, -1400.f));
	pChildObject->Transform()->SetLocalScale(Vec3(5.2f, 5.8f, 1.f));

	// MeshRender 설정
	pChildObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pChildObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIMtrl"));
	pChildObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, pUITex.GetPointer());
	float health = 1.f;
	pChildObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::FLOAT_0, &health);
	// AddGameObject
	pObject->AddChild(pChildObject);
	m_pScene->FindLayer(L"UI")->AddGameObject(pChildObject);

	//
	// Health Icon
	pObject = new CGameObject;
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);

	pObject->Transform()->SetLocalPos(Vec3(390.f, -260.f, 500.f));
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

	pObject->Transform()->SetLocalPos(Vec3(390.f, -300.f, 500.f));
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

	pObject->Transform()->SetLocalPos(Vec3(390.f, -340.f, 500.f));
	pObject->Transform()->SetLocalScale(Vec3(30.f, 30.f, 1.f));

	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"IconMtrl"));

	pObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, pTexThirst.GetPointer());

	pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::FLOAT_0, &fa);

	m_pScene->FindLayer(L"UI")->AddGameObject(pObject);
}

void CIngameScene::CreateInventoryUI()
{
	Vec3 vScale(150.f, 150.f, 1.f);

	Ptr<CTexture> PlayerTex = CResMgr::GetInst()->FindRes<CTexture>(L"PlayerTex");

	CGameObject* pObject = new CGameObject;
	pObject->SetName(L"Inventory Player Object");

	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);

	pObject->Transform()->SetLocalPos(Vec3(0.f, 0.f, 100.f));

	pObject->Transform()->SetLocalScale(vScale);

	// MeshRender 설정
	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"TexMtrl");
	pObject->MeshRender()->SetMaterial(pMtrl->Clone());
	pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, PlayerTex.GetPointer());

	// AddGameObject
	m_pScene->FindLayer(L"UI")->AddGameObject(pObject);
}
