// DialogPcbTray.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Handler.h"
#include "DialogPcbTray.h"
#include "afxdialogex.h"
#include "Variable.h"
#include "PublicFunction.h"

// CDialogPcbTray ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDialogPcbTray, CDialog)

CDialogPcbTray::CDialogPcbTray(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogPcbTray::IDD, pParent)
{
	m_bmGround.LoadBitmap(IDB_BITMAP_BACK_GROUND);

	//m_nSite = THD_LD_ROTATOR_BUFF_SITE;
}

CDialogPcbTray::~CDialogPcbTray()
{
}

void CDialogPcbTray::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MSG_PCB_DIALOG_TITLE,			m_msgTitle);
	DDX_Control(pDX, IDC_CUSTOM_PCB_DIALOG_ITEM_DATA,	m_pGridItemData);
	DDX_Control(pDX, IDC_BTN_PCB_DIALOG_EXIT,			m_btnExit);
}


BEGIN_MESSAGE_MAP(CDialogPcbTray, CDialog)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BTN_PCB_DIALOG_EXIT, &CDialogPcbTray::OnBnClickedBtnPcbDialogExit)
END_MESSAGE_MAP()


// CDialogPcbBuffer �޽��� ó�����Դϴ�.
BOOL CDialogPcbTray::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	OnInitButton();
	OnInitDigit();
	OnInitLabel();

	OnInitGridItemData();
	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

BOOL CDialogPcbTray::Create()
{
	return CDialog::Create( CDialogPcbTray::IDD );
}

void CDialogPcbTray::OnInitButton()
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

void CDialogPcbTray::OnInitDigit()
{
}

void CDialogPcbTray::OnInitGroupBox()
{
}

void CDialogPcbTray::OnInitLabel()
{
	m_msgTitle.ShowWindow(SW_SHOW);
	m_msgTitle.SetFont(clsFunc.m_pFont[1]);

	//Load Tray
	if (m_nSite == THD_LD_TRAY_PLATE )
	{
		m_msgTitle.SetWindowText(_T("Load Tray"));
	}
	//UnLoad Tray(First)
	else if (m_nSite == THD_ULD_1_STACKER)
	{
		m_msgTitle.SetWindowText(_T("UnLoader Tray (First)"));
	}
	//UnLoad Tray(Second)
	else if (m_nSite == THD_ULD_2_STACKER)
	{
		m_msgTitle.SetWindowText(_T("UnLoader Tray (Second)"));
	}
	//Reject Tray(First)
	else if (m_nSite == THD_REJECT_OUT_1_STACKER)
	{
		m_msgTitle.SetWindowText(_T("Reject Tray(First)"));
	}
	//Reject Tray(Second)
	else if (m_nSite == THD_REJECT_OUT_2_STACKER)
	{
		m_msgTitle.SetWindowText(_T("Reject Tray(Second)"));
	}
// 	else if (m_nSite == THD_REJECT_TRAY_BUFF_SITE_1)
// 	{
// 		m_msgTitle.SetWindowText(_T("Reject Tray #1"));
// 	}
// 	else if (m_nSite == THD_REJECT_TRAY_BUFF_SITE_2)
// 	{
// 		m_msgTitle.SetWindowText(_T("Reject Tray #2"));
// 	}
// 	else if (m_nSite == THD_REJECT_TRAY_BUFF_SITE_3)
// 	{
// 		m_msgTitle.SetWindowText(_T("Reject Tray #3"));
// 	}
// 	else if (m_nSite == THD_REJECT_TRAY_BUFF_SITE_4)
// 	{
// 		m_msgTitle.SetWindowText(_T("Reject Tray #4"));
// 	}
// 	else if (m_nSite == THD_REJECT_TRAY_BUFF_SITE_5)
// 	{
// 		m_msgTitle.SetWindowText(_T("Reject Tray #5"));
// 	}
// 	else if (m_nSite == THD_PLATE_CV_BCR_TRAY_SITE)
// 	{
// 		m_msgTitle.SetWindowText(_T("Barcode Tray"));
// 	}
// 	else
// 	{
// 		m_msgTitle.SetWindowText(_T("-"));
// 	}
	m_msgTitle.SetCenterText();
	m_msgTitle.SetColor(WHITE_C);
	m_msgTitle.SetGradientColor(ORANGE_C);
	m_msgTitle.SetTextColor(BLACK_C);
}

