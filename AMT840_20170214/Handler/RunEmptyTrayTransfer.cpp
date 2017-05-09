// EmptyTrayTransfer.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "RunEmptyTrayTransfer.h"
#include "Variable.h"
#include "FastechPublic_IO.h"
#include "CtlBd_Library.h"

//james 2016.0802 
#include "CtlBd_Variable.h"
#include "PublicFunction.h"
#include "ComizoaPublic.h"
#include "math.h"
#include "LogCtrl.h"
#include "LogFromat.h"

// clsRunEmptyTrayTransfer
CRunEmptyTrayTransfer clsRunEmptyTrayTransfer;
CRunEmptyTrayTransfer::CRunEmptyTrayTransfer()
{
	m_nRunStep	= -1;
	m_nInitStep	= -1;	
	m_nEmptyToReject = 0;

	m_nAxisNum = M_WORKTRAY_TRANSFER;
}

CRunEmptyTrayTransfer::~CRunEmptyTrayTransfer()
{
}


// clsRunEmptyTrayTransfer 멤버 함수
void CRunEmptyTrayTransfer::OnThreadRun(void)
{
	//james 2016.0804
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

void CRunEmptyTrayTransfer::OnRunInit(void)
{
	int nRet_1, nRet_2=0;

	if (st_sync_info.nInit_Flag[THD_WORK_TRANSFER] != INIT_CLEAR)		return;		//INIT_CLEAR 일때만 초기화 작업을 한다. 초기화가 끝나면 INIT_CLEAR -> INIT_READY가 되기 떄문에...
	 
	switch (m_nInitStep)
	{
		case -1:		

			m_nInitStep = 0;
			break;

		case 0:		
			//kwlee 2016.1221 Empty Stacker Elevator 초기화 후 Empty Transfer  초기화 진행
			//if( st_sync_info.nInit_Flag[THD_LD_TRAY_PLATE] < INIT_READY) break;
			if( st_sync_info.nInit_Flag[THD_LD_TRAY_PLATE] < INIT_READY || st_sync_info.nInit_Flag[THD_EMPTY_STACKER] < INIT_READY) break;
			if(COMI.mn_homechk_flag[M_WORK_RBT_Z] != CTL_YES) break;//2015.0915

			m_nInitStep = 100;
			break;

		case 100: //up
			Set_EmptyTrayTransfer_UpDown(IO_OFF); 
			m_nInitStep = 110;
			break;

		case 110:
			nRet_1 = Chk_EmptyTrayTransfer_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nInitStep = 200;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(4500, dWARNING, m_strAlarmCode);
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
				CTL_Lib.Alarm_Error_Occurrence(4510, CTL_Lib.mn_run_status, COMI.strCode);
				m_nInitStep = 200;
			}			
			break;

		case 300:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_TrayTrsfVacChk,	IO_OFF);  
			if(nRet_1 == IO_OFF)
			{
				m_nInitStep = 400;
			}
			else
			{//트레이 존재 에러 
				m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_TrayTrsfVacChk); 
				CTL_Lib.Alarm_Error_Occurrence(4520, dWARNING, m_strAlarmCode);
			}
			break;

		case 400:
			Set_EmptyTrayVacuum_OnOff(IO_OFF );
			Set_EmptyTransferEjectorOnOff(IO_OFF );
			m_nInitStep = 900;
			break;

		case 900:
			st_sync_info.nInit_Flag[THD_WORK_TRANSFER] = INIT_READY; 
			 
			m_nInitStep = 0;
			break;	

		default :
			break;

	}
}

void CRunEmptyTrayTransfer::OnRunMove(void)
{
	int nRet_1, nRet_2=0, nRet_3=0;
	int nCount = 0, nJobFlag=0;

	clsFunc.OnThreadFunctionStepTrace(4, m_nRunStep);	

	switch (m_nRunStep)
	{ 
		case -1: 

			m_nRunStep = 0;
			break;

		case 0:
			if(st_lot_info[LOT_CURR].nLotStatus == LOT_START || st_lot_info[LOT_NEXT].nLotStatus == LOT_START )
			{
//				st_handler_info.mn_removetray = NO; //kwlee 2017.0117
				m_nRunStep = 100;
			}

			//////////////////////////////////////////////////////////////////////////////////////////////
			//랏을 투입과 동시에 자동으로 empty 트레이를 제거하자.
			//2017.0430
			if( ( st_lot_info[LOT_CURR].strLotNo !=_T("") && st_lot_info[LOT_NEXT].strLotNo ==_T("") )  || ( st_lot_info[LOT_CURR].strLotNo ==_T("") && st_lot_info[LOT_NEXT].strLotNo ==_T("") ) )
			{
				if( st_handler_info.mn_removetray == CTL_REQ && st_handler_info.mn_uldnum == 1 )
				{
					if( st_lot_info[LOT_CURR].strLotNo ==_T("") && st_lot_info[LOT_NEXT].strLotNo ==_T("") )
					{
						m_nRunStep = 100;	
					}
					else if( st_count_info.nInCount[0][0] <= 0 && st_lot_info[LOT_CURR].strLotNo !=_T("") && st_lot_info[LOT_NEXT].strLotNo ==_T("") && st_handler_info.mn_uldnum == 1)
					{
						m_nRunStep = 100;
					}
					else
					{
						st_handler_info.mn_removetray = CTL_NO;
					}
				}				
			}

			//////////////////////////////////////////////////////////////////////////////////////////////
			//2017.0116
			//if( st_lot_info[LOT_CURR].strLotNo !=_T("") || st_lot_info[LOT_NEXT].strLotNo !=_T("")	)
			if( st_lot_info[LOT_CURR].strLotNo ==_T("") && st_lot_info[LOT_NEXT].strLotNo ==_T(""))
			{
				if( st_handler_info.mn_removetray == CTL_REQ && st_handler_info.mn_uldnum == 1 )
				{
					m_nRunStep = 100;	
				}
			}
			break;

		case 100://up
			Set_EmptyTrayTransfer_UpDown(IO_OFF); 
			m_nRunStep = 110;
			break;

		case 110:
			nRet_1 = Chk_EmptyTrayTransfer_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 200;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(4600, dWARNING, m_strAlarmCode);
				m_nRunStep = 100;
			}
			break;

		case 200:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_TrayTrsfVacChk,	IO_OFF);  
			if(nRet_1 == IO_OFF)
			{
				m_nRunStep = 300;
			}
			else
			{//트레이 존재 에러 
				m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_TrayTrsfVacChk); 
				CTL_Lib.Alarm_Error_Occurrence(4610, dWARNING, m_strAlarmCode);
			}
			break;

		case 300:
			Set_EmptyTrayVacuum_OnOff(IO_OFF );
			Set_EmptyTransferEjectorOnOff(IO_OFF );

			m_nRunStep = 400;
			break;

		case 400:
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nAxisNum, st_motor_info[m_nAxisNum].d_pos[P_WORKTRANS_EMPTY_STACKER], COMI.mn_runspeed_rate);  //left로 미리 이동해 있는다 
			
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
				CTL_Lib.Alarm_Error_Occurrence(4620, dWARNING, st_alarm_info.strCode);
				m_nRunStep = 400;
			}
			break; 


		case 900: //기본적인 초기 정리가 완료된 상태 
			st_tray_info[THD_WORK_TRANSFER].nTrayExist = CTL_NO; //james 2016.0915
			m_nTransfer_WaitPosMove_Flag = CTL_NO;
			m_nRunStep = 1000;
			break;

		//////////////////////////////////////////////////////////////////////////
		// Tray 공급 및 교체 담당
		//////////////////////////////////////////////////////////////////////////
		case 1000:
