// RunTestSiteRobot.cpp : 구현 파일입니다.
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

	//linear move 과련 셋팅
	m_nLinearMove_Index	= 3;   //현재 map index
	m_lAxisCnt	= 2;            //현재의 IndexNum에서 사용 가능한 모터 수량 최대 4개
	m_lpAxisNum[0] = M_TEST_RBT_Y; //m_lpAxisNum[4];        //현재의 IndexNum에서 사용하는 모터 실재 번호를 가진다
	m_lpAxisNum[1] = M_TEST_RBT_X;
	m_lpAxisNum[2] = M_TEST_RBT_Z;
	m_lpAxisNum[3] = M_TEST_DVC_EJECT_Z;

	m_nRobot_X = M_TEST_RBT_X;
	m_nRobot_Y = M_TEST_RBT_Y;
	m_nRobot_Z = M_TEST_RBT_Z; 
	m_nRobot_Ejector = M_TEST_DVC_EJECT_Z;

	m_dpTargetPosList[0]  = 0; //m_dpTargetPosList[4];		   //최대 4 포인트를 한번에 이동가능하다
	m_dpTargetPosList[1]  = 0;
	m_dpTargetPosList[2]  = 0;

 	m_dpSpdRatio[0] = 100; 		//	m_dpSpdRatio[3];        //속도비율 [0]:Vel, [1]:Acc, [2]:Dec
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

	if (st_handler_info.nSystemLock != FALSE)  return;	// 현재 시스템 Lock 상태인 경우 리턴  
	

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
				{//모터 상태가 mMST_STOP 이다 
					if(CTL_Lib.m_nStep_Single[M_TEST_RBT_Z] > 0)
					{
						COMI.Set_MotStop(1, M_TEST_RBT_Z) ; //긴급정지 
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

	if (st_sync_info.nInit_Flag[THD_TEST_RBT] != INIT_CLEAR)		return;		//INIT_CLEAR 일때만 초기화 작업을 한다. 초기화가 끝나면 INIT_CLEAR -> INIT_READY가 되기 떄문에...
	 
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

		case 500://james 2016.0827 그리퍼 디바이스 체크 정보 수정/보완, Gripper Off(클림프상태)에서 IO_OFF시 디바이스 있는상태, 그외는 ON/OFF 산관없이 모두 디바이스 없는 상태
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
			if(nRet_1 == BD_GOOD) //정상적으로 완료된 상태
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
	int nEnableSokPerSite[TEST_SOCKET_PARA] = {0,};//사이트당 소켓사용 가능개수
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

		case 100://피커 상태를 파악한다			
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
			{  //자재를 집고 있지 않다 
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
			if (nRet_1 == BD_GOOD) //좌측으로 이동
			{
				m_nRunStep = 400;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nRunStep = 300;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
				CTL_Lib.Alarm_Error_Occurrence(2110, dWARNING, st_alarm_info.strCode);
				m_nRunStep = 300;
			}
			break;

		case 400:
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Ejector, st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY], COMI.mn_runspeed_rate);
			if (nRet_1 == BD_GOOD) //좌측으로 이동
			{
				m_nRunStep = 500;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nRunStep = 400;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
				CTL_Lib.Alarm_Error_Occurrence(2111, dWARNING, st_alarm_info.strCode);
				m_nRunStep = 400;
			}
			break;

		case 500:
			m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_SAFETY];
			m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_SAFETY];
			nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate);
			if(nRet_1 == BD_GOOD) //정상적으로 완료된 상태
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
//테스트로봇 동작 플로워 확정 
//** Test robot 피커순서(정면에서 좌측 1 ~ 4)에 맞게 전체 사이트 위치를 순서를 정한다. 실제 소켓등 순서와 맞지않는 부분은 Map Exchange를 통해 해결한다
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*		
1. 테스터 로봇은 동작할 테스터를 확정 후 그 테스터를 기준으로 소켓정보에 맞게 로딩 버퍼에 자재 공급을 요청한다 
2. 테스트가 끝난 테스터가 있으면 언로딩 후 바로 로딩하며, 테스터가 비어있으면 로딩만 한다
3. 언로딩 순서는 GOOD, Reject 자재를 언로딩 버퍼에 놓은 후 리테스트 자재는 리테스트 버퍼에 놓는다
4. 리테스트 자재 놓기전 테스터에 놓을 자재가 피커와 리테스트 버퍼를 기준으로 있으면 로딩자재보다는 리테스트 자재를 먼저 집어 테스트 사이트에 공급/놓는다 
5. 소켓오프로 로딩 버퍼에 자재가 남아있으면 테스트 사이트위치에 따라 집은 후, 또는 간섭이 없으면 집지안고,있돈저재와 같이 로딩 버퍼에 추가 공급을 요청할 수 있다.
6.  리테스트 자재는 4개 이상 일렬로 배열되어있으면 처리 할 수 있다 
 */
		case 1000:
			if(st_lot_info[LOT_CURR].nLotStatus == LOT_CLEAR && st_lot_info[LOT_NEXT].nLotStatus == LOT_CLEAR )
			{
				m_nRunStep = 0;
				break;
			}

			nTemp_Test_Pick_THD_Site = 0; //클리어 
			nTemp_Test_Place_THD_Site = 0; //클리어 
			nTemp_Test_THD_WorkSite = 0; 
			m_nTestSite_THD_Work_Site = 0; //clear 

			for(i = 0; i < TEST_PICKER_PARA; i++) st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable = CTL_YES;//피커 모두 사용으로 셋팅 
			
			//////////////////////////////////////////////////////////////////////////////////////////////////////////
			//연속 lot이 가능하게 작업할 lot 정보를 LOT_CURR과 LOT_NEXT LotNo를 반복적으로 변경하여 Lot 정보로 찾는다 
			//////////////////////////////////////////////////////////////////////////////////////////////////////////
			if(m_nFindLotNo_Flag == 0)
			{
				m_nFindLotNo_Flag = 1;

				if(st_lot_info[LOT_CURR].nLotStatus >= LOT_START) //james 2016.0924 추가 
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
				
				// 연속 LOT 작업시 조건 체크 방법 
				//james 2016.0924 if(st_lot_info[LOT_NEXT].nLdTrayCnt > 0 && st_lot_info[LOT_NEXT].nLotStatus >= LOT_START) 
				if(st_lot_info[LOT_NEXT].nLotStatus >= LOT_START) 
				{
					m_strFindLotNo = st_lot_info[LOT_NEXT].strLotNo;
					m_nLotProcessNum = LOT_NEXT; 		
				}
				else
				{//연속 lot 이 진행중이지 않을떄는 현재 lot No 만 비교하게 break; 한다 
					break;
				}
			}//연속 lot 정보 확인 

			if(st_lot_info[m_nLotProcessNum].strLotNo == "") //lot 정보가 없으면 리턴 
			{
				break;
			}
			else if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_TEST_RBT] ==  LOT_END) //james 2016.0909 
			{
				break;
			}
			//next lot 테스트사이트의 LotNo 를 찾아 있으면 동작하고, 없으면 break 한다 
			nCount = 0; nSiteYesCnt = 0;	
			for(i = THD_TESTSITE_1; i <= THD_TESTSITE_8; i++) //james 206.0930
			{
				if(st_lot_info[m_nLotProcessNum].strLotNo ==  st_test_site_info[i].strLotNo)
				{
					nCount++;
				}
			}
			if(nCount == 0)//멀티랏일때 같은 랏이 없을 경우
			{			
				//2016.1107
 				if(st_lot_info[LOT_CURR].nLot_THD_Status[THD_TEST_RBT] >=  LOT_END_START && st_lot_info[LOT_NEXT].nLotStatus >= LOT_START) //현재 lot이 lot end 진행중이고, 처음 로딩 자재가 더이상 없으면 테스트 사이트도 Next Lot을 위해 Test Site #1 ~#4 까지는 모두 비워져 있으면 Nect Lot 정보로 지정한다 
 				{ 
					//if( nNextLotPlaceSite == true )
					if( clsFunc.CheckCurrLotEndNextLot( nNeedOneMoreSite ) == RET_GOOD )
					{
 						for( int nSiteNum = THD_TESTSITE_1; nSiteNum <= THD_TESTSITE_8; nSiteNum++) 
 						{//해당 테스트 사이트에 자재가 모두 없으면

 							nCount = 0;
							nRet_2 = 0;
 							for(i = 0; i < TEST_SOCKET_PARA; i++) //x 방향 소켓 수량 정보(8개, 0~7개)룰 수집한다 
 							{
 								if(st_lot_info[LOT_CURR].strLotNo ==  st_test_site_info[nSiteNum].strLotNo
 									&& st_test_site_info[nSiteNum].st_pcb_info[i].nYesNo == CTL_YES)
 								{
 									nCount++;//현재 랏의 디바이스가 소켓에 꽂혀있는 개수
 								}
								if(st_lot_info[LOT_CURR].strLotNo ==  st_test_site_info[nSiteNum].strLotNo &&
									st_test_site_info[nSiteNum].st_pcb_info[i].nEnable == YES)
								{
									nRet_2++; 
								}
							}
							if( nCount == 0 && nRet_2 > 0)
							{
								//리테스트자재가 있어서 여분으로 하나의 사이트를 더 가지고 갈 필요가 있을 떄 사이트 하나를 여분으로 남겨 놓는다.
								if( nNeedOneMoreSite == true)
								{
									nNeedOneMoreSite = false;
									continue;
								}
								st_test_site_info[nSiteNum].strLotNo = st_lot_info[LOT_NEXT].strLotNo; //test site의 lot정보를 LOT_NEXT로 변경 셋팅하여 LOT_NEXT를 진행한다 
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
			//TEST END PICK 정보 찾기
			///////////////////////////////////////////////////////////////////////////////////
			//Test site 의 정보를 확인하여 테스트가 끝난것이 있으면 WORK_PICK 작업이 가능하다 
			//테스트 소켓에서 테스트가 끝나 집을 수 있는 자재가 있으면 집는다, 언로딩 소팅.
			////////////////////////////////////////////////////////////////////////////////////	
			nRet_1 = clsFunc.Find_TestSite_Work_Pos(0, m_strFindLotNo, WORK_PICK, m_nSyncTestCount, m_npGetSiteInfo); //m_nSyncTestCount : 한번에 동시 작업할 테스트 사이트 수량(현재 4para)		
			if(nRet_1 == RET_GOOD) 
			{				
				nFlag = 0; //처음 선택된 사이트 확인
				for(nTHD_i = THD_TESTSITE_1; nTHD_i <= THD_TESTSITE_8; nTHD_i++)
				{
					if(st_find_testsite_info[nTHD_i][1].nRun_Mode == WORK_PICK && st_find_testsite_info[nTHD_i][1].nTotal_Cnt > 0) //1 테스트가 끝난 언로딩이 가능한 사이트 정보 
					{
						if(st_sync_info.nTestSite_Target_THD_Work_Site[1] == nTHD_i) //작업하던 곳에 테스트 사이트가 또 작업할 수 있는 상태로, 그곳을 이어서 작업해야 한다 
						{
							nFlag++; 
							nTemp_Test_Pick_THD_Site = nTHD_i; //작업하던 사이트로 해당 테스터 사이트를 이어서 작업한다 
							break;
						}

						if(nFlag == 0)
						{
							nTemp_Test_Pick_THD_Site = nTHD_i;
						}
						nFlag++; 
					}					
					//nTestSite[1] = st_find_testsite_info[st_sync_info.nTestSite_Target_THD_Work_Site].nSite_Pos; //left or right 어느사이트에서 작업하고 왔는지 정보 저장 	
					//nTestSite[2] = st_find_testsite_info[st_sync_info.nTestSite_Target_THD_Work_Site].nStart_FixPos;
				} //for

				//nTestSite[0] = nTemp_Test_Pick_THD_Site;
				/* james load & unload 조건을 모두 확이 후 밑에서 한번에 처리 
				if(nTemp_Test_Pick_THD_Site >= THD_TESTSITE_1) //THD_TESTSITE_1 ~ THD_TESTSITE_8 이제 작업할 테스트 사이트 위치 정보 
				{
					m_nTestSite_THD_Work_Site = nTemp_Test_Pick_THD_Site;
					m_nRunStep = 1100;
					break;
				}*/
			}

			///////////////////////////////////////////////////////////////////////////////////
			// TEST SITE PLACE 정보 찾기
			//////////////////////////////////////////////////////////////////////////////////
			// test site에 자재를 로딩할 WORK_PLACE 작업 조건을 찾는
			//테스트 사이트의 소켓이 비어있는 상태로, 로딩 자재를 집어 공급한다 
			////////////////////////////////////////////////////////////////////////////////////	
			nFlag = 0; //처음 선택된 사이트 확인
			nRet_1 = clsFunc.Find_TestSite_Work_Pos(0, m_strFindLotNo, WORK_PLACE, m_nSyncTestCount, m_npGetSiteInfo); //m_nSyncTestCount : 한번에 동시 작업할 테스트 사이트 수량(현재 4para)
			if(nRet_1 == RET_GOOD) 
			{				
				nFlag = 0; //처음 선택된 사이트 확인
				for(nTHD_i = THD_TESTSITE_1; nTHD_i <= THD_TESTSITE_8; nTHD_i++)
				{
					if(st_find_testsite_info[nTHD_i][2].nRun_Mode == WORK_PLACE && st_find_testsite_info[nTHD_i][2].nTotal_Cnt > 0) //1 테스트가 끝난 언로딩이 가능한 사이트 정보 
					{
						if(st_sync_info.nTestSite_Target_THD_Work_Site[2] == nTHD_i &&
							( st_test_site_info[nTHD_i].nTestBdStart == BD_END || st_test_site_info[nTHD_i].nTestBdStart == BD_NONE)) //작업하던 곳에 테스트 사이트가 또 작업할 수 있는 상태로, 그곳을 이어서 작업해야 한다 
						{
							nFlag++; //2016.0911
							nTemp_Test_Place_THD_Site = nTHD_i; //작업하던 사이트로 해당 테스터 사이트를 이어서 작업한다 
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

				/* james load & unload 조건을 모두 확이 후 밑에서 한번에 처리 
				if(nTemp_Test_Place_THD_Site >= THD_TESTSITE_1) //THD_TESTSITE_1 ~ THD_TESTSITE_8 이제 작업할 테스트 사이트 위치 정보 
				{
					m_nTestSite_THD_Work_Site = nTemp_Test_Place_THD_Site;
					m_nRunStep = 1200;
					break;
				}*/			 
			}
			
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//james 2016.0909 추가, 
			// lot end 진행시, THD_WORK_RBT이 LOT_END_START 했다는 이야기는 로딩 버퍼에만 자재가 있을수 있다는 이야기이다 
			// 즉 로딩 버퍼에 자재를 놓은 후 THD_WORK_RBT이 LOT_END_START 를 진행한다
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		
			nFlag = 0;
			if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END_START && st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_TEST_RBT] < LOT_END_START) //이미 lot end를 진행중이고, 로딩/리테스트할 자재가 없으면 로딩은 더이상 없다 
			{	//혹시 버퍼에 자재가 있으면 처리해야 함 
				nRet_1 = clsFunc.Find_TrayWork_Pos(WORK_PICK, THD_LD_BUFF, CTL_YES, m_npFindWorkPosYXCPB, THD_TEST_RBT, m_strFindLotNo);		 			
				if(nRet_1 == RET_GOOD && m_npFindWorkPosYXCPB[2] > 0) //로딩 버퍼에 더이상 자재가 없으면 test robot은 더이상 로딩 자재는 처리하지 않으니 이때  THD_TEST_RBT의 LOT_END_START 를 진행한다 
				{		
					nFlag++;
					//처리할 로딩 자재가 있다 , //로딩 버퍼에자재가 남아 있는 상태 
				}
				else if(m_npFindWorkPosYXCPB[2] == 0)
				{
					if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_TEST_RBT] < LOT_END_START) //james s2016.0911 1600
					{
						st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_TEST_RBT] =  LOT_END_START; //이제 더이상 로딩 버퍼에 해당 lot의 자재는 없다 
					}			 
				}	
			}

			if(nFlag == 0 && nTemp_Test_Place_THD_Site > 0 && st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_TEST_RBT] >=  LOT_END_START)
			{
				//로딩할 테스트사이트가 있고, 테스트 로봇이 lot end를 시작했으면, 이때부터는 로딩 자재는 리테스트 자재만 처리 가능하다 
				
				st_sync_info.nRetestBuff_Traget_THD_Work_Site  = st_lot_info[m_nLotProcessNum].nBuffer_Retest_THD_Num;
				if(st_lot_info[m_nLotProcessNum].strLotNo != st_buffer_info[st_sync_info.nRetestBuff_Traget_THD_Work_Site].strLotNo) 
				{//작업할 lot 정보와 실제 작업할 리테스트 사이트의 lot 정보가 틀리면 동작하지 말자 
					//james 2016.0918 test용으로 셋팅 st_test_site_info[st_sync_info.nRetestBuff_Traget_THD_Work_Site].strLotNo = st_lot_info[m_nLotProcessNum].strLotNo;
					break;
				} 

				///////////////////////////////////
				//test site 자재 정보 확인 
				//////////////////////////////////
				nCount = 0;
				for(nTHD_i = THD_TESTSITE_1; nTHD_i <= THD_TESTSITE_8; nTHD_i++)
				{
					if(st_test_site_info[nTHD_i].strLotNo == m_strFindLotNo) //WORK_PICK 할 수 있는 자재가 있으면 이 자재를 있으면 
					{
						for(x = 0; x < TEST_SOCKET_PARA; x++) //x 방향 소켓 수량 정보(8개, 0~7개)룰 수집한다 
						{
							if(st_test_site_info[nTHD_i].strLotNo == m_strFindLotNo && st_test_site_info[nTHD_i].st_pcb_info[x].nYesNo == CTL_YES)
							{
								nCount++;
							}
						}
					}
				}  
				////////////////////////////
				//리테스트 버퍼를 찾는다
				////////////////////////////
				nRet_1 = clsFunc.Find_TrayWork_Pos(0, st_sync_info.nRetestBuff_Traget_THD_Work_Site, CTL_YES, m_npFindWorkPosYXCPB, THD_TEST_RBT, m_strFindLotNo);
				if(nRet_1 == RET_GOOD) //리테스트 버퍼에 작업 가능한 자재가 있다
				{
					//nCount == 0 //test site에도 언로딩할 자재가 없다, //test site에도 더이상 언로딩할 자재가 없다. 이제 리테스트 버퍼에 남은 자재만 처리하면 된다 
					//2016.1023
					if( /*m_npFindWorkPosYXCPB[2] >= 7*/m_npFindWorkPosYXCPB[2] >= 4 || m_nSort_Count == 1000 || nCount == 0) //리테스트 버퍼의 자재가 1개 이상이거나, 더이상 테스트 사이트에 해당 lot의 자재가 없을떄는 마지막 리테스트 자재를 진행한다 
					{
						m_nSort_Count = 0; //james 2016.0918 추가 
						/* james 2016.1003 작업을 요청하지 않음 = CTL_REQ
						//TEST SITE 작업 위치확정 
						st_sync_info.nTestSite_Target_THD_Work_Site[2]									= nTemp_Test_Place_THD_Site; //m_nTestSite_THD_Work_Site; //작업할 테스트 사이트 정보 셋팅 						 
						st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2] = st_find_testsite_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2].nSite_Pos; //left/right
						st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3] = st_find_testsite_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2].nStart_FixPos;//fix start pos
						st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][4] = st_sync_info.nTestSite_Target_THD_Work_Site[2];//test site THD num(18~25)
						*/
						//////////////////////////////////////
						//james 2016.103 조건 통일
						//TEST SITE 작업 위치확정 
						//////////////////////////////////////
						st_sync_info.nTestSite_Target_THD_Work_Site[2]									    = nTemp_Test_Place_THD_Site; //작업할 테스트 사이트 정보 셋팅 
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
						{//에러 임
							break;
						}

						//retest buffer 작업 위치 확정 
						st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][0] = CTL_REQ;
						st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][1] = WORK_PICK; //자재를 집을 수 있게 요청 
						st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][2] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2];//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos;					 
						st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][3] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3];
						st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][4] = st_sync_info.nTestSite_Target_THD_Work_Site[2];//test site num					 

						nTHD_ID = st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][4];
						nFixPos = st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][3];

						bool bETest = false;
						//사용할 피커 정보 셋팅 
						for(i=0; i< 4; i++) //retest 버퍼 수량 4개 공간 확인  
						{
							if(nTHD_ID >= THD_TESTSITE_1 && nTHD_ID <= THD_TESTSITE_4) //죄측 테스터 ,소켓 기준은 좌측위치 부터 (1회작업)7,6,5,4, (2회작업)3,2,1,0 의 순서로 기준 소켓 위치및 정보 정의됨
							{//좌측 테스트 사이트 
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
							else if(nTHD_ID >= THD_TESTSITE_5 && nTHD_ID <= THD_TESTSITE_8) //죄측 테스터 , 소켓 기준은 좌측위치 부터  (1회작업)0,1,2,3, (2회작업)4,5,6,7 의 순서로 기준 소켓 위치및 정보 정의됨
							{//우측 테스트 사이트 
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

						//m_nRunStep = 5000;//retest buffer자재을 집어 테스트 사이트에 인서트한다
						//strTemp.Format(_T("[RETEST BUFFER1] %d"), m_nRunStep);
						//clsMem.OnNormalMessageWrite(strTemp);
						//break;	
						//2017.0224
						////////////////////////////////////////////////////////////////////////////////////////////////////////
						if(bETest == true )
						{
							m_nRunStep = 5000;//retest buffer자재을 집어 테스트 사이트에 인서트한다
							strTemp.Format(_T("[RETEST BUFFER1] %d"), m_nRunStep);
							clsMem.OnNormalMessageWrite(strTemp);
							break;
						}
//						nRet_1 = clsFunc.Find_RetestBuffer_PickPlace_WorkPos_Check(WORK_PICK, nTHD_ID, nFixPos, st_sync_info.nRetestBuff_Traget_THD_Work_Site, m_npPicker_YesNo, m_npFindWorkPosYXCPB, m_np_BuffInfo_Count);
//						if(nRet_1 == RET_GOOD)
//						{
//							m_nRunStep = 5000;//retest buffer자재을 집어 테스트 사이트에 인서트한다
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
// 								m_nRunStep = 5000;//retest buffer자재을 집어 테스트 사이트에 인서트한다
// 								strTemp.Format(_T("[RETEST BUFFER1] %d"), m_nRunStep);
// 								clsMem.OnNormalMessageWrite(strTemp);
// 								break; 
// 							}
// 						}
					}//if( m_npFindWorkPosYXCPB[2] >= 7 || nCoun == 0)
				}//if
				else
				{// //리테스트 버퍼에 작업 가능한 자재가 없다
					if(nCount == 0) //test site에도 언로딩할 자재가 없다
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
			////////////// LOT_END_START 시 retest 자재 처리 조건 

			/////////////////////////////////////////////////////////////////
			//테스터에 로딩 또는 언로딩 중 어느것을 먼저 작업할지 결정한다
			/////////////////////////////////////////////////////////////////
			if(nTemp_Test_Pick_THD_Site > 0 && nTemp_Test_Place_THD_Site > 0) 
			{//테스트 소켓에 로딩 & 언로딩이 모두 가능하면 로딩부터 작업 한다
				// jtkim 20160902
				m_nTestSite_THD_Work_Site = nTemp_Test_Place_THD_Site;

				m_nRunStep = 1200; //로딩 버퍼에서 집어 테스트 사이트에 넣는다 

				if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_TEST_RBT] ==  LOT_END_START) //james 2016.0911 로딩 자재는 더이상 없으면 테스트가 끝나는 사이트가 있으면 그 사이트의 자재를 처리하자 
				{
					m_nTestSite_THD_Work_Site	= nTemp_Test_Pick_THD_Site;
					m_nRunStep = 1100;// TEST SITE 자재 PICK 이후  언로딩 자재 소팅 까지 관리 
				}

				//2017.0319
				//strTemp.Format(_T("[1000_PICK_PLACE] Work_Site : %d Togo RunStep : %d"), m_nTestSite_THD_Work_Site, m_nRunStep);
				//clsMem.OnNormalMessageWrite(strTemp);

				/*
				if (nTemp_Test_Pick_THD_Site == nTemp_Test_Place_THD_Site)
				{
					nTemp_Test_Pick_THD_Site	= nTestSite[0];
					m_nTestSite_THD_Work_Site	= nTemp_Test_Pick_THD_Site;

					m_nRunStep = 1100;// TEST SITE 자재 PICK 이후  언로딩 자재 소팅 까지 관리 
				}
				else
				{
					m_nTestSite_THD_Work_Site = nTemp_Test_Place_THD_Site;
					
					m_nRunStep = 1200;
				}*/
				break;
			}
			else if(nTemp_Test_Place_THD_Site > 0)
			{//테스트 사이트에 넣을수 있으면 넣는것이 제일 우선 작업 순위이다 
				if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_TEST_RBT] >=  LOT_END_START) //james 2016.0918 로딩 자재는 더이상 없으면 테스트가 끝나는 사이트가 있으면 그 사이트의 자재를 처리하자 
				{
					nRet_1 = clsFunc.Find_TrayWork_Pos(0, st_sync_info.nRetestBuff_Traget_THD_Work_Site, CTL_YES, m_npFindWorkPosYXCPB, THD_TEST_RBT, m_strFindLotNo);

// 					if(nRet_1 == RET_GOOD && m_npFindWorkPosYXCPB[2] > 0)
// 					{
//						//2016.1107
// 						m_nSort_Count = 1000;
// 						break;//james 2016.0926 
// 					}
					//2017.0302
					//LOTEND_START일때 리테스트 자재가 1개라도 있으면 무조건 가지고 가면서 소켓 LOSS가 발생하는 데 좀 모으고 동작하자
					//혹시 동작 중인( 같은 랏으로 동작중인 사이트가 2개 이하라면
					nCount = 0;

					for(nTHD_i = THD_TESTSITE_1; nTHD_i <= THD_TESTSITE_8; nTHD_i++)
					{
						nSiteYesCnt = 0;
						//현재 사용가능한 사이트가 하나라도 있어야 한다 전부 disable 될 수 있으므로
						for(i = 0; i < TEST_SOCKET_PARA; i++)
						{
							if(	st_test_site_info[nTHD_i].st_pcb_info[i].nEnable == YES )
							{
								nSiteYesCnt++;
								break;
							}
						}
						if(st_test_site_info[nTHD_i].strLotNo == m_strFindLotNo && nSiteYesCnt > 0 ) //WORK_PICK 할 수 있는 자재가 있으면 이 자재를 있으면 
						{
							nCount++;
						}
					}  

					//2017.0312
					//1. 멀티 랏인경우 ( m_npFindWorkPosYXCPB[2] > 0  && nCount <=3 ) 멀티 랏인경우
					//2. 랏이 하나일때 m_npFindWorkPosYXCPB[2] > 0
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
				m_nRunStep = 1200;//로딩 버퍼에서 집어 테스트 사이트에 넣는다 

				//2017.0319
				//strTemp.Format(_T("[1000_PLACE] Work_Site : %d "), m_nTestSite_THD_Work_Site);
				//clsMem.OnNormalMessageWrite(strTemp);
				break;
			}
			else if(nTemp_Test_Pick_THD_Site > 0)
			{ //로딩할 곳이 없고, 테스트가 끝나 언로딩할 공간만 있다면 이 공간을 작업하자 
				//nTemp_Test_Pick_THD_Site	= nTestSite[0];
				m_nTestSite_THD_Work_Site	= nTemp_Test_Pick_THD_Site;
				m_nRunStep = 1100;// TEST SITE 자재 PICK 이후  언로딩 자재 소팅 까지 관리 
				//2017.0319
				strTemp.Format(_T("[1000_PICK] Work_Site : %d "), m_nTestSite_THD_Work_Site);
				clsMem.OnNormalMessageWrite(strTemp);
				break;
			}
			else
			{
				//작업할 테스트 사이트가 없다, 체크를 반복하며, 작업할 공간을 찾는다 
			}
			break;

		///////////////////////////////////////////////////////////////////////////////////////
		//TEST SITE PICK
		//테스트 언로딩 조건
		// TEST SITE 자재 PICK 이후  언로딩 자재 소팅 까지 관리 
		///////////////////////////////////////////////////////////////////////////////////////
		//1.테스트가 끝난 자재를 집어 언로딩 버퍼 또는 리테스트 버퍼에 언로딩 한다 
		//2.언로딩 작업한 테스트 소켓의 정보에 따라 그 소켓에 자재를 로딩한다 
		//3. 8개 소켓, 또는 좌(1~4), 우(5~8)사이트에 놓으면 해당 테스터를 test start 한다 
		//////////////////////////////////////////////////////////////////////////////////////
		case 1100:  //처음 해당 소켓을 작업하는 부분으로 소켓에 필요한 정보를 로딩 버퍼에 셋하여 테스터 사이트에 필요한 정보 만큼만 로딩버퍼에 공급되도록 쳇팅한다
			//load buffer에 자재 요청전 작업할 테스트 사이트 위치를 확정하고, 그 소켓 정보에 맞는 자재를 버퍼에 공급하도록 정보를 제공한다   
			//nTestSocket_0_4_Start_Pos => 0:정면에서 좌측 첫번째 소켓(0,1,2,3 socket 작업), 4:정면 조측에서 4번째 소켓(4,5,6,7 소켓 작업) 
			//sst_buffer_info[THD_LD_BUFF].st_pcb_info[i].nYesNo = CTL_YES;
			
			////////////////////////////////////////////
			//TEST SITE 작업 위치확정  //작업할 테스트 사이트 위치를 확정한다 
			//st_sync_info.nTestSite_Target_THD_Work_Site = m_nTestSite_THD_Work_Site; //작업할 테스트 사이트 정보 셋팅 
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
				if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_TEST_RBT] >=  LOT_END_START) //더이상 로딩 자재는 없으니 요청할 필요없음 
				{
					m_nRunStep = 2000;
					break;
				}
			}
			else 
			{	//lot이 구분되지 않으면 안된다 
				break;
			}

		 	//////////////////////////////////////////////////////////////////////////////////
			//테스트 사이트이 자재를 언로딩 하니, 이곳에 넣을 로딩 버퍼의 자재를 미리 요청 
			//////////////////////////////////////////////////////////////////////////////////

			/*james 2016.0926 
			TestSite_PickPlace_SocketData_LoadBuff_Map_Combine(0, st_sync_info.nTestSite_Target_THD_Work_Site[1], st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][3]); //버퍼 위치별 작업 가능여부 정보,피커의 사용 유/무 같이 셋팅

			//st_buffer_info[THD_LD_BUFF].nTestSIte_No			= st_sync_info.nTestSite_Target_THD_Work_Site[1];
			//st_buffer_info[THD_LD_BUFF].nTestSite_LR_Pos		= st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //로딩 버퍼의 테스터에 맞는 회전 각도를 정한다
			//st_buffer_info[THD_LD_BUFF].nTestSite_Start_FixPos	= st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].nStart_FixPos;

			// jtkim 20160831
			//if (st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_FREE || st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_CLEAR) 
			//{
				st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_REQ;
				st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][1] = WORK_PICK; //자재를 집을 수 있게 요청 
				st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][2] = st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][2]; //st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //로딩 버퍼의 테스터에 맞는 회전 각도를 정한다
								
				st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][3] = st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][3]; //james 2016.0924 추가 
				st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][4] = st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][4]; //james 2016.0924 추가
			//}
			*/

			if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_TEST_RBT] >=  LOT_END_START) //james2016.0929 
			{
				break;
			}

			//james 2016.0926 조건 변경 
			if(st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].strLotNo == st_lot_info[m_nLotProcessNum].strLotNo)
			{
				if (st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_FREE || st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_CLEAR) 
				{
					nRet_1 = TestSite_PickPlace_SocketData_LoadBuff_Map_Combine(0, st_sync_info.nTestSite_Target_THD_Work_Site[1], st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][3]); //버퍼 위치별 작업 가능여부 정보,피커의 사용 유/무 같이 셋팅
 		 
					if( nRet_1 == RET_GOOD )
					{
						st_sync_info.nSmema_Tray_Input_Req = CTL_YES;//테스터가 요청했다
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_REQ;
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][1] = WORK_PICK; //자재를 집을 수 있게 요청 
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][2] = st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][2]; //st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //로딩 버퍼의 테스터에 맞는 회전 각도를 정한다
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][3] = st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][3]; //james 2016.0924 추가 
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][4] = st_sync_info.nTestSite_Target_THD_Work_Site[1]; //james 2016.0924 추가

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
			}//james 2016.0926 조건 변경 

			//////////////////////////////////////////////////////////////////////////
			//2017.0430 //발주후 살릴것 -> 2017.0821 해제
