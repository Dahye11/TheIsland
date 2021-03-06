#pragma once
#include "stdafx.h"

class CPlayerProcess;
class CMonsterProcess;
class CNaturalProcess;
class CHousingProcess;
class CEtcProcess;

class CNetwork
{
	SINGLE(CNetwork)

private:
	vector<shared_ptr<thread>>		m_vWorkerThread;
	shared_ptr<thread>				m_pAcceptThread;
	shared_ptr<thread>				m_pUpdateThread;

private:
	HANDLE			m_hIocp;
	SOCKET			m_ListenSock;
	SOCKADDR_IN		m_clientAddr;
	int				m_addrLen;
	int				m_iCpuCore;
	int				m_iNumWorkerThread;
	volatile bool	m_bRunningServer;

private:
	CPlayerProcess*		m_pPlayerProcess;
	CMonsterProcess*	m_pAnimalProcess;
	CNaturalProcess*	m_pNaturalProcess;
	CHousingProcess*	m_pHousingProcess;
	CEtcProcess*		m_pEtcProcess;

private:
	USHORT GetUserID();

public:
	void Initialize();
	void StartServer();
	void CloseServer();
	void Disconnect();

	void CheckThisCputCount();

	void WorkerThread();
	void AcceptThread();
	void UpdateThread();

	void EndServer() { m_bRunningServer = false; }
	bool GetServerState() { return m_bRunningServer; }
	void GetServerIpAddress();

public:
	void Init_Process();
	void Release_Process();

public:
	HANDLE GetIocp();

public:
	static void Err_quit(const char* msg, int err_no);
	static void Err_display(const char* msg, int err_no);
};

