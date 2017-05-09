// RunUldGoodTrayLeftOutputElevator.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Handler.h"
#include "RunUldGoodTrayLeftOutputElevator.h"
#include "Variable.h"
#include "FastechPublic_IO.h"
#include "CtlBd_Library.h"

#include "ComizoaPublic.h"
#include "CtlBd_Variable.h"
#include "PublicFunction.h"
#include "AlgMemory.h"
#include "LogCtrl.h"
#include "LogFromat.h"

// CUldGoodTrayLeftOutputElevator
CRunUldGoodTrayLeftOutputElevator clsRunUldGoodTrayLeftOutputElevator;
CRunUldGoodTrayLeftOutputElevator::CRunUldGoodTrayLeftOutputElevator()
{

	m_nRunStep	= -1;
	m_nInitStep	= -1;

	m_nAxisNum = M_ULD_1_ELV;
}

CRunUldGoodTrayLeftOutputElevator::~CRunUldGoodTrayLeftOutputElevator()
{
}


// CUldGoodTrayLeftOutputElevator ��� �Լ�
void CRunUldGoodTrayLeftOutputElevator::OnThreadRun(void)
{
	//james 2016.0804
	if (st_handler_info.nSystemLock != FALSE)  return;	// ���� �ý��� Lock ������ ��� ����

	CTL_Lib.Process_Tray_Conveyor_AcMotor(0, THD_ULD_1_STACKER, st_io_info.o_Uld1StkAcMotOnOff ); // elevator stacker AC Motor �����ϴ� ��ƾ	

//	FAS_IO.set_out_bit(st_io_info.o_Uld1StkAcMotOnOff, IO_ON); //AC Motor on

//	st_basic_info.nUldGoodTrayStack_Count = 30; //4;

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

void CRunUldGoodTrayLeftOutputElevator::OnRunInit(void)
{
	int nRet_1, nRet_2, nRet_3;

	if (st_sync_info.nInit_Flag[THD_ULD_1_STACKER] != INIT_CLEAR)		return;		//INIT_CLEAR �϶��� �ʱ�ȭ �۾��� �Ѵ�. �ʱ�ȭ�� ������ INIT_CLEAR -> INIT_READY�� �Ǳ� ������...

	switch (m_nInitStep)
	{
		case -1:			 
			if(st_sync_info.nInit_Flag[THD_WORK_TRANSFER] < INIT_READY) break;
			m_nInitStep = 0;
			break;

		case 0:		
			m_nInitStep = 100;
			break;

		case 100: //up
			Set_GoodStackerLeftTray_UpDown(IO_OFF); 
			m_nInitStep = 110;
			break;

		case 110:
			nRet_1 = Chk_GoodStackerLeftTray_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nInitStep = 200;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(5000, dWARNING, m_strAlarmCode);
				m_nInitStep = 100;
			}
			break;

		case 200:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ReadyUld1CvyPassTrayChk,	IO_OFF); //stacker tary �� ó������ ����� �Ѵ�  
			if(nRet_1 == IO_OFF)
			{
				m_nInitStep = 300;
			}
			else
			{//Ʈ���� ���� ���� 
				m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_ReadyUld1CvyPassTrayChk);  
				CTL_Lib.Alarm_Error_Occurrence(5001, dWARNING, m_strAlarmCode);
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
				CTL_Lib.Alarm_Error_Occurrence(5002, CTL_Lib.mn_run_status, COMI.strCode);
				m_nInitStep = 300;
			}			
			break;

		case 400:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Uld1StkTrayExistChk,	IO_OFF); //stacker�� Ʈ���̰� �ִ�, ������ ������ �Ѵ�   
			if(nRet_1 == IO_OFF)
			{
				m_nInitStep = 410;
				m_nInitStep = 900; //james 2016.0826 
			}
			else
			{//Ʈ���� �������� �ʴ´�  
				 m_nInitStep = 900;
			}
			break;

		case 410:
			Set_GoodConveyorLeftStopper_UpDown(IO_OFF); 
			m_nInitStep = 411;
			break;

		case 411:
			nRet_1 = Chk_GoodConveyorLeftStopper_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nInitStep = 420;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(5003, dWARNING, m_strAlarmCode);
				m_nInitStep = 400;
			}
			break;

		case 420:
			st_sync_info.nConveyor_AcMotorCtl_Req[THD_ULD_1_STACKER] = CTL_REQ; //cv rail ac motor �����Ͽ� ������ Ʈ���̸� �� ����  
			m_bCVTrayFlag = false;
			m_nInitStep = 430;			 
			break;

		case 430:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Uld1StkTrayExistChk,	IO_OFF); //stacker tary �� ó������ ����� �Ѵ�  
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ReadyUld1CvyPassTrayChk,	IO_OFF); //stacker tary �� ó������ ����� �Ѵ�  
			nRet_3 = FAS_IO.get_in_bit(st_io_info.i_ReadyUld1CvyOutTrayChk,	IO_ON); //stacker tary �� ó������ ����� �Ѵ�  
			if(nRet_1 == IO_OFF && nRet_2 == IO_OFF && nRet_1 == IO_ON)
			{
				st_sync_info.nConveyor_AcMotorCtl_Req[THD_ULD_1_STACKER] = CTL_READY; 
				m_nInitStep = 900;
			}
			else
			{//Ʈ���̰� ���������� ���޵��� ���� ���·� ����Ʈ �ð� üũ  
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
					if(m_dwCVTrayWaitTime[2] > IO_CONVEYOR_WAIT_LIMIT) //2�� ������ ������ �����ǵ� ��� ������. ų����Ű�� ���� 
					{	//error 
						if(nRet_1 == IO_ON)
						{
							m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_Uld1StkTrayExistChk); 
							CTL_Lib.Alarm_Error_Occurrence(5004, dWARNING, m_strAlarmCode);
						}
						else if(nRet_2 == IO_ON)
						{
							m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ReadyUld1CvyPassTrayChk); 
							CTL_Lib.Alarm_Error_Occurrence(5005, dWARNING, m_strAlarmCode);
						}
						else //if(nRet_3 == IO_OFF)
						{
							m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ReadyUld1CvyOutTrayChk); 
							CTL_Lib.Alarm_Error_Occurrence(5006, dWARNING, m_strAlarmCode);
						}

						//������ �߻������� AC Motor�� ��� ���� ��Ų�� 
						st_sync_info.nConveyor_AcMotorCtl_Req[THD_ULD_1_STACKER] = CTL_FREE;  

						m_bCVTrayFlag = false; //ó������ �ٽð� �� �õ� 
						m_nRunStep = 420;
					}
				}
			} 
			break;

		case 900:
			st_sync_info.nInit_Flag[THD_ULD_1_STACKER] = INIT_READY; 
			 
			m_nInitStep = 0;
			break;	

		default :
			break;

	}
}


