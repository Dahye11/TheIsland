#pragma once
#include "Component.h"

#include "Ptr.h"
#include "Texture.h"
#include "Material.h"
#include "Mesh.h"

enum class ANIMATION_MODE
{
	LOOP,
	ONCE_RETURN,
	ONCE_STOP,
	END,
};

struct tAnimation
{
	int			iStartFrame;
	int			iEndFrame;
	int			iFrameLength;

	double		dStartTime;
	double		dEndTime;
	double		dTimeLength;

	ANIMATION_MODE	eMode;
};

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

	vector<float>				m_vecClipUpdateTime;
	vector<Matrix>				m_vecFinalBoneMat; // �ؽ��Ŀ� ������ ���� �������
	int							m_iFrameCount; // 30
	double						m_dCurTime;
	int							m_iCurClip; // Ŭ�� �ε���	
	int							m_iFrameIdx; // Ŭ���� ���� ������

	Ptr<CMaterial>				m_pBoneMtrl; // BoneData Update Compute Shader
	CStructuredBuffer*			m_pBoneFinalMat;  // Ư�� �������� ���� ���
	bool						m_bFinalMatUpdate; // ������� ���� ���࿩��

	wstring						m_strCurAniKey;
	wstring						m_strDefaultKey;
	map<wstring, tAnimation*>	m_mapClip;
	map<wstring, float>			m_mapClipUpdateTime;

	bool						m_bStop;			// �ִϸ��̼��� �׸� ��������

public:
	void SetBones(const vector<tMTBone>* _vecBones) { m_pVecBones = _vecBones; m_vecFinalBoneMat.resize(m_pVecBones->size()); }
	void SetAnimClip(const vector<tMTAnimClip>* _vecAnimClip);
	void UpdateData();
	void UpdateData_Inst(CStructuredBuffer* _pBoneBuffer, UINT _iRow);
	void SetClipTime(int _iClipIdx, float _fTime) { m_vecClipUpdateTime[_iClipIdx] = _fTime; }

	void AddClip( const wstring& strKey, int iStartFrame, int iEndFrame, ANIMATION_MODE eMode );

	CStructuredBuffer* GetFinalBoneMat() { return m_pBoneFinalMat; }
	UINT GetBoneCount() { return (UINT)m_pVecBones->size(); }

	void ChangeAnimation( const wstring& strKey );
	void SetDefaultKey( const wstring& strKey );			// �̰� ��������߸� ó�� Ŭ���� ���� ��.
	void Restart(const wstring& strKey = L"");				// �Ķ���Ϳ� �ƹ��͵� �ȳ����� �⺻������ ����Ʈ �ִϸ��̼����� �������

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

