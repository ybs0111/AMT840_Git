// EmptyStackerElevator.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Handler.h"
#include "RunEmptyStackerElevator.h"
#include "Variable.h"
#include "FastechPublic_IO.h"
#include "CtlBd_Library.h"

#include "ComizoaPublic.h"
#include "CtlBd_Variable.h"
#include "PublicFunction.h"
#include "LogCtrl.h"
#include "LogFromat.h"

// CEmptyStackerElevator
CRunEmptyStackerElevator clsRunEmptyStackerElevator;
CRunEmptyStackerElevator::CRunEmptyStackerElevator()
{
	m_bEmptyTrayCheck = false; 
	m_nRunStep	= -1;
	m_nInitStep	= -1;

	m_nAxisNum = M_EMPTY_ELV;

}

CRunEmptyStackerElevator::~CRunEmptyStackerElevator()
{
}


// CEmptyStackerElevator ��� �Լ�
void CRunEmptyStackerElevator::OnThreadRun(void)
{
	//james 2016.0730
	if (st_handler_info.nSystemLock != FALSE)  return;	// ���� �ý��� Lock ������ ��� ����

	//���� CTL_Lib.Process_Tray_Conveyor_AcMotor(0, THD_EMPTY_STACKER, st_io_info.o_LdStkAcMotOnOff ); //load elevator stacker AC Motor �����ϴ� ��ƾ	

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

void CRunEmptyStackerElevator::OnRunInit(void)
{
	int nRet_1, nRet_2=0;

	if (st_sync_info.nInit_Flag[THD_EMPTY_STACKER] != INIT_CLEAR)		return;		//INIT_CLEAR �϶��� �ʱ�ȭ �۾��� �Ѵ�. �ʱ�ȭ�� ������ INIT_CLEAR -> INIT_READY�� �Ǳ� ������...
	 
	switch (m_nInitStep)
	{
		case -1:
			//kwlee 2016.1221 Empty Stacker Elevator �ʱ�ȭ �� Empty Transfer  �ʱ�ȭ ����
			//if( st_sync_info.nInit_Flag[THD_WORK_TRANSFER] < INIT_READY ) break;
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
				CTL_Lib.Alarm_Error_Occurrence(4000, dWARNING, m_strAlarmCode);
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
				CTL_Lib.Alarm_Error_Occurrence(4010, CTL_Lib.mn_run_status, COMI.strCode);
				m_nInitStep = 200;
			}			
			break;

		case 300:
			m_nInitStep = 900;
			break;

		case 900:
			st_sync_info.nInit_Flag[THD_EMPTY_STACKER] = INIT_READY; 
			 
			m_nInitStep = 0;
			break;	

		default :
			break;

	}
}

