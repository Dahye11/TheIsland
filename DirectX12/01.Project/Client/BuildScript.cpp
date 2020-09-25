#include "stdafx.h"
#include "BuildScript.h"
#include "HousingMgr.h"
#include "NaturalScript.h"
#include "Network.h"
#include "PacketMgr.h"

CBuildScript::CBuildScript(HOUSING_TYPE eType, UINT iGrade) :
	CScript((UINT)SCRIPT_TYPE::WORLDSCRIPT),
	m_bBuild(false),
	m_eType(eType),
	m_iGrade(iGrade), 
	m_iIndex(0)
{
	switch (eType)
	{
	case HOUSING_FOUNDATION:
		m_bCollision = false;
		break;
	case HOUSING_WALL:
	case HOUSING_DOOR:
	case HOUSING_WINDOW:
	case HOUSING_FLOOR:
	{
		m_bCollision = true;
	}
		break;
	default:
		m_bCollision = false;
		break;
	}
}


CBuildScript::~CBuildScript()
{
}

void CBuildScript::Update()
{
}

void CBuildScript::OnCollision(CCollider2D * _pOther)
{
	if (_pOther->GetObj()->GetLayerIdx() != CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"House")->GetLayerIdx())
		return;

	// ÀÜµð´Â Ãæµ¹ x
	if (CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Environment")->GetLayerIdx() == _pOther->GetObj()->GetLayerIdx())
	{
		if (_pOther->GetObj()->GetScript<CNaturalScript>()->GetNaturalType() == NATURAL_BUSH)
		{
			return;
		}
	}

	HOUSING_TYPE eType = _pOther->GetObj()->GetScript<CBuildScript>()->GetHousingType();
	switch (m_eType)
	{
	case HOUSING_FOUNDATION:
	{
		if (eType == HOUSING_FOUNDATION)
		{
			Vec3 vPos = Transform()->GetLocalPos();
			Vec3 vOtherPos = _pOther->Transform()->GetLocalPos();
			Vec3 vOtherScale = _pOther->Transform()->GetLocalScale();

			Vec3 vDiff = vPos - vOtherPos;
			vDiff.Normalize(vDiff);
			// vOtherPos -> vPos

			Vec3 vOffset = Vec3(0.f, 0.f, 0.f);
			float fLean = vDiff.z / vDiff.x;

			if (fLean < 1.f && fLean > -1.f)
			{
				if (vDiff.x > 0.f)
					vOffset = Vec3(1.f, 0.f, 0.f);
				else
					vOffset = Vec3(-1.f, 0.f, 0.f);
			}
			else
			{
				if (vDiff.z > 0.f)
					vOffset = Vec3(0.f, 0.f, 1.f);
				else
					vOffset = Vec3(0.f, 0.f, -1.f);
			}

			vPos = vOtherPos + (vOffset * 400.f * vOtherScale);
			Transform()->SetLocalPos(vPos);
		}
	}
	break;
	case HOUSING_WALL:
	case HOUSING_DOOR:
	case HOUSING_WINDOW:
		if (eType == HOUSING_FOUNDATION)
		{
			Vec3 vPos = Transform()->GetLocalPos();
			Vec3 vOtherPos = _pOther->Transform()->GetLocalPos();
			Vec3 vOtherScale = _pOther->Transform()->GetLocalScale();

			Vec3 vDiff = vPos - vOtherPos;
			vDiff.Normalize(vDiff);
			// vOtherPos -> vPos

			Vec3 vOffset = Vec3(0.f, 0.f, 0.f);
			float fLean = vDiff.z / vDiff.x;

			if (fLean < 1.f && fLean > -1.f)
			{
				if (vDiff.x > 0.f)
				{
					vOffset = Vec3(1.f, 0.f, 0.f);
					Transform()->SetLocalRot(Vec3(-XM_PI / 2.f, 0.f, 0.f));
				}
				else
				{
					vOffset = Vec3(-1.f, 0.f, 0.f);
					Transform()->SetLocalRot(Vec3(-XM_PI / 2.f, 0.f, 0.f));
				}
			}
			else
			{
				if (vDiff.z > 0.f)
				{
					vOffset = Vec3(0.f, 0.f, 1.f);
					Transform()->SetLocalRot(Vec3(-XM_PI / 2.f, -XM_PI / 2.f, 0.f));
				}
				else
				{
					vOffset = Vec3(0.f, 0.f, -1.f);
					Transform()->SetLocalRot(Vec3(-XM_PI / 2.f, -XM_PI / 2.f, 0.f));
				}
			}

			vPos = vOtherPos + (vOffset * 190.f * vOtherScale);
			Transform()->SetLocalPos(vPos);

			m_bCollision = false;
			int test = -1;
			for (int i = 0; i < MeshRender()->GetMaterialCount(); ++i)
			{
				MeshRender()->GetSharedMaterial(i)->SetData(SHADER_PARAM::INT_3, &test);
			}
		}
		else if (eType > HOUSING_FOUNDATION && eType < HOUSING_END)
		{

		}
		else
		{
			m_bCollision = true;
			int test = -2;
			for (int i = 0; i < MeshRender()->GetMaterialCount(); ++i)
			{
				MeshRender()->GetSharedMaterial(i)->SetData(SHADER_PARAM::INT_3, &test);
			}
		}
		break;
	case HOUSING_FLOOR:
		if (eType > HOUSING_FOUNDATION && eType < HOUSING_FLOOR)
		{
			Vec3 vPos = Transform()->GetLocalPos();
			Vec3 vOtherPos = _pOther->Transform()->GetLocalPos();
			Vec3 vOtherScale = _pOther->Transform()->GetLocalScale();

			Vec3 vDiff = vPos - vOtherPos;
			vDiff.Normalize(vDiff);
			// vOtherPos -> vPos

			Vec3 vOffset = Vec3(0.f, 0.f, 0.f);
			Vec3 vRot = _pOther->Transform()->GetLocalRot();

			if (vRot.y != 0.f)
			{
				if (vDiff.z > 0.f)
				{
					vOffset = Vec3(0.f, 0.f, 1.f);
				}
				else
				{
					vOffset = Vec3(0.f, 0.f, -1.f);
				}
			}
			else
			{
				if (vDiff.x > 0.f)
				{
					vOffset = Vec3(1.f, 0.f, 0.f);
				}
				else
				{
					vOffset = Vec3(-1.f, 0.f, 0.f);
				}
			}

			vPos = vOtherPos + (vOffset * 200.f * vOtherScale);
			vPos.y += 240.f;
			Transform()->SetLocalPos(vPos);

			m_bCollision = false;
			int test = -1;
			for (int i = 0; i < MeshRender()->GetMaterialCount(); ++i)
			{
				MeshRender()->GetSharedMaterial(i)->SetData(SHADER_PARAM::INT_3, &test);
			}
		}
		else if (eType == HOUSING_FOUNDATION)
		{
			Vec3 vPos = Transform()->GetLocalPos();
			Vec3 vOtherPos = _pOther->Transform()->GetLocalPos();

			vPos = vOtherPos;
			vPos.y += 240.f;
			Transform()->SetLocalPos(vPos);

			m_bCollision = false;
			int test = -1;
			for (int i = 0; i < MeshRender()->GetMaterialCount(); ++i)
			{
				MeshRender()->GetSharedMaterial(i)->SetData(SHADER_PARAM::INT_3, &test);
			}
		}
		else
		{
			/*m_bCollision = true;
			int test = -2;
			for (int i = 0; i < MeshRender()->GetMaterialCount(); ++i)
			{
				MeshRender()->GetSharedMaterial(i)->SetData(SHADER_PARAM::INT_3, &test);
			}*/
		}
		break;
	case HOUSING_ETC:
		if (eType == HOUSING_FOUNDATION)
		{
			Vec3 vPos = Transform()->GetLocalPos();
			Vec3 vOtherPos = _pOther->Transform()->GetLocalPos();

			vPos.y = vOtherPos.y;

			Transform()->SetLocalPos(vPos);
		}
		break;
	default:
		break;
	}
}

