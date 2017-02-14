// DialogLotStart.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "DialogLotStart.h"
#include "afxdialogex.h"
#include "Variable.h"
#include "PublicFunction.h"
#include "ClientEcFirst.h"
#include "ClientRfid.h"
#include "Interface.h"
#include "KeyBoard.h"
#include "MyBasicData.h"
#include "CtlBd_Library.h"
#include "AlgMemory.h"
#include "LogFromat.h"
// CDialogLotStart 대화 상자입니다.
#define TM_DISPLAY		100
#define TM_START		200
#define TM_CANCEL		300
#define TM_RFID			400

IMPLEMENT_DYNAMIC(CDialogLotStart, CDialog)

CDialogLotStart::CDialogLotStart(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogLotStart::IDD, pParent)
{
	m_bmGround.LoadBitmap(IDB_BITMAP_BACK_GROUND); 
	m_nLotDisplayStep = 0;
}

CDialogLotStart::~CDialogLotStart()
{
}

void CDialogLotStart::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CUSTOM_LOT_INFO,						m_pGridLotInfo);
	DDX_Control(pDX, IDC_BTN_EXIT,								m_btnExit);
	DDX_Control(pDX, IDC_BTN_LOT_START,							m_btnLotStart);
	DDX_Control(pDX, IDC_BTN_LOT_CANCEL,						m_btnLotCancel);
	DDX_Control(pDX, IDC_BTN_RFID_READ,							m_btnRfidRead);
	DDX_Control(pDX, IDC_BTN_LOT_DISPLAY1,						m_btnLotDisplay);
	DDX_Control(pDX, IDC_BTN_CURR_LOT,							m_btnCurrLot);
	DDX_Control(pDX, IDC_BTN_NEXT_LOT,							m_btnNextLot);
}


BEGIN_MESSAGE_MAP(CDialogLotStart, CDialog)
	ON_BN_CLICKED(IDC_BTN_EXIT, &CDialogLotStart::OnBnClickedBtnExit)
	ON_BN_CLICKED(IDC_BTN_LOT_CANCEL, &CDialogLotStart::OnBnClickedBtnLotCancel)
	ON_BN_CLICKED(IDC_BTN_LOT_START, &CDialogLotStart::OnBnClickedBtnLotStart)
	ON_BN_CLICKED(IDC_BTN_RFID_READ, &CDialogLotStart::OnBnClickedBtnRfidRead)
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_LOT_DISPLAY1, &CDialogLotStart::OnBnClickedBtnLotDisplay1)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_LOT_INFO, OnLotCellClick)
	ON_BN_CLICKED(IDC_BTN_CURR_LOT, &CDialogLotStart::OnBnClickedBtnCurrLot)
	ON_BN_CLICKED(IDC_BTN_NEXT_LOT, &CDialogLotStart::OnBnClickedBtnNextLot)
END_MESSAGE_MAP()


// CDialogLotStart 메시지 처리기입니다.


BOOL CDialogLotStart::Create()
{
	return CDialog::Create( CDialogLotStart::IDD );
}


BOOL CDialogLotStart::DestroyWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	KillTimer(TM_DISPLAY);	
	KillTimer(TM_START);
	KillTimer(TM_CANCEL);
	KillTimer(TM_RFID);		

	if (st_handler_info.nMenuLock) st_handler_info.nMenuLock = FALSE;

	return CDialog::DestroyWindow();
}


