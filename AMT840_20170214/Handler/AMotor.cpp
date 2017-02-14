// AMotor.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "AMotor.h"

#include "ComizoaPublic.h"
#include "Cmmsdk.h"
#include "RunLdUldRobot.h"
#include "Variable.h"
#include "Ctlbd_Variable.h"


// AMotor

AMotor::AMotor()
{
	m_step = MMSTEP_NONE;
	m_bStopReq = false;
	m_MotMoveMode = MMMODE_NONE;

	m_dMotGotoPos = 0;
	m_dMotGotoSpeed = 1;

	m_nMotorId = -1;
}

AMotor::~AMotor()
{
}

// AMotor 멤버 함수


EMOTRET AMotor::MotorMoving_repeat()
{
	EMOTRET ret = MOTRET_PROCEED;
	int nRet_1 = 0;

	if( m_bStopReq )
	{
		if( m_step < MMSTEP_STOP )
		{
			m_step = MMSTEP_STOP;
		}

		switch( m_step )
		{
		case MMSTEP_NONE:																	break;

		case MMSTEP_MOVE_START:					MmStep_Move_Start();						break;
		case MMSTEP_MOVE_CHECK:					MmStep_Move_Check();						break;
		case MMSTEP_MOVE_FINISH:				MmStep_Move_Finish();	ret = MOTRET_GOOD;	break;
		
		// WORK ROBOT LINEAR
		case MMSTEP_WLINEAR_PICKER_UP:	
			clsRunLdUldRobot.Set_WorkRbtPicker_UpDown(IO_OFF);
			m_step = MMSTEP_WLINEAR_PICKER_UP_CHK;
			break;
		case MMSTEP_WLINEAR_PICKER_UP_CHK:
			nRet_1 = clsRunLdUldRobot.Chk_WorkRbtPicker_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_step = MMSTEP_WLINEAR_SAFETY;
			}
			else if(nRet_1 == RET_ERROR)
			{
				m_step = MMSTEP_MOVE_FAIL;
			}
			break;
			break;
		case MMSTEP_WLINEAR_SAFETY:				MmStep_WLinear_Safety();						break;
		case MMSTEP_WLINEAR_SAFETY_CHK:			MmStep_WLinear_Safety_Chk();					break;

		case MMSTEP_WLINEAR_MDL_CALC:
			break;
		case MMSTEP_WLINEAR_MDL_START:
			break;
		case MMSTEP_WLINEAR_MDL_CHK:
			break;
		case MMSTEP_WLINEAR_START:
			break;
		case MMSTEP_WLINEAR_CHK:
			break;
		case MMSTEP_WLINEAR_FINISH:
			break;

	// TEST ROBOT LINEAR
		case MMSTEP_TLINEAR_PICKER_UP:
			break;
		case MMSTEP_TLINEAR_PICKER_UP_CHK:
			break;
		case MMSTEP_TLINEAR_SAFETY:
			break;
		case MMSTEP_TLINEAR_SAFETY_CHK:
			break;
		case MMSTEP_TLINEAR_MDL_CALC:
			break;
		case MMSTEP_TLINEAR_MDL_START:
			break;
		case MMSTEP_TLINEAR_MDL_CHK:
			break;
		case MMSTEP_TLINEAR_START:
			break;
		case MMSTEP_TLINEAR_CHK:
			break;
		case MMSTEP_TLINEAR_FINISH:
			break;

		// HOME
		case MMSTEP_HOME_HOMECHECK:
			break;

		// STOP
		case MMSTEP_STOP:
			break;
		case MMSTEP_STOP_FINISH:
			break;
	
		// FAIL
		case MMSTEP_MOVE_FAIL:			MmStep_Move_Fail();		ret = MOTRET_ERROR;		break;
		case MMSTEP_STOP_FAIL:			MmStep_Stop_Fail();		ret = MOTRET_ERROR;		break; 
			break;

		}
	}
	return ret;
}

void AMotor::MotorStepClear()
{
	m_bStopReq = false;
	m_step = MMSTEP_NONE;
	m_nMotRetry_Cnt[m_nMotorId] = 0;
}

void AMotor::MotorMoveStart( double dPos, double dSpeed, EMOTMOVE_MODE motMoveMode )
{
	m_dMotGotoPos = dPos;
	m_dMotGotoSpeed = dSpeed;
	m_MotMoveMode = motMoveMode;

	m_step = MMSTEP_MOVE_START;
}

void AMotor::MmStep_Move_Start()
{
	int nRet = COMI.Start_SingleMove( m_nMotorId, m_dMotGotoPos, (int)m_dMotGotoSpeed );
	if( nRet == MOTRET_GOOD )
	{
		m_step = MMSTEP_MOVE_CHECK;
	}
	else if( nRet == MOTRET_ERROR || nRet == MOTRET_SAFETY )
	{
		m_step = MMSTEP_MOVE_FAIL;
	}
	else if( nRet == MOTRET_RETRY )
	{
		m_nMotRetry_Cnt[m_nMotorId]++;
		if( m_nMotRetry_Cnt[m_nMotorId] > 3)
		{
			m_step = MMSTEP_MOVE_FAIL;
		}
	}
}

