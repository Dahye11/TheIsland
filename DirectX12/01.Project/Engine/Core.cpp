#include "stdafx.h"
#include "Core.h"

#include "Device.h"
#include "KeyMgr.h"
#include "TimeMgr.h"
#include "ResMgr.h"

#include "SceneMgr.h"
#include "EventMgr.h"
#include "RenderMgr.h"

#include "PathMgr.h"
#include "ConstantBuffer.h"

#include "FontMgr.h"

CCore::CCore()
	: m_hMainHwnd(nullptr)
	, m_hInstance(NULL)
{
}

CCore::~CCore()
{
}

int CCore::Init(HWND _hWnd, const tResolution & _resolution, bool _bWindow)
{
	m_hMainHwnd = _hWnd;
	ChangeWindowSize(m_hMainHwnd, _resolution);
	ShowWindow(_hWnd, true);


	if ( FAILED( CDevice::GetInst()->Init( _hWnd, _resolution, _bWindow ) ) )
	{
		return E_FAIL;
	}

	CRenderMgr::GetInst()->Init( _hWnd, _resolution, _bWindow );

	// ������� �����
	CDevice::GetInst()->CreateConstBuffer(L"TRANSFORM_MATRIX", sizeof(tTransform), 512, CONST_REGISTER::b0);
	CDevice::GetInst()->CreateConstBuffer(L"MATERIAL_PARAM", sizeof(tMtrlParam), 512, CONST_REGISTER::b1);
	CDevice::GetInst()->CreateConstBuffer(L"ANIM2D", sizeof(tMtrlParam), 512, CONST_REGISTER::b2);

	// ���� ������� ����(1������ ���� �������Ϳ��� ���ӵǾ���)
	CDevice::GetInst()->CreateConstBuffer(L"LIGHT2D", sizeof(tLight2DInfo), 1, CONST_REGISTER::b3, true);
	CDevice::GetInst()->CreateConstBuffer(L"LIGHT3D", sizeof(tLight3DInfo), 1, CONST_REGISTER::b4, true);
	CDevice::GetInst()->CreateConstBuffer(L"GLOBAL VALUE", sizeof(tGlobalValue), 1, CONST_REGISTER::b5);

	// ���� ������� ���
	CDevice::GetInst()->SetGlobalConstBufferToRegister(CDevice::GetInst()->GetCB(CONST_REGISTER::b3), 0);
	CDevice::GetInst()->SetGlobalConstBufferToRegister(CDevice::GetInst()->GetCB(CONST_REGISTER::b4), 0);
	CDevice::GetInst()->SetGlobalConstBufferToRegister(CDevice::GetInst()->GetCB(CONST_REGISTER::b5), 0);


	CPathMgr::Init();
	CKeyMgr::GetInst()->Init();
	CTimeMgr::GetInst()->Init();
	CResMgr::GetInst()->Init();
	CSceneMgr::GetInst()->Init();
	CFontMgr::GetInst()->Init();

	return S_OK;
}

int CCore::ToolInit( HINSTANCE _hInstance, HWND _hWnd, const tResolution& _start, const tResolution & _resolution, bool _bWindow )
{
	m_hMainHwnd = _hWnd;
	m_hInstance = _hInstance;
	ChangeWindowSize( m_hMainHwnd, _start, _resolution );
	ShowWindow( _hWnd, true );


	if ( FAILED( CDevice::GetInst()->Init( _hWnd, _resolution, _bWindow ) ) )
	{
		return E_FAIL;
	}

	CRenderMgr::GetInst()->Init( _hWnd, _resolution, _bWindow );

	// ������� �����
	CDevice::GetInst()->CreateConstBuffer( L"TRANSFORM_MATRIX", sizeof( tTransform ), 512, CONST_REGISTER::b0 );
	CDevice::GetInst()->CreateConstBuffer( L"MATERIAL_PARAM", sizeof( tMtrlParam ), 512, CONST_REGISTER::b1 );
	CDevice::GetInst()->CreateConstBuffer( L"ANIM2D", sizeof( tMtrlParam ), 512, CONST_REGISTER::b2 );

	// ���� ������� ����(1������ ���� �������Ϳ��� ���ӵǾ���)
	CDevice::GetInst()->CreateConstBuffer( L"LIGHT2D", sizeof( tLight2DInfo ), 1, CONST_REGISTER::b3, true );
	CDevice::GetInst()->CreateConstBuffer( L"LIGHT3D", sizeof( tLight3DInfo ), 1, CONST_REGISTER::b4, true );
	CDevice::GetInst()->CreateConstBuffer( L"GLOBAL VALUE", sizeof( tGlobalValue ), 1, CONST_REGISTER::b5 );

	// ���� ������� ���
	CDevice::GetInst()->SetGlobalConstBufferToRegister( CDevice::GetInst()->GetCB( CONST_REGISTER::b3 ), 0 );
	CDevice::GetInst()->SetGlobalConstBufferToRegister( CDevice::GetInst()->GetCB( CONST_REGISTER::b4 ), 0 );
	CDevice::GetInst()->SetGlobalConstBufferToRegister( CDevice::GetInst()->GetCB( CONST_REGISTER::b5 ), 0 );


	CPathMgr::Init();
	CKeyMgr::GetInst()->Init();
	CTimeMgr::GetInst()->Init();
	CResMgr::GetInst()->Init();
	CSceneMgr::GetInst()->Init();
	CFontMgr::GetInst()->Init();

	return S_OK;
}


void CCore::ChangeWindowSize(HWND _hWnd, const tResolution & _resolution)
{
	RECT rt = { 0, 0, (int)_resolution.fWidth, (int)_resolution.fHeight };

	AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, false);
	SetWindowPos(_hWnd, nullptr, 10, 10, rt.right - rt.left, rt.bottom - rt.top, 0);
}

void CCore::ChangeWindowSize( HWND _hWnd, const tResolution & _start, const tResolution & _resolution )
{
	RECT rt = { ( int )_start.fWidth,  ( int )_start.fHeight, ( int )_resolution.fWidth, ( int )_resolution.fHeight };

	AdjustWindowRect( &rt, WS_OVERLAPPEDWINDOW, false );
	SetWindowPos( _hWnd, nullptr, 1, 1	, rt.right - rt.left, rt.bottom - rt.top, 0 );
}

void CCore::Progress()
{
	CKeyMgr::GetInst()->Update();
	CTimeMgr::GetInst()->Update();
	CSound::g_pFMOD->update();

	CEventMgr::GetInst()->clear();
	{
		CSceneMgr::GetInst()->Update();
		CRenderMgr::GetInst()->Render();
	}
	CEventMgr::GetInst()->Update();
}