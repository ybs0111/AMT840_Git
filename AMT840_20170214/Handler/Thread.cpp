///////////////////////////////////////////////////////////////////////////////
/* ************************************************************************* */
// FILE NAME : Thread.cpp
//
// Copyright (C) 2001 DEnT Corporation
// All rights reserved.
//
// CREATED BY  : Lee Kyu Il
// REVISION    :
//     1> 2000. 09. 14 : Created by Lee Kyu Il
// DESCRIPTION :
//     Implementation of the Thread and associated classes
//	   This code may be used in compiled form in any way you desire. This
//	   file may be redistributed unmodified by any means PROVIDING it is
//	   not sold for profit without the authors written consent, and
//	   providing that this notice and the authors name and all copyright
//	   notices remains intact.
//
//	   An email letting me know how you are using it would be nice as well.
/* ************************************************************************* */
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <iostream>

#include "afxmt.h"
#include "Thread.h"

//==================================================================//
#include "Variable.h"		// 전역 구조체
#include "MyJamData.h"		// 알람 발생 시 처리 클래스	
#include "RunTowerLamp.h"   // 타워 램프 제어 클래스
#include "RunReadIO.h"      // I/O 상태 읽기 클래스
#include "RunHandlerCheck.h"	// KEY BOX S/W 제어 클래스

// 시퀀스 과련 클래스 해더파일 추가할 것 . 
//
#include "ClientInterface.h"
#include "RunLdTrayInputShifterConveyor.h"
#include "RunLdTrayStackerElevator.h"
#include "RunLdTrayPlate.h"
#include "RunEmptyStackerElevator.h"
#include "RunUldGoodTrayLeftOutputElevator.h"
#include "RunUldGoodTrayLeftOutputConveyor.h"
#include "RunUldGoodTrayRightOutputElevator.h"
#include "RunUldGoodTrayRightOutputConveyor.h"
#include "RunEmptyTrayTransfer.h"
#include "RunRejectTrayInputConveyor.h"
#include "RunRejectTrayStackerElevator.h"
#include "RunRejectTrayFrontStackerElevator.h"
#include "RunRejectTrayFrontOutputConveyor.h"
#include "RunRejectTrayRearStackerElevator.h"
#include "RunRejectTrayRearOutputConveyor.h"
#include "RunRejectTrayTransfer.h"
#include "RunLdUldRobot.h"
#include "RunDvcLdBuffer.h"
#include "RunDvcUldBuffer.h"
#include "RunTestSiteRobot.h"
#include "RunTestSiteHead_8.h"
#include "RunTestSiteRetestBuffer_Left.h"
#include "RunTestSiteRetestBuffer_Right.h"
//------------------------------------------------------------------//

#include "ClientEcFirst.h"
#include "ClientInterface1.h"
#include "ClientInterface2.h"
#include "ClientInterface3.h"
#include "ClientInterface4.h"
#include "ClientInterface5.h"
#include "ClientInterface6.h"
#include "ClientInterface7.h"
#include "ClientInterface8.h"
#include "ServerInterface1.h"
#include "ServerInterface2.h"
#include "ServerInterface3.h"
#include "ServerInterface4.h"
#include "ServerInterface5.h"
#include "ServerInterface6.h"
#include "ServerInterface7.h"
#include "ServerInterface8.h"
#include "RunTestSitePart1.h"
#include "RunTestSitePart2.h"
#include "RunTestSitePart3.h"
#include "RunTestSitePart4.h"
#include "RunTestSitePart5.h"
#include "RunTestSitePart6.h"
#include "RunTestSitePart7.h"
#include "RunTestSitePart8.h"
#include "ClientEcFirst.h"
#include "ClientEcSecond.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CMyJamData	m_clsAlarm;

CWinThread *m_thread[40];
HANDLE m_thrHandle[40];


//==================================================================//
// 전역 쓰레드 선언
//==================================================================//
UINT OnThreadHandlerCheck(LPVOID  lParam);		// m_thread[1]
UINT OnThreadReadIO(LPVOID lParam);				// m_thread[0]
UINT OnThreadTowerLamp(LPVOID lParam);			// m_thread[2]
UINT OnThreadAlarmDisplay(LPVOID lParam);		// m_thread[3]
UINT OnThreadInterface(LPVOID lParam);							// m_thread[4]

