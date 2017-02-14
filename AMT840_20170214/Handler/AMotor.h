#pragma once

// AMotor 명령 대상입니다.
enum EMOTRET					// enum_Motor_Result
{
	MOTRET_ERROR = -1,
	MOTRET_READY = 0,
	MOTRET_PROCEED,
	MOTRET_GOOD,
	MOTRET_RETRY,
	MOTRET_SAFETY,
};

enum EMOTMOVE_MODE				// enum_Motor_Move_Mode
{
	MMMODE_NONE,
	MMMODE_MANUAL,
	MMMODE_DISTANCE,
	MMMODE_HOME,

	MMMODE_WITH_IO_MOVE,
};

enum EMOTMOVE_STEP				//enum_Motor_Move_Step
{
	MMSTEP_NONE,

	// MOVE
	MMSTEP_MOVE_START,
	MMSTEP_MOVE_CHECK,
	MMSTEP_MOVE_FINISH,

	// WORK ROBOT LINEAR
	MMSTEP_WLINEAR_PICKER_UP,
	MMSTEP_WLINEAR_PICKER_UP_CHK,
	MMSTEP_WLINEAR_SAFETY,
	MMSTEP_WLINEAR_SAFETY_CHK,
	MMSTEP_WLINEAR_MDL_CALC,
	MMSTEP_WLINEAR_MDL_START,
	MMSTEP_WLINEAR_MDL_CHK,
	MMSTEP_WLINEAR_START,
	MMSTEP_WLINEAR_CHK,
	MMSTEP_WLINEAR_FINISH,

	// TEST ROBOT LINEAR
	MMSTEP_TLINEAR_PICKER_UP,
	MMSTEP_TLINEAR_PICKER_UP_CHK,
	MMSTEP_TLINEAR_SAFETY,
	MMSTEP_TLINEAR_SAFETY_CHK,
	MMSTEP_TLINEAR_MDL_CALC,
	MMSTEP_TLINEAR_MDL_START,
	MMSTEP_TLINEAR_MDL_CHK,
	MMSTEP_TLINEAR_START,
	MMSTEP_TLINEAR_CHK,
	MMSTEP_TLINEAR_FINISH,

	// HOME
	MMSTEP_HOME_HOMECHECK,

	// STOP
	MMSTEP_STOP,
	MMSTEP_STOP_FINISH,
	
	// FAIL
	MMSTEP_MOVE_FAIL,
	MMSTEP_STOP_FAIL,
};

class AMotor : public CObject
{
public:
	AMotor();
	virtual ~AMotor();


protected:
	void MmStep_Move_Start();
	void MmStep_Move_Check();
	void MmStep_Move_Finish();
	EMOTRET MmStep_Home_HomeCheck();
	void MmStep_Stop();
	void MmStep_Stop_Finish();
	void MmStep_Move_Fail();
	void MmStep_Stop_Fail();

	void MmStep_WLinear_Safety();
	void MmStep_WLinear_Safety_Chk();
	void MmStep_WLinear_Mdl_Calc();
	void MmStep_WLinear_Mdl_Start();
	void MmStep_WLinear_Mdl_Chk();
	void MmStep_WLinear_Start();
	void MmStep_WLinear_Chk();
	void MmStep_WLinear_Finish();

	void MmStep_TLinear_Safety();
	void MmStep_TLinear_Safety_Chk();
	void MmStep_TLinear_Mdl_Calc();
	void MmStep_TLinear_Mdl_Start();
	void MmStep_TLinear_Mdl_Chk();
	void MmStep_TLinear_Start();
	void MmStep_TLinear_Chk();
	void MmStep_TLinear_Finish();


	void _repeat_Finish();
	void _finish_manual();

public:
	int  m_nMotRetry_Cnt[100];
	void SetMotorID( int iId ) { m_nMotorId = iId; }
	EMOTRET MotorMoving_repeat();

	void SetMotGotoPos( double dVal ) { m_dMotGotoPos = dVal; }
	void MotorStepClear();
	void MotorMoveStart( double dPos, double dSpeed, EMOTMOVE_MODE motMoveMode = MMMODE_NONE );
	void MotorHomeStart();

	void MotorWLinearStart( double dPosX, double dPosY, double dSpeed, EMOTMOVE_MODE motMoveMode = MMMODE_NONE );
	void MotorTLinearStart( double dPosX, double dPosY, double dSpeed, EMOTMOVE_MODE motMoveMode = MMMODE_NONE );
	

	void SetStopReq( bool bVal ) { m_bStopReq = bVal; }

	EMOTRET JogMoveStart( long lDir );

	static bool CheckTeachSafety( int nAxis, int nPos, double dVal );

protected:
	EMOTMOVE_STEP	m_step;

	bool			m_bStopReq;

	int				m_nMotorId;

	double			m_dMotGotoPos;
	double			m_dMotGotoSpeed;
	
	EMOTMOVE_MODE	m_MotMoveMode;

	double			m_dWLinearPos[2];
	double			m_dTLinearPos[2];
};


