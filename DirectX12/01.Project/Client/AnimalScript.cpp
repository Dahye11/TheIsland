#include "stdafx.h"
#include "AnimalScript.h"
#include "PlayerScript.h"

#include <Engine/ParticleSystem.h>
#include <Engine/NaviMgr.h>

#include <iostream>

CAnimalScript::CAnimalScript()
	: CScript((UINT)SCRIPT_TYPE::ANIMALSCRIPT)
	, m_tStatus {
		100.f, // HP
		100.f, // Stamina
		200.f, // Speed
		10.f,  // Damage
		2.f, // BehaviorTime
		BEHAVIOR_TYPE::B_EVASION, // Type
		ANIMAL_TYPE::A_DEER // Kind
	}
	, m_vOffsetScale(Vec3(20.f, 20.f, 20.f))
	, m_bBehavior(false)
	, m_pParticleObj(NULL)
	, m_bAnimalDead(false)
	, m_fCurrentTime(0.f)
	, m_vMoveDir(Vec3(0.f, 0.f, 0.f))
	, m_pTarget(NULL)
	, m_fAttackCoolTime(3.f)
	, m_fAttackTime(0.f)
	, m_bIdleBehavior(false)
	, m_fIdleBehaviorTime(3.f)
{

}

CAnimalScript::~CAnimalScript()
{
}

void CAnimalScript::Update()
{
	if (m_pParticleObj != NULL)
	{
		m_fParticleTime -= DT;
		if (m_fParticleTime < 0.f)
		{
			tEvent tEv;
			tEv.eType = EVENT_TYPE::DELETE_OBJECT;
			tEv.wParam = (DWORD_PTR)m_pParticleObj;
			CEventMgr::GetInst()->AddEvent(tEv);
			m_pParticleObj = NULL;
		}
	}
	if (m_bAnimalDead)
	{
		m_fLivingTime -= DT;
		if (m_fLivingTime < 0.f)
		{
			tEvent tEv;
			tEv.eType = EVENT_TYPE::DELETE_OBJECT;
			tEv.wParam = (DWORD_PTR)GetObj();
			CEventMgr::GetInst()->AddEvent(tEv);
		}
		return;
	}
	if (m_bIdleBehavior)
	{
		// ������ �ƹ��� ������ ���� �ð��� �帥 �� �ϴ� �ൿ
		m_fIdleBehaviorTime -= DT;
		if (m_fIdleBehaviorTime < 0.f)
		{
			m_bIdleBehavior = false;
			// ���� �ൿ�� ���� �ڿ� �� ������
			m_fIdleBehaviorTime = (rand () / (float)RAND_MAX * 5.f) + 3.f;
		}
		Vec3 vPos = Transform()->GetLocalPos();

		vPos += m_vMoveDir * m_tStatus.fSpeed * DT;

		if (ANIMAL_TYPE::A_BEAR == m_tStatus.eKind || ANIMAL_TYPE::A_BOAR == m_tStatus.eKind)
			Transform()->SetLocalRot(Vec3(-XM_PI / 2.f, atan2(m_vMoveDir.x, m_vMoveDir.z) + 3.141592f, 0.f));
		else
		{
			if (ANIMAL_TYPE::A_WOLF == m_tStatus.eKind)
				Transform()->SetLocalRot(Vec3(0.f, atan2(m_vMoveDir.x, m_vMoveDir.z), 0.f));
			else
				Transform()->SetLocalRot(Vec3(0.f, atan2(m_vMoveDir.x, m_vMoveDir.z) + 3.141592f, 0.f));
		}

		vPos.y = CNaviMgr::GetInst()->GetY(Transform()->GetWorldPos());

		Transform()->SetLocalPos(vPos);

		return;
	}

	if (m_fAttackTime > -1.f)
	{
		m_fAttackTime -= DT;
	}

	if (!m_bBehavior)
	{
		// �þ߿� ���� �ƹ��� ���ٸ�? ���ð� ����
		m_fIdleBehaviorTime -= DT;
		if (m_fIdleBehaviorTime < 0.f)
		{
			// ������ ������ ��� ���ʵ��� �����ϰ���
			m_bIdleBehavior = true;
			m_fIdleBehaviorTime = (rand() / (float)RAND_MAX * 5.f) + 3.f;;
			m_vMoveDir = Vec3(rand() / (float)RAND_MAX, 0.f, rand() / (float)RAND_MAX);
			m_vMoveDir = XMVector3Normalize(m_vMoveDir);
		}
		return;
	}

	m_fCurrentTime -= DT;
	if (m_fCurrentTime < 0.f)
	{
		m_bBehavior = false;
		m_fCurrentTime = 0.f;
		m_fIdleBehaviorTime = (rand() / (float)RAND_MAX * 5.f) + 3.f;;
	}

	// �÷��̾ �þ߹����� ��� ��� � �ൿ�� ��� ���� ���ΰ�
	// ȸ���� ���� ( �罿 )
	if (BEHAVIOR_TYPE::B_EVASION == m_tStatus.eType)
	{
		// ������ �������� ������ �޸�
		Vec3 vPos = Transform()->GetLocalPos();

		vPos += m_vMoveDir * m_tStatus.fSpeed * DT;

		vPos.y = CNaviMgr::GetInst()->GetY(Transform()->GetWorldPos());

		Transform()->SetLocalPos(vPos);
	}
	else if (BEHAVIOR_TYPE::B_PASSIVE == m_tStatus.eType)
	{
		// �񼱰� ����
		Vec3 vOtherPos = m_pTarget->Transform()->GetLocalPos();

		Vec3 vPos = Transform()->GetLocalPos();
		Vec3 vTempPos = vPos;

		Vec3 vDir = XMVector3Normalize(vOtherPos - vPos);
		vDir.y = 0.f;

		vPos += vDir * m_tStatus.fSpeed * DT;

		Vec3 vRot = m_pTarget->Transform()->GetLocalRot();

		if(ANIMAL_TYPE::A_BOAR == m_tStatus.eKind)
			Transform()->SetLocalRot(Vec3(-XM_PI / 2.f, atan2(vDir.x, vDir.z) + 3.141592f, 0.f));
		else
			Transform()->SetLocalRot(Vec3(0.f, atan2(vDir.x, vDir.z), 0.f));

		vPos.y = CNaviMgr::GetInst()->GetY(Transform()->GetWorldPos());

		Transform()->SetLocalPos(vPos);
	}
	else if (BEHAVIOR_TYPE::B_WARLIKE == m_tStatus.eType)
	{
		// ������
		// �i�ư��� �Ҵ�
		Vec3 vOtherPos = m_pTarget->Transform()->GetLocalPos();

		Vec3 vPos = Transform()->GetLocalPos();
		Vec3 vTempPos = vPos;

		Vec3 vDir = XMVector3Normalize(vOtherPos - vPos);
		vDir.y = 0.f;

		vPos += vDir * m_tStatus.fSpeed * DT;

		Vec3 vRot = m_pTarget->Transform()->GetLocalRot();

		Transform()->SetLocalRot(Vec3(-XM_PI / 2.f, atan2(vDir.x, vDir.z) + 3.141592f, 0.f));

		vPos.y = CNaviMgr::GetInst()->GetY(Transform()->GetWorldPos());

		Transform()->SetLocalPos(vPos);
	}
}

