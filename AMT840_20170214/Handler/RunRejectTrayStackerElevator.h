#pragma once

// CRunRejectTrayStackerElevator ��� ����Դϴ�.

class CRunRejectTrayStackerElevator : public CObject
{
public:
	CRunRejectTrayStackerElevator();
	virtual ~CRunRejectTrayStackerElevator();

public:
	void			Process_Tray_Insert(int n_Mode); //2016.0924 james 

	void			OnThreadRun(void);
	void			OnRunInit(void);
	void			OnRunMove(void);

	void			Set_StackerTray_UpDown( int nOnOff );
	int				Chk_StackerTray_UpDown( int nOnOff );

public:
	int				m_nLotProcessNum; //james 2016.0929 

	CString         m_strAlarmCode;	
	DWORD			m_dwWaitTime[3];
	DWORD			m_dwWaitStackerUpDown[3];
	bool			m_bStackerUpDown;
	bool            m_bTrayCheck_Flag;

	bool			m_bTrayInFlag;
	DWORD			m_dwTrayInWaitTime[3];

	bool			m_bAcMotorPassFlag;
	//bool			m_bAcMotorFlag;
	DWORD			m_dwAcMotorPasstWaitTime[3];

	///////////////////////////////////////////////////////////////////////
	//james 2016.0818 ������ ���
	///////////////////////////////////////////////////////////////////////
	int				m_nInsertStep; //2016.0924 james 

	int				m_nInitStep;
	int				m_nRunStep; 
	int				m_nStacker_MoveStep;

	int				m_nAxisNum;        //������ IndexNum���� ����ϴ� ���� ���� ��ȣ�� ������ 
	double			m_dpTargetPos;

	int				m_nWork_Site_Pos; //�۾� ���, pick or place 
	

	double			m_dReceive_Pos_Backup;		//������ ���� ��ġ ������ ��� 
	double           m_dSupply_Pos_Backup;	//����̽��� ����ִ� unload tray�� �޴� ���� 
	double           m_dReference_Pos_Backup;	//������ ���� ��ġ ������ ��� 

	double			m_dCurrentPos;
	double			m_dTargetPos;

	DWORD			m_dwSdWaitTime[3];
	bool			m_bSd_Move_Flag;

	bool			m_bCVTrayStackerArrivedFlag;
	DWORD			m_dwCVTrayStackerArrivedWaitTime[3];

	///////////////////////////////////////////////////////////////////////
};

extern CRunRejectTrayStackerElevator clsRunRejectTrayStackerElevator;

