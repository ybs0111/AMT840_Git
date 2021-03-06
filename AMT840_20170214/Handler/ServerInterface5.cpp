#include "StdAfx.h"
#include "ServerInterface5.h"
#include "PublicFunction.h"
#include "Variable.h"
#include "AlgMemory.h"
#include "RunTestSitePart5.h"
#include "FastechPublic_IO.h"
#include "LogFromat.h"
//kwlee 2017.0905
#include "XgemClient.h"

#define INTERFACE_SERVER_MSG_NAME_5		"INTERFACE_SERVER_SHAREMEM_5"

CServerInterface5 clsInterS5;

CServerInterface5::CServerInterface5(void)
{
	// 공유 메모리 선언...../////////////////////////////////////////////////////////////////////////////////////////
	m_hMemMapInter5	= NULL;
	m_pQueueInter5	= NULL;

	m_hMemMapInter5	= CreateFileMapping(INVALID_HANDLE_VALUE,
										NULL,
										PAGE_READWRITE,
										0,
										sizeof(INTER_SV_QUEUE_5),
										(LPCWSTR)INTERFACE_SERVER_MSG_NAME_5);
	if (m_hMemMapInter5 != NULL)
	{	
		if (GetLastError() != ERROR_ALREADY_EXISTS)
		{
			m_pQueueInter5 = (P_INTER_SV_QUEUE_5)MapViewOfFile(m_hMemMapInter5, FILE_MAP_ALL_ACCESS, 0, 0, 0);
			if (m_pQueueInter5 != NULL)
			{
				m_pQueueInter5->m_nFront	= 0;
				m_pQueueInter5->m_nRear		= 0;
				m_pQueueInter5->m_nCount	= 0;
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//m_nCommStep = 0;
	m_strHeader = _T("");
	m_nHeader	= 16;
}


CServerInterface5::~CServerInterface5(void)
{
	if (m_pQueueInter5 != NULL)	UnmapViewOfFile(m_pQueueInter5);	
	if (m_hMemMapInter5 != NULL) CloseHandle(m_hMemMapInter5);
}


CString	CServerInterface5::OnCharToString(char chData[], int nLength)
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

void CServerInterface5::OnMessageWrite(CString strMsg)
{
	CString strLog = _T("[HANDLER >> INTERFACE] ");

	if (m_hMemMapInter5 != NULL && m_pQueueInter5 != NULL)
	{
		strLog += strMsg;
		clsFunc.OnLogFileAdd(4, strLog);

		OnStringToChar(strMsg, m_pQueueInter5->m_chMsg[m_pQueueInter5->m_nRear]);

		m_pQueueInter5->m_nRear++;
		
		if (m_pQueueInter5->m_nRear >= 10) m_pQueueInter5->m_nRear = 0;
		
		m_pQueueInter5->m_nCount++;
	}
}

CString	CServerInterface5::OnMessageRead()
{
	CString strData = _T("");
	
	if (m_hMemMapInter5 != NULL && m_pQueueInter5 != NULL)
	{
		if (m_pQueueInter5->m_nCount > 0 )
		{
			strData = m_pQueueInter5->m_chMsg[m_pQueueInter5->m_nFront];
			
			m_pQueueInter5->m_nFront++;
			if (m_pQueueInter5->m_nFront >= 10) m_pQueueInter5->m_nFront = 0;
			
			m_pQueueInter5->m_nCount--;
		}
	}

	return strData;
}

void CServerInterface5::OnStringToChar(CString strData, char chData[])
{
	int nCnt;
	wchar_t *chBuf;

	chBuf = strData.GetBuffer(strData.GetLength());
	nCnt  = WideCharToMultiByte(CP_ACP, 0, chBuf, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, chBuf, -1, chData, nCnt, 0, 0);
}


void CServerInterface5::OnDataReceive(CString strMsg)
{
	int nHead;
	int nLength;
	int nPos;

	CString strTemp;
	CString strData;
	CString strLog = _T("[INTERFACE >> HANDLER] ");

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
			//OnDataAnalysis(strTemp);
//			clsMem.OnInterfaceMessagWrite(strTemp);
			OnMessageWrite(strTemp);
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
				//OnDataAnalysis(strTemp);
	//			clsMem.OnInterfaceMessagWrite(strTemp);
				OnMessageWrite(strTemp);
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
				//OnDataAnalysis(strTemp);
//				clsMem.OnInterfaceMessagWrite(strTemp);
				OnMessageWrite(strTemp);

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
					strTemp		= m_strData + strMsg.Mid(0, nLength - (m_nHeaderLength - m_strData.GetLength()));
					//OnDataAnalysis(strTemp);
//					clsMem.OnInterfaceMessagWrite(strTemp);
					OnMessageWrite(strTemp);

					nPos	= nLength - (m_nHeaderLength - m_strData.GetLength());
					strData = strMsg.Mid(nPos, nLength - nPos);

					m_nHeaderLength		= 0;
					m_strData			= _T("");

					if (m_strHeader == _T(""))
					{
						strTemp	= strData.Mid(0, m_nHeader);
					}
					else
					{
						nPos	= strData.Find(m_strHeader, 0);
						strTemp	= strData.Mid(nPos + m_strHeader.GetLength() + 1, m_nHeader);
					}
					nHead	= _wtoi(strTemp);

					if (nHead > 0)
					{
						nLength = strData.GetLength() - m_nHeader;

						if (nHead == nLength)
						{
							strTemp = strData.Mid(m_nHeader, nLength);
							//OnDataAnalysis(strTemp);
	//						clsMem.OnInterfaceMessagWrite(strTemp);
							OnMessageWrite(strTemp);
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
								strTemp = strData.Mid(m_nHeader, nHead);
								//OnDataAnalysis(strTemp);
	//							clsMem.OnInterfaceMessagWrite(strTemp);
								OnMessageWrite(strTemp);
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

	strTemp.Format(_T("[INTERFACE_NETWORK] %s"), strMsg);
	clsMem.OnNormalMessageWrite(strTemp);
}


void CServerInterface5::OnDataAnalysis(CString strMsg)
{
	CString strFunc;
	CString strTemp;
	CString strData;
	CString strRpy;
	CString strLotNo;
	CString strEqpID;
	CString strPartNo;
	CString strChamDoor;
	CString strModule;
	CString strBin;
	CString strBinMsg;
	CString strBarcode;
	CString strErrorMsg;
	CString strSocket;
	CString strLogKey[10];
	CString strLogData[10];

	int nSite = THD_TESTSITE_1 + 4;
	int i;
	int nPos[3];
	int nLength;
	// jtkim 20160930
	int nTemp[10];

	strFunc = OnDataItemAnalysis(0, 0, strMsg, _T("FUNCTION"));

	if (strFunc == _T("PGM_LOAD_RESULT"))
	{
		strData = strMsg;
		strData.Replace(_T("FUNCTION"), _T("FUNCTION_RPY"));
		strData += _T(" ");

		strLotNo = OnDataItemAnalysis(0, 0, strMsg, _T("LOT_ID"));
		strEqpID = OnDataItemAnalysis(0, 0, strMsg, _T("EQP_ID"));

		if (strLotNo == st_test_site_info[nSite].strLotNo && strEqpID == st_test_site_info[nSite].strEqpID)
		{
			strSocket = OnDataItemAnalysis(0, 0, strMsg, _T("SOCKET_INFO"));
			
			for (i=0; i<8; i++)
			{
				strTemp = strSocket.Mid((i * 2), 1);

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
					//kwlee 2017.0905
					if (st_basic_info.nModeXgem == YES)
					{
						clsXgem.OnMcSocket(st_test_site_info[nSite].st_pcb_info[i].nEnable,i+1);	
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
			// test site pgm download 완료......
			st_test_site_info[nSite].nPgmDown = YES;

			strData += _T("RESULT=PASS");
			strData += _T(" ");
			strData += _T("MSG=");
		}
		else
		{
			strData += _T("RESULT=FAIL");
			strData += _T(" ");
			strData += _T("MSG=LOT_NO/EQP_ID MissMatch");
		}

		strRpy.Format(_T("%016d"), strData.GetLength());
		st_server_info[INTERFACE_NETWORK_5].strSend = strRpy;
		::SendMessage(st_handler_info.hWnd, WM_SERVER_MSG + INTERFACE_NETWORK_5, SERVER_SEND, 0);

		st_server_info[INTERFACE_NETWORK_5].strSend = strData;
		::SendMessage(st_handler_info.hWnd, WM_SERVER_MSG + INTERFACE_NETWORK_5, SERVER_SEND, 0);
	}
	else if (strFunc == _T("TEST_ALARM"))
	{
		strLotNo = OnDataItemAnalysis(0, 0, strMsg, _T("LOT_ID"));
		strEqpID = OnDataItemAnalysis(0, 0, strMsg, _T("EQP_ID"));

		strData = _T("FUNCTION_RPY=TEST_ALARM");
		strData += _T(" ");
		
		strData += _T("EQP_ID=");
		strData += strEqpID;
		strData += _T(" ");
		strData += _T("RESULT=PASS");
		strData += _T(" ");
		strData += _T("MSG=");

		strRpy.Format(_T("%016d"), strData.GetLength());
		st_server_info[INTERFACE_NETWORK_5].strSend = strRpy;
		::SendMessage(st_handler_info.hWnd, WM_SERVER_MSG + INTERFACE_NETWORK_5, SERVER_SEND, 0);

		st_server_info[INTERFACE_NETWORK_5].strSend = strData;
		::SendMessage(st_handler_info.hWnd, WM_SERVER_MSG + INTERFACE_NETWORK_5, SERVER_SEND, 0);
	}
	else if (strFunc == _T("TEST_RESULT"))
	{
		strData = _T("FUNCTION_RPY=TEST_RESULT");
		strData += _T(" ");

		strLotNo	= OnDataItemAnalysis(0, 0, strMsg, _T("LOT_ID"));
		strPartNo	= OnDataItemAnalysis(0, 0, strMsg, _T("PART_NO"));
		strEqpID	= OnDataItemAnalysis(0, 0, strMsg, _T("EQP_ID"));
		strSocket	= OnDataItemAnalysis(0, 0, strMsg, _T("SOCKET_INFO"));

		if (strLotNo == st_test_site_info[nSite].strLotNo && strEqpID == st_test_site_info[nSite].strEqpID)
		{
			// test site lot정보와 eqp id정보가 일치하면 
			for (i=0; i<8; i++)
			{
				if (st_test_site_info[nSite].st_pcb_info[i].nYesNo	== YES && 
					st_test_site_info[nSite].st_pcb_info[i].nBin	!= BD_DATA_GOOD &&
					st_test_site_info[nSite].st_pcb_info[i].nBin	!= BD_DATA_REJECT &&
					st_test_site_info[nSite].st_pcb_info[i].nBin	!= BD_DATA_RETEST &&
					st_test_site_info[nSite].st_pcb_info[i].nBin	!= BD_DATA_ABORT)
				{
					// 해당 site에 pcb가 있으면 
					// pcb 정보를 가져옴
					strTemp.Format(_T("MDL%02d"), i + 1);
					strModule	= OnDataItemAnalysis(0, 0, strMsg, strTemp);
					nLength		= strModule.GetLength();

					nPos[0] = 0;
					if (strModule != _T(""))
					{
						nPos[1] = strModule.Find(_T(","), nPos[0]);
						if (nPos[1] > 0)
						{
							// pcb에 barcode정보를 가져옴
							strBarcode = strModule.Mid(0, (nPos[1] - nPos[0]));
							nPos[0]		= nPos[1] + 1;
							nPos[1]		= strModule.Find(_T(","), nPos[0]);
							nPos[0]		= nPos[1] + 1;
							nPos[1]		= strModule.Find(_T("_"), nPos[0]);
							nPos[0]		= nPos[1] + 1;
							nPos[1]		= strModule.Find(_T("_"), nPos[0]);
							strBin		= strModule.Mid(nPos[0], (nPos[1] - nPos[0]));

							nPos[0]		= nPos[1] + 1;
							nPos[1]		= strModule.Find(_T("_"), nPos[0]);
							if (nPos[1] > 0) strTemp = strModule.Mid(nPos[0] + 1, (nPos[1] - (nPos[0] + 1)));
							else strTemp = strModule.Mid(nPos[0] + 1, (nLength - (nPos[0] + 1)));
							nTemp[0]	= _wtoi(strTemp);

							nPos[0]		= nPos[1] + 1;
							nPos[1]		= strModule.Find(_T("_"), nPos[0]);
							if (nPos[1] > 0) strTemp = strModule.Mid(nPos[0] + 1, (nPos[1] - (nPos[0] + 1)));
							else strTemp = strModule.Mid(nPos[0] + 1, (nLength - (nPos[0] + 1)));
							nTemp[1]	= _wtoi(strTemp);

							nPos[0]		= nPos[1] + 1;
							nPos[1]		= strModule.Find(_T("_"), nPos[0]);
							if (nPos[1] > 0) strTemp = strModule.Mid(nPos[0] + 1, (nPos[1] - (nPos[0] + 1)));
							else strTemp = strModule.Mid(nPos[0] + 1, (nLength - (nPos[0] + 1)));
							nTemp[2]	= _wtoi(strTemp);

							nPos[0]		= nPos[1] + 1;
							nPos[1]		= strModule.Find(_T("_"), nPos[0]);
							if (nPos[1] > 0) strTemp = strModule.Mid(nPos[0] + 1, (nPos[1] - (nPos[0] + 1)));
							else strTemp = strModule.Mid(nPos[0] + 1, (nLength - (nPos[0] + 1)));
							nTemp[3]	= _wtoi(strTemp);

							// jtkim 20160930
							st_test_site_info[nSite].st_pcb_info[i].strBin		= strBin;
							st_test_site_info[nSite].st_pcb_info[i].nTemp[0]	= nTemp[0];
							st_test_site_info[nSite].st_pcb_info[i].nTemp[1]	= nTemp[1];
							st_test_site_info[nSite].st_pcb_info[i].nTemp[2]	= nTemp[2];
							st_test_site_info[nSite].st_pcb_info[i].nTemp[3]	= nTemp[3];

							if (strBarcode == st_test_site_info[nSite].st_pcb_info[i].strBarcode1D[0])
							{
								// barcode정보가 같으면 
								st_test_site_info[nSite].st_pcb_info[i].nRetestCnt++;
								if (strBin == _T("01") || strBin == _T("02") || strBin == _T("03") || strBin == _T("P"))
								{
									// 1/2/3 bin정보면 양품
									st_test_site_info[nSite].st_pcb_info[i].nBin = BD_DATA_GOOD;
									st_test_site_info[nSite].st_pcb_info[i].nFailCount = 0;//kwlee 2016.1124

									//kwlee 2016.1228
									//st_test_site_info[nSite].st_pcb_info[i].nBinGoodCnt++;
									//2017.0109
									strLogKey[0]	= _T("BIN");
									strLogData[0]	= _T("PASS");
	 								clsLog.LogProcess(_T("TESTER#5"), 
	 												  _T("CDIMM TEST"), 
	 												  1, 
	 												  st_test_site_info[nSite].st_pcb_info[i].strSerialNo, 
	 												  st_test_site_info[nSite].st_pcb_info[i].strLotNo, 
	 												  st_basic_info.strDeviceName, 
													  1, 
	 												  strLogKey, 
	 												  strLogData);
								}
								else
								{
									//kwlee 2016.1124
									//kwlee 2017.0126 테스트 해야 함
									st_test_site_info[nSite].st_pcb_info[i].nFailCount++;
									if(st_test_site_info[nSite].st_pcb_info[i].nFailCount > st_recipe_info.nFailCnt)
									{
										st_test_site_info[nSite].st_pcb_info[i].nEnable = NO;
										st_test_site_info[nSite].st_pcb_info[i].nBin = BD_DATA_CONTINUE_FAIL; //kwlee 2017.0125
										//kwlee 2017.0105
										strTemp.Format(_T("TSite_5,Socket Off Slot : %d BIN : [%s]"),i + 1,strBin);
										//kwlee 2017.0918
										st_handler_info.mstr_event_msg[0] = strTemp;
										::PostMessage(st_handler_info.hWnd, WM_MAIN_EVENT, YES, 0);	
										//
										clsMem.OnAbNormalMessagWrite(strTemp);//로그 저장
										//
									  //st_test_site_info[nSite].st_pcb_info[i].nBin = BD_DATA_CONTINUE_FAIL;
									}
									//

									//kwlee 2016.1228
									//st_test_site_info[nSite].st_pcb_info[i].nBinFailCnt[st_test_site_info[nSite].st_pcb_info[i].nScrCode]++;

									// 양품이 아니고 불량이면 scrap code 
									st_test_site_info[nSite].st_pcb_info[i].nScrCode = _wtoi(strBin);
									// jtkim 20161006
									if (st_test_site_info[nSite].st_pcb_info[i].nRetestCnt == 1)
									{
										if (st_test_site_info[nSite].st_pcb_info[i].nScrCode > 0)
										{//2016.1109
											if (st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)
											{
												if( st_test_site_info[nSite].st_pcb_info[i].strLotNo == st_lot_info[LOT_NEXT].strLotNo )
												{
													st_test_site_info[nSite].st_pcb_info[i].nRetest = st_code_info[LOT_NEXT].m_nScrapCode[st_test_site_info[nSite].st_pcb_info[i].nScrCode][1];
												}
												else
												{
													st_test_site_info[nSite].st_pcb_info[i].nRetest = st_code_info[LOT_CURR].m_nScrapCode[st_test_site_info[nSite].st_pcb_info[i].nScrCode][0];
												}
											}
											else
											{
												st_test_site_info[nSite].st_pcb_info[i].nRetest = st_code_info[LOT_CURR].m_nScrapCode[st_test_site_info[nSite].st_pcb_info[i].nScrCode][0];
											}
											//st_test_site_info[nSite].st_pcb_info[i].nRetest = st_work_info.nScrapRetest[st_test_site_info[nSite].st_pcb_info[i].nScrCode];
										}
										else
										{
											//kwlee 2016.1018
											//st_test_site_info[nSite].st_pcb_info[i].nRetest = st_basic_info.nRetestCount;
											st_test_site_info[nSite].st_pcb_info[i].nRetest = st_recipe_info.nTestRetest_Count;
										}
									}
									// jtkim 20160906
									//st_test_site_info[nSite].st_pcb_info[i].nRetestCnt++;
									//if (st_test_site_info[nSite].st_pcb_info[i].nRetestCnt > st_basic_info.nRetestCount)
									if (st_test_site_info[nSite].st_pcb_info[i].nRetestCnt > st_test_site_info[nSite].st_pcb_info[i].nRetest)
									{
										st_test_site_info[nSite].st_pcb_info[i].nBin = BD_DATA_REJECT;
									}
									else
									{
										st_test_site_info[nSite].st_pcb_info[i].nBin = BD_DATA_RETEST;
									}
									//2017.0109
									strLogKey[0]	= _T("BIN");
									strLogData[0]	= _T("FAIL");
									strLogKey[1]	= _T("SCRAP CODE");
									strLogData[1].Format(_T("%d"), st_test_site_info[nSite].st_pcb_info[i].nScrCode);
	 								clsLog.LogProcess(_T("TESTER#5"), 
	 												  _T("CDIMM TEST"), 
	 												  1, 
	 												  st_test_site_info[nSite].st_pcb_info[i].strSerialNo,
	 												  st_test_site_info[nSite].st_pcb_info[i].strLotNo, 
	 												  st_basic_info.strDeviceName, 
													  2, 
	 												  strLogKey, 
	 												  strLogData);
								}
								//kwlee 2017.0905
								if (st_basic_info.nModeXgem == YES)
								{
									if (st_test_site_info[nSite].st_pcb_info[i].nFailCount > 0 && st_test_site_info[nSite].st_pcb_info[i].nYesNo == YES)
									{
										if (st_test_site_info[nSite].st_pcb_info[i].nBin == BD_DATA_RETEST || st_test_site_info[nSite].st_pcb_info[i].nBin == BD_DATA_GOOD ||
											st_test_site_info[nSite].st_pcb_info[i].nBin == BD_DATA_REJECT)
										{
											clsXgem.OnMcRetestEnd(st_test_site_info[nSite].st_pcb_info[i].strSerialNo,i+1,st_test_site_info[nSite].st_pcb_info[i].nTrayCnt,nSite);
										}
									}
								}
								///
							}
							else
							{
								// barcode정보가 다르면 4598 scrap 처리
								st_test_site_info[nSite].st_pcb_info[i].nScrCode = 4598;
								// jtkim 20160906
								st_test_site_info[nSite].st_pcb_info[i].nRetestCnt++;
								//kwlee 2016.1018
								//if (st_test_site_info[nSite].st_pcb_info[i].nRetestCnt > st_basic_info.nRetestCount)
								if (st_test_site_info[nSite].st_pcb_info[i].nRetestCnt > st_recipe_info.nTestRetest_Count)
								{
									st_test_site_info[nSite].st_pcb_info[i].nBin = BD_DATA_REJECT;
								}
								else
								{
									st_test_site_info[nSite].st_pcb_info[i].nBin = BD_DATA_RETEST;
								}
								//2017.0109
								strLogKey[0]	= _T("BIN");
								strLogData[0]	= _T("FAIL");
								strLogKey[1]	= _T("SCRAP CODE");
								strLogData[1].Format(_T("%d"), st_test_site_info[nSite].st_pcb_info[i].nScrCode);
								clsLog.LogProcess(_T("TESTER#5"), 
	 												_T("CDIMM TEST"), 
	 												1, 
	 												_T("CDIMM"), 
	 												st_test_site_info[nSite].st_pcb_info[i].strLotNo, 
	 												st_basic_info.strDeviceName, 
													2, 
	 												strLogKey, 
	 												strLogData);
							}
							strBinMsg	= strModule.Mid(nPos[1] + 1, (nLength - (nPos[1] + 1)));
							// jtkim 20160930
							st_test_site_info[nSite].st_pcb_info[i].strMsg = strBinMsg;
						}
					}
				}
				//2016.1031 자재는 있는데 테스트를 진행하지 않음
				else if (st_test_site_info[nSite].st_pcb_info[i].nYesNo	== YES )
				{
					//st_test_site_info[nSite].st_pcb_info[i].nRetestCnt++;//리테스트를 추가하지 않는다
					strBin = _T("0");
					st_test_site_info[nSite].st_pcb_info[i].strBin = strBin;
					st_test_site_info[nSite].st_pcb_info[i].nScrCode = _wtoi(strBin);
					if (st_test_site_info[nSite].st_pcb_info[i].nRetestCnt == 1)
					{
						if (st_test_site_info[nSite].st_pcb_info[i].nScrCode > 0)
						{//2016.1109
							if (st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)
							{
								if( st_test_site_info[nSite].st_pcb_info[i].strLotNo == st_lot_info[LOT_NEXT].strLotNo )
								{
									st_test_site_info[nSite].st_pcb_info[i].nRetest = st_code_info[LOT_NEXT].m_nScrapCode[st_test_site_info[nSite].st_pcb_info[i].nScrCode][1];
								}
								else
								{
									st_test_site_info[nSite].st_pcb_info[i].nRetest = st_code_info[LOT_CURR].m_nScrapCode[st_test_site_info[nSite].st_pcb_info[i].nScrCode][0];
								}
							}
							else
							{
								st_test_site_info[nSite].st_pcb_info[i].nRetest = st_code_info[LOT_CURR].m_nScrapCode[st_test_site_info[nSite].st_pcb_info[i].nScrCode][0];
							}
							//st_test_site_info[nSite].st_pcb_info[i].nRetest = st_work_info.nScrapRetest[st_test_site_info[nSite].st_pcb_info[i].nScrCode];
						}
						else
						{
							st_test_site_info[nSite].st_pcb_info[i].nRetest = st_recipe_info.nTestRetest_Count;
						}
					}
					if (st_test_site_info[nSite].st_pcb_info[i].nRetestCnt > st_test_site_info[nSite].st_pcb_info[i].nRetest)
					{
						st_test_site_info[nSite].st_pcb_info[i].nBin = BD_DATA_REJECT;
					}
					else
					{
						st_test_site_info[nSite].st_pcb_info[i].nBin = BD_DATA_RETEST;
					}
					strBinMsg	= _T("");
					st_test_site_info[nSite].st_pcb_info[i].strMsg = strBinMsg;
					//SOCKET OFF 한다
					st_test_site_info[nSite].st_pcb_info[i].nEnable = NO;
				}
				strTemp = strSocket.Mid((i * 2), 1);

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
					//kwlee 2017.0905
					if (st_basic_info.nModeXgem == YES)
					{
						clsXgem.OnMcSocket(st_test_site_info[nSite].st_pcb_info[i].nEnable,i+1);	
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
		}
		else
		{
			// test site정보와 받은 lot no / eqp id 정보가 다르면 불량처리
			for (i=0; i<8; i++)
			{
				if (st_test_site_info[nSite].st_pcb_info[i].nYesNo == YES)
				{
					// barcode정보가 다르면 4598 scrap 처리
					st_test_site_info[nSite].st_pcb_info[i].nScrCode = 4598;
					st_test_site_info[nSite].st_pcb_info[i].nBin = BD_DATA_REJECT;
				}
			}
		}

		strData += _T("LOT_ID=");
		strData += strLotNo;
		strData += _T(" ");

		strData += _T("PART_NO=");
		strData += strPartNo;
		strData += _T(" ");

		strData += _T("EQP_ID=");
		strData += strEqpID;
		strData += _T(" ");

		strData += _T("RESULT=PASS");
		strData += _T(" ");
		strData += _T("MSG=");

		// JTKIM CHAMBER DOOR 미리 오픈하기
		clsTestSite5.m_nOpenDoor = YES;

		st_test_site_info[nSite].nTestBdStart = BD_BIN_OK;

		strRpy.Format(_T("%016d"), strData.GetLength());
		st_server_info[INTERFACE_NETWORK_5].strSend = strRpy;
		::SendMessage(st_handler_info.hWnd, WM_SERVER_MSG + INTERFACE_NETWORK_5, SERVER_SEND, 0);

		st_server_info[INTERFACE_NETWORK_5].strSend = strData;
		::SendMessage(st_handler_info.hWnd, WM_SERVER_MSG + INTERFACE_NETWORK_5, SERVER_SEND, 0);
	}
	//kwlee 2016.1031
	else if (strFunc == _T("TESTER_BEGIN"))
	{
		strEqpID	= OnDataItemAnalysis(0, 0, strMsg, _T("EQP_ID"));
		//strChamDoor = OnDataItemAnalysis(0, 0, strMsg, _T("MODE"));

		strData = _T("FUNCTION_RPY=TESTER_BEGIN");
		strData += _T(" ");
		strData += _T("EQP_ID=");
		strData += strEqpID;
		strData += _T(" ");

		//		st_test_site_info[nSite].strEqpID 
		if (strEqpID == st_test_site_info[nSite].strEqpID)
		{
			strData += _T("RESULT=PASS");
			strData += _T(" ");
			strData += _T("MSG=");
		}

		//kwlee 2016.1031

		strRpy.Format(_T("%016d"), strData.GetLength());
		st_server_info[INTERFACE_NETWORK_5].strSend = strRpy;
		::SendMessage(st_handler_info.hWnd, WM_SERVER_MSG + INTERFACE_NETWORK_5, SERVER_SEND, 0);

		st_server_info[INTERFACE_NETWORK_5].strSend = strData;
		::SendMessage(st_handler_info.hWnd, WM_SERVER_MSG + INTERFACE_NETWORK_5, SERVER_SEND, 0);
	}
	/////
	else if (strFunc == _T("CHAMBER_DOOR"))
	{
		strEqpID	= OnDataItemAnalysis(0, 0, strMsg, _T("EQP_ID"));
		strChamDoor = OnDataItemAnalysis(0, 0, strMsg, _T("MODE"));

		strData = _T("FUNCTION_RPY=CHAMBER_DOOR");
		strData += _T(" ");
		strData += _T("EQP_ID=");
		strData += strEqpID;
		strData += _T(" ");

//		st_test_site_info[nSite].strEqpID 
		if (strEqpID == st_test_site_info[nSite].strEqpID)
		{
			// jtkim 20160926
			if (st_handler_info.nAutoMode == YES)
			{
				if (strChamDoor == _T("OPEN"))
				{
					//kwlee 2016.1031
					clsTestSite5.m_nOpenDoor = YES;
				}
				else 
				{
					clsTestSite5.m_nCloseDoor = YES;
					//kwlee 2016.1021 tester에서 프로그램 적용 되지 않아 원복함..
					st_handler_info.m_tlDooropen[4][0] = GetCurrentTime();
				}
				strData += _T("RESULT=PASS");
				strData += _T(" ");
				strData += _T("MSG=");
			}
			else
			{
				//kwlee 2016.1111
				//if (st_handler_info.nRunStatus == dSTOP)
				if (st_handler_info.nRunStatus != dRUN)
				{
					if (FAS_IO.get_in_bit(st_io_info.i_TsiteDoorChk[4], IO_OFF))
					{
						if (strChamDoor == _T("OPEN"))
						{
							//kwlee 2016.1031
							clsTestSite5.m_nOpenDoor = YES;
						}
						else 
						{
							//kwlee 2016.1031
							clsTestSite5.m_nCloseDoor = YES;
							st_handler_info.m_tlDooropen[4][0] = GetCurrentTime();
						}
						strData += _T("RESULT=PASS");
						strData += _T(" ");
						strData += _T("MSG=");
					}
					else
					{
						strData += _T("RESULT=FAIL");
						strData += _T(" ");
						strData += _T("MSG=Door Open");
					}
				}
				else
				{
					strData += _T("RESULT=FAIL");
					strData += _T(" ");
					strData += _T("MSG=Handler Run Status");
				}
			}
		}
		else
		{
			strData += _T("RESULT=FAIL");
			strData += _T(" ");
			strData += _T("MSG=EQP ID MissMatch");
		}

		strRpy.Format(_T("%016d"), strData.GetLength());
		st_server_info[INTERFACE_NETWORK_5].strSend = strRpy;
		::SendMessage(st_handler_info.hWnd, WM_SERVER_MSG + INTERFACE_NETWORK_5, SERVER_SEND, 0);

		st_server_info[INTERFACE_NETWORK_5].strSend = strData;
		::SendMessage(st_handler_info.hWnd, WM_SERVER_MSG + INTERFACE_NETWORK_5, SERVER_SEND, 0);
	}
}



CString CServerInterface5::OnDataItemAnalysis(int nOpt, int nPos, CString strMsg, CString strFind)
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

	return strData;
}

void CServerInterface5::OnCommunication()
{
	CString strMsg;

	if (m_pQueueInter5 != NULL)
	{
		if (m_pQueueInter5->m_nCount > 0)
		{
			strMsg	= OnMessageRead();
			OnDataAnalysis(strMsg);
		}
	}
}
