#include "StdAfx.h"
#include "ClientInterface3.h"
#include "Variable.h"
#include "PublicFunction.h"
#include "CtlBd_Library.h"
#include "AlgMemory.h"
#include "LogFromat.h"

#define INTERFACE_CLIENT_MSG_NAME_3		"INTERFACE_CLIENT_SHAREMEM_3"

CClientInterface3 clsInterC3;

CClientInterface3::CClientInterface3(void)
{
	// 공유 메모리 선언...../////////////////////////////////////////////////////////////////////////////////////////
	m_hMemMapInter3	= NULL;
	m_pQueueInter3	= NULL;

	m_hMemMapInter3 = CreateFileMapping(INVALID_HANDLE_VALUE,
										 NULL,
										 PAGE_READWRITE,
										 0,
										 sizeof(INTER_CLIENT_QUEUE_3),
										 (LPCWSTR)INTERFACE_CLIENT_MSG_NAME_3);
	if (m_hMemMapInter3 != NULL)
	{	
		if (GetLastError() != ERROR_ALREADY_EXISTS)
		{	
			m_pQueueInter3 = (P_INTER_CLIENT_QUEUE_3)MapViewOfFile(m_hMemMapInter3, FILE_MAP_ALL_ACCESS, 0, 0, 0);
			if (m_pQueueInter3 != NULL)
			{
				m_pQueueInter3->m_nFront	= 0;
				m_pQueueInter3->m_nRear	= 0;
				m_pQueueInter3->m_nCount	= 0;
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	m_nCommStep = 0;
	m_strHeader = _T("");
	m_nHeader	= 16;
}


CClientInterface3::~CClientInterface3(void)
{
	if (m_pQueueInter3 != NULL)	UnmapViewOfFile(m_pQueueInter3);	
	if (m_hMemMapInter3 != NULL)	CloseHandle(m_hMemMapInter3);
}


void CClientInterface3::OnCommunication()
{
	if (st_handler_info.nRunStatus == dWARNING) return;

	switch (m_nCommStep)
	{
		case 0:
			// 공유메모리에 전송할 데이터가 있으면 
			if (m_pQueueInter3->m_nCount <= 0) break;
				
			m_nRetry		= 0;
			m_strSendMsg	= OnMessageRead();

			m_nCommStep = 100;
			break;

		case 100:
			// interface 상태 체크
			if (st_basic_info.nModeInterface == EQP_ON_LINE || st_basic_info.nModeTestInterface == EQP_ON_LINE)
			{
				m_nCommBin	= BD_NONE;

				m_nCommStep = 2000;
			}
			else
			{
				// off line mode 임의로 데이터 생성
				m_dwWaitTime[0] = GetTickCount();

				m_nCommStep = 1000;
			}
			break;

		case 1000:
			m_dwWaitTime[1] = GetTickCount();
			m_dwWaitTime[2] = m_dwWaitTime[1] - m_dwWaitTime[0];

			if (m_dwWaitTime[2] <= 0)
			{
				m_dwWaitTime[0] = GetTickCount();
				break;
			}

			if (m_dwWaitTime[2] > (DWORD)st_recipe_info.nAbortTime)
			{
				m_nCommBin	= BD_DATA_GOOD;

				m_nCommStep = 0;
			}
			break;

		case 2000:
			if (st_client_info[INTERFACE_NETWORK_3].nConnect == YES)
			{
				m_nCommStep = 2200;
			}
			else
			{
				m_dwWaitTime[0] = GetTickCount();

				::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + INTERFACE_NETWORK_3, CLIENT_CONNECT, 0);

				m_nCommStep = 2100;
			}
			break;

		case 2100:
			if (st_client_info[INTERFACE_NETWORK_3].nConnect == YES)
			{
				m_nCommStep = 2200;
			}
			else
			{
				m_dwWaitTime[1] = GetTickCount();
				m_dwWaitTime[2] = m_dwWaitTime[1] - m_dwWaitTime[0];

				if (m_dwWaitTime[2] <= 0)
				{
					m_dwWaitTime[0] = GetTickCount();
					break;
				}

				if (m_dwWaitTime[2] > 30000)
				{
					m_nRetry++;

					//kwlee 2016.1102
					//if (m_nRetry > 3)
					if (m_nRetry > 1)
					{
						m_nCommBin	= BD_CONT_TIMEOUT;

						::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + INTERFACE_NETWORK_3, CLIENT_CLOSE, 0);

						m_dwWaitTime[0] = GetTickCount();
						CTL_Lib.Alarm_Error_Occurrence(20000, dWARNING, _T("730000"));

						m_nCommStep = 0;
					}
					else
					{
						m_dwWaitTime[0] = GetTickCount();

						::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + INTERFACE_NETWORK_3, CLIENT_CLOSE, 0);
						//kwlee 2016.1102
						m_strSendMsg = _T("FUNCTION=TESTER_REREADY");
						m_nCommStep = 2150;
					}
				}
			}
			break;

		case 2150:
			m_dwWaitTime[1] = GetTickCount();
			m_dwWaitTime[2] = m_dwWaitTime[1] - m_dwWaitTime[0];

			if (m_dwWaitTime[2] <= 0)
			{
				m_dwWaitTime[0] = GetTickCount();
				break;
			}

			if (m_dwWaitTime[2] > WAIT_CLOSE)
			{
				m_nCommStep = 100;
			}
			break;

		case 2200:
			m_dwWaitTime[0] = GetTickCount();
			st_client_info[INTERFACE_NETWORK_3].strSend = m_strSendMsg;

			::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + INTERFACE_NETWORK_3, CLIENT_SEND, 0);

			m_nCommStep = 2300;
			break;

		case 2300:
			if (m_nCommBin == BD_DATA_GOOD)
			{
				m_dwWaitTime[0] = GetTickCount();
				::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + INTERFACE_NETWORK_3, CLIENT_CLOSE, 0);

				m_nCommStep = 2350;

				///////////////////////////////////////////////
				//2016.1203
				st_product_info.dTestTime += (double)st_test_site_info[THD_TESTSITE_3].dwBdTime;
				st_product_info.nTestCount++;
				/////////////////////////////////////////////////////
			}
			else if (m_nCommBin == BD_DATA_REJECT)
			{
				m_dwWaitTime[0] = GetTickCount();
				::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + INTERFACE_NETWORK_3, CLIENT_CLOSE, 0);

				m_nCommStep = 2350;
			}
			else
			{
				m_dwWaitTime[1] = GetTickCount();
				m_dwWaitTime[2] = m_dwWaitTime[1] - m_dwWaitTime[0];

				if (m_dwWaitTime[2] <= 0)
				{
					m_dwWaitTime[0] = GetTickCount();
					break;
				}

				if (m_dwWaitTime[2] > 300000)//최대 3분을 기다리다가 다시 명령어 전송 //2016.1027
				{
					m_nRetry++;

					//kwlee 2016.1102
					//if (m_nRetry > 3)
					if (m_nRetry > 1)
					{
						m_nCommBin	= BD_RCV_TIMEOUT;

						::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + INTERFACE_NETWORK_3, CLIENT_CLOSE, 0);

						m_dwWaitTime[0] = GetTickCount();
						CTL_Lib.Alarm_Error_Occurrence(20001, dWARNING, _T("730001"));

						m_nCommStep = 0;
					}
					else
					{
						m_dwWaitTime[0] = GetTickCount();
						::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + INTERFACE_NETWORK_3, CLIENT_CLOSE, 0);

						m_nCommStep = 2150;
					}
				}
			}
			break;

		case 2350:
			m_dwWaitTime[1] = GetTickCount();
			m_dwWaitTime[2] = m_dwWaitTime[1] - m_dwWaitTime[0];

			if (m_dwWaitTime[2] <= 0)
			{
				m_dwWaitTime[0] = GetTickCount();
				break;
			}

			if (m_dwWaitTime[2] > WAIT_CLOSE)
			{
				m_nCommStep = 0;
			}
			break;
	}
}

