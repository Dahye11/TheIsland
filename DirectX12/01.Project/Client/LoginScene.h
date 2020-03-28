#pragma once
#include <Engine/SceneScript.h>

class CGameObject;
class CChatScript;

class CLoginScene :
	public CSceneScript
{
public:
	CLoginScene();
	virtual ~CLoginScene();

	// CSceneScript��(��) ���� ��ӵ�
	virtual void Init() override;

private:
	CGameObject*		m_pID;
	CGameObject*		m_pPassword;
	CGameObject*		m_pIP;

	string				m_strID;
	string				m_strPassword;
	string				m_strIP;

public:
	CGameObject* GetIDObj();
	CGameObject* GetPasswordObj();

	CChatScript* GetID();
	CChatScript* GetPassword();
};

