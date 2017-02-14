//nTestMode
//nTestMode
//nTestMode
// DialogLotDisplay.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "DialogLotDisplay.h"
#include "afxdialogex.h"
#include "Variable.h"
#include "PublicFunction.h"
#include "Dialog_KeyBoard.h"
#include "Dialog_KeyPad.h"
//#include "DataCenterClient.h"
#include "ClientEcFirst.h"
#include "Alg_Mysql.h"
//#include "DialogBadInform.h"
#include "FastechPublic_IO.h"
#include "Dialog_Message.h"
#include "ClientRfid.h"
#include "AlgMemory.h"
#include "KeyBoard.h"
#include "Interface.h"
#include "ClientEcSecond.h"
#include "ClientInterface1.h"
#include "ClientInterface2.h"
#include "ClientInterface3.h"
#include "ClientInterface4.h"
#include "ClientInterface5.h"
#include "ClientInterface6.h"
#include "ClientInterface7.h"
#include "ClientInterface8.h"

// CDialogLotDisplay 대화 상자입니다.
#define TM_TRACK			100
#define TM_ONLINE			200
#define TM_SCRAP			300
#define TM_PCB				400
#define TM_DEL				500
#define TM_STATUS			600
#define TM_EXIT				700
#define TM_TEST				800
#define TM_CANCEL			900

IMPLEMENT_DYNAMIC(CDialogLotDisplay, CDialog)

CDialogLotDisplay::CDialogLotDisplay(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogLotDisplay::IDD, pParent)
{
	m_bmGround.LoadBitmap(IDB_BITMAP_BACK_GROUND);   

	m_strPcbBarcode = _T("");
	m_nAddScrap		= 0;
	m_nLotQty		= 0;
	m_nScrapCount	= 0;

	m_nLotEndStep	= -1;
}

CDialogLotDisplay::~CDialogLotDisplay()
{
}

void CDialogLotDisplay::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CUSTOM_PCB_INFO,						m_pGridPcbInfo);
	DDX_Control(pDX, IDC_CUSTOM_LOT_INFO,						m_pGridLotInfo);
	DDX_Control(pDX, IDC_GROUP_LOT_DISPLAY,						m_groupLotDisplay);
	DDX_Control(pDX, IDC_BTN_EXIT,								m_btnExit);
	//	DDX_Control(pDX, IDC_BTN_PCB_INFORM,						m_btnBadInform);
	DDX_Control(pDX, IDC_BTN_DIALOG_LOT_DISPLAY_END,			m_btnLotEnd);
	DDX_Control(pDX, IDC_BTN_DIALOG_LOT_DISPLAY_ADD,			m_btnAdd);
	DDX_Control(pDX, IDC_BTN_DIALOG_LOT_DISPLAY_DELETE,			m_btnDel);
	DDX_Control(pDX, IDC_BTN_DIALOG_LOT_DISPLAY_SCRAP,			m_btnScrapInform);
	DDX_Control(pDX, IDC_BTN_DIALOG_LOT_DISPLAY_ONLINE,			m_btnOnLineInform);
	DDX_Control(pDX, IDC_BTN_DIALOG_LOT_DISPLAY_CLEAR,			m_btnLotClear);
	DDX_Control(pDX, IDC_BTN_DIALOG_LOT_DISPLAY_CANCEL,			m_btnLotCancel);
	DDX_Control(pDX, IDC_MSG_DIALOG_LOT_DISPLAY_BARCODE,		m_msgPcbBarcode);
	DDX_Control(pDX, IDC_MSG_DIALOG_LOT_DISPLAY_BARCODE_DATA,	m_msgPcbBarcodeData);
	DDX_Control(pDX, IDC_MSG_DIALOG_LOT_DISPLAY_SCRAP,			m_msgScrapCode);
	DDX_Control(pDX, IDC_MSG_DIALOG_LOT_DISPLAY_SCRAP_DATA,		m_msgScrapCodeData);
	//	DDX_Control(pDX, IDC_MSG_DIALOG_LOT_DISPLAY_PCB_CNT,		m_msgPcbCnt);
	//	DDX_Control(pDX, IDC_MSG_DIALOG_LOT_DISPLAY_PCB_CNT_DATA,	m_msgPcbCntData);
	DDX_Control(pDX, IDC_MSG_DIALOG_LOT_DISPLAY_ARRAY_SN,		m_msgArraySn);
	DDX_Control(pDX, IDC_MSG_DIALOG_LOT_DISPLAY_ARRAY_SN_DATA,	m_msgArraySnData);
	DDX_Control(pDX, IDC_BTN_DIALOG_LOT_DATA_CLEAR, m_btnLotDataClear);
}


BEGIN_MESSAGE_MAP(CDialogLotDisplay, CDialog)
	ON_BN_CLICKED(IDC_BTN_EXIT, &CDialogLotDisplay::OnClickedBtnExit)
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
//	ON_BN_CLICKED(IDC_BTN_PCB_INFORM, &CDialogLotDisplay::OnBnClickedBtnPcbInform)
//	ON_BN_CLICKED(IDC_BTN_TRACK_INFORM, &CDialogLotDisplay::OnBnClickedBtnTrackOut)
	ON_BN_CLICKED(IDC_BTN_DIALOG_LOT_DISPLAY_ADD, &CDialogLotDisplay::OnBnClickedBtnDialogLotDisplayAdd)
	ON_BN_CLICKED(IDC_BTN_DIALOG_LOT_DISPLAY_DELETE, &CDialogLotDisplay::OnBnClickedBtnDialogLotDisplayDelete)
	ON_STN_CLICKED(IDC_MSG_DIALOG_LOT_DISPLAY_BARCODE_DATA, &CDialogLotDisplay::OnStnClickedMsgDialogLotDisplayBarcodeData)
	ON_STN_CLICKED(IDC_MSG_DIALOG_LOT_DISPLAY_SCRAP_DATA, &CDialogLotDisplay::OnStnClickedMsgDialogLotDisplayScrapData)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_PCB_INFO, OnScarpCellClick)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_LOT_INFO, OnLotCellClick)
	ON_BN_CLICKED(IDC_BTN_DIALOG_LOT_DISPLAY_SCRAP, &CDialogLotDisplay::OnBnClickedBtnDialogLotDisplayScrap)
	ON_BN_CLICKED(IDC_BTN_DIALOG_LOT_DISPLAY_ONLINE, &CDialogLotDisplay::OnBnClickedBtnDialogLotDisplayOnline)
//	ON_STN_CLICKED(IDC_MSG_DIALOG_LOT_DISPLAY_PCB_CNT_DATA, &CDialogLotDisplay::OnStnClickedMsgDialogLotDisplayPcbCntData)
	ON_BN_CLICKED(IDC_BTN_DIALOG_LOT_DISPLAY_CLEAR, &CDialogLotDisplay::OnBnClickedBtnDialogLotDisplayClear)
	ON_STN_CLICKED(IDC_MSG_DIALOG_LOT_DISPLAY_ARRAY_SN_DATA, &CDialogLotDisplay::OnStnClickedMsgDialogLotDisplayArraySnData)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_DIALOG_LOT_DISPLAY_END, &CDialogLotDisplay::OnBnClickedBtnDialogLotDisplayEnd)
	ON_BN_CLICKED(IDC_BTN_DIALOG_LOT_DISPLAY_CANCEL1, &CDialogLotDisplay::OnBnClickedBtnDialogLotDisplayCancel1)
	ON_BN_CLICKED(IDC_BTN_DIALOG_LOT_DISPLAY_CANCEL, &CDialogLotDisplay::OnBnClickedBtnDialogLotDisplayCancel)
	ON_BN_CLICKED(IDC_BTN_DIALOG_LOT_DATA_CLEAR, &CDialogLotDisplay::OnBnClickedBtnDialogLotDataClear)
END_MESSAGE_MAP()


// CDialogLotDisplay 메시지 처리기입니다.

BOOL CDialogLotDisplay::Create()
{
	return CDialog::Create( CDialogLotDisplay::IDD );
}

BOOL CDialogLotDisplay::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	OnInitGroupBox();
	OnInitLabel();
	OnInitButton();

	OnInitGridLotInfo();

	if (m_strLotNo != _T(""))
	{
		st_handler_info.nMenuLock = TRUE;

		m_nLotStatusStep	= 0;

		FAS_IO.set_out_bit(st_io_info.o_Buzz3OnOff, IO_ON);

		SetTimer(TM_STATUS, 100, NULL);
	}

	m_ArScrapCode.SetSize(1000);
	m_ArScrapSerial.SetSize(1000);
	m_ArArraySn.SetSize(1000);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDialogLotDisplay::OnClickedBtnExit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (st_handler_info.nMenuLock == TRUE)
	{
		st_handler_info.nMenuLock = FALSE;
	}

//	CDialog::OnOK();
	if (st_handler_info.cWndMain != NULL)
	{
		st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_LOT_END_DELETE_REQ, 0);

		// jtkim 20150818
		if (st_track_info.nStatus == YES)
		{
			st_track_info.nStatus = NO;
		}

	}
}


void CDialogLotDisplay::OnInitGroupBox()
{
	m_groupLotDisplay.SetFont(clsFunc.OnLogFont(16));
	m_groupLotDisplay.SetCatptionTextColor(BLUE_C);
	m_groupLotDisplay.SetBorderColor(ORANGE_C);
	m_groupLotDisplay.SetFontBold(TRUE);
	m_groupLotDisplay.SetBackgroundColor(WINDOW_UP);
}