void CBuildScript::OnCollisionEnter(CCollider2D * _pOther)
{
	// ÀÜµð´Â Ãæµ¹ x
	if (CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Environment")->GetLayerIdx() == _pOther->GetObj()->GetLayerIdx())
	{
		if (_pOther->GetObj()->GetScript<CNaturalScript>()->GetNaturalType() == NATURAL_BUSH)
		{
			return;
		}
	}

	switch (m_eType)
	{
	case HOUSING_FOUNDATION:
	{
		m_bCollision = true;
		int test = -2;
		for (int i = 0; i < MeshRender()->GetMaterialCount(); ++i)
		{
			MeshRender()->GetSharedMaterial(i)->SetData(SHADER_PARAM::INT_3, &test);
		}
	}
		break;
	case HOUSING_WALL:
	case HOUSING_DOOR:
	case HOUSING_WINDOW:
	{
		if (_pOther->GetObj()->GetLayerIdx() == CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"House")->GetLayerIdx())
		{
			return;
		}
	}
		break;
	case HOUSING_FLOOR:
	{
		if (_pOther->GetObj()->GetLayerIdx() == CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"House")->GetLayerIdx())
		{
			return;
		}
	}
		break;
	default:
		break;
	}
}

void CBuildScript::OnCollisionExit(CCollider2D * _pOther)
{
	// ÀÜµð´Â Ãæµ¹ x
	if (CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Environment")->GetLayerIdx() == _pOther->GetObj()->GetLayerIdx())
	{
		if (_pOther->GetObj()->GetScript<CNaturalScript>()->GetNaturalType() == NATURAL_BUSH)
		{
			return;
		}
	}

	switch (m_eType)
	{
	case HOUSING_FOUNDATION:
	{
		m_bCollision = false;
		int test = -1;
		for (int i = 0; i < MeshRender()->GetMaterialCount(); ++i)
		{
			MeshRender()->GetSharedMaterial(i)->SetData(SHADER_PARAM::INT_3, &test);
		}
	}
		break;
	case HOUSING_WALL:
	case HOUSING_DOOR:
	case HOUSING_WINDOW:
	{
		if (_pOther->GetObj()->GetLayerIdx() == CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"House")->GetLayerIdx())
		{
			if (_pOther->GetObj()->GetScript<CBuildScript>()->GetHousingType() == HOUSING_FOUNDATION)
			{
				m_bCollision = true;
				int test = -2;
				for (int i = 0; i < MeshRender()->GetMaterialCount(); ++i)
				{
					MeshRender()->GetSharedMaterial(i)->SetData(SHADER_PARAM::INT_3, &test);
				}
			}
			return;
		}
	}
	break;
	case HOUSING_FLOOR:
	{
		if (_pOther->GetObj()->GetLayerIdx() == CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"House")->GetLayerIdx())
		{
			if (_pOther->GetObj()->GetScript<CBuildScript>()->GetHousingType() > HOUSING_FOUNDATION &&
				_pOther->GetObj()->GetScript<CBuildScript>()->GetHousingType() < HOUSING_END)
			{
				m_bCollision = true;
				int test = -2;
				for (int i = 0; i < MeshRender()->GetMaterialCount(); ++i)
				{
					MeshRender()->GetSharedMaterial(i)->SetData(SHADER_PARAM::INT_3, &test);
				}
			}
			return;
		}
	}
		break;
	default:
		break;
	}
}

