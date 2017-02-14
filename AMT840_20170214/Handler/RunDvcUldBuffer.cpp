// RunDvcUldBuffer.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "RunDvcUldBuffer.h"


#include "Variable.h"
#include "FastechPublic_IO.h"
#include "CtlBd_Library.h"

#include "ComizoaPublic.h"
#include "CtlBd_Variable.h"
#include "PublicFunction.h"
#include "LogCtrl.h"
#include "AlgMemory.h"
#include "LogFromat.h"

// CRunDvcUldBuffer
CRunDvcUldBuffer clsRunDvcUldBuffer;
CRunDvcUldBuffer::CRunDvcUldBuffer()
{
	m_nRunStep	= -1;
	m_nInitStep	= -1;


	//linear move 과련 셋팅
	m_nLinearMove_Index	= 2;   //현재 map index
	m_lAxisCnt	= 2;            //현재의 IndexNum에서 사용 가능한 모터 수량 최대 4개
	m_lpAxisNum[0] = M_ULD_BUF_SHIFTER_Y; //m_lpAxisNum[4];        //현재의 IndexNum에서 사용하는 모터 실재 번호를 가진다
	m_lpAxisNum[1] = M_ULD_BUF_ROTATOR;


	m_nAxisNum_Y = M_ULD_BUF_SHIFTER_Y;
	m_nAxisNum_R = M_ULD_BUF_ROTATOR;

	//2016.1203
	m_dwCycleTime[0] = 0;
}

CRunDvcUldBuffer::~CRunDvcUldBuffer()
{
}


// CRunDvcUldBuffer 멤버 함수
void CRunDvcUldBuffer::OnThreadRun()
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


void CRunDvcUldBuffer::OnRunInit()
{
	int nRet_1, nRet_2 = 0;
	int nIO[4]={0,};

	if (st_sync_info.nInit_Flag[THD_ULD_BUFF] != INIT_CLEAR)		return;		//INIT_CLEAR 일때만 초기화 작업을 한다. 초기화가 끝나면 INIT_CLEAR -> INIT_READY가 되기 떄문에...
	 
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
				CTL_Lib.Alarm_Error_Occurrence(3500, CTL_Lib.mn_run_status, COMI.strCode);
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
				CTL_Lib.Alarm_Error_Occurrence(3510, CTL_Lib.mn_run_status, COMI.strCode);
				m_nInitStep = 100;
			}		
			break;
			//kwlee 2016.0907 Gripper 모두 벌리 도록 수정.
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
		case 320:
			m_nInitStep = 900;
			break;


		case 900:
			st_sync_info.nInit_Flag[THD_ULD_BUFF] = INIT_READY; 
			 
			m_nInitStep = 0;
			break;	

		default :
			break;

	}
}

