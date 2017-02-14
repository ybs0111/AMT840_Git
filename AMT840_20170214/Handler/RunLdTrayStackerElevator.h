#pragma once

// CRunLdTrayStackerElevator ��� ����Դϴ�.

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
	//james 2016.0802 ������ ���
	///////////////////////////////////////////////////////////////////////
	int			Elevator_Job_Move_Pos(int nMode, int nMotNum,  int nTargetPos);

public:
	int				m_nLotProcessNum; //james 2016.0929 lot�� LOT_CURR, LOT_NEXT ���� �̳��̴� 

	///////////////////////////////////////////////////////////////////////
	//james 2016.0802 ������ ���
	///////////////////////////////////////////////////////////////////////
	int				m_nInitStep;
	int				m_nRunStep; 
	int				m_nStacker_MoveStep;

	int				m_nAxisNum;        //������ IndexNum���� ����ϴ� ���� ���� ��ȣ�� ������ 
	double			m_dpTargetPos;

	DWORD			m_dwWaitTime[3];

	double			m_dReceive_Pos_Backup;		//������ ���� ��ġ ������ ��� 
	double           m_dSupply_Pos_Backup;	//����̽��� ����ִ� unload tray�� �޴� ���� 
	double           m_dReference_Pos_Backup;	//������ ���� ��ġ ������ ��� 

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