///////////////////////////////////////////////////////////////////////////////
//각종 load/unload/reject 스태커 & 트랜스퍼  관련 
///////////////////////////////////////////////////////////////////////////////
UINT OnThreadLdTrayInputShifterConveyor(LPVOID lParam);			// m_thread[5]
UINT OnThreadLdTrayStackerElevator(LPVOID lParam);				// m_thread[6]
UINT OnThreadLdTrayPlate(LPVOID lParam);						// m_thread[7]
UINT OnThreadEmptyStackerElevator(LPVOID lParam);				// m_thread[8]
UINT OnThreadUldGoodTrayLeftOutputElevator(LPVOID lParam);		// m_thread[9]
UINT OnThreadUldGoodTrayLeftOutputConveyor(LPVOID lParam);		// m_thread[10]
UINT OnThreadUldGoodTrayRightOutputElevator(LPVOID lParam);		// m_thread[11]
UINT OnThreadUldGoodTrayRightOutputConveyor(LPVOID lParam);		// m_thread[12]
UINT OnThreadEmptyTrayTransfer(LPVOID lParam);		            // m_thread[13]
// Reject stacker 관련 
UINT OnThreadRejectTrayInputConveyor(LPVOID lParam);		        // m_thread[14]
UINT OnThreadRejectTrayStackerElevator(LPVOID lParam);		    // m_thread[15]
UINT OnThreadRejectTrayFrontStackerElevator(LPVOID lParam);		// m_thread[16]
UINT OnThreadRejectTrayFrontOutputConveyor(LPVOID lParam);		// m_thread[17]
UINT OnThreadRejectTrayRearStackerElevator(LPVOID lParam);		// m_thread[18]
UINT OnThreadRejectTrayRearOutputConveyor(LPVOID lParam);		// m_thread[19]
UINT OnThreadRejectTrayTransfer(LPVOID lParam);		            // m_thread[20]
//Robot 관련 쓰레드
UINT OnThreadLdUldRobot(LPVOID lParam);							// m_thread[21]
//load/unload buffer 관련
UINT OnThreadDvcLdBuffer(LPVOID lParam);							// m_thread[22]
UINT OnThreadDvcUldBuffer(LPVOID lParam);						// m_thread[23]

///////////////////////////////////////////////////////////////////////////////
//Test Site Robot  관련 
///////////////////////////////////////////////////////////////////////////////
UINT OnThreadTestSiteRobot(LPVOID lParam);						// m_thread[24]
UINT OnThreadTestSiteHead_8(LPVOID lParam);						// m_thread[25]
UINT OnThreadTestSiteRetestBuffer_Left(LPVOID lParam);			// m_thread[26]
UINT OnThreadTestSiteRetestBuffer_Right(LPVOID lParam);			// m_thread[27]
UINT OnThreadTestSiteTop(LPVOID	lParam);						// m_thread[28]
UINT OnThreadTestSiteBottom(LPVOID lParam);						// m_thread[29]

//------------------------------------------------------------------//


CMutex mutex;
int AllStop[2];  // 쓰레드 강제 멈춤 플래그


//==================================================================//
// I/O 입출력 상태 읽기 쓰레드
//==================================================================//
UINT OnThreadHandlerCheck(LPVOID lParam)
{
	CSingleLock sing(&mutex);
	
	while(!AllStop[0])
	{
		sing.Lock();
		
		clsHdChk.OnStartCheck();   
		clsHdChk.OnStopCheck();
		clsHdChk.OnResetCheck();
		clsHdChk.OnBuzzerCheck();
		clsHdChk.OnAutoCheck();
		clsHdChk.OnManualCheck();
		clsHdChk.OnLightCurtainCheck();
		clsHdChk.OnLdUldMuteOnOff();
		clsHdChk.OnRejectMuteOnOff();
// 		clsHdChk.OnFrontMuteOn();
// 		clsHdChk.OnFrontMuteOff();
// 		clsHdChk.OnRightMuteOn();
// 		clsHdChk.OnRightMuteOff();

		Sleep(0);
		sing.Unlock();
	}

	m_thrHandle[1] = NULL;
	return TRUE;
}

//==================================================================//
// I/O 입출력 상태 읽기 쓰레드
//==================================================================//
UINT OnThreadReadIO(LPVOID lParam)
{
	CSingleLock sing(&mutex);
	
	while(!AllStop[0])
	{
		sing.Lock();
		
		clsReadIO.OnThreadRun();
		
		Sleep(0);
		sing.Unlock();
	}

	m_thrHandle[0] = NULL;
	return TRUE;
}

//==================================================================//
// 타워 램프 상태 변경 쓰레드
//==================================================================//
UINT OnThreadTowerLamp(LPVOID lParam)
{
	CSingleLock sing(&mutex);

	while(!AllStop[0])
	{
		sing.Lock();

		clsTowerLamp.OnThreadRun();

		Sleep(0);
		sing.Unlock();
	}

	m_thrHandle[2] = NULL;
	return TRUE;
}

