#pragma once
#include <Engine/Script.h>

class CRainScript :
	public CScript
{
public:
	CRainScript();
	~CRainScript();

	// CScript��(��) ���� ��ӵ�
	CLONE( CRainScript );
	virtual void Update() override;

private:
	bool m_bActive;

	float m_fAlpha;
};