void CAnimalScript::OnCollision(CCollider2D * _pOther)
{
	if (m_bAnimalDead)
		return;

	// �÷��̾ �ƴ� �ٸ� ��ü ( ȯ���� )
	if (CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Player")->GetLayerIdx() != _pOther->GetObj()->GetLayerIdx())
	{
		if (CollisionSphere(m_vOffsetScale, _pOther)) // ȯ���Ҷ��� �ڱ� ���븸ŭ �߰� �浹üũ�� �����ϰ� ������ �ݴ�� ƨ�ܳ�������
		{
			Vec3 vOtherPos = _pOther->Transform()->GetLocalPos();

			Vec3 vPos = Transform()->GetLocalPos();

			Vec3 vDir = XMVector3Normalize(vPos - vOtherPos);
			vDir.y = 0.f;

			vPos += vDir * m_tStatus.fSpeed * DT;

			Vec3 vRot = _pOther->Transform()->GetLocalRot();

			//Transform()->SetLocalRot(Vec3(0.f, atan2(vDir.x, vDir.z) + 3.141592f, 0.f));
			vPos.y = CNaviMgr::GetInst()->GetY(Transform()->GetWorldPos());

			Transform()->SetLocalPos(vPos);
		}

		// Ÿ�Կ� ���� �߰� �ൿ ��� ������ ��

		return;
	}

	// ȸ���� ���� ( �罿 )
	if (BEHAVIOR_TYPE::B_EVASION == m_tStatus.eType)
	{
		Vec3 vOtherPos = _pOther->Transform()->GetLocalPos();

		Vec3 vPos = Transform()->GetLocalPos();

		Vec3 vDir = XMVector3Normalize(vPos - vOtherPos);
		vDir.y = 0.f;

		vPos += vDir * m_tStatus.fSpeed * DT;

		Vec3 vRot = _pOther->Transform()->GetLocalRot();

		Transform()->SetLocalRot(Vec3(0.f, atan2(vDir.x, vDir.z) + 3.141592f, 0.f));
		vPos.y = CNaviMgr::GetInst()->GetY(Transform()->GetWorldPos());

		Transform()->SetLocalPos(vPos);
	}
	else if (BEHAVIOR_TYPE::B_PASSIVE == m_tStatus.eType)
	{
		// �񼱰� ���� ������ �þ߿� �÷��̾ ��������
		if (m_bBehavior)
		{
			m_fCurrentTime = m_tStatus.fBehaviorTime;
		}
		if (CollisionSphere(m_vOffsetScale, _pOther, 0.2f))
		{
			Vec3 vOtherPos = _pOther->Transform()->GetLocalPos();

			Vec3 vPos = Transform()->GetLocalPos();

			Vec3 vDir = XMVector3Normalize(vPos - vOtherPos);
			vDir.y = 0.f;

			vPos += vDir * m_tStatus.fSpeed * DT;

			Vec3 vRot = _pOther->Transform()->GetLocalRot();

			vPos.y = CNaviMgr::GetInst()->GetY(Transform()->GetWorldPos());

			Transform()->SetLocalPos(vPos);

			if (_pOther->GetObj() == m_pTarget)
			{
				if (m_fAttackTime < 0.f)
				{
					m_pTarget->GetScript<CPlayerScript>()->Damage(m_tStatus.fDamage);
					m_fAttackTime = m_fAttackCoolTime;
				}
			}
		}

	}
	else if (BEHAVIOR_TYPE::B_WARLIKE == m_tStatus.eType)
	{
		// ������
		Vec3 vOtherPos = _pOther->Transform()->GetLocalPos();

		Vec3 vPos = Transform()->GetLocalPos();
		Vec3 vTempPos = vPos;

		Vec3 vDir = XMVector3Normalize(vOtherPos - vPos);
		vDir.y = 0.f;

		vPos += vDir * m_tStatus.fSpeed * DT;

		Vec3 vRot = _pOther->Transform()->GetLocalRot();

		Transform()->SetLocalRot(Vec3(-XM_PI / 2.f, atan2(vDir.x, vDir.z) + 3.141592f, 0.f));

		vPos.y = CNaviMgr::GetInst()->GetY(Transform()->GetWorldPos());

		Transform()->SetLocalPos(vPos);

		if (CollisionSphere(m_vOffsetScale, _pOther, 0.2f))
		{
			if (m_fAttackTime < 0.f)
			{
				_pOther->GetObj()->GetScript<CPlayerScript>()->Damage(m_tStatus.fDamage);
				m_fAttackTime = m_fAttackCoolTime;
			}
			Transform()->SetLocalPos(vTempPos);
		}
		m_bIdleBehavior = false;
	}
	


}

void CAnimalScript::OnCollisionEnter(CCollider2D * _pOther)
{
	if (m_bAnimalDead)
		return;
	if (CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Player")->GetLayerIdx() == _pOther->GetObj()->GetLayerIdx())
	{
		if (BEHAVIOR_TYPE::B_PASSIVE != m_tStatus.eType)
		{
			// �÷��̾ ���� ���� �ȿ� ��������
			if (m_bBehavior)
				m_bBehavior = false; // �ϴ� �ൿ ����

			// �񼱰� ���� ������ �ƴ� ���� �þ߿� �÷��̾ ��������
			if (m_bIdleBehavior)
				m_bIdleBehavior = false;
		}
	}
}

void CAnimalScript::OnCollisionExit(CCollider2D * _pOther)
{
	if (m_bAnimalDead)
		return;
	// �÷��̾ �þ߹ݰ濡�� ����� ���� �ൿ�� �����ϵ���
	if (CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Player")->GetLayerIdx() == _pOther->GetObj()->GetLayerIdx())
	{
		// ȸ���� ���� ( �罿 )
		if (BEHAVIOR_TYPE::B_EVASION == m_tStatus.eType)
		{
			m_vMoveDir = _pOther->Transform()->GetLocalDir(DIR_TYPE::FRONT);
			Vec3 vOtherPos = _pOther->Transform()->GetLocalPos();
			Vec3 vPos = Transform()->GetLocalPos();

			m_vMoveDir = XMVector3Normalize(vPos - vOtherPos);
			m_vMoveDir.y = 0.f;

			m_bBehavior = true;
			m_fCurrentTime = m_tStatus.fBehaviorTime;
		}
		else if (BEHAVIOR_TYPE::B_PASSIVE == m_tStatus.eType)
		{
			// �񼱰� ���� ������ �þ߿� �÷��̾ ��������

		}
		else if (BEHAVIOR_TYPE::B_WARLIKE == m_tStatus.eType)
		{
			m_pTarget = _pOther->GetObj();

			m_vMoveDir = _pOther->Transform()->GetLocalDir(DIR_TYPE::FRONT);
			m_bBehavior = true;
			m_fCurrentTime = m_tStatus.fBehaviorTime;
		}
	}
}

bool CAnimalScript::CollisionSphere(Vec3 vOffsetScale, CCollider2D* _pOther, float fOffset)
{
	const Matrix& matCol1 = Collider2D()->GetColliderWorldMat();
	const Matrix& matCol2 = _pOther->GetColliderWorldMat();
		
	Vec3 vCol1 = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matCol1);
	Vec3 vCol2 = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matCol2);

	Vec3 vScale1 = Transform()->GetLocalScale();
	Vec3 vScale2 = _pOther->Transform()->GetLocalScale();

	Vec3 vColScale1 = vOffsetScale;
	Vec3 vColScale2 = _pOther->Collider2D()->GetOffsetScale() * fOffset;

	float fDist = pow(vCol2.x - vCol1.x, 2) + pow(vCol2.y - vCol1.y, 2) + pow(vCol2.z - vCol1.z, 2);
	fDist = sqrt(fDist);

	if (fDist > fabs(vScale1.x * vColScale1.x) + fabs(vScale2.x * vColScale2.x))
		return false;

	return true;
}

void CAnimalScript::SetAnimalStatus(tAnimalStatus tStatus)
{
	m_tStatus = tStatus;
}

tAnimalStatus CAnimalScript::GetAnimalStatus()
{
	return m_tStatus;
}

void CAnimalScript::SetOffsetScale(Vec3 vScale)
{
	m_vOffsetScale = vScale;
}

Vec3 CAnimalScript::GetOffsetScale()
{
	return m_vOffsetScale;
}

void CAnimalScript::SetBehaviorType(BEHAVIOR_TYPE eType)
{
	m_tStatus.eType = eType;
}

void CAnimalScript::SetAnimalType(ANIMAL_TYPE eKind)
{
	m_tStatus.eKind = eKind;
}

void CAnimalScript::SetAttackCoolTime(float fTime)
{
	m_fAttackCoolTime = fTime;
	m_fAttackTime = fTime;
}

void CAnimalScript::Damage(CGameObject* _pOther, float fDamage)
{
	if (m_pParticleObj == NULL)
	{
		// ====================
		// Particle Object ����
		// ====================
		CGameObject* pObject = new CGameObject;
		pObject->SetName(L"Animal Particle");
		pObject->AddComponent(new CTransform);
		pObject->AddComponent(new CParticleSystem);

		pObject->ParticleSystem()->SetStartColor(Vec4(1.f, 0.f, 0.f, 1.f));
		pObject->ParticleSystem()->SetEndColor(Vec4(0.5f, 0.f, 0.f, 0.6f));

		if (ANIMAL_TYPE::A_BOAR == m_tStatus.eKind)
			pObject->Transform()->SetLocalPos(Vec3(0.f, 0.f, 40.f));
		else if(ANIMAL_TYPE::A_BEAR == m_tStatus.eKind)
			pObject->Transform()->SetLocalPos(Vec3(0.f, 0.f, 4.f));
		else if(ANIMAL_TYPE::A_DEER == m_tStatus.eKind)
			pObject->Transform()->SetLocalPos(Vec3(0.f, 30.f, 0.f));
		else
			pObject->Transform()->SetLocalPos(Vec3(0.f, 2.5f, 0.f));

		GetObj()->AddChild(pObject);

		m_pParticleObj = pObject;

		tEvent tEv;
		tEv.eType = EVENT_TYPE::CREATE_OBJECT;
		tEv.wParam = (DWORD_PTR)pObject;
		tEv.lParam = 0;
		CEventMgr::GetInst()->AddEvent(tEv);
	}
	m_fParticleTime = 1.f;
	if (m_tStatus.fHp > 0.f) 
	{
		m_tStatus.fHp -= fDamage;
		if (BEHAVIOR_TYPE::B_PASSIVE == m_tStatus.eType)
		{
			m_bBehavior = true;
			m_pTarget = _pOther;
			m_fCurrentTime = m_tStatus.fBehaviorTime;
		}
	}
	else
	{
		m_bAnimalDead = true;
		m_fLivingTime = 3.f;
	}
}

bool CAnimalScript::GetAnimalDead()
{
	return m_bAnimalDead;
}
