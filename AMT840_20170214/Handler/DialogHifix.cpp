// DialogHifix.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Handler.h"
#include "DialogHifix.h"
#include "afxdialogex.h"
#include "Variable.h"
#include "PublicFunction.h"
// CDialogHifix ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDialogHifix, CDialog)

CDialogHifix::CDialogHifix(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogHifix::IDD, pParent)
{
	m_bmGround.LoadBitmap(IDB_BITMAP_BACK_GROUND); 
}

CDialogHifix::~CDialogHifix()
{
}

void CDialogHifix::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRID_HIFIX,						m_pGridHifix);
	DDX_Control(pDX, IDC_BTN_HIFIX_EXIT,					m_btnExit);
}


BEGIN_MESSAGE_MAP(CDialogHifix, CDialog)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BTN_HIFIX_EXIT, &CDialogHifix::OnClickedBtnHifixExit)
	ON_NOTIFY(NM_CLICK, IDC_GRID_HIFIX, OnCellClick)
END_MESSAGE_MAP()


// CDialogHifix �޽��� ó�����Դϴ�.


BOOL CDialogHifix::OnEraseBkgnd(CDC* pDC)
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

//	return CFormView::OnEraseBkgnd(pDC);
	return true;
}


BOOL CDialogHifix::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	
	OnInitHifix();
	OnInitGrid();
	OnInitButton();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CDialogHifix::OnClickedBtnHifixExit()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CDialog::OnOK();
}


void CDialogHifix::OnInitGrid()
{
	int   i, j;
	int	  nMaxRow, nMaxCol;

	CString strTmp;

	nMaxRow = m_nHifix; // unloader tray y count
	nMaxCol = 1;

	m_pGridHifix.SetFrameFocusCell(FALSE);
	m_pGridHifix.SetTrackFocusCell(FALSE);
	m_pGridHifix.EnableSelection(FALSE);

	m_pGridHifix.SetGridLineColor(BLACK_C);
	m_pGridHifix.SetGridLines(1);

	m_pGridHifix.SetRowCount(nMaxRow);
	m_pGridHifix.SetColumnCount(nMaxCol);

	m_pGridHifix.SetFixedRowCount(0);
	m_pGridHifix.SetFixedColumnCount(0);
	m_pGridHifix.SetFixedBkColor(RGB(0,0,200));
	m_pGridHifix.SetFixedBkColor(RGB(200,200,255));
	m_pGridHifix.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<nMaxRow; i++) 
	{
		m_pGridHifix.SetRowHeight(i, 45);

		for (j=0; j<nMaxCol; j++) 
		{
			m_pGridHifix.SetColumnWidth(j, 255);

			m_pGridHifix.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridHifix.SetItemState(i, j, GVIS_READONLY);
			m_pGridHifix.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
		}

		m_pGridHifix.SetItemText(i, 0, m_strHifix[i]);
	}

	m_pGridHifix.Invalidate(FALSE);
}


void CDialogHifix::OnInitButton()
{
	m_btnExit.SetBitmaps(IDC_BTN_HIFIX_EXIT, IDB_BITMAP_EXIT, WINDOW_DN, IDB_BITMAP_EXIT, WINDOW_UP);
	m_btnExit.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnExit.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnExit.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnExit.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnExit.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnExit.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnExit.SetFont(clsFunc.m_pFont[3]);
	m_btnExit.SetTooltipText(_T("Exit"));
}

void CDialogHifix::OnInitHifix()
{
	CString strLoadFile = _T("C:\\AMT840\\Hifix.TXT");
    CString	strTemp;
	CString strHead;
	CString strData;

	TCHAR chData[100];

	int i;

	m_nHifix = 0;

	GetPrivateProfileString(_T("I-TEST"), _T("Total"), _T("0"), chData, sizeof(chData), strLoadFile);
	strTemp.Format(_T("%s"), chData);

	m_nHifix = _wtoi(strTemp);

	if (m_nHifix > 0)
	{
		for (i=0; i<m_nHifix; i++)
		{
			strHead.Format(_T("%d"), i+1);
			GetPrivateProfileString(_T("I-TEST"), strHead, _T("0"), chData, sizeof(chData), strLoadFile);
			strData.Format(_T("%s"), chData);
			
			m_strHifix[i] = strData;
		}
	}
}

void CDialogHifix::OnCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	int nRow, nCol;
	int i;

	CString strTmp;
	CString strMsg;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	if (nRow < 0) return;

    m_strHifixData		= m_strHifix[nRow];

	for (i=0; i<m_nHifix; i++) 
	{
		m_pGridHifix.SetItemBkColour(i, 0, WHITE_C, CLR_DEFAULT);
	}
	m_pGridHifix.SetItemBkColour(nRow, 0, GREEN_C, BLACK_C);

	m_pGridHifix.Invalidate(FALSE);
}