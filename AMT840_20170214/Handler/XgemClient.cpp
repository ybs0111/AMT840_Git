#include "StdAfx.h"
#include "XgemClient.h"
#include "Variable.h"
#include "PublicFunction.h"
//#include "AlgMemory.h"
#include "CtlBd_Library.h"
#include "CtlBd_Function.h"
//#include "LogFormat.h"

#define XGEM_CLIENT_MSG_NAME		"XGEM_CLIENT_SHAREMEM"

CXgemClient clsXgem;

CXgemClient::CXgemClient(void)
{
	m_nXgemReq	= BIN_NOTUESD;
	m_strHeader = _T("HD");
	m_nHeader	= 6;

	// 공유 메모리 선언...../////////////////////////////////////////////////////////////////////////////////////////
	m_hMemMapXgem	= NULL;
	m_pQueueXgem		= NULL;

	m_hMemMapXgem	= CreateFileMapping(INVALID_HANDLE_VALUE,
										NULL,
										PAGE_READWRITE,
										0,
										sizeof(XGEM_CLIENT_QUEUE),
										(LPCWSTR)XGEM_CLIENT_MSG_NAME);
	if (m_hMemMapXgem != NULL)
	{	
		if (GetLastError() != ERROR_ALREADY_EXISTS)
		{	
			m_pQueueXgem = (P_XGEM_CLIENT_QUEUE)MapViewOfFile(m_hMemMapXgem, FILE_MAP_ALL_ACCESS, 0, 0, 0);
			if (m_pQueueXgem != NULL)
			{
				m_pQueueXgem->m_nFront	= 0;
				m_pQueueXgem->m_nRear	= 0;
				m_pQueueXgem->m_nCount	= 0;
			}
		}
	}
}


CXgemClient::~CXgemClient(void)
{
	if (m_pQueueXgem		!= NULL)	UnmapViewOfFile(m_pQueueXgem);	
	if (m_hMemMapXgem	!= NULL)	CloseHandle(m_hMemMapXgem);

}

CString	CXgemClient::OnCharToString(char chData[], int nLength)
{
	CString strTmp;
	BSTR	buf;
	int		nCnt;

	nCnt	= MultiByteToWideChar(CP_ACP, 0, chData, nLength, NULL, NULL);
	buf		= SysAllocStringLen(NULL, nCnt);
	MultiByteToWideChar(CP_ACP, 0, chData, nLength, buf, nCnt);
	strTmp.Format(_T("%s"), buf);

	SysFreeString(buf);

	return strTmp;
}

void CXgemClient::OnStringToChar(CString strData, char chData[])
{
	int nCnt;
	wchar_t *chBuf;

	chBuf = strData.GetBuffer(strData.GetLength());
	nCnt  = WideCharToMultiByte(CP_ACP, 0, chBuf, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, chBuf, -1, chData, nCnt, 0, 0);
}

void CXgemClient::OnMessageWrite(CString strMsg)
{
	CString strLog = _T("[HANDLER >> EC SERVER] ");

	if (m_hMemMapXgem != NULL && m_pQueueXgem != NULL)
	{
		strLog += strMsg;
		clsFunc.OnLogFileAdd(4, strLog);

		OnStringToChar(strMsg, m_pQueueXgem->m_chMsg[m_pQueueXgem->m_nRear]);

		m_pQueueXgem->m_nRear++;
		
		if (m_pQueueXgem->m_nRear >= 50) m_pQueueXgem->m_nRear = 0;
		
		m_pQueueXgem->m_nCount++;
	}
}

CString	CXgemClient::OnMessageRead()
{
	CString strData = _T("");
	
	if (m_hMemMapXgem != NULL && m_pQueueXgem != NULL)
	{
		if (m_pQueueXgem->m_nCount > 0 )
		{
			strData = m_pQueueXgem->m_chMsg[m_pQueueXgem->m_nFront];
			
			m_pQueueXgem->m_nFront++;
			if (m_pQueueXgem->m_nFront >= 50) m_pQueueXgem->m_nFront = 0;
			
			m_pQueueXgem->m_nCount--;
		}
	}

	return strData;
}

