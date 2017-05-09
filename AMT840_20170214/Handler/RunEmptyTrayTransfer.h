#pragma once

// CRunEmptyTrayTransfer 명령 대상입니다.

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
	//james 2016.0804 저장할 목록
	///////////////////////////////////////////////////////////////////////
	int				m_nInitStep;
	int				m_nRunStep; 
	int				m_nStacker_MoveStep;

	int				m_nAxisNum;        //현재의 IndexNum에서 사용하는 모터 실재 번호를 가진다 
	double			m_dpTargetPos;
	int				m_nWork_Site_Pos; //작업 방법, pick or place 

	CString         m_strAlarmCode;	
	DWORD			m_dwWaitTime[3];
	DWORD			m_dwWaitTransferUpDown[3];
	bool			m_bTransferUpDownFlag;

	bool			m_bVacuumOnOffFlag;
	DWORD			m_dwWaitVacuulOnOff[3];

	int				m_nTransferJobFlag[2]; //work 트랜스퍼에 동작을 요청한다 , [0]:요청정보, [1]:1:트레이를 집기, 2:트레이를 놓기		

	int				m_nTransfer_WaitPosMove_Flag; //대기 위치로 이송시킨 플레그 

	int				m_nEmptyToReject;

};

extern CRunEmptyTrayTransfer clsRunEmptyTrayTransfer;


