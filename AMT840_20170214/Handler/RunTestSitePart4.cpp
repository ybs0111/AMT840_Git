#include "StdAfx.h"
#include "RunTestSitePart4.h"
#include "Variable.h"
#include "PublicFunction.h"
#include "ClientInterface4.h"
#include "CtlBd_Library.h"
#include "CtlBd_Variable.h"
#include "FastechPublic_IO.h"
#include "ComizoaPublic.h"
#include "LogCtrl.h"
#include "LogFromat.h"

CRunTestSitePart4 clsTestSite4;

CRunTestSitePart4::CRunTestSitePart4(void)
{
	m_nBdNum = 0;
	m_nTHD_ID = THD_TESTSITE_4;
	m_nIO_No  = 3;
	m_nRunStep = -1;
	for(int i = 0; i < 10; i++ )
	{
		st_sync_info.nTestRbt_Dvc_Ld_Req[m_nTHD_ID][i] = 0;
	}
}


CRunTestSitePart4::~CRunTestSitePart4(void)
{
}


void CRunTestSitePart4::OnRunInit()
{
	int nRet_1, nRet_2=0;

	if (st_sync_info.nInit_Flag[THD_TESTSITE_4] != INIT_CLEAR)		return;		//INIT_CLEAR 일때만 초기화 작업을 한다. 초기화가 끝나면 INIT_CLEAR -> INIT_READY가 되기 떄문에...
	 
	switch (m_nInitStep)
	{
		case -1:			 

			m_nInitStep = 0;
			break;

		case 0:		

			m_nInitStep = 100;
			break;

		case 100:
			//kwlee 20160905 
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_Tsite4ExistChk, IO_ON);
			
			if (nRet_1 == IO_ON)
			{
				m_nInitStep = 1000;
			}
			else if (nRet_1 == IO_OFF)
			{
				m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_Tsite4ExistChk); 
				CTL_Lib.Alarm_Error_Occurrence(9301, dWARNING, m_strAlarmCode);
			}
			break;

		case 1000:
			Set_TestSite_UpDown(IO_ON);

			m_nInitStep = 1100;
			break;

		case 1100:
			nRet_1 = Chk_TestSite_UpDown(IO_ON);

			if (nRet_1 == RET_GOOD)
			{
				m_nInitStep = 1200;
			}
			else if (nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(9310, dWARNING, m_strAlarmCode);
			}
			break;

		case 1200:
			// door open 요청
			m_dwDoorWaitTime[0] = GetTickCount();

			// chamber door open 요청
			st_test_site_info[m_nTHD_ID].nTester_Req[0] = CTL_REQ;
			st_test_site_info[m_nTHD_ID].nTester_Req[1] = WORK_PICK;

			m_nInitStep = 1300;
			break;

		case 1300:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ChamberOpenChk[m_nIO_No],	IO_ON);  
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ChamberCloseChk[m_nIO_No],	IO_OFF);  

			if(nRet_1 == IO_ON && nRet_2 == IO_OFF )
			{
				m_dwDoorWaitTime[0] = GetTickCount();

				m_nInitStep = 1400;
			}
			else
			{
				m_dwDoorWaitTime[1] = GetTickCount();
				m_dwDoorWaitTime[2] = m_dwDoorWaitTime[1] - m_dwDoorWaitTime[0];

				if (m_dwDoorWaitTime[2] <= (DWORD)0)
				{
					m_dwDoorWaitTime[0] = GetTickCount();
					break;
				}

				if (m_dwDoorWaitTime[2] > (DWORD) 60000)
				{
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ChamberOpenChk[m_nIO_No]); 
					CTL_Lib.Alarm_Error_Occurrence(9320, dWARNING, m_strAlarmCode);
				}
			}
			break;

		case 1400:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ChamberOpenChk[m_nIO_No],	IO_ON);  
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ChamberCloseChk[m_nIO_No],	IO_OFF);  

			if(nRet_1 == IO_ON && nRet_2 == IO_OFF )
			{
				m_dwDoorWaitTime[1] = GetTickCount();
				m_dwDoorWaitTime[2] = m_dwDoorWaitTime[1] - m_dwDoorWaitTime[0];

				if (m_dwDoorWaitTime[2] <= (DWORD)0)
				{
					m_dwDoorWaitTime[0] = GetTickCount();
					break;
				}

				if (m_dwDoorWaitTime[2] > (DWORD)50)
				{
					m_nInitStep = 70000;
				}
			}
			else
			{
				m_nInitStep = 1300;
			}
			break;

		case 70000:
			st_sync_info.nInit_Flag[THD_TESTSITE_4] = INIT_READY; 
			 
			m_nInitStep = 0;
			break;
	}
}

