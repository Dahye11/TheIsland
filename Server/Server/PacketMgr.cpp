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

void CPacketMgr::Send_Packet(const unsigned short& _usID, void* _packet)
{
	char* packet = reinterpret_cast<char*>(_packet);
	int packet_size = packet[0];

	OVER_EX* send_over = new OVER_EX;
	memset(send_over, 0x00, sizeof(OVER_EX));
	send_over->m_Event = EV_SEND;
	memcpy(send_over->m_MessageBuffer, packet, packet_size);
	send_over->m_DataBuffer.buf = reinterpret_cast<char*>(send_over->m_MessageBuffer);
	send_over->m_DataBuffer.len = packet_size;

	int retval = WSASend(CProcess::m_pPlayerPool->m_cumPlayerPool[_usID]->GetPlayerSocket(), &send_over->m_DataBuffer, 1, NULL, 0, &send_over->m_Overlapped, NULL);

	if (retval == SOCKET_ERROR) {
		int err_no = WSAGetLastError();
		if (err_no != WSA_IO_PENDING) {
			CNetwork::Err_display("SEND EVENT SOCKET_ERR - ", err_no);
		}
	}
}

void CPacketMgr::Send_Login_OK_Packet(unsigned short _usID)
{
	sc_login_ok_packet packet;
	char* ip = CNetwork::GetInst()->GetIpAddr();
	cout << ip << endl;
	packet.size = sizeof(packet);
	packet.type = SC_LOGIN_OK;
	packet.id = _usID;
	strcpy_s(packet.ip, ip);
	Send_Packet(_usID, &packet);
}

void CPacketMgr::Send_Login_Fail_Packet(unsigned short _usID)
{
	sc_login_fail_packet packet;
	packet.size = sizeof(packet);
	packet.type = SC_LOGIN_FAIL;
	Send_Packet(_usID, &packet);
}

void CPacketMgr::Send_Accept_Packet(unsigned short _usID, concurrent_unordered_set<unsigned short>& _ViewList)
{
	sc_accept_packet	packet;
	packet.size = sizeof(sc_accept_packet);
	packet.type = SC_CONNECT;
	packet.id = _usID;

	// �����Ͽ� �ִ� �÷��̾�鿡�� ���� �÷��̾ �������� ViewList�� �߰���Ų��.
	for (auto& au : _ViewList)
	{
		if (au == _usID) 
			continue;
		Send_Packet(au, &packet);
	}
}

void CPacketMgr::Send_Status_Packet(unsigned short _usPlayer, unsigned short _usOther)
{
}

void CPacketMgr::Send_Pos_Packet(unsigned short _usPlayer, unsigned short _usOther)
{
}

void CPacketMgr::Send_Look_Packet(unsigned short _usPlayer)
{
}

void CPacketMgr::Send_Remove_Packet(unsigned short _usPlayer, unsigned short _usOther)
{
	if (_usPlayer != _usOther)
	{
		sc_disconnect_packet packet;
		ZeroMemory(&packet, sizeof(sc_disconnect_packet));
		packet.size = sizeof(sc_disconnect_packet);
		packet.type = SC_DISCONNECT;
		packet.id = _usPlayer;
		Send_Packet(_usOther, &packet);
	}
}

void CPacketMgr::Send_Disconnect_Packet(unsigned short _usPlayer)
{
	concurrent_unordered_set<unsigned short> list;

	sc_disconnect_packet	packet;
	packet.size = sizeof(sc_disconnect_packet);
	packet.type = SC_DISCONNECT;
	packet.id = _usPlayer;

	for (auto& au : list)
	{
		if (au == _usPlayer)
			continue;
	}
}
