// ScreenBasic.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "ScreenBasic.h"
#include "PublicFunction.h"
#include "Dialog_Message.h"
#include "io.h"			// 파일 존재 유무 검사 함수 호출하기 위해서는 반드시 필요
#include "Dialog_KeyBoard.h"
#include "Dialog_KeyPad.h"
#include "MyBasicData.h"
#include "Dialog_Message.h"
#include "Alg_Mysql.h"
//#include "InterfaceGms.h"
#include "XgemClient.h"
#include "AlgMemory.h"
#include "CtlBd_Variable.h"
//#include "ClientGms.h"
#include "Dialog_Pass_Check.h"
#include "Dialog_Pass_Check.h"
#include ".\\Ctrl\\KeyBoard.h"
#include "DialogHifix.h"
#include "LogFromat.h"
// CScreenBasic

#define TM_GMS 100

IMPLEMENT_DYNCREATE(CScreenBasic, CFormView)

CScreenBasic::CScreenBasic()
	: CFormView(CScreenBasic::IDD)
{
	m_bTimerCheck = true;
	m_nCount = 0;
	st_handler_info.nModelChangeCheck = FALSE; //kwlee 2016.1221 자재 변경시 Tray 확인 메시지 출력
}

CScreenBasic::~CScreenBasic()
{
	
}

void CScreenBasic::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_MODEL_CREATE,		m_btnModelCreate);
	DDX_Control(pDX, IDC_BTN_MODEL_DELETE,		m_btnModelDelete);
	DDX_Control(pDX, IDC_BTN_APPLY,				m_btnApply);
	DDX_Control(pDX, IDC_BTN_RELOAD,			m_btnReLoad);
	DDX_Control(pDX, IDC_BTN_AUTO,				m_btnAuto);
	DDX_Control(pDX, IDC_GROUP_MODEL_INFO,		m_groupModelInfo);
	DDX_Control(pDX, IDC_GROUP_DEVICE_MODE,		m_groupDeviceMode);
	DDX_Control(pDX, IDC_CUSTOM_DEVICE_MODE,	m_pGridDevice);
	DDX_Control(pDX, IDC_CUSTOM_MODEL_LIST,		m_pGridModel);
	/*DDX_Control(pDX, IDC_DGT_ROW_CNT, m_dgtPcbRowCnt);*/
	//	DDX_Control(pDX, IDC_DGT_COL_CNT, m_dgtPcbColCnt);
	//DDX_Control(pDX, IDC_MSG_ROW_CNT, m_msgPcbRowCnt);
	//DDX_Control(pDX, IDC_MSG_COL_CNT, m_msgPcbColCnt);
	DDX_Control(pDX, IDC_STATIC_TRAY, m_groupCdimm_Tray);
	DDX_Control(pDX, IDC_DGT_GMS_TIME, m_dgtGmsTime);
	DDX_Control(pDX, IDC_MSG_GMS_TIME, m_msgGmsTime);
	DDX_Control(pDX, IDC_STATIC_GMS, m_groupGmsTime);
	DDX_Control(pDX, IDC_BTN_TIMER, m_btnTimer);
	DDX_Control(pDX, IDC_GROUP_BASIC_BARCODE_BANK, m_groupBcrBank);
	DDX_Control(pDX, IDC_MSG_BASIC_BARCODE_BANK, m_msgBcrBank);
	DDX_Control(pDX, IDC_DGT_BASIC_BARCODE_BANK, m_dgtBcrBank);
	DDX_Control(pDX, IDC_MSG_LENGTH, m_msg_length);
	DDX_Control(pDX, IDC_GROUP_BARCODE_INFO, m_group_barcode_info);
	DDX_Control(pDX, IDC_GROUP_BARCODE_1D, m_group_barcode_1d);
	DDX_Control(pDX, IDC_GROUP_BARCODE_2D, m_group_barcode_2d);
	DDX_Control(pDX, IDC_MSG_IBM, m_msg_IBM);
	DDX_Control(pDX, IDC_MSG_SAMSUNG, m_msg_SAMSUNG);
	DDX_Control(pDX, IDC_MSG_BARCODE_DATA, m_msg_barcode_data);
	DDX_Control(pDX, IDC_DGT_LENGTH, m_dgt_length);
	DDX_Control(pDX, IDC_DGT_IBM_LENGTH, m_dgt_ibm_length);
	DDX_Control(pDX, IDC_DGT_SAMSUNG_LENGTH, m_dgt_samsung_length);
	DDX_Control(pDX, IDC_MSG_SAMSUNG_BARCODE_DATA, m_msg_samsung_barcode_data);
	DDX_Control(pDX, IDC_CUSTOM_PGM_MODE, m_pGridPgmMode);
	DDX_Control(pDX, IDC_GROUP_BASIC_PGM_MODE, m_groupPgmMode);
	DDX_Control(pDX, IDC_GROUP_BASIC_HIFIX_INFO, m_groupHifix);
	DDX_Control(pDX, IDC_MSG_HIFIX_INFO, m_msgHifixInfo);
	DDX_Control(pDX, IDC_MSG_HIFIX_DATA, m_msgHifixData);
	DDX_Control(pDX, IDC_MSG_ABORT_TIME, m_msgAbortTime);
	DDX_Control(pDX, IDC_DGT_ABORT_TIME, m_dgtAbortTime);
	DDX_Control(pDX, IDC_MSG_BARCODE_CNT, m_msgBarcodeCnt);
	DDX_Control(pDX, IDC_DGT_BARCODE_CNT, m_dgtBarcodeCnt);
	DDX_Control(pDX, IDC_GROUP_AUTO, m_groupAutoMode);
	DDX_Control(pDX, IDC_DGT_STACKER_TRAY_CNT, m_dgtStackTrayCnt);
	DDX_Control(pDX, IDC_MSG_STACKER_TRAY_CNT, m_msgStackerTrayCnt);
	DDX_Control(pDX, IDC_GROUP_MANUAL_START, m_groupManualStart);
	DDX_Control(pDX, IDC_BTN_TEST1, m_btnTester1);
	DDX_Control(pDX, IDC_BTN_TEST2, m_btnTester2);
	DDX_Control(pDX, IDC_BTN_TEST3, m_btnTester3);
	DDX_Control(pDX, IDC_BTN_TEST4, m_btnTester4);
	DDX_Control(pDX, IDC_BTN_TEST6, m_btnTester6);
	DDX_Control(pDX, IDC_BTN_TEST5, m_btnTester5);
	DDX_Control(pDX, IDC_BTN_TEST7, m_btnTester7);
	DDX_Control(pDX, IDC_BTN_TEST8, m_btnTester8);
	DDX_Control(pDX, IDC_BTN_CHAMBER_1, m_btnChamber_1);
	DDX_Control(pDX, IDC_BTN_CHAMBER_2, m_Chamber_2);
	DDX_Control(pDX, IDC_BTN_CHAMBER_3, m_btnChamber_3);
	DDX_Control(pDX, IDC_BTN_CHAMBER_4, m_btnChamber_4);
	DDX_Control(pDX, IDC_BTN_CHAMBER_5, m_btnChamber_5);
	DDX_Control(pDX, IDC_BTN_CHAMBER_6, m_btnChamber_6);
	DDX_Control(pDX, IDC_BTN_CHAMBER_7, m_btnChamber_7);
	DDX_Control(pDX, IDC_BTN_CHAMBER_8, m_btnChamber_8);
	DDX_Control(pDX, IDC_GROUP_MANUAL_START2, m_groupManualChamberDoor);
	DDX_Control(pDX, IDC_DGT_ALARM_DELAY_CNT, m_dgtAlarmDelayCnt);
	DDX_Control(pDX, IDC_MSG_ALARM_DELAY_CNT2, m_msgAlarmDelayCnt);
	DDX_Control(pDX, IDC_GROUP_DIRECTION, m_groupDirection);
	DDX_Control(pDX, IDC_BTN_DIRECTION, m_btnDirection);
	DDX_Control(pDX, IDC_GROUP_AUTOLOAD, m_groupAutoLoad);
	DDX_Control(pDX, IDC_BTN_AUTOLOAD, m_btnAutoLoad);
	DDX_Control(pDX, IDC_GROUP_LOTEND_USE, m_groupLotEndUse);
	DDX_Control(pDX, IDC_BTN_LOTEND_USE, m_btnLotendUse);
	DDX_Control(pDX, IDC_MSG_REJECT_STACKER_TRAY, m_msg_Reject_Tray_Cnt);
	DDX_Control(pDX, IDC_DGT_REJECT_STACKER_TRAY, m_dgtRejectTrayCnt);
}

BEGIN_MESSAGE_MAP(CScreenBasic, CFormView)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_MODEL_LIST,	OnModelCellClick)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_MODEL_LIST,	OnModelCellRClick)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_PGM_MODE,	OnPgmModeCellClick)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_DEVICE_MODE, OnDeviceCellClick)
	ON_BN_CLICKED(IDC_BTN_MODEL_CREATE,			&CScreenBasic::OnClickedBtnModelCreate)
	ON_BN_CLICKED(IDC_BTN_MODEL_DELETE,			&CScreenBasic::OnClickedBtnModelDelete)
	ON_BN_CLICKED(IDC_BTN_RELOAD,				&CScreenBasic::OnClickedBtnReload)
	ON_BN_CLICKED(IDC_BTN_APPLY,				&CScreenBasic::OnClickedBtnApply)
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_STN_CLICKED(IDC_DGT_ROW_CNT, &CScreenBasic::OnStnClickedDgtRowCnt)
	ON_STN_CLICKED(IDC_DGT_COL_CNT, &CScreenBasic::OnStnClickedDgtColCnt)
	ON_STN_CLICKED(IDC_DGT_GMS_TIME, &CScreenBasic::OnStnClickedDgtGmsTime)
	ON_BN_CLICKED(IDC_BTN_TIMER, &CScreenBasic::OnBnClickedBtnTimer)
	ON_STN_CLICKED(IDC_DGT_BASIC_BARCODE_BANK, &CScreenBasic::OnStnClickedDgtBasicBarcodeBank)
	ON_STN_CLICKED(IDC_DGT_IBM_LENGTH, &CScreenBasic::OnStnClickedDgtIbmLength)
	ON_STN_CLICKED(IDC_DGT_SAMSUNG_LENGTH, &CScreenBasic::OnStnClickedDgtSamsungLength)
	ON_STN_CLICKED(IDC_MSG_BARCODE_DATA, &CScreenBasic::OnStnClickedMsgBarcodeData)
	ON_STN_CLICKED(IDC_DGT_LENGTH, &CScreenBasic::OnStnClickedDgtLength)

	
	ON_STN_CLICKED(IDC_MSG_HIFIX_DATA, &CScreenBasic::OnStnClickedMsgHifixData)
	ON_STN_CLICKED(IDC_DGT_ABORT_TIME, &CScreenBasic::OnStnClickedDgtAbortTime)
	ON_STN_CLICKED(IDC_DGT_BARCODE_CNT, &CScreenBasic::OnStnClickedDgtBarcodeCnt)
	ON_BN_CLICKED(IDC_BTN_AUTO, &CScreenBasic::OnBnClickedBtnAuto)
	ON_STN_CLICKED(IDC_DGT_STACKER_TRAY_CNT, &CScreenBasic::OnStnClickedDgtStackerTrayCnt)
	ON_BN_CLICKED(IDC_BTN_TEST1, &CScreenBasic::OnBnClickedBtnTest1)
	ON_BN_CLICKED(IDC_BTN_TEST2, &CScreenBasic::OnBnClickedBtnTest2)
	ON_BN_CLICKED(IDC_BTN_TEST3, &CScreenBasic::OnBnClickedBtnTest3)
	ON_BN_CLICKED(IDC_BTN_TEST4, &CScreenBasic::OnBnClickedBtnTest4)
	ON_BN_CLICKED(IDC_BTN_TEST5, &CScreenBasic::OnBnClickedBtnTest5)
	ON_BN_CLICKED(IDC_BTN_TEST6, &CScreenBasic::OnBnClickedBtnTest6)
	ON_BN_CLICKED(IDC_BTN_TEST7, &CScreenBasic::OnBnClickedBtnTest7)
	ON_BN_CLICKED(IDC_BTN_TEST8, &CScreenBasic::OnBnClickedBtnTest8)
	ON_BN_CLICKED(IDC_BTN_CHAMBER_1, &CScreenBasic::OnBnClickedBtnChamber1)
	ON_BN_CLICKED(IDC_BTN_CHAMBER_2, &CScreenBasic::OnBnClickedBtnChamber2)
	ON_BN_CLICKED(IDC_BTN_CHAMBER_3, &CScreenBasic::OnBnClickedBtnChamber3)
	ON_BN_CLICKED(IDC_BTN_CHAMBER_4, &CScreenBasic::OnBnClickedBtnChamber4)
	ON_BN_CLICKED(IDC_BTN_CHAMBER_5, &CScreenBasic::OnBnClickedBtnChamber5)
	ON_BN_CLICKED(IDC_BTN_CHAMBER_6, &CScreenBasic::OnBnClickedBtnChamber6)
	ON_BN_CLICKED(IDC_BTN_CHAMBER_7, &CScreenBasic::OnBnClickedBtnChamber7)
	ON_BN_CLICKED(IDC_BTN_CHAMBER_8, &CScreenBasic::OnBnClickedBtnChamber8)
	ON_BN_CLICKED(IDC_BTN_DIRECTION, &CScreenBasic::OnBnClickedBtnDirection)
	ON_STN_CLICKED(IDC_DGT_ALARM_DELAY_CNT, &CScreenBasic::OnStnClickedDgtAlarmDelayCnt)
	ON_BN_CLICKED(IDC_BTN_AUTOLOAD, &CScreenBasic::OnBnClickedBtnAutoload)
	ON_BN_CLICKED(IDC_BTN_LOTEND_USE, &CScreenBasic::OnBnClickedBtnLotendUse)
	ON_STN_CLICKED(IDC_DGT_REJECT_STACKER_TRAY, &CScreenBasic::OnStnClickedDgtRejectStackerTray)
END_MESSAGE_MAP()


// CScreenBasic 진단입니다.

#ifdef _DEBUG
void CScreenBasic::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CScreenBasic::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CScreenBasic 메시지 처리기입니다.


void CScreenBasic::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	OnDataInit();
	OnInitGroupBox();
	OnInitLabel();
	OnInitDigit();
	OnInitButton();
	
	OnInitGridDeviceMode();
	OnInitGridModelList();
	//OnInitGridPgmMode();
	UpdateData(FALSE);
	
	st_handler_info.cWndBasic = this;
}


BOOL CScreenBasic::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class
/*	if (OnDataComparison() == RET_ERROR)
	{
		OnDataHistoryLog();
		OnDataApply();
	}
	st_handler_info.cWndBasic = NULL;*/
	return CFormView::DestroyWindow();
}

void CScreenBasic::OnInitGroupBox()
{
	m_groupModelInfo.SetFont(clsFunc.OnLogFont(16));
	m_groupModelInfo.SetCatptionTextColor(BLUE_C);
	m_groupModelInfo.SetBorderColor(ORANGE_C);
	m_groupModelInfo.SetFontBold(TRUE);
	m_groupModelInfo.SetBackgroundColor(WINDOW_UP);

	m_groupDeviceMode.SetFont(clsFunc.OnLogFont(16));
	m_groupDeviceMode.SetCatptionTextColor(BLUE_C);
	m_groupDeviceMode.SetBorderColor(ORANGE_C);
	m_groupDeviceMode.SetFontBold(TRUE);
	m_groupDeviceMode.SetBackgroundColor(WINDOW_UP);

	m_groupCdimm_Tray.SetFont(clsFunc.OnLogFont(16));
	m_groupCdimm_Tray.SetCatptionTextColor(BLUE_C);
	m_groupCdimm_Tray.SetBorderColor(ORANGE_C);
	m_groupCdimm_Tray.SetFontBold(TRUE);
	m_groupCdimm_Tray.SetBackgroundColor(WINDOW_UP);

	m_groupGmsTime.SetFont(clsFunc.OnLogFont(16));
	m_groupGmsTime.SetCatptionTextColor(BLUE_C);
	m_groupGmsTime.SetBorderColor(ORANGE_C);
	m_groupGmsTime.SetFontBold(TRUE);
	m_groupGmsTime.SetBackgroundColor(WINDOW_UP);

	m_groupBcrBank.SetFont(clsFunc.OnLogFont(16));
	m_groupBcrBank.SetCatptionTextColor(BLUE_C);
	m_groupBcrBank.SetBorderColor(ORANGE_C);
	m_groupBcrBank.SetBackgroundColor(WINDOW_UP1);
	m_groupBcrBank.SetFontBold(TRUE);


	m_groupPgmMode.SetFont(clsFunc.OnLogFont(16));
	m_groupPgmMode.SetCatptionTextColor(BLUE_C);
	m_groupPgmMode.SetBorderColor(ORANGE_C);
	m_groupPgmMode.SetBackgroundColor(WINDOW_UP1);
	m_groupPgmMode.SetFontBold(TRUE);
	
	//kwlee 2016.0727
	m_group_barcode_info.SetFont(clsFunc.OnLogFont(16));
	m_group_barcode_info.SetCatptionTextColor(BLUE_C);
	m_group_barcode_info.SetBorderColor(ORANGE_C);
	m_group_barcode_info.SetBackgroundColor(WINDOW_UP1);
	m_group_barcode_info.SetFontBold(TRUE);

	m_group_barcode_1d.SetFont(clsFunc.OnLogFont(16));
	m_group_barcode_1d.SetCatptionTextColor(BLUE_C);
	m_group_barcode_1d.SetBorderColor(ORANGE_C);
	m_group_barcode_1d.SetBackgroundColor(WINDOW_UP1);
	m_group_barcode_1d.SetFontBold(TRUE);

	m_group_barcode_2d.SetFont(clsFunc.OnLogFont(16));
	m_group_barcode_2d.SetCatptionTextColor(BLUE_C);
	m_group_barcode_2d.SetBorderColor(ORANGE_C);
	m_group_barcode_2d.SetBackgroundColor(WINDOW_UP1);
	m_group_barcode_2d.SetFontBold(TRUE);

	m_groupAutoMode.SetFont(clsFunc.OnLogFont(16));
	m_groupAutoMode.SetCatptionTextColor(BLUE_C);
	m_groupAutoMode.SetBorderColor(ORANGE_C);
	m_groupAutoMode.SetBackgroundColor(WINDOW_UP1);
	m_groupAutoMode.SetFontBold(TRUE);

	m_groupHifix.SetFont(clsFunc.OnLogFont(16));
	m_groupHifix.SetCatptionTextColor(BLUE_C);
	m_groupHifix.SetBorderColor(ORANGE_C);
	m_groupHifix.SetBackgroundColor(WINDOW_UP1);
	m_groupHifix.SetFontBold(TRUE);

	m_groupManualStart.SetFont(clsFunc.OnLogFont(16));
	m_groupManualStart.SetCatptionTextColor(BLUE_C);
	m_groupManualStart.SetBorderColor(ORANGE_C);
	m_groupManualStart.SetFontBold(TRUE);
	m_groupManualStart.SetBackgroundColor(WINDOW_UP);

	m_groupManualChamberDoor.SetFont(clsFunc.OnLogFont(16));
	m_groupManualChamberDoor.SetCatptionTextColor(BLUE_C);
	m_groupManualChamberDoor.SetBorderColor(ORANGE_C);
	m_groupManualChamberDoor.SetFontBold(TRUE);
	m_groupManualChamberDoor.SetBackgroundColor(WINDOW_UP);

	//kwlee 2016.1124
	m_groupDirection.SetFont(clsFunc.OnLogFont(16));
	m_groupDirection.SetCatptionTextColor(BLUE_C);
	m_groupDirection.SetBorderColor(ORANGE_C);
	m_groupDirection.SetBackgroundColor(WINDOW_UP1);
	m_groupDirection.SetFontBold(TRUE);
	//

	//kwlee 2016.1124
	m_groupAutoMode.SetFont(clsFunc.OnLogFont(16));
	m_groupAutoMode.SetCatptionTextColor(BLUE_C);
	m_groupAutoMode.SetBorderColor(ORANGE_C);
	m_groupAutoMode.SetBackgroundColor(WINDOW_UP1);
	m_groupAutoMode.SetFontBold(TRUE);

	//kwlee 2016.1201
	m_groupAutoLoad.SetFont(clsFunc.OnLogFont(16));
	m_groupAutoLoad.SetCatptionTextColor(BLUE_C);
	m_groupAutoLoad.SetBorderColor(ORANGE_C);
	m_groupAutoLoad.SetBackgroundColor(WINDOW_UP1);
	m_groupAutoLoad.SetFontBold(TRUE);
}