void CRunTestSitePart4::OnRunMove()
{
	int nRet_1, nRet_2=0, nRet_3=0;
	int nCount = 0, nBin = 0, nFlag = 0, nTemp =0;
	int i;
	int nTime = 100;

	clsFunc.OnThreadFunctionStepTrace(17, m_nRunStep);	

	switch (m_nRunStep)
	{ 
		case -1: 

			m_nRunStep = 0;
			break;

		case 0:			

			m_nRunStep = 100;
			break;

		case 100:// 
			m_nRunStep = 1000;
			break; 

		case 1000:
			if( clsFunc.EnableSocketSiteWorking(m_nIO_No) == RET_ERROR)
			{
				st_handler_info.nRunStatus = dWARNING;
				break;
			}

			if(st_sync_info.nTestRbt_Dvc_Ld_Req[m_nTHD_ID][0] == CTL_REQ) //test socket에 놓을때 
			{
				// test site robot이 자재를 place 요청이 있으면
				if(st_sync_info.nTestRbt_Dvc_Ld_Req[m_nTHD_ID][1] == WORK_PLACE)
				{
					// 자재 place 요청이 있으면 
					if (st_test_site_info[m_nTHD_ID].nTester_Req[2] == 1)
					{
						// 현재 door가 열려 있는 상태이면 
						nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ChamberOpenChk[m_nIO_No],	IO_ON);  
						nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ChamberCloseChk[m_nIO_No],	IO_OFF);  

						if(nRet_1 == IO_ON && nRet_2 == IO_OFF )
						{
							// door open 상태이면 
							st_sync_info.nTestRbt_Dvc_Ld_Req[m_nTHD_ID][0] = CTL_READY;
							if (st_sync_info.nTestRbt_Dvc_Ld_Req[m_nTHD_ID][5] == BD_START)
							{
								// test site robot에서 테스트 시작이면.......
								// door close 시킴.....
								m_nRunStep = 3000;
							}
						}
						else 
						{
							// door가 닫혀 있는 상태이면 .....
							m_nRunStep = 2000;
						}
					}
					else
					{
						// door가 닫혀 있는 상태이면 .....
						m_nRunStep = 2000;
					}
				}
			}

			if(st_sync_info.nTestRbt_Dvc_UnLd_Req[m_nTHD_ID][0] == CTL_REQ)
			{
				// test site robot이 자재를 pick요청이 있으면 
				if(st_sync_info.nTestRbt_Dvc_UnLd_Req[m_nTHD_ID][1] == WORK_PICK)
				{
					//
					if (st_test_site_info[m_nTHD_ID].nTester_Req[2] == 1)
					{
						// 현재 door가 열려 있는 상태이면 
						nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ChamberOpenChk[m_nIO_No],	IO_ON);  
						nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ChamberCloseChk[m_nIO_No],	IO_OFF);  

						if(nRet_1 == IO_ON && nRet_2 == IO_OFF )
						{
							// door 체크
							st_sync_info.nTestRbt_Dvc_UnLd_Req[m_nTHD_ID][0] = CTL_READY;
						}
						else 
						{
							// door open 요청
							m_nRunStep = 2000;
						}
					}
					else
					{
						// door가 닫혀 있는 상태이면
						// door open 요청
						m_nRunStep = 2000;
					}
				}
			}

			// jtkim 20160926
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			if (m_nOpenDoor == YES)
			{
				m_nOpenDoor = NO;
					
				m_nRunStep = 10000;
				break;
			}

			if (m_nCloseDoor == YES)
			{
				m_nCloseDoor = NO;
				
				m_nRunStep = 20000;

				break;
			}
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			break;

		case 2000:
			// door open 요청
			m_dwDoorWaitTime[0] = GetTickCount();

			// chamber door open 요청
			st_test_site_info[m_nTHD_ID].nTester_Req[0] = CTL_REQ;
			st_test_site_info[m_nTHD_ID].nTester_Req[1] = WORK_PICK;

			m_nRunStep = 2100;
			break;

		case 2100:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ChamberOpenChk[m_nIO_No],	IO_ON);  
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ChamberCloseChk[m_nIO_No],	IO_OFF);  

			if(nRet_1 == IO_ON && nRet_2 == IO_OFF )
			{
				m_dwDoorWaitTime[0] = GetTickCount();

				m_nRunStep = 2200;
			}
			else
			{
				m_dwDoorWaitTime[1] = GetTickCount();
				m_dwDoorWaitTime[2] = m_dwDoorWaitTime[1] - m_dwDoorWaitTime[0];

				if (m_dwDoorWaitTime[2] <= (DWORD)0)
				{
					m_dwDoorWaitTime[0] = GetTickCount();
					break;
				}
				
				if (m_dwDoorWaitTime[2] > (DWORD) 60000)
				{
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ChamberOpenChk[m_nIO_No]); 
					CTL_Lib.Alarm_Error_Occurrence(9330, dWARNING, m_strAlarmCode);

					m_nRunStep = 2000;
				}
			}
			break;

		case 2200:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ChamberOpenChk[m_nIO_No],	IO_ON);  
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ChamberCloseChk[m_nIO_No],	IO_OFF);  

			if(nRet_1 == IO_ON && nRet_2 == IO_OFF )
			{
				m_dwDoorWaitTime[1] = GetTickCount();
				m_dwDoorWaitTime[2] = m_dwDoorWaitTime[1] - m_dwDoorWaitTime[0];

				if (m_dwDoorWaitTime[2] <= (DWORD)0)
				{
					m_dwDoorWaitTime[0] = GetTickCount();
					break;
				}
				
				if (m_dwDoorWaitTime[2] > (DWORD)nTime)
				{
					// chamber door open 완료
					st_test_site_info[m_nTHD_ID].nTester_Req[0] = CTL_FREE;

					st_test_site_info[m_nTHD_ID].nTester_Req[2] = 1;
					if(st_sync_info.nTestRbt_Dvc_Ld_Req[m_nTHD_ID][0] == CTL_REQ)
					{
						// 자재 place 요청한 상태이면 
						st_sync_info.nTestRbt_Dvc_Ld_Req[m_nTHD_ID][0] = CTL_READY;
						if (st_sync_info.nTestRbt_Dvc_Ld_Req[m_nTHD_ID][5] == BD_START)
						{
							// 자재 place 작업이 완료 되어 start 요청이 있으면 
							m_nRunStep = 3000;
						}
						else
						{
							m_nRunStep = 1000;
						}
						break;
					}

					if(st_sync_info.nTestRbt_Dvc_UnLd_Req[m_nTHD_ID][0] == CTL_REQ)
					{
						// 자재 pick 요청한 상태이면 
						st_sync_info.nTestRbt_Dvc_UnLd_Req[m_nTHD_ID][0] = CTL_READY;

						m_nRunStep = 1000;
						break;
					}
				}
			}
			else
			{
				m_nRunStep = 2100;
			}
			break;

		case 3000:
			// door close 요청
			m_dwDoorWaitTime[0] = GetTickCount(); //kwlee 2016.1003

			st_test_site_info[m_nTHD_ID].nTester_Req[0] = CTL_REQ;
			st_test_site_info[m_nTHD_ID].nTester_Req[1] = WORK_PLACE;

			m_nRunStep = 3100;
			break;

		case 3100:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ChamberOpenChk[m_nIO_No],	IO_ON);  
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ChamberCloseChk[m_nIO_No],	IO_OFF);  

			if(nRet_1 == IO_OFF && nRet_2 == IO_ON )
			{
				m_dwDoorWaitTime[0] = GetTickCount();

				m_nRunStep = 3200;
			}
			else
			{
				m_dwDoorWaitTime[1] = GetTickCount();
				m_dwDoorWaitTime[2] = m_dwDoorWaitTime[1] - m_dwDoorWaitTime[0];

				if (m_dwDoorWaitTime[2] <= (DWORD)0)
				{
					m_dwDoorWaitTime[0] = GetTickCount();
					break;
				}
				
				if (m_dwDoorWaitTime[2] > (DWORD) 60000)
				{
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ChamberCloseChk[m_nIO_No]); 
					CTL_Lib.Alarm_Error_Occurrence(9340, dWARNING, m_strAlarmCode);

					m_nRunStep = 3000;
				}
			}
			break;

		case 3200:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ChamberOpenChk[m_nIO_No],	IO_ON);  
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ChamberCloseChk[m_nIO_No],	IO_OFF);  

			if(nRet_1 == IO_OFF && nRet_2 == IO_ON )
			{
				m_dwDoorWaitTime[1] = GetTickCount();
				m_dwDoorWaitTime[2] = m_dwDoorWaitTime[1] - m_dwDoorWaitTime[0];

				if (m_dwDoorWaitTime[2] <= (DWORD)0)
				{
					m_dwDoorWaitTime[0] = GetTickCount();
					break;
				}
				
				if (m_dwDoorWaitTime[2] > (DWORD)nTime)
				{
					// chamber door open 완료
					st_test_site_info[m_nTHD_ID].nTester_Req[0] = CTL_FREE;

					st_test_site_info[m_nTHD_ID].nTester_Req[2] = 0;
					if (st_sync_info.nTestRbt_Dvc_Ld_Req[m_nTHD_ID][5] == BD_START)
					{
						st_sync_info.nTestRbt_Dvc_Ld_Req[m_nTHD_ID][5] = BD_NONE;

						// tester 통신정보 설정
						nCount = 0;
						for (i=0; i<8; i++)
						{
							//2017.0322
							//if (st_test_site_info[m_nTHD_ID].st_pcb_info[i].nYesNo == YES)
							//kwlee 2017.0614
							st_test_site_info[m_nTHD_ID].st_pcb_info[i].strBarcode1D[0].TrimLeft();
							st_test_site_info[m_nTHD_ID].st_pcb_info[i].strBarcode1D[0].TrimRight();

							if ( st_test_site_info[m_nTHD_ID].st_pcb_info[i].nYesNo == YES && st_test_site_info[m_nTHD_ID].st_pcb_info[i].strBarcode1D[0] != _T("") && st_test_site_info[m_nTHD_ID].st_pcb_info[i].strBarcode1D[0] != _T("ERROR") )
							{
								nCount++;
							}
						}
						st_test_site_info[m_nTHD_ID].nModuleCount	= nCount;
						//st_test_site_info[m_nTHD_ID].nBarcodeMode	= st_recipe_info.nBarcodeCount;
						//kwlee 2017.0511
						st_test_site_info[m_nTHD_ID].nBarcodeMode	= st_recipe_info.nBarcodeCnt;
						//st_test_site_info[m_nTHD_ID].strOptCode		= _T("2R");
						st_test_site_info[m_nTHD_ID].nRetry			= st_recipe_info.nTestRetest_Count;
						if (st_test_site_info[m_nTHD_ID].strLotNo == st_lot_info[LOT_CURR].strLotNo)
						{
							st_test_site_info[m_nTHD_ID].nTestMode	= st_lot_info[LOT_CURR].nTestMode;
							st_test_site_info[m_nTHD_ID].nInterface	= st_lot_info[LOT_CURR].nInterface;
							st_test_site_info[m_nTHD_ID].nQty		= st_lot_info[LOT_CURR].nQty;

							// jtkim 20160929
							st_test_site_info[m_nTHD_ID].strOptCode	= st_lot_info[LOT_CURR].strOpCode;
							if (st_test_site_info[m_nTHD_ID].strOptCode == _T(""))
							{
								st_test_site_info[m_nTHD_ID].strOptCode = _T("00");
							}
							//2017.0105
							st_test_site_info[m_nTHD_ID].strFabSite = st_lot_info[LOT_CURR].strFabSite;
						}
						else 
						{
							st_test_site_info[m_nTHD_ID].nPgmDown	= st_lot_info[LOT_NEXT].nPgmDown;
							st_test_site_info[m_nTHD_ID].nInterface	= st_lot_info[LOT_NEXT].nInterface;
							st_test_site_info[m_nTHD_ID].nQty		= st_lot_info[LOT_NEXT].nQty;

							// jtkim 20160929
							st_test_site_info[m_nTHD_ID].strOptCode	= st_lot_info[LOT_NEXT].strOpCode;
							if (st_test_site_info[m_nTHD_ID].strOptCode == _T(""))
							{
								st_test_site_info[m_nTHD_ID].strOptCode = _T("00");
							}
							//2017.0105
							st_test_site_info[m_nTHD_ID].strFabSite = st_lot_info[LOT_NEXT].strFabSite;
						}
						////////////////////////////////////////////////////////////////////////////////////////////
						st_test_site_info[m_nTHD_ID].nTestBdStart = BD_START;
					}
					m_nRunStep = 1000;
				}
			}
			else
			{
				m_nRunStep = 3100;
			}
			break;

		case 10000:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ChamberOpenChk[m_nIO_No],	IO_ON);  
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ChamberCloseChk[m_nIO_No],	IO_OFF);  

			if(nRet_1 == IO_ON && nRet_2 == IO_OFF )
			{
				// door open 상태이면 
				//clsInterC1.OnChamberDoor(st_test_site_info[m_nTHD_ID], 0);

				m_nRunStep = 1000;
			}
			else 
			{
				// door가 닫혀 있는 상태이면 .....
				m_nRunStep = 10100;
			}
			break;

		case 10100:
			// door open 요청
			m_dwDoorWaitTime[0] = GetTickCount();

			st_test_site_info[m_nTHD_ID].nTester_Req[0] = CTL_REQ;
			st_test_site_info[m_nTHD_ID].nTester_Req[1] = WORK_PICK;

			m_nRunStep = 10200;
			break;

		case 10200:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ChamberOpenChk[m_nIO_No],	IO_ON);  
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ChamberCloseChk[m_nIO_No],	IO_OFF);  

			if(nRet_1 == IO_ON && nRet_2 == IO_OFF )
			{
				m_dwDoorWaitTime[0] = GetTickCount();

				m_nRunStep = 10300;
			}
			else
			{
				m_dwDoorWaitTime[1] = GetTickCount();
				m_dwDoorWaitTime[2] = m_dwDoorWaitTime[1] - m_dwDoorWaitTime[0];

				if (m_dwDoorWaitTime[2] <= (DWORD)0)
				{
					m_dwDoorWaitTime[0] = GetTickCount();
					break;
				}
				
				if (m_dwDoorWaitTime[2] > (DWORD) 60000)
				{
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ChamberOpenChk[m_nIO_No]); 
					CTL_Lib.Alarm_Error_Occurrence(9350, dWARNING, m_strAlarmCode);

					m_nRunStep = 10100;
				}
			}
			break;

		case 10300:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ChamberOpenChk[m_nIO_No],	IO_ON);  
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ChamberCloseChk[m_nIO_No],	IO_OFF);  

			if(nRet_1 == IO_ON && nRet_2 == IO_OFF )
			{
				m_dwDoorWaitTime[1] = GetTickCount();
				m_dwDoorWaitTime[2] = m_dwDoorWaitTime[1] - m_dwDoorWaitTime[0];

				if (m_dwDoorWaitTime[2] <= (DWORD)0)
				{
					m_dwDoorWaitTime[0] = GetTickCount();
					break;
				}
				
				if (m_dwDoorWaitTime[2] > (DWORD)nTime)
				{
					// chamber door open 완료
					st_test_site_info[m_nTHD_ID].nTester_Req[0] = CTL_FREE;

					m_nRunStep = 1000;
				}
			}
			else
			{
				m_nRunStep = 10200;
			}
			break;

		case 20000:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ChamberOpenChk[m_nIO_No],	IO_ON);  
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ChamberCloseChk[m_nIO_No],	IO_OFF);  

			if(nRet_1 == IO_OFF && nRet_2 == IO_ON )
			{
				m_nRunStep = 1000;
			}
			else
			{
				m_nRunStep = 20100;
			}
			break;

		case 20100:
			// door close 요청
			m_dwDoorWaitTime[0] = GetTickCount();

			st_test_site_info[m_nTHD_ID].nTester_Req[0] = CTL_REQ;
			st_test_site_info[m_nTHD_ID].nTester_Req[1] = WORK_PLACE;

			m_nRunStep = 20200;
			break;

		case 20200:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ChamberOpenChk[m_nIO_No],	IO_ON);  
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ChamberCloseChk[m_nIO_No],	IO_OFF);  

			//2017.0325
			//if(nRet_1 == IO_ON && nRet_2 == IO_OFF)
			if( nRet_1 == IO_OFF && nRet_2 == IO_ON )
			{
				m_dwDoorWaitTime[0] = GetTickCount();

				m_nRunStep = 20300;
			}
			else
			{
				m_dwDoorWaitTime[1] = GetTickCount();
				m_dwDoorWaitTime[2] = m_dwDoorWaitTime[1] - m_dwDoorWaitTime[0];

				if (m_dwDoorWaitTime[2] <= (DWORD)0)
				{
					m_dwDoorWaitTime[0] = GetTickCount();
					break;
				}
				
				if (m_dwDoorWaitTime[2] > (DWORD) 60000)
				{
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ChamberOpenChk[m_nIO_No]); 
					CTL_Lib.Alarm_Error_Occurrence(9260, dWARNING, m_strAlarmCode);

					m_nRunStep = 20100;
				}
			}
			break;

		case 20300:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ChamberOpenChk[m_nIO_No],	IO_ON);  
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ChamberCloseChk[m_nIO_No],	IO_OFF);  

			//2017.0325
			//if(nRet_1 == IO_ON && nRet_2 == IO_OFF)
			if( nRet_1 == IO_OFF && nRet_2 == IO_ON )
			{
				m_dwDoorWaitTime[1] = GetTickCount();
				m_dwDoorWaitTime[2] = m_dwDoorWaitTime[1] - m_dwDoorWaitTime[0];

				if (m_dwDoorWaitTime[2] <= (DWORD)0)
				{
					m_dwDoorWaitTime[0] = GetTickCount();
					break;
				}
				
				if (m_dwDoorWaitTime[2] > (DWORD)nTime)
				{
					// chamber door open 완료
					st_test_site_info[m_nTHD_ID].nTester_Req[0] = CTL_FREE;

					m_nRunStep = 1000;
				}
			}
			else
			{
				m_nRunStep = 20200;
			}
			break;
	}
}