void CRunDvcUldBuffer::OnRunMove()
{
	int nRet_1, nRet_2=0, nRet_3=0;
	int nCount = 0, i;
	CString strMsg;


	clsFunc.OnThreadFunctionStepTrace(2, m_nRunStep);
	switch (m_nRunStep)
	{ 
		case -1: 

			m_nRunStep = 0;
			break;

		case 0:
			//if(st_lot_info[LOT_CURR].nLot_THD_Status[THD_ULD_BUFF] == LOT_END) break;
			if(st_lot_info[LOT_CURR].nLotStatus == LOT_START || st_lot_info[LOT_NEXT].nLotStatus == LOT_START)
			{
				m_nRunStep = 100;
			} 	
			else if(st_lot_info[LOT_CURR].nLotStatus >= LOT_END_START && st_lot_info[LOT_CURR].nLot_THD_Status[THD_WORK_RBT] < LOT_END)
			{//Multi lot시Next lot 이 미리 끝나는 문제로 추가 
				m_nRunStep = 100;
			}
			break;

		case 100:
			//if(st_lot_info[LOT_CURR].nLotStatus == LOT_START || st_lot_info[LOT_NEXT].nLotStatus == LOT_START)
			//{
				m_nRunStep = 200;
			//} 			
			break;

		case 200:
			//m_nRunStep = 200;
			//break;

			m_dpTargetPosList[0] = st_motor_info[m_nAxisNum_Y].d_pos[P_ULDBUFF_Y_WORKRBT];
			m_dpTargetPosList[1] = st_motor_info[m_nAxisNum_R].d_pos[P_ULDBUFF_RTR_WORKRBT];
			nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate);
			if(nRet_1 == BD_GOOD) //정상적으로 완료된 상태
			{
				m_nRunStep = 300;
			}
			else if(nRet_1 == BD_ERROR)
			{
				m_nRunStep = 200;
				CTL_Lib.Alarm_Error_Occurrence(3600, dWARNING, st_alarm_info.strCode);
			}
			break;

		case 300:
			m_nRunStep = 900;
			break;

		case 900:
			
			m_nRunStep = 1000;
			break;

		////////////////////////////////////////////////////////////////////////////
		//1. 언로딩 버퍼는 테스트로보트가 요청를 시작하면 정보에 맞게 회전하여 준비상태를 알린다 
		//
		////////////////////////////////////////////////////////////////////////////
		case 1000:		
/*
//james 임의가동  테스트를 위해 임의 설정하는곳 시작 ////////////////////////
///////////////////////////////////			////////////////////////////////////////////
			//test TEST SITE 작업 위치확정  //작업할 테스트 사이트 위치를 확정한다 
			st_sync_info.nTestRbt_Dvc_Req[THD_ULD_BUFF][0] = CTL_REQ;
			st_sync_info.nTestRbt_Dvc_Req[THD_ULD_BUFF][1] = WORK_PLACE; //자재를 집을 수 있게 요청 
			st_sync_info.nTestRbt_Dvc_Req[THD_ULD_BUFF][2] = TESTSITE_LEFT; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //언로딩 버퍼를 테스터와 같은 1번 pin 방향을 돌려 받는다 

			st_sync_info.nTestSite_Target_THD_Work_Site = THD_TESTSITE_1;
			st_buffer_info[THD_ULD_BUFF].nTestSIte_No            = THD_TESTSITE_1;//st_sync_info.nTestSite_Target_THD_Work_Site;
			st_buffer_info[THD_ULD_BUFF].nTestSite_LR_Pos        = TESTSITE_LEFT; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //로딩 버퍼의 테스터에 맞는 회전 각도를 정한다
			st_buffer_info[THD_ULD_BUFF].nTestSite_Start_FixPos = 0;
//james 임의가동  테스트를 위해 임의 설정하는곳 끝 ////////////////////////
//////////////////////////////
*/
			if(st_sync_info.nTestRbt_Dvc_Req[THD_ULD_BUFF][0] == CTL_REQ && st_sync_info.nTestRbt_Dvc_Req[THD_ULD_BUFF][1] == WORK_PLACE)  
			{//st_sync_info.nTestRbt_Dvc_Req[THD_ULD_BUFF][2] 이 정보는 버퍼의 회전 방향을 결정한다
				m_nRunStep = 1100; 
			}
			else
			{
				if(st_lot_info[LOT_CURR].nLot_THD_Status[THD_TEST_RBT] == LOT_END || st_lot_info[LOT_CURR].nLotStatus < LOT_START)
				{
					st_lot_info[LOT_CURR].nLot_THD_Status[THD_ULD_BUFF] = LOT_END;
					//2016.1011
					//2016.1116
					st_sync_info.nWorkRbt_Dvc_Req[THD_ULD_BUFF][0] = CTL_CLEAR;
					//st_sync_info.nTestRbt_Dvc_Req[THD_ULD_BUFF][0] = CTL_CLEAR

					m_nRunStep = 0; 
				}
			}
			break;

		//////////////////////////////////////////////////////////
		// 언로딩 버퍼를 준비하여 테스트 사이트로 버퍼를 보내라
		//이때 버퍼는 모두 비워져 있어야 한다 
		//////////////////////////////////////////////////////////
		case 1100:			 
			nRet_1 = clsFunc.Check_BufferStatus(100, THD_ULD_BUFF, CTL_NO, m_npBuff_Info, m_npBuff_Status, m_npBuff_OutputStatus); //버퍼는 완전히 비워져 있어야 한다 
			if(nRet_1 == RET_GOOD)
			{
				//정상 상태, 처음으로 test robot 사이트로 이동하는 루틴으로, 언로딩 버퍼에는 자재가 하나도 없어야 한다 
				 m_nRunStep = 2000; 
			}
			else if(nRet_1 == RET_ERROR)
			{				 
				 CTL_Lib.Alarm_Error_Occurrence(3610, dWARNING, clsFunc.m_strAlarmCode);
			}
			break;		 

		////////////////////////////////////////////////////
		// test Robot Site로 이동한다 
		// 이동전 버퍼에는 자재가 하나도 남아 있으면 안된다 
		///////////////////////////////////////////////////
		case 2000:	
			m_dpTargetPosList[0] = st_motor_info[m_nAxisNum_Y].d_pos[P_ULDBUFF_Y_TESTRBT];
			if(st_sync_info.nTestRbt_Dvc_Req[THD_ULD_BUFF][2] == TESTSITE_LEFT)
			{
				m_dpTargetPosList[1] = st_motor_info[m_nAxisNum_R].d_pos[P_ULDBUFF_RTR_TEST_1_LEFT];
			}
			else if(st_sync_info.nTestRbt_Dvc_Req[THD_ULD_BUFF][2] == TESTSITE_RIGHT)
			{
				m_dpTargetPosList[1] = st_motor_info[m_nAxisNum_R].d_pos[P_ULDBUFF_RTR_TEST_2_RIGHT];
			}

			nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate);
			if(nRet_1 == BD_GOOD) //정상적으로 완료된 상태
			{
				m_nRunStep = 2100;
			}
			else if(nRet_1 == BD_ERROR)
			{
				m_nRunStep = 2000;
				CTL_Lib.Alarm_Error_Occurrence(3620, dWARNING, st_alarm_info.strCode);
			}
			break;

		case 2100:
			nRet_1 = clsFunc.Find_TrayWork_Pos(WORK_PLACE, THD_ULD_BUFF, CTL_NO, m_npFindWorkPosYXCPB, THD_TEST_RBT, st_worker_info.strLotNo_Empty);
			if(nRet_1 == RET_GOOD) //작업 가능한 자재가 있다
			{
				m_nWork_Pos = m_npFindWorkPosYXCPB[0]; 
			}
			else if(nRet_1 == RET_SKIP) //더이상 넣을 공간이 없다 
			{	//계속 진행할 수 없다
				m_nRunStep = 1000;	 
				break;
			}		 


			st_sync_info.nTestRbt_Dvc_Req[THD_ULD_BUFF][0] = CTL_READY;		//자재를 받을 준비가 되었다  
			m_nRunStep = 2200;
			break;

		case 2200://james test 
			//james 2016.0831 본체만 테스트 가동시만 사용 if(1 || st_sync_info.nTestRbt_Dvc_Req[THD_ULD_BUFF][0] == CTL_FREE)//자재를 로보트가 놓은 상태
			if(st_sync_info.nTestRbt_Dvc_Req[THD_ULD_BUFF][0] == CTL_FREE)//자재를 로보트가 놓은 상태
			{
//				st_sync_info.nWorkRbt_Dvc_Req[THD_ULD_BUFF][0] = CTL_CLEAR;
				//2016.1012
				st_sync_info.nTestRbt_Dvc_Req[THD_ULD_BUFF][0] = CTL_CLEAR;
				m_nRunStep = 2300;
			}
			else if(st_lot_info[LOT_CURR].nLot_THD_Status[THD_TEST_RBT] == LOT_END) //로보트 피커의 로딩 자재도 체크하고 lot end  진행 
			{
				st_lot_info[LOT_CURR].nLot_THD_Status[THD_ULD_BUFF] = LOT_END;
				st_sync_info.nWorkRbt_Dvc_Req[THD_ULD_BUFF][0] = CTL_CLEAR;
//				st_sync_info.nTestRbt_Dvc_Req[THD_ULD_BUFF][0] = CTL_CLEAR;
				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					strMsg = _T("[ULD_BUFFER] Uld_buffer lotend clear ");
					clsMem.OnNormalMessageWrite(strMsg);
				}