void CDialogLotDisplay::OnInitLabel()
{
	m_msgPcbBarcode.ShowWindow(SW_SHOW);
	m_msgPcbBarcode.SetFont(clsFunc.m_pFont[1]);
	m_msgPcbBarcode.SetWindowText(_T("Serial"));
	m_msgPcbBarcode.SetCenterText();
	m_msgPcbBarcode.SetColor(WHITE_C);
	m_msgPcbBarcode.SetGradientColor(ORANGE_C);
	m_msgPcbBarcode.SetTextColor(BLACK_C);

	m_msgPcbBarcodeData.ShowWindow(SW_SHOW);
	m_msgPcbBarcodeData.SetFont(clsFunc.m_pFont[1]);
	m_msgPcbBarcodeData.SetWindowText(_T("-"));
	m_msgPcbBarcodeData.SetCenterText();
	m_msgPcbBarcodeData.SetColor(WHITE_C);
	m_msgPcbBarcodeData.SetGradientColor(WHITE_C);
	m_msgPcbBarcodeData.SetTextColor(BLACK_C);

	m_msgScrapCode.ShowWindow(SW_SHOW);
	m_msgScrapCode.SetFont(clsFunc.m_pFont[1]);
	m_msgScrapCode.SetWindowText(_T("Scrap Code"));
	m_msgScrapCode.SetCenterText();
	m_msgScrapCode.SetColor(WHITE_C);
	m_msgScrapCode.SetGradientColor(ORANGE_C);
	m_msgScrapCode.SetTextColor(BLACK_C);

	m_msgScrapCodeData.ShowWindow(SW_SHOW);
	m_msgScrapCodeData.SetFont(clsFunc.m_pFont[1]);
	m_msgScrapCodeData.SetWindowText(_T("-"));
	m_msgScrapCodeData.SetCenterText();
	m_msgScrapCodeData.SetColor(WHITE_C);
	m_msgScrapCodeData.SetGradientColor(WHITE_C);
	m_msgScrapCodeData.SetTextColor(BLACK_C);

	m_msgArraySn.ShowWindow(SW_SHOW);
	m_msgArraySn.SetFont(clsFunc.m_pFont[1]);
	m_msgArraySn.SetWindowText(_T("Array S/N"));
	m_msgArraySn.SetCenterText();
	m_msgArraySn.SetColor(WHITE_C);
	m_msgArraySn.SetGradientColor(ORANGE_C);
	m_msgArraySn.SetTextColor(BLACK_C);

	m_msgArraySnData.ShowWindow(SW_SHOW);
	m_msgArraySnData.SetFont(clsFunc.m_pFont[1]);
	m_msgArraySnData.SetWindowText(_T("-"));
	m_msgArraySnData.SetCenterText();
	m_msgArraySnData.SetColor(WHITE_C);
	m_msgArraySnData.SetGradientColor(WHITE_C);
	m_msgArraySnData.SetTextColor(BLACK_C);

// 	m_msgPcbCnt.ShowWindow(SW_SHOW);
// 	m_msgPcbCnt.SetFont(clsFunc.m_pFont[1]);
// 	m_msgPcbCnt.SetWindowText(_T("PCB Count"));
// 	m_msgPcbCnt.SetCenterText();
// 	m_msgPcbCnt.SetColor(WHITE_C);
// 	m_msgPcbCnt.SetGradientColor(ORANGE_C);
// 	m_msgPcbCnt.SetTextColor(BLACK_C);

// 	m_msgPcbCntData.ShowWindow(SW_SHOW);
// 	m_msgPcbCntData.SetFont(clsFunc.m_pFont[1]);
// 	m_msgPcbCntData.SetWindowText(_T("-"));
// 	m_msgPcbCntData.SetCenterText();
// 	m_msgPcbCntData.SetColor(WHITE_C);
// 	m_msgPcbCntData.SetGradientColor(WHITE_C);
// 	m_msgPcbCntData.SetTextColor(BLACK_C);

	m_msgPcbBarcode.EnableWindow(TRUE);
	m_msgPcbBarcodeData.EnableWindow(TRUE);
	m_msgPcbBarcode.ShowWindow(SW_SHOW);
	m_msgPcbBarcodeData.ShowWindow(SW_SHOW);

	//m_msgPcbCnt.EnableWindow(FALSE);
	//m_msgPcbCntData.EnableWindow(FALSE);
	//m_msgPcbCnt.ShowWindow(SW_HIDE);
	//m_msgPcbCntData.ShowWindow(SW_HIDE);
/*
	if (st_basic_info.nModeWork != WORK_BYPASS)
	{
		m_msgPcbBarcode.EnableWindow(TRUE);
		m_msgPcbBarcodeData.EnableWindow(TRUE);
		m_msgPcbBarcode.ShowWindow(SW_SHOW);
		m_msgPcbBarcodeData.ShowWindow(SW_SHOW);
		
		m_msgPcbCnt.EnableWindow(FALSE);
		m_msgPcbCntData.EnableWindow(FALSE);
		m_msgPcbCnt.ShowWindow(SW_HIDE);
		m_msgPcbCntData.ShowWindow(SW_HIDE);
	}
	else
	{
		m_msgPcbBarcode.EnableWindow(FALSE);
		m_msgPcbBarcodeData.EnableWindow(FALSE);
		m_msgPcbBarcode.ShowWindow(SW_HIDE);
		m_msgPcbBarcodeData.ShowWindow(SW_HIDE);

		m_msgPcbCnt.EnableWindow(TRUE);
		m_msgPcbCntData.EnableWindow(TRUE);
		m_msgPcbCnt.ShowWindow(SW_SHOW);
		m_msgPcbCntData.ShowWindow(SW_SHOW);
	}
*/
}


void CDialogLotDisplay::OnInitButton()
{
	m_btnExit.SetBitmaps(IDC_BTN_EXIT, IDB_BITMAP_EXIT, WINDOW_DN, IDB_BITMAP_EXIT, WINDOW_UP);
	m_btnExit.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnExit.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnExit.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnExit.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnExit.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnExit.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnExit.SetFont(clsFunc.m_pFont[3]);
	m_btnExit.SetTooltipText(_T("Exit"));
/*
	m_btnBadInform.SetBitmaps(IDC_BTN_BAD_INFORM, IDB_BITMAP_CREATE_UP, WINDOW_DN, IDB_BITMAP_CREATE_UP, WINDOW_UP);
	m_btnBadInform.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnBadInform.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnBadInform.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnBadInform.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnBadInform.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnBadInform.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnBadInform.SetFont(clsFunc.m_pFont[3]);
	m_btnBadInform.SetTooltipText(_T("Bad PCB Inform"));
*/
	m_btnLotEnd.SetBitmaps(IDC_BTN_DIALOG_LOT_DISPLAY_END, IDB_NETWORK_ON_UP, WINDOW_DN, IDB_NETWORK_ON_UP, WINDOW_UP);
	m_btnLotEnd.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnLotEnd.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnLotEnd.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnLotEnd.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnLotEnd.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnLotEnd.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnLotEnd.SetFont(clsFunc.m_pFont[3]);
	m_btnLotEnd.SetTooltipText(_T("Track Out"));
	m_btnLotEnd.EnableWindow(FALSE);
	m_btnLotEnd.EnableWindow(TRUE);//YBS 2016.1007 TEST

	m_btnAdd.SetBitmaps(IDC_BTN_DIALOG_LOT_DISPLAY_ADD, IDB_BITMAP_CREATE_UP, WINDOW_DN, IDB_BITMAP_CREATE_UP, WINDOW_UP);
	m_btnAdd.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnAdd.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnAdd.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnAdd.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnAdd.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnAdd.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnAdd.SetFont(clsFunc.m_pFont[3]);
	m_btnAdd.SetTooltipText(_T("Add"));

	m_btnDel.SetBitmaps(IDC_BTN_DIALOG_LOT_DISPLAY_DELETE, IDB_BITMAP_DELETE_UP, WINDOW_DN, IDB_BITMAP_DELETE_UP, WINDOW_UP);
	m_btnDel.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnDel.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnDel.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnDel.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnDel.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnDel.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnDel.SetFont(clsFunc.m_pFont[3]);
	m_btnDel.SetTooltipText(_T("Del"));

	m_btnScrapInform.SetBitmaps(IDC_BTN_DIALOG_LOT_DISPLAY_DELETE, IDB_NETWORK_ON_UP, WINDOW_DN, IDB_NETWORK_ON_UP, WINDOW_UP);
	m_btnScrapInform.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnScrapInform.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnScrapInform.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnScrapInform.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnScrapInform.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnScrapInform.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnScrapInform.SetFont(clsFunc.m_pFont[3]);
	m_btnScrapInform.SetTooltipText(_T("Scrap Inform"));

	m_btnOnLineInform.SetBitmaps(IDC_BTN_DIALOG_LOT_DISPLAY_DELETE, IDB_NETWORK_ON_UP, WINDOW_DN, IDB_NETWORK_ON_UP, WINDOW_UP);
	m_btnOnLineInform.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnOnLineInform.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnOnLineInform.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnOnLineInform.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnOnLineInform.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnOnLineInform.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnOnLineInform.SetFont(clsFunc.m_pFont[3]);
	m_btnOnLineInform.SetTooltipText(_T("OnLine Inform"));
	m_btnOnLineInform.EnableWindow(FALSE);

	m_btnLotClear.SetBitmaps(IDC_BTN_DIALOG_LOT_DISPLAY_CLEAR, IDB_BITMAP_DOOR_OPEN, WINDOW_DN, IDB_BITMAP_DOOR_OPEN, WINDOW_UP);
	m_btnLotClear.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnLotClear.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnLotClear.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnLotClear.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnLotClear.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnLotClear.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnLotClear.SetFont(clsFunc.m_pFont[3]);
	m_btnLotClear.SetTooltipText(_T("Door Open"));

	m_btnLotCancel.SetBitmaps(IDC_BTN_DIALOG_LOT_DISPLAY_CANCEL, IDB_NETWORK_ON_DN, WINDOW_DN, IDB_NETWORK_ON_UP, WINDOW_UP);
	m_btnLotCancel.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnLotCancel.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnLotCancel.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnLotCancel.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnLotCancel.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnLotCancel.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnLotCancel.SetFont(clsFunc.m_pFont[3]);
	m_btnLotCancel.EnableWindow(FALSE);
	m_btnLotCancel.SetTooltipText(_T("Lot Cancel"));

	m_btnLotDataClear.SetBitmaps(IDC_BTN_DIALOG_LOT_DATA_CLEAR, IDB_NETWORK_ON_DN, WINDOW_DN, IDB_NETWORK_ON_UP, WINDOW_UP);
	m_btnLotDataClear.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnLotDataClear.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnLotDataClear.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnLotDataClear.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnLotDataClear.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnLotDataClear.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnLotDataClear.SetFont(clsFunc.m_pFont[3]);
	m_btnLotDataClear.SetTooltipText(_T("Lot Data Clear"));
}

BOOL CDialogLotDisplay::OnEraseBkgnd(CDC* pDC)
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


void CDialogLotDisplay::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CString strTemp;
	// jtkim 20150408
	CDialog_Message dlgMsg;

	int nRet;
	// jtkim 20150408
	int nResponse;
	int nCount;

	switch (nIDEvent)
	{
		case TM_TRACK:
			//nRet = OnTrackOutReq();
			nRet = OnLotEnd();
			if (nRet == RET_GOOD)
			{
				KillTimer(TM_TRACK);

				st_handler_info.nMenuLock = FALSE;
			}
			else if (nRet != RET_PROCEED)
			{
				KillTimer(TM_TRACK);

				m_btnLotEnd.EnableWindow(TRUE);
				m_btnLotClear.EnableWindow(TRUE);

				st_handler_info.nMenuLock = FALSE;
			}
			break;

		case TM_ONLINE:
			nRet = OnOnLineEndReq();

			if (nRet == RET_GOOD)
			{
				KillTimer(TM_ONLINE);
/*
				m_btnTrackInform.EnableWindow(TRUE);
				m_btnOnLineInform.EnableWindow(TRUE);
*/
				m_strPcbBarcodeData = _T("");
				m_strScrapCodeData	= _T("");
				m_strArraySnData	= _T("");

				m_msgPcbBarcodeData.SetWindowText(m_strPcbBarcodeData);
				m_msgScrapCodeData.SetWindowText(m_strScrapCodeData);
				m_msgArraySnData.SetWindowText(m_strArraySnData);

				OnInitGridPcbInfo();

				m_btnAdd.EnableWindow(TRUE);

				m_nScrapStep = 0;
				SetTimer(TM_SCRAP, 100, NULL);
//				st_handler_info.nMenuLock = FALSE;
			}
			else if (nRet != RET_PROCEED)
			{
				KillTimer(TM_ONLINE);

				m_btnAdd.EnableWindow(TRUE);

				st_handler_info.nMenuLock = FALSE;
			}
			break;

		case TM_SCRAP:
			nRet = OnScrapReq();

			if (nRet == RET_GOOD)
			{
				KillTimer(TM_SCRAP);
				
				OnLotDataDisplay();
				OnInitGridPcbInfo();

				//m_btnOnLineInform.EnableWindow(TRUE);
				// jtkim 20150408
				nCount = st_track_info.nCumPass + st_track_info.nCumReject;
				//if (st_track_info.nIn == st_lot_display_info[1].nQty)
				//2016.1205
				if (nCount == st_lot_display_info[1].nQty)
				{
					m_btnScrapInform.EnableWindow(TRUE);
				///	m_btnTrackInform.EnableWindow(TRUE);
					m_btnLotCancel.EnableWindow(TRUE);
					m_btnLotEnd.EnableWindow(TRUE);

					st_handler_info.nMenuLock = FALSE;
				}
				else
				{
					st_handler_info.nDlgLock = TRUE;
					clsFunc.OnMCStop();

					dlgMsg.m_nMessageType	= 1;

					dlgMsg.m_strMessage.Format(_T("[IN : %d] [SEE : %d] Lot PCB Count Mismatch\r\nTrack Out Yes/No? "), st_track_info.nIn,
																														st_lot_display_info[1].nQty);

					
					nResponse = (int)dlgMsg.DoModal();

					if(nResponse == IDOK)
					{
						m_btnScrapInform.EnableWindow(TRUE);
						m_btnLotCancel.EnableWindow(TRUE);
						m_btnLotEnd.EnableWindow(TRUE);
					}

					st_handler_info.nMenuLock = FALSE;

					strTemp.Format(_T("[IN : %d] [SEE : %d] Lot PCB Count Mismatch\r\nTrack Out Yes/No? "), nCount,
                            																					st_lot_display_info[1].nQty);
					clsMem.OnAbNormalMessagWrite(strTemp);
						
					m_btnScrapInform.EnableWindow(TRUE);
					st_handler_info.nMenuLock = FALSE;
				}
			}
			else if (nRet != RET_PROCEED)
			{
				KillTimer(TM_SCRAP);

				m_btnScrapInform.EnableWindow(TRUE);
				m_btnLotCancel.EnableWindow(TRUE);
				m_btnLotEnd.EnableWindow(TRUE);

				st_handler_info.nMenuLock = FALSE;
			}
			break;

		case TM_DEL:
			nRet = OnDelScrapReq();

			if (nRet == RET_GOOD)
			{
				KillTimer(TM_DEL);

				m_strPcbBarcodeData = _T("");
				m_strScrapCodeData	= _T("");
				m_strArraySnData	= _T("");

				m_msgPcbBarcodeData.SetWindowText(m_strPcbBarcodeData);
				m_msgScrapCodeData.SetWindowText(m_strScrapCodeData);
				m_msgArraySnData.SetWindowText(m_strArraySnData);

				OnInitGridPcbInfo();
				
				m_btnDel.EnableWindow(TRUE);

//				st_handler_info.nMenuLock = FALSE;
				m_nScrapStep = 0;
				SetTimer(TM_SCRAP, 100, NULL);
			}
			else if (nRet != RET_PROCEED)
			{
				KillTimer(TM_DEL);

				m_btnDel.EnableWindow(TRUE);

				st_handler_info.nMenuLock = FALSE;
			}
			break;

		case TM_STATUS:
			nRet = OnLotStatusReq();

			if (nRet == RET_GOOD)
			{
				KillTimer(TM_STATUS);

				OnLotDataDisplay();

				m_nScrapStep = 0;
				SetTimer(TM_SCRAP, 100, NULL);
			}
			else if (nRet != RET_PROCEED)
			{
				KillTimer(TM_STATUS);

				st_handler_info.nMenuLock = FALSE;
			}
			break;

		case TM_TEST:
			nRet = OnTestEndReq();

			if (nRet == RET_GOOD)
			{
				KillTimer(TM_TEST);

				m_btnScrapInform.EnableWindow(FALSE);

				m_nScrapStep = 0;
				SetTimer(TM_SCRAP, 100, NULL);
			}
			else if (nRet != RET_PROCEED)
			{
				KillTimer(TM_TEST);

				st_handler_info.nMenuLock = FALSE;
			}
			break;

		case TM_EXIT:
			KillTimer(TM_EXIT);

			OnClickedBtnExit();
			break;

		case TM_CANCEL:
			nRet = OnLotCancelReq();

			if (nRet == RET_GOOD)
			{
				KillTimer(TM_CANCEL);

				m_btnLotCancel.EnableWindow(TRUE);
			}
			else if (nRet != RET_PROCEED)
			{
				KillTimer(TM_CANCEL);

				st_handler_info.nMenuLock = FALSE;

				m_btnLotCancel.EnableWindow(TRUE);
			}
			break;
	}
	CDialog::OnTimer(nIDEvent);
}


