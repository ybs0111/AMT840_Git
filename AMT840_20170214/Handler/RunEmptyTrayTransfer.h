#pragma once

// CRunEmptyTrayTransfer ��� ����Դϴ�.

class CRunEmptyTrayTransfer : public CObject
{
public:
	CRunEmptyTrayTransfer();
	virtual ~CRunEmptyTrayTransfer();

public:
	void			OnThreadRun(void);
	void			OnRunInit(void);
	void			OnRunMove(void);

	void			Set_EmptyTrayTransfer_UpDown( int nOnOff );//
	int				Chk_EmptyTrayTransfer_UpDown( int nOnOff );

	void			Set_EmptyTransferEjectorOnOff( int nOnOff );
	void			Set_EmptyTrayVacuum_OnOff( int nOnOff );
	int				Chk_EmptyTrayVacuum_OnOff( int nOnOff );
	///////////////////////////////////////////////////////////////////////
	//james 2016.0804 ������ ���
	///////////////////////////////////////////////////////////////////////
	int				m_nInitStep;
	int				m_nRunStep; 
	int				m_nStacker_MoveStep;

	int				m_nAxisNum;        //������ IndexNum���� ����ϴ� ���� ���� ��ȣ�� ������ 
	double			m_dpTargetPos;
	int				m_nWork_Site_Pos; //�۾� ���, pick or place 

	CString         m_strAlarmCode;	
	DWORD			m_dwWaitTime[3];
	DWORD			m_dwWaitTransferUpDown[3];
	bool			m_bTransferUpDownFlag;

	bool			m_bVacuumOnOffFlag;
	DWORD			m_dwWaitVacuulOnOff[3];

	int				m_nTransferJobFlag[2]; //work Ʈ�����ۿ� ������ ��û�Ѵ� , [0]:��û����, [1]:1:Ʈ���̸� ����, 2:Ʈ���̸� ����		

	int				m_nTransfer_WaitPosMove_Flag; //��� ��ġ�� �̼۽�Ų �÷��� 

	int				m_nEmptyToReject;

};

extern CRunEmptyTrayTransfer clsRunEmptyTrayTransfer;


