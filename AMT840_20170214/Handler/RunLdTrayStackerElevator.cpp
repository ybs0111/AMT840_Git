// RunLdTrayStackerElevator.cpp : ���� �����Դϴ�.
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

// CLdTrayStackerElevator ��� �Լ�
void CRunLdTrayStackerElevator::OnThreadRun(void)
{
	//james 2016.0730
	if (st_handler_info.nSystemLock != FALSE)  return;	// ���� �ý��� Lock ������ ��� ����

	CTL_Lib.Process_Tray_Conveyor_AcMotor(0, THD_LD_STACKER, st_io_info.o_LdStkAcMotOnOff ); //load elevator stacker AC Motor �����ϴ� ��ƾ	

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

	if (st_sync_info.nInit_Flag[THD_LD_STACKER] != INIT_CLEAR)		return;		//INIT_CLEAR �϶��� �ʱ�ȭ �۾��� �Ѵ�. �ʱ�ȭ�� ������ INIT_CLEAR -> INIT_READY�� �Ǳ� ������...
	 
	switch (m_nInitStep)
	{
		case -1:
			//load cv shifter�� ���� �ֱ�ȭ�Ͽ� ���� ������ elevator�� �ʱ�ȭ �Ѵ� 
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
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ReadyCvyLd12OutTrayPassChk,	IO_OFF); //CV tray pass check  ����� �Ѵ� 
			if(nRet_1 == IO_OFF)
			{
				m_nInitStep = 300;
			}
			else
			{//Ʈ���� ���� ���� 
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

		case 100://������ �ʱ� ���� Ȯ�� 
			Set_LdStacker_UpDown(IO_OFF); //down ���� 
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

			if(nRet_1 == BD_GOOD) //�δ� �÷���Ʈ�� Ʈ���̰� ���� �� ���� 
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
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nAxisNum, st_motor_info[m_nAxisNum].d_pos[P_ELV_TRAY_INOUT_POS], COMI.mn_runspeed_rate);  //Ʈ���̸� ���� ��ġ�� �̸� �̵��Ѵ� 
			if (nRet_1 == BD_GOOD) //�������� �̵�  
			{			
				m_nRunStep = 300;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nRunStep = 200;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�  
				CTL_Lib.Alarm_Error_Occurrence(6050, dWARNING, st_alarm_info.strCode);
				m_nRunStep = 200;
			}
			break; 

		case 300:
			nRet_1 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 	
			if(nRet_1 == BD_GOOD) //�δ� �÷���Ʈ�� Ʈ���̰� ���� �� ���� 
			{
				m_strAlarmCode.Format(_T("900001")); //900000 1 0 "LOAD_STACKER_PLATE_SD_TRAY_ON_CHECK_ERROR" //kwlee 2016.0902 "900000" ->"900001"
				CTL_Lib.Alarm_Error_Occurrence(6060, dWARNING, m_strAlarmCode);
			}
			else if(nRet_1 == BD_ERROR)//�δ� �÷��̵忡 Ʈ���̰� ���� �������� ���� 
			{
				m_nRunStep = 400;
			}
			break;

		case 400:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayChk,	IO_OFF); //stacker tary �� ó������ ����� �Ѵ� 
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayArrivedChk,	IO_OFF); //elevtor �ȿ� Ʈ���� ���� Ȯ���ϴ� ����
			if(nRet_1 == IO_OFF && nRet_2 == IO_OFF )
			{
				m_nRunStep = 1000;
			}
			else
			{//Ʈ���� ���� ���� 
				if(nRet_1 == IO_ON) m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_LdStkTrayChk); 
				else m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_LdStkTrayArrivedChk); 
				CTL_Lib.Alarm_Error_Occurrence(6070, dWARNING, m_strAlarmCode);
			}
			break;

		//////////////////////////////////////////////////////////////////////////////////
		// �۾��ڰ� Ʈ���̸� �޴���� �����Ѵ� 
		//////////////////////////////////////////////////////////////////////////////////
		case 1000:  //load elevator stacker���� Ʈ���̸� ��û�� ���� 
			if(st_sync_info.nLdPlate_Tray_Supply_Req[THD_LD_STACKER] == CTL_REQ )//�÷���Ʈ���� Ʈ���� ��û 
			{
				st_sync_info.nLdElv_Tray_Req[THD_LD_STACKER] = CTL_REQ; //lot�� ���۵Ǹ� CTL_REQ �÷��׸� ���Ͽ� elevator�� Ʈ���� ������ ��û�Ѵ� 		

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
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayChk,	IO_ON); //elevator stacker tary �� �־�� �Ѵ� 
				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayArrivedChk,	IO_ON); //tary�� ������ �־�� �Ѵ� 
				nRet_3 = FAS_IO.get_in_bit(st_io_info.i_ReadyCvyLd12OutTrayPassChk,	IO_OFF); 

				if(nRet_1 == IO_ON && nRet_2 == IO_ON  && nRet_3 == IO_OFF)
				{
					st_tray_info[THD_LD_STACKER].nTrayExist = CTL_YES; //Ʈ�������� ���� ��

					st_sync_info.nConveyor_AcMotorCtl_Req[THD_LD_STACKER] = CTL_READY;

					m_dwWaitTime[0] = GetCurrentTime();
					m_nRunStep = 2100; //james 2016.0929 6000;
				}
				else
				{//Ʈ���� ����  ���� 
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
					//kwlee 2016.1103 ���� ��� ���� �� Stacker ��� ���� �˼� ����.
			/*		else
					{
						nRet_1 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayChk,	IO_OFF); //elevator stacker tary �� �־�� �Ѵ� 
						nRet_2 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayArrivedChk,	IO_OFF); //tary�� ������ �־�� �Ѵ� 

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
									else if(st_lot_info[LOT_NEXT].strLotNo == "") //lot ������ ������ ���� 
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
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayChk,	IO_ON); //elevator stacker tary �� �־�� �Ѵ� 
				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayArrivedChk,	IO_ON); //tary�� ������ �־�� �Ѵ� 
				nRet_3 = FAS_IO.get_in_bit(st_io_info.i_ReadyCvyLd12OutTrayPassChk,	IO_OFF); 

				if(nRet_1 == IO_ON && nRet_2 == IO_ON  && nRet_3 == IO_OFF)
				{
					st_tray_info[THD_LD_STACKER].nTrayExist = CTL_YES; //Ʈ�������� ���� ��

					st_sync_info.nConveyor_AcMotorCtl_Req[THD_LD_STACKER] = CTL_READY;

					m_dwWaitTime[0] = GetCurrentTime();
					m_nRunStep = 2100; //james 2016.0929 6000;
				}
				else
				{//Ʈ���� ����  ���� 
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
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayChk,	IO_ON); //elevator stacker tary �� �־�� �Ѵ� 
				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayArrivedChk,	IO_ON); //tary�� ������ �־�� �Ѵ� 
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

				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayChk,	IO_ON); //elevator stacker tary �� �־�� �Ѵ� 
				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayArrivedChk,	IO_ON); //tary�� ������ �־�� �Ѵ� 
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
		// Load Plate�� �����Ͽ� �۾��ϴ� ����
		//////////////////////////////////////////////////////////////////////////////////
		case 6000: //load plate�ּ� Ʈ���� ������ ��û������ �����Ѵ� 
			//lot end ������ üũ �Ѵ� 
			if(st_lot_info[m_nLotProcessNum].strLotNo == st_tray_info[THD_LD_STACKER].strLotNo) //2016.0912 
			{
				if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_TRAY_PLATE] >=  LOT_END
					&& st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_STACKER] == LOT_START)
				{
					st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_STACKER] =  LOT_END_START;
					m_nRunStep = 7000;
					break;
				}
			}//lot end ������ üũ �Ѵ�  

			if(st_sync_info.nLdPlate_Tray_Supply_Req[THD_LD_STACKER] == CTL_REQ) //load plate���� Ʈ���̸� ��û�� ���� 
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayChk,	IO_ON); //load stacker tray check    				
				if(nRet_1 == IO_ON)
				{
					m_nRetry = 0;

					m_nRunStep = 6100;
				}
				else
				{ //����Ŀ�� Ʈ���̰� ������ ��� �ִ� ���̴� ���� ���� Ʈ���̸� LD CV�� ��û �Ѵ�.
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
			if(nRet_1 == BD_GOOD) //�δ� �÷���Ʈ�� Ʈ���̰� ���� �� ���� 
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

			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_LdTrayPlateLfOffChk,	IO_ON); //plate off üũ  
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_LdTrayPlateRiOffChk,	IO_ON); //plate off üũ			
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

		case 6200: //Ʈ���̸� �÷���Ʈ�� �����ϴ� ��ƾ 
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
		// ���������� ���͸� �۾� ������ ��ġ�� �̵� ��Ų�� 
		//////////////////////////////////////////////////////////////////////////
		case 6300: //���� �̵� 
			nRet_1 = CTL_Lib.Elevator_Job_Move_Pos(0, m_nAxisNum,  P_ELV_SUPPLY_OFFSET);
			if(nRet_1 == RET_GOOD)
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayChk,	IO_ON); //load stacker tray check    
				nRet_2 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 	
				if(nRet_1 == IO_OFF && nRet_2 != BD_GOOD) //Ʈ���̰� ����Ŀ�� ���� ����, SD ������ �������� ���� ����, Ʈ���̰� ���� ����  
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
			if(nRet_1 == BD_ERROR) //�δ� �÷���Ʈ�� Ʈ���̰� ���� �� ���� 
			{
				m_strAlarmCode.Format(_T("900001")); //900001 1 0 "LOAD_STACKER_PLATE_SD_TRAY_OFF_CHECK_ERROR"
				CTL_Lib.Alarm_Error_Occurrence(6140, dWARNING, m_strAlarmCode);
				break;
			}

			//////////////////////////////////////////////////////////////////////////
			//load stacker elevator�� Ʈ���� ������ load plate�� ����Ʈ �Ѵ� 
			clsFunc.Handler_Tray_DataInfo_Shift(0, CTL_YES, THD_LD_STACKER, THD_LD_TRAY_PLATE);  //��� Ʈ���̸� �����ϵ� �ݺ��Ǿ�� �ϴ�, Ʈ���� �����Ѥ� ���ø� �ϰ� Ʋ���������ʴ´� 			
			//clsFunc.Handler_Tray_DataInfo_Shift(2, CTL_YES, THD_LD_STACKER, THD_LD_TRAY_PLATE);  //��� Ʈ���̸� �����ϵ� �ݺ��Ǿ�� �ϴ�, Ʈ���̿� ���簡 �մٰ� �����ϰ�, lot������ �Է��Ѵ�

			//////////////////////////////////////////////////////////////////////////

			st_sync_info.nLdPlate_Tray_Supply_Req[THD_LD_STACKER] = CTL_READY;
			m_nRunStep = 6500;
			break;

		case 6500:
			//Ʈ���̸� ���縦 ��ü ���̵��ϴ� ������ ����Ŀ�� �������� �÷���Ʈ�� Ʈ���̰� �������� . �� ��ġ���� �׳� ����ϰ�, ���� ��û���� �� ��������.
			//Ʈ���̰� ���ų�, ���޵� ���¿��� ���������� ���� �����ϸ� �÷���Ʈ Ʈ���̰� �����ϴ� ���� ���� �������� ����. ���� ���� ö��,.
			m_nRunStep = 6000;
			break; 

		////////////////////////////////////////////////////////////////
		// laod stacker���� ���̻� Ʈ���̰� ���� ���� �޴� ������ �Ѵ� 
		// ���������͸� ���� �ٽ� ���� �غ� �Ѵ� 
		////////////////////////////////////////////////////////////////
		case 7000: 		
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayArrivedChk,	IO_OFF); //tary�� ������ �־�� �Ѵ� 
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ReadyCvyLd12OutTrayPassChk,	IO_OFF); 
			if(nRet_1 == IO_ON || nRet_2 == IO_ON)
			{	if(nRet_1 == IO_ON) m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_LdStkTrayArrivedChk); 
				else m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ReadyCvyLd12OutTrayPassChk); 
				CTL_Lib.Alarm_Error_Occurrence(6150, dWARNING, m_strAlarmCode);
				break;
			}

			nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_START, m_nAxisNum, st_motor_info[m_nAxisNum].d_pos[P_ELV_TRAY_INOUT_POS], COMI.mn_runspeed_rate);   //������ �̵��Ѵ� 
			if (nRet_1 == BD_GOOD) //���� �̵� �ϸ鼭 SD�� ã�´�  
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
			nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_CHECK, m_nAxisNum, st_motor_info[m_nAxisNum].d_pos[P_ELV_TRAY_INOUT_POS], COMI.mn_runspeed_rate);  //2015.0407 james  //SD ��ġ���� �̵��ߴµ� SD�� �������� ������ Ʈ���̰� ���°��̴�
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
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayChk,	IO_OFF); //stacker tary �� ����� �Ѵ�  
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayArrivedChk,	IO_OFF);
			if(nRet_1 == IO_OFF && nRet_2 == IO_OFF)
			{
				st_tray_info[THD_LD_STACKER].nTrayExist = CTL_NO; //Ʈ�������� ���� Ŭ����
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
			else //if(nRet_1 == IO_ON) //james test 2016.0828 �׽�Ʈ ��
			{//Ʈ���� ���簡 �����Ѵ�  		

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
				st_tray_info[THD_LD_STACKER].nTrayExist = CTL_YES; //Ʈ�������� ���� Ŭ����
				m_nRunStep = 6000; //james  2016.0912 Ʈ���̰� ������ �ٽ�  ������ ���� 1000;  
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
//SD ������ �������� ��� ���� �̴� 
//	������  �ٷ� ���� ���������� �׻� SD sensor �����ġ�� �ⱸ�� �����ϸ� �̿����� �����Ѵ� 
//////////////////////////////////////////////////////////////////////////////////////////////////
	//if(m_nStackerMotorMove_UpDown_Flag != 100)
	//{
	//	m_nStackerMotorMove_UpDown_Flag = 1; //
	//}


	//P_ELV_SAFETY		  = 0,//���� ��ġ (down limit ��ġ�ϰ���)
	//P_ELV_UP_LIMIT		    ,  //high rail ���� �� Ʈ���� ���� �Ǵ� Ʈ���̸� ������  ���� ���ʱ��� �ø��� üũ
	//P_ELV_DN_LIMIT		    ,  //���ô� Ʈ���� ���� �� soflEo dncl tkdyd, �Ǵ� ���� �ظ�, Ʈ���� ���� üũ �Ǵ� Full�� ��ü�� �������� 	
	//P_ELV_SD_SENSOR				, //sd sensor ���� ��ġ(Ʈ���̰� ���� �̻� ������ ������. �ϳ��� ������ �������� ���� 	
	//P_ELV_TRAY_INOUT_POS,		//Ʈ���̸� ���� �Ǵ� �Է� ������ ��ġ(CONVEYOR ���� �����Ͽ� Ʈ���̸� �ְ�,  ���Ⱑ ������ ��ġ 
	//SD ���� ���������� ���� �̵��ϴ� ��ġ 
	//P_ELV_SUPPLY_OFFSET	,	  // SD pos + Ʈ���̸� �۾� �����ϰ� �ϴ� ��ġ 
	//P_ELV_RECEIVE_OFFSET	, // SD pos + Ʈ���̸� �޴� ��ġ 

/*
	if(st_handler_info.nRunStatus != dRUN)
	{   //time out �ð� üũ ���� ������ ����  
		if(m_bSd_Move_Flag == true) m_bSd_Move_Flag = false; 
	}

	switch(m_nStacker_MoveStep)
	{
		case 0:	
			//���� �̵� ������ �Ʒ� 3������ �����Ѵ� 
			if(nTargetPos == P_ELV_SUPPLY_OFFSET)//Ʈ���̸� �۾� ������ ������ UP�Ͽ� �����ϴ� ��ġ, SD pos ���� On ���� �� ���� ���� - �������� ��� �� + �������� P_ELV_SUPPLY_OFFSET ƼĪ��ŭ up ���� ���� ������ �Ϸ��ϰ�, Ʈ���̸�, �޵�� 
			{
				m_nStacker_MoveStep = 1000;
			}			
			else if(nTargetPos == P_ELV_RECEIVE_OFFSET) //�۾��� ���� Ʈ���̸� �޴� ��ġ, SD pos ���� On ���� �� ���� ���� - �������� ������ ��� �� - �������� P_ELV_RECEIVE_OFFSET ƼĪ��ŭ down �� �� Ʈ���̸� �޴� ��ġ  
			{
				//nRet_1 = FAS_IO.get_in_bit(st_io_info.i_UnldTrayRail1EmptyTrayFullChk,	IO_OFF);
				nRet_1 = COMI.Get_MotIOSensor(nMotNum, MOT_SENS_SD);
				nRet_2 = COMI.Check_MotPosRange(nMotNum, m_dReference_Pos_Backup, st_motor_info[nMotNum].d_allow); //�ٷ��� ������ �������� ������ ���� 
				dCurrentPos = COMI.Get_MotCurrentPos(nMotNum); 
				if(nRet_1 == BD_ERROR && nRet_2 == BD_GOOD &&  (dCurrentPos > 1 && dCurrentPos < st_motor_info[nMotNum].d_pos[P_ELV_SD_SENSOR])) 
				{ //�۾� ������ ������ ��� ���̰�, ���� ��ġ�� ���������� �������� �߰� ������ �ʿ������ �ٷ� ��������					
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
				//�ٸ� ������ �� �Լ��� ��� �� �ʿ� ���� 
				//���� �߻� �ʿ� 
			}			
			break;

			
		///////////////////////////////////////////////////////////////////////////
		// SD Sensor ���� �� �Ǵ� �Ʒ����� Ȯ���Ͽ�, ���� ��������, �Ʒ��� �ö󰣴� 
		///////////////////////////////////////////////////////////////////////////
		case 1000: //������ ��ġ Ȯ�� (SD Sensor ��� high or ���� ��ġ��������, low���� �ΰ��� ��ġ�� Ȯ���Ѵ�)			 
			nRet_1 = COMI.Get_MotIOSensor(nMotNum, MOT_SENS_SD); 	
			dCurrentPos = COMI.Get_MotCurrentPos(nMotNum); 
			CTL_Lib.SD_Sensor_Enable(0, nMotNum, CTL_NO); //sd sensor clear			

			if(nRet_1 == BD_GOOD)
			{//SD�� �����Ǹ� ���� �ִ� ���� 
				m_bSd_Move_Flag = false;
				m_nStacker_MoveStep = 6000; //���������� ���� �ִ� ���·� �Ʒ��� �����鼭 SD�� �������⸦ ��ٸ��� 
			}
			else //if(nRet_1 == BD_ERROR)
			{//SD�� �������� �ʾҰ�, SD ��ġ���� ���� ���·�, ���� �ö󰡸鼭 SD�� ã�´� 
				
				m_bSd_Move_Flag = false;
				m_nStacker_MoveStep = 2000; //traget pos�� �̵��ϴ� ��ƾ 
			}	 
			break;

		////////////////////////////////////////////////////////////
		//SD ������ ã���鼭 ���� �̵��ϴ� 
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
				{//limit �ð� ����, ���� ó���� �� �õ����� 
					COMI.Set_MotStop(1, nMotNum) ; //������� 
					m_nStacker_MoveStep = 1000;
					break;
				}
			}

			nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_START, nMotNum, st_motor_info[nMotNum].d_pos[P_ELV_SD_SENSOR], COMI.mn_runspeed_rate);   //���� �ر��� �̵��Ѵ� 
			if (nRet_1 == BD_GOOD) //���� �̵� �ϸ鼭 SD�� ã�´�  
			{					 				 
				m_nStacker_MoveStep = 2100;
			}
			else if (nRet_1 == BD_RETRY)
			{				 
				m_nStacker_MoveStep = 1000;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�  
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
			{//���������� üũ�ߴ�. 
				COMI.Set_MotStop(1, nMotNum) ; //������� 

				st_motor_info[nMotNum].n_sd_mv_chk = 0; //clear
				CTL_Lib.SD_Sensor_Enable(0, nMotNum, CTL_NO); //sd sensor clear

				m_nStacker_MoveStep = 2200;
				break;
			}
			nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_CHECK, nMotNum, st_motor_info[nMotNum].d_pos[P_ELV_SD_SENSOR], COMI.mn_runspeed_rate);  //2015.0407 james  //SD ��ġ���� �̵��ߴµ� SD�� �������� ������ Ʈ���̰� ���°��̴�
			if (nRet_1 == BD_GOOD)   
			{	//Ʈ���̰� ���� �����ϰ��̴�, �̸� Ʈ���̸� üũ�����Ͽ� Ʈ���̰� ������ �ö� �� �ʿ䰡 ���� 
				//�̰��� ���� ������ ���� 
				m_nStacker_MoveStep = 1000; //Ʈ���̰� ���� ���� 				 
			}
			else if (nRet_1 == BD_RETRY)
			{		 
				m_nStacker_MoveStep = 1000;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�  
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
			{//SD ������ �������� ������ ���� 
				COMI.Set_MotStop(1, nMotNum) ; //������� 

				m_bSd_Move_Flag = false;
				m_nStacker_MoveStep = 1000; //��õ�  ��ƾ  
				break;
			}

			m_dCurrentPos = COMI.Get_MotCurrentPos(nMotNum); 
			if(nTargetPos == P_ELV_SUPPLY_OFFSET)//Ʈ���̸� �۾� ������ ������ UP�Ͽ� �����ϴ� ��ġ, SD pos ���� On ���� �� ���� ���� - �������� ��� �� + �������� P_ELV_SUPPLY_OFFSET ƼĪ��ŭ up ���� ���� ������ �Ϸ��ϰ�, Ʈ���̸�, �޵�� 
			{
				m_dTargetPos = fabs(m_dCurrentPos + st_motor_info[nMotNum].d_pos[P_ELV_SUPPLY_OFFSET]); //���� ��ġ�� + P_ELV_SUPPLY_OFFSET 
			}
			else if(nTargetPos == P_ELV_RECEIVE_OFFSET) //Ʈ���̸� �޴� ���۽� 
			{
				m_dTargetPos = fabs(m_dCurrentPos - st_motor_info[nMotNum].d_pos[P_ELV_RECEIVE_OFFSET]); //������ ��ġ���� �� ������ sd ������ ���� �ȵɶ����� 
			}

			nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_START, nMotNum, m_dTargetPos, COMI.mn_runspeed_rate);   
			if (nRet_1 == BD_GOOD) //���ǿ� ���� ����/�Ʒ���  �̵�  
			{					 
				m_nStacker_MoveStep = 2210;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nStacker_MoveStep = 1000;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�  
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
			nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_CHECK, nMotNum, m_dTargetPos, COMI.mn_runspeed_rate);  //2015.0407 james  //SD ��ġ���� �̵��ߴµ� SD�� �������� ������ Ʈ���̰� ���°��̴�
			if (nRet_1 == BD_GOOD)   
			{	//Ʈ���̰� ���� �����ϰ��̴�, �̸� Ʈ���̸� üũ�����Ͽ� Ʈ���̰� ������ �ö� �� �ʿ䰡 ���� 
				//�̰��� ���� ������ ���� 
				m_nStacker_MoveStep = 2300; //Ʈ���̰� �ִ� ���� 				 
			}
			else if (nRet_1 == BD_RETRY)
			{		 
				m_nStacker_MoveStep = 1000;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�  
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
			if(nTargetPos == P_ELV_SUPPLY_OFFSET)//Ʈ���̸� �۾� ������ ������ UP�Ͽ� �����ϴ� ��ġ, SD pos ���� On ���� �� ���� ���� - �������� ��� �� + �������� P_ELV_SUPPLY_OFFSET ƼĪ��ŭ up ���� ���� ������ �Ϸ��ϰ�, Ʈ���̸�, �޵�� 
			{
				nRet_1 = COMI.Get_MotIOSensor(nMotNum, MOT_SENS_SD); 
				if(nRet_1 == BD_GOOD)
				{//���������� üũ�ߴ�. 					
					m_nStacker_MoveStep = 2400; // ��ƾ   
				}
				else
				{//error
					m_nStacker_MoveStep = 1000; //��õ�  ��ƾ   
				}
			}			
			else if(nTargetPos == P_ELV_RECEIVE_OFFSET) //�۾��� ���� Ʈ���̸� �޴� ��ġ, SD pos ���� On ���� �� ���� ���� - �������� ������ ��� �� - �������� P_ELV_RECEIVE_OFFSET ƼĪ��ŭ down �� �� Ʈ���̸� �޴� ��ġ  
			{
				nRet_1 = COMI.Get_MotIOSensor(nMotNum, MOT_SENS_SD); 
				if(nRet_1 == BD_ERROR)
				{//���������� �������� �ʾҴ�. 					
					m_nStacker_MoveStep = 2400; // ��ƾ   
				}
				else
				{//error
					m_nStacker_MoveStep = 1000; //��õ�  ��ƾ   
				}
			}
			break;

		case 2400:
			m_nStacker_MoveStep = 2500;
			break;

		case 2500:
			m_dCurrentPos = COMI.Get_MotCurrentPos(nMotNum); 
			if(nTargetPos == P_ELV_SUPPLY_OFFSET)//Ʈ���̸� �۾� ������ ������ UP�Ͽ� �����ϴ� ��ġ, SD pos ���� On ���� �� ���� ���� - �������� ��� �� + �������� P_ELV_SUPPLY_OFFSET ƼĪ��ŭ up ���� ���� ������ �Ϸ��ϰ�, Ʈ���̸�, �޵�� 
			{
				m_dSupply_Pos_Backup = fabs(m_dCurrentPos + st_motor_info[nMotNum].d_pos[P_ELV_SUPPLY_OFFSET]); //���� ��ġ�� + P_ELV_SUPPLY_OFFSET 
			}
			else if(nTargetPos == P_ELV_RECEIVE_OFFSET) //Ʈ���̸� �޴� ���۽� 
			{
				m_dReceive_Pos_Backup = fabs(m_dCurrentPos - st_motor_info[nMotNum].d_pos[P_ELV_RECEIVE_OFFSET]); //������ ��ġ���� �� ������ sd ������ ���� �ȵɶ����� 
			} 

			//m_dReference_Pos_Backup

			CTL_Lib.SD_Sensor_Enable(0, nMotNum, CTL_NO); //sd sensor clear
			m_nStacker_MoveStep = 0;
			nFuncRet = RET_GOOD;
			break; 

		//
		////////////////////////////////////////////////////////////
		//SD ������ �����ϰ� ������ �Ʒ��� �������� 
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
				{//limit �ð� ����, ���� ó���� �� �õ����� 
					COMI.Set_MotStop(1, nMotNum) ; //������� 
					m_nStacker_MoveStep = 1000;
					break;
				}
			}

			nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_START, nMotNum, st_motor_info[nMotNum].d_pos[P_ELV_DN_LIMIT], COMI.mn_runspeed_rate/3);   //������ �̵��Ѵ� 
			if (nRet_1 == BD_GOOD) //���� �̵� �ϸ鼭 SD�� ã�´�  
			{					 				 
				m_nStacker_MoveStep = 6100;
			}
			else if (nRet_1 == BD_RETRY)
			{				 
				m_nStacker_MoveStep = 1000;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�  
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
			{//���������� üũ�ߴ�. 
				COMI.Set_MotStop(1, nMotNum) ; //������� 			
				CTL_Lib.SD_Sensor_Enable(0, nMotNum, CTL_NO); //sd sensor clear

				m_nStacker_MoveStep = 6200;
				break;
			}
			nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_CHECK, nMotNum, st_motor_info[nMotNum].d_pos[P_ELV_DN_LIMIT], COMI.mn_runspeed_rate);  //2015.0407 james  //SD ��ġ���� �̵��ߴµ� SD�� �������� ������ Ʈ���̰� ���°��̴�
			if (nRet_1 == BD_GOOD)   
			{	 
				m_nStacker_MoveStep = 1000; //Ʈ���̰� ���� ���� 				 
			}
			else if (nRet_1 == BD_RETRY)
			{		 
				m_nStacker_MoveStep = 1000;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�  
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
			{//SD ������ �������� ������ ���������� ������ ����  
				COMI.Set_MotStop(1, nMotNum) ; //������� 

				m_bSd_Move_Flag = false;
				m_nStacker_MoveStep = 1000; //���� �ٽ� ���� �ö� ������ ��ƾ���� SD ���� ��ġ�� �����    
			}
			else
			{
				m_nStacker_MoveStep = 1000; //���� �ٽ� ���� �ö� ������ ��ƾ���� SD ���� ��ġ�� �����    
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

