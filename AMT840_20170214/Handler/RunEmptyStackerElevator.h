#pragma once

// CRunEmptyStackerElevator ��� ����Դϴ�.

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
	//james 2016.0802 ������ ���
	///////////////////////////////////////////////////////////////////////
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

	int				m_nTrayFull_Falg; //james 2016.0921 Ʈ���� full�ýǸ����� �ٿ��ϰ� �����Ѵ� 

	///////////////////////////////////////////////////////////////////////
};
extern CRunEmptyStackerElevator clsRunEmptyStackerElevator;

