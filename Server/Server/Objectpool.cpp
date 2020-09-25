#include "Objectpool.h"

concurrent_unordered_map<USHORT, CPlayer*> CObjectpool::m_cumPlayerPool;
concurrent_unordered_map<USHORT, CMonster*> CObjectpool::m_cumAnimalPool;
concurrent_unordered_map<USHORT, CNatural*> CObjectpool::m_cumNaturalPool;
concurrent_unordered_map<USHORT, CHousing*> CObjectpool::m_cumHousingPool;

CObjectpool::CObjectpool()
{
	Initalize();
}


CObjectpool::~CObjectpool()
{
	Release();
}

void CObjectpool::Initalize()
{
	Init_PlayerPool();
	Init_AnimalPool();
	Init_NaturalPool();
	Init_HousingPool();
}

void CObjectpool::Release()
{
	Release_PlayerPool();
	Release_AnimalPool();
	Release_NaturalPool();
	Release_HousingPool();
}

void CObjectpool::Init_PlayerPool()
{
	for (int i = 0; i < MAX_USER; ++i)
		m_cumPlayerPool.insert(make_pair(i, new class CPlayer()));

	cout << "Player POOL Create" << endl;
}

void CObjectpool::Init_AnimalPool()
{
	Evastion_Pawner = Vec3(10000.f, 0.f, 10000.f);
	Warlike_Pawner = Vec3(8100.f, 0.f, 16200.f);
	Passive_Pawner = Vec3(6900.f, 0.f, 5285.f);

	float fRadius;

	for (int i = BEGIN_ANIMAL; i < END_ANIMAL; ++i)
	{
		CMonster* Animal = new class CMonster();
		Animal->SetWakeUp(false);
		Animal->SetState(OBJ_STATE_TYPE::OST_LIVE);
		if (i < BEGIN_ANIMAL + ANIMAL_BEAR)
		{
			tAnimalStatus tStatus;
			tStatus.fHealth = 200.f;
			tStatus.fStamina = 100.f;
			tStatus.fDamage = 20.f;
			tStatus.fSpeed = 150.f;
			tStatus.eType = BEHAVIOR_TYPE::B_WARLIKE;
			tStatus.eKind = ANIMAL_TYPE::A_BEAR;

			Animal->SetAnimalStatus(tStatus);

			Vec3 vOffsetScale = Vec3(2.f, 2.f, 2.f);
			Animal->SetOffsetScale(vOffsetScale);

			Animal->SetLocalRot(Vec3(-3.141592654f / 2.f, 0.f, 0.f));
			Animal->SetLocalScale(Vec3(1.f, 1.f, 1.f));

			fRadius = 2000.f;

			float fDistance = (float)(rand() % (int)fRadius + 1);
			float fDegree = (float)(rand() % 360);
			float fRadian = fDegree / 100.f * 3.141592654f;

			Vec3 vPos = Warlike_Pawner;
			vPos.x += cos(fRadian) * fDistance;
			vPos.z += sin(fRadian) * fDistance;
#ifdef ANIMAL_TEST

#else
			Animal->SetLocalPos(vPos);
#endif // ANIMAL_TEST
		}
		else if (i < BEGIN_ANIMAL + ANIMAL_BEAR + ANIMAL_BOAR)
		{
			tAnimalStatus tStatus;
			tStatus.fHealth = 200.f;
			tStatus.fStamina = 100.f;
			tStatus.fDamage = 20.f;
			tStatus.fSpeed = 150.f;
			tStatus.eType = BEHAVIOR_TYPE::B_PASSIVE;
			tStatus.eKind = ANIMAL_TYPE::A_BOAR;

			Animal->SetAnimalStatus(tStatus);

			Vec3 vOffsetScale = Vec3(60.f, 60.f, 60.f);
			Animal->SetOffsetScale(vOffsetScale);

			Animal->SetLocalRot(Vec3(-3.141592654f / 2.f, 0.f, 0.f));
			Animal->SetLocalScale(Vec3(1.f, 1.f, 1.f));

			fRadius = 2000.f;

			float fDistance = (float)(rand() % (int)fRadius + 1);
			float fDegree = (float)(rand() % 360);
			float fRadian = fDegree / 100.f * 3.141592654f;

			Vec3 vPos = Warlike_Pawner;
			vPos.x += cos(fRadian) * fDistance;
			vPos.z += sin(fRadian) * fDistance;

#ifdef ANIMAL_TEST

#else
			Animal->SetLocalPos(vPos);
#endif // ANIMAL_TEST
		}
		else if (i < BEGIN_ANIMAL + ANIMAL_BEAR + ANIMAL_BOAR + ANIMAL_DEER)
		{
			tAnimalStatus tStatus;
			tStatus.fHealth = 100.f;
			tStatus.fStamina = 100.f;
			tStatus.fDamage = 0.f;
			tStatus.fSpeed = 250.f;
			tStatus.eType = BEHAVIOR_TYPE::B_EVASION;
			tStatus.eKind = ANIMAL_TYPE::A_DEER;

			Animal->SetAnimalStatus(tStatus);

			Vec3 vOffsetScale = Vec3(30.f, 30.f, 30.f);
			Animal->SetOffsetScale(vOffsetScale);

			fRadius = 8000.f;

			float fDistance = (float)(rand() % (int)fRadius + 1);
			float fDegree = (float)(rand() % 360);
			float fRadian = fDegree / 100.f * 3.141592654f;

			Vec3 vPos = Evastion_Pawner;
			vPos.x += cos(fRadian) * fDistance;
			vPos.z += sin(fRadian) * fDistance;

			Animal->SetLocalPos(vPos);
		}
		else if (i < END_ANIMAL)
		{
			tAnimalStatus tStatus;
			tStatus.fHealth = 200.f;
			tStatus.fStamina = 100.f;
			tStatus.fDamage = 20.f;
			tStatus.fSpeed = 200.f;
			tStatus.eType = BEHAVIOR_TYPE::B_PASSIVE;
			tStatus.eKind = ANIMAL_TYPE::A_WOLF;

			Animal->SetAnimalStatus(tStatus);

			Vec3 vOffsetScale = Vec3(2.f, 2.f, 2.f);
			Animal->SetOffsetScale(vOffsetScale);

			fRadius = 2000.f;

			float fDistance = (float)(rand() % (int)fRadius + 1);
			float fDegree = (float)(rand() % 360);
			float fRadian = fDegree / 100.f * 3.141592654f;

			Vec3 vPos = Passive_Pawner;
			vPos.x += cos(fRadian) * fDistance;
			vPos.z += sin(fRadian) * fDistance;

#ifdef ANIMAL_TEST
#else
			Animal->SetLocalPos(vPos);
#endif // ANIMAL_TEST
		}
#ifdef ANIMAL_TEST
		Animal->SetLocalPos(Vec3(16000.f, 200.f, 2000.f));
#else

#endif // ANIMAL_TEST
		m_cumAnimalPool.insert(make_pair(i, Animal));
	}
	cout << "Aniaml : " << END_ANIMAL - BEGIN_ANIMAL << endl;
}

