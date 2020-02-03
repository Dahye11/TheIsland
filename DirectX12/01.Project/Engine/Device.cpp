#include "stdafx.h"
#include "Device.h"
#include "ConstantBuffer.h"
#include "Texture.h"

DEFINITION_SINGLE( CDevice )

CDevice::CDevice() :
	m_pDevice( NULL ),
	m_pFence( NULL ),
	m_pFactory( NULL ),
	m_iCurTargetIdx( 0 ),
	m_hFenceEvent( NULL ),
	m_iFenceValue( 0 ),
	m_iCurDummyIdx( 0 )
{
}

CDevice::~CDevice()
{
	WaitForFenceEvent();
	CloseHandle( m_hFenceEvent );

	for ( size_t i = 0; i < m_vecCB.size(); ++i )
	{
		SAFE_DELETE( m_vecCB[i] );
	}
}

int CDevice::Init( HWND hWnd, const RESOLUTION & res, bool bWindow )
{
	m_hWnd = hWnd;
	m_tResolution = res;
	m_bWindow = bWindow;

	UINT iFlag = 0;

#ifdef _DEBUG
	D3D12GetDebugInterface( IID_PPV_ARGS( &m_pDbgCtrl ) );
	m_pDbgCtrl->EnableDebugLayer();
#endif	

	CreateDXGIFactory( IID_PPV_ARGS( &m_pFactory ) );

	// CreateDevice
	D3D12CreateDevice( nullptr, D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS( &m_pDevice ) );

	// CreateFence
	m_pDevice->CreateFence( 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( &m_pFence ) );
	m_iFenceValue = 1;

	// Create an event handle to use for frame synchronization.
	m_hFenceEvent = CreateEvent( nullptr, FALSE, FALSE, nullptr );
	if ( m_hFenceEvent == nullptr )
	{
		assert( nullptr );
	}

	// Command Queue 	
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	m_pDevice->CreateCommandQueue( &queueDesc, IID_PPV_ARGS( &m_pCmdQueue ) );

	// Create Command Allocator
	m_pDevice->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( &m_pCmdAlloc ) );

	// Creat CmdListRes
	m_pDevice->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( &m_pCmdAllocRes ) );

	// Create the command list.
	m_pDevice->CreateCommandList( 0, D3D12_COMMAND_LIST_TYPE_DIRECT
		, m_pCmdAlloc.Get(), nullptr, IID_PPV_ARGS( &m_pCmdListGraphic ) );

	// CmdListGraphic�� ����ϱ� ���� �ݾ�����Ѵ�.
	m_pCmdListGraphic->Close();

	m_pDevice->CreateCommandList( 0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCmdAllocRes.Get(), NULL, IID_PPV_ARGS( &m_pCmdListRes ) );
	
	// SwapChain �����
	CreateSwapChain();

	// View �����
	CreateView();

	// ViewPort �����
	CreateViewPort();

	// Empty Signature �����
	// ��Ʈ ���� 
	// �׸��� ȣ�� ���� �ش� �ڿ��� ���������ο� ���� �ڷ��̸�,
	// ��� ������ ���̴���, �Ǵ� ���̴� �ڿ����� ��� �� �������� ���������� ����Ѵ�.
	D3D12_ROOT_SIGNATURE_DESC sigDesc = {};
	sigDesc.NumParameters = 0;
	sigDesc.pParameters = nullptr;
	sigDesc.NumStaticSamplers = 0;
	sigDesc.pStaticSamplers = nullptr; // ��� �� Sampler ����
	sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; // �Է� ������ �ܰ�

	ComPtr<ID3DBlob> pSignature;
	ComPtr<ID3DBlob> pError;
	D3D12SerializeRootSignature( &sigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pSignature, &pError );
	m_pDevice->CreateRootSignature( 0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS( &m_arrSig[( UINT )ROOT_SIG_TYPE::INPUT_ASSEM] ) );

	return S_OK;
}

