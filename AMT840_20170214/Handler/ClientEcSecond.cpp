#include "StdAfx.h"
#include "ClientEcSecond.h"
#include "CtlBd_Library.h"
#include "Ctlbd_Variable.h"
#include "PublicFunction.h"
#include "AlgMemory.h"
#include "LogFromat.h"

#define EC_SECOND_CLIENT_MSG_NAME		"EC_SECOND_CLIENT_SHAREMEM"

CClientEcSecond clsEcSecond;

CClientEcSecond::CClientEcSecond(void)
{
	m_thrHandle = NULL;

	// 공유 메모리 선언...../////////////////////////////////////////////////////////////////////////////////////////
	m_hMemMapEcSecond	= NULL;
	m_pQueueEcSecond	= NULL;

	m_hMemMapEcSecond	= CreateFileMapping(INVALID_HANDLE_VALUE,
										    NULL,
										    PAGE_READWRITE,
										    0,
										    sizeof(EC_S_CLIENT_QUEUE),
										    (LPCWSTR)EC_SECOND_CLIENT_MSG_NAME);
	if (m_hMemMapEcSecond != NULL)
	{	
		if (GetLastError() != ERROR_ALREADY_EXISTS)
		{	
			m_pQueueEcSecond = (P_EC_S_CLIENT_QUEUE)MapViewOfFile(m_hMemMapEcSecond, FILE_MAP_ALL_ACCESS, 0, 0, 0);
			if (m_pQueueEcSecond != NULL)
			{
				m_pQueueEcSecond->m_nFront	= 0;
				m_pQueueEcSecond->m_nRear	= 0;
				m_pQueueEcSecond->m_nCount	= 0;
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	m_nCommStep = 0;
	m_strHeader = _T("");
	m_nHeader	= 16;
}


CClientEcSecond::~CClientEcSecond(void)
{
	if (m_pQueueEcSecond	!= NULL)	UnmapViewOfFile(m_pQueueEcSecond);	
	if (m_hMemMapEcSecond	!= NULL)	CloseHandle(m_hMemMapEcSecond);
}

void CClientEcSecond::OnOpen()
{
	m_hMemMapEcSecond	= CreateFileMapping(INVALID_HANDLE_VALUE,
											NULL,
											PAGE_READWRITE,
											0,
											sizeof(EC_S_CLIENT_QUEUE),
											(LPCWSTR)EC_SECOND_CLIENT_MSG_NAME);
	if (m_hMemMapEcSecond != NULL)
	{	
		if (GetLastError() != ERROR_ALREADY_EXISTS)
		{	
			m_pQueueEcSecond = (P_EC_S_CLIENT_QUEUE)MapViewOfFile(m_hMemMapEcSecond, FILE_MAP_ALL_ACCESS, 0, 0, 0);
			if (m_pQueueEcSecond != NULL)
			{
				m_pQueueEcSecond->m_nFront	= 0;
				m_pQueueEcSecond->m_nRear	= 0;
				m_pQueueEcSecond->m_nCount	= 0;
			}
		}
	}
}


void CClientEcSecond::OnClose()
{
	if (m_pQueueEcSecond	!= NULL)	UnmapViewOfFile(m_pQueueEcSecond);	
	if (m_hMemMapEcSecond	!= NULL)	CloseHandle(m_hMemMapEcSecond);
}

CString	CClientEcSecond::OnCharToString(char chData[], int nLength)
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

void CClientEcSecond::OnStringToChar(CString strData, char chData[])
{
	int nCnt;
	wchar_t *chBuf;

	chBuf = strData.GetBuffer(strData.GetLength());
	nCnt  = WideCharToMultiByte(CP_ACP, 0, chBuf, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, chBuf, -1, chData, nCnt, 0, 0);
}

void CClientEcSecond::OnMessageWrite(CString strMsg)
{
	CString strLog = _T("[HANDLER >> EC SERVER] ");

	if (m_hMemMapEcSecond != NULL && m_pQueueEcSecond != NULL)
	{
		strLog += strMsg;
		clsFunc.OnLogFileAdd(4, strLog);

		OnStringToChar(strMsg, m_pQueueEcSecond->m_chMsg[m_pQueueEcSecond->m_nRear]);

		m_pQueueEcSecond->m_nRear++;
		
		if (m_pQueueEcSecond->m_nRear >= 50) m_pQueueEcSecond->m_nRear = 0;
		
		m_pQueueEcSecond->m_nCount++;
	}
}

CString	CClientEcSecond::OnMessageRead()
{
	CString strData = _T("");
	
	if (m_hMemMapEcSecond!= NULL && m_pQueueEcSecond != NULL)
	{
		if (m_pQueueEcSecond->m_nCount > 0 )
		{
			strData = m_pQueueEcSecond->m_chMsg[m_pQueueEcSecond->m_nFront];
			
			m_pQueueEcSecond->m_nFront++;
			if (m_pQueueEcSecond->m_nFront >= 50) m_pQueueEcSecond->m_nFront = 0;
			
			m_pQueueEcSecond->m_nCount--;
		}
	}

	return strData;
}


void CClientEcSecond::OnDataReceive(CString strMsg)
{
	int nHead;
	int nLength;
	int nPos;

	CString strTemp;
	CString strData;
	CString strFunc;
	CString strLog = _T("[EC SERVER >> HANDLER] ");

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
		strTemp.Format(_T("[EC_SECOND_NETWORK] %s"), strMsg);
		clsMem.OnNormalMessageWrite(strTemp);

//		st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
	}
}


void CClientEcSecond::OnDataAnalysis(CString strMsg)
{
	CString strFunc;
	CString strTemp;
	CString strData;
	CString strRpy;
	CString strEqpModel;
	CString strLogKey[10];
	CString strLogData[10];	
	
	int nPos;


	int i, j;
	int nCount = 0;

	strFunc = OnDataItemAnalysis(0, 0, strMsg, _T("FUNCTION_RPY"));

	if (strFunc == _T("LOTDISPLAY"))
	{
		strData = OnDataItemAnalysis(0, 0, strMsg, _T("RESULT"));
		if (strData == _T("OK"))
		{
			st_lot_display_info[1].strLotNo			= OnDataItemAnalysis(0, 0, strMsg, _T("LOTID"));
			st_lot_display_info[1].strPartNo			= OnDataItemAnalysis(0, 0, strMsg, _T("PARTID"));
			st_lot_display_info[1].nQty				= _wtoi(OnDataItemAnalysis(0, 0, strMsg, _T("QTY")));
			st_lot_display_info[1].strCycleID		= OnDataItemAnalysis(0, 0, strMsg, _T("CYCLEID"));
			st_lot_display_info[1].strStepCnt		= OnDataItemAnalysis(0, 0, strMsg, _T("STEPCNT"));
			st_lot_display_info[1].strRetestKind		= OnDataItemAnalysis(0, 0, strMsg, _T("RETESTKIND"));
			st_lot_display_info[1].strRetestRes		= OnDataItemAnalysis(0, 0, strMsg, _T("RETESTRES"));
			st_lot_display_info[1].strOneShotFlag	= OnDataItemAnalysis(0, 0, strMsg, _T("ONESHOTFLAG"));
			st_lot_display_info[1].strTier			= OnDataItemAnalysis(0, 0, strMsg, _T("TIER"));
			st_lot_display_info[1].strOptcode		= OnDataItemAnalysis(0, 0, strMsg, _T("OPTCODE"));
			st_lot_display_info[1].strFabSite		= OnDataItemAnalysis(0, 0, strMsg, _T("FAB_SITE"));
			st_lot_display_info[1].strReworkCnt		= OnDataItemAnalysis(0, 0, strMsg, _T("REWORKCNT"));

			m_nLotDisplayFlag = m_nCommBin = BD_DATA_GOOD;
		}
		else
		{
			strData = OnDataItemAnalysis(0, 0, strMsg, _T("MSG"));
			if (strData != _T(""))
			{
				m_strErrorMsg	= strData.Mid(1, strData.GetLength()-1);
			}
			m_nLotDisplayFlag = m_nCommBin = BD_DATA_REJECT;
		}
	}
	else if (strFunc == _T("LOT_START"))
	{
		strData = OnDataItemAnalysis(0, 0, strMsg, _T("RESULT"));
		if (strData == _T("OK"))
		{
			// lot start 성공
			st_tray_info[st_work_info.nLdCvySite].strLotNo	= st_lot_display_info[0].strLotNo;
			st_tray_info[st_work_info.nLdCvySite].strPartNo	= st_lot_display_info[0].strPartNo;
			st_tray_info[st_work_info.nLdCvySite].nNewLotIn	= YES;

			if (st_handler_info.cWndMain != NULL)
			{
				st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_LOT_START_DELETE_REQ, 0);
			}
			m_nLotStartFlag = m_nCommBin = BD_DATA_GOOD;
		}
		else
		{
			strData = OnDataItemAnalysis(0, 0, strMsg, _T("MSG"));
			if (strData != _T(""))
			{
				m_strErrorMsg	= strData.Mid(1, strData.GetLength()-1);
			}
			m_nLotStartFlag = m_nCommBin = BD_DATA_REJECT;
		}
	}
	else if (strFunc == _T("ONLINE_END"))
	{
		strData = OnDataItemAnalysis(0, 0, strMsg, _T("RESULT"));
		if (strData == _T("OK"))
		{
			m_nOnlineEndFlag = m_nCommBin = BD_DATA_GOOD;
		}
		else
		{
			strData = OnDataItemAnalysis(0, 0, strMsg, _T("MSG"));
			if (strData != _T(""))
			{
				m_strErrorMsg	= strData.Mid(1, strData.GetLength()-1);
			}
			m_nOnlineEndFlag = m_nCommBin = BD_DATA_REJECT;
		}
	}
	else if (strFunc == _T("SCRAP_INFO"))
	{
		strData = OnDataItemAnalysis(0, 0, strMsg, _T("RESULT"));
		if (strData == _T("OK"))
		{
// 			st_lot_display_info[1].nScrapCnt = 0;
// 			strData = OnDataItemAnalysis(0, 0, strMsg, _T("SCRAP_CNT"));
// 			//st_lot_display_info.nScrapCnt	= _wtoi(strData);
// 			nCount	= _wtoi(strData);
// 
// 			if (nCount > 0)
// 			{
// 				for (i=0; i < nCount; i++)
// 				{
// 					strTemp.Format(_T("SCRAP_INFO%02d"), i + 1);
// 					strData = OnDataItemAnalysis(2, 0, strMsg, strTemp);
// 
// 					st_lot_display_info[1].strScrapCode[st_lot_display_info[1].nScrapCnt]	= OnDataItemAnalysis(0, 0, strData, _T("SCRAP_CODE"));
// 					st_lot_display_info[1].strScrapSerial[st_lot_display_info[1].nScrapCnt]	= OnDataItemAnalysis(0, 0, strData, _T("SERIAL"));
// 					st_lot_display_info[1].nScrapCnt++;
// 				}
// 			}
// 			m_nScrapInfoFlag = m_nCommBin = BD_DATA_GOOD;
			st_lot_display_info[1].nScrapCnt = 0;
			st_lot_display_info[1].nCodeCount = 0;
			for ( i = 0; i < 100; i++ )
			{
				st_lot_display_info[1].nCode[i] = 0;
				st_lot_display_info[1].strCode[i] = _T("");
			}
			strData = OnDataItemAnalysis(0, 0, strMsg, _T("SCRAP_CNT"));
			//st_lot_display_info.nScrapCnt	= _wtoi(strData);
			nCount	= _wtoi(strData);

			if (nCount > 0)
			{
				for (i=0; i < nCount; i++)
				{
					strTemp.Format(_T("SCRAP_INFO%03d"), i + 1);
					//strData = OnDataItemAnalysis(2, 0, strMsg, strTemp);
					//2016.1017
					strData = OnDataItemAnalysis(4, 0, strMsg, strTemp);

					st_lot_display_info[1].strScrapCode[st_lot_display_info[1].nScrapCnt]	= OnDataItemAnalysis(0, 0, strData, _T("SCRAP_CODE"));
					st_lot_display_info[1].strScrapSerial[st_lot_display_info[1].nScrapCnt]	= OnDataItemAnalysis(0, 0, strData, _T("SERIAL"));
/*					st_lot_display_info[1].nScrapCnt++;*/

					// scrap code별 수량
					if (st_lot_display_info[1].nCodeCount == 0)
					{
						// scrap code 수량이 0이면
						st_lot_display_info[1].nCode[st_lot_display_info[1].nCodeCount]++;
						st_lot_display_info[1].strCode[st_lot_display_info[1].nCodeCount] = st_lot_display_info[1].strScrapCode[st_lot_display_info[1].nScrapCnt];
						st_lot_display_info[1].nCodeCount++;
					}
					else
					{
						// scrap code가 있으면 같은 scrap code 증가하기 위해 
						nPos = -1;
						for (j=0; j<st_lot_display_info[1].nCodeCount; j++)
						{
							if (st_lot_display_info[1].strCode[j] == st_lot_display_info[1].strScrapCode[st_lot_display_info[1].nScrapCnt])
							{
								nPos = j;
								break;
							}
						}

						if (nPos < 0)
						{
							// 동일한 scrap code가 없으면 
							st_lot_display_info[1].nCode[st_lot_display_info[1].nCodeCount]++;
							st_lot_display_info[1].strCode[st_lot_display_info[1].nCodeCount] = st_lot_display_info[1].strScrapCode[st_lot_display_info[1].nScrapCnt];
							st_lot_display_info[1].nCodeCount++;
						}
						else
						{
							// 동일한 scrap code가 있으면 
							st_lot_display_info[1].nCode[nPos]++;
							st_lot_display_info[1].strCode[nPos] = st_lot_display_info[1].strScrapCode[st_lot_display_info[1].nScrapCnt];
						}
					}

					st_lot_display_info[1].nScrapCnt++;
				}
			}
			m_nScrapInfoFlag = m_nCommBin = BD_DATA_GOOD;
		}
		else
		{
			strData = OnDataItemAnalysis(0, 0, strMsg, _T("MSG"));
			if (strData != _T(""))
			{
				m_strErrorMsg	= strData.Mid(1, strData.GetLength()-1);
			}
			m_nScrapInfoFlag = m_nCommBin = BD_DATA_REJECT;
		}
	}
	else if (strFunc == _T("LOT_CANCEL"))
	{
		strData = OnDataItemAnalysis(0, 0, strMsg, _T("RESULT"));
		if (strData == _T("OK"))
		{
			m_nLotCancelFlag = m_nCommBin = BD_DATA_GOOD;
		}
		else
		{
			strData = OnDataItemAnalysis(0, 0, strMsg, _T("MSG"));
			if (strData != _T(""))
			{
				m_strErrorMsg	= strData.Mid(1, strData.GetLength()-1);
			}
			m_nLotCancelFlag = m_nCommBin = BD_DATA_REJECT;
		}
	}
	else if (strFunc == _T("DELETE_SCRAP"))
	{
		strData = OnDataItemAnalysis(0, 0, strMsg, _T("RESULT"));
		if (strData == _T("OK"))
		{
			m_nDelScrapFlg = m_nCommBin = BD_DATA_GOOD;
		}
		else
		{
			strData = OnDataItemAnalysis(0, 0, strMsg, _T("MSG"));
			if (strData != _T(""))
			{
				m_strErrorMsg	= strData.Mid(1, strData.GetLength()-1);
			}
			m_nDelScrapFlg = m_nCommBin = BD_DATA_REJECT;
		}
	}
	else if (strFunc == _T("LOT_END"))
	{
		strData = OnDataItemAnalysis(0, 0, strMsg, _T("RESULT"));
		if (strData == _T("OK"))
		{
			//2017.0109
			strLogKey[0]	= _T("TYPE");
			strLogData[0]	= _T("INTERFACE HANDLER");
			clsLog.LogLotInform(_T("INTERFACE"), _T("LOTOUT"), st_track_info.strLotNo, st_basic_info.strDeviceName, _T(""), 1, strLogKey, strLogData);

			st_track_info.nTestEndBin		= NO;
			st_track_info.strLotNo			= _T("");
			st_track_info.strPartNo			= _T("");
			st_track_info.nIn				= 0;
			st_track_info.nPrimePass		= 0;
			st_track_info.nPrimeReject		= 0;
			st_track_info.nCumPass			= 0;
			st_track_info.nCumReject		= 0;

			if (st_track_info.nStatus == YES)
			{
				st_track_info.nStatus = NO;
			}

			m_nLotEndFlag = m_nCommBin = BD_DATA_GOOD;

			if (st_basic_info.nModeRfid == NO)
			{
				if (st_handler_info.cWndMain != NULL)
				{
 					st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_LOT_END_DELETE_REQ, 0);
				}
			}
		}
		else
		{
			strData = OnDataItemAnalysis(0, 0, strMsg, _T("MSG"));
			if (strData != _T(""))
			{
				m_strErrorMsg	= strData.Mid(1, strData.GetLength()-1);
			}
			m_nLotEndFlag = m_nCommBin = BD_DATA_REJECT;
		}
	}
	else if (strFunc == _T("VERSION_UPDATE"))
	{
		strData = OnDataItemAnalysis(0, 0, strMsg, _T("RESULT"));
		if (strData == _T("PASS"))
		{
			m_nVerFlag = m_nCommBin = BD_DATA_GOOD;
		}
		else
		{
			strData = OnDataItemAnalysis(0, 0, strMsg, _T("MSG"));
			if (strData != _T(""))
			{
				m_strErrorMsg	= strData.Mid(1, strData.GetLength()-1);
			}
			m_nVerFlag = m_nCommBin = BD_DATA_REJECT;
		}
	}
	else if (strFunc == _T("EQP_STATUS_CHANGE"))
	{
		strData = OnDataItemAnalysis(0, 0, strMsg, _T("RESULT"));
		if (strData == _T("PASS"))
		{
			m_nStatusFlag = m_nCommBin = BD_DATA_GOOD;
		}
		else
		{
			strData = OnDataItemAnalysis(0, 0, strMsg, _T("MSG"));
			if (strData != _T(""))
			{
				m_strErrorMsg	= strData.Mid(1, strData.GetLength()-1);
			}
			m_nStatusFlag = m_nCommBin = BD_DATA_REJECT;
		}
	}
	else if (strFunc == _T("EQP_MODE_CHANGE"))
	{
		strData = OnDataItemAnalysis(0, 0, strMsg, _T("RESULT"));
		if (strData == _T("PASS"))
		{
			m_nModeChangFlag = m_nCommBin = BD_DATA_GOOD;
		}
		else
		{
			strData = OnDataItemAnalysis(0, 0, strMsg, _T("MSG"));
			if (strData != _T(""))
			{
				m_strErrorMsg	= strData.Mid(1, strData.GetLength()-1);
			}
			m_nModeChangFlag = m_nCommBin = BD_DATA_REJECT;
		}
	}
	else if (strFunc == _T("TOOL_CHANGE"))
	{
		strData = OnDataItemAnalysis(0, 0, strMsg, _T("RESULT"));
		if (strData == _T("PASS"))
		{
			m_nToolChangFlag = m_nCommBin = BD_DATA_GOOD;
		}
		else
		{
			strData = OnDataItemAnalysis(0, 0, strMsg, _T("MSG"));
			if (strData != _T(""))
			{
				m_strErrorMsg	= strData.Mid(1, strData.GetLength()-1);
			}
			m_nToolChangFlag = m_nCommBin = BD_DATA_REJECT;
		}
	}
	else if (strFunc == _T("MATCH_ID"))
	{
		strData = OnDataItemAnalysis(0, 0, strMsg, _T("RESULT"));
		if (strData == _T("PASS"))
		{
			m_nMatchFlag = m_nCommBin = BD_DATA_GOOD;
		}
		else
		{
			strData = OnDataItemAnalysis(0, 0, strMsg, _T("MSG"));
			if (strData != _T(""))
			{
				m_strErrorMsg	= strData.Mid(1, strData.GetLength()-1);
			}
			m_nMatchFlag = m_nCommBin = BD_DATA_REJECT;
		}
	}
	else if (strFunc == _T("DATE_TIME"))
	{
		strData = OnDataItemAnalysis(0, 0, strMsg, _T("RESULT"));
		if (strData == _T("PASS"))
		{
			m_nDateFlag = m_nCommBin = BD_DATA_GOOD;
		}
		else
		{
			strData = OnDataItemAnalysis(0, 0, strMsg, _T("MSG"));
			if (strData != _T(""))
			{
				m_strErrorMsg	= strData.Mid(1, strData.GetLength()-1);
			}
			m_nDateFlag = m_nCommBin = BD_DATA_REJECT;
		}
	}
}