BOOL CDialogLotStart::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	OnInitGridLotInfo();
	OnInitButton();

	if (st_basic_info.nModeRfid == YES)
	{
		//st_handler_info.nMenuLock = TRUE;

		SetTimer(TM_RFID, 100, NULL);
	}

	st_work_info.nLdCvySite					= m_nLdCvySite;
	st_tray_info[m_nLdCvySite].nTestMode	= 0;
	st_tray_info[m_nLdCvySite].nInterface	= 0;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDialogLotStart::OnInitGridLotInfo()
{
	int i, j;
	int nMaxRow, nMaxCol;
	int nRow;

	CString strTemp;

	//kwlee 2017.0105
	//nMaxRow = 10;
	nMaxRow = 11;
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
		//kwlee 2017.0105
		//m_pGridLotInfo.SetRowHeight(i, 45);
		m_pGridLotInfo.SetRowHeight(i, 41);

		for (j=0; j<nMaxCol; j++)
		{
			m_pGridLotInfo.SetColumnWidth(j, 128);
			
			m_pGridLotInfo.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridLotInfo.SetItemFont(i, j, &clsFunc.OnLogFont(14));
			m_pGridLotInfo.SetItemState(i, j, GVIS_READONLY);
		}
	}

	nRow = 0;
	m_pGridLotInfo.MergeCells(nRow, 0, nRow, 1);
	m_pGridLotInfo.SetItemBkColour(nRow, 0, BLACK_L, WHITE_C);
	m_pGridLotInfo.SetItemFont(nRow, 0, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 0, _T("Lot No."));

	m_pGridLotInfo.MergeCells(nRow, 2, nRow, 3);
	m_pGridLotInfo.SetItemBkColour(nRow, 2, WHITE_C, BLACK_L);
	m_pGridLotInfo.SetItemFont(nRow, 2, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 2, _T(""));

	nRow = 1;
	m_pGridLotInfo.MergeCells(nRow, 0, nRow, 1);
	m_pGridLotInfo.SetItemBkColour(nRow, 0, BLACK_L, WHITE_C);
	m_pGridLotInfo.SetItemFont(nRow, 0, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 0, _T("Part No."));

	m_pGridLotInfo.MergeCells(nRow, 2, nRow, 3);
	m_pGridLotInfo.SetItemBkColour(nRow, 2, WHITE_C, BLACK_L);
	m_pGridLotInfo.SetItemFont(nRow, 2, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 2, _T("-"));

	nRow = 2;
	m_pGridLotInfo.MergeCells(nRow, 0, nRow, 3);
	m_pGridLotInfo.SetItemBkColour(nRow, 0, BLACK_L, WHITE_C);
	m_pGridLotInfo.SetItemFont(nRow, 0, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 0, _T("Mode"));

	nRow = 3;
	m_pGridLotInfo.SetItemBkColour(nRow, 0, GREEN_C, BLACK_C);
	m_pGridLotInfo.SetItemFont(nRow, 0, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 0, _T("ONE SHOT"));

	m_pGridLotInfo.SetItemBkColour(nRow, 1, GREEN_D, BLACK_C);
	m_pGridLotInfo.SetItemFont(nRow, 1, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 1, _T("HOT"));

	m_pGridLotInfo.SetItemBkColour(nRow, 2, GREEN_D, BLACK_C);
	m_pGridLotInfo.SetItemFont(nRow, 2, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 2, _T("COLD"));

	m_pGridLotInfo.SetItemBkColour(nRow, 3, GREEN_D, BLACK_C);
	m_pGridLotInfo.SetItemFont(nRow, 3, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 3, _T("GATE"));

	nRow = 4;
	m_pGridLotInfo.MergeCells(nRow, 0, nRow, 3);
	m_pGridLotInfo.SetItemBkColour(nRow, 0, BLACK_L, WHITE_C);
	m_pGridLotInfo.SetItemFont(nRow, 0, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 0, _T("Mode"));

	nRow = 5;
	m_pGridLotInfo.MergeCells(nRow, 0, nRow, 1);
	m_pGridLotInfo.SetItemBkColour(nRow, 0, GREEN_C, BLACK_C);
	m_pGridLotInfo.SetItemFont(nRow, 0, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 0, _T("FTP MODE"));

	m_pGridLotInfo.MergeCells(nRow, 2, nRow, 3);
	m_pGridLotInfo.SetItemBkColour(nRow, 2, GREEN_D, BLACK_C);
	m_pGridLotInfo.SetItemFont(nRow, 2, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 2, _T("LOCAL MODE"));

	nRow = 6;
	m_pGridLotInfo.MergeCells(nRow, 0, nRow, 1);
	m_pGridLotInfo.SetItemBkColour(nRow, 0, BLACK_L, WHITE_C);
	m_pGridLotInfo.SetItemFont(nRow, 0, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 0, _T("OPT CODE"));

	m_pGridLotInfo.MergeCells(nRow, 2, nRow, 3);
	m_pGridLotInfo.SetItemBkColour(nRow, 2, WHITE_C, BLACK_L);
	m_pGridLotInfo.SetItemFont(nRow, 2, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 2, _T("-"));

	nRow = 7;
	m_pGridLotInfo.MergeCells(nRow, 0, nRow, 1);
	m_pGridLotInfo.SetItemBkColour(nRow, 0, BLACK_L, WHITE_C);
	m_pGridLotInfo.SetItemFont(nRow, 0, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 0, _T("RFID Yes/No"));

	m_pGridLotInfo.MergeCells(nRow, 2, nRow, 3);
	m_pGridLotInfo.SetItemFont(nRow, 2, &clsFunc.OnLogFont(20));
	if (st_basic_info.nModeRfid == NO)
	{
		m_pGridLotInfo.SetItemBkColour(nRow, 2, YELLOW_D, BLACK_C);
		m_pGridLotInfo.SetItemText(nRow, 2, _T("NO"));
	}
	else
	{
		m_pGridLotInfo.SetItemBkColour(nRow, 2, YELLOW_C, BLACK_C);
		m_pGridLotInfo.SetItemText(nRow, 2, _T("YES"));
	}

	nRow = 8;
	m_pGridLotInfo.MergeCells(nRow, 0, nRow, 1);
	m_pGridLotInfo.SetItemBkColour(nRow, 0, BLACK_L, WHITE_C);
	m_pGridLotInfo.SetItemFont(nRow, 0, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 0, _T("CARRIER ID"));

	m_pGridLotInfo.MergeCells(nRow, 2, nRow, 3);
	m_pGridLotInfo.SetItemBkColour(nRow, 2, WHITE_C, BLACK_L);
	m_pGridLotInfo.SetItemFont(nRow, 2, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 2, _T("-"));

	nRow = 9;
	m_pGridLotInfo.MergeCells(nRow, 0, nRow, 1);
	m_pGridLotInfo.SetItemBkColour(nRow, 0, BLACK_L, WHITE_C);
	m_pGridLotInfo.SetItemFont(nRow, 0, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 0, _T("QTY"));

	m_pGridLotInfo.MergeCells(nRow, 2, nRow, 3);
	m_pGridLotInfo.SetItemBkColour(nRow, 2, WHITE_C, BLACK_L);
	m_pGridLotInfo.SetItemFont(nRow, 2, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 2, _T("-"));

	//kwlee 2017.0105
	nRow = 10;
	m_pGridLotInfo.MergeCells(nRow, 0, nRow, 1);
	m_pGridLotInfo.SetItemBkColour(nRow, 0, BLACK_L, WHITE_C);
	m_pGridLotInfo.SetItemFont(nRow, 0, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 0, _T("FAB"));

	m_pGridLotInfo.MergeCells(nRow, 2, nRow, 3);
	m_pGridLotInfo.SetItemBkColour(nRow, 2, WHITE_C, BLACK_L);
	m_pGridLotInfo.SetItemFont(nRow, 2, &clsFunc.OnLogFont(20));
	m_pGridLotInfo.SetItemText(nRow, 2, _T("-"));
	//

	m_pGridLotInfo.Invalidate(FALSE);
}