void CRunTestSitePart4::OnThreadRun()
{
//	return;
	CTL_Lib.Process_TestChamber_AcMotor(0, THD_TESTSITE_4, 3); //chamber open/close AC Motor 제어하는 루틴	
//	OnCommunication();
	OnCommunication();

	if (st_handler_info.nSystemLock != FALSE)  return;	// 현재 시스템 Lock 상태인 경우 리턴  


	switch(st_handler_info.nRunStatus)
	{
		case dINIT:
			OnRunInit();
			break;

		case dRUN:
			OnRunMove();					
			break;

		case dSTOP:
			m_dwWaitTime[0] = GetCurrentTime();

			if (st_handler_info.nAutoMode == NO) OnRunDoor();
			break;

		case dWARNING:
			m_dwWaitTime[0] = GetCurrentTime();
			break;

		case dSELFCHECK:
			break;

		case dREINSTATE:
			break;

		case dRECOVERY:
			break;

		default:
			break;
	}
}


void CRunTestSitePart4::OnCommunication()
{
	COleDateTime	 tCurr;
	COleDateTimeSpan tDiff;

	int nSite = THD_TESTSITE_1 + 3;
	int	nTime;
	int i;
	
	CString strTime;

	if (st_test_site_info[nSite].nTestBdStart == BD_START)
	{
		// test site board에 테스트 시작이면 
		st_test_site_info[nSite].tStart			= COleDateTime::GetCurrentTime();
		st_test_site_info[nSite].nTestBdStart	= BD_READY_CHK;

		if (st_basic_info.nModeDevice == WITH_DVC)
		{
			if (st_basic_info.nModeInterface == EQP_ON_LINE || st_basic_info.nModeTestInterface == EQP_ON_LINE)
			{
				clsInterC4.OnTesterReady(st_test_site_info[nSite]);

				// jtkim 20160930 Log
				clsLog.OnLogTestSite(1, nSite, 0, st_test_site_info[nSite]);
			}
			else
			{
				st_test_site_info[nSite].nTestBdStart	= BD_BIN_CHK;
			}
		}
		else
		{
			st_test_site_info[nSite].nTestBdStart	= BD_BIN_CHK;
		}
	}
	else
	{
		if (st_basic_info.nModeDevice == WITH_DVC)
		{
			// with device mode
			if (st_basic_info.nModeInterface == EQP_ON_LINE || st_basic_info.nModeTestInterface == EQP_ON_LINE)
			{
				// on-line mode이면
				//kwlee 2016.1025 임시 최희택 책임 요청..
				//5분 동안 BD_READY_CHK 상태 이면 다시 BD_START 
// 				tCurr	= COleDateTime::GetCurrentTime();
// 				tDiff	= tCurr - st_test_site_info[nSite].tStart;
// 
// 				nTime = (int)tDiff.GetTotalSeconds();
// 
// 				if (nTime >180)
// 				{
// 					if (st_test_site_info[nSite].nTestBdStart == BD_READY_CHK)
// 					{
// 						st_test_site_info[nSite].nTestBdStart = BD_START;
// 					}
// 				}
				/////

				if (st_test_site_info[nSite].nTestBdStart == BD_READY_OK)
				{ 
					st_test_site_info[nSite].nTestBdStart = BD_MODULE_CHK;

					clsInterC4.OnModuleInfo(st_test_site_info[nSite]);

					// jtkim 20160930 Log
					clsLog.OnLogTestSite(2, nSite, 0, st_test_site_info[nSite]);
				}
				else if (st_test_site_info[nSite].nTestBdStart == BD_MODULE_OK)
				{
					st_test_site_info[nSite].nTestBdStart = BD_START_CHK;

					clsInterC4.OnTesterStart(st_test_site_info[nSite]);

					// jtkim 20160930 Log
					clsLog.OnLogTestSite(3, nSite, 0, st_test_site_info[nSite]);
				}
				else if (st_test_site_info[nSite].nTestBdStart == BD_PGM_CHK)
				{
					tCurr	= COleDateTime::GetCurrentTime();
					tDiff	= tCurr - st_test_site_info[nSite].tStart;

					nTime = (int)tDiff.GetTotalSeconds();
					// jtkim 20160830
					st_test_site_info[nSite].dwBdTime = (DWORD)nTime;

					if (nTime > 60000)
					{
						// 1분동안 기다려도 pgm 다운로드 안 되었으면 
						st_test_site_info[nSite].tStart			= COleDateTime::GetCurrentTime();

						clsInterC4.OnPgmDownload(st_test_site_info[nSite]);
					}
					else
					{
						if (st_test_site_info[nSite].nPgmDown == YES)
						{
							st_test_site_info[nSite].nTestBdStart = BD_START_CHK;

							clsInterC4.OnTesterStart(st_test_site_info[nSite]);
						}
					}
				}
				else if (st_test_site_info[nSite].nTestBdStart == BD_START_OK)
				{
					st_test_site_info[nSite].nTestBdStart	= BD_BIN_CHK;

					st_test_site_info[nSite].tStart			= COleDateTime::GetCurrentTime();
				}
				else if (st_test_site_info[nSite].nTestBdStart == BD_BIN_CHK)
				{
					tCurr	= COleDateTime::GetCurrentTime();
					tDiff	= tCurr - st_test_site_info[nSite].tStart;

					nTime = (int)tDiff.GetTotalSeconds();
					// jtkim 20160830
					st_test_site_info[nSite].dwBdTime = (DWORD)nTime;

					if (nTime > (st_recipe_info.nAbortTime))
					{
						clsInterC4.OnTesterAbort(st_test_site_info[nSite]);

						strTime.Format(_T("%02d:%02d:%02d"), tDiff.GetHours(), tDiff.GetMinutes(), tDiff.GetSeconds());
						for (i=0; i<8; i++)
						{
							if (st_test_site_info[nSite].st_pcb_info[i].nYesNo == YES)
							{
								st_test_site_info[nSite].st_pcb_info[i].nTestBdStart	= BD_END;
								st_test_site_info[nSite].st_pcb_info[i].strBdTime		= strTime;
							}

						}

						// jtkim 20160930 Log
						clsLog.OnLogTestSite(5, nSite, 0, st_test_site_info[nSite]);
					}
					else
					{
						//kwlee 2016.1021 tester에서 프로그램 적용 되지 않아 원복함..
						// jtkim 20160927
// 						if (m_nCloseDoor == YES)
// 						{
// 							m_nCloseDoor = NO;
// 
// 							clsInterC4.OnChamberDoor(st_test_site_info[m_nTHD_ID], 1);
// 
// 							// jtkim 20160930 Log
// 							clsLog.OnLogTestSite(4, nSite, 0, st_test_site_info[nSite]);
// 						}

// 						if (m_nCloseDoor == YES)//2016.1020
// 						{
// 							st_handler_info.m_tlDooropen[3][1] = GetCurrentTime();
// 							st_handler_info.m_tlDooropen[3][2] = st_handler_info.m_tlDooropen[3][1] - st_handler_info.m_tlDooropen[3][0];
// 							if( st_handler_info.m_tlDooropen[3][2] <= 0 ) st_handler_info.m_tlDooropen[3][0] = GetCurrentTime();
// 							if( st_handler_info.m_tlDooropen[3][2] > 1500 )
// 							{
// 								m_nCloseDoor = NO;
// 
// 								clsInterC4.OnChamberDoor(st_test_site_info[m_nTHD_ID], 1);
// 
// 								// jtkim 20160930 Log
// 								clsLog.OnLogTestSite(4, nSite, 0, st_test_site_info[nSite]);
// 							}
// 
// 						}
					}
				}
				else if (st_test_site_info[nSite].nTestBdStart == BD_BIN_OK)
				{
					tCurr	= COleDateTime::GetCurrentTime();
					tDiff	= tCurr - st_test_site_info[nSite].tStart;
					strTime.Format(_T("%02d:%02d:%02d"), tDiff.GetHours(), tDiff.GetMinutes(), tDiff.GetSeconds());
					for (i=0; i<8; i++)
					{
						if (st_test_site_info[nSite].st_pcb_info[i].nYesNo == YES)
						{
							st_test_site_info[nSite].st_pcb_info[i].nTestBdStart	= BD_END;
							st_test_site_info[nSite].st_pcb_info[i].strBdTime		= strTime;
						}

					}

					// jtkim 20161008 test time 구하기 위해
					nTime = (int)tDiff.GetTotalSeconds();
					if (st_lot_info[LOT_CURR].strLotNo == st_test_site_info[nSite].strLotNo)
					{
						st_lot_info[LOT_CURR].nBdTime += nTime;
						st_lot_info[LOT_CURR].nBdTimeCount++;
					}
					else
					{
						st_lot_info[LOT_NEXT].nBdTime += nTime;
						st_lot_info[LOT_NEXT].nBdTimeCount++;
					}
					// jtkim 20160930 Log
					clsLog.OnLogTestSite(5, nSite, 0, st_test_site_info[nSite]);

					st_test_site_info[nSite].nTestBdStart = BD_END;
				}
				else
				{
					if (st_test_site_info[nSite].nTestBdStart	!= BD_BIN_CHK) return;

					tCurr	= COleDateTime::GetCurrentTime();
					tDiff	= tCurr - st_test_site_info[nSite].tStart;

					nTime = (int)tDiff.GetTotalSeconds();
					// jtkim 20160830
					st_test_site_info[nSite].dwBdTime = (DWORD)nTime;


					if (nTime > (st_recipe_info.nAbortTime * 2))
					{
						clsInterC4.OnTesterAbort(st_test_site_info[nSite]);
					}
				}
			}
			else
			{
				if (st_test_site_info[nSite].nTestBdStart	!= BD_BIN_CHK) return;

				// without device mode이면
				tCurr	= COleDateTime::GetCurrentTime();
				tDiff	= tCurr - st_test_site_info[nSite].tStart;

				nTime = (int)tDiff.GetTotalSeconds();
				// jtkim 20160830
				st_test_site_info[nSite].dwBdTime = (DWORD)nTime;

				if (nTime > st_recipe_info.nAbortTime)
				{
					// abort 시간이 지나면 
					//st_test_site_info[nSite].nTestBdStart = BD_END;
					for (i=0; i<8; i++)
					{
						if ((rand() % 10) == 0)
						{
							st_test_site_info[nSite].st_pcb_info[i].nRetestCnt++;
							if (st_test_site_info[nSite].st_pcb_info[i].nRetestCnt > st_recipe_info.nTestRetest_Count)
							{
								st_test_site_info[nSite].st_pcb_info[i].nBin = BD_DATA_REJECT;
							}
							else 
							{
								st_test_site_info[nSite].st_pcb_info[i].nBin = BD_DATA_RETEST;
							}
						}
						else if ((i % 2) == 0)
						{
							st_test_site_info[nSite].st_pcb_info[i].nBin = BD_DATA_RETEST;
						}
						else
						{
							st_test_site_info[nSite].st_pcb_info[i].nBin = BD_DATA_GOOD;
						}
						st_test_site_info[nSite].st_pcb_info[i].nTestBdStart = BD_END;
					}
					st_test_site_info[nSite].nTestBdStart = BD_END;
				}
			}
		}
		else
		{
			if (st_test_site_info[nSite].nTestBdStart	!= BD_BIN_CHK) return;
			// without device mode이면
			tCurr	= COleDateTime::GetCurrentTime();
			tDiff	= tCurr - st_test_site_info[nSite].tStart;

			nTime = (int)tDiff.GetTotalSeconds();
			// jtkim 20160830
			st_test_site_info[nSite].dwBdTime = (DWORD)nTime;

			if (nTime > st_recipe_info.nAbortTime)
			{
				// abort 시간이 지나면 
				for (i=0; i<8; i++)
				{
					if ((rand() % 10) == 0)
					{
						st_test_site_info[nSite].st_pcb_info[i].nRetestCnt++;
						if (st_test_site_info[nSite].st_pcb_info[i].nRetestCnt > st_recipe_info.nTestRetest_Count)
						{
							st_test_site_info[nSite].st_pcb_info[i].nBin = BD_DATA_REJECT;
						}
						else 
						{
							st_test_site_info[nSite].st_pcb_info[i].nBin = BD_DATA_RETEST;
						}
					}
					else if ((rand() % 50) == 1)
					{
						st_test_site_info[nSite].st_pcb_info[i].nBin = BD_DATA_RETEST;
					}
					else
					{
						st_test_site_info[nSite].st_pcb_info[i].nBin = BD_DATA_GOOD;
					}
					st_test_site_info[nSite].st_pcb_info[i].nTestBdStart = BD_END;
				}
				st_test_site_info[nSite].nTestBdStart = BD_END;
			}
		}
	}
}


