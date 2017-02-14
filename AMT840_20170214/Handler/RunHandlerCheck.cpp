#include "StdAfx.h"
#include "RunHandlerCheck.h"
#include "PublicFunction.h"
#include "Variable.h"
#include "FastechPublic_IO.h"
#include "ComizoaPublic.h"
#include "Ctlbd_Variable.h"
#include "math.h"
#include "CtlBd_Library.h"
#include "XgemClient.h"
#include "AlgMemory.h"


CRunHandlerCheck clsHdChk;

CRunHandlerCheck::CRunHandlerCheck(void)
{
	m_nStartStep				= 0;
	m_nStopStep				= 0;
	m_nResetStep				= 0;
	m_nBuzzerStep			= 0;
	m_nAutoStep				= 0;
	m_nManualStep			= 0;
	m_nOldHifixMode		= -1;
	m_nLdUldMuteStep		= 0;
	m_nRejMuteStep			= 0;

	// jtkim 20160929
	m_bFrontMuteFlag		= NO;
//	FAS_IO.set_out_bit( st_io_info.o_LdUldMuteOn, IO_OFF);
//	FAS_IO.set_out_bit( st_io_info.o_LdUldMuteOff, IO_ON);

	m_bRightMuteFlag		= NO;
//	FAS_IO.set_out_bit( st_io_info.o_RejMuteOn, IO_OFF);
//	FAS_IO.set_out_bit( st_io_info.o_RejMuteOff, IO_ON);
}


CRunHandlerCheck::~CRunHandlerCheck(void)
{
}


