#include "stdafx.h"

#include "PacketMgr.h"
#include "Network.h"

#include <Engine/GameObject.h>
#include <Engine/Transform.h>
#include <Engine/NaviMgr.h>

CGameObject*	CPacketMgr::m_pPlayer;
int				CPacketMgr::count[(int)CS_END];
CPacketMgr::CPacketMgr()
{
	//ZeroMemory(m_cSendBuf, sizeof(m_cSendBuf));
	m_SendWsaBuf.buf = m_cSendBuf;
	m_SendWsaBuf.len = BUF_SIZE;

	for (int i = 0; i < CS_END; ++i) {
		count[i] = 0;
	}
}


CPacketMgr::~CPacketMgr()
{
}

void CPacketMgr::Send_Login_Packet(string strID)
{
	DWORD size{ 0 }, flag{ 0 };

	cs_login_packet*	login_packet = reinterpret_cast<cs_login_packet*>(m_cSendBuf);

	login_packet->size = sizeof(cs_login_packet);
	login_packet->type = CS_LOGIN;
	strcpy_s(login_packet->player_id, strID.c_str());

	m_SendWsaBuf.len = sizeof(cs_login_packet);

	int ret = WSASend(CNetwork::GetInst()->GetSocket(), &m_SendWsaBuf, 1, &size, flag, NULL, NULL);

	if (ret != 0) {
		int err_no = WSAGetLastError();
		CNetwork::Err_display("Err while sending packet - ", err_no);
	}
}

void CPacketMgr::Send_Move_Packet(Vec3 vWorldDir, bool bRun)
{
	DWORD size{ 0 }, flag{ 0 };

	cs_move_packet* packet = reinterpret_cast<cs_move_packet*>(m_cSendBuf);
	packet->size = sizeof(cs_move_packet);
	packet->type = CS_MOVE;
	packet->vLocalPos = m_pPlayer->Transform()->GetLocalPos();
	packet->vWorldDir = vWorldDir;
	packet->bRun = bRun;
	packet->fHeight = CNaviMgr::GetInst()->GetY(m_pPlayer->Transform()->GetLocalPos());

	m_SendWsaBuf.len = sizeof(cs_move_packet);

	int ret = WSASend(CNetwork::GetInst()->GetSocket(), &m_SendWsaBuf, 1, &size, flag, NULL, NULL);

	if (ret != 0) {
		int err_no = WSAGetLastError();
		CNetwork::Err_display("Err while sending packet - ", err_no);
	}
}

void CPacketMgr::Send_Chat_Packet(string message)
{
	cs_chat_packet* packet = reinterpret_cast<cs_chat_packet*>(m_cSendBuf);
	packet->size = sizeof(cs_chat_packet);
	packet->type = CS_CHAT;
	packet->id = CNetwork::m_usID;
	strcpy_s(packet->meesage, message.c_str());

	DWORD size{ 0 }, flag{ 0 };
	m_SendWsaBuf.len = sizeof(cs_chat_packet);
	int ret = WSASend(CNetwork::GetInst()->GetSocket(), &m_SendWsaBuf, 1, &size, flag, NULL, NULL);

	if (ret != 0) {
		int err_no = WSAGetLastError();
		CNetwork::Err_display("Err while sending packet - ", err_no);
	}
}

void CPacketMgr::Send_Logout_Packet()
{
	cs_logout_packet*	packet = reinterpret_cast<cs_logout_packet*>(m_cSendBuf);
	packet->size = sizeof(cs_logout_packet);
	packet->type = CS_LOGOUT;

	DWORD	size{ 0 }, flag{ 0 };
	m_SendWsaBuf.len = sizeof(cs_logout_packet);
	int ret = WSASend(CNetwork::GetInst()->GetSocket(), &m_SendWsaBuf, 1, &size, flag, NULL, NULL);

	if (ret != 0) {
		int err_no = WSAGetLastError();
		CNetwork::Err_display("Err while sending packet - ", err_no);
	}
}

