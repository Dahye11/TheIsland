#pragma once
#include "Component.h"

enum class PROJ_TYPE
{
	ORTHGRAPHIC,
	PERSPECTIVE,
};

class CFrustum;

class CCamera :
	public CComponent
{
public:
	CCamera();
	virtual ~CCamera();

private:
	CFrustum		m_Frustum;
	
	float			m_fNear;			// View Space �þ� ���� (near ~ far)
	float			m_fFar;

	float			m_fFOV;				// ���� ���� ��, �þ߰�
	float			m_fScale;			// ���� ���� ��, ���� ����

	Matrix			m_matView;			// �����
	Matrix			m_matViewInv;		// �俪���
	Matrix			m_matProj;			// �������
	Matrix			m_matProjInv;		// ���������

	PROJ_TYPE		m_eProType;			// ī�޶� ���� 
	UINT			m_iLayerCheck;		// ī�޶� ��� Layer ��Ʈ��

public:
	virtual void FinalUpdate();
	void Render();

public:
	void SetProjType( PROJ_TYPE eType );
	void SetFar( float fFar );
	void SetScale( float fScale );
	void SetFOV( float fFOV );

	void SetLayerCheck( int iLayerIdx, bool bAdd );
	void SetLayerAllCheck();
	void SetLayerAllRelease();

public:
	PROJ_TYPE GetProjType();
	float GetFar();
	float GetScale();
	float GetFOV();

	const Matrix& GetViewMat();
	const Matrix& GetViewMatInv();
	const Matrix& GetProjMat();
	const Matrix& GetProjMatInv();

public:
	CLONE( CCamera );
	virtual void SaveToScene( FILE* pFile );
	virtual void LoadFromScene( FILE* pFile );
};