void CDialogLotStart::OnInitButton()
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

	m_btnLotStart.SetBitmaps(IDC_BTN_LOT_START, IDB_NETWORK_ON_DN, WINDOW_DN, IDB_NETWORK_ON_UP, WINDOW_UP);
	m_btnLotStart.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnLotStart.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnLotStart.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnLotStart.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnLotStart.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnLotStart.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnLotStart.SetFont(clsFunc.m_pFont[3]);
	m_btnLotStart.EnableWindow(FALSE);
	m_btnLotStart.SetTooltipText(_T("Lot Start"));

	m_btnLotCancel.SetBitmaps(IDC_BTN_LOT_CANCEL, IDB_NETWORK_ON_DN, WINDOW_DN, IDB_NETWORK_ON_UP, WINDOW_UP);
	m_btnLotCancel.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnLotCancel.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnLotCancel.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnLotCancel.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnLotCancel.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnLotCancel.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnLotCancel.SetFont(clsFunc.m_pFont[3]);
	m_btnLotCancel.EnableWindow(FALSE);
	m_btnLotCancel.SetTooltipText(_T("Lot Cancel"));

	m_btnRfidRead.SetBitmaps(IDC_BTN_RFID_READ, IDB_BITMAP_GEAR_DN, WINDOW_DN, IDB_BITMAP_GEAR_UP, WINDOW_UP);
	m_btnRfidRead.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnRfidRead.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnRfidRead.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnRfidRead.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnRfidRead.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnRfidRead.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnRfidRead.SetFont(clsFunc.m_pFont[3]);
	m_btnRfidRead.SetTooltipText(_T("RFID Read"));

	m_btnLotDisplay.SetBitmaps(IDC_BTN_LOT_DISPLAY, IDB_BITMAP_GEAR_DN, WINDOW_DN, IDB_BITMAP_GEAR_UP, WINDOW_UP);
	m_btnLotDisplay.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnLotDisplay.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnLotDisplay.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnLotDisplay.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnLotDisplay.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnLotDisplay.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnLotDisplay.SetFont(clsFunc.m_pFont[3]);
	m_btnLotDisplay.SetTooltipText(_T("Lot Display"));

	m_btnCurrLot.SetBitmaps(IDC_BTN_CURR_LOT, IDB_NETWORK_ON_DN, WINDOW_DN, IDB_NETWORK_ON_UP, WINDOW_UP);
	m_btnCurrLot.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnCurrLot.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnCurrLot.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnCurrLot.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnCurrLot.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnCurrLot.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnCurrLot.SetFont(clsFunc.m_pFont[3]);
	m_btnCurrLot.SetTooltipText(_T("Curr Lot Display"));
	m_btnCurrLot.EnableWindow(FALSE);
	if (st_lot_info[LOT_CURR].nLotStatus >= LOT_START)
	{
		m_btnCurrLot.EnableWindow(TRUE);
	}

	m_btnNextLot.SetBitmaps(IDC_BTN_CURR_LOT, IDB_NETWORK_ON_DN, WINDOW_DN, IDB_NETWORK_ON_UP, WINDOW_UP);
	m_btnNextLot.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnNextLot.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnNextLot.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnNextLot.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnNextLot.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnNextLot.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnNextLot.SetFont(clsFunc.m_pFont[3]);
	m_btnNextLot.SetTooltipText(_T("Next Lot Display"));
	m_btnNextLot.EnableWindow(FALSE);
	if (st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)
	{
		m_btnNextLot.EnableWindow(TRUE);
	}
}

void CDialogLotStart::OnBnClickedBtnExit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strMsg;
	if( st_handler_info.cWndList != NULL )
	{
		strMsg = _T("Lot start dialog btnExit ");
		clsMem.OnNormalMessageWrite(strMsg);
	}

	if (st_handler_info.cWndMain != NULL)
	{
		st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_LOT_START_DELETE_REQ, 0);
	}
}


void CDialogLotStart::OnBnClickedBtnLotCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (st_handler_info.nMenuLock) return;
	if (m_strLotNo == _T("")) return;

	st_handler_info.nMenuLock = TRUE;
	m_nLotCancelStep = 0;

	SetTimer(TM_CANCEL, 100 ,NULL);
}


