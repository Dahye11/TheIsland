// MyForm.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Tool.h"
#include "MyForm.h"


#include <Engine/Scene.h>
#include <Engine/Layer.h>
#include <Engine/SceneMgr.h>
#include <Engine/SceneScript.h>

#include <Engine/ResMgr.h>

#include <Engine/Transform.h>
#include <Engine/ToolCamScript.h>

#include <Engine/CollisionMgr.h>
#include <Engine/Collider2D.h>

#include <algorithm>

#include <Engine/EventMgr.h>
#include <Engine/RenderMgr.h>
#include <Engine/NaviMgr.h>
#include <Engine/PathMgr.h>

#include "NaturalScript.h"

// CMyForm

IMPLEMENT_DYNCREATE( CMyForm, CFormView )

CMyForm::CMyForm()
	: CFormView( IDD_MYFORM )
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
	, m_iSelectObject( -1 )
	, m_bLoad( false )
	, m_iSelectScript( -1 )
{
}

CMyForm::~CMyForm()
{
}

void CMyForm::DoDataExchange( CDataExchange* pDX )
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
	DDX_Control( pDX, IDC_EDIT_OBJECTNAME, m_EditObjectName );
	DDX_Control( pDX, IDC_EDIT_POSX, m_EditPosX );
	DDX_Control( pDX, IDC_EDIT_POSY, m_EditPosY );
	DDX_Control( pDX, IDC_EDIT_POSZ, m_EditPosZ );
	DDX_Control( pDX, IDC_EDIT_SCALEX, m_EditScaleX );
	DDX_Control( pDX, IDC_EDIT_SCALEY, m_EditScaleY );
	DDX_Control( pDX, IDC_EDIT_SCALEZ, m_EditScaleZ );
	DDX_Control( pDX, IDC_EDIT_ROTX, m_EditRotX );
	DDX_Control( pDX, IDC_EDIT_ROTY, m_EditRotY );
	DDX_Control( pDX, IDC_EDIT_ROTZ, m_EditRotZ );
	DDX_Control( pDX, IDC_BUTTON_FBXLOAD, m_btnFBXLoad );
	DDX_Control( pDX, IDC_BUTTON_SAVE, m_btnSave );
	DDX_Control( pDX, IDC_BUTTON_LOAD, m_btnLoad );
	DDX_Control( pDX, IDC_EDIT_MOUSEPOSX, m_MousePosX );
	DDX_Control( pDX, IDC_EDIT_MOUSEPOSY, m_MousePosY );
	DDX_Control( pDX, IDC_EDIT_MOUSEPOSZ, m_MousePosZ );
	DDX_Control( pDX, IDC_EDIT_CAMERAPOSX, m_CameraPosX );
	DDX_Control( pDX, IDC_EDIT_CAMERAPOSY, m_CameraPosY );
	DDX_Control( pDX, IDC_EDIT_CAMERAPOSZ, m_CameraPosZ );
	DDX_Control( pDX, IDC_LIST_MESH, m_MeshList );
	DDX_Control( pDX, IDC_LIST_SCRIPT, m_ScriptList );
	DDX_Control( pDX, IDC_LIST_OBJECT, m_ObjectList );
}

