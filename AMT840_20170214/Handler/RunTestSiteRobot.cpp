// RunTestSiteRobot.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Handler.h"
#include "RunTestSiteRobot.h"
#include "Variable.h"
#include "FastechPublic_IO.h"
#include "CtlBd_Library.h"

#include "CtlBd_Variable.h"
#include "PublicFunction.h"
#include "ComizoaPublic.h"
#include "LogCtrl.h"
//kwlee 2017.0905
#include "XgemClient.h"

#include "RunDvcLdBuffer.h"
#include "RunDvcUldBuffer.h"
#include "LogFromat.h"
#include "AlgMemory.h"

#ifdef _WIN64
#include ".\\64bit\\Cmmsdk.h"
#include ".\\64bit\\CmmsdkDef.h"
#else
#include ".\\32bit\\Cmmsdk.h"
#include ".\\32bit\\CmmsdkDef.h"
#endif


// CRunTestSiteRobot
CRunTestSiteRobot clsRunTestSiteRobot;
CRunTestSiteRobot::CRunTestSiteRobot()
{
	m_nRunStep	= -1;
	m_nInitStep	= -1;
	
	m_nPickerPara = TEST_PICKER_PARA;//1
	m_nRobotSite = THD_TEST_RBT;

	//linear move ���� ����
	m_nLinearMove_Index	= 3;   //���� map index
	m_lAxisCnt	= 2;            //������ IndexNum���� ��� ������ ���� ���� �ִ� 4��
	m_lpAxisNum[0] = M_TEST_RBT_Y; //m_lpAxisNum[4];        //������ IndexNum���� ����ϴ� ���� ���� ��ȣ�� ������
	m_lpAxisNum[1] = M_TEST_RBT_X;
	m_lpAxisNum[2] = M_TEST_RBT_Z;
	m_lpAxisNum[3] = M_TEST_DVC_EJECT_Z;

	m_nRobot_X = M_TEST_RBT_X;
	m_nRobot_Y = M_TEST_RBT_Y;
	m_nRobot_Z = M_TEST_RBT_Z; 
	m_nRobot_Ejector = M_TEST_DVC_EJECT_Z;

	m_dpTargetPosList[0]  = 0; //m_dpTargetPosList[4];		   //�ִ� 4 ����Ʈ�� �ѹ��� �̵������ϴ�
	m_dpTargetPosList[1]  = 0;
	m_dpTargetPosList[2]  = 0;

 	m_dpSpdRatio[0] = 100; 		//	m_dpSpdRatio[3];        //�ӵ����� [0]:Vel, [1]:Acc, [2]:Dec
	m_dpSpdRatio[1] = 100;
	m_dpSpdRatio[2] = 100;


	// jtkim 20160831
	st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_FREE;
	for (int i=0; i<8; i++)
	{
		st_sync_info.nTestRbt_Dvc_Req[THD_TESTSITE_1 + 1][0] = CTL_FREE;
	}

	st_worker_info.strLotNo_Empty = "0"; //2016.0909
}

CRunTestSiteRobot::~CRunTestSiteRobot()
{
}

void CRunTestSiteRobot::OnThreadRun()
{
	int nRet_1 = 0;



	m_nRetestBuffer_PickPlace_Mode = 1; //james 2016.0929 

	if (st_handler_info.nSystemLock != FALSE)  return;	// ���� �ý��� Lock ������ ��� ����  
	

	switch(st_handler_info.nRunStatus)
	{
		case dINIT:
			OnRunInit();
			break;

		case dRUN:
			m_nRobot_Z_Stop_Flag = 0;

			OnRunMove();					
			break;

		case dSTOP:
			if(m_nRobot_Z_Stop_Flag == 0) //james 2016.1004 
			{
				nRet_1 = COMI.Check_Motion_State(M_TEST_RBT_Z, cmMST_STOP);//20150427 nRet_1 = cmmSxIsDone(n_MotorNum, &dwMotionDone);
				if (nRet_1 != BD_GOOD) 
				{//���� ���°� mMST_STOP �̴� 
					if(CTL_Lib.m_nStep_Single[M_TEST_RBT_Z] > 0)
					{
						COMI.Set_MotStop(1, M_TEST_RBT_Z) ; //������� 
						CTL_Lib.m_nStep_Single[M_TEST_RBT_Z] = 0;
						m_nRobot_Z_Stop_Flag = 100; 
					}
					else
					{
						m_nRobot_Z_Stop_Flag = 1;
					}					
				}
				else
				{
					m_nRobot_Z_Stop_Flag = 1;
				}
			}

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

void CRunTestSiteRobot::OnRunInit()
{
	int nRet_1, nRet_2, i;
	int nFlag = 0;

	if (st_sync_info.nInit_Flag[THD_TEST_RBT] != INIT_CLEAR)		return;		//INIT_CLEAR �϶��� �ʱ�ȭ �۾��� �Ѵ�. �ʱ�ȭ�� ������ INIT_CLEAR -> INIT_READY�� �Ǳ� ������...
	 
	switch (m_nInitStep)
	{
		case -1:		 

			m_nInitStep = 0;
			break;

		case 0:		

			m_nInitStep = 100;
			break;

		case 100:  
			nRet_1 = COMI.HomeCheck_Mot(m_nRobot_Z, MOT_SENS_ELM, MOT_TIMEOUT) ; //
			if(nRet_1 == BD_GOOD)
			{
				COMI.mn_homechk_flag[m_nRobot_Z] = CTL_YES;
				m_nInitStep = 200;
			}
			else if(nRet_1 == BD_ERROR)
			{
				CTL_Lib.mn_run_status = CTL_dWARNING;
				CTL_Lib.Alarm_Error_Occurrence(2000, CTL_Lib.mn_run_status, COMI.strCode);
				m_nInitStep = 100;
			}			
			break;

		case 200:  
			nRet_1 = COMI.HomeCheck_Mot(m_nRobot_Ejector, MOT_SENS_ELM, MOT_TIMEOUT) ; //
			if(nRet_1 == BD_GOOD)
			{
				COMI.mn_homechk_flag[m_nRobot_Ejector] = CTL_YES;
				m_nInitStep = 300;
			}
			else if(nRet_1 == BD_ERROR)
			{
				CTL_Lib.mn_run_status = CTL_dWARNING;
				CTL_Lib.Alarm_Error_Occurrence(2001, CTL_Lib.mn_run_status, COMI.strCode);
				m_nInitStep = 200;
			}			
			break;


		case 300:  
			nRet_1 = COMI.HomeCheck_Mot(m_nRobot_X, MOT_SENS_ELM, MOT_TIMEOUT) ; //
			if(nRet_1 == BD_GOOD)
			{
				COMI.mn_homechk_flag[m_nRobot_X] = CTL_YES;
				m_nInitStep = 400;
			}
			else if(nRet_1 == BD_ERROR)
			{
				CTL_Lib.mn_run_status = CTL_dWARNING;
				CTL_Lib.Alarm_Error_Occurrence(2002, CTL_Lib.mn_run_status, COMI.strCode);
				m_nInitStep = 300;
			}			
			break;

		case 400:  
			nRet_1 = COMI.HomeCheck_Mot(m_nRobot_Y, MOT_SENS_ELM, MOT_TIMEOUT) ; //
			if(nRet_1 == BD_GOOD)
			{
				COMI.mn_homechk_flag[m_nRobot_Y] = CTL_YES;
				m_nInitStep = 500;
			}
			else if(nRet_1 == BD_ERROR)
			{
				CTL_Lib.mn_run_status = CTL_dWARNING;
				CTL_Lib.Alarm_Error_Occurrence(2003, CTL_Lib.mn_run_status, COMI.strCode);
				m_nInitStep = 400;
			}			
			break;		

		case 500://james 2016.0827 �׸��� ����̽� üũ ���� ����/����, Gripper Off(Ŭ��������)���� IO_OFF�� ����̽� �ִ»���, �׿ܴ� ON/OFF ������� ��� ����̽� ���� ����
			 nFlag = 0;
			for(i=0; i<TEST_PICKER_PARA; i++)
			{
				m_npPicker_YesNo[i] = CTL_YES;				
				st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo = CTL_NO; //clear

				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_TestRbtPickerGripDvcExistChk[i],	IO_OFF);
				nRet_2 = FAS_IO.get_out_bit(st_io_info.o_TestRbtPickerGripCyl[i],	IO_OFF);
				if(nRet_1 == IO_ON) //james 2016.1003  && nRet_2 == IO_OFF) 
				{
					nFlag++;
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_TestRbtPickerGripDvcExistChk[i]); 
					CTL_Lib.Alarm_Error_Occurrence(2004, dWARNING, m_strAlarmCode);						 			
				}
				else
				{
					
				}
			}
			if(nFlag > 0) break;

			m_nInitStep = 600;
			break;

		case 600:
			Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_OFF);
			m_nInitStep = 610;
			break;

		case 610:
			nRet_1 = Chk_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nInitStep = 700;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(2005, dWARNING, m_strAlarmCode);
				m_nInitStep = 600;
			}
			break;

		case 700:
			m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_SAFETY];
			m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_SAFETY];
			nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate);
			if(nRet_1 == BD_GOOD) //���������� �Ϸ�� ����
			{
				m_nInitStep = 900;
			}
			else if(nRet_1 == BD_ERROR)
			{
				m_nInitStep = 700;
				CTL_Lib.Alarm_Error_Occurrence(2005, dWARNING, st_alarm_info.strCode);
			}
			break;

		case 900:
			st_sync_info.nInit_Flag[THD_TEST_RBT] = INIT_READY; 
			 
			m_nInitStep = 0;
			break;	

		default :
			break;

	}
}


void CRunTestSiteRobot::OnRunMove()
{
	int nRet_1, nRet_2=0, nRet_3=0;
	int nFlag = 0, i, j, x;
	int nTHD_i=0;
	int nTemp_Test_Pick_THD_Site, nTemp_Test_Place_THD_Site ,nTemp_Test_THD_WorkSite;
	int nGoodReject_Cnt = 0, nRetest_Cnt = 0;
	int nTHD_ID=0, nFixPos = 0, nPos =0;
	int nMaxCnt=0, nCount=0,nRetestExistCnt=0, nSiteYesCnt = 0;
	int nTestSite[10] = {0,};
	int nFind_JobPos = 0, nUseSocket = 0, nUseLotSocket = 0;
	int nDvcCount = 0, nSiteExistCnt = 0, nNumSok = 0;
	CString strFindLotNo,strTemp;
	bool nSkip = false;
	int nEnableSokPerSite[TEST_SOCKET_PARA] = {0,};//����Ʈ�� ���ϻ�� ���ɰ���
	bool nNeedOneMoreSite = false;

	clsFunc.OnThreadFunctionStepTrace(24, m_nRunStep);	

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

		case 100://��Ŀ ���¸� �ľ��Ѵ�			
			nFlag = 0;
			for(i=0; i<TEST_PICKER_PARA; i++)
			{
				m_npPicker_YesNo[i] = CTL_YES;
				st_picker[m_nRobotSite].st_pcb_info[i].nYesNo = CTL_NO;

				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_TestRbtPickerGripDvcExistChk[i],	IO_OFF);
				nRet_2 = FAS_IO.get_out_bit(st_io_info.o_TestRbtPickerGripCyl[i],	IO_OFF);
				if(nRet_1 == IO_ON)// james 2016.1003  && nRet_2 == IO_OFF) 
				{
					nFlag++;
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_TestRbtPickerGripDvcExistChk[i]); 
					CTL_Lib.Alarm_Error_Occurrence(2100, dWARNING, m_strAlarmCode);	
					break;					
				}
			}		

			if( nFlag == 0)
			{  //���縦 ���� ���� �ʴ� 
				m_nRunStep = 200;
			} 
			break; 

		case 200:
			Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON);
			m_nRunStep = 210;
			break;

		case 210:
			nRet_1 = Chk_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 300;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(2101, dWARNING, m_strAlarmCode);
				m_nRunStep = 200;
			}
			break;

		case 300:
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY], COMI.mn_runspeed_rate);
			if (nRet_1 == BD_GOOD) //�������� �̵�
			{
				m_nRunStep = 400;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nRunStep = 300;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
				CTL_Lib.Alarm_Error_Occurrence(2110, dWARNING, st_alarm_info.strCode);
				m_nRunStep = 300;
			}
			break;

		case 400:
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Ejector, st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY], COMI.mn_runspeed_rate);
			if (nRet_1 == BD_GOOD) //�������� �̵�
			{
				m_nRunStep = 500;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nRunStep = 400;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
				CTL_Lib.Alarm_Error_Occurrence(2111, dWARNING, st_alarm_info.strCode);
				m_nRunStep = 400;
			}
			break;

		case 500:
			m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_SAFETY];
			m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_SAFETY];
			nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate);
			if(nRet_1 == BD_GOOD) //���������� �Ϸ�� ����
			{
				m_nRunStep = 600;
			}
			else if(nRet_1 == BD_ERROR)
			{
				m_nRunStep = 500;
				CTL_Lib.Alarm_Error_Occurrence(2112, dWARNING, st_alarm_info.strCode);
			}
			break;

		case 600:
			if(st_lot_info[LOT_CURR].nLotStatus == LOT_START || st_lot_info[LOT_NEXT].nLotStatus == LOT_START )
			{
				m_nRunStep = 900;
			}
			break;

		case 900:
			m_nFindLotNo_Flag = 0;
			m_nRunStep = 1000;
			break;

 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//�׽�Ʈ�κ� ���� �÷ο� Ȯ�� 
//** Test robot ��Ŀ����(���鿡�� ���� 1 ~ 4)�� �°� ��ü ����Ʈ ��ġ�� ������ ���Ѵ�. ���� ���ϵ� ������ �����ʴ� �κ��� Map Exchange�� ���� �ذ��Ѵ�
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*		
1. �׽��� �κ��� ������ �׽��͸� Ȯ�� �� �� �׽��͸� �������� ���������� �°� �ε� ���ۿ� ���� ������ ��û�Ѵ� 
2. �׽�Ʈ�� ���� �׽��Ͱ� ������ ��ε� �� �ٷ� �ε��ϸ�, �׽��Ͱ� ��������� �ε��� �Ѵ�
3. ��ε� ������ GOOD, Reject ���縦 ��ε� ���ۿ� ���� �� ���׽�Ʈ ����� ���׽�Ʈ ���ۿ� ���´�
4. ���׽�Ʈ ���� ������ �׽��Ϳ� ���� ���簡 ��Ŀ�� ���׽�Ʈ ���۸� �������� ������ �ε����纸�ٴ� ���׽�Ʈ ���縦 ���� ���� �׽�Ʈ ����Ʈ�� ����/���´� 
5. ���Ͽ����� �ε� ���ۿ� ���簡 ���������� �׽�Ʈ ����Ʈ��ġ�� ���� ���� ��, �Ǵ� ������ ������ �����Ȱ�,�ֵ������ ���� �ε� ���ۿ� �߰� ������ ��û�� �� �ִ�.
6.  ���׽�Ʈ ����� 4�� �̻� �Ϸķ� �迭�Ǿ������� ó�� �� �� �ִ� 
 */
		case 1000:
			if(st_lot_info[LOT_CURR].nLotStatus == LOT_CLEAR && st_lot_info[LOT_NEXT].nLotStatus == LOT_CLEAR )
			{
				m_nRunStep = 0;
				break;
			}

			nTemp_Test_Pick_THD_Site = 0; //Ŭ���� 
			nTemp_Test_Place_THD_Site = 0; //Ŭ���� 
			nTemp_Test_THD_WorkSite = 0; 
			m_nTestSite_THD_Work_Site = 0; //clear 

			for(i = 0; i < TEST_PICKER_PARA; i++) st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable = CTL_YES;//��Ŀ ��� ������� ���� 
			
			//////////////////////////////////////////////////////////////////////////////////////////////////////////
			//���� lot�� �����ϰ� �۾��� lot ������ LOT_CURR�� LOT_NEXT LotNo�� �ݺ������� �����Ͽ� Lot ������ ã�´� 
			//////////////////////////////////////////////////////////////////////////////////////////////////////////
			if(m_nFindLotNo_Flag == 0)
			{
				m_nFindLotNo_Flag = 1;

				if(st_lot_info[LOT_CURR].nLotStatus >= LOT_START) //james 2016.0924 �߰� 
				{
					m_strFindLotNo = st_lot_info[LOT_CURR].strLotNo;
					m_nLotProcessNum = LOT_CURR; 	
				}
				else
				{
					break;
				}	 
			}
			else //if(m_nFindLotNo_Flag == 1)
			{		
				m_nFindLotNo_Flag = 0;				
				
				// ���� LOT �۾��� ���� üũ ��� 
				//james 2016.0924 if(st_lot_info[LOT_NEXT].nLdTrayCnt > 0 && st_lot_info[LOT_NEXT].nLotStatus >= LOT_START) 
				if(st_lot_info[LOT_NEXT].nLotStatus >= LOT_START) 
				{
					m_strFindLotNo = st_lot_info[LOT_NEXT].strLotNo;
					m_nLotProcessNum = LOT_NEXT; 		
				}
				else
				{//���� lot �� ���������� �������� ���� lot No �� ���ϰ� break; �Ѵ� 
					break;
				}
			}//���� lot ���� Ȯ�� 

			if(st_lot_info[m_nLotProcessNum].strLotNo == "") //lot ������ ������ ���� 
			{
				break;
			}
			else if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_TEST_RBT] ==  LOT_END) //james 2016.0909 
			{
				break;
			}
			//next lot �׽�Ʈ����Ʈ�� LotNo �� ã�� ������ �����ϰ�, ������ break �Ѵ� 
			nCount = 0; nSiteYesCnt = 0;	
			for(i = THD_TESTSITE_1; i <= THD_TESTSITE_8; i++) //james 206.0930
			{
				if(st_lot_info[m_nLotProcessNum].strLotNo ==  st_test_site_info[i].strLotNo)
				{
					nCount++;
				}
			}
			if(nCount == 0)//��Ƽ���϶� ���� ���� ���� ���
			{			
				//2016.1107
 				if(st_lot_info[LOT_CURR].nLot_THD_Status[THD_TEST_RBT] >=  LOT_END_START && st_lot_info[LOT_NEXT].nLotStatus >= LOT_START) //���� lot�� lot end �������̰�, ó�� �ε� ���簡 ���̻� ������ �׽�Ʈ ����Ʈ�� Next Lot�� ���� Test Site #1 ~#4 ������ ��� ����� ������ Nect Lot ������ �����Ѵ� 
 				{ 
					//if( nNextLotPlaceSite == true )
					if( clsFunc.CheckCurrLotEndNextLot( nNeedOneMoreSite ) == RET_GOOD )
					{
 						for( int nSiteNum = THD_TESTSITE_1; nSiteNum <= THD_TESTSITE_8; nSiteNum++) 
 						{//�ش� �׽�Ʈ ����Ʈ�� ���簡 ��� ������

 							nCount = 0;
							nRet_2 = 0;
 							for(i = 0; i < TEST_SOCKET_PARA; i++) //x ���� ���� ���� ����(8��, 0~7��)�� �����Ѵ� 
 							{
 								if(st_lot_info[LOT_CURR].strLotNo ==  st_test_site_info[nSiteNum].strLotNo
 									&& st_test_site_info[nSiteNum].st_pcb_info[i].nYesNo == CTL_YES)
 								{
 									nCount++;//���� ���� ����̽��� ���Ͽ� �����ִ� ����
 								}
								if(st_lot_info[LOT_CURR].strLotNo ==  st_test_site_info[nSiteNum].strLotNo &&
									st_test_site_info[nSiteNum].st_pcb_info[i].nEnable == YES)
								{
									nRet_2++; 
								}
							}
							if( nCount == 0 && nRet_2 > 0)
							{
								//���׽�Ʈ���簡 �־ �������� �ϳ��� ����Ʈ�� �� ������ �� �ʿ䰡 ���� �� ����Ʈ �ϳ��� �������� ���� ���´�.
								if( nNeedOneMoreSite == true)
								{
									nNeedOneMoreSite = false;
									continue;
								}
								st_test_site_info[nSiteNum].strLotNo = st_lot_info[LOT_NEXT].strLotNo; //test site�� lot������ LOT_NEXT�� ���� �����Ͽ� LOT_NEXT�� �����Ѵ� 
 								st_test_site_info[nSiteNum].strPartNo = st_lot_info[LOT_NEXT].strPartNo;
 								st_lot_info[LOT_NEXT].nLotNoApply_FirstTestStart = LOT_START;
								//strTemp.Format(_T("[LOT_NEXT] TestSite : %d Lot : %s start"), nSiteNum, st_test_site_info[nSiteNum].strLotNo);
								//kwlee 2017.0202
								strTemp.Format(_T("[LOT_NEXT] TestSite : %d Lot : %s start, Step : %d"), st_sync_info.nTestSite_Target_THD_Work_Site[1], st_lot_info[LOT_NEXT].strLotNo,m_nRunStep);
								clsMem.OnNormalMessageWrite(strTemp);	
							}
						}
 					}
				}
				//break;
			}
			 
			////////////////////////////////////////////////////////////////////////////////////
			//TEST END PICK ���� ã��
			///////////////////////////////////////////////////////////////////////////////////
			//Test site �� ������ Ȯ���Ͽ� �׽�Ʈ�� �������� ������ WORK_PICK �۾��� �����ϴ� 
			//�׽�Ʈ ���Ͽ��� �׽�Ʈ�� ���� ���� �� �ִ� ���簡 ������ ���´�, ��ε� ����.
			////////////////////////////////////////////////////////////////////////////////////	
			nRet_1 = clsFunc.Find_TestSite_Work_Pos(0, m_strFindLotNo, WORK_PICK, m_nSyncTestCount, m_npGetSiteInfo); //m_nSyncTestCount : �ѹ��� ���� �۾��� �׽�Ʈ ����Ʈ ����(���� 4para)		
			if(nRet_1 == RET_GOOD) 
			{				
				nFlag = 0; //ó�� ���õ� ����Ʈ Ȯ��
				for(nTHD_i = THD_TESTSITE_1; nTHD_i <= THD_TESTSITE_8; nTHD_i++)
				{
					if(st_find_testsite_info[nTHD_i][1].nRun_Mode == WORK_PICK && st_find_testsite_info[nTHD_i][1].nTotal_Cnt > 0) //1 �׽�Ʈ�� ���� ��ε��� ������ ����Ʈ ���� 
					{
						if(st_sync_info.nTestSite_Target_THD_Work_Site[1] == nTHD_i) //�۾��ϴ� ���� �׽�Ʈ ����Ʈ�� �� �۾��� �� �ִ� ���·�, �װ��� �̾ �۾��ؾ� �Ѵ� 
						{
							nFlag++; 
							nTemp_Test_Pick_THD_Site = nTHD_i; //�۾��ϴ� ����Ʈ�� �ش� �׽��� ����Ʈ�� �̾ �۾��Ѵ� 
							break;
						}

						if(nFlag == 0)
						{
							nTemp_Test_Pick_THD_Site = nTHD_i;
						}
						nFlag++; 
					}					
					//nTestSite[1] = st_find_testsite_info[st_sync_info.nTestSite_Target_THD_Work_Site].nSite_Pos; //left or right �������Ʈ���� �۾��ϰ� �Դ��� ���� ���� 	
					//nTestSite[2] = st_find_testsite_info[st_sync_info.nTestSite_Target_THD_Work_Site].nStart_FixPos;
				} //for

				//nTestSite[0] = nTemp_Test_Pick_THD_Site;
				/* james load & unload ������ ��� Ȯ�� �� �ؿ��� �ѹ��� ó�� 
				if(nTemp_Test_Pick_THD_Site >= THD_TESTSITE_1) //THD_TESTSITE_1 ~ THD_TESTSITE_8 ���� �۾��� �׽�Ʈ ����Ʈ ��ġ ���� 
				{
					m_nTestSite_THD_Work_Site = nTemp_Test_Pick_THD_Site;
					m_nRunStep = 1100;
					break;
				}*/
			}

			///////////////////////////////////////////////////////////////////////////////////
			// TEST SITE PLACE ���� ã��
			//////////////////////////////////////////////////////////////////////////////////
			// test site�� ���縦 �ε��� WORK_PLACE �۾� ������ ã��
			//�׽�Ʈ ����Ʈ�� ������ ����ִ� ���·�, �ε� ���縦 ���� �����Ѵ� 
			////////////////////////////////////////////////////////////////////////////////////	
			nFlag = 0; //ó�� ���õ� ����Ʈ Ȯ��
			nRet_1 = clsFunc.Find_TestSite_Work_Pos(0, m_strFindLotNo, WORK_PLACE, m_nSyncTestCount, m_npGetSiteInfo); //m_nSyncTestCount : �ѹ��� ���� �۾��� �׽�Ʈ ����Ʈ ����(���� 4para)
			if(nRet_1 == RET_GOOD) 
			{				
				nFlag = 0; //ó�� ���õ� ����Ʈ Ȯ��
				for(nTHD_i = THD_TESTSITE_1; nTHD_i <= THD_TESTSITE_8; nTHD_i++)
				{
					if(st_find_testsite_info[nTHD_i][2].nRun_Mode == WORK_PLACE && st_find_testsite_info[nTHD_i][2].nTotal_Cnt > 0) //1 �׽�Ʈ�� ���� ��ε��� ������ ����Ʈ ���� 
					{
						if(st_sync_info.nTestSite_Target_THD_Work_Site[2] == nTHD_i &&
							( st_test_site_info[nTHD_i].nTestBdStart == BD_END || st_test_site_info[nTHD_i].nTestBdStart == BD_NONE)) //�۾��ϴ� ���� �׽�Ʈ ����Ʈ�� �� �۾��� �� �ִ� ���·�, �װ��� �̾ �۾��ؾ� �Ѵ� 
						{
							nFlag++; //2016.0911
							nTemp_Test_Place_THD_Site = nTHD_i; //�۾��ϴ� ����Ʈ�� �ش� �׽��� ����Ʈ�� �̾ �۾��Ѵ� 
							break; 
						}

						if(nFlag == 0)
						{
							if( st_test_site_info[nTHD_i].nTestBdStart == BD_END || st_test_site_info[nTHD_i].nTestBdStart == BD_NONE )
							{
								nTemp_Test_Place_THD_Site = nTHD_i;
							}
						}
						nFlag++; 
					}
				} //for

				/* james load & unload ������ ��� Ȯ�� �� �ؿ��� �ѹ��� ó�� 
				if(nTemp_Test_Place_THD_Site >= THD_TESTSITE_1) //THD_TESTSITE_1 ~ THD_TESTSITE_8 ���� �۾��� �׽�Ʈ ����Ʈ ��ġ ���� 
				{
					m_nTestSite_THD_Work_Site = nTemp_Test_Place_THD_Site;
					m_nRunStep = 1200;
					break;
				}*/			 
			}
			
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//james 2016.0909 �߰�, 
			// lot end �����, THD_WORK_RBT�� LOT_END_START �ߴٴ� �̾߱�� �ε� ���ۿ��� ���簡 ������ �ִٴ� �̾߱��̴� 
			// �� �ε� ���ۿ� ���縦 ���� �� THD_WORK_RBT�� LOT_END_START �� �����Ѵ�
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		
			nFlag = 0;
			if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END_START && st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_TEST_RBT] < LOT_END_START) //�̹� lot end�� �������̰�, �ε�/���׽�Ʈ�� ���簡 ������ �ε��� ���̻� ���� 
			{	//Ȥ�� ���ۿ� ���簡 ������ ó���ؾ� �� 
				nRet_1 = clsFunc.Find_TrayWork_Pos(WORK_PICK, THD_LD_BUFF, CTL_YES, m_npFindWorkPosYXCPB, THD_TEST_RBT, m_strFindLotNo);		 			
				if(nRet_1 == RET_GOOD && m_npFindWorkPosYXCPB[2] > 0) //�ε� ���ۿ� ���̻� ���簡 ������ test robot�� ���̻� �ε� ����� ó������ ������ �̶�  THD_TEST_RBT�� LOT_END_START �� �����Ѵ� 
				{		
					nFlag++;
					//ó���� �ε� ���簡 �ִ� , //�ε� ���ۿ����簡 ���� �ִ� ���� 
				}
				else if(m_npFindWorkPosYXCPB[2] == 0)
				{
					if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_TEST_RBT] < LOT_END_START) //james s2016.0911 1600
					{
						st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_TEST_RBT] =  LOT_END_START; //���� ���̻� �ε� ���ۿ� �ش� lot�� ����� ���� 
					}			 
				}	
			}

			if(nFlag == 0 && nTemp_Test_Place_THD_Site > 0 && st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_TEST_RBT] >=  LOT_END_START)
			{
				//�ε��� �׽�Ʈ����Ʈ�� �ְ�, �׽�Ʈ �κ��� lot end�� ����������, �̶����ʹ� �ε� ����� ���׽�Ʈ ���縸 ó�� �����ϴ� 
				
				st_sync_info.nRetestBuff_Traget_THD_Work_Site  = st_lot_info[m_nLotProcessNum].nBuffer_Retest_THD_Num;
				if(st_lot_info[m_nLotProcessNum].strLotNo != st_buffer_info[st_sync_info.nRetestBuff_Traget_THD_Work_Site].strLotNo) 
				{//�۾��� lot ������ ���� �۾��� ���׽�Ʈ ����Ʈ�� lot ������ Ʋ���� �������� ���� 
					//james 2016.0918 test������ ���� st_test_site_info[st_sync_info.nRetestBuff_Traget_THD_Work_Site].strLotNo = st_lot_info[m_nLotProcessNum].strLotNo;
					break;
				} 

				///////////////////////////////////
				//test site ���� ���� Ȯ�� 
				//////////////////////////////////
				nCount = 0;
				for(nTHD_i = THD_TESTSITE_1; nTHD_i <= THD_TESTSITE_8; nTHD_i++)
				{
					if(st_test_site_info[nTHD_i].strLotNo == m_strFindLotNo) //WORK_PICK �� �� �ִ� ���簡 ������ �� ���縦 ������ 
					{
						for(x = 0; x < TEST_SOCKET_PARA; x++) //x ���� ���� ���� ����(8��, 0~7��)�� �����Ѵ� 
						{
							if(st_test_site_info[nTHD_i].strLotNo == m_strFindLotNo && st_test_site_info[nTHD_i].st_pcb_info[x].nYesNo == CTL_YES)
							{
								nCount++;
							}
						}
					}
				}  
				////////////////////////////
				//���׽�Ʈ ���۸� ã�´�
				////////////////////////////
				nRet_1 = clsFunc.Find_TrayWork_Pos(0, st_sync_info.nRetestBuff_Traget_THD_Work_Site, CTL_YES, m_npFindWorkPosYXCPB, THD_TEST_RBT, m_strFindLotNo);
				if(nRet_1 == RET_GOOD) //���׽�Ʈ ���ۿ� �۾� ������ ���簡 �ִ�
				{
					//nCount == 0 //test site���� ��ε��� ���簡 ����, //test site���� ���̻� ��ε��� ���簡 ����. ���� ���׽�Ʈ ���ۿ� ���� ���縸 ó���ϸ� �ȴ� 
					//2016.1023
					if( /*m_npFindWorkPosYXCPB[2] >= 7*/m_npFindWorkPosYXCPB[2] >= 4 || m_nSort_Count == 1000 || nCount == 0) //���׽�Ʈ ������ ���簡 1�� �̻��̰ų�, ���̻� �׽�Ʈ ����Ʈ�� �ش� lot�� ���簡 �������� ������ ���׽�Ʈ ���縦 �����Ѵ� 
					{
						m_nSort_Count = 0; //james 2016.0918 �߰� 
						/* james 2016.1003 �۾��� ��û���� ���� = CTL_REQ
						//TEST SITE �۾� ��ġȮ�� 
						st_sync_info.nTestSite_Target_THD_Work_Site[2]									= nTemp_Test_Place_THD_Site; //m_nTestSite_THD_Work_Site; //�۾��� �׽�Ʈ ����Ʈ ���� ���� 						 
						st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2] = st_find_testsite_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2].nSite_Pos; //left/right
						st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3] = st_find_testsite_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2].nStart_FixPos;//fix start pos
						st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][4] = st_sync_info.nTestSite_Target_THD_Work_Site[2];//test site THD num(18~25)
						*/
						//////////////////////////////////////
						//james 2016.103 ���� ����
						//TEST SITE �۾� ��ġȮ�� 
						//////////////////////////////////////
						st_sync_info.nTestSite_Target_THD_Work_Site[2]									    = nTemp_Test_Place_THD_Site; //�۾��� �׽�Ʈ ����Ʈ ���� ���� 
						st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2] = st_find_testsite_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2].nSite_Pos; //left/right
						st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3] = st_find_testsite_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2].nStart_FixPos;//fix start pos
						st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][4] = st_sync_info.nTestSite_Target_THD_Work_Site[2];//test site THD num(18~25)

						if (st_sync_info.nTestSite_Target_THD_Work_Site[2] >= THD_TESTSITE_1 && st_sync_info.nTestSite_Target_THD_Work_Site[2] <= THD_TESTSITE_8)
						{
							if (st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][0] == CTL_FREE || st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][0] == CTL_CLEAR)
							{
								st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][0] = CTL_REQ;
								st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][1] = WORK_PLACE;
							}
						}
						else
						{//���� ��
							break;
						}

						//retest buffer �۾� ��ġ Ȯ�� 
						st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][0] = CTL_REQ;
						st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][1] = WORK_PICK; //���縦 ���� �� �ְ� ��û 
						st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][2] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2];//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos;					 
						st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][3] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3];
						st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][4] = st_sync_info.nTestSite_Target_THD_Work_Site[2];//test site num					 

						nTHD_ID = st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][4];
						nFixPos = st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][3];

						bool bETest = false;
						//����� ��Ŀ ���� ���� 
						for(i=0; i< 4; i++) //retest ���� ���� 4�� ���� Ȯ��  
						{
							if(nTHD_ID >= THD_TESTSITE_1 && nTHD_ID <= THD_TESTSITE_4) //���� �׽��� ,���� ������ ������ġ ���� (1ȸ�۾�)7,6,5,4, (2ȸ�۾�)3,2,1,0 �� ������ ���� ���� ��ġ�� ���� ���ǵ�
							{//���� �׽�Ʈ ����Ʈ 
								nPos = (7 - nFixPos) - i;
								if(st_test_site_info[nTHD_ID].st_pcb_info[nPos].nEnable == CTL_YES) //socket ON
								{								 
									st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable = CTL_YES;
									bETest = true;
								}
								else //if(st_test_site_info[nTHD_ID].st_pcb_info[nPos].nEnable  == CTL_NO)//Socket OFF
								{								 
									st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable = CTL_NO;
								}
							}
							else if(nTHD_ID >= THD_TESTSITE_5 && nTHD_ID <= THD_TESTSITE_8) //���� �׽��� , ���� ������ ������ġ ����  (1ȸ�۾�)0,1,2,3, (2ȸ�۾�)4,5,6,7 �� ������ ���� ���� ��ġ�� ���� ���ǵ�
							{//���� �׽�Ʈ ����Ʈ 
								nPos = nFixPos + i;
								if(st_test_site_info[nTHD_ID].st_pcb_info[nPos].nEnable  == CTL_YES) //socket ON
								{								 
									st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable = CTL_YES;
									bETest = true;
								}
								else //if(st_test_site_info[nTHD_ID].st_pcb_info[nPos].nEnable == CTL_NO)//Socket OFF
								{								 
									st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable = CTL_NO;
								}
							}  
						}//for

						//m_nRunStep = 5000;//retest buffer������ ���� �׽�Ʈ ����Ʈ�� �μ�Ʈ�Ѵ�
						//strTemp.Format(_T("[RETEST BUFFER1] %d"), m_nRunStep);
						//clsMem.OnNormalMessageWrite(strTemp);
						//break;	
						//2017.0224
						////////////////////////////////////////////////////////////////////////////////////////////////////////
						if(bETest == true )
						{
							m_nRunStep = 5000;//retest buffer������ ���� �׽�Ʈ ����Ʈ�� �μ�Ʈ�Ѵ�
							strTemp.Format(_T("[RETEST BUFFER1] %d"), m_nRunStep);
							clsMem.OnNormalMessageWrite(strTemp);
							break;
						}
//						nRet_1 = clsFunc.Find_RetestBuffer_PickPlace_WorkPos_Check(WORK_PICK, nTHD_ID, nFixPos, st_sync_info.nRetestBuff_Traget_THD_Work_Site, m_npPicker_YesNo, m_npFindWorkPosYXCPB, m_np_BuffInfo_Count);
//						if(nRet_1 == RET_GOOD)
//						{
//							m_nRunStep = 5000;//retest buffer������ ���� �׽�Ʈ ����Ʈ�� �μ�Ʈ�Ѵ�
//							strTemp.Format(_T("[RETEST BUFFER1] %d"), m_nRunStep);
//							clsMem.OnNormalMessageWrite(strTemp);
//						}
// 						if(nRet_1 == RET_ABORT)
// 						{
// 							nCount = 0;
// 							for(i = 0; i < TEST_PICKER_PARA; i++)//st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == n_DvcYesNO && (n_DvcYesNO == CTL_YES && (n_DvcYesNO == CTL_YES || (n_PickerYesNO == CTL_NO && st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable == CTL_YES)) ) )
// 							{
// 								if(st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == CTL_YES)
// 								{
// 									nCount++;
// 								}
// 							}
// 
// 							if( nCount > 0 )
// 							{
// 								m_nRunStep = 5000;//retest buffer������ ���� �׽�Ʈ ����Ʈ�� �μ�Ʈ�Ѵ�
// 								strTemp.Format(_T("[RETEST BUFFER1] %d"), m_nRunStep);
// 								clsMem.OnNormalMessageWrite(strTemp);
// 								break; 
// 							}
// 						}
					}//if( m_npFindWorkPosYXCPB[2] >= 7 || nCoun == 0)
				}//if
				else
				{// //���׽�Ʈ ���ۿ� �۾� ������ ���簡 ����
					if(nCount == 0) //test site���� ��ε��� ���簡 ����
					{			
						if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_TEST_RBT] <= LOT_END_START) //james s2016.0911 1600
						{
							st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_TEST_RBT] =  LOT_END;
							/////
							break;
						}							  
					}
				}
			}//if(nFlag == 0 && nTemp_Test_Place_THD_Site > 0 && st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_TEST_RBT] >=  LOT_END_START)			 
			////////////// LOT_END_START �� retest ���� ó�� ���� 

			/////////////////////////////////////////////////////////////////
			//�׽��Ϳ� �ε� �Ǵ� ��ε� �� ������� ���� �۾����� �����Ѵ�
			/////////////////////////////////////////////////////////////////
			if(nTemp_Test_Pick_THD_Site > 0 && nTemp_Test_Place_THD_Site > 0) 
			{//�׽�Ʈ ���Ͽ� �ε� & ��ε��� ��� �����ϸ� �ε����� �۾� �Ѵ�
				// jtkim 20160902
				m_nTestSite_THD_Work_Site = nTemp_Test_Place_THD_Site;

				m_nRunStep = 1200; //�ε� ���ۿ��� ���� �׽�Ʈ ����Ʈ�� �ִ´� 

				if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_TEST_RBT] ==  LOT_END_START) //james 2016.0911 �ε� ����� ���̻� ������ �׽�Ʈ�� ������ ����Ʈ�� ������ �� ����Ʈ�� ���縦 ó������ 
				{
					m_nTestSite_THD_Work_Site	= nTemp_Test_Pick_THD_Site;
					m_nRunStep = 1100;// TEST SITE ���� PICK ����  ��ε� ���� ���� ���� ���� 
				}

				//2017.0319
				//strTemp.Format(_T("[1000_PICK_PLACE] Work_Site : %d Togo RunStep : %d"), m_nTestSite_THD_Work_Site, m_nRunStep);
				//clsMem.OnNormalMessageWrite(strTemp);

				/*
				if (nTemp_Test_Pick_THD_Site == nTemp_Test_Place_THD_Site)
				{
					nTemp_Test_Pick_THD_Site	= nTestSite[0];
					m_nTestSite_THD_Work_Site	= nTemp_Test_Pick_THD_Site;

					m_nRunStep = 1100;// TEST SITE ���� PICK ����  ��ε� ���� ���� ���� ���� 
				}
				else
				{
					m_nTestSite_THD_Work_Site = nTemp_Test_Place_THD_Site;
					
					m_nRunStep = 1200;
				}*/
				break;
			}
			else if(nTemp_Test_Place_THD_Site > 0)
			{//�׽�Ʈ ����Ʈ�� ������ ������ �ִ°��� ���� �켱 �۾� �����̴� 
				if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_TEST_RBT] >=  LOT_END_START) //james 2016.0918 �ε� ����� ���̻� ������ �׽�Ʈ�� ������ ����Ʈ�� ������ �� ����Ʈ�� ���縦 ó������ 
				{
					nRet_1 = clsFunc.Find_TrayWork_Pos(0, st_sync_info.nRetestBuff_Traget_THD_Work_Site, CTL_YES, m_npFindWorkPosYXCPB, THD_TEST_RBT, m_strFindLotNo);

// 					if(nRet_1 == RET_GOOD && m_npFindWorkPosYXCPB[2] > 0)
// 					{
//						//2016.1107
// 						m_nSort_Count = 1000;
// 						break;//james 2016.0926 
// 					}
					//2017.0302
					//LOTEND_START�϶� ���׽�Ʈ ���簡 1���� ������ ������ ������ ���鼭 ���� LOSS�� �߻��ϴ� �� �� ������ ��������
					//Ȥ�� ���� ����( ���� ������ �������� ����Ʈ�� 2�� ���϶��
					nCount = 0;

					for(nTHD_i = THD_TESTSITE_1; nTHD_i <= THD_TESTSITE_8; nTHD_i++)
					{
						nSiteYesCnt = 0;
						//���� ��밡���� ����Ʈ�� �ϳ��� �־�� �Ѵ� ���� disable �� �� �����Ƿ�
						for(i = 0; i < TEST_SOCKET_PARA; i++)
						{
							if(	st_test_site_info[nTHD_i].st_pcb_info[i].nEnable == YES )
							{
								nSiteYesCnt++;
								break;
							}
						}
						if(st_test_site_info[nTHD_i].strLotNo == m_strFindLotNo && nSiteYesCnt > 0 ) //WORK_PICK �� �� �ִ� ���簡 ������ �� ���縦 ������ 
						{
							nCount++;
						}
					}  

					//2017.0312
					//1. ��Ƽ ���ΰ�� ( m_npFindWorkPosYXCPB[2] > 0  && nCount <=3 ) ��Ƽ ���ΰ��
					//2. ���� �ϳ��϶� m_npFindWorkPosYXCPB[2] > 0
					if(st_lot_info[LOT_CURR].nLot_THD_Status[THD_TEST_RBT] >= LOT_END_START && st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)
					{
						if( nRet_1 == RET_GOOD && ( m_npFindWorkPosYXCPB[2] >= 4 || ( m_npFindWorkPosYXCPB[2] > 0  && nCount <=3 )  ) )
						{
							//2017.0319
							strTemp.Format(_T("[1000_PLACE_2LOTRETEST4] Count : %d "), nCount);
							clsMem.OnNormalMessageWrite(strTemp);
							m_nSort_Count = 1000;
							break;
						}
					}//2017.0325
// 					else if(st_lot_info[LOT_CURR].nLot_THD_Status[THD_TEST_RBT] >= LOT_END_START && st_lot_info[LOT_NEXT].nLotStatus >= LOT_END_START)
// 					{
// 						if( m_nLotProcessNum == LOT_NEXT && nRet_1 == RET_GOOD && m_npFindWorkPosYXCPB[2] > 0 )
// 						{ 							
// 							strTemp.Format(_T("[1000_PLACE_NEXTLOTRETEST] Count : %d "), nCount);
// 							clsMem.OnNormalMessageWrite(strTemp);
// 							m_nSort_Count = 1000;
// 							break;
// 						}
// 					}
					else
					{
						if( nRet_1 == RET_GOOD &&  m_npFindWorkPosYXCPB[2] > 0  )  
						{
							//2017.0319
							strTemp.Format(_T("[1000_PLACE_1LOTRETEST] Count : %d "), nCount);
							clsMem.OnNormalMessageWrite(strTemp);
							m_nSort_Count = 1000;
							break;
						}
					}

					//james 2016.0926 break;
				}

				m_nTestSite_THD_Work_Site = nTemp_Test_Place_THD_Site;
				m_nRunStep = 1200;//�ε� ���ۿ��� ���� �׽�Ʈ ����Ʈ�� �ִ´� 

				//2017.0319
				//strTemp.Format(_T("[1000_PLACE] Work_Site : %d "), m_nTestSite_THD_Work_Site);
				//clsMem.OnNormalMessageWrite(strTemp);
				break;
			}
			else if(nTemp_Test_Pick_THD_Site > 0)
			{ //�ε��� ���� ����, �׽�Ʈ�� ���� ��ε��� ������ �ִٸ� �� ������ �۾����� 
				//nTemp_Test_Pick_THD_Site	= nTestSite[0];
				m_nTestSite_THD_Work_Site	= nTemp_Test_Pick_THD_Site;
				m_nRunStep = 1100;// TEST SITE ���� PICK ����  ��ε� ���� ���� ���� ���� 
				//2017.0319
				strTemp.Format(_T("[1000_PICK] Work_Site : %d "), m_nTestSite_THD_Work_Site);
				clsMem.OnNormalMessageWrite(strTemp);
				break;
			}
			else
			{
				//�۾��� �׽�Ʈ ����Ʈ�� ����, üũ�� �ݺ��ϸ�, �۾��� ������ ã�´� 
			}
			break;

		///////////////////////////////////////////////////////////////////////////////////////
		//TEST SITE PICK
		//�׽�Ʈ ��ε� ����
		// TEST SITE ���� PICK ����  ��ε� ���� ���� ���� ���� 
		///////////////////////////////////////////////////////////////////////////////////////
		//1.�׽�Ʈ�� ���� ���縦 ���� ��ε� ���� �Ǵ� ���׽�Ʈ ���ۿ� ��ε� �Ѵ� 
		//2.��ε� �۾��� �׽�Ʈ ������ ������ ���� �� ���Ͽ� ���縦 �ε��Ѵ� 
		//3. 8�� ����, �Ǵ� ��(1~4), ��(5~8)����Ʈ�� ������ �ش� �׽��͸� test start �Ѵ� 
		//////////////////////////////////////////////////////////////////////////////////////
		case 1100:  //ó�� �ش� ������ �۾��ϴ� �κ����� ���Ͽ� �ʿ��� ������ �ε� ���ۿ� ���Ͽ� �׽��� ����Ʈ�� �ʿ��� ���� ��ŭ�� �ε����ۿ� ���޵ǵ��� �����Ѵ�
			//load buffer�� ���� ��û�� �۾��� �׽�Ʈ ����Ʈ ��ġ�� Ȯ���ϰ�, �� ���� ������ �´� ���縦 ���ۿ� �����ϵ��� ������ �����Ѵ�   
			//nTestSocket_0_4_Start_Pos => 0:���鿡�� ���� ù��° ����(0,1,2,3 socket �۾�), 4:���� �������� 4��° ����(4,5,6,7 ���� �۾�) 
			//sst_buffer_info[THD_LD_BUFF].st_pcb_info[i].nYesNo = CTL_YES;
			
			////////////////////////////////////////////
			//TEST SITE �۾� ��ġȮ��  //�۾��� �׽�Ʈ ����Ʈ ��ġ�� Ȯ���Ѵ� 
			//st_sync_info.nTestSite_Target_THD_Work_Site = m_nTestSite_THD_Work_Site; //�۾��� �׽�Ʈ ����Ʈ ���� ���� 
			st_sync_info.nTestSite_Target_THD_Work_Site[1] = m_nTestSite_THD_Work_Site; 
		
			st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][2] = st_find_testsite_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]][1].nSite_Pos; //left/right
			st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][3] = st_find_testsite_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]][1].nStart_FixPos;//fix start pos
			st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][4] = st_sync_info.nTestSite_Target_THD_Work_Site[1];//test site THD num(18~25)
		 
			if (st_sync_info.nTestSite_Target_THD_Work_Site[1] >= THD_TESTSITE_1 && st_sync_info.nTestSite_Target_THD_Work_Site[1] <= THD_TESTSITE_8)
			{
				if (st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][0] == CTL_FREE || st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][0] == CTL_CLEAR)
				{			 
					st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][0] = CTL_REQ;
					st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][1] = WORK_PICK;
					//st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][2] = st_find_testsite_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]][1].nSite_Pos; //left/right
					//st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][3] = st_find_testsite_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]][1].nStart_FixPos;//fix start pos
					//st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][4] = st_sync_info.nTestSite_Target_THD_Work_Site[1];//test site THD num(18~25)
					//2017.0319
					strTemp.Format(_T("[1100_TESTSITE_PICK] Work_Site : %d Start_FixPos : %d"), m_nTestSite_THD_Work_Site , st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][3]);
					clsMem.OnNormalMessageWrite(strTemp);
				}
			}	
			else
			{
				break;
			}

			if(st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].strLotNo == st_lot_info[m_nLotProcessNum].strLotNo)
			{
				if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_TEST_RBT] >=  LOT_END_START) //���̻� �ε� ����� ������ ��û�� �ʿ���� 
				{
					m_nRunStep = 2000;
					break;
				}
			}
			else 
			{	//lot�� ���е��� ������ �ȵȴ� 
				break;
			}

		 	//////////////////////////////////////////////////////////////////////////////////
			//�׽�Ʈ ����Ʈ�� ���縦 ��ε� �ϴ�, �̰��� ���� �ε� ������ ���縦 �̸� ��û 
			//////////////////////////////////////////////////////////////////////////////////

			/*james 2016.0926 
			TestSite_PickPlace_SocketData_LoadBuff_Map_Combine(0, st_sync_info.nTestSite_Target_THD_Work_Site[1], st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][3]); //���� ��ġ�� �۾� ���ɿ��� ����,��Ŀ�� ��� ��/�� ���� ����

			//st_buffer_info[THD_LD_BUFF].nTestSIte_No			= st_sync_info.nTestSite_Target_THD_Work_Site[1];
			//st_buffer_info[THD_LD_BUFF].nTestSite_LR_Pos		= st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //�ε� ������ �׽��Ϳ� �´� ȸ�� ������ ���Ѵ�
			//st_buffer_info[THD_LD_BUFF].nTestSite_Start_FixPos	= st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].nStart_FixPos;

			// jtkim 20160831
			//if (st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_FREE || st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_CLEAR) 
			//{
				st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_REQ;
				st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][1] = WORK_PICK; //���縦 ���� �� �ְ� ��û 
				st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][2] = st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][2]; //st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //�ε� ������ �׽��Ϳ� �´� ȸ�� ������ ���Ѵ�
								
				st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][3] = st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][3]; //james 2016.0924 �߰� 
				st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][4] = st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][4]; //james 2016.0924 �߰�
			//}
			*/

			if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_TEST_RBT] >=  LOT_END_START) //james2016.0929 
			{
				break;
			}

			//james 2016.0926 ���� ���� 
			if(st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].strLotNo == st_lot_info[m_nLotProcessNum].strLotNo)
			{
				if (st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_FREE || st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_CLEAR) 
				{
					nRet_1 = TestSite_PickPlace_SocketData_LoadBuff_Map_Combine(0, st_sync_info.nTestSite_Target_THD_Work_Site[1], st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][3]); //���� ��ġ�� �۾� ���ɿ��� ����,��Ŀ�� ��� ��/�� ���� ����
 		 
					if( nRet_1 == RET_GOOD )
					{
						st_sync_info.nSmema_Tray_Input_Req = CTL_YES;//�׽��Ͱ� ��û�ߴ�
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_REQ;
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][1] = WORK_PICK; //���縦 ���� �� �ְ� ��û 
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][2] = st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][2]; //st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //�ε� ������ �׽��Ϳ� �´� ȸ�� ������ ���Ѵ�
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][3] = st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][3]; //james 2016.0924 �߰� 
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][4] = st_sync_info.nTestSite_Target_THD_Work_Site[1]; //james 2016.0924 �߰�

						//2017.0319
						strTemp.Format(_T("[1100_TESTSITE_PICK_LD_BUFF_REQ] Work_Site : %d Start_FixPos : %d"), st_sync_info.nTestSite_Target_THD_Work_Site[1] , st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][3]);
						clsMem.OnNormalMessageWrite(strTemp);
					}
					else
					{
						if (st_handler_info.cWndMain != NULL)
						{
							st_other_info.nBuzYesNo = YES;
							if(st_sync_info.nTestSite_Target_THD_Work_Site[1]-THD_TESTSITE_1 >= 0 )
							{
								st_other_info.strBoxMsg.Format(_T("It have to enable 1 over per %dth socket. \r\n Please enable socket."), st_sync_info.nTestSite_Target_THD_Work_Site[1]-THD_TESTSITE_1 + 1);
							}
							else
							{
								st_other_info.strBoxMsg = _T("It have to enable 1 over per socket. \r\n Please enable socket.");
							}
							st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
						}
					}
				}
			}//james 2016.0926 ���� ���� 

			//////////////////////////////////////////////////////////////////////////
			//2017.0430 //������ �츱�� -> 2017.0821 ����
// 			if(st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].strLotNo == st_lot_info[m_nLotProcessNum].strLotNo)
// 			{
// 				//�ϴ� ���׽�Ʈ�� üƮ���� + ��δ� �����߿� ���׽�Ʈ���簡 �ִ��� üũ����.
// 				//1.�ϴ� ��밡���� ������ ����� üũ�Ѵ�.
// 				//st_sync_info.nTestSite_Target_THD_Work_Site[1]:�۾� ����
// 
// 				//�׽�Ʈ����Ʈ ��ε��Ҷ� �ҷ� ����̽��� �����, �׽�Ʈ ����Ʈ�� ����̽��� �ִ��� Ȯ�� �� �� 4���� �ƴ϶�(���� 4���̻��϶��� Ȯ������ �ٻڴ�)
// 				//�Ⱦ��� �׽�Ʈ����Ʈ�� ���� ���簡 retest���ۿ� ���簡 ����� ���  �ϴ�, ���� ���׽�Ʈ ���縦 �Ⱦ��Ѵ�.
// 
// 				int nDvc = 0, nRetestDvc = 0, nPos = 0;
// 				for(i=0; i< 4; i++) //loading ���� ���� 4�� 
// 				{
// 					int nTestSite_THD_Num = st_sync_info.nTestSite_Target_THD_Work_Site[1];
// 					int nTestSocket_0_4_Start_Pos = st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][3];
// 
// 					if( nTestSite_THD_Num >= THD_TESTSITE_1 && nTestSite_THD_Num <= THD_TESTSITE_4)
// 					{
// 						int nPos = (7 - nTestSocket_0_4_Start_Pos) - i;
// 						if(st_test_site_info[nTestSite_THD_Num].st_pcb_info[nPos].nEnable == CTL_YES) //socket ON
// 						{
// 							nDvc++;
// 						}
// 						if( st_test_site_info[nTestSite_THD_Num].st_pcb_info[nPos].nBin == BD_DATA_CONTINUE_FAIL ||
// 							st_test_site_info[nTestSite_THD_Num].st_pcb_info[nPos].nBin == BD_DATA_RETEST )
// 						{
// 							nRetestDvc++;
// 						}
// 
// 					}
// 					else //if(nTestSite_THD_Num >= THD_TESTSITE_5 && nTestSite_THD_Num <= THD_TESTSITE_8) //���� �׽��� , ���� ������ ������ġ ����  (1ȸ�۾�)0,1,2,3, (2ȸ�۾�)4,5,6,7 �� ������ ���� ���� ��ġ�� ���� ���ǵ�
// 					{
// 						nPos = nTestSocket_0_4_Start_Pos + i;
// 						if(st_test_site_info[nTestSite_THD_Num].st_pcb_info[nPos].nEnable  == CTL_YES) //socket ON
// 						{
// 							nDvc++;
// 						}
// 						if( st_test_site_info[nTestSite_THD_Num].st_pcb_info[nPos].nBin == BD_DATA_CONTINUE_FAIL ||
// 							st_test_site_info[nTestSite_THD_Num].st_pcb_info[nPos].nBin == BD_DATA_RETEST )
// 						{
// 							nRetestDvc++;
// 						}
// 					}  
// 				}
// 				//���׽�Ʈ ������ ���� ������ ã�´�
// 				nRet_1 = clsFunc.Find_TrayWork_Pos(0, st_sync_info.nRetestBuff_Traget_THD_Work_Site, CTL_YES, m_npFindWorkPosYXCPB, THD_TEST_RBT, m_strFindLotNo);
// 				if(nRet_1 == RET_GOOD) //���׽�Ʈ ���ۿ� �۾� ������ ���簡 �ִ�
// 				{
// 					nRetestDvc = nRetestDvc + m_npFindWorkPosYXCPB[2];
// 				}
// 
// 				if( nDvc <= nRetestDvc )
// 				{//���׽�Ʈ ������ ���縦 �Ⱦ��Ѵ�.
// 					m_nSort_Count = 1000;
// 				}
// 				else
// 				{//load buffer�� ���縦 ��û�Ѵ�.
// 					if (st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_FREE || st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_CLEAR) 
// 					{
// 						nRet_1 = TestSite_PickPlace_SocketData_LoadBuff_Map_Combine(0, st_sync_info.nTestSite_Target_THD_Work_Site[1], st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][3]); //���� ��ġ�� �۾� ���ɿ��� ����,��Ŀ�� ��� ��/�� ���� ����
// 
// 						if( nRet_1 == RET_GOOD )
// 						{
// 							st_sync_info.nSmema_Tray_Input_Req = CTL_YES;//�׽��Ͱ� ��û�ߴ�
// 							st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_REQ;
// 							st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][1] = WORK_PICK; //���縦 ���� �� �ְ� ��û 
// 							st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][2] = st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][2]; //st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //�ε� ������ �׽��Ϳ� �´� ȸ�� ������ ���Ѵ�
// 							st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][3] = st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][3]; //james 2016.0924 �߰� 
// 							st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][4] = st_sync_info.nTestSite_Target_THD_Work_Site[1]; //james 2016.0924 �߰�
// 
// 							//2017.0319
// 							strTemp.Format(_T("[1100_TESTSITE_PICK_LD_BUFF_REQ] Work_Site : %d Start_FixPos : %d"), st_sync_info.nTestSite_Target_THD_Work_Site[1] , st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][3]);
// 							clsMem.OnNormalMessageWrite(strTemp);
// 						}
// 						else
// 						{
// 							if (st_handler_info.cWndMain != NULL)
// 							{
// 								st_other_info.nBuzYesNo = YES;
// 								if(st_sync_info.nTestSite_Target_THD_Work_Site[1]-THD_TESTSITE_1 >= 0 )
// 								{
// 									st_other_info.strBoxMsg.Format(_T("It have to enable 1 over per %dth socket. \r\n Please enable socket."), st_sync_info.nTestSite_Target_THD_Work_Site[1]-THD_TESTSITE_1 + 1);
// 								}
// 								else
// 								{
// 									st_other_info.strBoxMsg = _T("It have to enable 1 over per socket. \r\n Please enable socket.");
// 								}
// 								st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
// 							}
// 						}
// 					}
// 				}
// 			}
			m_nRunStep = 2000;
			break;

			
		///////////////////////////////////////////////////////////////////////////////////////
		// LOAD BUFFER ���� PICK ���� TEST SITE�� �̵��Ͽ� TEST DVC PLACE/INSERT ���� �����Ѵ� 
		// �׽�Ʈ ����Ʈ loading ���� 
		///////////////////////////////////////////////////////////////////////////////////////
		//1.�ε� ���縦 ���� �׽�Ʈ ������ ������ ��ġ�� �����Ѵ�  
		//2.�ش� �׽�Ʈ ����Ʈ�� ���Ͽ� ���� ��ŭ ��� �־����� �׽��Ͱ� �׽�Ʈ �� �� �ְ� ������ ���Ѵ�   
		case 1200: //ó�� �ش� ������ �۾��ϴ� �κ����� ���Ͽ� �ʿ��� ������ �ε� ���ۿ� ���Ͽ� �׽��� ����Ʈ�� �ʿ��� ���� ��ŭ�� �ε����ۿ� ���޵ǵ��� �����Ѵ�
			//load buffer�� ���� ��û�� �۾��� �׽�Ʈ ����Ʈ ��ġ�� Ȯ���ϰ�, �� ���� ������ �´� ���縦 ���ۿ� �����ϵ��� ������ �����Ѵ�   
			//nTestSocket_0_4_Start_Pos => 0:���鿡�� ���� ù��° ����(0,1,2,3 socket �۾�), 4:���� �������� 4��° ����(4,5,6,7 ���� �۾�) 
			//sst_buffer_info[THD_LD_BUFF].st_pcb_info[i].nYesNo = CTL_YES;
			
			//TEST SITE �۾� ��ġȮ�� 
			st_sync_info.nTestSite_Target_THD_Work_Site[2]									= m_nTestSite_THD_Work_Site; //�۾��� �׽�Ʈ ����Ʈ ���� ���� 
			 
			st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2] = st_find_testsite_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2].nSite_Pos; //left/right
			st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3] = st_find_testsite_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2].nStart_FixPos;//fix start pos
			st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][4] = st_sync_info.nTestSite_Target_THD_Work_Site[2];//test site THD num(18~25)

			if (st_sync_info.nTestSite_Target_THD_Work_Site[2] >= THD_TESTSITE_1 && st_sync_info.nTestSite_Target_THD_Work_Site[2] <= THD_TESTSITE_8)
			{
				if (st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][0] == CTL_FREE || st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][0] == CTL_CLEAR)
				{
					st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][0] = CTL_REQ;
					st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][1] = WORK_PLACE;
					//st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2] = st_find_testsite_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2].nSite_Pos; //left/right
					//st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3] = st_find_testsite_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2].nStart_FixPos;//fix start pos
					//st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][4] = st_sync_info.nTestSite_Target_THD_Work_Site[2];//test site THD num(18~25)

					//2017.0319
					strTemp.Format(_T("[1200_LDBUFFPICK_TEST_PLACE] Work_Site : %d Start_FixPos : %d"), m_nTestSite_THD_Work_Site , st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3]);
					clsMem.OnNormalMessageWrite(strTemp);
				}
			}
			else
			{
				break;
			} 
			
			
			
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//retest bin buffer�� ������ Ȯ���Ͽ� �������� �̻� ���ۿ� �����ϸ� �ε� ���纸�� ���׽�Ʈ ���縦 ���� ó���Ѵ�.
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			if(st_lot_info[m_nLotProcessNum].strLotNo == st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].strLotNo) 
			{
				st_sync_info.nRetestBuff_Traget_THD_Work_Site  = st_lot_info[m_nLotProcessNum].nBuffer_Retest_THD_Num;
			} 
			else 
			{	//lot�� ���е��� ������ �ȵȴ� 
				break;
			}

			//���׽�Ʈ ���۸� ã�´�
			m_npFindWorkPosYXCPB[2] = 0;
			nRet_1 = clsFunc.Find_TrayWork_Pos(0, st_sync_info.nRetestBuff_Traget_THD_Work_Site, CTL_YES, m_npFindWorkPosYXCPB, THD_TEST_RBT, st_worker_info.strLotNo_Empty);
			if(nRet_1 == RET_GOOD) //�۾� ������ ���簡 �ִ�
			{
				if(/*m_npFindWorkPosYXCPB[2] >= 7*/m_npFindWorkPosYXCPB[2] >= 4)//2016.1023
				{					//7�� �̻��̸� 1�� �۾�����(8��)�� ���� ���� �̻��� ����� ���簡  �ִ°��̴� �ε� ���簡 �ƴ�, ���׽�Ʈ ���縦 ���� ó������.
					st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][0] = CTL_REQ;
					st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][1] = WORK_PICK; //���縦 ���� �� �ְ� ��û 
					st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][2] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2];//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos;					 
					st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][3] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3];
					st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][4] = st_sync_info.nTestSite_Target_THD_Work_Site[2];//test site num

					//st_buffer_info[st_sync_info.nRetestBuff_Traget_THD_Work_Site].nTestSIte_No            = st_sync_info.nTestSite_Target_THD_Work_Site[2];
					//st_buffer_info[st_sync_info.nRetestBuff_Traget_THD_Work_Site].nTestSite_LR_Pos        = st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //�ε� ������ �׽��Ϳ� �´� ȸ�� ������ ���Ѵ�
					//st_buffer_info[st_sync_info.nRetestBuff_Traget_THD_Work_Site].nTestSite_Start_FixPos  = st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nStart_FixPos;

					nTHD_ID = st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][4];
					nFixPos = st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][3];

					//����� ��Ŀ ���� ���� 
					for(i=0; i< 4; i++) //retest ���� ���� 4�� ���� Ȯ��  
					{
						if(nTHD_ID >= THD_TESTSITE_1 && nTHD_ID <= THD_TESTSITE_4) //���� �׽��� ,���� ������ ������ġ ���� (1ȸ�۾�)7,6,5,4, (2ȸ�۾�)3,2,1,0 �� ������ ���� ���� ��ġ�� ���� ���ǵ�
						{//���� �׽�Ʈ ����Ʈ 
							nPos = (7 - nFixPos) - i;
							if(st_test_site_info[nTHD_ID].st_pcb_info[nPos].nEnable == CTL_YES) //socket ON
							{								 
								st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable = CTL_YES;
							}
							else //if(st_test_site_info[nTHD_ID].st_pcb_info[nPos].nEnable  == CTL_NO)//Socket OFF
							{								 
								st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable = CTL_NO;
							}
						}
						else if(nTHD_ID >= THD_TESTSITE_5 && nTHD_ID <= THD_TESTSITE_8) //���� �׽��� , ���� ������ ������ġ ����  (1ȸ�۾�)0,1,2,3, (2ȸ�۾�)4,5,6,7 �� ������ ���� ���� ��ġ�� ���� ���ǵ�
						{//���� �׽�Ʈ ����Ʈ 
							nPos = nFixPos + i;
							if(st_test_site_info[nTHD_ID].st_pcb_info[nPos].nEnable  == CTL_YES) //socket ON
							{								 
								st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable = CTL_YES;
							}
							else //if(st_test_site_info[nTHD_ID].st_pcb_info[nPos].nEnable == CTL_NO)//Socket OFF
							{								 
								st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable = CTL_NO;
							}
						}  
					}

					//2017.0319
					strTemp.Format(_T("[1200_RETEST_BUFFER] Work_Site : %d Start_FixPos : %d"), nTHD_ID, nFixPos);
					clsMem.OnNormalMessageWrite(strTemp);

					m_nRunStep = 5000;//retest buffer������ ���� �׽�Ʈ ����Ʈ�� �μ�Ʈ�Ѵ�

					//strTemp.Format(_T("[RETEST BUFFER2] %d"), m_nRunStep);
					//clsMem.OnNormalMessageWrite(strTemp);

					break;
				}
			}
			else if(nRet_1 == RET_SKIP)  
			{	//���׽�Ʈ ���簡 �ϳ��� ���� 		
				//�ε� ���ۿ� ���縦 ��û�Ѵ�
				 
			}
			/////////////////////////////////////////////////////////////////////////////////////// testest ���� pick

			if(st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].strLotNo == st_lot_info[m_nLotProcessNum].strLotNo)
			{
				if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_TEST_RBT] >=  LOT_END_START) //���̻� �ε� ����� ������ ��û�� �ʿ���� 
				{
					//james 2016.0926 �߰� 
					nCount = 0;nRetestExistCnt = 0;
					for(x = 0; x < TEST_SOCKET_PARA; x++) //x ���� ���� ���� ����(8��, 0~7��)�� �����Ѵ� 
					{
						if(st_sync_info.nTestSite_Target_THD_Work_Site[2] >= THD_TESTSITE_1 && st_sync_info.nTestSite_Target_THD_Work_Site[2] <= THD_TESTSITE_4)
						{//left site 
							nPos = 7 - x;
							if(st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].strLotNo == m_strFindLotNo 
								&& st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].st_pcb_info[nPos].nYesNo == CTL_YES
								&& st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][5] == BD_NONE)
							{
								nCount++;
								if(nPos >= 4) nRetestExistCnt++;
							}
						}
						else if(st_sync_info.nTestSite_Target_THD_Work_Site[2] >= THD_TESTSITE_5 && st_sync_info.nTestSite_Target_THD_Work_Site[2] <= THD_TESTSITE_8)
						{//right site
							nPos = x;
							if(st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].strLotNo == m_strFindLotNo 
								&& st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].st_pcb_info[nPos].nYesNo == CTL_YES
								&& st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][5] == BD_NONE)
							{
								nCount++;
								if(nPos >= 4) nRetestExistCnt++;
								
							}
						}						
					}
					//kwlee 2016.1024
					//if(nCount > 0 && nRetestExistCnt == 0 && m_npFindWorkPosYXCPB[2] == 0) //m_npFindWorkPosYXCPB[2] == 0 ���׽�Ʈ ���ۿ� ���簡 ������ 
					if(nCount > 0 && nRetestExistCnt >= 0 && m_npFindWorkPosYXCPB[2] == 0) //m_npFindWorkPosYXCPB[2] == 0 ���׽�Ʈ ���ۿ� ���簡 ������ 
					{//�׽�Ʈ ���Ͽ� ������ ���� ���縦 �׽�Ʈ ��ŸƮ ���� �����̴� �ؾ� �Ѵ� 
						m_nRunStep = 8000;//james 2016.0926 
						break;
					}
					else
					{
						//2017.0613
						//strTemp.Format(_T("[RESTRBT_1200] nCount : %d nRetestExistCnt : %d m_npFindWorkPosYXCPB[2]:%d"), nCount, nRetestExistCnt, m_npFindWorkPosYXCPB[2]);
						//clsMem.OnNormalMessageWrite(strTemp);
						 m_nRunStep = 1000; //james 2016.0918 6000;
						break;
					}
				}
			} 
			else 
			{//lot�� ���е��� ������ �ȵȴ� 
				break;
			}

			//////////////////////////////////////////////////////////////////////////////////
			// �ε� ������ ���縦 ��û  �Ѵ� 
			//////////////////////////////////////////////////////////////////////////////////
			/* //james 2016.0926 ���� ���� 
			TestSite_PickPlace_SocketData_LoadBuff_Map_Combine(0, st_sync_info.nTestSite_Target_THD_Work_Site[2], st_find_testsite_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2].nStart_FixPos); //���� ��ġ�� �۾� ���ɿ��� ����,��Ŀ�� ��� ��/�� ���� ����

			//st_buffer_info[THD_LD_BUFF].nTestSIte_No			= st_sync_info.nTestSite_Target_THD_Work_Site[2];
			//st_buffer_info[THD_LD_BUFF].nTestSite_LR_Pos		= st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //�ε� ������ �׽��Ϳ� �´� ȸ�� ������ ���Ѵ�
			//st_buffer_info[THD_LD_BUFF].nTestSite_Start_FixPos	= st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nStart_FixPos;

			// jtkim 20160831
			if (st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_FREE || st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_CLEAR) 
			{
				st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_REQ;
				st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][1] = WORK_PICK; //���縦 ���� �� �ְ� ��û 
				st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][2] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2];//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //�ε� ������ �׽��Ϳ� �´� ȸ�� ������ ���Ѵ�
				st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][3] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3]; //james 2016.0924 �߰� 
				st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][4] = st_sync_info.nTestSite_Target_THD_Work_Site[2]; //test site num
			}*/

			if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_TEST_RBT] >=  LOT_END_START) //james2016.0929 
			{
				break;
			}

			//james 2016.0926 ���� ���� 
			if( st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].strLotNo == st_lot_info[m_nLotProcessNum].strLotNo)
			{
				if (st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_FREE || st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_CLEAR) 
				{
					nRet_1 = TestSite_PickPlace_SocketData_LoadBuff_Map_Combine(1, st_sync_info.nTestSite_Target_THD_Work_Site[2], st_find_testsite_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2].nStart_FixPos); //���� ��ġ�� �۾� ���ɿ��� ����,��Ŀ�� ��� ��/�� ���� ����

					if( nRet_1 == RET_GOOD )
					{
						st_sync_info.nSmema_Tray_Input_Req = CTL_YES;//�׽��Ͱ� ��û�ߴ�
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_REQ;
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][1] = WORK_PICK; //���縦 ���� �� �ְ� ��û 
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][2] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2];//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //�ε� ������ �׽��Ϳ� �´� ȸ�� ������ ���Ѵ�
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][3] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3]; //james 2016.0924 �߰� 
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][4] = st_sync_info.nTestSite_Target_THD_Work_Site[2]; //test site num

						//2017.0319
						strTemp.Format(_T("[1200_RETEST_BUFFER] Work_Site : %d Start_FixPos : %d"), nTHD_ID, nFixPos);
						clsMem.OnNormalMessageWrite(strTemp);
					}
					else
					{
						if (st_handler_info.cWndMain != NULL)
						{
							st_other_info.nBuzYesNo = YES;
							if(st_sync_info.nTestSite_Target_THD_Work_Site[1]-THD_TESTSITE_1 >= 0 )
							{
								st_other_info.strBoxMsg.Format(_T("It have to enable 1 over per %dth socket. \r\n Please enable socket."), st_sync_info.nTestSite_Target_THD_Work_Site[1]-THD_TESTSITE_1 + 1);
							}
							else
							{
								st_other_info.strBoxMsg = _T("It have to enable 1 over per socket. \r\n Please enable socket.");
							}
							st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
						}
						break;
					}
				}
				 m_nRunStep = 6000;
			}
			else
			{
				 m_nRunStep = 1000; //james 2016.0926 �߰� 
				 break;
			}

			//james 2016.0926 m_nRunStep = 6000;		
			break;


		/////////////////////////////////////////////////////////
		//�׽�Ʈ�� ���� ���縦 �׽�Ʈ ����Ʈ���� ���� ��ε��Ѵ� 
		//TEST SITE PICK
		/////////////////////////////////////////////////////////
		case 2000:
			 //nStart_FixPos �� ��������(��񿡼� ������ ���� ������ġ) ��ġ��, = 0 �ƴϸ� 4 ���� �ϳ��̴�.(���� ������ġ�� left site: 7, 3,  right site : 0, 4)
			nRet_1 = Process_DVC_Pick(0, st_sync_info.nTestSite_Target_THD_Work_Site[1], st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][2], st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][3]);
			
			if(nRet_1 == RET_GOOD)
			{
				//2017.0319
				strTemp.Format(_T("[2000_TEST SITE PICK] Work_Site : %d Test_Site : %d Start_FixPos : %d"), st_sync_info.nTestSite_Target_THD_Work_Site[1],st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][2],st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][3]);
				clsMem.OnNormalMessageWrite(strTemp);

				//////////////////////////////////////////////////
				//2016.1017
				bool bPcbYesNo = CTL_NO;
				int nTestnum = 0, nTestRest = 0, nPickDvc = 0;
				for(i = 0; i < TEST_SOCKET_PARA; i++)
				{
					if( st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].st_pcb_info[i].nYesNo == CTL_YES)
					{
						bPcbYesNo = CTL_YES;						
					}

					//2017.0301
					if( st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].st_pcb_info[i].nEnable == CTL_YES)
					{
						nPickDvc++;//�Ⱦ��� �׽����� ���� ���� ����
					}
				}
				if( bPcbYesNo == CTL_NO )
				{
					for(i = 0; i < TEST_SOCKET_PARA; i++)
					{
						st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].st_pcb_info[i].nTestBdStart = BD_NONE;
					}
				}
				//////////////////////////////////////////////////

				//////////////////////////////////////////////////
				// Next lot�� ���۵ǰ� ���� lot�� load ���簡 ���̻� ���� �����϶� �׽�Ʈ ����Ʈ�� ����(CURR LOT : NEXT LOT)�Ͽ� ����ϱ� ���� �߰�
				// 2 lot�� ��� �����. NEXT Lot : test #1 ~ #4����, CURR Lot : #5 ~ #8 ���� ���ÿ� 2 Lot�� ������ �� �־� �����Ͽ� ��� 
				if(st_lot_info[LOT_CURR].nLot_THD_Status[THD_TEST_RBT] >=  LOT_END_START && st_lot_info[LOT_NEXT].nLotStatus >= LOT_START) //���� lot�� lot end �������̰�, ó�� �ε� ���簡 ���̻� ������ �׽�Ʈ ����Ʈ�� Next Lot�� ���� Test Site #1 ~#4 ������ ��� ����� ������ Nect Lot ������ �����Ѵ� 
				{
// 					if(st_sync_info.nTestSite_Target_THD_Work_Site[1] >= THD_TESTSITE_1 && st_sync_info.nTestSite_Target_THD_Work_Site[1] <= THD_TESTSITE_5) //�ε� lot end �̰�, ���� �׽����� �����ϸ� NEXT LOT���� �����Ͽ� ����Ѵ�. 
					//2017.0302
					if(st_sync_info.nTestSite_Target_THD_Work_Site[1] >= THD_TESTSITE_1 && st_sync_info.nTestSite_Target_THD_Work_Site[1] <= THD_TESTSITE_8)
					{//�ش� �׽�Ʈ ����Ʈ�� ���簡 ��� ������ 
						nCount = 0;
						for(i = 0; i < TEST_SOCKET_PARA; i++) //x ���� ���� ���� ����(8��, 0~7��)�� �����Ѵ� 
						{//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].strLotNo == strLotNo && 
							if(st_lot_info[LOT_CURR].strLotNo ==  st_test_site_info[ st_sync_info.nTestSite_Target_THD_Work_Site[1]].strLotNo
								&& st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].st_pcb_info[i].nYesNo == CTL_YES)
							{
								nCount++;
							}
						}


						nRetestExistCnt = 0;
						if(nCount == 0) //�� �׽�Ʈ ����Ʈ��  ���簡 �ϳ��� ���������ʴ�, ����ִ� ���� 
						{
// 							for(i = 0 ; i < st_recipe_info.nRetestBuffer_Num; i++) //15
// 							{
// 								if(st_buffer_info[st_lot_info[LOT_CURR].nBuffer_Retest_THD_Num].st_pcb_info[i].nYesNo == CTL_YES)
// 								{
// 									nRetestExistCnt++;
// 								}
// 							} 
							//kwlee 2016.1108
// 							nSiteExistCnt = 0;
// 							nDvcCount = 0;
// 
// 							for(i = 0; i < 4; i++)
// 							{
//  								if(st_lot_info[LOT_CURR].strLotNo == st_picker[THD_TEST_RBT].st_pcb_info[i].strLotNo
//  									&& st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == CTL_YES
//  									&& (st_picker[THD_TEST_RBT].st_pcb_info[i].nBin == BD_DATA_RETEST || st_picker[THD_TEST_RBT].st_pcb_info[i].nBin == BD_DATA_CONTINUE_FAIL) )
// 								{
// 									nRetestExistCnt++;//���� retest ����
// 								}
// 							}

// 							for( int nSite = THD_TESTSITE_1; nSite <= THD_TESTSITE_8; nSite++)
// 							{
// 								nEnableSokPerSite[nSite-THD_TESTSITE_1] = 0;
// 								for(i = 0; i < TEST_SOCKET_PARA; i++)
// 								{
// 									if(st_lot_info[LOT_CURR].strLotNo == st_test_site_info[nSite].strLotNo
// 										&& st_test_site_info[nSite].st_pcb_info[i].nYesNo == CTL_YES)
// 									{
// 										nDvcCount++;//���� ���� ����̽��� ���Ͽ� �����ִ� ����
// 									}
// 								}
// 
// 
// 								for(j = 0; j < TEST_SOCKET_PARA; j++)
// 								{
// 									if(st_lot_info[LOT_CURR].strLotNo == st_test_site_info[nSite].strLotNo &&
// 										st_test_site_info[nSite].st_pcb_info[j].nEnable == YES )
// 									{
// 										nSiteExistCnt++;//������� ��밡���� �׽�Ʈ ����Ʈ
// 										nEnableSokPerSite[nSite-THD_TESTSITE_1]++;
// 									}
// 								}
// 							}
							/////
							//���� ���� ����̽��� ���Ͽ� �����ִ� ���� + ���׽�Ʈ ����
							//2017.0302
// 							bool nNextLotPlaceSite = true;
// 							if(  ( nDvcCount + nRetestExistCnt ) >= nSiteExistCnt  )
// 							{						
// 								nNextLotPlaceSite = false;
// 							}
// 							if( nNextLotPlaceSite == true )
// 							{
// 								if( ( nDvcCount + nRetestExistCnt ) < nSiteExistCnt )
// 								{
// 									nCount = 0;
// 									nUseSocket = 0;
// 									for( int nSiteNum = THD_TESTSITE_1; nSiteNum <= THD_TESTSITE_8; nSiteNum++)
// 									{
// 										nCount = 0;
// 										for(i = 0; i < TEST_SOCKET_PARA; i++) //x ���� ���� ���� ����(8��, 0~7��)�� �����Ѵ� 
// 										{
// 											if( st_lot_info[LOT_CURR].strLotNo == st_test_site_info[nSiteNum].strLotNo && st_test_site_info[nSiteNum].st_pcb_info[i].nYesNo == CTL_YES )
// 											{
// 												nCount++;
// 											}
// 										}
// 										//���϶����� ���ϻ�����̰� �������� �ʴ� ����Ʈ�� �ִ°��� �˾ƺ��� ����
// 										//���϶� ��밡���� ���� ������ ���ϰ� 
// 										//���� ����Ʈ�� ����̽��� �մ�üũ�Ͽ� ���� �κ��� �����ϴ����� ���׽�Ʈ�� �� 1������Ʈ�� ����ΰ� �۾��Ѵ�.
// 										if( nEnableSokPerSite[nSiteNum-THD_TESTSITE_1] > 0 )
// 										{
// 											nUseLotSocket++;//���϶� ��밡���� ����Ʈ ��
// 										}
// 										if( nEnableSokPerSite[nSiteNum] > 0 && nCount > 0)
// 										{
// 											//���� ������� ����Ʈ ��
// 											nUseSocket++;//���϶����� ������� ����
// 										}
// 									}
// 
// 									if( nRetestExistCnt >= 4) //�ҷ��� 4�� �̻� �߻��Ҷ��� ������ �ϳ��� ���� ����.
// 									{//���϶� ��밡���� ����Ʈ ���� ���׽������Ҷ� �� ���� �����ϴ� ����Ʈ ������ 1�� �̻��� ������ �����ϸ� true �Ͽ� ����Ʈ�� �Ķ����� �����Ѵ�.
// 										if( nUseLotSocket <= (nUseSocket + 1) )//1�� ���׽�Ʈ ���簡 ������ �ִ� 1�� ����Ʈ�� �� �ʿ��ϴ�
// 											nNextLotPlaceSite = false;	
// 										else
// 											nNeedOneMoreSite = true;
// 									}
// 									else
// 									{
// 										if( nUseLotSocket <= nUseSocket  )
// 											nNextLotPlaceSite = false;
// 									}
// 									if( nUseSocket > 0 && nUseLotSocket <= nUseSocket && nUseLotSocket <= 1 )//������ ��밡���� ����Ʈ�� 1������ �ְ� ���׽�Ʈ�� 1���̻� �����ϳ��� �ּ� 1�� �̻� ����Ʈ�� ����Ѵ�.
// 										nNextLotPlaceSite = false;	
// 								}
// 							}
							//if( (nDvcCount + nRetestExistCnt) <= ( nSiteExistCnt - nNumSok ) )
							//2017.0302
							//if( nNextLotPlaceSite == true )
							if( clsFunc.CheckCurrLotEndNextLot( nNeedOneMoreSite ) == RET_GOOD )
							{
								st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].strLotNo = st_lot_info[LOT_NEXT].strLotNo; //test site�� lot������ LOT_NEXT�� ���� �����Ͽ� LOT_NEXT�� �����Ѵ� 
								//2016.1024
								st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].strPartNo = st_lot_info[LOT_NEXT].strPartNo;
								st_lot_info[LOT_NEXT].nLotNoApply_FirstTestStart = LOT_START; //james 2016.0926 

								//strTemp.Format(_T("[LOT_NEXT] TestSite : %d Lot : %s start"), st_sync_info.nTestSite_Target_THD_Work_Site[1], st_lot_info[LOT_NEXT].strLotNo);
								//kwlee 2017.0202
								strTemp.Format(_T("[LOT_NEXT] TestSite : %d Lot : %s start, Step : %d"), st_sync_info.nTestSite_Target_THD_Work_Site[1], st_lot_info[LOT_NEXT].strLotNo,m_nRunStep);
								clsMem.OnNormalMessageWrite(strTemp);

							}
						}
					}
				}

				m_nRunStep = 2100; //3000;


				////////////////////////////////////////////////////////////////////
				//2016.1203
				m_dwCycleTime[1][0] = GetTickCount();
				////////////////////////////////////////////////////////////////////
			}
			else if(nRet_1 == RET_PICKER_NOT_FIND) //��Ŀ�� ���̻� �۾��� ������ ����.  
			{//�� ������ ������ �ȵ� 
				m_nRunStep = 3000;			
			}
			else if(nRet_1 == RET_TRAY_NOT_FIND) //�׽��Ϳ��� �� �̻� �۾��� �� �ִ� ������ ����
			{//�� ������ ������ �ȵ� 				
				m_nRunStep = 3000;  
			}
			break;

		case 2100:
			nRet_1 = clsFunc.Find_TrayWork_Pos(0, st_sync_info.nTestSite_Target_THD_Work_Site[1], CTL_YES, m_npFindWorkPosYXCPB, THD_TEST_RBT, st_worker_info.strLotNo_Empty);
		
			if(nRet_1 == RET_SKIP || st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][3] == 4) //�۾� ������ ���簡 �ִ�
			{
				st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][0] = CTL_FREE;
			}
			m_nRunStep = 3000; 
			break;



		//////////////////////////////////////////////////
		// �����ִ� ������ ���� ����� ã�´�  
		//////////////////////////////////////////////////
		case 3000: //���� ������ Ȯ���Ͽ� ��ε� ���۸� ������� �����Ѵ�(GOOD BIN, REJECT BIN)
			nGoodReject_Cnt = 0, nRetest_Cnt = 0;
			m_strSortDvc_LotNo = _T("");
			strFindLotNo  = _T("");
			nFlag = 0;
			
			for (i = 0; i < m_nPickerPara; i++) //4ea
			{
				if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_YES)
				{
	//2016.0906 test ��
					//if(i == 0) st_picker[m_nRobotSite].st_pcb_info[i].nBin = BD_DATA_REJECT;
					//else if(i == 2) st_picker[m_nRobotSite].st_pcb_info[i].nBin = BD_DATA_RETEST;
	///////////////

					if(st_picker[m_nRobotSite].st_pcb_info[i].nBin == BD_DATA_GOOD || st_picker[m_nRobotSite].st_pcb_info[i].nBin == BD_DATA_REJECT)
					{
						nGoodReject_Cnt++;
					}
					else if(st_picker[m_nRobotSite].st_pcb_info[i].nBin == BD_DATA_RETEST)
					{
						nRetest_Cnt++;
					}
					//kwlee 2017.0214
					else
					{
						if (st_handler_info.cWndMain != NULL)
						{
							st_other_info.nBuzYesNo = YES;
							st_other_info.strBoxMsg.Format(_T("�׽�Ʈ Bin������ �����ϴ�. Picker No[%d], LotNo[%s], Picker LotNo[%s]. \r\n Check Please."), i, m_strSortDvc_LotNo, st_picker[m_nRobotSite].st_pcb_info[i].strLotNo);  
							clsMem.OnAbNormalMessagWrite(st_other_info.strBoxMsg);//�α� ����
							st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
						}
						st_picker[m_nRobotSite].st_pcb_info[i].nBin = BD_DATA_RETEST;
						nRetest_Cnt++;
					}
					//
					if(nFlag == 0) //strFindLotNo.IsEmpty()) //james 2016.0922 �߰�, ó�� ���縸 lot ������ ����Ѵ�  
					{
						//strFindLotNo = st_picker[m_nRobotSite].st_pcb_info[i].strLotNo; //lot no�� ã�´� 
						m_strSortDvc_LotNo = st_picker[m_nRobotSite].st_pcb_info[i].strLotNo; //lot no�� ã�´� 
						nFlag++;

					}
					else
					{
						if(m_strSortDvc_LotNo != st_picker[m_nRobotSite].st_pcb_info[i].strLotNo) //james 2016.0922 Ȥ�� ���簡 ���� ��츦 ����Ͽ� ���� ���� ���ʹ� ù ����� lot ������ ���Ѵ� 
						{
							nFlag = 1000;
							if (st_handler_info.cWndMain != NULL)
							{
								st_other_info.nBuzYesNo = YES;
								st_other_info.strBoxMsg.Format(_T("[Lot No MissMatch] Picker No[%d], LotNo[%s], Picker LotNo[%s]. \r\n Check Please."), i, m_strSortDvc_LotNo, st_picker[m_nRobotSite].st_pcb_info[i].strLotNo);  
								//st_other_info.strBoxMsg = _T("[LOT END] eject on Unloading #1 Good, Reject Stacker Tray. \r\n Please Remove it.");
								st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
							}
						}
					}
				}
			}//for

			if(nFlag >= 1000)
			{//james 2016.0922 lot ������ Ʋ���� ������ �� ���� 
				CTL_Lib.Alarm_Error_Occurrence(2113, dWARNING, st_alarm_info.strCode); //��� ���� ��Ų�� 
				break;
			}

			m_nBinSort_THD_Work_Site = 0;
			if(nGoodReject_Cnt > 0) //good �Ǵ� ���� ���簡 �ִ� , ��ε� ���۸� ����Ѵ� 
			{
				m_nBinSort_THD_Work_Site = THD_ULD_BUFF;
				m_nRunStep = 3100;  
			}
			else if(nRetest_Cnt > 0) //���׽�Ʈ ���ۿ� ����
			{
				/*
				if(st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].strLotNo == st_buffer_info[THD_RETEST_1_BUFF].strLotNo) //lot�� ���ԵǸ� ����, �׽�Ʈ����Ʈ, ����Ŀ� � ������ ������ lot �̸��� ���� �Ͽ� ��������� ���� �Ѵ�
				{
					m_nBinSort_THD_Work_Site = THD_RETEST_1_BUFF; 
				}
				else //if(st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].strLotNo == st_buffer_info[THD_RETEST_2_BUFF].strLotNo)
				{
					m_nBinSort_THD_Work_Site = THD_RETEST_2_BUFF; 
				}*/
				
				//st_buffer_info[THD_RETEST_1_BUFF].strLotNo = m_strSortDvc_LotNo;
				if(m_strSortDvc_LotNo == st_buffer_info[THD_RETEST_1_BUFF].strLotNo) 
				{
					st_sync_info.nRetestBuff_Traget_THD_Work_Site  = THD_RETEST_1_BUFF; //st_lot_info[LOT_CURR].nBuffer_Retest_THD_Num;
					m_nBinSort_THD_Work_Site = st_sync_info.nRetestBuff_Traget_THD_Work_Site ;
				}
				else if(m_strSortDvc_LotNo == st_buffer_info[THD_RETEST_2_BUFF].strLotNo) 
				{
					st_sync_info.nRetestBuff_Traget_THD_Work_Site  = THD_RETEST_2_BUFF; //st_lot_info[LOT_NEXT].nBuffer_Retest_THD_Num;
					m_nBinSort_THD_Work_Site = st_sync_info.nRetestBuff_Traget_THD_Work_Site ;
				}
				else 
				{
					//lot�� ���е��� ������ �ȵȴ� 
					break;
				}

				m_nRunStep = 3100;  
			}
			else 
			{	 
				//���縦 ��� �����ߴ�, ó�����ε��ư� �۾��� �̾�� 
				m_nRunStep = 1000;  
			}
			break;

		case 3100:
			if(m_nBinSort_THD_Work_Site == THD_ULD_BUFF) //good, reject ���� ���� 
			{	//�׽�Ʈ ����Ʈ�� ���縦 ��ε� �ϴ�, �̰��� ���� ��ε� ������ ���縦 �̸� ��û 
				st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][0] = CTL_REQ;
				st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][1] = WORK_PLACE; //���縦 ���� �� �ְ� ��û 
				st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][2] = st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][2]; //st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //��ε� ���۸� �׽��Ϳ� ���� 1�� pin ������ ���� �޴´� 
				st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][3] = st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][3];//james 2016.0626 
				st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][4] = st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][4];
				 
				//st_buffer_info[m_nBinSort_THD_Work_Site].nTestSIte_No				= st_sync_info.nTestSite_Target_THD_Work_Site[1];
				//st_buffer_info[m_nBinSort_THD_Work_Site].nTestSite_LR_Pos			= st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //�ε� ������ �׽��Ϳ� �´� ȸ�� ������ ���Ѵ�
				//st_buffer_info[m_nBinSort_THD_Work_Site].nTestSite_Start_FixPos		= st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].nStart_FixPos;
				//2017.0319
				strTemp.Format(_T("[3100_ULD_BUFF] Work_Site : %d Start_FixPos : %d"), st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][4], st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][3]);
				clsMem.OnNormalMessageWrite(strTemp);

			}
			else if(m_nBinSort_THD_Work_Site == THD_RETEST_1_BUFF || m_nBinSort_THD_Work_Site == THD_RETEST_2_BUFF)  //retest bin ���׽�Ʈ ���� ����
			{	//�׽�Ʈ ����Ʈ�� ���縦 ��ε� �ϴ�, �̰��� ���� ��ε� ������ ���縦 �̸� ��û 
				st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][0] = CTL_REQ;
				st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][1] = WORK_PLACE; //���縦 ���� �� �ְ� ��û 
				st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][2] = st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][2]; // st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].nSite_Pos;
				st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][3] = st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][3]; //james 2016.0626 
				st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][4] = st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][4];

				//2017.0319
				strTemp.Format(_T("[3100_RETEST_BUFF] SIte : %d Work_Site : %d Start_FixPos : %d"), m_nBinSort_THD_Work_Site, st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][4], st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][3]);
				clsMem.OnNormalMessageWrite(strTemp);
				//st_buffer_info[m_nBinSort_THD_Work_Site].nTestSIte_No				= st_sync_info.nTestSite_Target_THD_Work_Site[1];
				//st_buffer_info[m_nBinSort_THD_Work_Site].nTestSite_LR_Pos			= st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //�ε� ������ �׽��Ϳ� �´� ȸ�� ������ ���Ѵ�
				//st_buffer_info[m_nBinSort_THD_Work_Site].nTestSite_Start_FixPos		= st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].nStart_FixPos;
			}
			else
			{
				break;
			}
			m_nMove_Flag[0] = 0;//2016.0911 
			m_nRunStep = 3200;  
			break;

		//////////////////////////////////////////////////
		//�����ִ� ������ ���� ����Ʈ�� �غ񵵱⸦ ��ٸ���.
		//���� PLACE() �Լ� �ȿ��� ó���ؾ� �̸� �̵����� 
		//////////////////////////////////////////////////
		case 3200://��ε� ���ۿ� ��ε� ���縦 ���´� 
			if(m_nBinSort_THD_Work_Site == THD_ULD_BUFF)
			{
				if(st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][0] == CTL_READY && st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][1] == WORK_PLACE)
				{//unload buffer�� �ε��� ������ �ֵ��� �غ�Ǿ� �ִ� 

					m_nRunStep = 3300;  
				}
				else
				{
					if(m_nMove_Flag[0] == 0)
					{
						m_nMove_Flag[0] = 1;
						m_nRunStep = 3210;  
					}
				}
			}
			else if(m_nBinSort_THD_Work_Site == THD_RETEST_1_BUFF || m_nBinSort_THD_Work_Site == THD_RETEST_2_BUFF) 
			{
				if(st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][0] == CTL_READY && st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][1] == WORK_PLACE)
				{//retest buffer�� �ε��� ������ �ֵ��� �غ�Ǿ� �ִ� 

					m_nRunStep = 3400;  
				}
			}
			break;

		case 3210:
			m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_ULDBUF_START];
			m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_ULDBUF_START]; // 

			nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate); 
			if (nRet_1 == BD_GOOD) //�������� �̵�
			{				 
				m_nRunStep = 3200;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nRunStep = 3210;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
				CTL_Lib.Alarm_Error_Occurrence(2120, dWARNING, st_alarm_info.strCode);
				m_nRunStep = 3210;
			}
			break; 	


		/////////////////////////////////////////////////////////////
		//good, reject �� �� �޴� unlaod buffer �� ���縦 ���� ����
		//�����ִ� ������ �� ���� ���� �Ѵ�
		/////////////////////////////////////////////////////////////
		case 3300:
			nRet_1 = Process_DVC_Place(0, m_nBinSort_THD_Work_Site, st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][2], st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][3]); //�� ���� ���� ����, ��ε� ���� �Ǵ� ���׽�Ʈ ���ۿ� ���縦 ���� ���´� 
			if(nRet_1 == RET_GOOD)
			{								
				st_sync_info.nTestRbt_Dvc_Req[THD_ULD_BUFF][0] = CTL_FREE;//���縦 �κ�Ʈ�� ���� ����

				/////////////////////////////////////////////////////////////////////////////////////
				//2016.1203
				m_dwCycleTime[1][1] = GetTickCount();
				m_dwCycleTime[1][2] = m_dwCycleTime[1][1] - m_dwCycleTime[1][0];

				if (m_dwCycleTime[1][2] > (DWORD)0)
				{
					clsFunc.OnCycleTime(5, 
										m_strLotNo[1],
										m_strPartNo[1],
										m_dwCycleTime[1][2], 
										0, 
										0);
					m_strLotNo[1] = m_strPartNo[1] = _T("");
				}
				/////////////////////////////////////////////////////////////////////////////////////

				m_nRunStep = 3600;
			}
			else if(nRet_1 == RET_PICKER_NOT_FIND) //��Ŀ�� ���̻� �۾��� ������ ����.  
			{
				m_nRunStep = 3300;			
			}
			else if(nRet_1 == RET_TRAY_NOT_FIND) //�׽��Ϳ��� �� �̻� �۾��� �� �ִ� ������ ����
			{
				
				m_nRunStep = 3300;  
			}
			break; 

		////////////////////////////////////////////////////////////////////////////////////////
		// RETEST Buffer DVC Place
		// retest bin buffer�� ���縦 ���������� ȿ�������� ���� ����� ������� 
		//�����ִ� ������ �� ���� ���� �Ѵ�
		////////////////////////////////////////////////////////////////////////////////////////
		case 3400:
			/*
			if(st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].strLotNo == st_buffer_info[THD_RETEST_1_BUFF].strLotNo) //retest buffer 1 �� �����۾��� �׽�Ʈ ����Ʈ lot ������ ������ �� lot�� �ش��ϴ� ���縦 ó���Ѵ� 
			{
				st_sync_info.nRetestBuff_Traget_THD_Work_Site = THD_RETEST_1_BUFF;
			}
			else if(st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].strLotNo == st_buffer_info[THD_RETEST_2_BUFF].strLotNo)//retest buffer 2 �� �����۾��� �׽�Ʈ ����Ʈ lot ������ ������ �� lot�� �ش��ϴ� ���縦 ó���Ѵ� 
			{
				st_sync_info.nRetestBuff_Traget_THD_Work_Site = THD_RETEST_2_BUFF;
			}*/

		
			//�ѹ��� ���� ���� �� �ִ� ��ġ�� ã�� ���� ������ Ȯ�� �Ѵ�
			nTHD_ID = st_sync_info.nTestSite_Target_THD_Work_Site[WORK_PICK]; 
			nFixPos = st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][3]; //st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].nStart_FixPos;		

			for(i = 0; i < TEST_PICKER_PARA; i++) //2016.0907 test site���� �۾��� ������ Ȯ���Ѵ�
			{
				m_npPicker_YesNo[i] = CTL_NO;//clear
				if(st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == CTL_YES)// && st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable == CTL_YES)
				{
					m_npPicker_YesNo[i] = CTL_YES; //�۾��� ��Ŀ�� Ȯ���Ѵ� 
				}
				/*
				if(st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][2] == TESTSITE_LEFT)
				{
					if(st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == CTL_YES)// && st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable == CTL_YES)
					{
						nFind_JobPos = (st_recipe_info.nTestSocket_Num - 1) - i ; //LEFT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ���������� 3,2,1,0 �� �ݴ��̴� 
						nFind_JobPos = nFind_JobPos - nFixPos;

						if(st_test_site_info[nTHD_ID].st_pcb_info[nFind_JobPos].nYesNo == CTL_NO )//&& st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //Ʈ���̿� ���簡  ������ ���� �����ϰ� ��				{
						{
							m_npPicker_YesNo[i] = CTL_YES; //�۾��� ��Ŀ�� Ȯ���Ѵ� 
						} 
					}
				}
				else if(st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][2] == TESTSITE_RIGHT)
				{
					if(st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == CTL_YES)// && st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable == CTL_YES)
					{
						nFind_JobPos = i ; //LEFT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ���������� 3,2,1,0 �� �ݴ��̴� 
						nFind_JobPos = nFind_JobPos + nFixPos;

						if(st_test_site_info[nTHD_ID].st_pcb_info[nFind_JobPos].nYesNo == CTL_NO)//&& st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //Ʈ���̿� ���簡  ������ ���� �����ϰ� ��				{
						{
							m_npPicker_YesNo[i] = CTL_YES; //�۾��� ��Ŀ�� Ȯ���Ѵ� 
						} 
					}
				} */
			}

			//m_npPicker_YesNo[i] == CTL_YES �� ������ ã�� �� �´� ������ ������ �װ��� ���� ������ �۾��Ѵ� 
			//clsFunc.Find_RetestBinBuffer_WorkPos_Check(int n_Mode, int n_TestSite, int n_BuffSite, int n_PickerDvcYesNO,int n_BuffYesNO, int *np_Picker_YesNo, int *np_FindPosYXCPB,  int *np_BuffInfo_Count)  

			if(m_nRetestBuffer_PickPlace_Mode == 1) //james 2016.0929
			{
				nRet_1 = clsFunc.Find_RetestBuffer_PickPlace_WorkPos_Check(WORK_PLACE, nTHD_ID, nFixPos, st_sync_info.nRetestBuff_Traget_THD_Work_Site, m_npPicker_YesNo, m_npFindWorkPosYXCPB, m_np_BuffInfo_Count);//james 2016.0928 retest buffer �۾����� ��ġ ã�� 
			}
			else
			{
				nRet_1 = clsFunc.Find_RetestBinBuffer_WorkPos_Check(0, st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][2], st_sync_info.nRetestBuff_Traget_THD_Work_Site, CTL_YES, CTL_NO, m_npPicker_YesNo, m_npFindWorkPosYXCPB,  m_np_BuffInfo_Count);
			}
			
			if(nRet_1 == RET_GOOD)
			{
				m_nFirstTray_Y_Num =  m_npFindWorkPosYXCPB[0];// m_npFindWorkPosYXCPB[4]�� ���� 
				m_nFirstTray_X_Num =  m_npFindWorkPosYXCPB[1];// m_npFindWorkPosYXCPB[4]�� ���� 
				m_nBuffer_Work_Cnt =  m_npFindWorkPosYXCPB[2];//james 2016.0929 
				m_nFirstPicker_Num =  m_npFindWorkPosYXCPB[3]; 
				m_nFirst_Buff_Num  =  m_npFindWorkPosYXCPB[4]; 

				m_nPartial_FirstRetestBuffPos_Select_No = m_nFirst_Buff_Num; //���׽�Ʈ ���� ��� ���� ��ġ 
				m_nRunStep = 3500;// 
			}
			else if(nRet_1 == RET_ABORT)  //james 2016.0929 �۾��� ���簡 ���� 
			{
				nCount = 0; 
				for(i = 0; i < TEST_PICKER_PARA; i++)//st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == n_DvcYesNO && (n_DvcYesNO == CTL_YES && (n_DvcYesNO == CTL_YES || (n_PickerYesNO == CTL_NO && st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable == CTL_YES)) ) )
				{
					if(st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == CTL_YES)
					{
						nCount++;
					}
				} 
			}
			else if(nRet_1 == RET_SKIP)  
			{	//��Ŀ ������ �� �´� ���׽�Ʈ ������ ���� ������ ���� 	
				//���縦 �������� �ѹ��� ���� ������ ������ �ȵȴ� 
				//�ϴ� ������ �ȵ�, ���� �� �������ϰ� ���� ��� ���� �Ұ� 
				//��Ŀ ������ �� �´� ���׽�Ʈ ������ ���� ������ ���� 	
				//������ ������ Ȯ���Ͽ� , �� ��ġ�� �������� ��� ������ �κи� �� ���� ���縦 ���� �� ó������.

				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// Partial retest buffer�� ����� �� ���� ���� �׽�Ʈ ����Ʈ ������ ������ �κ��� ���� �� �κ��� ���縦 ���� �׽�Ʈ �׽�Ʈ ����Ʈ�� �̵��Ѵ�
				// �켱 �������� ���߿� �ʿ�� ���� 
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//james 2016.0929 �ļ��϶��� �۾� �����ϰ� ���� 
				nMaxCnt=0;
				if(st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][2]  == TESTSITE_LEFT)
				{
					for(i = st_recipe_info.nRetestBuffer_Num - 1; i >= 0; i--)// st_recipe_info.nRetestBuffer_Num; i++)//15
					{
						nRetest_Cnt = m_np_BuffInfo_Count[i] ;
						if(nMaxCnt < nRetest_Cnt)
						{
							nMaxCnt = nRetest_Cnt;
							m_nFirst_Buff_Num = i; //���� ���� ���� �ٻ�ġ�� ������ ������ ������ �̰��� ���� ��ġ�� �۾��Ѵ� 
						}
					}
				}
				else if(st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][2]  == TESTSITE_RIGHT)
				{
					for(i = 0; i < st_recipe_info.nRetestBuffer_Num; i++)//15
					{
						nRetest_Cnt = m_np_BuffInfo_Count[i] ;
						if(nMaxCnt < nRetest_Cnt)
						{
							nMaxCnt = nRetest_Cnt;
							m_nFirst_Buff_Num = i; //���� ���� ���� �ٻ�ġ�� ������ ������ ������ �̰��� ���� ��ġ�� �۾��Ѵ� 
						}
					}
				}				
				m_nPartial_FirstRetestBuffPos_Select_No = m_nFirst_Buff_Num; //partial ���׽�Ʈ ������ ������ ���縦 �����ϱ� ���� ������ ���� ��ġ 

				nFlag = CTL_NO;
				for(i = 0; i < TEST_PICKER_PARA; i++) //test site���� �۾��� ������ Ȯ���Ѵ�
				{
					m_npPicker_YesNo[i] = CTL_NO;//clear
					if(st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == CTL_YES) //
					{
						if(st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][2]  == TESTSITE_LEFT) // st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos == TESTSITE_LEFT) //test socket left ������������ :7->6->5->4  3->2->1->0
						{ 
							nPos = m_nPartial_FirstRetestBuffPos_Select_No - (i * 2);
							if(st_buffer_info[st_sync_info.nRetestBuff_Traget_THD_Work_Site].st_pcb_info[nPos].nYesNo == CTL_YES) 
							{
								//nPos = ((TEST_SOCKET_PARA - 1) - nFixPos) - i; //(i * 2);
								if(st_test_site_info[nTHD_ID].st_pcb_info[((TEST_SOCKET_PARA - 1) - nFixPos) - i].nYesNo == CTL_NO
									&& st_test_site_info[nTHD_ID].st_pcb_info[((TEST_SOCKET_PARA - 1) - nFixPos) - i].nEnable == CTL_YES)
								{
									if(nFlag == CTL_NO)
									{
										m_nFirstPicker_Num = i;
										nFlag = CTL_YES;
									} 	
									m_npPicker_YesNo[i] = CTL_YES;		 
								}
							}
						}
						else //if(st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][2]  == TESTSITE_RIGHT) //st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].nSite_Pos == TESTSITE_RIGHT) //test socket right ������������ :0->1->2->3  4->5->6->7
						{ 
							nPos = m_nPartial_FirstRetestBuffPos_Select_No + (i * 2);
							if(st_buffer_info[st_sync_info.nRetestBuff_Traget_THD_Work_Site].st_pcb_info[nPos].nYesNo == CTL_YES) 
							{								
								if(st_test_site_info[nTHD_ID].st_pcb_info[nFixPos + i].nYesNo == CTL_NO 
									&& st_test_site_info[nTHD_ID].st_pcb_info[nFixPos + i].nEnable == CTL_YES)
								{						 						
									if(nFlag == CTL_NO)
									{
										m_nFirstPicker_Num = i;
										nFlag = CTL_YES;
									} 
									m_npPicker_YesNo[i] = CTL_YES; 							 
								}
							}
						}
					}//if
				}//for

				if(nFlag == CTL_YES)
				{
					//���� ���縸 ���� ����, �ٽ� ó�� ���� ��ġ�� ���縦 �߰��� ���� �׽�Ʈ ����Ʈ�� �̵��Ѵ�.
					m_nFirstTray_Y_Num = m_nPartial_FirstRetestBuffPos_Select_No;// m_npFindWorkPosYXCPB[4]�� ���� 
					m_nFirstTray_X_Num = m_nPartial_FirstRetestBuffPos_Select_No;// m_npFindWorkPosYXCPB[4]�� ���� 
					m_nFirstPicker_Num = m_nFirstPicker_Num; 
					m_nFirst_Buff_Num  = m_nPartial_FirstRetestBuffPos_Select_No; 

					m_npFindWorkPosYXCPB[0] = m_nFirstTray_Y_Num; 
					m_npFindWorkPosYXCPB[1] = m_nFirstTray_X_Num; 
					m_npFindWorkPosYXCPB[3] = m_nFirstPicker_Num;
					m_npFindWorkPosYXCPB[4] = m_nFirst_Buff_Num;

					m_nRunStep = 3500;//���� ��ġ�� ���縦 ���� �۾��� �׽�Ʈ ����Ʈ�� �̵��Ѵ�
				}
				else
				{//�̹� ���簡 �ִ°��� �˰� �����Ƿ� �̰����� �ü��� ����
					m_nRunStep = 3400; //james 2016.0929 1000;
				}
			}
			break;

		case 3500:
			nRet_1 = Process_DVC_Place(BIN_RETESTBIN, m_nBinSort_THD_Work_Site, st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][2], st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][3]); //�� ���� ���� ����, ��ε� ���� �Ǵ� ���׽�Ʈ ���ۿ� ���縦 ���� ���´� 
			
			if(nRet_1 == RET_GOOD)
			{					
				m_nRunStep = 3600;
			}
			else if(nRet_1 == RET_PICKER_NOT_FIND) //��Ŀ�� ���̻� �۾��� ������ ����.  
			{
				m_nRunStep = 3500;			
			}
			else if(nRet_1 == RET_TRAY_NOT_FIND) //�׽��Ϳ��� �� �̻� �۾��� �� �ִ� ������ ����
			{
				
				m_nRunStep = 3500;  
			}
			break; 
		///////////////////////////////////////////////////

		// ��ε� ���縦 ��� ó���� ����
		// lot end ������ Ȯ���Ѵ�
		///////////////////////////////////////////////////
		case 3600:
			nRetestExistCnt=0;
			for(i = 0; i < TEST_PICKER_PARA; i++) //test site���� �۾��� ������ Ȯ���Ѵ�
			{					 
				if(st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == CTL_YES)// && st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable == CTL_YES)
				{
					 nRetestExistCnt++;
				} 
			}
			if(nRetestExistCnt > 0) //���簡 ���������� 
			{
				m_nRunStep = 3000;  
				break;
			}
			else// if(nRetestExistCnt == 0)
			{
				m_nRunStep = 1000;  
				break;		
			}
			break;
			/* james 2016.0909 
			//���� lot�� lot end �������̰�, ó�� �ε� ���簡 ���̻� ������ �׽�Ʈ ����Ʈ�� Next Lot�� ���� Test Site #1 ~#4 ������ ��� ����� ������ Next Lot ������ �����Ѵ� 
			if(st_lot_info[LOT_CURR].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END_START && st_lot_info[LOT_NEXT].nLotStatus == LOT_START) 
			{
				if(st_sync_info.nTestSite_Target_THD_Work_Site[1] >= THD_TESTSITE_1 && st_sync_info.nTestSite_Target_THD_Work_Site[1] <= THD_TESTSITE_4) //�ε� lot end �̰�, ���� �׽����� �����ϸ� NEXT LOT���� �����Ͽ� ����Ѵ�. 
				{//�ش� �׽�Ʈ ����Ʈ�� ���簡 ��� ������ 
					nCoun = 0;
					for(nTHD_i = 0; nTHD_i <  8; nTHD_i++)
					{
						for(i = 0; i < TEST_SOCKET_PARA; i++) //x ���� ���� ���� ����(8��, 0~7��)�� �����Ѵ� 
						{//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].strLotNo == strLotNo && 
							if(st_test_site_info[nTHD_i].strLotNo ==  st_lot_info[LOT_CURR].strLotNo && st_test_site_info[nTHD_i].st_pcb_info[i].nYesNo == CTL_YES)
							{
								nCoun++;
							}
						}
					}
					 
					for(i = 0; i <  15; i++)  //���׽�Ʈ ���ۿ� ���� ���縦 Ȯ���Ѵ�
					{
						if(st_buffer_info[st_sync_info.nRetestBuff_Traget_THD_Work_Site].st_pcb_info[i].nYesNo == CTL_YES) 
						{
							nCoun++;
						}
					}

					//////////////////////////////////////////////////////////////////////////////////////////////////////////
					// ���� ��� �׽�Ʈ�� LOT_CURR �� ����� �ϳ��� ���������ʴ� 
					// Test Robot ���� ��� ���縦 ó���Ͽ� test site lot������ ��� NEXT LOT ������ ��ü�Ѵ�
					//////////////////////////////////////////////////////////////////////////////////////////////////////////
					if(nCoun == 0) 
					{
						 st_lot_info[LOT_CURR].nLot_THD_Status[THD_TEST_RBT] =  LOT_END_START;

						 st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].strLotNo = st_lot_info[LOT_NEXT].strLotNo; //���̻� CURR_LOT�� ������ ��ü LOT END ���̶� �׽�Ʈ ����Ʈ�� LOT ������ NEXT LOT���� ��� �����Ѵ�
					 
					}
				}
			}

			m_nRunStep = 1000;  
			break;	*/	

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//james 2016.0816 ���׽�Ʈ ��� ����
//���� pick & place ��� (��Ŀ�� ��ü Up/Down���� ���۽� ���簣 �浹�� �߻��� �� �־� ���� ���ǰ� �ʿ��ϴ� //
//1�� �۾����� : 0,2,4,6, 8,10,12,14 - 8�� 
//2�� �۾����� : 1,3,5,7, 9,11,13    - 7��
//
//���׽�Ʈ ���ۻ����: �� LOT�� �ϳ��� ���׽�Ʈ ���۸� ����Ѵ�, ����LOT ó�� ����
// Left/Right Tets Site���� ������ ������ PIN ������ �ݴ뿩�� ��� ����Ʈ���� �°����� Ȯ���Ͽ� ȸ���ϸ鼭 �޾ƾ� �Ѵ� 
// �̶� ���� ���۴� L/R ���о��� �ش� lot�� �Ѱ��� ��� �޾�, ó���Ѵ�.    
//
// PLACE ��� ������.
			//1����
// 1. 1�� �۾�����(¦����) ������ ������ �ѹ��� ���� ������ ã�� ���´�. 
// 2. 2�� �۾�����(Ȧ����) ���� ���縦 ���� ��Ȳ�̸� , �̰� ������ ����, ������ 1�� �۾� ������ ���縦 ���׽�Ʈ�� �����Ѵ� 
// 3. �� ����Ŭ ���� ��ε� ������, Ȥ�� Ȧ������ ���׽�Ʈ ���ۿ� ���簡 ������ �� ���縦 ¦������ �ִ��� ������ �ش� 
// 4. �� Ȧ������ �ӽ� ���� �������� ������ ����� �־�� �Ѵ� 

		//2����
//1��(¦����),2��(Ȧ����) �۾� ���� ������� ��� ������ �����Ѵ�
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////////////////////
		// RETEST Buffer DVC PICK
		// retest bin buffer�� ���縦 ���������� ȿ�������� ���� ����� ������� 
		////////////////////////////////////////////////////////////////////////////////////////
		case 5000:
			//james 2016.0929 			
			if(m_nRetestBuffer_PickPlace_Mode == 1)
			{
				m_nRunStep = 5600;  
				break;
			}
		

			//�۾��� �׽�Ʈ����Ʈ�� lot ������ retest buffer �۾� lot ������ Ȯ���Ͽ� �۾��� ���۸� Ȯ���Ѵ�, ������ �̹� ã�� �´� 
			/*if(st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].strLotNo == st_buffer_info[THD_RETEST_1_BUFF].strLotNo) //retest buffer 1 �� �����۾��� �׽�Ʈ ����Ʈ lot ������ ������ �� lot�� �ش��ϴ� ���縦 ó���Ѵ� 
			{
				st_sync_info.nRetestBuff_Traget_THD_Work_Site = THD_RETEST_1_BUFF;
			}
			else if(st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].strLotNo == st_buffer_info[THD_RETEST_2_BUFF].strLotNo)//retest buffer 2 �� �����۾��� �׽�Ʈ ����Ʈ lot ������ ������ �� lot�� �ش��ϴ� ���縦 ó���Ѵ� 
			{
				st_sync_info.nRetestBuff_Traget_THD_Work_Site = THD_RETEST_2_BUFF;
			}*/
		
			nTHD_ID = st_sync_info.nTestSite_Target_THD_Work_Site[2]; 
			nFixPos = st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][3]; //2016.0906 st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3];// st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nStart_FixPos;		
			for(i = 0; i < TEST_PICKER_PARA; i++) //4, test site���� �۾��� ������ Ȯ���Ѵ�
			{
				m_npPicker_YesNo[i] = CTL_NO;//clear
				if(st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == CTL_NO && st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable == CTL_YES)
				{
					if(st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][2]  == TESTSITE_LEFT) //st_test_site_info[nTHD_ID].nSite_Pos == TESTSITE_LEFT) //test socket left ������������ :7->6->5->4  3->2->1->0
					{ 
						nPos = ((TEST_SOCKET_PARA - 1) - nFixPos) - i;
						if(st_test_site_info[nTHD_ID].st_pcb_info[nPos].nYesNo == CTL_NO && st_test_site_info[nTHD_ID].st_pcb_info[nPos].nEnable == CTL_YES)
						{
							m_npPicker_YesNo[i] = CTL_YES;
						}
						else
						{
							st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable = CTL_NO; //��Ŀ���� 
						}
					}
					else if(st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][2] == TESTSITE_RIGHT) // st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].nSite_Pos == TESTSITE_RIGHT) //test socket right ������������ :0->1->2->3  4->5->6->7
					{ 
						nPos = nFixPos + i;
						if(st_test_site_info[nTHD_ID].st_pcb_info[nPos].nYesNo == CTL_NO && st_test_site_info[nTHD_ID].st_pcb_info[nPos].nEnable == CTL_YES)
						{
							m_npPicker_YesNo[i] = CTL_YES;
						}
						else
						{
							st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable = CTL_NO; //��Ŀ���� 
						}
					}
				}
			}//for

			//m_npPicker_YesNo[i] == CTL_YES �� ������ ã�� �� �´� ������ ������ �װ��� ���� ������ �۾��Ѵ� 
			//clsFunc.Find_RetestBinBuffer_WorkPos_Check(int n_Mode, int n_TestSite, int n_BuffSite, int n_PickerDvcYesNO,int n_BuffYesNO, int *np_Picker_YesNo, int *np_FindPosYXCPB,  int *np_BuffInfo_Count)  
			nRet_1 = clsFunc.Find_RetestBinBuffer_WorkPos_Check(0, st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][2], st_sync_info.nRetestBuff_Traget_THD_Work_Site, CTL_YES, CTL_YES, m_npPicker_YesNo, m_npFindWorkPosYXCPB, m_np_BuffInfo_Count);
			
			if(nRet_1 == RET_GOOD)
			{ 
				m_nFirstTray_Y_Num =  m_npFindWorkPosYXCPB[0];// m_npFindWorkPosYXCPB[4]�� ���� 
				m_nFirstTray_X_Num =  m_npFindWorkPosYXCPB[1];// m_npFindWorkPosYXCPB[4]�� ���� 
				m_nFirstPicker_Num = m_npFindWorkPosYXCPB[3]; //nFirstPicker_Num
				m_nFirst_Buff_Num  = m_npFindWorkPosYXCPB[4]; //nSortBuffer_First

				m_nPartial_FirstRetestBuffPos_Select_No = m_nFirst_Buff_Num;
				m_nRunStep = 5200;//���� ��ġ�� ���縦 ���� �۾��� �׽�Ʈ ����Ʈ�� �̵��Ѵ�
			}
			else if(nRet_1 == RET_SKIP)  
			{	//��Ŀ ������ �� �´� ���׽�Ʈ ������ ���� ������ ���� 	
				//������ ������ Ȯ���Ͽ� , �� ��ġ�� �������� ��� ������ �κи� �� ���� ���縦 ���� �� ó������.
			 
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// Partial retest buffer�� ����� �� ���� ���� �׽�Ʈ ����Ʈ ������ ������ �κ��� ���� �� �κ��� ���縦 ���� �׽�Ʈ �׽�Ʈ ����Ʈ�� �̵��Ѵ�
				// �켱 �������� ���߿� �ʿ�� ���� 
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				nMaxCnt=0;
				if(st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2]  == TESTSITE_LEFT)
				{
					for(i = st_recipe_info.nRetestBuffer_Num - 1; i >= 0; i--)// st_recipe_info.nRetestBuffer_Num; i++)//15
					{
						nRetest_Cnt = m_np_BuffInfo_Count[i] ;
						if(nMaxCnt < nRetest_Cnt)
						{
							nMaxCnt = nRetest_Cnt;
							m_nFirst_Buff_Num = i; //���� ���� ���� �ٻ�ġ�� ������ ������ ������ �̰��� ���� ��ġ�� �۾��Ѵ� 
						}
					}
				}
				else if(st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2]  == TESTSITE_RIGHT)
				{
					for(i = 0; i < st_recipe_info.nRetestBuffer_Num; i++)//15
					{
						nRetest_Cnt = m_np_BuffInfo_Count[i] ;
						if(nMaxCnt < nRetest_Cnt)
						{
							nMaxCnt = nRetest_Cnt;
							m_nFirst_Buff_Num = i; //���� ���� ���� �ٻ�ġ�� ������ ������ ������ �̰��� ���� ��ġ�� �۾��Ѵ� 
						}
					}
				}				
				m_nPartial_FirstRetestBuffPos_Select_No = m_nFirst_Buff_Num; //partial ���׽�Ʈ ������ ������ ���縦 �����ϱ� ���� ������ ���� ��ġ 

				nFlag = CTL_NO;
				for(i = 0; i < TEST_PICKER_PARA; i++) //test site���� �۾��� ������ Ȯ���Ѵ�
				{
					m_npPicker_YesNo[i] = CTL_NO;//clear
					if(st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == CTL_NO && st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable == CTL_YES) //��Ŀ���� 
					{
						if(st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2]  == TESTSITE_LEFT) // st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos == TESTSITE_LEFT) //test socket left ������������ :7->6->5->4  3->2->1->0
						{ 
							nPos = m_nPartial_FirstRetestBuffPos_Select_No - (i * 2);
							if(st_buffer_info[st_sync_info.nRetestBuff_Traget_THD_Work_Site].st_pcb_info[nPos].nYesNo == CTL_YES) 
							{
								//nPos = ((TEST_SOCKET_PARA - 1) - nFixPos) - i; //(i * 2);
								if(st_test_site_info[nTHD_ID].st_pcb_info[((TEST_SOCKET_PARA - 1) - nFixPos) - i].nYesNo == CTL_NO
									&& st_test_site_info[nTHD_ID].st_pcb_info[((TEST_SOCKET_PARA - 1) - nFixPos) - i].nEnable == CTL_YES)
								{
									if(nFlag == CTL_NO)
									{
										m_nFirstPicker_Num = i;
										nFlag = CTL_YES;
									} 	
									m_npPicker_YesNo[i] = CTL_YES;		 
								}
							}
						}
						else //if(st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2]  == TESTSITE_RIGHT) //st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].nSite_Pos == TESTSITE_RIGHT) //test socket right ������������ :0->1->2->3  4->5->6->7
						{ 
							nPos = m_nPartial_FirstRetestBuffPos_Select_No + (i * 2);
							if(st_buffer_info[st_sync_info.nRetestBuff_Traget_THD_Work_Site].st_pcb_info[nPos].nYesNo == CTL_YES) 
							{								
								 if(st_test_site_info[nTHD_ID].st_pcb_info[nFixPos + i].nYesNo == CTL_NO 
									 && st_test_site_info[nTHD_ID].st_pcb_info[nFixPos + i].nEnable == CTL_YES)
								 {						 						
									 if(nFlag == CTL_NO)
									 {
										 m_nFirstPicker_Num = i;
										 nFlag = CTL_YES;
									 } 
									 m_npPicker_YesNo[i] = CTL_YES; 							 
								 }
							}
						}
					}//if
				}//for

				if(nFlag == CTL_YES)
				{
					//���� ���縸 ���� ����, �ٽ� ó�� ���� ��ġ�� ���縦 �߰��� ���� �׽�Ʈ ����Ʈ�� �̵��Ѵ�.
					m_nFirstTray_Y_Num = m_nPartial_FirstRetestBuffPos_Select_No;// m_npFindWorkPosYXCPB[4]�� ���� 
					m_nFirstTray_X_Num = m_nPartial_FirstRetestBuffPos_Select_No;// m_npFindWorkPosYXCPB[4]�� ���� 
					m_nFirstPicker_Num = m_nFirstPicker_Num; 
					m_nFirst_Buff_Num  = m_nPartial_FirstRetestBuffPos_Select_No; 

					m_npFindWorkPosYXCPB[0] = m_nFirstTray_Y_Num; 
					m_npFindWorkPosYXCPB[1] = m_nFirstTray_X_Num; 
					m_npFindWorkPosYXCPB[3] = m_nFirstPicker_Num;
					m_npFindWorkPosYXCPB[4] = m_nFirst_Buff_Num;

					m_nRunStep = 5200;//���� ��ġ�� ���縦 ���� �۾��� �׽�Ʈ ����Ʈ�� �̵��Ѵ�
				}
				else
				{//�̹� ���簡 �ִ°��� �˰� �����Ƿ� �̰����� �ü��� ����
					m_nRunStep = 5600; //james 2016.0929 1000;
				}
			}
			break;

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// retest bin�� ���縦 ���� mode �̰�,
		// m_nFirstPicker_Num, m_nFirst_Buff_Num  ��ġ�� �̹� �����Ͽ� �켱 �Ʒ� PICK() �Լ� ���� �� �κ��� ã�� �������ϴ� Step �κ��� ��ŵ�Ǿ�� �Ѵ� 
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		case 5200:
			st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][0] = CTL_REQ;
			st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][1] = WORK_PICK; //���縦 ���� �� �ְ� ��û 
			st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][2] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2]; //st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos;

			//st_buffer_info[st_sync_info.nRetestBuff_Traget_THD_Work_Site].nTestSIte_No            = st_sync_info.nTestSite_Target_THD_Work_Site[2];
			//st_buffer_info[st_sync_info.nRetestBuff_Traget_THD_Work_Site].nTestSite_LR_Pos        = st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //�ε� ������ �׽��Ϳ� �´� ȸ�� ������ ���Ѵ�
			//st_buffer_info[st_sync_info.nRetestBuff_Traget_THD_Work_Site].nTestSite_Start_FixPos  = st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nStart_FixPos;
			m_nRunStep = 5300;
			break;

		case 5300: //���۰� �ִϵǱ⸦ ��ٸ��� 
			if(st_sync_info.nRetestBuff_Traget_THD_Work_Site == THD_RETEST_1_BUFF || st_sync_info.nRetestBuff_Traget_THD_Work_Site == THD_RETEST_2_BUFF) 
			{
				if(st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][0] == CTL_READY && st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][1] == WORK_PICK)
				{//retest buffer�� �ε��� ���簡 �غ�Ǿ� �ִ� 
					m_nRunStep = 5400;  
				}
			}
			
			/* james 2016.0918 
			if(m_nBinSort_THD_Work_Site == THD_RETEST_1_BUFF || m_nBinSort_THD_Work_Site == THD_RETEST_2_BUFF) 
			{
				if(st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][0] == CTL_READY && st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][1] == WORK_PICK)
				{//retest buffer�� �ε��� ���簡 �غ�Ǿ� �ִ� 
					m_nRunStep = 5400;  
				}
			}*/
			break;

		case 5400://���׽�Ʈ ���縦 ���´�
			nRet_1 = Process_DVC_Pick(BIN_RETESTBIN, st_sync_info.nRetestBuff_Traget_THD_Work_Site, st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2], st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3]);//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nStart_FixPos);
			
			if(nRet_1 == RET_GOOD)
			{					
				m_nRunStep = 7000;
			}
			else if(nRet_1 == RET_PICKER_NOT_FIND) //��Ŀ�� ���̻� �۾��� ������ ����.  
			{
				m_nRunStep = 5400;			
			}
			else if(nRet_1 == RET_TRAY_NOT_FIND) //�׽��Ϳ��� �� �̻� �۾��� �� �ִ� ������ ����
			{
				
				m_nRunStep = 5400;  
			}
			break;

//////////////////////////////////////////////////////////////////////
		//james 2016.0929 
			//retest buffer pick & Place  ��� ���� 
//////////////////////////////////////////////////////////////////////
		case 5600:

			nTHD_ID = st_sync_info.nTestSite_Target_THD_Work_Site[2]; 
			nFixPos = st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][3]; //2016.0906 st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3];// st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nStart_FixPos;		
			//break;

			//Find_RetestBuffer_PickPlace_WorkPos_Check(int n_PickPlace_Mode, int n_TestNum, int n_TestFixPos, int n_BuffSite, int *np_Picker_YesNo, int *np_FindPosYXCPB,  int *np_BuffInfo_Count)//james 2016.0928 retest buffer �۾����� ��ġ ã�� 
			nRet_1 = clsFunc.Find_RetestBuffer_PickPlace_WorkPos_Check(WORK_PICK, nTHD_ID, nFixPos, st_sync_info.nRetestBuff_Traget_THD_Work_Site, m_npPicker_YesNo, m_npFindWorkPosYXCPB, m_np_BuffInfo_Count);//james 2016.0928 retest buffer �۾����� ��ġ ã�� 
			if(nRet_1 == RET_GOOD)
			{ 
				m_nFirstTray_Y_Num =  m_npFindWorkPosYXCPB[0];// m_npFindWorkPosYXCPB[4]�� ���� 
				m_nFirstTray_X_Num =  m_npFindWorkPosYXCPB[1];// m_npFindWorkPosYXCPB[4]�� ���� 
				m_nBuffer_Work_Cnt =  m_npFindWorkPosYXCPB[2];//james 2016.0929 
				m_nFirstPicker_Num = m_npFindWorkPosYXCPB[3]; //nFirstPicker_Num
				m_nFirst_Buff_Num  = m_npFindWorkPosYXCPB[4]; //nSortBuffer_First

				m_nPartial_FirstRetestBuffPos_Select_No = m_nFirst_Buff_Num;
				m_nRunStep = 5700;//���� ��ġ�� ���縦 ���� �۾��� �׽�Ʈ ����Ʈ�� �̵��Ѵ�
			}
			else if(nRet_1 == RET_ABORT)  //james 2016.0929 �۾��� ���簡 ���� 
			{
				nCount = 0; 
				for(i = 0; i < TEST_PICKER_PARA; i++)//st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == n_DvcYesNO && (n_DvcYesNO == CTL_YES && (n_DvcYesNO == CTL_YES || (n_PickerYesNO == CTL_NO && st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable == CTL_YES)) ) )
				{
					if(st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == CTL_YES)
					{
						nCount++;
					}
				}

				//m_nRunStep = 7000;
				//2017.0224
				if( nCount > 0 )
				{
					m_nRunStep = 7000;
				}
				else
				{
					strTemp.Format(_T("Can not Find Insert to TestSite"));
					clsMem.OnAbNormalMessagWrite(strTemp);//�α� ����
					m_nRunStep = 1000;
				}
			}
			else if(nRet_1 == RET_SKIP)  
			{	//��Ŀ ������ �� �´� ���׽�Ʈ ������ ���� ������ ���� 	
				//������ ������ Ȯ���Ͽ� , �� ��ġ�� �������� ��� ������ �κи� �� ���� ���縦 ���� �� ó������.

				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// Partial retest buffer�� ����� �� ���� ���� �׽�Ʈ ����Ʈ ������ ������ �κ��� ���� �� �κ��� ���縦 ���� �׽�Ʈ �׽�Ʈ ����Ʈ�� �̵��Ѵ�
				// �켱 �������� ���߿� �ʿ�� ���� 
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				nMaxCnt=0;
				if(st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2]  == TESTSITE_LEFT)
				{
					for(i = st_recipe_info.nRetestBuffer_Num - 1; i >= 0; i--)// st_recipe_info.nRetestBuffer_Num; i++)//15
					{
						nRetest_Cnt = m_np_BuffInfo_Count[i] ;
						if(nMaxCnt < nRetest_Cnt)
						{
							nMaxCnt = nRetest_Cnt;
							m_nFirst_Buff_Num = i; //���� ���� ���� �ٻ�ġ�� ������ ������ ������ �̰��� ���� ��ġ�� �۾��Ѵ� 
						}
					}
				}
				else if(st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2]  == TESTSITE_RIGHT)
				{
					for(i = 0; i < st_recipe_info.nRetestBuffer_Num; i++)//15
					{
						nRetest_Cnt = m_np_BuffInfo_Count[i] ;
						if(nMaxCnt < nRetest_Cnt)
						{
							nMaxCnt = nRetest_Cnt;
							m_nFirst_Buff_Num = i; //���� ���� ���� �ٻ�ġ�� ������ ������ ������ �̰��� ���� ��ġ�� �۾��Ѵ� 
						}
					}
				}				
				m_nPartial_FirstRetestBuffPos_Select_No = m_nFirst_Buff_Num; //partial ���׽�Ʈ ������ ������ ���縦 �����ϱ� ���� ������ ���� ��ġ 

				nFlag = CTL_NO;
				int nPickerNum = 0;
				for(i = 0; i < TEST_PICKER_PARA; i++) 
				{
					if(st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == CTL_NO && st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable == CTL_YES) //��Ŀ���� 
					{
						nPickerNum = i;
						break;
					}
				}
				for(i = 0; i < TEST_PICKER_PARA; i++) //test site���� �۾��� ������ Ȯ���Ѵ�
				{
					m_npPicker_YesNo[i] = CTL_NO;//clear
					if(st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == CTL_NO && st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable == CTL_YES) //��Ŀ���� 
					{
						if(st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2]  == TESTSITE_LEFT) // st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos == TESTSITE_LEFT) //test socket left ������������ :7->6->5->4  3->2->1->0
						{
							nPos = m_nPartial_FirstRetestBuffPos_Select_No - ((i-nPickerNum) * 2);
							if(st_buffer_info[st_sync_info.nRetestBuff_Traget_THD_Work_Site].st_pcb_info[nPos].nYesNo == CTL_YES) //2016.1011
							{
								//nPos = ((TEST_SOCKET_PARA - 1) - nFixPos) - i; //(i * 2);
								if(st_test_site_info[nTHD_ID].st_pcb_info[((TEST_SOCKET_PARA - 1) - nFixPos) - i].nYesNo == CTL_NO
									&& st_test_site_info[nTHD_ID].st_pcb_info[((TEST_SOCKET_PARA - 1) - nFixPos) - i].nEnable == CTL_YES)
								{
									if(nFlag == CTL_NO)
									{
										m_nFirstPicker_Num = i;
										nFlag = CTL_YES;
									} 	
									m_npPicker_YesNo[i] = CTL_YES;		 
								}
							}
						}
						else //if(st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2]  == TESTSITE_RIGHT) //st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].nSite_Pos == TESTSITE_RIGHT) //test socket right ������������ :0->1->2->3  4->5->6->7
						{ 
 							nPos = m_nPartial_FirstRetestBuffPos_Select_No + ((i-nPickerNum) * 2);
							if( st_buffer_info[st_sync_info.nRetestBuff_Traget_THD_Work_Site].st_pcb_info[nPos].nYesNo == CTL_YES) //2016.1011
							{								
								if(st_test_site_info[nTHD_ID].st_pcb_info[nFixPos + i].nYesNo == CTL_NO 
									&& st_test_site_info[nTHD_ID].st_pcb_info[nFixPos + i].nEnable == CTL_YES)
								{						 						
									if(nFlag == CTL_NO)
									{
										m_nFirstPicker_Num = i;
										nFlag = CTL_YES;
									} 
									m_npPicker_YesNo[i] = CTL_YES; 							 
								}
							}
						}
					}//if
				}//for

				if(nFlag == CTL_YES)
				{
					//���� ���縸 ���� ����, �ٽ� ó�� ���� ��ġ�� ���縦 �߰��� ���� �׽�Ʈ ����Ʈ�� �̵��Ѵ�.
					m_nFirstTray_Y_Num = m_nPartial_FirstRetestBuffPos_Select_No;// m_npFindWorkPosYXCPB[4]�� ���� 
					m_nFirstTray_X_Num = m_nPartial_FirstRetestBuffPos_Select_No;// m_npFindWorkPosYXCPB[4]�� ���� 
					m_nFirstPicker_Num = m_nFirstPicker_Num; 
					m_nFirst_Buff_Num  = m_nPartial_FirstRetestBuffPos_Select_No; 

					m_npFindWorkPosYXCPB[0] = m_nFirstTray_Y_Num; 
					m_npFindWorkPosYXCPB[1] = m_nFirstTray_X_Num; 
					m_npFindWorkPosYXCPB[3] = m_nFirstPicker_Num;
					m_npFindWorkPosYXCPB[4] = m_nFirst_Buff_Num;

					m_nRunStep = 5700;//���� ��ġ�� ���縦 ���� �۾��� �׽�Ʈ ����Ʈ�� �̵��Ѵ�
				}
				else
				{//�̹� ���簡 �ִ°��� �˰� �����Ƿ� �̰����� �ü��� ����
					m_nRunStep = 5600; //james 2016.0929 1000;
				}
			}
			break;

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// retest bin�� ���縦 ���� mode �̰�,
		// m_nFirstPicker_Num, m_nFirst_Buff_Num  ��ġ�� �̹� �����Ͽ� �켱 �Ʒ� PICK() �Լ� ���� �� �κ��� ã�� �������ϴ� Step �κ��� ��ŵ�Ǿ�� �Ѵ� 
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		case 5700:
			st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][0] = CTL_REQ;
			st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][1] = WORK_PICK; //���縦 ���� �� �ְ� ��û 
			st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][2] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2]; //st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos;

			//st_buffer_info[st_sync_info.nRetestBuff_Traget_THD_Work_Site].nTestSIte_No            = st_sync_info.nTestSite_Target_THD_Work_Site[2];
			//st_buffer_info[st_sync_info.nRetestBuff_Traget_THD_Work_Site].nTestSite_LR_Pos        = st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //�ε� ������ �׽��Ϳ� �´� ȸ�� ������ ���Ѵ�
			//st_buffer_info[st_sync_info.nRetestBuff_Traget_THD_Work_Site].nTestSite_Start_FixPos  = st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nStart_FixPos;
			m_nRunStep = 5800;
			break;

		case 5800: //���۰� �ִϵǱ⸦ ��ٸ��� 
			if(st_sync_info.nRetestBuff_Traget_THD_Work_Site == THD_RETEST_1_BUFF || st_sync_info.nRetestBuff_Traget_THD_Work_Site == THD_RETEST_2_BUFF) 
			{
				if(st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][0] == CTL_READY && st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][1] == WORK_PICK)
				{//retest buffer�� �ε��� ���簡 �غ�Ǿ� �ִ� 
					m_nRunStep = 5900;  
				}
			}
			break;

		case 5900://���׽�Ʈ ���縦 ���´�
			//james 2016.0930 nRet_1 = Process_DVC_Pick(BIN_RETESTBIN, st_sync_info.nRetestBuff_Traget_THD_Work_Site, st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2], st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3]);//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nStart_FixPos);
			nRet_1 = Process_DVC_Pick(BIN_RETESTBIN, st_sync_info.nRetestBuff_Traget_THD_Work_Site, st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2], st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3]);
			
			if(nRet_1 == RET_GOOD)
			{			
				//james 2016.0929 m_nRunStep = 7000;

				nCount = 0; 
				for(i = 0; i < TEST_PICKER_PARA; i++)//st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == n_DvcYesNO && (n_DvcYesNO == CTL_YES && (n_DvcYesNO == CTL_YES || (n_PickerYesNO == CTL_NO && st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable == CTL_YES)) ) )
				{
					if(st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == CTL_YES)
					{
						nCount++;
					}
				}

				//if(m_np_BuffInfo_Count[16] <= nCount || nCount >= TEST_PICKER_PARA )
				if(nCount >= TEST_PICKER_PARA )
				{
					m_nRunStep = 7000;
				}
				else if(m_np_BuffInfo_Count[17] == 0)
				{
					m_nRunStep = 7000;
				}
				else 
				{
					m_nRunStep = 5600; //���� ���縦 ���´� 
					//2017.1206  üũ�ٶ�
					if( nCount > 0 && st_basic_info.nRetest1Pick_Mode == CTL_YES)
					{
						m_nRunStep = 7000;
					}
				}
			}
			else if(nRet_1 == RET_PICKER_NOT_FIND) //��Ŀ�� ���̻� �۾��� ������ ����.  
			{
				m_nRunStep = 5900;			
			}
			else if(nRet_1 == RET_TRAY_NOT_FIND) //�׽��Ϳ��� �� �̻� �۾��� �� �ִ� ������ ����
			{
				
				m_nRunStep = 5900;  
			}
			break;

		
		///////////////////////////////////////////////////////////////////////////////////////
		// LOAD BUFFER ���� PICK ���� TEST SITE�� �̵��Ͽ� TEST DVC INSERT ���� �����Ѵ� 
		//1.�ε� ���縦 ���� �׽�Ʈ ������ ������ ��ġ�� �����Ѵ�  
		//2.�ش� �׽�Ʈ ����Ʈ�� ���Ͽ� ���� ��ŭ ��� �־����� �׽��Ͱ� �׽�Ʈ �� �� �ְ� ������ ���Ѵ�   
		//////////////////////////////////////////////////////////////////////////////////////
		case 6000:
			m_nMove_Flag[0] = 0;
			m_nRunStep = 6010;
			break;

		case 6010:

			nRet_3 = clsFunc.Find_TrayWork_Pos(WORK_PICK, THD_LD_BUFF, CTL_YES, m_npFindWorkPosYXCPB, THD_TEST_RBT, m_strFindLotNo);

			if(nRet_3 == RET_GOOD && m_npFindWorkPosYXCPB[2] > 0 && st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_READY && st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][1] == WORK_PICK)
			{//load buffer�� �ε��� ���簡 �غ�Ǿ� �ִ� 
				//james 2016.1004 m_nRunStep = 6100;  

				////////////////////////////////////////////////////////////////////////////////////////
				// james 2016.100 Ȥ�� �ε� ���������� ������ Ʋ���� ��������, ������ ã�� ��???
				// �ε� ������ �غ� �̸� Ȯ���Ѵ� 
				////////////////////////////////////////////////////////////////////////////////////////
				if(st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2] == TESTSITE_LEFT) //LEFT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ���������� 3,2,1,0 �� �ݴ��̴� 
				{
					nRet_1 = COMI.Check_MotPosRange(M_LD_BUF_SHIFTER_Y, st_motor_info[M_LD_BUF_SHIFTER_Y].d_pos[P_LDBUFF_Y_TESTRBT], st_motor_info[M_LD_BUF_SHIFTER_Y].d_allow);
					nRet_2 = COMI.Check_MotPosRange(M_LD_BUF_ROTATOR, st_motor_info[M_LD_BUF_ROTATOR].d_pos[P_LDBUFF_RTR_TESTSITE_LEFT], st_motor_info[M_LD_BUF_ROTATOR].d_allow);
					if(nRet_1 != BD_GOOD || nRet_2 != BD_GOOD)
					{
// 						if(nRet_1 != BD_GOOD ) m_strAlarmCode.Format(_T("%02d0006"), M_LD_BUF_SHIFTER_Y); 
// 						else m_strAlarmCode.Format(_T("%02d0006"), M_LD_BUF_ROTATOR); 
// 						CTL_Lib.Alarm_Error_Occurrence(m_nPlace_Step, dWARNING, m_strAlarmCode);
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_REQ;
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][1] = 300; //james 2016.1005 WORK_PICK; //���縦 ���� �� �ְ� ��û 
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][2] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2];//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //�ε� ������ �׽��Ϳ� �´� ȸ�� ������ ���Ѵ�
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][3] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3]; //james 2016.0924 �߰� 
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][4] = st_sync_info.nTestSite_Target_THD_Work_Site[2]; //test site num

						//2017.0319
						strTemp.Format(_T("[6010TESTSITE_LEFT_LD_BUFF_REQ] Work_Site : %d Start_FixPos : %d"), st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][4] , st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][3]);
						clsMem.OnNormalMessageWrite(strTemp);
						break;
					} 
				}
				else if(st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2] == TESTSITE_RIGHT) //right sitet�� �۾��ϱ� ���� �۾� ���϶�//RIGHT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ����������0,1,2,3 �� ��ġ ������ �����ϴ�  
				{
					nRet_1 = COMI.Check_MotPosRange(M_LD_BUF_SHIFTER_Y, st_motor_info[M_LD_BUF_SHIFTER_Y].d_pos[P_LDBUFF_Y_TESTRBT], st_motor_info[M_LD_BUF_SHIFTER_Y].d_allow);
					nRet_2 = COMI.Check_MotPosRange(M_LD_BUF_ROTATOR, st_motor_info[M_LD_BUF_ROTATOR].d_pos[P_LDBUFF_RTR_TESTSITE_RIGHT], st_motor_info[M_LD_BUF_ROTATOR].d_allow);
					if(nRet_1 != BD_GOOD || nRet_2 != BD_GOOD)
					{
// 						if(nRet_1 != BD_GOOD ) m_strAlarmCode.Format(_T("%02d0006"), M_LD_BUF_SHIFTER_Y); 
// 						else m_strAlarmCode.Format(_T("%02d0006"), M_LD_BUF_ROTATOR); 
// 						CTL_Lib.Alarm_Error_Occurrence(m_nPlace_Step, dWARNING, m_strAlarmCode);
// 						break;
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_REQ;
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][1] = 300; //james 2016.1005 WORK_PICK; //���縦 ���� �� �ְ� ��û 
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][2] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2];//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //�ε� ������ �׽��Ϳ� �´� ȸ�� ������ ���Ѵ�
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][3] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3]; //james 2016.0924 �߰� 
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][4] = st_sync_info.nTestSite_Target_THD_Work_Site[2]; //test site num
						//2017.0319
						strTemp.Format(_T("[6010TESTSITE_RIGHT_LD_BUFF_REQ] Work_Site : %d Start_FixPos : %d"), st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][4] , st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][3]);
						clsMem.OnNormalMessageWrite(strTemp);
						break;
					} 
				}//james 2016.1004 
				else
				{
					// �̰����� ���� ���� ���� ��, ���� ã�� ��
					break;
				}			
				m_nRunStep = 6100;  
				
				////////////////////////////////////////////////////////////////////////
				//2016.1203
				m_dwCycleTime[0][0] = GetTickCount();

			}
			else
			{
				if(m_nMove_Flag[0] == 0)
				{
					m_nMove_Flag[0] = 1;
					m_nRunStep = 6020; 
				}



				//james 2016.0923 loading lot end�� ���� �Ǿ����� �� �̻� �ε� ���縦 ���� ������ ���� 
				//if(st_lot_info[LOT_CURR].nLot_THD_Status[THD_LD_TRAY_PLATE] >=  LOT_END_START )
				//2016.1121
				/*if( st_lot_info[LOT_CURR].nLot_THD_Status[THD_LD_TRAY_PLATE] >= LOT_END_START &&
					st_lot_info[LOT_CURR].nLot_THD_Status[THD_LD_BUFF] >= LOT_END_START && nRet_3 == RET_ERROR )
				{
					m_nRunStep = 1000;  
				}
				else
				{
					// jtkim 20161123
					if (nRet_3 == RET_SKIP)
					{
						m_nRunStep = 1000;  
					}
				}*/
				//2016.1125
				if( (st_lot_info[LOT_CURR].nLot_THD_Status[THD_LD_TRAY_PLATE] >= LOT_END_START &&
					st_lot_info[LOT_CURR].nLot_THD_Status[THD_LD_BUFF] >= LOT_END_START) || (nRet_3 == RET_SKIP))
				{
					m_nRunStep = 1000;
				}
			}
			break;			

		case 6020:
			m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_LDBUF_START];
			m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_LDBUF_START]; // 

			nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate); 
			if (nRet_1 == BD_GOOD) //�������� �̵�
			{				 
				m_nRunStep = 6010;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nRunStep = 6020;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
				CTL_Lib.Alarm_Error_Occurrence(2121, dWARNING, st_alarm_info.strCode);
				m_nRunStep = 6020;
			}
			break; 	

		case 6100:
			//james 2016.0930 nRet_1 = Process_DVC_Pick(0, THD_LD_BUFF, st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2], st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3]);//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos); //�ε� ������ ���縦 ���´�
			nRet_1 = Process_DVC_Pick(0, THD_LD_BUFF, st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2], st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3]);//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos); //�ε� ������ ���縦 ���´�
			
			if(nRet_1 == RET_GOOD)
			{
				//2017.1121 ���� üũ
				if (st_basic_info.nMissLoading_Mode == CTL_YES)
				{
					nRet_1 = FAS_IO.get_in_bit(st_io_info.i_MissLoadingChk1, IO_ON);
					nRet_2 = FAS_IO.get_in_bit(st_io_info.i_MissLoadingChk2, IO_ON);
					if(nRet_1 == IO_OFF || nRet_2 == IO_OFF) 
					{
						if(nRet_1 == IO_OFF ) m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_MissLoadingChk1); //Miss loading ���� ����
						else						   m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_MissLoadingChk2); //Miss loading ���� ����
						CTL_Lib.Alarm_Error_Occurrence(2199, dWARNING, m_strAlarmCode);
					}
				}

				m_nRunStep = 7000;
			}
			else if(nRet_1 == RET_PICKER_NOT_FIND) //��Ŀ�� ���̻� �۾��� ������ ����.  
			{
				m_nRunStep = 6100;
			}
			else if(nRet_1 == RET_TRAY_NOT_FIND) //�׽��Ϳ��� �� �̻� �۾��� �� �ִ� ������ ����
			{				
				m_nRunStep = 6100;  
			}
			break;

		case 6200:
			nSkip = false;
			for( i= 0; i < TEST_PICKER_PARA; i++ )
			{
				m_npPicker_YesNo[i] = CTL_NO;
				nTHD_ID = st_sync_info.nTestSite_Target_THD_Work_Site[2];
				nPos = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2];
				nFixPos = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3];
				if( st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == CTL_YES )
				{
					if( nPos == TESTSITE_LEFT)
					{
						if(st_test_site_info[nTHD_ID].st_pcb_info[((TEST_SOCKET_PARA - 1) - nFixPos) - i].nYesNo == CTL_NO &&
						   st_test_site_info[nTHD_ID].st_pcb_info[((TEST_SOCKET_PARA - 1) - nFixPos) - i].nEnable == CTL_YES)
						{							
						}
						else
						{
							nSkip = true;
							m_npPicker_YesNo[i] = CTL_YES;
							st_picker[THD_TEST_RBT].st_pcb_info[i].nBin = BD_DATA_RETEST;
						}
					}
					else if( nPos == TESTSITE_RIGHT)
					{
						nPos = m_nPartial_FirstRetestBuffPos_Select_No + (i * 2);
								
						if(st_test_site_info[nTHD_ID].st_pcb_info[nFixPos + i].nYesNo == CTL_NO &&
						   st_test_site_info[nTHD_ID].st_pcb_info[nFixPos + i].nEnable == CTL_YES)
						{						 						
						}
						else
						{
							nSkip = true;
							m_npPicker_YesNo[i] = CTL_YES;
							st_picker[THD_TEST_RBT].st_pcb_info[i].nBin = BD_DATA_RETEST;
						}
					}
				}
			}
			if( nSkip == true )
			{
				m_nRunStep = 6300;
			}
			else
			{
				m_nRunStep = 7000;
			}
			break;

		case 6300:
			//test site left/right ���� 

			st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][0] = CTL_REQ;
			st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][1] = WORK_PLACE; //���縦 ���� �� �ְ� ��û 
			st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][2] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2]; // st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].nSite_Pos;
			st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][3] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3]; //james 2016.0626 
			st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][4] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][4];


			nTHD_ID = st_sync_info.nTestSite_Target_THD_Work_Site[2];//st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2];//���� ����
			nFixPos = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3];
			nRet_1 = clsFunc.Find_RetestBuffer_PickPlace_WorkPos_Check(WORK_PLACE, nTHD_ID, nFixPos, st_sync_info.nRetestBuff_Traget_THD_Work_Site, m_npPicker_YesNo, m_npFindWorkPosYXCPB, m_np_BuffInfo_Count);//james 2016.0928 retest buffer �۾����� ��ġ ã�� 
			if(nRet_1 == RET_GOOD)
			{ 
				m_nFirstTray_Y_Num =  m_npFindWorkPosYXCPB[0];// m_npFindWorkPosYXCPB[4]�� ���� 
				m_nFirstTray_X_Num =  m_npFindWorkPosYXCPB[1];// m_npFindWorkPosYXCPB[4]�� ���� 
				m_nBuffer_Work_Cnt =  m_npFindWorkPosYXCPB[2];//james 2016.0929 
				m_nFirstPicker_Num = m_npFindWorkPosYXCPB[3]; //nFirstPicker_Num
				m_nFirst_Buff_Num  = m_npFindWorkPosYXCPB[4]; //nSortBuffer_First

				m_nPartial_FirstRetestBuffPos_Select_No = m_nFirst_Buff_Num;
				m_nRunStep = 6400;//���� ��ġ�� ���縦 ���� �۾��� �׽�Ʈ ����Ʈ�� �̵��Ѵ�
			}
			else if(nRet_1 == RET_ABORT)
			{
				nCount = 0; 
				for(i = 0; i < TEST_PICKER_PARA; i++)
				{
					if(st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == CTL_YES)
					{
						nCount++;
					}
				}

				m_nRunStep = 7000;//�׳� �� ���� �ִ´�. ��¿ �� ����
			}
			break;

		case 6400:
			nRet_1 = Process_DVC_Place(BIN_RETESTBIN, st_sync_info.nRetestBuff_Traget_THD_Work_Site, st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2], st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3], 1);

			if(nRet_1 == RET_GOOD)
			{					
				m_nRunStep = 7000;
			}
			else if(nRet_1 == RET_PICKER_NOT_FIND) //��Ŀ�� ���̻� �۾��� ������ ����.  
			{
				m_nRunStep = 7000;			
			}
			else if(nRet_1 == RET_TRAY_NOT_FIND) //�׽��Ϳ��� �� �̻� �۾��� �� �ִ� ������ ����
			{

				m_nRunStep = 6300;  
			}
			break; 


		//////////////////////////////////////////////////
		//Test site�� ���縦 �μ�Ʈ�Ѵ� 
		//////////////////////////////////////////////////
		case 7000:
			nRet_1 = Process_DVC_Place(0, st_sync_info.nTestSite_Target_THD_Work_Site[2], st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2], st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3]);//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nStart_FixPos);
			if(nRet_1 == RET_GOOD)
			{					
				m_nRunStep = 7100; //james 2016.1003 8000;

				///////////////////////////////////////////////////////////////////////////////////////
				//2016.1203
				m_dwCycleTime[0][1] = GetTickCount();
				m_dwCycleTime[0][2] = m_dwCycleTime[0][1] - m_dwCycleTime[0][0];

				m_strLotNo[0] = st_lot_info[m_nLotProcessNum].strLotNo;
				m_strPartNo[1] = st_lot_info[m_nLotProcessNum].strPartNo;
				if (m_dwCycleTime[0][2] > (DWORD)0)
				{
					clsFunc.OnCycleTime(4, 
							            m_strLotNo[0],
										m_strPartNo[0],
										m_dwCycleTime[0][2], 
										0, 
										0);

					m_strLotNo[0] = m_strPartNo[0] = _T("");

				}
				m_dwPlaceTime[0] = GetTickCount();
				///////////////////////////////////////////////////////////////////////////////////////

			}
			else if(nRet_1 == RET_PICKER_NOT_FIND) //��Ŀ�� ���̻� �۾��� ������ ����.  
			{
				m_nRunStep = 7000; //james 2016.0926 pick & place �ݺ� m_nRunStep = 1000; //james 2016.0924 7000;			
				
			}
			else if(nRet_1 == RET_TRAY_NOT_FIND) //�׽��Ϳ��� �� �̻� �۾��� �� �ִ� ������ ����
			{
				
				m_nRunStep = 7000;  
			}
			break;

		case 7100:
			nRetestExistCnt=0;
			for(i = 0; i < TEST_PICKER_PARA; i++) //test site���� �۾��� ������ Ȯ���Ѵ�
			{					 
				if(st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == CTL_YES)// && st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable == CTL_YES)
				{
					st_picker[THD_TEST_RBT].st_pcb_info[i].nBin = BD_DATA_RETEST; // 2016.1005 
					nRetestExistCnt++;
				} 
			}
			if(nRetestExistCnt > 0) //���簡 ���������� 
			{
				m_nRunStep = 3000; // 2016.1005 7000;  
				break;
			}
			else// if(nRetestExistCnt == 0)
			{
				m_nRunStep = 8000;  
				break;		
			}
			break;
		
		//////////////////////////////////////////////////////////////////////////////////////
		//Test site�� ���縦 �־��, �� ����Ʈ�� ��� ó�������� �׽�Ʈ START�� ���� �Ѵ�  
		//////////////////////////////////////////////////////////////////////////////////////
		case 8000:
			nRet_1 = clsFunc.Find_TrayWork_Pos(0, st_sync_info.nTestSite_Target_THD_Work_Site[2], CTL_NO, m_npFindWorkPosYXCPB,THD_TEST_RBT, st_worker_info.strLotNo_Empty);
			
			//2016.1229
			//�� ������ ������ UNLOAD�� ����̽��� �ִ��� �ٽ��ѹ� üũ����
			if( nRet_1 == RET_GOOD)
			{
				nFlag = 0;
				for(i = 0; i < 8; i++)  
				{
					if(st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].st_pcb_info[i].nYesNo == CTL_YES && 
						st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].st_pcb_info[i].nTestBdStart == BD_END)
					{
						nFlag++;
					}
				}
				if( nFlag > 0)  nRet_1 = RET_SKIP;
			}
			//////////////////////////////////////////////////////////////////////////////////////////////////////
			
			nFlag = 0;
			if(nRet_1 == RET_SKIP || st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3] == 4)//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nStart_FixPos == 4)//������ ��ġ�� 4�̸� ������ Tester�� Right ����Ʈ�� �۾��Ѱ�����, ���̻� �۾��� �����̾��� 
			{			
				for(i = 0; i < 8; i++)  
				{ 
					if(st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].st_pcb_info[i].nYesNo == CTL_YES && st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].st_pcb_info[i].nTestBdStart == BD_END)
					{
						nFlag++;
					}
				}
				if(nFlag > 0) //��ε��� ���簡 ������ ��ε� ���� �ؾ� �Ѵ� 
				{
					m_nRunStep = 1000; 
					break;
				}
				else
				{
					// jtkim 20161003
					/// m_nRunStep = 8100; 
					if (st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nTestBdStart == BD_NONE || st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nTestBdStart == BD_END)
					{
						m_nRunStep = 8100; 
					}
					else
					{
						m_nRunStep = 1000; 
					}
				}
			} 			
			else if(nRet_1 == RET_GOOD)
			{//�� �۾��� ������ �ִ� �׷��� ���縦 �� �־�� �Ѵ� 
				if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_TEST_RBT] >=  LOT_END_START) //   				 
				{//st_sync_info.nRetestBuff_Traget_THD_Work_Site

					nCount = 0;
					for(nTHD_i = THD_TESTSITE_1; nTHD_i <= THD_TESTSITE_8; nTHD_i++)
					{
						if(st_find_testsite_info[nTHD_i][WORK_PICK].nTotal_Cnt > 0 && st_test_site_info[nTHD_ID].strLotNo == m_strFindLotNo) //WORK_PICK �� �� �ִ� ���簡 ������ �� ���縦 ������ 
						{
							nCount++;
						}
					}

					nRet_1 = clsFunc.Find_TrayWork_Pos(0, st_sync_info.nRetestBuff_Traget_THD_Work_Site, CTL_YES, m_npFindWorkPosYXCPB, THD_TEST_RBT, m_strFindLotNo);
					if(nRet_1 == RET_SKIP) //���׽�Ʈ ���ۿ� �۾� ������ ���簡 �ִ�
					{// if(m_npFindWorkPosYXCPB[2] >= 0) //retest buffer�� ���簡 �ִ� 
						m_nRunStep = 8100; //���� lot�� ���̻� ������ ���簡 ������ ���� �������� test start�Ͽ� �׽�Ʈ �ϰ� �Ѵ� 
						break;
					} 
					else
					{
						m_nRunStep = 1000; 
					}
				}
				else
				{
					m_nRunStep = 1000; 
				}
			} 
			break;


		////////////////////////////////////////////////////
		//�׽�Ʈ ���� ��ŸƮ 
		////////////////////////////////////////////////////
		case 8100:
			//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nTester_Req[0] = CTL_REQ; //�׽��� �۾��� �˸��� 
			//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nTester_Req[1] = WORK_PLACE; //test scoket�� ���縦 �μ�Ʈ �ߴ�. �׽�Ʈ �����ؾ� �Ѵ�, chamber�� �ݰ� BD_START�� �����Ѵ� 
			//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nTester_Req[2] = BD_START_CHK; //st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].nSite_Pos;

			// jtkim 20160831
			st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][0] = CTL_REQ; //test start //CTL_REQ;
			st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][1] = WORK_PLACE; //���縦 ���� �� �ְ� ��û 
			st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][5] = BD_START; // test site start��û

			/*
			st_sync_info.dfnTestRbt_Dvc_Req[st_sync_info.nTestSite_Target_THD_Work_Site][0] = BD_START; //test start //CTL_REQ;
			st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nTestSite_Target_THD_Work_Site][1] = WORK_PLACE; //���縦 ���� �� �ְ� ��û 
			st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nTestSite_Target_THD_Work_Site][2] = st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].nSite_Pos;
			*/

			// �׽�Ʈ ��ŸƮ�� è������ ���� �� �׽�Ʈ �� ����Ʈ���� start �ϰ� ����  
			//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].nTestBdStart = BD_START; //test start
			// jtkim 2010831
			//st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nTestSite_Target_THD_Work_Site][0] = CTL_REQ; //test start //CTL_REQ;
			//st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nTestSite_Target_THD_Work_Site][1] = WORK_PLACE; //���縦 ���� �� �ְ� ��û 
			//st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nTestSite_Target_THD_Work_Site][2] = st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].nSite_Pos;

			//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].nTestBdStart = BD_START; //test start
			st_sync_info.nTestSite_Target_THD_Work_Site[2] = 0; //Ŭ���� �Ͽ�, �۾��� �׽��� ��ġ ã�� ���� �ʱ�ȭ �Ѵ�.
			                                                 // �ڷ� �̾ �׽�Ʈ ����Ʈ �۾��� �ϰ� ������ st_sync_info.nTestSite_Target_THD_Work_Site++; �ϸ� �ȴ� 
			//2016.1012
			st_sync_info.nTestSite_Target_THD_Work_Site[1] = 0; //Ŭ���� �Ͽ�, �۾��� �׽��� ��ġ ã�� ���� �ʱ�ȭ �Ѵ�.

			m_nRunStep = 1000; //test site ��� �۾��� �Ϸ�Ǿ���. 1000 ó�� �������� �̵��Ͽ� ���� ��ġ�� �۾��� ã�� �غ��Ѵ� 
			break;
			

		case 9000: 

			m_nRunStep = 1000;  
			break;


		default :
			break;
		}
}


//���ɿ��� ������ Ȯ���� �ε� ���ۿ� �ش� ������ ���� �ε� ���� ������ ��û �Ѵ� 
//�׽�Ʈ ���ϼ� �κ����� ��ġ �������� Ʋ����, ������ combine�ϴ� ����� �־�� �Ѵ� 
int CRunTestSiteRobot::TestSite_PickPlace_SocketData_LoadBuff_Map_Combine(int nMode, int nTestSite_THD_Num, int nTestSocket_0_4_Start_Pos) 
{//nTestSocket_0_4_Start_Pos => 0:���鿡�� ���� ù��° ����(0,1,2,3 socket �۾�), 4:���� �������� 4��° ����(4,5,6,7 ���� �۾�) �� ������ ��ġ�� �������� ���Ѵ� 
	int i, nPos=0;
	int nFuncRet = RET_ERROR;

	for(i=0; i< 4; i++) //loading ���� ���� 4�� 
	{
		if(nTestSite_THD_Num >= THD_TESTSITE_1 && nTestSite_THD_Num <= THD_TESTSITE_4) //���� �׽��� ,���� ������ ������ġ ���� (1ȸ�۾�)7,6,5,4, (2ȸ�۾�)3,2,1,0 �� ������ ���� ���� ��ġ�� ���� ���ǵ�
		{
			nPos = (7 - nTestSocket_0_4_Start_Pos) - i;
			if(st_test_site_info[nTestSite_THD_Num].st_pcb_info[nPos].nEnable == CTL_YES) //socket ON
			{
				st_buffer_info[THD_LD_BUFF].st_pcb_info[3 - i].nEnable = CTL_YES;
				nFuncRet = RET_GOOD;
				//james 2016.0926 ���� �߰� �ε� ������ ���縸�� ������ ������ ��� st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable = CTL_YES; 
			}
			else //if(st_test_site_info[nTestSite_THD_Num].st_pcb_info[nPos].nEnable  == CTL_NO)//Socket OFF
			{
				st_buffer_info[THD_LD_BUFF].st_pcb_info[3 - i].nEnable = CTL_NO;
				//james 2016.0926 ���� �߰� �ε� ������ ���縸�� ������ ������ ��� st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable = CTL_NO;
			}
		}
		else //if(nTestSite_THD_Num >= THD_TESTSITE_5 && nTestSite_THD_Num <= THD_TESTSITE_8) //���� �׽��� , ���� ������ ������ġ ����  (1ȸ�۾�)0,1,2,3, (2ȸ�۾�)4,5,6,7 �� ������ ���� ���� ��ġ�� ���� ���ǵ�
		{
			nPos = nTestSocket_0_4_Start_Pos + i;
			if(st_test_site_info[nTestSite_THD_Num].st_pcb_info[nPos].nEnable  == CTL_YES) //socket ON
			{
				st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nEnable = CTL_YES;
				nFuncRet = RET_GOOD;
				//james 2016.0926 ���� �߰� �ε� ������ ���縸�� ������ ������ ��� st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable = CTL_YES;
			}
			else //if(st_test_site_info[nTestSite_THD_Num].st_pcb_info[nPos].nEnable == CTL_NO)//Socket OFF
			{
				st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nEnable = CTL_NO;
				//james 2016.0926 ���� �߰� �ε� ������ ���縸�� ������ ������ ��� st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable = CTL_NO;
			}
		}  
	}

	return nFuncRet;
}

//////////////////////////////////////////////////
//1.load buffer
//2. test site
//3.retest 1, retest 2
//////////////////////////////////////////////////
int CRunTestSiteRobot::Process_DVC_Pick(int nMode, int nWork_Site, int nTest_Site, int nStart_FixPos) //int nStart_FixPos = Left(0), Right(4) �ΰ��� ��ġ�� �ϳ��̴�, ������ ���۱��� �������� �������� �۾� //2016.0814
{
	int nFuncRet = RET_PROCEED;
	int nRet_1, nRet_2, nRet_3 = 0, i, nFlag;	
	int nFind_JobPos =0; //�۾��� ��ġ�� ã�´�
	//int nStart_FixPos =0; //test site���� �������� ���� ��ġ  
	double dposZ = 0;
	 
	clsFunc.OnThreadFunctionStepTrace(25, m_nPick_Step);

	switch(m_nPick_Step)
	{
	case 0:
		/*
		if(nWork_Site == THD_LD_BUFF)
		{
			m_nSite_WorkSize_Num = st_recipe_info.nLdUldBuffer_Num; //4
		}
		else if(nWork_Site == THD_ULD_BUFF)
		{//���� ���� ���� 
			m_nSite_WorkSize_Num = st_recipe_info.nLdUldBuffer_Num; //4
		}
		else if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8)
		{
			m_nSite_WorkSize_Num = st_recipe_info.nTestSocket_Num; //8
		}
		else if(nWork_Site == THD_RETEST_1_BUFF)
		{
			m_nSite_WorkSize_Num = st_recipe_info.nRetestBuffer_Num; //15
		}
		else if(nWork_Site == THD_RETEST_2_BUFF)
		{
			m_nSite_WorkSize_Num = st_recipe_info.nRetestBuffer_Num; //15
		}*/

		m_bDvcWaitChk_Falg =  false;
		m_nPickRetry_Cnt = 0;
		m_nPick_Step = 100;
		break;

	case 100:
		m_nPick_Step = 900;
		break;
/*
		m_dCurrentPos[m_nRobot_Z] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[m_nRobot_Z] > st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
		{
			m_nPick_Step = 0;
			break;
		}
		m_dCurrentPos[m_nRobot_Ejector] = COMI.Get_MotCurrentPos(m_nRobot_Ejector);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[m_nRobot_Ejector] > st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
		{
			m_nPick_Step = 0;
			break;
		}
		m_dpTargetPosList[0] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_SAFETY];
		m_dpTargetPosList[1] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_SAFETY];
		nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate); 
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{
			m_nPick_Step = 400;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 300;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 300;
		}
*/
		 

	case 900:
		m_dCurrentPos[m_nRobot_Z] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[m_nRobot_Z] > st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
		{
			m_nPick_Step = 910;
		}
		else
		{
			m_nPick_Step = 920;
		}
		break;

	case 910:
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{
			m_nPick_Step = 920;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 900;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(2122, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 910;
		}
		break;

	case 920:
		m_dCurrentPos[m_nRobot_Ejector] = COMI.Get_MotCurrentPos(m_nRobot_Ejector);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[m_nRobot_Ejector] > st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
		{
			m_nPick_Step = 930;
		}
		else
		{
			m_nPick_Step = 940;
		}
		break;

	case 930:
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Ejector, st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{
			m_nPick_Step = 940;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 930;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(2123, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 930;
		}
		break;

	case 940:
		m_bDvcWaitChk_Falg = false;
		m_nPick_Step = 1000;
		break; 

	///////////////////////////////////////////////////////////////////
	// �۾� ������ Ȯ���Ѵ� 
	///////////////////////////////////////////////////////////////////
	case 1000:
		m_dCurrentPos[m_nRobot_Z] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[m_nRobot_Z] > st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
		{
			m_nPick_Step = 900;
			break;
		}
		m_dCurrentPos[m_nRobot_Ejector] = COMI.Get_MotCurrentPos(m_nRobot_Ejector);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[m_nRobot_Ejector] > st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[m_nRobot_Ejector].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
		{
			m_nPick_Step = 900;
			break;
		}

		nRet_1 = clsFunc.Check_PickerStatus(0, THD_TEST_RBT, CTL_YES, m_npPicker_YesNo, m_npPicker_Vacuum_Status, m_npPicker_OutputStatus); //nMode == 0 �̸� ��������, nMode == 1�̸� ��������� ������ ���� ��Ŀ ���¸� Ȯ���Ѵ�,
	
		if(nRet_1 == RET_GOOD) //���� ������ ���� ��Ŀ ���¸� üũ 
		{//��Ŀ ������ ���� ���°� �����̴�
		}
		else// if(nRet_1 == RET_ERROR)
		{
			if(m_bDvcWaitChk_Falg == false)
			{
				m_bDvcWaitChk_Falg =  true;
				m_dwWaitUntil[0] = GetCurrentTime();
				break;
			}
			else
			{
				m_dwWaitUntil[1] = GetCurrentTime();
				m_dwWaitUntil[2] = m_dwWaitUntil[1] - m_dwWaitUntil[0];
				if(m_dwWaitUntil[2] < IO_STABLE_WAIT) break;
				CTL_Lib.Alarm_Error_Occurrence(2124, dWARNING, clsFunc.m_strAlarmCode);
				m_bDvcWaitChk_Falg = false;
				break;
			}
		}

///////////////////////////////////////////////////////////////////////
// Retest Bin ó�� �ɼ����� ���׽�Ʈ ���� ��ġ���� �� ��Ŀ ��������  
// �Լ� ���������� ������ ���Ƿ� step�� jump �ؾ� ��
///////////////////////////////////////////////////////////////////////
		if(nMode == BIN_RETESTBIN)
		{
			m_nFirstTray_Y_Num = m_nFirst_Buff_Num; 
			m_nFirstTray_X_Num = m_nFirst_Buff_Num;

			m_npFindWorkPosYXCPB[0] = m_nFirst_Buff_Num;
			m_npFindWorkPosYXCPB[1] = m_nFirst_Buff_Num;
			 m_npFindWorkPosYXCPB[3] = m_nFirstPicker_Num;
			 m_nPick_Step = 2000;
			 break;
		}
///////////////////////////////////////////////////////////////////////

		//�۾� ���� ��ġ�� ã�´�
		nFlag = 0;
		m_nFirstTray_Y_Num = -1; m_nFirstTray_Y_Num = -1; m_nFirstPicker_Num = -1; //flag clear
		for(i = 0; i < m_nPickerPara; i++)
		{
			if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_NO && st_picker[m_nRobotSite].st_pcb_info[i].nEnable == CTL_YES)//��Ŀ�� ���簡 ��� �ִٴ� ������ ����
			{
				if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF)
				{
					m_nFirstPicker_Num = i; //������ ���� ��Ŀ ��ġ�� ���� �ȴ�
				}
				else //if(nWork_Site == THD_LD_BUFF || nWork_Site == THD_LD_BUFF || (nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8) )
				{
					m_nFirstPicker_Num = 0; //��Ŀ�� ���� ��ġ�� �������� �۾��Ѵ� 
				}
				
				i = m_nPickerPara;
			}
		}

		if(m_nFirstPicker_Num == -1)
		{
			//��Ŀ�� ���̻� �۾��� ���� ����
			m_nPick_Step = 0;
			nFuncRet = RET_PICKER_NOT_FIND;
			break;
		}

		nRet_1 = clsFunc.Find_TrayWork_Pos(0, nWork_Site, CTL_YES, m_npFindWorkPosYXCPB, THD_TEST_RBT, st_worker_info.strLotNo_Empty);
		if(nRet_1 == RET_GOOD) //�۾� ������ ���簡 �ִ�
		{
			if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF)
			{
				m_nFirstTray_Y_Num = m_npFindWorkPosYXCPB[0]; //������� ����  m_npFindWorkPosYXCPB[0];
				m_nFirstTray_X_Num = m_npFindWorkPosYXCPB[1];
			}
			else if(nWork_Site == THD_LD_BUFF)// || nWork_Site == THD_ULD_BUFF) 
			{
				m_nFirstTray_Y_Num = 0;
				m_nFirstTray_X_Num = 0;//���
			}
			else if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8)
			{
				m_nFirstTray_Y_Num = nStart_FixPos;//������� ���� m_nFirstTray_Y_Num = st_test_socket_info[st_sync_info.nTestSite_Target_THD_Work_Site].nWorkSocket_StartPos; 
				m_nFirstTray_X_Num = nStart_FixPos; //������ ���� ���� ��ġ��, 0 �Ǵ� 4 ��  st_test_socket_info[st_sync_info.nTestSite_Target_THD_Work_Site].nWorkSocket_StartPos;
			}
		}
		else if(nRet_1 == RET_SKIP) //���̻� ���簡 ����
		{	//��� ������ �� ����
			m_nPick_Step = 0;
			nFuncRet = RET_TRAY_NOT_FIND;
			break;
		}

		nFlag = 0;
		if(m_nFirstPicker_Num != -1 && m_nFirstTray_Y_Num != -1 && m_nFirstTray_X_Num != -1) //�۾��� ������ �� ������ ���� �۾��� �����Ѵ�
		{
			for(i = 0; i < m_nPickerPara; i++)  //4, m_n_FirstPicker_Num; y < m_mPickerPara; y++)
			{
				m_npPicker_YesNo[i] = CTL_NO; //�۾��� ��Ŀ���� clear
				if(i < m_nFirstPicker_Num) continue;
				if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_NO && st_picker[m_nRobotSite].st_pcb_info[i].nEnable == CTL_YES) //��Ŀ������ ������ 
				{
					if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF)
					{//james 2016.0922 retest buffer �� �̰��� �ǵ����� �ʴ´�. ������ step jump ��  
						if(i >= 0 && m_nFirstTray_X_Num + (i - m_nFirstPicker_Num) < st_recipe_info.nRetestBuffer_Num)//15
						{
							//���ך� ������ ���߿� �������� 
							if(nTest_Site == TESTSITE_LEFT) //left site�� �۾��ϱ� ���� �۾� ���϶�
							{
								nFind_JobPos = (st_recipe_info.nLdUldBuffer_Num - 1) - i ; //LEFT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ���������� 3,2,1,0 �� �ݴ��̴� 
								if(st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_YES)//&& st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //Ʈ���̿� ���簡  ������ ���� �����ϰ� ��
								{
									m_npPicker_YesNo[i] = CTL_YES;
									nFlag++;
								}
							}
							else if(nTest_Site == TESTSITE_RIGHT) //right sitet�� �۾��ϱ� ���� �۾� ���϶�
							{
								nFind_JobPos = i ; //RIGHT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ����������0,1,2,3 �� ��ġ ������ �����ϴ�  
								if(st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_YES && st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //Ʈ���̿� ���簡  ������ ���� �����ϰ� ��
								{
									m_npPicker_YesNo[i] = CTL_YES;
									nFlag++;
								}
							}
						}			
					}
					else if(nWork_Site == THD_LD_BUFF) 
					{//m_nPickerPara == 4
						if(i >= 0 && m_nFirstTray_X_Num + (i - m_nFirstPicker_Num) < st_recipe_info.nLdUldBuffer_Num)//4
						{
							if(nTest_Site == TESTSITE_LEFT) //left site�� �۾��ϱ� ���� �۾� ���϶�
							{
								nFind_JobPos = (st_recipe_info.nLdUldBuffer_Num - 1) - i ; //LEFT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ���������� 3,2,1,0 �� �ݴ��̴� 
								if(st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_YES)//james 2016.1005 ���� ������ ������ ����  && st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //Ʈ���̿� ���簡  ������ ���� �����ϰ� ��
								{
									m_npPicker_YesNo[i] = CTL_YES;
									nFlag++;
								}
							}
							else if(nTest_Site == TESTSITE_RIGHT) //right sitet�� �۾��ϱ� ���� �۾� ���϶�
							{
								nFind_JobPos = i ; //RIGHT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ����������0,1,2,3 �� ��ġ ������ �����ϴ�  
								if(st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_YES)//james 2016.1005 ���� ������ ������ ����  && st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //Ʈ���̿� ���簡  ������ ���� �����ϰ� ��
								{
									m_npPicker_YesNo[i] = CTL_YES;
									nFlag++;
								}
							}
						}						
					}
					else if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_4 ) //left site
					{
						if(i >= 0 && m_nFirstTray_X_Num + (i - m_nFirstPicker_Num) < st_recipe_info.nTestSocket_Num)//8
						{
							nFind_JobPos = (st_recipe_info.nTestSocket_Num - 1) - i ; //LEFT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ���������� 3,2,1,0 �� �ݴ��̴� 
							nFind_JobPos = nFind_JobPos - nStart_FixPos; //st_test_site_info[nWork_Site].nStart_FixPos;

							if(st_test_site_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_YES )//&& st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //Ʈ���̿� ���簡  ������ ���� �����ϰ� ��
							{
								m_npPicker_YesNo[i] = CTL_YES;
								nFlag++;
							}
						}
					}
					else if(nWork_Site >= THD_TESTSITE_5 && nWork_Site <= THD_TESTSITE_8 )//right site
					{
						if(i >= 0 && m_nFirstTray_X_Num + (i - m_nFirstPicker_Num) < st_recipe_info.nTestSocket_Num) //8
						{
							nFind_JobPos = i ; //RIGHT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ����������0,1,2,3 �� ��ġ ������ �����ϴ�  
							nFind_JobPos = nFind_JobPos + nStart_FixPos; //st_test_site_info[nWork_Site].nStart_FixPos;

							if(st_test_site_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_YES )//&& st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //Ʈ���̿� ���簡  ������ ���� �����ϰ� ��
							{
								m_npPicker_YesNo[i] = CTL_YES;
								nFlag++;
							}
						}
					}
				}
			}

			if(nFlag == 0) //�۾��� ��Ŀ�� �ϳ��� �غ�� �ʾҴ�.
			{ //�̺κ��� �ƿ��� �ȵ� 
				m_nPick_Step = 0;
				nFuncRet = RET_PICKER_NOT_FIND;//��Ŀ�� ���̻� �۾��� ���� ����
				break;
			}

			//���� �۾� ��ġ�� ����
			m_npFindWorkPosYXCPB[0] = m_nFirstTray_Y_Num; m_npFindWorkPosYXCPB[1] = m_nFirstTray_X_Num; m_npFindWorkPosYXCPB[3] = m_nFirstPicker_Num;

			m_nMove_Flag[0] = CTL_NO; m_nMove_Flag[1] = CTL_NO;
			m_nPick_Step = 2000;
		}		
		break;

	case 2000:
		m_dCurrentPos[2] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[2] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
		{
			m_nPick_Step = 900;
			break;
		}
		m_dCurrentPos[m_nRobot_Ejector] = COMI.Get_MotCurrentPos(m_nRobot_Ejector);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[m_nRobot_Ejector] > st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
		{
			m_nPick_Step = 900;
			break;
		}

		//james 2016.0826 ��Ŀ�� ����/�׽�Ʈ ������ ���� �浹������ ���� �߰� 
		if(nWork_Site == THD_LD_BUFF)
		{//Check_Robot_Buffer_Clash(int nTestDirection, int nRbtTHDSite, int TestBuffTHDSite, int nStartPos, int *nErrorInfoStatus) 
			if(nTest_Site == TESTSITE_LEFT) //LEFT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ���������� 3,2,1,0 �� �ݴ��̴� 
			{
				nRet_1 = COMI.Check_MotPosRange(M_LD_BUF_SHIFTER_Y, st_motor_info[M_LD_BUF_SHIFTER_Y].d_pos[P_LDBUFF_Y_TESTRBT], st_motor_info[M_LD_BUF_SHIFTER_Y].d_allow);
				nRet_2 = COMI.Check_MotPosRange(M_LD_BUF_ROTATOR, st_motor_info[M_LD_BUF_ROTATOR].d_pos[P_LDBUFF_RTR_TESTSITE_LEFT], st_motor_info[M_LD_BUF_ROTATOR].d_allow);
				if(nRet_1 != BD_GOOD || nRet_2 != BD_GOOD)
				{
					if(nRet_1 != BD_GOOD ) m_strAlarmCode.Format(_T("%02d0006"), M_LD_BUF_SHIFTER_Y); 
					else m_strAlarmCode.Format(_T("%02d0006"), M_LD_BUF_ROTATOR); 
					CTL_Lib.Alarm_Error_Occurrence(2125, dWARNING, m_strAlarmCode);
					break;
				}

				nRet_1 = clsFunc.Check_Robot_Buffer_Clash(TESTSITE_LEFT, THD_TEST_RBT, THD_LD_BUFF, m_nFirstTray_X_Num, m_nFirstPicker_Num, m_nClash_ErrorInfoStatus); //��Ŀ�� ���۵��� ���� �浹 Ȯ�� 
				if(nRet_1 == RET_ERROR)
				{
					CTL_Lib.Alarm_Error_Occurrence(2126, dWARNING, clsFunc.m_strAlarmCode);		
					break;
				}
			}
			else if(nTest_Site == TESTSITE_RIGHT) //right sitet�� �۾��ϱ� ���� �۾� ���϶�//RIGHT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ����������0,1,2,3 �� ��ġ ������ �����ϴ�  
			{
				nRet_1 = COMI.Check_MotPosRange(M_LD_BUF_SHIFTER_Y, st_motor_info[M_LD_BUF_SHIFTER_Y].d_pos[P_LDBUFF_Y_TESTRBT], st_motor_info[M_LD_BUF_SHIFTER_Y].d_allow);
				nRet_2 = COMI.Check_MotPosRange(M_LD_BUF_ROTATOR, st_motor_info[M_LD_BUF_ROTATOR].d_pos[P_LDBUFF_RTR_TESTSITE_RIGHT], st_motor_info[M_LD_BUF_ROTATOR].d_allow);
				if(nRet_1 != BD_GOOD || nRet_2 != BD_GOOD)
				{
					if(nRet_1 != BD_GOOD ) m_strAlarmCode.Format(_T("%02d0006"), M_LD_BUF_SHIFTER_Y); 
					else m_strAlarmCode.Format(_T("%02d0006"), M_LD_BUF_ROTATOR); 
					CTL_Lib.Alarm_Error_Occurrence(2130, dWARNING, m_strAlarmCode);
					break;
				}

				nRet_1 = clsFunc.Check_Robot_Buffer_Clash(TESTSITE_RIGHT, THD_TEST_RBT, THD_LD_BUFF, m_nFirstTray_X_Num, m_nFirstPicker_Num, m_nClash_ErrorInfoStatus); //��Ŀ�� ���۵��� ���� �浹 Ȯ�� 
				if(nRet_1 == RET_ERROR)
				{
					CTL_Lib.Alarm_Error_Occurrence(2131, dWARNING, clsFunc.m_strAlarmCode);		
					break;
				}
			}
		}
		else  if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_4 ) //left site
		{
			nRet_1 = clsFunc.Check_Robot_Buffer_Clash(TESTSITE_LEFT, THD_TEST_RBT, nWork_Site, m_nFirstTray_X_Num, m_nFirstPicker_Num, m_nClash_ErrorInfoStatus); //��Ŀ�� ���۵��� ���� �浹 Ȯ�� 
			if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(2132, dWARNING, clsFunc.m_strAlarmCode);		
				break;
			}
		}
		else  if(nWork_Site >= THD_TESTSITE_5 && nWork_Site <= THD_TESTSITE_8 ) //right site
		{
			nRet_1 = clsFunc.Check_Robot_Buffer_Clash(TESTSITE_RIGHT, THD_TEST_RBT, nWork_Site, m_nFirstTray_X_Num, m_nFirstPicker_Num, m_nClash_ErrorInfoStatus); //��Ŀ�� ���۵��� ���� �浹 Ȯ�� 
			if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(2133, dWARNING, clsFunc.m_strAlarmCode);		
				break;
			}
		}//
		else  if(nWork_Site >= THD_RETEST_1_BUFF && nWork_Site <= THD_RETEST_2_BUFF ) //james 2016.0929 �߰� 
		{
			//james 2016.0929 �߰� 
			nRet_1 = clsFunc.Check_Robot_Buffer_Clash(nTest_Site, THD_TEST_RBT, nWork_Site, m_nFirstTray_X_Num, m_nFirstPicker_Num, m_nClash_ErrorInfoStatus); //��Ŀ�� ���۵��� ���� �浹 Ȯ�� 
			if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(2134, dWARNING, clsFunc.m_strAlarmCode);		
				break;
			}  
		}//

		// jtkim 20160904
		//nRet_1 = clsFunc.Calculate_MovePos_Find(0, m_nRobot_Y, nWork_Site, 0, m_npFindWorkPosYXCPB, &m_dTemp_CalTargetPos_Y);             //Calculate_MovePos_Find(int nMotNum, int nSite, int nWorkPart, int *npFirstYXP_Pos, double *dpGetTargetPos)
		//nRet_2 = clsFunc.Calculate_MovePos_Find(0, m_nRobot_X, nWork_Site, 0, m_npFindWorkPosYXCPB, &m_dTemp_CalTargetPos_X);
		if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_4 ) //left site
		{
			nRet_1 = clsFunc.Calculate_MovePos_Find(0, m_nRobot_Y, nWork_Site, nStart_FixPos, m_npFindWorkPosYXCPB, &m_dTemp_CalTargetPos_Y);             //Calculate_MovePos_Find(int nMotNum, int nSite, int nWorkPart, int *npFirstYXP_Pos, double *dpGetTargetPos)
			nRet_2 = clsFunc.Calculate_MovePos_Find(0, m_nRobot_X, nWork_Site, nStart_FixPos, m_npFindWorkPosYXCPB, &m_dTemp_CalTargetPos_X);
		}
		else  if(nWork_Site >= THD_TESTSITE_5 && nWork_Site <= THD_TESTSITE_8 ) //right site
		{
			nRet_1 = clsFunc.Calculate_MovePos_Find(0, m_nRobot_Y, nWork_Site, nStart_FixPos, m_npFindWorkPosYXCPB, &m_dTemp_CalTargetPos_Y);             //Calculate_MovePos_Find(int nMotNum, int nSite, int nWorkPart, int *npFirstYXP_Pos, double *dpGetTargetPos)
			nRet_2 = clsFunc.Calculate_MovePos_Find(0, m_nRobot_X, nWork_Site, nStart_FixPos, m_npFindWorkPosYXCPB, &m_dTemp_CalTargetPos_X);

		}//
		else if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF)
		{
			nRet_1 = clsFunc.Calculate_MovePos_Find(0, m_nRobot_Y, nWork_Site, nTest_Site, m_npFindWorkPosYXCPB, &m_dTemp_CalTargetPos_Y);             //Calculate_MovePos_Find(int nMotNum, int nSite, int nWorkPart, int *npFirstYXP_Pos, double *dpGetTargetPos)
			nRet_2 = clsFunc.Calculate_MovePos_Find(0, m_nRobot_X, nWork_Site, nTest_Site, m_npFindWorkPosYXCPB, &m_dTemp_CalTargetPos_X);
		}
		else
		{
			nRet_1 = clsFunc.Calculate_MovePos_Find(0, m_nRobot_Y, nWork_Site, 0, m_npFindWorkPosYXCPB, &m_dTemp_CalTargetPos_Y);             //Calculate_MovePos_Find(int nMotNum, int nSite, int nWorkPart, int *npFirstYXP_Pos, double *dpGetTargetPos)
			nRet_2 = clsFunc.Calculate_MovePos_Find(0, m_nRobot_X, nWork_Site, 0, m_npFindWorkPosYXCPB, &m_dTemp_CalTargetPos_X);
		}

		if(nRet_1 == RET_GOOD && nRet_2 == RET_GOOD)
		{//ok
			m_dpTargetPosList[0] = m_dTemp_CalTargetPos_Y;
			m_dpTargetPosList[1] = m_dTemp_CalTargetPos_X;

			Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON); //�̸� ������  
		}
		else
		{
			CTL_Lib.Alarm_Error_Occurrence(2135, dWARNING, clsFunc.m_strAlarmCode);
			break;
		}
		
		nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate); 
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{
			m_bDvcWaitChk_Falg =  false;
			m_nPick_Step = 2100;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 2000;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(2136, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 2000;
		}
		break; 	

	case 2100:
		if(nWork_Site == THD_LD_BUFF)
		{
			

			//�ε� ���ۿ� ��û�Ͽ� ���簡 �غ�Ǿ� CTL_READY �� ����, ���縦 ���� �� �ִ� 
			 if(st_sync_info.nTestRbt_Dvc_Req[nWork_Site][0] == CTL_READY && st_sync_info.nTestRbt_Dvc_Req[nWork_Site][1] == WORK_PICK) //�ε� ���۰� �غ�Ǳ⸦ ��ٸ��� 
			 {				 
				nRet_1 = clsFunc.Check_Robot_Buffer_Clash(st_sync_info.nTestRbt_Dvc_Req[nWork_Site][2], THD_TEST_RBT, nWork_Site, m_nFirstTray_X_Num, m_nFirstPicker_Num, m_nClash_ErrorInfoStatus); //��Ŀ�� ���۵��� ���� �浹 Ȯ�� 
				if(nRet_1 == RET_ERROR)
				{
					CTL_Lib.Alarm_Error_Occurrence(2137, dWARNING, clsFunc.m_strAlarmCode);		
					break;
				}
				else
				{
					 m_nPick_Step = 3000;
				}
			 }		
		}
		//else if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF || nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8)
		else if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF)
		{// 
			if(st_sync_info.nTestRbt_Dvc_Req[nWork_Site][0] == CTL_READY && st_sync_info.nTestRbt_Dvc_Req[nWork_Site][1] == WORK_PICK) //�ε� ���۰� �غ�Ǳ⸦ ��ٸ��� 
			{				 
				//james 2016.0911 �ٲ�� �� 
				nRet_1 = clsFunc.Check_Robot_Buffer_Clash(st_sync_info.nTestRbt_Dvc_Req[nWork_Site][2], THD_TEST_RBT, nWork_Site, m_nFirstTray_X_Num, m_nFirstPicker_Num, m_nClash_ErrorInfoStatus); //��Ŀ�� ���۵��� ���� �浹 Ȯ�� 
				if(nRet_1 == RET_ERROR)
				{
					CTL_Lib.Alarm_Error_Occurrence(2140, dWARNING, clsFunc.m_strAlarmCode);		
					break;
				}
				else
				{
					m_nPick_Step = 3000;
				}  
			}
		}
		else if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF || nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8)
		{// 
			if(st_sync_info.nTestRbt_Dvc_UnLd_Req[nWork_Site][0] == CTL_READY && st_sync_info.nTestRbt_Dvc_UnLd_Req[nWork_Site][1] == WORK_PICK) //�ε� ���۰� �غ�Ǳ⸦ ��ٸ��� 
			{				 
				nRet_1 = clsFunc.Check_Robot_Buffer_Clash(st_sync_info.nTestRbt_Dvc_Req[nWork_Site][2], THD_TEST_RBT, nWork_Site, m_nFirstTray_X_Num, m_nFirstPicker_Num, m_nClash_ErrorInfoStatus); //��Ŀ�� ���۵��� ���� �浹 Ȯ�� 
				if(nRet_1 == RET_ERROR)
				{
					CTL_Lib.Alarm_Error_Occurrence(2150, dWARNING, clsFunc.m_strAlarmCode);		
					break;
				}
				else
				{
					m_nPick_Step = 3000;
				}  
			}
		}
		break;


	case 3000: 
		m_dCurrentPos[m_nRobot_Ejector] = COMI.Get_MotCurrentPos(m_nRobot_Ejector);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[m_nRobot_Ejector] > st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
		{
			m_nPick_Step = 900;
			break;
		}

		nRet_1 = COMI.Check_MotPosRange(m_nRobot_Y, m_dpTargetPosList[0], st_motor_info[m_nRobot_Y].d_allow);
		nRet_2 = COMI.Check_MotPosRange(m_nRobot_X, m_dpTargetPosList[1], st_motor_info[m_nRobot_X].d_allow);
		if(nRet_1 != BD_GOOD || nRet_2 != BD_GOOD)
		{
			m_nPick_Step = 2000;
			break;
		}

		Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON); //�̸� ������  

		if(nWork_Site == THD_LD_BUFF)
		{
			m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_LDBUF_PICK];
		}		
		else if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_4)
		{
			//james 2016.0918 
			if(nWork_Site == THD_TESTSITE_1)
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Chamber1OpenChk,	IO_ON);
				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_Chamber1CloseChk,	IO_OFF);
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_1_LEFT_PICK]; //kwlee 2016.1117
				if(nRet_1 != IO_ON || nRet_2 != IO_OFF) 
				{ 
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Chamber1OpenChk); 
					CTL_Lib.Alarm_Error_Occurrence(2160, dWARNING, m_strAlarmCode);	
					break;
				}
			}
			else if(nWork_Site == THD_TESTSITE_2)
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Chamber2OpenChk,	IO_ON);
				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_Chamber2CloseChk,	IO_OFF);
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_2_LEFT_PICK]; //kwlee 2016.1117
				if(nRet_1 != IO_ON || nRet_2 != IO_OFF) 
				{ 
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Chamber2OpenChk); 
					CTL_Lib.Alarm_Error_Occurrence(2170, dWARNING, m_strAlarmCode);	
					break;
				}
			}
			else if(nWork_Site == THD_TESTSITE_3)
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Chamber3OpenChk,	IO_ON);
				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_Chamber3CloseChk,	IO_OFF);
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_3_LEFT_PICK]; //kwlee 2016.1117
				if(nRet_1 != IO_ON || nRet_2 != IO_OFF) 
				{ 
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Chamber3OpenChk); 
					CTL_Lib.Alarm_Error_Occurrence(2180, dWARNING, m_strAlarmCode);	
					break;
				}
			}
			else if(nWork_Site == THD_TESTSITE_4)
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Chamber4OpenChk,	IO_ON);
				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_Chamber4CloseChk,	IO_OFF);
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_4_LEFT_PICK]; //kwlee 2016.1117
				if(nRet_1 != IO_ON || nRet_2 != IO_OFF) 
				{ 
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Chamber4OpenChk); 
					CTL_Lib.Alarm_Error_Occurrence(2190, dWARNING, m_strAlarmCode);	
					break;
				}
			}
			//m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_LEFT_PICK];
			Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON); //�̸� ������  
			m_nPick_Step = 3100;
			break;
		}
		else if(nWork_Site >= THD_TESTSITE_5 && nWork_Site <= THD_TESTSITE_8)
		{
			//james 2016.0918 
			if(nWork_Site == THD_TESTSITE_5)
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Chamber5OpenChk,	IO_ON);
				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_Chamber5CloseChk,	IO_OFF);
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_5_RIGHT_PICK]; //kwlee2016.1117
				if(nRet_1 != IO_ON || nRet_2 != IO_OFF) 
				{ 
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Chamber5OpenChk); 
					CTL_Lib.Alarm_Error_Occurrence(2200, dWARNING, m_strAlarmCode);	
					break;
				}
			}
			else if(nWork_Site == THD_TESTSITE_6)
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Chamber6OpenChk,	IO_ON);
				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_Chamber6CloseChk,	IO_OFF);
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_6_RIGHT_PICK]; //kwlee2016.1117
				if(nRet_1 != IO_ON || nRet_2 != IO_OFF) 
				{ 
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Chamber6OpenChk); 
					CTL_Lib.Alarm_Error_Occurrence(2210, dWARNING, m_strAlarmCode);	
					break;
				}
			}
			else if(nWork_Site == THD_TESTSITE_7)
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Chamber7OpenChk,	IO_ON);
				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_Chamber7CloseChk,	IO_OFF);
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_7_RIGHT_PICK]; //kwlee2016.1117
				if(nRet_1 != IO_ON || nRet_2 != IO_OFF) 
				{ 
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Chamber7OpenChk); 
					CTL_Lib.Alarm_Error_Occurrence(2220, dWARNING, m_strAlarmCode);	
					break;
				}
			}
			else if(nWork_Site == THD_TESTSITE_8)
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Chamber8OpenChk,	IO_ON);
				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_Chamber8CloseChk,	IO_OFF);
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_8_RIGHT_PICK]; //kwlee2016.1117
				if(nRet_1 != IO_ON || nRet_2 != IO_OFF) 
				{ 
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Chamber8OpenChk); 
					CTL_Lib.Alarm_Error_Occurrence(2230, dWARNING, m_strAlarmCode);	
					break;
				}
			}
			//m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_RIGHT_PICK]; //kwlee 2016.1117
			Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON); //�̸� ������  
			m_nPick_Step = 3100;
			break; //
		}
		else if(nWork_Site == THD_RETEST_1_BUFF)
		{
			m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_RETESTBUF_1_PICK];
		}
		else if(nWork_Site == THD_RETEST_2_BUFF)
		{
			m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_RETESTBUF_2_PICK];
		}
		else
		{//
			break;
		} 

		if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8) //james 2016.0922 test site �۾��̸� ejector�� ���� pick �۾��� �����ϴ� 
		{
			m_nPick_Step = 3110;
			break;
		}


		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, m_dpTargetPosList[2], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{ 			
			m_nPick_Step = 3100;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 3000;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(2240, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 3000;
		}		
		break;	

	case 3100:
		if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8) //test site �۾��̸� 
		{
			m_nPick_Step = 3110;
		}
		else 
		{
			m_nPick_Step = 3200;
		}
		break;

	///////////////////////////////////////////////////////////////////
	// Test site���� ��ŷ�ҷ���, ���縦 ���� ���� ejector ���� �Ͽ��� �Ѵ�
	// socket ejector ���� 
	///////////////////////////////////////////////////////////////////
	case 3110:
		nRet_1 = COMI.Check_MotPosRange(m_nRobot_Y, m_dpTargetPosList[0], st_motor_info[m_nRobot_Y].d_allow);
		nRet_2 = COMI.Check_MotPosRange(m_nRobot_X, m_dpTargetPosList[1], st_motor_info[m_nRobot_X].d_allow);
		if(nRet_1 != BD_GOOD || nRet_2 != BD_GOOD)
		{
			m_nPick_Step = 2000;
			break;
		}

		if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_4)
		{
			//kwlee 2016.1117
			//m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_LEFT_EJECT];
			if(nWork_Site == THD_TESTSITE_1)
			{
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_1_LEFT_EJECT];//kwlee 2016.1117
			}
			else if(nWork_Site == THD_TESTSITE_2)
			{
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_2_LEFT_EJECT];//kwlee 2016.1117
			}
			else if(nWork_Site == THD_TESTSITE_3)
			{
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_3_LEFT_EJECT];//kwlee 2016.1117
			}
			else if(nWork_Site == THD_TESTSITE_4)
			{
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_4_LEFT_EJECT];//kwlee 2016.1117
			}
			Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON); //�̸� ������  
		}
		else if(nWork_Site >= THD_TESTSITE_4 && nWork_Site <= THD_TESTSITE_8)
		{
			//kwlee 2016.1117
			//m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_RIGHT_EJECT]; 
			if(nWork_Site == THD_TESTSITE_5)
			{
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_5_RIGHT_EJECT];
			}
			else if(nWork_Site == THD_TESTSITE_6)
			{
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_6_RIGHT_EJECT];
			}
			else if(nWork_Site == THD_TESTSITE_7)
			{
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_7_RIGHT_EJECT];
			}
			else if(nWork_Site == THD_TESTSITE_8)
			{
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_8_RIGHT_EJECT];
			}
			//m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_RIGHT_EJECT];
			Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON); //�̸� ������  
		}
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, m_dpTargetPosList[2], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{
			m_nPickRetry_Cnt = 0;
			m_nPick_Step = 3120;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 3110;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			m_nPickRetry_Cnt++;
			if( m_nPickRetry_Cnt >= 2)
			{
				m_nPickRetry_Cnt = 0;
				CTL_Lib.Alarm_Error_Occurrence(2250, dWARNING, st_alarm_info.strCode);
			}
			m_nPick_Step = 3110;
		}		
		break;	

	case 3120:
		nRet_1 = COMI.Check_MotPosRange(m_nRobot_Y, m_dpTargetPosList[0], st_motor_info[m_nRobot_Y].d_allow);
		nRet_2 = COMI.Check_MotPosRange(m_nRobot_X, m_dpTargetPosList[1], st_motor_info[m_nRobot_X].d_allow);
		if(nRet_1 != BD_GOOD || nRet_2 != BD_GOOD)
		{
			m_nPick_Step = 2000;
			break;
		}
		if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_4)
		{
			m_dpTargetPosList[3] = st_motor_info[m_nRobot_Ejector].d_pos[P_TESTSITE_1_LEFT];
			Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON); //�̸� ������  
		}
		else if(nWork_Site >= THD_TESTSITE_5 && nWork_Site <= THD_TESTSITE_8)
		{
			m_dpTargetPosList[3] = st_motor_info[m_nRobot_Ejector].d_pos[P_TESTSITE_2_RIGHT];
			Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON); //�̸� ������  
		}
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Ejector, m_dpTargetPosList[3],  (int) (COMI.mn_runspeed_rate / 2));//2017.0325 COMI.mn_runspeed_rate
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{ 			
			m_nPick_Step = 3130;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 3120;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(2260, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 3120;
		}		
		break;	

	case 3130: //ejector ���� ��ġ�� �̵� 
		m_dpTargetPosList[3] = st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY];
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Ejector, m_dpTargetPosList[3],  (int) (COMI.mn_runspeed_rate / 2));//2017.0325 COMI.mn_runspeed_rate
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{ 			
			m_nPick_Step = 3140;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 3120;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(2270, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 3120;
		}		
		break;	

	//Z�� ����̽� test socket dvc pick 
	case 3140:
		nRet_1 = COMI.Check_MotPosRange(m_nRobot_Y, m_dpTargetPosList[0], st_motor_info[m_nRobot_Y].d_allow);
		nRet_2 = COMI.Check_MotPosRange(m_nRobot_X, m_dpTargetPosList[1], st_motor_info[m_nRobot_X].d_allow);
		if(nRet_1 != BD_GOOD || nRet_2 != BD_GOOD)
		{
			m_nPick_Step = 2000;
			break;
		}

		if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_4)
		{
			//kwlee 2016.1117
			if(nWork_Site == THD_TESTSITE_1)
			{
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_1_LEFT_PICK];
			}
			else if(nWork_Site == THD_TESTSITE_2)
			{
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_2_LEFT_PICK];
			}
			else if(nWork_Site == THD_TESTSITE_3)
			{
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_3_LEFT_PICK];
			}
			else if(nWork_Site == THD_TESTSITE_4)
			{
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_4_LEFT_PICK];
			}
			//m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_LEFT_PICK];
			//Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON); //�̸� ������  
		}
		else if(nWork_Site >= THD_TESTSITE_4 && nWork_Site <= THD_TESTSITE_8)
		{
			//kwlee 2016.1117
			if(nWork_Site == THD_TESTSITE_5)
			{
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_5_RIGHT_PICK];
			}
			else if(nWork_Site == THD_TESTSITE_6)
			{
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_6_RIGHT_PICK];
			}
			else if(nWork_Site == THD_TESTSITE_7)
			{
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_7_RIGHT_PICK];
			}
			else if(nWork_Site == THD_TESTSITE_8)
			{
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_8_RIGHT_PICK];
			}
			//m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_RIGHT_PICK];
			//Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON); //�̸� ������  
		}
		else
		{//
			break;
		} 

		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, m_dpTargetPosList[2], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{ 			
			m_nPick_Step = 3200;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 3130;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(2280, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 3130;
		}		
		break;	

	///////////////////////////////////
	//���縦 ���´� 
	///////////////////////////////////
	case 3200: //���縦 ���´� 
		Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_OFF);  
		m_nPick_Step = 3210;
		break;

	case 3210:
		nRet_1 = Chk_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_OFF);  
		if(nRet_1 == RET_GOOD)
		{
			m_bDvcWaitChk_Falg = false;
			m_nPick_Step = 3300;
		}
		else if(nRet_1 == RET_ERROR)
		{
			Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON);  //������ ������ 
			CTL_Lib.Alarm_Error_Occurrence(2290, dWARNING, m_strAlarmCode);
			m_nPick_Step = 3200;
		}
		break;

	case 3300:
		nRet_1 = clsFunc.Check_PickerStatus(1, THD_TEST_RBT, CTL_YES, m_npPicker_YesNo, m_npPicker_Vacuum_Status, m_npPicker_OutputStatus);
		if(nRet_1 == RET_GOOD) //���� ������ ���� ��Ŀ ���¸� üũ 
		{//��Ŀ ������ ���� ���°� �����̴�
			m_nPick_Step = 3400;
		}
		else// if(nRet_1 == RET_ERROR)
		{
			if(m_bDvcWaitChk_Falg == false)
			{
				m_bDvcWaitChk_Falg =  true;
				m_dwWaitUntil[0] = GetCurrentTime();
				break;
			}
			else
			{
				m_dwWaitUntil[1] = GetCurrentTime();
				m_dwWaitUntil[2] = m_dwWaitUntil[1] - m_dwWaitUntil[0];
				if(m_dwWaitUntil[2] < IO_STABLE_WAIT) break;
				m_nPick_Step = 3400; //james 2016.1003 CTL_Lib.Alarm_Error_Occurrence(m_nPick_Step, dWARNING, clsFunc.m_strAlarmCode);
				m_bDvcWaitChk_Falg = false;
				break;
			}
		}
		break;

	case 3400:
		if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8) //test site �۾��̸� 
		{
			m_nPick_Step = 3410;
		}
		else 
		{
			m_nPick_Step = 3500;
		}
		break;

	case 3410:
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Ejector, st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{ 			
			m_nPick_Step = 3420;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 3410;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(2300, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 3410;
		}		
		break;	

	case 3420:
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{ 			
			m_nPick_Step = 3500;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 3420;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			//CTL_Lib.Alarm_Error_Occurrence(2310, dWARNING, st_alarm_info.strCode);
			//2016.1120
			m_nPick_Step = 3430;
		}		
		break;

	case 3430://2016.1120
		nRet_1 = COMI.HomeCheck_Mot(m_nRobot_Z, MOT_SENS_ELM, MOT_TIMEOUT) ; //
		if(nRet_1 == BD_GOOD)
		{
			m_nPick_Step = 3440;
		}
		else if(nRet_1 == BD_ERROR)
		{
			CTL_Lib.Alarm_Error_Occurrence(2311, dWARNING, COMI.strCode);
			m_nPick_Step = 3430;
		}			
		break;

	case 3440:
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{ 			
			m_nPick_Step = 3500;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 3440;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(2312, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 3440;
		}		
		break;
	/////////////////////////////////////////////////////////////////////////
	// load/unload buffer���� �ε� ���縦 �������� ������ �׸��� ����� �κ����� �Ѵ� 
	// �׿� ����Ʈ�� �׸��۰� ��� �׷� �ʿ� ���� 
	/////////////////////////////////////////////////////////////////////////
	case 3500:
		if(nWork_Site == THD_LD_BUFF)
		{
			for(i = 0; i < m_nPickerPara; i++)  //4
			{
				if(nTest_Site == TESTSITE_LEFT) //left site�� �۾��ϱ� ���� �۾� ���϶�
				{
					m_npBuffer_YesNo[i] =  m_npPicker_YesNo[(m_nPickerPara - 1) - i]; //3,2,1,0 ���� �ݴ�� �۾���ġ ���� 
				}
				else if(nTest_Site == TESTSITE_RIGHT) //right sitet�� �۾��ϱ� ���� �۾� ���϶�
				{
					m_npBuffer_YesNo[i] = m_npPicker_YesNo[i];
				}
			}

			 if(st_sync_info.nTestRbt_Dvc_Req[nWork_Site][0] == CTL_READY && st_sync_info.nTestRbt_Dvc_Req[nWork_Site][1] == WORK_PICK) //�ε� ���۰� �غ�Ǿ� �ִ� 
			 {				 //james 2016.1005 = CTL_READY
				 m_nPick_Step = 3510;
			 }		
		}
		else  //if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF || nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8)
		{//���� ���� ���� 
			 m_nPick_Step = 4000;
		}		
		break;

	/////////////////////////////////////////////////////////////////////////
	// load/unload buffer���� �ε� ���縦 �������� ������ �׸��� ����� �κ����� �Ѵ� 
	// load buffer �� �׸��� �� �κ�Ʈ�� ���� ���� ��ġ�� �׸��۸� open�Ͽ� �κ��� ���� �� �� �ְ� �Ѵ�
	/////////////////////////////////////////////////////////////////////////
	case 3510:
		clsRunDvcLdBuffer.Set_Gripper_OnOff(0, IO_ON, m_npBuffer_YesNo); //�ε� ������ �׸��۸� open�Ͽ� �κ��� ���縦 ���� �� �� �ְ� �Ѵ� 
		m_nPick_Step = 3520;
		break;

	case 3520:
		nRet_1 = clsRunDvcLdBuffer.Chk_Gripper_OnOff(0, IO_ON, m_npBuffer_YesNo);
		if(nRet_1 == RET_GOOD)
		{
			m_bDvcWaitChk_Falg = false;
			m_nPick_Step = 3530;
		}
		else if(nRet_1 == RET_ERROR)
		{			 
			CTL_Lib.Alarm_Error_Occurrence(2320, dWARNING, m_strAlarmCode);
			m_nPick_Step = 3510;
		}
		break;

	case 3530://james 2016.0922 �߰� 
		nRet_1 = clsFunc.Check_BufferStatus(1, THD_LD_BUFF, CTL_NO, m_npBuffer_YesNo, m_npBuff_Status, m_npBuff_OutputStatus); //���������� ���� ���°� ��ġ�ؾ� �Ѵ� 
		if(nRet_1 == RET_GOOD)
		{//���� ����, //Enable �Ǿ� �ִ� ���� ��ġ�� ��� ä���� ����, �׽�Ʈ �κ� ����Ʈ�� �̼۰����ϴ� 
			m_nPick_Step = 4000;
		}
		else if(nRet_1== RET_ERROR)
		{//���� ���� ä��� �۾��� ������ ���� ���� 
			CTL_Lib.Alarm_Error_Occurrence(2330, dWARNING, clsFunc.m_strAlarmCode);
			m_nPick_Step = 3510;
		} 
		break;
		
		//james 2016.0922 m_nPick_Step = 4000;
		//break; 
	/////////////////////////////////////////////////////////////////////////		

	case 4000:
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{
			m_bDvcWaitChk_Falg = false;
			m_nPick_Step = 4100;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 4000;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(2340, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 4000;
		}
		break; 

	case 4100:
		// jtkim 20160831
		//if(nWork_Site == THD_LD_BUFF)
		if(nWork_Site == THD_LD_BUFF)
		{
			
			st_sync_info.nTestRbt_Dvc_Req[nWork_Site][0] = CTL_FREE; //test robot�� load buffer���� ����Ϸ� 
			st_sync_info.nAutoBufferIn = CTL_NO;
		}
		m_nPick_Step = 5000;
		break;

	case 5000:
		nRet_1 = clsFunc.Check_PickerStatus(1, THD_TEST_RBT, CTL_YES, m_npPicker_YesNo, m_npPicker_Vacuum_Status, m_npPicker_OutputStatus);
		if (nRet_1 != RET_GOOD) //
		{
			if(m_bDvcWaitChk_Falg == false)
			{
				m_bDvcWaitChk_Falg =  true;
				m_dwWaitUntil[0] = GetCurrentTime();
				break;
			}
			else
			{
				m_dwWaitUntil[1] = GetCurrentTime();
				m_dwWaitUntil[2] = m_dwWaitUntil[1] - m_dwWaitUntil[0];
				if(m_dwWaitUntil[2] < IO_STABLE_WAIT) break;
			}
			//20161026 ���� damage�� ���� �ҷ��� �۾��ڰ� ���縦 �ٿ���� �Ѵ�
			CTL_Lib.Alarm_Error_Occurrence(2349, dWARNING,  clsFunc.m_strAlarmCode);
			break;
		}

		for (i = m_nFirstPicker_Num; i < m_nPickerPara; i++)
		{
			// jtkim 20160905
			//if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_NO && m_npPicker_YesNo[i] == CTL_YES && m_npPicker_Vacuum_Status[i] == CTL_YES)
			if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_NO && m_npPicker_YesNo[i] == CTL_YES
				&& (m_npPicker_Vacuum_Status[i] == IO_ON && m_npPicker_OutputStatus[i] == IO_OFF))
			{
				if(nWork_Site == THD_LD_BUFF)
				{
					if(i >= 0 && m_npFindWorkPosYXCPB[1] + (i - m_nFirstPicker_Num) < st_recipe_info.nLdUldBuffer_Num)
					{
						m_npPicker_YesNo[i] = CTL_NO; //�۾��� ��Ŀ ���� Ŭ����

						if(nTest_Site == TESTSITE_LEFT) //left site�� �۾��ϱ� ���� �۾� ���϶�
						{
							nFind_JobPos = (st_recipe_info.nLdUldBuffer_Num - 1) - i ; //LEFT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ���������� 3,2,1,0 �� �ݴ��̴� 
								

							//�Ʒ� �Լ����� ������� ��ġ ������ �����Ѵ�
							m_npSet_WorkPosYXCPB[0]  = nFind_JobPos; //����sms Y�� ��� //������ ��ġ ����
							m_npSet_WorkPosYXCPB[1]  = nFind_JobPos; //qjvjsms X�� ������� ���� //������ ��ġ ����
							m_npSet_WorkPosYXCPB[3]  = i; //picker ��ġ ����

							clsFunc.Data_Exchange_PickPlace(PICKER_PICK_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //��Ŀ �� Ʈ���� ���� ���� �Լ�
							//�� �Լ����� ��� ������ ���� �� Ŭ���� �Ѵ�
							//st_picker[m_nRobotSite].st_pcb_info[y].nYesNo = CTL_YES;					//��Ŀ ���� ��
							//st_tray_info[nWork_Site].st_pcb_info[m_npFirstPosYXP[0] + (y - m_n_FirstPicker_Num)][m_npFirstPosYXP[1]].nYesNo = CTL_NO; //Ʈ���� ���� Ŭ����
							//st_count_info.nSSD_Input[0]++;
						}
						else if(nTest_Site == TESTSITE_RIGHT) //RIGHT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ����������0,1,2,3 �� ��ġ ������ �����ϴ� 
						{
							nFind_JobPos = i ; //RIGHT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ����������0,1,2,3 �� ��ġ ������ �����ϴ�  

							//�Ʒ� �Լ����� ������� ��ġ ������ �����Ѵ�
							m_npSet_WorkPosYXCPB[0]  = nFind_JobPos; //����sms Y�� ��� //������ ��ġ ����
							m_npSet_WorkPosYXCPB[1]  = nFind_JobPos; //qjvjsms X�� ������� ���� //������ ��ġ ����
							m_npSet_WorkPosYXCPB[3]  = i; //picker ��ġ ����

							clsFunc.Data_Exchange_PickPlace(PICKER_PICK_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //��Ŀ �� Ʈ���� ���� ���� �Լ� 
						}
						//2017.0109
						st_picker[m_nRobotSite].st_pcb_info[i].strLogKey[0]	= _T("LOAD_BUFFER");
						st_picker[m_nRobotSite].st_pcb_info[i].strLogData[0]	= st_picker[m_nRobotSite].st_pcb_info[i].strSerialNo;
						st_picker[m_nRobotSite].st_pcb_info[i].strMaterial.Format(_T("%s"), st_picker[m_nRobotSite].st_pcb_info[i].strSerialNo);
						st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[0] = _T("LOAD_BUFFER");
						st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[1] = _T("TEST_SITE");

						clsLog.LogTransfer(_T("TESTSITE_ROBOT"),
											_T("MOVE_CDIMM"),
											0,
											st_picker[m_nRobotSite].st_pcb_info[i].strMaterial,
											_T("CDIMM"),
											st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[0],
											st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[1],
											1, 
											st_picker[m_nRobotSite].st_pcb_info[i].strLogKey,
											st_picker[m_nRobotSite].st_pcb_info[i].strLogData);

						//kwlee 2017.0905
						if (st_basic_info.nModeXgem == YES)
						{
							if (st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == YES)
							{
								clsXgem.OnMcProductBufferToSocket(START,st_picker[m_nRobotSite].st_pcb_info[i].nTrayCnt,st_picker[m_nRobotSite].st_pcb_info[i].strSerialNo,nFind_JobPos,st_sync_info.nTestSite_Target_THD_Work_Site[2]);
							}
						}
						////
					}					
				} 
				else if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8)
				{
					if(i >= 0 && m_npFindWorkPosYXCPB[1] + (i - m_nFirstPicker_Num) < st_recipe_info.nTestSocket_Num)
					{
						m_npPicker_YesNo[i] = CTL_NO; //�۾��� ��Ŀ ���� Ŭ����

						if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_4)
						//{//if(st_test_site_info[nWork_Site].nSite_Pos == TESTSITE_LEFT) //left site�� �۾��ϱ� ���� �۾� ���϶�
						{
							nFind_JobPos = (st_recipe_info.nTestSocket_Num - 1) - i ; //LEFT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ���������� 3,2,1,0 �� �ݴ��̴� 
							nFind_JobPos = nFind_JobPos - nStart_FixPos; //st_test_site_info[nWork_Site].nStart_FixPos;								

							//�Ʒ� �Լ����� ������� ��ġ ������ �����Ѵ�
							m_npSet_WorkPosYXCPB[0]  = nFind_JobPos; //Test Site�� Y�� ������ ���� //������ ��ġ ����
							m_npSet_WorkPosYXCPB[1]  = nFind_JobPos; //Test Site�� X�� ��� //������ ��ġ ����
							m_npSet_WorkPosYXCPB[3]  = i; //picker ��ġ ����

							clsFunc.Data_Exchange_PickPlace(PICKER_PICK_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //��Ŀ �� Ʈ���� ���� ���� �Լ�
							//�� �Լ����� ��� ������ ���� �� Ŭ���� �Ѵ�
							//st_picker[m_nRobotSite].st_pcb_info[y].nYesNo = CTL_YES;					//��Ŀ ���� ��
							//st_tray_info[nWork_Site].st_pcb_info[m_npFirstPosYXP[0] + (y - m_n_FirstPicker_Num)][m_npFirstPosYXP[1]].nYesNo = CTL_NO; //Ʈ���� ���� Ŭ����
							//st_count_info.nSSD_Input[0]++;
						}
						else if(nWork_Site >= THD_TESTSITE_5 && nWork_Site <= THD_TESTSITE_8)
						//{//if(st_test_site_info[nWork_Site].nSite_Pos == TESTSITE_RIGHT) //RIGHT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ����������0,1,2,3 �� ��ġ ������ �����ϴ� 
						{
							nFind_JobPos = i ; //LEFT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ���������� 3,2,1,0 �� �ݴ��̴� 
							nFind_JobPos = nFind_JobPos + nStart_FixPos; //st_test_site_info[nWork_Site].nStart_FixPos;								

							//�Ʒ� �Լ����� ������� ��ġ ������ �����Ѵ�
							m_npSet_WorkPosYXCPB[0]  = nFind_JobPos; //Test Site�� Y�� ������ ���� //������ ��ġ ����
							m_npSet_WorkPosYXCPB[1]  = nFind_JobPos; //Test Site�� X�� ��� //������ ��ġ ����
							m_npSet_WorkPosYXCPB[3]  = i; //picker ��ġ ����

							clsFunc.Data_Exchange_PickPlace(PICKER_PICK_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //��Ŀ �� Ʈ���� ���� ���� �Լ� 
						}

						//2017.0109
						st_picker[m_nRobotSite].st_pcb_info[i].strLogKey[0]	= _T("TEST_SITE");
						st_picker[m_nRobotSite].st_pcb_info[i].strLogData[0]	= st_picker[m_nRobotSite].st_pcb_info[i].strSerialNo;

						st_picker[m_nRobotSite].st_pcb_info[i].strMaterial.Format(_T("%s"), st_picker[m_nRobotSite].st_pcb_info[i].strSerialNo);
						st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[0] = _T("TEST_SITE");
						st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[1] = _T("UNLOAD_BUFFER");//��δ��� ���� ���׽�Ʈ�� ���� �𸥴�.

						clsLog.LogTransfer(_T("TESTSITE_ROBOT"),
											_T("MOVE_CDIMM"),
											0,
											st_picker[m_nRobotSite].st_pcb_info[i].strMaterial,
											_T("CDIMM"),
											st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[0],
											st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[1],
											1, 
											st_picker[m_nRobotSite].st_pcb_info[i].strLogKey,
											st_picker[m_nRobotSite].st_pcb_info[i].strLogData);

					}
					//kwlee 2017.0905
					if (st_basic_info.nModeXgem == YES)
					{
						if (st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == YES)
						{
							clsXgem.OnMcProductSocketToBuffer(START,st_picker[m_nRobotSite].st_pcb_info[i].nTrayCnt,st_picker[m_nRobotSite].st_pcb_info[i].strSerialNo,nFind_JobPos,nWork_Site);
						}
					}
					////////
				}
				else if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF)
				{//
					if(nTest_Site == TESTSITE_LEFT) //left site�� �۾��ϱ� ���� �۾� ���϶�
					{
						nFind_JobPos = m_nFirst_Buff_Num - ((i - m_nFirstPicker_Num) * 2);  //13,11,9,7,5,3,1 ��ġ 
						if(nFind_JobPos >= 0  && nFind_JobPos >= 0 && nFind_JobPos < st_recipe_info.nRetestBuffer_Num)
						{
							m_npPicker_YesNo[i] = CTL_NO; //�۾��� ��Ŀ ���� Ŭ����

							//�Ʒ� �Լ����� ������� ��ġ ������ �����Ѵ�
							m_npSet_WorkPosYXCPB[0]  = nFind_JobPos; //����sms Y�� ��� //������ ��ġ ����
							m_npSet_WorkPosYXCPB[1]  = nFind_JobPos; //qjvjsms X�� ������� ���� //������ ��ġ ����
							m_npSet_WorkPosYXCPB[3]  = i; //picker ��ġ ����

							clsFunc.Data_Exchange_PickPlace(PICKER_PICK_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //��Ŀ �� Ʈ���� ���� ���� �Լ�
							//�� �Լ����� ��� ������ ���� �� Ŭ���� �Ѵ�
							//st_picker[m_nRobotSite].st_pcb_info[y].nYesNo = CTL_YES;					//��Ŀ ���� ��
							//st_tray_info[nWork_Site].st_pcb_info[m_npFirstPosYXP[0] + (y - m_n_FirstPicker_Num)][m_npFirstPosYXP[1]].nYesNo = CTL_NO; //Ʈ���� ���� Ŭ����
							//st_count_info.nSSD_Input[0]++;

							///////////////////////////////////////////////////////////////////////////////////////////
							//2016.1203
							st_product_info.nRetest++;
							///////////////////////////////////////////////////////////////////////////////////////////
						}
						else
						{
							nFind_JobPos = 0; //james test 2016.0907
						}
						//kwlee 2017.0905
						if (st_basic_info.nModeXgem == YES)
						{
							if (st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == YES)
							{
								clsXgem.OnMcRetestStart(st_picker[m_nRobotSite].st_pcb_info[i].strSerialNo);
							}
						}
						//

					}
					else if(nTest_Site == TESTSITE_RIGHT) //RIGHT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ����������0,1,2,3 �� ��ġ ������ �����ϴ� 
					{
						nFind_JobPos = m_nFirst_Buff_Num + ((i - m_nFirstPicker_Num) * 2);
						if(nFind_JobPos >= 0 && nFind_JobPos >= 0 && nFind_JobPos < st_recipe_info.nRetestBuffer_Num)
						{
							m_npPicker_YesNo[i] = CTL_NO; //�۾��� ��Ŀ ���� Ŭ����

							//�Ʒ� �Լ����� ������� ��ġ ������ �����Ѵ�
							m_npSet_WorkPosYXCPB[0]  = nFind_JobPos; //����sms Y�� ��� //������ ��ġ ����
							m_npSet_WorkPosYXCPB[1]  = nFind_JobPos; //qjvjsms X�� ������� ���� //������ ��ġ ����
							m_npSet_WorkPosYXCPB[3]  = i; //picker ��ġ ����

							clsFunc.Data_Exchange_PickPlace(PICKER_PICK_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //��Ŀ �� Ʈ���� ���� ���� �Լ� 

							///////////////////////////////////////////////////////////////////////////////////////////
							//2016.1203
							st_product_info.nRetest++;
							///////////////////////////////////////////////////////////////////////////////////////////
						}
						else
						{
							nFind_JobPos = 0; //james test 2016.0907
						}

					}

					//2016.1124
					//st_sync_info.nSmema_Tray_Input_Req = CTL_YES;
					st_handler_info.nAutoDirectionUseChk = CTL_YES;

					//2017.0109
					st_picker[m_nRobotSite].st_pcb_info[i].strLogKey[0]	= _T("RETEST_SITE");
					st_picker[m_nRobotSite].st_pcb_info[i].strLogData[0]	= st_picker[m_nRobotSite].st_pcb_info[i].strSerialNo;

					st_picker[m_nRobotSite].st_pcb_info[i].strMaterial.Format(_T("%s"), st_picker[m_nRobotSite].st_pcb_info[i].strSerialNo);
					if( nWork_Site == THD_RETEST_1_BUFF )// || nWork_Site == THD_RETEST_2_BUFF)
					{
						st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[0] = _T("RETEST_SITE#1");
					}
					else
					{
						st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[0] = _T("RETEST_SITE#2");
					}
					st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[1] = _T("TEST_SITE");

					clsLog.LogTransfer(_T("TESTSITE_ROBOT"),
										_T("MOVE_CDIMM"),
										0,
										st_picker[m_nRobotSite].st_pcb_info[i].strMaterial,
										_T("CDIMM"),
										st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[0],
										st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[1],
										1, 
										st_picker[m_nRobotSite].st_pcb_info[i].strLogKey,
										st_picker[m_nRobotSite].st_pcb_info[i].strLogData);
				}		 
			}
		}//for

		if (nRet_1 == RET_GOOD) // 1000 step���� ������ ��Ŀ ������ �°� ��� ������ ������ ��� ��
		{
			//ȭ�� ���� ����

			m_nDvcRepickCount = 0;
			m_nPartialPickCount = 0;

			m_bDvcWaitChk_Falg = false;

			// jtkim 20160930 Log
			if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8)
			{
				// jtkim 20161008 ������å�� ��û
				//clsLog.OnLogTestSite(6, nWork_Site, nTest_Site - TESTSITE_LEFT, st_test_site_info[nWork_Site]);
				//2016.1018
				if( nStart_FixPos == 0 )
				{
					clsLog.OnLogTestSite(6, nWork_Site, 0, st_test_site_info[nWork_Site]);
				}
				else
				{
					clsLog.OnLogTestSite(6, nWork_Site, 1, st_test_site_info[nWork_Site]);
				}
			}

			// jtkim 20150216 conveyor/loader tray ȭ�� ǥ��
			if (st_handler_info.cWndMain != NULL)
			{
				st_handler_info.cWndMain->PostMessage(WM_WORK_DISPLAY, MAIN_LD_PICKER_DISPLAY, 0);
				st_handler_info.cWndMain->PostMessage(WM_WORK_DISPLAY, MAIN_LD_TRAY_DISPLAY, 0);
				st_handler_info.cWndMain->PostMessage(WM_WORK_DISPLAY, MAIN_RETEST_BUF_DISPLAY, 0);
			}

			st_sync_info.nAutoBufferIn = CTL_YES;//2016.1117
			m_nPick_Step = 6000;
		}
		else if (nRet_1 == RET_ERROR) //������ ��Ŀ�� �Ѱ��� ����� ���� �������� �̰����� �̵�
		{
			CTL_Lib.Alarm_Error_Occurrence(2350, dWARNING,  clsFunc.m_strAlarmCode);
			//m_nPick_Step = 0;
		}
		break;

	case 6000:
		nFuncRet = RET_GOOD;
		m_nPick_Step = 0;
		break;

	default : 
		break;
		
	}

	return nFuncRet;

}

int CRunTestSiteRobot::Process_DVC_Place(int nMode, int nWork_Site, int nTest_Site, int nStart_FixPos, int Opt) //int nStart_FixPos = Left(0), Right(4) �ΰ��� ��ġ�� �ϳ��̴� , ������ ���۱��� �������� �۾� //2016.0814 
{
	int nFuncRet = RET_PROCEED;
	int nRet_1, nRet_2, nRet_3=0, i, nFlag;
	int nFind_JobPos=0;
	CString strTemp;
	
	 
	clsFunc.OnThreadFunctionStepTrace(26, m_nPlace_Step);

	switch(m_nPlace_Step)
	{
	case 0:
		/*
		if(nWork_Site == THD_LD_BUFF)
		{
			m_nSite_WorkSize_Num = st_recipe_info.nBufferY; //4
		}
		else if(nWork_Site == THD_ULD_BUFF)
		{//���� ���� ���� 
			m_nSite_WorkSize_Num = st_recipe_info.nBufferY; //4
		}
		else if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8)
		{
			m_nSite_WorkSize_Num = st_recipe_info.nTestSocket_Num; //8
		}
		else if(nWork_Site == THD_RETEST_1_BUFF)
		{
			m_nSite_WorkSize_Num = st_recipe_info.nRetestBuffer_Num; //16
		}
		else if(nWork_Site == THD_RETEST_2_BUFF)
		{
			m_nSite_WorkSize_Num = st_recipe_info.nRetestBuffer_Num; //16
		}*/

		m_bDvcWaitChk_Falg =  false;
		m_nPickRetry_Cnt = 0;
		//2016.1201
		m_nDirRetry_Cnt = 0;
		m_dwDirOffset = 0.0;
		m_nPlace_Step = 100;
		break;

	case 100:
		m_nPlace_Step = 900;
		break;

		m_dCurrentPos[m_nRobot_Z] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[m_nRobot_Z] > st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
		{
			m_nPlace_Step = 0;
			break;
		}
		m_dCurrentPos[m_nRobot_Ejector] = COMI.Get_MotCurrentPos(m_nRobot_Ejector);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[m_nRobot_Ejector] > st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
		{
			m_nPlace_Step = 0;
			break;
		}
		m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_SAFETY];
		m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_SAFETY];
		nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate); 
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{
			m_nPlace_Step = 400;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 300;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(2360, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 300;
		}
		break;

	case 900:
		m_dCurrentPos[m_nRobot_Z] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[m_nRobot_Z] > st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
		{
			m_nPlace_Step = 910;
		}
		else
		{
			m_nPlace_Step = 920;
		}
		break;

	case 910:
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{
			m_nPlace_Step = 920;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 900;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(2370, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 910;
		}
		break;

	case 920:
		m_dCurrentPos[m_nRobot_Ejector] = COMI.Get_MotCurrentPos(m_nRobot_Ejector);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[m_nRobot_Ejector] > st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[m_nRobot_Ejector].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
		{
			m_nPlace_Step = 930;
		}
		else
		{
			m_nPlace_Step = 940;
		}
		break;

	case 930:
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Ejector, st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{
			m_nPlace_Step = 940;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 930;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(2380, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 930;
		}
		break;

	case 940:
		m_bDvcWaitChk_Falg = false;
		m_nPlace_Step = 1000;
		break; 

	///////////////////////////////////////////////////////////////////////
	// �۾��� ������ ã��, ����
	///////////////////////////////////////////////////////////////////////
	case 1000:
		m_dCurrentPos[m_nRobot_Z] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[m_nRobot_Z] > st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
		{
			m_nPlace_Step = 900;
			break;
		}
		m_dCurrentPos[m_nRobot_Ejector] = COMI.Get_MotCurrentPos(m_nRobot_Ejector);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[m_nRobot_Ejector] > st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
		{
			m_nPlace_Step = 900;
			break;
		}

		nRet_1 = clsFunc.Check_PickerStatus(0, THD_TEST_RBT, CTL_YES, m_npPicker_YesNo, m_npPicker_Vacuum_Status, m_npPicker_OutputStatus); //nMode == 0 �̸� ��������, nMode == 1�̸� ��������� ������ ���� ��Ŀ ���¸� Ȯ���Ѵ�,
		if(nRet_1 == RET_GOOD) //���� ������ ���� ��Ŀ ���¸� üũ 
		{//��Ŀ ������ ���� ���°� �����̴�
		}
		else// if(nRet_1 == RET_ERROR)
		{
			if(m_bDvcWaitChk_Falg == false)
			{
				m_bDvcWaitChk_Falg =  true;
				m_dwWaitUntil[0] = GetCurrentTime();
				break;
			}
			else
			{
				m_dwWaitUntil[1] = GetCurrentTime();
				m_dwWaitUntil[2] = m_dwWaitUntil[1] - m_dwWaitUntil[0];
				if(m_dwWaitUntil[2] < IO_STABLE_WAIT) break;
				CTL_Lib.Alarm_Error_Occurrence(2390, dWARNING, clsFunc.m_strAlarmCode);
				m_bDvcWaitChk_Falg = false;
				break;
			}
		}

///////////////////////////////////////////////////////////////////////
// Retest Bin ó�� �ɼ����� ���׽�Ʈ ���� ��ġ���� �� ��Ŀ ��������  
// �Լ� ���������� �̸������� �����Ͽ� step�� jump �ؾ� ��
///////////////////////////////////////////////////////////////////////
		if(nMode == BIN_RETESTBIN || nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF)
		{
			m_nFirstTray_Y_Num = m_nFirst_Buff_Num; 
			m_nFirstTray_X_Num = m_nFirst_Buff_Num;

			m_npFindWorkPosYXCPB[0] = m_nFirst_Buff_Num;
			m_npFindWorkPosYXCPB[1] = m_nFirst_Buff_Num;
			m_npFindWorkPosYXCPB[3] = m_nFirstPicker_Num;
			m_nPlace_Step = 2000;
			break;
			//2016.1129
			if( Opt == 1)
			{
				nFlag = 0;
				for(i = 0; i < m_nPickerPara; i++)  //m_n_FirstPicker_Num; y < m_mPickerPara; y++)
				{
					m_npPicker_YesNo[i] = CTL_NO; //�۾��� ��Ŀ���� clear
					if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_YES)
					{
						if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF) 
						{
							if(st_picker[m_nRobotSite].st_pcb_info[i].nBin == BD_DATA_RETEST )
							{
								m_npPicker_YesNo[i] = CTL_YES;

								/*if(nTest_Site == TESTSITE_LEFT) //left site�� �۾��ϱ� ���� �۾� ���϶�
								{
									if(i >= 0 && m_nFirstTray_X_Num + (i - m_nFirstPicker_Num) < st_recipe_info.nRetestBuffer_Num)
									{
								
										nFind_JobPos = (st_recipe_info.nLdUldBuffer_Num - 1) - i ; //LEFT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ���������� 3,2,1,0 �� �ݴ��̴� 
										if(st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_NO)// && st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //Ʈ���̿� ���簡  ������ ���� �����ϰ� ��
										{
											nFlag = 1;
											m_npPicker_YesNo[i] = CTL_YES;
										}
									}
								}
								else if(nTest_Site == TESTSITE_RIGHT) //right sitet�� �۾��ϱ� ���� �۾� ���϶�
								{
									if(i >= 0 && m_nFirstTray_X_Num + (i - m_nFirstPicker_Num) < st_recipe_info.nRetestBuffer_Num)
									{
										nFind_JobPos = i ; //RIGHT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ����������0,1,2,3 �� ��ġ ������ �����ϴ�  
										if(st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_NO)// && st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //Ʈ���̿� ���簡  ������ ���� �����ϰ� ��
										{
											nFlag = 1;
											m_npPicker_YesNo[i] = CTL_YES;
										}
									}
								}
								else
								{
									//break;
								}*/	
							}
						}
					}
				}
			}
			m_nMove_Flag[0] = CTL_NO; m_nMove_Flag[1] = CTL_NO;
			m_nPlace_Step = 2000;
			break;
		}
///////////////////////////////////////////////////////////////////////

		//�۾� ���� ��ġ�� ã�´�
		nFlag = 0;
		m_nFirstTray_Y_Num = -1; m_nFirstTray_Y_Num = -1; m_nFirstPicker_Num = -1; //flag clear
		for(i = 0; i < m_nPickerPara; i++)
		{
			if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_YES && st_picker[m_nRobotSite].st_pcb_info[i].nEnable == CTL_YES)//��Ŀ�� ���簡 ��� �ִٴ� ������ ����
			{
				if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF)
				{
					m_nFirstPicker_Num = i; //������ ���� ��Ŀ ��ġ�� ���� 
				}
				else //if(nWork_Site == THD_LD_BUFF || nWork_Site == THD_LD_BUFF || (nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8) )
				{
					m_nFirstPicker_Num = 0; //��Ŀ�� ���� ��ġ�� �������� �۾��Ѵ� 
				}
				
				i = m_nPickerPara;
			}
		}

		if(m_nFirstPicker_Num == -1)
		{
			//��Ŀ�� ���̻� �۾��� ���� ����
			m_nPlace_Step = 0;
			nFuncRet = RET_PICKER_NOT_FIND;
			break;
		}

/* Retest Bin ó�� �ɼ����� ���׽�Ʈ ���� ��ġ���� �� ��Ŀ �������� �Լ� ���������� �̸������� �����Ͽ� step�� jump �ؾ� ��
////////////////////////////////////  james 2016.0814
		if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF)
		{
			//Find_RetestBufferWork_Pos(int n_RbtSite, int n_TestSite, int n_BuffSite, int n_BuffFirstNo, int n_SortBin, int n_PickerYesNO, int n_BuffYesNO, int *np_FindPosYXCPB, int *np_Picker_YesNo)
			if(st_buffer_info[nWork_Site].nTestSite_LR_Pos == TESTSITE_LEFT) //test left site�� ���� �۾��ϱ� ���� �۾� ���϶�, ���׽�Ʈ ���۴� �׻� ���� �⺻ ����(Right Test Site ����)�� �����Ѵ�, LEFT SITE���� ���� ���۸� ȸ���Ͽ� 1�� �� ������ ����� 
			{//���۰� ȸ���Ͽ� ���� 1�� ��ġ�� �ݴ�� �Ǿ��ִ� 
				nRet_1 = clsFunc.Find_RetestBufferWork_Pos(THD_TEST_RBT, st_buffer_info[nWork_Site].nTestSite_LR_Pos, nWork_Site, 14, BIN_RETESTBIN, CTL_YES, CTL_NO, m_npFindWorkPosYXCPB, m_npPicker_YesNo); //14 == ¦���� ���� ��ġ ã�� 14,12,10,8,6,4,2,0
				if(nRet_1 == RET_GOOD)
				{
					m_nFirstTray_Y_Num = m_npFindWorkPosYXCPB[0]; m_nFirstTray_X_Num = m_npFindWorkPosYXCPB[1];
					m_nFirst_Buff_Num = m_npFindWorkPosYXCPB[4];
					m_nPlace_Step = 2000;
					break;
				}

				nRet_1 = clsFunc.Find_RetestBufferWork_Pos(THD_TEST_RBT, st_buffer_info[nWork_Site].nTestSite_LR_Pos, nWork_Site, 13, BIN_RETESTBIN, CTL_YES, CTL_NO, m_npFindWorkPosYXCPB, m_npPicker_YesNo); //13 == Ȧ���� ���� ��ġ ã�� 13,11,9,7,5,3,1
				if(nRet_1 == RET_GOOD)
				{
					m_nFirstTray_Y_Num = m_npFindWorkPosYXCPB[0]; m_nFirstTray_X_Num = m_npFindWorkPosYXCPB[1];
					m_nFirst_Buff_Num = m_npFindWorkPosYXCPB[4];
					m_nPlace_Step = 2000;
					break;
				}
				else
				{//�۾��� �� �� ���� 
					m_nPlace_Step = 0;
					nFuncRet = RET_TRAY_NOT_FIND;
					break;
				}
			}
			else if(st_buffer_info[nWork_Site].nTestSite_LR_Pos == TESTSITE_RIGHT) //test right site�� ���� �۾��ϱ� ���� �۾� ���϶�, ���׽�Ʈ ���۴� �׻� ���� �⺻ ����(Right Test Site ����)�� �����Ѵ�, LEFT SITE���� ���� ���۸� ȸ���Ͽ� 1�� �� ������ ����� 
			{//���ۿ� ��Ŀ�� ��ġ�� ���� �����̴�
				nRet_1 = clsFunc.Find_RetestBufferWork_Pos(THD_TEST_RBT, st_buffer_info[nWork_Site].nTestSite_LR_Pos, nWork_Site, 0, BIN_RETESTBIN, CTL_YES, CTL_NO, m_npFindWorkPosYXCPB, m_npPicker_YesNo); //0 == ¦���� ���� ��ġ ã�� 0,2,4,6,8,10,12,14
				if(nRet_1 == RET_GOOD)
				{
					m_nFirstTray_Y_Num = m_npFindWorkPosYXCPB[0]; m_nFirstTray_X_Num = m_npFindWorkPosYXCPB[1];
					m_nFirst_Buff_Num = m_npFindWorkPosYXCPB[4];
					m_nPlace_Step = 2000;
					break;
				}

				nRet_1 = clsFunc.Find_RetestBufferWork_Pos(THD_TEST_RBT, st_buffer_info[nWork_Site].nTestSite_LR_Pos, nWork_Site, 1, BIN_RETESTBIN, CTL_YES, CTL_NO, m_npFindWorkPosYXCPB, m_npPicker_YesNo); //1 == Ȧ���� ���� ��ġ ã�� 1,3,5,7,9,11,13
				if(nRet_1 == RET_GOOD)
				{
					m_nFirstTray_Y_Num = m_npFindWorkPosYXCPB[0]; m_nFirstTray_X_Num = m_npFindWorkPosYXCPB[1];
					m_nFirst_Buff_Num = m_npFindWorkPosYXCPB[4];
					m_nPlace_Step = 2000;
					break;
				}
				else
				{//�۾��� �� �� ���� 
					m_nPlace_Step = 0;
					nFuncRet = RET_TRAY_NOT_FIND;
					break;
				}
			}
		}
		/////////////////////////////////// james 2016.0814 
*/


		nRet_1 = clsFunc.Find_TrayWork_Pos(0, nWork_Site, CTL_NO, m_npFindWorkPosYXCPB, THD_TEST_RBT, st_worker_info.strLotNo_Empty);
		if(nRet_1 == RET_GOOD) //�۾� ������ ���簡 �ִ�
		{
			if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF)
			{
				m_nFirstTray_Y_Num = m_npFindWorkPosYXCPB[0]; //������� ����
				m_nFirstTray_X_Num = m_npFindWorkPosYXCPB[1];
			}
			else if(nWork_Site == THD_LD_BUFF || nWork_Site == THD_ULD_BUFF) 
			{
				m_nFirstTray_Y_Num = 0;
				m_nFirstTray_X_Num = 0;
			}
			else if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8)
			{
				m_nFirstTray_Y_Num = nStart_FixPos;//������� ���� m_nFirstTray_Y_Num = st_test_socket_info[st_sync_info.nTestSite_Target_THD_Work_Site].nWorkSocket_StartPos; 
				m_nFirstTray_X_Num = nStart_FixPos; //0 �Ǵ� 4 ��  st_test_socket_info[st_sync_info.nTestSite_Target_THD_Work_Site].nWorkSocket_StartPos;
			}
		}
		else if(nRet_1 == RET_SKIP) //���̻� ���簡 ����
		{	//��� ������ �� ����
			m_nPlace_Step = 0;
			nFuncRet = RET_TRAY_NOT_FIND;
			break;
		}

		nFlag = 0;
		if(m_nFirstPicker_Num != -1 && m_nFirstTray_Y_Num != -1 && m_nFirstTray_X_Num != -1) //�۾��� ������ �� ������ ���� �۾��� �����Ѵ�
		{
			for(i = 0; i < m_nPickerPara; i++)  //m_n_FirstPicker_Num; y < m_mPickerPara; y++)
			{
				m_npPicker_YesNo[i] = CTL_NO; //�۾��� ��Ŀ���� clear
				if(i < m_nFirstPicker_Num) continue;
				if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_YES)// && st_picker[m_nRobotSite].st_pcb_info[i].nEnable == CTL_YES)
				{
					if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF) //�׽�Ʈ �κ��� ���� ������ ���� ���� ��Ŀ�� ������ ������ ��ġ���� 1:1 �� ���Ͽ� �浹�� ����� �Ѵ� 
					{//���׽�Ʈ ������ �̰����� ���� �ʴ´�, ������� ����  
						if(st_picker[m_nRobotSite].st_pcb_info[i].nBin == BD_DATA_RETEST )//|| st_picker[m_nRobotSite].st_pcb_info[i].nBin == BD_DATA_REJECT)
						{
							if(i >= 0 && m_nFirstTray_Y_Num + (i - m_nFirstPicker_Num) < st_recipe_info.nLdUldBuffer_Num)//4
							{
								if(nTest_Site == TESTSITE_LEFT) //left site�� �۾��ϱ� ���� �۾� ���϶�
								{
									nFind_JobPos = (st_recipe_info.nLdUldBuffer_Num - 1) - i ; //LEFT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ���������� 3,2,1,0 �� �ݴ��̴� 
									if(st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_NO)// && st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //Ʈ���̿� ���簡  ������ ���� �����ϰ� ��
									{
										m_npPicker_YesNo[i] = CTL_YES;
										nFlag++;
									}
								}
								else if(nTest_Site == TESTSITE_RIGHT) //right sitet�� �۾��ϱ� ���� �۾� ���϶�
								{
									nFind_JobPos = i ; //RIGHT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ����������0,1,2,3 �� ��ġ ������ �����ϴ�  
									if(st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_NO)// && st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //Ʈ���̿� ���簡  ������ ���� �����ϰ� ��
									{
										m_npPicker_YesNo[i] = CTL_YES;
										nFlag++;
									}
								}
								else
								{
									//break;
								}
							}		
						}
					}
					else if(nWork_Site == THD_LD_BUFF)// || nWork_Site == THD_ULD_BUFF) 
					{
						 if(i >= 0 && m_nFirstTray_Y_Num + (i - m_nFirstPicker_Num) < st_recipe_info.nLdUldBuffer_Num)//4
						{
							if(nTest_Site == TESTSITE_LEFT) //left site�� �۾��ϱ� ���� �۾� ���϶�
							{
								nFind_JobPos = (st_recipe_info.nLdUldBuffer_Num - 1) - i ; //LEFT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ���������� 3,2,1,0 �� �ݴ��̴� 
								if(st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_NO)// && st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //Ʈ���̿� ���簡  ������ ���� �����ϰ� ��
								{
									m_npPicker_YesNo[i] = CTL_YES;
									nFlag++;
								}
							}
							else if(nTest_Site == TESTSITE_RIGHT) //right sitet�� �۾��ϱ� ���� �۾� ���϶�
							{
								nFind_JobPos = i ; //RIGHT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ����������0,1,2,3 �� ��ġ ������ �����ϴ�  
								if(st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_NO)// && st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //Ʈ���̿� ���簡  ������ ���� �����ϰ� ��
								{
									m_npPicker_YesNo[i] = CTL_YES;
									nFlag++;
								}
							}
							else
							{
								//break;
							}
						}		
					}
					else if(nWork_Site == THD_ULD_BUFF) 
					{
						if(st_picker[m_nRobotSite].st_pcb_info[i].nBin == BD_DATA_GOOD || st_picker[m_nRobotSite].st_pcb_info[i].nBin == BD_DATA_REJECT)
						{
							if(i >= 0 && m_nFirstTray_Y_Num + (i - m_nFirstPicker_Num) < st_recipe_info.nLdUldBuffer_Num)//4
							{
								if(nTest_Site == TESTSITE_LEFT) //left site�� �۾��ϱ� ���� �۾� ���϶�
								{
									nFind_JobPos = (st_recipe_info.nLdUldBuffer_Num - 1) - i ; //LEFT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ���������� 3,2,1,0 �� �ݴ��̴� 
									if(st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_NO)// && st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //Ʈ���̿� ���簡  ������ ���� �����ϰ� ��
									{
										m_npPicker_YesNo[i] = CTL_YES;
										nFlag++;
									}
								}
								else if(nTest_Site == TESTSITE_RIGHT) //right sitet�� �۾��ϱ� ���� �۾� ���϶�
								{
									nFind_JobPos = i ; //RIGHT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ����������0,1,2,3 �� ��ġ ������ �����ϴ�  
									if(st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_NO)// && st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //Ʈ���̿� ���簡  ������ ���� �����ϰ� ��
									{
										m_npPicker_YesNo[i] = CTL_YES;
										nFlag++;
									}
								}
								else
								{
									//break;
								}
							}		
						}
					}
					else if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_4)//LEFT TEST SITE 
					{
						if(i >= 0 && m_nFirstTray_X_Num + (i - m_nFirstPicker_Num) < st_recipe_info.nTestSocket_Num)//8
						{
							nFind_JobPos = (st_recipe_info.nTestSocket_Num - 1) - i ; //LEFT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ���������� 3,2,1,0 �� �ݴ��̴� 
							nFind_JobPos = nFind_JobPos - nStart_FixPos; //st_test_site_info[nWork_Site].nStart_FixPos;


							//////////////////////////////////////////////////////////////////////////
							//2017.0320
							//820000 1 0 "REMOVE_TESTSITE_PICKER1_DVC_BECAUSE_TESTSITE_SCOKET_IS_DISABLE."
							//820001 1 0 "REMOVE_TESTSITE_PICKER2_DVC_BECAUSE_TESTSITE_SCOKET_IS_DISABLE."
							//820002 1 0 "REMOVE_TESTSITE_PICKER3_DVC_BECAUSE_TESTSITE_SCOKET_IS_DISABLE."
							//820003 1 0 "REMOVE_TESTSITE_PICKER4_DVC_BECAUSE_TESTSITE_SCOKET_IS_DISABLE."
							//���� ������ disable�Ǿ� �ִµ� ���縦 ����������� �Ѵٸ� ���⼭ �˶��� �߻��Ͽ� �۾��ڰ� ���縦 �� �� �ֵ��� �Ѵ�.
// 							if( st_test_site_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable != CTL_YES && st_test_site_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_NO)
// 							{//�˶� �߻�
// 								strTemp.Format(_T(" TESTRBT was goint to TESTSITE %d, SOCKET %d was disabled. so It have to be removed at Picker %d device"), nWork_Site - THD_TESTSITE_1 + 1, nFind_JobPos, i);
// 								clsMem.OnAbNormalMessagWrite(strTemp);//�α� ����
// 
// 								m_strAlarmCode.Format(_T("8200%02d"), i); 
// 								CTL_Lib.Alarm_Error_Occurrence(2728, dWARNING, m_strAlarmCode);
// 								return nFuncRet;
// 							}
							//////////////////////////////////////////////////////////////////////////
							if(st_test_site_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_NO )//&& st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //Ʈ���̿� ���簡  ������ ���� �����ϰ� ��
							{
								m_npPicker_YesNo[i] = CTL_YES;
								nFlag++;
							}
						}
					}
					else if(nWork_Site >= THD_TESTSITE_5 && nWork_Site <= THD_TESTSITE_8)//RIGHT TEST SITE 
					{
						if(i >= 0 && m_nFirstTray_X_Num + (i - m_nFirstPicker_Num) < st_recipe_info.nTestSocket_Num)//8
						{
							nFind_JobPos = i ; //RIGHT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ����������0,1,2,3 �� ��ġ ������ �����ϴ�  
							nFind_JobPos = nFind_JobPos + nStart_FixPos; //st_test_site_info[nWork_Site].nStart_FixPos;

							//////////////////////////////////////////////////////////////////////////
							//2017.0320
							//���� ������ disable�Ǿ� �ִµ� ���縦 ����������� �Ѵٸ� ���⼭ �˶��� �߻��Ͽ� �۾��ڰ� ���縦 �� �� �ֵ��� �Ѵ�.
// 							if( st_test_site_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable != CTL_YES && st_test_site_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_NO)
// 							{//�˶� �߻�
// 								strTemp.Format(_T(" TESTRBT was goint to TESTSITE %d, SOCKET %d was disabled. so It have to be removed at Picker %d device "), nWork_Site - THD_TESTSITE_1 + 1, nFind_JobPos, i);
// 								clsMem.OnAbNormalMessagWrite(strTemp);//�α� ����
// 
// 								m_strAlarmCode.Format(_T("8200%02d"), i); 
// 								CTL_Lib.Alarm_Error_Occurrence(2729, dWARNING, m_strAlarmCode);
// 								return nFuncRet;
// 							}
							//////////////////////////////////////////////////////////////////////////

							if(st_test_site_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_NO)// && st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //Ʈ���̿� ���簡  ������ ���� �����ϰ� ��
							{
								m_npPicker_YesNo[i] = CTL_YES;
								nFlag++;
							}
						}
					}
				}
			}

			if(nFlag == 0) //�۾��� ��Ŀ�� �ϳ��� �غ�� �ʾҴ�.
			{ //�̺κ��� �ƿ��� �ȵ� 
				m_nPlace_Step = 0;
				nFuncRet = RET_PICKER_NOT_FIND;//��Ŀ�� ���̻� �۾��� ���� ����
				break;
			}

			//���� �۾� ��ġ�� ����
			m_npFindWorkPosYXCPB[0] = m_nFirstTray_Y_Num; m_npFindWorkPosYXCPB[1] = m_nFirstTray_X_Num; m_npFindWorkPosYXCPB[3] = m_nFirstPicker_Num;

			m_nMove_Flag[0] = CTL_NO; m_nMove_Flag[1] = CTL_NO;
			m_nPlace_Step = 2000;
		}		
		break;

	case 2000:
		//james 2016.0826 ��Ŀ�� ����/�׽�Ʈ ������ ���� �浹������ ���� �߰� 
		if(nWork_Site == THD_LD_BUFF || nWork_Site == THD_ULD_BUFF)
		{//Check_Robot_Buffer_Clash(int nTestDirection, int nRbtTHDSite, int TestBuffTHDSite, int nStartPos, int *nErrorInfoStatus) 
			if(nTest_Site == TESTSITE_LEFT) //LEFT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ���������� 3,2,1,0 �� �ݴ��̴� 
			{
				nRet_1 = COMI.Check_MotPosRange(M_ULD_BUF_SHIFTER_Y, st_motor_info[M_ULD_BUF_SHIFTER_Y].d_pos[P_ULDBUFF_Y_TESTRBT], st_motor_info[M_ULD_BUF_SHIFTER_Y].d_allow);
				nRet_2 = COMI.Check_MotPosRange(M_ULD_BUF_ROTATOR, st_motor_info[M_ULD_BUF_ROTATOR].d_pos[P_ULDBUFF_RTR_TEST_1_LEFT], st_motor_info[M_ULD_BUF_ROTATOR].d_allow);
				if(nRet_1 != BD_GOOD || nRet_2 != BD_GOOD)
				{
					if(nRet_1 != BD_GOOD ) m_strAlarmCode.Format(_T("%02d0006"), M_ULD_BUF_SHIFTER_Y); 
					else m_strAlarmCode.Format(_T("%02d0006"), M_ULD_BUF_ROTATOR); 
					CTL_Lib.Alarm_Error_Occurrence(2400, dWARNING, m_strAlarmCode);
					break;
				}

				nRet_1 = clsFunc.Check_Robot_Buffer_Clash(TESTSITE_LEFT, THD_TEST_RBT, nWork_Site, m_nFirstTray_X_Num, m_nFirstPicker_Num, m_nClash_ErrorInfoStatus); //��Ŀ�� ���۵��� ���� �浹 Ȯ�� 
				if(nRet_1 == RET_ERROR)
				{
					CTL_Lib.Alarm_Error_Occurrence(2410, dWARNING, clsFunc.m_strAlarmCode);		
					break;
				}
			}
			else if(nTest_Site == TESTSITE_RIGHT) //right sitet�� �۾��ϱ� ���� �۾� ���϶�//RIGHT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ����������0,1,2,3 �� ��ġ ������ �����ϴ�  
			{
				nRet_1 = COMI.Check_MotPosRange(M_ULD_BUF_SHIFTER_Y, st_motor_info[M_ULD_BUF_SHIFTER_Y].d_pos[P_ULDBUFF_Y_TESTRBT], st_motor_info[M_ULD_BUF_SHIFTER_Y].d_allow);
				nRet_2 = COMI.Check_MotPosRange(M_ULD_BUF_ROTATOR, st_motor_info[M_ULD_BUF_ROTATOR].d_pos[P_ULDBUFF_RTR_TEST_2_RIGHT], st_motor_info[M_ULD_BUF_ROTATOR].d_allow);
				if(nRet_1 != BD_GOOD || nRet_2 != BD_GOOD)
				{
					if(nRet_1 != BD_GOOD ) m_strAlarmCode.Format(_T("%02d0006"), M_ULD_BUF_SHIFTER_Y); 
					else m_strAlarmCode.Format(_T("%02d0006"), M_ULD_BUF_ROTATOR); 
					CTL_Lib.Alarm_Error_Occurrence(2420, dWARNING, m_strAlarmCode);
					break;
				}

				nRet_1 = clsFunc.Check_Robot_Buffer_Clash(TESTSITE_RIGHT, THD_TEST_RBT, nWork_Site, m_nFirstTray_X_Num, m_nFirstPicker_Num, m_nClash_ErrorInfoStatus); //��Ŀ�� ���۵��� ���� �浹 Ȯ�� 
				if(nRet_1 == RET_ERROR)
				{
					CTL_Lib.Alarm_Error_Occurrence(2430, dWARNING, clsFunc.m_strAlarmCode);		
					break;
				}
			}
		}
		else  if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_4 ) //left site
		{
			nRet_1 = clsFunc.Check_Robot_Buffer_Clash(TESTSITE_LEFT, THD_TEST_RBT, nWork_Site, m_nFirstTray_X_Num, m_nFirstPicker_Num, m_nClash_ErrorInfoStatus); //��Ŀ�� ���۵��� ���� �浹 Ȯ�� 
			if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(2440, dWARNING, clsFunc.m_strAlarmCode);		
				break;
			}
		}
		else  if(nWork_Site >= THD_TESTSITE_5 && nWork_Site <= THD_TESTSITE_8 ) //right site
		{
			nRet_1 = clsFunc.Check_Robot_Buffer_Clash(TESTSITE_RIGHT, THD_TEST_RBT, nWork_Site, m_nFirstTray_X_Num, m_nFirstPicker_Num, m_nClash_ErrorInfoStatus); //��Ŀ�� ���۵��� ���� �浹 Ȯ�� 
			if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(2450, dWARNING, clsFunc.m_strAlarmCode);		
				break;
			}
		}//
		else if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF)
		{
			nRet_1 = clsFunc.Check_Robot_Buffer_Clash(nTest_Site, THD_TEST_RBT, nWork_Site, m_nFirstTray_X_Num, m_nFirstPicker_Num, m_nClash_ErrorInfoStatus); //��Ŀ�� ���۵��� ���� �浹 Ȯ�� 
			if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(2460, dWARNING, clsFunc.m_strAlarmCode);		
				break;
			}
			//2017.1206
			st_handler_info.nAutoDirectionUseChk = CTL_YES;
		}
		else 
		{
			break;
		}

		//if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8)
		//{
		//	m_nPlace_Step = 2010;
		//}
		//else
		//{
		//	m_nPlace_Step = 2100;
		//}
		//2017.1206
		if( (nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8 ) || (nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF) )
		{
			m_nPlace_Step = 2010;
		}
		else
		{
			m_nPlace_Step = 2100;
		}
		break;

	////////////////////////////////////////////////////////////////////////
	// ���� 1�� �� ���� üũ ��ƾ
	////////////////////////////////////////////////////////////////////////
	case 2010:
		m_dCurrentPos[2] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[2] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
		{
			m_nPlace_Step = 900;
			break;
		}
		m_dCurrentPos[m_nRobot_Ejector] = COMI.Get_MotCurrentPos(m_nRobot_Ejector);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[m_nRobot_Ejector] > st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
		{
			m_nPlace_Step = 900;
			break;
		}

	//	//2016.1117 direction
	//	if( st_basic_info.nDirectionCheckSkip == CTL_NO || st_handler_info.nAutoDirectionUseChk == CTL_YES )
	//	{
	//		//james 2016.0930 
	//		if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_4)
	//		{
	////				m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_DVC_DIRECTION_LEFTSITE_CHK];
	//			//2016.1201
	//			m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_DVC_DIRECTION_LEFTSITE_CHK] + m_dwDirOffset;
	//			m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_DVC_DIRECTION_LEFTSITE_CHK]; // 
	//		}
	//		else if(nWork_Site >= THD_TESTSITE_5 && nWork_Site <= THD_TESTSITE_8)
	//		{
	////				m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_DVC_DIRECTION_RIGHTSITE_CHK];
	//			//2016.1201
	//			m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_DVC_DIRECTION_RIGHTSITE_CHK] + m_dwDirOffset;
	//			m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_DVC_DIRECTION_RIGHTSITE_CHK]; // 
	//		}
	//		//m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_DVC_DIRECTION_CHK];
	//		//m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_DVC_DIRECTION_CHK]; // 
	//	
	//		nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate); 
	//		if (nRet_1 == BD_GOOD) //�������� �̵�
	//		{
	//			m_bDvcWaitChk_Falg =  false;
	//			m_nPlace_Step = 2020;

	//			//2016.1117
	//			//st_sync_info.nSmema_Tray_Input_Req = CTL_NO;
	//			st_handler_info.nAutoDirectionUseChk = CTL_NO;

	//		}
	//		else if (nRet_1 == BD_RETRY)
	//		{
	//			m_nPlace_Step = 2000;
	//		}
	//		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
	//		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
	//			CTL_Lib.Alarm_Error_Occurrence(2470, dWARNING, st_alarm_info.strCode);
	//			m_nPlace_Step = 2000;
	//		}
	//	}
	//	else
	//	{
	//		m_nPlace_Step = 2100;
	//	}
		//2017.1206  //Retest1�� TestSite1~4�� ���� Retest2�� TestSite5~8�� ���� üũ
		if( st_basic_info.nDirectionCheckSkip == CTL_NO || st_handler_info.nAutoDirectionUseChk == CTL_YES )
		{
// 			if( (nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_4) || nWork_Site == THD_RETEST_1_BUFF )
// 			{
// 				m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_DVC_DIRECTION_LEFTSITE_CHK] + m_dwDirOffset;
// 				m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_DVC_DIRECTION_LEFTSITE_CHK]; // 
// 			}
// 			else if( (nWork_Site >= THD_TESTSITE_5 && nWork_Site <= THD_TESTSITE_8) || nWork_Site == THD_RETEST_2_BUFF )
// 			{
// 				m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_DVC_DIRECTION_RIGHTSITE_CHK] + m_dwDirOffset;
// 				m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_DVC_DIRECTION_RIGHTSITE_CHK]; // 
// 			}
			//kwlee 20171207
			if( (nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_4) || nWork_Site == THD_RETEST_1_BUFF )
			{

				if(nTest_Site == TESTSITE_LEFT)
				{
					if ( m_nDirRetry_Cnt == 0)
					{
						m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_DVC_DIRECTION_LEFTSITE_CHK];
						m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_DVC_DIRECTION_LEFTSITE_CHK];
					}
					else if ( m_nDirRetry_Cnt == 1)
					{
						m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_DVC_DIRECTION_LEFTSITE_CHK_1];
						m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_DVC_DIRECTION_LEFTSITE_CHK_1];
					}
					else if ( m_nDirRetry_Cnt == 2)
					{
						m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_DVC_DIRECTION_LEFTSITE_CHK_2];
						m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_DVC_DIRECTION_LEFTSITE_CHK_2];
					}
				}
				else if (nTest_Site == TESTSITE_RIGHT)
				{
					if ( m_nDirRetry_Cnt == 0)
					{
						m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_DVC_DIRECTION_RIGHTSITE_CHK];
						m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_DVC_DIRECTION_RIGHTSITE_CHK]; // 
					}
					else if ( m_nDirRetry_Cnt == 1)
					{
						m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_DVC_DIRECTION_RIGHTSITE_CHK_1];
						m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_DVC_DIRECTION_RIGHTSITE_CHK_1]; // 
					}
					else if ( m_nDirRetry_Cnt == 2)
					{
						m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_DVC_DIRECTION_RIGHTSITE_CHK_2];
						m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_DVC_DIRECTION_RIGHTSITE_CHK_2]; // 
					}
				}
			
				 // 
			}
			else if( (nWork_Site >= THD_TESTSITE_5 && nWork_Site <= THD_TESTSITE_8) || nWork_Site == THD_RETEST_2_BUFF )
			{
				
				if(nTest_Site == TESTSITE_LEFT)
				{
					if ( m_nDirRetry_Cnt == 0)
					{
						m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_DVC_DIRECTION_LEFTSITE_CHK];
						m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_DVC_DIRECTION_LEFTSITE_CHK];
					}
					else if ( m_nDirRetry_Cnt == 1)
					{
						m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_DVC_DIRECTION_LEFTSITE_CHK_1];
						m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_DVC_DIRECTION_LEFTSITE_CHK_1];
					}
					else if ( m_nDirRetry_Cnt == 2)
					{
						m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_DVC_DIRECTION_LEFTSITE_CHK_2];
						m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_DVC_DIRECTION_LEFTSITE_CHK_2];
					}
				}
				else if (nTest_Site == TESTSITE_RIGHT)
				{
					if ( m_nDirRetry_Cnt == 0)
					{
						m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_DVC_DIRECTION_RIGHTSITE_CHK];
						m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_DVC_DIRECTION_RIGHTSITE_CHK]; // 
					}
					else if ( m_nDirRetry_Cnt == 1)
					{
						m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_DVC_DIRECTION_RIGHTSITE_CHK_1];
						m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_DVC_DIRECTION_RIGHTSITE_CHK_1]; // 
					}
					else if ( m_nDirRetry_Cnt == 2)
					{
						m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_DVC_DIRECTION_RIGHTSITE_CHK_2];
						m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_DVC_DIRECTION_RIGHTSITE_CHK_2]; // 
					}
				}
			}
			//
			nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate); 
			if (nRet_1 == BD_GOOD) //�������� �̵�
			{
				m_bDvcWaitChk_Falg =  false;
				m_nPlace_Step = 2020;
				st_handler_info.nAutoDirectionUseChk = CTL_NO;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nPlace_Step = 2000;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
				CTL_Lib.Alarm_Error_Occurrence(2470, dWARNING, st_alarm_info.strCode);
				m_nPlace_Step = 2000;
			}
		}
		else
		{
			m_nPlace_Step = 2100;
		}
		break; 	

	case 2020:
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_DVC_DIRECTION_CHK], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{
			m_bDvcWaitChk_Falg = false;
			m_nPlace_Step = 2030;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 2020;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(2480, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 2020;
		}
		break;

	case 2030:// ���� 1�� �� ���� üũ ��ƾ //james 2016.0827 
		if (st_basic_info.nModeDevice == WITHOUT_DVC)
		{
			m_nPlace_Step = 2040; //����
			break;
		}
		////kwlee 2016.0908 
		////if(st_test_site_info[nWork_Site].nSite_Pos == TESTSITE_LEFT)
		//if(nTest_Site == TESTSITE_LEFT) 
		//{
		//	nRet_1 = FAS_IO.get_in_bit(st_io_info.i_DvcDir1Chk,	IO_ON);	//left test site ��ġ ������	 
		//	nRet_2 = FAS_IO.get_in_bit(st_io_info.i_DvcDir2Chk,	IO_OFF);	
		//	if(nRet_1 == IO_ON && nRet_2 == IO_OFF)
		//	{
		//		m_nPlace_Step = 2040; //����
		//	}
		//	else
		//	{
		//		m_dwWaitUntil[0] = GetCurrentTime();
		//		m_nPlace_Step = 2031; //2016.1120
		//		break;
		//	}
		//}
		////kwlee 20160908
		////else if(st_test_site_info[nWork_Site].nSite_Pos == TESTSITE_RIGHT)
		//else if(nTest_Site == TESTSITE_RIGHT) 
		//{
		//	nRet_1 = FAS_IO.get_in_bit(st_io_info.i_DvcDir1Chk,	IO_OFF);	//fight test site ��ġ ������	 
		//	nRet_2 = FAS_IO.get_in_bit(st_io_info.i_DvcDir2Chk,	IO_ON);	
		//	//kwlee 20160908
		//	//if(nRet_1 == IO_ON && nRet_2 == IO_OFF)
		//	if( nRet_1 == IO_OFF && nRet_2 == IO_ON)
		//	{
		//		m_nPlace_Step = 2040; //����
		//	}
		//	else
		//	{
		//		m_dwWaitUntil[0] = GetCurrentTime();
		//		m_nPlace_Step = 2031; //2016.1120
		//		break;
		//	}
		//}
		//2017.1206
		if(nTest_Site == TESTSITE_LEFT || nWork_Site == THD_RETEST_1_BUFF )
		{
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_DvcDir1Chk,	IO_ON);
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_DvcDir2Chk,	IO_OFF);	
			if(nRet_1 == IO_ON && nRet_2 == IO_OFF)
			{
				m_nPlace_Step = 2040;
			}
			else
			{
				m_dwWaitUntil[0] = GetCurrentTime();
				m_nPlace_Step = 2031;
				break;
			}
		}
		else if(nTest_Site == TESTSITE_RIGHT || nWork_Site == THD_RETEST_2_BUFF )
		{
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_DvcDir1Chk,	IO_OFF);	 
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_DvcDir2Chk,	IO_ON);	
			if( nRet_1 == IO_OFF && nRet_2 == IO_ON)
			{
				m_nPlace_Step = 2040;
			}
			else
			{
				m_dwWaitUntil[0] = GetCurrentTime();
				m_nPlace_Step = 2031;
				break;
			}
		} 
		break; 

	case 2031://2016.1120
		if (st_basic_info.nModeDevice == WITHOUT_DVC)
		{
			m_nPlace_Step = 2040; //����
			break;
		}

		m_dwWaitUntil[1] = GetCurrentTime();
		m_dwWaitUntil[2] = m_dwWaitUntil[1] - m_dwWaitUntil[0];
		if( m_dwWaitUntil[2] <= 0 ) m_dwWaitUntil[0] = GetCurrentTime();
		if(m_dwWaitUntil[2] < (IO_STABLE_WAIT*5))  break; //500msec

		//if(nTest_Site == TESTSITE_LEFT) 
		//{
		//	nRet_1 = FAS_IO.get_in_bit(st_io_info.i_DvcDir1Chk,	IO_ON);	//left test site ��ġ ������	 
		//	nRet_2 = FAS_IO.get_in_bit(st_io_info.i_DvcDir2Chk,	IO_OFF);	
		//	if(nRet_1 == IO_ON && nRet_2 == IO_OFF)
		//	{
		//		m_nPlace_Step = 2040; //����
		//	}
		//	else
		//	{
		//		m_dwWaitUntil[0] = GetCurrentTime();
		//		if(nRet_1 == IO_OFF) m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_DvcDir1Chk); 
		//		else				 m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_DvcDir2Chk);  
		//		//2016.1201
		//		m_nPlace_Step = 2035;
		//		break;
		//	}
		//}
		//else if(nTest_Site == TESTSITE_RIGHT) 
		//{
		//	nRet_1 = FAS_IO.get_in_bit(st_io_info.i_DvcDir1Chk,	IO_OFF);	//fight test site ��ġ ������	 
		//	nRet_2 = FAS_IO.get_in_bit(st_io_info.i_DvcDir2Chk,	IO_ON);	
		//	if( nRet_1 == IO_OFF && nRet_2 == IO_ON)
		//	{
		//		m_nPlace_Step = 2040; //����
		//	}
		//	else
		//	{
		//		if(nRet_1 == IO_OFF) m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_DvcDir1Chk); 
		//		else				 m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_DvcDir2Chk);  
		//		//2016.1201
		//		m_nPlace_Step = 2035;
		//		break;
		//	}
		//}
		//2017.1206
		if(nTest_Site == TESTSITE_LEFT || nWork_Site == THD_RETEST_1_BUFF )
		{
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_DvcDir1Chk,	IO_ON);
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_DvcDir2Chk,	IO_OFF);	
			if(nRet_1 == IO_ON && nRet_2 == IO_OFF)
			{
				m_nPlace_Step = 2040; //����
			}
			else
			{
				m_dwWaitUntil[0] = GetCurrentTime();
				if(nRet_1 == IO_OFF) m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_DvcDir1Chk); 
				else				 m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_DvcDir2Chk);  
				m_nPlace_Step = 2035;
				break;
			}
		}
		else if(nTest_Site == TESTSITE_RIGHT || nWork_Site == THD_RETEST_2_BUFF )
		{
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_DvcDir1Chk,	IO_OFF);
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_DvcDir2Chk,	IO_ON);	
			if( nRet_1 == IO_OFF && nRet_2 == IO_ON)
			{
				m_nPlace_Step = 2040; 
			}
			else
			{
				if(nRet_1 == IO_OFF) m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_DvcDir1Chk); 
				else				 m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_DvcDir2Chk);  
				//2016.1201
				m_nPlace_Step = 2035;
				break;
			}
		} 
		break; 

		// �۾���ó�� 
	case 2035://2016.0910 
		m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY];
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, m_dpTargetPosList[2], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{ 
			//m_nPlace_Step = 2036;
			//2016.1201
			m_nDirRetry_Cnt++;
			if( m_nDirRetry_Cnt > 2 )
			{
				m_nDirRetry_Cnt = 0;
				m_dwDirOffset = 0.0;
				m_nPlace_Step = 2037;
			}
			else
			{
				//kwlee 20171207 del
// 				if( m_nDirRetry_Cnt == 1 ) m_dwDirOffset = -0.2;
// 				if( m_nDirRetry_Cnt == 2 ) m_dwDirOffset = 0.2;
				st_handler_info.nAutoDirectionUseChk = CTL_YES;
				m_nPlace_Step = 2036;
			}
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 2035;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(2490, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 2035;
		}		
		break; 

	case 2036://��Ʈ�� ������ �ִ� ��� �ڷ� �����ٰ� �ٽ� üũ �Ѵ�. 5mm
			//if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_4)
			//{
			//	m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_DVC_DIRECTION_LEFTSITE_CHK] + 10.0;
			//	m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_DVC_DIRECTION_LEFTSITE_CHK]; // 
			//}
			//else if(nWork_Site >= THD_TESTSITE_5 && nWork_Site <= THD_TESTSITE_8)
			//{
			//	m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_DVC_DIRECTION_RIGHTSITE_CHK] + 10.0;
			//	m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_DVC_DIRECTION_RIGHTSITE_CHK]; // 
			//}
			//20171206
			if( (nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_4) || nWork_Site == THD_RETEST_1_BUFF)
			{
				m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_DVC_DIRECTION_LEFTSITE_CHK] + 10.0;
				m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_DVC_DIRECTION_LEFTSITE_CHK]; // 
			}
			else if( (nWork_Site >= THD_TESTSITE_5 && nWork_Site <= THD_TESTSITE_8) || nWork_Site == THD_RETEST_2_BUFF)
			{
				m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_DVC_DIRECTION_RIGHTSITE_CHK] + 10.0;
				m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_DVC_DIRECTION_RIGHTSITE_CHK]; // 
			}

			nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate); 
			if (nRet_1 == BD_GOOD) //�������� �̵�
			{
				m_nPlace_Step = 2010;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nPlace_Step = 2036;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
				CTL_Lib.Alarm_Error_Occurrence(2471, dWARNING, st_alarm_info.strCode);
				m_nPlace_Step = 2036;
			}
			break;

	case 2037:
// 		m_dpTargetPosList[0] = st_motor_info[M_TEST_RBT_Y].d_pos[P_TESTRBT_XY_RETESTBUF_2_START];
// 		m_dpTargetPosList[1] = st_motor_info[M_TEST_RBT_X].d_pos[P_TESTRBT_XY_RETESTBUF_2_START];
		//kwlee 2017.0918
		m_dpTargetPosList[0] = st_motor_info[M_TEST_RBT_Y].d_pos[P_TESTRBT_XY_TESTDUCT_1_LEFT_START];
		m_dpTargetPosList[1] = st_motor_info[M_TEST_RBT_X].d_pos[P_TESTRBT_XY_TESTDUCT_1_LEFT_START];

		nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate); 
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{
			m_bDvcWaitChk_Falg =  false;
			//m_nPlace_Step = 2037;
			//2016.1201
			m_nPlace_Step = 2038;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 2037;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(2500, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 2037;
		}
		break; 	

	case 2038:
		//kwlee 2016.1124
		//st_sync_info.nSmema_Tray_Input_Req = CTL_YES;
		st_handler_info.nAutoDirectionUseChk = CTL_YES;
		CTL_Lib.Alarm_Error_Occurrence(2510, dWARNING, m_strAlarmCode);
		//2016.1201
		m_dwDirOffset = 0.0;
		m_nPlace_Step = 2010;
		break;

		////

	case 2040:
		m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY];
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, m_dpTargetPosList[2], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{ 			
			m_nPlace_Step = 2100;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 2040;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(2520, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 2040;
		}		
		break; 	

	case 2100:
		m_dCurrentPos[2] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[2] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
		{
			m_nPlace_Step = 900;
			break;
		}
		m_dCurrentPos[m_nRobot_Ejector] = COMI.Get_MotCurrentPos(m_nRobot_Ejector);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[m_nRobot_Ejector] > st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
		{
			m_nPlace_Step = 900;
			break;
		}
		
		// jtkim 20160904
		// nRet_1 = clsFunc.Calculate_MovePos_Find(0, m_nRobot_Y, nWork_Site, 0, m_npFindWorkPosYXCPB, &m_dTemp_CalTargetPos_Y);             //Calculate_MovePos_Find(int nMotNum, int nSite, int nWorkPart, int *npFirstYXP_Pos, double *dpGetTargetPos)
		// nRet_2 = clsFunc.Calculate_MovePos_Find(0, m_nRobot_X, nWork_Site, 0, m_npFindWorkPosYXCPB, &m_dTemp_CalTargetPos_X);

		if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_4 ) //left site
		{
			nRet_1 = clsFunc.Calculate_MovePos_Find(0, m_nRobot_Y, nWork_Site, nStart_FixPos, m_npFindWorkPosYXCPB, &m_dTemp_CalTargetPos_Y);             //Calculate_MovePos_Find(int nMotNum, int nSite, int nWorkPart, int *npFirstYXP_Pos, double *dpGetTargetPos)
			nRet_2 = clsFunc.Calculate_MovePos_Find(0, m_nRobot_X, nWork_Site, nStart_FixPos, m_npFindWorkPosYXCPB, &m_dTemp_CalTargetPos_X);
		}
		else  if(nWork_Site >= THD_TESTSITE_5 && nWork_Site <= THD_TESTSITE_8 ) //right site
		{
			nRet_1 = clsFunc.Calculate_MovePos_Find(0, m_nRobot_Y, nWork_Site, nStart_FixPos, m_npFindWorkPosYXCPB, &m_dTemp_CalTargetPos_Y);             //Calculate_MovePos_Find(int nMotNum, int nSite, int nWorkPart, int *npFirstYXP_Pos, double *dpGetTargetPos)
			nRet_2 = clsFunc.Calculate_MovePos_Find(0, m_nRobot_X, nWork_Site, nStart_FixPos, m_npFindWorkPosYXCPB, &m_dTemp_CalTargetPos_X);

		}//
		else  if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF ) //right site
		{
			nRet_1 = clsFunc.Calculate_MovePos_Find(0, m_nRobot_Y, nWork_Site, nTest_Site, m_npFindWorkPosYXCPB, &m_dTemp_CalTargetPos_Y);             //Calculate_MovePos_Find(int nMotNum, int nSite, int nWorkPart, int *npFirstYXP_Pos, double *dpGetTargetPos)
			nRet_2 = clsFunc.Calculate_MovePos_Find(0, m_nRobot_X, nWork_Site, nTest_Site, m_npFindWorkPosYXCPB, &m_dTemp_CalTargetPos_X);
		}
		else if(nWork_Site == THD_LD_BUFF || nWork_Site == THD_ULD_BUFF ) //james 2016.0922 ���� �߰� 
		{
			nRet_1 = clsFunc.Calculate_MovePos_Find(0, m_nRobot_Y, nWork_Site, 0, m_npFindWorkPosYXCPB, &m_dTemp_CalTargetPos_Y);             //Calculate_MovePos_Find(int nMotNum, int nSite, int nWorkPart, int *npFirstYXP_Pos, double *dpGetTargetPos)
			nRet_2 = clsFunc.Calculate_MovePos_Find(0, m_nRobot_X, nWork_Site, 0, m_npFindWorkPosYXCPB, &m_dTemp_CalTargetPos_X);
		} 
		else 
		{ //james 2016.0922 
			break;
		}
		
		if(nRet_1 == RET_GOOD && nRet_2 == RET_GOOD)
		{//ok
			m_dpTargetPosList[0] = m_dTemp_CalTargetPos_Y;
			m_dpTargetPosList[1] = m_dTemp_CalTargetPos_X;
		}
		else
		{
			CTL_Lib.Alarm_Error_Occurrence(2530, dWARNING, clsFunc.m_strAlarmCode);
			break;
		}		 

		
		nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate); 
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{
			m_bDvcWaitChk_Falg =  false;
			m_nPlace_Step = 2200;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 2100;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(2540, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 2100;
		}
		break; 	

	case 2200:
		if(nWork_Site == THD_LD_BUFF || nWork_Site == THD_ULD_BUFF)
		{
			//�ε� ���ۿ� ��û�Ͽ� ���簡 �غ�Ǿ� CTL_READY �� ����, ���縦 ���� �� �ִ� 
			 if(st_sync_info.nTestRbt_Dvc_Req[nWork_Site][0] == CTL_READY && st_sync_info.nTestRbt_Dvc_Req[nWork_Site][1] == WORK_PLACE) //�ε� ���۰� �غ�Ǳ⸦ ��ٸ��� 
			 {				 ////james 2016.1005 = CTL_READY
				 m_nPlace_Step = 3000;
			 }		
		}
		//else  if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF || (nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8))
		else  if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF)
		{//���� ���� ���� 
			if(st_sync_info.nTestRbt_Dvc_Req[nWork_Site][0] == CTL_READY && st_sync_info.nTestRbt_Dvc_Req[nWork_Site][1] == WORK_PLACE) //����/test site�� �غ�Ǳ⸦ ��ٸ��� 
			{				 
				 m_nPlace_Step = 3000;
			}	
		}	
		else if ((nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8))
		{
			if(st_sync_info.nTestRbt_Dvc_Ld_Req[nWork_Site][0] == CTL_READY && st_sync_info.nTestRbt_Dvc_Ld_Req[nWork_Site][1] == WORK_PLACE) //����/test site�� �غ�Ǳ⸦ ��ٸ��� 
			{				 
				 m_nPlace_Step = 3000;
			}
		}
		break;

	case 3000:  
		m_dCurrentPos[m_nRobot_Ejector] = COMI.Get_MotCurrentPos(m_nRobot_Ejector);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[m_nRobot_Ejector] > st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
		{
			m_nPlace_Step = 900;
			break;
		}

		nRet_1 = COMI.Check_MotPosRange(m_nRobot_Y, m_dpTargetPosList[0], st_motor_info[m_nRobot_Y].d_allow);
		nRet_2 = COMI.Check_MotPosRange(m_nRobot_X, m_dpTargetPosList[1], st_motor_info[m_nRobot_X].d_allow);
		if(nRet_1 != BD_GOOD || nRet_2 != BD_GOOD)
		{
			m_nPlace_Step = 2000;
			break;
		}

		//Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON); //�̸� ������  

		if(nWork_Site == THD_LD_BUFF)
		{
			m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_LDBUF_PLACE];
		}
		else if(nWork_Site == THD_ULD_BUFF)
		{//���� ���� ���� 
			m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_ULDBUF_PLACE];
		}
		else if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_4)
		{
			//james 2016.0918 
			if(nWork_Site == THD_TESTSITE_1)
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Chamber1OpenChk,	IO_ON);
				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_Chamber1CloseChk,	IO_OFF);
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_1_LEFT_PLACE]; //kwlee 2016.1117
				if(nRet_1 != IO_ON || nRet_2 != IO_OFF) 
				{ 
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Chamber1OpenChk); 
					CTL_Lib.Alarm_Error_Occurrence(2560, dWARNING, m_strAlarmCode);	
					break;
				}
			}
			else if(nWork_Site == THD_TESTSITE_2)
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Chamber2OpenChk,	IO_ON);
				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_Chamber2CloseChk,	IO_OFF);
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_2_LEFT_PLACE]; //kwlee 2016.1117
				if(nRet_1 != IO_ON || nRet_2 != IO_OFF) 
				{ 
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Chamber2OpenChk); 
					CTL_Lib.Alarm_Error_Occurrence(2570, dWARNING, m_strAlarmCode);	
					break;
				}
			}
			else if(nWork_Site == THD_TESTSITE_3)
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Chamber3OpenChk,	IO_ON);
				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_Chamber3CloseChk,	IO_OFF);
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_3_LEFT_PLACE]; //kwlee 2016.1117
				if(nRet_1 != IO_ON || nRet_2 != IO_OFF) 
				{ 
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Chamber3OpenChk); 
					CTL_Lib.Alarm_Error_Occurrence(2580, dWARNING, m_strAlarmCode);	
					break;
				}
			}
			else if(nWork_Site == THD_TESTSITE_4)
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Chamber4OpenChk,	IO_ON);
				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_Chamber4CloseChk,	IO_OFF);
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_4_LEFT_PLACE]; //kwlee 2016.1117
				if(nRet_1 != IO_ON || nRet_2 != IO_OFF) 
				{ 
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Chamber4OpenChk); 
					CTL_Lib.Alarm_Error_Occurrence(2590, dWARNING, m_strAlarmCode);	
					break;
				}
			}
			//m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_LEFT_PLACE];
		}
		else if(nWork_Site >= THD_TESTSITE_5 && nWork_Site <= THD_TESTSITE_8)
		{
			//james 2016.0918 
			if(nWork_Site == THD_TESTSITE_5)
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Chamber5OpenChk,	IO_ON);
				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_Chamber5CloseChk,	IO_OFF);
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_5_RIGHT_PLACE]; //kwlee 2016.1117
				if(nRet_1 != IO_ON || nRet_2 != IO_OFF) 
				{ 
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Chamber5OpenChk); 
					CTL_Lib.Alarm_Error_Occurrence(2600, dWARNING, m_strAlarmCode);	
					break;
				}
			}
			else if(nWork_Site == THD_TESTSITE_6)
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Chamber6OpenChk,	IO_ON);
				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_Chamber6CloseChk,	IO_OFF);
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_6_RIGHT_PLACE]; //kwlee 2016.1117
				if(nRet_1 != IO_ON || nRet_2 != IO_OFF) 
				{ 
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Chamber6OpenChk); 
					CTL_Lib.Alarm_Error_Occurrence(2610, dWARNING, m_strAlarmCode);	
					break;
				}
			}
			else if(nWork_Site == THD_TESTSITE_7)
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Chamber7OpenChk,	IO_ON);
				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_Chamber7CloseChk,	IO_OFF);
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_7_RIGHT_PLACE]; //kwlee 2016.1117
				if(nRet_1 != IO_ON || nRet_2 != IO_OFF) 
				{ 
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Chamber7OpenChk); 
					CTL_Lib.Alarm_Error_Occurrence(2620, dWARNING, m_strAlarmCode);	
					break;
				}
			}
			else if(nWork_Site == THD_TESTSITE_8)
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Chamber8OpenChk,	IO_ON);
				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_Chamber8CloseChk,	IO_OFF);
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_8_RIGHT_PLACE]; //kwlee 2016.1117
				if(nRet_1 != IO_ON || nRet_2 != IO_OFF) 
				{ 
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_Chamber8OpenChk); 
					CTL_Lib.Alarm_Error_Occurrence(2620, dWARNING, m_strAlarmCode);	
					break;
				}
			}

			//m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_RIGHT_PLACE];

		}
		else if(nWork_Site == THD_RETEST_1_BUFF)
		{
			m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_RETESTBUF_1_PLACE];
		}
		else if(nWork_Site == THD_RETEST_2_BUFF)
		{
			m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_RETESTBUF_2_PLACE];
		}
		else
		{//
			break;
		} 

		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, m_dpTargetPosList[2], (int) (COMI.mn_runspeed_rate / 3)); //�ӵ� �ٿ� 
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{ 			
			m_nPlace_Step = 3100;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 3000;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(2630, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 3000;
		}		
		break;	

	case 3100: //���縦 ���´� 
		if(nWork_Site == THD_LD_BUFF || nWork_Site == THD_ULD_BUFF)
		{
			 m_nPlace_Step = 3110;
		}
		else  //if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF || nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8)
		{
			m_nPlace_Step = 3200;
		}
		break;

	/////////////////////////////////////////////////////////////////////////
	// load/unload buffer���� �ε� ���縦 �������� ������ �׸��� ����� �κ����� �Ѵ� 
	// �׿� ����Ʈ�� �׸��۰� ��� �׷� �ʿ� ���� 
	/////////////////////////////////////////////////////////////////////////
	case 3110:
		if(nWork_Site == THD_LD_BUFF || nWork_Site == THD_ULD_BUFF)
		{
			for(i = 0; i < m_nPickerPara; i++)  //4
			{
				if(nTest_Site == TESTSITE_LEFT) //left site�� �۾��ϱ� ���� �۾� ���϶�
				{
					m_npBuffer_YesNo[i] =  m_npPicker_YesNo[(m_nPickerPara - 1) - i]; //3,2,1,0 ���� �ݴ�� �۾���ġ ���� 
				}
				else if(nTest_Site == TESTSITE_RIGHT) //right sitet�� �۾��ϱ� ���� �۾� ���϶�
				{
					m_npBuffer_YesNo[i] = m_npPicker_YesNo[i];
				}
			}

			 if(st_sync_info.nTestRbt_Dvc_Req[nWork_Site][0] == CTL_READY && st_sync_info.nTestRbt_Dvc_Req[nWork_Site][1] == WORK_PLACE) //�ε� ���۰� �غ�Ǿ� �ִ� 
			 {				 //james 2016.1005 = CTL_READY
				 m_nPlace_Step = 3120;
			 }		
		}
		else  //if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF || nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8)
		{//���� ���� ���� 
			 m_nPlace_Step = 3200;
		}		
		break;

	/////////////////////////////////////////////////////////////////////////
	// load/unload buffer���� �ε� ���縦 �������� ������ �׸��� ����� �κ����� �Ѵ� 
	// load buffer �� �׸��� �� �κ�Ʈ�� ���� ���� ��ġ�� �׸��۸� open�Ͽ� �κ��� ���� �� �� �ְ� �Ѵ�
	/////////////////////////////////////////////////////////////////////////
	case 3120:
		if(nWork_Site == THD_LD_BUFF)
		{
			clsRunDvcLdBuffer.Set_Gripper_OnOff(0, IO_OFF, m_npBuffer_YesNo); //�ε� ������ �׸��۸� open�Ͽ� �κ��� ���縦 ���� �� �� �ְ� �Ѵ� 
		}
		else if(nWork_Site == THD_ULD_BUFF)
		{
			clsRunDvcUldBuffer.Set_Gripper_OnOff(0, IO_OFF, m_npBuffer_YesNo); //�ε� ������ �׸��۸� open�Ͽ� �κ��� ���縦 ���� �� �� �ְ� �Ѵ� 
		}
		m_nPlace_Step = 3130;
		break;

	case 3130:
		if(nWork_Site == THD_LD_BUFF)
		{
			nRet_1 = clsRunDvcLdBuffer.Chk_Gripper_OnOff(0, IO_OFF, m_npBuffer_YesNo);
		}
		else if(nWork_Site == THD_ULD_BUFF)
		{
			nRet_1 = clsRunDvcUldBuffer.Chk_Gripper_OnOff(0, IO_OFF, m_npBuffer_YesNo);
		}

		if(nRet_1 == RET_GOOD)
		{
			m_bDvcWaitChk_Falg = false;
			m_nPlace_Step = 3140;
		}
		else if(nRet_1 == RET_ERROR)
		{			 
			CTL_Lib.Alarm_Error_Occurrence(2640, dWARNING, m_strAlarmCode);
			m_nPlace_Step = 3120;
		}
		break;

	case 3140:		//james 2016.0922 �߰� 
		nRet_1 = clsFunc.Check_BufferStatus(1, nWork_Site, CTL_YES, m_npBuffer_YesNo, m_npBuff_Status, m_npBuff_OutputStatus); //���������� ���� ���°� ��ġ�ؾ� �Ѵ� 
		if(nRet_1 == RET_GOOD)
		{//���� ����, //Enable �Ǿ� �ִ� ���� ��ġ�� ��� ä���� ����, �׽�Ʈ �κ� ����Ʈ�� �̼۰����ϴ� 
			m_bDvcWaitChk_Falg = false;
			m_nPlace_Step = 3200;
		}
		else if(nRet_1== RET_ERROR)
		{//���� ���� ä��� �۾��� ������ ���� ���� 
			CTL_Lib.Alarm_Error_Occurrence(2650, dWARNING, clsFunc.m_strAlarmCode);
			m_nPlace_Step = 3140;
		} 
		break;

		//james 2016.0922 
		//m_bDvcWaitChk_Falg = false;
		//m_nPlace_Step = 3200;
		//break;

	case 3200:
		nRet_1 = clsFunc.Check_PickerStatus(1, THD_TEST_RBT, CTL_YES, m_npPicker_YesNo, m_npPicker_Vacuum_Status, m_npPicker_OutputStatus);
		if(nRet_1 == RET_GOOD) //���� ������ ���� ��Ŀ ���¸� üũ 
		{//��Ŀ ������ ���� ���°� �����̴�
			m_nPlace_Step = 3300;
		}
		else// if(nRet_1 == RET_ERROR)
		{
			if(m_bDvcWaitChk_Falg == false)
			{
				m_bDvcWaitChk_Falg =  true;
				m_dwWaitUntil[0] = GetCurrentTime();
				break;
			}
			else
			{
				m_dwWaitUntil[1] = GetCurrentTime();
				m_dwWaitUntil[2] = m_dwWaitUntil[1] - m_dwWaitUntil[0];
				if(m_dwWaitUntil[2] < IO_STABLE_WAIT) break;
				CTL_Lib.Alarm_Error_Occurrence(2560, dWARNING, clsFunc.m_strAlarmCode);
				m_bDvcWaitChk_Falg = false;
				break;
			}
		}
		break;

	case 3300:
		m_nPlace_Step = 4000;
		break;

	case 4000:
		Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON);  //���縦 ���´� 
		m_nPlace_Step = 4010;
		break;

	case 4010:
		nRet_1 = Chk_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON);
		if(nRet_1 == RET_GOOD)
		{
			m_bDvcWaitChk_Falg = false;
			m_nPlace_Step = 4100;
		}
		else if(nRet_1 == RET_ERROR)
		{
			Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON);  //������ ������ 
			CTL_Lib.Alarm_Error_Occurrence(2570, dWARNING, m_strAlarmCode);
			m_nPlace_Step = 4000;
		}
		break;

	case 4100:
// 		if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8)
// 		{
// 			m_nPlace_Step = 4110;
// 			//2017.1119 MISS LOADING CHECK
// 			m_nPlace_Step = 4106;
// 		}
// 		else
// 		{
// 			m_nPlace_Step = 4200;
// 
// 			//2016.1117
// 			//kwlee 2017.0918 ���� �ļ� ���� del
// // 			if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF)
// // 			{
// // 				m_nPlace_Step = 4102;
// // 			}
// // 			else
// // 			{
// // 				m_nPlace_Step = 4200;
// // 			}
// 		}
// 		//2017.1119 
// 		if( (nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8) || ( nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF ) )
// 		{
// 			
// // 			m_nPlace_Step = 4110;
// // 			//2017.1119 MISS LOADING CHECK
// // 			m_nPlace_Step = 4106;
// 			//kwlee 20171119
// 			if (st_basic_info.nMissLoading_Mode == CTL_NO)
// 			{
// 				m_nPlace_Step = 4110;
// 			}
// 			else
// 			{
// 				//2017.1119 MISS LOADING CHECK
// 				m_nPlace_Step = 4106;
// 			}
// 			
// 			
// 		}
// 		else
// 		{
// 			m_nPlace_Step = 4200;
// 		}
// 		break;
		//2017.1120
		if (st_basic_info.nMissLoading_Mode == CTL_NO)
		{
			if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8)
			{
				m_nPlace_Step = 4110;
			}
			else
			{
				m_nPlace_Step = 4200;
			}
		}
		else
		{
			if( (nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8) || ( nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF ) )
			{
				m_nPlace_Step = 4106;
			}
			else
			{
				m_nPlace_Step = 4200;
			}
		}
		break;
		//2017.0918 Retest Buffer Place �� Ȯ�� ���� ����.

	case 4102:
		if(nWork_Site == THD_RETEST_1_BUFF )// || nWork_Site == THD_RETEST_2_BUFF)
		{
			m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_RETESTBUF_1_PICK];
			
		}
		else if(nWork_Site == THD_RETEST_2_BUFF)
		{
			m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_RETESTBUF_2_PICK];
		}
		else
		{
			break;
		}

		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, m_dpTargetPosList[2], (int) (COMI.mn_runspeed_rate / 3)); //�ӵ� �ٿ� 
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{
			m_nPlace_Step = 4105;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 4102;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(2791, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 4102;
		}
		break;

		//Place �� ���� Ȯ��.
	case 4105:
		for (i = 0; i < 4; i++)
		{
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_TestRbtPickerGripDvcExistChk[i], IO_ON);
			if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_YES)
			{
				if(nRet_1 != IO_ON)
				{
					//m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_TestRbtPickerGripDvcExistChk[i]);
					//CTL_Lib.Alarm_Error_Occurrence(2904, dWARNING, m_strAlarmCode);	
					//break;
					//2016.1128
					if( Opt == 1 )
					{
						if( st_picker[m_nRobotSite].st_pcb_info[i].nBin == BD_DATA_RETEST )
						{
							m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_TestRbtPickerGripDvcExistChk[i]);
							CTL_Lib.Alarm_Error_Occurrence(2905, dWARNING, m_strAlarmCode);
							break;
						}
					}
					else
					{
						m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_TestRbtPickerGripDvcExistChk[i]);
						CTL_Lib.Alarm_Error_Occurrence(2904, dWARNING, m_strAlarmCode);	
						break;
					}
				}

			}

		}
		m_nPlace_Step = 4200;
		break;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//2017.1119 MISS LOADING CHECK
		//Missloading ƼĪ�� ���̷� ����� üũ
	case 4106:

		if (st_basic_info.nMissLoading_Mode == CTL_YES)
		{
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_MissLoadingChk1, IO_ON);
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_MissLoadingChk2, IO_ON);
			if(nRet_1 == IO_ON || nRet_2 == IO_ON) 
			{
				m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_MissLoadingChk1); //Miss loading ���� ����
				CTL_Lib.Alarm_Error_Occurrence(2889, dWARNING, m_strAlarmCode);
				COMI.Set_MotStop(1, m_nRobot_Z);
				break;
			}
		}


		if( ( nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF ) )
		{
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_RETEST_MISSLOAD_CHECK], COMI.mn_runspeed_rate);
		}
		else
		{
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_MISSLOAD_CHECK], COMI.mn_runspeed_rate);
		}
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{
			m_dwWaitUntil[0] = GetCurrentTime();
			m_nPlace_Step = 4107;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 4106;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(2730, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 4106;
		}		
		break;

	case 4107:
		nRet_1 = FAS_IO.get_in_bit(st_io_info.i_MissLoadingChk1, IO_ON);
		nRet_2 = FAS_IO.get_in_bit(st_io_info.i_MissLoadingChk2, IO_ON);

		//if(nRet_1 == IO_ON && nRet_2 == IO_ON)
		//kwlee 20171119 ���� �� �˶�.
		if(nRet_1 == IO_OFF && nRet_2 == IO_OFF) 
		{
			if( (nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8) )
			{
				m_nPlace_Step = 4110;
			}
			else// || ( nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF )
			{
				m_nPlace_Step = 4200;
			}
		}
		else
		{
			m_dwWaitUntil[1] = GetCurrentTime();
			m_dwWaitUntil[2] = m_dwWaitUntil[1] - m_dwWaitUntil[0];
			if(m_dwWaitUntil[2] < IO_STABLE_WAIT) break;
			m_nPlace_Step = 4109;
		}
		break;


	case 4109:
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{
			m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_MissLoadingChk1); //Miss loading ���� ����
			CTL_Lib.Alarm_Error_Occurrence(2733, dWARNING, m_strAlarmCode);
			//m_nPlace_Step = 4106;
			//kwlee 20171119
			m_nPlace_Step = 4100;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 4109;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(2731, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 4109;
		}		
		break;
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	case 4110: //test site ���� INSERT 
		if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_4)
		{
			//kwlee 2016.1117
			if(nWork_Site == THD_TESTSITE_1)
			{
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_1_LEFT_INSERT];
			}
			else if(nWork_Site == THD_TESTSITE_2)
			{
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_2_LEFT_INSERT];
			}
			else if(nWork_Site == THD_TESTSITE_3)
			{
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_3_LEFT_INSERT];
			}
			else if(nWork_Site == THD_TESTSITE_4)
			{
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_4_LEFT_INSERT];
			}
			//m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_LEFT_INSERT]; 
		}
		else if(nWork_Site >= THD_TESTSITE_4 && nWork_Site <= THD_TESTSITE_8)
		{
			//kwlee 2016.1117
			if(nWork_Site == THD_TESTSITE_5)
			{
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_5_RIGHT_INSERT];
			}
			else if(nWork_Site == THD_TESTSITE_6)
			{
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_6_RIGHT_INSERT];
			}
			else if(nWork_Site == THD_TESTSITE_7)
			{
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_7_RIGHT_INSERT];
			}
			else if(nWork_Site == THD_TESTSITE_8)
			{
				m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_8_RIGHT_INSERT];
			}
			////
			//m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_TESTSITE_RIGHT_INSERT]; 
		} 
		else
		{
			break;
		}

		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, m_dpTargetPosList[2], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{ 			
			m_nPlace_Step = 4200;
			m_nPlace_Step = 4120;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 4110;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(2580, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 4110;
		}		
		break;

	case 4120:
		for (i = 0; i < 4; i++)
		{
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_TestRbtPickerGripDvcExistChk[i], IO_ON);
			if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_YES)
			{
				if(nRet_1 != IO_ON)
				{
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_TestRbtPickerGripDvcExistChk[i]);
					CTL_Lib.Alarm_Error_Occurrence(2904, dWARNING, m_strAlarmCode);	
					break;
				}
			}
		}
		m_nPlace_Step = 4200;
		break;

	case 4200:
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{
			m_bDvcWaitChk_Falg = false;
			m_nPlace_Step = 5000;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 4200;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(2590, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 4200;
		}
		break;

	case 5000:
		//nRet_1 = clsFunc.Check_PickerStatus(1, THD_TEST_RBT, CTL_NO, m_npPicker_YesNo, m_npPicker_Vacuum_Status, m_npPicker_OutputStatus);
		//2016.1129
		nRet_1 = clsFunc.Check_PickerStatus(1, THD_TEST_RBT, CTL_NO, m_npPicker_YesNo, m_npPicker_Vacuum_Status, m_npPicker_OutputStatus);
		if (nRet_1 != RET_GOOD) //
		{
			if(m_bDvcWaitChk_Falg == false)
			{
				m_bDvcWaitChk_Falg =  true;
				m_dwWaitUntil[0] = GetCurrentTime();
				break;
			}
			else
			{
				m_dwWaitUntil[1] = GetCurrentTime();
				m_dwWaitUntil[2] = m_dwWaitUntil[1] - m_dwWaitUntil[0];
				if(m_dwWaitUntil[2] < IO_STABLE_WAIT) break;
				CTL_Lib.Alarm_Error_Occurrence(2698, dWARNING, clsFunc.m_strAlarmCode);//2017.0313
				break;
			}
		}


		for (i = m_nFirstPicker_Num; i < m_nPickerPara; i++)
		{
			if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_YES && m_npPicker_YesNo[i] == CTL_YES
				&& (m_npPicker_Vacuum_Status[i] == IO_OFF && m_npPicker_OutputStatus[i] == IO_ON) )//james 2016.1003 CTL_YES)
			{
				if(nWork_Site == THD_LD_BUFF || nWork_Site == THD_ULD_BUFF)
				{
					if(i >= 0 && m_npFindWorkPosYXCPB[1] + (i - m_nFirstPicker_Num) < st_recipe_info.nLdUldBuffer_Num)
					{
						m_npPicker_YesNo[i] = CTL_NO; //�۾��� ��Ŀ ���� Ŭ����

						if(nTest_Site == TESTSITE_LEFT) //left site�� �۾��ϱ� ���� �۾� ���϶�
						{
							nFind_JobPos = (st_recipe_info.nLdUldBuffer_Num - 1) - i ; //LEFT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ���������� 3,2,1,0 �� �ݴ��̴� 
								

							//�Ʒ� �Լ����� ������� ��ġ ������ �����Ѵ�
							m_npSet_WorkPosYXCPB[0]  = nFind_JobPos; //����sms Y�� ��� //������ ��ġ ����
							m_npSet_WorkPosYXCPB[1]  = nFind_JobPos; //qjvjsms X�� ������� ���� //������ ��ġ ����
							m_npSet_WorkPosYXCPB[3]  = i; //picker ��ġ ����

							clsFunc.Data_Exchange_PickPlace(PICKER_PLACE_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //��Ŀ �� Ʈ���� ���� ���� �Լ�
							//�� �Լ����� ��� ������ ���� �� Ŭ���� �Ѵ�
							//st_picker[m_nRobotSite].st_pcb_info[y].nYesNo = CTL_YES;					//��Ŀ ���� ��
							//st_tray_info[nWork_Site].st_pcb_info[m_npFirstPosYXP[0] + (y - m_n_FirstPicker_Num)][m_npFirstPosYXP[1]].nYesNo = CTL_NO; //Ʈ���� ���� Ŭ����
							//st_count_info.nSSD_Input[0]++;
						}
						else if(nTest_Site == TESTSITE_RIGHT) //RIGHT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ����������0,1,2,3 �� ��ġ ������ �����ϴ� 
						{
							nFind_JobPos = i ; //RIGHT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ����������0,1,2,3 �� ��ġ ������ �����ϴ�  

							//�Ʒ� �Լ����� ������� ��ġ ������ �����Ѵ�
							m_npSet_WorkPosYXCPB[0]  = nFind_JobPos; //����sms Y�� ��� //������ ��ġ ����
							m_npSet_WorkPosYXCPB[1]  = nFind_JobPos; //qjvjsms X�� ������� ���� //������ ��ġ ����
							m_npSet_WorkPosYXCPB[3]  = i; //picker ��ġ ����

							clsFunc.Data_Exchange_PickPlace(PICKER_PLACE_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //��Ŀ �� Ʈ���� ���� ���� �Լ� 
						}
						//kwlee 2017.0905
						if (st_basic_info.nModeXgem == YES)
						{
							if (st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == YES)
							{
								clsXgem.OnMcProductSocketToBuffer(END,st_buffer_info[m_nRobotSite].st_pcb_info[i].nTrayCnt,st_buffer_info[m_nRobotSite].st_pcb_info[i].strSerialNo,nFind_JobPos,nWork_Site);
							}
						}
						////////
					}
				} 
				else if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8)
				{
					if(i >= 0 && m_npFindWorkPosYXCPB[1] + (i - m_nFirstPicker_Num) < st_recipe_info.nTestSocket_Num)
					{
						m_npPicker_YesNo[i] = CTL_NO; //�۾��� ��Ŀ ���� Ŭ����

						///////////////////////////////////////////////////////////////////////////////////////////////
						//2016.1203
						if (m_strLotNo[1] == _T(""))
						{
							m_strLotNo[1]	= st_picker[THD_TEST_RBT].st_pcb_info[i].strLotNo;
							m_strPartNo[1]	= st_picker[THD_TEST_RBT].st_pcb_info[i].strPartNo;
						}
						///////////////////////////////////////////////////////////////////////////////////////////////

						if(nTest_Site == TESTSITE_LEFT) //left site�� �۾��ϱ� ���� �۾� ���϶�
						{
							nFind_JobPos = (st_recipe_info.nTestSocket_Num - 1) - i ; //LEFT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ���������� 3,2,1,0 �� �ݴ��̴� 
							nFind_JobPos = nFind_JobPos - nStart_FixPos; //st_test_site_info[nWork_Site].nStart_FixPos;								

							//�Ʒ� �Լ����� ������� ��ġ ������ �����Ѵ�
							m_npSet_WorkPosYXCPB[0]  = nFind_JobPos; //Test Site�� Y�� ������ ���� //������ ��ġ ����
							m_npSet_WorkPosYXCPB[1]  = nFind_JobPos; //Test Site�� X�� ��� //������ ��ġ ����
							m_npSet_WorkPosYXCPB[3]  = i; //picker ��ġ ����

							clsFunc.Data_Exchange_PickPlace(PICKER_PLACE_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //��Ŀ �� Ʈ���� ���� ���� �Լ�
							//�� �Լ����� ��� ������ ���� �� Ŭ���� �Ѵ�
							//st_picker[m_nRobotSite].st_pcb_info[y].nYesNo = CTL_YES;					//��Ŀ ���� ��
							//st_tray_info[nWork_Site].st_pcb_info[m_npFirstPosYXP[0] + (y - m_n_FirstPicker_Num)][m_npFirstPosYXP[1]].nYesNo = CTL_NO; //Ʈ���� ���� Ŭ����
							//st_count_info.nSSD_Input[0]++;
						}
						else if(nTest_Site == TESTSITE_RIGHT) //RIGHT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ����������0,1,2,3 �� ��ġ ������ �����ϴ� 
						{
							nFind_JobPos = i ; //LEFT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ���������� 3,2,1,0 �� �ݴ��̴� 
							nFind_JobPos = nFind_JobPos + nStart_FixPos; //st_test_site_info[nWork_Site].nStart_FixPos;								

							//�Ʒ� �Լ����� ������� ��ġ ������ �����Ѵ�
							m_npSet_WorkPosYXCPB[0]  = nFind_JobPos; //Test Site�� Y�� ������ ���� //������ ��ġ ����
							m_npSet_WorkPosYXCPB[1]  = nFind_JobPos; //Test Site�� X�� ��� //������ ��ġ ����
							m_npSet_WorkPosYXCPB[3]  = i; //picker ��ġ ����

							clsFunc.Data_Exchange_PickPlace(PICKER_PLACE_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //��Ŀ �� Ʈ���� ���� ���� �Լ� 
						}
						else
						{
							break;
						}
						//kwlee 2017.0905
						if (st_basic_info.nModeXgem == YES)
						{
							if (st_test_site_info[m_nRobotSite].st_pcb_info[i].nYesNo == YES)
							{
								clsXgem.OnMcProductBufferToSocket(END,st_test_site_info[m_nRobotSite].st_pcb_info[i].nTrayCnt,st_test_site_info[m_nRobotSite].st_pcb_info[i].strSerialNo,nFind_JobPos,st_sync_info.nTestSite_Target_THD_Work_Site[1]);
							}
						}
						////
					}
				}
				else if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF)
				{ 										
					if(nTest_Site == TESTSITE_LEFT) //left site�� �۾��ϱ� ���� �۾� ���϶�
					{
						nFind_JobPos = m_nFirst_Buff_Num - ((i - m_nFirstPicker_Num) * 2);  //13,11,9,7,5,3,1 ��ġ 
						if(i >= 0 && nFind_JobPos >= 0 && nFind_JobPos < st_recipe_info.nRetestBuffer_Num) //2016.0906 if(i >= 0 && m_npFindWorkPosYXCPB[1] + (i - m_nFirstPicker_Num) < st_recipe_info.nRetestBuffer_Num)
						{
							//�Ʒ� �Լ����� ������� ��ġ ������ �����Ѵ�
							m_npSet_WorkPosYXCPB[0]  = nFind_JobPos; //����sms Y�� ��� //������ ��ġ ����
							m_npSet_WorkPosYXCPB[1]  = nFind_JobPos; //qjvjsms X�� ������� ���� //������ ��ġ ����
							m_npSet_WorkPosYXCPB[3]  = i; //picker ��ġ ����

							clsFunc.Data_Exchange_PickPlace(PICKER_PLACE_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //��Ŀ �� Ʈ���� ���� ���� �Լ�
							//�� �Լ����� ��� ������ ���� �� Ŭ���� �Ѵ�
							//st_picker[m_nRobotSite].st_pcb_info[y].nYesNo = CTL_YES;					//��Ŀ ���� ��
							//st_tray_info[nWork_Site].st_pcb_info[m_npFirstPosYXP[0] + (y - m_n_FirstPicker_Num)][m_npFirstPosYXP[1]].nYesNo = CTL_NO; //Ʈ���� ���� Ŭ����
							//st_count_info.nSSD_Input[0]++;
							//st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]]
							if (st_buffer_info[nWork_Site].st_pcb_info[m_npSet_WorkPosYXCPB[0]].nYesNo == YES)
							{
								st_count_info.nPrimeRejectCount[1][0]++;

								if (st_lot_info[m_nLotProcessNum].strLotNo == st_buffer_info[nWork_Site].st_pcb_info[m_npSet_WorkPosYXCPB[0]].strLotNo)
								{
									if(m_nLotProcessNum == LOT_CURR)
									{
										// ���� �������� Lot�� ������
										//st_lot_info[LOT_CURR].nLdCnt++;
										st_count_info.nPrimeRejectCount[0][0]++;
									}
									else if(m_nLotProcessNum == LOT_NEXT)
									{
										// next lot ���� ����
										//st_lot_info[LOT_NEXT].nLdCnt++;
										st_count_info.nPrimeRejectCount[0][1]++;
									}
								} 
							}
						}		
						else
						{
							nFind_JobPos = 0; //james test 2016.0907
						}
					}
					else if(nTest_Site == TESTSITE_RIGHT) //RIGHT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ����������0,1,2,3 �� ��ġ ������ �����ϴ� 
					{
						nFind_JobPos = m_nFirst_Buff_Num + ((i - m_nFirstPicker_Num) * 2);
						if(i >= 0 && nFind_JobPos >= 0  && nFind_JobPos < st_recipe_info.nRetestBuffer_Num) //2016.0906 if(i >= 0 && m_npFindWorkPosYXCPB[1] + (i - m_nFirstPicker_Num) < st_recipe_info.nRetestBuffer_Num)
						{
							//�Ʒ� �Լ����� ������� ��ġ ������ �����Ѵ�
							m_npSet_WorkPosYXCPB[0]  = nFind_JobPos; //����sms Y�� ��� //������ ��ġ ����
							m_npSet_WorkPosYXCPB[1]  = nFind_JobPos; //qjvjsms X�� ������� ���� //������ ��ġ ����
							m_npSet_WorkPosYXCPB[3]  = i; //picker ��ġ ����

							clsFunc.Data_Exchange_PickPlace(PICKER_PLACE_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //��Ŀ �� Ʈ���� ���� ���� �Լ� 

							if (st_buffer_info[nWork_Site].st_pcb_info[m_npSet_WorkPosYXCPB[0]].nYesNo == YES)
							{
								st_count_info.nPrimeRejectCount[1][0]++;

								if (st_lot_info[m_nLotProcessNum].strLotNo == st_buffer_info[nWork_Site].st_pcb_info[m_npSet_WorkPosYXCPB[0]].strLotNo)
								{
									if(m_nLotProcessNum == LOT_CURR)
									{
										// ���� �������� Lot�� ������
										//st_lot_info[LOT_CURR].nLdCnt++;
										st_count_info.nPrimeRejectCount[0][0]++;
									}
									else if(m_nLotProcessNum == LOT_NEXT)
									{
										// next lot ���� ����
										//st_lot_info[LOT_NEXT].nLdCnt++;
										st_count_info.nPrimeRejectCount[0][1]++;
									}
								} 
							}
						}
						else
						{
							nFind_JobPos = 0; //james test 2016.0907
						}
					}					
				}	
			}
		}

		if (nRet_1 == RET_GOOD) // 1000 step���� ������ ��Ŀ ������ �°� ��� ������ ������ ��� ��
		{
			//ȭ�� ���� ����
			// jtkim 20160930 Log
			if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8)
			{
				//clsLog.OnLogTestSite(0, nWork_Site, st_test_site_info[nWork_Site]);
				// jtkim 20161008 ������å�� ��û
				//clsLog.OnLogTestSite(0, nWork_Site, nTest_Site - TESTSITE_LEFT,st_test_site_info[nWork_Site]);
				//2016.1018
				if( nStart_FixPos == 0 )
					clsLog.OnLogTestSite(0, nWork_Site, 0,st_test_site_info[nWork_Site]);
				else
					clsLog.OnLogTestSite(0, nWork_Site, 1,st_test_site_info[nWork_Site]);
			}

			m_nDvcRepickCount = 0;
			m_nPartialPickCount = 0;

			m_bDvcWaitChk_Falg = false;

			// jtkim 20150216 conveyor/loader tray ȭ�� ǥ��
			if (st_handler_info.cWndMain != NULL)
			{
				st_handler_info.cWndMain->PostMessage(WM_WORK_DISPLAY, MAIN_LD_PICKER_DISPLAY, 0);
				st_handler_info.cWndMain->PostMessage(WM_WORK_DISPLAY, MAIN_LD_TRAY_DISPLAY, 0);
				st_handler_info.cWndMain->PostMessage(WM_WORK_DISPLAY, MAIN_RETEST_BUF_DISPLAY, 0);
			}

			//2017.0109
			//kwlee 2017.0117
		
			//st_picker[m_nRobotSite].st_pcb_info[i].strLogData ���α׷� �׾ ���Ƴ���..
			//Ȯ�� �ʿ�..
// 			clsLog.LogTransfer(_T("TESTSITE_ROBOT"),
// 								_T("MOVE_CDIMM"),
// 								1,
// 								st_picker[m_nRobotSite].st_pcb_info[i].strMaterial,
// 								_T("CDIMM"),
// 								st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[0],
// 								st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[1],
// 								1, 
// 								st_picker[m_nRobotSite].st_pcb_info[i].strLogKey,
// 								st_picker[m_nRobotSite].st_pcb_info[i].strLogData);

			m_nPlace_Step = 6000;
		}
		else if (nRet_1 == RET_ERROR) //������ ��Ŀ�� �Ѱ��� ����� ���� �������� �̰����� �̵�
		{
			CTL_Lib.Alarm_Error_Occurrence(2601, dWARNING, clsFunc.m_strAlarmCode);
			m_nPlace_Step = 0;
		}
		break;

	case 6000:
		nFuncRet = RET_GOOD;
		m_nPlace_Step = 0;
		break;

	default : 
		break;
		
	}

	return nFuncRet;

}



//james 2016.0814 ���׽�Ʈ ��� ����
//���� pick & place ��� (��Ŀ�� ��ü Up/Down���� ���۽� ���簣 �浹�� �߻��� �� �־� ���� ���ǰ� �ʿ��ϴ� //
//1�� �۾����� : 0,2,4,6, 8,10,12,14 - 8�� 
//2�� �۾����� : 1,3,5,7, 9,11,13    - 7��
//
//���׽�Ʈ ���ۻ����: �� LOT�� �ϳ����� ����Ѵ�, ����LOT ó�� ����
// Left/Right Tets Site���� ������ ������ PIN ������ �ݴ뿩�� ��� ����Ʈ���� �°����� Ȯ���Ͽ� ȸ���ϸ鼭 �޾ƾ� �Ѵ� 
// �̶� ���� ���۴� L/R ���о��� �ش� lot�� �Ѱ��� ��� �޾�, ó���Ѵ�.    
//
// PLACE ��� ������.
// 1. 1�� �۾�����(¦����) ������ ������ �ѹ��� ���� ������ ã�� ���´�. 
// 2. 2�� �۾�����(Ȧ����) ���� ���縦 ���� ��Ȳ�̸� , �̰� ������ ����, ������ 1�� �۾� ������ ���縦 ���׽�Ʈ�� �����Ѵ� 
// 3. �� ����Ŭ ���� ��ε� ������, Ȥ�� Ȧ������ ���׽�Ʈ ���ۿ� ���簡 ������ �� ���縦 ¦������ �ִ��� ������ �ش� 
// 4. �� Ȧ������ �ӽ� ���� �������� ������ ����� �־�� �Ѵ�

//����� ������� �ʴ´� (james 2016.0821 ����� Place()�� �����Ͽ����� ��)
int CRunTestSiteRobot::Process_BinSort_Place(int nMode, int nWork_Site, int nSortBin) //retest ���縦 ���� �Ѵ� 
{
	int nFuncRet = RET_PROCEED;
	int nRet_1, nRet_2, nRet_3 = 0, i, nFlag;
	int nFind_JobPos=0, nCount=0;

	int nBuff_x = 0;
	int nPicker_x =0; 
	int nPicker_Cnt = 0;
	int nBuffer_Cnt = 0;
	int nBuffFirstNo= 0;	

	int nStart_FixPos = 0; //james 2016.0905 test �����Ұ� 
	
	 
	clsFunc.OnThreadFunctionStepTrace(27, m_nPlace_Step);

	switch(m_nSortPlace_Step)
	{
	case 0:
		m_bDvcWaitChk_Falg =  false;
		m_nPickRetry_Cnt = 0;
		m_nSortPlace_Step = 100;

		nFlag = 0;
		break;

	case 100:
		m_nSortPlace_Step = 900;
		break;		

	case 900:
		m_dCurrentPos[m_nRobot_Z] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[m_nRobot_Z] > st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
		{
			m_nSortPlace_Step = 910;
		}
		else
		{
			m_nSortPlace_Step = 920;
		}
		break;

	case 910:
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{
			m_nSortPlace_Step = 920;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nSortPlace_Step = 900;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(2610, dWARNING, st_alarm_info.strCode);
			m_nSortPlace_Step = 910;
		}
		break;

	case 920:
		m_dCurrentPos[m_nRobot_Ejector] = COMI.Get_MotCurrentPos(m_nRobot_Ejector);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[m_nRobot_Ejector] > st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[m_nRobot_Ejector].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
		{
			m_nSortPlace_Step = 930;
		}
		else
		{
			m_nSortPlace_Step = 940;
		}
		break;

	case 930:
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Ejector, st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{
			m_nSortPlace_Step = 940;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nSortPlace_Step = 930;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(2620, dWARNING, st_alarm_info.strCode);
			m_nSortPlace_Step = 930;
		}
		break;

	case 940:
		m_bDvcWaitChk_Falg = false;
		m_nSortPlace_Step = 1000;
		break; 

	///////////////////////////////////////////////////////////////////////
	// �۾��� ������ ã��, ����
	///////////////////////////////////////////////////////////////////////
	case 1000:
		m_dCurrentPos[m_nRobot_Z] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[m_nRobot_Z] > st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
		{
			m_nSortPlace_Step = 900;
			break;
		}
		m_dCurrentPos[m_nRobot_Ejector] = COMI.Get_MotCurrentPos(m_nRobot_Ejector);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[m_nRobot_Ejector] > st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
		{
			m_nSortPlace_Step = 900;
			break;
		}

		nRet_1 = clsFunc.Check_PickerStatus(0, THD_TEST_RBT, CTL_YES, m_npPicker_YesNo, m_npPicker_Vacuum_Status, m_npPicker_OutputStatus); //nMode == 0 �̸� ��������, nMode == 1�̸� ��������� ������ ���� ��Ŀ ���¸� Ȯ���Ѵ�,
		if(nRet_1 == RET_GOOD) //���� ������ ���� ��Ŀ ���¸� üũ 
		{//��Ŀ ������ ���� ���°� �����̴�
		}
		else// if(nRet_1 == RET_ERROR)
		{
			if(m_bDvcWaitChk_Falg == false)
			{
				m_bDvcWaitChk_Falg =  true;
				m_dwWaitUntil[0] = GetCurrentTime();
				break;
			}
			else
			{
				m_dwWaitUntil[1] = GetCurrentTime();
				m_dwWaitUntil[2] = m_dwWaitUntil[1] - m_dwWaitUntil[0];
				if(m_dwWaitUntil[2] < IO_STABLE_WAIT) break;
				CTL_Lib.Alarm_Error_Occurrence(2630, dWARNING, clsFunc.m_strAlarmCode);
				m_bDvcWaitChk_Falg = false;
				break;
			}
		}

		//�۾� ���� ��ġ�� ã�´�		
		m_nFirst_Buff_Num = -1; m_nFirstPicker_Num = -1; //flag clear

		m_nSort_Count = 0;  
		for(i = 0; i < m_nPickerPara; i++)
		{
			m_nSortPickerInfo[i] = CTL_NO;
			nFlag = CTL_NO;
			if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_YES && st_picker[m_nRobotSite].st_pcb_info[i].nBdNum == nSortBin)// 
			{
				// jtkim 21061004 check
				if(nFlag == CTL_NO)
				{
					m_nFirstPicker_Num = i;
					m_nSort_Count++; //�ش������ ���� �� ���� 
					i = m_nPickerPara;

					nFlag = CTL_YES;
				}
			}
		}

		if(m_nFirstPicker_Num == -1)
		{
			//��Ŀ�� ���̻� �۾��� ���� ����
			m_nSortPlace_Step = 0;
			nFuncRet = RET_PICKER_NOT_FIND;
			break;
		}
		else
		{
			m_nSortPlace_Step = 1100;
		}
		break;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//�۾� 1���� : �ۼ��� ���� ��ġ�� ������ ã�´� 	//0,2,4,6,8,10,12,14(TESTSITE_RIGHT) . �Ǵ� 14,12,10,8,6,4,2,0(TESTSITE_LEFT)�� �����̸� ���� ������ �ݴ��� ������ �켱 ã�´�	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	case 1100:  
		nPicker_Cnt = 0;
		nBuffer_Cnt = 0;
		nFlag = CTL_NO;
		nRet_1 = CTL_NO;
		nBuffFirstNo= 0;

		if(nStart_FixPos == TESTSITE_LEFT) //test left site�� wkwofmf �۾��ϱ� ���� �۾� ���϶�, ���׽�Ʈ ���۴� �׻� ���� �⺻ ����(Right Test Site ����)�� �����Ѵ�, LEFT SITE���� ���� ���۸� ȸ���Ͽ� 1�� �� ������ ����� 
		{			
			nBuffFirstNo = st_recipe_info.nRetestBuffer_Num - 1;  //nRetestBuffer_Num == 15 // 14,12,10,8,6,4,2,0 ��� 
			//for(nBuff_x = 1; nBuff_x < st_recipe_info.nRetestBuffer_Num; (nBuff_x = nBuff_x + 2)) //nRetestBuffer_Num == 15 //0,2,4,6,8,10,12,14 �� �켱 ã�´�
			for(nBuff_x = nBuffFirstNo; nBuff_x >= 0; (nBuff_x = nBuff_x - 2))//nRetestBuffer_Num == 15 //14,12,10,8,6,4,2,0 �� 8�� ��ġ�� �켱 ã�´�
			{	//���� ��ġ�� �������� �ϳ��� ã�� ����. 		 
				m_nFirst_Buff_Num = -1;
				nPicker_Cnt = 0; nBuffer_Cnt = 0; //clear 
				nFlag = CTL_NO; 
				if(st_buffer_info[nWork_Site].st_pcb_info[nBuff_x].nYesNo == CTL_NO) //���ۿ��� ���۰����� ��ġ�� ã�´� 
				{				 
					for(nPicker_x = 0; nPicker_x < TEST_PICKER_PARA; nPicker_x++) //TEST_PICKER_PARA == 4 //for(nPicker_x = m_nFirstPicker_Num; nPicker_x < TEST_PICKER_PARA; nPicker_x++) //TEST_PICKER_PARA == 4
					{
						// jtkim 20161004 check
						m_npPicker_YesNo[nPicker_x] = CTL_NO; //�۾��� ��Ŀ���� clear�ϱ� 0���� for������ �ʱ�ȭ ���� 
						if(nPicker_x < m_nFirstPicker_Num) continue;

						if(st_picker[m_nRobotSite].st_pcb_info[nPicker_x].nYesNo == CTL_YES && st_picker[m_nRobotSite].st_pcb_info[nPicker_x].nBdNum == nSortBin)//��Ŀ�� ���簡 �հ�, ���ú�� ��ġ�ϸ� 
						{
							m_npPicker_YesNo[nPicker_x] = CTL_YES; 

							nPicker_Cnt++;
					 
							if(st_buffer_info[nWork_Site].st_pcb_info[ nBuff_x - ((nPicker_x - m_nFirstPicker_Num) * 2)].nYesNo == CTL_NO) //������ ������ ��ĭ�ǳ� �ϳ��� üũ, ���� ������� ��Ŀ����� 1/2 �� 1:1�� ��ġ�����ʾ� ���� �ʿ�.
							{
								//��Ŀ�� ������ ������ ����. ���� �� �� �ִ� 
								if(nFlag == CTL_NO)
								{
									m_nFirst_Buff_Num = nBuff_x;
									nFlag = CTL_YES;
								}
								nBuffer_Cnt++;
							}
							else
							{
								//��Ŀ�� ������ ������ �ٸ���. �� ������ ����  ��ġ������ �浹�Ͽ� ������ �� ����.
								nPicker_x = TEST_PICKER_PARA;
								break;
							}
						}
					} 

					if(nPicker_Cnt == nBuffer_Cnt) //���縦 ���ۿ� �ѹ��� ���� �� �ִ� ��ġ�� �ִ� //���� OK
					{
						nBuff_x = st_recipe_info.nRetestBuffer_Num;				 
						nRet_1 = CTL_YES;
					}
				}
			}
		}
		else if(nStart_FixPos == TESTSITE_RIGHT)
		{

			for(nBuff_x = 0; nBuff_x < st_recipe_info.nRetestBuffer_Num; (nBuff_x = nBuff_x + 2)) //nRetestBuffer_Num == 15 //0,2,4,6,8,10,12,14 �� �켱 ã�´�
			{			 
				m_nFirst_Buff_Num = -1;
				nPicker_Cnt = 0; nBuffer_Cnt = 0; //clear 
				nFlag = CTL_NO; 
				if(st_buffer_info[nWork_Site].st_pcb_info[nBuff_x].nYesNo == CTL_NO) //���ۿ��� ���۰����� ��ġ�� ã�´� 
				{				 
					for(nPicker_x = 0; nPicker_x < TEST_PICKER_PARA; nPicker_x++) //TEST_PICKER_PARA == 4 //for(nPicker_x = m_nFirstPicker_Num; nPicker_x < TEST_PICKER_PARA; nPicker_x++) //TEST_PICKER_PARA == 4
					{
						m_npPicker_YesNo[nPicker_x] = CTL_NO; //�۾��� ��Ŀ���� clear�ϱ� ���� 
						if(nPicker_x < m_nFirstPicker_Num) continue;

						if(st_picker[m_nRobotSite].st_pcb_info[nPicker_x].nYesNo == CTL_YES && st_picker[m_nRobotSite].st_pcb_info[nPicker_x].nBdNum == nSortBin)// 
						{
							m_npPicker_YesNo[nPicker_x] = CTL_YES; 
							nPicker_Cnt++;
					 
							if(st_buffer_info[nWork_Site].st_pcb_info[ nBuff_x + ((nPicker_x - m_nFirstPicker_Num) * 2)].nYesNo == CTL_NO)
							{
								//��Ŀ�� ������ ������ ����. ���� �� �� �ִ� 
								if(nFlag == CTL_NO)
								{
									m_nFirst_Buff_Num = nBuff_x;
									nFlag = CTL_YES;
								}
								nBuffer_Cnt++;
							}
							else
							{
								//��Ŀ�� ������ ������ �ٸ���. �� ������ ����  ��ġ������ �浹�Ͽ� ������ �� ����.
								m_nFirst_Buff_Num = -1;
								nPicker_x = TEST_PICKER_PARA;
								break;
							}
						}
					} 

					if(nPicker_Cnt == nBuffer_Cnt) //���縦 ���ۿ� �ѹ��� ���� �� �ִ� ��ġ�� �ִ� //���� OK
					{
						nBuff_x = 15;				 
						nRet_1 = CTL_YES;
					}
				}
			}

			if(nRet_1 == CTL_YES) //�ѹ��� ���� ��ġ ȹ�� 
			{
				m_nSortPlace_Step = 1150; 
			}
			else 
			{
				m_nFirst_Buff_Num = -1;
				m_nSortPlace_Step = 1120;
			}
		}
		break;

	///////////////////////////////////////////////
	//�۾� 2���� :Ȧ���� ���� ������ ã�´� 	//1,3,4,5,7,9,11,13 �� 2������ ã�´�	
	///////////////////////////////////////////////
	case 1120:
		nPicker_Cnt = 0;
		nBuffer_Cnt = 0;
		nFlag = CTL_NO;
		nRet_1 = CTL_NO;
		nBuffFirstNo= 0;

		if(nStart_FixPos == TESTSITE_LEFT) //left site�� �۾��ϱ� ���� �۾� ���϶�
		{			
			nBuffFirstNo = st_recipe_info.nRetestBuffer_Num - 2; //13,11,9,7,5,3,1 ��ġ 
			//for(nBuff_x = 1; nBuff_x < st_recipe_info.nRetestBuffer_Num; (nBuff_x = nBuff_x + 2)) //nRetestBuffer_Num == 15 //0,2,4,6,8,10,12,14 �� �켱 ã�´�
			for(nBuff_x = nBuffFirstNo; nBuff_x >= 0; (nBuff_x = nBuff_x - 2))//nRetestBuffer_Num == 15 //14,12,10,8,6,4,2,0 �� �켱 ã�´�
			{			 
				m_nFirst_Buff_Num = -1;
				nPicker_Cnt = 0; nBuffer_Cnt = 0; //clear 
				nFlag = CTL_NO; 
				if(st_buffer_info[nWork_Site].st_pcb_info[nBuff_x].nYesNo == CTL_NO) //���ۿ��� ���۰����� ��ġ�� ã�´� 
				{				 
					for(nPicker_x = 0; nPicker_x < TEST_PICKER_PARA; nPicker_x++) //TEST_PICKER_PARA == 4 //for(nPicker_x = m_nFirstPicker_Num; nPicker_x < TEST_PICKER_PARA; nPicker_x++) //TEST_PICKER_PARA == 4
					{
						m_npPicker_YesNo[nPicker_x] = CTL_NO; //�۾��� ��Ŀ���� clear�ϱ� ���� 
						if(nPicker_x < m_nFirstPicker_Num) continue;

						if(st_picker[m_nRobotSite].st_pcb_info[nPicker_x].nYesNo == CTL_YES && st_picker[m_nRobotSite].st_pcb_info[nPicker_x].nBdNum == nSortBin)// 
						{
							m_npPicker_YesNo[nPicker_x] = CTL_YES; 
							nPicker_Cnt++;
					 
							if(st_buffer_info[nWork_Site].st_pcb_info[ nBuff_x - ((nPicker_x - m_nFirstPicker_Num) * 2)].nYesNo == CTL_NO)
							{
								//��Ŀ�� ������ ������ ����. ���� �� �� �ִ� 
								if(nFlag == CTL_NO)
								{
									m_nFirst_Buff_Num = nBuff_x;
									nFlag = CTL_YES;
								}
								nBuffer_Cnt++;
							}
							else
							{
								//��Ŀ�� ������ ������ �ٸ���. �� ������ ����  ��ġ������ �浹�Ͽ� ������ �� ����.
								nPicker_x = TEST_PICKER_PARA;
								break;
							}
						}
					} 

					if(nPicker_Cnt == nBuffer_Cnt) //���縦 ���ۿ� �ѹ��� ���� �� �ִ� ��ġ�� �ִ� //���� OK
					{
						nBuff_x = st_recipe_info.nRetestBuffer_Num;				 
						nRet_1 = CTL_YES;
					}
				}
			}
		}
		else if(nStart_FixPos == TESTSITE_RIGHT)
		{
			for(nBuff_x = 1; nBuff_x < st_recipe_info.nRetestBuffer_Num; (nBuff_x = nBuff_x + 2)) //nRetestBuffer_Num == 15 //1,3,5,7,9,11,13 �� �켱 ã�´�
			{			 
				m_nFirst_Buff_Num = -1;
				nPicker_Cnt = 0; nBuffer_Cnt = 0; //clear 
				nFlag = CTL_NO; 
				if(st_buffer_info[nWork_Site].st_pcb_info[nBuff_x].nYesNo == CTL_NO) //���ۿ��� ���۰����� ��ġ�� ã�´� 
				{				 
					for(nPicker_x = 0; nPicker_x < TEST_PICKER_PARA; nPicker_x++) //TEST_PICKER_PARA == 4 //for(nPicker_x = m_nFirstPicker_Num; nPicker_x < TEST_PICKER_PARA; nPicker_x++) //TEST_PICKER_PARA == 4
					{
						m_npPicker_YesNo[nPicker_x] = CTL_NO; //�۾��� ��Ŀ���� clear�ϱ� ���� 
						if(nPicker_x < m_nFirstPicker_Num) continue;

						if(st_picker[m_nRobotSite].st_pcb_info[nPicker_x].nYesNo == CTL_YES && st_picker[m_nRobotSite].st_pcb_info[nPicker_x].nBdNum == nSortBin)// 
						{
							m_npPicker_YesNo[nPicker_x] = CTL_YES; 

							nPicker_Cnt++;
					 
							if(st_buffer_info[nWork_Site].st_pcb_info[ nBuff_x + ((nPicker_x - m_nFirstPicker_Num) * 2)].nYesNo == CTL_NO)
							{
								//��Ŀ�� ������ ������ ����. ���� �� �� �ִ� 
								if(nFlag == CTL_NO)
								{
									m_nFirst_Buff_Num = nBuff_x;
									nFlag = CTL_YES;
								}
								nBuffer_Cnt++;
							}
							else
							{
								//��Ŀ�� ������ ������ �ٸ���. �� ������ ����  ��ġ������ �浹�Ͽ� ������ �� ����.
								nPicker_x = TEST_PICKER_PARA;
								break;
							}
						}
					} 

					if(nPicker_Cnt == nBuffer_Cnt) //���縦 ���ۿ� �ѹ��� ���� �� �ִ� ��ġ�� �ִ�, //���� OK
					{
						nBuff_x = st_recipe_info.nRetestBuffer_Num;				 
						nRet_1 = CTL_YES;
					}
				}
			}
		}


		if(nRet_1 == CTL_YES) //�ѹ��� ���� ��ġ ȹ�� 
		{
			m_nSortPlace_Step = 1150; 
		}
		else 
		{
			m_nFirst_Buff_Num = -1;
			//error 
			//�̰��� ������ �ȵ� 
		}
		break;

	///////////////////////////////////////////////////////////////////
	// ��Ŀ�� ���� ������ �������� ������ ��Ŀ�� �����Ͽ� ó������ 
	///////////////////////////////////////////////////////////////////
	case 1150:		//���� �۾� ��ġ�� ����
		m_npFindWorkPosYXCPB[0] = m_nFirst_Buff_Num; m_npFindWorkPosYXCPB[1] = m_nFirst_Buff_Num; m_npFindWorkPosYXCPB[3] = m_nFirstPicker_Num;

		m_nMove_Flag[0] = CTL_NO; m_nMove_Flag[1] = CTL_NO;
		m_nSortPlace_Step = 2000;
		 	
		break;

	case 2000:
		m_dCurrentPos[2] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[2] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
		{
			m_nSortPlace_Step = 900;
			break;
		}
		m_dCurrentPos[m_nRobot_Ejector] = COMI.Get_MotCurrentPos(m_nRobot_Ejector);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[m_nRobot_Ejector] > st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
		{
			m_nSortPlace_Step = 900;
			break;
		}


		nRet_1 = clsFunc.Calculate_MovePos_Find(0, m_nRobot_Y, nWork_Site, 0, m_npFindWorkPosYXCPB, &m_dTemp_CalTargetPos_Y);             //Calculate_MovePos_Find(int nMotNum, int nSite, int nWorkPart, int *npFirstYXP_Pos, double *dpGetTargetPos)
		nRet_2 = clsFunc.Calculate_MovePos_Find(0, m_nRobot_X, nWork_Site, 0, m_npFindWorkPosYXCPB, &m_dTemp_CalTargetPos_X);
		if(nRet_1 == RET_GOOD && nRet_2 == RET_GOOD)
		{//ok
			m_dpTargetPosList[0] = m_dTemp_CalTargetPos_Y;
			m_dpTargetPosList[1] = m_dTemp_CalTargetPos_X;
		}
		else
		{
			CTL_Lib.Alarm_Error_Occurrence(2640, dWARNING, clsFunc.m_strAlarmCode);
			break;
		}
		
		nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate); 
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{
			m_bDvcWaitChk_Falg =  false;
			m_nSortPlace_Step = 2100;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nSortPlace_Step = 2000;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(2650, dWARNING, st_alarm_info.strCode);
			m_nSortPlace_Step = 2000;
		}
		break; 	

	case 2100:
		if(nWork_Site == THD_LD_BUFF || nWork_Site == THD_ULD_BUFF)
		{
			//�ε� ���ۿ� ��û�Ͽ� ���簡 �غ�Ǿ� CTL_READY �� ����, ���縦 ���� �� �ִ� 
			 if(st_sync_info.nTestRbt_Dvc_Req[nWork_Site][0] == CTL_READY && st_sync_info.nTestRbt_Dvc_Req[nWork_Site][1] == WORK_PLACE) //�ε� ���۰� �غ�Ǳ⸦ ��ٸ��� 
			 {				 //james 2016.1005 = CTL_READY
				 m_nSortPlace_Step = 3000;
			 }		
		}
		else  //if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF || nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8)
		{//���� ���� ���� 
			 m_nSortPlace_Step = 3000;
		}		
		break;

	case 3000:  
		m_dCurrentPos[m_nRobot_Ejector] = COMI.Get_MotCurrentPos(m_nRobot_Ejector);  // ���� Ư�� ���� ���� ��ġ ���� �Լ�
		if(m_dCurrentPos[m_nRobot_Ejector] > st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //���� ��ġ�� ��� ������ ����ġ ��Ų��
		{
			m_nSortPlace_Step = 900;
			break;
		}

		nRet_1 = COMI.Check_MotPosRange(m_nRobot_Y, m_dpTargetPosList[1], st_motor_info[m_nRobot_Y].d_allow);
		nRet_2 = COMI.Check_MotPosRange(m_nRobot_X, m_dpTargetPosList[0], st_motor_info[m_nRobot_X].d_allow);
			if(nRet_1 != BD_GOOD || nRet_2 != BD_GOOD)
		{
			m_nSortPlace_Step = 2000;
			break;
		}

		Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON); //�̸� ������  

		if(nWork_Site == THD_LD_BUFF)
		{
			m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_LDBUF_PLACE];
		}
		else if(nWork_Site == THD_ULD_BUFF)
		{//���� ���� ���� 
			m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_ULDBUF_PLACE];
		}
		else if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_4)
		{
			m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_AUTOTEACH_VISION_TESTSITE_LEFT];
		}
		else if(nWork_Site >= THD_TESTSITE_4 && nWork_Site <= THD_TESTSITE_8)
		{
			m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_AUTOTEACH_VISION_TESTSITE_RIGHT];
		}
		else if(nWork_Site == THD_RETEST_1_BUFF)
		{
			m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_RETESTBUF_1_PLACE];
		}
		else if(nWork_Site == THD_RETEST_2_BUFF)
		{
			m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_RETESTBUF_2_PLACE];
		}
		else
		{//
			break;
		} 

		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, m_dpTargetPosList[2], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{ 			
			m_nSortPlace_Step = 3100;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nSortPlace_Step = 3000;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(2660, dWARNING, st_alarm_info.strCode);
			m_nSortPlace_Step = 3000;
		}		
		break;	

	case 3100: //���縦 ���´� 
		if(nWork_Site == THD_LD_BUFF || nWork_Site == THD_ULD_BUFF)
		{
			 m_nSortPlace_Step = 3110;
		}
		else  //if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF || nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8)
		{
			m_nSortPlace_Step = 3200;
		}
		break;

	

	case 3200:
		nRet_1 = clsFunc.Check_PickerStatus(1, THD_TEST_RBT, CTL_NO, m_npPicker_YesNo, m_npPicker_Vacuum_Status, m_npPicker_OutputStatus);
		if(nRet_1 == RET_GOOD) //���� ������ ���� ��Ŀ ���¸� üũ 
		{//��Ŀ ������ ���� ���°� �����̴�
			m_nSortPlace_Step = 3300;
		}
		else// if(nRet_1 == RET_ERROR)
		{
			if(m_bDvcWaitChk_Falg == false)
			{
				m_bDvcWaitChk_Falg =  true;
				m_dwWaitUntil[0] = GetCurrentTime();
				break;
			}
			else
			{
				m_dwWaitUntil[1] = GetCurrentTime();
				m_dwWaitUntil[2] = m_dwWaitUntil[1] - m_dwWaitUntil[0];
				if(m_dwWaitUntil[2] < IO_STABLE_WAIT) break;
				CTL_Lib.Alarm_Error_Occurrence(2670, dWARNING, clsFunc.m_strAlarmCode);
				m_bDvcWaitChk_Falg = false;
				break;
			}
		}
		break;

	case 3300:
		m_nSortPlace_Step = 4000;
		break;

	case 4000:
		Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON);  //���縦 ���´� 
		m_nSortPlace_Step = 4010;
		break;

	case 4010:
		nRet_1 = Chk_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON);  
		if(nRet_1 == RET_GOOD)
		{
			m_bDvcWaitChk_Falg = false;
			m_nPick_Step = 4100;
		}
		else if(nRet_1 == RET_ERROR)
		{
			Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON);  //������ ������ 
			CTL_Lib.Alarm_Error_Occurrence(2680, dWARNING, m_strAlarmCode);
			m_nPick_Step = 4000;
		}
		break;

	case 4100:
		 
			m_nSortPlace_Step = 4200; 
		break;

	case 4200:
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //�������� �̵�
		{
			m_bDvcWaitChk_Falg = false;
			m_nSortPlace_Step = 5000;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nSortPlace_Step = 4200;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�
			CTL_Lib.Alarm_Error_Occurrence(2690, dWARNING, st_alarm_info.strCode);
			m_nSortPlace_Step = 4200;
		}
		break;

	case 5000:
		nRet_1 = clsFunc.Check_PickerStatus(1, THD_TEST_RBT, CTL_NO, m_npPicker_YesNo, m_npPicker_Vacuum_Status, m_npPicker_OutputStatus);
		if (nRet_1 != RET_GOOD) //
		{
			if(m_bDvcWaitChk_Falg == false)
			{
				m_bDvcWaitChk_Falg =  true;
				m_dwWaitUntil[0] = GetCurrentTime();
				break;
			}
			else
			{
				m_dwWaitUntil[1] = GetCurrentTime();
				m_dwWaitUntil[2] = m_dwWaitUntil[1] - m_dwWaitUntil[0];
				if(m_dwWaitUntil[2] < IO_STABLE_WAIT) break;
			}
		}

		nBuff_x = 0;
		for (i = m_nFirstPicker_Num; i < m_nPickerPara; i++)
		{
			if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_YES && m_npPicker_YesNo[i] == CTL_YES 
				&& (m_npPicker_Vacuum_Status[i] == IO_ON && m_npPicker_OutputStatus[i] == IO_ON) )
			{
				if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF)
				{
					if(i >= 0 && m_npFindWorkPosYXCPB[1] + (i - m_nFirstPicker_Num) < st_recipe_info.nRetestBuffer_Num)
					{
						if(nStart_FixPos == TESTSITE_LEFT) //left site�� �۾��ϱ� ���� �۾� ���϶�
						{
							nFind_JobPos = m_nFirst_Buff_Num - ((i - m_nFirstPicker_Num) * 2);  //13,11,9,7,5,3,1 ��ġ 

							//�Ʒ� �Լ����� ������� ��ġ ������ �����Ѵ�
							m_npSet_WorkPosYXCPB[0]  = nFind_JobPos; //����sms Y�� ��� //������ ��ġ ����
							m_npSet_WorkPosYXCPB[1]  = nFind_JobPos; //qjvjsms X�� ������� ���� //������ ��ġ ����
							m_npSet_WorkPosYXCPB[3]  = i; //picker ��ġ ����

							clsFunc.Data_Exchange_PickPlace(PICKER_PLACE_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //��Ŀ �� Ʈ���� ���� ���� �Լ�
							//�� �Լ����� ��� ������ ���� �� Ŭ���� �Ѵ�
							//st_picker[m_nRobotSite].st_pcb_info[y].nYesNo = CTL_YES;					//��Ŀ ���� ��
							//st_tray_info[nWork_Site].st_pcb_info[m_npFirstPosYXP[0] + (y - m_n_FirstPicker_Num)][m_npFirstPosYXP[1]].nYesNo = CTL_NO; //Ʈ���� ���� Ŭ����
							//st_count_info.nSSD_Input[0]++;
						}
						else if(nStart_FixPos == TESTSITE_RIGHT) //RIGHT TEST SITE �۾��϶�, �ε����ۿ��� ��Ŀ������ 0,1,2,3 �̰�, ���� ����������0,1,2,3 �� ��ġ ������ �����ϴ� 
						{
							nFind_JobPos = m_nFirst_Buff_Num + ((nPicker_x - m_nFirstPicker_Num) * 2);

							//�Ʒ� �Լ����� ������� ��ġ ������ �����Ѵ�
							m_npSet_WorkPosYXCPB[0]  = nFind_JobPos; //����sms Y�� ��� //������ ��ġ ����
							m_npSet_WorkPosYXCPB[1]  = nFind_JobPos; //qjvjsms X�� ������� ���� //������ ��ġ ����
							m_npSet_WorkPosYXCPB[3]  = i; //picker ��ġ ����

							clsFunc.Data_Exchange_PickPlace(PICKER_PLACE_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //��Ŀ �� Ʈ���� ���� ���� �Լ� 
						}
					}
				} 
			}
		} 

		if (nRet_1 == RET_GOOD) // 1000 step���� ������ ��Ŀ ������ �°� ��� ������ ������ ��� ��
		{
			//ȭ�� ���� ����

			m_nDvcRepickCount = 0;
			m_nPartialPickCount = 0;

			m_bDvcWaitChk_Falg = false;

			// jtkim 20150216 conveyor/loader tray ȭ�� ǥ��
			if (st_handler_info.cWndMain != NULL)
			{
				st_handler_info.cWndMain->PostMessage(WM_WORK_DISPLAY, MAIN_LD_PICKER_DISPLAY, 0);
				st_handler_info.cWndMain->PostMessage(WM_WORK_DISPLAY, MAIN_LD_TRAY_DISPLAY, 0);
				st_handler_info.cWndMain->PostMessage(WM_WORK_DISPLAY, MAIN_RETEST_BUF_DISPLAY, 0);
			}
			m_nSortPlace_Step = 6000;
		}
		else if (nRet_1 == RET_ERROR) //������ ��Ŀ�� �Ѱ��� ����� ���� �������� �̰����� �̵�
		{
			CTL_Lib.Alarm_Error_Occurrence(2700, dWARNING, clsFunc.m_strAlarmCode);
			m_nSortPlace_Step = 0;
		}
		break;

	case 6000:
		nFuncRet = RET_GOOD;
		m_nSortPlace_Step = 0;
		break;

	default : 
		break;
		
	}

	return nFuncRet;

}//james 2016.0814 




int CRunTestSiteRobot::Chk_TestSiteRobotAllGlipper( int nOnOff )
{
	if( nOnOff == IO_OFF)
	{
		if( FAS_IO.get_in_bit(st_io_info.i_TestRbtPickerGripDvcExistChk[0],	IO_OFF)	== IO_OFF &&
			FAS_IO.get_in_bit(st_io_info.i_TestRbtPickerGripDvcExistChk[1],	IO_OFF)	== IO_OFF &&
			FAS_IO.get_in_bit(st_io_info.i_TestRbtPickerGripDvcExistChk[2],	IO_OFF)	== IO_OFF &&
			FAS_IO.get_in_bit(st_io_info.i_TestRbtPickerGripDvcExistChk[3],	IO_OFF)	== IO_OFF )
		{
			
			return RET_GOOD;
		}
		else
		{
			return RET_ERROR;
		}
	}
	else
	{
		if( FAS_IO.get_in_bit(st_io_info.i_TestRbtPickerGripDvcExistChk[0],	IO_ON) == IO_ON &&
			FAS_IO.get_in_bit(st_io_info.i_TestRbtPickerGripDvcExistChk[1],	IO_ON) == IO_ON &&
			FAS_IO.get_in_bit(st_io_info.i_TestRbtPickerGripDvcExistChk[2],	IO_ON) == IO_ON &&
			FAS_IO.get_in_bit(st_io_info.i_TestRbtPickerGripDvcExistChk[3],	IO_ON) == IO_ON )
		{
			
			return RET_GOOD;
		}
		else
		{
			return RET_ERROR;
		}
	}
	

	return RET_ERROR;
}

void CRunTestSiteRobot::Set_TestSiteRobotGlipper_OnOff( int *npPickerInfo, int nOnOff )
{
	int i;
	int nNum = TEST_PICKER_PARA; 
	CString strLogKey[10];
	CString	strLogData[10];

	strLogKey[0] = _T("Mode Start");
	strLogData[0].Format(_T("%d"),0);

	for (i=0; i<nNum; i++)
	{
		if (npPickerInfo[i] == YES)
		{
// 			FAS_IO.set_out_bit(st_io_info.o_TestRbtPickerGripCyl[i], nOnOff);
// 			m_bGlipperOnOffFlag	= false;
// 			m_dwGlipperOnOff[0]	= GetCurrentTime();
			//2016..128
			/*if( Opt == 1 )//LDBUFF-> RETESTBUFFER
			{
				if( st_picker[THD_TEST_RBT].st_pcb_info[i].nBin == BD_DATA_RETEST )
				{
					FAS_IO.set_out_bit(st_io_info.o_TestRbtPickerGripCyl[i], nOnOff);
					m_bGlipperOnOffFlag	= false;
					m_dwGlipperOnOff[0]	= GetCurrentTime();
				}
			}
			else
			{*/
				FAS_IO.set_out_bit(st_io_info.o_TestRbtPickerGripCyl[i], nOnOff);
				m_bGlipperOnOffFlag	= false;
				m_dwGlipperOnOff[0]	= GetCurrentTime();
			//}
		}
	}
	///kwlee 2016.0812
	if (nOnOff == IO_ON)
	{
		//LogFunction(_T("TSITE_ROBOT"),_T("FORWARD"),_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("TSITE_ROBOT"),_T("FORWARD"),0,_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
	else
	{
		//LogFunction(_T("TSITE_ROBOT"),_T("BACKWARD"),_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("TSITE_ROBOT"),_T("BACKWARD"),0,_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
	/* james 2016.0807 
	m_bGlipperOnOffFlag	= false;
	m_dwGlipperOnOff[0]	= GetCurrentTime();
	FAS_IO.set_out_bit( st_io_info.o_TestRbtPickerGripCyl[npPickerInfo], nOnOff);
	*/
}

int CRunTestSiteRobot::Chk_TestSiteRobotGlipper_OnOff( int *npPickerInfo, int nOnOff )
{//james 2016.0827 �׸��� ����̽� üũ ���� ����/����, Gripper Off(Ŭ��������)���� IO_OFF�� ����̽� �ִ»���, �׿ܴ� ON/OFF ������� ��� ����̽� ���� ����

	
//	802508 1 0 "TESTER_ROBOT_PICKER1_GLIPPER_DVC_EXIST_CHECK"
//	802509 1 0 "TESTER_ROBOT_PICKER2_GLIPPER_DVC_EXIST_CHECK"
//	802510 1 0 "TESTER_ROBOT_PICKER3_GLIPPER_DVC_EXIST_CHECK"
//	802511 1 0 "TESTER_ROBOT_PICKER4_GLIPPER_DVC_EXIST_CHECK"

	CString strLogKey[10];
	CString	strLogData[10];

	int i=0;
	int nRet[4]		= {NO, NO, NO, NO};
	int nNum		= TEST_PICKER_PARA;
	int nWaitTime	= WAIT_RBTGLIPPER_ON_OFF;

	strLogKey[0] = _T("Mode End");
	strLogData[0].Format(_T("%d"),0);

	///* james �� �Լ��� �߸��Ǿ���. ���߿� �ٽ��ؾ� �� 
	

	for (i=0; i<nNum; i++)
	{
		if (npPickerInfo[i] == YES)
		{				
			if (nOnOff == IO_OFF)
			{
				if (m_bGlipperOnOffFlag == false )//&&					FAS_IO.get_in_bit(st_io_info.i_TestRbtPickerGripDvcExistChk[i],	IO_OFF)	== IO_OFF )
				{
					m_bGlipperOnOffFlag		= true;
					m_dwGlipperOnOff[0]	= GetCurrentTime();
				}
				else if (m_bGlipperOnOffFlag == true ) //&&						 FAS_IO.get_in_bit(st_io_info.i_TestRbtPickerGripDvcExistChk[i], IO_OFF) == IO_OFF )
				{
					m_dwGlipperOnOff[1] = GetCurrentTime();
					m_dwGlipperOnOff[2] = m_dwGlipperOnOff[1] - m_dwGlipperOnOff[0];

					if (m_dwGlipperOnOff[2] <= 0)
					{
						m_dwGlipperOnOff[0] = GetCurrentTime();
						return RET_PROCEED;
					}

					if (m_dwGlipperOnOff[2] > (DWORD)st_wait_info.nOffWaitTime[nWaitTime])
					{
						//LogFunction(_T("TSITE_ROBOT"),_T("BACKWARD"),_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
						//2017.0109
						clsLog.LogFunction(_T("TSITE_ROBOT"),_T("BACKWARD"),1,_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
						nRet[i] = YES;
					}
				}
				else
				{
					m_dwGlipperOnOff[1] = GetCurrentTime();
					m_dwGlipperOnOff[2] = m_dwGlipperOnOff[1] - m_dwGlipperOnOff[0];

					if (m_dwGlipperOnOff[2] <= 0)
					{
						m_dwGlipperOnOff[0] = GetCurrentTime();
						return RET_PROCEED;
					}

					if (m_dwGlipperOnOff[2] > (DWORD)st_wait_info.nLimitWaitTime[nWaitTime])
					{
						m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_TestRbtPickerGripDvcExistChk[i]); 
						//2017.0109
						clsLog.LogFunction(_T("TSITE_ROBOT"),_T("BACKWARD"),1,_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
						return RET_ERROR;
					}
				}
			}
			else //if(nOnOff == IO_ON)
			{
				if (m_bGlipperOnOffFlag == false ) //&&					FAS_IO.get_in_bit(st_io_info.i_TestRbtPickerGripDvcExistChk[i],	IO_ON)	== IO_ON )
				{
					m_bGlipperOnOffFlag			= true;
					m_dwGlipperOnOff[0]	= GetCurrentTime();
				}
				else if (m_bGlipperOnOffFlag == true ) //&&						 FAS_IO.get_in_bit(st_io_info.i_TestRbtPickerGripDvcExistChk[i], IO_ON)	== IO_ON )
				{
					m_dwGlipperOnOff[1]	= GetCurrentTime();
					m_dwGlipperOnOff[2]	= m_dwGlipperOnOff[1] - m_dwGlipperOnOff[0];

					if (m_dwGlipperOnOff[2] <= 0)
					{
						m_dwGlipperOnOff[0]	= GetCurrentTime();
						return RET_PROCEED;
					}

					if(m_dwGlipperOnOff[2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
					{
						//LogFunction(_T("TSITE_ROBOT"),_T("FORWARD"),_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
						//2017.0109
						clsLog.LogFunction(_T("TSITE_ROBOT"),_T("FORWARD"),1,_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
						nRet[i] = YES;
					}
				}
				else
				{
					m_dwGlipperOnOff[1]	= GetCurrentTime();
					m_dwGlipperOnOff[2]	= m_dwGlipperOnOff[1] - m_dwGlipperOnOff[0];

					if (m_dwGlipperOnOff[2] <= 0)
					{
						m_dwGlipperOnOff[0]	= GetCurrentTime();
						return RET_PROCEED;
					}

					if (m_dwGlipperOnOff[2] > (DWORD)st_wait_info.nLimitWaitTime[nWaitTime])
					{
						m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_TestRbtPickerGripDvcExistChk[i]); 
						//2017.0109
						clsLog.LogFunction(_T("TSITE_ROBOT"),_T("FORWARD"),1,_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
						return RET_ERROR;
					}
				}
			}			
		}
		else
		{
			nRet[i] = YES;
		}
	}
	if (nRet[0] == YES && nRet[1] == YES && nRet[2] == YES && nRet[3] == YES) return RET_GOOD;


	return RET_PROCEED;
}