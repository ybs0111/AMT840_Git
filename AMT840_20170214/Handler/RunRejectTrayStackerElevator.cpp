// RejectTrayStackerElevator.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "RunRejectTrayStackerElevator.h"
#include "Variable.h"
#include "FastechPublic_IO.h"
#include "CtlBd_Library.h"
#include "LogCtrl.h"


#include "ComizoaPublic.h"
#include "CtlBd_Variable.h"
#include "PublicFunction.h"
 #include "LogFromat.h"


// CRunRejectTrayStackerElevator
CRunRejectTrayStackerElevator clsRunRejectTrayStackerElevator;
CRunRejectTrayStackerElevator::CRunRejectTrayStackerElevator()
{
	m_nRunStep	= -1;
	m_nInitStep	= -1;

	m_nAxisNum = M_RJ_EMPTY_ELV;
}

CRunRejectTrayStackerElevator::~CRunRejectTrayStackerElevator()
{
}


// CRunRejectTrayStackerElevator 멤버 함수
void CRunRejectTrayStackerElevator::OnThreadRun(void)
{
	if (st_handler_info.nSystemLock != FALSE)  return;	// 현재 시스템 Lock 상태인 경우 리턴
	Process_Tray_Insert(0);
	CTL_Lib.Process_Tray_Conveyor_AcMotor(0, THD_REJECT_EMPTY_STACKER, st_io_info.o_RejEmptyCvyStkAcMotOnOff ); // Conveyor AC Motor 제어하는 루틴		 

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

void CRunRejectTrayStackerElevator::OnRunInit(void)
{
	int nRet_1;

	if (st_sync_info.nInit_Flag[THD_REJECT_EMPTY_STACKER] != INIT_CLEAR)		return;		//INIT_CLEAR 일때만 초기화 작업을 한다. 초기화가 끝나면 INIT_CLEAR -> INIT_READY가 되기 떄문에...
	
	switch (m_nInitStep)
	{
		case -1:
			 
			if(st_sync_info.nInit_Flag[THD_REJECT_TRANSFER] < INIT_READY) break;

			m_nInitStep = 0;
			break;

		case 0:		

			m_nInitStep = 100;
			break;

		case 100: //down
			Set_StackerTray_UpDown(IO_OFF); 
			m_nInitStep = 110;
			break;

		case 110:
			nRet_1 = Chk_StackerTray_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nInitStep = 200;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(7500, dWARNING, m_strAlarmCode);
				m_nInitStep = 100;
			}
			break;

		case 200:
			nRet_1 = COMI.HomeCheck_Mot(m_nAxisNum, MOT_SENS_ELM, MOT_TIMEOUT) ; //
			if(nRet_1 == BD_GOOD)
			{
				COMI.mn_homechk_flag[m_nAxisNum] = CTL_YES;
				m_nInitStep = 300;
			}
			else if(nRet_1 == BD_ERROR)
			{
				CTL_Lib.mn_run_status = CTL_dWARNING;
				CTL_Lib.Alarm_Error_Occurrence(7510, CTL_Lib.mn_run_status, COMI.strCode);
				m_nInitStep = 200;
			}			
			break;

		case 300:
			m_nInitStep = 900;
			break;

		case 900:
			st_sync_info.nInit_Flag[THD_REJECT_EMPTY_STACKER] = INIT_READY; 
			 
			m_nInitStep = 0;
			break;	

		default :
			break;

	}
}

