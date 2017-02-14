// SlotInfo.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "SlotInfo.h"
#include "afxdialogex.h"
#include "PublicFunction.h"
#include ".\\Ctrl\\KeyBoard.h"
#include "Dialog_Message.h"
// CSlotInfo 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSlotInfo, CDialogEx)

CSlotInfo::CSlotInfo(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSlotInfo::IDD, pParent)
{

}

CSlotInfo::~CSlotInfo()
{
}

void CSlotInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CUSTOM_SITE_INFO, m_Grid_SiteInfo);
	DDX_Control(pDX, IDC_CUSTOM_SLOT_INFO, m_GridSlotInfo);
	DDX_Control(pDX, IDC_STATIC_SLOT_INFO, m_group_SlotInfo);
	DDX_Control(pDX, IDC_BTN_SLOT_1, m_Btn_Slot_1);
	DDX_Control(pDX, IDC_BTN_SLOT_2, m_Btn_Slot_2);
	DDX_Control(pDX, IDC_BTN_SLOT_3, m_Btn_Slot_3);
	DDX_Control(pDX, IDC_BTN_SLOT_4, m_Btn_Slot_4);
	DDX_Control(pDX, IDC_BTN_SLOT_5, m_Btn_Slot_5);
	DDX_Control(pDX, IDC_BTN_SLOT_6, m_Btn_Slot_6);
	DDX_Control(pDX, IDC_BTN_SLOT_7, m_Btn_Slot_7);
	DDX_Control(pDX, IDC_BTN_SLOT_8, m_Btn_Slot_8);
	DDX_Control(pDX, IDC_CUSTOM_SITE_SELECT, m_Grid_SiteSel);
	DDX_Control(pDX, IDC_STATIC_SITE_SEL, m_group_Site_Sel);
	DDX_Control(pDX, IDC_STATIC_SITE_INFO, m_group_Site_Info);
}


BEGIN_MESSAGE_MAP(CSlotInfo, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_SLOT_1, &CSlotInfo::OnBnClickedBtnSlot1)
	ON_BN_CLICKED(IDC_BTN_SLOT_2, &CSlotInfo::OnBnClickedBtnSlot2)
	ON_BN_CLICKED(IDC_BTN_SLOT_3, &CSlotInfo::OnBnClickedBtnSlot3)
	ON_BN_CLICKED(IDC_BTN_SLOT_4, &CSlotInfo::OnBnClickedBtnSlot4)
	ON_BN_CLICKED(IDC_BTN_SLOT_5, &CSlotInfo::OnBnClickedBtnSlot5)
	ON_BN_CLICKED(IDC_BTN_SLOT_6, &CSlotInfo::OnBnClickedBtnSlot6)
	ON_BN_CLICKED(IDC_BTN_SLOT_7, &CSlotInfo::OnBnClickedBtnSlot7)
	ON_BN_CLICKED(IDC_BTN_SLOT_8, &CSlotInfo::OnBnClickedBtnSlot8)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_SLOT_INFO,				OnCellClickSlotInfo)
	
	ON_BN_CLICKED(IDC_BTN_UPDATE, &CSlotInfo::OnBnClickedBtnUpdate)
END_MESSAGE_MAP()


BOOL CSlotInfo::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	OnInitSlotInfo();
	OnInitSiteInfo();
	OnInitSiteSel();
	OnInitGroupBox();
	OnInitBtn();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