// 			nJobFlag = 0;
// 			if(st_sync_info.nWorkTransfer_Req[THD_LD_TRAY_PLATE][0] == CTL_REQ) //Load Tray Plate트레이의 빈 트,레이 교체를 요청 
// 			{
// 				nJobFlag += (int) pow((double)2, 0);// 작업할 위치 계산; //1
// 			}
// 			if(st_sync_info.nWorkTransfer_Req[THD_ULD_1_STACKER][0] == CTL_REQ) //언로딩 #1 에 빈 트레이 공급을 요청 받았다 
// 			{
// 				nJobFlag += (int) pow((double)2, 1);// 작업할 위치 계산; //2
// 			}
// 			if(st_sync_info.nWorkTransfer_Req[THD_ULD_2_STACKER][0] == CTL_REQ) //언로딩 #2 에 빈 트레이 공급을 요청 받았다 
// 			{
// 				nJobFlag += (int) pow((double)2, 2);// 작업할 위치 계산; //4
// 			}
			//2017.0116
			if( st_handler_info.mn_removetray == CTL_REQ || st_handler_info.mn_removetray == CTL_CHANGE)
			{
				//if( st_lot_info[LOT_CURR].strLotNo ==_T("") && st_lot_info[LOT_NEXT].strLotNo ==_T("")	)//kwlee 2017.0125
				//{
					if(st_sync_info.nWorkTransfer_Req[THD_ULD_1_STACKER][0] == CTL_REQ) //언로딩 #1 에 빈 트레이 공급을 요청 받았다 
					{
						nJobFlag += (int) pow((double)2, 1);// 작업할 위치 계산; //2
					}
					if(st_sync_info.nWorkTransfer_Req[THD_ULD_2_STACKER][0] == CTL_REQ) //언로딩 #2 에 빈 트레이 공급을 요청 받았다 
					{
						nJobFlag += (int) pow((double)2, 2);// 작업할 위치 계산; //4
					}
				//}
			}
			else
			{			
				if(st_sync_info.nWorkTransfer_Req[THD_LD_TRAY_PLATE][0] == CTL_REQ) //Load Tray Plate트레이의 빈 트,레이 교체를 요청 
				{
					nJobFlag += (int) pow((double)2, 0);// 작업할 위치 계산; //1
				}
				if(st_sync_info.nWorkTransfer_Req[THD_ULD_1_STACKER][0] == CTL_REQ) //언로딩 #1 에 빈 트레이 공급을 요청 받았다 
				{
					nJobFlag += (int) pow((double)2, 1);// 작업할 위치 계산; //2
				}
				if(st_sync_info.nWorkTransfer_Req[THD_ULD_2_STACKER][0] == CTL_REQ) //언로딩 #2 에 빈 트레이 공급을 요청 받았다 
				{
					nJobFlag += (int) pow((double)2, 2);// 작업할 위치 계산; //4
				}				
			}
			////

			//if(nJobFlag == 0 || st_lot_info[LOT_CURR].nLot_THD_Status[THD_WORK_RBT] ==  LOT_END) //james 2016.0926 	
			//2017.0116
			if( st_handler_info.mn_removetray == CTL_NO && (nJobFlag == 0 || st_lot_info[LOT_CURR].nLot_THD_Status[THD_WORK_RBT] ==  LOT_END) )
			{
				//lot end 확인 
				if(st_lot_info[LOT_CURR].nLotStatus >= LOT_START && st_lot_info[LOT_NEXT].nLotStatus >= LOT_START )
				{
					if(st_lot_info[LOT_CURR].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END && st_lot_info[LOT_NEXT].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END)
					{
						if(st_tray_info[THD_WORK_TRANSFER].nTrayExist == CTL_YES) 
						{
							m_nTransferJobFlag[0] = THD_EMPTY_STACKER; 
							m_nTransferJobFlag[1] = WORK_PLACE; 

							//이미 잡고 있는 트레이를 놓으러 empty site에 받을 준비하라고 요청한다 
							st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] = CTL_REQ;
							st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][1] = WORK_PLACE;

							m_nRunStep = 2000; //트레이를 놓으러 empty site로 이동한다 
							break;
						}
						else
						{
							st_lot_info[LOT_CURR].nLot_THD_Status[THD_WORK_TRANSFER] =  LOT_END;
							st_lot_info[LOT_NEXT].nLot_THD_Status[THD_WORK_TRANSFER] =  LOT_END;
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
				else if(st_lot_info[LOT_CURR].nLotStatus >= LOT_START
					&& st_lot_info[LOT_NEXT].nLotStatus < LOT_START ) //james 2016.0924 
				{
					if(st_lot_info[LOT_CURR].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END)
					{
						if(st_tray_info[THD_WORK_TRANSFER].nTrayExist == CTL_YES) 
						{
							m_nTransferJobFlag[0] = THD_EMPTY_STACKER; 
							m_nTransferJobFlag[1] = WORK_PLACE; 

							//이미 잡고 있는 트레이를 놓으러 empty site에 받을 준비하라고 요청한다 
							st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] = CTL_REQ;
							st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][1] = WORK_PLACE;

							m_nRunStep = 2000; //트레이를 놓으러 empty site로 이동한다 
							break;
						}
						else
						{
							st_lot_info[LOT_CURR].nLot_THD_Status[THD_WORK_TRANSFER] =  LOT_END;
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
						if(st_tray_info[THD_WORK_TRANSFER].nTrayExist == CTL_YES) 
						{
							m_nTransferJobFlag[0] = THD_EMPTY_STACKER; 
							m_nTransferJobFlag[1] = WORK_PLACE; 

							//이미 잡고 있는 트레이를 놓으러 empty site에 받을 준비하라고 요청한다 
							st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] = CTL_REQ;
							st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][1] = WORK_PLACE;

							m_nRunStep = 2000; //트레이를 놓으러 empty site로 이동한다 
							break;
						}
						else
						{
							st_lot_info[LOT_NEXT].nLot_THD_Status[THD_WORK_TRANSFER] =  LOT_END;
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
			}//if
					
			/////////////////////////////////////////////////////////////////////////////////////////////////
			//불합리한 동작을 방지하기 위해 작업전에 트레이를 가지고 있는지 / 없는지 확인후 동작을 진행한다 
			/////////////////////////////////////////////////////////////////////////////////////////////////
			if(st_tray_info[THD_WORK_TRANSFER].nTrayExist == CTL_YES) //트레이를 가지고 있으면, 로딩이  
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_TrayTrsfVacChk,	IO_ON);  
				if(nRet_1 == IO_OFF)
				{	//트레이가 없어서 에러발생  
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_TrayTrsfVacChk); 
					CTL_Lib.Alarm_Error_Occurrence(4630, dWARNING, m_strAlarmCode);
					break;
				}	

				if(nJobFlag == 1) //오직 로딩트레만 집어가야 한다면 트레이를 놓고 가야 함 
				{ //집고있던 트레이를 empty stacker에 놓고 로딩 빈 트레이를 집으러 이동한다 
					//m_nTransferJobFlag[2]; //work 트랜스퍼에 동작을 요청한다 , [0]:요청정보, [1]:1:트레이를 집기, 2:트레이를 놓기	
					m_nTransferJobFlag[0] = THD_EMPTY_STACKER; 
					m_nTransferJobFlag[1] = WORK_PLACE; 

					//이미 잡고 있는 트레이를 놓으러 empty site에 받을 준비하라고 요청한다 
					st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] = CTL_REQ;
					st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][1] = WORK_PLACE;
				}
				else if(nJobFlag == 2 || nJobFlag == 3|| nJobFlag == 4 || nJobFlag == 5 || nJobFlag == 6 || nJobFlag == 7)
				{//집고 있는 트레이를 언로딩 작업 요청받은 스태커에 공급한다 
					if(nJobFlag == 2 || nJobFlag == 3 || nJobFlag == 6 || nJobFlag == 7) //unload #1 또는 #2 사이트에 트레이 공급 요청 받은 상태
					{
						m_nTransferJobFlag[0] = THD_ULD_1_STACKER; 
						m_nTransferJobFlag[1] = WORK_PLACE; 
					}
					else if(nJobFlag == 4 || nJobFlag == 5)
					{
						m_nTransferJobFlag[0] = THD_ULD_2_STACKER; 
						m_nTransferJobFlag[1] = WORK_PLACE; 
					}
				}
			}
			else //if(st_tray_info[THD_WORK_TRANSFER].nTrayExist == CTL_NO) //플레그가 트레이를 집고있지 않은 상태로 traty PICK 작업을 진행한다  
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_TrayTrsfVacChk,	IO_OFF);  				
				if(nRet_1 == IO_ON)
				{
					//트레이 있으면 알람 
					m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_TrayTrsfVacChk); 
					CTL_Lib.Alarm_Error_Occurrence(4640, dWARNING, m_strAlarmCode);
					break;
				}

