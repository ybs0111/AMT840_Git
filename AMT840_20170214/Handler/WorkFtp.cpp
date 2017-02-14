// WorkFtp.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "WorkFtp.h"
#include "afxdialogex.h"
#include "Dialog_KeyBoard.h"
#include "Dialog_KeyPad.h"
#include "Dialog_Message.h"
#include "PublicFunction.h"

#include "CtlBd_Variable.h"
#include "Dialog_Message.h"
#include "Dialog_KeyBoard.h"
#include "DialogFtp.h"
#include "ServerVnR.h"
#include "FtpClient.h"
#include "FtpComm.h"
#include "LogFromat.h"

// CWorkFtp 대화 상자입니다.
#define TM_GMS	100


IMPLEMENT_DYNAMIC(CWorkFtp, CDialog)

CWorkFtp::CWorkFtp(CWnd* pParent /*=NULL*/)
	: CDialog(CWorkFtp::IDD, pParent)
{

	m_bmGround.LoadBitmap(IDB_BITMAP_DLG_BACK_GROUND);
	m_nFtpMode = 0;
}

CWorkFtp::~CWorkFtp()
{
}

void CWorkFtp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GROUP_NETWORK_FTP,			m_groupFtp);
	DDX_Control(pDX, IDC_GROUP_GMS,					m_groupFtpGms);
	DDX_Control(pDX, IDC_MSG_USER,					m_msgUser);
	DDX_Control(pDX, IDC_MSG_USER_DATA,				m_msgUserData);
	DDX_Control(pDX, IDC_MSG_PASSWORD,				m_msgPass);
	DDX_Control(pDX, IDC_MSG_PASSWORD_DATA,			m_msgPassData);
	DDX_Control(pDX, IDC_MSG_GMS_FTP_PATH,			m_msgGmsPath);
	DDX_Control(pDX, IDC_MSG_GMS_FTP_PATH_DATA,		m_msgGmsPathData);
	DDX_Control(pDX, IDC_MSG_CLIENT_IP_FTP,			m_msgFtpIP);
	DDX_Control(pDX, IDC_MSG_CLIENT_PORT_FTP,		m_msgFtpPort);
	DDX_Control(pDX, IDC_MSG_GMS_CHAN_1,			m_msgGmsCh[0]);
	DDX_Control(pDX, IDC_MSG_GMS_CHAN_2,			m_msgGmsCh[1]);
	DDX_Control(pDX, IDC_MSG_GMS_CHAN_3,			m_msgGmsCh[2]);
	DDX_Control(pDX, IDC_MSG_GMS_CHAN_4,			m_msgGmsCh[3]);
	DDX_Control(pDX, IDC_MSG_GMS_CHAN_5,			m_msgGmsCh[4]);
	DDX_Control(pDX, IDC_MSG_GMS_CHAN_6,			m_msgGmsCh[5]);
	DDX_Control(pDX, IDC_MSG_GMS_CHAN_7,			m_msgGmsCh[6]);
	DDX_Control(pDX, IDC_MSG_GMS_CHAN_8,			m_msgGmsCh[7]);
	DDX_Control(pDX, IDC_MSG_GMS_VOLTAGE_DATA_1,	m_msgGmsVoltData[0]);
	DDX_Control(pDX, IDC_MSG_GMS_VOLTAGE_DATA_2,	m_msgGmsVoltData[1]);
	DDX_Control(pDX, IDC_MSG_GMS_VOLTAGE_DATA_3,	m_msgGmsVoltData[2]);
	DDX_Control(pDX, IDC_MSG_GMS_VOLTAGE_DATA_4,	m_msgGmsVoltData[3]);
	DDX_Control(pDX, IDC_MSG_GMS_VOLTAGE_DATA_5,	m_msgGmsVoltData[4]);
	DDX_Control(pDX, IDC_MSG_GMS_VOLTAGE_DATA_6,	m_msgGmsVoltData[5]);
	DDX_Control(pDX, IDC_MSG_GMS_VOLTAGE_DATA_7,	m_msgGmsVoltData[6]);
	DDX_Control(pDX, IDC_MSG_GMS_VOLTAGE_DATA_8,	m_msgGmsVoltData[7]);
	DDX_Control(pDX, IDC_MSG_GMS_RESIST_DATA_1,		m_msgGmsResData[0]);
	DDX_Control(pDX, IDC_MSG_GMS_RESIST_DATA_2,		m_msgGmsResData[1]);
	DDX_Control(pDX, IDC_MSG_GMS_RESIST_DATA_3,		m_msgGmsResData[2]);
	DDX_Control(pDX, IDC_MSG_GMS_RESIST_DATA_4,		m_msgGmsResData[3]);
	DDX_Control(pDX, IDC_MSG_GMS_RESIST_DATA_5,		m_msgGmsResData[4]);
	DDX_Control(pDX, IDC_MSG_GMS_RESIST_DATA_6,		m_msgGmsResData[5]);
	DDX_Control(pDX, IDC_MSG_GMS_RESIST_DATA_7,		m_msgGmsResData[6]);
	DDX_Control(pDX, IDC_MSG_GMS_RESIST_DATA_8,		m_msgGmsResData[7]);
	DDX_Control(pDX, IDC_MSG_VNR_TIME,				m_msgVnRTime);
	DDX_Control(pDX, IDC_BTN_APPLY,					m_btnApply);
	DDX_Control(pDX, IDC_BTN_RELOAD,				m_btnReLoad);
	DDX_Control(pDX, IDC_BTN_CLIENT_IP_FTP,			m_btnFtpIP);
	DDX_Control(pDX, IDC_BTN_CLIENT_PORT_FTP,		m_btnFtpPort);
	DDX_Control(pDX, IDC_BTN_GMS_SEND,				m_btnGmsSend);
	DDX_Control(pDX, IDC_BTN_FTP_READ,				m_btnRead);
	DDX_Control(pDX, IDC_BTN_FTP_RECIPE_DOWNLOAD,	m_btnRecipe);
	DDX_Control(pDX, IDC_CHK_FTP_RECIPE_DOWNLOAD,	m_btnRecipeYesNo);
	DDX_Control(pDX, IDC_CHECK_WORK_GMS_USE,		m_btnFtpGmsUse);
	DDX_Control(pDX, IDC_CLIENT_IP_FTP,				m_aFtpIP);
	DDX_Control(pDX, IDC_EDIT_CLIENT_PORT_FTP,		m_editFtpPort);
	DDX_Control(pDX, IDC_DGT_VNR_TIME,				m_dgtVnRTime);
	DDX_Control(pDX, IDC_CUSTOM_TTS_VNR,			m_pGridVnR);
	DDX_Control(pDX, IDC_MSG_SCRAP_CODE_NAME,		m_msgScrapCodeName);
	DDX_Control(pDX, IDC_MSG_SCRAP_CODE_NAME_DATA,	m_msgScrapCodeNameData);
	DDX_Control(pDX, IDC_MSG_SCRAP_EQP_NAME,		m_msgScrapEqpName);
	DDX_Control(pDX, IDC_MSG_SCRAP_EQP_NAME_DATA,	m_msgScrapEqpNameData);
	DDX_Radio(pDX, IDC_RADIO_NOT_USED, m_nFtpMode);
}