void CObjectpool::Init_NaturalPool()
{
	for (int i = BEGIN_NATURAL; i < BEGIN_NATURAL + MAX_NATURAL; ++i) {
		m_cumNaturalPool.insert(make_pair(i, new CNatural()));
	}
	FILE* pFile = NULL;

	wstring ResPath = L"..\\Data\\Map.dat";
	string FullPath{ ResPath.begin(), ResPath.end() };

	fopen_s(&pFile, FullPath.c_str(), "r");

	int iSize = 0;
	fread(&iSize, sizeof(int), 1, pFile);

	for (int i = BEGIN_NATURAL; i < BEGIN_NATURAL + iSize; ++i)
	{
		wchar_t strName[MAX_PATH]{};
		size_t iLength = 0;
		fread(&iLength, sizeof(size_t), 1, pFile);
		fread(strName, sizeof(wchar_t), iLength, pFile);

		bool bNaturalScript;
		fread(&bNaturalScript, sizeof(bool), 1, pFile);
		if (bNaturalScript)
		{
			wchar_t strPath[33]{};
			fread(&iLength, sizeof(size_t), 1, pFile);
			fread(strPath, sizeof(wchar_t), iLength, pFile);

			if (iLength == 0)
				continue;

			NATURAL_TYPE eType;
			fread(&eType, sizeof(NATURAL_TYPE), 1, pFile);
			m_cumNaturalPool[i]->SetType(eType);
			m_cumNaturalPool[i]->SetOffsetPos(Vec3(0.f, 0.f, 60.f));
			m_cumNaturalPool[i]->SetOffsetScale(Vec3(1.7f, 1.7f, 1.7f));
		}

		Vec3 vLocalPos;
		Vec3 vLocalScale;
		Vec3 vLocalRot;
		fread(&vLocalPos, sizeof(Vec3), 1, pFile);
		m_cumNaturalPool[i]->SetLocalPos(vLocalPos);
		fread(&vLocalScale, sizeof(Vec3), 1, pFile);
		m_cumNaturalPool[i]->SetLocalScale(vLocalScale);
		fread(&vLocalRot, sizeof(Vec3), 1, pFile);
		m_cumNaturalPool[i]->SetLocalRot(vLocalRot);

		string str1;

		if (str1 == "sprucea" || str1 == "sprucec")
		{
		}
		else if (str1 == "spruceb")
		{
		}
		else if (str1 == "plainsgrass")
		{
			m_cumNaturalPool[i]->SetOffsetPos(Vec3(0.f, 0.f, 20.f));
			m_cumNaturalPool[i]->SetOffsetScale(Vec3(100.f, 100.f, 100.f));
		}
		else if (str1 == "mountainsrocks01")
		{
			m_cumNaturalPool[i]->SetOffsetPos(Vec3(20.f, 20.f, -40.f));
			m_cumNaturalPool[i]->SetOffsetScale(Vec3(350.f, 350.f, 350.f));
		}
		else if (str1 == "mountainsrocks02")
		{
			m_cumNaturalPool[i]->SetOffsetPos(Vec3(20.f, 0.f, 0.f));
			m_cumNaturalPool[i]->SetOffsetScale(Vec3(220.f, 220.f, 220.f));
		}
		else if (str1 == "genericcliffa")
		{
			m_cumNaturalPool[i]->SetOffsetPos(Vec3(0.f, 0.f, 20.f));
			m_cumNaturalPool[i]->SetOffsetScale(Vec3(10.f, 10.f, 10.f));
		}
		else if (str1 == "mountainsrocks01_a")
		{
			m_cumNaturalPool[i]->SetOffsetPos(Vec3(0.f, 0.f, 0.f));
			m_cumNaturalPool[i]->SetOffsetScale(Vec3(200.f, 200.f, 200.f));
		}
		else if (str1 == "mountainsrocks01_b")
		{
			m_cumNaturalPool[i]->SetOffsetPos(Vec3(0.f, 0.f, -60.f));
			m_cumNaturalPool[i]->SetOffsetScale(Vec3(220.f, 220.f, 220.f));
		}
		else if (str1 == "mountainsrocks01_c")
		{
			m_cumNaturalPool[i]->SetOffsetPos(Vec3(0.f, 0.f, 0.f));
			m_cumNaturalPool[i]->SetOffsetScale(Vec3(100.f, 100.f, 100.f));
		}
		else if (str1 == "mountainsrocks01_d")
		{
			m_cumNaturalPool[i]->SetOffsetPos(Vec3(0.f, 0.f, 0.f));
			m_cumNaturalPool[i]->SetOffsetScale(Vec3(100.f, 100.f, 100.f));
		}
		else
		{
			m_cumNaturalPool[i]->SetOffsetPos(Vec3(0.f, 0.f, 20.f));
			m_cumNaturalPool[i]->SetOffsetScale(Vec3(4.f, 4.f, 4.f));
		}
	}
	fclose(pFile);
	cout << "Natural : " << MAX_NATURAL << endl;
}

