#pragma once
#include <Engine/SceneScript.h>

class CIngameScene :
	public CSceneScript
{
public:
	CIngameScene();
	virtual ~CIngameScene();

	// CSceneScript��(��) ���� ��ӵ�
	virtual void Init() override;
};