void CDialogLotDisplay::OnInitGridPcbInfo()
{
	int i, j;
	int nMaxRow, nMaxCol;
//	int	nPos[2];
//	int nCount;

	CString strHeader[4] = {_T("No."),  _T("Scrap Code"), _T("Serial"), _T("Array S/N")};
	CString strTemp;
	CString strData;
	CString strPcb[7];
	
	if (m_nScrapCount == 0)
	{	
		nMaxRow = m_nScrapCount + 1;
		nMaxCol = 4;

		m_pGridPcbInfo.Refresh();

		m_pGridPcbInfo.SetFrameFocusCell(FALSE);
		m_pGridPcbInfo.SetTrackFocusCell(FALSE);
		m_pGridPcbInfo.EnableSelection(FALSE);

		m_pGridPcbInfo.SetGridLineColor(BLACK_C);
		m_pGridPcbInfo.SetGridLines(1);

		m_pGridPcbInfo.SetRowCount(nMaxRow);
		m_pGridPcbInfo.SetColumnCount(nMaxCol);

		m_pGridPcbInfo.SetFixedRowCount(0);
		m_pGridPcbInfo.SetFixedColumnCount(0);
		m_pGridPcbInfo.SetFixedBkColor(RGB(0,0,200));
		m_pGridPcbInfo.SetFixedBkColor(RGB(200,200,255));
		m_pGridPcbInfo.SetTextBkColor(RGB(150,150,200));

		for (i=0; i<nMaxRow; i++)
		{
			m_pGridPcbInfo.SetRowHeight(i, 30);

			for (j=0; j<nMaxCol; j++)
			{
				if (j == 0)
				{
					m_pGridPcbInfo.SetColumnWidth(j, 40);
				}
				else if (j == 1)
				{
					m_pGridPcbInfo.SetColumnWidth(j, 110);
				}
				else if (j == 2)
				{
					m_pGridPcbInfo.SetColumnWidth(j, 180);
				}
				else
				{
					m_pGridPcbInfo.SetColumnWidth(j, 200);
				}

				if (i == 0)
				{
					m_pGridPcbInfo.SetItemBkColour(i, j, BLACK_L, WHITE_C);
					m_pGridPcbInfo.SetItemText(0, j, strHeader[j]);
				}
				else
				{
					if (j == 0)
					{
						m_pGridPcbInfo.SetItemBkColour(i, j, BLACK_L, WHITE_C);
						strTemp.Format(_T("%d"), i);
						m_pGridPcbInfo.SetItemText(i, 0, strTemp);
					}
					else
					{
						m_pGridPcbInfo.SetItemBkColour(i, j, WHITE_C, BLACK_C);
					}
				}

				m_pGridPcbInfo.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
				m_pGridPcbInfo.SetItemFont(i, j, &clsFunc.OnLogFont(14));
				m_pGridPcbInfo.SetItemState(i, j, GVIS_READONLY);
			}
		}

		return;
	}

	nMaxRow = m_nScrapCount + 1;
	nMaxCol = 4;
	
	m_pGridPcbInfo.Refresh();

	m_pGridPcbInfo.SetFrameFocusCell(FALSE);
	m_pGridPcbInfo.SetTrackFocusCell(FALSE);
	m_pGridPcbInfo.EnableSelection(FALSE);
				   
	m_pGridPcbInfo.SetGridLineColor(BLACK_C);
	m_pGridPcbInfo.SetGridLines(1);
				   
	m_pGridPcbInfo.SetRowCount(nMaxRow);
	m_pGridPcbInfo.SetColumnCount(nMaxCol);
				   
	m_pGridPcbInfo.SetFixedRowCount(0);
	m_pGridPcbInfo.SetFixedColumnCount(0);
	m_pGridPcbInfo.SetFixedBkColor(RGB(0,0,200));
	m_pGridPcbInfo.SetFixedBkColor(RGB(200,200,255));
	m_pGridPcbInfo.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<nMaxRow; i++)
	{
		m_pGridPcbInfo.SetRowHeight(i, 30);

		for (j=0; j<nMaxCol; j++)
		{
			if (j == 0)
			{
				m_pGridPcbInfo.SetColumnWidth(j, 50);
			}
			else if (j == 1)
			{
				m_pGridPcbInfo.SetColumnWidth(j, 110);
			}
			else if (j == 2)
			{
				m_pGridPcbInfo.SetColumnWidth(j, 180);
			}
			else
			{
				m_pGridPcbInfo.SetColumnWidth(j, 200);
			}

			if (i == 0)
			{
				m_pGridPcbInfo.SetItemBkColour(i, j, BLACK_L, WHITE_C);
				m_pGridPcbInfo.SetItemText(0, j, strHeader[j]);
			}
			else
			{
				if (j == 0)
				{
					m_pGridPcbInfo.SetItemBkColour(i, j, BLACK_L, WHITE_C);
					strTemp.Format(_T("%d"), i);
					m_pGridPcbInfo.SetItemText(i, 0, strTemp);
				}
				else
				{
					m_pGridPcbInfo.SetItemBkColour(i, j, WHITE_C, BLACK_C);
				}
			}

			m_pGridPcbInfo.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridPcbInfo.SetItemFont(i, j, &clsFunc.OnLogFont(14));
			m_pGridPcbInfo.SetItemState(i, j, GVIS_READONLY);
		}
	}

	for (i=0; i<m_nScrapCount; i++)
	{
		strTemp.Format(_T("%d"), i + 1);
		m_pGridPcbInfo.SetItemText(i + 1, 0, strTemp);
		m_pGridPcbInfo.SetItemText(i + 1, 1, m_ArScrapCode.GetAt(i));
		m_pGridPcbInfo.SetItemText(i + 1, 2, m_ArScrapSerial.GetAt(i));
		m_pGridPcbInfo.SetItemText(i + 1, 3, m_ArArraySn.GetAt(i));
	}

	m_pGridPcbInfo.Invalidate(FALSE);
}


