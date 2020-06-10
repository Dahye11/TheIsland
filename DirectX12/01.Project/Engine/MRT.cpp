#include "stdafx.h"
#include "MRT.h"

#include "Device.h"
#include "RenderMgr.h"

CMRT::CMRT() 
	: m_arrRT{}
	, m_tVP{}
	, m_tScissorRect{}
{
}

CMRT::~CMRT()
{
}

void CMRT::Create( UINT iCount, tRT * arrRT, Ptr<CTexture> pDSTex )
{
	// DepthStencilTexture �� ����.
	assert( nullptr != pDSTex );

	m_iRTCount = iCount;
	memcpy( m_arrRT, arrRT, sizeof( tRT ) * iCount );
	m_pDSTex = pDSTex;

	// ������� RTV �� �����д�.
	D3D12_DESCRIPTOR_HEAP_DESC tDesc = {};
	tDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	tDesc.NumDescriptors = m_iRTCount;
	tDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	tDesc.NodeMask = 0;
	DEVICE->CreateDescriptorHeap( &tDesc, IID_PPV_ARGS( &m_pRTV ) );
	D3D12_CPU_DESCRIPTOR_HANDLE hRTVHeap = m_pRTV->GetCPUDescriptorHandleForHeapStart();

	UINT iRTVSize = CRenderMgr::GetInst()->GetRTVHeapSize();

	for ( UINT i = 0; i < m_iRTCount; ++i )
	{
		UINT iDestRange = 1;
		UINT iSrcRange = 1;

		D3D12_CPU_DESCRIPTOR_HANDLE hDescHandle = m_pRTV->GetCPUDescriptorHandleForHeapStart();
		hDescHandle.ptr += iRTVSize * i;

		ComPtr<ID3D12DescriptorHeap> pRTVHeap = m_arrRT[i].pTarget->GetRTV();
		D3D12_CPU_DESCRIPTOR_HANDLE hSrcHandle = pRTVHeap->GetCPUDescriptorHandleForHeapStart();

		DEVICE->CopyDescriptors( 1, &hDescHandle, &iDestRange
			, 1, &hSrcHandle, &iSrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_RTV );
	}

	for ( int i = 0; i < m_iRTCount; ++i )
	{
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = m_arrRT[i].pTarget->GetTex2D().Get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;	// Ÿ�ٿ���
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;			// �Ϲ� ���ҽ���
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		m_TargetToRes[i] = barrier;

		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;		// ���ҽ�
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;	// ���� Ÿ������
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		m_ResToTarget[i] = barrier;
	}

	m_tVP = D3D12_VIEWPORT{ 0.f, 0.f, m_arrRT[0].pTarget->Width() , m_arrRT[0].pTarget->Height(), 0.f, 1.f };
	m_tScissorRect = D3D12_RECT{ 0, 0, (LONG)m_arrRT[0].pTarget->Width(), (LONG)m_arrRT[0].pTarget->Height() };
}

void CMRT::OMSet( UINT iCount, UINT iOffset )
{
	UINT iRTVSize = CRenderMgr::GetInst()->GetRTVHeapSize();
	D3D12_CPU_DESCRIPTOR_HANDLE hRTVHandle = m_pRTV->GetCPUDescriptorHandleForHeapStart();
	hRTVHandle.ptr += iRTVSize * iOffset;

	D3D12_CPU_DESCRIPTOR_HANDLE hDSVHandle = m_pDSTex->GetDSV()->GetCPUDescriptorHandleForHeapStart();

	// ����Ʈ ����
	CMDLIST->RSSetViewports(1, &m_tVP);
	CMDLIST->RSSetScissorRects(1, &m_tScissorRect);

	// Ÿ�� ����	
	CMDLIST->OMSetRenderTargets( iCount, &hRTVHandle, FALSE, &hDSVHandle );
}

void CMRT::OMSet()
{
	// RenderTarget �� DepthStencilView �� ����	
	D3D12_CPU_DESCRIPTOR_HANDLE hRTVHandle = m_pRTV->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE hDSVHandle = m_pDSTex->GetDSV()->GetCPUDescriptorHandleForHeapStart();

	// ����Ʈ ����
	CMDLIST->RSSetViewports(1, &m_tVP);
	CMDLIST->RSSetScissorRects(1, &m_tScissorRect);

	// Ÿ�� ����	
	CMDLIST->OMSetRenderTargets( m_iRTCount, &hRTVHandle, TRUE/*DescHeap �� ���������� �ִ�*/, &hDSVHandle );
}

void CMRT::Clear()
{
	ResToTargetBarrier();

	// Ÿ�� Ŭ����	
	UINT iRTVSize = CRenderMgr::GetInst()->GetRTVHeapSize();

	for ( UINT i = 0; i < m_iRTCount; ++i )
	{
		D3D12_CPU_DESCRIPTOR_HANDLE hRTVHandle = m_pRTV->GetCPUDescriptorHandleForHeapStart();
		hRTVHandle.ptr += iRTVSize * i;
		float arrClearColor[4] = { m_arrRT[i].vClearColor.x, m_arrRT[i].vClearColor.y, m_arrRT[i].vClearColor.z, m_arrRT[i].vClearColor.w };
		CMDLIST->ClearRenderTargetView( hRTVHandle, arrClearColor, 0, nullptr );
	}

	if ( nullptr != m_pDSTex )
	{
		D3D12_CPU_DESCRIPTOR_HANDLE hDSVHandle = m_pDSTex->GetDSV()->GetCPUDescriptorHandleForHeapStart();
		CMDLIST->ClearDepthStencilView( hDSVHandle, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr );
	}
}

void CMRT::Clear( UINT iRTidx )
{
	// Ÿ�� Ŭ����	
	UINT iRTVSize = CRenderMgr::GetInst()->GetRTVHeapSize();
	D3D12_CPU_DESCRIPTOR_HANDLE hRTVHandle = m_pRTV->GetCPUDescriptorHandleForHeapStart();

	hRTVHandle.ptr += iRTVSize * iRTidx;
	float arrClearColor[4] = { m_arrRT[iRTidx].vClearColor.x
		, m_arrRT[iRTidx].vClearColor.y
		, m_arrRT[iRTidx].vClearColor.z
		, m_arrRT[iRTidx].vClearColor.w };

	CMDLIST->ClearRenderTargetView( hRTVHandle, arrClearColor, 0, nullptr );

	if ( nullptr != m_pDSTex )
	{
		D3D12_CPU_DESCRIPTOR_HANDLE hDSVHandle = m_pDSTex->GetDSV()->GetCPUDescriptorHandleForHeapStart();
		CMDLIST->ClearDepthStencilView( hDSVHandle, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr );
	}
}

void CMRT::TargetToResBarrier()
{
	CMDLIST->ResourceBarrier( m_iRTCount, m_TargetToRes );
}

void CMRT::ResToTargetBarrier()
{
	CMDLIST->ResourceBarrier( m_iRTCount, m_ResToTarget );
}

Ptr<CTexture> CMRT::GetRTTex( UINT iIdx )
{
	return m_arrRT[iIdx].pTarget;
}

Ptr<CTexture> CMRT::GetDSTex()
{
	return m_pDSTex;
}

D3D12_VIEWPORT CMRT::GetViewPort() const
{
	return m_tVP;
}
