// RunLdUldRobot.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "RunLdUldRobot.h"
#include "Variable.h"
#include "FastechPublic_IO.h"
#include "CtlBd_Library.h"

//james 2016.0802 
#include "CtlBd_Variable.h"
#include "PublicFunction.h"
#include "ComizoaPublic.h"
#include "LogCtrl.h"
#include "Barcode_1D.h"
#include "Barcode_2D.h"
#include "AlgMemory.h"
#include "LogFromat.h"

// CRunLdUldRobot
CRunLdUldRobot clsRunLdUldRobot;

CRunLdUldRobot::CRunLdUldRobot()
{

	m_nRunStep	= -1;
	m_nInitStep	= -1;
	
	m_nPickerPara = LD_ULD_PICKER_PARA;//1
	m_nRobotSite = THD_WORK_RBT;

	//linear move 과련 셋팅
	m_nLinearMove_Index	= 0;   //현재 map index
	m_lAxisCnt	= 2;            //현재의 IndexNum에서 사용 가능한 모터 수량 최대 4개
	m_lpAxisNum[0] = M_WORK_RBT_Y; //m_lpAxisNum[4];        //현재의 IndexNum에서 사용하는 모터 실재 번호를 가진다
	m_lpAxisNum[1] = M_WORK_RBT_X;
	m_lpAxisNum[2] = M_WORK_RBT_Z;

	m_nRobot_X = M_WORK_RBT_X;
	m_nRobot_Y = M_WORK_RBT_Y;
	m_nRobot_Z = M_WORK_RBT_Z; 

	m_dpTargetPosList[0]  = 0; //m_dpTargetPosList[4];		   //최대 4 포인트를 한번에 이동가능하다
	m_dpTargetPosList[1]  = 0;
	m_dpTargetPosList[2]  = 0;

 	m_dpSpdRatio[0] = 100; 		//	m_dpSpdRatio[3];        //속도비율 [0]:Vel, [1]:Acc, [2]:Dec
	m_dpSpdRatio[1] = 100;
	m_dpSpdRatio[2] = 100;

	//2016.1203
	m_dwCycleTime[0][0] = 0;
	m_dwCycleTime[1][0] = 0;
	m_strLotNo[0] = m_strPartNo[0] = _T("");
	m_strLotNo[1] = m_strPartNo[1] = _T("");
}

CRunLdUldRobot::~CRunLdUldRobot()
{
}


void CRunLdUldRobot::OnThreadRun()
{
	//james 2016.0804
	if (st_handler_info.nSystemLock != FALSE)  return;	// 현재 시스템 Lock 상태인 경우 리턴  
	

	switch(st_handler_info.nRunStatus)
	{
		case dINIT:
			OnRunInit();
			break;

		case dRUN:
			//break;
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

void CRunLdUldRobot::OnRunInit()
{
	int nRet_1, nRet_2;

	if (st_sync_info.nInit_Flag[THD_WORK_RBT] != INIT_CLEAR)		return;		//INIT_CLEAR 일때만 초기화 작업을 한다. 초기화가 끝나면 INIT_CLEAR -> INIT_READY가 되기 떄문에...
	 
	switch (m_nInitStep)
	{
		case -1:		 

			m_nInitStep = 0;
			break;

		case 0:		

			m_nInitStep = 100;
			break;

		case 100:  
			nRet_1 = COMI.HomeCheck_Mot(m_nRobot_Z, MOT_SENS_ELM, MOT_TIMEOUT) ; //
			if(nRet_1 == BD_GOOD)
			{
				COMI.mn_homechk_flag[m_nRobot_Z] = CTL_YES;
				m_nInitStep = 110;
			}
			else if(nRet_1 == BD_ERROR)
			{
				CTL_Lib.mn_run_status = CTL_dWARNING;
				CTL_Lib.Alarm_Error_Occurrence(1001, CTL_Lib.mn_run_status, COMI.strCode);
				m_nInitStep = 100;
			}			
			break;

		case 110:
			Set_WorkRbtPicker_UpDown(IO_OFF);
			m_nInitStep = 111;
			break;

		case 111:
			nRet_1 = Chk_WorkRbtPicker_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nInitStep = 200;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(1002, dWARNING, m_strAlarmCode);
				m_nInitStep = 110;
			}
			break;


		case 200:  
			nRet_1 = COMI.HomeCheck_Mot(m_nRobot_X, MOT_SENS_ELM, MOT_TIMEOUT) ; //
			if(nRet_1 == BD_GOOD)
			{
				COMI.mn_homechk_flag[m_nRobot_X] = CTL_YES;
				m_nInitStep = 300;
			}
			else if(nRet_1 == BD_ERROR)
			{
				CTL_Lib.mn_run_status = CTL_dWARNING;
				CTL_Lib.Alarm_Error_Occurrence(1003, CTL_Lib.mn_run_status, COMI.strCode);
				m_nInitStep = 200;
			}			
			break;

		case 300:  
			nRet_1 = COMI.HomeCheck_Mot(m_nRobot_Y, MOT_SENS_ELM, MOT_TIMEOUT) ; //
			if(nRet_1 == BD_GOOD)
			{
				COMI.mn_homechk_flag[m_nRobot_Y] = CTL_YES;
				m_nInitStep = 400;
			}
			else if(nRet_1 == BD_ERROR)
			{
				CTL_Lib.mn_run_status = CTL_dWARNING;
				CTL_Lib.Alarm_Error_Occurrence(1004, CTL_Lib.mn_run_status, COMI.strCode);
				m_nInitStep = 200;
			}			
			break;

		case 400://james 2016.0827 그리퍼 디바이스 체크 정보 수정/보완, Gripper Off(클림프상태)에서 IO_OFF시 디바이스 있는상태, 그외는 ON/OFF 산관없이 모두 디바이스 없는 상태
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_LdUldPickDvcChk,	IO_OFF);
			nRet_2 = FAS_IO.get_out_bit(st_io_info.o_LdUldPickGripCyl,	IO_OFF); //clamp 상태
			if(nRet_1 == IO_ON && nRet_2 == IO_OFF) 
			{
				m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_LdUldPickDvcChk); 
				CTL_Lib.Alarm_Error_Occurrence(1005, dWARNING, m_strAlarmCode);						
			}
			else//IO_OFF 자재 있음 B접점
			{				
				m_nInitStep = 500;		 
			}
			break;

		case 500:
			Set_WorkRbtRotator_OnOff(IO_OFF);
			m_nInitStep = 510;
			break;

		case 510:
			nRet_1 = Chk_WorkRbtRotator_OnOff(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nInitStep = 600;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(1006, dWARNING, m_strAlarmCode);
				m_nInitStep = 100;
			}
			break;

		case 600:
			m_nInitStep = 900;
			break;

		case 900:
			st_sync_info.nInit_Flag[THD_WORK_RBT] = INIT_READY; 
			 
			m_nInitStep = 0;
			break;	

		default :
			break;

	}
}

void CRunLdUldRobot::OnRunMove()
{
	int nRet_1, nRet_2=0, nRet_3=0;
	int nCount = 0, nBin = 0, nFlag = 0, nTemp =0, i=0;
	CString strMsg;

	CString strTable[5];
	double dAve;

	clsFunc.OnThreadFunctionStepTrace(31, m_nRunStep);	

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//2016.0921 james LOT END 체크 부분 
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if(st_lot_info[LOT_CURR].nLotStatus >= LOT_END_START)
	{
		if(st_lot_info[LOT_CURR].nLot_THD_Status[THD_WORK_RBT] == LOT_END && 
		   st_lot_info[LOT_CURR].nLot_THD_Status[st_lot_info[LOT_CURR].nStacker_Good_THD_Num] == LOT_END && 
		   st_lot_info[LOT_CURR].nLot_THD_Status[st_lot_info[LOT_CURR].nStacker_Reject_THD_Num] == LOT_END) //최종 lot End 조건
		{
			st_var.nTestDvcOutFlag = 0 ; // 

			//st_lot_info[LOT_CURR].nLotEnd[THD_UNDER_UNLOAD_TRAY_STACKER_SITE] = CTL_NO;
			//for(i=0; i<THREAD_MAX_SITE; i++) st_lot_info[LOT_CURR].nLotEnd[i] = CTL_NO; //2015.0302 
			//st_lot_info[LOT_CURR].nLotStatus = LOT_CLEAR;
			// lot end 추가
				 
			if (st_basic_info.nModeInterface == EQP_ON_LINE)
			{/*

				// jtkim 20150415
				if(FAS_IO.get_in_bit(st_io_info.i_UnldTrayRail1Unld2OutTrayChk, IO_ON) == IO_ON && 
					FAS_IO.get_in_bit(st_io_info.i_UnldTrayRail1Unld2LoTrayChk,	IO_OFF) == IO_OFF) 
				{
					FAS_IO.set_out_bit(st_io_info.o_UnldTrayMotOnOff, IO_OFF); //AC Motor off
				}

				clsInterfaceC.OnOnLineEnd(st_lot_info[LOT_CURR].strLotNo, 
											st_lot_info[LOT_CURR].strPartNo,
											st_count_info.nInCount[0][0]);

				st_track_info.nTestEndBin		= NO;
				st_track_info.strLotNo			= st_lot_info[LOT_CURR].strLotNo;
				st_track_info.strPartNo			= st_lot_info[LOT_CURR].strPartNo;
				st_track_info.nIn				= st_count_info.nInCount[0][0];
				st_track_info.nPrimePass		= st_count_info.nPrimeCount[0][0];
				st_track_info.nPrimeReject		= st_count_info.nPrimeRejectCount[0][0];
				st_track_info.nCumPass			= st_count_info.nPassCount[0][0];
				st_track_info.nCumReject		= st_count_info.nRejectCount[0][0];
				
				// jtkim 20150408
				if (st_basic_info.nModeXgem == YES)
				{
					tEnd	= COleDateTime::GetCurrentTime();
					tData	= tEnd - st_lot_info[LOT_CURR].tStart;

					clsXgem.OnMcLotEnd(st_lot_info[LOT_CURR].strLotNo, 
										st_lot_info[LOT_CURR].strPartNo,
										st_track_info.nIn,
										st_track_info.nCumPass, 
										st_track_info.nCumReject,
										tData);
				}

				clsFunc.OnMCStop();
				clsFunc.OnDatabaseLotEnd(st_track_info);

				if (st_handler_info.cWndMain != NULL)
				{
					st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_TRACK_OUT_CREATE_REQ, 0);
				}
				*/

				st_track_info.nTestEndBin		= NO;
				st_track_info.strLotNo			= st_lot_info[LOT_CURR].strLotNo;
				st_track_info.strPartNo			= st_lot_info[LOT_CURR].strPartNo;
				st_track_info.nIn				= st_count_info.nInCount[0][0];
				st_track_info.nPrimePass		= st_count_info.nPrimeCount[0][0];
				st_track_info.nPrimeReject		= st_count_info.nPrimeRejectCount[0][0];
				st_track_info.nCumPass			= st_count_info.nPassCount[0][0];
				st_track_info.nCumReject		= st_count_info.nRejectCount[0][0];
				// jtkim 20161008
				st_track_info.tStart			= st_lot_info[LOT_CURR].tLotStart;
				st_track_info.nBdTime			= st_lot_info[LOT_CURR].nBdTime;
				st_track_info.nBdTimeCount		= st_lot_info[LOT_CURR].nBdTimeCount;


				clsFunc.OnMCStop();

				if (st_handler_info.cWndMain != NULL)
				{
					st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_LOT_END_CREATE_REQ, 0);
				}


				////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//2016.1203
				clsFunc.OnDailyCycleData(st_lot_info[LOT_CURR].strLotNo, 
											st_lot_info[LOT_CURR].strPartNo,
											st_work_info.dDailyCycle, 
											st_lot_info[LOT_CURR].tLotStart, 
											CTime::GetCurrentTime());

				strTable[2] = st_lot_info[LOT_CURR].strPartNo;
				if (st_product_info.nCycleCount > 0)
				{
					dAve = st_product_info.dCycleTime / (double)st_product_info.nCycleCount;
				}
				else dAve = 0.0f;
				st_product_info.dCycleTime	= 0.0f;
				st_product_info.nCycleCount	= 0;
				strTable[3].Format(_T("%.1f"), dAve);

				if (st_product_info.nTestCount > 0)
				{
					dAve = st_product_info.dTestTime / (double)st_product_info.nTestCount;
				}
				else dAve = 0.0f;
				st_product_info.dTestTime	= 0.0f;
				st_product_info.nTestCount	= 0;
				strTable[4].Format(_T("%.1f"), dAve);

				Mysql.WriteCycle(strTable);
				////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			}
			else
			{
				if (st_handler_info.cWndMain != NULL)
				{
					st_other_info.nBuzYesNo = YES;
					st_other_info.strBoxMsg.Format(_T("[LOT END] Eject on Unloading Good #[%d], Reject #[%d] Stacker Tray. \r\n Please Remove it."),
										(st_lot_info[LOT_CURR].nStacker_Good_THD_Num - THD_ULD_1_STACKER) + 1, (st_lot_info[LOT_CURR].nStacker_Reject_THD_Num - THD_REJECT_OUT_1_STACKER) + 1);  
					//st_other_info.strBoxMsg = _T("[LOT END] eject on Unloading #1 Good, Reject Stacker Tray. \r\n Please Remove it.");
					st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
				}
				//CTL_Lib.Alarm_Error_Occurrence(0, dWARNING, _T("990000")); //inline off line 일때만 lot end 알람 발생시킴 
			} 

			clsFunc.Set_LotInfo_LotEndDataExchange(0);

			if(st_lot_info[LOT_CURR].nLotStatus < LOT_START && st_lot_info[LOT_NEXT].nLotStatus < LOT_START)
			{
				//st_handler_info.nRunStatus	= dSTOP;
				clsFunc.OnMCStop();
			}
		}
		 		
	}	
	//else
	//{
	//}
	//2016.1228
	// jtkim 20161107
	if(st_lot_info[LOT_NEXT].nLotStatus >= LOT_END_START)
	{
		if(st_lot_info[LOT_NEXT].nLot_THD_Status[THD_WORK_RBT] == LOT_END && 
		    st_lot_info[LOT_NEXT].nLot_THD_Status[st_lot_info[LOT_NEXT].nStacker_Good_THD_Num] == LOT_END && 
		    st_lot_info[LOT_NEXT].nLot_THD_Status[st_lot_info[LOT_NEXT].nStacker_Reject_THD_Num] == LOT_END) //최종 lot End 조건
		{
//			st_var.nTestDvcOutFlag = 0 ; // 

			//st_lot_info[LOT_CURR].nLotEnd[THD_UNDER_UNLOAD_TRAY_STACKER_SITE] = CTL_NO;
			//for(i=0; i<THREAD_MAX_SITE; i++) st_lot_info[LOT_CURR].nLotEnd[i] = CTL_NO; //2015.0302 
			//st_lot_info[LOT_CURR].nLotStatus = LOT_CLEAR;
			// lot end 추가
			if (st_basic_info.nModeInterface == EQP_ON_LINE)
			{
				st_track_info.nTestEndBin		= NO;
				st_track_info.strLotNo			= st_lot_info[LOT_NEXT].strLotNo;
				st_track_info.strPartNo			= st_lot_info[LOT_NEXT].strPartNo;
				st_track_info.nIn				= st_count_info.nInCount[1][0];
				st_track_info.nPrimePass		= st_count_info.nPrimeCount[1][0];
				st_track_info.nPrimeReject		= st_count_info.nPrimeRejectCount[1][0];
				st_track_info.nCumPass			= st_count_info.nPassCount[1][0];
				st_track_info.nCumReject		= st_count_info.nRejectCount[1][0];
				// jtkim 20161008
				st_track_info.tStart			= st_lot_info[LOT_NEXT].tLotStart;
				st_track_info.nBdTime			= st_lot_info[LOT_NEXT].nBdTime;
				st_track_info.nBdTimeCount		= st_lot_info[LOT_NEXT].nBdTimeCount;

				clsFunc.OnMCStop();

				if (st_handler_info.cWndMain != NULL)
				{
					st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_LOT_END_CREATE_REQ, 0);
				}
			}
			else
			{
				if (st_handler_info.cWndMain != NULL)
				{
					st_other_info.nBuzYesNo = YES;
					st_other_info.strBoxMsg.Format(_T("[LOT END] Eject on Unloading Good #[%d], Reject #[%d] Stacker Tray. \r\n Please Remove it."),
										(st_lot_info[LOT_CURR].nStacker_Good_THD_Num - THD_ULD_1_STACKER) + 1, (st_lot_info[LOT_CURR].nStacker_Reject_THD_Num - THD_REJECT_OUT_1_STACKER) + 1);  
					//st_other_info.strBoxMsg = _T("[LOT END] eject on Unloading #1 Good, Reject Stacker Tray. \r\n Please Remove it.");
					st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
				}
				//CTL_Lib.Alarm_Error_Occurrence(0, dWARNING, _T("990000")); //inline off line 일때만 lot end 알람 발생시킴 
			} 

			st_lot_info[LOT_NEXT].strLotNo		= _T("");
			st_lot_info[LOT_NEXT].strPartNo		= _T("");
			st_lot_info[LOT_NEXT].strProcess	= _T("");
			st_lot_info[LOT_NEXT].strProPerty	= _T("");
			st_lot_info[LOT_NEXT].strBypass		= _T("");

			st_lot_info[LOT_NEXT].nLotNoApply_FirstTestStart = LOT_CLEAR; //james 2016.0926 


			st_lot_info[LOT_NEXT].nLotStatus	= LOT_CLEAR;
			//james 2016.0924 for( i= 0; i < THREAD_MAX_SITE; i++)st_lot_info[LOT_NEXT].nLot_THD_Status[i] = LOT_CLEAR; 
			st_lot_info[LOT_NEXT].nNewLotIn		= 0;
			st_lot_info[LOT_NEXT].nLastModule	= 0;
			st_lot_info[LOT_NEXT].nTotalCnt		= 0;
			st_lot_info[LOT_NEXT].nLane			= 0; 
			st_lot_info[LOT_NEXT].nLdTrayCnt	= 0;
			st_lot_info[LOT_NEXT].nUnLdTrayCnt	= 0;
			for(i=0; i<3; i++) st_lot_info[LOT_NEXT].nBinSortCnt[i] = 0;
			st_lot_info[LOT_NEXT].nTrayRunMode_StandAlone			= 0; 

			//james 2016.0921 
			st_lot_info[LOT_NEXT].tLotStart					= 0;

			st_lot_info[LOT_NEXT].nAbortTime				= 0;
			st_lot_info[LOT_NEXT].nAbortTotal				= 0;
			st_lot_info[LOT_NEXT].nAbortCount				= 0;
			st_lot_info[LOT_NEXT].nAbortUsed				= 0;	 

			st_lot_info[LOT_NEXT].nStacker_Good_THD_Num		= 0;
			st_lot_info[LOT_NEXT].nStacker_Reject_THD_Num	= 0;
			st_lot_info[LOT_NEXT].nBuffer_Retest_THD_Num	= 0;

			st_lot_info[LOT_NEXT].nTestMode					= 0;// ONESHOT, HOT, COLD, GATE......
			st_lot_info[LOT_NEXT].nInterface				= 0;// ftp / local......
			st_lot_info[LOT_NEXT].nRetry					= 0;// retry count.....
			st_lot_info[LOT_NEXT].nBarcodeMode				= 0;// barcode 전송선택 (1개/2개)
			st_lot_info[LOT_NEXT].nQty						= 0;// lot 수량.....
			st_lot_info[LOT_NEXT].nModuleCount				= 0;// pgm download 완료......
			st_lot_info[LOT_NEXT].nPgmDown					= 0;
			st_lot_info[LOT_NEXT].nChamberDoor				= 0;// chanber door open......	
		}
	}



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////james 2016.0921 	 


	switch (m_nRunStep)
	{ 

		case -1: 

			m_nRunStep = 0;
			break;

		case 0:		
			if(st_lot_info[LOT_CURR].nLotStatus == LOT_START || st_lot_info[LOT_NEXT].nLotStatus == LOT_START )
			{
				m_nRunStep = 100;
			}
			break;

		case 100://피커 상태를 파악한다
			//james 2016.0827 그리퍼 디바이스 체크 정보 수정/보완, Gripper Off(클림프상태)에서 IO_OFF시 디바이스 있는상태, 그외는 ON/OFF 산관없이 모두 디바이스 없는 상태
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_LdUldPickDvcChk,	IO_OFF);
			nRet_2 = FAS_IO.get_out_bit(st_io_info.o_LdUldPickGripCyl,	IO_OFF); //clamp 상태
			if(nRet_1 == IO_ON && nRet_2 == IO_OFF) 
			{
				m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_LdUldPickDvcChk); 
				CTL_Lib.Alarm_Error_Occurrence(1100, dWARNING, m_strAlarmCode);					
			}
			else
			{
				st_picker[m_nRobotSite].st_pcb_info[0].nYesNo = CTL_NO; //james 2016.0909 
				m_nRunStep = 200;				 
			}
			break; 

		case 200:
			Set_Glipper_OnOff(IO_ON); //미리 벌린다 
			m_nRunStep = 210;
			break;

		case 210:
			nRet_1 = Chk_Glipper_OnOff(IO_ON); 			 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 300;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(1101, dWARNING, m_strAlarmCode);
				m_nRunStep = 110;
			}
			break;

		case 300:
			Set_WorkRbtPicker_UpDown(IO_OFF);
			m_nRunStep = 310;
			break;

		case 310:
			nRet_1 = Chk_WorkRbtPicker_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 400;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(1102, dWARNING, m_strAlarmCode);
				m_nRunStep = 300;
			}
			break;

		case 400:
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY], COMI.mn_runspeed_rate);
			if (nRet_1 == BD_GOOD) //좌측으로 이동
			{
				m_nRunStep = 500;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nRunStep = 400;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
				CTL_Lib.Alarm_Error_Occurrence(1103, dWARNING, st_alarm_info.strCode);
				m_nRunStep = 400;
			}
			break;

		case 500:
			Set_WorkRbtRotator_OnOff(IO_OFF);
			m_nRunStep = 510;
			break;

		case 510:
			nRet_1 = Chk_WorkRbtRotator_OnOff(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 600;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(1104, dWARNING, m_strAlarmCode);
				m_nRunStep = 500;
			}
			break;		

		case 600:
			m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_WORKROBOT_LDTRAY_START];
			m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_WORKROBOT_LDTRAY_START];
			nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate);
			if(nRet_1 == BD_GOOD) //정상적으로 완료된 상태
			{
				m_nRunStep = 700;
			}
			else if(nRet_1 == BD_ERROR)
			{
				m_nRunStep = 600;
				CTL_Lib.Alarm_Error_Occurrence(1105, dWARNING, st_alarm_info.strCode);
			}
			break;

		case 700:
			
			m_nRunStep = 900;
			 
			break;

		case 900:
			if(st_lot_info[LOT_CURR].nLotStatus == LOT_START || st_lot_info[LOT_NEXT].nLotStatus == LOT_START )
			{
				//load plate에 자재 요청
				st_sync_info.nWorkRbt_Dvc_Req[THD_LD_TRAY_PLATE][0] = CTL_REQ;
				st_sync_info.nWorkRbt_Dvc_Req[THD_LD_TRAY_PLATE][1] = WORK_PICK;

				m_nFindLotNo_Flag = 0; //2016.0912
				m_nRunStep = 1000;
			}
			break;

