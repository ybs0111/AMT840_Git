#pragma once

// CRunRejectTrayRearOutputConveyor 명령 대상입니다.

class CRunRejectTrayRearOutputConveyor : public CObject
{
public:
	CRunRejectTrayRearOutputConveyor();
	virtual ~CRunRejectTrayRearOutputConveyor();


public:
	void			OnThreadRun(void);
	void			OnRunInit(void);
	void			OnRunMove(void);

	void			Set_RejectRearStopper_UpDown( int nOnOff );
	int				Chk_RejectRearStopper_UpDown( int nOnOff );
public:
	CString         m_strAlarmCode;	
	DWORD			m_dwWaitTime[3];
	DWORD			m_dwWaitStopperUpDown[3];
	bool			m_bStopperUpDownFlag;
};

extern CRunRejectTrayRearOutputConveyor clsRunRejectTrayRearOutputConveyor;


