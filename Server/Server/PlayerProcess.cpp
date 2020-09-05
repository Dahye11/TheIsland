#include "stdafx.h"
#include "PlayerProcess.h"
#include "Network.h"
#include "PacketMgr.h"
#include "Player.h"
#include "DataBase.h"
#include "TimerMgr.h"

CPlayerProcess::CPlayerProcess()
{
	BindPacketProcess();
}


CPlayerProcess::~CPlayerProcess()
{
}

void CPlayerProcess::Init_Player(USHORT playerId, char* wcId)
{
	//m_pObjectPool->m_cumPlayerPool[playerId]->SetState();
	{
		tPlayerStatus tStatus;
		tStatus.fHealth = 100.f;
		tStatus.fHungry = 100.f;
		tStatus.fThirst = 100.f;
		tStatus.fSpeed = 200.f;
		tStatus.fDamage = 20.f;

		m_pObjectPool->m_cumPlayerPool[playerId]->SetPlayerStatus(tStatus);
	}
	int DB_Num = 9999;
	m_pObjectPool->m_cumPlayerPool[playerId]->SetDbNum(DB_Num);
	m_pObjectPool->m_cumPlayerPool[playerId]->SetNumID(playerId);
	m_pObjectPool->m_cumPlayerPool[playerId]->SetWcID(wcId);
	m_pObjectPool->m_cumPlayerPool[playerId]->SetLocalPos(Vec3(18000.f, 200.f, 2000.f));
	m_pObjectPool->m_cumPlayerPool[playerId]->SetLocalScale(Vec3(1.5f, 1.5f, 1.5f));
	//m_pObjectPool->m_cumPlayerPool[playerId]->SetLocalRot(Vec3(0.f, 0.f, 0.f));
}

void CPlayerProcess::AcceptClient(SOCKET& sSocket, USHORT playerId)
{
	CProcess::m_pObjectPool->m_cumPlayerPool[playerId]->SetSocket(sSocket);
	CProcess::m_pObjectPool->m_cumPlayerPool[playerId]->SetNumID(playerId);
	CProcess::m_pObjectPool->m_cumPlayerPool[playerId]->SetRecvState();
}

void CPlayerProcess::RecvPacket(USHORT playerId, char * packet, DWORD bytesize)
{
	char* cPacket = CProcess::m_pObjectPool->m_cumPlayerPool[playerId]->RecvEvent(bytesize, packet);

	if (cPacket != nullptr) {
		m_fpPacketProcess[packet[1]](playerId, packet);
		CProcess::m_pObjectPool->m_cumPlayerPool[playerId]->SetRecvState();
	}
}

