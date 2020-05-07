#pragma once

// CMyForm �� ���Դϴ�.

class CMyForm : public CFormView
{
	DECLARE_DYNCREATE(CMyForm)

protected:
	CMyForm();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CMyForm();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYFORM };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CButton m_btnAddObject;
	afx_msg void OnBnClickedButtonAddobject();
	CButton m_btnDelObject;
	afx_msg void OnBnClickedButtonDelobject();
	CString m_strObjectName;
	afx_msg void OnEnChangeEditObjectname();
	float m_fPosX;
	afx_msg void OnEnChangeEditPosx();
	float m_fPosY;
	afx_msg void OnEnChangeEditPosy();
	float m_fPosZ;
	afx_msg void OnEnChangeEditPosz();
	float m_fScaleX;
	afx_msg void OnEnChangeEditScalex();
	float m_fScaleY;
	afx_msg void OnEnChangeEditScaley();
	float m_fScaleZ;
	afx_msg void OnEnChangeEditScalez();
	float m_fRotX;
	afx_msg void OnEnChangeEditRotx();
	float m_fRotY;
	afx_msg void OnEnChangeEditRoty();
	float m_fRotZ;
	afx_msg void OnEnChangeEditRotz();
	CComboBox m_ComboObjectKind;
	afx_msg void OnCbnSelchangeComboObjectkind();
	CComboBox m_ComboObjects;
	afx_msg void OnCbnSelchangeComboObjects();
};


