#include "stdafx.h"
#include "Network.h"
#include "PlayerProcess.h"
#include "MonsterProcess.h"
#include "NaturalProcess.h"
#include "HousingProcess.h"

#include "PacketMgr.h"
#include "TimerMgr.h"
#include "DataBase.h"
#include "Player.h"

CNetwork::CNetwork()
{
	m_ListenSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	m_bRunningServer = true;

	// [ Process Init ]
	m_pPlayerProcess = nullptr;
	m_pMonsterProcess = nullptr;
	m_pNaturalProcess = nullptr;
	m_pHousingProcess = nullptr;
	m_UserID = 0;
	serverTimer = high_resolution_clock::now();
	eventTimer = high_resolution_clock::now();
	//------------------------------
	//Initialize();
	CheckThisCputCount();
	CDataBase::GetInst();
}

CNetwork::~CNetwork()
{
	CloseServer();
}

void CNetwork::GetServerIpAddress()
{
	PHOSTENT	hostinfo;
	char		hostname[50];
	char		ipaddr[50];
	memset(hostname, 0, sizeof(hostname));
	memset(ipaddr, 0, sizeof(ipaddr));

	int err_no = gethostname(hostname, sizeof(hostname));
	if (err_no == 0) {
		hostinfo = gethostbyname(hostname);
		strcpy_s(ipaddr, inet_ntoa(*reinterpret_cast<struct in_addr*>(hostinfo->h_addr_list[0])));
	}
	cout << "Server IP Address: " << ipaddr << endl;
}

HANDLE CNetwork::GetIocp()
{
	return m_hIocp;
}

void CNetwork::Initialize()
{
	CProcess::InitBeforeStart();

	// [ Process Init ] 
	m_pPlayerProcess = new CPlayerProcess();
	m_pMonsterProcess = new CMonsterProcess();
	m_pNaturalProcess = new CNaturalProcess();
	m_pHousingProcess = new CHousingProcess();

	CTimerMgr::GetInst()->Reset();

	// ==========================================
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		int err_no = WSAGetLastError();
		Err_display("InitWinSock() err", err_no);
	}

	int retval;

	m_ListenSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_ListenSock == INVALID_SOCKET) {
		int err_no = WSAGetLastError();
		Err_display("Socket Err", err_no);
	}

	SOCKADDR_IN	serveraddr;
	ZeroMemory(&serveraddr, sizeof(SOCKADDR_IN));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVER_PORT);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	retval = ::bind(m_ListenSock, (struct sockaddr*)&serveraddr, sizeof(SOCKADDR_IN));
	if (retval == SOCKET_ERROR) {
		int err_no = WSAGetLastError();
		Err_display("bind()", err_no);
	}
	retval = listen(m_ListenSock, 5);
	if (retval == SOCKET_ERROR) {
		int err_no = WSAGetLastError();
		Err_display("listen()", err_no);
	}
	//
	m_addrLen = sizeof(SOCKADDR_IN);
	memset(&m_clientAddr, 0, m_addrLen);

	//
	m_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	if (m_hIocp == NULL) {
		int err_no = WSAGetLastError();
		Err_display("InitCompletionPort() err", err_no);
	}

	GetServerIpAddress();
}

void CNetwork::StartServer()
{
	for (int i = 0; i < m_iNumWorkerThread; ++i) {
		m_vWorkerThread.push_back(std::shared_ptr<std::thread>(new std::thread{ [&]() {CNetwork::GetInst()->WorkerThread(); } }));
	}
	cout << "WorkerThread Create" << endl;

	m_pAcceptThread = std::shared_ptr<std::thread>(new std::thread{ [&]() {CNetwork::GetInst()->AcceptThread(); } });
	cout << "AcceptThread Create" << endl;

	m_pUpdateThread = std::shared_ptr< std::thread >(new std::thread{ [&]() {CNetwork::GetInst()->UpdateThread(); } });
	cout << "UpdateThread Create" << endl;

	/*m_pDatabaseThread = std::shared_ptr<std::thread>(new std::thread{ [&]() {CNetwork::GetInst()->DataBaseThread(); } });
	cout << "DatabaseThread Create" << endl;*/

	cout << "==============================" << endl;
	cout << "��       Server Start         ��" << endl;
	cout << "==============================" << endl;
}

void CNetwork::CloseServer()
{
	/*m_pDatabaseThread->join();
	cout << "DatabaseThread Close" << endl;*/
	m_pUpdateThread->join();
	cout << "UpdateThread Close" << std::endl;
	m_pAcceptThread->join();
	cout << "AcceptThread Close" << std::endl;

	for (auto& thread : m_vWorkerThread)
		thread->join();
	cout << "WorkerThread Close" << std::endl;

	EndServer();
	Disconnect();

	cout << "==============================" << endl;
	cout << "��       Server Exit          ��" << endl;
	cout << "==============================" << endl;
}

void CNetwork::Disconnect()
{
	closesocket(m_ListenSock);
	CloseHandle(m_hIocp);
	WSACleanup();
}

void CNetwork::CheckThisCputCount()
{
	// CPU , Thread ���� Ȯ��
	SYSTEM_INFO	si; // CPU ���� Ȯ�ο�
	GetSystemInfo(&si); // �ý��� ������ �޾ƿ´�.
	m_iCpuCore = static_cast<int>(si.dwNumberOfProcessors);
	m_iNumWorkerThread = static_cast<int>(m_iCpuCore * 2 - 2);
	cout << "CPU Core Count: " << m_iCpuCore << "\tThread: " << m_iNumWorkerThread << endl;
}

