// RejectTrayRearStackerElevator.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "RunRejectTrayRearStackerElevator.h"
#include "Variable.h"
#include "FastechPublic_IO.h"
#include "CtlBd_Library.h"
#include "LogCtrl.h"

#include "ComizoaPublic.h"
#include "CtlBd_Variable.h"
#include "PublicFunction.h"
#include "AlgMemory.h"
#include "LogFromat.h"

// CRunRejectTrayRearStackerElevator
CRunRejectTrayRearStackerElevator clsRunRejectTrayRearStackerElevator;
CRunRejectTrayRearStackerElevator::CRunRejectTrayRearStackerElevator()
{
	m_nRunStep	= -1;
	m_nInitStep	= -1;

	m_nAxisNum = M_RJ_ULD_2_ELV;
}

CRunRejectTrayRearStackerElevator::~CRunRejectTrayRearStackerElevator()
{
}


// CRunRejectTrayRearStackerElevator 멤버 함수
void CRunRejectTrayRearStackerElevator::OnThreadRun(void)
{
	if (st_handler_info.nSystemLock != FALSE)  return;	// 현재 시스템 Lock 상태인 경우 리턴

	CTL_Lib.Process_Tray_Conveyor_AcMotor(0, THD_REJECT_OUT_2_STACKER, st_io_info.o_Rej2UldCvyStkAcMotOnOff ); // elevator stacker AC Motor 제어하는 루틴	


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

void CRunRejectTrayRearStackerElevator::OnRunInit(void)
{
	int nRet_1, nRet_2, nRet_3=0;

	if (st_sync_info.nInit_Flag[THD_REJECT_OUT_2_STACKER] != INIT_CLEAR)		return;		//INIT_CLEAR 일때만 초기화 작업을 한다. 초기화가 끝나면 INIT_CLEAR -> INIT_READY가 되기 떄문에...

	switch (m_nInitStep)
	{
		case -1:
			 
			if(st_sync_info.nInit_Flag[THD_REJECT_TRANSFER] < INIT_READY) break;

			m_nInitStep = 0;
			break;

		case 0:		

			m_nInitStep = 100;
			break;

		case 100: //up
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
				CTL_Lib.Alarm_Error_Occurrence(7000, dWARNING, m_strAlarmCode);
				m_nInitStep = 100;
			}
			break;

		case 200:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Rej2StkTrayExistChk,	IO_OFF); //stacker tary 는 처음에는 없어야 한다  
			if(nRet_1 == IO_OFF)
			{
				m_nInitStep = 300;
			}
			else
			{//트레이 존재 에러 
				m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_Rej2StkTrayExistChk);  
				CTL_Lib.Alarm_Error_Occurrence(7010, dWARNING, m_strAlarmCode);
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
				CTL_Lib.Alarm_Error_Occurrence(7020, CTL_Lib.mn_run_status, COMI.strCode);
				m_nInitStep = 300;
			}			
			break;

		case 400:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Rej2StkTrayExistChk,	IO_OFF); //stacker에 트레이가 있다, 밖으로 뺴내야 한다   
			if(nRet_1 == IO_OFF)
			{
				m_nInitStep = 410;
			}
			else
			{//트레이 존재하지 않는다  
				 m_nInitStep = 500;
			}
			break;

		case 410:
			Set_StackerTray_UpDown(IO_OFF); 
			m_nInitStep = 411;
			break;

		case 411:
			nRet_1 = Chk_StackerTray_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nInitStep = 420;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(7030, dWARNING, m_strAlarmCode);
				m_nInitStep = 400;
			}
			break;

		case 420:
			//kwlee 2016.0902 Right Cv 계속 도는현상.
			st_sync_info.nConveyor_AcMotorCtl_Req[THD_REJECT_OUT_2_STACKER] = CTL_REQ; //cv rail ac motor 구동하여 적쨰된 트레이를 빼 낸다  
			//st_sync_info.nConveyor_AcMotorCtl_Req[THD_ULD_2_STACKER] = CTL_REQ; //cv rail ac motor 구동하여 적쨰된 트레이를 빼 낸다  
			 
			m_bCVTrayFlag = false;
			m_nInitStep = 430;			 
			break;

		case 430:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Rej1StkTrayExistChk,	IO_OFF); //stacker tary 는 처음에는 없어야 한다  
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_Rej2UldCvyTrayExistChk,	IO_OFF); //stacker tary 는 처음에는 없어야 한다  
			if(nRet_1 == IO_OFF && nRet_2 == IO_OFF)
			{
				//kwlee 2016.0902 Right Cv 계속 도는현상.
				//st_sync_info.nConveyor_AcMotorCtl_Req[THD_REJECT_OUT_1_STACKER] = CTL_READY; 
				st_sync_info.nConveyor_AcMotorCtl_Req[THD_REJECT_OUT_2_STACKER] = CTL_READY; 
				m_nInitStep = 900;
			}
			else
			{//트레이가 정상적으로 공급되지 않은 상태로 리미트 시간 체크  
				if(m_bCVTrayFlag == false)
				{
					m_dwCVTrayWaitTime[0] = GetCurrentTime();
					m_bCVTrayFlag = true;
				}
				else if(m_bCVTrayFlag == true)
				{
					m_dwCVTrayWaitTime[1] = GetCurrentTime();
					m_dwCVTrayWaitTime[2] = m_dwCVTrayWaitTime[1] - m_dwCVTrayWaitTime[0];
					if(m_dwCVTrayWaitTime[2] <= 0) m_dwCVTrayWaitTime[0] = GetCurrentTime();
					if(m_dwCVTrayWaitTime[2] > IO_CONVEYOR_WAIT_LIMIT) //2초 동안은 조건이 해제되도 계속 돌린다. 킬착시키기 위해 
					{	//error 
						if(nRet_1 == IO_ON)
						{
							m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_Uld1StkTrayExistChk); 
							CTL_Lib.Alarm_Error_Occurrence(7040, dWARNING, m_strAlarmCode);
						}
						else if(nRet_2 == IO_ON)
						{
							m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ReadyUld1CvyPassTrayChk); 
							CTL_Lib.Alarm_Error_Occurrence(7050, dWARNING, m_strAlarmCode);
						}
						else //if(nRet_3 == IO_OFF)
						{
							m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ReadyUld1CvyOutTrayChk); 
							CTL_Lib.Alarm_Error_Occurrence(7060, dWARNING, m_strAlarmCode);
						}

						//에러가 발생했으니 AC Motor를 즉시 정지 시킨다 
						//kwlee 2016.0902 
						//st_sync_info.nConveyor_AcMotorCtl_Req[THD_ULD_1_STACKER] = CTL_FREE;  
						st_sync_info.nConveyor_AcMotorCtl_Req[THD_REJECT_OUT_2_STACKER] = CTL_FREE;
						m_bCVTrayFlag = false; //처음으로 다시가 재 시도 
						m_nInitStep = 420;
					}
				}
			} 
			break;

		case 900:
			st_sync_info.nInit_Flag[THD_REJECT_OUT_2_STACKER] = INIT_READY; 
			 
			m_nInitStep = 0;
			break;	

		default :
			break;

	}
}