CString CClientEcSecond::OnDataItemAnalysis(int nOpt, int nPos, CString strMsg, CString strFind)
{
	CString strData = _T("");
	CString strHead;
	int length;
	int pos[4];

	
	length = strMsg.GetLength();

	pos[0] = -1;
	pos[0] = strMsg.Find(strFind, nPos);

	if(pos[0] < 0) return strData;

	pos[1] = -1;
	pos[1] = strMsg.Find(_T("="), pos[0]+1);
	if(pos[1] < 0) return strData;

	pos[2] = -1;
	pos[2] = strMsg.Find(_T(" "), pos[0]+1);

	if(nOpt == 0)
	{
		if(pos[2] == -1)
		{
			strData = strMsg.Mid(pos[1]+1, length-(pos[1]+1));
			if( strData==_T(")") ) strData = _T("");//2016.1017
		}
		else
		{
			strData = strMsg.Mid(pos[1]+1, pos[2]-(pos[1]+1));
			if( strData==_T(")") ) strData = _T("");//2016.1017
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
	else if( nOpt == 4)//2016.1017
	{
		pos[2] = strMsg.Find( _T("("), pos[1]+1);
		pos[3] = strMsg.Find( _T(")"), pos[1]+1);
		int n_length =	pos[3] - pos[2] - 1;				
		strData = strMsg.Mid(pos[2] + 1, n_length);
	}
	return strData;
}

void CClientEcSecond::OnCommunication()
{
	if (st_handler_info.nRunStatus == dWARNING) return;

	switch (m_nCommStep)
	{
		case 0:
			// 공유메모리에 전송할 데이터가 있으면 
			if (m_pQueueEcSecond->m_nCount <= 0) break;
				
			m_nRetry		= 0;
			m_strSendMsg	= OnMessageRead();

			m_nCommStep = 100;
			break;

		case 100:
			// interface 상태 체크
			if (st_basic_info.nModeInterface == EQP_ON_LINE)
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
			if (st_client_info[EC_SECOND_NETWORK].nConnect == YES)
			{
				m_nCommStep = 2200;
			}
			else
			{
				m_dwWaitTime[0] = GetTickCount();

				::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + EC_SECOND_NETWORK, CLIENT_CONNECT, 0);

				m_nCommStep = 2100;
			}
			break;

		case 2100:
			if (st_client_info[EC_SECOND_NETWORK].nConnect == YES)
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

				if (m_dwWaitTime[2] > 60000)
				{
					m_nRetry++;

					if (m_nRetry > 3)
					{
						m_nCommBin	= BD_CONT_TIMEOUT;

						::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + EC_SECOND_NETWORK, CLIENT_CLOSE, 0);

						m_dwWaitTime[0] = GetTickCount();
						CTL_Lib.Alarm_Error_Occurrence(20000, dWARNING, _T("610000"));

						m_nCommStep = 0;
					}
					else
					{
						m_dwWaitTime[0] = GetTickCount();

						::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + EC_SECOND_NETWORK, CLIENT_CLOSE, 0);

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
			st_client_info[EC_SECOND_NETWORK].strSend = m_strSendMsg;

			::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + EC_SECOND_NETWORK, CLIENT_SEND, 0);

			m_nCommStep = 2300;
			break;

		case 2300:
			if (m_nCommBin == BD_DATA_GOOD)
			{
				m_dwWaitTime[0] = GetTickCount();
				::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + EC_SECOND_NETWORK, CLIENT_CLOSE, 0);

				m_nCommStep = 2350;
			}
			else if (m_nCommBin == BD_DATA_REJECT)
			{
				m_dwWaitTime[0] = GetTickCount();
				::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + EC_SECOND_NETWORK, CLIENT_CLOSE, 0);

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

				if (m_dwWaitTime[2] > 100000)
				{
					m_nRetry++;

					if (m_nRetry > 3)
					{
						m_nCommBin	= BD_RCV_TIMEOUT;

						::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + EC_SECOND_NETWORK, CLIENT_CLOSE, 0);

						m_dwWaitTime[0] = GetTickCount();
						CTL_Lib.Alarm_Error_Occurrence(20001, dWARNING, _T("610001"));

						m_nCommStep = 0;
					}
					else
					{
						m_dwWaitTime[0] = GetTickCount();
						::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + EC_SECOND_NETWORK, CLIENT_CLOSE, 0);

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


void CClientEcSecond::OnEqpID(CString strEqpID)
{
	m_strEqpID = strEqpID;
}


void CClientEcSecond::OnEcLotDisplay(CString strLotNo, CString strOperID)
{
	CString strTemp;
	CString strData;
	CString strMsg;
	
	m_nLotDisplayFlag = BD_NONE;

	strData = _T("FUNCTION=LOTDISPLAY");
	strData += _T(" ");
	
	strTemp.Format(_T("LOTID=%s"), strLotNo);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("EQPID=%s"), m_strEqpID);
	strData += strTemp;
	strData += _T(" ");

	if (strOperID == _T(""))
	{
		strData += _T("OPERID=AUTO");
	}
	else
	{
		strTemp.Format(_T("OPERID=%s"), strOperID);
		strData += strTemp;
	}
	strMsg.Format(_T("%016d%s"), strData.GetLength(), strData);

	OnMessageWrite(strMsg);
}


