#include "stdafx.h"
#include "Network.h"
#include "LoginScene.h"
#include "IngameScene.h"

#include <Engine/Device.h>

#include <Engine/Ptr.h>

#include <Engine/Texture.h>

#include <Engine/ResMgr.h>
#include <Engine/CollisionMgr.h>
#include <Engine/FontMgr.h>

#include <Engine/GameObject.h>
#include <Engine/MeshRender.h>
#include <Engine/Transform.h>
#include <Engine/Camera.h>
#include <Engine/Light3D.h>
#include <Engine/LandScape.h>
#include <Engine/GridScript.h>

#include <Engine/ToolCamScript.h>

#include "PlayerCamScript.h"
#include "InventoryScript.h"
#include "ChatScript.h"
#include <Engine/TestScript.h>
#include "InputScript.h"
#include "AnimalScript.h"
#include <Engine/Animator3D.h>
#include "SunshineScript.h"
#include "StatusScript.h"

#include <Engine/NaviMgr.h>
#include <Engine/Layer.h>

unsigned int CNetwork::m_usID = 0;
CGameObject*	CNetwork::m_pPlayer;
CGameObject*	CNetwork::m_pChat;

CNetwork::CNetwork()
{
	
}

CNetwork::~CNetwork()
{
	//DisConnect();
}

void CNetwork::BindfpPacket()
{
	// - Common
	m_fpPacketProcess[SC_POS] = [&](char* packet) {Recv_Pos_Packet(packet); };
	m_fpPacketProcess[SC_ROT] = [&](char* packet) {Recv_Rot_packet(packet); };
	m_fpPacketProcess[SC_REMOVE] = [&](char* packet) {Recv_Remove_Packet(packet); };
	m_fpPacketProcess[SC_ANIMATION] = [&](char* packet) {Recv_Animation_Packet(packet); };
	// - Login
	m_fpPacketProcess[SC_LOGIN_OK] = [&](char* packet) {Recv_Login_OK_Packet(packet); };
	m_fpPacketProcess[SC_LOGIN_FAIL] = [&](char* packet) {Recv_Login_Fail_Packet(packet); };
	m_fpPacketProcess[SC_FULL_SERVER] = [&](char* packet) {Recv_Full_Server_Packet(packet); };
	m_fpPacketProcess[SC_DISCONNECT_SERVER] = [&](char* packet) {Recv_Disconnect_Server_Packet(packet); };
	// - Player
	m_fpPacketProcess[SC_STATUS_PLAYER] = [&](char* packet) {Recv_Status_Player_Packet(packet); };
	m_fpPacketProcess[SC_CHAT] = [&](char* packet) {Recv_Chat_Packet(packet); };
	// - Natural
	m_fpPacketProcess[SC_PUT_NATURAL] = [&](char* packet) {Recv_Put_Natural_Packet(packet); };
	m_fpPacketProcess[SC_DESTROY_NATURAL] = [&](char* packet) {Recv_Destroy_Natural_Packet(packet); };
	// - House
	m_fpPacketProcess[SC_INSTALL_HOUSE] = [&](char* packet) {Recv_Install_Housing_Packet(packet); };
	m_fpPacketProcess[SC_REMOVE_HOUSE] = [&](char* packet) {Recv_Remove_Housing_Packet(packet); };
	m_fpPacketProcess[SC_CHECK_HOUSE] = [&](char* packet) {Recv_Check_Housing_Packet(packet); };
	m_fpPacketProcess[SC_UPGRADE_HOUSE] = [&](char* packet) {Recv_Upgrade_Housing_Packet(packet); };
	// - Item
	m_fpPacketProcess[SC_ADD_ITEM] = [&](char* packet) {};
	m_fpPacketProcess[SC_REMOVE_ITEM] = [&](char* packet) {};
	// - Etc
	m_fpPacketProcess[SC_WEATHER] = [&](char* packet) {Recv_Weather_Packet(packet); };
	m_fpPacketProcess[SC_TIME] = [&](char* packet) {Recv_Time_Packet(packet); };
}

void CNetwork::Init(HWND hWnd)
{
	m_bLogin = false;
	m_bClientClose = true;
	m_RecvWsaBuf.buf = m_cRecvbuf;
	m_RecvWsaBuf.len = BUF_SIZE;
	m_in_packet_size = 0;
	m_saved_packet_size = 0;
	SetHwnd(hWnd);
	BindfpPacket();
}

