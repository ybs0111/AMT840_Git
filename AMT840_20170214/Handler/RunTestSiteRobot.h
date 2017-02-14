#pragma once


// CRunTestSiteRobot

class CRunTestSiteRobot : public CObject
{
public:
	CRunTestSiteRobot();
	virtual ~CRunTestSiteRobot();

public:
	void			OnThreadRun(void);
	void			OnRunInit(void);
	void			OnRunMove(void);

	int Chk_TestSiteRobotAllGlipper( int nOnOff ); //피커 Glipper를 전부 ON되어 있는지 전부 OFF되어 있는지 체크한다
	void Set_TestSiteRobotGlipper_OnOff( int *npPickerInfo, int nOnOff ); //james 2016.0807 void Set_TestSiteRobotGlipper_OnOff( int nPick, int nOnOff );//nPick : 피커 넘버(몇번째 피커인지)
	int Chk_TestSiteRobotGlipper_OnOff( int *npPickerInfo, int nOnOff ); //james 2016.0807 int Chk_TestSiteRobotGlipper_OnOff( int nPick, int nOnOff );//nPick : 피커 넘버

	int TestSite_PickPlace_SocketData_LoadBuff_Map_Combine(int nMode, int nTestSite_THD_Num, int nTestSocket_0_4_Start_Pos); //소케오프 유무를 확인후 로딩 버퍼에 해당 정보와 같이 로딩 자재 공급을 요청 한다 

	int Process_DVC_Pick(int nMode, int nWork_Site, int nTest_Site, int nStart_FixPos); //int nStart_FixPos = Left(0), Right(4) 두가지 위치중 하나이다 , 무조건 동작기준 죄측부터 작업 
	int Process_DVC_Place(int nMode, int nWork_Site, int nTest_Site, int nStart_FixPos, int Opt=0);//2016.1128 Process_DVC_Place(int nMode, int nWork_Site, int nTest_Site, int nStart_FixPos);

	int Process_BinSort_Place(int nMode, int nWork_Site, int nSortBin); //james 2016.0814 retest bin 소팅 

public:	
	int				m_nRobot_Z_Stop_Flag;
	int				m_nRetestBuffer_PickPlace_Mode; //0:한번에 놓는다, 1:파셜일때 더 찾아 집고 또는 놓을 수 있다 
	int				 m_npPicker_OutputStatus[4];//james 2016.0919 그리퍼의 output 상태를 체크 

	DWORD			m_dwWaitTime[3];

	int				m_nInitStep;
	int				m_nRunStep;  

	int				m_nPick_Step;
	int				m_nPlace_Step;
	int				m_nSortPick_Step; //james 2016.0814 언로딩 리테스트 빈 처리
	int				m_nSortPlace_Step;

	int				m_nLinearMove_Index;   //현재 map index 
	long			m_lAxisCnt;            //현재의 IndexNum에서 사용 가능한 모터 수량 최대 4개  
	long			m_lpAxisNum[4];        //현재의 IndexNum에서 사용하는 모터 실재 번호를 가진다 
	double			m_dpTargetPosList[4];		   //최대 4 포인트를 한번에 이동가능하다 
 	double			m_dpSpdRatio[4];			//속도비율 [0]:Vel, [1]:Acc, [2]:Dec
	int				m_nRobot_X;
	int				m_nRobot_Y;
	int				m_nRobot_Z; 
	int				m_nRobot_Ejector; 

	int				m_nSyncTestCount;//한번에 로딩 언로딩할 작업 갯수 
	int				m_npGetSiteInfo[10]; 

	int				m_nDvcRepickCount;
	int				m_nPartialPickCount;
	bool			m_bDvcWaitChk_Falg;
	int				m_nPickRetry_Cnt;
	int				m_nDirRetry_Cnt;//2016.1201
	double			m_dwDirOffset;//2016.1201

	int				m_nBuffer_Work_Cnt;//james 2016.0929
	int				m_nFirstPicker_Num;
	int				m_nFirstTray_X_Num;
	int				m_nFirstTray_Y_Num;

