// ScreenMain.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "MainFrm.h"
#include "ScreenMain.h"
#include "Variable.h"
#include "Dialog_Message.h"
#include "DialogMessageBox.h"
#include "Dialog_KeyBoard.h"
#include "PublicFunction.h"
#include "Ctlbd_Variable.h"
#include "ComizoaPublic.h"
#include "FastechPublic_IO.h"
#include "Cmmsdk.h"
#include "CmmsdkDef.h"
#include "FtpClient.h"
#include "ClientEcFirst.h"
#include "CtlBd_Library.h"
#include "Dialog_Message.h"
#include "DialogMessageView.h"
#include "DialogLotDisplay.h"
#include "DialogLotStart.h"
#include "DialogTesterInterface.h"
#include "DialogPcbTray.h"
#include "AlgMemory.h"
#include "MultiColorStatic.h"
#include "ClientInterface1.h"
#include "DialogPcbPicker.h"
#include "DialogPcbBuffer.h"
#include "DialogPcb.h"
#include "DialogNextYieldInfo.h"
#include "DialogBarcode.h"
#include "LogFromat.h"
#include "ServerInterface1.h"

#define TM_SCREEN_DISPLAY		100

IMPLEMENT_DYNCREATE(CScreenMain, CFormView)

CScreenMain::CScreenMain()
	: CFormView(CScreenMain::IDD)
{
	m_bmGround.LoadBitmap(IDB_BITMAP_BACK_GROUND);

	m_pImagelist		= NULL;
	m_pMsgBox			= NULL;
	m_pLotDisplay		= NULL;
	m_pMsgView			= NULL;
	m_pLotStart			= NULL;
	m_pTesterInterface  = NULL; 
	m_pPcbTray			= NULL;
	m_pPcbPicker		= NULL;
	m_pPcbBuffer		= NULL;
	m_pBarcode			= NULL;

	m_nLotYieldDisplayChange = FALSE;
	m_nSkipUse = FALSE; //kwlee 2016.0912
}

CScreenMain::~CScreenMain()
{
	if (m_pImagelist != NULL)
	{
		delete m_pImagelist;
		m_pImagelist = NULL;
	}
}

void CScreenMain::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_CUSTOM_MAIN_WORK_INFO_C,				m_pGridWorkInfoC);
	DDX_Control(pDX, IDC_CUSTOM_MAIN_WORK_INFO_N,				m_pGridWorkInfoN);
	DDX_Control(pDX, IDC_CUSTOM_TIME_INFO,						m_pGridTimeInfo);
	DDX_Control(pDX, IDC_CUSTOM_LD_TRAY,						m_pGridLdTray);
	DDX_Control(pDX, IDC_CUSTOM_UNLD_TRAY_FIRST,				m_pGridUnLdTrayFirst);
	DDX_Control(pDX, IDC_CUSTOM_UNLD_TRAY_SECOND,				m_pGridUnLdTraySecond);
	DDX_Control(pDX, IDC_CUSTOM_REJECT_TRAY_FIRST,				m_pGridRejectTrayFirst);
	DDX_Control(pDX, IDC_CUSTOM_REJECT_TRAY_SECOND,				m_pGridRejectTraySecond);
	DDX_Control(pDX, IDC_CUSTOM_TRAY_ROBOT,						m_pGridTrayRobot);
	DDX_Control(pDX, IDC_CUSTOM_LD_BUF,							m_pGridLdBuf);
	DDX_Control(pDX, IDC_CUSTOM_UNLD_BUF,						m_pGridUnLdBuf);
	DDX_Control(pDX, IDC_CUSTOM_TEST_SITE_ROBOT,				m_pGridTestSiteRobot);
	DDX_Control(pDX, IDC_CUSTOM_RETEST_BUF_FIRST_1,				m_pGridRetestBufFirst[0]);
	DDX_Control(pDX, IDC_CUSTOM_RETEST_BUF_FIRST_2,				m_pGridRetestBufFirst[1]);
	DDX_Control(pDX, IDC_CUSTOM_RETEST_BUF_SECOND_1,			m_pGridRetestBufSecond[0]);
	DDX_Control(pDX, IDC_CUSTOM_RETEST_BUF_SECOND_2,			m_pGridRetestBufSecond[1]);
	DDX_Control(pDX, IDC_CUSTOM_TEST_SITE_TOP_1,				m_pGridTestSiteTop[0]);
	DDX_Control(pDX, IDC_CUSTOM_TEST_SITE_TOP_2,				m_pGridTestSiteTop[1]);
	DDX_Control(pDX, IDC_CUSTOM_TEST_SITE_TOP_3,				m_pGridTestSiteTop[2]);
	DDX_Control(pDX, IDC_CUSTOM_TEST_SITE_TOP_4,				m_pGridTestSiteTop[3]);
	DDX_Control(pDX, IDC_CUSTOM_TEST_SITE_BOTTOM_1,				m_pGridTestSiteBottom[0]);
	DDX_Control(pDX, IDC_CUSTOM_TEST_SITE_BOTTOM_2,				m_pGridTestSiteBottom[1]);
	DDX_Control(pDX, IDC_CUSTOM_TEST_SITE_BOTTOM_3,				m_pGridTestSiteBottom[2]);
	DDX_Control(pDX, IDC_CUSTOM_TEST_SITE_BOTTOM_4,				m_pGridTestSiteBottom[3]);
	DDX_Control(pDX, IDC_CUSTOM_BIN_REFERNCE,					m_pGridRefernce);
	DDX_Control(pDX, IDC_CUSTOM_YIELD_LOT,						m_pGridYieldLot);
	DDX_Control(pDX, IDC_CUSTOM_YIELD_DAY,						m_pGridYieldDay);
	DDX_Control(pDX, IDC_GROUP_TIME_INFO,						m_groupTimeInfo);
	DDX_Control(pDX, IDC_GROUP_WORK_INFO,						m_groupWorkInfo);
	DDX_Control(pDX, IDC_GROUP_LD_TRAY,							m_groupLdTray);
	DDX_Control(pDX, IDC_GROUP_UNLD_TRAY_FIRST,					m_groupUnLdTrayFirst);
	DDX_Control(pDX, IDC_GROUP_UNLD_TRAY_SECOND,				m_groupUnLdTraySecond);
	DDX_Control(pDX, IDC_GROUP_REJECT_TRAY_FIRST,				m_groupRejectTrayFirst);
	DDX_Control(pDX, IDC_GROUP_REJECT_TRAY_SECOND,				m_groupRejectTraySecond);
	DDX_Control(pDX, IDC_GROUP_TRAY_ROBOT,						m_groupTrayRobot);
	DDX_Control(pDX, IDC_GROUP_LD_BUF,							m_groupLdBuf);
	DDX_Control(pDX, IDC_GROUP_UNLD_BUF,						m_groupUnLdBuf);
	DDX_Control(pDX, IDC_GROUP_TEST_SITE_ROBOT,					m_groupTestSiteRobot);
	DDX_Control(pDX, IDC_GROUP_RETEST_BUF_FIRST,				m_groupRetestBufFirst);
	DDX_Control(pDX, IDC_GROUP_RETEST_BUF_SECOND,				m_groupRetestBufSecond);
	DDX_Control(pDX, IDC_GROUP_TEST_SITE_TOP,					m_groupTestSiteTop);
	DDX_Control(pDX, IDC_GROUP_TEST_SITE_BOTTOM,				m_groupTestSiteBottom);
	DDX_Control(pDX, IDC_BTN_UNLD_TRAY_OUTPUT_FIRST,			m_btnUnLdOutputFirst);
	DDX_Control(pDX, IDC_BTN_UNLD_TRAY_OUTPUT_SECOND,			m_btnUnLdOutputSecond);
	DDX_Control(pDX, IDC_BTN_REJECT_TRAY_OUTPUT_FIRST,			m_btnRejectOutputFirst);
	DDX_Control(pDX, IDC_BTN_REJECT_TRAY_OUTPUT_SECOND,			m_btnRejectOutputSecond);
	DDX_Control(pDX, IDC_BTN_LOT_DISPLAY,						m_btnLotDisplay);
	DDX_Control(pDX, IDC_BTN_LD_CVY_LOT_INFO_1,					m_btnLdCvyLotInfo1);
	DDX_Control(pDX, IDC_BTN_LD_CVY_LOT_INFO_2,					m_btnLdCvyLotInfo2);
	DDX_Control(pDX, IDC_BTN_LOT_END,							m_btnLotEnd);
	DDX_Control(pDX, IDC_MSG_RETEST_BUFFER_FIRST,				m_msgRetestBufferFirst);
	DDX_Control(pDX, IDC_MSG_RETEST_BUFFER_SECOND,				m_msgRetestBufferSecond);
	DDX_Control(pDX, IDC_BTN_DOOR_OPEN, m_btnDoorOpen);
	DDX_Control(pDX, IDC_BTN_DOOR_CLOSE, m_btnDoorClose);
}

BEGIN_MESSAGE_MAP(CScreenMain, CFormView)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_MESSAGE(WM_WORK_DISPLAY,									OnMainWorkInfoDisplay)  // 출력용 사용자 정의 메시지
	ON_MESSAGE(WM_WORK_COMMAND,									OnMainWorkInfoCommand)  // 출력용 사용자 정의 메시지
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	//ON_NOTIFY(NM_CLICK, IDC_CUSTOM_WORK_INFO,		OnWorkCellClick)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_TIME_INFO,						OnTimeCellClick)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_MAIN_WORK_INFO_C,				OnWorkInfoCurrCellClick)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_YIELD_LOT,						OnYieldCellClick)
 	ON_BN_CLICKED(IDC_BTN_UNLD_TRAY_OUTPUT_FIRST,					&CScreenMain::OnBnClickedBtnUnldTrayOutputFirst)
 	ON_BN_CLICKED(IDC_BTN_UNLD_TRAY_OUTPUT_SECOND,					&CScreenMain::OnBnClickedBtnUnldTrayOutputSecond)
 	ON_BN_CLICKED(IDC_BTN_REJECT_TRAY_OUTPUT_FIRST,					&CScreenMain::OnBnClickedBtnRejectTrayOutputFirst)
 	ON_BN_CLICKED(IDC_BTN_REJECT_TRAY_OUTPUT_SECOND,				&CScreenMain::OnBnClickedBtnRejectTrayOutputSecond)
	ON_BN_CLICKED(IDC_BTN_LOT_DISPLAY, &CScreenMain::				OnBnClickedBtnLotDisplay)

	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_LD_TRAY,							OnLdTrayCellClick)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_TRAY_ROBOT,						OnTrayRobotCellClick)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_UNLD_TRAY_FIRST,					OnUnLdTrayFirstCellClick)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_UNLD_TRAY_SECOND,				OnUnLdTraySecondCellClick)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_LD_BUF,							OnLdBufCellClick)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_UNLD_BUF,						OnUnLdBufCellClick)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_REJECT_TRAY_FIRST,				OnRejectTrayFirstCellClick)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_REJECT_TRAY_SECOND,				OnRejectTraySecondCellClick)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_RETEST_BUF_FIRST_1,				OnRetestBufFirstCellClick1)
	//ON_NOTIFY(NM_CLICK, IDC_CUSTOM_RETEST_BUF_FIRST_2,				OnRetestBufFirstCellClick2)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_RETEST_BUF_SECOND_1,				OnRetestBufSecondCellClick1)
	//ON_NOTIFY(NM_CLICK, IDC_CUSTOM_RETEST_BUF_SECOND_2,				OnRetestBufSecondCellClick2)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_TEST_SITE_ROBOT,					OnTestSiteRobotCellClick)

	
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_TEST_SITE_TOP_1,					OnTestSite1CellClick)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_TEST_SITE_TOP_2,					OnTestSite2CellClick)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_TEST_SITE_TOP_3,					OnTestSite3CellClick)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_TEST_SITE_TOP_4,					OnTestSite4CellClick)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_TEST_SITE_BOTTOM_1,				OnTestSite5CellClick)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_TEST_SITE_BOTTOM_2,				OnTestSite6CellClick)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_TEST_SITE_BOTTOM_3,				OnTestSite7CellClick)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_TEST_SITE_BOTTOM_4,				OnTestSite8CellClick)
	

	//kwlee 2016.0912
	ON_NOTIFY(NM_DBLCLK, IDC_CUSTOM_TEST_SITE_TOP_1,				OnTestSite1DCellClick)
	ON_NOTIFY(NM_DBLCLK, IDC_CUSTOM_TEST_SITE_TOP_2,				OnTestSite2DCellClick)
	ON_NOTIFY(NM_DBLCLK, IDC_CUSTOM_TEST_SITE_TOP_3,				OnTestSite3DCellClick)
	ON_NOTIFY(NM_DBLCLK, IDC_CUSTOM_TEST_SITE_TOP_4,				OnTestSite4DCellClick)
	ON_NOTIFY(NM_DBLCLK, IDC_CUSTOM_TEST_SITE_BOTTOM_1,				OnTestSite5DCellClick)
	ON_NOTIFY(NM_DBLCLK, IDC_CUSTOM_TEST_SITE_BOTTOM_2,				OnTestSite6DCellClick)
	ON_NOTIFY(NM_DBLCLK, IDC_CUSTOM_TEST_SITE_BOTTOM_3,				OnTestSite7DCellClick)
	ON_NOTIFY(NM_DBLCLK, IDC_CUSTOM_TEST_SITE_BOTTOM_4,				OnTestSite8DCellClick)

	ON_NOTIFY(NM_DBLCLK, IDC_CUSTOM_LD_TRAY,						OnLdTrayDCellClick)
	ON_NOTIFY(NM_DBLCLK, IDC_CUSTOM_UNLD_TRAY_FIRST,				OnUnLdTrayFirstDCellClick)
	ON_NOTIFY(NM_DBLCLK, IDC_CUSTOM_UNLD_TRAY_SECOND,				OnUnLdTraySecondDCellClick)
	ON_NOTIFY(NM_DBLCLK, IDC_CUSTOM_LD_BUF,							OnLdBufDCellClick)
	ON_NOTIFY(NM_DBLCLK, IDC_CUSTOM_UNLD_BUF,						OnUnLdBufDCellClick)
	ON_NOTIFY(NM_DBLCLK, IDC_CUSTOM_REJECT_TRAY_FIRST,				OnRejectTrayFirstDCellClick)
	ON_NOTIFY(NM_DBLCLK, IDC_CUSTOM_REJECT_TRAY_SECOND,				OnRejectTraySecondDCellClick)
	ON_NOTIFY(NM_DBLCLK, IDC_CUSTOM_RETEST_BUF_FIRST_1,				OnRetestBufFirstDCellClick1)
	ON_NOTIFY(NM_DBLCLK, IDC_CUSTOM_RETEST_BUF_FIRST_2,				OnRetestBufFirstDCellClick2)
	ON_NOTIFY(NM_DBLCLK, IDC_CUSTOM_RETEST_BUF_SECOND_1,			OnRetestBufSecondDCellClick1)
	ON_NOTIFY(NM_DBLCLK, IDC_CUSTOM_RETEST_BUF_SECOND_2,			OnRetestBufSecondDCellClick2)
	ON_NOTIFY(NM_DBLCLK, IDC_CUSTOM_TEST_SITE_ROBOT,				OnTestSiteRobotDCellClick)
	////

	ON_NOTIFY(NM_RCLICK, IDC_CUSTOM_TEST_SITE_TOP_1,				OnTestSite1CellRClick)
	ON_NOTIFY(NM_RCLICK, IDC_CUSTOM_TEST_SITE_TOP_2,				OnTestSite2CellRClick)
	ON_NOTIFY(NM_RCLICK, IDC_CUSTOM_TEST_SITE_TOP_3,				OnTestSite3CellRClick)
	ON_NOTIFY(NM_RCLICK, IDC_CUSTOM_TEST_SITE_TOP_4,				OnTestSite4CellRClick)
	ON_NOTIFY(NM_RCLICK, IDC_CUSTOM_TEST_SITE_BOTTOM_1,				OnTestSite5CellRClick)
	ON_NOTIFY(NM_RCLICK, IDC_CUSTOM_TEST_SITE_BOTTOM_2,				OnTestSite6CellRClick)
	ON_NOTIFY(NM_RCLICK, IDC_CUSTOM_TEST_SITE_BOTTOM_3,				OnTestSite7CellRClick)
	ON_NOTIFY(NM_RCLICK, IDC_CUSTOM_TEST_SITE_BOTTOM_4,				OnTestSite8CellRClick)
	
	ON_BN_CLICKED(IDC_BTN_LD_CVY_LOT_INFO_1, &CScreenMain::OnBnClickedBtnLdCvyLotInfo1)
	ON_BN_CLICKED(IDC_BTN_LD_CVY_LOT_INFO_2, &CScreenMain::OnBnClickedBtnLdCvyLotInfo2)
	ON_BN_CLICKED(IDC_BTN_LOT_END, &CScreenMain::OnBnClickedBtnLotEnd)
	ON_BN_CLICKED(IDC_BTN_DOOR_OPEN, &CScreenMain::OnBnClickedBtnDoorOpen)
	ON_BN_CLICKED(IDC_BTN_DOOR_CLOSE, &CScreenMain::OnBnClickedBtnDoorClose)
END_MESSAGE_MAP()


// CScreenMain 진단입니다.

#ifdef _DEBUG
void CScreenMain::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CScreenMain::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void CScreenMain::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	OnInitButton();
	OnInitGroupBox();
	OnInitLabel();
	//OnInitDigit();
	OnInitGridWorkInfoCurr();
	OnInitGridWorkInfoNext();
	OnInitGridTimeInfo(st_work_info.nPgmInfo);	
	OnInitGridLdTray();
	OnInitGridUnLdTrayFirst();
	OnInitGridUnLdTraySecond();
	OnInitGridRejectTrayFirst();
	OnInitGridRejectTraySecond();
	OnInitGridTrayRobot();
	OnInitGridLdBuf();
	OnInitGridUnLdBuf();
	OnInitGridTestSiteRobot();
	OnInitGridRetestBufFirst1();
	//OnInitGridRetestBufFirst2();
	OnInitGridRetestBufSecond1();
	//OnInitGridRetestBufSecond2();
	OnInitGridTestSiteTop();
	OnInitGridTestSiteBottom();
	OnInitGridRefernce();
	OnInitGridYieldLot();
	OnInitGridYieldDay();

	OnMainDisplay();

	SetTimer(TM_SCREEN_DISPLAY,100,NULL);

	st_handler_info.cWndMain = this;
}

void CScreenMain::OnMainDisplay()
{
	OnMainCountDisplay();
	OnMainTimeDisplay();
	OnMainLdTray();
	OnMainUnLdTrayFirst();
	OnMainUnLdTraySecond();
	OnMainRejectTrayFirst();
	OnMainRejectTraySecond();
	OnMainTrayRobot();
	OnMainLdBuf();
	OnMainUnLdBuf();
	OnMainTestSiteRobot();
	OnMainRetestBufFirst();
	OnMainRetestBufSecond();
	OnMainTestSiteTop();
	OnMainTestSiteBottom();
	OnMainYieldDay();
	OnMainYieldLot();
	OnMainLotDisplay();

	if (st_handler_info.cWndTitle != NULL)
	{
		st_handler_info.cWndTitle->PostMessage(WM_STATUS_CHANGE, DEVICE_MODE, st_basic_info.nModeDevice);			// Device Mode
		st_handler_info.cWndTitle->PostMessage(WM_STATUS_CHANGE, INTERFACE_MODE, st_work_info.nFtpWork);
		//kwlee 2016.0825
		st_handler_info.cWndTitle->PostMessage(WM_STATUS_CHANGE, BCR_1D, st_basic_info.nMode1DBcr);			
		st_handler_info.cWndTitle->PostMessage(WM_STATUS_CHANGE, BCR_2D, st_basic_info.nMode2DBcr);		
	}
}


BOOL CScreenMain::DestroyWindow()
{
	KillTimer(TM_SCREEN_DISPLAY);

	if (m_pImagelist != NULL)
	{
		delete m_pImagelist;
		m_pImagelist = NULL;
	}

	if (m_pMsgBox != NULL)
	{
		m_pMsgBox->DestroyWindow();
		delete m_pMsgBox;
		m_pMsgBox = NULL;
	}
	
	if (m_pLotDisplay != NULL)
	{
		m_pLotDisplay->DestroyWindow();
		delete m_pLotDisplay;
		m_pLotDisplay = NULL;
	}
	
	if (m_pMsgView != NULL)
	{
		m_pMsgView->DestroyWindow();
		delete m_pMsgView;
		m_pMsgView = NULL;
	}
	
	if (m_pLotStart != NULL)
	{
		m_pLotStart->DestroyWindow();
		delete m_pLotStart;
		m_pLotStart = NULL;
	}

	if (m_pPcbTray != NULL)
	{
		m_pPcbTray->DestroyWindow();

		delete m_pPcbTray;
		m_pPcbTray = NULL;
	}

	if (m_pPcbPicker != NULL)
	{
		m_pPcbPicker->DestroyWindow();

		delete m_pPcbPicker;
		m_pPcbPicker = NULL;
	}

	if (m_pPcbBuffer != NULL)
	{
		m_pPcbBuffer->DestroyWindow();

		delete m_pPcbBuffer;
		m_pPcbBuffer = NULL;
	}

	if (m_pTesterInterface != NULL)
	{
		m_pTesterInterface->DestroyWindow();

		delete m_pTesterInterface;
		m_pTesterInterface = NULL;
	}

	if (m_pBarcode != NULL)
	{
		m_pBarcode->DestroyWindow();

		delete m_pBarcode;
		m_pBarcode = NULL;
	}
	st_handler_info.cWndMain = NULL;

	return CFormView::DestroyWindow();
}


BOOL CScreenMain::OnEraseBkgnd(CDC* pDC)
{
	CDC  memDC;                     // 메모리 DC
	CBitmap * pOldBitmap = NULL;    // m_BackBitmap으로 새로운 그림을 DC에 그릴 때, 이전 그려졌던 DC(즉, Bitmap)을 저장.
	BITMAP bitmapInfo;              // 그림의 정보(m_BackBitmap)
	st_handler_info.m_BackBitmap.GetBitmap(&bitmapInfo); // Bitmap 크기 구함.

	memDC.CreateCompatibleDC(pDC);  // 메모리 DC 생성
	pOldBitmap = memDC.SelectObject(&m_bmGround);
	pDC->BitBlt(0, 0, bitmapInfo.bmWidth, bitmapInfo.bmHeight, &memDC, 0, 0, SRCCOPY);   // 메모리 DC에 그림을 그림

	memDC.SelectObject(pOldBitmap);
	memDC.DeleteDC();

	return true; // 흰 부러쉬가 아닌 배경을 그려야 하기 때문에 true
}


void CScreenMain::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	CRect rect;

}

LRESULT CScreenMain::OnMainWorkInfoCommand(WPARAM wParam, LPARAM lParam) 
{
	switch (wParam)
	{
		case MAIN_MESSAGE_BOX_CREATE_REQ:
			if (st_work_info.nBdReadyChk == YES) return 0;

			if (m_pMsgBox == NULL)
			{
				m_pMsgBox			= new CDialogMessageBox;

				m_pMsgBox->Create();
				m_pMsgBox->OnMessageDisplay(st_other_info.strBoxMsg);
				m_pMsgBox->ShowWindow(SW_SHOW);
			}
			else
			{
				m_pMsgBox->OnMessageDisplay(st_other_info.strBoxMsg);
				m_pMsgBox->ShowWindow(SW_SHOW);

				//((CMainFrame *)AfxGetMainWnd())->SetFocus();
			}
			st_work_info.nBdReadyChk = YES;
			break;

		case MAIN_MESSAGE_BOX_DELETE_REQ:
			if (st_other_info.nBuzYesNo == NO)
			{
				if (m_pMsgBox != NULL)
				{
					m_pMsgBox->DestroyWindow();

					delete m_pMsgBox;
					m_pMsgBox = NULL;
				}
				st_work_info.nBdReadyChk = NO;
			}
			break;

		case MAIN_LOT_END_CREATE_REQ:
			if (m_pLotDisplay == NULL)
			{
				m_pLotDisplay	=	new CDialogLotDisplay;

				m_pLotDisplay->m_strLotNo = st_track_info.strLotNo;
				m_pLotDisplay->m_strPartNo = st_track_info.strPartNo;

				m_pLotDisplay->Create();
				m_pLotDisplay->ShowWindow(SW_SHOW);
			}
			else
			{
				m_pLotDisplay->SetFocus();
			}
			break;

		case MAIN_LOT_END_DELETE_REQ:
			if (m_pLotDisplay != NULL)
			{
				m_pLotDisplay->DestroyWindow();

				delete m_pLotDisplay;
				m_pLotDisplay = NULL;
				
			}
			break;

		case MAIN_LOT_START_CREATE_REQ:
			if (m_pLotStart == NULL)
			{
				m_pLotStart	=	new CDialogLotStart;
				m_pLotStart->m_nLdCvySite = (int) lParam;

				m_pLotStart->Create();
				m_pLotStart->ShowWindow(SW_SHOW);
				FAS_IO.set_out_bit(st_io_info.o_Buzz2OnOff,  IO_ON);
			}
			else
			{
				m_pLotStart->SetFocus();
			}
			break;

		case MAIN_LOT_START_DELETE_REQ:
			if (m_pLotStart != NULL)
			{
				m_pLotStart->DestroyWindow();

				delete m_pLotStart;
				m_pLotStart = NULL;

				OnMainLotDisplay();

				FAS_IO.set_out_bit(st_io_info.o_Buzz2OnOff,  IO_OFF);
			}
			break;
			//kwlee 2016.0808
		case MAIN_PCB_INTER_CREATE_MSG:
			if (m_pTesterInterface == NULL)
			{
				m_pTesterInterface			=	new CDialogTesterInterface;
				m_pTesterInterface->m_nSite= lParam;
				m_pTesterInterface->Create();
				m_pTesterInterface->ShowWindow(SW_SHOW);
			}
			else
			{
				//				m_pPcbInterface->SetFocus();
				m_pTesterInterface->ShowWindow(SW_SHOW);
			}
			break;

		case MAIN_PCB_INTER_DELETE_MSG:
			if (m_pTesterInterface != NULL)
			{
				m_pTesterInterface->DestroyWindow();
				delete m_pTesterInterface;
				m_pTesterInterface = NULL;
			}
			break;
			///Tray 6
		case MAIN_PCB_TRAY_CREATE_MSG:
			if (m_pPcbTray == NULL)
			{
				m_pPcbTray			=	new CDialogPcbTray;
				m_pPcbTray->m_nSite	= lParam;
				m_pPcbTray->Create();
				m_pPcbTray->ShowWindow(SW_SHOW);
			}
			else
			{
				//				m_pPcbTray->SetFocus();
				m_pPcbTray->ShowWindow(SW_SHOW);
			}
			break;

		case MAIN_PCB_TRAY_DELETE_MSG:
			if (m_pPcbTray != NULL)
			{
				m_pPcbTray->DestroyWindow();
				delete m_pPcbTray;
				m_pPcbTray = NULL;
			}
			break;

			//buffer 4
		case MAIN_PCB_BUFFER_CREATE_MSG:
			if (m_pPcbBuffer == NULL)
			{
				m_pPcbBuffer			=	new CDialogPcbBuffer;
				m_pPcbBuffer->m_nSite	= lParam;
				m_pPcbBuffer->Create();
				m_pPcbBuffer->ShowWindow(SW_SHOW);
			}
			else
			{
				//				m_pPcbTray->SetFocus();
				m_pPcbBuffer->ShowWindow(SW_SHOW);
			}
			break;

		case MAIN_PCB_BUFFER_DELETE_MSG:
			if (m_pPcbBuffer != NULL)
			{
				m_pPcbBuffer->DestroyWindow();
				delete m_pPcbBuffer;
				m_pPcbBuffer = NULL;
			}
			break;

			//Picker
		case MAIN_PCB_PICKER_CREATE_MSG:
			if (m_pPcbPicker == NULL)
			{
				m_pPcbPicker			=	new CDialogPcbPicker;
				m_pPcbPicker->m_nSite	= lParam;
				m_pPcbPicker->Create();
				m_pPcbPicker->ShowWindow(SW_SHOW);
			}
			else
			{
				//				m_pPcbTray->SetFocus();
				m_pPcbPicker->ShowWindow(SW_SHOW);
			}
			break;

		case MAIN_PCB_PICKER_DELETE_MSG:
			if (m_pPcbPicker != NULL)
			{
				m_pPcbPicker->DestroyWindow();
				delete m_pPcbPicker;
				m_pPcbPicker = NULL;
			}
			break;
		
		case MAIN_BARCODE_BOX_CREATE_REQ:
			if (m_pBarcode == NULL)
			{
				m_pBarcode				=	new CDialogBarcode;
				m_pBarcode->m_nWorkSite = lParam;
				m_pBarcode->Create();
				m_pBarcode->ShowWindow(SW_SHOW);
			}
			else
			{
				m_pBarcode->ShowWindow(SW_SHOW);
			}
			FAS_IO.set_out_bit(st_io_info.o_Buzz2OnOff,  IO_ON);
			break;

		case MAIN_BARCODE_BOX_DELETE_REQ:
			if (m_pBarcode != NULL)
			{
				m_pBarcode->DestroyWindow();
				delete m_pBarcode;
				m_pBarcode = NULL;
			}
			break;
	}
	return 0;
}


