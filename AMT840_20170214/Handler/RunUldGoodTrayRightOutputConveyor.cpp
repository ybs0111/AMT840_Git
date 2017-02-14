// RunUldGoodTrayRightOutputConveyor.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "RunUldGoodTrayRightOutputConveyor.h"
#include "Variable.h"
#include "FastechPublic_IO.h"
#include "CtlBd_Library.h"
#include "LogCtrl.h"
#include "LogFromat.h"

// CRunUldGoodTrayRightOutputConveyor
CRunUldGoodTrayRightOutputConveyor clsRunUldGoodTrayRightOutputConveyor;
CRunUldGoodTrayRightOutputConveyor::CRunUldGoodTrayRightOutputConveyor()
{
}

CRunUldGoodTrayRightOutputConveyor::~CRunUldGoodTrayRightOutputConveyor()
{
}


// CRunUldGoodTrayRightOutputConveyor 멤버 함수
void CRunUldGoodTrayRightOutputConveyor::OnThreadRun(void)
{
}

void CRunUldGoodTrayRightOutputConveyor::OnRunInit(void)
{
}

void CRunUldGoodTrayRightOutputConveyor::OnRunMove(void)
{
}

//nOnOff -> ON : DOWN 상태
//nOnOff -> OFF : UP 상태
void CRunUldGoodTrayRightOutputConveyor::Set_GoodConveyorRightStopper_UpDown( int nOnOff )
{

	CString strLogKey[10];
	CString	strLogData[10];

	strLogKey[0] = _T("Mode Start");
	strLogData[0].Format(_T("%d"),0);

	m_bStopperUpDownFlag = true;
	m_dwWaitStopperUpDown[0] = GetCurrentTime();

	FAS_IO.set_out_bit( st_io_info.o_ReadyUld2CvyStopCyl, nOnOff);

	///kwlee 2016.0812
	if (nOnOff == IO_ON)
	{
		//LogFunction(_T("ULD_TRAY_RIGHT_CONVEYOR"),_T("UP"),_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("ULD_TRAY_RIGHT_CONVEYOR"),_T("UP"),0,_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
	else
	{
		//LogFunction(_T("ULD_TRAY_RIGHT_CONVEYOR"),_T("DOWN"),_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("ULD_TRAY_RIGHT_CONVEYOR"),_T("DOWN"),0,_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
}

//nOnOff -> ON : DOWN 상태
//nOnOff -> OFF : UP 상태
int CRunUldGoodTrayRightOutputConveyor::Chk_GoodConveyorRightStopper_UpDown( int nOnOff )
{
	CString strLogKey[10];
	CString	strLogData[10];

	strLogKey[0] = _T("Mode End");
	strLogData[0].Format(_T("%d"),0);

//	800310 1 0 "UNLOADING_2_STACKER_UP_CHECK"
//	800311 1 0 "UNLOADING_2_STACKER_DOWN_CHECK"
	int nWaitTime = WAIT_STACKER_UP_DN;

	if (nOnOff == IO_OFF)
	{
		if (m_bStopperUpDownFlag == false &&
			FAS_IO.get_in_bit(st_io_info.i_Uld2StkUpChk,	IO_ON)	== IO_ON &&
			FAS_IO.get_in_bit(st_io_info.i_Uld2StkDnChk,	IO_OFF)	== IO_OFF)
		{
			m_bStopperUpDownFlag		= true;
			m_dwWaitStopperUpDown[0]	= GetCurrentTime();
		}
		else if (m_bStopperUpDownFlag == true &&
				 FAS_IO.get_in_bit(st_io_info.i_Uld2StkUpChk,	IO_ON)	== IO_ON &&
				 FAS_IO.get_in_bit(st_io_info.i_Uld2StkDnChk,	IO_OFF)	== IO_OFF)
		{
			m_dwWaitStopperUpDown[1] = GetCurrentTime();
			m_dwWaitStopperUpDown[2] = m_dwWaitStopperUpDown[1] - m_dwWaitStopperUpDown[0];

			if (m_dwWaitStopperUpDown[2] <= 0)
			{
				m_dwWaitStopperUpDown[0] = GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwWaitStopperUpDown[2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
			{
				//LogFunction(_T("ULD_TRAY_RIGHT_CONVEYOR"),_T("DOWN"),_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("ULD_TRAY_RIGHT_CONVEYOR"),_T("DOWN"),1,_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_GOOD;
			}
		}
		else
		{
			m_dwWaitStopperUpDown[1] = GetCurrentTime();
			m_dwWaitStopperUpDown[2] = m_dwWaitStopperUpDown[1] - m_dwWaitStopperUpDown[0];

			if (m_dwWaitStopperUpDown[2] <= 0)
			{
				m_dwWaitStopperUpDown[0] = GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwWaitStopperUpDown[2] > (DWORD)st_wait_info.nLimitWaitTime[nWaitTime])
			{
				m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_Uld2StkUpChk); 
				//2017.0109
				clsLog.LogFunction(_T("ULD_TRAY_RIGHT_CONVEYOR"),_T("DOWN"),1,_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_ERROR;
			}
		}
	}
	else
	{
		if (m_bStopperUpDownFlag == false &&
			FAS_IO.get_in_bit(st_io_info.i_Uld2StkUpChk,	IO_OFF)	== IO_OFF &&
			FAS_IO.get_in_bit(st_io_info.i_Uld2StkDnChk,	IO_ON)	== IO_ON)
		{
			m_bStopperUpDownFlag			= true;
			m_dwWaitStopperUpDown[0]	= GetCurrentTime();
		}
		else if (m_bStopperUpDownFlag == true &&
				 FAS_IO.get_in_bit(st_io_info.i_Uld2StkUpChk,	IO_OFF)	== IO_OFF &&
				 FAS_IO.get_in_bit(st_io_info.i_Uld2StkDnChk,	IO_ON)	== IO_ON)
		{
			m_dwWaitStopperUpDown[1]	= GetCurrentTime();
			m_dwWaitStopperUpDown[2]	= m_dwWaitStopperUpDown[1] - m_dwWaitStopperUpDown[0];

			if (m_dwWaitStopperUpDown[2] <= 0)
			{
				m_dwWaitStopperUpDown[0]	= GetCurrentTime();
				return RET_PROCEED;
			}

			if(m_dwWaitStopperUpDown[2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
			{
				//LogFunction(_T("ULD_TRAY_RIGHT_CONVEYOR"),_T("UP"),_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("ULD_TRAY_RIGHT_CONVEYOR"),_T("UP"),1,_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_GOOD;
			}
		}
		else
		{
			m_dwWaitStopperUpDown[1]	= GetCurrentTime();
			m_dwWaitStopperUpDown[2]	= m_dwWaitStopperUpDown[1] - m_dwWaitStopperUpDown[0];

			if (m_dwWaitStopperUpDown[2] <= 0)
			{
				m_dwWaitStopperUpDown[0]	= GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwWaitStopperUpDown[2] > (DWORD)st_wait_info.nLimitWaitTime[nWaitTime])
			{
				m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_Uld2StkDnChk); 
				//2017.0109
				clsLog.LogFunction(_T("ULD_TRAY_RIGHT_CONVEYOR"),_T("UP"),1,_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_ERROR;
			}
		}
	}

	return RET_PROCEED;
}