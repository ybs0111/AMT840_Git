// CtlBd_Function.cpp: implementation of the CCtlBd_Function class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "handler.h"
#include "CtlBd_Function.h"
#include "Cmmsdk.h"
#include "CmmsdkDef.h"
#include "CtlBd_Variable.h"
#include "ComizoaPublic.h"
#include "FastechPublic_IO.h"
//#include "FAS_HSSI.h"
#include "math.h"
#include "PublicFunction.h"
#include "Variable.h"
#include "AlgMemory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCtlBd_Function  CtlBdFunc;
CCtlBd_Function::CCtlBd_Function()
{
}

CCtlBd_Function::~CCtlBd_Function()
{
//st_ctlbd.n_TotalMotBoard_Number
}

int CCtlBd_Function::Board_Initialize(int n_mode)
{
	int nRet = 0;

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	//Motor Board Initialize 
	///////////////////////////////////////////////////////////////////////////////////////////////////////
/*	if(CTLBD_MOTBD_TYPE == 0) //0:파스텍 보드를 사용하는 장비이면, 1:커미조아 모터를 사용하는 장비 
	{
//		st_ctlbd.n_TotalMotBoard_Number = 3;  //모터보드가 3개 있다 
//		st_ctlbd.n_TotalMotorAxis_Number= 18; //모터의 총 수량은 18개이다  
//		st_ctlbd.n_MotorAxis_Number[0] = 8;   //0번 모터보드는 8축용 보드이다  
//		st_ctlbd.n_MotorAxis_Number[1] = 6;   //1번 모터보드는 6축용 보드이다 
//		st_ctlbd.n_MotorAxis_Number[2] = 4;   //2번 모터보드는 4축용 보드이다 
	}
	else if(CTLBD_MOTBD_TYPE == 1) //1:커미조아 보드를 사용하는 장비이면, 1:커미조아 모터를 사용하는 장비 
	{


	}*/

	return  RET_GOOD;
}

