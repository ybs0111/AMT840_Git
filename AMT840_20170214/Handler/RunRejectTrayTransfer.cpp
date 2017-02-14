// RejectTrayTransfer.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "RunRejectTrayTransfer.h"
#include "Variable.h"
#include "FastechPublic_IO.h"
#include "CtlBd_Library.h"
#include "CtlBd_Variable.h"
#include "PublicFunction.h"
#include "ComizoaPublic.h"
#include "math.h"
#include "LogCtrl.h"
#include "LogFromat.h"

// CRejectTrayTransfer
CRunRejectTrayTransfer clsRunRejectTrayTransfer;
CRunRejectTrayTransfer::CRunRejectTrayTransfer()
{
	m_nRunStep	= -1;
	m_nInitStep	= -1;	 

	m_nAxisNum = M_REJECTTRAY_TRANSFER;
}

CRunRejectTrayTransfer::~CRunRejectTrayTransfer()
{
}


// CRejectTrayTransfer 멤버 함수
void CRunRejectTrayTransfer::OnThreadRun(void)
{
	//james 2016.0818
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

void CRunRejectTrayTransfer::OnRunInit(void)
{
	int nRet_1, nRet_2=0;

	if (st_sync_info.nInit_Flag[THD_REJECT_TRANSFER] != INIT_CLEAR)		return;		//INIT_CLEAR 일때만 초기화 작업을 한다. 초기화가 끝나면 INIT_CLEAR -> INIT_READY가 되기 떄문에...
	 
	switch (m_nInitStep)
	{
		case -1:			 

			m_nInitStep = 0;
			break;

		case 0:		
			if(COMI.mn_homechk_flag[M_WORK_RBT_Z] != CTL_YES) break;//2015.0915
			m_nInitStep = 100;
			break;

		case 100: //up
			Set_RejectTrayTransfer_UpDown(IO_OFF); 
			m_nInitStep = 110;
			break;

		case 110:
			nRet_1 = Chk_RejectTrayTransfer_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nInitStep = 200;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(8220, dWARNING, m_strAlarmCode);
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
				CTL_Lib.Alarm_Error_Occurrence(8010, CTL_Lib.mn_run_status, COMI.strCode);
				m_nInitStep = 200;
			}			
			break;

		case 300:
			//kwlee 2016.0908 
			//nRet_1 = FAS_IO.get_in_bit(st_io_info.i_TrayTrsfVacChk,	IO_OFF);  
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_RejTrsfVacChk,	IO_OFF);  
			
			if(nRet_1 == IO_OFF)
			{
				m_nInitStep = 400;
			}
			else
			{//트레이 존재 에러 
				//kwlee 2016.0908
				//m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_TrayTrsfVacChk); 
				m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_RejTrsfVacChk); 
				CTL_Lib.Alarm_Error_Occurrence(8020, dWARNING, m_strAlarmCode);
			}
			break;

		case 400:
			Set_RejectTrayVacuum_OnOff(IO_OFF); //kwlee 2016.0908
			Set_RejectTransferEjectorOnOff(IO_OFF );
			m_nInitStep = 900;
			break;

		case 900:
			st_sync_info.nInit_Flag[THD_REJECT_TRANSFER] = INIT_READY; 
			 
			m_nInitStep = 0;
			break;	

		default :
			break;

	}
}