void CScreenBasic::OnInitLabel()
{
// 	m_msgPcbRowCnt.SetFont(clsFunc.m_pFont[5]);
// 	m_msgPcbRowCnt.SetWindowText(_T("RowCount"));
// 	m_msgPcbRowCnt.SetCenterText();
// 	m_msgPcbRowCnt.SetColor(WHITE_C);
// 	m_msgPcbRowCnt.SetGradientColor(ORANGE_C);
// 	m_msgPcbRowCnt.SetTextColor(BLACK_C);
// 
// 	m_msgPcbColCnt.SetFont(clsFunc.m_pFont[5]);
// 	m_msgPcbColCnt.SetWindowText(_T("ColCount"));
// 	m_msgPcbColCnt.SetCenterText();
// 	m_msgPcbColCnt.SetColor(WHITE_C);
// 	m_msgPcbColCnt.SetGradientColor(ORANGE_C);
// 	m_msgPcbColCnt.SetTextColor(BLACK_C);

	m_msgGmsTime.SetFont(clsFunc.m_pFont[5]);
	m_msgGmsTime.SetWindowText(_T("GmsTime"));
	m_msgGmsTime.SetCenterText();
	m_msgGmsTime.SetColor(WHITE_C);
	m_msgGmsTime.SetGradientColor(ORANGE_C);
	m_msgGmsTime.SetTextColor(BLACK_C);

	m_msgBcrBank.SetFont(clsFunc.m_pFont[5]);
	m_msgBcrBank.SetWindowText(_T("Barcode Bank"));
	m_msgBcrBank.SetCenterText();
	m_msgBcrBank.SetColor(WHITE_C);
	m_msgBcrBank.SetGradientColor(ORANGE_C);
	m_msgBcrBank.SetTextColor(BLACK_C);

	//kwlee 2016.0727
	m_msg_length.SetFont(clsFunc.m_pFont[5]);
	m_msg_length.SetWindowText(_T("Length"));
	m_msg_length.SetCenterText();
	m_msg_length.SetColor(WHITE_C);
	m_msg_length.SetGradientColor(ORANGE_C);
	m_msg_length.SetTextColor(BLACK_C);

	m_msg_IBM.SetFont(clsFunc.m_pFont[5]);
	m_msg_IBM.SetWindowText(_T("IBM"));
	m_msg_IBM.SetCenterText();
	m_msg_IBM.SetColor(WHITE_C);
	m_msg_IBM.SetGradientColor(ORANGE_C);
	m_msg_IBM.SetTextColor(BLACK_C);

	m_msg_SAMSUNG.SetFont(clsFunc.m_pFont[5]);
	m_msg_SAMSUNG.SetWindowText(_T("Samsung"));
	m_msg_SAMSUNG.SetCenterText();
	m_msg_SAMSUNG.SetColor(WHITE_C);
	m_msg_SAMSUNG.SetGradientColor(ORANGE_C);
	m_msg_SAMSUNG.SetTextColor(BLACK_C);

	m_msg_barcode_data.SetFont(clsFunc.m_pFont[5]);
	m_msg_barcode_data.SetWindowText(m_strBarcodeData[1]);
	m_msg_barcode_data.SetCenterText();
	m_msg_barcode_data.SetColor(WHITE_C);
	m_msg_barcode_data.SetGradientColor(WHITE_C);
	m_msg_barcode_data.SetTextColor(BLACK_C);

	m_msg_samsung_barcode_data.SetFont(clsFunc.m_pFont[5]);
	m_msg_samsung_barcode_data.SetWindowText(_T("Samsung Barcode Data"));
	m_msg_samsung_barcode_data.SetCenterText();
	m_msg_samsung_barcode_data.SetColor(WHITE_C);
	m_msg_samsung_barcode_data.SetGradientColor(ORANGE_C);
	m_msg_samsung_barcode_data.SetTextColor(BLACK_C);
	
	//kwlee 2016.0809
	m_msgHifixInfo.SetFont(clsFunc.m_pFont[5]);
	m_msgHifixInfo.SetWindowText(_T("Hifix Info"));
	m_msgHifixInfo.SetCenterText();
	m_msgHifixInfo.SetColor(WHITE_C);
	m_msgHifixInfo.SetGradientColor(ORANGE_C);
	m_msgHifixInfo.SetTextColor(BLACK_C);

	m_msgHifixData.SetFont(clsFunc.m_pFont[5]);
	m_msgHifixData.SetWindowText(m_strHifixData[1]);
	m_msgHifixData.SetCenterText();
	m_msgHifixData.SetColor(WHITE_C);
	m_msgHifixData.SetGradientColor(WHITE_C);
	m_msgHifixData.SetTextColor(BLACK_C);

	m_msgAbortTime.SetFont(clsFunc.m_pFont[5]);
	m_msgAbortTime.SetWindowText(_T("Abort Time"));
	m_msgAbortTime.SetCenterText();
	m_msgAbortTime.SetColor(WHITE_C);
	m_msgAbortTime.SetGradientColor(ORANGE_C);
	m_msgAbortTime.SetTextColor(BLACK_C);

	m_msgBarcodeCnt.SetFont(clsFunc.m_pFont[5]);
	m_msgBarcodeCnt.SetWindowText(_T("1D Barcode Cnt"));
	m_msgBarcodeCnt.SetCenterText();
	m_msgBarcodeCnt.SetColor(WHITE_C);
	m_msgBarcodeCnt.SetGradientColor(ORANGE_C);
	m_msgBarcodeCnt.SetTextColor(BLACK_C);

	//kwlee 2016.0930
	m_msgStackerTrayCnt.SetFont(clsFunc.m_pFont[5]);
	m_msgStackerTrayCnt.SetWindowText(_T("Stacker Tray Cnt"));
	m_msgStackerTrayCnt.SetCenterText();
	m_msgStackerTrayCnt.SetColor(WHITE_C);
	m_msgStackerTrayCnt.SetGradientColor(ORANGE_C);
	m_msgStackerTrayCnt.SetTextColor(BLACK_C);

	m_msgAlarmDelayCnt.SetFont(clsFunc.m_pFont[5]);
	m_msgAlarmDelayCnt.SetWindowText(_T("Alarm Delay Cnt"));
	m_msgAlarmDelayCnt.SetCenterText();
	m_msgAlarmDelayCnt.SetColor(WHITE_C);
	m_msgAlarmDelayCnt.SetGradientColor(ORANGE_C);
	m_msgAlarmDelayCnt.SetTextColor(BLACK_C);

	//kwlee 2017.0609
	m_msg_Reject_Tray_Cnt.SetFont(clsFunc.m_pFont[5]);
	m_msg_Reject_Tray_Cnt.SetWindowText(_T("Reject Stk Tray Cnt"));
	m_msg_Reject_Tray_Cnt.SetCenterText();
	m_msg_Reject_Tray_Cnt.SetColor(WHITE_C);
	m_msg_Reject_Tray_Cnt.SetGradientColor(ORANGE_C);
	m_msg_Reject_Tray_Cnt.SetTextColor(BLACK_C);
}


