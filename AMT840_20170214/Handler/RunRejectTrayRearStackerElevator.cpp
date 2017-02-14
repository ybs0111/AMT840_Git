// RejectTrayRearStackerElevator.cpp : ���� �����Դϴ�.
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


// CRunRejectTrayRearStackerElevator ��� �Լ�
void CRunRejectTrayRearStackerElevator::OnThreadRun(void)
{
	if (st_handler_info.nSystemLock != FALSE)  return;	// ���� �ý��� Lock ������ ��� ����

	CTL_Lib.Process_Tray_Conveyor_AcMotor(0, THD_REJECT_OUT_2_STACKER, st_io_info.o_Rej2UldCvyStkAcMotOnOff ); // elevator stacker AC Motor �����ϴ� ��ƾ	


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

	if (st_sync_info.nInit_Flag[THD_REJECT_OUT_2_STACKER] != INIT_CLEAR)		return;		//INIT_CLEAR �϶��� �ʱ�ȭ �۾��� �Ѵ�. �ʱ�ȭ�� ������ INIT_CLEAR -> INIT_READY�� �Ǳ� ������...

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
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Rej2StkTrayExistChk,	IO_OFF); //stacker tary �� ó������ ����� �Ѵ�  
			if(nRet_1 == IO_OFF)
			{
				m_nInitStep = 300;
			}
			else
			{//Ʈ���� ���� ���� 
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
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Rej2StkTrayExistChk,	IO_OFF); //stacker�� Ʈ���̰� �ִ�, ������ ������ �Ѵ�   
			if(nRet_1 == IO_OFF)
			{
				m_nInitStep = 410;
			}
			else
			{//Ʈ���� �������� �ʴ´�  
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
			//kwlee 2016.0902 Right Cv ��� ��������.
			st_sync_info.nConveyor_AcMotorCtl_Req[THD_REJECT_OUT_2_STACKER] = CTL_REQ; //cv rail ac motor �����Ͽ� ������ Ʈ���̸� �� ����  
			//st_sync_info.nConveyor_AcMotorCtl_Req[THD_ULD_2_STACKER] = CTL_REQ; //cv rail ac motor �����Ͽ� ������ Ʈ���̸� �� ����  
			 
			m_bCVTrayFlag = false;
			m_nInitStep = 430;			 
			break;

		case 430:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Rej1StkTrayExistChk,	IO_OFF); //stacker tary �� ó������ ����� �Ѵ�  
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_Rej2UldCvyTrayExistChk,	IO_OFF); //stacker tary �� ó������ ����� �Ѵ�  
			if(nRet_1 == IO_OFF && nRet_2 == IO_OFF)
			{
				//kwlee 2016.0902 Right Cv ��� ��������.
				//st_sync_info.nConveyor_AcMotorCtl_Req[THD_REJECT_OUT_1_STACKER] = CTL_READY; 
				st_sync_info.nConveyor_AcMotorCtl_Req[THD_REJECT_OUT_2_STACKER] = CTL_READY; 
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

						//������ �߻������� AC Motor�� ��� ���� ��Ų�� 
						//kwlee 2016.0902 
						//st_sync_info.nConveyor_AcMotorCtl_Req[THD_ULD_1_STACKER] = CTL_FREE;  
						st_sync_info.nConveyor_AcMotorCtl_Req[THD_REJECT_OUT_2_STACKER] = CTL_FREE;
						m_bCVTrayFlag = false; //ó������ �ٽð� �� �õ� 
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
				if(st_lot_info[m_nLotProcessNum].nStacker_Reject_THD_Num == THD_REJECT_OUT_2_STACKER)//�ش� ����Ŀ�� ���õ������� �������� 
				{
					st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_REJECT_OUT_2_STACKER] =  LOT_START;
					m_nRunStep = 110;
				}
			}
			else if(st_lot_info[LOT_NEXT].nLotStatus == LOT_START)
			{
				m_nLotProcessNum = LOT_NEXT;
				if(st_lot_info[m_nLotProcessNum].nStacker_Reject_THD_Num == THD_REJECT_OUT_2_STACKER) //�ش� ����Ŀ�� ���õ������� �������� 
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
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Rej2StkTrayExistChk,	IO_OFF); //stacker tary �� ó������ ����� �Ѵ�  
			if(nRet_1 == IO_OFF)
			{
				m_nRunStep = 310; //james 2016.0923 400;
			}
			else
			{//Ʈ���� ���� ���� 
				m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_Rej2StkTrayExistChk);  
				CTL_Lib.Alarm_Error_Occurrence(7120, dWARNING, m_strAlarmCode);
			}
			break;

		case 310:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Rej2UldCvyTrayExistChk,	IO_OFF); //stacker tary �� ó������ ����� �Ѵ�  
			if(nRet_1 == IO_OFF)
			{
				m_nRunStep = 400;
			}
			else
			{//Ʈ���� ���� ���� 
				m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_Rej2UldCvyTrayExistChk);  
				CTL_Lib.Alarm_Error_Occurrence(7130, dWARNING, m_strAlarmCode);
			}
			break;

		case 400:
			nRet_1 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 	
			if(nRet_1 == BD_GOOD) //�δ� �÷���Ʈ�� Ʈ���̰� ���� �� ���� 
			{
				m_strAlarmCode.Format(_T("900013")); //900013 1 0 "REJECT_2_STACKER_PLATE_SD_TRAY_ON_CHECK_ERROR"
				CTL_Lib.Alarm_Error_Occurrence(7140, dWARNING, m_strAlarmCode); 
			}
			else 
			{
				m_nRunStep = 500;
			}
			break;

		case 500://�̸� �޴� ��ġ�� �̵��Ѵ� 
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Rej2UldCvyTrayExistChk,	IO_ON); //james 2016.0926 
			if(nRet_1 == IO_ON)
			{
				COMI.Set_MotStop(MOT_STOP_DEC, m_nAxisNum); //��������   // ���� �̵� ���� �Լ� 
				m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Rej2UldCvyTrayExistChk); 
				CTL_Lib.Alarm_Error_Occurrence(7150, dWARNING, m_strAlarmCode);
				break;
			}

			nRet_1 = CTL_Lib.Elevator_Job_Move_Pos(0, m_nAxisNum,  P_ELV_RECEIVE_OFFSET); //Ʈ���̸� Ʈ�����ۿ��� �޴´� 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep =600;
			}
			break;

		case 600:
			nRet_1 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 	
			if(nRet_1 == BD_GOOD) //�δ� �÷���Ʈ�� Ʈ���̰� ���� �� ���� 
			{
				m_strAlarmCode.Format(_T("900013")); //900013 1 0 "REJECT_2_STACKER_PLATE_SD_TRAY_ON_CHECK_ERROR"
				CTL_Lib.Alarm_Error_Occurrence(7160, dWARNING, m_strAlarmCode);
			}
			else if(nRet_1 == BD_ERROR)//�δ� �÷��̵忡 Ʈ���̰� ���� �������� ���� 
			{
				m_nRunStep = 900;
			}
			break;

		case 900: //
			st_tray_info[THD_REJECT_OUT_2_STACKER].nTrayExist = CTL_NO;//2016.0923 
			m_nRunStep = 1000;
			break;


		//////////////////////////////////////////////////////////////////////////////
		// Work robot�� LOT �� Ȯ���Ͽ� �۾��� ��ε� ����Ŀ�� �����Ͽ� ��û�Ѵ� 
		//1. LOT ���۽� unload stacker ������ LOT ������ �Է��Ͽ� ���� ����ϰ� �Ѵ� 
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
			if(st_sync_info.nWorkRbt_Dvc_Req[THD_REJECT_OUT_2_STACKER][0] == CTL_REQ) //��ũ �κ����� uld 1 stacker�� Ʈ���� ������ ��û �ߴ� 
			{
				if(st_tray_info[THD_REJECT_OUT_2_STACKER].nTrayExist == CTL_YES) //Ʈ���� ������ �ִ� ���� 
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
							m_strAlarmCode.Format(_T("900013")); //900013 1 0 "REJECT_2_STACKER_PLATE_SD_TRAY_ON_CHECK_ERROR"
							CTL_Lib.Alarm_Error_Occurrence(7180, dWARNING, m_strAlarmCode);	
						}
						m_nRunStep = 1200;
					} 
				}
				else  //Ʈ���̰� ������ 
				{
					nRet_1 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 	
					if(nRet_1 == BD_ERROR) //�÷���Ʈ�� Ʈ���̰� ���� �ȵ� ���� 
					{
						m_nRunStep = 3000;
					}
					else if(nRet_1 == BD_ERROR) //�÷���Ʈ�� Ʈ���̰� ���� �ȵ� ���� 
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
				if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] ==  LOT_END) //���̻� �۾��� ���簡 ���� , lot end ����, �̰����� ��ü lot�� �����Ѵ�
				{				
					nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Rej2StkTrayExistChk,	IO_ON); //elevator stacker tary �� �־�� �Ѵ� 
					if(st_count_info.nUnLdStacker_Reject_TrayCnt[1] == 0 && nRet_1 == IO_OFF)
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
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Rej2UldCvyTrayExistChk,	IO_ON); //james 2016.0926 
			if(nRet_1 == IO_ON)
			{
				COMI.Set_MotStop(MOT_STOP_DEC, m_nAxisNum); //��������   // ���� �̵� ���� �Լ� 
				m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Rej2UldCvyTrayExistChk); 
				CTL_Lib.Alarm_Error_Occurrence(7200, dWARNING, m_strAlarmCode);
				break;
			}

			nRet_1 = CTL_Lib.Elevator_Job_Move_Pos(0, m_nAxisNum,  P_ELV_SUPPLY_OFFSET); //Ʈ���̸� Ʈ�����ۿ��� �޴´� 
			
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 1100;
			}
			break;

		case 1300:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Rej2UldCvyTrayExistChk,	IO_ON); //james 2016.0926 
			if(nRet_1 == IO_ON)
			{
				COMI.Set_MotStop(MOT_STOP_DEC, m_nAxisNum); //��������   // ���� �̵� ���� �Լ� 
				m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Rej2UldCvyTrayExistChk); 
				CTL_Lib.Alarm_Error_Occurrence(7210, dWARNING, m_strAlarmCode);
				break;
			}

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
			nRet_1 = clsFunc.Find_TrayWork_Pos(0, THD_REJECT_OUT_2_STACKER, CTL_NO, m_npFindWorkPosYXCPB, THD_WORK_RBT, st_worker_info.strLotNo_Empty);
			if(nRet_1 == RET_GOOD) //�۾� ������ ���簡 �ִ�
			{				
				///////////////////////////////////////////////////////
				// Ʈ���� ��ġ �ٽ� Ȯ�� �� ���� ����  
				///////////////////////////////////////////////////////

				nRet_1 = COMI.Get_MotIOSensor(m_nAxisNum, MOT_SENS_SD); 	
				if(nRet_1 == BD_GOOD)
				{
					st_sync_info.nWorkRbt_Dvc_Req[THD_REJECT_OUT_2_STACKER][0] = CTL_READY; //���� Ʈ������ �۾��� �����ϴ� 
					m_nRunStep = 1000;
				}
				else //if(nRet_1 == BD_ERROR) //�÷���Ʈ�� Ʈ���̰� ���� �ȵ� ���� 
				{
					m_strAlarmCode.Format(_T("900014")); //900014 1 0 "REJECT_2_STACKER_PLATE_SD_TRAY_OFF_CHECK_ERROR"
					CTL_Lib.Alarm_Error_Occurrence(7220, dWARNING, m_strAlarmCode);						
				} 
			}
			else if(nRet_1 == RET_SKIP) //���̻� ����̽��� ���� ������ ���� 
			{	//�� Ʈ���̸� �����ϰ�, �ٽ� �޾ƾ� �Ѵ� 

				///////////////////////////////////////////////////////
				// Ʈ���̸� �� �����ϰ� �ٽ� Ʈ���̸� ���� �޾ƾ� �Ѵ� 
				///////////////////////////////////////////////////////
				m_nRunStep = 3000;
			}
			break;		

		///////////////////////////////////////////////////////
		// Ʈ���̰� ������, Ʈ���̸� ���� �޾ƾ� �Ѵ� 
		///////////////////////////////////////////////////////
		case 3000:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Rej2UldCvyTrayExistChk,	IO_ON); //james 2016.0926 
			if(nRet_1 == IO_ON)
			{
				COMI.Set_MotStop(MOT_STOP_DEC, m_nAxisNum); //��������   // ���� �̵� ���� �Լ� 
				m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Rej2UldCvyTrayExistChk); 
				CTL_Lib.Alarm_Error_Occurrence(7230, dWARNING, m_strAlarmCode);
				break;
			}

			nRet_1 = CTL_Lib.Elevator_Job_Move_Pos(0, m_nAxisNum,  P_ELV_RECEIVE_OFFSET); //Ʈ���̸� Ʈ�����ۿ��� �޴´� 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 3100;
			}
			break;

		case 3100:
			if(st_tray_info[THD_REJECT_OUT_2_STACKER].nTrayExist == CTL_YES) //Ʈ���̰� ������, �۾��� ������ ���� ���·� �̹� �ѹ� �۾��� �Ϸ��ϴ�Ʈ�����̴�. Ʈ���� ������ �ִ� ���� 
			{
				st_tray_info[THD_REJECT_OUT_2_STACKER].nTrayExist = CTL_NO; //tray ���� Ŭ���� 

				st_lot_info[m_nLotProcessNum].nUnLdTrayCnt++; //2015.0225 james �ش� lot�� Ʈ���� ī��Ʈ ���� ���� 

				//st_count_info.nUnldStackerTray++; //Ʈ���� ����Ŀ Ʈ���� ���� ���� �ϳ� up 
				st_count_info.nUnLdStacker_Reject_TrayCnt[1]++; //unload good 1 stacker tray ���� ����

				if(st_count_info.nUnLdStacker_Reject_TrayCnt[1] >= st_basic_info.nUldGoodTrayStack_Count)//���� ������ ���� ������ �����Ѵ�  
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

		// ���ο� Ʈ���̸� ��û�Ѵ� 
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
					// Ʈ���� ������ ��� ���� ����  
					///////////////////////////////////////////////////////
					st_tray_info[THD_REJECT_OUT_2_STACKER].nTrayExist = CTL_YES;//2016.0912 
					st_sync_info.nRejectTransfer_Req[THD_REJECT_OUT_2_STACKER][0] = CTL_CLEAR;
					m_nRunStep = 3400;
				}
				else //if(nRet_1 == BD_ERROR) //�÷���Ʈ�� Ʈ���̰� ���� �ȵ� ���� 
				{
					m_strAlarmCode.Format(_T("900013")); //900013 1 0 "REJECT_2_STACKER_PLATE_SD_TRAY_ON_CHECK_ERROR."
					CTL_Lib.Alarm_Error_Occurrence(7340, dWARNING, m_strAlarmCode);						
				} 
			}
			else
			{
				//james 2016.0926 
				if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] ==  LOT_END) //���̻� �۾��� ���簡 ���� , lot end ����, �̰����� ��ü lot�� �����Ѵ�
				{				
					nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Rej2StkTrayExistChk,	IO_ON); //elevator stacker tary �� �־�� �Ѵ� 
					if(st_count_info.nUnLdStacker_Reject_TrayCnt[1] == 0 && nRet_1 == IO_OFF)
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

		case 3400:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Rej2UldCvyTrayExistChk,	IO_ON); //james 2016.0926 
			if(nRet_1 == IO_ON)
			{
				COMI.Set_MotStop(MOT_STOP_DEC, m_nAxisNum); //��������   // ���� �̵� ���� �Լ� 
				m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Rej2UldCvyTrayExistChk); 
				CTL_Lib.Alarm_Error_Occurrence(7250, dWARNING, m_strAlarmCode);
				break;
			}

			///////////////////////////////////////////////////////
			// Ʈ���̸� �κ��� ���� �� �ִ� ��ġ�� ����  
			///////////////////////////////////////////////////////
			nRet_1 = CTL_Lib.Elevator_Job_Move_Pos(0, m_nAxisNum,  P_ELV_SUPPLY_OFFSET); //Ʈ���̸� Ʈ�����ۿ��� �޴´� 
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
			else //if(nRet_1 == BD_ERROR) //�÷���Ʈ�� Ʈ���̰� ���� �ȵ� ���� 
			{
				m_strAlarmCode.Format(_T("900013")); //900013 1 0 "REJECT_2_STACKER_PLATE_SD_TRAY_ON_CHECK_ERROR."
				CTL_Lib.Alarm_Error_Occurrence(7260, dWARNING, m_strAlarmCode);						
			} 
			break;

		case 3600:
			m_nRunStep = 1000;
			break;
			

		////////////////////////////////////////////////////////////////
		// stacker�� ���� ����� Ʈ���̸� ������ 
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
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Rej2StkTrayExistChk,	IO_ON); //elevator stacker tary �� �־�� �Ѵ� 
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_Rej2UldCvyTrayExistChk,	IO_OFF); //tary�� �߰��� �����Ǹ� �ȵȴ�  
			if(nRet_1 == IO_ON && nRet_2 == IO_OFF)
			{				
				Set_ConveyorStopper_UpDown(IO_OFF); //up
				st_sync_info.nConveyor_AcMotorCtl_Req[THD_REJECT_OUT_2_STACKER] = CTL_FREE;//Ȥ�� ac motor�� �������̸� ��� ������ ��Ų�� 
				m_nRunStep = 7100;
			}
			else
			{//Ʈ����  ���� 
				if(nRet_1 == IO_ON) m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Rej2StkTrayExistChk); 
				else  m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_Rej2UldCvyTrayExistChk); 
				CTL_Lib.Alarm_Error_Occurrence(7280, dWARNING, m_strAlarmCode);
			}
			break;

		case 7100:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Rej2UldCvyTrayExistChk,	IO_ON); //james 2016.0926 
			if(nRet_1 == IO_ON)
			{
				COMI.Set_MotStop(MOT_STOP_DEC, m_nAxisNum); //��������   // ���� �̵� ���� �Լ� 
				m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Rej2UldCvyTrayExistChk); 
				CTL_Lib.Alarm_Error_Occurrence(7290, dWARNING, m_strAlarmCode);
				break;
			}

			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nAxisNum, st_motor_info[m_nAxisNum].d_pos[P_ELV_TRAY_INOUT_POS], COMI.mn_runspeed_rate);  //2015.0407 james  //SD ��ġ���� �̵��ߴµ� SD�� �������� ������ Ʈ���̰� ���°��̴�
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
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Rej2StkTrayExistChk,	IO_ON); //elevator stacker tary �� �־�� �Ѵ� 
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_Rej2UldCvyTrayExistChk,	IO_OFF); //tary�� �߰��� �����Ǹ� �ȵȴ�  
			if(nRet_1 == IO_ON && nRet_2 == IO_OFF )
			{				
				m_nRunStep = 8000;
			}
			else
			{//Ʈ����  ���� 
				if(nRet_1 == IO_ON) m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Rej2StkTrayExistChk); 
				else if(nRet_2 == IO_ON) m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_Rej2UldCvyTrayExistChk);  
				CTL_Lib.Alarm_Error_Occurrence(7320, dWARNING, m_strAlarmCode);
			}
			break;


		///////////////////////////////////////////////////////////
		// �۾��ڰ� ���� ��ư�� �۵���Ű�� ������ �� ���̴� 
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
				//load cv & stacker ac motor �� ���ÿ� ���� �������� ���ÿ� �����ϰ� �Ѵ� 
				st_sync_info.nConveyor_AcMotorCtl_Req[THD_REJECT_OUT_2_STACKER] = CTL_READY;  			 

				m_bCVTrayFlag = false;
				m_nRunStep = 8200;
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
							m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_Rej2StkTrayExistChk); 
							CTL_Lib.Alarm_Error_Occurrence(7330, dWARNING, m_strAlarmCode);
						}
						else if(nRet_2 == IO_OFF)
						{
							m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_Rej2UldCvyTrayExistChk); 
							CTL_Lib.Alarm_Error_Occurrence(7340, dWARNING, m_strAlarmCode);
						}

						//������ �߻������� AC Motor�� ��� ���� ��Ų�� 
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
				// �޼����� ����Ͽ� �۾��ڿ��� �˸��� 
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
		//LOT ENd�� Ʈ���� ���� 
		////////////////////////////////////////////
		case 9000:

			/*

			if(st_lot_info[m_nLotProcessNum].strLotNo == st_tray_info[THD_REJECT_OUT_1_STACKER].strLotNo && st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_REJECT_OUT_1_STACKER] ==  LOT_END )
			{
				/////////////////////////////////////////////////////
				//��ü lot end �޼��� ���
				/////////////////////////////////////////////////////

			}
			else if(st_lot_info[m_nLotProcessNum].strLotNo == st_tray_info[THD_REJECT_OUT_2_STACKER].strLotNo && st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_REJECT_OUT_2_STACKER] ==  LOT_END)
			{
				/////////////////////////////////////////////////////
				//��ü lot end �޼��� ���
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
			if (st_handler_info.cWndList != NULL)  // ����Ʈ �� ȭ�� ����
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
