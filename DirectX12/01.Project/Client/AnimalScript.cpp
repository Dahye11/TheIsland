#include "stdafx.h"
#include "AnimalScript.h"
#include "PlayerScript.h"

#include <iostream>

CAnimalScript::CAnimalScript()
	: CScript((UINT)SCRIPT_TYPE::MONSTERSCRIPT)
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
	, m_fCurrentTime(0.f)
	, m_vMoveDir(Vec3(0.f, 0.f, 0.f))
	, m_pTarget(NULL)
	, m_fAttackCoolTime(3.f)
	, m_fAttackTime(0.f)
{

}

CAnimalScript::~CAnimalScript()
{
}

void CAnimalScript::Update()
{
	if (m_fAttackTime > -1.f)
	{
		m_fAttackTime -= DT;
	}

	if (!m_bBehavior)
	{
		return;
	}

	m_fCurrentTime -= DT;
	if (m_fCurrentTime < 0.f)
	{
		m_bBehavior = false;
		m_fCurrentTime = 0.f;
	}

	// �÷��̾ �þ߹����� ��� ��� � �ൿ�� ��� ���� ���ΰ�
	// ȸ���� ���� ( �罿 )
	if (BEHAVIOR_TYPE::B_EVASION == m_tStatus.eType)
	{
		// ������ �������� ������ �޸�
		Vec3 vPos = Transform()->GetLocalPos();

		vPos += m_vMoveDir * m_tStatus.fSpeed * DT;

		Transform()->SetLocalPos(vPos);
	}
	else if (BEHAVIOR_TYPE::B_PASSIVE == m_tStatus.eType)
	{
		// �񼱰� ����

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
		Transform()->SetLocalPos(vPos);
	}
}

void CAnimalScript::OnCollision(CCollider2D * _pOther)
{
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
			Transform()->SetLocalPos(vPos);
		}

		// Ÿ�Կ� ���� �߰� �ൿ ��� ������ ��

		return;
	}


	// �÷��̾ ���� ���� �ȿ� ��������
	if (m_bBehavior)
		m_bBehavior = false; // �ϴ� �ൿ ����

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
		Transform()->SetLocalPos(vPos);
	}
	else if (BEHAVIOR_TYPE::B_PASSIVE == m_tStatus.eType)
	{
		// �񼱰� ���� ������ �þ߿� �÷��̾ ��������

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
		Transform()->SetLocalPos(vPos);

		if (CollisionSphere(m_vOffsetScale, _pOther))
		{
			if (m_fAttackTime < 0.f)
			{
				_pOther->GetObj()->GetScript<CPlayerScript>()->Damage(m_tStatus.fDamage);
				m_fAttackTime = m_fAttackCoolTime;
			}
			Transform()->SetLocalPos(vTempPos);
		}
	}
	


}

void CAnimalScript::OnCollisionEnter(CCollider2D * _pOther)
{

}

void CAnimalScript::OnCollisionExit(CCollider2D * _pOther)
{
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

bool CAnimalScript::CollisionSphere(Vec3 vOffsetScale, CCollider2D* _pOther)
{
	const Matrix& matCol1 = Collider2D()->GetColliderWorldMat();
	const Matrix& matCol2 = _pOther->GetColliderWorldMat();
		
	Vec3 vCol1 = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matCol1);
	Vec3 vCol2 = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matCol2);

	Vec3 vScale1 = Transform()->GetLocalScale();
	Vec3 vScale2 = _pOther->Transform()->GetLocalScale();

	Vec3 vColScale1 = vOffsetScale;
	Vec3 vColScale2 = _pOther->Collider2D()->GetOffsetScale();

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
