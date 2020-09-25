#include "stdafx.h"
#include "ToolItemScript.h"
#include "StuffScript.h"
#include "AnimalScript.h"
#include "InventoryScript.h"
#include "PlayerScript.h"
#include "UsableScript.h"

#include "BuildScript.h"
#include "NaturalScript.h"

#include <Engine/ParticleSystem.h>
#include <Engine/Light3D.h>
#include <Engine/NaviMgr.h>

#include "ArrowScript.h"

#include "Network.h"
#include "PacketMgr.h"

#include <iostream>

CToolItemScript::CToolItemScript(ITEM_TYPE eType, int iCount)
	: CItemScript((UINT) eType),
	m_pObj(NULL),
	m_pSubObj(NULL),
	m_bCharge(false),
	m_pHost(NULL)
{
	m_iMaxCount = TOOL_MAX_COUNT;
	if (iCount > m_iMaxCount)
		iCount = TOOL_MAX_COUNT;
	m_iCount = iCount;

	Ptr<CMeshData> pTex = NULL;

	switch (m_eItemType)
	{
	case ITEM_PICKAXE:
		m_fDamage = 30.f;
		break;
	case ITEM_AXE:
		m_fDamage = 30.f;
		break;
	case ITEM_HAMMER:
		m_fDamage = 15.f;
		break;
	case ITEM_MACHETTE:
		m_fDamage = 45.f;
		break;
	case ITEM_WOODCLUB:
		m_fDamage = 20.f;
		break;
	case ITEM_BOW:
		pTex = CResMgr::GetInst()->Load<CMeshData>(L"Arrow.mdat", L"MeshData\\arrow.mdat");
		m_pObj = pTex->Instantiate();
#ifdef CHECK_COLLISION
		m_pObj->AddComponent(new CCollider2D);

		m_pObj->Collider2D()->SetOffsetScale(Vec3(10.f, 10.f, 10.f));
		m_pObj->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::SPHERE);
#endif
		m_pObj->SetName(L"Arrow");
		m_pObj->Transform()->SetLocalPos(Vec3(10000.f, 400.f, 10000.f));
		//m_pObj->Transform()->SetLocalRot(Vec3(-XM_PI / 2.f, 0.f, 0.f));
		m_pObj->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
		CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Invisible")->AddGameObject(m_pObj);

		m_fDamage = 30.f;
		break;
	case ITEM_CAMPFIRE:
		pTex = CResMgr::GetInst()->Load<CMeshData>(L"Campfire.mdat", L"MeshData\\campfire.mdat");
		m_pObj = pTex->Instantiate();
		m_pObj->AddComponent(new CBuildScript(HOUSING_ETC));
#ifdef CHECK_COLLISION
		m_pObj->AddComponent( new CCollider2D );

		m_pObj->Collider2D()->SetOffsetScale(Vec3(150.f, 150.f, 150.f));
		m_pObj->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::SPHERE);
#endif
		m_pObj->SetName(L"Campfire");
		m_pObj->Transform()->SetLocalPos(Vec3(0.f, 10.f, 0.f));
		m_pObj->Transform()->SetLocalRot(Vec3(-XM_PI / 2.f, 0.f, 0.f));
		m_pObj->Transform()->SetLocalScale(Vec3(0.3f, 0.3f, 0.3f));
		m_pObj->GetScript<CBuildScript>()->Init();
		CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Invisible")->AddGameObject(m_pObj);
		m_fDamage = 0.f;
		break;
	case ITEM_MAP:
	{
		Ptr<CTexture> pMapTex = CResMgr::GetInst()->Load<CTexture>(L"MapTex", L"Texture\\IslandMap.png");
		m_pObj = new CGameObject;
		m_pObj->SetName(L"Map");
		m_pObj->AddComponent(new CTransform);
		m_pObj->AddComponent(new CMeshRender);

		m_pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"TexMtrl");
		m_pObj->MeshRender()->SetMaterial(pMtrl->Clone());
		m_pObj->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, pMapTex.GetPointer());

		m_pObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, 3000.f));
		m_pObj->Transform()->SetLocalScale(Vec3(800.f, 600.f, 1.f));

		CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Invisible")->AddGameObject(m_pObj);

		Ptr<CTexture> pLocationTex = CResMgr::GetInst()->Load<CTexture>(L"LocationTex", L"Texture\\navigation_white_192x192.png");
		m_pSubObj = new CGameObject;
		m_pSubObj->SetName(L"Player Location");
		m_pSubObj->AddComponent(new CTransform);
		m_pSubObj->AddComponent(new CMeshRender);

		m_pSubObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"ColorIconMtrl");
		m_pSubObj->MeshRender()->SetMaterial(pMtrl->Clone());
		Vec4 vColor = Vec4(1.f, 0.f, 0.f, 1.f);
		m_pSubObj->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::VEC4_0, &vColor);
		m_pSubObj->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, pLocationTex.GetPointer());

		m_pSubObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, 2900.f));
		m_pSubObj->Transform()->SetLocalScale(Vec3(20.f, 20.f, 1.f));

		CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Invisible")->AddGameObject(m_pSubObj);

		m_fDamage = 0.f;
	}
		break;
	default:
		break;
	}
}