void  CSlotInfo::OnInitSlotInfo()
{
	int   i, j;
	int	  max_row, max_col;
	int   row;
	int   nTemp;
	CString strTmp;
	CRect rect;

	//max_row = 12; // unloader tray y count
	max_row = 9; // unloader tray y count
	max_col = 2;

	m_GridSlotInfo.SetFrameFocusCell(FALSE);
	m_GridSlotInfo.SetTrackFocusCell(FALSE);
	m_GridSlotInfo.EnableSelection(FALSE);

	m_GridSlotInfo.SetGridLineColor(BLACK_C);
	m_GridSlotInfo.SetGridLines(1);

	m_GridSlotInfo.SetRowCount(max_row);
	m_GridSlotInfo.SetColumnCount(max_col);

	m_GridSlotInfo.SetFixedRowCount(0);
	m_GridSlotInfo.SetFixedColumnCount(0);
	m_GridSlotInfo.SetFixedBkColor(RGB(0,0,200));
	m_GridSlotInfo.SetFixedBkColor(RGB(200,200,255));
	m_GridSlotInfo.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<max_row; i++) 
	{
		m_GridSlotInfo.SetRowHeight(i, 27);

		for (j=0; j<max_col; j++) 
		{
			m_GridSlotInfo.SetColumnWidth(j, 140);

			m_GridSlotInfo.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_GridSlotInfo.SetItemState(i, j, GVIS_READONLY);
			m_GridSlotInfo.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
		}
	}

	row = 0;
	m_GridSlotInfo.MergeCells(row, 0, row, 1);
	m_GridSlotInfo.SetItemBkColour(row, 0, RGB(219, 229, 241), BLACK_C);
	m_GridSlotInfo.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_GridSlotInfo.SetItemText(row, 0, _T("Slot Info"));

	row = 1;
	m_GridSlotInfo.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
	m_GridSlotInfo.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_GridSlotInfo.SetItemText(row, 0, _T("Lot No"));

	strTmp.Format(_T("%s"),st_lot_info[LOT_CURR].strLotNo); //kwlee 2016.1124
	m_GridSlotInfo.SetItemBkColour(row, 1, WHITE_C, BLACK_C);
	m_GridSlotInfo.SetItemFont(row, 1, &clsFunc.OnLogFont(10));
	//m_GridSlotInfo.SetItemText(row, 1, _T("-"));
	m_GridSlotInfo.SetItemText(row, 1, strTmp);

	row = 2;
	m_GridSlotInfo.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
	m_GridSlotInfo.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_GridSlotInfo.SetItemText(row, 0, _T("Part No"));

	strTmp.Format(_T("%s"),st_lot_info[LOT_CURR].strPartNo); //kwlee 2016.1124
	m_GridSlotInfo.SetItemBkColour(row, 1, WHITE_C, BLACK_C);
	m_GridSlotInfo.SetItemFont(row, 1, &clsFunc.OnLogFont(10));
	//m_GridSlotInfo.SetItemText(row, 1, _T("-"));
	m_GridSlotInfo.SetItemText(row, 1, strTmp);

// 	row = 3;
// 	m_GridSlotInfo.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
// 	m_GridSlotInfo.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
// 	m_GridSlotInfo.SetItemText(row, 0, _T("1 Lot No"));
// 
// 	m_GridSlotInfo.SetItemBkColour(row, 1, WHITE_C, BLACK_C);
// 	m_GridSlotInfo.SetItemFont(row, 1, &clsFunc.OnLogFont(10));
// 	m_GridSlotInfo.SetItemText(row, 1, _T("-"));

	row = 3;
	//kwlee 2016.1124
	m_GridSlotInfo.SetItemBkColour(row, 0, ORANGE_C, BLACK_C);
	m_GridSlotInfo.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_GridSlotInfo.SetItemText(row, 0, _T("Test Time"));
	
	//kwlee 2016.1124
	nTemp = st_lot_info[LOT_CURR].nBdTime /60;
	strTmp.Format(_T("%d(분)"),nTemp);
	m_GridSlotInfo.SetItemBkColour(row, 1, WHITE_C, BLACK_C);
	m_GridSlotInfo.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
	m_GridSlotInfo.SetItemText(row, 1,strTmp);
	//m_GridSlotInfo.SetItemText(row, 1, _T("-"));
	////

// 	m_GridSlotInfo.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
// 	m_GridSlotInfo.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
// 	m_GridSlotInfo.SetItemText(row, 0, _T("2 Lot No"));
// 
// 	m_GridSlotInfo.SetItemBkColour(row, 1, WHITE_C, BLACK_C);
// 	m_GridSlotInfo.SetItemFont(row, 1, &clsFunc.OnLogFont(10));
// 	m_GridSlotInfo.SetItemText(row, 1, _T("-"));


	row = 4;
	m_GridSlotInfo.SetItemBkColour(row, 0, ORANGE_C, BLACK_C);
	m_GridSlotInfo.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_GridSlotInfo.SetItemText(row, 0, _T("Retest Count"));

	strTmp.Format(_T("%d"),st_recipe_info.nTestRetest_Count);
	m_GridSlotInfo.SetItemBkColour(row, 1, WHITE_C, BLACK_C);
	m_GridSlotInfo.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
	m_GridSlotInfo.SetItemText(row, 1,strTmp);
	//m_GridSlotInfo.SetItemText(row, 1, _T("-"));