/*
///////////////////////////////////////////
//2016.0805 소켓오프 기능 확정
///////////////////////////////////////////
1. 테스터는 동작할소켓 정보를 확인 후 소켓 정보에 맞게 버퍼에서 집는다
2. 소켓오프로  버퍼에 자재가 남아있으면 워크로봇은 버퍼에 놓지 말고 대기한다
3. 테스트로봇은 테스터에 놓고, 테스트 끝난 자재가 있으면 집어 언로딩 버퍼에 놓는다
4. 1항으로 가는것이 정상이나, 그외 소켓오프등으로 로딩 버퍼에 자재가 남아있으면 다음의 방법으로 동작한다
  1) 작업이 확정된 테스터의 소켓 정보를 확인후 버퍼와 일치하면 1항으로 이동.
      일치하지 않으면 다음동작
  2) 테스터 소켓 정보에 맞게 워크 로봇이 자재를 버퍼에 놓을 수 있도록 버퍼의 공간을 비워두게 테스터로봇은 로딩버퍼에 남아있는 작업 가능한 자재를 모두 집는다.
  3) 워크로봇은 버퍼가 비워져 있으니 확정된 테스터에 필요한 버퍼 공간 위치이 
     자재를 놓는다 
  4) 위3)의 동작을 반복하여 테스트로봇과, 버퍼의 정보가 테스터 소켓과 일치하면 
      로딩 버퍼의 작업이 완성한 상태이다 
  5) 테스트 로봇은 버퍼의 자재를 집는다
  6) 테스트 로봇은 이미 집고있던 자재와 지급 버퍼에서 집은 자재를 가지고 확정된 
      테스트 사이트로 이동한다.	
 */

		case 1000:	
			//////////////////////////////////////////////////////////////////////////////////////////////////////////
			//연속 lot이 가능하게 작업할 lot 정보를 LOT_CURR과 LOT_NEXT LotNo를 반복적으로 변경하여 Lot 정보로 찾는다 
			//////////////////////////////////////////////////////////////////////////////////////////////////////////
			if(m_nFindLotNo_Flag == 0)
			{
				m_nFindLotNo_Flag = 1;						

				if(st_lot_info[LOT_CURR].nLotStatus >= LOT_START) //james 2016.0924 추가 
				{
					m_strFindLotNo = st_lot_info[LOT_CURR].strLotNo;
					m_nLotProcessNum = LOT_CURR; 		
					//james 2016.0930 m_nFindLotNo_Flag = 1;						
				} 
				else
				{
					break; //james 2016.0930 
				}
			}
			else //if(m_nFindLotNo_Flag == 1)
			{		
				m_nFindLotNo_Flag = 0;						
				
				// 연속 LOT 작업시 조건 체크 방법 
				if(st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)  //james 2016.0924 if(st_lot_info[LOT_NEXT].nLdTrayCnt > 0 && st_lot_info[LOT_NEXT].nLotStatus >= LOT_START) 
				{
					nCount = 0;
					for(i = THD_TESTSITE_1; i <= THD_TESTSITE_8; i++) //james 206.0924 
					{
						if(st_lot_info[LOT_NEXT].strLotNo ==  st_test_site_info[i].strLotNo)
						{
							nCount++;
						}
					}
					if(nCount == 0)
					{
						break;
					}

					m_strFindLotNo = st_lot_info[LOT_NEXT].strLotNo;
					m_nLotProcessNum = LOT_NEXT; 		
				}
				else
				{//연속 lot 이 진행중이지 않을떄는 현재 lot No 만 비교하게 break; 한다 
					break;
				}	
			}//연속 lot 정보 확인 

			if(st_lot_info[m_nLotProcessNum].strLotNo == "")//lot 정보가 없으면 리턴 
			{
				break;
			}
			else if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] ==  LOT_END) //이미 해당 lot이 lot end이면 
			{
				break;
			}
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////

			////////////////////////////////////////////////////////////
			//언로딩 버퍼에서 자재 소팅을 요구했으면  처리한다 
			////////////////////////////////////////////////////////////
			if(st_sync_info.nWorkRbt_Dvc_Req[THD_ULD_BUFF][0] == CTL_REQ) 
			{
				if(m_nLoadBuff_Job_Continue_Flag == CTL_YES) //로딩 버퍼에 자재를 놓고 있는 중이니 계속 작업을 진행한다
				{	//james 2016.0924 	0 && 			
					nTemp = 0;
					//james 2016.0918 m_nLoadBuff_Job_Continue_Flag = CTL_NO;//james test

					if(m_nLotProcessNum == LOT_CURR)
					{
						if(st_lot_info[LOT_CURR].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END_START)
						{		
							if(st_lot_info[LOT_NEXT].nLotStatus >= LOT_START) 
							{
								if(st_lot_info[LOT_NEXT].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END_START)
								{
									m_nLoadBuff_Job_Continue_Flag = CTL_NO;
								}
							}
							else
							{
								m_nLoadBuff_Job_Continue_Flag = CTL_NO;
							}
							break; //james 2016.0930

							/* james 2016.0930 
							if(st_lot_info[LOT_NEXT].nLotStatus >= LOT_START) //james 206.0930 조건 추가 
							{
								if(st_lot_info[LOT_NEXT].nLotNoApply_FirstTestStart == LOT_START) //james 2016.0926 test site가 하나이상 Next LotNo로 셋팅되었을때 동작하자 
								{
									//next test 작업 시작 
								}
								else
								{
									m_nLoadBuff_Job_Continue_Flag = CTL_NO;
								}
							}
							else
							{
								m_nLoadBuff_Job_Continue_Flag = CTL_NO;
							}	*/						
						}
					}	
					else //if(m_nLotProcessNum == LOT_NEXT)
					{
						if(st_lot_info[LOT_NEXT].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END_START)
						{
							m_nLoadBuff_Job_Continue_Flag = CTL_NO;
							break; //james 2016.0930
						}
					}
				}
				else //if(m_nLoadBuff_Job_Continue_Flag == CTL_NO) 
				{
					//james 2016.0930 조건 추가 
					nCount = 0; 
					for(i = 3; i >= 0; i--)
					{
						if(st_buffer_info[THD_ULD_BUFF].st_pcb_info[i].nYesNo == CTL_YES)
						{
							nCount++;
							if(m_nLotProcessNum == LOT_CURR)
							{
								if(st_buffer_info[THD_ULD_BUFF].st_pcb_info[i].strLotNo == st_lot_info[LOT_CURR].strLotNo)
								{
									m_nRunStep = 6000; //unload buffer의 자재를 집어 소팅한다 
									break; 
								}
							}
							else if(m_nLotProcessNum == LOT_NEXT)
							{
								if(st_buffer_info[THD_ULD_BUFF].st_pcb_info[i].strLotNo == st_lot_info[LOT_NEXT].strLotNo)
								{
									m_nRunStep = 6000; //unload buffer의 자재를 집어 소팅한다 
									break; 
								}
							}
						}
					} 
					if(nCount  > 0)
					{						
						break; 
					}
					else
					{
						break;
					}
				}				
			}//언로딩 자재 조건 


			//////////////////////////////////////////////
			// 로딩 자재 조건 
			//////////////////////////////////////////////

			if(m_nLotProcessNum == LOT_CURR) 
			{
				if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END_START) //test robot이 LOT_END_START 사전에 진행되어야만 next lot을 진행 할 수 있다 
				{
					//kwlee 2016.1112 
					if (st_lot_info[LOT_CURR].nLot_THD_Status[THD_ULD_BUFF] == LOT_END)
					{
						m_nRunStep = 6500;
					}
					/////
					break;//LOT_CURR 의 로딩 자재가 로딩 버퍼에서 완전히 비워질떄까지는 next lot은 load plate 까지만 준비할 수 있고, 로봇이 집으면 안된다. 
				}

				if(st_lot_info[m_nLotProcessNum].nLotStatus >= LOT_END_START && st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] <  LOT_END_START) //메인 화면 강제 lot end 신호 //james 2016.0909 
				{
					st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] =  LOT_END_START;

					//james 2016.0929 
					nCount = 0;
					for(i = 0; i < st_recipe_info.nLdUldBuffer_Num; i++) //work robot는 로딩 버퍼에 자재를 놓을떄는 0->1->2->3 순서로 작업(정면 뒤에서 부터 1번이다)
					{
						if(st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nYesNo == CTL_YES)//	&& (st_buffer_info[nSite].st_pcb_info[i].strLotNo == str_LotNo || str_LotNo == "0") ) //nDVCYesNO == CTL_NO 조건에 충족한 정보만 체크, 사용가능한 소켓에만 작업가능
						{
							nCount++;			
						} 
					}
					if(nCount > 0)
					{
						st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_READY; //버퍼에 놓을수 있는 모든 자재를 놓았으면 그떄, 교체 요청을 진행 

						m_nLoadBuff_Job_Continue_Flag = CTL_NO; //clear
					} 
					break;
				}		 
			}
			else //if(m_nLotProcessNum == LOT_NEXT) 
			{
				if(st_lot_info[LOT_CURR].nLot_THD_Status[THD_TEST_RBT] <  LOT_END_START) //test robot이 LOT_END_START 사전에 진행되어야만 next lot을 진행 할 수 있다 
				{
					break;//LOT_CURR 의 로딩 자재가 로딩 버퍼에서 완전히 비워질떄까지는 next lot은 load plate 까지만 준비할 수 있고, 로봇이 집으면 안된다. 
				}

				if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END_START) //test robot이 LOT_END_START 사전에 진행되어야만 next lot을 진행 할 수 있다 
				{
					break;//LOT_CURR 의 로딩 자재가 로딩 버퍼에서 완전히 비워질떄까지는 next lot은 load plate 까지만 준비할 수 있고, 로봇이 집으면 안된다. 
				}

				if(st_lot_info[m_nLotProcessNum].nLotStatus >= LOT_END_START && st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] <  LOT_END_START) //메인 화면 강제 lot end 신호 //james 2016.0909 
				{
					st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] =  LOT_END_START;

					//james 2016.0929 
					nCount = 0;
					for(i = 0; i < st_recipe_info.nLdUldBuffer_Num; i++) //work robot는 로딩 버퍼에 자재를 놓을떄는 0->1->2->3 순서로 작업(정면 뒤에서 부터 1번이다)
					{
						if(st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nYesNo == CTL_YES)//	&& (st_buffer_info[nSite].st_pcb_info[i].strLotNo == str_LotNo || str_LotNo == "0") ) //nDVCYesNO == CTL_NO 조건에 충족한 정보만 체크, 사용가능한 소켓에만 작업가능
						{
							nCount++;			
						} 
					}
					if(nCount > 0)
					{
						st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_READY; //버퍼에 놓을수 있는 모든 자재를 놓았으면 그떄, 교체 요청을 진행 

						m_nLoadBuff_Job_Continue_Flag = CTL_NO; //clear
					} 
					break;
				}		 
			}

			 /*2016.0930 
			 //로딩 플레이트의 자재를 집는다 
			//james 2016.0909 로딩 자재를 집기전 lot 진행 상황과 lot 정보를 항상 체크 한다 			
			if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END_START)  
			{
				// work robot가 로딩 부분은 lot end start 를 시작한 상태로,버퍼, 테스트 사이트, 등이 모두 처리되면 lot end를 종료할 수 있다
				// 그러나 연속lot이 있어 이 워크 로봇은 조건이 되면 계속  동작해야 한다 
			
					//우선 로딩 버퍼의 자재가 있으면 그 자재까지 만 처리하고 , 로딩은 마무리한다.
					//이 로딩 자재가 로딩 버퍼에서 비워질떄까지는 next lot은 load plate 까지만 준비할 수 있다. 

				//더불어 테스트 사이트는 #1 ~#4 까지는 Next Lot 공급되면 처리할 예정으로 점진적으로 이 사이트는 비워주며, LOT_CURR 은 #5 ~#8 까지만 사용할 수 있게 TEST ROBOT에서 Test Site의 각 lot 정보를위치에 맞게 갱싱해 준다  
				
				if(st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)//james 2016.0424 
				{

				}
				else
				{
					nRet_1 = clsFunc.Find_TrayWork_Pos(WORK_PLACE, THD_LD_BUFF, CTL_YES, m_npFindWorkPosYXCPB, THD_WORK_RBT, st_worker_info.strLotNo_Empty);		 			
					if(m_npFindWorkPosYXCPB[2] > 0 && m_nLoadBuff_Job_Continue_Flag == CTL_YES) //로딩 버퍼에 자재가 하나라도 있으면 테스트 핤 있게 보낸다 
					{	
						st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_READY; //버퍼에 놓을수 있는 모든 자재를 놓았으면 그떄, 교체 요청을 진행 

					}
					m_nLoadBuff_Job_Continue_Flag = CTL_NO; //clear
					break;
				}
			}
			if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_TRAY_PLATE] >=  LOT_END_START
				&& st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] ==  LOT_START) //james 2016.0924 
			{
				//james 2016.0926 
				nCount = 0;
				for(i = 0; i < st_recipe_info.nLdUldBuffer_Num; i++) //work robot는 로딩 버퍼에 0,1,2,3 순서로 작업
				{ //버퍼의 자재 정보를 셋팅한다					 
					if(st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nYesNo == CTL_YES &&
						st_buffer_info[THD_LD_BUFF].st_pcb_info[i].strLotNo == st_lot_info[LOT_CURR].strLotNo)//james 2016.0924 조건 추가 
					{
						nCount++;
					}
				}
				if(nCount > 0)
				{
					if(st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_REQ )
					{
						st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_READY; //버퍼에 놓을수 있는 모든 자재를 놓았으면 그떄, 교체 요청을 진행 
						m_nLoadBuff_Job_Continue_Flag = CTL_NO; //clear
					}
				}//2016.0926 

				st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] =  LOT_END_START; //work robot는 lot end가 진행되도 , 언로딩 할 자재 및 next lot등 처리를 위해 계속 돌아야 한다 
				break;
			}*/

			  
			if(st_sync_info.nWorkRbt_Dvc_Req[THD_LD_TRAY_PLATE][0] == CTL_READY)//로딩 플레이트 집기 후 플레그 확인  && st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_REQ) //로딩 트레이가 준비되어 있고, 로딩 버퍼에서도 자재 공급을 요청 받았으면 동작하자 
			{
				if(m_strFindLotNo != st_tray_info[THD_LD_STACKER].strLotNo) // lot 정보가 틀리면 안된다 
				{						
					//메세지 출력 
					break;
				}

				//james 2016.0924 조건 추가 
				if(m_nLotProcessNum == LOT_NEXT) //Next lot 진행하는 조건이며 LOT_CURR의 자재는 로딩버퍼에 존재하면 안된다  
				{
					if(st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][0] != CTL_REQ)
					{
						break;
					}
				}
				else //if(m_nLotProcessNum == LOT_CURR) 
				{
					if(st_lot_info[LOT_NEXT].nLotStatus >= LOT_START && st_lot_info[LOT_CURR].nLotStatus >= LOT_END_START)//james 2016.0924 
					{
						break;
					}
				}

				/* james 2016.0930 
				nRet_1 = clsFunc.Find_TrayWork_Pos(WORK_PLACE, THD_LD_BUFF, CTL_NO, m_npFindWorkPosYXCPB, THD_WORK_RBT, st_worker_info.strLotNo_Empty);		 
				if(nRet_1 == RET_SKIP) //더이상 작업할 공간이 없다 
				{	
					st_sync_info.nWorkRbt_Dvc_Req[THD_LD_TRAY_PLATE][0] = CTL_REQ; //로딩 자재를 요청, 더이상 현재 트레이에는 없으니 로딩 트레이를 교체할 것이다 
					m_nLoadBuff_Job_Continue_Flag = CTL_NO; //clear
					break;
				}*/

				//2016.1117
// 				if(st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] != CTL_REQ) //james 2016.1005 미리 집고 있지는 말자, 리테스트 다발시 문제됨
// 				{
// 					if(m_nLoadBuff_Job_Continue_Flag == CTL_YES)
// 					{
// 						m_nLoadBuff_Job_Continue_Flag = CTL_NO;
// 					}
// 					break;
// 				}

				//2016.1114
				if( st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][0] != CTL_REQ ) break;

				m_nLoadBuff_Job_Continue_Flag = CTL_YES;
				m_nRunStep = 2000; //로딩 플레이트에서 자제를 집어 로딩 버퍼에 자재를 놓는다
			}
			break;

////////////////////////////////////////////////
// load tray의 자재를 집는다 
// 자재를 로딩 버퍼의 정보에 맞게 모두 채웠으면 1000 step로 다시가 초음 부터 시작한다 
////////////////////////////////////////////////
		case 2000:
			nRet_1 = Process_DVC_Pick(0, THD_LD_TRAY_PLATE);
			if(nRet_1 == RET_GOOD)
			{				
				m_nRunStep = 2100;
			}
			else if(nRet_1 == RET_PICKER_NOT_FIND) //피커는 더이상 작업할 내용이 없다. 이때는 로보트는 버퍼에 놓으러 이동한다
			{
				m_nRunStep = 2100;			
			}
			else if(nRet_1 == RET_TRAY_NOT_FIND) //트레이에는 더 이상 작업할 수 있는 내용이 없다
			{
				//st_sync_info.nLdRobot_Req[m_nLdRobot_Work_Pos_Num] = CTL_CHANGE; //트레이 교체 요청
				//m_nLdRobot_Work_Pos_Num = 0; //피커는 집을 수 있는 곳이있고, 트레이는 작업할 내용이 없으니 clear

				m_nRunStep = 2100; //2100; //1000;

			}
			break; 

		case 2100: //미리 자재를 요청하여 트레이에 자재가 없으면 plate에서 자동으로 트레이를 교체 작업을 할 것이다 
			//james 2016.0809 lot end 조건을 확인한다 
			if(st_lot_info[m_nLotProcessNum].strLotNo == st_tray_info[THD_LD_STACKER].strLotNo) //james 2016.0907 
			{
				/* james 2016.0930 
				nCount = 0;
				if(m_nLotProcessNum == LOT_CURR)
				{
					nCount = st_count_info.nInCount[0][0];
				}
				else if(m_nLotProcessNum == LOT_NEXT)
				{
					nCount = st_count_info.nInCount[0][1];
				}

			  if(nCount >= st_lot_info[m_nLotProcessNum].nQty) //james 2016.0929 if(st_count_info.nInCount[0][0] >= st_lot_info[m_nLotProcessNum].nQty)
			  {//해당 lot에서 작업할 수량은 모두 처리한 상태
				  //자재를 버퍼에 완전히 내려놓고 조건 확인 st_lot_info[LOT_CURR].nLot_THD_Status[THD_WORK_RBT] =  LOT_END_START;
			  }
			  else if(st_lot_info[m_nLotProcessNum].nLotStatus >= LOT_END_START) //메인 화면 강제 lot end 신호 //james 2016.0909 
			  {
				  //자재를 버퍼에 완전히 내려놓고 조건 확인  st_lot_info[LOT_CURR].nLot_THD_Status[THD_WORK_RBT] =  LOT_END_START;
			  }		
			  else 
			  {
				  //james 2016.0930 st_sync_info.nWorkRbt_Dvc_Req[THD_LD_TRAY_PLATE][0] = CTL_REQ; //로딩 자재를 요청, 더이상 현재 트레이에는 없으니 로딩 트레이를 교체할 것이다 
			  }*/

			  st_sync_info.nWorkRbt_Dvc_Req[THD_LD_TRAY_PLATE][0] = CTL_REQ; //james 2016.0930 //로딩 자재를 요청, 더이상 현재 트레이에는 없으니 로딩 트레이를 교체할 것이다 
			  m_nRunStep = 2300; 
			}
			else
			{//나오면 안됨 
				break; 
			} 
			break;

		////////////////////////////////////////////////////////////
		// 로딩 버퍼에 자재를 놓는다 
		////////////////////////////////////////////////////////////
		case 2300:			 
			if(st_picker[m_nRobotSite].st_pcb_info[0].nYesNo == CTL_NO)//피커에 자재가 들고 있다는 정보가 없다
			{//james 2016.1005 자재가 없으면 이후로 갈 필요없으니 처음으로 돌아가자 
				m_nRunStep = 1000; 
				break;
			} 
			m_nRunStep = 2400; 
			break;

		case 2400:
			 //로딩 버퍼가 로봇에게 자재를 요청했으면 공급한다. 모두 공급할 떄까지 반복 동작을 해야 하니 변수의 플레그를 유지하자  
			if(st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_REQ)
			{
				m_nRunStep = 2500;  
			}
			break;

		case 2500:
			nRet_1 = Process_DVC_Place(100, THD_LD_BUFF, st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][2]); //4번3,2,1번 순서로 사용할 수 있는 위치의 버퍼에 놓아야 간섭이 없다 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 2510; //james 2016.1003 2600;
			}
			else if(nRet_1 == RET_PICKER_NOT_FIND) //피커는 더이상 작업할 내용이 없다. 이때는 로보트는 버퍼에 놓으러 이동한다
			{
				m_nRunStep = 2600;			
			}
			else if(nRet_1 == RET_TRAY_NOT_FIND) //트레이에는 더 이상 작업할 수 있는 내용이 없다
			{
				//st_sync_info.nLdRobot_Req[m_nLdRobot_Work_Pos_Num] = CTL_CHANGE; //트레이 교체 요청
				//m_nLdRobot_Work_Pos_Num = 0; //피커는 집을 수 있는 곳이있고, 트레이는 작업할 내용이 없으니 clear

				m_nRunStep = 2600;  

			}
			break;

		case 2510: //james 2016.1003 
			 nCount = 0;
			 for(i = 0; i < LD_ULD_PICKER_PARA; i++) //test site에서 작업할 정보를 확보한다
			 {					 
				 if(st_picker[THD_WORK_RBT].st_pcb_info[i].nYesNo == CTL_YES)// && st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable == CTL_YES)
				 {
					 //2016.1203
					 m_strLotNo[0] = st_picker[THD_WORK_RBT].st_pcb_info[i].strLotNo;
					 m_strPartNo[0] = st_picker[THD_WORK_RBT].st_pcb_info[i].strPartNo;
					 nCount++;
				 } 
			 }
			 if(nCount > 0) //자재가 남아있으면 
			 {
				 m_nRunStep = 2500;   
			 }
			 else// if(nCount == 0)
			 {
				 m_nRunStep = 2600;   
			 }
			 break;


		case 2600:
			nRet_1 = clsFunc.Find_TrayWork_Pos(WORK_PLACE, THD_LD_BUFF, CTL_NO, m_npFindWorkPosYXCPB, THD_WORK_RBT, st_worker_info.strLotNo_Empty);		 			
			if(nRet_1 == RET_SKIP) //더이상 작업할 공간이 없다 
			{	
				st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_READY; //버퍼에 놓을수 있는 모든 자재를 놓았으면 그떄, 교체 요청을 진행 

				m_nLoadBuff_Job_Continue_Flag = CTL_NO; //clear
				m_nRunStep = 2700; //1000;  

				//2017.0220
				for ( i = 0; i < 4; i++)
				{
					if(st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nYesNo == CTL_YES )
					{
						m_strPartNo[0] = st_buffer_info[THD_LD_BUFF].st_pcb_info[i].strLotNo;
						m_strPartNo[0] = st_buffer_info[THD_LD_BUFF].st_pcb_info[i].strPartNo;
						break;
					}
				}

				////////////////////////////////////////////////////////////////////////////////
				//2016.1203
				if(m_npFindWorkPosYXCPB[2] == 0)
				{//버퍼에는 더이상 놓은 공간이 없다
					m_dwCycleTime[0][1] = GetTickCount();
					m_dwCycleTime[0][2] = m_dwCycleTime[0][1] - m_dwCycleTime[0][0];

					if (m_dwCycleTime[0][2] <= (DWORD)0)
					{
						m_dwCycleTime[0][0] = GetTickCount();
					}
					else
					{
						if (m_dwCycleTime[0][2] > (DWORD)0)
						{
							if( m_strLotNo[0] != _T("") && m_strPartNo[0] != _T("") )
							{
								clsFunc.OnCycleTime(0, 
													m_strLotNo[0],
													m_strPartNo[0],
													m_dwCycleTime[0][2], 
													0, 
													0);
							}

							m_strLotNo[0] = m_strPartNo[0] = _T("");
						}
						m_dwCycleTime[0][0] = GetTickCount();
					}
				}
				////////////////////////////////////////////////////////////////////////////////

			}
			else
			{
				m_nRunStep = 2700; // 1000;  
			}
			break;

		case 2610://체크만 한다.
			if(st_sync_info.nWorkRbt_Dvc_Req[THD_LD_TRAY_PLATE][0] != CTL_READY)
			{
				//2016.1228
				if( st_handler_info.mn_NoInput_test == CTL_YES)//투입할 자재는 있지만 작업자가 아직 투입을 않하고 있다.일단, 투입한 걸로 동작하자.
				{
					st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_READY; //더이상 버퍼에 놓을 자재가 없으니 갖고가라 
					m_nLoadBuff_Job_Continue_Flag = CTL_NO; //clear
					m_nRunStep = 2700; 
				}
				break;
			}

			nRet_1 = Process_DVC_Pick(0, THD_LD_TRAY_PLATE);
			if(nRet_1 == RET_GOOD)
			{				
				m_nRunStep = 2700;
			}
			else if(nRet_1 == RET_PICKER_NOT_FIND)
			{
				m_nRunStep = 2700;			
			}
			else if(nRet_1 == RET_TRAY_NOT_FIND) //트레이에는 더 이상 작업할 수 있는 내용이 없다
			{
				m_nRunStep = 2700;
			}
			break;
		
		///////////////////////////////////////////////////////////////////////
		//lot end 조건을 체크한다 
		///////////////////////////////////////////////////////////////////////
		case 2700: //james 2016.0909 	
			if(m_nLotProcessNum == LOT_CURR)
			{
				nCount = st_count_info.nInCount[0][0];
			}
			else if(m_nLotProcessNum == LOT_NEXT)
			{
				nCount = st_count_info.nInCount[0][1];
			}
			if(nCount >= st_lot_info[m_nLotProcessNum].nQty)//james 2016.0929 if(st_count_info.nInCount[0][0] >= st_lot_info[m_nLotProcessNum].nQty)
			{//해당 lot에서 작업할 수량은 모두 처리한 상태	

				//2016.1031 랏앤드를 하기전에 loadtrayplate에 있는 트레이를 제거하기위해 자재가 있는지 픽업해보고잇으면 에러 띄우고 없으면 제거하자
 				if(st_tray_info[THD_LD_TRAY_PLATE].nTrayExist == CTL_YES)
 				{
 					nRet_1 = clsFunc.Find_TrayWork_Pos(0, THD_LD_TRAY_PLATE, CTL_YES, m_npFindWorkPosYXCPB, THD_WORK_RBT, st_worker_info.strLotNo_Empty);
 					if(nRet_1 != RET_SKIP)
 					{
						st_handler_info.bLoaderOnFlag = CTL_YES;
 						st_sync_info.nWorkRbt_Dvc_Req[THD_LD_TRAY_PLATE][0] = CTL_REQ;
 						st_sync_info.nWorkRbt_Dvc_Req[THD_LD_TRAY_PLATE][1] = WORK_PICK;
 						m_nRunStep = 2610;
 						break;
 					}
 				}

				if(st_handler_info.bLoaderOnFlag == CTL_YES)
				{
					st_handler_info.bLoaderOnFlag = CTL_NO;
					st_sync_info.nWorkRbt_Dvc_Req[THD_LD_TRAY_PLATE][0] = CTL_REQ;
					st_sync_info.nWorkRbt_Dvc_Req[THD_LD_TRAY_PLATE][1] = WORK_PICK;
				}				

				st_lot_info[m_nLotProcessNum].nLotStatus = LOT_END_START;
				st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] =  LOT_END_START;

				//james 2016.0929 
				nCount = 0;
				for(i = 0; i < st_recipe_info.nLdUldBuffer_Num; i++) //work robot는 로딩 버퍼에 자재를 놓을떄는 0->1->2->3 순서로 작업(정면 뒤에서 부터 1번이다)
				{
					if(st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nYesNo == CTL_YES)//	&& (st_buffer_info[nSite].st_pcb_info[i].strLotNo == str_LotNo || str_LotNo == "0") ) //nDVCYesNO == CTL_NO 조건에 충족한 정보만 체크, 사용가능한 소켓에만 작업가능
					{
						nCount++;			
					} 
				}
				if(nCount > 0)
				{
					st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_READY; //버퍼에 놓을수 있는 모든 자재를 놓았으면 그떄, 교체 요청을 진행 

					m_nLoadBuff_Job_Continue_Flag = CTL_NO; //clear
				} 
			}
			else if(st_lot_info[m_nLotProcessNum].nLotStatus >= LOT_END_START) //메인 화면 강제 lot end 신호 //james 2016.0909 
			{
				st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] =  LOT_END_START;

				//james 2016.0929 
				nCount = 0;
				for(i = 0; i < st_recipe_info.nLdUldBuffer_Num; i++) //work robot는 로딩 버퍼에 자재를 놓을떄는 0->1->2->3 순서로 작업(정면 뒤에서 부터 1번이다)
				{
					if(st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nYesNo == CTL_YES)//	&& (st_buffer_info[nSite].st_pcb_info[i].strLotNo == str_LotNo || str_LotNo == "0") ) //nDVCYesNO == CTL_NO 조건에 충족한 정보만 체크, 사용가능한 소켓에만 작업가능
					{
						nCount++;			
					} 
				}
				if(nCount > 0)
				{
					st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_READY; //버퍼에 놓을수 있는 모든 자재를 놓았으면 그떄, 교체 요청을 진행 

					m_nLoadBuff_Job_Continue_Flag = CTL_NO; //clear
				} 
			}		 
			m_nRunStep = 1000; 
			break;