// 			if(st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].strLotNo == st_lot_info[m_nLotProcessNum].strLotNo)
// 			{
// 				//일단 리테스트르 체트하자 + 언로더 자재중에 리테스트자재가 있느지 체크하자.
// 				//1.일단 사용가능한 소켓이 몇개인지 체크한다.
// 				//st_sync_info.nTestSite_Target_THD_Work_Site[1]:작업 소켓
// 
// 				//테스트사이트 언로드할때 불량 디바이스를 세어보고, 테스트 사이트에 디바이스가 있는지 확인 후 꼭 4개가 아니라도(먼저 4개이상일때만 확인하자 바쁘다)
// 				//픽업할 테스트사이트의 넣을 자재가 retest버퍼에 자재가 충분할 경우  일단, 먼저 리테스트 자재를 픽업한다.
// 
// 				int nDvc = 0, nRetestDvc = 0, nPos = 0;
// 				for(i=0; i< 4; i++) //loading 버퍼 수량 4개 
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
// 					else //if(nTestSite_THD_Num >= THD_TESTSITE_5 && nTestSite_THD_Num <= THD_TESTSITE_8) //죄측 테스터 , 소켓 기준은 좌측위치 부터  (1회작업)0,1,2,3, (2회작업)4,5,6,7 의 순서로 기준 소켓 위치및 정보 정의됨
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
// 				//리테스트 버퍼의 자재 개수를 찾는다
// 				nRet_1 = clsFunc.Find_TrayWork_Pos(0, st_sync_info.nRetestBuff_Traget_THD_Work_Site, CTL_YES, m_npFindWorkPosYXCPB, THD_TEST_RBT, m_strFindLotNo);
// 				if(nRet_1 == RET_GOOD) //리테스트 버퍼에 작업 가능한 자재가 있다
// 				{
// 					nRetestDvc = nRetestDvc + m_npFindWorkPosYXCPB[2];
// 				}
// 
// 				if( nDvc <= nRetestDvc )
// 				{//리테스트 버퍼의 자재를 픽업한다.
// 					m_nSort_Count = 1000;
// 				}
// 				else
// 				{//load buffer에 자재를 요청한다.
// 					if (st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_FREE || st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_CLEAR) 
// 					{
// 						nRet_1 = TestSite_PickPlace_SocketData_LoadBuff_Map_Combine(0, st_sync_info.nTestSite_Target_THD_Work_Site[1], st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][3]); //버퍼 위치별 작업 가능여부 정보,피커의 사용 유/무 같이 셋팅
// 
// 						if( nRet_1 == RET_GOOD )
// 						{
// 							st_sync_info.nSmema_Tray_Input_Req = CTL_YES;//테스터가 요청했다
// 							st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_REQ;
// 							st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][1] = WORK_PICK; //자재를 집을 수 있게 요청 
// 							st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][2] = st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][2]; //st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //로딩 버퍼의 테스터에 맞는 회전 각도를 정한다
// 							st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][3] = st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][3]; //james 2016.0924 추가 
// 							st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][4] = st_sync_info.nTestSite_Target_THD_Work_Site[1]; //james 2016.0924 추가
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
		// LOAD BUFFER 자재 PICK 이후 TEST SITE로 이동하여 TEST DVC PLACE/INSERT 까지 진행한다 
		// 테스트 사이트 loading 영역 
		///////////////////////////////////////////////////////////////////////////////////////
		//1.로딩 자재를 집어 테스트 소켓의 정해진 위치에 삽입한다  
		//2.해당 테스트 사이트의 소켓에 넣을 만큼 모두 넣었으면 테스터가 테스트 할 수 있게 정보를 셋한다   
		case 1200: //처음 해당 소켓을 작업하는 부분으로 소켓에 필요한 정보를 로딩 버퍼에 셋하여 테스터 사이트에 필요한 정보 만큼만 로딩버퍼에 공급되도록 쳇팅한다
			//load buffer에 자재 요청전 작업할 테스트 사이트 위치를 확정하고, 그 소켓 정보에 맞는 자재를 버퍼에 공급하도록 정보를 제공한다   
			//nTestSocket_0_4_Start_Pos => 0:정면에서 좌측 첫번째 소켓(0,1,2,3 socket 작업), 4:정면 조측에서 4번째 소켓(4,5,6,7 소켓 작업) 
			//sst_buffer_info[THD_LD_BUFF].st_pcb_info[i].nYesNo = CTL_YES;
			
			//TEST SITE 작업 위치확정 
			st_sync_info.nTestSite_Target_THD_Work_Site[2]									= m_nTestSite_THD_Work_Site; //작업할 테스트 사이트 정보 셋팅 
			 
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
			//retest bin buffer의 정보룰 확인하여 일정갯수 이상 버퍼에 존재하면 로딩 자재보다 리테스트 자재를 먼저 처리한다.
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			if(st_lot_info[m_nLotProcessNum].strLotNo == st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].strLotNo) 
			{
				st_sync_info.nRetestBuff_Traget_THD_Work_Site  = st_lot_info[m_nLotProcessNum].nBuffer_Retest_THD_Num;
			} 
			else 
			{	//lot이 구분되지 않으면 안된다 
				break;
			}

			//리테스트 버퍼를 찾는다
			m_npFindWorkPosYXCPB[2] = 0;
			nRet_1 = clsFunc.Find_TrayWork_Pos(0, st_sync_info.nRetestBuff_Traget_THD_Work_Site, CTL_YES, m_npFindWorkPosYXCPB, THD_TEST_RBT, st_worker_info.strLotNo_Empty);
			if(nRet_1 == RET_GOOD) //작업 가능한 자재가 있다
			{
				if(/*m_npFindWorkPosYXCPB[2] >= 7*/m_npFindWorkPosYXCPB[2] >= 4)//2016.1023
				{					//7개 이상이면 1차 작업구간(8개)의 자재 수량 이상이 가까운 자재가  있는것이니 로딩 자재가 아닌, 리테스트 자재를 먼저 처리하자.
					st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][0] = CTL_REQ;
					st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][1] = WORK_PICK; //자재를 집을 수 있게 요청 
					st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][2] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2];//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos;					 
					st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][3] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3];
					st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][4] = st_sync_info.nTestSite_Target_THD_Work_Site[2];//test site num

					//st_buffer_info[st_sync_info.nRetestBuff_Traget_THD_Work_Site].nTestSIte_No            = st_sync_info.nTestSite_Target_THD_Work_Site[2];
					//st_buffer_info[st_sync_info.nRetestBuff_Traget_THD_Work_Site].nTestSite_LR_Pos        = st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //로딩 버퍼의 테스터에 맞는 회전 각도를 정한다
					//st_buffer_info[st_sync_info.nRetestBuff_Traget_THD_Work_Site].nTestSite_Start_FixPos  = st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nStart_FixPos;

					nTHD_ID = st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][4];
					nFixPos = st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][3];

					//사용할 피커 정보 셋팅 
					for(i=0; i< 4; i++) //retest 버퍼 수량 4개 공간 확인  
					{
						if(nTHD_ID >= THD_TESTSITE_1 && nTHD_ID <= THD_TESTSITE_4) //죄측 테스터 ,소켓 기준은 좌측위치 부터 (1회작업)7,6,5,4, (2회작업)3,2,1,0 의 순서로 기준 소켓 위치및 정보 정의됨
						{//좌측 테스트 사이트 
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
						else if(nTHD_ID >= THD_TESTSITE_5 && nTHD_ID <= THD_TESTSITE_8) //죄측 테스터 , 소켓 기준은 좌측위치 부터  (1회작업)0,1,2,3, (2회작업)4,5,6,7 의 순서로 기준 소켓 위치및 정보 정의됨
						{//우측 테스트 사이트 
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

					m_nRunStep = 5000;//retest buffer자재을 집어 테스트 사이트에 인서트한다

					//strTemp.Format(_T("[RETEST BUFFER2] %d"), m_nRunStep);
					//clsMem.OnNormalMessageWrite(strTemp);

					break;
				}
			}
			else if(nRet_1 == RET_SKIP)  
			{	//리테스트 자재가 하나도 없다 		
				//로딩 버퍼에 자재를 요청한다
				 
			}
			/////////////////////////////////////////////////////////////////////////////////////// testest 자재 pick

			if(st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].strLotNo == st_lot_info[m_nLotProcessNum].strLotNo)
			{
				if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_TEST_RBT] >=  LOT_END_START) //더이상 로딩 자재는 없으니 요청할 필요없음 
				{
					//james 2016.0926 추가 
					nCount = 0;nRetestExistCnt = 0;
					for(x = 0; x < TEST_SOCKET_PARA; x++) //x 방향 소켓 수량 정보(8개, 0~7개)룰 수집한다 
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
					//if(nCount > 0 && nRetestExistCnt == 0 && m_npFindWorkPosYXCPB[2] == 0) //m_npFindWorkPosYXCPB[2] == 0 리테스트 버퍼에 자재가 없을때 
					if(nCount > 0 && nRetestExistCnt >= 0 && m_npFindWorkPosYXCPB[2] == 0) //m_npFindWorkPosYXCPB[2] == 0 리테스트 버퍼에 자재가 없을때 
					{//테스트 소켓에 마지막 놓은 자재를 테스트 스타트 못한 상태이니 해야 한다 
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
			{//lot이 구분되지 않으면 안된다 
				break;
			}

			//////////////////////////////////////////////////////////////////////////////////
			// 로딩 버퍼의 자재를 요청  한다 
			//////////////////////////////////////////////////////////////////////////////////
			/* //james 2016.0926 조건 변경 
			TestSite_PickPlace_SocketData_LoadBuff_Map_Combine(0, st_sync_info.nTestSite_Target_THD_Work_Site[2], st_find_testsite_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2].nStart_FixPos); //버퍼 위치별 작업 가능여부 정보,피커의 사용 유/무 같이 셋팅

			//st_buffer_info[THD_LD_BUFF].nTestSIte_No			= st_sync_info.nTestSite_Target_THD_Work_Site[2];
			//st_buffer_info[THD_LD_BUFF].nTestSite_LR_Pos		= st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //로딩 버퍼의 테스터에 맞는 회전 각도를 정한다
			//st_buffer_info[THD_LD_BUFF].nTestSite_Start_FixPos	= st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nStart_FixPos;

			// jtkim 20160831
			if (st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_FREE || st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_CLEAR) 
			{
				st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_REQ;
				st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][1] = WORK_PICK; //자재를 집을 수 있게 요청 
				st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][2] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2];//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //로딩 버퍼의 테스터에 맞는 회전 각도를 정한다
				st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][3] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3]; //james 2016.0924 추가 
				st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][4] = st_sync_info.nTestSite_Target_THD_Work_Site[2]; //test site num
			}*/

			if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_TEST_RBT] >=  LOT_END_START) //james2016.0929 
			{
				break;
			}

			//james 2016.0926 조건 변경 
			if( st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].strLotNo == st_lot_info[m_nLotProcessNum].strLotNo)
			{
				if (st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_FREE || st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_CLEAR) 
				{
					nRet_1 = TestSite_PickPlace_SocketData_LoadBuff_Map_Combine(1, st_sync_info.nTestSite_Target_THD_Work_Site[2], st_find_testsite_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2].nStart_FixPos); //버퍼 위치별 작업 가능여부 정보,피커의 사용 유/무 같이 셋팅

					if( nRet_1 == RET_GOOD )
					{
						st_sync_info.nSmema_Tray_Input_Req = CTL_YES;//테스터가 요청했다
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_REQ;
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][1] = WORK_PICK; //자재를 집을 수 있게 요청 
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][2] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2];//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //로딩 버퍼의 테스터에 맞는 회전 각도를 정한다
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][3] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3]; //james 2016.0924 추가 
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
				 m_nRunStep = 1000; //james 2016.0926 추가 
				 break;
			}

			//james 2016.0926 m_nRunStep = 6000;		
			break;


		/////////////////////////////////////////////////////////
		//테스트가 끝난 자재를 테스트 사이트에서 집어 언로딩한다 
		//TEST SITE PICK
		/////////////////////////////////////////////////////////
		case 2000:
			 //nStart_FixPos 는 물리적인(장비에서 제어을 정한 기준위치) 위치로, = 0 아니면 4 둘중 하나이다.(실제 소켓위치는 left site: 7, 3,  right site : 0, 4)
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
						nPickDvc++;//픽업한 테스터의 소켓 가능 개수
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
				// Next lot이 시작되고 현재 lot은 load 자재가 더이상 없는 상태일때 테스트 사이트를 구분(CURR LOT : NEXT LOT)하여 사용하기 위해 추가
				// 2 lot이 장비에 존재시. NEXT Lot : test #1 ~ #4까지, CURR Lot : #5 ~ #8 까지 동시에 2 Lot이 존재할 수 있어 구분하여 사용 
				if(st_lot_info[LOT_CURR].nLot_THD_Status[THD_TEST_RBT] >=  LOT_END_START && st_lot_info[LOT_NEXT].nLotStatus >= LOT_START) //현재 lot이 lot end 진행중이고, 처음 로딩 자재가 더이상 없으면 테스트 사이트도 Next Lot을 위해 Test Site #1 ~#4 까지는 모두 비워져 있으면 Nect Lot 정보로 지정한다 
				{
// 					if(st_sync_info.nTestSite_Target_THD_Work_Site[1] >= THD_TESTSITE_1 && st_sync_info.nTestSite_Target_THD_Work_Site[1] <= THD_TESTSITE_5) //로딩 lot end 이고, 좌측 테스터을 가능하면 NEXT LOT으로 지정하여 사용한다. 
					//2017.0302
					if(st_sync_info.nTestSite_Target_THD_Work_Site[1] >= THD_TESTSITE_1 && st_sync_info.nTestSite_Target_THD_Work_Site[1] <= THD_TESTSITE_8)
					{//해당 테스트 사이트에 자재가 모두 없으면 
						nCount = 0;
						for(i = 0; i < TEST_SOCKET_PARA; i++) //x 방향 소켓 수량 정보(8개, 0~7개)룰 수집한다 
						{//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].strLotNo == strLotNo && 
							if(st_lot_info[LOT_CURR].strLotNo ==  st_test_site_info[ st_sync_info.nTestSite_Target_THD_Work_Site[1]].strLotNo
								&& st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].st_pcb_info[i].nYesNo == CTL_YES)
							{
								nCount++;
							}
						}


						nRetestExistCnt = 0;
						if(nCount == 0) //이 테스트 사이트는  자재가 하나도 남아있지않다, 비어있는 상태 
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
// 									nRetestExistCnt++;//현재 retest 개수
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
// 										nDvcCount++;//현재 랏의 디바이스가 소켓에 꽂혀있는 개수
// 									}
// 								}
// 
// 
// 								for(j = 0; j < TEST_SOCKET_PARA; j++)
// 								{
// 									if(st_lot_info[LOT_CURR].strLotNo == st_test_site_info[nSite].strLotNo &&
// 										st_test_site_info[nSite].st_pcb_info[j].nEnable == YES )
// 									{
// 										nSiteExistCnt++;//현재랏이 사용가능한 테스트 사이트
// 										nEnableSokPerSite[nSite-THD_TESTSITE_1]++;
// 									}
// 								}
// 							}
							/////
							//현재 랏의 디바이스가 소켓에 꽂혀있는 개수 + 리테스트 개수
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
// 										for(i = 0; i < TEST_SOCKET_PARA; i++) //x 방향 소켓 수량 정보(8개, 0~7개)룰 수집한다 
// 										{
// 											if( st_lot_info[LOT_CURR].strLotNo == st_test_site_info[nSiteNum].strLotNo && st_test_site_info[nSiteNum].st_pcb_info[i].nYesNo == CTL_YES )
// 											{
// 												nCount++;
// 											}
// 										}
// 										//동일랏으로 소켓사용중이고 존재하지 않는 사이트가 있는가를 알아보기 위해
// 										//동일랏 사용가능한 소켓 개수를 구하고 
// 										//구한 사이트에 디바이스가 잇는체크하여 없는 부분이 존재하는지와 리테스트가 들어갈 1개사이트를 비워두고 작업한다.
// 										if( nEnableSokPerSite[nSiteNum-THD_TESTSITE_1] > 0 )
// 										{
// 											nUseLotSocket++;//동일랏 사용가느한 사이트 수
// 										}
// 										if( nEnableSokPerSite[nSiteNum] > 0 && nCount > 0)
// 										{
// 											//현재 사용중인 사이트 수
// 											nUseSocket++;//동일랏으로 사용중인 소켓
// 										}
// 									}
// 
// 									if( nRetestExistCnt >= 4) //불량이 4개 이상 발생할때만 여분을 하나더 갖고 간다.
// 									{//동일랏 사용가능한 사이트 수가 리테스존재할때 와 횬재 동작하는 사이트 수에서 1개 이상의 여분이 존재하면 true 하여 사이트를 후랏으로 변경한다.
// 										if( nUseLotSocket <= (nUseSocket + 1) )//1은 리테스트 자재가 있으면 최대 1개 사이트가 더 필요하다
// 											nNextLotPlaceSite = false;	
// 										else
// 											nNeedOneMoreSite = true;
// 									}
// 									else
// 									{
// 										if( nUseLotSocket <= nUseSocket  )
// 											nNextLotPlaceSite = false;
// 									}
// 									if( nUseSocket > 0 && nUseLotSocket <= nUseSocket && nUseLotSocket <= 1 )//같은랏 사용가능한 사이트가 1군데만 있고 리테스트가 1개이상 존재하나면 최소 1개 이상 사이트를 사용한다.
// 										nNextLotPlaceSite = false;	
// 								}
// 							}
							//if( (nDvcCount + nRetestExistCnt) <= ( nSiteExistCnt - nNumSok ) )
							//2017.0302
							//if( nNextLotPlaceSite == true )
							if( clsFunc.CheckCurrLotEndNextLot( nNeedOneMoreSite ) == RET_GOOD )
							{
								st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].strLotNo = st_lot_info[LOT_NEXT].strLotNo; //test site의 lot정보를 LOT_NEXT로 변경 셋팅하여 LOT_NEXT를 진행한다 
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
			else if(nRet_1 == RET_PICKER_NOT_FIND) //피커는 더이상 작업할 내용이 없다.  
			{//이 조건이 나오면 안됨 
				m_nRunStep = 3000;			
			}
			else if(nRet_1 == RET_TRAY_NOT_FIND) //테스터에는 더 이상 작업할 수 있는 내용이 없다
			{//이 조건이 나오면 안됨 				
				m_nRunStep = 3000;  
			}
			break;

		case 2100:
			nRet_1 = clsFunc.Find_TrayWork_Pos(0, st_sync_info.nTestSite_Target_THD_Work_Site[1], CTL_YES, m_npFindWorkPosYXCPB, THD_TEST_RBT, st_worker_info.strLotNo_Empty);
		
			if(nRet_1 == RET_SKIP || st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][3] == 4) //작업 가능한 자재가 있다
			{
				st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][0] = CTL_FREE;
			}
			m_nRunStep = 3000; 
			break;



		//////////////////////////////////////////////////
		// 집고있는 자재의 소팅 방법을 찾는다  
		//////////////////////////////////////////////////
		case 3000: //소팅 정보를 확인하여 언로딩 버퍼를 사용할지 결정한다(GOOD BIN, REJECT BIN)
			nGoodReject_Cnt = 0, nRetest_Cnt = 0;
			m_strSortDvc_LotNo = _T("");
			strFindLotNo  = _T("");
			nFlag = 0;
			
			for (i = 0; i < m_nPickerPara; i++) //4ea
			{
				if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_YES)
				{
	//2016.0906 test 용
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
							st_other_info.strBoxMsg.Format(_T("테스트 Bin정보가 없습니다. Picker No[%d], LotNo[%s], Picker LotNo[%s]. \r\n Check Please."), i, m_strSortDvc_LotNo, st_picker[m_nRobotSite].st_pcb_info[i].strLotNo);  
							clsMem.OnAbNormalMessagWrite(st_other_info.strBoxMsg);//로그 저장
							st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
						}
						st_picker[m_nRobotSite].st_pcb_info[i].nBin = BD_DATA_RETEST;
						nRetest_Cnt++;
					}
					//
					if(nFlag == 0) //strFindLotNo.IsEmpty()) //james 2016.0922 추가, 처음 자재만 lot 정보를 기록한다  
					{
						//strFindLotNo = st_picker[m_nRobotSite].st_pcb_info[i].strLotNo; //lot no를 찾는다 
						m_strSortDvc_LotNo = st_picker[m_nRobotSite].st_pcb_info[i].strLotNo; //lot no를 찾는다 
						nFlag++;

					}
					else
					{
						if(m_strSortDvc_LotNo != st_picker[m_nRobotSite].st_pcb_info[i].strLotNo) //james 2016.0922 혹시 자재가 섞일 경우를 대비하여 다음 자재 부터는 첫 자재와 lot 정보를 비교한다 
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
			{//james 2016.0922 lot 정보가 틀리면 소팅할 수 없다 
				CTL_Lib.Alarm_Error_Occurrence(2113, dWARNING, st_alarm_info.strCode); //장비 정비 시킨다 
				break;
			}

			m_nBinSort_THD_Work_Site = 0;
			if(nGoodReject_Cnt > 0) //good 또는 리젝 자재가 있다 , 언로딩 버퍼를 사용한다 
			{
				m_nBinSort_THD_Work_Site = THD_ULD_BUFF;
				m_nRunStep = 3100;  
			}
			else if(nRetest_Cnt > 0) //리테스트 버퍼에 소팅
			{
				/*
				if(st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].strLotNo == st_buffer_info[THD_RETEST_1_BUFF].strLotNo) //lot이 투입되면 버퍼, 테스트사이트, 스태커등에 어떤 공간의 정보를 lot 이름과 같이 하여 사용할지를 결정 한다
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
					//lot이 구분되지 않으면 안된다 
					break;
				}

				m_nRunStep = 3100;  
			}
			else 
			{	 
				//자재를 모두 소팅했다, 처음으로돌아가 작업을 이어간다 
				m_nRunStep = 1000;  
			}
			break;

		case 3100:
			if(m_nBinSort_THD_Work_Site == THD_ULD_BUFF) //good, reject 자재 소팅 
			{	//테스트 사이트이 자재를 언로딩 하니, 이것을 놓을 언로딩 버퍼의 자재를 미리 요청 
				st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][0] = CTL_REQ;
				st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][1] = WORK_PLACE; //자재를 집을 수 있게 요청 
				st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][2] = st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][2]; //st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //언로딩 버퍼를 테스터와 같은 1번 pin 방향을 돌려 받는다 
				st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][3] = st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][3];//james 2016.0626 
				st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][4] = st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][4];
				 
				//st_buffer_info[m_nBinSort_THD_Work_Site].nTestSIte_No				= st_sync_info.nTestSite_Target_THD_Work_Site[1];
				//st_buffer_info[m_nBinSort_THD_Work_Site].nTestSite_LR_Pos			= st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //로딩 버퍼의 테스터에 맞는 회전 각도를 정한다
				//st_buffer_info[m_nBinSort_THD_Work_Site].nTestSite_Start_FixPos		= st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].nStart_FixPos;
				//2017.0319
				strTemp.Format(_T("[3100_ULD_BUFF] Work_Site : %d Start_FixPos : %d"), st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][4], st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][3]);
				clsMem.OnNormalMessageWrite(strTemp);

			}
			else if(m_nBinSort_THD_Work_Site == THD_RETEST_1_BUFF || m_nBinSort_THD_Work_Site == THD_RETEST_2_BUFF)  //retest bin 리테스트 자재 소팅
			{	//테스트 사이트이 자재를 언로딩 하니, 이것을 놓을 언로딩 버퍼의 자재를 미리 요청 
				st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][0] = CTL_REQ;
				st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][1] = WORK_PLACE; //자재를 집을 수 있게 요청 
				st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][2] = st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][2]; // st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].nSite_Pos;
				st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][3] = st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][3]; //james 2016.0626 
				st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][4] = st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][4];

				//2017.0319
				strTemp.Format(_T("[3100_RETEST_BUFF] SIte : %d Work_Site : %d Start_FixPos : %d"), m_nBinSort_THD_Work_Site, st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][4], st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][3]);
				clsMem.OnNormalMessageWrite(strTemp);
				//st_buffer_info[m_nBinSort_THD_Work_Site].nTestSIte_No				= st_sync_info.nTestSite_Target_THD_Work_Site[1];
				//st_buffer_info[m_nBinSort_THD_Work_Site].nTestSite_LR_Pos			= st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //로딩 버퍼의 테스터에 맞는 회전 각도를 정한다
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
		//집고있는 자재의 놓을 사이트가 준비도기를 기다린다.
		//향후 PLACE() 함수 안에서 처리해야 미리 이동가능 
		//////////////////////////////////////////////////
		case 3200://언로딩 버퍼에 언로딩 자재를 놓는다 
			if(m_nBinSort_THD_Work_Site == THD_ULD_BUFF)
			{
				if(st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][0] == CTL_READY && st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][1] == WORK_PLACE)
				{//unload buffer가 로딩할 받을수 있도록 준비되어 있다 

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
				{//retest buffer가 로딩할 받을수 있도록 준비되어 있다 

					m_nRunStep = 3400;  
				}
			}
			break;

		case 3210:
			m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_ULDBUF_START];
			m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_ULDBUF_START]; // 

			nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate); 
			if (nRet_1 == BD_GOOD) //좌측으로 이동
			{				 
				m_nRunStep = 3200;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nRunStep = 3210;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
				CTL_Lib.Alarm_Error_Occurrence(2120, dWARNING, st_alarm_info.strCode);
				m_nRunStep = 3210;
			}
			break; 	


		/////////////////////////////////////////////////////////////
		//good, reject 빈 을 받는 unlaod buffer 에 자재를 놓는 영역
		//집고있는 자재의 빈에 따라 소팅 한다
		/////////////////////////////////////////////////////////////
		case 3300:
			nRet_1 = Process_DVC_Place(0, m_nBinSort_THD_Work_Site, st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][2], st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][3]); //빈에 따라 자재 소팅, 언로딩 버퍼 또는 리테스트 버퍼에 자재를 내려 놓는다 
			if(nRet_1 == RET_GOOD)
			{								
				st_sync_info.nTestRbt_Dvc_Req[THD_ULD_BUFF][0] = CTL_FREE;//자재를 로보트가 놓은 상태

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
			else if(nRet_1 == RET_PICKER_NOT_FIND) //피커는 더이상 작업할 내용이 없다.  
			{
				m_nRunStep = 3300;			
			}
			else if(nRet_1 == RET_TRAY_NOT_FIND) //테스터에는 더 이상 작업할 수 있는 내용이 없다
			{
				
				m_nRunStep = 3300;  
			}
			break; 

		////////////////////////////////////////////////////////////////////////////////////////
		// RETEST Buffer DVC Place
		// retest bin buffer의 자재를 간섭문제로 효율적으로 집는 방법을 사용하자 
		//집고있는 자재의 빈에 따라 소팅 한다
		////////////////////////////////////////////////////////////////////////////////////////
		case 3400:
			/*
			if(st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].strLotNo == st_buffer_info[THD_RETEST_1_BUFF].strLotNo) //retest buffer 1 과 이제작업할 테스트 사이트 lot 정보의 같으면 그 lot에 해당하는 자재를 처리한다 
			{
				st_sync_info.nRetestBuff_Traget_THD_Work_Site = THD_RETEST_1_BUFF;
			}
			else if(st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].strLotNo == st_buffer_info[THD_RETEST_2_BUFF].strLotNo)//retest buffer 2 과 이제작업할 테스트 사이트 lot 정보의 같으면 그 lot에 해당하는 자재를 처리한다 
			{
				st_sync_info.nRetestBuff_Traget_THD_Work_Site = THD_RETEST_2_BUFF;
			}*/

		
			//한번에 내려 놓을 수 있는 위치를 찾기 위해 정보를 확인 한다
			nTHD_ID = st_sync_info.nTestSite_Target_THD_Work_Site[WORK_PICK]; 
			nFixPos = st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][3]; //st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].nStart_FixPos;		

			for(i = 0; i < TEST_PICKER_PARA; i++) //2016.0907 test site에서 작업할 정보를 확보한다
			{
				m_npPicker_YesNo[i] = CTL_NO;//clear
				if(st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == CTL_YES)// && st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable == CTL_YES)
				{
					m_npPicker_YesNo[i] = CTL_YES; //작업할 피커만 확인한다 
				}
				/*
				if(st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][2] == TESTSITE_LEFT)
				{
					if(st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == CTL_YES)// && st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable == CTL_YES)
					{
						nFind_JobPos = (st_recipe_info.nTestSocket_Num - 1) - i ; //LEFT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는 3,2,1,0 로 반대이다 
						nFind_JobPos = nFind_JobPos - nFixPos;

						if(st_test_site_info[nTHD_ID].st_pcb_info[nFind_JobPos].nYesNo == CTL_NO )//&& st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //트레이에 자재가  있으면 집기 가능하게 셋				{
						{
							m_npPicker_YesNo[i] = CTL_YES; //작업할 피커만 확인한다 
						} 
					}
				}
				else if(st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][2] == TESTSITE_RIGHT)
				{
					if(st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == CTL_YES)// && st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable == CTL_YES)
					{
						nFind_JobPos = i ; //LEFT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는 3,2,1,0 로 반대이다 
						nFind_JobPos = nFind_JobPos + nFixPos;

						if(st_test_site_info[nTHD_ID].st_pcb_info[nFind_JobPos].nYesNo == CTL_NO)//&& st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //트레이에 자재가  있으면 집기 가능하게 셋				{
						{
							m_npPicker_YesNo[i] = CTL_YES; //작업할 피커만 확인한다 
						} 
					}
				} */
			}

			//m_npPicker_YesNo[i] == CTL_YES 인 조건을 찾아 딱 맞는 구간이 있으면 그곳의 버퍼 공간을 작업한다 
			//clsFunc.Find_RetestBinBuffer_WorkPos_Check(int n_Mode, int n_TestSite, int n_BuffSite, int n_PickerDvcYesNO,int n_BuffYesNO, int *np_Picker_YesNo, int *np_FindPosYXCPB,  int *np_BuffInfo_Count)  

			if(m_nRetestBuffer_PickPlace_Mode == 1) //james 2016.0929
			{
				nRet_1 = clsFunc.Find_RetestBuffer_PickPlace_WorkPos_Check(WORK_PLACE, nTHD_ID, nFixPos, st_sync_info.nRetestBuff_Traget_THD_Work_Site, m_npPicker_YesNo, m_npFindWorkPosYXCPB, m_np_BuffInfo_Count);//james 2016.0928 retest buffer 작업가능 위치 찾기 
			}
			else
			{
				nRet_1 = clsFunc.Find_RetestBinBuffer_WorkPos_Check(0, st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][2], st_sync_info.nRetestBuff_Traget_THD_Work_Site, CTL_YES, CTL_NO, m_npPicker_YesNo, m_npFindWorkPosYXCPB,  m_np_BuffInfo_Count);
			}
			
			if(nRet_1 == RET_GOOD)
			{
				m_nFirstTray_Y_Num =  m_npFindWorkPosYXCPB[0];// m_npFindWorkPosYXCPB[4]와 같음 
				m_nFirstTray_X_Num =  m_npFindWorkPosYXCPB[1];// m_npFindWorkPosYXCPB[4]와 같음 
				m_nBuffer_Work_Cnt =  m_npFindWorkPosYXCPB[2];//james 2016.0929 
				m_nFirstPicker_Num =  m_npFindWorkPosYXCPB[3]; 
				m_nFirst_Buff_Num  =  m_npFindWorkPosYXCPB[4]; 

				m_nPartial_FirstRetestBuffPos_Select_No = m_nFirst_Buff_Num; //리테스트 버퍼 사용 시작 위치 
				m_nRunStep = 3500;// 
			}
			else if(nRet_1 == RET_ABORT)  //james 2016.0929 작업할 자재가 없다 
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
			{	//피커 정보와 딱 맞는 리테스트 버퍼의 자재 정보가 없다 	
				//자재를 놓을떄는 한번에 놓을 공간이 없으면 안된다 
				//일당 나오면 안됨, 추후 더 디테일하게 놓을 방법 강구 할것 
				//피커 정보와 딱 맞는 리테스트 버퍼의 자재 정보가 없다 	
				//최적의 조건을 확인하여 , 그 위치를 기준으로 삼아 부족한 부분만 의 버퍼 자재를 집은 후 처리하자.

				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// Partial retest buffer의 자재들 중 제일 많이 테스트 사이트 정보와 공유된 부부을 정해 이 부분의 자재를 집어 테스트 테스트 사이트로 이동한다
				// 우선 이조건은 나중에 필요시 하자 
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//james 2016.0929 파셜일때도 작업 가능하게 수정 
				nMaxCnt=0;
				if(st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][2]  == TESTSITE_LEFT)
				{
					for(i = st_recipe_info.nRetestBuffer_Num - 1; i >= 0; i--)// st_recipe_info.nRetestBuffer_Num; i++)//15
					{
						nRetest_Cnt = m_np_BuffInfo_Count[i] ;
						if(nMaxCnt < nRetest_Cnt)
						{
							nMaxCnt = nRetest_Cnt;
							m_nFirst_Buff_Num = i; //제일 많은 수량 근사치의 정보의 가지고 있으면 이것을 기준 위치로 작업한다 
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
							m_nFirst_Buff_Num = i; //제일 많은 수량 근사치의 정보의 가지고 있으면 이것을 기준 위치로 작업한다 
						}
					}
				}				
				m_nPartial_FirstRetestBuffPos_Select_No = m_nFirst_Buff_Num; //partial 리테스트 버퍼의 공간에 자재를 공급하기 위해 선정된 버퍼 위치 

				nFlag = CTL_NO;
				for(i = 0; i < TEST_PICKER_PARA; i++) //test site에서 작업할 정보를 확보한다
				{
					m_npPicker_YesNo[i] = CTL_NO;//clear
					if(st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == CTL_YES) //
					{
						if(st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][2]  == TESTSITE_LEFT) // st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos == TESTSITE_LEFT) //test socket left 실제소켓정보 :7->6->5->4  3->2->1->0
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
						else //if(st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][2]  == TESTSITE_RIGHT) //st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].nSite_Pos == TESTSITE_RIGHT) //test socket right 실제소켓정보 :0->1->2->3  4->5->6->7
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
					//집을 자재만 먼저 집고, 다시 처음 기준 위치의 자재를 추가로 집어 테스트 사이트로 이동한다.
					m_nFirstTray_Y_Num = m_nPartial_FirstRetestBuffPos_Select_No;// m_npFindWorkPosYXCPB[4]와 같음 
					m_nFirstTray_X_Num = m_nPartial_FirstRetestBuffPos_Select_No;// m_npFindWorkPosYXCPB[4]와 같음 
					m_nFirstPicker_Num = m_nFirstPicker_Num; 
					m_nFirst_Buff_Num  = m_nPartial_FirstRetestBuffPos_Select_No; 

					m_npFindWorkPosYXCPB[0] = m_nFirstTray_Y_Num; 
					m_npFindWorkPosYXCPB[1] = m_nFirstTray_X_Num; 
					m_npFindWorkPosYXCPB[3] = m_nFirstPicker_Num;
					m_npFindWorkPosYXCPB[4] = m_nFirst_Buff_Num;

					m_nRunStep = 3500;//현재 위치의 자재를 집어 작업할 테스트 사이트로 이동한다
				}
				else
				{//이미 자재가 있는것을 알고 왔으므로 이곳으로 올수는 없음
					m_nRunStep = 3400; //james 2016.0929 1000;
				}
			}
			break;

		case 3500:
			nRet_1 = Process_DVC_Place(BIN_RETESTBIN, m_nBinSort_THD_Work_Site, st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][2], st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[1]][3]); //빈에 따라 자재 소팅, 언로딩 버퍼 또는 리테스트 버퍼에 자재를 내려 놓는다 
			
			if(nRet_1 == RET_GOOD)
			{					
				m_nRunStep = 3600;
			}
			else if(nRet_1 == RET_PICKER_NOT_FIND) //피커는 더이상 작업할 내용이 없다.  
			{
				m_nRunStep = 3500;			
			}
			else if(nRet_1 == RET_TRAY_NOT_FIND) //테스터에는 더 이상 작업할 수 있는 내용이 없다
			{
				
				m_nRunStep = 3500;  
			}
			break; 
		///////////////////////////////////////////////////

		// 언로딩 자재를 모두 처리한 상태
		// lot end 조건을 확인한다
		///////////////////////////////////////////////////
		case 3600:
			nRetestExistCnt=0;
			for(i = 0; i < TEST_PICKER_PARA; i++) //test site에서 작업할 정보를 확보한다
			{					 
				if(st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == CTL_YES)// && st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable == CTL_YES)
				{
					 nRetestExistCnt++;
				} 
			}
			if(nRetestExistCnt > 0) //자재가 남아있으면 
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
			//현재 lot이 lot end 진행중이고, 처음 로딩 자재가 더이상 없으면 테스트 사이트도 Next Lot을 위해 Test Site #1 ~#4 까지는 모두 비워져 있으면 Next Lot 정보로 지정한다 
			if(st_lot_info[LOT_CURR].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END_START && st_lot_info[LOT_NEXT].nLotStatus == LOT_START) 
			{
				if(st_sync_info.nTestSite_Target_THD_Work_Site[1] >= THD_TESTSITE_1 && st_sync_info.nTestSite_Target_THD_Work_Site[1] <= THD_TESTSITE_4) //로딩 lot end 이고, 좌측 테스터을 가능하면 NEXT LOT으로 지정하여 사용한다. 
				{//해당 테스트 사이트에 자재가 모두 없으면 
					nCoun = 0;
					for(nTHD_i = 0; nTHD_i <  8; nTHD_i++)
					{
						for(i = 0; i < TEST_SOCKET_PARA; i++) //x 방향 소켓 수량 정보(8개, 0~7개)룰 수집한다 
						{//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].strLotNo == strLotNo && 
							if(st_test_site_info[nTHD_i].strLotNo ==  st_lot_info[LOT_CURR].strLotNo && st_test_site_info[nTHD_i].st_pcb_info[i].nYesNo == CTL_YES)
							{
								nCoun++;
							}
						}
					}
					 
					for(i = 0; i <  15; i++)  //리테스트 버퍼에 남은 자재를 확인한다
					{
						if(st_buffer_info[st_sync_info.nRetestBuff_Traget_THD_Work_Site].st_pcb_info[i].nYesNo == CTL_YES) 
						{
							nCoun++;
						}
					}

					//////////////////////////////////////////////////////////////////////////////////////////////////////////
					// 이제 모든 테스트에 LOT_CURR 의 자재는 하나도 남아있지않다 
					// Test Robot 기준 모든 자재를 처리하여 test site lot정보를 모두 NEXT LOT 정보로 교체한다
					//////////////////////////////////////////////////////////////////////////////////////////////////////////
					if(nCoun == 0) 
					{
						 st_lot_info[LOT_CURR].nLot_THD_Status[THD_TEST_RBT] =  LOT_END_START;

						 st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].strLotNo = st_lot_info[LOT_NEXT].strLotNo; //더이상 CURR_LOT은 없으니 전체 LOT END 전이라도 테스트 사이트의 LOT 정보를 NEXT LOT으로 모두 지정한다
					 
					}
				}
			}

			m_nRunStep = 1000;  
			break;	*/	

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//james 2016.0816 리테스트 방법 정의
//자재 pick & place 방법 (피커가 전체 Up/Down으로 동작시 자재간 충돌이 발생할 수 있어 많은 주의가 필요하다 //
//1차 작업구간 : 0,2,4,6, 8,10,12,14 - 8개 
//2차 작업구간 : 1,3,5,7, 9,11,13    - 7개
//
//리테스트 버퍼사용방법: 한 LOT에 하나의 리테스트 버퍼만 사용한다, 연속LOT 처리 때문
// Left/Right Tets Site에서 나오는 자재의 PIN 방향이 반대여서 어느 사이트에서 온것인지 확인하여 회전하면서 받아야 한다 
// 이때 받은 버퍼는 L/R 구분없이 해당 lot은 한곳에 모두 받아, 처리한다.    
//
// PLACE 방법 놓을때.
			//1차안