void CNetwork::WorkerThread()
{
	DWORD		num_byte;
	ULONGLONG	key64;
	PULONG_PTR	p_key = &key64;
	while (m_bRunningServer) {
		OVER_EX*	lpover_ex;

		BOOL	is_error = GetQueuedCompletionStatus(m_hIocp, &num_byte, p_key,
			reinterpret_cast<LPWSAOVERLAPPED *>(&lpover_ex), INFINITE);

		USHORT id = static_cast<unsigned>(key64);

		// ������ ���� : FALSE, ���� ����Ʈ ũ�� = 0
		// ���� ���� : TRUE, ���� ����Ʈ ũ�� = 0
		if (is_error == false)
		{
			int err_no = WSAGetLastError();
			if (err_no != WSA_IO_PENDING){
				std::cout << "[ Player: " << id << " ] Disconnect" << std::endl;
				m_pPlayerProcess->PlayerLogout(id);
			}
			continue;
		}

		if (num_byte == 0)
		{
			std::cout << "[ Player: " << id << " ] Disconnect" << std::endl;
			m_pPlayerProcess->PlayerLogout(id);
			continue;
		}
		switch (lpover_ex->m_Event)
		{
		case EV_RECV:
		{
			m_pPlayerProcess->RecvPacket(id, lpover_ex->m_MessageBuffer, num_byte);
			break;
		}
		case EV_SEND:
		{
			// Send ������ �߻��ϸ� �÷��̾ �����Ų��.
			if (num_byte != lpover_ex->m_DataBuffer.len) {
				int err_no = WSAGetLastError();
				Err_display("[Worker Thread]Send Error: ",err_no);
				m_pPlayerProcess->PlayerLogout(id);
			}
			delete lpover_ex;
			break;
		}
		case EV_MONSTER_UPDATE:
		{
			m_pMonsterProcess->UpdateMonster(lpover_ex->m_Status, id, lpover_ex->m_usOtherID);
			delete lpover_ex;
			break;
		}
		case EV_NATURAL_UPDATE:
		{
			m_pNaturalProcess->UpdateNatural(lpover_ex->m_Status, id, lpover_ex->m_usOtherID);
			delete lpover_ex;
			break;
		}
		case EV_DB:
		{
			delete lpover_ex;
			break;
		}
		default:
		{
			cout << "Unknown Event Type :" << lpover_ex->m_Event << endl;
			while (true);
		}
		}
	}
}

void CNetwork::AcceptThread()
{
	while (m_bRunningServer) {
		auto client_sock = WSAAccept(m_ListenSock, (struct sockaddr*)&m_clientAddr, &m_addrLen, NULL, NULL);

		if (client_sock == INVALID_SOCKET) {
			int err_no = WSAGetLastError();
			Err_display("ACCEPT INVALID_SOCKET!", err_no);
			break;
		}
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(client_sock), m_hIocp, m_UserID, 0);
		//cout << "���� �ο�: " << m_usUserID + 1 << endl;
		//cout << "���� : " << inet_ntoa(m_clientAddr.sin_addr) << '\t' << ntohs(m_clientAddr.sin_port) << endl;
		m_pPlayerProcess->AcceptClient(client_sock, m_UserID);
		++m_UserID;
	}
}

void CNetwork::UpdateThread()
{
	// ���� �ð��� ���������� ������Ʈ�� ���ֶ�� �Լ� �κ�
	if (CTimerMgr::GetInst()) {
		CTimerMgr::GetInst()->Start();
	}
	while (m_bRunningServer)
	{
		CTimerMgr::GetInst()->Tick();
		/*while (CProcess::EmptyEventQueue()) {
			this_thread::sleep_for(10ms);
		}*/
		if (high_resolution_clock::now() - serverTimer >= 1s)
		{
			CProcess::Time_Event();
			serverTimer = high_resolution_clock::now();
		}

		if (high_resolution_clock::now() - eventTimer >= 30s)
		{
			CProcess::Weather_Event();
			eventTimer = high_resolution_clock::now();
		}

		while (!CProcess::EmptyEventQueue())
		{
			Update_Event ev;
			if (CProcess::CheckEventStart(ev))
			{
				OVER_EX*	pOver_ex = new OVER_EX;
				ZeroMemory(&pOver_ex->m_Overlapped, sizeof(WSAOVERLAPPED));
				pOver_ex->m_Event = ev.m_EventType;
				pOver_ex->m_Status = ev.m_ObjState;
				pOver_ex->m_usOtherID = ev.m_From_Object;
				PostQueuedCompletionStatus(m_hIocp, 1, ev.m_Do_Object, &pOver_ex->m_Overlapped);
			}
			else
				break;
		}
	}
}

void CNetwork::DataBaseThread()
{
	while (m_bRunningServer)
	{
		this_thread::sleep_for(10ms);
		while (!CDataBase::GetInst()->EmptyDatabaseEventQueue())
		{
			DB_Event db_ev;
			if (CDataBase::GetInst()->PopDatabaseEventQueue(db_ev))
				CDataBase::GetInst()->RunDataBase(db_ev);
			else
				break;
		}
	}
}

void CNetwork::Err_quit(const char* msg, int err_no)
{
	WCHAR *lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCWSTR)lpMsgBuf, (LPCWSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(-1);
}

void CNetwork::Err_display(const char* msg, int err_no)
{
	WCHAR *lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	cout << "[ " << msg << " ]";
	wcout << L"���� " << lpMsgBuf << endl;
	LocalFree(lpMsgBuf);
	while (true);
}