void CRunEmptyStackerElevator::OnRunMove(void)
{

	int nRet_1, nRet_2=0, nRet_3=0,nRet_4=0;
	int nCount = 0;

	clsFunc.OnThreadFunctionStepTrace(3, m_nRunStep);		

	switch (m_nRunStep)
	{ 
		case -1: 

			m_nRunStep = 0;
			break;

		case 0:
			if(st_lot_info[LOT_CURR].nLotStatus == LOT_START)//james 2016.0929 
			{
				//kwlee 2017.0126
				//st_handler_info.mn_removetray = NO;
				m_nLotProcessNum = LOT_CURR;
				m_nRunStep = 100;

			}
			else if(st_lot_info[LOT_NEXT].nLotStatus == LOT_START)
			{
				m_nLotProcessNum = LOT_NEXT;
				m_nRunStep = 100;				
			}

			//////////////////////////////////////////////////////////////////////////////////////////////
			//���� ���԰� ���ÿ� �ڵ����� empty Ʈ���̸� ��������.
			//2017.0430
			if( ( st_lot_info[LOT_CURR].strLotNo !=_T("") && st_lot_info[LOT_NEXT].strLotNo ==_T("") )  || ( st_lot_info[LOT_CURR].strLotNo ==_T("") && st_lot_info[LOT_NEXT].strLotNo ==_T("") ) )
			{
				if( st_handler_info.mn_removetray == CTL_REQ )
				{
					if( st_lot_info[LOT_CURR].strLotNo ==_T("") && st_lot_info[LOT_NEXT].strLotNo ==_T("") )
					{
						m_nRunStep = 100;	
					}
					else if( st_count_info.nInCount[0][0] <= 0 && st_lot_info[LOT_CURR].strLotNo !=_T("") && st_lot_info[LOT_NEXT].strLotNo ==_T("") )
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
			//if( st_lot_info[LOT_CURR].strLotNo !=_T("") || st_lot_info[LOT_NEXT].strLotNo !=_T(""))
			if( st_lot_info[LOT_CURR].strLotNo ==_T("") && st_lot_info[LOT_NEXT].strLotNo ==_T(""))
			{
				if( st_handler_info.mn_removetray == CTL_REQ )
				{
					m_nRunStep = 100;	
				}
			}
			break; 

		case 100://������ �ʱ� ���� Ȯ�� 
			Set_StackerTray_UpDown(IO_ON); 
			m_nRunStep = 110;
			break;

		case 110:
			nRet_1 = Chk_StackerTray_UpDown(IO_ON); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 120;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(4020, dWARNING, m_strAlarmCode);
				m_nRunStep = 100;
			}
			break; 

		case 120:
			nRet_1 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 				
			if(nRet_1 == BD_GOOD) //�δ� �÷���Ʈ�� Ʈ���̰� ���� �� ���� 
			{
				m_strAlarmCode.Format(_T("900003")); //900003 1 0 "EMPTY_STACKER_PLATE_SD_TRAY_OFF_CHECK_ERROR"
				CTL_Lib.Alarm_Error_Occurrence(4030, dWARNING, m_strAlarmCode);
				
				break;
			}
			else 
			{
				m_nRunStep = 200;
			}
			break;

		case 200: //����Ŀ�� �̸� ��⸦ ���� 
			nRet_1 = CTL_Lib.Elevator_Job_Move_Pos(0, m_nAxisNum,  P_ELV_RECEIVE_OFFSET); //Ʈ���̸� Ʈ�����ۿ��� �޴´� 
			
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 300;
			}
			break;

		case 300:
			nRet_1 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 	
			if(nRet_1 == BD_GOOD) //�δ� �÷���Ʈ�� Ʈ���̰� ���� �� ���� 
			{
				m_nRunStep = 400; //james 2016.0915 �� Ʈ���̰� �־ ��������. ���� ����û ��Ȳ�� �ľ��Ͽ� ���� ��/�� ���� �ʿ� 
				break;

				m_strAlarmCode.Format(_T("900003")); //900003 1 0 "EMPTY_STACKER_PLATE_SD_TRAY_OFF_CHECK_ERROR"
				CTL_Lib.Alarm_Error_Occurrence(4100, dWARNING, m_strAlarmCode);
			}
			else if(nRet_1 == BD_ERROR)//�δ� �÷��̵忡 Ʈ���̰� ���� �������� ���� 
			{
				m_nRunStep = 400;
			}
			m_nRunStep = 400;
			break;

		case 400: //			
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_EmptyStkTrayChk,	IO_OFF); //stacker tary �� ó������ ����� �Ѵ�  
			if(nRet_1 == IO_OFF)
			{
				m_nRunStep = 1000;
			}
			else
			{//Ʈ���� ���� ���� 
				m_nRunStep = 1000;//james 2016.0915 �� Ʈ���̰� �־ ��������. ���� ����û ��Ȳ�� �ľ��Ͽ� ���� ��/�� ���� �ʿ� 
				break;
			 
				m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_EmptyStkTrayChk);  
				CTL_Lib.Alarm_Error_Occurrence(4110, dWARNING, m_strAlarmCode);
			}
			break;
			 

		//////////////////////////////////////////////////////////////////////////////////
		// empty stacker�� �����Ͽ� �۾��ϴ� ����
		//////////////////////////////////////////////////////////////////////////////////
		case 1000:  //load elevator stacker���� Ʈ���̸� ��û�� ����  
			if(st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] == CTL_REQ) //Ʈ�����ۿ��� empty stacker�� �� Ʈ���̸� ���� �� �ְ� ��û�Ѵ� 
			{//st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][1] == TRAY_PICK or TRAY_PLACE	
				if(st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][1] == WORK_PICK)
				{//Ʈ���̰� �������� ���� �� �ִ�.
					nRet_1 = FAS_IO.get_in_bit(st_io_info.i_EmptyStkTrayChk,	IO_ON); //load stacker up check    					
					if(nRet_1 == IO_ON)
					{
						m_nRunStep = 2000; 
						
						//m_bEmptyTrayCheck = false; //kwlee 2016.1221
					}
					else
					{
						//tray�� ���� ���� 
						// jtkim 20160902
						//m_nRunStep = 1100;
						//if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] == LOT_END)//james 2016.0930 [THD_TEST_RBT] ==  LOT_END) //james 2016.0918 �߰� 
						//kwlee 2016.1221 
						//if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] != LOT_END && m_bEmptyTrayCheck == false)
						//kwlee 2017.0110
						nRet_1 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayChk, IO_OFF); //elevator stacker tary �� �־�� �Ѵ� 
						nRet_2 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayArrivedChk,	IO_OFF); //tary�� ������ �־�� �Ѵ� 
						nRet_3 = FAS_IO.get_in_bit(st_io_info.i_ReadyLd1TrayExistChk, IO_OFF);
						nRet_4 = FAS_IO.get_in_bit(st_io_info.i_ReadyLd2TrayExistChk, IO_OFF);
					//	if((nRet_1 == IO_OFF && nRet_2 == IO_OFF && st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] < LOT_END)) 
							 //(nRet_1 == IO_OFF && nRet_2 == IO_OFF && st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] != LOT_CLEAR && st_count_info.nInCount[0][0] + 1) < st_lot_info[LOT_CURR].nQty))
						//if((nRet_1 == IO_OFF && nRet_2 == IO_OFF && nRet_3 == IO_OFF &&nRet_4 == IO_OFF && st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] < LOT_END))
						bool bTrayremove = false;
						if( st_handler_info.mn_removetray == CTL_REQ )
						{
							bTrayremove = true;
							if( st_handler_info.mn_removetray == CTL_REQ )
							{
								st_handler_info.mn_removetray = CTL_CHANGE;
								//m_nRunStep = 0;
								break;
							}
							else
							{
								if (st_handler_info.mn_removetray == CTL_CHANGE)
								{
									break;
								}
							}
						}
						else if(nRet_1 == IO_OFF && nRet_2 == IO_OFF && nRet_3 == IO_OFF && nRet_4 == IO_OFF && (st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] < LOT_END) )
						{
							//m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_EmptyStkTrayChk); 
							//CTL_Lib.Alarm_Error_Occurrence(4120, dWARNING, m_strAlarmCode); 
							int k, x,y,nDvc = 0,nTrayDvc = 0;
							for(  k = 0; k < 4; k++ )
							{
							 if( st_buffer_info[THD_ULD_BUFF].st_pcb_info[k].nYesNo == YES )
								 nDvc++;
							}
							for (x=0; x<st_recipe_info.nTrayX; x++)
							{
								for (y=0; y<st_recipe_info.nTrayY; y++)
								{
									if(st_tray_info[THD_ULD_1_STACKER].st_pcb_info[y][x].nYesNo == CTL_NO)
									{
										nTrayDvc++;
									}
								}
							}
							if( nTrayDvc < nDvc )
							{
								if (st_handler_info.cWndMain != NULL)
								{
									//m_bEmptyTrayCheck = true;//kwlee 2016.1221
									st_other_info.nBuzYesNo = YES;
									st_other_info.strBoxMsg = _T("There is no empty tray in Empty elevator. Please supply empty tray.");
									st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
									clsFunc.OnMCStop();
								}
							}

						
						}
						else
						{
							st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] = CTL_FREE;
							//2017.0116