BEGIN_MESSAGE_MAP(CWorkFtp, CDialog)
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_CLIENT_IP_FTP,		&CWorkFtp::OnClickedBtnClientIpFtp)
	ON_BN_CLICKED(IDC_BTN_CLIENT_PORT_FTP,		&CWorkFtp::OnClickedBtnClientPortFtp)
	ON_BN_CLICKED(IDC_CHECK_WORK_GMS_USE,		&CWorkFtp::OnClickedCheckWorkGmsUse)
	ON_STN_CLICKED(IDC_MSG_USER_DATA,			&CWorkFtp::OnClickedMsgUserData)
	ON_STN_CLICKED(IDC_MSG_PASSWORD_DATA,		&CWorkFtp::OnClickedMsgPasswordData)
	ON_STN_CLICKED(IDC_MSG_GMS_FTP_PATH_DATA,	&CWorkFtp::OnClickedMsgGmsFtpPathData)
	ON_BN_CLICKED(IDC_BTN_GMS_SEND,				&CWorkFtp::OnClickedBtnGmsSend)
	ON_STN_CLICKED(IDC_DGT_VNR_TIME,			&CWorkFtp::OnStnClickedDgtVnrTime)
	ON_BN_CLICKED(IDC_BTN_APPLY,				&CWorkFtp::OnBnClickedBtnApply)
	ON_MESSAGE(WM_WORK_DISPLAY,					&CWorkFtp::OnDataDisplay)
	ON_NOTIFY(NM_CLICK,		IDC_CUSTOM_TTS_VNR,	&CWorkFtp::OnVnRCellClick)
	ON_STN_CLICKED(IDC_MSG_SCRAP_CODE_NAME_DATA, &CWorkFtp::OnStnClickedMsgScrapCodeNameData)
	ON_STN_CLICKED(IDC_MSG_SCRAP_EQP_NAME_DATA, &CWorkFtp::OnStnClickedMsgScrapEqpNameData)
	ON_BN_CLICKED(IDC_RADIO_NOT_USED, &CWorkFtp::OnBnClickedRadioNotUsed)
	ON_BN_CLICKED(IDC_RADIO_LOCAL, &CWorkFtp::OnBnClickedRadioLocal)
	ON_BN_CLICKED(IDC_RADIO_FTP, &CWorkFtp::OnBnClickedRadioFtp)
	ON_BN_CLICKED(IDC_BTN_FTP_READ, &CWorkFtp::OnBnClickedBtnFtpRead)
	ON_BN_CLICKED(IDC_BTN_FTP_RECIPE_DOWNLOAD, &CWorkFtp::OnBnClickedBtnFtpRecipeDownload)
	ON_BN_CLICKED(IDC_CHK_FTP_RECIPE_DOWNLOAD, &CWorkFtp::OnBnClickedChkFtpRecipeDownload)
END_MESSAGE_MAP()


// CWorkFtp 메시지 처리기입니다.


BOOL CWorkFtp::DestroyWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CDialog::DestroyWindow();
}


BOOL CWorkFtp::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CString strTemp;

	OnDataInit();
	OnInitButton();
	OnInitGroupBox();
	OnInitLabel();
	OnInitDigit();
	OnInitGridVnR();

	st_handler_info.cWndFtp	= this;

	OnInterfaceIpAddress(m_strFtpIP[1]);
	m_aFtpIP.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);

	strTemp.Format(_T("%d"), m_nFtpPort[1]);
	m_editFtpPort.SetWindowText(strTemp);

	
	SetTimer(TM_GMS, 5000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CWorkFtp::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (OnDataComparison() == RET_ERROR)
	{
		OnDataHistoryLog();
		OnDataApply();
	}
}


BOOL CWorkFtp::OnEraseBkgnd(CDC* pDC)
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


void CWorkFtp::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == TM_GMS)
	{
		OnVnRDisplay();
	}

	CDialog::OnTimer(nIDEvent);
}


void CWorkFtp::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CDialog::PostNcDestroy();
}

void CWorkFtp::OnInitGroupBox()
{
	m_groupFtp.SetFont(clsFunc.OnLogFont(16));
	m_groupFtp.SetCatptionTextColor(BLUE_C);
	m_groupFtp.SetBorderColor(ORANGE_C);
	m_groupFtp.SetBackgroundColor(WINDOW_UP1);
	m_groupFtp.SetFontBold(TRUE);

	m_groupFtpGms.SetFont(clsFunc.OnLogFont(16));
	m_groupFtpGms.SetCatptionTextColor(BLUE_C);
	m_groupFtpGms.SetBorderColor(ORANGE_C);
	m_groupFtpGms.SetBackgroundColor(WINDOW_UP1);
	m_groupFtpGms.SetFontBold(TRUE);
}