CString	CClientInterface3::OnCharToString(char chData[], int nLength)
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

void CClientInterface3::OnStringToChar(CString strData, char chData[])
{
	int nCnt;
	wchar_t *chBuf;

	chBuf = strData.GetBuffer(strData.GetLength());
	nCnt  = WideCharToMultiByte(CP_ACP, 0, chBuf, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, chBuf, -1, chData, nCnt, 0, 0);
}

void CClientInterface3::OnMessageWrite(CString strMsg)
{
	CString strLog = _T("[HANDLER >> EC SERVER] ");

	if (m_hMemMapInter3 != NULL && m_pQueueInter3 != NULL)
	{
		strLog += strMsg;
		clsFunc.OnLogFileAdd(4, strLog);

		OnStringToChar(strMsg, m_pQueueInter3->m_chMsg[m_pQueueInter3->m_nRear]);

		m_pQueueInter3->m_nRear++;
		
		if (m_pQueueInter3->m_nRear >= 10) m_pQueueInter3->m_nRear = 0;
		
		m_pQueueInter3->m_nCount++;
	}
}

CString	CClientInterface3::OnMessageRead()
{
	CString strData = _T("");
	
	if (m_hMemMapInter3 != NULL && m_pQueueInter3 != NULL)
	{
		if (m_pQueueInter3->m_nCount > 0 )
		{
			strData = m_pQueueInter3->m_chMsg[m_pQueueInter3->m_nFront];
			
			m_pQueueInter3->m_nFront++;
			if (m_pQueueInter3->m_nFront >= 10) m_pQueueInter3->m_nFront = 0;
			
			m_pQueueInter3->m_nCount--;
		}
	}

	return strData;
}


void CClientInterface3::OnDataReceive(CString strMsg)
{
	int nHead;
	int nLength;
	int nPos;

	CString strTemp;
	CString strData;
	CString strFunc;
	CString strLog = _T("[INTERFACE SERVER 3 >> HANDLER] ");

	if (m_strHeader == _T(""))
	{
		strTemp = strMsg.Mid(0, m_nHeader);
	}
	else
	{
		nPos	= strMsg.Find(m_strHeader, 0);
		strTemp	= strMsg.Mid(nPos + m_strHeader.GetLength() + 1, m_nHeader);
	}
	nHead	= _wtoi(strTemp);

	// Header 
	if (nHead > 0)
	{
		nLength = strMsg.GetLength() - m_nHeader;

		if (nHead == nLength)
		{
			strTemp = strMsg.Mid(m_nHeader, nLength);
			OnDataAnalysis(strTemp);
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
				strTemp = strMsg.Mid(m_nHeader, nHead);
				OnDataAnalysis(strTemp);
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
				strTemp		= m_strData + strMsg;
				OnDataAnalysis(strTemp);

				m_nHeaderLength		= 0;
				m_strData			= _T("");
			}
			else
			{
				if (m_nHeaderLength > (m_strData.GetLength() + nLength))
				{
					m_strData		+= strMsg;

					// lot end 예외처리
					strFunc = OnDataItemAnalysis(0, 0, m_strData, _T("FUNCTION_RPY"));
					if (strFunc == _T("SORTER_LOT_END"))
					{
						nLength = m_strData.GetLength();
						if (nLength > 140)
						{
							OnDataAnalysis(m_strData);

							m_nHeaderLength		= 0;
							m_strData			= _T("");
						}
					}
				}
				else
				{
					strTemp		= m_strData + strMsg;

					if (m_nHeaderLength < strTemp.GetLength())
					{
						OnDataAnalysis(strTemp);

						m_nHeaderLength		= 0;
						m_strData			= _T("");
					}
				}
			}
		}
	}

	strLog += strMsg;
	clsFunc.OnLogFileAdd(4, strLog);

	if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
	{
		strTemp.Format(_T("[INTERFACE_CLIENT_3_NETWORK] %s"), strMsg);
		clsMem.OnNormalMessageWrite(strTemp);
	}
}


