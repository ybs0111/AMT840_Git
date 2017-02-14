// RunLdTrayStackerElevator.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "RunLdTrayStackerElevator.h"
#include "Variable.h"
#include "FastechPublic_IO.h"
#include "CtlBd_Library.h"

//james 2016.0802 
#include "CtlBd_Variable.h"
#include "PublicFunction.h"
#include "ComizoaPublic.h"
#include "LogCtrl.h"
#include "math.h"
#include "LogFromat.h"


// CRunLdTrayStackerElevator
CRunLdTrayStackerElevator clsRunLdTrayStackerElevator;
CRunLdTrayStackerElevator::CRunLdTrayStackerElevator()
{
	m_nRunStep	= -1;
	m_nInitStep	= -1;

	m_nAxisNum = M_LD_ELV;
}

CRunLdTrayStackerElevator::~CRunLdTrayStackerElevator()
{
}

// CLdTrayStackerElevator 멤버 함수
void CRunLdTrayStackerElevator::OnThreadRun(void)
{
	//james 2016.0730
	if (st_handler_info.nSystemLock != FALSE)  return;	// 현재 시스템 Lock 상태인 경우 리턴

	CTL_Lib.Process_Tray_Conveyor_AcMotor(0, THD_LD_STACKER, st_io_info.o_LdStkAcMotOnOff ); //load elevator stacker AC Motor 제어하는 루틴	

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

void CRunLdTrayStackerElevator::OnRunInit(void)
{
	int nRet_1;

	if (st_sync_info.nInit_Flag[THD_LD_STACKER] != INIT_CLEAR)		return;		//INIT_CLEAR 일때만 초기화 작업을 한다. 초기화가 끝나면 INIT_CLEAR -> INIT_READY가 되기 떄문에...
	 
	switch (m_nInitStep)
	{
		case -1:
			//load cv shifter가 먼저 최기화하여 문제 없을때 elevator를 초기화 한다 
			if (st_sync_info.nInit_Flag[THD_LD_CV_STACKER_LEFT] < INIT_READY || st_sync_info.nInit_Flag[THD_LD_CV_STACKER_RIGHT] < INIT_READY) break;  

			if(st_sync_info.nInit_Flag[THD_LD_TRAY_PLATE] < INIT_READY) break;
			if( st_sync_info.nInit_Flag[THD_WORK_TRANSFER] < INIT_READY ) break;

			m_nInitStep = 0;
			break;

		case 0:		

			m_nInitStep = 100;
			break;

		case 100:
			Set_LdStacker_UpDown(IO_OFF); 
			m_nInitStep = 110;
			break;

		case 110:
			nRet_1 = Chk_LdStacker_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nInitStep = 200;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, m_strAlarmCode);
				m_nInitStep = 100;
			}
			break;

		case 200:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ReadyCvyLd12OutTrayPassChk,	IO_OFF); //CV tray pass check  없어야 한다 
			if(nRet_1 == IO_OFF)
			{
				m_nInitStep = 300;
			}
			else
			{//트레이 존재 에러 
				if(nRet_1 == IO_ON) m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_ReadyCvyLd12OutTrayPassChk); 
				else m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_LdStkTrayArrivedChk); 
				CTL_Lib.Alarm_Error_Occurrence(6010, dWARNING, m_strAlarmCode);				
			}
			break;

		case 300:
			nRet_1 = COMI.HomeCheck_Mot(m_nAxisNum, MOT_SENS_ELM, MOT_TIMEOUT) ; //
			if(nRet_1 == BD_GOOD)
			{
				COMI.mn_homechk_flag[m_nAxisNum] = CTL_YES;
				m_nInitStep = 400;
			}
			else if(nRet_1 == BD_ERROR)
			{
				CTL_Lib.mn_run_status = CTL_dWARNING;
				CTL_Lib.Alarm_Error_Occurrence(6020, CTL_Lib.mn_run_status, COMI.strCode);
				m_nInitStep = 200;
			}
			break;

		case 400:
			st_sync_info.nInit_Flag[THD_LD_STACKER] = INIT_READY; 
			 
			m_nInitStep = 0;
			break;	

		default :
			break;

	}
}

