#include "stdafx.h"
#include "SceneMgr.h"

#include "Scene.h"
#include "Layer.h"
#include "GameObject.h"

#include "ResMgr.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "Transform.h"
#include "MeshRender.h"
#include "Animator2D.h"
#include "Animation2D.h"
#include "Collider2D.h"
#include "Light2D.h"
#include "Light3D.h"
#include "LandScape.h"

#include "TimeMgr.h"
#include "KeyMgr.h"
#include "Camera.h"

#include "CollisionMgr.h"
#include "EventMgr.h"
#include "RenderMgr.h"
#include "Device.h"
#include "Core.h"

#include "PlayerScript.h"
#include "MonsterScript.h"
#include "ToolCamScript.h"
#include "GridScript.h"

#include "FPSCamScript.h"
#include "StatusScript.h"
#include "QuickSlotScript.h"

CScene* CSceneMgr::GetCurScene()
{
	return m_pCurScene;
}

void CSceneMgr::ChangeScene(CScene * _pNextScene)
{
	SAFE_DELETE(m_pCurScene);
	m_pCurScene = _pNextScene;
}

CSceneMgr::CSceneMgr()
	: m_pCurScene(nullptr)	
{
}

CSceneMgr::~CSceneMgr()
{
	SAFE_DELETE(m_pCurScene);
}