void CRunRejectTrayRearStackerElevator::OnRunMove(void)
{
		int nRet_1, nRet_2=0, nRet_3=0;
	int nCount = 0;
	int x,y;
	CString strMsg;

	clsFunc.OnThreadFunctionStepTrace(11, m_nRunStep);		
	switch (m_nRunStep)
	{ 
		case -1: 

			m_nRunStep = 0;
			break;

		case 0:
			m_nRunStep = 100;
			break;

		case 100:// 
			if(st_lot_info[LOT_CURR].nLotStatus == LOT_START)
			{
				m_nLotProcessNum = LOT_CURR;
				if(st_lot_info[m_nLotProcessNum].nStacker_Reject_THD_Num == THD_REJECT_OUT_2_STACKER)//해당 스태커가 선택됐을때만 동작하자 
				{
					st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_REJECT_OUT_2_STACKER] =  LOT_START;
					m_nRunStep = 110;
				}
			}
			else if(st_lot_info[LOT_NEXT].nLotStatus == LOT_START)
			{
				m_nLotProcessNum = LOT_NEXT;
				if(st_lot_info[m_nLotProcessNum].nStacker_Reject_THD_Num == THD_REJECT_OUT_2_STACKER) //해당 스태커가 선택됐을때만 동작하자 
				{
					st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_REJECT_OUT_2_STACKER] =  LOT_START;
					m_nRunStep = 110;
				}
			}		 
			break;

		case 110:
			Set_ConveyorStopper_UpDown(IO_OFF);  //up
			m_nRunStep = 120;
			break;

		case 120:
			nRet_1 = Chk_ConveyorStopper_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 200;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(7100, dWARNING, m_strAlarmCode);
				m_nRunStep = 110;
			}
			break; 

		case 200:
			Set_StackerTray_UpDown(IO_OFF); 
			m_nRunStep = 210;
			break;

		case 210:
			nRet_1 = Chk_StackerTray_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 300;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(7110, dWARNING, m_strAlarmCode);
				m_nRunStep = 200;
			}
			break; 

		case 300:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Rej2StkTrayExistChk,	IO_OFF); //stacker tary 는 처음에는 없어야 한다  
			if(nRet_1 == IO_OFF)
			{
				m_nRunStep = 310; //james 2016.0923 400;
			}
			else
			{//트레이 존재 에러 
				m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_Rej2StkTrayExistChk);  
				CTL_Lib.Alarm_Error_Occurrence(7120, dWARNING, m_strAlarmCode);
			}
			break;

		case 310:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Rej2UldCvyTrayExistChk,	IO_OFF); //stacker tary 는 처음에는 없어야 한다  
			if(nRet_1 == IO_OFF)
			{
				m_nRunStep = 400;
			}
			else
			{//트레이 존재 에러 
				m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_Rej2UldCvyTrayExistChk);  
				CTL_Lib.Alarm_Error_Occurrence(7130, dWARNING, m_strAlarmCode);
			}
			break;

		case 400:
			nRet_1 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 	
			if(nRet_1 == BD_GOOD) //로더 플레이트에 트레이가 감지 된 상태 
			{
				m_strAlarmCode.Format(_T("900013")); //900013 1 0 "REJECT_2_STACKER_PLATE_SD_TRAY_ON_CHECK_ERROR"
				CTL_Lib.Alarm_Error_Occurrence(7140, dWARNING, m_strAlarmCode); 
			}
			else 
			{
				m_nRunStep = 500;
			}
			break;

		case 500://미리 받는 위치로 이동한다 
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Rej2UldCvyTrayExistChk,	IO_ON); //james 2016.0926 
			if(nRet_1 == IO_ON)
			{
				COMI.Set_MotStop(MOT_STOP_DEC, m_nAxisNum); //감속정지   // 모터 이동 정지 함수 
				m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Rej2UldCvyTrayExistChk); 
				CTL_Lib.Alarm_Error_Occurrence(7150, dWARNING, m_strAlarmCode);
				break;
			}

			nRet_1 = CTL_Lib.Elevator_Job_Move_Pos(0, m_nAxisNum,  P_ELV_RECEIVE_OFFSET); //트레이를 트랜스퍼에서 받는다 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep =600;
			}
			break;

		case 600:
			nRet_1 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 	
			if(nRet_1 == BD_GOOD) //로더 플레이트에 트레이가 감지 된 상태 
			{
				m_strAlarmCode.Format(_T("900013")); //900013 1 0 "REJECT_2_STACKER_PLATE_SD_TRAY_ON_CHECK_ERROR"
				CTL_Lib.Alarm_Error_Occurrence(7160, dWARNING, m_strAlarmCode);
			}
			else if(nRet_1 == BD_ERROR)//로더 플레이드에 트레이가 감지 되지않은 상태 
			{
				m_nRunStep = 900;
			}
			break;

		case 900: //
			st_tray_info[THD_REJECT_OUT_2_STACKER].nTrayExist = CTL_NO;//2016.0923 
			m_nRunStep = 1000;
			break;


		//////////////////////////////////////////////////////////////////////////////
		// Work robot가 LOT 을 확인하여 작업할 언로딩 스태커를 구분하여 요청한다 
		//1. LOT 시작시 unload stacker 정보에 LOT 정보를 입력하여 구분 사용하게 한다 
		//   st_tray_info[THD_ULD_1_STACKER].strLotNo
		/////////////////////////////////////////////////////////////////////////////
		case 1000: 
			m_nRetry = 0;
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Rej2StkUpChk,	IO_ON);
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_Rej2StkDnChk,	IO_OFF);
			if(nRet_1 == IO_ON && nRet_2 == IO_OFF)
			{				
				m_nRunStep = 1100; 
			}
			else
			{
				Set_StackerTray_UpDown(IO_ON); 
				m_nRunStep = 1010;
			}
			break;
		
		case 1010:
			nRet_1 = Chk_StackerTray_UpDown(IO_ON); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRetry = 0;
				m_nRunStep = 1100;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(7170, dWARNING, m_strAlarmCode);
				m_nRunStep = 1000;
			}
			break; 

		case 1100:
			if(st_sync_info.nWorkRbt_Dvc_Req[THD_REJECT_OUT_2_STACKER][0] == CTL_REQ) //워크 로봇에서 uld 1 stacker에 트레이 공간을 요청 했다 
			{
				if(st_tray_info[THD_REJECT_OUT_2_STACKER].nTrayExist == CTL_YES) //트레이 정보가 있는 상태 
				{
					nRet_1 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 	
					if(nRet_1 == BD_GOOD)
					{
						m_nRunStep = 2000;
					}
					else //if(nRet_1 == BD_ERROR) //플레이트에 트레이가 감지 안된 상태 
					{
						m_nRetry++;
						if (m_nRetry > 2)
						{
							m_nRetry = 0;
							m_strAlarmCode.Format(_T("900013")); //900013 1 0 "REJECT_2_STACKER_PLATE_SD_TRAY_ON_CHECK_ERROR"
							CTL_Lib.Alarm_Error_Occurrence(7180, dWARNING, m_strAlarmCode);	
						}
						m_nRunStep = 1200;
					} 
				}
				else  //트레이가 없으면 
				{
					nRet_1 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 	
					if(nRet_1 == BD_ERROR) //플레이트에 트레이가 감지 안된 상태 
					{
						m_nRunStep = 3000;
					}
					else if(nRet_1 == BD_ERROR) //플레이트에 트레이가 감지 안된 상태 
					{
						m_nRetry++;
						if (m_nRetry > 2)
						{
							m_nRetry = 0;
							m_strAlarmCode.Format(_T("900014")); //900014 1 0 "REJECT_2_STACKER_PLATE_SD_TRAY_OFF_CHECK_ERROR"
							CTL_Lib.Alarm_Error_Occurrence(7190, dWARNING, m_strAlarmCode);
						}
						m_nRunStep = 1300;
					}
				}
			}
			else 
			{//james 2016.0916 
				if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] ==  LOT_END) //더이상 작업할 자재가 없다 , lot end 진행, 이곳에서 전체 lot을 관리한다
				{				
					nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Rej2StkTrayExistChk,	IO_ON); //elevator stacker tary 가 있어야 한다 
					if(st_count_info.nUnLdStacker_Reject_TrayCnt[1] == 0 && nRet_1 == IO_OFF)
					{//트레이가 없는 상태 
						m_nRunStep = 9000;
					}
					else
					{//트레이가 있는 상태
						m_nRunStep = 7000; 
					}  
				}
			}
			break;

		case 1200:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Rej2UldCvyTrayExistChk,	IO_ON); //james 2016.0926 
			if(nRet_1 == IO_ON)
			{
				COMI.Set_MotStop(MOT_STOP_DEC, m_nAxisNum); //감속정지   // 모터 이동 정지 함수 
				m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Rej2UldCvyTrayExistChk); 
				CTL_Lib.Alarm_Error_Occurrence(7200, dWARNING, m_strAlarmCode);
				break;
			}

			nRet_1 = CTL_Lib.Elevator_Job_Move_Pos(0, m_nAxisNum,  P_ELV_SUPPLY_OFFSET); //트레이를 트랜스퍼에서 받는다 
			
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 1100;
			}
			break;

		case 1300:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Rej2UldCvyTrayExistChk,	IO_ON); //james 2016.0926 
			if(nRet_1 == IO_ON)
			{
				COMI.Set_MotStop(MOT_STOP_DEC, m_nAxisNum); //감속정지   // 모터 이동 정지 함수 
				m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Rej2UldCvyTrayExistChk); 
				CTL_Lib.Alarm_Error_Occurrence(7210, dWARNING, m_strAlarmCode);
				break;
			}

			nRet_1 = CTL_Lib.Elevator_Job_Move_Pos(0, m_nAxisNum,  P_ELV_RECEIVE_OFFSET); //트레이를 트랜스퍼에서 받는다 
			
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 1100;
			}
			break;
		//////////////////////////////////////////////////
		// 트레이가 준비되어있으니, 트레이 정보를 찾는다 
		//////////////////////////////////////////////////
		case 2000: 
			nRet_1 = clsFunc.Find_TrayWork_Pos(0, THD_REJECT_OUT_2_STACKER, CTL_NO, m_npFindWorkPosYXCPB, THD_WORK_RBT, st_worker_info.strLotNo_Empty);
			if(nRet_1 == RET_GOOD) //작업 가능한 자재가 있다
			{				
				///////////////////////////////////////////////////////
				// 트레이 위치 다시 확인 후 정보 셋팅  
				///////////////////////////////////////////////////////

				nRet_1 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 	
				if(nRet_1 == BD_GOOD)
				{
					st_sync_info.nWorkRbt_Dvc_Req[THD_REJECT_OUT_2_STACKER][0] = CTL_READY; //현재 트레이의 작업이 가능하다 
					m_nRunStep = 1000;
				}
				else //if(nRet_1 == BD_ERROR) //플레이트에 트레이가 감지 안된 상태 
				{
					m_strAlarmCode.Format(_T("900014")); //900014 1 0 "REJECT_2_STACKER_PLATE_SD_TRAY_OFF_CHECK_ERROR"
					CTL_Lib.Alarm_Error_Occurrence(7220, dWARNING, m_strAlarmCode);						
				} 
			}
			else if(nRet_1 == RET_SKIP) //더이상 디바이스를 놓을 공간이 없다 
			{	//빈 트레이를 적재하고, 다시 받아야 한다 

				///////////////////////////////////////////////////////
				// 트레이를 재 적재하고 다시 트레이를 공급 받아야 한다 
				///////////////////////////////////////////////////////
				m_nRunStep = 3000;
			}
			break;		

		///////////////////////////////////////////////////////
		// 트레이가 없으니, 트레이를 공급 받아야 한다 
		///////////////////////////////////////////////////////
		case 3000:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Rej2UldCvyTrayExistChk,	IO_ON); //james 2016.0926 
			if(nRet_1 == IO_ON)
			{
				COMI.Set_MotStop(MOT_STOP_DEC, m_nAxisNum); //감속정지   // 모터 이동 정지 함수 
				m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Rej2UldCvyTrayExistChk); 
				CTL_Lib.Alarm_Error_Occurrence(7230, dWARNING, m_strAlarmCode);
				break;
			}

			nRet_1 = CTL_Lib.Elevator_Job_Move_Pos(0, m_nAxisNum,  P_ELV_RECEIVE_OFFSET); //트레이를 트랜스퍼에서 받는다 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 3100;
			}
			break;

		case 3100:
			if(st_tray_info[THD_REJECT_OUT_2_STACKER].nTrayExist == CTL_YES) //트레이가 있으나, 작업할 공간이 없는 상태로 이미 한번 작업이 완료하느트레이이다. 트레이 정보가 있는 상태 
			{
				st_tray_info[THD_REJECT_OUT_2_STACKER].nTrayExist = CTL_NO; //tray 정보 클리어 

				st_lot_info[m_nLotProcessNum].nUnLdTrayCnt++; //2015.0225 james 해당 lot의 트레이 카운트 정보 증가 

				//st_count_info.nUnldStackerTray++; //트레이 스태커 트레이 적재 정보 하나 up 
				st_count_info.nUnLdStacker_Reject_TrayCnt[1]++; //unload good 1 stacker tray 적재 수량

				if(st_count_info.nUnLdStacker_Reject_TrayCnt[1] >= st_basic_info.nUldGoodTrayStack_Count)//셋팅 수랴을 적재 했으면 배출한다  
				{
					m_nRunStep = 7000;
				}
				else
				{				
					m_nRunStep = 3200;				
				}
			}
			else
			{
				m_nRunStep = 3200;
			}
			break;

		// 새로운 트레이를 요청한다 
		case 3200:
			st_sync_info.nRejectTransfer_Req[THD_REJECT_OUT_2_STACKER][0] = CTL_REQ;
			st_sync_info.nRejectTransfer_Req[THD_REJECT_OUT_2_STACKER][1] = WORK_PLACE;
			m_nRunStep = 3300;
			break;

		case 3300:
			if(st_sync_info.nRejectTransfer_Req[THD_REJECT_OUT_2_STACKER][0] == CTL_READY)
			{
				nRet_1 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 	
				if(nRet_1 == BD_GOOD)
				{
					///////////////////////////////////////////////////////
					// 트레이 정보를 모두 받은 상태  
					///////////////////////////////////////////////////////
					st_tray_info[THD_REJECT_OUT_2_STACKER].nTrayExist = CTL_YES;//2016.0912 
					st_sync_info.nRejectTransfer_Req[THD_REJECT_OUT_2_STACKER][0] = CTL_CLEAR;
					m_nRunStep = 3400;
				}
				else //if(nRet_1 == BD_ERROR) //플레이트에 트레이가 감지 안된 상태 
				{
					m_strAlarmCode.Format(_T("900013")); //900013 1 0 "REJECT_2_STACKER_PLATE_SD_TRAY_ON_CHECK_ERROR."
					CTL_Lib.Alarm_Error_Occurrence(7340, dWARNING, m_strAlarmCode);						
				} 
			}
			else
			{
				//james 2016.0926 
				if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] ==  LOT_END) //더이상 작업할 자재가 없다 , lot end 진행, 이곳에서 전체 lot을 관리한다
				{				
					nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Rej2StkTrayExistChk,	IO_ON); //elevator stacker tary 가 있어야 한다 
					if(st_count_info.nUnLdStacker_Reject_TrayCnt[1] == 0 && nRet_1 == IO_OFF)
					{//트레이가 없는 상태 
						m_nRunStep = 9000;
					}
					else
					{//트레이가 있는 상태
						m_nRunStep = 7000; 
					}  
				}
			}
			break;

		case 3400:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Rej2UldCvyTrayExistChk,	IO_ON); //james 2016.0926 
			if(nRet_1 == IO_ON)
			{
				COMI.Set_MotStop(MOT_STOP_DEC, m_nAxisNum); //감속정지   // 모터 이동 정지 함수 
				m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Rej2UldCvyTrayExistChk); 
				CTL_Lib.Alarm_Error_Occurrence(7250, dWARNING, m_strAlarmCode);
				break;
			}

			///////////////////////////////////////////////////////
			// 트레이를 로봇이 놓을 수 있는 위치로 셋팅  
			///////////////////////////////////////////////////////
			nRet_1 = CTL_Lib.Elevator_Job_Move_Pos(0, m_nAxisNum,  P_ELV_SUPPLY_OFFSET); //트레이를 트랜스퍼에서 받는다 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 3500;
			}
			break;

		case 3500:
			nRet_1 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 	
			if(nRet_1 == BD_GOOD)
			{			 
				m_nRunStep = 3600;
			}
			else //if(nRet_1 == BD_ERROR) //플레이트에 트레이가 감지 안된 상태 
			{
				m_strAlarmCode.Format(_T("900013")); //900013 1 0 "REJECT_2_STACKER_PLATE_SD_TRAY_ON_CHECK_ERROR."
				CTL_Lib.Alarm_Error_Occurrence(7260, dWARNING, m_strAlarmCode);						
			} 
			break;

		case 3600:
			m_nRunStep = 1000;
			break;
			

		////////////////////////////////////////////////////////////////
		// stacker를 내려 적재된 트레이를 빼낸다 
		////////////////////////////////////////////////////////////////
		case 7000: 
			Set_StackerTray_UpDown(IO_OFF);  //up
			m_nRunStep = 7010;
			break;

		case 7010:
			nRet_1 = Chk_StackerTray_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 7020;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(7270, dWARNING, m_strAlarmCode);
				m_nRunStep = 7000;
			}
			break; 

		case 7020:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Rej2StkTrayExistChk,	IO_ON); //elevator stacker tary 가 있어야 한다 
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_Rej2UldCvyTrayExistChk,	IO_OFF); //tary가 중간에 감지되면 안된다  
			if(nRet_1 == IO_ON && nRet_2 == IO_OFF)
			{				
				Set_ConveyorStopper_UpDown(IO_OFF); //up
				st_sync_info.nConveyor_AcMotorCtl_Req[THD_REJECT_OUT_2_STACKER] = CTL_FREE;//혹시 ac motor가 가동중이면 즉시 ㅈ정지 시킨다 
				m_nRunStep = 7100;
			}
			else
			{//트레이  에러 
				if(nRet_1 == IO_ON) m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Rej2StkTrayExistChk); 
				else  m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_Rej2UldCvyTrayExistChk); 
				CTL_Lib.Alarm_Error_Occurrence(7280, dWARNING, m_strAlarmCode);
			}
			break;

		case 7100:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Rej2UldCvyTrayExistChk,	IO_ON); //james 2016.0926 
			if(nRet_1 == IO_ON)
			{
				COMI.Set_MotStop(MOT_STOP_DEC, m_nAxisNum); //감속정지   // 모터 이동 정지 함수 
				m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Rej2UldCvyTrayExistChk); 
				CTL_Lib.Alarm_Error_Occurrence(7290, dWARNING, m_strAlarmCode);
				break;
			}

			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nAxisNum, st_motor_info[m_nAxisNum].d_pos[P_ELV_TRAY_INOUT_POS], COMI.mn_runspeed_rate);  //2015.0407 james  //SD 위치까지 이동했는데 SD가 감지되지 않으면 트레이가 없는것이다
			if (nRet_1 == BD_GOOD)   
			{	 
				m_nRunStep = 7200;  			 
			}
			else if (nRet_1 == BD_RETRY)
			{		 
				m_nRunStep = 7000;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{ 
				CTL_Lib.Alarm_Error_Occurrence(7300, dWARNING, st_alarm_info.strCode);

				////////if (st_handler_info.cWndList != NULL)  
				{
					//clsMem.OnNormalMessageWrite(_T("UnLoader Alarm : 2010"));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG); 
				}
				m_nRunStep = 7000;
			}
			break;

		case 7200:
			nRet_1 = Chk_ConveyorStopper_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 7300;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(7310, dWARNING, m_strAlarmCode);
				m_nRunStep = 7210;
			}
			break;  

		case 7210:
			Set_ConveyorStopper_UpDown(IO_OFF); 
			m_nRunStep = 7200;
			break;

		case 7300:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Rej2StkTrayExistChk,	IO_ON); //elevator stacker tary 가 있어야 한다 
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_Rej2UldCvyTrayExistChk,	IO_OFF); //tary가 중간에 감지되면 안된다  
			if(nRet_1 == IO_ON && nRet_2 == IO_OFF )
			{				
				m_nRunStep = 8000;
			}
			else
			{//트레이  에러 
				if(nRet_1 == IO_ON) m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Rej2StkTrayExistChk); 
				else if(nRet_2 == IO_ON) m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_Rej2UldCvyTrayExistChk);  
				CTL_Lib.Alarm_Error_Occurrence(7320, dWARNING, m_strAlarmCode);
			}
			break;


		///////////////////////////////////////////////////////////
		// 작업자가 배출 버튼을 작동시키면 나가야 할 것이다 
		///////////////////////////////////////////////////////////
		case 8000:
			st_sync_info.nConveyor_AcMotorCtl_Req[THD_REJECT_OUT_2_STACKER] = CTL_REQ;
			m_bCVTrayFlag = false; 
			m_nRunStep = 8100;
			break;

		case 8100:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Rej2StkTrayExistChk,	IO_OFF);
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_Rej2UldCvyTrayExistChk,	IO_ON);  

			if(nRet_1 == IO_OFF && nRet_2 == IO_ON)
			{
				//load cv & stacker ac motor 를 동시에 같이 구동시켜 동시에 정지하게 한다 
				st_sync_info.nConveyor_AcMotorCtl_Req[THD_REJECT_OUT_2_STACKER] = CTL_READY;  			 

				m_bCVTrayFlag = false;
				m_nRunStep = 8200;
			}
			else
			{//트레이가 정상적으로 공급되지 않은 상태로 리미트 시간 체크  
				if(m_bCVTrayFlag == false)
				{
					m_dwCVTrayWaitTime[0] = GetCurrentTime();
					m_bCVTrayFlag = true;
				}
				else if(m_bCVTrayFlag == true)
				{
					m_dwCVTrayWaitTime[1] = GetCurrentTime();
					m_dwCVTrayWaitTime[2] = m_dwCVTrayWaitTime[1] - m_dwCVTrayWaitTime[0];
					if(m_dwCVTrayWaitTime[2] <= 0) m_dwCVTrayWaitTime[0] = GetCurrentTime();
					if(m_dwCVTrayWaitTime[2] > IO_CONVEYOR_WAIT_LIMIT) //2초 동안은 조건이 해제되도 계속 돌린다. 킬착시키기 위해 
					{	//error 
						if(nRet_1 == IO_ON)
						{
							m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_Rej2StkTrayExistChk); 
							CTL_Lib.Alarm_Error_Occurrence(7330, dWARNING, m_strAlarmCode);
						}
						else if(nRet_2 == IO_OFF)
						{
							m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_Rej2UldCvyTrayExistChk); 
							CTL_Lib.Alarm_Error_Occurrence(7340, dWARNING, m_strAlarmCode);
						}

						//에러가 발생했으니 AC Motor를 즉시 정지 시킨다 
						st_sync_info.nConveyor_AcMotorCtl_Req[THD_REJECT_OUT_2_STACKER] = CTL_FREE;  		
						m_bCVTrayFlag = false;
				 
						m_nRunStep = 8000;
					}
				}
			}
			break;

		case 8200:
			if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] ==  LOT_END)
			{				
				st_count_info.nUnLdStacker_Reject_TrayCnt[1] = 0; //2016.0819  count clear
				m_nRunStep = 9000; 
			}
			else
			{
				///////////////////////////////////////
				// 메세지를 출력하여 작업자에게 알린다 
				///////////////////////////////////////

				if (st_handler_info.cWndMain != NULL)
				{
					st_other_info.nBuzYesNo = YES;
					st_other_info.strBoxMsg = _T("There are trays to eject on Reject#2 Coveyor. \r\n Please Remove it.");
					st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
				}

				Set_ConveyorStopper_UpDown(IO_ON);  //down
				st_count_info.nUnLdStacker_Reject_TrayCnt[1] = 0; //2016.0819  count clear
				m_nRunStep = 1000;
			}
			break;

		////////////////////////////////////////////
		//LOT ENd로 트레이 정리 
		////////////////////////////////////////////
		case 9000:

			/*

			if(st_lot_info[m_nLotProcessNum].strLotNo == st_tray_info[THD_REJECT_OUT_1_STACKER].strLotNo && st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_REJECT_OUT_1_STACKER] ==  LOT_END )
			{
				/////////////////////////////////////////////////////
				//전체 lot end 메세지 출력
				/////////////////////////////////////////////////////

			}
			else if(st_lot_info[m_nLotProcessNum].strLotNo == st_tray_info[THD_REJECT_OUT_2_STACKER].strLotNo && st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_REJECT_OUT_2_STACKER] ==  LOT_END)
			{
				/////////////////////////////////////////////////////
				//전체 lot end 메세지 출력
				/////////////////////////////////////////////////////
			}*/
			for (x=0; x<st_recipe_info.nTrayX; x++)
			{
				for (y=0; y<st_recipe_info.nTrayY; y++)
				{
					if(st_tray_info[THD_REJECT_OUT_2_STACKER].st_pcb_info[y][x].nYesNo == CTL_YES)
					{
						st_tray_info[THD_REJECT_OUT_2_STACKER].st_pcb_info[y][x].nYesNo = CTL_NO;
					}
				}
			}

			st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_REJECT_OUT_2_STACKER] =  LOT_END;
			Set_ConveyorStopper_UpDown(IO_ON);  //down
			m_nRunStep = 0;

			//kwlee 2016.1114
			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
				strMsg.Format(_T("[LOT END] Reject #2 Reject Stacker Tray. LotProcessNum =%d "), m_nLotProcessNum );
				clsMem.OnNormalMessageWrite(strMsg);
			}
			//
			break;



		default: 
			break;
	}
}