void CDialogLotStart::OnBnClickedBtnLotStart()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (st_handler_info.nMenuLock) return;
	if (m_strLotNo == _T(""))
	{
		st_other_info.strBoxMsg = _T("Lot No 입력되지 않았습니다. 확인해 주세요.");
		if (st_handler_info.cWndMain != NULL)
		{
			st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
		}

		clsFunc.OnMCStop();
		return;
	}
	if (m_strOpCode == _T(""))
	{
		st_other_info.strBoxMsg = _T("OP CODE 입력되지 않았습니다. 확인해 주세요.");
		if (st_handler_info.cWndMain != NULL)
		{
			st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
		}

		clsFunc.OnMCStop();

		return;
	}

	if (st_tray_info[m_nLdCvySite].nQty <= 0)
	{
		st_other_info.strBoxMsg = _T("수량이 입력되지 않았습니다. 확인해 주세요.");
		if (st_handler_info.cWndMain != NULL)
		{
			st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
		}

		clsFunc.OnMCStop();

		return;
	}
	// jtkim 20160929
	st_tray_info[m_nLdCvySite].strOptCode = m_strOpCode;
	//2017.0105
	st_lot_info[m_nLdCvySite].strFabSite	=  m_strFabSite;

	if(st_lot_info[LOT_CURR].nLotStatus >= LOT_START && st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)
	{
		if (st_lot_info[LOT_CURR].strLotNo != m_strLotNo && st_lot_info[LOT_NEXT].strLotNo != m_strLotNo)
		{
			st_other_info.strBoxMsg = _T("CURR LOT / NEXT LOT 진행중입니다. LOT 종료 후 실행해 주세요.");
			if (st_handler_info.cWndMain != NULL)
			{
				st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
			}

			//clsFunc.OnMCStop();

			return;
		}
	}

	if(st_lot_info[LOT_CURR].nLotStatus >= LOT_START)
	{
		if (st_lot_info[LOT_CURR].strLotNo == m_strLotNo)
		{
			st_tray_info[m_nLdCvySite].nTestMode	= 0;
			st_tray_info[m_nLdCvySite].nInterface	= 0;

			st_lot_info[LOT_CURR].nQty = st_lot_display_info[0].nQty; //james 2016.0929 

			st_tray_info[m_nLdCvySite].nQty			= st_lot_display_info[0].nQty;
			st_tray_info[m_nLdCvySite].strOptCode	= st_lot_display_info[0].strOptcode;
			// lot start 성공
			st_tray_info[m_nLdCvySite].strLotNo		= st_lot_info[LOT_CURR].strLotNo;
			st_tray_info[m_nLdCvySite].strPartNo	= st_lot_info[LOT_CURR].strPartNo;

			//2017.0105
			st_lot_info[m_nLdCvySite].strFabSite	=  st_lot_info[LOT_CURR].strFabSite;

			st_tray_info[m_nLdCvySite].nNewLotIn	= YES;
			if (st_handler_info.cWndMain != NULL)
			{
				st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_LOT_START_DELETE_REQ, 0);
			}
			return;
		}
	}

	if(st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)
	{
		if (st_lot_info[LOT_NEXT].strLotNo == m_strLotNo)
		{
			st_tray_info[m_nLdCvySite].nTestMode	= 0;
			st_tray_info[m_nLdCvySite].nInterface	= 0;

			st_lot_info[LOT_NEXT].nQty = st_lot_display_info[0].nQty; //james 2016.0929 

			st_tray_info[m_nLdCvySite].nQty			= st_lot_display_info[0].nQty;
			st_tray_info[m_nLdCvySite].strOptCode	= st_lot_display_info[0].strOptcode;
			// lot start 성공
			st_tray_info[m_nLdCvySite].strLotNo		= st_lot_info[LOT_NEXT].strLotNo;
			st_tray_info[m_nLdCvySite].strPartNo	= st_lot_info[LOT_NEXT].strPartNo;

			st_tray_info[m_nLdCvySite].nNewLotIn	= YES;

			if (st_handler_info.cWndMain != NULL)
			{
				st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_LOT_START_DELETE_REQ, 0);
			}
			return;
		}
	}


	//st_handler_info.mn_removetray = NO; //kwlee 2017.0117
	m_nLotStartStep = 0;
	st_handler_info.nMenuLock = TRUE;
	SetTimer(TM_START, 100, NULL);
}


void CDialogLotStart::OnBnClickedBtnRfidRead()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (st_handler_info.nMenuLock) return;
	m_nRfidReadStep = 0;
	st_handler_info.nMenuLock = TRUE;

	SetTimer(TM_RFID, 100, NULL);
}


BOOL CDialogLotStart::OnEraseBkgnd(CDC* pDC)
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


