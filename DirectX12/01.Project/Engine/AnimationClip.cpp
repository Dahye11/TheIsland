#include "stdafx.h"
#include "AnimationClip.h"


CAnimationClip::CAnimationClip() :
	m_iAnimationLimitFrame(24)
{
}


CAnimationClip::~CAnimationClip()
{
	for ( size_t i = 0; i < m_vecKeyFrame.size(); ++i )
	{
		Safe_Delete_Vector( m_vecKeyFrame[i]->vecKeyFrame );
	}
}

tAnimation3DClip CAnimationClip::GetClipInfo() const
{
	return m_tInfo;
}

tKeyFrame * CAnimationClip::GetKeyFrame( int iBone, int iFrame ) const
{
	assert( iBone >= 0 && iBone < m_vecKeyFrame.size() );
	
	return m_vecKeyFrame[iBone]->vecKeyFrame[iFrame];
}

bool CAnimationClip::IsEmptyFrame( int iBone ) const
{
	return m_vecKeyFrame[iBone]->vecKeyFrame.empty();
}

void CAnimationClip::SetClipInfo( const wstring & strName, ANIMATION_OPTION eOption, int iFrameMode, int iStartFrame, int iEndFrame, double dStartTime, double dEndTime )
{
	m_tInfo.eOption = eOption;
	m_tInfo.strName = strName;
	m_tInfo.iStartFrame = iStartFrame;
	m_tInfo.iEndFrame = iEndFrame;
	m_tInfo.iFrameLength = iEndFrame - iStartFrame;
	m_tInfo.dStartTime = dStartTime;
	m_tInfo.dEndTime = dEndTime;
	m_tInfo.dTimeLength = dEndTime - dStartTime;
	m_iAnimationLimitFrame = iFrameMode;
}

void CAnimationClip::SetClipInfo( ANIMATION_OPTION eOption, tFbxAnimClip * pClip )
{
	// ���ڷ� ���� �ִϸ��̼� �ɼ������� �����Ѵ�.
	m_tInfo.eOption = eOption;
	m_tInfo.strName = pClip->strName;

	// FBX Animation Clip���ִ� StartTime�� EndTime�� �̿��Ͽ� KeyFrame�� ���´�.
	m_tInfo.iStartFrame = pClip->tStartTime.GetFrameCount( pClip->eMode );
	m_tInfo.iEndFrame = pClip->tEndTime.GetFrameCount( pClip->eMode );
	m_tInfo.iFrameLength = m_tInfo.iEndFrame - m_tInfo.iStartFrame;

	// �ð� ������ �������ش�.
	m_tInfo.dStartTime = pClip->tStartTime.GetSecondDouble();
	m_tInfo.dEndTime = pClip->tEndTime.GetSecondDouble();
	m_tInfo.dTimeLength = m_tInfo.dEndTime - m_tInfo.dStartTime;

	// Ű ������ ����ŭ �ݺ��ϸ� ������ �������� ������ ��� ������ ��ġ, ũ��, ȸ�������� �̾ƿ´�.
	for ( size_t i = 0; i < pClip->vecBoneFrame.size(); ++i )
	{
		tBoneKeyFrame* pBoneKeyFrame = new tBoneKeyFrame;
		pBoneKeyFrame->iBoneIndex = pClip->vecBoneFrame[i].iBoneIdx;
		m_vecKeyFrame.push_back( pBoneKeyFrame );

		// �Ʒ����� Ű ������ ������ �����Ѵ�.
		pBoneKeyFrame->vecKeyFrame.reserve( pClip->vecBoneFrame[i].vecKeyFrame.size() );

		for ( size_t j = 0; j < pClip->vecBoneFrame[i].vecKeyFrame.size(); ++j )
		{
			tKeyFrame* pKeyFrame = new tKeyFrame;

			pKeyFrame->dTime = pClip->vecBoneFrame[i].vecKeyFrame[j].dTime;

			// ���� ���� Ű �����ӿ� �ش��ϴ� ��� ������ ���´�.
			FbxAMatrix mat = pClip->vecBoneFrame[i].vecKeyFrame[j].matTransform;

			FbxVector4 vPos, vScale;
			FbxQuaternion qRot;

			// ��ķκ��� ��ġ, ũ��, ȸ�� ������ ���´�.
			vPos = mat.GetT();
			vScale = mat.GetS();
			qRot = mat.GetQ();

			pKeyFrame->vScale = Vec3( ( float )vScale.mData[0], ( float )vScale.mData[1], ( float )vScale.mData[2] );
			pKeyFrame->vPos = Vec3( ( float )vPos.mData[0], ( float )vPos.mData[1], ( float )vPos.mData[2] );
			pKeyFrame->vRot = Vec4( ( float )qRot.mData[0], ( float )qRot.mData[1], ( float )qRot.mData[2], (float)qRot.mData[3] );

			pBoneKeyFrame->vecKeyFrame.push_back( pKeyFrame );
		}
	}
}

void CAnimationClip::AddCallback( int iFrame, void( *pFunc )( float ) )
{
	// �ִϸ��̼� �߰��� ȣ���� �ݹ��Լ��� �߰�
	tAnimationCallback* pCallback = new tAnimationCallback;

	// ���� ������ ����
	pCallback->iAnimationProgress = iFrame;
	// ���� �����ӿ��� ���� �������� ���ְ� ���̷� ����� ���� ������� ���Ѵ�.
	pCallback->fAnimationProgress = ( iFrame - m_tInfo.iStartFrame ) / ( float )( m_tInfo.iFrameLength );
	pCallback->func = bind( pFunc, std::placeholders::_1 );
	pCallback->bCall = false;

	m_tInfo.vecCallback.push_back( pCallback );
}

void CAnimationClip::AddCallback( float fProgress, void( *pFunc )( float ) )
{
	tAnimationCallback* pCallback = new tAnimationCallback;

	// �������� ������ ������ �������� �����ش�.
	// ������ * �ð����� + ���۽ð� = ����ð�
	// ����ð��� 1�ʴ� ������ ���� �����־� �������� ���Ѵ�.
	pCallback->iAnimationProgress = ( fProgress * m_tInfo.dTimeLength + m_tInfo.dStartTime ) * m_iAnimationLimitFrame;
	pCallback->fAnimationProgress = fProgress;
	pCallback->func = bind( pFunc, std::placeholders::_1 );
	pCallback->bCall = false;
	m_tInfo.vecCallback.push_back( pCallback );
}

void CAnimationClip::SetAnimationLimitFrame( int iAnimationLimitFrame )
{
	m_iAnimationLimitFrame = iAnimationLimitFrame;
}
