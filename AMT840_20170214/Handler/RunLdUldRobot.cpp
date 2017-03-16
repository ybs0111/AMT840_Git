// RunLdUldRobot.cpp : ���� �����Դϴ�.
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

	//linear move ���� ����
	m_nLinearMove_Index	= 0;   //���� map index
	m_lAxisCnt	= 2;            //������ IndexNum���� ��� ������ ���� ���� �ִ� 4��
	m_lpAxisNum[0] = M_WORK_RBT_Y; //m_lpAxisNum[4];        //������ IndexNum���� ����ϴ� ���� ���� ��ȣ�� ������
	m_lpAxisNum[1] = M_WORK_RBT_X;
	m_lpAxisNum[2] = M_WORK_RBT_Z;

	m_nRobot_X = M_WORK_RBT_X;
	m_nRobot_Y = M_WORK_RBT_Y;
	m_nRobot_Z = M_WORK_RBT_Z; 

	m_dpTargetPosList[0]  = 0; //m_dpTargetPosList[4];		   //�ִ� 4 ����Ʈ�� �ѹ��� �̵������ϴ�
	m_dpTargetPosList[1]  = 0;
	m_dpTargetPosList[2]  = 0;

 	m_dpSpdRatio[0] = 100; 		//	m_dpSpdRatio[3];        //�ӵ����� [0]:Vel, [1]:Acc, [2]:Dec
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
	if (st_handler_info.nSystemLock != FALSE)  return;	// ���� �ý��� Lock ������ ��� ����  
	

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

	if (st_sync_info.nInit_Flag[THD_WORK_RBT] != INIT_CLEAR)		return;		//INIT_CLEAR �϶��� �ʱ�ȭ �۾��� �Ѵ�. �ʱ�ȭ�� ������ INIT_CLEAR -> INIT_READY�� �Ǳ� ������...
	 
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

		case 400://james 2016.0827 �׸��� ����̽� üũ ���� ����/����, Gripper Off(Ŭ��������)���� IO_OFF�� ����̽� �ִ»���, �׿ܴ� ON/OFF ������� ��� ����̽� ���� ����
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_LdUldPickDvcChk,	IO_OFF);
			nRet_2 = FAS_IO.get_out_bit(st_io_info.o_LdUldPickGripCyl,	IO_OFF); //clamp ����
			if(nRet_1 == IO_ON && nRet_2 == IO_OFF) 
			{
				m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_LdUldPickDvcChk); 
				CTL_Lib.Alarm_Error_Occurrence(1005, dWARNING, m_strAlarmCode);						
			}
			else//IO_OFF ���� ���� B����
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
	//2016.0921 james LOT END üũ �κ� 
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if(st_lot_info[LOT_CURR].nLotStatus >= LOT_END_START)
	{
		if(st_lot_info[LOT_CURR].nLot_THD_Status[THD_WORK_RBT] == LOT_END && 
		   st_lot_info[LOT_CURR].nLot_THD_Status[st_lot_info[LOT_CURR].nStacker_Good_THD_Num] == LOT_END && 
		   st_lot_info[LOT_CURR].nLot_THD_Status[st_lot_info[LOT_CURR].nStacker_Reject_THD_Num] == LOT_END) //���� lot End ����
		{
			st_var.nTestDvcOutFlag = 0 ; // 

			//st_lot_info[LOT_CURR].nLotEnd[THD_UNDER_UNLOAD_TRAY_STACKER_SITE] = CTL_NO;
			//for(i=0; i<THREAD_MAX_SITE; i++) st_lot_info[LOT_CURR].nLotEnd[i] = CTL_NO; //2015.0302 
			//st_lot_info[LOT_CURR].nLotStatus = LOT_CLEAR;
			// lot end �߰�
				 
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
				//CTL_Lib.Alarm_Error_Occurrence(0, dWARNING, _T("990000")); //inline off line �϶��� lot end �˶� �߻���Ŵ 
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
		    st_lot_info[LOT_NEXT].nLot_THD_Status[st_lot_info[LOT_NEXT].nStacker_Reject_THD_Num] == LOT_END) //���� lot End ����
		{
//			st_var.nTestDvcOutFlag = 0 ; // 

			//st_lot_info[LOT_CURR].nLotEnd[THD_UNDER_UNLOAD_TRAY_STACKER_SITE] = CTL_NO;
			//for(i=0; i<THREAD_MAX_SITE; i++) st_lot_info[LOT_CURR].nLotEnd[i] = CTL_NO; //2015.0302 
			//st_lot_info[LOT_CURR].nLotStatus = LOT_CLEAR;
			// lot end �߰�
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
				//CTL_Lib.Alarm_Error_Occurrence(0, dWARNING, _T("990000")); //inline off line �϶��� lot end �˶� �߻���Ŵ 
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
			st_lot_info[LOT_NEXT].nBarcodeMode				= 0;// barcode ���ۼ��� (1��/2��)
			st_lot_info[LOT_NEXT].nQty						= 0;// lot ����.....
			st_lot_info[LOT_NEXT].nModuleCount				= 0;// pgm download �Ϸ�......
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

		case 100://��Ŀ ���¸� �ľ��Ѵ�
			//james 2016.0827 �׸��� ����̽� üũ ���� ����/����, Gripper Off(Ŭ��������)���� IO_OFF�� ����̽� �ִ»���, �׿ܴ� ON/OFF ������� ��� ����̽� ���� ����
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_LdUldPickDvcChk,	IO_OFF);
			nRet_2 = FAS_IO.get_out_bit(st_io_info.o_LdUldPickGripCyl,	IO_OFF); //clamp ����
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
			Set_Glipper_OnOff(IO_ON); //�̸� ������ 
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
			if (nRet_1 == BD_GOOD) //�������� �̵�
			{
				m_nRunStep = 500;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nRunStep = 400;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
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
			if(nRet_1 == BD_GOOD) //���������� �Ϸ�� ����
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
				//load plate�� ���� ��û
				st_sync_info.nWorkRbt_Dvc_Req[THD_LD_TRAY_PLATE][0] = CTL_REQ;
				st_sync_info.nWorkRbt_Dvc_Req[THD_LD_TRAY_PLATE][1] = WORK_PICK;

				m_nFindLotNo_Flag = 0; //2016.0912
				m_nRunStep = 1000;
			}
			break;

/*
///////////////////////////////////////////
//2016.0805 ���Ͽ��� ��� Ȯ��
///////////////////////////////////////////
1. �׽��ʹ� �����Ҽ��� ������ Ȯ�� �� ���� ������ �°� ���ۿ��� ���´�
2. ���Ͽ�����  ���ۿ� ���簡 ���������� ��ũ�κ��� ���ۿ� ���� ���� ����Ѵ�
3. �׽�Ʈ�κ��� �׽��Ϳ� ����, �׽�Ʈ ���� ���簡 ������ ���� ��ε� ���ۿ� ���´�
4. 1������ ���°��� �����̳�, �׿� ���Ͽ��������� �ε� ���ۿ� ���簡 ���������� ������ ������� �����Ѵ�
  1) �۾��� Ȯ���� �׽����� ���� ������ Ȯ���� ���ۿ� ��ġ�ϸ� 1������ �̵�.
      ��ġ���� ������ ��������
  2) �׽��� ���� ������ �°� ��ũ �κ��� ���縦 ���ۿ� ���� �� �ֵ��� ������ ������ ����ΰ� �׽��ͷκ��� �ε����ۿ� �����ִ� �۾� ������ ���縦 ��� ���´�.
  3) ��ũ�κ��� ���۰� ����� ������ Ȯ���� �׽��Ϳ� �ʿ��� ���� ���� ��ġ�� 
     ���縦 ���´� 
  4) ��3)�� ������ �ݺ��Ͽ� �׽�Ʈ�κ���, ������ ������ �׽��� ���ϰ� ��ġ�ϸ� 
      �ε� ������ �۾��� �ϼ��� �����̴� 
  5) �׽�Ʈ �κ��� ������ ���縦 ���´�
  6) �׽�Ʈ �κ��� �̹� �����ִ� ����� ���� ���ۿ��� ���� ���縦 ������ Ȯ���� 
      �׽�Ʈ ����Ʈ�� �̵��Ѵ�.	
 */

		case 1000:	
			//////////////////////////////////////////////////////////////////////////////////////////////////////////
			//���� lot�� �����ϰ� �۾��� lot ������ LOT_CURR�� LOT_NEXT LotNo�� �ݺ������� �����Ͽ� Lot ������ ã�´� 
			//////////////////////////////////////////////////////////////////////////////////////////////////////////
			if(m_nFindLotNo_Flag == 0)
			{
				m_nFindLotNo_Flag = 1;						

				if(st_lot_info[LOT_CURR].nLotStatus >= LOT_START) //james 2016.0924 �߰� 
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
				
				// ���� LOT �۾��� ���� üũ ��� 
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
				{//���� lot �� ���������� �������� ���� lot No �� ���ϰ� break; �Ѵ� 
					break;
				}	
			}//���� lot ���� Ȯ�� 

			if(st_lot_info[m_nLotProcessNum].strLotNo == "")//lot ������ ������ ���� 
			{
				break;
			}
			else if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] ==  LOT_END) //�̹� �ش� lot�� lot end�̸� 
			{
				break;
			}
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////

			////////////////////////////////////////////////////////////
			//��ε� ���ۿ��� ���� ������ �䱸������  ó���Ѵ� 
			////////////////////////////////////////////////////////////
			if(st_sync_info.nWorkRbt_Dvc_Req[THD_ULD_BUFF][0] == CTL_REQ) 
			{
				if(m_nLoadBuff_Job_Continue_Flag == CTL_YES) //�ε� ���ۿ� ���縦 ���� �ִ� ���̴� ��� �۾��� �����Ѵ�
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
							if(st_lot_info[LOT_NEXT].nLotStatus >= LOT_START) //james 206.0930 ���� �߰� 
							{
								if(st_lot_info[LOT_NEXT].nLotNoApply_FirstTestStart == LOT_START) //james 2016.0926 test site�� �ϳ��̻� Next LotNo�� ���õǾ����� �������� 
								{
									//next test �۾� ���� 
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
					//james 2016.0930 ���� �߰� 
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
									m_nRunStep = 6000; //unload buffer�� ���縦 ���� �����Ѵ� 
									break; 
								}
							}
							else if(m_nLotProcessNum == LOT_NEXT)
							{
								if(st_buffer_info[THD_ULD_BUFF].st_pcb_info[i].strLotNo == st_lot_info[LOT_NEXT].strLotNo)
								{
									m_nRunStep = 6000; //unload buffer�� ���縦 ���� �����Ѵ� 
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
			}//��ε� ���� ���� 


			//////////////////////////////////////////////
			// �ε� ���� ���� 
			//////////////////////////////////////////////

			if(m_nLotProcessNum == LOT_CURR) 
			{
				if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END_START) //test robot�� LOT_END_START ������ ����Ǿ�߸� next lot�� ���� �� �� �ִ� 
				{
					//kwlee 2016.1112 
					if (st_lot_info[LOT_CURR].nLot_THD_Status[THD_ULD_BUFF] == LOT_END)
					{
						m_nRunStep = 6500;
					}
					/////
					break;//LOT_CURR �� �ε� ���簡 �ε� ���ۿ��� ������ ������������� next lot�� load plate ������ �غ��� �� �ְ�, �κ��� ������ �ȵȴ�. 
				}

				if(st_lot_info[m_nLotProcessNum].nLotStatus >= LOT_END_START && st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] <  LOT_END_START) //���� ȭ�� ���� lot end ��ȣ //james 2016.0909 
				{
					st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] =  LOT_END_START;

					//james 2016.0929 
					nCount = 0;
					for(i = 0; i < st_recipe_info.nLdUldBuffer_Num; i++) //work robot�� �ε� ���ۿ� ���縦 �������� 0->1->2->3 ������ �۾�(���� �ڿ��� ���� 1���̴�)
					{
						if(st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nYesNo == CTL_YES)//	&& (st_buffer_info[nSite].st_pcb_info[i].strLotNo == str_LotNo || str_LotNo == "0") ) //nDVCYesNO == CTL_NO ���ǿ� ������ ������ üũ, ��밡���� ���Ͽ��� �۾�����
						{
							nCount++;			
						} 
					}
					if(nCount > 0)
					{
						st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_READY; //���ۿ� ������ �ִ� ��� ���縦 �������� �׋�, ��ü ��û�� ���� 

						m_nLoadBuff_Job_Continue_Flag = CTL_NO; //clear
					} 
					break;
				}		 
			}
			else //if(m_nLotProcessNum == LOT_NEXT) 
			{
				if(st_lot_info[LOT_CURR].nLot_THD_Status[THD_TEST_RBT] <  LOT_END_START) //test robot�� LOT_END_START ������ ����Ǿ�߸� next lot�� ���� �� �� �ִ� 
				{
					break;//LOT_CURR �� �ε� ���簡 �ε� ���ۿ��� ������ ������������� next lot�� load plate ������ �غ��� �� �ְ�, �κ��� ������ �ȵȴ�. 
				}

				if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END_START) //test robot�� LOT_END_START ������ ����Ǿ�߸� next lot�� ���� �� �� �ִ� 
				{
					break;//LOT_CURR �� �ε� ���簡 �ε� ���ۿ��� ������ ������������� next lot�� load plate ������ �غ��� �� �ְ�, �κ��� ������ �ȵȴ�. 
				}

				if(st_lot_info[m_nLotProcessNum].nLotStatus >= LOT_END_START && st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] <  LOT_END_START) //���� ȭ�� ���� lot end ��ȣ //james 2016.0909 
				{
					st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] =  LOT_END_START;

					//james 2016.0929 
					nCount = 0;
					for(i = 0; i < st_recipe_info.nLdUldBuffer_Num; i++) //work robot�� �ε� ���ۿ� ���縦 �������� 0->1->2->3 ������ �۾�(���� �ڿ��� ���� 1���̴�)
					{
						if(st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nYesNo == CTL_YES)//	&& (st_buffer_info[nSite].st_pcb_info[i].strLotNo == str_LotNo || str_LotNo == "0") ) //nDVCYesNO == CTL_NO ���ǿ� ������ ������ üũ, ��밡���� ���Ͽ��� �۾�����
						{
							nCount++;			
						} 
					}
					if(nCount > 0)
					{
						st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_READY; //���ۿ� ������ �ִ� ��� ���縦 �������� �׋�, ��ü ��û�� ���� 

						m_nLoadBuff_Job_Continue_Flag = CTL_NO; //clear
					} 
					break;
				}		 
			}

			 /*2016.0930 
			 //�ε� �÷���Ʈ�� ���縦 ���´� 
			//james 2016.0909 �ε� ���縦 ������ lot ���� ��Ȳ�� lot ������ �׻� üũ �Ѵ� 			
			if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END_START)  
			{
				// work robot�� �ε� �κ��� lot end start �� ������ ���·�,����, �׽�Ʈ ����Ʈ, ���� ��� ó���Ǹ� lot end�� ������ �� �ִ�
				// �׷��� ����lot�� �־� �� ��ũ �κ��� ������ �Ǹ� ���  �����ؾ� �Ѵ� 
			
					//�켱 �ε� ������ ���簡 ������ �� ������� �� ó���ϰ� , �ε��� �������Ѵ�.
					//�� �ε� ���簡 �ε� ���ۿ��� ������������� next lot�� load plate ������ �غ��� �� �ִ�. 

				//���Ҿ� �׽�Ʈ ����Ʈ�� #1 ~#4 ������ Next Lot ���޵Ǹ� ó���� �������� ���������� �� ����Ʈ�� ����ָ�, LOT_CURR �� #5 ~#8 ������ ����� �� �ְ� TEST ROBOT���� Test Site�� �� lot ��������ġ�� �°� ������ �ش�  
				
				if(st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)//james 2016.0424 
				{

				}
				else
				{
					nRet_1 = clsFunc.Find_TrayWork_Pos(WORK_PLACE, THD_LD_BUFF, CTL_YES, m_npFindWorkPosYXCPB, THD_WORK_RBT, st_worker_info.strLotNo_Empty);		 			
					if(m_npFindWorkPosYXCPB[2] > 0 && m_nLoadBuff_Job_Continue_Flag == CTL_YES) //�ε� ���ۿ� ���簡 �ϳ��� ������ �׽�Ʈ �A �ְ� ������ 
					{	
						st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_READY; //���ۿ� ������ �ִ� ��� ���縦 �������� �׋�, ��ü ��û�� ���� 

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
				for(i = 0; i < st_recipe_info.nLdUldBuffer_Num; i++) //work robot�� �ε� ���ۿ� 0,1,2,3 ������ �۾�
				{ //������ ���� ������ �����Ѵ�					 
					if(st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nYesNo == CTL_YES &&
						st_buffer_info[THD_LD_BUFF].st_pcb_info[i].strLotNo == st_lot_info[LOT_CURR].strLotNo)//james 2016.0924 ���� �߰� 
					{
						nCount++;
					}
				}
				if(nCount > 0)
				{
					if(st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_REQ )
					{
						st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_READY; //���ۿ� ������ �ִ� ��� ���縦 �������� �׋�, ��ü ��û�� ���� 
						m_nLoadBuff_Job_Continue_Flag = CTL_NO; //clear
					}
				}//2016.0926 

				st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] =  LOT_END_START; //work robot�� lot end�� ����ǵ� , ��ε� �� ���� �� next lot�� ó���� ���� ��� ���ƾ� �Ѵ� 
				break;
			}*/

			  
			if(st_sync_info.nWorkRbt_Dvc_Req[THD_LD_TRAY_PLATE][0] == CTL_READY)//�ε� �÷���Ʈ ���� �� �÷��� Ȯ��  && st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_REQ) //�ε� Ʈ���̰� �غ�Ǿ� �ְ�, �ε� ���ۿ����� ���� ������ ��û �޾����� �������� 
			{
				if(m_strFindLotNo != st_tray_info[THD_LD_STACKER].strLotNo) // lot ������ Ʋ���� �ȵȴ� 
				{						
					//�޼��� ��� 
					break;
				}

				//james 2016.0924 ���� �߰� 
				if(m_nLotProcessNum == LOT_NEXT) //Next lot �����ϴ� �����̸� LOT_CURR�� ����� �ε����ۿ� �����ϸ� �ȵȴ�  
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
				if(nRet_1 == RET_SKIP) //���̻� �۾��� ������ ���� 
				{	
					st_sync_info.nWorkRbt_Dvc_Req[THD_LD_TRAY_PLATE][0] = CTL_REQ; //�ε� ���縦 ��û, ���̻� ���� Ʈ���̿��� ������ �ε� Ʈ���̸� ��ü�� ���̴� 
					m_nLoadBuff_Job_Continue_Flag = CTL_NO; //clear
					break;
				}*/

				//2016.1117
// 				if(st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] != CTL_REQ) //james 2016.1005 �̸� ���� ������ ����, ���׽�Ʈ �ٹ߽� ������
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
				m_nRunStep = 2000; //�ε� �÷���Ʈ���� ������ ���� �ε� ���ۿ� ���縦 ���´�
			}
			break;