int	CDialogLotStart::OnLotDisplay()
{
	int nRet = 0;
	//char	mc_jamcode[10];
	bool bMdl = false;
	CString strDeviceName = _T(""), strRecipe = _T("");
	CString strMsg;

	switch (m_nLotDisplayStep)
	{
		case 0:
			if (st_basic_info.nModeInterface == EQP_ON_LINE)
			{
				m_nLotDisplayStep = 100;
			}
			else
			{
				return RET_GOOD;
			}
			break;

		//2016.1116
		case 10:
			nRet = clsFunc.GetModelFromPartID(m_strPartNo,m_strTmpLotNo);
			if( nRet != RET_GOOD)
			{//940005 0 A "Not Registered model."
// 				CTL_Lib.Alarm_Error_Occurrence(21001, dWARNING, _T("940005"));
// 
// 				st_other_info.strBoxMsg.Format( _T("[PartID Error] %s is not correct. Not registered model. check please."), (LPCTSTR) m_strPartNo );
// 				if (st_handler_info.cWndMain != NULL)
// 				{
// 					st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
// 				}
// 				break;
			}
			m_nLotDisplayStep = 20;
			break;

		case 20:
			if(st_work_info.m_strCurModel != m_strTmpLotNo)
			{
				m_strTmpLotNo = st_basic_info.strDeviceName + _T(".TXT");
				st_work_info.m_strCurModel = m_strTmpLotNo;
				clsFunc.LoadRegModelData();
				m_nLotDisplayStep = 30;
			}
			else
			{
				m_nLotDisplayStep = 100;
			}
			break;

		case 30:
			for ( int i = 0; i < st_work_info.m_nMdlTotal; i++)
			{
				if( st_work_info.m_strModelName[i] == st_work_info.m_strCurModel/*m_strModelNewname*/)
				{
					bMdl = true;
					break;
				}
			}
			if(bMdl == false)
			{
				//There is no model
				m_nLotDisplayStep = 10;
				break;
			}
			m_nLotDisplayStep = 40;
			break;

		case 40:
			//st_basic_info.strDeviceName = /*m_strModelNewname*/st_work_info..m_strCurModel + _T(".TXT");
			strDeviceName = st_work_info.m_strCurModel + _T(".TXT");
			strRecipe = OnRecipeExistCheck(strDeviceName);
			if(strRecipe != _T("") )
			{
				if (st_handler_info.cWndMain != NULL)
				{
					st_other_info.strBoxMsg = _T("Recipe Check Error. \r\n Plase Change The Recipe");
					st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
				}
				clsFunc.RunRegPartID();
				m_nLotDisplayStep = 50;
			}
			else//recipe change
			{
				st_basic_info.strDeviceName = strRecipe;
				clsBasic.OnBasic_Data_Load(1);
				clsBasic.OnModuleRobot_Teach_Data_Load(st_basic_info.strDeviceName);
				clsBasic.OnInterface_Data_Load();
				clsBasic.OnRecipe_Data_Load(st_basic_info.strDeviceName);
				m_nLotDisplayStep = 1000;
			}
			break;

		case 50:
				clsFunc.SendRegPartID();
				// 600411 0 A "Can't find Recipe file."
				CTL_Lib.Alarm_Error_Occurrence(9201, dWARNING, _T("600411") );
				if( st_handler_info.cWndList != NULL )
				{
					strDeviceName = st_work_info.m_strCurModel + _T(".TXT");
					strMsg.Format( _T(" Cannot find model file => %s") , strDeviceName);
					clsMem.OnNormalMessageWrite(strMsg);
				}
				m_nLotDisplayStep = 10;
				break;

		case 100:
			clsEcFirst.OnEcLotDisplay(m_strLotNo, _T(""));

			m_nLotDisplayStep = 200;
			break;

		case 200:
			if (clsEcFirst.m_nLotDisplayFlag == BD_DATA_GOOD)
			{
				//2016.1116
				st_work_info.m_strCurModel = m_strLotNo;
				m_nLotDisplayStep = 0;

				return RET_GOOD;
			}
			else if (clsEcFirst.m_nLotDisplayFlag == BD_DATA_REJECT)
			{
				m_nLotDisplayStep = 0;

				return RET_ERROR;
			}
			else if (clsEcFirst.m_nCommBin == BD_CONT_TIMEOUT)
			{
				m_nLotDisplayStep = 0;

				return RET_ERROR;
			}
			else if (clsEcFirst.m_nCommBin == BD_SEND_TIMEOUT)
			{
				m_nLotDisplayStep = 0;

				return RET_ERROR;
			}
			else if (clsEcFirst.m_nCommBin == BD_RCV_TIMEOUT)
			{
				m_nLotDisplayStep = 0;

				return RET_ERROR;
			}
			break;
	}
	return RET_PROCEED;
}


int	CDialogLotStart::OnLotStart()
{
	CString strLogKey[10];
	CString strLogData[10];

	switch (m_nLotStartStep)
	{
		case 0:
			if (st_basic_info.nModeInterface == EQP_ON_LINE)
			{
				m_nLotStartStep = 100;
			}
			else
			{
				return RET_GOOD;
			}
			break;

		case 100:
			clsEcFirst.OnEcLotStart(m_strLotNo, _T(""), 10);

			m_nLotStartStep = 200;
			break;

		case 200:
			if (clsEcFirst.m_nLotStartFlag == BD_DATA_GOOD)
			{
				clsLog.LogLotInform(_T("INTERFACE"), _T("LOTIN"), st_lot_display_info[0].strLotNo, st_basic_info.strDeviceName, _T(""), 0, strLogKey, strLogData);
				m_nLotStartStep = 0;

				return RET_GOOD;
			}
			else if (clsEcFirst.m_nLotStartFlag == BD_DATA_REJECT)
			{
				m_nLotStartStep = 0;

				return RET_ERROR;
			}
			else if (clsEcFirst.m_nCommBin == BD_CONT_TIMEOUT)
			{
				m_nLotStartStep = 0;

				return RET_ERROR;
			}
			else if (clsEcFirst.m_nCommBin == BD_SEND_TIMEOUT)
			{
				m_nLotStartStep = 0;

				return RET_ERROR;
			}
			else if (clsEcFirst.m_nCommBin == BD_RCV_TIMEOUT)
			{
				m_nLotStartStep = 0;

				return RET_ERROR;
			}
			break;
	}


	return RET_PROCEED;
}