void CPlayerProcess::PlayerLogin(USHORT playerId, char * packet)
{
	cs_login_packet*	login_packet = reinterpret_cast<cs_login_packet*>(packet);

	if (!ExistLoginList(playerId))
		InsertLoginList(playerId);
#ifdef DB_ON
	// USE DB
	string strName = login_packet->player_id;
	wstring wStrName;
	wStrName.assign(strName.begin(), strName.end());

	if (CDataBase::GetInst()->IsIDExist(wStrName)) {
		DB_Event UserInfo = CDataBase::GetInst()->GetUserInfo(wStrName);
		{

			tPlayerStatus tStatus;
			tStatus.fHealth = UserInfo.fHealth;
			tStatus.fHungry = UserInfo.fHungry;
			tStatus.fThirst = UserInfo.fThirst;
			tStatus.fSpeed = 200.f;
			tStatus.fDamage = 20.f;
			m_pObjectPool->m_cumPlayerPool[playerId]->SetPlayerStatus(tStatus);
		}
		m_pObjectPool->m_cumPlayerPool[playerId]->SetDbNum(UserInfo.inum);
		m_pObjectPool->m_cumPlayerPool[playerId]->SetNumID(playerId);
		m_pObjectPool->m_cumPlayerPool[playerId]->SetWcID(login_packet->player_id);
		m_pObjectPool->m_cumPlayerPool[playerId]->SetLocalPos(Vec3(UserInfo.fX, UserInfo.fY, UserInfo.fZ));
		m_pObjectPool->m_cumPlayerPool[playerId]->SetLocalScale(Vec3(1.5f, 1.5f, 1.5f));
		m_pObjectPool->m_cumPlayerPool[playerId]->SetLocalRot(Vec3(0.f, 0.f, 0.f));
		m_pObjectPool->m_cumPlayerPool[playerId]->SetConnect(true);

		CPacketMgr::Send_Login_OK_Packet(playerId);
	}
	else {
		DB_Event UserInfo{};
		UserInfo.strID = wStrName;
		UserInfo.fHealth = 100.f;
		UserInfo.fHungry = 100.f;
		UserInfo.fThirst = 100.f;
		UserInfo.fX = 18000.f;
		UserInfo.fY = 200.f;
		UserInfo.fZ = 2000.f;

		CDataBase::GetInst()->AddUserInfo(UserInfo);

		DB_Event Info;
		Info = CDataBase::GetInst()->GetUserInfo(wStrName);
		cout << Info.fX << ", " << Info.fY << ", " << Info.fZ << endl;
		{

			tPlayerStatus tStatus;
			tStatus.fHealth = UserInfo.fHealth;
			tStatus.fHungry = UserInfo.fHungry;
			tStatus.fThirst = UserInfo.fThirst;
			tStatus.fSpeed = 200.f;
			tStatus.fDamage = 20.f;
			m_pObjectPool->m_cumPlayerPool[playerId]->SetPlayerStatus(tStatus);
		}
		m_pObjectPool->m_cumPlayerPool[playerId]->SetDbNum(UserInfo.inum);
		m_pObjectPool->m_cumPlayerPool[playerId]->SetNumID(playerId);
		m_pObjectPool->m_cumPlayerPool[playerId]->SetWcID(login_packet->player_id);
		m_pObjectPool->m_cumPlayerPool[playerId]->SetLocalPos(Vec3(UserInfo.fX, UserInfo.fY, UserInfo.fZ));
		m_pObjectPool->m_cumPlayerPool[playerId]->SetLocalScale(Vec3(1.5f, 1.5f, 1.5f));
		m_pObjectPool->m_cumPlayerPool[playerId]->SetLocalRot(Vec3(0.f, 0.f, 0.f));
		m_pObjectPool->m_cumPlayerPool[playerId]->SetConnect(true);

		CPacketMgr::Send_Login_OK_Packet(playerId);
	}
#else
	// NO DB
	 //Player Init
	CPacketMgr::Send_Login_OK_Packet(playerId);
	cout << login_packet->player_id << "���� ���� �Ͽ����ϴ�. " << endl;
	Init_Player(playerId, login_packet->player_id);
#endif // DB_ON

	// Server -> Client�� �ʱ� �÷��̾� �� ��Ŷ ����
	//CPacketMgr::Send_Status_Player_Packet(playerId, playerId);
	//CPacketMgr::Send_Pos_Player_Packet(playerId, playerId);
	InitViewList(playerId);
}

void CPlayerProcess::PlayerMove(USHORT playerId, char * packet)
{
	cout << "Worker " << CTimerMgr::GetInst()->GetDeltaTime() << endl;
	cs_move_packet* move_packet = reinterpret_cast<cs_move_packet*>(packet);

	Vec3 vLocalPos = move_packet->vLocalPos;

	bool bRun = move_packet->bRun;
	Vec3 vWorldDir = move_packet->vWorldDir;

	Vec3 vPos = CProcess::m_pObjectPool->m_cumPlayerPool[playerId]->GetLocalPos();
	Vec3 vOriginPos = CProcess::m_pObjectPool->m_cumPlayerPool[playerId]->GetLocalPos();
	float fSpeed = CProcess::m_pObjectPool->m_cumPlayerPool[playerId]->GetSpeed();
	float fHeight = move_packet->fHeight;
	// Walk or Run
	if (bRun)
	{
		fSpeed *= 5.f;
	}
	vPos += vWorldDir * fSpeed * 0.05f;

	//CProcess::m_pObjectPool->m_cumPlayerPool[playerId]->SetLocalPos(vLocalPos);
	CProcess::m_pObjectPool->m_cumPlayerPool[playerId]->SetLocalPos(Vec3(vPos.x, fHeight, vPos.z));

	UpdateViewList(playerId);
}