void CXgemClient::OnCommunication()
{
	CString strKey[10];
	CString strValue[10];

	if (st_handler_info.nRunStatus == dWARNING) return;

	switch (m_nCommStep)
	{
		case 0:
			// 공유메모리에 전송할 데이터가 있으면 
			if (m_pQueueXgem->m_nCount <= 0) break;
				
			m_nRetry		= 0;
			m_strSendMsg	= OnMessageRead();

			m_nCommStep = 100;
			break;

		case 100:
			// interface 상태 체크
			/*if (st_basic_info.nModeInterface == EQP_ON_LINE)
			{
				m_nCommBin	= BD_NONE;

				m_nCommStep = 2000;
			}
			else
			{
				// off line mode 임의로 데이터 생성
				m_dwWaitTime[0] = GetTickCount();

				m_nCommStep = 1000;
			}*/
			m_nCommBin	= BD_NONE;

			m_nCommStep = 2000;
			break;

		case 1000:
			m_dwWaitTime[1] = GetTickCount();
			m_dwWaitTime[2] = m_dwWaitTime[1] - m_dwWaitTime[0];

			if (m_dwWaitTime[2] < 0)
			{
				m_dwWaitTime[0] = GetTickCount();
				break;
			}

			if (m_dwWaitTime[2] > (DWORD)2000)
			{
				m_nCommBin	= BD_DATA_GOOD;

				m_nCommStep = 0;
			}
			break;

		case 2000:
			if (st_client_info[XGEM_NETWORK].nConnect == YES)
			{
				m_nCommStep = 2200;
			}
			else
			{
				m_dwWaitTime[0] = GetTickCount();

				::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + XGEM_NETWORK, CLIENT_CONNECT, 0);

				m_nCommStep = 2100;
			}
			break;

		case 2100:
			if (st_client_info[XGEM_NETWORK].nConnect == YES)
			{
				m_nCommStep = 2200;
			}
			else
			{
				m_dwWaitTime[1] = GetTickCount();
				m_dwWaitTime[2] = m_dwWaitTime[1] - m_dwWaitTime[0];

				if (m_dwWaitTime[2] < 0)
				{
					m_dwWaitTime[0] = GetTickCount();
					break;
				}

				if (m_dwWaitTime[2] > 60000)
				{
					m_nRetry++;

					if (m_nRetry > 3)
					{
						m_nCommBin	= BD_CONT_TIMEOUT;

						::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + XGEM_NETWORK, CLIENT_CLOSE, 0);

						m_dwWaitTime[0] = GetTickCount();

						CtlBdFunc.Alarm_Error_Occurrence(20000, 0, dWARNING, _T("610000"));

						m_nCommStep = 0;
					}
					else
					{
						m_dwWaitTime[0] = GetTickCount();

						 ::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + XGEM_NETWORK, CLIENT_CLOSE, 0);

						m_nCommStep = 2150;
					}
				}
			}
			break;

		case 2150:
			m_dwWaitTime[1] = GetTickCount();
			m_dwWaitTime[2] = m_dwWaitTime[1] - m_dwWaitTime[0];

			if (m_dwWaitTime[2] < 0)
			{
				m_dwWaitTime[0] = GetTickCount();
				break;
			}

			if (m_dwWaitTime[2] > 200)
			{
				m_nCommStep = 100;
			}
			break;

		case 2200:
			m_dwWaitTime[0] = GetTickCount();
			st_client_info[XGEM_NETWORK].strSend = m_strSendMsg;

			::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + XGEM_NETWORK, CLIENT_SEND, 0);

			//m_nCommStep = 2300;
			m_nCommStep = 2350;
			break;

		case 2300:
			if (m_nCommBin == BD_DATA_GOOD)
			{
				m_nCommStep = 0;
			}
			else if (m_nCommBin == BD_DATA_REJECT)
			{
				m_nCommStep = 0;
			}
			else
			{
				m_dwWaitTime[1] = GetTickCount();
				m_dwWaitTime[2] = m_dwWaitTime[1] - m_dwWaitTime[0];

				if (m_dwWaitTime[2] < 0)
				{
					m_dwWaitTime[0] = GetTickCount();
					break;
				}

				if (m_dwWaitTime[2] > 100000)
				{
					m_nRetry++;

					if (m_nRetry > 3)
					{
						m_nCommBin	= BD_RCV_TIMEOUT;

						::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + XGEM_NETWORK, CLIENT_CLOSE, 0);

						m_dwWaitTime[0] = GetTickCount();

						CtlBdFunc.Alarm_Error_Occurrence(20001, 0, dWARNING, _T("610001"));

						m_nCommStep = 0;
					}
					else
					{
						m_dwWaitTime[0] = GetTickCount();
						::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + XGEM_NETWORK, CLIENT_CLOSE, 0);

						m_nCommStep = 2150;
					}
				}
			}
			break;

		case 2350:
			m_dwWaitTime[1] = GetTickCount();
			m_dwWaitTime[2] = m_dwWaitTime[1] - m_dwWaitTime[0];

			if (m_dwWaitTime[2] < 0)
			{
				m_dwWaitTime[0] = GetTickCount();
				break;
			}

			if (m_dwWaitTime[2] > 10)
			{
				m_nCommStep = 0;
			}
			break;
	}
}

void CXgemClient::OnDataReceive(CString strMsg)
{
	int nHead;
	int nLength;
	int nPos;

	CString strTmp;
	CString strData;
	CString strLog = _T("[XGEM >> HANDLER] ");

	if (m_strHeader == _T(""))
	{
		strTmp = strMsg.Mid(0, m_nHeader);
	}
	else
	{
		nPos	= strMsg.Find(m_strHeader, 0);
		strTmp	= strMsg.Mid(nPos + m_strHeader.GetLength() + 1, m_nHeader);
	}
	nHead	= _wtoi(strTmp);

	// Header 
	if (nHead > 0)
	{
		nLength = strMsg.GetLength();

		if (nHead == nLength)
		{
//			strTmp = strMsg.Mid(m_nHeader, nLength);
			OnDataAnalysis(strMsg);
		}
		else
		{
			if (nHead > nLength)
			{
				m_nHeaderLength		= nHead;
				m_strData			= strMsg.Mid(m_nHeader, nLength);
			}
			else
			{
//				strTmp = strMsg.Mid(m_nHeader, nHead);
				strTmp = strMsg.Mid(0, nHead);
				OnDataAnalysis(strTmp);

				nPos	= strMsg.Find(m_strHeader, nHead);
				strTmp	= strMsg.Mid(nPos + m_strHeader.GetLength() + 1, m_nHeader);
				nHead	= _wtoi(strTmp);
				if (nHead > 0)
				{
					strTmp = strMsg.Mid(nPos, nHead);
					OnDataAnalysis(strTmp);
				}
			}
		}
	}
	else
	{
		nLength = strMsg.GetLength();

		if (m_nHeaderLength > 0)
		{
			if (m_nHeaderLength == (m_strData.GetLength() + nLength))
			{
				strTmp		= m_strData + strMsg;
				OnDataAnalysis(strTmp);

				m_nHeaderLength		= 0;
				m_strData			= _T("");
			}
			else
			{
				if (m_nHeaderLength > (m_strData.GetLength() + nLength))
				{
					m_strData		+= strMsg;
				}
				else
				{
					strTmp		= m_strData + strMsg.Mid(0, nLength - (m_nHeaderLength - m_strData.GetLength()));
					OnDataAnalysis(strTmp);

					nPos	= nLength - (m_nHeaderLength - m_strData.GetLength());
					strData = strMsg.Mid(nPos, nLength - nPos);

					m_nHeaderLength		= 0;
					m_strData			= _T("");

					if (m_strHeader == _T(""))
					{
						strTmp	= strData.Mid(0, m_nHeader);
					}
					else
					{
						nPos	= strData.Find(m_strHeader, 0);
						strTmp	= strData.Mid(nPos + m_strHeader.GetLength() + 1, m_nHeader);
					}
					nHead	= _wtoi(strTmp);

					if (nHead > 0)
					{
						nLength = strData.GetLength() - m_nHeader;

						if (nHead == nLength)
						{
							strTmp = strData.Mid(m_nHeader, nLength);
							OnDataAnalysis(strTmp);
						}
						else
						{
							if (nHead > nLength)
							{
								m_nHeaderLength		= nHead;
								m_strData			= strData.Mid(m_nHeader, nLength);
							}
							else
							{
								strTmp = strData.Mid(m_nHeader, nHead);
								OnDataAnalysis(strTmp);
							}
						}
					}
					else
					{
						m_nHeaderLength		= 0;
						m_strData			= _T("");
					}
				}
			}
		}
	}

	strLog += strMsg;
	clsFunc.OnLogFileAdd(4, strLog);

	strTmp.Format(_T("[XGEM] %s"), strMsg);

	//clsMem.OnNormalMessageWrite(strTmp);
}