void CSceneMgr::init()
{
	// =================
	// �ʿ��� ���ҽ� �ε�
	// =================
	// Texture �ε�
	Ptr<CTexture> pTex = CResMgr::GetInst()->Load<CTexture>(L"TestTex", L"Texture\\Health.png");
	Ptr<CTexture> pExplosionTex = CResMgr::GetInst()->Load<CTexture>(L"Explosion", L"Texture\\Explosion\\Explosion80.png");	
	Ptr<CTexture> pBlackTex = CResMgr::GetInst()->Load<CTexture>(L"Black", L"Texture\\asd.png");
	Ptr<CTexture> pSky01 = CResMgr::GetInst()->Load<CTexture>(L"Sky01", L"Texture\\Skybox\\Sky01.png");
	Ptr<CTexture> pSky02 = CResMgr::GetInst()->Load<CTexture>(L"Sky02", L"Texture\\Skybox\\Sky02.jpg");

	Ptr<CTexture> pColor = CResMgr::GetInst()->Load<CTexture>(L"Tile", L"Texture\\Tile\\TILE_03.tga");
	Ptr<CTexture> pNormal = CResMgr::GetInst()->Load<CTexture>(L"Tile_n", L"Texture\\Tile\\TILE_03_N.tga");

	Ptr<CTexture> pUITex = CResMgr::GetInst()->Load<CTexture>(L"UITex", L"Texture\\Image2.png");
	Ptr<CTexture> pTexHealth = CResMgr::GetInst()->Load<CTexture>(L"HealthIconTex", L"Texture\\Health01.png");
	Ptr<CTexture> pTexHungry = CResMgr::GetInst()->Load<CTexture>(L"HungryIconTex", L"Texture\\Hungry01.png");
	Ptr<CTexture> pTexThirst = CResMgr::GetInst()->Load<CTexture>(L"ThirstIconTex", L"Texture\\Thirst01.png");


	
	// ===============
	// Test Scene ����
	// ===============
	m_pCurScene = new CScene;
	m_pCurScene->SetName(L"Test Scene");

	// ===============
	// Layer �̸� ����
	// ===============
	m_pCurScene->GetLayer(0)->SetName(L"Default");
	m_pCurScene->GetLayer(1)->SetName(L"Player");
	m_pCurScene->GetLayer(2)->SetName(L"Monster");
	m_pCurScene->GetLayer(3)->SetName(L"Bullet");
	m_pCurScene->GetLayer(4)->SetName(L"UI");
	m_pCurScene->GetLayer(31)->SetName(L"Tool");

	CGameObject* pObject = nullptr;
	CGameObject* pChildObject = nullptr;

	// ===================
	// Player ������Ʈ ����
	// ===================
	pObject = new CGameObject;
	pObject->SetName(L"Player Object");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);

	// Transform ����
	pObject->Transform()->SetLocalPos(Vec3(0.f, 25.f, 0.f));
	pObject->Transform()->SetLocalScale(Vec3(50.f, 50.f, 50.f));
	//pObject->Transform()->SetLocalRot(Vec3(XM_PI / 2.f, 0.f, 0.f));

	// MeshRender ����
	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CubeMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"TestMtrl"));
	pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, pColor.GetPointer());
	pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_1, pNormal.GetPointer());

	// Script ����
	pObject->AddComponent(new CPlayerScript);

	// AddGameObject
	m_pCurScene->FindLayer(L"Player")->AddGameObject(pObject);

	// ==================
	// Camera Object ����
	// ==================
	CGameObject* pMainCam = new CGameObject;
	pMainCam->SetName(L"MainCam");
	pMainCam->AddComponent(new CTransform);
	pMainCam->AddComponent(new CCamera);
	pMainCam->AddComponent(new CFPSCamScript);

	pMainCam->Transform()->SetLocalPos(Vec3(0.f, 100.f, 0.f));
	pMainCam->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
	pMainCam->Camera()->SetFar(100000.f);
	pMainCam->Camera()->SetLayerAllCheck();
	pMainCam->Camera()->SetLayerCheck(4, false);
	pObject->GetScript<CPlayerScript>()->SetCamera(pMainCam->Camera());
	m_pCurScene->FindLayer(L"Default")->AddGameObject(pMainCam);

	// ====================
	// UI Camera
	// ====================
	pObject = new CGameObject;
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CCamera);

	pObject->Camera()->SetProjType(PROJ_TYPE::ORTHGRAPHIC); // ���� ����
	pObject->Camera()->SetFar(10000.f);
	pObject->Camera()->SetLayerCheck(4, true);

	m_pCurScene->GetLayer(0)->AddGameObject(pObject);
	
	// ===================
	// UI ������Ʈ ����
	// ===================
	// UI QuickSlot BackGround
	pObject = new CGameObject;
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->FrustumCheck(false);

	pObject->Transform()->SetLocalPos(Vec3(0.f, -320.f, 1000.f));
	pObject->Transform()->SetLocalScale(Vec3(500.f, 80.f, 1.f));

	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIMtrl"));

	int a = 1;
	pObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::INT_0, &a);
	pObject->AddComponent(new CQuickSlotScript);
	m_pCurScene->FindLayer(L"UI")->AddGameObject(pObject);

	// UI QuickSlot slot
	for (int i = 0; i < 5; ++i)
	{
		pChildObject = new CGameObject;
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
		m_pCurScene->FindLayer(L"UI")->AddGameObject(pChildObject);
	}
	//
	// PlayerStatus BackGround
	pObject = new CGameObject;
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->SetName(L"Player Status");
	pObject->Transform()->SetLocalPos(Vec3(490.f, -300.f, 1500.f));
	pObject->Transform()->SetLocalScale(Vec3(250.f, 135.f, 1.f));

	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIMtrl"));

	a = 4; // White
	pObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::INT_0, &a);
	pObject->AddComponent(new CStatusScript);

	m_pCurScene->FindLayer(L"UI")->AddGameObject(pObject);

	// PlayerStatus HealthBar
	pChildObject = new CGameObject;
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
	m_pCurScene->FindLayer(L"UI")->AddGameObject(pChildObject);

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
	m_pCurScene->FindLayer(L"UI")->AddGameObject(pChildObject);
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
	m_pCurScene->FindLayer(L"UI")->AddGameObject(pChildObject);

	// =================================
	pChildObject = new CGameObject;
	pChildObject->SetName(L"Screen Damage");
	pChildObject->AddComponent(new CTransform);
	pChildObject->AddComponent(new CMeshRender);

	// Transform ����
	pChildObject->Transform()->SetLocalPos(Vec3(-1.95f, 2.25f, -1400.f));
	pChildObject->Transform()->SetLocalScale(Vec3(5.2f, 5.8f, 1.f));

	// MeshRender ����
	pChildObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pChildObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIMtrl"));
	pChildObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, pUITex.GetPointer());
	float health = 1.f;
	pChildObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::FLOAT_0, &health);
	// AddGameObject
	pObject->AddChild(pChildObject);
	m_pCurScene->FindLayer(L"UI")->AddGameObject(pChildObject);

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

	m_pCurScene->FindLayer(L"UI")->AddGameObject(pObject);
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

	m_pCurScene->FindLayer(L"UI")->AddGameObject(pObject);
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

	m_pCurScene->FindLayer(L"UI")->AddGameObject(pObject);

	// ====================
	// 3D Light Object �߰�
	// ====================
	pObject = new CGameObject;
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CLight3D);	   	
	
	pObject->Light3D()->SetLightPos(Vec3(0.f, 200.f, 1000.f));
	pObject->Light3D()->SetLightType(LIGHT_TYPE::DIR);
	pObject->Light3D()->SetDiffuseColor(Vec3(1.f, 1.f, 1.f));
	pObject->Light3D()->SetSpecular(Vec3(0.3f, 0.3f, 0.3f));
	pObject->Light3D()->SetAmbient(Vec3(0.1f, 0.1f, 0.1f));
	pObject->Light3D()->SetLightDir(Vec3(1.f, -1.f, 1.f));
	pObject->Light3D()->SetLightRange(500.f);
	   
	m_pCurScene->FindLayer(L"Default")->AddGameObject(pObject);

	// ===================
	// Test ������Ʈ ����
	// ===================
	pObject = new CGameObject;
	pObject->SetName(L"Test Object");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);	

	// Transform ����
	pObject->Transform()->SetLocalPos(Vec3(0.f, 200.f, 1000.f));
	pObject->Transform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
	//pObject->Transform()->SetLocalRot(Vec3(XM_PI / 2.f, 0.f, 0.f));

	// MeshRender ����
	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"TestMtrl"));
	pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, pColor.GetPointer());
	pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_1, pNormal.GetPointer());
	

	// AddGameObject
	m_pCurScene->FindLayer(L"Monster")->AddGameObject(pObject);


	// ====================
	// Monster ������Ʈ ����
	// ====================
	pObject = new CGameObject;
	pObject->SetName(L"Monster Object");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);	

	// Transform ����
	pObject->Transform()->SetLocalPos(Vec3(0.f, 200.f, 500.f));
	pObject->Transform()->SetLocalScale(Vec3(100.f, 100.f, 1.f));

	// MeshRender ����
	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"TestMtrl"));	
	
	// Script ����
	pObject->AddComponent(new CMonsterScript);

	// AddGameObject
	m_pCurScene->FindLayer(L"Monster")->AddGameObject(pObject);

	// ====================
	// Skybox ������Ʈ ����
	// ====================
	pObject = new CGameObject;
	pObject->SetName(L"SkyBox");
	pObject->FrustumCheck(false);
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);	

	// MeshRender ����
	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"SkyboxMtrl"));
	pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, pSky01.GetPointer());

	// AddGameObject
	m_pCurScene->FindLayer(L"Default")->AddGameObject(pObject);

	// =======================
	// LandScape ������Ʈ ����
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
	pObject->FrustumCheck(false);

	m_pCurScene->FindLayer( L"Default" )->AddGameObject( pObject );


	// ====================
	// Grid ������Ʈ ����
	// ====================
	pObject = new CGameObject;
	pObject->SetName(L"Grid");
	pObject->FrustumCheck(false);
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->AddComponent(new CGridScript);

	// Transform ����
	pObject->Transform()->SetLocalScale(Vec3(100000.f, 100000.f, 1.f));
	pObject->Transform()->SetLocalRot(Vec3(XM_PI / 2.f, 0.f, 0.f));

	// MeshRender ����
	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"GridMtrl"));	

	// Script ����	
	pObject->GetScript<CGridScript>()->SetToolCamera(pMainCam);
	pObject->GetScript<CGridScript>()->SetGridColor(Vec3(0.8f, 0.2f, 0.2f));

	// AddGameObject
	m_pCurScene->FindLayer(L"Tool")->AddGameObject(pObject);
		

	// =================================
	// CollisionMgr �浹 �׷�(Layer) ����
	// =================================
	// Player Layer �� Monster Layer �� �浹 �˻� ����
	CCollisionMgr::GetInst()->CheckCollisionLayer(L"Player", L"Monster");
	CCollisionMgr::GetInst()->CheckCollisionLayer(L"Bullet", L"Monster");
	
	m_pCurScene->Awake();
	m_pCurScene->Start();
}

