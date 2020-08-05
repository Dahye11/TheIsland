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

	unordered_map<wstring, tMTAnimClip> m_mapClip;

	vector<float>				m_vecClipUpdateTime;
	vector<Matrix>				m_vecFinalBoneMat; // �ؽ��Ŀ� ������ ���� �������
	int							m_iFrameCount; // 30
	double						m_dCurTime;
	int							m_iCurClip; // Ŭ�� �ε���	
	int							m_iFrameIdx; // Ŭ���� ���� ������

	Ptr<CMaterial>				m_pBoneMtrl; // BoneData Update Compute Shader
	CStructuredBuffer*			m_pBoneFinalMat;  // Ư�� �������� ���� ���
	bool						m_bFinalMatUpdate; // ������� ���� ���࿩��

public:
	void SetBones(const vector<tMTBone>* _vecBones) { m_pVecBones = _vecBones; m_vecFinalBoneMat.resize(m_pVecBones->size()); }
	void SetAnimClip(const vector<tMTAnimClip>* _vecAnimClip);
	void UpdateData();
	void UpdateData_Inst(CStructuredBuffer* _pBoneBuffer, UINT _iRow);
	void SetClipTime(int _iClipIdx, float _fTime) { m_vecClipUpdateTime[_iClipIdx] = _fTime; }

	void SetAnimClipInfo( wstring strAnimName, tMTAnimClip tClip );

	CStructuredBuffer* GetFinalBoneMat() { return m_pBoneFinalMat; }
	UINT GetBoneCount() { return (UINT)m_pVecBones->size(); }

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

