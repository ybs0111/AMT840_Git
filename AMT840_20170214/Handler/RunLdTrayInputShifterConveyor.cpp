// LdTrayInputShifterConveyor.cpp : ���� �����Դϴ�.
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

	m_nShiftMotor_Work_Site = CV_LEFT;//���� �۾� ���� ���� 0(left)->1(right)->0->1 �ݺ� 

	m_nAxisNum = M_LD_CV_LR_SHIFTER;
}

CRunLdTrayInputShifterConveyor::~CRunLdTrayInputShifterConveyor()
{
}


// CLdTrayInputShifterConveyor ��� �Լ�
void CRunLdTrayInputShifterConveyor::OnThreadRun(void)
{
	//james 2016.0730
	if (st_handler_info.nSystemLock != FALSE)  return;	// ���� �ý��� Lock ������ ��� ���� 

	Process_LoadTray_Insert(CV_LEFT); //Left Ʈ���� �Է���  �����ϴ� ��ƾ		 
	Process_LoadTray_Insert(CV_RIGHT); //Right Ʈ���� �Է���  �����ϴ� ��ƾ

	CTL_Lib.Process_Tray_Conveyor_AcMotor(0, THD_LD_CV_STACKER_LEFT, st_io_info.o_ReadyLd1CvyAcMotOnOff ); //Left Conveyor AC Motor �����ϴ� ��ƾ		 
	CTL_Lib.Process_Tray_Conveyor_AcMotor(0, THD_LD_CV_STACKER_RIGHT,  st_io_info.o_ReadyLd2CvyAcMotOnOff); //Right Conveyor AC Motor �����ϴ� ��ƾ	
	

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

	if (st_sync_info.nInit_Flag[THD_LD_CV_STACKER_LEFT] != INIT_CLEAR && st_sync_info.nInit_Flag[THD_LD_CV_STACKER_RIGHT] != INIT_CLEAR)		return;		//INIT_CLEAR �϶��� �ʱ�ȭ �۾��� �Ѵ�. �ʱ�ȭ�� ������ INIT_CLEAR -> INIT_READY�� �Ǳ� ������...
	 
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

		case 200: //stopper�� UP/�÷� 
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

		case 300: //����Ʈ�� �̵��ϸ� ������ ���������Ϳ� ������ Ȯ�� �ʿ� , Ʈ���̰� �� ������� ??
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
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nAxisNum, st_motor_info[m_nAxisNum].d_pos[P_LDCV_SFT_LEFT], COMI.mn_runspeed_rate);  //left�� �̸� �̵��� �ִ´� 
			if (nRet_1 == BD_GOOD) //�������� �̵�  
			{			
				m_nInitStep = 600;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nInitStep = 500;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�  
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
			m_nShiftMotor_Work_Site = CV_LEFT;//���� �۾� ���� ���� 0(left)->1(right)->0->1 �ݺ� 
			m_nRunStep = 100;			
			break;

		case 100://������ �ʱ� ���� Ȯ�� 
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
			Set_ConveyorStopper_UpDown(CV_LEFT, IO_OFF); //up ���� 
			Set_ConveyorStopper_UpDown(CV_RIGHT, IO_OFF);//up ���� 
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
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nAxisNum, st_motor_info[m_nAxisNum].d_pos[P_LDCV_SFT_LEFT], COMI.mn_runspeed_rate);  //left�� �̸� �̵��� �ִ´� 
			if (nRet_1 == BD_GOOD) //�������� �̵�  
			{			
				m_nRunStep = 1000;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nRunStep = 200;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�  
				CTL_Lib.Alarm_Error_Occurrence(5060, dWARNING, st_alarm_info.strCode);
				m_nRunStep = 200;
			}
			break; 

		case 1000:  //lot ������ Ȯ���Ѵ� 
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
					// new lot in ����� ���������� �Ϸ� �Ǿ����� 
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

							// jtkim 20161008 lot start �ð�
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
					// new lot in ����� ���������� �Ϸ� �Ǿ����� 
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

							// jtkim 20161008 lot start �ð�
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

				//james 2016.0924 �߰� 
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
		// Ʈ���̰� ����Ǹ� �۾� ������ Ʈ���̸� ����Ŀ�� �����Ѵ�
		////////////////////////////////////////////////////////////////////////
		case 2000: //loading tray�� ����Ʈ�� �ϳ��̻� ���� �Ǿ����� üũ 
			//
			//Process_LoadTray_Insert(*) �Լ����� ���� CV ������ lot ���� Ʈ���� ������ �� ó���� �Ʒ� st_sync_info.nLdCV_LR_TrayInStack_Flag[*] flag�� CTL_YES�� �� ���ش� 
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
				if(nRet_1 != IO_ON) //error, treay�� �� Ȯ�� 
				{
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ReadyLd1TrayExistChk); 
					CTL_Lib.Alarm_Error_Occurrence(5100, dWARNING, m_strAlarmCode);
					break;
				}

				nCount += 1; //Left Shift��  Ʈ���̰� �ִ� 
			}
			if(st_sync_info.nLdCV_LR_TrayInStack_Flag[THD_LD_CV_STACKER_RIGHT] == CTL_YES)
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ReadyLd2TrayExistChk,	IO_ON);
				if(nRet_1 != IO_ON) //error, treay�� �� Ȯ�� 
				{
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ReadyLd2TrayExistChk); 
					CTL_Lib.Alarm_Error_Occurrence(5110, dWARNING, m_strAlarmCode);
					break;
				}

				nCount += 2; //right Shift��  Ʈ���̰� �ִ� 
			}			 

			if(nCount > 0) //left �Ǵ� right ����Ʈ�� ������̵� ���޵Ǹ� �۾� ���� 
			{
				if(nCount == 1) //left �� �ִ� ���� //left shift �۾� ����
				{
					if(st_lot_info[LOT_CURR].nLotStatus == LOT_START) //���� lot�� lot end�� ���������� ���̻� CV ���� ���� lot�� ������ �ȵ� 
					{
						if(st_lot_info[LOT_CURR].strLotNo == st_tray_info[THD_LD_CV_STACKER_LEFT].strLotNo) //lot������ ���Ѵ� 
						{
							m_nShiftMotor_Work_Site = CV_LEFT;
							m_nRunStep = 3000; //current lot�� left shifter�� ���縦 ����
							break; //james 2016.0924 
						}	 
					}
					 //james 2016.0924 else if(st_lot_info[LOT_NEXT].nLotStatus == LOT_START) //next lot�� ���޵� ���� 
					if(st_lot_info[LOT_NEXT].nLotStatus == LOT_START) //next lot�� ���޵� ���� 
					{
						if(st_lot_info[LOT_NEXT].strLotNo == st_tray_info[THD_LD_CV_STACKER_LEFT].strLotNo)//lot������ ���Ѵ� 
						{
							m_nShiftMotor_Work_Site = CV_LEFT;
							m_nRunStep = 3000; //next lot�� left shifter�� ���縦 ����
							break; //james 2016.0924 
						} 
					}

					m_nRunStep = 1000;//james 2016.0924 �߰�, lot ������ ���������� ó������ �̵�  
				}
				else if(nCount == 2) //right ���� �ִ� ���� //right shift �۾� ����
				{
					if(st_lot_info[LOT_CURR].nLotStatus == LOT_START) //���� lot�� lot end�� ���������� ���̻� CV ���� ���� lot�� ������ �ȵ� 
					{
						 
						if(st_lot_info[LOT_CURR].strLotNo == st_tray_info[THD_LD_CV_STACKER_RIGHT].strLotNo)//lot������ ���Ѵ� 
						{
							m_nShiftMotor_Work_Site = CV_RIGHT;
							m_nRunStep = 3000; //current lot�� right shifter�� ���縦 ����
							break; //james 2016.0924 
						}	 
					}
					//james 2016.0924 else if(st_lot_info[LOT_NEXT].nLotStatus == LOT_START) //next lot�� ���޵� ���� 
					if(st_lot_info[LOT_NEXT].nLotStatus == LOT_START) //next lot�� ���޵� ���� 
					{
						if(st_lot_info[LOT_NEXT].strLotNo == st_tray_info[THD_LD_CV_STACKER_RIGHT].strLotNo)//lot������ ���Ѵ� 
						{
							m_nShiftMotor_Work_Site = CV_RIGHT;
							m_nRunStep = 3000; //next lot�� right shifter�� ���縦 ����
							break; //james 2016.0924 
						} 
					}
					m_nRunStep = 1000;//james 2016.0924 �߰�, lot ������ ���������� ó������ �̵�  
				}
				else if(nCount == 3) //left & right �� ��� �ִ� ���� 
				{
					if(m_nShiftMotor_Work_Site == CV_LEFT)//left shift �۾� ����
					{
						if(st_lot_info[LOT_CURR].nLotStatus == LOT_START) //���� lot�� lot end�� ���������� ���̻� CV ���� ���� lot�� ������ �ȵ� 
						{
							if(st_lot_info[LOT_CURR].strLotNo == st_tray_info[THD_LD_CV_STACKER_LEFT].strLotNo)//lot������ ���Ѵ� 
							{
								m_nShiftMotor_Work_Site = CV_LEFT;
								m_nRunStep = 3000; //current lot�� left shifter�� ���縦 ����
								break; //james 2016.0924 
							}	 
							else if(st_lot_info[LOT_CURR].strLotNo == st_tray_info[THD_LD_CV_STACKER_RIGHT].strLotNo)//lot������ ���Ѵ� 
							{
								m_nShiftMotor_Work_Site = CV_RIGHT;
								m_nRunStep = 3000; //current lot�� left shifter�� ���縦 ����
								break; //james 2016.0924 
							}	 
						}
						 //james 2016.0924 else if(st_lot_info[LOT_NEXT].nLotStatus == LOT_START) //next lot�� ���޵� ���� 
						if(st_lot_info[LOT_NEXT].nLotStatus == LOT_START) //next lot�� ���޵� ���� 
						{
							if(st_lot_info[LOT_NEXT].strLotNo == st_tray_info[THD_LD_CV_STACKER_LEFT].strLotNo)//lot������ ���Ѵ� 
							{
								m_nShiftMotor_Work_Site = CV_LEFT;
								m_nRunStep = 3000; //next lot�� left shifter�� ���縦 ����
								break; //james 2016.0924 
							} 
							else if(st_lot_info[LOT_NEXT].strLotNo == st_tray_info[THD_LD_CV_STACKER_RIGHT].strLotNo)//lot������ ���Ѵ� 
							{
								m_nShiftMotor_Work_Site = CV_RIGHT;
								m_nRunStep = 3000; //current lot�� left shifter�� ���縦 ����
								break; //james 2016.0924 
							}	
						}
					}
					else if(m_nShiftMotor_Work_Site == CV_RIGHT) //right shift �۾� ���� 
					{
						if(st_lot_info[LOT_CURR].nLotStatus == LOT_START) //���� lot�� lot end�� ���������� ���̻� CV ���� ���� lot�� ������ �ȵ� 
						{
							if(st_lot_info[LOT_CURR].strLotNo == st_tray_info[THD_LD_CV_STACKER_RIGHT].strLotNo)//lot������ ���Ѵ� 
							{
								m_nShiftMotor_Work_Site = CV_RIGHT;
								m_nRunStep = 3000; //current lot�� left shifter�� ���縦 ����
								break; //james 2016.0924 
							}
							else if(st_lot_info[LOT_CURR].strLotNo == st_tray_info[THD_LD_CV_STACKER_LEFT].strLotNo)//lot������ ���Ѵ� 
							{
								m_nShiftMotor_Work_Site = CV_LEFT;
								m_nRunStep = 3000; //current lot�� left shifter�� ���縦 ����
								break; //james 2016.0924 
							}	 
						}
						//james 2016.0924 else if(st_lot_info[LOT_NEXT].nLotStatus == LOT_START) //next lot�� ���޵� ���� 
						if(st_lot_info[LOT_NEXT].nLotStatus == LOT_START) //next lot�� ���޵� ���� 
						{
							if(st_lot_info[LOT_NEXT].strLotNo == st_tray_info[THD_LD_CV_STACKER_RIGHT].strLotNo)//lot������ ���Ѵ� 
							{
								m_nShiftMotor_Work_Site = CV_RIGHT;
								m_nRunStep = 3000; //current lot�� left shifter�� ���縦 ����
								break; //james 2016.0924 
							}	
							else if(st_lot_info[LOT_NEXT].strLotNo == st_tray_info[THD_LD_CV_STACKER_LEFT].strLotNo)//lot������ ���Ѵ� 
							{
								m_nShiftMotor_Work_Site = CV_LEFT;
								m_nRunStep = 3000; //next lot�� left shifter�� ���縦 ����
								break; //james 2016.0924 
							}  
						}
					}

					m_nRunStep = 1000;//james 2016.0924 �߰�, lot ������ ���������� ó������ �̵�  
				}
			}			
			break; 

		///////////////////////////////////////////////////////////////////////////////
		// Load Conveyor Shifter�� load elevator ������ CV shifter�� �̵� ���� ����� 
		///////////////////////////////////////////////////////////////////////////////
		case 3000:
			//if(st_lot_info[LOT_NEXT].nLot_THD_Status[THD_LD_STACKER] >=  LOT_END)
			//{
			//	m_nRunStep = 1000;
			//}

			if(st_sync_info.nLdElv_Tray_Req[THD_LD_STACKER] == CTL_REQ) //lot�� ���۵Ǹ� CTL_REQ �÷��׸� �� �Ѵ� 
			{
				//james 2016.0924 ���� �߰� m_nRunStep = 3100;
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
						/* james 2016.0926, load plate �� ���� �̵� 
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

			if(st_sync_info.nLdElv_Tray_Req[THD_LD_STACKER] == CTL_REQ) //lot�� ���۵Ǹ� CTL_REQ �÷��׸� �� �Ѵ� 
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
			if( m_dwNoInputWaitTime[2] > 5*1000*60)//5�� ���� �˶�
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
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ReadyCvyLd12OutTrayPassChk,	IO_OFF); //CV tray pass check  ����� �Ѵ� 
			if(nRet_1 == IO_OFF)
			{
				//����, �����Ͱ� ���� ���� 
			}
			else
			{//Ʈ���� ���� ���� 
				if(nRet_1 == IO_ON) m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_ReadyCvyLd12OutTrayPassChk); 
				CTL_Lib.Alarm_Error_Occurrence(5120, dWARNING, m_strAlarmCode);
				break;
			}

			if(m_nShiftMotor_Work_Site == CV_LEFT) 	m_dpTargetPos =  st_motor_info[m_nAxisNum].d_pos[P_LDCV_SFT_LEFT];
			else									m_dpTargetPos =  st_motor_info[m_nAxisNum].d_pos[P_LDCV_SFT_RIGHT];	 

			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nAxisNum, m_dpTargetPos, COMI.mn_runspeed_rate);  //left�� �̸� �̵��� �ִ´� 
			if (nRet_1 == BD_GOOD) //m_nShiftMotor_Work_Site �̵�  
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
			{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�  
				CTL_Lib.Alarm_Error_Occurrence(5130, dWARNING, st_alarm_info.strCode);
				m_nRunStep = 3100;
			}
			break; 

		case 3200://Ʈ���� ���¸� �ٽ��ѹ� üũ 
			if(st_sync_info.nLdCV_LR_TrayInStack_Flag[THD_LD_CV_STACKER_LEFT] == CTL_YES)
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ReadyLd1TrayExistChk,	IO_ON);
				if(nRet_1 != IO_ON) //error, treay�� �� Ȯ�� 
				{
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ReadyLd1TrayExistChk); 
					CTL_Lib.Alarm_Error_Occurrence(5140, dWARNING, m_strAlarmCode);
					break;
				} 
			}
			if(st_sync_info.nLdCV_LR_TrayInStack_Flag[THD_LD_CV_STACKER_RIGHT] == CTL_YES)
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ReadyLd2TrayExistChk,	IO_ON);
				if(nRet_1 != IO_ON) //error, treay�� �� Ȯ�� 
				{
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ReadyLd2TrayExistChk); 
					CTL_Lib.Alarm_Error_Occurrence(5150, dWARNING, m_strAlarmCode);
					break;
				}
			}
			m_nRunStep = 4000;
			break;
			

		////////////////////////////////////////////////////////////////////////
		// Load Conveyor�� Ʈ���̸� elevator ����Ʈ �۾� �� �������� �̵���Ų�� 
		// //load elevator�� �غ�Ǿ����� CV���� -> elevator�� Ʈ���̸� �����Ѵ� 
		////////////////////////////////////////////////////////////////////////
		case 4000: //�� ����Ʈ�� 2 ���Ͱ� Ʈ���̸� �̼� ���� ��ġ�� �����ϴ��� �� �����Ѵ� 
			///////////////////////////////////////////////
			//���� �����ϴ� ����� lot ������ ���Ѵ� 
			// james 2016.0912 
			///////////////////////////////////////////////
			if(st_lot_info[LOT_CURR].nLotStatus == LOT_START)
			{
				if(st_lot_info[LOT_CURR].strLotNo == st_tray_info[THD_LD_CV_STACKER_RIGHT].strLotNo)//lot������ ���Ѵ� 
				{
					//lot �̸� ���� ����  
				}	
				else if(st_lot_info[LOT_CURR].strLotNo == st_tray_info[THD_LD_CV_STACKER_LEFT].strLotNo)//lot������ ���Ѵ� 
				{
					 //lot �̸� ���� ����  
				}  
				else
				{//�ش� �ϴ� lot ������ ���� 
					break;
				}
			}
			else if(st_lot_info[LOT_NEXT].nLotStatus == LOT_START) //next lot�� ���޵� ���� 
			{
				if(st_lot_info[LOT_NEXT].strLotNo == st_tray_info[THD_LD_CV_STACKER_RIGHT].strLotNo)//lot������ ���Ѵ� 
				{
					//lot �̸� ���� ����  
				}	
				else if(st_lot_info[LOT_NEXT].strLotNo == st_tray_info[THD_LD_CV_STACKER_LEFT].strLotNo)//lot������ ���Ѵ� 
				{
					 //lot �̸� ���� ����  
				}  
				else
				{//�ش� �ϴ� lot ������ ���� 
					break;
				}
			}
			else
			{//LOT_START �� �������� ���� ������, Ʈ���̸� �� �����ϸ� �ȵȴ� 
				break;
			}
			//���� �����ϴ� ����� lot ������ ���Ѵ� 
			nRet_1 = COMI.Check_MotPosRange(m_nAxisNum, m_dpTargetPos, st_motor_info[m_nAxisNum].d_allow);//2014.1120 0);
			nRet_2 = COMI.Check_MotPosRange(M_LD_ELV, st_motor_info[M_LD_ELV].d_pos[P_ELV_TRAY_INOUT_POS], st_motor_info[M_LD_ELV].d_allow); 
			nRet_3 = FAS_IO.get_in_bit(st_io_info.i_LdStkDnChk,	IO_ON); //stacker up/dn cylinder�� down �Ǿ� �־�� �Ѵ� 
			if (nRet_1 == BD_GOOD && nRet_2 == BD_GOOD && nRet_3 == IO_ON)  //��� ������ ��ġ 
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

		case 4100: //stopper down �Ͽ� Ʈ���̸� ���� �غ��Ѵ� 

			if(m_nShiftMotor_Work_Site == CV_LEFT)
			{
				Set_ConveyorStopper_UpDown(CV_LEFT, IO_ON); //�Ǹ��� �ٿ� 
			}
			else //if(m_nShiftMotor_Work_Site == CV_RIGHT)
			{
				Set_ConveyorStopper_UpDown(CV_RIGHT, IO_ON); //�Ǹ��� �ٿ� 
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

		case 4200: //ac motor CV start �Ѵ� 
			if(m_nShiftMotor_Work_Site == CV_LEFT)
			{
				//load cv & stacker ac motor �� ���ÿ� ���� �������� ���ÿ� �̵��ϰ� �Ѵ� 
				st_sync_info.nConveyor_AcMotorCtl_Req[THD_LD_CV_STACKER_LEFT] = CTL_REQ; //cv rail ac motor ���� 
				st_sync_info.nConveyor_AcMotorCtl_Req[THD_LD_STACKER] = CTL_REQ;  //load stacker ac motor ���� 

				m_bCVTrayStackerArrivedFlag = false;
			}
			else //if(m_nShiftMotor_Work_Site == CV_RIGHT)
			{
				//load cv & stacker ac motor �� ���ÿ� ���� �������� ���ÿ� �̵��ϰ� �Ѵ� 
				st_sync_info.nConveyor_AcMotorCtl_Req[THD_LD_CV_STACKER_RIGHT] = CTL_REQ;
				st_sync_info.nConveyor_AcMotorCtl_Req[THD_LD_STACKER] = CTL_REQ;  //load stacker ac motor ���� 
				m_bCVTrayStackerArrivedFlag = false;
			}	
			m_nRunStep = 4300;
			break;

		case 4300: //Ʈ���̰� ��� ���������ε����� ���� 

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
					//load cv & stacker ac motor �� ���ÿ� ���� �������� ���ÿ� �����ϰ� �Ѵ� 
					st_sync_info.nConveyor_AcMotorCtl_Req[THD_LD_CV_STACKER_LEFT] = CTL_READY;
					st_sync_info.nConveyor_AcMotorCtl_Req[THD_LD_STACKER] = CTL_READY;
				}
				else if(m_nShiftMotor_Work_Site == CV_RIGHT)
				{
					//load cv & stacker ac motor �� ���ÿ� ���� �������� ���ÿ� �����ϰ� �Ѵ� 
					st_sync_info.nConveyor_AcMotorCtl_Req[THD_LD_CV_STACKER_RIGHT] = CTL_READY;
					st_sync_info.nConveyor_AcMotorCtl_Req[THD_LD_STACKER] = CTL_READY;
				}

				m_bCVTrayStackerArrivedFlag = false;
				m_nRunStep = 4400;
			}
			else
			{//Ʈ���̰� ���������� ���޵��� ���� ���·� ����Ʈ �ð� üũ  
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
					if(m_dwCVTrayStackerArrivedWaitTime[2] > IO_CONVEYOR_WAIT_LIMIT) //10 �� ���� ����Ʈ ����, 10�� �ȿ��� �����ؾ� �Ѵ� 
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

						//������ �߻������� AC Motor�� ��� ���� ��Ų�� 
						if(m_nShiftMotor_Work_Site == CV_LEFT)
						{
							//load cv & stacker ac motor �� ���ÿ� ���� �������� ���ÿ� �����ϰ� �Ѵ� 
							st_sync_info.nConveyor_AcMotorCtl_Req[THD_LD_CV_STACKER_LEFT] = CTL_FREE;
							st_sync_info.nConveyor_AcMotorCtl_Req[THD_LD_STACKER] = CTL_FREE;
						}
						else if(m_nShiftMotor_Work_Site == CV_RIGHT)
						{
							//load cv & stacker ac motor �� ���ÿ� ���� �������� ���ÿ� �����ϰ� �Ѵ� 
							st_sync_info.nConveyor_AcMotorCtl_Req[THD_LD_CV_STACKER_RIGHT] = CTL_FREE;
							st_sync_info.nConveyor_AcMotorCtl_Req[THD_LD_STACKER] = CTL_FREE;
						}

						m_bCVTrayStackerArrivedFlag = false; //ó������ �ٽð� �� �õ� 
						m_nRunStep = 4000;
					}
				}
			}
			break;

		case 4400: //������ ������ �ϱ� ���� �Ǹ����� ���۸� ���� ��Ű�� ���� üũ�� ���߿� �Ѵ� 
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
		// Load CV Shifter���� Load stacker �� Ʈ���̰� �̼۵Ǿ����� Ʈ���� ������ ���� ����Ʈ �Ѵ� 
		///////////////////////////////////////////////////////////////////////////////////////////////
		case 5000:
			if(m_nShiftMotor_Work_Site == CV_LEFT)
			{
				////////////////////////////////////////////////////////
				//left cv tray ���� �����͸� load stacker�� ����Ʈ ��ġ
				clsFunc.Handler_Tray_DataInfo_Shift(1, CTL_YES, THD_LD_CV_STACKER_LEFT, THD_LD_STACKER); //Ʈ���� ������ Load Left CV  -> Load Stacker�� ����Ʈ �Ѵ� // Handler_Tray_DataInfo_Shift(int nMode, int nDvc_Yes_No, int nSend_SIte, int nRcv_Site)

				////////////////////////////////////////////////////////	

				st_sync_info.nLdCV_LR_TrayInStack_Flag[THD_LD_CV_STACKER_LEFT] = CTL_NO; //Ʈ���̸� �̼��Ͽ� ������ ���� ���� Ŭ���� 
				st_tray_info[THD_LD_CV_STACKER_LEFT].nTrayExist = CTL_NO; //Ʈ�������� ���� Ŭ���� 
			}
			else if(m_nShiftMotor_Work_Site == CV_RIGHT)
			{
				////////////////////////////////////////////////////////
				//right cv tray ���� �����͸� load stacker�� ����Ʈ ��ġ 
				clsFunc.Handler_Tray_DataInfo_Shift(1, CTL_YES, THD_LD_CV_STACKER_RIGHT, THD_LD_STACKER); //Ʈ���� ������ Load Right CV  -> Load Stacker�� ����Ʈ �Ѵ�	

				////////////////////////////////////////////////////////	

				st_sync_info.nLdCV_LR_TrayInStack_Flag[THD_LD_CV_STACKER_RIGHT] = CTL_NO; //Ʈ���̸� �̼��Ͽ� ������ ���� ���� Ŭ���� 
				st_tray_info[THD_LD_CV_STACKER_RIGHT].nTrayExist = CTL_NO; //Ʈ�������� ���� Ŭ���� 
			}
			m_nRunStep = 5100;
			break;

		case 5100:
			
			
			st_sync_info.nLdElv_Tray_Req[THD_LD_STACKER] = CTL_READY; //�ε� ����Ŀ�� ���縦 ��� �־��� 
			m_nRunStep = 6000;
			break;
			
		case 6000: //stopper up, Ʈ���̸� ��� �������� �����۸� ��� ������ �غ��Ѵ� 
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
		//CV Shifter ���� �۾� ��ġ�� �ٸ� ����Ʈ�� ��ȯ�Ѵ� 
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
 




//nOnOff -> ON : DOWN ����
//nOnOff -> OFF : UP ����
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

//nOnOff -> ON : DOWN ����
//nOnOff -> OFF : UP ����
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

	if (nOnOff == IO_OFF)//UP ����
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


// james 2016.0802 conveyor�� ac motor ON/OFF�� �����Ѵ�, �ٸ� IO �� �����忡�� ���� 
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
	//ac motor ���� 
	//��� STOP �� �۾��ڰ� �޴���� OFF �ص� ���� ���� �����ϰ� �ϱ� ���� 
	//���� ���� �÷��� : CTL_REQ(���� ��û) - > CTL_READY(���� �Ϸ�) , CTL_FREE(�ٷ� ���� ��û
	////////////////////////////////////////////////////////////////////////////////////////////
	if(st_sync_info.nConveyor_AcMotorCtl_Req[nTHD_Num] == CTL_REQ)
	{
		FAS_IO.set_out_bit(nAC_Motor_IO_Num[nActionSite], IO_ON); //AC Motor on
		m_bAcMotorFlag[nActionSite] = true;
		m_dwAcMotorWaitTime[nActionSite][0] = GetCurrentTime();
	}
	else if(st_sync_info.nConveyor_AcMotorCtl_Req[nTHD_Num] == CTL_FREE) //���͸� �ٷ� ���� ��Ų�� 
	{
		m_bAcMotorFlag[nActionSite] = false;
		FAS_IO.set_out_bit(nAC_Motor_IO_Num[nActionSite], IO_OFF); //AC Motor off
	}
	else 
	{ //������ �Ϸ�Ǿ� AC Motor off flag�� �͵� ���� �� ������ ���� 5�ʵ��� ��� ������ 
		m_dwAcMotorWaitTime[nActionSite][1] = GetCurrentTime();
		m_dwAcMotorWaitTime[nActionSite][2] = m_dwAcMotorWaitTime[nActionSite][1] - m_dwAcMotorWaitTime[nActionSite][0];
		if(m_dwAcMotorWaitTime[nActionSite][2] < 0) m_dwAcMotorWaitTime[nActionSite][0] = GetCurrentTime();
		if(m_bAcMotorFlag[nActionSite] == true && m_dwAcMotorWaitTime[nActionSite][2] > IO_CONVEYOR_WAIT_STABLE) //2�� ������ ������ �����ǵ� ��� ������. ų����Ű�� ���� 
		{
			m_bAcMotorFlag[nActionSite] = false;
			FAS_IO.set_out_bit(nAC_Motor_IO_Num[nActionSite], IO_OFF); //AC Motor off
		}
	}
	*/
}

