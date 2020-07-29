#include "stdafx.h"
#include "MonsterScript.h"

CMonsterScript::CMonsterScript()
	: CScript((UINT)SCRIPT_TYPE::ANIMALSCRIPT)
	, m_iDir(1)
	, m_bUD(true)
{
}

CMonsterScript::~CMonsterScript()
{
}


void CMonsterScript::Update()
{
	// Transform ���� ��ǥ���� ���
	Vec3 vPos = Transform()->GetLocalPos();

	if (m_bUD)
	{
		if (vPos.x > 600.f)
			m_iDir = -1;
		else if (vPos.x < -600.f)
			m_iDir = 1;

		vPos.x += DT * 100.f * m_iDir;
	}
	else
	{
		if (vPos.y > 600.f)
			m_iDir = -1;
		else if (vPos.y < -600.f)
			m_iDir = 1;

		vPos.y += DT * 100.f * m_iDir;
	}

	// ������ ��ǥ�� �ٽ� �����ϱ�.
	Transform()->SetLocalPos(vPos);
}

void CMonsterScript::OnCollisionEnter(CCollider2D * _pOther)
{
	// �浹�� �߻��ϰ�, ��� ��ü�� �����̸� �����θ� ����
	if (L"Bullet Object" == _pOther->GetObj()->GetName())
	{		
		DeleteObject(GetObj());	// -->���� �̺�Ʈ ���	
	}	
}

void CMonsterScript::OnCollisionExit(CCollider2D * _pOther)
{	
}

void CMonsterScript::SetUpDown(bool bUD)
{
	m_bUD = bUD;
}