void CDevice::Render_Start( float( &arrFloat )[4] )
{
	m_iCurDummyIdx = 0;

	// �׸��� �غ�
	m_pCmdAlloc->Reset();

	m_pCmdListGraphic->Reset( m_pCmdAlloc.Get(), NULL );

	m_pCmdListGraphic->SetGraphicsRootSignature( m_arrSig[( UINT )ROOT_SIG_TYPE::INPUT_ASSEM].Get() );

	// �ʿ��� ���� ����
	m_pCmdListGraphic->RSSetViewports( 1, &m_tVP );
	m_pCmdListGraphic->RSSetScissorRects( 1, &m_tScissorRect );

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE; ;
	barrier.Transition.pResource = m_RenderTargets[m_iCurTargetIdx].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;		// ��¿���
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // �ٽ� ����۷� ����
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	m_pCmdListGraphic->ResourceBarrier( 1, &barrier );
	
	// RenderTarget�� DepthStencilView�� ����
	D3D12_CPU_DESCRIPTOR_HANDLE	hRTVHandle = m_pRTV->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE hDSVHandle = m_pDSV->GetCPUDescriptorHandleForHeapStart();

	// Ÿ�� ����
	hRTVHandle.ptr += m_iCurTargetIdx * m_iRTVHeapSize;
	m_pCmdListGraphic->OMSetRenderTargets( 1, &hRTVHandle, FALSE, nullptr );

	// Ÿ�� Ŭ����
	m_pCmdListGraphic->ClearRenderTargetView( hRTVHandle, arrFloat, 0, NULL );
	m_pCmdListGraphic->ClearDepthStencilView( hDSVHandle, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, NULL );

	// ù ��° ���� Descriptor Heap �ʱ�ȭ
	ClearDummyDescriptorHeap( 0 );
}

void CDevice::Render_Present()
{
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE; ;
	barrier.Transition.pResource = m_RenderTargets[m_iCurTargetIdx].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;	// ����ۿ���
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;			// �ٽ� ������� ����

	m_pCmdListGraphic->ResourceBarrier( 1, &barrier );
	m_pCmdListGraphic->Close();

	// Ŀ�ǵ� ����Ʈ ����	
	ID3D12CommandList* ppCommandLists[] = { m_pCmdListGraphic.Get() };
	m_pCmdQueue->ExecuteCommandLists( _countof( ppCommandLists ), ppCommandLists );

	// Present the frame.
	m_pSwapChain->Present( 0, 0 );

	WaitForFenceEvent();

	// ����� Ÿ�� �ε��� ����
	m_iCurTargetIdx == 0 ? m_iCurTargetIdx = 1 : m_iCurTargetIdx = 0;

	// ������� ������ �ʱ�ȭ
	for ( size_t i = 0; i < m_vecCB.size(); ++i )
		m_vecCB[i]->Clear();
}

void CDevice::WaitForFenceEvent()
{
	const UINT64 fence = m_iFenceValue;
	m_pCmdQueue->Signal( m_pFence.Get(), fence );
	m_iFenceValue++;

	int a = m_pFence->GetCompletedValue();

	if ( a < fence )
	{
		m_pFence->SetEventOnCompletion( fence, m_hFenceEvent );
		WaitForSingleObject( m_hFenceEvent, INFINITE );
	}
}

void CDevice::CreateConstBuffer( const wstring& strName, size_t iSize, size_t iMaxSize,	CONST_REGISTER eRegister, bool bGlobal )
{
	CConstantBuffer* pCB = new CConstantBuffer;
	pCB->SetName( strName );
	pCB->Create( (UINT)iSize, (UINT)iMaxSize, eRegister );
	m_vecCB.push_back( pCB );

	if ( bGlobal )
		SetGlobalConstBufferToRegister( pCB, 0 );
}