// 	m_GridSlotInfo.SetItemBkColour(row, 0, ORANGE_C, BLACK_C);
// 	m_GridSlotInfo.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
// 	m_GridSlotInfo.SetItemText(row, 0, _T("1D"));
// 
// 	m_GridSlotInfo.SetItemBkColour(row, 1, WHITE_C, BLACK_C);
// 	m_GridSlotInfo.SetItemFont(row, 1, &clsFunc.OnLogFont(10));
// 	m_GridSlotInfo.SetItemText(row, 1, _T("-"));

	row = 5;
	//kwlee 2016.1124
	m_GridSlotInfo.SetItemBkColour(row, 0, ORANGE_C, BLACK_C);
	m_GridSlotInfo.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_GridSlotInfo.SetItemText(row, 0, _T("Current Pos"));

	strTmp.Format(_T("%s"),st_lot_info[LOT_CURR_BACKUP].strLotNo);
	m_GridSlotInfo.SetItemBkColour(row, 1, WHITE_C, BLACK_C);
	m_GridSlotInfo.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
	m_GridSlotInfo.SetItemText(row, 1,strTmp);
// 	m_GridSlotInfo.SetItemBkColour(row, 0, ORANGE_C, BLACK_C);
// 	m_GridSlotInfo.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
// 	m_GridSlotInfo.SetItemText(row, 0, _T("1D"));
// 
// 	m_GridSlotInfo.SetItemBkColour(row, 1, WHITE_C, BLACK_C);
// 	m_GridSlotInfo.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
// 	m_GridSlotInfo.SetItemText(row, 1, _T("-"));

	row = 6;
	m_GridSlotInfo.SetItemBkColour(row, 0, ORANGE_C, BLACK_C);
	m_GridSlotInfo.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_GridSlotInfo.SetItemText(row, 0, _T("Curr Lot Status"));


	if(st_lot_info[LOT_CURR].nLotStatus == LOT_START)
	{
		strTmp.Format(_T("LOT_START"));
	}
	else if(st_lot_info[LOT_CURR].nLotStatus == LOT_END_START)
	{
		strTmp.Format(_T("LOT_END_START"));
	}
	else if(st_lot_info[LOT_CURR].nLotStatus == LOT_END)
	{
		strTmp.Format(_T("LOT_END"));
	}
	else
	{
		strTmp.Format(_T(""));
	}

	//strTmp.Format(_T("%d"),st_lot_info[LOT_CURR].nLotStatus);
	m_GridSlotInfo.SetItemBkColour(row, 1, WHITE_C, BLACK_C);
	m_GridSlotInfo.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
	m_GridSlotInfo.SetItemText(row, 1,strTmp);
// 	m_GridSlotInfo.SetItemBkColour(row, 0, ORANGE_C, BLACK_C);
// 	m_GridSlotInfo.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
// 	m_GridSlotInfo.SetItemText(row, 0, _T("2D"));

// 	m_GridSlotInfo.SetItemBkColour(row, 1, WHITE_C, BLACK_C);
// 	m_GridSlotInfo.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
// 	m_GridSlotInfo.SetItemText(row, 1, _T("-"));

	row = 7;
	m_GridSlotInfo.SetItemBkColour(row, 0, ORANGE_C, BLACK_C);
	m_GridSlotInfo.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_GridSlotInfo.SetItemText(row, 0, _T("Lot Start Time"));

	strTmp.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"), st_lot_info[LOT_CURR].tLotStart.GetYear(), st_lot_info[LOT_CURR].tLotStart.GetMonth(), 
		st_lot_info[LOT_CURR].tLotStart.GetDay(), st_lot_info[LOT_CURR].tLotStart.GetHour(), st_lot_info[LOT_CURR].tLotStart.GetMinute(), st_lot_info[LOT_CURR].tLotStart.GetSecond());
	m_GridSlotInfo.SetItemBkColour(row, 1, WHITE_C, BLACK_C);
	m_GridSlotInfo.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
	m_GridSlotInfo.SetItemText(row, 1,strTmp);
