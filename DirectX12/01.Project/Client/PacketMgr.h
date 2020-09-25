#pragma once
#include "header.h"

class CGameObject;

class CPacketMgr
{
	SINGLE(CPacketMgr)

private:
	WSABUF	m_SendWsaBuf;
	char		m_cSendBuf[MAX_PACKET_SIZE];

public:
	static CGameObject*	m_pPlayer;
	static int count[CS_END];

public:
	void SetPlayerObj(CGameObject* pObj) { m_pPlayer = pObj; }

public:
	void Send_Login_Packet(string strID);
	void Send_Move_Packet(Vec3 vWorldDir, bool bRun);
	void Send_Chat_Packet(string message);
	void Send_Logout_Packet();
	void Send_Pos_Player_Packet();
	void Send_Rot_player_Packet();
	void Send_Install_Housing_Packet(UINT uiType, Vec3 vLocalPos, Vec3 vLocalRot, Vec3 vLocalScale, Vec3 vOffsetPos, Vec3 vOffsetScale);
	void Send_Remove_Housing_Packet(USHORT houseId);
	void Send_Attack_Player_Packet(UINT attackType, USHORT attackId, char eitemType);
	void Send_Animation_Player_Packet(UINT uiAnimationType);
	void Send_Use_Item_Packet(char eType);
	void Send_Equip_Armor_Packet(char eType);
	void Send_Destroy_Armor_Packet();
};

