#pragma once

// CLdTrayInputShifterConveyor 명령 대상입니다.
enum
{
	CV_LEFT,
	CV_RIGHT,
};

class CRunLdTrayInputShifterConveyor : public CObject
{
public:
	CRunLdTrayInputShifterConveyor();
	virtual ~CRunLdTrayInputShifterConveyor();

public:
	void			OnThreadRun(void);
	void			OnRunInit(void);
	void			OnRunMove(void);

	void			Set_ConveyorStopper_UpDown( int nDir, int nOnOff );//nDir : Left, Right stopper
	int				Chk_ConveyorStopper_UpDown( int nDir, int nOnOff );

	//james 2016.0802 
	void			Process_Tray_Conveyor_AcMotor(int nLR_Mode);// conveyor의 ac motor를 제어한다
	void			Process_LoadTray_Insert(int nLR_Mode);

public:
	///////////////////////////////////////////////////////////////////////
	//2015.0225 저장할 목록
	///////////////////////////////////////////////////////////////////////
	int				m_nInitStep;
	int				m_nRunStep;
	int				m_nInsertStep[2]; //left/right 
	int				m_nFtpSite;
	int				m_nFtpRetry;				// scrap code retry 수량.....
	int				m_New_lot_in;


	CString      m_strAlarmCode;	
	DWORD		m_dwWaitTime[3];
	DWORD		m_dwWaitStopperUpDown[2][3];//[2] : Left,Right
	DWORD			m_dwFtpWaitTime[3];
	bool			m_bStopperUpDownFlag[2];

	//james 2016.0802	 
	int				m_nAxisNum;        //현재의 IndexNum에서 사용하는 모터 실재 번호를 가진다 
	double			m_dpTargetPos;

	int				m_nShiftMotor_Work_Site; //모터 작업 순서 정의 0(left)->1(right)->0->1 반복 

	bool			m_bAcMotorFlag[2];
	DWORD			m_dwAcMotorWaitTime[2][3];
	bool			m_bTrayInFlag[2];
	DWORD			m_dwTrayInWaitTime[2][3];
	bool			m_bTrayPassExistFlag[2];
	DWORD			m_dwTrayPassExistWaitTime[2][3];
	bool			m_bAcMotorPassFlag[2];
	DWORD			m_dwAcMotorPasstWaitTime[2][3];

	bool			m_bCVTrayStackerArrivedFlag;
	DWORD			m_dwCVTrayStackerArrivedWaitTime[3];


	DWORD			m_dwNoInputWaitTime[3];//2016.1228

 	//2016.1203
	CString			m_strLotNo;
	CString			m_strPartNo;	
	DWORD			m_dwCycleTime[3];
	
 
};
extern CRunLdTrayInputShifterConveyor clsRunLdTrayInputShifterConveyor;

