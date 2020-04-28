#include "stdafx.h"
#include "Animator3D.h"

#include "TimeMgr.h"
#include "ResMgr.h"
#include "MeshRender.h"

#include "StructuredBuffer.h"

CAnimator3D::CAnimator3D() :
	m_iCurClip(0),
	m_dCurTime(0.f),
	m_iFrameCount(30),
	CComponent(COMPONENT_TYPE::ANIMATOR3D),
	m_pBoneFinalMat(NULL),
	m_bFinalMatUpdate(false)
{
	m_pBoneMtrl = CResMgr::GetInst()->FindRes<CMaterial>( L"Aniamtion3DUpdateMtrl" );
	m_pBoneFinalMat = new CStructuredBuffer;
}


CAnimator3D::~CAnimator3D()
{
	SAFE_DELETE( m_pBoneFinalMat );
}

CAnimator3D * CAnimator3D::Clone()
{
	return new CAnimator3D(*this);
}

void CAnimator3D::SetBones( const vector<tMTBone>* vecBones )
{
	m_pVecBones = vecBones;
	m_vecFinalBoneMat.resize( m_pVecBones->size() );
}

void CAnimator3D::SetAnimClip( const vector<tMTAnimClip>* vecAnimClip )
{
	m_pVecClip = vecAnimClip;
	m_vecClipUpdateTime.resize( m_pVecClip->size() );
}

void CAnimator3D::UpdateData()
{
	if ( !m_bFinalMatUpdate )
	{
		// Bone Data Update	
		Ptr<CMesh> pMesh = MeshRender()->GetMesh();
		pMesh->GetBoneFrameDataBuffer()->UpdateData_CS( TEXTURE_REGISTER::t10 );
		pMesh->GetBoneOffsetBuffer()->UpdateData_CS( TEXTURE_REGISTER::t11 );

		Check_Mesh( pMesh );
		m_pBoneFinalMat->UpdateRWData( UAV_REGISTER::u0 );

		UINT iBoneCount = ( UINT )m_pVecBones->size();
		m_pBoneMtrl->SetData( SHADER_PARAM::INT_0, &iBoneCount );
		m_pBoneMtrl->SetData( SHADER_PARAM::INT_1, &m_iFrameIdx );

		UINT iGrounX = ( iBoneCount / 256 ) + 1;
		m_pBoneMtrl->Dispatch( iGrounX, 1, 1 );

		m_bFinalMatUpdate = true;
	}

	// t7 �������Ϳ� ������� ������(��������) ���ε�
	m_pBoneFinalMat->UpdateData( TEXTURE_REGISTER::t7 );
}

void CAnimator3D::SetClipTime( int iClipIdx, float fTime )
{
	m_vecClipUpdateTime[iClipIdx] = fTime;
}

vector<Matrix>& CAnimator3D::GetFinalBoneMat()
{
	// TODO: ���⿡ ��ȯ ������ �����մϴ�.
	return m_vecFinalBoneMat;
}

void CAnimator3D::Check_Mesh( Ptr<CMesh> pMesh )
{
	UINT iBoneCount = pMesh->GetBoneCount();
	if ( m_pBoneFinalMat->GetElementCount() < iBoneCount )
	{
		m_pBoneFinalMat->Create( sizeof( Matrix ), iBoneCount, nullptr );
	}
}

void CAnimator3D::Update()
{
}

void CAnimator3D::LateUpdate()
{
}

void CAnimator3D::FinalUpdate()
{
	m_dCurTime = 0.f;

	// ���� ������� Clip �� �ð��� �����Ѵ�.
	m_vecClipUpdateTime[m_iCurClip] += DT;

	if ( m_vecClipUpdateTime[m_iCurClip] >= m_pVecClip->at( m_iCurClip ).dTimeLength )
	{
		m_vecClipUpdateTime[m_iCurClip] = 0.f;
	}

	m_dCurTime = m_pVecClip->at( m_iCurClip ).dStartTime + m_vecClipUpdateTime[m_iCurClip];

	m_iFrameIdx = ( int )( m_dCurTime * ( float )m_iFrameCount );

	m_bFinalMatUpdate = false;
}

void CAnimator3D::SaveToScene( FILE * pFile )
{
}

void CAnimator3D::LoadFromScene( FILE * pFile )
{
}
