#pragma once
#include "Component.h"

#include "Ptr.h"
#include "Material.h"
#include "Mesh.h"

class CStructuredBuffer;

class CParticleSystem :
	public CComponent
{
public:
	CParticleSystem();
	~CParticleSystem();
	CLONE( CParticleSystem );

private:
	CStructuredBuffer*	m_pParticleBuffer;			// ����Ŭ ������ ����
	CStructuredBuffer*	m_pSharedBuffer;			// ��ƼŬ ���� �޸�
	UINT				m_iMaxParticle;				// �ִ� ��ƼŬ ����

	Ptr<CMaterial>		m_pUpdateMtrl;				// ��ǻƮ ���̴�
	Ptr<CMaterial>		m_pMtrl;					// ��ƼŬ ��������
	Ptr<CMesh>			m_pMesh;					// ��� �޽� (Point Mesh)

	float				m_fFrequency;				// ��ƼŬ ���� �ֱ�
	float				m_fAccTime;					// ��ƼŬ �����ð� üũ
	
	float				m_fMinLifeTime;				// ��ƼŬ �ּ� ���� �ð�
	float				m_fMaxLifeTime;				// ��ƼŬ �ִ� ���� �ð�

	float				m_fMinSpeed;				// ��ƼŬ ���� �ӵ�
	float				m_fMaxSpeed;				// ��ƼŬ ���� �ӵ�

	float				m_fStartScale;				// ��ƼŬ ���� ũ��
	float				m_fEndScale;				// ��ƼŬ ���� ũ��

	Vec4				m_vStartColor;				// ��ƼŬ �ʱ� ����
	Vec4				m_vEndColor;				// ��ƼŬ ���� ����

	int					m_iKind;					// ��ƼŬ ����


public:
	virtual void FinalUpdate();
	virtual void Render();

public:
	void SetStartColor( const Vec4& vColor );
	void SetEndColor( const Vec4& vColor );

	void SetStartSpeed(float fSpeed);
	void SetEndSpeed(float fSpeed);

	void SetKind( int iKind );

public:
	virtual void SaveToScene( FILE* _pFile );
	virtual void LoadFromScene( FILE* _pFile );
};