void CRunTestSitePart4::Set_TestSite_UpDown( int nOnOff )
{
	CString strLogKey[10];
	CString	strLogData[10];

	strLogKey[0] = _T("Mode Start");
	strLogData[0].Format(_T("%d"),0);

	m_bTestSiteUpDownFlag = false;
	m_dwWaitTestSiteUpDown[0]	= GetCurrentTime();

	if(nOnOff == IO_OFF) //up
	{
		FAS_IO.set_out_bit( st_io_info.o_Tsite4DnCyl, IO_OFF);
		FAS_IO.set_out_bit( st_io_info.o_Tsite4UpCyl, IO_ON);
	}
	else //if(nOnOff == IO_ON)//down
	{
		FAS_IO.set_out_bit( st_io_info.o_Tsite4UpCyl, IO_OFF);
		FAS_IO.set_out_bit( st_io_info.o_Tsite4DnCyl, IO_ON);
	}

	if (nOnOff == IO_ON)
	{
		//LogFunction(_T("TEST_SITE"),_T("UP"),_T("TEST_SITE_4"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("TEST_SITE"),_T("UP"),0,_T("TEST_SITE_4"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
	else
	{
		//LogFunction(_T("TEST_SITE"),_T("DOWN"),_T("TEST_SITE_4"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("TEST_SITE"),_T("DOWN"),0,_T("TEST_SITE_4"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
}


int	CRunTestSitePart4::Chk_TestSite_UpDown( int nOnOff )
{
	CString strLogKey[10];
	CString	strLogData[10];

	strLogKey[0] = _T("Mode End");
	strLogData[0].Format(_T("%d"),0);

	int nWaitTime = WAIT_PUSHER_ON_OFF;

	if (nOnOff == IO_OFF)//UP
	{
		if (m_bTestSiteUpDownFlag == false &&
			FAS_IO.get_in_bit(st_io_info.i_Tsite4UpChk, IO_ON)	== IO_ON &&
			FAS_IO.get_in_bit(st_io_info.i_Tsite4DnChk, IO_OFF)	== IO_OFF)
		{
			m_bTestSiteUpDownFlag			= true;
			m_dwWaitTestSiteUpDown[0]	= GetCurrentTime();
		}
		else if (m_bTestSiteUpDownFlag == true &&
				 FAS_IO.get_in_bit(st_io_info.i_Tsite4UpChk, IO_ON)	== IO_ON &&
				 FAS_IO.get_in_bit(st_io_info.i_Tsite4DnChk, IO_OFF)	== IO_OFF)
		{
			m_dwWaitTestSiteUpDown[1]	= GetCurrentTime();
			m_dwWaitTestSiteUpDown[2]	= m_dwWaitTestSiteUpDown[1] - m_dwWaitTestSiteUpDown[0];

			if (m_dwWaitTestSiteUpDown[2] <= 0)
			{
				m_dwWaitTestSiteUpDown[0]	= GetCurrentTime();
				return RET_PROCEED;
			}

			if(m_dwWaitTestSiteUpDown[2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
			{
				//LogFunction(_T("TEST_SITE"),_T("UP"),_T("TEST_SITE_4"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("TEST_SITE"),_T("UP"),1,_T("TEST_SITE_4"),_T("CYLINDER"),1,strLogKey,strLogData); 
				return RET_GOOD;
			}
		}
		else
		{
			m_dwWaitTestSiteUpDown[1]	= GetCurrentTime();
			m_dwWaitTestSiteUpDown[2]	= m_dwWaitTestSiteUpDown[1] - m_dwWaitTestSiteUpDown[0];

			if (m_dwWaitTestSiteUpDown[2] <= 0)
			{
				m_dwWaitTestSiteUpDown[0]	= GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwWaitTestSiteUpDown[2] > (DWORD)st_wait_info.nLimitWaitTime[nWaitTime])
			{
				m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_Tsite4UpChk); 
				//2017.0109
				clsLog.LogFunction(_T("TEST_SITE"),_T("UP"),1,_T("TEST_SITE_4"),_T("CYLINDER"),1,strLogKey,strLogData); 
				return RET_ERROR;
			}
		}
		
	}
	else //if (nOnOff == IO_ON)//DOWN
	{
		if (m_bTestSiteUpDownFlag == false &&
			FAS_IO.get_in_bit(st_io_info.i_Tsite4UpChk, IO_OFF) == IO_OFF &&
			FAS_IO.get_in_bit(st_io_info.i_Tsite4DnChk, IO_ON)	== IO_ON)
		{
			m_bTestSiteUpDownFlag		= true;
			m_dwWaitTestSiteUpDown[0]	= GetCurrentTime();
		}
		else if (m_bTestSiteUpDownFlag == true &&
			FAS_IO.get_in_bit(st_io_info.i_Tsite4UpChk, IO_OFF)	== IO_OFF &&
			FAS_IO.get_in_bit(st_io_info.i_Tsite4DnChk, IO_ON)	== IO_ON)
		{
			m_dwWaitTestSiteUpDown[1] = GetCurrentTime();
			m_dwWaitTestSiteUpDown[2] = m_dwWaitTestSiteUpDown[1] - m_dwWaitTestSiteUpDown[0];

			if (m_dwWaitTestSiteUpDown[2] <= 0)
			{
				m_dwWaitTestSiteUpDown[0] = GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwWaitTestSiteUpDown[2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
			{
				//LogFunction(_T("TEST_SITE"),_T("DOWN"),_T("TEST_SITE_4"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("TEST_SITE"),_T("DOWN"),1,_T("TEST_SITE_4"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_GOOD;
			}
		}
		else
		{
			m_dwWaitTestSiteUpDown[1] = GetCurrentTime();
			m_dwWaitTestSiteUpDown[2] = m_dwWaitTestSiteUpDown[1] - m_dwWaitTestSiteUpDown[0];

			if (m_dwWaitTestSiteUpDown[2] <= 0)
			{
				m_dwWaitTestSiteUpDown[0] = GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwWaitTestSiteUpDown[2] > (DWORD)st_wait_info.nLimitWaitTime[nWaitTime])
			{
				m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_Tsite4DnChk); 
				//2017.0109
				clsLog.LogFunction(_T("TEST_SITE"),_T("DOWN"),1,_T("TEST_SITE_4"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_ERROR;
			}
		}

	}

	return RET_PROCEED;
}


void CRunTestSitePart4::OnRunDoor()
{
	int nRet_1, nRet_2;
	int nIO;

	CString strAlarmCode;

	nIO = FAS_IO.get_in_bit(st_io_info.i_TsiteDoorChk[3], IO_OFF);

	switch (m_nDoorStep)
	{
		case 0:
			// jtkim 20160926
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			if (m_nOpenDoor == YES)
			{
				m_nOpenDoor = NO;

				m_nDoorStep = 10000;
				break;
			}

			if (m_nCloseDoor == YES)
			{
				m_nCloseDoor = NO;

				m_nDoorStep = 20000;

				break;
			}
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			break;

		case 10000:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ChamberOpenChk[m_nIO_No],	IO_ON);  
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ChamberCloseChk[m_nIO_No],	IO_OFF);  

			if(nRet_1 == IO_ON && nRet_2 == IO_OFF )
			{
				// door open 상태이면 
				//clsInterC5.OnChamberDoor(st_test_site_info[m_nTHD_ID], 0);

				m_nDoorStep = 0;
			}
			else 
			{
				// door가 닫혀 있는 상태이면 .....
				m_nDoorStep = 10100;
			}
			break;

		case 10100:
			// door open 요청
			if (nIO == IO_ON)
			{
				st_test_site_info[m_nTHD_ID].nTester_Req[0] = CTL_REQ;
				st_test_site_info[m_nTHD_ID].nTester_Req[1] = WORK_PICK;

				m_nDoorStep = 10200;
			}
			else
			{
				m_dwDoorWaitTime[0] = GetTickCount();

				m_nDoorStep = 10150;
			}
			break;

		case 10150:
			if (nIO == IO_ON)
			{
				m_nDoorStep = 10100;
			}
			else
			{
				m_dwDoorWaitTime[1] = GetTickCount();
				m_dwDoorWaitTime[2] = m_dwDoorWaitTime[1] - m_dwDoorWaitTime[0];

				if (m_dwDoorWaitTime[2] <= (DWORD)0)
				{
					m_dwDoorWaitTime[0] = GetTickCount();
					break;
				}

				if (m_dwDoorWaitTime[2] > (DWORD) 60000)
				{
					strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_TsiteDoorChk[3]);
					CTL_Lib.Alarm_Error_Occurrence(9370, dWARNING, strAlarmCode);

					m_nDoorStep = 10100;
				}
			}
			break;

		case 10200:
			//kwlee 2016.1108
		//	if (st_test_site_info[m_nTHD_ID].nTester_Req[0] == CTL_READY)
			//{
				// door open이 완료 되었으면 
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ChamberOpenChk[m_nIO_No],	IO_ON);  
				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ChamberCloseChk[m_nIO_No],	IO_OFF);  

				if(nRet_1 == IO_ON && nRet_2 == IO_OFF )
				{
					// door 센서 체크
					//clsInterC1.OnChamberDoor(st_test_site_info[m_nTHD_ID], 0);

					m_nDoorStep = 0;
				}
				else
				{
					m_nDoorStep = 10100;
				}
			//}
			break;

		case 20000:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ChamberOpenChk[m_nIO_No],	IO_ON);  
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ChamberCloseChk[m_nIO_No],	IO_OFF);  

			if(nRet_1 == IO_OFF && nRet_2 == IO_ON )
			{
			//	clsInterC4.OnChamberDoor(st_test_site_info[m_nTHD_ID], 1);

				m_nDoorStep = 0;
			}
			else
			{
				m_nDoorStep = 20100;
			}
			break;

		case 20100:
			if (nIO == IO_ON)
			{
				// door close 요청
				st_test_site_info[m_nTHD_ID].nTester_Req[0] = CTL_REQ;
				st_test_site_info[m_nTHD_ID].nTester_Req[1] = WORK_PLACE;

				m_nDoorStep = 20200;
			}
			else
			{
				m_dwDoorWaitTime[0] = GetTickCount();

				m_nDoorStep = 20150;
			}
			break;

		case 20150:
			if (nIO == IO_ON)
			{
				m_nDoorStep = 20100;
			}
			else
			{
				m_dwDoorWaitTime[1] = GetTickCount();
				m_dwDoorWaitTime[2] = m_dwDoorWaitTime[1] - m_dwDoorWaitTime[0];

				if (m_dwDoorWaitTime[2] <= (DWORD)0)
				{
					m_dwDoorWaitTime[0] = GetTickCount();
					break;
				}

				if (m_dwDoorWaitTime[2] > (DWORD) 60000)
				{
					strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_TsiteDoorChk[3]);
					CTL_Lib.Alarm_Error_Occurrence(9380, dWARNING, strAlarmCode);

					m_nDoorStep = 20100;
				}
			}
			break;

		case 20200:
			//kwlee 2016.1108
			//if (st_test_site_info[m_nTHD_ID].nTester_Req[0] == CTL_READY)
			//{
				// door close 요청이 완료한 상태이면
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ChamberOpenChk[m_nIO_No],	IO_ON);  
				nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ChamberCloseChk[m_nIO_No],	IO_OFF);  

				if(nRet_1 == IO_OFF && nRet_2 == IO_ON )
				{
				//	clsInterC4.OnChamberDoor(st_test_site_info[m_nTHD_ID], 1);

					m_nDoorStep = 0;
				}
				else
				{
					m_nDoorStep = 20100;
				}
		//	}
			break;
	}
}