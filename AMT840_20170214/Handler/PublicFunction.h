#include "Variable.h"
#include <afxtempl.h>

#pragma once

class CPublicFunction : public CObject
{
public:
	int				Find_RetestBuffer_PickPlace_WorkPos_Check(int n_PickPlace_Mode, int n_TestSite, int nTestFixPos, int n_BuffSite, int *np_Picker_YesNo, int *np_FindPosYXCPB,  int *np_BuffInfo_Count);//james 2016.0928 retest buffer 작업가능 위치 찾기 
	int				Set_LotInfo_LotStartData(int n_LotMode, CString str_LotNo); //james 2016.0921 lot start 시 셋팅할 내용을 정리한다 
	int				Set_LotInfo_LotEndDataExchange(int nMode); //james 2016.0921 lot end 완료시 LOT_CURR ~ LOT_CURR_BACKUP 까지의 데이터를 교환한다 
	int				Find_AllLotEndStatus(int n_Mode);
	// jtkim 20160827 retest/reject/unloading tray 작업영역 결정
	int				Find_Work_Site(int nLotNum, CString strLotNo, CString strPartNo);

	int				Find_TestRobot_WherePos(int n_Mode, int *np_TestSite);//james 2016.0822 test robot이 테스트 사이트 어디에 있는지 확인한다(Test Z & Eject robot의 동작전에 로보트의 현재 위치를 정확히 파악하고 동작할때 사용, 

	int				Check_TestRobot_Pos();//james 2016.0822 test robot이 테스트 사이트 어디에 있는지 확인한다(Test Z & Eject robot의 동작전에 로보트의 현재 위치를 정확히 파악하고 동작할때 사용, 

	int				Find_RetestBinBuffer_WorkPos_Check(int n_Mode, int n_TestSite, int n_BuffSite, int n_PickerDvcYesNO,int n_BuffYesNO, int *np_Picker_YesNo, int *np_FindPosYXCPB,  int *np_BuffInfo_Count) ; //int *np_Picker_YesNo 의 정보에 맞는 우치를 찾아 동작한다 

	int				Find_RetestBufferWork_Pos(int n_RbtSite, int n_TestSite, int n_BuffSite, int n_BuffFirstNo, int n_SortBin, int n_PickerYesNO, int n_BuffYesNO, int *np_FindPosYXCPB, int *np_Picker_YesNo); //일단 사용하지 말자 james 2016.0815 버퍼의 작업 가능한 위치를 찾는다 

	// james 2016.0803 
	int				Find_Compare_Buffer_and_Picker(int nMode, int nRobotSite, int nBufferSite, int nRbtYesNo, int nBuffYesNO, int *npFindPosYXCPB); //[0]:y,[1]:y, [3]:count, [4]]first picker, [5]:first buffer. [2]:작업 가능 수량
 
	int				Calculate_MovePos_Find(int nMode, int nMotNum, int nSite, int nWorkPart, int *npFindWorkPosYXCPB, double *dpGetTargetPos); //james 2016.0822 

	int				Find_TrayWork_Pos(int nMode, int nSite, int nDVCYesNO, int *npFindPosYXCPB, int nRobotSite, CString str_LotNo); //x,y,picker first, bufer first, 전체 작업 가능한 수량, //james 2016.0812 인자 추가  

	int				Handler_Tray_DataInfo_Shift(int nMode, int nDvc_Yes_No, int nSend_SIte, int nRcv_Site); //장비 데이터를 물류 이동에 따라 이동 시킨다 

	int				Check_PickerStatus(int nPickerMode, int nSite, int nYesNo, int *nPickerInfo, int *nPickerStatus, int *npPickerOutputStatus);//james 2016.0919 , int *npPickerOutputStatus 그리퍼 output 상태 추가 
	int				Check_BufferStatus(int nBufferMode, int nSite, int nYesNo, int *nBufferInfo, int *nBufferStatus, int *npBufferOutputStatus); //james 2016.0919 추가 

	int				Data_Exchange_PickPlace(int nPickPlace, int nJobCnt, int nRobotSite, int nWorkSite, int *npWorkPos);

	int				Find_TestSite_Work_Pos(int nMode, CString strLotNo, int PickPlace_Mode, int nTestSyncCount, int *npGetTestSiteInfo);