void AMotor::MmStep_Move_Check()
{
	int nRet = COMI.Check_SingleMove( m_nMotorId, m_dMotGotoPos );
	if( nRet == MOTRET_GOOD )
	{
		m_step = MMSTEP_MOVE_FINISH;
	}
	else if( nRet == MOTRET_ERROR || nRet == MOTRET_SAFETY )
	{
		m_step = MMSTEP_MOVE_FAIL;
	}
	else if( nRet == MOTRET_RETRY )
	{
		m_nMotRetry_Cnt[m_nMotorId]++;
		if( m_nMotRetry_Cnt[m_nMotorId] > 3)
		{
			m_step = MMSTEP_MOVE_FAIL;
		}
	}

}

void AMotor::MmStep_Move_Finish()
{
	_repeat_Finish();
	_finish_manual();
}

void AMotor::MmStep_WLinear_Safety()
{
	int nRet = COMI.Start_SingleMove( M_WORK_RBT_Z, st_motor_info[M_WORK_RBT_Z].d_pos[P_WORKROBOT_Z_SAFETY], (int)m_dMotGotoSpeed );
	if (nRet == BD_GOOD)
	{
		m_nMotRetry_Cnt[m_nMotorId] = 0;
		m_step = MMSTEP_WLINEAR_SAFETY_CHK;
	}
	else if(nRet == BD_ERROR || nRet == BD_SAFETY)
	{
		m_step = MMSTEP_MOVE_FAIL;
	}
	else if (nRet == BD_RETRY)
	{
		m_nMotRetry_Cnt[m_nMotorId]++;
		if( m_nMotRetry_Cnt[m_nMotorId] > 3)
		{
			m_step = MMSTEP_MOVE_FAIL;
		}
	}
}

void AMotor::MmStep_WLinear_Safety_Chk()
{
	int nRet = COMI.Check_SingleMove( M_WORK_RBT_Z, st_motor_info[M_WORK_RBT_Z].d_pos[P_WORKROBOT_Z_SAFETY] );
	if( nRet == BD_GOOD )
	{
		m_step = MMSTEP_WLINEAR_MDL_CALC;
	}
	else if (nRet == BD_ERROR || nRet == BD_SAFETY)
	{
		m_step = MMSTEP_MOVE_FAIL;
	}
	else if (nRet == BD_RETRY)
	{
		m_nMotRetry_Cnt[m_nMotorId]++;
		if( m_nMotRetry_Cnt[m_nMotorId] > 3)
		{
			m_step = MMSTEP_MOVE_FAIL;
		}
	}
}

void AMotor::MmStep_WLinear_Mdl_Calc()
{
	m_step = MMSTEP_WLINEAR_MDL_START;

	double dPos[2];//, dMdl[2];

	dPos[0] = COMI.Get_MotCurrentPos( M_WORK_RBT_X );
	dPos[0] = COMI.Get_MotCurrentPos( M_WORK_RBT_Y );
	
	//일단 간섭 구간이 없음

}

void AMotor::MmStep_WLinear_Mdl_Start()
{
	//double dMdl[2];

}

void AMotor::_repeat_Finish()
{
	if( m_bStopReq )
	{
		m_bStopReq = false;
	}
	m_step = MMSTEP_NONE;
}

void AMotor::_finish_manual()
{
	if( m_MotMoveMode == MMMODE_MANUAL ||
		m_MotMoveMode == MMMODE_HOME )
	{
		m_MotMoveMode = MMMODE_NONE;
	}
}


void AMotor::MmStep_Move_Fail()
{
	_repeat_Finish();
	_finish_manual();
}

void AMotor::MmStep_Stop_Fail()
{
	_repeat_Finish();
	_finish_manual();
}


void AMotor::MotorWLinearStart( double dPosX, double dPosY, double dSpeed, EMOTMOVE_MODE motMoveMode )
{
	m_dWLinearPos[0] = dPosX;
	m_dWLinearPos[1] = dPosY;

	m_dMotGotoSpeed = dSpeed;
	m_MotMoveMode = motMoveMode;

	m_step = MMSTEP_WLINEAR_PICKER_UP;
}


void AMotor::MotorTLinearStart( double dPosX, double dPosY, double dSpeed, EMOTMOVE_MODE motMoveMode )
{
	m_dTLinearPos[0] = dPosX;
	m_dTLinearPos[1] = dPosY;

	m_dMotGotoSpeed = dSpeed;
	m_MotMoveMode = motMoveMode;

	m_step = MMSTEP_TLINEAR_PICKER_UP;
}