void CWorkFtp::OnInitLabel()
{
	CString strTemp;
	int i;

	m_msgUser.SetFont(clsFunc.m_pFont[2]);
	m_msgUser.SetWindowText(_T("User"));
	m_msgUser.SetCenterText();
	m_msgUser.SetColor(WHITE_C);
	m_msgUser.SetGradientColor(ORANGE_C);
	m_msgUser.SetTextColor(BLACK_C);

	m_msgUserData.SetFont(clsFunc.m_pFont[2]);
	m_msgUserData.SetWindowText(m_strFtpUser[1]);
	m_msgUserData.SetCenterText();
	m_msgUserData.SetColor(WHITE_C);
	m_msgUserData.SetGradientColor(WHITE_C);
	m_msgUserData.SetTextColor(BLACK_C);

	m_msgPass.SetFont(clsFunc.m_pFont[2]);
	m_msgPass.SetWindowText(_T("Password"));
	m_msgPass.SetCenterText();
	m_msgPass.SetColor(WHITE_C);
	m_msgPass.SetGradientColor(ORANGE_C);
	m_msgPass.SetTextColor(BLACK_C);

	m_msgPassData.SetFont(clsFunc.m_pFont[2]);
	m_msgPassData.SetWindowText(m_strFtpPassword[1]);
	m_msgPassData.SetCenterText();
	m_msgPassData.SetColor(WHITE_C);
	m_msgPassData.SetGradientColor(WHITE_C);
	m_msgPassData.SetTextColor(BLACK_C);

	m_msgGmsPath.SetFont(clsFunc.m_pFont[2]);
	m_msgGmsPath.SetWindowText(_T("Ftp Path"));
	m_msgGmsPath.SetCenterText();
	m_msgGmsPath.SetColor(WHITE_C);
	m_msgGmsPath.SetGradientColor(ORANGE_C);
	m_msgGmsPath.SetTextColor(BLACK_C);

	m_msgGmsPathData.SetFont(clsFunc.m_pFont[2]);
	m_msgGmsPathData.SetWindowText(m_strFtpGmsPath[1]);
	m_msgGmsPathData.SetCenterText();
	m_msgGmsPathData.SetColor(WHITE_C);
	m_msgGmsPathData.SetGradientColor(WHITE_C);
	m_msgGmsPathData.SetTextColor(BLACK_C);

	m_msgFtpIP.SetFont(clsFunc.m_pFont[2]);
	m_msgFtpIP.SetWindowText(_T("FTP IP"));
	m_msgFtpIP.SetCenterText();
	m_msgFtpIP.SetColor(WHITE_C);
	m_msgFtpIP.SetGradientColor(ORANGE_C);
	m_msgFtpIP.SetTextColor(BLACK_C);

	m_msgFtpPort.SetFont(clsFunc.m_pFont[2]);
	m_msgFtpPort.SetWindowText(_T("FTP Port"));
	m_msgFtpPort.SetCenterText();
	m_msgFtpPort.SetColor(WHITE_C);
	m_msgFtpPort.SetGradientColor(ORANGE_C);
	m_msgFtpPort.SetTextColor(BLACK_C);

	m_msgVnRTime.SetFont(clsFunc.m_pFont[2]);
	m_msgVnRTime.SetWindowText(_T("TTS VnR Time"));
	m_msgVnRTime.SetCenterText();
	m_msgVnRTime.SetColor(WHITE_C);
	m_msgVnRTime.SetGradientColor(ORANGE_C);
	m_msgVnRTime.SetTextColor(BLACK_C);

	for (i=0; i<8; i++)
	{
		strTemp.Format(_T("Ch #%d"), i+1);

		m_msgGmsCh[i].SetFont(clsFunc.m_pFont[2]);
		m_msgGmsCh[i].SetWindowText(strTemp);
		m_msgGmsCh[i].SetCenterText();
		m_msgGmsCh[i].SetColor(WHITE_C);
		m_msgGmsCh[i].SetGradientColor(GREEN_C);
		m_msgGmsCh[i].SetTextColor(BLACK_C);

		m_msgGmsVoltData[i].SetFont(clsFunc.m_pFont[2]);
		m_msgGmsVoltData[i].SetWindowText(_T("-"));
		m_msgGmsVoltData[i].SetCenterText();
		m_msgGmsVoltData[i].SetColor(WHITE_C);
		m_msgGmsVoltData[i].SetGradientColor(WHITE_C);
		m_msgGmsVoltData[i].SetTextColor(BLACK_C);

		m_msgGmsResData[i].SetFont(clsFunc.m_pFont[2]);
		m_msgGmsResData[i].SetWindowText(_T("-"));
		m_msgGmsResData[i].SetCenterText();
		m_msgGmsResData[i].SetColor(WHITE_C);
		m_msgGmsResData[i].SetGradientColor(WHITE_C);
		m_msgGmsResData[i].SetTextColor(BLACK_C);
	}

	m_msgScrapCodeName.SetFont(clsFunc.m_pFont[2]);
	m_msgScrapCodeName.SetWindowText(_T("Scrap Code Name"));
	m_msgScrapCodeName.SetCenterText();
	m_msgScrapCodeName.SetColor(WHITE_C);
	m_msgScrapCodeName.SetGradientColor(ORANGE_C);
	m_msgScrapCodeName.SetTextColor(BLACK_C);

	m_msgScrapCodeNameData.SetFont(clsFunc.m_pFont[2]);
	m_msgScrapCodeNameData.SetWindowText(m_strScrapCodeName[1]);
	m_msgScrapCodeNameData.SetCenterText();
	m_msgScrapCodeNameData.SetColor(WHITE_C);
	m_msgScrapCodeNameData.SetGradientColor(WHITE_C);
	m_msgScrapCodeNameData.SetTextColor(BLACK_C);

	m_msgScrapEqpName.SetFont(clsFunc.m_pFont[2]);
	m_msgScrapEqpName.SetWindowText(_T("Scrap Eqp Name"));
	m_msgScrapEqpName.SetCenterText();
	m_msgScrapEqpName.SetColor(WHITE_C);
	m_msgScrapEqpName.SetGradientColor(ORANGE_C);
	m_msgScrapEqpName.SetTextColor(BLACK_C);

	m_msgScrapEqpNameData.SetFont(clsFunc.m_pFont[2]);
	m_msgScrapEqpNameData.SetWindowText(m_strScrapEqpName[1]);
	m_msgScrapEqpNameData.SetCenterText();
	m_msgScrapEqpNameData.SetColor(WHITE_C);
	m_msgScrapEqpNameData.SetGradientColor(WHITE_C);
	m_msgScrapEqpNameData.SetTextColor(BLACK_C);
}

