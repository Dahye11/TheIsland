#include "stdafx.h"
#include "Animator3D.h"

#include "TimeMgr.h"
#include "MeshData.h"
#include "Material.h"

CAnimator3D::CAnimator3D() :
	m_iCurClip(0),
	m_dCurTime(0.f),
	m_iFrameCount(30),
	CComponent(COMPONENT_TYPE::ANIMATOR3D)
{
}


CAnimator3D::~CAnimator3D()
{
}

CAnimator3D * CAnimator3D::Clone()
{
	return new CAnimator3D(*this);
}

void CAnimator3D::SetBones( const vector<tMTBone>* vecBones )
{
	m_vecBones = vecBones;
	m_vecFinalBoneMat.resize( m_vecBones->size() );
}

void CAnimator3D::SetAnimClip( const vector<tMTAnimClip>* vecAnimClip )
{
	m_vecClip = vecAnimClip;
	m_vecClipUpdateTime.resize( m_vecClip->size() );
}

void CAnimator3D::SetBoneTex( Ptr<CTexture> pBoneTex )
{
	m_pBoneTex = pBoneTex;
}

Ptr<CTexture> CAnimator3D::GetBornTex()
{
	return m_pBoneTex;
}

void CAnimator3D::UpdateData()
{
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

	if ( m_vecClipUpdateTime[m_iCurClip] >= m_vecClip->at( m_iCurClip ).dTimeLength )
	{
		m_vecClipUpdateTime[m_iCurClip] = 0.f;
	}

	m_dCurTime = m_vecClip->at( m_iCurClip ).dStartTime + m_vecClipUpdateTime[m_iCurClip];

	// �� ������ŭ �ݺ��ϸ� ���� �ð��� �°� ��� �� ����� ��� �������ش�.
	for ( size_t i = 0; i < m_vecBones->size(); ++i )
	{
		if ( m_vecBones->at( i ).vecKeyFrame.empty() )
		{
			// Ű������ �� ����� ���� ���� ���
			m_vecFinalBoneMat[i] = m_vecBones->at( i ).matBone;
			continue;
		}

		// �ð��� ���Ͽ� �ε����� ���Ѵ�.
		int	iFrameIndex = ( int )( m_dCurTime * ( float )m_iFrameCount );
		int	iFrameNextIndex = 0;

		/*{
			m_vecFinalBoneMat[i] = m_vecBones->at(i).matOffset *
				XMMatrixAffineTransformation(m_vecBones->at(i).vecKeyFrame[iFrameIndex].vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f)
					, m_vecBones->at(i).vecKeyFrame[iFrameIndex].qRot
					, m_vecBones->at(i).vecKeyFrame[iFrameIndex].vTranslate);
			continue;
		}*/

		if ( iFrameIndex >= m_vecClip->at( 0 ).iFrameLength - 1 )
		{
			iFrameNextIndex = iFrameIndex;
		}
		else
		{
			iFrameNextIndex = iFrameIndex + 1;
		}

		const tMTKeyFrame& tKeyFrame = m_vecBones->at( i ).vecKeyFrame[iFrameIndex];
		const tMTKeyFrame& tNextKeyFrame = m_vecBones->at( i ).vecKeyFrame[iFrameNextIndex];

		double	fFrameTime = tKeyFrame.dTime;
		double	fNextFrameTime = tNextKeyFrame.dTime;

		// �����Ӱ��� �ð��� ���� ������ �����ش�.
		double	fPercent = ( m_dCurTime - fFrameTime ) / ( 1.f / m_iFrameCount );

		XMVECTOR vS1 = tKeyFrame.vScale;
		XMVECTOR vS2 = tNextKeyFrame.vScale;

		XMVECTOR vT1 = tKeyFrame.vTranslate;
		XMVECTOR vT2 = tNextKeyFrame.vTranslate;

		XMVECTOR vQ1 = XMLoadFloat4( &tKeyFrame.qRot );
		XMVECTOR vQ2 = XMLoadFloat4( &tNextKeyFrame.qRot );

		XMVECTOR vS = XMVectorLerp( vS1, vS2, ( float )fPercent );
		XMVECTOR vT = XMVectorLerp( vT1, vT2, ( float )fPercent );
		XMVECTOR vQ = XMQuaternionSlerp( vQ1, vQ2, ( float )fPercent );

		XMVECTOR vQZero = XMVectorSet( 0.f, 0.f, 0.f, 1.f );

		// ������ ����� ���Ͽ� ���� ������� ������.				
		m_vecFinalBoneMat[i] = m_vecBones->at( i ).matOffset * XMMatrixAffineTransformation( vS, vQZero, vQ, vT );
	}
}

void CAnimator3D::SaveToScene( FILE * pFile )
{
}

void CAnimator3D::LoadFromScene( FILE * pFile )
{
}