void CNetwork::Err_quit(const char * msg, int err_no)
{
	WCHAR *lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCWSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(-1);
}

void CNetwork::Err_display(const char * msg, int err_no)
{
	WCHAR *lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	cout << "[ " << msg << " ]";
	wcout << L"에러 " << lpMsgBuf << endl;
	//while (true);
	LocalFree(lpMsgBuf);
}

bool CNetwork::ConnectServer(string ipAddr)
{
	char IP[15];
	strcpy_s(IP, ipAddr.c_str());

	WSADATA	wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	m_sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(SOCKADDR_IN));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVER_PORT);
	serveraddr.sin_addr.s_addr = inet_addr(IP);
	int retval = WSAConnect(m_sock, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr), NULL, NULL, NULL, NULL);
	if (retval == SOCKET_ERROR) {
		int err_no = WSAGetLastError();
		//Err_display("socket err", err_no);
		return false;
	}
	WSAAsyncSelect(m_sock, m_hWnd, WM_SOCKET, FD_CLOSE | FD_READ);
	m_bClientClose = false;
	return true;
}

void CNetwork::DisConnect(SOCKET socket)
{
	closesocket(socket);
	WSACleanup();
	cout << "DisConnect" << endl;
}

void CNetwork::RecvPacket()
{
	if (!m_bClientClose) {
		DWORD iobyte, ioflag = 0;

		int ret = WSARecv(m_sock, &m_RecvWsaBuf, 1, &iobyte, &ioflag, NULL, NULL);
		if (ret) {
			int err_no = WSAGetLastError();
			Err_display("Recv Error", err_no);
		}

		BYTE *ptr = reinterpret_cast<BYTE*>(m_cRecvbuf);

		while (0 != iobyte) {
			if (0 == m_in_packet_size) m_in_packet_size = ptr[0];
			if (iobyte + m_saved_packet_size >= m_in_packet_size) {
				memcpy(m_cPacketBuf + m_saved_packet_size, ptr, m_in_packet_size - m_saved_packet_size);
				m_fpPacketProcess[m_cPacketBuf[1]](m_cPacketBuf);
				ptr += m_in_packet_size - m_saved_packet_size;
				iobyte -= m_in_packet_size - m_saved_packet_size;
				m_in_packet_size = 0;
				m_saved_packet_size = 0;
			}
			else {
				memcpy(m_cPacketBuf + m_saved_packet_size, ptr, iobyte);
				m_saved_packet_size += iobyte;
				iobyte = 0;
			}
		}
	}
}

void CNetwork::Recv_Pos_Packet(char * packet)
{
	sc_pos_packet* pos_packet = reinterpret_cast<sc_pos_packet*>(packet);
	USHORT usId = pos_packet->usId;
	Vec3 vPos = pos_packet->vPos;
	Vec3 vRot = pos_packet->vRot;

	if(usId < MAX_USER)
		dynamic_cast<CIngameScene*>(pScene->GetSceneScript())->PlayerUpdate(usId, vPos, vRot);
	else if(usId < END_ANIMAL)
		dynamic_cast<CIngameScene*>(pScene->GetSceneScript())->AnimalUpdate(usId, vPos, vRot);
}
void CNetwork::Recv_Rot_packet(char * packet)
{
	sc_rot_packet* rot_packet = reinterpret_cast<sc_rot_packet*>(packet);
	USHORT usId = rot_packet->usId;
	Vec3 vRot = rot_packet->vRot;

	if (usId < MAX_USER)
		dynamic_cast<CIngameScene*>(pScene->GetSceneScript())->PlayerRotUpdate(usId, vRot);
	else if (usId < END_ANIMAL)
		dynamic_cast<CIngameScene*>(pScene->GetSceneScript())->AnimalRotUpdate(usId, vRot);

}
void CNetwork::Recv_Remove_Packet(char * packet)
{
	sc_remove_packet* remove_packet = reinterpret_cast<sc_remove_packet*>(packet);
	USHORT usId = remove_packet->usId;

	cout << usId << endl;
	if(usId < MAX_USER)
		dynamic_cast<CIngameScene*>(pScene->GetSceneScript())->PlayerDestroy(usId);
	else if(usId < END_ANIMAL)
		dynamic_cast<CIngameScene*>(pScene->GetSceneScript())->AnimalDestory(usId);
}
void CNetwork::Recv_Animation_Packet(char * packet)
{
	sc_animation_packet* animation_packet = reinterpret_cast<sc_animation_packet*>(packet);
	USHORT id = animation_packet->usId;
	UINT uiType = animation_packet->uiType;

	if (id < MAX_USER)
		dynamic_cast<CIngameScene*>(pScene->GetSceneScript())->PlayerAnimationUpdate(id, uiType);
	else
		dynamic_cast<CIngameScene*>(pScene->GetSceneScript())->AnimalAnimationUpdate(id, uiType);
}

