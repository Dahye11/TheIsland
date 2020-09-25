#pragma once
#include "stdafx.h"

class CObject
{
private:
	char	m_eState;
	Vec3	m_vLocalPos;
	Vec3	m_vLocalScale;
	Vec3	m_vLocalRot;
	Vec3	m_vLocalDir[(UINT)DIR_TYPE::END];

	// Collider
	Vec3	m_vOffsetPos;
	Vec3	m_vOffsetScale;

	shared_mutex m_ObjMutex;

public:
	explicit CObject();
	virtual ~CObject();

public:
	void SetLocalPos(float& fPosX, float& fPosY, float& fPosZ);
	void SetLocalPos(Vec3 vPos);
	void SetLocalRot(float& fRotX, float& fRotY, float& fRotZ);
	void SetLocalRot(Vec3 vRot);
	void SetLocalDir(DIR_TYPE uiDir, Vec3 vDir);
	void SetLocalScale(float& fScaleX, float& fScaleY, float& fScaleZ);
	void SetLocalScale(Vec3 vScale);
	void SetState(char eState);
	void SetOffsetPos(Vec3 vPos);
	void SetOffsetScale(Vec3 vScale);

public:
	const Vec3&	GetLocalPos();
	const Vec3&	GetLocalScale();
	const Vec3&	GetLocalRot();
	const Vec3& GetLocalDir(DIR_TYPE _eType);
	const char	GetState();

	const Vec3&	GetOffsetPos();
	const Vec3&	GetOffsetScale();
};