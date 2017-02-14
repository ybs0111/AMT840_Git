// ScreenInitial.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "ScreenInitial.h"
#include "Variable.h"
#include "CtlBd_Library.h"
#include "Ctlbd_Variable.h"
#include "PublicFunction.h"
#include "FastechPublic_IO.h"
#include "ComizoaPublic.h"
#include "Dialog_Message.h"
#include "LedButton.h"				// LED 버튼 생성 클래스 추가 
#include "Thread.h"
#include "XgemClient.h"
#include "AlgMemory.h"
#include "DialogMessageView.h"

// CScreenInitial

IMPLEMENT_DYNCREATE(CScreenInitial, CFormView)

CScreenInitial::CScreenInitial()
	: CFormView(CScreenInitial::IDD)
{
	m_nInitStep		= 0;
	m_nProcessStep	= 0;
	m_nProcessStep1 = 0;

	m_pMsgView = NULL;

	m_bmGround.LoadBitmap(IDB_BITMAP_BACK_GROUND);   
}

CScreenInitial::~CScreenInitial()
{
	if (m_pMsgView != NULL)
	{
		delete m_pMsgView;
		m_pMsgView = NULL;
	}
}

void CScreenInitial::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO_PG_01,			m_radPg[0]);
	DDX_Control(pDX, IDC_RADIO_PG_02,			m_radPg[1]);
	DDX_Control(pDX, IDC_RADIO_PG_03,			m_radPg[2]);
	DDX_Control(pDX, IDC_RADIO_PG_04,			m_radPg[3]);
	DDX_Control(pDX, IDC_RADIO_PG_05,			m_radPg[4]);
	DDX_Control(pDX, IDC_RADIO_PG_06,			m_radPg[5]);
	DDX_Control(pDX, IDC_RADIO_PG_07,			m_radPg[6]);
	DDX_Control(pDX, IDC_RADIO_PG_08,			m_radPg[7]);
	DDX_Control(pDX, IDC_RADIO_PG_09,			m_radPg[8]);
	DDX_Control(pDX, IDC_RADIO_PG_10,			m_radPg[9]);
	DDX_Control(pDX, IDC_RADIO_PG_11,			m_radPg[10]);
	DDX_Control(pDX, IDC_RADIO_PG_12,			m_radPg[11]);
	DDX_Control(pDX, IDC_RADIO_PG_13,			m_radPg[12]);
	DDX_Control(pDX, IDC_RADIO_PG_14,			m_radPg[13]);
	DDX_Control(pDX, IDC_RADIO_PG_15,			m_radPg[14]);
	DDX_Control(pDX, IDC_RADIO_PG_16,			m_radPg[15]);
	DDX_Control(pDX, IDC_RADIO_PG_17,			m_radPg[16]);
	DDX_Control(pDX, IDC_RADIO_PG_18,			m_radPg[17]);
	DDX_Control(pDX, IDC_RADIO_PG_19,			m_radPg[18]);
	DDX_Control(pDX, IDC_RADIO_PG_20,			m_radPg[19]);
	DDX_Control(pDX, IDC_RADIO_PG_21,			m_radPg[20]);
	DDX_Control(pDX, IDC_MSG_INITIAL,			m_msgInitial);
	DDX_Control(pDX, IDC_GIF_INITIALIZING,		m_pgifInitializing);
	DDX_Control(pDX, IDC_BTN_SKIP,				m_btnSkip);
}

BEGIN_MESSAGE_MAP(CScreenInitial, CFormView)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_SKIP,					&CScreenInitial::OnClickedBtnSkip)
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_WORK_COMMAND,					OnMainWorkInfoCommand)  // 출력용 사용자 정의 메시지
END_MESSAGE_MAP()


// CScreenInitial 진단입니다.

#ifdef _DEBUG
void CScreenInitial::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CScreenInitial::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void CScreenInitial::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	OnReadyInit();

	OnInitLabel();
	OnInitButton();
	OnInitProgress();
	OnInitRadio();

	OnInitChangeStatus(0);
	
	
	st_handler_info.cWndInitial = this;
}

void CScreenInitial::OnInitProgress()
{
	CSuperProgressCtrl::RegisterClass();

	m_ctrlProgress.Create(this, 250, 200, LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_INITIAL_STR_1)), 0);
	m_ctrlProgress.SetFillStyle(SP_FILL_HORZGRAD);
	m_ctrlProgress.SetColours(RGB(255,0,0), RGB(0,0,0));
	m_ctrlProgress.SetRange(0, 100);
	m_ctrlProgress.SetStep(1);

	m_ctrlProgress1.Create(this, 250, 350, LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_INITIAL_STR_2)), 0);
	m_ctrlProgress1.SetFillStyle(SP_FILL_HORZGRAD);
	m_ctrlProgress1.SetColours(RGB(0,0,255), RGB(0,0,0));
	m_ctrlProgress1.SetRange(0, 100);
	m_ctrlProgress1.SetStep(1);

}

// ********************************
// 초기화 작업 준비 
// ********************************
void CScreenInitial::OnReadyInit()
{
	clsFunc.OnDoorClose();

	if(st_handler_info.cWndTitle != NULL)
	{
		st_handler_info.cWndTitle->PostMessage(WM_STATUS_CHANGE, MACHINE_STATUS, st_handler_info.nRunStatus);
	}

	SetTimer(TMR_INIT, 250, NULL);  // 초기화 작업 타이머 생성
}

void CScreenInitial::OnInitLabel()     // 칼라 Label 생성
{
	if (m_pgifInitializing.Load(MAKEINTRESOURCE(IDR_GIF_INITIALIZING), _T("GIF")))
	{
		m_pgifInitializing.Draw();
	}

	m_msgInitial.SetFont(clsFunc.m_pFont[9]);
	m_msgInitial.SetWindowText(_T("initializing~!! Wait a moment~"));
	m_msgInitial.SetCenterText();
	m_msgInitial.SetColor(RGB(0,0,0));
	m_msgInitial.SetGradientColor(RGB(0,0,255));
	m_msgInitial.SetTextColor(RGB(255,255,255));
}


void CScreenInitial::OnInitButton()
{
	m_btnSkip.SetBitmaps(IDC_BTN_SKIP,  WINDOW_DN,  WINDOW_UP);
	m_btnSkip.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnSkip.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnSkip.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnSkip.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnSkip.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnSkip.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnSkip.SetFont(clsFunc.m_pFont[3]);
}

void CScreenInitial::OnInitRadio()
{
	LedState pLed;
	int i = 0;

	for (i=0; i<21; i++)
	{
		pLed = m_radPg[i].GetLedState();
		m_radPg[i].SetTextBackground(pLed, WINDOW_UP, TRUE);
		m_radPg[i].SetImage(IDB_REDBUTTON, 15);
	}
}