void CRunRejectTrayStackerElevator::OnRunMove(void)
{
		int nRet_1, nRet_2=0, nRet_3=0;
	int nCount = 0;

	clsFunc.OnThreadFunctionStepTrace(12, m_nRunStep);	

	switch (m_nRunStep)
	{ 
		case -1: 
			st_sync_info.nConveyor_AcMotorCtl_Req[THD_REJECT_EMPTY_STACKER] = CTL_FREE; //conveyor ac motor 정지
			m_nRunStep = 0;
			break;

		case 0:
			if(st_lot_info[LOT_CURR].nLotStatus == LOT_START)
			{
				m_nLotProcessNum = LOT_CURR;
				//m_nRunStep = 100; 
			}
			else if(st_lot_info[LOT_NEXT].nLotStatus == LOT_START)
			{
				m_nLotProcessNum = LOT_NEXT;
				//m_nRunStep = 100; 
			}		 

			if(st_lot_info[LOT_CURR].nLotStatus == LOT_START || st_lot_info[LOT_NEXT].nLotStatus == LOT_START )
			{
				st_sync_info.nConveyor_AcMotorCtl_Req[THD_REJECT_EMPTY_STACKER] = CTL_FREE; //conveyor ac motor 정지
				m_nRunStep = 100;
			}			
			break;

		case 100:// down
			Set_StackerTray_UpDown(IO_OFF); 
			m_nRunStep = 110;
			break;

		case 110:
			nRet_1 = Chk_StackerTray_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 120;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(7520, dWARNING, m_strAlarmCode);
				m_nRunStep = 100;
			}
			break; 

		case 120:
			nRet_1 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 	
			if(nRet_1 == BD_GOOD) //로더 플레이트에 트레이가 감지 된 상태 
			{
				m_strAlarmCode.Format(_T("900010")); //900010 1 0 "REJECT_EMPTY_STACKER_PLATE_SD_TRAY_OFF_CHECK_ERROR"
				CTL_Lib.Alarm_Error_Occurrence(7520, dWARNING, m_strAlarmCode);
				break;
			}
			else 
			{
				m_nRunStep = 900;
				//kwlee 2016.1221 Reject Empty Stacker Eelvator Up 상태에서 Arrive Sensor 감지 시 Alarm 발생
// 				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyCvyInTrayExistChk,	IO_ON); //트레이가 공급된 상태
// 				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyTrayArrivedChk,		IO_ON);
// 				
// 				if (nRet_1 == IO_ON || nRet_2 == IO_ON)
// 				{
// 					if(nRet_1 == IO_ON) m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_RejEmptyCvyInTrayExistChk); 
// 					else m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_RejEmptyTrayArrivedChk); 
// 					CTL_Lib.Alarm_Error_Occurrence(7521, dWARNING, m_strAlarmCode);
// 				}
// 				else
// 				{
// 					m_nRunStep = 900;
// 				}
// 				//
			}
			break;

		case 900:
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nAxisNum, st_motor_info[m_nAxisNum].d_pos[P_ELV_TRAY_INOUT_POS], COMI.mn_runspeed_rate);  //트레이를 받을 위치로 미리 이동한다 
			if (nRet_1 == BD_GOOD) //좌측으로 이동  
			{			
				m_nRunStep = 1000;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nRunStep = 900;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다  
				CTL_Lib.Alarm_Error_Occurrence(7530, dWARNING, st_alarm_info.strCode);
				m_nRunStep = 900;
			}
			break; 

		case 300:
		
			m_nRunStep = 1000;
			break;

		//////////////////////////////////////////////////////////////////////////////////
		// empty stacker와 연관하여 작업하는 영역
		//////////////////////////////////////////////////////////////////////////////////
		case 1000:  //load elevator stacker에서 트레이를 요청한 상태  
			/* james 2016.0929 
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyCvyInTrayExistChk,	IO_ON); //트레이가 공급된 상태
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyStkTrayExistChk,	IO_ON);
			if(nRet_1 == IO_ON && nRet_2== IO_ON)
			{
				m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_RejEmptyStkTrayExistChk); 
				CTL_Lib.Alarm_Error_Occurrence(m_nRunStep, dWARNING, m_strAlarmCode);
			}
			else if(nRet_1== IO_ON || nRet_2 == IO_ON)
			{
				m_bCVTrayStackerArrivedFlag = false;
				m_nRunStep = 1100;
			}*/

			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyCvyInTrayExistChk,	IO_ON); //트레이가 공급된 상태
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyStkTrayExistChk,	IO_ON);
			nRet_3 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyTrayArrivedChk,	IO_ON);
			if( nRet_1== IO_ON || nRet_2 == IO_ON || nRet_3 == IO_ON)
			{
				m_bCVTrayStackerArrivedFlag = false;
				m_nRunStep = 1100;
			}
			//kwlee 2016.1021
			else
			{
				//kwlee 2016.1019
				if (st_handler_info.cWndMain != NULL)
				{
					st_other_info.nBuzYesNo = YES;
					st_other_info.strBoxMsg.Format(_T("[REJECT EMPTY STACKER TRAY ] Please Insert tray."));  
					//st_other_info.strBoxMsg = _T("[LOT END] eject on Unloading #1 Good, Reject Stacker Tray. \r\n Please Remove it.");
					st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
				}
			}
			///
			break;

		case 1100:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyCvyInTrayExistChk,	IO_ON); //트레이가 공급된 상태
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyStkTrayExistChk,	IO_ON);
			nRet_3 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyTrayArrivedChk,	IO_ON);
			if( nRet_1== IO_ON || nRet_2 == IO_ON || nRet_3 == IO_ON)
			{
				if(m_bCVTrayStackerArrivedFlag == false)
				{
					m_bCVTrayStackerArrivedFlag = true;
					m_dwCVTrayStackerArrivedWaitTime[0]	= GetCurrentTime();
				}
				else if (m_bCVTrayStackerArrivedFlag == true)
				{
					m_dwCVTrayStackerArrivedWaitTime[1]	= GetCurrentTime();
					m_dwCVTrayStackerArrivedWaitTime[2]	= m_dwCVTrayStackerArrivedWaitTime[1] - m_dwCVTrayStackerArrivedWaitTime[0];
					if (m_dwCVTrayStackerArrivedWaitTime[2] <= 0)
					{
						m_bCVTrayStackerArrivedFlag = false;
						m_dwCVTrayStackerArrivedWaitTime[0]	= GetCurrentTime(); 
						m_nRunStep = 1000;
						break;
					}
					if(m_dwCVTrayStackerArrivedWaitTime[2] > 100) //100ms 안정식간까지 감지되었으면 센서 오류을 줄이기 위해
					{
						m_bCVTrayStackerArrivedFlag = false;
						m_nRunStep = 1200;
					}
				}
			}
			else
			{
				m_nRunStep = 1000;
			}
			break;

		case 1200:
			nRet_1 = COMI.Check_MotPosRange(m_nAxisNum, st_motor_info[m_nAxisNum].d_pos[P_ELV_TRAY_INOUT_POS], st_motor_info[m_nAxisNum].d_allow); 
			if(nRet_1 == BD_GOOD)
			{
				st_sync_info.nConveyor_AcMotorCtl_Req[THD_REJECT_EMPTY_STACKER] = CTL_REQ; //conveyor ac motor start
				m_bCVTrayStackerArrivedFlag = false;
				m_nRunStep = 1210;
			}
			else
			{
				//m_nRunStep = 900;
				//kwlee 2016.1221 Reject Empty Stacker Eelvator Up 상태에서 Arrive Sensor 감지 시 Alarm 발생
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyCvyInTrayExistChk,	IO_ON); //트레이가 공급된 상태
				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyTrayArrivedChk,		IO_ON);

				if (nRet_1 == IO_ON || nRet_2 == IO_ON)
				{
					if(nRet_1 == IO_ON) m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_RejEmptyCvyInTrayExistChk); 
					else m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_RejEmptyTrayArrivedChk); 
					CTL_Lib.Alarm_Error_Occurrence(7521, dWARNING, m_strAlarmCode);
				}
				else
				{
					m_nRunStep = 900;
				}
				//
			}			
			break;

		case 1210:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyTrayArrivedChk,	IO_ON); //트레이가 공급된 상태
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyStkTrayExistChk,	IO_ON);
			if(nRet_1 == IO_ON && nRet_2 == IO_ON )
			{
				st_sync_info.nConveyor_AcMotorCtl_Req[THD_REJECT_EMPTY_STACKER] = CTL_READY; //ac motor 정지
				m_dwWaitTime[0] = GetCurrentTime();
				m_nRunStep = 1220; //james 2016.0929 1300;
			}
			else
			{
				//트레이가 정상적으로 공급되지 않은 상태로 리미트 시간 체크  
				if(m_bCVTrayStackerArrivedFlag == false)
				{
					m_dwCVTrayStackerArrivedWaitTime[0] = GetCurrentTime();
					m_bCVTrayStackerArrivedFlag = true;
				}
				else if(m_bCVTrayStackerArrivedFlag == true)
				{
					m_dwCVTrayStackerArrivedWaitTime[1] = GetCurrentTime();
					m_dwCVTrayStackerArrivedWaitTime[2] = m_dwCVTrayStackerArrivedWaitTime[1] - m_dwCVTrayStackerArrivedWaitTime[0];
					if(m_dwCVTrayStackerArrivedWaitTime[2] <= 0) m_dwCVTrayStackerArrivedWaitTime[0] = GetCurrentTime();
					if(m_dwCVTrayStackerArrivedWaitTime[2] > IO_CONVEYOR_WAIT_LIMIT) //10 초 동안 리미트 설정, 10초 안에는 도착해야 한다 
					{	//error 
// 						m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_RejEmptyTrayArrivedChk); 
// 						CTL_Lib.Alarm_Error_Occurrence(7540, dWARNING, m_strAlarmCode); 

						//에러가 발생했으니 AC Motor를 즉시 정지 시킨다 
						st_sync_info.nConveyor_AcMotorCtl_Req[THD_REJECT_EMPTY_STACKER] = CTL_FREE;  

						m_bCVTrayStackerArrivedFlag = false; //처음으로 다시가 재 시도 

						if (st_handler_info.cWndMain != NULL)
						{
							st_other_info.nBuzYesNo = YES;
							st_other_info.strBoxMsg = _T("There are Reject_empty teray on Reject conveyor.");
							st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
						}

						m_nRunStep = 1200;
					}
				}
			}
			break;

		case 1220: //james 2016.0929 
			if(st_sync_info.nConveyor_AcMotorCtl_Req[THD_REJECT_EMPTY_STACKER] == CTL_CLEAR)
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyTrayArrivedChk,	IO_ON); //트레이가 공급된 상태
				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyStkTrayExistChk,	IO_ON);
				if(nRet_1 == IO_ON && nRet_2 == IO_ON )
				{
					m_nRunStep = 1300;
				}
				else
				{
					if(nRet_1 == IO_OFF) m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_RejEmptyTrayArrivedChk); 
					else m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_RejEmptyStkTrayExistChk); 
					CTL_Lib.Alarm_Error_Occurrence(7550, dWARNING, m_strAlarmCode);
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
				if(m_dwWaitTime[2] < 5000) 
				{
					break;
				}

				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyTrayArrivedChk,	IO_ON); //트레이가 공급된 상태
				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyStkTrayExistChk,	IO_ON);
				if(nRet_1 == IO_ON && nRet_2 == IO_ON )
				{
					m_nRunStep = 1300;
				}
				else
				{
					if(nRet_1 == IO_OFF) m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_RejEmptyTrayArrivedChk); 
					else m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_RejEmptyStkTrayExistChk); 
					CTL_Lib.Alarm_Error_Occurrence(7560, dWARNING, m_strAlarmCode);
					m_dwWaitTime[0] = GetCurrentTime();
				}
			}
			break;

		case 1300:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyTrayArrivedChk,	IO_ON); //트레이가 공급된 상태
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyStkTrayExistChk,	IO_ON);
			if(nRet_1 == IO_ON && nRet_2 == IO_ON )
			{
				clsFunc.Handler_Tray_DataInfo_Shift(0, CTL_YES, THD_REJECT_EMPTY_STACKER, THD_REJECT_EMPTY_STACKER); //트레이 정보를 reject empty cv -> reject empty stacker로 쉬프트 한다 
				m_nRunStep = 2000;
			}
			else
			{
				if(nRet_1 == IO_OFF) m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_RejEmptyTrayArrivedChk); 
				else m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_RejEmptyStkTrayExistChk); 
				CTL_Lib.Alarm_Error_Occurrence(7570, dWARNING, m_strAlarmCode); 
			}
			break;

		case 2000: //미리 작업할 위치에 대기시킨다 
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyTrayArrivedChk,	IO_ON);
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyStkTrayExistChk,	IO_ON);
			if(nRet_1 == IO_ON && nRet_2 == IO_ON )
			{
				Set_StackerTray_UpDown(IO_ON); //up
				m_nRunStep = 2010;
			}
			else
			{
				if(nRet_1 == IO_OFF) m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_RejEmptyTrayArrivedChk); 
				else m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_RejEmptyStkTrayExistChk);  
				CTL_Lib.Alarm_Error_Occurrence(7580, dWARNING, m_strAlarmCode); 
			}
			break;

		case 2010:
			nRet_1 = Chk_StackerTray_UpDown(IO_ON); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 3000;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(7590, dWARNING, m_strAlarmCode);
				m_nRunStep = 2000;
			}
			break;

		case 3000:
			nRet_1 = CTL_Lib.Elevator_Job_Move_Pos(0, m_nAxisNum,  P_ELV_SUPPLY_OFFSET); //트레이를 트랜스퍼에서 받는다 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 4000;
			}
			break;

		//////////////////////////////////////////
		// 트랜스퍼에서 트레이 공급을 요청받았다 
		/////////////////////////////////////////
		case 4000:
			if(st_sync_info.nRejectTransfer_Req[THD_REJECT_EMPTY_STACKER][0] == CTL_REQ) //트랜스퍼에서 empty stacker에 빈 트레이를 놓을 수 있게 요청한다 
			{//st_sync_info.nRejectTransfer_Req[THD_REJECT_EMPTY_STACKER][1] == TRAY_PICK or TRAY_PLACE			
				m_nRunStep = 4100;
			}		
			else //james 2016.0915 lot end 조건 체크 
			{
				if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_REJECT_EMPTY_STACKER] == LOT_END)
				{
					m_nRunStep = 0;
					break;
				}

				if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_REJECT_TRANSFER] ==  LOT_END) //james 2016.0915
				{
					if(st_lot_info[m_nLotProcessNum].nLotStatus >= LOT_START && st_lot_info[LOT_NEXT].nLotStatus >= LOT_START )//james 2016.0915 
					{
						st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_REJECT_EMPTY_STACKER] = LOT_END; //변경 없이 작업을 이어서 진행 
					}
					else 
					{
						m_nRunStep = 9000;
					}
				}				
			}
			break;

		case 4100://트레이를 플레이트에 공급하는 루틴 
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyStkUpChk,	IO_ON); //load stacker up check    
			if(nRet_1 == IO_ON)
			{
				m_nRunStep = 4200;
			}
			else
			{
				m_nRunStep = 4110;
			}
			break;

		case 4110:
			Set_StackerTray_UpDown(IO_ON); 
			m_nRunStep = 4120;
			break;

		case 4120:
			nRet_1 = Chk_StackerTray_UpDown(IO_ON); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 4200;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(7600, dWARNING, m_strAlarmCode);
				m_nRunStep = 4110;
			}
			break;

		/////////////////////////////////////////////////////////////////////////
		// 트레이 작업 방법에 따라 조건체크 (Trasfer 기준 Pick or Place 인지 판별)
		/////////////////////////////////////////////////////////////////////////
		case 4200:
			if(st_sync_info.nRejectTransfer_Req[THD_REJECT_EMPTY_STACKER][1] == WORK_PICK)
			{//최소 하나이상의 트레이가 empty stacker에 존재해야 한다 
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyStkTrayExistChk,	IO_ON); //load stacker up check    
				if(nRet_1 == IO_ON)
				{
					m_nWork_Site_Pos = P_ELV_SUPPLY_OFFSET;
					m_nRunStep = 4300;
				}
				else
				{
					//m_nRunStep = 8000; //james 2016.0926
					//kwlee 2016.1221 Reject Empty Stacker Eelvator Up 상태에서 Arrive Sensor 감지 시 Alarm 발생
					nRet_1 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyCvyInTrayExistChk,	IO_ON); //트레이가 공급된 상태
					nRet_2 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyTrayArrivedChk,		IO_ON);

					if (nRet_1 == IO_ON || nRet_2 == IO_ON)
					{
						if(nRet_1 == IO_ON) m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_RejEmptyCvyInTrayExistChk); 
						else m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_RejEmptyTrayArrivedChk); 
						CTL_Lib.Alarm_Error_Occurrence(7600, dWARNING, m_strAlarmCode);
					}
					else
					{
						m_nRunStep = 8000; //james 2016.0926
					}
					//
					break;

					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_RejEmptyStkTrayExistChk); 
					CTL_Lib.Alarm_Error_Occurrence(7610, dWARNING, m_strAlarmCode); 
				} 				
			}
			else //if(st_sync_info.nRejectTransfer_Req[THD_REJECT_EMPTY_STACKER][1] == TRAY_PLACE)
			{//받는 동작으로 비어 있어도 됨 

				m_nWork_Site_Pos = P_ELV_RECEIVE_OFFSET;
				m_nRunStep = 4300;
			}
			break;
			 
		//////////////////////////////////////////////////////////////////////////
		// 엘레베이터 모터를 작업 가능한 위치로 이동 시킨다 
		//////////////////////////////////////////////////////////////////////////
		case 4300: //모터 이동 
			nRet_1 = CTL_Lib.Elevator_Job_Move_Pos(0, m_nAxisNum,  m_nWork_Site_Pos); //트레이를 트랜스퍼에서 받는다 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 5000;
			}
			break;

		case 5000:			
			nRet_1 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 	
			if(st_sync_info.nRejectTransfer_Req[THD_REJECT_EMPTY_STACKER][1] == WORK_PICK) //
			{
				if(nRet_1 != BD_GOOD) //로더 플레이트에 트레이가 감지 안된 상태 
				{
					m_strAlarmCode.Format(_T("900009")); //900009 1 0 "REJECT_EMPTY_STACKER_PLATE_SD_TRAY_ON_CHECK_ERROR"
					CTL_Lib.Alarm_Error_Occurrence(7620, dWARNING, m_strAlarmCode);
					m_nRunStep = 4300;//2016.0923 
					break;
				}
			}
			else //if(st_sync_info.nRejectTransfer_Req[THD_REJECT_EMPTY_STACKER][1] == TRAY_PLACE)
			{
				if(nRet_1 == BD_GOOD) //리젝 플레이트에 트레이가 감지 된 상태 
				{
					m_strAlarmCode.Format(_T("900010")); //900010 1 0 "REJECT_EMPTY_STACKER_PLATE_SD_TRAY_OFF_CHECK_ERROR"
					CTL_Lib.Alarm_Error_Occurrence(7630, dWARNING, m_strAlarmCode);
					m_nRunStep = 4300;
					break;
				}
			}
			

			//샛팅시 조건은 이후 확인 필요 
			//stacker tray 1장 층 두께 :25mm
			//stacker tray 2장이상 스택시 층 간격 : 17mm

			st_sync_info.nRejectTransfer_Req[THD_REJECT_EMPTY_STACKER][0] = CTL_READY;
			m_nRunStep = 6000;
			break;

		/////////////////////////////////////////////
		// 트랜스퍼가 최종작업을 완료하길 기다린다 
		/////////////////////////////////////////////
		case 6000:
			if(st_sync_info.nRejectTransfer_Req[THD_REJECT_EMPTY_STACKER][0] == CTL_FREE) //트레스퍼 pick or place 트레이 작업 완료 
			{
				if(st_sync_info.nRejectTransfer_Req[THD_REJECT_EMPTY_STACKER][1] == WORK_PICK)
				{
					nRet_1 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 	
					if(nRet_1 == BD_ERROR) //트레이가 감지되지않은 상태 
					{
						st_sync_info.nRejectTransfer_Req[THD_REJECT_EMPTY_STACKER][0] = CTL_CLEAR;
						st_sync_info.nRejectTransfer_Req[THD_REJECT_EMPTY_STACKER][1] = CTL_CLEAR;

						m_nRunStep = 7000;
					}
					else //if(nRet_1 == BD_GOOD) //플레이트에 트레이가 감지 된 상태 
					{
						m_strAlarmCode.Format(_T("900010")); //900010 1 0 "REJECT_EMPTY_STACKER_PLATE_SD_TRAY_OFF_CHECK_ERROR"
						CTL_Lib.Alarm_Error_Occurrence(7640, dWARNING, m_strAlarmCode); 
					} 
				}
				else //if(st_sync_info.nRejectTransfer_Req[THD_REJECT_EMPTY_STACKER][1] == TRAY_PLACE)
				{
					nRet_1 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 	
					if(nRet_1 == BD_GOOD)//플레이트에 트레이가 감지 된 상태 
					{
						st_sync_info.nRejectTransfer_Req[THD_REJECT_EMPTY_STACKER][0] = CTL_CLEAR;
						st_sync_info.nRejectTransfer_Req[THD_REJECT_EMPTY_STACKER][1] = CTL_CLEAR;
						m_nRunStep = 7000;
					}
					if(nRet_1 == BD_ERROR) //플레이트에 트레이가 감지 안된 상태 
					{
						m_strAlarmCode.Format(_T("900009")); //900009 1 0 "REJECT_EMPTY_STACKER_PLATE_SD_TRAY_ON_CHECK_ERROR"
						CTL_Lib.Alarm_Error_Occurrence(7650, dWARNING, m_strAlarmCode); 
					}
				}				
			}
			break;

		////////////////////////////////
		// reject empty tray는 공급위치를 기본으로 준비하자 , 계속 리젝트레이를 공급해야 하니
		////////////////////////////////
		case 7000:
			nRet_1 = CTL_Lib.Elevator_Job_Move_Pos(0, m_nAxisNum,  P_ELV_SUPPLY_OFFSET); //트레이를 트랜스퍼에서 받는다 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 4000;
			}
			break; 

		case 8000: //james 2016.0926
			Set_StackerTray_UpDown(IO_OFF); 
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nAxisNum, st_motor_info[m_nAxisNum].d_pos[P_ELV_TRAY_INOUT_POS], COMI.mn_runspeed_rate);  //트레이를 받을 위치로 미리 이동한다 
			if (nRet_1 == BD_GOOD) //좌측으로 이동  
			{			
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyTrayArrivedChk,	IO_ON);
				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyStkTrayExistChk,	IO_ON);
				if(nRet_1 == IO_ON || nRet_2 == IO_ON )
				{
					if(nRet_1 == IO_OFF) m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_RejEmptyTrayArrivedChk); 
					else m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_RejEmptyStkTrayExistChk);  
					CTL_Lib.Alarm_Error_Occurrence(7660, dWARNING, m_strAlarmCode); 
					break;
				}
				else
				{
					st_tray_info[THD_REJECT_EMPTY_STACKER].nTrayExist = CTL_NO; //james 2016.0929 
					m_nRunStep = 1000;
				}

				/* james 2016.0929 
				m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_RejEmptyStkTrayExistChk); 
				CTL_Lib.Alarm_Error_Occurrence(m_nRunStep, dWARNING, m_strAlarmCode); 
				m_nRunStep = 1000;*/
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nRunStep = 8000;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다  
				CTL_Lib.Alarm_Error_Occurrence(7670, dWARNING, st_alarm_info.strCode);
				m_nRunStep = 8000;
			}
			break; 

		//////////////////////////////////////////////////////////
		// lot end시 작업 
		//////////////////////////////////////////////////////////
		case 9000://down
			Set_StackerTray_UpDown(IO_OFF); 
			m_nRunStep = 9010;
			break;

		case 9010:
			nRet_1 = Chk_StackerTray_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				//m_nRunStep = 9100;
				//kwlee 2016.1221 Reject Empty Stacker Eelvator Up 상태에서 Arrive Sensor 감지 시 Alarm 발생
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyCvyInTrayExistChk,	IO_ON); //트레이가 공급된 상태
				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyTrayArrivedChk,		IO_ON);

				if (nRet_1 == IO_ON || nRet_2 == IO_ON)
				{
					if(nRet_1 == IO_ON) m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_RejEmptyCvyInTrayExistChk); 
					else m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_RejEmptyTrayArrivedChk); 
					CTL_Lib.Alarm_Error_Occurrence(7600, dWARNING, m_strAlarmCode);
				}
				else
				{
					m_nRunStep = 9100;
				}
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(7680, dWARNING, m_strAlarmCode);
				m_nRunStep = 9100;
			}
			break;

		case 9100:
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nAxisNum, st_motor_info[m_nAxisNum].d_pos[P_ELV_DN_LIMIT], COMI.mn_runspeed_rate);  //left로 미리 이동해 있는다 
			if (nRet_1 == BD_GOOD) //좌측으로 이동  
			{			
				st_tray_info[THD_REJECT_EMPTY_STACKER].nTrayExist = CTL_NO; //james 2016.0929

				st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_REJECT_EMPTY_STACKER] = LOT_END; //변경 없이 작업을 이어서 진행 
				m_nRunStep = 0;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nRunStep = 9100;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다  
				CTL_Lib.Alarm_Error_Occurrence(7690, dWARNING, st_alarm_info.strCode);
				m_nRunStep = 9100;
			}
			break; 

		default:
			break;
	}
}


