#pragma once
#include <Engine/SceneScript.h>

class CTestScene :
	public CSceneScript
{
public:
	CTestScene();
	virtual ~CTestScene();

	// CSceneScript��(��) ���� ��ӵ�
	virtual void Init() override;
	virtual void Update() override;

};

