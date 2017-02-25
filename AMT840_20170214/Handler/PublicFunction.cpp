#include "StdAfx.h"
#include "PublicFunction.h"
#include "FastechPublic_IO.h"
#include "Variable.h"
#include "io.h"			// ���� ���� ���� �˻� �Լ� ȣ���ϱ� ���ؼ��� �ݵ�� �ʿ�
#include "ComizoaPublic.h"
#include "Ctlbd_Variable.h"
#include "Cmmsdk.h"
//#include "InterfaceGms.h"
#include "Alg_Mysql.h"
#include "XgemClient.h"
#include "AlgMemory.h"
#include "CtlBd_Library.h"
#include "math.h"
#include "ACriticalSection.h"

//james 2016.0926
#include "RunUldGoodTrayLeftOutputElevator.h"
#include "RunUldGoodTrayRightOutputElevator.h"
#include "RunRejectTrayFrontStackerElevator.h"
#include "RunRejectTrayRearStackerElevator.h"
//james 2016.0929 
#include "RunLdTrayStackerElevator.h"
#include "RunLdTrayPlate.h"
#include "RunDvcLdBuffer.h"
#include "RunRejectTrayStackerElevator.h"
#include "RunEmptyStackerElevator.h"

#include "RunLdUldRobot.h"
#include "RunTestSiteRobot.h"
//#include "Dialog_Message.h"
#include "LogFromat.h"

CPublicFunction clsFunc;

CMutex mutex_f;

#define MAX_LINE	500

CPublicFunction::CPublicFunction(void)
{
	int i;
	
	for(i=0; i<20; i++)
	{
		m_pFont[i]	= NULL;
		m_pFont[i]	= new CFont;
		m_pFont[i]->CreateFont(i + 10,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0, _T("MS Sans Serif"));		//Bitstream Vera Sans Mono
	}
	

	m_ArLotHistory.RemoveAll();

	m_safetyStep1 = 0;
	m_safetyStep2 = 0;
}


CPublicFunction::~CPublicFunction(void)
{
	int i;
	
	for(i=0; i<20; i++)
	{
		if (m_pFont[i] != NULL)
		{
			delete m_pFont[i];
			m_pFont[i] = NULL;
		}
	}
}

void CPublicFunction::OnSerialize(CArchive &ar)
{
	CObject::Serialize(ar);

	CString strTemp;
	
//	int i, j, k;
//	int x, y;
//	int nYear, nMonth, nDay, nHour, nMinute, nSecond;
//	int nNum[5];
 
}

LOGFONT CPublicFunction::OnLogFont(int nFontSize)
{
	LOGFONT lf;
	::ZeroMemory(&lf,sizeof(LOGFONT));
	lf.lfHeight = nFontSize;
	lf.lfWeight = 10;
	lf.lfWeight = 10;
	lf.lfItalic = false;
	lf.lfUnderline = false;
	lf.lfStrikeOut = false;
	lf.lfCharSet=VIETNAMESE_CHARSET;

	return lf;
}; 

int CPublicFunction::Set_LotInfo_LotStartData(int n_LotMode, CString str_LotNo) //james 2016.0921 lot start �� ������ ������ �����Ѵ�
{
	int i=0;

	st_lot_info[n_LotMode].nLotStatus = LOT_START;
	st_lot_info[n_LotMode].strLotNo = str_LotNo;

	//test site lot No ���� ���� 
	if(n_LotMode == LOT_CURR)
	{
		for(i = 0; i <8; i++) st_test_site_info[THD_TESTSITE_1 + i].strLotNo = st_lot_info[n_LotMode].strLotNo; //test site lot ������ ���� 
	}
	else if(n_LotMode == LOT_NEXT)
	{
		//LOT_NEXT �� ���� ���α׷����� ����ϸ鼭 ���� ���������϶����� test #1~#4 ���� ����� �� �ְ� ������ �ε� ���� ���������� ó���� ���÷� üũ�ϸ鼭 LOT_NEXT ��� �����Ѵ� 
	}

	for(i=0; i<THREAD_MAX_SITE; i++) st_lot_info[n_LotMode].nLot_THD_Status[i] = LOT_START; //�۾� ������ �˸��� 

	st_lot_info[n_LotMode].nLotNoApply_FirstTestStart = LOT_CLEAR; //james 2016.0926 

	//����Ŀ & buffer site lot No ���� ���� 
	//st_tray_info[st_lot_info[n_LotMode].nStacker_Good_THD_Num].strLotNo = st_lot_info[n_LotMode].strLotNo;
	//st_tray_info[st_lot_info[n_LotMode].nStacker_Reject_THD_Num].strLotNo = st_lot_info[n_LotMode].strLotNo;
	//st_buffer_info[st_lot_info[n_LotMode].nBuffer_Retest_THD_Num].strLotNo = st_lot_info[n_LotMode].strLotNo;

	return RET_GOOD;
}

int CPublicFunction::Set_LotInfo_LotEndDataExchange(int nMode) //james 2016.0921 lot end �Ϸ�� LOT_CURR ~ LOT_CURR_BACKUP ������ �����͸� ��ȯ�Ѵ� 
{
	int i=0;
	int j=0;

	///////////////////////////////////////////////////////////////////////////////////////////////
	//���� ���� clear
	/////////////////////////////////////////////////////////////////////////////////////////////// 
	if(st_lot_info[LOT_CURR].nLotStatus >= LOT_START) //LOT_CURR�� ����Ѵ�
	{
		st_lot_info[LOT_CURR_BACKUP].strLotNo = st_lot_info[LOT_CURR].strLotNo;
		st_lot_info[LOT_CURR_BACKUP].strPartNo = st_lot_info[LOT_CURR].strPartNo;
		st_lot_info[LOT_CURR_BACKUP].strProcess = st_lot_info[LOT_CURR].strProcess;
		st_lot_info[LOT_CURR_BACKUP].strProPerty = st_lot_info[LOT_CURR].strProPerty;
		st_lot_info[LOT_CURR_BACKUP].strBypass = st_lot_info[LOT_CURR].strBypass;

		st_lot_info[LOT_CURR_BACKUP].nLotNoApply_FirstTestStart = st_lot_info[LOT_CURR].nLotNoApply_FirstTestStart; //james 2016.0926 

		

		st_lot_info[LOT_CURR_BACKUP].nLotStatus = st_lot_info[LOT_CURR].nLotStatus;
		for(i=0; i<THREAD_MAX_SITE; i++) st_lot_info[LOT_CURR_BACKUP].nLot_THD_Status[i] = st_lot_info[LOT_CURR].nLot_THD_Status[i]; 
		st_lot_info[LOT_CURR_BACKUP].nNewLotIn  = st_lot_info[LOT_CURR].nNewLotIn;
		st_lot_info[LOT_CURR_BACKUP].nLastModule = st_lot_info[LOT_CURR].nLastModule;
		st_lot_info[LOT_CURR_BACKUP].nTotalCnt = st_lot_info[LOT_CURR].nTotalCnt;
		st_lot_info[LOT_CURR_BACKUP].nLane = st_lot_info[LOT_CURR].nLane;

		st_lot_info[LOT_CURR_BACKUP].nLdTrayCnt = st_lot_info[LOT_CURR].nLdTrayCnt;
		st_lot_info[LOT_CURR_BACKUP].nUnLdTrayCnt = st_lot_info[LOT_CURR].nUnLdTrayCnt;
		for(i=0; i<3; i++) st_lot_info[LOT_CURR_BACKUP].nBinSortCnt[i]  = st_lot_info[LOT_CURR].nBinSortCnt[i];
		st_lot_info[LOT_CURR_BACKUP].nTrayRunMode_StandAlone = st_lot_info[LOT_CURR].nTrayRunMode_StandAlone; 

		// jtkim 20150408
		st_lot_info[LOT_CURR_BACKUP].tStart		= st_lot_info[LOT_CURR].tStart;

		//james 2016.0921 
		st_lot_info[LOT_CURR_BACKUP].tLotStart		= st_lot_info[LOT_CURR].tLotStart;

		st_lot_info[LOT_CURR_BACKUP].nAbortTime = st_lot_info[LOT_CURR].nAbortTime;
		st_lot_info[LOT_CURR_BACKUP].nAbortTotal = st_lot_info[LOT_CURR].nAbortTotal;
		st_lot_info[LOT_CURR_BACKUP].nAbortCount = st_lot_info[LOT_CURR].nAbortCount;
		st_lot_info[LOT_CURR_BACKUP].nAbortUsed = st_lot_info[LOT_CURR].nAbortUsed;	 

		st_lot_info[LOT_CURR_BACKUP].nStacker_Good_THD_Num = st_lot_info[LOT_CURR].nStacker_Good_THD_Num;
		st_lot_info[LOT_CURR_BACKUP].nStacker_Reject_THD_Num = st_lot_info[LOT_CURR].nStacker_Reject_THD_Num;
		st_lot_info[LOT_CURR_BACKUP].nBuffer_Retest_THD_Num = st_lot_info[LOT_CURR].nBuffer_Retest_THD_Num;
				
		//st_tray_info[st_lot_info[LOT_CURR_BACKUP].nStacker_Good_THD_Num].strLotNo = st_tray_info[st_lot_info[LOT_CURR].nStacker_Good_THD_Num].strLotNo;
		//st_tray_info[st_lot_info[LOT_CURR_BACKUP].nStacker_Reject_THD_Num].strLotNo = st_tray_info[st_lot_info[LOT_CURR].nStacker_Reject_THD_Num].strLotNo;
		//st_tray_info[st_lot_info[LOT_CURR_BACKUP].nBuffer_Retest_THD_Num].strLotNo = st_tray_info[st_lot_info[LOT_CURR].nBuffer_Retest_THD_Num].strLotNo;
		//st_tray_info[THD_LD_TRAY_PLATE].strLotNo = st_tray_info[THD_LD_TRAY_PLATE].strLotNo;
		//for(i = THD_TESTSITE_1; i < THD_TESTSITE_8; i++)st_tray_info[i].strLotNo = st_test_site_info[i].strLotNo; 

		st_lot_info[LOT_CURR_BACKUP].nTestMode = st_lot_info[LOT_CURR].nTestMode;// ONESHOT, HOT, COLD, GATE......
		st_lot_info[LOT_CURR_BACKUP].nInterface = st_lot_info[LOT_CURR].nInterface;// ftp / local......
		st_lot_info[LOT_CURR_BACKUP].nRetry = st_lot_info[LOT_CURR].nRetry;// retry count.....
		st_lot_info[LOT_CURR_BACKUP].nBarcodeMode = st_lot_info[LOT_CURR].nBarcodeMode;// barcode ���ۼ��� (1��/2��)
		st_lot_info[LOT_CURR_BACKUP].nQty = st_lot_info[LOT_CURR].nQty;// lot ����.....
		st_lot_info[LOT_CURR_BACKUP].nModuleCount = st_lot_info[LOT_CURR].nModuleCount;// pgm download �Ϸ�......
		st_lot_info[LOT_CURR_BACKUP].nPgmDown = st_lot_info[LOT_CURR].nPgmDown;
		st_lot_info[LOT_CURR_BACKUP].nChamberDoor = st_lot_info[LOT_CURR].nChamberDoor;// chanber door open......	 
		// jtkim 20161008
		st_lot_info[LOT_CURR_BACKUP].nBdTime		= st_lot_info[LOT_CURR].nBdTime;
		st_lot_info[LOT_CURR_BACKUP].nBdTimeCount	= st_lot_info[LOT_CURR].nBdTimeCount;
		//
/*		
		// jtkim 20150307
		st_count_info.nInCount[0][0]				= st_count_info.nInCount[0][1];
		st_count_info.nPassCount[0][0]				= st_count_info.nPassCount[0][1];
		st_count_info.nRejectCount[0][0]			= st_count_info.nRejectCount[0][1];
		st_count_info.nPrimeCount[0][0]				= st_count_info.nPrimeCount[0][1];
		st_count_info.nPrimeRejectCount[0][0]		= st_count_info.nPrimeRejectCount[0][1];
*/
	}

	

	if(st_lot_info[LOT_NEXT].nLotStatus >= LOT_START || st_lot_info[LOT_NEXT].strLotNo != "") //Current Lot�� lot end ��������, next lot�� ����ǰ� �־����� Current Lot ������ Next lot ������ update �Ѵ� 
	{
		///* 2015.0302 james
		st_lot_info[LOT_CURR].strLotNo = "";
		st_lot_info[LOT_CURR].strPartNo = "";
		st_lot_info[LOT_CURR].strProcess = "";
		st_lot_info[LOT_CURR].strProPerty = "";
		st_lot_info[LOT_CURR].strBypass = "";

		st_lot_info[LOT_CURR].nLotNoApply_FirstTestStart = LOT_CLEAR; //james 2016.0926 

		st_lot_info[LOT_CURR].nLotStatus = LOT_CLEAR;
		//james 2016.0924 for( i= 0; i < THREAD_MAX_SITE; i++) st_lot_info[LOT_CURR].nLot_THD_Status[i] = LOT_CLEAR;  	 
		st_lot_info[LOT_CURR].nNewLotIn = 0;
		st_lot_info[LOT_CURR].nLastModule = 0;
		st_lot_info[LOT_CURR].nTotalCnt = 0;
		st_lot_info[LOT_CURR].nLane = 0; 
		st_lot_info[LOT_CURR].nLdTrayCnt = 0;
		st_lot_info[LOT_CURR].nUnLdTrayCnt = 0;
		for(i=0; i<3; i++) st_lot_info[LOT_CURR].nBinSortCnt[i] = 0;
		st_lot_info[LOT_CURR].nTrayRunMode_StandAlone = 0; 

		//james 2016.0921 
		st_lot_info[LOT_CURR].tLotStart		= 0;

		st_lot_info[LOT_CURR].nAbortTime = 0;
		st_lot_info[LOT_CURR].nAbortTotal = 0;
		st_lot_info[LOT_CURR].nAbortCount = 0;
		st_lot_info[LOT_CURR].nAbortUsed = 0;	 

		st_lot_info[LOT_CURR].nStacker_Good_THD_Num = 0;
		st_lot_info[LOT_CURR].nStacker_Reject_THD_Num = 0;
		st_lot_info[LOT_CURR].nBuffer_Retest_THD_Num = 0;

		st_lot_info[LOT_CURR].nTestMode = 0;// ONESHOT, HOT, COLD, GATE......
		st_lot_info[LOT_CURR].nInterface = 0;// ftp / local......
		st_lot_info[LOT_CURR].nRetry = 0;// retry count.....
		st_lot_info[LOT_CURR].nBarcodeMode = 0;// barcode ���ۼ��� (1��/2��)
		st_lot_info[LOT_CURR].nQty = 0;// lot ����.....
		st_lot_info[LOT_CURR].nModuleCount = 0;// pgm download �Ϸ�......
		st_lot_info[LOT_CURR].nPgmDown = 0;
		st_lot_info[LOT_CURR].nChamberDoor = 0;// chanber door open......	 
		///////////////////////////////////////////////////////////////////////////////////////////////
		//*/

		st_lot_info[LOT_CURR].strLotNo = st_lot_info[LOT_NEXT].strLotNo;
		st_lot_info[LOT_CURR].strPartNo = st_lot_info[LOT_NEXT].strPartNo;
		st_lot_info[LOT_CURR].strProcess = st_lot_info[LOT_NEXT].strProcess;
		st_lot_info[LOT_CURR].strProPerty = st_lot_info[LOT_NEXT].strProPerty;
		st_lot_info[LOT_CURR].strBypass = st_lot_info[LOT_NEXT].strBypass;

		st_lot_info[LOT_CURR].nLotNoApply_FirstTestStart = st_lot_info[LOT_NEXT].nLotNoApply_FirstTestStart; //james 2016.0926 

		st_lot_info[LOT_CURR].nLotStatus = st_lot_info[LOT_NEXT].nLotStatus;
		for(i=0; i<THREAD_MAX_SITE; i++) st_lot_info[LOT_CURR].nLot_THD_Status[i] = st_lot_info[LOT_NEXT].nLot_THD_Status[i]; 
		st_lot_info[LOT_CURR].nNewLotIn  = st_lot_info[LOT_NEXT].nNewLotIn;
		st_lot_info[LOT_CURR].nLastModule = st_lot_info[LOT_NEXT].nLastModule;
		st_lot_info[LOT_CURR].nTotalCnt = st_lot_info[LOT_NEXT].nTotalCnt;
		st_lot_info[LOT_CURR].nLane = st_lot_info[LOT_NEXT].nLane;

		st_lot_info[LOT_CURR].nLdTrayCnt = st_lot_info[LOT_NEXT].nLdTrayCnt;
		st_lot_info[LOT_CURR].nUnLdTrayCnt = st_lot_info[LOT_NEXT].nUnLdTrayCnt;
		for(i=0; i<3; i++) st_lot_info[LOT_CURR].nBinSortCnt[i] = st_lot_info[LOT_NEXT].nBinSortCnt[i];
		st_lot_info[LOT_CURR].nTrayRunMode_StandAlone = st_lot_info[LOT_NEXT].nTrayRunMode_StandAlone; 

		// jtkim 20150408
		st_lot_info[LOT_CURR].tStart		= st_lot_info[LOT_NEXT].tStart;

		//james 2016.0921 
		st_lot_info[LOT_CURR].tLotStart		= st_lot_info[LOT_NEXT].tLotStart;

		st_lot_info[LOT_CURR].nAbortTime = st_lot_info[LOT_NEXT].nAbortTime;
		st_lot_info[LOT_CURR].nAbortTotal = st_lot_info[LOT_NEXT].nAbortTotal;
		st_lot_info[LOT_CURR].nAbortCount = st_lot_info[LOT_NEXT].nAbortCount;
		st_lot_info[LOT_CURR].nAbortUsed = st_lot_info[LOT_NEXT].nAbortUsed;	 

		st_lot_info[LOT_CURR].nStacker_Good_THD_Num = st_lot_info[LOT_NEXT].nStacker_Good_THD_Num;
		st_lot_info[LOT_CURR].nStacker_Reject_THD_Num = st_lot_info[LOT_NEXT].nStacker_Reject_THD_Num;
		st_lot_info[LOT_CURR].nBuffer_Retest_THD_Num = st_lot_info[LOT_NEXT].nBuffer_Retest_THD_Num;		

		st_lot_info[LOT_CURR].nTestMode = st_lot_info[LOT_NEXT].nTestMode;// ONESHOT, HOT, COLD, GATE......
		st_lot_info[LOT_CURR].nInterface = st_lot_info[LOT_NEXT].nInterface;// ftp / local......
		st_lot_info[LOT_CURR].nRetry = st_lot_info[LOT_NEXT].nRetry;// retry count.....
		st_lot_info[LOT_CURR].nBarcodeMode = st_lot_info[LOT_NEXT].nBarcodeMode;// barcode ���ۼ��� (1��/2��)
		st_lot_info[LOT_CURR].nQty = st_lot_info[LOT_NEXT].nQty;// lot ����.....
		st_lot_info[LOT_CURR].nModuleCount = st_lot_info[LOT_NEXT].nModuleCount;// pgm download �Ϸ�......
		st_lot_info[LOT_CURR].nPgmDown = st_lot_info[LOT_NEXT].nPgmDown;
		st_lot_info[LOT_CURR].nChamberDoor = st_lot_info[LOT_NEXT].nChamberDoor;// chanber door open......	 

		for(i=0; i<8; i++) //james 2016.0926 �߰� 
		{
			st_test_site_info[THD_TESTSITE_1 + i].strLotNo = st_lot_info[LOT_NEXT].strLotNo;
			//2016.1102
			st_test_site_info[THD_TESTSITE_1 + i].strPartNo = st_lot_info[LOT_NEXT].strPartNo;
		}

		// jtkim 20161008
		st_lot_info[LOT_CURR].nBdTime		= st_lot_info[LOT_NEXT].nBdTime;
		st_lot_info[LOT_CURR].nBdTimeCount	= st_lot_info[LOT_NEXT].nBdTimeCount;

		//james 2016.0916 
		clsRunUldGoodTrayLeftOutputElevator.m_nLotProcessNum = LOT_CURR;
		clsRunUldGoodTrayRightOutputElevator.m_nLotProcessNum = LOT_CURR;
		clsRunRejectTrayFrontStackerElevator.m_nLotProcessNum = LOT_CURR;
		clsRunRejectTrayRearStackerElevator.m_nLotProcessNum = LOT_CURR;
		
		//james 2016.0929 
		clsRunLdTrayStackerElevator.m_nLotProcessNum = LOT_CURR;
		clsRunLdTrayPlate.m_nLotProcessNum = LOT_CURR;
		clsRunDvcLdBuffer.m_nLotProcessNum = LOT_CURR;		
		clsRunRejectTrayStackerElevator.m_nLotProcessNum = LOT_CURR;
		clsRunEmptyStackerElevator.m_nLotProcessNum = LOT_CURR;

		clsRunLdUldRobot.m_nLotProcessNum = LOT_CURR;
		clsRunTestSiteRobot.m_nLotProcessNum = LOT_CURR;

		//st_count_info.nInCount[0][0] = st_count_info.nInCount[0][1];
		//st_count_info.nInCount[0][1] = 0;

		// jtkim 20150307
		st_count_info.nInCount[0][0]				= st_count_info.nInCount[0][1];
		st_count_info.nPassCount[0][0]				= st_count_info.nPassCount[0][1];
		st_count_info.nRejectCount[0][0]			= st_count_info.nRejectCount[0][1];
		st_count_info.nPrimeCount[0][0]				= st_count_info.nPrimeCount[0][1];
		st_count_info.nPrimeRejectCount[0][0]		= st_count_info.nPrimeRejectCount[0][1];

		// jtkim 20150529 scrap code shift
		for (i=0; i<10000; i++)
		{
			for (j=0; j<2; j++)
			{
				st_code_info[LOT_CURR].m_nScrapCode[i][j] = st_code_info[LOT_NEXT].m_nScrapCode[i][j];
			}
		}

		//////////////////////////////////////////////////////////////////////////////
		//2016.1203
		st_work_info.nCycleCount	= 0;
		st_work_info.nCycleYesNo	= NO;
		st_work_info.dwTrayCycle	= 0;
		st_work_info.dwLotCycle = 0;
		//////////////////////////////////////////////////////////////////////////////
	}
	else
	{//2015.0303 james
		st_lot_info[LOT_CURR].strLotNo = "";
		st_lot_info[LOT_CURR].strPartNo = "";
		st_lot_info[LOT_CURR].strProcess = "";
		st_lot_info[LOT_CURR].strProPerty = "";
		st_lot_info[LOT_CURR].strBypass = "";

		st_lot_info[LOT_CURR].nLotNoApply_FirstTestStart = LOT_CLEAR; //james 2016.0926 

		st_lot_info[LOT_CURR].nLotStatus = LOT_CLEAR;
		//james 2016.0924 for( i= 0; i < THREAD_MAX_SITE; i++) st_lot_info[LOT_CURR].nLot_THD_Status[i] = LOT_CLEAR;  	 
		st_lot_info[LOT_CURR].nNewLotIn = 0;
		st_lot_info[LOT_CURR].nLastModule = 0;
		st_lot_info[LOT_CURR].nTotalCnt = 0;
		st_lot_info[LOT_CURR].nLane = 0; 
		st_lot_info[LOT_CURR].nLdTrayCnt = 0;
		st_lot_info[LOT_CURR].nUnLdTrayCnt = 0;
		for(i=0; i<3; i++) st_lot_info[LOT_CURR].nBinSortCnt[i] = 0;
		st_lot_info[LOT_CURR].nTrayRunMode_StandAlone = 0; 

		//james 2016.0921 
		st_lot_info[LOT_CURR].tLotStart		= 0;

		st_lot_info[LOT_CURR].nAbortTime = 0;
		st_lot_info[LOT_CURR].nAbortTotal = 0;
		st_lot_info[LOT_CURR].nAbortCount = 0;
		st_lot_info[LOT_CURR].nAbortUsed = 0;	 

		st_lot_info[LOT_CURR].nStacker_Good_THD_Num = 0;
		st_lot_info[LOT_CURR].nStacker_Reject_THD_Num = 0;
		st_lot_info[LOT_CURR].nBuffer_Retest_THD_Num = 0;

		st_lot_info[LOT_CURR].nTestMode = 0;// ONESHOT, HOT, COLD, GATE......
		st_lot_info[LOT_CURR].nInterface = 0;// ftp / local......
		st_lot_info[LOT_CURR].nRetry = 0;// retry count.....
		st_lot_info[LOT_CURR].nBarcodeMode = 0;// barcode ���ۼ��� (1��/2��)
		st_lot_info[LOT_CURR].nQty = 0;// lot ����.....
		st_lot_info[LOT_CURR].nModuleCount = 0;// pgm download �Ϸ�......
		st_lot_info[LOT_CURR].nPgmDown = 0;
		st_lot_info[LOT_CURR].nChamberDoor = 0;// chanber door open......	 
	}

	//clear
	st_lot_info[LOT_NEXT].strLotNo = "";
	st_lot_info[LOT_NEXT].strPartNo = "";
	st_lot_info[LOT_NEXT].strProcess = "";
	st_lot_info[LOT_NEXT].strProPerty = "";
	st_lot_info[LOT_NEXT].strBypass = "";

	st_lot_info[LOT_NEXT].nLotNoApply_FirstTestStart = LOT_CLEAR; //james 2016.0926 


	st_lot_info[LOT_NEXT].nLotStatus = LOT_CLEAR;
	//james 2016.0924 for( i= 0; i < THREAD_MAX_SITE; i++)st_lot_info[LOT_NEXT].nLot_THD_Status[i] = LOT_CLEAR; 
	st_lot_info[LOT_NEXT].nNewLotIn = 0;
	st_lot_info[LOT_NEXT].nLastModule = 0;
	st_lot_info[LOT_NEXT].nTotalCnt = 0;
	st_lot_info[LOT_NEXT].nLane = 0; 
	st_lot_info[LOT_NEXT].nLdTrayCnt = 0;
	st_lot_info[LOT_NEXT].nUnLdTrayCnt = 0;
	for(i=0; i<3; i++) st_lot_info[LOT_NEXT].nBinSortCnt[i] = 0;
	st_lot_info[LOT_NEXT].nTrayRunMode_StandAlone = 0; 

	//james 2016.0921 
	st_lot_info[LOT_NEXT].tLotStart		= 0;

	st_lot_info[LOT_NEXT].nAbortTime = 0;
	st_lot_info[LOT_NEXT].nAbortTotal = 0;
	st_lot_info[LOT_NEXT].nAbortCount = 0;
	st_lot_info[LOT_NEXT].nAbortUsed = 0;	 

	st_lot_info[LOT_NEXT].nStacker_Good_THD_Num = 0;
	st_lot_info[LOT_NEXT].nStacker_Reject_THD_Num = 0;
	st_lot_info[LOT_NEXT].nBuffer_Retest_THD_Num = 0;

	st_lot_info[LOT_NEXT].nTestMode = 0;// ONESHOT, HOT, COLD, GATE......
	st_lot_info[LOT_NEXT].nInterface = 0;// ftp / local......
	st_lot_info[LOT_NEXT].nRetry = 0;// retry count.....
	st_lot_info[LOT_NEXT].nBarcodeMode = 0;// barcode ���ۼ��� (1��/2��)
	st_lot_info[LOT_NEXT].nQty = 0;// lot ����.....
	st_lot_info[LOT_NEXT].nModuleCount = 0;// pgm download �Ϸ�......
	st_lot_info[LOT_NEXT].nPgmDown = 0;
	st_lot_info[LOT_NEXT].nChamberDoor = 0;// chanber door open......	 

	st_var.nTestDvcOutFlag = 0; //test ����Ʈ���� ���縦 ó�� ����� ���õȴ� 

	//m_strAlarmCode.Format(_T("900000")); //900000 0 "LOT_END"
	
	if (st_handler_info.cWndMain != NULL)
	{
		st_handler_info.cWndMain->SendMessage(WM_WORK_DISPLAY, MAIN_LOT_DISPLAY, 0);
	}
	return CTL_GOOD;
}
//lot end ���¸� üũ �Ѵ� 
int CPublicFunction::Find_AllLotEndStatus(int n_Mode)
{
	int nFuncRet = RET_SKIP; //RET_GOOD;


	return nFuncRet;
}

void CPublicFunction::OnTimeReset()
{
	CTime time;
//	int i,j;
	int i,j;
	time = CTime::GetCurrentTime();

	st_count_info.nInCount[1][0]	= 0;
	st_count_info.nInCount[1][1]	= 0;
	st_count_info.nPassCount[1][0]	= 0;
	st_count_info.nPassCount[1][1]	= 0;
	st_count_info.nRejectCount[1][0]= 0;
	st_count_info.nRejectCount[1][1]= 0;
	st_count_info.nPrimeCount[1][0]	= 0;
	st_count_info.nPrimeCount[1][1]	= 0;
	st_count_info.nPrimeRejectCount[1][0] = 0;
	st_count_info.nPrimeRejectCount[1][1] = 0;

	st_alarm_info.nAlarmNum = 0;
//	st_count_info.nDailyUph = 0;

	st_handler_info.tRun		= 0;			// Run Time
	st_handler_info.tJam		= 0;			// Jam Time
	st_handler_info.tStop		= 0;			// Stop Time
	st_handler_info.tMaint		= 0;			// Maint Time
	st_handler_info.tDRun		= 0;			// Run Time
	st_handler_info.tDJam		= 0;			// Jam Time
	st_handler_info.tDStop		= 0;			// Stop Time
	st_handler_info.tDMaint		= 0;			// Maint Time


	//2015.1005
	st_handler_info.m_tDRef = 0;
	for (i = 0; i<TSITE_SOCKET_CNT; i++)
	{
		st_handler_info.m_tDRdown[i] = 0; 
		st_handler_info.m_tDStop[i] = 0;
		st_handler_info.m_tDUserStop[i] = 0;
		st_handler_info.m_tDInStandby[i] = 0;  
		////////
	}

	for ( i = 0; i < TSITE_SOCKET_CNT; i++)
	{
		st_handler_info.m_tDtest[i] = 0;
		st_handler_info.m_tDSokOff[i] = 0;
		st_handler_info.m_tDOutStandby[i] = 0;
		st_handler_info.mn_Fail[i] = 0;
		st_handler_info.mn_Pass[i] = 0;
		st_handler_info.mn_Input[i] = 0;
	}

	CString str, mstr_temp;

	//kwlee 2015.1127
	for (i = 0; i<48; i++)
	{
		for (j = 0; j<48; j++)
		{
			st_DB_time.n_Db_time[i][j] = 0;
		}
	}

	
	///////////
	mstr_temp.Format(_T("%d"),st_handler_info.m_tDRef);
	:: WritePrivateProfileString(_T("TIME"), _T("Daily_Ref_Time"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	for (j =0; j<TSITE_SOCKET_CNT; j++)
	{
		str.Format(_T("Daily_RunDown_Time[%d]"), j);
		mstr_temp.Format(_T("%d"),st_handler_info.m_tDRdown[j]);
		:: WritePrivateProfileString(_T("TIME"), str, LPCTSTR(mstr_temp), st_path_info.strFileBasic);

		str.Format(_T("Daily_Stop[%d]"), j);
		mstr_temp.Format(_T("%d"),st_handler_info.m_tDStop[j]);
		:: WritePrivateProfileString(_T("TIME"), str, LPCTSTR(mstr_temp), st_path_info.strFileBasic);

		str.Format(_T("Daily_UserStop[%d]"), j);
		mstr_temp.Format(_T("%d"),st_handler_info.m_tDUserStop[j]);
		:: WritePrivateProfileString(_T("TIME"), str, LPCTSTR(mstr_temp), st_path_info.strFileBasic);
		//kwlee 2015.1127
// 		str.Format(_T("InStandBy_Time[%d]"), j);
// 		mstr_temp.Format(_T("%d"),st_handler_info.m_tDInStandby[j]);
// 		:: WritePrivateProfileString(_T("TIME"), str, LPCTSTR(mstr_temp), st_path_info.strFileBasic);
		str.Format(_T("InStandBy_Time[%d]"), j);
		mstr_temp.Format(_T("%d"),st_DB_time.n_Db_time[INSTANBY][j] );
		:: WritePrivateProfileString(_T("TIME"), str, LPCTSTR(mstr_temp), st_path_info.strFileBasic);


	}
}


void CPublicFunction::OnDailyDataSave()
{
	CString strFileName;				// ���������� ������ ���� �̸� ���� ���� 
	CString strCreateFile;				// �˶� ���� ������ ���Ͽ� ���� [����]+[���ϸ�]+[Ȯ����] ���� ���� 
	CString strListName, strTempData;	// �� �κк� �˶� �߻� Ƚ�� ���� ������ �α� ���� ���� ���� ���� 
	CString strContent;				// �� ���ο� ����� ���ڿ� ���� ���� ���� 
	int nExistence;						// ���� ���� ���� ���� �÷��� 
	char chFileName[256];				// �˻��� ���� ���� ���� �Լ� 
	char chMsg[1000];
	FILE  *fp ;							// ���Ͽ� ���� ������ ���� ���� 
	
	CString strTitleTime, strFileTime, strNewFile;		// ���Ͽ� ����� Ÿ��Ʋ �ð� ���� ����
	int nCurYear, nCurMonth, nCurDay;					// ��, ��, �� ���� ����
	int nCurHour, nCurMinute, nCurSecond;				// ��, ��, �� ���� ����
	int nMtbi = 0;

	double dAve;
	
	COleDateTime otCurr;									// ���� �ð� ���� ����
	CTime tCurr;										// Ÿ��Ʋ �ð� ���� ����

	CString strTemp;

	CString strData, strTime;
	
	/* ************************************************************************** */
	/* ���� �̸����� ����� ��¥ �����Ѵ�                                         */
	/* ************************************************************************** */
	otCurr = COleDateTime::GetCurrentTime();
	
	nCurYear		= otCurr.GetYear();  
	nCurMonth		= otCurr.GetMonth();  
    nCurDay			= otCurr.GetDay();  

	nCurHour		= otCurr.GetHour();
	nCurMinute		= otCurr.GetMinute();
	nCurSecond		= otCurr.GetSecond();
	
	strNewFile.Format(_T("DAY_DATA%04d%02d%02d"), nCurYear, nCurMonth, nCurDay);
	strTime.Format(_T("%04d/%02d/%02d %02d:%02d:%02d"), nCurYear, nCurMonth, nCurDay, nCurHour, nCurMinute, nCurSecond);
		
	strCreateFile = st_path_info.strData + strNewFile;
	strCreateFile += ".TXT";
	
	OnStringToChar(strCreateFile, chFileName);
	nExistence = _access(chFileName, 0);
	
	if (nExistence == -1)  /* ���� ������ */
	{
		strCreateFile = st_path_info.strData + strNewFile;
		strCreateFile += ".TXT";
		OnStringToChar(strCreateFile, chFileName);
	}
	
	/* ************************************************************************** */
	/* �˶� �߻� Ƚ�� ���� ���� ���Ͽ� �߰� ������ ���� ���Ϸ� ����               */
	/* ************************************************************************** */
	fopen_s(&fp, chFileName, "a+");
	if(!fp)
 	{
 //		AfxMessageBox(_T("The failure because we open the file."));
 		return;
 	}
	/* ************************************************************************** */

	/* ************************************************************************** */
	/* ���Ͽ� Ÿ��Ʋ ���� ����Ѵ�                                                */
	/* ************************************************************************** */
	strContent = _T("********************************************************************************************************");
	OnStringToChar(strContent, chMsg);
	fprintf(fp, "%s\r\n", chMsg) ;
//	fprintf(fp, chMsg) ;
//	fprintf(fp, "\r\n") ;

	strContent.Format(_T("| %-100s |"), _T("AMT840 [SSD Interface] : "));
	OnStringToChar(strContent, chMsg);
	fprintf(fp, "%s\r\n", chMsg);
//	fprintf(fp, chMsg) ; 
//	fprintf(fp, "\r\n") ;

	strContent.Format(_T("| %-100s |"), _T("Day Data Info File"));
	OnStringToChar(strContent, chMsg);
	fprintf(fp,"%s\r\n", chMsg) ;
//	fprintf(fp, chMsg) ;
//	fprintf(fp, "\r\n") ;

	strContent.Format(_T("| %-100s |"), _T("DIVISION  : AMT Coporation"));
	OnStringToChar(strContent, chMsg);
	fprintf(fp,"%s\n", chMsg) ;
//	fprintf(fp, chMsg) ;
//	fprintf(fp, "\r\n") ;

	strContent.Format(_T("| %-100s |"), _T("Save Time : "));
	strContent += strTime;
	OnStringToChar(strContent, chMsg);
	fprintf(fp,"%s\r\n", chMsg) ;
//	fprintf(fp, chMsg) ;
//	fprintf(fp, "\r\n") ;

	//strTemp.Format(_T("%02d:%02d:%02d"), st_handler_info.tRun.GetTotalHours(), st_handler_info.tRun.GetMinutes(), st_handler_info.tRun.GetSeconds() );
	strTemp.Format(_T("%02d:%02d:%02d"), st_handler_info.tRun.GetHours(), st_handler_info.tRun.GetMinutes(), st_handler_info.tRun.GetSeconds() );
	strContent.Format(_T("| %-100s |"), _T("Run Time : "));
	strContent += strTemp;
	OnStringToChar(strContent, chMsg);
	fprintf(fp,"%s\r\n", chMsg) ;
//	fprintf(fp, chMsg) ;
//	fprintf(fp, "\r\n") ;

	//strTemp.Format(_T("%02d:%02d:%02d"), st_handler_info.tStop.GetTotalHours(), st_handler_info.tStop.GetMinutes(), st_handler_info.tStop.GetSeconds() );
	strTemp.Format(_T("%02d:%02d:%02d"), st_handler_info.tStop.GetHours(), st_handler_info.tStop.GetMinutes(), st_handler_info.tStop.GetSeconds() );
	strContent.Format(_T("| %-100s |"), _T("Stop Time : "));
	strContent += strTemp;
	OnStringToChar(strContent, chMsg);
	fprintf(fp,"%s\r\n", chMsg) ;
//	fprintf(fp, chMsg) ;
//	fprintf(fp, "\r\n") ;

	//strTemp.Format(_T("%02d:%02d:%02d"), st_handler_info.tJam.GetTotalHours(), st_handler_info.tJam.GetMinutes(), st_handler_info.tJam.GetSeconds() );
	strTemp.Format(_T("%02d:%02d:%02d"), st_handler_info.tJam.GetHours(), st_handler_info.tJam.GetMinutes(), st_handler_info.tJam.GetSeconds() );
	strContent.Format(_T("| %-100s |"), _T("Alarm Time : "));
	strContent += strTemp;
	OnStringToChar(strContent, chMsg);
	fprintf(fp,"%s\r\n", chMsg) ;
//	fprintf(fp, chMsg) ;
//	fprintf(fp, "\r\n") ;

	strTemp.Format(_T("%d"), st_alarm_info.nAlarmNum);
	strContent.Format(_T("| %-100s |"), _T("Alarm Count : "));
	strContent += strTemp;
	OnStringToChar(strContent, chMsg);
	fprintf(fp,"%s\r\n", chMsg) ;
//	fprintf(fp, chMsg) ;
//	fprintf(fp, "\r\n") ;

	nMtbi = (int)st_handler_info.tRun.GetTotalHours() * 3600 + (int)st_handler_info.tRun.GetMinutes() * 60 + (int)st_handler_info.tRun.GetSeconds();
	if (st_alarm_info.nAlarmNum > 0)
	{
		nMtbi = nMtbi / st_alarm_info.nAlarmNum;
	}

	strTemp.Format(_T("%02d:%02d:%02d"), nMtbi / 3600, (nMtbi % 3600) / 60, nMtbi % 60 );
	strContent.Format(_T("| %-100s |"), _T("MTBI : "));
	strContent += strTemp;
	OnStringToChar(strContent, chMsg);
	fprintf(fp,"%s\r\n", chMsg) ;
//	fprintf(fp, chMsg) ;
//	fprintf(fp, "\r\n") ;

	strTemp.Format(_T("%d"), st_count_info.nInCount[1][0]);
	strContent.Format(_T("| %-100s |"), _T("TOTAL : "));
	strContent += strTemp;
	OnStringToChar(strContent, chMsg);
	fprintf(fp,"%s\r\n", chMsg) ;

	strTemp.Format(_T("%d"), st_count_info.nPrimeCount[1][0]);
	strContent.Format(_T("| %-100s |"), _T("PRIME PASS: "));
	strContent += strTemp;
	OnStringToChar(strContent, chMsg);
	fprintf(fp,"%s\r\n", chMsg) ;

	dAve = 0.0f;
	if (st_count_info.nPrimeCount[1][0] > 0) 
	{
		dAve = ((double)st_count_info.nPrimeCount[1][0] / (double)st_count_info.nInCount[1][0]) * (double)100;
	}
	strTemp.Format(_T("%.3f"), dAve);
	strContent.Format(_T("| %-100s |"), _T("PRIME YIELD: "));
	strContent += strTemp;
	OnStringToChar(strContent, chMsg);
	fprintf(fp,"%s\r\n", chMsg) ;
	
	strTemp.Format(_T("%d"), st_count_info.nPassCount[1][0]);
	strContent.Format(_T("| %-100s |"), _T("CUM : "));
	strContent += strTemp;
	OnStringToChar(strContent, chMsg);
	fprintf(fp,"%s\r\n", chMsg) ;

	dAve = 0.0f;
	if (st_count_info.nPassCount[1][0] > 0) 
	{
		dAve = ((double)st_count_info.nPassCount[1][0] / (double)st_count_info.nInCount[1][0]) * (double)100;
	}
	strTemp.Format(_T("%.3f"), dAve);
	strContent.Format(_T("| %-100s |"), _T("CUM YIELD : "));
	strContent += strTemp;
	OnStringToChar(strContent, chMsg);
	fprintf(fp,"%s\r\n", chMsg) ;

	dAve = 0.0f;
	if (st_count_info.nUphCnt > 0)
	{
		dAve = st_count_info.dDailyPer / (double)st_count_info.nUphCnt;
	}
	strTemp.Format(_T("%.3f"), dAve);
	strContent.Format(_T("| %-100s |"), _T("SOCKET YIELD : "));
	strContent += strTemp;
	OnStringToChar(strContent, chMsg);
	fprintf(fp,"%s\r\n", chMsg) ;

	strContent = _T("********************************************************************************************************");
	OnStringToChar(strContent, chMsg);
	fprintf(fp,"%s\r\n", chMsg) ;
//	fprintf(fp, chMsg) ;
//	fprintf(fp, "\r\n") ;

	if (ferror(fp))  
	{
//		Func.MsgLog("���� ���� ����!..") ;
		fclose(fp); 

		return ;
	}

	fclose(fp); 
/*	
	strContent.Format("| %-100s |", "AMT8560 [SSD Interface Tester] : " + g_ver.GetDate() );
	fprintf(fp,"%s\r\n", mstr_content) ;
	
	mstr_content.Format("| %-100s |", "Day Data Info File");
	fprintf(fp,"%s\r\n", mstr_content) ;
	
	mstr_content.Format("| %-100s |", "DIVISION  : AMT Coporation");
	fprintf(fp,"%s\n", mstr_content) ;

	mstr_content.Format("| %-100s |", "File Name : " + st_path.str_lot_data_path + str_new_file);
	fprintf(fp,"%s\r\n", mstr_content) ;

	mstr_content.Format("| %-100s |", "Save Time : " + str_time);
	fprintf(fp,"%s\r\n", mstr_content) ;

	strTemp.Format("%06d", g_lotMgr.GetInputCnt(PRIME));
	mstr_content.Format("| %-100s |", "Input Prime : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;

	strTemp.Format("%06d", g_lotMgr.GetPassCnt(PRIME));
	mstr_content.Format("| %-100s |", "Pass  Prime : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;

	strTemp.Format("%06d", g_lotMgr.GetFailCnt(PRIME));
	mstr_content.Format("| %-100s |", "Fail Prime : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content);

	fMdlyield = 0;
	
	if (g_lotMgr.GetPassCnt(PRIME) == 0)
	{
		fMdlyield = 0;
	}
	else
	{
		fMdlyield = (double)g_lotMgr.GetPassCnt(PRIME) / (double)g_lotMgr.GetInputCnt(PRIME) * 100.0f;
	}
	
	strTemp.Format("%3.2f%%", fMdlyield);
	
	mstr_content.Format("| %-100s |", "Prime Yield  : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;


	// CUM
	strTemp.Format("%06d", g_lotMgr.GetInputCnt(CUM));
	mstr_content.Format("| %-100s |", "Input Cum   : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;
	
	strTemp.Format("%06d", g_lotMgr.GetPassCnt(CUM));
	mstr_content.Format("| %-100s |", "Pass  Cum   : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;
	
	strTemp.Format("%06d", g_lotMgr.GetFailCnt(CUM));
	mstr_content.Format("| %-100s |", "Fail Cum  : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;

	fMdlyield = 0;

	if (g_lotMgr.GetPassCnt(CUM) == 0)
	{
		fMdlyield = 0;
	}
	else
	{
		fMdlyield = (double)g_lotMgr.GetPassCnt(CUM) / (double)g_lotMgr.GetInputCnt(CUM) * 100.0f;
	}
	
	strTemp.Format("%3.2f%%", fMdlyield);
	
	mstr_content.Format("| %-100s |", "Cum   Yield  : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;
	mstr_content.Format("| %-100s |", "");
	fprintf(fp,"%s\r\n", mstr_content) ;

	strTemp.Format( "%02d:%02d:%02d", st_handler.m_tDR.GetTotalHours(), st_handler.m_tDR.GetMinutes(), st_handler.m_tDR.GetSeconds() );
	mstr_content.Format("| %-100s |", "Run Time : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;

	strTemp.Format( "%02d:%02d:%02d", st_handler.m_tDS.GetTotalHours(), st_handler.m_tDS.GetMinutes(), st_handler.m_tDS.GetSeconds() );
	mstr_content.Format("| %-100s |", "Stop Time : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;

	strTemp.Format( "%02d:%02d:%02d", st_handler.m_tDJ.GetTotalHours(), st_handler.m_tDJ.GetMinutes(), st_handler.m_tDJ.GetSeconds() );
	mstr_content.Format("| %-100s |", "Alarm Time : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;

	strTemp.Format( "%d", g_handler.GetAlarmCnt() );
	mstr_content.Format("| %-100s |", "Alarm Count : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;

	int nSec_MTBI = st_handler.m_tDR.GetTotalHours() * 3600 + st_handler.m_tDR.GetMinutes() * 60 + st_handler.m_tDR.GetSeconds();
	if( g_handler.GetAlarmCnt() == 0 )
		nSec_MTBI = 0;
	else
	{
		nSec_MTBI /= g_handler.GetAlarmCnt();
	}

	strTemp.Format( "%02d:%02d:%02d", nSec_MTBI / 3600, (nSec_MTBI % 3600) / 60, nSec_MTBI % 60 );
	mstr_content.Format("| %-100s |", "MTBI : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;

	mstr_content = "********************************************************************************************************";
	fprintf(fp,"%s\r\n", mstr_content) ;
	
	str_temp_data = "";
	
	if (ferror(fp))  
	{
		Func.MsgLog("���� ���� ����!..") ;
		return ;
	}
	
	fclose(fp);  /* ���� ���� */
}

void CPublicFunction::OnHourDataSave(CString strHourName, int nQty, double dSocketYield, int nStart, int nOn, int nOff, CString strOff, CString strOn)
{
	CString strFileName;				// ���������� ������ ���� �̸� ���� ���� 
	CString strCreateFile;				// �˶� ���� ������ ���Ͽ� ���� [����]+[���ϸ�]+[Ȯ����] ���� ���� 
	CString strListName, strTempData;	// �� �κк� �˶� �߻� Ƚ�� ���� ������ �α� ���� ���� ���� ���� 
	CString strContent;				// �� ���ο� ����� ���ڿ� ���� ���� ���� 
	int nExistence;						// ���� ���� ���� ���� �÷��� 
	char chFileName[256];				// �˻��� ���� ���� ���� �Լ� 
	char chMsg[1000];
	FILE  *fp ;							// ���Ͽ� ���� ������ ���� ���� 
	
	CString strTitleTime, strFileTime, strNewFile;		// ���Ͽ� ����� Ÿ��Ʋ �ð� ���� ����
	int nCurYear, nCurMonth, nCurDay;					// ��, ��, �� ���� ����
	int nCurHour, nCurMinute, nCurSecond;				// ��, ��, �� ���� ����


	double dAve;
	//double dTemp;
	//
	
	COleDateTime otCurr;									// ���� �ð� ���� ����
	CTime tCurr;										// Ÿ��Ʋ �ð� ���� ����

	CString strTemp;

	CString strData, strTime;
	
	/* ************************************************************************** */
	/* ���� �̸����� ����� ��¥ �����Ѵ�                                         */
	/* ************************************************************************** */
	otCurr = COleDateTime::GetCurrentTime();
	
	nCurYear		= otCurr.GetYear();  
	nCurMonth		= otCurr.GetMonth();  
    nCurDay			= otCurr.GetDay();  

	nCurHour		= otCurr.GetHour();
	nCurMinute		= otCurr.GetMinute();
	nCurSecond		= otCurr.GetSecond();
	
	strNewFile.Format(_T("HOUR_DATA%04d%02d%02d"), nCurYear, nCurMonth, nCurDay);
	strTime.Format(_T("%04d/%02d/%02d %02d:%02d:%02d"), nCurYear, nCurMonth, nCurDay, nCurHour, nCurMinute, nCurSecond);
		
	strCreateFile = st_path_info.strData + strHourName;
	strCreateFile += ".TXT";
	
	OnStringToChar(strCreateFile, chFileName);
	nExistence = _access(chFileName, 0);
	
	if (nExistence == -1)  /* ���� ������ */
	{
		strCreateFile = st_path_info.strData + strHourName;
		strCreateFile += ".TXT";
		OnStringToChar(strCreateFile, chFileName);

		/* ************************************************************************** */
		/* �˶� �߻� Ƚ�� ���� ���� ���Ͽ� �߰� ������ ���� ���Ϸ� ����               */
		/* ************************************************************************** */
		fopen_s(&fp, chFileName, "a+");
		if(!fp)
		{
			//		AfxMessageBox(_T("The failure because we open the file."));
			return;
		}

		/* ************************************************************************** */
		/* ���Ͽ� Ÿ��Ʋ ���� ����Ѵ�                                                */
		/* ************************************************************************** */
		strContent = _T("********************************************************************************************************");
		OnStringToChar(strContent, chMsg);
		fprintf(fp, "%s\r\n", chMsg) ;

		strContent.Format(_T("| %-100s |"), _T("AMT840 [SSD Interface] : "));
		OnStringToChar(strContent, chMsg);
		fprintf(fp, "%s\r\n", chMsg);

		strContent.Format(_T("| %-100s |"), _T("Day Data Info File"));
		OnStringToChar(strContent, chMsg);
		fprintf(fp,"%s\r\n", chMsg) ;

		strContent.Format(_T("| %-100s |"), _T("DIVISION  : AMT Coporation"));
		OnStringToChar(strContent, chMsg);
		fprintf(fp,"%s\n", chMsg) ;
	}
	else
	{
		/* ************************************************************************** */
		/* �˶� �߻� Ƚ�� ���� ���� ���Ͽ� �߰� ������ ���� ���Ϸ� ����               */
		/* ************************************************************************** */
		fopen_s(&fp, chFileName, "a+");
		if(!fp)
		{
			//		AfxMessageBox(_T("The failure because we open the file."));
			return;
		}
	}

	strContent.Format(_T("| %-100s |"), _T("Save Time : "));
	strContent += strTime;
	OnStringToChar(strContent, chMsg);
	fprintf(fp,"%s\r\n", chMsg) ;

	strTemp.Format(_T("%d"), st_handler_info.tRunUph.GetTotalMinutes());
	strContent.Format(_T("| %-100s |"), _T("RUN TIME : "));
	strContent += strTemp;
	OnStringToChar(strContent, chMsg);
	fprintf(fp,"%s\r\n", chMsg) ;

	st_count_info.nDailyUph += nQty;
	st_count_info.nUphCnt++;

	strTemp.Format(_T("%d"), st_count_info.nDailyUph);
	strContent.Format(_T("| %-100s |"), _T("TOTAL : "));
	strContent += strTemp;
	OnStringToChar(strContent, chMsg);
	fprintf(fp,"%s\r\n", chMsg) ;

	strTemp.Format(_T("%d"), nQty);
	strContent.Format(_T("| %-100s |"), _T("QTY : "));
	strContent += strTemp;
	OnStringToChar(strContent, chMsg);
	fprintf(fp,"%s\r\n", chMsg) ;

	strTemp.Format(_T("%d"), nStart);
	strContent.Format(_T("| %-100s |"), _T("SOCKET : "));
	strContent += strTemp;
	OnStringToChar(strContent, chMsg);
	fprintf(fp,"%s\r\n", chMsg) ;

	st_count_info.dDailyPer += dSocketYield;
	dAve = 0.0f;
	if (st_count_info.nUphCnt > 0)
	{
		dAve = st_count_info.dDailyPer / (double)st_count_info.nUphCnt;
	}
	strTemp.Format(_T("Yield : %.2f/%.2f Count : %d Slot : %s"), dSocketYield, dAve, nOn, strOn);
	strContent.Format(_T("| %-100s |"), _T("SOCKET ON : "));
	strContent += strTemp;
	OnStringToChar(strContent, chMsg);
	fprintf(fp,"%s\r\n", chMsg) ;

	strTemp.Format(_T("%d Slot : %s"), nOff, strOff);
	strContent.Format(_T("| %-100s |"), _T("SOCKET OFF : "));
	strContent += strTemp;
	OnStringToChar(strContent, chMsg);
	fprintf(fp,"%s\r\n", chMsg) ;
	
	strContent = _T("********************************************************************************************************");
	OnStringToChar(strContent, chMsg);
	fprintf(fp,"%s\r\n", chMsg) ;

	if (ferror(fp))  
	{
//		Func.MsgLog("���� ���� ����!..") ;
		fclose(fp); 

		return ;
	}

	fclose(fp); 
}

void CPublicFunction::OnLogFileAdd(int nMsgSelect, CString strMsg)
{
	CFileFind filefind;
	CString mstr_cur_year, mstr_cur_month, mstr_cur_day, str_display_time; // ���� ��, ��, �� ���� ���������� ��ȯ�Ͽ� ������ ���� 
	int mn_cur_year, mn_cur_month, mn_cur_day, mn_cur_hour; // ���� ��, ��, �� ���� ���� ���� 
	CString mstr_file_name;		// ���������� ������ ���� �̸� ���� ���� 
	CString mstr_create_file;	// �˶� ���� ������ ���Ͽ� ���� [����]+[���ϸ�]+[Ȯ����] ���� ���� 
	CString mstr_list_name, mstr_temp_data;  // �� �κк� �˶� �߻� Ƚ�� ���� ������ �α� ���� ���� ���� ���� 
	CString mstr_content;		// �� ���ο� ����� ���ڿ� ���� ���� ���� 
	COleDateTime time_cur;		// �˻��� �ð� ���� ���� ���� 
	CTime m_time_current;		// ������ ����� ���� �ð� ���� ���� ����
	int mn_existence;			// ���� ���� ���� ���� �÷��� 
	char fileName[256];			// �˻��� ���� ���� ���� �Լ� 
	char chFileName[256];
	char chBackName[256];
	FILE  *fp ;					// ���Ͽ� ���� ������ ���� ����

	CString BackupName;
	CString mstr_cur_hour, mstr_cur_min, mstr_cur_sec;
	int Ret;

	if (strMsg.IsEmpty() == TRUE)  
	{
		return;
	}

	CSingleLock sing(&mutex_f);

	sing.Lock();
	// **************************************************************************
	// ���� �̸����� ����� ��¥ ������ ��´�                                   
	// **************************************************************************
	time_cur = COleDateTime::GetCurrentTime();  // ���� �ð� ������ ��´�. 

	m_time_current = CTime::GetCurrentTime() ;  // ������ ������ ���� �ð� ���� ��´�. 

	mn_cur_year = time_cur.GetYear();  

	mn_cur_month = time_cur.GetMonth();  

    mn_cur_day = time_cur.GetDay();  

	mn_cur_hour = time_cur.GetHour();
	// **************************************************************************

	// **************************************************************************
	// ��¥ ������ ���������� ��ȯ�Ͽ� ������ �����Ѵ�                           
	// **************************************************************************
	mstr_cur_year.Format(_T("%04d"), mn_cur_year);  
	mstr_cur_month.Format(_T("%02d"), mn_cur_month);
	mstr_cur_day.Format(_T("%02d"), mn_cur_day);

	// **************************************************************************
	// ���� �ð� ���� ��´�                                                     
	// **************************************************************************
	mn_cur_hour = time_cur.GetHour();				// ���� �ð� ������ �����Ѵ�. 
	mstr_cur_hour.Format(_T("%d"),time_cur.GetHour());	// ���� �ð� ������ �����Ѵ�.
	mstr_cur_min.Format(_T("%d"),time_cur.GetMinute());	// ���� �� ������ �����Ѵ�. 
	mstr_cur_sec.Format(_T("%d"),time_cur.GetSecond());	// ���� �� ������ �����Ѵ�. 
	str_display_time = m_time_current.Format(_T("%c"));	// ����Ʈ ���Ͽ� ����� Ÿ��Ʋ �ð� ���� ���� 
	// **************************************************************************

	switch(nMsgSelect)			// Operation Setting
	{		
		case 0:
		case 1:
			mstr_file_name = _T("OP") + mstr_cur_year;
			mstr_file_name += mstr_cur_month; 
			mstr_file_name += mstr_cur_day; 
			mstr_create_file = st_path_info.strOperation + mstr_file_name;
			break;
		case 2:							// ��� ���� ����.
			mstr_file_name = _T("MA") + mstr_cur_year;
			mstr_file_name += mstr_cur_month; 
			mstr_file_name += mstr_cur_day; 
			mstr_create_file = st_path_info.strMachine + mstr_file_name;
			break;
		case 3:							// ��� ����� ����.
			mstr_file_name = _T("DEBUG") + mstr_cur_year;
			mstr_file_name += mstr_cur_month; 
			mstr_file_name += mstr_cur_day; 
			mstr_create_file = st_path_info.strMachine + mstr_file_name;
			break;
		case 4:							// ��� ����� ����.
			mstr_file_name = _T("IT") + mstr_cur_year;
			mstr_file_name += mstr_cur_month; 
			mstr_file_name += mstr_cur_day; 
			mstr_create_file = st_path_info.strInterface + mstr_file_name;
			break;

		case 80:						// ��ü �޼���.
			mstr_file_name = _T("GMS") + mstr_cur_year;
			mstr_file_name += mstr_cur_month; 
			mstr_file_name += mstr_cur_day; 
			mstr_create_file = st_path_info.strGms + mstr_file_name;
			break;

		case 99:						// ��ü �޼���.
			mstr_file_name = _T("TO") + mstr_cur_year;
			mstr_file_name += mstr_cur_month; 
			mstr_file_name += mstr_cur_day; 
			mstr_create_file = st_path_info.strTotal + mstr_file_name;
			break;

	
	}

	mstr_create_file += _T(".TXT");

	//wsprintfA(fileName, "%S", mstr_create_file);
	clsFunc.OnStringToChar(mstr_create_file, fileName);

	mn_existence = filefind.FindFile(mstr_create_file,0);
	
	if (mn_existence == -1)
	{
		switch(nMsgSelect)			// Operation Setting
		{		
			case 0:
			case 1:
				mstr_file_name = _T("OP") + mstr_cur_year;
				mstr_file_name += mstr_cur_month; 
				mstr_file_name += mstr_cur_day; 
				mstr_create_file = st_path_info.strOperation + mstr_file_name;
				break;

			case 2:							// ��� ���� ����.
				mstr_file_name = _T("MA") + mstr_cur_year;
				mstr_file_name += mstr_cur_month; 
				mstr_file_name += mstr_cur_day; 
				mstr_create_file = st_path_info.strMachine + mstr_file_name;
				break;

			case 3:							// ��� ����� ����.
				mstr_file_name = _T("DEBUG") + mstr_cur_year;
				mstr_file_name += mstr_cur_month; 
				mstr_file_name += mstr_cur_day; 
				mstr_create_file = st_path_info.strMachine + mstr_file_name;
				break;

			case 4:							// ��� ����� ����.
				mstr_file_name = _T("IT") + mstr_cur_year;
				mstr_file_name += mstr_cur_month; 
				mstr_file_name += mstr_cur_day; 
				mstr_create_file = st_path_info.strInterface + mstr_file_name;
				break;

			case 99:						// ��ü �޼���.
				mstr_file_name = _T("TOT") + mstr_cur_year;
				mstr_file_name += mstr_cur_month; 
				mstr_file_name += mstr_cur_day; 
				mstr_create_file = st_path_info.strTotal + mstr_file_name;
				break;
		}
		mstr_create_file += _T(".TXT");
		// **********************************************************************
	}

	Ret = OnFileSizeCheck(mstr_create_file, 1048576, YES); //size and rename
	
	if(Ret == 1) //file size over
	{
		//
		BackupName = mstr_create_file + mstr_cur_hour + mstr_cur_min +  mstr_cur_sec + _T(".bak") ;
		
//		wsprintfA(chFileName, "%S", mstr_create_file);
		clsFunc.OnStringToChar(mstr_create_file, chFileName);
//		wsprintfA(chBackName, "%S", BackupName);
		clsFunc.OnStringToChar(BackupName, chBackName);
		rename(chFileName, chBackName);
	}

	// **************************************************************************
	// �˶� �߻� Ƚ�� ���� ���� ���Ͽ� �߰� ������ ���� ���Ϸ� ����              
	// **************************************************************************
	char chMode[10];
//	wsprintfA(chMode, "%S", _T("a+"));
	clsFunc.OnStringToChar(_T("a+"), chMode);
// jtkim 20130704
//	if ((fp = fopen(fileName, chMode)) == NULL) 
//	if ((fp = fopen_s(&fp,fileName, chMode)) == NULL) 
	fopen_s(&fp, fileName, "a+");
	if(!fp)
	{
//		AfxMessageBox(_T("File open failure!.."));

		return;
	}
	// **************************************************************************

	// **************************************************************************
	// �α� ���Ͽ� ���� �߻��� �˶� ���� �����Ѵ�                                
	// **************************************************************************
	if (st_handler_info.strUserId != _T(""))
	{
		mstr_content += "[";
		mstr_content += st_handler_info.strUserId;
		mstr_content += "] ";
	}
	
	mstr_content += str_display_time;
	mstr_content += _T(" , ") + strMsg;
	mstr_content += _T("\n");

	char chData[20000];
//	wsprintfA(chData, "%S", mstr_content);
	if (mstr_content.GetLength() > 20000) 
	{
		fclose(fp);

		return ;
	}

	clsFunc.OnStringToChar(mstr_content, chData);
//	fprintf(fp,_T("%s\n"),mstr_content) ;
	
	fprintf(fp, chData) ;
	if (ferror(fp))  
	{
		if (st_handler_info.cWndList != NULL)  // ����Ʈ �� ȭ�� ����
		{
//			st_other.str_op_msg = _T(_T("File save failure."));
//			wsprintfA(st_other_info.cNormalMsg, "%S", _T("File save failure."));
			//clsFunc.OnStringToChar(_T("File save failure."), st_other_info.cNormalMsg);
			clsMem.OnNormalMessageWrite(_T("File save failure."));
			//st_handler_info.cWndList->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
		}

		sing.Unlock();
		fclose(fp);

		return ;
	}
	// **************************************************************************
	fclose(fp);  // ������ �����Ѵ�.

	sing.Unlock();
}

int CPublicFunction::OnFileSizeCheck(CString strFileName, long lSize, int nCheck)
{
	CFileFind finder;
	long Len=0, flag=0;
	BOOL bContinue;

	if(bContinue = finder.FindFile(strFileName))
	{	
		if (nCheck == YES)			// Size�� üũ�ϴ°��̸�...
		{
			finder.FindFile(strFileName);
			finder.FindNextFile();
			finder.GetFileName();
			Len = (long)finder.GetLength();
			if(Len > lSize)
			{
				flag = 1;
			}
			finder.Close();	
		}
		else						// Size�� Ȯ���ϴ°��̸�...
		{
			finder.FindFile(strFileName);
			finder.FindNextFile();
			finder.GetFileName();
			Len = (long)finder.GetLength();
			
			flag = Len;
			
			finder.Close();	
		}
	}
	
	return flag;
}


int CPublicFunction::OnLightCurtainCheck()
{
	int nFuncRet = RET_GOOD;

	int nRet_1 = OnLdUldCurtainCheck();
	int nRet_2 = OnRejCurtainCheck();

	if( nRet_1 == RET_ERROR || nRet_2 == RET_ERROR )
	{
		nFuncRet =RET_ERROR;
	}
	else
	{
		nFuncRet = RET_GOOD;
	}

	return nFuncRet;
}

int CPublicFunction::OnLdUldCurtainCheck()
{
	int nFuncRet = RET_GOOD;
	CString strErr;
	m_nSafetyChk[0] = FAS_IO.get_out_bit( st_io_info.o_LdUldMuteOn, IO_ON);
	m_nSafetyChk[1] = FAS_IO.get_in_bit( st_io_info.i_LdUldLightChk, IO_OFF);//sense
	m_nSafetyChk[2] = FAS_IO.get_in_bit( st_io_info.i_LdUldStkLctChk, IO_OFF); //PLC

	//kwlee 2016.0826
	if (st_basic_info.nModeLdUldLightCutainSkip == YES)
	{
		return RET_GOOD;
	}

	switch(m_safetyStep1)
	{
		case 0:
			if( m_nSafetyChk[1] == IO_ON || m_nSafetyChk[2] == IO_ON )
			{
				st_sync_info.nLdUld_LightChk = NO;
				m_safetyStep1 = 100;
				m_dwWaitSafetyTime[0][0] = GetCurrentTime();
			}
			else
			{
				if( st_sync_info.nLdUld_LightChk == YES)
				{
					st_sync_info.nLdUld_LightChk = NO;
				}

			}
			break;

		case 100:
			if( m_nSafetyChk[1] == IO_ON || m_nSafetyChk[2] == IO_ON )
			{
				m_dwWaitSafetyTime[0][1] = GetCurrentTime();
				m_dwWaitSafetyTime[0][2] = m_dwWaitSafetyTime[0][1] - m_dwWaitSafetyTime[0][0];
				if( m_dwWaitSafetyTime[0][2] <= 0) 
				{
					m_dwWaitSafetyTime[0][0] = GetCurrentTime();
				}
				else if( m_dwWaitSafetyTime[0][2] > 100 )
				{
					m_safetyStep1 = 200;
				}
			}
			else
			{
				m_safetyStep1 = 0;
			}
			break;

		case 200:
			if( m_nSafetyChk[1] == IO_ON || m_nSafetyChk[2] == IO_ON )
			{				
				if( m_nSafetyChk[0] == CTL_YES)
				{
					m_dwWaitSafetyTime[0][0] = GetCurrentTime();
					strErr.Format( _T("Safety : Load unload light curtain check error" ) );					
					st_handler_info.mstr_event_msg[0] = strErr;
					::PostMessage(st_handler_info.hWnd, WM_MAIN_EVENT, CTL_YES, 0);	
					st_sync_info.nLdUld_LightChk = YES;
					m_safetyStep1 = 300;
				}
				else
				{
					//801214 1 0 ""LOAD/UNLOAD_ZONE_LIGHT_CURTAIN_OFF_CHECK_ERROR"
					CTL_Lib.Alarm_Error_Occurrence(9007, dWARNING, _T("801214"));
					nFuncRet =RET_ERROR;
					m_safetyStep1 = 0;
				}
			}
			else
			{
				m_safetyStep1 = 0;
			}
			break;

		case 300:
			if( m_nSafetyChk[1] == IO_ON || m_nSafetyChk[2] == IO_ON )
			{
				if( m_nSafetyChk[0] == CTL_YES && st_sync_info.nLdUld_LightChk == YES)
				{
					m_dwWaitSafetyTime[0][1] = GetCurrentTime();
					m_dwWaitSafetyTime[0][2] = m_dwWaitSafetyTime[0][1] - m_dwWaitSafetyTime[0][0];
					if( m_dwWaitSafetyTime[0][2] <= 0) 
					{
						m_dwWaitSafetyTime[0][0] = GetCurrentTime();
					}
					else if( m_dwWaitSafetyTime[0][2] > 20000 )
					{
						m_safetyStep1 = 0;
						::PostMessage(st_handler_info.hWnd, WM_MAIN_EVENT, CTL_NO, 0);
						//801214 1 0 ""LOAD/UNLOAD_ZONE_LIGHT_CURTAIN_OFF_CHECK_ERROR"
						CTL_Lib.Alarm_Error_Occurrence(9008, dWARNING, _T("801214"));
						nFuncRet =RET_ERROR;
					}
				}
				else
				{
					::PostMessage(st_handler_info.hWnd, WM_MAIN_EVENT, CTL_NO, 0);
					//801214 1 0 ""LOAD/UNLOAD_ZONE_LIGHT_CURTAIN_OFF_CHECK_ERROR"
					CTL_Lib.Alarm_Error_Occurrence(9007, dWARNING, _T("801214"));
					nFuncRet =RET_ERROR;
					m_safetyStep1 = 0;
				}
			}
			else
			{
				::PostMessage(st_handler_info.hWnd, WM_MAIN_EVENT, CTL_NO, 0);
				m_safetyStep1 = 0;
			}
			break;
	}
	return nFuncRet;
}

int CPublicFunction::OnRejCurtainCheck()
{
	int nFuncRet = RET_GOOD;
	CString strErr;
	m_nSafetyChk[3] = FAS_IO.get_out_bit( st_io_info.o_RejMuteOn, IO_ON);
	m_nSafetyChk[4] = FAS_IO.get_in_bit( st_io_info.i_RejectLightChk, IO_OFF);//sense
	m_nSafetyChk[5] = FAS_IO.get_in_bit( st_io_info.i_RejStkLctChk, IO_OFF); //PLC

	//kwlee 2016.0826
	if (st_basic_info.nModeRejLightCutainSkip == YES)
	{
		return RET_GOOD;
	}

	switch(m_safetyStep2)
	{
		case 0:
			if( m_nSafetyChk[4] == IO_ON || m_nSafetyChk[5] == IO_ON )
			{
				st_sync_info.nRej_LightChk = NO;
				m_safetyStep2 = 100;
				m_dwWaitSafetyTime[1][0] = GetCurrentTime();
			}
			else
			{
				if( st_sync_info.nRej_LightChk == YES)
				{
					st_sync_info.nRej_LightChk = NO;
				}
			}
			break;

		case 100:
			if( m_nSafetyChk[1] == IO_ON || m_nSafetyChk[2] == IO_ON )
			{
				m_dwWaitSafetyTime[1][1] = GetCurrentTime();
				m_dwWaitSafetyTime[1][2] = m_dwWaitSafetyTime[1][1] - m_dwWaitSafetyTime[1][0];
				if( m_dwWaitSafetyTime[1][2] <= 0) 
				{
					m_dwWaitSafetyTime[1][0] = GetCurrentTime();
				}
				else if( m_dwWaitSafetyTime[1][2] > 100 )
				{
					m_safetyStep2 = 200;
				}
			}
			else
			{
				m_safetyStep2 = 0;
			}
			break;

		case 200:
			if( m_nSafetyChk[4] == IO_ON || m_nSafetyChk[5] == IO_ON )
			{
				if( m_nSafetyChk[3] == CTL_YES)
				{
					m_dwWaitSafetyTime[1][0] = GetCurrentTime();
					strErr.Format( _T("Safety : Reject light curtain check error" ) );					
					st_handler_info.mstr_event_msg[0] = strErr;
					::PostMessage(st_handler_info.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
					st_sync_info.nRej_LightChk = YES;
					m_safetyStep2 = 300;
				}
				else
				{
					::PostMessage(st_handler_info.hWnd, WM_MAIN_EVENT, CTL_NO, 0);
					//801215 1 0 "REJECT_ZONE_LIGHT_CURTAIN_OFF_CHECK_ERROR"
					CTL_Lib.Alarm_Error_Occurrence(9009, dWARNING, _T("801215"));
					nFuncRet =RET_ERROR;
					m_safetyStep2 = 0;
				}
			}
			else
			{
				m_safetyStep2 = 0;
			}
			break;

		case 300:
			if( m_nSafetyChk[4] == IO_ON || m_nSafetyChk[5] == IO_ON )
			{
				if( m_nSafetyChk[3] == CTL_YES && st_sync_info.nRej_LightChk == YES)
				{
					m_dwWaitSafetyTime[1][1] = GetCurrentTime();
					m_dwWaitSafetyTime[1][2] = m_dwWaitSafetyTime[1][1] - m_dwWaitSafetyTime[1][0];
					if( m_dwWaitSafetyTime[1][2] <= 0) 
					{
						m_dwWaitSafetyTime[1][0] = GetCurrentTime();
					}
					else if( m_dwWaitSafetyTime[1][2] > 20000 )
					{
						m_safetyStep2 = 0;
						::PostMessage(st_handler_info.hWnd, WM_MAIN_EVENT, CTL_NO, 0);
						//801215 1 0 ""REJECT_ZONE_LIGHT_CURTAIN_OFF_CHECK_ERROR"
						CTL_Lib.Alarm_Error_Occurrence(9100, dWARNING, _T("801215"));
						nFuncRet =RET_ERROR;
					}
				}
				else
				{
					m_safetyStep2 = 0;
					::PostMessage(st_handler_info.hWnd, WM_MAIN_EVENT, CTL_NO, 0);
					//801215 1 0 ""REJECT_ZONE_LIGHT_CURTAIN_OFF_CHECK_ERROR"
					CTL_Lib.Alarm_Error_Occurrence(9100, dWARNING, _T("801215"));
					nFuncRet =RET_ERROR;
				}
			}
			else
			{
				::PostMessage(st_handler_info.hWnd, WM_MAIN_EVENT, CTL_NO, 0);
				m_safetyStep2 = 0;
			}
			break;
	}
	return nFuncRet;
}


int	CPublicFunction::OnMpCheck()
{
	long dwMotionDone=0;//2015.0308 
	int nFlag = 0;
/*
	//left m/p 
	if (FAS_IO.get_in_bit(st_io_info.i_LfTsiteDockSafetyChk, IO_ON) == IO_OFF)
	{
		if (FAS_IO.get_in_bit(st_io_info.i_LfLctChk, IO_OFF) == IO_OFF)
		{
			CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, _T("812415"));

			//2015.0309 
			nFlag = 0;
			cmmSxIsDone(M_TSITE_RBT_X, &dwMotionDone);	//�������� �̵�üũ 
			if(dwMotionDone != cmTRUE) nFlag++;
			cmmSxIsDone(M_TSITE_RBT_Y, &dwMotionDone);	//�������� �̵�üũ 
			if(dwMotionDone != cmTRUE) nFlag++;
			cmmSxIsDone(M_TSITE_RBT_Z, &dwMotionDone);	//�������� �̵�üũ 
			if(dwMotionDone != cmTRUE) nFlag++;

			if(1 || nFlag > 0)
			{
				COMI.Set_MotStop(0, M_TSITE_RBT_X);//(1, m_nMotorNum) ; //�������   //md_cmdpos_backup[n_Axis]
				COMI.Set_MotStop(0, M_TSITE_RBT_Y);
				COMI.Set_MotStop(0, M_TSITE_RBT_Z);
			} 	 

			return RET_ERROR;
		}
	}
	else
	{ 
		if(st_var.nCokBuffer_Change_End_Flag == 0)//20150410 james 
		{
			if (FAS_IO.get_in_bit(st_io_info.i_LfManipulatorLfClampFwdChk, IO_ON)	== IO_OFF ||
				FAS_IO.get_in_bit(st_io_info.i_LfManipulatorLfClampBwdChk, IO_OFF)	== IO_ON)
			{
				CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, _T("813709"));

				return RET_ERROR;
			}

			if (FAS_IO.get_in_bit(st_io_info.i_LfManipulatorRiClampFwdChk, IO_ON)	== IO_OFF ||
				FAS_IO.get_in_bit(st_io_info.i_LfManipulatorRiClampBwdChk, IO_OFF)	== IO_ON)
			{
				CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, _T("813711"));

				return RET_ERROR;
			}

			if (FAS_IO.get_in_bit(st_io_info.i_LfTsiteLfTopHiFixContactFwdChk, IO_ON)	== IO_OFF ||
				FAS_IO.get_in_bit(st_io_info.i_LfTsiteLfTopHiFixContactBwdChk, IO_OFF)	== IO_ON)
			{
				CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, _T("812409"));

				return RET_ERROR;
			}

			if (FAS_IO.get_in_bit(st_io_info.i_LfTsiteLfBtmHiFixContactFwdChk, IO_ON)	== IO_OFF ||
				FAS_IO.get_in_bit(st_io_info.i_LfTsiteLfBtmHiFixContactBwdChk, IO_OFF)	== IO_ON)
			{
				CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, _T("812411"));

				return RET_ERROR;
			}

			if (FAS_IO.get_in_bit(st_io_info.i_LfTsiteRiTopHiFixContactFwdChk, IO_ON)	== IO_OFF ||
				FAS_IO.get_in_bit(st_io_info.i_LfTsiteRiTopHiFixContactBwdChk, IO_OFF)	== IO_ON)
			{
				CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, _T("812413"));

				return RET_ERROR;
			}

			if (FAS_IO.get_in_bit(st_io_info.i_LfTsiteBtmCokLfClampFwdChk, IO_ON)	== IO_OFF ||
				FAS_IO.get_in_bit(st_io_info.i_LfTsiteBtmCokLfClampBwdChk, IO_OFF)	== IO_ON)
			{
				CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, _T("812201"));

				return RET_ERROR;
			}

			if (FAS_IO.get_in_bit(st_io_info.i_LfTsiteBtmCokRiClampFwdChk, IO_ON)	== IO_OFF ||
				FAS_IO.get_in_bit(st_io_info.i_LfTsiteBtmCokRiClampBwdChk, IO_OFF)	== IO_ON)
			{
				CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, _T("812203"));

				return RET_ERROR;
			}

			if (FAS_IO.get_in_bit(st_io_info.i_LfTsiteTopCokLfClampFwdChk, IO_ON)	== IO_OFF ||
				FAS_IO.get_in_bit(st_io_info.i_LfTsiteTopCokLfClampBwdChk, IO_OFF)	== IO_ON)
			{
				CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, _T("812601"));

				return RET_ERROR;
			}

			if (FAS_IO.get_in_bit(st_io_info.i_LfTsiteTopCokRiClampFwdChk, IO_ON)	== IO_OFF ||
				FAS_IO.get_in_bit(st_io_info.i_LfTsiteTopCokRiClampBwdChk, IO_OFF)	== IO_ON)
			{
				CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, _T("812603"));

				return RET_ERROR;
			}

			if (FAS_IO.get_in_bit(st_io_info.i_LfTsiteBtmTesterLfContactFwdChk, IO_ON)	== IO_OFF ||
				FAS_IO.get_in_bit(st_io_info.i_LfTsiteBtmTesterLfContactBwdChk, IO_OFF)	== IO_ON)
			{
				CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, _T("812313"));

				return RET_ERROR;
			}

			if (FAS_IO.get_in_bit(st_io_info.i_LfTsiteBtmTesterRiContactFwdChk, IO_ON)	== IO_OFF ||
				FAS_IO.get_in_bit(st_io_info.i_LfTsiteBtmTesterRiContactBwdChk, IO_OFF)	== IO_ON)
			{
				CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, _T("812315"));

				return RET_ERROR;
			}

			if (FAS_IO.get_in_bit(st_io_info.i_LfTsiteTopTesterLfContactFwdChk, IO_ON)	== IO_OFF ||
				FAS_IO.get_in_bit(st_io_info.i_LfTsiteTopTesterLfContactBwdChk, IO_OFF)	== IO_ON)
			{
				CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, _T("812512"));

				return RET_ERROR;
			}

			if (FAS_IO.get_in_bit(st_io_info.i_LfTsiteTopTesterRiContactFwdChk, IO_ON)	== IO_OFF ||
				FAS_IO.get_in_bit(st_io_info.i_LfTsiteTopTesterRiContactBwdChk, IO_OFF)	== IO_ON)
			{
				CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, _T("812514"));

				return RET_ERROR;
			}
		}
	}


	//right m/p
	if (FAS_IO.get_in_bit(st_io_info.i_RiTsiteDockSafetyChk, IO_ON) == IO_OFF)
	{
		if (FAS_IO.get_in_bit(st_io_info.i_RiLctChk, IO_OFF) == IO_OFF)
		{
			CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, _T("813315"));

			//2015.0309 
			nFlag = 0;
			cmmSxIsDone(M_TSITE_RBT_X, &dwMotionDone);	//�������� �̵�üũ 
			if(dwMotionDone != cmTRUE) nFlag++;
			cmmSxIsDone(M_TSITE_RBT_Y, &dwMotionDone);	//�������� �̵�üũ 
			if(dwMotionDone != cmTRUE) nFlag++;
			cmmSxIsDone(M_TSITE_RBT_Z, &dwMotionDone);	//�������� �̵�üũ 
			if(dwMotionDone != cmTRUE) nFlag++;

			if(1 || nFlag > 0)
			{
				COMI.Set_MotStop(0, M_TSITE_RBT_X);//(1, m_nMotorNum) ; //�������   //md_cmdpos_backup[n_Axis]
				COMI.Set_MotStop(0, M_TSITE_RBT_Y);
				COMI.Set_MotStop(0, M_TSITE_RBT_Z);
			} 	 

			return RET_ERROR; 
		}
	}
	else
	{
		if(st_var.nCokBuffer_Change_End_Flag == 0)//20150410 james 
		{
			if (FAS_IO.get_in_bit(st_io_info.i_RiManipulatorLfClampFwdChk, IO_ON)	== IO_OFF ||
				FAS_IO.get_in_bit(st_io_info.i_RiManipulatorLfClampBwdChk, IO_OFF)	== IO_ON)
			{
				CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, _T("813713"));

				return RET_ERROR;
			}

			if (FAS_IO.get_in_bit(st_io_info.i_RiManipulatorRiClampFwdChk, IO_ON)	== IO_OFF ||
				FAS_IO.get_in_bit(st_io_info.i_RiManipulatorRiClampBwdChk, IO_OFF)	== IO_ON)
			{
				CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, _T("813715"));

				return RET_ERROR;
			}

			if (FAS_IO.get_in_bit(st_io_info.i_RiTsiteLfTopHiFixContactFwdChk, IO_ON)	== IO_OFF ||
				FAS_IO.get_in_bit(st_io_info.i_RiTsiteLfTopHiFixContactBwdChk, IO_OFF)	== IO_ON)
			{
				CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, _T("813309"));

				return RET_ERROR;
			}

			if (FAS_IO.get_in_bit(st_io_info.i_RiTsiteLfBtmHiFixContactFwdChk, IO_ON)	== IO_OFF ||
				FAS_IO.get_in_bit(st_io_info.i_RiTsiteLfBtmHiFixContactBwdChk, IO_OFF)	== IO_ON)
			{
				CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, _T("813311"));

				return RET_ERROR;
			}

			if (FAS_IO.get_in_bit(st_io_info.i_RiTsiteRiTopHiFixContactFwdChk, IO_ON)	== IO_OFF ||
				FAS_IO.get_in_bit(st_io_info.i_RiTsiteRiTopHiFixContactBwdChk, IO_OFF)	== IO_ON)
			{
				CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, _T("813313"));

				return RET_ERROR;
			}

			if (FAS_IO.get_in_bit(st_io_info.i_RiTsiteBtmCokLfClampFwdChk, IO_ON)	== IO_OFF ||
				FAS_IO.get_in_bit(st_io_info.i_RiTsiteBtmCokLfClampBwdChk, IO_OFF)	== IO_ON)
			{
				CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, _T("813101"));

				return RET_ERROR;
			}

			if (FAS_IO.get_in_bit(st_io_info.i_RiTsiteBtmCokRiClampFwdChk, IO_ON)	== IO_OFF ||
				FAS_IO.get_in_bit(st_io_info.i_RiTsiteBtmCokRiClampBwdChk, IO_OFF)	== IO_ON)
			{
				CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, _T("813103"));

				return RET_ERROR;
			}

			if (FAS_IO.get_in_bit(st_io_info.i_RiTsiteTopCokLfClampFwdChk, IO_ON)	== IO_OFF ||
				FAS_IO.get_in_bit(st_io_info.i_RiTsiteTopCokLfClampBwdChk, IO_OFF)	== IO_ON)
			{
				CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, _T("813501"));

				return RET_ERROR;
			}

			if (FAS_IO.get_in_bit(st_io_info.i_RiTsiteTopCokRiClampFwdChk, IO_ON)	== IO_OFF ||
				FAS_IO.get_in_bit(st_io_info.i_RiTsiteTopCokRiClampBwdChk, IO_OFF)	== IO_ON)
			{
				CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, _T("813503"));

				return RET_ERROR;
			}

			if (FAS_IO.get_in_bit(st_io_info.i_RiTsiteBtmTesterLfContactFwdChk, IO_ON)	== IO_OFF ||
				FAS_IO.get_in_bit(st_io_info.i_RiTsiteBtmTesterLfContactBwdChk, IO_OFF)	== IO_ON)
			{
				CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, _T("813213"));

				return RET_ERROR;
			}

			if (FAS_IO.get_in_bit(st_io_info.i_RiTsiteBtmTesterRiContactFwdChk, IO_ON)	== IO_OFF ||
				FAS_IO.get_in_bit(st_io_info.i_RiTsiteBtmTesterRiContactBwdChk, IO_OFF)	== IO_ON)
			{
				CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, _T("813215"));

				return RET_ERROR;
			}

			if (FAS_IO.get_in_bit(st_io_info.i_RiTsiteTopTesterLfContactFwdChk, IO_ON)	== IO_OFF ||
				FAS_IO.get_in_bit(st_io_info.i_RiTsiteTopTesterLfContactBwdChk, IO_OFF)	== IO_ON)
			{
				CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, _T("813412"));

				return RET_ERROR;
			}

			if (FAS_IO.get_in_bit(st_io_info.i_RiTsiteTopTesterRiContactFwdChk, IO_ON)	== IO_OFF ||
				FAS_IO.get_in_bit(st_io_info.i_RiTsiteTopTesterRiContactBwdChk, IO_OFF)	== IO_ON)
			{
				CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, _T("813414"));

				return RET_ERROR;
			}
		}
	}*/
	//
	/* 2015.0308 ���� �����忡�� ó��
	if ( FAS_IO.get_in_bit(st_io_info.i_UnldTrayRail1Unld2LctChk, IO_OFF) == IO_OFF)
	{
		if ( FAS_IO.get_in_bit(st_io_info.i_UnldTrayRail1Unld2OutTrayChk,	IO_ON) != IO_ON)
		{
			if(st_sync_info.nConveyor_AcMotorCtl_Req[THD_UNDER_UNLOAD_TRAY_STACKER_SITE] != CTL_REQ)
			{
				m_strAlarmCode.Format(_T("81%04d"), st_io_info.i_UnldTrayRail1Unld2LctChk);
				CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, m_strAlarmCode);
				return RET_ERROR;
			}			
		}
	}*/

	return RET_GOOD;
}

int CPublicFunction::OnDoorOpenCheck()
{
	int i;
	CString strTemp;

	for (i=0; i<10; i++)
	{
		if (FAS_IO.get_in_bit(st_io_info.i_TopLdUldDoorChk[i], IO_ON) == IO_OFF)
		{
			m_strAlarmCode.Format(_T("81%04d"), st_io_info.i_TopLdUldDoorChk[i]);
//			CTL_Lib.Alarm_Error_Occurrence(6000+i, dWARNING, strTemp);

			st_alarm_info.strCode.Format(_T("81%04d"), st_io_info.i_TopLdUldDoorChk[i]);
			st_alarm_info.nCountMode	= 0;	
			st_alarm_info.nTypeMode		= eWARNING;
			st_alarm_info.nAlarmAssignSection = 2;

			return RET_ERROR;
		}
	}
	for (i=0; i<4; i++)
	{
		if (FAS_IO.get_in_bit(st_io_info.i_BtmLdUldDoorChk[i], IO_ON) == IO_OFF)
		{
			m_strAlarmCode.Format(_T("81%04d"), st_io_info.i_BtmLdUldDoorChk[i]);
//			CTL_Lib.Alarm_Error_Occurrence(6100+i, dWARNING, strTemp);
			st_alarm_info.strCode.Format(_T("81%04d"), st_io_info.i_BtmLdUldDoorChk[i]);
			st_alarm_info.nCountMode	= 0;	
			st_alarm_info.nTypeMode		= eWARNING;
			st_alarm_info.nAlarmAssignSection = 2;

			return RET_ERROR;
		}
	}
	for (i=0; i<8; i++)
	{
		if (FAS_IO.get_in_bit(st_io_info.i_TsiteDoorChk[i], IO_ON) == IO_OFF)
		{
			m_strAlarmCode.Format(_T("81%04d"), st_io_info.i_TsiteDoorChk[i]);
//			CTL_Lib.Alarm_Error_Occurrence(6200+i, dWARNING, strTemp);
			st_alarm_info.strCode.Format(_T("81%04d"), st_io_info.i_TsiteDoorChk[i]);
			st_alarm_info.nCountMode	= 0;	
			st_alarm_info.nTypeMode		= eWARNING;
			st_alarm_info.nAlarmAssignSection = 2;

			return RET_ERROR;
		}
	}
	for (i=0; i<4; i++)
	{
		if (FAS_IO.get_in_bit(st_io_info.i_TsiteSmokeChk[i], IO_OFF) == IO_ON)
		{
			m_strAlarmCode.Format(_T("81%04d"), st_io_info.i_TsiteSmokeChk[i]);
//			CTL_Lib.Alarm_Error_Occurrence(6300+i, dWARNING, strTemp);

			st_alarm_info.strCode.Format(_T("81%04d"), st_io_info.i_TsiteSmokeChk[i]);
			st_alarm_info.nCountMode	= 0;	
			st_alarm_info.nTypeMode		= eWARNING;
			st_alarm_info.nAlarmAssignSection = 2;

			return RET_ERROR;
		}
	}
	return RET_GOOD;
}

int CPublicFunction::OnEmoCheck()
{
	if (FAS_IO.get_in_bit(st_io_info.i_Emo1SwChk, IO_OFF) == IO_ON)
	{
		CTL_Lib.Alarm_Error_Occurrence(7000, dRUN, _T("810112"));

		return RET_ERROR;
	}

	if (FAS_IO.get_in_bit(st_io_info.i_Emo2SwChk, IO_OFF) == IO_ON)
	{
		CTL_Lib.Alarm_Error_Occurrence(7001, dRUN, _T("810113"));

		return RET_ERROR;
	}

	if (FAS_IO.get_in_bit(st_io_info.i_Emo3SwChk, IO_OFF) == IO_ON)
	{
		CTL_Lib.Alarm_Error_Occurrence(7002, dRUN, _T("810114"));

		return RET_ERROR;
	}

	if (FAS_IO.get_in_bit(st_io_info.i_Emo4SwChk, IO_OFF) == IO_ON)
	{
		CTL_Lib.Alarm_Error_Occurrence(7003, dRUN, _T("810115"));

		return RET_ERROR;
	}

	return RET_GOOD;
}

void CPublicFunction::OnDoorClose()
{
	if (OnDoorOpenCheck() == RET_GOOD)
	{
		FAS_IO.set_out_bit(st_io_info.o_DoorLockLdUld, IO_ON);
		FAS_IO.set_out_bit(st_io_info.o_DoorLockTestSite, IO_ON);
		
		if (st_handler_info.cWndList != NULL)
		{
			//wsprintfA(st_other_info.cNormalMsg, "%S", _T("Door LOCK Close"));
			//clsFunc.OnStringToChar(_T("Door LOCK Close"), st_other_info.cNormalMsg);
			clsMem.OnNormalMessageWrite(_T("Door LOCK Close"));
		}
	}
	else 
	{
		st_handler_info.nRunStatus = dWARNING;
	}
}

// jtkim 20130625
void CPublicFunction::OnDoorOpen()
{
	if (st_handler_info.nRunStatus		== dSTOP &&
		st_handler_info.nMachineMode	== MACHINE_MANUAL)
	{
		FAS_IO.set_out_bit(st_io_info.o_DoorLockLdUld, IO_OFF);
		FAS_IO.set_out_bit(st_io_info.o_DoorLockTestSite, IO_OFF);
	
		if (st_handler_info.cWndList != NULL)
		{
			//wsprintfA(st_other_info.cNormalMsg, "%S", _T("DOOR LOCK Open"));
			//clsFunc.OnStringToChar(_T("DOOR LOCK Open"), st_other_info.cNormalMsg);
			clsMem.OnNormalMessageWrite(_T("DOOR LOCK Open"));
			//st_handler_info.cWndList->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
		}
	}
	else
	{
		if (st_handler_info.cWndList != NULL)
		{
			//wsprintfA(st_other_info.cNormalMsg, "%S", _T("[STOP] + [MANUAL] Status Action"));
			//clsFunc.OnStringToChar(_T("[STOP] + [MANUAL] Status Action"), st_other_info.cNormalMsg);
			clsMem.OnAbNormalMessagWrite(_T("[STOP] + [MANUAL] Status Action"));
			//st_handler_info.cWndList->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
		}
	}
}
//kwlee 2016.0826
void CPublicFunction::OnLdUldDoorClose()
{
	if (OnDoorOpenCheck() == RET_GOOD)
	{
		FAS_IO.set_out_bit(st_io_info.o_DoorLockLdUld, IO_ON);

		if (st_handler_info.cWndList != NULL)
		{
			clsMem.OnNormalMessageWrite(_T("LdUld Door LOCK Close"));

		}
	}
	else 
	{
		st_handler_info.nRunStatus = dWARNING;
	}
}


void CPublicFunction::OnLdUldDoorOpen()
{
	if (st_handler_info.nRunStatus		== dSTOP &&
		st_handler_info.nMachineMode	== MACHINE_MANUAL)
	{
		FAS_IO.set_out_bit(st_io_info.o_DoorLockLdUld, IO_OFF);


		if (st_handler_info.cWndList != NULL)
		{
			clsMem.OnNormalMessageWrite(_T("LdUld DOOR LOCK Open"));
		}
	}
	else
	{
		if (st_handler_info.cWndList != NULL)
		{
			clsMem.OnAbNormalMessagWrite(_T("[STOP] + [MANUAL] Status Action"));
		}
	}
}

void CPublicFunction::OnTestDoorClose()
{
	if (OnDoorOpenCheck() == RET_GOOD)
	{
		FAS_IO.set_out_bit(st_io_info.o_DoorLockTestSite, IO_ON);

		if (st_handler_info.cWndList != NULL)
		{
			clsMem.OnNormalMessageWrite(_T("Tester Door LOCK Close"));
		}
	}
	else 
	{
		st_handler_info.nRunStatus = dWARNING;
	}
}


void CPublicFunction::OnTestDoorOpen()
{
	if (st_handler_info.nRunStatus		== dSTOP &&
		st_handler_info.nMachineMode	== MACHINE_MANUAL)
	{
		FAS_IO.set_out_bit(st_io_info.o_DoorLockTestSite, IO_OFF);

		if (st_handler_info.cWndList != NULL)
		{
			clsMem.OnNormalMessageWrite(_T("Tester DOOR LOCK Open"));
		}
	}
	else
	{
		if (st_handler_info.cWndList != NULL)
		{
			clsMem.OnAbNormalMessagWrite(_T("[STOP] + [MANUAL] Status Action"));	
		}
	}
}

int CPublicFunction::OnEncoderCheck(double dPos)
{
	CString strMsg;
	
	int nRet = NO;
	
//	CDialog_Message	dlgMsg;
		
//	dlgMsg.DoModal();
//	str_msg.Format("Now Reading Value is %ld, Setting?", (long)d_pos);
//	st_other.str_confirm_msg = _T(str_msg);
//	n_response = select_dlg.DoModal();
//	if (n_response == IDOK) 
//	{
//		Ret = YES;
//	}
//	else
//	{
//		Ret = NO;
//	}
	
	return nRet;
}

bool CPublicFunction::OnCreateFolder(CString strPath)
{
	int nRet = 0;
	char chfilename[100];

    do
	{
        nRet = strPath.Find('\\' , nRet + 1);
        if (nRet > 0 )
        {
            CString strTemp = strPath.Left(nRet+1);
			
			//wsprintfA(chfilename, "%S", strTemp);
			clsFunc.OnStringToChar(strTemp, chfilename);
            if (_access(chfilename.operator LPCTSTR(), 00) == -1)
			{
				CreateDirectory(strTemp , NULL);
			}
        }
    }while (nRet != -1);
	
    return true;
}
//==================================================================//
// ���� ���¸� RUN ���·� ����
//==================================================================//
// jtkim 20130625
void CPublicFunction::OnMCRun() 
{
	// [LIGHT CURTAIN] ������ ���� �Ŀ� OFF�� ����� ������ �� �����Ƿ� �Ŀ� OFF�� ����� ON ��Ű���� �Ѵ�
	// - ������ �Ǵ� ����� �ִµ� ��ġ ������ ���õ� ����̴�
	// - �� ����� �ʱ�ȭ �� �Ǵ� ���ο� LOT ���� �� �ѹ��� �����ϱ� �����̴�
	// OnPowerOnAxisAll();  // ��ü ������ �Ŀ� ON

	//==============================================================//
	// �˶� ���� ���� ���� [ CIM ���� ] ���� �۾� ���� 
	//==============================================================//
	//--------------------------------------------------------------//

	//==============================================================//
	// ���μ��� ���� ���� [ CIM ���� ] ���� �۾� ���� 
	//==============================================================//
	//--------------------------------------------------------------//

	// ��ȣ ���� �ʱ�ȭ
	st_handler_info.nLevelTeach = FALSE;
	st_handler_info.nLevelMaint = FALSE;

	//Light Curtain �ʱ�ȭ
	st_sync_info.nLdUld_LightChk = NO;
	st_sync_info.nRej_LightChk	 = NO;

	// ��ư ���� ����
	FAS_IO.set_out_bit(st_io_info.o_StartLamp, IO_ON);   // S1500
	FAS_IO.set_out_bit(st_io_info.o_StopLamp,  IO_OFF);  // S1501
	FAS_IO.set_out_bit(st_io_info.o_AlarmLamp, IO_OFF);
	
	if (st_handler_info.cWndTitle != NULL)	
	{
		st_handler_info.cWndTitle->PostMessage(WM_STATUS_CHANGE, MACHINE_STATUS, st_handler_info.nRunStatus);
	}

	// 20140812 jtkim
	if (st_alarm_info.nAlarmClear == YES)
	{
		st_alarm_info.strCode		= _T("");
		st_alarm_info.nAlarmClear	= NO;
				
		clsXgem.OnMcAlarm(st_alarm_info.strCode, 0);

		//2017.0109
		clsLog.OnLogAlarm(st_alarm_info.strCode, 1);
		st_alarm_info.stl_prev_alarm_time = GetTickCount();
	}
	// ��� ���� �Լ����� RUN ������ ��쿡�� ��ġ�� �����ϰ� �Ǿ� �ִ�
	// �嵥, ���� ���� ���¸� �������ִ� ���� ���� �̰��� �߰��Ѵ�
	//2016.1203
	st_work_info.dwFrontTime[1] = st_work_info.dwRearTime[0][1] = st_work_info.dwRearTime[1][1] = GetTickCount();


	FAS_IO.set_out_bit( st_io_info.o_LdUldMuteOn, IO_OFF);
	FAS_IO.set_out_bit( st_io_info.o_LdUldMuteOff, IO_ON);

	FAS_IO.set_out_bit( st_io_info.o_RejMuteOn, IO_OFF);
	FAS_IO.set_out_bit( st_io_info.o_RejMuteOff, IO_ON);

	COMI.mn_run_status = dRUN;
}

//==================================================================//
// ���� STOP ���¿� ���� ��ư ���� ���� ����
//==================================================================//
// jtkim 20130625
void CPublicFunction::OnMCStop() 
{
	//==============================================================//
	// ���� ���� ����
	//==============================================================//
	// �Ŵ��� ��忡�� STOP ��ư ���� ���� ���� ���� ������ ����
	if (st_work_info.nStatusEqp != PROC_MAINT)
	{
		// ���簡 �ɸ��� ���� ���¿��� STOP Ű ���� ��쿡�� [PROC_MAINT]�� �����Ѵ�
		st_work_info.nStatusEqp = PROC_MAINT;
	}
	//--------------------------------------------------------------//
	// �÷��� �������� ��� ����Ͽ� �߰���
	st_handler_info.nMenuLock = NO;

	// ��ư ���� ����
	FAS_IO.set_out_bit(st_io_info.o_StartLamp, IO_OFF);  // S1500
	FAS_IO.set_out_bit(st_io_info.o_StopLamp,  IO_ON);   // S1501

	st_handler_info.nRunStatus = dSTOP;  // ���� ���� �÷��� ���� [STOP]
	if (st_handler_info.cWndTitle != NULL)	
	{
		st_handler_info.cWndTitle->PostMessage(WM_STATUS_CHANGE, MACHINE_STATUS, st_handler_info.nRunStatus);
	}
	// ��� ���� �Լ����� RUN ������ ��쿡�� ��ġ�� �����ϰ� �Ǿ� �ִ�
	// �嵥, ���� ���� ���¸� �������ִ� ���� ���� �̰��� �߰��Ѵ�
	
	// recovery data save
	OnRecoveryReadWrite(0, 0);

	// jtkim 20150818
	if (st_work_info.dwFrontTime[0] == 0)
	{
		st_work_info.dwFrontTime[0] = st_work_info.dwRearTime[0][0] = st_work_info.dwRearTime[1][0] = GetTickCount();
	}

	// jtkim 20160101
	if (st_work_info.nCycleYesNo == YES)
	{
		st_work_info.nCycleYesNo = NO;
	}

	// jtkim 20150721
	if (st_work_info.nTsiteCycleYesNo == YES)
	{
		st_work_info.nTsiteCycleYesNo = NO;
	}
	COMI.mn_run_status = dSTOP;
}

//==================================================================//
// ���� ���� ON/OFF
//==================================================================//
// jtkim 20130625
void CPublicFunction::OnSound(int nOnOff) 
{
	switch(nOnOff)
	{
		case IO_OFF:  // SOUND OFF
			FAS_IO.set_out_bit(st_io_info.o_Buzz1OnOff, IO_OFF);  //
			FAS_IO.set_out_bit(st_io_info.o_Buzz2OnOff, IO_OFF);  //
			FAS_IO.set_out_bit(st_io_info.o_Buzz3OnOff, IO_OFF);  //

			// ���� OFF ���� ���ش�
			FAS_IO.set_out_bit(st_io_info.o_BuzzLamp, IO_OFF);  // 
			FAS_IO.set_out_bit(st_io_info.o_AlarmLamp, IO_OFF);
			// ���� ���� OFF �Ǿ��ٰ� ����
			// : �ش� �÷��� ���� �� ���� �ð� �Ŀ� �ڵ����� OFF ��Ű�� ���� �÷��� �����̴�
			st_lamp_info.nStatusBuzzerRing = FALSE;
			break;

		case IO_ON:  // SOUND ON
			// ���� ��� ��忡���� ���� ����Ѵ�
			if (st_lamp_info.nBuzzerMode == YES)
			{
				// ������ �ϳ��� ����ϵ��� �Ѵ�
				FAS_IO.set_out_bit(st_io_info.o_Buzz1OnOff, IO_ON);
				FAS_IO.set_out_bit(st_io_info.o_Buzz2OnOff, IO_ON);
				FAS_IO.set_out_bit(st_io_info.o_Buzz3OnOff, IO_ON);  //

				// ���� OFF ���� ���ش�
				FAS_IO.set_out_bit(st_io_info.o_BuzzLamp, IO_ON);
				FAS_IO.set_out_bit(st_io_info.o_AlarmLamp, IO_ON);

				st_lamp_info.lBuzzerOffWait[0] = GetCurrentTime();
				st_lamp_info.nStatusBuzzerRing = TRUE;
			}
			break;
	}
}

// jtkim 20130625
void CPublicFunction::OnInitVariable()
{
	st_sync_info.nInitializeError		= NO;
}


void CPublicFunction::OnIOPortClear()
{
	if (FAS_IO.get_out_bit(st_io_info.o_MainAir, IO_ON) == IO_OFF)
	{
		FAS_IO.set_out_bit(st_io_info.o_MainAir, IO_ON);
	}
	FAS_IO.set_out_bit(st_io_info.o_ReadyLd1CvyStkStopUpDnCyl, IO_OFF);
	FAS_IO.set_out_bit(st_io_info.o_ReadyLd2CvyStkStopUpDnCyl, IO_OFF);
	FAS_IO.set_out_bit(st_io_info.o_ReadyUld1CvyStopCyl, IO_OFF);
	FAS_IO.set_out_bit(st_io_info.o_ReadyUld2CvyStopCyl, IO_OFF);
	FAS_IO.set_out_bit(st_io_info.o_Rej1UldCvyStopUpDnCyl, IO_OFF);
	FAS_IO.set_out_bit(st_io_info.o_Rej2UldCvyStopUpDnCyl, IO_OFF);

	FAS_IO.set_out_bit(st_io_info.o_ReadyLd1CvyAcMotOnOff, IO_OFF);
	FAS_IO.set_out_bit(st_io_info.o_ReadyLd2CvyAcMotOnOff, IO_OFF);
	FAS_IO.set_out_bit(st_io_info.o_LdStkAcMotOnOff, IO_OFF);
	FAS_IO.set_out_bit(st_io_info.o_Uld1StkAcMotOnOff, IO_OFF);
	FAS_IO.set_out_bit(st_io_info.o_Uld2StkAcMotOnOff, IO_OFF);
	FAS_IO.set_out_bit(st_io_info.o_RejEmptyCvyStkAcMotOnOff, IO_OFF);
	FAS_IO.set_out_bit(st_io_info.o_Rej1UldCvyStkAcMotOnOff, IO_OFF);
	FAS_IO.set_out_bit(st_io_info.o_Rej2UldCvyStkAcMotOnOff, IO_OFF);

	FAS_IO.set_out_bit( st_io_info.o_LdUldMuteOn, IO_OFF);
	FAS_IO.set_out_bit( st_io_info.o_LdUldMuteOff, IO_ON);

	FAS_IO.set_out_bit( st_io_info.o_RejMuteOn, IO_OFF);
	FAS_IO.set_out_bit( st_io_info.o_RejMuteOff, IO_ON);
}


void CPublicFunction::OnMotorSpeed()
{
	int i = 0, j = 0, nRet = 0;
	int nMotor[2]={0,};

	CString strMsg;
/*
	// loader magazine x
	nMotor[0] = M_LD_MGZ_X;
	// 1mm �̵��ϴµ� �ʿ��� pulse
	COMI.Set_MotUnitDist(nMotor[0], 250);
	// 10mm �̵��ϴµ� �ʿ��� pulse (�ӵ��� 1Cm/�ʷ� �����ϰ� �ȴ�.)
	COMI.Set_MotUnitSpeed(nMotor[0], 250);
	// motor ����
	COMI.m_nMotInfo[nMotor[0]] = MOT_SERVO;
	COMI.mn_homecheck_method[nMotor[0]]= 6;

	// loader magazine z
	nMotor[0] = M_LD_MGZ_Z;
	// 1mm �̵��ϴµ� �ʿ��� pulse
	COMI.Set_MotUnitDist(nMotor[0], 2000);
	// 10mm �̵��ϴµ� �ʿ��� pulse (�ӵ��� 1Cm/�ʷ� �����ϰ� �ȴ�.)
	COMI.Set_MotUnitSpeed(nMotor[0], 2000);
	// motor ����
	COMI.m_nMotInfo[nMotor[0]] = MOT_SERVO;
	COMI.mn_homecheck_method[nMotor[0]]= 1;

	// pcb shuttle x
	nMotor[0] = M_PCB_MOVE_X;
	// 1mm �̵��ϴµ� �ʿ��� pulse
	COMI.Set_MotUnitDist(nMotor[0], 500);
	// 10mm �̵��ϴµ� �ʿ��� pulse (�ӵ��� 1Cm/�ʷ� �����ϰ� �ȴ�.)
	COMI.Set_MotUnitSpeed(nMotor[0], 500);
	// motor ����
	COMI.m_nMotInfo[nMotor[0]] = MOT_SERVO;
	COMI.mn_homecheck_method[nMotor[0]]= 6;

	// pcb shuttle left y
	nMotor[0] = M_PCB_MOVE_LF_Y;
	// 1mm �̵��ϴµ� �ʿ��� pulse
	COMI.Set_MotUnitDist(nMotor[0], 500);
	// 10mm �̵��ϴµ� �ʿ��� pulse (�ӵ��� 1Cm/�ʷ� �����ϰ� �ȴ�.)
	COMI.Set_MotUnitSpeed(nMotor[0], 500);
	// motor ����
	COMI.m_nMotInfo[nMotor[0]] = MOT_STEP;
	st_io_info.oMotPwr[nMotor[0]] = st_io_info.oPcbLfCvyStepMotOnOff;
	COMI.mn_homecheck_method[nMotor[0]]= 6;

	// pcb shuttle right y
	nMotor[0] = M_PCB_MOVE_RI_Y;
	// 1mm �̵��ϴµ� �ʿ��� pulse
	COMI.Set_MotUnitDist(nMotor[0], 500);
	// 10mm �̵��ϴµ� �ʿ��� pulse (�ӵ��� 1Cm/�ʷ� �����ϰ� �ȴ�.)
	COMI.Set_MotUnitSpeed(nMotor[0], 500);
	// motor ����
	COMI.m_nMotInfo[nMotor[0]] = MOT_STEP;
	st_io_info.oMotPwr[nMotor[0]] = st_io_info.oPcbRiCvyStepMotOnOff;
	COMI.mn_homecheck_method[nMotor[0]]= 6;

	// pcb shuttle move
	nMotor[0] = M_PCB_MOVE_Y;
	// 1mm �̵��ϴµ� �ʿ��� pulse
	COMI.Set_MotUnitDist(nMotor[0], 59.974);
	// 10mm �̵��ϴµ� �ʿ��� pulse (�ӵ��� 1Cm/�ʷ� �����ϰ� �ȴ�.)
	COMI.Set_MotUnitSpeed(nMotor[0], 59.974);
	// motor ����
	COMI.m_nMotInfo[nMotor[0]] = MOT_STEP;
	st_io_info.oMotPwr[nMotor[0]] = st_io_info.oPcbHookStepMotOnOff;
	COMI.mn_homecheck_method[nMotor[0]]= 1;

	// pcb transfer y
	nMotor[0] = M_PCB_RBT_Y;
	// 1mm �̵��ϴµ� �ʿ��� pulse
	COMI.Set_MotUnitDist(nMotor[0], 500);
	// 10mm �̵��ϴµ� �ʿ��� pulse (�ӵ��� 1Cm/�ʷ� �����ϰ� �ȴ�.)
	COMI.Set_MotUnitSpeed(nMotor[0], 500);
	// motor ����
	COMI.m_nMotInfo[nMotor[0]] = MOT_SERVO;
	COMI.mn_homecheck_method[nMotor[0]]= 6;

	// pcb transfer z
	nMotor[0] = M_PCB_RBT_Z;
	// 1mm �̵��ϴµ� �ʿ��� pulse
	COMI.Set_MotUnitDist(nMotor[0], 2000);
	// 10mm �̵��ϴµ� �ʿ��� pulse (�ӵ��� 1Cm/�ʷ� �����ϰ� �ȴ�.)
	COMI.Set_MotUnitSpeed(nMotor[0], 2000);
	// motor ����
	COMI.m_nMotInfo[nMotor[0]] = MOT_SERVO;
	COMI.mn_homecheck_method[nMotor[0]]= 1;

	// pcb transfer rotator
	nMotor[0] = M_PCB_RBT_ROT;
	// 1mm �̵��ϴµ� �ʿ��� pulse
	COMI.Set_MotUnitDist(nMotor[0], 555.7964007);
	// 10mm �̵��ϴµ� �ʿ��� pulse (�ӵ��� 1Cm/�ʷ� �����ϰ� �ȴ�.)
	COMI.Set_MotUnitSpeed(nMotor[0], 555.7964007);
	// motor ����
	COMI.m_nMotInfo[nMotor[0]] = MOT_SERVO;
	COMI.mn_homecheck_method[nMotor[0]]= 1;

	// jig align y
	nMotor[0] = M_JIG_ALIGN_Y;
	// 1mm �̵��ϴµ� �ʿ��� pulse
	COMI.Set_MotUnitDist(nMotor[0], 5000);
	// 10mm �̵��ϴµ� �ʿ��� pulse (�ӵ��� 1Cm/�ʷ� �����ϰ� �ȴ�.)
	COMI.Set_MotUnitSpeed(nMotor[0], 5000);
	// motor ����
	COMI.m_nMotInfo[nMotor[0]] = MOT_SERVO;
	COMI.mn_homecheck_method[nMotor[0]]= 6;

	// jig transfer x
	nMotor[0] = M_JIG_RBT_X;
	// 1mm �̵��ϴµ� �ʿ��� pulse
	COMI.Set_MotUnitDist(nMotor[0], 500);
	// 10mm �̵��ϴµ� �ʿ��� pulse (�ӵ��� 1Cm/�ʷ� �����ϰ� �ȴ�.)
	COMI.Set_MotUnitSpeed(nMotor[0], 500);
	// motor ����
	COMI.m_nMotInfo[nMotor[0]] = MOT_SERVO;
	COMI.mn_homecheck_method[nMotor[0]]= 6;

	// jig transfer z
	nMotor[0] = M_JIG_RBT_Z;
	// 1mm �̵��ϴµ� �ʿ��� pulse
	COMI.Set_MotUnitDist(nMotor[0], 2000);
	// 10mm �̵��ϴµ� �ʿ��� pulse (�ӵ��� 1Cm/�ʷ� �����ϰ� �ȴ�.)e
	COMI.Set_MotUnitSpeed(nMotor[0], 2000);
	// motor ����
	COMI.m_nMotInfo[nMotor[0]] = MOT_SERVO;
	COMI.mn_homecheck_method[nMotor[0]]= 1;

	// jig stacker 
	nMotor[0] = M_JIG_STK;
	// 1mm �̵��ϴµ� �ʿ��� pulse
	COMI.Set_MotUnitDist(nMotor[0], 6000);
	// 10mm �̵��ϴµ� �ʿ��� pulse (�ӵ��� 1Cm/�ʷ� �����ϰ� �ȴ�.)
	COMI.Set_MotUnitSpeed(nMotor[0], 6000);
	// motor ����
	COMI.m_nMotInfo[nMotor[0]] = MOT_SERVO;
	COMI.mn_homecheck_method[nMotor[0]]= 6;

	COMI.mn_max_home_limit_time = 120000;
	COMI.mn_max_move_limit_time = 120000;

	for (i = 0; i < MAXMOTOR; i++)
	{
		COMI.md_limit_position[i][0] = st_motor_info[i].d_limit_position[0];
		COMI.md_limit_position[i][1] = st_motor_info[i].d_limit_position[1];

		COMI.md_spd_vel[i][0]			= st_motor_info[i].d_spd_vel[0];
		COMI.md_spd_vel[i][1]			= st_motor_info[i].d_spd_vel[1];
		COMI.md_spd_vel[i][2]			= st_motor_info[i].d_spd_vel[2];

		COMI.md_spd_home[i]				= st_motor_info[i].d_spd_home[0];
		COMI.md_spd_jog[i]				= st_motor_info[i].d_spd_jog[0];

		COMI.mn_allow_value[i]			= st_motor_info[i].n_allow; 

//		COMI.mn_homecheck_method[i]		= 6;	//el on-> stop-> back-> el off - > stop 
		nRet = COMI.Set_HomeSetConfig(i, COMI.mn_homecheck_method[i], 0, 2, 1);
		if (nRet != BD_GOOD)
		{
			if (st_handler_info.cWndList != NULL)
			{
				strMsg.Format(_T("Set_HomeSet_Config Error [MotNum[%d]]"), i);
				//wsprintfA(st_other_info.cAbnormalMsg, "%S", strMsg);
				//clsFunc.OnStringToChar(strMsg, st_other_info.cAbnormalMsg);
				clsMem.OnAbNormalMessagWrite(strMsg);
				st_handler_info.cWndList->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� �Ϸ� ��� ��û
			}
		}
	}

	//////////////////////////////////////////////////////
	//SD Sensor ���� ���� �׽�Ʈ 
	//////////////////////////////////////////////////////
	COMI.Set_Motor_IO_Property(M_JIG_STK, cmSD_MODE, cmTRUE);
	COMI.Set_Motor_IO_Property(M_JIG_STK, cmSD_EN, cmFALSE);    //cmSD_EN=14 //cmFALSE = 0 SD ��Ȱ��, cmTRUE = 1 SD Ȱ�� 	
	COMI.Set_Motor_IO_Property(M_JIG_STK, cmSD_LOGIC, cmLOGIC_B); //cmSD_LOGIC=15, 0 (cmLOGIC_A) : A���� ���,1 (cmLOGIC_B) : B���� ���
	COMI.Set_Motor_IO_Property(M_JIG_STK, cmSD_LATCH, cmFALSE);//16

	COMI.mn_runspeed_rate		= st_handler_info.nRunSpeed;
	COMI.mn_manualspeed_rate	= st_handler_info.nManualSpeed;*/
}


int CPublicFunction::OnMainAirCheck()
{
	if (FAS_IO.get_in_bit(st_io_info.i_MainAirChk, IO_OFF) == IO_OFF)
	{//802607 1 0 "MAIN_AIR_OFF_CHECK_ERROR."
		CTL_Lib.Alarm_Error_Occurrence(6319, dWARNING, _T("802607"));

		return RET_ERROR;
	}

	return RET_GOOD;
}


int CPublicFunction::OnMainPowerCheck()		//2014.01.21 �ּ�ó��
{
	if (FAS_IO.get_in_bit(st_io_info.i_MainMcChk, IO_OFF) == IO_ON)
	{
		st_alarm_info.strCode		= _T("801409"); 
		st_alarm_info.nCountMode	= 0;	
		st_alarm_info.nTypeMode		= eWARNING;
		st_alarm_info.nAlarmAssignSection = 87;

		return RET_ERROR;
	}

	return RET_GOOD;
}


int CPublicFunction::OnMotorPowerCheck()	//2014.01.21 �ּ�ó��
{
	if (FAS_IO.get_in_bit(st_io_info.i_MainMcChk, IO_ON) == IO_OFF)
	{
		CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, _T("801409"));

		return RET_ERROR;
	}
	return RET_GOOD;
}


void CPublicFunction::OnAxisEncoderReset(int nAxis)
{
	if(st_work_info.nSimulationMode == 1)
	{// 
		return;
	}

	cmmStSetPosition(nAxis, cmCNT_COMM, (double)0);
	cmmStSetPosition(nAxis, cmCNT_FEED, (double)0);
}


void CPublicFunction::OnThreadFunctionStepTrace(int nPos, int nStep)
{

	CString strString;

	if (nStep == st_other_info.nPrevStep[nPos])
	{
		return;
	}
	//2017.0202
	strString.Format(_T("[Pos:%02d] Prev : %d -> now %d"),nPos,st_other_info.nPrevStep[nPos],nStep);
	OnLogFileAdd(3, strString);

	st_other_info.nPrevStep[nPos]			= nStep;
	m_tThreadTime[nPos]						= CTime::GetCurrentTime();
	st_other_info.strLastChangeDate[nPos]	= m_tThreadTime[nPos].Format("%y/%m/%d %H:%M:%S");

	if (st_other_info.strHistoryStep[nPos].GetLength() == 114 ||
		st_other_info.strHistoryStep[nPos].GetLength() == 278)
	{
		m_strThreadStep[nPos].Format(_T("%05d|"), nStep);
		st_other_info.strHistoryStep[nPos] += m_strThreadStep[nPos];
		st_other_info.strHistoryStep[nPos] += "\r\n|                                        |";
	}
	else if (st_other_info.strHistoryStep[nPos].GetLength() == 442)
	{
		m_strThreadStep[nPos].Format(_T("%05d|"), nStep);
		st_other_info.strHistoryStep[nPos] += m_strThreadStep[nPos];
	}
	else if (st_other_info.strHistoryStep[nPos].GetLength() >= 448)
	{
		m_strThreadStep[nPos].Format(_T("%05d,"), nStep);
		st_other_info.strHistoryStep[nPos] = m_strThreadStep[nPos];
	}
	else
	{
		m_strThreadStep[nPos].Format(_T("%05d,"), nStep);
		st_other_info.strHistoryStep[nPos] += m_strThreadStep[nPos];
	}
}


void CPublicFunction::OnLotStartClear()
{

}


CString	CPublicFunction::OnCharToString(char chData[], int nLength)
{
	CString strTmp;
	BSTR	buf;
	int		nCnt;

	nCnt	= MultiByteToWideChar(CP_ACP, 0, chData, nLength, NULL, NULL);
	buf		= SysAllocStringLen(NULL, nCnt);
	MultiByteToWideChar(CP_ACP, 0, chData, nLength, buf, nCnt);
	strTmp.Format(_T("%s"), buf);

	SysFreeString(buf);

	return strTmp;
}


void CPublicFunction::OnStringToChar(CString strData, char chData[])
{
	int nCnt;
	wchar_t *chBuf;

	chBuf = strData.GetBuffer(strData.GetLength());
	nCnt  = WideCharToMultiByte(CP_ACP, 0, chBuf, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, chBuf, -1, chData, nCnt, 0, 0);
}


//2015.02.10
CString CPublicFunction::ThreadID(int nID)
{
	CString strTmp = _T("");

	switch(nID+1)
	{
		case  1:
			strTmp.Format(_T("BarcodeRobot Run"));
			break;

		case 2:
			strTmp.Format(_T("BarcodeRobot BCRReading"));
			break;

		case  3:
			strTmp.Format(_T("CokRobot Run"));
			break;

		case  4:
			strTmp.Format(_T("CokRobot Move"));
			break;

		case  5:
			strTmp.Format(_T("EmptyStacker Run"));
			break;

		case  6:
			strTmp.Format(_T("EmptyStacker Move"));
			break;

		case  7:
			strTmp.Format(_T("LoadRobot Run"));
			break;

		case  8:
			strTmp.Format(_T("LoadRobot Buffer Place"));
			break;

		case  9:
			strTmp.Format(_T("LoadRobot TrayPick"));
			break;

		case  10:
			strTmp.Format(_T("LoadRotatorBuffer Run"));
			break;

		case  11:
			strTmp.Format(_T("LoadTrayConveyor Run"));
			break;

		case  12:
			strTmp.Format(_T("LoadTrayConveyor LdTrayBCR"));
			break;

		case  13:
			strTmp.Format(_T("LoadTrayConveyor LdTrayInput"));
			break;

		case  14:
			strTmp.Format(_T("LoadTrayConveyor Smema"));
			break;

		case  15:
			strTmp.Format(_T("LoadTrayPlate LoadTray_1"));
			break;

		case  16:
			strTmp.Format(_T("LoadTrayPlate LoadTray_2"));
			break;

		case  17:
			strTmp.Format(_T("LoadTrayPlate LoadTray_2_EmtyTray"));
			break;

		case  18:
			strTmp.Format(_T("LoadTrayShifter Run"));
			break;

		case  19:
			strTmp.Format(_T("LoadTrayShifter TrayPick"));
			break;

		case  20:
			strTmp.Format(_T("LoadTrayShifter TrayPlace"));
			break;

		case  21:
			strTmp.Format(_T("LeftMp Run"));
			break;

		case  22:
			strTmp.Format(_T("RejectTray Run"));
			break;

		case  23:
			strTmp.Format(_T("RejectTray Pick"));
			break;

		case  24:
			strTmp.Format(_T("RejectTray Place"));
			break;

		case  25:
			strTmp.Format(_T("RejectTray TrayUp"));
			break;

		case  26:
			strTmp.Format(_T("RejectTray TrayDn"));
			break;

		case  27:
			strTmp.Format(_T("RetestTray Run"));
			break;

		case  28:
			strTmp.Format(_T("RightMp Run"));
			break;

		case  29:
			strTmp.Format(_T("TestSiteRobot Run"));
			break;

		case  30:
			strTmp.Format(_T("TestSiteRobot BufferPick"));
			break;

		case  31:
			strTmp.Format(_T("TestSiteRobot BufferPlace"));
			break;

		case  32:
			strTmp.Format(_T("TestSiteRobot Cok Buff Pick"));
			break;

		case  33:
			strTmp.Format(_T("TestSiteRobot Cok Buff Place"));
			break;

		case  34:
			strTmp.Format(_T("TestSiteRobot CokExchange"));
			break;

		case  35:
			strTmp.Format(_T("TestSiteRobot Cok_TestPick"));
			break;

		case  36:
			strTmp.Format(_T("TestSiteRobot Cok_TestPlace"));
			break;

		case  37:
			strTmp.Format(_T("TestSiteRobot Test Pick"));
			break;

		case  38:
			strTmp.Format(_T("TestSiteRobot Test Place"));
			break;

		case  39:
			strTmp.Format(_T("UldRobot Run"));
			break;

		case  40:
			strTmp.Format(_T("UldRobot Buff Pick"));
			break;

		case  41:
			strTmp.Format(_T("UldRobot TrayPlace"));
			break;

		case  42:
			strTmp.Format(_T("UldRotatorBuffer Run"));
			break;

		case  43:
			strTmp.Format(_T("UldTrayConveyor Run"));
			break;

		case  44:
			strTmp.Format(_T("UldTrayConveyor Smema"));
			break;

		case  45:
			strTmp.Format(_T("UldTrayShifter Run"));
			break;

		case  46:
			strTmp.Format(_T("UldTrayShifter TrayPick"));
			break;

		case  47:
			strTmp.Format(_T("UldTrayShifter TrayPlace"));
			break;

		case  48:
			strTmp.Format(_T("UldTrayStacker Run"));
			break;

		case  49:
			strTmp.Format(_T("UldTrayStacker Move"));
			break;
	}

	return strTmp;
}

BOOL CPublicFunction::IsNum(CString strData)
{
	int nCnt = strData.GetLength();
	int i;
	char cTemp;

	for (i=0; i<nCnt; i++)
	{
		cTemp = (char)strData.GetAt(i);

		if(i==0 && cTemp == '-') continue; 

		if(cTemp >= '0' && cTemp <= '9') continue; 
		else break; 
	}
	if (i==nCnt) return TRUE;
	else return FALSE;
}

int	CPublicFunction::OnIoSafetyCheck(int nIO)
{
//	int nRet[5];

// 	switch (nIO)
// 	{
		//st_io_info.o_LfTsiteBtmSockIns1Cyl				= 2101;		// S2101
// 		case 2101:
// 		    if (st_handler_info.nPgmVer == 1) return RET_GOOD;
// 
// 			nRet[0] = FAS_IO.get_in_bit(st_io_info.i_LfTSiteBtmPogoChk[0], IO_OFF);
// 			nRet[1] = FAS_IO.get_in_bit(st_io_info.i_LfTSiteBtmPogoChk[1], IO_OFF);
// 
// 			if (nRet[0] == IO_OFF || nRet[1] == IO_OFF)
// 			{
// 				return RET_ERROR;
// 			}
// 			break;
// 
// 		//st_io_info.o_LfTsiteBtmSockIns2Cyl				= 2102;		// S2102
// 		case 2102:
// 			if (st_handler_info.nPgmVer == 1) return RET_GOOD;
// 
// 			nRet[0] = FAS_IO.get_in_bit(st_io_info.i_LfTSiteBtmPogoChk[2], IO_OFF);
// 			nRet[1] = FAS_IO.get_in_bit(st_io_info.i_LfTSiteBtmPogoChk[3], IO_OFF);
// 
// 			if (nRet[0] == IO_OFF || nRet[1] == IO_OFF)
// 			{
// 				return RET_ERROR;
// 			}
// 			break;
// 
// 		//st_io_info.o_LfTsiteBtmSockIns3Cyl				= 2103;		// S2103
// 		case 2103:
// 			if (st_handler_info.nPgmVer == 1) return RET_GOOD;
// 
// 			nRet[0] = FAS_IO.get_in_bit(st_io_info.i_LfTSiteBtmPogoChk[4], IO_OFF);
// 			nRet[1] = FAS_IO.get_in_bit(st_io_info.i_LfTSiteBtmPogoChk[5], IO_OFF);
// 
// 			if (nRet[0] == IO_OFF || nRet[1] == IO_OFF)
// 			{
// 				return RET_ERROR;
// 			}
// 			break;
// 	
// 		//st_io_info.o_LfTsiteBtmSockIns4Cyl				= 2104;		// S2104
// 		case 2104:
// 			if (st_handler_info.nPgmVer == 1) return RET_GOOD;
// 
// 			nRet[0] = FAS_IO.get_in_bit(st_io_info.i_LfTSiteBtmPogoChk[6], IO_OFF);
// 			nRet[1] = FAS_IO.get_in_bit(st_io_info.i_LfTSiteBtmPogoChk[7], IO_OFF);
// 
// 			if (nRet[0] == IO_OFF || nRet[1] == IO_OFF)
// 			{
// 				return RET_ERROR;
// 			}
// 			break;
// 		
// 		//st_io_info.o_LfTsiteBtmSockIns5Cyl				= 2105;		// S2105
// 		case 2105:
// 			if (st_handler_info.nPgmVer == 1) return RET_GOOD;
// 
// 			nRet[0] = FAS_IO.get_in_bit(st_io_info.i_LfTSiteBtmPogoChk[8], IO_OFF);
// 			nRet[1] = FAS_IO.get_in_bit(st_io_info.i_LfTSiteBtmPogoChk[9], IO_OFF);
// 
// 			if (nRet[0] == IO_OFF || nRet[1] == IO_OFF)
// 			{
// 				return RET_ERROR;
// 			}
// 			break;
// 
// 		//st_io_info.o_LfTsiteBtmSockIns6Cyl				= 2106;		// S2106
// 		case 2106:
// 			if (st_handler_info.nPgmVer == 1) return RET_GOOD;
// 
// 			nRet[0] = FAS_IO.get_in_bit(st_io_info.i_LfTSiteBtmPogoChk[10], IO_OFF);
// 			nRet[1] = FAS_IO.get_in_bit(st_io_info.i_LfTSiteBtmPogoChk[11], IO_OFF);
// 
// 			if (nRet[0] == IO_OFF || nRet[1] == IO_OFF)
// 			{
// 				return RET_ERROR;
// 			}
// 			break;
// 
// 		//st_io_info.o_LfTsiteTopSockIns1Cyl				= 2501;		// S2501
// 		case 2501:
// 			if (st_handler_info.nPgmVer == 1) return RET_GOOD;
// 
// 			nRet[0] = FAS_IO.get_in_bit(st_io_info.i_LfTSiteTopPogoChk[0], IO_OFF);
// 			nRet[1] = FAS_IO.get_in_bit(st_io_info.i_LfTSiteTopPogoChk[1], IO_OFF);
// 
// 			if (nRet[0] == IO_OFF || nRet[1] == IO_OFF)
// 			{
// 				return RET_ERROR;
// 			}
// 			break;
// 
// 		//st_io_info.o_LfTsiteTopSockIns2Cyl				= 2502;		// S2502
// 		case 2502:
// 			if (st_handler_info.nPgmVer == 1) return RET_GOOD;
// 
// 			nRet[0] = FAS_IO.get_in_bit(st_io_info.i_LfTSiteTopPogoChk[2], IO_OFF);
// 			nRet[1] = FAS_IO.get_in_bit(st_io_info.i_LfTSiteTopPogoChk[3], IO_OFF);
// 
// 			if (nRet[0] == IO_OFF || nRet[1] == IO_OFF)
// 			{
// 				return RET_ERROR;
// 			}
// 			break;
// 
// 		//st_io_info.o_LfTsiteTopSockIns3Cyl				= 2503;		// S2503
// 		case 2503:
// 			if (st_handler_info.nPgmVer == 1) return RET_GOOD;
// 
// 			nRet[0] = FAS_IO.get_in_bit(st_io_info.i_LfTSiteTopPogoChk[4], IO_OFF);
// 			nRet[1] = FAS_IO.get_in_bit(st_io_info.i_LfTSiteTopPogoChk[5], IO_OFF);
// 
// 			if (nRet[0] == IO_OFF || nRet[1] == IO_OFF)
// 			{
// 				return RET_ERROR;
// 			}
// 			break;
// 
// 		//st_io_info.o_LfTsiteTopSockIns4Cyl				= 2504;		// S2504
// 		case 2504:
// 			if (st_handler_info.nPgmVer == 1) return RET_GOOD;
// 
// 			nRet[0] = FAS_IO.get_in_bit(st_io_info.i_LfTSiteTopPogoChk[6], IO_OFF);
// 			nRet[1] = FAS_IO.get_in_bit(st_io_info.i_LfTSiteTopPogoChk[7], IO_OFF);
// 
// 			if (nRet[0] == IO_OFF || nRet[1] == IO_OFF)
// 			{
// 				return RET_ERROR;
// 			}
// 			break;
// 
// 		//st_io_info.o_LfTsiteTopSockIns5Cyl				= 2505;		// S2505
// 		case 2505:
// 			if (st_handler_info.nPgmVer == 1) return RET_GOOD;
// 
// 			nRet[0] = FAS_IO.get_in_bit(st_io_info.i_LfTSiteTopPogoChk[8], IO_OFF);
// 			nRet[1] = FAS_IO.get_in_bit(st_io_info.i_LfTSiteTopPogoChk[9], IO_OFF);
// 
// 			if (nRet[0] == IO_OFF || nRet[1] == IO_OFF)
// 			{
// 				return RET_ERROR;
// 			}
// 			break;
// 
// 		//st_io_info.o_LfTsiteTopSockIns6Cyl				= 2506;		// S2506
// 		case 2506:
// 			if (st_handler_info.nPgmVer == 1) return RET_GOOD;
// 
// 			nRet[0] = FAS_IO.get_in_bit(st_io_info.i_LfTSiteTopPogoChk[10], IO_OFF);
// 			nRet[1] = FAS_IO.get_in_bit(st_io_info.i_LfTSiteTopPogoChk[11], IO_OFF);
// 
// 			if (nRet[0] == IO_OFF || nRet[1] == IO_OFF)
// 			{
// 				return RET_ERROR;
// 			}
// 			break;
// 
// 		//st_io_info.o_RiTsiteBtmSockIns1Cyl				= 3001;		// S3001
// 		case 3001:
// 			if (st_handler_info.nPgmVer == 1) return RET_GOOD;
// 
// 			nRet[0] = FAS_IO.get_in_bit(st_io_info.i_RiTSiteBtmPogoChk[0], IO_OFF);
// 			nRet[1] = FAS_IO.get_in_bit(st_io_info.i_RiTSiteBtmPogoChk[1], IO_OFF);
// 
// 			if (nRet[0] == IO_OFF || nRet[1] == IO_OFF)
// 			{
// 				return RET_ERROR;
// 			}
// 			break;
// 
// 		//st_io_info.o_RiTsiteBtmSockIns2Cyl				= 3002;		// S3002
// 		case 3002:
// 			if (st_handler_info.nPgmVer == 1) return RET_GOOD;
// 
// 			nRet[0] = FAS_IO.get_in_bit(st_io_info.i_RiTSiteBtmPogoChk[2], IO_OFF);
// 			nRet[1] = FAS_IO.get_in_bit(st_io_info.i_RiTSiteBtmPogoChk[3], IO_OFF);
// 
// 			if (nRet[0] == IO_OFF || nRet[1] == IO_OFF)
// 			{
// 				return RET_ERROR;
// 			}
// 			break;
// 
// 		//st_io_info.o_RiTsiteBtmSockIns3Cyl				= 3003;		// S3003
// 		case 3003:
// 			if (st_handler_info.nPgmVer == 1) return RET_GOOD;
// 
// 			nRet[0] = FAS_IO.get_in_bit(st_io_info.i_RiTSiteBtmPogoChk[4], IO_OFF);
// 			nRet[1] = FAS_IO.get_in_bit(st_io_info.i_RiTSiteBtmPogoChk[5], IO_OFF);
// 
// 			if (nRet[0] == IO_OFF || nRet[1] == IO_OFF)
// 			{
// 				return RET_ERROR;
// 			}
// 			break;
// 
// 		//st_io_info.o_RiTsiteBtmSockIns4Cyl				= 3004;		// S3004
// 		case 3004:
// 			if (st_handler_info.nPgmVer == 1) return RET_GOOD;
// 
// 			nRet[0] = FAS_IO.get_in_bit(st_io_info.i_RiTSiteBtmPogoChk[6], IO_OFF);
// 			nRet[1] = FAS_IO.get_in_bit(st_io_info.i_RiTSiteBtmPogoChk[7], IO_OFF);
// 
// 			if (nRet[0] == IO_OFF || nRet[1] == IO_OFF)
// 			{
// 				return RET_ERROR;
// 			}
// 			break;
// 
// 		//st_io_info.o_RiTsiteBtmSockIns5Cyl				= 3005;		// S3005
// 		case 3005:
// 			if (st_handler_info.nPgmVer == 1) return RET_GOOD;
// 
// 			nRet[0] = FAS_IO.get_in_bit(st_io_info.i_RiTSiteBtmPogoChk[8], IO_OFF);
// 			nRet[1] = FAS_IO.get_in_bit(st_io_info.i_RiTSiteBtmPogoChk[9], IO_OFF);
// 
// 			if (nRet[0] == IO_OFF || nRet[1] == IO_OFF)
// 			{
// 				return RET_ERROR;
// 			}
// 			break;
// 
// 		//st_io_info.o_RiTsiteBtmSockIns6Cyl				= 3006;		// S3006
// 		case 3006:
// 			if (st_handler_info.nPgmVer == 1) return RET_GOOD;
// 
// 			nRet[0] = FAS_IO.get_in_bit(st_io_info.i_RiTSiteBtmPogoChk[10], IO_OFF);
// 			nRet[1] = FAS_IO.get_in_bit(st_io_info.i_RiTSiteBtmPogoChk[11], IO_OFF);
// 
// 			if (nRet[0] == IO_OFF || nRet[1] == IO_OFF)
// 			{
// 				return RET_ERROR;
// 			}
// 			break;
// 
// 		//st_io_info.o_RiTsiteTopSockIns1Cyl				= 3401;		// S3401
// 		case 3401:
// 			if (st_handler_info.nPgmVer == 1) return RET_GOOD;
// 
// 			nRet[0] = FAS_IO.get_in_bit(st_io_info.i_RiTSiteTopPogoChk[0], IO_OFF);
// 			nRet[1] = FAS_IO.get_in_bit(st_io_info.i_RiTSiteTopPogoChk[1], IO_OFF);
// 
// 			if (nRet[0] == IO_OFF || nRet[1] == IO_OFF)
// 			{
// 				return RET_ERROR;
// 			}
// 			break;
// 
// 		//st_io_info.o_RiTsiteTopSockIns2Cyl				= 3402;		// S3402
// 		case 3402:
// 			if (st_handler_info.nPgmVer == 1) return RET_GOOD;
// 
// 			nRet[0] = FAS_IO.get_in_bit(st_io_info.i_RiTSiteTopPogoChk[2], IO_OFF);
// 			nRet[1] = FAS_IO.get_in_bit(st_io_info.i_RiTSiteTopPogoChk[3], IO_OFF);
// 
// 			if (nRet[0] == IO_OFF || nRet[1] == IO_OFF)
// 			{
// 				return RET_ERROR;
// 			}
// 			break;
// 
// 		//st_io_info.o_RiTsiteTopSockIns3Cyl				= 3403;		// S3403
// 		case 3403:
// 			if (st_handler_info.nPgmVer == 1) return RET_GOOD;
// 
// 			nRet[0] = FAS_IO.get_in_bit(st_io_info.i_RiTSiteTopPogoChk[4], IO_OFF);
// 			nRet[1] = FAS_IO.get_in_bit(st_io_info.i_RiTSiteTopPogoChk[5], IO_OFF);
// 
// 			if (nRet[0] == IO_OFF || nRet[1] == IO_OFF)
// 			{
// 				return RET_ERROR;
// 			}
// 			break;
// 
// 		//st_io_info.o_RiTsiteTopSockIns4Cyl				= 3404;		// S3404
// 		case 3404:
// 			if (st_handler_info.nPgmVer == 1) return RET_GOOD;
// 
// 			nRet[0] = FAS_IO.get_in_bit(st_io_info.i_RiTSiteTopPogoChk[6], IO_OFF);
// 			nRet[1] = FAS_IO.get_in_bit(st_io_info.i_RiTSiteTopPogoChk[7], IO_OFF);
// 
// 			if (nRet[0] == IO_OFF || nRet[1] == IO_OFF)
// 			{
// 				return RET_ERROR;
// 			}
// 			break;
// 
// 		//st_io_info.o_RiTsiteTopSockIns5Cyl				= 3405;		// S3405
// 		case 3405:
// 			if (st_handler_info.nPgmVer == 1) return RET_GOOD;
// 
// 			nRet[0] = FAS_IO.get_in_bit(st_io_info.i_RiTSiteTopPogoChk[8], IO_OFF);
// 			nRet[1] = FAS_IO.get_in_bit(st_io_info.i_RiTSiteTopPogoChk[9], IO_OFF);
// 
// 			if (nRet[0] == IO_OFF || nRet[1] == IO_OFF)
// 			{
// 				return RET_ERROR;
// 			}
// 			break;
// 
// 		//st_io_info.o_RiTsiteTopSockIns6Cyl				= 3406;		// S3406
// 		case 3406:
// 			if (st_handler_info.nPgmVer == 1) return RET_GOOD;
// 
// 			nRet[0] = FAS_IO.get_in_bit(st_io_info.i_RiTSiteTopPogoChk[10], IO_OFF);
// 			nRet[1] = FAS_IO.get_in_bit(st_io_info.i_RiTSiteTopPogoChk[11], IO_OFF);
// 
// 			if (nRet[0] == IO_OFF || nRet[1] == IO_OFF)
// 			{
// 				return RET_ERROR;
// 			}
// 			break;
// 	}

	return RET_GOOD;
}

int CPublicFunction::OnRecoveryReadWrite(int nMode, int nReadWrite)
{
	CFile file;

	CString strFileName;
	CTime time;

	int nArchive;

	return RET_GOOD; //james 2016.0906 

	if (nMode == 0)
	{
		strFileName = st_path_info.strPathRecovery + _T("Handler.DAT");
	}
	else 
	{
		time = CTime::GetCurrentTime();
		strFileName.Format(_T("%sHandler_%04d%02d%02d%02d%02d.DAT"), st_path_info.strPathRecovery,
																	 time.GetYear(),
																	 time.GetMonth(),
																	 time.GetDay(),
																	 time.GetHour(),
																	 time.GetMinute());
	}

	if (nReadWrite == 0)
	{
		if (!file.Open(strFileName, CFile::modeWrite | CFile::modeCreate))
		{
			return RET_ERROR;
		}
		nArchive = CArchive::store;
	}
	else
	{
		if (!file.Open(strFileName, CFile::modeRead))
		{
			return RET_ERROR;
		}
		nArchive = CArchive::load;
	}

	CArchive ar(&file, nArchive);

	OnSerialize(ar);

	ar.Close();
	file.Close();
	return RET_GOOD;
}

void CPublicFunction::OnLogBarcode(CString strMsg)
{
	CString strFileName;				// ���������� ������ ���� �̸� ���� ���� 
	CString strCreateFile;				// �˶� ���� ������ ���Ͽ� ���� [����]+[���ϸ�]+[Ȯ����] ���� ���� 
	CString strListName, strTempData;	// �� �κк� �˶� �߻� Ƚ�� ���� ������ �α� ���� ���� ���� ���� 
	CString strContent;				// �� ���ο� ����� ���ڿ� ���� ���� ���� 
	int nExistence;						// ���� ���� ���� ���� �÷��� 
	char chFileName[256];				// �˻��� ���� ���� ���� �Լ� 
	char chMsg[1000];
	FILE  *fp ;							// ���Ͽ� ���� ������ ���� ���� 
	
	CString strTitleTime, strFileTime, strNewFile;		// ���Ͽ� ����� Ÿ��Ʋ �ð� ���� ����
	int nCurYear, nCurMonth, nCurDay;					// ��, ��, �� ���� ����
	int nCurHour, nCurMinute, nCurSecond;				// ��, ��, �� ���� ����
	int nMtbi = 0;

	//double dAve;
	//double dTemp;
	//
	
	COleDateTime otCurr;									// ���� �ð� ���� ����
	CTime tCurr;										// Ÿ��Ʋ �ð� ���� ����

	CString strTemp;

	CString strData, strTime;
	
	/* ************************************************************************** */
	/* ���� �̸����� ����� ��¥ �����Ѵ�                                         */
	/* ************************************************************************** */
	otCurr = COleDateTime::GetCurrentTime();

	nCurYear		= otCurr.GetYear();  
	nCurMonth		= otCurr.GetMonth();  
	nCurDay			= otCurr.GetDay();  

	nCurHour		= otCurr.GetHour();
	nCurMinute		= otCurr.GetMinute();
	nCurSecond		= otCurr.GetSecond();

	strNewFile.Format(_T("BARCODE_DATA%04d%02d%02d"), nCurYear, nCurMonth, nCurDay);
	strTime.Format(_T("%04d/%02d/%02d %02d:%02d:%02d"), nCurYear, nCurMonth, nCurDay, nCurHour, nCurMinute, nCurSecond);

	strCreateFile = st_path_info.strBarcode + strNewFile;
	strCreateFile += ".TXT";

	OnStringToChar(strCreateFile, chFileName);
	nExistence = _access(chFileName, 0);

	if (nExistence == -1)  /* ���� ������ */
	{
		strCreateFile = st_path_info.strBarcode + strNewFile;
		strCreateFile += ".TXT";
		OnStringToChar(strCreateFile, chFileName);
	}

	/* ************************************************************************** */
	/* �˶� �߻� Ƚ�� ���� ���� ���Ͽ� �߰� ������ ���� ���Ϸ� ����               */
	/* ************************************************************************** */
	fopen_s(&fp, chFileName, "a+");
	if(!fp)
	{
		//		AfxMessageBox(_T("The failure because we open the file."));
		return;
	}
	/* ************************************************************************** */
	strContent += strTime;
	strContent += _T(" : ");
	strContent += strMsg;
	clsFunc.OnStringToChar(strContent, chMsg);
	fprintf(fp,"%s\r\n", chMsg) ;

	if (ferror(fp))  
	{
		//		Func.MsgLog("���� ���� ����!..") ;
		fclose(fp); 

		return ;
	}

	fclose(fp); 
}


void CPublicFunction::OnLogBarcodeNg(CString strMsg)
{
	CString strFileName;				// ���������� ������ ���� �̸� ���� ���� 
	CString strCreateFile;				// �˶� ���� ������ ���Ͽ� ���� [����]+[���ϸ�]+[Ȯ����] ���� ���� 
	CString strListName, strTempData;	// �� �κк� �˶� �߻� Ƚ�� ���� ������ �α� ���� ���� ���� ���� 
	CString strContent;				// �� ���ο� ����� ���ڿ� ���� ���� ���� 
	int nExistence;						// ���� ���� ���� ���� �÷��� 
	char chFileName[256];				// �˻��� ���� ���� ���� �Լ� 
	char chMsg[1000];
	FILE  *fp ;							// ���Ͽ� ���� ������ ���� ���� 

	CString strTitleTime, strFileTime, strNewFile;		// ���Ͽ� ����� Ÿ��Ʋ �ð� ���� ����
	int nCurYear, nCurMonth, nCurDay;					// ��, ��, �� ���� ����
	int nCurHour, nCurMinute, nCurSecond;				// ��, ��, �� ���� ����
	int nMtbi = 0;

	//double dAve;
	//double dTemp;
	//

	COleDateTime otCurr;									// ���� �ð� ���� ����
	CTime tCurr;										// Ÿ��Ʋ �ð� ���� ����

	CString strTemp;

	CString strData, strTime;

	/* ************************************************************************** */
	/* ���� �̸����� ����� ��¥ �����Ѵ�                                         */
	/* ************************************************************************** */
	otCurr = COleDateTime::GetCurrentTime();

	nCurYear		= otCurr.GetYear();  
	nCurMonth		= otCurr.GetMonth();  
	nCurDay			= otCurr.GetDay();  

	nCurHour		= otCurr.GetHour();
	nCurMinute		= otCurr.GetMinute();
	nCurSecond		= otCurr.GetSecond();

	strNewFile.Format(_T("BARCODE_NG_DATA%04d%02d%02d"), nCurYear, nCurMonth, nCurDay);
	strTime.Format(_T("%04d/%02d/%02d %02d:%02d:%02d"), nCurYear, nCurMonth, nCurDay, nCurHour, nCurMinute, nCurSecond);

	strCreateFile = st_path_info.strBarcodeNg + strNewFile;
	strCreateFile += ".TXT";

	OnStringToChar(strCreateFile, chFileName);
	nExistence = _access(chFileName, 0);

	if (nExistence == -1)  /* ���� ������ */
	{
		strCreateFile = st_path_info.strBarcodeNg + strNewFile;
		strCreateFile += ".TXT";
		OnStringToChar(strCreateFile, chFileName);
	}

	/* ************************************************************************** */
	/* �˶� �߻� Ƚ�� ���� ���� ���Ͽ� �߰� ������ ���� ���Ϸ� ����               */
	/* ************************************************************************** */
	fopen_s(&fp, chFileName, "a+");
	if(!fp)
	{
		//		AfxMessageBox(_T("The failure because we open the file."));
		return;
	}
	/* ************************************************************************** */
	strContent += strTime;
	strContent += _T(" : ");
	strContent += strMsg;
	clsFunc.OnStringToChar(strContent, chMsg);
	fprintf(fp,"%s\r\n", chMsg) ;

	if (ferror(fp))  
	{
		//		Func.MsgLog("���� ���� ����!..") ;
		fclose(fp); 

		return ;
	}

	fclose(fp); 
}

void CPublicFunction::OnCycleTime(int nMode, CString strLotNo, CString strPartNo, DWORD dwTime1, DWORD dwTime2, DWORD dwTime3)
{
	CString strFileName;				// ���������� ������ ���� �̸� ���� ���� 
	CString strCreateFile;				// �˶� ���� ������ ���Ͽ� ���� [����]+[���ϸ�]+[Ȯ����] ���� ���� 
	CString strListName, strTempData;	// �� �κк� �˶� �߻� Ƚ�� ���� ������ �α� ���� ���� ���� ���� 
	CString strContent;				// �� ���ο� ����� ���ڿ� ���� ���� ���� 
	int nExistence;						// ���� ���� ���� ���� �÷��� 
	char chFileName[256];				// �˻��� ���� ���� ���� �Լ� 
	char chMsg[1000];
	FILE  *fp ;							// ���Ͽ� ���� ������ ���� ���� 

	CString strTitleTime, strFileTime;		// ���Ͽ� ����� Ÿ��Ʋ �ð� ���� ����
	int nCurYear, nCurMonth, nCurDay;					// ��, ��, �� ���� ����
	int nCurHour, nCurMinute, nCurSecond;				// ��, ��, �� ���� ����
	int nMtbi = 0;

//	double dAve;

	COleDateTime otCurr;									// ���� �ð� ���� ����
	COleDateTime tNext;
	CTime tCurr;										// Ÿ��Ʋ �ð� ���� ����

	CString strData, strTime;

	/* ************************************************************************** */
	/* ���� �̸����� ����� ��¥ �����Ѵ�                                         */
	/* ************************************************************************** */
	otCurr = COleDateTime::GetCurrentTime();

	nCurYear		= otCurr.GetYear();  
	nCurMonth		= otCurr.GetMonth();  
	nCurDay			= otCurr.GetDay();  

	nCurHour		= otCurr.GetHour();
	nCurMinute		= otCurr.GetMinute();
	nCurSecond		= otCurr.GetSecond();

	switch (nMode)
	{
		case 0:
			strTime.Format(_T("CYCLE_%04d%02d%02d"), nCurYear, nCurMonth, nCurDay);
			if (otCurr.GetHour() == 22 || otCurr.GetHour() == 23 || otCurr.GetHour() == 24)
			{
				if (otCurr.GetHour() == 22 && otCurr.GetSecond() > 0)
				{
					tNext = otCurr + COleDateTimeSpan(1, 0, 0, 0);
					strTime.Format(_T("CYCLE_%04d%02d%02d"), tNext.GetYear(), tNext.GetMonth(), tNext.GetDay());
				}
				else
				{
					tNext = otCurr + COleDateTimeSpan(1, 0, 0, 0);
					strTime.Format(_T("CYCLE_%04d%02d%02d"), tNext.GetYear(), tNext.GetMonth(), tNext.GetDay());
				}
			}

			strCreateFile = _T("D:\\AMT840\\Log\\LOT_DATA\\");
			strCreateFile += _T("LD_ROBOT\\");
			strCreateFile += strTime;
			strCreateFile += ".TXT";

			strData.Format(_T("[%04d%02d%02d%02d%02d%02d] LotNo : %s PartNo : %s CYCLE TIME : [%10d]"), nCurYear,
																										nCurMonth,
																										nCurDay,
																										nCurHour,
																										nCurMinute,
																										nCurSecond,
																										strLotNo,
																										strPartNo,
																										dwTime1 / (DWORD)1000);
			break;

		case 1:
			strTime.Format(_T("CYCLE_%04d%02d%02d"), nCurYear, nCurMonth, nCurDay);
			if (otCurr.GetHour() == 22 || otCurr.GetHour() == 23 || otCurr.GetHour() == 24)
			{
				if (otCurr.GetHour() == 22 && otCurr.GetSecond() > 0)
				{
					tNext = otCurr + COleDateTimeSpan(1, 0, 0, 0);
					strTime.Format(_T("CYCLE_%04d%02d%02d"), tNext.GetYear(), tNext.GetMonth(), tNext.GetDay());
				}
				else
				{
					tNext = otCurr + COleDateTimeSpan(1, 0, 0, 0);
					strTime.Format(_T("CYCLE_%04d%02d%02d"), tNext.GetYear(), tNext.GetMonth(), tNext.GetDay());
				}
			}

			strCreateFile = _T("D:\\AMT840\\Log\\LOT_DATA\\");
			strCreateFile += _T("LD_BUFFER\\");
			strCreateFile += strTime;
			strCreateFile += ".TXT";

			strData.Format(_T("[%04d%02d%02d%02d%02d%02d] LotNo : %s PartNo : %s  CYCLE TIME : [%10d]"), nCurYear,
																										 nCurMonth,
																										 nCurDay,
																										 nCurHour,
																										 nCurMinute,
																										 nCurSecond,
																										 strLotNo,
																										 strPartNo,
																										 dwTime1 / (DWORD)1000);
			break;

		case 2:
			strTime.Format(_T("CYCLE_%04d%02d%02d"), nCurYear, nCurMonth, nCurDay);
			if (otCurr.GetHour() == 22 || otCurr.GetHour() == 23 || otCurr.GetHour() == 24)
			{
				if (otCurr.GetHour() == 22 && otCurr.GetSecond() > 0)
				{
					tNext = otCurr + COleDateTimeSpan(1, 0, 0, 0);
					strTime.Format(_T("CYCLE_%04d%02d%02d"), tNext.GetYear(), tNext.GetMonth(), tNext.GetDay());
				}
				else
				{
					tNext = otCurr + COleDateTimeSpan(1, 0, 0, 0);
					strTime.Format(_T("CYCLE_%04d%02d%02d"), tNext.GetYear(), tNext.GetMonth(), tNext.GetDay());
				}
			}

			strCreateFile = _T("D:\\AMT840\\Log\\LOT_DATA\\");
			strCreateFile += _T("UNLD_ROBOT\\");
			strCreateFile += strTime;
			strCreateFile += ".TXT";

			strData.Format(_T("[%04d%02d%02d%02d%02d%02d] LotNo : %s PartNo : %s  CYCLE TIME : [%10d]"), nCurYear,
																										 nCurMonth,
																										 nCurDay,
																										 nCurHour,
																										 nCurMinute,
																										 nCurSecond,
																										 strLotNo,
																										 strPartNo,
																										 dwTime1 / (DWORD)1000);
			break;

		case 3:
			strTime.Format(_T("CYCLE_%04d%02d%02d"), nCurYear, nCurMonth, nCurDay);
			if (otCurr.GetHour() == 22 || otCurr.GetHour() == 23 || otCurr.GetHour() == 24)
			{
				if (otCurr.GetHour() == 22 && otCurr.GetSecond() > 0)
				{
					tNext = otCurr + COleDateTimeSpan(1, 0, 0, 0);
					strTime.Format(_T("CYCLE_%04d%02d%02d"), tNext.GetYear(), tNext.GetMonth(), tNext.GetDay());
				}
				else
				{
					tNext = otCurr + COleDateTimeSpan(1, 0, 0, 0);
					strTime.Format(_T("CYCLE_%04d%02d%02d"), tNext.GetYear(), tNext.GetMonth(), tNext.GetDay());
				}
			}

			strCreateFile = _T("D:\\AMT840\\Log\\LOT_DATA\\");
			strCreateFile += _T("UNLD_BUFFER\\");
			strCreateFile += strTime;
			strCreateFile += ".TXT";

			strData.Format(_T("[%04d%02d%02d%02d%02d%02d] LotNo : %s PartNo : %s  CYCLE TIME : [%10d]"), nCurYear,
																										 nCurMonth,
																										 nCurDay,
																										 nCurHour,
																										 nCurMinute,
																										 nCurSecond,
																										 strLotNo,
																										 strPartNo,
																										 dwTime1 / (DWORD)1000);
			break;

		case 4:
			strTime.Format(_T("CYCLE_%04d%02d%02d"), nCurYear, nCurMonth, nCurDay);
			if (otCurr.GetHour() == 22 || otCurr.GetHour() == 23 || otCurr.GetHour() == 24)
			{
				if (otCurr.GetHour() == 22 && otCurr.GetSecond() > 0)
				{
					tNext = otCurr + COleDateTimeSpan(1, 0, 0, 0);
					strTime.Format(_T("CYCLE_%04d%02d%02d"), tNext.GetYear(), tNext.GetMonth(), tNext.GetDay());
				}
				else
				{
					tNext = otCurr + COleDateTimeSpan(1, 0, 0, 0);
					strTime.Format(_T("CYCLE_%04d%02d%02d"), tNext.GetYear(), tNext.GetMonth(), tNext.GetDay());
				}
			}

			strCreateFile = _T("D:\\AMT840\\Log\\LOT_DATA\\");
			strCreateFile += _T("TEST_SITE_ROBOT\\");
			strCreateFile += strTime;
			strCreateFile += ".TXT";

			strData.Format(_T("[%04d%02d%02d%02d%02d%02d] LotNo : %s PartNo : %s  CYCLE TIME : [%10d]"), nCurYear,
																										 nCurMonth,
																										 nCurDay,
																										 nCurHour,
																										 nCurMinute,
																										 nCurSecond,
																										 strLotNo,
																										 strPartNo,
																										 dwTime1 / (DWORD)1000);
			break;

		case 5:
			strTime.Format(_T("CYCLE_%04d%02d%02d"), nCurYear, nCurMonth, nCurDay);
			if (otCurr.GetHour() == 22 || otCurr.GetHour() == 23 || otCurr.GetHour() == 24)
			{
				if (otCurr.GetHour() == 22 && otCurr.GetSecond() > 0)
				{
					tNext = otCurr + COleDateTimeSpan(1, 0, 0, 0);
					strTime.Format(_T("CYCLE_%04d%02d%02d"), tNext.GetYear(), tNext.GetMonth(), tNext.GetDay());
				}
				else
				{
					tNext = otCurr + COleDateTimeSpan(1, 0, 0, 0);
					strTime.Format(_T("CYCLE_%04d%02d%02d"), tNext.GetYear(), tNext.GetMonth(), tNext.GetDay());
				}
			}

			strCreateFile = _T("D:\\AMT840\\Log\\LOT_DATA\\");
			strCreateFile += _T("TEST_SITE_ROBOT\\");
			strCreateFile += strTime;
			strCreateFile += ".TXT";

			strData.Format(_T("[%04d%02d%02d%02d%02d%02d] LotNo : %s PartNo : %s  CYCLE TIME : [%10d]"), nCurYear,
																										 nCurMonth,
																										 nCurDay,
																										 nCurHour,
																										 nCurMinute,
																										 nCurSecond,
																										 strLotNo,
																										 strPartNo,
																										 dwTime1 / (DWORD)1000);
			break;

		case 6:
			strTime.Format(_T("CYCLE_%04d%02d%02d"), nCurYear, nCurMonth, nCurDay);
			if (otCurr.GetHour() == 22 || otCurr.GetHour() == 23 || otCurr.GetHour() == 24)
			{
				if (otCurr.GetHour() == 22 && otCurr.GetSecond() > 0)
				{
					tNext = otCurr + COleDateTimeSpan(1, 0, 0, 0);
					strTime.Format(_T("CYCLE_%04d%02d%02d"), tNext.GetYear(), tNext.GetMonth(), tNext.GetDay());
				}
				else
				{
					tNext = otCurr + COleDateTimeSpan(1, 0, 0, 0);
					strTime.Format(_T("CYCLE_%04d%02d%02d"), tNext.GetYear(), tNext.GetMonth(), tNext.GetDay());
				}
			}

			strCreateFile = _T("D:\\AMT840\\Log\\LOT_DATA\\");
			strCreateFile += _T("LD_TRAY\\");
			strCreateFile += strTime;
			strCreateFile += ".TXT";

			strData.Format(_T("[%04d%02d%02d%02d%02d%02d] LotNo : %s PartNo : %s  CYCLE TIME : %10d s [%10d(SMEMA) %10d(MOVE) %10d(STOP) %10d]"), nCurYear,
																																						nCurMonth,
																																						nCurDay,
																																						nCurHour,
																																						nCurMinute,
																																						nCurSecond,
																																						strLotNo,
																																						strPartNo,
																																						dwTime2 / (DWORD)1000,
																																						dwTime1 / (DWORD)1000,
																																						(dwTime2 / (DWORD)1000) - (dwTime3 / (DWORD)1000) - (dwTime1 / (DWORD)1000),
																																						dwTime3 / (DWORD)1000,
																																						dwTime2 / (DWORD)1000);
			break;

		case 7:
			strTime.Format(_T("CYCLE_%04d%02d%02d"), nCurYear, nCurMonth, nCurDay);
			if (otCurr.GetHour() == 22 || otCurr.GetHour() == 23 || otCurr.GetHour() == 24)
			{
				if (otCurr.GetHour() == 22 && otCurr.GetSecond() > 0)
				{
					tNext = otCurr + COleDateTimeSpan(1, 0, 0, 0);
					strTime.Format(_T("CYCLE_%04d%02d%02d"), tNext.GetYear(), tNext.GetMonth(), tNext.GetDay());
				}
				else
				{
					tNext = otCurr + COleDateTimeSpan(1, 0, 0, 0);
					strTime.Format(_T("CYCLE_%04d%02d%02d"), tNext.GetYear(), tNext.GetMonth(), tNext.GetDay());
				}
			}

			strCreateFile = _T("D:\\AMT840\\Log\\LOT_DATA\\");
			strCreateFile += _T("UNLD_TRAY\\");
			strCreateFile += strTime;
			strCreateFile += ".TXT";

			strData.Format(_T("[%04d%02d%02d%02d%02d%02d] LotNo : %s PartNo : %s  CYCLE TIME : %10d s [%10d(SMEMA) %10d(MOVE) %10d(STOP) %10d]"), nCurYear,
																																						nCurMonth,
																																						nCurDay,
																																						nCurHour,
																																						nCurMinute,
																																						nCurSecond,
																																						strLotNo,
																																						strPartNo,
																																						dwTime2 / (DWORD)1000,
																																						dwTime1 / (DWORD)1000,
																																						(dwTime2 / (DWORD)1000) - (dwTime3 / (DWORD)1000) - (dwTime1 / (DWORD)1000),
																																						dwTime3 / (DWORD)1000,
																																						dwTime2 / (DWORD)1000);
			break;
	}
	
	OnStringToChar(strCreateFile, chFileName);
	nExistence = _access(chFileName, 0);

	if (nExistence == -1)  /* ���� ������ */
	{
//		strCreateFile = st_path_info.strPathCycle + strTime;
//		strCreateFile += ".TXT";
		OnStringToChar(strCreateFile, chFileName);
	}

	/* ************************************************************************** */
	/* �˶� �߻� Ƚ�� ���� ���� ���Ͽ� �߰� ������ ���� ���Ϸ� ����               */
	/* ************************************************************************** */
	fopen_s(&fp, chFileName, "a+");
	if(!fp)
	{
		//		AfxMessageBox(_T("The failure because we open the file."));
		return;
	}
	/* ************************************************************************** */


	strContent.Format(_T("| %-100s |"), strData);
	OnStringToChar(strContent, chMsg);
	fprintf(fp,"%s\r\n", chMsg) ;

	if (ferror(fp))  
	{
		fclose(fp); 

		return ;
	}

	fclose(fp); 
}


void CPublicFunction::OnLotCycleData(CString strLotNo, CString strPartNo, int nCount, DWORD dwTray, DWORD dwLot)
{
	CString strFileName;				// ���������� ������ ���� �̸� ���� ���� 
	CString strCreateFile;				// �˶� ���� ������ ���Ͽ� ���� [����]+[���ϸ�]+[Ȯ����] ���� ���� 
	CString strListName, strTempData;	// �� �κк� �˶� �߻� Ƚ�� ���� ������ �α� ���� ���� ���� ���� 
	CString strContent;				// �� ���ο� ����� ���ڿ� ���� ���� ���� 
	int nExistence;						// ���� ���� ���� ���� �÷��� 
	char chFileName[256];				// �˻��� ���� ���� ���� �Լ� 
	char chMsg[1000];
	FILE  *fp ;							// ���Ͽ� ���� ������ ���� ���� 

	CString strTitleTime, strFileTime;		// ���Ͽ� ����� Ÿ��Ʋ �ð� ���� ����
	int nCurYear, nCurMonth, nCurDay;					// ��, ��, �� ���� ����
	int nCurHour, nCurMinute, nCurSecond;				// ��, ��, �� ���� ����
	int nMtbi = 0;

	double dAve;

	COleDateTime otCurr;									// ���� �ð� ���� ����
	CTime tCurr;										// Ÿ��Ʋ �ð� ���� ����

	CString strTemp;

	CString strData, strTime;

	/* ************************************************************************** */
	/* ���� �̸����� ����� ��¥ �����Ѵ�                                         */
	/* ************************************************************************** */
	otCurr = COleDateTime::GetCurrentTime();

	nCurYear		= otCurr.GetYear();  
	nCurMonth		= otCurr.GetMonth();  
	nCurDay			= otCurr.GetDay();  

	nCurHour		= otCurr.GetHour();
	nCurMinute		= otCurr.GetMinute();
	nCurSecond		= otCurr.GetSecond();

	strTime.Format(_T("CYCLE_%04d%02d%02d"), nCurYear, nCurMonth, nCurDay);

	strCreateFile = st_path_info.strPathCycle + strTime;
	strCreateFile += ".TXT";

	OnStringToChar(strCreateFile, chFileName);
	nExistence = _access(chFileName, 0);

	if (nExistence == -1)  /* ���� ������ */
	{
		strCreateFile = st_path_info.strPathCycle + strTime;
		strCreateFile += ".TXT";
		OnStringToChar(strCreateFile, chFileName);
	}

	/* ************************************************************************** */
	/* �˶� �߻� Ƚ�� ���� ���� ���Ͽ� �߰� ������ ���� ���Ϸ� ����               */
	/* ************************************************************************** */
	fopen_s(&fp, chFileName, "a+");
	if(!fp)
	{
		//		AfxMessageBox(_T("The failure because we open the file."));
		return;
	}
	/* ************************************************************************** */
	dAve = 0.0f;
	if (nCount > 0) 
	{
		dAve = ((double)dwLot / (double)nCount);
		st_work_info.dDailyCycle = dAve;
	}
	strTemp.Format(_T("LOTNO : [%s] PARTNO : [%s] Cycle Time [Tray : %d] [Lot : %.2f]"), strLotNo, strPartNo, dwTray, dAve);
	strContent.Format(_T("| %-100s |"), strTemp);
	OnStringToChar(strContent, chMsg);
	fprintf(fp,"%s\r\n", chMsg) ;

	if (ferror(fp))  
	{
		//		Func.MsgLog("���� ���� ����!..") ;
		fclose(fp); 

		return ;
	}

	fclose(fp); 
}

void CPublicFunction::OnDailyCycleData(CString strLotNo, CString strPartNo, double dTime, CTime tStart, CTime tEnd)
{
	CString strFileName;				// ���������� ������ ���� �̸� ���� ���� 
	CString strCreateFile;				// �˶� ���� ������ ���Ͽ� ���� [����]+[���ϸ�]+[Ȯ����] ���� ���� 
	CString strListName, strTempData;	// �� �κк� �˶� �߻� Ƚ�� ���� ������ �α� ���� ���� ���� ���� 
	CString strContent;				// �� ���ο� ����� ���ڿ� ���� ���� ���� 
	int nExistence;						// ���� ���� ���� ���� �÷��� 
	char chFileName[256];				// �˻��� ���� ���� ���� �Լ� 
	char chMsg[1000];
	FILE  *fp ;							// ���Ͽ� ���� ������ ���� ���� 

	CString strTitleTime, strFileTime;		// ���Ͽ� ����� Ÿ��Ʋ �ð� ���� ����
	int nCurYear, nCurMonth, nCurDay;					// ��, ��, �� ���� ����
	int nCurHour, nCurMinute, nCurSecond;				// ��, ��, �� ���� ����
	int nMtbi = 0;

	COleDateTime otCurr;									// ���� �ð� ���� ����
	CTime tCurr;										// Ÿ��Ʋ �ð� ���� ����

	CString strTemp;

	CString strData, strTime;

	/* ************************************************************************** */
	/* ���� �̸����� ����� ��¥ �����Ѵ�                                         */
	/* ************************************************************************** */
	otCurr = COleDateTime::GetCurrentTime();

	nCurYear		= otCurr.GetYear();  
	nCurMonth		= otCurr.GetMonth();  
	nCurDay			= otCurr.GetDay();  

	nCurHour		= otCurr.GetHour();
	nCurMinute		= otCurr.GetMinute();
	nCurSecond		= otCurr.GetSecond();

	strTime.Format(_T("D_CYCLE_%04d%02d%02d"), nCurYear, nCurMonth, nCurDay);
	strCreateFile = st_path_info.strPathCycle + strTime;
	strCreateFile += ".TXT";

	OnStringToChar(strCreateFile, chFileName);
	nExistence = _access(chFileName, 0);

	if (nExistence == -1)  /* ���� ������ */
	{
		strCreateFile = st_path_info.strPathCycle + strTime;
		strCreateFile += ".TXT";
		OnStringToChar(strCreateFile, chFileName);
	}

	/* ************************************************************************** */
	/* �˶� �߻� Ƚ�� ���� ���� ���Ͽ� �߰� ������ ���� ���Ϸ� ����               */
	/* ************************************************************************** */
	fopen_s(&fp, chFileName, "a+");
	if(!fp)
	{
		//		AfxMessageBox(_T("The failure because we open the file."));
		return;
	}
	/* ************************************************************************** */

	strTemp.Format(_T("[%04d%02d%02d %02d:%02d:%02d] : START : [%04d%02d%02d %02d:%02d:%02d] END :[%04d%02d%02d %02d:%02d:%02d] PARTNO : [%s] LOTNO : [%s] Cycle Time [%.2f]"), nCurYear,
																																										        nCurMonth,
																																										        nCurDay,
																																										        nCurHour,
																																										        nCurMinute,
																																												nCurSecond,
																																												tStart.GetYear(),
																																												tStart.GetMonth(),
																																												tStart.GetDay(),
																																												tStart.GetHour(),
																																												tStart.GetMinute(),
																																												tStart.GetSecond(),
																																												tEnd.GetYear(),
																																												tEnd.GetMonth(),
																																												tEnd.GetDay(),
																																												tEnd.GetHour(),
																																												tEnd.GetMinute(),
																																												tEnd.GetSecond(),
																																												strPartNo,
																																												strLotNo, 
																																												dTime);
	strContent.Format(_T("%s"), strTemp);
	OnStringToChar(strContent, chMsg);
	fprintf(fp,"%s\r\n", chMsg) ;

	if (ferror(fp))  
	{
		//		Func.MsgLog("���� ���� ����!..") ;
		fclose(fp); 

		return ;
	}

	fclose(fp); 
}

CString	CPublicFunction::OnRecipeCheck(CString strPartNo)
{
	CString strRecipe = _T("");						// �Լ� ���� �÷���
	CString strTemp;

	HANDLE hFind;
    WIN32_FIND_DATA fd;

	CString strFileName = st_path_info.strBasic;

	if (strFileName.Right (1) != "\\")
	{
        strFileName += "\\";
	}

	strFileName += "*.*";
    
    if ((hFind = ::FindFirstFile ((LPCTSTR) strFileName, &fd)) != INVALID_HANDLE_VALUE) 
	{
        while (::FindNextFile (hFind, &fd)) 
		{
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				strTemp = (fd.cFileName);
				if (OnRecipeMatch(strTemp, strPartNo) == RET_GOOD)
				{
					strRecipe = strTemp;
					break;
				}
			}
        }
		::FindClose(hFind);
    }

	return strRecipe;
}

int	CPublicFunction::OnRecipeMatch(CString strFileName, CString strPartNo)
{
	TCHAR chData[300];

	int nPos;
	int nLength;
	int i, j;
	int nCount;

	CString strTemp;
	CString strRecipe;
	CString strData;
	CString strFind[2];

	BOOL bFind;

	nPos = strFileName.Find(_T("."), 0);
	strRecipe = strFileName.Mid(0, nPos);
	strTemp.Format(_T("%s_TOTAL"), strRecipe);

	GetPrivateProfileString(st_basic_info.strScrapHead, strTemp, _T("0"), chData, sizeof(chData), _T("c:\\AMT840\\RECIPE_RULE.TXT"));
	strTemp.Format(_T("%s"), chData);
	nCount = _wtoi(strTemp);

	if (nCount == 0) return RET_ERROR;

	for (i=0; i<nCount; i++)
	{
		strTemp.Format(_T("%s_%02d"), strRecipe, i+1);
		GetPrivateProfileString(st_basic_info.strScrapHead, strTemp, _T("0"), chData, sizeof(chData), _T("c:\\AMT840\\RECIPE_RULE.TXT"));
		strData.Format(_T("%s"), chData);
		
		bFind = TRUE;
		nLength = strData.GetLength();

		for(j=0; j<nLength; j++)
		{
			strFind[0] = strPartNo.GetAt(j);
			strFind[1] = strData.GetAt(j);

			if (strFind[1] != _T("_"))
			{
				if (strFind[0] != strFind[1])
				{
					bFind = FALSE;
					break;
				}
			}
		}
		if (bFind) return RET_GOOD;
	}

	return RET_ERROR;
}




/////////////////////////////////////////////////////////////
// tray & Buffer �� �۾� ������ ������ ������ ã�´� 
// james 2016.0803 
// ��� ����(load/unload stacker) ����
//////////////////////////////////////////////////////////////2016.0814 
int CPublicFunction::Find_TrayWork_Pos(int nMode, int nSite, int nDVCYesNO, int *npFindPosYXCPB, int nRobotSite, CString str_LotNo) //first tray Y, first tray X, first picker num, first buff num, �۾� ���ǿ� �����ϴ� �� ���� �Ǵ� ���� ���� ��ü ����
{//james 2016.0909 �߰� CString str_LotNo lot ������ ���Ͽ� ó�� 

	int x, y, i, nFuncRet, nFlag;
	int nFirstY =0, nFirstX=0;
	int nPassCount = 0;
	bool bFlag = false;
	int nCount[2]={0,0};

	nFuncRet = RET_SKIP;

	 npFindPosYXCPB[0] = 0;npFindPosYXCPB[1] = 0;npFindPosYXCPB[2] = 0;npFindPosYXCPB[3] = 0;npFindPosYXCPB[4] = 0; //clear
	 nFlag = CTL_NO;

	 switch(nSite)
	 {
	 case THD_LD_TRAY_PLATE: //load olate tray ������ ����
	 case THD_ULD_1_STACKER: //unload 1 tray�� �� ���� ���� 
	 case THD_ULD_2_STACKER: //unload 2 tray�� �� ���� ���� 
	 case THD_REJECT_OUT_1_STACKER: //reject 1 tray�� �� ���� ���� 
	 case THD_REJECT_OUT_2_STACKER: //reject 1 tray�� �� ���� ���� 

		nFirstY = 0; //st_recipe_info.nTrayY;
		nFirstX = 0;	
		for(y = nFirstY; y < st_recipe_info.nTrayY; y++) //Y ���� ����
		{
			for(x = nFirstX; x < st_recipe_info.nTrayX; x++) //X ���� ���� 
			{ 
				if(st_tray_info[nSite].st_pcb_info[y][x].nYesNo == nDVCYesNO && (st_tray_info[nSite].st_pcb_info[y][x].strLotNo == str_LotNo || str_LotNo == "0") )//�Ò̤� 2016.0909 ���ǿ� ������ ������ üũ
				{
					if(nFlag == CTL_NO)
					{
						npFindPosYXCPB[0] = y; npFindPosYXCPB[1] = x;
						nFuncRet = RET_GOOD;
						nFlag = CTL_YES;
					}
					npFindPosYXCPB[2]++; //�۾� ������ count ���� üũ 			
				}
			}
		}
		break;

	case THD_LD_BUFF: //�ε� ���۴� test robot�� �۾��ϱ�� ������  test site�� ���� ������ ���� ���� �� ��ġ�� ��� ���ΰ� �����ȴ� 
		nFirstY = 0;  nFirstX = 0;	
		if(nRobotSite == THD_WORK_RBT) //����̸� �ε� ���ۿ� ���⸸ �����ϴ�, PLACE ������ 0,1,2,3 
		{
			if(nMode == WORK_PLACE)
			{
				for(i = 0; i < st_recipe_info.nLdUldBuffer_Num; i++) //work robot�� �ε� ���ۿ� ���縦 �������� 0->1->2->3 ������ �۾�(���� �ڿ��� ���� 1���̴�)
				{
// 					if(st_buffer_info[nSite].st_pcb_info[i].nYesNo == nDVCYesNO && st_buffer_info[nSite].st_pcb_info[i].nEnable == CTL_YES
// 						&& (st_buffer_info[nSite].st_pcb_info[i].strLotNo == str_LotNo || str_LotNo == "0") ) //nDVCYesNO == CTL_NO ���ǿ� ������ ������ üũ, ��밡���� ���Ͽ��� �۾�����
// 					{
// 						if(nFlag == CTL_NO)
// 						{
// 							npFindPosYXCPB[0] = i; npFindPosYXCPB[1] = i;
// 							nFuncRet = RET_GOOD;
// 							nFlag = CTL_YES;
// 						}
// 						npFindPosYXCPB[2]++; //�۾� ������ count ���� üũ 			
// 					}
					//2016.1130
					//���۸� �� ����Ѵٰ� �����Ѵ�.
					if(st_sync_info.nSmema_Tray_Input_Req == CTL_YES)
					{
						if(st_buffer_info[nSite].st_pcb_info[i].nYesNo == nDVCYesNO && st_buffer_info[nSite].st_pcb_info[i].nEnable == CTL_YES
							&& (st_buffer_info[nSite].st_pcb_info[i].strLotNo == str_LotNo || str_LotNo == "0") ) //nDVCYesNO == CTL_NO ���ǿ� ������ ������ üũ, ��밡���� ���Ͽ��� �۾�����
						{
							if(nFlag == CTL_NO)
							{
								npFindPosYXCPB[0] = i; npFindPosYXCPB[1] = i;
								nFuncRet = RET_GOOD;
								nFlag = CTL_YES;
							}
							npFindPosYXCPB[2]++; //�۾� ������ count ���� üũ 			
						}
						st_sync_info.nSmema_Tray_Input_Req = CTL_NO;
					}
					else
					{
						//st_buffer_info[nSite].st_pcb_info[i].nEnable = CTL_YES; //kwlee 2016.1227 Socket Off ���� �ӽ� ����.
						if(st_buffer_info[nSite].st_pcb_info[i].nYesNo == nDVCYesNO && st_buffer_info[nSite].st_pcb_info[i].nEnable == CTL_YES
							&& (st_buffer_info[nSite].st_pcb_info[i].strLotNo == str_LotNo || str_LotNo == "0") ) //nDVCYesNO == CTL_NO ���ǿ� ������ ������ üũ, ��밡���� ���Ͽ��� �۾�����
						{
							if(nFlag == CTL_NO)
							{
								npFindPosYXCPB[0] = i; npFindPosYXCPB[1] = i;
								nFuncRet = RET_GOOD;
								nFlag = CTL_YES;
							}
							npFindPosYXCPB[2]++; //�۾� ������ count ���� üũ 			
						}
					}
 
				}
			}
		}
		else if(nRobotSite == THD_TEST_RBT) // test robot�� �ε� ���ۿ��� ����̽��� ���ų� ���� �� �ִ� , Fix ��ġ���� 4���� ���� ������ ���� �ѹ��� �۾� �Ѵ� 
		{
			for(i = 0; i < st_recipe_info.nLdUldBuffer_Num; i++) //work robot�� �ε� ���ۿ� ���縦 �������� 0->1->2->3 ������ �۾�(���� �ڿ��� ���� 1���̴�)
			{
				if(st_buffer_info[nSite].st_pcb_info[i].nYesNo == nDVCYesNO && (st_buffer_info[nSite].st_pcb_info[i].strLotNo == str_LotNo  || str_LotNo == "0"))
				{
					if(nFlag == CTL_NO)
					{
						npFindPosYXCPB[0] = 0; npFindPosYXCPB[1] = 0; //��ü�� �ѹ��� ���ų� ���� ���� ��ġ�� ó�� 0 ���� �ϸ� �ȴ� 
						nFuncRet = RET_GOOD;
						nFlag = CTL_YES;
					}
					npFindPosYXCPB[2]++; //�۾� ������ count ���� üũ 	
				}
			}
		}		
		break;


	case THD_ULD_BUFF: //�׽�Ʈ�� ���� ���縦 �׽�Ʈ ���Ͽ��� ���� GOOD, Reject ���縸 �̰��� ���� ���´�, Retest ����� retest buffer�� ���� ���´�  
		nFirstY = 0;  nFirstX = 0;	 
		if(nRobotSite == THD_WORK_RBT) //����̸� ��ε� ���ۿ��� ���� ���⸸ �����ϴ�,  PICK ������ 3,2,1,0 �̴� 
		{
			if(nMode == WORK_PICK)
			{
				//work robot�� ��ε� ���� PICK ������ �տ��� ���� ���� 3->2->1->0 �� ������ �۾��Ѵ� 
				nFirstY = st_recipe_info.nLdUldBuffer_Num - 1; 
				for(i = nFirstY; i >= 0; i--) //Work��ε� ������ �۾��� 3->2->1->0, ������ ������ �۾��ؾ� ������ ����  
				{				 
					if((nFirstY - i) < 0 ) continue; //Ȥ�� - ���� ���� �޸� leak�� �߻�����

					if(st_buffer_info[nSite].st_pcb_info[i].nYesNo == nDVCYesNO && (st_buffer_info[nSite].st_pcb_info[i].strLotNo == str_LotNo  || str_LotNo == "0")) //nDVCYesNO == CTL_YES ���ǿ� ������ ������ üũ
					{
						if(nFlag == CTL_NO)
						{
							npFindPosYXCPB[0] = i; npFindPosYXCPB[1] = i;
							nFuncRet = RET_GOOD;
							nFlag = CTL_YES;
						}
						npFindPosYXCPB[2]++; //�۾� ������ count ���� üũ 			
					} 
				}
			}
		}
		else if(nRobotSite == THD_TEST_RBT) // �ε� ���ۿ��� ����̽��� ���� �� �ִ� , Fix ��ġ���� 4���� ���� ������ ���� �ѹ��� �۾� �Ѵ� 
		{
			for(i = 0; i < 4; i++) //Work��ε� ������ �۾��� 3->2->1->0, ������ ������ �۾��ؾ� ������ ����  
			{
				if(st_buffer_info[nSite].st_pcb_info[i].nYesNo == nDVCYesNO && (st_buffer_info[nSite].st_pcb_info[i].strLotNo == str_LotNo  || str_LotNo == "0"))
				{
					if(nFlag == CTL_NO)
					{
						npFindPosYXCPB[0] = 0; npFindPosYXCPB[1] = 0; //��ü�� �ѹ��� ���ų� ���� ���� ��ġ�� ó�� 0 ���� �ϸ� �ȴ� 
						nFuncRet = RET_GOOD;
						nFlag = CTL_YES;
					}
					npFindPosYXCPB[2]++; //�۾� ������ count ���� üũ 	
				}
			}
		}
		break; 

	//�κ�Ʈ�� ���� ���ݹ�����, �ϳ��ɷ� �ϳ��� �� 15���� ���� �ִ�.
	

	//james 2016.0814 ���׽�Ʈ ��� ����
	//���� pick & place ��� (��Ŀ�� ��ü Up/Down���� ���۽� ���簣 �浹�� �߻��� �� �־� ���� ���ǰ� �ʿ��ϴ� //
	//1�� �۾����� : 0,2,4,6, 8,10,12,14 - 8�� 
	//2�� �۾����� : 1,3,5,7, 9,11,13    - 7��
	// PLACE ��� ������.
	// 1. 1�� �۾�����(¦����) ������ ������ �ѹ��� ���� ������ ã�� ���´�. 
	// 2. 2�� �۾�����(Ȧ����) ���� ���縦 ���� ��Ȳ�̸� , �̰� ������ ����, ������ 1�� �۾� ������ ���縦 ���׽�Ʈ�� �����Ѵ� 
	// 3. �� ����Ŭ ���� ��ε� ������, Ȥ�� Ȧ������ ���׽�Ʈ ���ۿ� ���簡 ������ �� ���縦 ¦������ �ִ��� ������ �ش� 
	// 4. �� Ȧ������ �ӽ� ���� �������� ������ ����� �־�� �Ѵ� 
    
	case THD_RETEST_1_BUFF: //retest buffer #1 �� ���� Lot ���� �ΰ�(Retest#1, Retest#2)�� ������ �ϳ��� ���� �����Ͽ� ����Ѵ�
		nFirstY = 0;  nFirstX = 0;	i = 0; 		
		//int nDVC[4];
		//int nNo = 0;


		for(i = nFirstX; i < st_recipe_info.nRetestBuffer_Num; i++) //X ���� ���� 15�� ���
		{ 
			if(st_buffer_info[nSite].st_pcb_info[i].nYesNo == nDVCYesNO && (st_buffer_info[nSite].st_pcb_info[i].strLotNo == str_LotNo  || str_LotNo == "0"))// && (nMode == 100 || st_tray_info[nSite].st_pcb_info[y][i].nYesNo == CTL_YES)) //���ǿ� ������ ������ üũ
			{
				if(nFlag == CTL_NO)
				{
					npFindPosYXCPB[0] = i; npFindPosYXCPB[1] = i;
					nFuncRet = RET_GOOD;
					nFlag = CTL_YES;
				}
				npFindPosYXCPB[2]++; //�۾� ������ count ���� üũ 			
			}
		}
		break;

	case THD_RETEST_2_BUFF: //retest buffer #2 �� ���� Lot ���� �ΰ�(Retest#1, Retest#2)�� ������ �ϳ��� ���� �����Ͽ� ����Ѵ�
		nFirstY = 0;  nFirstX = 0;	i = 0; 
		for(i = nFirstX; i < st_recipe_info.nRetestBuffer_Num; i++) //X ���� ���� 
		{ 
			if(st_buffer_info[nSite].st_pcb_info[i].nYesNo == nDVCYesNO && (st_buffer_info[nSite].st_pcb_info[i].strLotNo == str_LotNo || str_LotNo == "0"))// && (nMode == 100 || st_tray_info[nSite].st_pcb_info[y][i].nYesNo == CTL_YES)) //���ǿ� ������ ������ üũ
			{
				if(nFlag == CTL_NO)
				{
					npFindPosYXCPB[0] = i; npFindPosYXCPB[1] = i;
					nFuncRet = RET_GOOD;
					nFlag = CTL_YES;
				}
				npFindPosYXCPB[2]++; //�۾� ������ count ���� üũ 			
			}
		}
		break;

	//LEFT TEST SITE
	case THD_TESTSITE_1: //test site ��ȣ
	case THD_TESTSITE_2: //test site ��ȣ
	case THD_TESTSITE_3: //test site ��ȣ
	case THD_TESTSITE_4: //test site ��ȣ
		nFirstY = 0;  nFirstX = 0;	i = 0; nCount[0]=0; nCount[1]=0;
		
		nFirstX = st_recipe_info.nTestSocket_Num - 1;  //8���� ������ ���� : 7->6->5->4, 3->2->1->0
		for(i = nFirstX; i >= 0; i--) //X ���� ����,.  7->6->5->4, 3->2->1->0
		{ 
			if(nDVCYesNO == CTL_YES) //����̽� �ִ°��� üũ�ϴ� ��ƾ, ���縦 ����� ���� ���� 
			{
				//if(st_test_site_info[nSite].st_pcb_info[i].nYesNo == nDVCYesNO && st_test_site_info[nSite].nTestBdStart == BD_END)
				if(st_test_site_info[nSite].st_pcb_info[i].nYesNo		== nDVCYesNO && 
				   st_test_site_info[nSite].st_pcb_info[i].nTestBdStart == BD_END && (st_test_site_info[nSite].st_pcb_info[i].strLotNo == str_LotNo || str_LotNo == "0") )
				{//���Ͽ� ����̽��� �ְ�, �׽�Ʈ�� ���� ���縸 ��� ������ �������� �ν����� 
					if(nFlag == CTL_NO)
					{
						if(i > 3) //7,6,5,4 ���� ���� ����
						{
							npFindPosYXCPB[0] = i; npFindPosYXCPB[1] = i;//���� ���� ��ġ ���� 
							npFindPosYXCPB[0] = 0; npFindPosYXCPB[1] = 0;//���������� ���� ��ġ (�̰��� �����)
						}
						else
						{//3,2,1,0 ���� ���� ����
							npFindPosYXCPB[0] = i; npFindPosYXCPB[1] = i; //���� ���� ��ġ ���� 
							npFindPosYXCPB[0] = 4; npFindPosYXCPB[1] = 4; //���������� ���� ��ġ (�̰��� �����)
						}

						nFuncRet = RET_GOOD;
						nFlag = CTL_YES;
					}
					npFindPosYXCPB[2]++; //�۾� ������ count ���� üũ 			
				}
			}
			else //if(nDVCYesNO == CTL_NO) //����̽� ���°��� üũ�ϴ� ��ƾ, ���縦 ���� ���� ���� Ȯ��
			{
				// if(st_test_site_info[nSite].st_pcb_info[i].nYesNo == nDVCYesNO && st_test_site_info[nSite].st_pcb_info[i].nEnable == CTL_YES)
				if(st_test_site_info[nSite].st_pcb_info[i].nYesNo		== nDVCYesNO && 
				   st_test_site_info[nSite].st_pcb_info[i].nEnable		== CTL_YES)
				{//������ ����ְ�, ����ϴ� ���ϸ� �������� ��� ������ �������� �ν����� 
					if(nFlag == CTL_NO || 1)
					{
						if(i > 3) //7,6,5,49 ���� ���� ����
						{
							nCount[0]++;
							if(nCount[0] >= 1)//james 2016.0926 ���� ���� ���Ͽ����� ������ 4)
							{
								npFindPosYXCPB[0] = i; npFindPosYXCPB[1] = i;
								npFindPosYXCPB[0] = 0; npFindPosYXCPB[1] = 0;//���������� ���� ��ġ (�̰��� �����)

								nFuncRet = RET_GOOD;
								nFlag = CTL_YES;
								npFindPosYXCPB[2]++; //�۾� ������ count ���� üũ 			
							}
						}
						else
						{//3,2,1,0 ���� ���� ����
							nCount[1]++;
							if(nCount[1] >= 1)//james 2016.0926 ���� ���� ���Ͽ����� ������ 4)
							{
								npFindPosYXCPB[0] = i; npFindPosYXCPB[1] = i;
								npFindPosYXCPB[0] = 4; npFindPosYXCPB[1] = 4;//���������� ���� ��ġ (�̰��� �����)

								nFuncRet = RET_GOOD;
								nFlag = CTL_YES;
								npFindPosYXCPB[2]++; //�۾� ������ count ���� üũ 			
							}
						}						
					}
					
				}
			}
		}
		break;

	//RIGHT TEST SITE
	case THD_TESTSITE_5: //test site ��ȣ
	case THD_TESTSITE_6: //test site ��ȣ
	case THD_TESTSITE_7: //test site ��ȣ
	case THD_TESTSITE_8: //test site ��ȣ 
		nFirstY = 0;  nFirstX = 0;	i = 0;  nCount[0]=0; nCount[1]=0;
		 
		for(i = nFirstX; i < st_recipe_info.nTestSocket_Num; i++) //X ���� ���� 
		{ 
			if(nDVCYesNO == CTL_YES) //����̽� �ִ°��� üũ�ϴ� ��ƾ, ���縦 ����� ���� ���� 
			{
				if(st_test_site_info[nSite].st_pcb_info[i].nYesNo == nDVCYesNO && st_test_site_info[nSite].st_pcb_info[i].nTestBdStart == BD_END
					 && (st_test_site_info[nSite].st_pcb_info[i].strLotNo == str_LotNo || str_LotNo == "0"))
				{//���Ͽ� ����̽��� �ְ�, �׽�Ʈ�� ���� ���縸 ��� ������ �������� �ν����� 
					if(nFlag == CTL_NO)
					{
						if(i < 4) //0,1,2,3 
						{
							npFindPosYXCPB[0] = 0; 
							npFindPosYXCPB[1] = 0;							
						}
						else
						{
							npFindPosYXCPB[0] = 4; 
							npFindPosYXCPB[1] = 4;	
						}
						nFuncRet = RET_GOOD;
						nFlag = CTL_YES;
					}
					npFindPosYXCPB[2]++; //�۾� ������ count ���� üũ 			
				}
			}
			else //if(nDVCYesNO == CTL_NO) //����̽� ���°��� üũ�ϴ� ��ƾ, ���縦 ���� ���� ���� Ȯ��
			{
				if(st_test_site_info[nSite].st_pcb_info[i].nYesNo == nDVCYesNO && st_test_site_info[nSite].st_pcb_info[i].nEnable == CTL_YES)
				{//������ ����ְ�, ����ϴ� ���ϸ� �������� ��� ������ �������� �ν����� 
					if(nFlag == CTL_NO || 1)
					{
						if(i < 4) //james 2016.0926 0,1,2,3 if(i > 3) //7,6,5,4 ���� ���� ����
						{
							nCount[0]++;
							if(nCount[0] >= 1)//james 2016.0926 ���� ���� ���Ͽ����� ������ 4)
							{
								npFindPosYXCPB[0] = i; npFindPosYXCPB[1] = i;
								npFindPosYXCPB[0] = 0; npFindPosYXCPB[1] = 0;//���������� ���� ��ġ (�̰��� �����)

								nFuncRet = RET_GOOD;
								nFlag = CTL_YES;
								npFindPosYXCPB[2]++; //�۾� ������ count ���� üũ 			
							}
						}
						else
						{//4,5,6,7  ���� ���� ����
							nCount[1]++;
							if(nCount[1] >= 1)//james 2016.0926 ���� ���� ���Ͽ����� ������ 4)
							{
								npFindPosYXCPB[0] = i; npFindPosYXCPB[1] = i;
								npFindPosYXCPB[0] = 4; npFindPosYXCPB[1] = 4;//���������� ���� ��ġ (�̰��� �����)

								nFuncRet = RET_GOOD;
								nFlag = CTL_YES;
								npFindPosYXCPB[2]++; //�۾� ������ count ���� üũ 			
							}
						}						
					}
				}
			}
		}
		 
		break;
	}

	return nFuncRet;
}


/////////////////////////////////////////
//james 2016.0810 
////////////////////////////////////////
/////////////////////////////////////////////////////////////
// tray & Buffer �� �۾� ������ ������ ������ ã�´� 
// james 2016.0803 
// ��� ����(load/unload stacker) ����
//////////////////////////////////////////////////////////////
//james 2016.0821 ���� ������� �ʴ´�
int CPublicFunction::Find_Compare_Buffer_and_Picker(int nMode, int nRobotSite, int nBufferSite, int nRbtYesNo, int nBuffYesNO, int *npFindPosYXCPB) //[0]:y,[1]:y, [3]:count, [4]]first picker, [5]:first buffer. [2]:�۾� ���� ����
{//
	int x, i, nFuncRet, nFlag;
	int nFirstY =0, nFirstX=0;
	bool bFlag = false;
	int nPickerPara = TEST_PICKER_PARA;//4
	int nPickerSameCnt, nTempCnt, nTotalSameCnt = 0;;	
	int nFirstPicker;

	int nMissMatch_Count = 0;

	nFuncRet = RET_SKIP;

	npFindPosYXCPB[0] = 0;npFindPosYXCPB[1] = 0;npFindPosYXCPB[2] = 0;npFindPosYXCPB[3] = 0;npFindPosYXCPB[4] = 0; //clear
	 nFlag = CTL_NO;
	 nTotalSameCnt = 0;;	

	 switch(nRobotSite)
	 {
	 case 1:

		 nMissMatch_Count = 0;
		 nPickerSameCnt = 0;
		 nFirstPicker = -1;
		for(i = 0; i < nPickerPara; i++)
		{
			if(st_picker[nRobotSite].st_pcb_info[i].nYesNo == nRbtYesNo && st_picker[nRobotSite].st_pcb_info[i].nEnable == CTL_YES)//��Ŀ�� ���簡 ��� �ִٴ� ������ ����
			{
				nPickerSameCnt++;
				if(nFirstPicker == -1) nFirstPicker = i;
			}
		}
		if(nPickerSameCnt <= 0)
		{
			nFuncRet = RET_PICKER_NOT_FIND; //�ش� ������ ���� 
			break;
		}

		for(x = nFirstX; x < st_recipe_info.nRetestBuffer_Num; x++) //X ���� ���� 
		{ 
			if(st_buffer_info[nBufferSite].st_pcb_info[x].nYesNo == nBuffYesNO)// && (nMode == 100 || st_tray_info[nSite].st_pcb_info[y][x].nYesNo == CTL_YES)) //���ǿ� ������ ������ üũ
			{
				nTempCnt = 0;
				for(i = nFirstPicker; i < nPickerPara; i++)
				{
					if(st_picker[nRobotSite].st_pcb_info[i].nYesNo == nRbtYesNo && st_picker[nRobotSite].st_pcb_info[i].nEnable == CTL_YES) //��Ŀ �����̰� 
					{
						if(x + (i - nFirstPicker) <= st_recipe_info.nRetestBuffer_Num - 1) //���� ��ġ�� ����� ���� ����
						{
							if(st_buffer_info[nBufferSite].st_pcb_info[x + (i - nFirstPicker)].nYesNo == nBuffYesNO)
							{
								nTotalSameCnt++;
							}
							else
							{
								nMissMatch_Count++;
							}
						}
					}
				}
			}
		}

		break;
	 }
	return nFuncRet;
}


////////////////////////////////////////////////////////////////////////
// ��� ����Ʈ�� Ʈ���� �� ���� ������ ����Ʈ �����Ѵ�  
// james 2016.0803  
////////////////////////////////////////////////////////////////////////
int CPublicFunction::Handler_Tray_DataInfo_Shift(int nMode, int nDvc_Yes_No, int nSend_SIte, int nRcv_Site)
{
	int x=0, y=0;
	 
	//nMode == 0 : ������ �����ϰ� ������ ���� , send data�� ������ �ʴ´�. 
	//nModw == 1 : ������ ����Ʈ �� send data�� ��� Ŭ����
	//nMode == 2 : ���� �۾��ϴ� lot�� ������ �־��ش�. ����̽� ������ nDvc_Yes_No ������ ���� ������ �Ѵ� 
	//nMode == 3 :  nSend_SIte data �� ��� Ŭ����

	if(nMode == 0) // nMode == 0 : ������ �����ϰ� ������ ����, send data�� ������ �ʴ´�. 
	{		
		for(y = 0; y < st_recipe_info.nTrayY; y++)
		{
			for(x=0; x < st_recipe_info.nTrayX; x++)
			{
				st_tray_info[nRcv_Site].st_pcb_info[y][x].strLotNo = st_tray_info[nSend_SIte].st_pcb_info[y][x].strLotNo;
				st_tray_info[nRcv_Site].st_pcb_info[y][x].strSerialNo = st_tray_info[nSend_SIte].st_pcb_info[y][x].strSerialNo;
				st_tray_info[nRcv_Site].st_pcb_info[y][x].strCSerialNo = st_tray_info[nSend_SIte].st_pcb_info[y][x].strCSerialNo;
				st_tray_info[nRcv_Site].st_pcb_info[y][x].strBarcode = st_tray_info[nSend_SIte].st_pcb_info[y][x].strBarcode;
				st_tray_info[nRcv_Site].st_pcb_info[y][x].strPartNo = st_tray_info[nSend_SIte].st_pcb_info[y][x].strPartNo;
				st_tray_info[nRcv_Site].st_pcb_info[y][x].strArrNo = st_tray_info[nSend_SIte].st_pcb_info[y][x].strArrNo;
				st_tray_info[nRcv_Site].st_pcb_info[y][x].strPPID = st_tray_info[nSend_SIte].st_pcb_info[y][x].strPPID;
				st_tray_info[nRcv_Site].st_pcb_info[y][x].strPSID = st_tray_info[nSend_SIte].st_pcb_info[y][x].strPSID;
				st_tray_info[nRcv_Site].st_pcb_info[y][x].strWWN = st_tray_info[nSend_SIte].st_pcb_info[y][x].strWWN; 

				st_tray_info[nRcv_Site].st_pcb_info[y][x].nYesNo = st_tray_info[nSend_SIte].st_pcb_info[y][x].nYesNo;
				st_tray_info[nRcv_Site].st_pcb_info[y][x].nBin = st_tray_info[nSend_SIte].st_pcb_info[y][x].nBin;
				st_tray_info[nRcv_Site].st_pcb_info[y][x].nRetest = st_tray_info[nSend_SIte].st_pcb_info[y][x].nRetest;
				st_tray_info[nRcv_Site].st_pcb_info[y][x].nBdNum = st_tray_info[nSend_SIte].st_pcb_info[y][x].nBdNum;
				st_tray_info[nRcv_Site].st_pcb_info[y][x].nEnable = st_tray_info[nSend_SIte].st_pcb_info[y][x].nEnable;
				//st_tray_info[THD_MACHINE_INPUT_NETWORK_SITE].st_pcb_info[y][x].nScrCodd =5;
			}
		}

		st_tray_info[nRcv_Site].nNewLotIn = st_tray_info[nSend_SIte].nNewLotIn;
		st_tray_info[nRcv_Site].nLastModule = st_tray_info[nSend_SIte].nLastModule; //������ �������� üũ
		st_tray_info[nRcv_Site].nModuleCnt = st_tray_info[nSend_SIte].nModuleCnt;
		st_tray_info[nRcv_Site].nTrayExist = st_tray_info[nSend_SIte].nTrayExist; //tray ���� ��/�� 
		st_tray_info[nRcv_Site].strLotNo = st_tray_info[nSend_SIte].strLotNo;// lot no......
		st_tray_info[nRcv_Site].strPartNo = st_tray_info[nSend_SIte].strPartNo;
		st_tray_info[nRcv_Site].strProcess = st_tray_info[nSend_SIte].strProcess;	// process id.....

		/*if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
		{
			sprintf(st_msg.c_normal_msg, "[CLEAR DATA] SHIFT_IDBUFF_OUTPUTREADY_BUFFER DATA CLEAR");
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
		}*/
	}
	else if (nMode == 1) // nMode == 1 : ������ ����Ʈ ����, send data �� ����� 
	{	 
		for(y = 0; y < st_recipe_info.nTrayY; y++)
		{
			for(x=0; x < st_recipe_info.nTrayX; x++)
			{
				st_tray_info[nRcv_Site].st_pcb_info[y][x].strLotNo	= st_tray_info[nSend_SIte].st_pcb_info[y][x].strLotNo;
				st_tray_info[nRcv_Site].st_pcb_info[y][x].strSerialNo	= st_tray_info[nSend_SIte].st_pcb_info[y][x].strSerialNo;
				st_tray_info[nRcv_Site].st_pcb_info[y][x].strCSerialNo= st_tray_info[nSend_SIte].st_pcb_info[y][x].strCSerialNo;
				st_tray_info[nRcv_Site].st_pcb_info[y][x].strBarcode	= st_tray_info[nSend_SIte].st_pcb_info[y][x].strBarcode;
				st_tray_info[nRcv_Site].st_pcb_info[y][x].strPartNo	= st_tray_info[nSend_SIte].st_pcb_info[y][x].strPartNo;
				st_tray_info[nRcv_Site].st_pcb_info[y][x].strArrNo	= st_tray_info[nSend_SIte].st_pcb_info[y][x].strArrNo;
				st_tray_info[nRcv_Site].st_pcb_info[y][x].strPPID		= st_tray_info[nSend_SIte].st_pcb_info[y][x].strPPID;
				st_tray_info[nRcv_Site].st_pcb_info[y][x].strPSID		= st_tray_info[nSend_SIte].st_pcb_info[y][x].strPSID;
				st_tray_info[nRcv_Site].st_pcb_info[y][x].strWWN		= st_tray_info[nSend_SIte].st_pcb_info[y][x].strWWN; 

				st_tray_info[nRcv_Site].st_pcb_info[y][x].nYesNo		= st_tray_info[nSend_SIte].st_pcb_info[y][x].nYesNo;
				st_tray_info[nRcv_Site].st_pcb_info[y][x].nBin		= st_tray_info[nSend_SIte].st_pcb_info[y][x].nBin;
				st_tray_info[nRcv_Site].st_pcb_info[y][x].nRetest		= st_tray_info[nSend_SIte].st_pcb_info[y][x].nRetest;
				st_tray_info[nRcv_Site].st_pcb_info[y][x].nBdNum		= st_tray_info[nSend_SIte].st_pcb_info[y][x].nBdNum;
				st_tray_info[nRcv_Site].st_pcb_info[y][x].nEnable		= st_tray_info[nSend_SIte].st_pcb_info[y][x].nEnable;


				st_tray_info[nSend_SIte].st_pcb_info[y][x].strLotNo = "";
				st_tray_info[nSend_SIte].st_pcb_info[y][x].strSerialNo = "";
				st_tray_info[nSend_SIte].st_pcb_info[y][x].strCSerialNo = "";
				st_tray_info[nSend_SIte].st_pcb_info[y][x].strBarcode = "";
				st_tray_info[nSend_SIte].st_pcb_info[y][x].strPartNo = "";
				st_tray_info[nSend_SIte].st_pcb_info[y][x].strArrNo = "";
				st_tray_info[nSend_SIte].st_pcb_info[y][x].strPPID = "";
				st_tray_info[nSend_SIte].st_pcb_info[y][x].strPSID = "";
				st_tray_info[nSend_SIte].st_pcb_info[y][x].strWWN = ""; 

				st_tray_info[nSend_SIte].st_pcb_info[y][x].nYesNo = 0;
				st_tray_info[nSend_SIte].st_pcb_info[y][x].nBin = 0;
				st_tray_info[nSend_SIte].st_pcb_info[y][x].nRetest = 0;
				st_tray_info[nSend_SIte].st_pcb_info[y][x].nBdNum = 0;
				st_tray_info[nSend_SIte].st_pcb_info[y][x].nEnable = 0;
			}
		}
		st_tray_info[nRcv_Site].nNewLotIn = st_tray_info[nSend_SIte].nNewLotIn;
		st_tray_info[nRcv_Site].nLastModule = st_tray_info[nSend_SIte].nLastModule; //������ �������� üũ
		st_tray_info[nRcv_Site].nModuleCnt = st_tray_info[nSend_SIte].nModuleCnt;
		st_tray_info[nRcv_Site].nTrayExist = st_tray_info[nSend_SIte].nTrayExist; //tray ���� ��/�� 
		st_tray_info[nRcv_Site].strLotNo = st_tray_info[nSend_SIte].strLotNo;// lot no......
		st_tray_info[nRcv_Site].strPartNo = st_tray_info[nSend_SIte].strPartNo;
		st_tray_info[nRcv_Site].strProcess = st_tray_info[nSend_SIte].strProcess;	// process id.....

		st_tray_info[nSend_SIte].nNewLotIn = 0;
		st_tray_info[nSend_SIte].nLastModule = 0; //������ �������� üũ
		st_tray_info[nSend_SIte].nModuleCnt = 0;
		st_tray_info[nSend_SIte].nTrayExist = 0;
		st_tray_info[nSend_SIte].strLotNo = "";// lot no......
		st_tray_info[nSend_SIte].strPartNo = "";
		st_tray_info[nSend_SIte].strProcess = "";	// process id.....


		/*if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
		{
			sprintf(st_msg.c_normal_msg, "[CLEAR DATA] SHIFT_IDBUFF_OUTPUTREADY_BUFFER DATA CLEAR");
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
		}*/
	}
	else if(nMode == 2) // ���� �۾��ϴ� lot�� ������ �־��ش�. ����̽� ������ nDvc_Yes_No ������ ���� ������ �Ѵ� 
	{	 
		for(y = 0; y < st_recipe_info.nTrayY; y++)
		{
			for(x=0; x < st_recipe_info.nTrayX; x++)
			{
				st_tray_info[nRcv_Site].st_pcb_info[y][x].strLotNo = st_tray_info[nSend_SIte].strLotNo;// lot no......
				st_tray_info[nRcv_Site].st_pcb_info[y][x].strSerialNo = "";
				st_tray_info[nRcv_Site].st_pcb_info[y][x].strCSerialNo = "";
				st_tray_info[nRcv_Site].st_pcb_info[y][x].strBarcode = "";
				st_tray_info[nRcv_Site].st_pcb_info[y][x].strPartNo =  st_tray_info[nSend_SIte].strPartNo;
				st_tray_info[nRcv_Site].st_pcb_info[y][x].strArrNo = "";
				st_tray_info[nRcv_Site].st_pcb_info[y][x].strPPID = "";
				st_tray_info[nRcv_Site].st_pcb_info[y][x].strPSID = "";
				st_tray_info[nRcv_Site].st_pcb_info[y][x].strWWN = ""; 

				st_tray_info[nRcv_Site].st_pcb_info[y][x].nYesNo = nDvc_Yes_No;
				st_tray_info[nRcv_Site].st_pcb_info[y][x].nBin = 0;
				st_tray_info[nRcv_Site].st_pcb_info[y][x].nRetest = 0;
				st_tray_info[nRcv_Site].st_pcb_info[y][x].nBdNum = 0;
				st_tray_info[nRcv_Site].st_pcb_info[y][x].nEnable = 0; 
			}
		}

		st_tray_info[nRcv_Site].nNewLotIn = st_tray_info[nSend_SIte].nNewLotIn;
		st_tray_info[nRcv_Site].nLastModule = st_tray_info[nSend_SIte].nLastModule; //������ �������� üũ
		st_tray_info[nRcv_Site].nModuleCnt = st_tray_info[nSend_SIte].nModuleCnt;
		st_tray_info[nRcv_Site].nTrayExist = st_tray_info[nSend_SIte].nTrayExist; //tray ���� ��/�� 
		st_tray_info[nRcv_Site].strLotNo = st_tray_info[nSend_SIte].strLotNo;// lot no......
		st_tray_info[nRcv_Site].strPartNo = st_tray_info[nSend_SIte].strPartNo;
		st_tray_info[nRcv_Site].strProcess = st_tray_info[nSend_SIte].strProcess;	// process id.....


		/*if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
		{
			sprintf(st_msg.c_normal_msg, "[CLEAR DATA] SHIFT_IDBUFF_OUTPUTREADY_BUFFER DATA CLEAR");
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
		}*/
	}		 

	else //if(n_mode == 3) // ������ ����Ʈ ������ ��� �ʿ� ���� �Ϸ� �� �� ����Ʈ�� ������ Ŭ���� �Ѵ� 
	{
		//memset(&st_tray_info[nSend_SIte], 0x00, sizeof(st_tray_info[nSend_SIte]) ); //clear 
		for(y = 0; y < st_recipe_info.nTrayY; y++)
		{
			for(x=0; x < st_recipe_info.nTrayX; x++)
			{
				st_tray_info[nSend_SIte].st_pcb_info[y][x].strLotNo = "";
				st_tray_info[nSend_SIte].st_pcb_info[y][x].strSerialNo = "";
				st_tray_info[nSend_SIte].st_pcb_info[y][x].strCSerialNo = "";
				st_tray_info[nSend_SIte].st_pcb_info[y][x].strBarcode = "";
				st_tray_info[nSend_SIte].st_pcb_info[y][x].strPartNo = "";
				st_tray_info[nSend_SIte].st_pcb_info[y][x].strArrNo = "";
				st_tray_info[nSend_SIte].st_pcb_info[y][x].strPPID = "";
				st_tray_info[nSend_SIte].st_pcb_info[y][x].strPSID = "";
				st_tray_info[nSend_SIte].st_pcb_info[y][x].strWWN = ""; 

				st_tray_info[nSend_SIte].st_pcb_info[y][x].nYesNo = 0;
				st_tray_info[nSend_SIte].st_pcb_info[y][x].nBin = 0;
				st_tray_info[nSend_SIte].st_pcb_info[y][x].nRetest = 0;
				st_tray_info[nSend_SIte].st_pcb_info[y][x].nBdNum = 0;
				st_tray_info[nSend_SIte].st_pcb_info[y][x].nEnable = 0;
				//st_tray_info[THD_MACHINE_INPUT_NETWORK_SITE].st_pcb_info[y][x].nScrCodd =5;
			}
		}

		st_tray_info[nSend_SIte].nNewLotIn = 0;
		st_tray_info[nSend_SIte].nLastModule = 0; //������ �������� üũ
		st_tray_info[nSend_SIte].nModuleCnt = 0;
		st_tray_info[nSend_SIte].nTrayExist = 0;
		st_tray_info[nSend_SIte].strLotNo = "";// lot no......
		st_tray_info[nSend_SIte].strPartNo = "";
		st_tray_info[nSend_SIte].strProcess = "";	// process id.....
		

		/*if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
		{
			sprintf(st_msg.c_normal_msg, "[CLEAR DATA] SHIFT_IDBUFF_OUTPUTREADY_BUFFER DATA CLEAR");
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
		}*/
	}		 

	return RET_GOOD;
}

////////////////////////////////////////////////////////////////////////
//Picker �� ���� ��/�� ���¸� üũ�Ѵ� 
//2015.0208 Ȯ��, 
////////////////////////////////////////////////////////////////////////
//james 2016.0827 �׸��� ����̽� üũ ���� ����/����, Gripper Off(Ŭ��������)���� IO_OFF�� ����̽� �ִ»���, �׿ܴ� ON/OFF ������� ��� ����̽� ���� ����
int	CPublicFunction::Check_PickerStatus(int nPickerMode, int nSite, int nYesNo, int *npPickerInfo, int *npPickerStatus, int *npPickerOutputStatus) //james 2016.0827 �׸��� ����̽� üũ ���� ����/����, Gripper Off(Ŭ��������)���� IO_OFF�� ����̽� �ִ»���, �׿ܴ� ON/OFF ������� ��� ����̽� ���� ����
{
	int nFuncRet = RET_GOOD;
	int i, nRet[4]={0,};
	int nIO_Num_Info[MAX_PICKER];
	int nIO_Output_Num_Info[MAX_PICKER];
	int nCurrent_PickerInfo[MAX_PICKER];
	int nPara_Num;
	//int npOutputStatus[MAX_PICKER];
	int dDvcCheck_Mode = 1; // james 2016.1003 �׽�Ʈ �κ� ���� ��� ���� 
	if(nSite == THD_WORK_RBT) 
	{
		nPara_Num = LD_ULD_PICKER_PARA;
		for(i = 0; i < nPara_Num; i++)
		{
			nIO_Num_Info[i] = st_io_info.i_LdUldPickDvcChk;  //
			nIO_Output_Num_Info[i] = st_io_info.o_LdUldPickGripCyl; 
			if(nPickerMode == 0)	 //���������� üũ�� ���� ���� Ȯ�� 
			{
				nCurrent_PickerInfo[i] = st_picker[nSite].st_pcb_info[i].nYesNo;
			}
			else //if(nPickerMode == 1) //�Լ� ����(�������) ���, CTL_YES�� �κи� ��Ũ �������� üũ�Ѵ� 
			{
				nCurrent_PickerInfo[i] = npPickerInfo[i];
			} 
		}
	}
	else if(nSite == THD_TEST_RBT) 
	{
		nPara_Num = TEST_PICKER_PARA;
		for(i = 0; i < nPara_Num; i++)
		{
			nIO_Num_Info[i] = st_io_info.i_TestRbtPickerGripDvcExistChk[i]; 
			nIO_Output_Num_Info[i] = st_io_info.o_TestRbtPickerGripCyl[i]; 
			if(nPickerMode == 0)	 //���������� üũ�� ���� ���� Ȯ��, ��ü ��Ŀ ������ ���� ���¸� üũ�Ѵ�  
			{
				nCurrent_PickerInfo[i] = st_picker[nSite].st_pcb_info[i].nYesNo;
			}
			else //if(nPickerMode == 1) //�Լ� ����(�������) ���,  CTL_YES�� �κи� ��Ũ �������� üũ�Ѵ� 
			{
				nCurrent_PickerInfo[i] = npPickerInfo[i];
			} 
		}  
	}
	else 
	{//�̰����� ���� �ȵ� 
		return RET_ERROR;
	}
	//��ġ �̵� //james 2016.0919 
	for(i = 0; i < nPara_Num; i++)
	{
		//james 2016.0919 ���� 
		npPickerStatus[i] = FAS_IO.get_in_bit(nIO_Num_Info[i],	IO_ON);  
		npPickerOutputStatus[i] = FAS_IO.get_out_bit(nIO_Output_Num_Info[i],	IO_ON); //�׸��� output ���� �߰� 
	}	


	if(st_basic_info.nModeDevice == WITHOUT_DVC) //with device �� �ƴϸ� 
	{
		for(i = 0; i < nPara_Num; i++)
		{
			//npPickerStatus[i] = CTL_NO;
			if(nYesNo == CTL_YES) //����̽��� �ִ� ���¸� üũ�� 
			{			
				if(nSite == THD_WORK_RBT) //james 2016.1003 
				{
					if(nCurrent_PickerInfo[i] == CTL_YES) 
					{
						if(npPickerOutputStatus[i] == IO_OFF)//james 2016.0919 �׸��� ���Ǹ� ���� 
						{
							npPickerStatus[i] = IO_ON;//james 2016.0918 CTL_NO;//clmap�ϰ� �־� ���� 
						}
						else
						{
							npPickerStatus[i] = IO_OFF; //open �ϰ� �ִ� ���·� ������ �־� ����ó�� 
						}
					}
				}
				else if(nSite == THD_TEST_RBT) 
				{
					if(nCurrent_PickerInfo[i] == CTL_YES)
					{
						 npPickerStatus[i] = IO_ON;//james 2016.0918 CTL_NO;//clmap�ϰ� �־� ����  
					}
				}
			}
			else //����̽��� ���� ���¸� üũ�� 
			{
				if(nSite == THD_WORK_RBT) //james 2016.1003 
				{
					if(nCurrent_PickerInfo[i] == CTL_YES) 
					{
						if(npPickerOutputStatus[i] == IO_ON)//james 2016.0919 �׸��� ���� ���� 
						{
							npPickerStatus[i] = IO_ON; // open�ϰ� �־� ���� ó���� 
						}
						else
						{
							npPickerStatus[i] = IO_OFF; //clamp �ϰ� �ִ� ���·� ������ �־� ����ó�� 
						}
					}
				}
				else if(nSite == THD_TEST_RBT) //james 2016.1003
				{
					if(nCurrent_PickerInfo[i] == CTL_YES) 
					{
						npPickerStatus[i] = IO_OFF; //clamp �ϰ� �ִ� ���·� ������ �־� ����ó�� 
					}
				}
			}
		}
		return RET_GOOD;
	}
/* //��ġ �̵� //james 2016.0919 
	for(i = 0; i < nPara_Num; i++)
	{
		//james 2016.0919 ���� 
		npPickerStatus[i] = FAS_IO.get_in_bit(nIO_Num_Info[i],	IO_ON);  
		npPickerOutputStatus[i] = FAS_IO.get_out_bit(nIO_Output_Num_Info[i],	IO_ON); //�׸��� output ���� �߰� 
	}	
*/
	for (i = 0; i < nPara_Num; i++) 
	{
		if(nYesNo == CTL_YES)	//����̽��� �ִ� ���¸� üũ�� �� 
		{
			if(nCurrent_PickerInfo[i] == CTL_YES) //CTL_YES: ��Ŀ ������ ����̽��� �ִ�, CTL_NO:��Ŀ ������ ����̽��� ����
			{			
				// jtkim 20160910
				//if(npPickerStatus[i] != IO_OFF)//CTL_NO) //IO_OFF�� ����̽� �ִ� ����, �׸��� Ŭ�����ø� ����̽� ��/�� üũ���� IO_OFF�� ����̽� �ִ� ����,   
				if(npPickerStatus[i] == IO_OFF || npPickerOutputStatus[i] == IO_ON)//james 2016.0919 CTL_NO)
				{	//error   //���� �ȵ� ���� 					
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, nIO_Num_Info[i]); //on check error 
					nFuncRet = RET_ERROR;
				}
			}
			else //if((nCurrent_PickerInfo[i] == CTL_NO) //CTL_YES: ��Ŀ ������ ����̽��� �ִ�, CTL_NO:��Ŀ ������ ����̽��� ����
			{//����̽��� ����� �Ѵ� 
				if(nPickerMode == 0) //���� ������ ���¸� üũ�ϴ� ���۽� ��� 
				{
					if(nSite == THD_WORK_RBT) //james 2016.1003 
					{
						if(npPickerStatus[i] == IO_ON && npPickerOutputStatus[i] == IO_OFF) //james 2016.0918 �߰� nIO_Output_Num_Info[i] 
						{	//error   //���� �ȵ� ���� 
							//vacuum on/off �� �ٷ� üũ 
							m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, nIO_Num_Info[i]); //on check error 
							nFuncRet = RET_ERROR;
						}
					}
					else if(nSite == THD_TEST_RBT) //james 2016.1003
					{
						if(npPickerStatus[i] == IO_ON || npPickerOutputStatus[i] == IO_OFF) //james 2016.0918 �߰� nIO_Output_Num_Info[i] 
						{	//error   //���� �ȵ� ���� 
							//vacuum on/off �� �ٷ� üũ 
							m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, nIO_Num_Info[i]); //on check error 
							nFuncRet = RET_ERROR;
						}
					}
				}
			}
		}
		else if(nYesNo == CTL_NO)	//������� ����,. ����̽��� ���� ���¸� üũ�� ��,  
		{
			if(nCurrent_PickerInfo[i] == CTL_YES) //CTL_YES: ��Ŀ ������ �۾��ϴ� �����̴�, CTL_NO:��Ŀ ������ �۾����� �ʴ� �����̴�
			{
				if(nSite == THD_WORK_RBT) //james 2016.1003 
				{ 
					if(npPickerStatus[i] == IO_ON && npPickerOutputStatus[i] == IO_OFF) //james 2016.0919 (npPickerStatus[i] == IO_OFF) //james 2016.0918 CTL_NO)
					{	//error   //���� �ȵ� ���� 
						m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, nIO_Num_Info[i]); //on check error 
						nFuncRet = RET_ERROR;
					}
				}
				else if(nSite == THD_TEST_RBT) //james 2016.1003
				{
					if(npPickerStatus[i] == IO_ON) //james 2016.0919 (npPickerStatus[i] == IO_OFF) //james 2016.0918 CTL_NO)
					{	//error   //���� �ȵ� ���� 
						m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, nIO_Num_Info[i]); //on check error 
						nFuncRet = RET_ERROR;
					}
				}
			}
			else //if((nCurrent_PickerInfo[i] == CTL_NO) //CTL_YES: ��Ŀ ������ ����̽��� �ִ�, CTL_NO:��Ŀ ������ ����̽��� ����
			{//����̽��� ����� �Ѵ� 
				if(nPickerMode == 0)//���� ������ ���¸� üũ�ϴ� ���۽� ��� 
				{
					if(nSite == THD_WORK_RBT) //james 2016.1003 
					{ 
						if(npPickerStatus[i] == IO_ON && npPickerOutputStatus[i] == IO_OFF) //james 2016.0919 �߰� nIO_Output_Num_Info[i] 
						{	//error   //���� �ȵ� ���� 
							//vacuum on/off �� �ٷ� üũ 
							m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, nIO_Num_Info[i]); //on check error 
							nFuncRet = RET_ERROR;
						}
					}
					else if(nSite == THD_TEST_RBT) //james 2016.1003
					{
						if(npPickerStatus[i] == IO_ON || npPickerOutputStatus[i] == IO_OFF) //james 2016.0919 �߰� nIO_Output_Num_Info[i] 
						{	//error   //���� �ȵ� ���� 
							//vacuum on/off �� �ٷ� üũ 
							m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, nIO_Num_Info[i]); //on check error 
							nFuncRet = RET_ERROR;
						}
					}
				}
			}
		}
	}

	return nFuncRet;
}

////////////////////////////////////////////////////////////////////////
//������ ���� ��/�� ���¸� üũ�Ѵ� 
//james 2016.0810, 
////////////////////////////////////////////////////////////////////////
//james 2016.0919 ����
int	CPublicFunction::Check_BufferStatus(int nBufferMode, int nSite, int nYesNo, int *nBufferInfo, int *nBufferStatus, int *npBufferOutputStatus) //james 2016.0919 �߰� 
{
	int nFuncRet = RET_GOOD;
	int i, nRet[4]={0,};
	int nIO_Num_Info[MAX_BUF_SIZE];
	int nIO_Output_Num_Info[MAX_BUF_SIZE];

	int nCurrent_BufferInfo[MAX_BUF_SIZE];
	int nPara_Num = MAX_LDULD_BUFF_PARA;

	 
	if(nSite == THD_LD_BUFF) 
	{
		nPara_Num = MAX_LDULD_BUFF_PARA;
		for(i = 0; i < nPara_Num; i++)
		{
			nIO_Num_Info[i] = st_io_info.i_LdBuffGripDvcExistChk[i];  
			nIO_Output_Num_Info[i] = st_io_info.o_LdBuffGripOnOffCyl[i]; 
			if(nBufferMode == 0)	 //���������� üũ�� ���� ������ �������� ���� ���� ���� Ȯ�� 
			{
				nCurrent_BufferInfo[i] = st_buffer_info[nSite].st_pcb_info[i].nYesNo;
			}
			else if(nBufferMode == 1) //�Լ� ����(�������) ����Ͽ� ���� ���� ���� Ȯ��  
			{
				nCurrent_BufferInfo[i] = nBufferInfo[i];
			} 
		}
	}
	else if(nSite == THD_ULD_BUFF) 
	{
		nPara_Num = MAX_LDULD_BUFF_PARA;
		for(i = 0; i < nPara_Num; i++)
		{
			nIO_Num_Info[i] = st_io_info.i_UldBuffGripDvcExistChk[i]; 
			nIO_Output_Num_Info[i] = st_io_info.o_UldBuffGripOnOffCyl[i]; 
			if(nBufferMode == 0)	 //���������� üũ�� ���� ���� Ȯ��, ��ü ��Ŀ ������ ���� ���¸� üũ�Ѵ�  
			{
				nCurrent_BufferInfo[i] = st_buffer_info[nSite].st_pcb_info[i].nYesNo;
			}
			else if(nBufferMode == 1) //�Լ� ����(�������) ���,  CTL_YES�� �κи� ��Ũ �������� üũ�Ѵ� 
			{
				nCurrent_BufferInfo[i] = nBufferInfo[i];
			} 
		}
	}
	else 
	{//�̰����� ���� �ȵ� 
		return RET_ERROR;
	}
	

	for(i = 0; i < nPara_Num; i++)
	{
		nBufferStatus[i] = FAS_IO.get_in_bit(nIO_Num_Info[i],	IO_ON);  
		npBufferOutputStatus[i] = FAS_IO.get_out_bit(nIO_Output_Num_Info[i],	IO_ON); //�׸��� output ���� �߰� 
	}	

	if(st_basic_info.nModeDevice == WITHOUT_DVC) //with device �� �ƴϸ� 
	{
		for(i = 0; i < nPara_Num; i++)
		{
			if(nBufferMode == 100) //������ ���簡 ��� ����� �Ѵ� 
			{
				if((nBufferStatus[i] == IO_ON && npBufferOutputStatus[i] == IO_OFF) || st_buffer_info[nSite].st_pcb_info[i].nYesNo == CTL_YES)
				{  				 
					m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, nIO_Num_Info[i]); //on check error 
					nFuncRet = RET_ERROR;
					return nFuncRet;
				}
				continue;
			}

			if(nYesNo == CTL_YES) //����̽��� �ִ� ���¸� üũ�� 
			{
				// jtkim 20160904
				// if(nCurrent_PickerInfo[i] == CTL_YES) npPickerStatus[i] = CTL_YES;
				if(nCurrent_BufferInfo[i] == CTL_YES)
				{
					if(npBufferOutputStatus[i] == IO_OFF)//james 2016.0919 �׸��� ���Ǹ� ���� 
					{
						nBufferStatus[i] = IO_ON;//james 2016.0918 CTL_NO;//clmap�ϰ� �־� ���� 
					}
					else
					{
						nBufferStatus[i] = IO_OFF; //open �ϰ� �ִ� ���·� ������ �־� ����ó�� 
					}
				}
			}
			else //����̽��� ���� ���¸� üũ�� 
			{
				if(nCurrent_BufferInfo[i] == CTL_YES) 
				{
					if(npBufferOutputStatus[i] == IO_ON)//james 2016.0919 �׸��� ���� ���� 
					{
						nBufferStatus[i] = IO_ON; // open�ϰ� �־� ���� ó���� 
					}
					else
					{
						nBufferStatus[i] = IO_OFF; //clamp �ϰ� �ִ� ���·� ������ �־� ����ó�� 
					}
				}
			}
		}
		return RET_GOOD;
	}

	//with device
	for (i = 0; i < nPara_Num; i++) 
	{
		if(nBufferMode == 100) //������ ���簡 ��� ����� �Ѵ� 
		{
			if((nBufferStatus[i] == IO_ON && npBufferOutputStatus[i] == IO_OFF) || st_buffer_info[nSite].st_pcb_info[i].nYesNo == CTL_YES)
			{  				 
				m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, nIO_Num_Info[i]); //on check error 
				nFuncRet = RET_ERROR;
				return nFuncRet;
			}
			continue;
		}

		if(nYesNo == CTL_YES)	//����̽��� �ִ� ���¸� üũ�� �� 
		{
			if(nCurrent_BufferInfo[i] == CTL_YES) //CTL_YES: ��Ŀ ������ ����̽��� �ִ�, CTL_NO:��Ŀ ������ ����̽��� ����
			{			
				 
				if(nBufferStatus[i] == IO_OFF || npBufferOutputStatus[i] == IO_ON)//james 2016.0919 CTL_NO)
				{	//error   //���� �ȵ� ���� 					
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, nIO_Num_Info[i]); //on check error 
					nFuncRet = RET_ERROR;
				}
			}
			else //if((nCurrent_PickerInfo[i] == CTL_NO) //CTL_YES: ��Ŀ ������ ����̽��� �ִ�, CTL_NO:��Ŀ ������ ����̽��� ����
			{//����̽��� ����� �Ѵ� 
				if(nBufferMode == 0) //���� ������ ���¸� üũ�ϴ� ���۽� ��� 
				{
					if(nBufferStatus[i] == IO_ON && npBufferOutputStatus[i] == IO_OFF) //james 2016.0918 �߰� nIO_Output_Num_Info[i] 
					{	 
						m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, nIO_Num_Info[i]); //on check error 
						nFuncRet = RET_ERROR;
					}
				}
			}
		}
		else if(nYesNo == CTL_NO)	//������� ����,. ����̽��� ���� ���¸� üũ�� ��,  
		{
			if(nCurrent_BufferInfo[i] == CTL_YES) //CTL_YES: ��Ŀ ������ �۾��ϴ� �����̴�, CTL_NO:��Ŀ ������ �۾����� �ʴ� �����̴�
			{ 
				if(nBufferStatus[i] == IO_ON && npBufferOutputStatus[i] == IO_OFF) //james 2016.0919 (npPickerStatus[i] == IO_OFF) //james 2016.0918 CTL_NO)
				{	//error   //���� �ȵ� ���� 
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, nIO_Num_Info[i]); //on check error 
					nFuncRet = RET_ERROR;
				}
			}
			else //if((nCurrent_BufferInfo[i] == CTL_NO) //CTL_YES: ��Ŀ ������ ����̽��� �ִ�, CTL_NO:��Ŀ ������ ����̽��� ����
			{//����̽��� ����� �Ѵ� 
				if(nBufferMode == 0)//���� ������ ���¸� üũ�ϴ� ���۽� ��� 
				{
					if(nBufferStatus[i] == IO_ON && npBufferOutputStatus[i] == IO_OFF) //james 2016.0919 �߰� nIO_Output_Num_Info[i] 
					{	//error   //���� �ȵ� ���� 
						//vacuum on/off �� �ٷ� üũ 
						m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, nIO_Num_Info[i]); //on check error 
						nFuncRet = RET_ERROR;
					}					 
				}
			}
		}
	}  

	return nFuncRet;
}

//2016.0911 james 
int CPublicFunction::Check_Robot_Buffer_Clash(int nTestDirection, int nRbtTHDSite, int TestBuffTHDSite, int nBuffStartPos, int nPickerStartPos, int *nErrorInfoStatus) //��Ŀ�� ������ �۾� ��ġ�� ������ �۾� ���� ���� Ȯ�� �Ѵ�
{//int nBuffStartPos �� �ǹ̴� Buffer, Test site socket�� ��Ŀ���� ���۵��� ���� ��ȣ�̴� 
	int nFuncRet = RET_GOOD;
	int i, nRet[4]={0,};
	int nPara_Num = MAX_SITE_PICKER; //4
	int nPos=0;
	int nFisrtPicker=0;

	/*
	if(nRbtTHDSite == THD_WORK_RBT)
	{
		nPara_Num = LD_ULD_PICKER_PARA;//1
	}
	else if(nRbtTHDSite == THD_TEST_RBT)
	{
		nPara_Num = TEST_PICKER_PARA;//4
	}*/

	for(i = 0; i < nPara_Num; i++) nErrorInfoStatus[i] = CTL_NO; //clear 
	if(nTestDirection == TESTSITE_LEFT) //�κ�Ʈ ��Ŀ ����� , ����/�׽�Ʈ ����Ʈ�� ��ȣ ������ �ݴ��̴�.  //���� �� ��Ŀ�� ȸ�� ������ ���� �ȴ� 
	{				
		if(nRbtTHDSite == THD_WORK_RBT)
		{
			for(i = 0; i < nPara_Num; i++) //nBuffStartPos�� �������� ��ġ��ȣ ū�ʿ� ���簡 ���������� ��Ŀ ���� �����̴� üũ
			{
				nErrorInfoStatus[i] = CTL_NO; //���� ���� Ŭ���� 				

				if(TestBuffTHDSite == THD_LD_BUFF) //���縦 only ���� �۾��� �Ѵ�, �۾� ������ 0->1->2->3 ���� �ڿ��� ���� ���´�
				{					
					if(nBuffStartPos > i) continue;
					if(st_buffer_info[TestBuffTHDSite].st_pcb_info[i].nYesNo == CTL_YES)  
					{						  
						m_strAlarmCode.Format(_T("8%d%04d"), IO_ON,st_io_info.i_LdBuffGripDvcExistChk[i]);//error 
						//���� , ������ ����̽��� ��Ŀ�� ���簡 �浹 ��
						nErrorInfoStatus[i] = CTL_YES; //���� ���� ����
						nFuncRet = RET_ERROR; 
					} 
				}
				else if(TestBuffTHDSite == THD_ULD_BUFF) //���縦 only ���� �۾��� �Ѵ�, �۾� ������ �տ��� ���� ���´� , 3->2->1->0 �̴�
				{
					if(nBuffStartPos >= i) continue; //���� ��ȣ ������ �۾��� �������� ū ��ġ ������ ���� ������ �浹�ϴ� // �����ؾ� �� 
					if(st_buffer_info[TestBuffTHDSite].st_pcb_info[i].nYesNo == CTL_YES)  
					{	//���� , ������ ����̽��� ��Ŀ�� ���簡 �浹 ��
						nErrorInfoStatus[i] = CTL_YES; //���� ���� ����
						m_strAlarmCode.Format(_T("8%d%04d"), IO_ON,st_io_info.i_UldBuffGripDvcExistChk[i]);//error 
						nFuncRet = RET_ERROR; 
					} 
				}
			}
		}
		else if(nRbtTHDSite == THD_TEST_RBT)
		{
			nFisrtPicker=0;
			for(i = 0; i < nPara_Num; i++)
			{
				//if(nBuffStartPos > i) continue; //Ȥ�� �߸� ����Ͽ� �޸� ���� ���� ���� �߻��� �����ϱ� ���� �߰� 		
				nRet[0] = FAS_IO.get_in_bit(st_io_info.i_TestRbtPickerGripDvcExistChk[i],	IO_OFF); //james 2016.1003 
				nErrorInfoStatus[i] = CTL_NO; //���� ���� Ŭ���� 
				if(st_picker[nRbtTHDSite].st_pcb_info[i].nYesNo == CTL_YES || nRet[0] == IO_ON) // ���縦 ������ �ִ�
				{				 					
					if(nFisrtPicker == 0)
					{
						nFisrtPicker = i; ////m_nFirstPicker_Num; //james 2016.0911 //i;
					}
					if(TestBuffTHDSite == THD_LD_BUFF)//nBuffStartPos == 3 �̴� 
					{
						nPos = (nPara_Num - 1) - i; // 3 - i //3,2,1,0 test site left �۾��̸�, test robot�� ��Ŀ ����� ������ ������ �ݴ��̴�. 
						if((MAX_LDULD_BUFF_PARA - 1) < nPos || 0 > nPos) continue; //���� ������� ũ�� ���� 
						if(st_buffer_info[TestBuffTHDSite].st_pcb_info[nPos].nYesNo == CTL_YES) //������ ������ ��Ŀ�� �ݴ� �� , 3->2->1->0 ,  4���� ���� ����
						{
							//���� , ������ ����̽��� ��Ŀ�� ���簡 �浹 ��
							nErrorInfoStatus[i] = CTL_YES; //���� ���� ����
							nFuncRet = RET_ERROR; 
							m_strAlarmCode.Format(_T("8%d%04d"), IO_ON,st_io_info.i_TestRbtPickerGripDvcExistChk[i]);//error 
						} 
					}
					else if(TestBuffTHDSite == THD_ULD_BUFF) //nBuffStartPos == 3 �̴� 
					{
						nPos = (nPara_Num - 1) - i; // 3 - i //3,2,1,0 test site left �۾��̸�, test robot�� ��Ŀ ����� ������ ������ �ݴ��̴�. 
						if((MAX_LDULD_BUFF_PARA - 1) < nPos || 0 > nPos) continue; //���� ������� ũ�� ���� 

						if(st_buffer_info[TestBuffTHDSite].st_pcb_info[nPos].nYesNo == CTL_YES) //������ ������ ��Ŀ�� �ݴ� �� , 3->2->1->0 ,  4���� ���� ����
						{
							//���� , ������ ����̽��� ��Ŀ�� ���簡 �浹 ��
							nErrorInfoStatus[i] = CTL_YES; //���� ���� ����
							nFuncRet = RET_ERROR; 
							m_strAlarmCode.Format(_T("8%d%04d"), IO_ON,st_io_info.i_TestRbtPickerGripDvcExistChk[i]);//error 
						} 
					}
					else if(TestBuffTHDSite == THD_TESTSITE_1 || TestBuffTHDSite == THD_TESTSITE_2 || TestBuffTHDSite == THD_TESTSITE_3 || TestBuffTHDSite == THD_TESTSITE_4)
					{//left test site
						//nPos = (nPara_Num - 1) - i; //  3 - i //3,2,1,0 test site left �۾��̸�, test robot�� ��Ŀ ����� ������ ������ �ݴ��̴�. 
						nPos = ((TEST_SOCKET_PARA - 1) - nBuffStartPos) - i; 
						if((TEST_SOCKET_PARA - 1) < nPos || 0 > nPos) continue; //���� ������� ũ�� ���� 

						if(st_test_site_info[TestBuffTHDSite].st_pcb_info[nPos].nYesNo == CTL_YES) //�׽��� ���� ������ ��Ŀ�� �ݴ� �� , 7 ~ 0  8���� ���� ����(nBuffStartPos ==> 7 �Ǵ� 3�ϰ��� 
						{
							//���� , ������ ����̽��� ��Ŀ�� ���簡 �ߵ� ��
							nErrorInfoStatus[i] = CTL_YES; //���� ���� ����
							nFuncRet = RET_ERROR; 
							m_strAlarmCode.Format(_T("8%d%04d"), IO_ON,st_io_info.i_TestRbtPickerGripDvcExistChk[i]);//error 
						} 
					}	 
					else if(TestBuffTHDSite == THD_RETEST_1_BUFF || TestBuffTHDSite == THD_RETEST_2_BUFF)
					{//left test site					
						 
							//nPos = (nPara_Num - 1) - i; //  3 - i //3,2,1,0 test site left �۾��̸�, test robot�� ��Ŀ ����� ������ ������ �ݴ��̴�. 
						nPos = nBuffStartPos - ((i - nPickerStartPos) * 2); //���׽�Ʈ ������ ������  ((MAX_RETEST_BUFF_PARA - 1) - nBuffStartPos) - (i * 2); //���׽�Ʈ ������ ������ 

						if((MAX_RETEST_BUFF_PARA - 1) < nPos || 0 > nPos) continue; //���� ������� ũ�� ���� 

						if(st_buffer_info[TestBuffTHDSite].st_pcb_info[nPos].nYesNo == CTL_YES) //�׽��� ���� ������ ��Ŀ�� �ݴ� �� , 7 ~ 0  8���� ���� ����(nBuffStartPos ==> 7 �Ǵ� 3�ϰ��� 
						{
							//���� , ������ ����̽��� ��Ŀ�� ���簡 �ߵ� ��
							nErrorInfoStatus[i] = CTL_YES; //���� ���� ����
							nFuncRet = RET_ERROR; 
							m_strAlarmCode.Format(_T("8%d%04d"), IO_ON,st_io_info.i_TestRbtPickerGripDvcExistChk[i]);//error 
						} 
					}
				}
			}//for
		}
		
	}
	else if(nTestDirection == TESTSITE_RIGHT) //�κ�Ʈ ��Ŀ ����� , ����/�׽�Ʈ ����Ʈ�� ��ȣ ������ �����ϴ� 
	{		 			
		if(nRbtTHDSite == THD_WORK_RBT)
		{			
			for(i = 0; i < nPara_Num; i++) //nBuffStartPos�� �������� ��ġ��ȣ ū�ʿ� ���簡 ���������� ��Ŀ ���� �����̴� üũ
			{
				nErrorInfoStatus[i] = CTL_NO; //���� ���� Ŭ���� 				

				if(TestBuffTHDSite == THD_LD_BUFF) //���縦 only ���� �۾��� �Ѵ�, �۾� ������ 0->1->2->3 ���� �ڿ��� ���� ���´�
				{					
					if(nBuffStartPos > i) continue;
					if(st_buffer_info[TestBuffTHDSite].st_pcb_info[i].nYesNo == CTL_YES)  
					{						  
						m_strAlarmCode.Format(_T("8%d%04d"), IO_ON,st_io_info.i_LdBuffGripDvcExistChk[i]);//error 
						//���� , ������ ����̽��� ��Ŀ�� ���簡 �浹 ��
						nErrorInfoStatus[i] = CTL_YES; //���� ���� ����
						nFuncRet = RET_ERROR; 
					} 
				}
				else if(TestBuffTHDSite == THD_ULD_BUFF) //���縦 only ���� �۾��� �Ѵ�, �۾� ������ �տ��� ���� ���´� , 3->2->1->0 �̴�
				{
					if(nBuffStartPos > i) continue; //���� ��ȣ ������ �۾��� �������� ū ��ġ ������ ���� ������ �浹�ϴ� // �����ؾ� �� 
					if(st_buffer_info[TestBuffTHDSite].st_pcb_info[i].nYesNo == CTL_YES)  
					{	//���� , ������ ����̽��� ��Ŀ�� ���簡 �浹 ��
						nErrorInfoStatus[i] = CTL_YES; //���� ���� ����
						m_strAlarmCode.Format(_T("8%d%04d"), IO_ON,st_io_info.i_UldBuffGripDvcExistChk[i]);//error 
						nFuncRet = RET_ERROR; 
					} 
				}
			}			
		}
		else if(nRbtTHDSite == THD_TEST_RBT) //�κ�Ʈ ��Ŀ ����� , ����/�׽�Ʈ ����Ʈ�� ��ȣ ������ �����ϴ� 
		{
			nFisrtPicker=0;
			for(i = 0; i < nPara_Num; i++)
			{
				//if( (nBuffStartPos - i ) < 0) continue; //Ȥ�� �߸� ����Ͽ� �޸� ���� ���� ���� �߻��� �����ϱ� ���� �߰� 
				nRet[0] = FAS_IO.get_in_bit(st_io_info.i_TestRbtPickerGripDvcExistChk[i],	IO_OFF); //james 2016.1003 
				nErrorInfoStatus[i] = CTL_NO; //���� ���� Ŭ���� 
				if(st_picker[nRbtTHDSite].st_pcb_info[i].nYesNo == CTL_YES || nRet[0] == IO_ON) // ���縦 ������ �ִ� //james 2016.1003 �߰�  || nRet[0] == IO_ON
				{				 
					if(nFisrtPicker == 0)
					{
						nFisrtPicker = i;
					}

					if(TestBuffTHDSite == THD_LD_BUFF)//nBuffStartPos == 0 �̴� 
					{
						if(st_buffer_info[TestBuffTHDSite].st_pcb_info[i].nYesNo == CTL_YES) //������ ������ ��Ŀ�� ���� , 0->1->2->3 ,  4���� ���� ����
						{
							//���� , ������ ����̽��� ��Ŀ�� ���簡 �浹 ��
							nErrorInfoStatus[i] = CTL_YES; //���� ���� ����
							nFuncRet = RET_ERROR; 
							//m_strAlarmCode.Format(_T("8%d%04d"), IO_ON,st_io_info.i_TestRbtPickerGripDvcExistChk[i]);//error 
							//kwlee 2017.0126 
							m_strAlarmCode.Format(_T("8%d%04d"), IO_ON,st_io_info.i_LdBuffGripDvcExistChk[i]);//error 
						} 
					}
					else if(TestBuffTHDSite == THD_ULD_BUFF) //nBuffStartPos == 0 �̴� 
					{
						if(st_buffer_info[TestBuffTHDSite].st_pcb_info[i].nYesNo == CTL_YES) //������ ������ ��Ŀ�� ����  , 0->1->2->3  ,  4���� ���� ����
						{
							//���� , ������ ����̽��� ��Ŀ�� ���簡 �浹 ��
							nErrorInfoStatus[i] = CTL_YES; //���� ���� ����
							nFuncRet = RET_ERROR; 
							//m_strAlarmCode.Format(_T("8%d%04d"), IO_ON,st_io_info.i_TestRbtPickerGripDvcExistChk[i]);//error 
							//kwlee 2017.0126
							m_strAlarmCode.Format(_T("8%d%04d"), IO_ON,st_io_info.i_UldBuffGripDvcExistChk[i]);//error 
						} 
					}
					else if(TestBuffTHDSite == THD_TESTSITE_5 || TestBuffTHDSite == THD_TESTSITE_6 || TestBuffTHDSite == THD_TESTSITE_7 || TestBuffTHDSite == THD_TESTSITE_8)
					{//left test site
						if((TEST_SOCKET_PARA - 1) < nBuffStartPos + i || 0 > nBuffStartPos + i) continue; //���� ������� ũ�� ���� 

						if(st_test_site_info[TestBuffTHDSite].st_pcb_info[nBuffStartPos + i].nYesNo == CTL_YES) //�׽��� ���� ������ ��Ŀ�� �ݴ� �� , 7 ~ 0  8���� ���� ����(nBuffStartPos ==> 0 �Ǵ� 4�ϰ��� 
						{
							//���� , ������ ����̽��� ��Ŀ�� ���簡 �ߵ� ��
							nErrorInfoStatus[i] = CTL_YES; //���� ���� ����
							nFuncRet = RET_ERROR; 
							m_strAlarmCode.Format(_T("8%d%04d"), IO_ON,st_io_info.i_TestRbtPickerGripDvcExistChk[i]);//error 
						} 
					}		
					else if(TestBuffTHDSite == THD_RETEST_1_BUFF || TestBuffTHDSite == THD_RETEST_2_BUFF)
					{//left test site
						nPos = nBuffStartPos + ((i - nPickerStartPos) * 2);

						if((MAX_RETEST_BUFF_PARA - 1) < nPos || 0 > nPos) continue; //���� ������� ũ�� ���� 

						if(st_test_site_info[TestBuffTHDSite].st_pcb_info[nPos].nYesNo == CTL_YES) //�׽��� ���� ������ ��Ŀ�� �ݴ� �� , 7 ~ 0  8���� ���� ����(nBuffStartPos ==> 0 �Ǵ� 4�ϰ��� 
						{
							//���� , ������ ����̽��� ��Ŀ�� ���簡 �ߵ� ��
							nErrorInfoStatus[i] = CTL_YES; //���� ���� ����
							nFuncRet = RET_ERROR; 
							m_strAlarmCode.Format(_T("8%d%04d"), IO_ON,st_io_info.i_TestRbtPickerGripDvcExistChk[i]);//error 
						} 
					}		
				}
			}//for
		}
	}

	return nFuncRet;
}





////////////////////////////////////////////////////////////
// �κ�Ʈ X,Y,Z Pos�� ã�´� 
//james 2016.0822 
////////////////////////////////////////////////////////////
int CPublicFunction::Calculate_MovePos_Find(int nMode, int nMotNum, int nSite, int nWorkPart, int *npFindWorkPosYXCPB, double *dpGetTargetPos)
{
	//n_workpos[2]:[0]:x, [1]:y , n_refnum = �۾� ������ ��Ŀ ��ȣ, �۾���ġ ���� ��ȣ   
	double dBasicPos=0, dVarMveGapVal=0, dCompleteMveVal=0; 
	int nX=0, nY=0, workpoint=0;

	

	switch (nMotNum)
	{

	case M_WORK_RBT_Y:
		if(nSite == THD_LD_TRAY_PLATE)
		{
			dBasicPos = st_motor_info[nMotNum].d_pos[P_WORKROBOT_LDTRAY_START];
			dVarMveGapVal = (st_recipe_info.dTrayPitch_Y * npFindWorkPosYXCPB[0]);
			dCompleteMveVal =  (dBasicPos + dVarMveGapVal);		
		}
		else if(nSite == THD_ULD_1_STACKER)
		{
			dBasicPos = st_motor_info[nMotNum].d_pos[P_WORKROBOT_ULDTRAY_1_START];
			dVarMveGapVal = (st_recipe_info.dTrayPitch_Y * npFindWorkPosYXCPB[0]);
			dCompleteMveVal =  (dBasicPos + dVarMveGapVal);		
		}
		else if(nSite == THD_ULD_2_STACKER)
		{
			dBasicPos = st_motor_info[nMotNum].d_pos[P_WORKROBOT_ULDTRAY_2_START];
			dVarMveGapVal = (st_recipe_info.dTrayPitch_Y * npFindWorkPosYXCPB[0]);
			dCompleteMveVal =  (dBasicPos + dVarMveGapVal);		
		}
		else if(nSite == THD_REJECT_OUT_1_STACKER)
		{
			dBasicPos = st_motor_info[nMotNum].d_pos[P_WORKROBOT_REJECT_1_START];
			dVarMveGapVal = (st_recipe_info.dTrayPitch_Y * npFindWorkPosYXCPB[0]);
			dCompleteMveVal =  (dBasicPos + dVarMveGapVal);		
		}
		else if(nSite == THD_REJECT_OUT_2_STACKER)
		{
			dBasicPos = st_motor_info[nMotNum].d_pos[P_WORKROBOT_REJECT_2_START];
			dVarMveGapVal = (st_recipe_info.dTrayPitch_Y * npFindWorkPosYXCPB[0]);
			dCompleteMveVal =  (dBasicPos + dVarMveGapVal);		
		}
		else if(nSite == THD_LD_BUFF)
		{
			dBasicPos = st_motor_info[nMotNum].d_pos[P_WORKROBOT_LD_BUFF_START];
			dVarMveGapVal = (st_recipe_info.dLdUldBufferPitch * npFindWorkPosYXCPB[0]);
			dCompleteMveVal =  (dBasicPos - dVarMveGapVal);		
		}
		else if(nSite == THD_ULD_BUFF)
		{
			dBasicPos = st_motor_info[nMotNum].d_pos[P_WORKROBOT_ULD_BUFF_START];
			dVarMveGapVal = (st_recipe_info.dLdUldBufferPitch * npFindWorkPosYXCPB[0]);
			dCompleteMveVal =  (dBasicPos - dVarMveGapVal);		
		}
		break;

	case M_WORK_RBT_X: //�ε� �κ��� load tray 1 , retest tray ��� ������ �־� �������� ������ �߰� 
		if(nSite == THD_LD_TRAY_PLATE)
		{// 
			dBasicPos = st_motor_info[nMotNum].d_pos[P_WORKROBOT_LDTRAY_START];//load1 start pos�� (st_recipe_info.nTrayX - 2) �� ��ġ�̴� 
			 
			dVarMveGapVal = (st_recipe_info.dTrayPitch_X * npFindWorkPosYXCPB[1]) - (st_recipe_info.dLdUldPickerPitch * npFindWorkPosYXCPB[3]); 			 
			dCompleteMveVal =  (dBasicPos + dVarMveGapVal);		
		}
		else if(nSite == THD_ULD_1_STACKER)
		{
			dBasicPos = st_motor_info[nMotNum].d_pos[P_WORKROBOT_ULDTRAY_1_START];
			dVarMveGapVal = (st_recipe_info.dTrayPitch_X * npFindWorkPosYXCPB[1]) - (st_recipe_info.dLdUldPickerPitch * npFindWorkPosYXCPB[3]); 
			dCompleteMveVal =  (dBasicPos + dVarMveGapVal);		
		}
		else if(nSite == THD_ULD_2_STACKER)
		{
			dBasicPos = st_motor_info[nMotNum].d_pos[P_WORKROBOT_ULDTRAY_2_START];
			dVarMveGapVal = (st_recipe_info.dTrayPitch_X * npFindWorkPosYXCPB[1]) - (st_recipe_info.dLdUldPickerPitch * npFindWorkPosYXCPB[3]); 
			dCompleteMveVal =  (dBasicPos + dVarMveGapVal);		
		}
		else if(nSite == THD_REJECT_OUT_1_STACKER)
		{
			dBasicPos = st_motor_info[nMotNum].d_pos[P_WORKROBOT_REJECT_1_START];
			dVarMveGapVal = (st_recipe_info.dTrayPitch_X * npFindWorkPosYXCPB[1]) - (st_recipe_info.dLdUldPickerPitch * npFindWorkPosYXCPB[3]); 
			dCompleteMveVal =  (dBasicPos + dVarMveGapVal);		
		}
		else if(nSite == THD_REJECT_OUT_2_STACKER)
		{
			dBasicPos = st_motor_info[nMotNum].d_pos[P_WORKROBOT_REJECT_2_START];
			dVarMveGapVal = (st_recipe_info.dTrayPitch_X * npFindWorkPosYXCPB[1]) - (st_recipe_info.dLdUldPickerPitch * npFindWorkPosYXCPB[3]); 
			dCompleteMveVal =  (dBasicPos + dVarMveGapVal);		
		}
		else if(nSite == THD_LD_BUFF)
		{
			dBasicPos = st_motor_info[nMotNum].d_pos[P_WORKROBOT_LD_BUFF_START];
			dVarMveGapVal = 0;//(st_recipe_info.dLdUldBufferPitch * npFindWorkPosYXCPB[1]) - (st_recipe_info.dLdUldPickerPitch * npFindWorkPosYXCPB[3]); 
			dCompleteMveVal =  (dBasicPos + dVarMveGapVal);		
		}
		else if(nSite == THD_ULD_BUFF)
		{
			dBasicPos = st_motor_info[nMotNum].d_pos[P_WORKROBOT_ULD_BUFF_START];
			dVarMveGapVal = 0;//(st_recipe_info.dLdUldBufferPitch * npFindWorkPosYXCPB[1]) - (st_recipe_info.dLdUldPickerPitch * npFindWorkPosYXCPB[3]); 
			dCompleteMveVal =  (dBasicPos + dVarMveGapVal);		
		}
		break;


	case M_TEST_RBT_Y:

		if(nSite == THD_LD_BUFF)
		{	
			dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_LDBUF_START];	
			dVarMveGapVal = 0;
			dCompleteMveVal =  (dBasicPos + dVarMveGapVal);		
		}
		else if(nSite == THD_ULD_BUFF)
		{	
			dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_ULDBUF_START];
			dVarMveGapVal = 0;			 
			dCompleteMveVal =  (dBasicPos + dVarMveGapVal);		
		}
		else if(nSite == THD_RETEST_1_BUFF)
		{	
			dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_RETESTBUF_1_START];
			dVarMveGapVal = 0; //(st_recipe_info.dRetestBufferPitch * npFindWorkPosYXCPB[4]) - (st_recipe_info.dTestPickerPitch * npFindWorkPosYXCPB[3]); 	  
			dCompleteMveVal =  (dBasicPos + dVarMveGapVal);		
		}
		else if(nSite == THD_RETEST_2_BUFF)
		{	
			dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_RETESTBUF_2_START];
			dVarMveGapVal = 0; //(st_recipe_info.dRetestBufferPitch * npFindWorkPosYXCPB[4]) - (st_recipe_info.dTestPickerPitch * npFindWorkPosYXCPB[3]); 	 
			dCompleteMveVal =  (dBasicPos + dVarMveGapVal);		
		}
		else if(nSite >= THD_TESTSITE_1 && nSite <= THD_TESTSITE_8)
		{
			if(nSite == THD_TESTSITE_1)
			{
				if(nWorkPart == 0)//�������� 0�� ��ġ / ���� 
				{
					dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_TESTDUCT_1_LEFT_START];
				}
				else if(nWorkPart == 4) //�������� 4�� ��ġ / ���� 
				{
					dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_TESTDUCT_1_RIGHT_START];
				}
			} 
			else if(nSite == THD_TESTSITE_2)
			{
				if(nWorkPart == 0)//�������� 0�� ��ġ / ���� 
				{
					dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_TESTDUCT_2_LEFT_START];
				}
				else if(nWorkPart == 4) //�������� 4�� ��ġ / ���� 
				{
					dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_TESTDUCT_2_RIGHT_START];
				}
			} 
			else if(nSite == THD_TESTSITE_3)
			{
				if(nWorkPart == 0)//�������� 0�� ��ġ / ���� 
				{
					dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_TESTDUCT_3_LEFT_START];
				}
				else if(nWorkPart == 4) //�������� 4�� ��ġ / ���� 
				{
					dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_TESTDUCT_3_RIGHT_START];
				}
			} 
			else if(nSite == THD_TESTSITE_4)
			{
				if(nWorkPart == 0)//�������� 0�� ��ġ / ���� 
				{
					dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_TESTDUCT_4_LEFT_START];
				}
				else if(nWorkPart == 4) //�������� 4�� ��ġ / ���� 
				{
					dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_TESTDUCT_4_RIGHT_START];
				}
			} 
			else if(nSite == THD_TESTSITE_5)
			{
				if(nWorkPart == 0)//�������� 0�� ��ġ / ���� 
				{
					dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_TESTDUCT_5_LEFT_START];
				}
				else if(nWorkPart == 4) //�������� 4�� ��ġ / ���� 
				{
					dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_TESTDUCT_5_RIGHT_START];
				}
			} 
			else if(nSite == THD_TESTSITE_6)
			{
				if(nWorkPart == 0)//�������� 0�� ��ġ / ���� 
				{
					dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_TESTDUCT_6_LEFT_START];
				}
				else if(nWorkPart == 4) //�������� 4�� ��ġ / ���� 
				{
					dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_TESTDUCT_6_RIGHT_START];
				}
			} 
			else if(nSite == THD_TESTSITE_7)
			{
				if(nWorkPart == 0)//�������� 0�� ��ġ / ���� 
				{
					dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_TESTDUCT_7_LEFT_START];
				}
				else if(nWorkPart == 4) //�������� 4�� ��ġ / ���� 
				{
					dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_TESTDUCT_7_RIGHT_START];
				}
			} 
			else if(nSite == THD_TESTSITE_8)
			{
				if(nWorkPart == 0)//�������� 0�� ��ġ / ���� 
				{
					dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_TESTDUCT_8_LEFT_START];
				}
				else if(nWorkPart == 4) //�������� 4�� ��ġ / ���� 
				{
					dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_TESTDUCT_8_RIGHT_START];
				}
			} 
			dVarMveGapVal = 0;	 
			dCompleteMveVal =  (dBasicPos + dVarMveGapVal);		 
		}
		break;

	case M_TEST_RBT_X:

		if(nSite == THD_LD_BUFF)
		{	
			dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_LDBUF_START];	
			dVarMveGapVal = 0;
			dCompleteMveVal =  (dBasicPos + dVarMveGapVal);		
		}
		else if(nSite == THD_ULD_BUFF)
		{	
			dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_ULDBUF_START];
			dVarMveGapVal = 0;			 
			dCompleteMveVal =  (dBasicPos + dVarMveGapVal);		
		}
		else if(nSite == THD_RETEST_1_BUFF)
		{	
			dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_RETESTBUF_1_START];

			if(nWorkPart == TESTSITE_LEFT)
			{
				dVarMveGapVal = (st_recipe_info.dRetestBufferPitch * (14 - npFindWorkPosYXCPB[4])) - (st_recipe_info.dTestPickerPitch * npFindWorkPosYXCPB[3]); 	  
			}
			else if(nWorkPart == TESTSITE_RIGHT)
			{
				dVarMveGapVal = (st_recipe_info.dRetestBufferPitch * npFindWorkPosYXCPB[4]) - (st_recipe_info.dTestPickerPitch * npFindWorkPosYXCPB[3]); 	  
			}
			//dVarMveGapVal = (st_recipe_info.dRetestBufferPitch * npFindWorkPosYXCPB[4]) - (st_recipe_info.dTestPickerPitch * npFindWorkPosYXCPB[3]); 	  
			dCompleteMveVal =  (dBasicPos + dVarMveGapVal);		
		}
		else if(nSite == THD_RETEST_2_BUFF)
		{	
			dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_RETESTBUF_2_START];
			if(nWorkPart == TESTSITE_LEFT)
			{
				//james 2016.0929 dVarMveGapVal = (st_recipe_info.dRetestBufferPitch * (npFindWorkPosYXCPB[4] - 14)) - (st_recipe_info.dTestPickerPitch * npFindWorkPosYXCPB[3]); 	  
				dVarMveGapVal = (st_recipe_info.dRetestBufferPitch * (14 - npFindWorkPosYXCPB[4])) - (st_recipe_info.dTestPickerPitch * npFindWorkPosYXCPB[3]); 
			}
			else if(nWorkPart == TESTSITE_RIGHT)
			{
				//james 2016.0929 dVarMveGapVal = (st_recipe_info.dRetestBufferPitch * npFindWorkPosYXCPB[4]) - (st_recipe_info.dTestPickerPitch * npFindWorkPosYXCPB[3]); 	  
				dVarMveGapVal = (st_recipe_info.dRetestBufferPitch * npFindWorkPosYXCPB[4]) - (st_recipe_info.dTestPickerPitch * npFindWorkPosYXCPB[3]); 
			}
			//dVarMveGapVal = (st_recipe_info.dRetestBufferPitch * npFindWorkPosYXCPB[4]) - (st_recipe_info.dTestPickerPitch * npFindWorkPosYXCPB[3]); 	  
			dCompleteMveVal =  (dBasicPos + dVarMveGapVal);		
		}
		else if(nSite >= THD_TESTSITE_1 && nSite <= THD_TESTSITE_8)
		{
			if(nSite == THD_TESTSITE_1)
			{
				if(nWorkPart == 0)//�������� 0�� ��ġ / ���� 
				{
					dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_TESTDUCT_1_LEFT_START];
				}
				else if(nWorkPart == 4) //�������� 4�� ��ġ / ���� 
				{
					dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_TESTDUCT_1_RIGHT_START];
				}
			} 
			else if(nSite == THD_TESTSITE_2)
			{
				if(nWorkPart == 0)//�������� 0�� ��ġ / ���� 
				{
					dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_TESTDUCT_2_LEFT_START];
				}
				else if(nWorkPart == 4) //�������� 4�� ��ġ / ���� 
				{
					dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_TESTDUCT_2_RIGHT_START];
				}
			} 
			else if(nSite == THD_TESTSITE_3)
			{
				if(nWorkPart == 0)//�������� 0�� ��ġ / ���� 
				{
					dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_TESTDUCT_3_LEFT_START];
				}
				else if(nWorkPart == 4) //�������� 4�� ��ġ / ���� 
				{
					dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_TESTDUCT_3_RIGHT_START];
				}
			} 
			else if(nSite == THD_TESTSITE_4)
			{
				if(nWorkPart == 0)//�������� 0�� ��ġ / ���� 
				{
					dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_TESTDUCT_4_LEFT_START];
				}
				else if(nWorkPart == 4) //�������� 4�� ��ġ / ���� 
				{
					dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_TESTDUCT_4_RIGHT_START];
				}
			} 
			else if(nSite == THD_TESTSITE_5)
			{
				if(nWorkPart == 0)//�������� 0�� ��ġ / ���� 
				{
					dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_TESTDUCT_5_LEFT_START];
				}
				else if(nWorkPart == 4) //�������� 4�� ��ġ / ���� 
				{
					dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_TESTDUCT_5_RIGHT_START];
				}
			} 
			else if(nSite == THD_TESTSITE_6)
			{
				if(nWorkPart == 0)//�������� 0�� ��ġ / ���� 
				{
					dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_TESTDUCT_6_LEFT_START];
				}
				else if(nWorkPart == 4) //�������� 4�� ��ġ / ���� 
				{
					dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_TESTDUCT_6_RIGHT_START];
				}
			} 
			else if(nSite == THD_TESTSITE_7)
			{
				if(nWorkPart == 0)//�������� 0�� ��ġ / ���� 
				{
					dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_TESTDUCT_7_LEFT_START];
				}
				else if(nWorkPart == 4) //�������� 4�� ��ġ / ���� 
				{
					dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_TESTDUCT_7_RIGHT_START];
				}
			} 
			else if(nSite == THD_TESTSITE_8)
			{
				if(nWorkPart == 0)//�������� 0�� ��ġ / ���� 
				{
					dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_TESTDUCT_8_LEFT_START];
				}
				else if(nWorkPart == 4) //�������� 4�� ��ġ / ���� 
				{
					dBasicPos = st_motor_info[nMotNum].d_pos[P_TESTRBT_XY_TESTDUCT_8_RIGHT_START];
				}
			} 
			dVarMveGapVal = 0;	 
			dCompleteMveVal =  (dBasicPos + dVarMveGapVal);		 
		}
		  
		break;
	}

	//limit �� ���� ���� 
	if(dCompleteMveVal < st_motor_info[nMotNum].d_limit_position[0]) //- LIMIT 
	{//000004
		//000010 0 A "MOTOR SOFTWARE MINUS LIMIT CHECK ERROR -[M_ROBOT_X]."
		//000011 0 A "MOTOR SOFTWARE PULS LIMIT CHECK ERROR -[M_ROBOT_X]."
		m_strAlarmCode.Format(_T("%02d0010"), nMotNum);
		return RET_ERROR;
	}	

	if(dCompleteMveVal > st_motor_info[nMotNum].d_limit_position[1]) //+ LIMIT 
	{
		m_strAlarmCode.Format(_T("%02d0011"), nMotNum);
		return RET_ERROR;
	} 

	*dpGetTargetPos = dCompleteMveVal;  

	return RET_GOOD;
}


///////////////////////////////////////////////////////////////////////
//2016.0804  Ȯ��
////////////////////////////////////////////////////////////////////////
int CPublicFunction::Data_Exchange_PickPlace(int nPickPlace, int nJobCnt, int nRobotSite, int nWorkSite, int *npWorkPos)//Pick & Place�� picker & Tray data exchange �۾�
{
	//int nJobCnt =�ѹ��� ó���� data �۾�����, int nRobotSite//�κ�Ʈ����Ʈ����, int nWorkSite//Ʈ���̻���Ʈ����, int *npRobotPos//picker����, int *npWorkPos//Ʈ���� ����
	//nPickPlace = 0:pick mode, 1:place mode
	CString strTemp;
	if(nPickPlace == PICKER_PICK_MODE) //Ʈ���̼� ���縦 ���� ���� //TRAY -> robot pick;
	{
		if(nRobotSite == THD_WORK_RBT)
		{
			if(nWorkSite == THD_LD_TRAY_PLATE) //Ʈ���̿��� ���� ���� ����
			{//��Ŀ�� ������ ������ ����, Ʈ���̴� ������ Ŭ���� �ȴ� 		

			}			
			else if(nWorkSite == THD_LD_BUFF) //Ʈ���̿��� ���� ���� ����
			{//��Ŀ�� ������ ������ ����, Ʈ���̴� ������ Ŭ���� �ȴ� 

			}
			else if(nWorkSite == THD_ULD_BUFF) //Ʈ���̿��� ���� ���� ����
			{//��Ŀ�� ������ ������ ����, Ʈ���̴� ������ Ŭ���� �ȴ� 

			}

			///////////////////////////////	 
			//data exchange
			//////////////////////////////
			if(nWorkSite == THD_LD_TRAY_PLATE) //|| nWorkSite == THD_LD_BUFF || nWorkSite == THD_ULD_BUFF  || nWorkSite == THD_REJECT_OUT_1_STACKER || nWorkSite == THD_REJECT_OUT_2_STACKER)
			{
				//test site�� ��� st_picker[nRobotSite].st_pcb_info[npRobotPos[0]].nEnable = st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nEnable;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nYesNo				= st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nYesNo;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBin				= st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nBin;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestCnt			= st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nRetestCnt;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetest				= st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nRetest;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBinNum				= st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nBinNum;
				//st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestSite[st_picker[nRobotSite].st_pcb_info[npRobotPos[0]].nRetestCnt]		= st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nRetestSite[st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nRetestCnt];
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBdNum				= st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nBdNum ;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nScrCode			= st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nScrCode;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strLotNo			= st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strLotNo;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strSerialNo			= st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strSerialNo;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode			= st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strBarcode;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPartNo			= st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strPartNo;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strArrNo			= st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strArrNo;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPPID				= st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strPPID;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPSID				= st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strPSID;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strWWN				= st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strWWN;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strCSerialNo		= st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strCSerialNo;
				//test site�� ��� st_picker[nRobotSite].st_pcb_info[npRobotPos[0]].tStart = st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].tStart;			

				// jtkim 201609003
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode1D[0]		= st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strBarcode1D[0];
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode1D[1]		= st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strBarcode1D[1];
				// jtkim 20160929
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode2D[0]		= st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strBarcode2D[0];
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode2D[1]		= st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strBarcode2D[1];
				
				//2016.1031
				if( st_handler_info.bLoaderOnFlag == CTL_YES )
				{
					st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nYesNo = CTL_NO;
					st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBin = -1;
					st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestCnt = 0;
					st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetest = 0;
					st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBinNum = -1;//����� ��� 
					st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestSite[st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nRetestCnt] = -1;
					st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBdNum  = -1;
					st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nScrCode = -1;
					st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strLotNo = _T("");
					st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strSerialNo = _T("");
					st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode = _T("");
					st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPartNo = _T("");
					st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strArrNo = _T("");
					st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPPID = _T("");
					st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPSID = _T("");
					st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strWWN = _T("");
					st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strCSerialNo = _T("");

					st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode1D[0]	= _T("");
					st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode1D[1]	= _T("");
					// jtkim 20160929
					st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode2D[0]	= _T("");
					st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode2D[1]	= _T("");
				}
				
				//////////////////////
				//data clear
				/////////////////////////
				//test site�� ��� st_picker[nRobotSite].st_pcb_info[npRobotPos[0]].nEnable = st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nEnable;
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nYesNo = CTL_NO;
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nBin = -1;
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nRetestCnt = 0;
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nRetest = 0;
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nBinNum = -1;//����� ��� 
				//st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nRetestSite[st_picker[nRobotSite].st_pcb_info[npRobotPos[0]].nRetestCnt] = -1;
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nBdNum  = -1;
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nScrCode = -1;
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strLotNo = _T("");
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strSerialNo = _T("");
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strBarcode = _T("");
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strPartNo = _T("");
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strArrNo = _T("");
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strPPID = _T("");
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strPSID = _T("");
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strWWN = _T("");
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strCSerialNo = _T("");
				//test site�� ��� st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].tStart;	
				// jtkim 20160903
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strBarcode1D[0] = _T("");
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strBarcode1D[1] = _T("");
				// jtkim 20160929
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strBarcode2D[0]	= _T("");
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strBarcode2D[1]	= _T("");
			}
			else if(nWorkSite ==THD_ULD_BUFF )
			{
				///test site�� ��� st_picker[nRobotSite].st_pcb_info[npRobotPos[0]].nEnable = st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nEnable;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nYesNo				= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nYesNo;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBin				= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nBin;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestCnt			= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nRetestCnt;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetest			    = st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nRetest;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBinNum				= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nBinNum;
				//st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestSite[st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestCnt]		= st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nRetestSite[st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nRetestCnt];
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBdNum				= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nBdNum ;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nScrCode			= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nScrCode;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strLotNo			= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strLotNo;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strSerialNo			= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strSerialNo;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode			= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strBarcode;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPartNo			= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strPartNo;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strArrNo			= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strArrNo;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPPID				= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strPPID;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPSID				= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strPSID;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strWWN				= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strWWN;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strCSerialNo		= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strCSerialNo;
				//test site�� ��� st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].tStart = st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].tStart;			
				// jtkim 20160903
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode1D[0]		= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strBarcode1D[0];
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode1D[1]		= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strBarcode1D[1];
				// jtkim 20160929
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode2D[0]		= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strBarcode2D[0];
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode2D[1]		= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strBarcode2D[1];
				//////////////////////
				//data clear
				/////////////////////////
				//test site�� ��� st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nEnable = st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nEnable;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nYesNo = CTL_NO;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nBin = -1;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nRetestCnt = 0;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nRetest = 0;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nBinNum = -1;//����� ��� 
				//st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nRetestSite[st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestCnt] = -1;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nBdNum  = -1;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nScrCode = -1;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strLotNo = _T("");
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strSerialNo = _T("");
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strBarcode = _T("");
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strPartNo = _T("");
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strArrNo = _T("");
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strPPID = _T("");
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strPSID = _T("");
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strWWN = _T("");
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strCSerialNo = _T("");

				// jtim 201660903
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strBarcode1D[0] = _T("");
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strBarcode1D[1] = _T("");
				// jtkim 20160929
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strBarcode2D[0]	= _T("");
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strBarcode2D[1]	= _T("");
				//test site�� ��� st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].tStart;	
			}
		}  //work robot �۾��Ϸ�
		else if(nRobotSite == THD_TEST_RBT) //PICKER_PICK_MODE
		{
			if(nWorkSite == THD_LD_BUFF) //�׽�Ʈ ���Ͽ��� ���� ���� ����
			{//��Ŀ�� ������ ������ ����, �׽�Ʈ ������ ������ Ŭ���� �ȴ� 		

			}			
			else if(nWorkSite == THD_RETEST_1_BUFF) //�׽�Ʈ ���Ͽ��� ���� ���� ����
			{//��Ŀ�� ������ ������ ����, �׽�Ʈ ������ ������ Ŭ���� �ȴ� 		

			}
			else if(nWorkSite == THD_RETEST_2_BUFF) //�׽�Ʈ ���Ͽ��� ���� ���� ����
			{//��Ŀ�� ������ ������ ����, �׽�Ʈ ������ ������ Ŭ���� �ȴ� 		

			}
			else if(nWorkSite >= THD_TESTSITE_1 && nWorkSite <= THD_TESTSITE_8) //�׽�Ʈ ���Ͽ��� ���� ���� ����
			{//��Ŀ�� ������ ������ ����, �׽�Ʈ ������ ������ Ŭ���� �ȴ� 		

			}

			///////////////////////////////	 
			//data exchange
			//////////////////////////////
			if(nWorkSite == THD_LD_BUFF || nWorkSite == THD_RETEST_1_BUFF || nWorkSite == THD_RETEST_2_BUFF)
			{
				//test site�� ��� st_picker[nRobotSite].st_pcb_info[npWorkPos[0]].nEnable = st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nEnable;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nYesNo				= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nYesNo;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBin				= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nBin;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestCnt			= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nRetestCnt;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetest				= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nRetest;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBinNum				= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nBinNum;//����� ��� 
				//st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestSite[st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestCnt]		= st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nRetestSite[st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nRetestCnt];
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBdNum				= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nBdNum ;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nScrCode			= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nScrCode;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strLotNo			= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strLotNo;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strSerialNo			= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strSerialNo;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode			= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strBarcode;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPartNo			= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strPartNo;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strArrNo			= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strArrNo;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPPID				= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strPPID;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPSID				= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strPSID;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strWWN				= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strWWN;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strCSerialNo		= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strCSerialNo;

				// jtkim 20160903
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode1D[0]		= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strBarcode1D[0];
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode1D[1]		= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strBarcode1D[1];
				// jtkim 20160929
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode2D[0]		= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strBarcode2D[0];
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode2D[1]		= st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strBarcode2D[1];
				//test site�� ��� st_picker[nRobotSite].st_pcb_info[npRobotPos[3]].tStart = st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].tStart;			

				//////////////////////
				//data clear
				/////////////////////////
				//test site�� ��� st_picker[nRobotSite].st_pcb_info[npRobotPos[3]].nEnable = st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nEnable;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nYesNo = CTL_NO;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nBin = -1;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nRetestCnt = 0;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nRetest = 0;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nBinNum = -1;//����� ��� 
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nRetestCnt = -1;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nBdNum  = -1;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nScrCode = -1;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strLotNo = _T("");
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strSerialNo = _T("");
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strBarcode = _T("");
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strPartNo = _T("");
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strArrNo = _T("");
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strPPID = _T("");
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strPSID = _T("");
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strWWN = _T("");
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strCSerialNo = _T("");

				// jtkim 201609003
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strBarcode1D[0]	= _T("");
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strBarcode1D[1]	= _T("");
				// jtkim 20160929
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strBarcode2D[0]	= _T("");
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strBarcode2D[1]	= _T("");
				//test site�� ��� st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].tStart;	
			}
			else if(nWorkSite >= THD_TESTSITE_1 && nWorkSite <= THD_TESTSITE_8) //�׽�Ʈ ���Ͽ��� ���� ���� ����
			{//��Ŀ�� ������ ������ ����, �׽�Ʈ ������ ������ Ŭ���� �ȴ� 		
				 
			 
				//test site�� ��� st_picker[nRobotSite].st_pcb_info[npRobotPos[0]].nEnable = st_test_site_info[nWorkSite][npWorkPos[0]].st_pcb_info[npWorkPos[1]].nEnable;
				
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nYesNo		= st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].nYesNo;

				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBin		= st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].nBin;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestCnt  = st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].nRetestCnt;
				//kwlee 2017.0203
				if( st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBin == BD_DATA_CONTINUE_FAIL )
				{
					if (st_handler_info.cWndList != NULL)
					{
						strTemp.Format(_T("[Data_Exchange] TestNum:%d Picker: %d: CONTINUE_FAIL->DATA_RETEST "),(nWorkSite - 18) +1, npWorkPos[3] +1);
						clsMem.OnNormalMessageWrite(strTemp);
					}
					st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBin = BD_DATA_RETEST;
				}
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetest     = st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].nRetest;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBinNum		= st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].nBinNum;//����� ��� 
				//st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestSite[st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestCnt] = st_test_site_info[nWorkSite - THD_LEFT_TEST_SITE][npWorkPos[0]].st_pcb_info[npWorkPos[1]].nRetestSite[st_test_site_info[nWorkSite][npWorkPos[0]].st_pcb_info[npWorkPos[1]].nRetestCnt];
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBdNum        = st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].nBdNum ;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nScrCode      = st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].nScrCode;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strLotNo      = st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strLotNo;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strSerialNo    = st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strSerialNo;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode    = st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strBarcode;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPartNo     = st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strPartNo;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strArrNo     = st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strArrNo;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPPID      = st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strPPID;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPSID      = st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strPSID;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strWWN       = st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strWWN;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strCSerialNo  = st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strCSerialNo;

				// jtkim 20160903
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode1D[0]  = st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strBarcode1D[0];
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode1D[1]  = st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strBarcode1D[1];
				// jtkim 20160929
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode2D[0]	 = st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strBarcode2D[0];
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode2D[1]	 = st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strBarcode2D[1];
				//test site�� ��� st_picker[nRobotSite].st_pcb_info[npRobotPos[0]].tStart = st_test_site_info[nWorkSite][npWorkPos[0]].st_pcb_info[npWorkPos[1]].tStart;			
				 
				//////////////////////
				//data clear
				/////////////////////////
				//test site�� ��� st_test_site_info[nWorkSite][npWorkPos[0]].st_pcb_info[npWorkPos[1]].nEnable =  ;
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].nTestBdStart = BD_NONE; //2015.0313 �߰�
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].nYesNo = CTL_NO;
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].nBin = -1;
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].nRetestCnt = 0; //kwlee 2016.1104 test
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].nRetest = 0;
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].nBinNum = -1;//����� ��� 
				//st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].nRetestSite[st_picker[nRobotSite].st_pcb_info[npWorkPos[1]].nRetestCnt] = -1; //kwlee 2017.0117 ���α׷� �׾ ���� ����.
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].nBdNum  = -1;
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].nScrCode = -1;
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strLotNo = _T("");
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strSerialNo = _T("");
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strBarcode = _T("");
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strPartNo = _T("");
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strArrNo = _T("");
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strPPID = _T("");
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strPSID = _T("");
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strWWN = _T("");
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strCSerialNo = _T("");
				 
				// jtkim 20160903
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strBarcode1D[0]	= _T("");
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strBarcode1D[1]	= _T("");
				// jtkim 20160929
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strBarcode2D[0]	= _T("");
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strBarcode2D[1]	= _T("");
				/* james 2016.0804 
				// jtkim 20150424
				if (nWorkSite == THD_LEFT_TEST_SITE)
				{
					clsInterfaceC.m_nBdInfo[0][npWorkPos[0]][npWorkPos[1]] = NO;
				}
				else
				{
					clsInterfaceC.m_nBdInfo[1][npWorkPos[0]][npWorkPos[1]] = NO;
				}//*/
				
				//test site�� ��� st_test_site_info[nWorkSite][npWorkPos[0]].st_pcb_info[npWorkPos[1]].tStart;	
			}
		}
	}
	else if(nPickPlace == PICKER_PLACE_MODE) //���縦 ���� ���۽� ��� , ��Ŀ���� Ʈ���̿� ���´�  //robot pick -> TRAY 
	{
		if(nRobotSite == THD_WORK_RBT)
		{
			if(nWorkSite == THD_LD_BUFF) //�ε� ���ۿ� ���縦 ���´� 
			{				 	
			}
			else if(nWorkSite == THD_ULD_1_STACKER) //���縦 ���´� 
			{				 	
			}
			else if(nWorkSite == THD_ULD_2_STACKER) //���縦 ���´� 
			{				 	
			}
			///////////////////////////////	 
			//data exchange
			//////////////////////////////
			if(nWorkSite == THD_LD_BUFF) // PICKER_PLACE_MODE
			{
				//test site�� ��� st_picker[nRobotSite].st_pcb_info[npRobotPos[0]].nEnable = st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nEnable;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nYesNo			= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nYesNo;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nBin			= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBin;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nRetestCnt		= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestCnt;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nRetest			= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetest;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nBinNum			= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBinNum;//����� ��� 
				//st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nRetestSite[st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestCnt] = st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestSite[st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestCnt];
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nBdNum			= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBdNum ;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nScrCode		= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nScrCode;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strLotNo		= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strLotNo;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strSerialNo		= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strSerialNo;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strBarcode		= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strPartNo		= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPartNo;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strArrNo		= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strArrNo;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strPPID			= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPPID;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strPSID			= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPSID;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strWWN			= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strWWN;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strCSerialNo	= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strCSerialNo;

				// jtkim 20160903
				//ybs 2016.1006
// 				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strBarcode1D[0]	= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode1D[0];
// 				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strBarcode1D[1]	= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode1D[1];
				// jtkim 20160929
				//ybs 2016.1006
// 				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strBarcode2D[0]	= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode2D[0];
// 				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strBarcode2D[1]	= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode2D[1];
				//test site�� ��� st_picker[nRobotSite].st_pcb_info[npRobotPos[0]].tStart = st_picker[nRobotSite].st_pcb_info[npRobotPos[0]].tStart;			

				//////////////////////
				//data clear
				/////////////////////////
				//test site�� ��� st_picker[nRobotSite].st_pcb_info[npRobotPos[0]].nEnable = st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nEnable;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nYesNo = CTL_NO;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBin = -1;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestCnt = 0;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetest = 0;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBinNum = -1;//����� ��� 
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestSite[st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nRetestCnt] = -1;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBdNum  = -1;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nScrCode = -1;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strLotNo = _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strSerialNo = _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode = _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPartNo = _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strArrNo = _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPPID = _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPSID = _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strWWN = _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strCSerialNo = _T("");

				// jtkim 20160903
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode1D[0]	= _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode1D[1]	= _T("");
				// jtkim 20160929
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode2D[0]	= _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode2D[1]	= _T("");
				//test site�� ��� st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].tStart;	
			}
			else if(nWorkSite == THD_ULD_1_STACKER || nWorkSite == THD_ULD_2_STACKER || nWorkSite == THD_REJECT_OUT_1_STACKER || nWorkSite == THD_REJECT_OUT_2_STACKER)
			{
				//test site�� ��� st_picker[nRobotSite].st_pcb_info[npRobotPos[0]].nEnable = st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nEnable;
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nYesNo			= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nYesNo;
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nBin			= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBin;
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nRetestCnt		= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestCnt;
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nRetest			= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetest;
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nBinNum			= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBinNum;//����� ��� 
				//st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nRetestSite[st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestCnt] = st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestSite[st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestCnt];
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nBdNum			= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBdNum ;
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nScrCode		= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nScrCode;
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strLotNo		= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strLotNo;
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strSerialNo		= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strSerialNo;
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strBarcode		= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode;
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strPartNo		= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPartNo;
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strArrNo		= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strArrNo;
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strPPID			= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPPID;
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strPSID			= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPSID;
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strWWN			= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strWWN;
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strCSerialNo	= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strCSerialNo;

				// jtkim 20160903
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strBarcode1D[0]	= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode1D[0];
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strBarcode1D[1]	= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode1D[1];
				// jtkim 20160929
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strBarcode2D[0]	= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode2D[0];
				st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].strBarcode2D[1]	= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode2D[1];
				//test site�� ��� st_picker[nRobotSite].st_pcb_info[npRobotPos[0]].tStart = st_picker[nRobotSite].st_pcb_info[npRobotPos[0]].tStart;			

				//////////////////////
				//data clear
				/////////////////////////
				//test site�� ��� st_picker[nRobotSite].st_pcb_info[npRobotPos[0]].nEnable = st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nEnable;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nYesNo = CTL_NO;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBin = -1;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestCnt = 0;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetest = 0;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBinNum = -1;//����� ��� 
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestSite[st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nRetestCnt] = -1;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBdNum  = -1;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nScrCode = -1;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strLotNo = _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strSerialNo = _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode = _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPartNo = _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strArrNo = _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPPID = _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPSID = _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strWWN = _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strCSerialNo = _T("");

				// jtkim 20160903
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode1D[0]	= _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode1D[1]	= _T("");
				// jtkim 20160929
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode2D[0]	= _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode2D[1]	= _T("");
				//test site�� ��� st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].tStart;	
			}
		}
		else if(nRobotSite == THD_TEST_RBT)
		{
			if(nWorkSite == THD_LD_BUFF) //Ʈ���̿� ���� ���� ����
			{//��Ŀ�� ������ Ŭ����ǰ�, Ʈ���̴� ������ ������  	
			}
			else if(nWorkSite == THD_ULD_BUFF) //Ʈ���̿� ���� ���� ����
			{//��Ŀ�� ������ Ŭ����ǰ�, Ʈ���̴� ������ ������  
			}
			else if(nWorkSite == THD_RETEST_1_BUFF) //Ʈ���̿� ���� ���� ����
			{//��Ŀ�� ������ Ŭ����ǰ�, Ʈ���̴� ������ ������  
			}
			else if(nWorkSite == THD_RETEST_2_BUFF) //Ʈ���̿� ���� ���� ����
			{//��Ŀ�� ������ Ŭ����ǰ�, Ʈ���̴� ������ ������  
			}
			else if(nWorkSite == THD_RETEST_1_BUFF) //Ʈ���̿� ���� ���� ����
			{//��Ŀ�� ������ Ŭ����ǰ�, Ʈ���̴� ������ ������  
			} 
			else if(nWorkSite >= THD_TESTSITE_1 && nWorkSite <= THD_TESTSITE_8) //�׽�Ʈ ���Ͽ��� ���� ���� ����
			{
			}
			///////////////////////////////	 
			//data exchange
			//////////////////////////////
			if(nWorkSite == THD_LD_BUFF || nWorkSite == THD_ULD_BUFF || nWorkSite == THD_RETEST_1_BUFF || nWorkSite == THD_RETEST_2_BUFF)
			{
				//test site�� ��� st_picker[nRobotSite].st_pcb_info[npRobotPos[0]].nEnable = st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nEnable;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nYesNo			= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nYesNo;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nBin			= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBin;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nRetestCnt		= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestCnt;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nRetest			= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetest;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nBinNum			= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBinNum;//����� ��� 
				//st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nRetestSite[st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestCnt] = st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestSite[st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestCnt];
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nBdNum			= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBdNum ;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nScrCode		= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nScrCode;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strLotNo		= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strLotNo;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strSerialNo		= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strSerialNo;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strBarcode		= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strPartNo		= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPartNo;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strArrNo		= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strArrNo;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strPPID			= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPPID;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strPSID			= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPSID;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strWWN			= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strWWN;
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strCSerialNo	= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strCSerialNo;
				// jtkim 20160903
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strBarcode1D[0]	= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode1D[0];
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strBarcode1D[1]	= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode1D[1];
				// jtkim 20160929
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strBarcode2D[0]	= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode2D[0];
				st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].strBarcode2D[1]	= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode2D[1];
				//test site�� ��� st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].tStart = st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].tStart;			

				//////////////////////
				//data clear
				/////////////////////////
				//test site�� ��� st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nEnable = st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]].nEnable;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nYesNo = CTL_NO;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBin = -1;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestCnt = 0;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetest = 0;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBinNum = -1;//����� ��� 
				//st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestSite[st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nRetestCnt] = -1;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBdNum  = -1;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nScrCode = -1;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strLotNo = _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strSerialNo = _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode = _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPartNo = _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strArrNo = _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPPID = _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPSID = _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strWWN = _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strCSerialNo = _T("");
				// jtkim 20160903
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode1D[0]	= _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode1D[1]	= _T("");
				// jtkim 20160929
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode2D[0]	= _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode2D[1]	= _T("");
				//test site�� ��� st_picker[nRobotSite].st_pcb_info[npRobotPos[0]].tStart;	
			}
			else if(nWorkSite >= THD_TESTSITE_1 && nWorkSite <= THD_TESTSITE_8) //�׽�Ʈ ���Ͽ��� ���� ���� ����
			{//��Ŀ�� ������ ������ ����, �׽�Ʈ ������ ������ Ŭ���� �ȴ� 		
				//test site�� ��� st_picker[nRobotSite].st_pcb_info[npRobotPos[0]].nEnable = st_test_site_info[nWorkSite][npWorkPos[0]].st_pcb_info[npWorkPos[1]].nEnable;
				
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].nYesNo = st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nYesNo;
				//st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].nBin = st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBin;
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].nBin = BD_NONE;
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].nRetestCnt = st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestCnt;
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].nRetest = st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetest;
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].nBinNum = st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBinNum ;//����� ��� 
				//st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestSite[st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestCnt] = st_test_site_info[nWorkSite - THD_LEFT_TEST_SITE][npWorkPos[0]].st_pcb_info[npWorkPos[1]].nRetestSite[st_test_site_info[nWorkSite][npWorkPos[0]].st_pcb_info[npWorkPos[1]].nRetestCnt];
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].nBdNum = st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBdNum  ;
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].nScrCode = st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nScrCode ;
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strLotNo = st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strLotNo;
			    st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strSerialNo = st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strSerialNo;
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strBarcode = st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode;
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strPartNo = st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPartNo;
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strArrNo = st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strArrNo;
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strPPID = st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPPID ;
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strPSID = st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPSID ;
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strWWN = st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strWWN;
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strCSerialNo = st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strCSerialNo;
				// jtkim 20160903
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strBarcode1D[0]	= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode1D[0];
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strBarcode1D[1]	= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode1D[1];
				// jtkim 20160929
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strBarcode2D[0]	= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode2D[0];
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].strBarcode2D[1]	= st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode2D[1];
				//test site�� ��� st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].tStart = st_test_site_info[nWorkSite][npWorkPos[0]].st_pcb_info[npWorkPos[1]].tStart;			
				// jtkim 20160906
				st_test_site_info[nWorkSite].st_pcb_info[npWorkPos[1]].nTestBdStart = BD_NONE;
				//////////////////////
				//data clear
				/////////////////////////
				//test site�� ��� st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nEnable = st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nEnable;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nYesNo = CTL_NO;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBin = -1;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestCnt = 0;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetest = 0;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBinNum = -1;//����� ��� 
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nRetestSite[st_tray_info[nWorkSite].st_pcb_info[npWorkPos[0]][npWorkPos[1]].nRetestCnt] = -1;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nBdNum  = -1;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].nScrCode = -1;
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strLotNo = _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strSerialNo = _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode = _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPartNo = _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strArrNo = _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPPID = _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strPSID = _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strWWN = _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strCSerialNo = _T("");

				// jtkim 20160903
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode1D[0]	= _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode1D[1]	= _T("");
				// jtkim 20160929
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode2D[0]	= _T("");
				st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].strBarcode2D[1]	= _T("");
				//test site�� ��� st_picker[nRobotSite].st_pcb_info[npWorkPos[3]].tStart;	
				 
				/* james 2016.0804 
				// jtkim 20150424
				if (nWorkSite == THD_LEFT_TEST_SITE)
				{
					clsInterfaceC.m_nBdInfo[0][npWorkPos[0]][npWorkPos[1]] = NO;
				}
				else
				{
					clsInterfaceC.m_nBdInfo[1][npWorkPos[0]][npWorkPos[1]] = NO;
				}//*/
				
				//test site�� ��� st_test_site_info[nWorkSite][npWorkPos[0]].st_pcb_info[npWorkPos[1]].tStart;	
			}
		}
	}
	
	return RET_GOOD;
}


////////////////////////////////////////////////////////////////////////////////////
//�׽�Ʈ ����Ʈ ����� ����
// test site socket ������ LEFT SITE(Test Site #1,#2,#3,#4), Right SITE(Test Site #5,#6,#7,#8)
//Left Site Socket ���� ��ȣ  : 7,6,5,4,3,2,1,0 --> ��Ŀ�ͼ��� �ݴ�  (��Ŀ:0,1,2,3)
//Right Site Socket ������ȣ  : 0,1,2,3,4,5,6,7  --> ��Ŀ�� ���� ��ġ(��Ŀ:0,1,2,3)
//�ٿ�� ���� Left/Right ��ġ�� ������� ��� ������ ���� �������� ���� ��ġ�� �����Ѵ�(FIX : 0,1,2,3,4,5,6,7)
// ���鿡�� ������ �׽�Ʈ����Ʈ ���� ��ġ �������� 0 -> 7 ���� Ȯ��
////////////////////////////////////////////////////////////////////////////////////
int CPublicFunction::Find_TestSite_Work_Pos(int nMode, CString strLotNo, int PickPlace_Mode, int nTestSyncCount, int *npGetWork_SiteInfo)
{
	int nFuncRet = RET_ERROR;
	int nTHD_i, x, nFixCnt, nToTalCnt;
	int n_Fix_03_Count; //���鿡�� ������ ���� ��ġ������ �������� 0 -> 7 �� 0 ~ 3 ���� ��ġ �۾����� ���� 
	int n_Fix_47_Count; //���鿡�� ������ ���� ��ġ������ �������� 0 -> 7 �� 4 ~ 7 ���� ��ġ �۾����� ���� 
	int nSite = 0; //Test site LEFT(TEST#1~#4), RIGHT(#5~#8) ��ġ, ���� ��ȣ�� �ݴ��̴� 
	int nSocketNo=0;
	int n_Fix_03_Enable_Count;
	int n_Fix_47_Enable_Count;
	
	//PickPlace_Mode ==>  0: pick mode ���� Ȯ��(�׽�Ʈ�� ���� ��ġ ������ �ִ��� üũ), 1:place mode(���縦 ������ �ִ� ��ġ�� �ִ��� üũ 
	//nTestSyncCount  ==>  ȯ�� üũ�� �ʿ��� �׽�Ʈ ���� ����//4para �̴� 4�ϰ��� , 4���� ���ÿ� üũ�ϴ� ���
	//npGetWork_SiteInfo  ==>  [0]:start socket pos(0~47), [1]end socket pos(0~47), [2]:left, right site ����(0,1), [3]:socket start Y Pos(0~1, btm, top ����), [4]:socket start X Pos(0~11, 12�� ���� ��ġ)
		
	for(nTHD_i = THD_TESTSITE_1; nTHD_i <= THD_TESTSITE_8; nTHD_i++) //0~7������ �׽�Ʈ ����Ʈ ���� 
	{	//������ ���� ������ �̸� Ŭ���� �Ѵ� 		
		st_find_testsite_info[nTHD_i][PickPlace_Mode].nRun_Mode		= 0;//0:�۾� �Ұ���, 1:�ε� �۾�����, 2:��ε� �۾����� 
		st_find_testsite_info[nTHD_i][PickPlace_Mode].nSite_Pos		= 0; //left/right site 
		st_find_testsite_info[nTHD_i][PickPlace_Mode].nStart_FixPos = 0; //�۾� ������ ����Ʈ 0~7 ���� �� ���� ���� 		
		st_find_testsite_info[nTHD_i][PickPlace_Mode].nFix_Cnt		= 0; //���� ����Ŭ���� �۾� ������ ���� 
		st_find_testsite_info[nTHD_i][PickPlace_Mode].nTotal_Cnt	= 0; //�ش� ����Ʈ�� ��ü �۾� ������ ���� 

		nFixCnt = 0; nToTalCnt = 0;
		n_Fix_03_Count = 0; n_Fix_47_Count = 0;
		n_Fix_03_Enable_Count = 0; n_Fix_47_Enable_Count = 0;
		
		if(nTHD_i < THD_TESTSITE_5)  nSite = TESTSITE_LEFT;  //1,2,3,4 test site 
		else                         nSite = TESTSITE_RIGHT; //5,6,7,8 test site

		for(x = 0; x < TEST_SOCKET_PARA; x++) //x ���� ���� ���� ����(8��, 0~7��)�� �����Ѵ� 
		{			 					
			if(PickPlace_Mode == WORK_PLACE) //2:Test site�� ���縦 ���� �� �ִ� ��ġ üũ
			{
				if(nSite == TESTSITE_LEFT) //test socket left ������������ :7->6->5->4  3->2->1->0
				{                          //�������� �۾����� ��������    :0->1->2->3, 4->5->6->7
					nSocketNo = (TEST_SOCKET_PARA - 1) - x; //�۾����� : 7->6->5->4  ->3->2->1->0
// 					if(st_test_site_info[nTHD_i].strLotNo == strLotNo && st_test_site_info[nTHD_i].st_pcb_info[nSocketNo].nYesNo == CTL_NO && 
// 					   st_test_site_info[nTHD_i].st_pcb_info[nSocketNo].nEnable == CTL_YES)
					//2016.1017
					if(st_test_site_info[nTHD_i].strLotNo == strLotNo && st_test_site_info[nTHD_i].st_pcb_info[nSocketNo].nYesNo == CTL_NO && 
					   st_test_site_info[nTHD_i].st_pcb_info[nSocketNo].nEnable == CTL_YES && st_test_site_info[nTHD_i].st_pcb_info[nSocketNo].nTestBdStart == BD_NONE &&
					   ( st_test_site_info[nTHD_i].nTestBdStart == BD_NONE || st_test_site_info[nTHD_i].nTestBdStart == BD_END) )
					{//����̽��� ����, ������ ������� �Ǿ������� ���縦 ���� �� �ִ�  
						nToTalCnt++; //ã�� �۾� ���ǰ� ���� ������ �ִ� ���� ���� 
						if(x < 4) n_Fix_03_Count++;
						else if(x < 8) n_Fix_47_Count++;
					}

					if(st_test_site_info[nTHD_i].st_pcb_info[nSocketNo].nEnable == CTL_YES)
					{
						if(x < 4) n_Fix_03_Enable_Count++;
						else if(x < 8) n_Fix_47_Enable_Count++;
					}
				}
				else if(nSite == TESTSITE_RIGHT)//test socket right ������������ : 0->1->2->3,  4->5->6->7
				{                               //�������� �۾�����  ��������           : 0->1->2->3,  4->5->6->7
					nSocketNo = x; //�������������� ��Ŀ���� �� ���� �۾������� ���� : 0->1->2->3,  4->5->6->7
//					if(st_test_site_info[nTHD_i].strLotNo == strLotNo && st_test_site_info[nTHD_i].st_pcb_info[nSocketNo].nYesNo == CTL_NO &&
//					   st_test_site_info[nTHD_i].st_pcb_info[nSocketNo].nEnable == CTL_YES)
					//2016.1017
					if(st_test_site_info[nTHD_i].strLotNo == strLotNo && st_test_site_info[nTHD_i].st_pcb_info[nSocketNo].nYesNo == CTL_NO &&
					   st_test_site_info[nTHD_i].st_pcb_info[nSocketNo].nEnable == CTL_YES &&  st_test_site_info[nTHD_i].st_pcb_info[nSocketNo].nTestBdStart == BD_NONE &&
					   ( st_test_site_info[nTHD_i].nTestBdStart == BD_NONE || st_test_site_info[nTHD_i].nTestBdStart == BD_END) )
					{//����̽��� ����, ������ ������� �Ǿ������� ���縦 ���� �� �ִ�  
						nToTalCnt++; //ã�� �۾� ���ǰ� ���� ������ �ִ� ���� ���� 
						if(x < 4) n_Fix_03_Count++;
						else if(x < 8) n_Fix_47_Count++;
					}
					if(st_test_site_info[nTHD_i].st_pcb_info[nSocketNo].nEnable == CTL_YES)
					{
						if(x < 4) n_Fix_03_Enable_Count++;
						else if(x < 8) n_Fix_47_Enable_Count++;
					}
				}				
			}
			else if(PickPlace_Mode == WORK_PICK) //1:���� �� �ִ� ��ġ üũ
			{
				//test left site�� ���� ���������� �������� ���������� �ݴ��̴� 
				if(nSite == TESTSITE_LEFT) //test socket left ������������ :7->6->5->4  3->2->1->0
				{                          //�������� �۾�����             :0->1->2->3, 4->5->6->7
					nSocketNo = (TEST_SOCKET_PARA - 1) - x; //�۾����� :    7->6->5->4  ->3->2->1->0
					//if(st_test_site_info[nTHD_i].strLotNo == strLotNo && st_test_site_info[nTHD_i].st_pcb_info[nSocketNo].nYesNo == CTL_YES 
					//	&& st_test_site_info[nTHD_i].nTestBdStart == BD_END)
					// jtkim 20160831
					if(st_test_site_info[nTHD_i].strLotNo == strLotNo && st_test_site_info[nTHD_i].st_pcb_info[nSocketNo].nYesNo == CTL_YES && 
					   st_test_site_info[nTHD_i].st_pcb_info[nSocketNo].nTestBdStart == BD_END)
					{//����̽��� ����, ������ ������� �Ǿ������� ���縦 ���� �� �ִ�  
						nToTalCnt++; //ã�� �۾� ���ǰ� ���� ������ �ִ� ���� ���� 
						if(x < 4) 
						{
							n_Fix_03_Count++; 
						}
						else if(x < 8)
						{
							n_Fix_47_Count++; 
						}						
					}
					//if(st_test_site_info[nTHD_i].st_pcb_info[nSocketNo].nEnable == CTL_YES)
					//kwlee 2017.0125
					if(st_test_site_info[nTHD_i].st_pcb_info[nSocketNo].nYesNo == CTL_YES)
					{
						if(x < 4) n_Fix_03_Enable_Count++;
						else if(x < 8) n_Fix_47_Enable_Count++;

						//�ļ����� ������ ���� 
						n_Fix_03_Enable_Count = 1; 
						n_Fix_47_Enable_Count = 1;
					}
				}//test right site�� ���� ���������� �������� ���������� �����ϴ� 
				else if(nSite == TESTSITE_RIGHT)//test socket right ������������ : 0->1->2->3,  4->5->6->7
				{                               //�������� �۾�����             : 0->1->2->3,  4->5->6->7
					nSocketNo = x; //�������������� ��Ŀ���� �� ���� �۾������� ���� : 0->1->2->3,  4->5->6->7
					//if(st_test_site_info[nTHD_i].strLotNo == strLotNo && st_test_site_info[nTHD_i].st_pcb_info[nSocketNo].nYesNo == CTL_YES && 
					//	st_test_site_info[nTHD_i].nTestBdStart == BD_END)
					if(st_test_site_info[nTHD_i].strLotNo == strLotNo && st_test_site_info[nTHD_i].st_pcb_info[nSocketNo].nYesNo == CTL_YES && 
					   st_test_site_info[nTHD_i].st_pcb_info[nSocketNo].nTestBdStart == BD_END)
					{//����̽��� ����, ������ ������� �Ǿ������� ���縦 ���� �� �ִ�  
						nToTalCnt++; //ã�� �۾� ���ǰ� ���� ������ �ִ� ���� ���� 
						if(x < 4)
						{
							n_Fix_03_Count++; 
						}
						else if(x < 8)
						{
							n_Fix_47_Count++; 
						}						
					}
					//if(st_test_site_info[nTHD_i].st_pcb_info[nSocketNo].nEnable == CTL_YES)
					//kwlee 2017.0125
					if(st_test_site_info[nTHD_i].st_pcb_info[nSocketNo].nYesNo == CTL_YES)
					{
						if(x < 4) n_Fix_03_Enable_Count++;
						else if(x < 8) n_Fix_47_Enable_Count++;

						//�ļ����� ������ ���� 
						n_Fix_03_Enable_Count = 1; 
						n_Fix_47_Enable_Count = 1;
					}
				}				 
			}//else if
		}//for

		if(nToTalCnt > 0 ) //�۾��� ������ �ִ� 
		{
			//if(n_Fix_03_Count >= n_Fix_03_Enable_Count && n_Fix_47_Count >= n_Fix_47_Enable_Count) //���õ� �׽�Ʈ ����Ʈ�� ��ü ������ �۾������� ����
			//kwlee 2017.0224
			if(n_Fix_03_Count > 0 && n_Fix_03_Count >= n_Fix_03_Enable_Count && n_Fix_47_Count >= n_Fix_47_Enable_Count) //���õ� �׽�Ʈ ����Ʈ�� ��ü ������ �۾������� ����
			{//���� left(1,2,3,4) �� right(5,6,7,8)�� ��� �䱸�� �۾��� ������ ����Ʈ�̸� �������� left site�� �켱.
			
				//npGetWork_SiteInfo[0] = i; //�۾��� �׽�Ʈ ����Ʈ ���� 
				if(PickPlace_Mode == WORK_PLACE) //Test site�� ���縦 ���� �� �ִ� ��ġ üũ
				{
					st_find_testsite_info[nTHD_i][PickPlace_Mode].nRun_Mode		= WORK_PLACE; //2: �׽�Ʈ ���Ͽ� �ε� ����
					st_find_testsite_info[nTHD_i][PickPlace_Mode].nSite_Pos		= nSite; //TESTSITE_LEFT,TESTSITE_RIGHT
					st_find_testsite_info[nTHD_i][PickPlace_Mode].nStart_FixPos = 0;//�������� ��ġ ���� ��ġ�� left site
					st_find_testsite_info[nTHD_i][PickPlace_Mode].nFix_Cnt		= n_Fix_03_Count;
					st_find_testsite_info[nTHD_i][PickPlace_Mode].nTotal_Cnt	= nToTalCnt;		
					nFuncRet = RET_GOOD;
				}
				else if(PickPlace_Mode == WORK_PICK) //0:���� �� �ִ� ��ġ üũ
				{
					st_find_testsite_info[nTHD_i][PickPlace_Mode].nRun_Mode		= WORK_PICK; //1; //�׽�Ʈ ���Ͽ� ��ε� ����
					st_find_testsite_info[nTHD_i][PickPlace_Mode].nSite_Pos		= nSite; //TESTSITE_LEFT,TESTSITE_RIGHT
					st_find_testsite_info[nTHD_i][PickPlace_Mode].nStart_FixPos = 0;//�������� ��ġ ���� ��ġ��  left site
					st_find_testsite_info[nTHD_i][PickPlace_Mode].nFix_Cnt		= n_Fix_03_Count;
					st_find_testsite_info[nTHD_i][PickPlace_Mode].nTotal_Cnt	= nToTalCnt;
					nFuncRet = RET_GOOD;
				}
			}
			//else if(n_Fix_03_Count >= n_Fix_03_Enable_Count)//2015.0906 0) //8�� ������ ���� 4�� ����(0 ~ 3)
			//kwlee 2017.0224
			else if(n_Fix_03_Count > 0 && n_Fix_03_Count >= n_Fix_03_Enable_Count)//2015.0906 0) //8�� ������ ���� 4�� ����(0 ~ 3)
			{
				if(PickPlace_Mode == WORK_PLACE) //Test site�� ���縦 ���� �� �ִ� ��ġ üũ
				{
					st_find_testsite_info[nTHD_i][PickPlace_Mode].nRun_Mode		= WORK_PLACE; //1: �׽�Ʈ ���Ͽ� �ε� ����
					st_find_testsite_info[nTHD_i][PickPlace_Mode].nSite_Pos		= nSite; //TESTSITE_LEFT,TESTSITE_RIGHT
					st_find_testsite_info[nTHD_i][PickPlace_Mode].nStart_FixPos = 0;//�������� ��ġ ���� ��ġ��  left site
					st_find_testsite_info[nTHD_i][PickPlace_Mode].nFix_Cnt		= n_Fix_03_Count;
					st_find_testsite_info[nTHD_i][PickPlace_Mode].nTotal_Cnt	= nToTalCnt;	
					nFuncRet = RET_GOOD;
				}
				else if(PickPlace_Mode == WORK_PICK) //0:���� �� �ִ� ��ġ üũ
				{
					st_find_testsite_info[nTHD_i][PickPlace_Mode].nRun_Mode		= WORK_PICK; //2; //�׽�Ʈ ���Ͽ� ��ε� ����
					st_find_testsite_info[nTHD_i][PickPlace_Mode].nSite_Pos		= nSite; //TESTSITE_LEFT,TESTSITE_RIGHT
					st_find_testsite_info[nTHD_i][PickPlace_Mode].nStart_FixPos = 0;//�������� ��ġ ���� ��ġ��  left site
					st_find_testsite_info[nTHD_i][PickPlace_Mode].nFix_Cnt		= n_Fix_03_Count;
					st_find_testsite_info[nTHD_i][PickPlace_Mode].nTotal_Cnt	= nToTalCnt;
					nFuncRet = RET_GOOD;
				}
					
			}
			//else if(n_Fix_47_Count >= n_Fix_47_Enable_Count)// 0) //8�� ������ ���� 4�� ����(4 ~ 7)
			//kwlee 2017.0224
			else if( n_Fix_47_Count > 0 && n_Fix_47_Count >= n_Fix_47_Enable_Count)// 0) //8�� ������ ���� 4�� ����(4 ~ 7)
			{
				//���� left(1,2,3,4) �Ǵ� right(5,6,7,8)�� ��������̶�  �۾��� ������ ����Ʈ�̸� �� ����Ʈ�� �켱.
				if(PickPlace_Mode == WORK_PLACE) //Test site�� ���縦 ���� �� �ִ� ��ġ üũ
				{
					st_find_testsite_info[nTHD_i][PickPlace_Mode].nRun_Mode		= WORK_PLACE; //1: �׽�Ʈ ���Ͽ� �ε� ����
					st_find_testsite_info[nTHD_i][PickPlace_Mode].nSite_Pos		= nSite; //TESTSITE_LEFT,TESTSITE_RIGHT
					st_find_testsite_info[nTHD_i][PickPlace_Mode].nStart_FixPos = 4;//�������� ��ġ ���� ��ġ��  right site
					st_find_testsite_info[nTHD_i][PickPlace_Mode].nFix_Cnt		= n_Fix_47_Count;
					st_find_testsite_info[nTHD_i][PickPlace_Mode].nTotal_Cnt	= nToTalCnt;	
					nFuncRet = RET_GOOD;
				}
				else if(PickPlace_Mode == WORK_PICK) //0:���� �� �ִ� ��ġ üũ
				{
					st_find_testsite_info[nTHD_i][PickPlace_Mode].nRun_Mode		= WORK_PICK; //2; //�׽�Ʈ ���Ͽ� ��ε� ����
					st_find_testsite_info[nTHD_i][PickPlace_Mode].nSite_Pos		= nSite; //TESTSITE_LEFT,TESTSITE_RIGHT
					st_find_testsite_info[nTHD_i][PickPlace_Mode].nStart_FixPos = 4;//�������� ��ġ ���� ��ġ��  right site
					st_find_testsite_info[nTHD_i][PickPlace_Mode].nFix_Cnt		= n_Fix_47_Count;
					st_find_testsite_info[nTHD_i][PickPlace_Mode].nTotal_Cnt	= nToTalCnt;
					nFuncRet = RET_GOOD;
				}
			}

			//nFuncRet = RET_GOOD;
		}//if
	}//for
	return nFuncRet;
} 


//james 2016.0816 �߰� 
//first picker num, first buff num, �۾� ���ǿ� �����ϴ� �� ���� �Ǵ� ���� ���� ��ü ����
//RETEST_BIN �� ó�� 
 int CPublicFunction::Find_RetestBinBuffer_WorkPos_Check(int n_Mode, int n_TestSite, int n_BuffSite, int n_PickerDvcYesNO,int n_BuffYesNO, int *np_Picker_YesNo, int *np_FindPosYXCPB,  int *np_BuffInfo_Count)  
{//james 2016.0815 ������ �۾� ������ ��ġ�� ã�´� 
	//n_BuffStartNo = 0(Right Test), 14(Left Test)�� ¦�� �������� ù��°�� ã�� ���� 
	//              = 1(Right Test), 13(Left Test)�� ¦�� �������� �ι�°�� ã�� ���� 
	//np_BuffInfo_Count[15] = 15�� ������ ��� ��ġ�� �˻��Ͽ� ���� ���� ����� �� �ִ� ��ġ�� ������ Ȯ�ΰ���. (��Ŀ�� ���簡 ������ ���� ���� �ѹ��� ������ �ִ� ��ġ�� ����Ͽ� �����Ѵ�
	//*np_Picker_YesNo[4] =  ���ڷ� ������ ��Ŀ ������ ���� ��ġ�� �켱 �����Ѵ�. , �� ������ �������� ã�� �����Ѵ�, �� ���̾����� ��Ŀ�� ���簡 ������ �������� ���� �˸��� ��ġ Ȯ�� ����(*np_BuffInfo_Count)
	int i, nFuncRet, nFlag;
	int nFirstY =0, nFirstX=0;
	bool bFlag = false;

	int nBuffFirstNo = 0;
	int nBuff_x = 0;
	int nPicker_x =0; 
	int nPicker_Cnt = 0;
	int nBuffer_Cnt = 0;

	int nSortBuffer_First = -1;
	int nFirstPicker_Num = 0;
	int npPicker_YesNo[4];
	int npBuffInfo_Count[15]; //15 ���ۿ� 4���� �κ� ��Ŀ���� ������ ���Ͽ� ���� ������ ���ϸ��� ��ġ�� ã�´�
	int nRetestSkip_flag = 0;
	int nFindPos = 0;

	nFuncRet = RET_SKIP;

	//  
	 nFlag = CTL_NO;

	 nFirstPicker_Num = -1;
	 for(i = 0; i < TEST_PICKER_PARA; i++)
	 {
		 //(n_PickerYesNO == CTL_NO �̸� ���縦 �����ִ� ���� �����̸�, ���۵�� ���縦 �������� ���·�, enable ������ ��Ŀ�� ���� 
		 if(np_Picker_YesNo[i] == n_PickerDvcYesNO) //st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == n_DvcYesNO && (n_DvcYesNO == CTL_YES && (n_DvcYesNO == CTL_YES || (n_PickerYesNO == CTL_NO && st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable == CTL_YES)) ) )
		 {
			 nFirstPicker_Num = i;
			 break;
		 }
	 }

	 for(i=0; i < st_recipe_info.nRetestBuffer_Num; i++) npBuffInfo_Count[i] = 0; //15 clear 

	 if(nFirstPicker_Num == -1)
	 {
		 return RET_SKIP; //��Ŀ�� �۾��� ������ ���� 
	 }	  

	 nRetestSkip_flag = 0;

	 switch(n_BuffSite)
	 {
	
	 case THD_RETEST_1_BUFF:
	 case THD_RETEST_2_BUFF:  
		if(n_TestSite == TESTSITE_LEFT)  //��Ŀ����� ���۰� �ݴ� ���� //test left site�� wkwofmf �۾��ϱ� ���� �۾� ���϶�, ���׽�Ʈ ���۴� �׻� ���� �⺻ ����(Right Test Site ����)�� �����Ѵ�, LEFT SITE���� ���� ���۸� ȸ���Ͽ� 1�� �� ������ ����� 
		{			
			//nBuffFirstNo = st_recipe_info.nRetestBuffer_Num - 1;  //nRetestBuffer_Num == 15  
			//for(nBuff_x = 1; nBuff_x < st_recipe_info.nRetestBuffer_Num; (nBuff_x = nBuff_x + 2)) //nRetestBuffer_Num == 15  
			for(nBuffFirstNo = 14; nBuffFirstNo >= 13; nBuffFirstNo--)//14,13 üũ, �ش� ������ ��� ���۰����� ������ ã�´�(0 ~ 14������ �ְ� ������ ���� ��ġ�� ã�´�
			{ //nBuffFirstNo = 14 : 14,12,10,8,6,4,2,0, nBuffFirstNo = 13 : 13,11,9,7,5,3,1 �� ã�� ���� �� 
				for(nBuff_x = nBuffFirstNo; nBuff_x >= 0; (nBuff_x = nBuff_x - 2))//nRetestBuffer_Num == 15  
				{	//���� ��ġ�� �������� �ϳ��� ã�� ����. 		 
					nSortBuffer_First = -1;
					nPicker_Cnt = 0; nBuffer_Cnt = 0; //clear 
					nFlag = CTL_NO; 
					//npBuffInfo_Count[nBuff_x] = 0; //clear 

					if(n_Mode == BIN_RETESTBIN)
					{
						//partial ���縦 ã�� ���
						if(nBuff_x <= np_FindPosYXCPB[0] && nBuff_x >= (np_FindPosYXCPB[0] - (3  * 2)) )//m_nPartial_FirstRetestBuffPos_Select_No; //�� ��ġ���� 4���� ������ ���ǿ��� �����ϰ� ã�ƾ� �Ѵ�. �ֳĸ� �̰����� ������ ���縦 ������ �� ���� �׽�Ʈ ����Ʈ���̵��� ���� 
						{
							continue; 
						}
					}

					if(st_buffer_info[n_BuffSite].st_pcb_info[nBuff_x].nYesNo == n_BuffYesNO) //���ۿ��� ���۰����� ��ġ�� ã�´� 
					{				 
						for(nPicker_x = 0; nPicker_x < TEST_PICKER_PARA; nPicker_x++) //TEST_PICKER_PARA == 4 //for(nPicker_x = m_nFirstPicker_Num; nPicker_x < TEST_PICKER_PARA; nPicker_x++) //TEST_PICKER_PARA == 4
						{
							npPicker_YesNo[nPicker_x] = CTL_NO; //�۾��� ��Ŀ���� clear�ϱ� 0���� for������ �ʱ�ȭ ���� 
							if(nPicker_x < nFirstPicker_Num) continue;

							if(np_Picker_YesNo[nPicker_x] == n_PickerDvcYesNO)// && (n_DvcYesNO == CTL_YES &&  (n_DvcYesNO == CTL_YES || (n_DvcYesNO == CTL_NO && np_Picker_YesNo[nPicker_x] == CTL_YES)) ) ) //(n_PickerYesNO == CTL_NO || st_picker[n_RbtSite].st_pcb_info[i].nBdNum == n_SortBin))//��Ŀ�� ���簡 �հ�, ���ú�� ��ġ�ϸ� 
							{	//n_PickerYesNO == CTL_NO �� PIC�ϱ� ���� ã�� ����, n_PickerYesNO == CTL_YES �� ���縦 PLACE �ϱ� ���� ã�� ���� 						 
								npPicker_YesNo[nPicker_x] = CTL_YES;
								nPicker_Cnt++;

								nFindPos = nBuff_x - ((nPicker_x - nFirstPicker_Num) * 2);
								if(nFindPos >= 0 && nFindPos < st_recipe_info.nRetestBuffer_Num) //james 2016.0927 > 0
								{					 
									if(st_buffer_info[n_BuffSite].st_pcb_info[nFindPos].nYesNo == n_BuffYesNO) //������ ������ ��ĭ�ǳ� �ϳ��� üũ, ���� ������� ��Ŀ����� 1/2 �� 1:1�� ��ġ�����ʾ� ���� �ʿ�.
									{
										//npPicker_YesNo[nPicker_x] = CTL_YES;
										//nPicker_Cnt++;

										//��Ŀ�� ������ ������ ����. ���� �� �� �ִ� 
										if(nFlag == CTL_NO)
										{
											nSortBuffer_First = nBuff_x;
											nFlag = CTL_YES;
										}
										nBuffer_Cnt++;

										npBuffInfo_Count[nBuff_x]++;//��� �۾� ���� ���� 
									}
									else
									{		
										//���� �ʴ� ����, ��Ŀ�� ���簡 ������������ ���������, �������� �����ϸ� �浹�� �߻��Ѵ� .. ö���� �����ʿ�.

										//��Ŀ�� ������ ������ �ٸ���. �� ������ ����  ��ġ������ �浹�Ͽ� ������ �� ����.
										//nPicker_x = TEST_PICKER_PARA;
										//break;
									}
								}
							}
						} //for

						if(nPicker_Cnt > 0 && nPicker_Cnt == nBuffer_Cnt) //���縦 ���ۿ� �ѹ��� ���� �� �ִ� ��ġ�� �ִ� //���� OK
						{	
							if(nFuncRet == RET_SKIP)
							{
								np_FindPosYXCPB[0] = nSortBuffer_First;
								np_FindPosYXCPB[1] = nSortBuffer_First;
								np_FindPosYXCPB[2] = nPicker_Cnt;
								np_FindPosYXCPB[3] = nFirstPicker_Num;
								np_FindPosYXCPB[4] = nSortBuffer_First;

								for(nPicker_x = 0; nPicker_x < TEST_PICKER_PARA; nPicker_x++)
								{
									np_Picker_YesNo[nPicker_x] = npPicker_YesNo[nPicker_x]; //��� �۾� ���� ���� 
								}		
								//nFuncRet = RET_GOOD;
								return RET_GOOD;
							}
						}		
						else if(nBuffer_Cnt > 0)
						{
							if(nRetestSkip_flag == 0)
							{
								np_FindPosYXCPB[0] = nSortBuffer_First;
								np_FindPosYXCPB[1] = nSortBuffer_First;
								np_FindPosYXCPB[2] = nPicker_Cnt;
								np_FindPosYXCPB[3] = nFirstPicker_Num;
								np_FindPosYXCPB[4] = nSortBuffer_First;

								for(nPicker_x = 0; nPicker_x < TEST_PICKER_PARA; nPicker_x++)
								{
									//np_Picker_YesNo[nPicker_x] = npPicker_YesNo[nPicker_x]; //����� ��Ŀ ���� ��� 
								}		

								nRetestSkip_flag++;
							}							
						}
					}//if
				}//for
			}//for
		}
		else if(n_TestSite == TESTSITE_RIGHT) //��Ŀ����� ���۰� ���� ����
		{

			for(nBuffFirstNo = 0; nBuffFirstNo <= 1; nBuffFirstNo++)//0,1 üũ, �ش� ������ ��� ���۰����� ������ ã�´�(0 ~ 14������ �ְ� ������ ���� ��ġ�� ã�´�
			{  //nBuffFirstNo = 0 : 0,2,4,6,8,10,12,14  nBuffFirstNo = 1 : 1,3,5,7,9,11,13 �� ã�� ���� �� 
				for(nBuff_x = nBuffFirstNo; nBuff_x <= 14; (nBuff_x = nBuff_x + 2))//nRetestBuffer_Num == 15  
				{	//���� ��ġ�� �������� �ϳ��� ã�� ����. 		 
					nSortBuffer_First = -1;
					nPicker_Cnt = 0; nBuffer_Cnt = 0; //clear 
					nFlag = CTL_NO; 

					if(n_Mode == BIN_RETESTBIN)
					{
						//partial ���縦 ã�� ���
						if(nBuff_x <= np_FindPosYXCPB[0] && nBuff_x >= (np_FindPosYXCPB[0] - (3  * 2)) )//m_nPartial_FirstRetestBuffPos_Select_No; //�� ��ġ���� 4���� ������ ���ǿ��� �����ϰ� ã�ƾ� �Ѵ�. �ֳĸ� �̰����� ������ ���縦 �������� ���� �׽�Ʈ ����Ʈ���̵��� ���� 
						{
							continue; 
						}
					}

					if(st_buffer_info[n_BuffSite].st_pcb_info[nBuff_x].nYesNo == n_BuffYesNO) //���ۿ��� ���۰����� ��ġ�� ã�´� 
					{				 
						for(nPicker_x = 0; nPicker_x < TEST_PICKER_PARA; nPicker_x++) //TEST_PICKER_PARA == 4 //for(nPicker_x = m_nFirstPicker_Num; nPicker_x < TEST_PICKER_PARA; nPicker_x++) //TEST_PICKER_PARA == 4
						{
							npPicker_YesNo[nPicker_x] = CTL_NO; //�۾��� ��Ŀ���� clear�ϱ� 0���� for������ �ʱ�ȭ ���� 
							if(nPicker_x < nFirstPicker_Num) continue;

							if(np_Picker_YesNo[nPicker_x] == n_PickerDvcYesNO)// && (n_DvcYesNO == CTL_YES &&  (n_DvcYesNO == CTL_YES || (n_DvcYesNO == CTL_NO && st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable == CTL_YES)) ) ) //(n_PickerYesNO == CTL_NO || st_picker[n_RbtSite].st_pcb_info[i].nBdNum == n_SortBin))//��Ŀ�� ���簡 �հ�, ���ú�� ��ġ�ϸ� 
							{	//n_PickerYesNO == CTL_NO �� PIC�ϱ� ���� ã�� ����, n_PickerYesNO == CTL_YES �� ���縦 PLACE �ϱ� ���� ã�� ���� 						 
								npPicker_YesNo[nPicker_x] = CTL_YES;
								nPicker_Cnt++;

								nFindPos = nBuff_x + ((nPicker_x - nFirstPicker_Num) * 2);//james 2016.0927 
								if(nFindPos >= 0 && nFindPos < st_recipe_info.nRetestBuffer_Num) //james 2016.0927 > 0
								{					 
									if(st_buffer_info[n_BuffSite].st_pcb_info[nFindPos].nYesNo == n_BuffYesNO) //������ ������ ��ĭ�ǳ� �ϳ��� üũ, ���� ������� ��Ŀ����� 1/2 �� 1:1�� ��ġ�����ʾ� ���� �ʿ�.
									{
										//npPicker_YesNo[nPicker_x] = CTL_YES;
										//nPicker_Cnt++;

										//��Ŀ�� ������ ������ ����. ���� �� �� �ִ� 
										if(nFlag == CTL_NO)
										{
											nSortBuffer_First = nBuff_x;
											nFlag = CTL_YES;
										}
										nBuffer_Cnt++;

										npBuffInfo_Count[nBuff_x]++;//��� �۾� ���� ���� 
									}
									else
									{		
										//���� �ʴ� ����, ��Ŀ�� ���簡 ������������ ���������, �������� �����ϸ� �浹�� �߻��Ѵ� .. ö���� �����ʿ�.

										//��Ŀ�� ������ ������ �ٸ���. �� ������ ����  ��ġ������ �浹�Ͽ� ������ �� ����.
										//nPicker_x = TEST_PICKER_PARA;
										//break;
									}
								}
							}
						} 

						if(nPicker_Cnt > 0 && nPicker_Cnt == nBuffer_Cnt) //���縦 ���ۿ� �ѹ��� ���� �� �ִ� ��ġ�� �ִ� //���� OK
						{	
							if(nFuncRet == RET_SKIP) //ó������ ������ ���� �۾� ������ ��ġ Ȯ�� 
							{
								np_FindPosYXCPB[0] = nSortBuffer_First;
								np_FindPosYXCPB[1] = nSortBuffer_First;
								np_FindPosYXCPB[2] = nPicker_Cnt;
								np_FindPosYXCPB[3] = nFirstPicker_Num;
								np_FindPosYXCPB[4] = nSortBuffer_First;

								for(nPicker_x = 0; nPicker_x < TEST_PICKER_PARA; nPicker_x++)
								{
									np_Picker_YesNo[nPicker_x] = npPicker_YesNo[nPicker_x]; //����� ��Ŀ ���� ��� 
								}		
								//nFuncRet = RET_GOOD;
								return RET_GOOD;
							}
						}	
						else if(nBuffer_Cnt > 0)
						{
							if(nRetestSkip_flag == 0)
							{
								np_FindPosYXCPB[0] = nSortBuffer_First;
								np_FindPosYXCPB[1] = nSortBuffer_First;
								np_FindPosYXCPB[2] = nPicker_Cnt;
								np_FindPosYXCPB[3] = nFirstPicker_Num;
								np_FindPosYXCPB[4] = nSortBuffer_First;

								for(nPicker_x = 0; nPicker_x < TEST_PICKER_PARA; nPicker_x++)
								{
									//np_Picker_YesNo[i] = npPicker_YesNo[i]; //����� ��Ŀ ���� ��� 
								}		

								nRetestSkip_flag++;
							}							
						}
					}//if
				}//for
			}//for
		}
		break;
	}
	 for(i=0; i < st_recipe_info.nRetestBuffer_Num; i++)
	 {
		 np_BuffInfo_Count[i] = npBuffInfo_Count[i];
	 }
	return nFuncRet;
}


//james 2016.0816 ����
//first picker num, first buff num, �۾� ���ǿ� �����ϴ� �� ���� �Ǵ� ���� ���� ��ü ����
 int CPublicFunction::Find_RetestBufferWork_Pos(int n_RbtSite, int n_TestSite, int n_BuffSite, int n_BuffFirstNo, int n_SortBin, int n_PickerYesNO, int n_BuffYesNO, int *np_FindPosYXCPB, int *np_Picker_YesNo)  //������
{//james 2016.0815 ������ �۾� ������ ��ġ�� ã�´� 
	//n_BuffStartNo = 0(Right Test), 14(Left Test)�� ¦�� �������� ù��°�� ã�� ���� 
	//              = 1(Right Test), 13(Left Test)�� ¦�� �������� �ι�°�� ã�� ���� 
	int i, nFuncRet, nFlag;
	int nFirstY =0, nFirstX=0;
	bool bFlag = false;

	int nBuff_x = 0;
	int nPicker_x =0; 
	int nPicker_Cnt = 0;
	int nBuffer_Cnt = 0;
	int nBuffFirstNo= 0;	

	int nSortBuffer_First = -1;
	int nFirstPicker_Num = 0;
	int npPicker_YesNo[4];
	int npBuffInfo_Count[15]; //15 ���ۿ� 4���� �κ� ��Ŀ���� ������ ���Ͽ� ���� ������ ���ϸ��� ��ġ�� ã�´�

	nFuncRet = RET_SKIP;

	//  
	 nFlag = CTL_NO;

	 nFirstPicker_Num = -1;
	 for(i = 0; i < 4; i++)
	 {
		 //(n_PickerYesNO == CTL_NO �̸� ���縦 �����ִ� ���� �����̸�, ���۵�� ���縦 �������� ���·�, enable ������ ��Ŀ�� ���� 
		 if(st_picker[n_RbtSite].st_pcb_info[i].nYesNo == n_PickerYesNO && (n_PickerYesNO == CTL_YES && (n_PickerYesNO == CTL_YES || (n_PickerYesNO == CTL_NO && st_picker[n_RbtSite].st_pcb_info[i].nEnable == CTL_YES)) ) )
		 {
			 nFirstPicker_Num = i;
			 break;
		 }
	 }

	 for(i=0; i<15; i++) npBuffInfo_Count[i] = 0; //clear 

	 if(nFirstPicker_Num == -1)
	 {
		 return RET_SKIP; //��Ŀ�� �۾��� ������ ���� 
	 }	  
	 

	 nFirstPicker_Num = np_FindPosYXCPB[3];

	 switch(n_BuffSite)
	 {
	
	 case THD_RETEST_1_BUFF:
	 case THD_RETEST_2_BUFF:  
		if(n_TestSite == TESTSITE_LEFT)  //��Ŀ����� ���۰� �ݴ� ���� //test left site�� wkwofmf �۾��ϱ� ���� �۾� ���϶�, ���׽�Ʈ ���۴� �׻� ���� �⺻ ����(Right Test Site ����)�� �����Ѵ�, LEFT SITE���� ���� ���۸� ȸ���Ͽ� 1�� �� ������ ����� 
		{			
			//nBuffFirstNo = st_recipe_info.nRetestBuffer_Num - 1;  //nRetestBuffer_Num == 15 // 14,12,10,8,6,4,2,0 ��� 
			//for(nBuff_x = 1; nBuff_x < st_recipe_info.nRetestBuffer_Num; (nBuff_x = nBuff_x + 2)) //nRetestBuffer_Num == 15 //0,2,4,6,8,10,12,14 �� �켱 ã�´�
			for(nBuff_x = n_BuffFirstNo; nBuff_x >= 0; (nBuff_x = nBuff_x - 2))//nRetestBuffer_Num == 15 //14,12,10,8,6,4,2,0 �� 8�� ��ġ�� �켱 ã�´�
			{	//���� ��ġ�� �������� �ϳ��� ã�� ����. 		 
				nSortBuffer_First = -1;
				nPicker_Cnt = 0; nBuffer_Cnt = 0; //clear 
				nFlag = CTL_NO; 
				if(st_buffer_info[n_BuffSite].st_pcb_info[nBuff_x].nYesNo == n_BuffYesNO) //���ۿ��� ���۰����� ��ġ�� ã�´� 
				{				 
					for(nPicker_x = 0; nPicker_x < TEST_PICKER_PARA; nPicker_x++) //TEST_PICKER_PARA == 4 //for(nPicker_x = m_nFirstPicker_Num; nPicker_x < TEST_PICKER_PARA; nPicker_x++) //TEST_PICKER_PARA == 4
					{
						npPicker_YesNo[i] = CTL_NO; //�۾��� ��Ŀ���� clear�ϱ� 0���� for������ �ʱ�ȭ ���� 
						if(i < nFirstPicker_Num) continue;

						if(st_picker[n_RbtSite].st_pcb_info[nPicker_x].nYesNo == n_PickerYesNO && (n_PickerYesNO == CTL_YES || (n_PickerYesNO == CTL_NO && st_picker[n_RbtSite].st_pcb_info[nPicker_x].nEnable == CTL_YES) ) ) //(n_PickerYesNO == CTL_NO || st_picker[n_RbtSite].st_pcb_info[i].nBdNum == n_SortBin))//��Ŀ�� ���簡 �հ�, ���ú�� ��ġ�ϸ� 
						{	//n_PickerYesNO == CTL_NO �� PIC�ϱ� ���� ã�� ����, n_PickerYesNO == CTL_YES �� ���縦 PLACE �ϱ� ���� ã�� ���� 						 
							npPicker_YesNo[i] = CTL_YES;
							nPicker_Cnt++;
					 
							if(st_buffer_info[n_BuffSite].st_pcb_info[ nBuff_x - ((nPicker_x - nFirstPicker_Num) * 2)].nYesNo == n_BuffYesNO) //������ ������ ��ĭ�ǳ� �ϳ��� üũ, ���� ������� ��Ŀ����� 1/2 �� 1:1�� ��ġ�����ʾ� ���� �ʿ�.
							{
								//��Ŀ�� ������ ������ ����. ���� �� �� �ִ� 
								if(nFlag == CTL_NO)
								{
									nSortBuffer_First = nBuff_x;
									nFlag = CTL_YES;
								}
								nBuffer_Cnt++;
							}
							else
							{
								//��Ŀ�� ������ ������ �ٸ���. �� ������ ����  ��ġ������ �浹�Ͽ� ������ �� ����.
								nPicker_x = TEST_PICKER_PARA;
								break;
							}
						}
					} 

					if(nPicker_Cnt == nBuffer_Cnt) //���縦 ���ۿ� �ѹ��� ���� �� �ִ� ��ġ�� �ִ� //���� OK
					{	
						np_FindPosYXCPB[0] = nSortBuffer_First;
						np_FindPosYXCPB[1] = nSortBuffer_First;
						np_FindPosYXCPB[2] = nPicker_Cnt;
						np_FindPosYXCPB[3] = nFirstPicker_Num;
						np_FindPosYXCPB[4] = nSortBuffer_First;

						for(nPicker_x = 0; nPicker_x < TEST_PICKER_PARA; nPicker_x++)
						{
							np_Picker_YesNo[i] = npPicker_YesNo[i];
						}						

						nBuff_x = -1;				 
						nFuncRet = RET_GOOD;
					}
				}
			}
		}
		else //if(st_buffer_info[n_BuffSite].nTestSite_LR_Pos == TESTSITE_RIGHT) //��Ŀ����� ���۰� ���� ����
		{

			for(nBuff_x = n_BuffFirstNo; nBuff_x < st_recipe_info.nRetestBuffer_Num; (nBuff_x = nBuff_x + 2)) //nRetestBuffer_Num == 15 //0,2,4,6,8,10,12,14 �� �켱 ã�´�
			{			 
				nSortBuffer_First = -1;
				nPicker_Cnt = 0; nBuffer_Cnt = 0; //clear 
				nFlag = CTL_NO; 
				if(st_buffer_info[n_BuffSite].st_pcb_info[nBuff_x].nYesNo == n_BuffYesNO) //���ۿ��� ���۰����� ��ġ�� ã�´� 
				{				 
					for(nPicker_x = 0; nPicker_x < TEST_PICKER_PARA; nPicker_x++) //TEST_PICKER_PARA == 4 //for(nPicker_x = m_nFirstPicker_Num; nPicker_x < TEST_PICKER_PARA; nPicker_x++) //TEST_PICKER_PARA == 4
					{
						npPicker_YesNo[i] = CTL_NO; //�۾��� ��Ŀ���� clear�ϱ� ���� 
						if(i < nFirstPicker_Num) continue;

						if(st_picker[n_RbtSite].st_pcb_info[i].nYesNo == n_PickerYesNO && (n_PickerYesNO == CTL_NO || st_picker[n_RbtSite].st_pcb_info[i].nBdNum == n_SortBin))// 
						{
							npPicker_YesNo[i] = CTL_YES; 
							nPicker_Cnt++;
					 
							if(st_buffer_info[n_BuffSite].st_pcb_info[ nBuff_x + ((nPicker_x - nFirstPicker_Num) * 2)].nYesNo == n_BuffYesNO)
							{
								//��Ŀ�� ������ ������ ����. ���� �� �� �ִ� 
								if(nFlag == CTL_NO)
								{
									nSortBuffer_First = nBuff_x;
									nFlag = CTL_YES;
								}
								nBuffer_Cnt++;
							}
							else
							{
								//��Ŀ�� ������ ������ �ٸ���. �� ������ ����  ��ġ������ �浹�Ͽ� ������ �� ����. 
								nPicker_x = TEST_PICKER_PARA;
								break;
							}
						}
					} 

					if(nPicker_Cnt == nBuffer_Cnt) //���縦 ���ۿ� �ѹ��� ���� �� �ִ� ��ġ�� �ִ� //���� OK
					{
						np_FindPosYXCPB[0] = nSortBuffer_First;
						np_FindPosYXCPB[1] = nSortBuffer_First;
						np_FindPosYXCPB[2] = nPicker_Cnt;
						np_FindPosYXCPB[3] = nFirstPicker_Num;
						np_FindPosYXCPB[4] = nSortBuffer_First;

						for(i = 0; i < TEST_PICKER_PARA; i++)
						{
							np_Picker_YesNo[i] = npPicker_YesNo[i];
						}						

						nBuff_x = st_recipe_info.nRetestBuffer_Num;				 
						nFuncRet = RET_GOOD;
					}
				}
			}
		}
		break;
	}

	return nFuncRet;
}



 int CPublicFunction::Find_TestRobot_WherePos(int n_Mode, int *np_TestSite)
 {
	 int nFuncRet = RET_ERROR;
	 int i;	 
	 int nRet_1,nRet_2,nRet_3,nRet_4;	 

	 for(i = 0; i < 8; i++)
	 {
		 //nRet_1 = COMI.Check_MotPosRange(M_TEST_RBT_X, st_motor_info[M_TEST_RBT_X].d_pos[P_TESTRBT_XY_TESTDUCT_1_LEFT_START + i], 50);
		// nRet_2 = COMI.Check_MotPosRange(M_TEST_RBT_Y, st_motor_info[M_TEST_RBT_Y].d_pos[P_TESTRBT_XY_TESTDUCT_1_LEFT_START + i], 50);
		// nRet_3 = COMI.Check_MotPosRange(M_TEST_RBT_X, st_motor_info[M_TEST_RBT_X].d_pos[P_TESTRBT_XY_TESTDUCT_1_RIGHT_START + i], 50);
		// nRet_4 = COMI.Check_MotPosRange(M_TEST_RBT_Y, st_motor_info[M_TEST_RBT_Y].d_pos[P_TESTRBT_XY_TESTDUCT_1_RIGHT_START + i], 50);
		 // jtkim 20160904
		 
		 //nRet_1 = COMI.Check_MotPosRange(M_TEST_RBT_X, st_motor_info[M_TEST_RBT_X].d_pos[P_TESTRBT_XY_TESTDUCT_1_LEFT_START + (i * 2)], 50);
		 //nRet_2 = COMI.Check_MotPosRange(M_TEST_RBT_Y, st_motor_info[M_TEST_RBT_Y].d_pos[P_TESTRBT_XY_TESTDUCT_1_LEFT_START + (i * 2)], 50);
		 //nRet_3 = COMI.Check_MotPosRange(M_TEST_RBT_X, st_motor_info[M_TEST_RBT_X].d_pos[P_TESTRBT_XY_TESTDUCT_1_RIGHT_START + (i * 2)], 50);
		 //nRet_4 = COMI.Check_MotPosRange(M_TEST_RBT_Y, st_motor_info[M_TEST_RBT_Y].d_pos[P_TESTRBT_XY_TESTDUCT_1_RIGHT_START + (i * 2)], 50);
		 //kwlee 2016.1219 Vision Test 
		 nRet_1 = COMI.Check_MotPosRange(M_TEST_RBT_X, st_motor_info[M_TEST_RBT_X].d_pos[P_TESTRBT_XY_TESTDUCT_1_LEFT_START + (i * 2)], 90);
		 nRet_2 = COMI.Check_MotPosRange(M_TEST_RBT_Y, st_motor_info[M_TEST_RBT_Y].d_pos[P_TESTRBT_XY_TESTDUCT_1_LEFT_START + (i * 2)], 90);
		 nRet_3 = COMI.Check_MotPosRange(M_TEST_RBT_X, st_motor_info[M_TEST_RBT_X].d_pos[P_TESTRBT_XY_TESTDUCT_1_RIGHT_START + (i * 2)], 90);
		 nRet_4 = COMI.Check_MotPosRange(M_TEST_RBT_Y, st_motor_info[M_TEST_RBT_Y].d_pos[P_TESTRBT_XY_TESTDUCT_1_RIGHT_START + (i * 2)], 90);
		// jtkim 20160903
/*
		 if(nRet_1 == BD_GOOD && nRet_2 == BD_GOOD)
		 {// i ��ġ�� left pos
			np_TestSite[0] =  i, np_TestSite[1] = TESTSITE_LEFT;
			nFuncRet = RET_GOOD; 
		 }
		 else if(nRet_3 == BD_GOOD && nRet_4 == BD_GOOD)
		 {//i ��ġ�� right pos
			 np_TestSite[0] =  i, np_TestSite[1] = TESTSITE_RIGHT;
			 nFuncRet = RET_GOOD; 
		 }
*/
		 if (nRet_1 == BD_GOOD || nRet_3 == BD_GOOD)
		 {
			 if (nRet_2 == BD_GOOD || nRet_4 == BD_GOOD)
			 {
				np_TestSite[0] =  i;
				if (i < 4) np_TestSite[1] = TESTSITE_LEFT;
				else np_TestSite[1] = TESTSITE_RIGHT;
				nFuncRet = RET_GOOD; 
			 }
		 }
	 }

	 return nFuncRet; 
 }
 ///////////////////////
 int CPublicFunction::Check_TestRobot_Pos()
 {
	 int nFuncRet = RET_ERROR;
/*	 int i;	*/ 
	 //int nRet_1,nRet_2,nRet_3,nRet_4;	 
	 int nRet_1[2][16];
	 int nRet_2[2][16];
	// for(i = 0; i < 8; i++)
	// {
		 //nRet_1 = COMI.Check_MotPosRange(M_TEST_RBT_X, st_motor_info[M_TEST_RBT_X].d_pos[P_TESTRBT_XY_TESTDUCT_1_LEFT_START + i], 50);
		// nRet_2 = COMI.Check_MotPosRange(M_TEST_RBT_Y, st_motor_info[M_TEST_RBT_Y].d_pos[P_TESTRBT_XY_TESTDUCT_1_LEFT_START + i], 50);
		// nRet_3 = COMI.Check_MotPosRange(M_TEST_RBT_X, st_motor_info[M_TEST_RBT_X].d_pos[P_TESTRBT_XY_TESTDUCT_1_RIGHT_START + i], 50);
		// nRet_4 = COMI.Check_MotPosRange(M_TEST_RBT_Y, st_motor_info[M_TEST_RBT_Y].d_pos[P_TESTRBT_XY_TESTDUCT_1_RIGHT_START + i], 50);
		 // jtkim 20160904
		 //nRet_1 = COMI.Check_MotPosRange(M_TEST_RBT_X, st_motor_info[M_TEST_RBT_X].d_pos[P_TESTRBT_XY_TESTDUCT_1_LEFT_START + (i * 2)], 50);
		 //nRet_2 = COMI.Check_MotPosRange(M_TEST_RBT_Y, st_motor_info[M_TEST_RBT_Y].d_pos[P_TESTRBT_XY_TESTDUCT_1_LEFT_START + (i * 2)], 50);
		 //nRet_3 = COMI.Check_MotPosRange(M_TEST_RBT_X, st_motor_info[M_TEST_RBT_X].d_pos[P_TESTRBT_XY_TESTDUCT_1_RIGHT_START + (i * 2)], 50);
		 //nRet_4 = COMI.Check_MotPosRange(M_TEST_RBT_Y, st_motor_info[M_TEST_RBT_Y].d_pos[P_TESTRBT_XY_TESTDUCT_1_RIGHT_START + (i * 2)], 50);
		 nRet_1[0][0] = COMI.Check_MotPosRange(M_TEST_RBT_X,16.345, 50); //#1-Left Duct#1
		 nRet_1[0][1] = COMI.Check_MotPosRange(M_TEST_RBT_X,15.151 , 50);
		 nRet_1[0][2] = COMI.Check_MotPosRange(M_TEST_RBT_X,15.151 , 50);
		 nRet_1[0][3] = COMI.Check_MotPosRange(M_TEST_RBT_X,15.349 , 50);

		 nRet_1[1][0] = COMI.Check_MotPosRange(M_TEST_RBT_X,165.801 , 50);//#1-Right Duct#1
		 nRet_1[1][1] = COMI.Check_MotPosRange(M_TEST_RBT_X,148.999, 50);
		 nRet_1[1][2] = COMI.Check_MotPosRange(M_TEST_RBT_X,148.999 , 50);
		 nRet_1[1][3] = COMI.Check_MotPosRange(M_TEST_RBT_X,165.498 , 50);

		 nRet_1[0][4] = COMI.Check_MotPosRange(M_TEST_RBT_X,1789.734 , 50); //#5-Left Duct#1
		 nRet_1[0][5] = COMI.Check_MotPosRange(M_TEST_RBT_X,1790.457, 50);
		 nRet_1[0][6] = COMI.Check_MotPosRange(M_TEST_RBT_X,1790.457 , 50);
		 nRet_1[0][7] = COMI.Check_MotPosRange(M_TEST_RBT_X, 1788.489, 50);

		 nRet_1[1][4] = COMI.Check_MotPosRange(M_TEST_RBT_X,1939.799 , 50);//#5 Right Duct#1
		 nRet_1[1][5] = COMI.Check_MotPosRange(M_TEST_RBT_X,1941.224 , 50);
		 nRet_1[1][6] = COMI.Check_MotPosRange(M_TEST_RBT_X,1938.764 , 50);
		 nRet_1[1][7] = COMI.Check_MotPosRange(M_TEST_RBT_X,1938.764 , 50);


		 ///Test Robot Y
		 nRet_2[0][0] = COMI.Check_MotPosRange(M_TEST_RBT_Y,694.122 , 50); //#1Left Duct#1
		 nRet_2[0][1] = COMI.Check_MotPosRange(M_TEST_RBT_Y,1658.855, 50);
		 nRet_2[0][2] = COMI.Check_MotPosRange(M_TEST_RBT_Y,2619.417 , 50);
		 nRet_2[0][3] = COMI.Check_MotPosRange(M_TEST_RBT_Y,3599.411 , 50);

		 nRet_2[1][0] = COMI.Check_MotPosRange(M_TEST_RBT_Y,694.366 , 50);	//#1-Right Duct#1
		 nRet_2[1][1] = COMI.Check_MotPosRange(M_TEST_RBT_Y,1658.855 , 50);
		 nRet_2[1][2] = COMI.Check_MotPosRange(M_TEST_RBT_Y,2619.417 , 50);
		 nRet_2[1][3] = COMI.Check_MotPosRange(M_TEST_RBT_Y,3599.710 , 50);



		 nRet_2[0][4] = COMI.Check_MotPosRange(M_TEST_RBT_Y,695.544 , 50);//#5-Left Duct#1
		 nRet_2[0][5] = COMI.Check_MotPosRange(M_TEST_RBT_Y,1658.855 , 50);
		 nRet_2[0][6] = COMI.Check_MotPosRange(M_TEST_RBT_Y,2619.417 , 50);
		 nRet_2[0][7] = COMI.Check_MotPosRange(M_TEST_RBT_Y,3600.519 , 50);

		 nRet_2[1][4] = COMI.Check_MotPosRange(M_TEST_RBT_Y, 695.864, 50);//#5- Right Duct#1
		 nRet_2[1][5] = COMI.Check_MotPosRange(M_TEST_RBT_Y,1658.855 , 50);
		 nRet_2[1][6] = COMI.Check_MotPosRange(M_TEST_RBT_Y,2619.417 , 50);
		 nRet_2[1][7] = COMI.Check_MotPosRange(M_TEST_RBT_Y,3600.819 , 50);
		// jtkim 20160903
/*
		 if(nRet_1 == BD_GOOD && nRet_2 == BD_GOOD)
		 {// i ��ġ�� left pos
			np_TestSite[0] =  i, np_TestSite[1] = TESTSITE_LEFT;
			nFuncRet = RET_GOOD; 
		 }
		 else if(nRet_3 == BD_GOOD && nRet_4 == BD_GOOD)
		 {//i ��ġ�� right pos
			 np_TestSite[0] =  i, np_TestSite[1] = TESTSITE_RIGHT;
			 nFuncRet = RET_GOOD; 
		 }
*/
// 		 if (nRet_1 == BD_GOOD || nRet_3 == BD_GOOD)
// 		 {
// 			 if (nRet_2 == BD_GOOD || nRet_4 == BD_GOOD)
// 			 {
// 				np_TestSite[0] =  i;
// 				if (i < 4) np_TestSite[1] = TESTSITE_LEFT;
// 				else np_TestSite[1] = TESTSITE_RIGHT;
// 				nFuncRet = RET_GOOD; 
// 			 }
// 		 }
	// }

		 for (int i = 0; i<2; i++)
		 {
			 for (int j = 0; j<8; j++)
			 {
				 if (nRet_1[i][j] == BD_GOOD && nRet_2[i][j] == BD_GOOD)
				 {
					 nFuncRet = RET_GOOD;
				 }
			 }
		 }
	 return nFuncRet; 
 }
 /////////////////////////

 int CPublicFunction::EnableSocketSiteWorking( int n_Site )
 {
	 int nFuncRet = RET_ERROR;
	 int i = 0, Site = THD_TESTSITE_1 + n_Site;
	 bool bEnable = false;
	CString strErr;


	return RET_GOOD;

	 for( i  =0 ;i < 8; i++ )
	 {
		if(st_buffer_info[Site].st_pcb_info[i].nEnable == CTL_YES)  
		{
			bEnable = true;
			break;
		}
	 }

	 //��� ������ ������ ���� ���� üũ
	 if(  bEnable== true  )
	 {
		 if(FAS_IO.get_in_bit( st_io_info.i_TsiteLockOnChk[n_Site] , IO_ON ) == IO_ON )//�׽�Ʈ�� �����Ǿ� �ִ�
		 {
				 if(  FAS_IO.get_in_bit( st_io_info.i_TsiteExistChk[n_Site] , IO_ON ) == IO_ON )
				 {
						if( FAS_IO.get_in_bit(st_io_info.i_TsiteUpChk[n_Site],	IO_OFF)	== IO_OFF &&
							FAS_IO.get_in_bit(st_io_info.i_TsiteDnChk[n_Site],	IO_ON)	== IO_ON)
						{
							nFuncRet = RET_GOOD;
						}
						else
						{
							strErr.Format( _T("TESTER : It doesn't check UpDn-Cylinser-Sensor on TESTER %d th" ), n_Site + 1 );
							st_handler_info.mstr_event_msg[0] = strErr;
							::PostMessage(st_handler_info.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
						}
				 }
				 else
				 {
					strErr.Format( _T("TESTER : There is not exist TESTER %d th, Please check socket enable."), n_Site + 1 );
					st_handler_info.mstr_event_msg[0] = strErr;
					::PostMessage(st_handler_info.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
				 }
		  }
		  else//�׽�Ʈ�� �����Ǿ� ���� �ʴ�
		  {
				strErr.Format( _T("TESTER : It doesn't check to lock on TESTER %d th, Please check socket enable."), n_Site + 1 );
				st_handler_info.mstr_event_msg[0] = strErr;
				::PostMessage(st_handler_info.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
		  }		
	 }
	 else//��� ������ ������ ���µ� üũ�Ǵ� ���
	 {
		//if(  FAS_IO.get_in_bit( st_io_info.i_TsiteExistChk[n_Site] , IO_ON ) == IO_ON )
		//{
			if( FAS_IO.get_in_bit(st_io_info.i_TsiteUpChk[n_Site],	IO_OFF)	== IO_OFF &&
				FAS_IO.get_in_bit(st_io_info.i_TsiteDnChk[n_Site],	IO_ON)	== IO_ON)
			{
				nFuncRet = RET_GOOD;
			}
			else
			{					
				strErr.Format( _T("TESTER : It checks UpDn-Cylinser-Sensor on TESTER %d th"), n_Site + 1 );
				st_handler_info.mstr_event_msg[0] = strErr;
				::PostMessage(st_handler_info.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
			}
		//}
		//else
		//{
		//}
	}

	 return nFuncRet;
 }


 int CPublicFunction::Find_Work_Site(int nLotNum, CString strLotNo, CString strPartNo)
 {
	 int i;

	 // jtkim 20160828
	 if (nLotNum == LOT_CURR)
	 {
		 for (i=0; i<8; i++)
		 {
			st_test_site_info[THD_TESTSITE_1 + i].strLotNo	= strLotNo;
			st_test_site_info[THD_TESTSITE_1 + i].strPartNo	= strPartNo;
		 }
	 }

	 Set_LotInfo_LotStartData(nLotNum, strLotNo);//james 2016.0924 �߰� 

	 if (st_lot_info[LOT_CURR].nStacker_Good_THD_Num == 0 && st_lot_info[LOT_NEXT].nStacker_Good_THD_Num == 0)
	 {
		 st_tray_info[THD_ULD_1_STACKER].strLotNo	= strLotNo;
		 st_tray_info[THD_ULD_1_STACKER].strPartNo	= strPartNo;

		 st_lot_info[nLotNum].nStacker_Good_THD_Num = THD_ULD_1_STACKER;

		 st_tray_info[THD_REJECT_OUT_1_STACKER].strLotNo	= strLotNo;
		 st_tray_info[THD_REJECT_OUT_1_STACKER].strPartNo	= strPartNo;

		 st_lot_info[nLotNum].nStacker_Reject_THD_Num = THD_REJECT_OUT_1_STACKER;

		 st_buffer_info[THD_RETEST_1_BUFF].strLotNo	= strLotNo;
		 st_buffer_info[THD_RETEST_1_BUFF].strPartNo	= strPartNo;

		 st_lot_info[nLotNum].nBuffer_Retest_THD_Num = THD_RETEST_1_BUFF;
	 }
	 else 
	 {
		 if (st_lot_info[LOT_CURR].nStacker_Good_THD_Num == THD_ULD_1_STACKER)
		 {
			 st_tray_info[THD_ULD_2_STACKER].strLotNo	= strLotNo;
			 st_tray_info[THD_ULD_2_STACKER].strPartNo	= strPartNo;

			 st_lot_info[nLotNum].nStacker_Good_THD_Num = THD_ULD_2_STACKER;

			 st_tray_info[THD_REJECT_OUT_2_STACKER].strLotNo	= strLotNo;
			 st_tray_info[THD_REJECT_OUT_2_STACKER].strPartNo	= strPartNo;

			 st_lot_info[nLotNum].nStacker_Reject_THD_Num = THD_REJECT_OUT_2_STACKER;

			 st_buffer_info[THD_RETEST_2_BUFF].strLotNo	= strLotNo;
			 st_buffer_info[THD_RETEST_2_BUFF].strPartNo	= strPartNo;

			 st_lot_info[nLotNum].nBuffer_Retest_THD_Num = THD_RETEST_2_BUFF;
		 }
		 else
		 {
			 st_tray_info[THD_ULD_1_STACKER].strLotNo	= strLotNo;
			 st_tray_info[THD_ULD_1_STACKER].strPartNo	= strPartNo;

			 st_lot_info[nLotNum].nStacker_Good_THD_Num = THD_ULD_1_STACKER;

			 st_tray_info[THD_REJECT_OUT_1_STACKER].strLotNo	= strLotNo;
			 st_tray_info[THD_REJECT_OUT_1_STACKER].strPartNo	= strPartNo;

			 st_lot_info[nLotNum].nStacker_Reject_THD_Num = THD_REJECT_OUT_1_STACKER;

			 st_buffer_info[THD_RETEST_1_BUFF].strLotNo	= strLotNo;
			 st_buffer_info[THD_RETEST_1_BUFF].strPartNo	= strPartNo;

			 st_lot_info[nLotNum].nBuffer_Retest_THD_Num = THD_RETEST_1_BUFF;
		 }
	 }

/*
	 if (st_lot_info[nLotNum].nStacker_Good_THD_Num == 0)
	 {
		 st_tray_info[THD_ULD_1_STACKER].strLotNo	= strLotNo;
		 st_tray_info[THD_ULD_1_STACKER].strPartNo	= strPartNo;

		 st_lot_info[nLotNum].nStacker_Good_THD_Num = THD_ULD_1_STACKER;
	 }
	 //else if (st_tray_info[THD_ULD_2_STACKER].strLotNo == _T(""))
	 else if (st_tray_info[THD_ULD_2_STACKER].strLotNo == _T(""))
	 {
		 st_tray_info[THD_ULD_2_STACKER].strLotNo	= strLotNo;
		 st_tray_info[THD_ULD_2_STACKER].strPartNo	= strPartNo;

		 st_lot_info[nLotNum].nStacker_Good_THD_Num = THD_ULD_2_STACKER;
	 }
	 else 
	 {
		 return RET_ERROR;
	 }

	 if (st_tray_info[THD_REJECT_OUT_1_STACKER].strLotNo == _T(""))
	 {
		 st_tray_info[THD_REJECT_OUT_1_STACKER].strLotNo	= strLotNo;
		 st_tray_info[THD_REJECT_OUT_1_STACKER].strPartNo	= strPartNo;

		 st_lot_info[nLotNum].nStacker_Reject_THD_Num = THD_REJECT_OUT_1_STACKER;
	 }
	 else if (st_tray_info[THD_REJECT_OUT_2_STACKER].strLotNo == _T(""))
	 {
		 st_tray_info[THD_REJECT_OUT_2_STACKER].strLotNo	= strLotNo;
		 st_tray_info[THD_REJECT_OUT_2_STACKER].strPartNo	= strPartNo;

		 st_lot_info[nLotNum].nStacker_Reject_THD_Num = THD_REJECT_OUT_2_STACKER;
	 }
	 else 
	 {
		 return RET_ERROR;
	 }

	 if (st_buffer_info[THD_RETEST_1_BUFF].strLotNo == _T(""))
	 {
		 st_buffer_info[THD_RETEST_1_BUFF].strLotNo	= strLotNo;
		 st_buffer_info[THD_RETEST_1_BUFF].strPartNo	= strPartNo;

		 st_lot_info[nLotNum].nBuffer_Retest_THD_Num = THD_RETEST_1_BUFF;
	 }
	 else if (st_buffer_info[THD_RETEST_2_BUFF].strLotNo == _T(""))
	 {
		 st_buffer_info[THD_RETEST_2_BUFF].strLotNo	= strLotNo;
		 st_buffer_info[THD_RETEST_2_BUFF].strPartNo	= strPartNo;

		 st_lot_info[nLotNum].nBuffer_Retest_THD_Num = THD_RETEST_2_BUFF;
	 }
	 else 
	 {
		 return RET_ERROR;
	 }
*/
	 return RET_GOOD;
 }

 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//james 2016.0928 
// ���׽�Ʈ ������ PICK & PLACE �۾��� �÷��ú��ϰ� �����ϱ� ���� �Լ� �߰� 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int CPublicFunction::Find_RetestBuffer_PickPlace_WorkPos_Check(int n_PickPlace_Mode, int n_TestNum, int n_TestFixPos, int n_BuffSite, int *np_Picker_YesNo, int *np_FindPosYXCPB,  int *np_BuffInfo_Count)//james 2016.0928 retest buffer �۾����� ��ġ ã�� 
{//james 2016.0815 ������ �۾� ������ ��ġ�� ã�´� 
	//n_BuffStartNo = 0(Right Test), 14(Left Test)�� ¦�� �������� ù��°�� ã�� ���� 
	//              = 1(Right Test), 13(Left Test)�� ¦�� �������� �ι�°�� ã�� ���� 
	//np_BuffInfo_Count[15] = 15�� ������ ��� ��ġ�� �˻��Ͽ� ���� ���� ����� �� �ִ� ��ġ�� ������ Ȯ�ΰ���. (��Ŀ�� ���簡 ������ ���� ���� �ѹ��� ������ �ִ� ��ġ�� ����Ͽ� �����Ѵ�
	//*np_Picker_YesNo[4] =  ���ڷ� ������ ��Ŀ ������ ���� ��ġ�� �켱 �����Ѵ�. , �� ������ �������� ã�� �����Ѵ�, �� ���̾����� ��Ŀ�� ���簡 ������ �������� ���� �˸��� ��ġ Ȯ�� ����(*np_BuffInfo_Count)
	int i, nFuncRet, nFlag;
	int nFirstY =0, nFirstX=0;
	bool bFlag = false;

	int nBuffFirstNo = 0;
	int nBuff_x = 0;
	int nPicker_x =0; 
	int nPicker_Cnt = 0;
	int nBuffer_Cnt = 0;

	//int nSortBuffer_First = -1;
	int nFirstPicker_Num = 0;
	int npPicker_YesNo[4];
	int npBuffInfo_Count[20]; //15 ���ۿ� 4���� �κ� ��Ŀ���� ������ ���Ͽ� ���� ������ ���ϸ��� ��ġ�� ã�´�
	int nRetestSkip_flag = 0;
	int nFindPos = 0;


	//james 2016.0928
	int nClash_Buffer_Flag = 0;
	//int nPicker_Flag = 0;
	////int nBuffer_Flag = 0;
	//int nSocketPos = 0;

	int nFirstBuffer_Num  = -1;
	int nTestSite_LeftRight = 0;
	int npPickerWork_Enable[4] = {0,0,0,0};

	nFuncRet = RET_ABORT; //RET_ABORT :�۾��� ������ ����,  RET_SKIP;//�ļȷ� �۾��� �����ϴ� 

	//  
	 nFlag = CTL_NO;
	 nFirstPicker_Num = -1;

	 for(i=0; i < 20; i++) npBuffInfo_Count[i] = 0; //james 2016.0929 20 clear st_recipe_info.nRetestBuffer_Num; i++) npBuffInfo_Count[i] = 0; //15 clear 

	 //test site lwft/right ���� 
	 if(n_TestNum >= THD_TESTSITE_1 && n_TestNum <= THD_TESTSITE_4) nTestSite_LeftRight = TESTSITE_LEFT;
	 else if(n_TestNum >= THD_TESTSITE_5 && n_TestNum <= THD_TESTSITE_8) nTestSite_LeftRight = TESTSITE_RIGHT;
	 
	 if(n_PickPlace_Mode == WORK_PICK)
	 {
		 for(i = 0; i < TEST_PICKER_PARA; i++) //4, test site���� �۾��� ������ Ȯ���Ѵ�
		 {
			 npPickerWork_Enable[i] = CTL_NO;//clear
			 if(st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == CTL_NO && st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable == CTL_YES)
			 {
				 if(nTestSite_LeftRight  == TESTSITE_LEFT) //st_test_site_info[nTHD_ID].nSite_Pos == TESTSITE_LEFT) //test socket left ������������ :7->6->5->4  3->2->1->0
				 { 
					 nFindPos = ((TEST_SOCKET_PARA - 1) - n_TestFixPos) - i;
					 if(st_test_site_info[n_TestNum].st_pcb_info[nFindPos].nYesNo == CTL_NO && st_test_site_info[n_TestNum].st_pcb_info[nFindPos].nEnable == CTL_YES)
					 {
						 if(nFirstPicker_Num == -1)
						 {
							 nFirstPicker_Num = i;
						 }
						 npPickerWork_Enable[i] = CTL_YES;
						 npBuffInfo_Count[16]++; //2016.0929 
					 }
					 else
					 {
						 st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable = CTL_NO; //��Ŀ���� 
					 }
				 }
				 else if(nTestSite_LeftRight == TESTSITE_RIGHT) // st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].nSite_Pos == TESTSITE_RIGHT) //test socket right ������������ :0->1->2->3  4->5->6->7
				 { 
					 nFindPos = n_TestFixPos + i;
					 if(st_test_site_info[n_TestNum].st_pcb_info[nFindPos].nYesNo == CTL_NO && st_test_site_info[n_TestNum].st_pcb_info[nFindPos].nEnable == CTL_YES)
					 {
						 if(nFirstPicker_Num == -1)
						 {
							 nFirstPicker_Num = i;
						 }
						 npPickerWork_Enable[i] = CTL_YES;
						  npBuffInfo_Count[16]++; //2016.0929 
					 }
					 else
					 {
						 st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable = CTL_NO; //��Ŀ���� 
					 }
				 }
			 }
		 }//for
	 }
	 else if(n_PickPlace_Mode == WORK_PLACE)
	 {
		 for(i = 0; i < TEST_PICKER_PARA; i++)//st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == n_DvcYesNO && (n_DvcYesNO == CTL_YES && (n_DvcYesNO == CTL_YES || (n_PickerYesNO == CTL_NO && st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable == CTL_YES)) ) )
		 {
			npPickerWork_Enable[i] = CTL_NO;//clear
			if(st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == CTL_YES)
			{
				if(nFirstPicker_Num == -1)
				{
					nFirstPicker_Num = i;
				}
				npPickerWork_Enable[i] = CTL_YES;
				 npBuffInfo_Count[16]++; //2016.0929 
				//break;
			}
		}
	 }

	 if(nFirstPicker_Num == -1)
	 {
		 return RET_ABORT; //��Ŀ�� �۾��� ������ ���� 
	 }	


	 for(i = 0; i < 15; i++)
	 {
		 if(st_buffer_info[n_BuffSite].st_pcb_info[i].nYesNo == CTL_YES) 
		 {
			 npBuffInfo_Count[17]++;
		 }

	 }

	 //james 2016.0929 for(i=0; i < st_recipe_info.nRetestBuffer_Num; i++) npBuffInfo_Count[i] = 0; //15 clear 
	 	   

	 nRetestSkip_flag = 0;

	 switch(nTestSite_LeftRight)
	 {
	//james 2016.0928 
	//case THD_RETEST_1_BUFF:
	//case THD_RETEST_2_BUFF:  
	case TESTSITE_LEFT:   //��Ŀ����� ���۰� �ݴ� ���� //test left site�� wkwofmf �۾��ϱ� ���� �۾� ���϶�, ���׽�Ʈ ���۴� �׻� ���� �⺻ ����(Right Test Site ����)�� �����Ѵ�, LEFT SITE���� ���� ���۸� ȸ���Ͽ� 1�� �� ������ ����� 
		//nBuffFirstNo = st_recipe_info.nRetestBuffer_Num - 1;  //nRetestBuffer_Num == 15  
		//for(nBuff_x = 1; nBuff_x < st_recipe_info.nRetestBuffer_Num; (nBuff_x = nBuff_x + 2)) //nRetestBuffer_Num == 15  
		for(nBuffFirstNo = 14; nBuffFirstNo >= 13; nBuffFirstNo--)//14,13 üũ, �ش� ������ ��� ���۰����� ������ ã�´�(0 ~ 14������ �ְ� ������ ���� ��ġ�� ã�´�
		{ //nBuffFirstNo = 14 : 14,12,10,8,6,4,2,0, nBuffFirstNo = 13 : 13,11,9,7,5,3,1 �� ã�� ���� �� 			
			for(nBuff_x = nBuffFirstNo; nBuff_x >= 0; (nBuff_x = nBuff_x - 2))//nRetestBuffer_Num == 15  
			{	//���� ��ġ�� �������� �ϳ��� ã�� ����. 				
				if(n_PickPlace_Mode == WORK_PICK)
				{
					if(st_buffer_info[n_BuffSite].st_pcb_info[nBuff_x].nYesNo == CTL_NO) //���ۿ��� ���۰����� ��ġ�� ã�´� 
					{
						continue;
					}
				}
				else if(n_PickPlace_Mode == WORK_PLACE)
				{
					if(st_buffer_info[n_BuffSite].st_pcb_info[nBuff_x].nYesNo == CTL_YES) //���ۿ��� ���۰����� ��ġ�� ã�´� 
					{
						continue;
					}
				}	

				nFirstBuffer_Num = -1; //clear 
				nPicker_Cnt = 0; nBuffer_Cnt = 0; //clear 
				nClash_Buffer_Flag = 0;
				//npBuffInfo_Count[nBuff_x] = 0; //clear 
				for(nPicker_x = 0; nPicker_x < TEST_PICKER_PARA; nPicker_x++) //TEST_PICKER_PARA == 4 //for(nPicker_x = m_nFirstPicker_Num; nPicker_x < TEST_PICKER_PARA; nPicker_x++) //TEST_PICKER_PARA == 4
				{
					npPicker_YesNo[nPicker_x] = CTL_NO; //�۾��� ��Ŀ���� clear�ϱ� 0���� for������ �ʱ�ȭ ���� 
					//if(nPicker_x < nFirstPicker_Num) continue; //�۾����� ��Ŀ�� ������ üũ�Ͽ� ���� ����� �浹�ϸ� �������� ���� 

					nFindPos = nBuff_x - ((nPicker_x - nFirstPicker_Num) * 2);
					if(nFindPos >= 0 && nFindPos < st_recipe_info.nRetestBuffer_Num) //james 2016.0927 > 0
					{			
						if(n_PickPlace_Mode == WORK_PICK)
						{	//pick ��� ���� ã�� 
							if(st_picker[THD_TEST_RBT].st_pcb_info[nPicker_x].nYesNo == CTL_YES )//������ ��ġ�� ���ۿ� �浹�� üũ�Ѵ� 
							{
								if(st_buffer_info[n_BuffSite].st_pcb_info[nFindPos].nYesNo == CTL_NO) //������ ������ ��ĭ�ǳ� �ϳ��� üũ, ���� ������� ��Ŀ����� 1/2 �� 1:1�� ��ġ�����ʾ� ���� �ʿ�.
								{
									//�����ص� �浹���� 
								}
								else if(st_buffer_info[n_BuffSite].st_pcb_info[nFindPos].nYesNo == CTL_YES) 
								{	//���� ��ġ���� �����ϸ� ���簣 �浹�߻�
									nClash_Buffer_Flag = -100;
									nPicker_x = TEST_PICKER_PARA;
									break;
								}
							}
							else if(npPickerWork_Enable[nPicker_x] == CTL_YES && st_picker[THD_TEST_RBT].st_pcb_info[nPicker_x].nYesNo == CTL_NO)//������ ����ְ�, ����̽��� ������ 
							{
								//npPicker_YesNo[nPicker_x] = CTL_YES;
								nPicker_Cnt++;
								if(st_buffer_info[n_BuffSite].st_pcb_info[nFindPos].nYesNo == CTL_YES) //������ ������ ��ĭ�ǳ� �ϳ��� üũ, ���� ������� ��Ŀ����� 1/2 �� 1:1�� ��ġ�����ʾ� ���� �ʿ�.
								{
									//��Ŀ�� ������ ������ ����. ���� �� �� �ִ� 
									if(nFirstBuffer_Num == -1)
									{
										nFirstBuffer_Num = nBuff_x;
									}

									npPicker_YesNo[nPicker_x] = CTL_YES;

									nBuffer_Cnt++;
									npBuffInfo_Count[nBuff_x]++;//��� �۾� ���� ���� 
									//npBuffInfo_Count[17]++; //2016.0929 
								}
								else if(st_buffer_info[n_BuffSite].st_pcb_info[nFindPos].nYesNo == CTL_NO) //������ ������ ��ĭ�ǳ� �ϳ��� üũ, ���� ������� ��Ŀ����� 1/2 �� 1:1�� ��ġ�����ʾ� ���� �ʿ�.
								{//�������� 
								}														
							}
						}
						else if(n_PickPlace_Mode == WORK_PLACE)
						{//retest buffer dvc place ��� ���� ã�� 
							if(st_picker[THD_TEST_RBT].st_pcb_info[nPicker_x].nYesNo == CTL_YES)//������ ��ġ�� ���ۿ� �浹�� üũ�Ѵ� 
							{
								//npPicker_YesNo[nPicker_x] = CTL_YES;
								nPicker_Cnt++;
								if(st_buffer_info[n_BuffSite].st_pcb_info[nFindPos].nYesNo == CTL_NO) //������ ������ ��ĭ�ǳ� �ϳ��� üũ, ���� ������� ��Ŀ����� 1/2 �� 1:1�� ��ġ�����ʾ� ���� �ʿ�.
								{
									//��Ŀ�� ������ ������ ����. ���� �� �� �ִ� 
									if(nFirstBuffer_Num == -1)
									{
										nFirstBuffer_Num = nBuff_x;
									}

									npPicker_YesNo[nPicker_x] = CTL_YES;

									nBuffer_Cnt++;
									npBuffInfo_Count[nBuff_x]++;//��� �۾� ���� ���� 
									//npBuffInfo_Count[17]++; //2016.0929 
								}
								else if(st_buffer_info[n_BuffSite].st_pcb_info[nFindPos].nYesNo == CTL_YES) 
								{	//���� ��ġ���� �����ϸ� ���簣 �浹�߻�
									nClash_Buffer_Flag = -100;
									nPicker_x = TEST_PICKER_PARA;
									break;
								}
							}
							else if(st_picker[THD_TEST_RBT].st_pcb_info[nPicker_x].nYesNo == CTL_NO)//����̽��� ������ 
							{
								//�������� 														
							}
						}//else if(n_PickPlace_Mode == WORK_PLACE)
					}//if(nFindPos >= 0 						
				} //for(nPicker_x = 0;

				if(nClash_Buffer_Flag == 0 && nBuffer_Cnt > 0 && nPicker_Cnt == nBuffer_Cnt) //���縦 ���ۿ� �ѹ��� ���� �� �ִ� ��ġ�� �ִ� //���� OK
				{	
					if( 1 )//nFuncRet == RET_ABORT)
					{
						np_FindPosYXCPB[0] = nFirstBuffer_Num;
						np_FindPosYXCPB[1] = nFirstBuffer_Num;
						np_FindPosYXCPB[2] = nBuffer_Cnt;
						np_FindPosYXCPB[3] = nFirstPicker_Num;
						np_FindPosYXCPB[4] = nFirstBuffer_Num;

						for(nPicker_x = 0; nPicker_x < TEST_PICKER_PARA; nPicker_x++)
						{
							np_Picker_YesNo[nPicker_x] = npPicker_YesNo[nPicker_x]; //��� �۾� ���� ���� 
						}		
						//nFuncRet = RET_GOOD;

						for(i=0; i < 20; i++) //james 2016.0929 st_recipe_info.nRetestBuffer_Num; i++)
						{
							np_BuffInfo_Count[i] = npBuffInfo_Count[i];
						}

						return RET_GOOD;
					}
				}		
				else if(nClash_Buffer_Flag == 0 && nBuffer_Cnt > 0)
				{
					if(nRetestSkip_flag == 0)
					{
						np_FindPosYXCPB[0] = nFirstBuffer_Num;
						np_FindPosYXCPB[1] = nFirstBuffer_Num;
						np_FindPosYXCPB[2] = nBuffer_Cnt;
						np_FindPosYXCPB[3] = nFirstPicker_Num;
						np_FindPosYXCPB[4] = nFirstBuffer_Num;

						for(nPicker_x = 0; nPicker_x < TEST_PICKER_PARA; nPicker_x++)
						{
							//np_Picker_YesNo[nPicker_x] = npPicker_YesNo[nPicker_x]; //����� ��Ŀ ���� ��� 
						}		

						nFuncRet = RET_SKIP;


						nRetestSkip_flag++;
					}							
				}
				else //if(nClash_Buffer_Flag == -100)
				{
					npBuffInfo_Count[nBuff_x] = 0;	//
				}					//if
			}//for(nBuff_x = nBuffFirstNo;
		}//for(nBuffFirstNo = 14
		break;
		
	case  TESTSITE_RIGHT:
		for(nBuffFirstNo = 0; nBuffFirstNo <= 1; nBuffFirstNo++)//0,1 üũ, �ش� ������ ��� ���۰����� ������ ã�´�(0 ~ 14������ �ְ� ������ ���� ��ġ�� ã�´�
		{  //nBuffFirstNo = 0 : 0,2,4,6,8,10,12,14  nBuffFirstNo = 1 : 1,3,5,7,9,11,13 �� ã�� ���� �� 
			for(nBuff_x = nBuffFirstNo; nBuff_x <= 14; (nBuff_x = nBuff_x + 2))//nRetestBuffer_Num == 15  
			{	//���� ��ġ�� �������� �ϳ��� ã�� ����. 				
				if(n_PickPlace_Mode == WORK_PICK)
				{
					if(st_buffer_info[n_BuffSite].st_pcb_info[nBuff_x].nYesNo == CTL_NO) //���ۿ��� ���۰����� ��ġ�� ã�´� 
					{
						continue;
					}
				}
				else if(n_PickPlace_Mode == WORK_PLACE)
				{
					if(st_buffer_info[n_BuffSite].st_pcb_info[nBuff_x].nYesNo == CTL_YES) //���ۿ��� ���۰����� ��ġ�� ã�´� 
					{
						continue;
					}
				}	

				nFirstBuffer_Num = -1; //clear 
				nPicker_Cnt = 0; nBuffer_Cnt = 0; //clear 
				nClash_Buffer_Flag = 0;
				//npBuffInfo_Count[nBuff_x] = 0; //clear 
				for(nPicker_x = 0; nPicker_x < TEST_PICKER_PARA; nPicker_x++) //TEST_PICKER_PARA == 4 //for(nPicker_x = m_nFirstPicker_Num; nPicker_x < TEST_PICKER_PARA; nPicker_x++) //TEST_PICKER_PARA == 4
				{
					npPicker_YesNo[nPicker_x] = CTL_NO; //�۾��� ��Ŀ���� clear�ϱ� 0���� for������ �ʱ�ȭ ���� 
					//if(nPicker_x < nFirstPicker_Num) continue; //�۾����� ��Ŀ�� ������ üũ�Ͽ� ���� ����� �浹�ϸ� �������� ���� 

					nFindPos = nBuff_x + ((nPicker_x - nFirstPicker_Num) * 2);//james 2016.0927 
					if(nFindPos >= 0 && nFindPos < st_recipe_info.nRetestBuffer_Num) //james 2016.0927 > 0
					{			
						if(n_PickPlace_Mode == WORK_PICK)
						{	//pick ��� ���� ã�� 
							if(st_picker[THD_TEST_RBT].st_pcb_info[nPicker_x].nYesNo == CTL_YES)//������ ��ġ�� ���ۿ� �浹�� üũ�Ѵ� 
							{
								if(st_buffer_info[n_BuffSite].st_pcb_info[nFindPos].nYesNo == CTL_NO) //������ ������ ��ĭ�ǳ� �ϳ��� üũ, ���� ������� ��Ŀ����� 1/2 �� 1:1�� ��ġ�����ʾ� ���� �ʿ�.
								{
									//�����ص� �浹���� 
								}
								else if(st_buffer_info[n_BuffSite].st_pcb_info[nFindPos].nYesNo == CTL_YES) 
								{	//���� ��ġ���� �����ϸ� ���簣 �浹�߻�
									nClash_Buffer_Flag = -100;
									nPicker_x = TEST_PICKER_PARA;
									break;
								}
							}
							else if(npPickerWork_Enable[nPicker_x] == CTL_YES && st_picker[THD_TEST_RBT].st_pcb_info[nPicker_x].nYesNo == CTL_NO)//����̽��� ������ 
							{
								//npPicker_YesNo[nPicker_x] = CTL_YES;
								nPicker_Cnt++;
								if(st_buffer_info[n_BuffSite].st_pcb_info[nFindPos].nYesNo == CTL_YES) //������ ������ ��ĭ�ǳ� �ϳ��� üũ, ���� ������� ��Ŀ����� 1/2 �� 1:1�� ��ġ�����ʾ� ���� �ʿ�.
								{
									//��Ŀ�� ������ ������ ����. ���� �� �� �ִ� 
									if(nFirstBuffer_Num == -1)
									{
										nFirstBuffer_Num = nBuff_x;										
									}

									npPicker_YesNo[nPicker_x] = CTL_YES;

									nBuffer_Cnt++;
									npBuffInfo_Count[nBuff_x]++;//��� �۾� ���� ���� 
									//npBuffInfo_Count[17]++; //2016.0929 
								}
								else if(st_buffer_info[n_BuffSite].st_pcb_info[nFindPos].nYesNo == CTL_NO) //������ ������ ��ĭ�ǳ� �ϳ��� üũ, ���� ������� ��Ŀ����� 1/2 �� 1:1�� ��ġ�����ʾ� ���� �ʿ�.
								{//�������� 
								}														
							}
						}
						else if(n_PickPlace_Mode == WORK_PLACE)
						{//retest buffer dvc place ��� ���� ã�� 
							if(st_picker[THD_TEST_RBT].st_pcb_info[nPicker_x].nYesNo == CTL_YES)//������ ��ġ�� ���ۿ� �浹�� üũ�Ѵ� 
							{								
								//npPicker_YesNo[nPicker_x] = CTL_YES;
								nPicker_Cnt++;
								if(st_buffer_info[n_BuffSite].st_pcb_info[nFindPos].nYesNo == CTL_NO) //������ ������ ��ĭ�ǳ� �ϳ��� üũ, ���� ������� ��Ŀ����� 1/2 �� 1:1�� ��ġ�����ʾ� ���� �ʿ�.
								{
									//��Ŀ�� ������ ������ ����. ���� �� �� �ִ� 
									if(nFirstBuffer_Num == -1)
									{
										nFirstBuffer_Num = nBuff_x;										
									}

									npPicker_YesNo[nPicker_x] = CTL_YES;

									nBuffer_Cnt++;
									npBuffInfo_Count[nBuff_x]++;//��� �۾� ���� ���� 
									//npBuffInfo_Count[17]++; //2016.0929 
								}
								else if(st_buffer_info[n_BuffSite].st_pcb_info[nFindPos].nYesNo == CTL_YES) 
								{	//���� ��ġ���� �����ϸ� ���簣 �浹�߻�
									nClash_Buffer_Flag = -100;
									nPicker_x = TEST_PICKER_PARA;
									break;
								}
							}
							else if(st_picker[THD_TEST_RBT].st_pcb_info[nPicker_x].nYesNo == CTL_NO)//����̽��� ������ 
							{
								//�������� 														
							}
						}//else if(n_PickPlace_Mode == WORK_PLACE)
					}//if(nFindPos >= 0 

				} //for(nPicker_x = 0;

				if(nClash_Buffer_Flag == 0 && nBuffer_Cnt > 0 && nPicker_Cnt == nBuffer_Cnt) //���縦 ���ۿ� �ѹ��� ���� �� �ִ� ��ġ�� �ִ� //���� OK
				{	
					if( 1 )//nFuncRet == RET_ABORT)
					{
						np_FindPosYXCPB[0] = nFirstBuffer_Num;
						np_FindPosYXCPB[1] = nFirstBuffer_Num;
						np_FindPosYXCPB[2] = nBuffer_Cnt;
						np_FindPosYXCPB[3] = nFirstPicker_Num;
						np_FindPosYXCPB[4] = nFirstBuffer_Num;

						for(nPicker_x = 0; nPicker_x < TEST_PICKER_PARA; nPicker_x++)
						{
							np_Picker_YesNo[nPicker_x] = npPicker_YesNo[nPicker_x]; //��� �۾� ���� ���� 
						}		

						for(i=0; i < 20; i++) //james 2016.0929 st_recipe_info.nRetestBuffer_Num; i++)
						{
							np_BuffInfo_Count[i] = npBuffInfo_Count[i];
						}

						//nFuncRet = RET_GOOD;
						return RET_GOOD;
					}
				}		
				else if(nClash_Buffer_Flag == 0 && nBuffer_Cnt > 0)
				{
					if(nRetestSkip_flag == 0)
					{
						np_FindPosYXCPB[0] = nFirstBuffer_Num;
						np_FindPosYXCPB[1] = nFirstBuffer_Num;
						np_FindPosYXCPB[2] = nBuffer_Cnt;
						np_FindPosYXCPB[3] = nFirstPicker_Num;
						np_FindPosYXCPB[4] = nFirstBuffer_Num;

						for(nPicker_x = 0; nPicker_x < TEST_PICKER_PARA; nPicker_x++)
						{
							//np_Picker_YesNo[nPicker_x] = npPicker_YesNo[nPicker_x]; //����� ��Ŀ ���� ��� 
						}		

						nRetestSkip_flag++;
						nFuncRet = RET_SKIP;
					}							
				}
				else //if(nClash_Buffer_Flag == -100)//if(nClash_Buffer_Flag == 0 
				{
					npBuffInfo_Count[nBuff_x] = 0;	//
				}					//if
			}//for(nBuff_x = nBuffFirstNo;
		}//for(nBuffFirstNo = 0
		break;
	}
	for(i=0; i < 20; i++) //james 2016.0929 st_recipe_info.nRetestBuffer_Num; i++)
	{
		np_BuffInfo_Count[i] = npBuffInfo_Count[i];
	}
	return nFuncRet;
}

int CPublicFunction::OnScrapCodeFind(int nMode, CString strLoadFile)
{
	TCHAR chData[100];

	int nCount;
	int nCode;
	int nCurr, nNext;
	int i;

	CString strTemp;
	CString strHead;
	CString strData;

	GetPrivateProfileString(st_basic_info.strScrapHead, _T("Total"), _T("0"), chData, sizeof(chData), strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nCount = _wtoi(strTemp);

	if (nCount > 0)
	{
		for (i=0; i<nCount; i++)
		{
			strHead.Format(_T("%d"), i+1);
			GetPrivateProfileString(st_basic_info.strScrapHead, strHead, _T("0"), chData, sizeof(chData), strLoadFile);
			strData.Format(_T("%s"), chData);
			
			nCode = _wtoi(strData.Mid(0, 4));
			nCurr = _wtoi(strData.Mid(5, 1));
			nNext = _wtoi(strData.Mid(7, 1));

			st_code_info[nMode].m_nScrapCode[nCode][0] = nCurr;
			st_code_info[nMode].m_nScrapCode[nCode][1] = nNext;
		}
	}
	else
	{
		return RET_ERROR;
	}

	return RET_GOOD;
}


int	 CPublicFunction::LoadRegModelData()
{
	int nFuncRet = RET_ERROR;
	int nTotal = 0;
	FILE    *fp ;
	int     existence;
	char chr_data[100];
	CString strMsg;
	
	CString str_LoadFile = _T("C:\\AMT840\\File\\ModelName.ini");

	clsFunc.OnStringToChar(str_LoadFile, chr_data);
	
	//existence = access(str_LoadFile, 0);
	existence = _access(chr_data,0);
	
	if (!existence)
	{
		//if ((fp=fopen(str_LoadFile,"rb")) == NULL)
		fopen_s(&fp, chr_data, "rb");
		if(!fp)
		{
			if (st_handler_info.cWndList != NULL)
			{
				strMsg.Format( _T("[%s] file open error."), str_LoadFile);
				clsMem.OnNormalMessageWrite(strMsg);
			}
			st_work_info.m_nMdlTotal = 0;
			return nFuncRet;
		}
	}
	else
	{
		if (st_handler_info.cWndList != NULL)
		{
			strMsg.Format( _T("[%s] file is not exist."), str_LoadFile);
			clsMem.OnNormalMessageWrite(strMsg);
		}
		st_work_info.m_nMdlTotal = 0;
		return nFuncRet;
	}
	:: GetPrivateProfileString(_T("Model_Name"), _T("Total"), _T("0"), (LPWSTR)chr_data, 100, str_LoadFile);
	nTotal = atoi(chr_data);
	
	st_work_info.m_nMdlTotal = nTotal;
	
	CString str,strLabel;
	for (int i = 0; i < nTotal; i++)
	{
		str.Format(_T("%d"),i+1);
		:: GetPrivateProfileString(_T("Model_Name"), str, _T("0"), (LPWSTR)chr_data, 100, str_LoadFile);
		strLabel.Format(_T("%s"), chr_data);
		strLabel.TrimLeft(' ');               
		strLabel.TrimRight(' ');
		st_work_info.m_strModelName[i] = strLabel;
	}
	fclose(fp);

	nFuncRet = RET_GOOD;

	return nFuncRet;
}

int	 CPublicFunction::GetModelFromPartID( CString PartID, CString& strModel)
{
	int nFuncRet = RET_ERROR, nRet = 0, nMdl = 0;
	int nItemLength = 0;
	CString strMsg;

	PartID.TrimLeft(' ');	
	PartID.TrimRight(' ');

	nRet = LoadRegModelData();
	if( nRet == RET_GOOD)
	{
		nItemLength = 0;
		nMdl = GetModelNumFromModel(st_work_info.m_strCurModel);
		nRet = ModelFileload(st_work_info.m_strCurModel);
		if( nMdl>=0 && nRet == RET_GOOD && nItemLength > 0) 
		{
			for (int ii = 0; ii < nItemLength; ii++)
			{
				m_strModel[nMdl][ii] = m_strItemValue[ii];
				if(m_strItemValue[ii].Compare( (LPCTSTR)PartID) == 0)
				{
					strModel = st_work_info.m_strCurModel;
					nFuncRet = RET_GOOD;
					if (st_handler_info.cWndList != NULL)  // ����Ʈ �� ȭ�� ����
					{
						strMsg.Format( _T("%s Model is same"), st_work_info.m_strCurModel);
						clsMem.OnNormalMessageWrite(strMsg);
					}

					break;
				}
			}
		}

		if(nFuncRet != RET_GOOD)
		{
			for (int i= 0; i < st_work_info.m_nMdlTotal; i++ )
			{
				nItemLength = 0;
				nRet = ModelFileload(st_work_info.m_strModelName[i]);
				if( nRet == RET_GOOD && nItemLength > 0) 
				{
					for (int ii = 0; ii < nItemLength; ii++)
					{
						m_strModel[i][ii] = m_strItemValue[ii];
						if(m_strItemValue[ii].Compare( (LPCTSTR)PartID) == 0)
						{
							strModel = st_work_info.m_strModelName[i];
							nFuncRet = RET_GOOD;
							if (st_handler_info.cWndList != NULL)
							{
								strMsg.Format( _T("%s Model is different"), strModel);
								clsMem.OnNormalMessageWrite(strMsg);
							}
							break;
						}
					}
				}
			}
		}	
	}
	return nFuncRet;
}

int CPublicFunction::ModelFileload(CString strMdl)
{
	int nFunRet = RET_ERROR;
	CString strFilePath;
	strFilePath = _T("C:\\AMT840\\File\\ModelName\\");
	strMdl.TrimLeft(); strMdl.TrimRight();
	CString strFile = strMdl + _T(".TXT");
	//FileRead(strFilePath + strFile);
	
	int nTotal = 0, i = 0;
	int nItemLength;

	FILE    *fp ;
	int     existence;
	char chr_data[100];	
	
	strFilePath += strFile;
	
	//existence = access(strFilePath, 0);
	OnStringToChar(strFilePath, chr_data);
	existence = _access(chr_data, 0);

	if (!existence)
	{
		//if ((fp=fopen(strFilePath,"rb")) == NULL)
		fopen_s(&fp, chr_data, "rb");
		if(!fp)
		{
			return nFunRet;
		}
	}
	else
	{
		//m_nMdlTotal = 0;
		return nFunRet;
	}
	
	for(int i = 0; i < MAX_LINE; i++)  
	{
		m_strItemValue[i].Empty();
	}
	
	
	:: GetPrivateProfileString(strMdl, _T("Total"), _T("0"), (LPWSTR)chr_data, 100, strFilePath);
	nTotal = atoi(chr_data);
	
	nItemLength = nTotal;
	
	CString str,strLabel;
	for (i = 0; i < nTotal; i++)
	{
		str.Format(_T("%d"),i+1);
		:: GetPrivateProfileString(strMdl, str, _T("0"), (LPWSTR)chr_data, 100, strFilePath);
		strLabel = chr_data;
		strLabel.TrimLeft(); strLabel.TrimRight();
		m_strItemValue[i] = strLabel;
	}
	fclose(fp);
	nFunRet = RET_GOOD;

	return nFunRet;
}

int CPublicFunction::GetModelNumFromModel( CString strModel)
{
	int nFuncRet = -1;
	int nRet = RET_ERROR;
	nRet = LoadRegModelData();
	if(nRet == RET_GOOD)
	{
		for ( int i = 0; i < st_work_info .m_nMdlTotal; i++ )
		{
			if( st_work_info.m_strModelName[i].Compare( (LPCTSTR) strModel) == 0 )
			{
				nFuncRet = i;
				break;
			}
		}
	}
	return nFuncRet;
}

void	CPublicFunction::	RunRegPartID()
{
	CWnd* m_pWnd = CWnd::FindWindow(NULL,_T("ModelChange"));
	if(m_pWnd){
		m_pWnd->SetForegroundWindow(); 	
		m_pWnd->ShowWindow(SW_RESTORE);	
	}
	else{
		::ShellExecute(NULL, NULL, _T("C:\\AMT840\\bin\\ModelChange.exe"), NULL, NULL, SW_SHOWNORMAL);
	}
}

void CPublicFunction::SendRegPartID()
{
	COPYDATASTRUCT cds;
	
	st_copydata = new st_copy_data;
	
	st_copydata->mn_mdl = 19;
	
	memset(st_copydata->str_partid, 0x00, sizeof(st_copydata->str_partid));
	//sprintf(st_copydata->str_partid, _T("%s"), (LPCTSTR) st_lot_info[LOT_CURR].strPartNo);
	OnStringToChar((LPCTSTR) st_lot_info[LOT_CURR].strPartNo, st_copydata->str_partid);
	
	cds.dwData = WM_MDL_PART_DATA;
	cds.cbData = sizeof(st_copy_data);/*(sizeof(st_copy_data))*///(DWORD)&st_copydata;
	cds.lpData = st_copydata;
	
	
	HWND hwnd = ::FindWindow(NULL,_T("ModelChange"));
	if (hwnd!= NULL)
	{
		HANDLE handle = AfxGetInstanceHandle();
		::SendMessage(hwnd, WM_COPYDATA, (WPARAM)handle, (LPARAM)&cds);
	}
	else
	{

//		CDialog_Message msg_dlg;
//		msg_dlg.m_strMessage = _T("ModelChange.exe can't find program(C:\\AMT840\\BIN\\ModelChange.exe");		
//		msg_dlg.DoModal();

		st_other_info.strBoxMsg = _T("ModelChange.exe can't find program(C:\\AMT840\\BIN\\ModelChange.exe");
		if (st_handler_info.cWndMain != NULL)
		{
			st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
		}
	}
	
	delete st_copydata;	
}

