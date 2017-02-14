#pragma once

// CRejectTrayTransfer ��� ����Դϴ�.

class CRunRejectTrayTransfer : public CObject
{
public:
	CRunRejectTrayTransfer();
	virtual ~CRunRejectTrayTransfer();

public:
	void			OnThreadRun(void);
	void			OnRunInit(void);
	void			OnRunMove(void);

	void			Set_RejectTrayTransfer_UpDown( int nOnOff ); //Up:IO_ON  Down:IO_OFF
	int				Chk_RejectTrayTransfer_UpDown( int nOnOff );

	void			Set_RejectTransferEjectorOnOff( int nOnOff );
	void			Set_RejectTrayVacuum_OnOff( int nOnOff );
	int				Chk_RejectTrayVacuum_OnOff( int nOnOff );

public:
	CString         m_strAlarmCode;	
	DWORD			m_dwWaitTime[3];
	DWORD			m_dwWaitTransferUpDown[3];
	bool			m_bTransferUpDownFlag;

	bool			m_bVacuumOnOffFlag;
	DWORD			m_dwWaitVacuulOnOff[3];

	///////////////////////////////////////////////////////////////////////
	//james 2016.0804 ������ ���
	///////////////////////////////////////////////////////////////////////
	int				m_nInitStep;
	int				m_nRunStep; 
	int				m_nStacker_MoveStep;

	int				m_nAxisNum;        //������ IndexNum���� ����ϴ� ���� ���� ��ȣ�� ������ 
	double			m_dpTargetPos;
	int				m_nWork_Site_Pos; //�۾� ���, pick or place 

	int				m_nTransferJobFlag[2]; //work Ʈ�����ۿ� ������ ��û�Ѵ� , [0]:��û����, [1]:1:Ʈ���̸� ����, 2:Ʈ���̸� ����		

	int				m_nTransfer_WaitPosMove_Flag; //��� ��ġ�� �̼۽�Ų �÷��� 

};

extern CRunRejectTrayTransfer clsRunRejectTrayTransfer;

