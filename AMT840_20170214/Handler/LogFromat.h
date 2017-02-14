#pragma once

#include "Variable.h"


enum LOG_KEY
{
	ALARM_KEY        = 0,
};

typedef struct _LOG_QUEUE_W_
{
	int			m_nRear;
	int			m_nFront;
	int			m_nCount;

	char		m_chMsg[100][1000];
} LOG_QUEUE_W, *P_LOG_QUEUE_W;


class CLogFromat
{
public:
	// log file 관련 
	CString				m_strLogKeyM[100][10];
	CString				m_strLogDataM[100][10];
	CString				m_strLogMaterialIdM[100];
	CString				m_strLogMaterialTypeM[100];
	int					m_nMoveFlag[100];

	HANDLE				m_hMemMapLogW;
	P_LOG_QUEUE_W		m_pQueueLogW;
//	HANDLE				m_hMemMapLogR;
//	P_LOG_QUEUE_R		m_pQueueLogR;
	int					m_nKeyCount[10];
	CString				m_strKey[10][10];
	CString				m_strValue[10][10];
	CString				m_strDevice[10];
	CString				m_strEvent[10];
	CString				m_strAlarmCode;
	
//	bool				m_bThread;

//	HANDLE				m_thrHandle;
	
	int					m_nOldRearCount;
	// alarm  Status(0:발생 1:해제)
	CString				m_strOldAlarm;
	DWORD				m_dwAlarmWaitTime[3];
	void				LogAlarm(CString strID, CString strEvent, CString strCode, int nStatus, int nDataCount, CString *strKey, CString *strData);
	// function  Status(0:start 1:end)
	CString				m_strOldFunc;
	DWORD				m_dwFuncWaitTime[3];
	void				LogFunction(CString strID, CString strEvent, int nStatus, CString strMatID, CString strMatType, int nDataCount, CString *strKey, CString *strData);
	// process  Status(0:start 1:end)
	CString				m_strOldProcess;
	DWORD				m_dwProcessWaitTime[3];
	void				LogProcess(CString strID, CString strEvent, int nStatus, CString strMatID, CString strLotID, CString strRecipe, int nDataCount, CString *strKey, CString *strData);
	// Transfer 
	CString				m_strOldTrans;
	DWORD				m_dwTransWaitTime[3];
	void				LogTransfer(CString strID, CString strEvent, int nStatus, CString strMatID, CString strMatType, CString strStartPos, CString strEndPos, int nDataCount, CString *strKey, CString *strData);
	// lot  Status(0:start 1:end)
	void				LogLotInform(CString strID, CString strEvent, CString strLotID, CString strRecipe, CString strCarrierID, int nDataCount, CString *strKey, CString *strData);
	// Config
	void				LogConfig(CString strID, CString strEvent, CString strRecipe, int nDataCount, CString *strKey, CString *strData);

	void				OnLogWrite(CString strMsg);
	void				OnAddKeyAlarm(CString strDevice, CString strEvent, int nKey, CString *strKey, CString *strValue);
	void				OnLogMotorAlarm(CString strDevice, CString strEvent, int nMotor, int *nAxis, double *dPos);
	void				OnLogAlarm(CString strAlarmCode, int nStatus);
	CString				GetMotorName(int nAxis);
	CString				GetDeviceID(int nAxis);
	CString				GetSiteName(int nSite);
	CString				GetRunStatus();
	void				OnLogMotorMove(int nAxis, int nStatus, double dPos);		//축번호, 움직임 상태 (END, START, ERROR, HOME END, START), 목적위치
	void				OnTransferLog(int nSite, CString strEvent, CString strMatID, CString strMatType, CString strStart, CString strEnd);	//모터 번호, Start/End, 이동하는 자재의 개수, 자재 ID, 자재 타입, 자재 이동시작지점, 도착지점

public:
	int				OnFileSizeCheck(CString strFileName, long lSize, int nCheck);
	void			OnLogTestSite(int nMode, int nSite, int nLfToRi, tagTEST_SITE_INFO &test_site);
	CString			OnCharToString(char chData[], int nLength);
	void			OnStringToChar(CString strData, char chData[]);
public:
	CLogFromat(void);
	~CLogFromat(void);
};

extern CLogFromat clsLog;

