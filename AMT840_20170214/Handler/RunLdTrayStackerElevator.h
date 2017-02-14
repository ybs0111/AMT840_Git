#pragma once

// CRunLdTrayStackerElevator 명령 대상입니다.

class CRunLdTrayStackerElevator : public CObject
{
public:
	CRunLdTrayStackerElevator();
	virtual ~CRunLdTrayStackerElevator();

public:
	void			OnThreadRun(void);
	void			OnRunInit(void);
	void			OnRunMove(void);


	void Set_LdStacker_UpDown( int nOnOff );
	int Chk_LdStacker_UpDown( int nOnOff );

	//////////////////////////////////////////////////////////////////////
	//james 2016.0802 저장할 목록
	///////////////////////////////////////////////////////////////////////
	int			Elevator_Job_Move_Pos(int nMode, int nMotNum,  int nTargetPos);

public:
	int				m_nLotProcessNum; //james 2016.0929 lot이 LOT_CURR, LOT_NEXT 둘중 이나이다 

	///////////////////////////////////////////////////////////////////////
	//james 2016.0802 저장할 목록
	///////////////////////////////////////////////////////////////////////
	int				m_nInitStep;
	int				m_nRunStep; 
	int				m_nStacker_MoveStep;

	int				m_nAxisNum;        //현재의 IndexNum에서 사용하는 모터 실재 번호를 가진다 
	double			m_dpTargetPos;

	DWORD			m_dwWaitTime[3];

	double			m_dReceive_Pos_Backup;		//기준점 잡은 위치 셋팅후 백업 
	double           m_dSupply_Pos_Backup;	//디바이스가 들어있는 unload tray를 받는 영역 
	double           m_dReference_Pos_Backup;	//기준점 잡은 위치 셋팅후 백업 

	double			m_dCurrentPos;
	double			m_dTargetPos;

	DWORD			m_dwSdWaitTime[3];
	bool			m_bSd_Move_Flag;

	///////////////////////////////////////////////////////////////////////
	// jtkim 20160902
	int				m_nRetry;

	CString m_strAlarmCode;
	bool m_bStackerUpDownFlag;
	DWORD m_dwWaitStackerUpDown[3];


};
extern CRunLdTrayStackerElevator clsRunLdTrayStackerElevator;