// 							if( st_handler_info.mn_removetray == CTL_REQ )
// 							{
// 								st_handler_info.mn_removetray = CTL_CHANGE;
// 								m_nRunStep = 0;
// 							}
						}
					}
				}
				else //if(st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][1] == TRAY_PLACE)
				{
					m_nRunStep = 2000;
				}						
			}	
			else //james 2016.0915 lot end ���� üũ 
			{
				//james 2016.0930 
				if(m_nLotProcessNum == LOT_CURR)
				{
					if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_TRANSFER] ==  LOT_END
						&& st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_EMPTY_STACKER] != LOT_END)
					{
						st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_EMPTY_STACKER] = LOT_END; //���� ���� �۾��� �̾ ���� 
						if(st_lot_info[LOT_NEXT].nLotStatus == LOT_CLEAR)
						{						
							m_nRunStep = 9000;
						} 
					}
				}
				else if(m_nLotProcessNum == LOT_NEXT)
				{
					if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_TRANSFER] ==  LOT_END 
						&& st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_EMPTY_STACKER] != LOT_END)
					{
						st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_EMPTY_STACKER] = LOT_END; //���� ���� �۾��� �̾ ���� 
						m_nRunStep = 9000;
					}
				}
				/* james 2016.0930 
				if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_EMPTY_STACKER] == LOT_END) 
				{
					if(st_lot_info[LOT_CURR].nLotStatus == LOT_START || st_lot_info[LOT_NEXT].nLotStatus == LOT_START )
					{

						break;
					}
					else
					{
						//james 2016.0929 m_nRunStep = 9000;
						break;
					}
				}

				if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_TRANSFER] ==  LOT_END) //james 2016.0915
				{
					//2016.0929 if(st_lot_info[LOT_CURR].nLotStatus >= LOT_START && st_lot_info[LOT_NEXT].nLotStatus >= LOT_START )//james 2016.0915 
					if(st_lot_info[m_nLotProcessNum].nLotStatus >= LOT_START)
					{
						st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_EMPTY_STACKER] = LOT_END; //���� ���� �۾��� �̾ ���� 
						//james 2016.0929 m_nRunStep = 9000;
					}
					else 
					{
						m_nRunStep = 9000;
					}
				}	*/			
			}
			break;

		case 1100:
			nRet_1 = CTL_Lib.Elevator_Job_Move_Pos(0, m_nAxisNum,  P_ELV_RECEIVE_OFFSET); //Ʈ���̸� Ʈ�����ۿ��� �޴´� 
			
			if(nRet_1 == RET_GOOD)
			{
				m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_EmptyStkTrayChk); 
				CTL_Lib.Alarm_Error_Occurrence(4130, dWARNING, m_strAlarmCode); 

				m_nRunStep = 1000;
			}
			break;

		case 2000://Ʈ���̸� �÷���Ʈ�� �����ϴ� ��ƾ 
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_EmptyStkUpChk,	IO_ON); //load stacker up check    
			if(nRet_1 == IO_ON)
			{
				m_nRunStep = 3000;
			}
			else
			{
				m_nRunStep = 2100;
			}
			break;

		case 2100:
			Set_StackerTray_UpDown(IO_ON); 
			m_nRunStep = 2110;
			break;

		case 2110:
			nRet_1 = Chk_StackerTray_UpDown(IO_ON); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 2000;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(4140, dWARNING, m_strAlarmCode);

				m_nRunStep = 2000;
			}
			break;

		/////////////////////////////////////////////////////////////////////////
		// Ʈ���� �۾� ����� ���� ����üũ (Trasfer ���� Pick or Place ���� �Ǻ�)
		/////////////////////////////////////////////////////////////////////////
		case 3000:
			if(st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][1] == WORK_PICK)
			{//�ּ� �ϳ��̻��� Ʈ���̰� empty stacker�� �����ؾ� �Ѵ� 
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_EmptyStkTrayChk,	IO_ON); //load stacker up check    
				if(nRet_1 == IO_ON)
				{
					m_nWork_Site_Pos = P_ELV_SUPPLY_OFFSET;
					m_nRunStep = 4000;
				}
				else
				{
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_EmptyStkTrayChk); 
					CTL_Lib.Alarm_Error_Occurrence(4150, dWARNING, m_strAlarmCode); 
				} 				
			}
			else //if(st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][1] == WORK_PLACE)
			{//�޴� �������� ��� �־ �� 

				m_nWork_Site_Pos = P_ELV_RECEIVE_OFFSET;
				m_nTrayFull_Falg = 0; //james 2016.0921 
				m_nRunStep = 4000;
			}
			break;
			 
		//////////////////////////////////////////////////////////////////////////
		// ���������� ���͸� �۾� ������ ��ġ�� �̵� ��Ų�� 
		//////////////////////////////////////////////////////////////////////////
		case 4000: //���� �̵� 
			nRet_1 = CTL_Lib.Elevator_Job_Move_Pos(0, m_nAxisNum,  m_nWork_Site_Pos); //Ʈ���̸� Ʈ�����ۿ��� �޴´� 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 5000;
			}
			else if(nRet_1 == RET_ABORT)// TRAY FULL Error //james 2016.0921 �߰� 
			{
				//kwlee 2016.1221 Tray Full 
 				//if(st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][1] == WORK_PLACE) //Ʈ���̸� ���� �����̸� 
 				//{
					if(m_nTrayFull_Falg == 0)
					{
						m_nTrayFull_Falg = 0;
						m_nRunStep = 4100;
					}
					else
					{
						if (st_handler_info.cWndMain != NULL)
						{
							m_nTrayFull_Falg = 0;

							st_other_info.nBuzYesNo = YES;
							st_other_info.strBoxMsg = _T("[TRAY FULL] eject on Empty Stacker Tray. \r\n Please Remove it.");
							st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
						}
					}
				//}
			}
			break;

		case 4100:
			///////////////////////////////////////////////////////////////////////////////////////////////////////
			//2017.0430
			//�ϴ� ������.
			if( st_handler_info.mn_auto_empty_tray == YES )
			{//��� �ִ°� ������
				//1.���� ��� ������ �ϴ� ������
				//2. �ȵ�� ������ �ٷ� ���� ����.
				st_handler_info.mn_out_empty = YES;//������ ������. 
// 				m_nRunStep = 5000;
// 				break;
			}
			///////////////////////////////////////////////////////////////////////////////////////////////////////

			Set_StackerTray_UpDown(IO_OFF); 
			m_nRunStep = 4110;
			break;

		case 4110:
			nRet_1 = Chk_StackerTray_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nTrayFull_Falg = 100;

				m_nRunStep = 4000;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(4160, dWARNING, m_strAlarmCode);
				m_nRunStep = 4100;
			}
			break;

		case 5000:
			/*
			nRet_1 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 	
			if(nRet_1 == BD_GOOD) //�δ� �÷���Ʈ�� Ʈ���̰� ���� �� ���� 
			{
				m_strAlarmCode.Format(_T("900000")); //900001 1 0 "LOAD_STACKER_PLATE_SD_TRAY_ON_CHECK_ERROR"
				CTL_Lib.Alarm_Error_Occurrence(m_nRunStep, dWARNING, m_strAlarmCode);
				break;
			}*/

			//���ý� ������ ���� Ȯ�� �ʿ� 
			//stacker tray 1�� �� �β� :25mm
			//stacker tray 2���̻� ���ý� �� ���� : 17mm
			m_nRetry = 0;
			st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] = CTL_READY;

			m_nRunStep = 6000;
			break;		

		case 5800:
			nRet_1 = CTL_Lib.Elevator_Job_Move_Pos(0, m_nAxisNum,  P_ELV_RECEIVE_OFFSET); //Ʈ���̸� Ʈ�����ۿ��� �޴´� 
			
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 6000;
			}
			break;

		case 5900:
			nRet_1 = CTL_Lib.Elevator_Job_Move_Pos(0, m_nAxisNum,  P_ELV_SUPPLY_OFFSET); //Ʈ���̸� Ʈ�����ۿ��� �޴´� 
			
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 6000;
			}
			break;

		case 6000:
			if(st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] == CTL_FREE) //Ʈ������ pick or place Ʈ���� �۾� �Ϸ� 
			{
				if(st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][1] == WORK_PICK)
				{
					nRet_1 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 	

					if(nRet_1 == BD_ERROR)
					{
						st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] = CTL_CLEAR;
						st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][1] = CTL_CLEAR;

						m_nRunStep = 7000;
					}
					else //if(nRet_1 == BD_GOOD) //�÷���Ʈ�� Ʈ���̰� ���� �� ���� 
					{
						m_nRetry++;
						if (m_nRetry > 2)
						{
							m_nRetry = 0;
							m_strAlarmCode.Format(_T("900003")); //900001 1 0 "LOAD_STACKER_PLATE_SD_TRAY_ON_CHECK_ERROR" //kwlee 2016.0902 "900000" ->"900003"
							CTL_Lib.Alarm_Error_Occurrence(4170, dWARNING, m_strAlarmCode); 
						}
						m_nRunStep = 5800;

						break;
					} 
				}
				else //if(st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][1] == TRAY_PLACE)
				{
					nRet_1 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 	
					if(nRet_1 == BD_GOOD)//�÷���Ʈ�� Ʈ���̰� ���� �� ���� 
					{
						st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] = CTL_CLEAR;
						st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][1] = CTL_CLEAR;
						m_nRunStep = 7000;
					}
					if(nRet_1 == BD_ERROR) //�δ� �÷���Ʈ�� Ʈ���̰� ���� �ȵ� ���� 
					{
						m_nRetry++;
						if (m_nRetry > 2)
						{
							m_nRetry = 0;
							m_strAlarmCode.Format(_T("900004")); //900001 1 0 "LOAD_STACKER_PLATE_SD_TRAY_ON_CHECK_ERROR" //kwlee 2016.0902 "900000" ->"900004"
							CTL_Lib.Alarm_Error_Occurrence(4180, dWARNING, m_strAlarmCode); 
						}
						m_nRunStep = 5900;

						break;
					}
				}				
			}
			else if(st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][1] == WORK_PICK)//WORK_PLACE) //st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] == CTL_REQ) //jame s2016.0828 
			{
				if(st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] == CTL_REQ)//james 2016.0906 
				{
					m_nRunStep = 1000;
				}


				//m_nRunStep = 1000;
			}
			break;

		case 7000:
			m_nRunStep = 1000;
			break;

		//////////////////////////////////////////////////////////
		// lot end�� �۾� 
		//////////////////////////////////////////////////////////
		case 9000://down
			Set_StackerTray_UpDown(IO_OFF); 
			m_nRunStep = 9010;
			break;

		case 9010:
			nRet_1 = Chk_StackerTray_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 9100;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(4190, dWARNING, m_strAlarmCode);
				m_nRunStep = 9100;
			}
			break;

		case 9100:
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nAxisNum, st_motor_info[m_nAxisNum].d_pos[P_ELV_DN_LIMIT], COMI.mn_runspeed_rate);  //left�� �̸� �̵��� �ִ´� 
			
			if (nRet_1 == BD_GOOD) //�������� �̵�  
			{			
				st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_EMPTY_STACKER] = LOT_END; //���� ���� �۾��� �̾ ���� 
				m_nRunStep = 0;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nRunStep = 9100;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�  
				CTL_Lib.Alarm_Error_Occurrence(4200, dWARNING, st_alarm_info.strCode);
				m_nRunStep = 9100;
			}
			break; 

		default:
			break;
	}
}


