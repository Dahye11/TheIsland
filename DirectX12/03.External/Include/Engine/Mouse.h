#pragma once
#include "Component.h"

class CMouse :
	public CComponent
{
public:
	CMouse();
	~CMouse();
	CLONE( CMouse );

public:
	virtual void Update();

public:
	// CComponent��(��) ���� ��ӵ�
	virtual void SaveToScene( FILE * _pFile ) override;
	virtual void LoadFromScene( FILE * _pFile ) override;
};