//////////////////////////////////////////////////////////////////////////////
// 언로딩 버퍼의 자재를 집어 소팅한다 
//////////////////////////////////////////////////////////////////////////////
		case 6000:
			nRet_1 = Process_DVC_Pick(0, THD_ULD_BUFF);
			if(nRet_1 == RET_GOOD)
			{					
				m_nRunStep = 6100;
			}
			else if(nRet_1 == RET_PICKER_NOT_FIND) //피커는 더이상 작업할 내용이 없다. 이때는 로보트는 버퍼에 놓으러 이동한다
			{
				if(st_picker[THD_WORK_RBT].st_pcb_info[0].nYesNo == CTL_NO) //자재를 집고 있지 않으면 처음으로 간다 
				{
					m_nRunStep = 1000;			
				}
			}
			else if(nRet_1 == RET_TRAY_NOT_FIND) //트레이에는 더 이상 작업할 수 있는 내용이 없다
			{				
				if(st_picker[THD_WORK_RBT].st_pcb_info[0].nYesNo == CTL_NO) //자재를 집고 있지 않으면 처음으로 간다 
				{
					m_nRunStep = 1000;			
				}
			}
			break;

		//////////////////////////////////////////////////
		// 언로딩 버퍼에 자재가 남아 있는지 확인한다 
		// 해당빈에 따라 소팅하다 (GOOD, REJECT BIN)
		//////////////////////////////////////////////////
		case 6100: //자재 정보를 확인하여 소팅 한다.
			nRet_1 = clsFunc.Find_TrayWork_Pos(WORK_PICK, THD_ULD_BUFF, CTL_YES, m_npFindWorkPosYXCPB, THD_WORK_RBT, st_worker_info.strLotNo_Empty);	
			if(nRet_1 == RET_GOOD) //작업 가능한 자재가 있다 
			{						
				m_nRunStep = 6200;  
			}
			else if(nRet_1 == RET_SKIP) //더이상 작업할 자재가 없다 
			{
				st_sync_info.nWorkRbt_Dvc_Req[THD_ULD_BUFF][0] = CTL_READY; //언로딩 버퍼의 모든 자재를 집어냈으면 교체 요청
				m_nRunStep = 6200;  

				////////////////////////////////////////////////////////////////////////
				//2016.1203
				m_dwCycleTime[1][1] = GetTickCount();
				m_dwCycleTime[1][2] = m_dwCycleTime[1][1] - m_dwCycleTime[1][0];

				if (m_dwCycleTime[1][2] <= (DWORD)0)
				{
					m_dwCycleTime[1][0] = GetTickCount();
				}
				else
				{
					if (m_dwCycleTime[1][2] > (DWORD)0)
					{
						if( m_strLotNo[1] != _T("") && m_strPartNo[1] != _T("") )
						{
							clsFunc.OnCycleTime(2, 
												m_strLotNo[1],
												m_strPartNo[1],
												m_dwCycleTime[1][2], 
												0, 
												0);
							m_strLotNo[1] = m_strPartNo[1] = _T("");
						}
					}

					m_dwCycleTime[1][0] = GetTickCount();
				}
				////////////////////////////////////////////////////////////////////////
			}
			break;

		/////////////////////////////////////////////////////////////////////////////////
		//집어 자제의 빈 정보에 따라 소팅한다 
		//소팅순서 : 1. good 자재, 2:리젝자재 
			//자재 Lot No 확인 필요 //james 2016.0924 확인해라 
		/////////////////////////////////////////////////////////////////////////////////
		case 6200: // 1 picker 임
			nFlag = 0; nTemp = 0;
			m_nWorkRobot_uldSite_SortJob_THD_Pos = -1;
			for(nCount = 0; nCount < 2; nCount++) //bin 종류는 reject, good 으로 2개의 소팅빈이 존재한다 
			{
				//언로딩 소팅순서는 1.rejext, 2:retest, 3:good 자재 순으로 소팅을 진행한다 
				if(nCount == 0)			nBin = BD_DATA_GOOD;  //첫번째 소팅할 빈  
				else if(nCount == 1)	nBin = BD_DATA_REJECT;//두번째 소팅할 빈
				//사용안함 else if(nCount == 2 )	nBin = BD_DATA_RETEST; //3번째 소팅할 빈, Retest Bin은 TEST ROBT에서 처리한다  

				if(st_picker[THD_WORK_RBT].st_pcb_info[0].nYesNo == CTL_YES)
				{			 
					if(st_picker[THD_WORK_RBT].st_pcb_info[0].nBin == nBin)
					{
						if(nBin == BD_DATA_GOOD)	
						{				
							//kwlee 2016.1118
// 							if(st_lot_info[m_nLotProcessNum].strLotNo == st_picker[THD_WORK_RBT].st_pcb_info[0].strLotNo) 
// 							{//james 2016.0921 
// 								if(st_lot_info[m_nLotProcessNum].nStacker_Good_THD_Num >= THD_ULD_1_STACKER && st_lot_info[m_nLotProcessNum].nStacker_Good_THD_Num <= THD_ULD_2_STACKER)
// 								{
// 									nFlag++;
// 									m_nWorkRobot_uldSite_SortJob_THD_Pos = st_lot_info[m_nLotProcessNum].nStacker_Good_THD_Num;
// 									m_nWorkRobot_Current_SortBin = nBin;
// 								}
// 								else
// 								{
// 									//굿 스태커 셋팅에 문제가 있다 , 에러 
// 								}
// 							} 	
							
							if(st_lot_info[LOT_CURR].strLotNo == st_picker[THD_WORK_RBT].st_pcb_info[0].strLotNo) 
							{//james 2016.0921 
								if(st_lot_info[LOT_CURR].nStacker_Good_THD_Num >= THD_ULD_1_STACKER && st_lot_info[LOT_CURR].nStacker_Good_THD_Num <= THD_ULD_2_STACKER)
								{
									nFlag++;
									m_nWorkRobot_uldSite_SortJob_THD_Pos = st_lot_info[LOT_CURR].nStacker_Good_THD_Num;
									m_nWorkRobot_Current_SortBin = nBin;
								}
								else
								{
									//굿 스태커 셋팅에 문제가 있다 , 에러 
								}
							}
							else if(st_lot_info[LOT_NEXT].strLotNo == st_picker[THD_WORK_RBT].st_pcb_info[0].strLotNo) 
							{
								if(st_lot_info[LOT_NEXT].nStacker_Good_THD_Num >= THD_ULD_1_STACKER && st_lot_info[LOT_NEXT].nStacker_Good_THD_Num <= THD_ULD_2_STACKER)
								{
									nFlag++;
									m_nWorkRobot_uldSite_SortJob_THD_Pos = st_lot_info[LOT_NEXT].nStacker_Good_THD_Num;
									m_nWorkRobot_Current_SortBin = nBin;
								}
								else
								{
									//굿 스태커 셋팅에 문제가 있다 , 에러 
								}
							}
						}
						else if(nBin == BD_DATA_REJECT)	
						{//james 2016.0921 
							//kwlee 2016.1118
// 							if(st_lot_info[m_nLotProcessNum].strLotNo == st_picker[THD_WORK_RBT].st_pcb_info[0].strLotNo) 
// 							{
// 								if(st_lot_info[m_nLotProcessNum].nStacker_Reject_THD_Num >= THD_REJECT_OUT_1_STACKER && st_lot_info[m_nLotProcessNum].nStacker_Reject_THD_Num <= THD_REJECT_OUT_2_STACKER)
// 								{
// 									nFlag++;
// 									m_nWorkRobot_uldSite_SortJob_THD_Pos = st_lot_info[m_nLotProcessNum].nStacker_Reject_THD_Num;
// 									m_nWorkRobot_Current_SortBin = nBin;
// 								}
// 								else
// 								{
// 									//굿 스태커 셋팅에 문제가 있다 , 에러 
// 								}
// 							}
							if(st_lot_info[LOT_CURR].strLotNo == st_picker[THD_WORK_RBT].st_pcb_info[0].strLotNo) 
							{
								if(st_lot_info[LOT_CURR].nStacker_Reject_THD_Num >= THD_REJECT_OUT_1_STACKER && st_lot_info[LOT_CURR].nStacker_Reject_THD_Num <= THD_REJECT_OUT_2_STACKER)
								{
									nFlag++;
									m_nWorkRobot_uldSite_SortJob_THD_Pos = st_lot_info[LOT_CURR].nStacker_Reject_THD_Num;
									m_nWorkRobot_Current_SortBin = nBin;
								}
								else
								{
									//굿 스태커 셋팅에 문제가 있다 , 에러 
								}
							}
							else if(st_lot_info[LOT_NEXT].strLotNo == st_picker[THD_WORK_RBT].st_pcb_info[0].strLotNo) 
							{
								if(st_lot_info[LOT_NEXT].nStacker_Reject_THD_Num >= THD_REJECT_OUT_1_STACKER && st_lot_info[LOT_NEXT].nStacker_Reject_THD_Num <= THD_REJECT_OUT_2_STACKER)
								{
									nFlag++;
									m_nWorkRobot_uldSite_SortJob_THD_Pos = st_lot_info[LOT_NEXT].nStacker_Reject_THD_Num;
									m_nWorkRobot_Current_SortBin = nBin;
								}
								else
								{
									//굿 스태커 셋팅에 문제가 있다 , 에러 
								}
							}
						}
					}
				}

				if(nFlag > 0) //작업할 트레이 정보가 있으면 작업한다 
				{
					nCount = 3;  
					break;
				}
			}//for

			if(m_nWorkRobot_uldSite_SortJob_THD_Pos == -1)
			{
				//선정되 빈이 없다. 문제 임 
				//2016.1024
// 				if( m_nLotProcessNum == LOT_CURR )
// 				{
// 					if(st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)
// 					{
// 						m_nLotProcessNum = LOT_NEXT;
// 					}
// 				}
// 				else if( m_nLotProcessNum == LOT_NEXT )
// 				{
// 					if(st_lot_info[LOT_CURR].nLotStatus >= LOT_START)
// 					{
// 						m_nLotProcessNum = LOT_CURR;
// 					}
// 				}
			}
			else
			{
				m_nRunStep = 6300;
			}
			break;

		case 6300:
			st_sync_info.nWorkRbt_Dvc_Req[m_nWorkRobot_uldSite_SortJob_THD_Pos][0] = CTL_REQ; //소팅 빈에 따른 해당 부위 준비 요청 
			m_nXYRobot_PreMove_Flag = 0;
			m_nRunStep = 6310;
			break;

		case 6310:
			if(st_sync_info.nWorkRbt_Dvc_Req[m_nWorkRobot_uldSite_SortJob_THD_Pos][0] == CTL_READY) 
			{//ready는 이곳에서 기다리면 대기 시간이 너무 많다. 미리 X,Y만 이동은 하자, 그러나 버퍼외, 트레이는 정보가 필요하니 참고필요 
				m_nRunStep = 6400;
			}
			else
			{
				if(m_nXYRobot_PreMove_Flag == 100) break; //이미 작업할 위치로 미리 이동한 상태 
				//언로딩 스태커가 준비가 안되어 있으면,  미리 작업할 위치로 로보트는 이동해야 한다
				m_nRunStep = 6320;
			}
			break;

		case 6320: //미리 해당 사이트로 이동한다 
			nRet_1 = clsFunc.Find_TrayWork_Pos(WORK_PLACE, m_nWorkRobot_uldSite_SortJob_THD_Pos, CTL_NO, m_npFindWorkPosYXCPB, THD_WORK_RBT, st_worker_info.strLotNo_Empty); //놓을때버퍼에서으 ㅣ작업순서는 뒤부터 놓는다 (4 -> 1 순서) 
			if(nRet_1 == RET_GOOD) //작업 가능한 자재가 있다
			{
				m_n_FirstTray_Y_Num = m_npFindWorkPosYXCPB[0]; m_n_FirstTray_X_Num = m_npFindWorkPosYXCPB[1];
			}
			else if(nRet_1 == RET_SKIP) //더이상 자재가 없다
			{	//이동할 공간이 없을때는 처음 위치로 이동한다 
				m_npFindWorkPosYXCPB[0] = 0; m_npFindWorkPosYXCPB[1] = 0; m_npFindWorkPosYXCPB[2] = 0; m_npFindWorkPosYXCPB[3] = 0; m_npFindWorkPosYXCPB[4] = 0;
				m_n_FirstTray_Y_Num = 0; m_n_FirstTray_X_Num = 0;
			}
			m_nRotator_Flag = 0;
			m_nRunStep = 6330;
			break;

		case 6330:
			nRet_1 = clsFunc.Calculate_MovePos_Find(0, m_nRobot_Y, m_nWorkRobot_uldSite_SortJob_THD_Pos, 0, m_npFindWorkPosYXCPB, &m_dTemp_CalTargetPos_Y);             //Calculate_MovePos_Find(int nMotNum, int nSite, int nWorkPart, int *npFirstYXP_Pos, double *dpGetTargetPos)
			nRet_2 = clsFunc.Calculate_MovePos_Find(0, m_nRobot_X, m_nWorkRobot_uldSite_SortJob_THD_Pos, 0, m_npFindWorkPosYXCPB, &m_dTemp_CalTargetPos_X);
			if(nRet_1 == RET_GOOD && nRet_2 == RET_GOOD)
			{//ok
				m_dpTargetPosList[0] = m_dTemp_CalTargetPos_Y;
				m_dpTargetPosList[1] = m_dTemp_CalTargetPos_X;
			}
			else
			{
				m_nRunStep = 6310;
				CTL_Lib.Alarm_Error_Occurrence(1106, dWARNING, clsFunc.m_strAlarmCode);
				break;
			}

			if(m_nRotator_Flag == 0) //
			{
				m_nRotator_Flag = 1;
				if(m_nWorkRobot_uldSite_SortJob_THD_Pos == THD_LD_BUFF || m_nWorkRobot_uldSite_SortJob_THD_Pos == THD_ULD_BUFF)
				{
					Set_WorkRbtRotator_OnOff(IO_ON); //rotator 돌리면서 이동한다
				}
				else //if(nWork_Site == THD_LD_TRAY_PLATE || nWork_Site == THD_ULD_1_STACKER || nWork_Site == THD_ULD_2_STACKER || nWork_Site == THD_REJECT_OUT_1_STACKER || nWork_Site == THD_REJECT_OUT_2_STACKER)
				{
					Set_WorkRbtRotator_OnOff(IO_OFF); //rotator을 트레이로 놓을 수 있게 회전하면서 이동한다
				}
			}
						
			nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate); 
			if (nRet_1 == BD_GOOD) //좌측으로 이동
			{				 
				m_nXYRobot_PreMove_Flag =  100;
				m_nRunStep = 6310;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nRunStep = 6310;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
				CTL_Lib.Alarm_Error_Occurrence(1107, dWARNING, st_alarm_info.strCode);
				m_nRunStep = 6310;
			}
			break; 	

		//////////////////////////////////////////////////////
		// 자재를 소팅한다 
		//////////////////////////////////////////////////////
		case 6400:
			nRet_1 = Process_DVC_Place(100, m_nWorkRobot_uldSite_SortJob_THD_Pos, 0); //언로딩스태커에소팅한다 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 6410; //james 2016.1003 6500;
			}
			else if(nRet_1 == RET_PICKER_NOT_FIND) //피커는 더이상 작업할 내용이 없다. 이때는 로보트는 버퍼에 놓으러 이동한다, 1 피커로 나로지 않음 
			{
				m_nRunStep = 6400;			
			}
			else if(nRet_1 == RET_TRAY_NOT_FIND) //트레이에는 더 이상 작업할 수 있는 내용이 없다, 1 피커로 나로지 않음 
			{
				//st_sync_info.nLdRobot_Req[m_nLdRobot_Work_Pos_Num] = CTL_CHANGE; //트레이 교체 요청
				//m_nLdRobot_Work_Pos_Num = 0; //피커는 집을 수 있는 곳이있고, 트레이는 작업할 내용이 없으니 clear

				m_nRunStep = 6400;  

			}
			break;

		case 6410: //james 2016.1003 
			nCount = 0;
			for(i = 0; i < 1; i++) //test site에서 작업할 정보를 확보한다
			{					 
				if(st_picker[THD_WORK_RBT].st_pcb_info[i].nYesNo == CTL_YES)// && st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable == CTL_YES)
				{
					nCount++;
				} 
			}
			if(nCount > 0) //자재가 남아있으면 
			{
				m_nRunStep = 6400;   
			}
			else// if(nCount == 0)
			{
				m_nRunStep = 6500;   
			}
			break;

		case 6500:
			nRet_1 = clsFunc.Find_TrayWork_Pos(WORK_PLACE, m_nWorkRobot_uldSite_SortJob_THD_Pos, CTL_NO, m_npFindWorkPosYXCPB, THD_WORK_RBT, st_worker_info.strLotNo_Empty);	
			if(nRet_1 == RET_GOOD) //작업 가능한 공간이 더 있다  
			{				
				if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_TEST_RBT] >=  LOT_END) 
				{//CURR_LOT 자재는 테스트 사이트, Retest Buffer 에도 없고, unload buffer에도 없는 상태이면 현재 lot의 자재는 모두 처리한 상태이다 
					//unload good stacker에서 전체 LOT END 를 관리한다
					// 2016.0918 추가 
					nRet_1 = clsFunc.Find_TrayWork_Pos(WORK_PICK, THD_ULD_BUFF, CTL_YES, m_npFindWorkPosYXCPB, THD_WORK_RBT, st_worker_info.strLotNo_Empty);
// 					//2016.1024
// 					nRet_1 = clsFunc.Find_TrayWork_Pos(WORK_PICK, THD_ULD_BUFF, CTL_YES, m_npFindWorkPosYXCPB, THD_WORK_RBT, st_lot_info[m_nLotProcessNum].strLotNo);
					if(nRet_1 == RET_GOOD)//작업 가능한 자재가 있다 
					{
						m_nRunStep = 6000;
					}
					else if(nRet_1 == RET_SKIP) //더이상 작업할 자재가 없다 
					{//테스트 사이트는 lot end 가 되었고, 언로딩 버퍼에도 더이상 해당 lot의 자재가 없으면 
						m_nRunStep = 9000;
					}
				}
				else
				{
					m_nRunStep = 6000;
				} 				 
			}
			else if(nRet_1 == RET_SKIP) //더이상 작업할 자재가 없다 
			{
				//james 2016.1003 st_sync_info.nWorkRbt_Dvc_Req[m_nWorkRobot_uldSite_SortJob_THD_Pos][0] = CTL_REQ; //미리 요청해 놓는다 

				m_nRunStep = 6600;
			}
			break;

		case 6600: //1 피커로 한번 작업하면 모두 작업이 완료 됨
			if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_TEST_RBT] >=  LOT_END) 
			{//CURR_LOT 자재는 테스트 사이트, Retest Buffer 에도 없고, unload buffer에도 없는 상태이면 현재 lot의 자재는 모두 처리한 상태이다 
			 //unload good stacker에서 전체 LOT END 를 관리한다

				nRet_1 = clsFunc.Find_TrayWork_Pos(WORK_PICK, THD_ULD_BUFF, CTL_YES, m_npFindWorkPosYXCPB, THD_WORK_RBT, st_worker_info.strLotNo_Empty);	
// 				//2016.1024
// 				nRet_1 = clsFunc.Find_TrayWork_Pos(WORK_PICK, THD_ULD_BUFF, CTL_YES, m_npFindWorkPosYXCPB, THD_WORK_RBT, st_lot_info[m_nLotProcessNum].strLotNo);
				if(nRet_1 == RET_GOOD)//작업 가능한 자재가 있다 
				{
					st_sync_info.nWorkRbt_Dvc_Req[m_nWorkRobot_uldSite_SortJob_THD_Pos][0] = CTL_REQ; //미리 요청해 놓는다 
					m_nRunStep = 1000;
				}
				else if(nRet_1 == RET_SKIP) //더이상 작업할 자재가 없다 
				{//테스트 사이트는 lot end 가 되었고, 언로딩 버퍼에도 더이상 해당 lot의 자재가 없으면 
					m_nRunStep = 9000;
				}
			}
			else
			{
				st_sync_info.nWorkRbt_Dvc_Req[m_nWorkRobot_uldSite_SortJob_THD_Pos][0] = CTL_REQ; //미리 요청해 놓는다 
				m_nRunStep = 1000; //6000
			}
			break;

		case 9000:
			//CURR_LOT 자재는 테스트 사이트, Retest Buffer 에도 없고, unload buffer에도 없는 상태이면 현재 lot의 자재는 모두 처리한 상태이다 
			st_lot_info[m_nLotProcessNum].nLotStatus = LOT_END;
			st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] =  LOT_END;//unload good #1, #2 stacker에서 전체 LOT END 를 관리한다 

			//kwlee 2016.1114
			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
				strMsg.Format(_T("[LOT END] LdUld Robot . LotProcessNum =%d "), m_nLotProcessNum );
				clsMem.OnNormalMessageWrite(strMsg);
			}

			if(st_lot_info[LOT_CURR].nLotStatus == LOT_START || st_lot_info[LOT_NEXT].nLotStatus >= LOT_START) //james 2016.0930 == LOT_START)
			{
				m_nRunStep = 1000;
			}
			else
			{
				m_nRunStep = 0; //처음 부터 다시 진행한다 
			}

			break;
	}
}

//james 2016.0912 LOT_CURR 과 LOT_NEXT 를 모두 같이 사용가능하도록 멤버 변수를 사용하여 통합관리 

///////////////////////////////////////////////////////////////////
//nWork_Site == WORKROBT가 PICK 하는 위치는 2 곳이 있다 
//
//1. THD_LD_TRAY_PLATE (load plate tray PICK)
//2. THD_ULD_BUFF (unload buffer PICK)
///////////////////////////////////////////////////////////////////
int CRunLdUldRobot::Process_DVC_Pick(int nMode, int nWork_Site)
{//return 종류 RET_TRAY_NOT_FIND, RET_PICKER_NOT_FIND, RET_GOOD, RET_PROCEED
	int nFuncRet = RET_PROCEED;
	int nRet_1, nRet_2, i, nFlag=0;
	int nPickStep = m_nPick_Step;

	CString strMsg;
	CString strLogKey[10];
	CString strLogData[10];

	//nWork_Site : 작업위치 3곳 가능 : THD_PLATE_LD_TRAY_1_SITE, THD_PLATE_LD_TRAY_2_SITE, THD_RETEST_TRAY_BUFF_SITE
	clsFunc.OnThreadFunctionStepTrace(8, m_nPick_Step);

	switch(m_nPick_Step)
	{
	case 0:
		m_bDvcWaitChk_Falg =  false;
		m_nPickRetry_Cnt = 0;
		m_nPick_Step = 100;
		break;

	case 100:	 
		Set_WorkRbtPicker_UpDown(IO_OFF); //미리 피커를 강제로 올린다

		m_dCurrentPos[m_nRobot_Z] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[m_nRobot_Z] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벗어나 있으면 저위치 시킨다
		{
			m_nPick_Step = 900;
		}
		else
		{
			m_nPick_Step = 1000;
		}
		break;

	case 900:
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_bDvcWaitChk_Falg =  false;
			m_nPick_Step = 1000;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 900;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(1108, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 900;
		}
		break;

	////////////////////////////////////////////////////////////////
	//피커 상태를 확인 후 문제가 없으면 작업할 피커를 찾는다
	////////////////////////////////////////////////////////////////
	case 1000:
		m_dCurrentPos[m_nRobot_Z] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[m_nRobot_Z] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벗어나 있으면 저위치 시킨다
		{
			m_nPick_Step = 900;
			break;
		}

		if(nWork_Site == THD_LD_TRAY_PLATE) // loadig tray 자재 집기 
		{
			nRet_1 = COMI.Get_MotIOSensor(M_LD_ELV, MOT_SENS_SD); 	
			if(nRet_1 != BD_GOOD) //로더 플레이트에 트레이가 감지 된 상태 
			{

				strLogKey[0]	= _T("TYPE");
				strLogData[0]	= _T("LD TRAY_PLATE");
				strLogKey[1]	= _T("STATUS");
				strLogData[1]	= _T("OFF");
				strLogKey[2].Format(_T("SD"));
				strLogData[2].Format(_T("0"));
				clsLog.OnAddKeyAlarm(_T("IO"), _T("ERROR"), 3, strLogKey, strLogData);

				m_strAlarmCode.Format(_T("900001")); //900000 1 0 "LOAD_STACKER_PLATE_SD_TRAY_ON_CHECK_ERROR" //kwlee 2016.0902 "900000" ->"900001"
				CTL_Lib.Alarm_Error_Occurrence(1109, dWARNING, m_strAlarmCode);			
				break;
			}  			 
		}
		else if(nWork_Site == THD_ULD_BUFF)
		{

		}

		nRet_1 = clsFunc.Check_PickerStatus(0, THD_WORK_RBT, CTL_YES, m_npPicker_YesNo, m_npPicker_Vacuum_Status, m_npPicker_OutputStatus);
		if(nRet_1 == RET_GOOD) //전역 변수에 따른 피커 상태를 체크 
		{			//피커 정보와 현재 상태가 정상이다
		}
		else// if(nRet_1 == RET_ERROR)xx
		{
			if(m_bDvcWaitChk_Falg == false)
			{
				m_bDvcWaitChk_Falg =  true;
				m_dwWaitUntil[0] = GetCurrentTime();
				break;
			}
			else
			{
				m_dwWaitUntil[1] = GetCurrentTime();
				m_dwWaitUntil[2] = m_dwWaitUntil[1] - m_dwWaitUntil[0];
				if(m_dwWaitUntil[2] < IO_STABLE_WAIT) break;
				CTL_Lib.Alarm_Error_Occurrence(1200, dWARNING, clsFunc.m_strAlarmCode);
				m_bDvcWaitChk_Falg = false;
				break;
			}
		}

		//작업 가능 위치를 찾는다
		nFlag = 0;
		m_n_FirstTray_Y_Num = -1; m_n_FirstTray_Y_Num = -1; m_n_FirstPicker_Num = -1; //flag clear
		for(i = 0; i < m_nPickerPara; i++)
		{
			if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_NO)//피커에 자재가 들고 있다는 정보가 없다
			{
				m_n_FirstPicker_Num = i;
				i = m_nPickerPara;
			}
		}

		if(m_n_FirstPicker_Num == -1)
		{
			//피커에 더이상 작업할 것이 없다
			m_nPick_Step = 0;
			nFuncRet = RET_PICKER_NOT_FIND;
			break;
		}

		nRet_1 = clsFunc.Find_TrayWork_Pos(WORK_PICK, nWork_Site, CTL_YES, m_npFindWorkPosYXCPB, THD_WORK_RBT, st_worker_info.strLotNo_Empty);
		if(nRet_1 == RET_GOOD) //작업 가능한 자재가 있다
		{
			m_n_FirstTray_Y_Num = m_npFindWorkPosYXCPB[0]; m_n_FirstTray_X_Num = m_npFindWorkPosYXCPB[1];
		}
		else if(nRet_1 == RET_SKIP) //더이상 자재가 없다
		{	//계속 진행할 수 없다
			m_nPick_Step = 0;
			nFuncRet = RET_TRAY_NOT_FIND;
			break;
		}

		nFlag = 0;
		if(m_n_FirstPicker_Num != -1 && m_n_FirstTray_Y_Num != -1 && m_n_FirstTray_X_Num != -1) //작업할 공간이 다 있을때 다음 작업을 진행한다
		{
			for(i = 0; i < m_nPickerPara; i++)  //m_nPickerPara == 1
			{
				m_npPicker_YesNo[i] = CTL_NO; //작업할 피커정보 clear
				if(i < m_n_FirstPicker_Num) continue;

				if(nWork_Site == THD_LD_TRAY_PLATE)
				{
					if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_NO)
					{
						if(i >= 0 && m_n_FirstTray_Y_Num + (i - m_n_FirstPicker_Num) < st_recipe_info.nTrayY)
						{
							if(st_tray_info[nWork_Site].st_pcb_info[ m_n_FirstTray_Y_Num + (i - m_n_FirstPicker_Num)][m_n_FirstTray_X_Num].nYesNo == CTL_YES) //트레이에 자재가  있으면 집기 가능하게 셋
							{
								m_npPicker_YesNo[i] = CTL_YES;
								//2016.1027
								if( st_handler_info.bLoaderOnFlag == CTL_YES )
								{
									m_npPicker_YesNo[i] = CTL_NO;
								}
								nFlag++;
								//2017.0109
								st_picker[m_nRobotSite].st_pcb_info[i].nStatusChang = NO;
							}
						}
					}
				}
				else if(nWork_Site == THD_ULD_BUFF) //작업 순서는 반대로 3,2,1,0  이다.
				{
					if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_NO)
					{
						if(i >= 0 && m_n_FirstTray_Y_Num + (i - m_n_FirstPicker_Num) < st_recipe_info.nLdUldBuffer_Num)
						{
							if(st_buffer_info[nWork_Site].st_pcb_info[m_n_FirstTray_Y_Num - (i - m_n_FirstPicker_Num)].nYesNo == CTL_YES) //트레이에 자재가  있으면 집기 가능하게 셋
							{
								m_npPicker_YesNo[i] = CTL_YES;
								nFlag++;
								//2017.0109
								st_picker[m_nRobotSite].st_pcb_info[i].nStatusChang = NO;
							}
						}
					}

				}
			}

			if(nFlag == 0) //작업할 피커가 하나도 준비되 않았다.
			{ //이부분이 아오면 안됨 
				m_nPick_Step = 0;
				nFuncRet = RET_PICKER_NOT_FIND;//피커에 더이상 작업할 것이 없다
				break;
			}

			//실제 작업 위치를 셋팅
			m_npFindWorkPosYXCPB[0] = m_n_FirstTray_Y_Num; m_npFindWorkPosYXCPB[1] = m_n_FirstTray_X_Num; m_npFindWorkPosYXCPB[3] = m_n_FirstPicker_Num;

			m_nMove_Flag[0] = CTL_NO; m_nMove_Flag[1] = CTL_NO;
			m_nRotator_Flag = 0;
			// jtkim 2016098
			//m_nPick_Step = 2000;
			if(nWork_Site == THD_LD_TRAY_PLATE)
			{
				m_nBarcode1dStep = 0;

				m_nPick_Step = 1100;
			}
			else
			{
				m_nBarcode2dStep = 0;

				m_nPick_Step = 1200;
			}
		}
		break;
		
	case 1100:
