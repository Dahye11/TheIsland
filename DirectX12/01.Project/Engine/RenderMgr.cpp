#include "stdafx.h"
#include "RenderMgr.h"

#include "Device.h"
#include "Camera.h"

#include "SceneMgr.h"
#include "ConstantBuffer.h"

#include "TimeMgr.h"
#include "EventMgr.h"

#include "Light3D.h"

#include "MRT.h"

CRenderMgr::CRenderMgr()
	: m_iRTVHeapSize(0)
	, m_arrMRT{}		
{	
}

CRenderMgr::~CRenderMgr()
{	
	Safe_Delete_Array( m_arrMRT );
}

void CRenderMgr::Render()
{
	// �ʱ�ȭ
	float arrColor[4] = { 0.f,0.f, 0.f, 1.f };
	CDevice::GetInst()->Render_Start(arrColor);

	// ���� ���� ������Ʈ
	UpdateLight2D();
	UpdateLight3D();

	// SwapChain MRT �ʱ�ȭ
	UINT iIdx = CDevice::GetInst()->GetSwapchainIdx();
	m_arrMRT[( UINT )MRT_TYPE::SWAPCHAIN]->Clear( iIdx );

	// DeferredMRT �ʱ�ȭ
	m_arrMRT[( UINT )MRT_TYPE::DEFERRED]->Clear();

	// ī�޶� �ݺ��� ���鼭
	for (size_t i = 0; i < m_vecCam.size(); ++i)
	{
		// ��ü �з� �۾� forward�� ����Ұ����� deffered�� ����� ������
		m_vecCam[i]->SortGameObject();

		// Deferred MRT ����
		m_arrMRT[( UINT )MRT_TYPE::DEFERRED]->OMSet();
		m_vecCam[i]->Render_Deferred();

		// Light MRT ����

		// SwapChain MRT ����		
		m_arrMRT[( UINT )MRT_TYPE::SWAPCHAIN]->OMSet( 1, iIdx );
		m_vecCam[i]->Render_Forward();
		// Ÿ���� SwapChain���� �ٲ����� ī�޶� Render�� Forward�� �����ؼ� Rendering

		// Merge (Diffuse Target, Diffuse Light Target, Specular Light Target)
	}	

	// ���
	CDevice::GetInst()->Render_present();
}

void CRenderMgr::Render_tool()
{
	// �ʱ�ȭ
	float arrColor[4] = { 0.f, 0.f, 0.f, 1.f };
	//Clear(arrColor);

	// ���� ���� ������Ʈ
	UpdateLight2D();
	UpdateLight3D();
}

void CRenderMgr::UpdateLight2D()
{
	static CConstantBuffer* pLight2DBuffer = CDevice::GetInst()->GetCB(CONST_REGISTER::b3);

	UINT iOffsetPos = pLight2DBuffer->AddData(&m_tLight2DInfo);
	CDevice::GetInst()->SetConstBufferToRegister(pLight2DBuffer, iOffsetPos);

	m_tLight2DInfo.iCount = 0;
}

void CRenderMgr::UpdateLight3D()
{
	static CConstantBuffer* pLight3DBuffer = CDevice::GetInst()->GetCB(CONST_REGISTER::b4);
	
	tLight3DInfo tLight3DArray;

	for (size_t i = 0; i < m_vecLight3D.size(); ++i)
	{
		const tLight3D& info = m_vecLight3D[i]->GetLight3DInfo();
		tLight3DArray.arrLight3D[i] = info;
	}
	tLight3DArray.iCurCount = (UINT)m_vecLight3D.size();
	
	UINT iOffsetPos = pLight3DBuffer->AddData(&tLight3DArray);
	CDevice::GetInst()->SetConstBufferToRegister(pLight3DBuffer, iOffsetPos);
	
	m_vecLight3D.clear();
}

CMRT * CRenderMgr::GetMRT( MRT_TYPE eType )
{
	return m_arrMRT[(UINT)eType];
}

UINT CRenderMgr::GetRTVHeapSize()
{
	return m_iRTVHeapSize;
}
