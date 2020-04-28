#pragma once
#include "Entity.h"

// ������ �����͸� ���ϴ� ���·� ���谡���� ����
// SRV�� UAV�� ���ÿ� ��������

class CStructuredBuffer :
	public CEntity
{
public:
	CStructuredBuffer();
	virtual	~CStructuredBuffer();

private:
	ComPtr<ID3D12Resource>			m_pBuffer;
	ComPtr<ID3D12DescriptorHeap>	m_pSRV;
	ComPtr<ID3D12DescriptorHeap>	m_pUAV;

	UINT							m_iElementSize;
	UINT							m_iElementCount;

	D3D12_RESOURCE_STATES			m_eResState;

public:
	void Create( UINT iElementSize, UINT iElementCount, void* pSysMem );

	void UpdateData( TEXTURE_REGISTER iRegister );		// rendering �ܰ�
	void UpdateData_CS( TEXTURE_REGISTER iRegister );	// computeshader �ܰ�
	void UpdateRWData( UAV_REGISTER iRegister );		// uav�� computershader �ܰ迡�� �ۿ�

	ComPtr<ID3D12DescriptorHeap> GetSRV();
	ComPtr<ID3D12DescriptorHeap> GetUAV();

	void SetResState( D3D12_RESOURCE_STATES eState );
	D3D12_RESOURCE_STATES GetResState();
	ComPtr<ID3D12Resource> GetBuffer();

	UINT GetElementSize();
	UINT GetElementCount();
	UINT GetBufferSize();
};

