// RunRejectTrayBuffer.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "RunRejectTrayBuffer.h"
#include "Variable.h"
#include "FastechPublic_IO.h"
#include "PublicFunction.h"
#include "LogCtrl.h"
#include "LogFromat.h"
#include "CtlBd_Variable.h"
#include "CtlBd_Library.h"
#include "ComizoaPublic.h"
// CRunRejectTrayBuffer

CRunRejectTrayBuffer clsRunRejectTrayBuffer;

CRunRejectTrayBuffer::CRunRejectTrayBuffer()
{
	m_nRunStep = 0;
	m_nInitStep = 0;
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	//2017.0430 empty option
	st_handler_info.mn_auto_empty_tray = NO;
	st_handler_info.mn_out_empty = NO;
	m_nEmptyToReject = 0;
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	m_nAxisNum = M_WORKTRAY_TRANSFER;
}

CRunRejectTrayBuffer::~CRunRejectTrayBuffer()
{
}


// CRunRejectTrayBuffer 멤버 함수
void CRunRejectTrayBuffer::OnThreadRun(void)
{
	if (st_handler_info.nSystemLock != FALSE)  return;		

	switch(st_handler_info.nRunStatus)
	{
	case dINIT:
		OnRunInit();
		break;

	case dRUN:
		OnRunMove();					
		break;

	case dSTOP:
		m_dwWaitTime[0] = GetCurrentTime();
		break;

	case dWARNING:
		m_dwWaitTime[0] = GetCurrentTime();
		break;

	case dSELFCHECK:
		break;

	case dREINSTATE:
		break;

	case dRECOVERY:
		break;

	default:
		break;
	}
}


void CRunRejectTrayBuffer::OnRunInit()
{
	int nRet_1, nRet_2=0;

	if (st_sync_info.nInit_Flag[THD_REJECT_BUFFER_STACK] != INIT_CLEAR)		return;		//INIT_CLEAR 일때만 초기화 작업을 한다. 초기화가 끝나면 INIT_CLEAR -> INIT_READY가 되기 떄문에...

	switch (m_nInitStep)
	{
		case 0:
			st_sync_info.nInit_Flag[THD_REJECT_BUFFER_STACK] = INIT_READY;
			m_nInitStep = 0;
			m_nRunStep = 0;
			break;

		default:
			break;
	}
}

void CRunRejectTrayBuffer::OnRunMove()
{
	int nRet_1 = 0, nRet_2 = 0;
	CString strTmp = _T("");

	clsFunc.OnThreadFunctionStepTrace(38, m_nRunStep);

	double dCurrPos = COMI.Get_MotCurrentPos( M_WORKTRAY_TRANSFER  );

	switch (m_nRunStep)
	{ 
		case 0:
			m_nRunStep = 1000;
			break;

		case 1000:
			if( st_handler_info.mn_auto_empty_tray == YES )
			{
				if( st_handler_info.mn_out_empty == YES )
				{
					m_nRunStep = 1100;
				}
			}
			break;

		case 1100:
			///////////////////////////////////////////////////////////////////////
			//2017.0430
			if( st_handler_info.mn_auto_empty_tray == YES )
			{//들고 있는걸 버리자
				if(st_handler_info.mn_out_empty == YES)//강제로 버리는 모드인지 확인
				{
					st_sync_info.nWorkTransfer_Req[THD_REJECT_BUFFER_STACK][0] = CTL_REQ;
					m_nRunStep = 2000;
				}
			}
			///////////////////////////////////////////////////////////////////////
			break;

		case 2000:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_EmptyRejFull,	IO_ON); //load stacker up check    
			if(nRet_1 == IO_ON)
			{
				strTmp.Format( _T("81%04d"), st_io_info.i_EmptyRejFull);
				CTL_Lib.Alarm_Error_Occurrence(9680, dWARNING, strTmp);
				break;
			}

			if( ( dCurrPos > ( st_motor_info[m_nAxisNum].d_pos[P_WORKTRANS_REJECT_STACK] - 10 ) ) &&
				( dCurrPos < ( st_motor_info[m_nAxisNum].d_pos[P_WORKTRANS_REJECT_STACK] + 10 ) ))
			{
				st_sync_info.nWorkTransfer_Req[THD_REJECT_BUFFER_STACK][0] = CTL_READY;
				m_nEmptyToReject++;
				if( m_nEmptyToReject > 5 )//5장정도를 미리 버린다.
				{
					st_handler_info.mn_out_empty = NO;
					m_nEmptyToReject = 0;
				}
				else
				{
					m_nEmptyToReject = 1000;
				}
			}
			break;

	}
}