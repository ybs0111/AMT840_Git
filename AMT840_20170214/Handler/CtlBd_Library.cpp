// CtlBd_Library.cpp: implementation of the CCtlBd_Library class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CtlBd_Library.h"
#include "io.h"			//2011.0514  파일 존재 유무 검사 함수 호출하기 위해서는 반드시 필요
#include "ComizoaPublic.h"
#include "FastechPublic_IO.h"
#include "PublicFunction.h"
#include "CtlBd_Function.h"
#include "Variable.h"
#include "math.h"
#include "CtlBd_Variable.h"
#include "RunHandlerCheck.h"
#include "AlgMemory.h"
#include "LogFromat.h"
//#include "CmmsdkDef.h"

#ifdef _WIN64
#include ".\\64bit\\Cmmsdk.h"
#include ".\\64bit\\CmmsdkDef.h"
#else
#include ".\\32bit\\Cmmsdk.h"
#include ".\\32bit\\CmmsdkDef.h"
#endif

st_linearmotion_param st_linearmot[8];

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCtlBd_Library::CCtlBd_Library()
{
	memset(m_dMvOldPos, 0, sizeof(m_dMvOldPos));
	memset(m_nRetryCnt, 0, sizeof(m_nRetryCnt));
}

CCtlBd_Library::~CCtlBd_Library()
{

}

CCtlBd_Library CTL_Lib;

//////////////////////////////////////////////////////////////////////////////
//version:0.1 (2011.1026) 
//알람발생 처리방법, 모터 및 I/O 관련 라이브러리를 초기화 및 응용하여 실제사용하는 함수정리 
//
//참고 기본함수 
//1. #include "ComizoaPublic.h"
//2. #include "FastechPublic_IO.h"
///////////////////////////////////////////////////////////////////////////////





int CCtlBd_Library::Alarm_Error_Occurrence(int nJamPos, int nRunStatus, CString strCode)
{
	//그간은 알람이 발생하면 각 쓰레드에서 jamcode 및 run status 정보를 바뀌어 장비를 정지하고 
	//문제를 해결했으나 앞으로는 이 함수를 모든곳에서 사용하여 알람정보 및 가동 상태를 표준함수 하나를 
	//사용하여 장비 가동 및 알람 문제를 처리한다 
	//
	//각 쓰레드에 각각있던 알람 처리방법은 이 함수를 모두 사용한다 
   //
//	CTL_Lib.mn_jampos = n_jampos; //jam이 발생한 위치를 저장한다 
//	CTL_Lib.mn_run_status = n_run_status; //장비의 가동상태를 변경한다 
//	COMI.mn_run_status = n_run_status;
	CString sAlarmMsg;

	st_alarm_info.strCode				= strCode; 
	st_alarm_info.nCountMode			= 0;	
	st_alarm_info.nTypeMode				= eWARNING;
	st_alarm_info.nAlarmAssignSection	= nJamPos;

	if(nRunStatus != dMOTOR_WARNING)
	{
		st_handler_info.nRunStatus			= nRunStatus;
	}
	
	if (st_handler_info.cWndList != NULL)  
	{
		if (strCode == _T("") || strCode == _T("999999"))
		{
			sAlarmMsg.Format(_T("Alarm Pos = %d"), nJamPos);
		}
		else
		{
			sAlarmMsg.Format(_T("Alarm Code = %s"), strCode);
		}
		//			wsprintfA(st_other_info.cAbnormalMsg, "%S", st_other_info.strAbnormalMsg);
		//clsFunc.OnStringToChar(st_other_info.strAbnormalMsg, st_other_info.cAbnormalMsg);
		clsMem.OnAbNormalMessagWrite(sAlarmMsg);
		// 동작 실패 출력 요청
		//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  
	}
// jtkim 20130704
//	strcpy(mc_alarmcode, c_alarmcode);
//	strcpy_s(mc_alarmcode, sizeof(mc_alarmcode), c_alarmcode);

 	return BD_GOOD;
}


int CCtlBd_Library::Initialize_motor_board(int n_bd_type, CString s_filename)	//motor 보드 초기화시 사용
{
	int nRet = 0, i=0;
	int nMotor[2]={0,};
//	long lMotorMap[2];
	CString strMsg;
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	//Motor Board Initialize 
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	if(n_bd_type == 0) //0:파스텍 보드를 사용하는 장비이면, 1:커미조아 모터를 사용하는 장비 
	{		//모터 보드별 축수를 정의한다 

		COMI.mn_totalmotboard_number = 3;  //모터보드가 3개 있다 
		COMI.mn_totalmotoraxis_number= 23; //모터의 총 수량은 23개이다  
		COMI.mn_bd_axis_number[0] = 8;   //0번 모터보드는 8축용 보드이다  
		COMI.mn_bd_axis_number[1] = 7;   //1번 모터보드는 8축용 보드이다 
		COMI.mn_bd_axis_number[2] = 8;   //2번 모터보드는 4축용 보드이다 
 	}
	else if(n_bd_type == 1) //1:커미조아 보드를 사용하는 장비이면, 1:커미조아 모터를 사용하는 장비 
	{
		//1. 보드 정보 읽어들인다 
		//2. 각종 셋팅 정보를 적용한다 
		nRet = COMI.Initialize_MotBoard(s_filename);			// MOTOR 보드 초기화 여부 검사한다
		if (nRet == BD_ERROR)
		{
			wsprintfA(mc_normal_msg, "%S", _T("[MOTOR BOARD] Initialize Error"));
			clsFunc.OnStringToChar(_T("[MOTOR BOARD] Initialize Error"), mc_normal_msg);
//			sprintf(mc_normal_msg,"[MOTOR BOARD] 초기화 에러.");
//			LogFile_Write(_T("c:\\AMT_LOG\\Motor\\"), mc_normal_msg);
			return BD_ERROR;
		}		

		COMI.mn_totalmotboard_number = 3;  //모터보드가 3개 있다 
		COMI.mn_totalmotoraxis_number= 23; //모터의 총 수량은 23개이다  
		COMI.mn_bd_axis_number[0] = 8;   //0번 모터보드는 8축용 보드이다  
		COMI.mn_bd_axis_number[1] = 7;   //1번 모터보드는 8축용 보드이다 
		COMI.mn_bd_axis_number[2] = 8;   //2번 모터보드는 8축용 보드이다 

		if(COMI.mn_motorbd_init_end == BD_YES)
		{			
			for(i=0; i<COMI.mn_totalmotoraxis_number; i++)
			{//n_simul_mode => 0:시뮬레이션 모드 비활성, 1:시뮬레이션 모드 활성

				if(st_work_info.nSimulationMode == 1) //모터를 가상으로 제어하여 실제 모터 출력이 나가고 기구물이 이동한것처럼 동작한다 
				{//활성
					COMI.Set_Simulation_Mode(i, 1); 
				}
				else
				{//비활성 
					COMI.Set_Simulation_Mode(i, 0); 
				}
			}		
		}
		//0: loading tray conveyor left/right shifter
		nMotor[0] = M_LD_CV_LR_SHIFTER;
		// 1mm 이동하는데 필요한 pulse
		COMI.Set_MotUnitDist(nMotor[0], 500);
		// 10mm 이동하는데 필요한 pulse (속도는 1Cm/초로 세팅하게 된다.)
		COMI.Set_MotUnitSpeed(nMotor[0], 500);
		// motor Home
		COMI.mn_homecheck_method[nMotor[0]]= 6;
		COMI.Set_MotorType_Init(nMotor[0], MOT_SERVO);

		//1: loading tray stacker elevator
		nMotor[0] = M_LD_ELV;
		// 1mm 이동하는데 필요한 pulse
		COMI.Set_MotUnitDist(nMotor[0], 1000);
		// 10mm 이동하는데 필요한 pulse (속도는 1Cm/초로 세팅하게 된다.)
		COMI.Set_MotUnitSpeed(nMotor[0], 1000);
		// motor Home
		COMI.mn_homecheck_method[nMotor[0]]= 6;

		COMI.Set_Motor_IO_Property(nMotor[0], cmSD_MODE, cmTRUE);
		COMI.Set_Motor_IO_Property(nMotor[0], cmSD_EN, cmFALSE);    //cmSD_EN=14 //cmFALSE = 0 SD 비활성, cmTRUE = 1 SD 활성 	
		COMI.Set_Motor_IO_Property(nMotor[0], cmSD_LOGIC, cmLOGIC_A); //cmSD_LOGIC=15, 0 (cmLOGIC_A) : A접점 방식,1 (cmLOGIC_B) : B접점 방식
		COMI.Set_Motor_IO_Property(nMotor[0], cmSD_LATCH, cmFALSE);//16
		COMI.Set_MotorType_Init(nMotor[0], MOT_SERVO);


		//2: empty tray stacker elevator 
		nMotor[0] = M_EMPTY_ELV;
		// 1mm 이동하는데 필요한 pulse
		COMI.Set_MotUnitDist(nMotor[0], 1000);
		// 10mm 이동하는데 필요한 pulse (속도는 1Cm/초로 세팅하게 된다.)
		COMI.Set_MotUnitSpeed(nMotor[0], 1000);
		// motor Home
		COMI.mn_homecheck_method[nMotor[0]]= 6;

		COMI.Set_Motor_IO_Property(nMotor[0], cmSD_MODE, cmTRUE);
		COMI.Set_Motor_IO_Property(nMotor[0], cmSD_EN, cmFALSE);    //cmSD_EN=14 //cmFALSE = 0 SD 비활성, cmTRUE = 1 SD 활성 	
		COMI.Set_Motor_IO_Property(nMotor[0], cmSD_LOGIC, cmLOGIC_A); //cmSD_LOGIC=15, 0 (cmLOGIC_A) : A접점 방식,1 (cmLOGIC_B) : B접점 방식
		COMI.Set_Motor_IO_Property(nMotor[0], cmSD_LATCH, cmFALSE);//16
		COMI.Set_MotorType_Init(nMotor[0], MOT_SERVO);
		COMI.Set_MotorType_Init(nMotor[0], MOT_SERVO);


		//3: unload tray #1(left) stacker elevator 
		nMotor[0] = M_ULD_1_ELV;	
		// 1mm 이동하는데 필요한 pulse
		COMI.Set_MotUnitDist(nMotor[0], 1000);
		// 10mm 이동하는데 필요한 pulse (속도는 1Cm/초로 세팅하게 된다.)
		COMI.Set_MotUnitSpeed(nMotor[0], 1000);
		// motor Home
		COMI.mn_homecheck_method[nMotor[0]]= 6;

		COMI.Set_Motor_IO_Property(nMotor[0], cmSD_MODE, cmTRUE);
		COMI.Set_Motor_IO_Property(nMotor[0], cmSD_EN, cmFALSE);    //cmSD_EN=14 //cmFALSE = 0 SD 비활성, cmTRUE = 1 SD 활성 	
		COMI.Set_Motor_IO_Property(nMotor[0], cmSD_LOGIC, cmLOGIC_A); //cmSD_LOGIC=15, 0 (cmLOGIC_A) : A접점 방식,1 (cmLOGIC_B) : B접점 방식
		COMI.Set_Motor_IO_Property(nMotor[0], cmSD_LATCH, cmFALSE);//16
		COMI.Set_MotorType_Init(nMotor[0], MOT_SERVO);


		//4: unload tray #2(right) stacker elevator 
		nMotor[0] = M_ULD_2_ELV;	
		// 1mm 이동하는데 필요한 pulse
		COMI.Set_MotUnitDist(nMotor[0], 1000);
		// 10mm 이동하는데 필요한 pulse (속도는 1Cm/초로 세팅하게 된다.)
		COMI.Set_MotUnitSpeed(nMotor[0], 1000);
		// motor Home
		COMI.mn_homecheck_method[nMotor[0]]= 6;

		COMI.Set_Motor_IO_Property(nMotor[0], cmSD_MODE, cmTRUE);
		COMI.Set_Motor_IO_Property(nMotor[0], cmSD_EN, cmFALSE);    //cmSD_EN=14 //cmFALSE = 0 SD 비활성, cmTRUE = 1 SD 활성 	
		COMI.Set_Motor_IO_Property(nMotor[0], cmSD_LOGIC, cmLOGIC_A); //cmSD_LOGIC=15, 0 (cmLOGIC_A) : A접점 방식,1 (cmLOGIC_B) : B접점 방식
		COMI.Set_Motor_IO_Property(nMotor[0], cmSD_LATCH, cmFALSE);//16
		COMI.Set_MotorType_Init(nMotor[0], MOT_SERVO);


		//5: reject tray empty stacker elevator 
		nMotor[0] = M_RJ_EMPTY_ELV;
		// 1mm 이동하는데 필요한 pulse
		COMI.Set_MotUnitDist(nMotor[0], 1000);
		// 10mm 이동하는데 필요한 pulse (속도는 1Cm/초로 세팅하게 된다.)
		COMI.Set_MotUnitSpeed(nMotor[0], 1000);
		// motor Home
		COMI.mn_homecheck_method[nMotor[0]]= 6;

		COMI.Set_Motor_IO_Property(nMotor[0], cmSD_MODE, cmTRUE);
		COMI.Set_Motor_IO_Property(nMotor[0], cmSD_EN, cmFALSE);    //cmSD_EN=14 //cmFALSE = 0 SD 비활성, cmTRUE = 1 SD 활성 	
		COMI.Set_Motor_IO_Property(nMotor[0], cmSD_LOGIC, cmLOGIC_A); //cmSD_LOGIC=15, 0 (cmLOGIC_A) : A접점 방식,1 (cmLOGIC_B) : B접점 방식
		COMI.Set_Motor_IO_Property(nMotor[0], cmSD_LATCH, cmFALSE);//16
		COMI.Set_MotorType_Init(nMotor[0], MOT_SERVO);

		//6: reject tray #1(front) stacker elevator
		nMotor[0] = M_RJ_ULD_1_ELV;	
		// 1mm 이동하는데 필요한 pulse
		COMI.Set_MotUnitDist(nMotor[0], 1000);
		// 10mm 이동하는데 필요한 pulse (속도는 1Cm/초로 세팅하게 된다.)
		COMI.Set_MotUnitSpeed(nMotor[0], 1000);
		// motor Home
		COMI.mn_homecheck_method[nMotor[0]]= 6;

		COMI.Set_Motor_IO_Property(nMotor[0], cmSD_MODE, cmTRUE);
		COMI.Set_Motor_IO_Property(nMotor[0], cmSD_EN, cmFALSE);    //cmSD_EN=14 //cmFALSE = 0 SD 비활성, cmTRUE = 1 SD 활성 	
		COMI.Set_Motor_IO_Property(nMotor[0], cmSD_LOGIC, cmLOGIC_A); //cmSD_LOGIC=15, 0 (cmLOGIC_A) : A접점 방식,1 (cmLOGIC_B) : B접점 방식
		COMI.Set_Motor_IO_Property(nMotor[0], cmSD_LATCH, cmFALSE);//16
		COMI.Set_MotorType_Init(nMotor[0], MOT_SERVO);


		//7: reject tray #2(rear) stacker elevator
		nMotor[0] = M_RJ_ULD_2_ELV;
		// 1mm 이동하는데 필요한 pulse
		COMI.Set_MotUnitDist(nMotor[0], 1000);
		// 10mm 이동하는데 필요한 pulse (속도는 1Cm/초로 세팅하게 된다.)
		COMI.Set_MotUnitSpeed(nMotor[0], 1000);
		// motor Home
		COMI.mn_homecheck_method[nMotor[0]]= 6;

		COMI.Set_Motor_IO_Property(nMotor[0], cmSD_MODE, cmTRUE);
		COMI.Set_Motor_IO_Property(nMotor[0], cmSD_EN, cmFALSE);    //cmSD_EN=14 //cmFALSE = 0 SD 비활성, cmTRUE = 1 SD 활성 	
		COMI.Set_Motor_IO_Property(nMotor[0], cmSD_LOGIC, cmLOGIC_A); //cmSD_LOGIC=15, 0 (cmLOGIC_A) : A접점 방식,1 (cmLOGIC_B) : B접점 방식
		COMI.Set_Motor_IO_Property(nMotor[0], cmSD_LATCH, cmFALSE);//16
		COMI.Set_MotorType_Init(nMotor[0], MOT_SERVO);


		//8: work robot x(work robot == load/unload robot)
		nMotor[0] = M_WORK_RBT_X;
		// 1mm 이동하는데 필요한 pulse
		COMI.Set_MotUnitDist(nMotor[0], 133.3235125);
		// 10mm 이동하는데 필요한 pulse (속도는 1Cm/초로 세팅하게 된다.)
		COMI.Set_MotUnitSpeed(nMotor[0], 133.3235125);
		// motor Home
		COMI.mn_homecheck_method[nMotor[0]]= 6;
		COMI.Set_MotorType_Init(nMotor[0], MOT_SERVO);

		//9: work robot y
		nMotor[0] = M_WORK_RBT_Y;		
		// 1mm 이동하는데 필요한 pulse
		//COMI.Set_MotUnitDist(nMotor[0], 227.335); //312.558804186754);
		COMI.Set_MotUnitDist(nMotor[0], 133.3235125); //312.558804186754);
		// 10mm 이동하는데 필요한 pulse (속도는 1Cm/초로 세팅하게 된다.)
		//COMI.Set_MotUnitSpeed(nMotor[0], 227.335); //312.558804186754);
		COMI.Set_MotUnitSpeed(nMotor[0], 133.3235125); //312.558804186754);
		// motor Home
		COMI.mn_homecheck_method[nMotor[0]]= 6;
		COMI.Set_MotorType_Init(nMotor[0], MOT_SERVO);

		//10: work robot z
		nMotor[0] = M_WORK_RBT_Z;	
		// 1mm 이동하는데 필요한 pulse
		COMI.Set_MotUnitDist(nMotor[0], 2000);//200.006211865404);
		// 10mm 이동하는데 필요한 pulse (속도는 1Cm/초로 세팅하게 된다.)
		COMI.Set_MotUnitSpeed(nMotor[0], 2000); //200.006211865404);
		// motor Home
		COMI.mn_homecheck_method[nMotor[0]]= 6;
		COMI.Set_MotorType_Init(nMotor[0], MOT_SERVO);

		//11: load buffer shifter Y
		nMotor[0] = M_LD_BUF_SHIFTER_Y;	
		// 1mm 이동하는데 필요한 pulse
		COMI.Set_MotUnitDist(nMotor[0], 1000);
		// 10mm 이동하는데 필요한 pulse (속도는 1Cm/초로 세팅하게 된다.)
		COMI.Set_MotUnitSpeed(nMotor[0], 1000);
		// motor Home
		COMI.mn_homecheck_method[nMotor[0]]= 6;
		COMI.Set_MotorType_Init(nMotor[0], MOT_SERVO);

		//12: load buffer rotator
		nMotor[0] = M_LD_BUF_ROTATOR;	
		// 1mm 이동하는데 필요한 pulse
		//COMI.Set_MotUnitDist(nMotor[0], 10);
		// 10mm 이동하는데 필요한 pulse (속도는 1Cm/초로 세팅하게 된다.)
		//COMI.Set_MotUnitSpeed(nMotor[0], 10);
		COMI.Set_MotUnitDist(nMotor[0], 100);
		// 10mm 이동하는데 필요한 pulse (속도는 1Cm/초로 세팅하게 된다.)
		COMI.Set_MotUnitSpeed(nMotor[0], 100);
		// motor Home
		COMI.mn_homecheck_method[nMotor[0]]= 1;
		COMI.Set_MotorType_Init(nMotor[0], MOT_SERVO);

		//13: unload buffer shifter Y
		nMotor[0] = M_ULD_BUF_SHIFTER_Y;	
		// 1mm 이동하는데 필요한 pulse
		COMI.Set_MotUnitDist(nMotor[0], 1000);
		// 10mm 이동하는데 필요한 pulse (속도는 1Cm/초로 세팅하게 된다.)
		COMI.Set_MotUnitSpeed(nMotor[0], 1000);
		// motor Home
		COMI.mn_homecheck_method[nMotor[0]]= 6;
		COMI.Set_MotorType_Init(nMotor[0], MOT_SERVO);

		// 14: unload buffer rotator
		nMotor[0] = M_ULD_BUF_ROTATOR;	
		// 1mm 이동하는데 필요한 pulse
		//COMI.Set_MotUnitDist(nMotor[0], 10);//111.102927114761);
		// 10mm 이동하는데 필요한 pulse (속도는 1Cm/초로 세팅하게 된다.)
		//COMI.Set_MotUnitSpeed(nMotor[0], 10);//111.102927114761);
		COMI.Set_MotUnitDist(nMotor[0], 100);//111.102927114761);
		// 10mm 이동하는데 필요한 pulse (속도는 1Cm/초로 세팅하게 된다.)
		COMI.Set_MotUnitSpeed(nMotor[0], 100);//111.102927114761);
		// motor Home
		COMI.mn_homecheck_method[nMotor[0]]= 1;
		COMI.Set_MotorType_Init(nMotor[0], MOT_SERVO);

		//15: spare 1


		//16: retest buffer #1(left) rotator
		nMotor[0] = M_RETEST_BUF_1_ROTATOR;	
		// 1mm 이동하는데 필요한 pulse
		//COMI.Set_MotUnitDist(nMotor[0], 10);
		//kwlee 2017.0331
		COMI.Set_MotUnitDist(nMotor[0], 100);
		// 10mm 이동하는데 필요한 pulse (속도는 1Cm/초로 세팅하게 된다.)
		//COMI.Set_MotUnitSpeed(nMotor[0], 10);
		//kwlee 2017.0331
		COMI.Set_MotUnitSpeed(nMotor[0], 100);
		// motor Homec
		COMI.mn_homecheck_method[nMotor[0]]= 1;
		COMI.Set_MotorType_Init(nMotor[0], MOT_SERVO);

		//17: retest buffer #2(right) rotator
		nMotor[0] = M_RETEST_BUF_2_ROTATOR;	
		// 1mm 이동하는데 필요한 pulse
		//COMI.Set_MotUnitDist(nMotor[0], 10);
		//kwlee 2017.0331
		COMI.Set_MotUnitDist(nMotor[0], 100);
		// 10mm 이동하는데 필요한 pulse (속도는 1Cm/초로 세팅하게 된다.)

		//COMI.Set_MotUnitSpeed(nMotor[0], 10);
		//kwlee 2017.0331
		COMI.Set_MotUnitSpeed(nMotor[0], 100);
		// motor Home
		COMI.mn_homecheck_method[nMotor[0]]= 1;
		COMI.Set_MotorType_Init(nMotor[0], MOT_SERVO);

		//18: work tray transfer
		nMotor[0] = M_WORKTRAY_TRANSFER;
		// 1mm 이동하는데 필요한 pulse
		COMI.Set_MotUnitDist(nMotor[0], 100.0031059);
		// 10mm 이동하는데 필요한 pulse (속도는 1Cm/초로 세팅하게 된다.)
		COMI.Set_MotUnitSpeed(nMotor[0], 100.0031059);
		// motor Home
		COMI.mn_homecheck_method[nMotor[0]]= 6;
		COMI.Set_MotorType_Init(nMotor[0], MOT_SERVO);

		//19: reject tray transfer
		nMotor[0] = M_REJECTTRAY_TRANSFER;	
		// 1mm 이동하는데 필요한 pulse
		COMI.Set_MotUnitDist(nMotor[0], 100.0031059);
		// 10mm 이동하는데 필요한 pulse (속도는 1Cm/초로 세팅하게 된다.)
		COMI.Set_MotUnitSpeed(nMotor[0], 100.0031059);
		// motor Home
		COMI.mn_homecheck_method[nMotor[0]]= 6;
		COMI.Set_MotorType_Init(nMotor[0], MOT_SERVO);

		//20: test robot y
		nMotor[0] = M_TEST_RBT_Y;	
		// 1mm 이동하는데 필요한 pulse
		//COMI.Set_MotUnitDist(nMotor[0], 333.309);
		COMI.Set_MotUnitDist(nMotor[0], 56.81568696);
		// 10mm 이동하는데 필요한 pulse (속도는 1Cm/초로 세팅하게 된다.)
		//COMI.Set_MotUnitSpeed(nMotor[0], 333.309);
		COMI.Set_MotUnitSpeed(nMotor[0], 56.81568696);
		// motor Home
		COMI.mn_homecheck_method[nMotor[0]]= 6;
		COMI.Set_MotorType_Init(nMotor[0], MOT_SERVO);

		//21: test robot x
		nMotor[0] = M_TEST_RBT_X;	
		// 1mm 이동하는데 필요한 pulse
		COMI.Set_MotUnitDist(nMotor[0], 333.3087813);
		// 10mm 이동하는데 필요한 pulse (속도는 1Cm/초로 세팅하게 된다.)
		COMI.Set_MotUnitSpeed(nMotor[0], 333.3087813);
		// motor Home
		COMI.mn_homecheck_method[nMotor[0]]= 6;
		COMI.Set_MotorType_Init(nMotor[0], MOT_SERVO);

		//22: test robot z
		nMotor[0] = M_TEST_RBT_Z;	
		// 1mm 이동하는데 필요한 pulse
		//COMI.Set_MotUnitDist(nMotor[0], 2500);
		COMI.Set_MotUnitDist(nMotor[0], 1000);
		// 10mm 이동하는데 필요한 pulse (속도는 1Cm/초로 세팅하게 된다.)
		//COMI.Set_MotUnitSpeed(nMotor[0], 2500);
		COMI.Set_MotUnitSpeed(nMotor[0], 1000);
		// motor Home
		COMI.mn_homecheck_method[nMotor[0]]= 6;
		COMI.Set_MotorType_Init(nMotor[0], MOT_SERVO);

		//23: test robot device eject z  (100w)
		nMotor[0] = M_TEST_DVC_EJECT_Z;	
		// 1mm 이동하는데 필요한 pulse
		//COMI.Set_MotUnitDist(nMotor[0], 1000);
		COMI.Set_MotUnitDist(nMotor[0], 2500);
		// 10mm 이동하는데 필요한 pulse (속도는 1Cm/초로 세팅하게 된다.)
		//COMI.Set_MotUnitSpeed(nMotor[0], 1000);
		COMI.Set_MotUnitSpeed(nMotor[0], 2500);
		// motor Home
		COMI.mn_homecheck_method[nMotor[0]]= 6;
		COMI.Set_MotorType_Init(nMotor[0], MOT_SERVO);

		for (i = 0; i < MAXMOTOR; i++)
		{
			COMI.md_limit_position[i][0]	= st_motor_info[i].d_limit_position[0];
			COMI.md_limit_position[i][1]	= st_motor_info[i].d_limit_position[1];

			COMI.md_spd_vel[i][0]			= st_motor_info[i].d_spd_vel[0];
			//COMI.md_spd_vel[i][1]			= st_motor_info[i].d_spd_vel[1];
			//COMI.md_spd_vel[i][2]			= st_motor_info[i].d_spd_vel[2];
			//2016.1209
			COMI.md_spd_vel[i][1]			= st_motor_info[i].d_accel[0];
			COMI.md_spd_vel[i][2]			= st_motor_info[i].d_decel[0];

			COMI.md_spd_home[i]				= st_motor_info[i].d_spd_home[0];
			COMI.md_spd_jog[i]				= st_motor_info[i].d_spd_jog[0];

			COMI.md_allow_value[i]			= st_motor_info[i].d_allow; 

			//		COMI.mn_homecheck_method[i]		= 6;	//el on-> stop-> back-> el off - > stop 
			nRet = COMI.Set_HomeSetConfig(i, COMI.mn_homecheck_method[i], 0, 2, 1);
			if (nRet != BD_GOOD)
			{
				if (st_handler_info.cWndList != NULL)
				{
					strMsg.Format(_T("Set_HomeSet_Config Error [MotNum[%d]]"), i);
					//wsprintfA(st_other_info.cAbnormalMsg, "%S", strMsg);
					//clsFunc.OnStringToChar(strMsg, st_other_info.cAbnormalMsg);
					clsMem.OnAbNormalMessagWrite(strMsg);
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 완료 출력 요청
				}
			}

			COMI.Set_MotPower(i, TRUE); //2015.2015

			//20150428 james 
			COMI.Set_CMD_CTL_Mode(i, cmCTRL_SEMI_C);//20150429 james cmCTRL_SEMI_C , 엔코더,(피드백기준) 기준으로 목표좌표를 설정하여 이송합니다(커멘드 정보는 무시되고 최종 feedback(엔코더) 위치로 제어가 이루어진다.
		}

		/*
		//예)		
		//COMI.Set_MotUnitDist(M_DCLDRBT_Y, 187.5);						 
		//COMI.Set_MotUnitDist(M_DCLDRBT_X, 500);  
		*/

		/* 예)
		//for (i = 0; i < MOT_MAXMOTOR; i++)
		//{
		//	COMI.Set_CMD_CTL_Mode(i, cmCTRL_SEMI_C);//cmCTRL_SEMI_C , 엔코더,(피드백기준) 기준으로 목표좌표를 설정하여 이송합니다
		//}*/

		/*예
	//	for (i = 0; i < COMI.mn_totalmotoraxis_number; i++) //2011.0126 Inposition를 default로 기능 추가 
	//	{
	// 		if(i == M_DCLDRBT_Y || i == M_DCLDRBT_X || i == M_LDTRANSFER_X || i == M_LDTRANSFER_Z || i == M_EMPTYTRANSFER_X || i == M_EMPTYTRANSFER_Z || i == M_ULDTRANSFER_X || i == M_ULDTRANSFER_Z)
	// 		{
	// 			COMI.Set_Motor_IO_Property(i, cmINP_EN, cmTRUE); //inposition 적용 
	// 		}
	// 		else
	// 		{
	// 			COMI.Set_Motor_IO_Property(i, cmINP_EN, cmFALSE);  //inposition 제외 
	// 		}
	//		COMI.Set_Motor_IO_Property(i, cmINP_EN, cmTRUE); //inposition 적용 
	//	}*/

		/* 예)
		//최대 출력 주파수를 설정하는 명령으로 최소는 
		//2011.0106 james 추가 15.26 pps(min) ~ 1,000,000 pps(max), 6000rpm	
		////////////////////////최소 주파수 설정 공식////////////////////////////////////////////
		//MinPPS = MaxPPS / 65,535
		//예를 들어서 MaxPPS 가 655,350 이면 MinPPS = 655,350 / 65,535 = 10 (PPS)이 됩니다.
		for (i = 0; i < COMI.mn_totalmotoraxis_number; i++) //2011.0126 Inposition를 default로 기능 추가 
		{
			if(i == M_DCLDRBT_Z || i == M_DCULDRBT_Z || i == M_INSERTRBT_Z || i == M_REMOVERBT_Z || i == M_SORTBUFFRBT_Z || i == M_ULDRBT_Z)
			{
				cmmCfgSetSpeedRange(i, 1000000);//2011.0106 cmmCfgSetSpeedRange)	(long Axis, double MaxPPS);
			}
			else
			{
				cmmCfgSetSpeedRange(i, 650000);
			}
		}*/

		/* 예)
		// 초기 속도를 2000으로 잡는다. 셋 팅이 없으면 0
  		COMI.Set_MotInitSpeed(M_DCLDRBT_Z, 20000);
  		COMI.Set_MotInitSpeed(M_DCULDRBT_Z, 20000);
	    */


		/* 예)
		//////////////////////////////////////////////////////
		//SD Sensor 관련 변수 테스트 
		//////////////////////////////////////////////////////
		

		COMI.Set_Motor_IO_Property(M_LDTRANSFER_Z, cmSD_EN, cmTRUE);    //cmSD_EN=14 //cmFALSE = 0 SD 비활성, cmTRUE = 1 SD 활성 	
		COMI.Set_Motor_IO_Property(M_LDTRANSFER_Z, cmSD_LOGIC, cmLOGIC_A); //cmSD_LOGIC=15, 0 (cmLOGIC_A) : A접점 방식,1 (cmLOGIC_B) : B접점 방식
		COMI.Set_Motor_IO_Property(M_LDTRANSFER_Z, cmSD_LATCH, cmFALSE);//16
		//0 (cmFALSE) : SD가 ON되어 감속중이거나 초기속도로 운전중일 때 SD신호가 다시 OFF상태로 변경되면 작업속도까지 다시 가속됩니다
		//1 (cmTRUE) : SD가 ON상태에서 OFF상태로 바뀌어도 작업속도로 가속하지 않습니다.																						
		COMI.Set_Motor_IO_Property(M_LDTRANSFER_Z, cmSD_MODE, cmTRUE);//17
		//0 : SD신호가 ON되면 초기속도까지 감속합니다(정지(停止)하지 않음).
		//1 : SD신호가 ON되면 감속 후 정지(停止)합니다.

		COMI.Set_Motor_IO_Property(M_EMPTYTRANSFER_Z, cmSD_EN, cmTRUE);    //cmSD_EN=14 //cmFALSE = 0 SD 비활성, cmTRUE = 1 SD 활성 	
		COMI.Set_Motor_IO_Property(M_EMPTYTRANSFER_Z, cmSD_LOGIC, cmLOGIC_A); //cmSD_LOGIC=15, 0 (cmLOGIC_A) : A접점 방식,1 (cmLOGIC_B) : B접점 방식
		COMI.Set_Motor_IO_Property(M_EMPTYTRANSFER_Z, cmSD_LATCH, cmFALSE);//16
		//0 (cmFALSE) : SD가 ON되어 감속중이거나 초기속도로 운전중일 때 SD신호가 다시 OFF상태로 변경되면 작업속도까지 다시 가속됩니다
		//1 (cmTRUE) : SD가 ON상태에서 OFF상태로 바뀌어도 작업속도로 가속하지 않습니다.																						
		COMI.Set_Motor_IO_Property(M_EMPTYTRANSFER_Z, cmSD_MODE, cmTRUE);//17
		//0 : SD신호가 ON되면 초기속도까지 감속합니다(정지(停止)하지 않음).
		//1 : SD신호가 ON되면 감속 후 정지(停止)합니다.

		COMI.Set_Motor_IO_Property(M_ULDTRANSFER_Z, cmSD_EN, cmTRUE);    //cmSD_EN=14 //cmFALSE = 0 SD 비활성, cmTRUE = 1 SD 활성 	
		COMI.Set_Motor_IO_Property(M_ULDTRANSFER_Z, cmSD_LOGIC, cmLOGIC_A); //cmSD_LOGIC=15, 0 (cmLOGIC_A) : A접점 방식,1 (cmLOGIC_B) : B접점 방식
		COMI.Set_Motor_IO_Property(M_ULDTRANSFER_Z, cmSD_LATCH, cmFALSE);//16
		//0 (cmFALSE) : SD가 ON되어 감속중이거나 초기속도로 운전중일 때 SD신호가 다시 OFF상태로 변경되면 작업속도까지 다시 가속됩니다
		//1 (cmTRUE) : SD가 ON상태에서 OFF상태로 바뀌어도 작업속도로 가속하지 않습니다.																						
		COMI.Set_Motor_IO_Property(M_ULDTRANSFER_Z, cmSD_MODE, cmTRUE);//17
		//0 : SD신호가 ON되면 초기속도까지 감속합니다(정지(停止)하지 않음).
		//1 : SD신호가 ON되면 감속 후 정지(停止)합니다.
		*/

		/*
		//ex)
		COMI.mn_homecheck_method[0] = 6;			// - Limit Sensor 감지
		COMI.mn_homecheck_method[1] = 6;			// - Limit Sensor 감지
		COMI.mn_homecheck_method[2] = 6;			// - Limit Sensor 감지
		COMI.mn_homecheck_method[3] = 1;			// - Limit Sensor 감지
		COMI.mn_homecheck_method[4] = 1;			// org Sensor 감지
		*/

		/*
		//예) 모든 모터의 리미트 위치를 셋팅한다 
		st_motor[M_DCLDRBT_Y].d_limit_position[0] = -1;
		st_motor[M_DCLDRBT_Y].d_limit_position[1] = 878; //805;

		st_motor[M_DCLDRBT_X].d_limit_position[0] = -1;
		st_motor[M_DCLDRBT_X].d_limit_position[1] = 300; //273;

		st_motor[M_DCLDRBT_Z].d_limit_position[0] = -1;
		st_motor[M_DCLDRBT_Z].d_limit_position[1] = 78;

		st_motor[M_DCLDRBT_P_X].d_limit_position[0] = -1;
		st_motor[M_DCLDRBT_P_X].d_limit_position[1] = 66;//65;

		st_motor[M_DCLDRBT_P_Y].d_limit_position[0] = -1;
		st_motor[M_DCLDRBT_P_Y].d_limit_position[1] = 35;  

		st_motor[M_DCULDRBT_Y].d_limit_position[0] = -1;
		st_motor[M_DCULDRBT_Y].d_limit_position[1] = 590;		 

		for (i = 0; i < COMI.mn_totalmotoraxis_number; i++) //2011.0126 Inposition를 default로 기능 추가 
		{//홈 체크시 등 작업자 사용시 문제가 되니 스킵한다 
			cmmCfgSetSoftLimit(i, TRUE, st_motor[i].d_limit_position[0], st_motor[i].d_limit_position[1]);
		}*/	 

	}

	return  BD_GOOD;
}