BEGIN_MESSAGE_MAP( CMyForm, CFormView )
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
	ON_BN_CLICKED( IDC_BUTTON_FBXLOAD, &CMyForm::OnBnClickedButtonFbxload )
	ON_BN_CLICKED( IDC_BUTTON_SAVE, &CMyForm::OnBnClickedButtonSave )
	ON_BN_CLICKED( IDC_BUTTON_LOAD, &CMyForm::OnBnClickedButtonLoad )
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_EN_CHANGE( IDC_EDIT_MOUSEPOSX, &CMyForm::OnEnChangeEditMouseposx )
	ON_EN_CHANGE( IDC_EDIT_MOUSEPOSY, &CMyForm::OnEnChangeEditMouseposy )
	ON_EN_CHANGE( IDC_EDIT_MOUSEPOSZ, &CMyForm::OnEnChangeEditMouseposz )
	ON_EN_CHANGE( IDC_EDIT_CAMERAPOSX, &CMyForm::OnEnChangeEditCameraposx )
	ON_EN_CHANGE( IDC_EDIT_CAMERAPOSY, &CMyForm::OnEnChangeEditCameraposy )
	ON_EN_CHANGE( IDC_EDIT_CAMERAPOSZ, &CMyForm::OnEnChangeEditCameraposz )
	ON_LBN_SELCHANGE( IDC_LIST_MESH, &CMyForm::OnLbnSelchangeListMesh )
	ON_LBN_SELCHANGE( IDC_LIST_SCRIPT, &CMyForm::OnLbnSelchangeListScript )
	ON_LBN_SELCHANGE( IDC_LIST_OBJECT, &CMyForm::OnLbnSelchangeListObject )
	ON_BN_CLICKED( IDC_BUTTON_SCRIPTLOAD, &CMyForm::OnBnClickedButtonScriptload )
END_MESSAGE_MAP()


// CMyForm �����Դϴ�.

#ifdef _DEBUG
void CMyForm::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CMyForm::Dump( CDumpContext& dc ) const
{
	CFormView::Dump( dc );
}
#endif
#endif //_DEBUG


void CMyForm::OnBnClickedButtonAddobject()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if ( m_strObjectName.IsEmpty() )
		return;

	if ( m_iSelectScript < 0 )
		return;

	CScene* pScene = CSceneMgr::GetInst()->GetCurScene();

	wstring strFBXName = L"MeshData\\" + m_strFBXName + L".mdat";
	m_vecPath.push_back( strFBXName );

	Ptr<CMeshData> pMeshData = FindMeshData( strFBXName );

	CGameObject* pObject = pMeshData->Instantiate();
	pObject->SetName( m_strObjectName.GetString() );
	pObject->AddComponent( new CNaturalScript( ( NATURAL_TYPE )m_iSelectScript ) );
	
	switch ( m_iSelectScript )
	{
	case 0:
		pObject->Transform()->SetLocalRot( Vec3( -XM_PI / 2.f, 0.f, 0.f ) );
		pObject->Transform()->SetLocalScale( Vec3( 30.f, 30.f, 30.f) );
		break;
	default:
		break;
	}

	m_vecGameObjet.push_back( pObject );

	pScene->AddGameObject( L"Environment", pObject, false );

	m_ObjectList.AddString( m_strObjectName );
}


void CMyForm::OnBnClickedButtonDelobject()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	auto iter = m_vecGameObjet.begin();
	auto iterEnd = m_vecGameObjet.end();

	for ( iter; iter != m_vecGameObjet.end();)
	{
		if ( *iter == m_vecGameObjet[m_iSelectObject] )
		{
			tEvent tEv;
			tEv.eType = EVENT_TYPE::DELETE_OBJECT;
			tEv.wParam = ( DWORD_PTR )( *iter );
			CEventMgr::GetInst()->AddEvent( tEv );

			iter = m_vecGameObjet.erase( iter );

			m_iSelectObject -= 1;
		}

		else
		{
			++iter;
		}
	}


	m_ObjectList.DeleteString( m_iSelectObject );
}


void CMyForm::OnEnChangeEditObjectname()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CFormView::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	m_EditObjectName.GetWindowTextW( m_strObjectName );
}


void CMyForm::OnEnChangeEditPosx()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CFormView::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	m_iSelectObject = m_ObjectList.GetCurSel();

	CString posX, posY, posZ;
	m_EditPosX.GetWindowTextW( posX );
	m_EditPosY.GetWindowTextW( posY );
	m_EditPosZ.GetWindowTextW( posZ );

	m_fPosX = _tstof( posX );
	m_fPosY = _tstof( posY );
	m_fPosZ = _tstof( posZ );

	m_vecGameObjet[m_iSelectObject]->Transform()->SetLocalPos( Vec3( m_fPosX, m_fPosY, m_fPosZ ) );
}


