// RunDvcLdBuffer.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Handler.h"
#include "RunDvcLdBuffer.h"


#include "Variable.h"
#include "FastechPublic_IO.h"
#include "CtlBd_Library.h"

#include "ComizoaPublic.h"
#include "CtlBd_Variable.h"
#include "PublicFunction.h"
#include "LogCtrl.h"
#include "LogFromat.h"
//kwlee 2017.0905
#include "XgemClient.h"

// CRunDvcLdBuffer
CRunDvcLdBuffer clsRunDvcLdBuffer;
CRunDvcLdBuffer::CRunDvcLdBuffer()
{
	m_nRunStep	= -1;
	m_nInitStep	= -1;


	//linear move ���� ����
	m_nLinearMove_Index	= 1;   //���� map index
	m_lAxisCnt	= 2;            //������ IndexNum���� ��� ������ ���� ���� �ִ� 4��
	m_lpAxisNum[0] = M_LD_BUF_SHIFTER_Y; //m_lpAxisNum[4];        //������ IndexNum���� ����ϴ� ���� ���� ��ȣ�� ������
	m_lpAxisNum[1] = M_LD_BUF_ROTATOR;


	m_nAxisNum_Y = M_LD_BUF_SHIFTER_Y;
	m_nAxisNum_R = M_LD_BUF_ROTATOR;
}

CRunDvcLdBuffer::~CRunDvcLdBuffer()
{
}