//==================================================================//
// 알람 출력 쓰레드
//==================================================================//
UINT OnThreadAlarmDisplay(LPVOID lParam)
{
	CSingleLock sing(&mutex);
	
	while(!AllStop[0])
	{
		sing.Lock();
		
		clsMyJamData.OnAlarmDisplay();  // 알람 발생한 경우 알람 화면 출력 함수
		
		Sleep(0);
		sing.Unlock();
	}

	m_thrHandle[3] = NULL;
	return TRUE;
}

//==================================================================//
// 통신 인터페이스  쓰레드
//==================================================================//
UINT OnThreadInterface(LPVOID lParam)			// m_thread[4]
{
	CSingleLock sing(&mutex);
	
	while(!AllStop[0])
	{
		sing.Lock();
		
		clsInterC1.OnCommunication();
		clsInterC2.OnCommunication();
		clsInterC3.OnCommunication();
		clsInterC4.OnCommunication();
		clsInterC5.OnCommunication();
		clsInterC6.OnCommunication();
		clsInterC7.OnCommunication();
		clsInterC8.OnCommunication();

		clsInterS1.OnCommunication();
		clsInterS2.OnCommunication();
		clsInterS3.OnCommunication();
		clsInterS4.OnCommunication();
		clsInterS5.OnCommunication();
		clsInterS6.OnCommunication();
		clsInterS7.OnCommunication();
		clsInterS8.OnCommunication();

		clsEcFirst.OnCommunication();
		clsEcSecond.OnCommunication();
		Sleep(0);
		sing.Unlock();
	}

	m_thrHandle[4] = NULL;
	return TRUE;
}

//==================================================================//
// Load Conveyor  쓰레드
//==================================================================//
UINT OnThreadLdTrayInputShifterConveyor(LPVOID lParam)		// m_thread[5]
{
	CSingleLock sing(&mutex);
	
	while(!AllStop[1])
	{
		sing.Lock();

		clsRunLdTrayInputShifterConveyor.OnThreadRun();
		
		Sleep(0);
		sing.Unlock();
	}

	m_thrHandle[5] = NULL;
	return TRUE;
}

//==================================================================//
// Load Stackser  쓰레드
//==================================================================//
UINT OnThreadLdTrayStackerElevator(LPVOID lParam)		// m_thread[6]
{
	CSingleLock sing(&mutex);
	
	while(!AllStop[1])
	{
		sing.Lock();
		clsRunLdTrayStackerElevator.OnThreadRun();
		
		Sleep(0);
		sing.Unlock();
	}

	m_thrHandle[6] = NULL;
	return TRUE;
}

//==================================================================//
// Empty Stackser  쓰레드
//==================================================================//
UINT OnThreadLdTrayPlate(LPVOID lParam)		// m_thread[7]
{
	CSingleLock sing(&mutex);
	
	while(!AllStop[1])
	{
		sing.Lock();
		clsRunLdTrayPlate.OnThreadRun();
		
		Sleep(0);
		sing.Unlock();
	}

	m_thrHandle[7] = NULL;
	return TRUE;
}

//==================================================================//
// UnloadConveyor Stackser  쓰레드
//==================================================================//
UINT OnThreadEmptyStackerElevator(LPVOID lParam)		// m_thread[8]
{
	CSingleLock sing(&mutex);
	
	while(!AllStop[1])
	{
		sing.Lock();
		
		clsRunEmptyStackerElevator.OnThreadRun();
		
		Sleep(0);
		sing.Unlock();
	}

	m_thrHandle[8] = NULL;
	return TRUE;
}

//==================================================================//
// Unload Stacker  쓰레드
//==================================================================//
UINT OnThreadUldGoodTrayLeftOutputElevator(LPVOID lParam)		// m_thread[9]
{
	CSingleLock sing(&mutex);
	
	while(!AllStop[1])
	{
		sing.Lock();
		
		clsRunUldGoodTrayLeftOutputElevator.OnThreadRun();
		
		Sleep(0);
		sing.Unlock();
	}

	m_thrHandle[9] = NULL;
	return TRUE;
}

