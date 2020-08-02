
// ToolView.cpp : CToolView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "Tool.h"
#endif

#include "ToolDoc.h"
#include "ToolView.h"
#include "MainFrm.h"

#include <Engine/Core.h>
#include <Engine/Scene.h>
#include <Engine/Layer.h>
#include <Engine/SceneScript.h>
#include <Engine/SceneMgr.h>
#include <Engine/NaviMgr.h>
#include <Engine/GameObject.h>
#include <Engine/Transform.h>

#include "MapToolScene.h"
#include "MyForm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CToolView

IMPLEMENT_DYNCREATE( CToolView, CScrollView )

BEGIN_MESSAGE_MAP( CToolView, CScrollView )
	// ǥ�� �μ� ����Դϴ�.
	ON_COMMAND( ID_FILE_PRINT, &CScrollView::OnFilePrint )
	ON_COMMAND( ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint )
	ON_COMMAND( ID_FILE_PRINT_PREVIEW, &CScrollView::OnFilePrintPreview )
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CToolView ����/�Ҹ�
HWND g_hWnd;

CToolView::CToolView() :
	m_pForm( NULL )
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
}

CToolView::~CToolView()
{
}

BOOL CToolView::PreCreateWindow( CREATESTRUCT& cs )
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CScrollView::PreCreateWindow( cs );
}

// CToolView �׸���

// WM_PAINT �޽��� �߻��� �� ȣ���.
void CToolView::OnDraw( CDC* pDC )
{
	CToolDoc* pDoc = GetDocument();
	ASSERT_VALID( pDoc );
	if ( !pDoc )
		return;



	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.

}


// CToolView �μ�

BOOL CToolView::OnPreparePrinting( CPrintInfo* pInfo )
{
	// �⺻���� �غ�
	return DoPreparePrinting( pInfo );
}

void CToolView::OnBeginPrinting( CDC* /*pDC*/, CPrintInfo* /*pInfo*/ )
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

}

void CToolView::OnEndPrinting( CDC* /*pDC*/, CPrintInfo* /*pInfo*/ )
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
}


// CToolView ����

#ifdef _DEBUG
void CToolView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CToolView::Dump( CDumpContext& dc ) const
{
	CScrollView::Dump( dc );
}

CToolDoc* CToolView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT( m_pDocument->IsKindOf( RUNTIME_CLASS( CToolDoc ) ) );
	return ( CToolDoc* )m_pDocument;
}
#endif //_DEBUG


// CToolView �޽��� ó����


void CToolView::SetForm( CMyForm * pForm )
{
	m_pForm = pForm;
}

void CToolView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	if ( FAILED( CCore::GetInst()->ToolInit( AfxGetInstanceHandle(), m_hWnd, tResolution{ 0, 0 }, tResolution{ 1280, 1000 }, true ) ) )
	{
		return;
	}


	CScene* pScene = CSceneMgr::GetInst()->GetCurScene();

	CMapToolScene* pScript = pScene->CreateSceneScript<CMapToolScene>( L"SceneScript" );

	CScrollView::SetScrollSizes( MM_TEXT, CSize( 1280, 720 ) );
}

// WM_LBUTTONDOWN �޽��� �߻� �� ȣ���.
void CToolView::OnLButtonDown( UINT nFlags, CPoint point )
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	CScrollView::OnLButtonDown( nFlags, point );

	/*D3DXVECTOR3 vMouse =
	{
		(float)point.x + GetScrollPos(0),
		(float)point.y + GetScrollPos(1),
		0.f
	};*/

	CGameObject* pObject = m_pForm->GetSelectObject();
	if ( !pObject )
		return;

	Vec3 vPos = pObject->Transform()->GetLocalPos();

	Vec3 vDis;
	vDis.x = m_vCamera.x - ( 640.f - m_vMouse.x );
	vDis.z = m_vCamera.z + ( 500.f - m_vMouse.y );

	vPos.x = vDis.x;
	vPos.z = vDis.z + 20.f;

	vPos.y = CNaviMgr::GetInst()->GetY( vPos );

	pObject->Transform()->SetLocalPos( vPos );


	// ȭ�鰻�� �Լ�. WM_PAINT �޽����� �߻���Ŵ.
	Invalidate( FALSE );
}

// WM_MOUSEMOVE �޽��� �߻� �� ȣ���.
void CToolView::OnMouseMove( UINT nFlags, CPoint point )
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CGameObject* pMouse = CSceneMgr::GetInst()->Mouse();
	m_vMouse = pMouse->Transform()->GetLocalPos();

	CString strPosX, strPosY, strPosZ;
	strPosX.Format( L"%f", m_vMouse.x );
	strPosY.Format( L"%f", m_vMouse.y );
	strPosZ.Format( L"%f", m_vMouse.z );

	m_pForm->m_MousePosX.SetWindowTextW( strPosX );
	m_pForm->m_MousePosY.SetWindowTextW( strPosY );
	m_pForm->m_MousePosZ.SetWindowTextW( strPosZ );

	CGameObject* pCamera = CSceneMgr::GetInst()->GetCurScene()->GetLayer( 0 )->GetMainCamera();
	m_vCamera = pCamera->Transform()->GetLocalPos();

	strPosX.Format( L"%f", m_vCamera.x );
	strPosY.Format( L"%f", m_vCamera.y );
	strPosZ.Format( L"%f", m_vCamera.z );

	m_pForm->m_CameraPosX.SetWindowTextW( strPosX );
	m_pForm->m_CameraPosY.SetWindowTextW( strPosY );
	m_pForm->m_CameraPosZ.SetWindowTextW( strPosZ );

	CScrollView::OnMouseMove( nFlags, point );
}
