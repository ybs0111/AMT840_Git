// DialogTesterInterface.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "DialogTesterInterface.h"
#include "afxdialogex.h"
#include "Variable.h"
#include "PublicFunction.h"

// CDialogPcbInterface 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialogTesterInterface, CDialog)

CDialogTesterInterface::CDialogTesterInterface(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogTesterInterface::IDD, pParent)
{
	m_bmGround.LoadBitmap(IDB_BITMAP_BACK_GROUND);

	m_nSite = 0;
	m_nCnt =0;
	m_nNum = 0;
}

CDialogTesterInterface::~CDialogTesterInterface()
{
}

void CDialogTesterInterface::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MSG_PCB_DIALOG_TITLE,				m_msgTitle);
	DDX_Control(pDX, IDC_CUSTOM_PCB_DIALOG_TOP_ITEM_DATA,	m_pGridItemData);
	DDX_Control(pDX, IDC_CUSTOM_PCB_DIALOG_BOTTOM_ITEM_DATA,m_pGridBottomItemData);
	DDX_Control(pDX, IDC_BTN_PCB_DIALOG_EXIT,				m_btnExit);
}


BEGIN_MESSAGE_MAP(CDialogTesterInterface, CDialog)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BTN_PCB_DIALOG_EXIT, &CDialogTesterInterface::OnBnClickedBtnPcbDialogExit)
END_MESSAGE_MAP()


// CDialogPcbBuffer 메시지 처리기입니다.
BOOL CDialogTesterInterface::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	OnInitButton();
	OnInitDigit();
	OnInitLabel();

	OnInitGridItemData();
	//OnInitGridBottomItemData();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CDialogTesterInterface::Create()
{
	return CDialog::Create( CDialogTesterInterface::IDD );
}

void CDialogTesterInterface::OnInitButton()
{
	m_btnExit.SetBitmaps(IDC_BTN_PCB_DIALOG_EXIT, IDB_BITMAP_EXIT, WINDOW_DN, IDB_BITMAP_EXIT, WINDOW_UP);
	m_btnExit.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnExit.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnExit.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnExit.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnExit.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnExit.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnExit.SetFont(clsFunc.m_pFont[3]);
	m_btnExit.SetTooltipText(_T("Exit"));
}

void CDialogTesterInterface::OnInitDigit()
{
}

void CDialogTesterInterface::OnInitGroupBox()
{
}

void CDialogTesterInterface::OnInitLabel()
{
	m_msgTitle.ShowWindow(SW_SHOW);
	m_msgTitle.SetFont(clsFunc.m_pFont[1]);
	if (m_nSite == 0)
	{
		//m_nRSite = THD_LEFT_TEST_SITE;
		m_msgTitle.SetWindowText(_T("Left Interface Site"));
	}
	else
	{
	//	m_nRSite = THD_RIGHT_TEST_SITE;
		m_msgTitle.SetWindowText(_T("Right Interface Site"));
	}
	m_msgTitle.SetCenterText();
	m_msgTitle.SetColor(WHITE_C);
	m_msgTitle.SetGradientColor(ORANGE_C);
	m_msgTitle.SetTextColor(BLACK_C);
}

BOOL CDialogTesterInterface::OnEraseBkgnd(CDC* pDC)
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

	return true; // 흰 부러쉬가 아닌 배경을 그려야 하기 때문에 true
}