// 	m_GridSlotInfo.SetItemBkColour(row, 0, ORANGE_C, BLACK_C);
// 	m_GridSlotInfo.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
// 	m_GridSlotInfo.SetItemText(row, 0, _T("Amount"));
// 
// 	m_GridSlotInfo.SetItemBkColour(row, 1, WHITE_C, BLACK_C);
// 	m_GridSlotInfo.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
// 	m_GridSlotInfo.SetItemText(row, 1, _T("-"));

 	row = 8;
	m_GridSlotInfo.SetItemBkColour(row, 0, ORANGE_C, BLACK_C);
	m_GridSlotInfo.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_GridSlotInfo.SetItemText(row, 0, _T("Lot Cnt"));

	strTmp.Format(_T("%d"),st_lot_info[LOT_CURR].nQty);
	m_GridSlotInfo.SetItemBkColour(row, 1, WHITE_C, BLACK_C);
	m_GridSlotInfo.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
	m_GridSlotInfo.SetItemText(row, 1,strTmp);
// 	m_GridSlotInfo.SetItemBkColour(row, 0, ORANGE_C, BLACK_C);
// 	m_GridSlotInfo.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
// 	m_GridSlotInfo.SetItemText(row, 0, _T("Test Time"));
// 
// 	m_GridSlotInfo.SetItemBkColour(row, 1, WHITE_C, BLACK_C);
// 	m_GridSlotInfo.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
// 	m_GridSlotInfo.SetItemText(row, 1, _T("-"));

// 	row = 9;
// 	m_GridSlotInfo.SetItemBkColour(row, 0, ORANGE_C, BLACK_C);
// 	m_GridSlotInfo.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
// 	m_GridSlotInfo.SetItemText(row, 0, _T("Pass Count"));
// 
// 	strTmp.Format(_T("%d"),st_count_info.nPassCount[0][0]);
// 	m_GridSlotInfo.SetItemBkColour(row, 1, WHITE_C, BLACK_C);
// 	m_GridSlotInfo.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
// 	m_GridSlotInfo.SetItemText(row, 1,strTmp);
// 	m_GridSlotInfo.SetItemBkColour(row, 0, ORANGE_C, BLACK_C);
// 	m_GridSlotInfo.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
// 	m_GridSlotInfo.SetItemText(row, 0, _T("Retest Count"));
// 
// 	m_GridSlotInfo.SetItemBkColour(row, 1, WHITE_C, BLACK_C);
// 	m_GridSlotInfo.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
// 	m_GridSlotInfo.SetItemText(row, 1, _T("-"));

// 	row = 10;
// 	m_GridSlotInfo.SetItemBkColour(row, 0, ORANGE_C, BLACK_C);
// 	m_GridSlotInfo.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
// 	m_GridSlotInfo.SetItemText(row, 0, _T("Fail Count"));
// 
// 	strTmp.Format(_T("%d"),st_count_info.nRejectCount[0][0]);
// 	m_GridSlotInfo.SetItemBkColour(row, 1, WHITE_C, BLACK_C);
// 	m_GridSlotInfo.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
// 	m_GridSlotInfo.SetItemText(row, 1,strTmp);
// 	m_GridSlotInfo.SetItemBkColour(row, 0, ORANGE_C, BLACK_C);
// 	m_GridSlotInfo.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
// 	m_GridSlotInfo.SetItemText(row, 0, _T("Current Pos"));
// 
// 	m_GridSlotInfo.SetItemBkColour(row, 1, WHITE_C, BLACK_C);
// 	m_GridSlotInfo.SetItemFont(row, 1, &clsFunc.OnLogFont(16));
// 	m_GridSlotInfo.SetItemText(row, 1, _T("-"));
}