void CClientInterface3::OnDataAnalysis(CString strMsg)
{
	CString strFunc;
	CString strData;
	CString strTemp;
	CString strBarcode;
	CString strLogKey[10];
	CString strLogData[10];

	int i;
	int nSite = THD_TESTSITE_1 + 2;
	int nPos;
	BOOL bData;

	strFunc = OnDataItemAnalysis(0, 0, strMsg, _T("FUNCTION_RPY"));

	if (strFunc == _T("TESTER_READY"))
	{
		strData = OnDataItemAnalysis(0, 0, strMsg, _T("RESULT"));
		if (strData == _T("PASS"))
		{
			strData = OnDataItemAnalysis(0, 0, strMsg, _T("SOCKET_INFO"));
			for (i=0; i<8; i++)
			{
				strTemp = strData.Mid((i * 2), 1);
				//kwlee 2016.1026 Recipe에서 Socket Off 하였으나 Result 받았을 경우 Socket Off 해제 됨
				if (st_test_site_info[nSite].st_pcb_info[i].nEnable == YES)
				{
					if (strTemp == _T("1"))
					{
						st_test_site_info[nSite].st_pcb_info[i].nEnable = YES;
					}
					else
					{
						st_test_site_info[nSite].st_pcb_info[i].nEnable = NO;
					}
				}

// 				if (strTemp == _T("1"))
// 				{
// 					st_test_site_info[nSite].st_pcb_info[i].nEnable = YES;
// 				}
// 				else
// 				{
// 					st_test_site_info[nSite].st_pcb_info[i].nEnable = NO;
// 				}
				/////
			}

			// test site tester ready체크 완료
			st_test_site_info[nSite].nTestBdStart	= BD_READY_OK;

			m_nRpyBin[INTER_READY] = m_nCommBin = BD_DATA_GOOD;
		}
		else
		{
			//kwlee 2016.1110
			//strData = OnDataItemAnalysis(0, 0, strMsg, _T("MSG"));
			strData = OnDataItemAnalysis(5, 0, strMsg, _T("MSG"));
			// Already Lot Loaded
			if (strData != _T(""))
			{
				m_strErrorMsg	= strData.Mid(1, strData.GetLength()-1);
			}
			nPos = -1;
			nPos = strData.Find(_T("Already Lot Loaded"), 0);

			if (nPos > 0)
			{
				st_test_site_info[nSite].nTestBdStart	= BD_READY_OK;

				m_nRpyBin[INTER_READY] = m_nCommBin = BD_DATA_GOOD;
			}
			else
			{
				m_nRpyBin[INTER_READY] = m_nCommBin = BD_DATA_REJECT;
			}
			//2016.1101				
			st_handler_info.mstr_event_msg[0].Format(_T("%d site %s,엔지니어 호출 해주세요"), 3, m_strErrorMsg );
			clsMem.OnAbNormalMessagWrite(st_handler_info.mstr_event_msg[0]);//로그 저장
			::PostMessage(st_handler_info.hWnd, WM_MAIN_EVENT, YES, 0);	
			st_test_site_info[nSite].nTestBdStart = BD_END;
		}
	}
	else if (strFunc == _T("SOCKET_INFO"))
	{
		strData = OnDataItemAnalysis(0, 0, strMsg, _T("RESULT"));
		if (strData == _T("PASS"))
		{
			strData = OnDataItemAnalysis(0, 0, strMsg, _T("SOCKET_INFO"));
			for (i=0; i<8; i++)
			{
				strTemp = strData.Mid((i * 2), 1);
				//kwlee 2016.1026 Recipe에서 Socket Off 하였으나 Result 받았을 경우 Socket Off 해제 됨
				if (st_test_site_info[nSite].st_pcb_info[i].nEnable == YES)
				{
					if (strTemp == _T("1"))
					{
						st_test_site_info[nSite].st_pcb_info[i].nEnable = YES;
					}
					else
					{
						st_test_site_info[nSite].st_pcb_info[i].nEnable = NO;
					}
				}
// 				if (strTemp == _T("1"))
// 				{
// 					st_test_site_info[nSite].st_pcb_info[i].nEnable = YES;
// 				}
// 				else
// 				{
// 					st_test_site_info[nSite].st_pcb_info[i].nEnable = NO;
// 				}
			}
			m_nRpyBin[INTER_SOCKET] = m_nCommBin = BD_DATA_GOOD;
		}
		else
		{
			//kwlee 2016.1110
			//strData = OnDataItemAnalysis(0, 0, strMsg, _T("MSG"));
			strData = OnDataItemAnalysis(5, 0, strMsg, _T("MSG"));
			if (strData != _T(""))
			{
				m_strErrorMsg	= strData.Mid(1, strData.GetLength()-1);
			}
			m_nRpyBin[INTER_SOCKET] = m_nCommBin = BD_DATA_REJECT;
			//2016.1101				
			st_handler_info.mstr_event_msg[0].Format(_T("%d site %s,엔지니어 호출 해주세요"), 3, m_strErrorMsg );
			clsMem.OnAbNormalMessagWrite(st_handler_info.mstr_event_msg[0]);//로그 저장
			::PostMessage(st_handler_info.hWnd, WM_MAIN_EVENT, YES, 0);	
			st_test_site_info[nSite].nTestBdStart = BD_END;
		}
	}
	else if (strFunc == _T("MDL_INFO"))
	{
		strData = OnDataItemAnalysis(0, 0, strMsg, _T("RESULT"));
		if (strData == _T("PASS"))
		{
			bData = TRUE;
			for (i=0; i<8; i++)
			{
				if (st_test_site_info[nSite].st_pcb_info[i].nEnable == YES &&
					st_test_site_info[nSite].st_pcb_info[i].nYesNo	== YES)
				{
					if (strData != st_test_site_info[nSite].st_pcb_info[i].strBarcode)
					{ 
						bData = FALSE;
						break;
					}
				}
			}

			if (bData) m_nRpyBin[INTER_MODULE] = BD_DATA_GOOD;
			else m_nRpyBin[INTER_MODULE] = BD_DATA_REJECT;

			st_test_site_info[nSite].nTestBdStart = BD_MODULE_OK;

			m_nCommBin = BD_DATA_GOOD;
		}
		else
		{
			//kwlee 2016.1110
			//strData = OnDataItemAnalysis(0, 0, strMsg, _T("MSG"));
			strData = OnDataItemAnalysis(5, 0, strMsg, _T("MSG"));
			if (strData != _T(""))
			{
				m_strErrorMsg	= strData.Mid(1, strData.GetLength()-1);
			}
			m_nRpyBin[INTER_MODULE] = m_nCommBin = BD_DATA_REJECT;
			//2016.1101				
			st_handler_info.mstr_event_msg[0].Format(_T("%d site %s,엔지니어 호출 해주세요"), 3, m_strErrorMsg );
			clsMem.OnAbNormalMessagWrite(st_handler_info.mstr_event_msg[0]);//로그 저장
			::PostMessage(st_handler_info.hWnd, WM_MAIN_EVENT, YES, 0);	
			st_test_site_info[nSite].nTestBdStart = BD_END;
		}
	}
	else if (strFunc == _T("PGM_LOAD"))
	{
		strData = OnDataItemAnalysis(0, 0, strMsg, _T("RESULT"));
		if (strData == _T("PASS"))
		{
			m_nRpyBin[INTER_PGM] = m_nCommBin = BD_DATA_GOOD;
		}
		else
		{
			//kwlee 2016.1110
			//strData = OnDataItemAnalysis(0, 0, strMsg, _T("MSG"));
			strData = OnDataItemAnalysis(5, 0, strMsg, _T("MSG"));
			if (strData != _T(""))
			{
				m_strErrorMsg	= strData.Mid(1, strData.GetLength()-1);
			}
			m_nRpyBin[INTER_PGM] = m_nCommBin = BD_DATA_REJECT;
			//2016.1101				
			st_handler_info.mstr_event_msg[0].Format(_T("%d site %s,엔지니어 호출 해주세요"), 3, m_strErrorMsg );
			clsMem.OnAbNormalMessagWrite(st_handler_info.mstr_event_msg[0]);//로그 저장
			::PostMessage(st_handler_info.hWnd, WM_MAIN_EVENT, YES, 0);	
			st_test_site_info[nSite].nTestBdStart = BD_END;
		}
	}
	else if (strFunc == _T("TEST_START"))
	{
		strData = OnDataItemAnalysis(0, 0, strMsg, _T("RESULT"));
		if (strData == _T("PASS"))
		{
			st_test_site_info[nSite].nTestBdStart = BD_START_OK;

			m_nRpyBin[INTER_START] = m_nCommBin = BD_DATA_GOOD;
			//2017.0109
			strLogKey[0]	= _T("SERIAL");
			for (i=0; i<8; i++)
			{
				if (st_test_site_info[nSite].st_pcb_info[i].nEnable == YES && st_test_site_info[nSite].st_pcb_info[i].nYesNo	== YES)
				{
					strLogData[0]	= st_test_site_info[THD_TESTSITE_3].st_pcb_info[i].strSerialNo;
					clsLog.LogProcess(_T("TESTER#3"), 
 								  _T("CDIMM TEST"), 
 								  0, 
 								  st_test_site_info[THD_TESTSITE_3].st_pcb_info[i].strSerialNo, 
 								  st_test_site_info[THD_TESTSITE_3].st_pcb_info[i].strLotNo, 
 								  st_basic_info.strDeviceName, 
 								  0, 
 								  strLogKey, 
 								  strLogData);
				}
			}
		}
		else
		{
			//kwlee 2016.1110
			//strData = OnDataItemAnalysis(0, 0, strMsg, _T("MSG"));
			strData = OnDataItemAnalysis(5, 0, strMsg, _T("MSG"));
			if (strData != _T(""))
			{
				m_strErrorMsg	= strData.Mid(1, strData.GetLength()-1);
			}
			m_nRpyBin[INTER_START] = m_nCommBin = BD_DATA_REJECT;
			//2016.1101				
			st_handler_info.mstr_event_msg[0].Format(_T("%d site %s,엔지니어 호출 해주세요"), 3, m_strErrorMsg );
			clsMem.OnAbNormalMessagWrite(st_handler_info.mstr_event_msg[0]);//로그 저장
			::PostMessage(st_handler_info.hWnd, WM_MAIN_EVENT, YES, 0);	
			st_test_site_info[nSite].nTestBdStart = BD_END;
		}
	}
	else if (strFunc == _T("LOT_TOTAL"))
	{
		strData = OnDataItemAnalysis(0, 0, strMsg, _T("RESULT"));
		if (strData == _T("PASS"))
		{
			m_nRpyBin[INTER_TOTAL] = m_nCommBin = BD_DATA_GOOD;
		}
		else
		{
			//kwlee 2016.1110
			//strData = OnDataItemAnalysis(0, 0, strMsg, _T("MSG"));
			strData = OnDataItemAnalysis(5, 0, strMsg, _T("MSG"));
			if (strData != _T(""))
			{
				m_strErrorMsg	= strData.Mid(1, strData.GetLength()-1);
			}
			m_nRpyBin[INTER_TOTAL] = m_nCommBin = BD_DATA_REJECT;
			//2016.1101				
			st_handler_info.mstr_event_msg[0].Format(_T("%d site %s,엔지니어 호출 해주세요"), 3, m_strErrorMsg );
			clsMem.OnAbNormalMessagWrite(st_handler_info.mstr_event_msg[0]);//로그 저장
			//2017.0324 에러 스킵 의미 없다
			::PostMessage(st_handler_info.hWnd, WM_MAIN_EVENT, YES, 0);	
			st_test_site_info[nSite].nTestBdStart = BD_END;
		}
	}
	else if (strFunc == _T("TESTER_OPTION"))
	{
		strData = OnDataItemAnalysis(0, 0, strMsg, _T("RESULT"));
		if (strData == _T("PASS"))
		{
			m_nRpyBin[INTER_OPTION] = m_nCommBin = BD_DATA_GOOD;
		}
		else
		{
			//kwlee 2016.1110
			//strData = OnDataItemAnalysis(0, 0, strMsg, _T("MSG"));
			strData = OnDataItemAnalysis(5, 0, strMsg, _T("MSG"));
			if (strData != _T(""))
			{
				m_strErrorMsg	= strData.Mid(1, strData.GetLength()-1);
			}
			m_nRpyBin[INTER_OPTION] = m_nCommBin = BD_DATA_REJECT;
			//2016.1101				
			st_handler_info.mstr_event_msg[0].Format(_T("%d site %s,엔지니어 호출 해주세요"), 3, m_strErrorMsg );
			clsMem.OnAbNormalMessagWrite(st_handler_info.mstr_event_msg[0]);//로그 저장
			::PostMessage(st_handler_info.hWnd, WM_MAIN_EVENT, YES, 0);	
			st_test_site_info[nSite].nTestBdStart = BD_END;
		}
	}
	else if (strFunc == _T("TEST_ABORT"))
	{
		strData = OnDataItemAnalysis(0, 0, strMsg, _T("RESULT"));
		if (strData == _T("PASS"))
		{
			for (i=0; i<8; i++)
			{
				st_test_site_info[nSite].st_pcb_info[i].nBin = BD_DATA_ABORT;
			}

			st_test_site_info[nSite].nTestBdStart = BD_BIN_OK;

			m_nRpyBin[INTER_ABORT] = m_nCommBin = BD_DATA_GOOD;
		}
		else
		{
			strData = OnDataItemAnalysis(0, 0, strMsg, _T("MSG"));
			if (strData != _T(""))
			{
				m_strErrorMsg	= strData.Mid(1, strData.GetLength()-1);
			}
			//2016.1026
			for (i=0; i<8; i++)
			{
				st_test_site_info[nSite].st_pcb_info[i].nBin = BD_DATA_ABORT;
			}
			st_test_site_info[nSite].nTestBdStart = BD_BIN_OK;
			////
			m_nRpyBin[INTER_ABORT] = m_nCommBin = BD_DATA_REJECT;
		}
	}
	else if (strFunc == _T("CHAMBER_DOOR"))
	{
		m_nCommBin = BD_DATA_GOOD;
	}
}