// 		nRet_1 = Process_Barcode1D(nWork_Site, m_npFindWorkPosYXCPB[1], m_npFindWorkPosYXCPB[0]);

// 		if (nRet_1 == RET_GOOD)
		{
			m_nPick_Step = 1500;
		}
		break;

	case 1200:
		//2016.1009 
		//로드언로더가 언로더 버퍼에서 디바이스를 픽업할때 1D,2D 모드를 체크해서
		//2D를 못 읽는 모드 인지 체크하여 동작한다.
		if( st_basic_info.nMode2DBcr == CTL_NO )
		{
			m_nPick_Step = 1300;
		}
		else
		{
			m_nPick_Step = 1400;
		}
		break;

	case 1300:
		nRet_1 = Process_Barcode1D(nWork_Site, m_npFindWorkPosYXCPB[1], m_npFindWorkPosYXCPB[0]);

		if (nRet_1 == RET_GOOD)
		{
			m_nPick_Step = 1500;
		}
		break;

	case 1400:
		nRet_1 = Process_Barcode2D(nWork_Site, m_npFindWorkPosYXCPB[1], m_npFindWorkPosYXCPB[0]);

		if (nRet_1 == RET_GOOD)
		{
			m_nPick_Step = 1500;
		}
		break;

	case 1500:
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_nPick_Step = 2000;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 1500;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(1201, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 1500;
		}
		break;

	case 2000:
		m_dCurrentPos[2] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[2] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벅어나 있으면 저위치 시킨다
		{
			m_nPick_Step = 900;
			break;
		}

		if(m_nRotator_Flag == 0) //
		{
			m_nRotator_Flag = 1;
			if(nWork_Site == THD_LD_TRAY_PLATE)//)
			{
				Set_WorkRbtRotator_OnOff(IO_OFF); //rotator 돌리면서 로딩/언로딩 버퍼에서 사용가능하게 돌리면서 이동한다
			}
			else //if(nWork_Site == THD_ULD_BUFF)
			{
				Set_WorkRbtRotator_OnOff(IO_ON); //rotator을 트레이로 놓을 수 있게 회전하면서 이동한다
			}
		}

		if(nWork_Site == THD_ULD_BUFF)
		{//Check_Robot_Buffer_Clash(int nTestDirection, int nRbtTHDSite, int TestBuffTHDSite, int nStartPos, int *nErrorInfoStatus) 
			nRet_1 = clsFunc.Check_Robot_Buffer_Clash(TESTSITE_LEFT, THD_WORK_RBT, THD_ULD_BUFF, m_n_FirstTray_Y_Num, m_n_FirstPicker_Num, m_nClash_ErrorInfoStatus); //피커와 버퍼등의 자재 충돌 확인 
			if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(1202, dWARNING, clsFunc.m_strAlarmCode);		
				break;
			}
		}

		nRet_1 = clsFunc.Calculate_MovePos_Find(0, m_nRobot_Y, nWork_Site, 0, m_npFindWorkPosYXCPB, &m_dTemp_CalTargetPos_Y);             
		nRet_2 = clsFunc.Calculate_MovePos_Find(0, m_nRobot_X, nWork_Site, 0, m_npFindWorkPosYXCPB, &m_dTemp_CalTargetPos_X);
		if(nRet_1 == RET_GOOD && nRet_2 == RET_GOOD)
		{//ok
			m_dpTargetPosList[0] = m_dTemp_CalTargetPos_Y;
			m_dpTargetPosList[1] = m_dTemp_CalTargetPos_X;
		}
		else
		{
			CTL_Lib.Alarm_Error_Occurrence(1203, dWARNING, clsFunc.m_strAlarmCode);
			break;
		}
		
		nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate); 
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_nRotator_Flag = 0;
			m_bDvcWaitChk_Falg =  false;
			m_nPick_Step = 2100;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 2000;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(1204, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 2000;
		}
		break; 	

	case 2100:
		if(nWork_Site == THD_LD_TRAY_PLATE)
		{
			nRet_1 = Chk_WorkRbtRotator_OnOff(IO_OFF); //rotator 돌리면서 이동한다
		}
		else //if(nWork_Site == THD_ULD_BUFF)
		{
			nRet_1 = Chk_WorkRbtRotator_OnOff(IO_ON); //rotator 돌리면서 이동한다
		}
		if(nRet_1 == RET_GOOD)
		{
			m_bDvcWaitChk_Falg = false;

			// jtkim 20160907 임시
			if( st_basic_info.nMode1DBcr == CTL_NO )//2016.0828 
			{
				st_picker[m_nRobotSite].st_pcb_info[m_n_FirstPicker_Num].strBarcode1D[0].Format(_T("SER CODE %04d"), rand() % 10000);
				st_picker[m_nRobotSite].st_pcb_info[m_n_FirstPicker_Num].strBarcode1D[1].Format(_T("IBM CODE %04d"), rand() % 10000);
			}

			m_nPick_Step = 3000;
		}
		else if(nRet_1 == RET_ERROR)
		{
			CTL_Lib.Alarm_Error_Occurrence(1205, dWARNING, m_strAlarmCode);
			m_nPick_Step = 2000;
		} 
		break;

	case 2200: //james 0828
		m_dCurrentPos[2] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[2] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벅어나 있으면 저위치 시킨다
		{
			m_nPick_Step = 900;
			break;
		}

		if( st_basic_info.nMode1DBcr == CTL_NO )//2016.0828 
		{
			st_picker[m_nRobotSite].st_pcb_info[m_n_FirstPicker_Num].strBarcode1D[0].Format(_T("SER CODE %04d"), rand() % 10000);
			st_picker[m_nRobotSite].st_pcb_info[m_n_FirstPicker_Num].strBarcode1D[1].Format(_T("IBM CODE %04d"), rand() % 10000);

			m_nPick_Step = 3000;
			break;
		}
		//kwlee 2016.1007