void CRunHandlerCheck::OnStartCheck()
{
//	int nNum;

	CString mstr_temp; //2015.0407 james 

	if (st_handler_info.nSystemLock != NO)	
	{
		return;
	}

	if (st_handler_info.nMenuLock != NO)	
	{
		return;
	}

	if (st_handler_info.nSwitchLock != NO)	
	{
		return;
	}

	if (st_handler_info.nRunStatus == dRUN)	
	{
		return;
	}

	if (st_handler_info.nInitialSuccess != YES)	
	{
		return;
	}

	if(st_handler_info.nMachineMode	== MACHINE_MANUAL)
	{
		if (FAS_IO.get_in_bit(st_io_info.i_StartChk, IO_OFF) == IO_ON)
		{
			if (FAS_IO.get_in_bit(st_io_info.i_FrontSelectSwChk, IO_ON) == IO_OFF ||
				FAS_IO.get_in_bit(st_io_info.i_RearSelectSwChk, IO_ON)	== IO_OFF )
			{
				if (st_work_info.nBdReadyChk == YES)
				{
					st_work_info.nBdReadyChk = NO;
				}

				if (st_handler_info.cWndMain != NULL)
				{
					//st_other_info.nBuzYesNo = NO;
					if (FAS_IO.get_in_bit(st_io_info.i_FrontSelectSwChk, IO_ON) == IO_OFF)
					{
						st_other_info.strBoxMsg = _T("Please Check Front Auto Key Status.");
					}
					else
					{
						st_other_info.strBoxMsg = _T("Please Check Rear Auto Key Status.");
					}
					st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
				}
			}
		}
		return;
	}

	switch(m_nStartStep)
	{
		case 0:
			// [STOP] 상태에서만 [START] 가능하도록 제한한다
			if (st_handler_info.nRunStatus == dSTOP)
			{
				if (FAS_IO.get_in_bit(st_io_info.i_StartChk, IO_OFF) == IO_ON)
				{
					// 만일 STOP 버튼이 함께 눌린 경우에는 무시한다
					if (FAS_IO.get_in_bit(st_io_info.i_StopChk, IO_OFF) == IO_ON)
					{
						break;
					}
					
					m_dwStartWaitTime[0] = GetCurrentTime();
					m_nStartStep = 100;
				}
			}
			break;

		case 100: 
			if (FAS_IO.get_in_bit(st_io_info.i_StartChk, IO_OFF) == IO_ON)
			{
				m_dwStartWaitTime[1] = GetCurrentTime();
				m_dwStartWaitTime[2] = m_dwStartWaitTime[1] - m_dwStartWaitTime[0];

				if (m_dwStartWaitTime[2] <= 0)
				{
					m_dwStartWaitTime[0] = GetCurrentTime();
					break;
				}

				if (m_dwStartWaitTime[2] > (DWORD)100)
				{
					m_dwStartWaitTime[0] = GetCurrentTime();
					m_nStartStep = 200;
				}
			}
			else
			{
				m_nStartStep = 0;
			}
			break;

		case 200:
			// =============================================================================
			// RUN 키 I/O 입력이 Off 되었는지 검사                                   
			// -> 알람 화면 출력 상태이면 알람 화면 종료 -> 메인 화면으로 전환       
			// =============================================================================
			if(FAS_IO.get_in_bit(st_io_info.i_StartChk, IO_ON) == IO_OFF)
			{
//				if(st_lot_info[LOT_CURR].nLotStatus == LOT_CLEAR)  //2015.0407 james lot 이 진행중이 아닐떄만 동작가능 
//				{
					// jtkim 20150714
					//nRet = clsFunc.OnTestSiteCokStatus();
					//if (nRet == RET_ERROR || st_var.nCokBuffer_Change_End_Flag != 0)
//					if (st_work_info.nOldHifix != st_recipe_info.nCOKBuff_IDNum_Type || st_var.nCokBuffer_Change_End_Flag != 0)
//					{//COK 교체 타이밍에서는 아래 Hifix 및 Conatct 실린더를 처리하면 COK를 교체할 수 없음 				
//						m_nStartStep = 300; //2015.0405 james 
//						break; 
//					}
//				}
				m_nStartStep = 300;
			}

			break;	

		case 300:
			if(OnRunPossibleCheck() == RET_GOOD)
			{
				// 알람이 출력된 상태에서 START 버튼 누른 경우에는 알람을 먼저 종료시킨다
				if (st_handler_info.nMenuNum != IDW_SCREEN_MAIN)	
				{
					::SendMessage(st_handler_info.hWnd, WM_FORM_CHANGE, SCREEN_MAIN, 0);  // 메인 화면으로 전환 요청
				}

				m_nStartStep = 400;

			}			else
			{
				st_handler_info.nRunStatus = dWARNING;

				m_nStartStep = 0;
			}
			break;

		case 400:
			clsFunc.OnMCRun();

			if (st_handler_info.cWndMain != NULL)
			{
//				st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_RECOVERY_CREATE_REQ, 0);
			}
			m_nStartStep = 500;
			break;

		case 500:
			// 20140812 jtkim
			if (st_basic_info.nModeXgemRunChk == YES && st_basic_info.nModeXgem == YES)
			{
				if (clsXgem.m_strOldRecipe != st_basic_info.strDeviceName)
				{
					m_dwStartWaitTime[0] = GetCurrentTime();

					clsXgem.m_nRunStatus = HOST_WAIT;

					clsXgem.OnMcRecipeCreate(0, st_basic_info.strDeviceName, st_path_info.strPathRecipe);

					m_nStartStep = 600;
				}
				else
				{
					clsXgem.OnMcRecipeSelete(st_basic_info.strDeviceName);

					m_nStartStep = 1200;
				}
//				clsXgem.OnMcRecipeSelete(st_basic_info.strDeviceName);
			}
			else
			{
				clsXgem.OnMcRecipeSelete(st_basic_info.strDeviceName);

				m_nStartStep = 1200;
			}
			break;

		case 600:
			// jtkim 20150418
			if (clsXgem.m_nRunStatus == HOST_RUN)
			{
				m_nStartStep = 1200;
			}
			else if (clsXgem.m_nRunStatus == HOST_STOP)
			{
				st_other_info.nBuzYesNo = YES;
				st_other_info.strBoxMsg = _T("XGem RCMD Stop. \r\n RCMD Check");

				if (st_handler_info.cWndMain != NULL)
				{
					st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
				}

				// jtkim 20130625
				clsFunc.OnMCStop();

				m_nStartStep = 0;
			}
			else
			{
				m_dwStartWaitTime[1] = GetCurrentTime();
				m_dwStartWaitTime[2] = m_dwStartWaitTime[1] - m_dwStartWaitTime[0];

				if (m_dwStartWaitTime[2] <= (DWORD)0)
				{
					m_dwStartWaitTime[0] = GetCurrentTime();
					break;
				}

				if (m_dwStartWaitTime[2] > (DWORD)10000)
				{
					if (st_handler_info.cWndList != NULL)	
					{
						//clsFunc.OnStringToChar(_T("[XGEM] Start Check Error"), st_other_info.cNormalMsg);
						clsMem.OnAbNormalMessagWrite(_T("[XGEM] RCMD Start Check Error"));
						//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
					}

					st_other_info.nBuzYesNo = YES;
					st_other_info.strBoxMsg = _T("XGem RCMD TimeOut. \r\n RCMD Check");

					if (st_handler_info.cWndMain != NULL)
					{
						st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
					}
					// jtkim 20130625
					clsFunc.OnMCStop();

					m_nStartStep = 0;
				}
			}
			break;

		case 1000:
			st_handler_info.nRunStatus	= dRECOVERY;
			
			m_nStartStep = 1100;
			break;
			
		case 1100:
			if ( 1 ||
				(st_recovery_info.nRecoveryFlag[PART_MGZ_ELV]		== NO && st_recovery_info.nRecoveryFlag[PART_PCB_SHUTTLE]	== NO) )
			{
				if (st_handler_info.cWndMain != NULL)
				{
//					st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_RECOVERY_DELETE_REQ, 0);
				}
				m_nStartStep = 1200;
			}
			else
			{
				if (st_handler_info.nRunStatus == dSTOP)
				{
					if (st_handler_info.cWndMain != NULL)
					{
//						st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_RECOVERY_DELETE_REQ, 0);
					}
					m_nStartStep = 0;
				}
			}
			break;

		case 1200:
			st_handler_info.nRunStatus = dRUN;

			if (st_work_info.nBdReadyChk == YES)
			{
				st_work_info.nBdReadyChk = NO;
			}

			if (st_handler_info.nMenuLock != FALSE)
			{
				st_handler_info.nMenuLock = FALSE;
			}

			if (st_handler_info.nAlarmScreen )
			{
				st_handler_info.nAlarmScreen = FALSE;
			}

			// 20140812 jtkim
			clsXgem.OnMcStatus(dRUN);

			if (CTL_Lib.m_nMcEmg == YES)
			{
				CTL_Lib.m_nMcEmg = NO;
			}

			if(st_handler_info.cWndTitle != NULL)
			{
				st_handler_info.cWndTitle->PostMessage(WM_STATUS_CHANGE, MACHINE_STATUS, st_handler_info.nRunStatus);
			}

			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
				clsMem.OnNormalMessageWrite(_T("MC Start Status...."));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
			}
			m_nStartStep = 0;
			st_handler_info.mn_userstop = FALSE; //2015.1005

			break;
	}
}