void CDialogTesterInterface::OnInitGridItemData()
{
	int   i, j;
	int	  nMaxRow, nMaxCol;
	int   nX, nY;
	
	double dAve;

	CString strTemp;

	CStringArray options;

	//nX		= 4;
	nX		= 8;
	nY		= 8;

	nMaxRow = 25; // unloader tray y count
	nMaxCol = 6;
	
	
	m_pGridItemData.SetFrameFocusCell(FALSE);
	m_pGridItemData.SetTrackFocusCell(FALSE);
	m_pGridItemData.EnableSelection(FALSE);

	m_pGridItemData.SetGridLineColor(BLACK_C);
	m_pGridItemData.SetGridLines(1);

	m_pGridItemData.SetRowCount(nMaxRow);
	m_pGridItemData.SetColumnCount(nMaxCol);

	m_pGridItemData.SetFixedRowCount(0);
	m_pGridItemData.SetFixedColumnCount(0);
	m_pGridItemData.SetFixedBkColor(RGB(0,0,200));
	m_pGridItemData.SetFixedBkColor(RGB(200,200,255));
	m_pGridItemData.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<nMaxRow; i++) 
	{
		m_pGridItemData.SetRowHeight(i, 20);

		for (j=0; j<nMaxCol; j++) 
		{
			m_pGridItemData.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridItemData.SetItemState(i, j, GVIS_READONLY);
			m_pGridItemData.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
			m_pGridItemData.SetItemFont(i, j, &clsFunc.OnLogFont(12));

			if (j == 0)
			{
				m_pGridItemData.SetColumnWidth(j, 68);
			}
			else if (j == 1)
			{
				m_pGridItemData.SetColumnWidth(j, 68);
			}
			else
			{
				m_pGridItemData.SetColumnWidth(j, 179);
				if (i == 0)
				{
					m_pGridItemData.SetItemBkColour(i, j, BLUE_D, WHITE_C);
					strTemp.Format(_T("%d"),  (j - 1));
					m_pGridItemData.SetItemText(i, j, strTemp);
				}

			}
		}
	}

// 	m_pGridTopItemData.SetItemBkColour(0, 1, BLUE_D, WHITE_C);
// 	m_pGridTopItemData.MergeCells(0, 0,  24, 0);
// 	m_pGridTopItemData.SetItemBkColour(0, 0, BLUE_D, WHITE_C);
// 	m_pGridTopItemData.SetItemFont(0, 0, &clsFunc.OnLogFont(18));
	

