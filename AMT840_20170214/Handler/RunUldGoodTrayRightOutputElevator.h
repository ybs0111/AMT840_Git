#pragma once

// CRunUldGoodTrayRightOutputElevator ��� ����Դϴ�.

class CRunUldGoodTrayRightOutputElevator : public CObject
{
public:
	CRunUldGoodTrayRightOutputElevator();
	virtual ~CRunUldGoodTrayRightOutputElevator();

public:
	void			OnThreadRun(void);
	void			OnRunInit(void);
	void			OnRunMove(void);

	void			Set_GoodStackerRightTray_UpDown( int nOnOff );
	int				Chk_GoodStackerRightTray_UpDown( int nOnOff );

	void			Set_GoodConveyorRightStopper_UpDown( int nOnOff );
	int				Chk_GoodConveyorRightStopper_UpDown( int nOnOff );

public:
	//james 2016.0912 lot�� LOT_CURR, LOT_NEXT ���� �̳��̴� 
	int				m_nLotProcessNum; 
	CString			m_strFindLotNo;
	int				m_nFindLotNo_Flag;
	///////// 2016.0912 
	///////////////////////////////////////////////////////////////////////
	//james 2016.0804 ������ ���
	///////////////////////////////////////////////////////////////////////
	int				m_nInitStep;
	int				m_nRunStep; 
	int				m_nStacker_MoveStep;

	int				m_nAxisNum;        //������ IndexNum���� ����ϴ� ���� ���� ��ȣ�� ������ 
	double			m_dpTargetPos;
	int				m_nWork_Site_Pos; //�۾� ���, pick or place 

	bool			m_bStopperUpDownFlag;
	DWORD			m_dwWaitStopperUpDown[3];

	bool			m_bCVTrayFlag;
	DWORD			m_dwCVTrayWaitTime[3];
	int				m_npFindWorkPosYXCPB[5]; //james 2015.0807 ó�� ã������ ����,  [0]first tray Y, [1]first tray X, [2]:�۾����� ���� �Ǵ� ���� ����, [3]first picker num, [4]first buff num
	int				m_npSet_WorkPosYXCPB[5]; //james 2015.0807 �۾� �Ϸ��� �۾� ���� ��ġ ����,  [0]first tray Y, [1]first tray X, [2]:�۾����� ���� �Ǵ� ���� ����, [3]first picker num, [4]first buff num


	CString         m_strAlarmCode;	
	DWORD			m_dwWaitTime[3];
	DWORD			m_dwWaitStackerRightUpDown[3];
	bool			m_bStackeRightUpDown;

	int				m_nRetry;

	///////////////////////////////////////////////////////////////
	//2016.1203
	CString		m_strLotNo;
	CString		m_strPartNo;
	DWORD		m_dwCycleTime[3];
	///////////////////////////////////////////////////////////////
};

extern CRunUldGoodTrayRightOutputElevator clsRunUldGoodTrayRightOutputElevator;
