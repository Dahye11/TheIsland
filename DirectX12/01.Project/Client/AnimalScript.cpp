#include "stdafx.h"
#include "AnimalScript.h"
#include "PlayerScript.h"

#include "NaturalScript.h"
#include "AnimalSpawner.h"

#include "BuildScript.h"
#include "ArrowScript.h"

#include <Engine/ParticleSystem.h>
#include <Engine/NaviMgr.h>

#include <Engine/Animator3D.h>

#include "Network.h"
#include "PacketMgr.h"

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
	, m_pSpawner(NULL)
	, m_bAttack(false)
	, m_iIndex(0)
{

}

CAnimalScript::~CAnimalScript()
{
}

void CAnimalScript::Update()
{
	// Particle Update
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
#ifdef NETWORK_ON
	return;
#endif // NETWORK_ON
	if (m_bAnimalDead)
	{
		// 사망
		m_fLivingTime -= DT;
		if (m_fLivingTime < 0.f)
		{
			tEvent tEv;
			tEv.eType = EVENT_TYPE::DELETE_OBJECT;
			tEv.wParam = (DWORD_PTR)GetObj();
			CEventMgr::GetInst()->AddEvent(tEv);

			// 스포너에게 정보 알림
			if (m_pSpawner)
			{
				m_pSpawner->DeadAnimal();
			}
		}
		return;
	}

	if (m_bAttack)
	{
		m_fAnimationCoolTime -= DT;
		if (m_fAnimationCoolTime < 0.f)
		{
			m_bAttack = false;
		}
	}

	if (m_bIdleBehavior)
	{
		// 주위에 아무도 없을때 일정 시간이 흐른 뒤 하는 행동
		m_fIdleBehaviorTime -= DT;
		if (m_fIdleBehaviorTime < 0.f)
		{
			m_bIdleBehavior = false;
			// 다음 행동은 몇초 뒤에 할 것인지
			m_fIdleBehaviorTime = (rand () / (float)RAND_MAX * 5.f) + 3.f;
		}
		Vec3 vPos = Transform()->GetLocalPos();
		m_vPrevPos = vPos;

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

		if (vPos.y <= 80.f)
		{
			vPos += -m_vMoveDir * m_tStatus.fSpeed * DT;
			vPos.y = CNaviMgr::GetInst()->GetY(Transform()->GetWorldPos());
		}

		Transform()->SetLocalPos(vPos);
		Animator3D()->ChangeAnimation(L"Walk");
		return;
	}

	if (m_fAttackTime > -1.f)
	{
		m_fAttackTime -= DT;
	}

	if (!m_bBehavior)
	{
		// 시야에 적이 아무도 없다면? 대기시간 감소
		m_fIdleBehaviorTime -= DT;
		if (m_fIdleBehaviorTime < 0.f)
		{
			// 랜덤한 방향을 잡고 몇초동안 움직일건지
			m_bIdleBehavior = true;
			m_fIdleBehaviorTime = (rand() / (float)RAND_MAX * 5.f) + 3.f;;
			m_vMoveDir = Vec3(rand() / (float)RAND_MAX, 0.f, rand() / (float)RAND_MAX);
			m_vMoveDir = XMVector3Normalize(m_vMoveDir);
		}
		if (!m_bIdleBehavior)
			Animator3D()->ChangeAnimation(L"Idle");
		return;
	}

	// 추격 종료
	m_fCurrentTime -= DT;
	if (m_fCurrentTime < 0.f)
	{
		m_bBehavior = false;
		m_fCurrentTime = 0.f;
		m_fIdleBehaviorTime = (rand() / (float)RAND_MAX * 5.f) + 3.f;
		Animator3D()->ChangeAnimation(L"Idle");
	}

	// 플레이어가 시야범위를 벗어날 경우 어떤 행동을 계속 취할 것인가
	// 회피형 동물 ( 사슴 )
	if (BEHAVIOR_TYPE::B_EVASION == m_tStatus.eType)
	{
		// 마지막 방향으로 꾸준히 달림
		Vec3 vPos = Transform()->GetLocalPos();
		m_vPrevPos = vPos;

		vPos += m_vMoveDir * m_tStatus.fSpeed * DT;

		vPos.y = CNaviMgr::GetInst()->GetY(Transform()->GetWorldPos());
		if (vPos.y <= 80.f)
		{
			vPos += -m_vMoveDir * m_tStatus.fSpeed * DT;
			vPos.y = CNaviMgr::GetInst()->GetY(Transform()->GetWorldPos());
		}
		Animator3D()->ChangeAnimation(L"Run");
		Transform()->SetLocalPos(vPos);
	}
	else if (BEHAVIOR_TYPE::B_PASSIVE == m_tStatus.eType)
	{
		// 비선공 유형
		Vec3 vOtherPos = m_pTarget->Transform()->GetLocalPos();

		Vec3 vPos = Transform()->GetLocalPos();
		Vec3 vTempPos = vPos;

		Vec3 vDir = XMVector3Normalize(vOtherPos - vPos);
		vDir.y = 0.f;

		m_vPrevPos = vPos;

		vPos += vDir * m_tStatus.fSpeed * DT;

		Vec3 vRot = m_pTarget->Transform()->GetLocalRot();

		if(ANIMAL_TYPE::A_BOAR == m_tStatus.eKind)
			Transform()->SetLocalRot(Vec3(-XM_PI / 2.f, atan2(vDir.x, vDir.z) + 3.141592f, 0.f));
		else
			Transform()->SetLocalRot(Vec3(0.f, atan2(vDir.x, vDir.z), 0.f));

		vPos.y = CNaviMgr::GetInst()->GetY(Transform()->GetWorldPos());
		if (vPos.y <= 80.f)
		{
			vPos += -m_vMoveDir * m_tStatus.fSpeed * DT;
			vPos.y = CNaviMgr::GetInst()->GetY(Transform()->GetWorldPos());
		}
		Transform()->SetLocalPos(vPos);

		if (!m_bAttack)
			Animator3D()->ChangeAnimation(L"Run");
	}
	else if (BEHAVIOR_TYPE::B_WARLIKE == m_tStatus.eType)
	{
		// 선공몹
		// 쫒아가서 팬다
		Vec3 vOtherPos = m_pTarget->Transform()->GetLocalPos();

		Vec3 vPos = Transform()->GetLocalPos();
		Vec3 vTempPos = vPos;

		Vec3 vDir = XMVector3Normalize(vOtherPos - vPos);
		vDir.y = 0.f;

		m_vPrevPos = vPos;

		vPos += vDir * m_tStatus.fSpeed * DT;

		Vec3 vRot = m_pTarget->Transform()->GetLocalRot();

		Transform()->SetLocalRot(Vec3(-XM_PI / 2.f, atan2(vDir.x, vDir.z) + 3.141592f, 0.f));

		vPos.y = CNaviMgr::GetInst()->GetY(Transform()->GetWorldPos());
		if (vPos.y <= 80.f)
		{
			vPos += -m_vMoveDir * m_tStatus.fSpeed * DT;
			vPos.y = CNaviMgr::GetInst()->GetY(Transform()->GetWorldPos());
		}
		Transform()->SetLocalPos(vPos);

		if(!m_bAttack)
			Animator3D()->ChangeAnimation(L"Run");
	}
}