// 	m_pGridTopItemData.SetItemBkColour(25, 1, BLUE_D, WHITE_C);
// 	m_pGridTopItemData.MergeCells(25, 0,  47, 0);
// 	m_pGridTopItemData.SetItemBkColour(25, 0, BLUE_D, WHITE_C);
// 	m_pGridTopItemData.SetItemFont(25, 0, &clsFunc.OnLogFont(18));
// 	m_pGridTopItemData.SetItemText(25, 0, _T("B"));

	// 1
	for (i= 0; i<nY; i++) 
	{
		//m_pGridItemData.SetItemText((i * 3) +1, 1, _T("LotNo"));
		//kwlee 2017.0919
		m_pGridItemData.SetItemText((i * 3) +1, 1, _T("SerialNo"));
		m_pGridItemData.SetItemBkColour((i * 3) + 1, 1,RGB(219, 229, 241), BLACK_C);

		m_pGridItemData.SetItemText((i * 3)+ 2, 1, _T("PartNo"));
		m_pGridItemData.SetItemBkColour((i * 3)  + 2, 1, RGB(219, 229, 241), BLACK_C);

		m_pGridItemData.SetItemText((i * 3)  + 3, 1, _T("Pass (%)"));
		m_pGridItemData.SetItemBkColour((i * 3) + 3, 1, RGB(219, 229, 241), BLACK_C);

		m_pGridItemData.SetItemBkColour(0, 1, BLUE_D, WHITE_C);
		m_pGridItemData.MergeCells((i * 3)+1, 0,  (i * 3)+3, 0);
		m_pGridItemData.SetItemBkColour((i * 3)+1, 0, BLUE_D, WHITE_C);
		m_pGridItemData.SetItemFont((i * 3)+1, 0, &clsFunc.OnLogFont(18));


		if (m_nSite == 0)
		{
			strTemp.Format(_T("%d"), 8 - m_nNum);
			m_pGridItemData.SetItemText((i * 3)+1, 0, strTemp);
			m_pGridItemData.SetItemText(0, 0, _T("LEFT"));
			m_pGridItemData.SetItemFont(0, 0, &clsFunc.OnLogFont(18));
		}
		else
		{
			strTemp.Format(_T("%d"),m_nNum +1);
			m_pGridItemData.SetItemText((i * 3)+1, 0, strTemp);
			m_pGridItemData.SetItemText(0, 0, _T("RIGHT"));
			m_pGridItemData.SetItemFont(0, 0, &clsFunc.OnLogFont(18));
		}

		//Data nx = 4;
		for (j=0; j<nX; j++) 
		{
			if (m_nSite == 0)
			{
				//strTemp.Format(_T("%s [%s]"), st_test_site_info[THD_TESTSITE_1 + j].st_pcb_info[j].strSerialNo, st_bd_info[THD_TESTSITE_1 + j][1].strBinHistory[j]);
				//kwlee 2017.0919
				if (st_test_site_info[THD_TESTSITE_1 + j].st_pcb_info[j].nEnable == YES)
				{
					strTemp.Format(_T("%s [%s]"), st_test_site_info[THD_TESTSITE_1 + j].st_pcb_info[j].strSerialNo, st_test_site_info[THD_TESTSITE_1 + j].st_pcb_info[j].strBin);	
				}
				else
				{
					strTemp.Format(_T("[Socket Off]"));
				}
				m_pGridItemData.SetItemText((i * 3) +1, j + 2, strTemp);
				
				m_pGridItemData.SetItemText((i * 3) + 2, j + 2, st_test_site_info[THD_TESTSITE_1 + j].st_pcb_info[j].strPartNo);

				dAve = 0.0;
				if (st_bd_info[m_nSite][1].nBdPassCnt[j] > 0)
				{
					dAve = ((double)st_bd_info[THD_TESTSITE_1 + j][1].nBdPassCnt[j] / (double)st_bd_info[THD_TESTSITE_1 + j][1].nBdTestCnt[j]) * (double)100;
				}
				strTemp.Format(_T("%.2f T:[%d]P:[%d]"), dAve, st_bd_info[m_nSite][1].nBdTestCnt[j], st_bd_info[m_nSite][1].nBdPassCnt[j]); 
				m_pGridItemData.SetItemText((i * 3) +3, j + 2, strTemp);
			}
			else
			{
				//strTemp.Format(_T("%s [%s]"), st_test_site_info[THD_TESTSITE_5 + j].st_pcb_info[j + 4].strLotNo, st_bd_info[THD_TESTSITE_5 + j][1].strBinHistory[j + 4]);
				//kwlee 2017.0919
				if (st_test_site_info[THD_TESTSITE_5 + j].st_pcb_info[j].nEnable == YES)
				{
					strTemp.Format(_T("%s [%s]"), st_test_site_info[THD_TESTSITE_5 + j].st_pcb_info[j].strSerialNo,st_test_site_info[THD_TESTSITE_5 + j].st_pcb_info[j].strBin);
				}
				else
				{
					strTemp.Format(_T("[Socket Off]"));
				}
				m_pGridItemData.SetItemText((i * 3) +1, j + 2, strTemp);
				
				//m_pGridItemData.SetItemText((i * 3) + 2, j + 2, st_test_site_info[THD_TESTSITE_5 + j].st_pcb_info[j + 4].strPartNo);
				m_pGridItemData.SetItemText((i * 3) + 2, j + 2, st_test_site_info[THD_TESTSITE_5 + j].st_pcb_info[j].strPartNo);

				dAve = 0.0;
				if (st_bd_info[m_nSite][1].nBdPassCnt[j] > 0)
				{
					//dAve = ((double)st_bd_info[m_nSite][1].nBdPassCnt[j + 4] / (double)st_bd_info[m_nSite][1].nBdTestCnt[j + 4]) * (double)100;
					dAve = ((double)st_bd_info[m_nSite][1].nBdPassCnt[j] / (double)st_bd_info[m_nSite][1].nBdTestCnt[j]) * (double)100;
				}
				strTemp.Format(_T("%.2f T:[%d]P:[%d]"), dAve, st_bd_info[m_nSite][1].nBdTestCnt[j], st_bd_info[m_nSite][1].nBdPassCnt[j]); 
				m_pGridItemData.SetItemText((i * 3) +3, j + 2, strTemp);
			}
		}
		m_nNum++;
	}
	UpdateData(FALSE);
}