BOOL CDialogPcbTray::OnEraseBkgnd(CDC* pDC)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	CDC  memDC;                     // �޸� DC
	CBitmap * pOldBitmap = NULL;    // m_BackBitmap���� ���ο� �׸��� DC�� �׸� ��, ���� �׷����� DC(��, Bitmap)�� ����.
	BITMAP bitmapInfo;              // �׸��� ����(m_BackBitmap)
	m_bmGround.GetBitmap(&bitmapInfo); // Bitmap ũ�� ����.

	memDC.CreateCompatibleDC(pDC);  // �޸� DC ����
	pOldBitmap = memDC.SelectObject(&m_bmGround);
	pDC->BitBlt(0, 0, bitmapInfo.bmWidth, bitmapInfo.bmHeight, &memDC, 0, 0, SRCCOPY);   // �޸� DC�� �׸��� �׸�

	memDC.SelectObject(pOldBitmap);
	memDC.DeleteDC();

	return true; // �� �η����� �ƴ� ����� �׷��� �ϱ� ������ true
}

void CDialogPcbTray::OnInitGridItem()
{
/*	int   i, j;
	int	  nMaxRow, nMaxCol;
	int   nRow;

	CString strTemp;

	CStringArray options;

	nMaxRow = 1; // unloader tray y count
	nMaxCol = 2;
	
	m_pGridItem.SetFrameFocusCell(FALSE);
	m_pGridItem.SetTrackFocusCell(FALSE);
	m_pGridItem.EnableSelection(FALSE);

	m_pGridItem.SetGridLineColor(BLACK_C);
	m_pGridItem.SetGridLines(1);

	m_pGridItem.SetRowCount(nMaxRow);
	m_pGridItem.SetColumnCount(nMaxCol);

	m_pGridItem.SetFixedRowCount(0);
	m_pGridItem.SetFixedColumnCount(0);
	m_pGridItem.SetFixedBkColor(RGB(0,0,200));
	m_pGridItem.SetFixedBkColor(RGB(200,200,255));
	m_pGridItem.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<nMaxRow; i++) 
	{
		m_pGridItem.SetRowHeight(i, 32);

		for (j=0; j<nMaxCol; j++) 
		{
			m_pGridItem.SetColumnWidth(j, 182);
				
			m_pGridItem.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
//			m_pGridItem.SetItemState(i, j, GVIS_READONLY);
			m_pGridItem.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
		}
	}

	m_pGridItem.SetCellType(0, 0, RUNTIME_CLASS(CGridCellCombo));
	m_pGridItem.SetItemBkColour(0, 0, WHITE_C, BLACK_C);
	m_pGridItem.SetItemFont(0, 0, &clsFunc.OnLogFont(18));
	m_pGridItem.SetItemText(0, 0, _T("LotNo"));

    options.Add(_T("LotNo"));
    options.Add(_T("PartNo"));
    options.Add(_T("Serial"));
	options.Add(_T("PPID"));
	options.Add(_T("WWN"));
	options.Add(_T("ARRAY"));
	options.Add(_T("C-Serial"));
	options.Add(_T("PSID"));
	options.Add(_T("BIN"));
	
    CGridCellCombo *pCell = (CGridCellCombo*) m_pGridItem.GetCell(0, 0);
    pCell->SetOptions(options);

	m_pGridItem.SetItemBkColour(0, 1, BLUE_D, WHITE_C);
	m_pGridItem.SetItemFont(0, 1, &clsFunc.OnLogFont(14));
	m_pGridItem.SetItemText(0, 1, _T("Data Change"));*/
}

