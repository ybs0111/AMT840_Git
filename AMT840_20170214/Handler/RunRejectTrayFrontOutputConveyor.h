#pragma once

// CRejectTrayFrontOutputConveyor 명령 대상입니다.

class CRunRejectTrayFrontOutputConveyor : public CObject
{
public:
	CRunRejectTrayFrontOutputConveyor();
	virtual ~CRunRejectTrayFrontOutputConveyor();

public:
	void			OnThreadRun(void);
	void			OnRunInit(void);
	void			OnRunMove(void);

	void			Set_RejectFrontStopper_UpDown( int nOnOff );
	int				Chk_RejectFrontStopper_UpDown( int nOnOff );

public:
	CString         m_strAlarmCode;	
	DWORD			m_dwWaitTime[3];
	DWORD			m_dwWaitStopperUpDown[3];
	bool			m_bStopperUpDownFlag;
};

extern CRunRejectTrayFrontOutputConveyor clsRunRejectTrayFrontOutputConveyor;