int	CDialogLotStart::OnLotCancel()
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
			clsEcFirst.OnEcLotCancel(m_strLotNo, _T(""));

			m_nLotCancelStep = 200;
			break;

		case 200:
			if (clsEcFirst.m_nLotCancelFlag == BD_DATA_GOOD)
			{
				m_nLotCancelStep = 0;

				return RET_GOOD;
			}
			else if (clsEcFirst.m_nLotCancelFlag == BD_DATA_REJECT)
			{
				m_nLotCancelStep = 0;

				return RET_ERROR;
			}
			else if (clsEcFirst.m_nCommBin == BD_CONT_TIMEOUT)
			{
				m_nLotCancelStep = 0;

				return RET_ERROR;
			}
			else if (clsEcFirst.m_nCommBin == BD_SEND_TIMEOUT)
			{
				m_nLotCancelStep = 0;

				return RET_ERROR;
			}
			else if (clsEcFirst.m_nCommBin == BD_RCV_TIMEOUT)
			{
				m_nLotCancelStep = 0;

				return RET_ERROR;
			}
			break;
	}

	return RET_PROCEED;
}


int	CDialogLotStart::OnRfidRead()
{
	switch (m_nRfidReadStep)
	{
		case 0:
			if (st_basic_info.nModeRfid == YES)
			{
				m_nRfidReadStep = 100;
			}
			else
			{
				return RET_GOOD;
			}
			break;

		case 100:
			m_dwWaitTime[0] = GetTickCount();
			::SendMessage(st_handler_info.hWnd, WM_RFID_MSG, RFID_READ, 1);

			m_nRfidReadStep = 300;
			break;

		case 300:
			// 데이터 송신 결과 확인 작업
			if (clsRfid.m_nCommBin == BIN_GOODBIN)
			{
				// RFID Data 읽기
			//	RfidReadAllData(1, clsRfid.m_strRfidData); //kwlee 2017.0125
				m_strLotNo = clsRfid.m_strRfidData[RF_LOT_ID_J];
				m_strPartNo = clsRfid.m_strRfidData[RF_PART_ID];

				m_nRfidReadStep = 0;

				return RET_GOOD;
			}
			else if (clsRfid.m_nCommBin == BIN_REJECTBIN)
			{
				m_nRfidReadStep = 0;

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

				if (m_dwWaitTime[2] > (DWORD)10000)//kwlee 2017.0125 5000 -> 10000
				{
					m_nRfidReadStep = 0;

					return RET_ERROR;
				}
			}
			break;
	}
	return RET_PROCEED;
}

void CDialogLotStart::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	int nRet;

	CString strTemp;

	switch (nIDEvent)
	{
		case TM_DISPLAY:
			nRet = OnLotDisplay();

			if (nRet == RET_GOOD)
			{
				KillTimer(TM_DISPLAY);
				
				m_strOpCode = _T("");
				if (st_basic_info.nModeInterface != EQP_ON_LINE)
				{
					st_tray_info[m_nLdCvySite].strLotNo		= m_strLotNo;
					st_tray_info[m_nLdCvySite].strPartNo	= m_strPartNo;

//					st_tray_info[m_nLdCvySite].nQty			= 100;

					//st_lot_info[LOT_CURR].nQty = st_lot_display_info[0].nQty; //james 2016.0929 

					st_lot_display_info[0].strOptcode = m_strOpCode = m_pGridLotInfo.GetItemText(6, 2);

				}
				else
				{
					//st_lot_info[LOT_CURR].nQty = st_lot_display_info[0].nQty; //james 2016.0929 
					st_tray_info[m_nLdCvySite].strOptCode	= _T("");
					st_tray_info[m_nLdCvySite].strPartNo	= st_lot_display_info[0].strPartNo;
					st_tray_info[m_nLdCvySite].nQty			= st_lot_display_info[0].nQty;
					st_tray_info[m_nLdCvySite].strOptCode	= st_lot_display_info[0].strOptcode;
					m_strOpCode								= st_lot_display_info[0].strOptcode;//2016.1011

					//2017.0105
					st_tray_info[m_nLdCvySite].strFabSite	=  st_lot_display_info[0].strFabSite;
				}
				m_btnLotStart.EnableWindow(TRUE);
				m_btnLotCancel.EnableWindow(TRUE);

				//kwlee 2017.0125
				if (st_basic_info.nModeRfid == YES)
				{
					m_pGridLotInfo.SetItemText(0, 2, st_tray_info[m_nLdCvySite].strLotNo); 
				}
				m_pGridLotInfo.SetItemText(1, 2, st_tray_info[m_nLdCvySite].strPartNo);
				m_pGridLotInfo.SetItemText(6, 2, st_lot_display_info[0].strOptcode);
				strTemp.Format(_T("%d"), st_tray_info[m_nLdCvySite].nQty	);
				m_pGridLotInfo.SetItemText(9, 2, strTemp);
				//kwlee 2017.0105
				m_pGridLotInfo.SetItemText(10, 2, st_tray_info[m_nLdCvySite].strFabSite);

				m_pGridLotInfo.Invalidate(FALSE);

				st_handler_info.nMenuLock = FALSE;
			}
			else if (nRet == RET_ERROR)
			{
				KillTimer(TM_DISPLAY);

				st_handler_info.nMenuLock = FALSE;
			}
			break;

		case TM_START:
			nRet = OnLotStart();

			if (nRet == RET_GOOD)
			{
				KillTimer(TM_START);
				if (st_basic_info.nModeInterface != EQP_ON_LINE)
				{
					st_tray_info[m_nLdCvySite].nNewLotIn = YES;
				}

				st_handler_info.nMenuLock = FALSE;
			}
			else if (nRet == RET_ERROR)
			{

				KillTimer(TM_START);

				st_handler_info.nMenuLock = FALSE;
			}
			break;

		case TM_CANCEL:
			nRet = OnLotCancel();

			if (nRet == RET_GOOD)
			{
				KillTimer(TM_CANCEL);

				st_handler_info.nMenuLock = FALSE;
			}
			else if (nRet == RET_ERROR)
			{
				KillTimer(TM_CANCEL);

				st_handler_info.nMenuLock = FALSE;
			}
			break;

		case TM_RFID:
			nRet = OnRfidRead();
		
			if (nRet == RET_GOOD)
			{
				KillTimer(TM_RFID);

// 				m_pGridLotInfo.SetItemText(6, 2, clsRfid.m_strRfidData[RF_CARRIER_ID]);
// 				m_pGridLotInfo.SetItemText(7, 2, clsRfid.m_strRfidData[RF_LOT_ID_J]);
// 				m_pGridLotInfo.SetItemText(8, 2, clsRfid.m_strRfidData[RF_LOT_ID_M]);
				//kwlee 2017.0125
				m_pGridLotInfo.SetItemText(8, 2, clsRfid.m_strRfidData[RF_CARRIER_ID]);
				//m_pGridLotInfo.SetItemText(8, 2, clsRfid.m_strRfidData[RF_LOT_ID_M]);

				m_pGridLotInfo.Invalidate(FALSE);
				
				if (m_strLotNo != _T(""))
				{
					SetTimer(TM_DISPLAY, 100, NULL);
				}

				st_handler_info.nMenuLock = FALSE;
			}
			else if (nRet == RET_ERROR)
			{
				KillTimer(TM_RFID);

				st_handler_info.nMenuLock = FALSE;
			}
			break;
	}
	CDialog::OnTimer(nIDEvent);
}