// 				///////////////////////////////////////////////////////////////////////////////////////////////////////////
// 				//2017.0430
				if( st_handler_info.mn_auto_empty_tray == YES  && st_handler_info.mn_out_empty == YES )
				{
					m_nTransferJobFlag[0] = THD_EMPTY_STACKER; 
					m_nTransferJobFlag[1] = WORK_PICK; 
					st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] = CTL_REQ;
					st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][1] = WORK_PICK;
					nJobFlag = 100;
				}
				///////////////////////////////////////////////////////////////////////////////////////////////////////////
				else if(nJobFlag == 1 || nJobFlag == 3 || nJobFlag == 5 || nJobFlag == 7) //오직 로딩트레만 먼저 집어가서 처리한 후 다음작업한다  
				{ // 로딩 빈 트레이를 집으러 이동한다 
					//m_nTransferJobFlag[2]; //work 트랜스퍼에 동작을 요청한다 , [0]:요청정보, [1]:1:트레이를 집기, 2:트레이를 놓기	
					m_nTransferJobFlag[0] = THD_LD_TRAY_PLATE; 
					m_nTransferJobFlag[1] = WORK_PICK; 
				}
				else //if((nJobFlag == 2 || nJobFlag == 4 || nJobFlag == 6)// 언로딩 스태커 1,2 위치   nJobFlag == 8
				{//언로딩 스태커 사이트에서 트레이 공급을 요청한 상태
 
					m_nTransferJobFlag[0] = THD_EMPTY_STACKER; 
					m_nTransferJobFlag[1] = WORK_PICK; 

					//트레이를 집고있지 않으니,  집으러 empty site에 가는 루틴으로 트레이를 집으러 간다
					st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] = CTL_REQ;
					st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][1] = WORK_PICK;

					nJobFlag = 100; //james 2018.0828 

				}
			}

			if(nJobFlag > 0) //작업 할 위치가 있다
			{
				m_nTransfer_WaitPosMove_Flag = CTL_NO;
				m_nRunStep = 2000;
			}
			break;

	 
		//////////////////////////////////////////////////////////////////////
		// 트랜스퍼를 대기 위치로 이송 시킨다 
		//////////////////////////////////////////////////////////////////////
		case 1100: //트랜스퍼 업 체크 
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_TrayTrsfUpChk,	IO_ON); //트랜스퍼가 피커를 들고 있는 조건 체크
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_TrayTrsfDnChk,	IO_OFF); //james 2016.0921 
			if(nRet_1 == IO_ON && nRet_2 == IO_OFF)
			{ //피커를 들고 있음 
				m_nRunStep = 1200;		 //james 2016.0921 		
			}
			else 
			{
				m_nRunStep = 1110;		
			}
			break;

		case 1110://transfer up 
			Set_EmptyTrayTransfer_UpDown(IO_OFF); 
			m_nRunStep = 1111;
			break;

		case 1111:
			nRet_1 = Chk_EmptyTrayTransfer_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 1200;
			}
			else if(nRet_1 == RET_ERROR)
			{
// 				CTL_Lib.Alarm_Error_Occurrence(4650, dWARNING, m_strAlarmCode);
// 				m_nRunStep = 1100;
				//2017.0116
				if( st_handler_info.mn_removetray == CTL_CHANGE )//st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] = CTL_FREE;
				{
					m_nRunStep = 0;
				}
				else
				{
					CTL_Lib.Alarm_Error_Occurrence(4650, dWARNING, m_strAlarmCode);
					m_nRunStep = 1100;
				}
			}

			break;

		case 1200:
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nAxisNum, st_motor_info[m_nAxisNum].d_pos[P_WORKTRANS_EMPTY_STACKER], COMI.mn_runspeed_rate);  //left로 미리 이동해 있는다 
			if (nRet_1 == BD_GOOD) //좌측으로 이동  
			{			
				m_nTransfer_WaitPosMove_Flag = CTL_YES;

				m_nRunStep = 1000;

				//2017.0109
				if( st_tray_info[THD_WORK_TRANSFER].nStartStatus  == 1)
				{
					st_tray_info[THD_WORK_TRANSFER].nStartStatus = 0;

					clsLog.LogTransfer(_T("EMPTY_TRAY_TRANSFER"),
										_T("MOVE_EMPTY_TRAY"),
										1,
										st_tray_info[THD_WORK_TRANSFER].strMaterial,
										_T("EMPTY_TRAY"),
										st_tray_info[THD_WORK_TRANSFER].strMovePos[1], 
										st_tray_info[THD_WORK_TRANSFER].strMovePos[0], 
										1, 
										st_tray_info[THD_WORK_TRANSFER].strLogKey,
										st_tray_info[THD_WORK_TRANSFER].strLogData);
				}
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nRunStep = 1200;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다  
				CTL_Lib.Alarm_Error_Occurrence(4660, dWARNING, st_alarm_info.strCode);
				m_nRunStep = 1200;
			}
			break; 


		//////////////////////////////////////////////////////////////////////
		// 해당 작업 위치로 가서 pick & place 하는 루틴 
		//////////////////////////////////////////////////////////////////////
		case 2000: //트랜스퍼 업 체크 
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_TrayTrsfUpChk,	IO_ON); //트랜스퍼가 피커를 들고 있는 조건 체크
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_TrayTrsfDnChk,	IO_OFF); //james 2016.0921 
			if(nRet_1 == IO_ON && nRet_2 == IO_OFF)
			{ //피커를 들고 있음 
				m_nRunStep = 2100;			
			}
			else 
			{
				m_nRunStep = 2010;			
			}
			break;

		case 2010://transfer up 
			Set_EmptyTrayTransfer_UpDown(IO_OFF); 
			m_nRunStep = 2011;
			break;

		case 2011:
			nRet_1 = Chk_EmptyTrayTransfer_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 2100;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(4670, dWARNING, m_strAlarmCode);
				m_nRunStep = 2000;
			}
			break;

		case 2100: //작업 위치로 이동하는 루틴
			if(m_nTransferJobFlag[0] == THD_LD_TRAY_PLATE)
			{
				m_dpTargetPos = st_motor_info[m_nAxisNum].d_pos[P_WORKTRANS_LD_STACKER];
			}
			else if(m_nTransferJobFlag[0] == THD_EMPTY_STACKER)
			{
				m_dpTargetPos = st_motor_info[m_nAxisNum].d_pos[P_WORKTRANS_EMPTY_STACKER];
				///////////////////////////////////////////////////////////////////////
				//2017.0430
				if( st_handler_info.mn_auto_empty_tray == YES  && st_handler_info.mn_out_empty == YES )
				{
					m_dpTargetPos = st_motor_info[m_nAxisNum].d_pos[P_WORKTRANS_REJECT_STACK];
				}
				///////////////////////////////////////////////////////////////////////
			}
			else if(m_nTransferJobFlag[0] == THD_ULD_1_STACKER)
			{
				m_dpTargetPos = st_motor_info[m_nAxisNum].d_pos[P_WORKTRANS_ULD_1_STACKER];
				//20161203
				st_work_info.dwRearSmema[0][0] = GetTickCount();
			}
			else if(m_nTransferJobFlag[0] == THD_ULD_2_STACKER)
			{
				m_dpTargetPos = st_motor_info[m_nAxisNum].d_pos[P_WORKTRANS_ULD_2_STACKER];
				//20161203
				st_work_info.dwRearSmema[1][0] = GetTickCount();
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
				CTL_Lib.Alarm_Error_Occurrence(4680, dWARNING, st_alarm_info.strCode);
				m_nRunStep = 2000;
			}
			break; 

		case 3000: //실제 작업하는 영역
			if(m_nTransferJobFlag[0] == THD_LD_TRAY_PLATE)
			{
				nRet_1 = COMI.Get_MotIOSensor(M_LD_ELV, MOT_SENS_SD); 	
				if(nRet_1 != BD_GOOD) //로더 플레이트에 트레이가 감지 된 상태 
				{
					m_strAlarmCode.Format(_T("900001")); //900003 1 0 "LD_STACKER_PLATE_SD_TRAY_OFF_CHECK_ERROR" 
					CTL_Lib.Alarm_Error_Occurrence(4690, dWARNING, m_strAlarmCode);
					break;
				}
				//스테커의 상태를 체크할 필요도 있음 					
			}
			else if(m_nTransferJobFlag[0] == THD_EMPTY_STACKER)
			{	 //스테커의 상태를 체크할 필요도 있음 	
				if(m_nTransferJobFlag[1] == WORK_PICK)
				{
					// jtkim 20160902
					if(st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] == CTL_FREE) //empty에서 트레이를 공급하여 집어갈 수 있게 한다 
					{
						//m_nRunStep = 1000;
						//kwlee 2017.0126
						if( st_handler_info.mn_removetray == CTL_CHANGE )//st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] = CTL_FREE;
						{
							st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] = CTL_CLEAR;
							m_nRunStep = 0;
						}
						else
						{
							m_nRunStep = 1000;
						}
						break;
					}

					if(st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] != CTL_READY) //empty에서 트레이를 공급하여 집어갈 수 있게 한다 
					{//empty stacker에서 트레이 공급하기까지 기다린다 
						break;
					}

					nRet_1 = COMI.Get_MotIOSensor(M_EMPTY_ELV, MOT_SENS_SD); 	
					if(nRet_1 != BD_GOOD) //로더 플레이트에 트레이가 감지 된 상태 
					{
						m_strAlarmCode.Format(_T("900003")); //900003 1 0 "EMPTY_STACKER_PLATE_SD_TRAY_OFF_CHECK_ERROR"
						CTL_Lib.Alarm_Error_Occurrence(4700, dWARNING, m_strAlarmCode);
						break;
					}
				} 
				else if(m_nTransferJobFlag[1] == WORK_PLACE)
				{
					if(st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] != CTL_READY) //empty에서 트레이를 받을 수 있 준비되었는지 기다린다 
					{//empty stacker에서 트레이 공급하기까지 기다린다 
						break;
					}
				}
			}
			else if(m_nTransferJobFlag[0] == THD_ULD_1_STACKER)
			{ //스테커의 상태를 체크할 필요도 있음 	
				if(st_sync_info.nWorkTransfer_Req[THD_ULD_1_STACKER][0] != CTL_REQ) //빈 트레이 공급을 욫어했어야 한다 
				{
					break;
				}
			}
			else if(m_nTransferJobFlag[0] == THD_ULD_2_STACKER)
			{
				 //스테커의 상태를 체크할 필요도 있음 	
				if(st_sync_info.nWorkTransfer_Req[THD_ULD_2_STACKER][0] != CTL_REQ) //빈 트레이 공급을 욫어했어야 한다 
				{
					break;
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

			Set_EmptyTrayTransfer_UpDown(IO_ON); //down 

			if(m_nTransferJobFlag[1] == WORK_PICK)
			{  //pick 동작 , 미리동작 
				Set_EmptyTransferEjectorOnOff(IO_OFF);
				Set_EmptyTrayVacuum_OnOff(IO_ON); //vacuum을 미리 키고 내려간다 
			}
			else //if(m_nTransferJobFlag[1] == TRAY_PLACE)
			{  //place 동작 
				// 완전히 
				//Set_EmptyTrayVacuum_OnOff(IO_OFF);
				//Set_EmptyTransferEjectorOnOff(IO_ON);
			}

			m_nRunStep = 3110;
			break;

		case 3110:
			nRet_1 = Chk_EmptyTrayTransfer_UpDown(IO_ON); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 3200;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(4710, dWARNING, m_strAlarmCode);
				m_nRunStep = 3100;
			}
			break;

		case 3200:
			if(m_nTransferJobFlag[1] == WORK_PICK)
			{  //pick 동작 
				Set_EmptyTransferEjectorOnOff(IO_OFF);
				Set_EmptyTrayVacuum_OnOff(IO_ON);
			}
			else //if(m_nTransferJobFlag[1] == TRAY_PLACE)
			{  //place 동작 
				Set_EmptyTrayVacuum_OnOff(IO_OFF);
				Set_EmptyTransferEjectorOnOff(IO_ON);
			}
			m_nRunStep = 3210;
			break;

		case 3210: //체크
			if(m_nTransferJobFlag[1] == WORK_PICK)
			{  //pick 동작 				 
				nRet_1 = Chk_EmptyTrayVacuum_OnOff(IO_ON);
				if(nRet_1 == RET_GOOD)
				{
					m_nRunStep = 3300;
				}
				else if(nRet_1 == RET_ERROR)
				{
					Set_EmptyTrayTransfer_UpDown(IO_OFF); //up
					CTL_Lib.Alarm_Error_Occurrence(4720, dWARNING, m_strAlarmCode);
					m_nRunStep = 2000;
				}
			}
			else //if(m_nTransferJobFlag[1] == TRAY_PLACE)
			{  //place 동작 
				nRet_1 = Chk_EmptyTrayVacuum_OnOff(IO_OFF);
				if(nRet_1 == RET_GOOD)
				{
					Set_EmptyTransferEjectorOnOff(IO_OFF); //ejector off
					m_nRunStep = 3300;
				}
				else if(nRet_1 == RET_ERROR)
				{
					Set_EmptyTrayTransfer_UpDown(IO_OFF); //up
					CTL_Lib.Alarm_Error_Occurrence(4730, dWARNING, m_strAlarmCode);
					m_nRunStep = 2000;
				}
			} 
			break;

		case 3300: //프렌스퍼 업 
			Set_EmptyTrayTransfer_UpDown(IO_OFF); 
			m_nRunStep = 3310;
			break;

		case 3310:
			nRet_1 = Chk_EmptyTrayTransfer_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 3400;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(4740, dWARNING, m_strAlarmCode);
				m_nRunStep = 3300;
			}
			break;

		case 3400:
			if(m_nTransferJobFlag[1] == WORK_PICK)
			{  //pick 동작 				 
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_TrayTrsfVacChk,	IO_ON);  
				if(nRet_1 == IO_ON)
				{	//정상 					 
					m_nRunStep = 4000;
				}
				else
				{	//트레이 있어야 한다 
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_TrayTrsfVacChk); 
					CTL_Lib.Alarm_Error_Occurrence(4750, dWARNING, m_strAlarmCode);
					break;
				}	
			}
			else //if(m_nTransferJobFlag[1] == TRAY_PLACE)
			{  //place 동작 
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_TrayTrsfVacChk,	IO_OFF); //CV tray pass check  없어야 한다 
				if(nRet_1 == IO_OFF)
				{//정상 					
					m_nRunStep = 4000;
				}
				else
				{
					//트레이 없어야 한다 
					m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_TrayTrsfVacChk); 
					CTL_Lib.Alarm_Error_Occurrence(4760, dWARNING, m_strAlarmCode); 
				}	
			} 
			break;			

		case 4000: //트레이 정보 셋팅 
			if(m_nTransferJobFlag[1] == WORK_PICK)
			{
				if(m_nTransferJobFlag[0] == THD_LD_TRAY_PLATE)//트랜스퍼 트레이 집기 
				{
					clsFunc.Handler_Tray_DataInfo_Shift(1, CTL_YES, THD_LD_TRAY_PLATE, THD_WORK_TRANSFER); //트레이 정보를 load plate -> transfer로 쉬프트 한다 // Handler_Tray_DataInfo_Shift(int nMode, int nDvc_Yes_No, int nSend_SIte, int nRcv_Site)

					st_sync_info.nWorkTransfer_Req[THD_LD_TRAY_PLATE][0] = CTL_READY;
					st_tray_info[THD_WORK_TRANSFER].nTrayExist = CTL_YES;

					//2017.0109
					st_tray_info[THD_WORK_TRANSFER].strLogKey[0]	= _T("EMPRT_TRAY_TRANSFER");
					if(st_lot_info[LOT_CURR].strLotNo != _T("") && st_lot_info[LOT_CURR].nLotStatus >= LOT_START && st_lot_info[LOT_CURR].nLotStatus < LOT_END_START)
					{
						st_tray_info[THD_WORK_TRANSFER].strLogData[0]	= st_lot_info[LOT_CURR].strLotNo;
					}
					else if(st_lot_info[LOT_NEXT].strLotNo != _T("") && st_lot_info[LOT_NEXT].nLotStatus >= LOT_START && st_lot_info[LOT_NEXT].nLotStatus < LOT_END_START)
					{
						st_tray_info[THD_WORK_TRANSFER].strLogData[0]	= st_lot_info[LOT_NEXT].strLotNo;
					}
					else
					{
						st_tray_info[THD_WORK_TRANSFER].strLogData[0]	= _T("$");
					}

					st_tray_info[THD_WORK_TRANSFER].strMaterial = _T("$");
					st_tray_info[THD_WORK_TRANSFER].nStartStatus = 1;
					st_tray_info[THD_WORK_TRANSFER].strMovePos[0] = clsLog.GetDeviceID( THD_LD_TRAY_PLATE );
					st_tray_info[THD_WORK_TRANSFER].strMovePos[1] = clsLog.GetDeviceID( THD_EMPTY_STACKER );


					clsLog.LogTransfer(_T("EMPTY_TRAY_TRANSFER"),
										_T("MOVE_EMPTY_TRAY"),
										0,
										st_tray_info[THD_WORK_TRANSFER].strMaterial,
										_T("EMPTY_TRAY"),
										st_tray_info[THD_WORK_TRANSFER].strMovePos[0], 
										st_tray_info[THD_WORK_TRANSFER].strMovePos[1], 
										1, 
										st_tray_info[THD_WORK_TRANSFER].strLogKey,
										st_tray_info[THD_WORK_TRANSFER].strLogData);


				}
				else if(m_nTransferJobFlag[0] == THD_EMPTY_STACKER)
				{
					clsFunc.Handler_Tray_DataInfo_Shift(1, CTL_YES, THD_EMPTY_STACKER, THD_WORK_TRANSFER); //트레이 정보를 empty stacker -> transfer로 쉬프트 한다 
					st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] = CTL_FREE;
					st_tray_info[THD_WORK_TRANSFER].nTrayExist = CTL_YES;
				}
			}
			else //if(m_nTransferJobFlag[1] == TRAY_PLACE) //트랜스퍼 트레이 놓기 
			{				
				if(m_nTransferJobFlag[0] == THD_EMPTY_STACKER)
				{
// 					clsFunc.Handler_Tray_DataInfo_Shift(1, CTL_YES, THD_WORK_TRANSFER, THD_EMPTY_STACKER); //트레이 정보를 transfer -> empty stacker로 쉬프트 한다 
// 
// 					st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] = CTL_FREE; //놓았다 
// 					st_tray_info[THD_WORK_TRANSFER].nTrayExist =  CTL_NO;

					////////////////////////////////////////////////////////////////////////////////////////////////////////
					//2017.0430
					if( st_handler_info.mn_auto_empty_tray == YES && st_handler_info.mn_out_empty == YES)
					{
						clsFunc.Handler_Tray_DataInfo_Shift(1, CTL_YES, THD_WORK_TRANSFER, THD_REJECT_BUFFER_STACK);

						st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] = CTL_FREE; //놓았다 
						st_tray_info[THD_WORK_TRANSFER].nTrayExist =  CTL_NO;

						m_nEmptyToReject++;
						if( m_nEmptyToReject > 6 )//6장정도를 미리 버린다. 한번은 empty tray stacker에 놓을 수도 있다.
						{
							st_handler_info.mn_out_empty = NO;
							m_nEmptyToReject = 0;
						}
					}
					////////////////////////////////////////////////////////////////////////////////////////////////////////	
					else
					{
						clsFunc.Handler_Tray_DataInfo_Shift(1, CTL_YES, THD_WORK_TRANSFER, THD_EMPTY_STACKER); //트레이 정보를 transfer -> empty stacker로 쉬프트 한다 
						st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] = CTL_FREE; //놓았다 
						st_tray_info[THD_WORK_TRANSFER].nTrayExist =  CTL_NO;	
					}
				}
				else if(m_nTransferJobFlag[0] == THD_ULD_1_STACKER)  
				{
					clsFunc.Handler_Tray_DataInfo_Shift(1, CTL_YES, THD_WORK_TRANSFER, THD_ULD_1_STACKER); //트레이 정보를 transfer -> uld 1로 쉬프트 한다 

					st_sync_info.nWorkTransfer_Req[THD_ULD_1_STACKER][0] = CTL_READY;
					st_tray_info[THD_WORK_TRANSFER].nTrayExist =  CTL_NO;

					//2016.1203
					st_work_info.dwRearSmema[0][1] = GetTickCount();

					//2017.0109
					st_tray_info[THD_WORK_TRANSFER].strLogKey[0]	= _T("EMPRT_TRAY_TRANSFER");
					if(st_lot_info[LOT_CURR].strLotNo != _T("") && st_lot_info[LOT_CURR].nLotStatus >= LOT_START && st_lot_info[LOT_CURR].nLotStatus < LOT_END_START)
					{
						st_tray_info[THD_WORK_TRANSFER].strLogData[0]	= st_lot_info[LOT_CURR].strLotNo;
					}
					else if(st_lot_info[LOT_NEXT].strLotNo != _T("") && st_lot_info[LOT_NEXT].nLotStatus >= LOT_START && st_lot_info[LOT_NEXT].nLotStatus < LOT_END_START)
					{
						st_tray_info[THD_WORK_TRANSFER].strLogData[0]	= st_lot_info[LOT_NEXT].strLotNo;
					}
					else
					{
						st_tray_info[THD_WORK_TRANSFER].strLogData[0]	= _T("$");
					}

					st_tray_info[THD_WORK_TRANSFER].strMaterial = _T("$");
					st_tray_info[THD_WORK_TRANSFER].nStartStatus = 1;
					st_tray_info[THD_WORK_TRANSFER].strMovePos[0] = clsLog.GetDeviceID( THD_EMPTY_STACKER );
					st_tray_info[THD_WORK_TRANSFER].strMovePos[1] = clsLog.GetDeviceID( THD_ULD_1_STACKER );

					clsLog.LogTransfer(_T("EMPTY_TRAY_TRANSFER"),
										_T("MOVE_EMPTY_TRAY"),
										0,
										st_tray_info[THD_WORK_TRANSFER].strMaterial,
										_T("EMPTY_TRAY"),
										st_tray_info[THD_WORK_TRANSFER].strMovePos[0], 
										st_tray_info[THD_WORK_TRANSFER].strMovePos[1], 
										1, 
										st_tray_info[THD_WORK_TRANSFER].strLogKey,
										st_tray_info[THD_WORK_TRANSFER].strLogData);
				}
				else if(m_nTransferJobFlag[0] == THD_ULD_2_STACKER) //이곳에서 집을 일은 없을 것임 
				{
					clsFunc.Handler_Tray_DataInfo_Shift(1, CTL_YES, THD_WORK_TRANSFER, THD_ULD_2_STACKER); //트레이 정보를 transfer -> uld 2 로 쉬프트 한다 

					st_sync_info.nWorkTransfer_Req[THD_ULD_2_STACKER][0] = CTL_READY;
					st_tray_info[THD_WORK_TRANSFER].nTrayExist =  CTL_NO;

					//2016.1203
					st_work_info.dwRearSmema[1][1] = GetTickCount();

					//2017.0109
					st_tray_info[THD_WORK_TRANSFER].strLogKey[0]	= _T("EMPRT_TRAY_TRANSFER");
					if(st_lot_info[LOT_CURR].strLotNo != _T("") && st_lot_info[LOT_CURR].nLotStatus >= LOT_START && st_lot_info[LOT_CURR].nLotStatus < LOT_END_START)
					{
						st_tray_info[THD_WORK_TRANSFER].strLogData[0]	= st_lot_info[LOT_CURR].strLotNo;
					}
					else if(st_lot_info[LOT_NEXT].strLotNo != _T("") && st_lot_info[LOT_NEXT].nLotStatus >= LOT_START && st_lot_info[LOT_NEXT].nLotStatus < LOT_END_START)
					{
						st_tray_info[THD_WORK_TRANSFER].strLogData[0]	= st_lot_info[LOT_NEXT].strLotNo;
					}
					else
					{
						st_tray_info[THD_WORK_TRANSFER].strLogData[0]	= _T("$");
					}

					st_tray_info[THD_WORK_TRANSFER].strMaterial = _T("$");
					st_tray_info[THD_WORK_TRANSFER].nStartStatus = 1;
					st_tray_info[THD_WORK_TRANSFER].strMovePos[0] = clsLog.GetDeviceID( THD_EMPTY_STACKER );
					st_tray_info[THD_WORK_TRANSFER].strMovePos[1] = clsLog.GetDeviceID( THD_ULD_2_STACKER );

					clsLog.LogTransfer(_T("EMPTY_TRAY_TRANSFER"),
										_T("MOVE_EMPTY_TRAY"),
										0,
										st_tray_info[THD_WORK_TRANSFER].strMaterial,
										_T("EMPTY_TRAY"),
										st_tray_info[THD_WORK_TRANSFER].strMovePos[0], 
										st_tray_info[THD_WORK_TRANSFER].strMovePos[1], 
										1, 
										st_tray_info[THD_WORK_TRANSFER].strLogKey,
										st_tray_info[THD_WORK_TRANSFER].strLogData);
				}	
			}
			m_nRunStep = 1000; 			 
			break;

		default:
			break;
	}
}




