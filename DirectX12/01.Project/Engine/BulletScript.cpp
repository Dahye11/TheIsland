#include "stdafx.h"
#include "BulletScript.h"


CBulletScript::CBulletScript()	
	: CScript((UINT)SCRIPT_TYPE::PLAYERSCRIPT)
{
}

CBulletScript::~CBulletScript()
{
}

void CBulletScript::Update()
{
	Vec3 vPos = Transform()->GetLocalPos();
	vPos.y += 400.f * DT;
	Transform()->SetLocalPos(vPos);
}

void CBulletScript::OnCollisionEnter(CCollider2D * _pOther)
{
	if (L"Monster Object" == _pOther->GetObj()->GetName())
	{
		DeleteObject(GetObj());
	}
}