void CRunHandlerCheck::OnFrontMuteOn()
{
	int nNum = 0;
	int nIO;

	if (st_handler_info.nSystemLock != NO)	
	{
		return;
	}

	if (st_handler_info.nMenuLock != NO)	
	{
		return;
	}

	if (st_handler_info.nSwitchLock != NO)	
	{
		return;
	}

	if (m_bFrontMuteFlag == YES)
	{
		return;
	}

	nIO = FAS_IO.get_in_bit(st_io_info.i_LdUldMuteOnChk, IO_ON);
	switch (m_nFrontMuteStep[nNum])
	{
		case 0:
			if (nIO == IO_ON)
			{
				m_dwFrontMuteWaitTime[nNum][0] = GetTickCount();

				m_nFrontMuteStep[nNum] = 100;
			}
			break;

		case 100:
			if (nIO == IO_ON)
			{
				m_dwFrontMuteWaitTime[nNum][1] = GetTickCount();
				m_dwFrontMuteWaitTime[nNum][2] = m_dwFrontMuteWaitTime[nNum][1] - m_dwFrontMuteWaitTime[nNum][0];

				if (m_dwFrontMuteWaitTime[nNum][2] <= (DWORD)0)
				{
					m_dwFrontMuteWaitTime[nNum][0] = GetTickCount();
					break;
				}

				if (m_dwFrontMuteWaitTime[nNum][2] > (DWORD)500)
				{
					m_nFrontMuteStep[nNum] = 300;
				}
			}
			else
			{
				m_nFrontMuteStep[nNum] = 0;
			}
			break;

		case 200:
			if (nIO == IO_OFF)
			{
				m_nFrontMuteStep[nNum] = 300;
			}
			break;

		case 300:
			m_bFrontMuteFlag = YES;

			m_dwFrontWaitTime[0] = GetTickCount();

			FAS_IO.set_out_bit( st_io_info.o_LdUldMuteOn, IO_ON);
			FAS_IO.set_out_bit( st_io_info.o_LdUldMuteOff, IO_OFF);

			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
				clsMem.OnNormalMessageWrite(_T("Load Unload Mute On Status...."));					
			}

			m_nFrontMuteStep[nNum] = 0;
			break;
	}
}


void CRunHandlerCheck::OnFrontMuteOff()
{
	int nNum = 1;
	int nIO;

	if (st_handler_info.nSystemLock != NO)	
	{
		return;
	}

	if (st_handler_info.nMenuLock != NO)	
	{
		return;
	}

	if (st_handler_info.nSwitchLock != NO)	
	{
		return;
	}

	if (m_bFrontMuteFlag == NO)
	{
		return;
	}

	nIO = FAS_IO.get_in_bit(st_io_info.i_LdUldMuteOffChk, IO_ON);
	switch (m_nFrontMuteStep[nNum])
	{
		case 0:
			if (nIO == IO_ON)
			{
				m_dwFrontMuteWaitTime[nNum][0] = GetTickCount();

				m_nFrontMuteStep[nNum] = 100;
			}
			else 
			{
				if (m_bFrontMuteFlag == YES)
				{
					m_dwFrontWaitTime[1] = GetTickCount();
					m_dwFrontWaitTime[2] = m_dwFrontWaitTime[1] - m_dwFrontWaitTime[0];

					if (m_dwFrontWaitTime[2] <= (DWORD) 0)
					{
						m_dwFrontWaitTime[0] = GetTickCount();
						break;
					}

					if (m_dwFrontWaitTime[2] > (DWORD) 15000)
					{
						m_nFrontMuteStep[nNum] = 300;
					}
				}
			}
			break;

		case 100:
			if (nIO == IO_ON)
			{
				m_dwFrontMuteWaitTime[nNum][1] = GetTickCount();
				m_dwFrontMuteWaitTime[nNum][2] = m_dwFrontMuteWaitTime[nNum][1] - m_dwFrontMuteWaitTime[nNum][0];

				if (m_dwFrontMuteWaitTime[nNum][2] <= (DWORD)0)
				{
					m_dwFrontMuteWaitTime[nNum][0] = GetTickCount();
					break;
				}

				if (m_dwFrontMuteWaitTime[nNum][2] > (DWORD)500)
				{
					m_nFrontMuteStep[nNum] = 300;
				}
			}
			else
			{
				m_nFrontMuteStep[nNum] = 0;
			}
			break;

		case 200:
			if (nIO == IO_OFF)
			{
				m_nFrontMuteStep[nNum] = 300;
			}
			break;

		case 300:
			m_bFrontMuteFlag = NO;

			FAS_IO.set_out_bit( st_io_info.o_LdUldMuteOn, IO_OFF);
			FAS_IO.set_out_bit( st_io_info.o_LdUldMuteOff, IO_ON);

			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
				clsMem.OnNormalMessageWrite(_T("Load Unload Mute Off Status...."));					
			}

			m_nFrontMuteStep[nNum] = 0;
			break;
	}
}


void CRunHandlerCheck::OnRightMuteOn()
{
	int nNum = 0;
	int nIO;

	if (st_handler_info.nSystemLock != NO)	
	{
		return;
	}

	if (st_handler_info.nMenuLock != NO)	
	{
		return;
	}

	if (st_handler_info.nSwitchLock != NO)	
	{
		return;
	}

	if (m_bRightMuteFlag == YES)
	{
		return;
	}

	nIO = FAS_IO.get_in_bit(st_io_info.i_RejMuteOnChk, IO_ON);
	switch (m_nRightMuteStep[nNum])
	{
		case 0:
			if (nIO == IO_ON)
			{
				m_dwRightMuteWaitTime[nNum][0] = GetTickCount();

				m_nRightMuteStep[nNum] = 100;
			}
			break;

		case 100:
			if (nIO == IO_ON)
			{
				m_dwRightMuteWaitTime[nNum][1] = GetTickCount();
				m_dwRightMuteWaitTime[nNum][2] = m_dwRightMuteWaitTime[nNum][1] - m_dwRightMuteWaitTime[nNum][0];

				if (m_dwRightMuteWaitTime[nNum][2] <= (DWORD)0)
				{
					m_dwRightMuteWaitTime[nNum][0] = GetTickCount();
					break;
				}

				if (m_dwRightMuteWaitTime[nNum][2] > (DWORD)500)
				{
					m_nRightMuteStep[nNum] = 300;
				}
			}
			else
			{
				m_nRightMuteStep[nNum] = 0;
			}
			break;

		case 200:
			if (nIO == IO_OFF)
			{
				m_nRightMuteStep[nNum] = 300;
			}
			break;

		case 300:
			m_bRightMuteFlag = YES;

			m_dwRightWaitTime[0] = GetTickCount();

			FAS_IO.set_out_bit( st_io_info.o_RejMuteOn, IO_ON);
			FAS_IO.set_out_bit( st_io_info.o_RejMuteOff, IO_OFF);

			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
				clsMem.OnNormalMessageWrite(_T("Reject Area Mute On Status...."));					
			}

			m_nRightMuteStep[nNum] = 0;
			break;
	}
}