void CObjectpool::Init_HousingPool()
{
}

void CObjectpool::Release_PlayerPool()
{
	for (auto& au : m_cumPlayerPool)
	{
		if (au.second) {
			delete au.second;
			au.second = nullptr;
		}
	}
	m_cumPlayerPool.clear();
}

void CObjectpool::Release_AnimalPool()
{
	for (auto& au : m_cumAnimalPool)
	{
		if (au.second) {
			delete au.second;
			au.second = nullptr;
		}
	}
	m_cumAnimalPool.clear();
}

void CObjectpool::Release_NaturalPool()
{
	for (auto& au : m_cumNaturalPool)
	{
		if (au.second)
		{
			delete au.second;
			au.second = nullptr;
		}
	}
	m_cumNaturalPool.clear();
}

void CObjectpool::Release_HousingPool()
{
	for (auto& au : m_cumHousingPool)
	{
		if (au.second)
		{
			delete au.second;
			au.second = nullptr;
		}
	}
	m_cumHousingPool.clear();
}

USHORT CObjectpool::GetLoginID()
{
	mu.lock();
	for (auto& User : m_cumPlayerPool)
	{
		bool bConnect = User.second->GetConnect();
		if (bConnect) continue;
		User.second->SetConnect(true);
		mu.unlock();
		return User.first;
	}
	mu.unlock();
	return MAX_USER;
}

