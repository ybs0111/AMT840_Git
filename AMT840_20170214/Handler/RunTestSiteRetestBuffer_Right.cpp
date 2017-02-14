// RunTestSiteRetestBuffer_Right.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "RunTestSiteRetestBuffer_Right.h"

#include "CtlBd_Library.h"
#include "CtlBd_Variable.h"
 #include "FastechPublic_IO.h"
#include "ComizoaPublic.h"
#include "Variable.h"
#include "PublicFunction.h"


// CRunTestSiteRetestBuffer_Right
CRunTestSiteRetestBuffer_Right clsRunTestSiteRetestBuffer_Right;
CRunTestSiteRetestBuffer_Right::CRunTestSiteRetestBuffer_Right()
{
	m_nRunStep	= -1;
	m_nInitStep	= -1;
	m_nTHD_ID = THD_RETEST_2_BUFF;
	m_nRetestBuff_R_Motor = M_RETEST_BUF_2_ROTATOR;
}

CRunTestSiteRetestBuffer_Right::~CRunTestSiteRetestBuffer_Right()
{
}


// CRunTestSiteRetestBuffer_Right 멤버 함수
void CRunTestSiteRetestBuffer_Right::OnThreadRun()
{
	if (st_handler_info.nSystemLock != FALSE)  return;	// 현재 시스템 Lock 상태인 경우 리턴  
	

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

void CRunTestSiteRetestBuffer_Right::OnRunInit()
{
	int nRet_1;

	if (st_sync_info.nInit_Flag[THD_RETEST_2_BUFF] != INIT_CLEAR)		return;		//INIT_CLEAR 일때만 초기화 작업을 한다. 초기화가 끝나면 INIT_CLEAR -> INIT_READY가 되기 떄문에...
	 
	switch (m_nInitStep)
	{
		case -1:		 

			m_nInitStep = 0;
			break;

		case 0:		
			//james 2016.1004 if(st_sync_info.nInit_Flag[THD_TEST_RBT] < INIT_READY)	break;
			if(COMI.mn_homechk_flag[M_TEST_RBT_Z] != CTL_YES) break;
			if(COMI.mn_homechk_flag[M_TEST_DVC_EJECT_Z] != CTL_YES)  break;

			m_nInitStep = 100;
			break;

		case 100:  
			nRet_1 = COMI.HomeCheck_Mot(m_nRetestBuff_R_Motor, MOT_SENS_ELM, MOT_TIMEOUT) ; //
			if(nRet_1 == BD_GOOD)
			{
				COMI.mn_homechk_flag[m_nRetestBuff_R_Motor] = CTL_YES;
				m_nInitStep = 200;
			}
			else if(nRet_1 == BD_ERROR)
			{
				CTL_Lib.mn_run_status = CTL_dWARNING;
				CTL_Lib.Alarm_Error_Occurrence(10100, CTL_Lib.mn_run_status, COMI.strCode);
				m_nInitStep = 100;
			}			
			break;

		case 200:     		//2016.0911 	 
			m_dTargetPos = st_motor_info[M_RETEST_BUF_2_ROTATOR].d_pos[P_RETEST_BUFF_1_RTR_TESTSITE_LEFT];
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, M_RETEST_BUF_2_ROTATOR, m_dTargetPos, COMI.mn_runspeed_rate);
			if (nRet_1 == BD_GOOD) //좌측으로 이동
			{				 
				m_nInitStep = 300;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nInitStep = 200;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
				CTL_Lib.Alarm_Error_Occurrence(10110, dWARNING, st_alarm_info.strCode);
				m_nInitStep = 200;
			}
			break; 

		case 300:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Ret2BuffDvcExistChk,	IO_OFF);
			if(nRet_1 == IO_ON) //리테스트 버퍼에 자재가 감지되면 
			{
				m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Ret2BuffDvcExistChk); 
				CTL_Lib.Alarm_Error_Occurrence(10120, dWARNING, m_strAlarmCode);	
			}
			else
			{
				m_nInitStep = 900;
			}
			break; 

		case 900:
			st_sync_info.nInit_Flag[THD_RETEST_2_BUFF] = INIT_READY; 
			 
			m_nInitStep = 0;
			break;	

		default :
			break;

	}
}

void CRunTestSiteRetestBuffer_Right::OnRunMove()
{
	int nRet_1, nRet_2=0, nRet_3=0;
	int nCount = 0, nBin = 0, nFlag = 0, nTemp =0;

	clsFunc.OnThreadFunctionStepTrace(23, m_nRunStep);	

	switch (m_nRunStep)
	{ 
		case -1: 

			m_nRunStep = 0;
			break;

		case 0:			

			m_nRunStep = 100;
			break;

		case 100:// 
			m_nRunStep = 1000;
			break; 

		case 1000:
			if(st_sync_info.nTestRbt_Dvc_Req[m_nTHD_ID][0] == CTL_REQ)
			{
				//st_sync_info.nTestRbt_Dvc_Req[m_nTHD_ID][1] = WORK_PLACE; //자재를 집을 수 있게 요청 
				if(st_sync_info.nTestRbt_Dvc_Req[m_nTHD_ID][2] == TESTSITE_LEFT) //테스트 사이트 1번 핀에 맞추어 회전 (버퍼 번호 14 ~ 0)
				{
					m_dTargetPos = st_motor_info[M_RETEST_BUF_2_ROTATOR].d_pos[P_RETEST_BUFF_2_RTR_TESTSITE_LEFT];
					m_nRunStep = 2000;
				}
				else if(st_sync_info.nTestRbt_Dvc_Req[m_nTHD_ID][2] == TESTSITE_RIGHT) //원점, 초기위치, 버퍼 번호 0 ~ 14
				{
					m_dTargetPos = st_motor_info[M_RETEST_BUF_2_ROTATOR].d_pos[P_RETEST_BUFF_2_RTR_TESTSITE_RIGHT];
					m_nRunStep = 2000;
				}
			}
			break;

		case 2000:
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, M_RETEST_BUF_2_ROTATOR, m_dTargetPos, COMI.mn_runspeed_rate);
			if (nRet_1 == BD_GOOD) //좌측으로 이동
			{				 
				m_nRunStep = 3000;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nRunStep = 2000;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
				CTL_Lib.Alarm_Error_Occurrence(10130, dWARNING, st_alarm_info.strCode);
				m_nRunStep = 2000;
			}
			break;

		case 3000:
			st_sync_info.nTestRbt_Dvc_Req[m_nTHD_ID][0] = CTL_READY;
			m_nRunStep = 1000;
			break;
	}
}