void CSlotInfo::OnInitSiteInfo()
{
	int   i, j;
	int	  max_row, max_col;
	//	int   row;

	CString strTmp;
	CRect rect;

	max_row = 8; // unloader tray y count
	max_col = 1;

	m_Grid_SiteInfo.SetFrameFocusCell(FALSE);
	m_Grid_SiteInfo.SetTrackFocusCell(FALSE);
	m_Grid_SiteInfo.EnableSelection(FALSE);

	m_Grid_SiteInfo.SetGridLineColor(BLACK_C);
	m_Grid_SiteInfo.SetGridLines(1);

	m_Grid_SiteInfo.SetRowCount(max_row);
	m_Grid_SiteInfo.SetColumnCount(max_col);

	m_Grid_SiteInfo.SetFixedRowCount(0);
	m_Grid_SiteInfo.SetFixedColumnCount(0);
	m_Grid_SiteInfo.SetFixedBkColor(RGB(0,0,200));
	m_Grid_SiteInfo.SetFixedBkColor(RGB(200,200,255));
	m_Grid_SiteInfo.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<max_row; i++) 
	{
		m_Grid_SiteInfo.SetRowHeight(i, 24);

		for (j=0; j<max_col; j++) 
		{
			m_Grid_SiteInfo.SetColumnWidth(j, 92);
			m_Grid_SiteInfo.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_Grid_SiteInfo.SetItemState(i, j, GVIS_READONLY);
			m_Grid_SiteInfo.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
			m_Grid_SiteInfo.SetItemText(i, j, _T(" "));
		}
	}
}

void CSlotInfo::OnInitSiteSel()
{
	int   i, j;
	int	  max_row, max_col;
	int   row;

	CString strTmp;
	CRect rect;

	max_row = 12; // unloader tray y count
	max_col = 1;

	m_Grid_SiteSel.SetFrameFocusCell(FALSE);
	m_Grid_SiteSel.SetTrackFocusCell(FALSE);
	m_Grid_SiteSel.EnableSelection(FALSE);

	m_Grid_SiteSel.SetGridLineColor(BLACK_C);
	m_Grid_SiteSel.SetGridLines(1);

	m_Grid_SiteSel.SetRowCount(max_row);
	m_Grid_SiteSel.SetColumnCount(max_col);

	m_Grid_SiteSel.SetFixedRowCount(0);
	m_Grid_SiteSel.SetFixedColumnCount(0);
	m_Grid_SiteSel.SetFixedBkColor(RGB(0,0,200));
	m_Grid_SiteSel.SetFixedBkColor(RGB(200,200,255));
	m_Grid_SiteSel.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<max_row; i++) 
	{
		m_Grid_SiteSel.SetRowHeight(i, 17);

		for (j=0; j<max_col; j++) 
		{
			m_Grid_SiteSel.SetColumnWidth(j, 113);
			m_Grid_SiteSel.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_Grid_SiteSel.SetItemState(i, j, GVIS_READONLY);
			m_Grid_SiteSel.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
			m_Grid_SiteSel.SetItemText(i, j, _T(" "));
		}
	}

	row = 0;
	m_Grid_SiteSel.SetItemBkColour(row, 0, WHITE_C, BLACK_C);
	m_Grid_SiteSel.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_Grid_SiteSel.SetItemText(row, 0, _T("LD_Buffer"));

	row = 1;
	m_Grid_SiteSel.SetItemBkColour(row, 0, WHITE_C, BLACK_C);
	m_Grid_SiteSel.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_Grid_SiteSel.SetItemText(row, 0, _T("ULD_Buffer"));

	row = 2;
	m_Grid_SiteSel.SetItemBkColour(row, 0, WHITE_C, BLACK_C);
	m_Grid_SiteSel.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_Grid_SiteSel.SetItemText(row, 0, _T("Retest_1"));

	row = 3;
	m_Grid_SiteSel.SetItemBkColour(row, 0, WHITE_C, BLACK_C);
	m_Grid_SiteSel.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_Grid_SiteSel.SetItemText(row, 0, _T("Retest_2"));

	row = 4;
	m_Grid_SiteSel.SetItemBkColour(row, 0, WHITE_C, BLACK_C);
	m_Grid_SiteSel.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_Grid_SiteSel.SetItemText(row, 0, _T("TestSite_1"));

	row = 5;
	m_Grid_SiteSel.SetItemBkColour(row, 0, WHITE_C, BLACK_C);
	m_Grid_SiteSel.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_Grid_SiteSel.SetItemText(row, 0, _T("TestSite_2"));

	row = 6;
	m_Grid_SiteSel.SetItemBkColour(row, 0, WHITE_C, BLACK_C);
	m_Grid_SiteSel.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_Grid_SiteSel.SetItemText(row, 0, _T("TestSite_3"));

	row = 7;
	m_Grid_SiteSel.SetItemBkColour(row, 0, WHITE_C, BLACK_C);
	m_Grid_SiteSel.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_Grid_SiteSel.SetItemText(row, 0, _T("TestSite_4"));

	row = 8;
	m_Grid_SiteSel.SetItemBkColour(row, 0, WHITE_C, BLACK_C);
	m_Grid_SiteSel.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_Grid_SiteSel.SetItemText(row, 0, _T("TestSite_5"));

	row = 9;
	m_Grid_SiteSel.SetItemBkColour(row, 0, WHITE_C, BLACK_C);
	m_Grid_SiteSel.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_Grid_SiteSel.SetItemText(row, 0, _T("TestSite_6"));

	row = 10;
	m_Grid_SiteSel.SetItemBkColour(row, 0, WHITE_C, BLACK_C);
	m_Grid_SiteSel.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_Grid_SiteSel.SetItemText(row, 0, _T("TestSite_7"));

	row = 11;
	m_Grid_SiteSel.SetItemBkColour(row, 0, WHITE_C, BLACK_C);
	m_Grid_SiteSel.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_Grid_SiteSel.SetItemText(row, 0, _T("TestSite_8"));
}