void CScreenMain::OnInitButton()
{
	m_btnUnLdOutputFirst.SetBitmaps(IDC_BTN_UNLD_TRAY_OUTPUT_FIRST, IDB_BITMAP_GEAR_DN, WINDOW_DN, IDB_BITMAP_GEAR_UP, WINDOW_UP);
	m_btnUnLdOutputFirst.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnUnLdOutputFirst.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnUnLdOutputFirst.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnUnLdOutputFirst.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnUnLdOutputFirst.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnUnLdOutputFirst.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnUnLdOutputFirst.SetFont(clsFunc.m_pFont[3]);
	m_btnUnLdOutputFirst.SetTooltipText(_T("#1 UnLoader 배출"));

	m_btnUnLdOutputSecond.SetBitmaps(IDC_BTN_UNLD_TRAY_OUTPUT_SECOND, IDB_BITMAP_GEAR_DN, WINDOW_DN, IDB_BITMAP_GEAR_UP, WINDOW_UP);
	m_btnUnLdOutputSecond.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnUnLdOutputSecond.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnUnLdOutputSecond.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnUnLdOutputSecond.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnUnLdOutputSecond.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnUnLdOutputSecond.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnUnLdOutputSecond.SetFont(clsFunc.m_pFont[3]);
	m_btnUnLdOutputSecond.SetTooltipText(_T("#2 UnLoader 배출"));

	m_btnRejectOutputFirst.SetBitmaps(IDC_BTN_REJECT_TRAY_OUTPUT_FIRST, IDB_BITMAP_GEAR_DN, WINDOW_DN, IDB_BITMAP_GEAR_UP, WINDOW_UP);
	m_btnRejectOutputFirst.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnRejectOutputFirst.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnRejectOutputFirst.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnRejectOutputFirst.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnRejectOutputFirst.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnRejectOutputFirst.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnRejectOutputFirst.SetFont(clsFunc.m_pFont[3]);
	m_btnRejectOutputFirst.SetTooltipText(_T("#1 Reject 배출"));

	m_btnRejectOutputSecond.SetBitmaps(IDC_BTN_REJECT_TRAY_OUTPUT_SECOND, IDB_BITMAP_GEAR_DN, WINDOW_DN, IDB_BITMAP_GEAR_UP, WINDOW_UP);
	m_btnRejectOutputSecond.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnRejectOutputSecond.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnRejectOutputSecond.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnRejectOutputSecond.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnRejectOutputSecond.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnRejectOutputSecond.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnRejectOutputSecond.SetFont(clsFunc.m_pFont[3]);
	m_btnRejectOutputSecond.SetTooltipText(_T("#2 Reject 배출"));

	m_btnLotDisplay.SetBitmaps(IDC_BTN_LOT_DISPLAY, IDB_NETWORK_ON_DN, WINDOW_DN, IDB_NETWORK_ON_UP, WINDOW_UP);
	m_btnLotDisplay.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnLotDisplay.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnLotDisplay.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnLotDisplay.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnLotDisplay.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnLotDisplay.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnLotDisplay.SetFont(clsFunc.m_pFont[3]);
	m_btnLotDisplay.SetTooltipText(_T("Lot Display"));

	m_btnLdCvyLotInfo1.SetBitmaps(IDC_BTN_LD_CVY_LOT_INFO_1, IDB_NETWORK_ON_DN, WINDOW_DN, IDB_NETWORK_ON_UP, WINDOW_UP);
	m_btnLdCvyLotInfo1.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnLdCvyLotInfo1.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnLdCvyLotInfo1.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnLdCvyLotInfo1.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnLdCvyLotInfo1.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnLdCvyLotInfo1.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnLdCvyLotInfo1.SetFont(clsFunc.m_pFont[3]);
	m_btnLdCvyLotInfo1.SetTooltipText(_T("Lot Info Display"));

	m_btnLdCvyLotInfo2.SetBitmaps(IDC_BTN_LD_CVY_LOT_INFO_2, IDB_NETWORK_ON_DN, WINDOW_DN, IDB_NETWORK_ON_UP, WINDOW_UP);
	m_btnLdCvyLotInfo2.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnLdCvyLotInfo2.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnLdCvyLotInfo2.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnLdCvyLotInfo2.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnLdCvyLotInfo2.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnLdCvyLotInfo2.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnLdCvyLotInfo2.SetFont(clsFunc.m_pFont[3]);
	m_btnLdCvyLotInfo2.SetTooltipText(_T("Lot Info Display"));

	m_btnLotEnd.SetBitmaps(IDC_BTN_LOT_END, IDB_NETWORK_ON_DN, WINDOW_DN, IDB_NETWORK_ON_UP, WINDOW_UP);
	m_btnLotEnd.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnLotEnd.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnLotEnd.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnLotEnd.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnLotEnd.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnLotEnd.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnLotEnd.SetFont(clsFunc.m_pFont[3]);
	m_btnLotEnd.SetTooltipText(_T("Lot End"));

	m_btnDoorClose.SetBitmaps(IDC_BTN_LD_CVY_LOT_INFO_2, IDB_NETWORK_ON_DN, WINDOW_DN, IDB_NETWORK_ON_UP, WINDOW_UP);
	m_btnDoorClose.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnDoorClose.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnDoorClose.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnDoorClose.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnDoorClose.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnDoorClose.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnDoorClose.SetFont(clsFunc.m_pFont[3]);
	m_btnDoorClose.SetTooltipText(_T("Door Close"));

	m_btnDoorOpen.SetBitmaps(IDC_BTN_LOT_END, IDB_NETWORK_ON_DN, WINDOW_DN, IDB_NETWORK_ON_UP, WINDOW_UP);
	m_btnDoorOpen.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnDoorOpen.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnDoorOpen.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnDoorOpen.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnDoorOpen.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnDoorOpen.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnDoorOpen.SetFont(clsFunc.m_pFont[3]);
	m_btnDoorOpen.SetTooltipText(_T("Door Open"));
}


LRESULT CScreenMain::OnMainWorkInfoDisplay(WPARAM wParam, LPARAM lParam) 
{
	int nPart = 0;
	int nSite = 0;

	nPart = (int)wParam;
	nSite = (int)lParam;

	switch (wParam)
	{
		case MAIN_TIMEINFO:
			OnMainTimeDisplay();
			break;

		case MAIN_COUNT_DISPLAY:
			OnMainCountDisplay();
			break;

		case MAIN_LOT_DISPLAY:
			OnMainLotDisplay();
			break;
	}

	return 0;
}


void CScreenMain::OnInitGridTimeInfo(int nMode)
{
	int   i, j;
	int	  max_row, max_col;
	int   row;
	CString strTmp;

	if (nMode == 0)
	{
		max_row = 4; // 
		max_col = 2;
	}
	else
	{
		max_row = 6; // 
		max_col = 2;
	}
	
	m_pGridTimeInfo.SetFrameFocusCell(FALSE);
	m_pGridTimeInfo.SetTrackFocusCell(FALSE);
	m_pGridTimeInfo.EnableSelection(FALSE);

	m_pGridTimeInfo.SetGridLineColor(BLACK_C);
	m_pGridTimeInfo.SetGridLines(1);

	m_pGridTimeInfo.SetRowCount(max_row);
	m_pGridTimeInfo.SetColumnCount(max_col);

	m_pGridTimeInfo.SetFixedRowCount(0);
	m_pGridTimeInfo.SetFixedColumnCount(0);
	m_pGridTimeInfo.SetFixedBkColor(RGB(0,0,200));
	m_pGridTimeInfo.SetFixedBkColor(RGB(200,200,255));
	m_pGridTimeInfo.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<max_row; i++) 
	{
		if (nMode == 0)
		{
			m_pGridTimeInfo.SetRowHeight(i, 37);
		}
		else
		{
			m_pGridTimeInfo.SetRowHeight(i, 25);
		}

		for (j=0; j<max_col; j++) 
		{
			m_pGridTimeInfo.SetColumnWidth(j, 120);
				
			m_pGridTimeInfo.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridTimeInfo.SetItemState(i, j, GVIS_READONLY);
			m_pGridTimeInfo.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
		}
	}

	row = 0;
	m_pGridTimeInfo.SetItemBkColour(row, 0, GREEN_L, BLACK_L);
	m_pGridTimeInfo.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridTimeInfo.SetItemText(row, 0, _T("RUN"));

	m_pGridTimeInfo.SetItemBkColour(row, 1, GREEN_L, BLACK_L);
	m_pGridTimeInfo.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
	m_pGridTimeInfo.SetItemText(row, 1, _T("00 : 00 : 00"));

	row = 1;
	m_pGridTimeInfo.SetItemBkColour(row, 0, YELLOW_L, BLACK_L);
	m_pGridTimeInfo.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridTimeInfo.SetItemText(row, 0, _T("STOP"));

	m_pGridTimeInfo.SetItemBkColour(row, 1, YELLOW_L, BLACK_L);
	m_pGridTimeInfo.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
	m_pGridTimeInfo.SetItemText(row, 1, _T("00 : 00 : 00"));

	row = 2;
	m_pGridTimeInfo.SetItemBkColour(row, 0, RED_L, BLACK_L);
	m_pGridTimeInfo.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridTimeInfo.SetItemText(row, 0, _T("ALARM"));

	m_pGridTimeInfo.SetItemBkColour(row, 1, RED_L, BLACK_L);
	m_pGridTimeInfo.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
	m_pGridTimeInfo.SetItemText(row, 1, _T("00 : 00 : 00"));

	row = 3;
	m_pGridTimeInfo.SetItemBkColour(row, 0, BLUE_C, WHITE_C);
	m_pGridTimeInfo.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridTimeInfo.SetItemText(row, 0, _T("MTBI"));

	m_pGridTimeInfo.SetItemBkColour(row, 1, BLUE_C, WHITE_C);
	m_pGridTimeInfo.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
	m_pGridTimeInfo.SetItemText(row, 1, _T("00 : 00 : 00"));

	if (nMode == 1)
	{
		row = 4;
		m_pGridTimeInfo.SetItemBkColour(row, 0, BLACK_C, GREEN_C);
		m_pGridTimeInfo.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
		m_pGridTimeInfo.SetItemText(row, 0, _T("Latest/Daily UPH"));
		//m_pGridTimeInfo.SetItemText(row, 0, _T("Daily UPH"));

		m_pGridTimeInfo.SetItemBkColour(row, 1, BLACK_C, GREEN_C);
		m_pGridTimeInfo.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
		m_pGridTimeInfo.SetItemText(row, 1, _T("0"));

		row = 5;
		m_pGridTimeInfo.SetItemBkColour(row, 0, YELLOW_L, BLACK_L);
		m_pGridTimeInfo.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
		m_pGridTimeInfo.SetItemText(row, 0, _T("Cycle Time"));

		m_pGridTimeInfo.SetItemBkColour(row, 1, WHITE_C, BLACK_L);
		m_pGridTimeInfo.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
		m_pGridTimeInfo.SetItemText(row, 1, _T("0"));
	}
}


void CScreenMain::OnInitGroupBox()
{
	m_groupWorkInfo.SetFont(clsFunc.OnLogFont(14));
	m_groupWorkInfo.SetCatptionTextColor(BLUE_C);
	m_groupWorkInfo.SetBorderColor(ORANGE_C);
	m_groupWorkInfo.SetFontBold(TRUE);
	m_groupWorkInfo.SetBackgroundColor(WINDOW_UP);
	
	m_groupTimeInfo.SetFont(clsFunc.OnLogFont(14));
	m_groupTimeInfo.SetCatptionTextColor(BLUE_C);
	m_groupTimeInfo.SetBorderColor(ORANGE_C);
	m_groupTimeInfo.SetFontBold(TRUE);
	m_groupTimeInfo.SetBackgroundColor(WINDOW_UP);

	m_groupLdTray.SetFont(clsFunc.OnLogFont(14));
	m_groupLdTray.SetCatptionTextColor(BLUE_C);
	m_groupLdTray.SetBorderColor(ORANGE_C);
	m_groupLdTray.SetFontBold(TRUE);
	m_groupLdTray.SetBackgroundColor(WINDOW_UP);

	m_groupUnLdTrayFirst.SetFont(clsFunc.OnLogFont(14));
	m_groupUnLdTrayFirst.SetCatptionTextColor(BLUE_C);
	m_groupUnLdTrayFirst.SetBorderColor(ORANGE_C);
	m_groupUnLdTrayFirst.SetFontBold(TRUE);
	m_groupUnLdTrayFirst.SetBackgroundColor(WINDOW_UP);

	m_groupUnLdTraySecond.SetFont(clsFunc.OnLogFont(14));
	m_groupUnLdTraySecond.SetCatptionTextColor(BLUE_C);
	m_groupUnLdTraySecond.SetBorderColor(ORANGE_C);
	m_groupUnLdTraySecond.SetFontBold(TRUE);
	m_groupUnLdTraySecond.SetBackgroundColor(WINDOW_UP);

	m_groupRejectTrayFirst.SetFont(clsFunc.OnLogFont(14));
	m_groupRejectTrayFirst.SetCatptionTextColor(BLUE_C);
	m_groupRejectTrayFirst.SetBorderColor(ORANGE_C);
	m_groupRejectTrayFirst.SetFontBold(TRUE);
	m_groupRejectTrayFirst.SetBackgroundColor(WINDOW_UP);

	m_groupRejectTraySecond.SetFont(clsFunc.OnLogFont(14));
	m_groupRejectTraySecond.SetCatptionTextColor(BLUE_C);
	m_groupRejectTraySecond.SetBorderColor(ORANGE_C);
	m_groupRejectTraySecond.SetFontBold(TRUE);
	m_groupRejectTraySecond.SetBackgroundColor(WINDOW_UP);

	m_groupTrayRobot.SetFont(clsFunc.OnLogFont(14));
	m_groupTrayRobot.SetCatptionTextColor(BLUE_C);
	m_groupTrayRobot.SetBorderColor(ORANGE_C);
	m_groupTrayRobot.SetFontBold(TRUE);
	m_groupTrayRobot.SetBackgroundColor(WINDOW_UP);

	m_groupLdBuf.SetFont(clsFunc.OnLogFont(14));
	m_groupLdBuf.SetCatptionTextColor(BLUE_C);
	m_groupLdBuf.SetBorderColor(ORANGE_C);
	m_groupLdBuf.SetFontBold(TRUE);
	m_groupLdBuf.SetBackgroundColor(WINDOW_UP);

	m_groupWorkInfo.SetFont(clsFunc.OnLogFont(14));
	m_groupWorkInfo.SetCatptionTextColor(BLUE_C);
	m_groupWorkInfo.SetBorderColor(ORANGE_C);
	m_groupWorkInfo.SetFontBold(TRUE);
	m_groupWorkInfo.SetBackgroundColor(WINDOW_UP);

	m_groupUnLdBuf.SetFont(clsFunc.OnLogFont(14));
	m_groupUnLdBuf.SetCatptionTextColor(BLUE_C);
	m_groupUnLdBuf.SetBorderColor(ORANGE_C);
	m_groupUnLdBuf.SetFontBold(TRUE);
	m_groupUnLdBuf.SetBackgroundColor(WINDOW_UP);

	m_groupTestSiteRobot.SetFont(clsFunc.OnLogFont(14));
	m_groupTestSiteRobot.SetCatptionTextColor(BLUE_C);
	m_groupTestSiteRobot.SetBorderColor(ORANGE_C);
	m_groupTestSiteRobot.SetFontBold(TRUE);
	m_groupTestSiteRobot.SetBackgroundColor(WINDOW_UP);

	m_groupRetestBufFirst.SetFont(clsFunc.OnLogFont(14));
	m_groupRetestBufFirst.SetCatptionTextColor(BLUE_C);
	m_groupRetestBufFirst.SetBorderColor(ORANGE_C);
	m_groupRetestBufFirst.SetFontBold(TRUE);
	m_groupRetestBufFirst.SetBackgroundColor(WINDOW_UP);

	m_groupRetestBufSecond.SetFont(clsFunc.OnLogFont(14));
	m_groupRetestBufSecond.SetCatptionTextColor(BLUE_C);
	m_groupRetestBufSecond.SetBorderColor(ORANGE_C);
	m_groupRetestBufSecond.SetFontBold(TRUE);
	m_groupRetestBufSecond.SetBackgroundColor(WINDOW_UP);

	m_groupTestSiteTop.SetFont(clsFunc.OnLogFont(14));
	m_groupTestSiteTop.SetCatptionTextColor(BLUE_C);
	m_groupTestSiteTop.SetBorderColor(ORANGE_C);
	m_groupTestSiteTop.SetFontBold(TRUE);
	m_groupTestSiteTop.SetBackgroundColor(WINDOW_UP);

	m_groupTestSiteBottom.SetFont(clsFunc.OnLogFont(14));
	m_groupTestSiteBottom.SetCatptionTextColor(BLUE_C);
	m_groupTestSiteBottom.SetBorderColor(ORANGE_C);
	m_groupTestSiteBottom.SetFontBold(TRUE);
	m_groupTestSiteBottom.SetBackgroundColor(WINDOW_UP);
}
void CScreenMain::OnInitLabel()
{
	m_msgRetestBufferFirst.SetFont(clsFunc.m_pFont[2]);
	m_msgRetestBufferFirst.SetWindowText(_T("-" ));
	m_msgRetestBufferFirst.SetCenterText();
	m_msgRetestBufferFirst.SetColor(WHITE_C);
	m_msgRetestBufferFirst.SetGradientColor( GREEN_C);
	m_msgRetestBufferFirst.SetTextColor(BLACK_C);

	m_msgRetestBufferSecond.SetFont(clsFunc.m_pFont[2]);
	m_msgRetestBufferSecond.SetWindowText(_T("-" ));
	m_msgRetestBufferSecond.SetCenterText();
	m_msgRetestBufferSecond.SetColor(WHITE_C);
	m_msgRetestBufferSecond.SetGradientColor( GREEN_C);
	m_msgRetestBufferSecond.SetTextColor(BLACK_C);
}

void CScreenMain::OnInitGridWorkInfoCurr()
{
	int   i, j;
	int	  max_row, max_col;
	int   row;

	CString strTmp;
	CRect rect;

	max_row = 6; // unloader tray y count
	max_col = 2;

	m_pGridWorkInfoC.SetFrameFocusCell(FALSE);
	m_pGridWorkInfoC.SetTrackFocusCell(FALSE);
	m_pGridWorkInfoC.EnableSelection(FALSE);

	m_pGridWorkInfoC.SetGridLineColor(BLACK_C);
	m_pGridWorkInfoC.SetGridLines(1);

	m_pGridWorkInfoC.SetRowCount(max_row);
	m_pGridWorkInfoC.SetColumnCount(max_col);

	m_pGridWorkInfoC.SetFixedRowCount(0);
	m_pGridWorkInfoC.SetFixedColumnCount(0);
	m_pGridWorkInfoC.SetFixedBkColor(RGB(0,0,200));
	m_pGridWorkInfoC.SetFixedBkColor(RGB(200,200,255));
	m_pGridWorkInfoC.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<max_row; i++) 
	{
		m_pGridWorkInfoC.SetRowHeight(i, 23);

		for (j=0; j<max_col; j++) 
		{
			m_pGridWorkInfoC.SetColumnWidth(j, 120);
				
			m_pGridWorkInfoC.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridWorkInfoC.SetItemState(i, j, GVIS_READONLY);
			m_pGridWorkInfoC.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
		}
	}
	
	row = 0;
	m_pGridWorkInfoC.MergeCells(row, 0, row, 1);
	m_pGridWorkInfoC.SetItemBkColour(row, 0, RGB(219, 229, 241), BLACK_C);
	m_pGridWorkInfoC.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridWorkInfoC.SetItemText(row, 0, _T("Current Lot Info"));

	row = 1;
	m_pGridWorkInfoC.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
	m_pGridWorkInfoC.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridWorkInfoC.SetItemText(row, 0, _T("Lot No"));

	m_pGridWorkInfoC.SetItemBkColour(row, 1, WHITE_C, BLACK_C);
	m_pGridWorkInfoC.SetItemFont(row, 1, &clsFunc.OnLogFont(13));
	m_pGridWorkInfoC.SetItemText(row, 1, _T("-"));

	row = 2;
	m_pGridWorkInfoC.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
	m_pGridWorkInfoC.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridWorkInfoC.SetItemText(row, 0, _T("Part No"));

	m_pGridWorkInfoC.SetItemBkColour(row, 1, WHITE_C, BLACK_C);
	m_pGridWorkInfoC.SetItemFont(row, 1, &clsFunc.OnLogFont(11));
	m_pGridWorkInfoC.SetItemText(row, 1, _T("-"));

	row = 3;
	m_pGridWorkInfoC.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
	m_pGridWorkInfoC.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridWorkInfoC.SetItemText(row, 0, _T("Last Module"));

	m_pGridWorkInfoC.SetItemBkColour(row, 1, WHITE_C, BLACK_C);
	m_pGridWorkInfoC.SetItemFont(row, 1, &clsFunc.OnLogFont(11));
	m_pGridWorkInfoC.SetItemText(row, 1, _T("-"));

	row = 4;
	m_pGridWorkInfoC.SetItemBkColour(row, 0, ORANGE_C, BLACK_C);
	m_pGridWorkInfoC.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridWorkInfoC.SetItemText(row, 0, _T("Hour Socket"));

	m_pGridWorkInfoC.SetItemBkColour(row, 1, WHITE_C, BLACK_C);
	m_pGridWorkInfoC.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
	m_pGridWorkInfoC.SetItemText(row, 1, _T("-"));

	row = 5;
	m_pGridWorkInfoC.SetItemBkColour(row, 0, ORANGE_C, BLACK_C);
	m_pGridWorkInfoC.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridWorkInfoC.SetItemText(row, 0, _T("Daily Socket"));

	m_pGridWorkInfoC.SetItemBkColour(row, 1, WHITE_C, BLACK_C);
	m_pGridWorkInfoC.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
	m_pGridWorkInfoC.SetItemText(row, 1, _T("-"));
}

void CScreenMain::OnInitGridWorkInfoNext()
{
	int   i, j;
	int	  max_row, max_col;
	int   row;

	CString strTmp;
	CRect rect;

	max_row = 3; // unloader tray y count
	max_col = 2;

	m_pGridWorkInfoN.SetFrameFocusCell(FALSE);
	m_pGridWorkInfoN.SetTrackFocusCell(FALSE);
	m_pGridWorkInfoN.EnableSelection(FALSE);

	m_pGridWorkInfoN.SetGridLineColor(BLACK_C);
	m_pGridWorkInfoN.SetGridLines(1);

	m_pGridWorkInfoN.SetRowCount(max_row);
	m_pGridWorkInfoN.SetColumnCount(max_col);

	m_pGridWorkInfoN.SetFixedRowCount(0);
	m_pGridWorkInfoN.SetFixedColumnCount(0);
	m_pGridWorkInfoN.SetFixedBkColor(RGB(0,0,200));
	m_pGridWorkInfoN.SetFixedBkColor(RGB(200,200,255));
	m_pGridWorkInfoN.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<max_row; i++) 
	{
		m_pGridWorkInfoN.SetRowHeight(i, 23);

		for (j=0; j<max_col; j++) 
		{
			m_pGridWorkInfoN.SetColumnWidth(j, 120);
				
			m_pGridWorkInfoN.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridWorkInfoN.SetItemState(i, j, GVIS_READONLY);
			m_pGridWorkInfoN.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
		}
	}

	row = 0;
	m_pGridWorkInfoN.MergeCells(row, 0, row, 1);
	m_pGridWorkInfoN.SetItemBkColour(row, 0, RGB(219, 229, 241), BLACK_C);
	m_pGridWorkInfoN.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridWorkInfoN.SetItemText(row, 0, _T("Next Lot Info"));

	row = 1;
	m_pGridWorkInfoN.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
	m_pGridWorkInfoN.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridWorkInfoN.SetItemText(row, 0, _T("Lot No"));

	m_pGridWorkInfoN.SetItemBkColour(row, 1, WHITE_C, BLACK_C);
	m_pGridWorkInfoN.SetItemFont(row, 1, &clsFunc.OnLogFont(13));
	m_pGridWorkInfoN.SetItemText(row, 1, _T("-"));

	row = 2;
	m_pGridWorkInfoN.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
	m_pGridWorkInfoN.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridWorkInfoN.SetItemText(row, 0, _T("Part No"));

	m_pGridWorkInfoN.SetItemBkColour(row, 1, WHITE_C, BLACK_C);
	m_pGridWorkInfoN.SetItemFont(row, 1, &clsFunc.OnLogFont(11));
	m_pGridWorkInfoN.SetItemText(row, 1, _T("-"));
}

void CScreenMain::OnTimer(UINT_PTR nIDEvent)
{
		switch(nIDEvent)
		{
			case TM_SCREEN_DISPLAY:
				OnMainLdTray();
				OnMainUnLdTrayFirst();
				OnMainUnLdTraySecond();
				OnMainRejectTrayFirst();
				OnMainRejectTraySecond();
				OnMainTrayRobot();
				OnMainLdBuf();
				OnMainUnLdBuf();
				OnMainTestSiteRobot();
				OnMainRetestBufFirst();
				OnMainRetestBufSecond();
				OnMainTestSiteTop();
				OnMainTestSiteBottom();
				OnMainYieldDay();
				OnMainCountDisplay();
/*
				if (m_nLotYieldDisplayChange)
				{
					OnMainYieldLot();
				}
				else
				{
					OnMainNextLotDataDisplay();
				}*/
				break;
		}
		

	CFormView::OnTimer(nIDEvent);
}

void CScreenMain::OnMainTimeDisplay()
{
//	int nState;
	static int nOldTime = 0;
	int nMtbi = 0;
	double dTemp;
	CString strTime;

	strTime.Format(_T("%02d:%02d:%02d"), st_handler_info.tStop.GetHours(), st_handler_info.tStop.GetMinutes(), st_handler_info.tStop.GetSeconds());
	m_pGridTimeInfo.SetItemText(1, 1, strTime);
	
	strTime.Format(_T("%02d:%02d:%02d"), st_handler_info.tRun.GetHours(), st_handler_info.tRun.GetMinutes(), st_handler_info.tRun.GetSeconds());
	m_pGridTimeInfo.SetItemText(0, 1, strTime);

	nMtbi = (int)st_handler_info.tRun.GetTotalHours() * 3600 + (int)st_handler_info.tRun.GetMinutes() * 60 + (int)st_handler_info.tRun.GetSeconds();
	if (st_alarm_info.nAlarmNum > 0)
	{
		nMtbi = nMtbi / st_alarm_info.nAlarmNum;
	}

	strTime.Format(_T("%02d:%02d:%02d"), nMtbi / 3600, (nMtbi % 3600) / 60, nMtbi % 60 );
	m_pGridTimeInfo.SetItemText(3, 1, strTime);
	
	strTime.Format(_T("%02d:%02d:%02d"), st_handler_info.tJam.GetHours(), st_handler_info.tJam.GetMinutes(), st_handler_info.tJam.GetSeconds());
	m_pGridTimeInfo.SetItemText(2, 1, strTime);

	if (st_work_info.nPgmInfo == 1)
	{
		if (st_count_info.nPassCount[1][0] <= 0) return;
		if (((int)st_handler_info.tRun.GetTotalHours() * 60 + (int)st_handler_info.tRun.GetMinutes()) <= 0) return;

		if (nOldTime != (int)(st_handler_info.tRef.GetHour()))
		{
			nOldTime = (int)(st_handler_info.tRef.GetHour() - 1);

			if (nOldTime == -1) nOldTime = 23;

			m_pGridTimeInfo.Invalidate(FALSE);
		}

		//dTemp = (double)((int)st_handler_info.tRun.GetTotalHours() * 60 + (int)st_handler_info.tRun.GetMinutes()) / 60.0f;
		//strTime.Format(_T("%d / %0.2f"), st_count_info.nLatestUph[nOldTime] ,double(st_count_info.nPassCount[1][0] / dTemp));
		//strTime.Format(_T("%0.2f"), double(st_count_info.nPassCount[1][0] / dTemp));
		dTemp = 0.0f;
		if (st_count_info.nDailyUph > 0 && st_count_info.nUphCnt > 0)
		{
			dTemp = (double)st_count_info.nDailyUph / (double)st_count_info.nUphCnt;
		}
		strTime.Format(_T("%d / %0.2f"), st_count_info.nUph , dTemp);
		m_pGridTimeInfo.SetItemText(4, 1, strTime);

		if (st_work_info.nCycleCount > 0)
		{
			dTemp = (double)st_work_info.dwLotCycle / (double)st_work_info.nCycleCount;

			strTime.Format(_T("%d / %0.2f"), st_work_info.dwTrayCycle , dTemp);
			m_pGridTimeInfo.SetItemText(5, 1, strTime);
		}
		else
		{
			strTime.Format(_T("%d / %0.2f"), st_work_info.dwTrayCycle , 0.0);
			m_pGridTimeInfo.SetItemText(5, 1, strTime);
		}
	}



	m_pGridTimeInfo.Invalidate(FALSE);
}

void CScreenMain::OnMainCountDisplay()
{
	CString strTemp;

	double dAve[2];

	if (st_lot_info[LOT_CURR].nLotStatus >= LOT_START && st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)
	{
		// lot 수량.
		strTemp.Format(_T("%d / %d"), st_count_info.nInCount[0][0], st_count_info.nInCount[0][1]);
		m_pGridYieldLot.SetItemText(2, 1, strTemp);

		strTemp.Format(_T("%d / %d"), st_count_info.nPrimeCount[0][0], st_count_info.nPrimeCount[0][1]);
		m_pGridYieldLot.SetItemText(3, 1, strTemp);

		strTemp.Format(_T("%d / %d"), st_count_info.nPassCount[0][0], st_count_info.nPassCount[0][1]);
		m_pGridYieldLot.SetItemText(3, 2, strTemp);

		// jtkim 20150307
		strTemp.Format(_T("%d / %d"), st_count_info.nPrimeRejectCount[0][0], st_count_info.nPrimeRejectCount[0][1]);
		m_pGridYieldLot.SetItemText(4, 1, strTemp);

		strTemp.Format(_T("%d / %d"), st_count_info.nRejectCount[0][0], st_count_info.nRejectCount[0][1]);
		m_pGridYieldLot.SetItemText(4, 2, strTemp);

		dAve[0] = dAve[1] = 0.0f;
		if (st_count_info.nPrimeCount[0][0] > 0) 
		{
			//dAve = ((double)st_count_info.nPrimeCount[0][0] / (double)st_count_info.nInCount[0][0]) * (double)100;
			dAve[0] = ((double)st_count_info.nPrimeCount[0][0] / ((double)st_count_info.nPrimeCount[0][0] + (double)st_count_info.nPrimeRejectCount[0][0])) * (double)100;
		}

		if (st_count_info.nPrimeCount[0][1] > 0) 
		{
			//dAve = ((double)st_count_info.nPrimeCount[0][0] / (double)st_count_info.nInCount[0][0]) * (double)100;
			dAve[1] = ((double)st_count_info.nPrimeCount[0][1] / ((double)st_count_info.nPrimeCount[0][1] + (double)st_count_info.nPrimeRejectCount[0][1])) * (double)100;
		}
		strTemp.Format(_T("%.2f / %.2f"), dAve[0], dAve[1]);
		m_pGridYieldLot.SetItemText(5, 1, strTemp);

		if (st_count_info.nPassCount[0][0] > 0) 
		{
			//dAve = ((double)st_count_info.nPassCount[0][0] / (double)st_count_info.nInCount[0][0]) * (double)100;
			dAve[0] = ((double)st_count_info.nPassCount[0][0] / ((double)st_count_info.nPassCount[0][0] + (double)st_count_info.nRejectCount[0][0])) * (double)100;
		}

		if (st_count_info.nPassCount[0][1] > 0) 
		{
			//dAve = ((double)st_count_info.nPassCount[0][0] / (double)st_count_info.nInCount[0][0]) * (double)100;
			dAve[1] = ((double)st_count_info.nPassCount[0][1] / ((double)st_count_info.nPassCount[0][1] + (double)st_count_info.nRejectCount[0][1])) * (double)100;
		}

		strTemp.Format(_T("%.2f / %.2f"), dAve[0], dAve[1]);
		m_pGridYieldLot.SetItemText(5, 2, strTemp);
	}
	else
	{
		// lot 수량.
		strTemp.Format(_T("%d"), st_count_info.nInCount[0][0]);
		m_pGridYieldLot.SetItemText(2, 1, strTemp);

		strTemp.Format(_T("%d"), st_count_info.nPrimeCount[0][0]);
		m_pGridYieldLot.SetItemText(3, 1, strTemp);

		strTemp.Format(_T("%d"), st_count_info.nPassCount[0][0]);
		m_pGridYieldLot.SetItemText(3, 2, strTemp);

		// jtkim 20150307
		strTemp.Format(_T("%d"), st_count_info.nPrimeRejectCount[0][0]);
		m_pGridYieldLot.SetItemText(4, 1, strTemp);

		strTemp.Format(_T("%d"), st_count_info.nRejectCount[0][0]);
		m_pGridYieldLot.SetItemText(4, 2, strTemp);

		if (st_count_info.nPrimeCount[0][0] > 0) 
		{
			//dAve = ((double)st_count_info.nPrimeCount[0][0] / (double)st_count_info.nInCount[0][0]) * (double)100;
			dAve[0] = ((double)st_count_info.nPrimeCount[0][0] / ((double)st_count_info.nPrimeCount[0][0] + (double)st_count_info.nPrimeRejectCount[0][0])) * (double)100;
			strTemp.Format(_T("%.2f"), dAve[0]);
			m_pGridYieldLot.SetItemText(5, 1, strTemp);
		}

		if (st_count_info.nPassCount[0][0] > 0) 
		{
			//dAve = ((double)st_count_info.nPassCount[0][0] / (double)st_count_info.nInCount[0][0]) * (double)100;
			dAve[0] = ((double)st_count_info.nPassCount[0][0] / ((double)st_count_info.nPassCount[0][0] + (double)st_count_info.nRejectCount[0][0])) * (double)100;
			strTemp.Format(_T("%.2f"), dAve[0]);
			m_pGridYieldLot.SetItemText(5, 2, strTemp);
		}
	}
	// daily 수량.
	
	strTemp.Format(_T("%d"), st_count_info.nInCount[1][0]);
	m_pGridYieldDay.SetItemText(2, 1, strTemp);

	strTemp.Format(_T("%d"), st_count_info.nPrimeCount[1][0]);
	m_pGridYieldDay.SetItemText(3, 1, strTemp);

	strTemp.Format(_T("%d"), st_count_info.nPassCount[1][0]);
	m_pGridYieldDay.SetItemText(3, 2, strTemp);

	// jtkim 20150307
	strTemp.Format(_T("%d"), st_count_info.nPrimeRejectCount[1][0]);
	m_pGridYieldDay.SetItemText(4, 1, strTemp);

	strTemp.Format(_T("%d"), st_count_info.nRejectCount[1][0]);
	m_pGridYieldDay.SetItemText(4, 2, strTemp);

	if (st_count_info.nPrimeCount[1][0] > 0) 
	{
		//dAve = ((double)st_count_info.nPrimeCount[1][0] / (double)st_count_info.nInCount[1][0]) * (double)100;
		dAve[0] = ((double)st_count_info.nPrimeCount[1][0] / ((double)st_count_info.nPrimeCount[1][0] + (double)st_count_info.nPrimeRejectCount[1][0])) * (double)100;
		strTemp.Format(_T("%.2f"), dAve[0]);
		m_pGridYieldDay.SetItemText(5, 1, strTemp);
	}

	if (st_count_info.nPassCount[1][0] > 0) 
	{
		//dAve = ((double)st_count_info.nPassCount[1][0] / (double)st_count_info.nInCount[1][0]) * (double)100;
		dAve[0] = ((double)st_count_info.nPassCount[1][0] / ((double)st_count_info.nPassCount[1][0] + (double)st_count_info.nRejectCount[1][0])) * (double)100;
		strTemp.Format(_T("%.2f"), dAve[0]);
		m_pGridYieldDay.SetItemText(5, 2, strTemp);
	}

	m_pGridYieldLot.Invalidate(FALSE);
	m_pGridYieldDay.Invalidate(FALSE);
}

