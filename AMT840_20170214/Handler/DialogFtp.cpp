// DialogFtp.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "DialogFtp.h"
#include "afxdialogex.h"
#include "FtpClient.h"
#include "Variable.h"
#include "PublicFunction.h"
// CDialogFtp 대화 상자입니다.

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


// CDialogFtp 메시지 처리기입니다.


BOOL CDialogFtp::OnEraseBkgnd(CDC* pDC)
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


BOOL CDialogFtp::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	OnInitButton();
	OnInitListRemote();
	OnFtpInfo();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDialogFtp::OnInitListRemote()
{
	CRect rect;

	m_listRemote.GetClientRect(&rect);
	// 컬럼 추가하는것 
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
		// 추가한후 추가된 index 를 받아옴
		index = m_listRemote.InsertItem(0, strTemp);
	}
	//찾을 화일 지정
	CFTPFileFind	finder(m_pFtp.m_pConnection);
	bWorking = finder.FindFile(strPath);

	while(bWorking)
	{
		//FTP 서버의 파일명을 얻어옴
		bWorking = finder.FindNextFile();		// 화일하나 찾음

	// 화일이름 추가
		strTemp = finder.GetFileName();
		if(strTemp == _T(".")) continue;	// "." 이면 다음으로 넘어감
			
		if(finder.IsDirectory())
		{
			//디렉토리면 [ ] 로 감쌈	
			strDirectory.Format(_T("[%s]"), strTemp);
		}
		else
		{
			strDirectory = strTemp;
		}
		// 추가한후 추가된 index 를 받아옴
		index = m_listRemote.InsertItem(0, strDirectory);

	// 화일크기 추가
		size = finder.GetLength();
		strTemp.Format(_T("%d"),size);
		m_listRemote.SetItemText(index, 1, strTemp);

	// 화일날짜 추가
		finder.GetLastWriteTime(time);
		strTemp = time.Format("%y%m%d %H:%M");
		m_listRemote.SetItemText(index, 2, strTemp);

	}

	UpdateData(FALSE);	// 화면에 있는값으로.....
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pFtp.Close();

	CDialog::OnOK();
}


void CDialogFtp::OnDblclkListDirectory(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION pos;

	CString strTemp;
	CString strDirectory = m_strPathRemote;

	pos = m_listRemote.GetFirstSelectedItemPosition();	// 위치를 구함
	
	if(pos == NULL) return;	// 아무것도 선택 안됬으면 그냥 return
	
	pos = pos -1;		// 1 빼줘야함
	strTemp = m_listRemote.GetItemText((int)pos,0);				// 화일명을 구함
	
	// 디렉토리이면
	if(strTemp.GetAt(0) == '[')
	{
		// 양쪽의 '[' 와 ']' 제거  왜냐하면 디렉토리명은 [ ] 로 둘러싸여 있으므로
		strTemp = strTemp.Left(strTemp.GetLength() -1);
		strTemp = strTemp.Right(strTemp.GetLength() -1);

		// 전체 디렉토리 이름 구하기
		if(strTemp == _T(".."))
		{
		// ".."가 눌려 한 수준 위로 올라가는 경우
			strDirectory = m_strPathRemote.Left(m_strPathRemote.ReverseFind('/'));	// 오른쪽 '/' 까지를 없앰
			
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

		//디렉토리 이동
		if(!m_pFtp.RCD(m_strPathRemote))
		{
			m_strPathRemote = m_pFtp.RCD();
			AfxMessageBox(_T("디렉토리 이동이 불가능함니다."));

			return;
		}

		//현재 디렉토리를 화면에 표시
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