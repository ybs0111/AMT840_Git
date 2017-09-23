// RunDvcLdBuffer.cpp : 구현 파일입니다.
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


	//linear move 과련 셋팅
	m_nLinearMove_Index	= 1;   //현재 map index
	m_lAxisCnt	= 2;            //현재의 IndexNum에서 사용 가능한 모터 수량 최대 4개
	m_lpAxisNum[0] = M_LD_BUF_SHIFTER_Y; //m_lpAxisNum[4];        //현재의 IndexNum에서 사용하는 모터 실재 번호를 가진다
	m_lpAxisNum[1] = M_LD_BUF_ROTATOR;


	m_nAxisNum_Y = M_LD_BUF_SHIFTER_Y;
	m_nAxisNum_R = M_LD_BUF_ROTATOR;
}

CRunDvcLdBuffer::~CRunDvcLdBuffer()
{
}


// CRunDvcLdBuffer 멤버 함수
void CRunDvcLdBuffer::OnThreadRun()
{	 
	if (st_handler_info.nSystemLock != FALSE)  return;	// 현재 시스템 Lock 상태인 경우 리턴

	//없음 CTL_Lib.Process_Tray_Conveyor_AcMotor(0, THD_EMPTY_STACKER, st_io_info.o_LdStkAcMotOnOff ); //load elevator stacker AC Motor 제어하는 루틴	

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

	if (st_sync_info.nInit_Flag[THD_LD_BUFF] != INIT_CLEAR)		return;		//INIT_CLEAR 일때만 초기화 작업을 한다. 초기화가 끝나면 INIT_CLEAR -> INIT_READY가 되기 떄문에...
	 
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
			//kwlee 2016.1215 Gripper 모두 벌리 도록 수정.
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
					if(st_lot_info[LOT_NEXT].nLotNoApply_FirstTestStart == LOT_START) //james 2016.0926 test site가 하나이상 Next LotNo로 셋팅되었을때 동작하자 
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
			if(nRet_1 == BD_GOOD) //정상적으로 완료된 상태
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
		//1. 로딩 버퍼는 테스트로보트가 요청를 시작하면서 움직인다 
		//2. Work RObot 사이트로 이동하여 로딩 자재를 요청
		//3. 로딩 자재를 모두 준비하여 테스트로봇 사이트로 버퍼를 보낸다
		//   이떄 버퍼는 테스트로봇이 작업할 테스트사이트의 정보에 맞게 요청한 공간에만 자재를 공급해야 한다
		////////////////////////////////////////////////////////////////////////////
		case 1000:		
			//break;
/*
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//james 임의가동  테스트를 위해 임의 설정하는곳 시작 ////////////////////////
			//////////////////////////////////////////////////////////////////////////////////
			//테스트 사이트이 자재를 언로딩 하니, 이곳에 넣을 로딩 버퍼의 자재를 미리 요청 
			//////////////////////////////////////////////////////////////////////////////////
			nTestSocket_0_4_Start_Pos = 0;
			st_sync_info.nTestSite_Target_THD_Work_Site = THD_TESTSITE_1;
			st_find_testsite_info[st_sync_info.nTestSite_Target_THD_Work_Site].nStart_FixPos = 0;
			st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].nSite_Pos = TESTSITE_LEFT;
			st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].nStart_FixPos = 0;
			//TestSite_PickPlace_SocketData_LoadBuff_Map_Combine(0, st_sync_info.nTestSite_Target_THD_Work_Site, st_find_testsite_info[st_sync_info.nTestSite_Target_THD_Work_Site].nStart_FixPos); //버퍼 위치별 작업 가능여부 정보,피커의 사용 유/무 같이 셋팅
			for(i=0; i< 4; i++) //loading 버퍼 수량 4개 
			{
				if(st_sync_info.nTestSite_Target_THD_Work_Site >= THD_TESTSITE_1 && st_sync_info.nTestSite_Target_THD_Work_Site <= THD_TESTSITE_4) //죄측 테스터 ,소켓 기준은 좌측위치 부터 (1회작업)7,6,5,4, (2회작업)3,2,1,0 의 순서로 기준 소켓 위치및 정보 정의됨
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
			st_buffer_info[THD_LD_BUFF].nTestSite_LR_Pos = st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //로딩 버퍼의 테스터에 맞는 회전 각도를 정한다
			st_buffer_info[THD_LD_BUFF].nTestSite_Start_FixPos = st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].nStart_FixPos;

			st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_REQ;
			st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][1] = WORK_PICK; //자재를 집을 수 있게 요청 
			st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][2] = st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //로딩 버퍼의 테스터에 맞는 회전 각도를 정한다
//james 임의가동  테스트를 위해 임의 설정하는곳 끝 ////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/
			//Test Robot이 지령을 내린다.
			if(st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_REQ && st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][1] == WORK_PICK)  
			{//st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][2] 이 정보는 버퍼의 회전 방향을 결정한다
			 //테스트사이트를 확정하여 로딩 버퍼 사용할 공간도 같이 셋팅한 후 위 사항을 요청한다 

				if(st_lot_info[LOT_CURR].nLotStatus >= LOT_END_START && st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)//jame 2016.0924 
				{//james 2016.0924 조건 추가 
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
			{//james 2016.0926 사용하지 않음 
				//로딩 버퍼에 테스트로봇은 소켓 오프등으로 집고있는 자재를 놓을 수 있게 요청 
			    //st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][2] 이 정보는 테스트 사이트가 left 인지 right인지의 정보로 버퍼의 회전 방향을 결정한다
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
		// 로딩 자재를 버퍼에 준비하여 테스트 사이트로 버퍼를 보내라
		///////////////////////////////////////////////////////////
		case 1100:
			nRet_1 = clsFunc.Check_BufferStatus(100, THD_LD_BUFF, CTL_NO, m_npBuff_Info, m_npBuff_Status, m_npBuff_OutputStatus); //버퍼는 완전히 비워져 있어야 한다 
			if(nRet_1 == RET_GOOD)
			{
				//정상 상태, 처음으로 work robot 사이트로 이동하는 루틴으로, 로딩 버퍼에는 자재가 하나도 없어야 한다 
				 m_nRunStep = 2000; 
			}
			else if(nRet_1 == RET_ERROR)
			{//자재가 남아있다면, 소켓 오프등으로 남아있는 자재이니 이떄는 테슽 로봇이 요청한 대로 바로 집을 수 있게 처리한다  

				m_nRunStep = 3000; 
				 //CTL_Lib.Alarm_Error_Occurrence(m_nRunStep, dWARNING, clsFunc.m_strAlarmCode);
			}
			break;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 로딩 버퍼에 테스트로봇의 소켓 오프등으로 집고있는 자재를 놓을 수 있게 요청 
		// 작업할 소켓에 집고있던 자재를 놓을 수 없을때는 이 피커으 자재는 로딩 버퍼에 놓고 이동한다 
		// 로딩 버퍼에 자재를 놓고 갔으면, 버퍼에는 추가 작업을 요청하지 않고 다음에 와서 남은 자재를 모두 집은 후 로딩 버퍼에 다음 작업을 요청한다 
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		case 1200: //이떄 버퍼는 완전히 비워져 있어야 한다 
			nRet_1 = clsFunc.Check_BufferStatus(100, THD_LD_BUFF, CTL_NO, m_npBuff_Info, m_npBuff_Status, m_npBuff_OutputStatus); //버퍼는 완전히 비워져 있어야 한다 
			if(nRet_1 == RET_GOOD)
			{
				//정상 상태, 
					m_nRunStep = 6000; //test robot가 집고있던 로딩 소켓 오프 자재를 놓는 영역이다. 이 자재를 놓고 테스트 사이트로 이동하여 작업한다
			}
			else if(nRet_1 == RET_ERROR)
			{//자재가 남아있다면 에러 
					CTL_Lib.Alarm_Error_Occurrence(3100, dWARNING, clsFunc.m_strAlarmCode);
			}			 
			break;


		////////////////////////////////////////////////////
		// Work Robot Site로 이동한다 
		// 이동전 버퍼에는 자재가 하나도 남아 있으면 안된다 
		// 로딩 자재를 다 받을 때까지 2000 step를 반복 동작한다 
		///////////////////////////////////////////////////
		case 2000:	
			m_dpTargetPosList[0] = st_motor_info[m_nAxisNum_Y].d_pos[P_LDBUFF_Y_WORKRBT];
			m_dpTargetPosList[1] = st_motor_info[m_nAxisNum_R].d_pos[P_LDBUFF_RTR_WORKRBT];

			nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate);
			// jtkim 20160831
			//nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nAxisNum_Y, m_dpTargetPosList[0], COMI.mn_runspeed_rate);
			if(nRet_1 == BD_GOOD) //정상적으로 완료된 상태
			{
				m_nRunStep = 2100;
			}
			else if(nRet_1 == BD_ERROR)
			{
				m_nRunStep = 2000;
				CTL_Lib.Alarm_Error_Occurrence(3110, dWARNING, st_alarm_info.strCode);
			}
			break;

		case 2100: //st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][2] == 테스트 사이트에서 작업할 위치 정보(Test left or test right
			nRet_1 = clsFunc.Find_TrayWork_Pos(WORK_PLACE, THD_LD_BUFF, CTL_NO, m_npFindWorkPosYXCPB, THD_WORK_RBT, st_worker_info.strLotNo_Empty);
			if(nRet_1 == RET_GOOD) //작업 가능한 자재가 있다, 놓는 작업은 뒤에서(0번) 부터 놓는다 
			{ //work robot는 로딩 버퍼에 자재를 놓을떄는 0->1->2->3 순서로 작업(정면 뒤에서 부터 1번이다)
				m_nWork_Pos = m_npFindWorkPosYXCPB[0]; 
			}
			else if(nRet_1 == RET_SKIP) //더이상 넣을 공간이 없다 
			{	//계속 진행할 수 없다
				m_nRunStep = 1000;	 
				break;
			}

			//work robot에 자재 공급을 요청 
//			st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_REQ; //로딩 자재 요청
//			st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][1] = WORK_PLACE; //자재 놓는 작업
//			//work robot에서 정보를 확인하여 알아서 작업한다 st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][2] = m_nWork_Pos; //작업할 시작 위치 
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
				//work robot에 자재 공급을 요청 
				st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_REQ; //로딩 자재 요청
				st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][1] = WORK_PLACE; //자재 놓는 작업
				//work robot에서 정보를 확인하여 알아서 작업한다 st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][2] = m_nWork_Pos; //작업할 시작 위치 
				m_nRunStep = 2200;
			}
	
			break;

		case 2200:
			if(st_sync_info.nWorkRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_READY)//자재를 로보트가 로딩버퍼에 모두 채워 놓은 상태
			{//work robot가 가능한 버퍼를 모두 채우고 CTL_READY 를 준다 
				nCount = 0;
				for(i = 0; i < st_recipe_info.nLdUldBuffer_Num; i++) //work robot는 로딩 버퍼에 0,1,2,3 순서로 작업
				{ //버퍼의 자재 정보를 셋팅한다
					m_npBuff_Info[i] = st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nYesNo; //버퍼의 자재 정보를 셋팅한다 
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
				{//자재가 하나도 없는 상태 
					if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END_START)
					{
						st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_BUFF] =  LOT_END_START;
						m_nRunStep = 0;
					}
				}
			}
			else if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END_START)//(st_lot_info[LOT_CURR].nLot_THD_Status[THD_LD_TRAY_PLATE] ==  LOT_END) //로보트 피커의 로딩 자재도 체크하고 lot end  진행 
			{
				nCount = 0;
				for(i = 0; i < st_recipe_info.nLdUldBuffer_Num; i++) //work robot는 로딩 버퍼에 0,1,2,3 순서로 작업
				{ //버퍼의 자재 정보를 셋팅한다
					m_npBuff_Info[i] = st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nYesNo; //버퍼의 자재 정보를 셋팅한다 
					//james 2016.0924 조건 추가  nCount++;
					if(st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nYesNo == CTL_YES &&
						st_buffer_info[THD_LD_BUFF].st_pcb_info[i].strLotNo == st_lot_info[m_nLotProcessNum].strLotNo)//james 2016.0924 조건 추가 
					{
						nCount++;
					}
				}

				if(nCount > 0)
				{
					//if(st_lot_info[LOT_NEXT].nLotStatus == LOT_START) //james 2016.0924 추가 
					//{
					//	break;
					//}

					//james 2016.0926 m_nRunStep = 2300;
				}
				else
				{//자재가 하나도 없는 상태 
					if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END_START && st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_BUFF] == LOT_START)
					{
						st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_BUFF] =  LOT_END_START;
						m_nRunStep = 0;
					}
				}
			}
			break;

		case 2300:
			nRet_1 = clsFunc.Check_BufferStatus(0, THD_LD_BUFF, CTL_YES, m_npBuff_Info, m_npBuff_Status, m_npBuff_OutputStatus); //전역변수와 센서 상태가 일치해야 한다 
			if(nRet_1 == RET_GOOD)
			{//정상 상태, //전역변수와 센서 상태가 일치해야 한다
				m_nRunStep = 2400;
			}
			else if(nRet_1== RET_ERROR)
			{//아직 
				 CTL_Lib.Alarm_Error_Occurrence(3120, dWARNING, clsFunc.m_strAlarmCode);
			} 
			break;

		case 2400:		 
			m_nRunStep = 3000;
			break;

		

		////////////////////////////////////////////////////
		// Test Robot Site로 이동한다 
		// 이동전 버퍼에는 자재가 하나 이상 있어야 된다 
		// 전에 놓고간/남아 있던 소켓 오프 자재도 있을 수 있다 
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
			if(nRet_1 == BD_GOOD) //정상적으로 완료된 상태
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
			nRet_1 = clsFunc.Check_BufferStatus(0, THD_LD_BUFF, CTL_YES, m_npBuff_Info, m_npBuff_Status, m_npBuff_OutputStatus); //전역변수와 센서 상태가 일치해야 한다 
			if(nRet_1 == RET_GOOD)
			{//정상 상태, //Enable 되어 있는 버퍼 위치는 모두 채워진 상태, 테스트 로봇 사이트로 이송가능하다 
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
			{//아직 버버 채우는 작업이 끝나지 않은 상태 
				 CTL_Lib.Alarm_Error_Occurrence(3140, dWARNING, clsFunc.m_strAlarmCode);
				 m_nRunStep = 3100;   
			} 
			break;

			//2016.1114
		case 3110:
			//Test Robot이 지령을 내린다.
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
		//로딩 버퍼가 준비되어 있으니 test robot이 자재 집어가기 요청
		/////////////////////////////////////////////////////////////
		case 3200:
			st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_READY; //test robot 자재 집어가기 요청
			m_nRunStep = 3300;
			break;

		case 3300:
			//st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_FREE; //james test시 사용, 이제 사용말자 

			if(st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_FREE)//TEST ROBOT 피커가 자재를 모두 집은 상태
			{
				//피커 작업 정보를 버퍼와 공유하여 동시에 같이 작업하게 한다 
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
				//james 2016.1004 로딩 버퍼는 준비되어있는데, 테스트 로봇이 재요청시 버퍼 위치 재 수정, 수락 m_nRunStep = 3200;
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
// 				//james 2016.1004 로딩 버퍼는 준비되어있는데, 테스트 로봇이 재요청시 버퍼 위치 재 수정, 수락 m_nRunStep = 3200;
// 				m_nRunStep = 3200;
// 			}
//			break; 

// 		case 3310: //james 2016.1005 사용하지 말자. 테스트 로봇이 내려와 있으면 충돌 발생 
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
// 			if(nRet_1 == BD_GOOD) //정상적으로 완료된 상태
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
	    // test robot이 집고있던 자재가 테스트 소켓오프등으로 처리할수 없을떄 로딩 버퍼에 놓고 간다 
		// Test Robot Site로 이동한다 
		// 이때 로딩 버퍼에는 자재가 하나도 없어야 한다 
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
			if(nRet_1 == BD_GOOD) //정상적으로 완료된 상태
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
			nRet_1 = clsFunc.Check_BufferStatus(100, THD_LD_BUFF, CTL_NO, m_npBuff_Info, m_npBuff_Status, m_npBuff_OutputStatus); // 로딩 버퍼에는 자재가 하나도 없어야 한다 
			if(nRet_1 == RET_GOOD)
			{//정상 상태, //Enable 되어 있는 버퍼 위치는 모두 채워진 상태, 테스트 로봇 사이트로 이송가능하다 
				 m_nRunStep = 6200;
			}
			else if(nRet_1== RET_ERROR)
			{//아직 버버 채우는 작업이 끝나지 않은 상태 
				 CTL_Lib.Alarm_Error_Occurrence(3160, dWARNING, clsFunc.m_strAlarmCode);
				 m_nRunStep = 6100;   
			} 
			break;

		case 6200:
			st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_READY;  //놓을 자재를 버퍼가 준비되었으니 놔라 
			m_nRunStep = 6300;
			break;

		case 6300:
			if(st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_FREE)//test robot가 정보에 따라 가져가지 못할 자재를 모두 놓은 상태 자재르 놓았으면 테스트 로봇은 로딩버퍼에 추가 작업 요청을 하면 안된다.
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