void CXgemClient::OnDataAnalysis(CString strMsg)
{
	int nLength;
//	int nHead;
	int nPos[2];

	CString strFunc;
	CString strFind;
	CString strRcmd;
	CString strConnect;
	
	strFunc = OnDataItemAnalysis(strMsg, _T("FUNC"));
			
	if (strFunc == _T("INTERFACE"))
	{
		nPos[0] = strMsg.Find(_T("CONNECT="), 0);
		nPos[1] = strMsg.Find(_T(","), nPos[0]);
				
		nLength = (nPos[1] - nPos[0]) - 8;
				
		strConnect	= _T("");
		strConnect = strMsg.Mid(nPos[0] + 8, nLength);
				
		if (strConnect == _T("0"))
		{
			// off line mode
			m_nXgemInterface = MES_OFF_LINE;
			if(st_handler_info.cWndTitle != NULL)  
			{
				st_handler_info.cWndTitle->PostMessage(WM_STATUS_CHANGE, XGEM_MODE, m_nXgemInterface);
 			}
		}
		else if(strConnect == _T("1"))
		{
			// on line local mode
			m_nXgemInterface = MES_ON_LINE_LOCAL;
			if(st_handler_info.cWndTitle != NULL)  
			{
				st_handler_info.cWndTitle->PostMessage(WM_STATUS_CHANGE, XGEM_MODE, m_nXgemInterface);
 			}
		}
		else if(strConnect == _T("2"))
		{
			// on line remote mode
			m_nXgemInterface = MES_ON_LINE_REMOTE;
			if(st_handler_info.cWndTitle != NULL)  
			{
				st_handler_info.cWndTitle->PostMessage(WM_STATUS_CHANGE, XGEM_MODE, m_nXgemInterface);
 			}
		}
	}
	else if (strFunc == _T("RCMD"))
	{
		strRcmd = OnDataItemAnalysis(strMsg, _T("RCMD_NUM"));
		strRcmd.MakeUpper();

		if(strRcmd == _T("START"))
		{
			m_nRunStatus = HOST_RUN;
		}
		else if(strRcmd == _T("STOP"))
		{
			m_nRunStatus = HOST_STOP;
			// jtkim 20161123
			if (st_handler_info.nRunStatus == dRUN)
			{
				//st_other_info.nBuzYesNo = YES;
				//st_other_info.strBoxMsg = _T("XGem RCMD Stop. \r\n RCMD Check");

				if (st_handler_info.cWndMain != NULL)
				{
					st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
				}

				clsFunc.OnMCStop();
			}
		}
	}
	else
	{
		//m_nCommBin = BD_DATA_GOOD;
	}
}


CString	CXgemClient::OnDataItemAnalysis(CString strMsg, CString strFind)
{
	int		nPos[2];
	int		nLength;
	CString strData;
	
	nPos[0] = strMsg.Find(strFind, 0);
	nPos[1] = strMsg.Find(_T(","), nPos[0]+1);
	
	nLength	= (nPos[1] - nPos[0]) - (strFind.GetLength() + 1);
	strData = strMsg.Mid(nPos[0] + strFind.GetLength() + 1, nLength);
	
	return strData;
}


void CXgemClient::OnXgemInitialize(CString strPath)
{
	CString strHead, strFunc;
	CString strMsg;
	if (st_client_info[XGEM_NETWORK].nConnect != YES)
	{
		::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + XGEM_NETWORK, CLIENT_CONNECT, 0);

		return;
	}

	strFunc = _T("FUNC=COMMUNICATION,CONFIG=");
	strFunc += strPath; 
	strFunc += _T(",");
	strHead.Format(_T("HD=%06d,"), strFunc.GetLength() + 10);

	// jtkim 20170131
	//st_client_info[XGEM_NETWORK].strSend = strHead + strFunc;
	//::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + XGEM_NETWORK, CLIENT_SEND, 0);
	strMsg = strHead + strFunc;;
	OnMessageWrite(strMsg);
}


void CXgemClient::OnMcInterface(int nMode)
{
	CString strHead, strFunc;
	CString strLog = _T("[HANDLER >> XGEM] ");
	CString strMsg;

	if (st_client_info[XGEM_NETWORK].nConnect != YES)
	{
		return;
	}

	switch(nMode)
	{
		case MES_OFF_LINE:
			strFunc = _T("FUNC=OPERATE,CONNECT=0,");
			break;

		case MES_ON_LINE_LOCAL:
			strFunc = _T("FUNC=OPERATE,CONNECT=1,");
			break;

		case MES_ON_LINE_REMOTE:
			strFunc = _T("FUNC=OPERATE,CONNECT=2,");
			break;
	}
	
	strHead.Format(_T("HD=%06d,"), strFunc.GetLength() + 10);

	//st_client_info[XGEM_NETWORK].strSend = strHead + strFunc;
	strMsg = strHead + strFunc;
	strLog += strMsg;
	clsFunc.OnLogFileAdd(4, strLog);

	//::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + XGEM_NETWORK, CLIENT_SEND, 0);
	OnMessageWrite(strMsg);
}


