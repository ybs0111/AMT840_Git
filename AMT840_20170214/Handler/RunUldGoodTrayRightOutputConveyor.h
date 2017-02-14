#pragma once

// CRunUldGoodTrayRightOutputConveyor 명령 대상입니다.

class CRunUldGoodTrayRightOutputConveyor : public CObject
{
public:
	CRunUldGoodTrayRightOutputConveyor();
	virtual ~CRunUldGoodTrayRightOutputConveyor();

public:
	void			OnThreadRun(void);
	void			OnRunInit(void);
	void			OnRunMove(void);

	void			Set_GoodConveyorRightStopper_UpDown( int nOnOff );
	int				Chk_GoodConveyorRightStopper_UpDown( int nOnOff );

public:
	CString			m_strAlarmCode;
	bool			m_bStopperUpDownFlag;
	DWORD			m_dwWaitStopperUpDown[3];
};

extern CRunUldGoodTrayRightOutputConveyor clsRunUldGoodTrayRightOutputConveyor;