void CRunEmptyTrayTransfer::Set_EmptyTrayTransfer_UpDown( int nOnOff )
{

	CString strLogKey[10];
	CString	strLogData[10];

	strLogKey[0] = _T("Mode Start");
	strLogData[0].Format(_T("%d"),0);

	m_bTransferUpDownFlag = false;
	m_dwWaitTransferUpDown[0]	= GetCurrentTime();
	if(nOnOff == IO_OFF) //up
	{
		FAS_IO.set_out_bit( st_io_info.o_TrayTrsfUpCyl, IO_ON);
		FAS_IO.set_out_bit( st_io_info.o_TrayTrsfDnCyl, IO_OFF);
	}
	else //if(nOnOff == IO_ON)//down
	{
		FAS_IO.set_out_bit( st_io_info.o_TrayTrsfUpCyl, IO_OFF);
		FAS_IO.set_out_bit( st_io_info.o_TrayTrsfDnCyl, IO_ON);
	}

	

	///kwlee 2016.0812
	if (nOnOff == IO_ON)
	{
		//LogFunction(_T("EMPTY_TRAY_TRANSFER"),_T("UP"),_T("TRAY_TRANSFER"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("EMPTY_TRAY_TRANSFER"),_T("UP"),0,_T("TRAY_TRANSFER"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
	else
	{
		//LogFunction(_T("EMPTY_TRAY_TRANSFER"),_T("DOWN"),_T("TRAY_TRANSFER"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("EMPTY_TRAY_TRANSFER"),_T("DOWN"),0,_T("TRAY_TRANSFER"),_T("CYLINDER"),1,strLogKey,strLogData);
	}

}

int	CRunEmptyTrayTransfer::Chk_EmptyTrayTransfer_UpDown( int nOnOff )
{
//800408 1 0 "TRAY_TRANSFER_TRAY_UP_OFF_CHECK_ERROR"
//800409 1 0 "TRAY_TRANSFER_TRAY_DOWN_OFF_CHECK_ERROR"
//810408 1 0 "TRAY_TRANSFER_TRAY_UP_ON_CHECK_ERROR"
//810409 1 0 "TRAY_TRANSFER_TRAY_DOWN_ON_CHECK_ERROR"
	int nWaitTime = WAIT_TRANSFER_UP_DN;

	CString strLogKey[10];
	CString	strLogData[10];


	strLogKey[0] = _T("Mode End");
	strLogData[0].Format(_T("%d"),0);

	if (nOnOff == IO_OFF)//UP
	{
		if (m_bTransferUpDownFlag == false &&
			FAS_IO.get_in_bit(st_io_info.i_TrayTrsfUpChk, IO_ON) == IO_ON &&
			FAS_IO.get_in_bit(st_io_info.i_TrayTrsfDnChk, IO_OFF) == IO_OFF)
		{
			m_bTransferUpDownFlag = true;
			m_dwWaitTransferUpDown[0] = GetCurrentTime();
		}
		else if (m_bTransferUpDownFlag == true &&
			FAS_IO.get_in_bit(st_io_info.i_TrayTrsfUpChk, IO_ON) == IO_ON &&
			FAS_IO.get_in_bit(st_io_info.i_TrayTrsfDnChk, IO_OFF) == IO_OFF)
		{
			m_dwWaitTransferUpDown[1] = GetCurrentTime();
			m_dwWaitTransferUpDown[2] = m_dwWaitTransferUpDown[1] - m_dwWaitTransferUpDown[0];

			if (m_dwWaitTransferUpDown[2] <= 0)
			{
				m_dwWaitTransferUpDown[0] = GetCurrentTime();
				return RET_PROCEED;
			}

			if(m_dwWaitTransferUpDown[2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
			{
				//LogFunction(_T("EMPTY_TRAY_TRANSFER"),_T("UP"),_T("TRAY_TRANSFER"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("EMPTY_TRAY_TRANSFER"),_T("UP"),1,_T("TRAY_TRANSFER"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_GOOD;
			}
		}
		else
		{
			m_dwWaitTransferUpDown[1] = GetCurrentTime();
			m_dwWaitTransferUpDown[2] = m_dwWaitTransferUpDown[1] - m_dwWaitTransferUpDown[0];

			if (m_dwWaitTransferUpDown[2] <= 0)
			{
				m_dwWaitTransferUpDown[0]	= GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwWaitTransferUpDown[2] > (DWORD)st_wait_info.nLimitWaitTime[nWaitTime])
			{
				//m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_TrayTrsfUpChk); 
				//2017.0109
				clsLog.LogFunction(_T("EMPTY_TRAY_TRANSFER"),_T("UP"),1,_T("TRAY_TRANSFER"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_ERROR;
			}
		}
		
	}
	else
	{
		if (m_bTransferUpDownFlag == false &&
			FAS_IO.get_in_bit(st_io_info.i_TrayTrsfUpChk, IO_OFF) == IO_OFF &&
			FAS_IO.get_in_bit(st_io_info.i_TrayTrsfDnChk, IO_ON) == IO_ON)
		{
			m_bTransferUpDownFlag		= true;
			m_dwWaitTransferUpDown[0]	= GetCurrentTime();
		}
		else if (m_bTransferUpDownFlag == true &&
			FAS_IO.get_in_bit(st_io_info.i_TrayTrsfUpChk, IO_OFF) == IO_OFF &&
			FAS_IO.get_in_bit(st_io_info.i_TrayTrsfDnChk, IO_ON) == IO_ON)
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
				//LogFunction(_T("EMPTY_TRAY_TRANSFER"),_T("DOWN"),_T("TRAY_TRANSFER"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("EMPTY_TRAY_TRANSFER"),_T("DOWN"),1,_T("TRAY_TRANSFER"),_T("CYLINDER"),1,strLogKey,strLogData); 
				return RET_GOOD;
			}
		}
		else//DOWN
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
				m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_TrayTrsfDnChk); 
				//2017.0109
				clsLog.LogFunction(_T("EMPTY_TRAY_TRANSFER"),_T("DOWN"),1,_T("TRAY_TRANSFER"),_T("CYLINDER"),1,strLogKey,strLogData); 
				return RET_ERROR;
			}
		}
	}

	return RET_PROCEED;
}

