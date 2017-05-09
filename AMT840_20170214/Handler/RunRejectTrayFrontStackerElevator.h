#pragma once

// CRunRejectTrayFrontStackerElevator 명령 대상입니다.

class CRunRejectTrayFrontStackerElevator : public CObject
{
public:
	CRunRejectTrayFrontStackerElevator();
	virtual ~CRunRejectTrayFrontStackerElevator();

public:
	void			OnThreadRun(void);
	void			OnRunInit(void);
	void			OnRunMove(void);

	void			Set_StackerTray_UpDown( int nOnOff );
	int				Chk_StackerTray_UpDown( int nOnOff );

	void			Set_ConveyorStopper_UpDown( int nOnOff );
	int				Chk_ConveyorStopper_UpDown( int nOnOff );

public:
	//james 2016.0915 lot이 LOT_CURR, LOT_NEXT 둘중 이나이다 
	int				m_nLotProcessNum; 
	CString			m_strFindLotNo;
	int				m_nFindLotNo_Flag;
	///////// 2016.0915 

	///////////////////////////////////////////////////////////////////////
	//james 2016.0819 저장할 목록
	///////////////////////////////////////////////////////////////////////
	int				m_nInitStep;
	int				m_nRunStep; 
	int				m_nStacker_MoveStep;

	int				m_nAxisNum;        //현재의 IndexNum에서 사용하는 모터 실재 번호를 가진다 
	double		m_dpTargetPos;
	int				m_nWork_Site_Pos; //작업 방법, pick or place 
	int				m_nRetry;

	bool			m_bStopperUpDownFlag;
	DWORD		m_dwWaitStopperUpDown[3];

	bool			m_bCVTrayFlag;
	DWORD		m_dwCVTrayWaitTime[3];
	

	CString      m_strAlarmCode;	
	DWORD		m_dwWaitTime[3];
	DWORD		m_dwWaitStackerUpDown[3];
	bool			m_bStackerUpDown;

	int				m_npFindWorkPosYXCPB[5]; //james 2015.0807 처음 찾을때의 정보,  [0]first tray Y, [1]first tray X, [2]:작업가능 수량 또는 공간 정보, [3]first picker num, [4]first buff num
	int				m_npSet_WorkPosYXCPB[5]; //james 2015.0807 작업 완료후 작업 셋팅 위치 정보,  [0]first tray Y, [1]first tray X, [2]:작업가능 수량 또는 공간 정보, [3]first picker num, [4]first buff num

};

extern CRunRejectTrayFrontStackerElevator clsRunRejectTrayFrontStackerElevator;