void CDialogLotDisplay::OnInitGridLotInfo()
{
	int i, j;
	int nMaxRow, nMaxCol;
	int nRow;

	CString strTemp;

	nMaxRow = 7;
	nMaxCol = 4;

	m_pGridLotInfo.Refresh();

	m_pGridLotInfo.SetFrameFocusCell(FALSE);
	m_pGridLotInfo.SetTrackFocusCell(FALSE);
	m_pGridLotInfo.EnableSelection(FALSE);
				   
	m_pGridLotInfo.SetGridLineColor(BLACK_C);
	m_pGridLotInfo.SetGridLines(1);
				   
	m_pGridLotInfo.SetRowCount(nMaxRow);
	m_pGridLotInfo.SetColumnCount(nMaxCol);
				   
	m_pGridLotInfo.SetFixedRowCount(0);
	m_pGridLotInfo.SetFixedColumnCount(0);
	m_pGridLotInfo.SetFixedBkColor(RGB(0,0,200));
	m_pGridLotInfo.SetFixedBkColor(RGB(200,200,255));
	m_pGridLotInfo.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<nMaxRow; i++)
	{
		m_pGridLotInfo.SetRowHeight(i, 34);

		for (j=0; j<nMaxCol; j++)
		{
			if ((j % 2) == 0)
			{
				m_pGridLotInfo.SetColumnWidth(j, 150);
			}
			else
			{
				m_pGridLotInfo.SetColumnWidth(j, 237);
			}
			
			m_pGridLotInfo.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridLotInfo.SetItemFont(i, j, &clsFunc.OnLogFont(14));
			m_pGridLotInfo.SetItemState(i, j, GVIS_READONLY);
		}
	}

	nRow = 0;
	m_pGridLotInfo.SetItemBkColour(nRow, 0, BLACK_L, WHITE_C);
	m_pGridLotInfo.SetItemFont(nRow, 0, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 0, _T("Part No."));

	m_pGridLotInfo.MergeCells(nRow, 1, nRow, 2);
	m_pGridLotInfo.SetItemBkColour(nRow, 1, WHITE_C, BLACK_L);
	m_pGridLotInfo.SetItemFont(nRow, 1, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 1, m_strPartNo);

	m_pGridLotInfo.MergeCells(nRow, 3, nRow, 2);
	m_pGridLotInfo.SetItemBkColour(nRow, 3, ORANGE_C, BLACK_L);
	m_pGridLotInfo.SetItemFont(nRow, 3, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 3, _T("Lot Display"));

	nRow = 1;
	m_pGridLotInfo.SetItemBkColour(nRow, 0, BLACK_L, WHITE_C);
	m_pGridLotInfo.SetItemFont(nRow, 0, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 0, _T("Lot No."));

	m_pGridLotInfo.SetItemBkColour(nRow, 1, WHITE_C, BLACK_L);
	m_pGridLotInfo.SetItemFont(nRow, 1, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 1, m_strLotNo);

	m_pGridLotInfo.SetItemBkColour(nRow, 2, BLACK_L, WHITE_C);
	m_pGridLotInfo.SetItemFont(nRow, 2, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 2, _T("Qty"));

	m_pGridLotInfo.SetItemBkColour(nRow, 3, WHITE_C, BLACK_L);
	m_pGridLotInfo.SetItemFont(nRow, 3, &clsFunc.OnLogFont(20));
	strTemp.Format(_T("%d"), m_nLotQty);
	m_pGridLotInfo.SetItemText(nRow, 3, strTemp);

	nRow = 2;
	m_pGridLotInfo.SetItemBkColour(nRow, 0, GREEN_C, BLACK_C);
	m_pGridLotInfo.SetItemFont(nRow, 0, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 0, _T("Pass Qty"));

	m_pGridLotInfo.SetItemBkColour(nRow, 1, WHITE_C, BLACK_L);
	m_pGridLotInfo.SetItemFont(nRow, 1, &clsFunc.OnLogFont(20));
	strTemp.Format(_T("%d"), st_track_info.nCumPass);
	m_pGridLotInfo.SetItemText(nRow, 1, strTemp);

	m_pGridLotInfo.SetItemBkColour(nRow, 2, RED_C, BLACK_C);
	m_pGridLotInfo.SetItemFont(nRow, 2, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 2, _T("Fail Qty"));

	m_pGridLotInfo.SetItemBkColour(nRow, 3, WHITE_C, BLACK_L);
	m_pGridLotInfo.SetItemFont(nRow, 3, &clsFunc.OnLogFont(20));
	strTemp.Format(_T("%d"), st_track_info.nCumPass);
	m_pGridLotInfo.SetItemText(nRow, 3, strTemp);

	nRow = 3;
	m_pGridLotInfo.SetItemBkColour(nRow, 0, BLACK_L, WHITE_C);
	m_pGridLotInfo.SetItemFont(nRow, 0, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 0, _T("One Shot"));

	m_pGridLotInfo.SetItemBkColour(nRow, 1, WHITE_C, BLACK_L);
	m_pGridLotInfo.SetItemFont(nRow, 1, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 1, m_strStep);

	m_pGridLotInfo.SetItemBkColour(nRow, 2, BLACK_L, WHITE_C);
	m_pGridLotInfo.SetItemFont(nRow, 2, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 2, _T("OPT Code"));

	m_pGridLotInfo.SetItemBkColour(nRow, 3, WHITE_C, BLACK_L);
	m_pGridLotInfo.SetItemFont(nRow, 3, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 3, m_strProCid);

	nRow = 4;
	m_pGridLotInfo.SetItemBkColour(nRow, 0, BLACK_L, WHITE_C);
	m_pGridLotInfo.SetItemFont(nRow, 0, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 0, _T("Tier"));

	m_pGridLotInfo.SetItemBkColour(nRow, 1, WHITE_C, BLACK_L);
	m_pGridLotInfo.SetItemFont(nRow, 1, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 1, m_strLotState);

	m_pGridLotInfo.SetItemBkColour(nRow, 2, BLACK_L, WHITE_C);
	m_pGridLotInfo.SetItemFont(nRow, 2, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 2, _T("FAB Site"));

	m_pGridLotInfo.SetItemBkColour(nRow, 3, WHITE_C, BLACK_L);
	m_pGridLotInfo.SetItemFont(nRow, 3, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 3, m_strMainState);

	nRow = 5;
	m_pGridLotInfo.SetItemBkColour(nRow, 0, BLACK_L, WHITE_C);
	m_pGridLotInfo.SetItemFont(nRow, 0, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 0, _T("Rework Count"));

	m_pGridLotInfo.SetItemBkColour(nRow, 1, WHITE_C, BLACK_L);
	m_pGridLotInfo.SetItemFont(nRow, 1, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 1, m_strComplot);

	m_pGridLotInfo.SetItemBkColour(nRow, 2, BLACK_L, WHITE_C);
	m_pGridLotInfo.SetItemFont(nRow, 2, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 2, _T("-"));

	m_pGridLotInfo.SetItemBkColour(nRow, 3, WHITE_C, BLACK_L);
	m_pGridLotInfo.SetItemFont(nRow, 3, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 3, _T("-"));

	nRow = 6;
	m_pGridLotInfo.SetItemBkColour(nRow, 0, ORANGE_C, BLACK_C);
	m_pGridLotInfo.SetItemFont(nRow, 0, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 0, _T("Message"));
	
	m_pGridLotInfo.MergeCells(nRow, 1, nRow, 3);
	m_pGridLotInfo.SetItemBkColour(nRow, 1, WHITE_C, BLACK_L);
	m_pGridLotInfo.SetItemFont(nRow, 1, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 1, _T("-"));

	m_pGridLotInfo.Invalidate(FALSE);
}

void CDialogLotDisplay::OnLotDataDisplay()
{
	CString strTemp;

	m_pGridLotInfo.SetItemText(0, 1, m_strPartNo);
	m_pGridLotInfo.SetItemText(1, 1, m_strLotNo);

	strTemp.Format(_T("%d"), m_nLotQty);
	m_pGridLotInfo.SetItemText(1, 3, strTemp);

	//m_pGridLotInfo.SetItemText(2, 1, m_strLotType);
	strTemp.Format(_T("%d"), m_nScrapCount);
	m_pGridLotInfo.SetItemText(2, 3, strTemp);

	m_pGridLotInfo.SetItemText(3, 1, m_strStep);
	m_pGridLotInfo.SetItemText(3, 3, m_strProCid);

	m_pGridLotInfo.SetItemText(4, 1, m_strLotState);
	m_pGridLotInfo.SetItemText(4, 3, m_strMainState);

	m_pGridLotInfo.SetItemText(5, 1, m_strComplot);
	m_pGridLotInfo.SetItemText(5, 3, m_strLine);

	m_pGridLotInfo.Invalidate(FALSE);
}

void CDialogLotDisplay::OnBnClickedBtnPcbInform()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
// 	CDialogBadInform dlgBad;
// 	
// 		if (dlgBad.DoModal() == IDOK)
// 		{
// 			OnIntGridPcbInfo();
// 		}
}



int	CDialogLotDisplay::OnOnLineEndReq()
{
	switch (m_nOnLineStep)
	{
		case 0:
			// 작업중인 모드체크
			if (st_basic_info.nModeInterface == EQP_ON_LINE)
			{
				// 작업모드가 on line 모드이면

				m_nOnLineStep = 100;
			}
			else
			{
				// off line 모드이면 
				m_nOnLineStep = 0;

				return RET_GOOD;
			}
			break;

		case 100:
			clsEcSecond.OnEcOnlineEnd(_T(""), st_lot_display_info[1]);

			m_nOnLineStep = 200;
			break;

		case 200:
			if (clsEcSecond.m_nOnlineEndFlag == BD_DATA_GOOD)
			{
				OnMessageDisplay(BIN_GOODBIN, _T("On-line End 데이터 전송 성공"));

				m_nOnLineStep = 0;

				return RET_GOOD;
			}
			else if (clsEcSecond.m_nOnlineEndFlag == BD_DATA_REJECT)
			{
				OnMessageDisplay(BIN_REJECTBIN, _T("On-line End 데이터 전송 실패"));

				m_nOnLineStep = 0;

				return RET_ERROR;
			}
			else
			{
				if (clsEcSecond.m_nCommBin == BD_CONT_TIMEOUT)
				{
					OnMessageDisplay(BIN_REJECTBIN, _T("EC 서버 접속 실패"));

					return RET_ERROR;
				}
			}
			break;
	}

	return RET_PROCEED;
}


int	CDialogLotDisplay::OnPcbInfoReq()
{
	CString strTemp;

	switch (m_nPcbInfoStep)
	{
		case 0:
			// 작업중인 모드체크
			if (st_basic_info.nModeInterface == EQP_ON_LINE)
			{
				// 작업모드가 on line 모드이면
				m_dwWaitTime[0] = GetTickCount();

				m_nPcbInfoStep = 100;
			}
			else
			{
				// off line 모드이면 
				m_nPcbInfoStep = 0;

				return RET_GOOD;
			}
			break;

		case 100:
			// datacenter client 현재 상태 체크
			if (st_client_info[EC_FIRST_NETWORK].nConnect == NO)
			{
				// 클라이언트 대기중이면 
				m_nPcbInfoStep = 200;
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
					// 5초동안 대기해서 강제 종료
					m_dwWaitTime[1] = GetTickCount();
					//::PostMessage(st_handler_info.hWnd, WM_CLIENT_MSG + EC_FIRST_NETWORK, CLIENT_CLOSE, 0);
					::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + EC_FIRST_NETWORK, CLIENT_CLOSE, 0);
				}
			}
			break;

		case 200:
			// client 연결 요청 
			m_dwWaitTime[0] = GetTickCount();
			//::PostMessage(st_handler_info.hWnd, WM_CLIENT_MSG + EC_FIRST_NETWORK, CLIENT_CONNECT, 0);
			::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + EC_FIRST_NETWORK, CLIENT_CONNECT, 0);

			m_nPcbInfoStep = 300;
			break;

		case 300:
			// client 연결상태 체크
			if (st_client_info[EC_FIRST_NETWORK].nConnect == YES)
			{
				// 현재 연결중이면 데이터 전송으로 
				m_nPcbInfoStep = 400;
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
						m_nPcbInfoStep = 0;
					}
					else
					{
						return RET_ERROR;
					}
				}
			}
			break;

		case 400:
			m_dwWaitTime[0]								= GetTickCount();

//			clsDataClient.m_nLotQtyReq					= BIN_NOTUESD;
//			st_client_info[EC_FIRST_NETWORK].strSend	= clsDataClient.OnLotQty(m_strPcbBarcode);

			//::PostMessage(st_handler_info.hWnd, WM_CLIENT_MSG + EC_FIRST_NETWORK, CLIENT_SEND, 0);
			::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + EC_FIRST_NETWORK, CLIENT_SEND, 0);

			m_nPcbInfoStep = 500;
			break;

		case 500:
			// 데이터 송신 결과 확인 작업
/*			if (clsDataClient.m_nLotQtyReq == BIN_GOODBIN)
			{	
				m_strPcbBarcode = _T("");
				m_pGridLotInfo.SetItemText(0, 1, m_strPcbBarcode);

				m_strLotNo		= clsDataClient.m_strLotNo;
				m_pGridLotInfo.SetItemText(1, 1, m_strLotNo);

				m_strPartNo		= clsDataClient.m_strPartNo;
				m_pGridLotInfo.SetItemText(1, 3, m_strPartNo);

				m_nLotQty		= clsDataClient.m_nLotPcbQty;
				m_nPcbArray		= clsDataClient.m_nPcbArray;

				if (m_nLotQty > 0)
				{
					strTemp.Format(_T("%d / %d"), m_nLotQty, m_nLotQty / m_nPcbArray);
				}
				else
				{
					strTemp.Format(_T("%d / %d"), 0, 0);
				}
				m_pGridLotInfo.SetItemText(2, 1, strTemp);

				m_nXout			= clsDataClient.m_nXout;
				strTemp.Format(_T("%d / %d"), m_nPcbArray, m_nXout);
				m_pGridLotInfo.SetItemText(3, 3, strTemp);
				
				m_pGridLotInfo.Invalidate(FALSE);
				m_nPcbInfoStep = 0;

				return RET_GOOD;
			}
			else if (clsDataClient.m_nLotQtyReq == BIN_REJECTBIN)
			{
				m_nPcbInfoStep = 0;

				return RET_ERROR;
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
					if (m_nRetry < st_basic_info.nRetry)
					{
						m_nPcbInfoStep = 100;
					}
					else
					{
						m_nPcbInfoStep = 0;

						return RET_ERROR;
					}
				}
			}*/
			break;
	}

	return RET_PROCEED;
}