void CXgemClient::OnMcRecipeCreate(int nMode, CString strRecipe, CString strPath)
{
	CString strHead, strFunc;
	CString strTmp;
	CString strLog = _T("[HANDLER >> XGEM] ");
	CString strMsg;

	if (st_client_info[XGEM_NETWORK].nConnect != YES)
	{
		return;
	}

	OnXgemRecipe(nMode, strPath, strRecipe);

	strFunc = _T("FUNC=PPID");
	strFunc	+= _T(",");
	strTmp.Format(_T("PP_NAME=%s"), strRecipe);
	strFunc	+= strTmp;
	strFunc	+= _T(",");
	strFunc += _T("METHOD=0");
	strFunc	+= _T(",");
	strTmp.Format(_T("PP_PATH=%s"), strPath);
	strFunc	+= strTmp;
	strFunc	+= _T(",");

	strHead.Format(_T("HD=%06d,"), strFunc.GetLength() + 10);

	//st_client_info[XGEM_NETWORK].strSend = strHead + strFunc;
	strMsg = strHead + strFunc;

	m_strOldRecipe = strRecipe;

	strLog += strMsg;
	clsFunc.OnLogFileAdd(4, strLog);
	//::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + XGEM_NETWORK, CLIENT_SEND, 0);
	OnMessageWrite(strMsg);
}


void CXgemClient::OnMcRecipeSelete(CString strRecipe)
{
	CString strHead, strFunc;
	CString strTmp;
	CString strLog = _T("[HANDLER >> XGEM] ");
	CString strMsg;

	if (st_client_info[XGEM_NETWORK].nConnect != YES)
	{
		return;
	}

	strFunc = _T("FUNC=CEID,CEID_NUM=1100,REPORT=1");
	strFunc += _T(",");
	strTmp.Format(_T("REPORT=1,REP_01={CNT=02,[DA_01=01,%s],[DA_02=01,%s]},"), st_basic_info.strEqp, strRecipe);
	strFunc += strTmp;
	strHead.Format(_T("HD=%06d,"), strFunc.GetLength() + 10);

	//st_client_info[XGEM_NETWORK].strSend = strHead + strFunc;
	strMsg = strHead + strFunc;

	strLog += strMsg;
	clsFunc.OnLogFileAdd(4, strLog);
	//::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + XGEM_NETWORK, CLIENT_SEND, 0);
	OnMessageWrite(strMsg);
}


void CXgemClient::OnMcStatus(int nStatus)
{
	CString strHead, strFunc;
	CString strTmp;
	CString strStatus;
	CString strLog = _T("[HANDLER >> XGEM] ");
	CString strMsg;

	if (st_client_info[XGEM_NETWORK].nConnect != YES)
	{
		return;
	}

	switch (nStatus)
	{
		case dSTOP:
			strStatus = _T("0");
			break;

		case dRUN:
			strStatus = _T("1");
			break;

		case dIDLE:
			strStatus = _T("4");
			break;

		case dWARNING:
			strStatus = _T("2");
			break;
	}

	strFunc = _T("FUNC=CEID,CEID_NUM=1001,REPORT=1");
	strFunc += _T(",");
	strTmp.Format(_T("REPORT=1,REP_01={CNT=02,[DA_01=01,%s],[DA_02=01,%s]},"), st_basic_info.strEqp, strStatus);
	strFunc += strTmp;
	strHead.Format(_T("HD=%06d,"), strFunc.GetLength() + 10);

	//st_client_info[XGEM_NETWORK].strSend = strHead + strFunc;
	strMsg = strHead + strFunc;

	strLog += strMsg;
	clsFunc.OnLogFileAdd(4, strLog);
	//::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + XGEM_NETWORK, CLIENT_SEND, 0);
	OnMessageWrite(strMsg);
}


void CXgemClient::OnMcLotStart(CString strLotNo, CString strPartNo)
{
	CString strHead, strFunc;
	CString strTmp;
	CString strLog = _T("[HANDLER >> XGEM] ");
	CString strMsg;

	if (st_client_info[XGEM_NETWORK].nConnect != YES)
	{
		return;
	}

	//strFunc = _T("FUNC=CEID,CEID_NUM=1002,REPORT=1");
	strFunc = _T("FUNC=CEID,CEID_NUM=9801,REPORT=1");
	strFunc += _T(",");
	strTmp.Format(_T("REP_01={CNT=04,[DA_01=01,%s],[DA_02=01,%s],[DA_03=01,%s],[DA_04=01,100]},"), st_basic_info.strEqp, strPartNo, strLotNo);
	strFunc += strTmp;
	strHead.Format(_T("HD=%06d,"), strFunc.GetLength() + 10);

	//st_client_info[XGEM_NETWORK].strSend = strHead + strFunc;
	strMsg = strHead + strFunc;

	strLog += strMsg;
	clsFunc.OnLogFileAdd(4, strLog);
	//::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + XGEM_NETWORK, CLIENT_SEND, 0);
	OnMessageWrite(strMsg);
}