//kwlee 2016.0804
void CScreenMain::OnMainLotDisplay()
{
	CString strTemp;

	double dAve;

	strTemp.Format(_T("%s (%d)"), st_lot_info[LOT_CURR].strLotNo, st_lot_info[LOT_CURR].nQty);
	m_pGridWorkInfoC.SetItemText(1, 1, strTemp);
	strTemp.Format(_T("%s (%d)"), st_lot_info[LOT_NEXT].strLotNo, st_lot_info[LOT_NEXT].nQty);
	m_pGridWorkInfoN.SetItemText(1, 1, strTemp);

	m_pGridWorkInfoC.SetItemText(2, 1, st_lot_info[LOT_CURR].strPartNo);
	m_pGridWorkInfoN.SetItemText(2, 1, st_lot_info[LOT_NEXT].strPartNo);

	if (st_lot_info[LOT_CURR].nLastModule == YES)
	{
		m_pGridWorkInfoC.SetItemText(3, 1, _T("YES"));
	}
	else
	{
		m_pGridWorkInfoC.SetItemText(3, 1, _T("NO"));
	}

	strTemp.Format(_T("%.2f / %d"), st_count_info.dHourPer, st_count_info.nHourSocket);
	m_pGridWorkInfoC.SetItemText(4, 1, strTemp);

	dAve = 0.0f;
	if (st_count_info.nUphCnt > 0)
	{
		dAve = st_count_info.dDailyPer / (double)st_count_info.nUphCnt;
	}
	strTemp.Format(_T("%.2f"), dAve);
	m_pGridWorkInfoC.SetItemText(5, 1, strTemp);

	if (st_buffer_info[THD_RETEST_1_BUFF].strLotNo != _T(""))
	{
		m_msgRetestBufferFirst.SetWindowText(st_buffer_info[THD_RETEST_1_BUFF].strLotNo);
	}

	if (st_buffer_info[THD_RETEST_2_BUFF].strLotNo != _T(""))
	{
		m_msgRetestBufferSecond.SetWindowText(st_buffer_info[THD_RETEST_2_BUFF].strLotNo);
	}

	if (st_tray_info[THD_REJECT_OUT_1_STACKER].strLotNo != _T(""))
	{
		m_pGridRejectTrayFirst.SetItemText(0, 0, st_tray_info[THD_REJECT_OUT_1_STACKER].strLotNo);
	}

	if (st_tray_info[THD_REJECT_OUT_2_STACKER].strLotNo != _T(""))
	{
		m_pGridRejectTraySecond.SetItemText(0, 0, st_tray_info[THD_REJECT_OUT_2_STACKER].strLotNo);
	}
		
	if (st_tray_info[THD_LD_TRAY_PLATE].strLotNo != _T(""))
	{
		m_pGridLdTray.SetItemText(0, 0, st_tray_info[THD_LD_TRAY_PLATE].strLotNo);
	}

	if (st_tray_info[THD_ULD_1_STACKER].strLotNo != _T(""))
	{
		m_pGridUnLdTrayFirst.SetItemText(0, 0, st_tray_info[THD_ULD_1_STACKER].strLotNo);
		
	}

	if (st_tray_info[THD_ULD_2_STACKER].strLotNo != _T(""))
	{
		m_pGridUnLdTraySecond.SetItemText(0, 0, st_tray_info[THD_ULD_2_STACKER].strLotNo);
	}

	m_pGridUnLdTraySecond.Invalidate(FALSE);
	m_pGridUnLdTrayFirst.Invalidate(FALSE);
	m_pGridLdTray.Invalidate(FALSE);
	m_pGridRejectTraySecond.Invalidate(FALSE);
	m_pGridRejectTrayFirst.Invalidate(FALSE);
	m_pGridWorkInfoC.Invalidate(FALSE);
	m_pGridWorkInfoN.Invalidate(FALSE);
}
////////

void CScreenMain::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CFormView::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CScreenMain::OnTimeCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	int nRow, nCol;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	if (st_work_info.nPgmInfo == 0)
	{
		st_work_info.nPgmInfo = 1;
	}
	else
	{
		st_work_info.nPgmInfo = 0;
	}
	OnInitGridTimeInfo(st_work_info.nPgmInfo);
}


void CScreenMain::OnWorkInfoCurrCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	if (st_handler_info.nRunStatus != dSTOP) return;
	//kwlee 2016.08.04
	CSlotInfo dlgData;

	return;

	if (dlgData.DoModal() == IDOK)
	{
	//	OnMainLotDisplay();
	}

}
////////////////////kwlee 2016.0912
void CScreenMain::OnLdTrayCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CDialog_Message dlgMsg;
	CString strMsg;
	int nRow, nCol;

	if (st_handler_info.nRunStatus != dSTOP) return;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	if (nRow == 0)return;

	//kwlee 2017.0126
	return;
	if(st_tray_info[THD_LD_TRAY_PLATE].st_pcb_info[2- nRow][nCol].nYesNo == CTL_YES)
	{
		dlgMsg.m_nMessageType = 1;
		dlgMsg.m_strMessage.Format(_T("Do you want a Device Skip?"));

		if (dlgMsg.DoModal() == IDOK)
		{
			st_tray_info[THD_LD_TRAY_PLATE].st_pcb_info[2 - nRow][nCol].nYesNo = NO;
			//kwlee 2017.0224
			strMsg.Format(_T("Ld_Tray %d Device Skip"),(2 - nRow) + 1);
			clsMem.OnAbNormalMessagWrite(strMsg);//로그 저장
		}
	}
	else
	{
		strMsg.Format(_T("Load Tray NO Device"));
		AfxMessageBox(strMsg);
	}

	m_pGridLdTray.Invalidate(FALSE);

}

void CScreenMain::OnTrayRobotCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CDialog_Message dlgMsg;
	CString strMsg;
	int nRow, nCol;

	if (st_handler_info.nRunStatus != dSTOP) return;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	//kwlee 2017.0126
	return;

	if(st_picker[THD_WORK_RBT].st_pcb_info[nRow].nYesNo == CTL_YES)
	{
		dlgMsg.m_nMessageType = 1;
		dlgMsg.m_strMessage.Format(_T("Do you want a [%02d] Device Skip?"),nCol);

		if (dlgMsg.DoModal() == IDOK)
		{
			st_picker[THD_WORK_RBT].st_pcb_info[nRow].nYesNo = CTL_NO;
			//kwlee 2017.0224
			strMsg.Format(_T("Work Robot %d Device Skip"),nRow + 1);
			clsMem.OnAbNormalMessagWrite(strMsg);//로그 저장
		}
	}
	else
	{
		strMsg.Format(_T("WORK Robot [%02d] NO Device"),nCol + 1);
		AfxMessageBox(strMsg);
	}

}

void CScreenMain::OnUnLdTrayFirstCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CDialog_Message dlgMsg;
	CString strMsg;
	int nRow, nCol;

	if (st_handler_info.nRunStatus != dSTOP) return;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	if (nRow == 0)return;

	//kwlee 2017.0126
	return;

	if(st_tray_info[THD_ULD_1_STACKER].st_pcb_info[nRow -2][nCol].nYesNo == CTL_YES)
	{
		dlgMsg.m_nMessageType = 1;
		dlgMsg.m_strMessage.Format(_T("Do you want a Device Skip?"));

		if (dlgMsg.DoModal() == IDOK)
		{
			st_tray_info[THD_ULD_1_STACKER].st_pcb_info[nRow -2][nCol].nYesNo = NO;
			//kwlee 2017.0224
			strMsg.Format(_T("Unload#1 %d Device Skip"),(nRow - 2) + 1);
			clsMem.OnAbNormalMessagWrite(strMsg);//로그 저장
		}
	}
	else
	{
		strMsg.Format(_T("Unload_1 Tray NO Device"));
		AfxMessageBox(strMsg);
	}


}

void CScreenMain::OnUnLdTraySecondCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CDialog_Message dlgMsg;
	CString strMsg;
	int nRow, nCol;

	if (st_handler_info.nRunStatus != dSTOP) return;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	if (nRow == 0)return;

	//kwlee 2017.0126
	return;

	if(st_tray_info[THD_ULD_2_STACKER].st_pcb_info[nRow -2][nCol].nYesNo == CTL_YES)
	{
		dlgMsg.m_nMessageType = 1;
		dlgMsg.m_strMessage.Format(_T("Do you want a Device Skip?"));

		if (dlgMsg.DoModal() == IDOK)
		{
			//kwlee 2017.0224
			strMsg.Format(_T("Unload#2 %d Device Skip"),(nRow - 2) + 1);
			clsMem.OnAbNormalMessagWrite(strMsg);//로그 저장
			st_tray_info[THD_ULD_2_STACKER].st_pcb_info[nRow -2][nCol].nYesNo = NO;
		}
	}
	else
	{
		strMsg.Format(_T("Unload_2 Tray NO Device"));
		AfxMessageBox(strMsg);
	}

}

void CScreenMain::OnLdBufCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CDialog_Message dlgMsg;
	CString strMsg;
	int nRow, nCol;

	if (st_handler_info.nRunStatus != dSTOP) return;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;
	//kwlee 2017.0126
	return;

	if(st_buffer_info[THD_LD_BUFF].st_pcb_info[nCol].nYesNo == CTL_YES)
	{
		dlgMsg.m_nMessageType = 1;
		dlgMsg.m_strMessage.Format(_T("Do you want a[%d] Device Skip?"),nCol);

		if (dlgMsg.DoModal() == IDOK)
		{
			st_buffer_info[THD_LD_BUFF].st_pcb_info[nCol].nYesNo = NO;
			//kwlee 2017.0224
			strMsg.Format(_T("Ld_Buff %d Device Skip"),nCol + 1);
			clsMem.OnAbNormalMessagWrite(strMsg);//로그 저장
		}
	}
	else
	{
		strMsg.Format(_T("Load Buffer NO Device"));
		AfxMessageBox(strMsg);
	}
}

void CScreenMain::OnUnLdBufCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CDialog_Message dlgMsg;
	CString strMsg;
	int nRow, nCol;

	if (st_handler_info.nRunStatus != dSTOP) return;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;
	//kwlee 2017.0126
	return;
	if(st_buffer_info[THD_ULD_BUFF].st_pcb_info[nCol].nYesNo == CTL_YES)
	{
		dlgMsg.m_nMessageType = 1;
		dlgMsg.m_strMessage.Format(_T("Do you want a [%d] Device Skip?"),nCol);

		if (dlgMsg.DoModal() == IDOK)
		{
			st_buffer_info[THD_ULD_BUFF].st_pcb_info[nCol].nYesNo = NO;
			//kwlee 2017.0224
			strMsg.Format(_T("ULd_Buff %d Device Skip"),nCol + 1);
			clsMem.OnAbNormalMessagWrite(strMsg);//로그 저장
		}
	}
	else
	{
		strMsg.Format(_T("Unload Buffer NO Device"));
		AfxMessageBox(strMsg);
	}

}

void CScreenMain::OnRejectTrayFirstCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CDialog_Message dlgMsg;
	CString strMsg;
	int nRow, nCol;

	if (st_handler_info.nRunStatus != dSTOP) return;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	if (nRow == 0)return;

	if(st_tray_info[THD_REJECT_OUT_1_STACKER].st_pcb_info[nRow -2][nCol].nYesNo == CTL_YES)
	{
		dlgMsg.m_nMessageType = 1;
		dlgMsg.m_strMessage.Format(_T("Do you want a Device Skip?"));

		if (dlgMsg.DoModal() == IDOK)
		{
			st_tray_info[THD_REJECT_OUT_1_STACKER].st_pcb_info[nRow -2][nCol].nYesNo = NO;
		}
	}
	else
	{
		strMsg.Format(_T("Reject_1 Tray NO Device"));
		AfxMessageBox(strMsg);
	}

}

void CScreenMain::OnRejectTraySecondCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CDialog_Message dlgMsg;
	CString strMsg;
	int nRow, nCol;

	if (st_handler_info.nRunStatus != dSTOP) return;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	if (nRow == 0)return;

	if(st_tray_info[THD_REJECT_OUT_2_STACKER].st_pcb_info[nRow -2][nCol].nYesNo == CTL_YES)
	{
		dlgMsg.m_nMessageType = 1;
		dlgMsg.m_strMessage.Format(_T("Do you want a Device Skip?"));

		if (dlgMsg.DoModal() == IDOK)
		{
			st_tray_info[THD_REJECT_OUT_2_STACKER].st_pcb_info[nRow -2][nCol].nYesNo = NO;
		}
	}
	else
	{
		strMsg.Format(_T("Reject_2 Tray NO Device"));
		AfxMessageBox(strMsg);
	}

}

void CScreenMain::OnRetestBufFirstCellClick1(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CDialog_Message dlgMsg;
	CString strMsg;
	int nRow, nCol;

	if (st_handler_info.nRunStatus != dSTOP) return;
	
	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	if (nRow == 0)return;

	if(st_buffer_info[THD_RETEST_1_BUFF].st_pcb_info[nRow - 1].nYesNo == CTL_YES)
	{
		dlgMsg.m_nMessageType = 1;
		dlgMsg.m_strMessage.Format(_T("Do you want a [%2d]Device Skip?"), nRow);

		if (dlgMsg.DoModal() == IDOK)
		{
			st_buffer_info[THD_RETEST_1_BUFF].st_pcb_info[nRow - 1].nYesNo = NO;
		}
	}
	else
	{
		strMsg.Format(_T("[%02d]Retest Buffer_1 NO Device"), nRow);
		AfxMessageBox(strMsg);
	}
}

// void CScreenMain::OnRetestBufFirstCellClick2(NMHDR *pNotifyStruct, LRESULT* pResult)
// {
// 	CDialog_Message dlgMsg;
// 	CString strMsg;
// 	int nRow, nCol;
// 
// 	if (st_handler_info.nRunStatus != dSTOP) return;
// 
// 	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;
// 
// 	nRow				= pItem->iRow;
// 	nCol				= pItem->iColumn;
// 
// }

void CScreenMain::OnRetestBufSecondCellClick1(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CDialog_Message dlgMsg;
	CString strMsg;
	int nRow, nCol;

	if (st_handler_info.nRunStatus != dSTOP) return;
	

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	if (nRow == 0)return;

	if(st_buffer_info[THD_RETEST_2_BUFF].st_pcb_info[nRow - 1].nYesNo == CTL_YES)
	{
		dlgMsg.m_nMessageType = 1;
		dlgMsg.m_strMessage.Format(_T("Do you want a [%2d]Device Skip?"), nRow);

		if (dlgMsg.DoModal() == IDOK)
		{
			st_buffer_info[THD_RETEST_2_BUFF].st_pcb_info[nRow - 1].nYesNo = NO;
		}
	}
	else
	{
		strMsg.Format(_T("[%02d]Retest Buffer_2 NO Device"), nRow);
		AfxMessageBox(strMsg);
	}

}

// void CScreenMain::OnRetestBufSecondCellClick2(NMHDR *pNotifyStruct, LRESULT* pResult)
// {
// 	CDialog_Message dlgMsg;
// 	CString strMsg;
// 	int nRow, nCol;
// 
// 	if (st_handler_info.nRunStatus != dSTOP) return;
// 
// 	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;
// 
// 	nRow				= pItem->iRow;
// 	nCol				= pItem->iColumn;
// 
// }

void CScreenMain::OnTestSiteRobotCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CDialog_Message dlgMsg;
	CString strMsg;
	int nRow, nCol;

	if (st_handler_info.nRunStatus != dSTOP) return;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	//kwlee 2017.0126
	return;
	
	if(st_picker[THD_TEST_RBT].st_pcb_info[nCol].nYesNo == CTL_YES)
	{
		dlgMsg.m_nMessageType = 1;
		dlgMsg.m_strMessage.Format(_T("Do you want a [%02d] Device Skip?"),nCol);

		if (dlgMsg.DoModal() == IDOK)
		{
			st_picker[THD_TEST_RBT].st_pcb_info[nCol].nYesNo = CTL_NO;
		}
	}
	else
	{
		strMsg.Format(_T("Test Site Robot [%02d] NO Device"),nCol + 1);
		AfxMessageBox(strMsg);
	}

}



void CScreenMain::OnTestSite1CellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{

	CDialog_Message dlgMsg;
	CString strMsg;
	int nRow, nCol;

	if (st_handler_info.nRunStatus != dSTOP) return;
	
	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	

	if (nCol ==0 )return;
	if (nRow ==0 || nRow ==1)return;
	
	if (st_test_site_info[THD_TESTSITE_1].st_pcb_info[9 - nRow].nEnable != YES) return;

	if(st_test_site_info[THD_TESTSITE_1].st_pcb_info[9 - nRow].nYesNo == YES)
	{
		dlgMsg.m_nMessageType = 1;
		dlgMsg.m_strMessage.Format(_T("Do you want a [01]Test Site [%d]Socket Device Skip?"),9 - nRow);

		if (dlgMsg.DoModal() == IDOK)
		{
			//st_test_site_info[THD_TESTSITE_1].st_pcb_info[nRow - 2].nSkipMode = YES;
			st_test_site_info[THD_TESTSITE_1].st_pcb_info[9 - nRow].nYesNo = NO;
		}
	}
	else
	{
		strMsg.Format(_T("Do you want a [01]Test Site [%d]Socket NO Device"),9 - nRow);
		AfxMessageBox(strMsg);
	}
}

void CScreenMain::OnTestSite2CellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CDialog_Message dlgMsg;
	CString strMsg;
	int nRow, nCol;

	if (st_handler_info.nRunStatus != dSTOP) return;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	

	if (nCol ==0 )return;
	if (nRow ==0 || nRow ==1)return;

	if (st_test_site_info[THD_TESTSITE_2].st_pcb_info[9 - nRow].nEnable != YES) return;

	if(st_test_site_info[THD_TESTSITE_2].st_pcb_info[9 - nRow].nYesNo == YES)
	{
		dlgMsg.m_nMessageType = 1;
		dlgMsg.m_strMessage.Format(_T("Do you want a [02]Test Site [%d]Socket Device Skip?"),9 - nRow);

		if (dlgMsg.DoModal() == IDOK)
		{
		
			//st_test_site_info[THD_TESTSITE_2].st_pcb_info[nRow - 2].nSkipMode = YES;
			st_test_site_info[THD_TESTSITE_2].st_pcb_info[9 - nRow].nYesNo = NO;
		}
	}
	else
	{
		strMsg.Format(_T("Do you want a [02]Test Site [%d]Socket NO Device"),9 - nRow);
		AfxMessageBox(strMsg);
	}

}

void CScreenMain::OnTestSite3CellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CDialog_Message dlgMsg;
	CString strMsg;
	int nRow, nCol;

	if (st_handler_info.nRunStatus != dSTOP) return;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;
	
	if (nCol ==0 )return;
	if (nRow ==0 || nRow ==1)return;



	if (st_test_site_info[THD_TESTSITE_3].st_pcb_info[9 - nRow].nEnable != YES) return;

	if(st_test_site_info[THD_TESTSITE_3].st_pcb_info[9 - nRow].nYesNo == YES)
	{
		dlgMsg.m_nMessageType = 1;
		dlgMsg.m_strMessage.Format(_T("Do you want a [03]Test Site [%d]Socket Device Skip?"),9 - nRow);

		if (dlgMsg.DoModal() == IDOK)
		{
		
			//st_test_site_info[THD_TESTSITE_3].st_pcb_info[nRow - 2].nSkipMode = YES;
			st_test_site_info[THD_TESTSITE_3].st_pcb_info[9 - nRow].nYesNo = NO;
		}
	}
	else
	{
		strMsg.Format(_T("Do you want a [03]Test Site [%d]Socket NO Device"),9 - nRow);
		AfxMessageBox(strMsg);
	}
}

void CScreenMain::OnTestSite4CellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CDialog_Message dlgMsg;
	CString strMsg;
	int nRow, nCol;

	if (st_handler_info.nRunStatus != dSTOP) return;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;
	
	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;


	if (nCol ==0 )return;
	if (nRow ==0 || nRow ==1)return;

	if (st_test_site_info[THD_TESTSITE_4].st_pcb_info[9 - nRow].nEnable != YES) return;

	if(st_test_site_info[THD_TESTSITE_4].st_pcb_info[9 - nRow].nYesNo == YES)
	{
		dlgMsg.m_nMessageType = 1;
		dlgMsg.m_strMessage.Format(_T("Do you want a [04]Test Site [%d]Socket Device Skip?"),9 - nRow);

		if (dlgMsg.DoModal() == IDOK)
		{
		
			//st_test_site_info[THD_TESTSITE_4].st_pcb_info[nRow - 2].nSkipMode = YES;
			st_test_site_info[THD_TESTSITE_4].st_pcb_info[9 - nRow].nYesNo = NO;
		}
	}
	else
	{
		strMsg.Format(_T("Do you want a [04]Test Site [%d]Socket NO Device"),9 - nRow);
		AfxMessageBox(strMsg);
	}
}

void CScreenMain::OnTestSite5CellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CDialog_Message dlgMsg;
	CString strMsg;
	int nRow, nCol;

	if (st_handler_info.nRunStatus != dSTOP) return;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;
	
	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	if (nCol ==0 )return;
	if (nRow ==0 || nRow ==1)return;

	if (st_test_site_info[THD_TESTSITE_5].st_pcb_info[nRow - 2].nEnable != YES) return;
	
	if(st_test_site_info[THD_TESTSITE_5].st_pcb_info[nRow - 2].nYesNo == YES)
	{
		dlgMsg.m_nMessageType = 1;
		dlgMsg.m_strMessage.Format(_T("Do you want a [05]Test Site [%d]Socket Device Skip?"),nRow -1);

		if (dlgMsg.DoModal() == IDOK)
		{
		
			//st_test_site_info[THD_TESTSITE_5].st_pcb_info[nRow - 2].nSkipMode = YES;
			st_test_site_info[THD_TESTSITE_5].st_pcb_info[nRow - 2].nYesNo = NO;
		}
	}
	else
	{
		strMsg.Format(_T("Do you want a [05]Test Site [%d]Socket NO Device"),nRow -1);
		AfxMessageBox(strMsg);
	}
}

void CScreenMain::OnTestSite6CellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CDialog_Message dlgMsg;
	CString strMsg;
	int nRow, nCol;

	if (st_handler_info.nRunStatus != dSTOP) return;
	
	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;
	
	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	
	if (nCol ==0 )return;
	if (nRow ==0 || nRow ==1)return;

	if (st_test_site_info[THD_TESTSITE_6].st_pcb_info[nRow - 2].nEnable != YES) return;

	
	if(st_test_site_info[THD_TESTSITE_6].st_pcb_info[nRow - 2].nYesNo == YES)
	{
		dlgMsg.m_nMessageType = 1;
		dlgMsg.m_strMessage.Format(_T("Do you want a [06]Test Site [%d]Socket Device Skip?"),nRow -1);

		if (dlgMsg.DoModal() == IDOK)
		{
			//st_test_site_info[THD_TESTSITE_6].st_pcb_info[nRow - 2].nSkipMode = YES;
			st_test_site_info[THD_TESTSITE_6].st_pcb_info[nRow - 2].nYesNo = NO;

		}
	}
	else
	{
		strMsg.Format(_T("Do you want a [06]Test Site [%d]Socket NO Device"),nRow -1);
		AfxMessageBox(strMsg);
	}
}

void CScreenMain::OnTestSite7CellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CDialog_Message dlgMsg;
	CString strMsg;
	int nRow, nCol;

	if (st_handler_info.nRunStatus != dSTOP) return;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	if (nCol ==0 )return;
	if (nRow ==0 || nRow ==1)return;

	if (st_test_site_info[THD_TESTSITE_7].st_pcb_info[nRow - 2].nEnable != YES) return;
	if(st_test_site_info[THD_TESTSITE_7].st_pcb_info[nRow - 2].nYesNo == YES)
	{
		dlgMsg.m_nMessageType = 1;
		dlgMsg.m_strMessage.Format(_T("Do you want a [07]Test Site [%d]Socket Device Skip?"),nRow -1);

		if (dlgMsg.DoModal() == IDOK)
		{
		
			//st_test_site_info[THD_TESTSITE_7].st_pcb_info[nRow - 2].nSkipMode = YES;
			st_test_site_info[THD_TESTSITE_7].st_pcb_info[nRow - 2].nYesNo = NO;

		}
	}
	else
	{
		strMsg.Format(_T("Do you want a [07]Test Site [%d]Socket NO Device"),nRow -1);
		AfxMessageBox(strMsg);
	}
}

void CScreenMain::OnTestSite8CellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CDialog_Message dlgMsg;
	CString strMsg;
	int nRow, nCol;

	if (st_handler_info.nRunStatus != dSTOP) return;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	if (nCol ==0 )return;
	if (nRow ==0 || nRow ==1)return;

	if (st_test_site_info[THD_TESTSITE_8].st_pcb_info[nRow - 2].nEnable != YES) return;

	if(st_test_site_info[THD_TESTSITE_8].st_pcb_info[nRow - 2].nYesNo == YES)
	{
		dlgMsg.m_nMessageType = 1;
		dlgMsg.m_strMessage.Format(_T("Do you want a [08]Test Site [%d]Socket Device Skip?"), nRow - 1);

		if (dlgMsg.DoModal() == IDOK)
		{
			//st_test_site_info[THD_TESTSITE_8].st_pcb_info[nRow - 2].nSkipMode = YES;
			st_test_site_info[THD_TESTSITE_8].st_pcb_info[nRow - 2].nYesNo = NO;
		}
	}
	else
	{
		strMsg.Format(_T("Do you want a [08]Test Site [%d]Socket NO Device"),nRow -1);
		AfxMessageBox(strMsg);
	}
}


//////

void CScreenMain::OnLdTrayDCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	if (st_handler_info.nRunStatus != dSTOP) return;

	if (st_handler_info.cWndMain != NULL)
	{
		st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_PCB_TRAY_CREATE_MSG, THD_LD_TRAY_PLATE );
	}
}

void CScreenMain::OnUnLdTrayFirstDCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	if (st_handler_info.nRunStatus != dSTOP) return;
	if (st_handler_info.cWndMain != NULL)
	{
		st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_PCB_TRAY_CREATE_MSG, THD_ULD_1_STACKER );
	}
}

void CScreenMain::OnUnLdTraySecondDCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	if (st_handler_info.nRunStatus != dSTOP) return;

	if (st_handler_info.cWndMain != NULL)
	{
		st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_PCB_TRAY_CREATE_MSG, THD_ULD_2_STACKER );
	}
}

void CScreenMain::OnLdBufDCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	if (st_handler_info.nRunStatus != dSTOP) return;

	if (st_handler_info.cWndMain != NULL)
	{
		st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_PCB_PICKER_CREATE_MSG, THD_LD_BUFF );
	}
}

void CScreenMain::OnUnLdBufDCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	if (st_handler_info.nRunStatus != dSTOP) return;
	if (st_handler_info.cWndMain != NULL)
	{
		st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_PCB_PICKER_CREATE_MSG, THD_ULD_BUFF );
	}
}

void CScreenMain::OnRejectTrayFirstDCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	if (st_handler_info.nRunStatus != dSTOP) return;
	if (st_handler_info.cWndMain != NULL)
	{
		st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_PCB_TRAY_CREATE_MSG, THD_REJECT_OUT_1_STACKER );
	}
}

void CScreenMain::OnRejectTraySecondDCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	if (st_handler_info.nRunStatus != dSTOP) return;
	if (st_handler_info.cWndMain != NULL)
	{
		st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_PCB_TRAY_CREATE_MSG, THD_REJECT_OUT_2_STACKER );
	}
}

void CScreenMain::OnRetestBufFirstDCellClick1(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	if (st_handler_info.nRunStatus != dSTOP) return;
	if (st_handler_info.cWndMain != NULL)
	{
		st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_PCB_BUFFER_CREATE_MSG, THD_RETEST_1_BUFF );
	}
}

void CScreenMain::OnRetestBufFirstDCellClick2(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	if (st_handler_info.nRunStatus != dSTOP) return;
	if (st_handler_info.cWndMain != NULL)
	{
		st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_PCB_BUFFER_CREATE_MSG, THD_RETEST_1_BUFF );
	}
}

void CScreenMain::OnRetestBufSecondDCellClick1(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	if (st_handler_info.nRunStatus != dSTOP) return;
	if (st_handler_info.cWndMain != NULL)
	{
		st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_PCB_BUFFER_CREATE_MSG, THD_RETEST_2_BUFF );
	}
}

void CScreenMain::OnRetestBufSecondDCellClick2(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	if (st_handler_info.nRunStatus != dSTOP) return;
	if (st_handler_info.cWndMain != NULL)
	{
		st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_PCB_BUFFER_CREATE_MSG, THD_RETEST_2_BUFF );
	}
}

void CScreenMain::OnTestSiteRobotDCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	if (st_handler_info.nRunStatus != dSTOP) return;
	if (st_handler_info.cWndMain != NULL)
	{
		st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_PCB_PICKER_CREATE_MSG, THD_TEST_RBT );
	}
}
void CScreenMain::OnTestSite1DCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	if (st_handler_info.nRunStatus != dSTOP) return;

	if (st_handler_info.cWndMain != NULL)
	{
		st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_PCB_INTER_CREATE_MSG, 0);
	}
}

void CScreenMain::OnTestSite2DCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	if (st_handler_info.nRunStatus != dSTOP) return;

	if (st_handler_info.cWndMain != NULL)
	{
		st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_PCB_INTER_CREATE_MSG, 0);
	}
}

void CScreenMain::OnTestSite3DCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	if (st_handler_info.nRunStatus != dSTOP) return;

	if (st_handler_info.cWndMain != NULL)
	{
		st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_PCB_INTER_CREATE_MSG, 0);
	}

}

void CScreenMain::OnTestSite4DCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	if (st_handler_info.nRunStatus != dSTOP) return;
	if (st_handler_info.cWndMain != NULL)
	{
		st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_PCB_INTER_CREATE_MSG, 0);
	}
}

void CScreenMain::OnTestSite5DCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	if (st_handler_info.nRunStatus != dSTOP) return;

	if (st_handler_info.cWndMain != NULL)
	{
		st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_PCB_INTER_CREATE_MSG, 1);
	}
}

void CScreenMain::OnTestSite6DCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	if (st_handler_info.nRunStatus != dSTOP) return;

	if (st_handler_info.cWndMain != NULL)
	{
		st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_PCB_INTER_CREATE_MSG, 1);
	}

}

void CScreenMain::OnTestSite7DCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	if (st_handler_info.nRunStatus != dSTOP) return;

	if (st_handler_info.cWndMain != NULL)
	{
		st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_PCB_INTER_CREATE_MSG, 1);
	}
}

void CScreenMain::OnTestSite8DCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	if (st_handler_info.nRunStatus != dSTOP) return;

	if (st_handler_info.cWndMain != NULL)
	{
		st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_PCB_INTER_CREATE_MSG, 1);
	}
}
///////////////////////