void CRunUldGoodTrayLeftOutputElevator::OnRunMove(void)
{
	int nRet_1, nRet_2=0, nRet_3=0;
	int nCount = 0;
	// jtkim 20161004
	int x, y;
	CString strMsg;

	clsFunc.OnThreadFunctionStepTrace(28, m_nRunStep);	
	switch (m_nRunStep)
	{ 
		case -1: 

			m_nRunStep = 0;
			break;

		case 0:
			if(st_lot_info[LOT_CURR].nLotStatus == LOT_START)
			{
				m_nLotProcessNum = LOT_CURR;
				if(st_lot_info[m_nLotProcessNum].nStacker_Good_THD_Num == THD_ULD_1_STACKER)//�ش� ����Ŀ�� ���õ������� �������� 
				{
					st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_ULD_1_STACKER] =  LOT_START;
					m_nRunStep = 100;
				}
			}
			else if(st_lot_info[LOT_NEXT].nLotStatus == LOT_START)
			{
				m_nLotProcessNum = LOT_NEXT;
				if(st_lot_info[m_nLotProcessNum].nStacker_Good_THD_Num == THD_ULD_1_STACKER) //�ش� ����Ŀ�� ���õ������� �������� 
				{
					st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_ULD_1_STACKER] =  LOT_START;
					m_nRunStep = 100;
				}
			}

			//////////////////////////////////////////////////////////////////////////////////////////////
			//���� ���԰� ���ÿ� �ڵ����� empty Ʈ���̸� ��������.
			//2017.0430
			if( ( st_lot_info[LOT_CURR].strLotNo !=_T("") && st_lot_info[LOT_NEXT].strLotNo ==_T("") )  || ( st_lot_info[LOT_CURR].strLotNo ==_T("") && st_lot_info[LOT_NEXT].strLotNo ==_T("") ) )
			{
				if( st_handler_info.mn_removetray == CTL_REQ  && st_handler_info.mn_uldnum==1 )
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
			//if( st_lot_info[LOT_CURR].strLotNo !=_T("") || st_lot_info[LOT_NEXT].strLotNo !=_T("")	)
			if( st_lot_info[LOT_CURR].strLotNo ==_T("") && st_lot_info[LOT_NEXT].strLotNo ==_T("")	)
			{
				if( st_handler_info.mn_removetray == CTL_REQ && st_handler_info.mn_uldnum==1 )
				{
					m_nRunStep = 100;	
				}
			}
			break;

		case 100://������ �ʱ� ���� Ȯ�� 
			Set_GoodStackerLeftTray_UpDown(IO_OFF); 
			m_nRunStep = 110;
			break;

		case 110:
			nRet_1 = Chk_GoodStackerLeftTray_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 120;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(5100, dWARNING, m_strAlarmCode);
				m_nRunStep = 100;
			}
			break; 

		case 120:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Uld1StkTrayExistChk,	IO_OFF); //stacker tary �� ó������ ����� �Ѵ�  
			if(nRet_1 == IO_OFF)
			{
				m_nRunStep = 200;
			}
			else
			{//Ʈ���� ���� ���� 
				m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_Uld1StkTrayExistChk);  
				CTL_Lib.Alarm_Error_Occurrence(5101, dWARNING, m_strAlarmCode);
			}
			break;

		case 200:
			Set_GoodStackerLeftTray_UpDown(IO_ON); 
			m_nRunStep = 210;
			break;

		case 210:
			nRet_1 = Chk_GoodStackerLeftTray_UpDown(IO_ON); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 220;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(5102, dWARNING, m_strAlarmCode);
				m_nRunStep = 200;
			}
			break; 

		case 220:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Uld1StkTrayExistChk,	IO_OFF); //stacker tary �� ó������ ����� �Ѵ�  
			if(nRet_1 == IO_OFF)
			{
				m_nRunStep = 300;
			}
			else
			{//Ʈ���� ���� ���� 
			
				if( st_handler_info.mn_removetray == CTL_REQ)
				{
					m_nRunStep = 300;
					break;
				}
				//�ٿ� �Ŀ� ������ üũ�ؼ� Ʈ���̰� �ִ��� Ȯ���Ѵ�. �����Ÿ��� Ŀ�� �ٷ� ������ ���� ������.
				//���� ������ ���� ���� Ÿ������ ���潱�� �ʿ�				
				m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_Uld1StkTrayExistChk);  
				CTL_Lib.Alarm_Error_Occurrence(5103, dWARNING, m_strAlarmCode);
			}
			break;

		case 300:
			nRet_1 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 	
			if(nRet_1 == BD_GOOD) //�δ� �÷���Ʈ�� Ʈ���̰� ���� �� ���� 
			{
				m_strAlarmCode.Format(_T("900006")); //900006 1 0 "UNLAOD_1_STACKER_PLATE_SD_TRAY_OFF_CHECK_ERROR"
				CTL_Lib.Alarm_Error_Occurrence(5104, dWARNING, m_strAlarmCode); 
			}
			else 
			{
				m_nRunStep = 400;
			}
			break;

		case 400://�̸� �޴� ��ġ�� �̵��Ѵ� 
			nRet_1 = CTL_Lib.Elevator_Job_Move_Pos(0, m_nAxisNum,  P_ELV_RECEIVE_OFFSET); //Ʈ���̸� Ʈ�����ۿ��� �޴´� 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 500;
			}
			break;

		case 500:
			nRet_1 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 	
			if(nRet_1 == BD_GOOD) //�δ� �÷���Ʈ�� Ʈ���̰� ���� �� ���� 
			{
				m_strAlarmCode.Format(_T("900006")); //900006 1 0 "UNLAOD_1_STACKER_PLATE_SD_TRAY_OFF_CHECK_ERROR"
				CTL_Lib.Alarm_Error_Occurrence(5104, dWARNING, m_strAlarmCode);
			}
			else if(nRet_1 == BD_ERROR)//�δ� �÷��̵忡 Ʈ���̰� ���� �������� ���� 
			{
				m_nRunStep = 600;
			}
			break;

		case 600: //
			m_nRunStep = 900;
			break;

		case 900: //�۾� ������ ����Ŀ �϶��� �����ϵ��� ����
			m_nRunStep = 1000;
			break;

		//////////////////////////////////////////////////////////////////////////////
		// Work robot�� LOT �� Ȯ���Ͽ� �۾��� ��ε� ����Ŀ�� �����Ͽ� ��û�Ѵ� 
		//1. LOT ���۽� unload stacker ������ LOT ������ �Է��Ͽ� ���� ����ϰ� �Ѵ� 
		//   st_tray_info[THD_ULD_1_STACKER].strLotNo
		/////////////////////////////////////////////////////////////////////////////
		case 1000: 
			m_nRetry = 0;

			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Uld1StkUpChk,	IO_ON);
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_Uld1StkDnChk,	IO_OFF);
			if(nRet_1 == IO_ON && nRet_2 == IO_OFF)
			{				
				m_nRunStep = 1100; 
			}
			else
			{
				Set_GoodStackerLeftTray_UpDown(IO_ON); 
				m_nRunStep = 1010;
			}
			break;

		case 1010:
			nRet_1 = Chk_GoodStackerLeftTray_UpDown(IO_ON); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 1100;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(5105, dWARNING, m_strAlarmCode);
				m_nRunStep = 1000;
			}
			break; 

		case 1100:
			//2017.0116