void CXgemClient::OnMcLotEnd(CString strLotNo, CString strPartNo, int nTotal, int nPass, int nFail)
{
	CString strHead, strFunc;
	CString strTmp;
	CString strData;
	CString strLog = _T("[HANDLER >> XGEM] ");
	CString strMsg;

	if (st_client_info[XGEM_NETWORK].nConnect != YES)
	{
		return;
	}

	//strFunc = _T("FUNC=CEID,CEID_NUM=1003,REPORT=1");
	strFunc = _T("FUNC=CEID,CEID_NUM=9802,REPORT=1");
	strFunc += _T(",");
	
//	strTmp.Format(_T("REP_01={CNT=07,[DA_01=01,%s],[DA_02=01,%s],[DA_03=01,%s],[DA_04=01,%d],[DA_05=01,%d],[DA_06=01,%d],[DA_07=01,%d]},"), st_basic_info.strEqp,
//																																			strPartNo, 
//																																			strLotNo,
//																																			time.GetTotalMinutes(),
//																																			nTotal,
//																																			nPass,
//			
//																																nFail);
	strTmp.Format(_T("REP_01={CNT=06,[DA_01=01,%s]"), st_basic_info.strEqp);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_02=01,%s]"), strPartNo);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_03=01,%s]"), strLotNo);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_04=01,%d]"), nTotal);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_05=01,%d]"), nPass);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_06=01,%d]},"), nFail);
	strFunc += strTmp;

	strHead.Format(_T("HD=%06d,"), strFunc.GetLength() + 10);

	//st_client_info[XGEM_NETWORK].strSend = strHead + strFunc;
	strMsg = strHead + strFunc;

	strLog += strMsg;
	clsFunc.OnLogFileAdd(4, strLog);
	//::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + XGEM_NETWORK, CLIENT_SEND, 0);
	OnMessageWrite(strMsg);
}


void CXgemClient::OnMcAlarm(CString strCode, int nStatus)
{
	CString strHead, strFunc;
	CString strLog = _T("[HANDLER >> XGEM] ");
	CString strMsg;

	if (st_client_info[XGEM_NETWORK].nConnect != YES)
	{
		return;
	}
	
	strFunc.Format(_T("FUNC=ALARM,CODE=%s,STATUS=%d,"), strCode,nStatus);
	strHead.Format(_T("HD=%06d,"), strFunc.GetLength() + 10);
	
	//st_client_info[XGEM_NETWORK].strSend = strHead + strFunc;
	strMsg = strHead + strFunc;

	strLog += strMsg;
	clsFunc.OnLogFileAdd(4, strLog);
	//::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + XGEM_NETWORK, CLIENT_SEND, 0);
	OnMessageWrite(strMsg);
}


void CXgemClient::OnMcPCBInfo(CString strBarcode)
{
	CString strHead, strFunc;
	CString strTmp;
	CString strLog = _T("[HANDLER >> XGEM] ");
	CString strMsg;

	CTime time;

	if (st_client_info[XGEM_NETWORK].nConnect != YES)
	{
		return;
	}

	time = CTime::GetCurrentTime();

	strFunc = _T("FUNC=CEID,CEID_NUM=1003,REPORT=1");
	strFunc += _T(",");
	strTmp.Format(_T("REPORT=1,REP_01={CNT=03,[DA_01=01,%04d%02d%02d%02d%02d%02d%02d],[DA_02=01,%s],[DA_03=01,%s]},"), time.GetYear(),
																													   time.GetMonth(),
																													   time.GetDay(), 
																													   time.GetHour(),
																													   time.GetMinute(),
																													   time.GetSecond(), 
																													   0,
																													   st_basic_info.strEqp,
																													   strBarcode);
	strFunc += strTmp;
	strHead.Format(_T("HD=%06d,"), strFunc.GetLength() + 10);
	//st_client_info[XGEM_NETWORK].strSend = strHead + strFunc;
	strMsg = strHead + strFunc;

	strLog += strMsg;
	clsFunc.OnLogFileAdd(4, strLog);
	//::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + XGEM_NETWORK, CLIENT_SEND, 0);
	OnMessageWrite(strMsg);
}


void CXgemClient::OnXgemRecipe(int nMode, CString strPath, CString strModel)
{
	CString strTemp;
	CString strRecipePath;
	
	strRecipePath = strPath + strModel;

	:: WritePrivateProfileString(_T("Machine Serial"), _T("Name"), LPCTSTR(_T("AMT")), strRecipePath);

	:: WritePrivateProfileString(_T("Recipe"), _T("Name"), LPCTSTR(st_basic_info.strModelName), strRecipePath);

	strTemp.Format(_T("%d"), st_basic_info.nRetry);
	:: WritePrivateProfileString(_T("Mode"), _T("Retry"), LPCTSTR(strTemp), strRecipePath);


// 	strTemp.Format(_T("%d"), st_basic_info.nJigEmptyTime);
// 	:: WritePrivateProfileString(_T("Mode"), _T("Jig_Empty_Time"), LPCTSTR(strTemp), strRecipePath);

// 	strTemp.Format(_T("%d"), st_basic_info.nJigFlag);
// 	:: WritePrivateProfileString(_T("Mode"), _T("Jig_Flag"), LPCTSTR(strTemp), strRecipePath);
// 
// 	strTemp.Format(_T("%d"), st_basic_info.nHookCount);
// 	:: WritePrivateProfileString(_T("Mode"), _T("Jig_Hook_Count"), LPCTSTR(strTemp), strRecipePath);
}


void CXgemClient::OnMcSocket(int nOnOff, int nX, int nY, CString strSerial)
{
}


void CXgemClient::OnMcSocket(int nOnOff, int nSocket)
{
	CString strHead, strFunc;
	CString strTmp;
	CString strData;
	CString strLog = _T("[HANDLER >> XGEM] ");
	CString strMsg;

	if (st_client_info[XGEM_NETWORK].nConnect != YES)
	{
		return;
	}

	if (nOnOff == 0)
	{
		strFunc = _T("FUNC=CEID,CEID_NUM=9825,REPORT=1");
	}
	else
	{
		strFunc = _T("FUNC=CEID,CEID_NUM=9826,REPORT=1");
	}
	strFunc += _T(",");
	
	strTmp.Format(_T("REP_01={CNT=02,[DA_01=01,%s]"), st_basic_info.strEqp);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_02=01,%d]}"), nSocket);
	strFunc += strTmp;

	strHead.Format(_T("HD=%06d,"), strFunc.GetLength() + 10);

	//st_client_info[XGEM_NETWORK].strSend = strHead + strFunc;
	strMsg = strHead + strFunc;

	strLog += strMsg;
	clsFunc.OnLogFileAdd(4, strLog);
	//::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + XGEM_NETWORK, CLIENT_SEND, 0);
	OnMessageWrite(strMsg);
}