// 		m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_WORKROBOT_BCR_1D_START];
// 		m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_WORKROBOT_BCR_1D_START];
		m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_WORKROBOT_BCR_1D_START_LD];
		m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_WORKROBOT_BCR_1D_START_LD];
		nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate); 
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_nRotator_Flag = 0; //clear
			m_bDvcWaitChk_Falg =  false;
			m_nPick_Step = 2300;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 2200;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(1206, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 2200;
		}
		break; 	

	case 2300:
		//kwlee 2016.1007 
		//m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_1D_POS]; 
		m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_1D_LD_POS]; 
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, m_dpTargetPosList[2], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{ 						
			m_nPick_Step = 2400;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 2300;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(1207, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 2300;
		}		
		break;


		////////////////////////////////////////////////////////
		// 1 D 바코드를 읽는다//실행한다
		// 바코드 정보를 자재에 넣는다 
		////////////////////////////////////////////////////////
	case 2400:
		if( st_basic_info.nMode1DBcr == CTL_NO )
		{
			st_picker[m_nRobotSite].st_pcb_info[m_n_FirstPicker_Num].strBarcode1D[0].Format(_T("SER CODE %04d"), rand() % 10000);
			st_picker[m_nRobotSite].st_pcb_info[m_n_FirstPicker_Num].strBarcode1D[1].Format(_T("IBM CODE %04d"), rand() % 10000);

			m_nPick_Step = 3000;
		}
		else
		{
			::SendMessage(st_handler_info.hWnd, WM_BARCODE_MSG_1, BARCODE_TRIGGER_1, 0);

			m_nPick_Step = 2500;
		}
		break;

	case 2500:
		if( clsBcr1D.m_nCommand == BD_END)
		{
			//clsBcr1D.m_strBarcode[0];
			//clsBcr1D.m_strBarcode[1];

			m_nRotator_Flag = 0;
			m_nPick_Step = 3000;
		}
		break;
 
	case 2900:
		Set_Glipper_OnOff(IO_ON); //미리 벌린다 
		m_nPick_Step = 2910;
		break;

	case 2910:
		nRet_1 = Chk_Glipper_OnOff(IO_ON); 
		if(nRet_1 == RET_GOOD )
		{
			m_nPick_Step = 3000;
		}
		else if(nRet_1 == RET_ERROR)
		{
			CTL_Lib.Alarm_Error_Occurrence(1208, dWARNING, m_strAlarmCode);
			m_nPick_Step = 2900;
		}
		break;


	///////////////////////////////////////////////
	//  디바이스를 실제 집는 위치 
	///////////////////////////////////////////////
	case 3000: 
		nRet_1 = FAS_IO.get_out_bit(st_io_info.o_LdUldPickGripCyl,	IO_ON); //IO_ON 벌리고 있는 상태이면 집기 동작 가능  
		if(nRet_1 == IO_OFF) 
		{  //그리퍼가 벌리고 있지 않으면 자재에 데미지를 주기때문에 미리 벌리고 내려야 한다 
			m_nPick_Step = 2900;
			break;
		}		

		nRet_1 = COMI.Check_MotPosRange(m_nRobot_Y, m_dpTargetPosList[0], st_motor_info[m_nRobot_Y].d_allow);
		nRet_2 = COMI.Check_MotPosRange(m_nRobot_X, m_dpTargetPosList[1], st_motor_info[m_nRobot_X].d_allow);
			if(nRet_1 != BD_GOOD || nRet_2 != BD_GOOD)
		{
			m_nPick_Step = 2000;
			break;
		}

		if(nWork_Site == THD_LD_TRAY_PLATE)
		{
			m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_LDTRAY_PICK];
		}
		else if(nWork_Site == THD_ULD_BUFF)
		{
			//2016.0923 
			nRet_1 = COMI.Check_MotPosRange(M_ULD_BUF_SHIFTER_Y, st_motor_info[M_ULD_BUF_SHIFTER_Y].d_pos[P_ULDBUFF_Y_WORKRBT], st_motor_info[M_ULD_BUF_SHIFTER_Y].d_allow);
			nRet_2 = COMI.Check_MotPosRange(M_ULD_BUF_ROTATOR, st_motor_info[M_ULD_BUF_ROTATOR].d_pos[P_ULDBUFF_RTR_WORKRBT], st_motor_info[M_ULD_BUF_ROTATOR].d_allow);
			if(nRet_1 != BD_GOOD || nRet_2 != BD_GOOD)
			{
				if(nRet_1 != BD_GOOD ) m_strAlarmCode.Format(_T("%02d0006"), M_ULD_BUF_SHIFTER_Y); 
				else m_strAlarmCode.Format(_T("%02d0006"), M_ULD_BUF_ROTATOR); 
				CTL_Lib.Alarm_Error_Occurrence(1209, dWARNING, m_strAlarmCode);
				break;
			}

			m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_ULDBUF_PICK];
		}
		else
		{
			break;
		}

		nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_START, m_nRobot_Z, m_dpTargetPosList[2], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{ 
			Set_Glipper_OnOff(IO_ON); //미리 벌린다 
			Set_WorkRbtPicker_UpDown(IO_ON); //동작할 피커를 미리 내리면서 이동한다
			m_nPick_Step = 3100;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 3000;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(1210, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 3000;
		}		
		break;

	case 3100:
		nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_CHECK, m_nRobot_Z, m_dpTargetPosList[2], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{ 			 
			m_nPick_Step = 3110;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 3000;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(1211, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 3000;
		}		
		break;

	case 3110: //picker down check 
		nRet_1 = Chk_Glipper_OnOff(IO_ON); 
		nRet_2 = Chk_WorkRbtPicker_UpDown(IO_ON); //동작할 피커를 미리 내리면서 이동한다
		if(nRet_1 == RET_GOOD && nRet_2 == RET_GOOD)
		{
			m_nPick_Step = 3200;
		}
		else if(nRet_1 == RET_ERROR || nRet_2 == RET_ERROR)
		{
			CTL_Lib.Alarm_Error_Occurrence(1212, dWARNING, m_strAlarmCode);
			m_nPick_Step = 3000;
		}
		break;

	///////////////////////////////////////////
	//자재 클램프 
	///////////////////////////////////////////
	case 3200: //자재를 집는다 
		Set_Glipper_OnOff(IO_OFF);  
		m_nPick_Step = 3210;
		break;

	case 3210:
		nRet_1 = Chk_Glipper_OnOff(IO_OFF);  
		if(nRet_1 == RET_GOOD)
		{
			m_bDvcWaitChk_Falg = false;
			m_nPick_Step = 3300;
		}
		else if(nRet_1 == RET_ERROR)
		{
			Set_Glipper_OnOff(IO_ON);  //벌린다  
			CTL_Lib.Alarm_Error_Occurrence(1213, dWARNING, m_strAlarmCode);
			m_nPick_Step = 3200;
		}
		break;

	case 3300://james 2016.0827 그리퍼 디바이스 체크 정보 수정/보완, Gripper Off(클림프상태)에서 IO_OFF시 디바이스 있는상태, 그외는 ON/OFF 산관없이 모두 디바이스 없는 상태
		nRet_1 = FAS_IO.Chk_IO_OnOff(st_io_info.i_LdUldPickDvcChk, IO_ON, IO_STABLE_WAIT, 1000); //IO_STABLE_WAIT == 100ms
		if(nRet_1 == RET_GOOD )
		{
			//m_nPick_Step = 3400;
			//2016.1027
			if( st_handler_info.bLoaderOnFlag == CTL_NO )
			{
				m_nPick_Step = 3400;
			}
			else
			{
				m_nPickRetry_Cnt++;
				if( m_nPickRetry_Cnt < 3)
				{
					m_nPick_Step = 3210;
					break;
				}
				m_nPickRetry_Cnt = 0;
				//900200 0 0 "REMOVE DEVIC. INPUT DEVUCE IS MUCH MORE THAN PRODUCT QTY."
				CTL_Lib.Alarm_Error_Occurrence(1250, dWARNING, _T("900200") );
// 				st_other_info.strBoxMsg = _T("디바이스를 제거해 주세요. 투입량이 생산량보다 많습니다.");
// 				clsMem.OnAbNormalMessagWrite(st_other_info.strBoxMsg);
// 				if (st_handler_info.cWndMain != NULL)
// 				{
// 					st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
// 				}
			}
		}
		else if(nRet_1 == RET_PROCEED)
		{
			break;
		}
		else if(nRet_1 == RET_ERROR)
		{
			if(st_basic_info.nModeDevice == WITHOUT_DVC && st_handler_info.bLoaderOnFlag == CTL_NO) //2016.0922 
			{
				m_nPick_Step = 3400;
				break;
			}

			//2016.1027
			if( st_handler_info.bLoaderOnFlag == CTL_YES )
			{
				m_nPick_Step = 3400;
				/*nFuncRet = RET_TRAY_NOT_FIND;*/
				break;
			}

			m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_LdUldPickDvcChk); 
			CTL_Lib.Alarm_Error_Occurrence(1214, dWARNING, m_strAlarmCode);
		} 
		break;

	case 3400:		
		if(nWork_Site == THD_LD_TRAY_PLATE)  //load tray에서 자재를 집을떄 
		{
			m_nPick_Step = 4000;//Load buffer의 gripper를 이곳에서 제어 
		}
		else //if(nWork_Site == THD_ULD_BUFF)//언로딩 버퍼에서 자재를 집을때
		{
			m_nPick_Step = 3500;// 
		} 
		break;

	//////////////////////////////////////////////////////////////////////////////
	// uld buffer의 그리퍼의 자재를 놓아 워킹로봇에서 자재를 집는것을 제어한다 
	//////////////////////////////////////////////////////////////////////////////
	case 3500: //언로딩 버퍼 그리퍼 오픈 
		FAS_IO.Set_IO_OnOff(st_io_info.o_UldBuffGripOnOffCyl[m_n_FirstTray_Y_Num], IO_ON);//벌린다			 
		m_nPick_Step = 3510;
		break;

	case 3510:
		nRet_1 = FAS_IO.Wait_IO_OnOff(st_io_info.o_UldBuffGripOnOffCyl[m_n_FirstTray_Y_Num], IO_OFF, st_wait_info.nOnWaitTime[WAIT_RBTGLIPPER_ON_OFF]);
		if(nRet_1 == RET_GOOD) //작업 가능한 자재가 있다
		{
			m_nPick_Step = 3520;
		}
		break;

	case 3520:
		nRet_1 = FAS_IO.Chk_IO_OnOff(st_io_info.i_UldBuffGripDvcExistChk[m_n_FirstTray_Y_Num], IO_ON, IO_STABLE_WAIT, 2000); //IO_STABLE_WAIT == 100ms
		if(nRet_1 == RET_GOOD) //감지가 안되도 안전화 시간까지는 기다리자 
		{
			m_nPick_Step = 3530;
		} 
		else if(nRet_1 == RET_PROCEED)
		{
			break;
		}
		else if(nRet_1 == RET_ERROR)
		{
			if(st_basic_info.nModeDevice == WITHOUT_DVC) //2016.0922 
			{
				m_nPick_Step = 3530;
				break;
			}

			m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_LdBuffGripDvcExistChk[m_n_FirstTray_Y_Num]); 
			CTL_Lib.Alarm_Error_Occurrence(1215, dWARNING, m_strAlarmCode);
		} 
		break; 

	case 3530:
		nRet_1 = FAS_IO.Chk_IO_OnOff(st_io_info.i_LdUldPickDvcChk, IO_ON, IO_STABLE_WAIT, 1000); //IO_STABLE_WAIT == 100ms
		if(nRet_1 == RET_GOOD)
		{
			m_nPick_Step = 4000;
		}
		else if(nRet_1 == RET_PROCEED)
		{
			break;
		}
		else if(nRet_1 == RET_ERROR)
		{
			if(st_basic_info.nModeDevice == WITHOUT_DVC) //2016.0922 
			{
				m_nPick_Step = 4000;
				break;
			}

			m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_LdUldPickDvcChk); 
			CTL_Lib.Alarm_Error_Occurrence(1216, dWARNING, m_strAlarmCode);
		} 
		break;
	//////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////
	//로보트 up 
	///////////////////////////////////////////
	case 4000:
		m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY];
		nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_START, m_nRobot_Z, m_dpTargetPosList[2], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{ 
			Set_WorkRbtPicker_UpDown(IO_OFF); //동작한 피커를 올린다  
			m_nPick_Step = 4100 ; //시간 단추을 위헤 체크는 다음으로 
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 4000;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(1217, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 4000;
		}		
		break;

	case 4100:
		nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_CHECK,m_nRobot_Z, m_dpTargetPosList[2], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{ 			 
			m_nPick_Step = 4110;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 4000;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(1218, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 4000;
		}		
		break;

	case 4110:
		nRet_1 = Chk_WorkRbtPicker_UpDown(IO_OFF); //동작할 피커를 미리 내리면서 이동한다
		if(nRet_1 == RET_GOOD)
		{
			m_bDvcWaitChk_Falg = false;
			m_nPick_Step = 5000;
		}
		else if(nRet_1 == RET_ERROR)
		{
			CTL_Lib.Alarm_Error_Occurrence(1219, dWARNING, m_strAlarmCode);
			m_nPick_Step = 4000;
		}
		break;

	case 5000: //picker 정보와 상태를 확인한다
// 		nRet_1 = clsFunc.Check_PickerStatus(1, THD_WORK_RBT, CTL_YES, m_npPicker_YesNo, m_npPicker_Vacuum_Status, m_npPicker_OutputStatus);
		//2016.1027
		if( st_handler_info.bLoaderOnFlag == CTL_YES )
		{
			nRet_1 = clsFunc.Check_PickerStatus(0, THD_WORK_RBT, CTL_NO, m_npPicker_YesNo, m_npPicker_Vacuum_Status, m_npPicker_OutputStatus);
		}
		else
		{
			nRet_1 = clsFunc.Check_PickerStatus(1, THD_WORK_RBT, CTL_YES, m_npPicker_YesNo, m_npPicker_Vacuum_Status, m_npPicker_OutputStatus);
		}
		if (nRet_1 != RET_GOOD) //
		{
			if(m_bDvcWaitChk_Falg == false)
			{
				m_bDvcWaitChk_Falg =  true;
				m_dwWaitUntil[0] = GetCurrentTime();
				break;
			}
			else
			{
				m_dwWaitUntil[1] = GetCurrentTime();
				m_dwWaitUntil[2] = m_dwWaitUntil[1] - m_dwWaitUntil[0];
				if(m_dwWaitUntil[2] < IO_STABLE_WAIT) break;

				//2017.0312 이상한데 왜 알람이 없었을까> 그냥 데이타가 이동하면 안되는데..
				CTL_Lib.Alarm_Error_Occurrence(7927, dWARNING, clsFunc.m_strAlarmCode);
				break;
			}
		}

		for (i = m_n_FirstPicker_Num; i < m_nPickerPara; i++)
		{
			// jtkim 20160905
			//if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_NO && m_npPicker_YesNo[i] == CTL_YES && m_npPicker_Vacuum_Status[i] == CTL_YES)
			//2016.1027
			if( st_handler_info.bLoaderOnFlag == CTL_YES )
			{
				if(nWork_Site == THD_LD_TRAY_PLATE)
				{
					if(i >= 0 && m_npFindWorkPosYXCPB[0] + (i - m_n_FirstPicker_Num) < st_recipe_info.nTrayY)
					{
						m_npPicker_YesNo[i] = CTL_NO; //작업한 피커 정보 클리어

						//아래 함수에서 사용위해 위치 정보를 셋팅한다
						m_npSet_WorkPosYXCPB[0]  = m_npFindWorkPosYXCPB[0] + (i - m_n_FirstPicker_Num); //tray Y Pos 위치
						m_npSet_WorkPosYXCPB[1]  = m_npFindWorkPosYXCPB[1]; //tray X Pos 위치
						m_npSet_WorkPosYXCPB[3]  = i; //picker 위치 정보

						clsFunc.Data_Exchange_PickPlace(PICKER_PICK_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //피커 및 트레이 정보 셋팅 함수						
					}
				}
			}			
			else if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_NO && m_npPicker_YesNo[i] == CTL_YES
				&& (m_npPicker_Vacuum_Status[i] == IO_ON && m_npPicker_OutputStatus[i] == IO_OFF))
			{
				if(nWork_Site == THD_LD_TRAY_PLATE)
				{
					if(i >= 0 && m_npFindWorkPosYXCPB[0] + (i - m_n_FirstPicker_Num) < st_recipe_info.nTrayY)
					{
						m_npPicker_YesNo[i] = CTL_NO; //작업한 피커 정보 클리어

						//아래 함수에서 사용위해 위치 정보를 셋팅한다
						m_npSet_WorkPosYXCPB[0]  = m_npFindWorkPosYXCPB[0] + (i - m_n_FirstPicker_Num); //tray Y Pos 위치
						m_npSet_WorkPosYXCPB[1]  = m_npFindWorkPosYXCPB[1]; //tray X Pos 위치
						m_npSet_WorkPosYXCPB[3]  = i; //picker 위치 정보

						clsFunc.Data_Exchange_PickPlace(PICKER_PICK_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //피커 및 트레이 정보 셋팅 함수						

						st_count_info.nSSD_Input[0]++;
						// jtkim 20160830
						st_count_info.nInCount[1][0]++; // daily
					
						if (st_lot_info[m_nLotProcessNum].strLotNo == st_picker[m_nRobotSite].st_pcb_info[i].strLotNo)//진행중인 Lot과 같으면
						{
							// 현재 또는 next lot 수량 증가
							st_lot_info[m_nLotProcessNum].nLdCnt++;

							if(m_nLotProcessNum == LOT_CURR)
							{
								st_count_info.nInCount[0][0]++;
							}
							else if(m_nLotProcessNum == LOT_NEXT)
							{
								st_count_info.nInCount[0][1]++;
							}
						}
						else
						{ 
							//현재 진행한 lot 정보와 같지 않으면 메세지 출력 
						}
					}
				}
				else if(nWork_Site == THD_ULD_BUFF)
				{
					if(i >= 0 && m_npFindWorkPosYXCPB[0] + (i - m_n_FirstPicker_Num) < st_recipe_info.nLdUldBuffer_Num)
					{
						m_npPicker_YesNo[i] = CTL_NO; //작업한 피커 정보 클리어

						//아래 함수에서 사용위해 위치 정보를 셋팅한다
						m_npSet_WorkPosYXCPB[0]  = m_npFindWorkPosYXCPB[0] + (i - m_n_FirstPicker_Num); //tray Y Pos 위치
						m_npSet_WorkPosYXCPB[1]  = m_npFindWorkPosYXCPB[1]; //tray X Pos 위치
						m_npSet_WorkPosYXCPB[3]  = i; //picker 위치 정보

						clsFunc.Data_Exchange_PickPlace(PICKER_PICK_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //피커 및 트레이 정보 셋팅 함수		

						//2016.1203
						if (m_strLotNo[1] == _T(""))
						{
							m_strLotNo[1]	= st_picker[THD_WORK_RBT].st_pcb_info[i].strLotNo;
							m_strPartNo[1] = st_picker[THD_WORK_RBT].st_pcb_info[i].strPartNo;
						}
					}
				}
			}
		}

		if (nRet_1 == RET_GOOD) // 1000 step에서 셋팅한 피커 정보에 맞게 모두 정상적 집었을 경우 이
		{
			//2017.0109
			//kwlee 2017.0117 막아놓음..
			/*
			st_picker[m_nRobotSite].nLogCount		= 0;
			st_picker[m_nRobotSite].strMaterial	= _T("");

			for (i = m_n_FirstPicker_Num; i < m_nPickerPara; i++)
			{
				if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_YES)
				{
					if(nWork_Site == THD_LD_TRAY_PLATE || nWork_Site == THD_ULD_BUFF)
					{
						if (st_picker[m_nRobotSite].st_pcb_info[i].nStatusChang == NO)
						{
							st_picker[m_nRobotSite].st_pcb_info[i].nStatusChang = YES;

							st_count_info.nInCount[1][0]++;
					
							if (st_lot_info[LOT_CURR].strLotNo == st_tray_info[nWork_Site ].strLotNo)
							{
									st_lot_info[LOT_CURR].nLdCnt++;
									st_count_info.nInCount[0][0]++;

								if (st_count_info.nInCount[0][0] == 1)
								{
									st_lot_info[LOT_CURR].tLotStart = CTime::GetCurrentTime();
								}
							}
							else
							{
								// next lot 수량 증가
									st_lot_info[LOT_NEXT].nLdCnt++;
									st_count_info.nInCount[0][1]++;

								if (st_count_info.nInCount[0][1] == 1)
								{
									st_lot_info[LOT_NEXT].tLotStart = CTime::GetCurrentTime();
								}
							}


							if( nWork_Site == THD_LD_TRAY_PLATE )
							{
								st_picker[m_nRobotSite].st_pcb_info[i].strLogKey[0]	= _T("LD_TRAY_PLATE_SITE");
								st_picker[m_nRobotSite].st_pcb_info[i].strLogData[0]	= st_picker[m_nRobotSite].st_pcb_info[i].strSerialNo;

								st_picker[m_nRobotSite].st_pcb_info[i].strMaterial.Format(_T("%s"), st_picker[m_nRobotSite].st_pcb_info[i].strSerialNo);
								st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[0] = clsLog.GetSiteName( nWork_Site );
								st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[1] = _T("LOAD_BUFFER_SITE");

								clsLog.LogTransfer(_T("LOADING_ROBOT"),
													_T("MOVE_CDIMM"),
													0,
													st_picker[m_nRobotSite].st_pcb_info[i].strMaterial,
													_T("CDIMM"),
													st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[0],
													st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[1],
													1, 
													st_picker[m_nRobotSite].st_pcb_info[i].strLogKey,
													st_picker[m_nRobotSite].st_pcb_info[i].strLogData);
							}
							else if( nWork_Site == THD_ULD_BUFF )
							{
								st_picker[m_nRobotSite].st_pcb_info[i].strLogKey[0]	= _T("UNLOAD_BUFFER");
								st_picker[m_nRobotSite].st_pcb_info[i].strLogData[0]	= st_picker[m_nRobotSite].st_pcb_info[i].strSerialNo;

								// jtkim 20161112
								st_picker[m_nRobotSite].st_pcb_info[i].strMaterial.Format(_T("%s"), st_picker[m_nRobotSite].st_pcb_info[i].strSerialNo);
								st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[0] = clsLog.GetSiteName( nWork_Site );
								if (st_picker[m_nRobotSite].st_pcb_info[i].nBin == BD_DATA_REJECT)
								{
									if(st_picker[m_nRobotSite].st_pcb_info[i].strLotNo == st_tray_info[THD_REJECT_OUT_1_STACKER].strLotNo)
									{
										st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[1] = _T("REJECT#1_TRAY");
									}
									else
									{
										st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[1] = _T("REJECT#2_TRAY");
									}
								}
								else
								{
									if(st_picker[m_nRobotSite].st_pcb_info[i].strLotNo == st_tray_info[THD_ULD_1_STACKER].strLotNo)
									{
										st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[1] = _T("UNOADLD#1_TRAY");
									}
									else
									{
										st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[1] = _T("UNOADLD#2_TRAY");
									}
								}

								clsLog.LogTransfer(_T("UNLOADING_ROBOT"),
													_T("MOVE_CDIMM"),
													0,
													st_picker[m_nRobotSite].st_pcb_info[i].strMaterial,
													_T("CDIMM"),
													st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[0],
													st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[1],
													1, 
													st_picker[m_nRobotSite].st_pcb_info[i].strLogKey,
													st_picker[m_nRobotSite].st_pcb_info[i].strLogData);
							}
						}
					}
					else
					{
						if (st_picker[m_nRobotSite].st_pcb_info[i].nStatusChang == NO)
						{
							st_picker[m_nRobotSite].st_pcb_info[i].nStatusChang = YES;
						}
					}
				}
			}
			*/
			//화면 정보 갱신
			m_nDvcRepickCount = 0;
			m_nPartialPickCount = 0;

			m_bDvcWaitChk_Falg = false;

			// jtkim 20150216 conveyor/loader tray 화면 표시
			if (st_handler_info.cWndMain != NULL)
			{
				st_handler_info.cWndMain->PostMessage(WM_WORK_DISPLAY, MAIN_LD_PICKER_DISPLAY, 0);
				st_handler_info.cWndMain->PostMessage(WM_WORK_DISPLAY, MAIN_LD_TRAY_DISPLAY, 0);
				st_handler_info.cWndMain->PostMessage(WM_WORK_DISPLAY, MAIN_RETEST_BUF_DISPLAY, 0);
			}

			m_nPick_Step = 6000;
		}
		else if (nRet_1 == RET_ERROR) //셋팅한 피커중 한개라도 제대로 집지 못했으면 이곳으로 이동
		{
			m_nDvcRepickCount++;
			CTL_Lib.Alarm_Error_Occurrence(1220, dWARNING, clsFunc.m_strAlarmCode);
			//kwlee 2017.0214
			//m_nPick_Step = 0;
		}
		break;

	case 6000:
		nRet_1 = clsFunc.Check_PickerStatus(0, THD_WORK_RBT, CTL_YES, m_npPicker_YesNo, m_npPicker_Data_Status, m_npPicker_OutputStatus);
		if (nRet_1 == RET_GOOD) //
		{
			nFuncRet = RET_GOOD;
			m_nPick_Step = 0;
		}
		else //if(nRet_1 == RET_ERROR)
		{
			if(m_bDvcWaitChk_Falg == false)
			{
				m_bDvcWaitChk_Falg =  true;
				m_dwWaitUntil[0] = GetCurrentTime();
				break;
			}
			else
			{
				m_dwWaitUntil[1] = GetCurrentTime();
				m_dwWaitUntil[2] = m_dwWaitUntil[1] - m_dwWaitUntil[0];
				if(m_dwWaitUntil[2] < IO_STABLE_WAIT) break;
				CTL_Lib.Alarm_Error_Occurrence(1221, dWARNING, clsFunc.m_strAlarmCode);
			}
		}
		break;

	default :
		break;
 
	}


	return nFuncRet;
}



//james 2016.0912 LOT_CURR 과 LOT_NEXT 를 모두 같이 사용가능하도록 멤버 변수를 사용하여 통합관리 

//PLACE의 nWork_Site 는 5곳이다
//1.THD_LD_BUFF
//2.THD_ULD_1_STACKER
//3.THD_ULD_2_STACKER
//4. THD_REJECT_OUT_1_STACKER
//5. THD_REJECT_OUT_2_STACKER
int CRunLdUldRobot::Process_DVC_Place(int nMode, int nWork_Site, int nTestSite_Pos) //nTestSitePos == TESTSITE_LEFT, TESTSITE_RIGHT 둘중 하나 
{//return 종류 RET_TRAY_NOT_FIND, RET_PICKER_NOT_FIND, RET_GOOD, RET_PROCEED
	int nFuncRet = RET_PROCEED;
	int nRet_1, nRet_2, i, nFlag, nTemp;
	 
//	int x, y;
	//if(nWork_Site == THD_LD_BUFF,THD_ULD_1_STACKER,THD_ULD_2_STACKER,THD_REJECT_OUT_1_STACKER,THD_REJECT_OUT_2_STACKER
	clsFunc.OnThreadFunctionStepTrace(7, m_nPlace_Step);

	switch(m_nPlace_Step)
	{
	case 0:
		m_bDvcWaitChk_Falg =  false;
		m_nPickRetry_Cnt = 0;
		m_nPlace_Step = 100;
		break;

	case 100:	 
		Set_WorkRbtPicker_UpDown(IO_OFF); //미리 피커를 강제로 올린다

		m_dCurrentPos[m_nRobot_Z] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[m_nRobot_Z] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벗어나 있으면 저위치 시킨다
		{
			m_nPlace_Step = 900;
		}
		else
		{
			m_nPlace_Step = 1000;
		}
		break;

	case 900:
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_bDvcWaitChk_Falg =  false;
			m_nPlace_Step = 1000;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 900;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(1230, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 900;
		}
		break;

	////////////////////////////////////////////////////////////////
	//피커 상태를 확인 후 문제가 없으면 작업할 피커를 찾는다
	////////////////////////////////////////////////////////////////
	case 1000:
		//2015.0305 추가 
		nRet_1 = clsFunc.Check_PickerStatus(0, THD_WORK_RBT, CTL_YES, m_npPicker_YesNo, m_npPicker_Vacuum_Status, m_npPicker_OutputStatus); //전역 변수와 센서 상태 체크 
		if(nRet_1 == RET_GOOD)
		{
			m_bDvcWaitChk_Falg = false; //피커 정보와 현재 상태가 정상이다
		}
		else// if(nRet_1 == RET_ERROR)
		{
			if(m_bDvcWaitChk_Falg == false)
			{
				m_bDvcWaitChk_Falg =  true;
				m_dwWaitUntil[0] = GetCurrentTime();
				break;
			}
			else
			{
				m_dwWaitUntil[1] = GetCurrentTime();
				m_dwWaitUntil[2] = m_dwWaitUntil[1] - m_dwWaitUntil[0];
				if(m_dwWaitUntil[2] < IO_STABLE_WAIT) break;
				CTL_Lib.Alarm_Error_Occurrence(1231, dWARNING, clsFunc.m_strAlarmCode);
				break;
			}
		}


		m_dCurrentPos[2] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[2] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벅어나 있으면 저위치 시킨다
		{
			m_nPlace_Step = 900;
			break;
		}		 

		//작업 가능 위치를 찾는다
		m_n_FirstTray_Y_Num = -1; m_n_FirstTray_Y_Num = -1; m_n_FirstPicker_Num = -1; //flag clear
		for(i = 0; i < m_nPickerPara; i++)
		{
			if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_YES)//피커에 자재가 들고 있다는 정보가 없다
			{
				m_n_FirstPicker_Num = i;
				i = m_nPickerPara;
			}
		}

		if(m_n_FirstPicker_Num == -1)
		{
			//피커에 더이상 작업할 것이 없다
			m_nPlace_Step = 0;
			nFuncRet = RET_PICKER_NOT_FIND;
			break;
		}

		nRet_1 = clsFunc.Find_TrayWork_Pos(WORK_PLACE, nWork_Site, CTL_NO, m_npFindWorkPosYXCPB, THD_WORK_RBT, st_worker_info.strLotNo_Empty); //놓을때버퍼에서으 ㅣ작업순서는 뒤부터 놓는다 (4 -> 1 순서) 
		if(nRet_1 == RET_GOOD) //작업 가능한 자재가 있다
		{
			m_n_FirstTray_Y_Num = m_npFindWorkPosYXCPB[0]; m_n_FirstTray_X_Num = m_npFindWorkPosYXCPB[1];
		}
		else if(nRet_1 == RET_SKIP) //더이상 자재가 없다
		{	//계속 진행할 수 없다
			m_nPlace_Step = 0;
			nFuncRet = RET_TRAY_NOT_FIND;
			break;
		}

		nFlag = 0;
		if(m_n_FirstPicker_Num != -1 && m_n_FirstTray_Y_Num != -1 && m_n_FirstTray_X_Num != -1) //작업할 공간이 다 있을때 다음 작업을 진행한다
		{
			for(i = 0;  i < m_nPickerPara; i++)  //m_n_FirstPicker_Num; y < m_mPickerPara; y++)
			{
				m_npPicker_YesNo[i] = CTL_NO; //작업할 피커정보 clear
				if(i < m_n_FirstPicker_Num) continue;

				if(nWork_Site == THD_ULD_1_STACKER || nWork_Site == THD_ULD_2_STACKER || nWork_Site == THD_REJECT_OUT_1_STACKER || nWork_Site == THD_REJECT_OUT_2_STACKER)
				{
					if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_YES)// && st_picker[m_nRobotSite].st_pcb_info[i].nEnable == CTL_YES)
					{
						if(i >= 0 && m_n_FirstTray_Y_Num + (i - m_n_FirstPicker_Num) < st_recipe_info.nTrayY)
						{
							if(st_tray_info[nWork_Site].st_pcb_info[ m_n_FirstTray_Y_Num + (i - m_n_FirstPicker_Num)][m_n_FirstTray_X_Num].nYesNo == CTL_NO) //트레이에 자재가  없으면 놓기 가능하게 셋
							{
								m_npPicker_YesNo[i] = CTL_YES;
								nFlag++;
							}
						}
					} 
				}
				else if(nWork_Site == THD_LD_BUFF || nWork_Site == THD_ULD_BUFF)
				{
					if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_YES)
					{
						if(i >= 0 && m_n_FirstTray_Y_Num + (i - m_n_FirstPicker_Num) < st_recipe_info.nLdUldBuffer_Num)
						{
							if(st_buffer_info[nWork_Site].st_pcb_info[m_n_FirstTray_Y_Num + (i - m_n_FirstPicker_Num)].nYesNo == CTL_NO && st_buffer_info[nWork_Site].st_pcb_info[m_n_FirstTray_Y_Num + (i - m_n_FirstPicker_Num)].nEnable == CTL_YES) // 
							{//버퍼에 자재가 없고, 사용 가능한 위치일때만  자재를 놓을수 있게 한다.
								m_npPicker_YesNo[i] = CTL_YES;
								nFlag++;
							}
						}
					}
				}
			}
			if(nFlag == 0) //작업할 피커가 하나도 준비되 않았다.
			{ //이부분이 아오면 안됨 
				m_nPlace_Step = 0;
				nFuncRet = RET_PICKER_NOT_FIND;//피커에 더이상 작업할 것이 없다
				break;
			}

			//실제 작업 위치를 셋팅
			m_npFindWorkPosYXCPB[0] = m_n_FirstTray_Y_Num; m_npFindWorkPosYXCPB[1] = m_n_FirstTray_X_Num; m_npFindWorkPosYXCPB[3] = m_n_FirstPicker_Num;

			m_nMove_Flag[0] = CTL_NO; m_nMove_Flag[1] = CTL_NO;

			m_nRotator_Flag = 0; //clear
/*
			if(nWork_Site == THD_LD_BUFF)
			{
				m_nBarcode2dStep = 0;

				m_nPlace_Step = 1100; //정해진 위치에서 1D 바코드를 읽고, 디바이스를 버퍼에 놓고 2D 바코드를 읽는다 
			}
			else
			{
				m_nPlace_Step = 2000;
			}
*/
			m_nPlace_Step = 2000;
		}
		break; 
/*
	////////////////////////////////////////////////////////
	// 정해진 위치에서 1D 바코드를 일고, 디바이스를 버퍼에 놓고 2D 바코드를 읽는다 
	////////////////////////////////////////////////////////
	case 1100: 
		m_dCurrentPos[2] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[2] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벅어나 있으면 저위치 시킨다
		{
			m_nPlace_Step = 900;
			break;
		}

		m_nPlace_Step = 2000;
		break;

		if( st_basic_info.nMode1DBcr == CTL_NO )//2016.0828 
		{
			m_nPlace_Step = 2000;
			break;
		}

		m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_WORKROBOT_BCR_1D_START];
		m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_WORKROBOT_BCR_1D_START];
		nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate); 
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_nRotator_Flag = 0; //clear
			m_bDvcWaitChk_Falg =  false;
			m_nPlace_Step = 1200;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 1100;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 1100;
		}
		break; 	

	case 1200:
		m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_1D_POS]; 
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, m_dpTargetPosList[2], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{ 						
			m_nPlace_Step = 1300;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 1200;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 1200;
		}		
		break;


	////////////////////////////////////////////////////////
	// 1 D 바코드를 읽는다//실행한다
	// 바코드 정보를 자재에 넣는다 
	////////////////////////////////////////////////////////
	case 1300:
		if( st_basic_info.nMode1DBcr == CTL_NO )
		{
			st_picker[m_nRobotSite].st_pcb_info[m_n_FirstPicker_Num].strBarcode2D.Format(_T("2D CODE %04d"), rand() % 10000);
			
			m_nPlace_Step = 2000;
		}
		else
		{
			::SendMessage(st_handler_info.hWnd, WM_BARCODE_MSG_1, BARCODE_TRIGGER_1, 0);
			m_nPlace_Step = 1400;
		}
		break;

	case 1400:
		if( clsBcr1D.m_nCommand == BD_END)
		{
			//clsBcr1D.m_strBarcode[0];
			//clsBcr1D.m_strBarcode[1];

			m_nRotator_Flag = 0;
			m_nPlace_Step = 2000;
		}
		
		break;
*/
	////////////////////////////////////////////////////////
	// 작업 할 위치로 X,Y robot 및 rotator 실린더 동작한다
	////////////////////////////////////////////////////////
	case 2000:
		m_dCurrentPos[2] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[2] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벗어나 있으면 정위치 시킨다
		{
			m_nPlace_Step = 900;
			break;
		}

		if(nWork_Site == THD_LD_BUFF)
		{
			nRet_1 = clsFunc.Check_Robot_Buffer_Clash(TESTSITE_LEFT, THD_WORK_RBT, THD_LD_BUFF, m_n_FirstTray_Y_Num, m_n_FirstPicker_Num, m_nClash_ErrorInfoStatus); //피커와 버퍼등의 자재 충돌 확인 
			if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(1232, dWARNING, clsFunc.m_strAlarmCode);		
				break;
			}
		}

		if(m_nRotator_Flag == 0) //
		{
			m_nRotator_Flag = 1;
			if(nWork_Site == THD_LD_BUFF || nWork_Site == THD_ULD_BUFF)
			{
				Set_WorkRbtRotator_OnOff(IO_ON); //rotator 돌리면서 이동한다
			}
			else //if(nWork_Site == THD_LD_TRAY_PLATE || nWork_Site == THD_ULD_1_STACKER || nWork_Site == THD_ULD_2_STACKER || nWork_Site == THD_REJECT_OUT_1_STACKER || nWork_Site == THD_REJECT_OUT_2_STACKER)
			{
				Set_WorkRbtRotator_OnOff(IO_OFF); //rotator을 트레이로 놓을 수 있게 회전하면서 이동한다
			}
		}

		nRet_1 = clsFunc.Calculate_MovePos_Find(0, m_nRobot_Y, nWork_Site, 0, m_npFindWorkPosYXCPB, &m_dTemp_CalTargetPos_Y);             //Calculate_MovePos_Find(int nMotNum, int nSite, int nWorkPart, int *npFirstYXP_Pos, double *dpGetTargetPos)
		nRet_2 = clsFunc.Calculate_MovePos_Find(0, m_nRobot_X, nWork_Site, 0, m_npFindWorkPosYXCPB, &m_dTemp_CalTargetPos_X);
		if(nRet_1 == RET_GOOD && nRet_2 == RET_GOOD)
		{//ok
			m_dpTargetPosList[0] = m_dTemp_CalTargetPos_Y;
			m_dpTargetPosList[1] = m_dTemp_CalTargetPos_X;
		}
		else
		{
			CTL_Lib.Alarm_Error_Occurrence(1233, dWARNING, clsFunc.m_strAlarmCode);
			break;
		}

		nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate); 
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_nRotator_Flag = 0; //clear
			m_bDvcWaitChk_Falg =  false;
			m_nPlace_Step = 2100;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 2000;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(1234, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 2000;
		}
		break; 	

	case 2100:
		if(nWork_Site == THD_LD_BUFF || nWork_Site == THD_ULD_BUFF)
		{
			nRet_1 = Chk_WorkRbtRotator_OnOff(IO_ON); //rotator 돌리면서 이동한다
		}
		else //if(nWork_Site == THD_LD_TRAY_PLATE || nWork_Site == THD_ULD_1_STACKER || nWork_Site == THD_ULD_2_STACKER || nWork_Site == THD_REJECT_OUT_1_STACKER || nWork_Site == THD_REJECT_OUT_2_STACKER)
		{
			nRet_1 = Chk_WorkRbtRotator_OnOff(IO_OFF); //rotator을 트레이로 놓을 수 있게 회전하면서 이동한다
		} 
		if(nRet_1 == RET_GOOD)
		{
			m_bDvcWaitChk_Falg = false;
			m_nPlace_Step = 3000;
		}
		else if(nRet_1 == RET_ERROR)
		{
			CTL_Lib.Alarm_Error_Occurrence(1235, dWARNING, m_strAlarmCode);
			m_nPlace_Step = 2000;
		} 
		break;

	case 2900: //james 2016.0922 
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_bDvcWaitChk_Falg =  false;
			m_nPlace_Step = 3000;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 2900;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(1236, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 2900;
		}
		break;


	////////////////////////////////////////////////////////
	// 작업 할 자재를 놓는다 
	////////////////////////////////////////////////////////
	case 3000: 		
		m_dCurrentPos[2] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[2] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벗어나 있으면 정위치 시킨다
		{
			m_nPlace_Step = 2900;
			break;
		}

		nRet_1 = clsFunc.Check_PickerStatus(0, THD_WORK_RBT, CTL_YES, m_npPicker_YesNo, m_npPicker_Vacuum_Status, m_npPicker_OutputStatus);
		if(nRet_1 == RET_GOOD)
		{
			m_bDvcWaitChk_Falg = false; //피커 정보와 현재 상태가 정상이다
		}
		else// if(nRet_1 == RET_ERROR)
		{
			if(m_bDvcWaitChk_Falg == false)
			{
				m_bDvcWaitChk_Falg =  true;
				m_dwWaitUntil[0] = GetCurrentTime();
				break;
			}
			else
			{
				m_dwWaitUntil[1] = GetCurrentTime();
				m_dwWaitUntil[2] = m_dwWaitUntil[1] - m_dwWaitUntil[0];
				if(m_dwWaitUntil[2] < IO_STABLE_WAIT) break;
				CTL_Lib.Alarm_Error_Occurrence(1237, dWARNING, clsFunc.m_strAlarmCode);
				break;
			}
		}

		nRet_1 = COMI.Check_MotPosRange(m_nRobot_Y, m_dpTargetPosList[0], st_motor_info[m_nRobot_Y].d_allow);
		nRet_2 = COMI.Check_MotPosRange(m_nRobot_X, m_dpTargetPosList[1], st_motor_info[m_nRobot_X].d_allow);
		if(nRet_1 != BD_GOOD || nRet_2 != BD_GOOD)
		{
			m_nPlace_Step = 2000;
			break;
		}

		if(nWork_Site == THD_LD_BUFF)
		{
			nRet_1 = COMI.Check_MotPosRange(M_LD_BUF_SHIFTER_Y, st_motor_info[M_LD_BUF_SHIFTER_Y].d_pos[P_LDBUFF_Y_WORKRBT], st_motor_info[M_LD_BUF_SHIFTER_Y].d_allow);
			nRet_2 = COMI.Check_MotPosRange(M_LD_BUF_ROTATOR, st_motor_info[M_LD_BUF_ROTATOR].d_pos[P_LDBUFF_RTR_WORKRBT], st_motor_info[M_LD_BUF_ROTATOR].d_allow);
			if(nRet_1 != BD_GOOD || nRet_2 != BD_GOOD)
			{
				if(nRet_1 != BD_GOOD ) m_strAlarmCode.Format(_T("%02d0006"), M_LD_BUF_SHIFTER_Y); 
				else m_strAlarmCode.Format(_T("%02d0006"), M_LD_BUF_ROTATOR); 
				CTL_Lib.Alarm_Error_Occurrence(1238, dWARNING, m_strAlarmCode);
				break;
			}

			//rotator 의 상태가 정상인지 확인한다 
			// jtkim 20160905
			// nRet_1 = FAS_IO.Chk_IO_OnOff(st_io_info.i_LdBuffGripDvcExistChk[m_n_FirstTray_Y_Num], IO_ON, IO_STABLE_WAIT, IO_STABLE_LIMIT); //IO_STABLE_WAIT == 100ms
			nRet_1 = FAS_IO.Chk_IO_OnOff(st_io_info.i_LdBuffGripDvcExistChk[m_n_FirstTray_Y_Num], IO_ON, IO_STABLE_WAIT, IO_STABLE_LIMIT); 
			if(nRet_1 == RET_PROCEED && st_basic_info.nModeDevice != WITHOUT_DVC) //감지가 안되도 안전화 시간까지는 기다리자 
			{
				break; 
			} 
			else if(nRet_1 == RET_ERROR && st_basic_info.nModeDevice != WITHOUT_DVC) //with device 가 아니면  
			{
				m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_LdBuffGripDvcExistChk[m_n_FirstTray_Y_Num]); 
				CTL_Lib.Alarm_Error_Occurrence(1239, dWARNING, m_strAlarmCode);
				break;
			}  

			nRet_1 = FAS_IO.Chk_IO_OnOff(st_io_info.i_LdUldPickRot_90_Chk, IO_ON, IO_STABLE_WAIT, IO_STABLE_LIMIT); //IO_STABLE_WAIT == 100ms
			if(nRet_1 == RET_PROCEED) //감지가 안되도 안전화 시간까지는 기다리자 
			{
				break; 
			} 
			else if(nRet_1 == RET_ERROR)
			{
				if(nWork_Site == THD_LD_BUFF || nWork_Site == THD_ULD_BUFF)
				{
					Set_WorkRbtRotator_OnOff(IO_ON); //rotator 돌리면서 이동한다
				}
				else //if(nWork_Site == THD_LD_TRAY_PLATE || nWork_Site == THD_ULD_1_STACKER || nWork_Site == THD_ULD_2_STACKER || nWork_Site == THD_REJECT_OUT_1_STACKER || nWork_Site == THD_REJECT_OUT_2_STACKER)
				{
					Set_WorkRbtRotator_OnOff(IO_OFF); //rotator을 트레이로 놓을 수 있게 회전하면서 이동한다
				}

				m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_LdUldPickRot_90_Chk); 
				CTL_Lib.Alarm_Error_Occurrence(1240, dWARNING, m_strAlarmCode);
				break;
			}  
		}
		else if(nWork_Site == THD_ULD_BUFF)
		{
			nRet_1 = COMI.Check_MotPosRange(M_ULD_BUF_SHIFTER_Y, st_motor_info[M_ULD_BUF_SHIFTER_Y].d_pos[P_ULDBUFF_Y_WORKRBT], st_motor_info[M_ULD_BUF_SHIFTER_Y].d_allow);
			nRet_2 = COMI.Check_MotPosRange(M_ULD_BUF_ROTATOR, st_motor_info[M_ULD_BUF_ROTATOR].d_pos[P_ULDBUFF_RTR_WORKRBT], st_motor_info[M_ULD_BUF_ROTATOR].d_allow);
			if(nRet_1 != BD_GOOD || nRet_2 != BD_GOOD)
			{
				if(nRet_1 != BD_GOOD ) m_strAlarmCode.Format(_T("%02d0006"), M_ULD_BUF_SHIFTER_Y); 
				else m_strAlarmCode.Format(_T("%02d0006"), M_ULD_BUF_ROTATOR); 
				CTL_Lib.Alarm_Error_Occurrence(1241, dWARNING, m_strAlarmCode);
				break;
			}

			//rotator 의 상태가 정상인지 확인한다 
			// jtkim 20160905
			// nRet_1 = FAS_IO.Chk_IO_OnOff(st_io_info.i_LdBuffGripDvcExistChk[m_n_FirstTray_Y_Num], IO_ON, IO_STABLE_WAIT, IO_STABLE_LIMIT); //IO_STABLE_WAIT == 100ms
			nRet_1 = FAS_IO.Chk_IO_OnOff(st_io_info.i_UldBuffGripDvcExistChk[m_n_FirstTray_Y_Num], IO_ON, IO_STABLE_WAIT, IO_STABLE_LIMIT); 
			if(nRet_1 == RET_PROCEED && st_basic_info.nModeDevice != WITHOUT_DVC) //감지가 안되도 안전화 시간까지는 기다리자 
			{
				break; 
			} 
			else if(nRet_1 == RET_ERROR && st_basic_info.nModeDevice != WITHOUT_DVC) //with device 가 아니면  
			{
				m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_UldBuffGripDvcExistChk[m_n_FirstTray_Y_Num]); 
				CTL_Lib.Alarm_Error_Occurrence(1242, dWARNING, m_strAlarmCode);
				break;
			}  

			nRet_1 = FAS_IO.Chk_IO_OnOff(st_io_info.i_LdUldPickRot_90_Chk, IO_ON, IO_STABLE_WAIT, IO_STABLE_LIMIT); //IO_STABLE_WAIT == 100ms
			if(nRet_1 == RET_PROCEED) //감지가 안되도 안전화 시간까지는 기다리자 
			{
				break; 
			} 
			else if(nRet_1 == RET_ERROR)
			{
				if(nWork_Site == THD_LD_BUFF || nWork_Site == THD_ULD_BUFF)
				{
					Set_WorkRbtRotator_OnOff(IO_ON); //rotator 돌리면서 이동한다
				}
				else //if(nWork_Site == THD_LD_TRAY_PLATE || nWork_Site == THD_ULD_1_STACKER || nWork_Site == THD_ULD_2_STACKER || nWork_Site == THD_REJECT_OUT_1_STACKER || nWork_Site == THD_REJECT_OUT_2_STACKER)
				{
					Set_WorkRbtRotator_OnOff(IO_OFF); //rotator을 트레이로 놓을 수 있게 회전하면서 이동한다
				}

				m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_LdUldPickRot_90_Chk); 
				CTL_Lib.Alarm_Error_Occurrence(1243, dWARNING, m_strAlarmCode);
				break;
			}  
		}
		else //if(nWork_Site == THD_LD_TRAY_PLATE || nWork_Site == THD_ULD_1_STACKER || nWork_Site == THD_ULD_2_STACKER || nWork_Site == THD_REJECT_OUT_1_STACKER || nWork_Site == THD_REJECT_OUT_2_STACKER)
		{
			nRet_1 = FAS_IO.Chk_IO_OnOff(st_io_info.i_LdUldPickRot_0_Chk, IO_ON, IO_STABLE_WAIT, IO_STABLE_LIMIT); //IO_STABLE_WAIT == 100ms
			if(nRet_1 == RET_PROCEED) //감지가 안되도 안전화 시간까지는 기다리자 
			{
				break;
			} 
			else if(nRet_1 == RET_ERROR)
			{
				if(nWork_Site == THD_LD_BUFF || nWork_Site == THD_ULD_BUFF)
				{
					Set_WorkRbtRotator_OnOff(IO_ON); //rotator 돌리면서 이동한다
				}
				else //if(nWork_Site == THD_LD_TRAY_PLATE || nWork_Site == THD_ULD_1_STACKER || nWork_Site == THD_ULD_2_STACKER || nWork_Site == THD_REJECT_OUT_1_STACKER || nWork_Site == THD_REJECT_OUT_2_STACKER)
				{
					Set_WorkRbtRotator_OnOff(IO_OFF); //rotator을 트레이로 놓을 수 있게 회전하면서 이동한다
				}

				m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_LdUldPickRot_0_Chk); 
				CTL_Lib.Alarm_Error_Occurrence(1244, dWARNING, m_strAlarmCode);
				break;
			}  
		}

		if(nWork_Site == THD_LD_TRAY_PLATE)
		{
			m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_LDTRAY_PLACE];
		}
		else if(nWork_Site == THD_LD_BUFF)
		{
			m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_LDBUF_PLCAE];
		}
		else if(nWork_Site == THD_ULD_BUFF)
		{
			m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_ULDBUF_PLCAE];
		}
		else if(nWork_Site == THD_ULD_1_STACKER)
		{
			m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_ULDTRAY_1_PLACE];
		}
		else if(nWork_Site == THD_ULD_2_STACKER)
		{
			m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_ULDTRAY_2_PLACE];
		}
		else if(nWork_Site == THD_REJECT_OUT_1_STACKER)
		{
			m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_REJECTTRAY_1_PLACE];
		}
		else if(nWork_Site == THD_REJECT_OUT_2_STACKER)
		{
			m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_REJECTTRAY_2_PLACE];
		}
		
		nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_START, m_nRobot_Z, m_dpTargetPosList[2], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{ 			
			Set_WorkRbtPicker_UpDown(IO_ON); //동작할 피커를 미리 내리면서 이동한다
			m_nPlace_Step = 3100;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 3000;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(1245, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 3000;
		}		
		break;

	case 3100:
		nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_CHECK, m_nRobot_Z, m_dpTargetPosList[2], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{ 			 
			m_nPlace_Step = 3110;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 3000;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(1246, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 3000;
		}		
		break;

	case 3110:
		nRet_1 = Chk_WorkRbtPicker_UpDown(IO_ON); //동작할 피커를 미리 내리면서 이동한다
		if(nRet_1 == RET_GOOD)
		{
			m_nPlace_Step = 3200;
		}
		else if(nRet_1 == RET_ERROR)
		{
			CTL_Lib.Alarm_Error_Occurrence(1247, dWARNING, m_strAlarmCode);
		}
		break;

	case 3200:
		if(nWork_Site == THD_LD_BUFF)// || nWork_Site == THD_ULD_BUFF) //로딩 자재를 로딩 버퍼에 놓을떄 
		{
			m_nPlace_Step = 3300;//Load buffer의 gripper를 이곳에서 제어 
		}
		else //if(nWork_Site == THD_LD_TRAY_PLATE || nWork_Site == THD_ULD_1_STACKER || nWork_Site == THD_ULD_2_STACKER || nWork_Site == THD_REJECT_OUT_1_STACKER || nWork_Site == THD_REJECT_OUT_2_STACKER)
		{
			m_nPlace_Step = 3400;//트레이에 놓는 영역
		} 
		break;

	////////////////////////////////////////////////////////////
	//Load buffer의 gripper를 이곳에서 제어 
	//제어 단순화을 위해 로딩 버퍼의 그리퍼를 이곳에서 제어 한다 
	////////////////////////////////////////////////////////////
	case 3300:
		FAS_IO.Set_IO_OnOff(st_io_info.o_LdBuffGripOnOffCyl[m_n_FirstTray_Y_Num], IO_OFF);//오므린다			 
		m_nPlace_Step = 3310;
		break;

	case 3310:
		nRet_1 = FAS_IO.Wait_IO_OnOff(st_io_info.o_LdBuffGripOnOffCyl[m_n_FirstTray_Y_Num], IO_OFF, st_wait_info.nOffWaitTime[WAIT_RBTGLIPPER_ON_OFF]);
		if(nRet_1 == RET_GOOD) //작업 가능한 자재가 있다
		{
			m_nPlace_Step = 3320;
		}
		break;

	case 3320:
		nRet_1 = FAS_IO.Chk_IO_OnOff(st_io_info.i_LdBuffGripDvcExistChk[m_n_FirstTray_Y_Num], IO_ON, IO_STABLE_WAIT, 1000);
		if(nRet_1 == RET_GOOD) //감지가 안되도 안전화 시간까지는 기다리자 
		{
			m_nPlace_Step = 3400;
		} 
		else if(nRet_1 == RET_PROCEED)
		{
			break;
		}
		else if(nRet_1 == RET_ERROR)
		{
			if(st_basic_info.nModeDevice == WITHOUT_DVC)
			{
				m_nPlace_Step = 3400;
				break;
			}
			m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_LdBuffGripDvcExistChk[m_n_FirstTray_Y_Num]); 
			CTL_Lib.Alarm_Error_Occurrence(1248, dWARNING, m_strAlarmCode);
		} 
		break; 

	/////////////////////////////////////////////////////////////
	//로보트의 자재를 로딩 버퍼에 놓는다 
	////////////////////////////////////////////////////////////
	case 3400:
		Set_Glipper_OnOff(IO_ON); // 벌린다 
		m_nPlace_Step = 3410;
		break;

	case 3410:
		nRet_1 = Chk_Glipper_OnOff(IO_ON);  
		if(nRet_1 == RET_GOOD)
		{
			m_bDvcWaitChk_Falg = false;
			m_nPlace_Step = 3420;
		}
		else if(nRet_1 == RET_ERROR)
		{
			CTL_Lib.Alarm_Error_Occurrence(1249, dWARNING, m_strAlarmCode);
			m_nPlace_Step = 3400;
		} 
		break;

	case 3420://james 2016.0827 그리퍼 디바이스 체크 정보 수정/보완, Gripper Off(클림프상태)에서 IO_OFF시 디바이스 있는상태, 그외는 ON/OFF 산관없이 모두 디바이스 없는 상태
		nRet_1 = FAS_IO.Chk_IO_OnOff(st_io_info.i_LdUldPickDvcChk, IO_ON, st_wait_info.nOnWaitTime[WAIT_RBTGLIPPER_ON_OFF], st_wait_info.nLimitWaitTime[WAIT_RBTGLIPPER_ON_OFF]); //IO_STABLE_WAIT == 100ms
		if(nRet_1 == RET_GOOD) //감지가 안되도 안전화 시간까지는 기다리자 
		{
			m_nPlace_Step = 4000;
		} 
		else if(nRet_1 == RET_ERROR)
		{
			m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_LdUldPickDvcChk); 
			CTL_Lib.Alarm_Error_Occurrence(1250, dWARNING, m_strAlarmCode);
		} 
		break; 

	////////////////////////////////////////////////////////////
	//로보트를 안전 위치로 올린다 
	////////////////////////////////////////////////////////////
	case 4000: 		
		nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_START, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			Set_WorkRbtPicker_UpDown(IO_OFF); //미리 피커를 강제로 올린다

			m_bDvcWaitChk_Falg =  false;
			m_nPlace_Step = 4010;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 4000;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(1251, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 4000;	
		}
		break;

	case 4010:
		nRet_1 = Chk_WorkRbtPicker_UpDown(IO_OFF);  
		if(nRet_1 == RET_GOOD)
		{
			m_bDvcWaitChk_Falg = false;
			m_nPlace_Step = 4020;
		}
		else if(nRet_1 == RET_ERROR)
		{
			CTL_Lib.Alarm_Error_Occurrence(1252, dWARNING, m_strAlarmCode);
			m_nPlace_Step = 4000;
		} 
		break;

	case 4020:
		nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_CHECK, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_bDvcWaitChk_Falg =  false;
			m_nPlace_Step = 4100;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 4000;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(1253, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 4000;
		}
		break;		

	case 4100:
		if(nWork_Site == THD_LD_BUFF)
		{
			mn_BcrFirst = CTL_YES;//2016.1111
			mn_BcrFirstChk = CTL_NO;
			mn_BcrFirstFinal = CTL_NO;
			m_nPlace_Step = 4200; //정해진 위치에서 1D 바코드를 일고, 디바이스를 버퍼에 놓고 2D 바코드를 읽는다 
		}
		else
		{
			m_nPlace_Step = 5000; 
		}
		break;