void CScreenBasic::OnInitDigit()
{

// 	m_dgtPcbRowCnt.SetStyle(CDigit::DS_INT, 2, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
// 	m_dgtPcbRowCnt.SetVal(m_nRowCnt[1]);
// 
// 	m_dgtPcbColCnt.SetStyle(CDigit::DS_INT, 2, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
// 	m_dgtPcbColCnt.SetVal(m_nColCnt[1]);

	m_dgtGmsTime.SetStyle(CDigit::DS_INT, 4, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgtGmsTime.SetVal(m_nGmsTime[1]);

// 	m_dgtBcrBank.SetStyle(CDigit::DS_INT, 2, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
// 	m_dgtBcrBank.SetVal(m_nBcrBank[1]);

// 	m_dgt_length.SetStyle(CDigit::DS_INT, 2, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
// 	m_dgt_length.SetVal(m_nBcr_1d_Len[1]);
// 
// 	m_dgt_ibm_length.SetStyle(CDigit::DS_INT, 2, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
// 	m_dgt_ibm_length.SetVal(m_nBcr_2d_IBM_Len[1]);
// 
// 	m_dgt_samsung_length.SetStyle(CDigit::DS_INT, 2, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
// 	m_dgt_samsung_length.SetVal(m_nBcr_2d_SAMSUNG_Len[1]);

	m_dgtAbortTime.SetStyle(CDigit::DS_INT, 5, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgtAbortTime.SetVal(m_nAbortTime[1]);

	m_dgtBarcodeCnt.SetStyle(CDigit::DS_INT, 4, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgtBarcodeCnt.SetVal(m_nBarcodeCnt[1]);

	//kwlee 2016.0930
	m_dgtStackTrayCnt.SetStyle(CDigit::DS_INT, 4, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgtStackTrayCnt.SetVal(m_nStackerTrayCnt[1]);

	//kwlee 2016.1124
	m_dgtAlarmDelayCnt.SetStyle(CDigit::DS_INT, 4, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgtAlarmDelayCnt.SetVal(m_nAlarmDelayCnt[1]);

	//kwlee 2017.0609
	m_dgtRejectTrayCnt.SetStyle(CDigit::DS_INT, 4, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgtRejectTrayCnt.SetVal(m_nRejectStackerTrayCnt[1]);
}


void CScreenBasic::OnInitButton()
{
	m_btnModelCreate.SetBitmaps(IDC_BTN_MODEL_CREATE, IDB_BITMAP_CREATE_DN, WINDOW_DN, IDB_BITMAP_CREATE_UP, WINDOW_UP);
	m_btnModelCreate.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnModelCreate.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnModelCreate.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnModelCreate.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnModelCreate.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnModelCreate.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnModelCreate.SetFont(clsFunc.m_pFont[3]);
	m_btnModelCreate.SetTooltipText(_T("Model Create"));

	m_btnModelDelete.SetBitmaps(IDC_BTN_MODEL_DELETE, IDB_BITMAP_DELETE_DN, WINDOW_DN, IDB_BITMAP_DELETE_UP, WINDOW_UP);
	m_btnModelDelete.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnModelDelete.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnModelDelete.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnModelDelete.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnModelDelete.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnModelDelete.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnModelDelete.SetFont(clsFunc.m_pFont[3]);
	m_btnModelDelete.SetTooltipText(_T("Model Delete"));

	m_btnApply.SetBitmaps(IDC_BTN_APPLY, IDB_BITMAP_APPLY_DN, WINDOW_DN, IDB_BITMAP_APPLY_UP, WINDOW_UP);
	m_btnApply.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnApply.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnApply.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnApply.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnApply.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnApply.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnApply.SetFont(clsFunc.m_pFont[3]);
	m_btnApply.SetTooltipText(_T("Basic Data Apply"));

	m_btnReLoad.SetBitmaps(IDC_BTN_RELOAD, IDB_BITMAP_RELOAD_DN, WINDOW_DN, IDB_BITMAP_RELOAD_UP, WINDOW_UP);
	m_btnReLoad.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnReLoad.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnReLoad.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnReLoad.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnReLoad.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnReLoad.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnReLoad.SetFont(clsFunc.m_pFont[3]);
	m_btnReLoad.SetTooltipText(_T("Basic Data ReLoad"));

	m_btnTimer.SetBitmaps(IDC_BTN_MODEL_CREATE, IDB_BITMAP_CREATE_DN, WINDOW_DN, IDB_BITMAP_CREATE_UP, WINDOW_UP);
	m_btnTimer.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnTimer.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnTimer.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnTimer.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnTimer.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnTimer.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnTimer.SetFont(clsFunc.m_pFont[3]);
	m_btnTimer.SetTooltipText(_T("START/STOP"));

	m_btnAuto.SetBitmaps(IDC_BTN_AUTO, IDB_BITMAP_APPLY_DN, WINDOW_DN, IDB_BITMAP_APPLY_UP, WINDOW_UP);
	m_btnAuto.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnAuto.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnAuto.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnAuto.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnAuto.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnAuto.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnAuto.SetFont(clsFunc.m_pFont[3]);
	m_btnAuto.SetTooltipText(_T("Basic Data Apply"));

	//kwlee 2016.1011
	m_btnTester1.SetBitmaps(IDC_BTN_LD_CVY_LOT_INFO_2, IDB_NETWORK_ON_DN, WINDOW_DN, IDB_NETWORK_ON_UP, WINDOW_UP);
	m_btnTester1.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnTester1.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnTester1.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnTester1.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnTester1.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnTester1.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnTester1.SetFont(clsFunc.m_pFont[3]);
	m_btnTester1.SetTooltipText(_T("Tester_1"));

	m_btnTester2.SetBitmaps(IDC_BTN_LD_CVY_LOT_INFO_2, IDB_NETWORK_ON_DN, WINDOW_DN, IDB_NETWORK_ON_UP, WINDOW_UP);
	m_btnTester2.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnTester2.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnTester2.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnTester2.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnTester2.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnTester2.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnTester2.SetFont(clsFunc.m_pFont[3]);
	m_btnTester2.SetTooltipText(_T("Tester_2"));

	m_btnTester3.SetBitmaps(IDC_BTN_LD_CVY_LOT_INFO_2, IDB_NETWORK_ON_DN, WINDOW_DN, IDB_NETWORK_ON_UP, WINDOW_UP);
	m_btnTester3.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnTester3.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnTester3.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnTester3.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnTester3.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnTester3.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnTester3.SetFont(clsFunc.m_pFont[3]);
	m_btnTester3.SetTooltipText(_T("Tester_3"));

	m_btnTester4.SetBitmaps(IDC_BTN_LD_CVY_LOT_INFO_2, IDB_NETWORK_ON_DN, WINDOW_DN, IDB_NETWORK_ON_UP, WINDOW_UP);
	m_btnTester4.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnTester4.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnTester4.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnTester4.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnTester4.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnTester4.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnTester4.SetFont(clsFunc.m_pFont[3]);
	m_btnTester4.SetTooltipText(_T("Tester_4"));

	m_btnTester5.SetBitmaps(IDC_BTN_LD_CVY_LOT_INFO_2, IDB_NETWORK_ON_DN, WINDOW_DN, IDB_NETWORK_ON_UP, WINDOW_UP);
	m_btnTester5.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnTester5.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnTester5.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnTester5.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnTester5.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnTester5.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnTester5.SetFont(clsFunc.m_pFont[3]);
	m_btnTester5.SetTooltipText(_T("Tester_5"));

	m_btnTester6.SetBitmaps(IDC_BTN_LD_CVY_LOT_INFO_2, IDB_NETWORK_ON_DN, WINDOW_DN, IDB_NETWORK_ON_UP, WINDOW_UP);
	m_btnTester6.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnTester6.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnTester6.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnTester6.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnTester6.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnTester6.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnTester6.SetFont(clsFunc.m_pFont[3]);
	m_btnTester6.SetTooltipText(_T("Tester_6"));

	m_btnTester7.SetBitmaps(IDC_BTN_LD_CVY_LOT_INFO_2, IDB_NETWORK_ON_DN, WINDOW_DN, IDB_NETWORK_ON_UP, WINDOW_UP);
	m_btnTester7.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnTester7.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnTester7.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnTester7.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnTester7.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnTester7.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnTester7.SetFont(clsFunc.m_pFont[3]);
	m_btnTester7.SetTooltipText(_T("Tester_7"));

	m_btnTester8.SetBitmaps(IDC_BTN_LD_CVY_LOT_INFO_2, IDB_NETWORK_ON_DN, WINDOW_DN, IDB_NETWORK_ON_UP, WINDOW_UP);
	m_btnTester8.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnTester8.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnTester8.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnTester8.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnTester8.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnTester8.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnTester8.SetFont(clsFunc.m_pFont[3]);
	m_btnTester8.SetTooltipText(_T("Tester_8"));


	
	m_btnChamber_1.SetBitmaps(IDC_BTN_LD_CVY_LOT_INFO_2, IDB_NETWORK_ON_DN, WINDOW_DN, IDB_NETWORK_ON_UP, WINDOW_UP);
	m_btnChamber_1.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnChamber_1.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnChamber_1.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnChamber_1.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnChamber_1.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnChamber_1.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnChamber_1.SetFont(clsFunc.m_pFont[3]);
	m_btnChamber_1.SetTooltipText(_T("Chamber1"));

	m_Chamber_2.SetBitmaps(IDC_BTN_LD_CVY_LOT_INFO_2, IDB_NETWORK_ON_DN, WINDOW_DN, IDB_NETWORK_ON_UP, WINDOW_UP);
	m_Chamber_2.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_Chamber_2.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_Chamber_2.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_Chamber_2.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_Chamber_2.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_Chamber_2.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_Chamber_2.SetFont(clsFunc.m_pFont[3]);
	m_Chamber_2.SetTooltipText(_T("Chamber2"));

	m_btnChamber_3.SetBitmaps(IDC_BTN_LD_CVY_LOT_INFO_2, IDB_NETWORK_ON_DN, WINDOW_DN, IDB_NETWORK_ON_UP, WINDOW_UP);
	m_btnChamber_3.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnChamber_3.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnChamber_3.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnChamber_3.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnChamber_3.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnChamber_3.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnChamber_3.SetFont(clsFunc.m_pFont[3]);
	m_btnChamber_3.SetTooltipText(_T("Chamber3"));

	m_btnChamber_4.SetBitmaps(IDC_BTN_LD_CVY_LOT_INFO_2, IDB_NETWORK_ON_DN, WINDOW_DN, IDB_NETWORK_ON_UP, WINDOW_UP);
	m_btnChamber_4.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnChamber_4.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnChamber_4.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnChamber_4.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnChamber_4.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnChamber_4.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnChamber_4.SetFont(clsFunc.m_pFont[3]);
	m_btnChamber_4.SetTooltipText(_T("Chamber4"));

	m_btnChamber_5.SetBitmaps(IDC_BTN_LD_CVY_LOT_INFO_2, IDB_NETWORK_ON_DN, WINDOW_DN, IDB_NETWORK_ON_UP, WINDOW_UP);
	m_btnChamber_5.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnChamber_5.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnChamber_5.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnChamber_5.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnChamber_5.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnChamber_5.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnChamber_5.SetFont(clsFunc.m_pFont[3]);
	m_btnChamber_5.SetTooltipText(_T("Chamber5"));

	m_btnChamber_6.SetBitmaps(IDC_BTN_LD_CVY_LOT_INFO_2, IDB_NETWORK_ON_DN, WINDOW_DN, IDB_NETWORK_ON_UP, WINDOW_UP);
	m_btnChamber_6.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnChamber_6.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnChamber_6.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnChamber_6.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnChamber_6.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnChamber_6.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnChamber_6.SetFont(clsFunc.m_pFont[3]);
	m_btnChamber_6.SetTooltipText(_T("Chamber6"));

	m_btnChamber_7.SetBitmaps(IDC_BTN_LD_CVY_LOT_INFO_2, IDB_NETWORK_ON_DN, WINDOW_DN, IDB_NETWORK_ON_UP, WINDOW_UP);
	m_btnChamber_7.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnChamber_7.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnChamber_7.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnChamber_7.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnChamber_7.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnChamber_7.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnChamber_7.SetFont(clsFunc.m_pFont[3]);
	m_btnChamber_7.SetTooltipText(_T("Chamber7"));

	m_btnChamber_8.SetBitmaps(IDC_BTN_LD_CVY_LOT_INFO_2, IDB_NETWORK_ON_DN, WINDOW_DN, IDB_NETWORK_ON_UP, WINDOW_UP);
	m_btnChamber_8.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnChamber_8.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnChamber_8.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnChamber_8.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnChamber_8.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnChamber_8.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnChamber_8.SetFont(clsFunc.m_pFont[3]);
	m_btnChamber_8.SetTooltipText(_T("Chamber8"));

	//kwlee 2016.1124
	m_btnDirection.SetBitmaps(IDC_BTN_DIRECTION, IDB_BITMAP_APPLY_DN, WINDOW_DN, IDB_BITMAP_APPLY_UP, WINDOW_UP);
	m_btnDirection.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnDirection.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnDirection.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnDirection.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnDirection.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnDirection.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnDirection.SetFont(clsFunc.m_pFont[3]);


	//kwlee 2016.1201
	m_btnAutoLoad.SetBitmaps(IDC_BTN_AUTOLOAD, IDB_BITMAP_APPLY_DN, WINDOW_DN, IDB_BITMAP_APPLY_UP, WINDOW_UP);
	m_btnAutoLoad.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnAutoLoad.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnAutoLoad.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnAutoLoad.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnAutoLoad.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnAutoLoad.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnAutoLoad.SetFont(clsFunc.m_pFont[3]);

	//kwlee 2016.0225
	m_btnLotendUse.SetBitmaps(IDC_BTN_LOTEND_USE, IDB_BITMAP_APPLY_DN, WINDOW_DN, IDB_BITMAP_APPLY_UP, WINDOW_UP);
	m_btnLotendUse.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnLotendUse.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnLotendUse.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnLotendUse.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnLotendUse.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnLotendUse.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnLotendUse.SetFont(clsFunc.m_pFont[3]);

	// kwlee 20160929
	if (st_handler_info.nAutoMode == YES)
	{

		m_btnAuto.SetBitmaps(IDC_BTN_AUTO, IDB_BITMAP_APPLY_DN, WINDOW_DN, IDB_BITMAP_APPLY_UP, WINDOW_UP);;
		m_btnAuto.SetWindowTextW(_T("Auto Mode"));
		
	}
	else
	{

		m_btnAuto.SetBitmaps(IDC_BTN_AUTO, IDB_BITMAP_APPLY_DN, WINDOW_DN, IDB_BITMAP_APPLY_UP, WINDOW_UP);
		m_btnAuto.SetWindowTextW(_T("Manual Mode"));

	}

	//kwlee 2016.1124
	if (st_basic_info.nDirectionCheckSkip == YES)
	{
		m_btnDirection.SetBitmaps(IDC_BTN_DIRECTION, IDB_BITMAP_APPLY_DN, WINDOW_DN, IDB_BITMAP_APPLY_UP, WINDOW_UP);;
		m_btnDirection.SetWindowTextW(_T("Direction Skip"));

	}
	else
	{
		m_btnDirection.SetBitmaps(IDC_BTN_DIRECTION, IDB_BITMAP_APPLY_DN, WINDOW_DN, IDB_BITMAP_APPLY_UP, WINDOW_UP);
		m_btnDirection.SetWindowTextW(_T("Direction Check Use"));

	}

	//kwlee 2016.1201
	if (st_basic_info.nAutoLoadMode == YES)
	{
		m_btnAutoLoad.SetBitmaps(IDC_BTN_AUTOLOAD, IDB_BITMAP_APPLY_DN, WINDOW_DN, IDB_BITMAP_APPLY_UP, WINDOW_UP);;
		m_btnAutoLoad.SetWindowTextW(_T("Auto Load Use"));

	}
	else
	{
		m_btnAutoLoad.SetBitmaps(IDC_BTN_AUTOLOAD, IDB_BITMAP_APPLY_DN, WINDOW_DN, IDB_BITMAP_APPLY_UP, WINDOW_UP);
		m_btnAutoLoad.SetWindowTextW(_T("Auto Load Skip"));

	}


	//kwlee 2016.0225
	if (st_basic_info.nLotEndSkipMode == YES)
	{
		m_btnLotendUse.SetBitmaps(IDC_BTN_LOTEND_USE, IDB_BITMAP_APPLY_DN, WINDOW_DN, IDB_BITMAP_APPLY_UP, WINDOW_UP);;
		m_btnLotendUse.SetWindowTextW(_T("Lot End Skip(서버랑 통신 않함)"));
	}
	else
	{
		m_btnLotendUse.SetBitmaps(IDC_BTN_LOTEND_USE, IDB_BITMAP_APPLY_DN, WINDOW_DN, IDB_BITMAP_APPLY_UP, WINDOW_UP);
		m_btnLotendUse.SetWindowTextW(_T("Lot End Use(서버랑 통신 함)"));
	}
}
//kwlee 2016.0809
void CScreenBasic::OnInitGridPgmMode()
{
	int   i, j;
	int	  nMaxRow, nMaxCol;
	//int   nRow;
	CString strTmp;

	nMaxRow = 9; // unloader tray y count
	//nMaxCol = 3;
	nMaxCol = 2;

	m_pGridPgmMode.SetFrameFocusCell(FALSE);
	m_pGridPgmMode.SetTrackFocusCell(FALSE);
	m_pGridPgmMode.EnableSelection(FALSE);

	m_pGridPgmMode.SetGridLineColor(BLACK_C);
	m_pGridPgmMode.SetGridLines(1);

	m_pGridPgmMode.SetRowCount(nMaxRow);
	m_pGridPgmMode.SetColumnCount(nMaxCol);

	m_pGridPgmMode.SetFixedRowCount(0);
	m_pGridPgmMode.SetFixedColumnCount(0);
	m_pGridPgmMode.SetFixedBkColor(RGB(0,0,200));
	m_pGridPgmMode.SetFixedBkColor(RGB(200,200,255));
	m_pGridPgmMode.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<nMaxRow; i++) 
	{
		m_pGridPgmMode.SetRowHeight(i, 32);

		for (j=0; j<nMaxCol; j++) 
		{
			//m_pGridDegree.SetColumnWidth(j, 210);
		//	m_pGridPgmMode.SetColumnWidth(j,180);
			m_pGridPgmMode.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridPgmMode.SetItemState(i, j, GVIS_READONLY);
			m_pGridPgmMode.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
		}
	}

	//nRow = 0;
	m_pGridPgmMode.MergeCells(0, 0, 0, 1);
	m_pGridPgmMode.SetItemBkColour(0, 0, BLUE_D, WHITE_C);
	m_pGridPgmMode.SetItemFont(0, 0, &clsFunc.OnLogFont(18));
	m_pGridPgmMode.SetItemText(0, 0, _T("PGM MODE"));

	for (i =0; i<8; i++)
	{
		m_pGridPgmMode.SetColumnWidth(0,80);
		m_pGridPgmMode.SetItemBkColour(i+1, 0, BLUE_D, WHITE_C);
		m_pGridPgmMode.SetItemFont(i+1, 0, &clsFunc.OnLogFont(14));
		strTmp.Format(_T("TestSite_%d"),i+1);
		m_pGridPgmMode.SetItemText(i+1, 0, strTmp);

		m_pGridPgmMode.SetColumnWidth(1,120);
		m_pGridPgmMode.SetItemBkColour(i+1, 1, GREEN_D, BLACK_C);
		m_pGridPgmMode.SetItemFont(i+1, 1, &clsFunc.OnLogFont(14));

		if (m_nFtpUse[1] == FALSE)
		{
			m_pGridPgmMode.SetItemText(i+1, 1, _T("FTP"));
			m_pGridPgmMode.SetItemBkColour(i+1, 1 , GREEN_C, BLACK_C);
			
		}
		else
		{
			m_pGridPgmMode.SetItemText(i+1, 1, _T("LOCAL"));
			m_pGridPgmMode.SetItemBkColour(i+1, 1 , GREEN_D, BLACK_C);
		}
		//m_pGridPgmMode.SetItemText(i+1, 1, _T("FTP"));

// 		m_pGridPgmMode.SetColumnWidth(2,60);
// 		m_pGridPgmMode.SetItemBkColour(i+1, 2, GREEN_D, BLACK_C);
// 		m_pGridPgmMode.SetItemFont(i+1, 2, &clsFunc.OnLogFont(14));
// 		m_pGridPgmMode.SetItemText(i+1, 2, _T("LOCAL"));
	}

	

	m_pGridPgmMode.Invalidate(FALSE);
}

void CScreenBasic::OnPgmModeCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{

// 	int nRow, nCol;
// 	INT_PTR nResponse;
// 
// 	CString strTmp;
// 	CString strMsg;
// 	CDialog_Message dlgMsg;
// 
// 	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;
// 
// 	nRow				= pItem->iRow;
// 	nCol				= pItem->iColumn;
// 
// 	if (nRow < 1 || nCol == 0) return;
// 	dlgMsg.m_nMessageType	= 1;
// 
// 	dlgMsg.m_strMessage.Format(_T("Do you want a select Test Site[%d] Change?"), nRow);
// 
// 	if (dlgMsg.DoModal() == IDOK)
// 	{
// 		//m_pGridPgmMode.SetItemBkColour(nRow, 1, GREEN_D, BLACK_C);
// 		//m_pGridPgmMode.SetItemBkColour(nRow, 2, GREEN_D, BLACK_C);
// 		if (nCol == 1)
// 		{
// 			if (m_nFtpUse[nRow -1][1] == FALSE)
// 			{
// 				strTmp.Format(_T("LOCAL"));
// 				m_nFtpUse[nRow -1][1] = TRUE;
// 				m_pGridPgmMode.SetItemBkColour(nRow, nCol, GREEN_D, BLACK_C);
// 			}
// 			else
// 			{
// 				strTmp.Format(_T("FTP"));
// 				m_nFtpUse[nRow -1][1] = FALSE;
// 				m_pGridPgmMode.SetItemBkColour(nRow, nCol, GREEN_C, BLACK_C);
// 			}
// 
// 			m_pGridPgmMode.SetItemText(nRow,nCol,strTmp);
// 		}
// 	}
	m_pGridPgmMode.Invalidate(FALSE);
}
void CScreenBasic::OnInitGridDeviceMode()
{
	CString strTmp;
	int i, j;
	
	m_pGridDevice.SetFrameFocusCell(FALSE);
	m_pGridDevice.SetTrackFocusCell(FALSE);
	m_pGridDevice.EnableSelection(FALSE);

	m_pGridDevice.SetGridLineColor(BLACK_C);
	m_pGridDevice.SetGridLines(1);

	m_pGridDevice.SetRowCount(30);
	m_pGridDevice.SetColumnCount(2);

	m_pGridDevice.SetFixedRowCount(0);
	m_pGridDevice.SetFixedColumnCount(0);
	m_pGridDevice.SetFixedBkColor(RGB(0,0,200));
	m_pGridDevice.SetFixedBkColor(RGB(200,200,255));
	m_pGridDevice.SetTextBkColor(RGB(150,150,200));

	//for (i=0; i<18; i++) 
	for (i=0; i<30; i++) 
	{
		m_pGridDevice.SetRowHeight(i, 24);

		for (j=0; j<2; j++) 
		{
			switch (j)
			{
				case 0:
					m_pGridDevice.SetColumnWidth(j, 60);
					break;

				case 1:
					m_pGridDevice.SetColumnWidth(j, 166);
					break;
			}

			m_pGridDevice.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridDevice.SetItemState(i, j, GVIS_READONLY);
			m_pGridDevice.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
		}
	}

	// IO MAP Header ////////////////////////////////////////////////////////////////////////////////////////////// 
	m_pGridDevice.SetItemBkColour(0, 0, RGB(219, 229, 241), BLACK_L);
	m_pGridDevice.SetItemFont(0, 0, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(0, 0, _T("No."));

	m_pGridDevice.SetItemBkColour(0, 1, RGB(219, 229, 241), BLACK_L);
	m_pGridDevice.SetItemFont(0, 1, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(0, 1, _T("MODE"));


	m_pGridDevice.SetItemBkColour(1, 0, BLACK_L, YELLOW_C);
	m_pGridDevice.SetItemFont(1, 0, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(1, 0, _T("1"));

	m_pGridDevice.SetItemBkColour(1, 1, BLACK_L, YELLOW_C);
	m_pGridDevice.SetItemFont(1, 1, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(1, 1, _T("WithOut"));

	m_pGridDevice.SetItemBkColour(2, 0, BLACK_L, YELLOW_C);
	m_pGridDevice.SetItemFont(2, 0, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(2, 0, _T("2"));

	m_pGridDevice.SetItemBkColour(2, 1, BLACK_L, YELLOW_C);
	m_pGridDevice.SetItemFont(2, 1, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(2, 1, _T("With"));

	if (m_nModeDevice[1]		== WITHOUT_DVC)
	{
		m_pGridDevice.SetItemBkColour(1, 0, RGB(79, 79, 225), WHITE_C);
		m_pGridDevice.SetItemBkColour(1, 1, RGB(79, 79, 225), WHITE_C);
	}
	else if (m_nModeDevice[1]	== WITH_DVC)
	{
		m_pGridDevice.SetItemBkColour(2, 0, RGB(79, 79, 225), WHITE_C);
		m_pGridDevice.SetItemBkColour(2, 1, RGB(79, 79, 225), WHITE_C);
	}

	m_pGridDevice.MergeCells(3, 0, 3, 1);
	m_pGridDevice.SetItemBkColour(3, 0, RGB(219, 229, 241), BLACK_L);
	m_pGridDevice.SetItemFont(3, 0, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(3, 0, _T("INTERFACE"));

	m_pGridDevice.SetItemBkColour(4, 0, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(4, 0, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(4, 0, _T("1"));

	m_pGridDevice.SetItemBkColour(4, 1, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(4, 1, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(4, 1, _T("Off Line"));

	m_pGridDevice.SetItemBkColour(5, 0, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(5, 0, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(5, 0, _T("2"));

	m_pGridDevice.SetItemBkColour(5, 1, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(5, 1, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(5, 1, _T("On Line"));

	if (m_nModeInterface[1] == EQP_OFF_LINE)
	{
		m_pGridDevice.SetItemBkColour(4, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(4, 1, GREEN_C, BLACK_C);
	}
	else
	{
		m_pGridDevice.SetItemBkColour(5, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(5, 1, GREEN_C, BLACK_C);
	}

	m_pGridDevice.MergeCells(6, 0, 6, 1);
	m_pGridDevice.SetItemBkColour(6, 0, RGB(219, 229, 241), BLACK_L);
	m_pGridDevice.SetItemFont(6, 0, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(6, 0, _T("Work Mode"));

	m_pGridDevice.SetItemBkColour(7, 0, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(7, 0, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(7, 0, _T("1"));

	m_pGridDevice.SetItemBkColour(7, 1, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(7, 1, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(7, 1, _T("Stand Alone"));

	m_pGridDevice.SetItemBkColour(8, 0, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(8, 0, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(8, 0, _T("2"));

	m_pGridDevice.SetItemBkColour(8, 1, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(8, 1, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(8, 1, _T("In-Line"));

	if (m_nModeRearSmema[1] == NO)
	{
		m_pGridDevice.SetItemBkColour(7, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(7, 1, GREEN_C, BLACK_C);
	}
	else
	{
		m_pGridDevice.SetItemBkColour(8, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(8, 1, GREEN_C, BLACK_C);
	}
	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	m_pGridDevice.MergeCells(9, 0, 9, 1);
	m_pGridDevice.SetItemBkColour(9, 0, RGB(219, 229, 241), BLACK_L);
	m_pGridDevice.SetItemFont(9, 0, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(9, 0, _T("RFID Read/Write"));

	m_pGridDevice.SetItemBkColour(10, 0, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(10, 0, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(10, 0, _T("1"));

	m_pGridDevice.SetItemBkColour(10, 1, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(10, 1, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(10, 1, _T("RFID No"));

	m_pGridDevice.SetItemBkColour(11, 0, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(11, 0, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(11, 0, _T("2"));

	m_pGridDevice.SetItemBkColour(11, 1, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(11, 1, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(11, 1, _T("RFID Yes"));

	if (m_nModeRfid[1] == NO)
	{
		m_pGridDevice.SetItemBkColour(10, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(10, 1, GREEN_C, BLACK_C);
	}
	else
	{
		m_pGridDevice.SetItemBkColour(11, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(11, 1, GREEN_C, BLACK_C);
	}
	
	/////////////////////////////////////////////////////////////////
	m_pGridDevice.MergeCells(12, 0, 12, 1);
	m_pGridDevice.SetItemBkColour(12, 0, RGB(219, 229, 241), BLACK_L);
	m_pGridDevice.SetItemFont(12, 0, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(12, 0, _T("PGM Mode"));

	m_pGridDevice.SetItemBkColour(13, 0, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(13, 0, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(13, 0, _T("1"));

	m_pGridDevice.SetItemBkColour(13, 1, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(13, 1, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(13, 1, _T("FTP"));

	m_pGridDevice.SetItemBkColour(14, 0, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(14, 0, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(14, 0, _T("2"));

	m_pGridDevice.SetItemBkColour(14, 1, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(14, 1, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(14, 1, _T("LOCAL"));

	if (m_nFtpUse[1] == NO)
	{
		m_pGridDevice.SetItemBkColour(13, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(13, 1, GREEN_C, BLACK_C);
	}
	else
	{
		m_pGridDevice.SetItemBkColour(14, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(14, 1, GREEN_C, BLACK_C);
	}

	/////////////////////////////////////////////////////////////////
	//Barcode 1D
	m_pGridDevice.MergeCells(15, 0, 15, 1);
	m_pGridDevice.SetItemBkColour(15, 0, RGB(219, 229, 241), BLACK_L);
	m_pGridDevice.SetItemFont(15, 0, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(15, 0, _T("BCR 1D Mode"));

	m_pGridDevice.SetItemBkColour(16, 0, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(16, 0, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(16, 0, _T("1"));

	m_pGridDevice.SetItemBkColour(16, 1, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(16, 1, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(16, 1, _T("BCR 1D Yes"));

	m_pGridDevice.SetItemBkColour(17, 0, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(17, 0, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(17, 0, _T("2"));

	m_pGridDevice.SetItemBkColour(17, 1, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(17, 1, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(17, 1, _T("BCR 1D No"));

	if (m_n1DBarcodeUse[1] == YES)
	{
		m_pGridDevice.SetItemBkColour(16, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(16, 1, GREEN_C, BLACK_C);
	}
	else
	{
		m_pGridDevice.SetItemBkColour(17, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(17, 1, GREEN_C, BLACK_C);
	}
	/////////////////////////////////////////////////////////////////
	//Barcode 2D 
	m_pGridDevice.MergeCells(18, 0, 18, 1);
	m_pGridDevice.SetItemBkColour(18, 0, RGB(219, 229, 241), BLACK_L);
	m_pGridDevice.SetItemFont(18, 0, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(18, 0, _T("BCR 2D Mode"));

	m_pGridDevice.SetItemBkColour(19, 0, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(19, 0, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(19, 0, _T("1"));

	m_pGridDevice.SetItemBkColour(19, 1, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(19, 1, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(19, 1, _T("BCR 2D Yes"));

	m_pGridDevice.SetItemBkColour(20, 0, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(20, 0, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(20, 0, _T("2"));

	m_pGridDevice.SetItemBkColour(20, 1, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(20, 1, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(20, 1, _T("BCR 2D No"));

	if (m_n2DBarcodeUse[1] == YES)
	{
		m_pGridDevice.SetItemBkColour(19, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(19, 1, GREEN_C, BLACK_C);
	}
	else
	{
		m_pGridDevice.SetItemBkColour(20, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(20, 1, GREEN_C, BLACK_C);
	}
	/////////////////////////////////////////////////////////////////
	//Ld/Uld Light Cutain 
	m_pGridDevice.MergeCells(21, 0, 21, 1);
	m_pGridDevice.SetItemBkColour(21, 0, RGB(219, 229, 241), BLACK_L);
	m_pGridDevice.SetItemFont(21, 0, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(21, 0, _T("Ld/Uld Light Cutain Mode"));

	m_pGridDevice.SetItemBkColour(22, 0, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(22, 0, &clsFunc.OnLogFont(17));
	m_pGridDevice.SetItemText(22, 0, _T("1"));

	m_pGridDevice.SetItemBkColour(22, 1, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(22, 1, &clsFunc.OnLogFont(17));
	m_pGridDevice.SetItemText(22, 1, _T("Ld/Uld Light Cutain Yes"));

	m_pGridDevice.SetItemBkColour(23, 0, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(23, 0, &clsFunc.OnLogFont(17));
	m_pGridDevice.SetItemText(23, 0, _T("2"));

	m_pGridDevice.SetItemBkColour(23, 1, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(23, 1, &clsFunc.OnLogFont(17));
	m_pGridDevice.SetItemText(23, 1, _T("Ld/Uld Light Cutain No"));

	if (m_nLdUldLightCutainSkip[1] == NO)
	{
		m_pGridDevice.SetItemBkColour(22, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(22, 1, GREEN_C, BLACK_C);
	}
	else
	{
		m_pGridDevice.SetItemBkColour(23, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(23, 1, GREEN_C, BLACK_C);
	}
	/////////////////////////////////////////////////////////////////
	//Rej Light Cutain 
	m_pGridDevice.MergeCells(24, 0, 24, 1);
	m_pGridDevice.SetItemBkColour(24, 0, RGB(219, 229, 241), BLACK_L);
	m_pGridDevice.SetItemFont(24, 0, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(24, 0, _T("Rej Light Cutain Mode"));

	m_pGridDevice.SetItemBkColour(25, 0, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(25, 0, &clsFunc.OnLogFont(17));
	m_pGridDevice.SetItemText(25, 0, _T("1"));

	m_pGridDevice.SetItemBkColour(25, 1, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(25, 1, &clsFunc.OnLogFont(17));
	m_pGridDevice.SetItemText(25, 1, _T("Rej Light Cutain Yes"));

	m_pGridDevice.SetItemBkColour(26, 0, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(26, 0, &clsFunc.OnLogFont(17));
	m_pGridDevice.SetItemText(26, 0, _T("2"));

	m_pGridDevice.SetItemBkColour(26, 1, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(26, 1, &clsFunc.OnLogFont(17));
	m_pGridDevice.SetItemText(26, 1, _T("Rej Light Cutain No"));

	if (m_nRejLightCutainSkip[1] == NO)
	{
		m_pGridDevice.SetItemBkColour(25, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(25, 1, GREEN_C, BLACK_C);
	}
	else
	{
		m_pGridDevice.SetItemBkColour(26, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(26, 1, GREEN_C, BLACK_C);
	}
	

	//Rej Light Cutain 
	m_pGridDevice.MergeCells(27, 0, 27, 1);
	m_pGridDevice.SetItemBkColour(27, 0, RGB(219, 229, 241), BLACK_L);
	m_pGridDevice.SetItemFont(27, 0, &clsFunc.OnLogFont(18));
	m_pGridDevice.SetItemText(27, 0, _T("Tester Interface"));

	m_pGridDevice.SetItemBkColour(28, 0, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(28, 0, &clsFunc.OnLogFont(17));
	m_pGridDevice.SetItemText(28, 0, _T("1"));

	m_pGridDevice.SetItemBkColour(28, 1, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(28, 1, &clsFunc.OnLogFont(17));
	m_pGridDevice.SetItemText(28, 1, _T("Off Line"));

	m_pGridDevice.SetItemBkColour(29, 0, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(29, 0, &clsFunc.OnLogFont(17));
	m_pGridDevice.SetItemText(39, 0, _T("2"));

	m_pGridDevice.SetItemBkColour(29, 1, GREEN_D, BLACK_C);
	m_pGridDevice.SetItemFont(29, 1, &clsFunc.OnLogFont(17));
	m_pGridDevice.SetItemText(29, 1, _T("On Line"));

	if (m_nModeTestInterface[1] == EQP_OFF_LINE)
	{
		m_pGridDevice.SetItemBkColour(28, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(28, 1, GREEN_C, BLACK_C);
	}
	else
	{
		m_pGridDevice.SetItemBkColour(29, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(29, 1, GREEN_C, BLACK_C);
	}
	m_pGridDevice.Invalidate(FALSE);
}


void CScreenBasic::OnDeviceCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CDialog_Pass_Check  pass_chk_dlg;

	int nRow, nCol;

	CString strTmp;
	CString strMsg;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	if(nRow < 1) return;

	if (nRow == 1 && m_nModeDevice[1] != WITHOUT_DVC)
	{
		if (st_lot_info[LOT_CURR].nLotStatus >= LOT_START || st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)
		{
			if (st_handler_info.cWndList != NULL)
			{
				clsMem.OnNormalMessageWrite(_T("Data Change Fail. Lot Start Status...."));
				st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
			}

			return;
		}

		m_nModeDevice[1] = WITHOUT_DVC; 

		m_pGridDevice.SetItemBkColour(1, 0, RGB(79, 79, 225), WHITE_C);
		m_pGridDevice.SetItemBkColour(1, 1, RGB(79, 79, 225), WHITE_C);

		m_pGridDevice.SetItemBkColour(2, 0, BLACK_L, YELLOW_C);
		m_pGridDevice.SetItemBkColour(2, 1, BLACK_L, YELLOW_C);
	}
	else if (nRow == 2 && m_nModeDevice[1] != WITH_DVC)
	{
		if (st_lot_info[LOT_CURR].nLotStatus >= LOT_START || st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)
		{
			if (st_handler_info.cWndList != NULL)
			{
				clsMem.OnNormalMessageWrite(_T("Data Change Fail. Lot Start Status...."));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
			}

			return;
		}

		m_nModeDevice[1] = WITH_DVC;

		m_pGridDevice.SetItemBkColour(1, 0, BLACK_L, YELLOW_C);
		m_pGridDevice.SetItemBkColour(1, 1, BLACK_L, YELLOW_C);

		m_pGridDevice.SetItemBkColour(2, 0, RGB(79, 79, 225), WHITE_C);
		m_pGridDevice.SetItemBkColour(2, 1, RGB(79, 79, 225), WHITE_C);
	}
	else if (nRow == 4 && m_nModeInterface[1] != EQP_OFF_LINE)
	{
		if (st_lot_info[LOT_CURR].nLotStatus >= LOT_START || st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)
		{
			if (st_handler_info.cWndList != NULL)
			{
				clsMem.OnNormalMessageWrite(_T("Data Change Fail. Lot Start Status...."));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
			}

			return;
		}

		m_nModeInterface[1] = EQP_OFF_LINE;

		m_pGridDevice.SetItemBkColour(4, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(4, 1, GREEN_C, BLACK_C);

		m_pGridDevice.SetItemBkColour(5, 0, GREEN_D, BLACK_C);
		m_pGridDevice.SetItemBkColour(5, 1, GREEN_D, BLACK_C);
	}
	else if (nRow == 5 && m_nModeInterface[1] != EQP_ON_LINE)
	{
		if (st_lot_info[LOT_CURR].nLotStatus >= LOT_START || st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)
		{
			if (st_handler_info.cWndList != NULL)
			{
				clsMem.OnNormalMessageWrite(_T("Data Change Fail. Lot Start Status...."));
				st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
			}

			return;
		}

		m_nModeInterface[1] = EQP_ON_LINE;

		m_pGridDevice.SetItemBkColour(4, 0, GREEN_D, BLACK_C);
		m_pGridDevice.SetItemBkColour(4, 1, GREEN_D, BLACK_C);

		m_pGridDevice.SetItemBkColour(5, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(5, 1, GREEN_C, BLACK_C);
	}
	else if (nRow == 7 && m_nModeRearSmema[1] != NO)
	{
/*
		if (st_lot_info[LOT_CURR].nLotStatus >= LOT_START || st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)
		{
			if (st_handler_info.cWndList != NULL)
			{
				clsMem.OnNormalMessageWrite(_T("Data Change Fail. Lot Start Status...."));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
			}

			return;
		}
*/
		m_nModeRearSmema[1] = NO;

		m_pGridDevice.SetItemBkColour(7, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(7, 1, GREEN_C, BLACK_C);

		m_pGridDevice.SetItemBkColour(8, 0, GREEN_D, BLACK_C);
		m_pGridDevice.SetItemBkColour(8, 1, GREEN_D, BLACK_C);
	}
	else if (nRow == 8 && m_nModeRearSmema[1] != YES)
	{
/*		
		if (st_lot_info[LOT_CURR].nLotStatus >= LOT_START || st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)
		{
			if (st_handler_info.cWndList != NULL)
			{
				clsMem.OnNormalMessageWrite(_T("Data Change Fail. Lot Start Status...."));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
			}

			return;
		}
*/
		m_nModeRearSmema[1] = YES;

		m_pGridDevice.SetItemBkColour(7, 0, GREEN_D, BLACK_C);
		m_pGridDevice.SetItemBkColour(7, 1, GREEN_D, BLACK_C);

		m_pGridDevice.SetItemBkColour(8, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(8, 1, GREEN_C, BLACK_C);
	}
	else if (nRow == 10 && m_nModeRfid[1] != NO)
	{
		if (st_lot_info[LOT_CURR].nLotStatus >= LOT_START || st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)
		{
			if (st_handler_info.cWndList != NULL)
			{
				clsMem.OnNormalMessageWrite(_T("Data Change Fail. Lot Start Status...."));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
			}

			return;
		}

		m_nModeRfid[1] = NO;

		m_pGridDevice.SetItemBkColour(10, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(10, 1, GREEN_C, BLACK_C);

		m_pGridDevice.SetItemBkColour(11, 0, GREEN_D, BLACK_C);
		m_pGridDevice.SetItemBkColour(11, 1, GREEN_D, BLACK_C);

	}
	else if (nRow == 11 && m_nModeRfid[1] != YES)
	{
		if (st_lot_info[LOT_CURR].nLotStatus >= LOT_START || st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)
		{
			if (st_handler_info.cWndList != NULL)
			{
				clsMem.OnNormalMessageWrite(_T("Data Change Fail. Lot Start Status...."));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
			}

			return;
		}

		m_nModeRfid[1] = YES;

		m_pGridDevice.SetItemBkColour(10, 0, GREEN_D, BLACK_C);
		m_pGridDevice.SetItemBkColour(10, 1, GREEN_D, BLACK_C);

		m_pGridDevice.SetItemBkColour(11, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(11, 1, GREEN_C, BLACK_C);
	}
	//kwlee 2016.0816
	else if (nRow == 13 && m_nFtpUse[1] != NO)
	{
		if (st_lot_info[LOT_CURR].nLotStatus >= LOT_START || st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)
		{
			if (st_handler_info.cWndList != NULL)
			{
				clsMem.OnNormalMessageWrite(_T("Data Change Fail. Lot Start Status...."));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
			}

			return;
		}

		m_nFtpUse[1] = NO;

		m_pGridDevice.SetItemBkColour(13, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(13, 1, GREEN_C, BLACK_C);

		m_pGridDevice.SetItemBkColour(14, 0, GREEN_D, BLACK_C);
		m_pGridDevice.SetItemBkColour(14, 1, GREEN_D, BLACK_C);

	}
	else if (nRow == 14 && m_nFtpUse[1] != YES)
	{
		if (st_lot_info[LOT_CURR].nLotStatus >= LOT_START || st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)
		{
			if (st_handler_info.cWndList != NULL)
			{
				clsMem.OnNormalMessageWrite(_T("Data Change Fail. Lot Start Status...."));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
			}

			return;
		}

		m_nFtpUse[1] = YES;

		m_pGridDevice.SetItemBkColour(13, 0, GREEN_D, BLACK_C);
		m_pGridDevice.SetItemBkColour(13, 1, GREEN_D, BLACK_C);

		m_pGridDevice.SetItemBkColour(14, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(14, 1, GREEN_C, BLACK_C);
	}
	//kwlee 2016.0825
	else if (nRow == 16 && m_n1DBarcodeUse[1] != YES)
	{
		if (st_lot_info[LOT_CURR].nLotStatus >= LOT_START || st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)
		{
			if (st_handler_info.cWndList != NULL)
			{
				clsMem.OnNormalMessageWrite(_T("Data Change Fail. Lot Start Status...."));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
			}

			return;
		}

		m_n1DBarcodeUse[1] = YES;

		m_pGridDevice.SetItemBkColour(16, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(16, 1, GREEN_C, BLACK_C);

		m_pGridDevice.SetItemBkColour(17, 0, GREEN_D, BLACK_C);
		m_pGridDevice.SetItemBkColour(17, 1, GREEN_D, BLACK_C);

	}
	else if (nRow == 17 && m_n1DBarcodeUse[1] != NO)
	{
		if (st_lot_info[LOT_CURR].nLotStatus >= LOT_START || st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)
		{
			if (st_handler_info.cWndList != NULL)
			{
				clsMem.OnNormalMessageWrite(_T("Data Change Fail. Lot Start Status...."));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
			}

			return;
		}
		m_n1DBarcodeUse[1] = NO;

		m_pGridDevice.SetItemBkColour(16, 0, GREEN_D, BLACK_C);
		m_pGridDevice.SetItemBkColour(16, 1, GREEN_D, BLACK_C);

		m_pGridDevice.SetItemBkColour(17, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(17, 1, GREEN_C, BLACK_C);
	}
	/////
	//Sam
	//kwlee 2016.0825
	else if (nRow == 19 && m_n2DBarcodeUse[1] != YES)
	{
		if (st_lot_info[LOT_CURR].nLotStatus >= LOT_START || st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)
		{
			if (st_handler_info.cWndList != NULL)
			{
				clsMem.OnNormalMessageWrite(_T("Data Change Fail. Lot Start Status...."));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
			}

			return;
		}

		m_n2DBarcodeUse[1] = YES;

		m_pGridDevice.SetItemBkColour(19, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(19, 1, GREEN_C, BLACK_C);

		m_pGridDevice.SetItemBkColour(20, 0, GREEN_D, BLACK_C);
		m_pGridDevice.SetItemBkColour(20, 1, GREEN_D, BLACK_C);

	}
	else if (nRow == 20 && m_n2DBarcodeUse[1] != NO)
	{
		if (st_lot_info[LOT_CURR].nLotStatus >= LOT_START || st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)
		{
			if (st_handler_info.cWndList != NULL)
			{
				clsMem.OnNormalMessageWrite(_T("Data Change Fail. Lot Start Status...."));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
			}

			return;
		}
		m_n2DBarcodeUse[1] = NO;

		m_pGridDevice.SetItemBkColour(19, 0, GREEN_D, BLACK_C);
		m_pGridDevice.SetItemBkColour(19, 1, GREEN_D, BLACK_C);

		m_pGridDevice.SetItemBkColour(20, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(20, 1, GREEN_C, BLACK_C);
	}
	////
	/////////
	//Ld/Uld Light Cutain
	else if (nRow == 22 && m_nLdUldLightCutainSkip[1] != NO)
	{
		if (st_lot_info[LOT_CURR].nLotStatus >= LOT_START || st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)
		{
			if (st_handler_info.cWndList != NULL)
			{
				clsMem.OnNormalMessageWrite(_T("Data Change Fail. Lot Start Status...."));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
			}

			return;
		}

		m_nLdUldLightCutainSkip[1] = NO;

		m_pGridDevice.SetItemBkColour(22, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(22, 1, GREEN_C, BLACK_C);

		m_pGridDevice.SetItemBkColour(23, 0, GREEN_D, BLACK_C);
		m_pGridDevice.SetItemBkColour(23, 1, GREEN_D, BLACK_C);

	}
	else if (nRow == 23 && m_nLdUldLightCutainSkip[1] != YES)
	{
		if (st_lot_info[LOT_CURR].nLotStatus >= LOT_START || st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)
		{
			if (st_handler_info.cWndList != NULL)
			{
				clsMem.OnNormalMessageWrite(_T("Data Change Fail. Lot Start Status...."));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
			}

			return;
		}
		m_nLdUldLightCutainSkip[1] = YES;

		m_pGridDevice.SetItemBkColour(22, 0, GREEN_D, BLACK_C);
		m_pGridDevice.SetItemBkColour(22, 1, GREEN_D, BLACK_C);

		m_pGridDevice.SetItemBkColour(23, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(23, 1, GREEN_C, BLACK_C);
	}
	////
	//Rej Light Cutain
	else if (nRow == 25 && m_nRejLightCutainSkip[1] != NO)
	{
		if (st_lot_info[LOT_CURR].nLotStatus >= LOT_START || st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)
		{
			if (st_handler_info.cWndList != NULL)
			{
				clsMem.OnNormalMessageWrite(_T("Data Change Fail. Lot Start Status...."));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
			}
			return;
		}

		m_nRejLightCutainSkip[1] = NO;

		m_pGridDevice.SetItemBkColour(25, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(25, 1, GREEN_C, BLACK_C);

		m_pGridDevice.SetItemBkColour(26, 0, GREEN_D, BLACK_C);
		m_pGridDevice.SetItemBkColour(26, 1, GREEN_D, BLACK_C);

	}
	else if (nRow == 26 && m_nRejLightCutainSkip[1] != YES)
	{
		if (st_lot_info[LOT_CURR].nLotStatus >= LOT_START || st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)
		{
			if (st_handler_info.cWndList != NULL)
			{
				clsMem.OnNormalMessageWrite(_T("Data Change Fail. Lot Start Status...."));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
			}

			return;
		}
		m_nRejLightCutainSkip[1] = YES;

		m_pGridDevice.SetItemBkColour(25, 0, GREEN_D, BLACK_C);
		m_pGridDevice.SetItemBkColour(25, 1, GREEN_D, BLACK_C);

		m_pGridDevice.SetItemBkColour(26, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(26, 1, GREEN_C, BLACK_C);
	}
	else if (nRow == 28 && m_nModeTestInterface[1] != EQP_OFF_LINE)
	{
		m_nModeTestInterface[1] = EQP_OFF_LINE;

		m_pGridDevice.SetItemBkColour(28, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(28, 1, GREEN_C, BLACK_C);

		m_pGridDevice.SetItemBkColour(29, 0, GREEN_D, BLACK_C);
		m_pGridDevice.SetItemBkColour(29, 1, GREEN_D, BLACK_C);

	}
	else if (nRow == 29 && m_nModeTestInterface[1] != EQP_ON_LINE)
	{
		m_nModeTestInterface[1] = EQP_ON_LINE;

		m_pGridDevice.SetItemBkColour(28, 0, GREEN_D, BLACK_C);
		m_pGridDevice.SetItemBkColour(28, 1, GREEN_D, BLACK_C);

		m_pGridDevice.SetItemBkColour(29, 0, GREEN_C, BLACK_C);
		m_pGridDevice.SetItemBkColour(29, 1, GREEN_C, BLACK_C);
	}
	////
	m_pGridDevice.Invalidate();
}

void CScreenBasic::OnInitGridModelList()
{
	CString strTmp;
	
	m_pGridModel.SetFrameFocusCell(FALSE);
	m_pGridModel.SetTrackFocusCell(FALSE);
	m_pGridModel.EnableSelection(FALSE);

	m_pGridModel.SetGridLineColor(BLACK_C);
	m_pGridModel.SetGridLines(1);

	m_pGridModel.SetRowCount(1);
	m_pGridModel.SetColumnCount(1);

	m_pGridModel.SetFixedRowCount(0);
	m_pGridModel.SetFixedColumnCount(0);
	m_pGridModel.SetFixedBkColor(RGB(0,0,200));
	m_pGridModel.SetFixedBkColor(RGB(200,200,255));
	m_pGridModel.SetTextBkColor(RGB(150,150,200));

	m_pGridModel.SetRowHeight(0, 50);
	m_pGridModel.SetColumnWidth(0, 300);

	m_pGridModel.SetItemFormat(0, 0, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	m_pGridModel.SetItemState(0, 0, GVIS_READONLY);
	m_pGridModel.SetItemBkColour(0, 0, WHITE_C, CLR_DEFAULT);

	m_pGridModel.SetItemBkColour(0, 0, RGB(219, 229, 241), BLACK_L);
	m_pGridModel.SetItemFont(0, 0, &clsFunc.OnLogFont(18));
	m_pGridModel.SetItemText(0, 0, _T("Model Name"));

	OnInitModelList(st_path_info.strBasic);
}

void CScreenBasic::OnModelCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CDialog_Message dlgMsg;
	int nRow, nCol;
	INT_PTR nResponse;

	CString strTmp;
	CString strMsg;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	if (nRow < 1) return;

	if (st_lot_info[LOT_CURR].nLotStatus >= LOT_START || st_lot_info[LOT_NEXT].nLotStatus >= LOT_START)
	{
		if (st_handler_info.cWndList != NULL)
		{
			clsMem.OnNormalMessageWrite(_T("Data Change Fail. Lot Start Status...."));
			//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
		}

		return;
	}

	strTmp = m_pGridModel.GetItemText(nRow, nCol);

	dlgMsg.m_nMessageType	= 1;
	dlgMsg.m_strMessage.Format(_T("[%s] Model\r\nDo you want a clicked model?"), strTmp);

	nResponse = dlgMsg.DoModal();
		
	if(nResponse == IDOK)
	{
		if(OnModelDeviceCheck(strTmp) == false)  // 입력된 디바이스 이름 정상 유무 검사 함수
		{
			return ;
		}

		m_strDeviceName[1]			= strTmp;
		st_basic_info.strDeviceName	= m_strDeviceName[1];
		st_handler_info.nModelChangeCheck = TRUE; //kwlee 2016.1221 자재 변경시 Tray 확인 메시지 출력
		clsBasic.OnBasic_Data_Load(1);
		clsBasic.OnModuleRobot_Teach_Data_Load(st_basic_info.strDeviceName);
		clsBasic.OnMaintenance_Data_Load();
		clsBasic.OnWaitTime_Data_Load();
		clsBasic.OnMotorSpeed_Set_Data_Load();
		clsBasic.OnInterface_Data_Load();
		clsBasic.OnRecipe_Data_Load(st_basic_info.strDeviceName);
			
		OnDataInit();
		OnInitGridDeviceMode();

		OnDeviceColorChange(nRow, nCol);

		// jtkim 20150929
		::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + FTP_NETWORK, 1, 0);

		m_pGridModel.Refresh();// .Invalidate();
	}
	else
	{
		m_pGridModel.SetItemBkColour(nRow, nCol, RGB(79, 79, 225), WHITE_C);
	}
}

void CScreenBasic::OnModelCellRClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CDialog_Message dlgMsg;
	CDialog_Pass_Check	dlgPass;

	int nRow, nCol;
	INT_PTR nResponse;

	CString strTmp;
	CString strMsg;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	if (nRow < 1) return;

	st_other_info.strPassLevel = _T("AMT_MASTER");

	dlgPass.m_nPassLevel = 6;
	nResponse = (int)dlgPass.DoModal();

	if (nResponse == IDOK)
	{
		strTmp = m_pGridModel.GetItemText(nRow, nCol);

		dlgMsg.m_nMessageType	= 1;
		dlgMsg.m_strMessage.Format(_T("[%s] Model\r\nDo you want a clicked model?"), strTmp);

		nResponse = dlgMsg.DoModal();

		if(nResponse == IDOK)
		{
			if(OnModelDeviceCheck(strTmp) == false)  // 입력된 디바이스 이름 정상 유무 검사 함수
			{
				return ;
			}

			m_strDeviceName[1]			= strTmp;
			st_basic_info.strDeviceName	= m_strDeviceName[1];

			clsBasic.OnBasic_Data_Load(1);
			clsBasic.OnModuleRobot_Teach_Data_Load(st_basic_info.strDeviceName);
			clsBasic.OnMaintenance_Data_Load();
			clsBasic.OnWaitTime_Data_Load();
			clsBasic.OnMotorSpeed_Set_Data_Load();
			clsBasic.OnInterface_Data_Load();
			clsBasic.OnRecipe_Data_Load(st_basic_info.strDeviceName);

			OnDataInit();
			OnInitGridDeviceMode();

			OnDeviceColorChange(nRow, nCol);

			m_pGridModel.Refresh();// .Invalidate();
		}
		else
		{
			m_pGridModel.SetItemBkColour(nRow, nCol, RGB(79, 79, 225), WHITE_C);
		}
	}
	else 
	{
		return;
	}
}



void CScreenBasic::OnDeviceColorChange(int nSelectRow, int nSelectCol)
{
	int nTotRowCnt;
	nTotRowCnt = m_pGridModel.GetRowCount();

	m_nDeviceName = nSelectRow;

	m_pGridModel.SetItemBkColour(m_nDeviceName, nSelectCol, RGB(79, 79, 225), WHITE_C);

	for(int i=1; i< nTotRowCnt; i++)
	{
		if(i != m_nDeviceName) 
		{
			m_pGridModel.SetItemBkColour(i, nSelectCol, BLACK_L, WHITE_C);
		}
	}

	m_nOldDeviceName = m_nDeviceName;

}

void CScreenBasic::OnDataHistoryLog()
{
	CString strMsg;

	if (m_nModeDevice[0]		!= m_nModeDevice[1])
	{
		strMsg.Format(_T("[BASIC] Device Mode Change %d -> %d"), m_nModeDevice[0], m_nModeDevice[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	if (m_nModeInterface[0]		!= m_nModeInterface[1])
	{
		strMsg.Format(_T("[BASIC] Interface Mode Change %d -> %d"), m_nModeInterface[0], m_nModeInterface[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	if (m_nModeSecsGem[0]		!= m_nModeSecsGem[1])
	{
		strMsg.Format(_T("[BASIC] SecsGem Mode Change %d -> %d"), m_nModeSecsGem[0], m_nModeSecsGem[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	if (m_nModeWork[0]		!= m_nModeWork[1])
	{
		strMsg.Format(_T("[BASIC] Work Mode Change %d -> %d"), m_nModeWork[0], m_nModeWork[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	if (m_strDeviceName[0]		!= m_strDeviceName[1])
	{
		strMsg.Format(_T("[BASIC] Device Name Change %s -> %s"), m_strDeviceName[0], m_strDeviceName[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	if (m_nModeFrontSmema[0]			!= m_nModeFrontSmema[1])
	{
		strMsg.Format(_T("[BASIC] Front Smema Check Mode Change %d -> %d"), m_nModeFrontSmema[0], m_nModeFrontSmema[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	if (m_nModeRearSmema[0]			!= m_nModeRearSmema[1])
	{
		strMsg.Format(_T("[BASIC] Rear Smema Check Mode Change %d -> %d"), m_nModeRearSmema[0], m_nModeRearSmema[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}
	
	if (m_nModeRfid[0]			!= m_nModeRfid[1])
	{
		strMsg.Format(_T("[BASIC] RFID Mode Change %d -> %d"), m_nModeRfid[0], m_nModeRfid[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	if (m_nFtpUse[0]			!= m_nFtpUse[1])
	{
		strMsg.Format(_T("[BASIC] PGM Mode Change %d -> %d"), m_nFtpUse[0], m_nFtpUse[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}
	//kwlee 2016.0825
	if (m_n1DBarcodeUse[0]			!= m_n1DBarcodeUse[1])
	{
		strMsg.Format(_T("[BASIC] Barcode Mode Change %d -> %d"), m_n1DBarcodeUse[0], m_n1DBarcodeUse[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}
	
	if (m_n2DBarcodeUse[0]			!= m_n2DBarcodeUse[1])
	{
		strMsg.Format(_T("[BASIC] Barcode Mode Change %d -> %d"), m_n2DBarcodeUse[0], m_n2DBarcodeUse[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	//kwlee 2016.0825
	if (m_nLdUldLightCutainSkip[0]			!= m_nLdUldLightCutainSkip[1])
	{
		strMsg.Format(_T("[BASIC] Ld/Uld Light Cutain Mode Change %d -> %d"), m_nLdUldLightCutainSkip[0], m_nLdUldLightCutainSkip[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	if (m_nRejLightCutainSkip[0]			!= m_nRejLightCutainSkip[1])
	{
		strMsg.Format(_T("[BASIC] Rej Light Cutain Mode Change %d -> %d"), m_nRejLightCutainSkip[0], m_nRejLightCutainSkip[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}
	
	////////////////////////////////

	if (m_strModelName[0]		!= m_strModelName[1])
	{
		strMsg.Format(_T("[BASIC] Model Name Change %s -> %s"), m_strModelName[0], m_strModelName[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}
	//kwlee 2016.0809
	if (m_strHifixData[0]!= m_strHifixData[1])
	{
		strMsg.Format(_T("Hifix Name Change %s -> %s"), m_strHifixData[0], m_strHifixData[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	if (m_nBcrBank[0]			!= m_nBcrBank[1])
	{
		strMsg.Format(_T("Barcode Bank Change %d -> %d"), m_nBcrBank[0], m_nBcrBank[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	if (m_nAbortTime[0]			!= m_nAbortTime[1])
	{
		strMsg.Format(_T("Abort Time Change %d -> %d"), m_nAbortTime[0], m_nAbortTime[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	if (m_nBarcodeCnt[0]			!= m_nBarcodeCnt[1])
	{
		strMsg.Format(_T("Barcode Count Change %d -> %d"), m_nBarcodeCnt[0], m_nBarcodeCnt[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	if (m_nModeTestInterface[0] != m_nModeTestInterface[1])
	{
		strMsg.Format(_T("Mode Tester Interface Count Change %d -> %d"), m_nModeTestInterface[0], m_nModeTestInterface[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}


	//kwlee 2016.0930
	if (m_nStackerTrayCnt[0]			!= m_nStackerTrayCnt[1])
	{
		strMsg.Format(_T("Stacker Tray Count Change %d -> %d"), m_nStackerTrayCnt[0], m_nStackerTrayCnt[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	//kwlee 2017.0609
	if (m_nRejectStackerTrayCnt[0]			!= m_nRejectStackerTrayCnt[1])
	{
		strMsg.Format(_T("Reject Stacker Tray Count Change %d -> %d"), m_nRejectStackerTrayCnt[0], m_nRejectStackerTrayCnt[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	//kwlee 2016.1124
	if (m_nAlarmDelayCnt[0]			!= m_nAlarmDelayCnt[1])
	{
		strMsg.Format(_T("Alarm Delay Cnt Change %d -> %d"), m_nAlarmDelayCnt[0], m_nAlarmDelayCnt[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	if (m_nDirectionCheck[0] != m_nDirectionCheck[1])
	{
		strMsg.Format(_T("DirectionCheck Change %d -> %d"), m_nDirectionCheck[0], m_nDirectionCheck[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}
	//kwlee 2016.1201
	if (m_nAutoLoadMode[0] != m_nAutoLoadMode[1])
	{
		strMsg.Format(_T("Auto Load Mode Change %d -> %d"), m_nAutoLoadMode[0], m_nAutoLoadMode[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}
}

void CScreenBasic::OnDataRecovery()
{
	m_nModeDevice[1]			= m_nModeDevice[0];			// [Title Bar 상태 표시] < WHIT/WHIT OUT ㅡ MODE 표시
	m_nModeInterface[1]			= m_nModeInterface[0];
	m_nModeSecsGem[1]			= m_nModeSecsGem[0];

	m_strDeviceName[1]			= m_strDeviceName[0];		// [Title Bar 상태 표시] < 장비에서 설정한 DEVICE 표시	>

	m_nModeFrontSmema[1]		= m_nModeFrontSmema[0];
	m_nModeRearSmema[1]			= m_nModeRearSmema[0];
	m_nModeRfid[1]				= m_nModeRfid[0];
	m_nBcrBank[1]				= m_nBcrBank[0];
	m_strModelName[1]			= m_strModelName[0];
	m_nFtpUse[1]				= m_nFtpUse[0]; 
	
	m_n1DBarcodeUse[1]			= m_n1DBarcodeUse[0]; 
	m_n2DBarcodeUse[1]			= m_n2DBarcodeUse[0];
	
	m_nLdUldLightCutainSkip[1] = m_nLdUldLightCutainSkip[0]; 
	m_nRejLightCutainSkip[1]  = m_nRejLightCutainSkip[0];

	m_strHifixData[1]	= m_strHifixData[0];

	m_nAbortTime[1]				= m_nAbortTime[0];
	m_nBarcodeCnt[1]			= m_nBarcodeCnt[0];
	m_nStackerTrayCnt[1]		= m_nStackerTrayCnt[0]; //kwlee 2016.0930
	m_nRejectStackerTrayCnt[1]	= m_nRejectStackerTrayCnt[0]; //kwlee 2017.0609
	m_nModeTestInterface[1]		= m_nModeTestInterface[0];
	m_nAlarmDelayCnt[1]			= m_nAlarmDelayCnt[0]; //kwlee 2016.1121
	m_nDirectionCheck[1]		= m_nDirectionCheck[0]; //kwlee 2016.1121
	m_nAutoLoadMode[1]			= m_nAutoLoadMode[0]; //kwlee 2016.1201
}


int	CScreenBasic::OnDataComparison()
{
	if (m_nModeDevice[0]			!= m_nModeDevice[1])						return RET_ERROR;			// [Title Bar 상태 표시] < WHIT/WHIT OUT ㅡ MODE 표시
	if (m_nModeInterface[0]			!= m_nModeInterface[1])						return RET_ERROR;
	if (m_nModeSecsGem[0]			!= m_nModeSecsGem[1])						return RET_ERROR;
	if (m_nModeWork[0]				!= m_nModeWork[1])							return RET_ERROR;

	if (m_strDeviceName[0]			!= m_strDeviceName[1])						return RET_ERROR;		// [Title Bar 상태 표시] < 장비에서 설정한 DEVICE 표시	>

	if (m_nModeFrontSmema[0]		!= m_nModeFrontSmema[1])					return RET_ERROR;
	if (m_nModeRearSmema[0]			!= m_nModeRearSmema[1])						return RET_ERROR;
	if (m_nModeRfid[0]				!= m_nModeRfid[1])							return RET_ERROR;
	if (m_strModelName[0]			!= m_strModelName[1])						return RET_ERROR;
	if (m_nBcrBank[0]				!= m_nBcrBank[1])							return RET_ERROR;
	if (m_nFtpUse[0]				!= m_nFtpUse[1])							return RET_ERROR;
	//if (m_nBarcodeUse[0]			!= m_nBarcodeUse[1])						return RET_ERROR;
	if (m_n1DBarcodeUse[0]			!= m_n1DBarcodeUse[1])						return RET_ERROR;
	if (m_n2DBarcodeUse[0]			!= m_n2DBarcodeUse[1])						return RET_ERROR;
	if (m_nLdUldLightCutainSkip[0]	!= m_nLdUldLightCutainSkip[1])				return RET_ERROR;
	if (m_nRejLightCutainSkip[0]	!= m_nRejLightCutainSkip[1])				return RET_ERROR;
	
	if (m_nAbortTime[0]				!= m_nAbortTime[1])							return RET_ERROR;
	if (m_nBarcodeCnt[0]			!= m_nBarcodeCnt[1])						return RET_ERROR;
	if (m_nStackerTrayCnt[0]		!= m_nStackerTrayCnt[1])					return RET_ERROR; //kwlee 2016.0930
	if (m_nRejectStackerTrayCnt[0]	!= m_nRejectStackerTrayCnt[1])				return RET_ERROR; //kwlee 2017.0609
	if (m_nModeTestInterface[0]		!= m_nModeTestInterface[1])					return RET_ERROR;
	if (m_nAlarmDelayCnt[0]			!= m_nAlarmDelayCnt[1])						return RET_ERROR; //kwlee 2016.1124
	if (m_nDirectionCheck[0]		!= m_nDirectionCheck[1])					return RET_ERROR; //kwlee 2016.1124
	if (m_nAutoLoadMode[0]			!= m_nAutoLoadMode[1])						return RET_ERROR; //kwlee 2016.1201
	return RET_GOOD;
}


void CScreenBasic::OnDataApply()
{
	//2017.0109
	if (st_basic_info.nModeDevice	!= m_nModeDevice[1])
	{
		if (st_work_info.nXgemChk == NO)
		{
			st_work_info.nXgemChk = YES;
		}
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("MODE DEVICE");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%d"), st_basic_info.nModeDevice);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%d"), m_nModeDevice[1]);

		clsLog.LogConfig(_T("BASIC"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	//2017.0109
	if (st_basic_info.nModeInterface	!= m_nModeInterface[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("MODE INTERFACE");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%d"), st_basic_info.nModeInterface);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%d"), m_nModeInterface[1]);

		clsLog.LogConfig(_T("BASIC"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_basic_info.nModeWork	!= m_nModeWork[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("MODE WORK");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%d"), st_basic_info.nModeWork);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%d"), m_nModeWork[1]);

		clsLog.LogConfig(_T("BASIC"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_basic_info.strDeviceName	!= m_strDeviceName[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("DEVICE NAME");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%s"), st_basic_info.strDeviceName);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%s"), m_strDeviceName[1]);

		clsLog.LogConfig(_T("BASIC"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_basic_info.nModeFrontSmema	!= m_nModeFrontSmema[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("FRONT SMEMA");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%d"), st_basic_info.nModeFrontSmema);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%d"), m_nModeFrontSmema[1]);

		clsLog.LogConfig(_T("BASIC"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_basic_info.nModeRearSmema	!= m_nModeRearSmema[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("REAR SMEMA");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%d"), st_basic_info.nModeRearSmema);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%d"), m_nModeRearSmema[1]);

		clsLog.LogConfig(_T("BASIC"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_basic_info.nGmsTime	!= m_nGmsTime[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("GMS TIME");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%d"), st_basic_info.nGmsTime);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%d"), m_nGmsTime[1]);

		clsLog.LogConfig(_T("BASIC"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_recipe_info.nBcrBank	!= m_nBcrBank[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("BCR BANK");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%d"), st_recipe_info.nBcrBank);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%d"), m_nBcrBank[1]);

		clsLog.LogConfig(_T("BASIC"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_basic_info.nMode1DBcr	!= m_n1DBarcodeUse[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("MODE 1DBCR");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%d"), st_basic_info.nMode1DBcr);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%d"), m_n1DBarcodeUse[1]);

		clsLog.LogConfig(_T("BASIC"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_basic_info.nMode2DBcr	!= m_n2DBarcodeUse[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("MODE 2DBCR");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%d"), st_basic_info.nMode2DBcr);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%d"), m_n2DBarcodeUse[1]);

		clsLog.LogConfig(_T("BASIC"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_recipe_info.nAbortTime	!= m_nAbortTime[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("ABORT TIME");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%d"), st_recipe_info.nAbortTime);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%d"), m_nAbortTime[1]);

		clsLog.LogConfig(_T("BASIC"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_recipe_info.nBarcodeCnt	!= m_nBarcodeCnt[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("BCR CNT");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%d"), st_recipe_info.nBarcodeCnt);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%d"), m_nBarcodeCnt[1]);

		clsLog.LogConfig(_T("BASIC"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_basic_info.nUldGoodTrayStack_Count	!= m_nStackerTrayCnt[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("GOODSTACKER CNT");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%d"), st_basic_info.nUldGoodTrayStack_Count);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%d"), m_nStackerTrayCnt[1]);

		clsLog.LogConfig(_T("BASIC"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}

	//kwlee 2017.0609
	if (st_basic_info.nRejectStackerTray_Cnt	!= m_nRejectStackerTrayCnt[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("REJECT STACKER CNT");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%d"), st_basic_info.nRejectStackerTray_Cnt);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%d"), m_nRejectStackerTrayCnt[1]);

		clsLog.LogConfig(_T("BASIC"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}

	if (st_basic_info.nAlarmDelayCnt	!= m_nAlarmDelayCnt[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("ALARMDELAY CNT");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%d"), st_basic_info.nAlarmDelayCnt);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%d"), m_nAlarmDelayCnt[1]);

		clsLog.LogConfig(_T("BASIC"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_basic_info.nModeTestInterface	!= m_nModeTestInterface[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("MODE INTERFACE");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%d"), st_basic_info.nModeTestInterface);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%d"), m_nModeTestInterface[1]);

		clsLog.LogConfig(_T("BASIC"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_basic_info.nDirectionCheckSkip	!= m_nDirectionCheck[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("DIRECTION SKIP");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%d"), st_basic_info.nDirectionCheckSkip);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%d"), m_nDirectionCheck[1]);

		clsLog.LogConfig(_T("BASIC"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_basic_info.nAutoLoadMode	!= m_nAutoLoadMode[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("AUTOLOAD MODE");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%d"), st_basic_info.nAutoLoadMode);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%d"), m_nAutoLoadMode[1]);

		clsLog.LogConfig(_T("BASIC"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}

	//kwlee 2017.0225
	if (st_basic_info.nLotEndSkipMode	!= m_nLotEndMode[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("LOT END MODE");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%d"), st_basic_info.nLotEndSkipMode);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%d"), m_nLotEndMode[1]);

		clsLog.LogConfig(_T("BASIC"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	st_basic_info.nModeDevice		= m_nModeDevice[1];	// [Title Bar 상태 표시] < WHIT/WHIT OUT ㅡ MODE 표시	>
	st_basic_info.nModeInterface	= m_nModeInterface[1];
//	st_basic_info.nModeSecsGem		= m_nModeSecsGem[1];
	st_basic_info.nModeWork			= m_nModeWork[1];
	st_basic_info.strDeviceName		= m_strDeviceName[1];	// [Title Bar 상태 표시] < 장비에서 설정한 DEVICE 표시	>
	st_basic_info.nModeFrontSmema	= m_nModeFrontSmema[1];
	st_basic_info.nModeRearSmema	= m_nModeRearSmema[1];
	//kwlee 2016.0325
	st_basic_info.nColCnt           = m_nColCnt[1];
	st_basic_info.nRowCnt           = m_nRowCnt[1];
	st_basic_info.nGmsTime           = m_nGmsTime[1];
	st_recipe_info.nBcrBank			= m_nBcrBank[1];

	st_basic_info.strModelName		= m_strModelName[1];
	
	
	st_basic_info.strHifixType[0] = m_strHifixData[1];

	st_basic_info.nModeRfid         = m_nModeRfid[1];
	

	st_basic_info.nMode1DBcr        = m_n1DBarcodeUse[1];
	st_basic_info.nMode2DBcr        = m_n2DBarcodeUse[1];

	st_basic_info.nModeLdUldLightCutainSkip = m_nLdUldLightCutainSkip[1];
	st_basic_info.nModeRejLightCutainSkip   = m_nRejLightCutainSkip[1];

	for (int i = 0; i<8; i++)
	{
		st_test_site_info[i].nInterface = m_nFtpUse[1];
	}
	
	st_recipe_info.nAbortTime	= m_nAbortTime[1];
	st_recipe_info.nBarcodeCnt	= m_nBarcodeCnt[1];

	st_basic_info.nUldGoodTrayStack_Count = m_nStackerTrayCnt[1]; //kwlee 2016.0930
	st_basic_info.nRejectStackerTray_Cnt = m_nRejectStackerTrayCnt[1]; //kwlee 2017.0609

	st_basic_info.nAlarmDelayCnt = m_nAlarmDelayCnt[1]; //kwlee 2016.1124
	st_basic_info.nModeTestInterface = m_nModeTestInterface[1];
	st_basic_info.nDirectionCheckSkip = m_nDirectionCheck[1]; //kwlee 2016.1124
	st_basic_info.nAutoLoadMode = m_nAutoLoadMode[1]; //kwlee 2016.1201
	st_basic_info.nLotEndSkipMode = m_nLotEndMode[1]; //kwlee 2017.0225

}

void CScreenBasic::OnDataBackup()
{
	m_nModeDevice[0]		= m_nModeDevice[1];			// [Title Bar 상태 표시] < WHIT/WHIT OUT ㅡ MODE 표시
	m_nModeInterface[0]		= m_nModeInterface[1];
	m_nModeSecsGem[0]		= m_nModeSecsGem[1];
	m_nModeWork[0]			= m_nModeWork[1];
	m_strDeviceName[0]		= m_strDeviceName[1];		// [Title Bar 상태 표시] < 장비에서 설정한 DEVICE 표시	>
	m_nModeFrontSmema[0]	= m_nModeFrontSmema[1];
	m_nModeRearSmema[0]		= m_nModeRearSmema[1];
	m_nModeRfid[0]			= m_nModeRfid[1];
	m_strModelName[0]		= m_strModelName[1];
	m_nBcrBank[0]			= m_nBcrBank[1];

	//kwlee 2016.0809
	m_strHifixData[0]		= m_strHifixData[1];
	m_n1DBarcodeUse[0]		= m_n1DBarcodeUse[1];
	m_n2DBarcodeUse[0]		= m_n2DBarcodeUse[1];

	m_nLdUldLightCutainSkip[0] = m_nLdUldLightCutainSkip[1];
	m_nRejLightCutainSkip[0] = m_nRejLightCutainSkip[1];
	m_nFtpUse[0] = m_nFtpUse[1];
	
	m_nAbortTime[0]			= m_nAbortTime[1];
	m_nBarcodeCnt[0]		= m_nBarcodeCnt[1];
	m_nStackerTrayCnt[0]		= m_nStackerTrayCnt[1]; //kwlee 2016.0930
	
	m_nRejectStackerTrayCnt[0]	= m_nRejectStackerTrayCnt[1]; //kwlee 2017.0609

	m_nModeTestInterface[0] = m_nModeTestInterface[1];
}


void CScreenBasic::OnDataInit()
{
	m_nModeDevice[1]					= st_basic_info.nModeDevice;
	m_strDeviceName[1]					= st_basic_info.strDeviceName;

	m_nModeInterface[1]					= st_basic_info.nModeInterface;
//	m_nModeSecsGem[1]					= st_basic_info.nModeSecsGem;
	m_nModeWork[1]						= st_basic_info.nModeWork;
	m_nModeFrontSmema[1]				= st_basic_info.nModeFrontSmema;
	m_nModeRearSmema[1]					= st_basic_info.nModeRearSmema;
	m_nModeRfid[1]						= st_basic_info.nModeRfid;
	m_strModelName[1]					= st_basic_info.strModelName;

	m_nBcrBank[1]						= st_recipe_info.nBcrBank;

	m_nRowCnt[1]							= st_basic_info.nRowCnt;
	m_nColCnt[1]							= st_basic_info.nColCnt;
	m_nGmsTime[1]							= st_basic_info.nGmsTime;

	//kwlee 2016.0809
	m_strHifixData[1]                     = st_basic_info.strHifixType[0];

	m_n1DBarcodeUse[1]                      = st_basic_info.nMode1DBcr;
	m_n2DBarcodeUse[1]                      = st_basic_info.nMode2DBcr;
	
	m_nLdUldLightCutainSkip[1]              =	st_basic_info.nModeLdUldLightCutainSkip;
	m_nRejLightCutainSkip[1]				=	st_basic_info.nModeRejLightCutainSkip;
	for (int i = 0; i<8; i++)
	{
		m_nFtpUse[1] = st_test_site_info[i].nInterface;
	}
	//st_basic_info.nUldGoodTrayStack_Count = m_nStackerTrayCnt[1];
	m_nAbortTime[1]						= st_recipe_info.nAbortTime;
	m_nBarcodeCnt[1]					= st_recipe_info.nBarcodeCnt;
	m_nStackerTrayCnt[1]                = st_basic_info.nUldGoodTrayStack_Count; //kwlee 2016.0930
	m_nRejectStackerTrayCnt[1]          = st_basic_info.nRejectStackerTray_Cnt; //kwlee 2017.0609

	m_nModeTestInterface[1]				= st_basic_info.nModeTestInterface;
	m_nAlarmDelayCnt[1]                = st_basic_info.nAlarmDelayCnt; //kwlee 2016.1124
	m_nDirectionCheck[1]                = st_basic_info.nDirectionCheckSkip; //kwlee 2016.1124
	m_nAutoLoadMode[1]                = st_basic_info.nAutoLoadMode; //kwlee 2016.1201
	m_nLotEndMode[1]                = st_basic_info.nLotEndSkipMode; //kwlee 2016.1201
	OnDataBackup();
}

int CScreenBasic::OnInitModelList(CString strFileName)
{
	int nCnt = 0;
	int nChk;						// 함수 리턴 플래그
	CString strTmp;

	HANDLE hFind;
    WIN32_FIND_DATA fd;

	if (strFileName.Right (1) != "\\")
	{
        strFileName += "\\";
	}

	strFileName += "*.*";
    
    if ((hFind = ::FindFirstFile ((LPCTSTR) strFileName, &fd)) != INVALID_HANDLE_VALUE) 
	{
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
		{
			nChk = OnAddItem(nCnt, &fd);
			if (nChk == RET_GOOD)
			{
				nCnt++;
			}
        }

        while (::FindNextFile (hFind, &fd)) 
		{
			nChk = RET_READY;
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				nChk = OnAddItem(nCnt, &fd);
				if (nChk == RET_ERROR) 
				{
					break;
				}
				else
				{
					nCnt++;
				}
			}
        }
		::FindClose(hFind);
    }
	
	return nCnt;
}

int CScreenBasic::OnAddItem(int nIndex, WIN32_FIND_DATA *pFd)
{
	CString strTempDvc;
	CString strDevice;
	CString strExpansion;
	int nPos;

	HICON	hIcon = NULL;

	strTempDvc = (pFd->cFileName);	// 파일 이름 설정

	strTempDvc.MakeUpper();
	
	/* ************************************************************************** */
	/* 파일 이름 중 확장자 확인                                                   */
	/* ************************************************************************** */
	nPos = strTempDvc.Find('.');		// 확장자 위치 설정
	
	if (nPos == -1)					// 확장자 미존재
	{
		strExpansion = _T("");
	}
	else								// 확장자 존재
	{
		strExpansion = strTempDvc.Mid(nPos + 1);
	}
	/* ************************************************************************** */

	if (strExpansion != _T("TXT"))
	{
		return RET_READY;	
	}

	strDevice = strTempDvc;			// 파일명 전체 화면 출력
	
	if (strDevice == m_strDeviceName[1])
	{
		m_nDeviceName = nIndex;
		OnDisplayModelList(1, nIndex, strDevice);
	}
	else
	{
		OnDisplayModelList(0, nIndex, strDevice);
	}
	/* ************************************************************************** */

    return RET_GOOD;
}

void CScreenBasic::OnDisplayModelList(int nMode, int nCount, CString strFileName)
{
	m_pGridModel.SetRowCount(nCount + 2); 
	m_pGridModel.SetColumnCount(1); 

	m_pGridModel.SetRowHeight(nCount + 1, 50);
	m_pGridModel.SetColumnWidth(0, 300);

	m_pGridModel.SetItemFormat(nCount + 1, 0, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	m_pGridModel.SetItemState(nCount + 1, 0, GVIS_READONLY);
	m_pGridModel.SetItemBkColour(nCount + 1, 0, WHITE_C, CLR_DEFAULT);

	if(nMode == 0)
	{
		m_pGridModel.SetItemBkColour(nCount + 1, 0, BLACK_L, YELLOW_C);
	}
	else
	{
		m_pGridModel.SetItemBkColour(nCount + 1, 0, RGB(79, 79, 225), WHITE_C);
	}
	m_pGridModel.SetItemFont(nCount + 1, 0, &clsFunc.OnLogFont(16));
	m_pGridModel.SetItemText(nCount + 1, 0, strFileName);
}

void CScreenBasic::OnClickedBtnModelCreate()
{
	// TODO: Add your control notification handler code here
	int nPos;

	CString strTmp, strFileName;
	
	KeyBoard(&strFileName);

	if(strFileName != "")
	{
		nPos = -1;
		nPos = strFileName.Find(_T("."), 0);
		
		strTmp = _T("");
		if(nPos > 0)
		{
			strTmp = strFileName.Mid(nPos+1, 3);
		}
		
		if(strTmp != "TXT")
		{
			strFileName += ".TXT";
		}
		
		if(OnModelCreateDeviceCheck(strFileName) == FALSE)
		{
			return;
		}

		OnDataBackup();

		clsBasic.OnBasic_Data_Save_As(strFileName);
		clsBasic.OnModuleRobot_Teach_Data_Save_As(strFileName);
		clsBasic.OnMaintenance_Data_Save_As(strFileName);
		clsBasic.OnInterface_Data_Save_As(strFileName);
		clsBasic.OnRecipe_Data_Save_As(strFileName);

		OnInitGridModelList();
	}
}


void CScreenBasic::OnClickedBtnModelDelete()
{
	// TODO: Add your control notification handler code here
	CDialog_Message	dlgMsg;
	char			chFileName[100];
	CString			strTmp;
	CString			strFileName;
	int				nResponse;
	int				nExistence;			// 파일 존재 유무 플래그

	strTmp = m_strDeviceName[1];
	if(OnModelDeviceCheck(strTmp) == false)  // 입력된 디바이스 이름 정상 유무 검사 함수
	{
		return ;
	}
	
	dlgMsg.m_nMessageType	= 1;
	dlgMsg.m_strMessage		= _T("Are you sure you want to delete the selected information?");

	nResponse = (int)dlgMsg.DoModal();
	
	if(nResponse == IDOK)
	{
		if(strTmp == m_strDeviceName[1])
		{
			m_strDeviceName[0] = m_strDeviceName[1] = m_pGridModel.GetItemText(1, 0);
			if(m_strDeviceName[1] == strTmp)
			{
				m_strDeviceName[1] = "";
				m_strDeviceName[0] = m_strDeviceName[1] = m_pGridModel.GetItemText(2, 0);
			}

		}
		
		strFileName = st_path_info.strBasic + strTmp;
		//wsprintfA(chFileName, "%S", strFileName);
		clsFunc.OnStringToChar(strFileName, chFileName);
// jtkim 20130704
//		nExistence = access(chFileName, 0) ;
		nExistence = _access(chFileName, 0) ;
		if (nExistence != -1)			// 파일 존재
		{
			DeleteFile(strFileName);  // 해당 파일 삭제
			
			OnInitGridModelList();
		}
		else  
		{
			return ;
		}
	}
}

void CScreenBasic::OnClickedBtnReload()
{
	CDialog_Message dlgMsg;
	int nResponse;

	dlgMsg.m_nMessageType	= 1;
	dlgMsg.m_strMessage		= _T("Do you want to change the existing value to the current value?");

	nResponse = (int)dlgMsg.DoModal();

	if (nResponse == IDOK)
	{
		OnDataRecovery();					// 화면 셋팅 정보 백업 받아놓은 변수로 복구 함수

		OnDataInit();
	}
}


void CScreenBasic::OnClickedBtnApply()
{
	CDialog_Message dlgMsg;
	CString strTemp, strErr;
	int nResponse;
	
	dlgMsg.m_nMessageType	= 1;
	dlgMsg.m_strMessage		= _T("Do you want to change the existing value to the current value?");

	nResponse = (int)dlgMsg.DoModal();

	if (nResponse == IDOK)
	{
		//2017.0309
// 		//초기화 후에는 무조건 interface,with 타임아웃 설정 시간 모드로 
// 		st_basic_info.nModeDevice = WITH_DVC;
// 		st_basic_info.nModeInterface = EQP_ON_LINE;
// 		st_basic_info.nModeTestInterface = EQP_ON_LINE;
// 		//처음 로딩시에는 무조건 타임아웃설정
// 		st_recipe_info.nAbortTime = 15000;
		strTemp.Empty();
		strErr.Empty();
		if( m_nModeDevice[1] != WITH_DVC || m_nModeInterface[1] != EQP_ON_LINE || m_nModeTestInterface[1] != EQP_ON_LINE || m_nAbortTime[1] < 12000) 
		{
			if( m_nModeDevice[1] != WITH_DVC )
			{
				dlgMsg.m_strMessage = _T("디바이스 없이 동작합니다. WITHOUT 모드가 맞습니까?");		
				strErr = _T(" 디바이스 없이 동적합니다. 유의하세요");
			}
			else if( m_nModeInterface[1] != EQP_ON_LINE )
			{
				dlgMsg.m_strMessage = _T("OnLine 모드가 아닙니다. Offline 모드가 맞습니까?");	
				strErr = _T("OnLine 모드가 아닙니다. 유의하세요");
			}
			else if( m_nModeTestInterface[1] != EQP_ON_LINE )
			{
				dlgMsg.m_strMessage = _T("테스터와 핸들러가 OnLine 모드가 아닙니다. Offline 모드가 맞습니까?");
				strErr = _T("테스터와핸들러가 OnLine 모드가 아닙니다. 유의하세요");
			}
			else if( m_nAbortTime[1] < 10000) 
			{
				dlgMsg.m_strMessage = _T("Abort Time이 너무 작습니다.맞습니까?");
				strErr = _T("Abort Time이 너무 작습니다. 유의하세요");
			}

			if (dlgMsg.DoModal() == IDOK)
			{
				strTemp.Format(_T("Basic 메유: %s"),dlgMsg.m_strMessage);
				clsMem.OnAbNormalMessagWrite(strTemp);//로그 저장
				st_handler_info.mstr_event_msg[0] = strErr;
				::PostMessage(st_handler_info.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
				clsMem.OnAbNormalMessagWrite(strErr);
			}
			else
			{
				strErr = _T("Basic 메뉴가 저장되지 않았습니다. 수정 후 다시 Apply 해주세요");
				st_handler_info.mstr_event_msg[0] = strErr;
				::PostMessage(st_handler_info.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
				return;
			}
		}


		OnDataHistoryLog();
		OnDataApply();
		OnDataBackup();

		OnInitialUpdate();

		
		if (st_handler_info.cWndTitle != NULL)
		{
			st_handler_info.cWndTitle->PostMessage(WM_STATUS_CHANGE, MODEL_MODE, 0);			// Device Mode
		}
	}
}

bool CScreenBasic::OnModelDeviceCheck(CString strFileName)
{
	CString strChkFile;  /* 생성할 [폴더]+[파일명] 저장 변수 */
	CString strChkExt;   /* 확장자 저장 변수 */
	int nPos;

	/* ************************************************************************** */
    /* 입력된 디바이스명 설정한다                                                 */
    /* ************************************************************************** */

	strFileName.MakeUpper();
	strFileName.TrimLeft(' ');               
	strFileName.TrimRight(' ');

	if(strFileName.IsEmpty())  
	{
		if(st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
		{
			//wsprintfA(st_other_info.cAbnormalMsg, "%S",  _T("[DEVICE] A name input error occurrence."));
			clsFunc.OnStringToChar(_T("[DEVICE] A name input error occurrence."), st_other_info.cAbnormalMsg);
			//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
		}
		return false;
	}
	/* ************************************************************************** */

	strChkFile = st_path_info.strPathDvc + strFileName;  // 생성할 [폴더]+[파일명] 설정
	nPos = strChkFile.Find(_T("."));  // 확장자 검사
	if (nPos == -1) 
	{
		strChkFile += _T(".TXT");
	}
	else 
	{
		strChkExt = strChkFile.Mid(nPos);  // 확장자 정보 설정
		if (strChkExt != _T(".TXT"))  
		{
			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
				//wsprintfA(st_other_info.cAbnormalMsg, "%S", _T("[DEVICE] A name extension input error occurrence."));
				//clsFunc.OnStringToChar(_T("[DEVICE] A name extension input error occurrence."), st_other_info.cAbnormalMsg);
				clsMem.OnAbNormalMessagWrite(_T("[DEVICE] A name extension input error occurrence."));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
			}
			return false;
		}
	}

	m_strLoadFile = strChkFile;  // 생성할 [폴더]+[파일명]+[확장자] 설정

	return true;
}

bool CScreenBasic::OnModelCreateDeviceCheck(CString strFileName)
{
	CString strChkFile;  // 생성할 [폴더]+[파일명] 저장 변수
	CString strChkExt;   // 확장자 저장 변수
	int nPos;

	/* ************************************************************************** */
    /* 입력된 디바이스명 설정한다                                                 */
    /* ************************************************************************** */
	strFileName.MakeUpper();
	strFileName.TrimLeft(' ');               
	strFileName.TrimRight(' ');

	if(strFileName.IsEmpty())  
	{
		if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
		{
			//wsprintfA(st_other_info.cNormalMsg, "%S", _T("[DEVICE] A name input error occurrence."));
			//clsFunc.OnStringToChar(_T("[DEVICE] A name input error occurrence."), st_other_info.cNormalMsg);
			clsMem.OnAbNormalMessagWrite(_T("[DEVICE] A name input error occurrence."));
			//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
		}
		return false;
	}
	/* ************************************************************************** */

	strChkFile = st_path_info.strBasic + strFileName;  // 생성할 [폴더]+[파일명] 설정
	nPos = strChkFile.Find(_T("."));  // 확장자 검사
	if (nPos == -1) 
		strChkFile += _T(".TXT");
	else 
	{
		strChkExt = strChkFile.Mid(nPos);  // 확장자 정보 설정
		if (strChkExt != _T(".TXT"))  
		{
			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
				//wsprintfA(st_other_info.cNormalMsg, "%S",  _T("[DEVICE] A name extension input error occurrence."));
				//clsFunc.OnStringToChar(_T("[DEVICE] A name extension input error occurrence."), st_other_info.cNormalMsg);
				clsMem.OnAbNormalMessagWrite(_T("[DEVICE] A name extension input error occurrence."));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
			}
			return false;
		}
	}

	m_strLoadFile = strChkFile;  // 생성할 [폴더]+[파일명]+[확장자] 설정

	return TRUE;
}

BOOL CScreenBasic::OnEraseBkgnd(CDC* pDC)
{
	CDC  memDC;                     // 메모리 DC
	CBitmap * pOldBitmap = NULL;    // m_BackBitmap으로 새로운 그림을 DC에 그릴 때, 이전 그려졌던 DC(즉, Bitmap)을 저장.
	BITMAP bitmapInfo;              // 그림의 정보(m_BackBitmap)
	st_handler_info.m_BackBitmap.GetBitmap(&bitmapInfo); // Bitmap 크기 구함.

	memDC.CreateCompatibleDC(pDC);  // 메모리 DC 생성
	pOldBitmap = memDC.SelectObject(&st_handler_info.m_BackBitmap);
	pDC->BitBlt(0, 0, bitmapInfo.bmWidth, bitmapInfo.bmHeight, &memDC, 0, 0, SRCCOPY);   // 메모리 DC에 그림을 그림

	memDC.SelectObject(pOldBitmap);
	memDC.DeleteDC();
	return true; // 흰 부러쉬가 아닌 배경을 그려야 하기 때문에 true
}



void CScreenBasic::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
/*	if (OnDataComparison() == RET_ERROR)
	{
		OnDataHistoryLog();
		OnDataApply();
	}
	st_handler_info.cWndBasic = NULL;*/
	CFormView::PostNcDestroy();
}


void CScreenBasic::OnDestroy()
{
	CFormView::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (OnDataComparison() == RET_ERROR)
	{
		OnDataHistoryLog();
		OnDataApply();
	}
	st_handler_info.cWndBasic = NULL;
}

HBRUSH CScreenBasic::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_STATIC)
	{
		switch(pWnd->GetDlgCtrlID())
		{
		case IDC_TEXT_TOOLNAME:
		case IDC_TEXT_PARTNO:
		case IDC_TEXT_COMMENT:
			pDC->SetBkColor(WHITE_C);
			return (HBRUSH)::GetStockObject(WHITE_BRUSH);
			break;

		default:
			pDC->SetBkMode(TRANSPARENT);
			return (HBRUSH)::GetStockObject(NULL_BRUSH);
			break;
		}

	}
	return hbr;
}


void CScreenBasic::OnTimer(UINT_PTR nIDEvent)
{
	CFormView::OnTimer(nIDEvent);
}


void CScreenBasic::OnStnClickedDgtRowCnt()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_KeyPad dlgKeyPad;

	dlgKeyPad.m_nKeypadMode			= 0;
	dlgKeyPad.m_strKeypadLowLimit	= _T("0");
	dlgKeyPad.m_strKeypadHighLimit	= _T("5");
	dlgKeyPad.m_strKeypadVal.Format(_T("%d"), m_nRowCnt[1]);

	if (dlgKeyPad.DoModal() == IDOK)
	{
		m_nRowCnt[1] = _wtoi(dlgKeyPad.m_strNewVal);
		m_dgtPcbRowCnt.SetVal(m_nRowCnt[1]);
	}
}


void CScreenBasic::OnStnClickedDgtColCnt()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_KeyPad dlgKeyPad;

	dlgKeyPad.m_nKeypadMode			= 0;
	dlgKeyPad.m_strKeypadLowLimit	= _T("0");
	dlgKeyPad.m_strKeypadHighLimit	= _T("5");
	dlgKeyPad.m_strKeypadVal.Format(_T("%d"), m_nColCnt[1]);

	if (dlgKeyPad.DoModal() == IDOK)
	{
		m_nColCnt[1] = _wtoi(dlgKeyPad.m_strNewVal);
		m_dgtPcbColCnt.SetVal(m_nColCnt[1]);
	}
}


void CScreenBasic::OnStnClickedDgtGmsTime()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_KeyPad dlgKeyPad;

	dlgKeyPad.m_nKeypadMode			= 0;
	dlgKeyPad.m_strKeypadLowLimit	= _T("10");
	dlgKeyPad.m_strKeypadHighLimit	= _T("1440");
	dlgKeyPad.m_strKeypadVal.Format(_T("%d"), m_nGmsTime[1]);

	if (dlgKeyPad.DoModal() == IDOK)
	{
		m_nGmsTime[1] = _wtoi(dlgKeyPad.m_strNewVal);
		m_dgtGmsTime.SetVal(m_nGmsTime[1]);
	}
}


void CScreenBasic::OnBnClickedBtnTimer()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	
	if (m_bTimerCheck == true)
	{
		SetTimer(TM_GMS,st_basic_info.nGmsTime,NULL);
		m_bTimerCheck = false;
		m_btnTimer.SetWindowText(_T("START"));
	}
	else
	{
		KillTimer(TM_GMS);
		m_btnTimer.SetWindowText(_T("STOP"));
		m_bTimerCheck = true;
	}
}


void CScreenBasic::OnStnClickedDgtBasicBarcodeBank()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_KeyPad dlgKeyPad;

	dlgKeyPad.m_nKeypadMode			= 0;
	dlgKeyPad.m_strKeypadLowLimit	= _T("1");
	dlgKeyPad.m_strKeypadHighLimit	= _T("15");
	dlgKeyPad.m_strKeypadVal.Format(_T("%d"), m_nBcrBank[1]);

	if (dlgKeyPad.DoModal() == IDOK)
	{
		m_nBcrBank[1] = _wtoi(dlgKeyPad.m_strNewVal);
		m_dgtBcrBank.SetVal(m_nBcrBank[1]);
	}
}


void CScreenBasic::OnStnClickedDgtIbmLength()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CScreenBasic::OnStnClickedDgtSamsungLength()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CScreenBasic::OnStnClickedMsgBarcodeData()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CScreenBasic::OnStnClickedDgtLength()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}




void CScreenBasic::OnStnClickedMsgHifixData()
{
	CDialogHifix dlgHifix;

	dlgHifix.DoModal();
	m_strHifixData[1] = dlgHifix.m_strHifixData;
	m_msgHifixData.SetWindowText(m_strHifixData[1]);
}


void CScreenBasic::OnStnClickedDgtAbortTime()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_KeyPad dlgKeyPad;

	dlgKeyPad.m_nKeypadMode			= 0;
	dlgKeyPad.m_strKeypadLowLimit	= _T("0");
	dlgKeyPad.m_strKeypadHighLimit	= _T("15000");
	dlgKeyPad.m_strKeypadVal.Format(_T("%d"), m_nAbortTime[1]);

	if (dlgKeyPad.DoModal() == IDOK)
	{
		m_nAbortTime[1] = _wtoi(dlgKeyPad.m_strNewVal);
		m_dgtAbortTime.SetVal(m_nAbortTime[1]);
	}
}


void CScreenBasic::OnStnClickedDgtBarcodeCnt()
{
	CDialog_KeyPad dlgKeyPad;

	dlgKeyPad.m_nKeypadMode			= 0;
	dlgKeyPad.m_strKeypadLowLimit	= _T("0");
	dlgKeyPad.m_strKeypadHighLimit	= _T("100");
	dlgKeyPad.m_strKeypadVal.Format(_T("%d"), m_nBarcodeCnt[1]);

	if (dlgKeyPad.DoModal() == IDOK)
	{
		m_nBarcodeCnt[1] = _wtoi(dlgKeyPad.m_strNewVal);
		m_dgtBarcodeCnt.SetVal(m_nBarcodeCnt[1]);
	}
}

//kwlee20160928
//kwlee 2016.1107
void CScreenBasic::OnBnClickedBtnAuto()
{
	CDialog_Message	dlgMsg;
	//int nResponse,i,j;
	int nResponse;
	bool bRun = false;
	if (st_handler_info.nAutoMode == YES)
	{	
		if (st_lot_info[LOT_CURR].strLotNo == _T(""))
		{
			st_handler_info.nAutoMode = NO;
			m_btnAuto.SetBitmaps(IDC_BTN_AUTO, IDB_BITMAP_APPLY_DN, WINDOW_DN, IDB_BITMAP_APPLY_UP, WINDOW_UP);
			m_btnAuto.SetWindowTextW(_T("Manual Mode"));

			clsFunc.OnLogFileAdd(0, _T("Manual Mode"));
		}
		else
		{
			dlgMsg.m_nMessageType	= 1;
			dlgMsg.m_strMessage		= _T("현재 설비상태가 AutoMode 입니다.");

			nResponse = (int)dlgMsg.DoModal();
			if(nResponse != IDOK) return;

			clsFunc.OnLogFileAdd(0, dlgMsg.m_strMessage);
		}

		
	}
	else
	{
// 		if (st_lot_info[LOT_CURR].strLotNo != _T(""))
// 		{
// 			for( i = 0; i < 8; i++)
// 			{
// 				for (j =0; j<8; j++)
// 				{
// 					if( st_test_site_info[i].st_pcb_info[j].nYesNo == YES)
// 					{
// 						bRun = true;
// 						break;
// 					}
// 				}
// 			}
// 			if( bRun == true)
// 			{
				st_handler_info.nAutoMode = YES;
				m_btnAuto.SetBitmaps(IDC_BTN_AUTO, IDB_BITMAP_APPLY_DN, WINDOW_DN, IDB_BITMAP_APPLY_UP, WINDOW_UP);
				m_btnAuto.SetWindowTextW(_T("Auto Mode"));

				clsFunc.OnLogFileAdd(0, _T("Auto Mode"));
			//}
		//}
	}
	Invalidate(FALSE);
}


void CScreenBasic::OnStnClickedDgtStackerTrayCnt()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_KeyPad dlgKeyPad;

	dlgKeyPad.m_nKeypadMode			= 0;
	dlgKeyPad.m_strKeypadLowLimit	= _T("0");
	dlgKeyPad.m_strKeypadHighLimit	= _T("100");
	dlgKeyPad.m_strKeypadVal.Format(_T("%d"), m_nStackerTrayCnt[1]);

	if (dlgKeyPad.DoModal() == IDOK)
	{
		m_nStackerTrayCnt[1] = _wtoi(dlgKeyPad.m_strNewVal);
		m_dgtStackTrayCnt.SetVal(m_nStackerTrayCnt[1]);
	}
}

//kwlee 2016.1011
void CScreenBasic::OnBnClickedBtnTest1()
{
	CDialog_Message	dlgMsg;
	int nSite = THD_TESTSITE_1, i, nResponse;
	bool nRun = false;

	if (st_handler_info.nAutoMode ==  NO)
	{
		if (st_handler_info.nRunStatus		== dSTOP &&
			st_handler_info.nMachineMode	== MACHINE_MANUAL)
		{
			st_test_site_info[nSite].nTestBdStart = BD_START;
		}
		else
		{
			AfxMessageBox(_T("설비 Stop 상태 및 Manual 상태 인지 확인 하세요"));
		}
	}
	else
	{
		dlgMsg.m_nMessageType	= 1;
		dlgMsg.m_strMessage		= _T("Do you want to start tester1 on Automode?");

		nResponse = (int)dlgMsg.DoModal();
		if(nResponse != IDOK) return;

		if(st_handler_info.nRunStatus == dSTOP)
		{
			for( i = 0; i < 8; i++)
			{
				if( st_test_site_info[nSite].st_pcb_info[i].nYesNo == YES)
				{
					nRun = true;
					break;
				}
			}
			if( nRun == true)
			{
				st_test_site_info[nSite].nTestBdStart = BD_START;
			}
		}
		else
		{
			AfxMessageBox(_T("설비 Stop 상태 및 Manual 상태 인지 확인 하세요"));
		}
	}
}


void CScreenBasic::OnBnClickedBtnTest2()
{
	CDialog_Message	dlgMsg;
	int nSite = THD_TESTSITE_2, i, nResponse;
	bool nRun = false;

	if (st_handler_info.nAutoMode == NO)
	{
		if (st_handler_info.nRunStatus		== dSTOP &&
			st_handler_info.nMachineMode	== MACHINE_MANUAL)
		{
			st_test_site_info[nSite].nTestBdStart = BD_START;
		}
		else
		{
			AfxMessageBox(_T("설비 Stop 상태 및 Manual 상태 인지 확인 하세요"));
		}
	}
	else
	{
		dlgMsg.m_nMessageType	= 1;
		dlgMsg.m_strMessage		= _T("Do you want to start tester2 on Automode?");

		nResponse = (int)dlgMsg.DoModal();
		if(nResponse != IDOK) return;

		if(st_handler_info.nRunStatus == dSTOP)
		{
			for( i = 0; i < 8; i++)
			{
				if( st_test_site_info[nSite].st_pcb_info[i].nYesNo == YES)
				{
					nRun = true;
					break;
				}
			}
			if( nRun == true)
			{
				st_test_site_info[nSite].nTestBdStart = BD_START;
			}
		}
		else
		{
			AfxMessageBox(_T("설비 Stop 상태 및 Manual 상태 인지 확인 하세요"));
		}
	}
}


void CScreenBasic::OnBnClickedBtnTest3()
{
	CDialog_Message	dlgMsg;
	int nSite = THD_TESTSITE_3, i, nResponse;
	bool nRun = false;

	if (st_handler_info.nAutoMode == NO)
	{
		if (st_handler_info.nRunStatus		== dSTOP &&
			st_handler_info.nMachineMode	== MACHINE_MANUAL)
		{
			st_test_site_info[nSite].nTestBdStart = BD_START;
		}
		else
		{
			AfxMessageBox(_T("설비 Stop 상태 및 Manual 상태 인지 확인 하세요"));
		}
	}
	else
	{
		dlgMsg.m_nMessageType	= 1;
		dlgMsg.m_strMessage		= _T("Do you want to start tester3 on Automode?");

		nResponse = (int)dlgMsg.DoModal();
		if(nResponse != IDOK) return;
		if(st_handler_info.nRunStatus == dSTOP)
		{
			for( i = 0; i < 8; i++)
			{
				if( st_test_site_info[nSite].st_pcb_info[i].nYesNo == YES)
				{
					nRun = true;
					break;
				}
			}
			if( nRun == true)
			{
				st_test_site_info[nSite].nTestBdStart = BD_START;
			}
		}
		else
		{
			AfxMessageBox(_T("Auto / Manual 상태가 Manual 인지 확인 하세요"));
		}
	}
}


void CScreenBasic::OnBnClickedBtnTest4()
{
	CDialog_Message	dlgMsg;
	int nSite = THD_TESTSITE_4, i, nResponse;
	bool nRun = false;

	if (st_handler_info.nAutoMode == NO)
	{
		if (st_handler_info.nRunStatus		== dSTOP &&
			st_handler_info.nMachineMode	== MACHINE_MANUAL)
		{
			st_test_site_info[nSite].nTestBdStart = BD_START;
		}
		else
		{
			AfxMessageBox(_T("설비 Stop 상태 및 Manual 상태 인지 확인 하세요"));
		}
	}
	else
	{
		dlgMsg.m_nMessageType	= 1;
		dlgMsg.m_strMessage		= _T("Do you want to start tester4 on Automode?");

		nResponse = (int)dlgMsg.DoModal();
		if(nResponse != IDOK) return;
		if(st_handler_info.nRunStatus == dSTOP)
		{
			for( i = 0; i < 8; i++)
			{
				if( st_test_site_info[nSite].st_pcb_info[i].nYesNo == YES)
				{
					nRun = true;
					break;
				}
			}
			if( nRun == true)
			{
				st_test_site_info[nSite].nTestBdStart = BD_START;
			}
		}
		else
		{
			AfxMessageBox(_T("Auto / Manual 상태가 Manual 인지 확인 하세요"));
		}
	}
}


void CScreenBasic::OnBnClickedBtnTest5()
{
	CDialog_Message	dlgMsg;
	int nSite = THD_TESTSITE_5, i, nResponse;
	bool nRun = false;

	if (st_handler_info.nAutoMode == NO)
	{
		if (st_handler_info.nRunStatus		== dSTOP &&
			st_handler_info.nMachineMode	== MACHINE_MANUAL)
		{
			st_test_site_info[nSite].nTestBdStart = BD_START;
		}
		else
		{
			AfxMessageBox(_T("설비 Stop 상태 및 Manual 상태 인지 확인 하세요"));
		}
	}
	else
	{
		dlgMsg.m_nMessageType	= 1;
		dlgMsg.m_strMessage		= _T("Do you want to start tester5 on Automode?");

		nResponse = (int)dlgMsg.DoModal();
		if(nResponse != IDOK) return;
		if(st_handler_info.nRunStatus == dSTOP)
		{
			for( i = 0; i < 8; i++)
			{
				if( st_test_site_info[nSite].st_pcb_info[i].nYesNo == YES)
				{
					nRun = true;
					break;
				}
			}
			if( nRun == true)
			{
				st_test_site_info[nSite].nTestBdStart = BD_START;
			}
		}
		else
		{
			AfxMessageBox(_T("Auto / Manual 상태가 Manual 인지 확인 하세요"));
		}
	}
}


void CScreenBasic::OnBnClickedBtnTest6()
{
	CDialog_Message	dlgMsg;
	int nSite = THD_TESTSITE_6, i, nResponse;
	bool nRun = false;

	if (st_handler_info.nAutoMode == NO)
	{
		if (st_handler_info.nRunStatus		== dSTOP &&
			st_handler_info.nMachineMode	== MACHINE_MANUAL)
		{
			st_test_site_info[nSite].nTestBdStart = BD_START;
		}
		else
		{
			AfxMessageBox(_T("설비 Stop 상태 및 Manual 상태 인지 확인 하세요"));
		}
	}
	else
	{
		dlgMsg.m_nMessageType	= 1;
		dlgMsg.m_strMessage		= _T("Do you want to start tester6 on Automode?");

		nResponse = (int)dlgMsg.DoModal();
		if(nResponse != IDOK) return;
		if(st_handler_info.nRunStatus == dSTOP)
		{
			for( i = 0; i < 8; i++)
			{
				if( st_test_site_info[nSite].st_pcb_info[i].nYesNo == YES)
				{
					nRun = true;
					break;
				}
			}
			if( nRun == true)
			{
				st_test_site_info[nSite].nTestBdStart = BD_START;
			}
		}
		else
		{
			AfxMessageBox(_T("Auto / Manual 상태가 Manual 인지 확인 하세요"));
		}
	}
}


void CScreenBasic::OnBnClickedBtnTest7()
{
	CDialog_Message	dlgMsg;
	int nSite = THD_TESTSITE_7, i, nResponse;
	bool nRun = false;

	if (st_handler_info.nAutoMode == NO)
	{
		if (st_handler_info.nRunStatus		== dSTOP &&
			st_handler_info.nMachineMode	== MACHINE_MANUAL)
		{
			st_test_site_info[nSite].nTestBdStart = BD_START;
		}
		else
		{
			AfxMessageBox(_T("설비 Stop 상태 및 Manual 상태 인지 확인 하세요"));
		}

	}
	else
	{
		dlgMsg.m_nMessageType	= 1;
		dlgMsg.m_strMessage		= _T("Do you want to start tester7 on Automode?");

		nResponse = (int)dlgMsg.DoModal();
		if(nResponse != IDOK) return;
		if(st_handler_info.nRunStatus == dSTOP)
		{
			for( i = 0; i < 8; i++)
			{
				if( st_test_site_info[nSite].st_pcb_info[i].nYesNo == YES)
				{
					nRun = true;
					break;
				}
			}
			if( nRun == true)
			{
				st_test_site_info[nSite].nTestBdStart = BD_START;
			}
		}
		else
		{
			AfxMessageBox(_T("Auto / Manual 상태가 Manual 인지 확인 하세요"));
		}
	}
}


void CScreenBasic::OnBnClickedBtnTest8()
{
	CDialog_Message	dlgMsg;
	int nSite = THD_TESTSITE_8, i, nResponse;
	bool nRun = false;

	if (st_handler_info.nAutoMode == NO)
	{
		if (st_handler_info.nRunStatus		== dSTOP &&
			st_handler_info.nMachineMode	== MACHINE_MANUAL)
		{
			st_test_site_info[nSite].nTestBdStart = BD_START;
		}
		else
		{
			AfxMessageBox(_T("설비 Stop 상태 및 Manual 상태 인지 확인 하세요"));
		}
	}
	else
	{
		dlgMsg.m_nMessageType	= 1;
		dlgMsg.m_strMessage		= _T("Do you want to start tester8 on Automode?");

		nResponse = (int)dlgMsg.DoModal();
		if(nResponse != IDOK) return;
		if(st_handler_info.nRunStatus == dSTOP)
		{
			for( i = 0; i < 8; i++)
			{
				if( st_test_site_info[nSite].st_pcb_info[i].nYesNo == YES)
				{
					nRun = true;
					break;
				}
			}
			if( nRun == true)
			{
				st_test_site_info[nSite].nTestBdStart = BD_START;
			}
		}
		else
		{
			AfxMessageBox(_T("Auto / Manual 상태가 Manual 인지 확인 하세요"));
		}
	}
}


void CScreenBasic::OnBnClickedBtnChamber1()
{
	
	if (st_handler_info.nAutoMode == NO)
	{
		if (st_handler_info.nRunStatus		== dSTOP &&
			st_handler_info.nMachineMode	== MACHINE_MANUAL)
		{
			st_test_site_info[THD_TESTSITE_1].nTester_Req[0] = CTL_REQ;
			st_test_site_info[THD_TESTSITE_1].nTester_Req[1] = WORK_PICK;
		}
		else
		{
			AfxMessageBox(_T("설비 Stop 상태 및 Manual 상태 인지 확인 하세요"));
		}
	}
	else
	{
		AfxMessageBox(_T("Auto / Manual 상태가 Manual 인지 확인 하세요"));
	}
}


void CScreenBasic::OnBnClickedBtnChamber2()
{
	if (st_handler_info.nAutoMode == NO)
	{
		if (st_handler_info.nRunStatus		== dSTOP &&
			st_handler_info.nMachineMode	== MACHINE_MANUAL)
		{
			st_test_site_info[THD_TESTSITE_2].nTester_Req[0] = CTL_REQ;
			st_test_site_info[THD_TESTSITE_2].nTester_Req[1] = WORK_PICK;
		}
		else
		{
			AfxMessageBox(_T("설비 Stop 상태 및 Manual 상태 인지 확인 하세요"));
		}
	}
	else
	{
		AfxMessageBox(_T("Auto / Manual 상태가 Manual 인지 확인 하세요"));
	}
}


void CScreenBasic::OnBnClickedBtnChamber3()
{
	if (st_handler_info.nAutoMode == NO)
	{
		if (st_handler_info.nRunStatus		== dSTOP &&
			st_handler_info.nMachineMode	== MACHINE_MANUAL)
		{
			st_test_site_info[THD_TESTSITE_3].nTester_Req[0] = CTL_REQ;
			st_test_site_info[THD_TESTSITE_3].nTester_Req[1] = WORK_PICK;
		}
		else
		{
			AfxMessageBox(_T("설비 Stop 상태 및 Manual 상태 인지 확인 하세요"));
		}
	}
	else
	{
		AfxMessageBox(_T("Auto / Manual 상태가 Manual 인지 확인 하세요"));
	}
}


void CScreenBasic::OnBnClickedBtnChamber4()
{
	if (st_handler_info.nAutoMode == NO)
	{
		if (st_handler_info.nRunStatus		== dSTOP &&
			st_handler_info.nMachineMode	== MACHINE_MANUAL)
		{
			st_test_site_info[THD_TESTSITE_4].nTester_Req[0] = CTL_REQ;
			st_test_site_info[THD_TESTSITE_4].nTester_Req[1] = WORK_PICK;
		}
		else
		{
			AfxMessageBox(_T("설비 Stop 상태 및 Manual 상태 인지 확인 하세요"));
		}
	}
	else
	{
		AfxMessageBox(_T("Auto / Manual 상태가 Manual 인지 확인 하세요"));
	}
}


void CScreenBasic::OnBnClickedBtnChamber5()
{
	if (st_handler_info.nAutoMode == NO)
	{
		if (st_handler_info.nRunStatus		== dSTOP &&
			st_handler_info.nMachineMode	== MACHINE_MANUAL)
		{
			st_test_site_info[THD_TESTSITE_5].nTester_Req[0] = CTL_REQ;
			st_test_site_info[THD_TESTSITE_5].nTester_Req[1] = WORK_PICK;
		}
		else
		{
			AfxMessageBox(_T("설비 Stop 상태 및 Manual 상태 인지 확인 하세요"));
		}
	}
	else
	{
		AfxMessageBox(_T("Auto / Manual 상태가 Manual 인지 확인 하세요"));
	}
}


void CScreenBasic::OnBnClickedBtnChamber6()
{
	if (st_handler_info.nAutoMode == NO)
	{
		if (st_handler_info.nRunStatus		== dSTOP &&
			st_handler_info.nMachineMode	== MACHINE_MANUAL)
		{
			st_test_site_info[THD_TESTSITE_6].nTester_Req[0] = CTL_REQ;
			st_test_site_info[THD_TESTSITE_6].nTester_Req[1] = WORK_PICK;
		}
		else
		{
			AfxMessageBox(_T("설비 Stop 상태 및 Manual 상태 인지 확인 하세요"));
		}
	}
	else
	{
		AfxMessageBox(_T("Auto / Manual 상태가 Manual 인지 확인 하세요"));
	}
}


void CScreenBasic::OnBnClickedBtnChamber7()
{
	if (st_handler_info.nAutoMode == NO)
	{
		if (st_handler_info.nRunStatus		== dSTOP &&
			st_handler_info.nMachineMode	== MACHINE_MANUAL)
		{
			st_test_site_info[THD_TESTSITE_7].nTester_Req[0] = CTL_REQ;
			st_test_site_info[THD_TESTSITE_7].nTester_Req[1] = WORK_PICK;
		}
		else
		{
			AfxMessageBox(_T("설비 Stop 상태 및 Manual 상태 인지 확인 하세요"));
		}
	}
	else
	{
		AfxMessageBox(_T("Auto / Manual 상태가 Manual 인지 확인 하세요"));
	}
}


void CScreenBasic::OnBnClickedBtnChamber8()
{
	if (st_handler_info.nAutoMode == NO)
	{
		if (st_handler_info.nRunStatus		== dSTOP &&
			st_handler_info.nMachineMode	== MACHINE_MANUAL)
		{
			st_test_site_info[THD_TESTSITE_8].nTester_Req[0] = CTL_REQ;
			st_test_site_info[THD_TESTSITE_8].nTester_Req[1] = WORK_PICK;
		}
		else
		{
			AfxMessageBox(_T("설비 Stop 상태 및 Manual 상태 인지 확인 하세요"));
		}
	}
	else
	{
		AfxMessageBox(_T("Auto / Manual 상태가 Manual 인지 확인 하세요"));
	}
}


void CScreenBasic::OnBnClickedBtnDirection()
{

	if (m_nDirectionCheck[1] == YES)
	{	
		m_nDirectionCheck[1] = NO;
		m_btnDirection.SetBitmaps(IDC_BTN_DIRECTION, IDB_BITMAP_APPLY_DN, WINDOW_DN, IDB_BITMAP_APPLY_UP, WINDOW_UP);
		m_btnDirection.SetWindowTextW(_T("Direction Check Use"));

	}
	else
	{

		m_nDirectionCheck[1] = YES;
		m_btnDirection.SetBitmaps(IDC_BTN_DIRECTION, IDB_BITMAP_APPLY_DN, WINDOW_DN, IDB_BITMAP_APPLY_UP, WINDOW_UP);
		m_btnDirection.SetWindowTextW(_T("Direction Skip"));

	}
	Invalidate(FALSE);
}


void CScreenBasic::OnStnClickedDgtAlarmDelayCnt()
{
	CDialog_KeyPad dlgKeyPad;

	dlgKeyPad.m_nKeypadMode			= 0;
	dlgKeyPad.m_strKeypadLowLimit	= _T("0");
	dlgKeyPad.m_strKeypadHighLimit	= _T("1000");
	dlgKeyPad.m_strKeypadVal.Format(_T("%d"), m_nAlarmDelayCnt[1]);

	if (dlgKeyPad.DoModal() == IDOK)
	{
		m_nAlarmDelayCnt[1] = _wtoi(dlgKeyPad.m_strNewVal);
		m_dgtAlarmDelayCnt.SetVal(m_nAlarmDelayCnt[1]);
	}
}


void CScreenBasic::OnBnClickedBtnAutoload()
{


	if (m_nAutoLoadMode[1] == YES)
	{	
		m_nAutoLoadMode[1] = NO;
		m_btnAutoLoad.SetBitmaps(IDC_BTN_DIRECTION, IDB_BITMAP_APPLY_DN, WINDOW_DN, IDB_BITMAP_APPLY_UP, WINDOW_UP);
		m_btnAutoLoad.SetWindowTextW(_T("Auto Load Skip"));

	}
	else
	{

		m_nAutoLoadMode[1] = YES;
		m_btnAutoLoad.SetBitmaps(IDC_BTN_DIRECTION, IDB_BITMAP_APPLY_DN, WINDOW_DN, IDB_BITMAP_APPLY_UP, WINDOW_UP);
		m_btnAutoLoad.SetWindowTextW(_T("Auto Load Use"));

	}
	Invalidate(FALSE);
}


void CScreenBasic::OnBnClickedBtnLotendUse()
{
	if (m_nLotEndMode[1] == YES)
	{	
		m_nLotEndMode[1] = NO;
		m_btnLotendUse.SetBitmaps(IDC_BTN_LOTEND_USE, IDB_BITMAP_APPLY_DN, WINDOW_DN, IDB_BITMAP_APPLY_UP, WINDOW_UP);
		m_btnLotendUse.SetWindowTextW(_T("Lot End Use(서버랑 통신 함)"));
	}
	else
	{
		m_nLotEndMode[1] = YES;
		m_btnLotendUse.SetBitmaps(IDC_BTN_LOTEND_USE, IDB_BITMAP_APPLY_DN, WINDOW_DN, IDB_BITMAP_APPLY_UP, WINDOW_UP);
		m_btnLotendUse.SetWindowTextW(_T("Lot End Skip(서버랑 통신 않함)"));
	}
	Invalidate(FALSE);
}

//kwlee 2017.0609
void CScreenBasic::OnStnClickedDgtRejectStackerTray()
{
	CDialog_KeyPad dlgKeyPad;

	dlgKeyPad.m_nKeypadMode			= 0;
	dlgKeyPad.m_strKeypadLowLimit	= _T("0");
	dlgKeyPad.m_strKeypadHighLimit	= _T("15");
	dlgKeyPad.m_strKeypadVal.Format(_T("%d"), m_nRejectStackerTrayCnt[1]);

	if (dlgKeyPad.DoModal() == IDOK)
	{
		m_nRejectStackerTrayCnt[1] = _wtoi(dlgKeyPad.m_strNewVal);
		m_dgtRejectTrayCnt.SetVal(m_nRejectStackerTrayCnt[1]);
	}
}