void CXgemClient::OnMcTrayIn(int nMode, CString strLotNo, CString strPartNo, int nTrayID)
{
	CString strHead, strFunc;
	CString strTmp;
	CString strData;
	CString strLog = _T("[HANDLER >> XGEM] ");
	CString strMsg;

	if (st_client_info[XGEM_NETWORK].nConnect != YES)
	{
		return;
	}

	if (nMode == 0)
	{
		strFunc = _T("FUNC=CEID,CEID_NUM=9803,REPORT=1");
	}
	else
	{
		strFunc = _T("FUNC=CEID,CEID_NUM=9804,REPORT=1");
	}
	strFunc += _T(",");
	

	strTmp.Format(_T("REP_01={CNT=04,[DA_01=01,%s]"), st_basic_info.strEqp);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_02=01,%s]"), strPartNo);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_03=01,%s]"), strLotNo);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_04=01,%d]}"), nTrayID);
	strFunc += strTmp;

	strHead.Format(_T("HD=%06d,"), strFunc.GetLength() + 10);

	//st_client_info[XGEM_NETWORK].strSend = strHead + strFunc;
	strMsg = strHead + strFunc;

	strLog += strMsg;
	clsFunc.OnLogFileAdd(4, strLog);
	//::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + XGEM_NETWORK, CLIENT_SEND, 0);
	OnMessageWrite(strMsg);
}


void CXgemClient::OnMcBarcode(int nMode, int nTrayID, int nPocket, int nProduct)
{
	CString strHead, strFunc;
	CString strTmp;
	CString strData;
	CString strLog = _T("[HANDLER >> XGEM] ");
	CString strMsg;

	if (st_client_info[XGEM_NETWORK].nConnect != YES)
	{
		return;
	}

	if (nMode == 0)
	{
		strFunc = _T("FUNC=CEID,CEID_NUM=9805,REPORT=1");
	}
	else
	{
		strFunc = _T("FUNC=CEID,CEID_NUM=9806,REPORT=1");
	}
	strFunc += _T(",");
	

	strTmp.Format(_T("REP_01={CNT=04,[DA_01=01,%s]"), st_basic_info.strEqp);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_02=01,%d]"), nTrayID);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_03=01,%d]"), nPocket);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_04=01,%d]}"), nProduct);
	strFunc += strTmp;

	strHead.Format(_T("HD=%06d,"), strFunc.GetLength() + 10);

	//st_client_info[XGEM_NETWORK].strSend = strHead + strFunc;
	strMsg = strHead + strFunc;

	strLog += strMsg;
	clsFunc.OnLogFileAdd(4, strLog);
	//::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + XGEM_NETWORK, CLIENT_SEND, 0);
	OnMessageWrite(strMsg);
}


void CXgemClient::OnMcTrayMoveLDBuffer(int nMode, int nTrayID, int nPocket, int nProduct, CString strSerial)
{
	CString strHead, strFunc;
	CString strTmp;
	CString strData;
	CString strLog = _T("[HANDLER >> XGEM] ");
	CString strMsg;

	if (st_client_info[XGEM_NETWORK].nConnect != YES)
	{
		return;
	}

	if (nMode == 0)
	{
		strFunc = _T("FUNC=CEID,CEID_NUM=9807,REPORT=1");
	}
	else
	{
		strFunc = _T("FUNC=CEID,CEID_NUM=9808,REPORT=1");
	}
	strFunc += _T(",");
	

	strTmp.Format(_T("REP_01={CNT=05,[DA_01=01,%s]"), st_basic_info.strEqp);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_02=01,%d]"), nTrayID);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_03=01,%d]"), nPocket);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_04=01,%d]}"), nProduct);
	strFunc += strTmp;

	//kwlee 2017.0905
	strTmp.Format(_T("[DA_05=01,%s]}"), strSerial);
	strFunc += strTmp;

	strHead.Format(_T("HD=%06d,"), strFunc.GetLength() + 10);

	//st_client_info[XGEM_NETWORK].strSend = strHead + strFunc;
	strMsg = strHead + strFunc;

	strLog += strMsg;
	clsFunc.OnLogFileAdd(4, strLog);
	//::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + XGEM_NETWORK, CLIENT_SEND, 0);
	OnMessageWrite(strMsg);
}


void CXgemClient::OnMcTrayMoveLDZone(int nMode, int nTrayID, int nPocket, int nProduct)
{
	CString strHead, strFunc;
	CString strTmp;
	CString strData;
	CString strLog = _T("[HANDLER >> XGEM] ");
	CString strMsg;

	if (st_client_info[XGEM_NETWORK].nConnect != YES)
	{
		return;
	}

	if (nMode == 0)
	{
		strFunc = _T("FUNC=CEID,CEID_NUM=9809,REPORT=1");
	}
	else
	{
		strFunc = _T("FUNC=CEID,CEID_NUM=9810,REPORT=1");
	}
	strFunc += _T(",");
	

	strTmp.Format(_T("REP_01={CNT=04,[DA_01=01,%s]"), st_basic_info.strEqp);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_02=01,%d]"), nTrayID);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_03=01,%d]"), nPocket);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_04=01,%d]}"), nProduct);
	strFunc += strTmp;

	strHead.Format(_T("HD=%06d,"), strFunc.GetLength() + 10);

	//st_client_info[XGEM_NETWORK].strSend = strHead + strFunc;
	strMsg = strHead + strFunc;

	strLog += strMsg;
	clsFunc.OnLogFileAdd(4, strLog);
	//::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + XGEM_NETWORK, CLIENT_SEND, 0);
	OnMessageWrite(strMsg);
}



