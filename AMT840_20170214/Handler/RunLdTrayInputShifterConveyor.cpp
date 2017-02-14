// LdTrayInputShifterConveyor.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "RunLdTrayInputShifterConveyor.h"
#include "Variable.h"
#include "FastechPublic_IO.h"
#include "CtlBd_Library.h"

//james 2016.0802 
#include "CtlBd_Variable.h"
#include "PublicFunction.h"
#include "ComizoaPublic.h"
#include "LogCtrl.h"
#include "AlgMemory.h"
#include "LogFromat.h"

// CLdTrayInputShifterConveyor
CRunLdTrayInputShifterConveyor clsRunLdTrayInputShifterConveyor;
CRunLdTrayInputShifterConveyor::CRunLdTrayInputShifterConveyor()
{
	m_nRunStep	= -1;
	m_nInitStep	= -1;

	m_nShiftMotor_Work_Site = CV_LEFT;//모터 작업 순서 정의 0(left)->1(right)->0->1 반복 

	m_nAxisNum = M_LD_CV_LR_SHIFTER;
}

CRunLdTrayInputShifterConveyor::~CRunLdTrayInputShifterConveyor()
{
}


// CLdTrayInputShifterConveyor 멤버 함수
void CRunLdTrayInputShifterConveyor::OnThreadRun(void)
{
	//james 2016.0730
	if (st_handler_info.nSystemLock != FALSE)  return;	// 현재 시스템 Lock 상태인 경우 리턴 

	Process_LoadTray_Insert(CV_LEFT); //Left 트레이 입력을  제어하는 루틴		 
	Process_LoadTray_Insert(CV_RIGHT); //Right 트레이 입력을  제어하는 루틴

	CTL_Lib.Process_Tray_Conveyor_AcMotor(0, THD_LD_CV_STACKER_LEFT, st_io_info.o_ReadyLd1CvyAcMotOnOff ); //Left Conveyor AC Motor 제어하는 루틴		 
	CTL_Lib.Process_Tray_Conveyor_AcMotor(0, THD_LD_CV_STACKER_RIGHT,  st_io_info.o_ReadyLd2CvyAcMotOnOff); //Right Conveyor AC Motor 제어하는 루틴	
	

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
 
void CRunLdTrayInputShifterConveyor::OnRunInit(void)
{
	 
	int nRet_1, nRet_2;

	if (st_sync_info.nInit_Flag[THD_LD_CV_STACKER_LEFT] != INIT_CLEAR && st_sync_info.nInit_Flag[THD_LD_CV_STACKER_RIGHT] != INIT_CLEAR)		return;		//INIT_CLEAR 일때만 초기화 작업을 한다. 초기화가 끝나면 INIT_CLEAR -> INIT_READY가 되기 떄문에...
	 
	switch (m_nInitStep)
	{
		case -1:
			//if (st_sync_info.nInit_Flag[THD_LD_STACKER] == INIT_CLEAR) break;

			m_nInitStep = 0;
			break;

		case 0:		

			m_nInitStep = 100;
			break;

		case 100:
			st_sync_info.nConveyor_AcMotorCtl_Req[THD_LD_CV_STACKER_LEFT] = CTL_FREE; //ac  motor stop 
			st_sync_info.nConveyor_AcMotorCtl_Req[THD_LD_CV_STACKER_RIGHT] = CTL_FREE; //ac  motor stop 
			m_nInitStep = 200;
			break;

		case 200: //stopper를 UP/올려 
			Set_ConveyorStopper_UpDown(CV_LEFT, IO_OFF);
			Set_ConveyorStopper_UpDown(CV_RIGHT, IO_OFF);
			m_nInitStep = 210;
			break;

		case 210:
			nRet_1 = Chk_ConveyorStopper_UpDown(CV_LEFT, IO_OFF );
			nRet_2 = Chk_ConveyorStopper_UpDown(CV_RIGHT, IO_OFF );
			if(nRet_1 == RET_GOOD && nRet_2 == RET_GOOD)
			{
				m_nInitStep = 300;
			}
			else if(nRet_1 == RET_ERROR || nRet_2 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(5000, dWARNING, m_strAlarmCode);
				m_nInitStep = 200;
			}
			break;

		case 300: //쉬프트가 이동하면 가섭이 엘리베이터와 없는지 확인 필요 , 트레이가 껴 있을까봐 ??
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ReadyCvyLd12OutTrayPassChk,	IO_OFF);			
			if(nRet_1 == IO_OFF)
			{
				m_nInitStep = 310;
			}
			else
			{
				m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ReadyCvyLd12OutTrayPassChk); 
				CTL_Lib.Alarm_Error_Occurrence(5010, dWARNING, m_strAlarmCode);
			}
			break;

		case 310:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ReadyLd1CvyInTrayChk,	IO_OFF);
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ReadyLd2CvyInTrayChk,	IO_OFF);
			if(nRet_1 == IO_OFF && nRet_2 == IO_OFF)
			{
				m_nInitStep = 400;
			}
			else
			{
				if(nRet_1 == IO_ON)
				{
					m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_ReadyLd1CvyInTrayChk); 
				}
				else
				{
					m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_ReadyLd2CvyInTrayChk); 
				}
				CTL_Lib.Alarm_Error_Occurrence(5020, dWARNING, m_strAlarmCode);
			}
			break;

		case 400: 
			nRet_1 = COMI.HomeCheck_Mot(m_nAxisNum, MOT_SENS_ELM, MOT_TIMEOUT) ; //
			if(nRet_1 == BD_GOOD)
			{
				COMI.mn_homechk_flag[m_nAxisNum] = CTL_YES;
				m_nInitStep = 500;
			}
			else if(nRet_1 == BD_ERROR)
			{
				CTL_Lib.mn_run_status = CTL_dWARNING;
				CTL_Lib.Alarm_Error_Occurrence(5030, CTL_Lib.mn_run_status, COMI.strCode);
				m_nInitStep = 400;
			}
			break;

		case 500:
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nAxisNum, st_motor_info[m_nAxisNum].d_pos[P_LDCV_SFT_LEFT], COMI.mn_runspeed_rate);  //left로 미리 이동해 있는다 
			if (nRet_1 == BD_GOOD) //좌측으로 이동  
			{			
				m_nInitStep = 600;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nInitStep = 500;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다  
				CTL_Lib.Alarm_Error_Occurrence(5040, dWARNING, st_alarm_info.strCode);
				m_nInitStep = 500;
			}
			break; 

		case 600:
			st_lot_info[LOT_CURR].nLotStatus = LOT_CLEAR;
			st_lot_info[LOT_NEXT].nLotStatus = LOT_CLEAR;

			st_sync_info.nInit_Flag[THD_LD_CV_STACKER_LEFT] = INIT_READY;
			st_sync_info.nInit_Flag[THD_LD_CV_STACKER_RIGHT] = INIT_READY;
			 
			m_nInitStep = 0;
			break;	

		default :
			break;

	}
}