void CDialogTesterInterface::OnInitGridBottomItemData()
{
	int   i, j;
	int	  nMaxRow, nMaxCol;
	int   nX, nY;

//	double dAve;

	CString strTemp;

	CStringArray options;

	nX		= 6;
	nY		= 1;

	nMaxRow = 10; // unloader tray y count
	nMaxCol = 8;
	
	m_pGridBottomItemData.SetFrameFocusCell(FALSE);
	m_pGridBottomItemData.SetTrackFocusCell(FALSE);
	m_pGridBottomItemData.EnableSelection(FALSE);

	m_pGridBottomItemData.SetGridLineColor(BLACK_C);
	m_pGridBottomItemData.SetGridLines(1);

	m_pGridBottomItemData.SetRowCount(nMaxRow);
	m_pGridBottomItemData.SetColumnCount(nMaxCol);

	m_pGridBottomItemData.SetFixedRowCount(0);
	m_pGridBottomItemData.SetFixedColumnCount(0);
	m_pGridBottomItemData.SetFixedBkColor(RGB(0,0,200));
	m_pGridBottomItemData.SetFixedBkColor(RGB(200,200,255));
	m_pGridBottomItemData.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<nMaxRow; i++) 
	{
		m_pGridBottomItemData.SetRowHeight(i, 20);

		for (j=0; j<nMaxCol; j++) 
		{
			m_pGridBottomItemData.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridBottomItemData.SetItemState(i, j, GVIS_READONLY);
			m_pGridBottomItemData.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
			m_pGridBottomItemData.SetItemFont(i, j, &clsFunc.OnLogFont(12));

			if (j == 0)
			{
				m_pGridBottomItemData.SetColumnWidth(j, 68);
			}
			else if (j == 1)
			{
				m_pGridBottomItemData.SetColumnWidth(j, 68);
			}
			else
			{
				m_pGridBottomItemData.SetColumnWidth(j, 175);
				if (i == 0)
				{
// 					m_pGridBottomItemData.SetItemBkColour(i, j, BLUE_D, WHITE_C);
// 					strTemp.Format(_T("%d"), (m_nSite * 24) + j - 1);
// 					m_pGridBottomItemData.SetItemText(i, j, strTemp);
				}
				else if (i == 5)
				{
// 					m_pGridBottomItemData.SetItemBkColour(i, j, BLUE_D, WHITE_C);
// 					strTemp.Format(_T("%d"), (m_nSite * 24) + j + 5);
// 					m_pGridBottomItemData.SetItemText(i, j, strTemp);
				}
			}
		}
	}

	m_pGridBottomItemData.SetItemBkColour(0, 1, BLUE_D, WHITE_C);

	m_pGridBottomItemData.MergeCells(0, 0,  9, 0);
	m_pGridBottomItemData.SetItemBkColour(0, 0, BLUE_D, WHITE_C);
	m_pGridBottomItemData.SetItemFont(0, 0, &clsFunc.OnLogFont(18));
	m_pGridBottomItemData.SetItemText(0, 0, _T("B"));

	for (i=0; i<nY; i++) 
	{	
		m_pGridBottomItemData.SetItemText((i * 4) + 1, 1, _T("LotNo"));
		m_pGridBottomItemData.SetItemBkColour((i * 4) + 1, 1,RGB(219, 229, 241), BLACK_C);

		m_pGridBottomItemData.SetItemText((i * 4) + 2, 1, _T("PartNo"));
		m_pGridBottomItemData.SetItemBkColour((i * 4) + 2, 1, RGB(219, 229, 241), BLACK_C);

		m_pGridBottomItemData.SetItemText((i * 4) + 3, 1, _T("Serial"));
		m_pGridBottomItemData.SetItemBkColour((i * 4) + 3, 1, RGB(219, 229, 241), BLACK_C);

		m_pGridBottomItemData.SetItemText((i * 4) + 4, 1, _T("Pass (%)"));
		m_pGridBottomItemData.SetItemBkColour((i * 4) + 4, 1, RGB(219, 229, 241), BLACK_C);

		m_pGridBottomItemData.SetItemText((i * 4) + 6, 1, _T("LotNo"));
		m_pGridBottomItemData.SetItemBkColour((i * 4) + 6, 1,RGB(219, 229, 241), BLACK_C);

		m_pGridBottomItemData.SetItemText((i * 4) + 7, 1, _T("PartNo"));
		m_pGridBottomItemData.SetItemBkColour((i * 4) + 7, 1, RGB(219, 229, 241), BLACK_C);

		m_pGridBottomItemData.SetItemText((i * 4) + 8, 1, _T("Serial"));
		m_pGridBottomItemData.SetItemBkColour((i * 4) + 8, 1, RGB(219, 229, 241), BLACK_C);

		m_pGridBottomItemData.SetItemText((i * 4) + 9, 1, _T("Pass (%)"));
		m_pGridBottomItemData.SetItemBkColour((i * 4) + 9, 1, RGB(219, 229, 241), BLACK_C);


// 		for (j=0; j<nX; j++) 
// 		{
// 			strTemp.Format(_T("%s [%s]"), st_test_site_info[THD_LEFT_TEST_SITE + m_nSite][0].st_pcb_info[j].strLotNo, st_bd_info[m_nSite][0].strBinHistory[j]);
// 			//m_pGridBottomItemData.SetItemText((i * 4) + 1, j + 2, st_test_site_info[THD_LEFT_TEST_SITE + m_nSite][0].st_pcb_info[j].strLotNo);
// 			m_pGridBottomItemData.SetItemText((i * 4) + 1, j + 2, strTemp);
// 			m_pGridBottomItemData.SetItemText((i * 4) + 2, j + 2, st_test_site_info[THD_LEFT_TEST_SITE + m_nSite][0].st_pcb_info[j].strPartNo);
// 			m_pGridBottomItemData.SetItemText((i * 4) + 3, j + 2, st_test_site_info[THD_LEFT_TEST_SITE + m_nSite][0].st_pcb_info[j].strSerialNo);
// 
// 			dAve = 0.0;
// 			if (st_bd_info[m_nSite][0].nBdPassCnt[j] > 0)
// 			{
// 				dAve = ((double)st_bd_info[m_nSite][0].nBdPassCnt[j] / (double)st_bd_info[m_nSite][0].nBdTestCnt[j]) * (double)100;
// 			}
// 			//strTemp.Format(_T("%.2f"), dAve); 
// 			strTemp.Format(_T("%.2f T:[%d]P:[%d]"), dAve, st_bd_info[m_nSite][0].nBdTestCnt[j], st_bd_info[m_nSite][0].nBdPassCnt[j]); 
// 			m_pGridBottomItemData.SetItemText((i * 4) + 4, j + 2, strTemp);
// 
// 			strTemp.Format(_T("%s [%s]"), st_test_site_info[THD_LEFT_TEST_SITE + m_nSite][0].st_pcb_info[j + 6].strLotNo, st_bd_info[m_nSite][0].strBinHistory[j + 6]);
// 			//m_pGridBottomItemData.SetItemText((i * 4) + 6, j + 2, st_test_site_info[THD_LEFT_TEST_SITE + m_nSite][0].st_pcb_info[j + 6].strLotNo);
// 			m_pGridBottomItemData.SetItemText((i * 4) + 6, j + 2, strTemp);
// 			m_pGridBottomItemData.SetItemText((i * 4) + 7, j + 2, st_test_site_info[THD_LEFT_TEST_SITE + m_nSite][0].st_pcb_info[j + 6].strPartNo);
// 			m_pGridBottomItemData.SetItemText((i * 4) + 8, j + 2, st_test_site_info[THD_LEFT_TEST_SITE + m_nSite][0].st_pcb_info[j + 6].strSerialNo);
// 
// 			dAve = 0.0;
// 			if (st_bd_info[m_nSite][0].nBdPassCnt[j + 6] > 0)
// 			{
// 				dAve = ((double)st_bd_info[m_nSite][0].nBdPassCnt[j + 6] / (double)st_bd_info[m_nSite][0].nBdTestCnt[j + 6]) * (double)100;
// 			}
// 			//strTemp.Format(_T("%.2f"), dAve); 
// 			strTemp.Format(_T("%.2f T:[%d]P:[%d]"), dAve, st_bd_info[m_nSite][0].nBdTestCnt[j + 6], st_bd_info[m_nSite][0].nBdPassCnt[j + 6]); 
// 			m_pGridBottomItemData.SetItemText((i * 4) + 9, j + 2, strTemp);
// 		}
	}

	m_pGridBottomItemData.Invalidate(FALSE);
}