void CClientEcSecond::OnEcLotStart(CString strLotNo, CString strOperID, int nSocket)
{
	CString strTemp;
	CString strData;
	CString strMsg;
	
	int i, j;
	int nCount[2];

	nCount[0] = nCount[1] = 0;

	m_nLotStartFlag = BD_NONE;

	strData = _T("FUNCTION=LOT_START");
	strData += _T(" ");
	
	strTemp.Format(_T("LOTNO=%s"), strLotNo);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("EQPID=%s"), m_strEqpID);
	strData += strTemp;
	strData += _T(" ");

	if (strOperID == _T(""))
	{
		strData += _T("OPERID=AUTO");
	}
	else
	{
		strTemp.Format(_T("OPERID=%s"), strOperID);
		strData += strTemp;
	}
	strData += _T(" ");

	for (i=0; i<8; i++)
	{
		for (j=0; j<8; j++)
		{
			nCount[0]++;
			if (st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].nEnable == YES)
			{
				nCount[1]++;
			}
		}
	}
	strTemp.Format(_T("TOT_SOCK_CNT=%d USE_SOCK_CNT=%d"), nCount[0], nCount[1]);
	strData += strTemp;
	strData += strTemp;

	strMsg.Format(_T("%016d%s"), strData.GetLength(), strData);

	OnMessageWrite(strMsg);
}