void CRunHandlerCheck::OnRightMuteOff()
{
	int nNum = 1;
	int nIO;

	if (st_handler_info.nSystemLock != NO)	
	{
		return;
	}

	if (st_handler_info.nMenuLock != NO)	
	{
		return;
	}

	if (st_handler_info.nSwitchLock != NO)	
	{
		return;
	}

	if (m_bRightMuteFlag == NO)
	{
		return;
	}

	nIO = FAS_IO.get_in_bit(st_io_info.i_RejMuteOffChk, IO_ON);
	switch (m_nRightMuteStep[nNum])
	{
		case 0:
			if (nIO == IO_ON)
			{
				m_dwRightMuteWaitTime[nNum][0] = GetTickCount();

				m_nRightMuteStep[nNum] = 100;
			}
			else 
			{
				if (m_bRightMuteFlag == YES)
				{
					m_dwRightWaitTime[1] = GetTickCount();
					m_dwRightWaitTime[2] = m_dwRightWaitTime[1] - m_dwRightWaitTime[0];

					if (m_dwRightWaitTime[2] <= (DWORD) 0)
					{
						m_dwRightWaitTime[0] = GetTickCount();
						break;
					}

					if (m_dwRightWaitTime[2] > (DWORD) 15000)
					{
						m_nRightMuteStep[nNum] = 300;
					}
				}
			}
			break;

		case 100:
			if (nIO == IO_ON)
			{
				m_dwRightMuteWaitTime[nNum][1] = GetTickCount();
				m_dwRightMuteWaitTime[nNum][2] = m_dwRightMuteWaitTime[nNum][1] - m_dwRightMuteWaitTime[nNum][0];

				if (m_dwRightMuteWaitTime[nNum][2] <= (DWORD)0)
				{
					m_dwRightMuteWaitTime[nNum][0] = GetTickCount();
					break;
				}

				if (m_dwRightMuteWaitTime[nNum][2] > (DWORD)500)
				{
					m_nRightMuteStep[nNum] = 300;
				}
			}
			else
			{
				m_nRightMuteStep[nNum] = 0;
			}
			break;

		case 200:
			if (nIO == IO_OFF)
			{
				m_nRightMuteStep[nNum] = 300;
			}
			break;

		case 300:
			m_bRightMuteFlag = NO;

			FAS_IO.set_out_bit( st_io_info.o_RejMuteOn, IO_OFF);
			FAS_IO.set_out_bit( st_io_info.o_RejMuteOff, IO_ON);

			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
				clsMem.OnNormalMessageWrite(_T("Reject Arae Mute Off Status...."));					
			}

			m_nRightMuteStep[nNum] = 0;
			break;
	}
}