void CDialogTesterInterface::OnBnClickedBtnPcbDialogExit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (st_handler_info.cWndMain != NULL)
	{
		st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_PCB_INTER_DELETE_MSG, 0);
	}
}

void CDialogTesterInterface::OnDisplayData()
{
	int i, j;
	int nY, nX;

//	double dAve;

	CString strTemp;

	nY = 1;
	nX = 6;

	for (i=0; i<nY; i++) 
	{	
		for (j=0; j<nX; j++) 
		{
// 			strTemp.Format(_T("%s [%s]"), st_test_site_info[THD_LEFT_TEST_SITE + m_nSite][0].st_pcb_info[j].strLotNo, st_bd_info[m_nSite][0].strBinHistory[j]);
// 			//m_pGridBottomItemData.SetItemText((i * 4) + 1, j + 2, st_test_site_info[THD_LEFT_TEST_SITE + m_nSite][0].st_pcb_info[j].strLotNo);
// 			m_pGridBottomItemData.SetItemText((i * 4) + 1, j + 2, strTemp);
// 			m_pGridBottomItemData.SetItemText((i * 4) + 2, j + 2, st_test_site_info[THD_LEFT_TEST_SITE + m_nSite][0].st_pcb_info[j].strPartNo);
// 			m_pGridBottomItemData.SetItemText((i * 4) + 3, j + 2, st_test_site_info[THD_LEFT_TEST_SITE + m_nSite][0].st_pcb_info[j].strSerialNo);
// 
// 			dAve = 0.0;
// 			if (st_bd_info[m_nSite][0].nBdPassCnt[j] > 0)
// 			{
// 				dAve = ((double)st_bd_info[m_nSite][0].nBdPassCnt[j] / (double)st_bd_info[m_nSite][0].nBdTestCnt[j]) * (double)100;
// 			}
// 			//strTemp.Format(_T("%.2f"), dAve); 
// 			strTemp.Format(_T("%.2f T:[%d]P:[%d]"), dAve, st_bd_info[m_nSite][0].nBdTestCnt[j], st_bd_info[m_nSite][0].nBdPassCnt[j]); 
// 			m_pGridBottomItemData.SetItemText((i * 4) + 4, j + 2, strTemp);
// 
// 			strTemp.Format(_T("%s [%s]"), st_test_site_info[THD_LEFT_TEST_SITE + m_nSite][0].st_pcb_info[j + 6].strLotNo, st_bd_info[m_nSite][0].strBinHistory[j + 6]);
// 			//m_pGridBottomItemData.SetItemText((i * 4) + 6, j + 2, st_test_site_info[THD_LEFT_TEST_SITE + m_nSite][0].st_pcb_info[j + 6].strLotNo);
// 			m_pGridBottomItemData.SetItemText((i * 4) + 6, j + 2, strTemp);
// 			m_pGridBottomItemData.SetItemText((i * 4) + 7, j + 2, st_test_site_info[THD_LEFT_TEST_SITE + m_nSite][0].st_pcb_info[j + 6].strPartNo);
// 			m_pGridBottomItemData.SetItemText((i * 4) + 8, j + 2, st_test_site_info[THD_LEFT_TEST_SITE + m_nSite][0].st_pcb_info[j + 6].strSerialNo);
// 
// 			dAve = 0.0;
// 			if (st_bd_info[m_nSite][0].nBdPassCnt[j + 6] > 0)
// 			{
// 				dAve = ((double)st_bd_info[m_nSite][0].nBdPassCnt[j + 6] / (double)st_bd_info[m_nSite][0].nBdTestCnt[j + 6]) * (double)100;
// 			}
// 			//strTemp.Format(_T("%.2f"), dAve); 
// 			strTemp.Format(_T("%.2f T:[%d]P:[%d]"), dAve, st_bd_info[m_nSite][0].nBdTestCnt[j + 6], st_bd_info[m_nSite][0].nBdPassCnt[j + 6]); 
// 			m_pGridBottomItemData.SetItemText((i * 4) + 9, j + 2, strTemp);
// 
// 			strTemp.Format(_T("%s [%s]"), st_test_site_info[THD_LEFT_TEST_SITE + m_nSite][1].st_pcb_info[j].strLotNo, st_bd_info[m_nSite][1].strBinHistory[j]);
// 			//m_pGridTopItemData.SetItemText((i * 4) + 1, j + 2, st_test_site_info[THD_LEFT_TEST_SITE + m_nSite][1].st_pcb_info[j].strLotNo);
// 			m_pGridTopItemData.SetItemText((i * 4) + 1, j + 2, strTemp/*st_test_site_info[THD_LEFT_TEST_SITE + m_nSite][1].st_pcb_info[j].strLotNo*/);
// 			m_pGridTopItemData.SetItemText((i * 4) + 2, j + 2, st_test_site_info[THD_LEFT_TEST_SITE + m_nSite][1].st_pcb_info[j].strPartNo);
// 			m_pGridTopItemData.SetItemText((i * 4) + 3, j + 2, st_test_site_info[THD_LEFT_TEST_SITE + m_nSite][1].st_pcb_info[j].strSerialNo);
// 
// 			dAve = 0.0;
// 			if (st_bd_info[m_nSite][1].nBdPassCnt[j] > 0)
// 			{
// 				dAve = ((double)st_bd_info[m_nSite][1].nBdPassCnt[j] / (double)st_bd_info[m_nSite][1].nBdTestCnt[j]) * (double)100;
// 			}
// 			//strTemp.Format(_T("%.2f"), dAve); 
// 			strTemp.Format(_T("%.2f T:[%d]P:[%d]"), dAve, st_bd_info[m_nSite][1].nBdTestCnt[j], st_bd_info[m_nSite][1].nBdPassCnt[j]); 
// 			m_pGridTopItemData.SetItemText((i * 4) + 4, j + 2, strTemp);
// 
// 			strTemp.Format(_T("%s [%s]"), st_test_site_info[THD_LEFT_TEST_SITE + m_nSite][1].st_pcb_info[j + 6].strLotNo, st_bd_info[m_nSite][1].strBinHistory[j + 6]);
// 			//m_pGridTopItemData.SetItemText((i * 4) + 6, j + 2, st_test_site_info[THD_LEFT_TEST_SITE + m_nSite][1].st_pcb_info[j + 6].strLotNo);
// 			m_pGridTopItemData.SetItemText((i * 4) + 6, j + 2, strTemp);
// 			m_pGridTopItemData.SetItemText((i * 4) + 7, j + 2, st_test_site_info[THD_LEFT_TEST_SITE + m_nSite][1].st_pcb_info[j + 6].strPartNo);
// 			m_pGridTopItemData.SetItemText((i * 4) + 8, j + 2, st_test_site_info[THD_LEFT_TEST_SITE + m_nSite][1].st_pcb_info[j + 6].strSerialNo);
// 			
// 			dAve = 0.0;
// 			if (st_bd_info[m_nSite][1].nBdPassCnt[j + 6] > 0)
// 			{
// 				dAve = ((double)st_bd_info[m_nSite][1].nBdPassCnt[j + 6] / (double)st_bd_info[m_nSite][1].nBdTestCnt[j + 6]) * (double)100;
// 			}
// 			//strTemp.Format(_T("%.2f"), dAve); 
// 			strTemp.Format(_T("%.2f T:[%d]P:[%d]"), dAve, st_bd_info[m_nSite][1].nBdTestCnt[j + 6], st_bd_info[m_nSite][1].nBdPassCnt[j + 6]); 
// 			m_pGridTopItemData.SetItemText((i * 4) + 9, j + 2, strTemp);
		}
	}

	m_pGridBottomItemData.Invalidate(FALSE);
	m_pGridItemData.Invalidate(FALSE);
}