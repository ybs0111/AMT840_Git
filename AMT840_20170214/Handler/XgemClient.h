#pragma once

typedef struct _XGEM_CLIENT_QUEUE_
{
	int			m_nRear;
	int			m_nFront;
	int			m_nCount;

	char		m_chMsg[50][5000];
} XGEM_CLIENT_QUEUE, *P_XGEM_CLIENT_QUEUE;

class CXgemClient
{
public:
	HANDLE					m_hMemMapXgem;
	P_XGEM_CLIENT_QUEUE		m_pQueueXgem;

	int						m_nRetry;
	int						m_nCommStep;
	int						m_nCommBin;

	CString					m_strSendMsg;
	CString					m_strErrorMsg;

	DWORD					m_dwWaitTime[3];
	void				OnCommunication();
	void				OnMessageWrite(CString strMsg);
	CString				OnMessageRead();
	CString				OnCharToString(char chData[], int nLength);
	void				OnStringToChar(CString strData, char chData[]);
	// jtkim 20170130
	void				OnMcSocket(int nOnOff, int nX, int nY, CString strSerial);
	void				OnMcSocket(int nOnOff, int nSocket);
	void				OnMcTrayIn(int nMode, CString strLotNo, CString strPartNo, int nTrayID);//Tray를 받기 시작할 때 올려준다.(없음)CEID 9803,9804
	void				OnMcBarcode(int nMode, int nTrayID, int nPocket, int nProduct);//Barcode Reading을 시작할 때 올려준다.(없음)CEID 9805,9806
	void				OnMcTrayMoveLDBuffer(int nMode, int nTrayID, int nPocket, int nProduct, CString strSerial);//Tray를 Barcode Reading Zone에서 LD Buffer Zone으로 이동할 때 올려준다. CEID 9807,9808
	void				OnMcTrayMoveLDZone(int nMode, int nTrayID, int nPocket, int nProduct);//Tray를 LD Buffer Zone에서 LD Zone으로 이동 시 올려준다. CEID 9809,9810
	//void				OnMcProductLoadBuffer(int nMode, int nTrayID, CString strSerial);//LD Robot이 제품을 Pick up 시작 시 올려준다. CEID 9811,9812
	//kwlee 2017.0905
	void				OnMcProductLoadBuffer(int nMode, int nTrayID, int nPocket, int nProduct);//LD Robot이 제품을 Pick up 시작 시 올려준다. CEID 9811,9812
	//
	//void				OnMcProductBufferToSocket(int nMode, int nTrayID, CString strSerial, int nSocket);//TestSite Robot이 제품을 Socket 이동 시작 시 올려준다. CEID 9813,9814
	//kwlee 2017.0905
	void				OnMcProductBufferToSocket(int nMode, int nTrayID, CString strSerial, int nSocket,int nTestSite);//TestSite Robot이 제품을 Socket 이동 시작 시 올려준다. CEID 9813,9814
	void				OnMcProductTest(int nMode, CString strSerial, int nSocket,int nTestSite); //Test 시작 시 올려준다. CEID 9815,9816
	void				OnMcProductSocketToBuffer(int nMode, int nTrayID, CString strSerial, int nSocket,int nTestSite); //TestSite Robot이 제품을 Pick up 시작 시 올려준다. CEID 9817,9818
	void				OnMcProductUnloadBuffer(int nMode, int nTrayID, CString strSerial); //ULD Robot이 제품을 Pick up 시작 시 올려준다. CEID 9819, 9820
	void				OnMcRetestStart(CString strSerial);
	void				OnMcRetestEnd(CString strSerial, int nSocket, int nTrayID,int nTestSite);
	void				OnMcTrayUnload(int nMode, CString strLotNo, CString strPartNo, int nTrayID);// Tray가 UnLoading 진행 시(배출 또는 적재) 올려준다. 양품은 TRAY가 1부터 시작하지만 불량은 TRAY가 100부터 시작한다. CEID 9823,9824

	CString				m_strOldRecipe; // jtkim 20150418
	CString				m_strHeader;
	CString				m_strData;

	int					m_nXgemInterface;
	int					m_nXgemReq;
	int					m_nRunStatus;
	int					m_nHeader;
	int					m_nHeaderLength;

	void				OnDataReceive(CString strMsg);
	void				OnDataAnalysis(CString strMsg);
	CString				OnDataItemAnalysis(CString strMsg, CString strFind);
	void				OnXgemInitialize(CString strPath);
	void				OnMcRecipeCreate(int nMode, CString strRecipe, CString strPath);
	void				OnMcRecipeSelete(CString strRecipe);
	void				OnMcStatus(int nStatus);
	void				OnMcLotStart(CString strLotNo, CString strPartNo);
	//void				OnMcLotEnd(CString strLotNo, CString strPartNo, int nTotal, int nPass, int nFail, COleDateTimeSpan time);
	//kwlee 2017.0905
	void				OnMcLotEnd(CString strLotNo, CString strPartNo, int nTotal, int nPass, int nFail);
	void				OnMcAlarm(CString strCode, int nStatus);
	void				OnMcPCBInfo(CString strBarcode);
	void				OnMcInterface(int nMode);
	void				OnXgemRecipe(int nMode, CString strPath, CString strModel);
public:
	CXgemClient(void);
	~CXgemClient(void);
};

extern CXgemClient clsXgem;

