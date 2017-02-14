// DialogPcbPicker.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Handler.h"
#include "DialogPcbPicker.h"
#include "afxdialogex.h"
#include "Variable.h"
#include "PublicFunction.h"

// CDialogPcbPicker ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDialogPcbPicker, CDialog)

CDialogPcbPicker::CDialogPcbPicker(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogPcbPicker::IDD, pParent)
{
	m_bmGround.LoadBitmap(IDB_BITMAP_BACK_GROUND);

	/*m_nSite = THD_LD_ROBOT_SITE;*/
}

CDialogPcbPicker::~CDialogPcbPicker()
{
}

void CDialogPcbPicker::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MSG_PCB_DIALOG_TITLE,			m_msgTitle);
	DDX_Control(pDX, IDC_CUSTOM_PCB_DIALOG_ITEM_DATA,	m_pGridItemData);
	DDX_Control(pDX, IDC_BTN_PCB_DIALOG_EXIT,			m_btnExit);
}


BEGIN_MESSAGE_MAP(CDialogPcbPicker, CDialog)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BTN_PCB_DIALOG_EXIT, &CDialogPcbPicker::OnBnClickedBtnPcbDialogExit)
END_MESSAGE_MAP()


// CDialogPcbPicker �޽��� ó�����Դϴ�.


BOOL CDialogPcbPicker::OnInitDialog()
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

BOOL CDialogPcbPicker::Create()
{
	return CDialog::Create( CDialogPcbPicker::IDD );
}

void CDialogPcbPicker::OnInitButton()
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

void CDialogPcbPicker::OnInitDigit()
{
}

void CDialogPcbPicker::OnInitGroupBox()
{
}

void CDialogPcbPicker::OnInitLabel()
{
	m_msgTitle.ShowWindow(SW_SHOW);

	m_msgTitle.SetFont(clsFunc.m_pFont[1]);
	if (m_nSite == THD_LD_BUFF)
	{
		m_msgTitle.SetWindowText(_T("Load Buffer"));
	}
	else if (m_nSite == THD_ULD_BUFF)
	{
		m_msgTitle.SetWindowText(_T("UnLoad Buffer"));
	}
	else if (m_nSite == THD_TEST_RBT)
	{
		m_msgTitle.SetWindowText(_T("Test Site Robot"));
	}

	m_msgTitle.SetCenterText();
	m_msgTitle.SetColor(WHITE_C);
	m_msgTitle.SetGradientColor(ORANGE_C);
	m_msgTitle.SetTextColor(BLACK_C);
}

BOOL CDialogPcbPicker::OnEraseBkgnd(CDC* pDC)
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

void CDialogPcbPicker::OnInitGridItem()
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

void CDialogPcbPicker::OnInitGridItemData()
{
	int   i, j;
	int	  nMaxRow, nMaxCol;
	int   nX, nY;

	CString strTemp;

	CStringArray options;

	nX		= 4;
	nY		= 1;

	nMaxRow = 7; // unloader tray y count
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

	for (i=0; i<nY; i++) 
	{
		m_pGridItemData.MergeCells( 1, 0,  6, 0);
		m_pGridItemData.SetItemBkColour( 1, 0, BLUE_D, WHITE_C);
		m_pGridItemData.SetItemFont( 1, 0, &clsFunc.OnLogFont(18));
		strTemp.Format(_T("%d"),i + 1);
		m_pGridItemData.SetItemText(1, 0, strTemp);

		m_pGridItemData.SetItemText(1, 1, _T("LotNo"));
		m_pGridItemData.SetItemBkColour(1, 1,RGB(219, 229, 241), BLACK_C);

		m_pGridItemData.SetItemText(2, 1, _T("PartNo"));
		m_pGridItemData.SetItemBkColour(2, 1, RGB(219, 229, 241), BLACK_C);

		m_pGridItemData.SetItemText(3, 1, _T("Serial"));
		m_pGridItemData.SetItemBkColour(3, 1, RGB(219, 229, 241), BLACK_C);

		m_pGridItemData.SetItemText(4, 1, _T("1D"));
		m_pGridItemData.SetItemBkColour(4, 1, RGB(219, 229, 241), BLACK_C);

		m_pGridItemData.SetItemText(5, 1, _T("1D"));
		m_pGridItemData.SetItemBkColour(5, 1, RGB(219, 229, 241), BLACK_C);

		m_pGridItemData.SetItemText(6, 1, _T("2D"));
		m_pGridItemData.SetItemBkColour(6, 1, RGB(219, 229, 241), BLACK_C);

		for (j=0; j<nX; j++) 
		{
			m_pGridItemData.SetItemText(1, j + 2, st_buffer_info[m_nSite].st_pcb_info[(nX * i)+ j].strLotNo);

			m_pGridItemData.SetItemText(2, j + 2, st_buffer_info[m_nSite].st_pcb_info[(nX * i)+ j].strPartNo);

			m_pGridItemData.SetItemText(3, j + 2, st_buffer_info[m_nSite].st_pcb_info[(nX * i)+ j].strSerialNo);

			m_pGridItemData.SetItemText(4, j + 2, st_buffer_info[m_nSite].st_pcb_info[(nX * i)+ j].strBarcode1D[0]);

			m_pGridItemData.SetItemText(5, j + 2, st_buffer_info[m_nSite].st_pcb_info[(nX * i)+ j].strBarcode1D[1]);

			// jtkim 20160929
			m_pGridItemData.SetItemText(6, j + 2, st_buffer_info[m_nSite].st_pcb_info[(nX * i)+ j].strBarcode2D[0]);
		}
	}
}

void CDialogPcbPicker::OnBnClickedBtnPcbDialogExit()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (st_handler_info.cWndMain != NULL)
	{
		st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_PCB_PICKER_DELETE_MSG, 0);
	}
}