/*
	////////////////////////////////////////////////////////
	// 정해진 위치에서 1D 바코드를 일고, 디바이스를 버퍼에 놓고 2D 바코드를 읽는다 
	////////////////////////////////////////////////////////
	case 4200: 
		m_dCurrentPos[2] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[2] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벅어나 있으면 저위치 시킨다
		{
			m_nPlace_Step = 4000;
			break;
		}

		if( st_basic_info.nMode2DBcr == CTL_NO )//2016.0828 
		{
			m_nPlace_Step = 5000;
			break;
		}

		m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_WORKROBOT_BCR_2D_START_LD];
		m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_WORKROBOT_BCR_2D_START_LD];
		nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate); 
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_nRotator_Flag = 0; //clear
			m_bDvcWaitChk_Falg =  false;
			m_nPlace_Step = 4300;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 4200;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 4200;
		}
		break; 	

	case 4300:
		m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_2D_LD_POS]; 
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, m_dpTargetPosList[2], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{ 						
			m_nPlace_Step = 4400;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 4300;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 4300;
		}		
		break;

	////////////////////////////////////////////////////////
	// 2 D 바코드를 읽는다//실행한다
	// 바코드 정보를 자재에 넣는다 
	////////////////////////////////////////////////////////
	case 4400: 
		
		if( st_basic_info.nMode2DBcr == CTL_NO )
		{
			m_nPlace_Step = 5000;
		}
		else
		{
			::SendMessage(st_handler_info.hWnd, WM_BARCODE_MSG_2, BARCODE_TRIGGER_1, 0);
			m_nPlace_Step = 4500;
		}
		break;

	case 4500:
		if( clsBcr2D.m_nCommand == BD_END)
		{
			//clsBcr2D.m_strBarcode[0];		
			m_nPlace_Step = 5000;
		}
		
		break;
*/
/*
	case 4200://2016.1006
// 		nRet_1 = Process_Barcode2D(nWork_Site, m_npFindWorkPosYXCPB[1], m_npFindWorkPosYXCPB[0]);
		
		nRet_1 = Process_Barcode1D(nWork_Site, m_npFindWorkPosYXCPB[1], m_npFindWorkPosYXCPB[0]);
		if (nRet_1 == RET_GOOD)
		{
			m_nRotator_Flag = 0;
			m_nPlace_Step = 4300;
			//m_nPlace_Step = 4250;//2016.1006
		}
		break;
*/
		//2016.1111
	case 4200:
		m_dCurrentPos[2] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[2] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벅어나 있으면 저위치 시킨다
		{
			m_nPlace_Step = 4000;
			break;
		}
		if( st_basic_info.nMode2DBcr == CTL_NO )
		{
			m_nPlace_Step = 4260;
		}
		else
		{
			m_nPlace_Step = 4250;
		}
		break;


	//2016.1006
	case 4250:
		//2016.1009
		if( st_basic_info.nMode2DBcr == CTL_NO )
		{
			m_nPlace_Step = 4300;
		}
		else
		{
			nRet_1 = Process_Barcode2D(nWork_Site, m_npFindWorkPosYXCPB[1], m_npFindWorkPosYXCPB[0]);

			if (nRet_1 == RET_GOOD)
			{
				m_nRotator_Flag = 0;
				m_nPlace_Step = 4260;
				if( mn_BcrFirstFinal == CTL_YES)
				{
					m_nPlace_Step = 4300;
				}
			}
		}
		break;


	case 4260:
		nRet_1 = Process_Barcode1D(nWork_Site, m_npFindWorkPosYXCPB[1], m_npFindWorkPosYXCPB[0]);
		if (nRet_1 == RET_GOOD)
		{
			if( mn_BcrFirstChk == CTL_NO)
			{
				mn_BcrFirst = CTL_NO;
				mn_BcrFirstFinal = CTL_YES;
				m_nPlace_Step = 4250;	
				if( st_basic_info.nMode2DBcr == CTL_NO )
				{
					m_nPlace_Step = 4300;
				}
			}
			else
			{				
				m_nPlace_Step = 4300;	
			}
		}
		break;


	case 4300:
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_nPlace_Step = 5000;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 4300;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(1254, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 4300;
		}
		break;
	////////////////////////////////////////////////////////
	// 작업을 완료한 상태로 피커 상태를 확인한다
	// 피커 데이터를 버퍼/트레이에 data exchange 한다
	////////////////////////////////////////////////////////
	case 5000: //picker 정보와 상태를 확인한다
		nRet_1 = clsFunc.Check_PickerStatus(1, THD_WORK_RBT, CTL_NO, m_npPicker_YesNo, m_npPicker_Vacuum_Status, m_npPicker_OutputStatus);
		if (nRet_1 != RET_GOOD) //
		{//안정 시간을 준다
			if(m_bDvcWaitChk_Falg == false)
			{
				m_bDvcWaitChk_Falg =  true;
				m_dwWaitUntil[0] = GetCurrentTime();
				break;
			}
			else
			{
				m_dwWaitUntil[1] = GetCurrentTime();
				m_dwWaitUntil[2] = m_dwWaitUntil[1] - m_dwWaitUntil[0];
				if(m_dwWaitUntil[2] < IO_STABLE_WAIT) break;

				CTL_Lib.Alarm_Error_Occurrence(1255, dWARNING, clsFunc.m_strAlarmCode);//2017.0313
				break;
			}
		}

		for (i = m_n_FirstPicker_Num; i < m_nPickerPara; i++)
		{
			//james 2016.0919 그리퍼의 output 상태를 체크 

			if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_YES && m_npPicker_YesNo[i] == CTL_YES
				&& (m_npPicker_Vacuum_Status[i] == IO_ON && m_npPicker_OutputStatus[i] == IO_ON))//james 2016.0918 CTL_YES)
			{

				if (m_strLotNo[1] == _T(""))
				{
					m_strLotNo[1]	= st_picker[m_nRobotSite].st_pcb_info[i].strLotNo;
					m_strPartNo[1] = st_picker[m_nRobotSite].st_pcb_info[i].strPartNo;
				}

				if(nWork_Site == THD_ULD_1_STACKER || nWork_Site == THD_ULD_2_STACKER || nWork_Site == THD_REJECT_OUT_1_STACKER || nWork_Site == THD_REJECT_OUT_2_STACKER)
				{
					if(i >= 0 && m_npFindWorkPosYXCPB[0] + (i - m_n_FirstPicker_Num) < st_recipe_info.nTrayY)
					{
						m_npPicker_YesNo[i] = CTL_NO;	//작업한 피커 정보 클리어

						//아래 함수에서 사용위해 위치 정보를 셋팅한다
						m_npSet_WorkPosYXCPB[0] = m_npFindWorkPosYXCPB[0] + (i - m_n_FirstPicker_Num); //tray Y Pos 위치
						m_npSet_WorkPosYXCPB[1] = m_npFindWorkPosYXCPB[1]; //tray X Pos 위치
						m_npSet_WorkPosYXCPB[3]  = i; //picker 위치 정보

						clsFunc.Data_Exchange_PickPlace(PICKER_PLACE_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //피커 및 트레이 정보 셋팅 함수
						//위 함수에서 모든 정보를 셋팅 및 클리어 한다
						//st_picker[m_nRobotSite].st_pcb_info[y].nYesNo = CTL_NO;					//피커 정보 셋
						//st_tray_info[nWork_Site].st_pcb_info[m_npFirstPosYXP[0] + (y - m_n_FirstPicker_Num)][m_npFirstPosYXP[1]].nYesNo = CTL_YES; //트레이 정보 셋 //자재있음
						if (st_tray_info[nWork_Site].st_pcb_info[m_npSet_WorkPosYXCPB[0]][m_npSet_WorkPosYXCPB[1]].nRetestCnt <= 1)
						{
							if(nWork_Site == THD_ULD_1_STACKER || nWork_Site == THD_ULD_2_STACKER) //james 2016.0912 
							{
								st_count_info.nPrimeCount[1][0]++;
								st_count_info.nPassCount[1][0]++;

								///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
								//2016.1203
								//UPH
								nTemp = (int)(st_handler_info.tRef.GetHour());

								st_count_info.nLatestUph[nTemp]++;
								st_count_info.nUph++;								
								st_product_info.nUph++;

								nTemp += 1;
								if (nTemp == 24) nTemp = 0;
								if (st_count_info.nLatestUph[nTemp] != 0) st_count_info.nLatestUph[nTemp] = 0;
								//UPH
								///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

								if (st_lot_info[m_nLotProcessNum].strLotNo == st_tray_info[nWork_Site].st_pcb_info[m_npSet_WorkPosYXCPB[0]][m_npSet_WorkPosYXCPB[1]].strLotNo)
								{
									// 현재 진행중인 Lot과 같으면, // next lot 수량 증가

									if(m_nLotProcessNum == LOT_CURR)
									{
										//st_lot_info[LOT_CURR].nLdCnt++;
										st_count_info.nPrimeCount[0][0]++;
										st_count_info.nPassCount[0][0]++;
									}
									else if(m_nLotProcessNum == LOT_NEXT)
									{
										//st_lot_info[LOT_NEXT].nLdCnt++;
										st_count_info.nPrimeCount[0][1]++;
										st_count_info.nPassCount[0][1]++;
									}

									/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
									//2016.1203
									st_product_info.nPrimeCount++;
									if (st_product_info.nPrimeCount > 0) 
									{
										st_product_info.dPrime = ((double)st_product_info.nPrimeCount / ((double)st_product_info.nPrimeCount + (double)st_product_info.nRejectCount)) * (double)100;
									}
									/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


									st_lot_info[m_nLotProcessNum].nTotalCnt++;

									//2017.0109
									clsLog.LogTransfer(_T("UNLOADING_ROBOT"),
										   _T("MOVE_CDIMM"),
										   0,
										   st_picker[m_nRobotSite].st_pcb_info[i].strMaterial,
										   _T("SSD"),
										   st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[0],
										   st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[1],
										   1, 
										   st_picker[m_nRobotSite].st_pcb_info[i].strLogKey,
										   st_picker[m_nRobotSite].st_pcb_info[i].strLogData);

								}
							}
							else if(nWork_Site == THD_REJECT_OUT_1_STACKER || nWork_Site == THD_REJECT_OUT_2_STACKER) //2016.0912 
							{
								st_count_info.nRejectCount[1][0]++;

								/////////////////////////////////////////////////////////////////////////////////////////////////////
								//2016.1203
								st_product_info.nRejectCount++;
								/////////////////////////////////////////////////////////////////////////////////////////////////////

								if (st_lot_info[m_nLotProcessNum].strLotNo == st_tray_info[nWork_Site].st_pcb_info[m_npSet_WorkPosYXCPB[0]][m_npSet_WorkPosYXCPB[1]].strLotNo)
								{
									if(m_nLotProcessNum == LOT_CURR)
									{
										// 현재 진행중인 Lot과 같으면
										//st_lot_info[LOT_CURR].nLdCnt++;
										st_count_info.nRejectCount[0][0]++;
									}
									else if(m_nLotProcessNum == LOT_NEXT)
									{
										// next lot 수량 증가
										//st_lot_info[LOT_NEXT].nLdCnt++;
										st_count_info.nRejectCount[0][1]++;
									}

									st_lot_info[m_nLotProcessNum].nTotalCnt++;
								} 
								//2017.0109
								clsLog.LogTransfer(_T("UNLOADING_ROBOT"),
										_T("MOVE_CDIMM"),
										0,
										st_picker[m_nRobotSite].st_pcb_info[i].strMaterial,
										_T("CDIMM"),
										st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[0],
										st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[1],
										1, 
										st_picker[m_nRobotSite].st_pcb_info[i].strLogKey,
										st_picker[m_nRobotSite].st_pcb_info[i].strLogData);
							}
							else if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF) //2016.0912 
							{
								st_count_info.nPrimeRejectCount[1][0]++;

								if (st_lot_info[m_nLotProcessNum].strLotNo == st_tray_info[nWork_Site].st_pcb_info[m_npSet_WorkPosYXCPB[0]][m_npSet_WorkPosYXCPB[1]].strLotNo)
								{
									if(m_nLotProcessNum == LOT_CURR)
									{
										// 현재 진행중인 Lot과 같으면
										//st_lot_info[LOT_CURR].nLdCnt++;
										st_count_info.nPrimeRejectCount[0][0]++;
									}
									else if(m_nLotProcessNum == LOT_NEXT)
									{
										// next lot 수량 증가
										//st_lot_info[LOT_NEXT].nLdCnt++;
										st_count_info.nPrimeRejectCount[0][1]++;
									}
								} 
							}
						}
						else
						{
							if(nWork_Site == THD_ULD_1_STACKER || nWork_Site == THD_ULD_2_STACKER)
							{
								st_count_info.nPassCount[1][0]++;
								if (st_lot_info[m_nLotProcessNum].strLotNo == st_tray_info[nWork_Site].st_pcb_info[m_npSet_WorkPosYXCPB[0]][m_npSet_WorkPosYXCPB[1]].strLotNo)
								{
									if(m_nLotProcessNum == LOT_CURR)
									{
										// 현재 진행중인 Lot과 같으면
										//st_lot_info[LOT_CURR].nLdCnt++;
										st_count_info.nPassCount[0][0]++;
									}
									else if(m_nLotProcessNum == LOT_NEXT)
									{
										// next lot 수량 증가
										//st_lot_info[LOT_NEXT].nLdCnt++;
										st_count_info.nPassCount[0][1]++;
									}

									st_lot_info[m_nLotProcessNum].nTotalCnt++;
								}
								//2017.0109
								clsLog.LogTransfer(_T("UNLOADING_ROBOT"),
										_T("MOVE_CDIMM"),
										0,
										st_picker[m_nRobotSite].st_pcb_info[i].strMaterial,
										_T("CDIMM"),
										st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[0],
										st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[1],
										1, 
										st_picker[m_nRobotSite].st_pcb_info[i].strLogKey,
										st_picker[m_nRobotSite].st_pcb_info[i].strLogData);

							}
							else if(nWork_Site == THD_REJECT_OUT_1_STACKER || nWork_Site == THD_REJECT_OUT_2_STACKER) //2016.0912 
							{
								st_count_info.nRejectCount[1][0]++;
								if (st_lot_info[m_nLotProcessNum].strLotNo == st_tray_info[nWork_Site].st_pcb_info[m_npSet_WorkPosYXCPB[0]][m_npSet_WorkPosYXCPB[1]].strLotNo)
								{
									if(m_nLotProcessNum == LOT_CURR)
									{
										// 현재 진행중인 Lot과 같으면
										//st_lot_info[LOT_CURR].nLdCnt++;
										st_count_info.nRejectCount[0][0]++;
									}
									else if(m_nLotProcessNum == LOT_NEXT)
									{
										// next lot 수량 증가
										//st_lot_info[LOT_NEXT].nLdCnt++;
										st_count_info.nRejectCount[0][1]++;
									}
									st_lot_info[m_nLotProcessNum].nTotalCnt++;
								}
								//2017.0109
								clsLog.LogTransfer(_T("UNLOADING_ROBOT"),
										_T("MOVE_CDIMM"),
										0,
										st_picker[m_nRobotSite].st_pcb_info[i].strMaterial,
										_T("CDIMM"),
										st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[0],
										st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[1],
										1, 
										st_picker[m_nRobotSite].st_pcb_info[i].strLogKey,
										st_picker[m_nRobotSite].st_pcb_info[i].strLogData);
							}
						}
					}
				}					 
				else if(nWork_Site = THD_LD_BUFF)
				{
					if(i >= 0 && m_npFindWorkPosYXCPB[0] + (i - m_n_FirstPicker_Num) < st_recipe_info.nLdUldBuffer_Num)
					{
						m_npPicker_YesNo[i] = CTL_NO;	//작업한 피커 정보 클리어

						//아래 함수에서 사용위해 위치 정보를 셋팅한다
						m_npSet_WorkPosYXCPB[0] = m_npFindWorkPosYXCPB[0] + (i - m_n_FirstPicker_Num); //tray Y Pos 위치
						m_npSet_WorkPosYXCPB[1] = m_npFindWorkPosYXCPB[1] + (i - m_n_FirstPicker_Num); //tray X Pos 위치
						m_npSet_WorkPosYXCPB[3]  = i; //picker 위치 정보

						clsFunc.Data_Exchange_PickPlace(PICKER_PLACE_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //피커 및 트레이 정보 셋팅 함수
						//위 함수에서 모든 정보를 셋팅 및 클리어 한다
						//st_picker[m_nRobotSite].st_pcb_info[y].nYesNo = CTL_NO;					//피커 정보 셋
						//st_tray_info[nWork_Site].st_pcb_info[m_npFirstPosYXP[0] + (y - m_n_FirstPicker_Num)][m_npFirstPosYXP[1]].nYesNo = CTL_YES; //트레이 정보 셋 //자재있음						
					}
					//2017.0109
					clsLog.LogTransfer(_T("LOADING_ROBOT"),
							_T("MOVE_CDIMM"),
							0,
							st_picker[m_nRobotSite].st_pcb_info[i].strMaterial,
							_T("CDIMM"),
							st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[0],
							st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[1],
							1, 
							st_picker[m_nRobotSite].st_pcb_info[i].strLogKey,
							st_picker[m_nRobotSite].st_pcb_info[i].strLogData);
				}				
			}
		}

		if (nRet_1 == RET_GOOD) // 1000 step에서 셋팅한 피커 정보에 맞게 모두 정상적 집었을 경우 이
		{
			//화면 정보 갱신
			m_bDvcWaitChk_Falg = false;

			// jtkim 20150303 conveyor/loader tray 화면 표시
			if (st_handler_info.cWndMain != NULL)
			{
				st_handler_info.cWndMain->PostMessage(WM_WORK_DISPLAY, MAIN_LD_PICKER_DISPLAY, 0);
				st_handler_info.cWndMain->PostMessage(WM_WORK_DISPLAY, MAIN_LD_BUF_PICK_DISPLAY, 0);
				st_handler_info.cWndMain->PostMessage(WM_WORK_DISPLAY, MAIN_COUNT_DISPLAY, 0);
			}
			m_nPlace_Step = 6000;
		}
		else if (nRet_1 == RET_ERROR) //셋팅한 피커중 한개라도 제대로 집지 못했으면 이곳으로 이동
		{
			 CTL_Lib.Alarm_Error_Occurrence(1255, dWARNING, clsFunc.m_strAlarmCode);
		}
		break;

	case 6000:
		nRet_1 = clsFunc.Check_PickerStatus(0, THD_WORK_RBT, CTL_YES, m_npPicker_YesNo, m_npPicker_Data_Status, m_npPicker_OutputStatus);
		
		if(nRet_1 == RET_GOOD)
		{
			nFuncRet = RET_GOOD;
			m_nPlace_Step = 0;
		}
		if (nRet_1 == RET_ERROR) //
		{
			if(m_bDvcWaitChk_Falg == false)
			{
				m_bDvcWaitChk_Falg =  true;
				m_dwWaitUntil[0] = GetCurrentTime();
				break;
			}
			else
			{
				m_dwWaitUntil[1] = GetCurrentTime();
				m_dwWaitUntil[2] = m_dwWaitUntil[1] - m_dwWaitUntil[0];
				if(m_dwWaitUntil[2] < IO_STABLE_WAIT) break;
			}
			CTL_Lib.Alarm_Error_Occurrence(1256, dWARNING, clsFunc.m_strAlarmCode);
		}
		break; 

	default :
		break;
	}

	return nFuncRet;
}