void CScreenInitial::OnDestroy()
{
	CFormView::OnDestroy();

	st_handler_info.cWndInitial = NULL;
}


void CScreenInitial::OnTimer(UINT_PTR nIDEvent)
{
	int nRunCHK;
	int i = 0, y=0, x=0;

	if (nIDEvent == TMR_INIT)
	{
		/*if (st_handler_info.nMenuLock != TRUE)	
		{
			st_handler_info.nMenuLock = TRUE;	// 메뉴 사용 못하도록 설정
		}*/
		
		nRunCHK = OnInitExcution();				// 시컨스 초기화 작업 진행
		if (nRunCHK == RET_GOOD)
		{
			KillTimer(TMR_INIT);  // INIT 수행 타이머 삭제
			
			if (st_handler_info.cWndList != NULL)	
			{
				//wsprintfA(st_other_info.cNormalMsg, "%S", _T("[INIT] EQuipment's Init Process END"));
				//clsFunc.OnStringToChar(_T("[INIT] EQuipment's Init Process END"), st_other_info.cNormalMsg);
				clsMem.OnNormalMessageWrite(_T("[INIT] EQuipment's Init Process END"));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
			}
			if (st_handler_info.nRunStatus != dSTOP)
			{
				clsFunc.OnMCStop();
			}
			if (st_handler_info.nMenuLock != FALSE)	
			{
				st_handler_info.nMenuLock = FALSE;  // 메뉴 사용 가능하도록 설정
			}

			st_handler_info.nInitialSuccess = YES;
			::PostMessage(st_handler_info.hWnd, WM_FORM_CHANGE, SCREEN_MAIN, 0);  // 메인 화면으로 전환 요청

		}
		else if (nRunCHK == RET_ERROR)
		{
			KillTimer(TMR_INIT);  // INIT 수행 타이머 삭제

			if (st_handler_info.cWndList != NULL)	
			{
				//wsprintfA(st_other_info.cAbnormalMsg, "%S", _T("[INIT] Equipment's Init Failure"));
				//clsFunc.OnStringToChar(_T("[INIT] Equipment's Init Failure"), st_other_info.cAbnormalMsg);
				clsMem.OnAbNormalMessagWrite(_T("[INIT] Equipment's Init Failure"));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
			}

			if (st_handler_info.nMenuLock != FALSE)	
			{
				st_handler_info.nMenuLock = FALSE;  // 메뉴 사용 가능하도록 설정
			}

			st_handler_info.nInitialSuccess = NO;
			::PostMessage(st_handler_info.hWnd, WM_FORM_CHANGE, SCREEN_MAIN, 0);  // 메인 화면으로 전환 요청
		}
	}

	CFormView::OnTimer(nIDEvent);
}

int CScreenInitial::OnInitExcution()
{
	int i = 0;
	int nRet[2];
	int nCount;

	CString strTemp;

	nRet[0] = RET_PROCEED;

	CDialog_Message msg_dlg;

	//if (st_handler_info.nRunStatus != dINIT)  return nRet[0];				// 현재 장비 상태가 RUN 아니면 리턴한다.

	switch (m_nInitStep) 
	{
		case 0:
			if(clsFunc.OnDoorOpenCheck() == RET_GOOD)
			{
				m_nProcessStep = 0;

//				OnRecoveryInit();

				m_nInitStep = 100;
			}
			else
			{
				st_handler_info.nRunStatus			= dWARNING;
				st_alarm_info.nAlarmAssignSection	= 6000;

				return RET_ERROR;
			}
			//2014.01.21 주석처리
			if (FAS_IO.get_in_bit(st_io_info.i_FrontSelectSwChk, IO_ON) == IO_OFF ||
				FAS_IO.get_in_bit(st_io_info.i_RearSelectSwChk, IO_ON)	== IO_OFF)
			{
				if (FAS_IO.get_in_bit(st_io_info.i_FrontSelectSwChk, IO_ON) == IO_OFF)
				{
					strTemp.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_FrontSelectSwChk);
					CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, strTemp);
				}
				else
				{
					strTemp.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_RearSelectSwChk);
					CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, strTemp);
				}

				return RET_ERROR;
			}