CString CClientInterface3::OnDataItemAnalysis(int nOpt, int nPos, CString strMsg, CString strFind)
{
	CString strData = _T("");
	CString strHead;
	int length;
	int pos[4];

	length = strMsg.GetLength();

	pos[0] = -1;
	pos[0] = strMsg.Find(strFind, nPos);

	if(pos[0] < 0) return strData;

	if (nOpt == 4)
	{
		pos[1] = -1;
		pos[1] = strMsg.Find(_T("^FS"), pos[0]+1);
		if(pos[1] < 0)
		{
			return strData;
		}

		strData = strMsg.Mid(pos[0]+3, pos[1]-(pos[0]+3));	

		return strData;
	}
	else
	{
		pos[1] = -1;
		pos[1] = strMsg.Find(_T("="), pos[0]+1);
		if(pos[1] < 0)
		{
			return strData;
		}
	}

	pos[2] = -1;
	pos[2] = strMsg.Find(_T(" "), pos[0]+1);

	if(nOpt == 0)
	{
		if(pos[2] == -1)
		{
			strData = strMsg.Mid(pos[1]+1, length-(pos[1]+1));	
		}
		else
		{
			strData = strMsg.Mid(pos[1]+1, pos[2]-(pos[1]+1));
		}
	}
	else if(nOpt == 1)
	{
		strData = strMsg.Mid(pos[0], length-(pos[0]));
	}
	else if(nOpt == 2)
	{
		pos[2] = -1;
		pos[2] = strMsg.Find(_T(")"), pos[1]+1);

		strData = strMsg.Mid(pos[1]+1, pos[2]-(pos[1]));
	}
	else if (nOpt == 3)
	{
		strHead.Format(_T("%c"), '"');

		pos[2] = strMsg.Find(strHead, 0);
		pos[3] = strMsg.Find(strHead, pos[2]+1);

		if (pos[2] > 0 && pos[3] > 0)
		{
			strData = strMsg.Mid(pos[2]+1, pos[3]-(pos[2]) - 1);
		}
		else
		{
			strData = _T("Message Fail");
		}
	}
	//kwlee 2016.1110
	else if (nOpt == 5)
	{
		strData = strMsg.Mid(pos[1], length-(pos[1]+1));
	}
	return strData;
}