void CRunEmptyStackerElevator::Set_StackerTray_UpDown( int nOnOff )
{
	CString strLogKey[10];
	CString	strLogData[10];

	strLogKey[0] = _T("Mode Start");
	strLogData[0].Format(_T("%d"),0);

	m_bStackerUpDownFlag = true;
	m_dwWaitStackerUpDown[0] = GetCurrentTime();

	FAS_IO.set_out_bit( st_io_info.o_EmptyStkUpCyl, nOnOff);
	FAS_IO.set_out_bit( st_io_info.o_EmptyStkDnCyl, !nOnOff);

	//kwlee 2016.0812
	if (nOnOff == IO_ON)
	{
		//LogFunction(_T("EMPTY_STACKER_ELEVATOR"),_T("UP"),_T("STACKER"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("EMPTY_STACKER_ELEVATOR"),_T("UP"),0,_T("STACKER"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
	else
	{
		//LogFunction(_T("EMPTY_STACKER_ELEVATOR"),_T("DOWN"),_T("STACKER"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("EMPTY_STACKER_ELEVATOR"),_T("DOWN"),0,_T("STACKER"),_T("CYLINDER"),1,strLogKey,strLogData);
	}


}


int CRunEmptyStackerElevator::Chk_StackerTray_UpDown( int nOnOff )
{
//	800206 1 0 "EMPTY_STACKER_UP_CHECK"
//	800207 1 0 "EMPTY_STACKER_DOWN_CHECK"
	int nWaitTime = WAIT_STACKER_UP_DN;

	CString strLogKey[10];
	CString	strLogData[10];

	strLogKey[0] = _T("Mode End");
	strLogData[0].Format(_T("%d"),0);

	if (nOnOff == IO_OFF)
	{
		if (m_bStackerUpDownFlag == false &&
			FAS_IO.get_in_bit(st_io_info.i_EmptyStkUpChk,	IO_OFF)	== IO_OFF &&
			FAS_IO.get_in_bit(st_io_info.i_EmptyStkDnChk,	IO_ON)	== IO_ON)
		{
			m_bStackerUpDownFlag		= true;
			m_dwWaitStackerUpDown[0]	= GetCurrentTime();
		}
		else if (m_bStackerUpDownFlag == true &&
				 FAS_IO.get_in_bit(st_io_info.i_EmptyStkUpChk,	IO_OFF)	== IO_OFF &&
				 FAS_IO.get_in_bit(st_io_info.i_EmptyStkDnChk,	IO_ON)	== IO_ON)
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
				//LogFunction(_T("EMPTY_STACKER_ELEVATOR"),_T("DOWN"),_T("STACKER_TRAY"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("EMPTY_STACKER_ELEVATOR"),_T("DOWN"),1,_T("STACKER_TRAY"),_T("CYLINDER"),1,strLogKey,strLogData);
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
				m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_EmptyStkDnChk); 
				//2017.0109
				clsLog.LogFunction(_T("EMPTY_STACKER_ELEVATOR"),_T("DOWN"),1,_T("STACKER_TRAY"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_ERROR;
			}
		}
	}
	else
	{
		if (m_bStackerUpDownFlag == false &&
			FAS_IO.get_in_bit(st_io_info.i_EmptyStkUpChk,	IO_ON)	== IO_ON &&
			FAS_IO.get_in_bit(st_io_info.i_EmptyStkDnChk,	IO_OFF)	== IO_OFF)
		{
			m_bStackerUpDownFlag			= true;
			m_dwWaitStackerUpDown[0]	= GetCurrentTime();
		}
		else if (m_bStackerUpDownFlag == true &&
				 FAS_IO.get_in_bit(st_io_info.i_EmptyStkUpChk,	IO_ON)	== IO_ON &&
				 FAS_IO.get_in_bit(st_io_info.i_EmptyStkDnChk,	IO_OFF)	== IO_OFF)
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
				//LogFunction(_T("EMPTY_STACKER_ELEVATOR"),_T("UP"),_T("STACKER_TRAY"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("EMPTY_STACKER_ELEVATOR"),_T("UP"),1,_T("STACKER_TRAY"),_T("CYLINDER"),1,strLogKey,strLogData);
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
				m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_EmptyStkUpChk); 
				//2017.0109
				clsLog.LogFunction(_T("EMPTY_STACKER_ELEVATOR"),_T("UP"),1,_T("STACKER_TRAY"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_ERROR;
			}
		}
	}

	return RET_PROCEED;
}