void CRunLdTrayStackerElevator::OnRunMove(void) //0816
{

	int nRet_1, nRet_2=0, nRet_3=0;
	int nCount = 0;

	clsFunc.OnThreadFunctionStepTrace(9, m_nRunStep);

	switch (m_nRunStep)
	{ 
		case -1: 

			m_nRunStep = 0;
			break;

		case 0:
			if(st_lot_info[LOT_CURR].nLotStatus == LOT_START)//james 2016.0929 
			{
				m_nLotProcessNum = LOT_CURR;
				m_nRunStep = 100;

			}
			else if(st_lot_info[LOT_NEXT].nLotStatus == LOT_START)
			{
				m_nLotProcessNum = LOT_NEXT;
				m_nRunStep = 100;				
			}	
			break;

		case 100://쉬프터 초기 상태 확인 
			Set_LdStacker_UpDown(IO_OFF); //down 상태 
			m_nRunStep = 110;
			break;

		case 110:
			nRet_1 = Chk_LdStacker_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 120;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(6030, dWARNING, m_strAlarmCode);
				m_nRunStep = 100;
			}
			break; 

		case 120:
			nRet_1 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 	

			if(nRet_1 == BD_GOOD) //로더 플레이트에 트레이가 감지 된 상태 
			{
				m_strAlarmCode.Format(_T("900001")); //900000 1 0 "LOAD_STACKER_PLATE_SD_TRAY_ON_CHECK_ERROR" //kwlee 2016.0902 "900000" ->"900001"
				CTL_Lib.Alarm_Error_Occurrence(6040, dWARNING, m_strAlarmCode);
				break;
			}
			else 
			{
				m_nRunStep = 200;
			}
			break;

		case 200:
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nAxisNum, st_motor_info[m_nAxisNum].d_pos[P_ELV_TRAY_INOUT_POS], COMI.mn_runspeed_rate);  //트레이를 받을 위치로 미리 이동한다 
			if (nRet_1 == BD_GOOD) //좌측으로 이동  
			{			
				m_nRunStep = 300;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nRunStep = 200;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다  
				CTL_Lib.Alarm_Error_Occurrence(6050, dWARNING, st_alarm_info.strCode);
				m_nRunStep = 200;
			}
			break; 

		case 300:
			nRet_1 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 	
			if(nRet_1 == BD_GOOD) //로더 플레이트에 트레이가 감지 된 상태 
			{
				m_strAlarmCode.Format(_T("900001")); //900000 1 0 "LOAD_STACKER_PLATE_SD_TRAY_ON_CHECK_ERROR" //kwlee 2016.0902 "900000" ->"900001"
				CTL_Lib.Alarm_Error_Occurrence(6060, dWARNING, m_strAlarmCode);
			}
			else if(nRet_1 == BD_ERROR)//로더 플레이드에 트레이가 감지 되지않은 상태 
			{
				m_nRunStep = 400;
			}
			break;

		case 400:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayChk,	IO_OFF); //stacker tary 는 처음에는 없어야 한다 
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayArrivedChk,	IO_OFF); //elevtor 안에 트레이 도착 확인하는 센서
			if(nRet_1 == IO_OFF && nRet_2 == IO_OFF )
			{
				m_nRunStep = 1000;
			}
			else
			{//트레이 존재 에러 
				if(nRet_1 == IO_ON) m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_LdStkTrayChk); 
				else m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_LdStkTrayArrivedChk); 
				CTL_Lib.Alarm_Error_Occurrence(6070, dWARNING, m_strAlarmCode);
			}
			break;

		//////////////////////////////////////////////////////////////////////////////////
		// 작업자가 트레이를 메뉴얼로 공급한다 
		//////////////////////////////////////////////////////////////////////////////////
		case 1000:  //load elevator stacker에서 트레이를 요청한 상태 
			if(st_sync_info.nLdPlate_Tray_Supply_Req[THD_LD_STACKER] == CTL_REQ )//플레이트에서 트레이 요청 
			{
				st_sync_info.nLdElv_Tray_Req[THD_LD_STACKER] = CTL_REQ; //lot이 시작되면 CTL_REQ 플레그를 셋하여 elevator에 트레이 공급을 요청한다 		

				m_dwWaitTime[0] = GetCurrentTime();
				m_nRunStep = 2000;
			}
			else
			{
				if(st_lot_info[m_nLotProcessNum].strLotNo == st_tray_info[THD_LD_STACKER].strLotNo) //2016.0912
				{
					if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_TRAY_PLATE] >=  LOT_END
						&& st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_STACKER] == LOT_START)//james 2016.0924 
					{
						st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_STACKER] = LOT_END_START;
						m_nRunStep = 7000;
					}
				}
			}
			break;

		case 2000:
			if(st_sync_info.nLdElv_Tray_Req[THD_LD_STACKER] == CTL_READY)
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayChk,	IO_ON); //elevator stacker tary 가 있어야 한다 
				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayArrivedChk,	IO_ON); //tary가 도착해 있어야 한다 
				nRet_3 = FAS_IO.get_in_bit(st_io_info.i_ReadyCvyLd12OutTrayPassChk,	IO_OFF); 

				if(nRet_1 == IO_ON && nRet_2 == IO_ON  && nRet_3 == IO_OFF)
				{
					st_tray_info[THD_LD_STACKER].nTrayExist = CTL_YES; //트레이존재 정보 셋

					st_sync_info.nConveyor_AcMotorCtl_Req[THD_LD_STACKER] = CTL_READY;

					m_dwWaitTime[0] = GetCurrentTime();
					m_nRunStep = 2100; //james 2016.0929 6000;
				}
				else
				{//트레이 없음  에러 
					if(nRet_1 == IO_ON) m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_LdStkTrayChk); 
					else m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_LdStkTrayArrivedChk); 
					CTL_Lib.Alarm_Error_Occurrence(6080, dWARNING, m_strAlarmCode);
				}
			}
			else
			{
				if(st_lot_info[m_nLotProcessNum].strLotNo == st_tray_info[THD_LD_STACKER].strLotNo) //2016.0912
				{
					if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_TRAY_PLATE] >=  LOT_END
						&& st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_STACKER] == LOT_START)//james 2016.0924 
					{
						st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_STACKER] = LOT_END_START;
						m_nRunStep = 7000;
					}
				}
			}
			break;
			//2016.1228
					//kwlee 2016.1103 자재 모두 투입 후 Stacker 대기 상태 알수 없음.
			/*		else
					{
						nRet_1 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayChk,	IO_OFF); //elevator stacker tary 가 있어야 한다 
						nRet_2 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayArrivedChk,	IO_OFF); //tary가 도착해 있어야 한다 

						if(nRet_1 == IO_OFF && nRet_2 == IO_OFF)
						{
							/*if (st_handler_info.cWndMain != NULL)
							{
								st_other_info.nBuzYesNo = YES;
								st_other_info.strBoxMsg.Format(_T("[LOAD TRAY STACKER ] Please Insert tray."));  
								//st_other_info.strBoxMsg = _T("[LOT END] eject on Unloading #1 Good, Reject Stacker Tray. \r\n Please Remove it.");
								st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
							}
							m_nRunStep = 2010;*/
							//2016.1107
			/*				if(st_lot_info[LOT_CURR].nLotStatus >= LOT_START || st_lot_info[LOT_NEXT].nLotStatus >= LOT_START )
							{
								if(st_lot_info[LOT_CURR].nLot_THD_Status[THD_WORK_RBT] <  LOT_END_START || st_lot_info[LOT_NEXT].nLot_THD_Status[THD_WORK_RBT] <  LOT_END_START)
								{
									if(st_lot_info[LOT_CURR].nLot_THD_Status[THD_WORK_RBT] <  LOT_END_START )
									{
										nRet_1 = FAS_IO.Chk_IO_OnOff(st_io_info.i_LdUldPickDvcChk, IO_ON, IO_STABLE_WAIT*8, 1000); //IO_STABLE_WAIT == 100ms
										if(nRet_1 == RET_GOOD)
										{
											//kwleee 2016.1201
											//if( (st_count_info.nInCount[1][0] + 1) < st_lot_info[LOT_CURR].nQty)           
											if( (st_count_info.nInCount[0][0] + 1) < st_lot_info[LOT_CURR].nQty)           
											{
												if (st_handler_info.cWndMain != NULL)
												{
													st_other_info.nBuzYesNo = YES;
													st_other_info.strBoxMsg.Format(_T("[LOAD TRAY STACKER ] Please Insert tray.")); 
													//st_other_info.strBoxMsg = _T("[LOT END] eject on Unloading #1 Good, Reject Stacker Tray. \r\n Please Remove it.");
													st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
												}

											}
										}
										else if(nRet_1 == RET_PROCEED)
										{
											break;
										}
									}
									else if(st_lot_info[LOT_NEXT].strLotNo == "") //lot 정보가 없으면 리턴 
									{
										break;
									}
									if(st_lot_info[LOT_NEXT].nLot_THD_Status[THD_WORK_RBT] <  LOT_END_START )
									{
										nRet_1 = FAS_IO.Chk_IO_OnOff(st_io_info.i_LdUldPickDvcChk, IO_ON, IO_STABLE_WAIT, 1000); //IO_STABLE_WAIT == 100ms
										if(nRet_1 == RET_GOOD)
										{
											if( (st_count_info.nInCount[1][1] + 1) < st_lot_info[LOT_NEXT].nQty)           
											{
												if (st_handler_info.cWndMain != NULL)
												{
													st_other_info.nBuzYesNo = YES;
													st_other_info.strBoxMsg.Format(_T("[LOAD TRAY STACKER ] Please Insert tray.")); 
													//st_other_info.strBoxMsg = _T("[LOT END] eject on Unloading #1 Good, Reject Stacker Tray. \r\n Please Remove it.");
													st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
												}
											}
										}
										else if(nRet_1 == RET_PROCEED)
										{
											break;
										}
									}
								}
							}
						}
					}
					///
				}
			}
			break;*/

		case 2010:
			if(st_sync_info.nLdElv_Tray_Req[THD_LD_STACKER] == CTL_READY)
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayChk,	IO_ON); //elevator stacker tary 가 있어야 한다 
				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayArrivedChk,	IO_ON); //tary가 도착해 있어야 한다 
				nRet_3 = FAS_IO.get_in_bit(st_io_info.i_ReadyCvyLd12OutTrayPassChk,	IO_OFF); 

				if(nRet_1 == IO_ON && nRet_2 == IO_ON  && nRet_3 == IO_OFF)
				{
					st_tray_info[THD_LD_STACKER].nTrayExist = CTL_YES; //트레이존재 정보 셋

					st_sync_info.nConveyor_AcMotorCtl_Req[THD_LD_STACKER] = CTL_READY;

					m_dwWaitTime[0] = GetCurrentTime();
					m_nRunStep = 2100; //james 2016.0929 6000;
				}
				else
				{//트레이 없음  에러 
					if(nRet_1 == IO_ON) m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_LdStkTrayChk); 
					else m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_LdStkTrayArrivedChk); 
					CTL_Lib.Alarm_Error_Occurrence(6080, dWARNING, m_strAlarmCode);
				}
			}
			else
			{
				if(st_lot_info[m_nLotProcessNum].strLotNo == st_tray_info[THD_LD_STACKER].strLotNo) //2016.0912
				{
					if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_TRAY_PLATE] >=  LOT_END
						&& st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_STACKER] == LOT_START)//james 2016.0924 
					{
						st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_STACKER] = LOT_END_START;
						m_nRunStep = 7000;
					}
				}
			}
			break;

		case 2100:
			if(st_sync_info.nConveyor_AcMotorCtl_Req[THD_LD_STACKER] == CTL_CLEAR)
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayChk,	IO_ON); //elevator stacker tary 가 있어야 한다 
				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayArrivedChk,	IO_ON); //tary가 도착해 있어야 한다 
				nRet_3 = FAS_IO.get_in_bit(st_io_info.i_ReadyCvyLd12OutTrayPassChk,	IO_OFF); 
				if(nRet_1 == IO_ON && nRet_2 == IO_ON  && nRet_3 == IO_OFF)
				{
					m_nRunStep = 6000;
				}
				else
				{
					if(nRet_1 == IO_ON) m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_LdStkTrayChk); 
					else m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_LdStkTrayArrivedChk); 
					CTL_Lib.Alarm_Error_Occurrence(6090, dWARNING, m_strAlarmCode);
				}
			}
			else
			{
				m_dwWaitTime[1] = GetCurrentTime();
				m_dwWaitTime[2] = m_dwWaitTime[1] - m_dwWaitTime[0];
				if(m_dwWaitTime[2] <= 0) 
				{
					m_dwWaitTime[0] = GetCurrentTime();
					break;
				}
				if(m_dwWaitTime[2] < 3000) 
				{
					break;
				}

				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayChk,	IO_ON); //elevator stacker tary 가 있어야 한다 
				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayArrivedChk,	IO_ON); //tary가 도착해 있어야 한다 
				nRet_3 = FAS_IO.get_in_bit(st_io_info.i_ReadyCvyLd12OutTrayPassChk,	IO_OFF); 
				if(nRet_1 == IO_ON && nRet_2 == IO_ON  && nRet_3 == IO_OFF)
				{
					m_nRunStep = 6000;
				}
				else
				{
					if(nRet_1 == IO_ON) m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_LdStkTrayChk); 
					else m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_LdStkTrayArrivedChk); 
					CTL_Lib.Alarm_Error_Occurrence(6100, dWARNING, m_strAlarmCode);
					m_dwWaitTime[0] = GetCurrentTime();
				}
			}
			break;

		//////////////////////////////////////////////////////////////////////////////////
		// Load Plate와 연관하여 작업하는 영역
		//////////////////////////////////////////////////////////////////////////////////
		case 6000: //load plate애서 트레이 공급을 요청했으면 공급한다 
			//lot end 조건을 체크 한다 
			if(st_lot_info[m_nLotProcessNum].strLotNo == st_tray_info[THD_LD_STACKER].strLotNo) //2016.0912 
			{
				if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_TRAY_PLATE] >=  LOT_END
					&& st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_STACKER] == LOT_START)
				{
					st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_STACKER] =  LOT_END_START;
					m_nRunStep = 7000;
					break;
				}
			}//lot end 조건을 체크 한다  

			if(st_sync_info.nLdPlate_Tray_Supply_Req[THD_LD_STACKER] == CTL_REQ) //load plate에서 트레이를 요청한 상태 
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayChk,	IO_ON); //load stacker tray check    				
				if(nRet_1 == IO_ON)
				{
					m_nRetry = 0;

					m_nRunStep = 6100;
				}
				else
				{ //스태커에 트레이가 없으면 비어 있는 것이니 내려 다음 트레이를 LD CV에 요청 한다.
					m_nRunStep = 7000;
				} 
			}
			break;

		case 6050: 
			nRet_1 = CTL_Lib.Elevator_Job_Move_Pos(0, m_nAxisNum,  P_ELV_SUPPLY_OFFSET);			
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 6100;
			}
			break;

		case 6100:
			nRet_1 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 	
			if(nRet_1 == BD_GOOD) //로더 플레이트에 트레이가 감지 된 상태 
			{
				m_nRetry++;
				if (m_nRetry > 2)
				{
					m_nRetry = 0;
					m_strAlarmCode.Format(_T("900001")); //900001 1 0 "LOAD_STACKER_PLATE_SD_TRAY_ON_CHECK_ERROR" //kwlee 2016.0902 "900000" ->"900001"
					CTL_Lib.Alarm_Error_Occurrence(6110, dWARNING, m_strAlarmCode);
				}
				
				m_nRunStep = 6050;
				break;
			}

			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_LdTrayPlateLfOffChk,	IO_ON); //plate off 체크  
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_LdTrayPlateRiOffChk,	IO_ON); //plate off 체크			
			if(nRet_1 == IO_ON && nRet_2 == IO_ON)
			{
				m_nRunStep = 6200;
			}
			else
			{
				if(nRet_1 == IO_ON) m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_LdTrayPlateLfOffChk); 
				else m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_LdTrayPlateRiOffChk); 
				CTL_Lib.Alarm_Error_Occurrence(6120, dWARNING, m_strAlarmCode);
			}
			break;

		case 6200: //트레이를 플레이트에 공급하는 루틴 
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_LdStkUpChk,	IO_ON); //load stacker up check    			
			if(nRet_1 == IO_ON)
			{
				m_nRunStep = 6300;
			}
			else
			{
				m_nRunStep = 6210;
			}
			break;

		case 6210:
			Set_LdStacker_UpDown(IO_ON); 
			m_nRunStep = 6211;
			break;

		case 6211:
			nRet_1 = Chk_LdStacker_UpDown(IO_ON); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 6300;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(6130, dWARNING, m_strAlarmCode);
				m_nRunStep = 6210;
			}
			break; 

		//////////////////////////////////////////////////////////////////////////
		// 엘레베이터 모터를 작업 가능한 위치로 이동 시킨다 
		//////////////////////////////////////////////////////////////////////////
		case 6300: //모터 이동 
			nRet_1 = CTL_Lib.Elevator_Job_Move_Pos(0, m_nAxisNum,  P_ELV_SUPPLY_OFFSET);
			if(nRet_1 == RET_GOOD)
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayChk,	IO_ON); //load stacker tray check    
				nRet_2 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 	
				if(nRet_1 == IO_OFF && nRet_2 != BD_GOOD) //트레이가 스태커에 없는 상태, SD 센서도 감지되지 않은 상태, 트레이가 없는 상태  
				{
					 m_nRunStep = 7000;
				}
				else
				{
					m_nRunStep = 6400;
				}
			}
			break;

		case 6400:
			nRet_1 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 	
			if(nRet_1 == BD_ERROR) //로더 플레이트에 트레이가 감지 된 상태 
			{
				m_strAlarmCode.Format(_T("900001")); //900001 1 0 "LOAD_STACKER_PLATE_SD_TRAY_OFF_CHECK_ERROR"
				CTL_Lib.Alarm_Error_Occurrence(6140, dWARNING, m_strAlarmCode);
				break;
			}

			//////////////////////////////////////////////////////////////////////////
			//load stacker elevator의 트레이 정보를 load plate로 쉬프트 한다 
			clsFunc.Handler_Tray_DataInfo_Shift(0, CTL_YES, THD_LD_STACKER, THD_LD_TRAY_PLATE);  //계속 트레이를 공급하도 반복되어야 하니, 트레이 정보ㅡㄹ 셋팅만 하고 틀리어하지않는다 			
			//clsFunc.Handler_Tray_DataInfo_Shift(2, CTL_YES, THD_LD_STACKER, THD_LD_TRAY_PLATE);  //계속 트레이를 공급하도 반복되어야 하니, 트레이에 자재가 잇다고 셋팅하고, lot정보만 입력한다

			//////////////////////////////////////////////////////////////////////////

			st_sync_info.nLdPlate_Tray_Supply_Req[THD_LD_STACKER] = CTL_READY;
			m_nRunStep = 6500;
			break;

		case 6500:
			//트레이를 적재를 전체 가이드하는 구조로 스태커가 내려가면 플레이트의 트레이가 떨어진다 . 그 위치에서 그냥 대기하고, 다음 요청오면 재 공급하자.
			//트레이가 끼거나, 공급된 상태에서 엘리베이터 모터 동작하면 플레이트 트레이가 지지하는 것이 없어 떨어질수 있음. 모터 제어 철저,.
			m_nRunStep = 6000;
			break; 

		////////////////////////////////////////////////////////////////
		// laod stacker에는 더이상 트레이가 없어 새로 받는 동작을 한다 
		// 엘리베이터를 내려 다시 받을 준비를 한다 
		////////////////////////////////////////////////////////////////
		case 7000: 		
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayArrivedChk,	IO_OFF); //tary가 도착해 있어야 한다 
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ReadyCvyLd12OutTrayPassChk,	IO_OFF); 
			if(nRet_1 == IO_ON || nRet_2 == IO_ON)
			{	if(nRet_1 == IO_ON) m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_LdStkTrayArrivedChk); 
				else m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ReadyCvyLd12OutTrayPassChk); 
				CTL_Lib.Alarm_Error_Occurrence(6150, dWARNING, m_strAlarmCode);
				break;
			}

			nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_START, m_nAxisNum, st_motor_info[m_nAxisNum].d_pos[P_ELV_TRAY_INOUT_POS], COMI.mn_runspeed_rate);   //밑으로 이동한다 
			if (nRet_1 == BD_GOOD) //위로 이동 하면서 SD를 찾는다  
			{		
				Set_LdStacker_UpDown(IO_OFF); 
				m_nRunStep = 7010;
			}
			else if (nRet_1 == BD_RETRY)
			{				 
				m_nRunStep = 7000;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{ 
				CTL_Lib.Alarm_Error_Occurrence(6150, dWARNING, st_alarm_info.strCode);

				if (st_handler_info.cWndList != NULL)  
				{
					//clsMem.OnNormalMessageWrite(_T("UnLoader Alarm : 2000"));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG); 
				}
				m_nRunStep = 7000;
			}
			break;

		case 7010:
			nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_CHECK, m_nAxisNum, st_motor_info[m_nAxisNum].d_pos[P_ELV_TRAY_INOUT_POS], COMI.mn_runspeed_rate);  //2015.0407 james  //SD 위치까지 이동했는데 SD가 감지되지 않으면 트레이가 없는것이다
			if (nRet_1 == BD_GOOD)   
			{	 
				m_nRunStep = 7100;  			 
			}
			else if (nRet_1 == BD_RETRY)
			{		 
				m_nRunStep = 7000;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{ 
				CTL_Lib.Alarm_Error_Occurrence(6160, dWARNING, st_alarm_info.strCode); 
				m_nRunStep = 7000;
			}
			break;

		case 7100:
			nRet_1 = Chk_LdStacker_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 7200;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(6170, dWARNING, m_strAlarmCode);
				m_nRunStep = 7000;
			}
			break;  

		case 7200:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayChk,	IO_OFF); //stacker tary 는 없어야 한다  
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayArrivedChk,	IO_OFF);
			if(nRet_1 == IO_OFF && nRet_2 == IO_OFF)
			{
				st_tray_info[THD_LD_STACKER].nTrayExist = CTL_NO; //트레이존재 정보 클리어
				m_nRunStep = 1000;

				if(st_lot_info[m_nLotProcessNum].strLotNo == st_tray_info[THD_LD_STACKER].strLotNo &&
					st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_STACKER] >=  LOT_END_START)
				{
					st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_STACKER] =  LOT_END;
					m_nRunStep = 0;
				}		
				else if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] ==  LOT_END_START) //james 2016.0930 
				{
					st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_STACKER] =  LOT_END;
					m_nRunStep = 0;
				}
			}
			else //if(nRet_1 == IO_ON) //james test 2016.0828 테스트 용
			{//트레이 존재가 존재한다  		

				if(st_lot_info[m_nLotProcessNum].strLotNo == st_tray_info[THD_LD_STACKER].strLotNo &&
					st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_STACKER] >=  LOT_END_START)
				{
					st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_STACKER] =  LOT_END;
					m_nRunStep = 0;
					break;
				}	
				else if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] ==  LOT_END_START) //james 2016.0930 
				{
					st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_STACKER] =  LOT_END;
					m_nRunStep = 0;
					break;
				}

				/* james 2016.0929 
				st_tray_info[THD_LD_STACKER].nTrayExist = CTL_YES; //트레이존재 정보 클리어
				m_nRunStep = 6000; //james  2016.0912 트레이가 있으면 다시  공급해 보자 1000;  
				*/
				m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_LdStkTrayChk); 				 
				CTL_Lib.Alarm_Error_Occurrence(6180, dWARNING, m_strAlarmCode);
			}			 
			break;

		default:
			break;
	}
}


