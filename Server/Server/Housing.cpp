#include "Housing.h"



CHousing::CHousing():
	m_eType(HOUSING_END),
	m_bInstall(false)

{
}


CHousing::~CHousing()
{
}

void CHousing::SetType(HOUSING_TYPE eType)
{
	unique_lock<shared_mutex> lock(m_smHousingSharedMutex);
	m_eType = eType;
}

void CHousing::SetInstall(bool bInstall)
{
	unique_lock<shared_mutex> lock(m_smHousingSharedMutex);
	m_bInstall = bInstall;
}

void CHousing::SetUpgrade()
{
	unique_lock<shared_mutex>	lock(m_smHousingSharedMutex);
	++m_uiGrade;
}

const HOUSING_TYPE & CHousing::GetType()
{
	shared_lock<shared_mutex> lock(m_smHousingSharedMutex);
	return m_eType;
}

const bool & CHousing::GetInstall()
{
	shared_lock<shared_mutex> lock(m_smHousingSharedMutex);
	return m_bInstall;
}

const int & CHousing::GetUpgrade()
{
	shared_lock<shared_mutex>	lock(m_smHousingSharedMutex);
	return m_uiGrade;
}