int	CDialogLotDisplay::OnScrapReq()
{
	int i;

	switch (m_nScrapStep)
	{
		case 0:
			// 작업중인 모드체크
			if (st_basic_info.nModeInterface == EQP_ON_LINE)
			{
				// 작업모드가 on line 모드이면
				m_nScrapStep = 100;
			}
			else
			{
				// off line 모드이면 
				m_nScrapStep = 0;

				return RET_GOOD;
			}
			break;

		case 100:
			clsEcSecond.OnEcScrapInfo(m_strLotNo);

			m_nScrapStep = 200;
			break;

		case 200:
			if (clsEcSecond.m_nScrapInfoFlag == BD_DATA_GOOD)
			{
				m_nScrapStep = 300;
			}
			else if (clsEcSecond.m_nScrapInfoFlag == BD_DATA_REJECT)
			{
				OnMessageDisplay(BIN_REJECTBIN, _T("Scrap Info 데이터 전송 실패"));

				m_nScrapStep = 0;

				return RET_ERROR;
			}
			else
			{
				if (clsEcSecond.m_nCommBin == BD_CONT_TIMEOUT)
				{
					OnMessageDisplay(BIN_REJECTBIN, _T("EC 서버 접속 실패"));

					m_nScrapStep = 0;

					return RET_ERROR;
				}
			}
			break;

		case 300:
			if (st_lot_display_info[1].nScrapCnt >= 0)	m_nScrapCount = st_lot_display_info[1].nScrapCnt;
			else m_nScrapCount = 0;

			for (i=0; i<m_nScrapCount; i++)
			{
				m_ArScrapSerial.SetAt(i, st_lot_display_info[1].strScrapSerial[i]);
				m_ArScrapCode.SetAt(i, st_lot_display_info[1].strScrapCode[i]);
				m_ArArraySn.SetAt(i, st_lot_display_info[1].strArray[i]);
			}

			OnMessageDisplay(BIN_GOODBIN, _T("Scrap Info 데이터 전송 성공"));

			m_nScrapStep = 0;

			return RET_GOOD;

			break;
	}
	return RET_PROCEED;
}


int	CDialogLotDisplay::OnLotStatusReq()
{
//	int i;

	switch (m_nLotStatusStep)
	{
		case 0:
			// 작업중인 모드체크
  			if (st_basic_info.nModeInterface == EQP_ON_LINE)
			{
				m_nLotStatusStep = 1000;
			} 
			else
			{
				// off line 모드이면 
				m_nLotStatusStep = 0;

				return RET_GOOD;
			}
			break;

		case 1000:
			clsEcSecond.OnEcLotDisplay(m_strLotNo,_T(""));

			m_nLotStatusStep = 1200;
			break;

		
		case 1200:
			if (clsEcSecond.m_nLotDisplayFlag == BD_DATA_GOOD)
			{
				m_strLotNo		=	st_lot_display_info[1].strLotNo;
				m_strPartNo		=	st_lot_display_info[1].strPartNo;
				//			m_strLotState	=	st_lot_display_info.strLotState;
				//			m_strLotType	=	st_lot_display_info.strLotType;
				//			m_strMainState	=	st_lot_display_info.strMainState;
				//			m_strProCid		=	st_lot_display_info.strProCid;
				//			m_strStep		=	st_lot_display_info.strStep;
				//			m_strComplot	=	st_lot_display_info.strComplot;
				//			m_strLine		=	st_lot_display_info.strLine;

				if (st_lot_display_info[1].nQty >= 0) m_nLotQty		=	st_lot_display_info[1].nQty;
				else m_nLotQty = 0;

				OnMessageDisplay(BIN_GOODBIN, _T("See Lot 데이터 전송 성공"));

				m_nLotStatusStep = 0;

				return RET_GOOD;
			}
			else if (clsEcSecond.m_nLotDisplayFlag == BD_DATA_REJECT)
			{
				m_nLotStatusStep = 0;

				return RET_ERROR;
			}
			else if (clsEcSecond.m_nCommBin == BD_CONT_TIMEOUT)
			{
				m_nLotStatusStep = 0;

				return RET_ERROR;
			}
			else if (clsEcSecond.m_nCommBin == BD_SEND_TIMEOUT)
			{
				m_nLotStatusStep = 0;

				return RET_ERROR;
			}
			else if (clsEcSecond.m_nCommBin == BD_RCV_TIMEOUT)
			{
				m_nLotStatusStep = 0;

				return RET_ERROR;
			}
/*			m_strLotNo		=	st_lot_display_info[1].strLotNo;
			m_strPartNo		=	st_lot_display_info[1].strPartNo;
//			m_strLotState	=	st_lot_display_info.strLotState;
//			m_strLotType	=	st_lot_display_info.strLotType;
//			m_strMainState	=	st_lot_display_info.strMainState;
//			m_strProCid		=	st_lot_display_info.strProCid;
//			m_strStep		=	st_lot_display_info.strStep;
//			m_strComplot	=	st_lot_display_info.strComplot;
//			m_strLine		=	st_lot_display_info.strLine;

			if (st_lot_display_info[1].nQty >= 0) m_nLotQty		=	st_lot_display_info[1].nQty;
			else m_nLotQty = 0;

			OnMessageDisplay(BIN_GOODBIN, _T("See Lot 데이터 전송 성공"));

			m_nLotStatusStep = 0;

			return RET_GOOD;*/

			break;
	}
	return RET_PROCEED;
}


int	CDialogLotDisplay::OnDelScrapReq()
{
	switch (m_nDelStep)
	{
		case 0:
			// 작업중인 모드체크
			if (st_basic_info.nModeInterface == EQP_ON_LINE)
			{
				// 작업모드가 on line 모드이면
				m_nDelStep = 100;
			}
			else
			{
				// off line 모드이면 
				m_nDelStep = 0;

				return RET_GOOD;
			}
			break;

		case 100:
			clsEcSecond.OnEcDeleteScrap(m_strLotNo, m_strPcbBarcodeData);

			m_nDelStep = 200;
			
			break;

		case 200:
			if (clsEcSecond.m_nDelScrapFlg == BD_DATA_GOOD)
			{
				// jtkim 20150309
				m_nScrapCount--;
				m_ArScrapCode.RemoveAt(m_nDelScrapNum, 1);
				m_ArScrapSerial.RemoveAt(m_nDelScrapNum, 1);
				m_ArArraySn.RemoveAt(m_nDelScrapNum, 1);

				m_strPcbBarcodeData = _T("");
				m_strScrapCodeData	= _T("");
				m_strArraySnData	= _T("");

				m_msgPcbBarcodeData.SetWindowText(m_strPcbBarcodeData);
				m_msgScrapCodeData.SetWindowText(m_strScrapCodeData);
				m_msgArraySnData.SetWindowText(m_strArraySnData);

				OnInitGridPcbInfo();

				OnMessageDisplay(BIN_GOODBIN, _T("Delete Scrap 데이터 전송 성공"));

				m_nDelStep = 0;

				return RET_GOOD;
			}
			else if (clsEcSecond.m_nDelScrapFlg == BD_DATA_REJECT)
			{
				OnMessageDisplay(BIN_REJECTBIN, _T("Delete Scrap 데이터 전송 실패"));

				m_nDelStep = 0;

				return RET_ERROR;
			}
			break;
	}

	return RET_PROCEED;
}


int	CDialogLotDisplay::OnTrackOutReq()
{
	switch (m_nTrackOutStep)
	{
		case 0:
			// 작업중인 모드체크
			if (st_basic_info.nModeInterface == EQP_ON_LINE)
			{
				// 작업모드가 on line 모드이면
				m_nTrackOutStep = 100;
			}
			else
			{
				// off line 모드이면 
				m_nTrackOutStep = 0;

				return RET_GOOD;
			}
			break;

		case 100:
//			clsEcSecond.OnEcLotEnd(m_strLotNo, _T(""), _T(""), 10, 0);

			m_nTrackOutStep = 200;
			break;

		case 200:
			if (clsEcSecond.m_nLotEndFlag == BD_DATA_GOOD)
			{
				m_strRfidData[RF_EQP_ID]		= st_basic_info.strEqp;
				m_strRfidData[RF_LOT_ID_J]		= st_lot_display_info[1].strLotNo;
//				m_strRfidData[RF_STEP]			= st_lot_display_info.strLotState;
//				m_strRfidData[RF_LOT_TYPE]		= st_lot_display_info.strLotType;
				m_strRfidData[RF_PART_ID]		= st_lot_display_info[1].strPartNo;
				m_strRfidData[RF_QTY].Format(_T("%d"), st_lot_display_info[1].nQty - st_lot_display_info[1].nScrapCnt);

				m_nTrackOutStep = 0;

				return RET_GOOD;
			}
			else if (clsEcSecond.m_nLotEndFlag == BD_DATA_REJECT)
			{
				m_nTrackOutStep = 0;

				return RET_ERROR;
			}
			else
			{
				if (clsEcSecond.m_nCommBin == BD_CONT_TIMEOUT)
				{
					return RET_ERROR;
				}
			}
			break;
	}

	return RET_PROCEED;
}

int CDialogLotDisplay::OnTestEndReq()
{
	switch(m_nTestEndStep)
	{
		case 0:
			// 작업중인 모드체크
			if (st_basic_info.nModeInterface == EQP_ON_LINE)
			{
				// 작업모드가 on line 모드이면
				m_nTestEndStep = 100;
			}
			else
			{
				// off line 모드이면 
				m_nTestEndStep = 0;

				return RET_GOOD;
			}
			break;

		case 100:
			clsEcSecond.OnEcTestEnd(m_strLotNo, m_strPartNo);

			m_nTestEndStep = 200;
			break;

		case 200:
			if (clsEcSecond.m_nTestEndFlag == BD_DATA_GOOD)
			{
				m_nTestEndStep = 0;

				return RET_GOOD;
			}
			else if (clsEcSecond.m_nTestEndFlag == BD_DATA_REJECT)
			{
				return RET_ERROR;
			}
			else
			{
				if (clsEcSecond.m_nCommBin == BD_CONT_TIMEOUT)
				{
					return RET_ERROR;
				}
			}
			break;

		default:
			if (m_nTestEndStep < 0) m_nTestEndStep = 0;
			break;
	}
	return RET_PROCEED;
}

void CDialogLotDisplay::OnBnClickedBtnDialogLotDisplayAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strBarcode;
	CTime	time;

	m_strScrapCodeData.TrimLeft(' ');
	m_strScrapCodeData.TrimRight(' ');
	m_strLotNo.TrimLeft(' ');
	m_strLotNo.TrimRight(' ');
	m_strPartNo.TrimLeft(' ');
	m_strPartNo.TrimRight(' ');
	if (m_strScrapCodeData == _T("")) return;
	if (m_strLotNo == _T("") || m_strPartNo == _T("")) return;

	if (m_strPcbBarcodeData == _T(""))
	{
		time = CTime::GetCurrentTime();
		strBarcode.Format(_T("%s%04d%02d%02d%02d%02d%02d%08d"), m_strLotNo,
															    time.GetYear(),
															    time.GetMonth(),
															    time.GetDay(),
															    time.GetHour(),
															    time.GetMinute(),
															    time.GetSecond(),
															    rand() % 100000000);

		m_strArraySnData = strBarcode;

	}

	st_lot_display_info[1].nScrapCnt = 1;

	st_lot_display_info[1].strScrapSerial[0]	= m_strPcbBarcodeData;
	st_lot_display_info[1].strScrapCode[0]		= m_strScrapCodeData;
	st_lot_display_info[1].strArray[0]			= m_strArraySnData;

	m_nRetry		= 0;
	m_nOnLineStep	= 0;

	m_btnAdd.EnableWindow(FALSE);

	SetTimer(TM_ONLINE, 100, NULL);
}