void CDialogLotStart::OnBnClickedBtnLotDisplay1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (st_handler_info.nMenuLock) return;
	if (m_strLotNo == _T(""))
	{
		st_other_info.strBoxMsg = _T("Lot No 입력되지 않았습니다. 확인해 주세요.");
		if (st_handler_info.cWndMain != NULL)
		{
			st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
		}

		clsFunc.OnMCStop();
		return;
	}
	//2016.1116
	if (m_strPartNo == _T(""))
	{
		st_other_info.strBoxMsg = _T("Part No 입력되지 않았습니다. 확인해 주세요.");
		if (st_handler_info.cWndMain != NULL)
		{
			st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
		}

		clsFunc.OnMCStop();
		return;
	}

	//kwlee 2016.1108
	if (st_handler_info.nAutoMode == NO)
	{
		st_other_info.strBoxMsg = _T("Basic 화면의 Auto/Manual Mode를 확인해 주세요.");
		if (st_handler_info.cWndMain != NULL)
		{
			st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
		}
		clsFunc.OnMCStop();
		return;
	}

	//kwlee 2016.1221 자재 변경 시 메시지 Pop Up
	if (st_handler_info.nModelChangeCheck == TRUE)
	{
		st_other_info.strBoxMsg = _T("Model이 변경 되었습니다. Tray를 확인해 주세요.");
		if (st_handler_info.cWndMain != NULL)
		{
			st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_MESSAGE_BOX_CREATE_REQ, 0);
		}
		st_handler_info.nModelChangeCheck = FALSE;
	}

	st_handler_info.nMenuLock = TRUE;

	SetTimer(TM_DISPLAY, 100, NULL);
}


