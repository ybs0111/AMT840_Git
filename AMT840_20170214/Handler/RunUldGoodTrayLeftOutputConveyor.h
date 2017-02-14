#pragma once

// CRunUldGoodTrayLeftOutputConveyor 명령 대상입니다.

class CRunUldGoodTrayLeftOutputConveyor : public CObject
{
public:
	CRunUldGoodTrayLeftOutputConveyor();
	virtual ~CRunUldGoodTrayLeftOutputConveyor();

public:
	void			OnThreadRun(void);
	void			OnRunInit(void);
	void			OnRunMove(void);

	void			Set_GoodConveyorLeftStopper_UpDown( int nOnOff );
	int				Chk_GoodConveyorLeftStopper_UpDown( int nOnOff );

public:
	CString			m_strAlarmCode;
	bool			m_bStopperUpDownFlag;
	DWORD			m_dwWaitStopperUpDown[3];


	bool			m_bTrayArrivedFlag;
	DWORD			m_dwTrayArrivedWaitTime[3];



};
extern CRunUldGoodTrayLeftOutputConveyor clsRunUldGoodTrayLeftOutputConveyor;


