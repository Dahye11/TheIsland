#include "stdafx.h"
#include "StatusScript.h"
#include "ArmorScript.h"
#include "GameOverScript.h"

CStatusScript::CStatusScript() :
	CScript((UINT)SCRIPT_TYPE::UISCRIPT),
	m_fHealth(100.f), m_fHungry(100.f), m_fThirst(100.f),
	m_bGameOver(false),
	m_pArmor(NULL),
	m_fArmor(0.f),
	m_pScreenDamage(NULL)
{
}


CStatusScript::~CStatusScript()
{
}

void CStatusScript::Update()
{
	if (KEY_HOLD(KEY_TYPE::KEY_L))
	{
		Damage(20.f * DT);
	}
#ifdef NETWORK_ON
#else
	m_fHungry -= 0.2f * DT;
	if (m_fHungry < 0.f)
	{
		m_fHungry = 0.f;
		Damage(0.5f * DT);
	}
	m_fThirst -= 0.35f * DT;
	if (m_fThirst < 0.f)
	{
		m_fThirst = 0.f;
		Damage(0.25f * DT);
	}
#endif // NETWORK_ON

	const vector<CGameObject *>& vecObj = GetObj()->GetChild();

	vecObj[0]->Transform()->SetLocalPos(Vec3(-0.46f + 0.46f * m_fHealth / 100.f, 0.3f, -150.f));
	vecObj[0]->Transform()->SetLocalScale(Vec3(230.f / 250.f * m_fHealth / 100.f, 30.f / 135.f, 1.f));

	vecObj[1]->Transform()->SetLocalPos(Vec3(-0.46f + 0.46f * m_fHungry / 100.f, 0.f, -150.f));
	vecObj[1]->Transform()->SetLocalScale(Vec3(230.f / 250.f * m_fHungry / 100.f, 30.f / 135.f, 1.f));

	vecObj[2]->Transform()->SetLocalPos(Vec3(-0.46f + 0.46f * m_fThirst / 100.f, -0.3f, -150.f));
	vecObj[2]->Transform()->SetLocalScale(Vec3(230.f / 250.f * m_fThirst / 100.f, 30.f / 135.f, 1.f));

	vecObj[3]->Transform()->SetLocalPos(Vec3(-0.46f + 0.46f * m_fArmor / 100.f, 0.3f, -160.f));
	vecObj[3]->Transform()->SetLocalScale(Vec3(230.f / 250.f * m_fArmor / 100.f, 30.f / 135.f, 1.f));

	float health = 1.f - (m_fHealth / 100.f);
	m_pScreenDamage->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::FLOAT_0, &health);
}

bool CStatusScript::Damage(float fDamage, bool bTrueDamage)
{
	if (m_bInvincible)
	{
		return false;
	}

	if (m_pArmor && !bTrueDamage)
	{
		m_fArmor -= fDamage;
		m_pArmor->GetScript<CArmorScript>()->SetArmorValue(m_fArmor);
		if (m_fArmor < 0.f)
		{
			m_fArmor = 0.f;
			m_pArmor->GetScript<CArmorScript>()->DestroyArmor();
			m_pArmor = NULL;
		}
		return false;
	}
	m_fHealth -= fDamage;
	if (m_fHealth <= 0.f && m_bGameOver == false)
	{
		CGameObject* pObject = new CGameObject;
		pObject->AddComponent(new CTransform);
		pObject->AddComponent(new CGameOverScript);
		pObject->SetName(L"GameOver");
		pObject->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
		pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
		pObject->GetScript<CGameOverScript>()->Init();
		CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"UI")->AddGameObject(pObject);
		m_bGameOver = true;
		ShowCursor(true);
		return true;
	}
	return false;
}

void CStatusScript::SetIncreaseHealth(float fAmount)
{
	m_fHealth += fAmount;
	if (m_fHealth > 100.f)
		m_fHealth = 100.f;
}

void CStatusScript::SetIncreaseHungry(float fAmount)
{
	m_fHungry += fAmount;
	if (m_fHungry > 100.f)
		m_fHungry = 100.f;
}

void CStatusScript::SetIncreasefThirst(float fAmount)
{
	m_fThirst += fAmount;
	if (m_fThirst > 100.f)
		m_fThirst = 100.f;
}

void CStatusScript::SetHealth(float fHealth)
{
	m_fHealth = fHealth;
}

void CStatusScript::SetHungry(float fHungry)
{
	m_fHungry = fHungry;
}

void CStatusScript::SetThirst(float fThirst)
{
	m_fThirst = fThirst;
}

void CStatusScript::SetScreenDamage(CGameObject * pObject)
{
	m_pScreenDamage = pObject;
}

bool CStatusScript::GetGameOver()
{
	return m_bGameOver;
}

void CStatusScript::EquipArmor(CGameObject* pArmor, float fArmor)
{
	m_pArmor = pArmor;
	m_fArmor = fArmor;
}

void CStatusScript::Invincible(bool bOn)
{
	m_bInvincible = bOn;
	if(m_bInvincible)
		m_fHealth = 100.f;
}