void CMyForm::OnEnChangeEditPosy()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CFormView::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_iSelectObject = m_ObjectList.GetCurSel();

	CString posX, posY, posZ;
	m_EditPosX.GetWindowTextW( posX );
	m_EditPosY.GetWindowTextW( posY );
	m_EditPosZ.GetWindowTextW( posZ );

	m_fPosX = _tstof( posX );
	m_fPosY = _tstof( posY );
	m_fPosZ = _tstof( posZ );

	m_vecGameObjet[m_iSelectObject]->Transform()->SetLocalPos( Vec3( m_fPosX, m_fPosY, m_fPosZ ) );
}


void CMyForm::OnEnChangeEditPosz()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CFormView::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_iSelectObject = m_ObjectList.GetCurSel();

	CString posX, posY, posZ;
	m_EditPosX.GetWindowTextW( posX );
	m_EditPosY.GetWindowTextW( posY );
	m_EditPosZ.GetWindowTextW( posZ );

	m_fPosX = _tstof( posX );
	m_fPosY = _tstof( posY );
	m_fPosZ = _tstof( posZ );

	m_vecGameObjet[m_iSelectObject]->Transform()->SetLocalPos( Vec3( m_fPosX, m_fPosY, m_fPosZ ) );
}


void CMyForm::OnEnChangeEditScalex()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CFormView::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_iSelectObject = m_ObjectList.GetCurSel();

	CString ScaleX, ScaleY, ScaleZ;
	m_EditScaleX.GetWindowTextW( ScaleX );
	m_EditScaleY.GetWindowTextW( ScaleY );
	m_EditScaleZ.GetWindowTextW( ScaleZ );

	m_fScaleX = _tstof( ScaleX );
	m_fScaleY = _tstof( ScaleY );
	m_fScaleZ = _tstof( ScaleZ );

	m_vecGameObjet[m_iSelectObject]->Transform()->SetLocalScale( Vec3( m_fScaleX, m_fScaleY, m_fScaleZ ) );
}


void CMyForm::OnEnChangeEditScaley()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CFormView::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_iSelectObject = m_ObjectList.GetCurSel();

	CString ScaleX, ScaleY, ScaleZ;
	m_EditScaleX.GetWindowTextW( ScaleX );
	m_EditScaleY.GetWindowTextW( ScaleY );
	m_EditScaleZ.GetWindowTextW( ScaleZ );

	m_fScaleX = _tstof( ScaleX );
	m_fScaleY = _tstof( ScaleY );
	m_fScaleZ = _tstof( ScaleZ );

	m_vecGameObjet[m_iSelectObject]->Transform()->SetLocalScale( Vec3( m_fScaleX, m_fScaleY, m_fScaleZ ) );
}


void CMyForm::OnEnChangeEditScalez()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CFormView::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_iSelectObject = m_ObjectList.GetCurSel();

	CString ScaleX, ScaleY, ScaleZ;
	m_EditScaleX.GetWindowTextW( ScaleX );
	m_EditScaleY.GetWindowTextW( ScaleY );
	m_EditScaleZ.GetWindowTextW( ScaleZ );

	m_fScaleX = _tstof( ScaleX );
	m_fScaleY = _tstof( ScaleY );
	m_fScaleZ = _tstof( ScaleZ );

	m_vecGameObjet[m_iSelectObject]->Transform()->SetLocalScale( Vec3( m_fScaleX, m_fScaleY, m_fScaleZ ) );
}


void CMyForm::OnEnChangeEditRotx()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CFormView::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_iSelectObject = m_ObjectList.GetCurSel();

	CString RotX, RotY, RotZ;
	m_EditRotX.GetWindowTextW( RotX );
	m_EditRotY.GetWindowTextW( RotY );
	m_EditRotZ.GetWindowTextW( RotZ );

	m_fRotX = _tstof( RotX );
	m_fRotY = _tstof( RotY );
	m_fRotZ = _tstof( RotZ );

	m_vecGameObjet[m_iSelectObject]->Transform()->SetLocalRot( Vec3( m_fRotX, m_fRotY, m_fRotZ ) );
}