void CScreenMain::OnTestSite1CellRClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CDialogPcb dlgPcb;

	int nRow, nCol;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	return; //kwlee 2017.0511

	if (st_handler_info.nRunStatus != dSTOP) return;	//2015.03.04 night


	if ((nRow-2) < 0) return;

	dlgPcb.m_nMode = 0;
	//nPos = nRow - 1;

	st_handler_info.nMenuLock = TRUE;

	dlgPcb.m_nY				= 10 - nRow;
	dlgPcb.m_pcb_info		= st_test_site_info[THD_TESTSITE_1].st_pcb_info[9 - nRow];

	if (dlgPcb.DoModal() == IDOK)
	{
		// jtkim 20150316
		OnPcbHistory(dlgPcb.m_nY, st_test_site_info[THD_TESTSITE_1].st_pcb_info[10 - nRow], dlgPcb.m_pcb_info);

		st_test_site_info[THD_TESTSITE_1].st_pcb_info[10 - nRow] = dlgPcb.m_pcb_info;
	}

	// jtkim20150626
	st_handler_info.nMenuLock = FALSE;

	OnMainDisplay();

}
void CScreenMain::OnTestSite2CellRClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CDialogPcb dlgPcb;

	int nRow, nCol;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	return; //kwlee 2017.0511

	if (st_handler_info.nRunStatus != dSTOP) return;	//2015.03.04 night


	if ((nRow-2) < 0) return;

	dlgPcb.m_nMode = 1;
	//nPos = nRow - 1;

	st_handler_info.nMenuLock = TRUE;

	dlgPcb.m_nY				= 10 - nRow;
	dlgPcb.m_pcb_info		= st_test_site_info[THD_TESTSITE_2].st_pcb_info[9 - nRow];

	if (dlgPcb.DoModal() == IDOK)
	{
		// jtkim 20150316
		OnPcbHistory(dlgPcb.m_nY, st_test_site_info[THD_TESTSITE_2].st_pcb_info[10 - nRow], dlgPcb.m_pcb_info);

		st_test_site_info[THD_TESTSITE_2].st_pcb_info[10 - nRow] = dlgPcb.m_pcb_info;
	}

	// jtkim20150626
	st_handler_info.nMenuLock = FALSE;
	
	OnMainDisplay();

}
void CScreenMain::OnTestSite3CellRClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CDialogPcb dlgPcb;

	int nRow, nCol;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	return; //kwlee 2017.0511

	if (st_handler_info.nRunStatus != dSTOP) return;	//2015.03.04 night


	if ((nRow-2) < 0) return;

	dlgPcb.m_nMode = 2;
	//nPos = nRow - 1;

	st_handler_info.nMenuLock = TRUE;

	dlgPcb.m_nY				= 10 - nRow;
	dlgPcb.m_pcb_info		= st_test_site_info[THD_TESTSITE_3].st_pcb_info[9 - nRow];

	if (dlgPcb.DoModal() == IDOK)
	{
		// jtkim 20150316
		OnPcbHistory(dlgPcb.m_nY, st_test_site_info[THD_TESTSITE_3].st_pcb_info[10 - nRow], dlgPcb.m_pcb_info);

		st_test_site_info[THD_TESTSITE_3].st_pcb_info[10 - nRow] = dlgPcb.m_pcb_info;
	}

	// jtkim20150626
	st_handler_info.nMenuLock = FALSE;

	OnMainDisplay();

}
void CScreenMain::OnTestSite4CellRClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CDialogPcb dlgPcb;

	int nRow, nCol;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	return; //kwlee 2017.0511

	if (st_handler_info.nRunStatus != dSTOP) return;	//2015.03.04 night


	if ((nRow-2) < 0) return;

	dlgPcb.m_nMode = 3;
	//nPos = nRow - 1;

	st_handler_info.nMenuLock = TRUE;

	dlgPcb.m_nY				= 10 - nRow;
	dlgPcb.m_pcb_info		= st_test_site_info[THD_TESTSITE_4].st_pcb_info[9 - nRow];

	if (dlgPcb.DoModal() == IDOK)
	{
		// jtkim 20150316
		OnPcbHistory(dlgPcb.m_nY, st_test_site_info[THD_TESTSITE_4].st_pcb_info[10 - nRow], dlgPcb.m_pcb_info);

		st_test_site_info[THD_TESTSITE_4].st_pcb_info[10 - nRow] = dlgPcb.m_pcb_info;
	}

	// jtkim20150626
	st_handler_info.nMenuLock = FALSE;

	OnMainDisplay();
}
void CScreenMain::OnTestSite5CellRClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CDialogPcb dlgPcb;

	int nRow, nCol;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	return; //kwlee 2017.0511

	if (st_handler_info.nRunStatus != dSTOP) return;	//2015.03.04 night


	if ((nRow-2) < 0) return;

	dlgPcb.m_nMode = 4;
	//nPos = nRow - 1;

	st_handler_info.nMenuLock = TRUE;

	dlgPcb.m_nY				= nRow - 1;
	dlgPcb.m_pcb_info		= st_test_site_info[THD_TESTSITE_5].st_pcb_info[nRow-2];

	if (dlgPcb.DoModal() == IDOK)
	{
		// jtkim 20150316
		OnPcbHistory(dlgPcb.m_nY, st_test_site_info[THD_TESTSITE_5].st_pcb_info[nRow-2], dlgPcb.m_pcb_info);

		st_test_site_info[THD_TESTSITE_5].st_pcb_info[nRow-2] = dlgPcb.m_pcb_info;
	}

	// jtkim20150626
	st_handler_info.nMenuLock = FALSE;

	OnMainDisplay();

}
void CScreenMain::OnTestSite6CellRClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CDialogPcb dlgPcb;

	int nRow, nCol;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	return; //kwlee 2017.0511

	if (st_handler_info.nRunStatus != dSTOP) return;	//2015.03.04 night


	if ((nRow-2) < 0) return;

	dlgPcb.m_nMode = 5;
	//nPos = nRow - 1;

	st_handler_info.nMenuLock = TRUE;

	dlgPcb.m_nY				= nRow - 1;
	dlgPcb.m_pcb_info		= st_test_site_info[THD_TESTSITE_6].st_pcb_info[nRow-2];

	if (dlgPcb.DoModal() == IDOK)
	{
		// jtkim 20150316
		OnPcbHistory(dlgPcb.m_nY, st_test_site_info[THD_TESTSITE_6].st_pcb_info[nRow-2], dlgPcb.m_pcb_info);

		st_test_site_info[THD_TESTSITE_6].st_pcb_info[nRow-2] = dlgPcb.m_pcb_info;
	}

	// jtkim20150626
	st_handler_info.nMenuLock = FALSE;

	OnMainDisplay();

}
void CScreenMain::OnTestSite7CellRClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CDialogPcb dlgPcb;

	int nRow, nCol;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	if (st_handler_info.nRunStatus != dSTOP) return;	//2015.03.04 night

	return; //kwlee 2017.0511

	if ((nRow-2) < 0) return;

	dlgPcb.m_nMode = 6;
	//nPos = nRow - 1;

	st_handler_info.nMenuLock = TRUE;

	dlgPcb.m_nY				= nRow - 1;
	dlgPcb.m_pcb_info		= st_test_site_info[THD_TESTSITE_7].st_pcb_info[nRow-2];

	if (dlgPcb.DoModal() == IDOK)
	{
		// jtkim 20150316
		OnPcbHistory(dlgPcb.m_nY, st_test_site_info[THD_TESTSITE_7].st_pcb_info[nRow-2], dlgPcb.m_pcb_info);

		st_test_site_info[THD_TESTSITE_7].st_pcb_info[nRow-2] = dlgPcb.m_pcb_info;
	}

	// jtkim20150626
	st_handler_info.nMenuLock = FALSE;

	OnMainDisplay();
}
void CScreenMain::OnTestSite8CellRClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CDialogPcb dlgPcb;

	int nRow, nCol;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	return; //kwlee 2017.0511

	if (st_handler_info.nRunStatus != dSTOP) return;	//2015.03.04 night


	if ((nRow-2) < 0) return;

	dlgPcb.m_nMode = 7;
	//nPos = nRow - 1;

	st_handler_info.nMenuLock = TRUE;

	dlgPcb.m_nY				= nRow - 1;
	dlgPcb.m_pcb_info		= st_test_site_info[THD_TESTSITE_8].st_pcb_info[nRow-2];

	if (dlgPcb.DoModal() == IDOK)
	{
		// jtkim 20150316
		OnPcbHistory(dlgPcb.m_nY, st_test_site_info[THD_TESTSITE_8].st_pcb_info[nRow-2], dlgPcb.m_pcb_info);

		st_test_site_info[THD_TESTSITE_8].st_pcb_info[nRow-2] = dlgPcb.m_pcb_info;
	}

	// jtkim20150626
	st_handler_info.nMenuLock = FALSE;

	OnMainDisplay();
}

void CScreenMain::OnYieldCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	//CDialogNextYieldInfo NextYield;
	//NextYield.DoModal();
	if (m_nLotYieldDisplayChange == FALSE)
	{
		OnInitGridNextLotYield();
		//OnMainNextLotDataDisplay();
		m_nLotYieldDisplayChange = TRUE;
	}
	else
	{
		OnInitGridYieldLot();
	//	OnMainCountDisplay();

		m_nLotYieldDisplayChange = FALSE;
	}
}
//kwlee 2016.0901
void CScreenMain::OnMainNextLotDataDisplay()
{
	CString strTemp;

	double dAve;

	// lot 수량.
	strTemp.Format(_T("%d"), st_count_info.nInCount[0][1]);
	m_pGridYieldLot.SetItemText(2, 1, strTemp);

	strTemp.Format(_T("%d"), st_count_info.nPrimeCount[0][1]);
	m_pGridYieldLot.SetItemText(3, 1, strTemp);

	strTemp.Format(_T("%d"), st_count_info.nPassCount[0][1]);
	m_pGridYieldLot.SetItemText(3, 2, strTemp);

	// jtkim 20150307
	strTemp.Format(_T("%d"), st_count_info.nPrimeRejectCount[0][1]);
	m_pGridYieldLot.SetItemText(4, 1, strTemp);

	strTemp.Format(_T("%d"), st_count_info.nRejectCount[0][1]);
	m_pGridYieldLot.SetItemText(4, 2, strTemp);

	if (st_count_info.nPrimeCount[0][1] > 0) 
	{
		//dAve = ((double)st_count_info.nPrimeCount[0][0] / (double)st_count_info.nInCount[0][0]) * (double)100;
		dAve = ((double)st_count_info.nPrimeCount[0][1] / ((double)st_count_info.nPrimeCount[0][1] + (double)st_count_info.nPrimeRejectCount[0][1])) * (double)100;
		strTemp.Format(_T("%.2f"), dAve);
		m_pGridYieldLot.SetItemText(5, 1, strTemp);
	}

	if (st_count_info.nPassCount[0][1] > 0) 
	{
		//dAve = ((double)st_count_info.nPassCount[0][0] / (double)st_count_info.nInCount[0][0]) * (double)100;
		dAve = ((double)st_count_info.nPassCount[0][1] / ((double)st_count_info.nPassCount[0][1] + (double)st_count_info.nRejectCount[0][1])) * (double)100;
		strTemp.Format(_T("%.2f"), dAve);
		m_pGridYieldLot.SetItemText(5, 2, strTemp);
	}

	m_pGridYieldLot.Invalidate(FALSE);
}

void CScreenMain::OnInitGridNextLotYield()
{
	int   i, j;
	int	  max_row, max_col;
	int   row;

	CString strTmp;
	CRect rect;

	max_row = 6; // unloader tray y count
	max_col = 3;

	m_pGridYieldLot.SetFrameFocusCell(FALSE);
	m_pGridYieldLot.SetTrackFocusCell(FALSE);
	m_pGridYieldLot.EnableSelection(FALSE);

	m_pGridYieldLot.SetGridLineColor(BLACK_C);
	m_pGridYieldLot.SetGridLines(1);

	m_pGridYieldLot.SetRowCount(max_row);
	m_pGridYieldLot.SetColumnCount(max_col);

	m_pGridYieldLot.SetFixedRowCount(0);
	m_pGridYieldLot.SetFixedColumnCount(0);
	m_pGridYieldLot.SetFixedBkColor(RGB(0,0,200));
	m_pGridYieldLot.SetFixedBkColor(RGB(200,200,255));
	m_pGridYieldLot.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<max_row; i++) 
	{
		m_pGridYieldLot.SetRowHeight(i, 22);

		for (j=0; j<max_col; j++) 
		{
			m_pGridYieldLot.SetColumnWidth(j, 80);

			m_pGridYieldLot.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridYieldLot.SetItemState(i, j, GVIS_READONLY);
			m_pGridYieldLot.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
		}
	}

	row = 0;
	m_pGridYieldLot.MergeCells(row, 0, row, 2);
	m_pGridYieldLot.SetItemBkColour(row, 0, RGB(219, 229, 241), BLACK_C);
	m_pGridYieldLot.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridYieldLot.SetItemText(row, 0, _T("Next Lot Yield Info"));

	row = 1;
	m_pGridYieldLot.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
	m_pGridYieldLot.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridYieldLot.SetItemText(row, 0, _T(""));

	m_pGridYieldLot.SetItemBkColour(row, 1, BLUE_D, WHITE_C);
	m_pGridYieldLot.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
	m_pGridYieldLot.SetItemText(row, 1, _T("Prime"));

	m_pGridYieldLot.SetItemBkColour(row, 2, BLUE_D, WHITE_C);
	m_pGridYieldLot.SetItemFont(row, 2, &clsFunc.OnLogFont(16));
	m_pGridYieldLot.SetItemText(row, 2, _T("Cum"));

	row = 2;
	m_pGridYieldLot.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
	m_pGridYieldLot.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridYieldLot.SetItemText(row, 0, _T("Input"));

	m_pGridYieldLot.MergeCells(row, 1, row, 2);
	m_pGridYieldLot.SetItemBkColour(row, 1, WHITE_C, BLACK_C);
	m_pGridYieldLot.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
	m_pGridYieldLot.SetItemText(row, 1, _T("0"));

	row = 3;
	m_pGridYieldLot.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
	m_pGridYieldLot.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridYieldLot.SetItemText(row, 0, _T("Pass"));

	m_pGridYieldLot.SetItemBkColour(row, 1, RED_L, BLACK_C);
	m_pGridYieldLot.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
	m_pGridYieldLot.SetItemText(row, 1, _T("0"));

	m_pGridYieldLot.SetItemBkColour(row, 2, RED_L, BLACK_C);
	m_pGridYieldLot.SetItemFont(row, 2, &clsFunc.OnLogFont(16));
	m_pGridYieldLot.SetItemText(row, 2, _T("0"));

	row = 4;
	m_pGridYieldLot.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
	m_pGridYieldLot.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridYieldLot.SetItemText(row, 0, _T("Fail"));

	m_pGridYieldLot.SetItemBkColour(row, 1, GREEN_L, BLACK_C);
	m_pGridYieldLot.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
	m_pGridYieldLot.SetItemText(row, 1, _T("0"));

	m_pGridYieldLot.SetItemBkColour(row, 2, GREEN_L, BLACK_C);
	m_pGridYieldLot.SetItemFont(row, 2, &clsFunc.OnLogFont(16));
	m_pGridYieldLot.SetItemText(row, 2, _T("0"));

	row = 5;
	m_pGridYieldLot.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
	m_pGridYieldLot.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridYieldLot.SetItemText(row, 0, _T("Yield"));

	m_pGridYieldLot.SetItemBkColour(row, 1, RED_L, BLACK_C);
	m_pGridYieldLot.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
	m_pGridYieldLot.SetItemText(row, 1, _T("0"));

	m_pGridYieldLot.SetItemBkColour(row, 2, RED_L, BLACK_C);
	m_pGridYieldLot.SetItemFont(row, 2, &clsFunc.OnLogFont(16));
	m_pGridYieldLot.SetItemText(row, 2, _T("0"));
}
////////////////////////////////
/*
void CScreenMain::OnWorkCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	int nRow, nCol;


	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	if(nRow < 2) return ; 
	if (st_handler_info.nRunStatus != dSTOP) return;

	switch (nRow)
	{
		case 5:
			break;

		default:
			break;
	}
//	m_pGridWorkInfo.Refresh();
}*/
void CScreenMain::OnPcbHistory(int nNum, tagPCB_INFO PcbOld, tagPCB_INFO PcbNew)
{
	int nBd;
	int nSlotCnt = 0;

	CString strMsg;

	if (PcbOld.strLotNo != PcbNew.strLotNo)
	{
		strMsg.Format(_T("LOT NO [%s] -> [%s] Change"), PcbOld.strLotNo, PcbNew.strLotNo); 
		clsFunc.OnLogFileAdd(0, strMsg);

		if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
		{
			clsMem.OnNormalMessageWrite(strMsg);
			st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
		}
	}
	if (PcbOld.strPartNo != PcbNew.strPartNo)
	{
		strMsg.Format(_T("PART NO [%s] -> [%s] Change"), PcbOld.strPartNo, PcbNew.strPartNo); 
		clsFunc.OnLogFileAdd(0, strMsg);

		if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
		{
			clsMem.OnNormalMessageWrite(strMsg);
			st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
		}
	}

	if (PcbOld.strBarcode1D[0] != PcbNew.strBarcode1D[0])
	{
		strMsg.Format(_T("BARCODE 1D [%s] -> [%s] Change"), PcbOld.strBarcode1D[0], PcbNew.strBarcode1D[0]); 
		clsFunc.OnLogFileAdd(0, strMsg);

		if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
		{
			clsMem.OnNormalMessageWrite(strMsg);
			st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
		}
	}

	if (PcbOld.strBarcode1D[1] != PcbNew.strBarcode1D[1])
	{
		strMsg.Format(_T("BARCODE 1D [%s] -> [%s] Change"), PcbOld.strBarcode1D[1], PcbNew.strBarcode1D[1]); 
		clsFunc.OnLogFileAdd(0, strMsg);

		if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
		{
			clsMem.OnNormalMessageWrite(strMsg);
			st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
		}
	}

	if (PcbOld.strBarcode2D != PcbNew.strBarcode2D)
	{
		strMsg.Format(_T("BARCODE 2D [%s] -> [%s] Change"), PcbOld.strBarcode2D, PcbNew.strBarcode2D); 
		clsFunc.OnLogFileAdd(0, strMsg);

		if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
		{
			clsMem.OnNormalMessageWrite(strMsg);
			st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
		}
	}

	nBd = nNum - 1;
	if (PcbOld.nYesNo != PcbNew.nYesNo)
	{
		strMsg.Format(_T("PCB YES/NO [%d] -> [%d] Change"), PcbOld.nYesNo, PcbNew.nYesNo); 
		clsFunc.OnLogFileAdd(0, strMsg);

// 		if (PcbOld.nYesNo == YES)
// 		{
// 			if (nBd >= 0)
// 			{
// // 				nSite		= (nNum) / 12;
// // 				nBd			= (nNum) % 12;
// // 				nLfToRi		= nSite / 2;
// // 				nTopToBtm	= nSite % 2;
// 
 			//	if (st_test_site_info[THD_TESTSITE_1].st_pcb_info[nBd-1].nYesNo != NO)
// 				{
// 					clsInterfaceC.m_nBdInfo[nLfToRi][nTopToBtm][nBd] = NO;
// 				}
// 
// 				if (st_basic_info.nModeInterface == EQP_ON_LINE && st_basic_info.nMp_Docking_Sensor_Disable == NO)
// 				{
// 					clsInterfaceC.OnBoardAbort(nBd, PcbNew.strLotNo, PcbNew.strSerialNo);
// 
// 					if (st_test_site_info[nLfToRi][nTopToBtm].st_pcb_info[nBd].nTestBdStart != BD_END)
// 					{
// 						st_test_site_info[nLfToRi][nTopToBtm].st_pcb_info[nBd].nTestBdStart	= BD_END;
// 					}
// 				}
// 			}
// 		}
// 		else
// 		{
// 			if (nBd >= 0)
// 			{
// 				if (st_basic_info.nModeInterface == EQP_ON_LINE && st_basic_info.nMp_Docking_Sensor_Disable == NO)
// 				{
// // 					nSite		= (nNum) / 12;
// // 					nBd			= (nNum) % 12;
// // 					nLfToRi		= nSite / 2;
// // 					nTopToBtm	= nSite % 2;
// 
// 					if (clsInterfaceC.m_nBdInfo[nLfToRi][nTopToBtm][nBd] != NO)
// 					{
// 						clsInterfaceC.m_nBdInfo[nLfToRi][nTopToBtm][nBd] = NO;
// 					}
// 
// 					clsInterfaceC.OnBoardReady(nBd, PcbNew);
// 				}
// 			}
// 		}

		if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
		{
			clsMem.OnNormalMessageWrite(strMsg);
			st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
		}
	}


	if (PcbOld.nRetestCnt != PcbNew.nRetestCnt)
	{
		strMsg.Format(_T("RETEST [%d] -> [%d] Change"), PcbOld.nRetestCnt, PcbNew.nRetestCnt); 
		clsFunc.OnLogFileAdd(0, strMsg);

		if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
		{
			clsMem.OnNormalMessageWrite(strMsg);
			st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
		}
	}


	if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
	{
		clsMem.OnNormalMessageWrite(strMsg);
		st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
	}
}

void CScreenMain::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CFormView::PostNcDestroy();
}


void CScreenMain::OnDestroy()
{
	CFormView::OnDestroy();
/*
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_pImagelist != NULL)
	{
		delete m_pImagelist;
		m_pImagelist = NULL;
	}

	if (m_pPcbInfo != NULL)
	{
		delete m_pPcbInfo;
		m_pPcbInfo = NULL;
	}

	if (m_pPcbStatus != NULL)
	{
		delete m_pPcbStatus;
		m_pPcbStatus = NULL;
	}

	if (m_pLotDisplay != NULL)
	{
		m_pLotDisplay->DestroyWindow();

		delete m_pLotDisplay;
		m_pLotDisplay = NULL;
	}

	st_handler_info.cWndMain = NULL;*/
}



void CScreenMain::OnClickedBtnMainAnimate()
{
	
	if (st_handler_info.nRunStatus != dSTOP) return;

	CDialog_Message dlgMsg;
//	int nResponse;

	dlgMsg.m_nMessageType	= 1;

// 	if (FAS_IO.get_in_bit(st_io_info.i_CokChangeCokClampFwdChk, IO_OFF)	== IO_ON ||
// 		FAS_IO.get_in_bit(st_io_info.i_CokChangeCokClampBwdChk, IO_ON)	== IO_OFF)
// 	{
// 		dlgMsg.m_strMessage		= _T("Do you want a COK Buffer UnClamp?");
// 
// 		nResponse = (int)dlgMsg.DoModal();
// 
// 		if (nResponse == IDOK)
// 		{
// 			FAS_IO.set_out_bit(st_io_info.o_CokChangeCokClampFwdCyl, IO_OFF);
// 			FAS_IO.set_out_bit(st_io_info.o_CokChangeCokClampBwdCyl, IO_ON);
// 		}
// 	}
// 	else
// 	{
// 		dlgMsg.m_strMessage		= _T("Do you want a COK Buffer Clamp?");
// 
// 		nResponse = (int)dlgMsg.DoModal();
// 
// 		if (nResponse == IDOK)
// 		{
// 			FAS_IO.set_out_bit(st_io_info.o_CokChangeCokClampFwdCyl, IO_OFF);
// 			FAS_IO.set_out_bit(st_io_info.o_CokChangeCokClampBwdCyl, IO_ON);
// 		}
// 	}
}


void CScreenMain::OnClickedBtnMainData()
{
	
	
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
 	if (st_handler_info.nRunStatus != dSTOP) return;
 
 	CDialog_Message dlgMsg;

// 	if (FAS_IO.get_in_bit(st_io_info.i_FrontSelectSwChk, IO_ON) == IO_OFF ||
// 		FAS_IO.get_in_bit(st_io_info.i_RearSelectSwChk1, IO_ON)	== IO_OFF ||
// 		FAS_IO.get_in_bit(st_io_info.i_RearSelectSwChk2, IO_ON)	== IO_OFF)
// 	{
// 		FAS_IO.set_out_bit(st_io_info.o_DoorLock, IO_OFF);
// 	}
// 	else
// 	{
// 		dlgMsg.m_nMessageType	= 0;
// 		dlgMsg.m_strMessage		= _T("Please Check Manual Key Status.");
// 
// 		dlgMsg.DoModal();
// 
// /*		if (FAS_IO.get_in_bit(st_io_info.i_ManualModeChk, IO_OFF) == IO_ON)
// 		{
// 			FAS_IO.set_out_bit(st_io_info.o_DoorLock, IO_OFF);
// 		}
// 		else
// 		{
// 			dlgMsg.m_nMessageType	= 0;
// 			dlgMsg.m_strMessage		= _T("Please Check Manual Key Status.");
// 
// 			dlgMsg.DoModal();
// 		}*/
// 	}
}


void CScreenMain::OnInitGridLdTray()
{
	CRect rtArea[2];
	int   nX, nY;
	int   nWidth, nHeight;
	int i, j;
	int nMaxRow, nMaxCol;
	int	nCenX, nCenY;
	CRect rect;
	CString strTemp;

	nMaxRow = st_recipe_info.nTrayY + 1;	
	nMaxCol = st_recipe_info.nTrayX;
	if (nMaxCol <= 0) nMaxCol = 1;
	if (nMaxRow <= 0) nMaxRow = 1;

	m_pGridLdTray.SetFrameFocusCell(FALSE);
	m_pGridLdTray.SetTrackFocusCell(FALSE);
	m_pGridLdTray.EnableSelection(FALSE);
				   
	m_pGridLdTray.SetGridLineColor(BLACK_C);
	m_pGridLdTray.SetGridLines(1);
				   
	m_pGridLdTray.SetRowCount(nMaxRow);
	m_pGridLdTray.SetColumnCount(nMaxCol);
				   
	m_pGridLdTray.SetFixedRowCount(0);
	m_pGridLdTray.SetFixedColumnCount(0);
	m_pGridLdTray.SetFixedBkColor(RGB(0,0,200));
	m_pGridLdTray.SetFixedBkColor(RGB(200,200,255));
	m_pGridLdTray.SetTextBkColor(RGB(150,150,200));

	GetDlgItem(IDC_GROUP_LD_TRAY)->GetWindowRect(rtArea[0]); // group box rect 구하기 
	ScreenToClient(rtArea[0]); // window상에서 group box 좌표 구하기

	nWidth	= rtArea[0].Width() - 30;
	nHeight	= rtArea[0].Height() - 30;

	nX	= nWidth / nMaxCol;

	if (nMaxRow > 0)	
	{
		nY = nHeight / nMaxRow;
	}
	else	
	{
		nY = 1;
	}

	// group box에서 grid 센터에 표시하기 위한 x좌표 구하기 
	nCenX = rtArea[0].left + ((rtArea[0].Width() - (nX * nMaxCol)) / 2) - 1;
	// group box에서 grid 센터에 표시하기 위한 y좌표 구하기 
	nCenY = rtArea[0].top + ((rtArea[0].Height() - (nY * nMaxRow)) / 2) + 4;
	// grid size 계산하여 중심에 다시 그리기 
	GetDlgItem(IDC_CUSTOM_LD_TRAY)->MoveWindow(nCenX, nCenY, (nX * nMaxCol) + 4, (nY * nMaxRow) + 4, TRUE);

	for (i=0; i<nMaxRow; i++)
	{
		m_pGridLdTray.SetRowHeight(i, nY);

		for (j=0; j<nMaxCol; j++)
		{
			m_pGridLdTray.SetColumnWidth(j, nX);
			m_pGridLdTray.SetItemBkColour(i, j, BLACK_L, WHITE_C);
			m_pGridLdTray.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridLdTray.SetItemFont(i, j, &clsFunc.OnLogFont(14));
			m_pGridLdTray.SetItemState(i, j, GVIS_READONLY);

			m_pGridLdTray.SetItemFont(i, j, &clsFunc.OnLogFont(12));
		}
	}

	//m_pGridLdTray.MergeCells(0, 0, 0, nMaxCol-1);
	m_pGridLdTray.SetItemBkColour(0, 0, YELLOW_L, BLACK_C);
	m_pGridLdTray.SetItemFont(0, 0, &clsFunc.OnLogFont(12));

	m_pGridLdTray.Invalidate(FALSE);
}


void CScreenMain::OnInitGridUnLdTrayFirst()
{
	CRect rtArea[2];
	int   nX, nY;
	int   nWidth, nHeight;
	int i, j;
	int nMaxRow, nMaxCol;
	int	nCenX, nCenY;
	CRect rect;
	CString strTemp;

	nMaxRow = st_recipe_info.nTrayY + 1;	
	nMaxCol = st_recipe_info.nTrayX;
	if (nMaxCol <= 0) nMaxCol = 1;
	if (nMaxRow <= 0) nMaxRow = 1;

	m_pGridUnLdTrayFirst.SetFrameFocusCell(FALSE);
	m_pGridUnLdTrayFirst.SetTrackFocusCell(FALSE);
	m_pGridUnLdTrayFirst.EnableSelection(FALSE);
				   
	m_pGridUnLdTrayFirst.SetGridLineColor(BLACK_C);
	m_pGridUnLdTrayFirst.SetGridLines(1);
				   
	m_pGridUnLdTrayFirst.SetRowCount(nMaxRow);
	m_pGridUnLdTrayFirst.SetColumnCount(nMaxCol);
				   
	m_pGridUnLdTrayFirst.SetFixedRowCount(0);
	m_pGridUnLdTrayFirst.SetFixedColumnCount(0);
	m_pGridUnLdTrayFirst.SetFixedBkColor(RGB(0,0,200));
	m_pGridUnLdTrayFirst.SetFixedBkColor(RGB(200,200,255));
	m_pGridUnLdTrayFirst.SetTextBkColor(RGB(150,150,200));

	GetDlgItem(IDC_GROUP_UNLD_TRAY_FIRST)->GetWindowRect(rtArea[0]); // group box rect 구하기 
	ScreenToClient(rtArea[0]); // window상에서 group box 좌표 구하기

	nWidth	= rtArea[0].Width() - 30;
	nHeight	= rtArea[0].Height() - 30;

	nX	= nWidth / nMaxCol;

	if (nMaxRow > 0)	
	{
		nY = nHeight / nMaxRow;
	}
	else	
	{
		nY = 1;
	}

	// group box에서 grid 센터에 표시하기 위한 x좌표 구하기 
	nCenX = rtArea[0].left + ((rtArea[0].Width() - (nX * nMaxCol)) / 2) - 1;
	// group box에서 grid 센터에 표시하기 위한 y좌표 구하기 
	nCenY = rtArea[0].top + ((rtArea[0].Height() - (nY * nMaxRow)) / 2) + 4;
	// grid size 계산하여 중심에 다시 그리기 
	GetDlgItem(IDC_CUSTOM_UNLD_TRAY_FIRST)->MoveWindow(nCenX, nCenY, (nX * nMaxCol) + 4, (nY * nMaxRow) + 4, TRUE);

	for (i=0; i<nMaxRow; i++)
	{
		m_pGridUnLdTrayFirst.SetRowHeight(i, nY);

		for (j=0; j<nMaxCol; j++)
		{
			m_pGridUnLdTrayFirst.SetColumnWidth(j, nX);
			m_pGridUnLdTrayFirst.SetItemBkColour(i, j, BLACK_L, WHITE_C);
			m_pGridUnLdTrayFirst.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridUnLdTrayFirst.SetItemFont(i, j, &clsFunc.OnLogFont(14));
			m_pGridUnLdTrayFirst.SetItemState(i, j, GVIS_READONLY);

			m_pGridUnLdTrayFirst.SetItemFont(i, j, &clsFunc.OnLogFont(12));
		}
	}

	//m_pGridUnLdTrayFirst.MergeCells(0, 0, 0, nMaxCol-1);
	m_pGridUnLdTrayFirst.SetItemBkColour(0, 0, YELLOW_L, BLACK_C);
	m_pGridUnLdTrayFirst.SetItemFont(0, 0, &clsFunc.OnLogFont(12));

	m_pGridUnLdTrayFirst.Invalidate(FALSE);
}


