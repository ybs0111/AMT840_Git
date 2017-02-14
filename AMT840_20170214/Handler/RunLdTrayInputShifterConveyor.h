#pragma once

// CLdTrayInputShifterConveyor ��� ����Դϴ�.
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
	void			Process_Tray_Conveyor_AcMotor(int nLR_Mode);// conveyor�� ac motor�� �����Ѵ�
	void			Process_LoadTray_Insert(int nLR_Mode);

public:
	///////////////////////////////////////////////////////////////////////
	//2015.0225 ������ ���
	///////////////////////////////////////////////////////////////////////
	int				m_nInitStep;
	int				m_nRunStep;
	int				m_nInsertStep[2]; //left/right 
	int				m_nFtpSite;
	int				m_nFtpRetry;				// scrap code retry ����.....
	int				m_New_lot_in;


	CString      m_strAlarmCode;	
	DWORD		m_dwWaitTime[3];
	DWORD		m_dwWaitStopperUpDown[2][3];//[2] : Left,Right
	DWORD			m_dwFtpWaitTime[3];
	bool			m_bStopperUpDownFlag[2];

	//james 2016.0802	 
	int				m_nAxisNum;        //������ IndexNum���� ����ϴ� ���� ���� ��ȣ�� ������ 
	double			m_dpTargetPos;

	int				m_nShiftMotor_Work_Site; //���� �۾� ���� ���� 0(left)->1(right)->0->1 �ݺ� 

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