void CDialogLotDisplay::OnBnClickedBtnDialogLotDisplayDelete()
{
	int i, nTemp;
	bool bfind;

	if (st_handler_info.nMenuLock == TRUE) return;
	if (m_strPcbBarcodeData == _T("") || m_strScrapCodeData == _T("")) return;
	if (m_nScrapCount <= 0) return;

	bfind = false;
	for (i=0; i<m_nScrapCount; i++)
	{
		if (m_ArScrapCode.GetAt(i)		==	m_strScrapCodeData	&&
			m_ArScrapSerial.GetAt(i)	==	m_strPcbBarcodeData	&&
			m_ArArraySn.GetAt(i)		==	m_strArraySnData)
		{
			bfind = true;
			nTemp = i;
		}
	}
	
	if (bfind)
	{ 
		// jtkim 20150309
		m_nDelScrapNum = nTemp;

		st_handler_info.nMenuLock = TRUE;
		
		m_nDelStep	=	0;

		SetTimer(TM_DEL, 100, NULL);

		m_btnDel.EnableWindow(FALSE);	

		OnInitGridPcbInfo();
	}
}


void CDialogLotDisplay::OnStnClickedMsgDialogLotDisplayBarcodeData()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	/*CDialog_KeyBoard dlgKeyBoard;

	dlgKeyBoard.m_strKeyBoardVal = m_strPcbBarcodeData;

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		m_strPcbBarcodeData = dlgKeyBoard.m_strKeyBoardVal;
		m_msgPcbBarcodeData.SetWindowText(m_strPcbBarcodeData);
	}*/
	CString strKey;

	strKey = m_strPcbBarcodeData;
	KeyBoard(&strKey);

	m_strPcbBarcodeData = strKey;
	m_msgPcbBarcodeData.SetWindowTextW(m_strPcbBarcodeData);
}


void CDialogLotDisplay::OnStnClickedMsgDialogLotDisplayScrapData()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	/*CDialog_KeyPad dlgKeyPad;
	CString	strTemp;

	dlgKeyPad.m_nKeypadMode			= 0;
	dlgKeyPad.m_strKeypadLowLimit	= _T("0");
	dlgKeyPad.m_strKeypadHighLimit	= _T("10000");

	strTemp.Format(_T("%d"), 422);
	dlgKeyPad.m_strKeypadVal = strTemp;

	if (dlgKeyPad.DoModal() == IDOK)
	{
		m_strScrapCodeData	= dlgKeyPad.m_strNewVal;
		m_msgScrapCodeData.SetWindowText(m_strScrapCodeData);
	}*/

	int nKey;

	nKey = 0;
	KeyPadI(0, 10000, &nKey);

	m_strScrapCodeData.Format(_T("%d"), nKey);
	m_msgScrapCodeData.SetWindowText(m_strScrapCodeData);
}

void CDialogLotDisplay::OnStnClickedMsgDialogLotDisplayArraySnData()
{
	/*CDialog_KeyBoard dlgKeyBoard;

	dlgKeyBoard.m_strKeyBoardVal = m_strArraySnData;

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		m_strArraySnData = dlgKeyBoard.m_strKeyBoardVal;
		m_msgArraySnData.SetWindowText(m_strArraySnData);
	}*/
	CString strKey;

	strKey = m_strArraySnData;
	KeyBoard(&strKey);

	m_strArraySnData = strKey;
	m_msgArraySnData.SetWindowTextW(m_strArraySnData);
}

void CDialogLotDisplay::OnScarpCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	int nRow, nCol;

	CString strTmp;
	CString strMsg;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	if (nRow < 1) return;

	m_strScrapCodeData	= m_pGridPcbInfo.GetItemText(nRow, 1);
	m_strPcbBarcodeData = m_pGridPcbInfo.GetItemText(nRow, 2);
	m_strArraySnData	= m_pGridPcbInfo.GetItemText(nRow, 3);

	m_msgPcbBarcodeData.SetWindowText(m_strPcbBarcodeData);
	m_msgScrapCodeData.SetWindowText(m_strScrapCodeData);
	m_msgArraySnData.SetWindowText(m_strArraySnData);
}


void CDialogLotDisplay::OnLotCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	int nRow, nCol;

	CString strTmp;
	CString strMsg;
	CString strKey;

	int nKey;
	//CDialog_KeyBoard dlgKeyBoard;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	// Header Click
	switch (nCol)
	{
		case 1:
			switch (nRow)
			{
				case 0:
					/*dlgKeyBoard.m_strKeyBoardVal = m_strPartNo;

					if (dlgKeyBoard.DoModal() == IDOK)
					{
						m_strPartNo = dlgKeyBoard.m_strKeyBoardVal;

						m_pGridLotInfo.SetItemText(nRow, nCol, m_strPartNo);
					}*/

					strKey = m_strPartNo;
					KeyBoard(&strKey);

					m_strPartNo = strKey;
					m_pGridLotInfo.SetItemText(nRow, nCol, m_strPartNo);
					break;

				case 1:
					/*dlgKeyBoard.m_strKeyBoardVal = m_strLotNo;

					if (dlgKeyBoard.DoModal() == IDOK)
					{
						m_strLotNo = dlgKeyBoard.m_strKeyBoardVal;

						m_pGridLotInfo.SetItemText(nRow, 1, m_strLotNo);
					}*/
					strKey = m_strLotNo;
					KeyBoard(&strKey);

					m_strLotNo = strKey;
					m_pGridLotInfo.SetItemText(nRow, 1, m_strLotNo);
					break;

				case 2:
					/*dlgKeyBoard.m_strKeyBoardVal = m_strLotType;

					if (dlgKeyBoard.DoModal() == IDOK)
					{
						m_strLotType = dlgKeyBoard.m_strKeyBoardVal;

						m_pGridLotInfo.SetItemText(nRow, nCol, m_strLotType);
					}*/
					strKey = m_strLotType;
					KeyBoard(&strKey);

					m_strLotType = strKey;
					m_pGridLotInfo.SetItemText(nRow, nCol, m_strLotType);
					break;

				case 3:
					/*dlgKeyBoard.m_strKeyBoardVal = m_strStep;

					if (dlgKeyBoard.DoModal() == IDOK)
					{
						m_strStep = dlgKeyBoard.m_strKeyBoardVal;

						m_pGridLotInfo.SetItemText(nRow, nCol, m_strStep);
					}*/
					strKey = m_strStep;
					KeyBoard(&strKey);

					m_strStep = strKey;
					m_pGridLotInfo.SetItemText(nRow, nCol, m_strStep);
					break;

				case 4:
					/*dlgKeyBoard.m_strKeyBoardVal = m_strLotState;

					if (dlgKeyBoard.DoModal() == IDOK)
					{
						m_strLotState = dlgKeyBoard.m_strKeyBoardVal;

						m_pGridLotInfo.SetItemText(nRow, nCol, m_strLotState);
					}*/
					strKey = m_strLotState;
					KeyBoard(&strKey);

					m_strLotState = strKey;
					m_pGridLotInfo.SetItemText(nRow, nCol, m_strLotState);
					break;

				case 5:
					/*dlgKeyBoard.m_strKeyBoardVal = m_strComplot;

					if (dlgKeyBoard.DoModal() == IDOK)
					{
						m_strComplot = dlgKeyBoard.m_strKeyBoardVal;

						m_pGridLotInfo.SetItemText(nRow, nCol, m_strComplot);
					}*/
					strKey = m_strComplot;
					KeyBoard(&strKey);

					m_strComplot = strKey;
					m_pGridLotInfo.SetItemText(nRow, nCol, m_strComplot);
					break;
			}
			break;

		case 2:
			switch (nRow)
			{
				case 0:
					/*dlgKeyBoard.m_strKeyBoardVal = m_strPartNo;

					if (dlgKeyBoard.DoModal() == IDOK)
					{
						m_strPartNo = dlgKeyBoard.m_strKeyBoardVal;

						m_pGridLotInfo.SetItemText(nRow, 1, m_strPartNo);
					}*/
					strKey = m_strPartNo;
					KeyBoard(&strKey);

					m_strPartNo = strKey;
					m_pGridLotInfo.SetItemText(nRow, 1, m_strPartNo);
					break;
			}
			break;

		case 3:
			switch (nRow)
			{
				case 0:
					if (st_handler_info.nMenuLock == TRUE) return;

					st_handler_info.nMenuLock = TRUE;

					m_nRetry		= 0;
					m_nLotStatusStep= 0;

					SetTimer(TM_STATUS, 100, NULL);
					break;

				case 1:
					/*strTmp.Format(_T("%d"), m_nLotQty);
					dlgKeyBoard.m_strKeyBoardVal = strTmp;

					if (dlgKeyBoard.DoModal() == IDOK)
					{
						strTmp = dlgKeyBoard.m_strKeyBoardVal;
						m_nLotQty	=	_wtoi(strTmp);

						m_pGridLotInfo.SetItemText(nRow, nCol, strTmp);
					}*/
					nKey = m_nLotQty;
					KeyPadI(1, 10000, &nKey);

					m_nLotQty = nKey;
					strTmp.Format(_T("%d"), m_nLotQty);
					m_pGridLotInfo.SetItemText(nRow, nCol, strTmp);
					break;

				case 2:
					/*strTmp.Format(_T("%d"), m_nScrapCount);
					dlgKeyBoard.m_strKeyBoardVal = strTmp;

					if (dlgKeyBoard.DoModal() == IDOK)
					{
						strTmp = dlgKeyBoard.m_strKeyBoardVal;
						m_nScrapCount = _wtoi(strTmp);

						m_pGridLotInfo.SetItemText(nRow, nCol, strTmp);
					}*/
					nKey = m_nScrapCount;
					KeyPadI(1, 10000, &nKey);

					m_nScrapCount = nKey;
					strTmp.Format(_T("%d"), m_nScrapCount);
					m_pGridLotInfo.SetItemText(nRow, nCol, strTmp);
					break;

				case 3:
					/*dlgKeyBoard.m_strKeyBoardVal = m_strProCid;

					if (dlgKeyBoard.DoModal() == IDOK)
					{
						m_strProCid = dlgKeyBoard.m_strKeyBoardVal;

						m_pGridLotInfo.SetItemText(nRow, nCol, m_strProCid);
					}*/
					strKey = m_strProCid;
					KeyBoard(&strKey);

					m_strProCid = strKey;
					m_pGridLotInfo.SetItemText(nRow, nCol, m_strProCid);
					break;

				case 4:
					/*dlgKeyBoard.m_strKeyBoardVal = m_strMainState;

					if (dlgKeyBoard.DoModal() == IDOK)
					{
						m_strMainState = dlgKeyBoard.m_strKeyBoardVal;

						m_pGridLotInfo.SetItemText(nRow, nCol, m_strMainState);
					}*/
					strKey = m_strMainState;
					KeyBoard(&strKey);

					m_strMainState = strKey;
					m_pGridLotInfo.SetItemText(nRow, nCol, m_strMainState);
					break;

				case 5:
					/*dlgKeyBoard.m_strKeyBoardVal = m_strLine;

					if (dlgKeyBoard.DoModal() == IDOK)
					{
						m_strLine = dlgKeyBoard.m_strKeyBoardVal;

						m_pGridLotInfo.SetItemText(nRow, nCol, m_strLine);
					}*/
					strKey = m_strLine;
					KeyBoard(&strKey);

					m_strLine = strKey;
					m_pGridLotInfo.SetItemText(nRow, nCol, m_strLine);
					break;
			}
			break;
	}

	m_pGridLotInfo.Invalidate(FALSE);
}

void CDialogLotDisplay::OnBnClickedBtnDialogLotDisplayScrap()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (st_handler_info.nMenuLock == TRUE) return;

	st_handler_info.nMenuLock = TRUE;

	m_nRetry		= 0;
	m_nScrapStep	= 0;

	m_btnScrapInform.EnableWindow(FALSE);

	SetTimer(TM_SCRAP, 100, NULL);
}


void CDialogLotDisplay::OnBnClickedBtnDialogLotDisplayOnline()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int i;
	if (st_handler_info.nMenuLock == TRUE) return;

	st_handler_info.nMenuLock = TRUE;

	// jtkim 20150309
	st_lot_display_info[1].nScrapCnt = 0;
	if (m_nScrapCount > 0)
	{
		st_lot_display_info[1].nScrapCnt = m_nScrapCount;
		for (i=0; i<m_nScrapCount; i++)
		{
			st_lot_display_info[1].strScrapSerial[i]	= m_ArScrapSerial.GetAt(i);
			st_lot_display_info[1].strScrapCode[i]		= m_ArScrapCode.GetAt(i);
			st_lot_display_info[1].strArray[i]			= m_ArArraySn.GetAt(i);
		}
	}

	m_nRetry		= 0;
	m_nOnLineStep	= 0;

	m_btnOnLineInform.EnableWindow(FALSE);

	SetTimer(TM_ONLINE, 100, NULL);
}


