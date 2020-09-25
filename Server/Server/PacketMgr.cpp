#include "PacketMgr.h"
#include "Process.h"
#include "DataBase.h"
#include "Network.h"
#include "Player.h"

CPacketMgr::CPacketMgr()
{	
}

CPacketMgr::~CPacketMgr()
{
}

void CPacketMgr::Send_Packet(USHORT playerId, void* packet)
{
	DWORD flags{ 0 };
	char* cPacket = reinterpret_cast<char*>(packet);
	int packet_size = cPacket[0];

	OVER_EX* send_over = new OVER_EX;
	memset(send_over, 0x00, sizeof(OVER_EX));
	send_over->m_Event = EV_SEND;
	memcpy(send_over->m_MessageBuffer, cPacket, packet_size);
	send_over->m_DataBuffer.buf = send_over->m_MessageBuffer;
	send_over->m_DataBuffer.len = packet_size;

	int retval = WSASend(CProcess::m_pObjectPool->m_cumPlayerPool[playerId]->GetSocket(),
		&send_over->m_DataBuffer, 1, NULL, flags, &send_over->m_Overlapped, NULL);

	if (retval == SOCKET_ERROR) {
		int err_no = WSAGetLastError();
		if (err_no != WSA_IO_PENDING) {
			CNetwork::Err_display("SEND EVENT SOCKET_ERR - ", err_no);
		}
	}
}

void CPacketMgr::Send_Login_OK_Packet(USHORT playerId)
{
	sc_login_ok_packet packet;
	packet.size = sizeof(packet);
	packet.type = SC_LOGIN_OK;
	packet.id = playerId;
	packet.fHealth = CProcess::m_pObjectPool->m_cumPlayerPool[playerId]->GetHealth();
	packet.fHungry = CProcess::m_pObjectPool->m_cumPlayerPool[playerId]->GetHungry();
	packet.fThirst = CProcess::m_pObjectPool->m_cumPlayerPool[playerId]->GetThirst();
	packet.vPos = CProcess::m_pObjectPool->m_cumPlayerPool[playerId]->GetLocalPos();
	Send_Packet(playerId, &packet);
}

void CPacketMgr::Send_Login_Fail_Packet(USHORT playerId)
{
	sc_login_fail_packet packet;
	packet.size = sizeof(packet);
	packet.type = SC_LOGIN_FAIL;
	Send_Packet(playerId, &packet);
}

void CPacketMgr::Send_Pos_Packet(USHORT User, USHORT Acter)
{
	sc_pos_packet packet;
	packet.size = sizeof(sc_pos_packet);
	packet.type = SC_POS;
	packet.usId = Acter;
	
	USHORT	user = User;
	USHORT	acter = Acter;

	
	if (acter < MAX_USER) {
		packet.vPos = CProcess::m_pObjectPool->m_cumPlayerPool[acter]->GetLocalPos();
		packet.vRot = CProcess::m_pObjectPool->m_cumPlayerPool[acter]->GetLocalRot();
		if (CProcess::m_pObjectPool->m_cumPlayerPool[user]->ExistList(Acter)) {
			Send_Packet(user, &packet);
		}
		else {
			CProcess::m_pObjectPool->m_cumPlayerPool[user]->InsertList(Acter);
			Send_Packet(user, &packet);
		}
	}
	else if (acter < END_ANIMAL) {
		packet.vPos = CProcess::m_pObjectPool->m_cumAnimalPool[acter]->GetLocalPos();
		packet.vRot = CProcess::m_pObjectPool->m_cumAnimalPool[acter]->GetLocalRot();
		if (CProcess::m_pObjectPool->m_cumPlayerPool[user]->ExistList(Acter)) {
			Send_Packet(user, &packet);
		}
		else {
			CProcess::m_pObjectPool->m_cumPlayerPool[user]->InsertList(Acter);
			Send_Packet(user, &packet);
		}
	}
}

void CPacketMgr::Send_Rot_Packet(USHORT User, USHORT Acter)
{
	sc_rot_packet packet;
	packet.size = sizeof(sc_rot_packet);
	packet.type = SC_ROT;
	packet.usId = Acter;
	
	USHORT acter = Acter;
	if (acter < MAX_USER)
		packet.vRot = CProcess::m_pObjectPool->m_cumPlayerPool[acter]->GetLocalRot();
	else
		packet.vRot = CProcess::m_pObjectPool->m_cumAnimalPool[acter]->GetLocalRot();

	Send_Packet(User, &packet);
}