// 			if( st_lot_info[LOT_CURR].strLotNo ==_T("") && st_lot_info[LOT_NEXT].strLotNo ==_T(""))
// 			{
// 				if( st_handler_info.mn_removetray == CTL_REQ )
// 				{
// 					if( st_handler_info.mn_uldnum != 1)		return;
// 					st_sync_info.nWorkRbt_Dvc_Req[THD_ULD_1_STACKER][0] = CTL_REQ;
// 				}
// 				else if (st_handler_info.mn_removetray == CTL_CHANGE) //kwlee 2017.0117
// 				{
// 					m_nRunStep = 2000;
// 				}
// 			}
			//2017.0508
			if( st_handler_info.mn_removetray == CTL_REQ || st_handler_info.mn_removetray == CTL_CHANGE)
			{
				if(st_lot_info[LOT_NEXT].strLotNo ==_T(""))
				{
					if( st_handler_info.mn_uldnum != 1)		return;
					st_sync_info.nWorkRbt_Dvc_Req[THD_ULD_1_STACKER][0] = CTL_REQ;
				}
				else if (st_handler_info.mn_removetray == CTL_CHANGE) //kwlee 2017.0117
				{
					m_nRunStep = 2000;
				}
				else
				{
					st_handler_info.mn_removetray = CTL_NO;
				}
			}
			if(st_sync_info.nWorkRbt_Dvc_Req[THD_ULD_1_STACKER][0] == CTL_REQ) //��ũ �κ����� uld 1 stacker�� Ʈ���� ������ ��û �ߴ� 
			{
				if(st_tray_info[THD_ULD_1_STACKER].nTrayExist == CTL_YES) //Ʈ���� ������ �ִ� ���� 
				{					
					nRet_1 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 	
					if(nRet_1 == BD_GOOD)
					{
						m_nRunStep = 2000;
					}
					else //if(nRet_1 == BD_ERROR) //�÷���Ʈ�� Ʈ���̰� ���� �ȵ� ���� 
					{
						m_nRetry++;
						if (m_nRetry > 2)
						{
							m_nRetry = 0;
							m_strAlarmCode.Format(_T("900005")); //900005 1 0 "UNLAOD_1_STACKER_PLATE_SD_TRAY_ON_CHECK_ERROR"
							CTL_Lib.Alarm_Error_Occurrence(5305, dWARNING, m_strAlarmCode);	
						}
						m_nRunStep = 1200;
					} 
				}
				else //if(st_tray_info[THD_ULD_1_STACKER].nTrayExist == CTL_NO)  //Ʈ���̰� ������ 
				{
					nRet_1 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 	
					if(nRet_1 == BD_ERROR) //�÷���Ʈ�� Ʈ���̰� ���� �ȵ� ���� 
					{
						m_nRunStep = 3000;
					}
					else if(nRet_1 == BD_GOOD) //�÷���Ʈ�� Ʈ���̰� ���� �ȵ� ���� 
					{
						m_nRetry++;
						if (m_nRetry > 2)
						{
							m_nRetry = 0;

							m_strAlarmCode.Format(_T("900006")); //900006 1 0 "UNLAOD_1_STACKER_PLATE_SD_TRAY_OFF_CHECK_ERROR"
							CTL_Lib.Alarm_Error_Occurrence(5106, dWARNING, m_strAlarmCode);
						}
						m_nRunStep = 1300;
					}
				}
			}
			else 
			{
				if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] ==  LOT_END) //���̻� �۾��� ���簡 ���� , lot end ����, �̰����� ��ü lot�� �����Ѵ�
				{				
					nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Uld1StkTrayExistChk,	IO_ON); //elevator stacker tary �� �־�� �Ѵ� 
					if(st_count_info.nUnLdStacker_Good_TrayCnt[0] == 0 && nRet_1 == IO_OFF)
					{//Ʈ���̰� ���� ���� 
						m_nRunStep = 9000;
					}
					else
					{//Ʈ���̰� �ִ� ����
						m_nRunStep = 7000; 
					}  
				}
			}
			break;

		case 1200:
			nRet_1 = CTL_Lib.Elevator_Job_Move_Pos(0, m_nAxisNum,  P_ELV_SUPPLY_OFFSET); //Ʈ���̸� Ʈ�����ۿ��� �޴´� 			
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 1100;
			}
			else if(nRet_1 == RET_ERROR)//james 2016.0923 
			{
				m_nRunStep = 1100;
			}
			break;

		case 1300:
			nRet_1 = CTL_Lib.Elevator_Job_Move_Pos(0, m_nAxisNum,  P_ELV_RECEIVE_OFFSET); //Ʈ���̸� Ʈ�����ۿ��� �޴´� 			
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 1100;
			}
			break;

		//////////////////////////////////////////////////
		// Ʈ���̰� �غ�Ǿ�������, Ʈ���� ������ ã�´� 
		//////////////////////////////////////////////////
		case 2000: 
			nRet_1 = clsFunc.Find_TrayWork_Pos(0, THD_ULD_1_STACKER, CTL_NO, m_npFindWorkPosYXCPB, THD_WORK_RBT, st_worker_info.strLotNo_Empty);
			//2017.0116
			//if( st_handler_info.mn_removetray == CTL_CHANGE && st_handler_info.mn_uldnum == 1 )
			//2017.0117
			if( st_handler_info.mn_removetray == CTL_REQ && st_handler_info.mn_uldnum == 1 )
			{
				nRet_1 = RET_SKIP;
			}
			else if( st_handler_info.mn_removetray == CTL_CHANGE )
			{
				if( st_handler_info.mn_uldnum != 1 )
				{
					m_nRunStep = 0;
				}
				else
				{
					nRet_1 = RET_SKIP;
					m_nRunStep = 3100;
				}
				//���ü��� ���µ�.. 
// 				if (st_handler_info.cWndList != NULL)  // ����Ʈ �� ȭ�� ����
// 				{
// 					strMsg.Format(_T("[REMOVE_TRAY] Removetray Unloading #1 Good Stacker Tray : %d "), st_handler_info.mn_uldnum ) ;
// 					clsMem.OnNormalMessageWrite(strMsg);
// 					return;
// 				}
			}
			/////
			if(nRet_1 == RET_GOOD) //�۾� ������ ���簡 �ִ�
			{				
				///////////////////////////////////////////////////////
				// Ʈ���� ��ġ �ٽ� Ȯ�� �� ���� ����  
				///////////////////////////////////////////////////////
				nRet_1 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 	
				if(nRet_1 == BD_GOOD)
				{
					st_sync_info.nWorkRbt_Dvc_Req[THD_ULD_1_STACKER][0] = CTL_READY; //���� Ʈ������ �۾��� �����ϴ� 
					m_nRunStep = 1000;
				}
				else //if(nRet_1 == BD_ERROR) //�÷���Ʈ�� Ʈ���̰� ���� �ȵ� ���� 
				{
					m_strAlarmCode.Format(_T("900005")); //900005 1 0 "UNLAOD_1_STACKER_PLATE_SD_TRAY_ON_CHECK_ERROR"
					CTL_Lib.Alarm_Error_Occurrence(5107, dWARNING, m_strAlarmCode);		
				} 
			}
			else if(nRet_1 == RET_SKIP) //���̻� ����̽��� ���� ������ ���� 
			{	//�� Ʈ���̸� �����ϰ�, �ٽ� �޾ƾ� �Ѵ� 

				///////////////////////////////////////////////////////
				// Ʈ���̸� �� �����ϰ� �ٽ� Ʈ���̸� ���� �޾ƾ� �Ѵ� 
				///////////////////////////////////////////////////////
				m_nRunStep = 3000;

				//////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//2016.1203
				if (st_work_info.nCycleYesNo == YES)
				{
					st_work_info.dwCycleTime[0][1] = GetTickCount();
					st_work_info.dwCycleTime[0][2] = st_work_info.dwCycleTime[0][1] - st_work_info.dwCycleTime[0][0];

					if (st_work_info.dwCycleTime[2] <= 0)
					{
						st_work_info.dwCycleTime[0][0] = GetTickCount();
					}
					else
					{
						st_work_info.dwTrayCycle	= st_work_info.dwCycleTime[0][2] / 1000;
						
						st_work_info.nCycleCount++;
						st_work_info.dwLotCycle	+= st_work_info.dwTrayCycle;
						// jtkim 20160328 product
						st_product_info.dCycleTime	= (double)st_work_info.dwLotCycle;
						st_product_info.nCycleCount	= st_work_info.nCycleCount;

						clsFunc.OnLotCycleData(st_lot_info[LOT_CURR].strLotNo,
							                   st_lot_info[LOT_CURR].strPartNo,
								               st_work_info.nCycleCount,
							                   st_work_info.dwTrayCycle, 
											   st_work_info.dwLotCycle);

						st_work_info.dwCycleTime[0][0] = GetTickCount();
					}
				}
				else
				{
					st_work_info.nCycleYesNo	= YES;
					st_work_info.dwCycleTime[0][0] = GetTickCount();
				}

			//////////////////////////////////////////////////////////////////////////////////////////////////////////////

			}
			break;		

		///////////////////////////////////////////////////////
		// Ʈ���̰� ������, Ʈ���̸� ���� �޾ƾ� �Ѵ� 
		///////////////////////////////////////////////////////
		case 3000:
			nRet_1 = CTL_Lib.Elevator_Job_Move_Pos(0, m_nAxisNum,  P_ELV_RECEIVE_OFFSET); //Ʈ���̸� Ʈ�����ۿ��� �޴´� 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 3100;
			}
			break;

		case 3100:
			if(st_tray_info[THD_ULD_1_STACKER].nTrayExist == CTL_YES) //Ʈ���̰� ������, �۾��� ������ ���� ���·� �̹� �ѹ� �۾��� �Ϸ��ϴ�Ʈ�����̴�. Ʈ���� ������ �ִ� ���� 
			{
				st_tray_info[THD_ULD_1_STACKER].nTrayExist = CTL_NO; //tray ���� Ŭ���� 

				st_lot_info[m_nLotProcessNum].nUnLdTrayCnt++; //�ش� lot�� Ʈ���� ī��Ʈ ���� ���� 

				//st_count_info.nUnldStackerTray++; //Ʈ���� ����Ŀ Ʈ���� ���� ���� �ϳ� up 
				st_count_info.nUnLdStacker_Good_TrayCnt[0]++; //unload good 1 stacker tray ���� ����

				//if(st_count_info.nUnLdStacker_Good_TrayCnt[0] >= st_basic_info.nUldGoodTrayStack_Count)//���� ������ ���� ������ �����Ѵ�  
				//2017.0116
				if( st_handler_info.mn_removetray == CTL_CHANGE || st_count_info.nUnLdStacker_Good_TrayCnt[0] >= st_basic_info.nUldGoodTrayStack_Count )
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

		/////////////////////////////////////////
		// ���ο� Ʈ���̸� ��û�Ѵ� 
		/////////////////////////////////////////
		case 3200:
			st_sync_info.nWorkTransfer_Req[THD_ULD_1_STACKER][0] = CTL_REQ;
			st_sync_info.nWorkTransfer_Req[THD_ULD_1_STACKER][1] = WORK_PLACE;
			m_nRunStep = 3300;
			break;

		case 3300:
			//if(st_sync_info.nWorkTransfer_Req[THD_ULD_1_STACKER][0] == CTL_READY)
			if(st_sync_info.nWorkTransfer_Req[THD_ULD_1_STACKER][0] == CTL_READY)
			{
				nRet_1 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 	
				if(nRet_1 == BD_GOOD)
				{
					///////////////////////////////////////////////////////
					// Ʈ���� ������ ��� ���� ����  
					///////////////////////////////////////////////////////
					st_tray_info[THD_ULD_1_STACKER].nTrayExist = CTL_YES;//2016.0912 

					st_sync_info.nWorkTransfer_Req[THD_ULD_1_STACKER][0] = CTL_CLEAR;
					m_nRunStep = 3400;
				}
				else //if(nRet_1 == BD_ERROR) //�÷���Ʈ�� Ʈ���̰� ���� �ȵ� ���� 
				{
					m_strAlarmCode.Format(_T("900005")); //900005 1 0 "UNLAOD_1_STACKER_PLATE_SD_TRAY_ON_CHECK_ERROR"
					CTL_Lib.Alarm_Error_Occurrence(5108, dWARNING, m_strAlarmCode);						
				} 
			}
			else if(st_handler_info.mn_removetray == CTL_NO &&st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_TRANSFER] ==  LOT_END)//2016.0911
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Uld1StkTrayExistChk,	IO_ON); //elevator stacker tary �� �־�� �Ѵ� 
				if(st_count_info.nUnLdStacker_Good_TrayCnt[0] == 0 && nRet_1 == IO_OFF)
				{//Ʈ���̰� ���� ���� 
					m_nRunStep = 9000;
				}
				else
				{//Ʈ���̰� �ִ� ����
					m_nRunStep = 7000; 
				}  
			}
			//2017.0116
			//if( st_lot_info[LOT_CURR].strLotNo !=_T("") || st_lot_info[LOT_NEXT].strLotNo !=_T(""))
			//kwlee 2017.0127