void CRunRejectTrayTransfer::OnRunMove(void)
{
	int nRet_1, nRet_2=0, nRet_3=0;
	int nCount = 0, nJobFlag=0;

	clsFunc.OnThreadFunctionStepTrace(13, m_nRunStep);	

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

		case 100://
			Set_RejectTrayTransfer_UpDown(IO_OFF); 
			m_nRunStep = 110;
			break;

		case 110:
			nRet_1 = Chk_RejectTrayTransfer_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 200;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(8030, dWARNING, m_strAlarmCode);
				m_nRunStep = 100;
			}
			break;

		case 200:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_RejTrsfVacChk,	IO_OFF);  
			if(nRet_1 == IO_OFF)
			{
				m_nRunStep = 300;
			}
			else
			{//트레이 존재 에러 
				m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_RejTrsfVacChk); 
				CTL_Lib.Alarm_Error_Occurrence(8040, dWARNING, m_strAlarmCode);
			}
			break;

		case 300:
			Set_RejectTrayVacuum_OnOff(IO_OFF);
			Set_RejectTransferEjectorOnOff(IO_OFF );
			m_nRunStep = 400;
			break;

		case 400:
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nAxisNum, st_motor_info[m_nAxisNum].d_pos[P_REJECTTRANS_EMPTY_STACKER], COMI.mn_runspeed_rate);  //left로 미리 이동해 있는다 
			if (nRet_1 == BD_GOOD) //좌측으로 이동  
			{			
				m_nRunStep = 900;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nRunStep = 400;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다  
				CTL_Lib.Alarm_Error_Occurrence(8050, dWARNING, st_alarm_info.strCode);
				m_nRunStep = 400;
			}
			break; 


		case 900: //기본적인 초기 정리가 완료된 상태 
			st_tray_info[THD_REJECT_TRANSFER].nTrayExist = CTL_NO; //james 2016.0915
			m_nTransfer_WaitPosMove_Flag = CTL_NO;
			m_nRunStep = 1000;
			break;

		//////////////////////////////////////////////////////////////////////////
		// Tray 공급 및 교체 담당
		//////////////////////////////////////////////////////////////////////////
		case 1000:
			/* james 2016.0926 
			nJobFlag = 0;
			if(st_tray_info[THD_REJECT_TRANSFER].nTrayExist == CTL_NO) //항상 empty 를 미리 집고있어야 한다, 플레그가 트레이를 집고있지 않은 상태로 traty PICK 작업을 진행한다  
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_RejTrsfVacChk,	IO_OFF); // 
				if(nRet_1 == IO_ON)
				{
					//트레이 있으면 알람 
					m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_RejTrsfVacChk); 
					CTL_Lib.Alarm_Error_Occurrence(m_nRunStep, dWARNING, m_strAlarmCode);
					break;
				}		

				
				m_nTransferJobFlag[0] = THD_REJECT_EMPTY_STACKER; 
				m_nTransferJobFlag[1] = WORK_PICK; 

				//트레이를 집고있지 않으니,  집으러 empty site에 가는 루틴으로 트레이를 집으러 간다
// 				st_sync_info.nRejectTransfer_Req[THD_REJECT_TRANSFER][0] = CTL_REQ;
// 				st_sync_info.nRejectTransfer_Req[THD_REJECT_TRANSFER][1] = WORK_PICK;//2016.0830 ybs
				st_sync_info.nRejectTransfer_Req[THD_REJECT_EMPTY_STACKER][0] = CTL_REQ;
				st_sync_info.nRejectTransfer_Req[THD_REJECT_EMPTY_STACKER][1] = WORK_PICK;

				m_nTransfer_WaitPosMove_Flag = CTL_NO;
				m_nRunStep = 2000;
				break; 
			} */

			nJobFlag = 0;
			if(st_sync_info.nRejectTransfer_Req[THD_REJECT_OUT_1_STACKER][0] == CTL_REQ) //reject #1 에 빈 트레이 공급을 요청 받았다 
			{
				nJobFlag += (int) pow((double)2, 0);// 작업할 위치 계산; //1
			}
			if(st_sync_info.nRejectTransfer_Req[THD_REJECT_OUT_2_STACKER][0] == CTL_REQ) //언로딩 #2 에 빈 트레이 공급을 요청 받았다 
			{
				nJobFlag += (int) pow((double)2, 1);// 작업할 위치 계산; //2
			}

			if(nJobFlag == 0 || st_lot_info[LOT_CURR].nLot_THD_Status[THD_WORK_RBT] ==  LOT_END) //james 2016.0926 			  
			{				
				//lot end 확인 
				if(st_lot_info[LOT_CURR].nLotStatus >= LOT_START && st_lot_info[LOT_NEXT].nLotStatus >= LOT_START ) //2 모드 다 동작시 
				{
					if(st_lot_info[LOT_CURR].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END && st_lot_info[LOT_NEXT].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END)
					{
						if(st_tray_info[THD_REJECT_TRANSFER].nTrayExist == CTL_YES) 
						{
							m_nTransferJobFlag[0] = THD_REJECT_EMPTY_STACKER; 
							m_nTransferJobFlag[1] = WORK_PLACE; 

							//이미 잡고 있는 트레이를 놓으러 empty site에 받을 준비하라고 요청한다 
							st_sync_info.nRejectTransfer_Req[THD_REJECT_EMPTY_STACKER][0] = CTL_REQ;
							st_sync_info.nRejectTransfer_Req[THD_REJECT_EMPTY_STACKER][1] = WORK_PLACE;

							m_nRunStep = 2000; //트레이를 놓으러 empty site로 이동한다 
							break;
						}
						else
						{
							st_lot_info[LOT_CURR].nLot_THD_Status[THD_REJECT_TRANSFER] =  LOT_END;
							st_lot_info[LOT_NEXT].nLot_THD_Status[THD_REJECT_TRANSFER] =  LOT_END;
							m_nRunStep = 0; 
							break;
						}
					}
					else
					{
						if(m_nTransfer_WaitPosMove_Flag == CTL_NO)
						{
							m_nRunStep = 1100;
							break;
						} 
					}
				}
				else if(st_lot_info[LOT_CURR].nLotStatus >= LOT_START)
				{
					if(st_lot_info[LOT_CURR].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END)
					{
						if(st_tray_info[THD_REJECT_TRANSFER].nTrayExist == CTL_YES) 
						{
							m_nTransferJobFlag[0] = THD_REJECT_EMPTY_STACKER; 
							m_nTransferJobFlag[1] = WORK_PLACE; 

							//이미 잡고 있는 트레이를 놓으러 empty site에 받을 준비하라고 요청한다 
							st_sync_info.nRejectTransfer_Req[THD_REJECT_EMPTY_STACKER][0] = CTL_REQ;
							st_sync_info.nRejectTransfer_Req[THD_REJECT_EMPTY_STACKER][1] = WORK_PLACE;

							m_nRunStep = 2000; //트레이를 놓으러 empty site로 이동한다 
							break;
						}
						else
						{
							st_lot_info[LOT_CURR].nLot_THD_Status[THD_REJECT_TRANSFER] =  LOT_END;
							m_nRunStep = 0; 
							break;
						}
					}
					else
					{
						if(m_nTransfer_WaitPosMove_Flag == CTL_NO)
						{
							m_nRunStep = 1100;
							break;
						} 
					}
				}
				else if(st_lot_info[LOT_NEXT].nLotStatus >= LOT_START )
				{
					if(st_lot_info[LOT_NEXT].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END)
					{
						if(st_tray_info[THD_REJECT_TRANSFER].nTrayExist == CTL_YES) 
						{
							m_nTransferJobFlag[0] = THD_EMPTY_STACKER; 
							m_nTransferJobFlag[1] = WORK_PLACE; 

							//이미 잡고 있는 트레이를 놓으러 empty site에 받을 준비하라고 요청한다 
							st_sync_info.nRejectTransfer_Req[THD_REJECT_EMPTY_STACKER][0] = CTL_REQ;
							st_sync_info.nRejectTransfer_Req[THD_REJECT_EMPTY_STACKER][1] = WORK_PLACE;

							m_nRunStep = 2000; //트레이를 놓으러 empty site로 이동한다 
							break;
						}
						else
						{
							st_lot_info[LOT_NEXT].nLot_THD_Status[THD_REJECT_TRANSFER] =  LOT_END;
							m_nRunStep = 0; 
							break;
						}
					}
					else
					{
						if(m_nTransfer_WaitPosMove_Flag == CTL_NO)
						{
							m_nRunStep = 1100;
							break;
						} 
					}
				}//else if	
			}//if //james 2016.0926  
			
			//불합리한 동작을 방지하기 위해 작업전에 트레이를 가지고 있는지 / 없는지 확인후 동작을 진행한다 
			if(st_tray_info[THD_REJECT_TRANSFER].nTrayExist == CTL_YES) //트레이를 가지고 있으면, 로딩이  
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_RejTrsfVacChk,	IO_ON); //
				if(nRet_1 == IO_OFF)
				{	//트레이가 없어서 에러발생  
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_RejTrsfVacChk); 
					CTL_Lib.Alarm_Error_Occurrence(8100, dWARNING, m_strAlarmCode);
					break;
				}	

				if(nJobFlag == 1) //reject #1 사이트에 빈 트레이 공급
				{ //집고있던 트레이를 reject #1 사이트에 빈 트레이 공급
					//m_nTransferJobFlag[2]; //트랜스퍼에 동작을 요청한다 , [0]:요청정보, [1]:1:트레이를 집기, 2:트레이를 놓기	
					m_nTransferJobFlag[0] = THD_REJECT_OUT_1_STACKER; 
					m_nTransferJobFlag[1] = WORK_PLACE; 
				}
				else if(nJobFlag == 2) //reject #2 사이트에 빈 트레이 공급
				{ //집고있던 트레이를 reject #2 사이트에 빈 트레이 공급
					//m_nTransferJobFlag[2]; //트랜스퍼에 동작을 요청한다 , [0]:요청정보, [1]:1:트레이를 집기, 2:트레이를 놓기	
					m_nTransferJobFlag[0] = THD_REJECT_OUT_2_STACKER; 
					m_nTransferJobFlag[1] = WORK_PLACE; 
				} 
				else if(nJobFlag == 3) //reject1, 2 모두 요청 
				{//1 부터 진행 
					m_nTransferJobFlag[0] = THD_REJECT_OUT_1_STACKER; 
					m_nTransferJobFlag[1] = WORK_PLACE; 
				}

				if(nJobFlag > 0) //작업 할 위치가 있다
				{
					m_nTransfer_WaitPosMove_Flag = CTL_NO;
					m_nRunStep = 2000;
				}
			}		
			else //if(st_tray_info[THD_REJECT_TRANSFER].nTrayExist == CTL_NO) //항상 empty 를 미리 집고있어야 한다, 플레그가 트레이를 집고있지 않은 상태로 traty PICK 작업을 진행한다  
			{//james 2016.0915 이곳으로 제어 위치 이동 
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_RejTrsfVacChk,	IO_OFF); // 
				if(nRet_1 == IO_ON)
				{
					//트레이 있으면 알람 
					m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_RejTrsfVacChk); 
					CTL_Lib.Alarm_Error_Occurrence(8110, dWARNING, m_strAlarmCode);
					break;
				}		

				m_nTransferJobFlag[0] = THD_REJECT_EMPTY_STACKER; 
				m_nTransferJobFlag[1] = WORK_PICK; 

				//트레이를 집고있지 않으니,  집으러 empty site에 가는 루틴으로 트레이를 집으러 간다