void CScreenMain::OnInitGridUnLdTraySecond()
{
	CRect rtArea[2];
	int   nX, nY;
	int   nWidth, nHeight;
	int i, j;
	int nMaxRow, nMaxCol;
	int	nCenX, nCenY;
	CRect rect;
	CString strTemp;

	nMaxRow = st_recipe_info.nTrayY + 1;	
	nMaxCol = st_recipe_info.nTrayX;
	if (nMaxCol <= 0) nMaxCol = 1;
	if (nMaxRow <= 0) nMaxRow = 1;

	m_pGridUnLdTraySecond.SetFrameFocusCell(FALSE);
	m_pGridUnLdTraySecond.SetTrackFocusCell(FALSE);
	m_pGridUnLdTraySecond.EnableSelection(FALSE);
				   
	m_pGridUnLdTraySecond.SetGridLineColor(BLACK_C);
	m_pGridUnLdTraySecond.SetGridLines(1);
				   
	m_pGridUnLdTraySecond.SetRowCount(nMaxRow);
	m_pGridUnLdTraySecond.SetColumnCount(nMaxCol);
				   
	m_pGridUnLdTraySecond.SetFixedRowCount(0);
	m_pGridUnLdTraySecond.SetFixedColumnCount(0);
	m_pGridUnLdTraySecond.SetFixedBkColor(RGB(0,0,200));
	m_pGridUnLdTraySecond.SetFixedBkColor(RGB(200,200,255));
	m_pGridUnLdTraySecond.SetTextBkColor(RGB(150,150,200));

	GetDlgItem(IDC_GROUP_UNLD_TRAY_SECOND)->GetWindowRect(rtArea[0]); // group box rect 구하기 
	ScreenToClient(rtArea[0]); // window상에서 group box 좌표 구하기

	nWidth	= rtArea[0].Width() - 30;
	nHeight	= rtArea[0].Height() - 30;

	nX	= nWidth / nMaxCol;

	if (nMaxRow > 0)	
	{
		nY = nHeight / nMaxRow;
	}
	else	
	{
		nY = 1;
	}

	// group box에서 grid 센터에 표시하기 위한 x좌표 구하기 
	nCenX = rtArea[0].left + ((rtArea[0].Width() - (nX * nMaxCol)) / 2) - 1;
	// group box에서 grid 센터에 표시하기 위한 y좌표 구하기 
	nCenY = rtArea[0].top + ((rtArea[0].Height() - (nY * nMaxRow)) / 2) + 4;
	// grid size 계산하여 중심에 다시 그리기 
	GetDlgItem(IDC_CUSTOM_UNLD_TRAY_SECOND)->MoveWindow(nCenX, nCenY, (nX * nMaxCol) + 4, (nY * nMaxRow) + 4, TRUE);

	for (i=0; i<nMaxRow; i++)
	{
		m_pGridUnLdTraySecond.SetRowHeight(i, nY);

		for (j=0; j<nMaxCol; j++)
		{
			m_pGridUnLdTraySecond.SetColumnWidth(j, nX);
			m_pGridUnLdTraySecond.SetItemBkColour(i, j, BLACK_L, WHITE_C);
			m_pGridUnLdTraySecond.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridUnLdTraySecond.SetItemFont(i, j, &clsFunc.OnLogFont(14));
			m_pGridUnLdTraySecond.SetItemState(i, j, GVIS_READONLY);

			m_pGridUnLdTraySecond.SetItemFont(i, j, &clsFunc.OnLogFont(12));
		}
	}

	//m_pGridUnLdTraySecond.MergeCells(0, 0, 0, nMaxCol-1);
	m_pGridUnLdTraySecond.SetItemBkColour(0, 0, YELLOW_L, BLACK_C);
	m_pGridUnLdTraySecond.SetItemFont(0, 0, &clsFunc.OnLogFont(12));

	m_pGridUnLdTraySecond.Invalidate(FALSE);
}


void CScreenMain::OnInitGridRejectTrayFirst()
{
	CRect rtArea[2];
	int   nX, nY;
	int   nWidth, nHeight;
	int i, j;
	int nMaxRow, nMaxCol;
	int	nCenX, nCenY;
	CRect rect;
	CString strTemp;

	nMaxRow = st_recipe_info.nTrayY + 1;	
	nMaxCol = st_recipe_info.nTrayX;
	if (nMaxCol <= 0) nMaxCol = 1;
	if (nMaxRow <= 0) nMaxRow = 1;

	m_pGridRejectTrayFirst.SetFrameFocusCell(FALSE);
	m_pGridRejectTrayFirst.SetTrackFocusCell(FALSE);
	m_pGridRejectTrayFirst.EnableSelection(FALSE);
				   
	m_pGridRejectTrayFirst.SetGridLineColor(BLACK_C);
	m_pGridRejectTrayFirst.SetGridLines(1);
				   
	m_pGridRejectTrayFirst.SetRowCount(nMaxRow);
	m_pGridRejectTrayFirst.SetColumnCount(nMaxCol);
				   
	m_pGridRejectTrayFirst.SetFixedRowCount(0);
	m_pGridRejectTrayFirst.SetFixedColumnCount(0);
	m_pGridRejectTrayFirst.SetFixedBkColor(RGB(0,0,200));
	m_pGridRejectTrayFirst.SetFixedBkColor(RGB(200,200,255));
	m_pGridRejectTrayFirst.SetTextBkColor(RGB(150,150,200));

	GetDlgItem(IDC_GROUP_REJECT_TRAY_FIRST)->GetWindowRect(rtArea[0]); // group box rect 구하기 
	ScreenToClient(rtArea[0]); // window상에서 group box 좌표 구하기

	nWidth	= rtArea[0].Width() - 30;
	nHeight	= rtArea[0].Height() - 30;

	nX	= nWidth / nMaxCol;

	if (nMaxRow > 0)	
	{
		nY = nHeight / nMaxRow;
	}
	else	
	{
		nY = 1;
	}

	// group box에서 grid 센터에 표시하기 위한 x좌표 구하기 
	nCenX = rtArea[0].left + ((rtArea[0].Width() - (nX * nMaxCol)) / 2) - 1;
	// group box에서 grid 센터에 표시하기 위한 y좌표 구하기 
	nCenY = rtArea[0].top + ((rtArea[0].Height() - (nY * nMaxRow)) / 2) + 4;
	// grid size 계산하여 중심에 다시 그리기 
	GetDlgItem(IDC_CUSTOM_REJECT_TRAY_FIRST)->MoveWindow(nCenX, nCenY, (nX * nMaxCol) + 4, (nY * nMaxRow) + 4, TRUE);

	for (i=0; i<nMaxRow; i++)
	{
		m_pGridRejectTrayFirst.SetRowHeight(i, nY);

		for (j=0; j<nMaxCol; j++)
		{
			m_pGridRejectTrayFirst.SetColumnWidth(j, nX);
			m_pGridRejectTrayFirst.SetItemBkColour(i, j, BLACK_L, WHITE_C);
			m_pGridRejectTrayFirst.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridRejectTrayFirst.SetItemFont(i, j, &clsFunc.OnLogFont(14));
			m_pGridRejectTrayFirst.SetItemState(i, j, GVIS_READONLY);

			m_pGridRejectTrayFirst.SetItemFont(i, j, &clsFunc.OnLogFont(12));
		}
	}

	//m_pGridRejectTrayFirst.MergeCells(0, 0, 0, nMaxCol-1);
	m_pGridRejectTrayFirst.SetItemBkColour(0, 0, YELLOW_L, BLACK_C);
	m_pGridRejectTrayFirst.SetItemFont(0, 0, &clsFunc.OnLogFont(12));

	m_pGridRejectTrayFirst.Invalidate(FALSE);
}


void CScreenMain::OnInitGridRejectTraySecond()
{
	CRect rtArea[2];
	int   nX, nY;
	int   nWidth, nHeight;
	int i, j;
	int nMaxRow, nMaxCol;
	int	nCenX, nCenY;
	CRect rect;
	CString strTemp;

	nMaxRow = st_recipe_info.nTrayY + 1;	
	nMaxCol = st_recipe_info.nTrayX;
	if (nMaxCol <= 0) nMaxCol = 1;
	if (nMaxRow <= 0) nMaxRow = 1;

	m_pGridRejectTraySecond.SetFrameFocusCell(FALSE);
	m_pGridRejectTraySecond.SetTrackFocusCell(FALSE);
	m_pGridRejectTraySecond.EnableSelection(FALSE);
				   
	m_pGridRejectTraySecond.SetGridLineColor(BLACK_C);
	m_pGridRejectTraySecond.SetGridLines(1);
				   
	m_pGridRejectTraySecond.SetRowCount(nMaxRow);
	m_pGridRejectTraySecond.SetColumnCount(nMaxCol);
				   
	m_pGridRejectTraySecond.SetFixedRowCount(0);
	m_pGridRejectTraySecond.SetFixedColumnCount(0);
	m_pGridRejectTraySecond.SetFixedBkColor(RGB(0,0,200));
	m_pGridRejectTraySecond.SetFixedBkColor(RGB(200,200,255));
	m_pGridRejectTraySecond.SetTextBkColor(RGB(150,150,200));

	GetDlgItem(IDC_GROUP_REJECT_TRAY_SECOND)->GetWindowRect(rtArea[0]); // group box rect 구하기 
	ScreenToClient(rtArea[0]); // window상에서 group box 좌표 구하기

	nWidth	= rtArea[0].Width() - 30;
	nHeight	= rtArea[0].Height() - 30;

	nX	= nWidth / nMaxCol;

	if (nMaxRow > 0)	
	{
		nY = nHeight / nMaxRow;
	}
	else	
	{
		nY = 1;
	}

	// group box에서 grid 센터에 표시하기 위한 x좌표 구하기 
	nCenX = rtArea[0].left + ((rtArea[0].Width() - (nX * nMaxCol)) / 2) - 1;
	// group box에서 grid 센터에 표시하기 위한 y좌표 구하기 
	nCenY = rtArea[0].top + ((rtArea[0].Height() - (nY * nMaxRow)) / 2) + 4;
	// grid size 계산하여 중심에 다시 그리기 
	GetDlgItem(IDC_CUSTOM_REJECT_TRAY_SECOND)->MoveWindow(nCenX, nCenY, (nX * nMaxCol) + 4, (nY * nMaxRow) + 4, TRUE);

	for (i=0; i<nMaxRow; i++)
	{
		m_pGridRejectTraySecond.SetRowHeight(i, nY);

		for (j=0; j<nMaxCol; j++)
		{
			m_pGridRejectTraySecond.SetColumnWidth(j, nX);
			m_pGridRejectTraySecond.SetItemBkColour(i, j, BLACK_L, WHITE_C);
			m_pGridRejectTraySecond.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridRejectTraySecond.SetItemFont(i, j, &clsFunc.OnLogFont(14));
			m_pGridRejectTraySecond.SetItemState(i, j, GVIS_READONLY);

			m_pGridRejectTraySecond.SetItemFont(i, j, &clsFunc.OnLogFont(12));
		}
	}

	//m_pGridRejectTraySecond.MergeCells(0, 0, 0, nMaxCol-1);
	m_pGridRejectTraySecond.SetItemBkColour(0, 0, YELLOW_L, BLACK_C);
	m_pGridRejectTraySecond.SetItemFont(0, 0, &clsFunc.OnLogFont(12));

	m_pGridRejectTraySecond.Invalidate(FALSE);
}


void CScreenMain::OnInitGridTrayRobot()
{
	CRect rtArea[2];
	int   nX, nY;
	int   nWidth, nHeight;
	int i, j;
	int nMaxRow, nMaxCol;
	int	nCenX, nCenY;
	CRect rect;
	CString strTemp;

	nMaxRow = 1;	
	nMaxCol = 1;
	if (nMaxCol <= 0) nMaxCol = 1;

	m_pGridTrayRobot.SetFrameFocusCell(FALSE);
	m_pGridTrayRobot.SetTrackFocusCell(FALSE);
	m_pGridTrayRobot.EnableSelection(FALSE);
				   
	m_pGridTrayRobot.SetGridLineColor(BLACK_C);
	m_pGridTrayRobot.SetGridLines(1);
				   
	m_pGridTrayRobot.SetRowCount(nMaxRow);
	m_pGridTrayRobot.SetColumnCount(nMaxCol);
				   
	m_pGridTrayRobot.SetFixedRowCount(0);
	m_pGridTrayRobot.SetFixedColumnCount(0);
	m_pGridTrayRobot.SetFixedBkColor(RGB(0,0,200));
	m_pGridTrayRobot.SetFixedBkColor(RGB(200,200,255));
	m_pGridTrayRobot.SetTextBkColor(RGB(150,150,200));

	GetDlgItem(IDC_GROUP_TRAY_ROBOT)->GetWindowRect(rtArea[0]); // group box rect 구하기 
	ScreenToClient(rtArea[0]); // window상에서 group box 좌표 구하기

	nWidth	= rtArea[0].Width() - 30;
	nHeight	= rtArea[0].Height() - 30;

	nX	= nWidth / nMaxCol;

	if (nMaxRow > 0)	
	{
		nY = nHeight / nMaxRow;
	}
	else	
	{
		nY = 1;
	}

	// group box에서 grid 센터에 표시하기 위한 x좌표 구하기 
	nCenX = rtArea[0].left + ((rtArea[0].Width() - (nX * nMaxCol)) / 2) - 1;
	// group box에서 grid 센터에 표시하기 위한 y좌표 구하기 
	nCenY = rtArea[0].top + ((rtArea[0].Height() - (nY * nMaxRow)) / 2) + 4;
	// grid size 계산하여 중심에 다시 그리기 
	GetDlgItem(IDC_CUSTOM_TRAY_ROBOT)->MoveWindow(nCenX, nCenY, (nX * nMaxCol) + 4, (nY * nMaxRow) + 4, TRUE);

	for (i=0; i<nMaxRow; i++)
	{
		m_pGridTrayRobot.SetRowHeight(i, nY);

		for (j=0; j<nMaxCol; j++)
		{
			m_pGridTrayRobot.SetColumnWidth(j, nX);
			m_pGridTrayRobot.SetItemBkColour(i, j, BLACK_L, WHITE_C);
			m_pGridTrayRobot.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridTrayRobot.SetItemFont(i, j, &clsFunc.OnLogFont(14));
			m_pGridTrayRobot.SetItemState(i, j, GVIS_READONLY);

			m_pGridTrayRobot.SetItemFont(i, j, &clsFunc.OnLogFont(12));
		}
	}

	m_pGridTrayRobot.Invalidate(FALSE);
}


void CScreenMain::OnInitGridLdBuf()
{
	CRect rtArea[2];
	int   nX, nY;
	int   nWidth, nHeight;
	int i, j;
	int nMaxRow, nMaxCol;
	int	nCenX, nCenY;
	CRect rect;
	CString strTemp;

	nMaxRow = 2;	
	nMaxCol = st_recipe_info.nLdUldBuffer_Num;
	if (nMaxCol <= 0) nMaxCol = 1;

	m_pGridLdBuf.SetFrameFocusCell(FALSE);
	m_pGridLdBuf.SetTrackFocusCell(FALSE);
	m_pGridLdBuf.EnableSelection(FALSE);
				   
	m_pGridLdBuf.SetGridLineColor(BLACK_C);
	m_pGridLdBuf.SetGridLines(1);
				   
	m_pGridLdBuf.SetRowCount(nMaxRow);
	m_pGridLdBuf.SetColumnCount(nMaxCol);
				   
	m_pGridLdBuf.SetFixedRowCount(0);
	m_pGridLdBuf.SetFixedColumnCount(0);
	m_pGridLdBuf.SetFixedBkColor(RGB(0,0,200));
	m_pGridLdBuf.SetFixedBkColor(RGB(200,200,255));
	m_pGridLdBuf.SetTextBkColor(RGB(150,150,200));

	GetDlgItem(IDC_GROUP_LD_BUF)->GetWindowRect(rtArea[0]); // group box rect 구하기 
	ScreenToClient(rtArea[0]); // window상에서 group box 좌표 구하기

	nWidth	= rtArea[0].Width() - 30;
	nHeight	= rtArea[0].Height() - 30;

	nX	= nWidth / nMaxCol;

	if (nMaxRow > 0)	
	{
		nY = nHeight / nMaxRow;
	}
	else	
	{
		nY = 1;
	}

	// group box에서 grid 센터에 표시하기 위한 x좌표 구하기 
	nCenX = rtArea[0].left + ((rtArea[0].Width() - (nX * nMaxCol)) / 2) - 1;
	// group box에서 grid 센터에 표시하기 위한 y좌표 구하기 
	nCenY = rtArea[0].top + ((rtArea[0].Height() - (nY * nMaxRow)) / 2) + 4;
	// grid size 계산하여 중심에 다시 그리기 
	GetDlgItem(IDC_CUSTOM_LD_BUF)->MoveWindow(nCenX, nCenY, (nX * nMaxCol) + 4, (nY * nMaxRow) + 4, TRUE);

	for (i=0; i<nMaxRow; i++)
	{
		m_pGridLdBuf.SetRowHeight(i, nY);

		for (j=0; j<nMaxCol; j++)
		{
			m_pGridLdBuf.SetColumnWidth(j, nX);
			m_pGridLdBuf.SetItemBkColour(i, j, BLACK_L, WHITE_C);
			m_pGridLdBuf.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridLdBuf.SetItemFont(i, j, &clsFunc.OnLogFont(14));
			m_pGridLdBuf.SetItemState(i, j, GVIS_READONLY);

			m_pGridLdBuf.SetItemFont(i, j, &clsFunc.OnLogFont(12));
		}
	}

	m_pGridLdBuf.MergeCells(0, 0, 0, nMaxCol - 1);
	m_pGridLdBuf.SetItemBkColour(0, 0, YELLOW_L, BLACK_C);

	m_pGridLdBuf.Invalidate(FALSE);
}


void CScreenMain::OnInitGridUnLdBuf()
{
	CRect rtArea[2];
	int   nX, nY;
	int   nWidth, nHeight;
	int i, j;
	int nMaxRow, nMaxCol;
	int	nCenX, nCenY;
	CRect rect;
	CString strTemp;

	nMaxRow = 2;	
	nMaxCol = st_recipe_info.nLdUldBuffer_Num;
	if (nMaxCol <= 0) nMaxCol = 1;

	m_pGridUnLdBuf.SetFrameFocusCell(FALSE);
	m_pGridUnLdBuf.SetTrackFocusCell(FALSE);
	m_pGridUnLdBuf.EnableSelection(FALSE);
				   
	m_pGridUnLdBuf.SetGridLineColor(BLACK_C);
	m_pGridUnLdBuf.SetGridLines(1);
				   
	m_pGridUnLdBuf.SetRowCount(nMaxRow);
	m_pGridUnLdBuf.SetColumnCount(nMaxCol);
				   
	m_pGridUnLdBuf.SetFixedRowCount(0);
	m_pGridUnLdBuf.SetFixedColumnCount(0);
	m_pGridUnLdBuf.SetFixedBkColor(RGB(0,0,200));
	m_pGridUnLdBuf.SetFixedBkColor(RGB(200,200,255));
	m_pGridUnLdBuf.SetTextBkColor(RGB(150,150,200));

	GetDlgItem(IDC_GROUP_UNLD_BUF)->GetWindowRect(rtArea[0]); // group box rect 구하기 
	ScreenToClient(rtArea[0]); // window상에서 group box 좌표 구하기

	nWidth	= rtArea[0].Width() - 30;
	nHeight	= rtArea[0].Height() - 30;

	nX	= nWidth / nMaxCol;

	if (nMaxRow > 0)	
	{
		nY = nHeight / nMaxRow;
	}
	else	
	{
		nY = 1;
	}

	// group box에서 grid 센터에 표시하기 위한 x좌표 구하기 
	nCenX = rtArea[0].left + ((rtArea[0].Width() - (nX * nMaxCol)) / 2) - 1;
	// group box에서 grid 센터에 표시하기 위한 y좌표 구하기 
	nCenY = rtArea[0].top + ((rtArea[0].Height() - (nY * nMaxRow)) / 2) + 4;
	// grid size 계산하여 중심에 다시 그리기 
	GetDlgItem(IDC_CUSTOM_UNLD_BUF)->MoveWindow(nCenX, nCenY, (nX * nMaxCol) + 4, (nY * nMaxRow) + 4, TRUE);

	for (i=0; i<nMaxRow; i++)
	{
		m_pGridUnLdBuf.SetRowHeight(i, nY);

		for (j=0; j<nMaxCol; j++)
		{
			m_pGridUnLdBuf.SetColumnWidth(j, nX);
			m_pGridUnLdBuf.SetItemBkColour(i, j, BLACK_L, WHITE_C);
			m_pGridUnLdBuf.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridUnLdBuf.SetItemFont(i, j, &clsFunc.OnLogFont(14));
			m_pGridUnLdBuf.SetItemState(i, j, GVIS_READONLY);

			m_pGridUnLdBuf.SetItemFont(i, j, &clsFunc.OnLogFont(12));
		}
	}

	m_pGridUnLdBuf.MergeCells(0, 0, 0, nMaxCol - 1);
	m_pGridUnLdBuf.SetItemBkColour(0, 0, YELLOW_L, BLACK_C);

	m_pGridUnLdBuf.Invalidate(FALSE);
}


void CScreenMain::OnInitGridTestSiteRobot()
{
	CRect rtArea[2];
	int   nX, nY;
	int   nWidth, nHeight;
	int i, j;
	int nMaxRow, nMaxCol;
	int	nCenX, nCenY;
	CRect rect;
	CString strTemp;

	nMaxRow = 2;	
	nMaxCol = 4;
	if (nMaxCol <= 0) nMaxCol = 1;

	m_pGridTestSiteRobot.SetFrameFocusCell(FALSE);
	m_pGridTestSiteRobot.SetTrackFocusCell(FALSE);
	m_pGridTestSiteRobot.EnableSelection(FALSE);
				   
	m_pGridTestSiteRobot.SetGridLineColor(BLACK_C);
	m_pGridTestSiteRobot.SetGridLines(1);
				   
	m_pGridTestSiteRobot.SetRowCount(nMaxRow);
	m_pGridTestSiteRobot.SetColumnCount(nMaxCol);
				   
	m_pGridTestSiteRobot.SetFixedRowCount(0);
	m_pGridTestSiteRobot.SetFixedColumnCount(0);
	m_pGridTestSiteRobot.SetFixedBkColor(RGB(0,0,200));
	m_pGridTestSiteRobot.SetFixedBkColor(RGB(200,200,255));
	m_pGridTestSiteRobot.SetTextBkColor(RGB(150,150,200));

	GetDlgItem(IDC_GROUP_TEST_SITE_ROBOT)->GetWindowRect(rtArea[0]); // group box rect 구하기 
	ScreenToClient(rtArea[0]); // window상에서 group box 좌표 구하기

	nWidth	= rtArea[0].Width() - 30;
	nHeight	= rtArea[0].Height() - 30;

	nX	= nWidth / nMaxCol;

	if (nMaxRow > 0)	
	{
		nY = nHeight / nMaxRow;
	}
	else	
	{
		nY = 1;
	}

	// group box에서 grid 센터에 표시하기 위한 x좌표 구하기 
	nCenX = rtArea[0].left + ((rtArea[0].Width() - (nX * nMaxCol)) / 2) - 1;
	// group box에서 grid 센터에 표시하기 위한 y좌표 구하기 
	nCenY = rtArea[0].top + ((rtArea[0].Height() - (nY * nMaxRow)) / 2) + 4;
	// grid size 계산하여 중심에 다시 그리기 
	GetDlgItem(IDC_CUSTOM_TEST_SITE_ROBOT)->MoveWindow(nCenX, nCenY, (nX * nMaxCol) + 4, (nY * nMaxRow) + 4, TRUE);

	for (i=0; i<nMaxRow; i++)
	{
		m_pGridTestSiteRobot.SetRowHeight(i, nY);

		for (j=0; j<nMaxCol; j++)
		{
			m_pGridTestSiteRobot.SetColumnWidth(j, nX);
			m_pGridTestSiteRobot.SetItemBkColour(i, j, BLACK_L, WHITE_C);
			m_pGridTestSiteRobot.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridTestSiteRobot.SetItemFont(i, j, &clsFunc.OnLogFont(14));
			m_pGridTestSiteRobot.SetItemState(i, j, GVIS_READONLY);

			m_pGridTestSiteRobot.SetItemFont(i, j, &clsFunc.OnLogFont(12));
		}
	}

	m_pGridTestSiteRobot.MergeCells(0, 0, 0, nMaxCol - 1);
	m_pGridTestSiteRobot.SetItemBkColour(0, 0, YELLOW_L, BLACK_C);
	m_pGridTestSiteRobot.Invalidate(FALSE);
}


void CScreenMain::OnInitGridRetestBufFirst1()
{
	CRect rtArea[2];
	int   nX, nY;
	int   nWidth, nHeight;
	int i, j;
	int nMaxRow, nMaxCol;
	int	nCenX, nCenY;
//	int nRow, nCol;

	CRect rect;
	CString strTemp;

	//nMaxRow = st_recipe_info.nRetestBuffer_Num / 2;	
	nMaxRow = st_recipe_info.nRetestBuffer_Num + 1;	
	//if ((nMaxRow * 2) != st_recipe_info.nRetestBuffer_Num) nMaxRow++;
// 	nMaxRow = st_recipe_info.nRetestBuffer_Num;
 	nMaxCol = 2;	
	//if (nMaxCol <= 0) nMaxCol = 1;

	m_pGridRetestBufFirst[0].SetFrameFocusCell(FALSE);
	m_pGridRetestBufFirst[0].SetTrackFocusCell(FALSE);
	m_pGridRetestBufFirst[0].EnableSelection(FALSE);
				   
	m_pGridRetestBufFirst[0].SetGridLineColor(BLACK_C);
	m_pGridRetestBufFirst[0].SetGridLines(1);
				   
	m_pGridRetestBufFirst[0].SetRowCount(nMaxRow);
	m_pGridRetestBufFirst[0].SetColumnCount(nMaxCol);
				   
	m_pGridRetestBufFirst[0].SetFixedRowCount(0);
	m_pGridRetestBufFirst[0].SetFixedColumnCount(0);
	m_pGridRetestBufFirst[0].SetFixedBkColor(RGB(0,0,200));
	m_pGridRetestBufFirst[0].SetFixedBkColor(RGB(200,200,255));
	m_pGridRetestBufFirst[0].SetTextBkColor(RGB(150,150,200));

	GetDlgItem(IDC_MSG_RETEST_BUFFER_FIRST_1)->GetWindowRect(rtArea[0]); // group box rect 구하기 
	ScreenToClient(rtArea[0]); // window상에서 group box 좌표 구하기

	nWidth	= rtArea[0].Width() - 10;
	nHeight	= rtArea[0].Height() - 5 ;

	nX	= nWidth / nMaxCol;

	if (nMaxRow > 0)	
	{
		nY = nHeight / nMaxRow;
	}
	else	
	{
		nY = 1;
	}

	// group box에서 grid 센터에 표시하기 위한 x좌표 구하기 
	nCenX = rtArea[0].left + ((rtArea[0].Width() - (nX * nMaxCol)) / 2) - 2;
	// group box에서 grid 센터에 표시하기 위한 y좌표 구하기 
	nCenY = rtArea[0].top + ((rtArea[0].Height() - (nY * nMaxRow)) / 2) - 1;
	// grid size 계산하여 중심에 다시 그리기 
	GetDlgItem(IDC_CUSTOM_RETEST_BUF_FIRST_1)->MoveWindow(nCenX, nCenY, (nX * nMaxCol) + 5, (nY * nMaxRow) + 5, TRUE);

	for (i=0; i<nMaxRow; i++)
	{
		
// 		if (i == 0)
// 		{
// 			m_pGridRetestBufFirst[0].SetRowHeight(i, 40);
// 		}
// 		else
// 		{
// 			m_pGridRetestBufFirst[0].SetRowHeight(i, 60);
// 		}
		if (i == 0)
		{
			m_pGridRetestBufFirst[0].MergeCells(i, 0, i, 1);
			m_pGridRetestBufFirst[0].SetItemBkColour(i, 0, RGB(219, 229, 241), BLACK_C);
			m_pGridRetestBufFirst[0].SetItemFont(i, 0, &clsFunc.OnLogFont(14));
			m_pGridRetestBufFirst[0].SetItemText(i, 0, _T("Retest Buff #1"));
		}

		m_pGridRetestBufFirst[0].SetRowHeight(i, nY);

		for (j=0; j<nMaxCol; j++)
		{
			m_pGridRetestBufFirst[0].SetColumnWidth(j, nX );
			if (i ==0)
			{
			}
			else
			{
				m_pGridRetestBufFirst[0].SetItemBkColour(i, j, BLACK_L, WHITE_C);
				m_pGridRetestBufFirst[0].SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
				m_pGridRetestBufFirst[0].SetItemFont(i, j, &clsFunc.OnLogFont(13));
				m_pGridRetestBufFirst[0].SetItemState(i, j, GVIS_READONLY);
			}
			//m_pGridRetestBufFirst[0].SetItemFont(i, j, &clsFunc.OnLogFont(12));
		}
	}

	m_pGridRetestBufFirst[0].Invalidate(FALSE);
}

// void CScreenMain::OnInitGridRetestBufFirst2()
// {
// 	CRect rtArea[2];
// 	int   nX, nY;
// 	int   nWidth, nHeight;
// 	int i, j;
// 	int nMaxRow, nMaxCol;
// 	int	nCenX, nCenY;
// //	int nRow, nCol;
// 
// 	CRect rect;
// 	CString strTemp;
// 
// 	nMaxRow = st_recipe_info.nRetestBuffer_Num / 2;	
// 	nMaxCol = 2;
// 	if (nMaxCol <= 0) nMaxCol = 1;
// 	if (nMaxRow <= 0) nMaxRow = 1;
// 
// 	m_pGridRetestBufFirst[1].SetFrameFocusCell(FALSE);
// 	m_pGridRetestBufFirst[1].SetTrackFocusCell(FALSE);
// 	m_pGridRetestBufFirst[1].EnableSelection(FALSE);
// 				   
// 	m_pGridRetestBufFirst[1].SetGridLineColor(BLACK_C);
// 	m_pGridRetestBufFirst[1].SetGridLines(1);
// 				   
// 	m_pGridRetestBufFirst[1].SetRowCount(nMaxRow);
// 	m_pGridRetestBufFirst[1].SetColumnCount(nMaxCol);
// 				   
// 	m_pGridRetestBufFirst[1].SetFixedRowCount(0);
// 	m_pGridRetestBufFirst[1].SetFixedColumnCount(0);
// 	m_pGridRetestBufFirst[1].SetFixedBkColor(RGB(0,0,200));
// 	m_pGridRetestBufFirst[1].SetFixedBkColor(RGB(200,200,255));
// 	m_pGridRetestBufFirst[1].SetTextBkColor(RGB(150,150,200));
// 
// 	GetDlgItem(IDC_MSG_RETEST_BUFFER_FIRST_2)->GetWindowRect(rtArea[0]); // group box rect 구하기 
// 	ScreenToClient(rtArea[0]); // window상에서 group box 좌표 구하기
// 
// 	nWidth	= rtArea[0].Width() - 10;
// 	nHeight	= rtArea[0].Height() - 3;
// 
// 	nX	= nWidth / nMaxCol;
// 
// 	if (nMaxRow > 0)	
// 	{
// 		nY = nHeight / nMaxRow;
// 	}
// 	else	
// 	{
// 		nY = 1;
// 	}
// 
// 	// group box에서 grid 센터에 표시하기 위한 x좌표 구하기 
// 	nCenX = rtArea[0].left + ((rtArea[0].Width() - (nX * nMaxCol)) / 2) - 2;
// 	// group box에서 grid 센터에 표시하기 위한 y좌표 구하기 
// 	nCenY = rtArea[0].top + ((rtArea[0].Height() - (nY * nMaxRow)) / 2) - 1;
// 	// grid size 계산하여 중심에 다시 그리기 
// 	GetDlgItem(IDC_CUSTOM_RETEST_BUF_FIRST_2)->MoveWindow(nCenX, nCenY, (nX * nMaxCol) + 5, (nY * nMaxRow) + 5, TRUE);
// 
// 	for (i=0; i<nMaxRow; i++)
// 	{
// 		m_pGridRetestBufFirst[1].SetRowHeight(i, nY);
// 
// 		for (j=0; j<nMaxCol; j++)
// 		{
// 			m_pGridRetestBufFirst[1].SetColumnWidth(j, nX);
// 			m_pGridRetestBufFirst[1].SetItemBkColour(i, j, BLACK_L, WHITE_C);
// 			m_pGridRetestBufFirst[1].SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
// 			m_pGridRetestBufFirst[1].SetItemFont(i, j, &clsFunc.OnLogFont(14));
// 			m_pGridRetestBufFirst[1].SetItemState(i, j, GVIS_READONLY);
// 
// 			m_pGridRetestBufFirst[1].SetItemFont(i, j, &clsFunc.OnLogFont(12));
// 		}
// 	}
// 
// 	m_pGridRetestBufFirst[1].Invalidate(FALSE);
// }