void CDialogLotStart::OnLotCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	int nRow, nCol;

	CString strTmp;
	CString strMsg;
	CString strKey;
	CString str,strData,strTemp;
	int		nKey;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	switch (nRow)
	{
		case 0:
			if (nCol == 2 || nCol == 3)
			{
				strKey = m_strLotNo;
				KeyBoard(&strKey);

				strKey.MakeUpper();
				m_strLotNo = strKey;

				m_pGridLotInfo.SetItemText(nRow, 2, m_strLotNo);
			}
			break;

		case 1:
			if (nCol == 2 || nCol == 3)
			{
				
				strKey = m_strPartNo;
				KeyBoard(&strKey);

				//kwlee 2016.1029 임시 Part No Device와 현재 Device Name 비교
				str = st_basic_info.strDeviceName;

				strData	= str.Mid(10,4);
				strData.MakeUpper();

				strKey.MakeUpper();
				strTemp = strKey.Mid(0,4);
				///
				if (strTemp == strData)
				{
					m_strPartNo = strKey;
				}
				else
				{
					AfxMessageBox(_T("Not Same Part No and Device, Please Check Device Name"));
					m_strPartNo = _T("");
				}
				//m_strPartNo = strKey;

				m_pGridLotInfo.SetItemText(nRow, 2, m_strPartNo);
			}
			break;

		case 3:
			m_pGridLotInfo.SetItemBkColour(nRow, 0, GREEN_D, BLACK_C);
			m_pGridLotInfo.SetItemBkColour(nRow, 1, GREEN_D, BLACK_C);
			m_pGridLotInfo.SetItemBkColour(nRow, 2, GREEN_D, BLACK_C);
			m_pGridLotInfo.SetItemBkColour(nRow, 3, GREEN_D, BLACK_C);

			m_pGridLotInfo.SetItemBkColour(nRow, nCol, GREEN_C, BLACK_C);
			break;

		case 5:
			m_pGridLotInfo.SetItemBkColour(nRow, 0, GREEN_D, BLACK_C);
			m_pGridLotInfo.SetItemBkColour(nRow, 2, GREEN_D, BLACK_C);

			// 차후 추가예정
			if (nCol == 0 || nCol == 1)
			{
				// 차후 추가예정
				m_pGridLotInfo.SetItemBkColour(nRow, 0, GREEN_C, BLACK_C);
			}
			else
			{
				m_pGridLotInfo.SetItemBkColour(nRow, 2, GREEN_C, BLACK_C);
			}
			break;

		case 6:
			// jtkim 2160929
			if (nCol == 2 || nCol == 3)
			{
				strKey = m_strOpCode;
				KeyBoard(&strKey);

				strKey.MakeUpper();
				m_strOpCode = strKey;

				m_pGridLotInfo.SetItemText(nRow, 2, m_strOpCode);
			}
			break;

		case 9:
			if (nCol == 2)
			{
				nKey = st_tray_info[m_nLdCvySite].nQty;
				KeyPadI(1, 1000, &nKey);

				st_tray_info[m_nLdCvySite].nQty = nKey;

				strTmp.Format(_T("%d"), st_tray_info[m_nLdCvySite].nQty);
				m_pGridLotInfo.SetItemText(9, 2, strTmp);
			}
			break;
	}

	m_pGridLotInfo.Invalidate(FALSE);
}


void CDialogLotStart::OnBnClickedBtnCurrLot()
{

	if(st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)
	{
		AfxMessageBox(_T("투입랏이 현재 전 랏이 될 수 없고 후 랏이어야 합니다. 랏을 체크해 주세요"));
		return;
	}


	CString strMsg;

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_strLotNo	= st_lot_info[LOT_CURR].strLotNo;
	m_strPartNo = st_lot_info[LOT_CURR].strPartNo;
	// jtkim 20160929
	m_strOpCode	= st_lot_info[LOT_CURR].strOpCode;
	//2017.0105
	m_strFabSite = st_lot_info[LOT_CURR].strFabSite;

	m_pGridLotInfo.SetItemText(0, 2, st_lot_info[LOT_CURR].strLotNo);
	m_pGridLotInfo.SetItemText(1, 2, st_lot_info[LOT_CURR].strPartNo);
	// jtkim 20160929
	m_pGridLotInfo.SetItemText(6, 2, st_lot_info[LOT_CURR].strOpCode);
	
	if( st_handler_info.cWndList != NULL )
	{
		strMsg.Format(_T("Lot start dialog btn Curr : %d site"), st_work_info.nLdCvySite+1);
		clsMem.OnNormalMessageWrite(strMsg);
	}

	m_pGridLotInfo.Invalidate(FALSE);
}


void CDialogLotStart::OnBnClickedBtnNextLot()
{
	CString strMsg;

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_strLotNo	= st_lot_info[LOT_NEXT].strLotNo;
	m_strPartNo = st_lot_info[LOT_NEXT].strPartNo;
	// jtkim 20160929
	m_strOpCode	= st_lot_info[LOT_NEXT].strOpCode;
	//2017.0105
	m_strFabSite = st_lot_info[LOT_NEXT].strFabSite;

	m_pGridLotInfo.SetItemText(0, 2, st_lot_info[LOT_NEXT].strLotNo);
	m_pGridLotInfo.SetItemText(1, 2, st_lot_info[LOT_NEXT].strPartNo);
	// jtkim 20160929
	m_pGridLotInfo.SetItemText(6, 2, st_lot_info[LOT_NEXT].strOpCode);

	if( st_handler_info.cWndList != NULL )
	{
		strMsg.Format(_T("Lot start dialog btn Next : %d site"), st_work_info.nLdCvySite+1);
		clsMem.OnNormalMessageWrite(strMsg);
	}

	m_pGridLotInfo.Invalidate(FALSE);
}

CString	CDialogLotStart::OnRecipeExistCheck(CString strFileNo)
{
	CString strRecipe = _T("");						// 함수 리턴 플래그
	CString strTemp;

	HANDLE hFind;
    WIN32_FIND_DATA fd;

	CString strFileName = st_path_info.strBasic;

	if (strFileName.Right (1) != "\\")
	{
        strFileName += "\\";
	}

	strFileName += "*.*";
    
    if ((hFind = ::FindFirstFile ((LPCTSTR) strFileName, &fd)) != INVALID_HANDLE_VALUE) 
	{
        while (::FindNextFile (hFind, &fd)) 
		{
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				strTemp = (fd.cFileName);
				strFileNo.TrimLeft();
				strFileNo.TrimRight();
				strFileNo.TrimLeft(' '); 
				strFileNo.TrimRight(' '); 

				if ( strTemp == strFileNo)
				{
					strRecipe = strTemp;
					break;
				}
			}
        }
		::FindClose(hFind);
    }

	return strRecipe;
}