#pragma once

// CRunRejectTrayFrontStackerElevator ��� ����Դϴ�.

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
	//james 2016.0915 lot�� LOT_CURR, LOT_NEXT ���� �̳��̴� 
	int				m_nLotProcessNum; 
	CString			m_strFindLotNo;
	int				m_nFindLotNo_Flag;
	///////// 2016.0915 

	///////////////////////////////////////////////////////////////////////
	//james 2016.0819 ������ ���
	///////////////////////////////////////////////////////////////////////
	int				m_nInitStep;
	int				m_nRunStep; 
	int				m_nStacker_MoveStep;

	int				m_nAxisNum;        //������ IndexNum���� ����ϴ� ���� ���� ��ȣ�� ������ 
	double		m_dpTargetPos;
	int				m_nWork_Site_Pos; //�۾� ���, pick or place 
	int				m_nRetry;

	bool			m_bStopperUpDownFlag;
	DWORD		m_dwWaitStopperUpDown[3];

	bool			m_bCVTrayFlag;
	DWORD		m_dwCVTrayWaitTime[3];
	

	CString      m_strAlarmCode;	
	DWORD		m_dwWaitTime[3];
	DWORD		m_dwWaitStackerUpDown[3];
	bool			m_bStackerUpDown;

	int				m_npFindWorkPosYXCPB[5]; //james 2015.0807 ó�� ã������ ����,  [0]first tray Y, [1]first tray X, [2]:�۾����� ���� �Ǵ� ���� ����, [3]first picker num, [4]first buff num
	int				m_npSet_WorkPosYXCPB[5]; //james 2015.0807 �۾� �Ϸ��� �۾� ���� ��ġ ����,  [0]first tray Y, [1]first tray X, [2]:�۾����� ���� �Ǵ� ���� ����, [3]first picker num, [4]first buff num

};

extern CRunRejectTrayFrontStackerElevator clsRunRejectTrayFrontStackerElevator;


