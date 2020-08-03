#include "stdafx.h"

#include "PlayerScript.h"

#include "InputScript.h"
#include "InventoryScript.h"
#include "StatusScript.h"
#include "QuickSlotScript.h"

#include "AnimalScript.h"

#include "ItemScript.h"
#include "StuffScript.h"

#include "BuildScript.h"

#include <Engine/Camera.h>
#include <Engine/RenderMgr.h>
#include <Engine/NaviMgr.h>

#include <iostream>

CPlayerScript::CPlayerScript()
	: CScript((UINT)SCRIPT_TYPE::PLAYERSCRIPT)
	, m_fSpeed(200.f)
	, m_fDamage(30.f)
	, m_fScaleSpeed(1.f)
	, m_bEnable(true)
	, m_pChat(NULL)
	, m_pInventory(NULL)
	, m_pStatus(NULL)
	, m_bInvincible(false)
	, m_pMainCamera(NULL)
	, m_fAttackCoolTime(PLAYER_ATTACK_COOLTIME)
{
	m_vCollisionObj.reserve(5);
}

CPlayerScript::~CPlayerScript()
{
	m_vCollisionObj.clear();
}

void CPlayerScript::SetChatObject(CGameObject * pObj)
{
	m_pChat = pObj;
}

void CPlayerScript::SetInventoryObject(CGameObject * pObj)
{
	m_pInventory = pObj;
}

void CPlayerScript::SetStatusObject(CGameObject * pObj)
{
	m_pStatus = pObj;
}

void CPlayerScript::SetQuickSlot(CQuickSlotScript * pQuickSlot)
{
	m_pQuickSlot = pQuickSlot;
}

CGameObject * CPlayerScript::GetStatusObject()
{
	return m_pStatus;
}

CGameObject * CPlayerScript::GetInventoryObject()
{
	return m_pInventory;
}

CGameObject * CPlayerScript::GetChatObject()
{
	return m_pChat;
}

void CPlayerScript::EnableItem(int num)
{
	m_pInventory->GetScript<CInventoryScript>()->EnableItem(GetObj(), num);
}

void CPlayerScript::DisableItem(int num)
{
	m_pInventory->GetScript<CInventoryScript>()->DisableItem(GetObj(), num);
}

void CPlayerScript::SetMainCamera(CCamera * pCamera)
{
	m_pMainCamera = pCamera;
}

bool CPlayerScript::GetEnable()
{
	return m_bEnable;
}

void CPlayerScript::Awake()
{

}