void CPlayerProcess::PlayerLogout(USHORT playerId)
{
	auto& User = m_pObjectPool->m_cumPlayerPool[playerId];

	// �̹� Disconnect�� ������ ���
	if (!User->GetConnect())
		return;
	// ���� Diconnect�� ���°� �ƴ� ���
#ifdef DB_ON
	DB_Event ev;
	ev.fHealth = User->GetHealth();
	ev.fHungry = User->GetHungry();
	ev.fThirst = User->GetThirst();
#endif // DB_ON

	m_pObjectPool->m_cumPlayerPool[playerId]->SetConnect(false);
	string name = m_pObjectPool->m_cumPlayerPool[playerId]->GetWcID();
	cout << name << " ���� �α׾ƿ� �ϼ̽��ϴ�. " << endl;
	if (ExistLoginList(playerId))
		DeleteLoginList(playerId);

	concurrent_unordered_set<USHORT>list;
	CopyBeforeLoginList(list);
	for (auto& au : list)
		CPacketMgr::Send_Remove_Packet(au, playerId);
}

void CPlayerProcess::PlayerPos(USHORT playerId, char * packet)
{
	cs_pos_packet* pos_packet = reinterpret_cast<cs_pos_packet*>(packet);

	//Vec3 vPrePos = m_pObjectPool->m_cumPlayerPool[playerId]->GetLocalPos();
	Vec3 vCurrPos = pos_packet->vLocalPos;

	m_pObjectPool->m_cumPlayerPool[playerId]->SetLocalPos(vCurrPos);
	UpdateViewList(playerId);
	/*if (ObjectRangeCheck(vPrePos, vCurrPos, 10.f)) {
		m_pObjectPool->m_cumPlayerPool[playerId]->SetLocalPos(vCurrPos);
		UpdateViewList(playerId);
	}
	else
		return;*/
}

void CPlayerProcess::PlayerRot(USHORT playerId, char * packet)
{
	cs_rot_packet* rot_packet = reinterpret_cast<cs_rot_packet*>(packet);
	Vec3 vPreRot = m_pObjectPool->m_cumPlayerPool[playerId]->GetLocalRot();
	Vec3 vCurrRot = rot_packet->vRot;
	Vec3 vRight = rot_packet->vDir[0]; DIR_TYPE::RIGHT;
	Vec3 vUp = rot_packet->vDir[1]; DIR_TYPE::UP;
	Vec3 vFront = rot_packet->vDir[2]; DIR_TYPE::FRONT;
	if (vPreRot != vCurrRot) {
		m_pObjectPool->m_cumPlayerPool[playerId]->SetLocalRot(vCurrRot);
		m_pObjectPool->m_cumPlayerPool[playerId]->SetLocalDir(DIR_TYPE::RIGHT, vRight);
		m_pObjectPool->m_cumPlayerPool[playerId]->SetLocalDir(DIR_TYPE::UP, vUp);
		m_pObjectPool->m_cumPlayerPool[playerId]->SetLocalDir(DIR_TYPE::FRONT, vFront);
	}
}

