// DialogNextYieldInfo.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "DialogNextYieldInfo.h"
#include "afxdialogex.h"
#include "PublicFunction.h"

// CDialogNextYieldInfo 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialogNextYieldInfo, CDialogEx)

CDialogNextYieldInfo::CDialogNextYieldInfo(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogNextYieldInfo::IDD, pParent)
{

}

CDialogNextYieldInfo::~CDialogNextYieldInfo()
{
}

void CDialogNextYieldInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CUSTOM_NEXT_YIELD_INFO, m_pGridNextYield);
	DDX_Control(pDX, IDC_STATIC_NEXT_LOTYIELD_INFO, m_groupNextYield);
	DDX_Control(pDX, IDC_BTN_EXIT, m_btnExit);
}


BEGIN_MESSAGE_MAP(CDialogNextYieldInfo, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_EXIT, &CDialogNextYieldInfo::OnBnClickedBtnExit)
END_MESSAGE_MAP()


// CDialogNextYieldInfo 메시지 처리기입니다.


BOOL CDialogNextYieldInfo::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	InitGridNextLotYield();
	InitGroupBox();
	InitBtn();
	InitData();
	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
void CDialogNextYieldInfo::InitGroupBox()
{
	m_groupNextYield.SetFont(clsFunc.OnLogFont(16));
	m_groupNextYield.SetCatptionTextColor(BLUE_C);
	m_groupNextYield.SetBorderColor(ORANGE_C);
	m_groupNextYield.SetFontBold(TRUE);
	//m_groupNextYield.SetBackgroundColor(WINDOW_UP);
}

void CDialogNextYieldInfo::InitGridNextLotYield()
{
	int   i, j;
	int	  max_row, max_col;
	int   row;

	CString strTmp;
	CRect rect;

	max_row = 6; // unloader tray y count
	max_col = 3;

	m_pGridNextYield.SetFrameFocusCell(FALSE);
	m_pGridNextYield.SetTrackFocusCell(FALSE);
	m_pGridNextYield.EnableSelection(FALSE);

	m_pGridNextYield.SetGridLineColor(BLACK_C);
	m_pGridNextYield.SetGridLines(1);

	m_pGridNextYield.SetRowCount(max_row);
	m_pGridNextYield.SetColumnCount(max_col);

	m_pGridNextYield.SetFixedRowCount(0);
	m_pGridNextYield.SetFixedColumnCount(0);
	m_pGridNextYield.SetFixedBkColor(RGB(0,0,200));
	m_pGridNextYield.SetFixedBkColor(RGB(200,200,255));
	m_pGridNextYield.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<max_row; i++) 
	{
		m_pGridNextYield.SetRowHeight(i, 30);

		for (j=0; j<max_col; j++) 
		{
			m_pGridNextYield.SetColumnWidth(j, 91);

			m_pGridNextYield.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridNextYield.SetItemState(i, j, GVIS_READONLY);
			m_pGridNextYield.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
		}
	}

	row = 0;
	m_pGridNextYield.MergeCells(row, 0, row, 2);
	m_pGridNextYield.SetItemBkColour(row, 0, RGB(219, 229, 241), BLACK_C);
	m_pGridNextYield.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridNextYield.SetItemText(row, 0, _T("Next Lot Yield Info"));

	row = 1;
	m_pGridNextYield.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
	m_pGridNextYield.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridNextYield.SetItemText(row, 0, _T(""));

	m_pGridNextYield.SetItemBkColour(row, 1, BLUE_D, WHITE_C);
	m_pGridNextYield.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
	m_pGridNextYield.SetItemText(row, 1, _T("Prime"));

	m_pGridNextYield.SetItemBkColour(row, 2, BLUE_D, WHITE_C);
	m_pGridNextYield.SetItemFont(row, 2, &clsFunc.OnLogFont(16));
	m_pGridNextYield.SetItemText(row, 2, _T("Cum"));

	row = 2;
	m_pGridNextYield.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
	m_pGridNextYield.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridNextYield.SetItemText(row, 0, _T("Input"));

	m_pGridNextYield.MergeCells(row, 1, row, 2);
	m_pGridNextYield.SetItemBkColour(row, 1, WHITE_C, BLACK_C);
	m_pGridNextYield.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
	m_pGridNextYield.SetItemText(row, 1, _T("0"));

	row = 3;
	m_pGridNextYield.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
	m_pGridNextYield.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridNextYield.SetItemText(row, 0, _T("Pass"));

	m_pGridNextYield.SetItemBkColour(row, 1, GREEN_L, BLACK_C);
	m_pGridNextYield.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
	m_pGridNextYield.SetItemText(row, 1, _T("0"));

	m_pGridNextYield.SetItemBkColour(row, 2, GREEN_L, BLACK_C);
	m_pGridNextYield.SetItemFont(row, 2, &clsFunc.OnLogFont(16));
	m_pGridNextYield.SetItemText(row, 2, _T("0"));

	row = 4;
	m_pGridNextYield.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
	m_pGridNextYield.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridNextYield.SetItemText(row, 0, _T("Fail"));

	m_pGridNextYield.SetItemBkColour(row, 1, RED_L, BLACK_C);
	m_pGridNextYield.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
	m_pGridNextYield.SetItemText(row, 1, _T("0"));

	m_pGridNextYield.SetItemBkColour(row, 2, RED_L, BLACK_C);
	m_pGridNextYield.SetItemFont(row, 2, &clsFunc.OnLogFont(16));
	m_pGridNextYield.SetItemText(row, 2, _T("0"));

	row = 5;
	m_pGridNextYield.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
	m_pGridNextYield.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridNextYield.SetItemText(row, 0, _T("Yield"));

	m_pGridNextYield.SetItemBkColour(row, 1, GREEN_L, BLACK_C);
	m_pGridNextYield.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
	m_pGridNextYield.SetItemText(row, 1, _T("0"));

	m_pGridNextYield.SetItemBkColour(row, 2, GREEN_L, BLACK_C);
	m_pGridNextYield.SetItemFont(row, 2, &clsFunc.OnLogFont(16));
	m_pGridNextYield.SetItemText(row, 2, _T("0"));

}
void CDialogNextYieldInfo::InitBtn()
{
	m_btnExit.SetBitmaps(IDC_BTN_EXIT, IDB_BITMAP_EXIT, WINDOW_DN, IDB_BITMAP_EXIT, WINDOW_UP);
	m_btnExit.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
//	m_btnExit.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnExit.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnExit.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnExit.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnExit.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnExit.SetFont(clsFunc.m_pFont[3]);
	
	m_btnExit.SetTooltipText(_T("Exit"));
}