void CMyForm::OnEnChangeEditRoty()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CFormView::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_iSelectObject = m_ObjectList.GetCurSel();

	CString RotX, RotY, RotZ;
	m_EditRotX.GetWindowTextW( RotX );
	m_EditRotY.GetWindowTextW( RotY );
	m_EditRotZ.GetWindowTextW( RotZ );

	m_fRotX = _tstof( RotX );
	m_fRotY = _tstof( RotY );
	m_fRotZ = _tstof( RotZ );

	m_vecGameObjet[m_iSelectObject]->Transform()->SetLocalRot( Vec3( m_fRotX, m_fRotY, m_fRotZ ) );
}


void CMyForm::OnEnChangeEditRotz()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CFormView::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_iSelectObject = m_ObjectList.GetCurSel();

	CString RotX, RotY, RotZ;
	m_EditRotX.GetWindowTextW( RotX );
	m_EditRotY.GetWindowTextW( RotY );
	m_EditRotZ.GetWindowTextW( RotZ );

	m_fRotX = _tstof( RotX );
	m_fRotY = _tstof( RotY );
	m_fRotZ = _tstof( RotZ );

	m_vecGameObjet[m_iSelectObject]->Transform()->SetLocalRot( Vec3( m_fRotX, m_fRotY, m_fRotZ ) );
}

Ptr<CMeshData> CMyForm::FindMeshData( const wstring & strFBXName )
{
	for ( auto iter = m_vecFBX.begin(); iter != m_vecFBX.end(); ++iter )
	{
		if ( ( *iter ) == NULL )
			continue;

		if ( ( *iter )->GetName() == strFBXName )
			return *iter;
	}

	return nullptr;
}


void CMyForm::OnBnClickedButtonFbxload()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	wchar_t	strFilter[] = L"�������(*.*)|*.*|||";
	CFileDialog	dlg( TRUE, NULL, L"Mesh", OFN_HIDEREADONLY, strFilter );
	CGameObject* pObject = NULL;
	Ptr<CMeshData> pMeshData = nullptr;
	if ( IDOK == dlg.DoModal() )
	{
		CString strName = dlg.GetFileTitle();				// ���� �̸� �б�
		CString strExt = dlg.GetFileExt();
		wstring strMeshPath = L"MeshData\\" + strName + "." + strExt;
		wstring strFbxPath = L"FBX\\" + strName + "." + strExt;

		strExt.MakeUpper();

		CScene* pScene = CSceneMgr::GetInst()->GetCurScene();
		if ( lstrcmp( strExt, L"FBX" ) == 0 )
		{
			pMeshData = CResMgr::GetInst()->LoadFBX( strFbxPath );
			pMeshData->Save( pMeshData->GetPath() );

			wstring strMeshName = L"MeshData\\" + strName + ".mdat";

			pMeshData = CResMgr::GetInst()->Load<CMeshData>( strMeshName, strMeshName );

		}

		else if ( lstrcmp( strExt, L"MDAT" ) == 0 )
		{
			pMeshData = CResMgr::GetInst()->Load<CMeshData>( strMeshPath, strMeshPath );

		}

		m_vecFBX.push_back( pMeshData );
		m_MeshList.AddString( strName );
	}

	//CCollider2D* pColl1 = new CCollider2D;
	//CCollider2D* pColl2 = new CCollider2D;

	//CCollisionMgr::GetInst()->CollisionSphereRay( pColl1, pColl2 );
}