/*			if (FAS_IO.get_in_bit(st_io_info.i_AutoModeChk, IO_OFF) == IO_OFF)
			{
				if (FAS_IO.get_in_bit(st_io_info.i_FrontSelectSwChk, IO_ON) == IO_OFF)
				{
					strTemp.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_FrontSelectSwChk);
					CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, strTemp);
				}
				else
				{
					strTemp.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_RearSelectSwChk);
					CTL_Lib.Alarm_Error_Occurrence(6000, dWARNING, strTemp);
				}
			}*/
			break;

		case 50:
			if (FAS_IO.get_in_bit(st_io_info.i_FrontSelectSwChk, IO_ON) == IO_ON &&
				FAS_IO.get_in_bit(st_io_info.i_RearSelectSwChk, IO_ON)	== IO_ON )
			{
				m_nInitStep = 100;
			}
			else
			{
				m_nInitStep = 0;
			}
			break;

		case 100:
			if (st_lot_info[LOT_CURR].nLotStatus == YES)
			{
				OnRecoveryInit();

				st_lot_info[LOT_CURR].nLotStatus = NO;
				st_recovery_info.nMcRecoveryUsed = NO;
				st_recovery_info.nMcDataRecovery = NO;

				m_nInitStep = 300;
			}
			else
			{
				OnRecoveryInit();

				m_nInitStep = 300;
			}
			break;

		case 200:
			if (st_other_info.nConfirmMsg == NO)
			{
				OnRecoveryInit();
				st_recovery_info.nMcRecoveryUsed = NO;
				st_recovery_info.nMcDataRecovery = NO;

				m_nInitStep = 300;
			}
			else if (st_other_info.nConfirmMsg == YES)
			{
				// recovery data read......
				clsFunc.OnRecoveryReadWrite(0, 1);

				st_recovery_info.nMcRecoveryUsed = NO;
				st_recovery_info.nMcDataRecovery = YES;

				m_nInitStep = 300;
			}
			break;

		case 300:
			if (st_handler_info.nInitRetry == YES)
			{	
				if (FAS_IO.m_nIoBoardInitialFlag == NO || COMI.mn_motorbd_init_end == NO)  
				{
					m_nInitStep = 400;
				}
				else
				{
					OnInitChangeStatus(1);	// I/O Board 초기화 완료
					OnInitChangeStatus(2);	// motion board 초기화 완료

					m_nProcessStep += 8;

					m_nInitStep = 800;
				}
			}
			else
			{
				m_nInitStep = 400;
			}
			break;

		case 400:	
			if (FAS_IO.m_nIoBoardInitialFlag == NO)  
			{
				CTL_Lib.Alarm_Error_Occurrence(333, dWARNING, _T("900001"));

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnAbNormalMessagWrite(_T("I/O Board Initialized Fail..."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
				}

				m_nInitStep = 0;						// 초기화 진행 스텝 정보 초기화 
				return RET_ERROR;
			}
			else
			{
				OnInitChangeStatus(1);	// I/O Board 초기화 완료

				clsFunc.OnIOPortClear();
				
				m_nProcessStep += 4;

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnNormalMessageWrite(_T("I/O Board Initialized Success..."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
				}

				m_nInitStep = 500;
			}
			break;

		case 500:
			if (COMI.mn_motorbd_init_end == YES)
			{
				OnInitChangeStatus(2);	// I/O Board 초기화 완료

				m_nProcessStep += 8;
				
				m_nInitStep = 600;
			}
			else
			{
				if (st_work_info.nSimulationMode == 1)
				{
					OnInitChangeStatus(2);	// I/O Board 초기화 완료

					m_nInitStep = 600;
				}
				else
				{
					nRet[1] = CTL_Lib.Initialize_motor_board(1,  _T("c:\\AMT840\\Handler\\amt840.cme2"));			// MOTOR 보드 초기화 여부 검사한다
			
					if (nRet[1] == RET_ERROR)
					{
						st_alarm_info.strCode		= _T("900001");  //kwlee 2016.0902 "900000" ->"900001"
						st_alarm_info.nCountMode	= 0;
						st_alarm_info.nTypeMode		= eWARNING;
						st_handler_info.nRunStatus  = dWARNING;
						st_alarm_info.nAlarmAssignSection = 334;

						m_nInitStep		= 0;						// 초기화 진행 스텝 정보 초기화 

						if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
						{
							clsMem.OnAbNormalMessagWrite(_T("Motor Board Initialized Fail..."));
							//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
						}	

	 					return RET_ERROR;
					}
					else
					{
						/// 추가.......
						if (st_recovery_info.nMcDataRecovery == NO)
						{
							for (i = 0; i < MAXMOTOR; i++)
							{
								COMI.mn_homechk_flag[i] = NO;
							}
							m_nProcessStep += 8;

							if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
							{
								clsMem.OnNormalMessageWrite(_T("Motor Board Initialized Success..."));
								//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
							}

							OnInitChangeStatus(2);	// I/O Board 초기화 완료

							m_nInitStep = 600;
						}
						else
						{
							nCount = 0;
/*8							for (i=0; i<M_MAX_MOTOR_NUM; i++)
							{
								dCurrPos	= COMI.Get_MotCurrentPos(i);
								dData		= fabs(dCurrPos - st_work_info.dCurrPos[i]);

								if (dData > st_motor_info[i].d_allow)
								{
									COMI.mn_homechk_flag[i] = NO;
								}
								else
								{
									COMI.mn_homechk_flag[i] = YES;
								}
							}*/

							m_nProcessStep += 8;

							if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
							{
								clsMem.OnNormalMessageWrite(_T("Motor Board Initialized Success..."));
								//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
							}

							OnInitChangeStatus(2);	// I/O Board 초기화 완료

							m_nInitStep = 600;
						}
					}
				}
			}
			break;

		case 600:
			st_other_info.nConfirmMsg	= -1;
			
			st_handler_info.cWndInitial->PostMessage(WM_WORK_COMMAND, INIT_INITIALIZE_COMMAND, 0);
			m_nInitStep = 700;
			break;

		case 700:
			if (st_other_info.nConfirmMsg == YES)
			{
				if (st_handler_info.nMenuLock != FALSE)	
				{
					st_handler_info.nMenuLock = FALSE;
				}

				m_nInitStep = 800;
			}
			else if(st_other_info.nConfirmMsg == NO)
			{
				nRet[0]	= RET_ERROR;

				m_nInitStep = 0;
			}
			break;

		case 800:
			clsFunc.OnInitVariable();
			
			AllStop[1] = 1;
			// Loading tray Convaetyor Left/Right Shifter Thread
			while(WaitForSingleObject(m_thrHandle[5], 5000) != WAIT_OBJECT_0)
			{
				if (m_thrHandle[5] == NULL) break;
				Sleep(0);
			}
			// Load Tray Stacker Eelvator Thread
			while(WaitForSingleObject(m_thrHandle[6], 5000) != WAIT_OBJECT_0)
			{
				if (m_thrHandle[6] == NULL) break;
				Sleep(0);
			}
			// Load Tray Plate Thread
			while(WaitForSingleObject(m_thrHandle[7], 5000) != WAIT_OBJECT_0)
			{
				if (m_thrHandle[7] == NULL) break;
				Sleep(0);
			}
			//Empty Stacker Elevator Thread
			while(WaitForSingleObject(m_thrHandle[8], 5000) != WAIT_OBJECT_0)
			{
				if (m_thrHandle[8] == NULL) break;
				Sleep(0);
			}
			// Unload Tray #1(left)Elevator Thread
			while(WaitForSingleObject(m_thrHandle[9], 5000) != WAIT_OBJECT_0)
			{
				if (m_thrHandle[9] == NULL) break;
				Sleep(0);
			}
			 // Unload Tray #1(left)Coveyor Thread
			while(WaitForSingleObject(m_thrHandle[10], 5000) != WAIT_OBJECT_0)
			{
				if (m_thrHandle[10] == NULL) break;
				Sleep(0);
			}
			// Unload Tray #2(Right)Elevator Thread
			while(WaitForSingleObject(m_thrHandle[11], 5000) != WAIT_OBJECT_0)
			{
				if (m_thrHandle[11] == NULL) break;
				Sleep(0);
			}
			// Unload Tray #2(Right)Coveyor Thread
			while(WaitForSingleObject(m_thrHandle[12], 5000) != WAIT_OBJECT_0)
			{
				if (m_thrHandle[12] == NULL) break;
				Sleep(0);
			}
			//Empty Tray Transfer Thread
			while(WaitForSingleObject(m_thrHandle[13], 5000) != WAIT_OBJECT_0)
			{
				if (m_thrHandle[13] == NULL) break;
				Sleep(0);
			}
			//Empty Reject Tray Stacker Conveyor Thread
			while(WaitForSingleObject(m_thrHandle[14], 5000) != WAIT_OBJECT_0)
			{
				if (m_thrHandle[14] == NULL) break;
				Sleep(0);
			}
			//Empty Reject Tray Stacker Elevator Thread
			while(WaitForSingleObject(m_thrHandle[15], 5000) != WAIT_OBJECT_0)
			{
				if (m_thrHandle[15] == NULL) break;
				Sleep(0);
			}
			//Reject Tray#1(Front)Stacker Elevator Thread
			while(WaitForSingleObject(m_thrHandle[16], 5000) != WAIT_OBJECT_0)
			{
				if (m_thrHandle[16] == NULL) break;
				Sleep(0);
			}
			//Reject Tray#1(Front)Stacker Output Conveyor Thread
			while(WaitForSingleObject(m_thrHandle[17], 5000) != WAIT_OBJECT_0)
			{
				if (m_thrHandle[17] == NULL) break;
				Sleep(0);
			}
			//Reject Tray#2(Rear)Stacker Elevator Thread
			while(WaitForSingleObject(m_thrHandle[18], 5000) != WAIT_OBJECT_0)
			{
				if (m_thrHandle[18] == NULL) break;
				Sleep(0);
			}
			//Reject Tray#1(Rear)Stacker Output Conveyor Thread
			while(WaitForSingleObject(m_thrHandle[19], 5000) != WAIT_OBJECT_0)
			{
				if (m_thrHandle[19] == NULL) break;
				Sleep(0);
			}
			//Reject Tray Transfer Thread
			while(WaitForSingleObject(m_thrHandle[20], 5000) != WAIT_OBJECT_0)
			{
				if (m_thrHandle[20] == NULL) break;
				Sleep(0);
			}
			// Work Robot Thread
			while(WaitForSingleObject(m_thrHandle[21], 5000) != WAIT_OBJECT_0)
			{
				if (m_thrHandle[21] == NULL) break;
				Sleep(0);
			}
			// Load Buffer Thread
			while(WaitForSingleObject(m_thrHandle[22], 5000) != WAIT_OBJECT_0)
			{
				if (m_thrHandle[22] == NULL) break;
				Sleep(0);
			}
			// Unload Buffer Thread
			while(WaitForSingleObject(m_thrHandle[23], 5000) != WAIT_OBJECT_0)
			{
				if (m_thrHandle[23] == NULL) break;
				Sleep(0);
			}
			// Test Site Robot Thread
			while(WaitForSingleObject(m_thrHandle[24], 5000) != WAIT_OBJECT_0)
			{
				if (m_thrHandle[24] == NULL) break;
				Sleep(0);
			}
			// Test Site Robot Thread
			while(WaitForSingleObject(m_thrHandle[25], 5000) != WAIT_OBJECT_0)
			{
				if (m_thrHandle[25] == NULL) break;
				Sleep(0);
			}
			// Test Site Robot Thread
			while(WaitForSingleObject(m_thrHandle[26], 5000) != WAIT_OBJECT_0)
			{
				if (m_thrHandle[26] == NULL) break;
				Sleep(0);
			}
			// Test Site Robot Thread
			while(WaitForSingleObject(m_thrHandle[27], 5000) != WAIT_OBJECT_0)
			{
				if (m_thrHandle[27] == NULL) break;
				Sleep(0);
			}
			// jtkim 20150330 recovery동작
			if (st_recovery_info.nMcDataRecovery == NO)
			{
				st_handler_info.nRunStatus = dINIT;
			}
			clsFunc.OnDoorClose();
			
			if (st_handler_info.cWndTitle != NULL)	
			{
 				st_handler_info.cWndTitle->PostMessage(WM_STATUS_CHANGE, MACHINE_STATUS, st_handler_info.nRunStatus);
 			}
			m_nInitStep = 900;
			break;

		case 900:
			AllStop[1] = 0;

			//kwlee 2016.0822
			m_thread[5] = AfxBeginThread(OnThreadLdTrayInputShifterConveyor, this);  
			if (m_thread[5] != NULL)	
			{
				m_thrHandle[5] = m_thread[5]->m_hThread;
			}
			m_thread[6] = AfxBeginThread(OnThreadLdTrayStackerElevator, this); 
			if (m_thread[6] != NULL)	
			{
				m_thrHandle[6] = m_thread[6]->m_hThread;
			}

			m_thread[7] = AfxBeginThread(OnThreadLdTrayPlate, this);  
			if (m_thread[7] != NULL)	
			{
				m_thrHandle[7] = m_thread[7]->m_hThread;
			}

			m_thread[8] = AfxBeginThread(OnThreadEmptyStackerElevator, this);  
			if (m_thread[8] != NULL)	
			{
				m_thrHandle[8] = m_thread[8]->m_hThread;
			}

			m_thread[9] = AfxBeginThread(OnThreadUldGoodTrayLeftOutputElevator, this);  
			if (m_thread[9] != NULL)	
			{
				m_thrHandle[9] = m_thread[9]->m_hThread;
			}

			m_thread[10] = AfxBeginThread(OnThreadUldGoodTrayLeftOutputConveyor, this);  
			if (m_thread[10] != NULL)	
			{
				m_thrHandle[10] = m_thread[10]->m_hThread;
			}

			m_thread[11] = AfxBeginThread(OnThreadUldGoodTrayRightOutputElevator, this);  
			if (m_thread[11] != NULL)	
			{
				m_thrHandle[11] = m_thread[11]->m_hThread;
			}

			m_thread[12] = AfxBeginThread(OnThreadUldGoodTrayRightOutputConveyor, this);  
			if (m_thread[12] != NULL)	
			{
				m_thrHandle[12] = m_thread[12]->m_hThread;
			}

			m_thread[13] = AfxBeginThread(OnThreadEmptyTrayTransfer, this);  
			if (m_thread[13] != NULL)	
			{
				m_thrHandle[13] = m_thread[13]->m_hThread;
			}

			m_thread[14] = AfxBeginThread(OnThreadRejectTrayInputConveyor, this);  
			if (m_thread[14] != NULL)	
			{
				m_thrHandle[14] = m_thread[14]->m_hThread;
			}

			m_thread[15] = AfxBeginThread(OnThreadRejectTrayStackerElevator, this);  
			if (m_thread[15] != NULL)	
			{
				m_thrHandle[15] = m_thread[15]->m_hThread;
			}

			m_thread[16] = AfxBeginThread(OnThreadRejectTrayFrontStackerElevator, this);  
			if (m_thread[16] != NULL)	
			{
				m_thrHandle[16] = m_thread[16]->m_hThread;
			}

			m_thread[17] = AfxBeginThread(OnThreadRejectTrayFrontOutputConveyor, this);  
			if (m_thread[17] != NULL)	
			{
				m_thrHandle[17] = m_thread[17]->m_hThread;
			}
			m_thread[18] = AfxBeginThread(OnThreadRejectTrayRearStackerElevator, this);  
			if (m_thread[18] != NULL)	
			{
				m_thrHandle[18] = m_thread[18]->m_hThread;
			}
			m_thread[19] = AfxBeginThread(OnThreadRejectTrayRearOutputConveyor, this);  
			if (m_thread[19] != NULL)	
			{
				m_thrHandle[19] = m_thread[19]->m_hThread;
			}
			m_thread[20] = AfxBeginThread(OnThreadRejectTrayTransfer, this);  
			if (m_thread[20] != NULL)	
			{
				m_thrHandle[20] = m_thread[20]->m_hThread;
			}
			m_thread[21] = AfxBeginThread(OnThreadLdUldRobot, this);  
			if (m_thread[21] != NULL)	
			{
				m_thrHandle[21] = m_thread[21]->m_hThread;
			}
			m_thread[22] = AfxBeginThread(OnThreadDvcLdBuffer, this);  
			if (m_thread[22] != NULL)	
			{
				m_thrHandle[22] = m_thread[22]->m_hThread;
			}
			m_thread[23] = AfxBeginThread(OnThreadDvcUldBuffer, this);  
			if (m_thread[23] != NULL)	
			{
				m_thrHandle[23] = m_thread[23]->m_hThread;
			}
			m_thread[24] = AfxBeginThread(OnThreadTestSiteRobot, this);  
			if (m_thread[24] != NULL)	
			{
				m_thrHandle[24] = m_thread[24]->m_hThread;
			}
			////////
			m_thread[25] = AfxBeginThread(OnThreadTestSiteHead_8, this);
			if (m_thread[25] != NULL) 
			{
				m_thrHandle[25] = m_thread[25]->m_hThread;
			}
			else 
			{
				AfxMessageBox(_T("Cannot begin the thread [threadTestSiteHead_8]"));
			}

			m_thread[26] = AfxBeginThread(OnThreadTestSiteRetestBuffer_Left, this);
			if (m_thread[26] != NULL) 
			{
				m_thrHandle[26] = m_thread[26]->m_hThread;
			}
			else 
			{
				AfxMessageBox(_T("Cannot begin the thread [threadTestSiteRetestBuffer_Left]"));
			}

			m_thread[27] = AfxBeginThread(OnThreadTestSiteRetestBuffer_Right, this);
			if (m_thread[27] != NULL) 
			{
				m_thrHandle[27] = m_thread[27]->m_hThread;
			}
			else 
			{
				AfxMessageBox(_T("Cannot begin the thread [threadTestSiteRetestBuffer_Right]"));
			}

			if (st_recovery_info.nMcDataRecovery == YES)
			{
				st_sync_info.nInit_Flag[THD_LD_CV_STACKER_LEFT]		= INIT_READY;
				st_sync_info.nInit_Flag[THD_LD_CV_STACKER_RIGHT]	= INIT_READY;
				st_sync_info.nInit_Flag[THD_LD_STACKER]				= INIT_READY;
				st_sync_info.nInit_Flag[THD_LD_TRAY_PLATE]			= INIT_READY;
				st_sync_info.nInit_Flag[THD_EMPTY_STACKER]			= INIT_READY;
				st_sync_info.nInit_Flag[THD_ULD_1_STACKER]			= INIT_READY;
				st_sync_info.nInit_Flag[THD_ULD_2_STACKER]			= INIT_READY;
				st_sync_info.nInit_Flag[THD_REJECT_EMPTY_STACKER]	= INIT_READY;
				st_sync_info.nInit_Flag[THD_REJECT_OUT_1_STACKER]	= INIT_READY;
				st_sync_info.nInit_Flag[THD_REJECT_OUT_2_STACKER]	= INIT_READY;
				st_sync_info.nInit_Flag[THD_WORK_RBT]				= INIT_READY;
				st_sync_info.nInit_Flag[THD_TEST_RBT]				= INIT_READY;
				st_sync_info.nInit_Flag[THD_LD_BUFF]				= INIT_READY;
				st_sync_info.nInit_Flag[THD_ULD_BUFF]				= INIT_READY;
				st_sync_info.nInit_Flag[THD_WORK_TRANSFER]			= INIT_READY;
				st_sync_info.nInit_Flag[THD_REJECT_TRANSFER]		= INIT_READY;
				//kwlee 2016.1003
				st_sync_info.nInit_Flag[THD_RETEST_1_BUFF]          = INIT_READY;
				st_sync_info.nInit_Flag[THD_RETEST_2_BUFF]          = INIT_READY;
			}
			m_nInitStep = 1000;
			break;

		case 1000:
			//kwlee 2016.0822
			if (st_sync_info.nInit_Flag[THD_LD_CV_STACKER_LEFT] == INIT_READY && st_sync_info.nInit_Flag[THD_LD_CV_STACKER_RIGHT] == INIT_READY)
			{
				m_nProcessStep					+= 5;

				OnInitChangeStatus(3);
				st_sync_info.nInit_Flag[THD_LD_CV_STACKER_LEFT]		= INIT_COMPLETE;
				st_sync_info.nInit_Flag[THD_LD_CV_STACKER_RIGHT]	= INIT_COMPLETE;
			}

			if (st_sync_info.nInit_Flag[THD_LD_STACKER] == INIT_READY)
			{
				m_nProcessStep					+= 5;

				OnInitChangeStatus(4);
				st_sync_info.nInit_Flag[THD_LD_STACKER]		= INIT_COMPLETE;
			}

			if (st_sync_info.nInit_Flag[THD_LD_TRAY_PLATE] == INIT_READY)
			{
				m_nProcessStep					+= 5;

				OnInitChangeStatus(5);
				st_sync_info.nInit_Flag[THD_LD_TRAY_PLATE]		= INIT_COMPLETE;
			}

			if (st_sync_info.nInit_Flag[THD_EMPTY_STACKER] == INIT_READY)
			{
				m_nProcessStep					+= 5;

				OnInitChangeStatus(6);
				st_sync_info.nInit_Flag[THD_EMPTY_STACKER]		= INIT_COMPLETE;
			}

			if (st_sync_info.nInit_Flag[THD_ULD_1_STACKER] == INIT_READY)
			{
				m_nProcessStep					+= 5;

				OnInitChangeStatus(7);
				st_sync_info.nInit_Flag[THD_ULD_1_STACKER]		= INIT_COMPLETE;
			}

			if (st_sync_info.nInit_Flag[THD_ULD_2_STACKER] == INIT_READY)
			{
				m_nProcessStep					+= 5;

				OnInitChangeStatus(8);
				st_sync_info.nInit_Flag[THD_ULD_2_STACKER]		= INIT_COMPLETE;
			}

			if (st_sync_info.nInit_Flag[THD_REJECT_EMPTY_STACKER] == INIT_READY)
			{
				m_nProcessStep					+= 4;

				OnInitChangeStatus(9);
				st_sync_info.nInit_Flag[THD_REJECT_EMPTY_STACKER]	= INIT_COMPLETE;
			}

			if (st_sync_info.nInit_Flag[THD_REJECT_OUT_1_STACKER] == INIT_READY)
			{
				m_nProcessStep					+= 4;

				OnInitChangeStatus(10);
				st_sync_info.nInit_Flag[THD_REJECT_OUT_1_STACKER]	= INIT_COMPLETE;
			}

			if (st_sync_info.nInit_Flag[THD_REJECT_OUT_2_STACKER] == INIT_READY)
			{
				m_nProcessStep					+= 5;

				OnInitChangeStatus(11);
				st_sync_info.nInit_Flag[THD_REJECT_OUT_2_STACKER]	= INIT_COMPLETE;
			}

			if (st_sync_info.nInit_Flag[THD_WORK_RBT] == INIT_READY)
			{
				m_nProcessStep					+= 5;

				OnInitChangeStatus(12);
				st_sync_info.nInit_Flag[THD_WORK_RBT]		= INIT_COMPLETE;
			}

			if (st_sync_info.nInit_Flag[THD_TEST_RBT] == INIT_READY)
			{
				m_nProcessStep					+= 5;

				OnInitChangeStatus(13);
				st_sync_info.nInit_Flag[THD_TEST_RBT]		= INIT_COMPLETE;
			}
			
			if (st_sync_info.nInit_Flag[THD_LD_BUFF] == INIT_READY)
			{
				m_nProcessStep					+= 4;

				OnInitChangeStatus(14);
				st_sync_info.nInit_Flag[THD_LD_BUFF]		= INIT_COMPLETE;
			}

			if (st_sync_info.nInit_Flag[THD_ULD_BUFF] == INIT_READY)
			{
				m_nProcessStep					+= 4;

				OnInitChangeStatus(15);
				st_sync_info.nInit_Flag[THD_ULD_BUFF]		= INIT_COMPLETE;
			}

			if (st_sync_info.nInit_Flag[THD_WORK_TRANSFER] == INIT_READY)
			{
				m_nProcessStep					+= 4;

				OnInitChangeStatus(16);
				st_sync_info.nInit_Flag[THD_WORK_TRANSFER]		= INIT_COMPLETE;
			}

			if (st_sync_info.nInit_Flag[THD_REJECT_TRANSFER] == INIT_READY)
			{
				m_nProcessStep					+= 4;

				OnInitChangeStatus(17);
				st_sync_info.nInit_Flag[THD_REJECT_TRANSFER]		= INIT_COMPLETE;
			}
			////////////
			if (st_sync_info.nInit_Flag[THD_TESTSITE_1] == INIT_READY)
			{
				st_sync_info.nInit_Flag[THD_TESTSITE_1] = INIT_COMPLETE;
			}

			if (st_sync_info.nInit_Flag[THD_TESTSITE_2] == INIT_READY)
			{
				st_sync_info.nInit_Flag[THD_TESTSITE_2] = INIT_COMPLETE;
			}

			if (st_sync_info.nInit_Flag[THD_TESTSITE_3] == INIT_READY)
			{
				st_sync_info.nInit_Flag[THD_TESTSITE_3] = INIT_COMPLETE;
			}

			if (st_sync_info.nInit_Flag[THD_TESTSITE_4] == INIT_READY)
			{
				st_sync_info.nInit_Flag[THD_TESTSITE_4] = INIT_COMPLETE;
			}

			if (st_sync_info.nInit_Flag[THD_TESTSITE_5] == INIT_READY)
			{
				st_sync_info.nInit_Flag[THD_TESTSITE_5] = INIT_COMPLETE;
			}

			if (st_sync_info.nInit_Flag[THD_TESTSITE_6] == INIT_READY)
			{
				st_sync_info.nInit_Flag[THD_TESTSITE_6] = INIT_COMPLETE;
			}

			if (st_sync_info.nInit_Flag[THD_TESTSITE_7] == INIT_READY)
			{
				st_sync_info.nInit_Flag[THD_TESTSITE_7] = INIT_COMPLETE;
			}

			if (st_sync_info.nInit_Flag[THD_TESTSITE_8] == INIT_READY)
			{
				st_sync_info.nInit_Flag[THD_TESTSITE_8] = INIT_COMPLETE;
			}

			if (st_sync_info.nInit_Flag[THD_RETEST_1_BUFF] == INIT_READY)
			{
				st_sync_info.nInit_Flag[THD_RETEST_1_BUFF] = INIT_COMPLETE;
			}

			if (st_sync_info.nInit_Flag[THD_RETEST_2_BUFF] == INIT_READY)
			{
				st_sync_info.nInit_Flag[THD_RETEST_2_BUFF] = INIT_COMPLETE;
			}
			//kwlee 2016.0822
// 			if (st_sync_info.nInitUnldTrayCvy == INIT_READY)
// 			{
// 				m_nProcessStep					+= 4;
// 
// 				OnInitChangeStatus(18);
// 				st_sync_info.nInitUnldTrayCvy		= INIT_COMPLETE;
// 			}
// 
// 			if (st_sync_info.nInitUnldTrayPlate == INIT_READY)
// 			{
// 				m_nProcessStep					+= 4;
// 
// 				OnInitChangeStatus(19);
// 				st_sync_info.nInitUnldTrayPlate		= INIT_COMPLETE;
// 			}
// 
// 			if (st_sync_info.nInitUnldTrayShifter == INIT_READY)
// 			{
// 				m_nProcessStep					+= 4;
// 
// 				OnInitChangeStatus(20);
// 				st_sync_info.nInitUnldTrayShifter		= INIT_COMPLETE;
// 			}
// 
// 			if (st_sync_info.nInitUnldTrayStk == INIT_READY)
// 			{
// 				m_nProcessStep					+= 4;
// 
// 				OnInitChangeStatus(21);
// 				st_sync_info.nInitUnldTrayStk		= INIT_COMPLETE;
// 			}
/*
			if (st_sync_info.nInitBcrRbt			== INIT_COMPLETE && st_sync_info.nInitLdTrayCvy			== INIT_COMPLETE
			&&	st_sync_info.nInitLdTrayPlate		== INIT_COMPLETE && st_sync_info.nInitLdTrayShifter		== INIT_COMPLETE
			&&	st_sync_info.nInitLdRbt				== INIT_COMPLETE && st_sync_info.nInitLdBuff			== INIT_COMPLETE
			&&	st_sync_info.nInitEmptyStk			== INIT_COMPLETE && st_sync_info.nInitCokRbt			== INIT_COMPLETE
			&&	st_sync_info.nInitRetBuff			== INIT_COMPLETE && st_sync_info.nInitRejBuff			== INIT_COMPLETE
			&&	st_sync_info.nInitUnldTrayPlate		== INIT_COMPLETE && st_sync_info.nInitUnldTrayShifter	== INIT_COMPLETE
			&&	st_sync_info.nInitUnldRbt			== INIT_COMPLETE && st_sync_info.nInitUnldBuff			== INIT_COMPLETE
			&&	st_sync_info.nInitUnldTrayStk		== INIT_COMPLETE && st_sync_info.nInitTSiteRbt			== INIT_COMPLETE
			&& st_sync_info.nInitLfMP				== INIT_COMPLETE &&	st_sync_info.nInitRiMP				== INIT_COMPLETE 
			&&	st_sync_info.nInitUnldTrayCvy		== INIT_COMPLETE)
			{

				m_nRetry	= 0;
				m_nMcStep	= 0;

				m_nInitStep = 1100;
			}*/
// 			st_sync_info.nInit_Flag[THD_LD_BUFF]				= INIT_COMPLETE;
// 			st_sync_info.nInit_Flag[THD_ULD_BUFF]				 = INIT_COMPLETE;
			if (st_sync_info.nInit_Flag[THD_LD_CV_STACKER_LEFT]		== INIT_COMPLETE && st_sync_info.nInit_Flag[THD_LD_CV_STACKER_RIGHT]	 == INIT_COMPLETE
			&&	st_sync_info.nInit_Flag[THD_LD_STACKER]				== INIT_COMPLETE && st_sync_info.nInit_Flag[THD_LD_TRAY_PLATE]			 == INIT_COMPLETE
			&&	st_sync_info.nInit_Flag[THD_EMPTY_STACKER]			== INIT_COMPLETE && st_sync_info.nInit_Flag[THD_ULD_1_STACKER]			 == INIT_COMPLETE
			&&	st_sync_info.nInit_Flag[THD_ULD_2_STACKER]			== INIT_COMPLETE && st_sync_info.nInit_Flag[THD_REJECT_EMPTY_STACKER]	 == INIT_COMPLETE
			&& st_sync_info.nInit_Flag[THD_REJECT_OUT_1_STACKER]	== INIT_COMPLETE &&	st_sync_info.nInit_Flag[THD_REJECT_OUT_2_STACKER]    == INIT_COMPLETE
			&& st_sync_info.nInit_Flag[THD_WORK_RBT]				== INIT_COMPLETE &&	st_sync_info.nInit_Flag[THD_TEST_RBT]				 == INIT_COMPLETE
			&& st_sync_info.nInit_Flag[THD_LD_BUFF]					== INIT_COMPLETE &&	st_sync_info.nInit_Flag[THD_ULD_BUFF]				 == INIT_COMPLETE
			&& st_sync_info.nInit_Flag[THD_WORK_TRANSFER]			== INIT_COMPLETE && st_sync_info.nInit_Flag[THD_REJECT_TRANSFER]		 == INIT_COMPLETE 
			&& st_sync_info.nInit_Flag[THD_TESTSITE_1]				== INIT_COMPLETE && st_sync_info.nInit_Flag[THD_TESTSITE_2]				 == INIT_COMPLETE 
			&& st_sync_info.nInit_Flag[THD_TESTSITE_3]				== INIT_COMPLETE && st_sync_info.nInit_Flag[THD_TESTSITE_4]				 == INIT_COMPLETE 
			&& st_sync_info.nInit_Flag[THD_TESTSITE_5]				== INIT_COMPLETE && st_sync_info.nInit_Flag[THD_TESTSITE_6]				 == INIT_COMPLETE 
			&& st_sync_info.nInit_Flag[THD_TESTSITE_7]				== INIT_COMPLETE && st_sync_info.nInit_Flag[THD_TESTSITE_8]				 == INIT_COMPLETE 
			&& st_sync_info.nInit_Flag[THD_RETEST_1_BUFF]			== INIT_COMPLETE && st_sync_info.nInit_Flag[THD_RETEST_2_BUFF]			 == INIT_COMPLETE)			
			{
				m_nRetry	= 0;
				m_nMcStep	= 0;

				m_nInitStep = 5000;
			}
			break;

		case 1100:
			nRet[0] = OnMcEnable();

			if (nRet[0] == RET_GOOD)
			{
				m_nInitStep = 5000;
			}
			else if (nRet[0] == RET_ERROR)
			{
				return RET_ERROR;
			}
			break;

		case 5000:
			m_nInitStep = 0; 
			st_handler_info.nInitialSuccess = YES;
			nRet[0]		= RET_GOOD;
			break;
	}

	if (m_nProcessStep < 50)
	{
		m_ctrlProgress.SetPos(m_nProcessStep);
	}
	else if (m_nProcessStep > 50)
	{
		if (m_nProcessStep > 100)	m_nProcessStep = 100;

		m_ctrlProgress.SetPos(100);
		m_ctrlProgress1.SetPos(m_nProcessStep);
	}

	return nRet[0];
}