void CClientInterface3::OnTesterReady(tagTEST_SITE_INFO &test_site)
{
	CString strTemp;
	CString strData;
	CString strMsg;

	m_nRpyBin[INTER_READY]	= BD_NONE;

	strData = _T("FUNCTION=TESTER_READY");
	strData += _T(" ");

	strTemp.Format(_T("LOT_ID=%s"), test_site.strLotNo);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("EQP_ID=%s"), test_site.strEqpID);
	strData += strTemp;
	strData += _T(" ");

	switch (test_site.nTestMode)
	{ 
		case 0:
			strTemp.Format(_T("TEST_MODE=%s"), _T("ONESHOT"));
			break;

		case 1:
			strTemp.Format(_T("TEST_MODE=%s"), _T("HOT"));
			break;

		case 2:
			strTemp.Format(_T("TEST_MODE=%s"), _T("COLD"));
			break;

		case 3:
			strTemp.Format(_T("TEST_MODE=%s"), _T("GATE"));
			break;
	}
	strData += strTemp;
	strData += _T(" ");

	if (test_site.nInterface == 0)
	{
		strTemp.Format(_T("PGM_SEARCH=%s"), _T("FTP"));
	}
	else
	{
		strTemp.Format(_T("PGM_SEARCH=%s"), _T("LOCAL"));
	}
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("PART_NO=%s"), test_site.strPartNo);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("RETRY_COUNT=%d"), test_site.nRetry);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("QTY=%d"), test_site.nQty);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("OPT=%s"), test_site.strOptCode);
	strData += strTemp;

	strMsg.Format(_T("%016d%s"), strData.GetLength(), strData);

	OnMessageWrite(strMsg);
}