CToolItemScript::~CToolItemScript()
{
}

void CToolItemScript::Update()
{
	CItemScript::Update();
	if (m_eItemType == ITEM_MAP)
	{
		if (m_pObj && m_pSubObj && m_pHost)
		{
			Vec3 vPos = m_pHost->Transform()->GetLocalPos();
			Vec3 vRot = m_pHost->Transform()->GetLocalRot();
			Vec3 vLocationPos;

			vLocationPos.x = vPos.x / 22000.f * 600.f - 300.f;
			vLocationPos.y = vPos.z / 22000.f * 600.f - 300.f;
			vLocationPos.z = 2900.f;

			m_pSubObj->Transform()->SetLocalPos(vLocationPos);
			m_pSubObj->Transform()->SetLocalRot(Vec3(0.f, 0.f, -vRot.y + (XM_PI * 1.f)));
		}
	}
	else if (m_eItemType == ITEM_BOW)
	{
		if (!m_bCharge)
			return;

		if (KEY_HOLD(KEY_TYPE::KEY_LBTN) && m_bCharge)
		{
			m_pObj->GetScript<CArrowScript>()->Charge();
		}
		if (KEY_AWAY(KEY_TYPE::KEY_LBTN) && m_bCharge)
		{
			Vec3 vDir = m_pSubObj->Transform()->GetWorldDir(DIR_TYPE::FRONT);
			Vec3 vRot = m_pSubObj->Transform()->GetLocalRot();
			Vec3 vPos = m_pSubObj->Transform()->GetLocalPos();
			m_pObj->MeshRender()->SetDynamicShadow(true);
			m_bCharge = false;
			m_pObj->GetScript<CArrowScript>()->Shoot(vDir, vRot, vPos);

			// Regen
			Ptr<CMeshData> pTex = CResMgr::GetInst()->Load<CMeshData>(L"Arrow.mdat", L"MeshData\\arrow.mdat");
			m_pObj = pTex->Instantiate();

#ifdef CHECK_COLLISION
			m_pObj->AddComponent( new CCollider2D );

			m_pObj->Collider2D()->SetOffsetScale(Vec3(10.f, 10.f, 10.f));
			m_pObj->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::SPHERE);
#endif
			m_pObj->SetName(L"Arrow");
			m_pObj->Transform()->SetLocalPos(Vec3(10000.f, 400.f, 10000.f));
			//m_pObj->Transform()->SetLocalRot(Vec3(-XM_PI / 2.f, 0.f, 0.f));
			m_pObj->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
			CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Invisible")->AddGameObject(m_pObj);
		}
	}
}