//void CXgemClient::OnMcProductLoadBuffer(int nMode, int nTrayID, CString strSerial)
//kwlee 2017.0905
void CXgemClient::OnMcProductLoadBuffer(int nMode, int nTrayID, int nPocket, int nProduct)
{
	CString strHead, strFunc;
	CString strTmp;
	CString strData;
	CString strLog = _T("[HANDLER >> XGEM] ");
	CString strMsg;

	if (st_client_info[XGEM_NETWORK].nConnect != YES)
	{
		return;
	}

	if (nMode == 0)
	{
		strFunc = _T("FUNC=CEID,CEID_NUM=9811,REPORT=1");
	}
	else
	{
		strFunc = _T("FUNC=CEID,CEID_NUM=9812,REPORT=1");
	}
	strFunc += _T(",");
	

	//strTmp.Format(_T("REP_01={CNT=03,[DA_01=01,%s]"), st_basic_info.strEqp);
	//kwlee 2017.0905
	strTmp.Format(_T("REP_01={CNT=04,[DA_01=01,%s]"), st_basic_info.strEqp);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_02=01,%d]"), nTrayID);
	strFunc += strTmp;
	strFunc += _T(",");

// 	strTmp.Format(_T("[DA_03=01,%s]}"), strSerial);
// 	strFunc += strTmp;
	//kwlee 2017.0905
	strTmp.Format(_T("[DA_03=01,%d]"), nPocket);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_04=01,%d]}"), nProduct);
	strFunc += strTmp;


	strHead.Format(_T("HD=%06d,"), strFunc.GetLength() + 10);

	//st_client_info[XGEM_NETWORK].strSend = strHead + strFunc;
	strMsg = strHead + strFunc;

	strLog += strMsg;
	clsFunc.OnLogFileAdd(4, strLog);
	//::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + XGEM_NETWORK, CLIENT_SEND, 0);
	OnMessageWrite(strMsg);
}


void CXgemClient::OnMcProductBufferToSocket(int nMode, int nTrayID, CString strSerial, int nSocket,int nTestSite)
{
	CString strHead, strFunc;
	CString strTmp;
	CString strData;
	CString strLog = _T("[HANDLER >> XGEM] ");
	CString strMsg;

	if (st_client_info[XGEM_NETWORK].nConnect != YES)
	{
		return;
	}

	if (nMode == 0)
	{
		strFunc = _T("FUNC=CEID,CEID_NUM=9813,REPORT=1");
	}
	else
	{
		strFunc = _T("FUNC=CEID,CEID_NUM=9814,REPORT=1");
	}
	strFunc += _T(",");
	

	strTmp.Format(_T("REP_01={CNT=04,[DA_01=01,%s]"), st_basic_info.strEqp);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_02=01,%d]"), nTrayID);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_03=01,%s]"), strSerial);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_04=01,%d]}"), nSocket);
	strFunc += strTmp;

	strHead.Format(_T("HD=%06d,"), strFunc.GetLength() + 10);

	//st_client_info[XGEM_NETWORK].strSend = strHead + strFunc;
	strMsg = strHead + strFunc;

	strLog += strMsg;
	clsFunc.OnLogFileAdd(4, strLog);
	//::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + XGEM_NETWORK, CLIENT_SEND, 0);
	OnMessageWrite(strMsg);
}


void CXgemClient::OnMcProductTest(int nMode, CString strSerial, int nSocket,int nTestSite)
{
	CString strHead, strFunc;
	CString strTmp;
	CString strData;
	CString strLog = _T("[HANDLER >> XGEM] ");
	CString strMsg;

	if (st_client_info[XGEM_NETWORK].nConnect != YES)
	{
		return;
	}

	if (nMode == 0)
	{
		strFunc = _T("FUNC=CEID,CEID_NUM=9815,REPORT=1");
	}
	else
	{
		strFunc = _T("FUNC=CEID,CEID_NUM=9816,REPORT=1");
	}
	strFunc += _T(",");
	

	strTmp.Format(_T("REP_01={CNT=04,[DA_01=01,%s]"), st_basic_info.strEqp);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_02=01,%s]"), strSerial);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_03=01,%d]}"), nSocket);
	strFunc += strTmp;

	//kwlee 2017.0905
	strTmp.Format(_T("[DA_04=01,%d]}"), nTestSite);
	strFunc += strTmp;

	strHead.Format(_T("HD=%06d,"), strFunc.GetLength() + 10);

	//st_client_info[XGEM_NETWORK].strSend = strHead + strFunc;
	strMsg = strHead + strFunc;

	strLog += strMsg;
	clsFunc.OnLogFileAdd(4, strLog);
	//::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + XGEM_NETWORK, CLIENT_SEND, 0);
	OnMessageWrite(strMsg);
}


void CXgemClient::OnMcProductSocketToBuffer(int nMode, int nTrayID, CString strSerial, int nSocket,int nTestSite)
{
	CString strHead, strFunc;
	CString strTmp;
	CString strData;
	CString strLog = _T("[HANDLER >> XGEM] ");
	CString strMsg;

	if (st_client_info[XGEM_NETWORK].nConnect != YES)
	{
		return;
	}

	if (nMode == 0)
	{
		strFunc = _T("FUNC=CEID,CEID_NUM=9817,REPORT=1");
	}
	else
	{
		strFunc = _T("FUNC=CEID,CEID_NUM=9818,REPORT=1");
	}
	strFunc += _T(",");
	

	strTmp.Format(_T("REP_01={CNT=05,[DA_01=01,%s]"), st_basic_info.strEqp);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_02=01,%d]"), nTrayID);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_03=01,%s]"), strSerial);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_04=01,%d]}"), nSocket);
	strFunc += strTmp;

	//kwlee 2017.0905
	strTmp.Format(_T("[DA_05=01,%d]}"), nTestSite);
	strFunc += strTmp;

	strHead.Format(_T("HD=%06d,"), strFunc.GetLength() + 10);

	//st_client_info[XGEM_NETWORK].strSend = strHead + strFunc;
	strMsg = strHead + strFunc;

	strLog += strMsg;
	clsFunc.OnLogFileAdd(4, strLog);
	//::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + XGEM_NETWORK, CLIENT_SEND, 0);
	OnMessageWrite(strMsg);
}


