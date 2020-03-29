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

	Vec4					m_vFontColor;
	Vec4					m_vFontBackColor;

public:
	CInputScript();
	virtual ~CInputScript();

	CLONE(CInputScript);

	// CScript��(��) ���� ��ӵ�
	virtual void Update() override;

public:
	void Init();
	// ��Ʈ�� ��� ���� �Ѱ����� ���ӿ�����Ʈ�� �ִ� �޽� �ϳ��� ���
	// �ִ� 15ĭ�� ���ڿ��̶�� 15���� ���ӿ�����Ʈ ����
	void AddInputObject(CGameObject* pObject);
	// ���콺 �浹�Ҷ� ����� �ڽ�
	void SetRect(Rect rc);
	// �۾� ���� �⺻���� ���
	void SetFontColor(Vec4 vColor);
	// �۾� ��� ���� �⺻���� ����
	void SetFontBackColor(Vec4 vColor);

	void SetEnable(bool bEnable);

	// �Է�ĭ�� �Էµ� ���ڿ� ��ȯ
	string GetString();
	bool GetEnable();
};