// 1. 1차 작업구간(짝수열) 까지는 무조건 한번에 놓을 공간을 찾아 놓는다. 
// 2. 2차 작업구간(홀수열) 까지 자재를 넣을 상황이면 , 이것 까지만 놓고, 이제는 1차 작업 구간의 자재를 리테스트를 진행한다 
// 3. 매 사이클 자재 언로딩 소팅후, 혹시 홀수열의 리테스트 버퍼에 자재가 있으면 이 자재를 짝수열로 최대한 정렬해 준다 
// 4. 즉 홀수열은 임시 버퍼 공간으로 무조건 비워져 있어야 한다 

		//2차안
//1차(짝수열),2차(홀수열) 작업 구간 상관없이 모든 조건을 수용한다
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////////////////////
		// RETEST Buffer DVC PICK
		// retest bin buffer의 자재를 간섭문제로 효율적으로 집는 방법을 사용하자 
		////////////////////////////////////////////////////////////////////////////////////////
		case 5000:
			//james 2016.0929 			
			if(m_nRetestBuffer_PickPlace_Mode == 1)
			{
				m_nRunStep = 5600;  
				break;
			}
		

			//작업할 테스트사이트의 lot 정보와 retest buffer 작업 lot 정보를 확인하여 작업할 버퍼를 확정한다, 위에서 이미 찾아 온다 
			/*if(st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].strLotNo == st_buffer_info[THD_RETEST_1_BUFF].strLotNo) //retest buffer 1 과 이제작업할 테스트 사이트 lot 정보의 같으면 그 lot에 해당하는 자재를 처리한다 
			{
				st_sync_info.nRetestBuff_Traget_THD_Work_Site = THD_RETEST_1_BUFF;
			}
			else if(st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].strLotNo == st_buffer_info[THD_RETEST_2_BUFF].strLotNo)//retest buffer 2 과 이제작업할 테스트 사이트 lot 정보의 같으면 그 lot에 해당하는 자재를 처리한다 
			{
				st_sync_info.nRetestBuff_Traget_THD_Work_Site = THD_RETEST_2_BUFF;
			}*/
		
			nTHD_ID = st_sync_info.nTestSite_Target_THD_Work_Site[2]; 
			nFixPos = st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][3]; //2016.0906 st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3];// st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nStart_FixPos;		
			for(i = 0; i < TEST_PICKER_PARA; i++) //4, test site에서 작업할 정보를 확보한다
			{
				m_npPicker_YesNo[i] = CTL_NO;//clear
				if(st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == CTL_NO && st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable == CTL_YES)
				{
					if(st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][2]  == TESTSITE_LEFT) //st_test_site_info[nTHD_ID].nSite_Pos == TESTSITE_LEFT) //test socket left 실제소켓정보 :7->6->5->4  3->2->1->0
					{ 
						nPos = ((TEST_SOCKET_PARA - 1) - nFixPos) - i;
						if(st_test_site_info[nTHD_ID].st_pcb_info[nPos].nYesNo == CTL_NO && st_test_site_info[nTHD_ID].st_pcb_info[nPos].nEnable == CTL_YES)
						{
							m_npPicker_YesNo[i] = CTL_YES;
						}
						else
						{
							st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable = CTL_NO; //피커제거 
						}
					}
					else if(st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][2] == TESTSITE_RIGHT) // st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].nSite_Pos == TESTSITE_RIGHT) //test socket right 실제소켓정보 :0->1->2->3  4->5->6->7
					{ 
						nPos = nFixPos + i;
						if(st_test_site_info[nTHD_ID].st_pcb_info[nPos].nYesNo == CTL_NO && st_test_site_info[nTHD_ID].st_pcb_info[nPos].nEnable == CTL_YES)
						{
							m_npPicker_YesNo[i] = CTL_YES;
						}
						else
						{
							st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable = CTL_NO; //피커제거 
						}
					}
				}
			}//for

			//m_npPicker_YesNo[i] == CTL_YES 인 조건을 찾아 딱 맞는 구간이 있으면 그곳의 버퍼 공간을 작업한다 
			//clsFunc.Find_RetestBinBuffer_WorkPos_Check(int n_Mode, int n_TestSite, int n_BuffSite, int n_PickerDvcYesNO,int n_BuffYesNO, int *np_Picker_YesNo, int *np_FindPosYXCPB,  int *np_BuffInfo_Count)  
			nRet_1 = clsFunc.Find_RetestBinBuffer_WorkPos_Check(0, st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][2], st_sync_info.nRetestBuff_Traget_THD_Work_Site, CTL_YES, CTL_YES, m_npPicker_YesNo, m_npFindWorkPosYXCPB, m_np_BuffInfo_Count);
			
			if(nRet_1 == RET_GOOD)
			{ 
				m_nFirstTray_Y_Num =  m_npFindWorkPosYXCPB[0];// m_npFindWorkPosYXCPB[4]와 같음 
				m_nFirstTray_X_Num =  m_npFindWorkPosYXCPB[1];// m_npFindWorkPosYXCPB[4]와 같음 
				m_nFirstPicker_Num = m_npFindWorkPosYXCPB[3]; //nFirstPicker_Num
				m_nFirst_Buff_Num  = m_npFindWorkPosYXCPB[4]; //nSortBuffer_First

				m_nPartial_FirstRetestBuffPos_Select_No = m_nFirst_Buff_Num;
				m_nRunStep = 5200;//현재 위치의 자재를 집어 작업할 테스트 사이트로 이동한다
			}
			else if(nRet_1 == RET_SKIP)  
			{	//피커 정보와 딱 맞는 리테스트 버퍼의 자재 정보가 없다 	
				//최적의 조건을 확인하여 , 그 위치를 기준으로 삼아 부족한 부분만 의 버퍼 자재를 집은 후 처리하자.
			 
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// Partial retest buffer의 자재들 중 제일 많이 테스트 사이트 정보와 공유된 부부을 정해 이 부분의 자재를 집어 테스트 테스트 사이트로 이동한다
				// 우선 이조건은 나중에 필요시 하자 
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
							m_nFirst_Buff_Num = i; //제일 많은 수량 근사치의 정보의 가지고 있으면 이것을 기준 위치로 작업한다 
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
							m_nFirst_Buff_Num = i; //제일 많은 수량 근사치의 정보의 가지고 있으면 이것을 기준 위치로 작업한다 
						}
					}
				}				
				m_nPartial_FirstRetestBuffPos_Select_No = m_nFirst_Buff_Num; //partial 리테스트 버퍼의 공간에 자재를 공급하기 위해 선정된 버퍼 위치 

				nFlag = CTL_NO;
				for(i = 0; i < TEST_PICKER_PARA; i++) //test site에서 작업할 정보를 확보한다
				{
					m_npPicker_YesNo[i] = CTL_NO;//clear
					if(st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == CTL_NO && st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable == CTL_YES) //피커제거 
					{
						if(st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2]  == TESTSITE_LEFT) // st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos == TESTSITE_LEFT) //test socket left 실제소켓정보 :7->6->5->4  3->2->1->0
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
						else //if(st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2]  == TESTSITE_RIGHT) //st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].nSite_Pos == TESTSITE_RIGHT) //test socket right 실제소켓정보 :0->1->2->3  4->5->6->7
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
					//집을 자재만 먼저 집고, 다시 처음 기준 위치의 자재를 추가로 집어 테스트 사이트로 이동한다.
					m_nFirstTray_Y_Num = m_nPartial_FirstRetestBuffPos_Select_No;// m_npFindWorkPosYXCPB[4]와 같음 
					m_nFirstTray_X_Num = m_nPartial_FirstRetestBuffPos_Select_No;// m_npFindWorkPosYXCPB[4]와 같음 
					m_nFirstPicker_Num = m_nFirstPicker_Num; 
					m_nFirst_Buff_Num  = m_nPartial_FirstRetestBuffPos_Select_No; 

					m_npFindWorkPosYXCPB[0] = m_nFirstTray_Y_Num; 
					m_npFindWorkPosYXCPB[1] = m_nFirstTray_X_Num; 
					m_npFindWorkPosYXCPB[3] = m_nFirstPicker_Num;
					m_npFindWorkPosYXCPB[4] = m_nFirst_Buff_Num;

					m_nRunStep = 5200;//현재 위치의 자재를 집어 작업할 테스트 사이트로 이동한다
				}
				else
				{//이미 자재가 있는것을 알고 왔으므로 이곳으로 올수는 없음
					m_nRunStep = 5600; //james 2016.0929 1000;
				}
			}
			break;

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// retest bin의 자재를 집는 mode 이고,
		// m_nFirstPicker_Num, m_nFirst_Buff_Num  위치는 이미 정리하여 우선 아래 PICK() 함수 에서 이 부분을 찾고 재정의하는 Step 부부은 스킵되어야 한다 
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		case 5200:
			st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][0] = CTL_REQ;
			st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][1] = WORK_PICK; //자재를 집을 수 있게 요청 
			st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][2] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2]; //st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos;

			//st_buffer_info[st_sync_info.nRetestBuff_Traget_THD_Work_Site].nTestSIte_No            = st_sync_info.nTestSite_Target_THD_Work_Site[2];
			//st_buffer_info[st_sync_info.nRetestBuff_Traget_THD_Work_Site].nTestSite_LR_Pos        = st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //로딩 버퍼의 테스터에 맞는 회전 각도를 정한다
			//st_buffer_info[st_sync_info.nRetestBuff_Traget_THD_Work_Site].nTestSite_Start_FixPos  = st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nStart_FixPos;
			m_nRunStep = 5300;
			break;

		case 5300: //버퍼가 주니되기를 기다린다 
			if(st_sync_info.nRetestBuff_Traget_THD_Work_Site == THD_RETEST_1_BUFF || st_sync_info.nRetestBuff_Traget_THD_Work_Site == THD_RETEST_2_BUFF) 
			{
				if(st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][0] == CTL_READY && st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][1] == WORK_PICK)
				{//retest buffer에 로딩할 자재가 준비되어 있다 
					m_nRunStep = 5400;  
				}
			}
			
			/* james 2016.0918 
			if(m_nBinSort_THD_Work_Site == THD_RETEST_1_BUFF || m_nBinSort_THD_Work_Site == THD_RETEST_2_BUFF) 
			{
				if(st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][0] == CTL_READY && st_sync_info.nTestRbt_Dvc_Req[m_nBinSort_THD_Work_Site][1] == WORK_PICK)
				{//retest buffer에 로딩할 자재가 준비되어 있다 
					m_nRunStep = 5400;  
				}
			}*/
			break;

		case 5400://리테스트 자재를 집는다
			nRet_1 = Process_DVC_Pick(BIN_RETESTBIN, st_sync_info.nRetestBuff_Traget_THD_Work_Site, st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2], st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3]);//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nStart_FixPos);
			
			if(nRet_1 == RET_GOOD)
			{					
				m_nRunStep = 7000;
			}
			else if(nRet_1 == RET_PICKER_NOT_FIND) //피커는 더이상 작업할 내용이 없다.  
			{
				m_nRunStep = 5400;			
			}
			else if(nRet_1 == RET_TRAY_NOT_FIND) //테스터에는 더 이상 작업할 수 있는 내용이 없다
			{
				
				m_nRunStep = 5400;  
			}
			break;

