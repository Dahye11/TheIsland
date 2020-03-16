#pragma once
#include "stdafx.h"

class CDataBase
{
	DECLARE_SINGLE(CDataBase)

private:
	SQLHENV		m_hEnv;
	SQLHDBC		m_hDbc;
	SQLHSTMT	m_hStmt;

	concurrency::concurrent_queue<DataBase_Event>m_ccqDataBaseQueue;
	concurrency::concurrent_queue<DataBase_Event>m_ccqSaveStateQueue;

	std::function<void(DataBase_Event&)>m_fpDataBaseProcess[DB_END];
	std::vector<User_Data> m_UserData;

public:
	void HandleDiagnosticRecord(SQLHANDLE _hHandle, SQLSMALLINT _hType, RETCODE RetCode);
	bool ConnectDataBase();
	void DisConnectDataBase();
	void RunDataBase(DataBase_Event& event);
public:
	void LogInPorcess(DataBase_Event& event);
	void LogOutPorcess(DataBase_Event& event);

public:
	void BindDataBaseFP();
	bool DataBaseQueueIsEmpty();
	void InsertToDataBaseQueue(DataBase_Event& event);
	bool PopFromDataBaseQueue(DataBase_Event& event);
	void InsertToStateQueue(DataBase_Event& event);
	bool PopFromStateQueue(DataBase_Event& event);

	bool	GetIsLogin(std::string sID);
	bool	GetIsLogin(unsigned short usID);
	void	SetIsLogin(int iPlayerNum, unsigned short usID, std::string ID);
	int		FindIsLogin(unsigned short usID, wchar_t* user_id, bool bDelete = false);
};