void CRunLdTrayInputShifterConveyor::OnRunMove(void) //0816
{
	int nRet_1, nRet_2=0, nRet_3=0;
	int nCount = 0;
	int nLotContinue = 0;
	CString strTemp;

	clsFunc.OnThreadFunctionStepTrace(5, m_nRunStep);

	switch (m_nRunStep)
	{ 
		case -1:
			m_nRunStep = 0;
			break;

		case 0:			
			m_nShiftMotor_Work_Site = CV_LEFT;//모터 작업 순서 정의 0(left)->1(right)->0->1 반복 
			m_nRunStep = 100;			
			break;

		case 100://쉬프터 초기 상태 확인 
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ReadyLd1CvyStopUpChk,	IO_ON);
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ReadyLd2CvyStopUpChk,	IO_ON);
			if(nRet_1 == IO_ON && nRet_2 == IO_ON)
			{
				m_nRunStep = 200;
			}
			else
			{
				m_nRunStep = 110;
			}
			break;

		case 110:
			Set_ConveyorStopper_UpDown(CV_LEFT, IO_OFF); //up 상태 
			Set_ConveyorStopper_UpDown(CV_RIGHT, IO_OFF);//up 상태 
			m_nRunStep = 111;
			break;

		case 111:
			nRet_1 = Chk_ConveyorStopper_UpDown(CV_LEFT, IO_OFF );
			nRet_2 = Chk_ConveyorStopper_UpDown(CV_RIGHT, IO_OFF );
			if(nRet_1 == RET_GOOD && nRet_2 == RET_GOOD)
			{
				m_nRunStep = 100;
			}
			else if(nRet_1 == RET_ERROR || nRet_2 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(5050, dWARNING, m_strAlarmCode);
				m_nRunStep = 110;
			}
			break;

		case 200:
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nAxisNum, st_motor_info[m_nAxisNum].d_pos[P_LDCV_SFT_LEFT], COMI.mn_runspeed_rate);  //left로 미리 이동해 있는다 
			if (nRet_1 == BD_GOOD) //좌측으로 이동  
			{			
				m_nRunStep = 1000;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nRunStep = 200;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다  
				CTL_Lib.Alarm_Error_Occurrence(5060, dWARNING, st_alarm_info.strCode);
				m_nRunStep = 200;
			}
			break; 

		case 1000:  //lot 시작을 확인한다 
			if(0) //james 2016.0924 st_lot_info[LOT_CURR].nLotStatus == LOT_START || st_lot_info[LOT_NEXT].nLotStatus == LOT_START )
			{
				if (st_handler_info.cWndMain != NULL)
				{
					st_handler_info.cWndMain->SendMessage(WM_WORK_DISPLAY, MAIN_LOT_DISPLAY, 0);
				}
				m_nRunStep = 2000;
			}
			else
			{
				m_New_lot_in = NO;
				// jtkim 20160827
				if (st_tray_info[THD_LD_CV_STACKER_LEFT].nNewLotIn == YES)
				{
					// new lot in 통신이 정상적으로 완료 되었으면 
					st_tray_info[THD_LD_CV_STACKER_LEFT].nNewLotIn = NO;
					
					//if(st_lot_info[LOT_CURR].nLotStatus < LOT_START )
					//{
					//2016.1120
					//////////////////////////////////////////////////////////////////////////
					nLotContinue = 0;
					if( st_lot_info[LOT_CURR].nLotStatus < LOT_START )
					{
						nLotContinue = 1;
						m_New_lot_in = YES;
					}
					else 
					{
						if( st_lot_info[LOT_CURR].nLotStatus >= LOT_START )
						{
							if( st_lot_info[LOT_CURR].strLotNo == st_tray_info[THD_LD_CV_STACKER_LEFT].strLotNo )
							{
								nLotContinue = -1;
							}
						}
					}

					if( nLotContinue == -1 )
					{
					}
					else if( nLotContinue == 1 )
					{
					//////////////////////////////////////////////////////////////////////////
						if (st_tray_info[THD_LD_CV_STACKER_LEFT].nQty > 0)
						{
							st_lot_info[LOT_CURR].nLotStatus	= LOT_START;
							st_lot_info[LOT_CURR].strLotNo		= st_tray_info[THD_LD_CV_STACKER_LEFT].strLotNo;
							st_lot_info[LOT_CURR].strPartNo		= st_tray_info[THD_LD_CV_STACKER_LEFT].strPartNo;

							st_lot_info[LOT_CURR].nTestMode		= st_tray_info[THD_LD_CV_STACKER_LEFT].nTestMode;
							st_lot_info[LOT_CURR].nInterface	= st_tray_info[THD_LD_CV_STACKER_LEFT].nInterface;
							st_lot_info[LOT_CURR].nQty			= st_tray_info[THD_LD_CV_STACKER_LEFT].nQty;

							// jtkim 20160929
							st_lot_info[LOT_CURR].strOpCode		= st_tray_info[THD_LD_CV_STACKER_LEFT].strOptCode;

							st_count_info.nInCount[0][0]			= 0;
							st_count_info.nPrimeCount[0][0]			= 0;
							st_count_info.nPassCount[0][0]			= 0;
							st_count_info.nRejectCount[0][0]		= 0;
							st_count_info.nPrimeRejectCount[0][0]	= 0;

							// jtkim 20161008 lot start 시간
							st_lot_info[LOT_CURR].nBdTime		= 0;
							st_lot_info[LOT_CURR].nBdTimeCount	= 0;
							st_lot_info[LOT_CURR].tLotStart		= CTime::GetCurrentTime();

							clsFunc.Find_Work_Site(LOT_CURR, st_lot_info[LOT_CURR].strLotNo, st_lot_info[LOT_CURR].strPartNo);
							strTemp.Format(_T("[LdShifterConv1] LD_CV_STACKER_LEFT LOT_CURR : %s"), st_lot_info[LOT_CURR].strLotNo);
							clsMem.OnNormalMessageWrite(strTemp);
						}
						else
						{
							clsFunc.OnMCStop();

							if (st_handler_info.cWndMain != NULL)
							{
								st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_LOT_START_CREATE_REQ, THD_LD_CV_STACKER_LEFT);
							}
						}
					}
					else
					{
						//if(st_lot_info[LOT_NEXT].nLotStatus < LOT_START)
						//{
						//2016.1120
						//////////////////////////////////////////////////////////////////////////
						nLotContinue = 0;
						if( st_lot_info[LOT_NEXT].nLotStatus < LOT_START )
						{
							nLotContinue = 1;
						}
						else 
						{
							if( st_lot_info[LOT_NEXT].nLotStatus >= LOT_START )
							{
								if( st_lot_info[LOT_NEXT].strLotNo == st_tray_info[THD_LD_CV_STACKER_LEFT].strLotNo )
								{
									nLotContinue = -1;
								}
							}
						}

						if( nLotContinue == -1 )
						{
						}
						else if( nLotContinue == 1 )
						{
						//////////////////////////////////////////////////////////////////////////
							if (st_tray_info[THD_LD_CV_STACKER_LEFT].nQty > 0)
							{
								st_lot_info[LOT_NEXT].nLotStatus	= LOT_START;
								st_lot_info[LOT_NEXT].strLotNo		= st_tray_info[THD_LD_CV_STACKER_LEFT].strLotNo;
								st_lot_info[LOT_NEXT].strPartNo		= st_tray_info[THD_LD_CV_STACKER_LEFT].strPartNo;

								st_lot_info[LOT_NEXT].nTestMode		= st_tray_info[THD_LD_CV_STACKER_LEFT].nTestMode;
								st_lot_info[LOT_NEXT].nInterface	= st_tray_info[THD_LD_CV_STACKER_LEFT].nInterface;
								st_lot_info[LOT_NEXT].nQty			= st_tray_info[THD_LD_CV_STACKER_LEFT].nQty;

								// jtkim 20160929
								//st_lot_info[LOT_CURR].strOpCode		= st_tray_info[THD_LD_CV_STACKER_LEFT].strOptCode;
								st_lot_info[LOT_NEXT].strOpCode		= st_tray_info[THD_LD_CV_STACKER_LEFT].strOptCode;

								st_count_info.nInCount[0][1]			= 0;
								st_count_info.nPrimeCount[0][1]			= 0;
								st_count_info.nPassCount[0][1]			= 0;
								st_count_info.nRejectCount[0][1]		= 0;
								st_count_info.nPrimeRejectCount[0][1]	= 0;

								clsMem.OnNormalMessageWrite(strTemp);
								clsFunc.Find_Work_Site(LOT_NEXT, st_lot_info[LOT_NEXT].strLotNo, st_lot_info[LOT_NEXT].strPartNo);
								strTemp.Format(_T("[LdShifterConv2] LD_CV_STACKER_LEFT LOT_NEXT : %s"), st_lot_info[LOT_NEXT].strLotNo);
								clsMem.OnNormalMessageWrite(strTemp);
							}
							else
							{
								clsFunc.OnMCStop();

								if (st_handler_info.cWndMain != NULL)
								{
									st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_LOT_START_CREATE_REQ, THD_LD_CV_STACKER_LEFT);
								}
							}
						}
					}
					//james 2016.0924break;
				}

				if (st_tray_info[THD_LD_CV_STACKER_RIGHT].nNewLotIn == YES)
				{
					// new lot in 통신이 정상적으로 완료 되었으면 
					st_tray_info[THD_LD_CV_STACKER_RIGHT].nNewLotIn = NO;
					
					//if(st_lot_info[LOT_CURR].nLotStatus < LOT_START)
					//2016.1120
					//////////////////////////////////////////////////////////////////////////
					nLotContinue = 0;
					if( st_lot_info[LOT_CURR].nLotStatus < LOT_START )
					{
						nLotContinue = 1;
						m_New_lot_in = YES;
					}
					else 
					{
						if( st_lot_info[LOT_CURR].nLotStatus >= LOT_START )
						{
							if( st_lot_info[LOT_CURR].strLotNo == st_tray_info[THD_LD_CV_STACKER_RIGHT].strLotNo )
							{
								nLotContinue = -1;
							}
						}
					}

					if( nLotContinue == -1 )
					{
					}
					else if( nLotContinue == 1 )
					{
					//////////////////////////////////////////////////////////////////////////
						if (st_tray_info[THD_LD_CV_STACKER_RIGHT].nQty > 0)
						{
							st_lot_info[LOT_CURR].nLotStatus	= LOT_START;
							st_lot_info[LOT_CURR].strLotNo		= st_tray_info[THD_LD_CV_STACKER_RIGHT].strLotNo;
							st_lot_info[LOT_CURR].strPartNo		= st_tray_info[THD_LD_CV_STACKER_RIGHT].strPartNo;
							st_lot_info[LOT_CURR].nQty			= st_tray_info[THD_LD_CV_STACKER_RIGHT].nQty;

							// jtkim 20160929
							st_lot_info[LOT_CURR].strOpCode		= st_tray_info[THD_LD_CV_STACKER_RIGHT].strOptCode;

							st_count_info.nInCount[0][0]		= 0;
							st_count_info.nPrimeCount[0][0]		= 0;
							st_count_info.nPassCount[0][0]		= 0;
							st_count_info.nRejectCount[0][0]	= 0;

							// jtkim 20161008 lot start 시간
// 							st_lot_info[LOT_NEXT].nBdTime		= 0;
// 							st_lot_info[LOT_NEXT].nBdTimeCount	= 0;
// 							st_lot_info[LOT_NEXT].tLotStart		= CTime::GetCurrentTime();

							st_lot_info[LOT_CURR].nBdTime		= 0;
							st_lot_info[LOT_CURR].nBdTimeCount	= 0;
							st_lot_info[LOT_CURR].tLotStart		= CTime::GetCurrentTime();

							clsFunc.Find_Work_Site(LOT_CURR, st_lot_info[LOT_CURR].strLotNo, st_lot_info[LOT_CURR].strPartNo);
							strTemp.Format(_T("[LdShifterConv3] THD_LD_CV_STACKER_RIGHT LOT_CURR : %s"), st_lot_info[LOT_CURR].strLotNo);
							clsMem.OnNormalMessageWrite(strTemp);
						}
						else
						{
							clsFunc.OnMCStop();

							if (st_handler_info.cWndMain != NULL)
							{
								st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_LOT_START_CREATE_REQ, THD_LD_CV_STACKER_RIGHT);
							}
						}
					}
					else
					{
						//if(st_lot_info[LOT_NEXT].nLotStatus < LOT_START)
						//2016.1120
						//////////////////////////////////////////////////////////////////////////
						nLotContinue = 0;
						if( st_lot_info[LOT_NEXT].nLotStatus < LOT_START )
						{
							nLotContinue = 1;
						}
						else 
						{
							if( st_lot_info[LOT_NEXT].nLotStatus >= LOT_START )
							{
								if( st_lot_info[LOT_NEXT].strLotNo == st_tray_info[THD_LD_CV_STACKER_RIGHT].strLotNo )
								{
									nLotContinue = -1;
								}
							}
						}

						if( nLotContinue == -1 )
						{
						}
						else if( nLotContinue == 1 )
						{
							//////////////////////////////////////////////////////////////////////////
							if (st_tray_info[THD_LD_CV_STACKER_RIGHT].nQty > 0)
							{
								st_lot_info[LOT_NEXT].nLotStatus	= LOT_START;
								st_lot_info[LOT_NEXT].strLotNo		= st_tray_info[THD_LD_CV_STACKER_RIGHT].strLotNo;
								st_lot_info[LOT_NEXT].strPartNo		= st_tray_info[THD_LD_CV_STACKER_RIGHT].strPartNo;
								st_lot_info[LOT_NEXT].nQty			= st_tray_info[THD_LD_CV_STACKER_RIGHT].nQty;

								// jtkim 20160929
								st_lot_info[LOT_NEXT].strOpCode		= st_tray_info[THD_LD_CV_STACKER_RIGHT].strOptCode;

								st_count_info.nInCount[0][1]		= 0;
								st_count_info.nPrimeCount[0][1]		= 0;
								st_count_info.nPassCount[0][1]		= 0;
								st_count_info.nRejectCount[0][1]	= 0;

								clsFunc.Find_Work_Site(LOT_NEXT, st_lot_info[LOT_NEXT].strLotNo, st_lot_info[LOT_NEXT].strPartNo);
								strTemp.Format(_T("[LdShifterConv4] THD_LD_CV_STACKER_RIGHT LOT_NEXT : %s"), st_lot_info[LOT_NEXT].strLotNo);
								clsMem.OnNormalMessageWrite(strTemp);
							}
							else
							{
								clsFunc.OnMCStop();

								if (st_handler_info.cWndMain != NULL)
								{
									st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_LOT_START_CREATE_REQ, THD_LD_CV_STACKER_RIGHT);
								}
							}							
						}
					}
					//james 2016.0924break;
				}

				//james 2016.0924 추가 
				if(st_lot_info[LOT_CURR].nLotStatus == LOT_START || st_lot_info[LOT_NEXT].nLotStatus == LOT_START )
				{
					if (st_handler_info.cWndMain != NULL)
					{
						st_handler_info.cWndMain->SendMessage(WM_WORK_DISPLAY, MAIN_LOT_DISPLAY, 0);
					}

					if( 1 || nLotContinue == true )
					{
						//2016.1109
						if(m_New_lot_in == YES)
						{
							m_nRunStep = 1900;
						}
						else
						{
							m_nRunStep = 2000;
						}

						////////////////////////////////////////////////////////////////
						//2016.1203
						st_work_info.dwFrontSmema[0] = GetTickCount();
						
					}
				}
			}
			break;

		case 1900:
			if (st_handler_info.nFtpVer == 0)
			{
				m_nRunStep = 2000;
			}
			else if (st_handler_info.nFtpVer == 1)
			{
				clsFunc.OnScrapCodeFind(m_nFtpSite, _T("C:\\AMT840\\File\\RETEST_RULE.TXT"));

				m_nRunStep = 2000;
			}
			else
			{
				if (st_lot_info[LOT_CURR].nScrapCode == YES)
				{
					m_nFtpRetry = 0;
					m_nFtpSite	= LOT_CURR;

					m_nRunStep = 1910;
				}
				else if (st_lot_info[LOT_NEXT].nScrapCode == YES)
				{
					m_nFtpRetry = 0;
					m_nFtpSite	= LOT_NEXT;

					m_nRunStep = 1910;
				}
				else
				{
					// jtkim 20150730
					clsFunc.OnScrapCodeFind(m_nFtpSite, _T("C:\\AMT840\\File\\RETEST_RULE.TXT"));

					m_nRunStep = 2000;
				}
			}
			break;

		case 1910:
			m_dwFtpWaitTime[0] = GetTickCount();

			st_code_info[m_nFtpSite].m_nScrapBin = BD_NONE;
			::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + FTP_NETWORK, 0, m_nFtpSite);

			m_nRunStep = 1920;
			break;

		case 1920:
			if (st_code_info[m_nFtpSite].m_nScrapBin == BD_DATA_GOOD)
			{
				st_lot_info[m_nFtpSite].nScrapCode = NO;

				m_nRunStep = 2000;
			}
			else if (st_code_info[m_nFtpSite].m_nScrapBin == BD_DATA_REJECT)
			{
				m_nFtpRetry++;
				if (m_nFtpRetry >= 3)
				{
					m_nFtpRetry = 0;
					clsFunc.OnScrapCodeFind(m_nFtpSite, _T("C:\\AMT840\\File\\RETEST_RULE.TXT"));

					m_nRunStep = 2000;
				}
				else
				{
					m_nRunStep = 1910;
				}
			}
			else
			{
				m_dwFtpWaitTime[1] = GetTickCount();
				m_dwFtpWaitTime[2] = m_dwFtpWaitTime[1] - m_dwFtpWaitTime[0];

				if (m_dwFtpWaitTime[2] <= (DWORD)0)
				{
					m_dwFtpWaitTime[0] = GetTickCount();
					break;
				}

				if (m_dwFtpWaitTime[2] > (DWORD)10000)
				{
					m_nFtpRetry++;
					if (m_nFtpRetry >= 3)
					{
						m_nFtpRetry = 0;
						m_nRunStep = 1930;
					}
					else
					{
						m_nRunStep = 1910;
					}
				}
			}
			break;

		case 1930:
			m_dwFtpWaitTime[0] = GetTickCount();

			st_other_info.nConfirmMsg	= -1;
			st_other_info.strConfirmMsg = _T("RETEST_RULE.TXT Download Fail\r\n Retry Yes/No...?");

			if (st_handler_info.cWndMain != NULL)
			{
				st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_VIEW_CREATE_REQ, 0);
			}

			clsFunc.OnMCStop();

			m_nRunStep = 1940;
			break;

		case 1940:
			if (st_other_info.nConfirmMsg == 1)
			{
				m_nRunStep = 1910;
			}
			else if (st_other_info.nConfirmMsg == 0)
			{
				clsFunc.OnScrapCodeFind(m_nFtpSite, _T("C:\\AMT840\\File\\RETEST_RULE.TXT"));

				m_nRunStep = 2000;
			}
			else
			{
				m_dwFtpWaitTime[1] = GetTickCount();
				m_dwFtpWaitTime[2] = m_dwFtpWaitTime[1] - m_dwFtpWaitTime[0];

				if (m_dwFtpWaitTime[2] <= (DWORD)0)
				{
					m_dwFtpWaitTime[0] = GetTickCount();
					break;
				}

				if (m_dwFtpWaitTime[2] > (DWORD)60000)
				{
					m_nRunStep = 1930;
				}
			}
			break;

		////////////////////////////////////////////////////////////////////////
		// 트레이가 적재되면 작업 가능한 트레이를 스태커에 공급한다
		////////////////////////////////////////////////////////////////////////
		case 2000: //loading tray가 쉬프트에 하나이상 적재 되었는지 체크 
			//
			//Process_LoadTray_Insert(*) 함수에서 자제 CV 넣은후 lot 정보 트레이 정보등 다 처리후 아래 st_sync_info.nLdCV_LR_TrayInStack_Flag[*] flag를 CTL_YES로 셋 해준다 
			if(st_lot_info[LOT_CURR].nLotStatus < LOT_START && st_lot_info[LOT_NEXT].nLotStatus < LOT_START )
			{//2016.0923 
				m_nRunStep = 1000;
				break;
			}

			if(st_sync_info.nLdCV_LR_TrayInStack_Flag[THD_LD_CV_STACKER_LEFT] == CTL_NO )
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ReadyLd1TrayExistChk,	IO_ON);
				if(nRet_1== IO_ON)
				{
					clsFunc.OnMCStop();

					if (st_handler_info.cWndMain != NULL)
					{
						if(nRet_1 == IO_ON)
						{
							st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_LOT_START_CREATE_REQ, THD_LD_CV_STACKER_LEFT);
						}				
					}
					break;
				}				
			}
			
			if(	st_sync_info.nLdCV_LR_TrayInStack_Flag[THD_LD_CV_STACKER_RIGHT] == CTL_NO)
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ReadyLd2TrayExistChk,	IO_ON);
				if(nRet_1== IO_ON )
				{
					clsFunc.OnMCStop();

					if (st_handler_info.cWndMain != NULL)
					{
						if(nRet_1 == IO_ON)
						{
							st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_LOT_START_CREATE_REQ, THD_LD_CV_STACKER_RIGHT);
						}					
					}
					break;
				}
			}

			nCount = 0;
			if(st_sync_info.nLdCV_LR_TrayInStack_Flag[THD_LD_CV_STACKER_LEFT] == CTL_YES)
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ReadyLd1TrayExistChk,	IO_ON);
				if(nRet_1 != IO_ON) //error, treay를 재 확인 
				{
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ReadyLd1TrayExistChk); 
					CTL_Lib.Alarm_Error_Occurrence(5100, dWARNING, m_strAlarmCode);
					break;
				}

				nCount += 1; //Left Shift에  트레이가 있다 
			}
			if(st_sync_info.nLdCV_LR_TrayInStack_Flag[THD_LD_CV_STACKER_RIGHT] == CTL_YES)
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ReadyLd2TrayExistChk,	IO_ON);
				if(nRet_1 != IO_ON) //error, treay를 재 확인 
				{
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ReadyLd2TrayExistChk); 
					CTL_Lib.Alarm_Error_Occurrence(5110, dWARNING, m_strAlarmCode);
					break;
				}

				nCount += 2; //right Shift에  트레이가 있다 
			}			 

			if(nCount > 0) //left 또는 right 쉬프트에 어느쪽이든 공급되면 작업 진행 
			{
				if(nCount == 1) //left 만 있는 상태 //left shift 작업 진행
				{
					if(st_lot_info[LOT_CURR].nLotStatus == LOT_START) //현재 lot이 lot end를 시작했으면 더이상 CV 에는 현재 lot이 있으면 안됨 
					{
						if(st_lot_info[LOT_CURR].strLotNo == st_tray_info[THD_LD_CV_STACKER_LEFT].strLotNo) //lot정보를 비교한다 
						{
							m_nShiftMotor_Work_Site = CV_LEFT;
							m_nRunStep = 3000; //current lot의 left shifter의 자재를 공급
							break; //james 2016.0924 
						}	 
					}
					 //james 2016.0924 else if(st_lot_info[LOT_NEXT].nLotStatus == LOT_START) //next lot이 공급된 상태 
					if(st_lot_info[LOT_NEXT].nLotStatus == LOT_START) //next lot이 공급된 상태 
					{
						if(st_lot_info[LOT_NEXT].strLotNo == st_tray_info[THD_LD_CV_STACKER_LEFT].strLotNo)//lot정보를 비교한다 
						{
							m_nShiftMotor_Work_Site = CV_LEFT;
							m_nRunStep = 3000; //next lot의 left shifter의 자재를 공급
							break; //james 2016.0924 
						} 
					}

					m_nRunStep = 1000;//james 2016.0924 추가, lot 정보가 맞지앟으면 처음으로 이동  
				}
				else if(nCount == 2) //right 에만 있는 상태 //right shift 작업 진행
				{
					if(st_lot_info[LOT_CURR].nLotStatus == LOT_START) //현재 lot이 lot end를 시작했으면 더이상 CV 에는 현재 lot이 있으면 안됨 
					{
						 
						if(st_lot_info[LOT_CURR].strLotNo == st_tray_info[THD_LD_CV_STACKER_RIGHT].strLotNo)//lot정보를 비교한다 
						{
							m_nShiftMotor_Work_Site = CV_RIGHT;
							m_nRunStep = 3000; //current lot의 right shifter의 자재를 공급
							break; //james 2016.0924 
						}	 
					}
					//james 2016.0924 else if(st_lot_info[LOT_NEXT].nLotStatus == LOT_START) //next lot이 공급된 상태 
					if(st_lot_info[LOT_NEXT].nLotStatus == LOT_START) //next lot이 공급된 상태 
					{
						if(st_lot_info[LOT_NEXT].strLotNo == st_tray_info[THD_LD_CV_STACKER_RIGHT].strLotNo)//lot정보를 비교한다 
						{
							m_nShiftMotor_Work_Site = CV_RIGHT;
							m_nRunStep = 3000; //next lot의 right shifter의 자재를 공급
							break; //james 2016.0924 
						} 
					}
					m_nRunStep = 1000;//james 2016.0924 추가, lot 정보가 맞지앟으면 처음으로 이동  
				}
				else if(nCount == 3) //left & right 에 모두 있는 상태 
				{
					if(m_nShiftMotor_Work_Site == CV_LEFT)//left shift 작업 진행
					{
						if(st_lot_info[LOT_CURR].nLotStatus == LOT_START) //현재 lot이 lot end를 시작했으면 더이상 CV 에는 현재 lot이 있으면 안됨 
						{
							if(st_lot_info[LOT_CURR].strLotNo == st_tray_info[THD_LD_CV_STACKER_LEFT].strLotNo)//lot정보를 비교한다 
							{
								m_nShiftMotor_Work_Site = CV_LEFT;
								m_nRunStep = 3000; //current lot의 left shifter의 자재를 공급
								break; //james 2016.0924 
							}	 
							else if(st_lot_info[LOT_CURR].strLotNo == st_tray_info[THD_LD_CV_STACKER_RIGHT].strLotNo)//lot정보를 비교한다 
							{
								m_nShiftMotor_Work_Site = CV_RIGHT;
								m_nRunStep = 3000; //current lot의 left shifter의 자재를 공급
								break; //james 2016.0924 
							}	 
						}
						 //james 2016.0924 else if(st_lot_info[LOT_NEXT].nLotStatus == LOT_START) //next lot이 공급된 상태 
						if(st_lot_info[LOT_NEXT].nLotStatus == LOT_START) //next lot이 공급된 상태 
						{
							if(st_lot_info[LOT_NEXT].strLotNo == st_tray_info[THD_LD_CV_STACKER_LEFT].strLotNo)//lot정보를 비교한다 
							{
								m_nShiftMotor_Work_Site = CV_LEFT;
								m_nRunStep = 3000; //next lot의 left shifter의 자재를 공급
								break; //james 2016.0924 
							} 
							else if(st_lot_info[LOT_NEXT].strLotNo == st_tray_info[THD_LD_CV_STACKER_RIGHT].strLotNo)//lot정보를 비교한다 
							{
								m_nShiftMotor_Work_Site = CV_RIGHT;
								m_nRunStep = 3000; //current lot의 left shifter의 자재를 공급
								break; //james 2016.0924 
							}	
						}
					}
					else if(m_nShiftMotor_Work_Site == CV_RIGHT) //right shift 작업 진행 
					{
						if(st_lot_info[LOT_CURR].nLotStatus == LOT_START) //현재 lot이 lot end를 시작했으면 더이상 CV 에는 현재 lot이 있으면 안됨 
						{
							if(st_lot_info[LOT_CURR].strLotNo == st_tray_info[THD_LD_CV_STACKER_RIGHT].strLotNo)//lot정보를 비교한다 
							{
								m_nShiftMotor_Work_Site = CV_RIGHT;
								m_nRunStep = 3000; //current lot의 left shifter의 자재를 공급
								break; //james 2016.0924 
							}
							else if(st_lot_info[LOT_CURR].strLotNo == st_tray_info[THD_LD_CV_STACKER_LEFT].strLotNo)//lot정보를 비교한다 
							{
								m_nShiftMotor_Work_Site = CV_LEFT;
								m_nRunStep = 3000; //current lot의 left shifter의 자재를 공급
								break; //james 2016.0924 
							}	 
						}
						//james 2016.0924 else if(st_lot_info[LOT_NEXT].nLotStatus == LOT_START) //next lot이 공급된 상태 
						if(st_lot_info[LOT_NEXT].nLotStatus == LOT_START) //next lot이 공급된 상태 
						{
							if(st_lot_info[LOT_NEXT].strLotNo == st_tray_info[THD_LD_CV_STACKER_RIGHT].strLotNo)//lot정보를 비교한다 
							{
								m_nShiftMotor_Work_Site = CV_RIGHT;
								m_nRunStep = 3000; //current lot의 left shifter의 자재를 공급
								break; //james 2016.0924 
							}	
							else if(st_lot_info[LOT_NEXT].strLotNo == st_tray_info[THD_LD_CV_STACKER_LEFT].strLotNo)//lot정보를 비교한다 
							{
								m_nShiftMotor_Work_Site = CV_LEFT;
								m_nRunStep = 3000; //next lot의 left shifter의 자재를 공급
								break; //james 2016.0924 
							}  
						}
					}

					m_nRunStep = 1000;//james 2016.0924 추가, lot 정보가 맞지앟으면 처음으로 이동  
				}
			}			
			break; 

		///////////////////////////////////////////////////////////////////////////////
		// Load Conveyor Shifter를 load elevator 영역에 CV shifter를 이동 시켜 맞춘다 
		///////////////////////////////////////////////////////////////////////////////
		case 3000:
			//if(st_lot_info[LOT_NEXT].nLot_THD_Status[THD_LD_STACKER] >=  LOT_END)
			//{
			//	m_nRunStep = 1000;
			//}

			if(st_sync_info.nLdElv_Tray_Req[THD_LD_STACKER] == CTL_REQ) //lot이 시작되면 CTL_REQ 플레그를 셋 한다 
			{
				//james 2016.0924 조건 추가 m_nRunStep = 3100;
				if(st_lot_info[LOT_CURR].nLotStatus == LOT_START)
				{
					
					if(st_lot_info[LOT_CURR].strLotNo == st_tray_info[m_nShiftMotor_Work_Site].strLotNo)
					{
						m_nRunStep = 3100;
						break;
					}
					else//2016.1228
					{
						if (st_handler_info.cWndMain != NULL)
						{
							st_other_info.nBuzYesNo = YES;
							st_other_info.strBoxMsg.Format(_T("[LOAD TRAY STACKER ] Please Insert tray.")); 
							st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
						}
						m_nRunStep = 3010;
					}
				}
				else if(st_lot_info[LOT_NEXT].nLotStatus == LOT_START)
				{
					//st_tray_info[m_nShiftMotor_Work_Site].strLotNo = "N1";
					if(st_lot_info[LOT_NEXT].strLotNo == st_tray_info[m_nShiftMotor_Work_Site].strLotNo)
					{
						/* james 2016.0926, load plate 로 조건 이동 
						nCount = 0;
						for(i = THD_TESTSITE_1; i <= THD_TESTSITE_4; i++) //james 206.0924 
						{
							
							if(st_lot_info[LOT_NEXT].strLotNo ==  st_test_site_info[i].strLotNo)
							{
								nCount++;
							}
						}
						if(nCount == 0)
						{
							break;
						}*/

						m_nRunStep = 3100;
						break;
					}
					else//2016.1228
					{
						if (st_handler_info.cWndMain != NULL)
						{
							st_other_info.nBuzYesNo = YES;
							st_other_info.strBoxMsg.Format(_T("[LOAD TRAY STACKER ] Please Insert tray.")); 
							st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
						}
						m_nRunStep = 3010;
					}
				}

			}
			break;

		//2016.1228
		case 3010:
			st_handler_info.mn_NoInput_test = CTL_YES;
			m_dwNoInputWaitTime[0] = GetCurrentTime();
			m_nRunStep = 3020;
			break;

		case 3020:
			m_dwNoInputWaitTime[1] = GetCurrentTime();
			m_dwNoInputWaitTime[2] = m_dwNoInputWaitTime[1] - m_dwNoInputWaitTime[0];

			if(st_sync_info.nLdElv_Tray_Req[THD_LD_STACKER] == CTL_REQ) //lot이 시작되면 CTL_REQ 플레그를 셋 한다 
			{
				if(st_lot_info[LOT_CURR].nLotStatus == LOT_START)
				{					
					if(st_lot_info[LOT_CURR].strLotNo == st_tray_info[m_nShiftMotor_Work_Site].strLotNo)
					{						
						m_nRunStep = 3100;
						st_handler_info.mn_NoInput_test = CTL_NO; //2016.1228
						if (st_handler_info.cWndMain != NULL)
						{
							st_other_info.nBuzYesNo = NO;
							st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_DELETE_REQ, 0);
						}
						break;
					}
				}
				else if(st_lot_info[LOT_NEXT].nLotStatus == LOT_START)
				{
					if(st_lot_info[LOT_NEXT].strLotNo == st_tray_info[m_nShiftMotor_Work_Site].strLotNo)
					{
						m_nRunStep = 3100;
						st_handler_info.mn_NoInput_test = CTL_NO;
						if (st_handler_info.cWndMain != NULL)
						{
							st_other_info.nBuzYesNo = NO;
							st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_DELETE_REQ, 0);
						}
						break;
					}
				}
			}

			if( m_dwNoInputWaitTime[2] <= 0 ) m_dwNoInputWaitTime[0] = GetCurrentTime();
			if( m_dwNoInputWaitTime[2] > 5*1000*60)//5분 마다 알람
			{
					if (st_handler_info.cWndMain != NULL)
					{
						st_other_info.nBuzYesNo = YES;
						st_other_info.strBoxMsg.Format(_T("[LOAD TRAY STACKER ] Please Insert tray.")); 
						st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
					}
					m_nRunStep = 3020;
			}
			break;

		case 3100:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ReadyCvyLd12OutTrayPassChk,	IO_OFF); //CV tray pass check  없어야 한다 
			if(nRet_1 == IO_OFF)
			{
				//정상, 쉬프터가 동작 가능 
			}
			else
			{//트레이 존재 에러 
				if(nRet_1 == IO_ON) m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_ReadyCvyLd12OutTrayPassChk); 
				CTL_Lib.Alarm_Error_Occurrence(5120, dWARNING, m_strAlarmCode);
				break;
			}

			if(m_nShiftMotor_Work_Site == CV_LEFT) 	m_dpTargetPos =  st_motor_info[m_nAxisNum].d_pos[P_LDCV_SFT_LEFT];
			else									m_dpTargetPos =  st_motor_info[m_nAxisNum].d_pos[P_LDCV_SFT_RIGHT];	 

			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nAxisNum, m_dpTargetPos, COMI.mn_runspeed_rate);  //left로 미리 이동해 있는다 
			if (nRet_1 == BD_GOOD) //m_nShiftMotor_Work_Site 이동  
			{			
				m_nRunStep = 3200;

				//2017.0109
				if( m_nShiftMotor_Work_Site  == CV_LEFT )
				{
					st_tray_info[m_nShiftMotor_Work_Site].strLogKey[0]	= _T("LD_CV_STACKER_LEFT");
				}
				else
				{
					st_tray_info[m_nShiftMotor_Work_Site].strLogKey[0]	= _T("LD_CV_STACKER_RIGHT");
				}
				st_tray_info[m_nShiftMotor_Work_Site].strMovePos[0] = clsLog.GetDeviceID( m_nShiftMotor_Work_Site );
				st_tray_info[m_nShiftMotor_Work_Site].strMovePos[1] = clsLog.GetDeviceID( THD_LD_STACKER );

				if(st_lot_info[LOT_CURR].strLotNo != _T("") && st_lot_info[LOT_CURR].nLotStatus >= LOT_START && st_lot_info[LOT_CURR].nLotStatus < LOT_END_START)
				{
					st_tray_info[m_nShiftMotor_Work_Site].strLogData[0]	= st_lot_info[LOT_CURR].strLotNo;
				}
				else if(st_lot_info[LOT_NEXT].strLotNo != _T("") && st_lot_info[LOT_NEXT].nLotStatus >= LOT_START && st_lot_info[LOT_NEXT].nLotStatus < LOT_END_START)
				{
					st_tray_info[m_nShiftMotor_Work_Site].strLogData[0]	= st_lot_info[LOT_NEXT].strLotNo;
				}
				else
				{
					st_tray_info[m_nShiftMotor_Work_Site].strLogData[0]	= _T("$");
				}

				st_tray_info[m_nShiftMotor_Work_Site].strMaterial = _T("$");
				st_tray_info[m_nShiftMotor_Work_Site].nStartStatus = 1;

				clsLog.LogTransfer(_T("LD_CV_STACKER"),
									_T("MOVE_CDIMM_TRAY"),
									0,
									st_tray_info[THD_WORK_TRANSFER].strMaterial,
									_T("CDIMM_TRAY"),
									st_tray_info[THD_WORK_TRANSFER].strMovePos[0], 
									st_tray_info[THD_WORK_TRANSFER].strMovePos[1], 
									1, 
									st_tray_info[THD_WORK_TRANSFER].strLogKey,
									st_tray_info[THD_WORK_TRANSFER].strLogData);


			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nRunStep = 3100;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다  
				CTL_Lib.Alarm_Error_Occurrence(5130, dWARNING, st_alarm_info.strCode);
				m_nRunStep = 3100;
			}
			break; 

		case 3200://트레이 상태를 다시한번 체크 
			if(st_sync_info.nLdCV_LR_TrayInStack_Flag[THD_LD_CV_STACKER_LEFT] == CTL_YES)
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ReadyLd1TrayExistChk,	IO_ON);
				if(nRet_1 != IO_ON) //error, treay를 재 확인 
				{
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ReadyLd1TrayExistChk); 
					CTL_Lib.Alarm_Error_Occurrence(5140, dWARNING, m_strAlarmCode);
					break;
				} 
			}
			if(st_sync_info.nLdCV_LR_TrayInStack_Flag[THD_LD_CV_STACKER_RIGHT] == CTL_YES)
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ReadyLd2TrayExistChk,	IO_ON);
				if(nRet_1 != IO_ON) //error, treay를 재 확인 
				{
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ReadyLd2TrayExistChk); 
					CTL_Lib.Alarm_Error_Occurrence(5150, dWARNING, m_strAlarmCode);
					break;
				}
			}
			m_nRunStep = 4000;
			break;
			

		////////////////////////////////////////////////////////////////////////
		// Load Conveyor의 트레이를 elevator 사이트 작업 할 영역으로 이동시킨다 
		// //load elevator가 준비되었으면 CV에서 -> elevator로 트레이를 공급한다 
		////////////////////////////////////////////////////////////////////////
		case 4000: //각 사이트의 2 모터가 트레이를 이송 받을 위치에 존재하는지 재 점검한다 
			///////////////////////////////////////////////
			//현재 공급하는 자재와 lot 정보를 비교한다 
			// james 2016.0912 
			///////////////////////////////////////////////
			if(st_lot_info[LOT_CURR].nLotStatus == LOT_START)
			{
				if(st_lot_info[LOT_CURR].strLotNo == st_tray_info[THD_LD_CV_STACKER_RIGHT].strLotNo)//lot정보를 비교한다 
				{
					//lot 이름 정보 정상  
				}	
				else if(st_lot_info[LOT_CURR].strLotNo == st_tray_info[THD_LD_CV_STACKER_LEFT].strLotNo)//lot정보를 비교한다 
				{
					 //lot 이름 정보 정상  
				}  
				else
				{//해당 하는 lot 정보가 없다 
					break;
				}
			}
			else if(st_lot_info[LOT_NEXT].nLotStatus == LOT_START) //next lot이 공급된 상태 
			{
				if(st_lot_info[LOT_NEXT].strLotNo == st_tray_info[THD_LD_CV_STACKER_RIGHT].strLotNo)//lot정보를 비교한다 
				{
					//lot 이름 정보 정상  
				}	
				else if(st_lot_info[LOT_NEXT].strLotNo == st_tray_info[THD_LD_CV_STACKER_LEFT].strLotNo)//lot정보를 비교한다 
				{
					 //lot 이름 정보 정상  
				}  
				else
				{//해당 하는 lot 정보가 없다 
					break;
				}
			}
			else
			{//LOT_START 가 진행중인 것이 없으니, 트레이를 더 공급하면 안된다 
				break;
			}
			//현재 공급하는 자재와 lot 정보를 비교한다 
			nRet_1 = COMI.Check_MotPosRange(m_nAxisNum, m_dpTargetPos, st_motor_info[m_nAxisNum].d_allow);//2014.1120 0);
			nRet_2 = COMI.Check_MotPosRange(M_LD_ELV, st_motor_info[M_LD_ELV].d_pos[P_ELV_TRAY_INOUT_POS], st_motor_info[M_LD_ELV].d_allow); 
			nRet_3 = FAS_IO.get_in_bit(st_io_info.i_LdStkDnChk,	IO_ON); //stacker up/dn cylinder도 down 되어 있어야 한다 
			if (nRet_1 == BD_GOOD && nRet_2 == BD_GOOD && nRet_3 == IO_ON)  //모두 정상인 위치 
			{
				m_nRunStep = 4100;
			}
			else
			{
				if(nRet_3 == IO_OFF)
				{
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_LdStkDnChk); 
					CTL_Lib.Alarm_Error_Occurrence(5160, dWARNING, m_strAlarmCode);
				}
				m_nRunStep = 3000;
			}
			break;

		case 4100: //stopper down 하여 트레이를 보낼 준비한다 

			if(m_nShiftMotor_Work_Site == CV_LEFT)
			{
				Set_ConveyorStopper_UpDown(CV_LEFT, IO_ON); //실린더 다운 
			}
			else //if(m_nShiftMotor_Work_Site == CV_RIGHT)
			{
				Set_ConveyorStopper_UpDown(CV_RIGHT, IO_ON); //실린더 다운 
			}			 
			m_nRunStep = 4110;
			break;

		case 4110:
			if(m_nShiftMotor_Work_Site == CV_LEFT)
			{
				nRet_1 = Chk_ConveyorStopper_UpDown(CV_LEFT, IO_ON );
			}
			else //if(m_nShiftMotor_Work_Site == CV_RIGHT)
			{
				nRet_1 = Chk_ConveyorStopper_UpDown(CV_RIGHT, IO_ON );
			}	
			
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 4200;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(5170, dWARNING, m_strAlarmCode);
				m_nRunStep = 4100;
			}
			break;

		case 4200: //ac motor CV start 한다 
			if(m_nShiftMotor_Work_Site == CV_LEFT)
			{
				//load cv & stacker ac motor 를 동시에 같이 구동시켜 동시에 이동하게 한다 
				st_sync_info.nConveyor_AcMotorCtl_Req[THD_LD_CV_STACKER_LEFT] = CTL_REQ; //cv rail ac motor 구동 
				st_sync_info.nConveyor_AcMotorCtl_Req[THD_LD_STACKER] = CTL_REQ;  //load stacker ac motor 구동 

				m_bCVTrayStackerArrivedFlag = false;
			}
			else //if(m_nShiftMotor_Work_Site == CV_RIGHT)
			{
				//load cv & stacker ac motor 를 동시에 같이 구동시켜 동시에 이동하게 한다 
				st_sync_info.nConveyor_AcMotorCtl_Req[THD_LD_CV_STACKER_RIGHT] = CTL_REQ;
				st_sync_info.nConveyor_AcMotorCtl_Req[THD_LD_STACKER] = CTL_REQ;  //load stacker ac motor 구동 
				m_bCVTrayStackerArrivedFlag = false;
			}	
			m_nRunStep = 4300;
			break;

		case 4300: //트레이가 모두 정상적으로도착한 상태 

			if(m_nShiftMotor_Work_Site == CV_LEFT)
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ReadyLd1TrayExistChk,	IO_OFF);				 
			}
			else if(m_nShiftMotor_Work_Site == CV_RIGHT)
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ReadyLd2TrayExistChk,	IO_OFF);
			}

			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ReadyCvyLd12OutTrayPassChk,	IO_OFF); 
			nRet_3 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayArrivedChk,	IO_ON); 

			if(nRet_1 == IO_OFF && nRet_2 == IO_OFF && nRet_3 == IO_ON)
			{
				if(m_nShiftMotor_Work_Site == CV_LEFT)
				{
					//load cv & stacker ac motor 를 동시에 같이 구동시켜 동시에 정지하게 한다 
					st_sync_info.nConveyor_AcMotorCtl_Req[THD_LD_CV_STACKER_LEFT] = CTL_READY;
					st_sync_info.nConveyor_AcMotorCtl_Req[THD_LD_STACKER] = CTL_READY;
				}
				else if(m_nShiftMotor_Work_Site == CV_RIGHT)
				{
					//load cv & stacker ac motor 를 동시에 같이 구동시켜 동시에 정지하게 한다 
					st_sync_info.nConveyor_AcMotorCtl_Req[THD_LD_CV_STACKER_RIGHT] = CTL_READY;
					st_sync_info.nConveyor_AcMotorCtl_Req[THD_LD_STACKER] = CTL_READY;
				}

				m_bCVTrayStackerArrivedFlag = false;
				m_nRunStep = 4400;
			}
			else
			{//트레이가 정상적으로 공급되지 않은 상태로 리미트 시간 체크  
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
						if(nRet_1 == IO_ON)
						{
							if(m_nShiftMotor_Work_Site == CV_LEFT)
							{
								m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ReadyLd1TrayExistChk); 
							}
							else
							{
								m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ReadyLd2TrayExistChk); 
							}
							CTL_Lib.Alarm_Error_Occurrence(5180, dWARNING, m_strAlarmCode);
						}
						else if(nRet_2 == IO_ON)
						{
							m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ReadyCvyLd12OutTrayPassChk); 
							CTL_Lib.Alarm_Error_Occurrence(5190, dWARNING, m_strAlarmCode);
						}
						else //if(nRet_3 == IO_OFF)
						{
							m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_LdStkTrayArrivedChk); 
							CTL_Lib.Alarm_Error_Occurrence(5200, dWARNING, m_strAlarmCode);
						}

						//에러가 발생했으니 AC Motor를 즉시 정지 시킨다 
						if(m_nShiftMotor_Work_Site == CV_LEFT)
						{
							//load cv & stacker ac motor 를 동시에 같이 구동시켜 동시에 정지하게 한다 
							st_sync_info.nConveyor_AcMotorCtl_Req[THD_LD_CV_STACKER_LEFT] = CTL_FREE;
							st_sync_info.nConveyor_AcMotorCtl_Req[THD_LD_STACKER] = CTL_FREE;
						}
						else if(m_nShiftMotor_Work_Site == CV_RIGHT)
						{
							//load cv & stacker ac motor 를 동시에 같이 구동시켜 동시에 정지하게 한다 
							st_sync_info.nConveyor_AcMotorCtl_Req[THD_LD_CV_STACKER_RIGHT] = CTL_FREE;
							st_sync_info.nConveyor_AcMotorCtl_Req[THD_LD_STACKER] = CTL_FREE;
						}

						m_bCVTrayStackerArrivedFlag = false; //처음으로 다시가 재 시도 
						m_nRunStep = 4000;
					}
				}
			}
			break;

		case 4400: //동작을 빠르게 하기 위해 실린더만 동작를 먼저 시키고 센서 체크는 나중에 한다 
			if(m_nShiftMotor_Work_Site == CV_LEFT)
			{
				Set_ConveyorStopper_UpDown(CV_LEFT, IO_OFF); //up
			}
			else// if(m_nShiftMotor_Work_Site == CV_RIGHT)
			{
				Set_ConveyorStopper_UpDown(CV_RIGHT, IO_OFF);//up
			}			 
			m_nRunStep = 5000;
			break;

		///////////////////////////////////////////////////////////////////////////////////////////////
		// Load CV Shifter에서 Load stacker 로 트레이가 이송되었으니 트레이 정보를 전부 쉬프트 한다 
		///////////////////////////////////////////////////////////////////////////////////////////////
		case 5000:
			if(m_nShiftMotor_Work_Site == CV_LEFT)
			{
				////////////////////////////////////////////////////////
				//left cv tray 적재 데이터를 load stacker에 쉬프트 위치
				clsFunc.Handler_Tray_DataInfo_Shift(1, CTL_YES, THD_LD_CV_STACKER_LEFT, THD_LD_STACKER); //트레이 정보를 Load Left CV  -> Load Stacker로 쉬프트 한다 // Handler_Tray_DataInfo_Shift(int nMode, int nDvc_Yes_No, int nSend_SIte, int nRcv_Site)

				////////////////////////////////////////////////////////	

				st_sync_info.nLdCV_LR_TrayInStack_Flag[THD_LD_CV_STACKER_LEFT] = CTL_NO; //트레이를 이송하여 없으니 적재 정보 클리어 
				st_tray_info[THD_LD_CV_STACKER_LEFT].nTrayExist = CTL_NO; //트레이존재 정보 클리어 
			}
			else if(m_nShiftMotor_Work_Site == CV_RIGHT)
			{
				////////////////////////////////////////////////////////
				//right cv tray 적재 데이터를 load stacker에 쉬프트 위치 
				clsFunc.Handler_Tray_DataInfo_Shift(1, CTL_YES, THD_LD_CV_STACKER_RIGHT, THD_LD_STACKER); //트레이 정보를 Load Right CV  -> Load Stacker로 쉬프트 한다	

				////////////////////////////////////////////////////////	

				st_sync_info.nLdCV_LR_TrayInStack_Flag[THD_LD_CV_STACKER_RIGHT] = CTL_NO; //트레이를 이송하여 없으니 적재 정보 클리어 
				st_tray_info[THD_LD_CV_STACKER_RIGHT].nTrayExist = CTL_NO; //트레이존재 정보 클리어 
			}
			m_nRunStep = 5100;
			break;

		case 5100:
			
			
			st_sync_info.nLdElv_Tray_Req[THD_LD_STACKER] = CTL_READY; //로딩 스태커에 자재를 모두 넣었다 
			m_nRunStep = 6000;
			break;
			
		case 6000: //stopper up, 트레이르 모두 보냈으니 스토퍼를 들어 다음을 준비한다 
			if(m_nShiftMotor_Work_Site == CV_LEFT)
			{
				Set_ConveyorStopper_UpDown(CV_LEFT, IO_OFF);
			}
			else //if(m_nShiftMotor_Work_Site == CV_RIGHT)
			{
				Set_ConveyorStopper_UpDown(CV_RIGHT, IO_OFF);
			}			 
			m_nRunStep = 6010;
			break;

		case 6010: //stopper up
			if(m_nShiftMotor_Work_Site == CV_LEFT)
			{
				nRet_1 = Chk_ConveyorStopper_UpDown(CV_LEFT, IO_OFF );
			}
			else if(m_nShiftMotor_Work_Site == CV_RIGHT)
			{
				nRet_1 = Chk_ConveyorStopper_UpDown(CV_RIGHT, IO_OFF );
			}				
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 7000;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(5210, dWARNING, m_strAlarmCode);
				m_nRunStep = 6000;
			}
			break;


		/////////////////////////////////////////////////////////////
		//CV Shifter 다음 작업 위치를 다른 사이트로 변환한다 
		/////////////////////////////////////////////////////////////
		case 7000:			
			if(m_nShiftMotor_Work_Site == CV_LEFT) m_nShiftMotor_Work_Site = CV_RIGHT;
			else m_nShiftMotor_Work_Site = CV_LEFT;

			///////////////////////////////////////////////////////////////////////////////////////////////////////
			//2016.1203
			m_dwCycleTime[1] = GetTickCount();
			m_dwCycleTime[2] = m_dwCycleTime[1] - m_dwCycleTime[0];

			if (m_dwCycleTime[2] <= (DWORD)0)
			{
				m_dwCycleTime[0] = GetTickCount();
			}
			else
			{
				st_work_info.dwFrontSmema[2]	= st_work_info.dwFrontSmema[1] - st_work_info.dwFrontSmema[0];
				st_work_info.dwFrontTime[2]		= st_work_info.dwFrontTime[1] - st_work_info.dwFrontTime[0];

				if(st_lot_info[LOT_CURR].nLotStatus == LOT_START)
				{
					clsFunc.OnCycleTime(6, 
										st_lot_info[LOT_CURR].strLotNo,
										st_lot_info[LOT_CURR].strPartNo,
										st_work_info.dwFrontSmema[2], 
										m_dwCycleTime[2], 
										st_work_info.dwFrontTime[2]);
				}
				else if(st_lot_info[LOT_NEXT].nLotStatus == LOT_START )
				{
					clsFunc.OnCycleTime(6, 
										st_lot_info[LOT_NEXT].strLotNo,
										st_lot_info[LOT_NEXT].strPartNo,
										st_work_info.dwFrontSmema[2], 
										m_dwCycleTime[2], 
										st_work_info.dwFrontTime[2]);
				}
							
				if (st_work_info.dwFrontTime[2] == (DWORD)0)
				{
//								st_product_info.nFrontTime += (int)st_work_info.dwFrontSmema[2] / 1000;
				}

				st_work_info.dwFrontTime[0] = st_work_info.dwFrontTime[1] = 0;
				m_dwCycleTime[0] = GetTickCount();
			}
			//////////////////////////////////////////////////////////////////////////////////////////////////////

			//2017.0109
			if( st_tray_info[m_nShiftMotor_Work_Site].nStartStatus == 1)
			{
				st_tray_info[m_nShiftMotor_Work_Site].nStartStatus = 0;

				clsLog.LogTransfer(_T("LD_CV_STACKER"),
									_T("MOVE_CDIMM_TRAY"),
									1,
									st_tray_info[THD_WORK_TRANSFER].strMaterial,
									_T("CDIMM_TRAY"),
									st_tray_info[THD_WORK_TRANSFER].strMovePos[0], 
									st_tray_info[THD_WORK_TRANSFER].strMovePos[1], 
									1, 
									st_tray_info[THD_WORK_TRANSFER].strLogKey,
									st_tray_info[THD_WORK_TRANSFER].strLogData);
			}

			m_nRunStep = 1000;
			break;		

		default :
			break;

	}
}
 