void CNetwork::Recv_Login_OK_Packet(char * packet)
{
	pScene = CSceneMgr::GetInst()->GetCurScene();
	sc_login_ok_packet* login_packet = reinterpret_cast<sc_login_ok_packet*>(packet);
	m_usID = login_packet->id;
	if (!m_bLogin) {
		dynamic_cast<CLoginScene*>(pScene->GetSceneScript())->NextScene();
		m_bLogin = true;
	}

	if (m_pPlayer != nullptr) {
		m_pPlayer->Transform()->SetLocalPos(login_packet->vPos);
	}
	cout << "Login Success" << endl;
}

void CNetwork::Recv_Login_Fail_Packet(char * packet)
{
	sc_login_fail_packet* login_packet = reinterpret_cast<sc_login_fail_packet*>(packet);
	cout << "Login Failed" << endl;
}

void CNetwork::Recv_Full_Server_Packet(char * packet)
{
	sc_full_server_packet* full_server_packet = reinterpret_cast<sc_full_server_packet*>(packet);
	cout << "Full Server" << endl;
}

void CNetwork::Recv_Disconnect_Server_Packet(char * packet)
{
	sc_disconnect_server_packet* disconnect_server_packet = reinterpret_cast<sc_disconnect_server_packet*>(packet);
	cout << "Server Exit" << endl;
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		CSceneMgr::GetInst()->GetCurScene()->GetLayer(i)->RemoveAll();
	}
	PostQuitMessage(0);
	return;
}

void CNetwork::Recv_Status_Player_Packet(char * packet)
{
	sc_status_player_packet* status_player_packet = reinterpret_cast<sc_status_player_packet*>(packet);
	float fHealth = status_player_packet->fHealth;
	float fHungry = status_player_packet->fHungry;
	float fThrist = status_player_packet->fThrist;

	CGameObject* pStatus = CSceneMgr::GetInst()->GetCurScene()->GetLayer(30)->FindObject(L"Player Status");

	CStatusScript* pScript = pStatus->GetScript<CStatusScript>();
	pScript->SetHealth(fHealth);
	pScript->SetHungry(fHungry);
	pScript->SetThirst(fThrist);
}

void CNetwork::Recv_Chat_Packet(char * packet)
{
	sc_chat_packet* chat_packet = reinterpret_cast<sc_chat_packet*>(packet);

	string name = chat_packet->ID;
	string Msg(chat_packet->meesage);

	dynamic_cast<CIngameScene*>(pScene->GetSceneScript())->ChatUpdate(name, Msg);
	
}

void CNetwork::Recv_Put_Natural_Packet(char * packet)
{
	sc_put_natural_packet* put_natural_packet = reinterpret_cast<sc_put_natural_packet*>(packet);
	float fHealth = put_natural_packet->fHealth;
	bool bDestroy = put_natural_packet->bDestroy;
	USHORT natural_id = put_natural_packet->natural_id;
	Vec3 vLocalPos = put_natural_packet->vLocalPos;
	Vec3 vLocalScale = put_natural_packet->vLocalScale;
	Vec3 vOffsetPos = put_natural_packet->vOffsetPos;
	Vec3 vOffsetScale = put_natural_packet->vOffsetScale;

	UINT uiType = 0;
	Vec3 vLocalRot = Vec3(0.f, 0.f, 0.f);

	// 추가
	dynamic_cast<CIngameScene*>(pScene->GetSceneScript())->InstallNatural(uiType, natural_id, vLocalPos, vLocalRot, vLocalScale, vOffsetPos, vOffsetScale, fHealth, bDestroy);
}