void CPlayerScript::Update()
{
	if (m_pStatus->GetScript<CStatusScript>()->GetGameOver())
	{
		return;
	}

	Vec3 vPos = Transform()->GetLocalPos();
	Vec3 vOriginPos = Transform()->GetLocalPos();
	m_vPrevPos = vPos;
	float fSpeed = m_fSpeed;

	if(m_fAttackCoolTime > -1.f)
		m_fAttackCoolTime -= DT;
	

	if (m_pChat && m_pInventory)
	{
		// ä��â�� �����ְ� �κ��丮�� �����ִ� ���
		if (!m_pChat->GetScript<CInputScript>()->GetEnable() && !m_pInventory->GetScript<CInventoryScript>()->GetInventoryActive())
		{
			if (KEY_TAB(KEY_TYPE::KEY_LBTN))
			{
				PlayerPicking(LEFT_CLICK);
			}
			else if (KEY_TAB(KEY_TYPE::KEY_RBTN))
			{
				PlayerPicking(RIGHT_CLICK);
			}

			if (KEY_HOLD(KEY_TYPE::KEY_LSHIFT))
			{
				fSpeed *= 5.f;
			}

			if (KEY_HOLD(KEY_TYPE::KEY_W))
			{
				Vec3 vFront = -Transform()->GetWorldDir(DIR_TYPE::FRONT);
				vPos += vFront * fSpeed * DT;
			}

			if (KEY_HOLD(KEY_TYPE::KEY_S))
			{
				Vec3 vBack = Transform()->GetWorldDir(DIR_TYPE::FRONT);
				vPos += vBack * fSpeed * DT;
			}

			if (KEY_HOLD(KEY_TYPE::KEY_A))
			{
				Vec3 vLeft = Transform()->GetWorldDir(DIR_TYPE::RIGHT);
				vPos += vLeft * fSpeed * DT;
			}

			if (KEY_HOLD(KEY_TYPE::KEY_D))
			{
				Vec3 vRight = -Transform()->GetWorldDir(DIR_TYPE::RIGHT);
				vPos += vRight * fSpeed * DT;
			}

			POINT vMousePos = CKeyMgr::GetInst()->GetMousePos();
			tResolution vResolution = CRenderMgr::GetInst()->GetResolution();
			Vec2 vCenter = Vec2(vResolution.fWidth / 2.f, vResolution.fHeight / 2.f);

			Vec2 vDrag = Vec2((float)(vMousePos.x - vCenter.x), (float)(vCenter.y - vMousePos.y));

			Vec3 vRot = Transform()->GetLocalRot();

			//std::cout << vDrag.y << std::endl;
			// x -= 18, y -= 41
			vDrag.x += 18.f;

			if (vDrag.x != 0.f)
			{
				int num = m_pQuickSlot->GetSelect();
				m_pInventory->GetScript<CInventoryScript>()->Use_Highlight(GetObj(), NULL, num);
			}

			vRot.y += vDrag.x * DT * 0.5f;

			if (vRot.y > 360.f)
				vRot.y -= 360.f;

			Transform()->SetLocalRot(Vec3(0.f, vRot.y, 0.f));

			float fHeight = CNaviMgr::GetInst()->GetY(Transform()->GetWorldPos());
			float fMaxHeight = max(m_fHouseHeight, fHeight);
			if (vOriginPos.y > fMaxHeight + 5.f)
			{
				vOriginPos.y -= m_fSpeed * DT * 5.f;
			}
			else
			{
				vOriginPos.y = fMaxHeight;
			}

			Transform()->SetLocalPos( Vec3(vPos.x, vOriginPos.y, vPos.z) );

			SetCursorPos(vCenter.x, vCenter.y);
			m_fHouseHeight = 0.f;
			m_bEnable = true;
		}
		else
			m_bEnable = false;
	}
	else
	{
		if (KEY_HOLD(KEY_TYPE::KEY_LSHIFT))
		{
			fSpeed *= 5.f;
		}

		if (KEY_HOLD(KEY_TYPE::KEY_W))
		{
			Vec3 vFront = Transform()->GetWorldDir(DIR_TYPE::FRONT);
			vPos += vFront * fSpeed * DT;
		}

		if (KEY_HOLD(KEY_TYPE::KEY_S))
		{
			Vec3 vBack = -Transform()->GetWorldDir(DIR_TYPE::FRONT);
			vPos += vBack * fSpeed * DT;
		}

		if (KEY_HOLD(KEY_TYPE::KEY_A))
		{
			Vec3 vLeft = -Transform()->GetWorldDir(DIR_TYPE::RIGHT);
			vPos += vLeft * fSpeed * DT;
		}

		if (KEY_HOLD(KEY_TYPE::KEY_D))
		{
			Vec3 vRight = Transform()->GetWorldDir(DIR_TYPE::RIGHT);
			vPos += vRight * fSpeed * DT;
		}

		Vec2 vDrag = CKeyMgr::GetInst()->GetDragDir();
		Vec3 vRot = Transform()->GetLocalRot();

		vRot.y += vDrag.x * DT * 1.5f;

		if (vRot.y > 360.f)
			vRot.y -= 360.f;

		Transform()->SetLocalRot(Vec3(0.f, vRot.y, 0.f));

		float fHeight = CNaviMgr::GetInst()->GetY(Transform()->GetWorldPos());
		if (fHeight > m_fHeight)
			m_fHeight = fHeight;
		vPos.y = m_fHeight;

		Transform()->SetLocalPos( vPos );

		m_bEnable = true;
	}
}

