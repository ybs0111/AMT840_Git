#include "StdAfx.h"
//#include "LogFromat.h"
#include "CtlBd_Library.h"
#include "ComizoaPublic.h"
//#include "Variable.h"
//#include "io.h"			// 파일 존재 유무 검사 함수 호출하기 위해서는 반드시 필요
#include "AlgMemory.h"
#include "PublicFunction.h"
#include "LogFromat.h"


#define LOG_NAME_W			"LOG_SHAREMEM_W"
// #define LOG_NAME_R			"LOG_SHAREMEM_R"

CLogFromat clsLog;

UINT OnMessageWrite(LPVOID lParam);

CLogFromat::CLogFromat(void)
{
	m_hMemMapLogW	= NULL;
	m_pQueueLogW	= NULL;

//	m_hMemMapLogR	= NULL;
//	m_pQueueLogR	= NULL;

	m_hMemMapLogW = CreateFileMapping(INVALID_HANDLE_VALUE,
									 NULL,
									 PAGE_READWRITE,
									 0,
									 sizeof(LOG_QUEUE_W),
									 (LPCWSTR)LOG_NAME_W);

	if (m_hMemMapLogW != NULL)
	{
		m_pQueueLogW = (P_LOG_QUEUE_W)MapViewOfFile(m_hMemMapLogW, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (m_pQueueLogW != NULL)
		{
			m_pQueueLogW->m_nFront	= 0;
			m_pQueueLogW->m_nRear		= 0;
			m_pQueueLogW->m_nCount	= 0;
		}
	}
	else
	{
		if (m_hMemMapLogW != NULL)
		{
			m_pQueueLogW = (P_LOG_QUEUE_W)MapViewOfFile(m_hMemMapLogW, FILE_MAP_READ, 0, 0, 0);
		}
	}
	int i;
	for (i=0; i<100; i++)
	{
		m_nMoveFlag[i] = NO;
	}	
}


CLogFromat::~CLogFromat(void)
{
	if (m_pQueueLogW != NULL)	UnmapViewOfFile(m_pQueueLogW);	
	if (m_hMemMapLogW != NULL)	CloseHandle(m_hMemMapLogW);
}

/*
UINT OnMessageWrite(LPVOID lParam)
{
	CLogFormat *pMsg = (CLogFormat *)lParam;

	while(pMsg->m_bThread)
	{
		if (pMsg->m_hMemMapLogR != NULL && pMsg->m_pQueueLogR != NULL)
		{
			if (pMsg->m_pQueueLogR->m_nCount != pMsg->m_nOldRearCount)
			{
				pMsg->m_nOldRearCount			= pMsg->m_pQueueLogR->m_nCount;
				pMsg->m_pQueueLogW->m_nCount	= pMsg->m_pQueueLogR->m_nCount;
			}
		}
	}

	return 0;
}
*/

CString	CLogFromat::OnCharToString(char chData[], int nLength)
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


void CLogFromat::OnStringToChar(CString strData, char chData[])
{
	int nCnt;
	wchar_t *chBuf;

	chBuf = strData.GetBuffer(strData.GetLength());
	nCnt  = WideCharToMultiByte(CP_ACP, 0, chBuf, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, chBuf, -1, chData, nCnt, 0, 0);
}


int CLogFromat::OnFileSizeCheck(CString strFileName, long lSize, int nCheck)
{
	CFileFind finder;
	long Len=0, flag=0;
	BOOL bContinue;

	if(bContinue = finder.FindFile(strFileName))
	{	
		if (nCheck == YES)			// Size를 체크하는것이면...
		{
			finder.FindFile(strFileName);
			finder.FindNextFile();
			finder.GetFileName();
			Len = (long)finder.GetLength();
			if(Len > lSize)
			{
				flag = 1;
			}
			finder.Close();	
		}
		else						// Size를 확인하는것이면...
		{
			finder.FindFile(strFileName);
			finder.FindNextFile();
			finder.GetFileName();
			Len = (long)finder.GetLength();
			
			flag = Len;
			
			finder.Close();	
		}
	}
	
	return flag;
}

void CLogFromat::OnLogWrite(CString strMsg)
{
	CString strTemp;

	if (m_hMemMapLogW != NULL && m_pQueueLogW != NULL)
	{
		if (strMsg == _T(""))
		{
			return;
		}

		OnStringToChar(strMsg, m_pQueueLogW->m_chMsg[m_pQueueLogW->m_nRear]);

		m_pQueueLogW->m_nRear++;

		if (m_pQueueLogW->m_nRear >= 100) m_pQueueLogW->m_nRear = 0;
	
		m_pQueueLogW->m_nCount++;
	}
}

void CLogFromat::OnLogTestSite(int nMode, int nSite, int nLfToRi, tagTEST_SITE_INFO &test_site)
{
	CFileFind filefind;
	CString mstr_cur_year, mstr_cur_month, mstr_cur_day, str_display_time; // 현재 년, 월, 일 정보 문자형으로 변환하여 저장할 변수 
	int mn_cur_year, mn_cur_month, mn_cur_day, mn_cur_hour; // 현재 년, 월, 일 정보 저장 변수 
	CString mstr_file_name;		// 마지막으로 생성된 파일 이름 저장 변수 
	CString mstr_create_file;	// 알람 정보 저장할 파일에 대한 [폴더]+[파일명]+[확장자] 설정 변수 
	CString mstr_list_name, mstr_temp_data;  // 각 부분별 알람 발생 횟수 정보 저장할 로그 파일 정보 저장 변수 
	CString mstr_content;		// 한 라인에 출력할 문자열 정보 저장 변수 
	COleDateTime time_cur;		// 검사할 시간 정보 저장 변수 
	CTime m_time_current;		// 간략한 헝식의 현재 시간 정보 저장 변수
	int mn_existence;			// 파일 존재 유무 설정 플래그 
	char fileName[256];			// 검색할 파일 정보 설정 함수 
	char chFileName[256];
	char chBackName[256];
	FILE  *fp ;					// 파일에 대한 포인터 설정 변수

	int nRet;
	int i;
	//kwlee 2016.1228
	int nTotal; 
	double dYield;
	//

	CString strTemp;
	CString strBd;
	CString strMsg;
	CString BackupName;
	CString mstr_cur_hour, mstr_cur_min, mstr_cur_sec;

	// **************************************************************************
	// 파일 이름으로 사용할 날짜 정보를 얻는다                                   
	// **************************************************************************
	time_cur = COleDateTime::GetCurrentTime();  // 현재 시간 정보를 얻는다. 

	m_time_current = CTime::GetCurrentTime() ;  // 간략한 형식의 현재 시간 정보 얻는다. 

	mn_cur_year = time_cur.GetYear();  

	mn_cur_month = time_cur.GetMonth();  

    mn_cur_day = time_cur.GetDay();  

	mn_cur_hour = time_cur.GetHour();
	// **************************************************************************

	// **************************************************************************
	// 날짜 정보를 문자형으로 변환하여 변수에 설정한다                           
	// **************************************************************************
	mstr_cur_year.Format(_T("%04d"), mn_cur_year);  
	mstr_cur_month.Format(_T("%02d"), mn_cur_month);
	mstr_cur_day.Format(_T("%02d"), mn_cur_day);

	// **************************************************************************
	// 현재 시간 정보 얻는다                                                     
	// **************************************************************************
	mn_cur_hour = time_cur.GetHour();				// 현재 시간 정보를 설정한다. 
	mstr_cur_hour.Format(_T("%d"),time_cur.GetHour());	// 현재 시간 정보를 설정한다.
	mstr_cur_min.Format(_T("%d"),time_cur.GetMinute());	// 현재 분 정보를 설정한다. 
	mstr_cur_sec.Format(_T("%d"),time_cur.GetSecond());	// 현재 초 정보를 설정한다. 
	str_display_time = m_time_current.Format(_T("%c"));	// 리스트 파일에 출력할 타이틀 시간 정보 설정 
	// **************************************************************************

	mstr_file_name = mstr_cur_year;
	mstr_file_name += mstr_cur_month; 
	mstr_file_name += mstr_cur_day; 
	mstr_file_name += _T("_"); 
	strTemp.Format(_T("%s_%s"), st_basic_info.strEqp, test_site.strEqpID);
	mstr_file_name += strTemp;
	mstr_create_file = st_path_info.strInterface + mstr_file_name;

	mstr_create_file += _T(".TXT");

	OnStringToChar(mstr_create_file, fileName);

	mn_existence = filefind.FindFile(mstr_create_file,0);
	
	if (mn_existence == -1)
	{
		mstr_file_name = mstr_cur_year;
		mstr_file_name += mstr_cur_month; 
		mstr_file_name += mstr_cur_day; 
		mstr_file_name += _T("_"); 
		strTemp.Format(_T("%s_%s"), st_basic_info.strEqp, test_site.strEqpID);
		mstr_file_name += strTemp;
		mstr_create_file = st_path_info.strInterface + mstr_file_name;

		mstr_create_file += _T(".TXT");
		// **********************************************************************
	}

	nRet = OnFileSizeCheck(mstr_create_file, 1048576, YES); //size and rename
	
	if(nRet == 1) //file size over
	{
		BackupName = mstr_create_file + mstr_cur_hour + mstr_cur_min +  mstr_cur_sec + _T(".bak") ;
		
		OnStringToChar(mstr_create_file, chFileName);
		OnStringToChar(BackupName, chBackName);

		rename(chFileName, chBackName);
	}

	// **************************************************************************
	// 알람 발생 횟수 정보 저장 파일에 추가 가능한 형태 파일로 생성              
	// **************************************************************************
	char chMode[10];

	OnStringToChar(_T("a+"), chMode);

	fopen_s(&fp, fileName, "a+");
	if(!fp)
	{
		return;
	}
	// **************************************************************************

	// **************************************************************************
	// 로그 파일에 현재 발생한 알람 정보 저장한다                                
	// **************************************************************************
	if (st_handler_info.strUserId != _T(""))
	{
		mstr_content += "[";
		mstr_content += st_handler_info.strUserId;
		mstr_content += "] ";
	}
	
	if (nMode == 0)
	{
		// insert
		if (nLfToRi == 0) strMsg = _T("INSERT 1");
		else strMsg = _T("INSERT 2");
		strMsg += _T("\n");

		for (i=0; i<8; i++)
		{
			strTemp.Format(_T("DUT[%02d]"), i + 1);
			strMsg += strTemp;
			strMsg += _T("\t");
			if (test_site.st_pcb_info[i].nBin != BD_DATA_GOOD &&
				test_site.st_pcb_info[i].nBin != BD_DATA_REJECT &&
				test_site.st_pcb_info[i].nBin != BD_DATA_RETEST)
			{
				strTemp.Format(_T("%s %s"), test_site.st_pcb_info[i].strBarcode1D[0],
					test_site.st_pcb_info[i].strBarcode1D[1]);
				strMsg += strTemp;
				strMsg += _T("\t");

				strTemp.Format(_T("%d"), test_site.st_pcb_info[i].nYesNo);
				strMsg += strTemp;
				strMsg += _T("\t");
			}
			else
			{
				strMsg += _T("  ");
				strMsg += _T("\t");

				strMsg += _T("0");
				strMsg += _T("\t");
			}

			if (test_site.st_pcb_info[i].nEnable == YES)
			{
				strMsg += _T("USED");
				strMsg += _T("\t");

				strMsg += _T("USED");
				strMsg += _T("\t");
			}
			else
			{
				strMsg += _T("NO");
				strMsg += _T("\t");

				strMsg += _T("NO");
				strMsg += _T("\t");
			}

			strMsg += test_site.strLotNo;
			strMsg += _T("\n");
		}
	}
	else if (nMode == 1)
	{
		// insert
		strMsg = _T("READY");
		strMsg += _T("\n");

		for (i=0; i<8; i++)
		{
			strTemp.Format(_T("DUT[%02d]"), i + 1);
			strMsg += strTemp;
			strMsg += _T("\t");
			strTemp.Format(_T("%s %s"), test_site.st_pcb_info[i].strBarcode1D[0],
										test_site.st_pcb_info[i].strBarcode1D[1]);
			strMsg += strTemp;
			strMsg += _T("\t");

			strTemp.Format(_T("%d"), test_site.st_pcb_info[i].nYesNo);
			strMsg += strTemp;
			strMsg += _T("\t");

			if (test_site.st_pcb_info[i].nEnable == YES)
			{
				strMsg += _T("USED");
				strMsg += _T("\t");
			}
			else
			{
				strMsg += _T("NO");
				strMsg += _T("\t");
			}

			strMsg += test_site.strLotNo;
			strMsg += _T("\t");

			if (test_site.st_pcb_info[i].nRetestCnt == 0)
			{
				strMsg += _T("P");
				strMsg += _T("\t");
			}
			else
			{
				strTemp.Format(_T("%dR"), test_site.st_pcb_info[i].nRetestCnt);
				strMsg += strTemp;
				strMsg += _T("\t");
			}
			strMsg += _T("\n");
		}
	}
	else if (nMode == 2)
	{
		// insert
		strMsg = _T("MDL INFO");
		strMsg += _T("\n");

		for (i=0; i<8; i++)
		{
			strTemp.Format(_T("DUT[%02d]"), i + 1);
			strMsg += strTemp;
			strMsg += _T("\t");
			strTemp.Format(_T("%s %s"), test_site.st_pcb_info[i].strBarcode1D[0],
										test_site.st_pcb_info[i].strBarcode1D[1]);
			strMsg += strTemp;
			strMsg += _T("\t");

			strTemp.Format(_T("%d"), test_site.st_pcb_info[i].nYesNo);
			strMsg += strTemp;
			strMsg += _T("\t");

			if (test_site.st_pcb_info[i].nEnable == YES)
			{
				strMsg += _T("USED");
				strMsg += _T("\t");
			}
			else
			{
				strMsg += _T("NO");
				strMsg += _T("\t");
			}

			strMsg += test_site.strLotNo;
			strMsg += _T("\t");

			if (test_site.st_pcb_info[i].nRetestCnt == 0)
			{
				strMsg += _T("P");
				strMsg += _T("\t");
			}
			else
			{
				strTemp.Format(_T("%dR"), test_site.st_pcb_info[i].nRetestCnt);
				strMsg += strTemp;
				strMsg += _T("\t");
				//kwlee 2016.1018
				//if (test_site.st_pcb_info[i].nRetestCnt > st_basic_info.nRetestCount)
				if (test_site.st_pcb_info[i].nRetestCnt > st_recipe_info.nTestRetest_Count)
				{
					strMsg += _T("F");
					strMsg += _T("\t");
				}
				else
				{
					strMsg += _T("I");
					strMsg += _T("\t");
				}
			}
			strMsg += _T("\n");
		}
	}
	else if (nMode == 3)
	{
		// insert
		strMsg = _T("START");
		strMsg += _T("\n");

		for (i=0; i<8; i++)
		{
			strTemp.Format(_T("DUT[%02d]"), i + 1);
			strMsg += strTemp;
			strMsg += _T("\t");
			strTemp.Format(_T("%s %s"), test_site.st_pcb_info[i].strBarcode1D[0],
										test_site.st_pcb_info[i].strBarcode1D[1]);
			strMsg += strTemp;
			strMsg += _T("\t");

			strTemp.Format(_T("%d"), test_site.st_pcb_info[i].nYesNo);
			strMsg += strTemp;
			strMsg += _T("\t");

			if (test_site.st_pcb_info[i].nEnable == YES)
			{
				strMsg += _T("USED");
				strMsg += _T("\t");
			}
			else
			{
				strMsg += _T("NO");
				strMsg += _T("\t");
			}

			strMsg += test_site.strLotNo;
			strMsg += _T("\t");

			if (test_site.st_pcb_info[i].nRetestCnt == 0)
			{
				strMsg += _T("P");
				strMsg += _T("\t");
			}
			else
			{
				strTemp.Format(_T("%dR"), test_site.st_pcb_info[i].nRetestCnt);
				strMsg += strTemp;
				strMsg += _T("\t");
			}
			strMsg += _T("\n");
		}
	}
	else if (nMode == 4)
	{
		// insert
		strMsg = _T("CHAMBER CLOSE");
		strMsg += _T("\n");

		for (i=0; i<8; i++)
		{
			strTemp.Format(_T("DUT[%02d]"), i + 1);
			strMsg += strTemp;
			strMsg += _T("\t");
			strTemp.Format(_T("%s %s"), test_site.st_pcb_info[i].strBarcode1D[0],
										test_site.st_pcb_info[i].strBarcode1D[1]);
			strMsg += strTemp;
			strMsg += _T("\t");

			strTemp.Format(_T("%d"), test_site.st_pcb_info[i].nYesNo);
			strMsg += strTemp;
			strMsg += _T("\t");

			if (test_site.st_pcb_info[i].nEnable == YES)
			{
				strMsg += _T("USED");
				strMsg += _T("\t");
			}
			else
			{
				strMsg += _T("NO");
				strMsg += _T("\t");
			}

			strMsg += test_site.strLotNo;
			strMsg += _T("\t");

			if (test_site.st_pcb_info[i].nRetestCnt == 0)
			{
				strMsg += _T("P");
				strMsg += _T("\t");
			}
			else
			{
				strTemp.Format(_T("%dR"), test_site.st_pcb_info[i].nRetestCnt);
				strMsg += strTemp;
				strMsg += _T("\t");
			}
			strMsg += _T("\n");
		}
	}
	else if (nMode == 5)
	{
		// insert
		strMsg = _T("RESULT");
		strMsg += _T("\n");

		for (i=0; i<8; i++)
		{
			strTemp.Format(_T("DUT[%02d]"), i + 1);
			strMsg += strTemp;
			strMsg += _T("\t");
			strTemp.Format(_T("%s %s"), test_site.st_pcb_info[i].strBarcode1D[0],
										test_site.st_pcb_info[i].strBarcode1D[1]);
			strMsg += strTemp;
			strMsg += _T("\t");

			strTemp.Format(_T("%d"), test_site.st_pcb_info[i].nYesNo);
			strMsg += strTemp;
			strMsg += _T("\t");

			if (test_site.st_pcb_info[i].nEnable == YES)
			{
				strMsg += _T("USED");
				strMsg += _T("\t");
			}
			else
			{
				strMsg += _T("NO");
				strMsg += _T("\t");
			}

			strMsg += test_site.strLotNo;
			strMsg += _T("\t");

			if (test_site.st_pcb_info[i].nRetestCnt == 1)
			{
				strMsg += _T("P");
				strMsg += _T("\t");
			}
			else
			{
				strTemp.Format(_T("%dR"), test_site.st_pcb_info[i].nRetestCnt - 1);
				strMsg += strTemp;
				strMsg += _T("\t");
			}

			strMsg += test_site.st_pcb_info[i].strBin;
			strMsg += _T("\t");

			//kwlee 2016.1228
// 			if (test_site.st_pcb_info[i].nBin != BD_DATA_GOOD)
// 			{	
// 				strTemp.Format(_T("%s_%d"),test_site.st_pcb_info[i].strBin,test_site.st_pcb_info[i].nBinFailCnt[test_site.st_pcb_info[i].nScrCode]);
// 				strMsg += strTemp;
// 				strMsg += _T("\t");
// 
// 				nTotal = test_site.st_pcb_info[i].nFailCount + test_site.st_pcb_info[i].nBinGoodCnt;
// 				dYield = (double)test_site.st_pcb_info[i].nBinGoodCnt / (double)nTotal * (double)100;
// 
// 				strTemp.Format(_T("%0.2f"),dYield);
// 				strMsg += strTemp;
// 				strMsg += _T("\t");
// 				///////////
// 			}
			strTemp.Format(_T("%d"), test_site.st_pcb_info[i].nTemp[0]);
			strMsg += strTemp;
			strMsg += _T("\t");

			strTemp.Format(_T("%d"), test_site.st_pcb_info[i].nTemp[1]);
			strMsg += strTemp;
			strMsg += _T("\t");

			strTemp.Format(_T("%d"), test_site.st_pcb_info[i].nTemp[2]);
			strMsg += strTemp;
			strMsg += _T("\t");

			strTemp.Format(_T("%d"), test_site.st_pcb_info[i].nTemp[3]);
			strMsg += strTemp;
			strMsg += _T("\t");

			strMsg += test_site.st_pcb_info[i].strBdTime;
			strMsg += _T("\t");

			
			strMsg += _T("\n");
		}
		//2016.1018
// 		if(test_site.strLotNo == st_lot_display_info[0].strLotNo)
// 		{
// 			strTemp.Format(_T("PART_NO=%s"), st_lot_display_info[0].strPartNo);
// 			strMsg += strTemp;
// 			strMsg += _T("\n");
// 			strTemp.Format(_T("OPT=%s"), st_lot_display_info[0].strOptcode);
// 			strMsg += strTemp;
// 			strMsg += _T("\n");
// 			strTemp.Format(_T("LOT=%s"), st_lot_display_info[0].strLotNo);
// 			strMsg += strTemp;
// 			strMsg += _T("\n");	
// 			strTemp.Format(_T("FAB=%s"), st_lot_display_info[0].strFabSite);
// 			strMsg += strTemp;
// 			strMsg += _T("\n");	
// 		}
// 		else if( test_site.strLotNo == st_lot_display_info[1].strLotNo)
// 		{
// 			strTemp.Format(_T("PART_NO=%s"), st_lot_display_info[1].strPartNo);
// 			strMsg += strTemp;
// 			strMsg += _T("\n");
// 			strTemp.Format(_T("OPT=%s"), st_lot_display_info[1].strOptcode);
// 			strMsg += strTemp;
// 			strMsg += _T("\n");
// 			strTemp.Format(_T("LOT=%s"), st_lot_display_info[1].strLotNo);
// 			strMsg += strTemp;
// 			strMsg += _T("\n");	
// 			strTemp.Format(_T("FAB=%s"), st_lot_display_info[1].strFabSite);
// 			strMsg += strTemp;
// 			strMsg += _T("\n");	
// 		}

		//kwlee 2017.0105
		strTemp.Format(_T("PART_NO=%s"), test_site.strPartNo);
		strMsg += strTemp;
		strMsg += _T("\n");
		strTemp.Format(_T("OPT=%s"), test_site.strOptCode);
		strMsg += strTemp;
		strMsg += _T("\n");
		strTemp.Format(_T("LOT=%s"), test_site.strLotNo);
		strMsg += strTemp;
		strMsg += _T("\n");	
		strTemp.Format(_T("FAB=%s"), test_site.strFabSite);
		strMsg += strTemp;
		strMsg += _T("\n");	
	}
	else if (nMode == 6)
	{
		// PCB OUT
		//2016.1018
		if (nLfToRi == 0) strMsg = _T("PICK START 1");
		else strMsg = _T("PICK START 2");
//		else strMsg = _T("PICK END");

		strMsg += _T("\n");

		for (i=0; i<8; i++)
		{
			strTemp.Format(_T("DUT[%02d]"), i + 1);
			strMsg += strTemp;
			strMsg += _T("\t");

			if (test_site.st_pcb_info[i].nBin == BD_DATA_GOOD ||
				test_site.st_pcb_info[i].nBin == BD_DATA_GOOD ||
				test_site.st_pcb_info[i].nBin == BD_DATA_GOOD )
			{
				strTemp.Format(_T("%s %s"), test_site.st_pcb_info[i].strBarcode1D[0],
					test_site.st_pcb_info[i].strBarcode1D[1]);
				strMsg += strTemp;
				strMsg += _T("\t");

				strTemp.Format(_T("%d"), test_site.st_pcb_info[i].nYesNo);
				strMsg += strTemp;
				strMsg += _T("\t");

			}

			strMsg += _T("\n");
		}
	}

	mstr_content += str_display_time;
	mstr_content += _T("\r\n");
	mstr_content += strMsg;
	mstr_content += _T("\n");

	char chData[20000];

	if (mstr_content.GetLength() > 20000) 
	{
		fclose(fp);

		return ;
	}

	OnStringToChar(mstr_content, chData);

	fprintf(fp, chData) ;

	if (ferror(fp))  
	{
		if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
		{
			clsMem.OnNormalMessageWrite(_T("File save failure."));
		}

		fclose(fp);

		return ;
	}
	// **************************************************************************
	fclose(fp);  // 파일을 종료한다.
}




void CLogFromat::OnAddKeyAlarm(CString strDevice, CString strEvent, int nKey, CString *strKey, CString *strValue)
{
	int i;

	m_strDevice[ALARM_KEY]	= strDevice;
	m_strEvent[ALARM_KEY]	= strEvent;

	if (m_nKeyCount[ALARM_KEY] > 10) return;

	m_nKeyCount[ALARM_KEY] = nKey;
	for (i=0; i<nKey; i++)
	{
		m_strKey[ALARM_KEY][i]		= strKey[i];
		m_strValue[ALARM_KEY][i]	= strValue[i];
	}
}

void CLogFromat::OnLogMotorAlarm(CString strDevice, CString strEvent, int nMotor, int *nAxis, double *dPos)
{
	CString strKey[10];
	CString strValue[10];

	if (nMotor == 1)
	{
		strKey[0]	= _T("AXIS");
		strValue[0].Format(_T("%s"), GetDeviceID(nAxis[0]));
		strKey[1]	= _T("TARGET");
		strValue[1].Format(_T("%.3f"), dPos[0]);
		strKey[2]	= _T("CURRENT");
		strValue[2].Format(_T("%.3f"), COMI.Get_MotCurrentPos(nAxis[0]));

		OnAddKeyAlarm(strDevice, strEvent, 3, strKey, strValue);
	}
	else if (nMotor == 2)
	{
		strKey[0]	= _T("AXIS");
		strValue[0].Format(_T("%s"), GetDeviceID(nAxis[0]));
		strKey[1]	= _T("TARGET");
		strValue[1].Format(_T("%.3f"), dPos[0]);
		strKey[2]	= _T("CURRENT");
		strValue[2].Format(_T("%.3f"), COMI.Get_MotCurrentPos(nAxis[0]));

		strKey[3]	= _T("AXIS");
		strValue[3].Format(_T("%s"), GetDeviceID(nAxis[1]));
		strKey[4]	= _T("TARGET");
		strValue[4].Format(_T("%.3f"), dPos[1]);
		strKey[5]	= _T("CURRENT");
		strValue[5].Format(_T("%.3f"), COMI.Get_MotCurrentPos(nAxis[1]));

		OnAddKeyAlarm(strDevice, strEvent, 6, strKey, strValue);
	}
}
void CLogFromat::OnLogAlarm(CString strAlarmCode, int nStatus)
{
	LogAlarm(m_strDevice[ALARM_KEY], m_strEvent[ALARM_KEY], strAlarmCode, nStatus, m_nKeyCount[ALARM_KEY], m_strKey[ALARM_KEY], m_strValue[ALARM_KEY]);
}
CString CLogFromat::GetMotorName(int nAxis)
{
	CString strRet;

	switch (nAxis)
	{
		case M_LD_CV_LR_SHIFTER:
			strRet = _T("LOADING_TRAY_CONVEYOR_LR_SHIFTER");
			break;
		case M_LD_ELV:
			strRet = _T("LOAD_TRAY_STACKER_ELEVATOR");
			break;
		case M_EMPTY_ELV:
			strRet = _T("EMPTY_TRAY_STACKER_ELEVATOR");
			break;
		case M_ULD_1_ELV:
			strRet = _T("UNLOAD_TRAY#1_TRAY_STACKER_ELEVATOR");
			break;
		case M_ULD_2_ELV:
			strRet = _T("UNLOAD_TRAY#2_TRAY_STACKER_ELEVATOR");
			break;
		case M_RJ_EMPTY_ELV:
			strRet = _T("REJECT_TRAY_EMPTY_STACKER_ELEVATOR");
			break;
		case	M_RJ_ULD_1_ELV:
			strRet = _T("REJECT_TRAY#1_STACKER_ELEVATOR");
			break;
		case M_RJ_ULD_2_ELV:
			strRet = _T("REJECT_TRAY#2_STACKER_ELEVATOR");
			break;
		// 2번째 보드(8축)	
		
		case M_WORK_RBT_Y:
			strRet = _T("WORK_ROBOT_Y");
			break;
		case M_WORK_RBT_X:
			strRet = _T("WORK_ROBOT_X");
			break;
		case M_WORK_RBT_Z:
			strRet = _T("WORK_ROBOT_Z");
			break;
		case M_LD_BUF_SHIFTER_Y:
			strRet = _T("LOAD_BUFFER_SHIFTER_Y");
			break;
		case M_LD_BUF_ROTATOR:
			strRet = _T("LOAD_BUFFER_ROTATOR");
			break;
		case M_ULD_BUF_SHIFTER_Y:
			strRet = _T("UNLOAD_BUFFER_SHIFTER_Y");
			break;
		case M_ULD_BUF_ROTATOR:
			strRet = _T("UNLOAD_BUFFER_ROTATOR");
			break;
		//M_SPARE_1						,			//15: spare 1
		// 3번째 보드(8축)	
		case M_RETEST_BUF_1_ROTATOR:
			strRet = _T("RETEST_BUFFER#1_ROTATOR");
			break;		
		case M_RETEST_BUF_2_ROTATOR:
			strRet = _T("RETEST_BUFFER#2_ROTATOR");
			break;
		case M_WORKTRAY_TRANSFER:
			strRet = _T("WORK_TRAY_TRANSFER");
			break;
		case M_REJECTTRAY_TRANSFER:
			strRet = _T("REJECT_TRAY_TRANSFER");
			break;
		case M_TEST_RBT_Y:
			strRet = _T("TEST_ROBOT_Y");
			break;
		case M_TEST_RBT_X:
			strRet = _T("TEST_ROBOT_X");
			break;
		case M_TEST_RBT_Z:
			strRet = _T("TEST_ROBOT_Z");
			break;
		case M_TEST_DVC_EJECT_Z://23
			strRet = _T("TEST_ROBOT_DEVICE_EJECT_Z");
			break;
	}

	return strRet;
}

CString CLogFromat::GetDeviceID(int nAxis)
{
	CString strRet;

	switch (nAxis)
	{
		case M_LD_CV_LR_SHIFTER:
			strRet = _T("LOADING_TRAY_CONVEYOR_LR_SHIFTER");
			break;
		case M_LD_ELV:
			strRet = _T("LOAD_TRAY_STACKER_ELEVATOR");
			break;
		case M_EMPTY_ELV:
			strRet = _T("EMPTY_TRAY_STACKER_ELEVATOR");
			break;
		case M_ULD_1_ELV:
			strRet = _T("UNLOAD_TRAY#1_TRAY_STACKER_ELEVATOR");
			break;
		case M_ULD_2_ELV:
			strRet = _T("UNLOAD_TRAY#2_TRAY_STACKER_ELEVATOR");
			break;
		case M_RJ_EMPTY_ELV:
			strRet = _T("REJECT_TRAY_EMPTY_STACKER_ELEVATOR");
			break;
		case	M_RJ_ULD_1_ELV:
			strRet = _T("REJECT_TRAY#1_STACKER_ELEVATOR");
			break;
		case M_RJ_ULD_2_ELV:
			strRet = _T("REJECT_TRAY#2_STACKER_ELEVATOR");
			break;
		// 2번째 보드(8축)	
		
		case M_WORK_RBT_Y:
			strRet = _T("WORK_ROBOT_Y");
			break;
		case M_WORK_RBT_X:
			strRet = _T("WORK_ROBOT_X");
			break;
		case M_WORK_RBT_Z:
			strRet = _T("WORK_ROBOT_Z");
			break;
		case M_LD_BUF_SHIFTER_Y:
			strRet = _T("LOAD_BUFFER_SHIFTER_Y");
			break;
		case M_LD_BUF_ROTATOR:
			strRet = _T("LOAD_BUFFER_ROTATOR");
			break;
		case M_ULD_BUF_SHIFTER_Y:
			strRet = _T("UNLOAD_BUFFER_SHIFTER_Y");
			break;
		case M_ULD_BUF_ROTATOR:
			strRet = _T("UNLOAD_BUFFER_ROTATOR");
			break;
		//M_SPARE_1						,			//15: spare 1
		// 3번째 보드(8축)	
		case M_RETEST_BUF_1_ROTATOR:
			strRet = _T("RETEST_BUFFER#1_ROTATOR");
			break;		
		case M_RETEST_BUF_2_ROTATOR:
			strRet = _T("RETEST_BUFFER#2_ROTATOR");
			break;
		case M_WORKTRAY_TRANSFER:
			strRet = _T("WORK_TRAY_TRANSFER");
			break;
		case M_REJECTTRAY_TRANSFER:
			strRet = _T("REJECT_TRAY_TRANSFER");
			break;
		case M_TEST_RBT_Y:
			strRet = _T("TEST_ROBOT_Y");
			break;
		case M_TEST_RBT_X:
			strRet = _T("TEST_ROBOT_X");
			break;
		case M_TEST_RBT_Z:
			strRet = _T("TEST_ROBOT_Z");
			break;
		case M_TEST_DVC_EJECT_Z://23
			strRet = _T("TEST_ROBOT_DEVICE_EJECT_Z");
			break;
	}

	return strRet;
}

CString CLogFromat::GetSiteName(int nSite)
{
	CString strRet;

	switch (nSite)
	{
		case THD_LD_CV_STACKER_LEFT:
			strRet = _T("LOADING_CONVEYOR_LEFT_SHIFTER_SITE");
			break;
		case THD_LD_CV_STACKER_RIGHT:
			strRet = _T("LOADING_CONVEYOR_RIGHT_SHIFTER_SITE");
			break;
		case THD_LD_STACKER:
			strRet = _T("LOAD_TRAY_STACKER_SITE");
			break;
		case THD_LD_TRAY_PLATE:
			strRet = _T("LOAD_TRAY_PLATE_SITE");
			break;
		case THD_EMPTY_STACKER:
			strRet = _T("EMPTY_STACKER_SITE");
			break;
		case THD_ULD_1_STACKER:
			strRet = _T("UNLOAD_TRAY#1_STACKER_SITE");
			break;
		case THD_ULD_2_STACKER:
			strRet = _T("UNLOAD_TRAY#2_STACKER_SITE");
			break;
		case THD_REJECT_EMPTY_STACKER:
			strRet = _T("REJECT_EMPTY_STACKER_SITE");
			break;
		case THD_REJECT_OUT_1_STACKER:
			strRet = _T("REJECT_OUT#1_STACKER_SITE");
			break;
		case THD_REJECT_OUT_2_STACKER:
			strRet = _T("REJECT_OUT#2_STACKER_SITE");
			break;
		case THD_WORK_RBT:
			strRet = _T("WORK_ROBOT_SITE");
			break;
		case THD_TEST_RBT:
			strRet = _T("TEST_ROBOT_SITE");
			break;
		case THD_LD_BUFF:
			strRet = _T("LOAD_BUFFER_SITE");
			break;
		case THD_ULD_BUFF:
			strRet = _T("UNLOAD_BUFFER_SITE");
			break;
		case THD_RETEST_1_BUFF:
			strRet = _T("RETEST_BUFFER#1_SITE");
			break;
		case THD_RETEST_2_BUFF:
			strRet = _T("RETEST_BUFFER#2_SITE");
			break;
		case THD_WORK_TRANSFER:
			strRet = _T("WORK_TRANSFER_SITE");
			break;
		case THD_REJECT_TRANSFER:
			strRet = _T("REJECT_TRANSFER_SITE");
			break;
		case THD_TESTSITE_1:
			strRet = _T("TESTSITE_#1_SITE");
			break;
		case THD_TESTSITE_2:
			strRet = _T("TESTSITE_#2_SITE");
			break;
		case THD_TESTSITE_3:
			strRet = _T("TESTSITE_#3_SITE");
			break;
		case THD_TESTSITE_4:
			strRet = _T("TESTSITE_#4_SITE");
			break;
		case THD_TESTSITE_5:
			strRet = _T("TESTSITE_#5_SITE");
			break;
		case THD_TESTSITE_6:
			strRet = _T("TESTSITE_#6_SITE");
			break;
		case THD_TESTSITE_7:
			strRet = _T("TESTSITE_#7_SITE");
			break;
		case THD_TESTSITE_8:
			strRet = _T("TESTSITE_#8_SITE");
			break;
	}
		return strRet;
}

CString CLogFromat::GetRunStatus()
{
	CString strRet;

	switch (st_handler_info.nRunStatus)
	{
		case dSTOP:
			strRet = _T("STOP");
			break;

		case dRUN:
			strRet = _T("RUN");
			break;

		case dIDLE:
			strRet = _T("IDLE");
			break;

		case dJAM:
			strRet = _T("JAM");
			break;

		case dLOTEND:
			strRet = _T("LOTEND");
			break;

		case dINIT:
			strRet = _T("INIT");
			break;

		case dWARNING:
			strRet = _T("WARNING");
			break;

		case dLOCK:
			strRet = _T("LOCK");
			break;

		case dSELFCHECK:
			strRet = _T("SELFCHECK");
			break;

		case dMAINT:
			strRet = _T("MAINT");
			break;

		case dREINSTATE:
			strRet = _T("REINSTATE");
			break;

		case dRECOVERY:
			strRet = _T("RECOVERY");
			break;
	}
	return strRet;
}

void CLogFromat::OnLogMotorMove(int nAxis, int nStatus, double dPos)
{
	//CString strLogKey[10];
	//CString strLogData[10];
	CString strAxis;
	CString strStatus[2] = {_T("END"), _T("START")};
	CString strPos;
	CString strDvcID;
	CString strRunStatus;
	CString strMaterialType = _T("");
	CString strMaterialId	= _T("");

	int i, j;
	strDvcID	= GetDeviceID(nAxis);
	strAxis		= GetMotorName(nAxis);
	strPos.Format(_T("%0.3f"), dPos);

	m_strLogKeyM[nAxis][0]	= _T("START");
	m_strLogKeyM[nAxis][1]	= _T("END");
	m_strLogKeyM[nAxis][2]	= _T("MOTION");
	m_strLogDataM[nAxis][0].Format(_T("%.3f"), COMI.Get_MotCurrentPos(nAxis));
	m_strLogDataM[nAxis][1].Format(_T("%.3f"), dPos);
	m_strLogDataM[nAxis][2]	= strStatus[nStatus];

	if (nStatus == 1)
	{
		if (m_nMoveFlag[nAxis] == YES)
		{
			LogFunction(strDvcID, _T("MOTOR MOVE") , 1, strMaterialId, strMaterialType, 2, m_strLogKeyM[nAxis], m_strLogDataM[nAxis]);
		}
		else
		{
			m_nMoveFlag[nAxis] = YES;
		}
	}
	else
	{
		m_nMoveFlag[nAxis] = NO;
	}

	switch(nAxis)
	{
		case M_LD_CV_LR_SHIFTER:
			break;

		case M_LD_ELV:
			for (i=0; i<st_recipe_info.nTrayX; i++)
			{
				for (j=0; j<st_recipe_info.nTrayY; j++)
				{
					if (st_tray_info[THD_LD_TRAY_PLATE].st_pcb_info[i][j].nYesNo == YES)
					{
						//st_tray_info[THD_LD_TRAY_PLATE].st_pcb_info[y][x].strLotNo	= st_tray_info[THD_LD_STACKER].strLotNo;
						strMaterialId	= st_tray_info[THD_LD_TRAY_PLATE].st_pcb_info[i][j].strLotNo;
						strMaterialType = _T("CDIMM");
						break;
					}
				}
			}
			break;
			
		case M_EMPTY_ELV:
			break;
			
		case M_ULD_1_ELV:
			break;
	
		case M_ULD_2_ELV:
			break;
	
		case M_RJ_EMPTY_ELV:
			break;
		case	M_RJ_ULD_1_ELV:
			break;
		case M_RJ_ULD_2_ELV:
			break;
		case M_WORK_RBT_Y:
		case M_WORK_RBT_X:
		case M_WORK_RBT_Z:
			if (st_picker[THD_WORK_RBT].st_pcb_info[0].nYesNo == YES)
			{
				strMaterialId	= st_picker[THD_WORK_RBT].st_pcb_info[0].strSerialNo;
				strMaterialType = _T("CDIMM");					
			}
			break;

		case M_LD_BUF_SHIFTER_Y:
		case M_LD_BUF_ROTATOR:
			for(i = 0; i < 4; i++)
			{
				if( st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nYesNo == YES)
				{
						strMaterialId	= st_buffer_info[THD_LD_BUFF].st_pcb_info[i].strSerialNo;
						strMaterialType = _T("CDIMM");
						break;
				}
			}
			break;

		case M_ULD_BUF_SHIFTER_Y:		
		case M_ULD_BUF_ROTATOR:
			for(i = 0; i < 4; i++)
			{
				if( st_buffer_info[THD_ULD_BUFF].st_pcb_info[i].nYesNo == YES)
				{
						strMaterialId	= st_buffer_info[THD_ULD_BUFF].st_pcb_info[i].strSerialNo;
						strMaterialType = _T("CDIMM");
						break;
				}
			}
			break;
			
		//M_SPARE_1						,			//15: spare 1
		// 3번째 보드(8축)	
		case M_RETEST_BUF_1_ROTATOR:
			for( i = 0; i < st_recipe_info.nRetestBuffer_Num; i++)
			{
				if( st_buffer_info[THD_RETEST_1_BUFF].st_pcb_info[i].nYesNo == YES)
				{
						strMaterialId	= st_buffer_info[THD_RETEST_1_BUFF].st_pcb_info[i].strSerialNo;
						strMaterialType = _T("CDIMM");
						break;
				}
			}
			break;		
		case M_RETEST_BUF_2_ROTATOR:
			for( i = 0; i < st_recipe_info.nRetestBuffer_Num; i++)
			{
				if( st_buffer_info[THD_RETEST_2_BUFF].st_pcb_info[i].nYesNo == YES)
				{
						strMaterialId	= st_buffer_info[THD_RETEST_2_BUFF].st_pcb_info[i].strSerialNo;
						strMaterialType = _T("CDIMM");
						break;
				}
			}
			break;

		case M_WORKTRAY_TRANSFER:
			break;
		case M_REJECTTRAY_TRANSFER:
			break;

		case M_TEST_RBT_Y:
		case M_TEST_RBT_X:
		case M_TEST_RBT_Z:
			for( i = 0; i < 4; i++ )
			{
				if (st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == YES)
				{
					strMaterialId	= st_picker[THD_TEST_RBT].st_pcb_info[i].strSerialNo;
					strMaterialType = _T("CDIMM");	
					break;
				}
			}
			break;
		case M_TEST_DVC_EJECT_Z://23
			break;
	
	}
	//LogFunction(strDvcID, _T("MOTOR MOVE") , nStatus, st_recipe_info.strCokTypeName[st_recipe_info.nHifix - 1], _T("SSD"), 3, strLogKey, strLogData);
	if (nStatus == 1) LogFunction(strDvcID, _T("MOTOR MOVE") , 0, strMaterialId, strMaterialType, 2, m_strLogKeyM[nAxis], m_strLogDataM[nAxis]);
	else LogFunction(strDvcID, _T("MOTOR MOVE") , 1, strMaterialId, strMaterialType, 2, m_strLogKeyM[nAxis], m_strLogDataM[nAxis]);
}

void CLogFromat::OnTransferLog(int nSite, CString strEvent, CString strMatID, CString strMatType, CString strStart, CString strEnd)
{
//	int i;
	CString strDvcID;
	CString strLogKey[10];
	CString strLogData[10];

	strDvcID = GetSiteName(nSite);

	//LogTransfer(strDvcID, strEvent, strMatID, strMatType, strStart, strEnd, 0, strLogKey, strLogData);
}

//LogAlarm(CString strID, CString strEvent, CString strCode, int nStatus, int nDataCount, CString *strKey, CString *strData);
void CLogFromat::LogAlarm(CString strID, CString strEvent, CString strCode, int nStatus, int nDataCount, CString *strKey, CString *strData)
{
	CFileException e;
	CStdioFile file;

	SYSTEMTIME Time;	

	int i;
	int nAdd;

	CString strTime[3];
	CString strLog = _T("");
	CString strTemp;

	GetLocalTime(&Time);

	strTime[1].Format(_T("%04d/%02d/%02d"), Time.wYear, 
											Time.wMonth, 
											Time.wDay);
	strTime[0] += strTime[1];
	strTime[0] += _T("\t");

	strTime[2].Format(_T("%02d:%02d:%02d.%03d"), Time.wHour, 
												 Time.wMinute, 
												 Time.wSecond,
												 Time.wMilliseconds);
	strTime[0] += strTime[2];
	strTime[0] += _T("\t");

	strTemp.Format(_T("'%s'\t"), strID);
	strLog += strTemp;

	strTemp.Format(_T("'%s'\t"), _T("ALM"));
	strLog += strTemp;

	strTemp.Format(_T("'%s'\t"), strEvent);
	strLog += strTemp;

	strTemp.Format(_T("'%s'\t"), strCode);
	strLog += strTemp;

	if (nStatus == 0)
	{
		strTemp.Format(_T("'%s'\t"), _T("OCCURRED"));
		strLog += strTemp;
	}
	else
	{
		strTemp.Format(_T("'%s'\t"), _T("RELEASED"));
		strLog += strTemp;
	}

	nAdd = 0;
	for (i=0; i<nDataCount; i++)
	{
		nAdd++;
		if (nAdd == nDataCount)
		{
			strTemp.Format(_T("('%s', '%s')"), strKey[i], strData[i]);
		}
		else
		{
			strTemp.Format(_T("('%s', '%s')\t"), strKey[i], strData[i]);
		}
		strLog += strTemp;
	}

	if (m_strOldAlarm == strLog)
	{
		//2011.0504 추가 같은 메세지가 나오는 문제 처리 
		m_dwAlarmWaitTime[1] = GetCurrentTime();
		m_dwAlarmWaitTime[2] = m_dwAlarmWaitTime[1] - m_dwAlarmWaitTime[0];
		
		if(m_dwAlarmWaitTime[2] < 0)
		{
			m_dwAlarmWaitTime[0] = GetTickCount();
			return;
		}
		
		if (m_dwFuncWaitTime[2] < 60000) 
		{
			//OK 파일로 내용 저장 
			return;
		}
	}

	m_dwAlarmWaitTime[0]	= GetCurrentTime();
	m_strOldAlarm			= strLog;

	strTime[0]				+= strLog;
	OnLogWrite(strTime[0]);
}

//LogFunction(CString strID, CString strEvent, CString strMatID, CString strMatType, int nDataCount, CString *strKey, CString *strData);
void CLogFromat::LogFunction(CString strID, CString strEvent, int nStatus, CString strMatID, CString strMatType, int nDataCount, CString *strKey, CString *strData)
{
	CFileException e;
	CStdioFile file;

	SYSTEMTIME Time;	

	int i;
	int nAdd;

	CString strTime[3];
	CString strLog = _T("");
	CString strTemp;
	CString strVal[2];

	GetLocalTime(&Time);

	strTime[1].Format(_T("%04d/%02d/%02d"), Time.wYear, 
											Time.wMonth, 
											Time.wDay);
	strTime[0] += strTime[1];
	strTime[0] += _T("\t");

	strTime[2].Format(_T("%02d:%02d:%02d.%03d"), Time.wHour, 
												 Time.wMinute, 
												 Time.wSecond,
												 Time.wMilliseconds);
	strTime[0] += strTime[2];
	strTime[0] += _T("\t");

	strVal[0] = strID;
	if (strVal[0] == _T("")) strVal[0] = _T("$");
	strTemp.Format(_T("'%s'\t"), strVal[0]);
	strLog += strTemp;

	strTemp.Format(_T("'%s'\t"), _T("FNC"));
	strLog += strTemp;

	strVal[0] = strEvent;
	if (strVal[0] == _T("")) strVal[0] = _T("$");
	strTemp.Format(_T("'%s'\t"), strVal[0]);
	strLog += strTemp;

	if (nStatus == 0)
	{
		strTemp.Format(_T("'%s'\t"), _T("START"));
		strLog += strTemp;
	}
	else
	{
		strTemp.Format(_T("'%s'\t"), _T("END"));
		strLog += strTemp;
	}

	strVal[0] = strMatID;
	if (strVal[0] == _T("")) strVal[0] = _T("$");
	strTemp.Format(_T("'%s'\t"), strVal[0]);
	strLog += strTemp;

	strVal[0] = strMatType;
	if (strVal[0] == _T("")) strVal[0] = _T("$");
	strTemp.Format(_T("'%s'\t"), strVal[0]);
	strLog += strTemp;

	strKey[nDataCount]		= _T("LOTID");
	strData[nDataCount]		= st_lot_info[LOT_CURR].strLotNo;
	strKey[nDataCount + 1]	= _T("RECIPE");
	strData[nDataCount + 1]	= st_basic_info.strDeviceName;

	nAdd = 0;
	for (i=0; i<nDataCount + 2; i++)
	{
		nAdd++;
		if (nAdd == (nDataCount + 2))
		{
			strVal[0] = strKey[i];
			if (strVal[0] == _T("")) strVal[0] = _T("$");
			strVal[1] = strData[i];
			if (strVal[1] == _T("")) strVal[1] = _T("$");

			strTemp.Format(_T("('%s', '%s')"), strVal[0], strVal[1]);
		}
		else
		{
			strVal[0] = strKey[i];
			if (strVal[0] == _T("")) strVal[0] = _T("$");
			strVal[1] = strData[i];
			if (strVal[1] == _T("")) strVal[1] = _T("$");

			strTemp.Format(_T("('%s', '%s')\t"), strVal[0], strVal[1]);
		}
		strLog += strTemp;
	}

	if (m_strOldFunc == strLog)
	{
		//2011.0504 추가 같은 메세지가 나오는 문제 처리 
		m_dwFuncWaitTime[1] = GetCurrentTime();
		m_dwFuncWaitTime[2] = m_dwFuncWaitTime[1] - m_dwFuncWaitTime[0];
		
		if(m_dwFuncWaitTime[2] < 0)
		{
			m_dwFuncWaitTime[0] = GetTickCount();
			return;
		}

		if (m_dwFuncWaitTime[2] < 60000) 
		{
			//OK 파일로 내용 저장 
			return;
		}
	}

	m_dwFuncWaitTime[0]	= GetCurrentTime();
	m_strOldFunc			= strLog;

	strTime[0]				+= strLog;
	OnLogWrite(strTime[0]);
}

//LogProcess(CString strID, CString strEvent, int nStatus, CString strMatID, CString strLotID, CString strRecipe, int nDataCount, CString *strKey, CString *strData);
void CLogFromat::LogProcess(CString strID, CString strEvent, int nStatus, CString strMatID, CString strLotID, CString strRecipe, int nDataCount, CString *strKey, CString *strData)
{
	CFileException e;
	CStdioFile file;

	SYSTEMTIME Time;	

	int i;
	int nAdd;

	CString strTime[3];
	CString strLog = _T("");
	CString strTemp;
	CString strVal[3];

	GetLocalTime(&Time);

	strTime[1].Format(_T("%04d/%02d/%02d"), Time.wYear, 
											Time.wMonth, 
											Time.wDay);
	strTime[0] += strTime[1];
	strTime[0] += _T("\t");

	strTime[2].Format(_T("%02d:%02d:%02d.%03d"), Time.wHour, 
												 Time.wMinute, 
												 Time.wSecond,
												 Time.wMilliseconds);
	strTime[0] += strTime[2];
	strTime[0] += _T("\t");

	strVal[0] = strID;
	if (strVal[0] == _T("")) strVal[0] = _T("$");
	strTemp.Format(_T("'%s'\t"), strVal[0]);
	strLog += strTemp;

	strTemp.Format(_T("'%s'\t"), _T("PRC"));
	strLog += strTemp;

	strVal[0] = strEvent;
	if (strVal[0] == _T("")) strVal[0] = _T("$");
	strTemp.Format(_T("'%s'\t"), strVal[0]);
	strLog += strTemp;

	if (nStatus == 0)
	{
		strTemp.Format(_T("'%s'\t"), _T("START"));
		strLog += strTemp;
	}
	else
	{
		strTemp.Format(_T("'%s'\t"), _T("END"));
		strLog += strTemp;
	}

	strVal[0] = strMatID;
	if (strVal[0] == _T("")) strVal[0] = _T("$");
	strTemp.Format(_T("'%s'\t"), strVal[0]);
	strLog += strTemp;

	strVal[0] = strLotID;
	if (strVal[0] == _T("")) strVal[0] = _T("$");
	strTemp.Format(_T("'%s'\t"), strVal[0]);
	strLog += strTemp;

	strVal[0] = strRecipe;
	if (strVal[0] == _T("")) strVal[0] = _T("$");
	strTemp.Format(_T("'%s'\t"), strVal[0]);
	strLog += strTemp;

	nAdd = 0;
	for (i=0; i<nDataCount; i++)
	{
		nAdd++;
		if (nAdd == nDataCount)
		{
			strVal[0] = strKey[i];
			if (strVal[0] == _T("")) strVal[0] = _T("$");
			strVal[1] = strData[i];
			if (strVal[1] == _T("")) strData[1] = _T("$");

			strTemp.Format(_T("('%s', '%s')"), strVal[0], strVal[1]);
		}
		else
		{
			strVal[0] = strKey[i];
			if (strVal[0] == _T("")) strVal[0] = _T("$");
			strVal[1] = strData[i];
			if (strVal[1] == _T("")) strData[1] = _T("$");

			strTemp.Format(_T("('%s', '%s')\t"), strVal[0], strVal[1]);
		}
		strLog += strTemp;
	}

	if (m_strOldProcess == strLog)
	{
		//2011.0504 추가 같은 메세지가 나오는 문제 처리 
		m_dwProcessWaitTime[1] = GetCurrentTime();
		m_dwProcessWaitTime[2] = m_dwProcessWaitTime[1] - m_dwProcessWaitTime[0];
		
		if(m_dwProcessWaitTime[2] < 0)
		{
			m_dwProcessWaitTime[0] = GetTickCount();
			return;
		}
		
		if (m_dwFuncWaitTime[2] < 60000) 
		{
			//OK 파일로 내용 저장 
			return;
		}
	}

	m_dwProcessWaitTime[0]	= GetCurrentTime();
	m_strOldProcess			= strLog;

	strTime[0]				+= strLog;
	OnLogWrite(strTime[0]);
}

//LogTransfer(CString strID, CString strEvent, CString strMatID, CString strMatType, CString strStartPos, CString strEndPos, int nDataCount, CString *strKey, CString *strData);
void CLogFromat::LogTransfer(CString strID, CString strEvent, int nStatus, CString strMatID, CString strMatType, CString strStartPos, CString strEndPos, int nDataCount, CString *strKey, CString *strData)
{
	CFileException e;
	CStdioFile file;

	SYSTEMTIME Time;	

	int i;
	int nAdd;

	CString strTime[3];
	CString strFile;
	CString strLog = _T("");
	CString strTemp;
	CString strVal[2];

	GetLocalTime(&Time);

	strTime[1].Format(_T("%04d/%02d/%02d"), Time.wYear, 
											Time.wMonth, 
											Time.wDay);
	strTime[0] += strTime[1];
	strTime[0] += _T("\t");

	strTime[2].Format(_T("%02d:%02d:%02d.%03d"), Time.wHour, 
												 Time.wMinute, 
												 Time.wSecond,
												 Time.wMilliseconds);
	strTime[0] += strTime[2];
	strTime[0] += _T("\t");

	strVal[0] = strID;
	if (strVal[0] == _T("")) strVal[0] = _T("$");
	strTemp.Format(_T("'%s'\t"), strVal[0]);
	strLog += strTemp;

	strTemp.Format(_T("'%s'\t"), _T("XFR"));
	strLog += strTemp;

	strVal[0] = strEvent;
	if (strVal[0] == _T("")) strVal[0] = _T("$");
	strTemp.Format(_T("'%s'\t"), strVal[0]);
	strLog += strTemp;

	if (nStatus == 0)
	{
		strTemp.Format(_T("'%s'\t"), _T("START"));
		strLog += strTemp;
	}
	else
	{
		strTemp.Format(_T("'%s'\t"), _T("END"));
		strLog += strTemp;
	}

	strVal[0] = strMatID;
	if (strVal[0] == _T("")) strVal[0] = _T("$");
	strTemp.Format(_T("'%s'\t"), strVal[0]);
	strLog += strTemp;

	strVal[0] = strMatType;
	if (strVal[0] == _T("")) strVal[0] = _T("$");
	strTemp.Format(_T("'%s'\t"), strVal[0]);
	strLog += strTemp;

	strVal[0] = strStartPos;
	if (strVal[0] == _T("")) strVal[0] = _T("$");
	strTemp.Format(_T("'%s'\t"), strVal[0]);
	strLog += strTemp;

	strVal[0] = strEndPos;
	if (strVal[0] == _T("")) strVal[0] = _T("$");
	strTemp.Format(_T("'%s'\t"), strVal[0]);
	strLog += strTemp;

	nAdd = 0;
	for (i=0; i<nDataCount; i++)
	{
		nAdd++;
		if (nAdd == nDataCount)
		{
			strVal[0] = strKey[i];
			if (strVal[0] == _T("")) strVal[0] = _T("$");
			strVal[1] = strData[i];
			if (strVal[1] == _T("")) strData[1] = _T("$");

			strTemp.Format(_T("('%s', '%s')"), strVal[0], strVal[1]);
		}
		else
		{
			strVal[0] = strKey[i];
			if (strVal[0] == _T("")) strVal[0] = _T("$");
			strVal[1] = strData[i];
			if (strVal[1] == _T("")) strData[1] = _T("$");

			strTemp.Format(_T("('%s', '%s')\t"), strVal[0], strVal[1]);
		}
		strLog += strTemp;
	}

	if (m_strOldTrans == strLog)
	{
		//2011.0504 추가 같은 메세지가 나오는 문제 처리 
		m_dwTransWaitTime[1] = GetCurrentTime();
		m_dwTransWaitTime[2] = m_dwTransWaitTime[1] - m_dwTransWaitTime[0];
		
		if(m_dwTransWaitTime[2] < 0)
		{
			m_dwTransWaitTime[0] = GetTickCount();
			return;
		}
		
		if (m_dwFuncWaitTime[2] < 60000) 
		{
			//OK 파일로 내용 저장 
			return;
		}
	}

	m_dwTransWaitTime[0]	= GetCurrentTime();
	m_strOldTrans			= strLog;

	strTime[0]				+= strLog;
	OnLogWrite(strTime[0]);
}

//LogLotInform(CString strID, CString strEvent, CString strLotID, CString strRecipe, CString strCarrierID, int nDataCount, CString *strKey, CString *strData);
void CLogFromat::LogLotInform(CString strID, CString strEvent, CString strLotID, CString strRecipe, CString strCarrierID, int nDataCount, CString *strKey, CString *strData)
{
	CFileException e;
	CStdioFile file;

	SYSTEMTIME Time;	

	int i;
	int nAdd;

	CString strTime[2];
	CString strLog = _T("");
	CString strTemp;
	CString strVal[2];

	GetLocalTime(&Time);

	strTime[0].Format(_T("%04d/%02d/%02d"), Time.wYear, 
											Time.wMonth, 
											Time.wDay);
	strLog += strTime[0];
	strLog += _T("\t");

	strTime[1].Format(_T("%02d:%02d:%02d.%03d"), Time.wHour, 
												 Time.wMinute, 
												 Time.wSecond,
												 Time.wMilliseconds);
	strLog += strTime[1];
	strLog += _T("\t");

	strVal[0] = strID;
	if (strVal[0] == _T("")) strVal[0] = _T("$");
	strTemp.Format(_T("'%s'\t"), strVal[0]);
	strLog += strTemp;

	strTemp.Format(_T("'%s'\t"), _T("LEH"));
	strLog += strTemp;

	strVal[0] = strEvent;
	if (strVal[0] == _T("")) strVal[0] = _T("$");
	strTemp.Format(_T("'%s'\t"), strVal[0]);
	strLog += strTemp;

	strVal[0] = strLotID;
	if (strVal[0] == _T("")) strVal[0] = _T("$");
	strTemp.Format(_T("'%s'\t"), strVal[0]);
	strLog += strTemp;

	strVal[0] = strRecipe;
	if (strVal[0] == _T("")) strVal[0] = _T("$");
	strTemp.Format(_T("'%s'\t"), strVal[0]);
	strLog += strTemp;

	strVal[0] = strCarrierID;
	if (strVal[0] == _T("")) strVal[0] = _T("$");
	strTemp.Format(_T("'%s'\t"), strVal[0]);
	strLog += strTemp;

	nAdd = 0;
	for (i=0; i<nDataCount; i++)
	{
		nAdd++;
		if (nAdd == nDataCount)
		{
			strVal[0] = strKey[i];
			if (strVal[0] == _T("")) strVal[0] = _T("$");
			strVal[1] = strData[i];
			if (strVal[1] == _T("")) strData[1] = _T("$");

			strTemp.Format(_T("('%s', '%s')"), strVal[0], strVal[1]);
		}
		else
		{
			strVal[0] = strKey[i];
			if (strVal[0] == _T("")) strVal[0] = _T("$");
			strVal[1] = strData[i];
			if (strVal[1] == _T("")) strData[1] = _T("$");

			strTemp.Format(_T("('%s', '%s')\t"), strVal[0], strVal[1]);
		}
		strLog += strTemp;
	}

	OnLogWrite(strLog);
}

//LogConfig(CString strID, CString strEvent, CString strRecipe, int nDataCount, CString *strKey, CString *strData);
void CLogFromat::LogConfig(CString strID, CString strEvent, CString strRecipe, int nDataCount, CString *strKey, CString *strData)
{
	CFileException e;
	CStdioFile file;

	SYSTEMTIME Time;	

	int i;
	int nAdd;

	CString strTime[2];
	CString strLog = _T("");
	CString strTemp;
	CString strVal[2];

	GetLocalTime(&Time);

	strTime[0].Format(_T("%04d/%02d/%02d"), Time.wYear, 
											Time.wMonth, 
											Time.wDay);
	strLog += strTime[0];
	strLog += _T("\t");

	strTime[1].Format(_T("%02d:%02d:%02d.%03d"), Time.wHour, 
												 Time.wMinute, 
												 Time.wSecond,
												 Time.wMilliseconds);
	strLog += strTime[1];
	strLog += _T("\t");

	strVal[0] = strID;
	if (strVal[0] == _T("")) strVal[0] = _T("$");
	strTemp.Format(_T("'%s'\t"), strVal[0]);
	strLog += strTemp;

	strTemp.Format(_T("'%s'\t"), _T("CFG"));
	strLog += strTemp;

	strVal[0] = strRecipe;
	if (strVal[0] == _T("")) strVal[0] = _T("$");
	strTemp.Format(_T("'%s'\t"), strVal[0]);
	strLog += strTemp;

	nAdd = 0;
	for (i=0; i<nDataCount; i++)
	{
		nAdd++;
		if (nAdd == nDataCount)
		{
			strVal[0] = strKey[i];
			if (strVal[0] == _T("")) strVal[0] = _T("$");
			strVal[1] = strData[i];
			if (strVal[1] == _T("")) strData[1] = _T("$");

			strTemp.Format(_T("('%s', '%s')"), strVal[0], strVal[1]);
		}
		else
		{
			strVal[0] = strKey[i];
			if (strVal[0] == _T("")) strVal[0] = _T("$");
			strVal[1] = strData[i];
			if (strVal[1] == _T("")) strData[1] = _T("$");

			strTemp.Format(_T("('%s', '%s')\t"), strVal[0], strVal[1]);
		}
		strLog += strTemp;
	}

	OnLogWrite(strLog);
}