// CRunDvcLdBuffer ��� �Լ�
void CRunDvcLdBuffer::OnThreadRun()
{	 
	if (st_handler_info.nSystemLock != FALSE)  return;	// ���� �ý��� Lock ������ ��� ����

	//���� CTL_Lib.Process_Tray_Conveyor_AcMotor(0, THD_EMPTY_STACKER, st_io_info.o_LdStkAcMotOnOff ); //load elevator stacker AC Motor �����ϴ� ��ƾ	

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


void CRunDvcLdBuffer::OnRunInit()
{
	int nRet_1, nRet_2=0;
	int nIO[4]={0,};

	if (st_sync_info.nInit_Flag[THD_LD_BUFF] != INIT_CLEAR)		return;		//INIT_CLEAR �϶��� �ʱ�ȭ �۾��� �Ѵ�. �ʱ�ȭ�� ������ INIT_CLEAR -> INIT_READY�� �Ǳ� ������...
	 
	switch (m_nInitStep)
	{
		case -1:
			 
 			if(st_sync_info.nInit_Flag[THD_WORK_RBT] < INIT_READY || st_sync_info.nInit_Flag[THD_TEST_RBT] < INIT_READY) break;
//			if(st_sync_info.nInit_Flag[M_WORK_RBT_Z] < INIT_READY/* || st_sync_info.nInit_Flag[M_TEST_RBT_Z] < INIT_READY*/) break;

			m_nInitStep = 0;
			break;

		case 0:		

			m_nInitStep = 100;
			break;

		case 100:
			nRet_1 = COMI.HomeCheck_Mot(m_nAxisNum_Y, MOT_SENS_ELM, MOT_TIMEOUT) ; //
			if(nRet_1 == BD_GOOD)
			{
				COMI.mn_homechk_flag[m_nAxisNum_Y] = CTL_YES;
				m_nInitStep = 200;
			}
			else if(nRet_1 == BD_ERROR)
			{
				CTL_Lib.mn_run_status = CTL_dWARNING;
				CTL_Lib.Alarm_Error_Occurrence(3000, CTL_Lib.mn_run_status, COMI.strCode);
				m_nInitStep = 100;
			}			
			break;

		case 200:
			nRet_1 = COMI.HomeCheck_Mot(m_nAxisNum_R, MOT_SENS_ELM, MOT_TIMEOUT) ; //
			if(nRet_1 == BD_GOOD)
			{
				COMI.mn_homechk_flag[m_nAxisNum_R] = CTL_YES;
				m_nInitStep = 300;
			}
			else if(nRet_1 == BD_ERROR)
			{
				CTL_Lib.mn_run_status = CTL_dWARNING;
				CTL_Lib.Alarm_Error_Occurrence(3010, CTL_Lib.mn_run_status, COMI.strCode);
				m_nInitStep = 100;
			}		
			break;
			//kwlee 2016.1215 Gripper ��� ���� ���� ����.
		case 300:
			for (int i=0; i<4; i++)
			{
				nIO[i] = IO_ON;
			}
			Set_Gripper_OnOff(0, IO_ON, nIO);
			m_nInitStep = 310;
			break;

		case 310:
			for (int i=0; i<4; i++)
			{
				nIO[i] = IO_ON;
			}
			nRet_1 = Chk_Gripper_OnOff(0, IO_ON, nIO);

			if(nRet_1 == RET_GOOD)
			{
				m_nInitStep = 320;
			}
			else if(nRet_1 == RET_ERROR)
			{			 
				CTL_Lib.Alarm_Error_Occurrence(3520, dWARNING, m_strAlarmCode);
				m_nInitStep = 300;
			}
			break;
			//////
		//case 300:
		  case 320:
			m_nInitStep = 900;
			break;


		case 900:
			st_sync_info.nInit_Flag[THD_LD_BUFF] = INIT_READY; 
			//2016.1114
			st_sync_info.mn_lduld_rbt = CTL_NO;

			m_nInitStep = 0;
			break;	

		default :
			break;

	}
}

void CRunDvcLdBuffer::OnRunMove()
{
	int nRet_1, nRet_2=0, nRet_3=0;
	int nCount = 0, i, nPos=0, nTestSocket_0_4_Start_Pos=0;

	clsFunc.OnThreadFunctionStepTrace(1, m_nRunStep);	

	switch (m_nRunStep)
	{ 
		case -1: 

			m_nRunStep = 0;
			break;

		case 0:
			if(st_lot_info[LOT_CURR].nLotStatus == LOT_START || st_lot_info[LOT_NEXT].nLotStatus == LOT_START)
			{
				//james 2016.0926 m_nRunStep = 100;
				if(st_lot_info[LOT_CURR].nLotStatus == LOT_START)
				{
					m_nLotProcessNum = LOT_CURR;
					m_nRunStep = 100;
				}
				else if(st_lot_info[LOT_NEXT].nLotStatus == LOT_START)
				{
					if(st_lot_info[LOT_NEXT].nLotNoApply_FirstTestStart == LOT_START) //james 2016.0926 test site�� �ϳ��̻� Next LotNo�� ���õǾ����� �������� 
					{			
						m_nLotProcessNum = LOT_NEXT;
						m_nRunStep = 100;
					}
					else
					{
						break;
					}
				}				
			} 
			break;

		case 100:
			st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_CLEAR; //clear james 2016.0926 

			m_nRunStep = 200;
			break;

		case 200:
		

			m_dpTargetPosList[0] = st_motor_info[m_nAxisNum_Y].d_pos[P_LDBUFF_Y_WORKRBT];
			m_dpTargetPosList[1] = st_motor_info[m_nAxisNum_R].d_pos[P_LDBUFF_RTR_WORKRBT];
			//nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate);
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nAxisNum_Y, m_dpTargetPosList[0], COMI.mn_runspeed_rate);
			if(nRet_1 == BD_GOOD) //���������� �Ϸ�� ����
			{
				m_nRunStep = 300;
			}
			else if(nRet_1 == BD_ERROR)
			{
				m_nRunStep = 200;
				CTL_Lib.Alarm_Error_Occurrence(3020, dWARNING, st_alarm_info.strCode);
			}
			break;

		case 300:
			for(i = 0; i < 4; i++)
			{
				m_npBuff_Info[i] = CTL_YES;			 
			}
			Set_Gripper_OnOff(0, IO_ON, m_npBuff_Info); //open

			for(i = 0; i < 4; i++)
			{
				st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nYesNo = CTL_NO;
				m_npBuff_Info[i] = CTL_NO;			 
			}
			m_nRunStep = 900;
			break;

		case 900:
			
			m_nRunStep = 1000;
			break;

		////////////////////////////////////////////////////////////////////////////
		//1. �ε� ���۴� �׽�Ʈ�κ�Ʈ�� ��û�� �����ϸ鼭 �����δ� 
		//2. Work RObot ����Ʈ�� �̵��Ͽ� �ε� ���縦 ��û
		//3. �ε� ���縦 ��� �غ��Ͽ� �׽�Ʈ�κ� ����Ʈ�� ���۸� ������
		//   �̋� ���۴� �׽�Ʈ�κ��� �۾��� �׽�Ʈ����Ʈ�� ������ �°� ��û�� �������� ���縦 �����ؾ� �Ѵ�
		////////////////////////////////////////////////////////////////////////////
		case 1000:		
			//break;
/*
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//james ���ǰ���  �׽�Ʈ�� ���� ���� �����ϴ°� ���� ////////////////////////
			//////////////////////////////////////////////////////////////////////////////////
			//�׽�Ʈ ����Ʈ�� ���縦 ��ε� �ϴ�, �̰��� ���� �ε� ������ ���縦 �̸� ��û 
			//////////////////////////////////////////////////////////////////////////////////
			nTestSocket_0_4_Start_Pos = 0;
			st_sync_info.nTestSite_Target_THD_Work_Site = THD_TESTSITE_1;
			st_find_testsite_info[st_sync_info.nTestSite_Target_THD_Work_Site].nStart_FixPos = 0;
			st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].nSite_Pos = TESTSITE_LEFT;
			st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].nStart_FixPos = 0;
			//TestSite_PickPlace_SocketData_LoadBuff_Map_Combine(0, st_sync_info.nTestSite_Target_THD_Work_Site, st_find_testsite_info[st_sync_info.nTestSite_Target_THD_Work_Site].nStart_FixPos); //���� ��ġ�� �۾� ���ɿ��� ����,��Ŀ�� ��� ��/�� ���� ����
			for(i=0; i< 4; i++) //loading ���� ���� 4�� 
			{
				if(st_sync_info.nTestSite_Target_THD_Work_Site >= THD_TESTSITE_1 && st_sync_info.nTestSite_Target_THD_Work_Site <= THD_TESTSITE_4) //���� �׽��� ,���� ������ ������ġ ���� (1ȸ�۾�)7,6,5,4, (2ȸ�۾�)3,2,1,0 �� ������ ���� ���� ��ġ�� ���� ���ǵ�
				{
					nPos = (7 - nTestSocket_0_4_Start_Pos) - i;
					if(st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].st_pcb_info[nPos].nEnable == CTL_YES || 1) //socket ON
					{
						st_buffer_info[THD_LD_BUFF].st_pcb_info[3 - i].nEnable = CTL_YES;
						st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable = CTL_YES;

						st_buffer_info[THD_LD_BUFF].st_pcb_info[3 - i].nYesNo = CTL_NO;
					}
					else //if(st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].st_pcb_info[nPos].nEnable  == CTL_NO)//Socket OFF
					{
						st_buffer_info[THD_LD_BUFF].st_pcb_info[3 - i].nEnable = CTL_NO;
						st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable = CTL_NO;
					}
				}

				m_npBuff_Info[i] = CTL_YES;			
			}
			Set_Gripper_OnOff(0, IO_ON, m_npBuff_Info);

			st_buffer_info[THD_LD_BUFF].nTestSIte_No = st_sync_info.nTestSite_Target_THD_Work_Site;
			st_buffer_info[THD_LD_BUFF].nTestSite_LR_Pos = st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //�ε� ������ �׽��Ϳ� �´� ȸ�� ������ ���Ѵ�
			st_buffer_info[THD_LD_BUFF].nTestSite_Start_FixPos = st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].nStart_FixPos;

			st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_REQ;
			st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][1] = WORK_PICK; //���縦 ���� �� �ְ� ��û 
			st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][2] = st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //�ε� ������ �׽��Ϳ� �´� ȸ�� ������ ���Ѵ�
//james ���ǰ���  �׽�Ʈ�� ���� ���� �����ϴ°� �� ////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/
			//Test Robot�� ������ ������.
			if(st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_REQ && st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][1] == WORK_PICK)  
			{//st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][2] �� ������ ������ ȸ�� ������ �����Ѵ�
			 //�׽�Ʈ����Ʈ�� Ȯ���Ͽ� �ε� ���� ����� ������ ���� ������ �� �� ������ ��û�Ѵ� 

				if(st_lot_info[LOT_CURR].nLotStatus >= LOT_END_START && st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)//jame 2016.0924 
				{//james 2016.0924 ���� �߰� 
//					if(THD_TESTSITE_1 <= st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][4] && THD_TESTSITE_4 >= st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][4])
					//2016.1110
					if(THD_TESTSITE_1 <= st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][4] && THD_TESTSITE_8 >= st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][4])
					{
						if(st_lot_info[LOT_NEXT].strLotNo == st_test_site_info[st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][4]].strLotNo)
						{
							m_nRunStep = 1100; 
						}
					}
					else
					{
						break;
					}
				}
				else
				{
					m_nRunStep = 1100; 
				}
				//james 2016.0924 m_nRunStep = 1100; 
			}
			else if(st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_REQ && st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][1] == WORK_PLACE) 
			{//james 2016.0926 ������� ���� 
				//�ε� ���ۿ� �׽�Ʈ�κ��� ���� ���������� �����ִ� ���縦 ���� �� �ְ� ��û 
			    //st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][2] �� ������ �׽�Ʈ ����Ʈ�� left ���� right������ ������ ������ ȸ�� ������ �����Ѵ�
				m_nRunStep = 1200; 
			}
			else 
			{
				if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END_START && st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_BUFF] <  LOT_END_START)
				{
					st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_BUFF] =  LOT_END_START;
					m_nRunStep = 0;
				}
			}
			break;

		///////////////////////////////////////////////////////////
		// �ε� ���縦 ���ۿ� �غ��Ͽ� �׽�Ʈ ����Ʈ�� ���۸� ������
		///////////////////////////////////////////////////////////
		case 1100:
			nRet_1 = clsFunc.Check_BufferStatus(100, THD_LD_BUFF, CTL_NO, m_npBuff_Info, m_npBuff_Status, m_npBuff_OutputStatus); //���۴� ������ ����� �־�� �Ѵ� 
			if(nRet_1 == RET_GOOD)
			{
				//���� ����, ó������ work robot ����Ʈ�� �̵��ϴ� ��ƾ����, �ε� ���ۿ��� ���簡 �ϳ��� ����� �Ѵ� 
				 m_nRunStep = 2000; 
			}
			else if(nRet_1 == RET_ERROR)
			{//���簡 �����ִٸ�, ���� ���������� �����ִ� �����̴� �̋��� �ך� �κ��� ��û�� ��� �ٷ� ���� �� �ְ� ó���Ѵ�  

				m_nRunStep = 3000; 
				 //CTL_Lib.Alarm_Error_Occurrence(m_nRunStep, dWARNING, clsFunc.m_strAlarmCode);
			}
			break;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// �ε� ���ۿ� �׽�Ʈ�κ��� ���� ���������� �����ִ� ���縦 ���� �� �ְ� ��û 
		// �۾��� ���Ͽ� �����ִ� ���縦 ���� �� �������� �� ��Ŀ�� ����� �ε� ���ۿ� ���� �̵��Ѵ� 
		// �ε� ���ۿ� ���縦 ���� ������, ���ۿ��� �߰� �۾��� ��û���� �ʰ� ������ �ͼ� ���� ���縦 ��� ���� �� �ε� ���ۿ� ���� �۾��� ��û�Ѵ� 
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		case 1200: //�̋� ���۴� ������ ����� �־�� �Ѵ� 
			nRet_1 = clsFunc.Check_BufferStatus(100, THD_LD_BUFF, CTL_NO, m_npBuff_Info, m_npBuff_Status, m_npBuff_OutputStatus); //���۴� ������ ����� �־�� �Ѵ� 
			if(nRet_1 == RET_GOOD)
			{
				//���� ����, 
					m_nRunStep = 6000; //test robot�� �����ִ� �ε� ���� ���� ���縦 ���� �����̴�. �� ���縦 ���� �׽�Ʈ ����Ʈ�� �̵��Ͽ� �۾��Ѵ�
			}
			else if(nRet_1 == RET_ERROR)
			{//���簡 �����ִٸ� ���� 
					CTL_Lib.Alarm_Error_Occurrence(3100, dWARNING, clsFunc.m_strAlarmCode);
			}			 
			break;


		////////////////////////////////////////////////////
		// Work Robot Site�� �̵��Ѵ� 
		// �̵��� ���ۿ��� ���簡 �ϳ��� ���� ������ �ȵȴ� 
		// �ε� ���縦 �� ���� ������ 2000 step�� �ݺ� �����Ѵ� 
		///////////////////////////////////////////////////
		case 2000:	
			m_dpTargetPosList[0] = st_motor_info[m_nAxisNum_Y].d_pos[P_LDBUFF_Y_WORKRBT];
			m_dpTargetPosList[1] = st_motor_info[m_nAxisNum_R].d_pos[P_LDBUFF_RTR_WORKRBT];

			nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate);
			// jtkim 20160831
			//nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nAxisNum_Y, m_dpTargetPosList[0], COMI.mn_runspeed_rate);
			if(nRet_1 == BD_GOOD) //���������� �Ϸ�� ����
			{
				m_nRunStep = 2100;
			}
			else if(nRet_1 == BD_ERROR)
			{
				m_nRunStep = 2000;
				CTL_Lib.Alarm_Error_Occurrence(3110, dWARNING, st_alarm_info.strCode);
			}
			break;

		case 2100: //st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][2] == �׽�Ʈ ����Ʈ���� �۾��� ��ġ ����(Test left or test right
			nRet_1 = clsFunc.Find_TrayWork_Pos(WORK_PLACE, THD_LD_BUFF, CTL_NO, m_npFindWorkPosYXCPB, THD_WORK_RBT, st_worker_info.strLotNo_Empty);
			if(nRet_1 == RET_GOOD) //�۾� ������ ���簡 �ִ�, ���� �۾��� �ڿ���(0��) ���� ���´� 
			{ //work robot�� �ε� ���ۿ� ���縦 �������� 0->1->2->3 ������ �۾�(���� �ڿ��� ���� 1���̴�)
				m_nWork_Pos = m_npFindWorkPosYXCPB[0]; 
			}
			else if(nRet_1 == RET_SKIP) //���̻� ���� ������ ���� 
			{	//��� ������ �� ����
				m_nRunStep = 1000;	 
				break;
			}

			//work robot�� ���� ������ ��û 
//			st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_REQ; //�ε� ���� ��û
//			st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][1] = WORK_PLACE; //���� ���� �۾�
//			//work robot���� ������ Ȯ���Ͽ� �˾Ƽ� �۾��Ѵ� st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][2] = m_nWork_Pos; //�۾��� ���� ��ġ 
//			m_nRunStep = 2200;
			//2016.1205
			if(st_sync_info.mn_lduld_rbt == CTL_YES && 
				st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END_START && st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_BUFF] <  LOT_END_START)
			{
				st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_BUFF] =  LOT_END_START;
				m_nRunStep = 0;
			}
			else
			{
				//work robot�� ���� ������ ��û 
				st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_REQ; //�ε� ���� ��û
				st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][1] = WORK_PLACE; //���� ���� �۾�
				//work robot���� ������ Ȯ���Ͽ� �˾Ƽ� �۾��Ѵ� st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][2] = m_nWork_Pos; //�۾��� ���� ��ġ 
				m_nRunStep = 2200;
			}
	
			break;

		case 2200:
			if(st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_READY)//���縦 �κ�Ʈ�� �ε����ۿ� ��� ä�� ���� ����
			{//work robot�� ������ ���۸� ��� ä��� CTL_READY �� �ش� 
				nCount = 0;
				for(i = 0; i < st_recipe_info.nLdUldBuffer_Num; i++) //work robot�� �ε� ���ۿ� 0,1,2,3 ������ �۾�
				{ //������ ���� ������ �����Ѵ�
					m_npBuff_Info[i] = st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nYesNo; //������ ���� ������ �����Ѵ� 
					nCount++;

					//kwlee 2017.0905
					if (st_basic_info.nModeXgem == YES)
					{
						if (st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nYesNo == YES)
						{
							clsXgem.OnMcTrayMoveLDBuffer(START,st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nTrayCnt,st_recipe_info.nTrayY,st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nProductCnt,st_buffer_info[THD_LD_BUFF].st_pcb_info[i].strCSerialNo);
						}
					}
				}

				if(nCount > 0)
				{
					m_nRunStep = 2300;

					/////////////////////////////////////////////////////////////////////////////////////////////////
					//2016.1203
					m_strLotNo = st_lot_info[m_nLotProcessNum].strLotNo;
					m_strPartNo = st_lot_info[m_nLotProcessNum].strPartNo;

					m_dwCycleTime[1] = GetTickCount();
					m_dwCycleTime[2] = m_dwCycleTime[1] - m_dwCycleTime[0];

					if (m_dwCycleTime[2] <= (DWORD)0)
					{
						m_dwCycleTime[0] = GetTickCount();
					}
					else
					{
						if( m_strLotNo != _T("") && m_strPartNo != _T("") )
						{
							clsFunc.OnCycleTime(1, 
												m_strLotNo,
												m_strPartNo,
												m_dwCycleTime[2], 
												0, 
												0);
						}

						m_dwCycleTime[0] = GetTickCount();
					}
					/////////////////////////////////////////////////////////////////////////////////////////////////

					//2017.0109
					st_buffer_info[THD_LD_BUFF].nLogCount = 0;
					st_buffer_info[THD_LD_BUFF].strMaterial = _T(""); 
					for (i=0; i<4; i++)
					{
						if (st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nYesNo == YES)
						{
							st_buffer_info[THD_LD_BUFF].strMaterial = st_buffer_info[THD_LD_BUFF].st_pcb_info[i].strSerialNo;
							break;
						}
					}
					for (i=0; i<4; i++)
					{
						if (st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nYesNo == YES)
						{
							st_buffer_info[THD_LD_BUFF].strLogKey[st_buffer_info[THD_LD_BUFF].nLogCount].Format(_T("%02d"), i + 1);
							st_buffer_info[THD_LD_BUFF].strLogData[st_buffer_info[THD_LD_BUFF].nLogCount] = st_buffer_info[THD_LD_BUFF].st_pcb_info[i].strSerialNo;
							st_buffer_info[THD_LD_BUFF].nLogCount++;
						}						
					}
					
					clsLog.LogTransfer(_T("LOADING_BUFFER"), 
									   _T("MOVE_BUFFER"), 
									   0, 
									   st_buffer_info[THD_LD_BUFF].strMaterial,
									   _T("CDIMM"), 
									   _T("BUFFER"), 
									   _T("TEST_SITE"),
									   st_buffer_info[THD_LD_BUFF].nLogCount,
									   st_buffer_info[THD_LD_BUFF].strLogKey,
									   st_buffer_info[THD_LD_BUFF].strLogData);

				}
				else
				{//���簡 �ϳ��� ���� ���� 
					if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END_START)
					{
						st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_BUFF] =  LOT_END_START;
						m_nRunStep = 0;
					}
				}
			}
			else if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END_START)//(st_lot_info[LOT_CURR].nLot_THD_Status[THD_LD_TRAY_PLATE] ==  LOT_END) //�κ�Ʈ ��Ŀ�� �ε� ���絵 üũ�ϰ� lot end  ���� 
			{
				nCount = 0;
				for(i = 0; i < st_recipe_info.nLdUldBuffer_Num; i++) //work robot�� �ε� ���ۿ� 0,1,2,3 ������ �۾�
				{ //������ ���� ������ �����Ѵ�
					m_npBuff_Info[i] = st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nYesNo; //������ ���� ������ �����Ѵ� 
					//james 2016.0924 ���� �߰�  nCount++;
					if(st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nYesNo == CTL_YES &&
						st_buffer_info[THD_LD_BUFF].st_pcb_info[i].strLotNo == st_lot_info[m_nLotProcessNum].strLotNo)//james 2016.0924 ���� �߰� 
					{
						nCount++;
					}
				}

				if(nCount > 0)
				{
					//if(st_lot_info[LOT_NEXT].nLotStatus == LOT_START) //james 2016.0924 �߰� 
					//{
					//	break;
					//}

					//james 2016.0926 m_nRunStep = 2300;
				}
				else
				{//���簡 �ϳ��� ���� ���� 
					if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END_START && st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_BUFF] == LOT_START)
					{
						st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_BUFF] =  LOT_END_START;
						m_nRunStep = 0;
					}
				}
			}
			break;

		case 2300:
			nRet_1 = clsFunc.Check_BufferStatus(0, THD_LD_BUFF, CTL_YES, m_npBuff_Info, m_npBuff_Status, m_npBuff_OutputStatus); //���������� ���� ���°� ��ġ�ؾ� �Ѵ� 
			if(nRet_1 == RET_GOOD)
			{//���� ����, //���������� ���� ���°� ��ġ�ؾ� �Ѵ�
				m_nRunStep = 2400;
			}
			else if(nRet_1== RET_ERROR)
			{//���� 
				 CTL_Lib.Alarm_Error_Occurrence(3120, dWARNING, clsFunc.m_strAlarmCode);
			} 
			break;

		case 2400:		 
			m_nRunStep = 3000;
			break;

		

		////////////////////////////////////////////////////
		// Test Robot Site�� �̵��Ѵ� 
		// �̵��� ���ۿ��� ���簡 �ϳ� �̻� �־�� �ȴ� 
		// ���� ����/���� �ִ� ���� ���� ���絵 ���� �� �ִ� 
		///////////////////////////////////////////////////
		case 3000:
			m_dpTargetPosList[0] = st_motor_info[m_nAxisNum_Y].d_pos[P_LDBUFF_Y_TESTRBT];

			if(st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][2] == TESTSITE_LEFT)
			{
				m_dpTargetPosList[1] = st_motor_info[m_nAxisNum_R].d_pos[P_LDBUFF_RTR_TESTSITE_LEFT];
			}
			else if(st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][2] == TESTSITE_RIGHT)
			{
				m_dpTargetPosList[1] = st_motor_info[m_nAxisNum_R].d_pos[P_LDBUFF_RTR_TESTSITE_RIGHT];
			}
			nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate);
			if(nRet_1 == BD_GOOD) //���������� �Ϸ�� ����
			{
				//kwlee 2017.0905
				if (st_basic_info.nModeXgem == YES)
				{
					for (i = 0; i < st_recipe_info.nLdUldBuffer_Num; i++)
					{
						if (st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nYesNo == YES)
						{
							clsXgem.OnMcTrayMoveLDBuffer(END,st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nTrayCnt,st_recipe_info.nTrayY,st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nProductCnt,st_buffer_info[THD_LD_BUFF].st_pcb_info[i].strSerialNo );
						}
					}
				}
				///
				m_nRunStep = 3100;
			}
			else if(nRet_1 == BD_ERROR)
			{
				m_nRunStep = 3000;
				CTL_Lib.Alarm_Error_Occurrence(3130, dWARNING, st_alarm_info.strCode);
			}
			break;

		case 3100:
			nRet_1 = clsFunc.Check_BufferStatus(0, THD_LD_BUFF, CTL_YES, m_npBuff_Info, m_npBuff_Status, m_npBuff_OutputStatus); //���������� ���� ���°� ��ġ�ؾ� �Ѵ� 
			if(nRet_1 == RET_GOOD)
			{//���� ����, //Enable �Ǿ� �ִ� ���� ��ġ�� ��� ä���� ����, �׽�Ʈ �κ� ����Ʈ�� �̼۰����ϴ� 
				 //m_nRunStep = 3200;
				//2016.1114
				if( st_sync_info.mn_lduld_rbt == CTL_YES )
				{
					st_sync_info.mn_lduld_rbt = CTL_NO;
					m_nRunStep = 3110;
				}
				else
				{
					m_nRunStep = 3200;
				}

				//2017.0109
				clsLog.LogTransfer(_T("LOADING_BUFFER"), 
									_T("MOVE_BUFFER"), 
									1, 
									st_buffer_info[THD_LD_BUFF].strMaterial,
									_T("CDIMM"), 
									_T("BUFFER"), 
									_T("TEST_SITE"),
									st_buffer_info[THD_LD_BUFF].nLogCount,
									st_buffer_info[THD_LD_BUFF].strLogKey,
									st_buffer_info[THD_LD_BUFF].strLogData);
			}
			else if(nRet_1== RET_ERROR)
			{//���� ���� ä��� �۾��� ������ ���� ���� 
				 CTL_Lib.Alarm_Error_Occurrence(3140, dWARNING, clsFunc.m_strAlarmCode);
				 m_nRunStep = 3100;   
			} 
			break;

			//2016.1114
		case 3110:
			//Test Robot�� ������ ������.
			if(st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_REQ && st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][1] == WORK_PICK)  
			{
				if(st_lot_info[LOT_CURR].nLotStatus >= LOT_END_START && st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)
				{
					if(THD_TESTSITE_1 <= st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][4] && THD_TESTSITE_8 >= st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][4])
					{
						if(st_lot_info[LOT_NEXT].strLotNo == st_test_site_info[st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][4]].strLotNo)
						{
							m_nRunStep = 3200; 
						}
						else if(st_lot_info[LOT_CURR].strLotNo == st_test_site_info[st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][4]].strLotNo)
						{
							m_nRunStep = 3200;
						}
					}
					else
					{
						break;
					}
				}
				else
				{
					m_nRunStep = 3200; 
				}
			}
			else 
			{
				if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END_START && st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_BUFF] <  LOT_END_START)
				{
					//st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_BUFF] =  LOT_END_START;
					//m_nRunStep = 0;
					//2016.1121
					nRet_2 = CTL_ERROR;
					for ( i = 0 ; i < 4; i++ )
					{
// 						if( st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nYesNo == CTL_YES &&
// 							st_buffer_info[THD_LD_BUFF].st_pcb_info[i].strLotNo == st_lot_info[LOT_CURR].strLotNo )
// 						{
// 							nRet_2 = CTL_GOOD;
// 							break;
// 						}
						/////////////////////////////////////////////////////////
						//2016.1205
						if( st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nYesNo == CTL_YES &&
							st_buffer_info[THD_LD_BUFF].st_pcb_info[i].strLotNo == st_lot_info[m_nLotProcessNum].strLotNo )
						{
							nRet_2 = CTL_GOOD;
							break;
						}
						/////////////////////////////////////////////////////////
					}
					if( nRet_2 == RET_ERROR)
					{
						st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_BUFF] =  LOT_END_START;
						m_nRunStep = 0;
						
					}
				}
			}
			break;

		/////////////////////////////////////////////////////////////
		//�ε� ���۰� �غ�Ǿ� ������ test robot�� ���� ����� ��û
		/////////////////////////////////////////////////////////////
		case 3200:
			st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_READY; //test robot ���� ����� ��û
			m_nRunStep = 3300;
			break;

		case 3300:
			//st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_FREE; //james test�� ���, ���� ��븻�� 

			if(st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_FREE)//TEST ROBOT ��Ŀ�� ���縦 ��� ���� ����
			{
				//��Ŀ �۾� ������ ���ۿ� �����Ͽ� ���ÿ� ���� �۾��ϰ� �Ѵ� 
				st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_CLEAR;
				//m_nRunStep = 1000;
				if( st_sync_info.nAutoBufferIn == CTL_YES )//2016.1117
				{
					//m_nRunStep = 1000;
					//20161205
					if( st_basic_info.nAutoLoadMode == CTL_YES)
					{
						m_nRunStep = 3400;
					}
					else
					{
						st_sync_info.mn_lduld_rbt = CTL_NO;
						m_nRunStep = 1000;
					}
				}
				else
				{
					//m_nRunStep = 3400;
					//20161205
					if( st_basic_info.nAutoLoadMode == CTL_YES )
					{
						m_nRunStep = 3400;
					}
					else
					{
						st_sync_info.mn_lduld_rbt = CTL_NO;
						m_nRunStep = 1000;
					}
				}

				//2017.0109

			}
			else if(st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_REQ && st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][1] == 300) //jame 2016.1005 
			{
				st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][1] = WORK_PICK;
				//james 2016.1004 �ε� ���۴� �غ�Ǿ��ִµ�, �׽�Ʈ �κ��� ���û�� ���� ��ġ �� ����, ���� m_nRunStep = 3200;
				m_nRunStep = 3000;
			}
			else if(st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_REQ) //james 2016.1005 
			{
				m_nRunStep = 3200; // 
			}
			break; 

		case 3400:
			if( st_sync_info.nAutoBufferIn != CTL_YES ) break;
			//2016.1114
			nRet_1 = clsFunc.Check_BufferStatus(100, THD_LD_BUFF, CTL_NO, m_npBuff_Info, m_npBuff_Status, m_npBuff_OutputStatus);
			if(nRet_1 == RET_GOOD && st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END_START && st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_BUFF] <  LOT_END_START)
			{
				st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_BUFF] =  LOT_END_START;
				m_nRunStep = 0;
			}
			else if(nRet_1 == RET_GOOD)
			{
				if( st_basic_info.nAutoLoadMode == CTL_YES )
				{
					st_sync_info.mn_lduld_rbt = CTL_YES;
					m_nRunStep = 2000; 
				}
				else
				{
					st_sync_info.mn_lduld_rbt = CTL_NO;
					m_nRunStep = 1000;
				}
			}
//			else
//			{
//				m_nRunStep = 1000;
//			}
			break;



//james 2016.1005 
// 			else if(st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_REQ)
// 			{
// 				//james 2016.1004 �ε� ���۴� �غ�Ǿ��ִµ�, �׽�Ʈ �κ��� ���û�� ���� ��ġ �� ����, ���� m_nRunStep = 3200;
// 				m_nRunStep = 3200;
// 			}
//			break; 

// 		case 3310: //james 2016.1005 ������� ����. �׽�Ʈ �κ��� ������ ������ �浹 �߻� 
// 			m_dpTargetPosList[0] = st_motor_info[m_nAxisNum_Y].d_pos[P_LDBUFF_Y_TESTRBT];
// 
// 			if(st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][2] == TESTSITE_LEFT)
// 			{
// 				m_dpTargetPosList[1] = st_motor_info[m_nAxisNum_R].d_pos[P_LDBUFF_RTR_TESTSITE_LEFT];
// 			}
// 			else if(st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][2] == TESTSITE_RIGHT)
// 			{
// 				m_dpTargetPosList[1] = st_motor_info[m_nAxisNum_R].d_pos[P_LDBUFF_RTR_TESTSITE_RIGHT];
// 			}
// 			nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate);
// 			if(nRet_1 == BD_GOOD) //���������� �Ϸ�� ����
// 			{
// 				m_nRunStep = 3200;
// 			}
// 			else if(nRet_1 == BD_ERROR)
// 			{
// 				m_nRunStep = 3310;
// 				CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);
// 			}
// 			break;


		////////////////////////////////////////////////////
	    // test robot�� �����ִ� ���簡 �׽�Ʈ ���Ͽ��������� ó���Ҽ� ������ �ε� ���ۿ� ���� ���� 
		// Test Robot Site�� �̵��Ѵ� 
		// �̶� �ε� ���ۿ��� ���簡 �ϳ��� ����� �Ѵ� 
		///////////////////////////////////////////////////
		case 6000:
			m_dpTargetPosList[0] = st_motor_info[m_nAxisNum_Y].d_pos[P_LDBUFF_Y_TESTRBT];

			if(st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][2] == TESTSITE_LEFT)
			{
				m_dpTargetPosList[1] = st_motor_info[m_nAxisNum_R].d_pos[P_LDBUFF_RTR_TESTSITE_LEFT];
			}
			else if(st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][2] == TESTSITE_RIGHT)
			{
				m_dpTargetPosList[1] = st_motor_info[m_nAxisNum_R].d_pos[P_LDBUFF_RTR_TESTSITE_RIGHT];
			}
			//nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate);
			nRet_1 = BD_GOOD;
			if(nRet_1 == BD_GOOD) //���������� �Ϸ�� ����
			{
				m_nRunStep = 6100;
			}
			else if(nRet_1 == BD_ERROR)
			{
				m_nRunStep = 6000;
				CTL_Lib.Alarm_Error_Occurrence(3150, dWARNING, st_alarm_info.strCode);
			}
			break;

		case 6100:
			nRet_1 = clsFunc.Check_BufferStatus(100, THD_LD_BUFF, CTL_NO, m_npBuff_Info, m_npBuff_Status, m_npBuff_OutputStatus); // �ε� ���ۿ��� ���簡 �ϳ��� ����� �Ѵ� 
			if(nRet_1 == RET_GOOD)
			{//���� ����, //Enable �Ǿ� �ִ� ���� ��ġ�� ��� ä���� ����, �׽�Ʈ �κ� ����Ʈ�� �̼۰����ϴ� 
				 m_nRunStep = 6200;
			}
			else if(nRet_1== RET_ERROR)
			{//���� ���� ä��� �۾��� ������ ���� ���� 
				 CTL_Lib.Alarm_Error_Occurrence(3160, dWARNING, clsFunc.m_strAlarmCode);
				 m_nRunStep = 6100;   
			} 
			break;

		case 6200:
			st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_READY;  //���� ���縦 ���۰� �غ�Ǿ����� ���� 
			m_nRunStep = 6300;
			break;

		case 6300:
			if(st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_FREE)//test robot�� ������ ���� �������� ���� ���縦 ��� ���� ���� ���縣 �������� �׽�Ʈ �κ��� �ε����ۿ� �߰� �۾� ��û�� �ϸ� �ȵȴ�.
			{
				st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_CLEAR; 
				m_nRunStep = 6400;
			} 
			else if(st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_REQ)
			{
				 m_nRunStep = 6200;
			}
			break;

		case 6400: 
			m_nRunStep = 1000;
			
			break;
			///
	}
}



void CRunDvcLdBuffer::Set_Gripper_OnOff(int nMode, int nOnOff, int *npPickerInfo)
{
	int i;
	int nNum		= 4;
	int nRecovery	= RECO_LD_RBT_PICK_VAC_1;

	CString strLogKey[10];
	CString strLogData[10];
	
	strLogKey[0] = _T("Mode Start");
	strLogData[0].Format(_T("%d"),nMode);

	if (nMode > 1) return;	 

	for (i=0; i<nNum; i++)
	{
		if (npPickerInfo[i] == YES)
		{
			FAS_IO.set_out_bit(st_io_info.o_LdBuffGripOnOffCyl[i], nOnOff);
			m_bPickerFlag			= false;
			m_dwPickerWaitTime[0]	= GetCurrentTime();
		}
	}

	//kwlee 2016.0812
	if (nOnOff == IO_ON)
	{
		//LogFunction(_T("DVC_LD_BUFFER"),_T("FORWARD"),_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("DVC_LD_BUFFER"),_T("FORWARD"),0,_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
	else
	{
		//LogFunction(_T("DVC_LD_BUFFER"),_T("BACKWARD"),_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("DVC_LD_BUFFER"),_T("BACKWARD"),0,_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
}

int CRunDvcLdBuffer::Chk_Gripper_OnOff(int nMode, int nOnOff, int *npPickerInfo)
{

//	800712 1 0 "LOAD_UNLOAD_PICKER_GLIPPER_DVC_EXIST_CHECK"
	int i =0;
	int nNum		= 4;
	int nWaitTime = WAIT_RBTGLIPPER_ON_OFF;

	CString strLogKey[10];
	CString strLogData[10];
	strLogKey[0] = _T("Mode End");
	strLogData[0].Format(_T("%d"),nMode);

	if (nOnOff == IO_OFF)
	{
		if (m_bPickerFlag == false)
		{
			m_bPickerFlag		= true;
			m_dwPickerWaitTime[0]	= GetCurrentTime();
		}
		else if (m_bPickerFlag == true )
		{
			m_dwPickerWaitTime[1] = GetCurrentTime();
			m_dwPickerWaitTime[2] = m_dwPickerWaitTime[1] - m_dwPickerWaitTime[0];

			if (m_dwPickerWaitTime[2] <= 0)
			{
				m_dwPickerWaitTime[0] = GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwPickerWaitTime[2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
			{
				//LogFunction(_T("DVC_LDBUFFER"),_T("BACKWARD"),_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("DVC_LDBUFFER"),_T("BACKWARD"),1,_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_GOOD;
			}
		} 
	}
	else
	{
		if (m_bPickerFlag == false)
		{
			m_bPickerFlag			= true;
			m_dwPickerWaitTime[0]	= GetCurrentTime();
		}
		else if (m_bPickerFlag == true)
		{
			m_dwPickerWaitTime[1]	= GetCurrentTime();
			m_dwPickerWaitTime[2]	= m_dwPickerWaitTime[1] - m_dwPickerWaitTime[0];

			if (m_dwPickerWaitTime[2] <= 0)
			{
				m_dwPickerWaitTime[0]	= GetCurrentTime();
				return RET_PROCEED;
			}

			if(m_dwPickerWaitTime[2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
			{
				//LogFunction(_T("DVC_LDBUFFER"),_T("FORWARD"),_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
				clsLog.LogFunction(_T("DVC_LDBUFFER"),_T("FORWARD"),1,_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
				return RET_GOOD;
			}
		}
	}

	return RET_PROCEED;
}