void CPlayerProcess::PlayerAttack(USHORT playerId, char * packet)
{
	cs_attack_packet* attack_packet = reinterpret_cast<cs_attack_packet*>(packet);
	UINT	uiType = attack_packet->attack_uiType;
	USHORT	attack_id = attack_packet->attack_id;
	char	eType = attack_packet->eType;

	float	fDamage = GetDamage(eType);


	if ((UINT)ATTACK_TYPE::ANIMAL == uiType)
	{
		auto& Animal = m_pObjectPool->m_cumAnimalPool[attack_id];

		float fHealth = Animal->GetHealth();

		fHealth -= fDamage;

		if (fHealth > 0.f) {
			Animal->SetHealth(fHealth);
			cout << "Animal Attack : " << attack_id << endl;
		}
		else {
			Animal->SetWakeUp(false);
			Animal->SetState(OST_DIE);
			fHealth = 0.f;
			Animal->SetHealth(fHealth);
			PushEvent_Animal_Die(attack_id, playerId);
			cout << "Animal Attack : " << attack_id << endl;

			// Item Get
			char eItemType;
			int	iAmount = 1;
			int random = rand() % 3;
			if (random == 0)
				eItemType = ITEM_LEATHER;
			else if (random == 1)
				eItemType == ITEM_BONE;
			else if (random == 2)
				eItemType == ITEM_MEAT;

			if (eItemType == ITEM_MACHETTE)
				iAmount = 3;
			CPacketMgr::Send_Add_Item_Packet(playerId, eItemType, iAmount);
		}

		BEHAVIOR_TYPE Type = Animal->GetType();

		if (Type == (UINT)BEHAVIOR_TYPE::B_PASSIVE)
			PushEvent_Animal_Attack(attack_id, playerId);

	}
	else if ((UINT)ATTACK_TYPE::NATURAL == uiType)
	{
		auto& User = m_pObjectPool->m_cumPlayerPool[playerId];
		auto& Natural = m_pObjectPool->m_cumNaturalPool[attack_id];

		bool bDestroy = Natural->GetDestroy();
		if (!bDestroy) return;

		if (eType == N_NONE)
			return;
		if (CollisionSphere(User, Natural, 0.2f)) {
			float fHealth = Natural->GetHealth();

			fHealth -= fDamage;

			if (fHealth <= 0.f) {
				Natural->SetDestroy(true);
				Natural->SetAngle(0.f);

				fHealth = 0.f;
				Natural->SetHealth(fHealth);
				PushEvent_Natural_Die(attack_id);
			}
			else {
				Natural->SetHealth(fHealth);
				char eItemType = ITEM_END;
				int iAmount = 1;
				if (eType == N_TREE) {
					int random = rand() % 5;
					if (random == 0)
						eItemType = ITEM_APPLE;
					else {
						if (eType == ITEM_AXE) {
							iAmount = 3;
							eItemType = ITEM_WOOD;
						}
					}

				}
				else {
					if (eType == N_STONE) {
						if (eItemType == ITEM_PICKAXE) {
							iAmount = 3;
							eItemType = ITEM_STONE;
						}
					}
					else if (eType == N_BUSH) {
						int iAmount = 2;
						eItemType = ITEM_CLOTH;
					}
				}
				CPacketMgr::Send_Add_Item_Packet(playerId, eItemType, iAmount);
			}
		}
	}
	else
		return;
}

void CPlayerProcess::PlayerAnimation(USHORT playerId, char * packet)
{
	USHORT User = playerId;

	cs_animation_packet* animation_packet = reinterpret_cast<cs_animation_packet*>(packet);

	UINT uiType = animation_packet->uiType;

	concurrent_unordered_set<USHORT>	UserViewList;
	m_pObjectPool->m_cumPlayerPool[User]->CopyUserViewList(UserViewList);

	for (auto& other : UserViewList) {
		bool bConnect = m_pObjectPool->m_cumPlayerPool[other]->GetConnect();
		if (!bConnect) continue;
		CPacketMgr::Send_Animation_Packet(other, User, uiType);
	}
}

void CPlayerProcess::PlayerUseItem(USHORT playerId, char * packet)
{
	auto& User = m_pObjectPool->m_cumPlayerPool[playerId];
	cs_use_item_packet* use_item_packet = reinterpret_cast<cs_use_item_packet*>(packet);
	char eType = use_item_packet->eType;

	float fValue = GetValue(eType);

	if (eType > ITEM_FOOD && eType < ITEM_FOOD_END) {
		if (eType == ITEM_MEAT)
			User->SetIncreaseHungry(fValue);
		else {
			User->SetIncreaseHungry(fValue);
		}
	}
	else if (eType > ITEM_DRINK && eType < ITEM_DRINK_END) {
		if (eType != ITEM_EMPTY_BOTTLE) {
			User->SetIncreaseThirst(fValue);
		}
	}
	else if (eType > ITEM_HEAL && eType < ITEM_HEAL_END) {
		User->SetIncreaseHealth(fValue);
	}
	CPacketMgr::Send_Status_Player_Packet(playerId);
}

