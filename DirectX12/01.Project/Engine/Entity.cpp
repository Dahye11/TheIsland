#include "stdafx.h"
#include "Entity.h"


CEntity::CEntity()
{
}


CEntity::~CEntity()
{
}

void CEntity::SetName( wstring strName )
{
	m_strName = strName;
}

const wstring & CEntity::GetName()
{
	// TODO: ���⿡ ��ȯ ������ �����մϴ�.
	return m_strName;
}