void CClientEcSecond::OnEcOnlineEnd(CString strOperID, tagLOT_DISPLAY_INFO lot_display)
{
	CString strTemp;
	CString strData;
	CString strMsg;

	int i;

	m_nOnlineEndFlag = BD_NONE;

	strData = _T("FUNCTION=ONLINE_END");
	strData += _T(" ");
	
	strTemp.Format(_T("LOTID=%s"), lot_display.strLotNo);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("EQPID=%s"), m_strEqpID);
	strData += strTemp;
	strData += _T(" ");

	if (strOperID == _T(""))
	{
		strData += _T("OPERID=AUTO");
		strData += _T(" ");
	}
	else
	{
		strTemp.Format(_T("OPERID=%s"), strOperID);
		strData += strTemp;
		strData += _T(" ");
	}

	if (lot_display.nScrapCnt > 0)
	{
		//strTemp.Format(_T("SCRAP_CNT=%d"), lot_display.nScrapCnt);
		//strData += strTemp;
		//strData += _T(" ");

		for (i=0; i<lot_display.nScrapCnt; i++)
		{
			strTemp.Format(_T("SCRAP_INFO%03d=(SCRAP_CODE=%s ARRAYSN=%s SERIAL=%s)"), i + 1,
																					  lot_display.strScrapCode[i],
																					  lot_display.strArray[i],
																					  lot_display.strScrapSerial[i]);
			strData += strTemp;
			strData += _T(" ");
		}

		strTemp.Format(_T("SCRAP_CNT=%d"), lot_display.nScrapCnt);
		strData += strTemp;
		strData += _T(" ");

	}
	else
	{
		strData += _T("SCRAP_CNT=0");
	}

	strMsg.Format(_T("%016d%s"), strData.GetLength(), strData);

	OnMessageWrite(strMsg);
}