bool CObjectpool::Check_Install_House(CHousing * pHouse)
{
	CHousing* phouse = pHouse;
	/*for (auto& house : m_cumHousingPool) {
		if (CollisionHouse(house, phouse))
		{
			return false;
		}
	}*/
	/*for (auto& natural : m_cumNaturalPool) {
		if (CollisionHouse(natural, phouse)) {
			if (natural.second->GetType() == N_BUSH)
				continue;
			return false;
		}
	}*/
	/*for (auto& player : m_cumPlayerPool) {
		if (CollisionHouse(player, phouse)) {
			if (!player.second->GetConnect())
				continue;
			return false;
		}
	}*/
	return true;
}

void CObjectpool::Install_House(CHousing * pHouse, USHORT usIndex)
{
	m_cumHousingPool.insert(make_pair(usIndex, pHouse));
}

void CObjectpool::Remove_House(USHORT usIndex)
{
	if (m_cumHousingPool.count(usIndex))
		m_cumHousingPool.unsafe_erase(usIndex); // lock ÇÊ¿ä
}

bool CObjectpool::Upgrade_House(USHORT usIndex)
{
	if (m_cumHousingPool.count(usIndex)) {
		UINT uiGrade = m_cumHousingPool[usIndex]->GetUpgrade();
		if (uiGrade >= MAX_GRADE - 1)
			return false;
		m_cumHousingPool[usIndex]->SetUpgrade();
		return true;
	}
	else
		return false;
}