//==================================================================//
// Load Reject Stackser  쓰레드
//==================================================================//
UINT OnThreadUldGoodTrayLeftOutputConveyor(LPVOID lParam)	// m_thread[10]
{
	CSingleLock sing(&mutex);
	
	while(!AllStop[1])
	{
		sing.Lock();
		
		clsRunUldGoodTrayLeftOutputConveyor.OnThreadRun();
		
		Sleep(0);
		sing.Unlock();
	}

	m_thrHandle[10] = NULL;
	return TRUE;
}

//==================================================================//
// Reject Stackser  쓰레드
//==================================================================//
UINT OnThreadUldGoodTrayRightOutputElevator(LPVOID lParam)		// m_thread[11]
{
	CSingleLock sing(&mutex);
	
	while(!AllStop[1])
	{
		sing.Lock();
		
		clsRunUldGoodTrayRightOutputElevator.OnThreadRun();
		
		Sleep(0);
		sing.Unlock();
	}

	m_thrHandle[11] = NULL;
	return TRUE;
}

//==================================================================//
// Load Unload Robot  쓰레드
//==================================================================//
UINT OnThreadUldGoodTrayRightOutputConveyor(LPVOID lParam)			// m_thread[12]
{
	CSingleLock sing(&mutex);
	
	while(!AllStop[1])
	{
		sing.Lock();
		
		clsRunUldGoodTrayRightOutputConveyor.OnThreadRun();
		
		Sleep(0);
		sing.Unlock();
	}

	m_thrHandle[12] = NULL;
	return TRUE;
}

//==================================================================//
// Test Site Robot  쓰레드
//==================================================================//
UINT OnThreadEmptyTrayTransfer(LPVOID lParam)		// m_thread[13]
{
	CSingleLock sing(&mutex);
	
	while(!AllStop[1])
	{
		sing.Lock();
		
		clsRunEmptyTrayTransfer.OnThreadRun();
		
		Sleep(0);
		sing.Unlock();
	}

	m_thrHandle[13] = NULL;
	return TRUE;
}

//==================================================================//
// Load Buffer  쓰레드
//==================================================================//
UINT OnThreadRejectTrayInputConveyor(LPVOID lParam)		// m_thread[14]
{
	CSingleLock sing(&mutex);
	
	while(!AllStop[1])
	{
		sing.Lock();
		
		clsRunRejectTrayInputConveyor.OnThreadRun();
		
		Sleep(0);
		sing.Unlock();
	}

	m_thrHandle[14] = NULL;
	return TRUE;
}

//==================================================================//
// Unload Buffer  쓰레드
//==================================================================//
UINT OnThreadRejectTrayStackerElevator(LPVOID lParam)		// m_thread[15]
{
	CSingleLock sing(&mutex);
	
	while(!AllStop[1])
	{
		sing.Lock();
		
		clsRunRejectTrayStackerElevator.OnThreadRun();
		
		Sleep(0);
		sing.Unlock();
	}

	m_thrHandle[15] = NULL;
	return TRUE;
}


//==================================================================//
// Tray Transfer  쓰레드
//==================================================================//
UINT OnThreadRejectTrayFrontStackerElevator(LPVOID lParam)		// m_thread[16]
{
	CSingleLock sing(&mutex);
	
	while(!AllStop[1])
	{
		sing.Lock();
		
		clsRunRejectTrayFrontStackerElevator.OnThreadRun();
		
		Sleep(0);
		sing.Unlock();
	}

	m_thrHandle[16] = NULL;
	return TRUE;
}

//==================================================================//
// Reject Tray Transfer  쓰레드
//==================================================================//
UINT OnThreadRejectTrayFrontOutputConveyor(LPVOID lParam)	// m_thread[17]
{
	CSingleLock sing(&mutex);
	
	while(!AllStop[1])
	{
		sing.Lock();
		
		clsRunRejectTrayFrontOutputConveyor.OnThreadRun();
		
		Sleep(0);
		sing.Unlock();
	}

	m_thrHandle[17] = NULL;
	return TRUE;
}

//==================================================================//
// Retest Buffer  쓰레드
//==================================================================//
UINT OnThreadRejectTrayRearStackerElevator(LPVOID lParam)		// m_thread[18]
{
	CSingleLock sing(&mutex);
	
	while(!AllStop[1])
	{
		sing.Lock();
		
		clsRunRejectTrayFrontOutputConveyor.OnThreadRun();
		
		Sleep(0);
		sing.Unlock();
	}

	m_thrHandle[18] = NULL;
	return TRUE;
}

UINT OnThreadRejectTrayRearOutputConveyor(LPVOID lParam)		// m_thread[19]
{
	CSingleLock sing(&mutex);
	
	while(!AllStop[1])
	{
		sing.Lock();
		
		clsRunRejectTrayRearStackerElevator.OnThreadRun();
		
		Sleep(0);
		sing.Unlock();
	}

	m_thrHandle[19] = NULL;
	return TRUE;
}