void CSlotInfo::OnInitGroupBox()
{
	m_group_SlotInfo.SetFont(clsFunc.OnLogFont(16));
	m_group_SlotInfo.SetCatptionTextColor(BLUE_C);
	m_group_SlotInfo.SetBorderColor(ORANGE_C);
	m_group_SlotInfo.SetFontBold(TRUE);
	//m_group_SlotInfo.SetBackgroundColor(WINDOW_UP);

	m_group_Site_Info.SetFont(clsFunc.OnLogFont(16));
	m_group_Site_Info.SetCatptionTextColor(BLUE_C);
	m_group_Site_Info.SetBorderColor(ORANGE_C);
	m_group_Site_Info.SetFontBold(TRUE);
	//m_group_Site_Info.SetBackgroundColor(WINDOW_UP);

	m_group_Site_Sel.SetFont(clsFunc.OnLogFont(16));
	m_group_Site_Sel.SetCatptionTextColor(BLUE_C);
	m_group_Site_Sel.SetBorderColor(ORANGE_C);
	m_group_Site_Sel.SetFontBold(TRUE);
//	m_group_Site_Sel.SetBackgroundColor(WINDOW_UP);
}
void CSlotInfo::OnInitBtn()
{
	m_Btn_Slot_1.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_Btn_Slot_1.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_Btn_Slot_1.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_Btn_Slot_1.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_Btn_Slot_1.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_Btn_Slot_1.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	//m_btn_Sodimm.SetFont(clsFunc.m_pFont[3]);
	m_Btn_Slot_1.SetTooltipText(_T("Slot_1"));

	m_Btn_Slot_2.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_Btn_Slot_2.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_Btn_Slot_2.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_Btn_Slot_2.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_Btn_Slot_2.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_Btn_Slot_2.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_Btn_Slot_2.SetTooltipText(_T("Slot_2"));

	m_Btn_Slot_3.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_Btn_Slot_3.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_Btn_Slot_3.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_Btn_Slot_3.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_Btn_Slot_3.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_Btn_Slot_3.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	//m_btn_Sodimm.SetFont(clsFunc.m_pFont[3]);
	m_Btn_Slot_3.SetTooltipText(_T("Slot_3"));

	m_Btn_Slot_4.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_Btn_Slot_4.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_Btn_Slot_4.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_Btn_Slot_4.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_Btn_Slot_4.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_Btn_Slot_4.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	//m_btn_Sodimm.SetFont(clsFunc.m_pFont[3]);
	m_Btn_Slot_4.SetTooltipText(_T("Slot_4"));

	m_Btn_Slot_5.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_Btn_Slot_5.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_Btn_Slot_5.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_Btn_Slot_5.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_Btn_Slot_5.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_Btn_Slot_5.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	//m_btn_Sodimm.SetFont(clsFunc.m_pFont[3]);
	m_Btn_Slot_5.SetTooltipText(_T("Slot_5"));

	m_Btn_Slot_6.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_Btn_Slot_6.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_Btn_Slot_6.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_Btn_Slot_6.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_Btn_Slot_6.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_Btn_Slot_6.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	//m_btn_Sodimm.SetFont(clsFunc.m_pFont[3]);
	m_Btn_Slot_6.SetTooltipText(_T("Slot_6"));

	m_Btn_Slot_7.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_Btn_Slot_7.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_Btn_Slot_7.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_Btn_Slot_7.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_Btn_Slot_7.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_Btn_Slot_7.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	//m_btn_Sodimm.SetFont(clsFunc.m_pFont[3]);
	m_Btn_Slot_7.SetTooltipText(_T("Slot_7"));

	m_Btn_Slot_8.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_Btn_Slot_8.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_Btn_Slot_8.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_Btn_Slot_8.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_Btn_Slot_8.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_Btn_Slot_8.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	//m_btn_Sodimm.SetFont(clsFunc.m_pFont[3]);
	m_Btn_Slot_8.SetTooltipText(_T("Slot_8"));

}