void CScreenInitial::OnInitChangeStatus(int nNum)
{
	int i = 0;

	if (nNum == 0)
	{
		for (i=0; i<21; i++)
		{
			m_radPg[i].Depress(FALSE);
		}
	}
	else if (nNum >= 1 && nNum <= 21)
	{
		m_radPg[nNum-1].Depress(TRUE);
	}


	/*switch(nNum)
	{
		case 0:
			for (i=0; i<21; i++)
			{
				m_radPg[i].Depress(FALSE);
			}
			
			break;

		case 1:
			m_radPg01.Depress(TRUE);
			break;

		case 2:
			m_radPg02.Depress(TRUE);
			break;

		case 3:
			m_radPg03.Depress(TRUE);
			break;

		case 4:
			m_radPg04.Depress(TRUE);
			break;

		case 5:
			m_radPg05.Depress(TRUE);
			break;

		case 6:
			m_radPg06.Depress(TRUE);
			break;

		case 7:
			m_radPg07.Depress(TRUE);
			break;

		case 8:
			m_radPg08.Depress(TRUE);
			break;

		case 9:
			m_radPg09.Depress(TRUE);
			break;

		case 10:
			m_radPg10.Depress(TRUE);
			break;

		case 11:
			m_radPg11.Depress(TRUE);
			break;

		case 12:
			m_radPg12.Depress(TRUE);
			break;
	}*/
}