void CPacketMgr::Send_Remove_Packet(USHORT User, USHORT Acter)
{
	sc_remove_packet packet;
	packet.size = sizeof(sc_remove_packet);
	packet.type = SC_REMOVE;
	packet.usId = Acter;

	USHORT user = User;
	USHORT acter = Acter;

	if (acter < END_ANIMAL) {
		if (CProcess::m_pObjectPool->m_cumPlayerPool[User]->ExistList(Acter)) {
			CProcess::m_pObjectPool->m_cumPlayerPool[User]->DeleteList(Acter);
		}
		Send_Packet(user, &packet);
	}
}

void CPacketMgr::Send_Animation_Packet(USHORT User, USHORT Acter, UINT uiAnimationType)
{
	sc_animation_packet packet;
	packet.size = sizeof(sc_animation_packet);
	packet.type = SC_ANIMATION;
	packet.usId = Acter;
	packet.uiType = uiAnimationType;
	Send_Packet(User, &packet);
}

void CPacketMgr::Send_Status_Player_Packet(USHORT playerId)
{
	sc_status_player_packet packet;
	packet.size = sizeof(sc_status_player_packet);
	packet.type = SC_STATUS_PLAYER;

	packet.fHealth = CProcess::m_pObjectPool->m_cumPlayerPool[playerId]->GetHealth();
	packet.fHungry = CProcess::m_pObjectPool->m_cumPlayerPool[playerId]->GetHungry();
	packet.fThrist = CProcess::m_pObjectPool->m_cumPlayerPool[playerId]->GetThirst();

	Send_Packet(playerId, &packet);
}

void CPacketMgr::Send_Chat_Packet(USHORT playerId, USHORT OtherId, char message[])
{
	sc_chat_packet packet;
	packet.size = sizeof(sc_chat_packet);
	packet.type = SC_CHAT;
	strcpy_s(packet.ID, CProcess::m_pObjectPool->m_cumPlayerPool[playerId]->GetWcID());
	strcpy_s(packet.meesage, message);
	Send_Packet(OtherId, &packet);
}

void CPacketMgr::Send_Death_Player_Packet(USHORT playerId)
{
	sc_death_player_packet packet;
	packet.size = sizeof(sc_death_player_packet);
	packet.type = SC_DEATH_PLAYER;
	packet.fHealth = 100.f;
	packet.fHungry = 100.f;
	packet.fStamina = 100.f;
	packet.id = playerId;
	packet.vLocalPos = Vec3(18000.f, 200.f, 2000.f);
	Send_Packet(playerId, &packet);
}

void CPacketMgr::Send_Install_Housing_Packet(USHORT player_Id, USHORT housing_Id)
{
	sc_install_housing_packet packet;

	packet.size = sizeof(sc_install_housing_packet);
	packet.type = SC_INSTALL_HOUSE;
	packet.house_id = housing_Id;
	packet.house_grade = CProcess::m_pObjectPool->m_cumHousingPool[housing_Id]->GetUpgrade();

	packet.housing_type =	CProcess::m_pObjectPool->m_cumHousingPool[housing_Id]->GetType();
	packet.vLocalPos =		CProcess::m_pObjectPool->m_cumHousingPool[housing_Id]->GetLocalPos();
	packet.vLocalRot =		CProcess::m_pObjectPool->m_cumHousingPool[housing_Id]->GetLocalRot();
	packet.vLocalScale =	CProcess::m_pObjectPool->m_cumHousingPool[housing_Id]->GetLocalScale();
	Send_Packet(player_Id, &packet);
}

void CPacketMgr::Send_Remove_Housing_Packet(USHORT player_Id, USHORT housing_Id)
{
	sc_remove_housing_packet packet;

	packet.size = sizeof(sc_remove_housing_packet);
	packet.type = SC_REMOVE_HOUSE;
	packet.house_id = housing_Id;
	Send_Packet(player_Id, &packet);
}

void CPacketMgr::Send_Check_Housing_Packet(USHORT player_Id, USHORT housing_Id, bool bCheck)
{
	sc_check_housing_packet	packet;
	packet.size = sizeof(sc_check_housing_packet);
	packet.type = SC_CHECK_HOUSE;
	packet.house_id = housing_Id;
	packet.bCheck = bCheck;
	Send_Packet(player_Id, &packet);
}