UINT CToolItemScript::Use_Right(CGameObject* pHost, CGameObject* pObj, int num)
{
	
	switch (m_eItemType)
	{
	case ITEM_PICKAXE:
		break;
	case ITEM_AXE:
		break;
	case ITEM_HAMMER:
	{
		if (pObj == NULL)
			return 0;

		if (pObj->GetLayerIdx() == CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"House")->GetLayerIdx())
		{
			tEvent tEv;
			tEv.eType = EVENT_TYPE::DELETE_OBJECT;
			tEv.wParam = (DWORD_PTR)pObj;
			CEventMgr::GetInst()->AddEvent(tEv);
		}
	}
		break;
	case ITEM_MACHETTE:
		break;
	case ITEM_WOODCLUB:
		break;
	case ITEM_BOW:
		break;
	case ITEM_CAMPFIRE:
		break;
	case ITEM_MAP:
		break;
	default:
		break;
	}

	return m_eItemType;
}

UINT CToolItemScript::Use_Left(CGameObject* pHost, CGameObject* pObj, int num)
{
	CGameObject* pObject = nullptr;

	switch (m_eItemType)
	{
	case ITEM_PICKAXE:
	case ITEM_AXE:
	case ITEM_MACHETTE:
	case ITEM_WOODCLUB:
		if (pObj == NULL)
			break;
		// Animal Attack
		if (pObj->GetScript<CAnimalScript>())
		{
			if (!pObj->GetScript<CAnimalScript>()->GetAnimalDead())
			{
#ifdef NETWORK_ON	
				CPacketMgr::GetInst()->Send_Attack_Player_Packet(0, pObj->GetScript<CAnimalScript>()->GetIndex(), m_eItemType);
				pObj->GetScript<CAnimalScript>()->Damage(pHost, 0);
#else
				pObj->GetScript<CAnimalScript>()->Damage(pHost, m_fDamage);
#endif NETWORK_ON
			}
			else
			{
				int random = rand() % 3;
				int iAmount = 1;
				CItemScript* pItem = NULL;
				if (random == 0)
					pItem = new CStuffScript(ITEM_TYPE::ITEM_LEATHER);
				else if(random == 1)
					pItem = new CStuffScript(ITEM_TYPE::ITEM_BONE);
				else
					pItem = new CUsableScript(ITEM_TYPE::ITEM_MEAT);

				if (m_eItemType == ITEM_MACHETTE)
					iAmount = 3;
				pHost->GetScript<CPlayerScript>()->GetInventoryObject()->GetScript<CInventoryScript>()->AddItem(pItem, 1);

#ifdef NETWORK_ON	
				CPacketMgr::GetInst()->Send_Attack_Player_Packet( 0, pObj->GetScript<CAnimalScript>()->GetIndex(), m_eItemType);
#else
#endif NETWORK_ON
			}
		}
		// Environment Attack (Tree, Stone, Bush)
		else if (pObj->GetScript<CNaturalScript>())
		{
			if (!pObj->GetScript<CNaturalScript>()->GetDestroy())
			{
				NATURAL_TYPE eType = pObj->GetScript<CNaturalScript>()->GetNaturalType();

				if (eType == NATURAL_NONE)
				{
					return 0;
				}
#ifdef NETWORK_ON	
				CPacketMgr::GetInst()->Send_Attack_Player_Packet( 1, pObj->GetScript<CNaturalScript>()->GetIndex(), m_eItemType);
				pObj->GetScript<CNaturalScript>()->Damage(pHost, 0);
#else
				pObj->GetScript<CNaturalScript>()->Damage(pHost, m_fDamage);
#endif NETWORK_ON
				if (eType == NATURAL_TREE)
				{
					int iAmount = 1;

					int random = rand() % 5;
					if (random == 0)
					{
						CItemScript* pItem = new CUsableScript(ITEM_TYPE::ITEM_APPLE);
						pHost->GetScript<CPlayerScript>()->GetInventoryObject()->GetScript<CInventoryScript>()->AddItem(pItem, iAmount);
					}
					else
					{
						if (m_eItemType == ITEM_AXE)
							iAmount = 3;
						CItemScript* pItem = new CStuffScript(ITEM_TYPE::ITEM_WOOD);
						pHost->GetScript<CPlayerScript>()->GetInventoryObject()->GetScript<CInventoryScript>()->AddItem(pItem, iAmount);
					}
				}
				else if (eType == NATURAL_STONE)
				{
					int iAmount = 1;
					if (m_eItemType == ITEM_PICKAXE)
						iAmount = 3;
					CItemScript* pItem = new CStuffScript(ITEM_TYPE::ITEM_STONE);
					pHost->GetScript<CPlayerScript>()->GetInventoryObject()->GetScript<CInventoryScript>()->AddItem(pItem, iAmount);
				}
				else if (eType == NATURAL_BUSH)
				{
					int iAmount = 2;
					CItemScript* pItem = new CStuffScript(ITEM_TYPE::ITEM_CLOTH);
					pHost->GetScript<CPlayerScript>()->GetInventoryObject()->GetScript<CInventoryScript>()->AddItem(pItem, iAmount);
				}
			}
		}
		break;
	case ITEM_HAMMER:
	{
		if (pObj == NULL)
			return 0;

		if (pObj->GetLayerIdx() == CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"House")->GetLayerIdx())
		{
			int idx = pHost->GetScript<CPlayerScript>()->GetInventoryObject()->GetScript<CInventoryScript>()->CheckItem(ITEM_TYPE::ITEM_STONE, 3);
			if (idx == -1)
			{
				return 0;
			}

			if (pObj->GetScript<CBuildScript>()->Upgrade())
			{
				pHost->GetScript<CPlayerScript>()->GetInventoryObject()->GetScript<CInventoryScript>()->DecreaseItem(idx, 3);
			}
		}
	}
		break;
	case ITEM_BOW:
	{
		int idx = pHost->GetScript<CPlayerScript>()->GetInventoryObject()->GetScript<CInventoryScript>()->CheckItem(ITEM_TYPE::ITEM_ARROW, 1);
		if (idx == -1)
		{
			return 0;
		}
		if (m_pObj)
		{
			m_pObj->AddComponent(new CArrowScript);
			
			m_pSubObj = pHost->GetScript<CPlayerScript>()->GetMainCameraObject();
			Vec3 vDir = m_pSubObj->Transform()->GetWorldDir(DIR_TYPE::FRONT);
			Vec3 vRot = m_pSubObj->Transform()->GetLocalRot();
			Vec3 vPos = m_pSubObj->Transform()->GetLocalPos();

			m_pObj->GetScript<CArrowScript>()->Init(pHost, m_fDamage);
			m_bCharge = true;

			pHost->GetScript<CPlayerScript>()->GetInventoryObject()->GetScript<CInventoryScript>()->DecreaseItem(idx, 1);
		}
	}
		break;
	case ITEM_CAMPFIRE:
		if (m_pObj->GetScript<CBuildScript>()->Build())
		{
			m_pObj->MeshRender()->SetDynamicShadow(true);

			pObject = new CGameObject;
			pObject->AddComponent(new CTransform);
			pObject->AddComponent(new CParticleSystem);
			pObject->FrustumCheck(false);
			pObject->SetName(L"FireEffect");
			pObject->Transform()->SetLocalPos(Vec3(0.f, 0.f, 60.f));
			pObject->Transform()->SetLocalScale(Vec3(1.25f, 1.25f, 1.25f));
			m_pObj->AddChild(pObject);

			pObject = new CGameObject;
			pObject->AddComponent(new CTransform);
			pObject->AddComponent(new CParticleSystem);
			pObject->SetName(L"FireEffect2");
			pObject->FrustumCheck(false);
			pObject->Transform()->SetLocalPos(Vec3(0.f, 0.f, 40.f));
			pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
			pObject->ParticleSystem()->SetStartColor(Vec4(1.f, 0.7f, 0.f, 1.f));
			pObject->ParticleSystem()->SetEndColor(Vec4(0.5f, 0.1f, 0.f, 0.6f));
			m_pObj->AddChild(pObject);

			pObject = new CGameObject;
			pObject->AddComponent(new CTransform);
			pObject->AddComponent(new CLight3D);
			pObject->SetName(L"FireLight");

			pObject->Light3D()->SetLightPos(Vec3(0.f, 0.f, 30.f));
			pObject->Light3D()->SetLightType(LIGHT_TYPE::POINT);
			pObject->Light3D()->SetDiffuseColor(Vec3(0.9f, 0.5f, 0.1f));
			pObject->Light3D()->SetSpecular(Vec3(0.3f, 0.3f, 0.3f));
			pObject->Light3D()->SetAmbient(Vec3(0.1f, 0.1f, 0.1f));
			pObject->Light3D()->SetLightRange(2000.f);

			pObject->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
			pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
			m_pObj->AddChild(pObject);

			tEvent evt = {};

			evt.eType = EVENT_TYPE::TRANSFER_LAYER;
			evt.wParam = (DWORD_PTR)m_pObj;
			evt.lParam = ((DWORD_PTR)6 << 16 | (DWORD_PTR)true);

			CEventMgr::GetInst()->AddEvent(evt);

			if (!SetItemIncrease(-1))
			{
				pHost->GetScript<CPlayerScript>()->GetInventoryObject()->GetScript<CInventoryScript>()->OnAddable(num);
			}
			m_pObj = NULL;
		}
		break;
	case ITEM_MAP:
		break;
	default:
		break;
	}

	return m_eItemType;
}