BOOL CScreenInitial::DestroyWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
//	st_handler_info.cWndInitial = NULL;

	return CFormView::DestroyWindow();
}


void CScreenInitial::OnClickedBtnSkip()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다
	KillTimer(TMR_INIT) ;  // 초기화 진행 타이머 삭제 
	// **************************************************************************
	// 화면 전환 가능하도록 플래그 설정한다                                      
	// -> st_handler.n_menu_lock : 메뉴 사용 가능 상태 플래그                    
	// -> st_handler.n_initial   : 초기화 작업 완료 플래그                       
	// **************************************************************************
	if (st_handler_info.nInitialSuccess == YES)
	{
		st_handler_info.nInitialSuccess = NO;
	}
	
	if (st_handler_info.nMenuLock != FALSE)
	{
		st_handler_info.nMenuLock = FALSE;
	}
	// **************************************************************************
	
	if (st_handler_info.nRunStatus != dSTOP)  // STOP 상태 
	{
		clsFunc.OnMCStop();		// 장비 STOP 시 플래그 설정 및 I/O 출력 함수
	}
}


BOOL CScreenInitial::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CDC  memDC;                     // 메모리 DC
	CBitmap * pOldBitmap = NULL;    // m_BackBitmap으로 새로운 그림을 DC에 그릴 때, 이전 그려졌던 DC(즉, Bitmap)을 저장.
	BITMAP bitmapInfo;              // 그림의 정보(m_BackBitmap)
	m_bmGround.GetBitmap(&bitmapInfo); // Bitmap 크기 구함.

	memDC.CreateCompatibleDC(pDC);  // 메모리 DC 생성
	pOldBitmap = memDC.SelectObject(&m_bmGround);
	pDC->BitBlt(0, 0, bitmapInfo.bmWidth, bitmapInfo.bmHeight, &memDC, 0, 0, SRCCOPY);   // 메모리 DC에 그림을 그림

	memDC.SelectObject(pOldBitmap);
	memDC.DeleteDC();

