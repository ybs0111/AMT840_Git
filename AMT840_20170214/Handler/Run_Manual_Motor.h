#pragma once

// CRun_Manual_Motor 명령 대상입니다.

#include "Variable.h"
#include "AMotor.h"

#define GETMOTOR( motorID )		g_Run_Motors.GetMotor( motorID )


class CRun_Manual_Motor : public CObject
{
public:
	CRun_Manual_Motor();
	virtual ~CRun_Manual_Motor();

public:
	void Thread_Run();
//	AMotor& GetMotor( ENUM_MOTOR motorID ) { return m_motor[motorID]; }

protected:
	AMotor m_motor[M_MAX_MOT_NUM];
};


extern CRun_Manual_Motor	g_Run_Motors;