void CClientInterface3::OnSocketInfo(tagTEST_SITE_INFO &test_site)
{
	int i;

	CString strTemp;
	CString strData;
	CString strSocket;
	CString strMsg;

	m_nRpyBin[INTER_SOCKET] = BD_NONE;

	strData = _T("FUNCTION=SOCKET_INFO");
	strData += _T(" ");

	strTemp.Format(_T("LOT_ID=%s"), test_site.strLotNo);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("EQP_ID=%s"), test_site.strEqpID);
	strData += strTemp;
	strData += _T(" ");

	strSocket = _T("SOCKET_INFO=");
	for (i=0; i<8; i++)
	{
		strTemp.Format(_T("%d"), test_site.st_pcb_info[i].nEnable);
		strSocket += strTemp;
		if (i < 7)
		{
			strSocket += _T(",");
		}
	}
	strData += strSocket;

	strMsg.Format(_T("%016d%s"), strData.GetLength(), strData);

	OnMessageWrite(strMsg);
}


void CClientInterface3::OnModuleInfo(tagTEST_SITE_INFO &test_site)
{
	int i;

	CString strTemp;
	CString strData;
	CString strMsg;
	CString strBarcode;

	m_nRpyBin[INTER_MODULE] = BD_NONE;

	strData = _T("FUNCTION=MDL_INFO");
	strData += _T(" ");

	strTemp.Format(_T("LOT_ID=%s"), test_site.strLotNo);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("EQP_ID=%s"), test_site.strEqpID);
	strData += strTemp;
	strData += _T(" ");

	switch (test_site.nTestMode)
	{ 
		case 0:
			strTemp.Format(_T("TEST_MODE=%s"), _T("ONESHOT"));
			break;

		case 1:
			strTemp.Format(_T("TEST_MODE=%s"), _T("HOT"));
			break;

		case 2:
			strTemp.Format(_T("TEST_MODE=%s"), _T("COLD"));
			break;

		case 3:
			strTemp.Format(_T("TEST_MODE=%s"), _T("GATE"));
			break;
	}
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("PART_NO=%s"), test_site.strPartNo);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("OPT=%s"), test_site.strOptCode);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("BACRDOE_OPT2=%d"), test_site.nBarcodeMode);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("MDL_CNT=%d"), test_site.nModuleCount);
	strData += strTemp;
	strData += _T(" ");
	
	strBarcode = _T("");
	for (i=0; i<8; i++)
	{
		if (test_site.st_pcb_info[i].nYesNo == YES)
		{
			if (test_site.nBarcodeMode == 1)
			{
				//if (test_site.st_pcb_info[i].nRetestCnt == 0)
				//kwlee 2017.0728
				if (test_site.st_pcb_info[i].nRetestCnt == 0 && st_recipe_info.nTestRetest_Count != 0)
				{
					strTemp.Format(_T("MDL%02d=%s,P,I"), i+1, test_site.st_pcb_info[i].strBarcode1D[0]);
				}
				else 
				{
					//2016.1109
					//if (test_site.st_pcb_info[i].nRetestCnt >= st_recipe_info.nTestRetest_Count)
					if( test_site.st_pcb_info[i].nRetestCnt >= test_site.st_pcb_info[i].nRetest )
					{
						strTemp.Format(_T("MDL%02d=%s,%dR,F"), i+1, test_site.st_pcb_info[i].strBarcode1D[0],
							test_site.st_pcb_info[i].nRetestCnt);
					}
					else
					{
						strTemp.Format(_T("MDL%02d=%s,%dR,I"), i+1, test_site.st_pcb_info[i].strBarcode1D[0],
							test_site.st_pcb_info[i].nRetestCnt);
					}
				}
			}
			else
			{
				//if (test_site.st_pcb_info[i].nRetestCnt == 0)
				//kwlee 2017.0728
				if (test_site.st_pcb_info[i].nRetestCnt == 0 && st_recipe_info.nTestRetest_Count != 0)
				{
					strTemp.Format(_T("MDL%02d=%s,%s,P,I"), i+1, test_site.st_pcb_info[i].strBarcode1D[0],
						test_site.st_pcb_info[i].strBarcode1D[1]);
				}
				else
				{
					if (test_site.st_pcb_info[i].nRetestCnt >= st_recipe_info.nTestRetest_Count)
					{
						strTemp.Format(_T("MDL%02d=%s,%s,%dR,F"), i+1, test_site.st_pcb_info[i].strBarcode1D[0],
							test_site.st_pcb_info[i].strBarcode1D[1],
							test_site.st_pcb_info[i].nRetestCnt);
					}
					else
					{
						strTemp.Format(_T("MDL%02d=%s,%s,%dR,I"), i+1, test_site.st_pcb_info[i].strBarcode1D[0],
							test_site.st_pcb_info[i].strBarcode1D[1],
							test_site.st_pcb_info[i].nRetestCnt);
					}
				}
			}
		}
		else
		{
			strTemp.Format(_T("MDL%02d=NULL"), i+1);
		}

		test_site.st_pcb_info[i].strBarcode = strTemp;

		strBarcode += strTemp;
		strBarcode += _T(" ");
	}
	strData += strBarcode;

	strMsg.Format(_T("%016d%s"), strData.GetLength(), strData);

	OnMessageWrite(strMsg);
}