void CScreenMain::OnInitGridRetestBufSecond1()
{
	CRect rtArea[2];
	int   nX, nY;
	int   nWidth, nHeight;
	int i, j;
	int nMaxRow, nMaxCol;
	int	nCenX, nCenY;
//	int nRow, nCol;

	CRect rect;
	CString strTemp;

// 	nMaxRow = st_recipe_info.nRetestBuffer_Num / 2;	
// 	if ((nMaxRow * 2) != st_recipe_info.nRetestBuffer_Num) nMaxRow++;
// 
// 	nMaxCol = 2;
// 	if (nMaxCol <= 0) nMaxCol = 1;
	nMaxRow = st_recipe_info.nRetestBuffer_Num + 1;	
	nMaxCol = 2; 
	m_pGridRetestBufSecond[0].SetFrameFocusCell(FALSE);
	m_pGridRetestBufSecond[0].SetTrackFocusCell(FALSE);
	m_pGridRetestBufSecond[0].EnableSelection(FALSE);
				   
	m_pGridRetestBufSecond[0].SetGridLineColor(BLACK_C);
	m_pGridRetestBufSecond[0].SetGridLines(1);
				   
	m_pGridRetestBufSecond[0].SetRowCount(nMaxRow);
	m_pGridRetestBufSecond[0].SetColumnCount(nMaxCol);
				   
	m_pGridRetestBufSecond[0].SetFixedRowCount(0);
	m_pGridRetestBufSecond[0].SetFixedColumnCount(0);
	m_pGridRetestBufSecond[0].SetFixedBkColor(RGB(0,0,200));
	m_pGridRetestBufSecond[0].SetFixedBkColor(RGB(200,200,255));
	m_pGridRetestBufSecond[0].SetTextBkColor(RGB(150,150,200));

	GetDlgItem(IDC_MSG_RETEST_BUFFER_SECOND_1)->GetWindowRect(rtArea[0]); // group box rect 구하기 
	ScreenToClient(rtArea[0]); // window상에서 group box 좌표 구하기

	nWidth	= rtArea[0].Width() - 10;
	nHeight	= rtArea[0].Height() - 5;

	nX	= nWidth / nMaxCol;

	if (nMaxRow > 0)	
	{
		nY = nHeight / nMaxRow;
	}
	else	
	{
		nY = 1;
	}

	// group box에서 grid 센터에 표시하기 위한 x좌표 구하기 
	nCenX = rtArea[0].left + ((rtArea[0].Width() - (nX * nMaxCol)) / 2) - 2;
	// group box에서 grid 센터에 표시하기 위한 y좌표 구하기 
	nCenY = rtArea[0].top + ((rtArea[0].Height() - (nY * nMaxRow)) / 2) - 1;
	// grid size 계산하여 중심에 다시 그리기 
	GetDlgItem(IDC_CUSTOM_RETEST_BUF_SECOND_1)->MoveWindow(nCenX, nCenY, (nX * nMaxCol) + 5, (nY * nMaxRow) + 5, TRUE);

	for (i=0; i<nMaxRow; i++)
	{
// 		if (i == 0)
// 		{
// 			m_pGridRetestBufSecond[0].SetRowHeight(i, 40);
// 		}
// 		else
// 		{
// 			m_pGridRetestBufSecond[0].SetRowHeight(i, 60);
// 		}
		if (i == 0)
		{
			m_pGridRetestBufSecond[0].MergeCells(i, 0, i, 1);
			m_pGridRetestBufSecond[0].SetItemBkColour(i, 0, RGB(219, 229, 241), BLACK_C);
			m_pGridRetestBufSecond[0].SetItemFont(i, 0, &clsFunc.OnLogFont(14));
			m_pGridRetestBufSecond[0].SetItemText(i, 0, _T("Retest Buff #2"));
		}

		m_pGridRetestBufSecond[0].SetRowHeight(i, nY);

		for (j=0; j<nMaxCol; j++)
		{


			m_pGridRetestBufSecond[0].SetColumnWidth(j, nX );

			if (i ==0)
			{
			}
			else
			{
				m_pGridRetestBufSecond[0].SetItemBkColour(i, j, BLACK_L, WHITE_C);
				m_pGridRetestBufSecond[0].SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
				m_pGridRetestBufSecond[0].SetItemFont(i, j, &clsFunc.OnLogFont(13));
				m_pGridRetestBufSecond[0].SetItemState(i, j, GVIS_READONLY);
			}
			//m_pGridRetestBufSecond[0].SetItemFont(i, j, &clsFunc.OnLogFont(12));
		}
	}

	m_pGridRetestBufSecond[0].Invalidate(FALSE);
}


// void CScreenMain::OnInitGridRetestBufSecond2()
// {
// 	CRect rtArea[2];
// 	int   nX, nY;
// 	int   nWidth, nHeight;
// 	int i, j;
// 	int nMaxRow, nMaxCol;
// 	int	nCenX, nCenY;
// //	int nRow, nCol;
// 
// 	CRect rect;
// 	CString strTemp;
// 
// 	nMaxRow = st_recipe_info.nRetestBuffer_Num / 2;	
// 	nMaxCol = 2;
// 	if (nMaxCol <= 0) nMaxCol = 1;
// 	if (nMaxRow <= 0) nMaxRow = 1;
// 
// 	m_pGridRetestBufSecond[1].SetFrameFocusCell(FALSE);
// 	m_pGridRetestBufSecond[1].SetTrackFocusCell(FALSE);
// 	m_pGridRetestBufSecond[1].EnableSelection(FALSE);
// 				   
// 	m_pGridRetestBufSecond[1].SetGridLineColor(BLACK_C);
// 	m_pGridRetestBufSecond[1].SetGridLines(1);
// 				   
// 	m_pGridRetestBufSecond[1].SetRowCount(nMaxRow);
// 	m_pGridRetestBufSecond[1].SetColumnCount(nMaxCol);
// 				   
// 	m_pGridRetestBufSecond[1].SetFixedRowCount(0);
// 	m_pGridRetestBufSecond[1].SetFixedColumnCount(0);
// 	m_pGridRetestBufSecond[1].SetFixedBkColor(RGB(0,0,200));
// 	m_pGridRetestBufSecond[1].SetFixedBkColor(RGB(200,200,255));
// 	m_pGridRetestBufSecond[1].SetTextBkColor(RGB(150,150,200));
// 
// 	GetDlgItem(IDC_MSG_RETEST_BUFFER_SECOND_2)->GetWindowRect(rtArea[0]); // group box rect 구하기 
// 	ScreenToClient(rtArea[0]); // window상에서 group box 좌표 구하기
// 
// 	nWidth	= rtArea[0].Width() - 10;
// 	nHeight	= rtArea[0].Height() - 3;
// 
// 	nX	= nWidth / nMaxCol;
// 
// 	if (nMaxRow > 0)	
// 	{
// 		nY = nHeight / nMaxRow;
// 	}
// 	else	
// 	{
// 		nY = 1;
// 	}
// 
// 	// group box에서 grid 센터에 표시하기 위한 x좌표 구하기 
// 	nCenX = rtArea[0].left + ((rtArea[0].Width() - (nX * nMaxCol)) / 2) - 2;
// 	// group box에서 grid 센터에 표시하기 위한 y좌표 구하기 
// 	nCenY = rtArea[0].top + ((rtArea[0].Height() - (nY * nMaxRow)) / 2) - 1;
// 	// grid size 계산하여 중심에 다시 그리기 
// 	GetDlgItem(IDC_CUSTOM_RETEST_BUF_SECOND_2)->MoveWindow(nCenX, nCenY, (nX * nMaxCol) + 5, (nY * nMaxRow) + 5, TRUE);
// 
// 	for (i=0; i<nMaxRow; i++)
// 	{
// 		m_pGridRetestBufSecond[1].SetRowHeight(i, nY);
// 
// 		for (j=0; j<nMaxCol; j++)
// 		{
// 			m_pGridRetestBufSecond[1].SetColumnWidth(j, nX);
// 			m_pGridRetestBufSecond[1].SetItemBkColour(i, j, BLACK_L, WHITE_C);
// 			m_pGridRetestBufSecond[1].SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
// 			m_pGridRetestBufSecond[1].SetItemFont(i, j, &clsFunc.OnLogFont(14));
// 			m_pGridRetestBufSecond[1].SetItemState(i, j, GVIS_READONLY);
// 
// 			m_pGridRetestBufSecond[1].SetItemFont(i, j, &clsFunc.OnLogFont(12));
// 		}
// 	}
// 
// 	m_pGridRetestBufSecond[1].Invalidate(FALSE);
// }

void CScreenMain::OnInitGridTestSiteTop()
{
	int i, j, k;
	int nRow, nCol;
	int nMaxRow, nMaxCol;
	CString strTemp;

	nMaxRow = 10;	
	nMaxCol = 2;
	if (nMaxCol <= 0) nMaxCol = 1;

	for (i=0; i<4; i++)
	{
		m_pGridTestSiteTop[i].SetFrameFocusCell(FALSE);
		m_pGridTestSiteTop[i].SetTrackFocusCell(FALSE);
		m_pGridTestSiteTop[i].EnableSelection(FALSE);
				   
		m_pGridTestSiteTop[i].SetGridLineColor(BLACK_C);
		m_pGridTestSiteTop[i].SetGridLines(1);
				   
		m_pGridTestSiteTop[i].SetRowCount(nMaxRow);
		m_pGridTestSiteTop[i].SetColumnCount(nMaxCol);
				   
		m_pGridTestSiteTop[i].SetFixedRowCount(0);
		m_pGridTestSiteTop[i].SetFixedColumnCount(0);
		m_pGridTestSiteTop[i].SetFixedBkColor(RGB(0,0,200));
		m_pGridTestSiteTop[i].SetFixedBkColor(RGB(200,200,255));
		m_pGridTestSiteTop[i].SetTextBkColor(RGB(150,150,200));

		for (j=0; j<nMaxRow; j++)
		{
			m_pGridTestSiteTop[i].SetRowHeight(j, 30);

			for (k=0; k<nMaxCol; k++)
			{
				if (k == 0) m_pGridTestSiteTop[i].SetColumnWidth(k, 27);
				else m_pGridTestSiteTop[i].SetColumnWidth(k, 100);

				m_pGridTestSiteTop[i].SetItemBkColour(j, k, BLACK_L, WHITE_C);
				m_pGridTestSiteTop[i].SetItemFormat(j, k, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
				m_pGridTestSiteTop[i].SetItemFont(j, k, &clsFunc.OnLogFont(14));
				m_pGridTestSiteTop[i].SetItemState(j, k, GVIS_READONLY);

				// jtkim 20161002
				if (j > 0 && k > 0)
				{
					m_pGridTestSiteTop[i].SetItemFont(j, k, &clsFunc.OnLogFont(10));
				}
			}
		}


		nRow = 0;
		nCol = 0;
		//m_pGridTestSiteTop[i].MergeCells(nRow, nCol, nRow, nCol + 1);
		m_pGridTestSiteTop[i].SetItemBkColour(nRow, nCol, BLUE_D, WHITE_C);
		m_pGridTestSiteTop[i].SetItemFont(nRow, nCol, &clsFunc.OnLogFont(14));
		strTemp.Format(_T("%02d"), i + 1);
		m_pGridTestSiteTop[i].SetItemText(nRow, nCol, strTemp);

		nRow = 0;
		nCol = 1;
		m_pGridTestSiteTop[i].SetItemBkColour(nRow, nCol, BLUE_D, WHITE_C);
		m_pGridTestSiteTop[i].SetItemFont(nRow, nCol, &clsFunc.OnLogFont(13));
		strTemp.Format(_T("Test Time"));
		m_pGridTestSiteTop[i].SetItemText(nRow, nCol, strTemp);


		nRow = 1;
		nCol = 0;
		m_pGridTestSiteTop[i].MergeCells(nRow, nCol, nRow, nCol + 1);
		m_pGridTestSiteTop[i].SetItemBkColour(nRow, nCol, YELLOW_L, BLACK_C);
		m_pGridTestSiteTop[i].SetItemFont(nRow, nCol, &clsFunc.OnLogFont(12));
		m_pGridTestSiteTop[i].SetItemText(nRow, nCol, _T("-"));

		nRow = 2;
		nCol = 0;
		m_pGridTestSiteTop[i].SetItemBkColour(nRow, nCol, BLUE_D, WHITE_C);
		m_pGridTestSiteTop[i].SetItemFont(nRow, nCol, &clsFunc.OnLogFont(14));
		m_pGridTestSiteTop[i].SetItemText(nRow, nCol, _T("8"));

		nRow = 3;
		nCol = 0;
		m_pGridTestSiteTop[i].SetItemBkColour(nRow, nCol, BLUE_D, WHITE_C);
		m_pGridTestSiteTop[i].SetItemFont(nRow, nCol, &clsFunc.OnLogFont(14));
		m_pGridTestSiteTop[i].SetItemText(nRow, nCol, _T("7"));

		nRow = 4;
		nCol = 0;
		m_pGridTestSiteTop[i].SetItemBkColour(nRow, nCol, BLUE_D, WHITE_C);
		m_pGridTestSiteTop[i].SetItemFont(nRow, nCol, &clsFunc.OnLogFont(14));
		m_pGridTestSiteTop[i].SetItemText(nRow, nCol, _T("6"));

		nRow = 5;
		nCol = 0;
		m_pGridTestSiteTop[i].SetItemBkColour(nRow, nCol, BLUE_D, WHITE_C);
		m_pGridTestSiteTop[i].SetItemFont(nRow, nCol, &clsFunc.OnLogFont(14));
		m_pGridTestSiteTop[i].SetItemText(nRow, nCol, _T("5"));

		nRow = 6;
		nCol = 0;
		m_pGridTestSiteTop[i].SetItemBkColour(nRow, nCol, BLUE_D, WHITE_C);
		m_pGridTestSiteTop[i].SetItemFont(nRow, nCol, &clsFunc.OnLogFont(14));
		m_pGridTestSiteTop[i].SetItemText(nRow, nCol, _T("4"));

		nRow = 7;
		nCol = 0;
		m_pGridTestSiteTop[i].SetItemBkColour(nRow, nCol, BLUE_D, WHITE_C);
		m_pGridTestSiteTop[i].SetItemFont(nRow, nCol, &clsFunc.OnLogFont(14));
		m_pGridTestSiteTop[i].SetItemText(nRow, nCol, _T("3"));

		nRow = 8;
		nCol = 0;
		m_pGridTestSiteTop[i].SetItemBkColour(nRow, nCol, BLUE_D, WHITE_C);
		m_pGridTestSiteTop[i].SetItemFont(nRow, nCol, &clsFunc.OnLogFont(14));
		m_pGridTestSiteTop[i].SetItemText(nRow, nCol, _T("2"));

		nRow = 9;
		nCol = 0;
		m_pGridTestSiteTop[i].SetItemBkColour(nRow, nCol, BLUE_D, WHITE_C);
		m_pGridTestSiteTop[i].SetItemFont(nRow, nCol, &clsFunc.OnLogFont(14));
		m_pGridTestSiteTop[i].SetItemText(nRow, nCol, _T("1"));

		m_pGridTestSiteTop[i].Invalidate(FALSE);
	}
}


void CScreenMain::OnInitGridTestSiteBottom()
{
	int i, j, k;
	int nMaxRow, nMaxCol;
	int nRow, nCol;

	CString strTemp;

	nMaxRow = 10;	
	nMaxCol = 2;
	if (nMaxCol <= 0) nMaxCol = 1;

	for (i=0; i<4; i++)
	{
		m_pGridTestSiteBottom[i].SetFrameFocusCell(FALSE);
		m_pGridTestSiteBottom[i].SetTrackFocusCell(FALSE);
		m_pGridTestSiteBottom[i].EnableSelection(FALSE);
				   
		m_pGridTestSiteBottom[i].SetGridLineColor(BLACK_C);
		m_pGridTestSiteBottom[i].SetGridLines(1);
				   
		m_pGridTestSiteBottom[i].SetRowCount(nMaxRow);
		m_pGridTestSiteBottom[i].SetColumnCount(nMaxCol);
				   
		m_pGridTestSiteBottom[i].SetFixedRowCount(0);
		m_pGridTestSiteBottom[i].SetFixedColumnCount(0);
		m_pGridTestSiteBottom[i].SetFixedBkColor(RGB(0,0,200));
		m_pGridTestSiteBottom[i].SetFixedBkColor(RGB(200,200,255));
		m_pGridTestSiteBottom[i].SetTextBkColor(RGB(150,150,200));

		for (j=0; j<nMaxRow; j++)
		{
			m_pGridTestSiteBottom[i].SetRowHeight(j, 30);

			for (k=0; k<nMaxCol; k++)
			{
				if (k == 0) m_pGridTestSiteBottom[i].SetColumnWidth(k, 27);
				else m_pGridTestSiteBottom[i].SetColumnWidth(k, 100);

				m_pGridTestSiteBottom[i].SetItemBkColour(j, k, BLACK_L, WHITE_C);
				m_pGridTestSiteBottom[i].SetItemFormat(j, k, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
				m_pGridTestSiteBottom[i].SetItemFont(j, k, &clsFunc.OnLogFont(14));
				m_pGridTestSiteBottom[i].SetItemState(j, k, GVIS_READONLY);

				// jtkim 20161002
				if (j > 0 && k > 0)
				{
					m_pGridTestSiteBottom[i].SetItemFont(j, k, &clsFunc.OnLogFont(10));
				}
			}
		}

		nRow = 0;
		nCol = 0;
		//m_pGridTestSiteBottom[i].MergeCells(nRow, nCol, nRow, nCol + 1);
		m_pGridTestSiteBottom[i].SetItemBkColour(nRow, nCol, BLUE_D, WHITE_C);
		m_pGridTestSiteBottom[i].SetItemFont(nRow, nCol, &clsFunc.OnLogFont(14));
		strTemp.Format(_T("%02d"), i + 5);
		m_pGridTestSiteBottom[i].SetItemText(nRow, nCol, strTemp);

		nRow = 0;
		nCol = 1;
		m_pGridTestSiteBottom[i].SetItemBkColour(nRow, nCol, BLUE_D, WHITE_C);
		m_pGridTestSiteBottom[i].SetItemFont(nRow, nCol, &clsFunc.OnLogFont(13));
		strTemp.Format(_T("Test Time"));
		m_pGridTestSiteBottom[i].SetItemText(nRow, nCol, strTemp);


		nRow = 1;
		nCol = 0;
		m_pGridTestSiteBottom[i].MergeCells(nRow, nCol, nRow, nCol + 1);
		m_pGridTestSiteBottom[i].SetItemBkColour(nRow, nCol, YELLOW_L, BLACK_C);
		m_pGridTestSiteBottom[i].SetItemFont(nRow, nCol, &clsFunc.OnLogFont(12));
		m_pGridTestSiteBottom[i].SetItemText(nRow, nCol, _T("-"));

		nRow = 2;
		nCol = 0;
		m_pGridTestSiteBottom[i].SetItemBkColour(nRow, nCol, BLUE_D, WHITE_C);
		m_pGridTestSiteBottom[i].SetItemFont(nRow, nCol, &clsFunc.OnLogFont(14));
		m_pGridTestSiteBottom[i].SetItemText(nRow, nCol, _T("1"));

		nRow = 3;
		nCol = 0;
		m_pGridTestSiteBottom[i].SetItemBkColour(nRow, nCol, BLUE_D, WHITE_C);
		m_pGridTestSiteBottom[i].SetItemFont(nRow, nCol, &clsFunc.OnLogFont(14));
		m_pGridTestSiteBottom[i].SetItemText(nRow, nCol, _T("2"));

		nRow = 4;
		nCol = 0;
		m_pGridTestSiteBottom[i].SetItemBkColour(nRow, nCol, BLUE_D, WHITE_C);
		m_pGridTestSiteBottom[i].SetItemFont(nRow, nCol, &clsFunc.OnLogFont(14));
		m_pGridTestSiteBottom[i].SetItemText(nRow, nCol, _T("3"));

		nRow = 5;
		nCol = 0;
		m_pGridTestSiteBottom[i].SetItemBkColour(nRow, nCol, BLUE_D, WHITE_C);
		m_pGridTestSiteBottom[i].SetItemFont(nRow, nCol, &clsFunc.OnLogFont(14));
		m_pGridTestSiteBottom[i].SetItemText(nRow, nCol, _T("4"));

		nRow = 6;
		nCol = 0;
		m_pGridTestSiteBottom[i].SetItemBkColour(nRow, nCol, BLUE_D, WHITE_C);
		m_pGridTestSiteBottom[i].SetItemFont(nRow, nCol, &clsFunc.OnLogFont(14));
		m_pGridTestSiteBottom[i].SetItemText(nRow, nCol, _T("5"));

		nRow = 7;
		nCol = 0;
		m_pGridTestSiteBottom[i].SetItemBkColour(nRow, nCol, BLUE_D, WHITE_C);
		m_pGridTestSiteBottom[i].SetItemFont(nRow, nCol, &clsFunc.OnLogFont(14));
		m_pGridTestSiteBottom[i].SetItemText(nRow, nCol, _T("6"));

		nRow = 8;
		nCol = 0;
		m_pGridTestSiteBottom[i].SetItemBkColour(nRow, nCol, BLUE_D, WHITE_C);
		m_pGridTestSiteBottom[i].SetItemFont(nRow, nCol, &clsFunc.OnLogFont(14));
		m_pGridTestSiteBottom[i].SetItemText(nRow, nCol, _T("7"));

		nRow = 9;
		nCol = 0;
		m_pGridTestSiteBottom[i].SetItemBkColour(nRow, nCol, BLUE_D, WHITE_C);
		m_pGridTestSiteBottom[i].SetItemFont(nRow, nCol, &clsFunc.OnLogFont(14));
		m_pGridTestSiteBottom[i].SetItemText(nRow, nCol, _T("8"));

		m_pGridTestSiteBottom[i].Invalidate(FALSE);
	}
}

void CScreenMain::OnInitGridRefernce()
{
	int   i, j;
	int	  nMaxRow, nMaxCol;
	int   nRow;
	CString strTmp;

	nMaxRow = 1; // unloader tray y count
	//kwlee 2016.1103
	//nMaxCol = 8;
	//nMaxCol = 10;
	nMaxCol = 12; //kwlee 2016.1201
	
	m_pGridRefernce.SetFrameFocusCell(FALSE);
	m_pGridRefernce.SetTrackFocusCell(FALSE);
	m_pGridRefernce.EnableSelection(FALSE);

	m_pGridRefernce.SetGridLineColor(BLACK_C);
	m_pGridRefernce.SetGridLines(1);

	m_pGridRefernce.SetRowCount(nMaxRow);
	m_pGridRefernce.SetColumnCount(nMaxCol);

	m_pGridRefernce.SetFixedRowCount(0);
	m_pGridRefernce.SetFixedColumnCount(0);
	m_pGridRefernce.SetFixedBkColor(RGB(0,0,200));
	m_pGridRefernce.SetFixedBkColor(RGB(200,200,255));
	m_pGridRefernce.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<nMaxRow; i++) 
	{
		m_pGridRefernce.SetRowHeight(i, 40);

		for (j=0; j<nMaxCol; j++) 
		{
			m_pGridRefernce.SetColumnWidth(j, 44);
				
			m_pGridRefernce.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridRefernce.SetItemState(i, j, GVIS_READONLY);
			m_pGridRefernce.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
		}
	}

	nRow = 0;
	m_pGridRefernce.SetItemBkColour(nRow, 0, BIN_OFF, BLACK_C);
	m_pGridRefernce.SetItemFont(nRow, 0, &clsFunc.OnLogFont(12));
	m_pGridRefernce.SetItemText(nRow, 0, _T("Off"));

	m_pGridRefernce.SetItemBkColour(nRow, 1, BIN_LOAD, BLACK_C);
	m_pGridRefernce.SetItemFont(nRow, 1, &clsFunc.OnLogFont(12));
	m_pGridRefernce.SetItemText(nRow, 1, _T("Load"));

	m_pGridRefernce.SetItemBkColour(nRow, 2, BIN_READY, BLACK_C);
	m_pGridRefernce.SetItemFont(nRow, 2, &clsFunc.OnLogFont(12));
	m_pGridRefernce.SetItemText(nRow, 2, _T("Ready"));

	m_pGridRefernce.SetItemBkColour(nRow, 3, BIN_START, BLACK_C);
	m_pGridRefernce.SetItemFont(nRow, 3, &clsFunc.OnLogFont(12));
	m_pGridRefernce.SetItemText(nRow, 3, _T("Test"));

	m_pGridRefernce.SetItemBkColour(nRow, 4, BIN_ABORT, BLACK_C);
	m_pGridRefernce.SetItemFont(nRow, 4, &clsFunc.OnLogFont(12));
	m_pGridRefernce.SetItemText(nRow, 4, _T("Abort"));

	m_pGridRefernce.SetItemBkColour(nRow, 5, BIN_GOOD, BLACK_C);
	m_pGridRefernce.SetItemFont(nRow, 5, &clsFunc.OnLogFont(12));
	m_pGridRefernce.SetItemText(nRow, 5, _T("Pass"));

	m_pGridRefernce.SetItemBkColour(nRow, 6, BIN_FAIL, BLACK_C);
	m_pGridRefernce.SetItemFont(nRow, 6, &clsFunc.OnLogFont(12));
	m_pGridRefernce.SetItemText(nRow, 6, _T("Fail"));

	//kwlee 2016.1201
	m_pGridRefernce.SetItemBkColour(nRow, 7, CONTINUE_FAIL, BLACK_C);
	m_pGridRefernce.SetItemFont(nRow, 7, &clsFunc.OnLogFont(12));
	m_pGridRefernce.SetItemText(nRow, 7, _T("ContiFail"));

	m_pGridRefernce.SetItemBkColour(nRow, 8, OPT_FAIL, BLACK_C);
	m_pGridRefernce.SetItemFont(nRow, 8, &clsFunc.OnLogFont(12));
	m_pGridRefernce.SetItemText(nRow, 8, _T("OptFail"));
	///

	m_pGridRefernce.SetItemBkColour(nRow, 9, BIN_RELOAD, BLACK_C);
	m_pGridRefernce.SetItemFont(nRow, 9, &clsFunc.OnLogFont(12));
	m_pGridRefernce.SetItemText(nRow, 9, _T("Reload"));
	
	m_pGridRefernce.SetItemBkColour(nRow, 10, YELLOW_L, BLACK_C);
	m_pGridRefernce.SetItemFont(nRow, 10, &clsFunc.OnLogFont(12));
	m_pGridRefernce.SetItemText(nRow, 10, _T("Curr Lot"));

	m_pGridRefernce.SetItemBkColour(nRow, 11, ORANGE_C, BLACK_C);
	m_pGridRefernce.SetItemFont(nRow, 11, &clsFunc.OnLogFont(12));
	m_pGridRefernce.SetItemText(nRow, 11, _T("Next Lot"));

	m_pGridRefernce.Invalidate(FALSE);
}


void CScreenMain::OnInitGridYieldLot()
{
	int   i, j;
	int	  max_row, max_col;
	int   row;

	CString strTmp;
	CRect rect;

	max_row = 6; // unloader tray y count
	max_col = 3;

	m_pGridYieldLot.SetFrameFocusCell(FALSE);
	m_pGridYieldLot.SetTrackFocusCell(FALSE);
	m_pGridYieldLot.EnableSelection(FALSE);

	m_pGridYieldLot.SetGridLineColor(BLACK_C);
	m_pGridYieldLot.SetGridLines(1);

	m_pGridYieldLot.SetRowCount(max_row);
	m_pGridYieldLot.SetColumnCount(max_col);

	m_pGridYieldLot.SetFixedRowCount(0);
	m_pGridYieldLot.SetFixedColumnCount(0);
	m_pGridYieldLot.SetFixedBkColor(RGB(0,0,200));
	m_pGridYieldLot.SetFixedBkColor(RGB(200,200,255));
	m_pGridYieldLot.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<max_row; i++) 
	{
		m_pGridYieldLot.SetRowHeight(i, 22);

		for (j=0; j<max_col; j++) 
		{
			m_pGridYieldLot.SetColumnWidth(j, 80);
				
			m_pGridYieldLot.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridYieldLot.SetItemState(i, j, GVIS_READONLY);
			m_pGridYieldLot.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
		}
	}

	row = 0;
	m_pGridYieldLot.MergeCells(row, 0, row, 2);
	m_pGridYieldLot.SetItemBkColour(row, 0, RGB(219, 229, 241), BLACK_C);
	m_pGridYieldLot.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridYieldLot.SetItemText(row, 0, _T("Lot Yield Info"));

	row = 1;
	m_pGridYieldLot.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
	m_pGridYieldLot.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridYieldLot.SetItemText(row, 0, _T(""));

	m_pGridYieldLot.SetItemBkColour(row, 1, BLUE_D, WHITE_C);
	m_pGridYieldLot.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
	m_pGridYieldLot.SetItemText(row, 1, _T("Prime"));

	m_pGridYieldLot.SetItemBkColour(row, 2, BLUE_D, WHITE_C);
	m_pGridYieldLot.SetItemFont(row, 2, &clsFunc.OnLogFont(16));
	m_pGridYieldLot.SetItemText(row, 2, _T("Cum"));

	row = 2;
	m_pGridYieldLot.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
	m_pGridYieldLot.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridYieldLot.SetItemText(row, 0, _T("Input"));

	m_pGridYieldLot.MergeCells(row, 1, row, 2);
	m_pGridYieldLot.SetItemBkColour(row, 1, WHITE_C, BLACK_C);
	m_pGridYieldLot.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
	m_pGridYieldLot.SetItemText(row, 1, _T("0"));

	row = 3;
	m_pGridYieldLot.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
	m_pGridYieldLot.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridYieldLot.SetItemText(row, 0, _T("Pass"));

	m_pGridYieldLot.SetItemBkColour(row, 1, GREEN_L, BLACK_C);
	m_pGridYieldLot.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
	m_pGridYieldLot.SetItemText(row, 1, _T("0"));

	m_pGridYieldLot.SetItemBkColour(row, 2, GREEN_L, BLACK_C);
	m_pGridYieldLot.SetItemFont(row, 2, &clsFunc.OnLogFont(16));
	m_pGridYieldLot.SetItemText(row, 2, _T("0"));

	row = 4;
	m_pGridYieldLot.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
	m_pGridYieldLot.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridYieldLot.SetItemText(row, 0, _T("Fail"));

	m_pGridYieldLot.SetItemBkColour(row, 1, RED_L, BLACK_C);
	m_pGridYieldLot.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
	m_pGridYieldLot.SetItemText(row, 1, _T("0"));

	m_pGridYieldLot.SetItemBkColour(row, 2, RED_L, BLACK_C);
	m_pGridYieldLot.SetItemFont(row, 2, &clsFunc.OnLogFont(16));
	m_pGridYieldLot.SetItemText(row, 2, _T("0"));

	row = 5;
	m_pGridYieldLot.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
	m_pGridYieldLot.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridYieldLot.SetItemText(row, 0, _T("Yield"));

	m_pGridYieldLot.SetItemBkColour(row, 1, GREEN_L, BLACK_C);
	m_pGridYieldLot.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
	m_pGridYieldLot.SetItemText(row, 1, _T("0"));

	m_pGridYieldLot.SetItemBkColour(row, 2, GREEN_L, BLACK_C);
	m_pGridYieldLot.SetItemFont(row, 2, &clsFunc.OnLogFont(16));
	m_pGridYieldLot.SetItemText(row, 2, _T("0"));
}