void CDialogNextYieldInfo::OnBnClickedBtnExit()
{
	CDialog::OnOK();
}
void CDialogNextYieldInfo::InitData()
{
	CString strTemp;

	double dAve;

	// lot 수량.
	strTemp.Format(_T("%d"), st_count_info.nInCount[0][1]);
	m_pGridNextYield.SetItemText(2, 1, strTemp);

	strTemp.Format(_T("%d"), st_count_info.nPrimeCount[0][1]);
	m_pGridNextYield.SetItemText(3, 1, strTemp);

	strTemp.Format(_T("%d"), st_count_info.nPassCount[0][1]);
	m_pGridNextYield.SetItemText(3, 2, strTemp);

	// jtkim 20150307
	strTemp.Format(_T("%d"), st_count_info.nPrimeRejectCount[0][1]);
	m_pGridNextYield.SetItemText(4, 1, strTemp);

	strTemp.Format(_T("%d"), st_count_info.nRejectCount[0][1]);
	m_pGridNextYield.SetItemText(4, 2, strTemp);

	if (st_count_info.nPrimeCount[0][1] > 0) 
	{
		//dAve = ((double)st_count_info.nPrimeCount[0][0] / (double)st_count_info.nInCount[0][0]) * (double)100;
		dAve = ((double)st_count_info.nPrimeCount[0][1] / ((double)st_count_info.nPrimeCount[0][1] + (double)st_count_info.nPrimeRejectCount[0][1])) * (double)100;
		strTemp.Format(_T("%.2f"), dAve);
		m_pGridNextYield.SetItemText(5, 1, strTemp);
	}

	if (st_count_info.nPassCount[0][1] > 0) 
	{
		//dAve = ((double)st_count_info.nPassCount[0][0] / (double)st_count_info.nInCount[0][0]) * (double)100;
		dAve = ((double)st_count_info.nPassCount[0][1] / ((double)st_count_info.nPassCount[0][1] + (double)st_count_info.nRejectCount[0][1])) * (double)100;
		strTemp.Format(_T("%.2f"), dAve);
		m_pGridNextYield.SetItemText(5, 2, strTemp);
	}

	m_pGridNextYield.Invalidate(FALSE);
}