#include "stdafx.h"
#include "UsableScript.h"
#include "StatusScript.h"
#include "PlayerScript.h"
#include "InventoryScript.h"

#include <Engine/NaviMgr.h>

#include <iostream>

#include "PacketMgr.h"

CUsableScript::CUsableScript(ITEM_TYPE eType, int iCount)
	: CItemScript((UINT)eType)
{
	switch (eType)
	{
	case ITEM_MEAT:
		m_fValue = 15.f;
		break;
	case ITEM_COOKMEAT:
		m_fValue = 25.f;
		break;
	case ITEM_CHOCOLATEBAR:
		m_fValue = 50.f;
		break;
	case ITEM_CAN:
		m_fValue = 25.f;
		break;
	case ITEM_APPLE:
		m_fValue = 15.f;
		break;
	case ITEM_EMPTY_BOTTLE:
		m_fValue = 0.f;
		break;
	case ITEM_WATER_BOTTLE:
		m_fValue = 25.f;
		break;
	case ITEM_MEDICKIT:
		m_fValue = 100.f;
		break;
	case ITEM_BANDAGE:
		m_fValue = 10.f;
		break;
	default:
		m_fValue = 0;
		break;
	}
	m_iMaxCount = USABLE_MAX_COUNT;
	if (iCount > m_iMaxCount)
		iCount = USABLE_MAX_COUNT;
	m_iCount = iCount;
}


CUsableScript::~CUsableScript()
{
}

void CUsableScript::Update()
{
	CItemScript::Update();
}

UINT CUsableScript::Use_Right(CGameObject* pHost, CGameObject* pObj, int num)
{
	
	if (pObj == NULL)
		return 0;

	if (m_eItemType == ITEM_MEAT && pObj->GetName() == L"Campfire")
	{
		CItemScript* pItem = new CUsableScript(ITEM_COOKMEAT);
		pHost->GetScript<CPlayerScript>()->GetInventoryObject()->GetScript<CInventoryScript>()->AddItem(pItem, 1);

		if (!SetItemIncrease(-1))
		{
			pHost->GetScript<CPlayerScript>()->GetInventoryObject()->GetScript<CInventoryScript>()->OnAddable(num);
		}
	}

	else if (m_eItemType == ITEM_EMPTY_BOTTLE)
	{
		Vec3 vPos = pHost->Transform()->GetLocalPos();
		Vec3 vDir = pHost->Transform()->GetWorldDir(DIR_TYPE::FRONT);
		vPos += -vDir * 80.f;
		vPos.y = CNaviMgr::GetInst()->GetY(vPos);
		if (vPos.y <= 120.f)
		{
			CItemScript* pItem = new CUsableScript(ITEM_WATER_BOTTLE);
			pHost->GetScript<CPlayerScript>()->GetInventoryObject()->GetScript<CInventoryScript>()->AddItem(pItem, 1);

			if (!SetItemIncrease(-1))
			{
				pHost->GetScript<CPlayerScript>()->GetInventoryObject()->GetScript<CInventoryScript>()->OnAddable(num);
			}
		}
	}

	return m_eItemType;
}

UINT CUsableScript::Use_Left(CGameObject* pHost, CGameObject* pObj, int num)
{
	bool bUse = true;
	if (m_eItemType > ITEM_FOOD && m_eItemType < ITEM_FOOD_END)
	{
		if (m_eItemType == ITEM_MEAT)
		{
#ifdef NETWORK_ON
			CPacketMgr::GetInst()->Send_Use_Item_Packet(m_eItemType);
#else
			pHost->GetScript<CPlayerScript>()->GetStatusObject()->GetScript<CStatusScript>()->Damage(m_fValue);
			pHost->GetScript<CPlayerScript>()->GetStatusObject()->GetScript<CStatusScript>()->SetIncreaseHungry(m_fValue);
#endif // NETWORK_ON
		}
		else
		{
#ifdef NETWORK_ON
			CPacketMgr::GetInst()->Send_Use_Item_Packet(m_eItemType);
#else
			pHost->GetScript<CPlayerScript>()->GetStatusObject()->GetScript<CStatusScript>()->SetIncreaseHungry(m_fValue);
#endif // NETWORK_ON
		}
	}
	else if (m_eItemType > ITEM_DRINK && m_eItemType < ITEM_DRINK_END)
	{
		if (m_eItemType == ITEM_EMPTY_BOTTLE)
		{
			Vec3 vPos = pHost->Transform()->GetLocalPos();
			Vec3 vDir = pHost->Transform()->GetWorldDir(DIR_TYPE::FRONT);
			vPos += -vDir * 80.f;
			vPos.y = CNaviMgr::GetInst()->GetY(vPos);
			if (vPos.y <= 120.f)
			{
				CItemScript* pItem = new CUsableScript(ITEM_WATER_BOTTLE);
				pHost->GetScript<CPlayerScript>()->GetInventoryObject()->GetScript<CInventoryScript>()->AddItem(pItem, 1);
			}
			else
			{
				bUse = false;
			}
		}
		else
		{
#ifdef NETWORK_ON
			CPacketMgr::GetInst()->Send_Use_Item_Packet(m_eItemType);
#else
			pHost->GetScript<CPlayerScript>()->GetStatusObject()->GetScript<CStatusScript>()->SetIncreasefThirst(m_fValue);
#endif // NETWORK_ON
			if (m_eItemType == ITEM_WATER_BOTTLE)
			{
				CItemScript* pItem = new CUsableScript(ITEM_EMPTY_BOTTLE);
				pHost->GetScript<CPlayerScript>()->GetInventoryObject()->GetScript<CInventoryScript>()->AddItem(pItem, 1);
			}
		}
	}
	else if (m_eItemType > ITEM_HEAL && m_eItemType < ITEM_HEAL_END)
	{
#ifdef NETWORK_ON
		CPacketMgr::GetInst()->Send_Use_Item_Packet(m_eItemType);
#else
		pHost->GetScript<CPlayerScript>()->GetStatusObject()->GetScript<CStatusScript>()->SetIncreaseHealth(m_fValue);
#endif // NETWORK_ON
	}

	if (bUse)
	{
		if (!SetItemIncrease(-1))
		{
			pHost->GetScript<CPlayerScript>()->GetInventoryObject()->GetScript<CInventoryScript>()->OnAddable(num);
		}
	}

	return m_eItemType;
}

void CUsableScript::Use_Highlight(CGameObject* pHost, CGameObject* pObj, int num)
{

}