void CScreenMain::OnInitGridYieldDay()
{
	int   i, j;
	int	  max_row, max_col;
	int   row;

	CString strTmp;
	CRect rect;

	max_row = 6; // unloader tray y count
	max_col = 3;

	m_pGridYieldDay.SetFrameFocusCell(FALSE);
	m_pGridYieldDay.SetTrackFocusCell(FALSE);
	m_pGridYieldDay.EnableSelection(FALSE);

	m_pGridYieldDay.SetGridLineColor(BLACK_C);
	m_pGridYieldDay.SetGridLines(1);

	m_pGridYieldDay.SetRowCount(max_row);
	m_pGridYieldDay.SetColumnCount(max_col);

	m_pGridYieldDay.SetFixedRowCount(0);
	m_pGridYieldDay.SetFixedColumnCount(0);
	m_pGridYieldDay.SetFixedBkColor(RGB(0,0,200));
	m_pGridYieldDay.SetFixedBkColor(RGB(200,200,255));
	m_pGridYieldDay.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<max_row; i++) 
	{
		m_pGridYieldDay.SetRowHeight(i, 22);

		for (j=0; j<max_col; j++) 
		{
			m_pGridYieldDay.SetColumnWidth(j, 80);
				
			m_pGridYieldDay.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridYieldDay.SetItemState(i, j, GVIS_READONLY);
			m_pGridYieldDay.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
		}
	}

	row = 0;
	m_pGridYieldDay.MergeCells(row, 0, row, 2);
	m_pGridYieldDay.SetItemBkColour(row, 0, RGB(219, 229, 241), BLACK_C);
	m_pGridYieldDay.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridYieldDay.SetItemText(row, 0, _T("Daily Yield Info"));

	row = 1;
	m_pGridYieldDay.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
	m_pGridYieldDay.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridYieldDay.SetItemText(row, 0, _T(""));

	m_pGridYieldDay.SetItemBkColour(row, 1, BLUE_D, WHITE_C);
	m_pGridYieldDay.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
	m_pGridYieldDay.SetItemText(row, 1, _T("Prime"));

	m_pGridYieldDay.SetItemBkColour(row, 2, BLUE_D, WHITE_C);
	m_pGridYieldDay.SetItemFont(row, 2, &clsFunc.OnLogFont(16));
	m_pGridYieldDay.SetItemText(row, 2, _T("Cum"));

	row = 2;
	m_pGridYieldDay.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
	m_pGridYieldDay.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridYieldDay.SetItemText(row, 0, _T("Input"));

	m_pGridYieldDay.MergeCells(row, 1, row, 2);
	m_pGridYieldDay.SetItemBkColour(row, 1, WHITE_C, BLACK_C);
	m_pGridYieldDay.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
	m_pGridYieldDay.SetItemText(row, 1, _T("0"));

	row = 3;
	m_pGridYieldDay.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
	m_pGridYieldDay.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridYieldDay.SetItemText(row, 0, _T("Pass"));

	m_pGridYieldDay.SetItemBkColour(row, 1, GREEN_L, BLACK_C);
	m_pGridYieldDay.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
	m_pGridYieldDay.SetItemText(row, 1, _T("0"));

	m_pGridYieldDay.SetItemBkColour(row, 2, GREEN_L, BLACK_C);
	m_pGridYieldDay.SetItemFont(row, 2, &clsFunc.OnLogFont(16));
	m_pGridYieldDay.SetItemText(row, 2, _T("0"));

	row = 4;
	m_pGridYieldDay.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
	m_pGridYieldDay.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridYieldDay.SetItemText(row, 0, _T("Fail"));

	m_pGridYieldDay.SetItemBkColour(row, 1, RED_L, BLACK_C);
	m_pGridYieldDay.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
	m_pGridYieldDay.SetItemText(row, 1, _T("0"));

	m_pGridYieldDay.SetItemBkColour(row, 2, RED_L, BLACK_C);
	m_pGridYieldDay.SetItemFont(row, 2, &clsFunc.OnLogFont(16));
	m_pGridYieldDay.SetItemText(row, 2, _T("0"));

	row = 5;
	m_pGridYieldDay.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
	m_pGridYieldDay.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridYieldDay.SetItemText(row, 0, _T("Yield"));

	m_pGridYieldDay.SetItemBkColour(row, 1, GREEN_L, BLACK_C);
	m_pGridYieldDay.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
	m_pGridYieldDay.SetItemText(row, 1, _T("0"));

	m_pGridYieldDay.SetItemBkColour(row, 2, GREEN_L, BLACK_C);
	m_pGridYieldDay.SetItemFont(row, 2, &clsFunc.OnLogFont(16));
	m_pGridYieldDay.SetItemText(row, 2, _T("0"));
}

void CScreenMain::OnBnClickedBtnUnldTrayOutputFirst()
{
	if (st_handler_info.nRunStatus == dRUN) return;

	//2017.0116
	CDialog_Message dlgMsg;
	CString strMsg;

	if( st_lot_info[LOT_CURR].strLotNo !=_T("")  || st_lot_info[LOT_NEXT].strLotNo !=_T("")	)
	{
		strMsg.Format(_T("현재 랏이 동작 중입니다. 랏을 먼저 제거해 주세요."));
		AfxMessageBox(strMsg);
		return;
	}

	if( st_handler_info.mn_removetray >= CTL_REQ )
	{
		strMsg.Format(_T("현재 Tray 제거 중입니다."));
		AfxMessageBox(strMsg);
		return;
	}

	st_handler_info.mn_removetray = CTL_NO;
	st_handler_info.mn_uldnum = -1;
	dlgMsg.m_strMessage = _T("Empty Elv의 Tray를 Unload#1에 Tray를 제거하시겠습니까?");
	if (dlgMsg.DoModal() == IDOK)
	{
		st_handler_info.mn_uldnum = 1;
		st_handler_info.mn_removetray = CTL_REQ;
	}
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[0].nYesNo = NO;
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[1].nYesNo = NO;
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[2].nYesNo = NO;
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[3].nYesNo = NO;
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[4].nYesNo = NO;
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[5].nYesNo = NO;
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[6].nYesNo = NO;
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[7].nYesNo = NO;
// 	
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[0].strBarcode1D[0] = _T("11S00VK307YH10MS6AH0DE");
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[0].strBarcode1D[0].MakeUpper();
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[0].strBarcode1D[1] = _T("KR_M351A8K43BB0-CK0_1643");
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[0].strBarcode1D[1].MakeUpper();
// 	
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[1].strBarcode1D[0] = _T("11S00VK307YH10MS6AH0DD");
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[1].strBarcode1D[0].MakeUpper();
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[1].strBarcode1D[1] = _T("KR_M351A8K43BB0-CK0_1643");
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[1].strBarcode1D[1].MakeUpper();
// 	
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[2].strBarcode1D[0] = _T("11S00VK307YH10MS6AH0DC");
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[2].strBarcode1D[0].MakeUpper();
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[2].strBarcode1D[1] = _T("KR_M351A8K43BB0-CK0_1643");
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[2].strBarcode1D[1].MakeUpper();
// 	
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[3].strBarcode1D[0] = _T("11S00VK307YH10MS6AH0DB");
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[3].strBarcode1D[0].MakeUpper();
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[3].strBarcode1D[1] = _T("KR_M351A8K43BB0-CK0_1643");
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[3].strBarcode1D[1].MakeUpper();
// 	
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[4].strBarcode1D[0] = _T("11S00VK307YH10MS615167");
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[4].strBarcode1D[0].MakeUpper();
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[4].strBarcode1D[1] = _T("KR_M351A8K43BB0-CK0_1643");
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[4].strBarcode1D[1].MakeUpper();
// 	
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[5].strBarcode1D[0] = _T("11S00VK307YH10MS61516F");
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[5].strBarcode1D[0].MakeUpper();
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[5].strBarcode1D[1] = _T("KR_M351A8K43BB0-CK0_1643");
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[5].strBarcode1D[1].MakeUpper();
// 	
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[6].strBarcode1D[0] = _T("11S00VK307YH10MS61517A");
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[6].strBarcode1D[0].MakeUpper();
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[6].strBarcode1D[1] = _T("KR_M351A8K43BB0-CK0_1643");
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[6].strBarcode1D[1].MakeUpper();
// 	
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[7].strBarcode1D[0] = _T("11S00VK307YH10MS61517B");
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[7].strBarcode1D[0].MakeUpper();
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[7].strBarcode1D[1] = _T("KR_M351A8K43BB0-CK0_1643");
// 	st_test_site_info[THD_TESTSITE_4].st_pcb_info[7].strBarcode1D[1].MakeUpper();
// 	
// 	st_test_site_info[THD_TESTSITE_4].strOptCode = _T("01");
// 	
// 	st_test_site_info[THD_TESTSITE_4].strEqpID	= _T("ICBT04");
// 	
 	//clsInterS1.OnDataAnalysis(_T("FUNCTION=TEST_RESULT LOT_ID=FY1T110601 EQP_ID=ICBT05 TEST_MODE=ONESHOT PART_NO=M352BAG70DM0-YK0M0-00K SOCKET_INFO=1,1,1,1,1,1,1,1 MDL01=11S00LP766YH10MS61516A,KR_M352BAG70DM0-YK0_1601,P_99_H020_H021_C022_C023 MDL02=11S00LP766YH10MS61516B,KR_M352BAG70DM0-YK0_1601,P_99_H000_H000_C000_C000 MDL03=11S00LP766YH10MS61516C,KR_M352BAG70DM0-YK0_1601,P_99_H000_H000_C000_C000 MDL04=11S00LP766YH10MS61516D,KR_M352BAG70DM0-YK0_1601,P_99_H000_H000_C000_C000 MDL05=11S00LP766YH10MS61516E,KR_M352BAG70DM0-YK0_1601,P_99_H000_H000_C000_C000 MDL06=11S00LP766YH10MS61516F,KR_M352BAG70DM0-YK0_1601,P_99_H000_H000_C000_C000 MDL07=11S00LP766YH10MS61517A,KR_M352BAG70DM0-YK0_1601,P_99_H000_H000_C000_C000 MDL08=11S00LP766YH10MS61517B,KR_M352BAG70DM0-YK0_1601,P_99_H000_H000_C000_C000 MDL_CNT=8 TEST_TIME=00:00:36 RESULT=PASS"));

}

void CScreenMain::OnBnClickedBtnUnldTrayOutputSecond()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	st_handler_info.nRunStatus = dRUN;
	if (st_handler_info.nRunStatus == dRUN) return;
	
	//2017.0116
// 	CDialog_Message dlgMsg;
// 	CString strMsg;
// 
// 	if( st_lot_info[LOT_CURR].strLotNo !=_T("")  || st_lot_info[LOT_NEXT].strLotNo !=_T("")	)
// 	{
// 		strMsg.Format(_T("현재 랏이 동작 중입니다. 랏을 먼저 제거해 주세요."));
// 		AfxMessageBox(strMsg);
// 		return;
// 	}
// 
// 	if( st_handler_info.mn_removetray >= CTL_REQ )
// 	{
// 		strMsg.Format(_T("현재 Tray 제거 중입니다."));
// 		AfxMessageBox(strMsg);
// 		return;
// 	}
// 
// 	st_handler_info.mn_removetray = CTL_NO;
// 	st_handler_info.mn_uldnum = -1;
// 	dlgMsg.m_strMessage = _T("Empty Elv의 Tray를 Unload#2에 Tray를 제거하시겠습니까?");
// 	if (dlgMsg.DoModal() == IDOK)
// 	{
// 		st_handler_info.mn_uldnum = 2;
// 		st_handler_info.mn_removetray = CTL_REQ;
// 	}


	return;
	//
	//st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_BARCODE_BOX_CREATE_REQ,1 );//1(1D),2(2D ld),3(2D uld)
	if (st_handler_info.cWndMain != NULL)
	{
		st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_LOT_END_CREATE_REQ, 0);
	}
}


void CScreenMain::OnBnClickedBtnRejectTrayOutputFirst()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//st_test_site_info[THD_TESTSITE_6].nTestBdStart = 0;
	st_handler_info.nRunStatus = dSTOP;
	return;
	if (st_handler_info.nRunStatus == dRUN) return;
}


void CScreenMain::OnBnClickedBtnRejectTrayOutputSecond()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (st_handler_info.nRunStatus == dRUN) return;

	st_test_site_info[THD_TESTSITE_4].st_pcb_info[7].strBarcode1D[0] = _T("11S00VK307YH10MS61517B");
	st_test_site_info[THD_TESTSITE_4].st_pcb_info[7].strBarcode1D[0].MakeUpper();
	st_test_site_info[THD_TESTSITE_4].st_pcb_info[7].strBarcode1D[1] = _T("KR_M351A8K43BB0-CK0_1643");
	st_test_site_info[THD_TESTSITE_4].st_pcb_info[7].strBarcode1D[1].MakeUpper();
}


void CScreenMain::OnMainLdTray()
{
	int x, y;

	CString strTemp;

	// jtkim 20161002
	for (x=0; x<st_recipe_info.nTrayX; x++)
	{
		for (y=0; y<st_recipe_info.nTrayY; y++)
		{
			if(st_tray_info[THD_LD_TRAY_PLATE].st_pcb_info[y][x].nYesNo == CTL_YES)
			{
				//strTemp = st_tray_info[THD_LD_TRAY_PLATE].st_pcb_info[y][x].strBarcode1D[0].Right(4);// Mid(0, 4);
				//kwlee 2016.1214
				strTemp.Format(_T("%s"),st_tray_info[THD_LD_TRAY_PLATE].st_pcb_info[y][x].strBarcode1D[0].Right(4));
				//m_pGridLdTray.SetItemBkColour((st_recipe_info.nTrayY) - y, x, BIN_LOAD, BLACK_C);
				m_pGridLdTray.SetItemText((st_recipe_info.nTrayY) - y, x, strTemp);
				if (st_lot_info[LOT_CURR].strLotNo == st_tray_info[THD_LD_TRAY_PLATE].st_pcb_info[y][x].strLotNo)
				{
					m_pGridLdTray.SetItemBkColour((st_recipe_info.nTrayY) - y, x, BIN_LOAD, BLACK_C);
				}
				else
				{
					m_pGridLdTray.SetItemBkColour((st_recipe_info.nTrayY) - y, x, SKY_C, BLACK_C);
				}
			}
			else
			{
				m_pGridLdTray.SetItemBkColour((st_recipe_info.nTrayY) - y, x, BLACK_L, WHITE_C);
				m_pGridLdTray.SetItemText((st_recipe_info.nTrayY) - y, x, _T("-"));
			}
		}
	}
		
	m_pGridLdTray.Invalidate(FALSE);
}


void CScreenMain::OnMainUnLdTrayFirst()
{
	int x, y;

	CString strTemp;

	for (x=0; x<st_recipe_info.nTrayX; x++)
	{
		for (y=0; y<st_recipe_info.nTrayY; y++)
		{
			if(st_tray_info[THD_ULD_1_STACKER].st_pcb_info[y][x].nYesNo == CTL_YES)
			{
				//strTemp = st_tray_info[THD_ULD_1_STACKER].st_pcb_info[y][x].strBarcode1D[0].Right(4);// Mid(0, 4);
				//kwlee 2016.1214
				strTemp.Format(_T("%s"),st_tray_info[THD_ULD_1_STACKER].st_pcb_info[y][x].strBarcode1D[0].Right(4));
				m_pGridUnLdTrayFirst.SetItemBkColour((st_recipe_info.nTrayY) - y, x, BIN_GOOD, BLACK_C);
				m_pGridUnLdTrayFirst.SetItemText((st_recipe_info.nTrayY) - y, x, strTemp);
			}
			else
			{
				m_pGridUnLdTrayFirst.SetItemBkColour((st_recipe_info.nTrayY) - y, x, BLACK_L, WHITE_C);
				m_pGridUnLdTrayFirst.SetItemText((st_recipe_info.nTrayY) - y, x, _T("-"));
			}
		}
	}
		
	m_pGridUnLdTrayFirst.Invalidate(FALSE);
}


void CScreenMain::OnMainUnLdTraySecond()
{
	int x, y;

	CString strTemp;

	for (x=0; x<st_recipe_info.nTrayX; x++)
	{
		for (y=0; y<st_recipe_info.nTrayY; y++)
		{
			if(st_tray_info[THD_ULD_2_STACKER].st_pcb_info[y][x].nYesNo == CTL_YES)
			{
				//strTemp = st_tray_info[THD_ULD_2_STACKER].st_pcb_info[y][x].strBarcode1D[0].Right(4);// Mid(0, 4);
				//kwlee 2016.1214
				strTemp.Format(_T("%s"),st_tray_info[THD_ULD_2_STACKER].st_pcb_info[y][x].strBarcode1D[0].Right(4));
				m_pGridUnLdTraySecond.SetItemBkColour((st_recipe_info.nTrayY) - y, x, BIN_GOOD, BLACK_C);
				m_pGridUnLdTraySecond.SetItemText((st_recipe_info.nTrayY) - y, x, strTemp);
			}
			else
			{
				m_pGridUnLdTraySecond.SetItemBkColour((st_recipe_info.nTrayY) - y, x, BLACK_L, WHITE_C);
				m_pGridUnLdTraySecond.SetItemText((st_recipe_info.nTrayY) - y, x, _T("-"));
			}
		}
	}
		
	m_pGridUnLdTraySecond.Invalidate(FALSE);
}


void CScreenMain::OnMainRejectTrayFirst()
{
	int x, y;

	CString strTemp;

	for (x=0; x<st_recipe_info.nTrayX; x++)
	{
		for (y=0; y<st_recipe_info.nTrayY; y++)
		{
			if(st_tray_info[THD_REJECT_OUT_1_STACKER].st_pcb_info[y][x].nYesNo == CTL_YES)
			{
				//strTemp = st_tray_info[THD_REJECT_OUT_1_STACKER].st_pcb_info[y][x].strBarcode1D[0].Right(4);// Mid(0, 4);
				//kwlee 2016.1214
				strTemp.Format(_T("%s"),st_tray_info[THD_REJECT_OUT_1_STACKER].st_pcb_info[y][x].strBarcode1D[0].Right(4));
				m_pGridRejectTrayFirst.SetItemBkColour((st_recipe_info.nTrayY) - y, x, BIN_FAIL, BLACK_C);
				m_pGridRejectTrayFirst.SetItemText((st_recipe_info.nTrayY) - y, x, strTemp);
			}
			else
			{
				m_pGridRejectTrayFirst.SetItemBkColour((st_recipe_info.nTrayY) - y, x, BLACK_L, WHITE_C);
				m_pGridRejectTrayFirst.SetItemText((st_recipe_info.nTrayY) - y, x, _T("-"));
			}
		}
	}

	m_pGridRejectTrayFirst.Invalidate(FALSE);
}


void CScreenMain::OnMainRejectTraySecond()
{
	int x, y;

	CString strTemp;

	for (x=0; x<st_recipe_info.nTrayX; x++)
	{
		for (y=0; y<st_recipe_info.nTrayY; y++)
		{
			if(st_tray_info[THD_REJECT_OUT_2_STACKER].st_pcb_info[y][x].nYesNo == CTL_YES)
			{
				//strTemp = st_tray_info[THD_REJECT_OUT_2_STACKER].st_pcb_info[y][x].strBarcode1D[0].Right(4);// Mid(0, 4);
				//kwlee 2016.1214
				strTemp.Format(_T("%s"),st_tray_info[THD_REJECT_OUT_2_STACKER].st_pcb_info[y][x].strBarcode1D[0].Right(4));
				m_pGridRejectTraySecond.SetItemBkColour((st_recipe_info.nTrayY) - y, x, BIN_FAIL, BLACK_C);
				m_pGridRejectTraySecond.SetItemText((st_recipe_info.nTrayY) - y, x, strTemp);
			}
			else
			{
				m_pGridRejectTraySecond.SetItemBkColour((st_recipe_info.nTrayY) - y, x, BLACK_L, WHITE_C);
				m_pGridRejectTraySecond.SetItemText((st_recipe_info.nTrayY) - y, x, _T("-"));
			}
		}
	}
	
	m_pGridRejectTraySecond.Invalidate(FALSE);
}
void CScreenMain::OnMainWorkRobot()
{
	
}

void CScreenMain::OnMainTrayRobot()
{
	int i;

	CString strTemp;

	// jtkim 20161002
	for (i=0; i<1; i++)
	{
		if(st_picker[THD_WORK_RBT].st_pcb_info[i].nYesNo == CTL_YES)
		{
			if (st_picker[THD_WORK_RBT].st_pcb_info[i].nBin == BD_DATA_GOOD)
			{
				m_pGridTrayRobot.SetItemText(0, i, _T("P"));
				if (st_lot_info[LOT_CURR].strLotNo == st_picker[THD_WORK_RBT].st_pcb_info[i].strLotNo)
				{
					m_pGridTrayRobot.SetItemBkColour(0, i, BIN_GOOD, BLACK_C);
				}
				else
				{
					m_pGridTrayRobot.SetItemBkColour(0, i, BLUE_C, BLACK_C);
				}
			}
			else if (st_picker[THD_WORK_RBT].st_pcb_info[i].nBin == BD_DATA_REJECT)
			{
				m_pGridTrayRobot.SetItemText(0, i, _T("F"));
				if (st_lot_info[LOT_CURR].strLotNo == st_picker[THD_WORK_RBT].st_pcb_info[i].strLotNo)
				{
					m_pGridTrayRobot.SetItemBkColour(0, i, RED_C, BLACK_C);
				}
				else
				{
					m_pGridTrayRobot.SetItemBkColour(0, i, RED_L, BLACK_C);
				}
			}
			else if (st_picker[THD_WORK_RBT].st_pcb_info[i].nBin == BD_DATA_ABORT)
			{
				m_pGridTrayRobot.SetItemText(0, i, _T("A"));
				if (st_lot_info[LOT_CURR].strLotNo == st_picker[THD_WORK_RBT].st_pcb_info[i].strLotNo)
				{
					m_pGridTrayRobot.SetItemBkColour(0, i, BIN_ABORT, BLACK_C);
				}
				else
				{
					m_pGridTrayRobot.SetItemBkColour(0, i, ORANGE_C, BLACK_C);
				}
			}
			else
			{
				m_pGridTrayRobot.SetItemText(0, i, _T("L"));
				m_pGridTrayRobot.SetItemBkColour(0, i, BIN_LOAD, BLACK_C);
			}
		}
		else
		{
			m_pGridTrayRobot.SetItemBkColour(0, i, BLACK_L, WHITE_C);
			m_pGridTrayRobot.SetItemText(0, i, _T("-"));
		}
	}
		
	m_pGridTrayRobot.Invalidate(FALSE);
}


void CScreenMain::OnMainLdBuf()
{
	int x;

	CString strTemp;

	// jtkim 20161002
	for (x=0; x<st_recipe_info.nLdUldBuffer_Num; x++)
	{
		if(st_buffer_info[THD_LD_BUFF].st_pcb_info[x].nYesNo == CTL_YES)
		{
			//strTemp = st_buffer_info[THD_LD_BUFF].st_pcb_info[x].strBarcode1D[0].Right(4);// .Mid(0, 4);
			//kwlee 2016.1214
			strTemp.Format(_T("%s"),st_buffer_info[THD_LD_BUFF].st_pcb_info[x].strBarcode1D[0].Right(4));
			if (st_lot_info[LOT_CURR].strLotNo == st_buffer_info[THD_LD_BUFF].st_pcb_info[x].strLotNo)
			{
				m_pGridLdBuf.SetItemBkColour(1, x, BIN_LOAD, BLACK_C);
			}
			else
			{
				m_pGridLdBuf.SetItemBkColour(1, x, BIN_LOAD, BLACK_C);
			}
			m_pGridLdBuf.SetItemText(1, x, strTemp);
		}
		else
		{
			m_pGridLdBuf.SetItemBkColour(1, x, BLACK_L, WHITE_C);
			m_pGridLdBuf.SetItemText(1, x, _T("-"));
		}
	}
		
	strTemp.Format(_T("SITE[%02d]  POS[%02d]"), st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][4],
											  st_sync_info.nTestRbt_Dvc_Req[THD_LD_BUFF][3]);

	m_pGridLdBuf.SetItemText(0, 0, strTemp);

	m_pGridLdBuf.Invalidate(FALSE);
}


void CScreenMain::OnMainUnLdBuf()
{
	int x;

	CString strTemp;


	for (x=0; x<st_recipe_info.nLdUldBuffer_Num; x++)
	{
		if(st_buffer_info[THD_ULD_BUFF].st_pcb_info[x].nYesNo == CTL_YES)
		{
			//strTemp = st_buffer_info[THD_ULD_BUFF].st_pcb_info[x].strBarcode1D[0].Right(4);// .Mid(0, 4);
			//kwlee 2016.1214
			strTemp.Format(_T("%s"),st_buffer_info[THD_ULD_BUFF].st_pcb_info[x].strBarcode1D[0].Right(4));
			if (st_buffer_info[THD_ULD_BUFF].st_pcb_info[x].nBin == BD_DATA_GOOD)
			{
				//m_pGridUnLdBuf.SetItemBkColour(1, x, BIN_GOOD, BLACK_C);
				m_pGridUnLdBuf.SetItemText(1, x, strTemp);
				if (st_lot_info[LOT_CURR].strLotNo == st_buffer_info[THD_ULD_BUFF].st_pcb_info[x].strLotNo)
				{
					m_pGridUnLdBuf.SetItemBkColour(1, x, BIN_GOOD, BLACK_C);
				}
				else
				{
					m_pGridUnLdBuf.SetItemBkColour(1, x, BLUE_C, BLACK_C);
				}
			}
			else if (st_buffer_info[THD_ULD_BUFF].st_pcb_info[x].nBin == BD_DATA_REJECT)
			{
				//m_pGridUnLdBuf.SetItemBkColour(1, x, BIN_FAIL, BLACK_C);
				m_pGridUnLdBuf.SetItemText(1, x, strTemp);
				if (st_lot_info[LOT_CURR].strLotNo == st_buffer_info[THD_ULD_BUFF].st_pcb_info[x].strLotNo)
				{
					m_pGridUnLdBuf.SetItemBkColour(1, x, RED_C, BLACK_C);
				}
				else
				{
					m_pGridUnLdBuf.SetItemBkColour(1, x, RED_L, BLACK_C);
				}
			}
			else if (st_buffer_info[THD_ULD_BUFF].st_pcb_info[x].nBin == BD_DATA_ABORT)
			{
				//m_pGridUnLdBuf.SetItemBkColour(1, x, BIN_ABORT, BLACK_C);
				m_pGridUnLdBuf.SetItemText(1, x, strTemp);
				if (st_lot_info[LOT_CURR].strLotNo == st_buffer_info[THD_ULD_BUFF].st_pcb_info[x].strLotNo)
				{
					m_pGridUnLdBuf.SetItemBkColour(1, x, BIN_ABORT, BLACK_C);
				}
				else
				{
					m_pGridUnLdBuf.SetItemBkColour(1, x, ORANGE_C, BLACK_C);
				}
			}
		}
		else
		{
			m_pGridUnLdBuf.SetItemBkColour(1, x, BLACK_L, WHITE_C);
			m_pGridUnLdBuf.SetItemText(1, x, _T("-"));
		}
	}
		
	strTemp.Format(_T("SITE[%02d]  POS[%02d]"), st_sync_info.nTestRbt_Dvc_Req[THD_ULD_BUFF][4],
											  st_sync_info.nTestRbt_Dvc_Req[THD_ULD_BUFF][3]);

	m_pGridUnLdBuf.SetItemText(0, 0, strTemp);

	m_pGridUnLdBuf.Invalidate(FALSE);
}


void CScreenMain::OnMainTestSiteRobot()
{
	int i;

	CString strTemp;

	for (i=0; i<4; i++)
	{
		if(st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == CTL_YES)
		{
			if (st_picker[THD_TEST_RBT].st_pcb_info[i].nBin == BD_DATA_GOOD)
			{
				//kwlee 2016.1105
				//m_pGridTestSiteRobot.SetItemText(1, i, _T("P"));
				//strTemp = st_picker[THD_TEST_RBT].st_pcb_info[i].strBarcode1D[0].Right(4);// .Mid(0, 4);
				//kwlee 2016.1214
				strTemp.Format(_T("%s"),st_picker[THD_TEST_RBT].st_pcb_info[i].strBarcode1D[0].Right(4));
				if (st_lot_info[LOT_CURR].strLotNo == st_picker[THD_TEST_RBT].st_pcb_info[i].strLotNo)
				{
					m_pGridTestSiteRobot.SetItemBkColour(1, i, BIN_GOOD, BLACK_C);
				}
				else
				{
					m_pGridTestSiteRobot.SetItemBkColour(1, i, BLUE_C, BLACK_C);
				}
				m_pGridTestSiteRobot.SetItemText(1, i, strTemp); //kwlee 2016.1105
			}
			else if (st_picker[THD_TEST_RBT].st_pcb_info[i].nBin == BD_DATA_REJECT)
			{
				//kwlee 2016.1105
				//m_pGridTestSiteRobot.SetItemText(1, i, _T("F"));
				strTemp = st_picker[THD_TEST_RBT].st_pcb_info[i].strBarcode1D[0].Right(4);// .Mid(0, 4);

				if (st_lot_info[LOT_CURR].strLotNo == st_picker[THD_TEST_RBT].st_pcb_info[i].strLotNo)
				{
					m_pGridTestSiteRobot.SetItemBkColour(1, i, RED_C, BLACK_C);
				}
				else
				{
					m_pGridTestSiteRobot.SetItemBkColour(1, i, RED_L, BLACK_C);
				}
				m_pGridTestSiteRobot.SetItemText(1, i, strTemp); //kwlee 2016.1105
			}
			else if (st_picker[THD_TEST_RBT].st_pcb_info[i].nBin == BD_DATA_ABORT)
			{
				//kwlee 2016.1105
				//m_pGridTestSiteRobot.SetItemText(1, i, _T("A"));
				//strTemp = st_picker[THD_TEST_RBT].st_pcb_info[i].strBarcode1D[0].Right(4);// .Mid(0, 4);
				//kwlee 2016.1214
				strTemp.Format(_T("%s"),st_picker[THD_TEST_RBT].st_pcb_info[i].strBarcode1D[0].Right(4));
				if (st_lot_info[LOT_CURR].strLotNo == st_picker[THD_TEST_RBT].st_pcb_info[i].strLotNo)
				{
					m_pGridTestSiteRobot.SetItemBkColour(1, i, BIN_ABORT, BLACK_C);
				}
				else
				{
					m_pGridTestSiteRobot.SetItemBkColour(1, i, ORANGE_C, BLACK_C);
				}
				m_pGridTestSiteRobot.SetItemText(1, i, strTemp); //kwlee 2016.1105
			}
			else if (st_picker[THD_TEST_RBT].st_pcb_info[i].nBin == BD_DATA_RETEST)
			{
				//m_pGridTestSiteRobot.SetItemText(1, i, _T("R"));
				//strTemp = st_picker[THD_TEST_RBT].st_pcb_info[i].strBarcode1D[0].Right(4);// .Mid(0, 4);
				//kwlee 2016.1214
				strTemp.Format(_T("%s"),st_picker[THD_TEST_RBT].st_pcb_info[i].strBarcode1D[0].Right(4));

				if (st_lot_info[LOT_CURR].strLotNo == st_picker[THD_TEST_RBT].st_pcb_info[i].strLotNo)
				{
					m_pGridTestSiteRobot.SetItemBkColour(1, i, BIN_ABORT, BLACK_C);
				}
				else
				{
					m_pGridTestSiteRobot.SetItemBkColour(1, i, ORANGE_C, BLACK_C);
				}
				m_pGridTestSiteRobot.SetItemText(1, i, strTemp); //kwlee 2016.1105
			}

			else
			{
				
				//strTemp = st_picker[THD_TEST_RBT].st_pcb_info[i].strBarcode1D[0].Right(4);// .Mid(0, 4);
				//kwlee 2016.1214
				strTemp.Format(_T("%s"),st_picker[THD_TEST_RBT].st_pcb_info[i].strBarcode1D[0].Right(4));
				m_pGridTestSiteRobot.SetItemBkColour(1, i, BIN_LOAD, BLACK_C);
				//m_pGridTestSiteRobot.SetItemText(1, i, _T("L"));
				m_pGridTestSiteRobot.SetItemText(1, i, strTemp); //kwlee 2016.1105
			}
		}
		else
		{
			m_pGridTestSiteRobot.SetItemBkColour(1, i, BLACK_L, WHITE_C);
			m_pGridTestSiteRobot.SetItemText(1, i, _T("-"));
		}
	}
	strTemp.Format(_T("SITE[%02d]  POS[%02d]"), st_sync_info.nTestRbt_Dvc_Req[THD_ULD_BUFF][4],
		st_sync_info.nTestRbt_Dvc_Req[THD_ULD_BUFF][3]);

	m_pGridTestSiteRobot.SetItemText(0, 0, strTemp);	

	m_pGridTestSiteRobot.Invalidate(FALSE);
}