// 				st_sync_info.nRejectTransfer_Req[THD_REJECT_TRANSFER][0] = CTL_REQ;
// 				st_sync_info.nRejectTransfer_Req[THD_REJECT_TRANSFER][1] = WORK_PICK;//2016.0830 ybs
				st_sync_info.nRejectTransfer_Req[THD_REJECT_EMPTY_STACKER][0] = CTL_REQ;
				st_sync_info.nRejectTransfer_Req[THD_REJECT_EMPTY_STACKER][1] = WORK_PICK;

				m_nTransfer_WaitPosMove_Flag = CTL_NO;
				m_nRunStep = 2000;
				break; 
			} 
			break;

	 
		//////////////////////////////////////////////////////////////////////
		// 트랜스퍼를 reject empty stacker 대기 위치로 이송 시킨다 
		//////////////////////////////////////////////////////////////////////
		case 1100: //트랜스퍼 업 체크 
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_TrayTrsfUpChk,	IO_ON); //트랜스퍼가 피커를 들고 있는 조건 체크
			if(nRet_1 == IO_ON)
			{ //피커를 들고 있음 
				m_nRunStep = 1110;			
			}
			else 
			{
				m_nRunStep = 1200;			
			}
			break;

		case 1110://transfer up 
			Set_RejectTrayTransfer_UpDown(IO_OFF); 
			m_nRunStep = 1111;
			break;

		case 1111:
			nRet_1 = Chk_RejectTrayTransfer_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 1200;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(8120, dWARNING, m_strAlarmCode);
				m_nRunStep = 1100;
			}
			break;

		case 1200:
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nAxisNum, st_motor_info[m_nAxisNum].d_pos[P_REJECTTRANS_EMPTY_STACKER], COMI.mn_runspeed_rate);  //left로 미리 이동해 있는다 
			if (nRet_1 == BD_GOOD) //좌측으로 이동  
			{			
				m_nTransfer_WaitPosMove_Flag = CTL_YES;

				m_nRunStep = 1000;

				//2017.0109
				if( st_tray_info[THD_REJECT_TRANSFER].nStartStatus  == 1)
				{
					st_tray_info[THD_REJECT_TRANSFER].nStartStatus = 0;

					clsLog.LogTransfer(_T("REJECT_TRAY_TRANSFER"),
										_T("MOVE_EMPTY_TRAY"),
										1,
										st_tray_info[THD_REJECT_TRANSFER].strMaterial,
										_T("EMPTY_TRAY"),
										st_tray_info[THD_REJECT_TRANSFER].strMovePos[1], 
										st_tray_info[THD_REJECT_TRANSFER].strMovePos[0], 
										1, 
										st_tray_info[THD_REJECT_TRANSFER].strLogKey,
										st_tray_info[THD_REJECT_TRANSFER].strLogData);
				}
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nRunStep = 1200;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다  
				CTL_Lib.Alarm_Error_Occurrence(8130, dWARNING, st_alarm_info.strCode);
				m_nRunStep = 1200;
			}
			break; 


		//////////////////////////////////////////////////////////////////////
		// 해당 작업 위치로 가서 pick & place 하는 루틴 
		//////////////////////////////////////////////////////////////////////
		case 2000: //트랜스퍼 업 체크 
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_RejTrsfUpChk,	IO_ON); //트랜스퍼가 피커를 들고 있는 조건 체크
			if(nRet_1 == IO_ON)
			{ //피커를 들고 있음 
				m_nRunStep = 2100;			
			}
			else 
			{
				m_nRunStep = 2010;			
			}
			break;

		case 2010://transfer up 
			Set_RejectTrayTransfer_UpDown(IO_OFF); 
			m_nRunStep = 2011;
			break;

		case 2011:
			nRet_1 = Chk_RejectTrayTransfer_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 2100;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(8140, dWARNING, m_strAlarmCode);
				m_nRunStep = 2000;
			}
			break;

		case 2100: //작업 위치로 이동하는 루틴
			if(m_nTransferJobFlag[0] == THD_REJECT_EMPTY_STACKER)
			{
				m_dpTargetPos = st_motor_info[m_nAxisNum].d_pos[P_REJECTTRANS_EMPTY_STACKER];
			}
			else if(m_nTransferJobFlag[0] == THD_REJECT_OUT_1_STACKER)
			{
				m_dpTargetPos = st_motor_info[m_nAxisNum].d_pos[P_REJECTTRANS_REJECT_1_STACKER];
			}
			else if(m_nTransferJobFlag[0] == THD_REJECT_OUT_2_STACKER)
			{
				m_dpTargetPos = st_motor_info[m_nAxisNum].d_pos[P_REJECTTRANS_REJECT_2_STACKER];
			}
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nAxisNum, m_dpTargetPos, COMI.mn_runspeed_rate);  //left로 미리 이동해 있는다 
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
				CTL_Lib.Alarm_Error_Occurrence(8150, dWARNING, st_alarm_info.strCode);
				m_nRunStep = 2000;
			}
			break;  

		case 3000: //실제 작업하는 영역
			/* james 2016.0915 
			if(st_lot_info[LOT_CURR].nLot_THD_Status[THD_WORK_RBT] ==  LOT_END)
			{
				st_lot_info[LOT_CURR].nLot_THD_Status[THD_REJECT_TRANSFER] =  LOT_END;
				m_nRunStep = 0; 
				break;
			}*/


			if(m_nTransferJobFlag[0] == THD_REJECT_EMPTY_STACKER)
			{	 //스테커의 상태를 체크할 필요도 있음 	
				if(m_nTransferJobFlag[1] == WORK_PICK)
				{
					if(st_sync_info.nRejectTransfer_Req[THD_REJECT_EMPTY_STACKER][0] != CTL_READY) //empty에서 트레이를 공급하여 집어갈 수 있게 한다 
					{//empty stacker에서 트레이 공급하기까지 기다린다 
						break;
					}
				} 
				else if(m_nTransferJobFlag[1] == WORK_PLACE)
				{
					if(st_sync_info.nRejectTransfer_Req[THD_REJECT_EMPTY_STACKER][0] != CTL_READY) //empty에서 트레이를 받을 수 있 준비되었는지 기다린다 
					{//empty stacker에서 트레이 공급하기까지 기다린다 
						break;
					}
				}
			}
			else if(m_nTransferJobFlag[0] == THD_REJECT_OUT_1_STACKER)
			{	 //스테커의 상태를 체크할 필요도 있음 	
				if(st_sync_info.nRejectTransfer_Req[THD_REJECT_OUT_1_STACKER][0] != CTL_REQ) //빈 트레이 공급을 욫어했어야 한다 
				{
					break;;
				}
			}
			else if(m_nTransferJobFlag[0] == THD_REJECT_OUT_2_STACKER)
			{	 //스테커의 상태를 체크할 필요도 있음 	
				if(st_sync_info.nRejectTransfer_Req[THD_REJECT_OUT_2_STACKER][0] != CTL_REQ)//빈 트레이 공급을 욫어했어야 한다 
				{
					break;;
				}
			}
			m_nRunStep = 3100;
			break;

		case 3100:
			nRet_1 = COMI.Check_MotPosRange(m_nAxisNum, m_dpTargetPos, st_motor_info[m_nAxisNum].d_allow); 			
			if (nRet_1 != BD_GOOD)  //모두 정상인 위치 
			{//위치가 맞지 않으니 다시 확인하여 동작하자
				m_nRunStep = 2000;
				break;
			}

			Set_RejectTrayTransfer_UpDown(IO_ON);  //down

			if(m_nTransferJobFlag[1] == WORK_PICK)
			{  //pick 동작 , 미리동작 
				Set_RejectTransferEjectorOnOff(IO_OFF);
				Set_RejectTrayVacuum_OnOff(IO_ON); //트레이를 집기위해 vacuum을 키고 내려간다 
			}
			else //if(m_nTransferJobFlag[1] == TRAY_PLACE)
			{  //place 동작 
				// 완전히 
				//Set_RejectTrayVacuum_OnOff(IO_OFF);
				//Chk_RejectTrayVacuum_OnOff(IO_ON);
			}

			m_nRunStep = 3110;
			break;

		case 3110:
			nRet_1 = Chk_RejectTrayTransfer_UpDown(IO_ON); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 3200;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(8160, dWARNING, m_strAlarmCode);
				m_nRunStep = 3100;
			}
			break;

		case 3200:
			if(m_nTransferJobFlag[1] == WORK_PICK)
			{  //pick 동작 
				Set_RejectTransferEjectorOnOff(IO_OFF);
				Set_RejectTrayVacuum_OnOff(IO_ON);
			}
			else //if(m_nTransferJobFlag[1] == TRAY_PLACE)
			{  //place 동작 
				Set_RejectTrayVacuum_OnOff(IO_OFF);
				Set_RejectTransferEjectorOnOff(IO_ON);
			}
			m_nRunStep = 3210;
			break;

		case 3210: //체크
			if(m_nTransferJobFlag[1] == WORK_PICK)
			{  //pick 동작 				 
				nRet_1 = Chk_RejectTrayVacuum_OnOff(IO_ON);
				if(nRet_1 == RET_GOOD)
				{
					m_nRunStep = 3300;
				}
				else if(nRet_1 == RET_ERROR)
				{
					CTL_Lib.Alarm_Error_Occurrence(8170, dWARNING, m_strAlarmCode);
					m_nRunStep = 3200;
				}
			}
			else //if(m_nTransferJobFlag[1] == TRAY_PLACE)
			{  //place 동작 
				nRet_1 = Chk_RejectTrayVacuum_OnOff(IO_OFF);
				if(nRet_1 == RET_GOOD)
				{
					Set_RejectTransferEjectorOnOff(IO_OFF); //ejector off
					m_nRunStep = 3300;
				}
				else if(nRet_1 == RET_ERROR)
				{
					CTL_Lib.Alarm_Error_Occurrence(8180, dWARNING, m_strAlarmCode);
					m_nRunStep = 3200;
				}
			} 
			break;

		case 3300: //프렌스퍼 업 
			Set_RejectTrayTransfer_UpDown(IO_OFF); 
			m_nRunStep = 3310;
			break;

		case 3310:
			nRet_1 = Chk_RejectTrayTransfer_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 3400;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(8190, dWARNING, m_strAlarmCode);
				m_nRunStep = 3300;
			}
			break;

		case 3400:
			if(m_nTransferJobFlag[1] == WORK_PICK)
			{  //pick 동작 				 
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_RejTrsfVacChk,	IO_ON);  
				if(nRet_1 == IO_ON)
				{	//정상 					 
					m_nRunStep = 4000;
				}
				else
				{
					//트레이 있어야 한다 
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_RejTrsfVacChk); 
					CTL_Lib.Alarm_Error_Occurrence(8200, dWARNING, m_strAlarmCode);
					break;
				}	
			}
			else //if(m_nTransferJobFlag[1] == TRAY_PLACE)
			{  //place 동작 
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_RejTrsfVacChk,	IO_OFF); //CV tray pass check  없어야 한다 
				if(nRet_1 == IO_OFF)
				{//정상 					 
					m_nRunStep = 4000;
				}
				else
				{
					//트레이 없어야 한다 
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_RejTrsfVacChk); 
					CTL_Lib.Alarm_Error_Occurrence(8210, dWARNING, m_strAlarmCode); 
				}	
			} 
			break;

			

		/////////////////////////////////////////////////////
		// 트레이 data 정보 쉬프트 
		/////////////////////////////////////////////////////
		case 4000: //트레이 정보 셋팅 
			if(m_nTransferJobFlag[1] == WORK_PICK)
			{
				if(m_nTransferJobFlag[0] == THD_REJECT_EMPTY_STACKER)
				{
					clsFunc.Handler_Tray_DataInfo_Shift(0, CTL_NO, THD_REJECT_EMPTY_STACKER, THD_REJECT_TRANSFER); //트레이 정보를 empty stacker-> transfer로 쉬프트 한다 

					st_sync_info.nRejectTransfer_Req[THD_REJECT_EMPTY_STACKER][0] =  CTL_FREE; //작업 완료 
					st_tray_info[THD_REJECT_TRANSFER].nTrayExist = CTL_YES;
				} 
			}
			else //if(m_nTransferJobFlag[1] == TRAY_PLACE) //트랜스퍼 트레이 놓기 
			{
				st_tray_info[THD_REJECT_TRANSFER].nTrayExist = CTL_NO;//james 2016.0926 

				if(m_nTransferJobFlag[0] == THD_REJECT_EMPTY_STACKER)
				{
					clsFunc.Handler_Tray_DataInfo_Shift(1, CTL_YES, THD_REJECT_TRANSFER, THD_REJECT_EMPTY_STACKER); //트레이 정보를 load plate -> transfer로 쉬프트 한다 
					st_sync_info.nRejectTransfer_Req[THD_REJECT_EMPTY_STACKER][0] = CTL_FREE; //작업 완료 
					st_tray_info[THD_REJECT_EMPTY_STACKER].nTrayExist = CTL_YES; 
				}
				else if(m_nTransferJobFlag[0] == THD_REJECT_OUT_1_STACKER)  
				{
					clsFunc.Handler_Tray_DataInfo_Shift(1, CTL_YES, THD_REJECT_TRANSFER, THD_REJECT_OUT_1_STACKER); //트레이 정보를 load plate -> transfer로 쉬프트 한다 
				 
					st_sync_info.nRejectTransfer_Req[THD_REJECT_OUT_1_STACKER][0] = CTL_READY; //작업 완료 
					st_tray_info[THD_REJECT_OUT_1_STACKER].nTrayExist = CTL_YES;

					//2017.0109
					st_tray_info[THD_REJECT_TRANSFER].strLogKey[0]	= _T("REJECT_TRAY_TRANSFER");
					if(st_lot_info[LOT_CURR].strLotNo != _T("") && st_lot_info[LOT_CURR].nLotStatus >= LOT_START && st_lot_info[LOT_CURR].nLotStatus < LOT_END_START)
					{
						st_tray_info[THD_REJECT_TRANSFER].strLogData[0]	= st_lot_info[LOT_CURR].strLotNo;
					}
					else if(st_lot_info[LOT_NEXT].strLotNo != _T("") && st_lot_info[LOT_NEXT].nLotStatus >= LOT_START && st_lot_info[LOT_NEXT].nLotStatus < LOT_END_START)
					{
						st_tray_info[THD_REJECT_TRANSFER].strLogData[0]	= st_lot_info[LOT_NEXT].strLotNo;
					}
					else
					{
						st_tray_info[THD_REJECT_TRANSFER].strLogData[0]	= _T("$");
					}

					st_tray_info[THD_REJECT_TRANSFER].strMaterial = _T("$");
					st_tray_info[THD_REJECT_TRANSFER].nStartStatus = 1;
					st_tray_info[THD_REJECT_TRANSFER].strMovePos[0] = clsLog.GetDeviceID( THD_REJECT_OUT_1_STACKER );
					st_tray_info[THD_REJECT_TRANSFER].strMovePos[1] = clsLog.GetDeviceID( THD_REJECT_EMPTY_STACKER );

					clsLog.LogTransfer(_T("REJECT_TRAY_TRANSFER"),
										_T("MOVE_EMPTY_TRAY"),
										0,
										st_tray_info[THD_REJECT_TRANSFER].strMaterial,
										_T("EMPTY_TRAY"),
										st_tray_info[THD_REJECT_TRANSFER].strMovePos[0], 
										st_tray_info[THD_REJECT_TRANSFER].strMovePos[1], 
										1, 
										st_tray_info[THD_REJECT_TRANSFER].strLogKey,
										st_tray_info[THD_REJECT_TRANSFER].strLogData);

				}
				else if(m_nTransferJobFlag[0] == THD_REJECT_OUT_2_STACKER) //이곳에서 집을 일은 없을 것임 
				{
					clsFunc.Handler_Tray_DataInfo_Shift(1, CTL_YES, THD_REJECT_TRANSFER, THD_REJECT_OUT_2_STACKER); //트레이 정보를 load plate -> transfer로 쉬프트 한다 
					st_sync_info.nRejectTransfer_Req[THD_REJECT_OUT_2_STACKER][0] = CTL_READY; //작업 완료 
					st_tray_info[THD_REJECT_OUT_2_STACKER].nTrayExist = CTL_YES;

					//2017.0109
					st_tray_info[THD_REJECT_TRANSFER].strLogKey[0]	= _T("REJECT_TRAY_TRANSFER");
					if(st_lot_info[LOT_CURR].strLotNo != _T("") && st_lot_info[LOT_CURR].nLotStatus >= LOT_START && st_lot_info[LOT_CURR].nLotStatus < LOT_END_START)
					{
						st_tray_info[THD_REJECT_TRANSFER].strLogData[0]	= st_lot_info[LOT_CURR].strLotNo;
					}
					else if(st_lot_info[LOT_NEXT].strLotNo != _T("") && st_lot_info[LOT_NEXT].nLotStatus >= LOT_START && st_lot_info[LOT_NEXT].nLotStatus < LOT_END_START)
					{
						st_tray_info[THD_REJECT_TRANSFER].strLogData[0]	= st_lot_info[LOT_NEXT].strLotNo;
					}
					else
					{
						st_tray_info[THD_REJECT_TRANSFER].strLogData[0]	= _T("$");
					}

					st_tray_info[THD_REJECT_TRANSFER].strMaterial = _T("$");
					st_tray_info[THD_REJECT_TRANSFER].nStartStatus = 1;
					st_tray_info[THD_REJECT_TRANSFER].strMovePos[0] = clsLog.GetDeviceID( THD_REJECT_OUT_2_STACKER );
					st_tray_info[THD_REJECT_TRANSFER].strMovePos[1] = clsLog.GetDeviceID( THD_REJECT_EMPTY_STACKER );

					st_tray_info[THD_REJECT_TRANSFER].nStartStatus = 0;

					clsLog.LogTransfer(_T("REJECT_TRAY_TRANSFER"),
										_T("MOVE_EMPTY_TRAY"),
										0,
										st_tray_info[THD_REJECT_TRANSFER].strMaterial,
										_T("EMPTY_TRAY"),
										st_tray_info[THD_REJECT_TRANSFER].strMovePos[0], 
										st_tray_info[THD_REJECT_TRANSFER].strMovePos[1], 
										1, 
										st_tray_info[THD_REJECT_TRANSFER].strLogKey,
										st_tray_info[THD_REJECT_TRANSFER].strLogData);
					
				}
			}
			m_nRunStep = 1000; 
			 
			break;

		default:
			break;
	}
}