void CAnimalScript::OnCollision(CCollider2D * _pOther)
{
#ifdef NETWORK_ON
	return;
#endif // NETWORK_ON

	if (m_bAnimalDead)
		return;

	// 플레이어가 아닌 다른 물체 ( 환경요소 )
	if (CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Player")->GetLayerIdx() != _pOther->GetObj()->GetLayerIdx())
	{
		// 잔디는 충돌 x
		if (CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Environment")->GetLayerIdx() == _pOther->GetObj()->GetLayerIdx())
		{
			if (_pOther->GetObj()->GetScript<CNaturalScript>()->GetNaturalType() == NATURAL_BUSH)
			{
				return;
			}

			if (CollisionSphere(m_vOffsetScale, _pOther)) // 환경요소랑은 자기 몸통만큼 추가 충돌체크를 진행하고 막히면 반대로 튕겨나오도록
			{
				Vec3 vOtherPos = _pOther->Transform()->GetLocalPos();

				Vec3 vPos = Transform()->GetLocalPos();

				Vec3 vDir = XMVector3Normalize(vPos - vOtherPos);
				vDir.y = 0.f;

				m_vPrevPos = vPos;

				vPos += vDir * m_tStatus.fSpeed * DT;

				Vec3 vRot = _pOther->Transform()->GetLocalRot();

				//Transform()->SetLocalRot(Vec3(0.f, atan2(vDir.x, vDir.z) + 3.141592f, 0.f));
				vPos.y = CNaviMgr::GetInst()->GetY(Transform()->GetWorldPos());
				if (vPos.y <= 80.f)
				{
					vPos += -m_vMoveDir * m_tStatus.fSpeed * DT;
					vPos.y = CNaviMgr::GetInst()->GetY(Transform()->GetWorldPos());
				}
				Transform()->SetLocalPos(vPos);
			}
		}

		// 타입에 따른 추가 행동 요소 구현할 곳
		// 화살과 충돌체크
		if (CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Arrow")->GetLayerIdx() == _pOther->GetObj()->GetLayerIdx())
		{
			if (!_pOther->GetObj()->GetScript<CArrowScript>()->GetCollision())
			{
				if (CollisionSphere(m_vOffsetScale, _pOther)) // 화살에 맞음
				{
#ifdef NETWORK_ON
					CPacketMgr::GetInst()->Send_Attack_Player_Packet(0, m_iIndex, _pOther->GetObj()->GetScript<CArrowScript>()->GetDamage());
#else
#endif

					float fDamage = _pOther->GetObj()->GetScript<CArrowScript>()->GetDamage();
					Damage(_pOther->GetObj()->GetScript<CArrowScript>()->GetHost(), fDamage);
					_pOther->GetObj()->GetScript<CArrowScript>()->Collision(GetObj());
				}
			}
		}

		// 건물과 부딪힌건지 건물은 AABB로 충돌체크 수행
		if (CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"House")->GetLayerIdx() == _pOther->GetObj()->GetLayerIdx())
		{
			HOUSING_TYPE eType = _pOther->GetObj()->GetScript<CBuildScript>()->GetHousingType();
			if (eType >= HOUSING_TYPE::HOUSING_FOUNDATION && eType < HOUSING_TYPE::HOUSING_END)
			{
				bool bCollision = false;
				bCollision = CollisionHouse(m_vOffsetScale, _pOther, _pOther->GetObj()->GetScript<CBuildScript>()->GetOffsetScale(), eType);
				if (bCollision)
				{
					Vec3 vPos = Transform()->GetWorldPos();
					Vec3 vBuildPos = _pOther->Transform()->GetLocalPos();

					float fDiff = vBuildPos.y - vPos.y;

					Transform()->SetLocalPos(m_vPrevPos);
				}
			}
		}

		return;
	}


	// 회피형 동물 ( 사슴 )
	if (BEHAVIOR_TYPE::B_EVASION == m_tStatus.eType)
	{
		m_vMoveDir = -_pOther->Transform()->GetLocalDir(DIR_TYPE::FRONT);
		Vec3 vOtherPos = _pOther->Transform()->GetLocalPos();
		Vec3 vPos = Transform()->GetLocalPos();

		Vec3 vRot = _pOther->Transform()->GetLocalRot();
		Vec3 vDir = XMVector3Normalize(vPos - vOtherPos);
		vDir.y = 0.f;
		Transform()->SetLocalRot(Vec3(0.f, atan2(vDir.x, vDir.z) + 3.141592f, 0.f));

		m_vMoveDir = XMVector3Normalize(vPos - vOtherPos);
		m_vMoveDir.y = 0.f;

		m_bBehavior = true;
		m_fCurrentTime = m_tStatus.fBehaviorTime;
	}
	else if (BEHAVIOR_TYPE::B_PASSIVE == m_tStatus.eType)
	{
		// 비선공 유형 동물의 시야에 플레이어가 들어왔을때
		if (CollisionSphere(m_vOffsetScale, _pOther, 0.2f))
		{
			if (_pOther->GetObj() == m_pTarget)
			{
				if (m_fAttackTime < 0.f)
				{
					m_pTarget->GetScript<CPlayerScript>()->Damage(m_tStatus.fDamage);
					Animator3D()->ChangeAnimation(L"Attack");
					m_fAnimationCoolTime = ANIMAL_ANIMATION_COOLTIME;
					m_bAttack = true;
					m_fAttackTime = m_fAttackCoolTime;
				}
			}
		}

	}
	else if (BEHAVIOR_TYPE::B_WARLIKE == m_tStatus.eType)
	{
		m_pTarget = _pOther->GetObj();

		m_vMoveDir = -_pOther->Transform()->GetLocalDir(DIR_TYPE::FRONT);
		m_bBehavior = true;
		m_fCurrentTime = m_tStatus.fBehaviorTime;
		m_bIdleBehavior = false;

		Vec3 vOtherPos = _pOther->Transform()->GetLocalPos();
		Vec3 vPos = Transform()->GetLocalPos();
		Vec3 vDir = XMVector3Normalize(vOtherPos - vPos);
		vDir.y = 0.f;
		Vec3 vRot = _pOther->Transform()->GetLocalRot();
		Transform()->SetLocalRot(Vec3(-XM_PI / 2.f, atan2(vDir.x, vDir.z) + 3.141592f, 0.f));

		if (CollisionSphere(m_vOffsetScale, _pOther, 0.2f))
		{
			if (_pOther->GetObj() == m_pTarget)
			{
				if (m_fAttackTime < 0.f)
				{
					m_pTarget->GetScript<CPlayerScript>()->Damage(m_tStatus.fDamage);
					Animator3D()->ChangeAnimation(L"Attack");
					m_fAnimationCoolTime = ANIMAL_ANIMATION_COOLTIME;
					m_bAttack = true;
					m_fAttackTime = m_fAttackCoolTime;
				}
			}
		}
	}

}

void CAnimalScript::OnCollisionEnter(CCollider2D * _pOther)
{
#ifdef NETWORK_ON
	return;
#endif // NETWORK_ON

	if (m_bAnimalDead)
		return;
	if (CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Player")->GetLayerIdx() == _pOther->GetObj()->GetLayerIdx())
	{
		if (BEHAVIOR_TYPE::B_PASSIVE != m_tStatus.eType)
		{
			// 플레이어가 일정 범위 안에 들어왔을때
			if (m_bBehavior)
				m_bBehavior = false; // 하던 행동 중지

			// 비선공 유형 동물이 아닌 동물 시야에 플레이어가 들어왔을때
			if (m_bIdleBehavior)
				m_bIdleBehavior = false;
		}
	}
}

void CAnimalScript::OnCollisionExit(CCollider2D * _pOther)
{
#ifdef NETWORK_ON
	return;
#endif // NETWORK_ON
	if (m_bAnimalDead)
		return;
	// 플레이어가 시야반경에서 벗어나도 일정 행동을 수행하도록
	if (CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Player")->GetLayerIdx() == _pOther->GetObj()->GetLayerIdx())
	{
		// 회피형 동물 ( 사슴 )
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
			// 비선공 유형 동물의 시야에 플레이어가 들어왔을때

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
		// Particle Object 생성
		// ====================
		CGameObject* pObject = new CGameObject;
		pObject->SetName(L"Animal Particle");
		pObject->FrustumCheck(false);
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
		// 회피형 동물 ( 사슴 )
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
			m_bBehavior = true;
			m_pTarget = _pOther;
			m_fCurrentTime = m_tStatus.fBehaviorTime;

		}
		else if (BEHAVIOR_TYPE::B_WARLIKE == m_tStatus.eType)
		{
			m_pTarget = _pOther;

			m_vMoveDir = _pOther->Transform()->GetLocalDir(DIR_TYPE::FRONT);
			m_bBehavior = true;
			m_fCurrentTime = m_tStatus.fBehaviorTime;
		}
	}
	else // 사망 But 소멸 x
	{
		m_bAnimalDead = true;
		m_fLivingTime = 10.f;

		Animator3D()->ChangeAnimation( L"Die" );
	}
}

bool CAnimalScript::GetAnimalDead()
{
	return m_bAnimalDead;
}

void CAnimalScript::SetAnimalSpawner(CAnimalSpawner* pSpawner)
{
	m_pSpawner = pSpawner;
}

void CAnimalScript::SetAnimation( CAnimator3D * pAnimation )
{
	switch ( m_tStatus.eKind )
	{
	case A_BEAR:
		pAnimation->AddClip( L"Walk", 432, 465, ANIMATION_MODE::ONCE_RETURN );
		pAnimation->AddClip( L"Run", 398, 420, ANIMATION_MODE::ONCE_RETURN );
		pAnimation->AddClip( L"Idle", 234, 390, ANIMATION_MODE::LOOP );
		pAnimation->AddClip( L"Eat", 111, 233, ANIMATION_MODE::ONCE_RETURN );
		pAnimation->AddClip( L"Die", 0, 50, ANIMATION_MODE::ONCE_STOP );
		pAnimation->AddClip( L"Attack", 51, 110, ANIMATION_MODE::ONCE_RETURN );
		pAnimation->SetDefaultKey( L"Idle" );
		break;

	case A_BOAR:
		pAnimation->AddClip( L"Walk", 0, 29, ANIMATION_MODE::ONCE_RETURN );
		pAnimation->AddClip( L"Run", 30, 47, ANIMATION_MODE::ONCE_RETURN );
		pAnimation->AddClip( L"Idle", 231, 400, ANIMATION_MODE::LOOP );
		pAnimation->AddClip( L"Eat", 48, 230, ANIMATION_MODE::ONCE_RETURN );
		pAnimation->AddClip( L"Die", 401, 423, ANIMATION_MODE::ONCE_STOP );
		pAnimation->AddClip( L"Attack", 424, 445, ANIMATION_MODE::ONCE_RETURN );
		pAnimation->SetDefaultKey( L"Idle" );
		break;

	case A_DEER:	
		pAnimation->AddClip( L"Walk", 393, 423, ANIMATION_MODE::ONCE_RETURN );
		pAnimation->AddClip( L"Run", 423, 442, ANIMATION_MODE::ONCE_RETURN );
		pAnimation->AddClip( L"Idle", 0, 230, ANIMATION_MODE::LOOP );
		pAnimation->AddClip( L"Eat", 231, 320, ANIMATION_MODE::ONCE_RETURN );
		pAnimation->AddClip( L"Die", 321, 392, ANIMATION_MODE::ONCE_STOP );
		pAnimation->SetDefaultKey( L"Idle" );
		break;
		
	case A_WOLF:	
		pAnimation->AddClip( L"Walk", 0, 29, ANIMATION_MODE::ONCE_RETURN );
		pAnimation->AddClip( L"Run", 30, 46, ANIMATION_MODE::ONCE_RETURN );
		pAnimation->AddClip( L"Idle", 47, 212, ANIMATION_MODE::LOOP );
		pAnimation->AddClip( L"Eat", 213, 400, ANIMATION_MODE::ONCE_RETURN );
		pAnimation->AddClip( L"Die", 480, 546, ANIMATION_MODE::ONCE_STOP );
		pAnimation->AddClip( L"Attack", 547, 575, ANIMATION_MODE::ONCE_RETURN );
		pAnimation->SetDefaultKey( L"Idle" );
		break;

	default:
		break;
	}
}

bool CAnimalScript::CollisionHouse(Vec3 vOffsetScale, CCollider2D* _pOther, Vec3 vHouseOffsetScale, UINT iType)
{
	const Matrix& matCol1 = Collider2D()->GetColliderWorldMat();
	const Matrix& matCol2 = _pOther->GetColliderWorldMat();

	Vec3 vCol1 = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matCol1);
	Vec3 vCol2 = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matCol2);

	Vec3 vScale1 = Transform()->GetLocalScale();
	Vec3 vScale2 = _pOther->Transform()->GetLocalScale();

	Vec3 vColScale1 = vOffsetScale;
	Vec3 vColScale2 = Vec3(1.f, 1.f, 1.f);

	HOUSING_TYPE eType = (HOUSING_TYPE)iType;

	switch (eType)
	{
	case HOUSING_FOUNDATION:
		vColScale2 = vHouseOffsetScale;
		break;
	case HOUSING_WALL:
	case HOUSING_WINDOW:
	case HOUSING_DOOR:
	{
		Vec3 vRot = _pOther->Transform()->GetLocalRot();
		if (vRot.y != 0.f)
		{
			vColScale2 = Vec3(vHouseOffsetScale.z, vHouseOffsetScale.y, vHouseOffsetScale.x);
			//std::cout << "Turn" << vOffsetScale.x << std::endl;
		}
		else
		{
			vColScale2 = Vec3(vHouseOffsetScale.x, vHouseOffsetScale.y, vHouseOffsetScale.z);
		}
	}
	break;
	case HOUSING_FLOOR:
	{
		vColScale2 = vHouseOffsetScale;
	}
	break;
	default:
		vColScale2 = vHouseOffsetScale;
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