void CPacketMgr::Send_Upgrade_Housing_Packet(USHORT PlayerId, USHORT house_Id)
{
	sc_upgrade_housing_packet packet;
	packet.size = sizeof(sc_upgrade_housing_packet);
	packet.type = SC_UPGRADE_HOUSE;
	packet.house_id = house_Id;
	packet.house_grade = CProcess::m_pObjectPool->m_cumHousingPool[house_Id]->GetUpgrade();

	Send_Packet(PlayerId, &packet);
}

void CPacketMgr::Send_Put_Natural_Packet(USHORT PlayerId, USHORT NaturalId)
{
	sc_put_natural_packet packet;
	packet.size = sizeof(sc_put_natural_packet);
	packet.type = SC_PUT_NATURAL;
	packet.natural_id = NaturalId;
	packet.fHealth = CProcess::m_pObjectPool->m_cumNaturalPool[NaturalId]->GetHealth();
	packet.bDestroy = CProcess::m_pObjectPool->m_cumNaturalPool[NaturalId]->GetDestroy();
	packet.vLocalPos = CProcess::m_pObjectPool->m_cumNaturalPool[NaturalId]->GetLocalPos();
	packet.vLocalScale = CProcess::m_pObjectPool->m_cumNaturalPool[NaturalId]->GetLocalScale();
	packet.vOffsetPos = CProcess::m_pObjectPool->m_cumNaturalPool[NaturalId]->GetOffsetPos();
	packet.vOffsetScale = CProcess::m_pObjectPool->m_cumNaturalPool[NaturalId]->GetOffsetScale();
	Send_Packet(PlayerId, &packet);
}

void CPacketMgr::Send_Natural_Destroy_Packet(USHORT PlayerId, USHORT NaturalId)
{
	sc_destroy_natural_packet packet;
	packet.size = sizeof(sc_destroy_natural_packet);
	packet.type = SC_DESTROY_NATURAL;
	packet.natural_id = NaturalId;
	packet.vOriginRot = CProcess::m_pObjectPool->m_cumNaturalPool[NaturalId]->GetOriginRot();

	Send_Packet(PlayerId, &packet);

}

void CPacketMgr::Send_Weather_Packet(USHORT player_Id, bool bRain)
{
	sc_weather_packet packet;
	packet.size = sizeof(sc_weather_packet);
	packet.type = SC_WEATHER;
	packet.bRain = bRain;
	Send_Packet(player_Id, &packet);
}

void CPacketMgr::Send_Time_Packet(USHORT player_Id, int Day, int Hour, int Min)
{
	sc_time_packet packet;
	packet.size = sizeof(sc_time_packet);
	packet.type = SC_TIME;
	packet.iday = Day;
	packet.ihour = Hour;
	packet.imin = Min;
	Send_Packet(player_Id, &packet);
}

void CPacketMgr::Send_Add_Item_Packet(USHORT player_Id, char eItemtype, int iAmount)
{
	sc_add_item_packet packet;
	packet.size = sizeof(sc_add_item_packet);
	packet.type = SC_ADD_ITEM;
	packet.eItemType = eItemtype;
	packet.iAmount = iAmount;

	Send_Packet(player_Id, &packet);
}

void CPacketMgr::Send_Full_Server_Packet(SOCKET m_sock)
{
	sc_full_server_packet packet;
	packet.size = sizeof(sc_full_server_packet);
	packet.type = SC_FULL_SERVER;

	char* cPacket = reinterpret_cast<char*>(&packet);
	char	iSize = cPacket[0];

	DWORD flags{0};
	int retval = send(m_sock, cPacket, iSize, flags);

	if (retval == SOCKET_ERROR) {
		int err_no = WSAGetLastError();
		if (err_no != WSA_IO_PENDING) {
			CNetwork::Err_display("Send Socket Err - ", err_no);
		}
	}
}

void CPacketMgr::Send_Disconnect_Server_Packet(USHORT player_Id)
{
	sc_disconnect_server_packet packet;
	packet.size = sizeof(sc_disconnect_server_packet);
	packet.tpye = SC_DISCONNECT_SERVER;
	Send_Packet(player_Id, &packet);
}
