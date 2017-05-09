// Thread.h:
//
//////////////////////////////////////////////////////////////////////

#ifndef _AFX_THREAD_H__
#define _AFX_THREAD_H__

extern CWinThread *m_thread[40];
extern HANDLE m_thrHandle[40];

//==================================================================//
// 전역 쓰레드 선언
//==================================================================//
extern UINT OnThreadHandlerCheck(LPVOID  lParam);		// m_thread[1]
extern UINT	OnThreadReadIO(LPVOID lParam);				// m_thread[0]
extern UINT	OnThreadTowerLamp(LPVOID lParam);			// m_thread[2]
extern UINT OnThreadAlarmDisplay(LPVOID lParam);		// m_thread[3]
extern UINT OnThreadInterface(LPVOID lParam);			// m_thread[4]
extern UINT OnThreadLoadConveyor(LPVOID lParam);		// m_thread[5]
extern UINT OnThreadLoadStacker(LPVOID lParam);			// m_thread[6]
extern UINT OnThreadEmptyStacker(LPVOID lParam);		// m_thread[7]
extern UINT OnThreadUnloadConveyor(LPVOID lParam);		// m_thread[8]
extern UINT OnThreadUnloadStacker(LPVOID lParam);		// m_thread[9]
extern UINT OnThreadLoadRejectStacker(LPVOID lParam);	// m_thread[10]
extern UINT OnThreadRejectStacker(LPVOID lParam);		// m_thread[11]
extern UINT OnThreadLdUldRobot(LPVOID lParam);			// m_thread[12]
extern UINT OnThreadTestSiteRobot(LPVOID lParam);		// m_thread[13]
extern UINT OnThreadLdUldBuffer(LPVOID lParam);			// m_thread[14]
extern UINT OnThreadTrayTransfer(LPVOID lParam);		// m_thread[15]
extern UINT OnThreadRejectTrayTransfer(LPVOID lParam);	// m_thread[16]
extern UINT OnThreadRetestBuffer(LPVOID lParam);		// m_thread[17]

///////////////////////////////////////////////////////////////////////////////
//2016.0728 james 
///////////////////////////////////////////////////////////////////////////////

//통신 관련
extern UINT OnThreadInterface(LPVOID lParam);							// m_thread[4]

///////////////////////////////////////////////////////////////////////////////
//각종 load/unload/reject 스태커 & 트랜스퍼  관련 
///////////////////////////////////////////////////////////////////////////////
extern UINT	OnThreadLdTrayInputShifterConveyor(LPVOID lParam);			// m_thread[5]
extern UINT	OnThreadLdTrayStackerElevator(LPVOID lParam);				// m_thread[6]
extern UINT	OnThreadLdTrayPlate(LPVOID lParam);							// m_thread[7]
extern UINT	OnThreadEmptyStackerElevator(LPVOID lParam);				// m_thread[8]
extern UINT	OnThreadUldGoodTrayLeftOutputElevator(LPVOID lParam);		// m_thread[9]
extern UINT	OnThreadUldGoodTrayLeftOutputConveyor(LPVOID lParam);		// m_thread[10]
extern UINT	OnThreadUldGoodTrayRightOutputElevator(LPVOID lParam);		// m_thread[11]
extern UINT	OnThreadUldGoodTrayRightOutputConveyor(LPVOID lParam);		// m_thread[12]
extern UINT	OnThreadEmptyTrayTransfer(LPVOID lParam);		            // m_thread[13]
// Reject stacker 관련 
extern UINT	OnThreadRejectTrayInputConveyor(LPVOID lParam);		        // m_thread[14]
extern UINT	OnThreadRejectTrayStackerElevator(LPVOID lParam);		    // m_thread[15]
extern UINT	OnThreadRejectTrayFrontStackerElevator(LPVOID lParam);		// m_thread[16]
extern UINT	OnThreadRejectTrayFrontOutputConveyor(LPVOID lParam);		// m_thread[17]
extern UINT	OnThreadRejectTrayRearStackerElevator(LPVOID lParam);		// m_thread[18]
extern UINT	OnThreadRejectTrayRearOutputConveyor(LPVOID lParam);		// m_thread[19]
extern UINT	OnThreadRejectTrayTransfer(LPVOID lParam);		            // m_thread[20]
//Robot 관련 쓰레드
extern UINT	OnThreadLdUldRobot(LPVOID lParam);							// m_thread[21]
//load/unload buffer 관련
extern UINT	OnThreadDvcLdBuffer(LPVOID lParam);							// m_thread[22]
extern UINT	OnThreadDvcUldBuffer(LPVOID lParam);						// m_thread[23]

///////////////////////////////////////////////////////////////////////////////
//Test Site Robot  관련 
///////////////////////////////////////////////////////////////////////////////
extern UINT	OnThreadTestSiteRobot(LPVOID lParam);						// m_thread[24]
extern UINT	OnThreadTestSiteHead_8(LPVOID lParam);						// m_thread[25]
extern UINT	OnThreadTestSiteRetestBuffer_Left(LPVOID lParam);			// m_thread[26]
extern UINT	OnThreadTestSiteRetestBuffer_Right(LPVOID lParam);			// m_thread[27]

//////////////////////////////////////////////////////////////////////////////////////
// Test Site 통신관련 Thread
//////////////////////////////////////////////////////////////////////////////////////
extern UINT OnThreadTestSiteTop(LPVOID	lParam);						// m_thread[28]
extern UINT OnThreadTestSiteBottom(LPVOID lParam);						// m_thread[29]

//extern UINT OnThreadRejectTrayBuffer(LPVOID lParam);              // m_thread[30]




//

extern int AllStop[2];  // 쓰레드 강제 멈춤 플래그

#endif