void CDialogLotDisplay::OnStnClickedMsgDialogLotDisplayPcbCntData()
{
	/*CDialog_KeyPad dlgKeyPad;
	CString	strTemp;

	dlgKeyPad.m_nKeypadMode			= 0;
	dlgKeyPad.m_strKeypadLowLimit	= _T("0");
	dlgKeyPad.m_strKeypadHighLimit	= _T("100");

	strTemp.Format(_T("%d"), 1);
	dlgKeyPad.m_strKeypadVal = strTemp;

	if (dlgKeyPad.DoModal() == IDOK)
	{
		strTemp	= dlgKeyPad.m_strNewVal;
		m_nAddScrap = _wtoi(strTemp);

		m_msgPcbCntData.SetWindowText(strTemp);
	}*/

	int nKey;

	CString strTemp;

	nKey = 0;
	KeyPadI(0, 100, &nKey);

	m_nAddScrap = nKey;

	strTemp.Format(_T("%d"), m_nAddScrap);
	m_msgPcbCntData.SetWindowText(strTemp);
}


BOOL CDialogLotDisplay::DestroyWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	KillTimer(TM_TRACK);
	KillTimer(TM_STATUS);
	st_handler_info.nMenuLock = FALSE;

	return CDialog::DestroyWindow();
}


void CDialogLotDisplay::OnBnClickedBtnDialogLotDisplayClear()
{
	m_btnOnLineInform.EnableWindow(TRUE);
	if (st_handler_info.nRunStatus != dSTOP) return;
 
 //	CDialog_Message dlgMsg;
 /*
	if (FAS_IO.get_in_bit(st_io_info.i_FrontSelectSwChk, IO_ON) == IO_OFF ||
		FAS_IO.get_in_bit(st_io_info.i_RearSelectSwChk, IO_ON)	== IO_OFF)
	{
 		FAS_IO.set_out_bit(st_io_info.o_DoorLock, IO_OFF);
	}
	else
	{

		dlgMsg.m_nMessageType	= 0;
		dlgMsg.m_strMessage		= _T("Manual Key Status Check");

		dlgMsg.DoModal();

	if (FAS_IO.get_in_bit(st_io_info.i_ManualModeChk, IO_OFF) == IO_ON)
	{
		st_other_info.strBoxMsg = _T("Please Check Manual Key Status.");
		st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
	}
*/
/*	CDialog_Message dlgMsg;

	if (FAS_IO.get_in_bit(st_io_info.i_FrontSelectSwChk, IO_ON) == IO_OFF ||
		FAS_IO.get_in_bit(st_io_info.i_RearSelectSwChk1, IO_ON)	== IO_OFF ||
		FAS_IO.get_in_bit(st_io_info.i_RearSelectSwChk2, IO_ON)	== IO_OFF)
	{
		FAS_IO.set_out_bit(st_io_info.o_DoorLock, IO_OFF);
	}
	else
	{
		dlgMsg.m_nMessageType	= 0;
		dlgMsg.m_strMessage		= _T("Please Check Manual Key Status.");

		dlgMsg.DoModal();
	}*/
}


void CDialogLotDisplay::OnDestroy()
{
	CDialog::OnDestroy();

	FAS_IO.set_out_bit(st_io_info.o_Buzz3OnOff, IO_OFF);

	KillTimer(TM_STATUS);
	st_handler_info.nMenuLock = FALSE;
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

int	CDialogLotDisplay::OnLotCancelReq()
{
	switch (m_nLotCancelStep)
	{
		case 0:
			if (st_basic_info.nModeInterface == EQP_ON_LINE)
			{
				m_nLotCancelStep = 100;
			}
			else
			{
				return RET_GOOD;
			}
			break;

		case 100:
			clsEcSecond.OnEcLotCancel(m_strLotNo, _T(""));

			m_nLotCancelStep = 200;
			break;

		case 200:
			if (clsEcSecond.m_nLotCancelFlag == BD_DATA_GOOD)
			{
				m_nLotCancelStep = 0;

				return RET_GOOD;
			}
			else if (clsEcSecond.m_nLotCancelFlag == BD_DATA_REJECT)
			{
				m_nLotCancelStep = 0;

				return RET_ERROR;
			}
			else if (clsEcSecond.m_nCommBin == BD_CONT_TIMEOUT)
			{
				m_nLotCancelStep = 0;

				return RET_ERROR;
			}
			else if (clsEcSecond.m_nCommBin == BD_SEND_TIMEOUT)
			{
				m_nLotCancelStep = 0;

				return RET_ERROR;
			}
			else if (clsEcSecond.m_nCommBin == BD_RCV_TIMEOUT)
			{
				m_nLotCancelStep = 0;

				return RET_ERROR;
			}
			break;
	}

	return RET_PROCEED;
}

// jtkim lot end
int CDialogLotDisplay::OnLotEnd()
{
	CString strTemp;
	switch (m_nLotEndStep)
	{
		case 0:
			if (st_basic_info.nModeRfid == YES)
			{
				m_nRetry = 0;
				m_nLotEndStep = 100;
			}
			else
			{
				m_nLotEndStep = 1000;
			}
			break;

		case 100:
/*
			if (clsRfid.OnMakeReadCodeAll(1))
			{
				m_dwWaitTime[0] = GetTickCount();

				m_nLotEndStep = 300;
			}
			else 
			{
				m_dwWaitTime[0] = GetTickCount();

				m_nLotEndStep = 200;
			}
*/
			m_dwWaitTime[0] = GetTickCount();
			::SendMessage(st_handler_info.hWnd, WM_RFID_MSG, RFID_READ, 1);

			m_nLotEndStep = 300;
			break;

		case 300:
			// 데이터 송신 결과 확인 작업
			if (clsRfid.m_nCommBin == BIN_GOODBIN)
			{
/*
				for (i=0; i<20; i++)
				{
					m_strRfidData[i] = clsRfid.m_strRfidData[i];
				}
*/
				// RFID Data 읽기
				RfidReadAllData(1, clsRfid.m_strRfidData);

				OnMessageDisplay(BIN_GOODBIN, _T("RFID Data Receive OK."));

				m_nLotEndStep = 1000;
			}
			else if (clsRfid.m_nCommBin == BIN_REJECTBIN)
			{
				OnMessageDisplay(BIN_REJECTBIN, _T("RFID Data Receive NG."));

				m_nLotEndStep = 0;

				return RET_ERROR;
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

				if (m_dwWaitTime[2] > (DWORD)5000)
				{
					m_nLotEndStep = 0;

					return RET_ERROR;
				}
			}
			break;

		case 1000:
			// 작업중인 모드체크
			if (st_basic_info.nModeInterface == EQP_ON_LINE)
			{
				// 작업모드가 on line 모드이면
				m_nLotEndStep = 1100;
			}
			else
			{
				// off line 모드이면 
				m_nLotEndStep = 2000;
			}
			break;

		case 1100:
			//clsEcSecond.OnEcLotEnd(st_lot_display_info[1].strLotNo, _T(""), _T(""), st_track_info.nCumPass, st_track_info.nCumReject);
			//2016.1012 이번주  LOTEND 안되게
			//clsEcSecond.m_nLotEndFlag = BD_DATA_GOOD;
			//2016.1018
			{
				int nSite, mSite,i;
				bool bBreak = false;
				if( st_lot_info[LOT_CURR].nLotStatus == LOT_CLEAR && st_lot_info[LOT_CURR].strLotNo == "")
				{
					if(st_lot_info[LOT_NEXT].nLotStatus >= LOT_START || st_lot_info[LOT_NEXT].strLotNo != "")
					{
					}
					else
					{
						for(  nSite = 0; nSite < TESTSITE_PARA; nSite++)
						{
							mSite = THD_TESTSITE_1 + nSite;
							for( i = 0; i <TEST_SOCKET_PARA; i++ )
							{
								if( st_test_site_info[mSite].st_pcb_info[i].nEnable == YES )
								{
									bBreak = YES;
									break;
								}
							}
							if(bBreak == YES)
							{//OnLotComplete(tagTEST_SITE_INFO &test_site)
								if( nSite == 0 )
									clsInterC1.OnLotComplete(st_test_site_info[mSite]);
								else if( nSite == 1 )
									clsInterC2.OnLotComplete(st_test_site_info[mSite]);
								else if( nSite == 2 )
									clsInterC3.OnLotComplete(st_test_site_info[mSite]);
								else if( nSite == 3 )
									clsInterC4.OnLotComplete(st_test_site_info[mSite]);
								else if( nSite == 4 )
									clsInterC5.OnLotComplete(st_test_site_info[mSite]);
								else if( nSite == 5 )
									clsInterC6.OnLotComplete(st_test_site_info[mSite]);
								else if( nSite == 6 )
									clsInterC7.OnLotComplete(st_test_site_info[mSite]);
								else if( nSite == 7 )
									clsInterC8.OnLotComplete(st_test_site_info[mSite]);

								bBreak = NO;
							}
						}
					}
				}
			}
// 			clsEcSecond.OnEcLotEnd(st_lot_display_info[1].strLotNo, _T(""), _T(""), st_track_info.nCumPass, st_track_info.tStart, st_track_info.nBdTime, st_track_info.nBdTimeCount, st_lot_display_info[1]);
// 			m_nLotEndStep = 1200;
			//2016.1205

			if (st_track_info.nCumPass + st_track_info.nCumReject == st_lot_display_info[1].nQty)
			{
				clsEcSecond.OnEcLotEnd(st_lot_display_info[1].strLotNo, _T(""), _T(""), st_track_info.nCumPass, st_track_info.tStart, st_track_info.nBdTime, st_track_info.nBdTimeCount, st_lot_display_info[1]);
				m_nLotEndStep = 1200;
			}
			else
			{
				OnMessageDisplay(BIN_REJECTBIN, _T("Lot End error. Lot num is different. Do lot cancle please"));
				m_nLotEndStep = 0;
			}
			break;

		case 1200:
			if ( clsEcSecond.m_nLotEndFlag == BD_DATA_GOOD)
			{
/*
				m_strRfidData[RF_EQP_ID]		= st_basic_info.strEqp;
				m_strRfidData[RF_LOT_ID_J]		= st_lot_display_info.strLotNo;
				m_strRfidData[RF_STEP]			= st_lot_display_info.strLotState;
				m_strRfidData[RF_LOT_TYPE]		= st_lot_display_info.strLotType;
				m_strRfidData[RF_PART_ID]		= st_lot_display_info.strPartNo;
				m_strRfidData[RF_QTY].Format(_T("%d"), st_lot_display_info.nQty - st_lot_display_info.nScrapCnt);
*/
				clsRfid.m_strRfidData[RF_EQP_ID]		= st_basic_info.strEqp;
				clsRfid.m_strRfidData[RF_LOT_ID_J]		= st_lot_display_info[1].strLotNo;
//				clsRfid.m_strRfidData[RF_STEP]			= st_lot_display_info.strLotState;
//				clsRfid.m_strRfidData[RF_LOT_TYPE]		= st_lot_display_info.strLotType;
				clsRfid.m_strRfidData[RF_PART_ID]		= st_lot_display_info[1].strPartNo;
				clsRfid.m_strRfidData[RF_QTY].Format(_T("%d"), st_lot_display_info[1].nQty - st_lot_display_info[1].nScrapCnt);

				OnMessageDisplay(BIN_GOODBIN, _T("Lot End Data Send OK"));
				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					strTemp.Format(_T("Lot End Data Send OK %s, CURR_nLotStatus = %d"), st_lot_display_info[1].strLotNo, st_lot_info[LOT_CURR].nLotStatus);
					clsMem.OnNormalMessageWrite(strTemp);
				}
				m_nLotEndStep = 2000;

			}
			else if (clsEcSecond.m_nLotEndFlag == BD_DATA_REJECT)
			{
				OnMessageDisplay(BIN_GOODBIN, _T("Lot End Data Send NG"));

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					strTemp.Format(_T("Lot End Data Send NG %s, NEXT_nLotStatus = %d"), st_lot_display_info[1].strLotNo, st_lot_info[LOT_NEXT].nLotStatus);
					clsMem.OnNormalMessageWrite(strTemp);
				}

				m_nLotEndStep = 1000;

				return RET_ERROR;
			}
			else
			{
				if (clsEcSecond.m_nCommBin == BD_CONT_TIMEOUT)
				{
					OnMessageDisplay(BIN_GOODBIN, _T("EC SERVER CONNECT NG"));
					if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
					{
						strTemp.Format(_T("EC SERVER CONNECT NG %s, NEXT_nLotStatus = %d"), st_lot_display_info[1].strLotNo, st_lot_info[LOT_NEXT].nLotStatus);
						clsMem.OnNormalMessageWrite(strTemp);
					}

					m_nLotEndStep = 1000;
					if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
					{
						strTemp = _T("EC SERVER CONNECT NG");
						clsMem.OnNormalMessageWrite(strTemp);
					}
					return RET_ERROR;
				}
			}
			break;

		case 2000:
			if (st_basic_info.nModeRfid == YES)
			{
				m_nRetry = 0;
				m_nLotEndStep = 2100;
			}
			else
			{
				//m_nLotEndStep = 3000;
				m_nLotEndStep = -1;

				return RET_GOOD;
			}
			break;

		case 2100:
/*
			if (clsRfid.OnMakeWriteCodeAll(1, m_strRfidData))
			{
				m_dwWaitTime[0] = GetTickCount();

				m_nLotEndStep = 2300;
			}
			else 
			{
				m_dwWaitTime[0] = GetTickCount();

				m_nLotEndStep = 2200;
			}
*/		
			// RFID DATA 쓰기
			m_dwWaitTime[0] = GetTickCount();
			::SendMessage(st_handler_info.hWnd, WM_RFID_MSG, RFID_WRITE, 1);

			m_nLotEndStep = 2300;
			break;

		case 2200:
			m_dwWaitTime[1] = GetTickCount();
			m_dwWaitTime[2] = m_dwWaitTime[1] - m_dwWaitTime[0];

			if (m_dwWaitTime[2] <= 0)
			{
				m_dwWaitTime[0] = GetTickCount();
				break;
			}

			if (m_dwWaitTime[2] > (DWORD)5000)
			{
				m_nRetry++;
				if (m_nRetry >= 3)
				{
					m_nLotEndStep = 2000;

					return RET_ERROR;
				}
				else
				{
					m_nLotEndStep = 2100;
				}
			}
			break;

		case 2300:
			// 데이터 송신 결과 확인 작업
			if (clsRfid.m_nCommBin == BIN_GOODBIN)
			{
				OnMessageDisplay(BIN_GOODBIN, _T("RFID DATA SEND OK"));

				m_nLotEndStep = 4000;
			}
			else if (clsRfid.m_nCommBin == BIN_REJECTBIN)
			{
				OnMessageDisplay(BIN_REJECTBIN, _T("RFID DATA SEND NG"));

				m_nLotEndStep = 2000;

				return RET_ERROR;
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

				if (m_dwWaitTime[2] > (DWORD)5000)
				{
					m_nLotEndStep = 2000;

					return RET_ERROR;
				}
			}
			break;

		case 3000:
			if (st_basic_info.nModeInterface == EQP_ON_LINE)
			{
				m_nLotEndStep = 3100;
			}
			else 
			{
				m_nLotEndStep = -1;

				return RET_GOOD;
			}
			break;

		case 3100:
			clsEcSecond.OnEqpMatch(st_lot_display_info[1].strLotNo, m_strRfidData[RF_CARRIER_ID]);

			m_nLotEndStep = 3200;
			break;

		case 3200:
			if (clsEcSecond.m_nMatchFlag == BD_DATA_GOOD)
			{
				OnMessageDisplay(BIN_GOODBIN, _T("Match 데이터 전송 완료"));

				m_nLotEndStep = 4000;
			}
			else if (clsEcSecond.m_nMatchFlag == BD_DATA_REJECT)
			{
				OnMessageDisplay(BIN_GOODBIN, _T("Match 데이터 전송 실패"));

				m_nLotEndStep = 3000;

				return RET_ERROR;
			}
			else
			{
				if (clsEcSecond.m_nCommBin == BD_CONT_TIMEOUT)
				{
					m_nLotEndStep = 3000;

					return RET_ERROR;
				}
			}
			break;

		case 4000:
			if (st_basic_info.nModeInterface == EQP_ON_LINE)
			{
				m_nLotEndStep = 4100;
			}
			else 
			{
				m_nLotEndStep = -1;

				return RET_GOOD;
			}
			break;

		case 4100:
			clsEcSecond.OnEqpFullCarrierMove(m_strRfidData[RF_CARRIER_ID]);

			m_nLotEndStep = 4200;
			break;

		case 4200:
			if (clsEcSecond.m_nFullCarrierFlag == BD_DATA_GOOD)
			{
				OnMessageDisplay(BIN_GOODBIN, _T("Full Carrier Move 데이터 전송 완료"));

				m_nLotEndStep = -1;

				return RET_GOOD;
			}
			else if (clsEcSecond.m_nFullCarrierFlag == BD_DATA_REJECT)
			{
				OnMessageDisplay(BIN_GOODBIN, _T("Full Carrier Move 데이터 전송 실패"));

				m_nLotEndStep = -1;

				return RET_GOOD;
			}
			else
			{
				if (clsEcSecond.m_nCommBin == BD_CONT_TIMEOUT)
				{
					m_nLotEndStep = 4000;

					return RET_ERROR;
				}
			}
			break;
	}
	return RET_PROCEED;
}

