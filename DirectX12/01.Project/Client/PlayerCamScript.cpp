#include "stdafx.h"
#include "PlayerCamScript.h"


CPlayerCamScript::CPlayerCamScript() :
	CScript((UINT)SCRIPT_TYPE::UISCRIPT),
	m_pPlayer(nullptr)
{
}

CPlayerCamScript::~CPlayerCamScript()
{
}

void CPlayerCamScript::Update()
{
	Vec3 vDir = m_pPlayer->Transform()->GetWorldDir(DIR_TYPE::FRONT);

	Vec3 vPos = m_pPlayer->Transform()->GetLocalPos();
	Vec3 vRot = m_pPlayer->Transform()->GetLocalRot();

	vPos += vDir * 120.f;
	vPos.y = 50.f;

	vRot.y = vRot.y + XM_PI;

	Transform()->SetLocalPos(vPos);
	Transform()->SetLocalRot(vRot);
}