//kwlee 2016.1017 Test
//void CClientEcSecond::OnEcLotEnd(CString strLotNo, CString strOperID, CString strSplitLotNo, int nPass, int nFail)
void CClientEcSecond::OnEcLotEnd(CString strLotNo, CString strOperID, CString strSplitLotNo, int nPass, CTime tStart, int nBdTime, int nBdTimeCount, tagLOT_DISPLAY_INFO lot_display)
{
	CString strTemp;
	CString strData;
	CString strMsg;

//	m_nLotEndFlag = BD_NONE;

	//FUNCTION=LOT_END EQPID=TH-86 OPERID=07588310 LOTID=SSAC000640 SPLITLOTID= GOODQTY=34 SCRAP_CNT=2 
		//SCRAP_INFO01=(SCRAP_CODE=4050 SCRAP_QTY=1 ) SCRAP_INFO02=(SCRAP_CODE=4097 SCRAP_QTY=8 ) 
		//START_TIME=20161006010958 END_TIME=20161006011707 TEST_TIME=426 ms TEST_PGM="M386AR2016092022" 
		//SPD="M386A4G40DM0-CPB0N1449" FILE="MNNNAWP.TXT" WP="NORMAL" COMMENT="" ASTFLAG=A


// strData = _T("FUNCTION=LOT_END");
// strData += _T(" ");
// 
// strTemp.Format(_T("EQPID=%s"), m_strEqpID);
// strData += strTemp;
// strData += _T(" ");
// 
// if (strOperID == _T(""))
// {
// 	strData += _T("OPERID=AUTO");
// 	strData += _T(" ");
// }
// else
// {
// 	strTemp.Format(_T("OPERID=%s"), strOperID);
// 	strData += strTemp;
// 	strData += _T(" ");
// }
// 
// //strTemp.Format(_T("LOTID=%s"), strLotNo);
// strTemp.Format(_T("LOTID=Y1FS000641"));
// strData += strTemp;
// strData += _T(" ");
// 
// strTemp.Format(_T("SPLITLOTID=%s"), strSplitLotNo);
// strData += strTemp;
// strData += _T(" ");
// 
// //strTemp.Format(_T("GOODQTY=%d"), nPass);
// strTemp.Format(_T("GOODQTY=41"));
// strData += strTemp;
// strData += _T(" ");
// 
// //test ybs 2016.1007
// // strTemp.Format(_T("SCRAP_CNT=%d"),nFail);
// // strData += strTemp;
// // strData += _T(" ");
// 
// strTemp.Format(_T("SCRAP_CNT=2"));
// strData += strTemp;
// strData += _T(" ");
// 
// strTemp = _T("SCRAP_INFO01=(SCRAP_CODE=4155 SCRAP_QTY=1 ) SCRAP_INFO02=(SCRAP_CODE=4144 SCRAP_QTY=1 )");
// strData += strTemp;
// strData += _T(" ");
// 
// strTemp = _T("START_TIME=20161006010958 END_TIME=20161006011707 TEST_TIME=426 ms");// TEST_PGM="M386AR2016092022";
// strData += strTemp;
// strData += _T(" ");
// //SPD="M386A4G40DM0-CPB0N1449" FILE="MNNNAWP.TXT" WP="NORMAL" COMMENT="" ASTFLAG=A
// //test 1007
// 
// strTemp.Format(_T("TEST_PGM=M386AR2016092022"));
// strTemp.Format(_T("TEST_PGM="));
// strData += strTemp;
// strData += _T(" ");
// 
// strTemp.Format(_T("SPD=M386A4G40DM0-CPB0N1449"));
// strTemp.Format(_T("SPD="));
// strData += strTemp;
// strData += _T(" ");
// 
// strTemp.Format(_T("FILE=MNNNAWP.TXT"));
// strTemp.Format(_T("FILE="));
// strData += strTemp;
// strData += _T(" ");
// 
// strTemp.Format(_T("WP=NORMAL"));
// strTemp.Format(_T("WP="));
// strData += strTemp;
// strData += _T(" ");
// 
// 
// strTemp.Format(_T("COMMENT="));
// strData += strTemp;
// strData += _T(" ");
// 
// strTemp.Format(_T("ASTFLAG=A"));
// strData += strTemp;
// 
// strMsg.Format(_T("%016d%s"), strData.GetLength(), strData);
// OnMessageWrite(strMsg);
// 	return;

	CTime tEnd;

	int nAve;
	int i;

	// jtkim 20161008
	tEnd = CTime::GetCurrentTime();

	m_nLotEndFlag = BD_NONE;

	strData = _T("FUNCTION=LOT_END");
	strData += _T(" ");

	strTemp.Format(_T("EQPID=%s"), m_strEqpID);
	strData += strTemp;
	strData += _T(" ");

	if (strOperID == _T(""))
	{
		strData += _T("OPERID=AUTO");
		strData += _T(" ");
	}
	else
	{
		strTemp.Format(_T("OPERID=%s"), strOperID);
		strData += strTemp;
		strData += _T(" ");
	}

	strTemp.Format(_T("LOTID=%s"), strLotNo);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("SPLITLOTID=%s"), strSplitLotNo);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("GOODQTY=%d"), nPass);
	strData += strTemp;
	strData += _T(" ");

	// jtkim 20161008
