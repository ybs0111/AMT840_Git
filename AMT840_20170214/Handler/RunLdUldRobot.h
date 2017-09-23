#pragma once


// CRunLdUldRobot

class CRunLdUldRobot :  public CObject
{
public:
	CRunLdUldRobot();
	virtual ~CRunLdUldRobot();

public:
	void			OnThreadRun(void);
	void			OnRunInit(void);
	void			OnRunMove(void);

	void Set_WorkRbtPicker_UpDown( int nOnOff );
	int Chk_WorkRbtPicker_UpDown( int nOnOff );
	void Set_WorkRbtRotator_OnOff( int nOnOff );
	int Chk_WorkRbtRotator_OnOff( int nOnOff );// On : 90도 회전 Off : 0도
	void Set_Glipper_OnOff( int nOnOff );
	int Chk_Glipper_OnOff( int nOnOff );

public:
	CString m_strAlarmCode;
	bool m_bPickerDownFlag;
	DWORD m_dwWaitPickerUpDown[3];
	bool m_bRotatorOnOffFlag;
	DWORD m_dwWaitRotatorOnOff[3];
	bool m_bGlipperOnOffFlag;
	DWORD m_dwGlipperOnOff[3];

	///////////////////////////////////////////////////////////////////////
	//james 2016.0804 저장할 목록
	///////////////////////////////////////////////////////////////////////

	int Process_DVC_Pick(int nMode, int nWork_Site);
	int Process_DVC_Place(int nMode, int nWork_Site, int nTestSitePos);

	//james 2016.0919 추가 
	int				 m_npPicker_OutputStatus[4];//james 2016.0919 그리퍼의 output 상태를 체크 
	//james 2016.0912 lot이 LOT_CURR, LOT_NEXT 둘중 이나이다 
	int				m_nLotProcessNum; 
	CString			m_strFindLotNo;
	int				m_nFindLotNo_Flag;
	///////// 2016.0912 
	DWORD			m_dwBarcodeWaitTime[3];
	int				m_nRetry;
	int				m_nBarcode1dStep;
	int				m_nBarcode2dStep;
	int				Process_Barcode1D(int nWork_Site, int nX, int nY);
	int				Process_Barcode2D(int nWork_site, int nX, int nY);
	
	DWORD			m_dwWaitTime[3];

	int				m_nInitStep;
	int				m_nRunStep;  

	int				m_nPick_Step;
	int				m_nPlace_Step;

	double			m_dCurrentPos[100];//x,y,z

	int				m_nDvcRepickCount;
	int				m_nPartialPickCount;
	bool			m_bDvcWaitChk_Falg;
	int				m_nPickRetry_Cnt;

	int				m_npTemp_Picker_YesNo[4];  //임시 작업가능피커 정의 
	int				m_npPicker_YesNo[4];		//작업 가능 피커 정의 
	int				m_npPicker_Vacuum_Status[4];//현재 백큠 상태 정보 
	int				m_npPicker_Data_Status[4]; //디바이스 수량등 정보 
	int				m_npPicker_Error_Status[4];//피커 down후 트레이에서 집을때 백큠 에러 정보 

	int				m_nClash_ErrorInfoStatus[4];

 

	int				m_nWorkRobot_uldSite_SortJob_THD_Pos;	 //THD_REJECT_OUT_1_STACKER 등 THD 정보임 
	int				m_nWorkRobot_Current_SortBin;    //현재 로봇이 소팅할 빈 정보


	DWORD			m_dwWaitUntil[3];
	DWORD			m_dwVacuumWaitTime[3];

	int				m_n_FirstPicker_Num;
	int				m_n_FirstTray_X_Num;
	int				m_n_FirstTray_Y_Num;

	//int				m_n_FirstBuff_X_Num;
	//int				m_n_FirstBuff_Y_Num;

	
	int				m_nPickerPara;
	int				m_nRobotSite;

	int				m_npFindWorkPosYXCPB[5]; //james 2015.0807 처음 찾을때의 정보,  [0]first tray Y, [1]first tray X, [2]:작업가능 수량 또는 공간 정보, [3]first picker num, [4]first buff num
	int				m_npSet_WorkPosYXCPB[5]; //james 2015.0807 작업 완료후 작업 셋팅 위치 정보,  [0]first tray Y, [1]first tray X, [2]:작업가능 수량 또는 공간 정보, [3]first picker num, [4]first buff num

	int				m_nLdRobot_Work_Pos_Num; //load1 = 0, load2 tray = 1 , load buffer = 2

	int				m_nMove_Flag[5]; //모터 이동 플레그 
	int				m_nRotator_Flag;
	int				m_nXYRobot_PreMove_Flag;//미리 작업 위치로 이동하는 플레그 

	int				m_nLoadBuff_Job_Continue_Flag; //로딩 플레이트에서 집어 버퍼에 공급중인지 확인하는 플레그, 로딩 작업을 이어서 계속 진행하기 위해 추가, 언로딩 자재가 와도 로딩을 끝낸후 처리한다 

	int				m_nAxisNum;        //현재의 IndexNum에서 사용하는 모터 실재 번호를 가진다 
	double			m_dpTargetPos;
	int				m_nWork_Site_Pos; //작업 방법, pick or place 

	double          m_dTemp_CalTargetPos_Y;
	double          m_dTemp_CalTargetPos_X;

	int				m_nLinearMove_Index;   //현재 map index 
	long			m_lAxisCnt;            //현재의 IndexNum에서 사용 가능한 모터 수량 최대 4개  
	long			m_lpAxisNum[4];        //현재의 IndexNum에서 사용하는 모터 실재 번호를 가진다 
	double			m_dpTargetPosList[4];		   //최대 4 포인트를 한번에 이동가능하다 
 	double			m_dpSpdRatio[3];			//속도비율 [0]:Vel, [1]:Acc, [2]:Dec
	int				m_nRobot_X;
	int				m_nRobot_Y;
	int				m_nRobot_Z; 

	int				mn_BcrFirst;
	int				mn_BcrFirstChk;
	int				mn_BcrFirstFinal;

	//2016.1203
	CString			m_strLotNo[2];
	CString			m_strPartNo[2];	
	DWORD			m_dwCycleTime[2][3];

	//kwlee 2017.0905
	int            m_nProduct_Cnt;
	 
};
extern CRunLdUldRobot clsRunLdUldRobot;