//아래 내용은 I/O 보드 초기화시 필요한 내용을 예로 들은것이니 
//장비별 필요한 내용을 셋팅하여 사용하면 된다.
int CCtlBd_Library::Initialize_io_Board(int n_bd_type)
{
	//if(n_bd_type == 0) //파스텍 HSSI I/O board 
	//{
	//}
	int i=0,j=0;
	INT HSSI_speed = 0;
	int Ret = 0;
	INT pMasterNo;
	

	/////////////////초기에 초기화시 //////////////////////////////////////////////////////////////////////
	Ret = FAS_IO.Set_IO_BoardOpen(&pMasterNo/*, START_NOTHING*/); //START_LOADDATA); //START_NOTHING); //START_CLEAR);
	
	if (Ret == FALSE)
	{
		return BD_ERROR;
	}

	/* 예) 아래 내용은 장비별 내용이 틀리니 참고하여 사용하면 된다 
	Ret = FAS_IO.Set_IO_PortAutoEnable(0, 0);	
	if (Ret != BD_GOOD)
	{
		sprintf(mc_normal_msg,"0 Master, 0 Port, not enable");
	
		FAS_IO.mn_io_board_initial_flag = BD_NO;
		return BD_ERROR;
	}

	Ret = FAS_IO.Set_IO_PortAutoEnable(0, 1);
	if (Ret != BD_GOOD)
	{
		sprintf(mc_normal_msg,"0 Master, 1 Port, not enable");
	
		FAS_IO.mn_io_board_initial_flag = BD_NO;
		return BD_ERROR;
	}

	Ret = FAS_IO.Set_IO_PortAutoEnable(0, 2);
	
	if (Ret != CTLBD_RET_GOOD)
	{
		sprintf(mc_normal_msg,"0 Master, 2 Port, not enable");
		
		FAS_IO.mn_io_board_initial_flag = BD_NO;
		return BD_ERROR;
	}


	for (i = 0; i < 8; i++)
	{
		Ret = FAS_IO.Set_IO_SlaveEnable(0, 0, i, TRUE);
		
		if (Ret != CTLBD_RET_GOOD)
		{
			sprintf(mc_normal_msg,"Slave #%d, not enable", i);
				
			i = -1;
			
			if (j < 3)
			{
				j++;
			}
			else
			{
				FAS_IO.mn_io_board_initial_flag = BD_NO;
				return BD_ERROR;
			}
			continue;
		}
		
		sprintf(mc_normal_msg,"Slave #%d, enable success", i);

		Sleep(100);
	} 

	for (i = 0; i < 4; i++)
	{
		Ret = FAS_IO.Set_IO_HSSISpeed(0, i, PORT_SPEED_10);
		Ret = FAS_IO.Get_IO_HSSISpeed(0, i, &HSSI_speed);

		if (i <= 3)
		{
			Ret = FAS_IO.Set_IO_HSSISpeed(1, i, PORT_SPEED_10);
			Ret = FAS_IO.Get_IO_HSSISpeed(1, i, &HSSI_speed);
		}
	}

	FAS_IO.Set_IO_DefineWord(0, 0, 0, 0xFF00); // 0번 Master, 0번 포트, 0번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 0, 1, 0xFF00); // 0번 Master, 0번 포트, 1번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 0, 2, 0x0000); // 0번 Master, 0번 포트, 2번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 0, 3, 0x000f); // 0번 Master, 0번 포트, 3번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 0, 4, 0x000f); // 0번 Master, 0번 포트, 4번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 0, 5, 0x000f); // 0번 Master, 0번 포트, 5번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 0, 6, 0x000f); // 0번 Master, 0번 포트, 6번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 0, 7, 0x006f); // 0번 Master, 0번 포트, 7번 슬레이브 , 16bit 셋팅

	FAS_IO.Set_IO_DefineWord(0, 1, 0, 0xffff); // 0번 Master, 1번 포트, 0번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 1, 1, 0xffff); // 0번 Master, 1번 포트, 1번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 1, 2, 0x0000); // 0번 Master, 1번 포트, 2번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 1, 3, 0xffff); // 0번 Master, 1번 포트, 3번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 1, 4, 0xffff); // 0번 Master, 1번 포트, 4번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 1, 5, 0xffff); // 0번 Master, 1번 포트, 5번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 1, 6, 0x0000); // 0번 Master, 1번 포트, 6번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 1, 7, 0xffff); // 0번 Master, 1번 포트, 7번 슬레이브 , 16bit 셋팅

	FAS_IO.Set_IO_DefineWord(0, 2, 0, 0xffff); // 0번 Master, 2번 포트, 0번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 2, 1, 0x00ff); // 0번 Master, 2번 포트, 1번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 2, 2, 0xffff); // 0번 Master, 2번 포트, 2번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 2, 3, 0xffff); // 0번 Master, 2번 포트, 3번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 2, 4, 0xffff); // 0번 Master, 2번 포트, 4번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 2, 5, 0xffff); // 0번 Master, 2번 포트, 5번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 2, 6, 0x00ff); // 0번 Master, 2번 포트, 6번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 2, 7, 0xffff); // 0번 Master, 2번 포트, 7번 슬레이브 , 16bit 셋팅

	FAS_IO.Set_IO_DefineWord(0, 3, 0, 0xffff); // 0번 Master, 3번 포트, 0번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 3, 1, 0xffff); // 0번 Master, 3번 포트, 1번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 3, 2, 0xffff); // 0번 Master, 3번 포트, 2번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 3, 3, 0x00ff); // 0번 Master, 3번 포트, 3번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 3, 4, 0xffff); // 0번 Master, 3번 포트, 4번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 3, 5, 0xffff); // 0번 Master, 3번 포트, 5번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 3, 6, 0x0000); // 0번 Master, 3번 포트, 6번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 3, 7, 0xffff); // 0번 Master, 3번 포트, 7번 슬레이브 , 16bit 셋팅

	FAS_IO.Set_IO_DefineWord(1, 0, 0, 0x003f); // 1번 Master, 0번 포트, 0번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(1, 0, 1, 0x00ff); // 1번 Master, 0번 포트, 1번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(1, 0, 2, 0x001f); // 1번 Master, 0번 포트, 2번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(1, 0, 3, 0x001f); // 1번 Master, 0번 포트, 3번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(1, 0, 4, 0x001f); // 1번 Master, 0번 포트, 4번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(1, 0, 5, 0x003f); // 1번 Master, 0번 포트, 5번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(1, 0, 6, 0x00ff); // 1번 Master, 0번 포트, 6번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(1, 0, 7, 0x00ff); // 1번 Master, 0번 포트, 7번 슬레이브 , 16bit 셋팅
	
	FAS_IO.Set_IO_DefineWord(1, 1, 0, 0x001f); // 1번 Master, 1번 포트, 0번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(1, 1, 1, 0x001f); // 1번 Master, 1번 포트, 1번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(1, 1, 2, 0x001f); // 1번 Master, 1번 포트, 2번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(1, 1, 3, 0x001f); // 1번 Master, 1번 포트, 3번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(1, 1, 4, 0x000f); // 1번 Master, 1번 포트, 4번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(1, 1, 5, 0x001f); // 1번 Master, 1번 포트, 5번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(1, 1, 6, 0x000f); // 1번 Master, 1번 포트, 6번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(1, 1, 7, 0x00ff); // 1번 Master, 1번 포트, 7번 슬레이브 , 16bit 셋팅
	
	FAS_IO.Set_IO_DefineWord(1, 2, 0, 0x00ff); // 1번 Master, 2번 포트, 0번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(1, 2, 1, 0x00ff); // 1번 Master, 2번 포트, 1번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(1, 2, 2, 0x00ff); // 1번 Master, 2번 포트, 2번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(1, 2, 3, 0x0000); // 1번 Master, 2번 포트, 3번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(1, 2, 4, 0x0000); // 1번 Master, 2번 포트, 4번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(1, 2, 5, 0x0000); // 1번 Master, 2번 포트, 5번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(1, 2, 6, 0x0000); // 1번 Master, 2번 포트, 6번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(1, 2, 7, 0x0000); // 1번 Master, 2번 포트, 7번 슬레이브 , 16bit 셋팅
	*/

	FAS_IO.m_nIoBoardInitialFlag = BD_YES;
	
	return BD_GOOD;
}


