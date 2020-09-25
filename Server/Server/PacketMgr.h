#pragma once
#include "stdafx.h"

class CPacketMgr
{
	SINGLE(CPacketMgr)

public:
	static void Send_Packet(USHORT playerId, void* packet);

public: // 로그인 관련 패킷
	static void Send_Login_OK_Packet(USHORT playerId);
	static void Send_Login_Fail_Packet(USHORT playerId);

public: // [ COMMON ]
	static void Send_Pos_Packet(USHORT User, USHORT Acter);
	static void Send_Rot_Packet(USHORT User, USHORT Acter);
	static void Send_Remove_Packet(USHORT User, USHORT Acter);
	static void Send_Animation_Packet(USHORT User, USHORT Acter, UINT uiAnimationType);

public: // Player 관련 패킷
	static void Send_Status_Player_Packet(USHORT playerId);
	static void Send_Chat_Packet(USHORT playerId, USHORT OtherId, char message[]);
	static void Send_Death_Player_Packet(USHORT playerId);

public:
	static void Send_Install_Housing_Packet(USHORT player_Id, USHORT housing_Id);
	static void Send_Remove_Housing_Packet(USHORT player_Id, USHORT housing_Id);
	static void Send_Check_Housing_Packet(USHORT player_Id, USHORT housing_Id, bool bCheck);
	static void Send_Upgrade_Housing_Packet(USHORT player_Id, USHORT house_Id);
public:
	static void Send_Put_Natural_Packet(USHORT PlayerId, USHORT NaturalId);
	static void Send_Natural_Destroy_Packet(USHORT PlayerId, USHORT NaturalId);

public: // etc
	static void Send_Weather_Packet(USHORT player_Id, bool bRain);
	static void Send_Time_Packet(USHORT player_Id, int Day, int Hour, int Min);

public:
	static void Send_Add_Item_Packet(USHORT player_Id, char eItemtype, int iAmount);

public: // Server
	static void Send_Full_Server_Packet(SOCKET m_sock);
	static void Send_Disconnect_Server_Packet(USHORT player_Id);
};