void CMyForm::OnBnClickedButtonSave()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if ( m_vecGameObjet.empty() )
		return;

	FILE* pFile = NULL;
	
	wstring ResPath = CPathMgr::GetResPath();
	ResPath += L"Data\\Map.dat";
	string FullPath{ ResPath.begin(), ResPath.end() };
	
	fopen_s( &pFile, FullPath.c_str(), "w" );

	int iSize = m_vecGameObjet.size();

	fwrite( &iSize, sizeof( int ), 1, pFile );

	for ( int i = 0; i < iSize; ++i )
	{
		wstring strName = m_vecGameObjet[i]->GetName();
		size_t iLength = strName.size();
		fwrite( &iLength, sizeof( size_t ), 1, pFile );
		fwrite( strName.c_str(), sizeof( wchar_t ), iLength, pFile );

		bool bNaturalScript = m_vecGameObjet[i]->GetScript<CNaturalScript>() == NULL ? false : true;
		fwrite( &bNaturalScript, sizeof( bool ), 1, pFile );

		if ( bNaturalScript )
		{
			wstring strPath = m_vecPath[i];
			iLength = strPath.size();
			fwrite( &iLength, sizeof( size_t ), 1, pFile );
			fwrite( strPath.c_str(), sizeof( wchar_t ), iLength, pFile );
			m_vecGameObjet[i]->GetScript<CNaturalScript>()->SaveToScene( pFile );
		}

		m_vecGameObjet[i]->Transform()->SaveToScene( pFile );		
	}

	fclose( pFile );
}


void CMyForm::OnBnClickedButtonLoad()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	FILE* pFile = NULL;

	wstring ResPath = CPathMgr::GetResPath();
	ResPath += L"Data\\Map.dat";
	string FullPath{ ResPath.begin(), ResPath.end() };

	fopen_s( &pFile, FullPath.c_str(), "r" );

	int iSize = 0;
	fread( &iSize, sizeof( int ), 1, pFile );
		
	for ( int i = 0; i < iSize; ++i )
	{
		CGameObject* pObject = nullptr;

		wchar_t strName[MAX_PATH]{};
		size_t iLength = 0;
		fread( &iLength, sizeof( size_t ), 1, pFile );
		fread( strName, sizeof( wchar_t ), iLength, pFile );

		bool bNaturalScript;
		fread( &bNaturalScript, sizeof( bool ), 1, pFile );

		if ( bNaturalScript )
		{
			wchar_t strPath[MAX_PATH]{};
			fread( &iLength, sizeof( size_t ), 1, pFile );
			fread( strPath, sizeof( wchar_t ), iLength, pFile );

			if ( iLength == 0 || strPath == L"")
				continue;

			Ptr<CMeshData> pMeshData = FindMeshData( strPath );
			
			if ( pMeshData == nullptr )
			{
				pMeshData = CResMgr::GetInst()->Load<CMeshData>( strPath, strPath );
				m_MeshList.AddString( strName );
				m_vecFBX.push_back( pMeshData );
			}

			pObject = pMeshData->Instantiate();
			pObject->AddComponent( new CNaturalScript( NATURAL_TREE ) );
			pObject->GetScript<CNaturalScript>()->LoadFromScene( pFile );

			m_vecPath.push_back( strPath );
		}

		pObject->Transform()->LoadFromScene( pFile );
		pObject->SetName( strName );

		CScene* pScene = CSceneMgr::GetInst()->GetCurScene();
		m_vecGameObjet.push_back( pObject );
		m_ObjectList.AddString( strName );
		pScene->AddGameObject( L"Environment", pObject, false );
	}
	
	fclose( pFile );
}

void CMyForm::OnLButtonDown( UINT nFlags, CPoint point )
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
}


void CMyForm::OnLButtonUp( UINT nFlags, CPoint point )
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.


	//m_bChoiceObject = false;

	CFormView::OnLButtonUp( nFlags, point );
}

CGameObject * CMyForm::GetSelectObject()
{
	if ( !m_vecGameObjet.empty() )
	{
		if ( m_vecGameObjet.size() > m_iSelectObject || m_iSelectObject > -1 )
			return m_vecGameObjet[m_iSelectObject];
	}

	return NULL;
}