int CCtlBd_Library::Motor_SafetyCheck(int n_mode, int n_axis, double d_targetpos) 	//모터 안전 정보 셋팅시 사용 
{
	//안전 관련하여는 참고하여 장비별 적용한다 

//	char cJamcode[10];
	int nRet = 0, nRet_1 = 0, nRet_2 = 0, nRet_3=0;
	double d_CurPos[30] = {0,};
	double d_Pos[30] = {0,};
	double d_GapCheck = 0;
 

	int i = 0;
	int nCheck = 0;
//	int nIO[10];
	int nTime = 5;
	int nIO[10];

	CString strMsg;
	CString strTemp;  

//	BOOL bFlag;
	// **************************************************************************
	// 모터 알람 상태 검사한다                                                   
	// -> 알람 발생한 경우 알람 해제한다                                         
	// **************************************************************************

	if(st_handler_info.nRunStatus == dRUN || st_handler_info.nRunStatus == dINIT ||
	   st_handler_info.nRunStatus == dSTOP)
	{
		if (n_mode == 10)
		{
			nRet = BD_GOOD;
		}
		else
		{
			nRet = clsFunc.OnDoorOpenCheck();
		}
		
		if(nRet == BD_ERROR)
		{
			return BD_ERROR;
		}

		nRet = clsFunc.OnLightCurtainCheck();

		if (nRet == BD_ERROR)
		{
			if (m_nMcEmg == NO)
			{
				m_nMcEmg = YES;
			}

			return BD_ERROR;
		}

		nRet = clsFunc.OnMainAirCheck();

		if (nRet == BD_ERROR)
		{
			return BD_ERROR;
		}

		nRet = clsFunc.OnEmoCheck();

		if (nRet == RET_ERROR)
		{
			if (m_nMcEmg == NO)
			{
				m_nMcEmg = YES;
			}

			return BD_ERROR;
		}

		nRet = clsFunc.OnMotorPowerCheck();

		if (nRet == RET_ERROR)
		{
			if (m_nMcEmg == NO)
			{
				m_nMcEmg = YES;
			}

			return BD_ERROR;
		}
		/*
		if (clsFunc.OnMpCheck() == RET_ERROR)
		{
			return BD_ERROR;
		}*/
	}

	// **************************************************************************
	// 모터 파워 상태 검사한다                                                   
	// -> 모터 POWER OFF 시 POWER ON 상태로 만든다                               
	// **************************************************************************
	if (COMI.Get_MotPower(n_axis) == BD_ERROR )          // 모터 POWER 상태 검사 함수 
	{
//		if (COMI.Set_MotPower(n_axis, NO) == CTLBD_RET_GOOD)       // 모터 POWER ON 설정 함수 
		if (COMI.Set_MotPower(n_axis, TRUE) == CTLBD_RET_GOOD)       // 모터 POWER ON 설정 함수 
		{
			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
				// 20120502 jtkim
				//wsprintfA(st_other_info.cNormalMsg, "%S", _T("[Motor_Power Reset_1] Axis=%d, rcnt=%d"), n_axis, st_motor_info[n_axis].n_retry_cnt);
				strMsg.Format(_T("[Motor_Power Reset_1] Axis=%d, rcnt=%d"), n_axis, st_motor_info[n_axis].n_retry_cnt);
				//clsFunc.OnStringToChar(strMsg, st_other_info.cNormalMsg);
				clsMem.OnNormalMessageWrite(strMsg);
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
			}

			if (COMI.Get_MotPower(n_axis) == BD_ERROR)  // 모터 POWER 상태 검사 함수 
			{
				if (st_motor_info[n_axis].n_retry_cnt > (MOT_RTY_CNT))
				{
					st_alarm_info.strCode.Format(_T("%02d0005"),  n_axis);
					st_alarm_info.nCountMode	= 0;	
					st_alarm_info.nTypeMode		= eWARNING;
					st_alarm_info.nAlarmAssignSection = 22;

					//2011.0210 st_motor[n_axis].n_retry_cnt = 0;
					return BD_ERROR;
				}
				else
				{
					st_motor_info[n_axis].n_retry_cnt++ ;
					return BD_RETRY;
				}
			}
		}
		else
		{
			if (st_motor_info[n_axis].n_retry_cnt > MOT_RTY_CNT)
			{
				st_alarm_info.strCode.Format(_T("%02d0005"),  n_axis);
				st_alarm_info.nCountMode	= 0;	
				st_alarm_info.nTypeMode		= eWARNING;
				st_alarm_info.nAlarmAssignSection = 23;

				return BD_ERROR;
			}
			else
			{
				st_motor_info[n_axis].n_retry_cnt++ ;
				return BD_RETRY;
			}
		}
	}

	//모터 알람 발생시 처리 조건 
	if (COMI.Get_MotAlarmStatus(n_axis) == BD_ERROR)      // 모터 ALARM 상태 검사 함수 
	{
	
		if (COMI.Set_MotAlarmClear(n_axis) == CTL_GOOD)       // 모터 ALARM CLEAR 함수 
		{
			//091119 james			Sleep(1000);  // 일정 시간 후에 상태 확인하기 위해 SLEEP 사용한다 
			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
				//wsprintfA(st_other_info.cNormalMsg, "%S", _T("[Alarm Reset] Axis=%d, r_cnt=%d"), n_axis, st_motor_info[n_axis].n_retry_cnt);
				strMsg.Format(_T("[Alarm Reset] Axis=%d, r_cnt=%d"), n_axis, st_motor_info[n_axis].n_retry_cnt);
				//clsFunc.OnStringToChar(strMsg, st_other_info.cNormalMsg);
				clsMem.OnNormalMessageWrite(strMsg);
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
			}

			if (COMI.Get_MotAlarmStatus(n_axis) == BD_ERROR )  // 모터 ALARM 상태 검사 함수 
			{
				if (st_motor_info[n_axis].n_retry_cnt > MOT_RTY_CNT)
				{			
					st_alarm_info.strCode.Format(_T("%02d0002"),  n_axis);
					st_alarm_info.nCountMode	= 0;	
					st_alarm_info.nTypeMode		= eWARNING;
					st_alarm_info.nAlarmAssignSection = 24;

					//2011.0210 st_motor[n_axis].n_retry_cnt = 0;  // 알람 해제 시도 횟수 
					return BD_ERROR;
				}
				else
				{
					st_motor_info[n_axis].n_retry_cnt++ ;
					return BD_RETRY;
				}
			}
		}
		else
		{
			if (st_motor_info[n_axis].n_retry_cnt > MOT_RTY_CNT)
			{
				st_alarm_info.strCode.Format(_T("%02d0002"),  n_axis);
				st_alarm_info.nCountMode	= 0;	
				st_alarm_info.nTypeMode		= eWARNING;
				st_alarm_info.nAlarmAssignSection = 25;
				//2011.0210 st_motor[n_axis].n_retry_cnt = 0;  // 알람 해제 시도 횟수 
				return BD_ERROR;
			}
			else
			{
				st_motor_info[n_axis].n_retry_cnt++ ;
				return BD_RETRY;
			}
		}
	}

	d_CurPos[0] = COMI.Get_MotCurrentPos(M_WORK_RBT_X);
	d_CurPos[1] = COMI.Get_MotCurrentPos(M_WORK_RBT_Y);
	d_CurPos[2] = COMI.Get_MotCurrentPos(M_WORK_RBT_Z);
	d_CurPos[3] = COMI.Get_MotCurrentPos(M_WORKTRAY_TRANSFER);

	d_CurPos[10] = COMI.Get_MotCurrentPos(M_TEST_RBT_X);
	d_CurPos[11] = COMI.Get_MotCurrentPos(M_TEST_RBT_Y);
	d_CurPos[12] = COMI.Get_MotCurrentPos(M_TEST_RBT_Z);

	switch (n_axis)
	{

		// 0번째 보드(8축)	
	case M_LD_CV_LR_SHIFTER:
		nIO[0] = FAS_IO.get_in_bit_wait(0, st_io_info.i_ReadyCvyLd12OutTrayPassChk,	IO_OFF, nTime);
		nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ReadyCvyLd12OutTrayPassChk,	IO_OFF); //CV tray pass check  없어야 한다 
		if(nRet_1 == IO_ON)
		{	//트레이 존재 에러 
			if(nRet_1 == IO_ON) st_alarm_info.strCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_ReadyCvyLd12OutTrayPassChk); 
			CTL_Lib.Alarm_Error_Occurrence(100, dWARNING, st_alarm_info.strCode);
			return BD_ERROR;
		}
		break;

	case M_LD_ELV:
		nIO[0] = FAS_IO.get_in_bit_wait(0, st_io_info.i_ReadyCvyLd12OutTrayPassChk,	IO_OFF, nTime);
		nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ReadyCvyLd12OutTrayPassChk,	IO_OFF); //CV tray pass check  없어야 한다 
		if(nRet_1 == IO_ON)
		{	//트레이 존재 에러 
			if(nRet_1 == IO_ON) st_alarm_info.strCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_ReadyCvyLd12OutTrayPassChk); 
			CTL_Lib.Alarm_Error_Occurrence(100, dWARNING, st_alarm_info.strCode);
			return BD_ERROR;
		}
		if(d_CurPos[M_LD_ELV] > st_motor_info[M_LD_ELV].d_pos[P_ELV_TRAY_INOUT_POS] + st_motor_info[M_LD_ELV].d_allow &&
			d_targetpos < st_motor_info[M_LD_ELV].d_pos[P_ELV_TRAY_INOUT_POS] + st_motor_info[M_LD_ELV].d_allow )
		{
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayArrivedChk,	IO_OFF); //엘리베이터가 내려 올때 LD ELV arrived tray  없어야 한다 
			if(nRet_2 == IO_ON)
			{	//트레이 존재 에러 
//				if(nRet_1 == IO_ON) st_alarm_info.strCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_LdStkTrayArrivedChk); 
//				CTL_Lib.Alarm_Error_Occurrence(101, dWARNING, st_alarm_info.strCode);
//				return BD_ERROR;
			}
		}
		break;

	case M_EMPTY_ELV:
	case M_ULD_1_ELV:
	case M_ULD_2_ELV:
		break;
	case M_RJ_EMPTY_ELV:
		if(d_CurPos[M_RJ_EMPTY_ELV] > st_motor_info[M_RJ_EMPTY_ELV].d_pos[P_ELV_TRAY_INOUT_POS] + st_motor_info[M_RJ_EMPTY_ELV].d_allow &&
			d_targetpos < st_motor_info[M_RJ_EMPTY_ELV].d_pos[P_ELV_TRAY_INOUT_POS] + st_motor_info[M_RJ_EMPTY_ELV].d_allow )
		{
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_RejEmptyTrayArrivedChk,	IO_OFF); //엘리베이터가 내려 올때 LD ELV arrived tray  없어야 한다 
			if(nRet_1 == IO_ON)
			{	//트레이 존재 에러 
				if(nRet_1 == IO_ON) st_alarm_info.strCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_RejEmptyTrayArrivedChk); 
				CTL_Lib.Alarm_Error_Occurrence(101, dWARNING, st_alarm_info.strCode);
				return BD_ERROR;
			}
		}
		break;
	case M_RJ_ULD_1_ELV:
	case M_RJ_ULD_2_ELV:
		break;
		// 1번째 보드(8축)	
	case M_WORK_RBT_X: 
		if( d_CurPos[0] > st_motor_info[M_WORK_RBT_X].d_pos[P_WORKROBOT_BCR_2D_START_LD] - st_motor_info[M_WORK_RBT_X].d_allow &&
			d_CurPos[0] < st_motor_info[M_WORK_RBT_X].d_pos[P_WORKROBOT_BCR_2D_START_LD] + st_motor_info[M_WORK_RBT_X].d_allow &&
			d_CurPos[2] > st_motor_info[M_WORK_RBT_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[M_WORK_RBT_Z].d_allow)
		{
			if( d_targetpos > st_motor_info[M_WORK_RBT_X].d_pos[P_WORKROBOT_BCR_2D_START_LD] - st_motor_info[M_WORK_RBT_X].d_allow &&
				d_targetpos < st_motor_info[M_WORK_RBT_X].d_pos[P_WORKROBOT_BCR_2D_START_LD] + st_motor_info[M_WORK_RBT_X].d_allow )
			{
				if(d_CurPos[2] > st_motor_info[M_WORK_RBT_Z].d_pos[P_WORKROBOT_Z_2D_LD_POS] + st_motor_info[M_WORK_RBT_Z].d_allow)
				{
					m_strMotorAlarmCode.Format(_T("%02d0008"),M_WORK_RBT_Z ); 
					CTL_Lib.Alarm_Error_Occurrence(100, dMOTOR_WARNING, m_strMotorAlarmCode);  
					return BD_ERROR;
				}
			}
			else
			{
				m_strMotorAlarmCode.Format(_T("%02d0008"),M_WORK_RBT_Z ); 
				CTL_Lib.Alarm_Error_Occurrence(100, dMOTOR_WARNING, m_strMotorAlarmCode);  
				return BD_ERROR;
			}
		}
		else if(d_CurPos[2] > st_motor_info[M_WORK_RBT_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[M_WORK_RBT_Z].d_allow)
		{
			m_strMotorAlarmCode.Format(_T("%02d0008"),M_WORK_RBT_Z ); 
			CTL_Lib.Alarm_Error_Occurrence(100, dMOTOR_WARNING, m_strMotorAlarmCode);  
			return BD_ERROR;
		}
		break;

	case M_WORK_RBT_Y: 
		//2016.1023
		if( d_targetpos > ( st_motor_info[M_WORK_RBT_Y].d_pos[P_WORKROBOT_BCR_2D_START_LD] - (3 * st_recipe_info.dLdUldBufferPitch) - (4*st_motor_info[M_WORK_RBT_Z].d_allow) ) &&
			d_targetpos < ( st_motor_info[M_WORK_RBT_Y].d_pos[P_WORKROBOT_BCR_2D_START_LD] + (5*st_motor_info[M_WORK_RBT_Z].d_allow) ) &&
			(d_CurPos[2] > st_motor_info[M_WORK_RBT_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[M_WORK_RBT_Z].d_allow) )
		{
			nRet_1 = FAS_IO.get_out_bit(st_io_info.o_LdUldPickGripCyl,	IO_ON);
			if( nRet_1 != IO_ON)
			{				
				m_strMotorAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.o_LdUldPickGripCyl); 
				CTL_Lib.Alarm_Error_Occurrence(91, dMOTOR_WARNING, m_strMotorAlarmCode);  
				return BD_ERROR;
			}
			if( d_CurPos[0] > st_motor_info[M_WORK_RBT_X].d_pos[P_WORKROBOT_BCR_2D_START_LD] + st_motor_info[M_WORK_RBT_X].d_allow ||
				d_CurPos[0] < st_motor_info[M_WORK_RBT_X].d_pos[P_WORKROBOT_BCR_2D_START_LD] - st_motor_info[M_WORK_RBT_X].d_allow)
			{
				m_strMotorAlarmCode.Format(_T("%02d0008"),M_WORK_RBT_X ); 
				CTL_Lib.Alarm_Error_Occurrence(100, dMOTOR_WARNING, m_strMotorAlarmCode);  
				return BD_ERROR;
			}
			if(d_CurPos[2] > st_motor_info[M_WORK_RBT_Z].d_pos[P_WORKROBOT_Z_2D_LD_POS] + st_motor_info[M_WORK_RBT_Z].d_allow)
			{
				m_strMotorAlarmCode.Format(_T("%02d0008"),M_WORK_RBT_Z ); 
				CTL_Lib.Alarm_Error_Occurrence(100, dMOTOR_WARNING, m_strMotorAlarmCode);  
				return BD_ERROR;
			}
		}
		else if(d_CurPos[2] > st_motor_info[M_WORK_RBT_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[M_WORK_RBT_Z].d_allow)
		{
			m_strMotorAlarmCode.Format(_T("%02d0008"),M_WORK_RBT_Z ); 
			CTL_Lib.Alarm_Error_Occurrence(100, dMOTOR_WARNING, m_strMotorAlarmCode);  
			return BD_ERROR;
		}
		break;

	case M_WORK_RBT_Z:
// 		if( d_targetpos == st_motor_info[M_WORK_RBT_Z].d_pos[P_WORKROBOT_Z_ULDTRAY_1_PLACE] ||
// 			d_targetpos == st_motor_info[M_WORK_RBT_Z].d_pos[P_WORKROBOT_Z_ULDTRAY_2_PLACE] ||
// 			d_targetpos == st_motor_info[M_WORK_RBT_Z].d_pos[P_WORKROBOT_Z_LDTRAY_PICK] || 
// 			d_targetpos == st_motor_info[M_WORK_RBT_Z].d_pos[P_WORKROBOT_Z_LDTRAY_PLACE] )
// 		{
// 			if()
// 			m_strMotorAlarmCode.Format(_T("%02d0008"),M_WORK_RBT_Z ); 
// 			CTL_Lib.Alarm_Error_Occurrence(101, dMOTOR_WARNING, m_strMotorAlarmCode);  
// 			return BD_ERROR;
// 
// 		}
// 		if( d_targetpos == st_motor_info[M_WORK_RBT_Z].d_pos[P_WORKROBOT_Z_REJECTTRAY_1_PLACE] ||
// 			d_targetpos == st_motor_info[M_WORK_RBT_Z].d_pos[P_WORKROBOT_Z_REJECTTRAY_2_PLACE] )
// 		{
// 			m_strMotorAlarmCode.Format(_T("%02d0008"),M_WORK_RBT_Z ); 
// 			CTL_Lib.Alarm_Error_Occurrence(102, dMOTOR_WARNING, m_strMotorAlarmCode);  
// 			return BD_ERROR;
// 
// 		}
		//kwlee 2017.0203
		if(st_recipe_info.nTrayY <= 1)
		{
			m_strMotorAlarmCode.Format(_T("%02d0008"),M_WORK_RBT_Z ); 
			CTL_Lib.Alarm_Error_Occurrence(19000, dMOTOR_WARNING, m_strMotorAlarmCode);
			strMsg.Format(_T("TrayY 수가 1보다 적을 수 없습니다."));
			clsMem.OnNormalMessageWrite(strMsg);
			return BD_ERROR;
		}
		if (d_CurPos[0] <= (st_motor_info[M_WORK_RBT_X].d_pos[P_WORKROBOT_ULDTRAY_2_START] + 50) &&
			(d_CurPos[1] <= st_motor_info[M_WORK_RBT_Y].d_pos[P_WORKROBOT_LDTRAY_START] + ( (st_recipe_info.nTrayY-1) * st_recipe_info.dTrayPitch_Y) + 50) )
		{
			if ( d_CurPos[0] >= (st_motor_info[M_WORK_RBT_X].d_pos[P_WORKROBOT_LDTRAY_START] - 50) && d_CurPos[0] <= (st_motor_info[M_WORK_RBT_X].d_pos[P_WORKROBOT_LDTRAY_START] + 50 ) )
			{
				if (d_CurPos[2] >= (st_motor_info[M_WORK_RBT_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[M_WORK_RBT_Z].d_allow))
				{
					if(d_CurPos[3] <= (st_motor_info[M_WORKTRAY_TRANSFER].d_pos[P_WORKTRANS_EMPTY_STACKER] - 10* st_motor_info[M_WORK_RBT_Z].d_allow))
					{
						m_strMotorAlarmCode.Format(_T("%02d0008"),M_WORK_RBT_Z ); 
						CTL_Lib.Alarm_Error_Occurrence(10000, dMOTOR_WARNING, m_strMotorAlarmCode);  //Work Robot이 Ld Tray 위치이고 Empty Transfer가 Ld Tray 위치로 이동 하려고 할때 Alarm 
						return BD_ERROR;
					}
				}
			}
			else if (d_CurPos[0] >= (st_motor_info[M_WORK_RBT_X].d_pos[P_WORKROBOT_ULDTRAY_1_START] - 50) && d_CurPos[0] <= (st_motor_info[M_WORK_RBT_X].d_pos[P_WORKROBOT_ULDTRAY_1_START] + 50) )
			{
				if (d_CurPos[2] >= (st_motor_info[M_WORK_RBT_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[M_WORK_RBT_Z].d_allow))
				{
					if(d_CurPos[3] >= (st_motor_info[M_WORKTRAY_TRANSFER].d_pos[P_WORKTRANS_EMPTY_STACKER] + 10 * st_motor_info[M_WORK_RBT_Z].d_allow))
					{
						m_strMotorAlarmCode.Format(_T("%02d0008"),M_WORK_RBT_Z ); 
						CTL_Lib.Alarm_Error_Occurrence(10001, dMOTOR_WARNING, m_strMotorAlarmCode);  //Work Robot이 ULd Tray#1 위치이고 Empty Transfer가 Uld Tray#1 위치로 이동 하려고 할때 Alarm
						return BD_ERROR;
					}
				}
			}
			else if (d_CurPos[0] >= (st_motor_info[M_WORK_RBT_X].d_pos[P_WORKROBOT_ULDTRAY_2_START] - 50) && d_CurPos[0] <= (st_motor_info[M_WORK_RBT_X].d_pos[P_WORKROBOT_ULDTRAY_2_START] + 50) )
			{
				if (d_CurPos[2] >= (st_motor_info[M_WORK_RBT_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[M_WORK_RBT_Z].d_allow))
				{
					if(d_CurPos[3] >= (st_motor_info[M_WORKTRAY_TRANSFER].d_pos[P_WORKTRANS_ULD_1_STACKER] + 10 * st_motor_info[M_WORK_RBT_Z].d_allow))
					{
						m_strMotorAlarmCode.Format(_T("%02d0008"),M_WORK_RBT_Z ); 
						CTL_Lib.Alarm_Error_Occurrence(10002, dMOTOR_WARNING, m_strMotorAlarmCode);  //Work Robot이 ULd Tray#2 위치이고 Empty Transfer가 Uld Tray#2 위치로 이동 하려고 할때 Alarm
						return BD_ERROR;
					}
				}
			}
			else
			{
				if ( d_targetpos > (st_motor_info[M_WORK_RBT_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[M_WORK_RBT_Z].d_allow))
				{
					m_strMotorAlarmCode.Format(_T("%02d0008"),M_WORK_RBT_Z ); 
					CTL_Lib.Alarm_Error_Occurrence(10003, dMOTOR_WARNING, m_strMotorAlarmCode);  
					return BD_ERROR;
				}

			}
		}
		else if (d_CurPos[0] >= (st_motor_info[M_WORK_RBT_X].d_pos[P_WORKROBOT_REJECT_1_START] + 50))
		{
			if ( (d_CurPos[1] >= (st_motor_info[M_WORK_RBT_Y].d_pos[P_WORKROBOT_REJECT_1_START] - 50))  && d_CurPos[1] <= (st_motor_info[M_WORK_RBT_Y].d_pos[P_WORKROBOT_REJECT_1_START] + ( (st_recipe_info.nTrayY-1) * st_recipe_info.dTrayPitch_Y) + 50 ))
			{
				if (d_CurPos[2] > (st_motor_info[M_WORK_RBT_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[M_WORK_RBT_Z].d_allow))
				{
					if(d_CurPos[3] >= st_motor_info[M_REJECTTRAY_TRANSFER].d_pos[P_REJECTTRANS_EMPTY_STACKER] + 10 * st_motor_info[M_WORK_RBT_Z].d_allow)
					{
						m_strMotorAlarmCode.Format(_T("%02d0008"),M_WORK_RBT_Z ); 
						CTL_Lib.Alarm_Error_Occurrence(10004, dMOTOR_WARNING, m_strMotorAlarmCode);  //Work Robot이Reject Tray#1 위치이고 Empty Transfer가 Reject Tray#1 위치로 이동 하려고 할때 Alarm
						return BD_ERROR;
					}
				}
			}
			else if ( ( d_CurPos[1] >= (st_motor_info[M_WORK_RBT_Y].d_pos[P_WORKROBOT_REJECT_2_START] - 50)) && d_CurPos[1] <= (st_motor_info[M_WORK_RBT_Y].d_pos[P_WORKROBOT_REJECT_2_START] + ( (st_recipe_info.nTrayY-1) * st_recipe_info.dTrayPitch_Y) + 50 ))
			{
				if (d_CurPos[2] >= (st_motor_info[M_WORK_RBT_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[M_WORK_RBT_Z].d_allow))
				{
					if(d_CurPos[3] >= (st_motor_info[M_REJECTTRAY_TRANSFER].d_pos[P_REJECTTRANS_REJECT_1_STACKER] + 10 * st_motor_info[M_WORK_RBT_Z].d_allow))
					{
						m_strMotorAlarmCode.Format(_T("%02d0008"),M_WORK_RBT_Z ); 
						CTL_Lib.Alarm_Error_Occurrence(10005, dMOTOR_WARNING, m_strMotorAlarmCode);  //Work Robot이 Reject Tray#2 위치이고 Empty Transfer가 Reject Tray#2 위치로 이동 하려고 할때 Alarm
						return BD_ERROR;
					}
				}
			}
			else
			{
// 				m_strMotorAlarmCode.Format(_T("%02d0008"),M_WORK_RBT_Z ); 
// 				CTL_Lib.Alarm_Error_Occurrence(10006, dMOTOR_WARNING, m_strMotorAlarmCode);  
// 				return BD_ERROR;
				if ( d_targetpos > (st_motor_info[M_WORK_RBT_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[M_WORK_RBT_Z].d_allow))
				{
					m_strMotorAlarmCode.Format(_T("%02d0008"),M_WORK_RBT_Z ); 
					CTL_Lib.Alarm_Error_Occurrence(10003, dMOTOR_WARNING, m_strMotorAlarmCode);  
					return BD_ERROR;
				}
			}
		}
		////
		break;

	case M_LD_BUF_SHIFTER_Y:	
	case M_LD_BUF_ROTATOR:
		if(d_CurPos[2] > st_motor_info[M_WORK_RBT_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[M_WORK_RBT_Z].d_allow)
		{
			nRet_1 = COMI.Check_MotPosRange(M_WORK_RBT_X, st_motor_info[M_WORK_RBT_X].d_pos[P_WORKROBOT_LD_BUFF_START], 50);
			nRet_2 = COMI.Check_MotPosRange(M_WORK_RBT_Y, st_motor_info[M_WORK_RBT_Y].d_pos[P_WORKROBOT_LD_BUFF_START], 50);

			if (nRet_1 == BD_GOOD && nRet_2 == BD_GOOD )  // 이동할 위치에 이미 존재함
			{
				m_strMotorAlarmCode.Format(_T("%02d0008"),M_WORK_RBT_Z ); 
				CTL_Lib.Alarm_Error_Occurrence(100, dMOTOR_WARNING, m_strMotorAlarmCode);  
				return BD_ERROR;
			}
		}

		if(d_CurPos[12] > st_motor_info[M_TEST_RBT_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[M_TEST_RBT_Z].d_allow)
		{
			nRet_1 = COMI.Check_MotPosRange(M_TEST_RBT_X, st_motor_info[M_TEST_RBT_X].d_pos[P_TESTRBT_XY_LDBUF_START], 50);
			nRet_2 = COMI.Check_MotPosRange(M_TEST_RBT_Y, st_motor_info[M_TEST_RBT_Y].d_pos[P_TESTRBT_XY_LDBUF_START], 50);

			if (nRet_1 == BD_GOOD && nRet_2 == BD_GOOD )  // 이동할 위치에 이미 존재함
			{
				m_strMotorAlarmCode.Format(_T("%02d0008"),M_TEST_RBT_Z ); 
				CTL_Lib.Alarm_Error_Occurrence(100, dMOTOR_WARNING, m_strMotorAlarmCode);  
				return BD_ERROR;
			}
		} 
		break;	

	case M_ULD_BUF_SHIFTER_Y:
	case M_ULD_BUF_ROTATOR:
		if(d_CurPos[2] > st_motor_info[M_WORK_RBT_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[M_WORK_RBT_Z].d_allow)
		{
			nRet_1 = COMI.Check_MotPosRange(M_WORK_RBT_X, st_motor_info[M_WORK_RBT_X].d_pos[P_WORKROBOT_ULD_BUFF_START], 50);
			nRet_2 = COMI.Check_MotPosRange(M_WORK_RBT_Y, st_motor_info[M_WORK_RBT_Y].d_pos[P_WORKROBOT_ULD_BUFF_START], 50);

			if (nRet_1 == BD_GOOD && nRet_2 == BD_GOOD )  // 이동할 위치에 이미 존재함
			{
				m_strMotorAlarmCode.Format(_T("%02d0008"),M_WORK_RBT_Z ); 
				CTL_Lib.Alarm_Error_Occurrence(100, dMOTOR_WARNING, m_strMotorAlarmCode);  
				return BD_ERROR;
			}
		}
		if(d_CurPos[12] > st_motor_info[M_TEST_RBT_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[M_TEST_RBT_Z].d_allow)
		{
			nRet_1 = COMI.Check_MotPosRange(M_TEST_RBT_X, st_motor_info[M_TEST_RBT_X].d_pos[P_TESTRBT_XY_ULDBUF_START], 50);
			nRet_2 = COMI.Check_MotPosRange(M_TEST_RBT_Y, st_motor_info[M_TEST_RBT_Y].d_pos[P_TESTRBT_XY_ULDBUF_START], 50);

			if (nRet_1 == BD_GOOD && nRet_2 == BD_GOOD )  // 이동할 위치에 이미 존재함
			{
				m_strMotorAlarmCode.Format(_T("%02d0008"),M_TEST_RBT_Z ); 
				CTL_Lib.Alarm_Error_Occurrence(100, dMOTOR_WARNING, m_strMotorAlarmCode);  
				return BD_ERROR;
			}
		}
		break;


	case M_SPARE_1:
		// 2번째 보드(8축)	
	case M_RETEST_BUF_1_ROTATOR:
	case M_RETEST_BUF_2_ROTATOR:
		if(d_CurPos[12] > st_motor_info[M_TEST_RBT_Z].d_pos[P_TESTRBT_Z_SAFETY] + st_motor_info[M_TEST_RBT_Z].d_allow)
		{
			nRet_1 = COMI.Check_MotPosRange(M_TEST_RBT_X, st_motor_info[M_TEST_RBT_X].d_pos[P_TESTRBT_XY_RETESTBUF_1_START], 600);
			nRet_2 = COMI.Check_MotPosRange(M_TEST_RBT_Y, st_motor_info[M_TEST_RBT_Y].d_pos[P_TESTRBT_XY_RETESTBUF_1_START], 600);

			if (nRet_1 == BD_GOOD && nRet_2 == BD_GOOD )  // 이동할 위치에 이미 존재함
			{
				m_strMotorAlarmCode.Format(_T("%02d0008"),M_TEST_RBT_Z ); 
				CTL_Lib.Alarm_Error_Occurrence(100, dMOTOR_WARNING, m_strMotorAlarmCode);  
				return BD_ERROR;
			}
		}
		break;


	case M_WORKTRAY_TRANSFER:
		nIO[0] = FAS_IO.get_in_bit_wait(0, st_io_info.i_TrayTrsfUpChk,	IO_ON, nTime);
		nIO[1] = FAS_IO.get_in_bit_wait(0, st_io_info.i_TrayTrsfDnChk,	IO_OFF, nTime);		 
		if(nIO[0] == IO_OFF || nIO[1] == IO_ON)
		{	//트레이 존재 에러 
			if(nRet_1 == IO_ON) st_alarm_info.strCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_TrayTrsfUpChk); 
			CTL_Lib.Alarm_Error_Occurrence(100, dWARNING, st_alarm_info.strCode);
			return BD_ERROR;
		}
		break;

	case M_REJECTTRAY_TRANSFER:
		nIO[0] = FAS_IO.get_in_bit_wait(0, st_io_info.i_RejTrsfUpChk,	IO_ON, nTime);
		nIO[1] = FAS_IO.get_in_bit_wait(0, st_io_info.i_RejTrsfDnChk,	IO_OFF, nTime);		 
		if(nIO[0] == IO_OFF || nIO[1] == IO_ON)
		{	//트레이 존재 에러 
			if(nRet_1 == IO_ON) st_alarm_info.strCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_RejTrsfUpChk); 
			CTL_Lib.Alarm_Error_Occurrence(100, dWARNING, st_alarm_info.strCode);
			return BD_ERROR;
		}
		break;

	case M_TEST_RBT_X:
	case M_TEST_RBT_Y:
		if(d_CurPos[12] > st_motor_info[M_TEST_RBT_Z].d_pos[P_TESTRBT_Z_SAFETY] + st_motor_info[M_TEST_RBT_Z].d_allow)
		{
			m_strMotorAlarmCode.Format(_T("%02d0008"),M_TEST_RBT_Z ); 
			CTL_Lib.Alarm_Error_Occurrence(100, dMOTOR_WARNING, m_strMotorAlarmCode);  
			return BD_ERROR;
		}
		d_CurPos[13] = COMI.Get_MotCurrentPos(M_TEST_DVC_EJECT_Z);
		if(d_CurPos[13] > st_motor_info[M_TEST_DVC_EJECT_Z].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[M_TEST_DVC_EJECT_Z].d_allow)
		{
			m_strMotorAlarmCode.Format(_T("%02d0008"),M_TEST_DVC_EJECT_Z ); 
			CTL_Lib.Alarm_Error_Occurrence(100, dMOTOR_WARNING, m_strMotorAlarmCode);  
			return BD_ERROR;
		}
		break; 

	case M_TEST_RBT_Z:
		if(d_CurPos[11] > st_motor_info[M_TEST_RBT_Y].d_pos[P_TESTRBT_Z_DVC_DIRECTION_CHK] + st_motor_info[M_TEST_RBT_Y].d_allow) //Y축 test site 위치 임을 알수있다
		{
			if(d_targetpos > st_motor_info[M_TEST_RBT_Z].d_pos[P_TESTRBT_Z_SAFETY] + st_motor_info[M_TEST_RBT_Z].d_allow) //더 내리는 위치로 test site 위치안에 있어야 한다
			{
				nRet_1 = clsFunc.Find_TestRobot_WherePos(0, m_npTestSitePos_Info);
				if(nRet_1 == RET_GOOD)
				{//m_npTestSitePos_Info[0] = test site 0 ~ 7 , m_npTestSitePos_Info[1]:left 또는 rigjt 정보 
					nIO[0] = FAS_IO.get_in_bit(st_io_info.i_ChamberOpenChk[m_npTestSitePos_Info[0]],	IO_ON);
					nIO[1] = FAS_IO.get_in_bit(st_io_info.i_ChamberCloseChk[m_npTestSitePos_Info[0]],	IO_OFF);
					if(nIO[0] == IO_ON && nIO[1] == IO_OFF)
					{
						//정상인 상태, chamber 가 open 되어 있다 +
					}
					else
					{
						if(nRet_1 == IO_ON) st_alarm_info.strCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ChamberOpenChk[m_npTestSitePos_Info[0]]); 
						CTL_Lib.Alarm_Error_Occurrence(100, dWARNING, st_alarm_info.strCode);
						return BD_ERROR;
					} 
				}
				else //해당 사이트를 찾지 못했다. error
				{
					m_strMotorAlarmCode.Format(_T("%02d0008"),M_TEST_RBT_Z ); 
					CTL_Lib.Alarm_Error_Occurrence(100, dMOTOR_WARNING, m_strMotorAlarmCode);  
					return BD_ERROR;
				}
			}
		}
		//kwlee 2016.0906 Safety 추가.
		nRet_1 = COMI.Check_MotPosRange(M_TEST_RBT_X, st_motor_info[M_TEST_RBT_X].d_pos[P_TESTRBT_XY_ULDBUF_START], 50);
		nRet_2 = COMI.Check_MotPosRange(M_TEST_RBT_Y, st_motor_info[M_TEST_RBT_Y].d_pos[P_TESTRBT_XY_ULDBUF_START], 50);

		if (nRet_1 == BD_GOOD && nRet_2 == BD_GOOD )  // 이동할 위치에 이미 존재함
		{
			if(d_CurPos[12] > st_motor_info[M_TEST_RBT_Z].d_pos[P_TESTRBT_Z_ULDBUF_PLACE] + st_motor_info[M_TEST_RBT_Z].d_allow)
			{
				m_strMotorAlarmCode.Format(_T("%02d0008"),M_TEST_RBT_Z ); 
				CTL_Lib.Alarm_Error_Occurrence(100, dMOTOR_WARNING, m_strMotorAlarmCode);  
				return BD_ERROR;
			}
		}
		//kwlee 2016.1004
// 		nRet_1 = clsFunc.Check_TestRobot_Pos();
// 		if (nRet_1 != BD_GOOD )
// 		{
// 			if(d_targetpos > st_motor_info[M_TEST_RBT_Z].d_pos[P_TESTRBT_Z_SAFETY] + st_motor_info[M_TEST_RBT_Z].d_allow) //더 내리는 위치로 test site 위치안에 있어야 한다
// 			{
// 				m_strMotorAlarmCode.Format(_T("%02d0008"),M_TEST_RBT_Z ); 
// 				CTL_Lib.Alarm_Error_Occurrence(100, dMOTOR_WARNING, m_strMotorAlarmCode);  
// 				return BD_ERROR;
// 			}
// 		}
		
			
		
		//kwlee 2016.0906 Safety 추가
// 		if(d_targetpos > st_motor_info[M_TEST_RBT_Z].d_pos[P_TESTRBT_Z_SAFETY] + st_motor_info[M_TEST_RBT_Z].d_allow) 
// 		{
// 			nRet_1 = COMI.Check_MotPosRange(M_TEST_RBT_X, st_motor_info[M_TEST_RBT_X].d_pos[P_TESTRBT_XY_ULDBUF_START], 50);
// 			nRet_2 = COMI.Check_MotPosRange(M_TEST_RBT_Y, st_motor_info[M_TEST_RBT_Y].d_pos[P_TESTRBT_XY_ULDBUF_START], 50);
// 
// 			if (nRet_1 == BD_GOOD && nRet_2 == BD_GOOD )  // 이동할 위치에 이미 존재함
// 			{
// 				nIO[0] = FAS_IO.get_in_bit(st_io_info.i_UldBuff1GripDvcExistChk,	IO_ON);
// 				nIO[1] = FAS_IO.get_in_bit(st_io_info.i_UldBuff2GripDvcExistChk,	IO_ON);	
// 				nIO[2] = FAS_IO.get_in_bit(st_io_info.i_UldBuff3GripDvcExistChk,	IO_ON);
// 				nIO[3] = FAS_IO.get_in_bit(st_io_info.i_UldBuff4GripDvcExistChk,	IO_ON);
// 
// 				nIO[4] = FAS_IO.get_out_bit(st_io_info.i_UldBuff1GripDvcExistChk,	IO_OFF);
// 				nIO[5] = FAS_IO.get_out_bit(st_io_info.i_UldBuff2GripDvcExistChk,	IO_OFF);	
// 				nIO[6] = FAS_IO.get_out_bit(st_io_info.i_UldBuff3GripDvcExistChk,	IO_OFF);
// 				nIO[7] = FAS_IO.get_out_bit(st_io_info.i_UldBuff4GripDvcExistChk,	IO_OFF);
// 
// 				if (nIO[0] == IO_ON || nIO[1] == IO_ON || nIO[2] == IO_ON || nIO[3] == IO_ON )  // 이동할 위치에 이미 존재함
// 				{
// 					if(nIO[0] == IO_ON) st_alarm_info.strCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_UldBuff1GripDvcExistChk); 
// 					if(nIO[1] == IO_ON) st_alarm_info.strCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_UldBuff2GripDvcExistChk); 
// 					if(nIO[2] == IO_ON) st_alarm_info.strCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_UldBuff3GripDvcExistChk); 
// 					if(nIO[3] == IO_ON) st_alarm_info.strCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_UldBuff4GripDvcExistChk); 
// 
// 					CTL_Lib.Alarm_Error_Occurrence(100, dWARNING, st_alarm_info.strCode);
// 					return BD_ERROR;
// 					// 					m_strMotorAlarmCode.Format(_T("%02d0008"),M_TEST_RBT_Z ); 
// 					// 					CTL_Lib.Alarm_Error_Occurrence(100, dMOTOR_WARNING, m_strMotorAlarmCode);  
// 					// 					return BD_ERROR;
// 				}
// 			}
//		}
		break;

	case M_TEST_DVC_EJECT_Z: 
		d_CurPos[13] = COMI.Get_MotCurrentPos(M_TEST_DVC_EJECT_Z);
		if(d_CurPos[13] > st_motor_info[M_TEST_DVC_EJECT_Z].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[M_TEST_DVC_EJECT_Z].d_allow)
		{
			if(d_CurPos[11] > st_motor_info[M_TEST_RBT_Y].d_pos[P_TESTRBT_XY_DVC_DIRECTION_LEFTSITE_CHK] + 20) //james 2016.0930 P_TESTRBT_XY_DVC_DIRECTION_CHK] + 20)
			{
				//작업 영역 
			}
			else 
			{
				m_strMotorAlarmCode.Format(_T("%02d0008"),M_TEST_DVC_EJECT_Z ); 
				CTL_Lib.Alarm_Error_Occurrence(100, dMOTOR_WARNING, m_strMotorAlarmCode);  
				return BD_ERROR;
			}				
		}

		if(d_CurPos[11] > st_motor_info[M_TEST_RBT_Y].d_pos[P_TESTRBT_Z_DVC_DIRECTION_CHK] + st_motor_info[M_TEST_RBT_Y].d_allow) //Y축 test site 위치 임을 알수있다
		{
			if(d_targetpos > st_motor_info[M_TEST_DVC_EJECT_Z].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[M_TEST_DVC_EJECT_Z].d_allow) //더 내리는 위치로 test site 위치안에 있어야 한다
			{
				nRet_1 = clsFunc.Find_TestRobot_WherePos(0, m_npTestSitePos_Info);
				if(nRet_1 == RET_GOOD)
				{//m_npTestSitePos_Info[0] = test site 0 ~ 7 , m_npTestSitePos_Info[1]:left 또는 rigjt 정보 
					nIO[0] = FAS_IO.get_in_bit(st_io_info.i_ChamberOpenChk[m_npTestSitePos_Info[0]],	IO_ON);
					nIO[1] = FAS_IO.get_in_bit(st_io_info.i_ChamberCloseChk[m_npTestSitePos_Info[0]],	IO_OFF);
					if(nIO[0] == IO_ON && nIO[1] == IO_OFF)
					{
						//정상인 상태, chamber 가 open 되어 있다 +
					}
					else
					{
						if(nRet_1 == IO_ON) st_alarm_info.strCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ChamberOpenChk[m_npTestSitePos_Info[0]]); 
						CTL_Lib.Alarm_Error_Occurrence(100, dWARNING, st_alarm_info.strCode);
						return BD_ERROR;
					} 
				}
				else //해당 사이트를 찾지 못했다. error
				{
					m_strMotorAlarmCode.Format(_T("%02d0008"),M_TEST_RBT_Z ); 
					CTL_Lib.Alarm_Error_Occurrence(100, dMOTOR_WARNING, m_strMotorAlarmCode);  
					return BD_ERROR;
				}
			}
		}

		break;
	}

	return RET_GOOD;
}

BOOL CCtlBd_Library::CreateFolder(LPCTSTR szPath)
{
	int nRet = 0;
	CString strTemp;
	CString strPath = szPath;
	char chFileName[256];

    do{
        nRet = strPath.Find('\\' , nRet + 1);
        if (nRet > 0 )
        {
            strTemp = strPath.Left(nRet+1);
			//wsprintfA(chFileName, "%S", strTemp);
			clsFunc.OnStringToChar(strTemp, chFileName);
            if (_access(chFileName.operator LPCTSTR(), 00) == -1)
			{
				CreateDirectory(strTemp , NULL);
			}
        }
    }while (nRet != -1);
	
    return nRet;
}

int CCtlBd_Library::FileSizeCheck(char * cp_filename, int n_size, int n_check)
{
	CFileFind finder;
	long nLen=0, nflag=0;
	BOOL bContinue;
	
	if(bContinue = finder.FindFile((LPCTSTR)cp_filename))
	{	
		if (n_check == BD_YES)			// Size를 체크하는것이면...
		{
			finder.FindFile((LPCTSTR)cp_filename);
			finder.FindNextFile();
			finder.GetFileName();
			nLen = (long)finder.GetLength();
			if(nLen > n_size)
			{
				nflag = 1;
			}
			finder.Close();	
		}
		else						// Size를 확인하는것이면...
		{
			finder.FindFile((LPCTSTR)cp_filename);
			finder.FindNextFile();
			finder.GetFileName();
			nLen = (long)finder.GetLength();
			
			nflag = nLen;
			
			finder.Close();	
		}
	}
	
	return nflag;
}

int CCtlBd_Library::LogFile_Write(char * cp_dir_path, char * cp_msg)
{
//2011.0417	sing.Lock(); //2011.0417 

	CString mstr_cur_year, mstr_cur_month, mstr_cur_day, str_display_time; // 현재 년, 월, 일 정보 문자형으로 변환하여 저장할 변수 
	int mn_cur_year, mn_cur_month, mn_cur_day, mn_cur_hour; // 현재 년, 월, 일 정보 저장 변수 
	CString mstr_file_name, mstr_dir_path, str_msg;		// 마지막으로 생성된 파일 이름 저장 변수 
	CString mstr_create_file;	// 알람 정보 저장할 파일에 대한 [폴더]+[파일명]+[확장자] 설정 변수 
	CString mstr_list_name, mstr_temp_data;  // 각 부분별 알람 발생 횟수 정보 저장할 로그 파일 정보 저장 변수 
	CString mstr_content;		// 한 라인에 출력할 문자열 정보 저장 변수 
	COleDateTime time_cur;		// 검사할 시간 정보 저장 변수 
	CTime m_time_current;		// 간략한 헝식의 현재 시간 정보 저장 변수
	char chr_buf[20]={0,};
	int mn_existence, nlength;			// 파일 존재 유무 설정 플래그 
	char cfileName[256]={0,};			// 검색할 파일 정보 설정 함수 
	FILE  *fp ;					// 파일에 대한 포인터 설정 변수 

	CString BackupName;
	CString mstr_cur_hour, mstr_cur_min, mstr_cur_sec;
	int Ret=0;

	nlength = (int)strlen(cp_msg);
	if(nlength <= 0) //저장할 메세지가 없다 
	{
		return RET_ERROR;
	}
	str_msg = cp_msg;
 
	// **************************************************************************
	// 파일 이름으로 사용할 날짜 정보를 얻는다                                   
	// **************************************************************************
	time_cur = COleDateTime::GetCurrentTime();  // 현재 시간 정보를 얻는다. 

	m_time_current = CTime::GetCurrentTime() ;  // 간략한 형식의 현재 시간 정보 얻는다. 

	mn_cur_year = time_cur.GetYear();  

	mn_cur_month = time_cur.GetMonth();  

    mn_cur_day = time_cur.GetDay();  

	mn_cur_hour = time_cur.GetHour();
	// **************************************************************************

	// **************************************************************************
	// 날짜 정보를 문자형으로 변환하여 변수에 설정한다                           
	// **************************************************************************
// jtkim 20130704
//	mstr_cur_year = LPCTSTR(_itoa( mn_cur_year, chr_buf, 10 ));  
	mstr_cur_year.Format(_T("%04d"), mn_cur_year);
	mstr_cur_month.Format(_T("%02d"), mn_cur_month);
	mstr_cur_day.Format(_T("%02d"), mn_cur_day);

	// **************************************************************************
	// 현재 시간 정보 얻는다                                                     
	// **************************************************************************
	mn_cur_hour = time_cur.GetHour();				// 현재 시간 정보를 설정한다. 
	mstr_cur_hour.Format(_T("%d"),time_cur.GetHour());	// 현재 시간 정보를 설정한다.
	mstr_cur_min.Format(_T("%d"),time_cur.GetMinute());	// 현재 분 정보를 설정한다. 
	mstr_cur_sec.Format(_T("%d"),time_cur.GetSecond());	// 현재 초 정보를 설정한다. 
	str_display_time = m_time_current.Format(_T("%c"));	// 리스트 파일에 출력할 타이틀 시간 정보 설정 
	// **************************************************************************


	mstr_dir_path = cp_dir_path;
	mstr_file_name = mstr_cur_year;
	mstr_file_name += mstr_cur_month; 
	mstr_file_name += mstr_cur_day; 
	mstr_create_file = mstr_dir_path + mstr_file_name;

	mstr_create_file += _T(".TXT");

//	sprintf(cfileName, "%s", mstr_create_file);
	//wsprintfA(cfileName, "%S", mstr_create_file);
	clsFunc.OnStringToChar(mstr_create_file, cfileName);
// jtkim 20130704
//	mn_existence = access(cfileName,0) ;
	mn_existence = _access(cfileName,0);
	if (mn_existence == -1)
	{//해당 파일이 존재하지 않는 상태이다 
		CreateFolder(mstr_create_file); 
	}
	else 
	{//해당 파일이 이미 존재한다.
		//정상 
	}
	
	Ret = FileSizeCheck(cfileName, 1048576, BD_YES); //size and rename	

	char chCreateFile[256];
	char chBackFile[256];

	if(Ret == 1) //file size over
	{
		BackupName = mstr_create_file + mstr_cur_hour + mstr_cur_min +  mstr_cur_sec + _T(".bak") ;

		//wsprintfA(chCreateFile, "%S", mstr_create_file);
		clsFunc.OnStringToChar(mstr_create_file, chCreateFile);
		//wsprintfA(chBackFile, "%S", BackupName);
		clsFunc.OnStringToChar(BackupName, chBackFile);

		rename(chCreateFile, chBackFile);
	}

	char chMode[10];
//	wsprintfA(chMode, "%S", _T("a+"));
	clsFunc.OnStringToChar(_T("a+"), chMode);
// jtkim 20130704
//	if ((fp = fopen(chCreateFile, chMode)) == NULL) 
	fopen_s(&fp, chCreateFile, "a+");
	if(!fp)
	{
		//AfxMessageBox("File open failure!..");
		//sprintf(mc_normal_msg,"LogFile_Write a+ Error"); 
		if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
		{
			//wsprintfA(st_other_info.cAbnormalMsg, "%S", _T("File open failure!.."));
			//clsFunc.OnStringToChar(_T("File open failure!.."), st_other_info.cAbnormalMsg);
			clsMem.OnAbNormalMessagWrite(_T("File open failure!.."));
			//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
		}
 		return RET_ERROR;
	}
	// **************************************************************************
	
	mstr_content += str_display_time;
	mstr_content += _T(" , ") + str_msg;

	char chData[4096];

	//wsprintfA(chData, "%S", mstr_content);
	clsFunc.OnStringToChar(mstr_content, chData);
	fprintf(fp,chData) ;
	if (ferror(fp))  
	{
		clearerr(fp); //2011.0418 

		fclose(fp) ;  //2011.0417  파일 종료
		return RET_ERROR;
	}
	// **************************************************************************
	fclose(fp);  // 파일을 종료한다.

	return RET_GOOD;
}

int CCtlBd_Library::IO_Safety(int n_io, int n_onoff)
{

	return RET_GOOD;
}

int	CCtlBd_Library::OnMotorGoCheck(int n_motor, double d_pos)
{
	return RET_GOOD;
}


///////////////////////////////////////////////////////////////////////////////////////
//2105.0102 
///////////////////////////////////////////////////////////////////////////////////////
//==================================================================//
// 입력된 위치로 모터 이동
// - [현재 위치 == 이동할 위치] 그냥 리턴됨
//==================================================================//
int CCtlBd_Library::Single_Move(int n_RunMethod, int n_MotorNum, double d_MovePos, int n_SpeedRate) 
{
	int nFuncRet = BD_PROCEED;
	long dwMotionDone=0;
	int nRet_1;
	int nAxis[2];
	double dTarget[2];
	
	COMI.mn_runspeed_rate = st_handler_info.nRunSpeed;
	

	if(n_RunMethod == 0) //0:move start만 진행, 1:move check만 진행, 2:move start and check까지 완료하고 BD_GOOD 처리 
	{

	}

	// 모터 축별로 데이터 관리
	nAxis[0]	= n_MotorNum;
	dTarget[0]	= d_MovePos;

	switch(m_nStep_Single[n_MotorNum])
	{
	case 0:
		if(n_RunMethod == ONLY_MOVE_CHECK)
		{
			m_nStep_Single[n_MotorNum] = 200;
			break;
		}
		
		nRet_1 = COMI.Check_MotPosRange(n_MotorNum, d_MovePos, st_motor_info[n_MotorNum].d_allow);//2014.1120 0);
		if (nRet_1 == BD_GOOD)  // 이동할 위치에 이미 존재함
		{
			m_nStep_Single[n_MotorNum] = 0;
			nFuncRet = BD_GOOD;
			break;
		}		
		//2015.0302 m_nMotRetry_Cnt[n_MotorNum] = 0;// 재시도 횟수 초기화
		m_nStep_Single[n_MotorNum] = 10; //2014.1015 100;
		m_dwUntil_Wait[n_MotorNum][0] = GetCurrentTime();
		break;

	case 10://pre start moving wait....
		/* 20150428 james 
		nRet_1 = cmmSxIsDone(n_MotorNum, &dwMotionDone);
		
		if(m_dwUntil_Wait[n_MotorNum][0]  < 0) m_dwUntil_Wait[n_MotorNum][0] = GetCurrentTime();
		m_dwUntil_Wait[n_MotorNum][1] = GetCurrentTime();
		m_dwUntil_Wait[n_MotorNum][2] = m_dwUntil_Wait[n_MotorNum][1] - m_dwUntil_Wait[n_MotorNum][0];
		if (nRet_1 == cmERR_NONE)
		{
			if (dwMotionDone != cmTRUE) //모션 동작 완료 == 1, 동작중은 0
			{	
				if(m_dwUntil_Wait[n_MotorNum][2] > 15000) //max 10sec wait
				{
					if (st_handler_info.cWndList != NULL)  
					{
						clsMem.OnNormalMessageWrite(_T("CCtlBd_Library::Single_Move = 10 1"));
						st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG); 
					}
					cmmSxStopEmg(n_MotorNum);	
			
					m_nStep_Single[n_MotorNum] = 100;
				}
				
			}
			else
			{
				m_nStep_Single[n_MotorNum] = 100;
			}
		}
		else
		{//2015.0215 추가 
			// jtkim 20150419
			if(m_dwUntil_Wait[n_MotorNum][2] > 20)
			{
				if (st_handler_info.cWndList != NULL)  
				{
					clsMem.OnNormalMessageWrite(_T("CCtlBd_Library::Single_Move = 10 2"));
					st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG); 
				}
	
				cmmSxStopEmg(n_MotorNum);	

				m_nStep_Single[n_MotorNum] = 0;
			}
		}*/

		//20150428 james 모터 state로 읽어 모터상태를 체크후 처리
		if(m_dwUntil_Wait[n_MotorNum][0]  < 0) m_dwUntil_Wait[n_MotorNum][0] = GetCurrentTime();
		m_dwUntil_Wait[n_MotorNum][1] = GetCurrentTime();
		m_dwUntil_Wait[n_MotorNum][2] = m_dwUntil_Wait[n_MotorNum][1] - m_dwUntil_Wait[n_MotorNum][0];

		//20150428 james 모터 state로 읽어 모터상태를 체크후 처리
		nRet_1 = COMI.Check_Motion_State(n_MotorNum, cmMST_STOP);//20150427 nRet_1 = cmmSxIsDone(n_MotorNum, &dwMotionDone);
		if (nRet_1 == BD_GOOD) 
		{//모터 상태가 mMST_STOP 이다 
			m_nStep_Single[n_MotorNum] = 100;
		}
		else if(nRet_1 == BD_PROCEED) //모터 상태가 동작중이다 
		{ 
			if(m_dwUntil_Wait[n_MotorNum][2] > 15000) //max 10sec wait
			{
				if (st_handler_info.cWndList != NULL)  
				{
					clsMem.OnNormalMessageWrite(_T("CCtlBd_Library::Single_Move = 10 1"));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG); 
				}
				cmmSxStopEmg(n_MotorNum);	
			
				m_nStep_Single[n_MotorNum] = 100;
			} 
		}
		else if(nRet_1 == BD_ERROR)//
		{  
			if(m_dwUntil_Wait[n_MotorNum][2] > 200)
			{
				if (st_handler_info.cWndList != NULL)  
				{
					clsMem.OnNormalMessageWrite(_T("CCtlBd_Library::Single_Move = 10 2"));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG); 
				}
	
				cmmSxStopEmg(n_MotorNum);	

				m_nStep_Single[n_MotorNum] = 0;
			}
		}
		break;

	case 100:// 입력된 위치로 모터 이동시킴
		/*2015.0216 james 
		if(n_RunMethod == ONLY_MOVE_CHECK)
		{
			m_nStep_Single[n_MotorNum] = 200;
			break;
		}*/

		nRet_1 = COMI.Start_SingleMove(n_MotorNum, d_MovePos, n_SpeedRate);
		if (nRet_1 == BD_GOOD)
		{
			if(n_MotorNum == 5)
			{
				m_nStep_Single[n_MotorNum] = 100;
			}
			if(n_RunMethod == ONLY_MOVE_START)
			{
				m_nStep_Single[n_MotorNum] = 0;
				nFuncRet = BD_GOOD;
				break;
			}
			else
			{
				m_nStep_Single[n_MotorNum] = 200;
			}
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{			
			m_nMotRetry_Cnt[n_MotorNum]++;// 재시도 횟수 증가시킴
			if (m_nMotRetry_Cnt[n_MotorNum]  > MAX_MOT_RETRY)
			{				
				m_nMotRetry_Cnt[n_MotorNum] = 0;
				m_nStep_Single[n_MotorNum] = 0;

				//2015.0312
				st_alarm_info.strCode.Format(_T("%02d0006"), n_MotorNum);
				st_alarm_info.nCountMode	= 0;	
				st_alarm_info.nTypeMode		= eWARNING;
				st_alarm_info.nAlarmAssignSection = 6;
				m_nStep_Single[n_MotorNum] = 0;
				m_nMotRetry_Cnt[n_MotorNum] = 0;

				if (st_handler_info.cWndList != NULL)  
				{
					clsMem.OnNormalMessageWrite(_T("CCtlBd_Library::Single_Move = 100 1"));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG); 
				}
				
				//2017.0109
				clsLog.OnLogMotorAlarm(_T("MOTOR"), _T("ERROR"), 1, nAxis, dTarget);

				nFuncRet = BD_ERROR; //nRet_1;
			}
			else
			{
				m_nStep_Single[n_MotorNum] = 100;
			}
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nMotRetry_Cnt[n_MotorNum]++;// 재시도 횟수 증가시킴			
			if (m_nMotRetry_Cnt[n_MotorNum]  > MAX_MOT_RETRY)
			{
				st_alarm_info.strCode.Format(_T("%02d0006"), n_MotorNum);
				st_alarm_info.nCountMode	= 0;	
				st_alarm_info.nTypeMode		= eWARNING;
				st_alarm_info.nAlarmAssignSection = 6;
				m_nStep_Single[n_MotorNum] = 0;
				m_nMotRetry_Cnt[n_MotorNum] = 0;

				if (st_handler_info.cWndList != NULL)  
				{
					clsMem.OnNormalMessageWrite(_T("CCtlBd_Library::Single_Move = 100 2"));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG); 
				}

				//2017.0109
				clsLog.OnLogMotorAlarm(_T("MOTOR"), _T("ERROR"), 1, nAxis, dTarget);

				nFuncRet = BD_ERROR;
			}
			else
			{
				m_nStep_Single[n_MotorNum] = 100;
			}
		}
		break;

	case 200:
		if(n_RunMethod == ONLY_MOVE_START)
		{
			m_nStep_Single[n_MotorNum] = 0;
			break;
		}

		nRet_1 = COMI.Check_SingleMove(n_MotorNum, d_MovePos);  // 이동 완료 확인
		if (nRet_1 == BD_GOOD)  // 정상 완료
		{
			m_nMotRetry_Cnt[n_MotorNum] = 0;
			m_nStep_Single[n_MotorNum] = 0;
			nFuncRet = BD_GOOD;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{
			m_nMotRetry_Cnt[n_MotorNum]++;// 재시도 횟수 증가시킴			
			if (m_nMotRetry_Cnt[n_MotorNum] > MAX_MOT_RETRY)
			{
				m_nStep_Single[n_MotorNum] = 0;

				//2015.0312
				st_alarm_info.strCode.Format(_T("%02d0006"), n_MotorNum);
				st_alarm_info.nCountMode	= 0;	
				st_alarm_info.nTypeMode		= eWARNING;
				st_alarm_info.nAlarmAssignSection = 6;
				m_nStep_Single[n_MotorNum] = 0;
				m_nMotRetry_Cnt[n_MotorNum] = 0;

				if (st_handler_info.cWndList != NULL)  
				{
					clsMem.OnNormalMessageWrite(_T("CCtlBd_Library::Single_Move = 200 1"));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG); 
				}
				//2017.0109
				clsLog.OnLogMotorAlarm(_T("MOTOR"), _T("ERROR"), 1, nAxis, dTarget);
				nFuncRet = BD_ERROR; //nRet_1;
			}
			else
			{
				m_nStep_Single[n_MotorNum] = 0;
			}
		}
		else if (nRet_1 == BD_RETRY)  // 동작 재시도
		{
			m_nMotRetry_Cnt[n_MotorNum]++;// 재시도 횟수 증가시킴			
			// 무한루프에 빠질수도 있지만 횟수를 지정해서 사용하면 문제 없음
			if (m_nMotRetry_Cnt[n_MotorNum] > MAX_MOT_RETRY)
			{
				st_alarm_info.strCode.Format(_T("%02d0006"), n_MotorNum);
				st_alarm_info.nCountMode	= 0;	
				st_alarm_info.nTypeMode		= eWARNING;
				st_alarm_info.nAlarmAssignSection = 6;
				m_nStep_Single[n_MotorNum] = 0;
				m_nMotRetry_Cnt[n_MotorNum] = 0;

				if (st_handler_info.cWndList != NULL)  
				{
					clsMem.OnNormalMessageWrite(_T("CCtlBd_Library::Single_Move = 200 2"));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG); 
				}
				//2017.0109
				clsLog.OnLogMotorAlarm(_T("MOTOR"), _T("ERROR"), 1, nAxis, dTarget);
				nFuncRet = BD_ERROR;
			}
			else
			{
				
				m_nStep_Single[n_MotorNum] = 10;//james 20150429  jtkim 20150419 //
				//20150429 james m_nStep_Single[n_MotorNum] = 100;
			}
		}
		break;

	}

	return nFuncRet;
}



int CCtlBd_Library::Linear_Move(int n_RunMethod, int n_LinearMapIndex, long l_AxisCnt, long *lp_AxisNum, double *dp_TargetPosList, int n_SpeedRate) 
{
	int nFuncRet = BD_PROCEED;
	int nRet[4] = {0,}, nCnt =0, i=0;
	double dp_SpdRatio[3] = {100,100,100};
	double dCurrentPos_Z = 0; //2015.0309 
	

	if(n_RunMethod == 0) //0:move start만 진행, 1:move check만 진행, 2:move start and check까지 완료하고 BD_GOOD 처리 
	{

	}
  
	switch(m_nStep_Linear[n_LinearMapIndex])
	{
	case 0:	

		nCnt = 0;
		st_linearmot[n_LinearMapIndex].l_AxisCnt = l_AxisCnt;
		COMI.ml_axiscnt[n_LinearMapIndex] = l_AxisCnt;
		for(i=0; i<l_AxisCnt; i++)
		{					 
			COMI.mp_axisnum[n_LinearMapIndex][i] = lp_AxisNum[i]; 
			st_linearmot[n_LinearMapIndex].lp_AxisNum[i] = lp_AxisNum[i]; 
			st_linearmot[n_LinearMapIndex].dp_PosList[i] = dp_TargetPosList[i];

			nRet[i] = COMI.Check_MotPosRange(lp_AxisNum[i], dp_TargetPosList[i], st_motor_info[lp_AxisNum[i]].d_allow);
			if(nRet[i] == BD_GOOD) nCnt++;
		}

		
		 

		if(nCnt == l_AxisCnt)
		{//이미 모든 모터의 현재 위치가 이동하려는 위치와 허용오차 안에 있으면 동작할 필요가 없으니 끝내자.
			nFuncRet = BD_GOOD;
			break;
		}

		if(n_RunMethod == ONLY_MOVE_CHECK)
		{
			m_nStep_Linear[n_LinearMapIndex] = 200; 
			break;
		}

		
		m_dwUntil_Wait[lp_AxisNum[0]][0] = GetCurrentTime();//20150429 james 
		
		m_nLinearRetry_Cnt[n_LinearMapIndex] = 0; 
		m_nStep_Linear[n_LinearMapIndex] = 100;
		/*
		if(n_LinearMapIndex == 2 && lp_AxisNum[0] == M_TSITE_RBT_X)   //2015.0309 추가 test site robot index map
		{
			m_nStep_Linear[n_LinearMapIndex] = 10;
		}*/
		break;

	case 10://2015.0309 추가 test site robot 안전 조건 추가 
		/*
		dCurrentPos_Z = COMI.Get_MotCurrentPos(M_TSITE_RBT_Z);  // 모터 특정 축의 현재 위치 리턴 함수 
		if(dCurrentPos_Z > st_motor_info[M_TSITE_RBT_Z].d_pos[P_TSITE_RBT_Z_SAFETY] + st_motor_info[M_TSITE_RBT_Z].d_allow) //안전 위치를 벅어나 있으면 저위치 시킨다 
		{
			if(st_motor_info[M_TSITE_RBT_Z].n_z_safety_ignore_flag != 1 || st_handler_info.nRunStatus != dRUN)// 
			{
				m_nStep_Linear[n_LinearMapIndex] = 20;
				break;
			}
			else
			{
				m_dwUntil_Wait[lp_AxisNum[0]][0] = GetCurrentTime();//20150429 james 
				m_nStep_Linear[n_LinearMapIndex] = 100;
			}
		}
		else
		{
			m_dwUntil_Wait[lp_AxisNum[0]][0] = GetCurrentTime();//20150429 james 
			m_nStep_Linear[n_LinearMapIndex] = 100;
		}*/

		m_dwUntil_Wait[lp_AxisNum[0]][0] = GetCurrentTime();//20150429 james 
		m_nStep_Linear[n_LinearMapIndex] = 100;

		break;

	case 20:
		/*
		nRet[0] = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, M_TSITE_RBT_Z, st_motor_info[M_TSITE_RBT_Z].d_pos[P_TSITE_RBT_Z_SAFETY], COMI.mn_runspeed_rate);  
		if (nRet[0] == BD_GOOD) //좌측으로 이동  
		{						 
			m_dwUntil_Wait[lp_AxisNum[0]][0] = GetCurrentTime();//20150429 james 
			m_nStep_Linear[n_LinearMapIndex] = 100;
		}
		else if (nRet[0] == BD_RETRY)
		{
			m_nStep_Linear[n_LinearMapIndex] = 20;
		}
		else if (nRet[0] == BD_ERROR || nRet[0] == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다  
			//2015.0312
			st_alarm_info.strCode.Format(_T("%02d0006"), M_TSITE_RBT_Z);
			st_alarm_info.nCountMode	= 0;	
			st_alarm_info.nTypeMode		= eWARNING;
			st_alarm_info.nAlarmAssignSection = 6; 

			CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);
			m_nStep_Linear[n_LinearMapIndex] = 20;
		}*/
		break; 
		
	case 100:	//20150429 james  추가 
		if(m_dwUntil_Wait[lp_AxisNum[0]][0]  < 0) m_dwUntil_Wait[lp_AxisNum[0]][0] = GetCurrentTime();
		m_dwUntil_Wait[lp_AxisNum[0]][1] = GetCurrentTime();
		m_dwUntil_Wait[lp_AxisNum[0]][2] = m_dwUntil_Wait[lp_AxisNum[0]][1] - m_dwUntil_Wait[lp_AxisNum[0]][0];

		nCnt =0;
		for(i=0; i<l_AxisCnt; i++)
		{	
			//20150428 james 모터 state로 읽어 모터상태를 체크후 처리
			nRet[i] = COMI.Check_Motion_State(lp_AxisNum[i], cmMST_STOP);//20150427 nRet_1 = cmmSxIsDone(n_MotorNum, &dwMotionDone);
			if (nRet[i] == BD_GOOD) 
			{//모터 상태가 mMST_STOP 이다 
				nCnt++; //정상인 수량 
				if(nCnt >= l_AxisCnt)
				{
					m_nStep_Linear[n_LinearMapIndex] = 110;
				}
			}
			else if(nRet[i] == BD_PROCEED) //모터 상태가 동작중이다 
			{ 
				if(m_dwUntil_Wait[lp_AxisNum[0]][2] > 15000) //max 15sec wait
				{
					if (st_handler_info.cWndList != NULL)  
					{
						clsMem.OnNormalMessageWrite(_T("CCtlBd_Library::Linear_Move = 15 1"));
						//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG); 
					}

					for(i=0; i<l_AxisCnt; i++)
					{
						cmmSxStopEmg(lp_AxisNum[i]);	
					}
			
					m_nStep_Linear[n_LinearMapIndex] = 100;
					break;
				} 
			}
			else if(nRet[i] == BD_ERROR)//
			{  
				if(m_dwUntil_Wait[lp_AxisNum[i]][2] > 200)
				{
					if (st_handler_info.cWndList != NULL)  
					{
						clsMem.OnNormalMessageWrite(_T("CCtlBd_Library::Linear_Move = 10 2"));
						st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG); 
					}
	
					for(i=0; i<l_AxisCnt; i++)
					{
						cmmSxStopEmg(lp_AxisNum[i]);	
					}
			
					m_nStep_Linear[n_LinearMapIndex] = 100;
					break;
				}
			}
		}
		break;

	case 110: //20150429 james 추가 

		dp_SpdRatio[0] = (double)n_SpeedRate;

		nRet[0] = COMI.Start_LinearMove(n_LinearMapIndex, st_linearmot[n_LinearMapIndex].dp_PosList, dp_SpdRatio);
		if (nRet[0] == BD_GOOD)
		{
			if(n_RunMethod == ONLY_MOVE_START)
			{
				m_nStep_Linear[n_LinearMapIndex] = 0;
				nFuncRet = BD_GOOD;
				break;
			}
			else
			{
				m_nStep_Linear[n_LinearMapIndex] = 200;
			}
		}
		else if (nRet[0] == BD_ERROR || nRet[0] == BD_SAFETY)
		{
			m_nLinearRetry_Cnt[n_LinearMapIndex]++;
			
			if(m_nLinearRetry_Cnt[n_LinearMapIndex] > MAX_MOT_RETRY) 
			{
				//2015.0312
				st_alarm_info.strCode.Format(_T("%02d0006"), lp_AxisNum[0]);
				st_alarm_info.nCountMode	= 0;	
				st_alarm_info.nTypeMode		= eWARNING;
				st_alarm_info.nAlarmAssignSection = 6; 
				/////

				m_nStep_Linear[n_LinearMapIndex] = 0;
				nFuncRet = BD_ERROR; //nRet[0];
			}
			else 
			{				
				m_nStep_Linear[n_LinearMapIndex] = 100;
			}
		}
		else if (nRet[0] == BD_RETRY)  // 동작 재시도
		{
			m_nLinearRetry_Cnt[n_LinearMapIndex]++;
			if(m_nLinearRetry_Cnt[n_LinearMapIndex] > MAX_MOT_RETRY) 
			{
		 
				st_alarm_info.strCode.Format(_T("%02d0006"), lp_AxisNum[0]);
				st_alarm_info.nCountMode	= 0;	
				st_alarm_info.nTypeMode		= eWARNING;
				st_alarm_info.nAlarmAssignSection = 6;

				m_nStep_Linear[n_LinearMapIndex] = 0;
				m_nMotRetry_Cnt[n_LinearMapIndex] = 0;
				nFuncRet = BD_ERROR;
			}
			else 
			{				
				m_nStep_Linear[n_LinearMapIndex] = 100;
			}
		}
		break;
		
	case 200:
		if(n_RunMethod == ONLY_MOVE_START)
		{
			m_nStep_Linear[n_LinearMapIndex] = 0; 
			break;
		}
		nRet[0] = COMI.Check_LinearMove(n_LinearMapIndex, st_linearmot[n_LinearMapIndex].dp_PosList) ;
		if (nRet[0] == BD_GOOD)  // 정상 완료
		{
			m_nLinearRetry_Cnt[n_LinearMapIndex] = 0; //clear
			m_nStep_Linear[n_LinearMapIndex] = 0;
			nFuncRet = BD_GOOD;
		}
		else if (nRet[0] == BD_ERROR || nRet[0] == BD_SAFETY)
		{
			m_nLinearRetry_Cnt[n_LinearMapIndex]++;
			
			if(m_nLinearRetry_Cnt[n_LinearMapIndex] > MAX_MOT_RETRY) 
			{
				//2015.0312
				st_alarm_info.strCode.Format(_T("%02d0006"), lp_AxisNum[0]);
				st_alarm_info.nCountMode	= 0;	
				st_alarm_info.nTypeMode		= eWARNING;
				st_alarm_info.nAlarmAssignSection = 6; 
				/////

				m_nStep_Linear[n_LinearMapIndex] = 0;
				nFuncRet = BD_ERROR; //nRet[0];
			}
			else 
			{				
				m_nStep_Linear[n_LinearMapIndex] = 100;
			}
		}
		else if (nRet[0] == BD_RETRY)  // 동작 재시도
		{
			// 무한루프에 빠질수도 있지만 횟수를 지정해서 사용하면 문제 없다
			m_nLinearRetry_Cnt[n_LinearMapIndex]++;
			
			if(m_nLinearRetry_Cnt[n_LinearMapIndex] > MAX_MOT_RETRY) 
			{
				//2015.0312
				st_alarm_info.strCode.Format(_T("%02d0006"), lp_AxisNum[0]);
				st_alarm_info.nCountMode	= 0;	
				st_alarm_info.nTypeMode		= eWARNING;
				st_alarm_info.nAlarmAssignSection = 6; 
				/////

				m_nStep_Linear[n_LinearMapIndex] = 0;
				m_nMotRetry_Cnt[n_LinearMapIndex] = 0;
				nFuncRet = BD_ERROR;
			}
			else 
			{				
				m_nStep_Linear[n_LinearMapIndex] = 100;
			}
		}	
		break;
	}
 
	return nFuncRet;
}


int CCtlBd_Library::SD_Sensor_Enable(int n_Mode, int n_AxisNum, int n_Enable)
{//Constant speed mode 에서는 SD 신호 입력이 무시됩니다.
	int nFuncRet = BD_PROCEED;

	if(n_Enable == CTL_YES) 
	{
		COMI.Set_Motor_IO_Property(n_AxisNum, cmSD_EN, cmTRUE);  
		COMI.Set_Motor_IO_Property(n_AxisNum, cmSD_LOGIC,cmFALSE);
		COMI.Set_Motor_IO_Property(n_AxisNum, cmSD_MODE,cmTRUE);
		COMI.Set_Motor_IO_Property(n_AxisNum, cmSD_LATCH, cmTRUE);

		st_motor_info[n_AxisNum].n_sd_mv_chk = 1; //sd를 조건을 확인하기 위해 이동한다 
	}
	else
	{
		COMI.Set_Motor_IO_Property(n_AxisNum, cmSD_EN, cmFALSE);  
		//COMI.Set_Motor_IO_Property(n_AxisNum, cmSD_LOGIC,cmTRUE);
		//COMI.Set_Motor_IO_Property(n_AxisNum, cmSD_MODE,cmTRUE);
		COMI.Set_Motor_IO_Property(n_AxisNum, cmSD_LATCH, cmFALSE);
		st_motor_info[n_AxisNum].n_sd_mv_chk = 0;
	}

	return BD_GOOD;
}

////////////////// 20150428 이하 함수 추가 

// ******************************************************************************
//20150427 
// 다중 모터 HOMING 작업 진행 함수                                               
// ******************************************************************************
int CCtlBd_Library::HomeCheck_Mot(int n_Axis, int n_HomeMode, int n_TimeOut) 
{
	int  nRet =-1, nRet_1=0, nRet_2=0, nRet_3=0, nFuncRet = BD_ERROR;
	long IsEnable=0;
	double LimitN=0, LimitP=0;
	CString strMsg; 

	if(st_work_info.nSimulationMode == 1)
	{// 
		return BD_GOOD;
	}

	/* 20150428 james 

	nFuncRet = BD_PROCEED;
	//20150427 n_HomeMode = mn_homecheck_method[n_Axis];

	if (n_HomeMode == 1) //org
	{	
		n_HomeMode = 0;
	}
	else if (n_HomeMode == 6)//el-
	{
		n_HomeMode = 1;
	}
	
	nRet = CTL_Lib.Motor_SafetyCheck(0, n_Axis, 0);
	if (nRet == BD_ERROR)
	{ 
		COMI.Set_MotStop(MOT_STOP_DEC, n_Axis); //감속정지   // 모터 이동 정지 함수 		
		COMI.mn_home_step[n_Axis] = 0; 
		return BD_ERROR;
	}
	else if(nRet == BD_RETRY) //091216 추가 
	{
		return BD_RETRY;
	}

//	cmmHomeSetConfig(n_Axis, 6, 0, 10, 0);
//	cmmHomeSetPosClrMode(n_Axis, 0);
	//cmmCfgSetMioProperty(m_nAxis, cmORG_LOGIC, m_nOrgLogic);
	//cmmCfgSetMioProperty(m_nAxis, cmEZ_LOGIC, m_nEzLogic);
	//cmmCfgSetMioProperty(m_nAxis, cmERC_OUT, m_bErcOut);  
	//cmmHomeSetSpeedPattern(m_nAxis, m_nSpeedMode, m_fWorkSpeed, m_fAcc, m_fDec, m_fRvsSpeed);


	switch(COMI.mn_home_step[n_Axis])
	{
		case 0 :
			if(COMI.mn_sd_io_property_flag[n_Axis] == BD_YES)  //SD Mode를 사용하는 모터이면 해당 모터는 홈 체크전 비활성한다 
			{
				COMI.Set_Motor_IO_Property(n_Axis, cmSD_EN, cmFALSE); //cmINP_EN=9 //cmFALSE = 0 INP 비활성, cmTRUE = 1 INP 활성 
			}
			 
			nRet_1 = COMI.Set_MotStop(MOT_STOP_DEC, n_Axis); //모터를 정지시킨다 		 
			nRet_2 = COMI.Set_MotHomeSpeed(MOT_SPD_CM_CTL, n_Axis, COMI.md_spd_home[n_Axis], 100, 100) ; //st_motor[i].d_spd_home[0] * 10, st_motor[i].d_spd_home[0] * 10);
			nRet_3 = COMI.Set_SoftLimit(n_Axis, FALSE,COMI.md_limit_position[n_Axis][0], COMI.md_limit_position[n_Axis][1]); //cmmCfgSetSoftLimit(n_Axis, FALSE, COMI.md_limit_position[n_Axis][0], COMI.md_limit_position[n_Axis][1]); //S/W limit 해제 
			if(nRet_1 == BD_GOOD && nRet_2 == BD_GOOD && nRet_3 == BD_GOOD) //20150422 james 조건이 충족했을때만 다음 스탭으로이동 가능 
			{
				COMI.mn_home_step[n_Axis] = 100;
			}
			else
			{
				nFuncRet = BD_ERROR;
			}
			break;

		case 100:
			if (COMI.Get_MotAlarmStatus(n_Axis) == BD_GOOD)  // BD_ERROR는 알람이 발생하지 않은 상태 
			{
				COMI.mn_home_step[n_Axis] = 200;
			}
			else
			{
				COMI.mn_home_step[n_Axis] = 110;
			}
			break;

		case 110 :
			if (COMI.Set_MotAlarmClear(n_Axis) == BD_GOOD)  // 모터 ALARM CLEAR 함수
			{
				COMI.mn_home_step[n_Axis]  = 100;
			}
			else
			{
				if (COMI.mn_retry_cnt[n_Axis] > COMI.mn_max_retry_cnt)
				{
					COMI.mn_retry_cnt[n_Axis] = 0;						
					COMI.mn_home_step[n_Axis] = 0;					 
					nFuncRet = BD_ERROR;					
				}
				else
				{
					COMI.mn_retry_cnt[n_Axis]++ ;
					COMI.mn_home_step[n_Axis] = 110;
				}
			}
			break;

		case 200 :
			if ( COMI.Get_MotPower(n_Axis) == BD_GOOD )  
			{// 모터 POWER 상태 ON
				COMI.mn_home_step[n_Axis] = 300;
			}
			else
			{
				COMI.mn_home_step[n_Axis] = 210;
			}
			break;

		case 210 :
			if ( COMI.Set_MotPower(n_Axis, BD_YES) == BD_GOOD )  // 모터 POWER ON 설정 함수
			{
				COMI.mn_home_step[n_Axis]  = 200;
			}
			else
			{
				if (COMI.mn_retry_cnt[n_Axis] > COMI.mn_max_retry_cnt)
				{
					COMI.mn_retry_cnt[n_Axis] = 0;						
					COMI.mn_home_step[n_Axis] = 0;					 
					nFuncRet = BD_ERROR;					
				}
				else
				{
					COMI.mn_retry_cnt[n_Axis]++ ;
					COMI.mn_home_step[n_Axis] = 210;
				}
			}
			break;

		case 300 :  // 모터 HOMING 
			nRet = COMI.Home_Start(n_Axis, cmDIR_N);//0 또는 cmDIR_N(-) 방향
			if (nRet == BD_GOOD)
			{
				COMI.ml_motion_move_time[n_Axis][0] = GetCurrentTime();
				COMI.mn_home_step[n_Axis] = 310;
			}
			else
			{
				if (COMI.mn_retry_cnt[n_Axis] > COMI.mn_max_retry_cnt)
				{				
					COMI.Set_MotStop(MOT_STOP_DEC, n_Axis); //감속정지   // 모터 이동 정지 함수 		

					COMI.mn_retry_cnt[n_Axis] = 0;						
					COMI.mn_home_step[n_Axis] = 0;					 
					nFuncRet = BD_ERROR;			
				}
				else
				{
					COMI.mn_retry_cnt[n_Axis]++ ;
					COMI.mn_home_step[n_Axis] = 300;
				}
			}
			break;	

		case 310 :  // MOTION DONE 검사 		
			nRet_1 = COMI.Get_MotMotionDone(n_Axis);        // 모터 이동 완료 검사 함수 //홈 체크시는 쓰면 안됨,  무조건 멈추면 YES 됨 
			nRet_2 = COMI.Get_MotMotionZeroReturn(n_Axis);  // 모터 ZERO BIT 검사 함수 
			nRet_3 = COMI.Home_GetSuccess(n_Axis, &IsEnable);

			if(n_HomeMode == MOT_SENS_HOME) //home sensor로 홈 체크하는 모드이면 
			{
				if (nRet_1 == BD_GOOD && nRet_2 == BD_GOOD && nRet_3 == BD_GOOD)
				{
					if (IsEnable == cmTRUE)//1 (TRUE)지정한 축은 현재 원점복귀가 정상적으로 수행된 상태입니다.
					{
						COMI.ml_motion_move_time[n_Axis][0] = GetCurrentTime();
						COMI.mn_home_step[n_Axis] = 320;
					}	
					else if(nRet_1 == BD_GOOD && nRet_3 == BD_GOOD)// 홈 체크 리턴이 나오지 않을때 
					{
						COMI.mn_retry_cnt[n_Axis]++ ;
 						COMI.mn_home_step[n_Axis] = 301;
					}
				}
				else if(n_HomeMode == MOT_SENS_ELM) //-EL sensor로 홈 체크하는 모드이면 
				{//st_motor[n_Axis].n_homecheck_method == 6)	// -EL로 홈체크 하는 모터일 경우에는 ZeroReturn은 보지 않는다.
					if (nRet_1 == BD_GOOD && nRet_3 == BD_GOOD)//1 (TRUE)지정한 축은 현재 원점복귀가 정상적으로 수행된 상태입니다.
					{
						COMI.mn_home_step[n_Axis] = 400;				
					}
				}
			}
			else if(nRet_1 == BD_ERROR || nRet_2 == BD_ERROR || nRet_3 == BD_ERROR)
			{
				COMI.Set_MotStop(MOT_STOP_DEC, n_Axis); //감속정지   // 모터 이동 정지 함수 		

				COMI.mn_retry_cnt[n_Axis] = 0;						
				COMI.mn_home_step[n_Axis] = 0;					 
				nFuncRet = BD_ERROR;		
				break;
			}

			COMI.ml_motion_move_time[n_Axis][1] = GetCurrentTime();
			COMI.ml_motion_move_time[n_Axis][2] = COMI.ml_motion_move_time[n_Axis][1] - COMI.ml_motion_move_time[n_Axis][0];
			
			if (COMI.ml_motion_move_time[n_Axis][2] >= n_TimeOut)
			{
				if (COMI.mn_retry_cnt[n_Axis] > COMI.mn_max_retry_cnt)
				{	
					COMI.Set_MotStop(MOT_STOP_DEC, n_Axis); //감속정지   // 모터 이동 정지 함수 		

					COMI.mn_retry_cnt[n_Axis] = 0;						
					COMI.mn_home_step[n_Axis] = 0;					 
					nFuncRet = BD_ERROR;	

					COMI.ml_alarmcode_id = 8; //000008 1 0 "MOTOR_HOME_CHECK_TIMEOUT_ERROR."
					strMsg.Format(_T("MOTNUM[%d] HOME_CHECK_TIMEOUT_ERROR]_1"), n_Axis);
					CTL_Lib.Motor_Error_Occurrence(3, n_Axis, COMI.ml_alarmcode_id, strMsg);	
				}
				else
				{
					COMI.mn_retry_cnt[n_Axis]++; 
				}
			}	
			else if(nRet_1 == BD_GOOD && COMI.ml_motion_move_time[n_Axis][2] > COMI.mn_max_home_limit_time) //최대 설정 시간까지 홈 체크가 끝난지 안았으면 다음 재 시도한다 
			{
				COMI.Set_MotStop(MOT_STOP_DEC, n_Axis); //감속정지   // 모터 이동 정지 함수 		

				COMI.mn_retry_cnt[n_Axis] = 0;						
				COMI.mn_home_step[n_Axis] = 0;					 
				nFuncRet = BD_ERROR;	

				COMI.ml_alarmcode_id = 8; //000008 1 0 "MOTOR_HOME_CHECK_TIMEOUT_ERROR."
				strMsg.Format(_T("MOTNUM[%d] HOME_CHECK_TIMEOUT_ERROR]_2"), n_Axis);
				CTL_Lib.Motor_Error_Occurrence(3, n_Axis, COMI.ml_alarmcode_id, strMsg);	 
			}
			break;	

		case 301: //
			COMI.Set_MotAlarmClear(n_Axis);
			COMI.mn_home_step[n_Axis] = 0;
			break;

		case 320 :  // HOME 센서 검사 
			if (COMI.Get_MotIOSensor(n_Axis, MOT_SENS_HOME) == BD_GOOD)  // 모터 센서 상태 검사 함수 [EL(+)    EL(-)    HOME]
			{
				COMI.mn_home_step[n_Axis] = 400;
			}
			else
			{			 
				COMI.ml_motion_move_time[n_Axis][1] = GetCurrentTime();
				COMI.ml_motion_move_time[n_Axis][2] = COMI.ml_motion_move_time[n_Axis][1] - COMI.ml_motion_move_time[n_Axis][0];
				if (COMI.ml_motion_move_time[n_Axis][2] > 500) //500ms  정도는 안정시간으로 기다려 준다 
				{
					COMI.Set_MotStop(MOT_STOP_DEC, n_Axis); //감속정지   // 모터 이동 정지 함수 		

					COMI.mn_retry_cnt[n_Axis] = 0;						
					COMI.mn_home_step[n_Axis] = 0;					 
					nFuncRet = BD_ERROR;	

					COMI.ml_alarmcode_id = 8; //000008 1 0 "MOTOR_HOME_CHECK_TIMEOUT_ERROR."
					strMsg.Format(_T("MOTNUM[%d] HOME_CHECK_SENSOR_CHECK_ERROR]"), n_Axis);
					CTL_Lib.Motor_Error_Occurrence(3, n_Axis, COMI.ml_alarmcode_id, strMsg);	 
				}			
			}
			break;

		case 400:
			COMI.ml_motion_move_time[n_Axis][0] = GetCurrentTime();
			COMI.mn_home_step[n_Axis] = 410;
			break;


		case 410 :  // 모터 커맨드 위치 초기화 
			// 20111212 jtkim
			//ml_motion_move_time[n_Axis][0] = GetCurrentTime();
			COMI.ml_motion_move_time[n_Axis][1] = GetCurrentTime();
			COMI.ml_motion_move_time[n_Axis][2] = COMI.ml_motion_move_time[n_Axis][1] - COMI.ml_motion_move_time[n_Axis][0];
			if (COMI.ml_motion_move_time[n_Axis][2] > 1000) //1초후에 정지한 상태에서 zjapsem & 엔코더 0로 클리어 
			{
				nRet_1 = cmmStSetPosition(n_Axis, cmCNT_COMM, (double)0);
				nRet_2 = cmmStSetPosition(n_Axis, cmCNT_FEED, (double)0);
				if(nRet_1 == cmERR_NONE && nRet_2 == cmERR_NONE)
				{
					COMI.mn_home_step[n_Axis] = 500;
				}
				else
				{
					COMI.Set_MotStop(MOT_STOP_DEC, n_Axis); //감속정지   // 모터 이동 정지 함수 		
					COMI.mn_retry_cnt[n_Axis] = 0;						
					COMI.mn_home_step[n_Axis] = 0;					 
					nFuncRet = BD_ERROR;	
					COMI.ml_alarmcode_id = 8; //000008 1 0 "MOTOR_HOME_CHECK_TIMEOUT_ERROR."
					strMsg.Format(_T("MOTNUM[%d] HOME_CHECK_cmCNT_FEED_0_CLEAR_ERROR]"), n_Axis);
					CTL_Lib.Motor_Error_Occurrence(3, n_Axis, COMI.ml_alarmcode_id, strMsg);	 
				} 				
			}		 
			break;

		case 500 :  // HOMING 완료 
	 		nRet_1 = COMI.Set_SoftLimit(n_Axis, TRUE,COMI.md_limit_position[n_Axis][0], COMI.md_limit_position[n_Axis][1]);  //cmmCfgSetSoftLimit(n_Axis, TRUE, md_limit_position[n_Axis][0], md_limit_position[n_Axis][1]); //S/W limit 설정  
 			COMI.mn_retry_cnt[n_Axis] = 0;						
			COMI.mn_home_step[n_Axis] = 0;	
			COMI.mn_homechk_flag[n_Axis] = BD_YES;  

			COMI.Set_MotStop(MOT_STOP_DEC, n_Axis); //감속정지  //20150420 james  cmmSxStop(n_Axis, FALSE, TRUE);  // 모터 이동 정지 함수 
			nFuncRet = BD_GOOD;
			break;
	}*/

	return nFuncRet;  
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////
//									MOTOR ERROR CODE DEFINITIONs								//
// 20150420 james		장비별로 이곳의 내용을 수정하여 사용 																		//
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCtlBd_Library::Motor_Error_Occurrence(int n_Mode, long l_Axis, long l_ErrorIDNum, CString strErrorMsg)//20150420 james 모터 리턴 에러 메세지 출력 
{
	CString strMsg, strMsg_1, strMsg_2, strMsg_ErrIDNum;	 
	 

	strMsg = _T("");
	strMsg_1 = _T("");
	strMsg_2 = _T("");
	strMsg_ErrIDNum = _T("");

	//n_Mode 1:strErrorMsg 만 사용, 2: n_Axis + n_ErrorIDNum 만 사용, 3:n_Axis + n_ErrorIDNum + strErrorMsg 모두 사용

	//-9999 ~ 099999 까지 번호 사용가능 

	////////////////////
	//모터 이름 정의 
	////////////////////
	switch(l_Axis)
	{//AMT8562는 29개의 모터를 사용한다 
	case -1: //사용하지 않는 루틴
		l_Axis = 0; //알람 코드 생성을 위해 초기화 추가 
		strMsg_1 = _T("");		
		break;
	case 0: //0번모터 
		strMsg_1.Format(_T("M_TRAY_LOAD_SUPPLY_X[%02ld]"), l_Axis);
		break;
	case 1:  //1번모터 
		strMsg_1.Format(_T("M_ELV1_LOAD_TRAY[%02ld]"), l_Axis);
		break;
	case 2:  //2번모터 
		strMsg_1.Format(_T("M_ELV2_EMPTY_TRAY[%02ld]"), l_Axis);
		break;
	case 3:   
		strMsg_1.Format(_T("M_ELV3_UNLOAD_TRAY1[%02ld]"), l_Axis);
		break;
	case 4:   
		strMsg_1.Format(_T("M_ELV4_UNLOAD_TRAY2[%02ld]"), l_Axis);
		break;
	case 5:   
		strMsg_1.Format(_T("M_ELV5_REJECT_LOAD[%02ld]"), l_Axis);
		break;
	case 6:   
		strMsg_1.Format(_T("M_ELV6_REJECT_1[%02ld]"), l_Axis);
		break;
	case 7:   
		strMsg_1.Format(_T("M_ELV7_REJECT_2[%02ld]"), l_Axis);
		break;
	case 8:   
		strMsg_1.Format(_T("M_DVC_LOAD_GANTRY_X[%02ld]"), l_Axis);
		break;
	case 9:   
		strMsg_1.Format(_T("M_DVC_LOAD_GANTRY_Y[%02ld]"), l_Axis);
		break;
	case 10:   
		strMsg_1.Format(_T("M_DVC_LOAD_GANTRY_Z[%02ld]"), l_Axis);
		break;
	case 11:   
		strMsg_1.Format(_T("M_LEFT_BUFF_TURN_INDEX_Y[%02ld]"), l_Axis);
		break;
	case 12:   
		strMsg_1.Format(_T("M_LEFT_BUFF_TURN[%02ld]"), l_Axis);
		break;
	case 13:   
		strMsg_1.Format(_T("M_RIGHT_BUFF_TURN_INDEX_Y[%02ld]"), l_Axis);
		break;
	case 14:   
		strMsg_1.Format(_T("M_RIGHT_BUFF_TURN[%02ld]"), l_Axis);
		break;
	case 15:   
		
		break;
	case 16:   
		strMsg_1.Format(_T("M_RETEST1_BUFF_LEFT_TURN[%02ld]"), l_Axis);
		break;
	case 17:   
		strMsg_1.Format(_T("M_RETEST2_BUFF_RIGHT_TURN[%02ld]"), l_Axis);
		break;
	case 18:   
		strMsg_1.Format(_T("M_LOAD_TRAY_TRANSFER_X[%02ld]"), l_Axis);
		break;
	case 19:   
		strMsg_1.Format(_T("M_REJECT_TRAY_TRANSFER_Y[%02ld]"), l_Axis);
		break;
	case 20:   
		strMsg_1.Format(_T("M_TESTER_SHUTTLE_X[%02ld]"), l_Axis);
		break;
	case 21:   
		strMsg_1.Format(_T("M_TESTER_SHUTTLE_Y[%02ld]"), l_Axis);
		break;
	case 22:   
		strMsg_1.Format(_T("M_TESTER_SHUTTLE_Z1[%02ld]"), l_Axis);
		break;
	case 23:   
		strMsg_1.Format(_T("M_TESTER_SHUTTLE_EJECTOR_Z2[%02ld]"), l_Axis);
		break;
	 
	default :
		strMsg_1.Format(_T("MOTOR_NUM_UNKNOWN_DEFAULT_ERROR[%02ld]"), l_Axis);
		l_Axis = 0;
		break;  

	}//모터 정보 이름 
		 

	//////////////////////////////////////////////////////////////////
	//커미조아 모터 함수 에러 정보 취합
	//////////////////////////////////////////////////////////////////
	switch(l_ErrorIDNum)
	{
	case -1:        //사용하지 않는 루틴 
		l_ErrorIDNum = 1; //알람 코드 생성을 위해 초기화 추가  
		strMsg_2.Format(_T(""));
		 break;	

	//모터 구동작업중 알람 정보
	case 1:
		 strMsg_2.Format(_T("MOTOR_POWER_OFF_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case 2:
		 strMsg_2.Format(_T("MOTOR_ALARM_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case 3:
		 strMsg_2.Format(_T("MOTOR_HOME_CHECK_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case 4:
		 strMsg_2.Format(_T("MOTOR_MOVE_START_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case 5:
		 strMsg_2.Format(_T("MOTOR_MOVE_CHECK_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case 6:
		 strMsg_2.Format(_T("MOTOR_MOVE_END_TIME_OUT_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case 7:
		 strMsg_2.Format(_T("MOTOR_SAFETY_CHECK_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;

	//모터 구동 관련 상태 및 리턴 관려 커미조아 사용 알람 정보
	case 10: 
		strMsg_2.Format(_T("MOTOR_SET_IO_PROPERTY_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case 11: 
		strMsg_2.Format(_T("MOTOR_PARAMETER_SETTING_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case 12: 
		strMsg_2.Format(_T("MOTOR_MOVE_POS_MINUS_LIMIT_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case 13: 
		strMsg_2.Format(_T("MOTOR_MOVE_POS_PLUS_LIMIT_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case 14: 
		strMsg_2.Format(_T("MOTOR_CMDPOS_FEEDPOS_DEVITION_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case 15: 
		strMsg_2.Format(_T("MOTOR_ALARM_CLEAR_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case 16: 
		strMsg_2.Format(_T("MOTOR_ALARM_STATUS_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case 17: 
		strMsg_2.Format(_T("MOTOR_ESTOP_RETURN_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case 18: 
		strMsg_2.Format(_T("MOTOR_GETCURRENTPOS_RETURN_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case 19: 
		strMsg_2.Format(_T("MOTOR_INPOSITION_CHECK_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case 20: 
		strMsg_2.Format(_T("MOTOR_MOTION_DONE_CHECK_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case 21: 
		strMsg_2.Format(_T("MOTOR_MOTION_DONE_ZERO_RETURN_CHECK_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case 22: 
		strMsg_2.Format(_T("MOTOR_HOME_CONFIG_RETURN_CHECK_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case 23: 
		strMsg_2.Format(_T("MOTOR_HOME_SPEED_RETURN_CHECK_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case 24: 
		strMsg_2.Format(_T("MOTOR_OVERRIDE_SPEED_RETURN_CHECK_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case 25: 
		strMsg_2.Format(_T("MOTOR_SPEED_RETURN_CHECK_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;

	case 26: 
		strMsg_2.Format(_T("MOTOR_UNIT_SPEED_RETURN_CHECK_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case 27: 
		strMsg_2.Format(_T("MOTOR_INTERRUPT_STATUS_CHECK_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case 28: 
		strMsg_2.Format(_T("MOTOR_TRIGGERSET_ONE_COMPARE_CHECK_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case 29: 
		strMsg_2.Format(_T("MOTOR_UNIT_DISTANCE_RETURN_CHECK_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case 30: 
		strMsg_2.Format(_T("MOTOR_SPEED_RATIO_RETURN_CHECK_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case 31: 
		strMsg_2.Format(_T("MOTOR_MOTION_STATE_CHECK_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;

	//List Motion 구동 동작 관련, 모터 번호 또는 이름에 이 내용을 더해서 메세지를 출력한다  
	case 41: 
		strMsg_2.Format(_T("MOTOR_LIST_MOTION_LMMAPAXIS_CREATE_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case 42: 
		strMsg_2.Format(_T("MOTOR_LINEAR_MOTION_IXMAPAXIS_CREATE_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;

	//Motor board 관련 에러 
	case 50: 
		strMsg_2.Format(_T("MOTOR_BOARD_LOAD_DLL_OPEN_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case 51: 
		strMsg_2.Format(_T("MOTOR_BOARD_LOAD_DEVICE_OPEN_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case 52: 
		strMsg_2.Format(_T("MOTOR_BOARD_CME_FILE_OPEN_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;
 

	////////////////////////////////////////////////////////////////////////////////////////////////
    //Motor dll return error id num
	////////////////////////////////////////////////////////////////////////////////////////////////
	case cmERR_NONE: //0: //정상인 상태 
		 strMsg_2 = _T("[%04ld]"), abs(l_ErrorIDNum);
		 break;
	case cmERR_UNKNOWN:
		strMsg_2.Format(_T("MOTOR_UNKNOWN_ERROR[%04ld]"), abs(l_ErrorIDNum));
		 break;	
	case cmERR_MEM_ALLOC_FAIL://-290
		 strMsg_2.Format(_T("Memory allocation fail[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_GLOBAL_MEM_FAIL://-292
		 strMsg_2.Format(_T("Global memory allocation fail[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_ISR_CONNEC_FAIL://-310
		 strMsg_2.Format(_T("ISR(Interrupt Service Routine) registration fail[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_DIVIDE_BY_ZERO://-400
		 strMsg_2.Format(_T("Cause divide by zero error[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_WORNG_NUM_DATA://-500
		 strMsg_2.Format(_T("Number of data is too small or too big[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_VER_MISMATCH://-600
		 strMsg_2.Format(_T("Version(of file or device) mismatch[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_INVALID_DEVICE_ID://-1010
		 strMsg_2.Format(_T("User set invalid device id. Refer to “DeviceId” property.[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_INVALID_HANDLE://-1020
		 strMsg_2.Format(_T("Device handle is not valid. This means that loading a device has been failed or not performed. Refer to “GnLoadDevcie” function[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_UNSUPORTED_FUNC://-1030
		 strMsg_2.Format(_T("User called an unsupported function for the specified product.[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_INVALID_PARAMETER://-1101
		 strMsg_2.Format(_T("Some of the function parameters are invalid.[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_INVALID_CHANNEL://-1105
		 strMsg_2.Format(_T("UThe channel setting parameter(s) is(are) invalid.[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_INVALID_INPUT_RANGE://-1111
		 strMsg_2.Format(_T("Invalid range value (AI, AO).[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_INVALID_FREQ_RANGE://-1121
		 strMsg_2.Format(_T("User selected invalid frequency range.[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_FILE_CREATE_FAIL://-1501
		 strMsg_2.Format(_T("File creation has been failed.[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_FILE_OPEN_FAIL://-1511
		 strMsg_2.Format(_T("File opening has been failed.[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_FILE_READ_FAIL://-1522
		 strMsg_2.Format(_T("File reading fail.[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_EVENT_CREATE_FAIL://-1550
		 strMsg_2.Format(_T("Event handle creation has been failed.[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_DITHREAD_CRE://-1570
		 strMsg_2.Format(_T("D/I state change monitor thread creation fail[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_BUFFER_SMALL://-1580
		 strMsg_2.Format(_T("Buffer size is too small[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_HIGH_TIMER_UNSUPP://-1090
		 strMsg_2.Format(_T("The installed hardware doesn‟t support a high-resolution performance counter.[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_OUT_OF_RANGE://-1600
		 strMsg_2.Format(_T("The range of some parameter is out of range that it is occured.[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_ON_MOTION://-5001
		 strMsg_2.Format(_T("This code is just a symbolic code. This error will never occur.[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_STOP_BY_SLP://-5002
		 strMsg_2.Format(_T("Abnormally stopped by positive soft limit[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_STOP_BY_SLN://-5003
		 strMsg_2.Format(_T("Abnormally stopped by negative soft limit[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_STOP_BY_CMP3://-5004
		 strMsg_2.Format(_T("Abnormally stopped by comparator3[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_STOP_BY_CMP4://-5005
		 strMsg_2.Format(_T("Abnormally stopped by comparator4[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_STOP_BY_CMP5://-5006
		 strMsg_2.Format(_T("Abnormally stopped by comparator5[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_STOP_BY_ELP://-5007
		 strMsg_2.Format(_T("Abnormally stopped by (+) external limit[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_STOP_BY_ELN://-5008
		 strMsg_2.Format(_T("Abnormally stopped by (-) external limit[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_STOP_BY_ALM://-5009
		 strMsg_2.Format(_T("Abnormally stopped by alarm input signal[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_STOP_BY_CSTP://-5010
		 strMsg_2.Format(_T("Abnormally stopped by CSTP input signal[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_STOP_BY_CEMG://-5011
		 strMsg_2.Format(_T("Abnormally stopped by CEMG input signal[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_STOP_BY_SD://-5012
		 strMsg_2.Format(_T("Abnormally stopped by SD input signal[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_STOP_BY_DERROR://-5013
		 strMsg_2.Format(_T("Abnormally stopped by operation data error[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_STOP_BY_IP://-5014
		 strMsg_2.Format(_T("Abnormally stopped by other axis error during interpolation[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_STOP_BY_PO://-5015
		 strMsg_2.Format(_T("An overflow occurred in the PA/PB input buffer[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_STOP_BY_AO://-5016
		 strMsg_2.Format(_T("Out of range position counter during interpolation[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_STOP_BY_EE://-5017
		 strMsg_2.Format(_T("An EA/EB input error occurred (does not stop)[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_STOP_BY_PE://-5018
		 strMsg_2.Format(_T("An PA/PB input error occurred (does not stop)[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_STOP_BY_SLVERR://-5019
		 strMsg.Format(_T("Abnormally stopped because slave axis has been stopped[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_STOP_BY_SEMG://-5120
		 strMsg_2.Format(_T("Abnormally stopped by software emergency setting[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_MOT_MAOMODE://-5110
		 strMsg_2.Format(_T("Master output mode is not CW/CCW mode during Master/Slave operation[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_MOT_SLAVE_SET://-5120
		 strMsg_2.Format(_T("Slave start has been failed during Master/Slave operation[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_SPEED_RANGE_OVER://-5130
		 strMsg_2.Format(_T("Speed setting value exceeds setting range[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_INVALID_SPEED_SET://-5140
		 strMsg_2.Format(_T("Speed setting value is not valid[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_INVALID_IXMAP://-5150
		 strMsg_2.Format(_T("Invalid Interpolation Map[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_INVALID_LMMAP://-5160
		 strMsg_2.Format(_T("Invalid List-Motion Map[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_MOT_SEQ_SKIPPED://-5170
		 strMsg_2.Format(_T("Invalid List-Motion Map[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_CMPIX_INVALID_MAP://-5180
		 strMsg_2.Format(_T("Interpolated position compare output map is not valid[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_INVALID_ARC_POS://-5190
		 strMsg_2.Format(_T("Position data for circular interpolation is invalid[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_LMX_ADD_ITEM_FAIL://-5200
		 strMsg_2.Format(_T("failed to add an job item to extend list motion[%04ld]"), abs(l_ErrorIDNum));
		 break;
	case cmERR_LMX_IS_NOT_ACTIVE://-5300
		 strMsg_2.Format(_T("Extended ListMotion' is not active extend list motion[%04ld]"), abs(l_ErrorIDNum));
		 break;
	default:
		strMsg_2.Format(_T("MOTOR_UNKNOWN_DEFAULT_ERROR[%04ld]"), abs(l_ErrorIDNum));
		l_ErrorIDNum = 1; //초기화 처리 
		break;
	 }
	 
	 /*
	 //n_Mode 1:strErrorMsg 만 사용, 2: n_Axis + n_ErrorIDNum 만 사용, 3:n_Axis + n_ErrorIDNum + strErrorMsg 모두 사용
	 if(n_Mode == 1) //메세지만 출력 
	 {
		 l_Axis = 0; //알람코드 6자리 유지위해 추가 
		 l_ErrorIDNum = 0;
		 //strMsg = strErrorMsg;
	 }
	 else if(n_Mode == 2) //모터 번호 및 알람코드 정보 출력 
	 {
		 strErrorMsg = _T("");
		 //strMsg = strMsg_1 + strMsg_2;		  
	 }
	 else if(n_Mode == 3) //모터 번호+ 알람코드 + 메세지  출력 
	 {
		 	//전체 정보 사용
	 }*/

	 

	 strMsg = strMsg_1 + _T("") + strMsg_2 + _T("") + strErrorMsg;	

	 strMsg_ErrIDNum.Format(_T("%02ld%04ld"), l_Axis, abs(l_ErrorIDNum)); //필요시 사용위해 6자리의 에러 코드 정의 

	 
	
	 if(n_Mode > 0)
	 {//장비에 알람 정보 처리 

		 //쓰레드에서 알람 관련 장비 스톱은 처리하게 하자 st_handler_info.nRunStatus = dWARNING;
	 }
	 else if(n_Mode == 0)
	 {//모터 관련 메세지만 로깅하여 파일에 쓰고, 장비는 정상 가동시킨다 
		 //같은 메세지를 계속 쓰지 못하게 할것 
		 if(str_Motor_Msg == strMsg)
		 {
			 return;
		 }
	 }

	 //////////////////////////////////////////////////////////////////////////
	 //알람 코드를 쓰려면 이곳의 뵨수 정보 중 (strMsg_ErrIDNum,mc_alarmcode)   하나를 사용 하자 
	 OnStringToChar(strMsg_ErrIDNum, mc_alarmcode);  // 알람코드 변환 
	 //////////////////////////////////////////////////////////////////////////
	 // jtkim 20150501
	 if(COMI.mn_errormsg_debug_mode)
	 {
		OnStringToChar(strMsg, mc_normal_msg);  //msg 화면 출력 
		COMI.Debug_File_Create(0, mc_normal_msg);    //file로 log	  
	 }

	 str_Motor_Msg = strMsg; //마지막 사용한 메세지 정보를 저장

	 if (st_handler_info.cWndList != NULL)
	 {
		clsMem.OnNormalMessageWrite(strMsg);
		//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
	 }
}



void CCtlBd_Library::OnStringToChar(CString strData, char chData[])
{
	int nCnt;
	wchar_t *chBuf;

	chBuf = strData.GetBuffer(strData.GetLength());
	nCnt  = WideCharToMultiByte(CP_ACP, 0, chBuf, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, chBuf, -1, chData, nCnt, 0, 0);
}


int CCtlBd_Library::Debug_File_Create(int n_mode, char* s_msg) //2012.1029 jamestest  CString s_msg)
{
//2011.0417	sing.Lock(); //2011.0417 
	CFileFind filefind;
	CString mstr_cur_year, mstr_cur_month, mstr_cur_day, str_display_time; // 현재 년, 월, 일 정보 문자형으로 변환하여 저장할 변수 
	int mn_cur_year, mn_cur_month, mn_cur_day, mn_cur_hour; // 현재 년, 월, 일 정보 저장 변수 
	CString mstr_file_name;		// 마지막으로 생성된 파일 이름 저장 변수 
	CString mstr_create_file;	// 알람 정보 저장할 파일에 대한 [폴더]+[파일명]+[확장자] 설정 변수 
	CString mstr_list_name, mstr_temp_data;  // 각 부분별 알람 발생 횟수 정보 저장할 로그 파일 정보 저장 변수 
	CString mstr_content;		// 한 라인에 출력할 문자열 정보 저장 변수 
	COleDateTime time_cur;		// 검사할 시간 정보 저장 변수 
	CTime m_time_current;		// 간략한 헝식의 현재 시간 정보 저장 변수
	char chr_buf[20]={0,};
	int mn_existence;			// 파일 존재 유무 설정 플래그 
	char fileName[256]={0,};			// 검색할 파일 정보 설정 함수 
	FILE  *fp = NULL;					// 파일에 대한 포인터 설정 변수 

	CString BackupName;
	CString mstr_cur_hour, mstr_cur_min, mstr_cur_sec;
	int nRet=BD_GOOD;

	if (0) //2012.1029 jamestest s_msg.IsEmpty() == TRUE)  
	{
		return BD_ERROR;
	}

	// **************************************************************************
	// 파일 이름으로 사용할 날짜 정보를 얻는다                                   
	// **************************************************************************
	time_cur = COleDateTime::GetCurrentTime();  // 현재 시간 정보를 얻는다. 

	m_time_current = CTime::GetCurrentTime() ;  // 간략한 형식의 현재 시간 정보 얻는다. 

	mn_cur_year = time_cur.GetYear();  

	mn_cur_month = time_cur.GetMonth();  

    mn_cur_day = time_cur.GetDay();  

	mn_cur_hour = time_cur.GetHour();
	// **************************************************************************

	// **************************************************************************
	// 날짜 정보를 문자형으로 변환하여 변수에 설정한다                           
	// **************************************************************************
//	mstr_cur_year = LPCTSTR(_itoa( mn_cur_year, chr_buf, 10 ));  
	mstr_cur_year.Format(_T("%04d"), mn_cur_year);
//2012.1029 jamestest mstr_cur_month.Format("%02d", mn_cur_month);
//2012.1029 jamestestmstr_cur_day.Format("%02d", mn_cur_day);

	// **************************************************************************
	// 현재 시간 정보 얻는다                                                     
	// **************************************************************************
	mn_cur_hour = time_cur.GetHour();				// 현재 시간 정보를 설정한다. 
//2012.1029 jamestest mstr_cur_hour.Format("%d",time_cur.GetHour());	// 현재 시간 정보를 설정한다.
//2012.1029 jamestest mstr_cur_min.Format("%d",time_cur.GetMinute());	// 현재 분 정보를 설정한다. 
//2012.1029 jamestest mstr_cur_sec.Format("%d",time_cur.GetSecond());	// 현재 초 정보를 설정한다. 
	str_display_time = m_time_current.Format("%c");	// 리스트 파일에 출력할 타이틀 시간 정보 설정 
	// **************************************************************************


	mstr_file_name =  mstr_cur_year;
	mstr_file_name += mstr_cur_month; 
	mstr_file_name += mstr_cur_day; 
//2012.1029 jamestestmstr_create_file = "c:\\AMT_LOG\\Motor\\" + mstr_file_name;
	mstr_create_file += ".TXT";

	//wsprintfA(fileName, "%S", mstr_create_file);
	clsFunc.OnStringToChar(mstr_create_file, fileName);

	mn_existence = filefind.FindFile(mstr_create_file,0);

	nRet = FileSizeCheck(mstr_create_file, 1048576, BD_YES); //size and rename
	if(nRet == 1) //file size over
	{
//2012.1029 jamestest		BackupName = mstr_create_file + mstr_cur_hour + mstr_cur_min +  mstr_cur_sec + ".bak" ;
//2012.1029 jamestest		rename(mstr_create_file, BackupName);
	}

	if (0) ////2012.1029 jamestest (fp = fopen(mstr_create_file,"a+")) == NULL)  
	{
		//AfxMessageBox("File open failure!..");
		//wsprintfA(mc_normal_msg, "%S", _T("File open failure!.._4"));
		clsFunc.OnStringToChar(_T("File open failure!.._4"), mc_normal_msg);
//		sprintf(mc_normal_msg, "File open failure!.._4");
		return BD_ERROR;
	}

	// **************************************************************************
	// 로그 파일에 현재 발생한 알람 정보 저장한다                                
	// **************************************************************************
	
	mstr_content = str_display_time;//2012.1029 jamestest

//2012.1029 jamestest mstr_content += " : " + s_msg;
//	_snprintf(fileName, sizeof(fileName), "%S", mstr_content);
	//wsprintfA(fileName, "%S", mstr_content);
	clsFunc.OnStringToChar(mstr_content, fileName);
//	fprintf(fp,_T("%S\n"),mstr_content) ;
	fprintf(fp,fileName) ;

	if (ferror(fp))  
	{
		//wsprintfA(mc_normal_msg, "%S", _T("Debug_File_Create ferror return error"));
		clsFunc.OnStringToChar(_T("Debug_File_Create ferror return error"), mc_normal_msg);
//		sprintf(mc_normal_msg,"Debug_File_Create ferror return error");	
		clearerr(fp); //2011.0418 

		fclose(fp) ;  //2011.0417  파일 종료
		return BD_ERROR;
	}
	fclose(fp);  // 파일을 종료한다.

	return nRet;
}

int CCtlBd_Library::FileSizeCheck(CString s_filename, long l_size, int n_check)
{
	CFileFind finder;
	long Len=0, flag=0;
	BOOL bContinue;
	
	if(bContinue = finder.FindFile(s_filename))
	{	
		if (n_check == BD_YES)			// Size를 체크하는것이면...
		{
			finder.FindFile(s_filename);
			finder.FindNextFile();
			finder.GetFileName();
			Len = (long)finder.GetLength();
			if(Len > l_size)
			{
				flag = 1;
			}
			finder.Close();	
		}
		else						// Size를 확인하는것이면...
		{
			finder.FindFile(s_filename);
			finder.FindNextFile();
			finder.GetFileName();
			Len = (long)finder.GetLength();
			
			flag = Len;
			
			finder.Close();	
		}
	}
	
	return flag;
}


//////////////////////////////////////
//james 2016.0803 
/////////////////////////////////////
int CCtlBd_Library::Elevator_Job_Move_Pos(int nMode, int n_AxisNum,  int n_TargetPos_Site)
{
	int nFuncRet = RET_PROCEED;
	int nRet_1, nRet_2;
	double dCurrentPos;
	CString strlog;
//	double dPos_1, dPos_2, dPos_3;

	clsFunc.OnThreadFunctionStepTrace(30, m_nSD_Elv_MoveStep[n_AxisNum]);


	//LIGHT CURTAIN
	if( st_sync_info.nLdUld_LightChk == YES )
	{
		if( n_AxisNum == M_LD_CV_LR_SHIFTER || n_AxisNum == M_LD_ELV || n_AxisNum == M_EMPTY_ELV || n_AxisNum == M_ULD_1_ELV || n_AxisNum == M_ULD_2_ELV )
		{
			cmmSxStop(n_AxisNum, FALSE, FALSE); 
			return BD_RETRY;
		}
	}
	else if( st_sync_info.nRej_LightChk == YES )
	{
		if( n_AxisNum == M_RJ_EMPTY_ELV || n_AxisNum == M_RJ_ULD_1_ELV || n_AxisNum == M_RJ_ULD_2_ELV )
		{
			cmmSxStop(n_AxisNum, FALSE, FALSE); 
			return BD_RETRY;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////
//SD 센서가 기준점을 잡는 센서 이다 
//	센서로  바로 밑이 기준점으로 항상 SD sensor 대기위치로 기구을 셋팅하면 이에따라 제어한다 
//////////////////////////////////////////////////////////////////////////////////////////////////
	//if(m_nStackerMotorMove_UpDown_Flag != 100)
	//{
	//	m_nStackerMotorMove_UpDown_Flag = 1; //
	//}


	//P_ELV_SAFETY		  = 0,//안전 위치 (down limit 위치일것임)
	//P_ELV_UP_LIMIT		    ,  //high rail 까지 빈 트레이 공급 또는 트레이를 받을떄  제일 윗쪽까지 올리면 체크
	//P_ELV_DN_LIMIT		    ,  //평상시는 트레이 받은 후 soflEo dncl tkdyd, 또는 제일 밑면, 트레이 최종 체크 또는 Full로 교체시 내려간다 	
	//P_ELV_SD_SENSOR				, //sd sensor 감지 위치(트레이가 한장 이상 있을때 감지됨. 하나도 없으면 감지되지 않음 	
	//P_ELV_TRAY_INOUT_POS,		//트레이를 배출 또는 입력 가능한 위치(CONVEYOR 등이 동작하여 트레이를 넣고,  빼기가 가능한 위치 
	//SD 센서 기준점으로 부터 이동하는 위치 
	//P_ELV_SUPPLY_OFFSET	,	  // SD pos + 트레이를 작업 가능하게 하는 위치 
	//P_ELV_RECEIVE_OFFSET	, // SD pos + 트레이를 받는 위치 

	if(st_handler_info.nRunStatus != dRUN)
	{   //time out 시간 체크 문제 보완을 위해  
		if(m_bSD_MoveFlag[n_AxisNum] == true) m_bSD_MoveFlag[n_AxisNum] = false; 
	}

	switch(m_nSD_Elv_MoveStep[n_AxisNum])
	{
		case 0:	
			//모터 이동 조건은 아래 3가지만 동작한다 
			if(n_TargetPos_Site == P_ELV_SUPPLY_OFFSET)//트레이를 작업 가능한 영역에 UP하여 공급하는 위치, SD pos 센서 On 감지 후 센서 기준 - 방항으로 벗어난 후 + 방향으로 P_ELV_SUPPLY_OFFSET 티칭만큼 up 한후 모터 동작은 완료하고, 트레이르, 받든다 
			{
				st_sync_info.nSmema_Tray_Output_Req = 0;
				m_nSD_Elv_MoveStep[n_AxisNum] = 1000;
			}			
			else if(n_TargetPos_Site == P_ELV_RECEIVE_OFFSET) //작업이 끝난 트레이를 받는 위치, SD pos 센서 On 감지 후 센서 기준 - 방항으로 센서를 벗어난 후 - 방향으로 P_ELV_RECEIVE_OFFSET 티칭만큼 down 한 후 트레이를 받는 위치  
			{
				//nRet_1 = FAS_IO.get_in_bit(st_io_info.i_UnldTrayRail1EmptyTrayFullChk,	IO_OFF);
				nRet_1 = COMI.Get_MotIOSensor(n_AxisNum, MOT_SENS_SD);
				nRet_2 = COMI.Check_MotPosRange(n_AxisNum, m_dSD_Supply_Pos_Backup[n_AxisNum], st_motor_info[n_AxisNum].d_allow); //바로전 셋팅한 기준점이 맞으면 동작 
				dCurrentPos = COMI.Get_MotCurrentPos(n_AxisNum); 
				if(nRet_1 == BD_ERROR && nRet_2 == BD_GOOD &&  (dCurrentPos > 1 && dCurrentPos < st_motor_info[n_AxisNum].d_pos[P_ELV_SD_SENSOR])) 
				{ //작업 목적이 기준점 잡는 것이고, 현재 위치가 기준점으로 여겨지면 추가 동작이 필요없으니 바로 리턴하자					
					m_nSD_Elv_MoveStep[n_AxisNum] = 0;
					nFuncRet = RET_GOOD;
				}
				else
				{
					st_sync_info.nSmema_Tray_Output_Req = 0;
					m_nSD_Elv_MoveStep[n_AxisNum] = 1000;
				}
			}
			else 
			{
				//다른 동작은 이 함수를 사용 할 필요 없음 
				//에러 발생 필요 
			}			
			break;

			
		///////////////////////////////////////////////////////////////////////////
		// SD Sensor 기준 위 또는 아래인지 확인하여, 위면 내려가고, 아래면 올라간다 
		///////////////////////////////////////////////////////////////////////////
		case 1000: //기준점 위치 확인 (SD Sensor 대비 high or 같은 위치인지인지, low인지 두가지 위치를 확인한다)			 
			nRet_1 = COMI.Get_MotIOSensor(n_AxisNum, MOT_SENS_SD); 	
			dCurrentPos = COMI.Get_MotCurrentPos(n_AxisNum); 
			CTL_Lib.SD_Sensor_Enable(0, n_AxisNum, CTL_NO); //sd sensor clear			

			if(nRet_1 == BD_GOOD)
			{//SD가 감지되면 위에 있는 상태 
				m_bSD_MoveFlag[n_AxisNum] = false;
				m_nSD_Elv_MoveStep[n_AxisNum] = 6000; //기준점보다 위에 있는 상태로 아래로 내리면서 SD를 지나가기를 기다리자 
			}
			else //if(nRet_1 == BD_ERROR)
			{//SD도 감지되지 않았고, SD 위치보다 낮은 상태로, 위로 올라가면서 SD를 찾는다 
				if(n_TargetPos_Site == P_ELV_RECEIVE_OFFSET)
				{
					nRet_1 = COMI.Get_MotIOSensor(n_AxisNum, MOT_SENS_SD);
					nRet_2 = COMI.Check_MotPosRange(n_AxisNum, m_dSD_Receive_Pos_Backup[n_AxisNum], st_motor_info[n_AxisNum].d_allow); //바로전 셋팅한 기준점이 맞으면 동작 
					if(nRet_1 == BD_GOOD && nRet_2 == BD_GOOD) //전에 받았던 위치에 자재를 내려놓고다시 받을 준비를 하는 루틴이다 
					{
						m_nSD_Elv_MoveStep[n_AxisNum] = 2500;
						break;
					}
				}

				m_bSD_MoveFlag[n_AxisNum] = false;
				m_nSD_Elv_MoveStep[n_AxisNum] = 2000; //traget pos를 이동하는 루틴 
			}	 
			break;

		////////////////////////////////////////////////////////////
		//SD 센서를 찾으면서 위로 이동하다 
		////////////////////////////////////////////////////////////	
		case 2000: 
			if(m_bSD_MoveFlag[n_AxisNum] == false)
			{
				m_dwSdWaitTime[n_AxisNum][0] = GetTickCount();
				m_bSD_MoveFlag[n_AxisNum] = true;
				CTL_Lib.SD_Sensor_Enable(0, n_AxisNum, CTL_YES); //sd sensor set
			}
			else if(m_bSD_MoveFlag[n_AxisNum] == true)
			{
				m_dwSdWaitTime[n_AxisNum][1] = GetTickCount();
				m_dwSdWaitTime[n_AxisNum][2] = m_dwSdWaitTime[n_AxisNum][1] - m_dwSdWaitTime[n_AxisNum][0];
				if(m_dwSdWaitTime[n_AxisNum][2] <= 0 )
				{
					m_dwSdWaitTime[n_AxisNum][0] = GetTickCount();
					break;
				}
				if(m_dwSdWaitTime[n_AxisNum][2] > MOT_ELE_MOVE_WAITLIMIT)
				{//limit 시간 지남, 에러 처리후 재 시도하자 
					COMI.Set_MotStop(1, n_AxisNum) ; //긴급정지 
					m_nSD_Elv_MoveStep[n_AxisNum] = 1000;
					break;
				}
			}

			nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_START, n_AxisNum, st_motor_info[n_AxisNum].d_pos[P_ELV_SD_SENSOR], COMI.mn_runspeed_rate);   //제일 밑까지 이동한다 
			if (nRet_1 == BD_GOOD) //위로 이동 하면서 SD를 찾는다  
			{					 				 
				m_nSD_Elv_MoveStep[n_AxisNum] = 2100;
			}
			else if (nRet_1 == BD_RETRY)
			{				 
				m_nSD_Elv_MoveStep[n_AxisNum] = 1000;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다  
				CTL_Lib.Alarm_Error_Occurrence(272, dWARNING, st_alarm_info.strCode);

				if (st_handler_info.cWndList != NULL)  
				{
					//clsMem.OnNormalMessageWrite(_T("UnLoader Alarm : 2000"));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG); 
				}
				m_nSD_Elv_MoveStep[n_AxisNum] = 1000;
			}
			break;

		case 2100:
			nRet_1 = COMI.Get_MotIOSensor(n_AxisNum, MOT_SENS_SD); 
			if(nRet_1 == BD_GOOD)
			{//정상적으로 체크했다. 
				COMI.Set_MotStop(1, n_AxisNum) ; //긴급정지 

				st_motor_info[n_AxisNum].n_sd_mv_chk = 0; //clear
				CTL_Lib.SD_Sensor_Enable(0, n_AxisNum, CTL_NO); //sd sensor clear

				m_nSD_Elv_MoveStep[n_AxisNum] = 2200;
				break;
			}
			nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_CHECK, n_AxisNum, st_motor_info[n_AxisNum].d_pos[P_ELV_SD_SENSOR], COMI.mn_runspeed_rate);  //2015.0407 james  //SD 위치까지 이동했는데 SD가 감지되지 않으면 트레이가 없는것이다
			if (nRet_1 == BD_GOOD)   
			{	//트레이가 없는 상태일것이다, 미리 트레이를 체크가능하여 트레이가 없으면 올라 갈 필요가 없다 
				//이곳에 오면 문제가 있음 
				if(n_TargetPos_Site == P_ELV_SUPPLY_OFFSET)//트레이를 작업 가능한 영역에 UP하여 공급하는 위치, SD pos 센서 On 감지 후 센서 기준 - 방항으로 벗어난 후 + 방향으로 P_ELV_SUPPLY_OFFSET 티칭만큼 up 한후 모터 동작은 완료하고, 트레이르, 받든다 
				{
					if(n_AxisNum == M_LD_ELV)//2016.0828 james 
					{
						nRet_1 = FAS_IO.get_in_bit(st_io_info.i_LdStkTrayChk,	IO_ON); //load stacker tray check    
						if(nRet_1 == IO_OFF)
						{
							m_nSD_Elv_MoveStep[n_AxisNum] = 0;
							nFuncRet = RET_GOOD;
						}
					}
					else
					{
						//james test m_nSD_Elv_MoveStep[n_AxisNum] = 1000; //트레이가 없는 상태  
						m_nSD_Elv_MoveStep[n_AxisNum] = 0;
						nFuncRet = RET_GOOD;
					}
				}			
				else if(n_TargetPos_Site == P_ELV_RECEIVE_OFFSET) //작업이 끝난 트레이를 받는 위치, SD pos 센서 On 감지 후 센서 기준 - 방항으로 센서를 벗어난 후 - 방향으로 P_ELV_RECEIVE_OFFSET 티칭만큼 down 한 후 트레이를 받는 위치  
				{				
					m_nSD_Elv_MoveStep[n_AxisNum] = 2300; //트레이가 없는 상태 				 
				}
			}
			else if (nRet_1 == BD_RETRY)
			{		 
				m_nSD_Elv_MoveStep[n_AxisNum] = 1000;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다  
				CTL_Lib.Alarm_Error_Occurrence(273, dWARNING, st_alarm_info.strCode);

				////////if (st_handler_info.cWndList != NULL)  
				{
					//clsMem.OnNormalMessageWrite(_T("UnLoader Alarm : 2010"));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG); 
				}
				m_nSD_Elv_MoveStep[n_AxisNum] = 1000;
			}
			break;

		case 2200:
			if(n_TargetPos_Site == P_ELV_SUPPLY_OFFSET)//트레이를 작업 가능한 영역에 UP하여 공급하는 위치, SD pos 센서 On 감지 후 센서 기준 - 방항으로 벗어난 후 + 방향으로 P_ELV_SUPPLY_OFFSET 티칭만큼 up 한후 모터 동작은 완료하고, 트레이르, 받든다 
			{
				nRet_1 = COMI.Get_MotIOSensor(n_AxisNum, MOT_SENS_SD); 
				if(nRet_1 != BD_GOOD)
				{//SD 센서가 감지되지 않으면 에러 
					COMI.Set_MotStop(1, n_AxisNum) ; //긴급정지 

					m_bSD_MoveFlag[n_AxisNum] = false;
					m_nSD_Elv_MoveStep[n_AxisNum] = 1000; //재시도  루틴  
					break;
				}
			}

			dCurrentPos = COMI.Get_MotCurrentPos(n_AxisNum); 
			if(n_TargetPos_Site == P_ELV_SUPPLY_OFFSET)//트레이를 작업 가능한 영역에 UP하여 공급하는 위치, SD pos 센서 On 감지 후 센서 기준 - 방항으로 벗어난 후 + 방향으로 P_ELV_SUPPLY_OFFSET 티칭만큼 up 한후 모터 동작은 완료하고, 트레이르, 받든다 
			{
				m_dTargetPos[n_AxisNum] = fabs(dCurrentPos + st_motor_info[n_AxisNum].d_pos[P_ELV_SUPPLY_OFFSET]); //현재 위치에 + P_ELV_SUPPLY_OFFSET 
			}
			else if(n_TargetPos_Site == P_ELV_RECEIVE_OFFSET) //트레이를 받는 동작시 
			{
				m_dTargetPos[n_AxisNum] = fabs(dCurrentPos - st_motor_info[n_AxisNum].d_pos[P_ELV_RECEIVE_OFFSET]); //셋팅한 위치까지 더 내린다 sd 센서가 감지 안될때까지 
			}

			nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_START, n_AxisNum, m_dTargetPos[n_AxisNum], COMI.mn_runspeed_rate);   
			if (nRet_1 == BD_GOOD) //조건에 따라 위로/아래로  이동  
			{
				st_sync_info.nSmema_Tray_Output_Req = 0;
				m_nSD_Elv_MoveStep[n_AxisNum] = 2210;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nSD_Elv_MoveStep[n_AxisNum] = 1000;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다  
				st_sync_info.nSmema_Tray_Output_Req++;
				if( st_sync_info.nSmema_Tray_Output_Req > 3)
				{
					st_sync_info.nSmema_Tray_Output_Req = 0;
					CTL_Lib.Alarm_Error_Occurrence(274, dWARNING, st_alarm_info.strCode);

					if (st_handler_info.cWndList != NULL)  
					{
						//clsMem.OnNormalMessageWrite(_T("UnLoader Alarm : 2100"));
						//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG); 
					}
				}
				m_nSD_Elv_MoveStep[n_AxisNum] = 1000;
			}
			break;

		case 2210:
			nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_CHECK, n_AxisNum, m_dTargetPos[n_AxisNum], COMI.mn_runspeed_rate);  //2015.0407 james  //SD 위치까지 이동했는데 SD가 감지되지 않으면 트레이가 없는것이다
			if (nRet_1 == BD_GOOD)   
			{	//트레이가 없는 상태일것이다, 미리 트레이를 체크가능하여 트레이가 없으면 올라 갈 필요가 없다 
				//이곳에 오면 문제가 있음 
				m_nSD_Elv_MoveStep[n_AxisNum] = 2300; //트레이가 있는 상태 				 
			}
			else if (nRet_1 == BD_RETRY)
			{		 
				m_nSD_Elv_MoveStep[n_AxisNum] = 1000;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다  
				//2016.1117
				st_sync_info.nSmema_Tray_Output_Req++;
				if( st_sync_info.nSmema_Tray_Output_Req > 3)
				{
					st_sync_info.nSmema_Tray_Output_Req = 0;
					CTL_Lib.Alarm_Error_Occurrence(275, dWARNING, st_alarm_info.strCode);

					if (st_handler_info.cWndList != NULL)  
					{
						strlog.Format(_T("Motor:%d Target: %.3f Feedback: %.3f"), n_AxisNum, m_dTargetPos[n_AxisNum], COMI.Get_MotCurrentPos(n_AxisNum));
						clsMem.OnAbNormalMessagWrite(strlog);
					}
				}

				m_nSD_Elv_MoveStep[n_AxisNum] = 1000;
			}
			break;

		case 2300:
			if(n_TargetPos_Site == P_ELV_SUPPLY_OFFSET)//트레이를 작업 가능한 영역에 UP하여 공급하는 위치, SD pos 센서 On 감지 후 센서 기준 - 방항으로 벗어난 후 + 방향으로 P_ELV_SUPPLY_OFFSET 티칭만큼 up 한후 모터 동작은 완료하고, 트레이르, 받든다 
			{
				nRet_1 = COMI.Get_MotIOSensor(n_AxisNum, MOT_SENS_SD); 
				if(nRet_1 == BD_GOOD)
				{//정상적으로 체크했다. 					
					m_nSD_Elv_MoveStep[n_AxisNum] = 2400; // 루틴   
				}
				else
				{//error
					m_nSD_Elv_MoveStep[n_AxisNum] = 1000; //재시도  루틴   
				}
			}			
			else if(n_TargetPos_Site == P_ELV_RECEIVE_OFFSET) //작업이 끝난 트레이를 받는 위치, SD pos 센서 On 감지 후 센서 기준 - 방항으로 센서를 벗어난 후 - 방향으로 P_ELV_RECEIVE_OFFSET 티칭만큼 down 한 후 트레이를 받는 위치  
			{
				nRet_1 = COMI.Get_MotIOSensor(n_AxisNum, MOT_SENS_SD); 
				if(nRet_1 == BD_ERROR)
				{//정상적으로 감지되지 않았다. 					
					m_nSD_Elv_MoveStep[n_AxisNum] = 2400; // 루틴   
				}
				else
				{//error
					m_nSD_Elv_MoveStep[n_AxisNum] = 1000; //재시도  루틴   
				}
			}
			break;

		case 2400:
			m_nSD_Elv_MoveStep[n_AxisNum] = 2500;
			break;

		case 2500:
			dCurrentPos = COMI.Get_MotCurrentPos(n_AxisNum); 
			if(n_TargetPos_Site == P_ELV_SUPPLY_OFFSET)//트레이를 작업 가능한 영역에 UP하여 공급하는 위치, SD pos 센서 On 감지 후 센서 기준 - 방항으로 벗어난 후 + 방향으로 P_ELV_SUPPLY_OFFSET 티칭만큼 up 한후 모터 동작은 완료하고, 트레이르, 받든다 
			{
				m_dSD_Supply_Pos_Backup[n_AxisNum] = fabs(dCurrentPos + st_motor_info[n_AxisNum].d_pos[P_ELV_SUPPLY_OFFSET]); //현재 위치에 + P_ELV_SUPPLY_OFFSET 
			}
			else if(n_TargetPos_Site == P_ELV_RECEIVE_OFFSET) //트레이를 받는 동작시 
			{
				m_dSD_Receive_Pos_Backup[n_AxisNum] = fabs(dCurrentPos - st_motor_info[n_AxisNum].d_pos[P_ELV_RECEIVE_OFFSET]); //셋팅한 위치까지 더 내린다 sd 센서가 감지 안될때까지 
			} 

			//m_dReference_Pos_Backup

			CTL_Lib.SD_Sensor_Enable(0, n_AxisNum, CTL_NO); //sd sensor clear
			m_nSD_Elv_MoveStep[n_AxisNum] = 0;
			nFuncRet = RET_GOOD;
			break; 

		//
		////////////////////////////////////////////////////////////
		//SD 센서를 감지하고 있으니 아래로 내려가자 
		////////////////////////////////////////////////////////////	
		case 6000: 
			if(m_bSD_MoveFlag[n_AxisNum] == false)
			{
				m_dwSdWaitTime[n_AxisNum][0] = GetTickCount();
				m_bSD_MoveFlag[n_AxisNum] = true;
				CTL_Lib.SD_Sensor_Enable(0, n_AxisNum, CTL_NO); //sd sensor set
			}
			else if(m_bSD_MoveFlag[n_AxisNum] == true)
			{
				m_dwSdWaitTime[n_AxisNum][1] = GetTickCount();
				m_dwSdWaitTime[n_AxisNum][2] = m_dwSdWaitTime[n_AxisNum][1] - m_dwSdWaitTime[n_AxisNum][0];
				if(m_dwSdWaitTime[n_AxisNum][2] <= 0)
				{
					m_dwSdWaitTime[n_AxisNum][0] = GetTickCount();
					break;
				}
				if(m_dwSdWaitTime[n_AxisNum][2] > MOT_ELE_MOVE_WAITLIMIT)
				{//limit 시간 지남, 에러 처리후 재 시도하자 
					COMI.Set_MotStop(1, n_AxisNum) ; //긴급정지 
					m_nSD_Elv_MoveStep[n_AxisNum] = 1000;
					break;
				}
			}

			nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_START, n_AxisNum, st_motor_info[n_AxisNum].d_pos[P_ELV_DN_LIMIT], COMI.mn_runspeed_rate/3);   //밑으로 이동한다 
			if (nRet_1 == BD_GOOD) //위로 이동 하면서 SD를 찾는다  
			{					 				 
				m_nSD_Elv_MoveStep[n_AxisNum] = 6100;
			}
			else if (nRet_1 == BD_RETRY)
			{				 
				m_nSD_Elv_MoveStep[n_AxisNum] = 1000;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다  
				CTL_Lib.Alarm_Error_Occurrence(375, dWARNING, st_alarm_info.strCode);

				if (st_handler_info.cWndList != NULL)  
				{
					//clsMem.OnNormalMessageWrite(_T("UnLoader Alarm : 2000"));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG); 
				}
				m_nSD_Elv_MoveStep[n_AxisNum] = 1000;
			}
			break;

		case 6100:
			nRet_1 = COMI.Get_MotIOSensor(n_AxisNum, MOT_SENS_SD); 
			if(nRet_1 == BD_ERROR)
			{//정상적으로 체크했다. 
				COMI.Set_MotStop(1, n_AxisNum) ; //긴급정지 			
				CTL_Lib.SD_Sensor_Enable(0, n_AxisNum, CTL_NO); //sd sensor clear

				m_nSD_Elv_MoveStep[n_AxisNum] = 7000;
				break;
			}
			nRet_1 = CTL_Lib.Single_Move(ONLY_MOVE_CHECK, n_AxisNum, st_motor_info[n_AxisNum].d_pos[P_ELV_DN_LIMIT], COMI.mn_runspeed_rate);  //2015.0407 james  //SD 위치까지 이동했는데 SD가 감지되지 않으면 트레이가 없는것이다
			if (nRet_1 == BD_GOOD)   
			{	 
				m_nSD_Elv_MoveStep[n_AxisNum] = 6200;  //완전히 내렸으나 아직도 SD룰 감지하고 있는 상태로 , TRAY FULL 상태이다 				 
			}
			else if (nRet_1 == BD_RETRY)
			{		 
				m_nSD_Elv_MoveStep[n_AxisNum] = 1000;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다  
				CTL_Lib.Alarm_Error_Occurrence(276, dWARNING, st_alarm_info.strCode);

				////////if (st_handler_info.cWndList != NULL)  
				{
					//clsMem.OnNormalMessageWrite(_T("UnLoader Alarm : 2010"));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG); 
				}
				m_nSD_Elv_MoveStep[n_AxisNum] = 1000;
			}
			break;

		case 6200:
			/////////////////////////////////////////////////////////////////////
			// TRAY FULL Error
			/////////////////////////////////////////////////////////////////////
			nFuncRet = RET_ABORT; // james s2018.0828
			//m_nSD_Elv_MoveStep[n_AxisNum] = 0; // james s2018.0828 1000;
			m_nSD_Elv_MoveStep[n_AxisNum] = 6100; // kwlee 2016.1227
			
			if (st_handler_info.cWndMain != NULL)
			{
				st_other_info.nBuzYesNo = YES;
				if( n_AxisNum == M_EMPTY_ELV )
				{
					st_other_info.strBoxMsg = _T("[EMPTY TRAY FULL] Empty tray is Full. \r\n Please Remove it.");
				}
				else if( n_AxisNum == M_ULD_1_ELV )
				{
					st_other_info.strBoxMsg = _T("[UNLOAD TRAY#1 TRAY FULL] Unload Elv#1 tray is Full. \r\n Please Remove it.");
				}
				else if( n_AxisNum == M_ULD_2_ELV )
				{
					st_other_info.strBoxMsg = _T("[UNLOAD TRAY#2 FULL] Unload Elv#2 tray is Full. \r\n Please Remove it.");
				}
				else if( n_AxisNum == M_RJ_EMPTY_ELV )
				{
					st_other_info.strBoxMsg = _T("[REJECT EMPTY TRAY FULL] Reject empty tray is Full. \r\n Please Remove it.");
				}
				else if( n_AxisNum == M_RJ_ULD_1_ELV )
				{
					st_other_info.strBoxMsg = _T("[REJECT TRAY#1 FULL] Reject tray#1 is Full. \r\n Please Remove it.");
				}
				else if( n_AxisNum == M_RJ_ULD_2_ELV )
				{
					st_other_info.strBoxMsg = _T("[REJECT TRAY#2 FULL] Reject tray#2 is Full. \r\n Please Remove it.");
				}
				else
				{
					st_other_info.strBoxMsg = _T("[TRAY FULL] Warnning : Tray is Full. \r\n Please Remove it.");
				}
				st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
				//kwlee 2016.1227
				clsMem.OnAbNormalMessagWrite(st_other_info.strBoxMsg);
				clsFunc.OnMCStop();
			}

			break;


		case 7000:
			nRet_1 = COMI.Get_MotIOSensor(n_AxisNum, MOT_SENS_SD); 
			if(nRet_1 == BD_ERROR)
			{//SD 센서가 감지되지 않으면 정상적으로 내려운 상태  
				COMI.Set_MotStop(1, n_AxisNum) ; //긴급정지 

				m_bSD_MoveFlag[n_AxisNum] = false;
				m_nSD_Elv_MoveStep[n_AxisNum] = 1000; //이제 다시 위로 올라가 정해진 루틴으로 SD 기준 위치에 맞춘다    
			}
			else
			{
				m_nSD_Elv_MoveStep[n_AxisNum] = 1000; //이제 다시 위로 올라가 정해진 루틴으로 SD 기준 위치에 맞춘다    
			}
			break;
	}	 

	if(m_nSD_Elv_MoveStep[n_AxisNum] == 0 || nFuncRet == RET_GOOD)
	{
		CTL_Lib.SD_Sensor_Enable(0, n_AxisNum, CTL_NO); //sd sensor clear
		m_nSd_Motor_MoveStart_Flag[n_AxisNum] = 0; //모터 이동 클리어 
	}

	return nFuncRet;
}


// james 2016.0802 conveyor의 ac motor ON/OFF만 제어한다, 다른 IO 는 쓰레드에서 제어 
void CCtlBd_Library::Process_Tray_Conveyor_AcMotor(int n_Mode, int n_THD_Site, int n_AcMotor_IO_Num)
{
	int nRet_1=0, nRet_2=0;	 

	//if(1 || n_AcMotor_IO_Num != 2) 
	//{
	//	return;
	//}

	 
	//LIGHT CURTAIN
	if( st_sync_info.nLdUld_LightChk == YES )
	{
		if( n_THD_Site == THD_LD_CV_STACKER_LEFT || n_THD_Site == THD_LD_CV_STACKER_RIGHT || n_THD_Site == THD_LD_STACKER ||
			n_THD_Site == THD_ULD_1_STACKER || n_THD_Site == THD_ULD_2_STACKER )
		{
			if( st_sync_info.nConveyor_AcMotorCtl_Req[n_THD_Site] == CTL_CHANGE )
			{
				st_sync_info.nConveyor_AcMotorCtl_Req[n_THD_Site] = CTL_REQ;
				////return;
			}
		}
	}
	else if( st_sync_info.nRej_LightChk == YES )
	{
		if( n_THD_Site == THD_REJECT_EMPTY_STACKER || n_THD_Site == THD_REJECT_OUT_1_STACKER || n_THD_Site == THD_REJECT_OUT_2_STACKER )
		{
			if( st_sync_info.nConveyor_AcMotorCtl_Req[n_THD_Site] == CTL_CHANGE )
			{
				st_sync_info.nConveyor_AcMotorCtl_Req[n_THD_Site] = CTL_REQ;
				//return;
			}
		}
	} 	
	////////////////////////////////////////////////////////////////////////////////////////////
	//ac motor 제어 
	//장비 STOP 및 작업자가 메뉴얼로 OFF 해도 지속 동작 가능하게 하기 위함 
	//동작 순서 플레그 : CTL_REQ(동작 요청) - > CTL_READY(동작 완료) , CTL_FREE(바로 정지 요청
	////////////////////////////////////////////////////////////////////////////////////////////
	if(st_sync_info.nConveyor_AcMotorCtl_Req[n_THD_Site] == CTL_REQ)
	{
		//james 2016.0923
		FAS_IO.set_out_bit(n_AcMotor_IO_Num, IO_ON); //AC Motor on		 

		m_bAcMotorFlag[n_THD_Site] = true;
		m_dwAcMotorWaitTime[n_THD_Site][0] = GetCurrentTime();

		st_sync_info.nConveyor_AcMotorCtl_Req[n_THD_Site] = CTL_CHANGE;
	}
	else if(st_sync_info.nConveyor_AcMotorCtl_Req[n_THD_Site] == CTL_FREE) //모터를 바로 정지 시킨다 
	{
		st_sync_info.nConveyor_AcMotorCtl_Req[n_THD_Site] = CTL_CLEAR; //2016.0918 

		m_bAcMotorFlag[n_THD_Site] = false;
		FAS_IO.set_out_bit(n_THD_Site, IO_OFF); //AC Motor off
		 

	}
	else if(st_sync_info.nConveyor_AcMotorCtl_Req[n_THD_Site] == CTL_READY)
	{ //조건이 완료되어 AC Motor off flag가 와도 셋팅 및 안정을 위해 5초동안 계속 돌린다 
		m_dwAcMotorWaitTime[n_THD_Site][1] = GetCurrentTime();
		m_dwAcMotorWaitTime[n_THD_Site][2] = m_dwAcMotorWaitTime[n_THD_Site][1] - m_dwAcMotorWaitTime[n_THD_Site][0];
		if(m_dwAcMotorWaitTime[n_THD_Site][2] <= 0) m_dwAcMotorWaitTime[n_THD_Site][0] = GetCurrentTime();

		if(n_THD_Site == THD_ULD_1_STACKER || n_THD_Site == THD_ULD_2_STACKER || n_THD_Site == THD_REJECT_OUT_1_STACKER || n_THD_Site == THD_REJECT_OUT_2_STACKER)
		{//배출하는 트레이는 바로 컨베어를 멈추자
			if(m_bAcMotorFlag[n_THD_Site] == true && m_dwAcMotorWaitTime[n_THD_Site][2] > 100) //IO_CONVEYOR_WAIT_STABLE) //2초 동안은 조건이 해제되도 계속 돌린다. 킬착시키기 위해 
			{
				st_sync_info.nConveyor_AcMotorCtl_Req[n_THD_Site] = CTL_CLEAR; //2016.0918 
				m_bAcMotorFlag[n_THD_Site] = false;
				FAS_IO.set_out_bit(n_AcMotor_IO_Num, IO_OFF); //AC Motor off

			}
		}
		else
		{
			if(m_bAcMotorFlag[n_THD_Site] == true && m_dwAcMotorWaitTime[n_THD_Site][2] > 5000) //IO_CONVEYOR_WAIT_STABLE) //2초 동안은 조건이 해제되도 계속 돌린다. 킬착시키기 위해 
			{
				st_sync_info.nConveyor_AcMotorCtl_Req[n_THD_Site] = CTL_CLEAR; //2016.0918 
				m_bAcMotorFlag[n_THD_Site] = false;
				FAS_IO.set_out_bit(n_AcMotor_IO_Num, IO_OFF); //AC Motor off
			 
			}
		}
	}
}

// james 2016.0802 conveyor의 ac motor ON/OFF만 제어한다, 다른 IO 는 쓰레드에서 제어 
void CCtlBd_Library::Process_TestChamber_AcMotor(int n_Mode, int n_THD_Site, int n_AcMotor_IO_Num)
{
	int nRet_1=0, nRet_2=0;	  
	
	////////////////////////////////////////////////////////////////////////////////////////////
	//ac motor 제어 
	//장비 STOP 및 작업자가 메뉴얼로 OFF 해도 지속 동작 가능하게 하기 위함 
	//동작 순서 플레그 : CTL_REQ(동작 요청) - > CTL_READY(동작 완료) , CTL_FREE(바로 정지 요청
	////////////////////////////////////////////////////////////////////////////////////////////
	// jtkim 20161001
	/*
	if(st_test_site_info[n_THD_Site].nTester_Req[0] == CTL_REQ)
	{
		if(st_test_site_info[n_THD_Site].nTester_Req[1] == WORK_PICK) //open chamber
		{
			FAS_IO.set_out_bit(st_io_info.o_ChamberAcMotOpen[n_AcMotor_IO_Num], IO_ON); //AC Motor o_Chamber1AcMotOpen on
			FAS_IO.set_out_bit(st_io_info.o_ChamberAcMotCose[n_AcMotor_IO_Num], IO_OFF); //AC Motor o_Chamber1AcMotClose off
		}
		else if(st_test_site_info[n_THD_Site].nTester_Req[1] == WORK_PLACE) //close chamber
		{
			FAS_IO.set_out_bit(st_io_info.o_ChamberAcMotOpen[n_AcMotor_IO_Num], IO_OFF); //AC Motor o_Chamber1AcMotOpen off
			FAS_IO.set_out_bit(st_io_info.o_ChamberAcMotCose[n_AcMotor_IO_Num], IO_ON); //AC Motor o_Chamber1AcMotClose on
		}
		m_bAcMotorFlag[n_THD_Site] = true;
		m_dwAcMotorWaitTime[n_THD_Site][0] = GetCurrentTime();

		st_test_site_info[n_THD_Site].nTester_Req[0] = CTL_CHANGE;
	}
	else if(st_test_site_info[n_THD_Site].nTester_Req[0] == CTL_CHANGE) //실행 중 
	{
		if(st_test_site_info[n_THD_Site].nTester_Req[1] == WORK_PICK) //open chamber, 자재 test site 에서 집기전 실행 
		{
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ChamberOpenChk[n_AcMotor_IO_Num],	IO_ON);  
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ChamberCloseChk[n_AcMotor_IO_Num],	IO_OFF);  
			if(nRet_1 == IO_ON && nRet_2 == IO_OFF )
			{
				 st_test_site_info[n_THD_Site].nTester_Req[0] = CTL_READY;
			}
			else
			{// 알람은 로보트에서 체크하여 처리 하자, 이곳에서 하면 안됨 
				//m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ChamberOpenChk[n_AcMotor_IO_Num]); 
				//CTL_Lib.Alarm_Error_Occurrence(m_nRunStep, dWARNING, m_strAlarmCode);
			}
		}
		else if(st_test_site_info[n_THD_Site].nTester_Req[1] == WORK_PLACE) //close chamber, 자재 test site에 insert 후 진행
		{
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ChamberOpenChk[n_AcMotor_IO_Num],	IO_OFF);  
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ChamberCloseChk[n_AcMotor_IO_Num],	IO_ON);  
			if(nRet_1 == IO_OFF && nRet_2 == IO_ON )
			{
				 st_test_site_info[n_THD_Site].nTester_Req[0] = CTL_READY;
			}
			else
			{// 알람은 로보트에서 체크하여 처리 하자, 이곳에서 하면 안됨 
				//m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ChamberOpenChk[n_AcMotor_IO_Num]); 
				//CTL_Lib.Alarm_Error_Occurrence(m_nRunStep, dWARNING, m_strAlarmCode);
			}
		}
		 
	}
	else if(st_test_site_info[n_THD_Site].nTester_Req[0] == CTL_FREE) //open chamber
	{
		m_bAcMotorFlag[n_THD_Site] = false;
		FAS_IO.set_out_bit(st_io_info.o_ChamberAcMotOpen[n_AcMotor_IO_Num], IO_ON); //AC Motor o_Chamber1AcMotOpen on
		FAS_IO.set_out_bit(st_io_info.o_ChamberAcMotCose[n_AcMotor_IO_Num], IO_OFF); //AC Motor o_Chamber1AcMotClose off
	}
	else if(st_test_site_info[n_THD_Site].nTester_Req[0] == CTL_READY) //off
	{ //조건이 완료되어 AC Motor off flag가 와도 셋팅 및 안정을 위해 5초동안 계속 돌린다 
		m_dwAcMotorWaitTime[n_THD_Site][1] = GetCurrentTime();
		m_dwAcMotorWaitTime[n_THD_Site][2] = m_dwAcMotorWaitTime[n_THD_Site][1] - m_dwAcMotorWaitTime[n_THD_Site][0];
		if(m_dwAcMotorWaitTime[n_THD_Site][2] < 0) m_dwAcMotorWaitTime[n_THD_Site][0] = GetCurrentTime();
		
		//if(m_bAcMotorFlag[n_THD_Site] == true && m_dwAcMotorWaitTime[n_THD_Site][2] > IO_CONVEYOR_WAIT_STABLE) //2초 동안은 조건이 해제되도 계속 돌린다. 킬착시키기 위해 
		if(m_bAcMotorFlag[n_THD_Site] == true && m_dwAcMotorWaitTime[n_THD_Site][2] > 500) //2초 동안은 조건이 해제되도 계속 돌린다. 킬착시키기 위해 
		{
			m_bAcMotorFlag[n_THD_Site] = false;

			FAS_IO.set_out_bit(st_io_info.o_ChamberAcMotOpen[n_AcMotor_IO_Num], IO_OFF); //AC Motor o_Chamber1AcMotOpen off
			FAS_IO.set_out_bit(st_io_info.o_ChamberAcMotCose[n_AcMotor_IO_Num], IO_OFF); //AC Motor o_Chamber1AcMotClose on//AC Motor off
		}
	}
	*/

	if(st_test_site_info[n_THD_Site].nTester_Req[0] == CTL_REQ)
	{
		if(st_test_site_info[n_THD_Site].nTester_Req[1] == WORK_PICK) //open chamber
		{
			m_bAcMotorFlag[n_THD_Site] = false;

			FAS_IO.set_out_bit(st_io_info.o_ChamberAcMotOpen[n_AcMotor_IO_Num], IO_ON); //AC Motor o_Chamber1AcMotOpen on
			FAS_IO.set_out_bit(st_io_info.o_ChamberAcMotCose[n_AcMotor_IO_Num], IO_OFF); //AC Motor o_Chamber1AcMotClose off
		}
		else if(st_test_site_info[n_THD_Site].nTester_Req[1] == WORK_PLACE) //close chamber
		{
			m_bAcMotorFlag[n_THD_Site] = false;

			FAS_IO.set_out_bit(st_io_info.o_ChamberAcMotOpen[n_AcMotor_IO_Num], IO_OFF); //AC Motor o_Chamber1AcMotOpen off
			FAS_IO.set_out_bit(st_io_info.o_ChamberAcMotCose[n_AcMotor_IO_Num], IO_ON); //AC Motor o_Chamber1AcMotClose on
		}
		st_test_site_info[n_THD_Site].nTester_Req[0] = CTL_CLEAR; //kwlee 2016.1003
	}
	else if(st_test_site_info[n_THD_Site].nTester_Req[0] == CTL_FREE)
	{
		FAS_IO.set_out_bit(st_io_info.o_ChamberAcMotOpen[n_AcMotor_IO_Num], IO_OFF); //AC Motor o_Chamber1AcMotOpen off
		FAS_IO.set_out_bit(st_io_info.o_ChamberAcMotCose[n_AcMotor_IO_Num], IO_OFF); //AC Motor o_Chamber1AcMotClose on//AC Motor off

		st_test_site_info[n_THD_Site].nTester_Req[0] = CTL_CLEAR; 
	}
	else 
	{
		if (st_handler_info.nRunStatus != dRUN)
		{
			if (m_bAcMotorFlag[n_THD_Site] == true)
			{
				m_dwAcMotorWaitTime[n_THD_Site][1] = GetCurrentTime();
				m_dwAcMotorWaitTime[n_THD_Site][2] = m_dwAcMotorWaitTime[n_THD_Site][1] - m_dwAcMotorWaitTime[n_THD_Site][0];
				if(m_dwAcMotorWaitTime[n_THD_Site][2] <= 0) 
				{
					m_dwAcMotorWaitTime[n_THD_Site][0] = GetCurrentTime();
					return;
				}
		
				if(m_dwAcMotorWaitTime[n_THD_Site][2] > 100)
				{
					m_bAcMotorFlag[n_THD_Site] = false;

					FAS_IO.set_out_bit(st_io_info.o_ChamberAcMotOpen[n_AcMotor_IO_Num], IO_OFF); //AC Motor o_Chamber1AcMotOpen off
					FAS_IO.set_out_bit(st_io_info.o_ChamberAcMotCose[n_AcMotor_IO_Num], IO_OFF); //AC Motor o_Chamber1AcMotClose on//AC Motor off
				}
			}
			else
			{
				if (st_test_site_info[n_THD_Site].nTester_Req[1] == WORK_PICK)
				{
					nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ChamberOpenChk[n_AcMotor_IO_Num],	IO_ON);  
					nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ChamberCloseChk[n_AcMotor_IO_Num],	IO_OFF);  
			
					if(nRet_1 == IO_ON && nRet_2 == IO_OFF )
					{
						m_bAcMotorFlag[n_THD_Site] = true;
						m_dwAcMotorWaitTime[n_THD_Site][0] = GetCurrentTime();
					}
				}
				else if (st_test_site_info[n_THD_Site].nTester_Req[1] == WORK_PLACE)
				{
					nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ChamberOpenChk[n_AcMotor_IO_Num],	IO_OFF);  
					nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ChamberCloseChk[n_AcMotor_IO_Num],	IO_ON);  

					if(nRet_1 == IO_OFF && nRet_2 == IO_ON )
					{
						m_bAcMotorFlag[n_THD_Site] = true;
						m_dwAcMotorWaitTime[n_THD_Site][0] = GetCurrentTime();
					}
				}
			}
		}
	}
}