void CPacketMgr::Send_Pos_Player_Packet()
{
	cs_pos_packet* packet = reinterpret_cast<cs_pos_packet*>(m_cSendBuf);
	packet->size = sizeof(cs_pos_packet);
	packet->type = CS_POS;
	packet->vLocalPos = m_pPlayer->Transform()->GetLocalPos();

	DWORD size{ 0 }, flag{ 0 };
	m_SendWsaBuf.len = sizeof(cs_pos_packet);
	int ret = WSASend(CNetwork::GetInst()->GetSocket(), &m_SendWsaBuf, 1, &size, flag, NULL, NULL);

	if (ret != 0) {
		int err_no = WSAGetLastError();
		CNetwork::Err_display("Err while sending packet - ", err_no);
	}
}

void CPacketMgr::Send_Rot_player_Packet()
{
	cs_rot_packet* packet = reinterpret_cast<cs_rot_packet*>(m_cSendBuf);
	packet->size = sizeof(cs_rot_packet);
	packet->type = CS_ROT;
	packet->vRot = m_pPlayer->Transform()->GetLocalRot();
	packet->vDir[0] = m_pPlayer->Transform()->GetLocalDir(DIR_TYPE::RIGHT);
	packet->vDir[1] = m_pPlayer->Transform()->GetLocalDir(DIR_TYPE::UP);
	packet->vDir[2] = m_pPlayer->Transform()->GetLocalDir(DIR_TYPE::FRONT);

	DWORD size{ 0 }, flag{ 0 };
	m_SendWsaBuf.len = sizeof(cs_rot_packet);
	int ret = WSASend(CNetwork::GetInst()->GetSocket(), &m_SendWsaBuf, 1, &size, flag, NULL, NULL);

	if (ret != 0) {
		int err_no = WSAGetLastError();
		CNetwork::Err_display("Err while sending packet - ", err_no);
	}
}

void CPacketMgr::Send_Install_Housing_Packet(UINT uiType, Vec3 vLocalPos, Vec3 vLocalRot, Vec3 vLocalScale, Vec3 vOffsetPos, Vec3 vOffsetScale)
{
	cs_install_housing_packet* install_housing_packet = reinterpret_cast<cs_install_housing_packet*>(m_cSendBuf);
	install_housing_packet->size = sizeof(cs_install_housing_packet);
	install_housing_packet->type = CS_HOUSING_INSTALL;
	install_housing_packet->housing_type = uiType;
	install_housing_packet->vLocalPos = vLocalPos;
	install_housing_packet->vLocalRot = vLocalRot;
	install_housing_packet->vLocalScale = vLocalScale;
	install_housing_packet->vOffsetPos = vOffsetPos;
	install_housing_packet->vOffsetScale = vOffsetScale;

	DWORD	size{ 0 }, flag{ 0 };
	m_SendWsaBuf.len = sizeof(cs_install_housing_packet);
	int retval = WSASend(CNetwork::GetInst()->GetSocket(), &m_SendWsaBuf, 1, &size, flag, NULL, NULL);

	if (retval != 0)
	{
		int err_no = WSAGetLastError();
		CNetwork::Err_display("Err while sending packet - ", err_no);
	}
}

void CPacketMgr::Send_Remove_Housing_Packet(USHORT houseId)
{
	cs_remove_housing_packet* remove_housing_packet = reinterpret_cast<cs_remove_housing_packet*>(m_cSendBuf);
	remove_housing_packet->type = CS_HOUSING_REMOVE;
	remove_housing_packet->house_id = houseId;

	DWORD	size{ 0 }, flag{ 0 };
	m_SendWsaBuf.len = sizeof(cs_remove_housing_packet);
	int retval = WSASend(CNetwork::GetInst()->GetSocket(), &m_SendWsaBuf, 1, &size, flag, NULL, NULL);
	if (retval != 0)
	{
		int err_no = WSAGetLastError();
		CNetwork::Err_display("Err while sending packet - ", err_no);
	}
}