void CRunEmptyTrayTransfer::Set_EmptyTransferEjectorOnOff( int nOnOff )
{

	FAS_IO.set_out_bit(st_io_info.o_TrayTrsfEjtOnOff, nOnOff);
}

void CRunEmptyTrayTransfer::Set_EmptyTrayVacuum_OnOff( int nOnOff )
{
	m_bVacuumOnOffFlag = false;
	m_dwWaitVacuulOnOff[0]	= GetCurrentTime();

	if( nOnOff == IO_ON )
	{
		FAS_IO.set_out_bit( st_io_info.o_TrayTrsfVacOnOff, IO_ON);
		FAS_IO.set_out_bit( st_io_info.o_TrayTrsfEjtOnOff, IO_OFF);
	}
	else
	{
		FAS_IO.set_out_bit( st_io_info.o_TrayTrsfVacOnOff, IO_OFF);
		FAS_IO.set_out_bit( st_io_info.o_TrayTrsfEjtOnOff, IO_ON);
	}


}

//	800410 1 0 "TRAY_TRANSFER_TRAY_VACUUM_ON_OFF_CHECK_ERROR"
//	810410 1 0 "TRAY_TRANSFER_TRAY_VACUUM_ON_CHECK_ERROR"
int	CRunEmptyTrayTransfer::Chk_EmptyTrayVacuum_OnOff( int nOnOff )
{
	int nWaitTime= WAIT_TRANSFER_VACUUM_ON_OFF;


	if (nOnOff == IO_OFF)
	{
		if (st_basic_info.nModeDevice == WITHOUT_TRAY || 1)
		{
			if (m_bVacuumOnOffFlag == false)
			{
				m_bVacuumOnOffFlag		= true;
				m_dwWaitVacuulOnOff[0]	= GetCurrentTime();
			}
			else if (m_bVacuumOnOffFlag == true)
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

		}
		else
		{
			if (m_bVacuumOnOffFlag == false && FAS_IO.get_in_bit(st_io_info.i_TrayTrsfVacChk,	IO_OFF)	== IO_OFF)
			{
				m_bVacuumOnOffFlag		= true;
				m_dwWaitVacuulOnOff[0]	= GetCurrentTime();
			}
			else if (m_bVacuumOnOffFlag == true && FAS_IO.get_in_bit(st_io_info.i_TrayTrsfVacChk,	IO_OFF)	== IO_OFF)
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
					m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_TrayTrsfVacChk);
					return RET_ERROR;
				}
			}
		}
	}
	else
	{
		if (st_basic_info.nModeDevice == WITH_DVC || 1)
		{
			if (m_bVacuumOnOffFlag == false && FAS_IO.get_in_bit(st_io_info.i_TrayTrsfVacChk,	IO_ON)	== IO_ON)
			{
				m_bVacuumOnOffFlag		= true;
				m_dwWaitVacuulOnOff[0]	= GetCurrentTime();
			}
			else if (m_bVacuumOnOffFlag == true && FAS_IO.get_in_bit(st_io_info.i_TrayTrsfVacChk,	IO_ON)	== IO_ON)
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
					m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_TrayTrsfVacChk);
					return RET_ERROR;
				}
			}
		}
		else
		{
			if (m_bVacuumOnOffFlag == false)
			{
				m_bVacuumOnOffFlag			= true;
				m_dwWaitVacuulOnOff[0]	= GetCurrentTime();
			}
			else if (m_bVacuumOnOffFlag == true)
			{
				m_dwWaitVacuulOnOff[1]	= GetCurrentTime();
				m_dwWaitVacuulOnOff[2]	= m_dwWaitVacuulOnOff[1] - m_dwWaitVacuulOnOff[0];


				if (m_dwWaitVacuulOnOff[2] <= 0)
				{
					m_dwWaitVacuulOnOff[0]	= GetCurrentTime();
					return RET_PROCEED;
				}

				else if(m_dwWaitVacuulOnOff[2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
				{
					RET_GOOD;
				}
			}
		}
	}

	return RET_PROCEED;
}