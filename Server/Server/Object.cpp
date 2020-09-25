#include "Object.h"

CObject::CObject() :
	m_vLocalPos(Vec3(0.f, 0.f, 0.f)),
	m_vLocalRot(Vec3(0.f, 0.f, 0.f)),
	m_vLocalScale(Vec3(1.f, 1.f, 1.f)),
	m_vOffsetScale(Vec3(150.f, 150.f, 150.f)),
	m_eState(OBJ_STATE_TYPE::OST_LIVE)
{
}

CObject::~CObject()
{
}

void CObject::SetLocalPos(float& fPosX, float& fPosY, float& fPosZ)
{
	unique_lock<shared_mutex>lock(m_ObjMutex);
	m_vLocalPos = Vec3(fPosX, fPosY, fPosZ);
}

void CObject::SetLocalPos(Vec3 vPos)
{
	unique_lock<shared_mutex>lock(m_ObjMutex);
	m_vLocalPos = vPos;
}

void CObject::SetLocalRot(float& fRotX, float& fRotY, float& fRotZ)
{
	unique_lock<shared_mutex>lock(m_ObjMutex);
	m_vLocalRot = Vec3(fRotX, fRotY, fRotZ);
}

void CObject::SetLocalRot(Vec3 vRot)
{
	unique_lock<shared_mutex>lock(m_ObjMutex);
	m_vLocalRot = vRot;
}

void CObject::SetLocalDir(DIR_TYPE uiDir, Vec3 vDir)
{
	unique_lock<shared_mutex> lock(m_ObjMutex);
	m_vLocalDir[(UINT)uiDir] = vDir;
}

void CObject::SetLocalScale(float & fScaleX, float & fScaleY, float & fScaleZ)
{
	unique_lock<shared_mutex>lock(m_ObjMutex);
	m_vLocalScale = Vec3(fScaleX, fScaleY, fScaleZ);
}

void CObject::SetLocalScale(Vec3 vScale)
{
	unique_lock<shared_mutex>lock(m_ObjMutex);
	m_vLocalScale = vScale;
}

void CObject::SetState(char uiState)
{
	unique_lock<shared_mutex>lock(m_ObjMutex);
	m_eState = uiState;
}

void CObject::SetOffsetPos(Vec3 vPos)
{
	unique_lock<shared_mutex> lock(m_ObjMutex);
	m_vOffsetPos = vPos;
}

void CObject::SetOffsetScale(Vec3 vScale)
{
	unique_lock<shared_mutex> lock(m_ObjMutex);
	m_vOffsetScale = vScale;
}

const Vec3& CObject::GetLocalPos()
{
	shared_lock<shared_mutex>lock(m_ObjMutex);
	return m_vLocalPos;
}

const Vec3& CObject::GetLocalRot()
{
	shared_lock<shared_mutex>lock(m_ObjMutex);
	return m_vLocalRot;
}

const Vec3 & CObject::GetLocalDir(DIR_TYPE _eType)
{
	shared_lock<shared_mutex> lock(m_ObjMutex);
	return m_vLocalDir[(UINT)_eType];
	// TODO: 여기에 반환 구문을 삽입합니다.
}

const Vec3& CObject::GetLocalScale()
{ 
	shared_lock<shared_mutex> lock(m_ObjMutex);
	return m_vLocalScale;
}

const char CObject::GetState()
{
	shared_lock<shared_mutex>lock(m_ObjMutex);
	return m_eState;
}

const Vec3& CObject::GetOffsetPos()
{
	shared_lock<shared_mutex>lock(m_ObjMutex);
	return m_vOffsetPos;
}

const Vec3& CObject::GetOffsetScale()
{
	shared_lock<shared_mutex>lock(m_ObjMutex);
	return m_vOffsetScale;
}