void CRunRejectTrayStackerElevator::Set_StackerTray_UpDown( int nOnOff )
{
	CString strLogKey[10];
	CString	strLogData[10];

	strLogKey[0] = _T("Mode Start");
	strLogData[0].Format(_T("%d"),0);

	m_bStackerUpDown = true;
	m_dwWaitStackerUpDown[0] = GetCurrentTime();

	FAS_IO.set_out_bit( st_io_info.o_RejEmptyStkUpCyl, nOnOff);
	FAS_IO.set_out_bit( st_io_info.o_RejEmptyStkDnCyl, !nOnOff);

	///kwlee 2016.0812
	if (nOnOff == IO_ON)
	{
		//LogFunction(_T("REJECT_TRAY_STK_ELATOR"),_T("UP"),_T("REJECT_STK"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("REJECT_TRAY_STK_ELATOR"),_T("UP"),0,_T("REJECT_STK"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
	else
	{
		//LogFunction(_T("REJECT_TRAY_STK_ELATOR"),_T("DOWN"),_T("REJECT_STK"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("REJECT_TRAY_STK_ELATOR"),_T("DOWN"),0,_T("REJECT_STK"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
}

int CRunRejectTrayStackerElevator::Chk_StackerTray_UpDown( int nOnOff )
{
	CString strLogKey[10];
	CString	strLogData[10];

	strLogKey[0] = _T("Mode End");
	strLogData[0].Format(_T("%d"),0);

//	800513 1 0 "REJECT_LOAD_STACKER_DOWN_CHECK"
//	800513 1 0 "REJECT_LOAD_STACKER_DOWN_CHECK"
	int nWaitTime = WAIT_STACKER_UP_DN;

	if (nOnOff == IO_OFF)
	{
		if (m_bStackerUpDown == false &&
			FAS_IO.get_in_bit(st_io_info.i_RejEmptyStkUpChk,	IO_OFF)	== IO_OFF &&
			FAS_IO.get_in_bit(st_io_info.i_RejEmptyStkDnChk,	IO_ON)	== IO_ON)
		{
			m_bStackerUpDown		= true;
			m_dwWaitStackerUpDown[0]	= GetCurrentTime();
		}
		else if (m_bStackerUpDown == true &&
				 FAS_IO.get_in_bit(st_io_info.i_RejEmptyStkUpChk,	IO_OFF)	== IO_OFF &&
				 FAS_IO.get_in_bit(st_io_info.i_RejEmptyStkDnChk,	IO_ON)	== IO_ON)
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
				//LogFunction(_T("REJECT_TRAY_STK_ELATOR"),_T("DOWN"),_T("REJECT_STK"),_T("CYLINDER"),1,strLogKey,strLogData);//kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("REJECT_TRAY_STK_ELATOR"),_T("DOWN"),1,_T("REJECT_STK"),_T("CYLINDER"),1,strLogKey,strLogData);
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
				m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_RejEmptyStkDnChk); 
				//2017.0109
				clsLog.LogFunction(_T("REJECT_TRAY_STK_ELATOR"),_T("DOWN"),1,_T("REJECT_STK"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_ERROR;
			}
		}
	}
	else
	{
		if (m_bStackerUpDown == false &&
			FAS_IO.get_in_bit(st_io_info.i_RejEmptyStkUpChk,	IO_ON)	== IO_ON &&
			FAS_IO.get_in_bit(st_io_info.i_RejEmptyStkDnChk,	IO_OFF)	== IO_OFF)
		{
			m_bStackerUpDown			= true;
			m_dwWaitStackerUpDown[0]	= GetCurrentTime();
		}
		else if (m_bStackerUpDown == true &&
				 FAS_IO.get_in_bit(st_io_info.i_RejEmptyStkUpChk,	IO_ON)	== IO_ON &&
				 FAS_IO.get_in_bit(st_io_info.i_RejEmptyStkDnChk,	IO_OFF)	== IO_OFF)
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
				//LogFunction(_T("REJECT_TRAY_STK_ELATOR"),_T("UP"),_T("REJECT_STK"),_T("CYLINDER"),1,strLogKey,strLogData);//kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("REJECT_TRAY_STK_ELATOR"),_T("UP"),1,_T("REJECT_STK"),_T("CYLINDER"),1,strLogKey,strLogData);
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
				m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_RejEmptyStkUpChk); 
				//2017.0109
				clsLog.LogFunction(_T("REJECT_TRAY_STK_ELATOR"),_T("UP"),1,_T("REJECT_STK"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_ERROR;
			}
		}
	}

	return RET_PROCEED;
}