void CNetwork::Recv_Destroy_Natural_Packet(char * packet)
{
	sc_destroy_natural_packet* destroy_natural_packet = reinterpret_cast<sc_destroy_natural_packet*>(packet);
	USHORT natural_id = destroy_natural_packet->natural_id;
	Vec3	Rot = destroy_natural_packet->vOriginRot;

	// 제거
	dynamic_cast<CIngameScene*>(pScene->GetSceneScript())->DestroyNatural(natural_id);
}

void CNetwork::Recv_Install_Housing_Packet(char * packet)
{
	sc_install_housing_packet* install_housing_packet = reinterpret_cast<sc_install_housing_packet*>(packet);
	USHORT house_id = install_housing_packet->house_id;
	UINT grade = install_housing_packet->house_grade;
	UINT type = install_housing_packet->housing_type;
	Vec3 vPos = install_housing_packet->vLocalPos;
	Vec3 vRot = install_housing_packet->vLocalRot;
	Vec3 vScale = install_housing_packet->vLocalScale;

	// INSTALL 전송
	dynamic_cast<CIngameScene*>(pScene->GetSceneScript())->InstallHousing(type, grade, house_id, vPos, vRot, vScale);
}

void CNetwork::Recv_Remove_Housing_Packet(char * packet)
{
	sc_remove_housing_packet* remove_housing_packet = reinterpret_cast<sc_remove_housing_packet*>(packet);
	USHORT house_id = remove_housing_packet->house_id;

	// REMOVE 
	dynamic_cast<CIngameScene*>(pScene->GetSceneScript())->DestroyHousing(house_id);
}

void CNetwork::Recv_Check_Housing_Packet(char * packet)
{
	sc_check_housing_packet* check_housing_packet = reinterpret_cast<sc_check_housing_packet*>(packet);
	bool bCheck = check_housing_packet->bCheck;
	char eItemType = check_housing_packet->eItemType;
	int iCount = check_housing_packet->iCount;
	m_bHousingCheck = bCheck;

	if(bCheck)
		dynamic_cast<CIngameScene*>(pScene->GetSceneScript())->DecreaseItem(eItemType, iCount);
}

void CNetwork::Recv_Upgrade_Housing_Packet(char * packet)
{
	sc_upgrade_housing_packet*	upgrade_housing_packet = reinterpret_cast<sc_upgrade_housing_packet*>(packet);
	USHORT house_id = upgrade_housing_packet->house_id;
	UINT uiGrade = upgrade_housing_packet->house_grade;
	char eType = upgrade_housing_packet->house_type;

	dynamic_cast<CIngameScene*>(pScene->GetSceneScript())->UpdateHousing(eType, house_id, uiGrade);
}

void CNetwork::Recv_Add_Item_Packet(char * packet)
{
	sc_add_item_packet* add_item_packet = reinterpret_cast<sc_add_item_packet*>(packet);
	char eType = add_item_packet->eItemType;
	int iAmount = add_item_packet->iAmount;

	// 아이템 추가하는 부분
}

void CNetwork::Recv_Weather_Packet(char * packet)
{
	sc_weather_packet* weather_packet = reinterpret_cast<sc_weather_packet*>(packet);
	bool bRain = weather_packet->bRain;

	CGameObject* pSunshineObject = pScene->GetLayer( 0 )->FindObject( L"Sunshine" );

	if ( pSunshineObject )
	{
		CSunshineScript* pSunshineScript = pSunshineObject->GetScript<CSunshineScript>();
		pSunshineScript->SetRainDrop( bRain );
	}

}

void CNetwork::Recv_Time_Packet(char * packet)
{
	sc_time_packet* time_packet = reinterpret_cast<sc_time_packet*>(packet);

	CGameObject* pSunshineObject = pScene->GetLayer(0)->FindObject(L"Sunshine");

	if (pSunshineObject)
	{
		CSunshineScript* pSunshineScript = pSunshineObject->GetScript<CSunshineScript>();
		pSunshineScript->SetTime(time_packet->iday, time_packet->ihour, time_packet->imin);
	}
}
