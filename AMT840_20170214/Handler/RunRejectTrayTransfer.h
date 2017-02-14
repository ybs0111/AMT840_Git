#pragma once

// CRejectTrayTransfer 명령 대상입니다.

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
	//james 2016.0804 저장할 목록
	///////////////////////////////////////////////////////////////////////
	int				m_nInitStep;
	int				m_nRunStep; 
	int				m_nStacker_MoveStep;

	int				m_nAxisNum;        //현재의 IndexNum에서 사용하는 모터 실재 번호를 가진다 
	double			m_dpTargetPos;
	int				m_nWork_Site_Pos; //작업 방법, pick or place 

	int				m_nTransferJobFlag[2]; //work 트랜스퍼에 동작을 요청한다 , [0]:요청정보, [1]:1:트레이를 집기, 2:트레이를 놓기		

	int				m_nTransfer_WaitPosMove_Flag; //대기 위치로 이송시킨 플레그 

};

extern CRunRejectTrayTransfer clsRunRejectTrayTransfer;

