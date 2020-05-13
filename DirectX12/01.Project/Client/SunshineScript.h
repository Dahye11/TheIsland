#pragma once
#include <Engine/Script.h>

class CSunshineScript :
	public CScript
{
	float m_fTime;
	float m_fDarkness;

	CGameObject* m_pSkybox;

public:
	CSunshineScript();
	virtual ~CSunshineScript();

	CLONE(CSunshineScript);

	// CScript��(��) ���� ��ӵ�
	virtual void Update() override;

public:
	float GetTime();
	void SetTime(float fTime);

	void SetSkybox(CGameObject* pObject);
};