void CPlayerScript::OnCollision(CCollider2D * _pOther)
{
	// �÷��̾��� �ൿ�ݰ� ( ����, �Ǽ� �� ���� ������Ʈ�� 

	// �÷��̾�� �浹�Ѱ� ������ �ƴ� ������ - 
	if (CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Animal")->GetLayerIdx() != _pOther->GetObj()->GetLayerIdx())
	{
		// �ǹ��� �ε������� �ǹ��� AABB�� �浹üũ ����
		if (CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"House")->GetLayerIdx() == _pOther->GetObj()->GetLayerIdx())
		{
			HOUSING_TYPE eType = _pOther->GetObj()->GetScript<CBuildScript>()->GetHousingType();
			if (eType >= HOUSING_TYPE::HOUSING_FOUNDATION && eType < HOUSING_TYPE::HOUSING_END)
			{
				bool bCollision = false;
				bCollision = CollisionHouse(_pOther, _pOther->GetObj()->GetScript<CBuildScript>()->GetOffsetScale(), eType);
				if (bCollision)
				{
					Vec3 vPos = Transform()->GetWorldPos();
					Vec3 vBuildPos = _pOther->Transform()->GetLocalPos();

					float fDiff = vBuildPos.y - vPos.y;
					if (fDiff < 30.f && (eType == HOUSING_FOUNDATION || eType == HOUSING_FLOOR))
					{
						if (m_fHouseHeight < vBuildPos.y)
							m_fHouseHeight = vBuildPos.y;
					}
					else
					{
						Transform()->SetLocalPos(m_vPrevPos);
					}
				}
			}
			return;
		}

		// ������ �浹ũ���� ������Ʈ�� �ε�������
		if (CollisionSphere(_pOther, _pOther->GetOffsetScale(), 0.2f))
		{
			Vec3 vOtherPos = _pOther->Transform()->GetLocalPos();

			Vec3 vPos = Transform()->GetLocalPos();

			Vec3 vDir = XMVector3Normalize(vPos - vOtherPos);
			vDir.y = 0.f;

			if (KEY_HOLD(KEY_TYPE::KEY_LSHIFT))
			{
				vPos += vDir * m_fSpeed * DT * 5.f;
			}
			else
			{
				vPos += vDir * m_fSpeed * DT;
			}

			Vec3 vRot = _pOther->Transform()->GetLocalRot();

			Transform()->SetLocalPos(vPos);
		}
	}
	else
	{
		// ���� ������ ũ��� �ε�������
		Vec3 vOffsetScale = _pOther->GetObj()->GetScript<CAnimalScript>()->GetOffsetScale();
		if (CollisionSphere(_pOther, vOffsetScale, 0.2f))
		{
			Vec3 vOtherPos = _pOther->Transform()->GetLocalPos();

			Vec3 vPos = Transform()->GetLocalPos();

			Vec3 vDir = XMVector3Normalize(vPos - vOtherPos);
			vDir.y = 0.f;

			if (KEY_HOLD(KEY_TYPE::KEY_LSHIFT))
			{
				vPos += vDir * m_fSpeed * DT * 5.f;
			}
			else
			{
				vPos += vDir * m_fSpeed * DT;
			}

			Vec3 vRot = _pOther->Transform()->GetLocalRot();

			Transform()->SetLocalPos(vPos);
		}
	}
}

void CPlayerScript::OnCollisionEnter(CCollider2D * _pOther)
{
	// ��ŷ���� ���������� �ϱ� ���� �ݰ������ ������ ���Ϳ� ����
	m_vCollisionObj.emplace_back(_pOther->GetObj());
}

void CPlayerScript::OnCollisionExit(CCollider2D * _pOther)
{
	//if (_pOther->GetObj()->GetLayerIdx() == CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"House")->GetLayerIdx())
	//{
	//	if (_pOther->GetObj()->GetScript<CBuildScript>()->GetHousingType() == HOUSING_FOUNDATION)
	//	{
	//		m_bOnHouse = false;
	//	}
	//}
	// ������ ���Ϳ��� ����
	auto p = find(m_vCollisionObj.begin(), m_vCollisionObj.end(), _pOther->GetObj());
	if (p == m_vCollisionObj.end())
	{
		// �������� �ִµ� ��������� ���� ���?
	}
	m_vCollisionObj.erase(p);
}

void CPlayerScript::Damage(float fDamage)
{
	if (!m_bInvincible)
	{
		m_pStatus->GetScript<CStatusScript>()->Damage(fDamage);
	}
}

