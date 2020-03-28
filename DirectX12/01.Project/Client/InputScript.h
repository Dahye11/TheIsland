#pragma once
#include <Engine/Script.h>

// ���콺�� Ŭ���ؼ� ���ڸ� �Է��ϴ� ��쿡 ���

class CInputScript :
	public CScript
{
	bool					m_bEnable;
	int						m_iCount;
	string					m_strInput;
	vector<CGameObject*>	m_vFont;
	Rect					m_rcBox;
public:
	CInputScript();
	virtual ~CInputScript();

	CLONE(CInputScript);

	// CScript��(��) ���� ��ӵ�
	virtual void Update() override;

public:
	void AddInputObject(CGameObject* pObject);
	void SetRect(Rect rc);
	void Init();
};

