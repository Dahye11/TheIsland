#pragma once
#include "stdafx.h"

#include "Object.h"
#include "Player.h"
#include "Monster.h"
#include "Housing.h"
#include "Natural.h"

class CObjectpool
{
public:
	CObjectpool();
	~CObjectpool();

private:
	mutex mu;

private:
	void Initalize();
	void Release();

private:
	void Init_PlayerPool();
	void Init_AnimalPool();
	void Init_NaturalPool();
	void Init_HousingPool();

	void Release_PlayerPool();
	void Release_AnimalPool();
	void Release_NaturalPool();
	void Release_HousingPool();

public:
	template<typename T>
	bool CollisionHouse(std::pair<const USHORT, T*> pFirst, CHousing* pSecond) {
		Vec3 vPos1 = pFirst.second->GetLocalPos();
		Vec3 vPos2 = pSecond->GetLocalPos();

		Vec3 vScale1 = pFirst.second->GetLocalScale();
		Vec3 vScale2 = pSecond->GetLocalScale();

		Vec3 vColScale1 = pFirst.second->GetOffsetScale();
		Vec3 vColScale2 = Vec3(1.f, 1.f, 1.f);

		Vec3 vHouseOffsetScale = pSecond->GetOffsetScale();

		char eType = pSecond->GetType();

		switch (eType)
		{
		case HOUSING_FOUNDATION:
			vColScale2 = vHouseOffsetScale;
			break;
		case HOUSING_WALL:
		case HOUSING_WINDOW:
		case HOUSING_DOOR:
		{
			Vec3 vRot = pSecond->GetLocalRot();
			if (vRot.y != 0.f)
			{
				vColScale2 = Vec3(vHouseOffsetScale.z, vHouseOffsetScale.y, vHouseOffsetScale.x);
			}
			else
			{
				vColScale2 = Vec3(vHouseOffsetScale.x, vHouseOffsetScale.y, vHouseOffsetScale.z);
			}
		}
		break;
		case HOUSING_FLOOR:
		{
			vColScale2 = vHouseOffsetScale;
		}
		break;
		default:
			vColScale2 = vHouseOffsetScale;
			break;
		}

		vScale1 *= vColScale1;
		vScale2 *= vColScale2;

		Vec3 vMax, vOtherMax;
		Vec3 vMin, vOtherMin;

		Vec3 vPos = pFirst.second->GetLocalPos();
		vPos.y += 50.f;

		vMax = vPos + vScale1;
		vMin = vPos - vScale1;
		vOtherMax = vPos1 + vScale2;
		vOtherMin = vPos2 - vScale2;

		if (vMax.x < vOtherMin.x || vMin.x > vOtherMax.x) return false;
		if (vMax.y < vOtherMin.y || vMin.y > vOtherMax.y) return false;
		if (vMax.z < vOtherMin.z || vMin.z > vOtherMax.z) return false;

		return true;
	}
public:
	static concurrent_unordered_map<USHORT, CPlayer*> m_cumPlayerPool;
	static concurrent_unordered_map<USHORT, CMonster*> m_cumAnimalPool;
	static concurrent_unordered_map<USHORT, CNatural*> m_cumNaturalPool;
	static concurrent_unordered_map<USHORT, CHousing*> m_cumHousingPool;

	USHORT GetLoginID();
	bool Check_Install_House(CHousing* pHouse);
	void Install_House(CHousing* pHouse, USHORT usIndex);
};