	 int			Check_Robot_Buffer_Clash(int nTestDirection, int nRbtTHDSite, int TestBuffTHDSite, int nBuffStartPos, int nPickerStartPos, int *nErrorInfoStatus); //피커와 버퍼의 작업 위치간 공간의 작업 여부 가능 확인 한다
	 

	CString			m_strAlarmCode;

	/////////////////////////////////////////

	// jtkim 20160107
	void				OnDailyCycleData(CString strLotNo, CString strPartNo, double dTime, CTime tStart, CTime tEnd);
	// jtkim 20150929
	CString			OnRecipeCheck(CString strPartNo);
	int					OnRecipeMatch(CString strFileName, CString strPartNo);
	int					OnRecoveryReadWrite(int nMode, int nReadWrite);

	CFont			*m_pFont[20];

	CString			m_strThreadStep[100];
	CTime			m_tThreadTime[100];

	CArray			<tagLOT_HISTORY_INFO, tagLOT_HISTORY_INFO&>	m_ArLotHistory;

	int					m_safetyStep1;
	int					m_safetyStep2;
	int					m_nSafetyChk[6];
	DWORD			m_dwWaitSafetyTime[2][3];

public:
	CPublicFunction(void);
	~CPublicFunction(void);
	LOGFONT			OnLogFont(int nFontSize);
	void			OnLogFileAdd(int nMsgSelect, CString strMsg);
	int				OnFileSizeCheck(CString strFileName, long lSize, int nCheck);
	int				OnLightCurtainCheck();
	int				OnDoorOpenCheck();
	void			OnDoorOpen();			// jtkim 20130625
	void			OnDoorClose();			// jtkim 20130625
	int				OnMpCheck();
	int				OnEncoderCheck(double dPos);
	bool			OnCreateFolder(CString strPath);
	int				OnEmoCheck();
	int				OnMainAirCheck();
	int				OnMainPowerCheck();
	int				OnMotorPowerCheck();
	int				OnIoSafetyCheck(int nIO);
	void			OnLotStartClear();
	BOOL			IsNum(CString strData);
	// ************************************
	// 장비 상태 변경 함수이다. 
	// ************************************
	void			OnMCRun();				// jtkim 20130625
	void			OnMCStop();				// jtkim 20130625
	void			OnSound(int nOnOff);	// jtkim 20130625
	// ************************************
	void			OnLogBarcode(CString strMsg);
	void			OnLogBarcodeNg(CString strMsg);
	void			OnSerialize(CArchive &ar);
	void            OnTestDoorOpen();
	void            OnTestDoorClose();
	void            OnLdUldDoorOpen();
	void            OnLdUldDoorClose();
	void			OnDailyDataSave();
	void			OnHourDataSave(CString strHourName, int nQty, double dSocketYield, int nStart, int nOn, int nOff, CString strOff, CString strOn);
	void			OnTimeReset();
	void			OnInitVariable();		// jtkim 20130625
	void			OnIOPortClear();
	void			OnMotorSpeed();
	void			OnAxisEncoderReset(int nAxis);
	void			OnThreadFunctionStepTrace(int nPos, int nStep);
	CString			OnCharToString(char chData[], int nLength);
	void			OnStringToChar(CString strData, char chData[]);
	CString			ThreadID(int nID); //2015.02.10;

	int				OnLdUldLightCurtainCheck();
	int				OnRejectCurtainCheck();
	 int			EnableSocketSiteWorking( int n_Site );

	int				OnLdUldCurtainCheck();
	int				OnRejCurtainCheck();

	int				OnScrapCodeFind(int nMode, CString strLoadFile);

	//2016.1116
	int				LoadRegModelData();
	int				GetModelFromPartID( CString PartID, CString& strModel);
	int				ModelFileload(CString strMdl);
	int				GetModelNumFromModel( CString strModel);
	void			RunRegPartID();
	void			SendRegPartID();

	//2016.1203
	void			OnCycleTime(int nMode, CString strLotNo, CString strPartNo, DWORD dwTime1, DWORD dwTime2, DWORD dwTime3);
	void			OnLotCycleData(CString strLotNo, CString strPartNo,int nCount, DWORD dwTray, DWORD dwLot);

	int				Check_LastSiteCurrentSite( CString strLotNo );
	bool			CheckCurrLotEndNextLot();

public:
		CString m_strModel[100][500];
		CString m_strItemValue[500];
};

extern CPublicFunction clsFunc;
