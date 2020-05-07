// MyForm.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Tool.h"
#include "MyForm.h"


#include <Engine/Scene.h>
#include <Engine/SceneMgr.h>
#include <Engine/SceneScript.h>

#include <Engine/GameObject.h>
// CMyForm

IMPLEMENT_DYNCREATE(CMyForm, CFormView)

CMyForm::CMyForm()
	: CFormView(IDD_MYFORM)
	, m_strObjectName( _T( "" ) )
	, m_fPosX( 0 )
	, m_fPosY( 0 )
	, m_fPosZ( 0 )
	, m_fScaleX( 0 )
	, m_fScaleY( 0 )
	, m_fRotX( 0 )
	, m_fRotY( 0 )
	, m_fRotZ( 0 )
	, m_fScaleZ( 0 )
{
	
}

CMyForm::~CMyForm()
{
}

void CMyForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange( pDX );
	DDX_Control( pDX, IDC_BUTTON_ADDOBJECT, m_btnAddObject );
	DDX_Text( pDX, IDC_EDIT_OBJECTNAME, m_strObjectName );
	DDX_Text( pDX, IDC_EDIT_POSX, m_fPosX );
	DDX_Text( pDX, IDC_EDIT_POSY, m_fPosY );
	DDX_Text( pDX, IDC_EDIT_POSZ, m_fPosZ );
	DDX_Text( pDX, IDC_EDIT_SCALEX, m_fScaleX );
	DDX_Text( pDX, IDC_EDIT_SCALEY, m_fScaleY );
	DDX_Text( pDX, IDC_EDIT_SCALEZ, m_fScaleZ );
	DDX_Text( pDX, IDC_EDIT_ROTX, m_fRotX );
	DDX_Text( pDX, IDC_EDIT_ROTY, m_fRotY );
	DDX_Text( pDX, IDC_EDIT_ROTZ, m_fRotZ );
	DDX_Control( pDX, IDC_BUTTON_DELOBJECT, m_btnDelObject );
	DDX_Control( pDX, IDC_COMBO_OBJECTKIND, m_ComboObjectKind );
	DDX_Control( pDX, IDC_COMBO_OBJECTS, m_ComboObjects );
}

BEGIN_MESSAGE_MAP(CMyForm, CFormView)
	ON_BN_CLICKED( IDC_BUTTON_ADDOBJECT, &CMyForm::OnBnClickedButtonAddobject )
	ON_EN_CHANGE( IDC_EDIT_OBJECTNAME, &CMyForm::OnEnChangeEditObjectname )
	ON_EN_CHANGE( IDC_EDIT_POSX, &CMyForm::OnEnChangeEditPosx )
	ON_EN_CHANGE( IDC_EDIT_POSY, &CMyForm::OnEnChangeEditPosy )
	ON_EN_CHANGE( IDC_EDIT_POSZ, &CMyForm::OnEnChangeEditPosz )
	ON_EN_CHANGE( IDC_EDIT_SCALEX, &CMyForm::OnEnChangeEditScalex )
	ON_EN_CHANGE( IDC_EDIT_SCALEY, &CMyForm::OnEnChangeEditScaley )
	ON_EN_CHANGE( IDC_EDIT_SCALEZ, &CMyForm::OnEnChangeEditScalez )
	ON_EN_CHANGE( IDC_EDIT_ROTX, &CMyForm::OnEnChangeEditRotx )
	ON_EN_CHANGE( IDC_EDIT_ROTY, &CMyForm::OnEnChangeEditRoty )
	ON_EN_CHANGE( IDC_EDIT_ROTZ, &CMyForm::OnEnChangeEditRotz )
	ON_BN_CLICKED( IDC_BUTTON_DELOBJECT, &CMyForm::OnBnClickedButtonDelobject )
	ON_CBN_SELCHANGE( IDC_COMBO_OBJECTKIND, &CMyForm::OnCbnSelchangeComboObjectkind )
	ON_CBN_SELCHANGE( IDC_COMBO_OBJECTS, &CMyForm::OnCbnSelchangeComboObjects )
END_MESSAGE_MAP()


// CMyForm �����Դϴ�.

#ifdef _DEBUG
void CMyForm::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CMyForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


void CMyForm::OnBnClickedButtonAddobject()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	CScene* pScene = CSceneMgr::GetInst()->GetCurScene();

	CGameObject* pObject = new CGameObject;
	pScene->AddGameObject( L"Default", pObject, false );

	int a = 0;
}


void CMyForm::OnBnClickedButtonDelobject()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMyForm::OnEnChangeEditObjectname()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CFormView::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMyForm::OnEnChangeEditPosx()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CFormView::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMyForm::OnEnChangeEditPosy()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CFormView::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMyForm::OnEnChangeEditPosz()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CFormView::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMyForm::OnEnChangeEditScalex()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CFormView::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMyForm::OnEnChangeEditScaley()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CFormView::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMyForm::OnEnChangeEditScalez()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CFormView::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMyForm::OnEnChangeEditRotx()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CFormView::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMyForm::OnEnChangeEditRoty()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CFormView::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMyForm::OnEnChangeEditRotz()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CFormView::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMyForm::OnCbnSelchangeComboObjectkind()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	int  iSel = m_ComboObjectKind.GetCurSel();

	//CString strScriptName;
	//m_ComboObjectKind.GetLBText( iSel, strScriptName);
}


void CMyForm::OnCbnSelchangeComboObjects()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}
