// DialogBarcode.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "DialogBarcode.h"
#include "afxdialogex.h"
#include "PublicFunction.h"
#include "Variable.h"
#include ".\\Ctrl\\KeyBoard.h"
#include "Barcode_1D.h"
#include "Barcode_2D.h"
// CDialogBarcode 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialogBarcode, CDialog)

CDialogBarcode::CDialogBarcode(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogBarcode::IDD, pParent)
{
	
}

CDialogBarcode::~CDialogBarcode()
{
}

void CDialogBarcode::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_BARCODE_EXIT,							m_btnBarcodeExit);
	DDX_Control(pDX, IDC_BTN_BARCODE_RETRY,							m_btnBarcodeRetry);
	DDX_Control(pDX, IDC_MSG_BARCODE_IBM,							m_msgBarcodeIbm);
	DDX_Control(pDX, IDC_MSG_BARCODE_IBM_DATA,						m_msgBarcodeIbmData);
	DDX_Control(pDX, IDC_MSG_BARCODE_VENDOR,						m_msgBarcodeVendor);
	DDX_Control(pDX, IDC_MSG_BARCODE_VENDOR_DATA,					m_msgBarcodeVendorData);
	DDX_Control(pDX, IDC_MSG_BARCODE_WORK_SITE,						m_msgBarcodeWorkSite);
}


BEGIN_MESSAGE_MAP(CDialogBarcode, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_BARCODE_RETRY, &CDialogBarcode::OnClickedBtnBarcodeRetry)
	ON_BN_CLICKED(IDC_BTN_BARCODE_EXIT, &CDialogBarcode::OnClickedBtnBarcodeExit)
	ON_STN_CLICKED(IDC_MSG_BARCODE_IBM_DATA, &CDialogBarcode::OnStnClickedMsgBarcodeIbmData)
	ON_STN_CLICKED(IDC_MSG_BARCODE_VENDOR_DATA, &CDialogBarcode::OnStnClickedMsgBarcodeVendorData)
END_MESSAGE_MAP()


// CDialogBarcode 메시지 처리기입니다.

BOOL CDialogBarcode::Create()
{
	return CDialog::Create( CDialogBarcode::IDD );
}


void CDialogBarcode::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CDialog::OnTimer(nIDEvent);
}


BOOL CDialogBarcode::DestroyWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	return CDialog::DestroyWindow();
}


BOOL CDialogBarcode::OnInitDialog()
{

	CDialog::OnInitDialog();
	OnInitLabel();
	OnInitButton();
	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDialogBarcode::OnClickedBtnBarcodeRetry()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	st_other_info.nConfirmMsg = 1;

	if (st_handler_info.cWndMain != NULL)
	{
		st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_BARCODE_BOX_DELETE_REQ, 0);
	}
}


void CDialogBarcode::OnClickedBtnBarcodeExit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	st_other_info.nConfirmMsg = 0;
//	CDialog::OnOK();
	if (st_handler_info.cWndMain != NULL)
	{
		st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_BARCODE_BOX_DELETE_REQ, 0);
	}
}