void CPlayerProcess::PlayerEquipArmor(USHORT playerId, char * packet)
{
	cs_equip_armor_packet* armor_packet = reinterpret_cast<cs_equip_armor_packet*>(packet);

	char eType = armor_packet->eType;

	float fValue = GetArmor(eType);

	m_pObjectPool->m_cumPlayerPool[playerId]->SetArmor(fValue);
}

void CPlayerProcess::PlayerDestroyArmor(USHORT playerId, char * packet)
{
	cs_destroy_armor_packet* armor_packet = reinterpret_cast<cs_destroy_armor_packet*>(packet);
	
	float fValue = 0.f;
	m_pObjectPool->m_cumPlayerPool[playerId]->SetArmor(fValue);
}

void CPlayerProcess::PlayerInstallHousing(USHORT playerId, char * packet)
{
	cs_install_housing_packet* install_housing_packet = reinterpret_cast<cs_install_housing_packet*>(packet);

	USHORT house_id = m_houseNum++;
	UINT uiType = install_housing_packet->housing_type;
	Vec3 vLocalPos = install_housing_packet->vLocalPos;
	Vec3 vLocalRot = install_housing_packet->vLocalRot;
	Vec3 vLocalScale = install_housing_packet->vLocalScale;
	Vec3 vOffsetPos = install_housing_packet->vOffsetPos;
	Vec3 vOffsetScale = install_housing_packet->vOffsetScale;

	CHousing* House = new CHousing();
	House->SetInstall(true);
	House->SetType((HOUSING_TYPE)uiType);
	House->SetLocalPos(vLocalPos);
	House->SetLocalRot(vLocalRot);
	House->SetLocalScale(vLocalScale);

	if (uiType >= HOUSING_WALL && uiType < HOUSING_FLOOR)
		House->SetOffsetPos(Vec3(0.f, 0.f, 120.f));
	House->SetOffsetScale(Vec3(195.f, 195.f, 195.f));
	m_pObjectPool->Install_House(House, house_id);

	concurrent_unordered_set<USHORT> loginList;

	CopyBeforeLoginList(loginList);

	for (auto& au : loginList)
	{
		bool bConnect = m_pObjectPool->m_cumPlayerPool[au]->GetConnect();
		if (!bConnect)	continue;
		if (au == playerId) continue;
		CPacketMgr::Send_Install_Housing_Packet(au, house_id);
	}
}

void CPlayerProcess::PlayerRemoveHousing(USHORT playerId, char * packet)
{
	cs_remove_housing_packet* remove_housing_packet = reinterpret_cast<cs_remove_housing_packet*>(packet);

	USHORT house_id = remove_housing_packet->house_id;

	m_pObjectPool->Remove_House(house_id);

	concurrent_unordered_set<USHORT> loginList;

	CopyBeforeLoginList(loginList);

	for (auto& au : loginList)
	{
		bool bConnect = m_pObjectPool->m_cumPlayerPool[au]->GetConnect();
		if (!bConnect)	continue;
		if (au == playerId) continue;
		CPacketMgr::Send_Remove_Housing_Packet(au, house_id);
	}
}

void CPlayerProcess::PlayerNaturalAttack(USHORT playerId, char * packet)
{
	cs_natural_attack_packet* natural_attack_packet = reinterpret_cast<cs_natural_attack_packet*>(packet);
}

void CPlayerProcess::PlayerChat(USHORT _usID, char * _packet)
{
	cs_chat_packet* chat_packet = reinterpret_cast<cs_chat_packet*>(_packet);

	concurrent_unordered_set<USHORT> loginList;

	CopyBeforeLoginList(loginList);

	for (auto& player : loginList) {
		CPacketMgr::GetInst()->Send_Chat_Packet(_usID, player, chat_packet->meesage);
	}
}