void CDialogPcbTray::OnInitGridItemData()
{
	int   i, j,nTemp;
	int	  nMaxRow, nMaxCol;
	int   nX, nY;
	int	  nCenX, nCenY;
	CString strTemp;
	int   nWidth, nHeight;
	CStringArray options;

	CRect rect;
	CRect rtArea[2];

// 	nX		= 3;
// 	nY		= 2;

// 	nMaxRow = 11; // unloader tray y count
// 	nMaxCol = 5;
	

	nMaxRow = (st_recipe_info.nTrayY * 6) + 1; // unloader tray y count
	nMaxCol = st_recipe_info.nTrayX + 2;

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

	GetDlgItem(IDC_GROUP_TRAY)->GetWindowRect(rtArea[0]); // group box rect ���ϱ� 
	ScreenToClient(rtArea[0]); // window�󿡼� group box ��ǥ ���ϱ�
// 
 	nWidth	= rtArea[0].Width() - 105;
 	nHeight	= rtArea[0].Height() - 30;
// 
	nX	= nWidth / st_recipe_info.nTrayX;

	if (nMaxRow > 0)	
	{
		nY = nHeight / nMaxRow;
	}
	else	
	{
		nY = 1;
	}
// 
	nCenX = rtArea[0].left + ((rtArea[0].Width() - (nX * st_recipe_info.nTrayX + 70)) / 2) - 1;
	// group box���� grid ���Ϳ� ǥ���ϱ� ���� y��ǥ ���ϱ� 
	nCenY = rtArea[0].top + ((rtArea[0].Height() - (nY * nMaxRow)) / 2) - 1;
	// grid size ����Ͽ� �߽ɿ� �ٽ� �׸��� 
	GetDlgItem(IDC_CUSTOM_PCB_DIALOG_ITEM_DATA)->MoveWindow(nCenX, nCenY, (nX * st_recipe_info.nTrayX) + 75, (nY * nMaxRow) + 4, TRUE);

	for (i=0; i<nMaxRow; i++) 
	{
		m_pGridItemData.SetRowHeight(i, 20);

		for (j=0; j<nMaxCol; j++) 
		{
			m_pGridItemData.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridItemData.SetItemState(i, j, GVIS_READONLY);
			m_pGridItemData.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);

			if (j == 0)
			{
				m_pGridItemData.SetColumnWidth(j, 60);
			}
			else if (j == 1)
			{
				m_pGridItemData.SetColumnWidth(j, 60);
			}
			else
			{
				m_pGridItemData.SetColumnWidth(j, 180);
				if (i == 0)
				{
					m_pGridItemData.SetItemBkColour(i, j, BLUE_D, WHITE_C);
					strTemp.Format(_T("%d"), j - 1);
					m_pGridItemData.SetItemText(i, j, strTemp);
				}
			}
		}
	}

	m_pGridItemData.MergeCells(0, 0,  0, 1);
	m_pGridItemData.SetItemBkColour(0, 0, BLUE_D, WHITE_C);
	m_pGridItemData.SetItemText(0, 0, _T("-"));

	for (i=0; i<st_recipe_info.nTrayY; i++) 
	{
		m_pGridItemData.MergeCells((i * 6) + 1, 0,  ((i + 1) * 6), 0);
		m_pGridItemData.SetItemBkColour((i * 6) + 1, 0, BLUE_D, WHITE_C);

		nTemp = st_recipe_info.nTrayY - (i+1);
		strTemp.Format(_T("%d"),i + 1);
		m_pGridItemData.SetItemText((i * 6) + 1, 0, strTemp);

		m_pGridItemData.SetItemText((i * 6) + 1, 1, _T("LotNo"));
		m_pGridItemData.SetItemBkColour((i * 6) + 1, 1,RGB(219, 229, 241), BLACK_C);

		m_pGridItemData.SetItemText((i * 6) + 2, 1, _T("PartNo"));
		m_pGridItemData.SetItemBkColour((i * 6) + 2, 1, RGB(219, 229, 241), BLACK_C);

		m_pGridItemData.SetItemText((i * 6) + 3, 1, _T("Serial"));
		m_pGridItemData.SetItemBkColour((i * 6) + 3, 1, RGB(219, 229, 241), BLACK_C);

		m_pGridItemData.SetItemText((i * 6) + 4, 1, _T("1D"));
		m_pGridItemData.SetItemBkColour((i * 6) + 4, 1, RGB(219, 229, 241), BLACK_C);

		m_pGridItemData.SetItemText((i * 6) + 5, 1, _T("1D"));
		m_pGridItemData.SetItemBkColour((i * 6) + 5, 1, RGB(219, 229, 241), BLACK_C);

		m_pGridItemData.SetItemText((i * 6) + 6, 1, _T("2D"));
		m_pGridItemData.SetItemBkColour((i * 6) + 6, 1, RGB(219, 229, 241), BLACK_C);
		//m_pGridItemData.SetItemText((i * 4) + 4, 1, _T("Array"));
		//m_pGridItemData.SetItemBkColour((i * 4) + 4, 1, RGB(219, 229, 241), BLACK_C);
		
		for (j=0; j<st_recipe_info.nTrayX; j++) 
		{
			m_pGridItemData.SetItemText((i * 6) + 1, j + 2, st_tray_info[m_nSite].st_pcb_info[i][j].strLotNo);

			m_pGridItemData.SetItemText((i * 6) + 2, j + 2, st_tray_info[m_nSite].st_pcb_info[i][j].strPartNo);

			m_pGridItemData.SetItemText((i * 6) + 3, j + 2, st_tray_info[m_nSite].st_pcb_info[i][j].strSerialNo);

			m_pGridItemData.SetItemText((i * 6) + 4, j + 2, st_tray_info[m_nSite].st_pcb_info[i][j].strBarcode1D[0]);

			m_pGridItemData.SetItemText((i * 6) + 5, j + 2, st_tray_info[m_nSite].st_pcb_info[i][j].strBarcode1D[1]);

			// jtkim 20160929
			m_pGridItemData.SetItemText((i * 6) + 6, j + 2, st_tray_info[m_nSite].st_pcb_info[i][j].strBarcode2D[0]);

			// jtkim 20150316
			// 				strTemp.Format(_T("%d"), st_tray_info[m_nSite].st_pcb_info[nTemp][j].nRetestCnt);
			// 				m_pGridItemData.SetItemText((i * 4) + 4, j + 2, strTemp);
		}
	}

///////////////////////


}
void CDialogPcbTray::OnBnClickedBtnPcbDialogExit()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (st_handler_info.cWndMain != NULL)
	{
		st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_PCB_TRAY_DELETE_MSG, 0);
	}
}