int CCtlBd_Function::Robot_Clash_Safety_Check(int n_Mode, int n_SourceAxis, int n_ReferenceAxis, double d_Safety_Gap, double d_TargetPos)
{//d_Safety_Gap = 300; //300mm 이상 떨어져 있어야 한다 //DC Loading 사이트 

	//n_Mode => 0: 모터 이동전 순수하게 체크만 하는 상태, 1: 모터가 이동중일때 체크하는 상태 
	int nRet = RET_ERROR;

	return nRet;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Inposition 생략 및 거리별 속도를 관리한다 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int CCtlBd_Function::MotorSpeed_Control(int n_Axis, double d_CurrentPos, double d_TargetPos, double * dp_Speed)
{
	double dCurrentPos = 0, dTargetPos = 0, dVal =0, d_abs_val = 0, dAccDecTime=0; 

// 	if(n_Axis == M_DCLDRBT_Z || n_Axis == M_DCULDRBT_Z || n_Axis == M_ULDRBT_Z)
// 	{
// 		dCurrentPos = COMI.Get_MotCurrentPos(n_Axis);		
// 
// 		//목표위치 계산 방법은 ==> 현재위치 - 목표이동 위치 이다 
// 		dVal = dCurrentPos - d_TargetPos;
// 		if(st_motor[n_Axis].d_pos[0] >= d_TargetPos) //P_DCLDRBT_Z_SAFETY == 0 ,목표 지점이 안전위치와 같으면 정해진 속도로 이동 
// 		{
// 			dp_Speed[0] = st_motor[n_Axis].d_spd_vel[0]; dp_Speed[1] = st_motor[n_Axis].d_spd_vel[1]; dp_Speed[2] = st_motor[n_Axis].d_spd_vel[2]; //가속 펄수 값은 9,000,000 이다
// 			//UP할때는 Inposition를 보자  말자 
// 			COMI.Set_Motor_IO_Property(n_Axis, cmINP_EN, cmFALSE); //cmINP_EN=9 //cmFALSE = 0 INP 비활성, cmTRUE = 1 INP 활성 
// 		}
// 		else if(dVal > 0) //먼 위치에서 가까운 위치(홈 위치) 로 이동했을때는 + 값 발생 , 모터가 UP 할 때 발생 
// 		{
// 			//원래 셋팅되어있는 값 사용 
// 			//650,000 13,000,000 13,000,000   ==> 16mm 이동하는데 65ms 소요 
// 			//300,000  9,000000   9,000000    ==> 16mm 이동하는데 80ms 소요 
// 			dp_Speed[0] = st_motor[n_Axis].d_spd_vel[0]; dp_Speed[1] = st_motor[n_Axis].d_spd_vel[1]; dp_Speed[2] = st_motor[n_Axis].d_spd_vel[2]; //가속 펄수 값은 9,000,000 이다
// 
// 			//UP할때 또는 정확한 목표지점으로 가지 않아도 되는 조건이면 Inposition를 보지 말자 
// 			COMI.Set_Motor_IO_Property(n_Axis, cmINP_EN, cmFALSE); //cmINP_EN=9 //cmFALSE = 0 INP 비활성, cmTRUE = 1 INP 활성 
// 		} //12mm 이하로 내려갈 때 
// 		else
// 		{/*
// 			if(n_Axis == M_DCLDRBT_Z || n_Axis == M_DCULDRBT_Z )
// 			{
// 				if(dVal > -12) //-1 ~ -19 DC로디로봇이 DC 소켓에 놓을때 - 값 발생  //홈 위치에서 증가 방향으로 이동하려면 - 값이 발생한다 
// 				{//dAccPPS = (d_work - st_motor[n_Axis].d_initial_speed) / (d_accel_time / 1000);
// 					//dAccDecTime = (d_work - st_motor[n_Axis].d_initial_speed) / (dAccPPS / 1000);
// 					dAccDecTime = (300000 - st_motor[n_Axis].d_initial_speed) / (5000000 / 1000); //60ms		
// 					dp_Speed[0] = 300000; dp_Speed[1] = dAccDecTime; dp_Speed[2] = dAccDecTime; //가속 펄수 값은 9,000,000 이다 
// 					
// 					COMI.Set_Motor_IO_Property(n_Axis, cmINP_EN, cmTRUE); //cmINP_EN=9 //cmFALSE = 0 INP 비활성, cmTRUE = 1 INP 활성 
// 				} 
// 				//20mm 이하로 내려갈 때 
// 				else if(dVal > -38) //-1 ~ -19 DC로디로봇이 DC 소켓에 놓을때 - 값 발생  //홈 위치에서 증가 방향으로 이동하려면 - 값이 발생한다 
// 				{//dAccPPS = (d_work - st_motor[n_Axis].d_initial_speed) / (d_accel_time / 1000);
// 				 //dAccDecTime = (d_work - st_motor[n_Axis].d_initial_speed) / (dAccPPS / 1000);
// 					dAccDecTime = (300000 - st_motor[n_Axis].d_initial_speed) / (9000000 / 1000); //dAccDecTime = 33 ms			
// 					dp_Speed[0] = 300000; dp_Speed[1] = dAccDecTime; dp_Speed[2] = dAccDecTime; //가속 펄수 값은 9,000,000 이다 
// 
// 					COMI.Set_Motor_IO_Property(n_Axis, cmINP_EN, cmTRUE); //cmINP_EN=9 //cmFALSE = 0 INP 비활성, cmTRUE = 1 INP 활성 
// 				} //20mm이상 내려갈때 
// 				else //if(dVal < -38)  //-20 이하 로딩로봇이 트레이에서 집을때, 언로딩 로봇이 인서트 버퍼 bottom에 놓을때 발생 
// 				{
// 					//dAccDecTime = (650000 - st_motor[n_Axis].d_initial_speed) / (12000000 / 1000); //dAccDecTime = 300 ms			
// 					//dp_Speed[0] = 650000; dp_Speed[1] = dAccDecTime; dp_Speed[2] = dAccDecTime; //가속 펄수 값은 9,000,000 이다 			
// 					dAccDecTime = (st_motor[n_Axis].d_spd_vel[0] - st_motor[n_Axis].d_initial_speed) / (12000000 / 1000); //dAccDecTime = 300 ms			
// 					dp_Speed[0] = st_motor[n_Axis].d_spd_vel[0]; dp_Speed[1] = dAccDecTime; dp_Speed[2] = dAccDecTime; //가속 펄수 값은 9,000,000 이다 		
// 					
// 					COMI.Set_Motor_IO_Property(n_Axis, cmINP_EN, cmTRUE); //cmINP_EN=9 //cmFALSE = 0 INP 비활성, cmTRUE = 1 INP 활성 			
// 				}
// 			}
// */
// 			if(n_Axis == M_DCLDRBT_Z)
// 			{
// 				if(d_TargetPos == st_motor[M_DCLDRBT_Z].d_pos[P_DCLDRBT_Z_LDPLATE_PICK])
// 				{
// 					//091012 dp_Speed[0] = 650000; dp_Speed[1] = 50; dp_Speed[2] = 100; //  
// 					dp_Speed[0] = 450000; dp_Speed[1] = 100; dp_Speed[2] = 50; //  
// 				}
// 				else if(d_TargetPos == st_motor[M_DCLDRBT_Z].d_pos[P_DCLDRBT_Z_DCBUFF_PICK])
// 				{
// 					dp_Speed[0] = 650000; dp_Speed[1] = 50; dp_Speed[2] = 100; //  
// 				}
// 				else if(d_TargetPos == st_motor[M_DCLDRBT_Z].d_pos[P_DCLDRBT_Z_DCTEST_PICK])
// 				{
// 					dp_Speed[0] = 450000; dp_Speed[1] = 50; dp_Speed[2] = 50; //  
// 				}
// 				else if(d_TargetPos == st_motor[M_DCLDRBT_Z].d_pos[P_DCLDRBT_Z_LDPLATE_PLACE])
// 				{
// 					dp_Speed[0] = 650000; dp_Speed[1] = 50; dp_Speed[2] = 100; //  
// 				}
// 				else if(d_TargetPos == st_motor[M_DCLDRBT_Z].d_pos[P_DCLDRBT_Z_LDBUFF_PLACE])
// 				{
// 					dp_Speed[0] = 650000; dp_Speed[1] = 50; dp_Speed[2] = 100; //  
// 				}
// 				else if(d_TargetPos == st_motor[M_DCLDRBT_Z].d_pos[P_DCLDRBT_Z_DCTEST_PLACE])
// 				{
// 					//dp_Speed[0] = 450000; dp_Speed[1] = 50; dp_Speed[2] = 50; //  
// 					//091012 dp_Speed[0] = 380000; dp_Speed[1] = 50; dp_Speed[2] = 50; //
// 					dp_Speed[0] = 380000; dp_Speed[1] = 50; dp_Speed[2] = 50; //  
// 				}
// 				else
// 				{
// 					dp_Speed[0] = st_motor[n_Axis].d_spd_vel[0]; dp_Speed[1] = st_motor[n_Axis].d_spd_vel[1]; dp_Speed[2] = st_motor[n_Axis].d_spd_vel[2]; //가속 펄수 값은 9,000,000 이다
// 				}
// 				COMI.Set_Motor_IO_Property(n_Axis, cmINP_EN, cmTRUE); //cmINP_EN=9 //cmFALSE = 0 INP 비활성, cmTRUE = 1 INP 활성 
// 			}
// 			else if(n_Axis == M_DCULDRBT_Z)
// 			{
// 				if(d_TargetPos == st_motor[M_DCULDRBT_Z].d_pos[P_DCULDRBT_Z_DCTEST_PICK])
// 				{
// 					dp_Speed[0] = 550000; dp_Speed[1] = 50; dp_Speed[2] = 100; //  
// 				}
// 				else if(d_TargetPos == st_motor[M_DCULDRBT_Z].d_pos[P_DCULDRBT_Z_DCBUFF_PICK])
// 				{
// 					dp_Speed[0] = 400000; dp_Speed[1] = 50; dp_Speed[2] = 50; //  
// 				}
// 				else if(d_TargetPos == st_motor[M_DCULDRBT_Z].d_pos[P_DCULDRBT_Z_INSERTBUFF_TOP_PICK])
// 				{
// 					dp_Speed[0] = 400000; dp_Speed[1] = 50; dp_Speed[2] = 50; //  
// 				}
// 				else if(d_TargetPos == st_motor[M_DCULDRBT_Z].d_pos[P_DCULDRBT_Z_INSERTBUFF_BOTTOM_PICK])
// 				{
// 					dp_Speed[0] = 550000; dp_Speed[1] = 50; dp_Speed[2] = 100; //  
// 				}
// 				else if(d_TargetPos == st_motor[M_DCULDRBT_Z].d_pos[P_DCULDRBT_Z_DCTEST_PLACE])
// 				{
// 					dp_Speed[0] = 550000; dp_Speed[1] = 50; dp_Speed[2] = 100; //  
// 				}
// 				else if(d_TargetPos == st_motor[M_DCULDRBT_Z].d_pos[P_DCULDRBT_Z_DCBUFF_PLACE])
// 				{
// 					dp_Speed[0] = 400000; dp_Speed[1] = 50; dp_Speed[2] = 50; //  
// 				}
// 				else if(d_TargetPos == st_motor[M_DCULDRBT_Z].d_pos[P_DCULDRBT_Z_INSERTBUFF_TOP_PLACE])
// 				{
// 					//dp_Speed[0] = 400000; dp_Speed[1] = 50; dp_Speed[2] = 50; //  
// 					dp_Speed[0] = 400000; dp_Speed[1] = 50; dp_Speed[2] = 100; //  
// 				}
// 				else if(d_TargetPos == st_motor[M_DCULDRBT_Z].d_pos[P_DCULDRBT_Z_INSERTBUFF_BOTTOM_PLACE])
// 				{
// 					//dp_Speed[0] = 550000; dp_Speed[1] = 50; dp_Speed[2] = 100; //  
// 					dp_Speed[0] = 550000; dp_Speed[1] = 50; dp_Speed[2] = 100; //  
// 				}
// 				else
// 				{
// 					dp_Speed[0] = st_motor[n_Axis].d_spd_vel[0]; dp_Speed[1] = st_motor[n_Axis].d_spd_vel[1]; dp_Speed[2] = st_motor[n_Axis].d_spd_vel[2]; //가속 펄수 값은 9,000,000 이다
// 				}
// 				COMI.Set_Motor_IO_Property(n_Axis, cmINP_EN, cmTRUE); //cmINP_EN=9 //cmFALSE = 0 INP 비활성, cmTRUE = 1 INP 활성 
// 			}
// 			else// if(n_Axis == M_ULDRBT_Z)
// 			{//090909 
// 				if(d_TargetPos == st_motor[M_ULDRBT_Z].d_pos[P_ULDRBT_Z_REMOVEBUFF_TOP_PICK])
// 				{
// 					dp_Speed[0] = 400000; dp_Speed[1] = 100; dp_Speed[2] = 150; //  
// 				}
// 				else if(d_TargetPos == st_motor[M_ULDRBT_Z].d_pos[P_ULDRBT_Z_REMOVEBUFF_MIDDLE_PICK])
// 				{
// 					dp_Speed[0] = 500000; dp_Speed[1] = 100; dp_Speed[2] = 150; //  
// 				}
// 				else if(d_TargetPos == st_motor[M_ULDRBT_Z].d_pos[P_ULDRBT_Z_REMOVEBUFF_BOTTOM_PICK])
// 				{
// 					dp_Speed[0] = 400000; dp_Speed[1] = 80; dp_Speed[2] = 150; //  
// 				}
// 				else if(d_TargetPos == st_motor[M_ULDRBT_Z].d_pos[P_ULDRBT_Z_MANUALBUFF_PICK])
// 				{
// 					dp_Speed[0] = 500000; dp_Speed[1] = 100; dp_Speed[2] = 150; //  
// 				}
// 				else if(d_TargetPos == st_motor[M_ULDRBT_Z].d_pos[P_ULDRBT_Z_MANUALBUFF_PLACE])
// 				{
// 					dp_Speed[0] = 400000; dp_Speed[1] = 100; dp_Speed[2] = 150; //  
// 				}
// 				else if(d_TargetPos == st_motor[M_ULDRBT_Z].d_pos[P_ULDRBT_Z_ULDPLATE_PLACE])
// 				{
// 					dp_Speed[0] = 400000; dp_Speed[1] = 80; dp_Speed[2] = 150; //  
// 				}
// 				else
// 				{
// 					dp_Speed[0] = st_motor[n_Axis].d_spd_vel[0]; dp_Speed[1] = st_motor[n_Axis].d_spd_vel[1]; dp_Speed[2] = st_motor[n_Axis].d_spd_vel[2]; //가속 펄수 값은 9,000,000 이다
// 				}
// 				COMI.Set_Motor_IO_Property(n_Axis, cmINP_EN, cmTRUE); //cmINP_EN=9 //cmFALSE = 0 INP 비활성, cmTRUE = 1 INP 활성 			
// 			}
// 		}
// 	}
// 	else if(n_Axis == M_SORTBUFFRBT_Z || n_Axis == M_INSERTRBT_Z || n_Axis == M_REMOVERBT_Z)
// 	{
// 		dCurrentPos = COMI.Get_MotCurrentPos(n_Axis);		
// 
// 		//목표위치 계산 방법은 ==> 현재위치 - 목표이동 위치 이다 
// 		dVal = dCurrentPos - d_TargetPos;
// 		if(dVal > 0) //먼 위치에서 가까운 위치(홈 위치) 로 이동했을때는 + 값 발생 , 모터가 UP 할 때 발생 
// 		{
// 			dp_Speed[0] = st_motor[n_Axis].d_spd_vel[0]; dp_Speed[1] = st_motor[n_Axis].d_spd_vel[1]; dp_Speed[2] = st_motor[n_Axis].d_spd_vel[2]; //가속 펄수 값은 9,000,000 이다
// 			//UP할때 또는 정확한 목표지점으로 가지 않아도 되는 조건이면 Inposition를 보지 말자 
// 			COMI.Set_Motor_IO_Property(n_Axis, cmINP_EN, cmFALSE); //cmINP_EN=9 //cmFALSE = 0 INP 비활성, cmTRUE = 1 INP 활성 
// 		}
// 		else //motor 가 다운 할때 발생 
// 		{
// 			dp_Speed[0] = st_motor[n_Axis].d_spd_vel[0]; dp_Speed[1] = st_motor[n_Axis].d_spd_vel[1]; dp_Speed[2] = st_motor[n_Axis].d_spd_vel[2]; //가속 펄수 값은 9,000,000 이다
// 			COMI.Set_Motor_IO_Property(n_Axis, cmINP_EN, cmTRUE); //cmINP_EN=9 //cmFALSE = 0 INP 비활성, cmTRUE = 1 INP 활성 		
// 		}
// 	}
// 	/*
// 	else if(n_Axis == M_DCLDRBT_X || n_Axis == M_DCULDRBT_X) //아직 사용안함 
// 	{//로보트 X 스크류 모터는 정해진 속도로 게속 이동하여 굳이 필요없음.
// 		dAccDecTime = (st_motor[n_Axis].d_spd_vel[0] - st_motor[n_Axis].d_initial_speed) / (9750000 / 1000); //dAccDecTime = 66.6 ms	//이동구간 시간 : 289ms		
// 		dp_Speed[0] = st_motor[n_Axis].d_spd_vel[0]; dp_Speed[1] = dAccDecTime; dp_Speed[2] = dAccDecTime; //가속 펄수 값은 9,000,000 이다 
// 	}*/
// 	else if(n_Axis == M_DCLDRBT_Y || n_Axis == M_DCULDRBT_Y)// || n_Axis == M_ULDRBT_Y)
// 	{
// 		dCurrentPos = COMI.Get_MotCurrentPos(n_Axis);
// 		d_abs_val = fabs(dCurrentPos - d_TargetPos);
// 		dVal = dCurrentPos - d_TargetPos; //090830 fabs(dCurrentPos - d_TargetPos);
// 		if(d_abs_val < st_size.d_tray_dcld_mm_y + 1) //피치 Y + 2 mm 보다 작으면 정속도를 다운한다 , 진동 문제로 
// 		{
// 			dp_Speed[0] = 200000; dp_Speed[1] = 100; dp_Speed[2] = 100; //가속 펄수 값은 9,000,000 이다
// 		}
// 		else if(n_Axis == M_DCLDRBT_Y && st_motor[M_DCLDRBT_Y].d_pos[P_DCLDRBT_DCSAFETY_POS] == d_TargetPos && dVal < 0) //모터가 증가 방향이면 
// 		{
// 			dp_Speed[0] = 100000; dp_Speed[1] = 100; dp_Speed[2] = 100; //가속 펄수 값은 9,000,000 이다
// 		}
// 		else if(n_Axis == M_DCULDRBT_Y && st_motor[M_DCULDRBT_Y].d_pos[P_DCULDRBT_DCSAFETY_POS] == d_TargetPos && dVal < 0) //모터가 증가 방향이면 
// 		{
// 			dp_Speed[0] = 100000; dp_Speed[1] = 100; dp_Speed[2] = 100; //가속 펄수 값은 9,000,000 이다
// 		}
// 		else
// 		{
// 			dp_Speed[0] = st_motor[n_Axis].d_spd_vel[0]; dp_Speed[1] = st_motor[n_Axis].d_spd_vel[1]; dp_Speed[2] = st_motor[n_Axis].d_spd_vel[2]; 
// 		}
// 	}
// 	else if(n_Axis == M_REMOVERBT_Y) 
// 	{//090907
// 		dCurrentPos = COMI.Get_MotCurrentPos(n_Axis);
// 		d_abs_val = fabs(dCurrentPos - d_TargetPos);
// 		dVal = dCurrentPos - d_TargetPos; //090830 fabs(dCurrentPos - d_TargetPos);
// 		if(n_Axis == M_REMOVERBT_Y && st_motor[M_REMOVERBT_Y].d_pos[P_REMOVERBT_BIBTABLE_SAFETY_POS] == d_TargetPos && dVal < 0) //모터가 증가 방향이면 
// 		{
// 			dp_Speed[0] = 10000; dp_Speed[1] = 100; dp_Speed[2] = 100; //가속 펄수 값은 9,000,000 이다
// 		}		
// 		else
// 		{
// 			dp_Speed[0] = st_motor[n_Axis].d_spd_vel[0]; dp_Speed[1] = st_motor[n_Axis].d_spd_vel[1]; dp_Speed[2] = st_motor[n_Axis].d_spd_vel[2]; 
// 		}
// 	}
// 	else if(n_Axis == M_ULDRBT_Y)//보간 속도 율 
// 	{
// 		dCurrentPos = COMI.Get_MotCurrentPos(n_Axis);
// 		dVal = fabs(dCurrentPos - d_TargetPos);
// 		if(dVal < 50)
// 		{
// 			dp_Speed[0] = 50; dp_Speed[1] = 50; dp_Speed[2] = 50; //speed rate를 50%로 변경 한다
// 		}
// 		else if(dVal < 100) //100mm이하 위치 
// 		{
// 			dp_Speed[0] = 60; dp_Speed[1] = 60; dp_Speed[2] = 60; //speed rate를 50%로 변경 한다
// 		}
// 		else if(dVal < 200) //100mm이하 위치 
// 		{
// 			dp_Speed[0] = 70; dp_Speed[1] = 70; dp_Speed[2] = 70; //speed rate를 50%로 변경 한다
// 		}
// 		else
// 		{
// 			dp_Speed[0] = 100; dp_Speed[1] = 100; dp_Speed[2] = 100; //speed rate를 100%로 변경 한다
// 		}
// 	}
// 	else if(n_Axis == M_ULDRBT_X) //보간 속도 율 
// 	{
// 		dCurrentPos = COMI.Get_MotCurrentPos(n_Axis);
// 		dVal = fabs(dCurrentPos - d_TargetPos);
// 		if(dVal > 400)
// 		{
// 			dp_Speed[0] = 50; dp_Speed[1] = 50; dp_Speed[2] = 50; //speed rate를 50%로 변경 한다
// 		}		
// 		else
// 		{
// 			dp_Speed[0] = 100; dp_Speed[1] = 100; dp_Speed[2] = 100; //speed rate를 100%로 변경 한다
// 		}
// 	}
// 	/*
// 	else if(n_Axis == M_DCLDRBT_X || n_Axis == M_DCULDRBT_X || n_Axis == M_ULDRBT_X)
// 	{
// 		dCurrentPos = COMI.Get_MotCurrentPos(n_Axis);
// 		dVal = fabs(dCurrentPos - d_TargetPos);
// 		if(dVal < st_size.d_tray_dcld_mm_x + 1) //피치 Y + 2 mm 보다 작으면 정속도를 다운한다 , 진동 문제로 
// 		{
// 			dp_Speed[0] = 10000; dp_Speed[1] = 200; dp_Speed[2] = 200; //가속 펄수 값은 9,000,000 이다
// 		}
// 		else
// 		{
// 			dp_Speed[0] = st_motor[n_Axis].d_spd_vel[0]; dp_Speed[1] = st_motor[n_Axis].d_spd_vel[1]; dp_Speed[2] = st_motor[n_Axis].d_spd_vel[2]; 
// 		}
// 	}*/
// 	else
// 	{
// 		dp_Speed[0] = st_motor[n_Axis].d_spd_vel[0]; dp_Speed[1] = st_motor[n_Axis].d_spd_vel[1]; dp_Speed[2] = st_motor[n_Axis].d_spd_vel[2]; 
// 	}
 
	return RET_GOOD;
}


int CCtlBd_Function::Alarm_Error_Occurrence(int n_Count, int n_Type, int n_Status, CString s_JamCode)
{
	CString strMsg;

     st_alarm_info.nCountMode		= n_Count; 
	 st_alarm_info.nTypeMode		= n_Type; 
	 st_alarm_info.strCode			= s_JamCode; 
//	 st_handler_info.nRunStatus		= n_Status; 
	  
//	  MyJamData.On_Alarm_Info_Set_to_Variable(alarm.str_code);
	  
	if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
	{
		if(n_Count == MOT_DEBUG)
		{
			strMsg.Format(_T("[MOT SAFETY CHECK]=> [%s] [%s]"), st_alarm_info.strCode, st_alarm_info.strCurrMsg);
			//wsprintfA(st_other_info.cAbnormalMsg, "%S", strMsg);
			
			if (st_handler_info.cWndList != NULL)
			{
				//clsFunc.OnStringToChar(strMsg, st_other_info.cAbnormalMsg);
			
				clsMem.OnAbNormalMessagWrite(strMsg);
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
			}
		}
		else
		{
			strMsg.Format(_T("[%s] [%s]"), st_alarm_info.strCode, st_alarm_info.strCurrMsg);
			//wsprintfA(st_other_info.cAbnormalMsg, "%S", strMsg);
			//clsFunc.OnStringToChar(strMsg, st_other_info.cAbnormalMsg);

			if (st_handler_info.cWndList != NULL)
			{
				clsMem.OnAbNormalMessagWrite(strMsg);
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
			}
		}
	}
	  
	return RET_GOOD;
}

int CCtlBd_Function::Send_Error_Message(int n_Mode, int n_Axis, CString s_JamCode, CString s_JamMsg)
{
//	long lErrorCode;
//	short int nErrorParseAxis;
//	short int nErrorParseReason;
	char cErrorMsg[200];
	CString sMsg, sTemp;
	
	memset(&cErrorMsg, 0, sizeof(cErrorMsg));  // 데이터 저장 변수 초기화 
/*
	if(n_Mode == MOT_ERR_MSG) 
	{//문자 메세지를 받아 리스트 화면에 디스플레이 한다   
		if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
		{
			st_other.str_abnormal_msg = s_JamMsg;
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
		}		
	}
	else if(n_Mode == MOT_ERR_CODE)
	{//jamcode  설정 		
		if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
		{
			st_other.str_abnormal_msg = s_JamMsg;
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
		}	
	}
*/	
/*	if(CTLBD_DEBUG_MODE) 
	{
		if(CTLBD_MOTBD_TYPE == 0) //파스텍 보드 
		{

		}
		else if(CTLBD_MOTBD_TYPE == 1) //커미조아 보드 
		{
			cmmErrGetLastCode(&lErrorCode);						//마지막 발생한 에러 코드 확인  
			nErrorParseAxis = cmmErrParseAxis(lErrorCode);     //에러를 발생시킨 축 정보를 가져온다 
			nErrorParseReason = cmmErrParseReason(lErrorCode); //error code를 받는다 
			cmmErrGetString(lErrorCode, cErrorMsg, 200);

			sTemp.Format(_T("Msg=%s"), cErrorMsg);
			sMsg.Format(_T("Motor Num:%d, Code=%d, Axis=%d, reason=%d, %s"), n_Axis, lErrorCode, nErrorParseAxis, nErrorParseReason, sTemp);
			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
				wsprintfA(st_other_info.cAbnormalMsg, "%S", sMsg);
				st_handler_info.cWndList->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
				if(MOT_DEBUG) clsFunc.OnLogFileAdd(3, sMsg);
			}		

			sMsg.Format(_T("Index or Axis=%d, %s"), n_Axis, s_JamMsg);
			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
//				st_other.str_abnormal_msg = sMsg;
//				sprintf(st_other.c_abnormal_msg, st_other.str_abnormal_msg);
				wsprintfA(st_other_info.cAbnormalMsg, "%S", sMsg);
				st_handler_info.cWndList->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
				if(MOT_DEBUG) clsFunc.OnLogFileAdd(3, sMsg);
			}	
		}
	} 
	st_alarm_info.strCode = s_JamCode; //alarm.n_count_mode = 0; alarm.n_type_mode = CTL_dWARNING;
*/	
	return YES; 
}

int CCtlBd_Function::OnMotor_SafetyCheck(int n_Mode, int n_Axis, double d_TargetPos)
{/*
	// n_Mode = 0:Home Check, 1:Start, 2:Check, 3:Jog
	CString strMsg;
	int nRet = 0, nRet_1 = 0, nRet_2 = 0;
	double d_CurPos[3] = {0,};
	double d_Pos[3] = {0,};
	double d_GapCheck[3] = {0,};
	int i = 0;
	 
	int n_SetTime = 50; //091127 20->50으로 변경 100; //500ms 동안 계속 감기되면 그때 비로소 I/O 상태 return 

	// **************************************************************************
	// 모터 알람 상태 검사한다                                                   
	// -> 알람 발생한 경우 알람 해제한다                                         
	// **************************************************************************
	//software limit 값 셋팅 체크 에러 
	if(st_handler_info.nRunStatus == dRUN)
	{
		nRet = clsFunc.OnDoorOpenCheck();		
		
		if(nRet == RET_ERROR)
		{		// 문이 열려있고, 안전장치 가동중이면 에러를 리턴한다. 2K6/06/09/ViboX
			return RET_ERROR;
		}
	}


	if (COMI.Get_MotAlarmStatus(n_Axis) == RET_ERROR)           // 모터 ALARM 상태 검사 함수 
	{
		//091216 추가 
		if(st_motor_info[n_Axis].n_retry_time_flag == NO)
		{
			st_motor_info[n_Axis].n_retry_time_flag		= YES;
			st_motor_info[n_Axis].l_retry_time_wait[0]	= GetCurrentTime();
			COMI.Set_MotAlarmClear(n_Axis);
			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
				strMsg.Format(_T("[Motor Alarm Reset_1] Axis=%d, Wait:%d"), n_Axis, 500);
				//wsprintfA(st_other_info.cNormalMsg, "%S", strMsg);
				//clsFunc.OnStringToChar(strMsg, st_other_info.cNormalMsg);
				clsMem.OnNormalMessageWrite(strMsg);
				st_handler_info.cWndList->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
			}
		}

		st_motor_info[n_Axis].l_retry_time_wait[1] = GetCurrentTime();
		st_motor_info[n_Axis].l_retry_time_wait[2] = st_motor_info[n_Axis].l_retry_time_wait[1] - st_motor_info[n_Axis].l_retry_time_wait[0];
		if(st_motor_info[n_Axis].l_retry_time_wait[2] < 0)
		{
			st_motor_info[n_Axis].n_retry_time_flag		= YES;
			st_motor_info[n_Axis].l_retry_time_wait[0]	= GetCurrentTime();

			return RET_RETRY;
		}
		else if(st_motor_info[n_Axis].l_retry_time_wait[2] < 500) //500ms초 이상이 되었을때 다음조건 체크)
		{
			return RET_RETRY;
		}

		if (COMI.Set_MotAlarmClear(n_Axis) == RET_GOOD)       // 모터 ALARM CLEAR 함수 
		{
			st_motor_info[n_Axis].n_retry_time_flag = NO; //091216 

			//091119 james			Sleep(1000);  // 일정 시간 후에 상태 확인하기 위해 SLEEP 사용한다 
			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
				strMsg.Format(_T("[Motor Alarm Reset_2] Axis=%d, Wait:%d"), n_Axis, 500);
				//wsprintfA(st_other_info.cNormalMsg, "%S", strMsg);
				//clsFunc.OnStringToChar(strMsg, st_other_info.cNormalMsg);
				clsMem.OnNormalMessageWrite(strMsg);
				st_handler_info.cWndList->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
			}

			if (COMI.Get_MotAlarmStatus(n_Axis) == RET_ERROR )  // 모터 ALARM 상태 검사 함수 
			{
				if (st_motor_info[n_Axis].n_retry_cnt > MOT_RTY_CNT)
				{			
					st_alarm_info.strCode.Format(_T("%02d0002"), n_Axis);
					CtlBdFunc.ms_ErrMsg = _T("[Motor_SafetyCheck_1] Alarm Check Error");				
					CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_Axis, st_alarm_info.strCode, CtlBdFunc.ms_ErrMsg);
					st_motor_info[n_Axis].n_retry_cnt = 0;  // 알람 해제 시도 횟수

					return RET_ERROR;
				}
				else
				{
					st_motor_info[n_Axis].n_retry_cnt++ ;

					return RET_RETRY;
				}
			}
		}
		else
		{
			st_motor_info[n_Axis].n_retry_time_flag = NO; //091216 

			if (st_motor_info[n_Axis].n_retry_cnt > MOT_RTY_CNT)
			{
				st_alarm_info.strCode.Format(_T("%02d0002"), n_Axis);
				CtlBdFunc.ms_ErrMsg = _T("[Motor_SafetyCheck_2] Alarm Check Error");				
				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_Axis, st_alarm_info.strCode, CtlBdFunc.ms_ErrMsg);
				
				st_motor_info[n_Axis].n_retry_cnt = 0;  // 알람 해제 시도 횟수 

				return RET_ERROR;
			}
			else
			{
				st_motor_info[n_Axis].n_retry_cnt++ ;

				return RET_RETRY;
			}
		}
	}
	// **************************************************************************

	// **************************************************************************
	// 모터 파워 상태 검사한다                                                   
	// -> 모터 POWER OFF 시 POWER ON 상태로 만든다                               
	// **************************************************************************
	if (COMI.Get_MotPower(n_Axis) == RET_ERROR )          // 모터 POWER 상태 검사 함수 
	{
		//091216 추가 
		if(st_motor_info[n_Axis].n_retry_time_flag == NO)
		{
			st_motor_info[n_Axis].n_retry_time_flag		= YES;
			st_motor_info[n_Axis].l_retry_time_wait[0]	= GetCurrentTime();

			COMI.Set_MotPower(n_Axis, NO);
			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
				strMsg.Format(_T("[Motor Power Reset_1] Axis=%d, Wait:%d"), n_Axis, 500);
				//wsprintfA(st_other_info.cNormalMsg, "%S", strMsg);
				//clsFunc.OnStringToChar(strMsg, st_other_info.cNormalMsg);
				clsMem.OnNormalMessageWrite(strMsg);
				st_handler_info.cWndList->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
			}
		}

		st_motor_info[n_Axis].l_retry_time_wait[1] = GetCurrentTime();
		st_motor_info[n_Axis].l_retry_time_wait[2] = st_motor_info[n_Axis].l_retry_time_wait[1] - st_motor_info[n_Axis].l_retry_time_wait[0];
		if(st_motor_info[n_Axis].l_retry_time_wait[2] < 0)
		{
			st_motor_info[n_Axis].n_retry_time_flag		= YES;
			st_motor_info[n_Axis].l_retry_time_wait[0]	= GetCurrentTime();
			
			return RET_RETRY;
		}
		else if(st_motor_info[n_Axis].l_retry_time_wait[2] < 500) //500ms초 이상이 되었을때 다음조건 체크)
		{
			return RET_RETRY;
		}

		if (COMI.Set_MotPower(n_Axis, NO) == RET_GOOD)       // 모터 POWER ON 설정 함수 
		{
			st_motor_info[n_Axis].n_retry_time_flag = NO; //091216 

//091119 james			Sleep(1000);  // 일정 시간 후에 상태 확인하기 위해 SLEEP 사용한다 
			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
				strMsg.Format(_T("[Motor Power Reset_2] Axis=%d, Wait:%d"), n_Axis, 500);
				//wsprintfA(st_other_info.cNormalMsg, "%S", strMsg);
				//clsFunc.OnStringToChar(strMsg, st_other_info.cNormalMsg);
				clsMem.OnNormalMessageWrite(strMsg);
				st_handler_info.cWndList->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
			}

			if (COMI.Get_MotPower(n_Axis) == RET_ERROR)  // 모터 POWER 상태 검사 함수 
			{
				if (st_motor_info[n_Axis].n_retry_cnt > (MOT_RTY_CNT))
				{ 
					st_alarm_info.strCode.Format(_T("%02d0005"), n_Axis);
					CtlBdFunc.ms_ErrMsg = _T("[Motor_SafetyCheck_1] Power Check Error");				
					CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_Axis, st_alarm_info.strCode, CtlBdFunc.ms_ErrMsg);

					st_motor_info[n_Axis].n_retry_cnt = 0;
					return RET_ERROR;
				}
				else
				{
					st_motor_info[n_Axis].n_retry_cnt++ ;
					return RET_RETRY;
				}
			}
		}
		else
		{
			st_motor_info[n_Axis].n_retry_time_flag = NO; //091216 

			if (st_motor_info[n_Axis].n_retry_cnt > MOT_RTY_CNT)
			{
				st_alarm_info.strCode.Format(_T("%02d0005"),  n_Axis); 
				CtlBdFunc.ms_ErrMsg = _T("[Motor_SafetyCheck_2] Power Check Error");				
				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_Axis, st_alarm_info.strCode, CtlBdFunc.ms_ErrMsg);
				st_motor_info[n_Axis].n_retry_cnt = 0;
				return RET_ERROR;
			}
			else
			{
				st_motor_info[n_Axis].n_retry_cnt++ ;
				return RET_RETRY;
			}
		}
	}

	st_motor_info[n_Axis].n_retry_time_flag = NO;//091216
 */
	return RET_GOOD;
}


int CCtlBd_Function::Initialize_FASIO_Board(void) 
{
	int m=0, p=0, s=0;
	int nRet=0;
	WORD wMaskMap = 0;
/*
	INT nHSSI_speed=0;
	INT nMasterNo=0, nPortNo =0, nSlaveNo=0; 

	nRet = FAS_IO.Set_IO_BoardOpen(&nMasterNo, START_NOTHING);  // I/O 보드 오픈 함수 
	if (nRet == CTLBD_RET_ERROR)
	{
		return CTLBD_RET_ERROR; //IO_BOARD_OPEN_FAILED;
	}

	nRet = FAS_IO.Search_IO_Master(&nMasterNo);              // PC에 연결된 마스터 보드 갯수 검사 함수 
	if (nRet == CTLBD_RET_ERROR)
	{
		return CTLBD_RET_ERROR;
	}
	
	for(m=0; m<nMasterNo; m++)
	{
		nRet = FAS_IO.Search_IO_Port(m, &nPortNo);						// 마스터 보드에 연결된 PORT 갯수 검사 함수 
		if (nRet == CTLBD_RET_ERROR) return CTLBD_RET_ERROR;	//PORT_SEARCH_FAILED;
		
		for(p=0; p<nPortNo; p++)
		{
			nRet = FAS_IO.Set_IO_HSSISpeed(m, p, PORT_SPEED_10);		// PORT와 SLAVE I/O 모듈과의 통신 속도 설정 함수 
			if (nRet == CTLBD_RET_ERROR) return CTLBD_RET_ERROR; //IO_SPEED_SET_FAILED;
			 
			nRet = FAS_IO.Search_IO_Slave(m, p, &nSlaveNo);			// PORT에 연결된 SLAVE 검사 함수 
			if (nRet == CTLBD_RET_ERROR) return CTLBD_RET_ERROR; //IO_SLAVE_SEARCH_FAILED;

			for(s=0; s<nSlaveNo; s++)
			{
				nRet = FAS_IO.Set_IO_SlaveEnable(m, p, s, CTLBD_YES);		// SLAVE I/O 모듈 동작 ENABLE/DISABLE 설정 함수 
				if (nRet == CTLBD_RET_ERROR)	return CTLBD_RET_ERROR; //IO_SLAVE_ENABLE_FAILED;

				wMaskMap = 0; //clear
				for(i=0; i<16; i++) //0~15 까지의 Port 16bit 셋팅, 각각의 I/O 상태를 정의한다 
				{			
					wMaskMap = wMaskMap | (WORD)pow(2, mw_IOMaskMap[m][p][s][i]);					 
				}
				FAS_IO.Set_IO_DefineWord(m, p, s, wMaskMap); // x Master, x포트, x번 슬레이브 , 16bit 셋팅
				//FAS_IO.Set_IO_DefineWord(0, 0, 0, 0x0003); // 0번 Master, 0번 포트, 0번 슬레이브 , 16bit 셋팅
				//FAS_IO.Set_IO_DefineWord(0, 0, 1, 0x0003); // 0번 Master, 0번 포트, 1번 슬레이브 , 16bit 셋팅
				//FAS_IO.Set_IO_DefineWord(0, 1, 0, 0x00ff); // 0번 Master, 1번 포트, 0번 슬레이브 , 16bit 셋팅
			}
		}
	}
*/	
	return RET_GOOD;
}



int CCtlBd_Function::Initialize_InterruptMask_TriggerConfig(void) 
{
	int i, nAxis;
//	st_motor[0].l_Set_InterruptMask[12] = CTL_YES;
//	COMI.Set_InterruptMask(0, st_motor[0].l_Set_InterruptMask);
	
//	double dRatio[3] = {100,100,100};
	//Set_TriggerSet_OneCompareConfig(int n_Axis, int n_Source, int n_Method, double d_ComparePosition)
//	COMI.Set_TriggerSet_OneCompareConfig(0, 0, 1, 5000);


//	st_motor[].n_interrupt_flag

/*
	"Normal Stop", // 0
	"Succesive start", // 1
	"", // 2
	"", // 3
	"Start of acceleration", //4
	"End of acceleration", // 5
	"Start of deceleration", // 6
	"End of deceleration", // 7
	"", // 8
	"", // 9
	"Position error tolerance exceed", // 10
	"General Comparator", // 11
	"Compared trigger", // 12
	"CLR signal resets the counter value", // 13
	"LTC input making counter value latched", // 14
	"ORG input signal ON", // 15
	"SD input signal ON", // 16
	"+DR input changes", // 17
	"-DR input changes" // 18

int		n_interrupt_source;  //인터럽트 처리 조건 
	    //n_Mode : 	szCompareSource[4][50] = { 
		//0				"COMMAND Counter",
		//1				"Feedback Counter",
		//2			    "Deviation Counter",
		//3			    "General Counter"

	int		n_interrupt_method; //사용하는 조건 정의  
		//CHAR szCompareMethod[6][50] = {
		//0				"Disable",
		//1				"CMP == COUNTER (No direction)",
		//2				"CMP == COUNTER (Only when counting up)",
		//3				"CMP == COUNTER (Only when counting down)",
		//4				"CMP > COUNTER",
		//5				"CMP < COUNTER"
*/	
	for(i=0; i<5; i++)	//for(nAxis=0; nAxis<MOT_MAXMOTOR; nAxis++)
	{
		if(i == 0)
		{
			nAxis = 0; 
			st_motor_info[nAxis].n_interrupt_flag = YES; //dc load robot Y 무조건 설정 
		}
		else if(i == 1)
		{
			nAxis = 5;
			st_motor_info[nAxis].n_interrupt_flag = YES; //dc unload robot Y 무조건 설정
		}
		else if(i == 2) 
		{
			nAxis = 16;	
			st_motor_info[nAxis].n_interrupt_flag = YES; //insert robot Y 무조건 설정
		}
		else if(i == 3)
		{
			nAxis = 19;		//2009.5.27 james 소켓 푸쉬 추가 
			//st_motor[nAxis].n_interrupt_flag = CTL_YES; //소켓 푸쉬 무조건 설정
		}
		else if(i == 4)
		{
			nAxis = 20;	
			st_motor_info[nAxis].n_interrupt_flag = YES; //remove robot Y 무조건 설정
		}

		if(st_motor_info[nAxis].n_interrupt_flag == YES) //인터럽트를 사용하는 모터이면 
		{
			st_motor_info[nAxis].l_Set_InterruptMask[0]		= NO; //"Normal Stop", // 0
			st_motor_info[nAxis].l_Set_InterruptMask[1]		= NO; //"Succesive start", // 1
			st_motor_info[nAxis].l_Set_InterruptMask[2]		= NO; //"", // 2
			st_motor_info[nAxis].l_Set_InterruptMask[3]		= NO; //"", // 3
			st_motor_info[nAxis].l_Set_InterruptMask[4]		= NO; //"Start of acceleration", //4
			st_motor_info[nAxis].l_Set_InterruptMask[5]		= NO; //End of acceleration", // 5
			st_motor_info[nAxis].l_Set_InterruptMask[6]		= NO; //"Start of deceleration", // 6
			st_motor_info[nAxis].l_Set_InterruptMask[7]		= NO; //"End of deceleration", // 7
			st_motor_info[nAxis].l_Set_InterruptMask[8]		= NO; //"", // 8
			st_motor_info[nAxis].l_Set_InterruptMask[9]		= NO; //"", // 9
			st_motor_info[nAxis].l_Set_InterruptMask[10]	= NO; //"Position error tolerance exceed", // 10
			st_motor_info[nAxis].l_Set_InterruptMask[11]	= NO; //"General Comparator", // 11

			//이것만 우선 사용한다 
			st_motor_info[nAxis].l_Set_InterruptMask[12]	= YES; //"Compared trigger", // 12

			st_motor_info[nAxis].l_Set_InterruptMask[13]	= NO; //"CLR signal resets the counter value", // 13
			st_motor_info[nAxis].l_Set_InterruptMask[14]	= NO; //"LTC input making counter value latched", // 14
			st_motor_info[nAxis].l_Set_InterruptMask[15]	= NO; //"ORG input signal ON", // 15
			st_motor_info[nAxis].l_Set_InterruptMask[16]	= NO; //"SD input signal ON", // 16
			st_motor_info[nAxis].l_Set_InterruptMask[17]	= NO; //"+DR input changes", // 17
			st_motor_info[nAxis].l_Set_InterruptMask[18]	= NO; //"-DR input changes" // 18

			//인터럽트 마스키 정보를 셋팅한다 
			//Set_InterruptMask(int n_Axis, long l_Set_InterruptMask[19])
			COMI.Set_InterruptMask(nAxis, st_motor_info[nAxis].l_Set_InterruptMask);

			//인터럽트 발생 조건 및 구간 설정 
			//Set_TriggerSet_OneCompareConfig(int n_Axis, int n_Source, int n_Method, double d_ComparePosition)
			COMI.Set_TriggerSet_OneCompareConfig(nAxis, st_motor_info[nAxis].n_interrupt_source, st_motor_info[nAxis].n_interrupt_method, st_motor_info[nAxis].n_interrupt_position);
		}
	}		
	return RET_GOOD;
}