/*
	strTemp.Format(_T("SCRAP_CNT=%d"), nFail);
	strData += strTemp;
	strData += _T(" ");
*/
	strTemp.Format(_T("SCRAP_CNT=%d"), lot_display.nCodeCount);
	strData += strTemp;
	strData += _T(" ");

	for (i=0; i<lot_display.nCodeCount; i++)
	{
		//2016.1025
		//서버에서 SCRAP 을 받을때는 SCRAP_INF%03d 보낼때는 SCRAP_INF%02d 거지같은 서버
		strTemp.Format(_T("SCRAP_INFO%02d=(SCRAP_CODE=%s SCRAP_QTY=%d)"), i + 1,
																		  lot_display.strCode[i],
																		  lot_display.nCode[i]);
		strData += strTemp;
		strData += _T(" ");
	}

	strTemp.Format(_T("START_TIME=%04d%02d%02d%02d%02d%02d"), tStart.GetYear(),
															  tStart.GetMonth(),
															  tStart.GetDay(),
															  tStart.GetHour(),
															  tStart.GetMinute(),
															  tStart.GetSecond());
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("END_TIME=%04d%02d%02d%02d%02d%02d"), tEnd.GetYear(),
															tEnd.GetMonth(),
															tEnd.GetDay(),
															tEnd.GetHour(),
															tEnd.GetMinute(),
															tEnd.GetSecond());
	strData += strTemp;
	strData += _T(" ");

	nAve = 0;
	if (nBdTimeCount > 0)
	{
		nAve = nBdTime / nBdTimeCount;
	}
	strTemp.Format(_T("TEST_TIME=%d"), nAve);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("TEST_PGM="));
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("SPD="));
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("FILE="));
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("WP="));
	strData += strTemp;
	strData += _T(" ");


	strTemp.Format(_T("COMMENT="));
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("ASTFLAG=A"));
	strData += strTemp;

	strMsg.Format(_T("%016d%s"), strData.GetLength(), strData);
	OnMessageWrite(strMsg);
}