void CMyForm::OnEnChangeEditMouseposx()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CFormView::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMyForm::OnEnChangeEditMouseposy()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CFormView::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMyForm::OnEnChangeEditMouseposz()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CFormView::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMyForm::OnEnChangeEditCameraposx()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CFormView::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMyForm::OnEnChangeEditCameraposy()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CFormView::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString strCameraY;
	m_CameraPosY.GetWindowTextW( strCameraY );
	float fCameraY = _tstof( strCameraY );
	CGameObject* pCamera = CSceneMgr::GetInst()->GetCurScene()->GetLayer( 0 )->GetMainCamera();
	CToolCamScript* pScript = pCamera->GetScript<CToolCamScript>();
	pScript->SetMaxY( fCameraY );
}


void CMyForm::OnEnChangeEditCameraposz()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CFormView::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMyForm::OnLbnSelchangeListMesh()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	m_MeshList.GetText( m_MeshList.GetCurSel(), m_strFBXName );
	m_EditObjectName.SetWindowTextW( m_strFBXName );
}


void CMyForm::OnLbnSelchangeListScript()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	m_iSelectScript = m_ScriptList.GetCurSel();

	if ( m_iSelectObject > 0 && m_iSelectObject < m_vecGameObjet.size() )
	{
		CNaturalScript* pScript = m_vecGameObjet[m_iSelectObject]->GetScript<CNaturalScript>();
		pScript->SetType( m_iSelectScript );


		switch ( m_iSelectScript )
		{
		case 0:
			m_vecGameObjet[m_iSelectObject]->Transform()->SetLocalRot( Vec3( -XM_PI / 2.f, 0.f, 0.f ) );
			m_vecGameObjet[m_iSelectObject]->Transform()->SetLocalScale( Vec3( 30.f, 30.f, 30.f ) );
			break;
		default:
			break;
		}
	}

}


void CMyForm::OnLbnSelchangeListObject()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_iSelectObject = m_ObjectList.GetCurSel();

	if ( m_iSelectObject >= m_vecGameObjet.size() || m_iSelectObject < 0 )
	{
		m_bChoiceObject = false;
		return;
	}


	Vec3 vPos = m_vecGameObjet[m_iSelectObject]->Transform()->GetLocalPos();
	Vec3 vScale = m_vecGameObjet[m_iSelectObject]->Transform()->GetLocalScale();
	Vec3 vRot = m_vecGameObjet[m_iSelectObject]->Transform()->GetLocalRot();

	CString PosX, PosY, PosZ;
	CString RotX, RotY, RotZ;
	CString ScaleX, ScaleY, ScaleZ;

	PosX.Format( L"%f", vPos.x );
	PosY.Format( L"%f", vPos.y );
	PosZ.Format( L"%f", vPos.z );
	RotX.Format( L"%f", vRot.x );
	RotY.Format( L"%f", vRot.y );
	RotZ.Format( L"%f", vRot.z );
	ScaleX.Format( L"%f", vScale.x );
	ScaleY.Format( L"%f", vScale.y );
	ScaleZ.Format( L"%f", vScale.z );

	m_EditPosX.SetWindowTextW( PosX );
	m_EditPosY.SetWindowTextW( PosY );
	m_EditPosZ.SetWindowTextW( PosZ );
	m_EditRotX.SetWindowTextW( RotX );
	m_EditRotY.SetWindowTextW( RotY );
	m_EditRotZ.SetWindowTextW( RotZ );
	m_EditScaleX.SetWindowTextW( ScaleX );
	m_EditScaleY.SetWindowTextW( ScaleY );
	m_EditScaleZ.SetWindowTextW( ScaleZ );

	m_bChoiceObject = true;
}


void CMyForm::OnBnClickedButtonScriptload()
{
	if ( m_bLoad )
		return;
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_ScriptList.AddString( L"Tree" );
	m_ScriptList.AddString( L"Stone" );
	m_ScriptList.AddString( L"Bush" );	
	m_ScriptList.AddString( L"None" );

	m_bLoad = true;
}