void CRunRejectTrayRearStackerElevator::Set_StackerTray_UpDown( int nOnOff )
{
	CString strLogKey[10];
	CString	strLogData[10];

	strLogKey[0] = _T("Mode Start");
	strLogData[0].Format(_T("%d"),0);

	m_bStackerUpDown = true;
	m_dwWaitStackerUpDown[0] = GetCurrentTime();

	FAS_IO.set_out_bit( st_io_info.o_Rej2StkUpCyl, nOnOff);
	FAS_IO.set_out_bit( st_io_info.o_Rej2StkDnCyl, !nOnOff);

	///kwlee 2016.0812
	if (nOnOff == IO_ON)
	{
		//LogFunction(_T("REJECT_TRAY_REAR_STK_ELVATOR"),_T("UP"),_T("REJECT_REAR_STACKER"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("REJECT_TRAY_REAR_STK_ELVATOR"),_T("UP"),0,_T("REJECT_REAR_STACKER"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
	else
	{
		//LogFunction(_T("REJECT_TRAY_REAR_STK_ELVATOR"),_T("DOWN"),_T("REJECT_REAR_STACKER"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("REJECT_TRAY_REAR_STK_ELVATOR"),_T("DOWN"),0,_T("REJECT_REAR_STACKER"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
}

int	CRunRejectTrayRearStackerElevator::Chk_StackerTray_UpDown( int nOnOff )
{
	CString strLogKey[10];
	CString	strLogData[10];

	strLogKey[0] = _T("Mode End");
	strLogData[0].Format(_T("%d"),0);

//	800614 1 0 "REJECT_2_STACKER_DOWN_CHECK"
//	800614 1 0 "REJECT_2_STACKER_DOWN_CHECK"
	int nWaitTime = WAIT_STACKER_UP_DN;

	if (nOnOff == IO_OFF)
	{
		if (m_bStackerUpDown == false &&
			FAS_IO.get_in_bit(st_io_info.i_Rej2StkUpChk,	IO_OFF)	== IO_OFF &&
			FAS_IO.get_in_bit(st_io_info.i_Rej2StkDnChk,	IO_ON)	== IO_ON)
		{
			m_bStackerUpDown		= true;
			m_dwWaitStackerUpDown[0]	= GetCurrentTime();
		}
		else if (m_bStackerUpDown == true &&
				 FAS_IO.get_in_bit(st_io_info.i_Rej2StkUpChk,	IO_OFF)	== IO_OFF &&
				 FAS_IO.get_in_bit(st_io_info.i_Rej2StkDnChk,	IO_ON)	== IO_ON)
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
				//LogFunction(_T("REJECT_TRAY_REAR_STK_ELVATOR"),_T("DOWN"),_T("REJECT_REAR_STACKER"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("REJECT_TRAY_REAR_STK_ELVATOR"),_T("DOWN"),1,_T("REJECT_REAR_STACKER"),_T("CYLINDER"),1,strLogKey,strLogData);
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
				m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_Rej2StkDnChk); 
				//2017.0109
				clsLog.LogFunction(_T("REJECT_TRAY_REAR_STK_ELVATOR"),_T("DOWN"),1,_T("REJECT_REAR_STACKER"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_ERROR;
			}
		}
	}
	else
	{
		if (m_bStackerUpDown == false &&
			FAS_IO.get_in_bit(st_io_info.i_Rej2StkUpChk,	IO_ON)	== IO_ON &&
			FAS_IO.get_in_bit(st_io_info.i_Rej2StkDnChk,	IO_OFF)	== IO_OFF)
		{
			m_bStackerUpDown			= true;
			m_dwWaitStackerUpDown[0]	= GetCurrentTime();
		}
		else if (m_bStackerUpDown == true &&
				 FAS_IO.get_in_bit(st_io_info.i_Rej2StkUpChk,	IO_ON)	== IO_ON &&
				 FAS_IO.get_in_bit(st_io_info.i_Rej2StkDnChk,	IO_OFF)	== IO_OFF)
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
				//LogFunction(_T("REJECT_TRAY_REAR_STK_ELVATOR"),_T("UP"),_T("REJECT_REAR_STACKER"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("REJECT_TRAY_REAR_STK_ELVATOR"),_T("UP"),1,_T("REJECT_REAR_STACKER"),_T("CYLINDER"),1,strLogKey,strLogData);
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
				m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_Rej2StkUpChk); 
				//2017.0109
				clsLog.LogFunction(_T("REJECT_TRAY_REAR_STK_ELVATOR"),_T("UP"),1,_T("REJECT_REAR_STACKER"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_ERROR;
			}
		}
	}

	return RET_PROCEED;
}

void CRunRejectTrayRearStackerElevator::Set_ConveyorStopper_UpDown( int nOnOff )
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

int	CRunRejectTrayRearStackerElevator::Chk_ConveyorStopper_UpDown( int nOnOff )
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
				 FAS_IO.get_in_bit(st_io_info.i_Rej2UldCvyStopUpChk,	IO_OFF)	== IO_OFF &&
				 FAS_IO.get_in_bit(st_io_info.i_Rej2UldCvyStopDnChk,	IO_ON)	== IO_ON)
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