void CPlayerScript::PlayerPicking(bool bLeft)
{
	if (m_fAttackCoolTime > 0.f)
		return;
	else
	{
		m_fAttackCoolTime = PLAYER_ATTACK_COOLTIME;
	}
	POINT vPoint = CKeyMgr::GetInst()->GetMousePos();

	tResolution vResolution = CRenderMgr::GetInst()->GetResolution();
	//Vec2 vPoint = Vec2(vResolution.fWidth / 2.f, vResolution.fHeight / 2.f);

	Matrix matProj = m_pMainCamera->GetProjMat();

	Vec3 vDirRay, vPosRay;
	vDirRay.x = ((2.0f * (float)vPoint.x) / vResolution.fWidth - 1.f) / matProj._11;
	vDirRay.y = ((-2.0f * (float)vPoint.y) / vResolution.fHeight + 1.f) / matProj._22;
	vDirRay.z = 1.f;

	Matrix matView = m_pMainCamera->GetViewMat();
	Matrix matViewInv = m_pMainCamera->GetViewMatInv();

	vPosRay = XMVector3TransformCoord(vPosRay, matViewInv);
	vDirRay = XMVector3TransformNormal(vDirRay, matViewInv);
	vDirRay = XMVector3Normalize(vDirRay);

	CGameObject* pCollider = NULL;
	// �浹�ݰ� �ȿ��ִ� ��ü��� ��ŷ�� ����
	for (int i = 0; i < m_vCollisionObj.size(); ++i)
	{
		if (CollisionRay(vPosRay, vDirRay, m_vCollisionObj[i]->Collider2D()))
		{
			pCollider = m_vCollisionObj[i];
			break;
		}
	}
	int num = m_pQuickSlot->GetSelect();
	if(bLeft)
		m_pInventory->GetScript<CInventoryScript>()->Use_Left(GetObj(), pCollider, num);
	else
		m_pInventory->GetScript<CInventoryScript>()->Use_Right(GetObj(), pCollider, num);
}


bool CPlayerScript::CollisionSphere(CCollider2D* _pOther, Vec3 vOffsetScale, float fOffset)
{
	const Matrix& matCol1 = Collider2D()->GetColliderWorldMat();
	const Matrix& matCol2 = _pOther->GetColliderWorldMat();

	Vec3 vCol1 = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matCol1);
	Vec3 vCol2 = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matCol2);

	Vec3 vScale1 = Transform()->GetLocalScale();
	Vec3 vScale2 = _pOther->Transform()->GetLocalScale();

	Vec3 vColScale1 = Collider2D()->GetOffsetScale() * fOffset;
	Vec3 vColScale2 = vOffsetScale;

	float fDist = pow(vCol2.x - vCol1.x, 2) + pow(vCol2.y - vCol1.y, 2) + pow(vCol2.z - vCol1.z, 2);
	fDist = sqrt(fDist);

	if (fDist > fabs(vScale1.x * vColScale1.x) + fabs(vScale2.x * vColScale2.x))
		return false;

	return true;
}