//				m_nRunStep = 0;
				m_nRunStep = 2210;
			}
			//james 2016.0929 추가 
			if(st_sync_info.nTestRbt_Dvc_Req[THD_ULD_BUFF][0] == CTL_REQ && st_sync_info.nTestRbt_Dvc_Req[THD_ULD_BUFF][1] == WORK_PLACE)  
			{//st_sync_info.nTestRbt_Dvc_Req[THD_ULD_BUFF][2] 이 정보는 버퍼의 회전 방향을 결정한다
				//m_nRunStep = 1000; 
			}
			break;

		case 2210:
			if(st_sync_info.nTestRbt_Dvc_Req[THD_ULD_BUFF][0] == CTL_FREE)//자재를 로보트가 놓은 상태
			{
				st_sync_info.nTestRbt_Dvc_Req[THD_ULD_BUFF][0] = CTL_CLEAR;
				m_nRunStep = 2300;
			}
			break;

		case 2300:
			nRet_1 = clsFunc.Check_BufferStatus(0, THD_ULD_BUFF, CTL_YES, m_npBuff_Info, m_npBuff_Status, m_npBuff_OutputStatus); //전역변수와 센서 상태가 일치해야 한다 
			if(nRet_1 == RET_GOOD)
			{//정상 상태, //전역변수와 센서 상태가 일치해야 한다
				 m_nRunStep = 2400;
			}
			else if(nRet_1== RET_ERROR)
			{//아직 
				 CTL_Lib.Alarm_Error_Occurrence(3630, dWARNING, clsFunc.m_strAlarmCode);
			} 
			break;

		case 2400:
			//2016.0906 test 용

			/////////////////////////////////////////////////////////////////////////////////////
			//2016.1203
			m_strLotNo = st_buffer_info[THD_ULD_BUFF].strLotNo;
			m_strPartNo = st_buffer_info[THD_ULD_BUFF].strPartNo;
			/////////////////////////////////////////////////////////////////////////////////////
			
			m_nRunStep = 6000; 
			break;