void CBuildScript::Init()
{
	if (m_bCollision)
	{
		int test = -2;
		for (int i = 0; i < MeshRender()->GetMaterialCount(); ++i)
		{
			MeshRender()->GetSharedMaterial(i)->SetData(SHADER_PARAM::INT_3, &test);
		}
	}
	else
	{
		int test = -1;
		for (int i = 0; i < MeshRender()->GetMaterialCount(); ++i)
		{
			MeshRender()->GetSharedMaterial(i)->SetData(SHADER_PARAM::INT_3, &test);
		}
	}
}

bool CBuildScript::Build(bool bSendPacket)
{
	if (m_bCollision)
		return false;

	// Install_Housing packet Send
	Vec3 vPos = Transform()->GetLocalPos();
	Vec3 vRot = Transform()->GetLocalRot();
	Vec3 vScale = Transform()->GetLocalScale();
#ifdef NETWORK_ON	
	if (bSendPacket)
		CPacketMgr::GetInst()->Send_Install_Housing_Packet(m_eType, vPos, vRot, vScale, Vec3(0.f, 0.f, 120.f), Vec3(195.f, 195.f, 195.f));
	return false;
#else
	for ( int i = 0; i < MeshRender()->GetMaterialCount(); ++i )
	{
		MeshRender()->GetCloneMaterial( i )->SetShader( CResMgr::GetInst()->FindRes<CShader>( L"Std3DShader" ) );
		//SetData(SHADER_PARAM::INT_3, &test);
	}

	if ( m_eType == HOUSING_FOUNDATION )
	{
		CGameObject* pFloor = CHousingMgr::GetInst()->GetHousingMeshData( HOUSING_FLOOR )->Instantiate();
		pFloor->SetName( L"Foundation_Floor" );
		pFloor->FrustumCheck( false );
		pFloor->Transform()->SetLocalPos( Vec3( 0.f, 0.f, -14.f ) );
		pFloor->Transform()->SetLocalScale( Vec3( 1.f, 1.f, 1.f ) );
		for ( int i = 0; i < MeshRender()->GetMaterialCount(); ++i )
		{
			pFloor->MeshRender()->GetCloneMaterial( i )->SetShader( CResMgr::GetInst()->FindRes<CShader>( L"Std3DShader" ) );
			//SetData(SHADER_PARAM::INT_3, &test);
		}
		GetObj()->AddChild( pFloor );
	}
	return true;
#endif

}

