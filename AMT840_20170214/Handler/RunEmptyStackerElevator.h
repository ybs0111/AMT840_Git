#pragma once

// CRunEmptyStackerElevator 명령 대상입니다.

class CRunEmptyStackerElevator : public CObject
{
public:
	CRunEmptyStackerElevator();
	virtual ~CRunEmptyStackerElevator();

public:
	void			OnThreadRun(void);
	void			OnRunInit(void);
	void			OnRunMove(void);

	void			Set_StackerTray_UpDown( int nOnOff );
	int				Chk_StackerTray_UpDown( int nOnOff );

	int				m_nLotProcessNum; //james 2016.0929 
	bool            m_bEmptyTrayCheck; //kwlee 2016.1221
	// jtkim 20160902
	int				m_nRetry;
public:
	CString m_strAlarmCode;
	DWORD m_dwWaitTime[3];
	DWORD m_dwWaitStackerUpDown[3];
	bool m_bStackerUpDownFlag;


	///////////////////////////////////////////////////////////////////////
	//james 2016.0802 저장할 목록
	///////////////////////////////////////////////////////////////////////
	int				m_nInitStep;
	int				m_nRunStep; 
	int				m_nStacker_MoveStep;

	int				m_nAxisNum;        //현재의 IndexNum에서 사용하는 모터 실재 번호를 가진다 
	double			m_dpTargetPos;

	int				m_nWork_Site_Pos; //작업 방법, pick or place 
	 
	

	double			m_dReceive_Pos_Backup;		//기준점 잡은 위치 셋팅후 백업 
	double           m_dSupply_Pos_Backup;	//디바이스가 들어있는 unload tray를 받는 영역 
	double           m_dReference_Pos_Backup;	//기준점 잡은 위치 셋팅후 백업 

	double			m_dCurrentPos;
	double			m_dTargetPos;

	DWORD			m_dwSdWaitTime[3];
	bool			m_bSd_Move_Flag;

	int				m_nTrayFull_Falg; //james 2016.0921 트레이 full시실린더를 다운하고 동작한다 

	///////////////////////////////////////////////////////////////////////
};
extern CRunEmptyStackerElevator clsRunEmptyStackerElevator;

