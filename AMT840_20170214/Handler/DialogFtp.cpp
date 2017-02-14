// DialogFtp.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Handler.h"
#include "DialogFtp.h"
#include "afxdialogex.h"
#include "FtpClient.h"
#include "Variable.h"
#include "PublicFunction.h"
// CDialogFtp ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDialogFtp, CDialog)

CDialogFtp::CDialogFtp(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogFtp::IDD, pParent)
{
	m_bmGround.LoadBitmap(IDB_BITMAP_BACK_GROUND);
}

CDialogFtp::~CDialogFtp()
{
}

void CDialogFtp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DIRECTORY,	m_listRemote);
	DDX_Control(pDX, IDC_BTN_EXIT,			m_btnExit);
}


BEGIN_MESSAGE_MAP(CDialogFtp, CDialog)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BTN_EXIT, &CDialogFtp::OnClickedBtnExit)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_DIRECTORY, &CDialogFtp::OnDblclkListDirectory)
END_MESSAGE_MAP()


// CDialogFtp �޽��� ó�����Դϴ�.


BOOL CDialogFtp::OnEraseBkgnd(CDC* pDC)
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


BOOL CDialogFtp::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	OnInitButton();
	OnInitListRemote();
	OnFtpInfo();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CDialogFtp::OnInitListRemote()
{
	CRect rect;

	m_listRemote.GetClientRect(&rect);
	// �÷� �߰��ϴ°� 
	m_listRemote.InsertColumn(0, _T("File Name"), LVCFMT_LEFT, rect.Width() - 160);
	m_listRemote.InsertColumn(1, _T("Size"), LVCFMT_RIGHT, 60);
	m_listRemote.InsertColumn(2, _T("Date"), LVCFMT_LEFT, 80);
}

void CDialogFtp::OnUpdateRemote(CString strPath)
{
	CString strTemp;
	CString strDirectory;

	BOOL bContinue = TRUE;

	unsigned long size;

	CTime time;

	int bWorking;
	int index;

	m_listRemote.DeleteAllItems();

	if(strPath != _T("/"))
	{
		strTemp = _T("[..]");
		// �߰����� �߰��� index �� �޾ƿ�
		index = m_listRemote.InsertItem(0, strTemp);
	}
	//ã�� ȭ�� ����
	CFTPFileFind	finder(m_pFtp.m_pConnection);
	bWorking = finder.FindFile(strPath);

	while(bWorking)
	{
		//FTP ������ ���ϸ��� ����
		bWorking = finder.FindNextFile();		// ȭ���ϳ� ã��

	// ȭ���̸� �߰�
		strTemp = finder.GetFileName();
		if(strTemp == _T(".")) continue;	// "." �̸� �������� �Ѿ
			
		if(finder.IsDirectory())
		{
			//���丮�� [ ] �� ����	
			strDirectory.Format(_T("[%s]"), strTemp);
		}
		else
		{
			strDirectory = strTemp;
		}
		// �߰����� �߰��� index �� �޾ƿ�
		index = m_listRemote.InsertItem(0, strDirectory);

	// ȭ��ũ�� �߰�
		size = finder.GetLength();
		strTemp.Format(_T("%d"),size);
		m_listRemote.SetItemText(index, 1, strTemp);

	// ȭ�ϳ�¥ �߰�
		finder.GetLastWriteTime(time);
		strTemp = time.Format("%y%m%d %H:%M");
		m_listRemote.SetItemText(index, 2, strTemp);

	}

	UpdateData(FALSE);	// ȭ�鿡 �ִ°�����.....
}

void CDialogFtp::OnFtpInfo()
{
	if (m_pFtp.Open(st_client_info[FTP_NETWORK].strIp, 
				    st_basic_info.strFtpUser,
				    st_basic_info.strFtpPass,
				    st_client_info[FTP_NETWORK].nPort))
	{
		m_strPathRemote = m_pFtp.RCD();

		OnUpdateRemote(m_strPathRemote);
	}
}

void CDialogFtp::OnClickedBtnExit()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pFtp.Close();

	CDialog::OnOK();
}


void CDialogFtp::OnDblclkListDirectory(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	POSITION pos;

	CString strTemp;
	CString strDirectory = m_strPathRemote;

	pos = m_listRemote.GetFirstSelectedItemPosition();	// ��ġ�� ����
	
	if(pos == NULL) return;	// �ƹ��͵� ���� �ȉ����� �׳� return
	
	pos = pos -1;		// 1 �������
	strTemp = m_listRemote.GetItemText((int)pos,0);				// ȭ�ϸ��� ����
	
	// ���丮�̸�
	if(strTemp.GetAt(0) == '[')
	{
		// ������ '[' �� ']' ����  �ֳ��ϸ� ���丮���� [ ] �� �ѷ��ο� �����Ƿ�
		strTemp = strTemp.Left(strTemp.GetLength() -1);
		strTemp = strTemp.Right(strTemp.GetLength() -1);

		// ��ü ���丮 �̸� ���ϱ�
		if(strTemp == _T(".."))
		{
		// ".."�� ���� �� ���� ���� �ö󰡴� ���
			strDirectory = m_strPathRemote.Left(m_strPathRemote.ReverseFind('/'));	// ������ '/' ������ ����
			
			if(strDirectory == _T(""))
			{
				strDirectory += _T("/");
			}
			m_strPathRemote = strDirectory;
		}
		else
		{
			strDirectory += _T("/");
			strDirectory += strTemp;

			strDirectory.Format(_T("%s%s%s"), m_strPathRemote, _T("/"), strTemp);
			m_strPathRemote = strDirectory;
		}

		//���丮 �̵�
		if(!m_pFtp.RCD(m_strPathRemote))
		{
			m_strPathRemote = m_pFtp.RCD();
			AfxMessageBox(_T("���丮 �̵��� �Ұ����Դϴ�."));

			return;
		}

		//���� ���丮�� ȭ�鿡 ǥ��
		m_strPathRemote = m_pFtp.RCD();

		OnUpdateRemote(m_strPathRemote);
	}	// end of if

	*pResult = 0;
}

void CDialogFtp::OnInitButton()
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
}