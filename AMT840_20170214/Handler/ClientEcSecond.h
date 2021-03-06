#pragma once

#include "Variable.h"
typedef struct _EC_S_CLIENT_QUEUE_
{
	int			m_nRear;
	int			m_nFront;
	int			m_nCount;

	char		m_chMsg[50][5000];
} EC_S_CLIENT_QUEUE, *P_EC_S_CLIENT_QUEUE;

class CClientEcSecond
{
public:
	HANDLE					m_hMemMapEcSecond;
	P_EC_S_CLIENT_QUEUE		m_pQueueEcSecond;

	int						m_nHeader;
	int						m_nHeaderLength;
	int						m_nTestEndFlag;
	int						m_nOnlineEndFlag;
	int						m_nScrapInfoFlag;
	int						m_nSeeLotFlag;
	int						m_nDelScrapFlg;
	int						m_nLotEndFlag;
	int						m_nVerFlag;
	int						m_nStatusFlag;
	int						m_nModeChangFlag;
	int						m_nFullCarrierFlag;
	int						m_nEmptyCarrierFlag;
	int						m_nToolChangFlag;
	int						m_nMatchFlag;
	int						m_nDateFlag;
	int						m_nLotDisplayFlag;
	int						m_nLotStartFlag;
	int						m_nLotCancelFlag;
	// jtkim 20150806
	int						m_nPcbInfoFlag;
	CString					m_strSerial;
	CString					m_strPpid;
	CString					m_strWwn;
	CString					m_strCserial;
	CString					m_strPsid;

	CString					m_strHeader;
	CString					m_strData;
	CString					m_strEqpID;
	// 
	void					OnEqpID(CString strEqpID);
	void					OnEcLotDisplay(CString strLotNo, CString strOperID);
	void					OnEcLotStart(CString strLotNo, CString strOperID, int nSocket);
	void					OnEcOnlineEnd(CString strOperID, tagLOT_DISPLAY_INFO lot_display);
	void					OnEcLotEnd(CString strLotNo, CString strOperID, CString strSplitLotNo, int nPass, CTime tStart, int nBdTime, int nBdTimeCount, tagLOT_DISPLAY_INFO lot_display);

	void					OnEcLotCancel(CString strLotNo, CString strOperID);
	void					OnEcScrapInfo(CString strLotNo);
	void					OnEcDeleteScrap(CString strLotNo, CString strSerial);
	// jtkim 20150805
	void					OnEcPcbInfo(CString strLotId, CString strPartId, CString strSerial);
	void					OnOpen();
	void					OnClose();
	void					OnEcSLotEnd(tagPCB_INFO pcb_info);
	void					OnEcTestEnd(CString strLotId, CString strPartId);
	//void					OnEcOnlineEnd(tagLOT_DISPLAY_INFO lot_display);
	//void					OnEcScrapInfo(CString strLotId);
	void					OnEcSeeLot(CString strLotId);
	//void					OnEcDeleteScrap(CString strLotId, CString strSerial, CString strArray);
	//void					OnEcLotEnd(CString strLotId);
	void					OnEqpVersionUpdate(CString strDescrip);
	void					OnEqpStatusChange(CString strMode, CString strCode, CString strDescrip);
	void					OnEqpModeChange(CString strMode);
	void					OnEqpFullCarrierMove(CString srtCarrierId);
	void					OnEqpEmptyCarrierMove();
	void					OnEqpToolChange(CString strToolId);
	void					OnEqpMatch(CString strLotId, CString strCarrierId);
	void					OnEqpDate();
	void					OnCommunication();
	void					OnMessageWrite(CString strMsg);
	CString					OnMessageRead();
	CString					OnCharToString(char chData[], int nLength);
	void					OnStringToChar(CString strData, char chData[]);
	void					OnDataReceive(CString strMsg);
	void					OnDataAnalysis(CString strMsg);
	CString					OnDataItemAnalysis(int nOpt, int nPos, CString strMsg, CString strFind);

public:
	bool					m_bThread;

	int						m_nRetry;
	int						m_nCommStep;
	int						m_nCommBin;

	CString					m_strSendMsg;
	CString					m_strErrorMsg;

	DWORD					m_dwWaitTime[3];

	HANDLE					m_thrHandle;

public:
	CClientEcSecond(void);
	~CClientEcSecond(void);
};

extern CClientEcSecond clsEcSecond;