bool CPlayerScript::CollisionRay(Vec3 vPosRay, Vec3 vDirRay, CCollider2D * _pOther)
{
	Matrix matWorldInv = XMMatrixIdentity();
	Vec3 vPos = _pOther->Transform()->GetLocalPos();
	vPos += _pOther->Collider2D()->GetOffsetPos();
	matWorldInv._41 = -vPos.x;
	matWorldInv._42 = -vPos.y;
	matWorldInv._43 = -vPos.z;

	vPosRay = XMVector3TransformCoord(vPosRay, matWorldInv);
	vDirRay = XMVector3TransformNormal(vDirRay, matWorldInv);

	Vec3 vv = XMVector3Dot(vDirRay, vDirRay);
	Vec3 qv = XMVector3Dot(vPosRay, vDirRay);
	Vec3 qq = XMVector3Dot(vPosRay, vPosRay);

	float r;
	if (CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Animal")->GetLayerIdx() != _pOther->GetObj()->GetLayerIdx())
	{
		r = _pOther->Transform()->GetLocalScale().x * _pOther->GetOffsetScale().x;
	}
	else
	{
		r = _pOther->Transform()->GetLocalScale().x * _pOther->GetObj()->GetScript<CAnimalScript>()->GetOffsetScale().x;
	}

	float rr = r * r;

	if (qv.x * qv.x - vv.x * (qq.x - rr) >= 0)
	{
		return true;
	}
	return false;
}

bool CPlayerScript::CollisionHouse(CCollider2D* _pOther, Vec3 vOffsetScale, UINT iType)
{
	const Matrix& matCol1 = Collider2D()->GetColliderWorldMat();
	const Matrix& matCol2 = _pOther->GetColliderWorldMat();

	Vec3 vCol1 = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matCol1);
	Vec3 vCol2 = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matCol2);

	Vec3 vScale1 = Transform()->GetLocalScale();
	Vec3 vScale2 = _pOther->Transform()->GetLocalScale();

	Vec3 vColScale1 = Vec3(20.f, 60.f, 20.f);
	Vec3 vColScale2 = Vec3(1.f, 1.f, 1.f);

	HOUSING_TYPE eType = (HOUSING_TYPE)iType;

	switch (eType)
	{
	case HOUSING_FOUNDATION:
		vColScale2 = vOffsetScale;
		break;
	case HOUSING_WALL:
	case HOUSING_WINDOW:
	{
		Vec3 vRot = _pOther->Transform()->GetLocalRot();
		if (vRot.y != 0.f)
		{
			vColScale2 = Vec3(vOffsetScale.z, vOffsetScale.y, vOffsetScale.x);
			//std::cout << "Turn" << vOffsetScale.x << std::endl;
		}
		else
		{
			vColScale2 = Vec3(vOffsetScale.x, vOffsetScale.y, vOffsetScale.z);
		}
	}
		break;
	case HOUSING_DOOR:
	{
		Vec3 vRot = _pOther->Transform()->GetLocalRot();
		Vec3 vOffsetPos = Vec3(0.f, 0.f, 0.f);
		if (vRot.y != 0.f)
		{
			vColScale2 = Vec3(vOffsetScale.z / 3.5f, vOffsetScale.y / 3.5f, vOffsetScale.x);
			vOffsetPos = Vec3(80.f, 0.f, 0.f);
		}
		else
		{
			// vOffsetScale.x = 20.f;
			vColScale2 = Vec3(vOffsetScale.x, vOffsetScale.y / 3.5f, vOffsetScale.z / 3.5f);
			vOffsetPos = Vec3(0.f, 0.f, 80.f);
		}
		if (CollisionHouse_Door(_pOther, vColScale2, vOffsetPos))
			return true;
		if (CollisionHouse_Door(_pOther, vColScale2, -vOffsetPos))
			return true;

		return false;
	}
		break;
	case HOUSING_FLOOR:
	{
		vColScale2 = vOffsetScale;
	}
		break;
	default:
		vColScale2 = vOffsetScale;
		break;
	}

	vScale1 *= vColScale1;
	vScale2 *= vColScale2;

	Vec3 vMax, vOtherMax;
	Vec3 vMin, vOtherMin;

	Vec3 vPos = Transform()->GetLocalPos();
	vPos.y += 50.f;

	vMax = vPos + vScale1;
	vMin = vPos - vScale1;
	vOtherMax = vCol2 + vScale2;
	vOtherMin = vCol2 - vScale2;

	//std::cout << "Player : " << vPos.x << " || " << vPos.y << " || " << vPos.z << " || " << std::endl;
	//std::cout << "House : " << vCol2.x << " || " << vCol2.y << " || " << vCol2.z << " || " << std::endl;

	if (vMax.x < vOtherMin.x || vMin.x > vOtherMax.x) return false;
	if (vMax.y < vOtherMin.y || vMin.y > vOtherMax.y) return false;
	if (vMax.z < vOtherMin.z || vMin.z > vOtherMax.z) return false;

	return true;
}

bool CPlayerScript::CollisionHouse_Door(CCollider2D * _pOther, Vec3 vOffsetScale, Vec3 vOffsetPos)
{
	const Matrix& matCol1 = Collider2D()->GetColliderWorldMat();
	const Matrix& matCol2 = _pOther->GetColliderWorldMat();

	Vec3 vCol1 = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matCol1);
	Vec3 vCol2 = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matCol2);

	Vec3 vScale1 = Transform()->GetLocalScale();
	Vec3 vScale2 = _pOther->Transform()->GetLocalScale();

	Vec3 vColScale1 = Vec3(20.f, 60.f, 20.f);

	vScale1 *= vColScale1;
	vScale2 *= vOffsetScale;

	Vec3 vMax, vOtherMax;
	Vec3 vMin, vOtherMin;

	Vec3 vPos = Transform()->GetLocalPos();
	vPos.y += 50.f;

	vCol2 += vOffsetPos;

	vMax = vPos + vScale1;
	vMin = vPos - vScale1;
	vOtherMax = vCol2 + vScale2;
	vOtherMin = vCol2 - vScale2;

	if (vMax.x < vOtherMin.x || vMin.x > vOtherMax.x) return false;
	if (vMax.y < vOtherMin.y || vMin.y > vOtherMax.y) return false;
	if (vMax.z < vOtherMin.z || vMin.z > vOtherMax.z) return false;

	return true;
}