//james 임의가동  테스트를 위해 임의 설정하는곳 시작 ////////////////////////
/////////////////////////////// test
			for(i=0; i< 4; i++) //loading 버퍼 수량 4개 
			{
				if(st_sync_info.nTestSite_Target_THD_Work_Site[WORK_PICK] >= THD_TESTSITE_1 && st_sync_info.nTestSite_Target_THD_Work_Site[WORK_PICK] <= THD_TESTSITE_4) //죄측 테스터 ,소켓 기준은 좌측위치 부터 (1회작업)7,6,5,4, (2회작업)3,2,1,0 의 순서로 기준 소켓 위치및 정보 정의됨
				{
					  
					st_buffer_info[THD_ULD_BUFF].st_pcb_info[i].nYesNo = CTL_YES;
					if(i == 0 || i == 2) st_buffer_info[THD_ULD_BUFF].st_pcb_info[i].nBin = BD_DATA_REJECT;//BD_DATA_GOOD;
					else				 st_buffer_info[THD_ULD_BUFF].st_pcb_info[i].nBin = BD_DATA_GOOD;
						
					st_buffer_info[THD_ULD_BUFF].st_pcb_info[i].strLotNo = st_lot_info[LOT_CURR].strLotNo; 
				}
				m_npBuff_Info[i] = CTL_YES;	
			}

			Set_Gripper_OnOff(0, IO_OFF, m_npBuff_Info);