int CRunLdTrayStackerElevator::Elevator_Job_Move_Pos(int nSite, int nMotNum,  int nTargetPos)
{
	int nFuncRet = RET_PROCEED;
//	int nRet_1, nRet_2;
//	double dCurrentPos;
//	double dPos_1, dPos_3;

	clsFunc.OnThreadFunctionStepTrace(32, m_nStacker_MoveStep);



/////////////////////////////////////////////////////////////////////////////////////////////////
//SD 센서가 기준점을 잡는 센서 이다 
//	센서로  바로 밑이 기준점으로 항상 SD sensor 대기위치로 기구을 셋팅하면 이에따라 제어한다 
//////////////////////////////////////////////////////////////////////////////////////////////////
	//if(m_nStackerMotorMove_UpDown_Flag != 100)
	//{
	//	m_nStackerMotorMove_UpDown_Flag = 1; //
	//}


	//P_ELV_SAFETY		  = 0,//안전 위치 (down limit 위치일것임)
	//P_ELV_UP_LIMIT		    ,  //high rail 까지 빈 트레이 공급 또는 트레이를 받을떄  제일 윗쪽까지 올리면 체크
	//P_ELV_DN_LIMIT		    ,  //평상시는 트레이 받은 후 soflEo dncl tkdyd, 또는 제일 밑면, 트레이 최종 체크 또는 Full로 교체시 내려간다 	
	//P_ELV_SD_SENSOR				, //sd sensor 감지 위치(트레이가 한장 이상 있을때 감지됨. 하나도 없으면 감지되지 않음 	
	//P_ELV_TRAY_INOUT_POS,		//트레이를 배출 또는 입력 가능한 위치(CONVEYOR 등이 동작하여 트레이를 넣고,  빼기가 가능한 위치 
	//SD 센서 기준점으로 부터 이동하는 위치 
	//P_ELV_SUPPLY_OFFSET	,	  // SD pos + 트레이를 작업 가능하게 하는 위치 
	//P_ELV_RECEIVE_OFFSET	, // SD pos + 트레이를 받는 위치 

/*
	if(st_handler_info.nRunStatus != dRUN)
	{   //time out 시간 체크 문제 보완을 위해  
		if(m_bSd_Move_Flag == true) m_bSd_Move_Flag = false; 
	}

	switch(m_nStacker_MoveStep)
	{
		case 0:	
			//모터 이동 조건은 아래 3가지만 동작한다 
			if(nTargetPos == P_ELV_SUPPLY_OFFSET)//트레이를 작업 가능한 영역에 UP하여 공급하는 위치, SD pos 센서 On 감지 후 센서 기준 - 방항으로 벗어난 후 + 방향으로 P_ELV_SUPPLY_OFFSET 티칭만큼 up 한후 모터 동작은 완료하고, 트레이르, 받든다 
			{
				m_nStacker_MoveStep = 1000;
			}			
			else if(nTargetPos == P_ELV_RECEIVE_OFFSET) //작업이 끝난 트레이를 받는 위치, SD pos 센서 On 감지 후 센서 기준 - 방항으로 센서를 벗어난 후 - 방향으로 P_ELV_RECEIVE_OFFSET 티칭만큼 down 한 후 트레이를 받는 위치  
			{
				//nRet_1 = FAS_IO.get_in_bit(st_io_info.i_UnldTrayRail1EmptyTrayFullChk,	IO_OFF);
				nRet_1 = COMI.Get_MotIOSensor(nMotNum, MOT_SENS_SD);
				nRet_2 = COMI.Check_MotPosRange(nMotNum, m_dReference_Pos_Backup, st_motor_info[nMotNum].d_allow); //바로전 셋팅한 기준점이 맞으면 동작 
				dCurrentPos = COMI.Get_MotCurrentPos(nMotNum); 
				if(nRet_1 == BD_ERROR && nRet_2 == BD_GOOD &&  (dCurrentPos > 1 && dCurrentPos < st_motor_info[nMotNum].d_pos[P_ELV_SD_SENSOR])) 
				{ //작업 목적이 기준점 잡는 것이고, 현재 위치가 기준점으로 여겨지면 추가 동작이 필요없으니 바로 리턴하자					
					m_nStacker_MoveStep = 0;
					nFuncRet = RET_GOOD;
				}
				else
				{
					m_nStacker_MoveStep = 1000;
				}
			}
			else 
			{
				//다른 동작은 이 함수를 사용 할 필요 없음 
				//에러 발생 필요 
			}			
			break;

			
		///////////////////////////////////////////////////////////////////////////
		// SD Sensor 기준 위 또는 아래인지 확인하여, 위면 내려가고, 아래면 올라간다 
		///////////////////////////////////////////////////////////////////////////
		case 1000: //기준점 위치 확인 (SD Sensor 대비 high or 같은 위치인지인지, low인지 두가지 위치를 확인한다)			 
			nRet_1 = COMI.Get_MotIOSensor(nMotNum, MOT_SENS_SD); 	
			dCurrentPos = COMI.Get_MotCurrentPos(nMotNum); 
			CTL_Lib.SD_Sensor_Enable(0, nMotNum, CTL_NO); //sd sensor clear			

			if(nRet_1 == BD_GOOD)
			{//SD가 감지되면 위에 있는 상태 
				m_bSd_Move_Flag = false;
				m_nStacker_MoveStep = 6000; //기준점보다 위에 있는 상태로 아래로 내리면서 SD를 지나가기를 기다리자 
			}
			else //if(nRet_1 == BD_ERROR)
			{//SD도 감지되지 않았고, SD 위치보다 낮은 상태로, 위로 올라가면서 SD를 찾는다 
				
				m_bSd_Move_Flag = false;
				m_nStacker_MoveStep = 2000; //traget pos를 이동하는 루틴 
			}	 
			break;

		////////////////////////////////////////////////////////////
		//SD 센서를 찾으면서 위로 이동하다 
		////////////////////////////////////////////////////////////	
		case 2000: 
			if(m_bSd_Move_Flag == false)
			{
				m_dwSdWaitTime[0] = GetTickCount();
				m_bSd_Move_Flag = true;
				CTL_Lib.SD_Sensor_Enable(0, nMotNum, CTL_YES); //sd sensor set
			}
			else if(m_bSd_Move_Flag == true)
			{
				m_dwSdWaitTime[1] = GetTickCount();
				m_dwSdWaitTime[2] = m_dwSdWaitTime[1] - m_dwSdWaitTime[0];
				if(m_dwSdWaitTime[2] > MOT_ELE_MOVE_WAITLIMIT)
				{//limit 시간 지남, 에러 처리후 재 시도하자 
					COMI.Set_MotStop(1, nMotNum) ; //긴급정지 
					m_nStacker_MoveStep = 1000;
					break;
				}
			}

			nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_START, nMotNum, st_motor_info[nMotNum].d_pos[P_ELV_SD_SENSOR], COMI.mn_runspeed_rate);   //제일 밑까지 이동한다 
			if (nRet_1 == BD_GOOD) //위로 이동 하면서 SD를 찾는다  
			{					 				 
				m_nStacker_MoveStep = 2100;
			}
			else if (nRet_1 == BD_RETRY)
			{				 
				m_nStacker_MoveStep = 1000;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다  
				CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);

				if (st_handler_info.cWndList != NULL)  
				{
					//clsMem.OnNormalMessageWrite(_T("UnLoader Alarm : 2000"));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG); 
				}
				m_nStacker_MoveStep = 1000;
			}
			break;

		case 2100:
			nRet_1 = COMI.Get_MotIOSensor(nMotNum, MOT_SENS_SD); 
			if(nRet_1 == BD_GOOD)
			{//정상적으로 체크했다. 
				COMI.Set_MotStop(1, nMotNum) ; //긴급정지 

				st_motor_info[nMotNum].n_sd_mv_chk = 0; //clear
				CTL_Lib.SD_Sensor_Enable(0, nMotNum, CTL_NO); //sd sensor clear

				m_nStacker_MoveStep = 2200;
				break;
			}
			nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_CHECK, nMotNum, st_motor_info[nMotNum].d_pos[P_ELV_SD_SENSOR], COMI.mn_runspeed_rate);  //2015.0407 james  //SD 위치까지 이동했는데 SD가 감지되지 않으면 트레이가 없는것이다
			if (nRet_1 == BD_GOOD)   
			{	//트레이가 없는 상태일것이다, 미리 트레이를 체크가능하여 트레이가 없으면 올라 갈 필요가 없다 
				//이곳에 오면 문제가 있음 
				m_nStacker_MoveStep = 1000; //트레이가 없는 상태 				 
			}
			else if (nRet_1 == BD_RETRY)
			{		 
				m_nStacker_MoveStep = 1000;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다  
				CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);

				////////if (st_handler_info.cWndList != NULL)  
				{
					//clsMem.OnNormalMessageWrite(_T("UnLoader Alarm : 2010"));
					st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG); 
				}
				m_nStacker_MoveStep = 1000;
			}
			break;

		case 2200:
			nRet_1 = COMI.Get_MotIOSensor(nMotNum, MOT_SENS_SD); 
			if(nRet_1 != BD_GOOD)
			{//SD 센서가 감지되지 않으면 에러 
				COMI.Set_MotStop(1, nMotNum) ; //긴급정지 

				m_bSd_Move_Flag = false;
				m_nStacker_MoveStep = 1000; //재시도  루틴  
				break;
			}

			m_dCurrentPos = COMI.Get_MotCurrentPos(nMotNum); 
			if(nTargetPos == P_ELV_SUPPLY_OFFSET)//트레이를 작업 가능한 영역에 UP하여 공급하는 위치, SD pos 센서 On 감지 후 센서 기준 - 방항으로 벗어난 후 + 방향으로 P_ELV_SUPPLY_OFFSET 티칭만큼 up 한후 모터 동작은 완료하고, 트레이르, 받든다 
			{
				m_dTargetPos = fabs(m_dCurrentPos + st_motor_info[nMotNum].d_pos[P_ELV_SUPPLY_OFFSET]); //현재 위치에 + P_ELV_SUPPLY_OFFSET 
			}
			else if(nTargetPos == P_ELV_RECEIVE_OFFSET) //트레이를 받는 동작시 
			{
				m_dTargetPos = fabs(m_dCurrentPos - st_motor_info[nMotNum].d_pos[P_ELV_RECEIVE_OFFSET]); //셋팅한 위치까지 더 내린다 sd 센서가 감지 안될때까지 
			}

			nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_START, nMotNum, m_dTargetPos, COMI.mn_runspeed_rate);   
			if (nRet_1 == BD_GOOD) //조건에 따라 위로/아래로  이동  
			{					 
				m_nStacker_MoveStep = 2210;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nStacker_MoveStep = 1000;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다  
				CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);

				if (st_handler_info.cWndList != NULL)  
				{
					//clsMem.OnNormalMessageWrite(_T("UnLoader Alarm : 2100"));
					st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG); 
				}
				m_nStacker_MoveStep = 1000;
			}
			break;

		case 2210:
			nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_CHECK, nMotNum, m_dTargetPos, COMI.mn_runspeed_rate);  //2015.0407 james  //SD 위치까지 이동했는데 SD가 감지되지 않으면 트레이가 없는것이다
			if (nRet_1 == BD_GOOD)   
			{	//트레이가 없는 상태일것이다, 미리 트레이를 체크가능하여 트레이가 없으면 올라 갈 필요가 없다 
				//이곳에 오면 문제가 있음 
				m_nStacker_MoveStep = 2300; //트레이가 있는 상태 				 
			}
			else if (nRet_1 == BD_RETRY)
			{		 
				m_nStacker_MoveStep = 1000;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다  
				CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);

				if (st_handler_info.cWndList != NULL)  
				{
					//clsMem.OnNormalMessageWrite(_T("UnLoader Alarm : 2010"));
					st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG); 
				}
				m_nStacker_MoveStep = 1000;
			}
			break;

		case 2300:
			if(nTargetPos == P_ELV_SUPPLY_OFFSET)//트레이를 작업 가능한 영역에 UP하여 공급하는 위치, SD pos 센서 On 감지 후 센서 기준 - 방항으로 벗어난 후 + 방향으로 P_ELV_SUPPLY_OFFSET 티칭만큼 up 한후 모터 동작은 완료하고, 트레이르, 받든다 
			{
				nRet_1 = COMI.Get_MotIOSensor(nMotNum, MOT_SENS_SD); 
				if(nRet_1 == BD_GOOD)
				{//정상적으로 체크했다. 					
					m_nStacker_MoveStep = 2400; // 루틴   
				}
				else
				{//error
					m_nStacker_MoveStep = 1000; //재시도  루틴   
				}
			}			
			else if(nTargetPos == P_ELV_RECEIVE_OFFSET) //작업이 끝난 트레이를 받는 위치, SD pos 센서 On 감지 후 센서 기준 - 방항으로 센서를 벗어난 후 - 방향으로 P_ELV_RECEIVE_OFFSET 티칭만큼 down 한 후 트레이를 받는 위치  
			{
				nRet_1 = COMI.Get_MotIOSensor(nMotNum, MOT_SENS_SD); 
				if(nRet_1 == BD_ERROR)
				{//정상적으로 감지되지 않았다. 					
					m_nStacker_MoveStep = 2400; // 루틴   
				}
				else
				{//error
					m_nStacker_MoveStep = 1000; //재시도  루틴   
				}
			}
			break;

		case 2400:
			m_nStacker_MoveStep = 2500;
			break;

		case 2500:
			m_dCurrentPos = COMI.Get_MotCurrentPos(nMotNum); 
			if(nTargetPos == P_ELV_SUPPLY_OFFSET)//트레이를 작업 가능한 영역에 UP하여 공급하는 위치, SD pos 센서 On 감지 후 센서 기준 - 방항으로 벗어난 후 + 방향으로 P_ELV_SUPPLY_OFFSET 티칭만큼 up 한후 모터 동작은 완료하고, 트레이르, 받든다 
			{
				m_dSupply_Pos_Backup = fabs(m_dCurrentPos + st_motor_info[nMotNum].d_pos[P_ELV_SUPPLY_OFFSET]); //현재 위치에 + P_ELV_SUPPLY_OFFSET 
			}
			else if(nTargetPos == P_ELV_RECEIVE_OFFSET) //트레이를 받는 동작시 
			{
				m_dReceive_Pos_Backup = fabs(m_dCurrentPos - st_motor_info[nMotNum].d_pos[P_ELV_RECEIVE_OFFSET]); //셋팅한 위치까지 더 내린다 sd 센서가 감지 안될때까지 
			} 

			//m_dReference_Pos_Backup

			CTL_Lib.SD_Sensor_Enable(0, nMotNum, CTL_NO); //sd sensor clear
			m_nStacker_MoveStep = 0;
			nFuncRet = RET_GOOD;
			break; 

		//
		////////////////////////////////////////////////////////////
		//SD 센서를 감지하고 있으니 아래로 내려가자 
		////////////////////////////////////////////////////////////	
		case 6000: 
			if(m_bSd_Move_Flag == false)
			{
				m_dwSdWaitTime[0] = GetTickCount();
				m_bSd_Move_Flag = true;
				CTL_Lib.SD_Sensor_Enable(0, nMotNum, CTL_NO); //sd sensor set
			}
			else if(m_bSd_Move_Flag == true)
			{
				m_dwSdWaitTime[1] = GetTickCount();
				m_dwSdWaitTime[2] = m_dwSdWaitTime[1] - m_dwSdWaitTime[0];
				if(m_dwSdWaitTime[2] > MOT_ELE_MOVE_WAITLIMIT)
				{//limit 시간 지남, 에러 처리후 재 시도하자 
					COMI.Set_MotStop(1, nMotNum) ; //긴급정지 
					m_nStacker_MoveStep = 1000;
					break;
				}
			}

			nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_START, nMotNum, st_motor_info[nMotNum].d_pos[P_ELV_DN_LIMIT], COMI.mn_runspeed_rate/3);   //밑으로 이동한다 
			if (nRet_1 == BD_GOOD) //위로 이동 하면서 SD를 찾는다  
			{					 				 
				m_nStacker_MoveStep = 6100;
			}
			else if (nRet_1 == BD_RETRY)
			{				 
				m_nStacker_MoveStep = 1000;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다  
				CTL_Lib.Alarm_Error_Occurrence(274, dWARNING, st_alarm_info.strCode);

				if (st_handler_info.cWndList != NULL)  
				{
					//clsMem.OnNormalMessageWrite(_T("UnLoader Alarm : 2000"));
					st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG); 
				}
				m_nStacker_MoveStep = 1000;
			}
			break;

		case 6100:
			nRet_1 = COMI.Get_MotIOSensor(nMotNum, MOT_SENS_SD); 
			if(nRet_1 == BD_ERROR)
			{//정상적으로 체크했다. 
				COMI.Set_MotStop(1, nMotNum) ; //긴급정지 			
				CTL_Lib.SD_Sensor_Enable(0, nMotNum, CTL_NO); //sd sensor clear

				m_nStacker_MoveStep = 6200;
				break;
			}
			nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_CHECK, nMotNum, st_motor_info[nMotNum].d_pos[P_ELV_DN_LIMIT], COMI.mn_runspeed_rate);  //2015.0407 james  //SD 위치까지 이동했는데 SD가 감지되지 않으면 트레이가 없는것이다
			if (nRet_1 == BD_GOOD)   
			{	 
				m_nStacker_MoveStep = 1000; //트레이가 없는 상태 				 
			}
			else if (nRet_1 == BD_RETRY)
			{		 
				m_nStacker_MoveStep = 1000;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다  
				CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);

				////////if (st_handler_info.cWndList != NULL)  
				{
					//clsMem.OnNormalMessageWrite(_T("UnLoader Alarm : 2010"));
					st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG); 
				}
				m_nStacker_MoveStep = 1000;
			}
			break;

		case 6200:
			nRet_1 = COMI.Get_MotIOSensor(nMotNum, MOT_SENS_SD); 
			if(nRet_1 == BD_ERROR)
			{//SD 센서가 감지되지 않으면 정상적으로 내려운 상태  
				COMI.Set_MotStop(1, nMotNum) ; //긴급정지 

				m_bSd_Move_Flag = false;
				m_nStacker_MoveStep = 1000; //이제 다시 위로 올라가 정해진 루틴으로 SD 기준 위치에 맞춘다    
			}
			else
			{
				m_nStacker_MoveStep = 1000; //이제 다시 위로 올라가 정해진 루틴으로 SD 기준 위치에 맞춘다    
			}
			break;
	}	 
*/
	return nFuncRet;
}




