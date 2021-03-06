#pragma once
#include <Engine/Script.h>
#define MAX_CHAT_LINE 9

class CChatScript : 
	public CScript
{
	CGameObject*			m_pChatObj[MAX_CHAT_LINE];
	string					m_strChat[MAX_CHAT_LINE];
	string					m_strChatName[MAX_CHAT_LINE];

	Vec4					m_vNameColor;
	Vec4					m_vChatColor;
	Vec4					m_vBackColor;

	bool					m_bCheat;
public:
	CChatScript();
	virtual ~CChatScript();

	CLONE( CChatScript );

public:
	virtual void Update() override;

public:
	void AddChatObject(CGameObject* pObject, int line);

	void AddChat(string name, string chat);
};