void CClientEcSecond::OnEcLotCancel(CString strLotNo, CString strOperID)
{
	CString strTemp;
	CString strData;
	CString strMsg;

	m_nLotCancelFlag = BD_NONE;

	strData = _T("FUNCTION=LOT_CANCEL");
	strData += _T(" ");

	strTemp.Format(_T("LOT_CNT=%d"), 1);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("LOT_01=%s"), strLotNo);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("EQPID=%s"), m_strEqpID);
	strData += strTemp;
	strData += _T(" ");

	if (strOperID == _T(""))
	{
		strData += _T("OPERID=AUTO");
	}
	else
	{
		strTemp.Format(_T("OPERID=%s"), strOperID);
		strData += strTemp;
	}
	strMsg.Format(_T("%016d%s"), strData.GetLength(), strData);

	OnMessageWrite(strMsg);
}


void CClientEcSecond::OnEcScrapInfo(CString strLotNo)
{
	CString strTemp;
	CString strData;
	CString strMsg;

	m_nScrapInfoFlag = BD_NONE;

	strData = _T("FUNCTION=SCRAP_INFO");
	strData += _T(" ");

	strTemp.Format(_T("EQPID=%s"), m_strEqpID);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("LOTID=%s"), strLotNo);
	strData += strTemp;
	//	strData += _T(" ");

	//	strData += _T("AUTOLINE=NO");

	strMsg.Format(_T("%016d%s"), strData.GetLength(), strData);

	OnMessageWrite(strMsg);
}


void CClientEcSecond::OnEcDeleteScrap(CString strLotNo, CString strSerial)
{
	CString strTemp;
	CString strData;
	CString strMsg;

	m_nDelScrapFlg = BD_NONE;

	strData = _T("FUNCTION=DELETE_SCRAP");
	strData += _T(" ");
	//kwlee 2016.1017 
	//strTemp.Format(_T("LOTNO=%s"), strLotNo);
	strTemp.Format(_T("LOTID=%s"), strLotNo);
	strData += strTemp;
	strData += _T(" ");
	//kwlee 2016.1017 추가
	strTemp.Format(_T("EQPID=%s"), st_basic_info.strEqp);
	strData += strTemp;
	strData += _T(" ");
	///
	strTemp.Format(_T("SERIAL=%s"), strSerial);
	strData += strTemp;
	strData += _T(" ");
	
	strData += _T("BYPASS=NO");

	strMsg.Format(_T("%016d%s"), strData.GetLength(), strData);

	OnMessageWrite(strMsg);
}


void CClientEcSecond::OnEcSLotEnd(tagPCB_INFO pcb_info)
{
	CString strTemp;
	CString strData;
	CString strMsg;
	
	strData = _T("FUNCTION=SLOT_END");
	strData += _T(" ");
	
	strTemp.Format(_T("EQPID=%s"), st_basic_info.strEqp);
	strData += strTemp;
	strData += _T(" ");

	strData += _T("OPERID=AUTO");
	strData += _T(" ");

	strTemp.Format(_T("LOTNO=%s"), pcb_info.strLotNo);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("PARTID=%s"), pcb_info.strPartNo);
	strData += strTemp;
	strData += _T(" ");
/*
	strTemp.Format(_T("SERIAL=%s"), pcb_info.strSerialNo);
	strData += strTemp;
	strData += _T(" ");

	if (pcb_info.nBin == BD_DATA_GOOD)
	{
		strData += _T("TESTRESULT=PASS");
		strData += _T(" ");
	}
	else
	{
		strData += _T("TESTRESULT=FAIL");
		strData += _T(" ");
	}

	strTemp.Format(_T("PPID=%s"), pcb_info.strPPID);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("WWN=%s"), pcb_info.strWWN);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("SCRAP_CODE=%d"), pcb_info.nScrCode);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("BIN=%d"), pcb_info.nBin);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("SLOT=%d"), pcb_info.nBdNum);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("ARRAY_SN=%s"), pcb_info.strArrNo);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("C_SERIAL=%s"), pcb_info.strCSerialNo);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("PSID=%s"), pcb_info.strPSID);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("P_SERIAL=%d"), pcb_info.strPSID);
	strData += strTemp;

	strMsg.Format(_T("%016d%s"), strData.GetLength(), strData);
*/
	OnMessageWrite(strMsg);
}

void CClientEcSecond::OnEcPcbInfo(CString strLotId, CString strPartId, CString strSerial)
{
	CString strTemp;
	CString strData;
	CString strMsg;
	
	strData = _T("FUNCTION=LABEL_SERIAL_INFO");
	strData += _T(" ");
	
	strTemp.Format(_T("EQPID=%s"), st_basic_info.strEqp);
	strData += strTemp;
	strData += _T(" ");
/*
	strTemp.Format(_T("LOTID=%s"), strLotId);
	strData += strTemp;
	strData += _T(" ");
*/
	strTemp.Format(_T("PARTID=%s"), strPartId);
	strData += strTemp;
	strData += _T(" ");

	strData += _T("OPERID=AUTO");
	strData += _T(" ");

	strTemp.Format(_T("SERIAL=%s"), strSerial);
	strData += strTemp;

	strMsg.Format(_T("%016d%s"), strData.GetLength(), strData);

	m_nPcbInfoFlag = BD_NONE;

	OnMessageWrite(strMsg);
}

void CClientEcSecond::OnEcTestEnd(CString strLotId, CString strPartId)
{
	CString strTemp;
	CString strData;
	CString strMsg;
	
	strData = _T("FUNCTION=TEST_END");
	strData += _T(" ");
	
	strTemp.Format(_T("EQPID=%s"), st_basic_info.strEqp);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("LOTID=%s"), strLotId);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("PARTNO=%s"), strPartId);
	strData += strTemp;

	strMsg.Format(_T("%016d%s"), strData.GetLength(), strData);

	m_nTestEndFlag = BD_NONE;

	OnMessageWrite(strMsg);
}


void CClientEcSecond::OnEcSeeLot(CString strLotId)
{
	CString strTemp;
	CString strData;
	CString strMsg;

	strData = _T("FUNCTION=SEE_LOT");
	strData += _T(" ");

	strTemp.Format(_T("LOTNO=%s"), strLotId);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("EQPID=%s"), st_basic_info.strEqp);
	strData += strTemp;
	strData += _T(" ");

	strData += _T("OPERID=AUTO");

	strMsg.Format(_T("%016d%s"), strData.GetLength(), strData);

	m_nSeeLotFlag = BD_NONE;

	OnMessageWrite(strMsg);
}