void CRunRejectTrayTransfer::Set_RejectTrayTransfer_UpDown( int nOnOff )
{
	CString strLogKey[10];
	CString	strLogData[10];

	strLogKey[0] = _T("Mode Start");
	strLogData[0].Format(_T("%d"),0);

	m_bTransferUpDownFlag = false;
	m_dwWaitTransferUpDown[0]	= GetCurrentTime();

	if(nOnOff == IO_OFF) //up
	{
		FAS_IO.set_out_bit( st_io_info.o_RejTrsfUpCyl, IO_ON);
		FAS_IO.set_out_bit( st_io_info.o_RejTrsfDnCyl, IO_OFF);
	}
	else //if(nOnOff == IO_ON)//down
	{
		FAS_IO.set_out_bit( st_io_info.o_RejTrsfUpCyl, IO_OFF);
		FAS_IO.set_out_bit( st_io_info.o_RejTrsfDnCyl, IO_ON);
	}
	//FAS_IO.set_out_bit( st_io_info.o_RejTrsfUpCyl, nOnOff);
	//FAS_IO.set_out_bit( st_io_info.o_RejTrsfDnCyl, !nOnOff);

	///kwlee 2016.0812
	if (nOnOff == IO_ON)
	{
		//LogFunction(_T("REJECT_TRAY_TRANSFER"),_T("UP"),_T("REJECT_TRAY_TRANSFER"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("REJECT_TRAY_TRANSFER"),_T("UP"),0,_T("REJECT_TRAY_TRANSFER"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
	else
	{
		//LogFunction(_T("REJECT_TRAY_TRANSFER"),_T("DOWN"),_T("REJECT_TRAY_TRANSFER"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("REJECT_TRAY_TRANSFER"),_T("DOWN"),0,_T("REJECT_TRAY_TRANSFER"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
}

int	CRunRejectTrayTransfer::Chk_RejectTrayTransfer_UpDown( int nOnOff )
{
	CString strLogKey[10];
	CString	strLogData[10];

	strLogKey[0] = _T("Mode End");
	strLogData[0].Format(_T("%d"),0);

//800411 1 0 "REJECT_TRANSFER_TRAY_UP_OFF_CHECK_ERROR"
//800412 1 0 "REJECT_TRANSFER_TRAY_DOWN_OFF_CHECK_ERROR"
//810411 1 0 "REJECT_TRANSFER_TRAY_UP_ON_CHECK_ERROR"
//810412 1 0 "REJECT_TRANSFER_TRAY_DOWN_ON_CHECK_ERROR"
	int nWaitTime = WAIT_TRANSFER_UP_DN;

	if (nOnOff == IO_OFF)//UP
	{
		if (m_bTransferUpDownFlag == false &&
			FAS_IO.get_in_bit(st_io_info.i_RejTrsfUpChk, IO_ON)	== IO_ON &&
			FAS_IO.get_in_bit(st_io_info.i_RejTrsfDnChk, IO_OFF)	== IO_OFF)
		{
			m_bTransferUpDownFlag			= true;
			m_dwWaitTransferUpDown[0]	= GetCurrentTime();
		}
		else if (m_bTransferUpDownFlag == true &&
			FAS_IO.get_in_bit(st_io_info.i_RejTrsfUpChk, IO_ON)	== IO_ON &&
			FAS_IO.get_in_bit(st_io_info.i_RejTrsfDnChk, IO_OFF)	== IO_OFF)
		{
			m_dwWaitTransferUpDown[1]	= GetCurrentTime();
			m_dwWaitTransferUpDown[2]	= m_dwWaitTransferUpDown[1] - m_dwWaitTransferUpDown[0];

			if (m_dwWaitTransferUpDown[2] <= 0)
			{
				m_dwWaitTransferUpDown[0]	= GetCurrentTime();
				return RET_PROCEED;
			}

			if(m_dwWaitTransferUpDown[2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
			{
				//LogFunction(_T("REJECT_TRAY_TRANSFER"),_T("UP"),_T("REJECT_TRAY_TRANSFER"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("REJECT_TRAY_TRANSFER"),_T("UP"),1,_T("REJECT_TRAY_TRANSFER"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_GOOD;
			}
		}
		else
		{
			m_dwWaitTransferUpDown[1]	= GetCurrentTime();
			m_dwWaitTransferUpDown[2]	= m_dwWaitTransferUpDown[1] - m_dwWaitTransferUpDown[0];

			if (m_dwWaitTransferUpDown[2] <= 0)
			{
				m_dwWaitTransferUpDown[0]	= GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwWaitTransferUpDown[2] > (DWORD)st_wait_info.nLimitWaitTime[nWaitTime])
			{
				m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_RejTrsfUpChk); 
				//2017.0109
				clsLog.LogFunction(_T("REJECT_TRAY_TRANSFER"),_T("UP"),1,_T("REJECT_TRAY_TRANSFER"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_ERROR;
			}
		}
		
	}
	else //if (nOnOff == IO_ON)//DOWN
	{
		if (m_bTransferUpDownFlag == false &&
			FAS_IO.get_in_bit(st_io_info.i_RejTrsfUpChk, IO_OFF) == IO_OFF &&
			FAS_IO.get_in_bit(st_io_info.i_RejTrsfDnChk, IO_ON)	== IO_ON)
		{
			m_bTransferUpDownFlag		= true;
			m_dwWaitTransferUpDown[0]	= GetCurrentTime();
		}
		else if (m_bTransferUpDownFlag == true &&
			FAS_IO.get_in_bit(st_io_info.i_RejTrsfUpChk, IO_OFF)	== IO_OFF &&
			FAS_IO.get_in_bit(st_io_info.i_RejTrsfDnChk, IO_ON)	== IO_ON)
		{
			m_dwWaitTransferUpDown[1] = GetCurrentTime();
			m_dwWaitTransferUpDown[2] = m_dwWaitTransferUpDown[1] - m_dwWaitTransferUpDown[0];

			if (m_dwWaitTransferUpDown[2] <= 0)
			{
				m_dwWaitTransferUpDown[0] = GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwWaitTransferUpDown[2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
			{
				//LogFunction(_T("REJECT_TRAY_TRANSFER"),_T("DOWN"),_T("REJECT_TRAY_TRANSFER"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("REJECT_TRAY_TRANSFER"),_T("DOWN"),1,_T("REJECT_TRAY_TRANSFER"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_GOOD;
			}
		}
		else
		{
			m_dwWaitTransferUpDown[1] = GetCurrentTime();
			m_dwWaitTransferUpDown[2] = m_dwWaitTransferUpDown[1] - m_dwWaitTransferUpDown[0];

			if (m_dwWaitTransferUpDown[2] <= 0)
			{
				m_dwWaitTransferUpDown[0] = GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwWaitTransferUpDown[2] > (DWORD)st_wait_info.nLimitWaitTime[nWaitTime])
			{
				m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_RejTrsfDnChk); 
				//2017.0109
				clsLog.LogFunction(_T("REJECT_TRAY_TRANSFER"),_T("DOWN"),1,_T("REJECT_TRAY_TRANSFER"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_ERROR;
			}
		}

	}

	return RET_PROCEED;
}



void CRunRejectTrayTransfer::Set_RejectTransferEjectorOnOff( int nOnOff )
{
	FAS_IO.set_out_bit(st_io_info.o_RejTrsfEjtOnOff, nOnOff);
}

void CRunRejectTrayTransfer::Set_RejectTrayVacuum_OnOff( int nOnOff )
{
	m_bVacuumOnOffFlag = false;
	m_dwWaitVacuulOnOff[0]	= GetCurrentTime();

	if( nOnOff == IO_ON )
	{
		FAS_IO.set_out_bit( st_io_info.o_RejTrsfVacOnOff, IO_ON);
		FAS_IO.set_out_bit( st_io_info.o_RejTrsfEjtOnOff, IO_OFF);
	}
	else
	{
		FAS_IO.set_out_bit( st_io_info.o_RejTrsfVacOnOff, IO_OFF);
		FAS_IO.set_out_bit( st_io_info.o_RejTrsfEjtOnOff, IO_ON);
	}


}

//	800413 1 0 "REJECT_TRANSFER_TRAY_VACUUM_OFF_CHECK_ERROR"
//	810413 1 0 "REJECT_TRANSFER_TRAY_VACUUM_ON_ON_CHECK_ERROR"
int	CRunRejectTrayTransfer::Chk_RejectTrayVacuum_OnOff( int nOnOff )
{
	int nWaitTime	= WAIT_TRANSFER_VACUUM_ON_OFF;

	if (nOnOff == IO_OFF)
	{
		if (m_bVacuumOnOffFlag == false && FAS_IO.get_in_bit(st_io_info.i_RejTrsfVacChk,	IO_OFF)	== IO_OFF)
		{
			m_bVacuumOnOffFlag		= true;
			m_dwWaitVacuulOnOff[0]	= GetCurrentTime();
		}
		else if (m_bVacuumOnOffFlag == true && FAS_IO.get_in_bit(st_io_info.i_RejTrsfVacChk,	IO_OFF)	== IO_OFF)
		{
			m_dwWaitVacuulOnOff[1] = GetCurrentTime();
			m_dwWaitVacuulOnOff[2] = m_dwWaitVacuulOnOff[1] - m_dwWaitVacuulOnOff[0];

			if (m_dwWaitVacuulOnOff[2] <= 0)
			{
				m_dwWaitVacuulOnOff[0] = GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwWaitVacuulOnOff[2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
			{
				return RET_GOOD;
			}
		}
		else
		{
			m_dwWaitVacuulOnOff[1] = GetCurrentTime();
			m_dwWaitVacuulOnOff[2] = m_dwWaitVacuulOnOff[1] - m_dwWaitVacuulOnOff[0];

			if (m_dwWaitVacuulOnOff[2] <= 0)
			{
				m_dwWaitVacuulOnOff[0] = GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwWaitVacuulOnOff[2] > (DWORD)st_wait_info.nLimitWaitTime[nWaitTime])
			{
				m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_RejTrsfVacChk);
				return RET_ERROR;
			}
		}

	}
	else
	{

		if (m_bVacuumOnOffFlag == false && FAS_IO.get_in_bit(st_io_info.i_RejTrsfVacChk,	IO_ON)	== IO_ON)
		{
			m_bVacuumOnOffFlag		= true;
			m_dwWaitVacuulOnOff[0]	= GetCurrentTime();
		}
		else if (m_bVacuumOnOffFlag == true && FAS_IO.get_in_bit(st_io_info.i_RejTrsfVacChk,	IO_ON)	== IO_ON)
		{
			m_dwWaitVacuulOnOff[1]	= GetCurrentTime();
			m_dwWaitVacuulOnOff[2]	= m_dwWaitVacuulOnOff[1] - m_dwWaitVacuulOnOff[0];

			if (m_dwWaitVacuulOnOff[2] <= 0)
			{
				m_dwWaitVacuulOnOff[0]	= GetCurrentTime();
				return RET_PROCEED;
			}

			if(m_dwWaitVacuulOnOff[2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
			{
				return RET_GOOD;
			}
		}
		else
		{
			m_dwWaitVacuulOnOff[1]	= GetCurrentTime();
			m_dwWaitVacuulOnOff[2]	= m_dwWaitVacuulOnOff[1] - m_dwWaitVacuulOnOff[0];

			if (m_dwWaitVacuulOnOff[2] <= 0)
			{
				m_dwWaitVacuulOnOff[0]	= GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwWaitVacuulOnOff[2] > (DWORD)st_wait_info.nLimitWaitTime[nWaitTime])
			{
				m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_RejTrsfVacChk);
				return RET_ERROR;
			}
		}	

	}

	return RET_PROCEED;
}