void CDevice::SetConstBufferToRegister( CConstantBuffer * pCB, UINT iOffset )
{
	UINT iDestRange = 1;
	UINT iSrcRange = 1;

	// 0�� ������ ������� ������
	D3D12_CPU_DESCRIPTOR_HANDLE hDescHandle = m_vecDummyDescriptor[m_iCurDummyIdx]->GetCPUDescriptorHandleForHeapStart();
	hDescHandle.ptr += m_iCBVIncreSize * ( UINT )pCB->GetRegisterNum();

	D3D12_CPU_DESCRIPTOR_HANDLE hSrcHandle = pCB->GetCBV()->GetCPUDescriptorHandleForHeapStart();
	hSrcHandle.ptr += iOffset * m_iCBVIncreSize;

	m_pDevice->CopyDescriptors( 1, &hDescHandle, &iDestRange, 1, &hSrcHandle, &iSrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
}

void CDevice::SetGlobalConstBufferToRegister( CConstantBuffer * pCB, UINT iOffset )
{
	UINT iDestRange = 1;
	UINT iSrcRange = 1;

	// 0�� ������ ��� ���� ������
	D3D12_CPU_DESCRIPTOR_HANDLE hDescHandle = m_pInitDescriptor->GetCPUDescriptorHandleForHeapStart();
	hDescHandle.ptr += m_iCBVIncreSize * ( UINT )pCB->GetRegisterNum();

	D3D12_CPU_DESCRIPTOR_HANDLE hSrcHandle = pCB->GetCBV()->GetCPUDescriptorHandleForHeapStart();
	hSrcHandle.ptr += iOffset * m_iCBVIncreSize;

	m_pDevice->CopyDescriptors( 1, &hDescHandle, &iDestRange,
		1, &hSrcHandle, &iSrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
}

void CDevice::SetTextrueToRegister( CTexture * pTexture, TEXTURE_REGISTER eRegister )
{
	UINT iDestRange = 1;
	UINT iSrcRange = 1;

	// 0�� ������ ������� ������
	D3D12_CPU_DESCRIPTOR_HANDLE hDescHandle = m_vecDummyDescriptor[m_iCurDummyIdx]->GetCPUDescriptorHandleForHeapStart();
	hDescHandle.ptr += m_iCBVIncreSize * ( UINT )eRegister;

	D3D12_CPU_DESCRIPTOR_HANDLE hSrcHandle = pTexture->GetSRV()->GetCPUDescriptorHandleForHeapStart();

	m_pDevice->CopyDescriptors( 1, &hDescHandle, &iDestRange, 
		1, &hSrcHandle, &iSrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
}

void CDevice::ClearDummyDescriptorHeap( UINT iDummyIndex )
{
	D3D12_CPU_DESCRIPTOR_HANDLE hDescHandle = m_vecDummyDescriptor[iDummyIndex]->GetCPUDescriptorHandleForHeapStart();

	D3D12_CPU_DESCRIPTOR_HANDLE hSrcHandle = m_pInitDescriptor->GetCPUDescriptorHandleForHeapStart();

	UINT iDestRange = ( UINT )TEXTURE_REGISTER::END;
	UINT iSrcRange = ( UINT )TEXTURE_REGISTER::END;

	m_pDevice->CopyDescriptors( 1, &hDescHandle, &iDestRange,
		1, &hSrcHandle, &iSrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
}

void CDevice::UpdateTable()
{
	ID3D12DescriptorHeap* pDescriptor = m_vecDummyDescriptor[m_iCurDummyIdx].Get();
	m_pCmdListGraphic->SetDescriptorHeaps( 1, &pDescriptor );

	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = pDescriptor->GetGPUDescriptorHandleForHeapStart();
	m_pCmdListGraphic->SetGraphicsRootDescriptorTable( 0, gpuHandle );

	// ���� ���� Descriptor Heap�� ����Ű�� �ε����� ������Ų��.
	++m_iCurDummyIdx;

	// ���� ���� Descriptor Heap�� �ʱ�ȭ �Ѵ�. ( ���� ��� ���۴� ���´� )
	ClearDummyDescriptorHeap( m_iCurDummyIdx );
}

void CDevice::ExcuteResourceLoad()
{
	// ���ҽ� �ε� ��� �ݱ�
	m_pCmdListRes->Close();

	// Ŀ�ǵ� ����Ʈ ����
	ID3D12CommandList* ppCommandLists[] = { m_pCmdListRes.Get() };
	m_pCmdQueue->ExecuteCommandLists( _countof( ppCommandLists ), ppCommandLists );

	WaitForFenceEvent();

	// �ٽ� Ȱ��ȭ
	m_pCmdAllocRes->Reset();
	m_pCmdListRes->Reset( m_pCmdAllocRes.Get(), NULL );
}

void CDevice::CreateSwapChain()
{
	// ��� �����츦 �����Ͽ�, ȭ�� ���ſ����� ���
	DXGI_SWAP_CHAIN_DESC tDesc = {};

	tDesc.BufferCount = 2; // dx12 �� ���� ī��Ʈ�� 2�� �־��ش�

	tDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // ������ �ȼ� ����(�ȼ��� 4����Ʈ
	tDesc.BufferDesc.Width = ( UINT )m_tResolution.fWidth;  // ������ �ػ�(������ �ػ󵵶� ��ġ���ѳ���)
	tDesc.BufferDesc.Height = ( UINT )m_tResolution.fHeight;// ������ �ػ�(������ �ػ󵵶� ��ġ���ѳ���)
	tDesc.BufferDesc.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED; // ������� �ػ� ����ġ�� ȭ�� �����ϸ��� �Ұ�����
	tDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // �ȼ��� ��ĵ���� ����
	tDesc.BufferDesc.RefreshRate.Numerator = 100; // ȭ�� ���� ����
	tDesc.BufferDesc.RefreshRate.Denominator = 1;    // ȭ�� ���� ���� 

	tDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // ��� Ÿ�� �뵵�� ���۸� �����.
	tDesc.Flags = 0; // DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH

	tDesc.OutputWindow = m_hWnd;	// ��� ������
	tDesc.Windowed = m_bWindow;   // â ��� or ��üȭ�� ���
	tDesc.SampleDesc.Count = 1;		// ��Ƽ ���� ��� ����
	tDesc.SampleDesc.Quality = 0;
	tDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD; // ���� �ĸ� ���� ��ü �� ���� ������ ���� ����

	HRESULT hr = m_pFactory->CreateSwapChain( m_pCmdQueue.Get(), &tDesc, &m_pSwapChain );
}

void CDevice::CreateView()
{
	// RenderTargetViewHeap �� �޸� ������
	m_iRTVHeapSize = m_pDevice->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_RTV );

	// dx12 �������� ���� RenderTargetView ����
	// Descriptor Heap(������ ��) �� Dx11 �� RTV, DSV, UAV, SRV �� ���� ��ü

	// RenderTargetView �����	
	D3D12_DESCRIPTOR_HEAP_DESC tDesc = {};
	tDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	tDesc.NumDescriptors = 2;
	tDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	tDesc.NodeMask = 0;
	m_pDevice->CreateDescriptorHeap( &tDesc, IID_PPV_ARGS( &m_pRTV ) );

	D3D12_CPU_DESCRIPTOR_HANDLE hRTVHeap = m_pRTV->GetCPUDescriptorHandleForHeapStart();

	// Create a RTV for each frame.
	for ( UINT i = 0; i < 2; i++ )
	{
		// ������ SwapChain ���� ���۸� ������
		m_pSwapChain->GetBuffer( i, IID_PPV_ARGS( &m_RenderTargets[i] ) );

		// �ش� ���۷� RenderTarvetView ������
		m_pDevice->CreateRenderTargetView( m_RenderTargets[i].Get(), nullptr, hRTVHeap );
		hRTVHeap.ptr += m_iRTVHeapSize; // Offset ����
	}

	// DepthStencilView �����
	// Create a Texture2D
	D3D12_RESOURCE_DESC textureDesc{};
	textureDesc.MipLevels = 1;
	textureDesc.Format = DXGI_FORMAT_D32_FLOAT;
	textureDesc.Width = ( UINT )m_tResolution.fWidth;
	textureDesc.Height = ( UINT )m_tResolution.fHeight;
	textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	textureDesc.DepthOrArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

	//D3D12_CLEAR_VLAUE�� �� ���� �ʱ�ȭ�� �� �ִ� ����ü
	CD3DX12_CLEAR_VALUE depthOptimizedClearValue( DXGI_FORMAT_D32_FLOAT, 1.f, 0 );

	HRESULT hr = m_pDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_DEFAULT ),
		D3D12_HEAP_FLAG_NONE, &textureDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthOptimizedClearValue,
		IID_PPV_ARGS( &m_pDepthStencilTex ) );

	if ( FAILED( hr ) )
		assert( NULL );

	// DepthStencilView�� ������ DescriptorHeap ����
	tDesc = {};
	tDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	tDesc.NumDescriptors = 1;
	tDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	tDesc.NodeMask = 0;

	hr = m_pDevice->CreateDescriptorHeap( &tDesc, IID_PPV_ARGS( &m_pDSV ) );

	D3D12_CPU_DESCRIPTOR_HANDLE hDSVHandle = m_pDSV->GetCPUDescriptorHandleForHeapStart();
	m_pDevice->CreateDepthStencilView( m_pDepthStencilTex.Get(), NULL, hDSVHandle );
}

void CDevice::CreateViewPort()
{	
	// DirectX �� �׷��� ȭ�� ũ�⸦ �����Ѵ�.
	m_tVP = D3D12_VIEWPORT{ 0.f, 0.f, m_tResolution.fWidth, m_tResolution.fHeight, 0.f, 1.f };
	m_tScissorRect = D3D12_RECT{ 0, 0, ( LONG )m_tResolution.fWidth, ( LONG )m_tResolution.fHeight };
}

void CDevice::CreateRootSignature()
{
	m_iCBVIncreSize = m_pDevice->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );

	// ���� �� Descriptor Table�� �ۼ��Ѵ�.
	vector<D3D12_DESCRIPTOR_RANGE> vecRange;
	
	D3D12_ROOT_PARAMETER slotParam{};
	vecRange.clear();

	D3D12_DESCRIPTOR_RANGE range{};
	range.BaseShaderRegister = 0;
	range.NumDescriptors = 5;
	range.OffsetInDescriptorsFromTableStart = -1;
	range.RegisterSpace = 0;
	range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	vecRange.push_back( range );

	range = {};
	range.BaseShaderRegister = 0;
	range.BaseShaderRegister = 13;
	range.OffsetInDescriptorsFromTableStart = 5;
	range.RegisterSpace = 0;
	range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	vecRange.push_back( range );

	slotParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	slotParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	slotParam.DescriptorTable.NumDescriptorRanges = vecRange.size();
	slotParam.DescriptorTable.pDescriptorRanges = &vecRange[0];

	CreateSamplerDesc();

	// ��Ʈ ����
	D3D12_ROOT_SIGNATURE_DESC sigDesc = {};
	sigDesc.NumParameters = 1;
	sigDesc.pParameters = &slotParam;
	sigDesc.NumStaticSamplers = m_vecSamplerDesc.size();
	sigDesc.pStaticSamplers = &m_vecSamplerDesc[0];
	sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ComPtr<ID3DBlob> pSignature;
	ComPtr<ID3DBlob> pError;
	HRESULT hr = D3D12SerializeRootSignature( &sigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pSignature, &pError );
	m_pDevice->CreateRootSignature( 0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS( &m_arrSig[( UINT )ROOT_SIG_TYPE::INPUT_ASSEM] ) );

	// ���̿� Descriptor Heap�����
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc{};

	UINT iDescriptorNum = 0;
	for ( size_t i = 0; i < vecRange.size(); ++i )
	{
		iDescriptorNum += vecRange[i].NumDescriptors;
	}

	cbvHeapDesc.NumDescriptors = iDescriptorNum;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	for ( size_t i = 0; i < 512; ++i )
	{
		ComPtr<ID3D12DescriptorHeap> pDummyDescriptor;
		DEVICE->CreateDescriptorHeap( &cbvHeapDesc, IID_PPV_ARGS( &pDummyDescriptor ) );
		m_vecDummyDescriptor.push_back( pDummyDescriptor );
	}

	// �ʱ�ȭ�� Dummy Descriptor Heap 
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	m_pDevice->CreateDescriptorHeap( &cbvHeapDesc, IID_PPV_ARGS( &m_pInitDescriptor ) );
}