// 			if( st_lot_info[LOT_CURR].strLotNo ==_T("") && st_lot_info[LOT_NEXT].strLotNo ==_T("")	)
// 			{
				if( st_handler_info.mn_removetray == CTL_CHANGE )//st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] = CTL_FREE;
				{
					st_sync_info.nWorkTransfer_Req[THD_ULD_1_STACKER][0] = CTL_CLEAR;
					
					nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Uld1StkTrayExistChk,	IO_ON); //elevator stacker tary �� �־�� �Ѵ� 
					if(st_count_info.nUnLdStacker_Good_TrayCnt[0] == 0 && nRet_1 == IO_OFF)
					{//Ʈ���̰� ���� ���� 
						m_nRunStep = 9000;
					}
					else
					{//Ʈ���̰� �ִ� ����
						m_nRunStep = 7000; 
					} 
					break;//�� �̻� ���� Ʈ���̰� ����.
				}
// 				else
// 				{
// 					m_nRunStep = 3400; 
// 				}
			//}
			break;

		case 3400:
			///////////////////////////////////////////////////////
			// Ʈ���̸� �κ��� ���� �� �ִ� ��ġ�� ����  
			///////////////////////////////////////////////////////
			nRet_1 = CTL_Lib.Elevator_Job_Move_Pos(0, m_nAxisNum,  P_ELV_SUPPLY_OFFSET); //Ʈ���̸� Ʈ�����ۿ��� �޴´� 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 3500;
			}
			else if(nRet_1 == RET_ABORT) //tray full
			{
				m_nRunStep = 7000;
			}
			break;

		case 3500:
			nRet_1 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 	
			if(nRet_1 == BD_GOOD)
			{			 
				m_nRunStep = 3600;
			}
			else //if(nRet_1 == BD_ERROR) //�÷���Ʈ�� Ʈ���̰� ���� �ȵ� ���� 
			{
				m_strAlarmCode.Format(_T("900005")); //900005 1 0 "UNLAOD_1_STACKER_PLATE_SD_TRAY_ON_CHECK_ERROR"
				CTL_Lib.Alarm_Error_Occurrence(5109, dWARNING, m_strAlarmCode);						
			} 
			break;

		case 3600:
			//james 2016.0912 st_sync_info.nWorkRbt_Dvc_Req[THD_ULD_1_STACKER][0] = CTL_READY;//2016.0828 
			//st_tray_info[THD_ULD_1_STACKER].nTrayExist = CTL_YES; //tray ���� ��
			m_nRunStep = 1000;
			break;
			

		////////////////////////////////////////////////////////////////
		// stacker�� ���� ����� Ʈ���̸� ������ 
		////////////////////////////////////////////////////////////////
		case 7000: 
			Set_GoodStackerLeftTray_UpDown(IO_OFF); 
			m_nRunStep = 7010;
			break;

		case 7010:
			nRet_1 = Chk_GoodStackerLeftTray_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 7020;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(5110, dWARNING, m_strAlarmCode);
				m_nRunStep = 7000;
			}
			break; 

		case 7020:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Uld1StkTrayExistChk,	IO_ON); //elevator stacker tary �� �־�� �Ѵ� 
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ReadyUld1CvyPassTrayChk,	IO_OFF); //tary�� �߰��� �����Ǹ� �ȵȴ�  
			if(nRet_1 == IO_ON && nRet_2 == IO_OFF)
			{				
				Set_GoodConveyorLeftStopper_UpDown(IO_OFF); 
				st_sync_info.nConveyor_AcMotorCtl_Req[THD_ULD_1_STACKER] = CTL_FREE;//Ȥ�� ac motor�� �������̸� ��� ������ ��Ų�� 

				m_nRunStep = 7100;
			}
			else
			{//Ʈ����  ���� 
				if(nRet_1 == IO_ON) m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Uld1StkTrayExistChk); 
				else  m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_ReadyUld1CvyPassTrayChk); 
				CTL_Lib.Alarm_Error_Occurrence(5111, dWARNING, m_strAlarmCode);
			}
			break;

		case 7100:			
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nAxisNum, st_motor_info[m_nAxisNum].d_pos[P_ELV_TRAY_INOUT_POS], COMI.mn_runspeed_rate);  //2015.0407 james  //SD ��ġ���� �̵��ߴµ� SD�� �������� ������ Ʈ���̰� ���°��̴�
			if (nRet_1 == BD_GOOD)   
			{	 
				st_tray_info[THD_ULD_1_STACKER].nTrayExist = CTL_NO; //james 2016.0923 
				m_nRunStep = 7200;  			 
			}
			else if (nRet_1 == BD_RETRY)
			{		 
				m_nRunStep = 7000;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{ 
				CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);

				////////if (st_handler_info.cWndList != NULL)  
				{
					//clsMem.OnNormalMessageWrite(_T("UnLoader Alarm : 2010"));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG); 
				}
				m_nRunStep = 7000;
			}
			break;

		case 7200:
			nRet_1 = Chk_GoodConveyorLeftStopper_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 7300;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(5200, dWARNING, m_strAlarmCode);
				m_nRunStep = 7210;
			}
			break;  

		case 7210:
			Set_GoodConveyorLeftStopper_UpDown(IO_OFF); 
			m_nRunStep = 7200;
			break;

		case 7300:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Uld1StkTrayExistChk,	IO_ON); //elevator stacker tary �� �־�� �Ѵ� 
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ReadyUld1CvyPassTrayChk,	IO_OFF); //tary�� �߰��� �����Ǹ� �ȵȴ� 
			nRet_3 = FAS_IO.get_in_bit(st_io_info.i_ReadyUld1CvyOutTrayChk,	IO_OFF); //Ʈ���̰� ����� �Ѵ� 
			if(nRet_1 == IO_ON && nRet_2 == IO_OFF  && nRet_3 == IO_OFF)
			{				
				m_nRunStep = 7400;
			}
			else
			{//Ʈ����  ���� 
				if(nRet_1 == IO_ON) m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Uld1StkTrayExistChk); 
				else if(nRet_2 == IO_ON) m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ReadyUld1CvyPassTrayChk); 
				else  m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ReadyUld1CvyOutTrayChk); 
				CTL_Lib.Alarm_Error_Occurrence(5201, dWARNING, m_strAlarmCode);
			}
			break;

		case 7400:
			st_sync_info.nConveyor_AcMotorCtl_Req[THD_ULD_1_STACKER] = CTL_REQ;
			m_bCVTrayFlag = false; 
			m_nRunStep = 7500;
			break;

		case 7500:	
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Uld1StkTrayExistChk,	IO_OFF);
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ReadyUld1CvyPassTrayChk,	IO_OFF); 
			nRet_3 = FAS_IO.get_in_bit(st_io_info.i_ReadyUld1CvyOutTrayChk,	IO_ON); 

			if(nRet_1 == IO_OFF && nRet_2 == IO_OFF && nRet_3 == IO_ON)
			{
				//load cv & stacker ac motor �� ���ÿ� ���� �������� ���ÿ� �����ϰ� �Ѵ� 
				st_sync_info.nConveyor_AcMotorCtl_Req[THD_ULD_1_STACKER] = CTL_READY;  			 

				m_bCVTrayFlag = false;
				m_nRunStep = 7600;
			}
			else
			{//Ʈ���̰� ���������� ���޵��� ���� ���·� ����Ʈ �ð� üũ  
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
					if(m_dwCVTrayWaitTime[2] > IO_CONVEYOR_WAIT_LIMIT) //2�� ������ ������ �����ǵ� ��� ������. ų����Ű�� ���� 
					{	//error 
						if(nRet_1 == IO_ON)
						{
							m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_Uld1StkTrayExistChk); 
							CTL_Lib.Alarm_Error_Occurrence(5202, dWARNING, m_strAlarmCode);
						}
						else if(nRet_2 == IO_ON)
						{
							m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_ReadyUld1CvyPassTrayChk); 
							CTL_Lib.Alarm_Error_Occurrence(5203, dWARNING, m_strAlarmCode);
						}
						else //if(nRet_3 == IO_OFF)
						{
							m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ReadyUld1CvyOutTrayChk); 
							CTL_Lib.Alarm_Error_Occurrence(5204, dWARNING, m_strAlarmCode);
						}

						//������ �߻������� AC Motor�� ��� ���� ��Ų�� 
						st_sync_info.nConveyor_AcMotorCtl_Req[THD_ULD_1_STACKER] = CTL_FREE;  		
						m_bCVTrayFlag = false;
				 
						m_nRunStep = 7400;
					}
				}
			}
			break;

		case 7600:

			//2017.0116
			if( st_handler_info.mn_removetray == CTL_CHANGE )
			{
				st_count_info.nUnLdStacker_Good_TrayCnt[0] = 0; //2016.0804  count clear
				m_nRunStep = 9000; 
				break;
			}

			if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] ==  LOT_END)
			{			
				st_count_info.nUnLdStacker_Good_TrayCnt[0] = 0; //2016.0804  count clear
				m_nRunStep = 9000; 
			}
			else
			{
				///////////////////////////////////////
				// �޼����� ����Ͽ� �۾��ڿ��� �˸��� 
				///////////////////////////////////////

				if (st_handler_info.cWndMain != NULL)
				{
					st_other_info.nBuzYesNo = YES;
					st_other_info.strBoxMsg = _T("There are trays to eject on Unloading Good#1 Coveyor. \r\n Please Remove it.");
					st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
				}

				st_count_info.nUnLdStacker_Good_TrayCnt[0] = 0; //2016.0804  count clear

				Set_GoodConveyorLeftStopper_UpDown(IO_ON); //james 2016.0930 


				//////////////////////////////////////////////////////////////////////////////////////////////////////
				//2016.1203
				if (st_work_info.nCycleYesNo == YES)
				{
					st_work_info.dwCycleTime[0][1] = GetTickCount();
					st_work_info.dwCycleTime[0][2] = st_work_info.dwCycleTime[1] - st_work_info.dwCycleTime[0];

					if (st_work_info.dwCycleTime[0][2] <= (DWORD)0)
					{
						st_work_info.dwCycleTime[0][0] = GetTickCount();
					}
					else
					{
						st_work_info.dwTrayCycle	= st_work_info.dwCycleTime[0][2] / 1000;
						
						st_work_info.nCycleCount++;
						st_work_info.dwLotCycle	+= st_work_info.dwTrayCycle;

						// jtkim 20160328 product
						st_product_info.dCycleTime	= (double)st_work_info.dwLotCycle;
						st_product_info.nCycleCount	= st_work_info.nCycleCount;

						clsFunc.OnLotCycleData(st_lot_info[LOT_CURR].strLotNo,
												st_lot_info[LOT_CURR].strPartNo,
												st_work_info.nCycleCount,
												st_work_info.dwTrayCycle, 
												st_work_info.dwLotCycle);

						st_work_info.dwCycleTime[0][0] = GetTickCount();
					}
				}
				else
				{
					st_work_info.nCycleYesNo	= YES;
					st_work_info.dwCycleTime[0][0] = GetTickCount();
				}

				m_dwCycleTime[1] = GetTickCount();
				m_dwCycleTime[2] = m_dwCycleTime[1] - m_dwCycleTime[0];

				if (m_dwCycleTime[2] <= (DWORD)0)
				{
					m_dwCycleTime[0] = GetTickCount();
				}
				else
				{
					st_work_info.dwRearSmema[0][2] = st_work_info.dwRearSmema[0][1] - st_work_info.dwRearSmema[0][0];
					st_work_info.dwRearTime[0][2]	= st_work_info.dwRearTime[0][1] - st_work_info.dwRearTime[0][0];

					clsFunc.OnCycleTime(7, 
										st_tray_info[THD_ULD_1_STACKER].strLotNo,
										st_tray_info[THD_ULD_1_STACKER].strPartNo,
										st_work_info.dwRearSmema[0][2], 
										m_dwCycleTime[2], 
										st_work_info.dwRearTime[0][2]);

					st_work_info.dwRearTime[0][0]	= st_work_info.dwRearTime[0][1] = 0;
					m_dwCycleTime[0] = GetTickCount();
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				m_nRunStep = 1000;
			}
			break;

		////////////////////////////////////////////
		//LOT ENd�� Ʈ���� ���� 
		////////////////////////////////////////////
		case 9000: 
			//st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_ULD_1_STACKER] =  LOT_END;
			//2017.0116
			if( st_handler_info.mn_removetray != CTL_CHANGE )
			{
				st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_ULD_1_STACKER] =  LOT_END;
			}
			st_handler_info.mn_removetray = NO; //kwlee 2017.0126
			//kwlee 2016.1114
			if (st_handler_info.cWndList != NULL)  // ����Ʈ �� ȭ�� ����
			{
				strMsg.Format(_T("[LOT END] Unloading #1 Good Stacker Tray. LotProcessNum =%d "), m_nLotProcessNum ) ;
				//2017.0116
				if( st_handler_info.mn_removetray == CTL_CHANGE )
				{
					//kwlee 2017.0126
					//st_handler_info.mn_removetray = NO;
					strMsg.Format(_T("[TRAY REMOVE] Unloading #1 Good Stacker Tray. LotProcessNum =%d "), m_nLotProcessNum ) ;
				}
				clsMem.OnAbNormalMessagWrite(strMsg);//�α� ����
			}
			//////

			/*
			if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_ULD_1_STACKER] ==  LOT_END 
				&& st_lot_info[m_nLotProcessNum].nLot_THD_Status[st_lot_info[m_nLotProcessNum].nStacker_Reject_THD_Num]  ==  LOT_END )
			{
				/////////////////////////////////////////////////////
				//��ü lot end �޼��� ���
				/////////////////////////////////////////////////////
				if (st_handler_info.cWndMain != NULL)
				{
					st_other_info.nBuzYesNo = YES;
					st_other_info.strBoxMsg = _T("[LOT END] eject on Unloading #1 Good, Reject Stacker Tray. \r\n Please Remove it.");
					st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
				}
			}*/
			

			for (x=0; x<st_recipe_info.nTrayX; x++)
			{
				for (y=0; y<st_recipe_info.nTrayY; y++)
				{
					if(st_tray_info[THD_ULD_1_STACKER].st_pcb_info[y][x].nYesNo == CTL_YES)
					{
						st_tray_info[THD_ULD_1_STACKER].st_pcb_info[y][x].nYesNo = CTL_NO;
					}
				}
			}

			if (st_tray_info[THD_ULD_1_STACKER].nTrayExist == CTL_YES)
			{
				st_tray_info[THD_ULD_1_STACKER].nTrayExist = CTL_NO;
			}
			Set_GoodConveyorLeftStopper_UpDown(IO_ON); //james 2016.0930 

			m_nRunStep = 0;
			break;

		default: 
			break;
	}
}