//	return CFormView::OnEraseBkgnd(pDC);
	return true;
}

LRESULT CScreenInitial::OnMainWorkInfoCommand(WPARAM wParam, LPARAM lParam) 
{
	CDialog_Message dlgMsg;

	switch (wParam)
	{
		case INIT_RECOVERY_COMMAND:
			dlgMsg.m_strMessage			= _T("Do you want a recovery Machine Data ?");
			dlgMsg.m_nMessageType		= 1;
	
			st_other_info.nConfirmMsg	= -1;

			dlgMsg.DoModal();
			break;

		case INIT_INITIALIZE_COMMAND:
			dlgMsg.m_strMessage			= _T("Do you want a Machine Initialize?");
			dlgMsg.m_nMessageType		= 1;
	
			st_other_info.nConfirmMsg	= -1;

			dlgMsg.DoModal();
			break;
	}
	return 0;
}


void CScreenInitial::OnRecoveryInit()
{

}

void CScreenInitial::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	//	st_handler_info.cWndInitial = NULL;
	CFormView::PostNcDestroy();
}


int CScreenInitial::OnMcEnable()
{
	switch	(m_nMcStep)
	{
		case 0:
			// 작업중인 모드체크/
			if (st_basic_info.nModeInterface == EQP_ON_LINE)
			{
				// 작업모드가 on line 모드이면
				m_dwWaitTime[0] = GetTickCount();

				m_nMcStep = 1000;
			}
			else
			{
				// off line 모드이면 
				m_nMcStep = 0;

				return RET_GOOD;
			}
			break;

		case 1000:
			// datacenter client 현재 상태 체크
			if (st_client_info[EC_FIRST_NETWORK].nConnect == NO)
			{
				// 클라이언트 대기중이면 
				m_nRetry = 0;

				m_nMcStep = 1100;
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

				if (m_dwWaitTime[2] > 10000)
				{
					// 5초동안 대기해서 강제 종료
					m_dwWaitTime[1] = GetTickCount();
					//::PostMessage(st_handler_info.hWnd, WM_CLIENT_MSG + EC_FIRST_NETWORK, CLIENT_CLOSE, 0);
					::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + EC_FIRST_NETWORK, CLIENT_CLOSE, 0);
				}
			}
			break;

		case 1100:
			// client 연결 요청 
			m_dwWaitTime[0] = GetTickCount();
			//::PostMessage(st_handler_info.hWnd, WM_CLIENT_MSG + EC_FIRST_NETWORK, CLIENT_CONNECT, 0);
			::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + EC_FIRST_NETWORK, CLIENT_CONNECT, 0);

			m_nMcStep = 1200;
			break;

		case 1200:
			// client 연결상태 체크
			if (st_client_info[EC_FIRST_NETWORK].nConnect == YES)
			{
				// 현재 연결중이면 데이터 전송으로 
				m_nMcStep = 1300;
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

				if (m_dwWaitTime[2] > 5000)
				{
					m_nRetry++;
					if (m_nRetry < st_basic_info.nRetry)
					{
						m_nMcStep = 1100;
					}
					else
					{
						CTL_Lib.Alarm_Error_Occurrence(515, dWARNING, _T("550001"));

						m_nMcStep = 0;

						return RET_ERROR;
					}
				}
			}
			break;

		case 1300:
			m_dwWaitTime[0]								= GetTickCount();