void CClientEcSecond::OnEqpVersionUpdate(CString strDescrip)
{
	CString strTemp;
	CString strData;
	CString strMsg;

	strData = _T("FUNCTION=VERSION_UPDATE");
	strData += _T(" ");

	strTemp.Format(_T("EQPID=%s"), st_basic_info.strEqp);
	strData += strTemp;
	strData += _T(" ");

	strData += _T("EQPMODEL=ROUTER");
	strData += _T(" ");

	strTemp.Format(_T("DESC=%s"), strDescrip);
	strData += strTemp;

	strMsg.Format(_T("%016d%s"), strData.GetLength(), strData);

	m_nVerFlag = BD_NONE;

	OnMessageWrite(strMsg);
}

void CClientEcSecond::OnEqpStatusChange(CString strMode, CString strCode, CString strDescrip)
{
	CString strTemp;
	CString strData;
	CString strMsg;

	strData = _T("FUNCTION=EQP_STATUS_CHANGE");
	strData += _T(" ");

	strTemp.Format(_T("EQPID=%s"), st_basic_info.strEqp);
	strData += strTemp;
	strData += _T(" ");

	strData += _T("OPERID=AUTO");
	strData += _T(" ");

	strTemp.Format(_T("MODE=%s"), strMode);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("CODE=%s"), strCode);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("COMMEND=%s"), strDescrip);
	strData += strTemp;
	strData += _T(" ");

	if (strMode == _T("ENABLE"))
	{
		strData += _T("ENABLEOK=Y");
	}
	else
	{
		strData += _T("ENABLEOK=N");
	}

	strMsg.Format(_T("%016d%s"), strData.GetLength(), strData);

	m_nStatusFlag = BD_NONE;

	OnMessageWrite(strMsg);
}

void CClientEcSecond::OnEqpModeChange(CString strMode)
{
	CString strTemp;
	CString strData;
	CString strMsg;

	strData = _T("FUNCTION=EQP_MODE_CHANGE");
	strData += _T(" ");

	strTemp.Format(_T("EQPID=%s"), st_basic_info.strEqp);
	strData += strTemp;
	strData += _T(" ");

	strData += _T("OPERID=AUTO");
	strData += _T(" ");

	strTemp.Format(_T("EQPMODE=%s"), strMode);
	strData += strTemp;

	strMsg.Format(_T("%016d%s"), strData.GetLength(), strData);

	m_nModeChangFlag = BD_NONE;

	OnMessageWrite(strMsg);
}

void CClientEcSecond::OnEqpFullCarrierMove(CString srtCarrierId)
{
	CString strTemp;
	CString strData;
	CString strMsg;

	strData = _T("FUNCTION=FULL_CARRIER_MOVE_REQ");
	strData += _T(" ");

	strTemp.Format(_T("EQPID=%s"), st_basic_info.strEqp);
	strData += strTemp;
	strData += _T(" ");

	strData += _T("LOTID=EMAGAZINE");
	strData += _T(" ");

	strData += _T("PORTID=AROUT-02_I1");
	strData += _T(" ");

	strTemp.Format(_T("CARRIERID=%s"), srtCarrierId);
	strData += strTemp;

	strMsg.Format(_T("%016d%s"), strData.GetLength(), strData);

	m_nFullCarrierFlag = BD_NONE;

	OnMessageWrite(strMsg);
}

void CClientEcSecond::OnEqpEmptyCarrierMove()
{
	CString strTemp;
	CString strData;
	CString strMsg;

	strData = _T("FUNCTION=EMPTY_CARRIER_MOVE_REQ");
	strData += _T(" ");

	strTemp.Format(_T("EQPID=%s"), st_basic_info.strEqp);
	strData += strTemp;
	strData += _T(" ");

	strData += _T("LOTID=");
	strData += _T(" ");

	strData += _T("PORTID=AROUT-01_I1");
	strData += _T(" ");

	strData += _T("CARRIERID=");
	strData += _T(" ");

	strData += _T("FEEDERID=");
	strData += _T(" ");

	strData += _T("MATCODE=");
	strData += _T(" ");

	strData += _T("QTY=");

	strMsg.Format(_T("%016d%s"), strData.GetLength(), strData);

	m_nEmptyCarrierFlag = BD_NONE;

	OnMessageWrite(strMsg);
}

void CClientEcSecond::OnEqpToolChange(CString strToolId)
{
	CString strTemp;
	CString strData;
	CString strMsg;

	strData = _T("FUNCTION=TOOL_CHANGE");
	strData += _T(" ");

	strTemp.Format(_T("EQPID=%s"), st_basic_info.strEqp);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("TOOID=%s"), strToolId);
	strData += strTemp;

	strMsg.Format(_T("%016d%s"), strData.GetLength(), strData);

	m_nToolChangFlag = BD_NONE;

	OnMessageWrite(strMsg);
}

void CClientEcSecond::OnEqpMatch(CString strLotId, CString strCarrierId)
{
	CString strTemp;
	CString strData;
	CString strMsg;

	strData = _T("FUNCTION=MATCH_ID");
	strData += _T(" ");

	strTemp.Format(_T("EQPID=%s"), st_basic_info.strEqp);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("CARRIER_ID=%s"), strCarrierId);
	strData += strTemp;
	strData += _T(" ");

	strTemp.Format(_T("LOT_ID=%s"), strLotId);
	strData += strTemp;

	strMsg.Format(_T("%016d%s"), strData.GetLength(), strData);

	m_nMatchFlag = BD_NONE;

	OnMessageWrite(strMsg);
}

void CClientEcSecond::OnEqpDate()
{
	CString strTemp;
	CString strData;
	CString strMsg;

	strData = _T("FUNCTION=DATE_TIME");
	strData += _T(" ");

	strTemp.Format(_T("EQPID=%s"), st_basic_info.strEqp);
	strData += strTemp;

	strMsg.Format(_T("%016d%s"), strData.GetLength(), strData);

	m_nDateFlag = BD_NONE;

	OnMessageWrite(strMsg);
}