void CSceneMgr::Update()
{		
	m_pCurScene->Update();
	m_pCurScene->LateUpdate();

	// Rendermgr ī�޶� �ʱ�ȭ
	CRenderMgr::GetInst()->ClearCamera();

	m_pCurScene->finalUpdate();
	   
	// �浹 ó��
	CCollisionMgr::GetInst()->Update();
}

void CSceneMgr::Update_tool()
{
	// Rendermgr ī�޶� �ʱ�ȭ
	CRenderMgr::GetInst()->ClearCamera();
	m_pCurScene->finalUpdate();
}

void CSceneMgr::FindGameObjectByTag(const wstring& _strTag, vector<CGameObject*>& _vecFindObj)
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		const vector<CGameObject*>& vecObject = m_pCurScene->GetLayer(i)->GetObjects();
		for (size_t j = 0; j < vecObject.size(); ++j)
		{
			if (_strTag == vecObject[j]->GetName())
			{
				_vecFindObj.push_back(vecObject[j]);
			}
		}
	}	
}

void CSceneMgr::CreateScene( const wstring & _strTag )
{
	CScene* pScene = new CScene;
	pScene->SetName( _strTag );
}

bool Compare(CGameObject* _pLeft, CGameObject* _pRight)
{
	return (_pLeft->Transform()->GetWorldPos().z < _pRight->Transform()->GetWorldPos().z);
}

//void CSceneMgr::FindGameObjectByPoint(POINT _point, vector<CGameObject*>& _vecFindObj, CCamera* _pToolCam)
//{
//	CCamera* pCam = _pToolCam;
//	if (CCore::GetInst()->GetSceneMod() == SCENE_MOD::SCENE_PLAY)
//	{
//		pCam = CRenderMgr::GetInst()->GetCamera(0);
//	}
//
//	tResolution tRes = CRenderMgr::GetInst()->GetResolution();
//	Vec3 vPickPos = Vec3((float)_point.x - (tRes.fWidth / 2.f), (tRes.fHeight / 2.f) - (float)_point.y, 0.f);
//	vPickPos *= pCam->GetScale(); 
//	vPickPos += pCam->Transform()->GetWorldPos();
//
//	for (int i = 0; i < MAX_LAYER; ++i)
//	{
//		const vector<CGameObject*>& vecObject = m_pCurScene->GetLayer(i)->GetObjects();
//		for (size_t j = 0; j < vecObject.size(); ++j)
//		{
//			if (vecObject[j]->Transform()->IsCasting(vPickPos))
//			{
//				_vecFindObj.push_back(vecObject[j]);
//			}
//		}
//	}
//
//	sort(_vecFindObj.begin(), _vecFindObj.end(), Compare);
//}

