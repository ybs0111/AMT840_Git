// LogCtrl.h : LogCtrl DLL�� �⺻ ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CLogCtrlApp
// �� Ŭ������ ������ ������ LogCtrl.cpp�� �����Ͻʽÿ�.
//

class CLogCtrlApp : public CWinApp
{
public:
	CLogCtrlApp();

// �������Դϴ�.
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
// alarm  Status(0:�߻� 1:����)
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