void CXgemClient::OnMcProductUnloadBuffer(int nMode, int nTrayID, CString strSerial)
{
	CString strHead, strFunc;
	CString strTmp;
	CString strData;
	CString strLog = _T("[HANDLER >> XGEM] ");
	CString strMsg;

	if (st_client_info[XGEM_NETWORK].nConnect != YES)
	{
		return;
	}

	if (nMode == 0)
	{
		strFunc = _T("FUNC=CEID,CEID_NUM=9819,REPORT=1");
	}
	else
	{
		strFunc = _T("FUNC=CEID,CEID_NUM=9820,REPORT=1");
	}
	strFunc += _T(",");
	

	strTmp.Format(_T("REP_01={CNT=03,[DA_01=01,%s]"), st_basic_info.strEqp);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_02=01,%d]"), nTrayID);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_03=01,%s]}"), strSerial);
	strFunc += strTmp;

	strHead.Format(_T("HD=%06d,"), strFunc.GetLength() + 10);

	//st_client_info[XGEM_NETWORK].strSend = strHead + strFunc;
	strMsg = strHead + strFunc;

	strLog += strMsg;
	clsFunc.OnLogFileAdd(4, strLog);
	//::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + XGEM_NETWORK, CLIENT_SEND, 0);
	OnMessageWrite(strMsg);
}


void CXgemClient::OnMcTrayUnload(int nMode, CString strLotNo, CString strPartNo, int nTrayID)
{
	CString strHead, strFunc;
	CString strTmp;
	CString strData;
	CString strLog = _T("[HANDLER >> XGEM] ");
	CString strMsg;

	if (st_client_info[XGEM_NETWORK].nConnect != YES)
	{
		return;
	}

	if (nMode == 0)
	{
		strFunc = _T("FUNC=CEID,CEID_NUM=9823,REPORT=1");
	}
	else
	{
		strFunc = _T("FUNC=CEID,CEID_NUM=9824,REPORT=1");
	}
	strFunc += _T(",");
	

	strTmp.Format(_T("REP_01={CNT=04,[DA_01=01,%s]"), st_basic_info.strEqp);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_02=01,%s]"), strPartNo);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_03=01,%s]"), strLotNo);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_04=01,%d]}"), nTrayID);
	strFunc += strTmp;

	strHead.Format(_T("HD=%06d,"), strFunc.GetLength() + 10);

	//st_client_info[XGEM_NETWORK].strSend = strHead + strFunc;
	strMsg = strHead + strFunc;

	strLog += strMsg;
	clsFunc.OnLogFileAdd(4, strLog);
	//::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + XGEM_NETWORK, CLIENT_SEND, 0);
	OnMessageWrite(strMsg);
}


void CXgemClient::OnMcRetestStart(CString strSerial)
{
	CString strHead, strFunc;
	CString strTmp;
	CString strData;
	CString strLog = _T("[HANDLER >> XGEM] ");
	CString strMsg;

	if (st_client_info[XGEM_NETWORK].nConnect != YES)
	{
		return;
	}

	strFunc = _T("FUNC=CEID,CEID_NUM=9821,REPORT=1");
	strFunc += _T(",");
	

	strTmp.Format(_T("REP_01={CNT=02,[DA_01=01,%s]"), st_basic_info.strEqp);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_02=01,%s]}"), strSerial);
	strFunc += strTmp;

	strHead.Format(_T("HD=%06d,"), strFunc.GetLength() + 10);

	//st_client_info[XGEM_NETWORK].strSend = strHead + strFunc;
	strMsg = strHead + strFunc;

	strLog += strMsg;
	clsFunc.OnLogFileAdd(4, strLog);
	//::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + XGEM_NETWORK, CLIENT_SEND, 0);
	OnMessageWrite(strMsg);
}


void CXgemClient::OnMcRetestEnd(CString strSerial, int nSocket, int nTrayID, int nTestSite)
{
	CString strHead, strFunc;
	CString strTmp;
	CString strData;
	CString strLog = _T("[HANDLER >> XGEM] ");
	CString strMsg;

	if (st_client_info[XGEM_NETWORK].nConnect != YES)
	{
		return;
	}

	strFunc = _T("FUNC=CEID,CEID_NUM=9822,REPORT=1");
	strFunc += _T(",");
	
	strTmp.Format(_T("REP_01={CNT=05,[DA_01=01,%s]"), st_basic_info.strEqp);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_02=01,%d]"), nTrayID);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_03=01,%s]"), strSerial);
	strFunc += strTmp;
	strFunc += _T(",");

	strTmp.Format(_T("[DA_04=01,%d]}"), nSocket);
	strFunc += strTmp;
	//kwlee 2017.0905
	strTmp.Format(_T("[DA_05=01,%d]}"), nTestSite);
	strFunc += strTmp;

	strHead.Format(_T("HD=%06d,"), strFunc.GetLength() + 10);

	//st_client_info[XGEM_NETWORK].strSend = strHead + strFunc;
	strMsg = strHead + strFunc;

	strLog += strMsg;
	clsFunc.OnLogFileAdd(4, strLog);
	//::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + XGEM_NETWORK, CLIENT_SEND, 0);
	OnMessageWrite(strMsg);
}