UINT OnThreadRejectTrayTransfer(LPVOID lParam)		            // m_thread[20]
{
	CSingleLock sing(&mutex);
	
	while(!AllStop[1])
	{
		sing.Lock();
		
		clsRunRejectTrayTransfer.OnThreadRun();
		
		Sleep(0);
		sing.Unlock();
	}

	m_thrHandle[20] = NULL;
	return TRUE;
}

//Robot 관련 쓰레드
UINT OnThreadLdUldRobot(LPVOID lParam)							// m_thread[21]
{
	CSingleLock sing(&mutex);
	
	while(!AllStop[1])
	{
		sing.Lock();
		
		clsRunLdUldRobot.OnThreadRun();
		
		Sleep(0);
		sing.Unlock();
	}

	m_thrHandle[21] = NULL;
	return TRUE;
}

//load/unload buffer 관련
UINT OnThreadDvcLdBuffer(LPVOID lParam)							// m_thread[22]
{
	CSingleLock sing(&mutex);
	
	while(!AllStop[1])
	{
		sing.Lock();
		
		clsRunDvcLdBuffer.OnThreadRun();
		
		Sleep(0);
		sing.Unlock();
	}

	m_thrHandle[22] = NULL;
	return TRUE;
}

UINT OnThreadDvcUldBuffer(LPVOID lParam)						// m_thread[23]
{
	CSingleLock sing(&mutex);
	
	while(!AllStop[1])
	{
		sing.Lock();
		
		clsRunDvcUldBuffer.OnThreadRun();
		
		Sleep(0);
		sing.Unlock();
	}

	m_thrHandle[23] = NULL;
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////
//Test Site Robot  관련 
///////////////////////////////////////////////////////////////////////////////
UINT OnThreadTestSiteRobot(LPVOID lParam)						// m_thread[24]
{
	CSingleLock sing(&mutex);
	
	while(!AllStop[1])
	{
		sing.Lock();
		
		clsRunTestSiteRobot.OnThreadRun();
		
		Sleep(0);
		sing.Unlock();
	}

	m_thrHandle[24] = NULL;
	return TRUE;
}

UINT OnThreadTestSiteHead_8(LPVOID lParam)						// m_thread[25]
{
	CSingleLock sing(&mutex);
	
	while(!AllStop[1])
	{
		sing.Lock();
		
		clsRunTestSiteHead_8.OnThreadRun();
		
		Sleep(0);
		sing.Unlock();
	}

	m_thrHandle[25] = NULL;
	return TRUE;
}

UINT OnThreadTestSiteRetestBuffer_Left(LPVOID lParam)			// m_thread[26]
{
	CSingleLock sing(&mutex);
	
	while(!AllStop[1])
	{
		sing.Lock();
		
		clsRunTestSiteRetestBuffer_Left.OnThreadRun();
		
		Sleep(0);
		sing.Unlock();
	}

	m_thrHandle[26] = NULL;
	return TRUE;
}

UINT OnThreadTestSiteRetestBuffer_Right(LPVOID lParam)			// m_thread[27]
{
	CSingleLock sing(&mutex);
	
	while(!AllStop[1])
	{
		sing.Lock();
		
		clsRunTestSiteRetestBuffer_Right.OnThreadRun();
		
		Sleep(0);
		sing.Unlock();
	}

	m_thrHandle[27] = NULL;
	return TRUE;
}

UINT OnThreadTestSiteTop(LPVOID lParam)			// m_thread[28]
{
	CSingleLock sing(&mutex);
	
	while(!AllStop[0])
	{
		sing.Lock();
		
		clsTestSite1.OnThreadRun();
		clsTestSite2.OnThreadRun();
		clsTestSite3.OnThreadRun();
		clsTestSite4.OnThreadRun();

		Sleep(0);
		sing.Unlock();
	}

	m_thrHandle[28] = NULL;
	return TRUE;
}

UINT OnThreadTestSiteBottom(LPVOID lParam)		// m_thread[29]
{
	CSingleLock sing(&mutex);
	
	while(!AllStop[0])
	{
		sing.Lock();
		
		clsTestSite5.OnThreadRun();
		clsTestSite6.OnThreadRun();
		clsTestSite7.OnThreadRun();
		clsTestSite8.OnThreadRun();

		Sleep(0);
		sing.Unlock();
	}

	m_thrHandle[29] = NULL;
	return TRUE;
}