void CRunUldGoodTrayLeftOutputElevator::Set_GoodStackerLeftTray_UpDown( int nOnOff )
{
	CString strLogKey[10];
	CString	strLogData[10];

	strLogKey[0] = _T("Mode Start");
	strLogData[0].Format(_T("%d"),0);

	m_bStackerLeftUpDown = true;
	m_dwWaitStackerLeftUpDown[0] = GetCurrentTime();

	FAS_IO.set_out_bit( st_io_info.o_Uld1StkUpCyl, nOnOff);
	FAS_IO.set_out_bit( st_io_info.o_Uld1StkDnCyl, !nOnOff);

	//kwlee 2016.0812
	if (nOnOff == IO_ON)
	{
		//LogFunction(_T("ULD_TRAY_LEFT_ELEVATOR"),_T("UP"),_T("STACKER"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("ULD_TRAY_LEFT_ELEVATOR"),_T("UP"),0,_T("STACKER"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
	else
	{
		//LogFunction(_T("ULD_TRAY_LEFT_ELEVATOR"),_T("DOWN"),_T("STACKER"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("ULD_TRAY_LEFT_ELEVATOR"),_T("DOWN"),0,_T("STACKER"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
}

int CRunUldGoodTrayLeftOutputElevator::Chk_GoodStackerLeftTray_UpDown( int nOnOff )
{
	CString strLogKey[10];
	CString	strLogData[10];

	strLogKey[0] = _T("Mode End");
	strLogData[0].Format(_T("%d"),0);

//	800308 1 0 "UNLOADING_1_STACKER_UP_CHECK"
//	800309 1 0 "UNLOADING_1_STACKER_DOWN_CHECK"
	int nWaitTime = WAIT_STACKER_UP_DN;

	if (nOnOff == IO_OFF)
	{
		if (m_bStackerLeftUpDown == false &&
			FAS_IO.get_in_bit(st_io_info.i_Uld1StkUpChk,	IO_OFF)	== IO_OFF &&
			FAS_IO.get_in_bit(st_io_info.i_Uld1StkDnChk,	IO_ON)	== IO_ON)
		{
			m_bStackerLeftUpDown		= true;
			m_dwWaitStackerLeftUpDown[0]	= GetCurrentTime();
		}
		else if (m_bStackerLeftUpDown == true &&
				 FAS_IO.get_in_bit(st_io_info.i_Uld1StkUpChk,	IO_OFF)	== IO_OFF &&
				 FAS_IO.get_in_bit(st_io_info.i_Uld1StkDnChk,	IO_ON)	== IO_ON)
		{
			m_dwWaitStackerLeftUpDown[1] = GetCurrentTime();
			m_dwWaitStackerLeftUpDown[2] = m_dwWaitStackerLeftUpDown[1] - m_dwWaitStackerLeftUpDown[0];

			if (m_dwWaitStackerLeftUpDown[2] <= 0)
			{
				m_dwWaitStackerLeftUpDown[0] = GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwWaitStackerLeftUpDown[2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
			{
				//LogFunction(_T("ULD_TRAY_LEFT_ELEVATOR"),_T("DOWN"),_T("STACKER"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
				//2017.019
				clsLog.LogFunction(_T("ULD_TRAY_LEFT_ELEVATOR"),_T("DOWN"),1,_T("STACKER"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_GOOD;
			}
		}
		else
		{
			m_dwWaitStackerLeftUpDown[1] = GetCurrentTime();
			m_dwWaitStackerLeftUpDown[2] = m_dwWaitStackerLeftUpDown[1] - m_dwWaitStackerLeftUpDown[0];

			if (m_dwWaitStackerLeftUpDown[2] <= 0)
			{
				m_dwWaitStackerLeftUpDown[0] = GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwWaitStackerLeftUpDown[2] > (DWORD)st_wait_info.nLimitWaitTime[nWaitTime])
			{
				m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_Uld1StkDnChk); 
				//2017.019
				clsLog.LogFunction(_T("ULD_TRAY_LEFT_ELEVATOR"),_T("DOWN"),1,_T("STACKER"),_T("CYLINDER"),1,strLogKey,strLogData);				return RET_ERROR;
			}
		}
	}
	else
	{
		if (m_bStackerLeftUpDown == false &&
			FAS_IO.get_in_bit(st_io_info.i_Uld1StkUpChk,	IO_ON)	== IO_ON &&
			FAS_IO.get_in_bit(st_io_info.i_Uld1StkDnChk,	IO_OFF)	== IO_OFF)
		{
			m_bStackerLeftUpDown			= true;
			m_dwWaitStackerLeftUpDown[0]	= GetCurrentTime();
		}
		else if (m_bStackerLeftUpDown == true &&
				 FAS_IO.get_in_bit(st_io_info.i_Uld1StkUpChk,	IO_ON)	== IO_ON &&
				 FAS_IO.get_in_bit(st_io_info.i_Uld1StkDnChk,	IO_OFF)	== IO_OFF)
		{
			m_dwWaitStackerLeftUpDown[1]	= GetCurrentTime();
			m_dwWaitStackerLeftUpDown[2]	= m_dwWaitStackerLeftUpDown[1] - m_dwWaitStackerLeftUpDown[0];

			if (m_dwWaitStackerLeftUpDown[2] <= 0)
			{
				m_dwWaitStackerLeftUpDown[0]	= GetCurrentTime();
				return RET_PROCEED;
			}

			if(m_dwWaitStackerLeftUpDown[2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
			{
				//LogFunction(_T("ULD_TRAY_LEFT_ELEVATOR"),_T("UP"),_T("STACKER"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("ULD_TRAY_LEFT_ELEVATOR"),_T("UP"),1,_T("STACKER"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_GOOD;
			}
		}
		else
		{
			m_dwWaitStackerLeftUpDown[1]	= GetCurrentTime();
			m_dwWaitStackerLeftUpDown[2]	= m_dwWaitStackerLeftUpDown[1] - m_dwWaitStackerLeftUpDown[0];

			if (m_dwWaitStackerLeftUpDown[2] <= 0)
			{
				m_dwWaitStackerLeftUpDown[0]	= GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwWaitStackerLeftUpDown[2] > (DWORD)st_wait_info.nLimitWaitTime[nWaitTime])
			{
				m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_Uld1StkUpChk); 
				//2017.0109
				clsLog.LogFunction(_T("ULD_TRAY_LEFT_ELEVATOR"),_T("UP"),1,_T("STACKER"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_ERROR;
			}
		}
	}

	return RET_PROCEED;
}


void CRunUldGoodTrayLeftOutputElevator::Set_GoodConveyorLeftStopper_UpDown( int nOnOff )
{
	CString strLogKey[10];
	CString	strLogData[10];

	strLogKey[0] = _T("Mode Start");
	strLogData[0].Format(_T("%d"),0);


	m_bStopperUpDownFlag = true;
	m_dwWaitStopperUpDown[0] = GetCurrentTime();

	FAS_IO.set_out_bit( st_io_info.o_ReadyUld1CvyStopCyl, nOnOff);

	///kwlee 2016.0812
	if (nOnOff == IO_ON)
	{
		//LogFunction(_T("ULD_TRAY_LEFT_CONVEYOR"),_T("UP"),_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("ULD_TRAY_LEFT_CONVEYOR"),_T("UP"),0,_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
	else
	{
		//LogFunction(_T("ULD_TRAY_LEFT_CONVEYOR"),_T("DOWN"),_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("ULD_TRAY_LEFT_CONVEYOR"),_T("DOWN"),0,_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
}

int CRunUldGoodTrayLeftOutputElevator::Chk_GoodConveyorLeftStopper_UpDown( int nOnOff )
{
	CString strLogKey[10];
	CString	strLogData[10];

	strLogKey[0] = _T("Mode End");
	strLogData[0].Format(_T("%d"),0);

//	800308 1 0 "UNLOADING_1_STACKER_UP_CHECK"
//	800309 1 0 "UNLOADING_1_STACKER_DOWN_CHECK"
	int nWaitTime = WAIT_STACKER_UP_DN;

	if (nOnOff == IO_OFF)
	{
		if (m_bStopperUpDownFlag == false &&
			FAS_IO.get_in_bit(st_io_info.i_ReadyUld1CvyStopUpChk,	IO_ON)	== IO_ON &&
			FAS_IO.get_in_bit(st_io_info.i_ReadyUld1CvyStopDnChk,	IO_OFF)	== IO_OFF)
		{
			m_bStopperUpDownFlag		= true;
			m_dwWaitStopperUpDown[0]	= GetCurrentTime();
		}
		else if (m_bStopperUpDownFlag == true &&
				 FAS_IO.get_in_bit(st_io_info.i_ReadyUld1CvyStopUpChk,	IO_ON)	== IO_ON &&
				 FAS_IO.get_in_bit(st_io_info.i_ReadyUld1CvyStopDnChk,	IO_OFF)	== IO_OFF)
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
				//LogFunction(_T("ULD_TRAY_LEFT_CONVEYOR"),_T("DOWN"),_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("ULD_TRAY_LEFT_CONVEYOR"),_T("DOWN"),1,_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
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
				m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_ReadyUld1CvyStopUpChk); 
				//2017.0109
				clsLog.LogFunction(_T("ULD_TRAY_LEFT_CONVEYOR"),_T("DOWN"),1,_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_ERROR;
			}
		}
	}
	else
	{
		if (m_bStopperUpDownFlag == false &&
			FAS_IO.get_in_bit(st_io_info.i_ReadyUld1CvyStopUpChk,	IO_OFF)	== IO_OFF &&
			FAS_IO.get_in_bit(st_io_info.i_ReadyUld1CvyStopDnChk,	IO_ON)	== IO_ON)
		{
			m_bStopperUpDownFlag			= true;
			m_dwWaitStopperUpDown[0]	= GetCurrentTime();
		}
		else if (m_bStopperUpDownFlag == true &&
				 FAS_IO.get_in_bit(st_io_info.i_ReadyUld1CvyStopUpChk,	IO_OFF)	== IO_OFF &&
				 FAS_IO.get_in_bit(st_io_info.i_ReadyUld1CvyStopDnChk,	IO_ON)	== IO_ON)
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
				//LogFunction(_T("ULD_TRAY_LEFT_CONVEYOR"),_T("UP"),_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("ULD_TRAY_LEFT_CONVEYOR"),_T("UP"),1,_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData); 
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
				m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_ReadyUld1CvyStopDnChk); 
				//2017.0109
				clsLog.LogFunction(_T("ULD_TRAY_LEFT_CONVEYOR"),_T("UP"),1,_T("STOPPER"),_T("CYLINDER"),1,strLogKey,strLogData); 
				return RET_ERROR;
			}
		}
	}

	return RET_PROCEED;
}