void CClientInterface3::OnTesterStart(tagTEST_SITE_INFO &test_site)
{
	CString strTemp;
	CString strData;
	CString strMsg;

	m_nRpyBin[INTER_START]	= BD_NONE;

	strData = _T("FUNCTION=TEST_START");
	strData += _T(" ");

	strTemp.Format(_T("LOT_ID=%s"), test_site.strLotNo);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("EQP_ID=%s"), test_site.strEqpID);
	strData += strTemp;
	strData += _T(" ");

	switch (test_site.nTestMode)
	{ 
		case 0:
			strTemp.Format(_T("TEST_MODE=%s"), _T("ONESHOT"));
			break;

		case 1:
			strTemp.Format(_T("TEST_MODE=%s"), _T("HOT"));
			break;

		case 2:
			strTemp.Format(_T("TEST_MODE=%s"), _T("COLD"));
			break;

		case 3:
			strTemp.Format(_T("TEST_MODE=%s"), _T("GATE"));
			break;
	}
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("PART_NO=%s"), test_site.strPartNo);
	strData += strTemp;

	strMsg.Format(_T("%016d%s"), strData.GetLength(), strData);

	OnMessageWrite(strMsg);
}


void CClientInterface3::OnPgmDownload(tagTEST_SITE_INFO &test_site)
{
	CString strTemp;
	CString strData;
	CString strMsg;

	m_nRpyBin[INTER_PGM]	= BD_NONE;
	test_site.nPgmDown = NO;

	strData = _T("FUNCTION=PGM_LOAD");
	strData += _T(" ");

	strTemp.Format(_T("LOT_ID=%s"), test_site.strLotNo);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("EQP_ID=%s"), test_site.strEqpID);
	strData += strTemp;
	strData += _T(" ");

	switch (test_site.nTestMode)
	{ 
		case 0:
			strTemp.Format(_T("TEST_MODE=%s"), _T("ONESHOT"));
			break;

		case 1:
			strTemp.Format(_T("TEST_MODE=%s"), _T("HOT"));
			break;

		case 2:
			strTemp.Format(_T("TEST_MODE=%s"), _T("COLD"));
			break;

		case 3:
			strTemp.Format(_T("TEST_MODE=%s"), _T("GATE"));
			break;
	}
	strData += strTemp;
	strData += _T(" ");

	if (test_site.nInterface == 0)
	{
		strTemp.Format(_T("PGM_SEARCH=%s"), _T("FTP"));
	}
	else
	{
		strTemp.Format(_T("PGM_SEARCH=%s"), _T("LOCAL"));
	}
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("PART_NO=%s"), test_site.strPartNo);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("OPT=%s"), test_site.strOptCode);
	strData += strTemp;

	strMsg.Format(_T("%016d%s"), strData.GetLength(), strData);

	OnMessageWrite(strMsg);
}


