#pragma once

#include "Entity.h"

class CConstantBuffer : 
	public CEntity
{
private:
	friend class CDevice;

private:
	CConstantBuffer();
	virtual ~CConstantBuffer();

private:
	ComPtr<ID3D12Resource>			m_pBuffer;			// ���� �������(���ҽ�)
	ComPtr<ID3D12DescriptorHeap>	m_pCBV;				// ������� ������ ��(View ����)
	D3D12_CPU_DESCRIPTOR_HANDLE		m_hCBV;				// ������ �� �ڵ�
	UINT							m_iBufferSize;		// ������� ���� ũ��
	UINT							m_iMaxCount;		// ������� �ִ� ����
	UINT							m_iCurCount;		// ������ۿ� Add�� ���� ������ ����
	CONST_REGISTER					m_eRegisterNum;		// ������� ��� �������� ��ȣ
	BYTE*							m_pData;			// ������ۿ� ���ε� �ּ�

	D3D12_DESCRIPTOR_HEAP_DESC		m_tHeapDesc;		// ������ۿ� ������ ���� �����ϴ� ����

public:
	void Create( UINT iBufferSize, UINT iMaxCount, CONST_REGISTER eRegister );
	void SetData( void* pSrc );
	void SetData( void* pSrc, UINT iSize );
	void SetData( void* pSrc, UINT iSize, UINT iOffserPos );
	UINT AddData( void* pSrc );
	void Clear();

	ComPtr<ID3D12DescriptorHeap> GetCBV();
	D3D12_CPU_DESCRIPTOR_HANDLE GetHandle();
	CONST_REGISTER	GetRegisterNum();
};