	int				m_nFirstBuff_X_Num;
	int				m_nFirstBuff_Y_Num;
	 
//	int				m_nSite_WorkSize_Num; //공통으로사용하기 위해 추가  //버퍼,테스트사이트등 작업사이즈 작업 사이트 정보에따라 임의로 저장하여 사용
	int				m_nFirst_Buff_Num; //공통으로사용하기 위해 추가 , //버퍼, 소켓등 정보 확인에 사용 

	int				m_nPartial_FirstRetestBuffPos_Select_No; //리테스트 자재 위치가 테스트 사이트의 정보와 맞는 자재가 공간에 없을때 작업할 버퍼 위치를 선정하고 이곳에 부족한 부분의 자재를 채워준 후 테스트 사이트에 리테스트 빈을 공급한다  

	//2016.0814 
	int				m_npBuffer_YesNo[4];		//작업 가능 피커 정의 [4];		//작업 가능 피커 정의 
	int				m_npBuff_Status[4];
	int				m_npBuff_OutputStatus[4];//james 2016.0919 그리퍼의 output 상태를 체크 

	int				m_npTemp_Picker_YesNo[4];  //임시 작업가능피커 정의 
	int				m_npPicker_YesNo[4];		//작업 가능 피커 정의 
	int				m_npPicker_Vacuum_Status[4];//현재 백큠 상태 정보 
	int				m_npPicker_Data_Status[4]; //디바이스 수량등 정보 
	int				m_npPicker_Error_Status[4];//피커 down후 트레이에서 집을때 백큠 에러 정보 

	double			m_dCurrentPos[100];//x,y,z, eject
	
	int				m_np_BuffInfo_Count[20]; //james 2016.0929 [16]:피커 작업 가능 수량, [17]: 버퍼 총 수량  [15]; //
	int				m_nMove_Flag[5]; //모터 이동 플레그 
	int				m_npFindWorkPosYXCPB[5]; //james 2015.0807 처음 찾을때의 정보,  [0]first tray Y, [1]first tray X, [2]:작업가능 수량 또는 공간 정보, [3]first picker num, [4]first buff num
	int				m_npSet_WorkPosYXCPB[5]; //james 2015.0807 작업 완료후 작업 셋팅 위치 정보,  [0]first tray Y, [1]first tray X, [2]:작업가능 수량 또는 공간 정보, [3]first picker num, [4]first buff num

	int				m_nTestSite_THD_Work_Site; //THD NUM, 어느 테스트 사이트와 작업하기 위해 움직이는지 확정
	int				m_nBinSort_THD_Work_Site; //테슽가 끝나 집고있는 자재를 소팅하는 영역

	double          m_dTemp_CalTargetPos_Y;
	double          m_dTemp_CalTargetPos_X;

	DWORD			m_dwWaitUntil[3];
	DWORD			m_dwVacuumWaitTime[3];
	int				m_nPickerPara;
	int				m_nRobotSite;


	int				m_nSortPickerInfo[4];
	//int				m_nSortPicker_First;
//	int				m_nSortBuffer_First;
	int				m_nSort_Count;

	int				m_nLotProcessNum; //james 2016.0911 lot이 LOT_CURR, LOT_NEXT 둘중 이나이다 
	CString			m_strFindLotNo;
	int				m_nFindLotNo_Flag;

	CString			m_strSortDvc_LotNo; //현재 소팅하려는 자재의 lot 정보 

	int				m_nClash_ErrorInfoStatus[4];



	CString m_strAlarmCode;
	bool m_bGlipperOnOffFlag;
	DWORD m_dwGlipperOnOff[3];

	//2016.1203
	DWORD			m_dwPlaceTime[3];
	DWORD			m_dwCycleTime[2][3];
	CString			m_strLotNo[2];
	CString			m_strPartNo[2];
};
extern CRunTestSiteRobot clsRunTestSiteRobot;