void CPacketMgr::Send_Attack_Player_Packet(UINT attackType, USHORT attackId, char eitemType)
{
	cs_attack_packet* attack_packet = reinterpret_cast<cs_attack_packet*>(m_cSendBuf);

	attack_packet->size = sizeof(cs_attack_packet);
	attack_packet->type = CS_ATTACK;
	attack_packet->attack_uiType = attackType;
	attack_packet->attack_id = attackId;
	attack_packet->eType = eitemType;

	DWORD size{ 0 }, flag{ 0 };
	m_SendWsaBuf.len = sizeof(cs_attack_packet);
	int retval = WSASend(CNetwork::GetInst()->GetSocket(), &m_SendWsaBuf, 1, &size, flag, NULL, NULL);

	if (retval != 0) {
		int err_no = WSAGetLastError();
		CNetwork::Err_display("Err sending packet - ", err_no);
	}
}

void CPacketMgr::Send_Animation_Player_Packet(UINT uiAnimationType)
{
	cs_animation_packet* animation_packet = reinterpret_cast<cs_animation_packet*>(m_cSendBuf);
	animation_packet->size = sizeof(cs_animation_packet);
	animation_packet->type = CS_ANIMATION;
	animation_packet->uiType = uiAnimationType;

	DWORD size{ 0 }, flag{ 0 };
	m_SendWsaBuf.len = sizeof(cs_animation_packet);
	int retval = WSASend(CNetwork::GetInst()->GetSocket(), &m_SendWsaBuf, 1, &size, flag, NULL, NULL);

	if (retval != 0) {
		int err_no = WSAGetLastError();
		CNetwork::Err_display("Err while sending packet - ", err_no);
	}
}

void CPacketMgr::Send_Use_Item_Packet(char eType)
{
	cs_use_item_packet* use_item_packet = reinterpret_cast<cs_use_item_packet*>(m_cSendBuf);
	use_item_packet->size = sizeof(cs_use_item_packet);
	use_item_packet->type = CS_USE_ITEM;
	use_item_packet->eType = eType;

	DWORD	size{ 0 }, flag{ 0 };
	m_SendWsaBuf.len = sizeof(cs_use_item_packet);
	int retval = WSASend(CNetwork::GetInst()->GetSocket(), &m_SendWsaBuf, 1, &size, flag, NULL, NULL);

	if (retval != 0) {
		int err_no = WSAGetLastError();
		CNetwork::Err_display("Err while sending pakcet - ", err_no);
	}
}

void CPacketMgr::Send_Equip_Armor_Packet(char eType)
{
	cs_equip_armor_packet* packet = reinterpret_cast<cs_equip_armor_packet*>(m_cSendBuf);
	packet->size = sizeof(cs_equip_armor_packet);
	packet->type = CS_EQUIP_ARMOR;
	packet->eType = eType;

	DWORD	size{ 0 }, flag{ 0 };
	m_SendWsaBuf.len = sizeof(cs_equip_armor_packet);
	int retval = WSASend(CNetwork::GetInst()->GetSocket(), &m_SendWsaBuf, 1, &size, flag, NULL, NULL);

	if (retval != 0) {
		int err_no = WSAGetLastError();
		CNetwork::Err_display("Err while sending pakcet - ", err_no);
	}
}

void CPacketMgr::Send_Destroy_Armor_Packet()
{
	cs_destroy_armor_packet* packet = reinterpret_cast<cs_destroy_armor_packet*>(m_cSendBuf);
	packet->size = sizeof(cs_destroy_armor_packet);
	packet->type = CS_DESTROY_ARMOR;

	DWORD	size{ 0 }, flag{ 0 };
	m_SendWsaBuf.len = sizeof(cs_destroy_armor_packet);
	int retval = WSASend(CNetwork::GetInst()->GetSocket(), &m_SendWsaBuf, 1, &size, flag, NULL, NULL);

	if (retval != 0) {
		int err_no = WSAGetLastError();
		CNetwork::Err_display("Err while sending pakcet - ", err_no);
	}
}