void CDialogLotDisplay::OnMessageDisplay(int nBin, CString strMsg)
{
	if (nBin == BIN_GOODBIN)
	{
		m_pGridLotInfo.SetItemBkColour(6, 1, GREEN_C, BLACK_C);
	}
	else
	{
		m_pGridLotInfo.SetItemBkColour(6, 1, RED_C, BLACK_C);
	}
	m_pGridLotInfo.SetItemText(6, 1, strMsg);

	m_pGridLotInfo.Invalidate(FALSE);
}

void CDialogLotDisplay::OnBnClickedBtnDialogLotDisplayEnd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (st_handler_info.nMenuLock == TRUE) return;

	if (m_strLotNo == _T("") && 0) return;

	st_handler_info.nMenuLock = TRUE;

	m_nRetry		= 0;
	m_nTrackOutStep = 0;
	if (m_nLotEndStep == -1) m_nLotEndStep = 0;

	m_btnLotEnd.EnableWindow(FALSE);

	SetTimer(TM_TRACK, 100, NULL);
}


void CDialogLotDisplay::OnBnClickedBtnDialogLotDisplayCancel1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDialogLotDisplay::OnBnClickedBtnDialogLotDisplayCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (st_handler_info.nMenuLock == TRUE) return;

	st_handler_info.nMenuLock = TRUE;

	m_nRetry		= 0;
	m_nLotCancelStep	= 0;

	m_btnLotCancel.EnableWindow(FALSE);

	SetTimer(TM_CANCEL, 100, NULL);
}


void CDialogLotDisplay::OnBnClickedBtnDialogLotDataClear()
{
	//kwlee 2016.1221 Lot_Data_Clear
	CDialog_Message dlgMsg;
	CString strMsg;
	int i, nFlag = 0;

	if (st_handler_info.nRunStatus == dRUN) return;

	dlgMsg.m_nMessageType = 1;
	dlgMsg.m_strMessage.Format(_T("NEXT LOT Data를 삭제 하시겠습니까?"));


	for (i =0; i<4; i++)
	{
		if ((st_buffer_info[THD_LD_BUFF].st_pcb_info[i].strLotNo == st_lot_info[LOT_NEXT].strLotNo && st_lot_info[LOT_NEXT].strLotNo !=_T(""))||
			(st_buffer_info[THD_ULD_BUFF].st_pcb_info[i].strLotNo == st_lot_info[LOT_NEXT].strLotNo && st_lot_info[LOT_NEXT].strLotNo !=_T(""))||
			(st_picker[THD_TEST_RBT].st_pcb_info[i].strLotNo == st_lot_info[LOT_NEXT].strLotNo && st_lot_info[LOT_NEXT].strLotNo !=_T(""))||
			(st_picker[THD_WORK_RBT].st_pcb_info[i].strLotNo == st_lot_info[LOT_NEXT].strLotNo && st_lot_info[LOT_NEXT].strLotNo !=_T("")) ||
			(st_tray_info[THD_LD_TRAY_PLATE].strLotNo == st_lot_info[LOT_NEXT].strLotNo && st_lot_info[LOT_NEXT].strLotNo !=_T("")))
		{
			nFlag++;
		}
	}

	if (nFlag == 0)
	{
		if (dlgMsg.DoModal() == IDOK)
		{
			st_lot_info[LOT_NEXT].strLotNo = "";
			st_lot_info[LOT_NEXT].strPartNo = "";
			st_lot_info[LOT_NEXT].strProcess = "";
			st_lot_info[LOT_NEXT].strProPerty = "";
			st_lot_info[LOT_NEXT].strBypass = "";

			st_lot_info[LOT_NEXT].nLotNoApply_FirstTestStart = LOT_CLEAR; //james 2016.0926 

			st_lot_info[LOT_NEXT].nLotStatus = LOT_CLEAR;
			//james 2016.0924 for( i= 0; i < THREAD_MAX_SITE; i++) st_lot_info[LOT_CURR].nLot_THD_Status[i] = LOT_CLEAR;  	 
			st_lot_info[LOT_NEXT].nNewLotIn = 0;
			st_lot_info[LOT_NEXT].nLastModule = 0;
			st_lot_info[LOT_NEXT].nTotalCnt = 0;
			st_lot_info[LOT_NEXT].nLane = 0; 
			st_lot_info[LOT_NEXT].nLdTrayCnt = 0;
			st_lot_info[LOT_NEXT].nUnLdTrayCnt = 0;
			for(i=0; i<3; i++) st_lot_info[LOT_NEXT].nBinSortCnt[i] = 0;
			st_lot_info[LOT_NEXT].nTrayRunMode_StandAlone = 0; 

			//james 2016.0921 
			st_lot_info[LOT_NEXT].tLotStart		= 0;

			st_lot_info[LOT_NEXT].nAbortTime = 0;
			st_lot_info[LOT_NEXT].nAbortTotal = 0;
			st_lot_info[LOT_NEXT].nAbortCount = 0;
			st_lot_info[LOT_NEXT].nAbortUsed = 0;	 

			st_lot_info[LOT_NEXT].nStacker_Good_THD_Num = 0;
			st_lot_info[LOT_NEXT].nStacker_Reject_THD_Num = 0;
			st_lot_info[LOT_NEXT].nBuffer_Retest_THD_Num = 0;

			st_lot_info[LOT_NEXT].nTestMode = 0;// ONESHOT, HOT, COLD, GATE......
			st_lot_info[LOT_NEXT].nInterface = 0;// ftp / local......
			st_lot_info[LOT_NEXT].nRetry = 0;// retry count.....
			st_lot_info[LOT_NEXT].nBarcodeMode = 0;// barcode 전송선택 (1개/2개)
			st_lot_info[LOT_NEXT].nQty = 0;// lot 수량.....
			st_lot_info[LOT_NEXT].nModuleCount = 0;// pgm download 완료......
			st_lot_info[LOT_NEXT].nPgmDown = 0;
			st_lot_info[LOT_NEXT].nChamberDoor = 0;// chanber door open....
		}
	}
	else
	{
		AfxMessageBox(_T("NEXT LOT이 투입 되었습니다. NEXT LOT 정보를 CLEAR 할 수 없습니다."));
	}
}