//////////////////////////////////////////////////////////////////////
		//james 2016.0929 
			//retest buffer pick & Place  방법 변경 
//////////////////////////////////////////////////////////////////////
		case 5600:

			nTHD_ID = st_sync_info.nTestSite_Target_THD_Work_Site[2]; 
			nFixPos = st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][3]; //2016.0906 st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3];// st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nStart_FixPos;		
			//break;

			//Find_RetestBuffer_PickPlace_WorkPos_Check(int n_PickPlace_Mode, int n_TestNum, int n_TestFixPos, int n_BuffSite, int *np_Picker_YesNo, int *np_FindPosYXCPB,  int *np_BuffInfo_Count)//james 2016.0928 retest buffer 작업가능 위치 찾기 
			nRet_1 = clsFunc.Find_RetestBuffer_PickPlace_WorkPos_Check(WORK_PICK, nTHD_ID, nFixPos, st_sync_info.nRetestBuff_Traget_THD_Work_Site, m_npPicker_YesNo, m_npFindWorkPosYXCPB, m_np_BuffInfo_Count);//james 2016.0928 retest buffer 작업가능 위치 찾기 
			if(nRet_1 == RET_GOOD)
			{ 
				m_nFirstTray_Y_Num =  m_npFindWorkPosYXCPB[0];// m_npFindWorkPosYXCPB[4]와 같음 
				m_nFirstTray_X_Num =  m_npFindWorkPosYXCPB[1];// m_npFindWorkPosYXCPB[4]와 같음 
				m_nBuffer_Work_Cnt =  m_npFindWorkPosYXCPB[2];//james 2016.0929 
				m_nFirstPicker_Num = m_npFindWorkPosYXCPB[3]; //nFirstPicker_Num
				m_nFirst_Buff_Num  = m_npFindWorkPosYXCPB[4]; //nSortBuffer_First

				m_nPartial_FirstRetestBuffPos_Select_No = m_nFirst_Buff_Num;
				m_nRunStep = 5700;//현재 위치의 자재를 집어 작업할 테스트 사이트로 이동한다
			}
			else if(nRet_1 == RET_ABORT)  //james 2016.0929 작업할 자재가 없다 
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
					clsMem.OnAbNormalMessagWrite(strTemp);//로그 저장
					m_nRunStep = 1000;
				}
			}
			else if(nRet_1 == RET_SKIP)  
			{	//피커 정보와 딱 맞는 리테스트 버퍼의 자재 정보가 없다 	
				//최적의 조건을 확인하여 , 그 위치를 기준으로 삼아 부족한 부분만 의 버퍼 자재를 집은 후 처리하자.

				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// Partial retest buffer의 자재들 중 제일 많이 테스트 사이트 정보와 공유된 부부을 정해 이 부분의 자재를 집어 테스트 테스트 사이트로 이동한다
				// 우선 이조건은 나중에 필요시 하자 
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
							m_nFirst_Buff_Num = i; //제일 많은 수량 근사치의 정보의 가지고 있으면 이것을 기준 위치로 작업한다 
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
							m_nFirst_Buff_Num = i; //제일 많은 수량 근사치의 정보의 가지고 있으면 이것을 기준 위치로 작업한다 
						}
					}
				}				
				m_nPartial_FirstRetestBuffPos_Select_No = m_nFirst_Buff_Num; //partial 리테스트 버퍼의 공간에 자재를 공급하기 위해 선정된 버퍼 위치 

				nFlag = CTL_NO;
				int nPickerNum = 0;
				for(i = 0; i < TEST_PICKER_PARA; i++) 
				{
					if(st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == CTL_NO && st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable == CTL_YES) //피커제거 
					{
						nPickerNum = i;
						break;
					}
				}
				for(i = 0; i < TEST_PICKER_PARA; i++) //test site에서 작업할 정보를 확보한다
				{
					m_npPicker_YesNo[i] = CTL_NO;//clear
					if(st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == CTL_NO && st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable == CTL_YES) //피커제거 
					{
						if(st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2]  == TESTSITE_LEFT) // st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos == TESTSITE_LEFT) //test socket left 실제소켓정보 :7->6->5->4  3->2->1->0
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
						else //if(st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2]  == TESTSITE_RIGHT) //st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].nSite_Pos == TESTSITE_RIGHT) //test socket right 실제소켓정보 :0->1->2->3  4->5->6->7
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
					//집을 자재만 먼저 집고, 다시 처음 기준 위치의 자재를 추가로 집어 테스트 사이트로 이동한다.
					m_nFirstTray_Y_Num = m_nPartial_FirstRetestBuffPos_Select_No;// m_npFindWorkPosYXCPB[4]와 같음 
					m_nFirstTray_X_Num = m_nPartial_FirstRetestBuffPos_Select_No;// m_npFindWorkPosYXCPB[4]와 같음 
					m_nFirstPicker_Num = m_nFirstPicker_Num; 
					m_nFirst_Buff_Num  = m_nPartial_FirstRetestBuffPos_Select_No; 

					m_npFindWorkPosYXCPB[0] = m_nFirstTray_Y_Num; 
					m_npFindWorkPosYXCPB[1] = m_nFirstTray_X_Num; 
					m_npFindWorkPosYXCPB[3] = m_nFirstPicker_Num;
					m_npFindWorkPosYXCPB[4] = m_nFirst_Buff_Num;

					m_nRunStep = 5700;//현재 위치의 자재를 집어 작업할 테스트 사이트로 이동한다
				}
				else
				{//이미 자재가 있는것을 알고 왔으므로 이곳으로 올수는 없음
					m_nRunStep = 5600; //james 2016.0929 1000;
				}
			}
			break;

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// retest bin의 자재를 집는 mode 이고,
		// m_nFirstPicker_Num, m_nFirst_Buff_Num  위치는 이미 정리하여 우선 아래 PICK() 함수 에서 이 부분을 찾고 재정의하는 Step 부부은 스킵되어야 한다 
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		case 5700:
			st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][0] = CTL_REQ;
			st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][1] = WORK_PICK; //자재를 집을 수 있게 요청 
			st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][2] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2]; //st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos;

			//st_buffer_info[st_sync_info.nRetestBuff_Traget_THD_Work_Site].nTestSIte_No            = st_sync_info.nTestSite_Target_THD_Work_Site[2];
			//st_buffer_info[st_sync_info.nRetestBuff_Traget_THD_Work_Site].nTestSite_LR_Pos        = st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //로딩 버퍼의 테스터에 맞는 회전 각도를 정한다
			//st_buffer_info[st_sync_info.nRetestBuff_Traget_THD_Work_Site].nTestSite_Start_FixPos  = st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nStart_FixPos;
			m_nRunStep = 5800;
			break;

		case 5800: //버퍼가 주니되기를 기다린다 
			if(st_sync_info.nRetestBuff_Traget_THD_Work_Site == THD_RETEST_1_BUFF || st_sync_info.nRetestBuff_Traget_THD_Work_Site == THD_RETEST_2_BUFF) 
			{
				if(st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][0] == CTL_READY && st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][1] == WORK_PICK)
				{//retest buffer에 로딩할 자재가 준비되어 있다 
					m_nRunStep = 5900;  
				}
			}
			break;

		case 5900://리테스트 자재를 집는다
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
					m_nRunStep = 5600; //남은 자재를 집는다 
					//2017.1206  체크바람
					if( nCount > 0 && st_basic_info.nRetest1Pick_Mode == CTL_YES)
					{
						m_nRunStep = 7000;
					}
				}
			}
			else if(nRet_1 == RET_PICKER_NOT_FIND) //피커는 더이상 작업할 내용이 없다.  
			{
				m_nRunStep = 5900;			
			}
			else if(nRet_1 == RET_TRAY_NOT_FIND) //테스터에는 더 이상 작업할 수 있는 내용이 없다
			{
				
				m_nRunStep = 5900;  
			}
			break;

		
		///////////////////////////////////////////////////////////////////////////////////////
		// LOAD BUFFER 자재 PICK 이후 TEST SITE로 이동하여 TEST DVC INSERT 까지 진행한다 
		//1.로딩 자재를 집어 테스트 소켓의 정해진 위치에 삽입한다  
		//2.해당 테스트 사이트의 소켓에 넣을 만큼 모두 넣었으면 테스터가 테스트 할 수 있게 정보를 셋한다   
		//////////////////////////////////////////////////////////////////////////////////////
		case 6000:
			m_nMove_Flag[0] = 0;
			m_nRunStep = 6010;
			break;

		case 6010:

			nRet_3 = clsFunc.Find_TrayWork_Pos(WORK_PICK, THD_LD_BUFF, CTL_YES, m_npFindWorkPosYXCPB, THD_TEST_RBT, m_strFindLotNo);

			if(nRet_3 == RET_GOOD && m_npFindWorkPosYXCPB[2] > 0 && st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] == CTL_READY && st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][1] == WORK_PICK)
			{//load buffer에 로딩할 자재가 준비되어 있다 
				//james 2016.1004 m_nRunStep = 6100;  

				////////////////////////////////////////////////////////////////////////////////////////
				// james 2016.100 혹시 로딩 로테이터의 방향이 틀리면 돌려주자, 문제는 찾을 것???
				// 로딩 버퍼의 준비를 미리 확인한다 
				////////////////////////////////////////////////////////////////////////////////////////
				if(st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2] == TESTSITE_LEFT) //LEFT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는 3,2,1,0 로 반대이다 
				{
					nRet_1 = COMI.Check_MotPosRange(M_LD_BUF_SHIFTER_Y, st_motor_info[M_LD_BUF_SHIFTER_Y].d_pos[P_LDBUFF_Y_TESTRBT], st_motor_info[M_LD_BUF_SHIFTER_Y].d_allow);
					nRet_2 = COMI.Check_MotPosRange(M_LD_BUF_ROTATOR, st_motor_info[M_LD_BUF_ROTATOR].d_pos[P_LDBUFF_RTR_TESTSITE_LEFT], st_motor_info[M_LD_BUF_ROTATOR].d_allow);
					if(nRet_1 != BD_GOOD || nRet_2 != BD_GOOD)
					{
// 						if(nRet_1 != BD_GOOD ) m_strAlarmCode.Format(_T("%02d0006"), M_LD_BUF_SHIFTER_Y); 
// 						else m_strAlarmCode.Format(_T("%02d0006"), M_LD_BUF_ROTATOR); 
// 						CTL_Lib.Alarm_Error_Occurrence(m_nPlace_Step, dWARNING, m_strAlarmCode);
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][0] = CTL_REQ;
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][1] = 300; //james 2016.1005 WORK_PICK; //자재를 집을 수 있게 요청 
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][2] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2];//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //로딩 버퍼의 테스터에 맞는 회전 각도를 정한다
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][3] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3]; //james 2016.0924 추가 
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][4] = st_sync_info.nTestSite_Target_THD_Work_Site[2]; //test site num

						//2017.0319
						strTemp.Format(_T("[6010TESTSITE_LEFT_LD_BUFF_REQ] Work_Site : %d Start_FixPos : %d"), st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][4] , st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][3]);
						clsMem.OnNormalMessageWrite(strTemp);
						break;
					} 
				}
				else if(st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2] == TESTSITE_RIGHT) //right sitet를 작업하기 위한 작업 중일때//RIGHT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는0,1,2,3 로 위치 순서가 동일하다  
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
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][1] = 300; //james 2016.1005 WORK_PICK; //자재를 집을 수 있게 요청 
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][2] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2];//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT , //로딩 버퍼의 테스터에 맞는 회전 각도를 정한다
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][3] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3]; //james 2016.0924 추가 
						st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][4] = st_sync_info.nTestSite_Target_THD_Work_Site[2]; //test site num
						//2017.0319
						strTemp.Format(_T("[6010TESTSITE_RIGHT_LD_BUFF_REQ] Work_Site : %d Start_FixPos : %d"), st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][4] , st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][3]);
						clsMem.OnNormalMessageWrite(strTemp);
						break;
					} 
				}//james 2016.1004 
				else
				{
					// 이곳으로 오면 조건 에러 임, 문제 찾을 것
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



				//james 2016.0923 loading lot end가 시작 되었으면 더 이상 로딩 자재를 집을 이유가 없다 
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
			if (nRet_1 == BD_GOOD) //좌측으로 이동
			{				 
				m_nRunStep = 6010;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nRunStep = 6020;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
				CTL_Lib.Alarm_Error_Occurrence(2121, dWARNING, st_alarm_info.strCode);
				m_nRunStep = 6020;
			}
			break; 	

		case 6100:
			//james 2016.0930 nRet_1 = Process_DVC_Pick(0, THD_LD_BUFF, st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2], st_sync_info.nTestRbt_Dvc_UnLd_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3]);//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos); //로딩 버퍼의 자재를 집는다
			nRet_1 = Process_DVC_Pick(0, THD_LD_BUFF, st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2], st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3]);//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nSite_Pos); //로딩 버퍼의 자재를 집는다
			
			if(nRet_1 == RET_GOOD)
			{
				//2017.1121 센서 체크
				if (st_basic_info.nMissLoading_Mode == CTL_YES)
				{
					nRet_1 = FAS_IO.get_in_bit(st_io_info.i_MissLoadingChk1, IO_ON);
					nRet_2 = FAS_IO.get_in_bit(st_io_info.i_MissLoadingChk2, IO_ON);
					if(nRet_1 == IO_OFF || nRet_2 == IO_OFF) 
					{
						if(nRet_1 == IO_OFF ) m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_MissLoadingChk1); //Miss loading 센서 에러
						else						   m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_MissLoadingChk2); //Miss loading 센서 에러
						CTL_Lib.Alarm_Error_Occurrence(2199, dWARNING, m_strAlarmCode);
					}
				}

				m_nRunStep = 7000;
			}
			else if(nRet_1 == RET_PICKER_NOT_FIND) //피커는 더이상 작업할 내용이 없다.  
			{
				m_nRunStep = 6100;
			}
			else if(nRet_1 == RET_TRAY_NOT_FIND) //테스터에는 더 이상 작업할 수 있는 내용이 없다
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
			//test site left/right 설정 

			st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][0] = CTL_REQ;
			st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][1] = WORK_PLACE; //자재를 집을 수 있게 요청 
			st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][2] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2]; // st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[1]].nSite_Pos;
			st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][3] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3]; //james 2016.0626 
			st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nRetestBuff_Traget_THD_Work_Site][4] = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][4];


			nTHD_ID = st_sync_info.nTestSite_Target_THD_Work_Site[2];//st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2];//버퍼 방향
			nFixPos = st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3];
			nRet_1 = clsFunc.Find_RetestBuffer_PickPlace_WorkPos_Check(WORK_PLACE, nTHD_ID, nFixPos, st_sync_info.nRetestBuff_Traget_THD_Work_Site, m_npPicker_YesNo, m_npFindWorkPosYXCPB, m_np_BuffInfo_Count);//james 2016.0928 retest buffer 작업가능 위치 찾기 
			if(nRet_1 == RET_GOOD)
			{ 
				m_nFirstTray_Y_Num =  m_npFindWorkPosYXCPB[0];// m_npFindWorkPosYXCPB[4]와 같음 
				m_nFirstTray_X_Num =  m_npFindWorkPosYXCPB[1];// m_npFindWorkPosYXCPB[4]와 같음 
				m_nBuffer_Work_Cnt =  m_npFindWorkPosYXCPB[2];//james 2016.0929 
				m_nFirstPicker_Num = m_npFindWorkPosYXCPB[3]; //nFirstPicker_Num
				m_nFirst_Buff_Num  = m_npFindWorkPosYXCPB[4]; //nSortBuffer_First

				m_nPartial_FirstRetestBuffPos_Select_No = m_nFirst_Buff_Num;
				m_nRunStep = 6400;//현재 위치의 자재를 집어 작업할 테스트 사이트로 이동한다
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

				m_nRunStep = 7000;//그냥 다 집어 넣는다. 얼쩔 수 없다
			}
			break;

		case 6400:
			nRet_1 = Process_DVC_Place(BIN_RETESTBIN, st_sync_info.nRetestBuff_Traget_THD_Work_Site, st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][2], st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3], 1);

			if(nRet_1 == RET_GOOD)
			{					
				m_nRunStep = 7000;
			}
			else if(nRet_1 == RET_PICKER_NOT_FIND) //피커는 더이상 작업할 내용이 없다.  
			{
				m_nRunStep = 7000;			
			}
			else if(nRet_1 == RET_TRAY_NOT_FIND) //테스터에는 더 이상 작업할 수 있는 내용이 없다
			{

				m_nRunStep = 6300;  
			}
			break; 


		//////////////////////////////////////////////////
		//Test site에 자재를 인서트한다 
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
			else if(nRet_1 == RET_PICKER_NOT_FIND) //피커는 더이상 작업할 내용이 없다.  
			{
				m_nRunStep = 7000; //james 2016.0926 pick & place 반복 m_nRunStep = 1000; //james 2016.0924 7000;			
				
			}
			else if(nRet_1 == RET_TRAY_NOT_FIND) //테스터에는 더 이상 작업할 수 있는 내용이 없다
			{
				
				m_nRunStep = 7000;  
			}
			break;

		case 7100:
			nRetestExistCnt=0;
			for(i = 0; i < TEST_PICKER_PARA; i++) //test site에서 작업할 정보를 확보한다
			{					 
				if(st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == CTL_YES)// && st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable == CTL_YES)
				{
					st_picker[THD_TEST_RBT].st_pcb_info[i].nBin = BD_DATA_RETEST; // 2016.1005 
					nRetestExistCnt++;
				} 
			}
			if(nRetestExistCnt > 0) //자재가 남아있으면 
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
		//Test site에 자재를 넣어고, 한 사이트를 모두 처리했으면 테스트 START를 진행 한다  
		//////////////////////////////////////////////////////////////////////////////////////
		case 8000:
			nRet_1 = clsFunc.Find_TrayWork_Pos(0, st_sync_info.nTestSite_Target_THD_Work_Site[2], CTL_NO, m_npFindWorkPosYXCPB,THD_TEST_RBT, st_worker_info.strLotNo_Empty);
			
			//2016.1229
			//빈 소켓이 있지만 UNLOAD할 디바이스가 있는지 다시한번 체크하자
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
			if(nRet_1 == RET_SKIP || st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][3] == 4)//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nStart_FixPos == 4)//물리적 위치가 4이면 마지막 Tester의 Right 사이트를 작업한것으로, 더이상 작업할 내용이없다 
			{			
				for(i = 0; i < 8; i++)  
				{ 
					if(st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].st_pcb_info[i].nYesNo == CTL_YES && st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].st_pcb_info[i].nTestBdStart == BD_END)
					{
						nFlag++;
					}
				}
				if(nFlag > 0) //언로딩할 자재가 있으면 언로딩 부터 해야 한다 
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
			{//더 작업할 공간이 있다 그러면 자재를 더 넣어야 한다 
				if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_TEST_RBT] >=  LOT_END_START) //   				 
				{//st_sync_info.nRetestBuff_Traget_THD_Work_Site

					nCount = 0;
					for(nTHD_i = THD_TESTSITE_1; nTHD_i <= THD_TESTSITE_8; nTHD_i++)
					{
						if(st_find_testsite_info[nTHD_i][WORK_PICK].nTotal_Cnt > 0 && st_test_site_info[nTHD_ID].strLotNo == m_strFindLotNo) //WORK_PICK 할 수 있는 자재가 있으면 이 자재를 있으면 
						{
							nCount++;
						}
					}

					nRet_1 = clsFunc.Find_TrayWork_Pos(0, st_sync_info.nRetestBuff_Traget_THD_Work_Site, CTL_YES, m_npFindWorkPosYXCPB, THD_TEST_RBT, m_strFindLotNo);
					if(nRet_1 == RET_SKIP) //리테스트 버퍼에 작업 가능한 자재가 있다
					{// if(m_npFindWorkPosYXCPB[2] >= 0) //retest buffer에 자재가 있다 
						m_nRunStep = 8100; //현재 lot은 더이상 공급할 자재가 없으니 현재 넣으것을 test start하여 테스트 하게 한다 
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
		//테스트 보드 스타트 
		////////////////////////////////////////////////////
		case 8100:
			//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nTester_Req[0] = CTL_REQ; //테스터 작업을 알린다 
			//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nTester_Req[1] = WORK_PLACE; //test scoket에 자재를 인서트 했다. 테스트 시작해야 한다, chamber을 닫고 BD_START를 진행한다 
			//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site[2]].nTester_Req[2] = BD_START_CHK; //st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].nSite_Pos;

			// jtkim 20160831
			st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][0] = CTL_REQ; //test start //CTL_REQ;
			st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][1] = WORK_PLACE; //자재를 집을 수 있게 요청 
			st_sync_info.nTestRbt_Dvc_Ld_Req[st_sync_info.nTestSite_Target_THD_Work_Site[2]][5] = BD_START; // test site start요청

			/*
			st_sync_info.dfnTestRbt_Dvc_Req[st_sync_info.nTestSite_Target_THD_Work_Site][0] = BD_START; //test start //CTL_REQ;
			st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nTestSite_Target_THD_Work_Site][1] = WORK_PLACE; //자재를 집을 수 있게 요청 
			st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nTestSite_Target_THD_Work_Site][2] = st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].nSite_Pos;
			*/

			// 테스트 스타트는 챔버등을 장착 후 테스트 각 가이트에서 start 하게 하자  
			//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].nTestBdStart = BD_START; //test start
			// jtkim 2010831
			//st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nTestSite_Target_THD_Work_Site][0] = CTL_REQ; //test start //CTL_REQ;
			//st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nTestSite_Target_THD_Work_Site][1] = WORK_PLACE; //자재를 집을 수 있게 요청 
			//st_sync_info.nTestRbt_Dvc_Req[st_sync_info.nTestSite_Target_THD_Work_Site][2] = st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].nSite_Pos;

			//st_test_site_info[st_sync_info.nTestSite_Target_THD_Work_Site].nTestBdStart = BD_START; //test start
			st_sync_info.nTestSite_Target_THD_Work_Site[2] = 0; //클리어 하여, 작업한 테스터 위치 찾는 것을 초기화 한다.
			                                                 // 뒤로 이어서 테스트 사이트 작업을 하고 싶으면 st_sync_info.nTestSite_Target_THD_Work_Site++; 하면 된다 
			//2016.1012
			st_sync_info.nTestSite_Target_THD_Work_Site[1] = 0; //클리어 하여, 작업한 테스터 위치 찾는 것을 초기화 한다.

			m_nRunStep = 1000; //test site 모든 작업이 완료되었다. 1000 처음 스텝으로 이동하여 다음 위치의 작업을 찾아 준비한다 
			break;
			

		case 9000: 

			m_nRunStep = 1000;  
			break;


		default :
			break;
		}
}