void CRunLdUldRobot::Set_WorkRbtPicker_UpDown( int nOnOff )
{
	CString strLogKey[10];
	CString	strLogData[10];

	strLogKey[0] = _T("Mode Start");
	strLogData[0].Format(_T("%d"),0);

	m_bPickerDownFlag	= false;
	m_dwWaitPickerUpDown[0]	= GetCurrentTime();

	FAS_IO.set_out_bit( st_io_info.o_LdUldPickUpCyl, nOnOff);
	FAS_IO.set_out_bit( st_io_info.o_LdUldPickDnCyl, !nOnOff);

	///kwlee 2016.0812
	if (nOnOff == IO_ON)
	{
		//LogFunction(_T("LD_ULD_ROBOT"),_T("UP"),_T("PICKER"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("LD_ULD_ROBOT"),_T("UP"),0,_T("PICKER"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
	else
	{
		//LogFunction(_T("LD_ULD_ROBOT"),_T("DOWN"),_T("PICKER"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("LD_ULD_ROBOT"),_T("DOWN"),0,_T("PICKER"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
}

int CRunLdUldRobot::Chk_WorkRbtPicker_UpDown( int nOnOff )
{
	CString strLogKey[10];
	CString	strLogData[10];

	strLogKey[0] = _T("Mode End");
	strLogData[0].Format(_T("%d"),0);

//	800708 1 0 "LOAD_UNLOAD_PICKER_UP_CHECK"
//	800709 1 0 "LOAD_UNLOAD_PICKER_DOWN_CHECK"
	int nWaitTime = WAIT_RBTPICKER_UP_DN;

	if (nOnOff == IO_OFF)
	{
		if (m_bPickerDownFlag == false &&
			FAS_IO.get_in_bit(st_io_info.i_LdUldPickUpChk,	IO_OFF)	== IO_OFF &&
			FAS_IO.get_in_bit(st_io_info.i_LdUldPickDnChk,	IO_ON)	== IO_ON)
		{
			m_bPickerDownFlag		= true;
			m_dwWaitPickerUpDown[0]	= GetCurrentTime();
		}
		else if (m_bPickerDownFlag == true &&
				 FAS_IO.get_in_bit(st_io_info.i_LdUldPickUpChk,	IO_OFF)	== IO_OFF &&
				 FAS_IO.get_in_bit(st_io_info.i_LdUldPickDnChk,	IO_ON)	== IO_ON)
		{
			m_dwWaitPickerUpDown[1] = GetCurrentTime();
			m_dwWaitPickerUpDown[2] = m_dwWaitPickerUpDown[1] - m_dwWaitPickerUpDown[0];

			if (m_dwWaitPickerUpDown[2] <= 0)
			{
				m_dwWaitPickerUpDown[0] = GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwWaitPickerUpDown[2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
			{
				//LogFunction(_T("LD_ULD_ROBOT"),_T("DOWN"),_T("PICKER"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("LD_ULD_ROBOT"),_T("DOWN"),1,_T("PICKER"),_T("CYLINDER"),1,strLogKey,strLogData); 
				return RET_GOOD;
			}
		}
		else
		{
			m_dwWaitPickerUpDown[1] = GetCurrentTime();
			m_dwWaitPickerUpDown[2] = m_dwWaitPickerUpDown[1] - m_dwWaitPickerUpDown[0];

			if (m_dwWaitPickerUpDown[2] <= 0)
			{
				m_dwWaitPickerUpDown[0] = GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwWaitPickerUpDown[2] > (DWORD)st_wait_info.nLimitWaitTime[nWaitTime])
			{
				m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_LdUldPickDnChk); 
				//2017.0109
				clsLog.LogFunction(_T("LD_ULD_ROBOT"),_T("DOWN"),1,_T("PICKER"),_T("CYLINDER"),1,strLogKey,strLogData); 
				return RET_ERROR;
			}
		}
	}
	else
	{
		if (m_bPickerDownFlag == false &&
			FAS_IO.get_in_bit(st_io_info.i_LdUldPickUpChk,	IO_ON)	== IO_ON &&
			FAS_IO.get_in_bit(st_io_info.i_LdUldPickDnChk,	IO_OFF)	== IO_OFF)
		{
			m_bPickerDownFlag			= true;
			m_dwWaitPickerUpDown[0]	= GetCurrentTime();
		}
		else if (m_bPickerDownFlag == true &&
				 FAS_IO.get_in_bit(st_io_info.i_LdUldPickUpChk,	IO_ON)	== IO_ON &&
				 FAS_IO.get_in_bit(st_io_info.i_LdUldPickDnChk,	IO_OFF)	== IO_OFF)
		{
			m_dwWaitPickerUpDown[1]	= GetCurrentTime();
			m_dwWaitPickerUpDown[2]	= m_dwWaitPickerUpDown[1] - m_dwWaitPickerUpDown[0];

			if (m_dwWaitPickerUpDown[2] <= 0)
			{
				m_dwWaitPickerUpDown[0]	= GetCurrentTime();
				return RET_PROCEED;
			}

			if(m_dwWaitPickerUpDown[2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
			{
				//LogFunction(_T("LD_ULD_ROBOT"),_T("UP"),_T("PICKER"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("LD_ULD_ROBOT"),_T("UP"),1,_T("PICKER"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_GOOD;
			}
		}
		else
		{
			m_dwWaitPickerUpDown[1]	= GetCurrentTime();
			m_dwWaitPickerUpDown[2]	= m_dwWaitPickerUpDown[1] - m_dwWaitPickerUpDown[0];

			if (m_dwWaitPickerUpDown[2] <= 0)
			{
				m_dwWaitPickerUpDown[0]	= GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwWaitPickerUpDown[2] > (DWORD)st_wait_info.nLimitWaitTime[nWaitTime])
			{
				//m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_LdUldPickUpChk); 
				//2017.0109
				clsLog.LogFunction(_T("LD_ULD_ROBOT"),_T("UP"),1,_T("PICKER"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_ERROR;
			}
		}
	}

	return RET_PROCEED;
}


void CRunLdUldRobot::Set_WorkRbtRotator_OnOff( int nOnOff )//On:90도 Off:0도
{

	CString strLogKey[10];
	CString	strLogData[10];

	strLogKey[0] = _T("Mode Start");
	strLogData[0].Format(_T("%d"),0);

	m_bRotatorOnOffFlag	= false;
	m_dwWaitRotatorOnOff[0]	= GetCurrentTime();

	FAS_IO.set_out_bit( st_io_info.o_LdUldPickRot_90_Cyl, nOnOff);
	FAS_IO.set_out_bit( st_io_info.o_LdUldPickRot_0_Cyl, !nOnOff);

	///kwlee 2016.0812
	if (nOnOff == IO_ON)
	{
		//LogFunction(_T("LD_ULD_ROBOT"),_T("90º"),_T("ROTATOR"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("LD_ULD_ROBOT"),_T("90º"),0,_T("ROTATOR"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
	else
	{
		//LogFunction(_T("LD_ULD_ROBOT"),_T("0º"),_T("ROTATOR"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("LD_ULD_ROBOT"),_T("0º"),0,_T("ROTATOR"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
}

int CRunLdUldRobot::Chk_WorkRbtRotator_OnOff( int nOnOff )// On : 90도 회전 Off : 0도
{

	CString strLogKey[10];
	CString	strLogData[10];

	strLogKey[0] = _T("Mode End");
	strLogData[0].Format(_T("%d"),0);

//	800710 1 0 "LOAD_UNLOAD_PICKER_ROTATOR_0_CHECK"
//	800711 1 0 "LOAD_UNLOAD_PICKER_ROTATOR_90_CHECK"
	int nWaitTime = WAIT_RBTROTATOR_ON_OFF;

	if (nOnOff == IO_OFF)
	{
		if (m_bRotatorOnOffFlag == false &&
			FAS_IO.get_in_bit(st_io_info.i_LdUldPickRot_90_Chk,	IO_OFF)	== IO_OFF &&
			FAS_IO.get_in_bit(st_io_info.i_LdUldPickRot_0_Chk,	IO_ON)	== IO_ON)
		{
			m_bRotatorOnOffFlag		= true;
			m_dwWaitRotatorOnOff[0]	= GetCurrentTime();
		}
		else if (m_bRotatorOnOffFlag == true &&
				 FAS_IO.get_in_bit(st_io_info.i_LdUldPickRot_90_Chk,	IO_OFF)	== IO_OFF &&
				 FAS_IO.get_in_bit(st_io_info.i_LdUldPickRot_0_Chk,	IO_ON)	== IO_ON)
		{
			m_dwWaitRotatorOnOff[1] = GetCurrentTime();
			m_dwWaitRotatorOnOff[2] = m_dwWaitRotatorOnOff[1] - m_dwWaitRotatorOnOff[0];

			if (m_dwWaitRotatorOnOff[2] <= 0)
			{
				m_dwWaitRotatorOnOff[0] = GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwWaitRotatorOnOff[2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
			{
				//LogFunction(_T("LD_ULD_ROBOT"),_T("0º"),_T("ROTATOR"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
				//2017.0101
				clsLog.LogFunction(_T("LD_ULD_ROBOT"),_T("0º"),1,_T("ROTATOR"),_T("CYLINDER"),1,strLogKey,strLogData); 
				return RET_GOOD;
			}
		}
		else
		{
			m_dwWaitRotatorOnOff[1] = GetCurrentTime();
			m_dwWaitRotatorOnOff[2] = m_dwWaitRotatorOnOff[1] - m_dwWaitRotatorOnOff[0];

			if (m_dwWaitRotatorOnOff[2] <= 0)
			{
				m_dwWaitRotatorOnOff[0] = GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwWaitRotatorOnOff[2] > (DWORD)st_wait_info.nLimitWaitTime[nWaitTime])
			{
				m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_LdUldPickRot_0_Chk); 
				//2017.0101
				clsLog.LogFunction(_T("LD_ULD_ROBOT"),_T("0º"),1,_T("ROTATOR"),_T("CYLINDER"),1,strLogKey,strLogData); 
				return RET_ERROR;
			}
		}
	}
	else
	{
		if (m_bRotatorOnOffFlag == false &&
			FAS_IO.get_in_bit(st_io_info.i_LdUldPickRot_90_Chk,	IO_ON)	== IO_ON &&
			FAS_IO.get_in_bit(st_io_info.i_LdUldPickRot_0_Chk,	IO_OFF)	== IO_OFF)
		{
			m_bRotatorOnOffFlag			= true;
			m_dwWaitRotatorOnOff[0]	= GetCurrentTime();
		}
		else if (m_bRotatorOnOffFlag == true &&
				 FAS_IO.get_in_bit(st_io_info.i_LdUldPickRot_90_Chk,	IO_ON)	== IO_ON &&
				 FAS_IO.get_in_bit(st_io_info.i_LdUldPickRot_0_Chk,	IO_OFF)	== IO_OFF)
		{
			m_dwWaitRotatorOnOff[1]	= GetCurrentTime();
			m_dwWaitRotatorOnOff[2]	= m_dwWaitRotatorOnOff[1] - m_dwWaitRotatorOnOff[0];

			if (m_dwWaitRotatorOnOff[2] <= 0)
			{
				m_dwWaitRotatorOnOff[0]	= GetCurrentTime();
				return RET_PROCEED;
			}

			if(m_dwWaitRotatorOnOff[2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
			{
				//LogFunction(_T("LD_ULD_ROBOT"),_T("90º"),_T("ROTATOR"),_T("CYLINDER"),1,strLogKey,strLogData);//kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("LD_ULD_ROBOT"),_T("90º"),1,_T("ROTATOR"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_GOOD;
			}
		}
		else
		{
			m_dwWaitRotatorOnOff[1]	= GetCurrentTime();
			m_dwWaitRotatorOnOff[2]	= m_dwWaitRotatorOnOff[1] - m_dwWaitRotatorOnOff[0];

			if (m_dwWaitRotatorOnOff[2] <= 0)
			{
				m_dwWaitRotatorOnOff[0]	= GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwWaitRotatorOnOff[2] > (DWORD)st_wait_info.nLimitWaitTime[nWaitTime])
			{
				m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_LdUldPickRot_90_Chk); 
				//2017.0109
				clsLog.LogFunction(_T("LD_ULD_ROBOT"),_T("90º"),1,_T("ROTATOR"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_ERROR;
			}
		}
	}

	return RET_PROCEED;
}


void CRunLdUldRobot::Set_Glipper_OnOff( int nOnOff )
{
	CString strLogKey[10];
	CString	strLogData[10];

	strLogKey[0] = _T("Mode Start");
	strLogData[0].Format(_T("%d"),0);

	m_bGlipperOnOffFlag	= false;
	m_dwGlipperOnOff[0]	= GetCurrentTime();

	FAS_IO.set_out_bit( st_io_info.o_LdUldPickGripCyl, nOnOff);

	//kwlee 2016.0812
	if (nOnOff == IO_ON)
	{
		//LogFunction(_T("LD_ULD_ROBOT"),_T("FORWARD"),_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("LD_ULD_ROBOT"),_T("FORWARD"),0,_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
	else
	{
		//LogFunction(_T("LD_ULD_ROBOT"),_T("BACKWARD"),_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("LD_ULD_ROBOT"),_T("BACKWARD"),0,_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
}

int CRunLdUldRobot::Chk_Glipper_OnOff( int nOnOff )//james 2016.0827 그리퍼 디바이스 체크 정보 수정/보완, Gripper Off(클림프상태)에서 IO_OFF시 디바이스 있는상태, 그외는 ON/OFF 산관없이 모두 디바이스 없는 상태
{
	CString strLogKey[10];
	CString	strLogData[10];

	strLogKey[0] = _T("Mode End");
	strLogData[0].Format(_T("%d"),0);

//	800712 1 0 "LOAD_UNLOAD_PICKER_GLIPPER_DVC_EXIST_CHECK"
	int nWaitTime = WAIT_RBTGLIPPER_ON_OFF;

	if (nOnOff == IO_OFF)
	{
		if (m_bGlipperOnOffFlag == false )//&&	FAS_IO.get_in_bit(st_io_info.i_LdUldPickDvcChk,	IO_OFF)	== IO_OFF )
		{
			m_bGlipperOnOffFlag		= true;
			m_dwGlipperOnOff[0]	= GetCurrentTime();
		}
		else if (m_bGlipperOnOffFlag == true)// &&	 FAS_IO.get_in_bit(st_io_info.i_LdUldPickDvcChk, IO_OFF) == IO_OFF )
		{
			m_dwGlipperOnOff[1] = GetCurrentTime();
			m_dwGlipperOnOff[2] = m_dwGlipperOnOff[1] - m_dwGlipperOnOff[0];

			if (m_dwGlipperOnOff[2] <= 0)
			{
				m_dwGlipperOnOff[0] = GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwGlipperOnOff[2] > (DWORD)st_wait_info.nOffWaitTime[nWaitTime])
			{
				//LogFunction(_T("LD_ULD_ROBOT"),_T("BACKWARD"),_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("LD_ULD_ROBOT"),_T("BACKWARD"),1,_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_GOOD;
			}
		}
		else
		{
			m_dwGlipperOnOff[1] = GetCurrentTime();
			m_dwGlipperOnOff[2] = m_dwGlipperOnOff[1] - m_dwGlipperOnOff[0];

			if (m_dwGlipperOnOff[2] <= 0)
			{
				m_dwGlipperOnOff[0] = GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwGlipperOnOff[2] > (DWORD)st_wait_info.nLimitWaitTime[nWaitTime])
			{
				m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_LdUldPickDvcChk); 
				//2017.0109
				clsLog.LogFunction(_T("LD_ULD_ROBOT"),_T("BACKWARD"),1,_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_ERROR;
			}
		}
	}
	else
	{
		if (m_bGlipperOnOffFlag == false)// &&	FAS_IO.get_in_bit(st_io_info.i_LdUldPickDvcChk,	IO_ON)	== IO_ON )
		{
			m_bGlipperOnOffFlag			= true;
			m_dwGlipperOnOff[0]	= GetCurrentTime();
		}
		else if (m_bGlipperOnOffFlag == true) //&&		 FAS_IO.get_in_bit(st_io_info.i_LdUldPickDvcChk, IO_ON)	== IO_ON )
		{
			m_dwGlipperOnOff[1]	= GetCurrentTime();
			m_dwGlipperOnOff[2]	= m_dwGlipperOnOff[1] - m_dwGlipperOnOff[0];

			if (m_dwGlipperOnOff[2] <= 0)
			{
				m_dwGlipperOnOff[0]	= GetCurrentTime();
				return RET_PROCEED;
			}

			if(m_dwGlipperOnOff[2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
			{
				//LogFunction(_T("LD_ULD_ROBOT"),_T("FORWARD"),_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("LD_ULD_ROBOT"),_T("FORWARD"),1,_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_GOOD;
			}
		}
		else
		{
			m_dwGlipperOnOff[1]	= GetCurrentTime();
			m_dwGlipperOnOff[2]	= m_dwGlipperOnOff[1] - m_dwGlipperOnOff[0];

			if (m_dwGlipperOnOff[2] <= 0)
			{
				m_dwGlipperOnOff[0]	= GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwGlipperOnOff[2] > (DWORD)st_wait_info.nLimitWaitTime[nWaitTime])
			{
				m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_LdUldPickDvcChk); 
				//2017.0109
				clsLog.LogFunction(_T("LD_ULD_ROBOT"),_T("FORWARD"),1,_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_ERROR;
			}
		}
	}

	return RET_PROCEED;
}


int CRunLdUldRobot::Process_Barcode1D(int nWork_Site, int nX, int nY)
{
	int nRet[5];
	int nCount;
	BOOL bBarcodeFlag = FALSE;

	clsFunc.OnThreadFunctionStepTrace(32, m_nBarcode1dStep);
	switch (m_nBarcode1dStep)
	{
		case 0:
			// 1D 바코드 읽기 모드이면 
			m_nRetry = 0;
			

			m_nBarcode1dStep = 100;
			break;

		case 100:
			// work robot z축 상태확인 
			m_dCurrentPos[m_nRobot_Z] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // 모터 특정 축의 현재 위치 리턴 함수
			if(m_dCurrentPos[m_nRobot_Z] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벗어나 있으면 저위치 시킨다
			{
				// work robot z축 안전위치에 있지 않으면 안전위치로 이동 
				m_nBarcode1dStep = 200;
			}
			else
			{
				// work robot z축 안전위치에 있으면 
				m_nBarcode1dStep = 1000;
			}
			break;

		case 200:
			nRet[0] = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY], COMI.mn_runspeed_rate);
			if (nRet[0] == BD_GOOD) //좌측으로 이동
			{
				m_nBarcode1dStep = 1000;
			}
			else if (nRet[0] == BD_RETRY)
			{
				m_nBarcode1dStep = 200;
			}
			else if (nRet[0] == BD_ERROR || nRet[0] == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
				CTL_Lib.Alarm_Error_Occurrence(1300, dWARNING, st_alarm_info.strCode);

				m_nBarcode1dStep = 200;
			}
			break;

		case 1000:
			m_dCurrentPos[2] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // 모터 특정 축의 현재 위치 리턴 함수
			if(m_dCurrentPos[2] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벅어나 있으면 저위치 시킨다
			{
				m_nBarcode1dStep = 200;

				break;
			}

			// plate 위치에 tray 1D Barcode 위치로 이동 
			//st_recipe_info.dLdUldBufferPitch 2016.1006
			//kwlee 2016.1007
// 			m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_WORKROBOT_BCR_1D_START] - ((double)nY * st_recipe_info.dLdUldBufferPitch); //st_recipe_info.dTrayPitch_Y);
// 			m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_WORKROBOT_BCR_1D_START];

// 			m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_WORKROBOT_BCR_1D_START_LD] - ((double)nY * st_recipe_info.dLdUldBufferPitch); //st_recipe_info.dTrayPitch_Y);
// 			m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_WORKROBOT_BCR_1D_START_LD];
			
			//2016.1023
			if(m_nRotator_Flag == 0) //
			{
				m_nRotator_Flag = 1;
				Set_WorkRbtRotator_OnOff(IO_OFF); //rotator 돌리면서 이동한다
			}

			//2016.1009
			if (nWork_Site == THD_ULD_BUFF)
			{
				m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_WORKROBOT_BCR_1D_START_UNLD] - ((double)nY * st_recipe_info.dLdUldBufferPitch); //st_recipe_info.dTrayPitch_Y);
				m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_WORKROBOT_BCR_1D_START_UNLD];
			}
			else// if(THD_LD_BUFF)
			{
				m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_WORKROBOT_BCR_1D_START_LD] - ((double)nY * st_recipe_info.dLdUldBufferPitch); //st_recipe_info.dTrayPitch_Y);
				m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_WORKROBOT_BCR_1D_START_LD];
			}

			nRet[0] = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate); 

			if (nRet[0] == BD_GOOD) //좌측으로 이동
			{
				m_nRotator_Flag = 0;
				m_nBarcode1dStep = 1100;
			}
			else if (nRet[0] == BD_RETRY)
			{
				m_nBarcode1dStep = 1000;
			}
			else if (nRet[0] == BD_ERROR || nRet[0] == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
				CTL_Lib.Alarm_Error_Occurrence(1301, dWARNING, st_alarm_info.strCode);

				m_nBarcode1dStep = 1000;
			}
			break;

		case 1100:
			nRet[1] = Chk_WorkRbtRotator_OnOff(IO_OFF); 
			if(nRet[1] == RET_GOOD)
			{
				// work robot z축 barcode 읽기 위치로 이동 
				//kwlee 2016.1007
				//m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_1D_POS]; 
				//m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_1D_LD_POS]; 
				//2016.1009
				if (nWork_Site == THD_ULD_BUFF)
				{
					m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_1D_UNLD_POS];
				}
				else
				{
					m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_1D_LD_POS]; 
				}

				nRet[0] = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, m_dpTargetPosList[2], COMI.mn_runspeed_rate);

				if (nRet[0] == BD_GOOD) //좌측으로 이동
				{ 						
					m_nBarcode1dStep = 1200;
				}
				else if (nRet[0] == BD_RETRY)
				{
					m_nBarcode1dStep = 1100;
				}
				else if (nRet[0] == BD_ERROR || nRet[0] == BD_SAFETY)
				{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
					CTL_Lib.Alarm_Error_Occurrence(1302, dWARNING, st_alarm_info.strCode);

					m_nBarcode1dStep = 1100;
				}		
			}
			else if(nRet[1] == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(1303, dWARNING, m_strAlarmCode);
				m_nBarcode1dStep = 1000;
			}
			break;

		case 1200:
			if (st_basic_info.nMode1DBcr == NO)
			{
				// 1D 바코드 읽기 모드가 아니면 
				m_nBarcode1dStep = 0;

				return RET_GOOD;
			}

			// 바코드 읽기 요청 
			clsBcr1D.m_nCommand = BD_NONE;
			::SendMessage(st_handler_info.hWnd, WM_BARCODE_MSG_1, BARCODE_TRIGGER_1, 0);

			m_nBarcode1dStep = 1300;
			break;

		case 1300:
			if (clsBcr1D.m_nCommand == BD_END)
			{
				// 바코드가 정상적으로 읽었으면 
				nCount = 0;
				//kwlee 2016.1114
				//if (clsBcr1D.m_strBarcode[0] != _T("") && clsBcr1D.m_strBarcode[0] != _T("ERROR"))
				if (clsBcr1D.m_strBarcode[0] == _T("") || clsBcr1D.m_strBarcode[0] == _T("ERROR"))
				{
					// 바코드 데이터가 null/error이면 
					nCount++;
				}
				//kwlee 2016.1114
				//if (clsBcr1D.m_strBarcode[1] != _T("") && clsBcr1D.m_strBarcode[1] != _T("ERROR"))
				if (clsBcr1D.m_strBarcode[1] == _T("") || clsBcr1D.m_strBarcode[1] == _T("ERROR"))
				{
					// 바코드 데이터가 null/error이면 
					nCount++;
				}
/*
				if (st_recipe_info.nBarcodeCnt == 2)
				{
					// 제품에 바코드가 2개 있으면  
					if (clsBcr1D.m_strBarcode[1] == _T("") || clsBcr1D.m_strBarcode[1] == _T("ERROR"))
					{
						// 바코드 데이터가 null/error이면 
						if (!bBarcodeFlag) bBarcodeFlag = TRUE;
					}
				}
*/
				//kwlee 2016.1114
				//if (st_recipe_info.nBarcodeCnt != nCount)
				if (nCount > 0)
				{
					// barcode가 에러이면 retry 여부 확인
					m_nRetry++;
					if (m_nRetry > 2)
					{
						m_nRetry = 0;
						// 나중에 알람코드 추가
						m_nBarcode1dStep = 1400;
						break;
					}
					m_nBarcode1dStep = 1200;

					break;
				}

//				st_tray_info[nWork_Site].st_pcb_info[nY][nX].strBarcode1D[0] = clsBcr1D.m_strBarcode[0];
//				st_tray_info[nWork_Site].st_pcb_info[nY][nX].strBarcode1D[1] = clsBcr1D.m_strBarcode[1];
				//2016.1006
//  				st_buffer_info[nWork_Site].st_pcb_info[nY].strBarcode1D[0] = clsBcr1D.m_strBarcode[0];
// 				st_buffer_info[nWork_Site].st_pcb_info[nY].strBarcode1D[1] = clsBcr1D.m_strBarcode[1];
				
				if (nWork_Site == THD_ULD_BUFF)
				{
					if( st_buffer_info[nWork_Site].st_pcb_info[nY].strBarcode1D[0] != clsBcr1D.m_strBarcode[0] ||
						st_buffer_info[nWork_Site].st_pcb_info[nY].strBarcode1D[1] != clsBcr1D.m_strBarcode[1])
					{
						st_other_info.strBoxMsg.Format(_T("uldbuf: IBM 1D_1[%s] 1D_2[%s], read: IBM 1D_1[%s] 1D_2[%s] is not same. Reject"), 
							st_buffer_info[nWork_Site].st_pcb_info[nY].strBarcode1D[0],	st_buffer_info[nWork_Site].st_pcb_info[nY].strBarcode1D[1],
							clsBcr1D.m_strBarcode[0], clsBcr1D.m_strBarcode[1]);
						clsMem.OnAbNormalMessagWrite(st_other_info.strBoxMsg);
						if (st_handler_info.cWndMain != NULL)
						{
							st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
						}
						st_buffer_info[nWork_Site].st_pcb_info[nY].strBarcode1D[0] = clsBcr1D.m_strBarcode[0];
						st_buffer_info[nWork_Site].st_pcb_info[nY].strBarcode1D[1] = clsBcr1D.m_strBarcode[1];
						st_buffer_info[nWork_Site].st_pcb_info[nY].nBin = BD_DATA_REJECT;
						//clsFunc.OnMCStop();
						//900101 1 0 "BARCODE_IS_NOT_SAME."
						CTL_Lib.Alarm_Error_Occurrence(1501, dWARNING, _T("900101"));
					}
					else
					{
						st_buffer_info[nWork_Site].st_pcb_info[nY].strBarcode1D[0] = clsBcr1D.m_strBarcode[0];
						st_buffer_info[nWork_Site].st_pcb_info[nY].strBarcode1D[1] = clsBcr1D.m_strBarcode[1];
					}
					//kwlee 2016.1114
					m_nBarcode1dStep = 2000;
				}
				else
				{
					st_buffer_info[nWork_Site].st_pcb_info[nY].strBarcode1D[0] = clsBcr1D.m_strBarcode[0];
					st_buffer_info[nWork_Site].st_pcb_info[nY].strBarcode1D[1] = clsBcr1D.m_strBarcode[1];
					//2016.1111
					if( st_buffer_info[nWork_Site].st_pcb_info[nY].strBarcode1D[0] == st_buffer_info[nWork_Site].st_pcb_info[nY].strBarcode2D[0])
					{						
						m_nBarcode1dStep = 2000;
					}
					else
					{//2016.1111
						if( mn_BcrFirstChk == CTL_NO)
						{
							m_nBarcode1dStep = 2000;
							break;
						}

						//2016.1111
 						st_other_info.strBoxMsg.Format(_T("IBM 1D[%s] 2D[%s]"), st_buffer_info[nWork_Site].st_pcb_info[nY].strBarcode1D[0],
 																				st_buffer_info[nWork_Site].st_pcb_info[nY].strBarcode2D[0]);
						if (st_handler_info.cWndMain != NULL)
						{
							st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
						}

						//clsFunc.OnMCStop();
						//900100 1 0 "BARCODE_READ_ERROR."
						CTL_Lib.Alarm_Error_Occurrence(1503, dWARNING, _T("900100"));

						m_nBarcode1dStep = 2000;
					}
				}
			}
			break;

		case 1400:
			// 바코드 읽기 불량으로 확인 요청
//			clsFunc.OnMCStop();

			st_other_info.nConfirmMsg = -1;
			if (st_handler_info.cWndMain != NULL)
			{
				st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_BARCODE_BOX_CREATE_REQ, 0);
			}
			m_dwBarcodeWaitTime[0] = GetTickCount();

			m_nBarcode1dStep = 1500;
			break;

		case 1500:
			if (st_other_info.nConfirmMsg == 0)
			{
				bBarcodeFlag = FALSE;
				// barcode retry 상태가 아니면
				if (clsBcr1D.m_strBarcode[0] == _T("") || clsBcr1D.m_strBarcode[0] == _T("ERROR"))
				{
					// 바코드 데이터가 null/error이면 
					bBarcodeFlag = TRUE;
				}

				if (st_recipe_info.nBarcodeCnt == 2)
				{
					// 제품에 바코드가 2개 있으면  
					if (clsBcr1D.m_strBarcode[1] == _T("") || clsBcr1D.m_strBarcode[1] == _T("ERROR"))
					{
						// 바코드 데이터가 null/error이면 
						if (!bBarcodeFlag) bBarcodeFlag = TRUE;
					}
				}

				if (bBarcodeFlag)
				{

					m_nBarcode1dStep = 1400;
					//900100 1 0 "BARCODE_READ_ERROR."
					CTL_Lib.Alarm_Error_Occurrence(1513, dWARNING, _T("900100"));
				}
				else
				{
					// manual 입력 상태확인
 					//st_tray_info[nWork_Site].st_pcb_info[nY][nX].strBarcode1D[0] = clsBcr1D.m_strBarcode[0];
 					//st_tray_info[nWork_Site].st_pcb_info[nY][nX].strBarcode1D[1] = clsBcr1D.m_strBarcode[1];
					//2016.1006
					st_buffer_info[nWork_Site].st_pcb_info[nY].strBarcode1D[0] = clsBcr1D.m_strBarcode[0];
					st_buffer_info[nWork_Site].st_pcb_info[nY].strBarcode1D[1] = clsBcr1D.m_strBarcode[1];

					m_nBarcode1dStep = 2000;
				}
			}
			else if (st_other_info.nConfirmMsg == 1)
			{
				// barcode retry 요청이면 
				m_nBarcode1dStep = 1200;
			}
			else
			{
				m_dwBarcodeWaitTime[1] = GetTickCount();
				m_dwBarcodeWaitTime[2] = m_dwBarcodeWaitTime[1] - m_dwBarcodeWaitTime[0];

				if (m_dwBarcodeWaitTime[2] <= (DWORD)0)
				{
					m_dwBarcodeWaitTime[0] = GetTickCount();
					break;
				}

				if (m_dwBarcodeWaitTime[2] > (DWORD)120000)
				{
					m_nBarcode1dStep = 1400;
				}
			}
			break;

		case 2000:
			// work robot z축 상태확인 
			m_dCurrentPos[m_nRobot_Z] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // 모터 특정 축의 현재 위치 리턴 함수
			if(m_dCurrentPos[m_nRobot_Z] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벗어나 있으면 저위치 시킨다
			{
				// work robot z축 안전위치에 있지 않으면 안전위치로 이동 
				m_nBarcode1dStep = 2100;
			}
			else
			{
				// work robot z축 안전위치에 있으면 
				m_nBarcode1dStep = 0;

				return RET_GOOD;
			}
			break;

		case 2100:
			nRet[0] = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY], COMI.mn_runspeed_rate);
			if (nRet[0] == BD_GOOD) //좌측으로 이동
			{
				m_nBarcode1dStep = 0;

				return RET_GOOD;
			}
			else if (nRet[0] == BD_RETRY)
			{
				m_nBarcode1dStep = 2100;
			}
			else if (nRet[0] == BD_ERROR || nRet[0] == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
				CTL_Lib.Alarm_Error_Occurrence(1304, dWARNING, st_alarm_info.strCode);

				m_nBarcode1dStep = 2100;
			}
			break;
	}

	return RET_PROCEED;
}


int CRunLdUldRobot::Process_Barcode2D(int nWork_site, int nX, int nY)
{
	int nRet[5];
		
	clsFunc.OnThreadFunctionStepTrace(33, m_nBarcode2dStep);
	switch (m_nBarcode2dStep)
	{
		case 0:
			// 1D 바코드 읽기 모드이면 
			m_nRetry = 0;

			m_nBarcode2dStep = 100;
			break;

		case 100:
			// work robot z축 상태확인 
			m_dCurrentPos[m_nRobot_Z] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // 모터 특정 축의 현재 위치 리턴 함수
			if(m_dCurrentPos[m_nRobot_Z] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벗어나 있으면 저위치 시킨다
			{
				// work robot z축 안전위치에 있지 않으면 안전위치로 이동 
				m_nBarcode2dStep = 200;
			}
			else
			{
				// work robot z축 안전위치에 있으면 
				m_nBarcode2dStep = 1000;
			}
			break;

		case 200:
			nRet[0] = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY], COMI.mn_runspeed_rate);
			if (nRet[0] == BD_GOOD) //좌측으로 이동
			{
				m_nBarcode2dStep = 1000;
			}
			else if (nRet[0] == BD_RETRY)
			{
				m_nBarcode2dStep = 200;
			}
			else if (nRet[0] == BD_ERROR || nRet[0] == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
				CTL_Lib.Alarm_Error_Occurrence(1400, dWARNING, st_alarm_info.strCode);

				m_nBarcode2dStep = 200;
			}
			break;

		case 1000:
			m_dCurrentPos[2] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // 모터 특정 축의 현재 위치 리턴 함수
			if(m_dCurrentPos[2] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벅어나 있으면 저위치 시킨다
			{
				m_nBarcode2dStep = 200;

				break;
			}

// 			if(m_nRotator_Flag == 0) //
// 			{
// 				m_nRotator_Flag = 1;
// 				Set_WorkRbtRotator_OnOff(IO_OFF); //rotator 돌리면서 이동한다
// 			}


			// plate 위치에 tray 1D Barcode 위치로 이동  
			if (nWork_site == THD_LD_BUFF)
			{
				m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_WORKROBOT_BCR_2D_START_LD] - ((double)nY * st_recipe_info.dLdUldBufferPitch);
				m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_WORKROBOT_BCR_2D_START_LD];
			}
			else
			{
				m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_WORKROBOT_BCR_2D_START_UNLD] - ((double)nY * st_recipe_info.dLdUldBufferPitch);
				m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_WORKROBOT_BCR_2D_START_UNLD];
			}


			nRet[0] = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate); 

			if (nRet[0] == BD_GOOD) //좌측으로 이동
			{
				m_nBarcode2dStep = 1100;
			}
			else if (nRet[0] == BD_RETRY)
			{
				m_nBarcode2dStep = 1000;
			}
			else if (nRet[0] == BD_ERROR || nRet[0] == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
				CTL_Lib.Alarm_Error_Occurrence(1401, dWARNING, st_alarm_info.strCode);

				m_nBarcode2dStep = 1000;
			}
			break;

		case 1100:
			// work robot z축 barcode 읽기 위치로 이동 
			if (nWork_site == THD_LD_BUFF)
			{
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_2D_LD_POS]; 
			}
			else
			{
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_2D_UNLD_POS]; 
			}

			nRet[0] = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, m_dpTargetPosList[2], COMI.mn_runspeed_rate);

			if (nRet[0] == BD_GOOD) //좌측으로 이동
			{ 						
				m_nBarcode2dStep = 1200;
			}
			else if (nRet[0] == BD_RETRY)
			{
				m_nBarcode2dStep = 1100;
			}
			else if (nRet[0] == BD_ERROR || nRet[0] == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
				CTL_Lib.Alarm_Error_Occurrence(1402, dWARNING, st_alarm_info.strCode);

				m_nBarcode2dStep = 1100;
			}		
			break;

		case 1200:
			if (st_basic_info.nMode2DBcr == NO)
			{
				// 1D 바코드 읽기 모드가 아니면 
				m_nBarcode2dStep = 0;

				return RET_GOOD;
			}

			// 바코드 읽기 요청 
			clsBcr2D.m_nCommand = BD_NONE;
			::SendMessage(st_handler_info.hWnd, WM_BARCODE_MSG_2, BARCODE_TRIGGER_1, 1);

			m_nBarcode2dStep = 1300;
			break;

		case 1300:
			if (clsBcr2D.m_nCommand == BD_END)
			{
				// 바코드가 정상적으로 읽었으면 
				if ( clsBcr2D.m_strBarcode[0] == _T("") || clsBcr2D.m_strBarcode[0] == _T("ERROR"))
				{
					// 바코드 데이터가 null/error이면 
					m_nRetry++;
					if (m_nRetry > 2)
					{
						m_nRetry = 0;
						//2016.1111
						if( mn_BcrFirst == CTL_YES )
						{
							mn_BcrFirstChk = CTL_NO;
							m_nBarcode2dStep = 2000;
							break;
						}
						// 나중에 알람코드 추가
						m_nBarcode2dStep = 1400;
						break;
					}
					m_nBarcode2dStep = 1200;

					break;
				}

				// jtkim 20160929

				//st_picker[m_nRobotSite].st_pcb_info[i]
				if (nWork_site == THD_ULD_BUFF)
				{
					st_buffer_info[nWork_site].st_pcb_info[nY].strBarcode2D[0] = clsBcr2D.m_strBarcode[0];
					st_buffer_info[nWork_site].st_pcb_info[nY].strBarcode2D[1] = clsBcr2D.m_strBarcode[1];

					if (st_buffer_info[nWork_site].st_pcb_info[nY].strBarcode1D[0] == st_buffer_info[nWork_site].st_pcb_info[nY].strBarcode2D[0])
					{
						m_nBarcode2dStep = 2000;
					}
					else
					{
						st_other_info.strBoxMsg.Format(_T("IBM 1D[%s] 2D[%s] is not same. Reject"), st_buffer_info[nWork_site].st_pcb_info[nY].strBarcode1D[0],
							st_buffer_info[nWork_site].st_pcb_info[nY].strBarcode2D[0]);
						clsMem.OnAbNormalMessagWrite(st_other_info.strBoxMsg); //kwlee 2016.1219
						if (st_handler_info.cWndMain != NULL)
						{
							st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
						}

						if (nWork_site == THD_ULD_BUFF)
						{
							st_buffer_info[nWork_site].st_pcb_info[nY].nBin = BD_DATA_REJECT;
						}
						//clsFunc.OnMCStop();
						//900101 1 0 "BARCODE_IS_NOT_SAME."
						CTL_Lib.Alarm_Error_Occurrence(1502, dWARNING, _T("900101"));


						m_nBarcode2dStep = 2000;
					}
				}
				else
				{
					//st_picker[m_nRobotSite].st_pcb_info[0]
					//st_picker[m_nRobotSite].st_pcb_info[0].strBarcode2D[0] = clsBcr2D.m_strBarcode[0];
					//st_picker[m_nRobotSite].st_pcb_info[0].strBarcode2D[1] = clsBcr2D.m_strBarcode[1];
					//20161006
					st_buffer_info[nWork_site].st_pcb_info[nY].strBarcode2D[0] = clsBcr2D.m_strBarcode[0];
					st_buffer_info[nWork_site].st_pcb_info[nY].strBarcode2D[1] = clsBcr2D.m_strBarcode[1];

					//if (st_picker[m_nRobotSite].st_pcb_info[0].strBarcode1D[0] == st_picker[m_nRobotSite].st_pcb_info[0].strBarcode2D[0])
					//2016.1006
					//2016.1111
					if( mn_BcrFirst == CTL_YES )
					{
						mn_BcrFirstChk = CTL_YES;
						m_nBarcode2dStep = 2000;
					}
					else
					{
						mn_BcrFirstChk = CTL_YES;
						if( st_buffer_info[nWork_site].st_pcb_info[nY].strBarcode1D[0] == st_buffer_info[nWork_site].st_pcb_info[nY].strBarcode2D[0])
						{
							m_nBarcode2dStep = 2000;
						}
						else
						{
							//st_other_info.strBoxMsg.Format(_T("IBM 1D[%s] 2D[%s]"), st_picker[m_nRobotSite].st_pcb_info[0].strBarcode1D[0],
							//														st_picker[m_nRobotSite].st_pcb_info[0].strBarcode2D[0]);
							//2016.1006
 							st_other_info.strBoxMsg.Format(_T("IBM 1D[%s] 2D[%s]"), st_buffer_info[nWork_site].st_pcb_info[nY].strBarcode1D[0],
 																					st_buffer_info[nWork_site].st_pcb_info[nY].strBarcode2D[0]);
							if (st_handler_info.cWndMain != NULL)
							{
								st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
							}

							if (nWork_site == THD_ULD_BUFF)
							{
								st_buffer_info[nWork_site].st_pcb_info[nY].nBin = BD_DATA_REJECT;
							}
							//clsFunc.OnMCStop();
							//900100 1 0 "BARCODE_READ_ERROR."
							CTL_Lib.Alarm_Error_Occurrence(1523, dWARNING, _T("900100"));

							m_nBarcode2dStep = 2000;
						}
					}
				}
/*				st_buffer_info[nWork_site].st_pcb_info[nY].strBarcode2D[0] = clsBcr2D.m_strBarcode[0];
				st_buffer_info[nWork_site].st_pcb_info[nY].strBarcode2D[1] = clsBcr2D.m_strBarcode[1];

				if (st_buffer_info[nWork_site].st_pcb_info[nY].strBarcode1D[0] == st_buffer_info[nWork_site].st_pcb_info[nY].strBarcode2D[0])
				{
					m_nBarcode2dStep = 2000;
				}
				else
				{
					st_other_info.strBoxMsg.Format(_T("IBM 1D[%s] 2D[%s]"), st_buffer_info[nWork_site].st_pcb_info[nY].strBarcode1D[0],
																			st_buffer_info[nWork_site].st_pcb_info[nY].strBarcode2D[0]);
					if (st_handler_info.cWndMain != NULL)
					{
						st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
					}

					if (nWork_site == THD_ULD_BUFF)
					{
						st_buffer_info[nWork_site].st_pcb_info[nY].nBin = BD_DATA_REJECT;
					}
					clsFunc.OnMCStop();

					
					m_nBarcode2dStep = 2000;
				}*/
			}
			break;

		case 1400:
			// 바코드 읽기 불량으로 확인 요청
			//clsFunc.OnMCStop();
			//2016.1024
			if (nWork_site == THD_LD_BUFF)
			{
				m_nRetry = 0;
				m_nRetry++;
				m_nBarcode2dStep = 1450;
			}
			else
			{
				st_other_info.nConfirmMsg = -1;
				if (st_handler_info.cWndMain != NULL)
				{
					st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_BARCODE_BOX_CREATE_REQ, (nWork_site - THD_LD_BUFF) + 1);
				}
				m_dwBarcodeWaitTime[0] = GetTickCount();


				m_nBarcode2dStep = 1500;
			}
			break;

		case 1410:
			if (st_basic_info.nMode2DBcr == NO)
			{
				// 1D 바코드 읽기 모드가 아니면 
				m_nBarcode2dStep = 0;

				return RET_GOOD;
			}
			// 바코드 읽기 요청 
			clsBcr2D.m_nCommand = BD_NONE;
			::SendMessage(st_handler_info.hWnd, WM_BARCODE_MSG_2, BARCODE_TRIGGER_1, 1);

			m_nBarcode2dStep = 1420;
			break;

		case 1420:
			if (clsBcr2D.m_nCommand == BD_END)
			{
				// 바코드가 정상적으로 읽었으면 
				if ( clsBcr2D.m_strBarcode[0] == _T("") || clsBcr2D.m_strBarcode[0] == _T("ERROR"))
				{
					// 바코드 데이터가 null/error이면 
					m_nRetry++;
					if (m_nRetry > 2)
					{
						m_nRetry = 0;

						st_other_info.nConfirmMsg = -1;
						if (st_handler_info.cWndMain != NULL)
						{
							st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_BARCODE_BOX_CREATE_REQ, (nWork_site - THD_LD_BUFF) + 1);
						}
						m_dwBarcodeWaitTime[0] = GetTickCount();

						m_nBarcode2dStep = 1500;
						
						break;
					}
					m_nBarcode2dStep = 1450;
					break;
				}
				else
				{
					m_nBarcode2dStep = 1300;
				}
			}
			break;

		case 1450:
			if( m_nRetry <= 1) nRet[3] = 1;
			else               nRet[3] = -1;
			m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_WORKROBOT_BCR_2D_START_LD] - ((double)nY * st_recipe_info.dLdUldBufferPitch) - (nRet[3]*m_nRetry*2);
			m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_WORKROBOT_BCR_2D_START_LD];

			nRet[0] = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate); 

			if (nRet[0] == BD_GOOD) //좌측으로 이동
			{
				m_nBarcode2dStep = 1410;
			}
			else if (nRet[0] == BD_RETRY)
			{
				m_nBarcode2dStep = 1450;
			}
			else if (nRet[0] == BD_ERROR || nRet[0] == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
				CTL_Lib.Alarm_Error_Occurrence(1600, dWARNING, st_alarm_info.strCode);

				m_nBarcode2dStep = 1450;
			}
			break;

		case 1500:
			if (st_other_info.nConfirmMsg == 0)
			{
				// barcode retry 상태가 아니면
				// manual 입력 상태확인
				if (clsBcr2D.m_strBarcode[1] == _T("") || clsBcr2D.m_strBarcode[1] == _T("ERROR"))
				{
					// 바코드 데이터가 null/error이면 

					//900100 1 0 "BARCODE_READ_ERROR."
					CTL_Lib.Alarm_Error_Occurrence(1633, dWARNING, _T("900100"));
					m_nBarcode2dStep = 1400;
				}
				else
				{
					// manual 입력 상태확인
					// jtkim 20160929
					if (nWork_site == THD_ULD_BUFF)
					{
						st_buffer_info[nWork_site].st_pcb_info[nY].strBarcode2D[0] = clsBcr2D.m_strBarcode[1];
						//st_buffer_info[nWork_site].st_pcb_info[nY].strBarcode2D[1] = clsBcr2D.m_strBarcode[1];

						if (st_buffer_info[nWork_site].st_pcb_info[nY].strBarcode1D[0] == st_buffer_info[nWork_site].st_pcb_info[nY].strBarcode2D[0])
						{
							m_nBarcode2dStep = 2000;
						}
						else
						{
							st_other_info.strBoxMsg.Format(_T("IBM 1D[%s] 2D[%s]"), st_buffer_info[nWork_site].st_pcb_info[nY].strBarcode1D[0],
								st_buffer_info[nWork_site].st_pcb_info[nY].strBarcode2D[0]);
							if (st_handler_info.cWndMain != NULL)
							{
								st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
							}

							if (nWork_site == THD_ULD_BUFF)
							{
								st_buffer_info[nWork_site].st_pcb_info[nY].nBin = BD_DATA_REJECT;
							}
							//clsFunc.OnMCStop();
							//900100 1 0 "BARCODE_READ_ERROR."
							CTL_Lib.Alarm_Error_Occurrence(1544, dWARNING, _T("900100"));



							m_nBarcode2dStep = 2000;
						}
					}
					else
					{
// 						st_picker[m_nRobotSite].st_pcb_info[0].strBarcode2D[0] = clsBcr2D.m_strBarcode[1];
// 						if (st_picker[m_nRobotSite].st_pcb_info[0].strBarcode1D[0] == st_picker[m_nRobotSite].st_pcb_info[0].strBarcode2D[0])
// 						{
// 							m_nBarcode2dStep = 2000;
// 						}
// 						else
// 						{
// 							st_other_info.strBoxMsg.Format(_T("IBM 1D[%s] 2D[%s]"), st_picker[m_nRobotSite].st_pcb_info[0].strBarcode1D[0],
// 								st_picker[m_nRobotSite].st_pcb_info[0].strBarcode2D[0]);
// 							if (st_handler_info.cWndMain != NULL)
// 							{
// 								st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
// 							}
// 
// 							if (nWork_site == THD_ULD_BUFF)
// 							{
// 								st_buffer_info[nWork_site].st_pcb_info[nY].nBin = BD_DATA_REJECT;
// 							}
// 							clsFunc.OnMCStop();
// 
// 							m_nBarcode2dStep = 2000;
// 						}
						//2016.1015
						st_buffer_info[nWork_site].st_pcb_info[nY].strBarcode2D[0] = clsBcr2D.m_strBarcode[1];
						if( st_buffer_info[nWork_site].st_pcb_info[nY].strBarcode1D[0] == st_buffer_info[nWork_site].st_pcb_info[nY].strBarcode2D[0])
						{
							m_nBarcode2dStep = 2000;
						}
						else
						{
							st_other_info.strBoxMsg.Format(_T("IBM 1D[%s] 2D[%s]"), st_buffer_info[nWork_site].st_pcb_info[nY].strBarcode1D[0],
								st_buffer_info[nWork_site].st_pcb_info[nY].strBarcode2D[0]);
							if (st_handler_info.cWndMain != NULL)
							{
								st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
							}

							if (nWork_site == THD_ULD_BUFF)
							{
								st_buffer_info[nWork_site].st_pcb_info[nY].nBin = BD_DATA_REJECT;
							}
							//clsFunc.OnMCStop();
							//900100 1 0 "BARCODE_READ_ERROR."
							CTL_Lib.Alarm_Error_Occurrence(1555, dWARNING, _T("900100"));


							m_nBarcode2dStep = 2000;
						}
					}
				}
			}
			else if (st_other_info.nConfirmMsg == 1)
			{
				// barcode retry 요청이면 
				m_nBarcode2dStep = 1200;
			}
			else
			{
				m_dwBarcodeWaitTime[1] = GetTickCount();
				m_dwBarcodeWaitTime[2] = m_dwBarcodeWaitTime[1] - m_dwBarcodeWaitTime[0];

				if (m_dwBarcodeWaitTime[2] <= (DWORD)0)
				{
					m_dwBarcodeWaitTime[0] = GetTickCount();
					break;
				}

				if (m_dwBarcodeWaitTime[2] > (DWORD)120000)
				{
					m_nBarcode2dStep = 1400;
				}
			}
			break;

		case 2000:
			// work robot z축 상태확인 
			m_dCurrentPos[m_nRobot_Z] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // 모터 특정 축의 현재 위치 리턴 함수
			if(m_dCurrentPos[m_nRobot_Z] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벗어나 있으면 저위치 시킨다
			{
				// work robot z축 안전위치에 있지 않으면 안전위치로 이동 
				m_nBarcode2dStep = 2100;
			}
			else
			{
				// work robot z축 안전위치에 있으면 
				m_nBarcode2dStep = 0;

				return RET_GOOD;
			}
			break;

		case 2100:
			nRet[0] = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY], COMI.mn_runspeed_rate);
			if (nRet[0] == BD_GOOD) //좌측으로 이동
			{
				m_nBarcode2dStep = 0;

				return RET_GOOD;
			}
			else if (nRet[0] == BD_RETRY)
			{
				m_nBarcode2dStep = 2100;
			}
			else if (nRet[0] == BD_ERROR || nRet[0] == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
				CTL_Lib.Alarm_Error_Occurrence(1403, dWARNING, st_alarm_info.strCode);

				m_nBarcode2dStep = 2100;
			}
			break;
	}

	return RET_PROCEED;
}