//			clsDataClient.m_nMcRecipeReq				= BIN_NOTUESD;
//			st_client_info[EC_FIRST_NETWORK].strSend	= clsDataClient.OnMcRecipe(st_basic_info.strToolName);

			//::PostMessage(st_handler_info.hWnd, WM_CLIENT_MSG + EC_FIRST_NETWORK, CLIENT_SEND, 0);
//			::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + EC_FIRST_NETWORK, CLIENT_SEND, 0);

			m_nMcStep = 1400;
			break;

		case 1400:
			// 데이터 송신 결과 확인 작업
/*			if (clsDataClient.m_nMcRecipeReq == BIN_GOODBIN)
			{
				m_nMcStep = 0;

				return RET_GOOD;
			}
			else if (clsDataClient.m_nMcRecipeReq == BIN_REJECTBIN)
			{
				m_nRetry++;
				if (m_nRetry < st_basic_info.nRetry)
				{
					m_nMcStep = 1000;
				}
				else
				{
					m_nRetry = 0;

					CTL_Lib.Alarm_Error_Occurrence(516, dWARNING, _T("550002"));

					m_nMcStep = 1000;

					return RET_ERROR;
				}
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

				if (m_dwWaitTime[2] > 5000)
				{
					m_nRetry++;
					if (m_nRetry < st_basic_info.nRetry)
					{
						m_nMcStep = 1000;
					}
					else
					{
						m_nRetry = 0;

						CTL_Lib.Alarm_Error_Occurrence(517, dWARNING, _T("550002"));

						m_nMcStep = 1000;

						return RET_ERROR;
					}
				}
			}*/
			break;
	}

	return RET_PROCEED;
}

void CScreenInitial::OnMessageView()
{
	if(m_pMsgView == NULL)
	{
		m_pMsgView					= new CDialogMessageView;
		m_pMsgView->m_nConfirmType	= st_other_info.nConfirmType;
		m_pMsgView->m_strMessage	= st_other_info.strConfirmMsg;
		m_pMsgView->Create();
		m_pMsgView->ShowWindow(SW_SHOW);
	}
	else
	{
		//				m_pMsgView->SetFocus();
		m_pMsgView->ShowWindow(SW_SHOW);
	}
}