void CToolItemScript::Use_Highlight(CGameObject* pHost, CGameObject* pObj, int num)
{
	switch (m_eItemType)
	{
	case ITEM_CAMPFIRE:
		if (m_pObj)
		{
			Vec3 vDir = pHost->Transform()->GetWorldDir(DIR_TYPE::FRONT);
			Vec3 vPos = pHost->Transform()->GetLocalPos();

			vPos += -vDir * 300.f;
			vPos.y = CNaviMgr::GetInst()->GetY(vPos);
			//vPos.y += 20;

			m_pObj->Transform()->SetLocalPos(vPos);
		}
		break;
	case ITEM_MAP:
		break;
	default:
		break;
	}
}

UINT CToolItemScript::EnableItem(CGameObject* pHost, int num)
{
	switch (m_eItemType)
	{
	case ITEM_CAMPFIRE:
		if (m_pObj)
		{
			tEvent evt = {};

			evt.eType = EVENT_TYPE::TRANSFER_LAYER;
			evt.wParam = (DWORD_PTR)m_pObj;
			evt.lParam = ((DWORD_PTR)5 << 16 | (DWORD_PTR)true);

			CEventMgr::GetInst()->AddEvent(evt);
		}
		break;
	case ITEM_MAP:
		if (m_pObj)
		{
			tEvent evt = {};

			evt.eType = EVENT_TYPE::TRANSFER_LAYER;
			evt.wParam = (DWORD_PTR)m_pObj;
			evt.lParam = ((DWORD_PTR)30 << 16 | (DWORD_PTR)true);

			CEventMgr::GetInst()->AddEvent(evt);

			tEvent evt2 = {};

			evt2.eType = EVENT_TYPE::TRANSFER_LAYER;
			evt2.wParam = (DWORD_PTR)m_pSubObj;
			evt2.lParam = ((DWORD_PTR)30 << 16 | (DWORD_PTR)true);

			CEventMgr::GetInst()->AddEvent(evt2);

			m_pHost = pHost;
		}
		break;
	default:
		break;
	}

	return m_eItemType;
}

void CToolItemScript::DisableItem(CGameObject* pHost, int num)
{
	switch (m_eItemType)
	{
	case ITEM_CAMPFIRE:
	case ITEM_MAP:
		if (m_pObj)
		{
			tEvent evt = {};

			evt.eType = EVENT_TYPE::TRANSFER_LAYER;
			evt.wParam = (DWORD_PTR)m_pObj;
			evt.lParam = ((DWORD_PTR)29 << 16 | (DWORD_PTR)true);

			CEventMgr::GetInst()->AddEvent(evt);

			tEvent evt2 = {};

			evt2.eType = EVENT_TYPE::TRANSFER_LAYER;
			evt2.wParam = (DWORD_PTR)m_pSubObj;
			evt2.lParam = ((DWORD_PTR)29 << 16 | (DWORD_PTR)true);

			CEventMgr::GetInst()->AddEvent(evt2);
		}
		break;
	default:
		break;
	}
}