void CPlayerProcess::InitViewList(USHORT playerId)
{
	USHORT user = playerId;
	// Player ViewList Update

	concurrent_unordered_set<USHORT> loginList;
	CopyBeforeLoginList(loginList);

	// [ Add Player List ]
	Vec3 player_pos = m_pObjectPool->m_cumPlayerPool[user]->GetLocalPos();
	// ------------------- CHECK -------------------- &?????
	for (auto au : loginList) {
		bool bConnect = m_pObjectPool->m_cumPlayerPool[au]->GetConnect();
		if (!bConnect) continue;
		Vec3 other_pos = m_pObjectPool->m_cumPlayerPool[au]->GetLocalPos();
		if (ObjectRangeCheck(player_pos, other_pos, PLAYER_VIEW_RANGE))
		{
			CPacketMgr::Send_Pos_Packet(au, user);
			CPacketMgr::Send_Pos_Packet(user, au);
		}
	}

	// [ Add Animal List ]
	for (auto& au : m_pObjectPool->m_cumAnimalPool) {
		char eType = au.second->GetState();
		if (eType == OBJ_STATE_TYPE::OST_DIE) continue;

		Vec3 Animal_Pos = au.second->GetLocalPos();

		if (ObjectRangeCheck(player_pos, Animal_Pos, PLAYER_VIEW_RANGE))
		{
			if (!au.second->GetWakeUp())
			{
				au.second->SetWakeUp(true);

				PushEvent_Animal_Behavior(au.first, playerId);
			}
			CPacketMgr::Send_Pos_Packet(playerId, au.first);
		}
	}

	// [ Add Housing ]
	for (auto& au : m_pObjectPool->m_cumHousingPool)
	{
		bool bInstall = m_pObjectPool->m_cumHousingPool[au.first]->GetInstall();
		if (!bInstall) continue;
		CPacketMgr::Send_Install_Housing_Packet(playerId, au.first);
	}

	// [ Add Natural ]
	for (auto& au : m_pObjectPool->m_cumNaturalPool)
	{
		bool bDestroy = m_pObjectPool->m_cumNaturalPool[au.first]->GetDestroy();
		if (bDestroy) continue;
		CPacketMgr::Send_Put_Natural_Packet(playerId, au.first);
	}
}

