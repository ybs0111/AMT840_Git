// LogCtrl.h : LogCtrl DLL의 기본 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"		// 주 기호입니다.


// CLogCtrlApp
// 이 클래스의 구현을 보려면 LogCtrl.cpp를 참조하십시오.
//

class CLogCtrlApp : public CWinApp
{
public:
	CLogCtrlApp();

// 재정의입니다.
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

// DLL Open
extern "C" __declspec(dllexport)void		LogOpen();
// DLL Close
extern "C" __declspec(dllexport)void		LogClose();
// zip
extern "C" __declspec(dllexport)BOOL		LogZip(CString strOriginFile, CString strZipFile);
// alarm  Status(0:발생 1:해제)
extern "C" __declspec(dllexport)void		LogAlarm(CString strID, CString strEvent, CString strCode, int nStatus, int nDataCount, CString *strKey, CString *strData);
// function  Status(0:start 1:end)
extern "C" __declspec(dllexport)void		LogFunction(CString strID, CString strEvent, CString strMatID, CString strMatType, int nDataCount, CString *strKey, CString *strData);
// process  Status(0:start 1:end)
extern "C" __declspec(dllexport)void		LogProcess(CString strID, CString strEvent, int nStatus, CString strMatID, CString strLotID, CString strRecipe, int nDataCount, CString *strKey, CString *strData);
// Transfer 
extern "C" __declspec(dllexport)void		LogTransfer(CString strID, CString strEvent, CString strMatID, CString strMatType, CString strStartPos, CString strEndPos, int nDataCount, CString *strKey, CString *strData);
// lot  Status(0:start 1:end)
extern "C" __declspec(dllexport)void		LogLotInform(CString strID, CString strEvent, CString strLotID, CString strRecipe, CString strCarrierID, int nDataCount, CString *strKey, CString *strData);
// Config
extern "C" __declspec(dllexport)void		LogConfig(CString strID, CString strEvent, CString strRecipe, int nDataCount, CString *strKey, CString *strData);
// Total Log
extern "C" __declspec(dllexport)void		LogTotalA(CString strMsg);
// Communication Log
extern "C" __declspec(dllexport)void		LogCommunicationA(CString strID, CString strMsg);
// alarm Log
extern "C" __declspec(dllexport)void		LogAlarmA(int nType, CString strCode, CString strMsg);