//소케오프 유무를 확인후 로딩 버퍼에 해당 정보와 같이 로딩 자재 공급을 요청 한다 
//테스트 소켓솨 로봇등은 위치 정보등이 틀리니, 순서를 combine하는 기능이 있어야 한다 
int CRunTestSiteRobot::TestSite_PickPlace_SocketData_LoadBuff_Map_Combine(int nMode, int nTestSite_THD_Num, int nTestSocket_0_4_Start_Pos) 
{//nTestSocket_0_4_Start_Pos => 0:정면에서 좌측 첫번째 소켓(0,1,2,3 socket 작업), 4:정면 조측에서 4번째 소켓(4,5,6,7 소켓 작업) 의 무릴적 위치를 기준으로 정한다 
	int i, nPos=0;
	int nFuncRet = RET_ERROR;

	for(i=0; i< 4; i++) //loading 버퍼 수량 4개 
	{
		if(nTestSite_THD_Num >= THD_TESTSITE_1 && nTestSite_THD_Num <= THD_TESTSITE_4) //죄측 테스터 ,소켓 기준은 좌측위치 부터 (1회작업)7,6,5,4, (2회작업)3,2,1,0 의 순서로 기준 소켓 위치및 정보 정의됨
		{
			nPos = (7 - nTestSocket_0_4_Start_Pos) - i;
			if(st_test_site_info[nTestSite_THD_Num].st_pcb_info[nPos].nEnable == CTL_YES) //socket ON
			{
				st_buffer_info[THD_LD_BUFF].st_pcb_info[3 - i].nEnable = CTL_YES;
				nFuncRet = RET_GOOD;
				//james 2016.0926 조건 추가 로딩 버퍼의 자재만을 집으러 갈때만 사용 st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable = CTL_YES; 
			}
			else //if(st_test_site_info[nTestSite_THD_Num].st_pcb_info[nPos].nEnable  == CTL_NO)//Socket OFF
			{
				st_buffer_info[THD_LD_BUFF].st_pcb_info[3 - i].nEnable = CTL_NO;
				//james 2016.0926 조건 추가 로딩 버퍼의 자재만을 집으러 갈때만 사용 st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable = CTL_NO;
			}
		}
		else //if(nTestSite_THD_Num >= THD_TESTSITE_5 && nTestSite_THD_Num <= THD_TESTSITE_8) //죄측 테스터 , 소켓 기준은 좌측위치 부터  (1회작업)0,1,2,3, (2회작업)4,5,6,7 의 순서로 기준 소켓 위치및 정보 정의됨
		{
			nPos = nTestSocket_0_4_Start_Pos + i;
			if(st_test_site_info[nTestSite_THD_Num].st_pcb_info[nPos].nEnable  == CTL_YES) //socket ON
			{
				st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nEnable = CTL_YES;
				nFuncRet = RET_GOOD;
				//james 2016.0926 조건 추가 로딩 버퍼의 자재만을 집으러 갈때만 사용 st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable = CTL_YES;
			}
			else //if(st_test_site_info[nTestSite_THD_Num].st_pcb_info[nPos].nEnable == CTL_NO)//Socket OFF
			{
				st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nEnable = CTL_NO;
				//james 2016.0926 조건 추가 로딩 버퍼의 자재만을 집으러 갈때만 사용 st_picker[THD_TEST_RBT].st_pcb_info[i].nEnable = CTL_NO;
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
int CRunTestSiteRobot::Process_DVC_Pick(int nMode, int nWork_Site, int nTest_Site, int nStart_FixPos) //int nStart_FixPos = Left(0), Right(4) 두가지 위치중 하나이다, 무조건 동작기준 물리적인 죄측부터 작업 //2016.0814
{
	int nFuncRet = RET_PROCEED;
	int nRet_1, nRet_2, nRet_3 = 0, i, nFlag;	
	int nFind_JobPos =0; //작업할 위치를 찾는다
	//int nStart_FixPos =0; //test site에서 물리적인 시작 위치  
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
		{//집을 일이 없음 
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
		m_dCurrentPos[m_nRobot_Z] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[m_nRobot_Z] > st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벗어나 있으면 저위치 시킨다
		{
			m_nPick_Step = 0;
			break;
		}
		m_dCurrentPos[m_nRobot_Ejector] = COMI.Get_MotCurrentPos(m_nRobot_Ejector);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[m_nRobot_Ejector] > st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벗어나 있으면 저위치 시킨다
		{
			m_nPick_Step = 0;
			break;
		}
		m_dpTargetPosList[0] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_SAFETY];
		m_dpTargetPosList[1] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_SAFETY];
		nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate); 
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_nPick_Step = 400;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 300;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 300;
		}
*/
		 

	case 900:
		m_dCurrentPos[m_nRobot_Z] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[m_nRobot_Z] > st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벗어나 있으면 저위치 시킨다
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
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_nPick_Step = 920;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 900;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(2122, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 910;
		}
		break;

	case 920:
		m_dCurrentPos[m_nRobot_Ejector] = COMI.Get_MotCurrentPos(m_nRobot_Ejector);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[m_nRobot_Ejector] > st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벗어나 있으면 저위치 시킨다
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
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_nPick_Step = 940;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 930;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(2123, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 930;
		}
		break;

	case 940:
		m_bDvcWaitChk_Falg = false;
		m_nPick_Step = 1000;
		break; 

	///////////////////////////////////////////////////////////////////
	// 작업 조건을 확인한다 
	///////////////////////////////////////////////////////////////////
	case 1000:
		m_dCurrentPos[m_nRobot_Z] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[m_nRobot_Z] > st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벗어나 있으면 정위치 시킨다
		{
			m_nPick_Step = 900;
			break;
		}
		m_dCurrentPos[m_nRobot_Ejector] = COMI.Get_MotCurrentPos(m_nRobot_Ejector);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[m_nRobot_Ejector] > st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[m_nRobot_Ejector].d_allow) //안전 위치를 벗어나 있으면 정위치 시킨다
		{
			m_nPick_Step = 900;
			break;
		}

		nRet_1 = clsFunc.Check_PickerStatus(0, THD_TEST_RBT, CTL_YES, m_npPicker_YesNo, m_npPicker_Vacuum_Status, m_npPicker_OutputStatus); //nMode == 0 이면 전역변수, nMode == 1이면 멤버변수의 정보에 따라 피커 상태를 확인한다,
	
		if(nRet_1 == RET_GOOD) //전역 변수에 따른 피커 상태를 체크 
		{//피커 정보와 현재 상태가 정상이다
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
// Retest Bin 처리 옵션으로 리테스트 빈은 위치정보 및 피커 정보등을  
// 함수 들어오기전에 셋팅해 오므로 step을 jump 해야 함
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

		//작업 가능 위치를 찾는다
		nFlag = 0;
		m_nFirstTray_Y_Num = -1; m_nFirstTray_Y_Num = -1; m_nFirstPicker_Num = -1; //flag clear
		for(i = 0; i < m_nPickerPara; i++)
		{
			if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_NO && st_picker[m_nRobotSite].st_pcb_info[i].nEnable == CTL_YES)//피커에 자재가 들고 있다는 정보가 없다
			{
				if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF)
				{
					m_nFirstPicker_Num = i; //정보에 따라 피커 위치는 가변 된다
				}
				else //if(nWork_Site == THD_LD_BUFF || nWork_Site == THD_LD_BUFF || (nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8) )
				{
					m_nFirstPicker_Num = 0; //피커의 시작 위치는 고정으로 작업한다 
				}
				
				i = m_nPickerPara;
			}
		}

		if(m_nFirstPicker_Num == -1)
		{
			//피커에 더이상 작업할 것이 없다
			m_nPick_Step = 0;
			nFuncRet = RET_PICKER_NOT_FIND;
			break;
		}

		nRet_1 = clsFunc.Find_TrayWork_Pos(0, nWork_Site, CTL_YES, m_npFindWorkPosYXCPB, THD_TEST_RBT, st_worker_info.strLotNo_Empty);
		if(nRet_1 == RET_GOOD) //작업 가능한 자재가 있다
		{
			if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF)
			{
				m_nFirstTray_Y_Num = m_npFindWorkPosYXCPB[0]; //사용하지 않음  m_npFindWorkPosYXCPB[0];
				m_nFirstTray_X_Num = m_npFindWorkPosYXCPB[1];
			}
			else if(nWork_Site == THD_LD_BUFF)// || nWork_Site == THD_ULD_BUFF) 
			{
				m_nFirstTray_Y_Num = 0;
				m_nFirstTray_X_Num = 0;//사용
			}
			else if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8)
			{
				m_nFirstTray_Y_Num = nStart_FixPos;//사용하지 않음 m_nFirstTray_Y_Num = st_test_socket_info[st_sync_info.nTestSite_Target_THD_Work_Site].nWorkSocket_StartPos; 
				m_nFirstTray_X_Num = nStart_FixPos; //물리적 으로 정한 위치로, 0 또는 4 임  st_test_socket_info[st_sync_info.nTestSite_Target_THD_Work_Site].nWorkSocket_StartPos;
			}
		}
		else if(nRet_1 == RET_SKIP) //더이상 자재가 없다
		{	//계속 진행할 수 없다
			m_nPick_Step = 0;
			nFuncRet = RET_TRAY_NOT_FIND;
			break;
		}

		nFlag = 0;
		if(m_nFirstPicker_Num != -1 && m_nFirstTray_Y_Num != -1 && m_nFirstTray_X_Num != -1) //작업할 공간이 다 있을때 다음 작업을 진행한다
		{
			for(i = 0; i < m_nPickerPara; i++)  //4, m_n_FirstPicker_Num; y < m_mPickerPara; y++)
			{
				m_npPicker_YesNo[i] = CTL_NO; //작업할 피커정보 clear
				if(i < m_nFirstPicker_Num) continue;
				if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_NO && st_picker[m_nRobotSite].st_pcb_info[i].nEnable == CTL_YES) //피커정보가 맞을떄 
				{
					if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF)
					{//james 2016.0922 retest buffer 는 이곳을 ㅗ들어오지 않는다. 위에서 step jump 함  
						if(i >= 0 && m_nFirstTray_X_Num + (i - m_nFirstPicker_Num) < st_recipe_info.nRetestBuffer_Num)//15
						{
							//리테슽 조건은 나중에 생각하자 
							if(nTest_Site == TESTSITE_LEFT) //left site룰 작업하기 위한 작업 중일때
							{
								nFind_JobPos = (st_recipe_info.nLdUldBuffer_Num - 1) - i ; //LEFT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는 3,2,1,0 로 반대이다 
								if(st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_YES)//&& st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //트레이에 자재가  있으면 집기 가능하게 셋
								{
									m_npPicker_YesNo[i] = CTL_YES;
									nFlag++;
								}
							}
							else if(nTest_Site == TESTSITE_RIGHT) //right sitet를 작업하기 위한 작업 중일때
							{
								nFind_JobPos = i ; //RIGHT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는0,1,2,3 로 위치 순서가 동일하다  
								if(st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_YES && st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //트레이에 자재가  있으면 집기 가능하게 셋
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
							if(nTest_Site == TESTSITE_LEFT) //left site룰 작업하기 위한 작업 중일때
							{
								nFind_JobPos = (st_recipe_info.nLdUldBuffer_Num - 1) - i ; //LEFT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는 3,2,1,0 로 반대이다 
								if(st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_YES)//james 2016.1005 자재 있으면 무조건 집자  && st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //트레이에 자재가  있으면 집기 가능하게 셋
								{
									m_npPicker_YesNo[i] = CTL_YES;
									nFlag++;
								}
							}
							else if(nTest_Site == TESTSITE_RIGHT) //right sitet를 작업하기 위한 작업 중일때
							{
								nFind_JobPos = i ; //RIGHT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는0,1,2,3 로 위치 순서가 동일하다  
								if(st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_YES)//james 2016.1005 자재 있으면 무조건 집자  && st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //트레이에 자재가  있으면 집기 가능하게 셋
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
							nFind_JobPos = (st_recipe_info.nTestSocket_Num - 1) - i ; //LEFT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는 3,2,1,0 로 반대이다 
							nFind_JobPos = nFind_JobPos - nStart_FixPos; //st_test_site_info[nWork_Site].nStart_FixPos;

							if(st_test_site_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_YES )//&& st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //트레이에 자재가  있으면 집기 가능하게 셋
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
							nFind_JobPos = i ; //RIGHT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는0,1,2,3 로 위치 순서가 동일하다  
							nFind_JobPos = nFind_JobPos + nStart_FixPos; //st_test_site_info[nWork_Site].nStart_FixPos;

							if(st_test_site_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_YES )//&& st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //트레이에 자재가  있으면 집기 가능하게 셋
							{
								m_npPicker_YesNo[i] = CTL_YES;
								nFlag++;
							}
						}
					}
				}
			}

			if(nFlag == 0) //작업할 피커가 하나도 준비되 않았다.
			{ //이부분이 아오면 안됨 
				m_nPick_Step = 0;
				nFuncRet = RET_PICKER_NOT_FIND;//피커에 더이상 작업할 것이 없다
				break;
			}

			//실제 작업 위치를 셋팅
			m_npFindWorkPosYXCPB[0] = m_nFirstTray_Y_Num; m_npFindWorkPosYXCPB[1] = m_nFirstTray_X_Num; m_npFindWorkPosYXCPB[3] = m_nFirstPicker_Num;

			m_nMove_Flag[0] = CTL_NO; m_nMove_Flag[1] = CTL_NO;
			m_nPick_Step = 2000;
		}		
		break;

	case 2000:
		m_dCurrentPos[2] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[2] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벅어나 있으면 저위치 시킨다
		{
			m_nPick_Step = 900;
			break;
		}
		m_dCurrentPos[m_nRobot_Ejector] = COMI.Get_MotCurrentPos(m_nRobot_Ejector);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[m_nRobot_Ejector] > st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벗어나 있으면 저위치 시킨다
		{
			m_nPick_Step = 900;
			break;
		}

		//james 2016.0826 피커와 버퍼/테스트 소켓의 자재 충돌방지을 위해 추가 
		if(nWork_Site == THD_LD_BUFF)
		{//Check_Robot_Buffer_Clash(int nTestDirection, int nRbtTHDSite, int TestBuffTHDSite, int nStartPos, int *nErrorInfoStatus) 
			if(nTest_Site == TESTSITE_LEFT) //LEFT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는 3,2,1,0 로 반대이다 
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

				nRet_1 = clsFunc.Check_Robot_Buffer_Clash(TESTSITE_LEFT, THD_TEST_RBT, THD_LD_BUFF, m_nFirstTray_X_Num, m_nFirstPicker_Num, m_nClash_ErrorInfoStatus); //피커와 버퍼등의 자재 충돌 확인 
				if(nRet_1 == RET_ERROR)
				{
					CTL_Lib.Alarm_Error_Occurrence(2126, dWARNING, clsFunc.m_strAlarmCode);		
					break;
				}
			}
			else if(nTest_Site == TESTSITE_RIGHT) //right sitet를 작업하기 위한 작업 중일때//RIGHT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는0,1,2,3 로 위치 순서가 동일하다  
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

				nRet_1 = clsFunc.Check_Robot_Buffer_Clash(TESTSITE_RIGHT, THD_TEST_RBT, THD_LD_BUFF, m_nFirstTray_X_Num, m_nFirstPicker_Num, m_nClash_ErrorInfoStatus); //피커와 버퍼등의 자재 충돌 확인 
				if(nRet_1 == RET_ERROR)
				{
					CTL_Lib.Alarm_Error_Occurrence(2131, dWARNING, clsFunc.m_strAlarmCode);		
					break;
				}
			}
		}
		else  if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_4 ) //left site
		{
			nRet_1 = clsFunc.Check_Robot_Buffer_Clash(TESTSITE_LEFT, THD_TEST_RBT, nWork_Site, m_nFirstTray_X_Num, m_nFirstPicker_Num, m_nClash_ErrorInfoStatus); //피커와 버퍼등의 자재 충돌 확인 
			if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(2132, dWARNING, clsFunc.m_strAlarmCode);		
				break;
			}
		}
		else  if(nWork_Site >= THD_TESTSITE_5 && nWork_Site <= THD_TESTSITE_8 ) //right site
		{
			nRet_1 = clsFunc.Check_Robot_Buffer_Clash(TESTSITE_RIGHT, THD_TEST_RBT, nWork_Site, m_nFirstTray_X_Num, m_nFirstPicker_Num, m_nClash_ErrorInfoStatus); //피커와 버퍼등의 자재 충돌 확인 
			if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(2133, dWARNING, clsFunc.m_strAlarmCode);		
				break;
			}
		}//
		else  if(nWork_Site >= THD_RETEST_1_BUFF && nWork_Site <= THD_RETEST_2_BUFF ) //james 2016.0929 추가 
		{
			//james 2016.0929 추가 
			nRet_1 = clsFunc.Check_Robot_Buffer_Clash(nTest_Site, THD_TEST_RBT, nWork_Site, m_nFirstTray_X_Num, m_nFirstPicker_Num, m_nClash_ErrorInfoStatus); //피커와 버퍼등의 자재 충돌 확인 
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

			Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON); //미리 벌린다  
		}
		else
		{
			CTL_Lib.Alarm_Error_Occurrence(2135, dWARNING, clsFunc.m_strAlarmCode);
			break;
		}
		
		nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate); 
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_bDvcWaitChk_Falg =  false;
			m_nPick_Step = 2100;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 2000;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(2136, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 2000;
		}
		break; 	

	case 2100:
		if(nWork_Site == THD_LD_BUFF)
		{
			

			//로딩 버퍼에 요청하여 자재가 준비되어 CTL_READY 된 상태, 자재를 집을 수 있다 
			 if(st_sync_info.nTestRbt_Dvc_Req[nWork_Site][0] == CTL_READY && st_sync_info.nTestRbt_Dvc_Req[nWork_Site][1] == WORK_PICK) //로딩 버퍼가 준비되기를 기다린다 
			 {				 
				nRet_1 = clsFunc.Check_Robot_Buffer_Clash(st_sync_info.nTestRbt_Dvc_Req[nWork_Site][2], THD_TEST_RBT, nWork_Site, m_nFirstTray_X_Num, m_nFirstPicker_Num, m_nClash_ErrorInfoStatus); //피커와 버퍼등의 자재 충돌 확인 
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
			if(st_sync_info.nTestRbt_Dvc_Req[nWork_Site][0] == CTL_READY && st_sync_info.nTestRbt_Dvc_Req[nWork_Site][1] == WORK_PICK) //로딩 버퍼가 준비되기를 기다린다 
			{				 
				//james 2016.0911 바꿔야 됨 
				nRet_1 = clsFunc.Check_Robot_Buffer_Clash(st_sync_info.nTestRbt_Dvc_Req[nWork_Site][2], THD_TEST_RBT, nWork_Site, m_nFirstTray_X_Num, m_nFirstPicker_Num, m_nClash_ErrorInfoStatus); //피커와 버퍼등의 자재 충돌 확인 
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
			if(st_sync_info.nTestRbt_Dvc_UnLd_Req[nWork_Site][0] == CTL_READY && st_sync_info.nTestRbt_Dvc_UnLd_Req[nWork_Site][1] == WORK_PICK) //로딩 버퍼가 준비되기를 기다린다 
			{				 
				nRet_1 = clsFunc.Check_Robot_Buffer_Clash(st_sync_info.nTestRbt_Dvc_Req[nWork_Site][2], THD_TEST_RBT, nWork_Site, m_nFirstTray_X_Num, m_nFirstPicker_Num, m_nClash_ErrorInfoStatus); //피커와 버퍼등의 자재 충돌 확인 
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
		m_dCurrentPos[m_nRobot_Ejector] = COMI.Get_MotCurrentPos(m_nRobot_Ejector);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[m_nRobot_Ejector] > st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벗어나 있으면 저위치 시킨다
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

		Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON); //미리 벌린다  

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
			Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON); //미리 벌린다  
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
			Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON); //미리 벌린다  
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

		if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8) //james 2016.0922 test site 작업이면 ejector를 한후 pick 작업이 가능하다 
		{
			m_nPick_Step = 3110;
			break;
		}


		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, m_dpTargetPosList[2], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{ 			
			m_nPick_Step = 3100;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 3000;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(2240, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 3000;
		}		
		break;	

	case 3100:
		if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8) //test site 작업이면 
		{
			m_nPick_Step = 3110;
		}
		else 
		{
			m_nPick_Step = 3200;
		}
		break;

	///////////////////////////////////////////////////////////////////
	// Test site에서 피킹할려면, 자재를 빼는 소켓 ejector 진행 하여야 한다
	// socket ejector 진행 
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
			Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON); //미리 벌린다  
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
			Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON); //미리 벌린다  
		}
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, m_dpTargetPosList[2], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_nPickRetry_Cnt = 0;
			m_nPick_Step = 3120;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 3110;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
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
			Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON); //미리 벌린다  
		}
		else if(nWork_Site >= THD_TESTSITE_5 && nWork_Site <= THD_TESTSITE_8)
		{
			m_dpTargetPosList[3] = st_motor_info[m_nRobot_Ejector].d_pos[P_TESTSITE_2_RIGHT];
			Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON); //미리 벌린다  
		}
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Ejector, m_dpTargetPosList[3],  (int) (COMI.mn_runspeed_rate / 2));//2017.0325 COMI.mn_runspeed_rate
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{ 			
			m_nPick_Step = 3130;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 3120;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(2260, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 3120;
		}		
		break;	

	case 3130: //ejector 안전 위치로 이동 
		m_dpTargetPosList[3] = st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY];
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Ejector, m_dpTargetPosList[3],  (int) (COMI.mn_runspeed_rate / 2));//2017.0325 COMI.mn_runspeed_rate
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{ 			
			m_nPick_Step = 3140;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 3120;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(2270, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 3120;
		}		
		break;	

	//Z축 디바이스 test socket dvc pick 
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
			//Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON); //미리 벌린다  
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
			//Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON); //미리 벌린다  
		}
		else
		{//
			break;
		} 

		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, m_dpTargetPosList[2], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{ 			
			m_nPick_Step = 3200;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 3130;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(2280, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 3130;
		}		
		break;	

	///////////////////////////////////
	//자재를 집는다 
	///////////////////////////////////
	case 3200: //자재를 집는다 
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
			Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON);  //에러니 벌린다 
			CTL_Lib.Alarm_Error_Occurrence(2290, dWARNING, m_strAlarmCode);
			m_nPick_Step = 3200;
		}
		break;

	case 3300:
		nRet_1 = clsFunc.Check_PickerStatus(1, THD_TEST_RBT, CTL_YES, m_npPicker_YesNo, m_npPicker_Vacuum_Status, m_npPicker_OutputStatus);
		if(nRet_1 == RET_GOOD) //전역 변수에 따른 피커 상태를 체크 
		{//피커 정보와 현재 상태가 정상이다
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
		if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8) //test site 작업이면 
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
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{ 			
			m_nPick_Step = 3420;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 3410;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(2300, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 3410;
		}		
		break;	

	case 3420:
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{ 			
			m_nPick_Step = 3500;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 3420;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
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
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{ 			
			m_nPick_Step = 3500;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 3440;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(2312, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 3440;
		}		
		break;
	/////////////////////////////////////////////////////////////////////////
	// load/unload buffer에서 로딩 자재를 집을떄는 버퍼의 그리퍼 제어는 로봇에서 한다 
	// 그외 사이트는 그리퍼가 없어서 그럴 필요 없음 
	/////////////////////////////////////////////////////////////////////////
	case 3500:
		if(nWork_Site == THD_LD_BUFF)
		{
			for(i = 0; i < m_nPickerPara; i++)  //4
			{
				if(nTest_Site == TESTSITE_LEFT) //left site룰 작업하기 위한 작업 중일때
				{
					m_npBuffer_YesNo[i] =  m_npPicker_YesNo[(m_nPickerPara - 1) - i]; //3,2,1,0 으로 반대로 작업위치 지정 
				}
				else if(nTest_Site == TESTSITE_RIGHT) //right sitet를 작업하기 위한 작업 중일때
				{
					m_npBuffer_YesNo[i] = m_npPicker_YesNo[i];
				}
			}

			 if(st_sync_info.nTestRbt_Dvc_Req[nWork_Site][0] == CTL_READY && st_sync_info.nTestRbt_Dvc_Req[nWork_Site][1] == WORK_PICK) //로딩 버퍼가 준비되어 있다 
			 {				 //james 2016.1005 = CTL_READY
				 m_nPick_Step = 3510;
			 }		
		}
		else  //if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF || nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8)
		{//집을 일이 없음 
			 m_nPick_Step = 4000;
		}		
		break;

	/////////////////////////////////////////////////////////////////////////
	// load/unload buffer에서 로딩 자재를 집을떄는 버퍼의 그리퍼 제어는 로봇에서 한다 
	// load buffer 의 그리퍼 중 로보트가 집은 자재 위치의 그리퍼만 open하여 로봇이 집어 갈 수 있게 한다
	/////////////////////////////////////////////////////////////////////////
	case 3510:
		clsRunDvcLdBuffer.Set_Gripper_OnOff(0, IO_ON, m_npBuffer_YesNo); //로딩 버퍼의 그리퍼를 open하여 로봇이 자재를 집어 갈 수 있게 한다 
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

	case 3530://james 2016.0922 추가 
		nRet_1 = clsFunc.Check_BufferStatus(1, THD_LD_BUFF, CTL_NO, m_npBuffer_YesNo, m_npBuff_Status, m_npBuff_OutputStatus); //전역변수와 센서 상태가 일치해야 한다 
		if(nRet_1 == RET_GOOD)
		{//정상 상태, //Enable 되어 있는 버퍼 위치는 모두 채워진 상태, 테스트 로봇 사이트로 이송가능하다 
			m_nPick_Step = 4000;
		}
		else if(nRet_1== RET_ERROR)
		{//아직 버버 채우는 작업이 끝나지 않은 상태 
			CTL_Lib.Alarm_Error_Occurrence(2330, dWARNING, clsFunc.m_strAlarmCode);
			m_nPick_Step = 3510;
		} 
		break;
		
		//james 2016.0922 m_nPick_Step = 4000;
		//break; 
	/////////////////////////////////////////////////////////////////////////		

	case 4000:
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_bDvcWaitChk_Falg = false;
			m_nPick_Step = 4100;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPick_Step = 4000;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(2340, dWARNING, st_alarm_info.strCode);
			m_nPick_Step = 4000;
		}
		break; 

	case 4100:
		// jtkim 20160831
		//if(nWork_Site == THD_LD_BUFF)
		if(nWork_Site == THD_LD_BUFF)
		{
			
			st_sync_info.nTestRbt_Dvc_Req[nWork_Site][0] = CTL_FREE; //test robot가 load buffer에서 집기완료 
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
			//20161026 자재 damage를 적게 할려고 작업자가 자재를 붙여줘야 한다
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
						m_npPicker_YesNo[i] = CTL_NO; //작업한 피커 정보 클리어

						if(nTest_Site == TESTSITE_LEFT) //left site룰 작업하기 위한 작업 중일때
						{
							nFind_JobPos = (st_recipe_info.nLdUldBuffer_Num - 1) - i ; //LEFT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는 3,2,1,0 로 반대이다 
								

							//아래 함수에서 사용위해 위치 정보를 셋팅한다
							m_npSet_WorkPosYXCPB[0]  = nFind_JobPos; //버퍼sms Y만 사용 //버퍼의 위치 지정
							m_npSet_WorkPosYXCPB[1]  = nFind_JobPos; //qjvjsms X는 사용하지 않음 //버퍼의 위치 지정
							m_npSet_WorkPosYXCPB[3]  = i; //picker 위치 정보

							clsFunc.Data_Exchange_PickPlace(PICKER_PICK_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //피커 및 트레이 정보 셋팅 함수
							//위 함수에서 모든 정보를 셋팅 및 클리어 한다
							//st_picker[m_nRobotSite].st_pcb_info[y].nYesNo = CTL_YES;					//피커 정보 셋
							//st_tray_info[nWork_Site].st_pcb_info[m_npFirstPosYXP[0] + (y - m_n_FirstPicker_Num)][m_npFirstPosYXP[1]].nYesNo = CTL_NO; //트레이 정보 클리어
							//st_count_info.nSSD_Input[0]++;
						}
						else if(nTest_Site == TESTSITE_RIGHT) //RIGHT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는0,1,2,3 로 위치 순서가 동일하다 
						{
							nFind_JobPos = i ; //RIGHT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는0,1,2,3 로 위치 순서가 동일하다  

							//아래 함수에서 사용위해 위치 정보를 셋팅한다
							m_npSet_WorkPosYXCPB[0]  = nFind_JobPos; //버퍼sms Y만 사용 //버퍼의 위치 지정
							m_npSet_WorkPosYXCPB[1]  = nFind_JobPos; //qjvjsms X는 사용하지 않음 //버퍼의 위치 지정
							m_npSet_WorkPosYXCPB[3]  = i; //picker 위치 정보

							clsFunc.Data_Exchange_PickPlace(PICKER_PICK_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //피커 및 트레이 정보 셋팅 함수 
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
						m_npPicker_YesNo[i] = CTL_NO; //작업한 피커 정보 클리어

						if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_4)
						//{//if(st_test_site_info[nWork_Site].nSite_Pos == TESTSITE_LEFT) //left site룰 작업하기 위한 작업 중일때
						{
							nFind_JobPos = (st_recipe_info.nTestSocket_Num - 1) - i ; //LEFT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는 3,2,1,0 로 반대이다 
							nFind_JobPos = nFind_JobPos - nStart_FixPos; //st_test_site_info[nWork_Site].nStart_FixPos;								

							//아래 함수에서 사용위해 위치 정보를 셋팅한다
							m_npSet_WorkPosYXCPB[0]  = nFind_JobPos; //Test Site는 Y는 용하지 않음 //버퍼의 위치 지정
							m_npSet_WorkPosYXCPB[1]  = nFind_JobPos; //Test Site는 X만 사용 //버퍼의 위치 지정
							m_npSet_WorkPosYXCPB[3]  = i; //picker 위치 정보

							clsFunc.Data_Exchange_PickPlace(PICKER_PICK_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //피커 및 트레이 정보 셋팅 함수
							//위 함수에서 모든 정보를 셋팅 및 클리어 한다
							//st_picker[m_nRobotSite].st_pcb_info[y].nYesNo = CTL_YES;					//피커 정보 셋
							//st_tray_info[nWork_Site].st_pcb_info[m_npFirstPosYXP[0] + (y - m_n_FirstPicker_Num)][m_npFirstPosYXP[1]].nYesNo = CTL_NO; //트레이 정보 클리어
							//st_count_info.nSSD_Input[0]++;
						}
						else if(nWork_Site >= THD_TESTSITE_5 && nWork_Site <= THD_TESTSITE_8)
						//{//if(st_test_site_info[nWork_Site].nSite_Pos == TESTSITE_RIGHT) //RIGHT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는0,1,2,3 로 위치 순서가 동일하다 
						{
							nFind_JobPos = i ; //LEFT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는 3,2,1,0 로 반대이다 
							nFind_JobPos = nFind_JobPos + nStart_FixPos; //st_test_site_info[nWork_Site].nStart_FixPos;								

							//아래 함수에서 사용위해 위치 정보를 셋팅한다
							m_npSet_WorkPosYXCPB[0]  = nFind_JobPos; //Test Site는 Y는 용하지 않음 //버퍼의 위치 지정
							m_npSet_WorkPosYXCPB[1]  = nFind_JobPos; //Test Site는 X만 사용 //버퍼의 위치 지정
							m_npSet_WorkPosYXCPB[3]  = i; //picker 위치 정보

							clsFunc.Data_Exchange_PickPlace(PICKER_PICK_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //피커 및 트레이 정보 셋팅 함수 
						}

						//2017.0109
						st_picker[m_nRobotSite].st_pcb_info[i].strLogKey[0]	= _T("TEST_SITE");
						st_picker[m_nRobotSite].st_pcb_info[i].strLogData[0]	= st_picker[m_nRobotSite].st_pcb_info[i].strSerialNo;

						st_picker[m_nRobotSite].st_pcb_info[i].strMaterial.Format(_T("%s"), st_picker[m_nRobotSite].st_pcb_info[i].strSerialNo);
						st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[0] = _T("TEST_SITE");
						st_picker[m_nRobotSite].st_pcb_info[i].strMovePos[1] = _T("UNLOAD_BUFFER");//언로더로 갈지 리테스트로 갈지 모른다.

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
					if(nTest_Site == TESTSITE_LEFT) //left site룰 작업하기 위한 작업 중일때
					{
						nFind_JobPos = m_nFirst_Buff_Num - ((i - m_nFirstPicker_Num) * 2);  //13,11,9,7,5,3,1 위치 
						if(nFind_JobPos >= 0  && nFind_JobPos >= 0 && nFind_JobPos < st_recipe_info.nRetestBuffer_Num)
						{
							m_npPicker_YesNo[i] = CTL_NO; //작업한 피커 정보 클리어

							//아래 함수에서 사용위해 위치 정보를 셋팅한다
							m_npSet_WorkPosYXCPB[0]  = nFind_JobPos; //버퍼sms Y만 사용 //버퍼의 위치 지정
							m_npSet_WorkPosYXCPB[1]  = nFind_JobPos; //qjvjsms X는 사용하지 않음 //버퍼의 위치 지정
							m_npSet_WorkPosYXCPB[3]  = i; //picker 위치 정보

							clsFunc.Data_Exchange_PickPlace(PICKER_PICK_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //피커 및 트레이 정보 셋팅 함수
							//위 함수에서 모든 정보를 셋팅 및 클리어 한다
							//st_picker[m_nRobotSite].st_pcb_info[y].nYesNo = CTL_YES;					//피커 정보 셋
							//st_tray_info[nWork_Site].st_pcb_info[m_npFirstPosYXP[0] + (y - m_n_FirstPicker_Num)][m_npFirstPosYXP[1]].nYesNo = CTL_NO; //트레이 정보 클리어
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
					else if(nTest_Site == TESTSITE_RIGHT) //RIGHT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는0,1,2,3 로 위치 순서가 동일하다 
					{
						nFind_JobPos = m_nFirst_Buff_Num + ((i - m_nFirstPicker_Num) * 2);
						if(nFind_JobPos >= 0 && nFind_JobPos >= 0 && nFind_JobPos < st_recipe_info.nRetestBuffer_Num)
						{
							m_npPicker_YesNo[i] = CTL_NO; //작업한 피커 정보 클리어

							//아래 함수에서 사용위해 위치 정보를 셋팅한다
							m_npSet_WorkPosYXCPB[0]  = nFind_JobPos; //버퍼sms Y만 사용 //버퍼의 위치 지정
							m_npSet_WorkPosYXCPB[1]  = nFind_JobPos; //qjvjsms X는 사용하지 않음 //버퍼의 위치 지정
							m_npSet_WorkPosYXCPB[3]  = i; //picker 위치 정보

							clsFunc.Data_Exchange_PickPlace(PICKER_PICK_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //피커 및 트레이 정보 셋팅 함수 

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

		if (nRet_1 == RET_GOOD) // 1000 step에서 셋팅한 피커 정보에 맞게 모두 정상적 집었을 경우 이
		{
			//화면 정보 갱신

			m_nDvcRepickCount = 0;
			m_nPartialPickCount = 0;

			m_bDvcWaitChk_Falg = false;

			// jtkim 20160930 Log
			if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8)
			{
				// jtkim 20161008 최희택책임 요청
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

			// jtkim 20150216 conveyor/loader tray 화면 표시
			if (st_handler_info.cWndMain != NULL)
			{
				st_handler_info.cWndMain->PostMessage(WM_WORK_DISPLAY, MAIN_LD_PICKER_DISPLAY, 0);
				st_handler_info.cWndMain->PostMessage(WM_WORK_DISPLAY, MAIN_LD_TRAY_DISPLAY, 0);
				st_handler_info.cWndMain->PostMessage(WM_WORK_DISPLAY, MAIN_RETEST_BUF_DISPLAY, 0);
			}

			st_sync_info.nAutoBufferIn = CTL_YES;//2016.1117
			m_nPick_Step = 6000;
		}
		else if (nRet_1 == RET_ERROR) //셋팅한 피커중 한개라도 제대로 집지 못했으면 이곳으로 이동
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

int CRunTestSiteRobot::Process_DVC_Place(int nMode, int nWork_Site, int nTest_Site, int nStart_FixPos, int Opt) //int nStart_FixPos = Left(0), Right(4) 두가지 위치중 하나이다 , 무조건 동작기준 죄측부터 작업 //2016.0814 
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
		{//집을 일이 없음 
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

		m_dCurrentPos[m_nRobot_Z] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[m_nRobot_Z] > st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벗어나 있으면 저위치 시킨다
		{
			m_nPlace_Step = 0;
			break;
		}
		m_dCurrentPos[m_nRobot_Ejector] = COMI.Get_MotCurrentPos(m_nRobot_Ejector);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[m_nRobot_Ejector] > st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벗어나 있으면 저위치 시킨다
		{
			m_nPlace_Step = 0;
			break;
		}
		m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_SAFETY];
		m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_SAFETY];
		nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate); 
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_nPlace_Step = 400;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 300;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(2360, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 300;
		}
		break;

	case 900:
		m_dCurrentPos[m_nRobot_Z] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[m_nRobot_Z] > st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벗어나 있으면 저위치 시킨다
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
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_nPlace_Step = 920;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 900;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(2370, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 910;
		}
		break;

	case 920:
		m_dCurrentPos[m_nRobot_Ejector] = COMI.Get_MotCurrentPos(m_nRobot_Ejector);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[m_nRobot_Ejector] > st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[m_nRobot_Ejector].d_allow) //안전 위치를 벗어나 있으면 저위치 시킨다
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
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_nPlace_Step = 940;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 930;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(2380, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 930;
		}
		break;

	case 940:
		m_bDvcWaitChk_Falg = false;
		m_nPlace_Step = 1000;
		break; 

	///////////////////////////////////////////////////////////////////////
	// 작업할 조건을 찾고, 셋팅
	///////////////////////////////////////////////////////////////////////
	case 1000:
		m_dCurrentPos[m_nRobot_Z] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[m_nRobot_Z] > st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벗어나 있으면 저위치 시킨다
		{
			m_nPlace_Step = 900;
			break;
		}
		m_dCurrentPos[m_nRobot_Ejector] = COMI.Get_MotCurrentPos(m_nRobot_Ejector);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[m_nRobot_Ejector] > st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벗어나 있으면 저위치 시킨다
		{
			m_nPlace_Step = 900;
			break;
		}

		nRet_1 = clsFunc.Check_PickerStatus(0, THD_TEST_RBT, CTL_YES, m_npPicker_YesNo, m_npPicker_Vacuum_Status, m_npPicker_OutputStatus); //nMode == 0 이면 전역변수, nMode == 1이면 멤버변수의 정보에 따라 피커 상태를 확인한다,
		if(nRet_1 == RET_GOOD) //전역 변수에 따른 피커 상태를 체크 
		{//피커 정보와 현재 상태가 정상이다
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
// Retest Bin 처리 옵션으로 리테스트 빈은 위치정보 및 피커 정보등을  
// 함수 들어오기전에 미리정보를 셋팅하여 step을 jump 해야 함
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
					m_npPicker_YesNo[i] = CTL_NO; //작업할 피커정보 clear
					if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_YES)
					{
						if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF) 
						{
							if(st_picker[m_nRobotSite].st_pcb_info[i].nBin == BD_DATA_RETEST )
							{
								m_npPicker_YesNo[i] = CTL_YES;

								/*if(nTest_Site == TESTSITE_LEFT) //left site룰 작업하기 위한 작업 중일때
								{
									if(i >= 0 && m_nFirstTray_X_Num + (i - m_nFirstPicker_Num) < st_recipe_info.nRetestBuffer_Num)
									{
								
										nFind_JobPos = (st_recipe_info.nLdUldBuffer_Num - 1) - i ; //LEFT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는 3,2,1,0 로 반대이다 
										if(st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_NO)// && st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //트레이에 자재가  있으면 집기 가능하게 셋
										{
											nFlag = 1;
											m_npPicker_YesNo[i] = CTL_YES;
										}
									}
								}
								else if(nTest_Site == TESTSITE_RIGHT) //right sitet를 작업하기 위한 작업 중일때
								{
									if(i >= 0 && m_nFirstTray_X_Num + (i - m_nFirstPicker_Num) < st_recipe_info.nRetestBuffer_Num)
									{
										nFind_JobPos = i ; //RIGHT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는0,1,2,3 로 위치 순서가 동일하다  
										if(st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_NO)// && st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //트레이에 자재가  있으면 집기 가능하게 셋
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

		//작업 가능 위치를 찾는다
		nFlag = 0;
		m_nFirstTray_Y_Num = -1; m_nFirstTray_Y_Num = -1; m_nFirstPicker_Num = -1; //flag clear
		for(i = 0; i < m_nPickerPara; i++)
		{
			if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_YES && st_picker[m_nRobotSite].st_pcb_info[i].nEnable == CTL_YES)//피커에 자재가 들고 있다는 정보가 없다
			{
				if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF)
				{
					m_nFirstPicker_Num = i; //정보에 따라 피커 위치는 가변 
				}
				else //if(nWork_Site == THD_LD_BUFF || nWork_Site == THD_LD_BUFF || (nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8) )
				{
					m_nFirstPicker_Num = 0; //피커의 시작 위치는 고정으로 작업한다 
				}
				
				i = m_nPickerPara;
			}
		}

		if(m_nFirstPicker_Num == -1)
		{
			//피커에 더이상 작업할 것이 없다
			m_nPlace_Step = 0;
			nFuncRet = RET_PICKER_NOT_FIND;
			break;
		}

/* Retest Bin 처리 옵션으로 리테스트 빈은 위치정보 및 피커 정보등을 함수 들어오기전에 미리정보를 셋팅하여 step을 jump 해야 함
////////////////////////////////////  james 2016.0814
		if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF)
		{
			//Find_RetestBufferWork_Pos(int n_RbtSite, int n_TestSite, int n_BuffSite, int n_BuffFirstNo, int n_SortBin, int n_PickerYesNO, int n_BuffYesNO, int *np_FindPosYXCPB, int *np_Picker_YesNo)
			if(st_buffer_info[nWork_Site].nTestSite_LR_Pos == TESTSITE_LEFT) //test left site를 소팅 작업하기 위한 작업 중일때, 리테스트 버퍼는 항상 같은 기본 방향(Right Test Site 방향)을 유지한다, LEFT SITE에서 오면 버퍼를 회전하여 1번 핀 방향을 맞춘다 
			{//버퍼가 회전하여 시작 1번 위치가 반대로 되어있다 
				nRet_1 = clsFunc.Find_RetestBufferWork_Pos(THD_TEST_RBT, st_buffer_info[nWork_Site].nTestSite_LR_Pos, nWork_Site, 14, BIN_RETESTBIN, CTL_YES, CTL_NO, m_npFindWorkPosYXCPB, m_npPicker_YesNo); //14 == 짝수열 버퍼 우치 찾기 14,12,10,8,6,4,2,0
				if(nRet_1 == RET_GOOD)
				{
					m_nFirstTray_Y_Num = m_npFindWorkPosYXCPB[0]; m_nFirstTray_X_Num = m_npFindWorkPosYXCPB[1];
					m_nFirst_Buff_Num = m_npFindWorkPosYXCPB[4];
					m_nPlace_Step = 2000;
					break;
				}

				nRet_1 = clsFunc.Find_RetestBufferWork_Pos(THD_TEST_RBT, st_buffer_info[nWork_Site].nTestSite_LR_Pos, nWork_Site, 13, BIN_RETESTBIN, CTL_YES, CTL_NO, m_npFindWorkPosYXCPB, m_npPicker_YesNo); //13 == 홀수열 버퍼 우치 찾기 13,11,9,7,5,3,1
				if(nRet_1 == RET_GOOD)
				{
					m_nFirstTray_Y_Num = m_npFindWorkPosYXCPB[0]; m_nFirstTray_X_Num = m_npFindWorkPosYXCPB[1];
					m_nFirst_Buff_Num = m_npFindWorkPosYXCPB[4];
					m_nPlace_Step = 2000;
					break;
				}
				else
				{//작업을 할 수 없다 
					m_nPlace_Step = 0;
					nFuncRet = RET_TRAY_NOT_FIND;
					break;
				}
			}
			else if(st_buffer_info[nWork_Site].nTestSite_LR_Pos == TESTSITE_RIGHT) //test right site를 소팅 작업하기 위한 작업 중일때, 리테스트 버퍼는 항상 같은 기본 방향(Right Test Site 방향)을 유지한다, LEFT SITE에서 오면 버퍼를 회전하여 1번 핀 방향을 맞춘다 
			{//버퍼와 피커의 위치가 같아 정상이다
				nRet_1 = clsFunc.Find_RetestBufferWork_Pos(THD_TEST_RBT, st_buffer_info[nWork_Site].nTestSite_LR_Pos, nWork_Site, 0, BIN_RETESTBIN, CTL_YES, CTL_NO, m_npFindWorkPosYXCPB, m_npPicker_YesNo); //0 == 짝수열 버퍼 우치 찾기 0,2,4,6,8,10,12,14
				if(nRet_1 == RET_GOOD)
				{
					m_nFirstTray_Y_Num = m_npFindWorkPosYXCPB[0]; m_nFirstTray_X_Num = m_npFindWorkPosYXCPB[1];
					m_nFirst_Buff_Num = m_npFindWorkPosYXCPB[4];
					m_nPlace_Step = 2000;
					break;
				}

				nRet_1 = clsFunc.Find_RetestBufferWork_Pos(THD_TEST_RBT, st_buffer_info[nWork_Site].nTestSite_LR_Pos, nWork_Site, 1, BIN_RETESTBIN, CTL_YES, CTL_NO, m_npFindWorkPosYXCPB, m_npPicker_YesNo); //1 == 홀수열 버퍼 우치 찾기 1,3,5,7,9,11,13
				if(nRet_1 == RET_GOOD)
				{
					m_nFirstTray_Y_Num = m_npFindWorkPosYXCPB[0]; m_nFirstTray_X_Num = m_npFindWorkPosYXCPB[1];
					m_nFirst_Buff_Num = m_npFindWorkPosYXCPB[4];
					m_nPlace_Step = 2000;
					break;
				}
				else
				{//작업을 할 수 없다 
					m_nPlace_Step = 0;
					nFuncRet = RET_TRAY_NOT_FIND;
					break;
				}
			}
		}
		/////////////////////////////////// james 2016.0814 
*/


		nRet_1 = clsFunc.Find_TrayWork_Pos(0, nWork_Site, CTL_NO, m_npFindWorkPosYXCPB, THD_TEST_RBT, st_worker_info.strLotNo_Empty);
		if(nRet_1 == RET_GOOD) //작업 가능한 자재가 있다
		{
			if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF)
			{
				m_nFirstTray_Y_Num = m_npFindWorkPosYXCPB[0]; //사용하지 않음
				m_nFirstTray_X_Num = m_npFindWorkPosYXCPB[1];
			}
			else if(nWork_Site == THD_LD_BUFF || nWork_Site == THD_ULD_BUFF) 
			{
				m_nFirstTray_Y_Num = 0;
				m_nFirstTray_X_Num = 0;
			}
			else if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8)
			{
				m_nFirstTray_Y_Num = nStart_FixPos;//사용하지 않음 m_nFirstTray_Y_Num = st_test_socket_info[st_sync_info.nTestSite_Target_THD_Work_Site].nWorkSocket_StartPos; 
				m_nFirstTray_X_Num = nStart_FixPos; //0 또는 4 임  st_test_socket_info[st_sync_info.nTestSite_Target_THD_Work_Site].nWorkSocket_StartPos;
			}
		}
		else if(nRet_1 == RET_SKIP) //더이상 자재가 없다
		{	//계속 진행할 수 없다
			m_nPlace_Step = 0;
			nFuncRet = RET_TRAY_NOT_FIND;
			break;
		}

		nFlag = 0;
		if(m_nFirstPicker_Num != -1 && m_nFirstTray_Y_Num != -1 && m_nFirstTray_X_Num != -1) //작업할 공간이 다 있을때 다음 작업을 진행한다
		{
			for(i = 0; i < m_nPickerPara; i++)  //m_n_FirstPicker_Num; y < m_mPickerPara; y++)
			{
				m_npPicker_YesNo[i] = CTL_NO; //작업할 피커정보 clear
				if(i < m_nFirstPicker_Num) continue;
				if(st_picker[m_nRobotSite].st_pcb_info[i].nYesNo == CTL_YES)// && st_picker[m_nRobotSite].st_pcb_info[i].nEnable == CTL_YES)
				{
					if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF) //테스트 로봇이 개별 동작을 하지 못해 피커와 버퍼의 정보를 위치에서 1:1 로 비교하여 충돌이 없어야 한다 
					{//리테스트 조건은 이곳으로 오지 않는다, 사용하지 않음  
						if(st_picker[m_nRobotSite].st_pcb_info[i].nBin == BD_DATA_RETEST )//|| st_picker[m_nRobotSite].st_pcb_info[i].nBin == BD_DATA_REJECT)
						{
							if(i >= 0 && m_nFirstTray_Y_Num + (i - m_nFirstPicker_Num) < st_recipe_info.nLdUldBuffer_Num)//4
							{
								if(nTest_Site == TESTSITE_LEFT) //left site룰 작업하기 위한 작업 중일때
								{
									nFind_JobPos = (st_recipe_info.nLdUldBuffer_Num - 1) - i ; //LEFT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는 3,2,1,0 로 반대이다 
									if(st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_NO)// && st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //트레이에 자재가  있으면 집기 가능하게 셋
									{
										m_npPicker_YesNo[i] = CTL_YES;
										nFlag++;
									}
								}
								else if(nTest_Site == TESTSITE_RIGHT) //right sitet를 작업하기 위한 작업 중일때
								{
									nFind_JobPos = i ; //RIGHT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는0,1,2,3 로 위치 순서가 동일하다  
									if(st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_NO)// && st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //트레이에 자재가  있으면 집기 가능하게 셋
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
							if(nTest_Site == TESTSITE_LEFT) //left site룰 작업하기 위한 작업 중일때
							{
								nFind_JobPos = (st_recipe_info.nLdUldBuffer_Num - 1) - i ; //LEFT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는 3,2,1,0 로 반대이다 
								if(st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_NO)// && st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //트레이에 자재가  있으면 집기 가능하게 셋
								{
									m_npPicker_YesNo[i] = CTL_YES;
									nFlag++;
								}
							}
							else if(nTest_Site == TESTSITE_RIGHT) //right sitet를 작업하기 위한 작업 중일때
							{
								nFind_JobPos = i ; //RIGHT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는0,1,2,3 로 위치 순서가 동일하다  
								if(st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_NO)// && st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //트레이에 자재가  있으면 집기 가능하게 셋
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
								if(nTest_Site == TESTSITE_LEFT) //left site룰 작업하기 위한 작업 중일때
								{
									nFind_JobPos = (st_recipe_info.nLdUldBuffer_Num - 1) - i ; //LEFT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는 3,2,1,0 로 반대이다 
									if(st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_NO)// && st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //트레이에 자재가  있으면 집기 가능하게 셋
									{
										m_npPicker_YesNo[i] = CTL_YES;
										nFlag++;
									}
								}
								else if(nTest_Site == TESTSITE_RIGHT) //right sitet를 작업하기 위한 작업 중일때
								{
									nFind_JobPos = i ; //RIGHT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는0,1,2,3 로 위치 순서가 동일하다  
									if(st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_NO)// && st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //트레이에 자재가  있으면 집기 가능하게 셋
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
							nFind_JobPos = (st_recipe_info.nTestSocket_Num - 1) - i ; //LEFT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는 3,2,1,0 로 반대이다 
							nFind_JobPos = nFind_JobPos - nStart_FixPos; //st_test_site_info[nWork_Site].nStart_FixPos;


							//////////////////////////////////////////////////////////////////////////
							//2017.0320
							//820000 1 0 "REMOVE_TESTSITE_PICKER1_DVC_BECAUSE_TESTSITE_SCOKET_IS_DISABLE."
							//820001 1 0 "REMOVE_TESTSITE_PICKER2_DVC_BECAUSE_TESTSITE_SCOKET_IS_DISABLE."
							//820002 1 0 "REMOVE_TESTSITE_PICKER3_DVC_BECAUSE_TESTSITE_SCOKET_IS_DISABLE."
							//820003 1 0 "REMOVE_TESTSITE_PICKER4_DVC_BECAUSE_TESTSITE_SCOKET_IS_DISABLE."
							//만일 소켓이 disable되어 있는데 자재를 집어넣을려고 한다면 여기서 알람을 발생하여 작업자가 자재를 뺄 수 있도록 한다.
// 							if( st_test_site_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable != CTL_YES && st_test_site_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_NO)
// 							{//알람 발생
// 								strTemp.Format(_T(" TESTRBT was goint to TESTSITE %d, SOCKET %d was disabled. so It have to be removed at Picker %d device"), nWork_Site - THD_TESTSITE_1 + 1, nFind_JobPos, i);
// 								clsMem.OnAbNormalMessagWrite(strTemp);//로그 저장
// 
// 								m_strAlarmCode.Format(_T("8200%02d"), i); 
// 								CTL_Lib.Alarm_Error_Occurrence(2728, dWARNING, m_strAlarmCode);
// 								return nFuncRet;
// 							}
							//////////////////////////////////////////////////////////////////////////
							if(st_test_site_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_NO )//&& st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //트레이에 자재가  있으면 집기 가능하게 셋
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
							nFind_JobPos = i ; //RIGHT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는0,1,2,3 로 위치 순서가 동일하다  
							nFind_JobPos = nFind_JobPos + nStart_FixPos; //st_test_site_info[nWork_Site].nStart_FixPos;

							//////////////////////////////////////////////////////////////////////////
							//2017.0320
							//만일 소켓이 disable되어 있는데 자재를 집어넣을려고 한다면 여기서 알람을 발생하여 작업자가 자재를 뺄 수 있도록 한다.
// 							if( st_test_site_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable != CTL_YES && st_test_site_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_NO)
// 							{//알람 발생
// 								strTemp.Format(_T(" TESTRBT was goint to TESTSITE %d, SOCKET %d was disabled. so It have to be removed at Picker %d device "), nWork_Site - THD_TESTSITE_1 + 1, nFind_JobPos, i);
// 								clsMem.OnAbNormalMessagWrite(strTemp);//로그 저장
// 
// 								m_strAlarmCode.Format(_T("8200%02d"), i); 
// 								CTL_Lib.Alarm_Error_Occurrence(2729, dWARNING, m_strAlarmCode);
// 								return nFuncRet;
// 							}
							//////////////////////////////////////////////////////////////////////////

							if(st_test_site_info[nWork_Site].st_pcb_info[nFind_JobPos].nYesNo == CTL_NO)// && st_buffer_info[nWork_Site].st_pcb_info[nFind_JobPos].nEnable == CTL_YES) //트레이에 자재가  있으면 집기 가능하게 셋
							{
								m_npPicker_YesNo[i] = CTL_YES;
								nFlag++;
							}
						}
					}
				}
			}

			if(nFlag == 0) //작업할 피커가 하나도 준비되 않았다.
			{ //이부분이 아오면 안됨 
				m_nPlace_Step = 0;
				nFuncRet = RET_PICKER_NOT_FIND;//피커에 더이상 작업할 것이 없다
				break;
			}

			//실제 작업 위치를 셋팅
			m_npFindWorkPosYXCPB[0] = m_nFirstTray_Y_Num; m_npFindWorkPosYXCPB[1] = m_nFirstTray_X_Num; m_npFindWorkPosYXCPB[3] = m_nFirstPicker_Num;

			m_nMove_Flag[0] = CTL_NO; m_nMove_Flag[1] = CTL_NO;
			m_nPlace_Step = 2000;
		}		
		break;

	case 2000:
		//james 2016.0826 피커와 버퍼/테스트 소켓의 자재 충돌방지을 위해 추가 
		if(nWork_Site == THD_LD_BUFF || nWork_Site == THD_ULD_BUFF)
		{//Check_Robot_Buffer_Clash(int nTestDirection, int nRbtTHDSite, int TestBuffTHDSite, int nStartPos, int *nErrorInfoStatus) 
			if(nTest_Site == TESTSITE_LEFT) //LEFT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는 3,2,1,0 로 반대이다 
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

				nRet_1 = clsFunc.Check_Robot_Buffer_Clash(TESTSITE_LEFT, THD_TEST_RBT, nWork_Site, m_nFirstTray_X_Num, m_nFirstPicker_Num, m_nClash_ErrorInfoStatus); //피커와 버퍼등의 자재 충돌 확인 
				if(nRet_1 == RET_ERROR)
				{
					CTL_Lib.Alarm_Error_Occurrence(2410, dWARNING, clsFunc.m_strAlarmCode);		
					break;
				}
			}
			else if(nTest_Site == TESTSITE_RIGHT) //right sitet를 작업하기 위한 작업 중일때//RIGHT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는0,1,2,3 로 위치 순서가 동일하다  
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

				nRet_1 = clsFunc.Check_Robot_Buffer_Clash(TESTSITE_RIGHT, THD_TEST_RBT, nWork_Site, m_nFirstTray_X_Num, m_nFirstPicker_Num, m_nClash_ErrorInfoStatus); //피커와 버퍼등의 자재 충돌 확인 
				if(nRet_1 == RET_ERROR)
				{
					CTL_Lib.Alarm_Error_Occurrence(2430, dWARNING, clsFunc.m_strAlarmCode);		
					break;
				}
			}
		}
		else  if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_4 ) //left site
		{
			nRet_1 = clsFunc.Check_Robot_Buffer_Clash(TESTSITE_LEFT, THD_TEST_RBT, nWork_Site, m_nFirstTray_X_Num, m_nFirstPicker_Num, m_nClash_ErrorInfoStatus); //피커와 버퍼등의 자재 충돌 확인 
			if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(2440, dWARNING, clsFunc.m_strAlarmCode);		
				break;
			}
		}
		else  if(nWork_Site >= THD_TESTSITE_5 && nWork_Site <= THD_TESTSITE_8 ) //right site
		{
			nRet_1 = clsFunc.Check_Robot_Buffer_Clash(TESTSITE_RIGHT, THD_TEST_RBT, nWork_Site, m_nFirstTray_X_Num, m_nFirstPicker_Num, m_nClash_ErrorInfoStatus); //피커와 버퍼등의 자재 충돌 확인 
			if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(2450, dWARNING, clsFunc.m_strAlarmCode);		
				break;
			}
		}//
		else if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF)
		{
			nRet_1 = clsFunc.Check_Robot_Buffer_Clash(nTest_Site, THD_TEST_RBT, nWork_Site, m_nFirstTray_X_Num, m_nFirstPicker_Num, m_nClash_ErrorInfoStatus); //피커와 버퍼등의 자재 충돌 확인 
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
	// 소켓 1번 핀 방향 체크 루틴
	////////////////////////////////////////////////////////////////////////
	case 2010:
		m_dCurrentPos[2] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[2] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벅어나 있으면 저위치 시킨다
		{
			m_nPlace_Step = 900;
			break;
		}
		m_dCurrentPos[m_nRobot_Ejector] = COMI.Get_MotCurrentPos(m_nRobot_Ejector);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[m_nRobot_Ejector] > st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벗어나 있으면 저위치 시킨다
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
	//		if (nRet_1 == BD_GOOD) //좌측으로 이동
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
	//		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
	//			CTL_Lib.Alarm_Error_Occurrence(2470, dWARNING, st_alarm_info.strCode);
	//			m_nPlace_Step = 2000;
	//		}
	//	}
	//	else
	//	{
	//		m_nPlace_Step = 2100;
	//	}
		//2017.1206  //Retest1이 TestSite1~4번 인지 Retest2가 TestSite5~8번 인지 체크
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
			if (nRet_1 == BD_GOOD) //좌측으로 이동
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
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
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
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_bDvcWaitChk_Falg = false;
			m_nPlace_Step = 2030;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 2020;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(2480, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 2020;
		}
		break;

	case 2030:// 소켓 1번 핀 방향 체크 루틴 //james 2016.0827 
		if (st_basic_info.nModeDevice == WITHOUT_DVC)
		{
			m_nPlace_Step = 2040; //정상
			break;
		}
		////kwlee 2016.0908 
		////if(st_test_site_info[nWork_Site].nSite_Pos == TESTSITE_LEFT)
		//if(nTest_Site == TESTSITE_LEFT) 
		//{
		//	nRet_1 = FAS_IO.get_in_bit(st_io_info.i_DvcDir1Chk,	IO_ON);	//left test site 위치 방향임	 
		//	nRet_2 = FAS_IO.get_in_bit(st_io_info.i_DvcDir2Chk,	IO_OFF);	
		//	if(nRet_1 == IO_ON && nRet_2 == IO_OFF)
		//	{
		//		m_nPlace_Step = 2040; //정상
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
		//	nRet_1 = FAS_IO.get_in_bit(st_io_info.i_DvcDir1Chk,	IO_OFF);	//fight test site 위치 방향임	 
		//	nRet_2 = FAS_IO.get_in_bit(st_io_info.i_DvcDir2Chk,	IO_ON);	
		//	//kwlee 20160908
		//	//if(nRet_1 == IO_ON && nRet_2 == IO_OFF)
		//	if( nRet_1 == IO_OFF && nRet_2 == IO_ON)
		//	{
		//		m_nPlace_Step = 2040; //정상
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
			m_nPlace_Step = 2040; //정상
			break;
		}

		m_dwWaitUntil[1] = GetCurrentTime();
		m_dwWaitUntil[2] = m_dwWaitUntil[1] - m_dwWaitUntil[0];
		if( m_dwWaitUntil[2] <= 0 ) m_dwWaitUntil[0] = GetCurrentTime();
		if(m_dwWaitUntil[2] < (IO_STABLE_WAIT*5))  break; //500msec

		//if(nTest_Site == TESTSITE_LEFT) 
		//{
		//	nRet_1 = FAS_IO.get_in_bit(st_io_info.i_DvcDir1Chk,	IO_ON);	//left test site 위치 방향임	 
		//	nRet_2 = FAS_IO.get_in_bit(st_io_info.i_DvcDir2Chk,	IO_OFF);	
		//	if(nRet_1 == IO_ON && nRet_2 == IO_OFF)
		//	{
		//		m_nPlace_Step = 2040; //정상
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
		//	nRet_1 = FAS_IO.get_in_bit(st_io_info.i_DvcDir1Chk,	IO_OFF);	//fight test site 위치 방향임	 
		//	nRet_2 = FAS_IO.get_in_bit(st_io_info.i_DvcDir2Chk,	IO_ON);	
		//	if( nRet_1 == IO_OFF && nRet_2 == IO_ON)
		//	{
		//		m_nPlace_Step = 2040; //정상
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
				m_nPlace_Step = 2040; //정상
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

		// 작업자처리 
	case 2035://2016.0910 
		m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY];
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, m_dpTargetPosList[2], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //좌측으로 이동
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
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(2490, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 2035;
		}		
		break; 

	case 2036://벨트라서 오차가 있다 잠깐 뒤로 빠졌다가 다시 체크 한다. 5mm
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
			if (nRet_1 == BD_GOOD) //좌측으로 이동
			{
				m_nPlace_Step = 2010;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nPlace_Step = 2036;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
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
		if (nRet_1 == BD_GOOD) //좌측으로 이동
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
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
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
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{ 			
			m_nPlace_Step = 2100;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 2040;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(2520, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 2040;
		}		
		break; 	

	case 2100:
		m_dCurrentPos[2] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[2] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벅어나 있으면 저위치 시킨다
		{
			m_nPlace_Step = 900;
			break;
		}
		m_dCurrentPos[m_nRobot_Ejector] = COMI.Get_MotCurrentPos(m_nRobot_Ejector);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[m_nRobot_Ejector] > st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벗어나 있으면 저위치 시킨다
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
		else if(nWork_Site == THD_LD_BUFF || nWork_Site == THD_ULD_BUFF ) //james 2016.0922 조건 추가 
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
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_bDvcWaitChk_Falg =  false;
			m_nPlace_Step = 2200;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 2100;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(2540, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 2100;
		}
		break; 	

	case 2200:
		if(nWork_Site == THD_LD_BUFF || nWork_Site == THD_ULD_BUFF)
		{
			//로딩 버퍼에 요청하여 자재가 준비되어 CTL_READY 된 상태, 자재를 집을 수 있다 
			 if(st_sync_info.nTestRbt_Dvc_Req[nWork_Site][0] == CTL_READY && st_sync_info.nTestRbt_Dvc_Req[nWork_Site][1] == WORK_PLACE) //로딩 버퍼가 준비되기를 기다린다 
			 {				 ////james 2016.1005 = CTL_READY
				 m_nPlace_Step = 3000;
			 }		
		}
		//else  if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF || (nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8))
		else  if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF)
		{//집을 일이 없음 
			if(st_sync_info.nTestRbt_Dvc_Req[nWork_Site][0] == CTL_READY && st_sync_info.nTestRbt_Dvc_Req[nWork_Site][1] == WORK_PLACE) //버퍼/test site가 준비되기를 기다린다 
			{				 
				 m_nPlace_Step = 3000;
			}	
		}	
		else if ((nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8))
		{
			if(st_sync_info.nTestRbt_Dvc_Ld_Req[nWork_Site][0] == CTL_READY && st_sync_info.nTestRbt_Dvc_Ld_Req[nWork_Site][1] == WORK_PLACE) //버퍼/test site가 준비되기를 기다린다 
			{				 
				 m_nPlace_Step = 3000;
			}
		}
		break;

	case 3000:  
		m_dCurrentPos[m_nRobot_Ejector] = COMI.Get_MotCurrentPos(m_nRobot_Ejector);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[m_nRobot_Ejector] > st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벗어나 있으면 저위치 시킨다
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

		//Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON); //미리 벌린다  

		if(nWork_Site == THD_LD_BUFF)
		{
			m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_LDBUF_PLACE];
		}
		else if(nWork_Site == THD_ULD_BUFF)
		{//집을 일이 없음 
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

		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, m_dpTargetPosList[2], (int) (COMI.mn_runspeed_rate / 3)); //속도 다운 
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{ 			
			m_nPlace_Step = 3100;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 3000;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(2630, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 3000;
		}		
		break;	

	case 3100: //자재를 놓는다 
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
	// load/unload buffer에서 로딩 자재를 집을떄는 버퍼의 그리퍼 제어는 로봇에서 한다 
	// 그외 사이트는 그리퍼가 없어서 그럴 필요 없음 
	/////////////////////////////////////////////////////////////////////////
	case 3110:
		if(nWork_Site == THD_LD_BUFF || nWork_Site == THD_ULD_BUFF)
		{
			for(i = 0; i < m_nPickerPara; i++)  //4
			{
				if(nTest_Site == TESTSITE_LEFT) //left site룰 작업하기 위한 작업 중일때
				{
					m_npBuffer_YesNo[i] =  m_npPicker_YesNo[(m_nPickerPara - 1) - i]; //3,2,1,0 으로 반대로 작업위치 지정 
				}
				else if(nTest_Site == TESTSITE_RIGHT) //right sitet를 작업하기 위한 작업 중일때
				{
					m_npBuffer_YesNo[i] = m_npPicker_YesNo[i];
				}
			}

			 if(st_sync_info.nTestRbt_Dvc_Req[nWork_Site][0] == CTL_READY && st_sync_info.nTestRbt_Dvc_Req[nWork_Site][1] == WORK_PLACE) //로딩 버퍼가 준비되어 있다 
			 {				 //james 2016.1005 = CTL_READY
				 m_nPlace_Step = 3120;
			 }		
		}
		else  //if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF || nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8)
		{//집을 일이 없음 
			 m_nPlace_Step = 3200;
		}		
		break;

	/////////////////////////////////////////////////////////////////////////
	// load/unload buffer에서 로딩 자재를 집을떄는 버퍼의 그리퍼 제어는 로봇에서 한다 
	// load buffer 의 그리퍼 중 로보트가 집은 자재 위치의 그리퍼만 open하여 로봇이 집어 갈 수 있게 한다
	/////////////////////////////////////////////////////////////////////////
	case 3120:
		if(nWork_Site == THD_LD_BUFF)
		{
			clsRunDvcLdBuffer.Set_Gripper_OnOff(0, IO_OFF, m_npBuffer_YesNo); //로딩 버퍼의 그리퍼를 open하여 로봇이 자재를 집어 갈 수 있게 한다 
		}
		else if(nWork_Site == THD_ULD_BUFF)
		{
			clsRunDvcUldBuffer.Set_Gripper_OnOff(0, IO_OFF, m_npBuffer_YesNo); //로딩 버퍼의 그리퍼를 open하여 로봇이 자재를 집어 갈 수 있게 한다 
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

	case 3140:		//james 2016.0922 추가 
		nRet_1 = clsFunc.Check_BufferStatus(1, nWork_Site, CTL_YES, m_npBuffer_YesNo, m_npBuff_Status, m_npBuff_OutputStatus); //전역변수와 센서 상태가 일치해야 한다 
		if(nRet_1 == RET_GOOD)
		{//정상 상태, //Enable 되어 있는 버퍼 위치는 모두 채워진 상태, 테스트 로봇 사이트로 이송가능하다 
			m_bDvcWaitChk_Falg = false;
			m_nPlace_Step = 3200;
		}
		else if(nRet_1== RET_ERROR)
		{//아직 버버 채우는 작업이 끝나지 않은 상태 
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
		if(nRet_1 == RET_GOOD) //전역 변수에 따른 피커 상태를 체크 
		{//피커 정보와 현재 상태가 정상이다
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
		Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON);  //자재를 놓는다 
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
			Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON);  //에러니 벌린다 
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
// 			//kwlee 2017.0918 자재 파손 관련 del
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
		//2017.0918 Retest Buffer Place 후 확인 수정 예정.

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

		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, m_dpTargetPosList[2], (int) (COMI.mn_runspeed_rate / 3)); //속도 다운 
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_nPlace_Step = 4105;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 4102;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(2791, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 4102;
		}
		break;

		//Place 후 센서 확인.
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
		//Missloading 티칭한 높이로 상승후 체크
	case 4106:

		if (st_basic_info.nMissLoading_Mode == CTL_YES)
		{
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_MissLoadingChk1, IO_ON);
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_MissLoadingChk2, IO_ON);
			if(nRet_1 == IO_ON || nRet_2 == IO_ON) 
			{
				m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_MissLoadingChk1); //Miss loading 센서 에러
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
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_dwWaitUntil[0] = GetCurrentTime();
			m_nPlace_Step = 4107;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 4106;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(2730, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 4106;
		}		
		break;

	case 4107:
		nRet_1 = FAS_IO.get_in_bit(st_io_info.i_MissLoadingChk1, IO_ON);
		nRet_2 = FAS_IO.get_in_bit(st_io_info.i_MissLoadingChk2, IO_ON);

		//if(nRet_1 == IO_ON && nRet_2 == IO_ON)
		//kwlee 20171119 감지 시 알람.
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
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_MissLoadingChk1); //Miss loading 센서 에러
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
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(2731, dWARNING, st_alarm_info.strCode);
			m_nPlace_Step = 4109;
		}		
		break;
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	case 4110: //test site 자재 INSERT 
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
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{ 			
			m_nPlace_Step = 4200;
			m_nPlace_Step = 4120;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 4110;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
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
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_bDvcWaitChk_Falg = false;
			m_nPlace_Step = 5000;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nPlace_Step = 4200;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
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
						m_npPicker_YesNo[i] = CTL_NO; //작업한 피커 정보 클리어

						if(nTest_Site == TESTSITE_LEFT) //left site룰 작업하기 위한 작업 중일때
						{
							nFind_JobPos = (st_recipe_info.nLdUldBuffer_Num - 1) - i ; //LEFT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는 3,2,1,0 로 반대이다 
								

							//아래 함수에서 사용위해 위치 정보를 셋팅한다
							m_npSet_WorkPosYXCPB[0]  = nFind_JobPos; //버퍼sms Y만 사용 //버퍼의 위치 지정
							m_npSet_WorkPosYXCPB[1]  = nFind_JobPos; //qjvjsms X는 사용하지 않음 //버퍼의 위치 지정
							m_npSet_WorkPosYXCPB[3]  = i; //picker 위치 정보

							clsFunc.Data_Exchange_PickPlace(PICKER_PLACE_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //피커 및 트레이 정보 셋팅 함수
							//위 함수에서 모든 정보를 셋팅 및 클리어 한다
							//st_picker[m_nRobotSite].st_pcb_info[y].nYesNo = CTL_YES;					//피커 정보 셋
							//st_tray_info[nWork_Site].st_pcb_info[m_npFirstPosYXP[0] + (y - m_n_FirstPicker_Num)][m_npFirstPosYXP[1]].nYesNo = CTL_NO; //트레이 정보 클리어
							//st_count_info.nSSD_Input[0]++;
						}
						else if(nTest_Site == TESTSITE_RIGHT) //RIGHT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는0,1,2,3 로 위치 순서가 동일하다 
						{
							nFind_JobPos = i ; //RIGHT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는0,1,2,3 로 위치 순서가 동일하다  

							//아래 함수에서 사용위해 위치 정보를 셋팅한다
							m_npSet_WorkPosYXCPB[0]  = nFind_JobPos; //버퍼sms Y만 사용 //버퍼의 위치 지정
							m_npSet_WorkPosYXCPB[1]  = nFind_JobPos; //qjvjsms X는 사용하지 않음 //버퍼의 위치 지정
							m_npSet_WorkPosYXCPB[3]  = i; //picker 위치 정보

							clsFunc.Data_Exchange_PickPlace(PICKER_PLACE_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //피커 및 트레이 정보 셋팅 함수 
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
						m_npPicker_YesNo[i] = CTL_NO; //작업한 피커 정보 클리어

						///////////////////////////////////////////////////////////////////////////////////////////////
						//2016.1203
						if (m_strLotNo[1] == _T(""))
						{
							m_strLotNo[1]	= st_picker[THD_TEST_RBT].st_pcb_info[i].strLotNo;
							m_strPartNo[1]	= st_picker[THD_TEST_RBT].st_pcb_info[i].strPartNo;
						}
						///////////////////////////////////////////////////////////////////////////////////////////////

						if(nTest_Site == TESTSITE_LEFT) //left site룰 작업하기 위한 작업 중일때
						{
							nFind_JobPos = (st_recipe_info.nTestSocket_Num - 1) - i ; //LEFT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는 3,2,1,0 로 반대이다 
							nFind_JobPos = nFind_JobPos - nStart_FixPos; //st_test_site_info[nWork_Site].nStart_FixPos;								

							//아래 함수에서 사용위해 위치 정보를 셋팅한다
							m_npSet_WorkPosYXCPB[0]  = nFind_JobPos; //Test Site는 Y는 용하지 않음 //버퍼의 위치 지정
							m_npSet_WorkPosYXCPB[1]  = nFind_JobPos; //Test Site는 X만 사용 //버퍼의 위치 지정
							m_npSet_WorkPosYXCPB[3]  = i; //picker 위치 정보

							clsFunc.Data_Exchange_PickPlace(PICKER_PLACE_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //피커 및 트레이 정보 셋팅 함수
							//위 함수에서 모든 정보를 셋팅 및 클리어 한다
							//st_picker[m_nRobotSite].st_pcb_info[y].nYesNo = CTL_YES;					//피커 정보 셋
							//st_tray_info[nWork_Site].st_pcb_info[m_npFirstPosYXP[0] + (y - m_n_FirstPicker_Num)][m_npFirstPosYXP[1]].nYesNo = CTL_NO; //트레이 정보 클리어
							//st_count_info.nSSD_Input[0]++;
						}
						else if(nTest_Site == TESTSITE_RIGHT) //RIGHT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는0,1,2,3 로 위치 순서가 동일하다 
						{
							nFind_JobPos = i ; //LEFT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는 3,2,1,0 로 반대이다 
							nFind_JobPos = nFind_JobPos + nStart_FixPos; //st_test_site_info[nWork_Site].nStart_FixPos;								

							//아래 함수에서 사용위해 위치 정보를 셋팅한다
							m_npSet_WorkPosYXCPB[0]  = nFind_JobPos; //Test Site는 Y는 용하지 않음 //버퍼의 위치 지정
							m_npSet_WorkPosYXCPB[1]  = nFind_JobPos; //Test Site는 X만 사용 //버퍼의 위치 지정
							m_npSet_WorkPosYXCPB[3]  = i; //picker 위치 정보

							clsFunc.Data_Exchange_PickPlace(PICKER_PLACE_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //피커 및 트레이 정보 셋팅 함수 
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
					if(nTest_Site == TESTSITE_LEFT) //left site룰 작업하기 위한 작업 중일때
					{
						nFind_JobPos = m_nFirst_Buff_Num - ((i - m_nFirstPicker_Num) * 2);  //13,11,9,7,5,3,1 위치 
						if(i >= 0 && nFind_JobPos >= 0 && nFind_JobPos < st_recipe_info.nRetestBuffer_Num) //2016.0906 if(i >= 0 && m_npFindWorkPosYXCPB[1] + (i - m_nFirstPicker_Num) < st_recipe_info.nRetestBuffer_Num)
						{
							//아래 함수에서 사용위해 위치 정보를 셋팅한다
							m_npSet_WorkPosYXCPB[0]  = nFind_JobPos; //버퍼sms Y만 사용 //버퍼의 위치 지정
							m_npSet_WorkPosYXCPB[1]  = nFind_JobPos; //qjvjsms X는 사용하지 않음 //버퍼의 위치 지정
							m_npSet_WorkPosYXCPB[3]  = i; //picker 위치 정보

							clsFunc.Data_Exchange_PickPlace(PICKER_PLACE_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //피커 및 트레이 정보 셋팅 함수
							//위 함수에서 모든 정보를 셋팅 및 클리어 한다
							//st_picker[m_nRobotSite].st_pcb_info[y].nYesNo = CTL_YES;					//피커 정보 셋
							//st_tray_info[nWork_Site].st_pcb_info[m_npFirstPosYXP[0] + (y - m_n_FirstPicker_Num)][m_npFirstPosYXP[1]].nYesNo = CTL_NO; //트레이 정보 클리어
							//st_count_info.nSSD_Input[0]++;
							//st_buffer_info[nWorkSite].st_pcb_info[npWorkPos[0]]
							if (st_buffer_info[nWork_Site].st_pcb_info[m_npSet_WorkPosYXCPB[0]].nYesNo == YES)
							{
								st_count_info.nPrimeRejectCount[1][0]++;

								if (st_lot_info[m_nLotProcessNum].strLotNo == st_buffer_info[nWork_Site].st_pcb_info[m_npSet_WorkPosYXCPB[0]].strLotNo)
								{
									if(m_nLotProcessNum == LOT_CURR)
									{
										// 현재 진행중인 Lot과 같으면
										//st_lot_info[LOT_CURR].nLdCnt++;
										st_count_info.nPrimeRejectCount[0][0]++;
									}
									else if(m_nLotProcessNum == LOT_NEXT)
									{
										// next lot 수량 증가
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
					else if(nTest_Site == TESTSITE_RIGHT) //RIGHT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는0,1,2,3 로 위치 순서가 동일하다 
					{
						nFind_JobPos = m_nFirst_Buff_Num + ((i - m_nFirstPicker_Num) * 2);
						if(i >= 0 && nFind_JobPos >= 0  && nFind_JobPos < st_recipe_info.nRetestBuffer_Num) //2016.0906 if(i >= 0 && m_npFindWorkPosYXCPB[1] + (i - m_nFirstPicker_Num) < st_recipe_info.nRetestBuffer_Num)
						{
							//아래 함수에서 사용위해 위치 정보를 셋팅한다
							m_npSet_WorkPosYXCPB[0]  = nFind_JobPos; //버퍼sms Y만 사용 //버퍼의 위치 지정
							m_npSet_WorkPosYXCPB[1]  = nFind_JobPos; //qjvjsms X는 사용하지 않음 //버퍼의 위치 지정
							m_npSet_WorkPosYXCPB[3]  = i; //picker 위치 정보

							clsFunc.Data_Exchange_PickPlace(PICKER_PLACE_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //피커 및 트레이 정보 셋팅 함수 

							if (st_buffer_info[nWork_Site].st_pcb_info[m_npSet_WorkPosYXCPB[0]].nYesNo == YES)
							{
								st_count_info.nPrimeRejectCount[1][0]++;

								if (st_lot_info[m_nLotProcessNum].strLotNo == st_buffer_info[nWork_Site].st_pcb_info[m_npSet_WorkPosYXCPB[0]].strLotNo)
								{
									if(m_nLotProcessNum == LOT_CURR)
									{
										// 현재 진행중인 Lot과 같으면
										//st_lot_info[LOT_CURR].nLdCnt++;
										st_count_info.nPrimeRejectCount[0][0]++;
									}
									else if(m_nLotProcessNum == LOT_NEXT)
									{
										// next lot 수량 증가
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

		if (nRet_1 == RET_GOOD) // 1000 step에서 셋팅한 피커 정보에 맞게 모두 정상적 집었을 경우 이
		{
			//화면 정보 갱신
			// jtkim 20160930 Log
			if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8)
			{
				//clsLog.OnLogTestSite(0, nWork_Site, st_test_site_info[nWork_Site]);
				// jtkim 20161008 최희택책임 요청
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

			// jtkim 20150216 conveyor/loader tray 화면 표시
			if (st_handler_info.cWndMain != NULL)
			{
				st_handler_info.cWndMain->PostMessage(WM_WORK_DISPLAY, MAIN_LD_PICKER_DISPLAY, 0);
				st_handler_info.cWndMain->PostMessage(WM_WORK_DISPLAY, MAIN_LD_TRAY_DISPLAY, 0);
				st_handler_info.cWndMain->PostMessage(WM_WORK_DISPLAY, MAIN_RETEST_BUF_DISPLAY, 0);
			}

			//2017.0109
			//kwlee 2017.0117
		
			//st_picker[m_nRobotSite].st_pcb_info[i].strLogData 프로그램 죽어서 막아놓음..
			//확인 필요..
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
		else if (nRet_1 == RET_ERROR) //셋팅한 피커중 한개라도 제대로 집지 못했으면 이곳으로 이동
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



//james 2016.0814 리테스트 방법 정의
//자재 pick & place 방법 (피커가 전체 Up/Down으로 동작시 자재간 충돌이 발생할 수 있어 많은 주의가 필요하다 //
//1차 작업구간 : 0,2,4,6, 8,10,12,14 - 8개 
//2차 작업구간 : 1,3,5,7, 9,11,13    - 7개
//
//리테스트 버퍼사용방법: 한 LOT에 하나씩만 사용한다, 연속LOT 처리 때문
// Left/Right Tets Site에서 나오는 자재의 PIN 방향이 반대여서 어느 사이트에서 온것인지 확인하여 회전하면서 받아야 한다 
// 이때 받은 버퍼는 L/R 구분없이 해당 lot은 한곳에 모두 받아, 처리한다.    
//
// PLACE 방법 놓을때.
// 1. 1차 작업구간(짝수열) 까지는 무조건 한번에 놓을 공간을 찾아 놓는다. 
// 2. 2차 작업구간(홀수열) 까지 자재를 넣을 상황이면 , 이것 까지만 놓고, 이제는 1차 작업 구간의 자재를 리테스트를 진행한다 
// 3. 매 사이클 자재 언로딩 소팅후, 혹시 홀수열의 리테스트 버퍼에 자재가 있으면 이 자재를 짝수열로 최대한 정렬해 준다 
// 4. 즉 홀수열은 임시 버퍼 공간으로 무조건 비워져 있어야 한다

//현재는 사용하지 않는다 (james 2016.0821 현재는 Place()에 통합하여적용 중)
int CRunTestSiteRobot::Process_BinSort_Place(int nMode, int nWork_Site, int nSortBin) //retest 자재를 소팅 한다 
{
	int nFuncRet = RET_PROCEED;
	int nRet_1, nRet_2, nRet_3 = 0, i, nFlag;
	int nFind_JobPos=0, nCount=0;

	int nBuff_x = 0;
	int nPicker_x =0; 
	int nPicker_Cnt = 0;
	int nBuffer_Cnt = 0;
	int nBuffFirstNo= 0;	

	int nStart_FixPos = 0; //james 2016.0905 test 삭제할것 
	
	 
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
		m_dCurrentPos[m_nRobot_Z] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[m_nRobot_Z] > st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벗어나 있으면 저위치 시킨다
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
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_nSortPlace_Step = 920;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nSortPlace_Step = 900;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(2610, dWARNING, st_alarm_info.strCode);
			m_nSortPlace_Step = 910;
		}
		break;

	case 920:
		m_dCurrentPos[m_nRobot_Ejector] = COMI.Get_MotCurrentPos(m_nRobot_Ejector);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[m_nRobot_Ejector] > st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[m_nRobot_Ejector].d_allow) //안전 위치를 벗어나 있으면 저위치 시킨다
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
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_nSortPlace_Step = 940;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nSortPlace_Step = 930;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(2620, dWARNING, st_alarm_info.strCode);
			m_nSortPlace_Step = 930;
		}
		break;

	case 940:
		m_bDvcWaitChk_Falg = false;
		m_nSortPlace_Step = 1000;
		break; 

	///////////////////////////////////////////////////////////////////////
	// 작업할 조건을 찾고, 셋팅
	///////////////////////////////////////////////////////////////////////
	case 1000:
		m_dCurrentPos[m_nRobot_Z] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[m_nRobot_Z] > st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벗어나 있으면 저위치 시킨다
		{
			m_nSortPlace_Step = 900;
			break;
		}
		m_dCurrentPos[m_nRobot_Ejector] = COMI.Get_MotCurrentPos(m_nRobot_Ejector);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[m_nRobot_Ejector] > st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벗어나 있으면 저위치 시킨다
		{
			m_nSortPlace_Step = 900;
			break;
		}

		nRet_1 = clsFunc.Check_PickerStatus(0, THD_TEST_RBT, CTL_YES, m_npPicker_YesNo, m_npPicker_Vacuum_Status, m_npPicker_OutputStatus); //nMode == 0 이면 전역변수, nMode == 1이면 멤버변수의 정보에 따라 피커 상태를 확인한다,
		if(nRet_1 == RET_GOOD) //전역 변수에 따른 피커 상태를 체크 
		{//피커 정보와 현재 상태가 정상이다
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

		//작업 가능 위치를 찾는다		
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
					m_nSort_Count++; //해당빈으로 소팅 할 수량 
					i = m_nPickerPara;

					nFlag = CTL_YES;
				}
			}
		}

		if(m_nFirstPicker_Num == -1)
		{
			//피커에 더이상 작업할 것이 없다
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
	//작업 1구간 : 작수열 버퍼 위치의 정보를 찾는다 	//0,2,4,6,8,10,12,14(TESTSITE_RIGHT) . 또는 14,12,10,8,6,4,2,0(TESTSITE_LEFT)의 자재이면 같은 숫자의 반대의 순서로 우선 찾는다	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	case 1100:  
		nPicker_Cnt = 0;
		nBuffer_Cnt = 0;
		nFlag = CTL_NO;
		nRet_1 = CTL_NO;
		nBuffFirstNo= 0;

		if(nStart_FixPos == TESTSITE_LEFT) //test left site를 wkwofmf 작업하기 위한 작업 중일때, 리테스트 버퍼는 항상 같은 기본 방향(Right Test Site 방향)을 유지한다, LEFT SITE에서 오면 버퍼를 회전하여 1번 핀 방향을 맞춘다 
		{			
			nBuffFirstNo = st_recipe_info.nRetestBuffer_Num - 1;  //nRetestBuffer_Num == 15 // 14,12,10,8,6,4,2,0 사용 
			//for(nBuff_x = 1; nBuff_x < st_recipe_info.nRetestBuffer_Num; (nBuff_x = nBuff_x + 2)) //nRetestBuffer_Num == 15 //0,2,4,6,8,10,12,14 을 우선 찾는다
			for(nBuff_x = nBuffFirstNo; nBuff_x >= 0; (nBuff_x = nBuff_x - 2))//nRetestBuffer_Num == 15 //14,12,10,8,6,4,2,0 의 8개 위치을 우선 찾는다
			{	//버퍼 위치르 기준으로 하나씩 찾아 간다. 		 
				m_nFirst_Buff_Num = -1;
				nPicker_Cnt = 0; nBuffer_Cnt = 0; //clear 
				nFlag = CTL_NO; 
				if(st_buffer_info[nWork_Site].st_pcb_info[nBuff_x].nYesNo == CTL_NO) //버퍼에서 시작가능한 위치를 찾는다 
				{				 
					for(nPicker_x = 0; nPicker_x < TEST_PICKER_PARA; nPicker_x++) //TEST_PICKER_PARA == 4 //for(nPicker_x = m_nFirstPicker_Num; nPicker_x < TEST_PICKER_PARA; nPicker_x++) //TEST_PICKER_PARA == 4
					{
						// jtkim 20161004 check
						m_npPicker_YesNo[nPicker_x] = CTL_NO; //작업할 피커정보 clear하기 0부터 for문으로 초기화 위해 
						if(nPicker_x < m_nFirstPicker_Num) continue;

						if(st_picker[m_nRobotSite].st_pcb_info[nPicker_x].nYesNo == CTL_YES && st_picker[m_nRobotSite].st_pcb_info[nPicker_x].nBdNum == nSortBin)//피커에 자재가 잇고, 소팅빈과 일치하면 
						{
							m_npPicker_YesNo[nPicker_x] = CTL_YES; 

							nPicker_Cnt++;
					 
							if(st_buffer_info[nWork_Site].st_pcb_info[ nBuff_x - ((nPicker_x - m_nFirstPicker_Num) * 2)].nYesNo == CTL_NO) //버퍼의 정보를 한칸건너 하나씩 체크, 버퍼 사이즈와 피커사이즈가 1/2 로 1:1로 일치하지않아 조건 필요.
							{
								//피커와 버퍼의 조건이 같다. 동작 할 수 있다 
								if(nFlag == CTL_NO)
								{
									m_nFirst_Buff_Num = nBuff_x;
									nFlag = CTL_YES;
								}
								nBuffer_Cnt++;
							}
							else
							{
								//피커와 버퍼의 조건이 다르다. 이 버퍼의 시작  위치에서는 충돌하여 동작할 수 없다.
								nPicker_x = TEST_PICKER_PARA;
								break;
							}
						}
					} 

					if(nPicker_Cnt == nBuffer_Cnt) //자재를 버퍼에 한번에 놓을 수 있는 위치가 있다 //정상 OK
					{
						nBuff_x = st_recipe_info.nRetestBuffer_Num;				 
						nRet_1 = CTL_YES;
					}
				}
			}
		}
		else if(nStart_FixPos == TESTSITE_RIGHT)
		{

			for(nBuff_x = 0; nBuff_x < st_recipe_info.nRetestBuffer_Num; (nBuff_x = nBuff_x + 2)) //nRetestBuffer_Num == 15 //0,2,4,6,8,10,12,14 을 우선 찾는다
			{			 
				m_nFirst_Buff_Num = -1;
				nPicker_Cnt = 0; nBuffer_Cnt = 0; //clear 
				nFlag = CTL_NO; 
				if(st_buffer_info[nWork_Site].st_pcb_info[nBuff_x].nYesNo == CTL_NO) //버퍼에서 시작가능한 위치를 찾는다 
				{				 
					for(nPicker_x = 0; nPicker_x < TEST_PICKER_PARA; nPicker_x++) //TEST_PICKER_PARA == 4 //for(nPicker_x = m_nFirstPicker_Num; nPicker_x < TEST_PICKER_PARA; nPicker_x++) //TEST_PICKER_PARA == 4
					{
						m_npPicker_YesNo[nPicker_x] = CTL_NO; //작업할 피커정보 clear하기 위해 
						if(nPicker_x < m_nFirstPicker_Num) continue;

						if(st_picker[m_nRobotSite].st_pcb_info[nPicker_x].nYesNo == CTL_YES && st_picker[m_nRobotSite].st_pcb_info[nPicker_x].nBdNum == nSortBin)// 
						{
							m_npPicker_YesNo[nPicker_x] = CTL_YES; 
							nPicker_Cnt++;
					 
							if(st_buffer_info[nWork_Site].st_pcb_info[ nBuff_x + ((nPicker_x - m_nFirstPicker_Num) * 2)].nYesNo == CTL_NO)
							{
								//피커와 버퍼의 조건이 같다. 동작 할 수 있다 
								if(nFlag == CTL_NO)
								{
									m_nFirst_Buff_Num = nBuff_x;
									nFlag = CTL_YES;
								}
								nBuffer_Cnt++;
							}
							else
							{
								//피커와 버퍼의 조건이 다르다. 이 버퍼의 시작  위치에서는 충돌하여 동작할 수 없다.
								m_nFirst_Buff_Num = -1;
								nPicker_x = TEST_PICKER_PARA;
								break;
							}
						}
					} 

					if(nPicker_Cnt == nBuffer_Cnt) //자재를 버퍼에 한번에 놓을 수 있는 위치가 있다 //정상 OK
					{
						nBuff_x = 15;				 
						nRet_1 = CTL_YES;
					}
				}
			}

			if(nRet_1 == CTL_YES) //한번에 놓을 위치 획득 
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
	//작업 2구간 :홀수열 버퍼 정보를 찾는다 	//1,3,4,5,7,9,11,13 을 2번쨰로 찾는다	
	///////////////////////////////////////////////
	case 1120:
		nPicker_Cnt = 0;
		nBuffer_Cnt = 0;
		nFlag = CTL_NO;
		nRet_1 = CTL_NO;
		nBuffFirstNo= 0;

		if(nStart_FixPos == TESTSITE_LEFT) //left site룰 작업하기 위한 작업 중일때
		{			
			nBuffFirstNo = st_recipe_info.nRetestBuffer_Num - 2; //13,11,9,7,5,3,1 위치 
			//for(nBuff_x = 1; nBuff_x < st_recipe_info.nRetestBuffer_Num; (nBuff_x = nBuff_x + 2)) //nRetestBuffer_Num == 15 //0,2,4,6,8,10,12,14 을 우선 찾는다
			for(nBuff_x = nBuffFirstNo; nBuff_x >= 0; (nBuff_x = nBuff_x - 2))//nRetestBuffer_Num == 15 //14,12,10,8,6,4,2,0 을 우선 찾는다
			{			 
				m_nFirst_Buff_Num = -1;
				nPicker_Cnt = 0; nBuffer_Cnt = 0; //clear 
				nFlag = CTL_NO; 
				if(st_buffer_info[nWork_Site].st_pcb_info[nBuff_x].nYesNo == CTL_NO) //버퍼에서 시작가능한 위치를 찾는다 
				{				 
					for(nPicker_x = 0; nPicker_x < TEST_PICKER_PARA; nPicker_x++) //TEST_PICKER_PARA == 4 //for(nPicker_x = m_nFirstPicker_Num; nPicker_x < TEST_PICKER_PARA; nPicker_x++) //TEST_PICKER_PARA == 4
					{
						m_npPicker_YesNo[nPicker_x] = CTL_NO; //작업할 피커정보 clear하기 위해 
						if(nPicker_x < m_nFirstPicker_Num) continue;

						if(st_picker[m_nRobotSite].st_pcb_info[nPicker_x].nYesNo == CTL_YES && st_picker[m_nRobotSite].st_pcb_info[nPicker_x].nBdNum == nSortBin)// 
						{
							m_npPicker_YesNo[nPicker_x] = CTL_YES; 
							nPicker_Cnt++;
					 
							if(st_buffer_info[nWork_Site].st_pcb_info[ nBuff_x - ((nPicker_x - m_nFirstPicker_Num) * 2)].nYesNo == CTL_NO)
							{
								//피커와 버퍼의 조건이 같다. 동작 할 수 있다 
								if(nFlag == CTL_NO)
								{
									m_nFirst_Buff_Num = nBuff_x;
									nFlag = CTL_YES;
								}
								nBuffer_Cnt++;
							}
							else
							{
								//피커와 버퍼의 조건이 다르다. 이 버퍼의 시작  위치에서는 충돌하여 동작할 수 없다.
								nPicker_x = TEST_PICKER_PARA;
								break;
							}
						}
					} 

					if(nPicker_Cnt == nBuffer_Cnt) //자재를 버퍼에 한번에 놓을 수 있는 위치가 있다 //정상 OK
					{
						nBuff_x = st_recipe_info.nRetestBuffer_Num;				 
						nRet_1 = CTL_YES;
					}
				}
			}
		}
		else if(nStart_FixPos == TESTSITE_RIGHT)
		{
			for(nBuff_x = 1; nBuff_x < st_recipe_info.nRetestBuffer_Num; (nBuff_x = nBuff_x + 2)) //nRetestBuffer_Num == 15 //1,3,5,7,9,11,13 을 우선 찾는다
			{			 
				m_nFirst_Buff_Num = -1;
				nPicker_Cnt = 0; nBuffer_Cnt = 0; //clear 
				nFlag = CTL_NO; 
				if(st_buffer_info[nWork_Site].st_pcb_info[nBuff_x].nYesNo == CTL_NO) //버퍼에서 시작가능한 위치를 찾는다 
				{				 
					for(nPicker_x = 0; nPicker_x < TEST_PICKER_PARA; nPicker_x++) //TEST_PICKER_PARA == 4 //for(nPicker_x = m_nFirstPicker_Num; nPicker_x < TEST_PICKER_PARA; nPicker_x++) //TEST_PICKER_PARA == 4
					{
						m_npPicker_YesNo[nPicker_x] = CTL_NO; //작업할 피커정보 clear하기 위해 
						if(nPicker_x < m_nFirstPicker_Num) continue;

						if(st_picker[m_nRobotSite].st_pcb_info[nPicker_x].nYesNo == CTL_YES && st_picker[m_nRobotSite].st_pcb_info[nPicker_x].nBdNum == nSortBin)// 
						{
							m_npPicker_YesNo[nPicker_x] = CTL_YES; 

							nPicker_Cnt++;
					 
							if(st_buffer_info[nWork_Site].st_pcb_info[ nBuff_x + ((nPicker_x - m_nFirstPicker_Num) * 2)].nYesNo == CTL_NO)
							{
								//피커와 버퍼의 조건이 같다. 동작 할 수 있다 
								if(nFlag == CTL_NO)
								{
									m_nFirst_Buff_Num = nBuff_x;
									nFlag = CTL_YES;
								}
								nBuffer_Cnt++;
							}
							else
							{
								//피커와 버퍼의 조건이 다르다. 이 버퍼의 시작  위치에서는 충돌하여 동작할 수 없다.
								nPicker_x = TEST_PICKER_PARA;
								break;
							}
						}
					} 

					if(nPicker_Cnt == nBuffer_Cnt) //자재를 버퍼에 한번에 놓을 수 있는 위치가 있다, //정상 OK
					{
						nBuff_x = st_recipe_info.nRetestBuffer_Num;				 
						nRet_1 = CTL_YES;
					}
				}
			}
		}


		if(nRet_1 == CTL_YES) //한번에 놓을 위치 획득 
		{
			m_nSortPlace_Step = 1150; 
		}
		else 
		{
			m_nFirst_Buff_Num = -1;
			//error 
			//이곳이 나오면 안됨 
		}
		break;

	///////////////////////////////////////////////////////////////////
	// 피커와 버퍼 정보를 구했으니 동작할 피커를 정의하여 처리하자 
	///////////////////////////////////////////////////////////////////
	case 1150:		//실제 작업 위치를 셋팅
		m_npFindWorkPosYXCPB[0] = m_nFirst_Buff_Num; m_npFindWorkPosYXCPB[1] = m_nFirst_Buff_Num; m_npFindWorkPosYXCPB[3] = m_nFirstPicker_Num;

		m_nMove_Flag[0] = CTL_NO; m_nMove_Flag[1] = CTL_NO;
		m_nSortPlace_Step = 2000;
		 	
		break;

	case 2000:
		m_dCurrentPos[2] = COMI.Get_MotCurrentPos(m_nRobot_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[2] > st_motor_info[m_nRobot_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벅어나 있으면 저위치 시킨다
		{
			m_nSortPlace_Step = 900;
			break;
		}
		m_dCurrentPos[m_nRobot_Ejector] = COMI.Get_MotCurrentPos(m_nRobot_Ejector);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[m_nRobot_Ejector] > st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벗어나 있으면 저위치 시킨다
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
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_bDvcWaitChk_Falg =  false;
			m_nSortPlace_Step = 2100;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nSortPlace_Step = 2000;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(2650, dWARNING, st_alarm_info.strCode);
			m_nSortPlace_Step = 2000;
		}
		break; 	

	case 2100:
		if(nWork_Site == THD_LD_BUFF || nWork_Site == THD_ULD_BUFF)
		{
			//로딩 버퍼에 요청하여 자재가 준비되어 CTL_READY 된 상태, 자재를 집을 수 있다 
			 if(st_sync_info.nTestRbt_Dvc_Req[nWork_Site][0] == CTL_READY && st_sync_info.nTestRbt_Dvc_Req[nWork_Site][1] == WORK_PLACE) //로딩 버퍼가 준비되기를 기다린다 
			 {				 //james 2016.1005 = CTL_READY
				 m_nSortPlace_Step = 3000;
			 }		
		}
		else  //if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF || nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8)
		{//집을 일이 없음 
			 m_nSortPlace_Step = 3000;
		}		
		break;

	case 3000:  
		m_dCurrentPos[m_nRobot_Ejector] = COMI.Get_MotCurrentPos(m_nRobot_Ejector);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[m_nRobot_Ejector] > st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[m_nRobot_Z].d_allow) //안전 위치를 벗어나 있으면 저위치 시킨다
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

		Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON); //미리 벌린다  

		if(nWork_Site == THD_LD_BUFF)
		{
			m_dpTargetPosList[2] = st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_LDBUF_PLACE];
		}
		else if(nWork_Site == THD_ULD_BUFF)
		{//집을 일이 없음 
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
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{ 			
			m_nSortPlace_Step = 3100;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nSortPlace_Step = 3000;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(2660, dWARNING, st_alarm_info.strCode);
			m_nSortPlace_Step = 3000;
		}		
		break;	

	case 3100: //자재를 놓는다 
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
		if(nRet_1 == RET_GOOD) //전역 변수에 따른 피커 상태를 체크 
		{//피커 정보와 현재 상태가 정상이다
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
		Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON);  //자재를 놓는다 
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
			Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON);  //에러니 벌린다 
			CTL_Lib.Alarm_Error_Occurrence(2680, dWARNING, m_strAlarmCode);
			m_nPick_Step = 4000;
		}
		break;

	case 4100:
		 
			m_nSortPlace_Step = 4200; 
		break;

	case 4200:
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_bDvcWaitChk_Falg = false;
			m_nSortPlace_Step = 5000;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nSortPlace_Step = 4200;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
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
						if(nStart_FixPos == TESTSITE_LEFT) //left site룰 작업하기 위한 작업 중일때
						{
							nFind_JobPos = m_nFirst_Buff_Num - ((i - m_nFirstPicker_Num) * 2);  //13,11,9,7,5,3,1 위치 

							//아래 함수에서 사용위해 위치 정보를 셋팅한다
							m_npSet_WorkPosYXCPB[0]  = nFind_JobPos; //버퍼sms Y만 사용 //버퍼의 위치 지정
							m_npSet_WorkPosYXCPB[1]  = nFind_JobPos; //qjvjsms X는 사용하지 않음 //버퍼의 위치 지정
							m_npSet_WorkPosYXCPB[3]  = i; //picker 위치 정보

							clsFunc.Data_Exchange_PickPlace(PICKER_PLACE_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //피커 및 트레이 정보 셋팅 함수
							//위 함수에서 모든 정보를 셋팅 및 클리어 한다
							//st_picker[m_nRobotSite].st_pcb_info[y].nYesNo = CTL_YES;					//피커 정보 셋
							//st_tray_info[nWork_Site].st_pcb_info[m_npFirstPosYXP[0] + (y - m_n_FirstPicker_Num)][m_npFirstPosYXP[1]].nYesNo = CTL_NO; //트레이 정보 클리어
							//st_count_info.nSSD_Input[0]++;
						}
						else if(nStart_FixPos == TESTSITE_RIGHT) //RIGHT TEST SITE 작업일때, 로딩버퍼에서 피커정보눈 0,1,2,3 이고, 버퍼 정보정보는0,1,2,3 로 위치 순서가 동일하다 
						{
							nFind_JobPos = m_nFirst_Buff_Num + ((nPicker_x - m_nFirstPicker_Num) * 2);

							//아래 함수에서 사용위해 위치 정보를 셋팅한다
							m_npSet_WorkPosYXCPB[0]  = nFind_JobPos; //버퍼sms Y만 사용 //버퍼의 위치 지정
							m_npSet_WorkPosYXCPB[1]  = nFind_JobPos; //qjvjsms X는 사용하지 않음 //버퍼의 위치 지정
							m_npSet_WorkPosYXCPB[3]  = i; //picker 위치 정보

							clsFunc.Data_Exchange_PickPlace(PICKER_PLACE_MODE, 1, m_nRobotSite, nWork_Site, m_npSet_WorkPosYXCPB); //피커 및 트레이 정보 셋팅 함수 
						}
					}
				} 
			}
		} 

		if (nRet_1 == RET_GOOD) // 1000 step에서 셋팅한 피커 정보에 맞게 모두 정상적 집었을 경우 이
		{
			//화면 정보 갱신

			m_nDvcRepickCount = 0;
			m_nPartialPickCount = 0;

			m_bDvcWaitChk_Falg = false;

			// jtkim 20150216 conveyor/loader tray 화면 표시
			if (st_handler_info.cWndMain != NULL)
			{
				st_handler_info.cWndMain->PostMessage(WM_WORK_DISPLAY, MAIN_LD_PICKER_DISPLAY, 0);
				st_handler_info.cWndMain->PostMessage(WM_WORK_DISPLAY, MAIN_LD_TRAY_DISPLAY, 0);
				st_handler_info.cWndMain->PostMessage(WM_WORK_DISPLAY, MAIN_RETEST_BUF_DISPLAY, 0);
			}
			m_nSortPlace_Step = 6000;
		}
		else if (nRet_1 == RET_ERROR) //셋팅한 피커중 한개라도 제대로 집지 못했으면 이곳으로 이동
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
{//james 2016.0827 그리퍼 디바이스 체크 정보 수정/보완, Gripper Off(클림프상태)에서 IO_OFF시 디바이스 있는상태, 그외는 ON/OFF 산관없이 모두 디바이스 없는 상태

	
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

	///* james 이 함수는 잘못되었다. 나중에 다시해야 함 
	

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