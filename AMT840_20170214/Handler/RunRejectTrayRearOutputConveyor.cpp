// RejectTrayRearOutputConveyor.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "RunRejectTrayRearOutputConveyor.h"
#include "Variable.h"
#include "FastechPublic_IO.h"
#include "CtlBd_Library.h"
#include "LogCtrl.h"
#include "LogFromat.h"


// CRejectTrayRearOutputConveyor
CRunRejectTrayRearOutputConveyor clsRunRejectTrayRearOutputConveyor;
CRunRejectTrayRearOutputConveyor::CRunRejectTrayRearOutputConveyor()
{
}

CRunRejectTrayRearOutputConveyor::~CRunRejectTrayRearOutputConveyor()
{
}


// CRejectTrayRearOutputConveyor 멤버 함수
void CRunRejectTrayRearOutputConveyor::OnThreadRun(void)
{
}

void CRunRejectTrayRearOutputConveyor::OnRunInit(void)
{
}

void CRunRejectTrayRearOutputConveyor::OnRunMove(void)
{
}
//nOnOff -> ON : DOWN 상태
//nOnOff -> OFF : UP 상태
void CRunRejectTrayRearOutputConveyor::Set_RejectRearStopper_UpDown( int nOnOff )
{
	CString strLogKey[10];
	CString	strLogData[10];

	strLogKey[0] = _T("Mode Start");
	strLogData[0].Format(_T("%d"),0);

	m_bStopperUpDownFlag	= false;
	m_dwWaitStopperUpDown[0]	= GetCurrentTime();

	FAS_IO.set_out_bit( st_io_info.o_Rej2UldCvyStopUpDnCyl, nOnOff);

	///kwlee 2016.0812
	if (nOnOff == IO_ON)
	{
		//LogFunction(_T("REJECT_TRAY_REAR_CONVEYOR"),_T("UP"),_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("REJECT_TRAY_REAR_CONVEYOR"),_T("UP"),0,_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
	else
	{
		//LogFunction(_T("REJECT_TRAY_REAR_CONVEYOR"),_T("DOWN"),_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("REJECT_TRAY_REAR_CONVEYOR"),_T("DOWN"),0,_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
}
//nOnOff -> ON : DOWN 상태
//nOnOff -> OFF : UP 상태
int	CRunRejectTrayRearOutputConveyor::Chk_RejectRearStopper_UpDown( int nOnOff )
{
	CString strLogKey[10];
	CString	strLogData[10];

	strLogKey[0] = _T("Mode End");
	strLogData[0].Format(_T("%d"),0);

//	800508 1 0 "REJECT_2_UNLOADING_CONVEYOR_STOPPER_UP_OFF_CHECK_ERROR"
//	800509 1 0 "REJECT_2_UNLOADING_CONVEYOR_STOPPER_DOWN_OFF_CHECK_ERROR"
//	810508 1 0 "REJECT_2_UNLOADING_CONVEYOR_STOPPER_UP_ON_CHECK_ERROR"
//	810509 1 0 "REJECT_2_UNLOADING_CONVEYOR_STOPPER_DOWN_ON_CHECK_ERROR"
	int nWaitTime = WAIT_STOPPER_UP_DN;

	if (nOnOff == IO_OFF)
	{
		if (m_bStopperUpDownFlag == false &&
			FAS_IO.get_in_bit(st_io_info.i_Rej2UldCvyStopUpChk,	IO_ON)	== IO_ON &&
			FAS_IO.get_in_bit(st_io_info.i_Rej2UldCvyStopDnChk,	IO_OFF)	== IO_OFF)
		{
			m_bStopperUpDownFlag		= true;
			m_dwWaitStopperUpDown[0]	= GetCurrentTime();
		}
		else if (m_bStopperUpDownFlag == true &&
				 FAS_IO.get_in_bit(st_io_info.i_Rej2UldCvyStopUpChk,	IO_ON)	== IO_ON &&
				 FAS_IO.get_in_bit(st_io_info.i_Rej2UldCvyStopDnChk,	IO_OFF)	== IO_OFF)
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
				//LogFunction(_T("REJECT_TRAY_REAR_CONVEYOR"),_T("DOWN"),_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("REJECT_TRAY_REAR_CONVEYOR"),_T("DOWN"),1,_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
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
				m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_Rej2UldCvyStopDnChk); 
				//2017.0109
				clsLog.LogFunction(_T("REJECT_TRAY_REAR_CONVEYOR"),_T("DOWN"),1,_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_ERROR;
			}
		}

	}
	else
	{

		if (m_bStopperUpDownFlag == false &&
			FAS_IO.get_in_bit(st_io_info.i_Rej2UldCvyStopUpChk,	IO_OFF)	== IO_OFF &&
			FAS_IO.get_in_bit(st_io_info.i_Rej2UldCvyStopDnChk,	IO_ON)	== IO_ON)
		{
			m_bStopperUpDownFlag			= true;
			m_dwWaitStopperUpDown[0]	= GetCurrentTime();
		}
		else if (m_bStopperUpDownFlag == true &&
				 FAS_IO.get_in_bit(st_io_info.i_Rej2UldCvyStopUpChk,	IO_ON)	== IO_ON &&
				 FAS_IO.get_in_bit(st_io_info.i_Rej2UldCvyStopDnChk,	IO_OFF)	== IO_OFF)
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
				//LogFunction(_T("REJECT_TRAY_REAR_CONVEYOR"),_T("UP"),_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("REJECT_TRAY_REAR_CONVEYOR"),_T("UP"),1,_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
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
				m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_Rej2UldCvyStopUpChk); 
				//2017.0109
				clsLog.LogFunction(_T("REJECT_TRAY_REAR_CONVEYOR"),_T("UP"),1,_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_ERROR;
			}
		}

	}

	return RET_PROCEED;
}