void CPlayerProcess::UpdateViewList(USHORT playerId)
{
	// [ Initalize ]
	USHORT user = playerId;

	concurrent_unordered_set<USHORT>	loginList;
	concurrent_unordered_set<USHORT>	beforeViewList; // �̵��ϱ� ���� ViewList
	concurrent_unordered_set<USHORT>	afterViewList; // �̵� ���� ViewList

	CopyBeforeLoginList(loginList);
	m_pObjectPool->m_cumPlayerPool[user]->CopyViewList(beforeViewList);

	Vec3 vPlayer_Pos = m_pObjectPool->m_cumPlayerPool[playerId]->GetLocalPos();

	// [ Update Player List ]
	// Player ViewList Update
	for (auto other : loginList)
	{
		if (other == playerId) continue;
		bool bConnect = m_pObjectPool->m_cumPlayerPool[other]->GetConnect();
		if (!bConnect) continue;
		Vec3 vOther_Pos = m_pObjectPool->m_cumPlayerPool[other]->GetLocalPos();
		if (ObjectRangeCheck(vPlayer_Pos, vOther_Pos, PLAYER_VIEW_RANGE)) {
			afterViewList.insert(other);
		}

	}
	// [ Update Animal List ]
	// - Animal ViewList Update
	for (auto& animal : m_pObjectPool->m_cumAnimalPool)
	{
		char eType = animal.second->GetState();
		if (eType == OBJ_STATE_TYPE::OST_DIE) continue;
		Vec3 Other_Pos = animal.second->GetLocalPos();
		if (ObjectRangeCheck(vPlayer_Pos, Other_Pos, PLAYER_VIEW_RANGE))
		{
			afterViewList.insert(animal.first);
		}
	}

	// - Player Pos Send
	CPacketMgr::Send_Pos_Packet(user, user);

	// [ Before ViewList Obejct Remove ]
	for (auto before : beforeViewList)
	{
		if (afterViewList.end() == afterViewList.find(before)) // ������Ʈ�� ViewList�� �������� ���� ���
		{
			if (before < MAX_USER) // Object�� �÷��̾��� ���
			{
				CPacketMgr::Send_Remove_Packet(user, before);
				CPacketMgr::Send_Remove_Packet(before, user);
			}
			else if (before < END_ANIMAL) // Object�� ������ ���
			{
				CPacketMgr::Send_Remove_Packet(user, before);
			}
			else
			{
				cout << before << ": �߸��� Index �Դϴ�." << endl;
				continue;
			}
		}
	}

	// [ After ViewList Object Update ]
	for (auto after : afterViewList)
	{
		if (beforeViewList.end() == beforeViewList.find(after)) // New ViewList �� ���
		{
			if (after < MAX_USER)
			{
				CPacketMgr::Send_Pos_Packet(user, after);
				CPacketMgr::Send_Pos_Packet(after, user);
			}
			else
			{
				bool bWakeUp = m_pObjectPool->m_cumAnimalPool[after]->GetWakeUp();
				if (!bWakeUp)
				{
					m_pObjectPool->m_cumAnimalPool[after]->SetWakeUp(true);
					PushEvent_Animal_Behavior(after, user);

				}
				CPacketMgr::Send_Pos_Packet(user, after);
			}
		}
		else // ������ ViewList �� ���
		{
			if (after < MAX_USER){
				CPacketMgr::Send_Pos_Packet(user, after);
				CPacketMgr::Send_Pos_Packet(after, user);
			}
			else {
				bool bWakeUp = m_pObjectPool->m_cumAnimalPool[after]->GetWakeUp();
				if (!bWakeUp)
				{
					m_pObjectPool->m_cumAnimalPool[after]->SetWakeUp(true);
					PushEvent_Animal_Behavior(after, user);
				}
			}
		}
	}
}

float CPlayerProcess::GetDamage(char eType)
{
	float fDamage;

	switch (eType) {
	case ITEM_PICKAXE:
		fDamage = 30.f;
		break;
	case ITEM_AXE:
		fDamage = 30.f;
		break;
	case ITEM_MACHETTE:
		fDamage = 45.f;
	case ITEM_WOODCLUB:
		fDamage = 20.f;
	break;
	case ITEM_HAMMER:
		fDamage = 15.f;
	break;
	case ITEM_BOW:
		fDamage = 30.f;
	break;
	default:
		fDamage = 0.f;
		break;
	}

	return fDamage;
}

float CPlayerProcess::GetValue(char eType)
{
	float fValue{};
	switch (eType) {
	case ITEM_MEAT:
		fValue = 15.f;
		break;
	case ITEM_COOKMEAT:
		fValue = 20.f;
		break;
	case ITEM_CHOCOLATEBAR:
		fValue = 50.f;
		break;
	case ITEM_CAN:
		fValue = 25.f;
		break;
	case ITEM_APPLE:
		fValue = 15.f;
		break;
	case ITEM_EMPTY_BOTTLE:
		fValue = 0.f;
		break;
	case ITEM_WATER_BOTTLE:
		fValue = 25.f;
		break;
	case ITEM_MEDICKIT:
		fValue = 100.f;
		break;
	case ITEM_BANDAGE:
		fValue = 10.f;
		break;
	default:
		fValue = 0;
		break;
	}

	return fValue;
}

float CPlayerProcess::GetArmor(char eType)
{
	float fValue{};
	switch (eType) 
	{
	case ITEM_TSHIRT:
		fValue = 30.f;
		break;
	case ITEM_SHIRT:
		fValue = 60.f;
		break;
	case ITEM_JACKET:
		fValue = 100.f;
		break;
	default:
		fValue = 0.f;
		break;
	}
	return fValue;
}