void CBuildScript::MustBuild()
{
	for (int i = 0; i < MeshRender()->GetMaterialCount(); ++i)
	{
		MeshRender()->GetCloneMaterial(i)->SetShader(CResMgr::GetInst()->FindRes<CShader>(L"Std3DShader"));
		//SetData(SHADER_PARAM::INT_3, &test);
	}

	if (m_eType == HOUSING_FOUNDATION)
	{
		CGameObject* pFloor = CHousingMgr::GetInst()->GetHousingMeshData(HOUSING_FLOOR)->Instantiate();
		pFloor->SetName(L"Foundation_Floor");
		pFloor->FrustumCheck(false);
		pFloor->Transform()->SetLocalPos(Vec3(0.f, 0.f, -14.f));
		pFloor->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
		for (int i = 0; i < MeshRender()->GetMaterialCount(); ++i)
		{
			pFloor->MeshRender()->GetCloneMaterial(i)->SetShader(CResMgr::GetInst()->FindRes<CShader>(L"Std3DShader"));
			//SetData(SHADER_PARAM::INT_3, &test);
		}
		GetObj()->AddChild(pFloor);
	}
}

bool CBuildScript::Upgrade()
{
	if(m_iGrade >= MAX_GRADE - 1)
		return false;

	m_iGrade++;

	CGameObject* pObject = NULL;
	pObject = CHousingMgr::GetInst()->GetHousingMeshData((HOUSING_TYPE)m_eType, m_iGrade)->Instantiate();
	pObject->AddComponent(new CBuildScript((HOUSING_TYPE)m_eType, m_iGrade));

#ifdef CHECK_COLLISION
	pObject->AddComponent( new CCollider2D );

	pObject->Collider2D()->SetOffsetScale( Vec3( 195.f, 195.f, 195.f ) );

	if ( m_eType >= HOUSING_WALL && m_eType < HOUSING_FLOOR )
		pObject->Collider2D()->SetOffsetPos( Vec3( 0.f, 0.f, 120.f ) );

	pObject->Collider2D()->SetCollider2DType( COLLIDER2D_TYPE::SPHERE );

#endif
	pObject->SetName(L"House");

	Vec3 vPos = Transform()->GetLocalPos();
	Vec3 vRot = Transform()->GetLocalRot();
	Vec3 vScale = Transform()->GetLocalScale();
	pObject->Transform()->SetLocalPos(vPos);
	pObject->Transform()->SetLocalRot(vRot);
	pObject->Transform()->SetLocalScale(vScale);

	for (int i = 0; i < MeshRender()->GetMaterialCount(); ++i)
	{
		pObject->MeshRender()->GetCloneMaterial(i)->SetShader(CResMgr::GetInst()->FindRes<CShader>(L"Std3DShader"));
		//SetData(SHADER_PARAM::INT_3, &test);
	}

	if (m_eType == HOUSING_FOUNDATION)
	{
		CGameObject* pFloor = CHousingMgr::GetInst()->GetHousingMeshData(HOUSING_FLOOR, m_iGrade)->Instantiate();
		pFloor->SetName(L"Foundation_Floor");
		pFloor->FrustumCheck(false);
		pFloor->Transform()->SetLocalPos(Vec3(0.f, 0.f, -14.f));
		pFloor->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
		for (int i = 0; i < MeshRender()->GetMaterialCount(); ++i)
		{
			pFloor->MeshRender()->GetCloneMaterial(i)->SetShader(CResMgr::GetInst()->FindRes<CShader>(L"Std3DShader"));
			//SetData(SHADER_PARAM::INT_3, &test);
		}
		pObject->AddChild(pFloor);
	}
	CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"House")->AddGameObject(pObject);

	tEvent tEv;
	tEv.eType = EVENT_TYPE::DELETE_OBJECT;
	tEv.wParam = (DWORD_PTR)GetObj();
	CEventMgr::GetInst()->AddEvent(tEv);

	return true;
}

HOUSING_TYPE CBuildScript::GetHousingType()
{
	return m_eType;
}

Vec3 CBuildScript::GetOffsetScale()
{
	Vec3 vScale;
	switch (m_eType)
	{
	case HOUSING_FOUNDATION:
		vScale = Vec3(200.f, 200.f, 200.f);
		break;
	case HOUSING_WALL:
	case HOUSING_DOOR:
	case HOUSING_WINDOW:
		vScale = Vec3(20.f, 200.f, 200.f);
		break;
	case HOUSING_FLOOR:
		vScale = Vec3(200.f, 20.f, 200.f);
		break;
	default:
		break;
	}

	return vScale;
}