void CDevice::CreateSamplerDesc()
{
	D3D12_STATIC_SAMPLER_DESC sampler = {};
	sampler.Filter = D3D12_FILTER_ANISOTROPIC;
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;		// �ٵ���
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.MipLODBias = 0;
	sampler.MaxAnisotropy = 0;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	sampler.MinLOD = 0.0f;
	sampler.MaxLOD = D3D12_FLOAT32_MAX;
	sampler.ShaderRegister = 0;
	sampler.RegisterSpace = 0;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	m_vecSamplerDesc.push_back( sampler );

	sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;		// [0.0, 1.0] ������ ��� �ؽ�ó ��ǥ�� ���� 0.0 �Ǵ� 1.0�� �ؽ�ó �������� �����˴ϴ�.
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.MipLODBias = 0;
	sampler.MaxAnisotropy = 0;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	sampler.MinLOD = 0.0f;
	sampler.MaxLOD = D3D12_FLOAT32_MAX;
	sampler.ShaderRegister = 1;
	sampler.RegisterSpace = 0;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	m_vecSamplerDesc.push_back( sampler );
}

ComPtr<ID3D12GraphicsCommandList> CDevice::GetCmdList()
{
	return m_pCmdListGraphic;
}

ComPtr<ID3D12GraphicsCommandList> CDevice::GetCmdListRes()
{
	return m_pCmdListRes;
}

ComPtr<ID3D12Device> CDevice::GetDevice()
{
	return m_pDevice;
}

ComPtr<ID3D12RootSignature> CDevice::GetRootSignature( ROOT_SIG_TYPE eType )
{
	return m_arrSig[( UINT )eType];
}

CConstantBuffer * CDevice::GetConstBuffer( CONST_REGISTER eRegister )
{
	return m_vecCB[( UINT )eRegister];
}