////////////////////////////////////////////////
// load tray�� ���縦 ���´� 
// ���縦 �ε� ������ ������ �°� ��� ä������ 1000 step�� �ٽð� ���� ���� �����Ѵ� 
////////////////////////////////////////////////
		case 2000:
			nRet_1 = Process_DVC_Pick(0, THD_LD_TRAY_PLATE);
			if(nRet_1 == RET_GOOD)
			{				
				m_nRunStep = 2100;
			}
			else if(nRet_1 == RET_PICKER_NOT_FIND) //��Ŀ�� ���̻� �۾��� ������ ����. �̶��� �κ�Ʈ�� ���ۿ� ������ �̵��Ѵ�
			{
				m_nRunStep = 2100;			
			}
			else if(nRet_1 == RET_TRAY_NOT_FIND) //Ʈ���̿��� �� �̻� �۾��� �� �ִ� ������ ����
			{
				//st_sync_info.nLdRobot_Req[m_nLdRobot_Work_Pos_Num] = CTL_CHANGE; //Ʈ���� ��ü ��û
				//m_nLdRobot_Work_Pos_Num = 0; //��Ŀ�� ���� �� �ִ� �����ְ�, Ʈ���̴� �۾��� ������ ������ clear

				m_nRunStep = 2100; //2100; //1000;

			}
			break; 

		case 2100: //�̸� ���縦 ��û�Ͽ� Ʈ���̿� ���簡 ������ plate���� �ڵ����� Ʈ���̸� ��ü �۾��� �� ���̴� 
			//james 2016.0809 lot end ������ Ȯ���Ѵ� 
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
			  {//�ش� lot���� �۾��� ������ ��� ó���� ����
				  //���縦 ���ۿ� ������ �������� ���� Ȯ�� st_lot_info[LOT_CURR].nLot_THD_Status[THD_WORK_RBT] =  LOT_END_START;
			  }
			  else if(st_lot_info[m_nLotProcessNum].nLotStatus >= LOT_END_START) //���� ȭ�� ���� lot end ��ȣ //james 2016.0909 
			  {
				  //���縦 ���ۿ� ������ �������� ���� Ȯ��  st_lot_info[LOT_CURR].nLot_THD_Status[THD_WORK_RBT] =  LOT_END_START;
			  }		
			  else 
			  {
				  //james 2016.0930 st_sync_info.nWorkRbt_Dvc_Req[THD_LD_TRAY_PLATE][0] = CTL_REQ; //�ε� ���縦 ��û, ���̻� ���� Ʈ���̿��� ������ �ε� Ʈ���̸� ��ü�� ���̴� 
			  }*/

			  st_sync_info.nWorkRbt_Dvc_Req[THD_LD_TRAY_PLATE][0] = CTL_REQ; //james 2016.0930 //�ε� ���縦 ��û, ���̻� ���� Ʈ���̿��� ������ �ε� Ʈ���̸� ��ü�� ���̴� 
			  m_nRunStep = 2300; 
			}
			else
			{//������ �ȵ� 
				break; 
			} 
			break;

		////////////////////////////////////////////////////////////
		// �ε� ���ۿ� ���縦 ���´� 
		////////////////////////////////////////////////////////////
		case 2300:			 
			if(st_picker[m_nRobotSite].st_pcb_info[0].nYesNo == CTL_NO)//��Ŀ�� ���簡 ��� �ִٴ� ������ ����
			{//james 2016.1005 ���簡 ������ ���ķ� �� �ʿ������ ó������ ���ư��� 
				m_nRunStep = 1000; 
				break;
			} 
			m_nRunStep = 2400; 
			break;

		case 2400:
			 //�ε� ���۰� �κ����� ���縦 ��û������ �����Ѵ�. ��� ������ ������ �ݺ� ������ �ؾ� �ϴ� ������ �÷��׸� ��������  
			if(st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_REQ)
			{
				m_nRunStep = 2500;  
			}
			break;

		case 2500:
			nRet_1 = Process_DVC_Place(100, THD_LD_BUFF, st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][2]); //4��3,2,1�� ������ ����� �� �ִ� ��ġ�� ���ۿ� ���ƾ� ������ ���� 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 2510; //james 2016.1003 2600;
			}
			else if(nRet_1 == RET_PICKER_NOT_FIND) //��Ŀ�� ���̻� �۾��� ������ ����. �̶��� �κ�Ʈ�� ���ۿ� ������ �̵��Ѵ�
			{
				m_nRunStep = 2600;			
			}
			else if(nRet_1 == RET_TRAY_NOT_FIND) //Ʈ���̿��� �� �̻� �۾��� �� �ִ� ������ ����
			{
				//st_sync_info.nLdRobot_Req[m_nLdRobot_Work_Pos_Num] = CTL_CHANGE; //Ʈ���� ��ü ��û
				//m_nLdRobot_Work_Pos_Num = 0; //��Ŀ�� ���� �� �ִ� �����ְ�, Ʈ���̴� �۾��� ������ ������ clear

				m_nRunStep = 2600;  

			}
			break;

		case 2510: //james 2016.1003 
			 nCount = 0;
			 for(i = 0; i < LD_ULD_PICKER_PARA; i++) //test site���� �۾��� ������ Ȯ���Ѵ�
			 {					 
				 if(st_picker[THD_WORK_RBT].st_pcb_info[i].nYesNo == CTL_YES)// && st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable == CTL_YES)
				 {
					 //2016.1203
					 m_strLotNo[0] = st_picker[THD_WORK_RBT].st_pcb_info[i].strLotNo;
					 m_strPartNo[0] = st_picker[THD_WORK_RBT].st_pcb_info[i].strPartNo;
					 nCount++;
				 } 
			 }
			 if(nCount > 0) //���簡 ���������� 
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
			if(nRet_1 == RET_SKIP) //���̻� �۾��� ������ ���� 
			{	
				st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_READY; //���ۿ� ������ �ִ� ��� ���縦 �������� �׋�, ��ü ��û�� ���� 

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
				{//���ۿ��� ���̻� ���� ������ ����
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

		case 2610://üũ�� �Ѵ�.
			if(st_sync_info.nWorkRbt_Dvc_Req[THD_LD_TRAY_PLATE][0] != CTL_READY)
			{
				//2016.1228
				if( st_handler_info.mn_NoInput_test == CTL_YES)//������ ����� ������ �۾��ڰ� ���� ������ ���ϰ� �ִ�.�ϴ�, ������ �ɷ� ��������.
				{
					st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_READY; //���̻� ���ۿ� ���� ���簡 ������ ������ 
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
			else if(nRet_1 == RET_TRAY_NOT_FIND) //Ʈ���̿��� �� �̻� �۾��� �� �ִ� ������ ����
			{
				m_nRunStep = 2700;
			}
			break;
		
		///////////////////////////////////////////////////////////////////////
		//lot end ������ üũ�Ѵ� 
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
			{//�ش� lot���� �۾��� ������ ��� ó���� ����	

				//2016.1031 ���ص带 �ϱ����� loadtrayplate�� �ִ� Ʈ���̸� �����ϱ����� ���簡 �ִ��� �Ⱦ��غ��������� ���� ���� ������ ��������
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
				for(i = 0; i < st_recipe_info.nLdUldBuffer_Num; i++) //work robot�� �ε� ���ۿ� ���縦 �������� 0->1->2->3 ������ �۾�(���� �ڿ��� ���� 1���̴�)
				{
					if(st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nYesNo == CTL_YES)//	&& (st_buffer_info[nSite].st_pcb_info[i].strLotNo == str_LotNo || str_LotNo == "0") ) //nDVCYesNO == CTL_NO ���ǿ� ������ ������ üũ, ��밡���� ���Ͽ��� �۾�����
					{
						nCount++;			
					} 
				}
				if(nCount > 0)
				{
					st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_READY; //���ۿ� ������ �ִ� ��� ���縦 �������� �׋�, ��ü ��û�� ���� 

					m_nLoadBuff_Job_Continue_Flag = CTL_NO; //clear
				} 
			}
			else if(st_lot_info[m_nLotProcessNum].nLotStatus >= LOT_END_START) //���� ȭ�� ���� lot end ��ȣ //james 2016.0909 
			{
				st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] =  LOT_END_START;

				//james 2016.0929 
				nCount = 0;
				for(i = 0; i < st_recipe_info.nLdUldBuffer_Num; i++) //work robot�� �ε� ���ۿ� ���縦 �������� 0->1->2->3 ������ �۾�(���� �ڿ��� ���� 1���̴�)
				{
					if(st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nYesNo == CTL_YES)//	&& (st_buffer_info[nSite].st_pcb_info[i].strLotNo == str_LotNo || str_LotNo == "0") ) //nDVCYesNO == CTL_NO ���ǿ� ������ ������ üũ, ��밡���� ���Ͽ��� �۾�����
					{
						nCount++;			
					} 
				}
				if(nCount > 0)
				{
					st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_READY; //���ۿ� ������ �ִ� ��� ���縦 �������� �׋�, ��ü ��û�� ���� 

					m_nLoadBuff_Job_Continue_Flag = CTL_NO; //clear
				} 
			}		 
			m_nRunStep = 1000; 
			break;

//////////////////////////////////////////////////////////////////////////////
// ��ε� ������ ���縦 ���� �����Ѵ� 
//////////////////////////////////////////////////////////////////////////////
		case 6000:
			nRet_1 = Process_DVC_Pick(0, THD_ULD_BUFF);
			if(nRet_1 == RET_GOOD)
			{					
				m_nRunStep = 6100;
			}
			else if(nRet_1 == RET_PICKER_NOT_FIND) //��Ŀ�� ���̻� �۾��� ������ ����. �̶��� �κ�Ʈ�� ���ۿ� ������ �̵��Ѵ�
			{
				if(st_picker[THD_WORK_RBT].st_pcb_info[0].nYesNo == CTL_NO) //���縦 ���� ���� ������ ó������ ���� 
				{
					m_nRunStep = 1000;			
				}
			}
			else if(nRet_1 == RET_TRAY_NOT_FIND) //Ʈ���̿��� �� �̻� �۾��� �� �ִ� ������ ����
			{				
				if(st_picker[THD_WORK_RBT].st_pcb_info[0].nYesNo == CTL_NO) //���縦 ���� ���� ������ ó������ ���� 
				{
					m_nRunStep = 1000;			
				}
			}
			break;

		//////////////////////////////////////////////////
		// ��ε� ���ۿ� ���簡 ���� �ִ��� Ȯ���Ѵ� 
		// �ش�� ���� �����ϴ� (GOOD, REJECT BIN)
		//////////////////////////////////////////////////
		case 6100: //���� ������ Ȯ���Ͽ� ���� �Ѵ�.
			nRet_1 = clsFunc.Find_TrayWork_Pos(WORK_PICK, THD_ULD_BUFF, CTL_YES, m_npFindWorkPosYXCPB, THD_WORK_RBT, st_worker_info.strLotNo_Empty);	
			if(nRet_1 == RET_GOOD) //�۾� ������ ���簡 �ִ� 
			{						
				m_nRunStep = 6200;  
			}
			else if(nRet_1 == RET_SKIP) //���̻� �۾��� ���簡 ���� 
			{
				st_sync_info.nWorkRbt_Dvc_Req[THD_ULD_BUFF][0] = CTL_READY; //��ε� ������ ��� ���縦 ��������� ��ü ��û
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
		//���� ������ �� ������ ���� �����Ѵ� 
		//���ü��� : 1. good ����, 2:�������� 
			//���� Lot No Ȯ�� �ʿ� //james 2016.0924 Ȯ���ض� 
		/////////////////////////////////////////////////////////////////////////////////
		case 6200: // 1 picker ��
			nFlag = 0; nTemp = 0;
			m_nWorkRobot_uldSite_SortJob_THD_Pos = -1;
			for(nCount = 0; nCount < 2; nCount++) //bin ������ reject, good ���� 2���� ���ú��� �����Ѵ� 
			{
				//��ε� ���ü����� 1.rejext, 2:retest, 3:good ���� ������ ������ �����Ѵ� 
				if(nCount == 0)			nBin = BD_DATA_GOOD;  //ù��° ������ ��  
				else if(nCount == 1)	nBin = BD_DATA_REJECT;//�ι�° ������ ��
				//������ else if(nCount == 2 )	nBin = BD_DATA_RETEST; //3��° ������ ��, Retest Bin�� TEST ROBT���� ó���Ѵ�  

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
// 									//�� ����Ŀ ���ÿ� ������ �ִ� , ���� 
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
									//�� ����Ŀ ���ÿ� ������ �ִ� , ���� 
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
									//�� ����Ŀ ���ÿ� ������ �ִ� , ���� 
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
// 									//�� ����Ŀ ���ÿ� ������ �ִ� , ���� 
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
									//�� ����Ŀ ���ÿ� ������ �ִ� , ���� 
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
									//�� ����Ŀ ���ÿ� ������ �ִ� , ���� 
								}
							}
						}
					}
				}

				if(nFlag > 0) //�۾��� Ʈ���� ������ ������ �۾��Ѵ� 
				{
					nCount = 3;  
					break;
				}
			}//for

			if(m_nWorkRobot_uldSite_SortJob_THD_Pos == -1)
			{
				//������ ���� ����. ���� �� 
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
			st_sync_info.nWorkRbt_Dvc_Req[m_nWorkRobot_uldSite_SortJob_THD_Pos][0] = CTL_REQ; //���� �� ���� �ش� ���� �غ� ��û 
			m_nXYRobot_PreMove_Flag = 0;
			m_nRunStep = 6310;
			break;

		case 6310:
			if(st_sync_info.nWorkRbt_Dvc_Req[m_nWorkRobot_uldSite_SortJob_THD_Pos][0] == CTL_READY) 
			{//ready�� �̰����� ��ٸ��� ��� �ð��� �ʹ� ����. �̸� X,Y�� �̵��� ����, �׷��� ���ۿ�, Ʈ���̴� ������ �ʿ��ϴ� �����ʿ� 
				m_nRunStep = 6400;
			}
			else
			{
				if(m_nXYRobot_PreMove_Flag == 100) break; //�̹� �۾��� ��ġ�� �̸� �̵��� ���� 
				//��ε� ����Ŀ�� �غ� �ȵǾ� ������,  �̸� �۾��� ��ġ�� �κ�Ʈ�� �̵��ؾ� �Ѵ�
				m_nRunStep = 6320;
			}
			break;

		case 6320: //�̸� �ش� ����Ʈ�� �̵��Ѵ� 
			nRet_1 = clsFunc.Find_TrayWork_Pos(WORK_PLACE, m_nWorkRobot_uldSite_SortJob_THD_Pos, CTL_NO, m_npFindWorkPosYXCPB, THD_WORK_RBT, st_worker_info.strLotNo_Empty); //���������ۿ����� ���۾������� �ں��� ���´� (4 -> 1 ����) 
			if(nRet_1 == RET_GOOD) //�۾� ������ ���簡 �ִ�
			{
				m_n_FirstTray_Y_Num = m_npFindWorkPosYXCPB[0]; m_n_FirstTray_X_Num = m_npFindWorkPosYXCPB[1];
			}
			else if(nRet_1 == RET_SKIP) //���̻� ���簡 ����
			{	//�̵��� ������ �������� ó�� ��ġ�� �̵��Ѵ� 
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
					Set_WorkRbtRotator_OnOff(IO_ON); //rotator �����鼭 �̵��Ѵ�
				}
				else //if(nWork_Site == THD_LD_TRAY_PLATE || nWork_Site == THD_ULD_1_STACKER || nWork_Site == THD_ULD_2_STACKER || nWork_Site == THD_REJECT_OUT_1_STACKER || nWork_Site == THD_REJECT_OUT_2_STACKER)
				{
					Set_WorkRbtRotator_OnOff(IO_OFF); //rotator�� Ʈ���̷� ���� �� �ְ� ȸ���ϸ鼭 �̵��Ѵ�
				}
			}
						
			nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate); 
			if (nRet_1 == BD_GOOD) //�������� �̵�
			{				 
				m_nXYRobot_PreMove_Flag =  100;
				m_nRunStep = 6310;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nRunStep = 6310;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
				CTL_Lib.Alarm_Error_Occurrence(1107, dWARNING, st_alarm_info.strCode);
				m_nRunStep = 6310;
			}
			break; 	

		//////////////////////////////////////////////////////
		// ���縦 �����Ѵ� 
		//////////////////////////////////////////////////////
		case 6400:
			nRet_1 = Process_DVC_Place(100, m_nWorkRobot_uldSite_SortJob_THD_Pos, 0); //��ε�����Ŀ�������Ѵ� 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 6410; //james 2016.1003 6500;
			}
			else if(nRet_1 == RET_PICKER_NOT_FIND) //��Ŀ�� ���̻� �۾��� ������ ����. �̶��� �κ�Ʈ�� ���ۿ� ������ �̵��Ѵ�, 1 ��Ŀ�� ������ ���� 
			{
				m_nRunStep = 6400;			
			}
			else if(nRet_1 == RET_TRAY_NOT_FIND) //Ʈ���̿��� �� �̻� �۾��� �� �ִ� ������ ����, 1 ��Ŀ�� ������ ���� 
			{
				//st_sync_info.nLdRobot_Req[m_nLdRobot_Work_Pos_Num] = CTL_CHANGE; //Ʈ���� ��ü ��û
				//m_nLdRobot_Work_Pos_Num = 0; //��Ŀ�� ���� �� �ִ� �����ְ�, Ʈ���̴� �۾��� ������ ������ clear

				m_nRunStep = 6400;  

			}
			break;

		case 6410: //james 2016.1003 
			nCount = 0;
			for(i = 0; i < 1; i++) //test site���� �۾��� ������ Ȯ���Ѵ�
			{					 
				if(st_picker[THD_WORK_RBT].st_pcb_info[i].nYesNo == CTL_YES)// && st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable == CTL_YES)
				{
					nCount++;
				} 
			}
			if(nCount > 0) //���簡 ���������� 
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
			if(nRet_1 == RET_GOOD) //�۾� ������ ������ �� �ִ�  
			{				
				if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_TEST_RBT] >=  LOT_END) 
				{//CURR_LOT ����� �׽�Ʈ ����Ʈ, Retest Buffer ���� ����, unload buffer���� ���� �����̸� ���� lot�� ����� ��� ó���� �����̴� 
					//unload good stacker���� ��ü LOT END �� �����Ѵ�
					// 2016.0918 �߰� 
					nRet_1 = clsFunc.Find_TrayWork_Pos(WORK_PICK, THD_ULD_BUFF, CTL_YES, m_npFindWorkPosYXCPB, THD_WORK_RBT, st_worker_info.strLotNo_Empty);
// 					//2016.1024
// 					nRet_1 = clsFunc.Find_TrayWork_Pos(WORK_PICK, THD_ULD_BUFF, CTL_YES, m_npFindWorkPosYXCPB, THD_WORK_RBT, st_lot_info[m_nLotProcessNum].strLotNo);
					if(nRet_1 == RET_GOOD)//�۾� ������ ���簡 �ִ� 
					{
						m_nRunStep = 6000;
					}
					else if(nRet_1 == RET_SKIP) //���̻� �۾��� ���簡 ���� 
					{//�׽�Ʈ ����Ʈ�� lot end �� �Ǿ���, ��ε� ���ۿ��� ���̻� �ش� lot�� ���簡 ������ 
						m_nRunStep = 9000;
					}
				}
				else
				{
					m_nRunStep = 6000;
				} 				 
			}
			else if(nRet_1 == RET_SKIP) //���̻� �۾��� ���簡 ���� 
			{
				//james 2016.1003 st_sync_info.nWorkRbt_Dvc_Req[m_nWorkRobot_uldSite_SortJob_THD_Pos][0] = CTL_REQ; //�̸� ��û�� ���´� 

				m_nRunStep = 6600;
			}
			break;

		case 6600: //1 ��Ŀ�� �ѹ� �۾��ϸ� ��� �۾��� �Ϸ� ��
			if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_TEST_RBT] >=  LOT_END) 
			{//CURR_LOT ����� �׽�Ʈ ����Ʈ, Retest Buffer ���� ����, unload buffer���� ���� �����̸� ���� lot�� ����� ��� ó���� �����̴� 
			 //unload good stacker���� ��ü LOT END �� �����Ѵ�

				nRet_1 = clsFunc.Find_TrayWork_Pos(WORK_PICK, THD_ULD_BUFF, CTL_YES, m_npFindWorkPosYXCPB, THD_WORK_RBT, st_worker_info.strLotNo_Empty);	
// 				//2016.1024
// 				nRet_1 = clsFunc.Find_TrayWork_Pos(WORK_PICK, THD_ULD_BUFF, CTL_YES, m_npFindWorkPosYXCPB, THD_WORK_RBT, st_lot_info[m_nLotProcessNum].strLotNo);
				if(nRet_1 == RET_GOOD)//�۾� ������ ���簡 �ִ� 
				{
					st_sync_info.nWorkRbt_Dvc_Req[m_nWorkRobot_uldSite_SortJob_THD_Pos][0] = CTL_REQ; //�̸� ��û�� ���´� 
					m_nRunStep = 1000;
				}
				else if(nRet_1 == RET_SKIP) //���̻� �۾��� ���簡 ���� 
				{//�׽�Ʈ ����Ʈ�� lot end �� �Ǿ���, ��ε� ���ۿ��� ���̻� �ش� lot�� ���簡 ������ 
					m_nRunStep = 9000;
				}
			}
			else
			{
				st_sync_info.nWorkRbt_Dvc_Req[m_nWorkRobot_uldSite_SortJob_THD_Pos][0] = CTL_REQ; //�̸� ��û�� ���´� 
				m_nRunStep = 1000; //6000
			}
			break;

		case 9000:
			//CURR_LOT ����� �׽�Ʈ ����Ʈ, Retest Buffer ���� ����, unload buffer���� ���� �����̸� ���� lot�� ����� ��� ó���� �����̴� 
			st_lot_info[m_nLotProcessNum].nLotStatus = LOT_END;
			st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] =  LOT_END;//unload good #1, #2 stacker���� ��ü LOT END �� �����Ѵ� 

			//kwlee 2016.1114
			if (st_handler_info.cWndList != NULL)  // ����Ʈ �� ȭ�� ����
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
				m_nRunStep = 0; //ó�� ���� �ٽ� �����Ѵ� 
			}

			break;
	}
}