void CScreenMain::OnMainRetestBufFirst()
{
	int y;

	CString strTemp;

	// jtkim 20161002
	for (y=0; y<st_recipe_info.nRetestBuffer_Num; y++)
	{
		strTemp.Format(_T("%d"), y + 1);
		m_pGridRetestBufFirst[0].SetItemBkColour(y + 1, 0, BLUE_D, WHITE_C);
		m_pGridRetestBufFirst[0].SetItemText(y + 1, 0, strTemp);
		if(st_buffer_info[THD_RETEST_1_BUFF].st_pcb_info[y].nYesNo == CTL_YES)
		{
			//strTemp = st_buffer_info[THD_RETEST_1_BUFF].st_pcb_info[y].strBarcode1D[0].Right(4);// .Mid(0, 4);
			//kwlee 2016.1214
			strTemp.Format(_T("%s"),st_buffer_info[THD_RETEST_1_BUFF].st_pcb_info[y].strBarcode1D[0].Right(4));
			m_pGridRetestBufFirst[0].SetItemText(y + 1, 1, strTemp);
			if (st_lot_info[LOT_CURR].strLotNo == st_buffer_info[THD_RETEST_1_BUFF].st_pcb_info[y].strLotNo)
			{
				m_pGridRetestBufFirst[0].SetItemBkColour(y + 1, 1, RED_C, BLACK_C);
			}
			else
			{
				m_pGridRetestBufFirst[0].SetItemBkColour(y + 1, 1, RED_L, BLACK_C);
			}
		}
		else
		{
			m_pGridRetestBufFirst[0].SetItemBkColour(y + 1 , 1, BLACK_L, WHITE_C);
			m_pGridRetestBufFirst[0].SetItemText(y + 1, 1, _T("-"));
		}
	}

	m_pGridRetestBufFirst[0].Invalidate(FALSE);
}


void CScreenMain::OnMainRetestBufSecond()
{
	int y;

	CString strTemp;

	// jtkim 20161002
	for (y=0; y<st_recipe_info.nRetestBuffer_Num; y++)
	{
		strTemp.Format(_T("%d"), y + 1);
		m_pGridRetestBufSecond[0].SetItemBkColour(y + 1 , 0, BLUE_D, WHITE_C);
		m_pGridRetestBufSecond[0].SetItemText(y + 1, 0, strTemp);

		if(st_buffer_info[THD_RETEST_2_BUFF].st_pcb_info[y].nYesNo == CTL_YES)
		{
			//strTemp = st_buffer_info[THD_RETEST_2_BUFF].st_pcb_info[y].strBarcode1D[0].Right(4);// .Mid(0, 4);
			//kwlee 2016.1214
			strTemp.Format(_T("%s"),st_buffer_info[THD_RETEST_2_BUFF].st_pcb_info[y].strBarcode1D[0].Right(4));
			//m_pGridRetestBufSecond[0].SetItemBkColour(y + 1 , 1, BIN_RELOAD, BLACK_C);
			m_pGridRetestBufSecond[0].SetItemText(y + 1 , 1, strTemp);
			if (st_lot_info[LOT_CURR].strLotNo == st_buffer_info[THD_RETEST_2_BUFF].st_pcb_info[y].strLotNo)
			{
				m_pGridRetestBufSecond[0].SetItemBkColour(y + 1, 1, RED_C, BLACK_C);
			}
			else
			{
				m_pGridRetestBufSecond[0].SetItemBkColour(y + 1, 1, RED_L, BLACK_C);
			}
		}
		else
		{
			m_pGridRetestBufSecond[0].SetItemBkColour(y + 1, 1, BLACK_L, WHITE_C);
			m_pGridRetestBufSecond[0].SetItemText(y + 1 , 1, _T("-"));
		}
	}

	m_pGridRetestBufSecond[0].Invalidate(FALSE);
}


void CScreenMain::OnMainTestSiteTop()
{
	CString			 strTemp;

	int i, j;

	for (i=0; i<4; i++)
	{
		if (st_test_site_info[THD_TESTSITE_1 + i].nTestBdStart == BD_BIN_CHK)
		{
			strTemp.Format(_T("%d"), st_test_site_info[THD_TESTSITE_1 + i].dwBdTime);
			m_pGridTestSiteTop[i].SetItemBkColour(0,1, BIN_START, BLACK_C);
			m_pGridTestSiteTop[i].SetItemText(0, 1, strTemp);

		}
		else
		{
			strTemp.Format(_T("Test Time"));
			m_pGridTestSiteTop[i].SetItemText(0, 1, strTemp);
			m_pGridTestSiteTop[i].SetItemBkColour(0,1, BLUE_D, WHITE_C);
		}
		////////////////////////
		
		m_pGridTestSiteTop[i].SetItemText(1, 0, st_test_site_info[THD_TESTSITE_1 + i].strLotNo);
		//kwlee 2016.1102
		if (st_lot_info[LOT_CURR].strLotNo == st_test_site_info[THD_TESTSITE_1 + i].strLotNo)
		{
			m_pGridTestSiteTop[i].SetItemBkColour(1, 0, YELLOW_L, BLACK_C);
		}
		else
		{
			m_pGridTestSiteTop[i].SetItemBkColour(1, 0, ORANGE_C, BLACK_C);
		}
		
		/////

		for (j=0; j<8; j++)
		{
			if (st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].nEnable == YES)
			{
				if (st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].nYesNo == YES)
				{
					if (st_test_site_info[THD_TESTSITE_1 + i].nTestBdStart == BD_READY_CHK)
					{
						//m_pGridTestSiteBottom[i].SetItemText((j + 2), 1, _T("Ready"));
						m_pGridTestSiteTop[i].SetItemText(9 - (j), 1, st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].strBarcode1D[0].Right(17));
						if (st_lot_info[LOT_CURR].strLotNo == st_test_site_info[THD_TESTSITE_1+ i].st_pcb_info[j].strLotNo)
						{
							m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, BIN_LOAD, BLACK_C);
						}
						else
						{
							//kwlee 2016.1102
							//m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, BLUE_L, BLACK_C);
							m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, BIN_LOAD, BLACK_C);
						}
					}
					else if (st_test_site_info[THD_TESTSITE_1 + i].nTestBdStart == BD_MODULE_CHK)
					{
						//m_pGridTestSiteBottom[i].SetItemText((j + 2), 1, _T("Module"));
						m_pGridTestSiteTop[i].SetItemText(9 - (j), 1, st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].strBarcode1D[0].Right(17));
						if (st_lot_info[LOT_CURR].strLotNo == st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].strLotNo)
						{
							m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, GREEN_C, BLACK_C);
						}
						else
						{
							//kwlee 2016.1102
							m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, GREEN_C, BLACK_C);
							//m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, GREEN_L, BLACK_C);
						}
					}
					else if (st_test_site_info[THD_TESTSITE_1 + i].nTestBdStart == BD_START_CHK)
					{
						//m_pGridTestSiteBottom[i].SetItemText((j + 2), 1, _T("Start"));
						m_pGridTestSiteTop[i].SetItemText(9 - (j), 1, st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].strBarcode1D[0].Right(17));
						if (st_lot_info[LOT_CURR].strLotNo == st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].strLotNo)
						{
							m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, YELLOW_C, BLACK_C);
						}
						else
						{
							//kwlee 2016.1102
							m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, YELLOW_C, BLACK_C);
							//m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, YELLOW_L, BLACK_C);
						}
					}
					else if (st_test_site_info[THD_TESTSITE_1 + i].nTestBdStart == BD_BIN_CHK)
					{
						m_pGridTestSiteTop[i].SetItemText(9 - (j), 1, st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].strBarcode1D[0].Right(17));
						if (st_lot_info[LOT_CURR].strLotNo == st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].strLotNo)
						{
							m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, YELLOW_C, BLACK_C);
						}
						else
						{
							//kwlee 2016.1102
							m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, YELLOW_C, BLACK_C);
							//m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, YELLOW_L, BLACK_C);
						}
					}
					///////
					else if (st_test_site_info[THD_TESTSITE_1 + i].nTestBdStart == BD_END)
					{
						if (st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].nBin == BD_DATA_GOOD && st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].nTestBdStart == BD_END)
						{
							m_pGridTestSiteTop[i].SetItemText(9 - (j), 1, _T("Pass"));
							if (st_lot_info[LOT_CURR].strLotNo == st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].strLotNo)
							{
								m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, BIN_GOOD, BLACK_C);
							}
							else
							{
								//kwlee 2016.1102
								m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, BIN_GOOD, BLACK_C);
								//m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, BLUE_C, BLACK_C);
							}
						}
						else if (st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].nBin == BD_DATA_REJECT && st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].nTestBdStart == BD_END)
						{
							m_pGridTestSiteTop[i].SetItemText(9 - (j), 1, _T("Fail"));
							if (st_lot_info[LOT_CURR].strLotNo == st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].strLotNo)
							{
								m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, RED_C, BLACK_C);
							}
							else
							{
								m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, RED_C, BLACK_C);
								//m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, RED_L, BLACK_C);
							}
						}
						else if (st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].nBin == BD_DATA_ABORT && st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].nTestBdStart == BD_END)
						{
							
							m_pGridTestSiteTop[i].SetItemText(9 - (j), 1, _T("Abort"));
							if (st_lot_info[LOT_CURR].strLotNo == st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].strLotNo)
							{
								m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, BIN_ABORT, BLACK_C);
							}
							else
							{
								m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, BIN_ABORT, BLACK_C);
								//m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, ORANGE_C, BLACK_C);
							}
						}
					
						else if (st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].nBin == BD_DATA_RETEST && st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].nTestBdStart == BD_END)
						{
							m_pGridTestSiteTop[i].SetItemText(9 - (j), 1, _T("Retest"));
							if (st_lot_info[LOT_CURR].strLotNo == st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].strLotNo)
							{
								m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, RED_C, BLACK_C);
							}
							else
							{
								m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, RED_C, BLACK_C);
								//m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, RED_L, BLACK_C);
							}
						}
						else 
						{
							if (st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].nTestBdStart != BD_END)
							{
								m_pGridTestSiteTop[i].SetItemText(9 - (j), 1, st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].strBarcode1D[0].Right(17));
								m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, BIN_LOAD, BLACK_C);
							}
							else
							{
								m_pGridTestSiteTop[i].SetItemText(9 - (j), 1, st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].strBarcode1D[0].Right(17));
								m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, BIN_LOAD, BLACK_C);
							}
						}
					}
					else
					{
						m_pGridTestSiteTop[i].SetItemText(9 - (j), 1, _T("-"));
						m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, BIN_LOAD, BLACK_C);
					}
				}
				//kwlee 2016.0912 Skip
// 				else if (st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].nSkipMode == YES)
// 				{
// 					m_pGridTestSiteTop[i].SetItemText(9 - (j), 1, _T("-"));
// 					m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, BLACK_L, WHITE_C);
// 				}
				//
				else
				{
					m_pGridTestSiteTop[i].SetItemText(9 - (j), 1, _T("-"));
					m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, BLACK_L, WHITE_C);
				}
			}
			else
			{
				//m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, BIN_OFF, BLACK_C);
				//m_pGridTestSiteTop[i].SetItemText(9 - (j), 1, _T("Off"));
				//kwlee 2016.1201
				if (st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].nBin == BD_DATA_CONTINUE_FAIL && 
					st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].nTestBdStart == BD_END)
				{
					m_pGridTestSiteTop[i].SetItemText(9 - (j), 1, _T("Conti Fail"));
					if (st_lot_info[LOT_CURR].strLotNo == st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].strLotNo)
					{
						m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, CONTINUE_FAIL, BLACK_C);
					}
					else
					{
						m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, CONTINUE_FAIL, BLACK_C);
						//m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, ORANGE_C, BLACK_C);
					}
				}
				//kwlee 2016.1201
				else if (st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].nBin == BD_DATA_OPT_FAIL)
				{
					m_pGridTestSiteTop[i].SetItemText(9 - (j), 1, _T("Opt Fail"));
					if (st_lot_info[LOT_CURR].strLotNo == st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].strLotNo)
					{
						m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, OPT_FAIL, BLACK_C);
					}
					else
					{
						m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, OPT_FAIL, BLACK_C);
						//m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, ORANGE_C, BLACK_C);
					}
				}		
				else
				{
					m_pGridTestSiteTop[i].SetItemBkColour(9 - (j), 1, BIN_OFF, BLACK_C);

					m_pGridTestSiteTop[i].SetItemText(9 - (j), 1, _T("Off"));
				}
			}
		}
		m_pGridTestSiteTop[i].Invalidate(FALSE);
	}
}


void CScreenMain::OnMainTestSiteBottom()
{
	CString			 strTemp;

	COleDateTime	 tCurr;
	COleDateTimeSpan tDiff;

	int i, j;

	// jtki m20161002
	for (i=0; i<4; i++)
	{
		if (st_test_site_info[THD_TESTSITE_1 + 4 + i].nTestBdStart == BD_BIN_CHK)
		{
			strTemp.Format(_T("%d"), st_test_site_info[THD_TESTSITE_1 + 4 + i].dwBdTime);
			m_pGridTestSiteBottom[i].SetItemBkColour(0,1, BIN_START, BLACK_C);
			m_pGridTestSiteBottom[i].SetItemText(0, 1, strTemp);

		}
		else
		{
			strTemp.Format(_T("Test Time"));
			m_pGridTestSiteBottom[i].SetItemText(0, 1, strTemp);
			m_pGridTestSiteBottom[i].SetItemBkColour(0,1, BLUE_D, WHITE_C);
		}

		m_pGridTestSiteBottom[i].SetItemText(1, 0, st_test_site_info[THD_TESTSITE_1 + 4 + i].strLotNo);
		//kwlee 2016.1102
		if (st_lot_info[LOT_CURR].strLotNo == st_test_site_info[THD_TESTSITE_1 + 4 + i].strLotNo)
		{
			m_pGridTestSiteBottom[i].SetItemBkColour(1, 0, YELLOW_L, BLACK_C);
		}
		else
		{
			m_pGridTestSiteBottom[i].SetItemBkColour(1, 0, ORANGE_C, BLACK_C);
		}
		/////
		for (j=0; j<8; j++)
		{
			if (st_test_site_info[THD_TESTSITE_1 + 4 + i].st_pcb_info[j].nEnable == YES)
			{
				if (st_test_site_info[THD_TESTSITE_1 + 4 + i].st_pcb_info[j].nYesNo == YES)
				{
					if (st_test_site_info[THD_TESTSITE_1 + 4 + i].nTestBdStart == BD_READY_CHK)
					{
						//m_pGridTestSiteBottom[i].SetItemText((j + 2), 1, _T("Ready"));
						m_pGridTestSiteBottom[i].SetItemText((j + 2), 1, st_test_site_info[THD_TESTSITE_1 + 4 + i].st_pcb_info[j].strBarcode1D[0].Right(17));
						if (st_lot_info[LOT_CURR].strLotNo == st_test_site_info[THD_TESTSITE_1 + 4 + i].st_pcb_info[j].strLotNo)
						{
							m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, BIN_LOAD, BLACK_C);
						}
						else
						{
							//kwlee 2016.1102
							//m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, BLUE_L, BLACK_C);
							m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, BIN_LOAD, BLACK_C);
						}
					}
					else if (st_test_site_info[THD_TESTSITE_1 + 4 + i].nTestBdStart == BD_MODULE_CHK)
					{
						//m_pGridTestSiteBottom[i].SetItemText((j + 2), 1, _T("Module"));
						m_pGridTestSiteBottom[i].SetItemText((j + 2), 1, st_test_site_info[THD_TESTSITE_1 + 4 + i].st_pcb_info[j].strBarcode1D[0].Right(17));
						if (st_lot_info[LOT_CURR].strLotNo == st_test_site_info[THD_TESTSITE_1 + 4 + i].st_pcb_info[j].strLotNo)
						{
							m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, GREEN_C, BLACK_C);
						}
						else
						{
							//kwlee 2016.1102
							m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, GREEN_C, BLACK_C);
							//m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, GREEN_L, BLACK_C);
						}
					}
					else if (st_test_site_info[THD_TESTSITE_1 + 4 + i].nTestBdStart == BD_START_CHK)
					{
						//m_pGridTestSiteBottom[i].SetItemText((j + 2), 1, _T("Start"));
						m_pGridTestSiteBottom[i].SetItemText((j + 2), 1, st_test_site_info[THD_TESTSITE_1 + 4 + i].st_pcb_info[j].strBarcode1D[0].Right(17));
						if (st_lot_info[LOT_CURR].strLotNo == st_test_site_info[THD_TESTSITE_1 + 4 + i].st_pcb_info[j].strLotNo)
						{
							m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, YELLOW_C, BLACK_C);
						}
						else
						{
							//kwlee 2016.1102
							m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, YELLOW_C, BLACK_C);
							//m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, YELLOW_L, BLACK_C);
						}
					}
					else if (st_test_site_info[THD_TESTSITE_1 + 4 + i].nTestBdStart == BD_BIN_CHK)
					{
						m_pGridTestSiteBottom[i].SetItemText((j + 2), 1, st_test_site_info[THD_TESTSITE_1 + 4 + i].st_pcb_info[j].strBarcode1D[0].Right(17));
						if (st_lot_info[LOT_CURR].strLotNo == st_test_site_info[THD_TESTSITE_1 + 4 + i].st_pcb_info[j].strLotNo)
						{
							m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, YELLOW_C, BLACK_C);
						}
						else
						{
							//kwlee 2016.1102
							m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, YELLOW_C, BLACK_C);
							//m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, YELLOW_L, BLACK_C);
						}
					}
					
					else if (st_test_site_info[THD_TESTSITE_1 + 4 + i].nTestBdStart == BD_END)
					{
						if (st_test_site_info[THD_TESTSITE_1 + 4 + i].st_pcb_info[j].nBin == BD_DATA_GOOD && st_test_site_info[THD_TESTSITE_1 + 4 + i].st_pcb_info[j].nTestBdStart == BD_END)
						{
							m_pGridTestSiteBottom[i].SetItemText((j + 2), 1, _T("Pass"));
							if (st_lot_info[LOT_CURR].strLotNo == st_test_site_info[THD_TESTSITE_1 + 4 + i].st_pcb_info[j].strLotNo)
							{
								m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, BIN_GOOD, BLACK_C);
							}
							else
							{
								m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, BIN_GOOD, BLACK_C);
								//m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, BLUE_C, BLACK_C);
							}
						}
						else if (st_test_site_info[THD_TESTSITE_1 + 4 + i].st_pcb_info[j].nBin == BD_DATA_REJECT && st_test_site_info[THD_TESTSITE_1 + 4 + i].st_pcb_info[j].nTestBdStart == BD_END)
						{
							m_pGridTestSiteBottom[i].SetItemText((j + 2), 1, _T("Fail"));
							if (st_lot_info[LOT_CURR].strLotNo == st_test_site_info[THD_TESTSITE_1 + 4 + i].st_pcb_info[j].strLotNo)
							{
								m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, RED_C, BLACK_C);
							}
							else
							{
								//kwlee 2016.1102
								m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, RED_C, BLACK_C);
								//m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, RED_L, BLACK_C);
							}
						}
						else if (st_test_site_info[THD_TESTSITE_1 + 4 + i].st_pcb_info[j].nBin == BD_DATA_ABORT && st_test_site_info[THD_TESTSITE_1 + 4 + i].st_pcb_info[j].nTestBdStart == BD_END)
						{
							
							m_pGridTestSiteBottom[i].SetItemText((j + 2), 1, _T("Abort"));
							if (st_lot_info[LOT_CURR].strLotNo == st_test_site_info[THD_TESTSITE_1 + 4 + i].st_pcb_info[j].strLotNo)
							{
								m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, BIN_ABORT, BLACK_C);
							}
							else
							{
								//kwlee 2016.1102
								m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, BIN_ABORT, BLACK_C);
								//m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, ORANGE_C, BLACK_C);
							}
						}
						else if (st_test_site_info[THD_TESTSITE_1 + 4 + i].st_pcb_info[j].nBin == BD_DATA_RETEST && st_test_site_info[THD_TESTSITE_1 + 4 + i].st_pcb_info[j].nTestBdStart == BD_END)
						{
							m_pGridTestSiteBottom[i].SetItemText((j + 2), 1, _T("Retest"));
							if (st_lot_info[LOT_CURR].strLotNo == st_test_site_info[THD_TESTSITE_1 + 4 + i].st_pcb_info[j].strLotNo)
							{
								m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, RED_C, BLACK_C);
							}
							else
							{
								m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, RED_C, BLACK_C);
								//m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, RED_L, BLACK_C);
							}
						}
						
						else 
						{
							if (st_test_site_info[THD_TESTSITE_1 + 4 + i].st_pcb_info[j].nTestBdStart != BD_END)
							{
								m_pGridTestSiteBottom[i].SetItemText((j + 2), 1, st_test_site_info[THD_TESTSITE_1 + 4 + i].st_pcb_info[j].strBarcode1D[0].Right(17));
								m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, BIN_LOAD, BLACK_C);
							}
							else
							{
								m_pGridTestSiteBottom[i].SetItemText((j + 2), 1, st_test_site_info[THD_TESTSITE_1 + 4 + i].st_pcb_info[j].strBarcode1D[0].Right(17));
								m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, BIN_LOAD, BLACK_C);
							}
						}
					}
					else
					{
						m_pGridTestSiteBottom[i].SetItemText((j + 2), 1, _T("-"));
						m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, BIN_LOAD, BLACK_C);
					}
				}
				//kwlee 2016.0912 Skip
// 				else if (st_test_site_info[THD_TESTSITE_1 + 4 + i].st_pcb_info[j].nSkipMode == YES)
// 				{
// 					m_pGridTestSiteBottom[i].SetItemText((j + 2), 1, _T("-"));
// 					m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, BLACK_L, WHITE_C);
// 				}
				//
				else
				{
					m_pGridTestSiteBottom[i].SetItemText((j + 2), 1, _T("-"));
					m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, BLACK_L, WHITE_C);
				}
			}
			else
			{

				//kwlee 2016.1201
				if (st_test_site_info[THD_TESTSITE_1 + 4 + i].st_pcb_info[j].nBin == BD_DATA_CONTINUE_FAIL && st_test_site_info[THD_TESTSITE_1 + 4 + i].st_pcb_info[j].nTestBdStart == BD_END)
				{
					m_pGridTestSiteBottom[i].SetItemText((j + 2), 1, _T("Conti Fail"));
					if (st_lot_info[LOT_CURR].strLotNo == st_test_site_info[THD_TESTSITE_1 + 4 + i].st_pcb_info[j].strLotNo)
					{
						m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, CONTINUE_FAIL, BLACK_C);
					}
					else
					{
						m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, CONTINUE_FAIL, BLACK_C);
						//m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, RED_L, BLACK_C);
					}
				}
				else if (st_test_site_info[THD_TESTSITE_1 + 4 + i].st_pcb_info[j].nBin == BD_DATA_OPT_FAIL)
				{
					m_pGridTestSiteBottom[i].SetItemText((j + 2), 1, _T("Opt Fail"));
					if (st_lot_info[LOT_CURR].strLotNo == st_test_site_info[THD_TESTSITE_1 + 4 + i].st_pcb_info[j].strLotNo)
					{
						m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, OPT_FAIL, BLACK_C);
					}
					else
					{
						m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, OPT_FAIL, BLACK_C);
						//m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, RED_L, BLACK_C);
					}
				}
				else
				{
					m_pGridTestSiteBottom[i].SetItemBkColour((j + 2), 1, BIN_OFF, BLACK_C);
					m_pGridTestSiteBottom[i].SetItemText((j + 2), 1, _T("Off"));
				}
			}
		}
		m_pGridTestSiteBottom[i].Invalidate(FALSE);
	}
}


void CScreenMain::OnMainYieldDay()
{
	CString strTemp;

//	double dAve;

	OnMainCountDisplay();
	// jtkim 20150307
/*	strTemp.Format(_T("%d"), st_count_info.nPrimeRejectCount[1][0]);
	m_pGridYieldDay.SetItemText(4, 1, strTemp);

	strTemp.Format(_T("%d"), st_count_info.nRejectCount[1][0]);
	m_pGridYieldDay.SetItemText(4, 2, strTemp);

	if (st_count_info.nPrimeCount[1][0] > 0) 
	{
		//dAve = ((double)st_count_info.nPrimeCount[1][0] / (double)st_count_info.nInCount[1][0]) * (double)100;
		dAve = ((double)st_count_info.nPrimeCount[1][0] / ((double)st_count_info.nPrimeCount[1][0] + (double)st_count_info.nPrimeRejectCount[1][0])) * (double)100;
		strTemp.Format(_T("%.2f"), dAve);
		m_pGridYieldDay.SetItemText(5, 1, strTemp);
	}

	if (st_count_info.nPassCount[1][0] > 0) 
	{
		//dAve = ((double)st_count_info.nPassCount[1][0] / (double)st_count_info.nInCount[1][0]) * (double)100;
		dAve = ((double)st_count_info.nPassCount[1][0] / ((double)st_count_info.nPassCount[1][0] + (double)st_count_info.nRejectCount[1][0])) * (double)100;
		strTemp.Format(_T("%.2f"), dAve);
		m_pGridYieldDay.SetItemText(5, 2, strTemp);
	}

	m_pGridYieldDay.Invalidate(FALSE);*/
}


void CScreenMain::OnMainYieldLot()
{
	CString strTemp;

//	double dAve;

	OnMainCountDisplay();
/*
	// lot 수량.
	strTemp.Format(_T("%d"), st_count_info.nInCount[0][0]);
	m_pGridYieldLot.SetItemText(2, 1, strTemp);

	strTemp.Format(_T("%d"), st_count_info.nPrimeCount[0][0]);
	m_pGridYieldLot.SetItemText(3, 1, strTemp);

	strTemp.Format(_T("%d"), st_count_info.nPassCount[0][0]);
	m_pGridYieldLot.SetItemText(3, 2, strTemp);

	// jtkim 20150307
	strTemp.Format(_T("%d"), st_count_info.nPrimeRejectCount[0][0]);
	m_pGridYieldLot.SetItemText(4, 1, strTemp);

	strTemp.Format(_T("%d"), st_count_info.nRejectCount[0][0]);
	m_pGridYieldLot.SetItemText(4, 2, strTemp);

	if (st_count_info.nPrimeCount[0][0] > 0) 
	{
		//dAve = ((double)st_count_info.nPrimeCount[0][0] / (double)st_count_info.nInCount[0][0]) * (double)100;
		dAve = ((double)st_count_info.nPrimeCount[0][0] / ((double)st_count_info.nPrimeCount[0][0] + (double)st_count_info.nPrimeRejectCount[0][0])) * (double)100;
		strTemp.Format(_T("%.2f"), dAve);
		m_pGridYieldLot.SetItemText(5, 1, strTemp);
	}

	if (st_count_info.nPassCount[0][0] > 0) 
	{
		//dAve = ((double)st_count_info.nPassCount[0][0] / (double)st_count_info.nInCount[0][0]) * (double)100;
		dAve = ((double)st_count_info.nPassCount[0][0] / ((double)st_count_info.nPassCount[0][0] + (double)st_count_info.nRejectCount[0][0])) * (double)100;
		strTemp.Format(_T("%.2f"), dAve);
		m_pGridYieldLot.SetItemText(5, 2, strTemp);
	}

	m_pGridYieldLot.Invalidate(FALSE);*/
}


void CScreenMain::OnBnClickedBtnLotDisplay()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
/*	for (int i=0; i<8; i++)
	{
		st_test_site_info[THD_TESTSITE_1 + i].strLotNo	= st_lot_info[LOT_CURR].strLotNo;
		st_test_site_info[THD_TESTSITE_1 + i].strPartNo	= st_lot_info[LOT_CURR].strPartNo;
	}
	return;*/
	if (st_handler_info.nRunStatus == dRUN) return;

	st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_LOT_END_CREATE_REQ, 0);
}


void CScreenMain::OnBnClickedBtnLdCvyLotInfo1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
// 	for (int i =0; i<8; i++)
// 	{
// 		st_test_site_info[THD_TESTSITE_1  + i].nTestBdStart = BD_BIN_CHK;
// 	}
// 	return;
	st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_LOT_START_CREATE_REQ, 0);
}


void CScreenMain::OnBnClickedBtnLdCvyLotInfo2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
// 	for (int i =0; i<8; i++)
// 	{
// 		st_test_site_info[THD_TESTSITE_1  + i].nTestBdStart = BD_DATA_GOOD;
// 	}
// 	return;
	st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_LOT_START_CREATE_REQ, 1);
}


void CScreenMain::OnBnClickedBtnLotEnd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_Message dlgMsg;
	CString strMsg;
	int nLotendCnt = 0, i = 0;

	if (st_handler_info.nRunStatus == dRUN) return;

	dlgMsg.m_nMessageType = 1;
	dlgMsg.m_strMessage.Format(_T("Do you want a Lot End?"));

	if (dlgMsg.DoModal() == IDOK)
	{
		st_lot_info[LOT_CURR].nLotStatus = LOT_END_START;

		for( i = 0; i < 4; i++)
		{
			if( st_buffer_info[THD_LD_BUFF].st_pcb_info[i].strLotNo == st_lot_info[LOT_CURR].strLotNo &&
				st_buffer_info[THD_LD_BUFF].st_pcb_info[i].nYesNo == YES )
			{
				nLotendCnt++;
				break;
			}

			if( st_picker[THD_TEST_RBT].st_pcb_info[i].strLotNo == st_lot_info[LOT_CURR].strLotNo &&
				st_picker[THD_TEST_RBT].st_pcb_info[i].nYesNo == YES )
			{
				nLotendCnt++;
				break;
			}

		}

		for(i = 0 ; i < st_recipe_info.nRetestBuffer_Num; i++) 
		{
			if(st_buffer_info[st_lot_info[LOT_CURR].nBuffer_Retest_THD_Num].st_pcb_info[i].nYesNo == CTL_YES)
			{
				nLotendCnt++;
				break;
			}
		}

		if( nLotendCnt == 0 )
		{
			for( int nSiteNum = THD_TESTSITE_1; nSiteNum <= THD_TESTSITE_8; nSiteNum++)
			{
				for(i = 0; i < TEST_SOCKET_PARA; i++)
				{
					if(st_lot_info[LOT_CURR].strLotNo == st_test_site_info[nSiteNum].strLotNo
						&& st_test_site_info[nSiteNum].st_pcb_info[i].nYesNo == CTL_YES)
					{
						nLotendCnt++;
					}
				}
			}
		}

// 		if( nLotendCnt == 0 )
// 		{
// 			st_lot_info[LOT_CURR].nLot_THD_Status[THD_TEST_RBT] = LOT_END;
// 		}

		if( nLotendCnt == 0 )
		{
			strMsg.Format(_T("[%s] Lot End, Testsite lotend"), st_lot_info[LOT_CURR].strLotNo);
		}
		else
		{
			strMsg.Format(_T("[%s] Lot End start"), st_lot_info[LOT_CURR].strLotNo);

		}


		clsMem.OnNormalMessageWrite(strMsg);
	}
}

void CScreenMain::OnBnClickedBtnDoorOpen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	clsFunc.OnLdUldDoorOpen();
	clsFunc.OnTestDoorOpen();
}


void CScreenMain::OnBnClickedBtnDoorClose()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	clsFunc.OnLdUldDoorClose();
	clsFunc.OnTestDoorClose();
}