void CRunLdTrayStackerElevator::Set_LdStacker_UpDown( int nOnOff )
{
	CString strLogKey[10];
	CString	strLogData[10];

	strLogKey[0] = _T("Mode Start");
	strLogData[0].Format(_T("%d"),0);

	m_bStackerUpDownFlag	= false;
	m_dwWaitStackerUpDown[0]	= GetCurrentTime();

	FAS_IO.set_out_bit( st_io_info.o_LdStkUpCyl, nOnOff);
	FAS_IO.set_out_bit( st_io_info.o_LdStkDnCyl, !nOnOff);

	///kwlee 2016.0812
	if (nOnOff == IO_ON)
	{
		//LogFunction(_T("LD_TRAY_STK_ELEVATOR"),_T("UP"),_T("LD_STACKER"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("LD_TRAY_STK_ELEVATOR"),_T("UP"),0,_T("LD_STACKER"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
	else
	{
		//LogFunction(_T("LD_TRAY_STK_ELEVATOR"),_T("DOWN"),_T("LD_STACKER"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("LD_TRAY_STK_ELEVATOR"),_T("DOWN"),0,_T("LD_STACKER"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
}

int CRunLdTrayStackerElevator::Chk_LdStacker_UpDown( int nOnOff )
{
//	800112 1 0 "LOADING_STACKER_UP_CHECK"
//	800113 1 0 "LOADING_STACKER_DOWN_CHECK"
	int nWaitTime = WAIT_STACKER_UP_DN;

	CString strLogKey[10];
	CString	strLogData[10];

	strLogKey[0] = _T("Mode End");
	strLogData[0].Format(_T("%d"),0);


	if (nOnOff == IO_OFF)
	{
		if (m_bStackerUpDownFlag == false &&
			FAS_IO.get_in_bit(st_io_info.i_LdStkUpChk,	IO_OFF)	== IO_OFF &&
			FAS_IO.get_in_bit(st_io_info.i_LdStkDnChk,	IO_ON)	== IO_ON)
		{
			m_bStackerUpDownFlag		= true;
			m_dwWaitStackerUpDown[0]	= GetCurrentTime();
		}
		else if (m_bStackerUpDownFlag == true &&
				 FAS_IO.get_in_bit(st_io_info.i_LdStkUpChk,	IO_OFF)	== IO_OFF &&
				 FAS_IO.get_in_bit(st_io_info.i_LdStkDnChk,	IO_ON)	== IO_ON)
		{
			m_dwWaitStackerUpDown[1] = GetCurrentTime();
			m_dwWaitStackerUpDown[2] = m_dwWaitStackerUpDown[1] - m_dwWaitStackerUpDown[0];

			if (m_dwWaitStackerUpDown[2] <= 0)
			{
				m_dwWaitStackerUpDown[0] = GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwWaitStackerUpDown[2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
			{
				//LogFunction(_T("LD_TRAY_STK_ELEVATOR"),_T("DOWN"),_T("LD_STACKER"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("LD_TRAY_STK_ELEVATOR"),_T("DOWN"),1,_T("LD_STACKER"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_GOOD;
			}
		}
		else
		{
			m_dwWaitStackerUpDown[1] = GetCurrentTime();
			m_dwWaitStackerUpDown[2] = m_dwWaitStackerUpDown[1] - m_dwWaitStackerUpDown[0];

			if (m_dwWaitStackerUpDown[2] <= 0)
			{
				m_dwWaitStackerUpDown[0] = GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwWaitStackerUpDown[2] > (DWORD)st_wait_info.nLimitWaitTime[nWaitTime])
			{
				m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_LdStkDnChk); 
				//2017.0109
				clsLog.LogFunction(_T("LD_TRAY_STK_ELEVATOR"),_T("DOWN"),1,_T("LD_STACKER"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_ERROR;
			}
		}
	}
	else
	{
		if (m_bStackerUpDownFlag == false &&
			FAS_IO.get_in_bit(st_io_info.i_LdStkUpChk,	IO_ON)	== IO_ON &&
			FAS_IO.get_in_bit(st_io_info.i_LdStkDnChk,	IO_OFF)	== IO_OFF)
		{
			m_bStackerUpDownFlag			= true;
			m_dwWaitStackerUpDown[0]	= GetCurrentTime();
		}
		else if (m_bStackerUpDownFlag == true &&
				 FAS_IO.get_in_bit(st_io_info.i_LdStkUpChk,	IO_ON)	== IO_ON &&
				 FAS_IO.get_in_bit(st_io_info.i_LdStkDnChk,	IO_OFF)	== IO_OFF)
		{
			m_dwWaitStackerUpDown[1]	= GetCurrentTime();
			m_dwWaitStackerUpDown[2]	= m_dwWaitStackerUpDown[1] - m_dwWaitStackerUpDown[0];

			if (m_dwWaitStackerUpDown[2] <= 0)
			{
				m_dwWaitStackerUpDown[0]	= GetCurrentTime();
				return RET_PROCEED;
			}

			if(m_dwWaitStackerUpDown[2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
			{
				//LogFunction(_T("LD_TRAY_STK_ELEVATOR"),_T("UP"),_T("LD_STACKER"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("LD_TRAY_STK_ELEVATOR"),_T("UP"),1,_T("LD_STACKER"),_T("CYLINDER"),1,strLogKey,strLogData); 
				return RET_GOOD;
			}
		}
		else
		{
			m_dwWaitStackerUpDown[1]	= GetCurrentTime();
			m_dwWaitStackerUpDown[2]	= m_dwWaitStackerUpDown[1] - m_dwWaitStackerUpDown[0];

			if (m_dwWaitStackerUpDown[2] <= 0)
			{
				m_dwWaitStackerUpDown[0]	= GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwWaitStackerUpDown[2] > (DWORD)st_wait_info.nLimitWaitTime[nWaitTime])
			{
				m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_LdStkUpChk); 
				//2017.0109
				clsLog.LogFunction(_T("LD_TRAY_STK_ELEVATOR"),_T("UP"),1,_T("LD_STACKER"),_T("CYLINDER"),1,strLogKey,strLogData); 
				return RET_ERROR;
			}
		}
	}

	return RET_PROCEED;
}