/////////////////////////////////////////////////////////////		
//james 임의가동  테스트를 위해 임의 설정하는곳 끝 ////////////////////////
			 m_nRunStep = 6000; 
			break;

	

		////////////////////////////////////////////////////
		// Work Robot Site로 이동한다 
		// 이동전 버퍼에는 자재가 하나 이상 있어야 된다 
		///////////////////////////////////////////////////
		case 6000:
			m_dpTargetPosList[0] = st_motor_info[m_nAxisNum_Y].d_pos[P_ULDBUFF_Y_WORKRBT];
			m_dpTargetPosList[1] = st_motor_info[m_nAxisNum_R].d_pos[P_ULDBUFF_RTR_WORKRBT]; 
			nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate);
			if(nRet_1 == BD_GOOD) //정상적으로 완료된 상태
			{
				m_nRunStep = 6100;
			}
			else if(nRet_1 == BD_ERROR)
			{
				m_nRunStep = 6000;
				CTL_Lib.Alarm_Error_Occurrence(3640, dWARNING, st_alarm_info.strCode);
			}
			break;

		case 6100:
			nRet_1 = clsFunc.Check_BufferStatus(0, THD_ULD_BUFF, CTL_YES, m_npBuff_Info, m_npBuff_Status, m_npBuff_OutputStatus); //전역변수와 센서 상태가 일치해야 한다 
			if(nRet_1 == RET_GOOD)
			{//정상 상태, //Enable 되어 있는 버퍼 위치는 모두 채워진 상태, 테스트 로봇 사이트로 이송가능하다 
				 m_nRunStep = 6200;
			}
			else if(nRet_1== RET_ERROR)
			{//아직 버버 채우는 작업이 끝나지 않은 상태 
				 CTL_Lib.Alarm_Error_Occurrence(3650, dWARNING, clsFunc.m_strAlarmCode);
				 m_nRunStep = 6100;   
			} 
			break;

		case 6200:
			st_sync_info.nWorkRbt_Dvc_Req[THD_ULD_BUFF][0] = CTL_REQ; //work robot이 자재 집어가게 요청
			st_sync_info.nWorkRbt_Dvc_Req[THD_ULD_BUFF][1] = WORK_PICK;			
			m_nRunStep = 6300;
			break;

		case 6300:
			if(st_sync_info.nWorkRbt_Dvc_Req[THD_ULD_BUFF][0] == CTL_READY)//로보트 피커가 자재를 모두 처리한 상태
			{
				st_sync_info.nWorkRbt_Dvc_Req[THD_ULD_BUFF][0] = CTL_CLEAR;
				m_nRunStep = 1000;

				//////////////////////////////////////////////////////////////////////////
				//2016.1203
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
						clsFunc.OnCycleTime(3, 
											m_strLotNo,
											m_strPartNo,
											m_dwCycleTime[2], 
											0, 
											0);
					}

					m_dwCycleTime[0] = GetTickCount();
				}
				//////////////////////////////////////////////////////////////////////////

				break;
			}
			break;
	}
}



void CRunDvcUldBuffer::Set_Gripper_OnOff(int nMode, int nOnOff, int *npPickerInfo)
{
	int i;
	int nNum		= 4;
	int nRecovery	= RECO_LD_RBT_PICK_VAC_1;

	CString strLogKey[10];
	CString	strLogData[10];
	if (nMode > 1) return;	 


	strLogKey[0] = _T("Mode Start");
	strLogData[0].Format(_T("%d"),nMode);

	for (i=0; i<nNum; i++)
	{
		if (npPickerInfo[i] == YES)
		{
			FAS_IO.set_out_bit(st_io_info.o_UldBuffGripOnOffCyl[i], nOnOff);
			m_bPickerFlag			= false;
			m_dwPickerWaitTime[0]	= GetCurrentTime();
		}
	}
	///kwlee 2016.0812
	if (nOnOff == IO_ON)
	{
		//LogFunction(_T("DVC_ULD_BUFFER"),_T("FORWARD"),_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("DVC_ULD_BUFFER"),_T("FORWARD"),0,_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
	else
	{
		//LogFunction(_T("DVC_ULD_BUFFER"),_T("BACKWARD"),_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("DVC_ULD_BUFFER"),_T("BACKWARD"),0,_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
}

int CRunDvcUldBuffer::Chk_Gripper_OnOff(int nMode, int nOnOff, int *npPickerInfo)
{

//	800712 1 0 "LOAD_UNLOAD_PICKER_GLIPPER_DVC_EXIST_CHECK"
//	int i;
	int nNum		= 4;
	int nWaitTime = WAIT_RBTGLIPPER_ON_OFF;
	
	CString strLogKey[10];
	CString	strLogData[10];

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
				//LogFunction(_T("DVC_ULD_BUFFER"),_T("BACKWARD"),_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData);///kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("DVC_ULD_BUFFER"),_T("BACKWARD"),1,_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
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
				//LogFunction(_T("DVC_ULD_BUFFER"),_T("FORWARD"),_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData);///kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("DVC_ULD_BUFFER"),_T("FORWARD"),1,_T("GRIPPER"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_GOOD;
			}
		}
	}

	return RET_PROCEED;
}