void CDialogBarcode::OnInitLabel()
{
	//IBM
	//m_msgBarcodeIbm.ShowWindow(SW_SHOW);
	m_msgBarcodeIbm.SetFont(clsFunc.m_pFont[1]);
	m_msgBarcodeIbm.SetWindowText(_T("IBM Barcode"));
	m_msgBarcodeIbm.SetCenterText();
	m_msgBarcodeIbm.SetColor(WHITE_C);
	m_msgBarcodeIbm.SetGradientColor(ORANGE_C);
	m_msgBarcodeIbm.SetTextColor(BLACK_C);

	//m_msgBarcodeIbmData.ShowWindow(SW_SHOW);
	m_msgBarcodeIbmData.SetFont(clsFunc.m_pFont[1]);
	//m_msgBarcodeIbmData.SetWindowText(_T("-"));
	m_msgBarcodeIbmData.SetCenterText();
	m_msgBarcodeIbmData.SetColor(WHITE_C);
	m_msgBarcodeIbmData.SetGradientColor(WHITE_C);
	m_msgBarcodeIbmData.SetTextColor(BLACK_C);

	//SAM
	//m_msgBarcodeVendor.ShowWindow(SW_SHOW);
	m_msgBarcodeVendor.SetFont(clsFunc.m_pFont[1]);
	m_msgBarcodeVendor.SetWindowText(_T("Vendor Barcode"));
	m_msgBarcodeVendor.SetCenterText();
	m_msgBarcodeVendor.SetColor(WHITE_C);
	m_msgBarcodeVendor.SetGradientColor(ORANGE_C);
	m_msgBarcodeVendor.SetTextColor(BLACK_C);

	//m_msgBarcodeVendorData.ShowWindow(SW_SHOW);
	m_msgBarcodeVendorData.SetFont(clsFunc.m_pFont[1]);
	//m_msgBarcodeVendorData.SetWindowText(_T("-"));
	m_msgBarcodeVendorData.SetCenterText();
	m_msgBarcodeVendorData.SetColor(WHITE_C);
	m_msgBarcodeVendorData.SetGradientColor(WHITE_C);
	m_msgBarcodeVendorData.SetTextColor(BLACK_C);

	//WORK SITE 표시
	m_msgBarcodeWorkSite.ShowWindow(SW_SHOW);
	m_msgBarcodeWorkSite.SetFont(clsFunc.m_pFont[1]);
	//m_msgBarcodeWorkSite.SetWindowText(_T("-"));
	m_msgBarcodeWorkSite.SetCenterText();
	m_msgBarcodeWorkSite.SetColor(GREEN_C);
	m_msgBarcodeWorkSite.SetGradientColor(GREEN_C);
	m_msgBarcodeWorkSite.SetTextColor(BLACK_C);

	///////////
	if (m_nWorkSite == 0)//1D LD PLATE BARCODE
	{
		m_msgBarcodeIbm.ShowWindow(SW_SHOW);
		m_msgBarcodeIbmData.ShowWindow(SW_SHOW);
		m_msgBarcodeIbmData.SetWindowText(clsBcr1D.m_strBarcode[0]);

		m_msgBarcodeVendor.ShowWindow(SW_SHOW);
		m_msgBarcodeVendorData.ShowWindow(SW_SHOW);
		m_msgBarcodeVendorData.SetWindowText(clsBcr1D.m_strBarcode[1]);

		m_msgBarcodeWorkSite.SetWindowText(_T("LOAD PLATE 1D BARCODE"));
	}
	else if (m_nWorkSite == 1)//2D LD_BUFFER
	{
		m_msgBarcodeIbm.ShowWindow(SW_HIDE);
		m_msgBarcodeIbmData.ShowWindow(SW_HIDE);

		m_msgBarcodeVendor.ShowWindow(SW_SHOW);
		m_msgBarcodeVendorData.ShowWindow(SW_SHOW);
		m_msgBarcodeVendorData.SetWindowText(clsBcr2D.m_strBarcode[1]);

		m_msgBarcodeWorkSite.SetWindowText(_T("LOAD BUFF 2D BARCODE"));
	}
	else if (m_nWorkSite == 2)//2D ULD_BUFFER
	{
		m_msgBarcodeIbm.ShowWindow(SW_HIDE);
		m_msgBarcodeIbmData.ShowWindow(SW_HIDE);

		m_msgBarcodeVendor.ShowWindow(SW_SHOW);
		m_msgBarcodeVendorData.ShowWindow(SW_SHOW);
		m_msgBarcodeVendorData.SetWindowText(clsBcr2D.m_strBarcode[1]);

		m_msgBarcodeWorkSite.SetWindowText(_T("UNLOAD BUFF 2D BARCODE"));
	}
	else
	{
		m_msgBarcodeIbm.ShowWindow(SW_SHOW);
		m_msgBarcodeIbmData.ShowWindow(SW_SHOW);
		m_msgBarcodeIbmData.SetWindowText(_T("-"));

		m_msgBarcodeVendor.ShowWindow(SW_SHOW);
		m_msgBarcodeVendorData.ShowWindow(SW_SHOW);
		m_msgBarcodeVendorData.SetWindowText(_T("-"));

		m_msgBarcodeWorkSite.SetWindowText(_T("-"));
	}
}
void CDialogBarcode::OnStnClickedMsgBarcodeIbmData()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strKey;

	KeyBoard(&strKey);

	clsBcr1D.m_strBarcode[0] = strKey;//1D LD PLATE
	m_msgBarcodeIbmData.SetWindowTextW(strKey);
}
void CDialogBarcode::OnStnClickedMsgBarcodeVendorData()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strKey;
	KeyBoard(&strKey);

	if (m_nWorkSite == 0)//1D LD PLATE
	{
		clsBcr1D.m_strBarcode[1] = strKey;
	}
	else if (m_nWorkSite == 1)//2D LD Buff
	{
		clsBcr2D.m_strBarcode[1] = strKey;
	}
	else if (m_nWorkSite == 2)//2D ULD Buff
	{
		clsBcr2D.m_strBarcode[1] = strKey;
	}
	
	m_msgBarcodeVendorData.SetWindowTextW(strKey);
}
void CDialogBarcode::OnInitButton()
{
	m_btnBarcodeExit.SetBitmaps(IDC_BTN_BARCODE_EXIT, IDB_BITMAP_EXIT, WINDOW_DN, IDB_BITMAP_EXIT, WINDOW_UP);
	m_btnBarcodeExit.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnBarcodeExit.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnBarcodeExit.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnBarcodeExit.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnBarcodeExit.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnBarcodeExit.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnBarcodeExit.SetFont(clsFunc.m_pFont[3]);
	m_btnBarcodeExit.SetTooltipText(_T("Exit"));

	m_btnBarcodeRetry.SetBitmaps(IDC_BTN_BARCODE_RETRY, IDB_BITMAP_GEAR_DN, WINDOW_DN, IDB_BITMAP_GEAR_UP, WINDOW_UP);
	m_btnBarcodeRetry.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnBarcodeRetry.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnBarcodeRetry.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnBarcodeRetry.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnBarcodeRetry.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnBarcodeRetry.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnBarcodeRetry.SetFont(clsFunc.m_pFont[3]);
	m_btnBarcodeRetry.SetTooltipText(_T("Barcode Retry"));
}