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
	CGameObject*			m_pSpace;

	Vec4					m_vBox;
	// {x1, y1, x2, y2}

	Vec4					m_vFontColor;
	Vec4					m_vFontBackColor;

public:
	CInputScript();
	virtual ~CInputScript();

	CLONE(CInputScript);

	// CScript��(��) ���� ��ӵ�
	virtual void Update() override;

private:
	void WriteFont(char font);

public:
	void Init();
	// ��Ʈ�� ��� ���� �Ѱ����� ���ӿ�����Ʈ�� �ִ� �޽� �ϳ��� ���
	// �ִ� 15ĭ�� ���ڿ��̶�� 15���� ���ӿ�����Ʈ ����
	void AddInputObject(CGameObject* pObject);
	// ���콺 �浹�Ҷ� ����� �ڽ� {x1, y1, x2, y2}
	void SetRect(Vec4 rc);
	Vec4 GetRect();
	// �۾� ���� �⺻���� ���
	void SetFontColor(Vec4 vColor);
	// �۾� ��� ���� �⺻���� ����
	void SetFontBackColor(Vec4 vColor);

	// �۾� ��� ������Ʈ
	void SetFontSpace(CGameObject* pObject);

	void SetEnable(bool bEnable);

	// �Է�ĭ�� �Էµ� ���ڿ� ��ȯ
	string GetString();
	bool GetEnable();
};