//james 2016.0912 LOT_CURR �� LOT_NEXT �� ��� ���� ��밡���ϵ��� ��� ������ ����Ͽ� ���հ��� 

///////////////////////////////////////////////////////////////////
//nWork_Site == WORKROBT�� PICK �ϴ� ��ġ�� 2 ���� �ִ� 
//
//1. THD_LD_TRAY_PLATE (load plate tray PICK)
//2. THD_ULD_BUFF (unload buffer PICK)
///////////////////////////////////////////////////////////////////
int CRunLdUldRobot::Process_DVC_Pick(int nMode, int nWork_Site)
{//return ���� RET_TRAY_NOT_FIND, RET_PICKER_NOT_FIND, RET_GOOD, RET_PROCEED
	int nFuncRet = RET_PROCEED;
	int nRet_1, nRet_2, i, nFlag=0;
	int nPickStep = m_nPick_Step;

	CString strMsg;
	CString strLogKey[10];
	CString strLogData[10];

	//nWork_Site : �۾���ġ 3�� ���� : THD_PLATE_LD_TRAY_1_SITE, THD_PLATE_LD_TRAY_2_SITE, THD_RETEST_TRAY_BUFF_SITE
	clsFunc.OnThreadFunctionStepTrace(8, m_nPick_Step);

	switch(m_nPick_Step)
	{
	case 0:
		m_bDvcWaitChk_Falg =  false;
		m_nPickRetry_Cnt = 0;
		m_nPick_Step = 100;
		break;

	case 100:	 
		Set_WorkRbtPicker_UpDown(IO_OFF); //�̸� ��Ŀ�� ������ �ø���

		m_dCurrentPos[m_nRobot_Z] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[m_nRobot_Z] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
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
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{
			m_bDvcWaitChk_Falg =  false;
			m_nPick_Step = 1000;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 900;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(1108, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 900;
		}
		break;

	////////////////////////////////////////////////////////////////
	//��Ŀ ���¸� Ȯ�� �� ������ ������ �۾��� ��Ŀ�� ã�´�
	////////////////////////////////////////////////////////////////
	case 1000:
		m_dCurrentPos[m_nRobot_Z] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[m_nRobot_Z] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
		{
			m_nPick_Step = 900;
			break;
		}

		if(nWork_Site == THD_LD_TRAY_PLATE) // loadig tray ���� ���� 
		{
			nRet_1 = COMI.Get_MotIOSensor(M_LD_ELV, MOT_SENS_SD); 	
			if(nRet_1 != BD_GOOD) //�δ� �÷���Ʈ�� Ʈ���̰� ���� �� ���� 
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
		if(nRet_1 == RET_GOOD) //���� ������ ���� ��Ŀ ���¸� üũ 
		{			//��Ŀ ������ ���� ���°� �����̴�
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

		//�۾� ���� ��ġ�� ã�´�
		nFlag = 0;
		m_n_FirstTray_Y_Num = -1; m_n_FirstTray_Y_Num = -1; m_n_FirstPicker_Num = -1; //flag clear
		for(i = 0; i < m_nPickerPara; i++)
		{
			if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_NO)//��Ŀ�� ���簡 ��� �ִٴ� ������ ����
			{
				m_n_FirstPicker_Num = i;
				i = m_nPickerPara;
			}
		}

		if(m_n_FirstPicker_Num == -1)
		{
			//��Ŀ�� ���̻� �۾��� ���� ����
			m_nPick_Step = 0;
			nFuncRet = RET_PICKER_NOT_FIND;
			break;
		}

		nRet_1 = clsFunc.Find_TrayWork_Pos(WORK_PICK, nWork_Site, CTL_YES, m_npFindWorkPosYXCPB, THD_WORK_RBT, st_worker_info.strLotNo_Empty);
		if(nRet_1 == RET_GOOD) //�۾� ������ ���簡 �ִ�
		{
			m_n_FirstTray_Y_Num = m_npFindWorkPosYXCPB[0]; m_n_FirstTray_X_Num = m_npFindWorkPosYXCPB[1];
		}
		else if(nRet_1 == RET_SKIP) //���̻� ���簡 ����
		{	//��� ������ �� ����
			m_nPick_Step = 0;
			nFuncRet = RET_TRAY_NOT_FIND;
			break;
		}

		nFlag = 0;
		if(m_n_FirstPicker_Num != -1 && m_n_FirstTray_Y_Num != -1 && m_n_FirstTray_X_Num != -1) //�۾��� ������ �� ������ ���� �۾��� �����Ѵ�
		{
			for(i = 0; i < m_nPickerPara; i++)  //m_nPickerPara == 1
			{
				m_npPicker_YesNo[i] = CTL_NO; //�۾��� ��Ŀ���� clear
				if(i < m_n_FirstPicker_Num) continue;

				if(nWork_Site == THD_LD_TRAY_PLATE)
				{
					if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_NO)
					{
						if(i >= 0 && m_n_FirstTray_Y_Num + (i - m_n_FirstPicker_Num) < st_recipe_info.nTrayY)
						{
							if(st_tray_info[nWork_Site].st_pcb_info[ m_n_FirstTray_Y_Num + (i - m_n_FirstPicker_Num)][m_n_FirstTray_X_Num].nYesNo == CTL_YES) //Ʈ���̿� ���簡  ������ ���� �����ϰ� ��
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
				else if(nWork_Site == THD_ULD_BUFF) //�۾� ������ �ݴ�� 3,2,1,0  �̴�.
				{
					if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_NO)
					{
						if(i >= 0 && m_n_FirstTray_Y_Num + (i - m_n_FirstPicker_Num) < st_recipe_info.nLdUldBuffer_Num)
						{
							if(st_buffer_info[nWork_Site].st_pcb_info[m_n_FirstTray_Y_Num - (i - m_n_FirstPicker_Num)].nYesNo == CTL_YES) //Ʈ���̿� ���簡  ������ ���� �����ϰ� ��
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

			if(nFlag == 0) //�۾��� ��Ŀ�� �ϳ��� �غ�� �ʾҴ�.
			{ //�̺κ��� �ƿ��� �ȵ� 
				m_nPick_Step = 0;
				nFuncRet = RET_PICKER_NOT_FIND;//��Ŀ�� ���̻� �۾��� ���� ����
				break;
			}

			//���� �۾� ��ġ�� ����
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
		//�ε��δ��� ��δ� ���ۿ��� ����̽��� �Ⱦ��Ҷ� 1D,2D ��带 üũ�ؼ�
		//2D�� �� �д� ��� ���� üũ�Ͽ� �����Ѵ�.
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
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{
			m_nPick_Step = 2000;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 1500;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(1201, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 1500;
		}
		break;

	case 2000:
		m_dCurrentPos[2] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[2] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
		{
			m_nPick_Step = 900;
			break;
		}

		if(m_nRotator_Flag == 0) //
		{
			m_nRotator_Flag = 1;
			if(nWork_Site == THD_LD_TRAY_PLATE)//)
			{
				Set_WorkRbtRotator_OnOff(IO_OFF); //rotator �����鼭 �ε�/��ε� ���ۿ��� ��밡���ϰ� �����鼭 �̵��Ѵ�
			}
			else //if(nWork_Site == THD_ULD_BUFF)
			{
				Set_WorkRbtRotator_OnOff(IO_ON); //rotator�� Ʈ���̷� ���� �� �ְ� ȸ���ϸ鼭 �̵��Ѵ�
			}
		}

		if(nWork_Site == THD_ULD_BUFF)
		{//Check_Robot_Buffer_Clash(int nTestDirection, int nRbtTHDSite, int TestBuffTHDSite, int nStartPos, int *nErrorInfoStatus) 
			nRet_1 = clsFunc.Check_Robot_Buffer_Clash(TESTSITE_LEFT, THD_WORK_RBT, THD_ULD_BUFF, m_n_FirstTray_Y_Num, m_n_FirstPicker_Num, m_nClash_ErrorInfoStatus); //��Ŀ�� ���۵��� ���� �浹 Ȯ�� 
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
		if (nRet_1 == BD_GOOD) //�������� �̵�
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
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(1204, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 2000;
		}
		break; 	

	case 2100:
		if(nWork_Site == THD_LD_TRAY_PLATE)
		{
			nRet_1 = Chk_WorkRbtRotator_OnOff(IO_OFF); //rotator �����鼭 �̵��Ѵ�
		}
		else //if(nWork_Site == THD_ULD_BUFF)
		{
			nRet_1 = Chk_WorkRbtRotator_OnOff(IO_ON); //rotator �����鼭 �̵��Ѵ�
		}
		if(nRet_1 == RET_GOOD)
		{
			m_bDvcWaitChk_Falg = false;

			// jtkim 20160907 �ӽ�
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
		m_dCurrentPos[2] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[2] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
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
		if (nRet_1 == BD_GOOD) //�������� �̵�
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
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(1206, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 2200;
		}
		break; 	

	case 2300:
		//kwlee 2016.1007 
		//m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_1D_POS]; 
		m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_1D_LD_POS]; 
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, m_dpTargetPosList[2], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{ 						
			m_nPick_Step = 2400;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 2300;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(1207, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 2300;
		}		
		break;


		////////////////////////////////////////////////////////
		// 1 D ���ڵ带 �д´�//�����Ѵ�
		// ���ڵ� ������ ���翡 �ִ´� 
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
		Set_Glipper_OnOff(IO_ON); //�̸� ������ 
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
	//  ����̽��� ���� ���� ��ġ 
	///////////////////////////////////////////////
	case 3000: 
		nRet_1 = FAS_IO.get_out_bit(st_io_info.o_LdUldPickGripCyl,	IO_ON); //IO_ON ������ �ִ� �����̸� ���� ���� ����  
		if(nRet_1 == IO_OFF) 
		{  //�׸��۰� ������ ���� ������ ���翡 �������� �ֱ⶧���� �̸� ������ ������ �Ѵ� 
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
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{ 
			Set_Glipper_OnOff(IO_ON); //�̸� ������ 
			Set_WorkRbtPicker_UpDown(IO_ON); //������ ��Ŀ�� �̸� �����鼭 �̵��Ѵ�
			m_nPick_Step = 3100;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 3000;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(1210, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 3000;
		}		
		break;

	case 3100:
		nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_CHECK, m_nRobot_Z, m_dpTargetPosList[2], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{ 			 
			m_nPick_Step = 3110;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 3000;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(1211, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 3000;
		}		
		break;

	case 3110: //picker down check 
		nRet_1 = Chk_Glipper_OnOff(IO_ON); 
		nRet_2 = Chk_WorkRbtPicker_UpDown(IO_ON); //������ ��Ŀ�� �̸� �����鼭 �̵��Ѵ�
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
	//���� Ŭ���� 
	///////////////////////////////////////////
	case 3200: //���縦 ���´� 
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
			Set_Glipper_OnOff(IO_ON);  //������  
			CTL_Lib.Alarm_Error_Occurrence(1213, dWARNING, m_strAlarmCode);
			m_nPick_Step = 3200;
		}
		break;

	case 3300://james 2016.0827 �׸��� ����̽� üũ ���� ����/����, Gripper Off(Ŭ��������)���� IO_OFF�� ����̽� �ִ»���, �׿ܴ� ON/OFF ������� ��� ����̽� ���� ����
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
// 				st_other_info.strBoxMsg = _T("����̽��� ������ �ּ���. ���Է��� ���귮���� �����ϴ�.");
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
		if(nWork_Site == THD_LD_TRAY_PLATE)  //load tray���� ���縦 ������ 
		{
			m_nPick_Step = 4000;//Load buffer�� gripper�� �̰����� ���� 
		}
		else //if(nWork_Site == THD_ULD_BUFF)//��ε� ���ۿ��� ���縦 ������
		{
			m_nPick_Step = 3500;// 
		} 
		break;

	//////////////////////////////////////////////////////////////////////////////
	// uld buffer�� �׸����� ���縦 ���� ��ŷ�κ����� ���縦 ���°��� �����Ѵ� 
	//////////////////////////////////////////////////////////////////////////////
	case 3500: //��ε� ���� �׸��� ���� 
		FAS_IO.Set_IO_OnOff(st_io_info.o_UldBuffGripOnOffCyl[m_n_FirstTray_Y_Num], IO_ON);//������			 
		m_nPick_Step = 3510;
		break;

	case 3510:
		nRet_1 = FAS_IO.Wait_IO_OnOff(st_io_info.o_UldBuffGripOnOffCyl[m_n_FirstTray_Y_Num], IO_OFF, st_wait_info.nOnWaitTime[WAIT_RBTGLIPPER_ON_OFF]);
		if(nRet_1 == RET_GOOD) //�۾� ������ ���簡 �ִ�
		{
			m_nPick_Step = 3520;
		}
		break;

	case 3520:
		nRet_1 = FAS_IO.Chk_IO_OnOff(st_io_info.i_UldBuffGripDvcExistChk[m_n_FirstTray_Y_Num], IO_ON, IO_STABLE_WAIT, 2000); //IO_STABLE_WAIT == 100ms
		if(nRet_1 == RET_GOOD) //������ �ȵǵ� ����ȭ �ð������� ��ٸ��� 
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
	//�κ�Ʈ up 
	///////////////////////////////////////////
	case 4000:
		m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY];
		nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_START, m_nRobot_Z, m_dpTargetPosList[2], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{ 
			Set_WorkRbtPicker_UpDown(IO_OFF); //������ ��Ŀ�� �ø���  
			m_nPick_Step = 4100 ; //�ð� ������ ���� üũ�� �������� 
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 4000;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(1217, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 4000;
		}		
		break;

	case 4100:
		nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_CHECK,m_nRobot_Z, m_dpTargetPosList[2], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{ 			 
			m_nPick_Step = 4110;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 4000;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(1218, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 4000;
		}		
		break;

	case 4110:
		nRet_1 = Chk_WorkRbtPicker_UpDown(IO_OFF); //������ ��Ŀ�� �̸� �����鼭 �̵��Ѵ�
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

	case 5000: //picker ������ ���¸� Ȯ���Ѵ�
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

				//2017.0312 �̻��ѵ� �� �˶��� ��������> �׳� ����Ÿ�� �̵��ϸ� �ȵǴµ�..
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
						m_npPicker_YesNo[i] = CTL_NO; //�۾��� ��Ŀ ���� Ŭ����

						//�Ʒ� �Լ����� ������� ��ġ ������ �����Ѵ�
						m_npSet_WorkPosYXCPB[0]  = m_npFindWorkPosYXCPB[0] + (i - m_n_FirstPicker_Num); //tray Y Pos ��ġ
						m_npSet_WorkPosYXCPB[1]  = m_npFindWorkPosYXCPB[1]; //tray X Pos ��ġ
						m_npSet_WorkPosYXCPB[3]  = i; //picker ��ġ ����

						clsFunc.Data_Exchange_PickPlace(PICKER_PICK_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //��Ŀ �� Ʈ���� ���� ���� �Լ�						
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
						m_npPicker_YesNo[i] = CTL_NO; //�۾��� ��Ŀ ���� Ŭ����

						//�Ʒ� �Լ����� ������� ��ġ ������ �����Ѵ�
						m_npSet_WorkPosYXCPB[0]  = m_npFindWorkPosYXCPB[0] + (i - m_n_FirstPicker_Num); //tray Y Pos ��ġ
						m_npSet_WorkPosYXCPB[1]  = m_npFindWorkPosYXCPB[1]; //tray X Pos ��ġ
						m_npSet_WorkPosYXCPB[3]  = i; //picker ��ġ ����

						clsFunc.Data_Exchange_PickPlace(PICKER_PICK_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //��Ŀ �� Ʈ���� ���� ���� �Լ�						

						st_count_info.nSSD_Input[0]++;
						// jtkim 20160830
						st_count_info.nInCount[1][0]++; // daily
					
						if (st_lot_info[m_nLotProcessNum].strLotNo == st_picker[m_nRobotSite].st_pcb_info[i].strLotNo)//�������� Lot�� ������
						{
							// ���� �Ǵ� next lot ���� ����
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
							//���� ������ lot ������ ���� ������ �޼��� ��� 
						}
					}
				}
				else if(nWork_Site == THD_ULD_BUFF)
				{
					if(i >= 0 && m_npFindWorkPosYXCPB[0] + (i - m_n_FirstPicker_Num) < st_recipe_info.nLdUldBuffer_Num)
					{
						m_npPicker_YesNo[i] = CTL_NO; //�۾��� ��Ŀ ���� Ŭ����

						//�Ʒ� �Լ����� ������� ��ġ ������ �����Ѵ�
						m_npSet_WorkPosYXCPB[0]  = m_npFindWorkPosYXCPB[0] + (i - m_n_FirstPicker_Num); //tray Y Pos ��ġ
						m_npSet_WorkPosYXCPB[1]  = m_npFindWorkPosYXCPB[1]; //tray X Pos ��ġ
						m_npSet_WorkPosYXCPB[3]  = i; //picker ��ġ ����

						clsFunc.Data_Exchange_PickPlace(PICKER_PICK_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //��Ŀ �� Ʈ���� ���� ���� �Լ�		

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

		if (nRet_1 == RET_GOOD) // 1000 step���� ������ ��Ŀ ������ �°� ��� ������ ������ ��� ��
		{
			//2017.0109
			//kwlee 2017.0117 ���Ƴ���..
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
								// next lot ���� ����
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
			//ȭ�� ���� ����
			m_nDvcRepickCount = 0;
			m_nPartialPickCount = 0;

			m_bDvcWaitChk_Falg = false;

			// jtkim 20150216 conveyor/loader tray ȭ�� ǥ��
			if (st_handler_info.cWndMain != NULL)
			{
				st_handler_info.cWndMain->PostMessage(WM_WORK_DISPLAY, MAIN_LD_PICKER_DISPLAY, 0);
				st_handler_info.cWndMain->PostMessage(WM_WORK_DISPLAY, MAIN_LD_TRAY_DISPLAY, 0);
				st_handler_info.cWndMain->PostMessage(WM_WORK_DISPLAY, MAIN_RETEST_BUF_DISPLAY, 0);
			}

			m_nPick_Step = 6000;
		}
		else if (nRet_1 == RET_ERROR) //������ ��Ŀ�� �Ѱ��� ����� ���� �������� �̰����� �̵�
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



//james 2016.0912 LOT_CURR �� LOT_NEXT �� ��� ���� ��밡���ϵ��� ��� ������ ����Ͽ� ���հ��� 

//PLACE�� nWork_Site �� 5���̴�
//1.THD_LD_BUFF
//2.THD_ULD_1_STACKER
//3.THD_ULD_2_STACKER
//4. THD_REJECT_OUT_1_STACKER
//5. THD_REJECT_OUT_2_STACKER
int CRunLdUldRobot::Process_DVC_Place(int nMode, int nWork_Site, int nTestSite_Pos) //nTestSitePos == TESTSITE_LEFT, TESTSITE_RIGHT ���� �ϳ� 
{//return ���� RET_TRAY_NOT_FIND, RET_PICKER_NOT_FIND, RET_GOOD, RET_PROCEED
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
		Set_WorkRbtPicker_UpDown(IO_OFF); //�̸� ��Ŀ�� ������ �ø���

		m_dCurrentPos[m_nRobot_Z] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[m_nRobot_Z] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
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
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{
			m_bDvcWaitChk_Falg =  false;
			m_nPlace_Step = 1000;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 900;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(1230, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 900;
		}
		break;

	////////////////////////////////////////////////////////////////
	//��Ŀ ���¸� Ȯ�� �� ������ ������ �۾��� ��Ŀ�� ã�´�
	////////////////////////////////////////////////////////////////
	case 1000:
		//2015.0305 �߰� 
		nRet_1 = clsFunc.Check_PickerStatus(0, THD_WORK_RBT, CTL_YES, m_npPicker_YesNo, m_npPicker_Vacuum_Status, m_npPicker_OutputStatus); //���� ������ ���� ���� üũ 
		if(nRet_1 == RET_GOOD)
		{
			m_bDvcWaitChk_Falg = false; //��Ŀ ������ ���� ���°� �����̴�
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


		m_dCurrentPos[2] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[2] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
		{
			m_nPlace_Step = 900;
			break;
		}		 

		//�۾� ���� ��ġ�� ã�´�
		m_n_FirstTray_Y_Num = -1; m_n_FirstTray_Y_Num = -1; m_n_FirstPicker_Num = -1; //flag clear
		for(i = 0; i < m_nPickerPara; i++)
		{
			if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_YES)//��Ŀ�� ���簡 ��� �ִٴ� ������ ����
			{
				m_n_FirstPicker_Num = i;
				i = m_nPickerPara;
			}
		}

		if(m_n_FirstPicker_Num == -1)
		{
			//��Ŀ�� ���̻� �۾��� ���� ����
			m_nPlace_Step = 0;
			nFuncRet = RET_PICKER_NOT_FIND;
			break;
		}

		nRet_1 = clsFunc.Find_TrayWork_Pos(WORK_PLACE, nWork_Site, CTL_NO, m_npFindWorkPosYXCPB, THD_WORK_RBT, st_worker_info.strLotNo_Empty); //���������ۿ����� ���۾������� �ں��� ���´� (4 -> 1 ����) 
		if(nRet_1 == RET_GOOD) //�۾� ������ ���簡 �ִ�
		{
			m_n_FirstTray_Y_Num = m_npFindWorkPosYXCPB[0]; m_n_FirstTray_X_Num = m_npFindWorkPosYXCPB[1];
		}
		else if(nRet_1 == RET_SKIP) //���̻� ���簡 ����
		{	//��� ������ �� ����
			m_nPlace_Step = 0;
			nFuncRet = RET_TRAY_NOT_FIND;
			break;
		}

		nFlag = 0;
		if(m_n_FirstPicker_Num != -1 && m_n_FirstTray_Y_Num != -1 && m_n_FirstTray_X_Num != -1) //�۾��� ������ �� ������ ���� �۾��� �����Ѵ�
		{
			for(i = 0;  i < m_nPickerPara; i++)  //m_n_FirstPicker_Num; y < m_mPickerPara; y++)
			{
				m_npPicker_YesNo[i] = CTL_NO; //�۾��� ��Ŀ���� clear
				if(i < m_n_FirstPicker_Num) continue;

				if(nWork_Site == THD_ULD_1_STACKER || nWork_Site == THD_ULD_2_STACKER || nWork_Site == THD_REJECT_OUT_1_STACKER || nWork_Site == THD_REJECT_OUT_2_STACKER)
				{
					if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_YES)// && st_picker[m_nRobotSite].st_pcb_info[i].nEnable == CTL_YES)
					{
						if(i >= 0 && m_n_FirstTray_Y_Num + (i - m_n_FirstPicker_Num) < st_recipe_info.nTrayY)
						{
							if(st_tray_info[nWork_Site].st_pcb_info[ m_n_FirstTray_Y_Num + (i - m_n_FirstPicker_Num)][m_n_FirstTray_X_Num].nYesNo == CTL_NO) //Ʈ���̿� ���簡  ������ ���� �����ϰ� ��
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
							{//���ۿ� ���簡 ����, ��� ������ ��ġ�϶���  ���縦 ������ �ְ� �Ѵ�.
								m_npPicker_YesNo[i] = CTL_YES;
								nFlag++;
							}
						}
					}
				}
			}
			if(nFlag == 0) //�۾��� ��Ŀ�� �ϳ��� �غ�� �ʾҴ�.
			{ //�̺κ��� �ƿ��� �ȵ� 
				m_nPlace_Step = 0;
				nFuncRet = RET_PICKER_NOT_FIND;//��Ŀ�� ���̻� �۾��� ���� ����
				break;
			}

			//���� �۾� ��ġ�� ����
			m_npFindWorkPosYXCPB[0] = m_n_FirstTray_Y_Num; m_npFindWorkPosYXCPB[1] = m_n_FirstTray_X_Num; m_npFindWorkPosYXCPB[3] = m_n_FirstPicker_Num;

			m_nMove_Flag[0] = CTL_NO; m_nMove_Flag[1] = CTL_NO;

			m_nRotator_Flag = 0; //clear
/*
			if(nWork_Site == THD_LD_BUFF)
			{
				m_nBarcode2dStep = 0;

				m_nPlace_Step = 1100; //������ ��ġ���� 1D ���ڵ带 �а�, ����̽��� ���ۿ� ���� 2D ���ڵ带 �д´� 
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
	// ������ ��ġ���� 1D ���ڵ带 �ϰ�, ����̽��� ���ۿ� ���� 2D ���ڵ带 �д´� 
	////////////////////////////////////////////////////////
	case 1100: 
		m_dCurrentPos[2] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[2] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
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
		if (nRet_1 == BD_GOOD) //�������� �̵�
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
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 1100;
		}
		break; 	

	case 1200:
		m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_1D_POS]; 
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, m_dpTargetPosList[2], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{ 						
			m_nPlace_Step = 1300;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 1200;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 1200;
		}		
		break;


	////////////////////////////////////////////////////////
	// 1 D ���ڵ带 �д´�//�����Ѵ�
	// ���ڵ� ������ ���翡 �ִ´� 
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
	// �۾� �� ��ġ�� X,Y robot �� rotator �Ǹ��� �����Ѵ�
	////////////////////////////////////////////////////////
	case 2000:
		m_dCurrentPos[2] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[2] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
		{
			m_nPlace_Step = 900;
			break;
		}

		if(nWork_Site == THD_LD_BUFF)
		{
			nRet_1 = clsFunc.Check_Robot_Buffer_Clash(TESTSITE_LEFT, THD_WORK_RBT, THD_LD_BUFF, m_n_FirstTray_Y_Num, m_n_FirstPicker_Num, m_nClash_ErrorInfoStatus); //��Ŀ�� ���۵��� ���� �浹 Ȯ�� 
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
				Set_WorkRbtRotator_OnOff(IO_ON); //rotator �����鼭 �̵��Ѵ�
			}
			else //if(nWork_Site == THD_LD_TRAY_PLATE || nWork_Site == THD_ULD_1_STACKER || nWork_Site == THD_ULD_2_STACKER || nWork_Site == THD_REJECT_OUT_1_STACKER || nWork_Site == THD_REJECT_OUT_2_STACKER)
			{
				Set_WorkRbtRotator_OnOff(IO_OFF); //rotator�� Ʈ���̷� ���� �� �ְ� ȸ���ϸ鼭 �̵��Ѵ�
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
		if (nRet_1 == BD_GOOD) //�������� �̵�
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
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(1234, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 2000;
		}
		break; 	

	case 2100:
		if(nWork_Site == THD_LD_BUFF || nWork_Site == THD_ULD_BUFF)
		{
			nRet_1 = Chk_WorkRbtRotator_OnOff(IO_ON); //rotator �����鼭 �̵��Ѵ�
		}
		else //if(nWork_Site == THD_LD_TRAY_PLATE || nWork_Site == THD_ULD_1_STACKER || nWork_Site == THD_ULD_2_STACKER || nWork_Site == THD_REJECT_OUT_1_STACKER || nWork_Site == THD_REJECT_OUT_2_STACKER)
		{
			nRet_1 = Chk_WorkRbtRotator_OnOff(IO_OFF); //rotator�� Ʈ���̷� ���� �� �ְ� ȸ���ϸ鼭 �̵��Ѵ�
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
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{
			m_bDvcWaitChk_Falg =  false;
			m_nPlace_Step = 3000;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 2900;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(1236, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 2900;
		}
		break;


	////////////////////////////////////////////////////////
	// �۾� �� ���縦 ���´� 
	////////////////////////////////////////////////////////
	case 3000: 		
		m_dCurrentPos[2] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[2] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
		{
			m_nPlace_Step = 2900;
			break;
		}

		nRet_1 = clsFunc.Check_PickerStatus(0, THD_WORK_RBT, CTL_YES, m_npPicker_YesNo, m_npPicker_Vacuum_Status, m_npPicker_OutputStatus);
		if(nRet_1 == RET_GOOD)
		{
			m_bDvcWaitChk_Falg = false; //��Ŀ ������ ���� ���°� �����̴�
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

			//rotator �� ���°� �������� Ȯ���Ѵ� 
			// jtkim 20160905
			// nRet_1 = FAS_IO.Chk_IO_OnOff(st_io_info.i_LdBuffGripDvcExistChk[m_n_FirstTray_Y_Num], IO_ON, IO_STABLE_WAIT, IO_STABLE_LIMIT); //IO_STABLE_WAIT == 100ms
			nRet_1 = FAS_IO.Chk_IO_OnOff(st_io_info.i_LdBuffGripDvcExistChk[m_n_FirstTray_Y_Num], IO_ON, IO_STABLE_WAIT, IO_STABLE_LIMIT); 
			if(nRet_1 == RET_PROCEED && st_basic_info.nModeDevice != WITHOUT_DVC) //������ �ȵǵ� ����ȭ �ð������� ��ٸ��� 
			{
				break; 
			} 
			else if(nRet_1 == RET_ERROR && st_basic_info.nModeDevice != WITHOUT_DVC) //with device �� �ƴϸ�  
			{
				m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_LdBuffGripDvcExistChk[m_n_FirstTray_Y_Num]); 
				CTL_Lib.Alarm_Error_Occurrence(1239, dWARNING, m_strAlarmCode);
				break;
			}  

			nRet_1 = FAS_IO.Chk_IO_OnOff(st_io_info.i_LdUldPickRot_90_Chk, IO_ON, IO_STABLE_WAIT, IO_STABLE_LIMIT); //IO_STABLE_WAIT == 100ms
			if(nRet_1 == RET_PROCEED) //������ �ȵǵ� ����ȭ �ð������� ��ٸ��� 
			{
				break; 
			} 
			else if(nRet_1 == RET_ERROR)
			{
				if(nWork_Site == THD_LD_BUFF || nWork_Site == THD_ULD_BUFF)
				{
					Set_WorkRbtRotator_OnOff(IO_ON); //rotator �����鼭 �̵��Ѵ�
				}
				else //if(nWork_Site == THD_LD_TRAY_PLATE || nWork_Site == THD_ULD_1_STACKER || nWork_Site == THD_ULD_2_STACKER || nWork_Site == THD_REJECT_OUT_1_STACKER || nWork_Site == THD_REJECT_OUT_2_STACKER)
				{
					Set_WorkRbtRotator_OnOff(IO_OFF); //rotator�� Ʈ���̷� ���� �� �ְ� ȸ���ϸ鼭 �̵��Ѵ�
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

			//rotator �� ���°� �������� Ȯ���Ѵ� 
			// jtkim 20160905
			// nRet_1 = FAS_IO.Chk_IO_OnOff(st_io_info.i_LdBuffGripDvcExistChk[m_n_FirstTray_Y_Num], IO_ON, IO_STABLE_WAIT, IO_STABLE_LIMIT); //IO_STABLE_WAIT == 100ms
			nRet_1 = FAS_IO.Chk_IO_OnOff(st_io_info.i_UldBuffGripDvcExistChk[m_n_FirstTray_Y_Num], IO_ON, IO_STABLE_WAIT, IO_STABLE_LIMIT); 
			if(nRet_1 == RET_PROCEED && st_basic_info.nModeDevice != WITHOUT_DVC) //������ �ȵǵ� ����ȭ �ð������� ��ٸ��� 
			{
				break; 
			} 
			else if(nRet_1 == RET_ERROR && st_basic_info.nModeDevice != WITHOUT_DVC) //with device �� �ƴϸ�  
			{
				m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_UldBuffGripDvcExistChk[m_n_FirstTray_Y_Num]); 
				CTL_Lib.Alarm_Error_Occurrence(1242, dWARNING, m_strAlarmCode);
				break;
			}  

			nRet_1 = FAS_IO.Chk_IO_OnOff(st_io_info.i_LdUldPickRot_90_Chk, IO_ON, IO_STABLE_WAIT, IO_STABLE_LIMIT); //IO_STABLE_WAIT == 100ms
			if(nRet_1 == RET_PROCEED) //������ �ȵǵ� ����ȭ �ð������� ��ٸ��� 
			{
				break; 
			} 
			else if(nRet_1 == RET_ERROR)
			{
				if(nWork_Site == THD_LD_BUFF || nWork_Site == THD_ULD_BUFF)
				{
					Set_WorkRbtRotator_OnOff(IO_ON); //rotator �����鼭 �̵��Ѵ�
				}
				else //if(nWork_Site == THD_LD_TRAY_PLATE || nWork_Site == THD_ULD_1_STACKER || nWork_Site == THD_ULD_2_STACKER || nWork_Site == THD_REJECT_OUT_1_STACKER || nWork_Site == THD_REJECT_OUT_2_STACKER)
				{
					Set_WorkRbtRotator_OnOff(IO_OFF); //rotator�� Ʈ���̷� ���� �� �ְ� ȸ���ϸ鼭 �̵��Ѵ�
				}

				m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_LdUldPickRot_90_Chk); 
				CTL_Lib.Alarm_Error_Occurrence(1243, dWARNING, m_strAlarmCode);
				break;
			}  
		}
		else //if(nWork_Site == THD_LD_TRAY_PLATE || nWork_Site == THD_ULD_1_STACKER || nWork_Site == THD_ULD_2_STACKER || nWork_Site == THD_REJECT_OUT_1_STACKER || nWork_Site == THD_REJECT_OUT_2_STACKER)
		{
			nRet_1 = FAS_IO.Chk_IO_OnOff(st_io_info.i_LdUldPickRot_0_Chk, IO_ON, IO_STABLE_WAIT, IO_STABLE_LIMIT); //IO_STABLE_WAIT == 100ms
			if(nRet_1 == RET_PROCEED) //������ �ȵǵ� ����ȭ �ð������� ��ٸ��� 
			{
				break;
			} 
			else if(nRet_1 == RET_ERROR)
			{
				if(nWork_Site == THD_LD_BUFF || nWork_Site == THD_ULD_BUFF)
				{
					Set_WorkRbtRotator_OnOff(IO_ON); //rotator �����鼭 �̵��Ѵ�
				}
				else //if(nWork_Site == THD_LD_TRAY_PLATE || nWork_Site == THD_ULD_1_STACKER || nWork_Site == THD_ULD_2_STACKER || nWork_Site == THD_REJECT_OUT_1_STACKER || nWork_Site == THD_REJECT_OUT_2_STACKER)
				{
					Set_WorkRbtRotator_OnOff(IO_OFF); //rotator�� Ʈ���̷� ���� �� �ְ� ȸ���ϸ鼭 �̵��Ѵ�
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
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{ 			
			Set_WorkRbtPicker_UpDown(IO_ON); //������ ��Ŀ�� �̸� �����鼭 �̵��Ѵ�
			m_nPlace_Step = 3100;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 3000;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(1245, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 3000;
		}		
		break;

	case 3100:
		nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_CHECK, m_nRobot_Z, m_dpTargetPosList[2], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{ 			 
			m_nPlace_Step = 3110;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 3000;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(1246, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 3000;
		}		
		break;

	case 3110:
		nRet_1 = Chk_WorkRbtPicker_UpDown(IO_ON); //������ ��Ŀ�� �̸� �����鼭 �̵��Ѵ�
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
		if(nWork_Site == THD_LD_BUFF)// || nWork_Site == THD_ULD_BUFF) //�ε� ���縦 �ε� ���ۿ� ������ 
		{
			m_nPlace_Step = 3300;//Load buffer�� gripper�� �̰����� ���� 
		}
		else //if(nWork_Site == THD_LD_TRAY_PLATE || nWork_Site == THD_ULD_1_STACKER || nWork_Site == THD_ULD_2_STACKER || nWork_Site == THD_REJECT_OUT_1_STACKER || nWork_Site == THD_REJECT_OUT_2_STACKER)
		{
			m_nPlace_Step = 3400;//Ʈ���̿� ���� ����
		} 
		break;

	////////////////////////////////////////////////////////////
	//Load buffer�� gripper�� �̰����� ���� 
	//���� �ܼ�ȭ�� ���� �ε� ������ �׸��۸� �̰����� ���� �Ѵ� 
	////////////////////////////////////////////////////////////
	case 3300:
		FAS_IO.Set_IO_OnOff(st_io_info.o_LdBuffGripOnOffCyl[m_n_FirstTray_Y_Num], IO_OFF);//���Ǹ���			 
		m_nPlace_Step = 3310;
		break;

	case 3310:
		nRet_1 = FAS_IO.Wait_IO_OnOff(st_io_info.o_LdBuffGripOnOffCyl[m_n_FirstTray_Y_Num], IO_OFF, st_wait_info.nOffWaitTime[WAIT_RBTGLIPPER_ON_OFF]);
		if(nRet_1 == RET_GOOD) //�۾� ������ ���簡 �ִ�
		{
			m_nPlace_Step = 3320;
		}
		break;

	case 3320:
		nRet_1 = FAS_IO.Chk_IO_OnOff(st_io_info.i_LdBuffGripDvcExistChk[m_n_FirstTray_Y_Num], IO_ON, IO_STABLE_WAIT, 1000);
		if(nRet_1 == RET_GOOD) //������ �ȵǵ� ����ȭ �ð������� ��ٸ��� 
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
	//�κ�Ʈ�� ���縦 �ε� ���ۿ� ���´� 
	////////////////////////////////////////////////////////////
	case 3400:
		Set_Glipper_OnOff(IO_ON); // ������ 
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

	case 3420://james 2016.0827 �׸��� ����̽� üũ ���� ����/����, Gripper Off(Ŭ��������)���� IO_OFF�� ����̽� �ִ»���, �׿ܴ� ON/OFF ������� ��� ����̽� ���� ����
		nRet_1 = FAS_IO.Chk_IO_OnOff(st_io_info.i_LdUldPickDvcChk, IO_ON, st_wait_info.nOnWaitTime[WAIT_RBTGLIPPER_ON_OFF], st_wait_info.nLimitWaitTime[WAIT_RBTGLIPPER_ON_OFF]); //IO_STABLE_WAIT == 100ms
		if(nRet_1 == RET_GOOD) //������ �ȵǵ� ����ȭ �ð������� ��ٸ��� 
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
	//�κ�Ʈ�� ���� ��ġ�� �ø��� 
	////////////////////////////////////////////////////////////
	case 4000: 		
		nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_START, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{
			Set_WorkRbtPicker_UpDown(IO_OFF); //�̸� ��Ŀ�� ������ �ø���

			m_bDvcWaitChk_Falg =  false;
			m_nPlace_Step = 4010;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 4000;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
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
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{
			m_bDvcWaitChk_Falg =  false;
			m_nPlace_Step = 4100;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 4000;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
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
			m_nPlace_Step = 4200; //������ ��ġ���� 1D ���ڵ带 �ϰ�, ����̽��� ���ۿ� ���� 2D ���ڵ带 �д´� 
		}
		else
		{
			m_nPlace_Step = 5000; 
		}
		break;
/*
	////////////////////////////////////////////////////////
	// ������ ��ġ���� 1D ���ڵ带 �ϰ�, ����̽��� ���ۿ� ���� 2D ���ڵ带 �д´� 
	////////////////////////////////////////////////////////
	case 4200: 
		m_dCurrentPos[2] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[2] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
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
		if (nRet_1 == BD_GOOD) //�������� �̵�
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
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 4200;
		}
		break; 	

	case 4300:
		m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_2D_LD_POS]; 
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, m_dpTargetPosList[2], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{ 						
			m_nPlace_Step = 4400;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 4300;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 4300;
		}		
		break;

	////////////////////////////////////////////////////////
	// 2 D ���ڵ带 �д´�//�����Ѵ�
	// ���ڵ� ������ ���翡 �ִ´� 
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
		m_dCurrentPos[2] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[2] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
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
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{
			m_nPlace_Step = 5000;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 4300;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(1254, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 4300;
		}
		break;
	////////////////////////////////////////////////////////
	// �۾��� �Ϸ��� ���·� ��Ŀ ���¸� Ȯ���Ѵ�
	// ��Ŀ �����͸� ����/Ʈ���̿� data exchange �Ѵ�
	////////////////////////////////////////////////////////
	case 5000: //picker ������ ���¸� Ȯ���Ѵ�
		nRet_1 = clsFunc.Check_PickerStatus(1, THD_WORK_RBT, CTL_NO, m_npPicker_YesNo, m_npPicker_Vacuum_Status, m_npPicker_OutputStatus);
		if (nRet_1 != RET_GOOD) //
		{//���� �ð��� �ش�
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
			//james 2016.0919 �׸����� output ���¸� üũ 

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
						m_npPicker_YesNo[i] = CTL_NO;	//�۾��� ��Ŀ ���� Ŭ����

						//�Ʒ� �Լ����� ������� ��ġ ������ �����Ѵ�
						m_npSet_WorkPosYXCPB[0] = m_npFindWorkPosYXCPB[0] + (i - m_n_FirstPicker_Num); //tray Y Pos ��ġ
						m_npSet_WorkPosYXCPB[1] = m_npFindWorkPosYXCPB[1]; //tray X Pos ��ġ
						m_npSet_WorkPosYXCPB[3]  = i; //picker ��ġ ����

						clsFunc.Data_Exchange_PickPlace(PICKER_PLACE_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //��Ŀ �� Ʈ���� ���� ���� �Լ�
						//�� �Լ����� ��� ������ ���� �� Ŭ���� �Ѵ�
						//st_picker[m_nRobotSite].st_pcb_info[y].nYesNo = CTL_NO;					//��Ŀ ���� ��
						//st_tray_info[nWork_Site].st_pcb_info[m_npFirstPosYXP[0] + (y - m_n_FirstPicker_Num)][m_npFirstPosYXP[1]].nYesNo = CTL_YES; //Ʈ���� ���� �� //��������
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
									// ���� �������� Lot�� ������, // next lot ���� ����

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
										// ���� �������� Lot�� ������
										//st_lot_info[LOT_CURR].nLdCnt++;
										st_count_info.nRejectCount[0][0]++;
									}
									else if(m_nLotProcessNum == LOT_NEXT)
									{
										// next lot ���� ����
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
										// ���� �������� Lot�� ������
										//st_lot_info[LOT_CURR].nLdCnt++;
										st_count_info.nPrimeRejectCount[0][0]++;
									}
									else if(m_nLotProcessNum == LOT_NEXT)
									{
										// next lot ���� ����
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
										// ���� �������� Lot�� ������
										//st_lot_info[LOT_CURR].nLdCnt++;
										st_count_info.nPassCount[0][0]++;
									}
									else if(m_nLotProcessNum == LOT_NEXT)
									{
										// next lot ���� ����
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
										// ���� �������� Lot�� ������
										//st_lot_info[LOT_CURR].nLdCnt++;
										st_count_info.nRejectCount[0][0]++;
									}
									else if(m_nLotProcessNum == LOT_NEXT)
									{
										// next lot ���� ����
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
						m_npPicker_YesNo[i] = CTL_NO;	//�۾��� ��Ŀ ���� Ŭ����

						//�Ʒ� �Լ����� ������� ��ġ ������ �����Ѵ�
						m_npSet_WorkPosYXCPB[0] = m_npFindWorkPosYXCPB[0] + (i - m_n_FirstPicker_Num); //tray Y Pos ��ġ
						m_npSet_WorkPosYXCPB[1] = m_npFindWorkPosYXCPB[1] + (i - m_n_FirstPicker_Num); //tray X Pos ��ġ
						m_npSet_WorkPosYXCPB[3]  = i; //picker ��ġ ����

						clsFunc.Data_Exchange_PickPlace(PICKER_PLACE_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //��Ŀ �� Ʈ���� ���� ���� �Լ�
						//�� �Լ����� ��� ������ ���� �� Ŭ���� �Ѵ�
						//st_picker[m_nRobotSite].st_pcb_info[y].nYesNo = CTL_NO;					//��Ŀ ���� ��
						//st_tray_info[nWork_Site].st_pcb_info[m_npFirstPosYXP[0] + (y - m_n_FirstPicker_Num)][m_npFirstPosYXP[1]].nYesNo = CTL_YES; //Ʈ���� ���� �� //��������						
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

		if (nRet_1 == RET_GOOD) // 1000 step���� ������ ��Ŀ ������ �°� ��� ������ ������ ��� ��
		{
			//ȭ�� ���� ����
			m_bDvcWaitChk_Falg = false;

			// jtkim 20150303 conveyor/loader tray ȭ�� ǥ��
			if (st_handler_info.cWndMain != NULL)
			{
				st_handler_info.cWndMain->PostMessage(WM_WORK_DISPLAY, MAIN_LD_PICKER_DISPLAY, 0);
				st_handler_info.cWndMain->PostMessage(WM_WORK_DISPLAY, MAIN_LD_BUF_PICK_DISPLAY, 0);
				st_handler_info.cWndMain->PostMessage(WM_WORK_DISPLAY, MAIN_COUNT_DISPLAY, 0);
			}
			m_nPlace_Step = 6000;
		}
		else if (nRet_1 == RET_ERROR) //������ ��Ŀ�� �Ѱ��� ����� ���� �������� �̰����� �̵�
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


void CRunLdUldRobot::Set_WorkRbtRotator_OnOff( int nOnOff )//On:90�� Off:0��
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
		//LogFunction(_T("LD_ULD_ROBOT"),_T("90��"),_T("ROTATOR"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("LD_ULD_ROBOT"),_T("90��"),0,_T("ROTATOR"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
	else
	{
		//LogFunction(_T("LD_ULD_ROBOT"),_T("0��"),_T("ROTATOR"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("LD_ULD_ROBOT"),_T("0��"),0,_T("ROTATOR"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
}

int CRunLdUldRobot::Chk_WorkRbtRotator_OnOff( int nOnOff )// On : 90�� ȸ�� Off : 0��
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
				//LogFunction(_T("LD_ULD_ROBOT"),_T("0��"),_T("ROTATOR"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
				//2017.0101
				clsLog.LogFunction(_T("LD_ULD_ROBOT"),_T("0��"),1,_T("ROTATOR"),_T("CYLINDER"),1,strLogKey,strLogData); 
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
				clsLog.LogFunction(_T("LD_ULD_ROBOT"),_T("0��"),1,_T("ROTATOR"),_T("CYLINDER"),1,strLogKey,strLogData); 
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
				//LogFunction(_T("LD_ULD_ROBOT"),_T("90��"),_T("ROTATOR"),_T("CYLINDER"),1,strLogKey,strLogData);//kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("LD_ULD_ROBOT"),_T("90��"),1,_T("ROTATOR"),_T("CYLINDER"),1,strLogKey,strLogData);
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
				clsLog.LogFunction(_T("LD_ULD_ROBOT"),_T("90��"),1,_T("ROTATOR"),_T("CYLINDER"),1,strLogKey,strLogData);
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

int CRunLdUldRobot::Chk_Glipper_OnOff( int nOnOff )//james 2016.0827 �׸��� ����̽� üũ ���� ����/����, Gripper Off(Ŭ��������)���� IO_OFF�� ����̽� �ִ»���, �׿ܴ� ON/OFF ������� ��� ����̽� ���� ����
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
			// 1D ���ڵ� �б� ����̸� 
			m_nRetry = 0;
			

			m_nBarcode1dStep = 100;
			break;

		case 100:
			// work robot z�� ����Ȯ�� 
			m_dCurrentPos[m_nRobot_Z] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
			if(m_dCurrentPos[m_nRobot_Z] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
			{
				// work robot z�� ������ġ�� ���� ������ ������ġ�� �̵� 
				m_nBarcode1dStep = 200;
			}
			else
			{
				// work robot z�� ������ġ�� ������ 
				m_nBarcode1dStep = 1000;
			}
			break;

		case 200:
			nRet[0] = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY], COMI.mn_runspeed_rate);
			if (nRet[0] == BD_GOOD) //�������� �̵�
			{
				m_nBarcode1dStep = 1000;
			}
			else if (nRet[0] == BD_RETRY)
			{
				m_nBarcode1dStep = 200;
			}
			else if (nRet[0] == BD_ERROR || nRet[0] == BD_SAFETY)
			{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
				CTL_Lib.Alarm_Error_Occurrence(1300, dWARNING, st_alarm_info.strCode);

				m_nBarcode1dStep = 200;
			}
			break;

		case 1000:
			m_dCurrentPos[2] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
			if(m_dCurrentPos[2] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
			{
				m_nBarcode1dStep = 200;

				break;
			}

			// plate ��ġ�� tray 1D Barcode ��ġ�� �̵� 
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
				Set_WorkRbtRotator_OnOff(IO_OFF); //rotator �����鼭 �̵��Ѵ�
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

			if (nRet[0] == BD_GOOD) //�������� �̵�
			{
				m_nRotator_Flag = 0;
				m_nBarcode1dStep = 1100;
			}
			else if (nRet[0] == BD_RETRY)
			{
				m_nBarcode1dStep = 1000;
			}
			else if (nRet[0] == BD_ERROR || nRet[0] == BD_SAFETY)
			{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
				CTL_Lib.Alarm_Error_Occurrence(1301, dWARNING, st_alarm_info.strCode);

				m_nBarcode1dStep = 1000;
			}
			break;

		case 1100:
			nRet[1] = Chk_WorkRbtRotator_OnOff(IO_OFF); 
			if(nRet[1] == RET_GOOD)
			{
				// work robot z�� barcode �б� ��ġ�� �̵� 
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

				if (nRet[0] == BD_GOOD) //�������� �̵�
				{ 						
					m_nBarcode1dStep = 1200;
				}
				else if (nRet[0] == BD_RETRY)
				{
					m_nBarcode1dStep = 1100;
				}
				else if (nRet[0] == BD_ERROR || nRet[0] == BD_SAFETY)
				{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
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
				// 1D ���ڵ� �б� ��尡 �ƴϸ� 
				m_nBarcode1dStep = 0;

				return RET_GOOD;
			}

			// ���ڵ� �б� ��û 
			clsBcr1D.m_nCommand = BD_NONE;
			::SendMessage(st_handler_info.hWnd, WM_BARCODE_MSG_1, BARCODE_TRIGGER_1, 0);

			m_nBarcode1dStep = 1300;
			break;

		case 1300:
			if (clsBcr1D.m_nCommand == BD_END)
			{
				// ���ڵ尡 ���������� �о����� 
				nCount = 0;
				//kwlee 2016.1114
				//if (clsBcr1D.m_strBarcode[0] != _T("") && clsBcr1D.m_strBarcode[0] != _T("ERROR"))
				if (clsBcr1D.m_strBarcode[0] == _T("") || clsBcr1D.m_strBarcode[0] == _T("ERROR"))
				{
					// ���ڵ� �����Ͱ� null/error�̸� 
					nCount++;
				}
				//kwlee 2016.1114
				//if (clsBcr1D.m_strBarcode[1] != _T("") && clsBcr1D.m_strBarcode[1] != _T("ERROR"))
				if (clsBcr1D.m_strBarcode[1] == _T("") || clsBcr1D.m_strBarcode[1] == _T("ERROR"))
				{
					// ���ڵ� �����Ͱ� null/error�̸� 
					nCount++;
				}
/*
				if (st_recipe_info.nBarcodeCnt == 2)
				{
					// ��ǰ�� ���ڵ尡 2�� ������  
					if (clsBcr1D.m_strBarcode[1] == _T("") || clsBcr1D.m_strBarcode[1] == _T("ERROR"))
					{
						// ���ڵ� �����Ͱ� null/error�̸� 
						if (!bBarcodeFlag) bBarcodeFlag = TRUE;
					}
				}
*/
				//kwlee 2016.1114
				//if (st_recipe_info.nBarcodeCnt != nCount)
				if (nCount > 0)
				{
					// barcode�� �����̸� retry ���� Ȯ��
					m_nRetry++;
					if (m_nRetry > 2)
					{
						m_nRetry = 0;
						// ���߿� �˶��ڵ� �߰�
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
			// ���ڵ� �б� �ҷ����� Ȯ�� ��û
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
				// barcode retry ���°� �ƴϸ�
				if (clsBcr1D.m_strBarcode[0] == _T("") || clsBcr1D.m_strBarcode[0] == _T("ERROR"))
				{
					// ���ڵ� �����Ͱ� null/error�̸� 
					bBarcodeFlag = TRUE;
				}

				if (st_recipe_info.nBarcodeCnt == 2)
				{
					// ��ǰ�� ���ڵ尡 2�� ������  
					if (clsBcr1D.m_strBarcode[1] == _T("") || clsBcr1D.m_strBarcode[1] == _T("ERROR"))
					{
						// ���ڵ� �����Ͱ� null/error�̸� 
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
					// manual �Է� ����Ȯ��
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
				// barcode retry ��û�̸� 
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
			// work robot z�� ����Ȯ�� 
			m_dCurrentPos[m_nRobot_Z] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
			if(m_dCurrentPos[m_nRobot_Z] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
			{
				// work robot z�� ������ġ�� ���� ������ ������ġ�� �̵� 
				m_nBarcode1dStep = 2100;
			}
			else
			{
				// work robot z�� ������ġ�� ������ 
				m_nBarcode1dStep = 0;

				return RET_GOOD;
			}
			break;

		case 2100:
			nRet[0] = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY], COMI.mn_runspeed_rate);
			if (nRet[0] == BD_GOOD) //�������� �̵�
			{
				m_nBarcode1dStep = 0;

				return RET_GOOD;
			}
			else if (nRet[0] == BD_RETRY)
			{
				m_nBarcode1dStep = 2100;
			}
			else if (nRet[0] == BD_ERROR || nRet[0] == BD_SAFETY)
			{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
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
			// 1D ���ڵ� �б� ����̸� 
			m_nRetry = 0;

			m_nBarcode2dStep = 100;
			break;

		case 100:
			// work robot z�� ����Ȯ�� 
			m_dCurrentPos[m_nRobot_Z] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
			if(m_dCurrentPos[m_nRobot_Z] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
			{
				// work robot z�� ������ġ�� ���� ������ ������ġ�� �̵� 
				m_nBarcode2dStep = 200;
			}
			else
			{
				// work robot z�� ������ġ�� ������ 
				m_nBarcode2dStep = 1000;
			}
			break;

		case 200:
			nRet[0] = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY], COMI.mn_runspeed_rate);
			if (nRet[0] == BD_GOOD) //�������� �̵�
			{
				m_nBarcode2dStep = 1000;
			}
			else if (nRet[0] == BD_RETRY)
			{
				m_nBarcode2dStep = 200;
			}
			else if (nRet[0] == BD_ERROR || nRet[0] == BD_SAFETY)
			{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
				CTL_Lib.Alarm_Error_Occurrence(1400, dWARNING, st_alarm_info.strCode);

				m_nBarcode2dStep = 200;
			}
			break;

		case 1000:
			m_dCurrentPos[2] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
			if(m_dCurrentPos[2] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
			{
				m_nBarcode2dStep = 200;

				break;
			}

// 			if(m_nRotator_Flag == 0) //
// 			{
// 				m_nRotator_Flag = 1;
// 				Set_WorkRbtRotator_OnOff(IO_OFF); //rotator �����鼭 �̵��Ѵ�
// 			}


			// plate ��ġ�� tray 1D Barcode ��ġ�� �̵�  
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

			if (nRet[0] == BD_GOOD) //�������� �̵�
			{
				m_nBarcode2dStep = 1100;
			}
			else if (nRet[0] == BD_RETRY)
			{
				m_nBarcode2dStep = 1000;
			}
			else if (nRet[0] == BD_ERROR || nRet[0] == BD_SAFETY)
			{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
				CTL_Lib.Alarm_Error_Occurrence(1401, dWARNING, st_alarm_info.strCode);

				m_nBarcode2dStep = 1000;
			}
			break;

		case 1100:
			// work robot z�� barcode �б� ��ġ�� �̵� 
			if (nWork_site == THD_LD_BUFF)
			{
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_2D_LD_POS]; 
			}
			else
			{
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_2D_UNLD_POS]; 
			}

			nRet[0] = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, m_dpTargetPosList[2], COMI.mn_runspeed_rate);

			if (nRet[0] == BD_GOOD) //�������� �̵�
			{ 						
				m_nBarcode2dStep = 1200;
			}
			else if (nRet[0] == BD_RETRY)
			{
				m_nBarcode2dStep = 1100;
			}
			else if (nRet[0] == BD_ERROR || nRet[0] == BD_SAFETY)
			{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
				CTL_Lib.Alarm_Error_Occurrence(1402, dWARNING, st_alarm_info.strCode);

				m_nBarcode2dStep = 1100;
			}		
			break;

		case 1200:
			if (st_basic_info.nMode2DBcr == NO)
			{
				// 1D ���ڵ� �б� ��尡 �ƴϸ� 
				m_nBarcode2dStep = 0;

				return RET_GOOD;
			}

			// ���ڵ� �б� ��û 
			clsBcr2D.m_nCommand = BD_NONE;
			::SendMessage(st_handler_info.hWnd, WM_BARCODE_MSG_2, BARCODE_TRIGGER_1, 1);

			m_nBarcode2dStep = 1300;
			break;

		case 1300:
			if (clsBcr2D.m_nCommand == BD_END)
			{
				// ���ڵ尡 ���������� �о����� 
				if ( clsBcr2D.m_strBarcode[0] == _T("") || clsBcr2D.m_strBarcode[0] == _T("ERROR"))
				{
					// ���ڵ� �����Ͱ� null/error�̸� 
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
						// ���߿� �˶��ڵ� �߰�
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
			// ���ڵ� �б� �ҷ����� Ȯ�� ��û
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
				// 1D ���ڵ� �б� ��尡 �ƴϸ� 
				m_nBarcode2dStep = 0;

				return RET_GOOD;
			}
			// ���ڵ� �б� ��û 
			clsBcr2D.m_nCommand = BD_NONE;
			::SendMessage(st_handler_info.hWnd, WM_BARCODE_MSG_2, BARCODE_TRIGGER_1, 1);

			m_nBarcode2dStep = 1420;
			break;

		case 1420:
			if (clsBcr2D.m_nCommand == BD_END)
			{
				// ���ڵ尡 ���������� �о����� 
				if ( clsBcr2D.m_strBarcode[0] == _T("") || clsBcr2D.m_strBarcode[0] == _T("ERROR"))
				{
					// ���ڵ� �����Ͱ� null/error�̸� 
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

			if (nRet[0] == BD_GOOD) //�������� �̵�
			{
				m_nBarcode2dStep = 1410;
			}
			else if (nRet[0] == BD_RETRY)
			{
				m_nBarcode2dStep = 1450;
			}
			else if (nRet[0] == BD_ERROR || nRet[0] == BD_SAFETY)
			{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
				CTL_Lib.Alarm_Error_Occurrence(1600, dWARNING, st_alarm_info.strCode);

				m_nBarcode2dStep = 1450;
			}
			break;

		case 1500:
			if (st_other_info.nConfirmMsg == 0)
			{
				// barcode retry ���°� �ƴϸ�
				// manual �Է� ����Ȯ��
				if (clsBcr2D.m_strBarcode[1] == _T("") || clsBcr2D.m_strBarcode[1] == _T("ERROR"))
				{
					// ���ڵ� �����Ͱ� null/error�̸� 

					//900100 1 0 "BARCODE_READ_ERROR."
					CTL_Lib.Alarm_Error_Occurrence(1633, dWARNING, _T("900100"));
					m_nBarcode2dStep = 1400;
				}
				else
				{
					// manual �Է� ����Ȯ��
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
				// barcode retry ��û�̸� 
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
			// work robot z�� ����Ȯ�� 
			m_dCurrentPos[m_nRobot_Z] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
			if(m_dCurrentPos[m_nRobot_Z] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
			{
				// work robot z�� ������ġ�� ���� ������ ������ġ�� �̵� 
				m_nBarcode2dStep = 2100;
			}
			else
			{
				// work robot z�� ������ġ�� ������ 
				m_nBarcode2dStep = 0;

				return RET_GOOD;
			}
			break;

		case 2100:
			nRet[0] = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY], COMI.mn_runspeed_rate);
			if (nRet[0] == BD_GOOD) //�������� �̵�
			{
				m_nBarcode2dStep = 0;

				return RET_GOOD;
			}
			else if (nRet[0] == BD_RETRY)
			{
				m_nBarcode2dStep = 2100;
			}
			else if (nRet[0] == BD_ERROR || nRet[0] == BD_SAFETY)
			{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
				CTL_Lib.Alarm_Error_Occurrence(1403, dWARNING, st_alarm_info.strCode);

				m_nBarcode2dStep = 2100;
			}
			break;
	}

	return RET_PROCEED;
}