// james 2016.0924 reject tray를 conveyor ac motor 요청  
void CRunRejectTrayStackerElevator::Process_Tray_Insert(int n_Mode)
{
	int nFuncRet = RET_PROCEED;
	int nRet_1, nRet_2, nRet_3;	 

	int nTHD_Num;
//	int nActionSite;
//	int n_IO_TrayIn_Num[2];
//	int n_IO_TrayPassExist_Num[2];
  
	 
	nTHD_Num = THD_REJECT_EMPTY_STACKER;

	switch (m_nInsertStep)
	{
		case -1:	

			m_nInsertStep = 0;
			break;

		case 0: 
			m_nInsertStep = 1000; 
			break;  

		case 1000:
			//st_tray_info[nTHD_Num].nTrayExist = CTL_YES;

			if(st_tray_info[nTHD_Num].nTrayExist == CTL_YES)
			{ //이미 트레이가 들어와 있는 상태로 더이상 조건을 체크하면 이중으로 셋팅 됨 
				break;
			}

			/////////////////////////////////////////////////////////////////
			// 작업자가 트레이를 입구에 넣은 상태
			/////////////////////////////////////////////////////////////////
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyCvyInTrayExistChk,	IO_ON);
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyTrayArrivedChk,	IO_ON);//트레이가 이미 쉬프터에 끝까지 장착된 상태, 작업자가 한번에 뒤까지 밀어넣은 상태 
			if(nRet_1 == IO_ON || nRet_2 == IO_ON)  
			{
				if(m_bTrayInFlag == false)
				{
					m_dwTrayInWaitTime[0] = GetCurrentTime();
					m_bTrayInFlag = true;
				}
				else if(m_bTrayInFlag == true)
				{
					m_dwTrayInWaitTime[1] = GetCurrentTime();
					m_dwTrayInWaitTime[2] = m_dwTrayInWaitTime[1] - m_dwTrayInWaitTime[0];
					if(m_dwTrayInWaitTime[2] <= 0) m_dwTrayInWaitTime[0] = GetCurrentTime();
					if(m_dwTrayInWaitTime[2] > IO_STABLE_WAIT) //2초 동안은 조건이 해제되도 계속 돌린다. 킬착시키기 위해 
					{
						m_bAcMotorPassFlag = false;
						st_sync_info.nConveyor_AcMotorCtl_Req[nTHD_Num] = CTL_REQ; //ac  motor start  
						m_nInsertStep = 1100;						
					}
				}
			}
			else 
			{//감지 되지안거나, 깜박이면 플레그 클리어 
				m_bTrayInFlag= false;
			}//

			
			break;

		case 1100: 

			m_bAcMotorPassFlag = false;
			m_nInsertStep  = 2000;	
			break;

		case 2000:
			///////////////////////////////////////////////////////////////////////////////////
			//트레이가 이미 쉬프터에 끝까지 장착된 상태, 작업자가 한번에 뒤까지 밀어넣은 상태 
			///////////////////////////////////////////////////////////////////////////////////			
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyCvyInTrayExistChk,	IO_OFF);
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyTrayArrivedChk,	IO_ON);
			nRet_3 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyStkTrayExistChk,	IO_ON);
			if(nRet_1 == IO_OFF && nRet_2 == IO_ON  && nRet_3 == IO_ON)
			{
				st_sync_info.nConveyor_AcMotorCtl_Req[nTHD_Num] = CTL_READY;

				/////////////////////////////////
				//트레이 적재된 정보를 셋한다 
				/////////////////////////////////
				//st_tray_info[nTHD_Num].nTrayExist = CTL_YES;
				m_nInsertStep = 2100; //james 2016.0929 3000;	
			}
			else 
			{
				if(m_bAcMotorPassFlag == false)
				{
					m_dwAcMotorPasstWaitTime[0] = GetCurrentTime();
					m_bAcMotorPassFlag = true;
				}
				else if(m_bAcMotorPassFlag == true)
				{
					m_dwAcMotorPasstWaitTime[1] = GetCurrentTime();
					m_dwAcMotorPasstWaitTime[2] = m_dwAcMotorPasstWaitTime[1] - m_dwAcMotorPasstWaitTime[0];
					if(m_dwAcMotorPasstWaitTime[2] <= 0) m_dwAcMotorPasstWaitTime[0] = GetCurrentTime();
					if(m_dwAcMotorPasstWaitTime[2] > IO_CONVEYOR_WAIT_LIMIT) //리미트 시가이 10초이며, 이 시간이 지나도 트레이가 들어가지 않으면 에러이다 
					{
						m_bAcMotorPassFlag = false;
						st_sync_info.nConveyor_AcMotorCtl_Req[nTHD_Num] = CTL_FREE; //ac  motor 정지 
						m_nInsertStep = 1000;						

						//에러 발생 
					}
				}
			}
			break;

		case 2100: //james 2016.0929  
			if(st_sync_info.nConveyor_AcMotorCtl_Req[nTHD_Num] == CTL_CLEAR)
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyCvyInTrayExistChk,	IO_OFF);
				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyTrayArrivedChk,	IO_ON);
				nRet_3 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyStkTrayExistChk,	IO_ON);
				if(nRet_1 == IO_OFF && nRet_2 == IO_ON  && nRet_3 == IO_ON)
				{
					m_nInsertStep = 3000;
				}
				else
				{
					if(nRet_1 == IO_ON) m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_RejEmptyCvyInTrayExistChk); 
					else m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_RejEmptyTrayArrivedChk); 
					CTL_Lib.Alarm_Error_Occurrence(7700, dWARNING, m_strAlarmCode);
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

				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyCvyInTrayExistChk,	IO_OFF);
				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyTrayArrivedChk,	IO_ON);
				nRet_3 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyStkTrayExistChk,	IO_ON);
				if(nRet_1 == IO_OFF && nRet_2 == IO_ON  && nRet_3 == IO_ON)
				{
					m_nInsertStep = 3000;
				}
				else
				{
					if(nRet_1 == IO_ON) m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_LdStkTrayChk); 
					else m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_LdStkTrayArrivedChk); 
					CTL_Lib.Alarm_Error_Occurrence(7710, dWARNING, m_strAlarmCode);
					m_dwWaitTime[0] = GetCurrentTime();
					st_sync_info.nConveyor_AcMotorCtl_Req[nTHD_Num] = CTL_FREE; //ac  motor 정지 
				}
			}
			break;

		case 3000:
			//////////////////////////////////////////////////////////////////////////
			//화면 프로그램과 통신하여 적재된 트레이의 정보를 셋팅할 수 있게 한다
			//lot 정보 셋팅 
			//////////////////////////////////////////////////////////////////////////

			/*
			if(st_lot_info[LOT_CURR].nLotStatus >= LOT_END_START)
				{
					if(st_lot_info[LOT_CURR].nLotStatus != LOT_END)//2015.0302 james st_lot_info[LOT_CURR].nLotStatus[THD_PLATE_LD_TRAY_2_SITE] == CTL_NO) //if(st_var.nLotWork_Count > 2) //이미 작업중인 lot이 2개 이상이면 대기한다 
					{ 
						break; //현재 lot이 end 진행중이면, 로딩 plate site에서 작업이 끝날때까지는 다음 lot를 투입하지 못하게 한다.
					}
				}

				if(st_lot_info[LOT_NEXT].nLotStatus >= LOT_END_START)
				{					  
					break; //next lot이 end 진행중이면, 로딩 plate site에서 작업이 끝날때까지는 다음 lot를 투입하지 못하게 한다.					 
				}

				if(st_lot_info[LOT_CURR].nNewLotIn == CTL_YES && st_lot_info[LOT_NEXT].nNewLotIn == CTL_YES) //2015.0309 추가 
				{//lot 이 2개이상 진행중이면 smema을 요청하지 않는다.
					if(st_lot_info[LOT_NEXT].nLastModule == CTL_YES)
					{
						break;
					}
				}
			*/			
			m_nInsertStep  = 3100;	
			break;

		case 3100:
			
			st_tray_info[nTHD_Num].nTrayExist = CTL_YES;

			//clsFunc.Handler_Tray_DataInfo_Shift(2, CTL_YES, nTHD_Num, nTHD_Num); //처음 공급된 트레이로 트레이 정보를 입력 한다 

			m_nInsertStep  = 4000;	
			break;

		case 4000:
			m_nInsertStep = 1000;
			nFuncRet = RET_GOOD;
			break;
	}
}