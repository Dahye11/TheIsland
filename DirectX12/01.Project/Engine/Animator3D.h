#pragma once
#include "Component.h"

#include "Ptr.h"
#include "Texture.h"
#include "Material.h"
#include "Mesh.h"

class CStructuredBuffer;

class CAnimator3D :
	public CComponent
{
public:
	CAnimator3D();
	~CAnimator3D();

	CAnimator3D* Clone();

private:
	const vector<tMTBone>*		m_pVecBones;
	const vector<tMTAnimClip>*	m_pVecClip;

	vector<float>					m_vecClipUpdateTime;
	vector<Matrix>				m_vecFinalBoneMat; // �ؽ��Ŀ� ������ ���� �������
	int										m_iFrameCount; // 30
	double								m_dCurTime;
	int										m_iCurClip; // Ŭ�� �ε���	
	int										m_iFrameIdx; // Ŭ���� ���� ������

	Ptr<CMaterial>				m_pBoneMtrl; // BoneData Update Compute Shader
	CStructuredBuffer*		m_pBoneFinalMat;  // Ư�� �������� ���� ���
	bool									m_bFinalMatUpdate; // ������� ���� ���࿩��

public:
	void SetBones( const vector<tMTBone>* vecBones );
	void SetAnimClip( const vector<tMTAnimClip>* vecAnimClip );
	//void SetBoneTex( Ptr<CTexture> pBoneTex );
	//Ptr<CTexture> GetBornTex();

	void UpdateData();

	void SetClipTime( int iClipIdx, float fTime );

	vector<Matrix>& GetFinalBoneMat();

private:
	void Check_Mesh( Ptr<CMesh> pMesh );

public:
	virtual void Awake() {};
	virtual void Start() {};
	virtual void Update();
	virtual void LateUpdate();
	virtual void FinalUpdate();

public:
	virtual void SaveToScene( FILE* pFile );
	virtual void LoadFromScene( FILE* pFile );
};