void CSlotInfo::OnBnClickedBtnSlot1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnInitSiteInfo();
	m_Grid_SiteInfo.SetItemBkColour(0, 0,GREEN_C, BLACK_C);
	m_Grid_SiteInfo.SetItemFont(0, 0, &clsFunc.OnLogFont(16));
	m_Grid_SiteInfo.SetItemText(0, 0, _T("1"));
	m_Grid_SiteInfo.Invalidate();
}


void CSlotInfo::OnBnClickedBtnSlot2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnInitSiteInfo();
	m_Grid_SiteInfo.SetItemBkColour(1, 0,GREEN_C, BLACK_C);
	m_Grid_SiteInfo.SetItemFont(1, 0, &clsFunc.OnLogFont(16));
	m_Grid_SiteInfo.SetItemText(1, 0, _T("2"));
	m_Grid_SiteInfo.Invalidate();
}


void CSlotInfo::OnBnClickedBtnSlot3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnInitSiteInfo();
	m_Grid_SiteInfo.SetItemBkColour(2, 0,GREEN_C, BLACK_C);
	m_Grid_SiteInfo.SetItemFont(2, 0, &clsFunc.OnLogFont(16));
	m_Grid_SiteInfo.SetItemText(2, 0, _T("3"));
	m_Grid_SiteInfo.Invalidate();
}


void CSlotInfo::OnBnClickedBtnSlot4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnInitSiteInfo();
	m_Grid_SiteInfo.SetItemBkColour(3, 0,GREEN_C, BLACK_C);
	m_Grid_SiteInfo.SetItemFont(3, 0, &clsFunc.OnLogFont(16));
	m_Grid_SiteInfo.SetItemText(3, 0, _T("4"));
	m_Grid_SiteInfo.Invalidate();
}


void CSlotInfo::OnBnClickedBtnSlot5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnInitSiteInfo();
	m_Grid_SiteInfo.SetItemBkColour(4, 0,GREEN_C, BLACK_C);
	m_Grid_SiteInfo.SetItemFont(4, 0, &clsFunc.OnLogFont(16));
	m_Grid_SiteInfo.SetItemText(4, 0, _T("5"));
	m_Grid_SiteInfo.Invalidate();
}


void CSlotInfo::OnBnClickedBtnSlot6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnInitSiteInfo();
	m_Grid_SiteInfo.SetItemBkColour(5, 0,GREEN_C, BLACK_C);
	m_Grid_SiteInfo.SetItemFont(5, 0, &clsFunc.OnLogFont(16));
	m_Grid_SiteInfo.SetItemText(5, 0, _T("6"));
	m_Grid_SiteInfo.Invalidate();
}


void CSlotInfo::OnBnClickedBtnSlot7()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnInitSiteInfo();
	m_Grid_SiteInfo.SetItemBkColour(6, 0,GREEN_C, BLACK_C);
	m_Grid_SiteInfo.SetItemFont(6, 0, &clsFunc.OnLogFont(16));
	m_Grid_SiteInfo.SetItemText(6, 0, _T("7"));
	m_Grid_SiteInfo.Invalidate();
}


void CSlotInfo::OnBnClickedBtnSlot8()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnInitSiteInfo();
	m_Grid_SiteInfo.SetItemBkColour(7, 0,GREEN_C, BLACK_C);
	m_Grid_SiteInfo.SetItemFont(7, 0, &clsFunc.OnLogFont(16));
	m_Grid_SiteInfo.SetItemText(7, 0, _T("8"));
	m_Grid_SiteInfo.Invalidate();
}