void CObjectpool::Init_Animal(USHORT usIndex)
{
	auto& Animal = m_cumAnimalPool[usIndex];
	Animal->SetWakeUp(false);
	Animal->SetState(OBJ_STATE_TYPE::OST_LIVE);
	Animal->SetExit(false);
	Animal->SetHit(false);
	Animal->SetFirstAttack(true);

	char eType = Animal->GetKind();

	switch (eType)
	{
	case ANIMAL_TYPE::A_BEAR:
	{
		{
			tAnimalStatus tStatus;
			tStatus.fHealth = 200.f;
			tStatus.fStamina = 100.f;
			tStatus.fDamage = 20.f;
			tStatus.fSpeed = 150.f;
			tStatus.eType = BEHAVIOR_TYPE::B_WARLIKE;
			tStatus.eKind = ANIMAL_TYPE::A_BEAR;

			Animal->SetAnimalStatus(tStatus);
		}
	}
		break;
	case ANIMAL_TYPE::A_BOAR:
	{
		{
			tAnimalStatus tStatus;
			tStatus.fHealth = 200.f;
			tStatus.fStamina = 100.f;
			tStatus.fDamage = 20.f;
			tStatus.fSpeed = 150.f;
			tStatus.eType = BEHAVIOR_TYPE::B_PASSIVE;
			tStatus.eKind = ANIMAL_TYPE::A_BOAR;

			Animal->SetAnimalStatus(tStatus);
		}
	}
		break;
	case ANIMAL_TYPE::A_DEER:
	{
		tAnimalStatus tStatus;
		tStatus.fHealth = 100.f;
		tStatus.fStamina = 100.f;
		tStatus.fDamage = 0.f;
		tStatus.fSpeed = 250.f;
		tStatus.eType = BEHAVIOR_TYPE::B_EVASION;
		tStatus.eKind = ANIMAL_TYPE::A_DEER;

		Animal->SetAnimalStatus(tStatus);
	}
		break;
	case ANIMAL_TYPE::A_WOLF:
	{
		tAnimalStatus tStatus;
		tStatus.fHealth = 200.f;
		tStatus.fStamina = 100.f;
		tStatus.fDamage = 20.f;
		tStatus.fSpeed = 200.f;
		tStatus.eType = BEHAVIOR_TYPE::B_PASSIVE;
		tStatus.eKind = ANIMAL_TYPE::A_WOLF;

		Animal->SetAnimalStatus(tStatus);
	}
		break;
	}
}

void CObjectpool::Animal_Collision(USHORT usIndex)
{
	auto&	Animal = m_cumAnimalPool[usIndex];

	if (!Animal->GetWakeUp()) return;
	char eState = Animal->GetState();
	if (eState == OST_DIE || eState == OST_REMOVE) return;

	// NATURAL COLLISION
	for (auto& natural : m_cumNaturalPool) {
		char eType = natural.second->GetType();

		if (NATURAL_TYPE::N_BUSH == eType)
			continue;

		if (CollisionSphere(Animal, natural, 0.2f)) {
			Vec3 vNaturalPos = natural.second->GetLocalPos();
			Vec3 vAnimalPos = Animal->GetLocalPos();
			Vec3 vDir = XMVector3Normalize(vAnimalPos - vNaturalPos);
			float fSpeed = Animal->GetSpeed();
			vDir.y = 0.f;
			vAnimalPos += vDir * fSpeed * 0.1f;
			Animal->SetLocalPos(vAnimalPos);
		}
	}
	for (auto& animal : m_cumAnimalPool) {
		if (animal.first == usIndex) continue;
		char eState = animal.second->GetState();
		if (eState == OST_DIE || eState == OST_REMOVE) continue;

		if (CollisionSphere(Animal, animal, 30.f)) {
			Vec3 OtherPos = animal.second->GetLocalPos();
			Vec3 AnimalPos = Animal->GetLocalPos();

			Vec3 vDir = XMVector3Normalize(OtherPos - AnimalPos);
			float fSpeed = Animal->GetSpeed();
			vDir.y = 0;
			AnimalPos -= vDir * fSpeed * 0.1f;
			Animal->SetLocalPos(AnimalPos);
		}
	}
}

void CObjectpool::Player_Collision(USHORT usIndex)
{
	auto& User = m_cumPlayerPool[usIndex];

	//// NATURAL COLLISION
	//for (auto& natural : m_cumNaturalPool) {
	//	char eType = natural.second->GetType();
	//	if (NATURAL_TYPE::N_BUSH == eType) {
	//		continue;
	//	}
	//	bool bDestroy = natural.second->GetDestroy();
	//	if (bDestroy) continue;

	//	if (CollisionSphere(User, natural)) {
	//		Vec3 vNaturalPos = natural.second->GetLocalPos();
	//		Vec3 vPlayerPos = User->GetLocalPos();
	//	}
	//}
}
