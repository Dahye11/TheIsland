#pragma once
#include <Engine/Script.h>

class CPlayerCamScript :
	public CScript
{
	CGameObject* m_pPlayer;
public:
	CPlayerCamScript();
	virtual ~CPlayerCamScript();

	CLONE(CPlayerCamScript);

	// CScript��(��) ���� ��ӵ�
	virtual void Update() override;

public:
	void SetPlayer(CGameObject* _pPlayer) {
		m_pPlayer = _pPlayer;
	}
};

