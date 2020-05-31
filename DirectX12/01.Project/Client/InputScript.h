#pragma once
#include <Engine/Script.h>

// ���콺�� Ŭ���ؼ� ���ڸ� �Է��ϴ� ��쿡 ���

class CInputScript :
	public CScript
{
	int						m_iCount;
	int						m_iMaxCount;
	bool					m_bEnable;

	string					m_strInput;
	string					m_strEmpty;

	Vec4					m_vBox;
	// {x1, y1, x2, y2}

	Vec4					m_vHighlightColor;
	Vec4					m_vBackColor;
	Vec4					m_vFontColor;
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
	void Clear();
	// ���콺 �浹�Ҷ� ����� �ڽ� {x1, y1, x2, y2}
	void SetRect(Vec4 rc);
	Vec4 GetRect();
	// �۾� ���� �⺻���� ���
	void SetFontColor(Vec4 vColor);
	// �۾� ��� ���� �⺻���� ����
	void SetHighlightColor(Vec4 vColor);
	void SetBackColor(Vec4 vColor);

	void SetEnable(bool bEnable);

	void SetMaxCount(int iCount);

	bool GetEnable();
	string GetString();
};

