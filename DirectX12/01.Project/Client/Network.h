#pragma once
#include "header.h"

class CGameObject;
class CScene;

class CNetwork
{
	SINGLE(CNetwork)

private:
	SOCKET	m_sock;
	WSABUF	m_RecvWsaBuf;
	char	m_cRecvbuf[1024];

	DWORD	m_in_packet_size;
	DWORD	m_saved_packet_size;
	char	m_cPacketBuf[255];

	bool m_bLogin;
	bool m_bClientClose;

	bool m_bHousingCheck;

	float m_fServerTime;

private:
	function<void(char*)> m_fpPacketProcess[SC_END];

private:
	void BindfpPacket();
public:
	CScene* pScene;
	static unsigned int m_usID;
	static CGameObject*	m_pPlayer;
	static CGameObject*	m_pChat;

	HWND	m_hWnd;

public:
	void Init(HWND hWnd);
public:
	SOCKET&	GetSocket() { return m_sock; }
	float GetTime()const { return m_fServerTime; }
public:
	static void Err_quit(const char* msg, int err_no);
	static void Err_display(const char* msg, int err_no);
public:
	bool ConnectServer(string ipAddr);
	void DisConnect(SOCKET socket);
	void RecvPacket();

public:
	// - Player, Animal Common
	void Recv_Pos_Packet(char* packet);
	void Recv_Rot_packet(char* packet);
	void Recv_Remove_Packet(char* packet);
	void Recv_Animation_Packet(char* packet);
	// - Login
	void Recv_Login_OK_Packet(char* packet);
	void Recv_Login_Fail_Packet(char* packet);
	void Recv_Full_Server_Packet(char* packet);
	void Recv_Disconnect_Server_Packet(char* packet);

	// - Player
	void Recv_Status_Player_Packet(char* packet);
	void Recv_Chat_Packet(char* packet);

public: // Natural
	void Recv_Put_Natural_Packet(char* packet);
	void Recv_Destroy_Natural_Packet(char* packet);
	
public: // Housing
	void Recv_Install_Housing_Packet(char* packet);
	void Recv_Remove_Housing_Packet(char* packet);
	void Recv_Check_Housing_Packet(char* packet);
	void Recv_Upgrade_Housing_Packet(char* packet);

public:
	void Recv_Add_Item_Packet(char* packet);

public: //etc
	void Recv_Weather_Packet(char* packet);
	void Recv_Time_Packet(char* packet);

public:
	void SetHwnd(HWND hWnd) { m_hWnd = hWnd; }
	void SetChatObj(CGameObject* pObj) { m_pChat = pObj; }
	void SetPlayerObj(CGameObject* pObj) { m_pPlayer = pObj; }

	void SetHousingCheck(bool bCheck) { m_bHousingCheck = bCheck; }
	bool GetHousingCheck() { return m_bHousingCheck; }
};