//nOnOff -> ON : DOWN 상태
//nOnOff -> OFF : UP 상태
void CRunLdTrayInputShifterConveyor::Set_ConveyorStopper_UpDown( int nDir, int nOnOff )
{
	CString strLogKey[10];
	CString	strLogData[10];

	strLogKey[0] = _T("Mode Start");
	strLogData[0].Format(_T("%d"),0);

	m_bStopperUpDownFlag[nDir]	= false;
	m_dwWaitStopperUpDown[nDir][0]	= GetCurrentTime();

	
	if(nDir == CV_LEFT )
	{
		FAS_IO.set_out_bit( st_io_info.o_ReadyLd1CvyStkStopUpDnCyl, nOnOff);
	}
	else
	{
		FAS_IO.set_out_bit( st_io_info.o_ReadyLd2CvyStkStopUpDnCyl, nOnOff);
	}

	///kwlee 2016.0812
	if (nOnOff == IO_ON)
	{
		//LogFunction(_T("LD_TRAY_CONVEYOR"),_T("UP"),_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("LD_TRAY_CONVEYOR"),_T("UP"),0,_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
	else
	{
		//LogFunction(_T("LD_TRAY_CONVEYOR"),_T("DOWN"),_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
		clsLog.LogFunction(_T("LD_TRAY_CONVEYOR"),_T("DOWN"),0,_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
}

//nOnOff -> ON : DOWN 상태
//nOnOff -> OFF : UP 상태
int CRunLdTrayInputShifterConveyor::Chk_ConveyorStopper_UpDown( int nDir, int nOnOff )
{
	CString strLogKey[10];
	CString	strLogData[10];

	strLogKey[0] = _T("Mode End");
	strLogData[0].Format(_T("%d"),0);

//	800010 1 0 "READY_LOADING_1_STACK_STOPPER_UP_OFF_CHECK"
//	800011 1 0 "READY_LOADING_1_STACK_STOPPER_DOWN_OFF_CHECK"
//	800014 1 0 "READY_LOADING_2_STACK_STOPPER_UP_OFF_CHECK"
//	800015 1 0 "READY_LOADING_2_STACK_STOPPER_DOWN_OFF_CHECK"
//	810010 1 0 "READY_LOADING_1_STACK_STOPPER_UP_ON_CHECK_ERROR"
//	810011 1 0 "READY_LOADING_1_STACK_STOPPER_DOWN_ON_CHECK_ERROR"
//	810014 1 0 "READY_LOADING_2_STACK_STOPPER_UP_ON_CHECK_ERROR"
//	810015 1 0 "READY_LOADING_2_STACK_STOPPER_DOWN_ON_CHECK_ERROR"
	int nWaitTime = WAIT_STOPPER_UP_DN;

	if (nOnOff == IO_OFF)//UP 상태
	{
		if(nDir == CV_LEFT )
		{
			if (m_bStopperUpDownFlag[nDir] == false &&
				FAS_IO.get_in_bit(st_io_info.i_ReadyLd1CvyStopUpChk,	IO_ON)	== IO_ON &&
				FAS_IO.get_in_bit(st_io_info.i_ReadyLd1CvyStopDnChk,	IO_OFF)	== IO_OFF)
			{
				m_bStopperUpDownFlag[nDir]		= true;
				m_dwWaitStopperUpDown[nDir][0]	= GetCurrentTime();
			}
			else if (m_bStopperUpDownFlag[nDir] == true &&
					 FAS_IO.get_in_bit(st_io_info.i_ReadyLd1CvyStopUpChk,	IO_ON)	== IO_ON &&
					 FAS_IO.get_in_bit(st_io_info.i_ReadyLd1CvyStopDnChk,	IO_OFF)	== IO_OFF)
			{
				m_dwWaitStopperUpDown[nDir][1] = GetCurrentTime();
				m_dwWaitStopperUpDown[nDir][2] = m_dwWaitStopperUpDown[nDir][1] - m_dwWaitStopperUpDown[nDir][0];

				if (m_dwWaitStopperUpDown[nDir][2] <= 0)
				{
					m_dwWaitStopperUpDown[nDir][0] = GetCurrentTime();
					return RET_PROCEED;
				}

				if (m_dwWaitStopperUpDown[nDir][2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
				{
					//LogFunction(_T("LD_TRAY_CONVEYOR"),_T("DOWN"),_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
					//2017.0109
					clsLog.LogFunction(_T("LD_TRAY_CONVEYOR"),_T("DOWN"),1,_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData); 
					return RET_GOOD;
				}
			}
			else
			{
				m_dwWaitStopperUpDown[nDir][1] = GetCurrentTime();
				m_dwWaitStopperUpDown[nDir][2] = m_dwWaitStopperUpDown[nDir][1] - m_dwWaitStopperUpDown[nDir][0];

				if (m_dwWaitStopperUpDown[nDir][2] <= 0)
				{
					m_dwWaitStopperUpDown[nDir][0] = GetCurrentTime();
					return RET_PROCEED;
				}

				if (m_dwWaitStopperUpDown[nDir][2] > (DWORD)st_wait_info.nLimitWaitTime[nWaitTime])
				{
					m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_ReadyLd1CvyStopDnChk); 
					//2017.0109
					clsLog.LogFunction(_T("LD_TRAY_CONVEYOR"),_T("DOWN"),1,_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData); 
					return RET_ERROR;
				}
			}

		}
		else
		{

			if (m_bStopperUpDownFlag[nDir] == false &&
				FAS_IO.get_in_bit(st_io_info.i_ReadyLd2CvyStopUpChk,	IO_ON)	== IO_ON &&
				FAS_IO.get_in_bit(st_io_info.i_ReadyLd2CvyStopDnChk,	IO_OFF)	== IO_OFF)
			{
				m_bStopperUpDownFlag[nDir]		= true;
				m_dwWaitStopperUpDown[nDir][0]	= GetCurrentTime();
			}
			else if (m_bStopperUpDownFlag[nDir] == true &&
					 FAS_IO.get_in_bit(st_io_info.i_ReadyLd2CvyStopUpChk,	IO_ON)	== IO_ON &&
					 FAS_IO.get_in_bit(st_io_info.i_ReadyLd2CvyStopDnChk,	IO_OFF)	== IO_OFF)
			{
				m_dwWaitStopperUpDown[nDir][1] = GetCurrentTime();
				m_dwWaitStopperUpDown[nDir][2] = m_dwWaitStopperUpDown[nDir][1] - m_dwWaitStopperUpDown[nDir][0];

				if (m_dwWaitStopperUpDown[nDir][2] <= 0)
				{
					m_dwWaitStopperUpDown[nDir][0] = GetCurrentTime();
					return RET_PROCEED;
				}

				if (m_dwWaitStopperUpDown[nDir][2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
				{
					//LogFunction(_T("LD_TRAY_CONVEYOR"),_T("DOWN"),_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
					//2017.0109
					clsLog.LogFunction(_T("LD_TRAY_CONVEYOR"),_T("DOWN"),1,_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData); 
					return RET_GOOD;
				}
			}
			else
			{
				m_dwWaitStopperUpDown[nDir][1] = GetCurrentTime();
				m_dwWaitStopperUpDown[nDir][2] = m_dwWaitStopperUpDown[nDir][1] - m_dwWaitStopperUpDown[nDir][0];

				if (m_dwWaitStopperUpDown[nDir][2] <= 0)
				{
					m_dwWaitStopperUpDown[nDir][0] = GetCurrentTime();
					return RET_PROCEED;
				}

				if (m_dwWaitStopperUpDown[nDir][2] > (DWORD)st_wait_info.nLimitWaitTime[nWaitTime])
				{
					m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_ReadyLd2CvyStopDnChk); 
					//2017.0109
					clsLog.LogFunction(_T("LD_TRAY_CONVEYOR"),_T("DOWN"),1,_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData); 
					return RET_ERROR;
				}
			}
		}
	}
	else
	{
		if(nDir == CV_LEFT )
		{
			if (m_bStopperUpDownFlag[nDir] == false &&
				FAS_IO.get_in_bit(st_io_info.i_ReadyLd1CvyStopUpChk,	IO_OFF)	== IO_OFF &&
				FAS_IO.get_in_bit(st_io_info.i_ReadyLd1CvyStopDnChk,	IO_ON)	== IO_ON)
			{
				m_bStopperUpDownFlag[nDir]			= true;
				m_dwWaitStopperUpDown[nDir][0]	= GetCurrentTime();
			}
			else if (m_bStopperUpDownFlag[nDir] == true &&
					 FAS_IO.get_in_bit(st_io_info.i_ReadyLd1CvyStopUpChk,	IO_OFF)	== IO_OFF &&
					 FAS_IO.get_in_bit(st_io_info.i_ReadyLd1CvyStopDnChk,	IO_ON)	== IO_ON)
			{
				m_dwWaitStopperUpDown[nDir][1]	= GetCurrentTime();
				m_dwWaitStopperUpDown[nDir][2]	= m_dwWaitStopperUpDown[nDir][1] - m_dwWaitStopperUpDown[nDir][0];

				if (m_dwWaitStopperUpDown[nDir][2] <= 0)
				{
					m_dwWaitStopperUpDown[nDir][0]	= GetCurrentTime();
					return RET_PROCEED;
				}

				if(m_dwWaitStopperUpDown[nDir][2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
				{
					//LogFunction(_T("LD_TRAY_CONVEYOR"),_T("UP"),_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
					//2017.0109
					clsLog.LogFunction(_T("LD_TRAY_CONVEYOR"),_T("UP"),1,_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData); 
					return RET_GOOD;
				}
			}
			else
			{
				m_dwWaitStopperUpDown[nDir][1]	= GetCurrentTime();
				m_dwWaitStopperUpDown[nDir][2]	= m_dwWaitStopperUpDown[nDir][1] - m_dwWaitStopperUpDown[nDir][0];

				if (m_dwWaitStopperUpDown[nDir][2] <= 0)
				{
					m_dwWaitStopperUpDown[nDir][0]	= GetCurrentTime();
					return RET_PROCEED;
				}

				if (m_dwWaitStopperUpDown[nDir][2] > (DWORD)st_wait_info.nLimitWaitTime[nWaitTime])
				{
					m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_ReadyLd1CvyStopUpChk); 
					//2017.0109
					clsLog.LogFunction(_T("LD_TRAY_CONVEYOR"),_T("UP"),1,_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData); 
					return RET_ERROR;
				}
			}
		}
		else
		{
			if (m_bStopperUpDownFlag[nDir] == false &&
				FAS_IO.get_in_bit(st_io_info.i_ReadyLd2CvyStopUpChk,	IO_OFF)	== IO_OFF &&
				FAS_IO.get_in_bit(st_io_info.i_ReadyLd2CvyStopDnChk,	IO_ON)	== IO_ON)
			{
				m_bStopperUpDownFlag[nDir]			= true;
				m_dwWaitStopperUpDown[nDir][0]	= GetCurrentTime();
			}
			else if (m_bStopperUpDownFlag[nDir] == true &&
					 FAS_IO.get_in_bit(st_io_info.i_ReadyLd2CvyStopUpChk,	IO_OFF)	== IO_OFF &&
					 FAS_IO.get_in_bit(st_io_info.i_ReadyLd2CvyStopDnChk,	IO_ON)	== IO_ON)
			{
				m_dwWaitStopperUpDown[nDir][1]	= GetCurrentTime();
				m_dwWaitStopperUpDown[nDir][2]	= m_dwWaitStopperUpDown[nDir][1] - m_dwWaitStopperUpDown[nDir][0];

				if (m_dwWaitStopperUpDown[nDir][2] <= 0)
				{
					m_dwWaitStopperUpDown[nDir][0]	= GetCurrentTime();
					return RET_PROCEED;
				}

				if(m_dwWaitStopperUpDown[nDir][2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
				{
					//LogFunction(_T("LD_TRAY_CONVEYOR"),_T("UP"),_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
					//2017.0109
					clsLog.LogFunction(_T("LD_TRAY_CONVEYOR"),_T("UP"),1,_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData); 
					return RET_GOOD;
				}
			}
			else
			{
				m_dwWaitStopperUpDown[nDir][1]	= GetCurrentTime();
				m_dwWaitStopperUpDown[nDir][2]	= m_dwWaitStopperUpDown[nDir][1] - m_dwWaitStopperUpDown[nDir][0];

				if (m_dwWaitStopperUpDown[nDir][2] <= 0)
				{
					m_dwWaitStopperUpDown[nDir][0]	= GetCurrentTime();
					return RET_PROCEED;
				}

				if (m_dwWaitStopperUpDown[nDir][2] > (DWORD)st_wait_info.nLimitWaitTime[nWaitTime])
				{
					m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_ReadyLd2CvyStopUpChk); 
					//2017.0109
					clsLog.LogFunction(_T("LD_TRAY_CONVEYOR"),_T("UP"),1,_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData); 
					return RET_ERROR;
				}
			}

		}


	}

	return RET_PROCEED;
}


// james 2016.0802 conveyor의 ac motor ON/OFF만 제어한다, 다른 IO 는 쓰레드에서 제어 
void CRunLdTrayInputShifterConveyor::Process_Tray_Conveyor_AcMotor(int nLR_Mode)
{
	/* james 2016.0803 
	int nRet_1, nRet_2;	 

	int nTHD_Num;
	int nActionSite;
	int nAC_Motor_IO_Num[2];
 

	if(nLR_Mode == CV_LEFT)
	{		
		nTHD_Num = THD_LD_CV_STACKER_LEFT;
		nActionSite = CV_LEFT; //0
		nAC_Motor_IO_Num[nActionSite] = st_io_info.o_ReadyLd1CvyAcMotOnOff; 
		 
	}
	else //if(nLR_Mode == CV_RIGHT)
	{
		nTHD_Num = THD_LD_CV_STACKER_RIGHT;
		nActionSite = CV_RIGHT; //1
		nAC_Motor_IO_Num[nActionSite] = st_io_info.o_ReadyLd2CvyAcMotOnOff;		 
	}	 
	
	////////////////////////////////////////////////////////////////////////////////////////////
	//ac motor 제어 
	//장비 STOP 및 작업자가 메뉴얼로 OFF 해도 지속 동작 가능하게 하기 위함 
	//동작 순서 플레그 : CTL_REQ(동작 요청) - > CTL_READY(동작 완료) , CTL_FREE(바로 정지 요청
	////////////////////////////////////////////////////////////////////////////////////////////
	if(st_sync_info.nConveyor_AcMotorCtl_Req[nTHD_Num] == CTL_REQ)
	{
		FAS_IO.set_out_bit(nAC_Motor_IO_Num[nActionSite], IO_ON); //AC Motor on
		m_bAcMotorFlag[nActionSite] = true;
		m_dwAcMotorWaitTime[nActionSite][0] = GetCurrentTime();
	}
	else if(st_sync_info.nConveyor_AcMotorCtl_Req[nTHD_Num] == CTL_FREE) //모터를 바로 정지 시킨다 
	{
		m_bAcMotorFlag[nActionSite] = false;
		FAS_IO.set_out_bit(nAC_Motor_IO_Num[nActionSite], IO_OFF); //AC Motor off
	}
	else 
	{ //조건이 완료되어 AC Motor off flag가 와도 셋팅 및 안정을 위해 5초동안 계속 돌린다 
		m_dwAcMotorWaitTime[nActionSite][1] = GetCurrentTime();
		m_dwAcMotorWaitTime[nActionSite][2] = m_dwAcMotorWaitTime[nActionSite][1] - m_dwAcMotorWaitTime[nActionSite][0];
		if(m_dwAcMotorWaitTime[nActionSite][2] < 0) m_dwAcMotorWaitTime[nActionSite][0] = GetCurrentTime();
		if(m_bAcMotorFlag[nActionSite] == true && m_dwAcMotorWaitTime[nActionSite][2] > IO_CONVEYOR_WAIT_STABLE) //2초 동안은 조건이 해제되도 계속 돌린다. 킬착시키기 위해 
		{
			m_bAcMotorFlag[nActionSite] = false;
			FAS_IO.set_out_bit(nAC_Motor_IO_Num[nActionSite], IO_OFF); //AC Motor off
		}
	}
	*/
}

// james 2016.0802 ;load tray를 쉬프터에 적재시 사용 
void CRunLdTrayInputShifterConveyor::Process_LoadTray_Insert(int nLR_Mode)
{
	int nFuncRet = RET_PROCEED;
	int nRet_1, nRet_2;	 

	int nTHD_Num;
	int nActionSite;
	int n_IO_TrayIn_Num[2];
	int n_IO_TrayPassExist_Num[2];
 

	if(nLR_Mode == CV_LEFT)
	{		
		nTHD_Num = THD_LD_CV_STACKER_LEFT;
		nActionSite = CV_LEFT; //0
		n_IO_TrayIn_Num[nActionSite] = st_io_info.i_ReadyLd1CvyInTrayChk; 
		n_IO_TrayPassExist_Num[nActionSite] = st_io_info.i_ReadyLd1TrayExistChk; 
		 
	}
	else //if(nLR_Mode == CV_RIGHT)
	{
		nTHD_Num = THD_LD_CV_STACKER_RIGHT;
		nActionSite = CV_RIGHT; //1
		n_IO_TrayIn_Num[nActionSite] = st_io_info.i_ReadyLd2CvyInTrayChk;	
		n_IO_TrayPassExist_Num[nActionSite] = st_io_info.i_ReadyLd2TrayExistChk; 
	}	 

	switch (m_nInsertStep[nActionSite])
	{
		case -1:	

			m_nInsertStep[nActionSite] = 0;
			break;

		case 0:
			if(st_sync_info.nInit_Flag[THD_LD_CV_STACKER_LEFT] == INIT_READY && st_sync_info.nInit_Flag[THD_LD_CV_STACKER_RIGHT] == INIT_READY) //최기화 완료후나 가능 
			{
				m_nInsertStep[nActionSite] = 1000;
			}
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
			nRet_1 = FAS_IO.get_in_bit(n_IO_TrayIn_Num[nActionSite],	IO_ON);
			if(nRet_1 == IO_ON)  
			{
				if(m_bTrayInFlag[nActionSite] == false)
				{
					m_dwTrayInWaitTime[nActionSite][0] = GetCurrentTime();
					m_bTrayInFlag[nActionSite] = true;
				}
				else if(m_bTrayInFlag[nActionSite] == true)
				{
					m_dwTrayInWaitTime[nActionSite][1] = GetCurrentTime();
					m_dwTrayInWaitTime[nActionSite][2] = m_dwTrayInWaitTime[nActionSite][1] - m_dwTrayInWaitTime[nActionSite][0];
					if(m_dwTrayInWaitTime[nActionSite][2] <= 0) m_dwTrayInWaitTime[nActionSite][0] = GetCurrentTime();
					if(m_dwTrayInWaitTime[nActionSite][2] > IO_STABLE_WAIT) //2초 동안은 조건이 해제되도 계속 돌린다. 킬착시키기 위해 
					{
						m_bAcMotorFlag[nActionSite] = false;
						st_sync_info.nConveyor_AcMotorCtl_Req[nTHD_Num] = CTL_REQ; //ac  motor start 

						//FAS_IO.set_out_bit(st_io_info.o_ReadyLd1CvyAcMotOnOff, IO_ON); //AC Motor on

						m_nInsertStep[nActionSite] = 1100;						
					}
				}
			}
			else 
			{//감지 되지안거나, 깜박이면 플레그 클리어 
				m_bTrayInFlag[nActionSite] = false;
			}//

			/////////////////////////////////////////////////////////////////
			//트레이가 이미 쉬프터에 끝까지 장착된 상태, 작업자가 한번에 뒤까지 밀어넣은 상태 
			/////////////////////////////////////////////////////////////////			
			nRet_1 = FAS_IO.get_in_bit(n_IO_TrayPassExist_Num[nActionSite],	IO_ON);
			if(nRet_1 == IO_ON)
			{
				if(m_bTrayPassExistFlag[nActionSite] == false)
				{
					m_dwTrayPassExistWaitTime[nActionSite][0] = GetCurrentTime();
					m_bTrayPassExistFlag[nActionSite] = true;
				}
				else if(m_bTrayPassExistFlag[nActionSite] == true)
				{
					m_dwTrayPassExistWaitTime[nActionSite][1] = GetCurrentTime();
					m_dwTrayPassExistWaitTime[nActionSite][2] = m_dwTrayPassExistWaitTime[nActionSite][1] - m_dwTrayPassExistWaitTime[nActionSite][0];
					if(m_dwTrayPassExistWaitTime[nActionSite][2] <= 0) m_dwTrayPassExistWaitTime[nActionSite][0] = GetCurrentTime();
					if(m_dwTrayPassExistWaitTime[nActionSite][2] > IO_STABLE_WAIT) //100ms 동안 조건이 계속 같을 때 정상
					{
						m_bTrayPassExistFlag[nActionSite] = false;
						st_sync_info.nConveyor_AcMotorCtl_Req[nTHD_Num] = CTL_REQ; //ac  motor start, 트레이를 완전히 밀착위해 들어갔어도 더 돌려본다 

						m_nInsertStep[nActionSite] = 1100;						
					}
				}
			}
			else
			{//감지 되지안거나, 깜박이면 플레그 클리어 
				m_bTrayPassExistFlag[nActionSite] = false;
			} 
			break;

		case 1100:
			//break;

			m_bAcMotorPassFlag[nActionSite] = false;
			m_nInsertStep[nActionSite] = 2000;	
			break;

		case 2000:
			///////////////////////////////////////////////////////////////////////////////////
			//트레이가 이미 쉬프터에 끝까지 장착된 상태, 작업자가 한번에 뒤까지 밀어넣은 상태 
			///////////////////////////////////////////////////////////////////////////////////			
			nRet_1 = FAS_IO.get_in_bit(n_IO_TrayIn_Num[nActionSite],	IO_OFF);
			nRet_2 = FAS_IO.get_in_bit(n_IO_TrayPassExist_Num[nActionSite],	IO_ON);
			if(nRet_1 == IO_OFF && nRet_2 == IO_ON )
			{
				st_sync_info.nConveyor_AcMotorCtl_Req[nTHD_Num] = CTL_READY;

				/////////////////////////////////
				//트레이 적재된 정보를 셋한다 
				/////////////////////////////////
				//st_tray_info[nTHD_Num].nTrayExist = CTL_YES;
				m_nInsertStep[nActionSite] = 3000;	
			}
			else 
			{
				if(m_bAcMotorPassFlag[nActionSite] == false)
				{
					m_dwAcMotorPasstWaitTime[nActionSite][0] = GetCurrentTime();
					m_bAcMotorPassFlag[nActionSite] = true;
				}
				else if(m_bAcMotorPassFlag[nActionSite] == true)
				{
					m_dwAcMotorPasstWaitTime[nActionSite][1] = GetCurrentTime();
					m_dwAcMotorPasstWaitTime[nActionSite][2] = m_dwAcMotorPasstWaitTime[nActionSite][1] - m_dwAcMotorPasstWaitTime[nActionSite][0];
					if(m_dwAcMotorPasstWaitTime[nActionSite][2] <= 0) m_dwAcMotorPasstWaitTime[nActionSite][0] = GetCurrentTime();
					if(m_dwAcMotorPasstWaitTime[nActionSite][2] > IO_CONVEYOR_WAIT_LIMIT) //리미트 시가이 10초이며, 이 시간이 지나도 트레이가 들어가지 않으면 에러이다 
					{
						m_bAcMotorPassFlag[nActionSite] = false;
						st_sync_info.nConveyor_AcMotorCtl_Req[nTHD_Num] = CTL_FREE; //ac  motor 정지 
						m_nInsertStep[nActionSite] = 1000;						

						//에러 발생 
					}
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
			// jtkim 20160812 lot start 화면 요청.......
			st_tray_info[nTHD_Num].nNewLotIn = NO; // lot start 정보 초기화
			if (st_handler_info.cWndMain != NULL)
			{
				st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_LOT_START_CREATE_REQ, nTHD_Num);
			}
			m_nInsertStep[nActionSite] = 3100;	
			break;

		case 3100:
			if (st_tray_info[nTHD_Num].nNewLotIn == YES)
			{
				// lot start 정상적으로 완료되었으면....
				st_tray_info[nTHD_Num].nTrayExist = CTL_YES;

				st_sync_info.nLdCV_LR_TrayInStack_Flag[nTHD_Num] = CTL_YES;

				clsFunc.Handler_Tray_DataInfo_Shift(2, CTL_YES, nTHD_Num, nTHD_Num); //처음 공급된 트레이로 트레이 정보를 입력 한다 

				m_nInsertStep[nActionSite] = 4000;	
			}
			else
			{
				if (st_tray_info[nTHD_Num].nNewLotIn != 0)
				{
					//m_nInsertStep[nActionSite] = 3100;	
				}
			}
			break;

		case 4000:
			m_nInsertStep[nActionSite] = 1000;
			nFuncRet = RET_GOOD;
			break;
	}
}