void CWorkFtp::OnInitDigit()
{
	m_dgtVnRTime.SetStyle(CDigit::DS_INT, 3, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgtVnRTime.SetVal(m_nVnRTime[1]);
}


void CWorkFtp::OnInitButton()
{
	m_btnApply.SetBitmaps(IDC_BTN_APPLY, IDB_BITMAP_APPLY_DN1, WINDOW_DN1, IDB_BITMAP_APPLY_UP1, WINDOW_UP1);
	m_btnApply.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	m_btnApply.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	m_btnApply.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	m_btnApply.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnApply.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnApply.SetFont(clsFunc.m_pFont[2]);
	m_btnApply.SetTooltipText(_T("Recipe Data Apply"));

	m_btnReLoad.SetBitmaps(IDC_BTN_RELOAD, IDB_BITMAP_RELOAD_DN1, WINDOW_DN1, IDB_BITMAP_RELOAD_UP1, WINDOW_UP1);
	m_btnReLoad.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	m_btnReLoad.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	m_btnReLoad.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	m_btnReLoad.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnReLoad.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnReLoad.SetFont(clsFunc.m_pFont[2]);
	m_btnReLoad.SetTooltipText(_T("Recipe Data ReLoad"));

	m_btnFtpGmsUse.SetBitmaps(IDC_CHECK_WORK_GMS_USE, IDB_BITMAP_USED_UP, WINDOW_DN1, IDB_BITMAP_USED_DN, WINDOW_UP1);
	m_btnFtpGmsUse.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	m_btnFtpGmsUse.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	m_btnFtpGmsUse.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	m_btnFtpGmsUse.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnFtpGmsUse.SetFont(clsFunc.m_pFont[2]);
	m_btnFtpGmsUse.SetCheck(m_nFtpGmsUse[1]);
	if (m_btnFtpGmsUse.GetCheck())
	{
		m_btnFtpGmsUse.SetWindowText(_T("FTP Yes"));
	}
	else
	{
		m_btnFtpGmsUse.SetWindowText(_T("FTP No"));
	}

	m_btnFtpIP.SetBitmaps(IDC_BTN_CLIENT_IP_FTP, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
	m_btnFtpIP.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	m_btnFtpIP.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	m_btnFtpIP.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	m_btnFtpIP.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnFtpIP.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnFtpIP.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnFtpIP.SetFont(clsFunc.m_pFont[1]);
	m_btnFtpIP.SetTooltipText(_T("FTP Client IP Address"));

	m_btnFtpPort.SetBitmaps(IDC_BTN_CLIENT_PORT_FTP, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
	m_btnFtpPort.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	m_btnFtpPort.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	m_btnFtpPort.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	m_btnFtpPort.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnFtpPort.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnFtpPort.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnFtpPort.SetFont(clsFunc.m_pFont[1]);
	m_btnFtpPort.SetTooltipText(_T("FTP Client Port"));

	m_btnGmsSend.SetBitmaps(IDC_BTN_GMS_SEND, IDB_BTN_UPLOAD_DN, WINDOW_DN1, IDB_BTN_UPLOAD_UP, WINDOW_UP1);
	m_btnGmsSend.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	m_btnGmsSend.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	m_btnGmsSend.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	m_btnGmsSend.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnGmsSend.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnGmsSend.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnGmsSend.SetFont(clsFunc.m_pFont[1]);
	m_btnGmsSend.SetTooltipText(_T("GMS Data Send"));

	m_btnRead.SetBitmaps(IDC_BTN_FTP_READ, IDB_BITMAP_RELOAD_DN, WINDOW_DN1, IDB_BITMAP_RELOAD_UP, WINDOW_UP1);
	m_btnRead.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	m_btnRead.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	m_btnRead.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	m_btnRead.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnRead.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnRead.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnRead.SetFont(clsFunc.m_pFont[1]);
	m_btnRead.SetTooltipText(_T("ScrapCode File Read"));

	m_btnRecipe.SetBitmaps(IDC_BTN_FTP_RECIPE_DOWNLOAD, IDB_BTN_DOWNLOAD_DN, WINDOW_DN1, IDB_BTN_DOWNLOAD_UP, WINDOW_UP1);
	m_btnRecipe.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	m_btnRecipe.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	m_btnRecipe.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	m_btnRecipe.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnRecipe.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnRecipe.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnRecipe.SetFont(clsFunc.m_pFont[1]);
	m_btnRecipe.SetTooltipText(_T("Recipe Download"));

	m_btnRecipeYesNo.SetBitmaps(IDC_CHK_FTP_RECIPE_DOWNLOAD, IDB_BITMAP_USED_UP, WINDOW_DN1, IDB_BITMAP_USED_DN, WINDOW_UP1);
	m_btnRecipeYesNo.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	m_btnRecipeYesNo.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	m_btnRecipeYesNo.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	m_btnRecipeYesNo.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnRecipeYesNo.SetFont(clsFunc.m_pFont[2]);
	m_btnRecipeYesNo.SetCheck(m_nRecipeYesNo[1]);
	if (m_btnRecipeYesNo.GetCheck())
	{
		m_btnRecipeYesNo.SetWindowText(_T("Recipe Yes"));
	}
	else
	{
		m_btnRecipeYesNo.SetWindowText(_T("Recipe No"));
	}
}

void CWorkFtp::OnInitArray()
{
	int i;

	for (i=0; i<8; i++)
	{
		m_ArValVolt[i].SetSize(60);
		m_ArValRes[i].SetSize(60);
	}
}

void CWorkFtp::OnInitGridVnR()
{
	int i, j;
	int nMaxRow, nMaxCol;
	int nRow;

	CString strTemp;
// 	CString strHeader[5]	= {_T("CH No."), _T("SPEC"), _T("UCL"), _T("LCL"), _T("Location")};
// 	CString strLocation[8]	= {_T("Loading Robot Pitch"), _T("Unloading Robot Pitch"), _T("Loading Tray Shifter"), 
// 							   _T("Unloading Tray Shifter"), _T("Testsite Robot Z"), _T("Reject Gripper"), _T(""), _T("")};
	CString strHeader[5]	= {_T("CH No."), _T("SPEC"), _T("UCL"), _T("LCL"), _T("Location")};
	CString strLocation[8]	= {_T("-"), _T("-"), _T("-"), 
		_T("-"), _T("-"), _T("-"), _T(""), _T("")};

	nMaxCol = 65;
	nMaxRow = 17;

	m_pGridVnR.Refresh();

	m_pGridVnR.SetFrameFocusCell(FALSE);
	m_pGridVnR.SetTrackFocusCell(FALSE);
	m_pGridVnR.EnableSelection(FALSE);

	m_pGridVnR.SetGridLineColor(BLACK_C);
	m_pGridVnR.SetGridLines(1);

	m_pGridVnR.SetRowCount(nMaxRow);
	m_pGridVnR.SetColumnCount(nMaxCol);

	m_pGridVnR.SetFixedRowCount(0);
	m_pGridVnR.SetFixedColumnCount(0);
	m_pGridVnR.SetFixedBkColor(RGB(0,0,200));
	m_pGridVnR.SetFixedBkColor(RGB(200,200,255));
	m_pGridVnR.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<nMaxRow; i++)
	{
		m_pGridVnR.SetRowHeight(i, 35);

		for (j=0; j<nMaxCol; j++)
		{
			m_pGridVnR.SetColumnWidth(j, 80);

			m_pGridVnR.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridVnR.SetItemFont(i, j, &clsFunc.OnLogFont(16));
			m_pGridVnR.SetItemState(i, j, GVIS_READONLY);

			if (j==4) m_pGridVnR.SetColumnWidth(j, 160);
			if (i==0)
			{
				m_pGridVnR.SetItemBkColour(i, j, BLACK_L, WHITE_C);

				if (j<5) m_pGridVnR.SetItemText(0, j, strHeader[j]);
				else
				{
					strTemp.Format(_T("%02d"), j-4);
					m_pGridVnR.SetItemText(0, j, strTemp);
				}
			}
			else m_pGridVnR.SetItemBkColour(i, j, WINDOW_UP, BLACK_L);
					
		}
	}

	for (i=0; i<8; i++)
	{
		nRow = i+i;

		m_pGridVnR.MergeCells(nRow+1, 0, nRow+2, 0);
		m_pGridVnR.MergeCells(nRow+1, 4, nRow+2, 4);

		strTemp.Format(_T("# %02d"), i+1);
		m_pGridVnR.SetItemText(nRow+1, 0, strTemp);
		m_pGridVnR.SetItemText(nRow+1, 4, strLocation[i]);
	}

	nRow = 0;

	for (j=0; j<8; j++)
	{
		nRow = j+j+1;
		
		m_pGridVnR.SetItemText(nRow+1, 1, m_strVnrSpec[1][0][j]);
		m_pGridVnR.SetItemText(nRow, 1, m_strVnrSpec[1][1][j]);

		m_pGridVnR.SetItemText(nRow+1, 2, m_strVnrUcl[1][0][j]);
		m_pGridVnR.SetItemText(nRow, 2, m_strVnrUcl[1][1][j]);

		m_pGridVnR.SetItemText(nRow+1, 3, m_strVnrLcl[1][0][j]);
		m_pGridVnR.SetItemText(nRow, 3, m_strVnrLcl[1][1][j]);
	}

	m_pGridVnR.Invalidate(FALSE);
}

void CWorkFtp::OnDataHistoryLog()
{
	CString strMsg;

	if (m_nFtpGmsUse[0]			!= m_nFtpGmsUse[1])
	{
		strMsg.Format(_T("FTP GMS Use Change %d -> %d"), m_nFtpGmsUse[0], m_nFtpGmsUse[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	if (m_nFtpPort[0]			!= m_nFtpPort[1])
	{
		strMsg.Format(_T("FTP Port Change %d -> %d"), m_nFtpPort[0], m_nFtpPort[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	if (m_nVnRTime[0]			!= m_nVnRTime[1])
	{
		strMsg.Format(_T("VnR Check Time Change %d -> %d"), m_nVnRTime[0], m_nVnRTime[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	if (m_strFtpIP[0]			!= m_strFtpIP[1])
	{
		strMsg.Format(_T("FTP IP Change %s -> %s"), m_strFtpIP[0], m_strFtpIP[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	if (m_strFtpUser[0]			!= m_strFtpUser[1])
	{
		strMsg.Format(_T("FTP User Change %s -> %s"), m_strFtpUser[0], m_strFtpUser[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	if (m_strFtpPassword[0]		!= m_strFtpPassword[1])
	{
		strMsg.Format(_T("FTP Password Change %s -> %s"), m_strFtpPassword[0], m_strFtpPassword[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	if (m_strFtpGmsPath[0]		!= m_strFtpGmsPath[1])
	{
		strMsg.Format(_T("FTP GMS Path Change %s -> %s"), m_strFtpGmsPath[0], m_strFtpGmsPath[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	if (m_strScrapCodeName[0]	!= m_strScrapCodeName[1])
	{
		strMsg.Format(_T("FTP Scrap Code Change %s -> %s"), m_strScrapCodeName[0], m_strScrapCodeName[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	if (m_strScrapEqpName[0]	!= m_strScrapEqpName[1])
	{
		strMsg.Format(_T("FTP Scrap Eqp Change %s -> %s"), m_strScrapEqpName[0], m_strScrapEqpName[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	if (m_nRecipeYesNo[0]	!= m_nRecipeYesNo[1])
	{
		strMsg.Format(_T("Recipe Download YesNo Change %d -> %d"), m_nRecipeYesNo[0], m_nRecipeYesNo[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}
}


void CWorkFtp::OnDataRecovery()
{
	int i, j;

	m_nFtpGmsUse[1]			= m_nFtpGmsUse[0];
	m_nFtpPort[1]			= m_nFtpPort[0];
	m_nVnRTime[1]			= m_nVnRTime[0];

	m_strFtpIP[1]			= m_strFtpIP[0];
	m_strFtpUser[1]			= m_strFtpUser[0];
	m_strFtpPassword[1]		= m_strFtpPassword[0];
	m_strFtpGmsPath[1]		= m_strFtpGmsPath[0];

	for (j=0; j<2; j++)
	{
		for (i=0; i<8; i++)
		{
			m_strVnrSpec[1][j][i]	= m_strVnrSpec[0][j][i];
			m_strVnrUcl[1][j][i]	= m_strVnrUcl[0][j][i];
			m_strVnrLcl[1][j][i]	= m_strVnrLcl[0][j][i];
		}
	}

	m_strScrapCodeName[1]		= m_strScrapCodeName[0];
	m_strScrapEqpName[1]		= m_strScrapEqpName[0];

	m_nRecipeYesNo[1]			= m_nRecipeYesNo[0];
}


int CWorkFtp::OnDataComparison()
{
	int i, j;

	if (m_nFtpGmsUse[0]			!= m_nFtpGmsUse[1])		return RET_ERROR;
	if (m_nFtpPort[0]			!= m_nFtpPort[1])		return RET_ERROR;
	if (m_nVnRTime[0]			!= m_nVnRTime[1])		return RET_ERROR;

	if (m_strFtpIP[0]			!= m_strFtpIP[1])		return RET_ERROR;
	if (m_strFtpUser[0]			!= m_strFtpUser[1])		return RET_ERROR;
	if (m_strFtpPassword[0]		!= m_strFtpPassword[1]) return RET_ERROR;
	if (m_strFtpGmsPath[0]		!= m_strFtpGmsPath[1])	return RET_ERROR;

	for (j=0; j<2; j++)
	{
		for (i=0; i<8; i++)
		{
			if (m_strVnrSpec[0][j][i]	!= m_strVnrSpec[1][j][i])	return RET_ERROR;
			if (m_strVnrUcl[0][j][i]	!= m_strVnrUcl[1][j][i])	return RET_ERROR;
			if (m_strVnrLcl[0][j][i]	!= m_strVnrLcl[1][j][i])	return RET_ERROR;
		}
	}

	if (m_strScrapCodeName[0]	!= m_strScrapCodeName[1])	return RET_ERROR;
	if (m_strScrapEqpName[0]	!= m_strScrapEqpName[1])	return RET_ERROR;

	if (m_nRecipeYesNo[0]		!= m_nRecipeYesNo[1])		return RET_ERROR;

	return RET_GOOD;
}


void CWorkFtp::OnDataApply()
{
	int i, j;

	//2017.0109
	if (st_handler_info.nFtpVer	!= m_nFtpGmsUse[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("MODE FTP");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%d"), st_handler_info.nFtpVer);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%d"), m_nFtpGmsUse[1]);

		clsLog.LogConfig(_T("WORKFTP"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_client_info[FTP_NETWORK].nPort	!= m_nFtpPort[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("FTP PORT");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%d"), st_client_info[FTP_NETWORK].nPort);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%d"), m_nFtpPort[1]);

		clsLog.LogConfig(_T("WORKFTP"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_basic_info.strFtpUser	!= m_strFtpUser[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("FTP USER");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%s"), st_basic_info.strFtpUser);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%s"), m_strFtpUser[1]);

		clsLog.LogConfig(_T("WORKFTP"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_basic_info.strFtpPass	!= m_strFtpPassword[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("FTP PASSWARD");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%s"), st_basic_info.strFtpPass);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%s"), m_strFtpPassword[1]);

		clsLog.LogConfig(_T("WORKFTP"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_basic_info.strScrapName	!= m_strScrapCodeName[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("SCRAP NAME");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%s"), st_basic_info.strScrapName);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%s"), m_strScrapCodeName[1]);

		clsLog.LogConfig(_T("WORKFTP"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_basic_info.strScrapHead	!= m_strScrapEqpName[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("SCRAP HEAD");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%s"), st_basic_info.strScrapHead);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%s"), m_strScrapEqpName[1]);

		clsLog.LogConfig(_T("WORKFTP"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_basic_info.nRecipeYesNo	!= m_nRecipeYesNo[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("RECIPE");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%d"), st_basic_info.nRecipeYesNo);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%d"), m_nRecipeYesNo[1]);

		clsLog.LogConfig(_T("WORKFTP"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//	st_basic_info.nModeFtpGms			= m_nFtpGmsUse[1];
	st_handler_info.nFtpVer				= m_nFtpGmsUse[1];
	st_client_info[FTP_NETWORK].nPort	= m_nFtpPort[1];

	st_client_info[FTP_NETWORK].strIp	= m_strFtpIP[1];
	st_basic_info.strFtpUser			= m_strFtpUser[1];
	st_basic_info.strFtpPass			= m_strFtpPassword[1];
	st_path_info.strPathFtpGms			= m_strFtpGmsPath[1];
	st_basic_info.nVnRTime				= m_nVnRTime[1];

	for (j=0; j<2; j++)
	{
		for (i=0; i<8; i++)
		{
			st_basic_info.strVnrSpec[j][i]		= m_strVnrSpec[1][j][i];
			st_basic_info.strVnrUcl[j][i]		= m_strVnrUcl[1][j][i];
			st_basic_info.strVnrLcl[j][i]		= m_strVnrLcl[1][j][i];
		}
	}

	st_basic_info.strScrapName			= m_strScrapCodeName[1];
	st_basic_info.strScrapHead			= m_strScrapEqpName[1];

	st_basic_info.nRecipeYesNo			= m_nRecipeYesNo[1];
}


void CWorkFtp::OnDataBackup()
{
	int i, j;

	m_nFtpGmsUse[0]			= m_nFtpGmsUse[1];
	m_nFtpPort[0]			= m_nFtpPort[1];
	m_nVnRTime[0]			= m_nVnRTime[1];

	m_strFtpIP[0]			= m_strFtpIP[1];
	m_strFtpUser[0]			= m_strFtpUser[1];
	m_strFtpPassword[0]		= m_strFtpPassword[1];
	m_strFtpGmsPath[0]		= m_strFtpGmsPath[1];

	for (j=0; j<2; j++)
	{
		for (i=0; i<8; i++)
		{
			m_strVnrSpec[0][j][i]	= m_strVnrSpec[1][j][i];
			m_strVnrUcl[0][j][i]	= m_strVnrUcl[1][j][i];
			m_strVnrLcl[0][j][i]	= m_strVnrLcl[1][j][i];
		}
	}

	m_strScrapCodeName[0]	= m_strScrapCodeName[1];
	m_strScrapEqpName[0]	= m_strScrapEqpName[1];

	m_nRecipeYesNo[0]		= m_nRecipeYesNo[1];
}


void CWorkFtp::OnDataInit()
{
	int i, j;

	//m_nFtpGmsUse[1]			= st_basic_info.nModeFtpGms;
	m_nFtpGmsUse[1]			= st_handler_info.nFtpVer;
	m_nFtpPort[1]			= st_client_info[FTP_NETWORK].nPort;
	m_nVnRTime[1]			= st_basic_info.nVnRTime;

	m_strFtpIP[1]			= st_client_info[FTP_NETWORK].strIp;
	m_strFtpUser[1]			= st_basic_info.strFtpUser;
	m_strFtpPassword[1]		= st_basic_info.strFtpPass;
	m_strFtpGmsPath[1]		= st_path_info.strPathFtpGms;

	for (j=0; j<2; j++)
	{
		for (i=0; i<8; i++)
		{
			m_strVnrSpec[1][j][i]		= st_basic_info.strVnrSpec[j][i];
			m_strVnrUcl[1][j][i]		= st_basic_info.strVnrUcl[j][i];
			m_strVnrLcl[1][j][i]		= st_basic_info.strVnrLcl[j][i];
		}
	}

	m_strScrapCodeName[1]		= st_basic_info.strScrapName;
	m_strScrapEqpName[1]		= st_basic_info.strScrapHead;

	m_nFtpMode					= m_nFtpGmsUse[1];
	UpdateData(FALSE);

	m_nRecipeYesNo[1]			= st_basic_info.nRecipeYesNo;

	OnDataBackup();  // 현재 클래스 변수 설정 상태 백업
}

LRESULT CWorkFtp::OnDataDisplay(WPARAM wParam, LPARAM lParam)
{
	int i, j, nRow;
	
	i = wParam;

	m_ArValVolt[i].InsertAt(0, st_gms_info.strValVolt[i]);
	m_ArValRes[i].InsertAt(0, st_gms_info.strValRes[i]);

	for (j=0; j<60; j++)
	{
		nRow = i+i;

		m_pGridVnR.SetItemText(nRow+1, j+5, m_ArValVolt[i].GetAt(j));
		m_pGridVnR.SetItemText(nRow+2, j+5, m_ArValRes[i].GetAt(j));
	}

	if (m_ArValVolt[i].GetSize() > 60) m_ArValVolt[i].RemoveAt(60);
	if (m_ArValRes[i].GetSize() > 60) m_ArValRes[i].RemoveAt(60);
	
	return 0;
}

void CWorkFtp::OnClickedBtnClientIpFtp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_KeyBoard dlgKeyBoard;
	CString strTmp;

	dlgKeyBoard.m_strKeyBoardVal = _T("");

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		// TODO: Add your control notification handler code here
		m_strFtpIP[1] = dlgKeyBoard.m_strKeyBoardVal;

		m_strFtpIP[1].MakeUpper();
		m_strFtpIP[1].TrimLeft(' ');               
		m_strFtpIP[1].TrimRight(' ');

		OnInterfaceIpAddress(m_strFtpIP[1]);

		m_aFtpIP.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	}
}


void CWorkFtp::OnClickedBtnClientPortFtp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_KeyBoard dlgKeyBoard;
	CString strTemp;

	strTemp.Format(_T("%d"), m_nFtpPort[1]);
	dlgKeyBoard.m_strKeyBoardVal = strTemp;

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		strTemp = dlgKeyBoard.m_strKeyBoardVal;
		m_nFtpPort[1] = _wtoi(strTemp);

		m_editFtpPort.SetWindowText(strTemp);
	}
}


void CWorkFtp::OnClickedCheckWorkGmsUse()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_btnFtpGmsUse.GetCheck())
	{
		m_nFtpGmsUse[1] = YES;

		m_btnFtpGmsUse.SetWindowText(_T("FTP Yes"));
	}
	else
	{
		m_nFtpGmsUse[1] = NO;

		m_btnFtpGmsUse.SetWindowText(_T("FTP No"));
	}
}


void CWorkFtp::OnClickedMsgUserData()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_KeyBoard dlgKeyBoard;

	dlgKeyBoard.m_strKeyBoardVal = m_strFtpUser[1];

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		m_strFtpUser[1] = dlgKeyBoard.m_strKeyBoardVal;

		m_msgUserData.SetWindowText(m_strFtpUser[1]);
	}
}


void CWorkFtp::OnClickedMsgPasswordData()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_KeyBoard dlgKeyBoard;

	dlgKeyBoard.m_strKeyBoardVal = m_strFtpPassword[1];

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		m_strFtpPassword[1] = dlgKeyBoard.m_strKeyBoardVal;

		m_msgPassData.SetWindowText(m_strFtpPassword[1]);
	}
}


void CWorkFtp::OnClickedMsgGmsFtpPathData()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogFtp dlgFtp;

	if (dlgFtp.DoModal())
	{
		if (dlgFtp.m_strPathRemote != _T(""))
		{
			m_strFtpGmsPath[1] = dlgFtp.m_strPathRemote;

			m_msgGmsPathData.SetWindowText(m_strFtpGmsPath[1]);
		}
	}
}

void CWorkFtp::OnInterfaceIpAddress(CString strIP)
{
	int nDot1, nDot2, nDot3, nStart;

	nStart = 0;
	nDot1 = strIP.Find('.');
	nDot2 = strIP.Find('.', nDot1 + 1);
	nDot3 = strIP.Find('.', nDot2 + 1);

	m_byIp[0]	= _wtoi(strIP.Mid(nStart, nDot1 - nStart));
	nStart		= nDot1 + 1;
	m_byIp[1]	= _wtoi(strIP.Mid(nStart, nDot2 - nStart));
	nStart		= nDot2 + 1;
	m_byIp[2]	= _wtoi(strIP.Mid(nStart, nDot3 - nStart));
	nStart		= nDot3 + 1;
	m_byIp[3]	= _wtoi(strIP.Mid(nStart));
}

void CWorkFtp::OnClickedBtnGmsSend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
/*	if (m_pFtp.Open(st_client_info[FTP_NETWORK].strIp, 
				    st_basic_info.strFtpUser,
				    st_basic_info.strFtpPass,
				    st_client_info[FTP_NETWORK].nPort))
	{
		m_pFtp.DownLoad(st_path_info.strPathFtpGms, _T("E:\\jtkim.TXT"));
	}*/
//	clsFtp.OnScrapCodeDownload();
//	CFtpComm clsFtp;

//	clsFtp.OnScrapCodeDownload(0, st_path_info.strPathFtpGms, _T("RETEST_RULE.TXT"));
	// jtkim 20150514 ftp scrap code
	::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + FTP_NETWORK, 0, 0);
}


void CWorkFtp::OnStnClickedDgtVnrTime()
{
	CDialog_KeyPad dlgKeyPad;

	dlgKeyPad.m_nKeypadMode			= 0;
	dlgKeyPad.m_strKeypadLowLimit	= _T("0");
	dlgKeyPad.m_strKeypadHighLimit	= _T("999");
	dlgKeyPad.m_strKeypadVal.Format(_T("%d"), m_nVnRTime[1]);

	if (dlgKeyPad.DoModal() == IDOK)
	{
		m_nVnRTime[1] = _wtoi(dlgKeyPad.m_strNewVal);
		m_dgtVnRTime.SetVal(m_nVnRTime[1]);
	}
}


void CWorkFtp::OnBnClickedBtnApply()
{
	CDialog_Message  dlgMsg;
	int nResponse;

	if (OnDataComparison() == RET_ERROR)
	{
		dlgMsg.m_nMessageType	= 1;
		dlgMsg.m_strMessage		= _T("Do you want to change the existing value to the current value?");

		nResponse =  (int)dlgMsg.DoModal();

		if (nResponse == IDOK)
		{
			if (OnDataComparison() == RET_ERROR)
			{
				OnDataHistoryLog();
				OnDataApply();
				OnDataBackup();
			}
		}
	}
	//OnDataApply();
}

void CWorkFtp::OnVnRCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CDialog_KeyBoard	dlgKeyBoard;
	CDialog_KeyPad		dlgKeyPad;

	int nRow, nCol, nTemp;
	int nBuff[16] = {0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7};

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	if (nRow == 0) return;
	if (nCol >= 4 || nCol < 0) return;

	if (nCol == 1)	//Spec
	{
		nTemp = nRow % 2;

		dlgKeyBoard.m_strKeyBoardVal = m_strVnrSpec[1][nTemp][nBuff[nRow-1]];

		if (dlgKeyBoard.DoModal() == IDOK)
		{
			m_strVnrSpec[1][nTemp][nBuff[nRow-1]] = dlgKeyBoard.m_strKeyBoardVal;
		}
	}
	else if (nCol == 2)	//Ucl
	{
		nTemp = nRow % 2;

		dlgKeyBoard.m_strKeyBoardVal = m_strVnrUcl[1][nTemp][nBuff[nRow-1]];

		if (dlgKeyBoard.DoModal() == IDOK)
		{
			m_strVnrUcl[1][nTemp][nBuff[nRow-1]] = dlgKeyBoard.m_strKeyBoardVal;
		}
	}
	else if (nCol == 3)	//Lcl
	{
		nTemp = nRow % 2;

		dlgKeyBoard.m_strKeyBoardVal = m_strVnrLcl[1][nTemp][nBuff[nRow-1]];

		if (dlgKeyBoard.DoModal() == IDOK)
		{
			m_strVnrLcl[1][nTemp][nBuff[nRow-1]] = dlgKeyBoard.m_strKeyBoardVal;
		}
	}

	OnInitGridVnR();
}


void CWorkFtp::OnVnRDisplay()
{
	int i;

	CString strTemp;
	CString str,strtmp;
	CString strTmp;

	for (i=0; i<60; i++)
	{
		// channel 1
		strTemp.Format(_T("%.3f"), clsVnR.m_dVolt[0][i]);
		m_pGridVnR.SetItemText(1, i + 5, strTemp);

		strTemp.Format(_T("%.3f"), clsVnR.m_dResistance[0][i]);
		m_pGridVnR.SetItemText(2, i + 5, strTemp);

		// channel 2
		strTemp.Format(_T("%.3f"), clsVnR.m_dVolt[1][i]);
		m_pGridVnR.SetItemText(3, i + 5, strTemp);

		strTemp.Format(_T("%.3f"), clsVnR.m_dResistance[1][i]);
		m_pGridVnR.SetItemText(4, i + 5, strTemp);

		// channel 3
		strTemp.Format(_T("%.3f"), clsVnR.m_dVolt[2][i]);
		m_pGridVnR.SetItemText(5, i + 5, strTemp);

		strTemp.Format(_T("%.3f"), clsVnR.m_dResistance[2][i]);
		m_pGridVnR.SetItemText(6, i + 5, strTemp);

		// channel 4
		strTemp.Format(_T("%.3f"), clsVnR.m_dVolt[3][i]);
		m_pGridVnR.SetItemText(7, i + 5, strTemp);

		strTemp.Format(_T("%.3f"), clsVnR.m_dResistance[3][i]);
		m_pGridVnR.SetItemText(8, i + 5, strTemp);

		// channel 5
		strTemp.Format(_T("%.3f"), clsVnR.m_dVolt[4][i]);
		m_pGridVnR.SetItemText(9, i + 5, strTemp);

		strTemp.Format(_T("%.3f"), clsVnR.m_dResistance[4][i]);
		m_pGridVnR.SetItemText(10, i + 5, strTemp);

		// channel 6
		strTemp.Format(_T("%.3f"), clsVnR.m_dVolt[5][i]);
		m_pGridVnR.SetItemText(11, i + 5, strTemp);

		strTemp.Format(_T("%.3f"), clsVnR.m_dResistance[5][i]);
		m_pGridVnR.SetItemText(12, i + 5, strTemp);

	}
	
	m_pGridVnR.Invalidate(FALSE);
}

void CWorkFtp::OnStnClickedMsgScrapCodeNameData()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_KeyBoard dlgKeyBoard;
	
	dlgKeyBoard.m_strKeyBoardVal = _T("");

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		// TODO: Add your control notification handler code here
		m_strScrapCodeName[1] = dlgKeyBoard.m_strKeyBoardVal;

		m_msgScrapCodeNameData.SetWindowText(m_strScrapCodeName[1]);
	}
}


void CWorkFtp::OnStnClickedMsgScrapEqpNameData()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_KeyBoard dlgKeyBoard;
	
	dlgKeyBoard.m_strKeyBoardVal = _T("");

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		// TODO: Add your control notification handler code here
		m_strScrapEqpName[1] = dlgKeyBoard.m_strKeyBoardVal;

		m_msgScrapEqpNameData.SetWindowText(m_strScrapEqpName[1]);
	}
}


void CWorkFtp::OnBnClickedRadioNotUsed()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nFtpGmsUse[1] = 0;
}


void CWorkFtp::OnBnClickedRadioLocal()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	CDialog_Message dlgMsg;
	
//	CString strFtpName = _T("D:\\AMT840\\FILE\\RETEST_RULE.TXT");
	
//	int i, j;

	m_nFtpGmsUse[1] = 1;
/*
	dlgMsg.m_nMessageType	= 1;
	dlgMsg.m_strMessage.Format(_T("Do you want a ScrapCode?"));

	if (dlgMsg.DoModal() == IDOK)
	{
		for (i=0; i<10000; i++)
		{
			for (j=0; j<2; j++)
			{
				st_code_info[LOT_CURR].m_nScrapCode[i][j] = st_recipe_info.nTestRetest_Count;
			}
		}

		clsFunc.OnScrapCodeFind(LOT_CURR, strFtpName);
	}
*/
}


void CWorkFtp::OnBnClickedRadioFtp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	CDialog_Message dlgMsg;

//	CString strFtpName = _T("D:\\AMT840\\FILE\\RETEST_RULE.TXT");

//	int i, j;

	m_nFtpGmsUse[1] = 2;
/*
	dlgMsg.m_nMessageType	= 1;
	dlgMsg.m_strMessage.Format(_T("Do you want a ScrapCode?"));

	if (dlgMsg.DoModal() == IDOK)
	{
		for (i=0; i<10000; i++)
		{
			for (j=0; j<2; j++)
			{
				st_code_info[LOT_CURR].m_nScrapCode[i][j] = st_recipe_info.nTestRetest_Count;
			}
		}

		clsFunc.OnScrapCodeFind(LOT_CURR, strFtpName);
	}
*/
}


void CWorkFtp::OnBnClickedBtnFtpRead()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
 	CDialog_Message dlgMsg;
 
 	CString strFtpName;// = _T("D:\\AMT840\\FILE\\RETEST_RULE.TXT");
 
 	int i, j;
 
 	m_nFtpGmsUse[1] = 2;
 
 	dlgMsg.m_nMessageType	= 1;
 	dlgMsg.m_strMessage.Format(_T("Do you want a ScrapCode?"));
 
 	if (dlgMsg.DoModal() == IDOK)
 	{
 		for (i=0; i<10000; i++)
 		{
 			for (j=0; j<2; j++)
 			{
 				st_code_info[LOT_CURR].m_nScrapCode[i][j] = st_recipe_info.nTestRetest_Count;
 			}
 		}
 
 		if (m_nFtpGmsUse[1] == 1)
 		{
 			strFtpName = _T("C:\\AMT840\\FILE\\RETEST_RULE.TXT");
 		}
 		else if (m_nFtpGmsUse[1] == 2)
 		{
 			strFtpName = _T("C:\\AMT840\\RETEST_RULE.TXT");
 		}
 		clsFunc.OnScrapCodeFind(LOT_CURR, strFtpName);
 	}
}


void CWorkFtp::OnBnClickedBtnFtpRecipeDownload()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_Message dlgMsg;

	dlgMsg.m_nMessageType	= 1;
	dlgMsg.m_strMessage.Format(_T("Do You Want to Download The Recipe"));

	if (dlgMsg.DoModal() == IDOK)
	{
		// jtkim 20150929
		::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + FTP_NETWORK, 1, 0);
	}
}


void CWorkFtp::OnBnClickedChkFtpRecipeDownload()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_btnRecipeYesNo.GetCheck())
	{
		m_nRecipeYesNo[1] = YES;

		m_btnRecipeYesNo.SetWindowText(_T("Recipe Yes"));
	}
	else
	{
		m_nRecipeYesNo[1] = NO;

		m_btnRecipeYesNo.SetWindowText(_T("Recipe No"));
	}
}
