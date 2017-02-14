#pragma once

#include "Variable.h"
typedef struct _INTER_CLIENT_QUEUE_6_
{
	int			m_nRear;
	int			m_nFront;
	int			m_nCount;

	char		m_chMsg[10][5000];
} INTER_CLIENT_QUEUE_6, *P_INTER_CLIENT_QUEUE_6;

class CClientInterface6
{
public:
	int							m_nRetry;
	int							m_nCommStep;
	int							m_nCommBin;
	int							m_nHeader;
	int							m_nHeaderLength;
	int							m_nRpyBin[100];

	CString						m_strSendMsg;
	CString						m_strErrorMsg;
	CString						m_strHeader;
	CString						m_strData;

	DWORD						m_dwWaitTime[3];

	HANDLE						m_hMemMapInter6;
	P_INTER_CLIENT_QUEUE_6		m_pQueueInter6;

	void						OnCommunication();
	void						OnMessageWrite(CString strMsg);
	CString						OnMessageRead();
	CString						OnCharToString(char chData[], int nLength);
	void						OnStringToChar(CString strData, char chData[]);
	void						OnDataReceive(CString strMsg);
	void						OnDataAnalysis(CString strMsg);
	CString						OnDataItemAnalysis(int nOpt, int nPos, CString strMsg, CString strFind);
	void						OnTesterReady(tagTEST_SITE_INFO &test_site); 
	void						OnSocketInfo(tagTEST_SITE_INFO &test_site);
	void						OnModuleInfo(tagTEST_SITE_INFO &test_site);
	void						OnTesterStart(tagTEST_SITE_INFO &test_site);
	void						OnPgmDownload(tagTEST_SITE_INFO &test_site);
	//void						OnPgmComplete(tagTEST_SITE_INFO test_site);
	void						OnLotComplete(tagTEST_SITE_INFO &test_site);
	//void						OnTesterAlarm(tagTEST_SITE_INFO test_site);
	void						OnTesterOption(tagTEST_SITE_INFO &test_site);
	void						OnTesterAbort(tagTEST_SITE_INFO &test_site);
	// jtkim 20160926
	void						OnChamberDoor(tagTEST_SITE_INFO &test_site, int nStatus);

public:
	CClientInterface6(void);
	~CClientInterface6(void);
};

extern CClientInterface6 clsInterC6;