// james 2016.0802 ;load tray�� �����Ϳ� ����� ��� 
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
			if(st_sync_info.nInit_Flag[THD_LD_CV_STACKER_LEFT] == INIT_READY && st_sync_info.nInit_Flag[THD_LD_CV_STACKER_RIGHT] == INIT_READY) //�ֱ�ȭ �Ϸ��ĳ� ���� 
			{
				m_nInsertStep[nActionSite] = 1000;
			}
			break;  

		case 1000:
			//st_tray_info[nTHD_Num].nTrayExist = CTL_YES;

			if(st_tray_info[nTHD_Num].nTrayExist == CTL_YES)
			{ //�̹� Ʈ���̰� ���� �ִ� ���·� ���̻� ������ üũ�ϸ� �������� ���� �� 
				break;
			}

			/////////////////////////////////////////////////////////////////
			// �۾��ڰ� Ʈ���̸� �Ա��� ���� ����
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
					if(m_dwTrayInWaitTime[nActionSite][2] > IO_STABLE_WAIT) //2�� ������ ������ �����ǵ� ��� ������. ų����Ű�� ���� 
					{
						m_bAcMotorFlag[nActionSite] = false;
						st_sync_info.nConveyor_AcMotorCtl_Req[nTHD_Num] = CTL_REQ; //ac  motor start 

						//FAS_IO.set_out_bit(st_io_info.o_ReadyLd1CvyAcMotOnOff, IO_ON); //AC Motor on

						m_nInsertStep[nActionSite] = 1100;						
					}
				}
			}
			else 
			{//���� �����Ȱų�, �����̸� �÷��� Ŭ���� 
				m_bTrayInFlag[nActionSite] = false;
			}//

			/////////////////////////////////////////////////////////////////
			//Ʈ���̰� �̹� �����Ϳ� ������ ������ ����, �۾��ڰ� �ѹ��� �ڱ��� �о���� ���� 
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
					if(m_dwTrayPassExistWaitTime[nActionSite][2] > IO_STABLE_WAIT) //100ms ���� ������ ��� ���� �� ����
					{
						m_bTrayPassExistFlag[nActionSite] = false;
						st_sync_info.nConveyor_AcMotorCtl_Req[nTHD_Num] = CTL_REQ; //ac  motor start, Ʈ���̸� ������ �������� ��� �� �������� 

						m_nInsertStep[nActionSite] = 1100;						
					}
				}
			}
			else
			{//���� �����Ȱų�, �����̸� �÷��� Ŭ���� 
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
			//Ʈ���̰� �̹� �����Ϳ� ������ ������ ����, �۾��ڰ� �ѹ��� �ڱ��� �о���� ���� 
			///////////////////////////////////////////////////////////////////////////////////			
			nRet_1 = FAS_IO.get_in_bit(n_IO_TrayIn_Num[nActionSite],	IO_OFF);
			nRet_2 = FAS_IO.get_in_bit(n_IO_TrayPassExist_Num[nActionSite],	IO_ON);
			if(nRet_1 == IO_OFF && nRet_2 == IO_ON )
			{
				st_sync_info.nConveyor_AcMotorCtl_Req[nTHD_Num] = CTL_READY;

				/////////////////////////////////
				//Ʈ���� ����� ������ ���Ѵ� 
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
					if(m_dwAcMotorPasstWaitTime[nActionSite][2] > IO_CONVEYOR_WAIT_LIMIT) //����Ʈ �ð��� 10���̸�, �� �ð��� ������ Ʈ���̰� ���� ������ �����̴� 
					{
						m_bAcMotorPassFlag[nActionSite] = false;
						st_sync_info.nConveyor_AcMotorCtl_Req[nTHD_Num] = CTL_FREE; //ac  motor ���� 
						m_nInsertStep[nActionSite] = 1000;						

						//���� �߻� 
					}
				}
			}
			break;

		case 3000:
			//////////////////////////////////////////////////////////////////////////
			//ȭ�� ���α׷��� ����Ͽ� ����� Ʈ������ ������ ������ �� �ְ� �Ѵ�
			//lot ���� ���� 
			//////////////////////////////////////////////////////////////////////////

			/*
			if(st_lot_info[LOT_CURR].nLotStatus >= LOT_END_START)
				{
					if(st_lot_info[LOT_CURR].nLotStatus != LOT_END)//2015.0302 james st_lot_info[LOT_CURR].nLotStatus[THD_PLATE_LD_TRAY_2_SITE] == CTL_NO) //if(st_var.nLotWork_Count > 2) //�̹� �۾����� lot�� 2�� �̻��̸� ����Ѵ� 
					{ 
						break; //���� lot�� end �������̸�, �ε� plate site���� �۾��� ������������ ���� lot�� �������� ���ϰ� �Ѵ�.
					}
				}

				if(st_lot_info[LOT_NEXT].nLotStatus >= LOT_END_START)
				{					  
					break; //next lot�� end �������̸�, �ε� plate site���� �۾��� ������������ ���� lot�� �������� ���ϰ� �Ѵ�.					 
				}

				if(st_lot_info[LOT_CURR].nNewLotIn == CTL_YES && st_lot_info[LOT_NEXT].nNewLotIn == CTL_YES) //2015.0309 �߰� 
				{//lot �� 2���̻� �������̸� smema�� ��û���� �ʴ´�.
					if(st_lot_info[LOT_NEXT].nLastModule == CTL_YES)
					{
						break;
					}
				}
			*/
			// jtkim 20160812 lot start ȭ�� ��û.......
			st_tray_info[nTHD_Num].nNewLotIn = NO; // lot start ���� �ʱ�ȭ
			if (st_handler_info.cWndMain != NULL)
			{
				st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_LOT_START_CREATE_REQ, nTHD_Num);
			}
			m_nInsertStep[nActionSite] = 3100;	
			break;

		case 3100:
			if (st_tray_info[nTHD_Num].nNewLotIn == YES)
			{
				// lot start ���������� �Ϸ�Ǿ�����....
				st_tray_info[nTHD_Num].nTrayExist = CTL_YES;

				st_sync_info.nLdCV_LR_TrayInStack_Flag[nTHD_Num] = CTL_YES;

				clsFunc.Handler_Tray_DataInfo_Shift(2, CTL_YES, nTHD_Num, nTHD_Num); //ó�� ���޵� Ʈ���̷� Ʈ���� ������ �Է� �Ѵ� 

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