void CSlotInfo::OnCellClickSlotInfo(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	int nRow, nCol;

	CString strKey;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	return;

	if(nRow < 0) return;

	//lotNo
	if (nRow == 0)
	{
		KeyBoard(&strKey);

		m_strLotNo = strKey;
		m_GridSlotInfo.SetItemText(nRow, 1, strKey);
	}
	//PartNo
	else if (nRow == 1 )
	{
	
		KeyBoard(&strKey);
		m_strPartNo = strKey;
		m_GridSlotInfo.SetItemText(nRow, 1, strKey);
	}
	//NextLotNo
	else if (nRow == 2 )
	{

		KeyBoard(&strKey);
		m_strNextLotNo = strKey;
		m_GridSlotInfo.SetItemText(nRow, 1, strKey);
	}
	//CurrLotNo
	else if (nRow == 3 )
	{
		KeyBoard(&strKey);
		m_strCurrLotNo = strKey;
		m_GridSlotInfo.SetItemText(nRow,1, strKey);
	}
	//1D
	else if (nRow == 4 )
	{
		KeyBoard(&strKey);
		m_str1D[0] = strKey;
		m_GridSlotInfo.SetItemText(nRow, 1, strKey);
	}
	//1D
	else if (nRow == 5)
	{
		KeyBoard(&strKey);
		m_str1D[1] = strKey;
		m_GridSlotInfo.SetItemText(nRow, 1, strKey);
	}
	//2D
	else if (nRow == 6 )
	{

		return;
		KeyBoard(&strKey);
// #define LOT_CLEAR						0 //2015.0303 james -1
// #define LOT_START						1
// #define LOT_END_START					2
// #define LOT_END							3
		m_str2D = strKey;

		if(m_str2D == _T("0"))
		{
			m_GridSlotInfo.SetItemText(nRow, 1, _T("LOT CLEAR") );
		}
		else if(m_str2D == _T("1"))
		{
			m_GridSlotInfo.SetItemText(nRow, 1, _T("LOT START") );
		}
		else if(m_str2D == _T("2"))
		{
			m_GridSlotInfo.SetItemText(nRow, 1, _T("LOT END START") );
		}
		else if(m_str2D == _T("3"))
		{
			m_GridSlotInfo.SetItemText(nRow, 1, _T("LOT END") );
		}
	}
	//Amount
	else if (nRow == 7 )
	{
		KeyBoard(&strKey);
		m_strAmount = strKey;
		m_GridSlotInfo.SetItemText(nRow, 1, strKey);
	}
	//TestTime
	else if (nRow == 8 )
	{
		KeyBoard(&strKey);
		m_strTestTime = strKey;
		m_GridSlotInfo.SetItemText(nRow, 1, strKey);
	}
	//RetestCnt
	else if (nRow == 9 )
	{
		KeyBoard(&strKey);
		m_nRetestCnt = _wtoi(strKey);
		m_GridSlotInfo.SetItemText(nRow, 1, strKey);
	}
	//CurPos
	else if (nRow == 10 )
	{
		KeyBoard(&strKey);
		m_strCurrPos = strKey;
		m_GridSlotInfo.SetItemText(nRow, 1, strKey);
	}
	
	
	m_GridSlotInfo.Invalidate();
}

void CSlotInfo::OnBnClickedBtnUpdate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	CDialog_Message dlgMsg;

//	int				nResponse;

//	CString			strTemp;

//	dlgMsg.m_nMessageType	= 1;

// 	if (m_nWorkNum < 0) return;
// 
// 	if (m_nWorkNum == 0)
// 	{
// 		dlgMsg.m_strMessage		= _T("Do you want a update information the current lot status?");
// 	}
// 	else
// 	{
// 		dlgMsg.m_strMessage		= _T("Do you want a update information the next lot status?");
// 	}
// 
// 	nResponse = (int)dlgMsg.DoModal();
// 
// 	if (nResponse == IDOK)
// 	{
// 		st_lot_info[m_nWorkNum].strLotNo = m_strLotNo;
// 		m_pGridLotInfo.SetItemText(1, 1, st_lot_info[m_nWorkNum].strLotNo);
// 
// 		st_lot_info[m_nWorkNum].strPartNo = m_strPartNo;
// 		m_pGridLotInfo.SetItemText(2, 1, st_lot_info[m_nWorkNum].strPartNo);
// 	}
}