void CClientInterface3::OnLotComplete(tagTEST_SITE_INFO &test_site)
{
	CString strTemp;
	CString strData;
	CString strMsg;

	m_nRpyBin[INTER_TOTAL]	= BD_NONE;

	strData = _T("FUNCTION=LOT_TOTAL");
	strData += _T(" ");

	strTemp.Format(_T("LOT_ID=%s"), test_site.strLotNo);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("EQP_ID=%s"), test_site.strEqpID);
	strData += strTemp;
	strData += _T(" ");

	switch (test_site.nTestMode)
	{ 
		case 0:
			strTemp.Format(_T("TEST_MODE=%s"), _T("ONESHOT"));
			break;

		case 1:
			strTemp.Format(_T("TEST_MODE=%s"), _T("HOT"));
			break;

		case 2:
			strTemp.Format(_T("TEST_MODE=%s"), _T("COLD"));
			break;

		case 3:
			strTemp.Format(_T("TEST_MODE=%s"), _T("GATE"));
			break;
	}
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("PART_NO=%s"), test_site.strPartNo);
	strData += strTemp;
	strData += _T(" ");

	strTemp = _T("PASS_CNT=");
	strData += strTemp;
	strData += _T(" ");

	strTemp = _T("FAIL_CNT=");
	strData += strTemp;

	strMsg.Format(_T("%016d%s"), strData.GetLength(), strData);

	OnMessageWrite(strMsg);
}


void CClientInterface3::OnTesterOption(tagTEST_SITE_INFO &test_site)
{
	CString strTemp;
	CString strData;
	CString strMsg;

	m_nRpyBin[INTER_OPTION] = BD_NONE;

	strData = _T("FUNCTION=TESTER_READY");
	strData += _T(" ");

	strTemp.Format(_T("LOT_ID=%s"), test_site.strLotNo);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("EQP_ID=%s"), test_site.strEqpID);
	strData += strTemp;
	strData += _T(" ");

	switch (test_site.nTestMode)
	{ 
		case 0:
			strTemp.Format(_T("TEST_MODE=%s"), _T("ONESHOT"));
			break;

		case 1:
			strTemp.Format(_T("TEST_MODE=%s"), _T("HOT"));
			break;

		case 2:
			strTemp.Format(_T("TEST_MODE=%s"), _T("COLD"));
			break;

		case 3:
			strTemp.Format(_T("TEST_MODE=%s"), _T("GATE"));
			break;
	}
	strData += strTemp;
	strData += _T(" ");

	if (test_site.nInterface == 0)
	{
		strTemp.Format(_T("PGM_SEARCH=%s"), _T("FTP"));
	}
	else
	{
		strTemp.Format(_T("PGM_SEARCH=%s"), _T("LOCAL"));
	}
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("PART_NO=%s"), test_site.strPartNo);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("RETRY_COUNT=%d"), test_site.nRetry);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("QTY=%d"), test_site.nQty);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("OPT=%s"), test_site.strOptCode);
	strData += strTemp;

	strMsg.Format(_T("%016d%s"), strData.GetLength(), strData);

	OnMessageWrite(strMsg);
}


void CClientInterface3::OnTesterAbort(tagTEST_SITE_INFO &test_site)
{
	CString strTemp;
	CString strData;
	CString strMsg;

	m_nRpyBin[INTER_ABORT]	= BD_NONE;

	strData = _T("FUNCTION=TEST_ABORT");
	strData += _T(" ");

	strTemp.Format(_T("LOT_ID=%s"), test_site.strLotNo);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("EQP_ID=%s"), test_site.strEqpID);
	strData += strTemp;
	strData += _T(" ");

	switch (test_site.nTestMode)
	{ 
		case 0:
			strTemp.Format(_T("TEST_MODE=%s"), _T("ONESHOT"));
			break;

		case 1:
			strTemp.Format(_T("TEST_MODE=%s"), _T("HOT"));
			break;

		case 2:
			strTemp.Format(_T("TEST_MODE=%s"), _T("COLD"));
			break;

		case 3:
			strTemp.Format(_T("TEST_MODE=%s"), _T("GATE"));
			break;
	}
	strData += strTemp;

	strMsg.Format(_T("%016d%s"), strData.GetLength(), strData);

	OnMessageWrite(strMsg);
}


void CClientInterface3::OnChamberDoor(tagTEST_SITE_INFO &test_site, int nStatus)
{
	CString strTemp;
	CString strData;
	CString strMsg;

	m_nRpyBin[INTER_DOOR]	= BD_NONE;

	strData = _T("FUNCTION=CHAMBER_DOOR");
	strData += _T(" ");

	strTemp.Format(_T("EQP_ID=%s"), test_site.strEqpID);
	strData += strTemp;
	strData += _T(" ");

	if (nStatus == 0)
	{ 
		strTemp = _T("TEST_MODE=OPEN");
	}
	else
	{
		strTemp = _T("TEST_MODE=CLOSE");
	}

	strData += strTemp;

	strMsg.Format(_T("%016d%s"), strData.GetLength(), strData);

	OnMessageWrite(strMsg);
}