void CRunHandlerCheck::OnStopCheck()
{
	if (st_handler_info.nSystemLock != NO)	
	{
		return;
	}

	if (st_handler_info.nSwitchLock != NO)	
	{
		return;
	}

	if (st_handler_info.nRunStatus == dSTOP)	
	{
		return;
	}

	switch(m_nStopStep)
	{
		case 0:
			if(FAS_IO.get_in_bit(st_io_info.i_StopChk, IO_OFF) == IO_ON)
			{
				if (FAS_IO.get_in_bit(st_io_info.i_StartChk, IO_ON) == IO_ON)
				{
					break;
				}

				m_dwStopWaitTime[0] = GetCurrentTime();
				m_nStopStep = 100;
			}
			break;

		case 100:
			if(FAS_IO.get_in_bit(st_io_info.i_StopChk, IO_OFF) == IO_ON)
			{
				m_dwStopWaitTime[1] = GetCurrentTime();
				m_dwStopWaitTime[2] = m_dwStopWaitTime[1] - m_dwStopWaitTime[0];

				if(m_dwStopWaitTime[2] <= 0)
				{
					m_dwStopWaitTime[0] = GetCurrentTime();
					break;
				}

				if(m_dwStopWaitTime[2] > (DWORD)100)
				{
					m_dwStopWaitTime[0] = GetCurrentTime();
					m_nStopStep = 200;
				}
			}
			else
			{
				m_nStopStep = 0;
			}
			break;

		case 200:
			if(FAS_IO.get_in_bit(st_io_info.i_StopChk, IO_ON) == IO_OFF)
			{
				// 초기화 작업중에 STOP 버튼이 눌리면 초기화 작업을 중단한다
				if (st_handler_info.nRunStatus == dINIT)
				{
					m_nStopStep = 0;
					break;
				}
				// jtkim 20130625
				clsFunc.OnMCStop();

				// 20140812 jtkim
				clsXgem.OnMcStatus(dSTOP);

				// jtkim 20150721
				if (st_work_info.nCycleYesNo == YES)
				{
					st_work_info.nCycleYesNo = NO;
				}

				if(st_handler_info.cWndTitle != NULL)
				{
					st_handler_info.cWndTitle->PostMessage(WM_STATUS_CHANGE, MACHINE_STATUS, st_handler_info.nRunStatus);
				}

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnNormalMessageWrite(_T("MC Stop Status...."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
				}
				m_nStopStep = 0;

				st_handler_info.mn_userstop = CTL_YES;//2015.1005

			}
			break;
	}
}


void CRunHandlerCheck::OnResetCheck()
{
	if (st_handler_info.nSystemLock != NO)	
	{
		return;
	}

	if (st_handler_info.nSwitchLock != NO)	
	{
		return;
	}

	switch(m_nResetStep)
	{
		case 0:
			// **********************************************************************
			//  Reset 키 I/O 입력이 On 상태인지 확인                                 
			// **********************************************************************
			if(FAS_IO.get_in_bit(st_io_info.i_AlarmChk, IO_OFF) == IO_ON)
			{
				m_dwResetWaitTime[0] = GetCurrentTime();
				m_nResetStep = 100;
			}
			break;

		case 100: 
			if(FAS_IO.get_in_bit(st_io_info.i_AlarmChk, IO_OFF) == IO_ON)
			{
				m_dwResetWaitTime[1] = GetCurrentTime();
				m_dwResetWaitTime[2] = m_dwResetWaitTime[1] - m_dwResetWaitTime[0];

				if(m_dwResetWaitTime[2] <= 0)
				{
					m_dwResetWaitTime[0] = GetCurrentTime();
					break;
				}

				if(m_dwResetWaitTime[2] > (DWORD)100)
				{
					m_nResetStep = 200;
				}
			}
			else
			{
				m_nResetStep = 0;
			}
			break;

		case 200:
			if (FAS_IO.get_in_bit(st_io_info.i_AlarmChk, IO_ON) == IO_OFF)
			{
				st_alarm_info.nResetStatus = NO;

				if (st_alarm_info.nResetStatus == YES)
				{
					st_alarm_info.nResetStatus = NO;  // 알람 리셋 작업 필요 없음
					
					if (st_handler_info.cWndList != NULL)	
					{
						//wsprintfA(st_other_info.cNormalMsg, "%S", _T("[JAM RESET] JAM RESET."));
						//clsFunc.OnStringToChar(_T("[JAM RESET] JAM RESET."), st_other_info.cNormalMsg);
						clsMem.OnNormalMessageWrite(_T("[JAM RESET] JAM RESET."));
						//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
					}

					// 설비 상태가 RUN 상태이면 사운드만 종료하고 RUN 상태 아니면 [STOP] 상태로 변경한다
					// jtkim 20130625
					clsFunc.OnMCStop();  // 설비 STOP 상태에 대한 버튼 램프 상태 변경

					// 20140812 jtkim
					clsXgem.OnMcStatus(dSTOP);

					clsFunc.OnSound(IO_OFF);
				}
				else
				{
					if (st_handler_info.cWndList != NULL)	
					{
						//wsprintfA(st_other_info.cNormalMsg, "%S", _T("[SOUND] OFF."));
						//clsFunc.OnStringToChar(_T("[SOUND] OFF."), st_other_info.cNormalMsg);
						clsMem.OnNormalMessageWrite(_T("[SOUND] OFF."));
						//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
					}
					// jtkim 20130625
					clsFunc.OnSound(IO_OFF);  // 부저 사운드 ON/OFF
				}

				m_nResetStep = 0;
			}
			break;
	}
}


void CRunHandlerCheck::OnBuzzerCheck()
{
	if (st_handler_info.nSystemLock != NO)	
	{
		return;
	}

	if (st_handler_info.nSwitchLock != NO)	
	{
		return;
	}

	switch(m_nBuzzerStep)
	{
		case 0:
			if(FAS_IO.get_in_bit(st_io_info.i_BuzzChk, IO_OFF) == IO_ON)
			{
				m_dwBuzzerWaitTime[0] = GetCurrentTime();
				m_nBuzzerStep = 100;
			}
			break;
			
		case 100: 
			if(FAS_IO.get_in_bit(st_io_info.i_BuzzChk, IO_ON) == IO_ON)
			{
				m_dwBuzzerWaitTime[1] = GetCurrentTime();
				m_dwBuzzerWaitTime[2] = m_dwBuzzerWaitTime[1] - m_dwBuzzerWaitTime[0];

				if(m_dwBuzzerWaitTime[2] <= 0)
				{
					m_dwBuzzerWaitTime[0] = GetCurrentTime();
					break;
				}

				if(m_dwBuzzerWaitTime[2] > (DWORD)100)
				{
					m_nBuzzerStep = 200;
				}
			}
			else
			{
				m_nBuzzerStep = 0;
			}
			break;
			
		case 200:
			if (FAS_IO.get_in_bit(st_io_info.i_BuzzChk, IO_OFF) == IO_OFF)
			{
				if (st_handler_info.cWndList != NULL)	
				{
					//wsprintfA(st_other_info.cNormalMsg, "%S", _T("[SOUND] OFF."));
					//clsFunc.OnStringToChar(_T("[SOUND] OFF."), st_other_info.cNormalMsg);
					clsMem.OnNormalMessageWrite(_T("[SOUND] OFF."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
				}
				// jtkim 20130625
				clsFunc.OnSound(IO_OFF);  // 부저 사운드 ON/OFF
				
				m_nBuzzerStep = 0;
			}
			break;
	}
}


void CRunHandlerCheck::OnAutoCheck()
{
	int nRet;

	if (st_handler_info.nSystemLock != NO)	
	{
		return;
	}

	if (st_handler_info.nSwitchLock != NO)	
	{
		return;
	}

	if(st_handler_info.nRunStatus	== dRUN || st_handler_info.nRunStatus	== dINIT)
	{
		return;
	}

	if(st_handler_info.nMachineMode	== MACHINE_AUTO)
	{
		return;
	}

	switch(m_nAutoStep)
	{
		case 0:		//2014.01.21 주석처리
			if(FAS_IO.get_in_bit(st_io_info.i_FrontSelectSwChk, IO_OFF) == IO_ON &&
			   FAS_IO.get_in_bit(st_io_info.i_RearSelectSwChk, IO_OFF)	== IO_ON)			
			{
				//if (FAS_IO.get_in_bit(st_io_info.i_AutoModeChk, IO_OFF) == IO_ON)
				{
					m_dwAutoWaitTime[0] = GetTickCount();
					m_nAutoStep = 100;
				}
			}
			break;

		case 100:	//2014.01.21 주석처리
			if( FAS_IO.get_in_bit(st_io_info.i_FrontSelectSwChk, IO_OFF) == IO_ON &&
			   FAS_IO.get_in_bit(st_io_info.i_RearSelectSwChk, IO_OFF)	== IO_ON )
			{
				m_dwAutoWaitTime[1] = GetTickCount();
				m_dwAutoWaitTime[2] = m_dwAutoWaitTime[1] - m_dwAutoWaitTime[0];

				if(m_dwAutoWaitTime[2] <= 0)
				{
					m_dwAutoWaitTime[0] = GetTickCount();
					break;
				}

				if(m_dwAutoWaitTime[2] > (DWORD)200)
				{
					m_nAutoStep = 200;
				}
			}
			else
			{
				m_nAutoStep = 0;
			}
			break;

		case 200:
			nRet = clsFunc.OnDoorOpenCheck();

			if(nRet == RET_GOOD)
			{
				m_nAutoStep = 400;
			}
			else if(nRet != RET_PROCEED)
			{
				st_handler_info.nRunStatus			= dWARNING;
				st_alarm_info.nAlarmAssignSection	= 6000;

				m_nAutoStep = 300;
			}
			break;

		case 300:
			nRet = clsFunc.OnDoorOpenCheck();

			if(nRet == RET_GOOD)
			{
				m_nAutoStep = 0;
			}
			break;

		case 400:
			m_dwAutoWaitTime[0] = GetTickCount();
			m_nAutoStep = 500;
			break;

		case 500:
			m_dwAutoWaitTime[1] = GetTickCount();
			m_dwAutoWaitTime[2] = m_dwAutoWaitTime[1] - m_dwAutoWaitTime[0];
			
			if(m_dwAutoWaitTime[2] <= 0)
			{
				m_dwAutoWaitTime[0] = GetTickCount();
				break;
			}

			if(m_dwAutoWaitTime[2] > (DWORD)500)
			{
				m_nAutoStep = 600;
			}
			break;

		case 600:
			clsFunc.OnDoorClose();

			st_handler_info.nMachineMode = MACHINE_AUTO;
			m_nAutoStep = 0;
			break;
	}
}


void CRunHandlerCheck::OnManualCheck()
{
	if (st_handler_info.nSystemLock != NO)	
	{
		return;
	}

	if (st_handler_info.nSwitchLock != NO)	
	{
		return;
	}

//	if(st_handler_info.nRunStatus	!= dSTOP)
	if(st_handler_info.nRunStatus	== dRUN || st_handler_info.nRunStatus	== dINIT)
	{
		return;
	}

	if(st_handler_info.nMachineMode	== MACHINE_MANUAL)
	{
		return;
	}

	switch(m_nManualStep)
	{
		case 0:		//2014.01.21 주석처리
			if(FAS_IO.get_in_bit(st_io_info.i_FrontSelectSwChk, IO_ON) == IO_OFF ||
			   FAS_IO.get_in_bit(st_io_info.i_RearSelectSwChk, IO_ON)  == IO_OFF)
			{
				m_dwManualWaitTime[0] = GetTickCount();
				m_nManualStep = 100;
			}

			break;

		case 100:	//2014.01.21 주석처리
			if(FAS_IO.get_in_bit(st_io_info.i_FrontSelectSwChk, IO_ON) == IO_OFF ||
			   FAS_IO.get_in_bit(st_io_info.i_RearSelectSwChk, IO_ON)  == IO_OFF)
			{
				m_dwManualWaitTime[1] = GetTickCount();
				m_dwManualWaitTime[2] = m_dwManualWaitTime[1] - m_dwManualWaitTime[0];

				if(m_dwManualWaitTime[2] <= 0)
				{
					m_dwManualWaitTime[0] = GetTickCount();
					break;
				}

				if(m_dwManualWaitTime[2] > (DWORD)200)
				{
					m_nManualStep = 200;
				}
			}
			else
			{
				m_nManualStep = 0;
			}
			break;

		case 200:
			m_dwManualWaitTime[0] = GetTickCount();
			m_nManualStep = 300;
			break;

		case 300:
			m_dwManualWaitTime[1] = GetTickCount();
			m_dwManualWaitTime[2] = m_dwManualWaitTime[1] - m_dwManualWaitTime[0];
			
			if(m_dwManualWaitTime[2] <= 0)
			{
				m_dwManualWaitTime[0] = GetTickCount();
				break;
			}

			if(m_dwManualWaitTime[2] > (DWORD)500)
			{
				m_nManualStep = 400;
			}
			break;

		case 400:
			st_handler_info.nMachineMode = MACHINE_MANUAL;

			// 매뉴얼 모드로 변경되면 안전상 STOP 상태로 변경한다
			// jtkim 20130625
			clsFunc.OnMCStop();  // 설비 STOP 상태에 대한 버튼 램프 상태 변경
			//clsFunc.OnDoorOpen();  // SAFETY DOOR LOCK 해제
			m_nManualStep = 0;
			break;
	}
}


int CRunHandlerCheck::OnRunPossibleCheck()
{
	CString strAlram;

	if (clsFunc.OnMotorPowerCheck() == RET_ERROR)
	{
		return BD_ERROR;
	}

	if (clsFunc.OnDoorOpenCheck() == RET_ERROR)
	{
		return RET_ERROR;
	}

	if (clsFunc.OnLightCurtainCheck() == RET_ERROR)
	{
		return RET_ERROR;
	}
	
/*
// 	if (FAS_IO.get_in_bit(st_io_info.i_CokChangeCokClampBwdChk, IO_ON)	== IO_ON ||
// 		FAS_IO.get_in_bit(st_io_info.i_CokChangeCokClampFwdChk, IO_OFF)	== IO_OFF)
// 	{
// 		CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, _T("804007"));
// 
// 		return RET_ERROR;
// 	}

	if (clsFunc.OnMpCheck() == RET_ERROR)
	{
		return RET_ERROR;
	}
*/
	if (FAS_IO.get_in_bit(st_io_info.i_FrontSelectSwChk, IO_ON) == IO_OFF)
	{
		if (st_handler_info.cWndMain != NULL)
		{
			//st_other_info.nBuzYesNo = NO;
			if (FAS_IO.get_in_bit(st_io_info.i_FrontSelectSwChk, IO_ON) == IO_OFF)
			{
				st_other_info.strBoxMsg = _T("Please Check Front Auto Key Status.");
			}
			else
			{
				st_other_info.strBoxMsg = _T("Please Check Rear Auto Key Status.");
			}

			if (st_handler_info.cWndMain != NULL)
			{
				st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
			}
		}

		return RET_ERROR;
	}
	else
	{
		if (FAS_IO.get_in_bit(st_io_info.i_AutoModeChk, IO_ON) == IO_OFF)
		{
			//st_other_info.nBuzYesNo = NO;
			st_other_info.strBoxMsg = _T("Please Check Rear Auto Key Status.");

			if (st_handler_info.cWndMain != NULL)
			{
				st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
			}
		}
	}
/*
	// 20150404 jtkim
	for (i=0; i<M_MAX_MOTOR_NUM; i++)
	{
		if(i == 10 || i == 11 || i == 23 || i == 24) continue; 

		if (COMI.mn_homechk_flag[i] == BD_NO)
		{
			strAlram.Format(_T("%02d0001"), i); 
			CTL_Lib.Alarm_Error_Occurrence(6000, dMOTOR_WARNING, strAlram);
			
			return BD_ERROR;
		}
	}
*/
	return RET_GOOD;
}

void	CRunHandlerCheck::	OnLdUldMuteOnOff()
{
	if (st_handler_info.nSystemLock != NO)	
	{
		return;
	}

	if (st_handler_info.nMenuLock != NO)	
	{
		return;
	}

	if (st_handler_info.nSwitchLock != NO)	
	{
		return;
	}

	if ( st_handler_info.nRunStatus ==dWARNING )	
	{
		return;
	}

// 	if (st_handler_info.nInitialSuccess != YES)	
// 	{
// 		return;
// 	}
	switch(m_nLdUldMuteStep)
	{
		case 0:
			if(FAS_IO.get_in_bit(st_io_info.i_LdUldMuteOnChk, IO_OFF) == IO_ON && FAS_IO.get_in_bit(st_io_info.i_LdUldMuteOffChk, IO_OFF) == IO_OFF)
			{
				m_dwLdUldMuteWaitTime[0] = GetCurrentTime();
				m_nLdUldMuteStep = 100;
			}
			else if(FAS_IO.get_in_bit(st_io_info.i_LdUldMuteOnChk, IO_OFF) == IO_OFF && FAS_IO.get_in_bit(st_io_info.i_LdUldMuteOffChk, IO_OFF) == IO_ON)
			{
				m_dwLdUldMuteWaitTime[0] = GetCurrentTime();
				m_nLdUldMuteStep = 110;
			}
			break;

		case 100:
			if(FAS_IO.get_in_bit(st_io_info.i_LdUldMuteOnChk, IO_OFF) == IO_ON )
			{
				m_dwLdUldMuteWaitTime[1] = GetCurrentTime();
				m_dwLdUldMuteWaitTime[2] = m_dwLdUldMuteWaitTime[1] - m_dwLdUldMuteWaitTime[0];

				if(m_dwLdUldMuteWaitTime[2] <= 0)
				{
					m_dwLdUldMuteWaitTime[0] = GetCurrentTime();
					break;
				}
// 				if(m_dwLdUldMuteWaitTime[2] > (DWORD)30)
// 				{
// 					m_dwLdUldMuteWaitTime[0] = GetCurrentTime();
					m_nLdUldMuteStep = 200;
//				}
// 				else
// 				{
// 					m_nLdUldMuteStep = 0;
// 				}
			}
			else
			{
				m_nLdUldMuteStep = 0;
			}
			break;

		case 110:
			if(/*FAS_IO.get_in_bit(st_io_info.i_LdUldMuteOnChk, IO_OFF) == IO_OFF && */FAS_IO.get_in_bit(st_io_info.i_LdUldMuteOffChk, IO_OFF) == IO_ON)
			{
				m_dwLdUldMuteWaitTime[1] = GetCurrentTime();
				m_dwLdUldMuteWaitTime[2] = m_dwLdUldMuteWaitTime[1] - m_dwLdUldMuteWaitTime[0];

				if(m_dwLdUldMuteWaitTime[2] <= 0)
				{
					m_dwLdUldMuteWaitTime[0] = GetCurrentTime();
					break;
				}
// 				if(m_dwLdUldMuteWaitTime[2] > (DWORD)20)
// 				{
// 					m_dwLdUldMuteWaitTime[0] = GetCurrentTime();
					m_nLdUldMuteStep = 300;
//				}
// 				else
// 				{
// 					m_nLdUldMuteStep = 0;
// 				}
			}
			else
			{
				m_nLdUldMuteStep = 0;
			}
			break;

		case 200:
			if(FAS_IO.get_in_bit(st_io_info.i_LdUldMuteOnChk, IO_ON) == IO_ON)// && FAS_IO.get_in_bit(st_io_info.i_LdUldMuteOffChk, IO_OFF) == IO_OFF)
			{
				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnNormalMessageWrite(_T("Load Unload Mute On Status...."));					
				}
				FAS_IO.set_out_bit( st_io_info.o_LdUldMuteOn, IO_ON);
				FAS_IO.set_out_bit( st_io_info.o_LdUldMuteOff, IO_OFF);
				m_nLdUldMuteStep = 0;
			}
			else
			{
				m_nLdUldMuteStep = 0;
			}
			break;

		case 300:
			if(/*FAS_IO.get_in_bit(st_io_info.i_LdUldMuteOnChk, IO_OFF) == IO_OFF)// && */FAS_IO.get_in_bit(st_io_info.i_LdUldMuteOffChk, IO_ON) == IO_ON)
			{
				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnNormalMessageWrite(_T("Load Unload Mute OFF Status...."));					
				}
				FAS_IO.set_out_bit( st_io_info.o_LdUldMuteOff, IO_ON);
				FAS_IO.set_out_bit( st_io_info.o_LdUldMuteOn, IO_OFF);
				m_nLdUldMuteStep = 0;
			}
			else
			{
				m_nLdUldMuteStep = 0;
			}
			break;
		}
		return;
}

void	CRunHandlerCheck::	OnRejectMuteOnOff()
{
	if (st_handler_info.nSystemLock != NO)	
	{
		return;
	}

	if (st_handler_info.nMenuLock != NO)	
	{
		return;
	}

	if (st_handler_info.nSwitchLock != NO)	
	{
		return;
	}

	if ( st_handler_info.nRunStatus ==dWARNING )	
	{
		return;
	}

// 	if (st_handler_info.nInitialSuccess != YES)	
// 	{
// 		return;
// 	}
	switch(m_nRejMuteStep)
	{
		case 0:
			if(FAS_IO.get_in_bit(st_io_info.i_RejMuteOnChk, IO_OFF) == IO_ON && FAS_IO.get_in_bit(st_io_info.i_RejMuteOffChk, IO_OFF) == IO_OFF)
			{
				m_dwRejMuteWaitTime[0] = GetCurrentTime();
				m_nRejMuteStep = 100;
			}
			else if(FAS_IO.get_in_bit(st_io_info.i_RejMuteOnChk, IO_OFF) == IO_OFF && FAS_IO.get_in_bit(st_io_info.i_RejMuteOffChk, IO_OFF) == IO_ON)
			{
				m_dwRejMuteWaitTime[0] = GetCurrentTime();
				m_nRejMuteStep = 110;
			}
			break;

		case 100:
			if(FAS_IO.get_in_bit(st_io_info.i_RejMuteOnChk, IO_OFF) == IO_ON && FAS_IO.get_in_bit(st_io_info.i_RejMuteOffChk, IO_OFF) == IO_OFF)
			{
				m_dwRejMuteWaitTime[1] = GetCurrentTime();
				m_dwRejMuteWaitTime[2] = m_dwRejMuteWaitTime[1] - m_dwRejMuteWaitTime[0];

				if(m_dwRejMuteWaitTime[2] <= 0)
				{
					m_dwRejMuteWaitTime[0] = GetCurrentTime();
					break;
				}
				if(m_dwRejMuteWaitTime[2] > (DWORD)100)
				{
					m_dwRejMuteWaitTime[0] = GetCurrentTime();
					m_nRejMuteStep = 200;
				}
				else
				{
					m_nRejMuteStep = 0;
				}
			}
			break;

		case 110:
			if(FAS_IO.get_in_bit(st_io_info.i_RejMuteOnChk, IO_ON) == IO_ON )//&& FAS_IO.get_in_bit(st_io_info.i_RejMuteOffChk, IO_OFF) == IO_ON)
			{
				m_dwRejMuteWaitTime[1] = GetCurrentTime();
				m_dwRejMuteWaitTime[2] = m_dwRejMuteWaitTime[1] - m_dwRejMuteWaitTime[0];

				if(m_dwRejMuteWaitTime[2] <= 0)
				{
					m_dwRejMuteWaitTime[0] = GetCurrentTime();
					break;
				}
				if(m_dwRejMuteWaitTime[2] > (DWORD)100)
				{
					m_dwRejMuteWaitTime[0] = GetCurrentTime();
					m_nRejMuteStep = 300;
				}
				else
				{
					m_nRejMuteStep = 0;
				}
			}
			break;

		case 200:
			if(FAS_IO.get_in_bit(st_io_info.i_RejMuteOnChk, IO_ON) == IO_ON )//&& FAS_IO.get_in_bit(st_io_info.i_RejMuteOffChk, IO_OFF) == IO_OFF)
			{
				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnNormalMessageWrite(_T("Reject Mute On Status...."));					
				}
				FAS_IO.set_out_bit( st_io_info.o_RejMuteOn, IO_ON);
				FAS_IO.set_out_bit( st_io_info.o_RejMuteOff, IO_OFF);
				m_nRejMuteStep = 0;
			}
			break;

		case 300:
			if(/*FAS_IO.get_in_bit(st_io_info.i_RejMuteOnChk, IO_OFF) == IO_OFF && */FAS_IO.get_in_bit(st_io_info.i_RejMuteOffChk, IO_ON) == IO_ON)
			{
				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnNormalMessageWrite(_T("Reject Mute OFF Status...."));					
				}
				FAS_IO.set_out_bit( st_io_info.o_RejMuteOff, IO_ON);
				FAS_IO.set_out_bit( st_io_info.o_RejMuteOn, IO_OFF);
				m_nRejMuteStep = 0;
			}
			break;
		}
		return;
}


void CRunHandlerCheck::OnLightCurtainCheck()
{
	int nRet;
	

	if (st_handler_info.nRunStatus == dSTOP) return;

	nRet = clsFunc.OnLightCurtainCheck();
}
