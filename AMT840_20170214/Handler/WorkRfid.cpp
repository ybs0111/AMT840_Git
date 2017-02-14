// WorkRfid.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "WorkRfid.h"
#include "afxdialogex.h"
#include "PublicFunction.h"
#include "Dialog_KeyBoard.h"
#include "Dialog_KeyPad.h"
#include "Dialog_Message.h"
#include "CtlBd_Variable.h"
#include "ClientRfid.h"


// CWorkRfid 대화 상자입니다.
#define TM_RFID_READ		200
#define TM_RFID_WRITE		300
IMPLEMENT_DYNAMIC(CWorkRfid, CDialog)

CWorkRfid::CWorkRfid(CWnd* pParent /*=NULL*/)
	: CDialog(CWorkRfid::IDD, pParent)
{

}

CWorkRfid::~CWorkRfid()
{
}

void CWorkRfid::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GROUP_NETWORK_RFID,		m_groupRfid);
	DDX_Control(pDX, IDC_GROUP_RFID_INFO,			m_groupRfidInfo);
	DDX_Control(pDX, IDC_BTN_APPLY,					m_btnApply);
	DDX_Control(pDX, IDC_BTN_RELOAD,				m_btnReLoad);
	DDX_Control(pDX, IDC_BUTTON_RFID_READ,			m_btnRfidRead);
	DDX_Control(pDX, IDC_BUTTON_RFID_WRITE,			m_btnRfidWrite);
	DDX_Control(pDX, IDC_MSG_CLIENT_IP_RFID,		m_msgClientIpRfid);
	DDX_Control(pDX, IDC_MSG_CLIENT_PORT_RFID,		m_msgClientPortRfid);
	DDX_Control(pDX, IDC_CLIENT_IP_RFID,			m_clientIpRfid);
	DDX_Control(pDX, IDC_EDIT_CLIENT_PORT_RFID,		m_editClientPortRfid);
	DDX_Control(pDX, IDC_BTN_CLIENT_IP_RFID,		m_btnClientIpRfid);
	DDX_Control(pDX, IDC_BTN_CLIENT_PORT_RFID,		m_btnClientPortRfid);
	DDX_Control(pDX, IDC_MSG_CARRIER_ID,			m_msgCarrier);
	DDX_Control(pDX, IDC_MSG_CARRIER_ID_DATA,		m_msgCarrierData);
	DDX_Control(pDX, IDC_MSG_EQP_NO,				m_msgEqpNo);
	DDX_Control(pDX, IDC_MSG_EQP_NO_DATA,			m_msgEqpNoData);
	DDX_Control(pDX, IDC_MSG_QTY,					m_msgQty);
	DDX_Control(pDX, IDC_MSG_QTY_DATA,				m_msgQtyData);
	DDX_Control(pDX, IDC_MSG_LOT_NO_J,				m_msgLotNoJ);
	DDX_Control(pDX, IDC_MSG_LOT_NO_J_DATA,			m_msgLotNoJData);
	DDX_Control(pDX, IDC_MSG_STEP,					m_msgStep);
	DDX_Control(pDX, IDC_MSG_STEP_DATA,				m_msgStepData);
	DDX_Control(pDX, IDC_MSG_PART_NO,				m_msgPartNo);
	DDX_Control(pDX, IDC_MSG_PART_NO_DATA,			m_msgPartNoData);
	DDX_Control(pDX, IDC_MSG_LOT_NO_M,				m_msgLotNoM);
	DDX_Control(pDX, IDC_MSG_LOT_NO_M_DATA,			m_msgLotNoMData);
	DDX_Control(pDX, IDC_MSG_QTY_T,					m_msgQtyT);
	DDX_Control(pDX, IDC_MSG_QTY_T_DATA,			m_msgQtyTData);
	DDX_Control(pDX, IDC_MSG_LOT_TYPE,				m_msgLotType);
	DDX_Control(pDX, IDC_MSG_LOT_TYPE_DATA,			m_msgLotTypeData);
	DDX_Control(pDX, IDC_MSG_PKG_CODE,				m_msgPkgCode);
	DDX_Control(pDX, IDC_MSG_PKG_CODE_DATA,			m_msgPkgCodeData);
	DDX_Control(pDX, IDC_MSG_CNT,					m_msgCnt);
	DDX_Control(pDX, IDC_MSG_CNT_DATA,				m_msgCntData);
	DDX_Control(pDX, IDC_MSG_LOT_FLAG,				m_msgFlag);
	DDX_Control(pDX, IDC_MSG_LOT_FLAG_DATA,			m_msgFlagData);
	DDX_Control(pDX, IDC_MSG_REJECT,				m_msgReject);
	DDX_Control(pDX, IDC_MSG_REJECT_DATA,			m_msgRejectData);
	DDX_Control(pDX, IDC_MSG_BIN,					m_msgBin);
	DDX_Control(pDX, IDC_MSG_BIN_DATA,				m_msgBinData);
	DDX_Control(pDX, IDC_MSG_TRAY_QTY,				m_msgTrayQty);
	DDX_Control(pDX, IDC_MSG_TRAY_QTY_DATA,			m_msgTrayQtyData);
	DDX_Control(pDX, IDC_MSG_NONE_1,				m_msgNone1);
	DDX_Control(pDX, IDC_MSG_NONE_1_DATA,			m_msgNone1Data);
	DDX_Control(pDX, IDC_MSG_NONE_2,				m_msgNone2);
	DDX_Control(pDX, IDC_MSG_NONE_2_DATA,			m_msgNone2Data);
}


BEGIN_MESSAGE_MAP(CWorkRfid, CDialog)
	ON_BN_CLICKED(IDC_BTN_CLIENT_IP_RFID, &CWorkRfid::OnBnClickedBtnClientIpRfid)
	ON_BN_CLICKED(IDC_BTN_CLIENT_PORT_RFID, &CWorkRfid::OnBnClickedBtnClientPortRfid)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_RFID_READ, &CWorkRfid::OnBnClickedButtonRfidRead)
	ON_BN_CLICKED(IDC_BUTTON_RFID_WRITE, &CWorkRfid::OnBnClickedButtonRfidWrite)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BTN_RELOAD, &CWorkRfid::OnBnClickedBtnReload)
	ON_BN_CLICKED(IDC_BTN_APPLY, &CWorkRfid::OnBnClickedBtnApply)
	ON_STN_CLICKED(IDC_MSG_CARRIER_ID_DATA, &CWorkRfid::OnStnClickedMsgCarrierIdData)
	ON_STN_CLICKED(IDC_MSG_EQP_NO_DATA, &CWorkRfid::OnStnClickedMsgEqpNoData)
	ON_STN_CLICKED(IDC_MSG_QTY_DATA, &CWorkRfid::OnStnClickedMsgQtyData)
	ON_STN_CLICKED(IDC_MSG_LOT_NO_J_DATA, &CWorkRfid::OnStnClickedMsgLotNoJData)
	ON_STN_CLICKED(IDC_MSG_STEP_DATA, &CWorkRfid::OnStnClickedMsgStepData)
	ON_STN_CLICKED(IDC_MSG_PART_NO_DATA, &CWorkRfid::OnStnClickedMsgPartNoData)
	ON_STN_CLICKED(IDC_MSG_LOT_NO_M_DATA, &CWorkRfid::OnStnClickedMsgLotNoMData)
	ON_STN_CLICKED(IDC_MSG_QTY_T_DATA, &CWorkRfid::OnStnClickedMsgQtyTData)
	ON_STN_CLICKED(IDC_MSG_LOT_TYPE_DATA, &CWorkRfid::OnStnClickedMsgLotTypeData)
	ON_STN_CLICKED(IDC_MSG_PKG_CODE_DATA, &CWorkRfid::OnStnClickedMsgPkgCodeData)
	ON_STN_CLICKED(IDC_MSG_CNT_DATA, &CWorkRfid::OnStnClickedMsgCntData)
	ON_STN_CLICKED(IDC_MSG_LOT_FLAG_DATA, &CWorkRfid::OnStnClickedMsgLotFlagData)
	ON_STN_CLICKED(IDC_MSG_REJECT_DATA, &CWorkRfid::OnStnClickedMsgRejectData)
	ON_STN_CLICKED(IDC_MSG_BIN_DATA, &CWorkRfid::OnStnClickedMsgBinData)
	ON_STN_CLICKED(IDC_MSG_TRAY_QTY_DATA, &CWorkRfid::OnStnClickedMsgTrayQtyData)
	ON_STN_CLICKED(IDC_MSG_NONE_1_DATA, &CWorkRfid::OnStnClickedMsgNone1Data)
	ON_STN_CLICKED(IDC_MSG_NONE_2_DATA, &CWorkRfid::OnStnClickedMsgNone2Data)
END_MESSAGE_MAP()


// CWorkRfid 메시지 처리기입니다.


BOOL CWorkRfid::DestroyWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CDialog::DestroyWindow();
}

BOOL CWorkRfid::OnInitDialog()
{
	CDialog::OnInitDialog();

	OnDataInit();

	OnInitGroupBox();
	OnInitLabel();
	OnInitButton();
	OnInterface_Display();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CWorkRfid::OnInitGroupBox()
{
	m_groupRfid.SetFont(clsFunc.OnLogFont(16));
	m_groupRfid.SetCatptionTextColor(BLUE_C);
	m_groupRfid.SetBorderColor(ORANGE_C);
	m_groupRfid.SetBackgroundColor(WINDOW_UP1);
	m_groupRfid.SetFontBold(TRUE);

	m_groupRfidInfo.SetFont(clsFunc.OnLogFont(16));
	m_groupRfidInfo.SetCatptionTextColor(BLUE_C);
	m_groupRfidInfo.SetBorderColor(ORANGE_C);
	m_groupRfidInfo.SetBackgroundColor(WINDOW_UP1);
	m_groupRfidInfo.SetFontBold(TRUE);
}

void CWorkRfid::OnInitLabel()
{
	m_msgClientIpRfid.SetFont(clsFunc.m_pFont[1]);
	m_msgClientIpRfid.SetWindowText(_T("IP"));
	m_msgClientIpRfid.SetCenterText();
	m_msgClientIpRfid.SetColor(WHITE_C);
	m_msgClientIpRfid.SetGradientColor(ORANGE_C);
	m_msgClientIpRfid.SetTextColor(BLACK_C);

	m_msgClientPortRfid.SetFont(clsFunc.m_pFont[1]);
	m_msgClientPortRfid.SetWindowText(_T("Port"));
	m_msgClientPortRfid.SetCenterText();
	m_msgClientPortRfid.SetColor(WHITE_C);
	m_msgClientPortRfid.SetGradientColor(ORANGE_C);
	m_msgClientPortRfid.SetTextColor(BLACK_C);

	m_msgCarrier.SetFont(clsFunc.m_pFont[3]);
	m_msgCarrier.SetWindowText(_T("CARRIER ID"));
	m_msgCarrier.SetCenterText();
	m_msgCarrier.SetColor(YELLOW_L);
	m_msgCarrier.SetGradientColor(YELLOW_L);
	m_msgCarrier.SetTextColor(BLACK_C);

	m_msgCarrierData.SetFont(clsFunc.m_pFont[3]);
	m_msgCarrierData.SetWindowText(_T("-"));
	m_msgCarrierData.SetCenterText();
	m_msgCarrierData.SetColor(TEXT_BC);
	m_msgCarrierData.SetGradientColor(TEXT_BC);
	m_msgCarrierData.SetTextColor(BLACK_C);

	m_msgEqpNo.SetFont(clsFunc.m_pFont[3]);
	m_msgEqpNo.SetWindowText(_T("EQP ID"));
	m_msgEqpNo.SetCenterText();
	m_msgEqpNo.SetColor(YELLOW_L);
	m_msgEqpNo.SetGradientColor(YELLOW_L);
	m_msgEqpNo.SetTextColor(BLACK_C);

	m_msgEqpNoData.SetFont(clsFunc.m_pFont[3]);
	m_msgEqpNoData.SetWindowText(_T("-"));
	m_msgEqpNoData.SetCenterText();
	m_msgEqpNoData.SetColor(TEXT_BC);
	m_msgEqpNoData.SetGradientColor(TEXT_BC);
	m_msgEqpNoData.SetTextColor(BLACK_C);

	m_msgQty.SetFont(clsFunc.m_pFont[3]);
	m_msgQty.SetWindowText(_T("QTY"));
	m_msgQty.SetCenterText();
	m_msgQty.SetColor(YELLOW_L);
	m_msgQty.SetGradientColor(YELLOW_L);
	m_msgQty.SetTextColor(BLACK_C);

	m_msgQtyData.SetFont(clsFunc.m_pFont[3]);
	m_msgQtyData.SetWindowText(_T("-"));
	m_msgQtyData.SetCenterText();
	m_msgQtyData.SetColor(TEXT_BC);
	m_msgQtyData.SetGradientColor(TEXT_BC);
	m_msgQtyData.SetTextColor(BLACK_C);

	m_msgLotNoJ.SetFont(clsFunc.m_pFont[3]);
	m_msgLotNoJ.SetWindowText(_T("JA Lot No"));
	m_msgLotNoJ.SetCenterText();
	m_msgLotNoJ.SetColor(YELLOW_L);
	m_msgLotNoJ.SetGradientColor(YELLOW_L);
	m_msgLotNoJ.SetTextColor(BLACK_C);

	m_msgLotNoJData.SetFont(clsFunc.m_pFont[3]);
	m_msgLotNoJData.SetWindowText(_T("-"));
	m_msgLotNoJData.SetCenterText();
	m_msgLotNoJData.SetColor(TEXT_BC);
	m_msgLotNoJData.SetGradientColor(TEXT_BC);
	m_msgLotNoJData.SetTextColor(BLACK_C);

	m_msgStep.SetFont(clsFunc.m_pFont[3]);
	m_msgStep.SetWindowText(_T("STEP"));
	m_msgStep.SetCenterText();
	m_msgStep.SetColor(YELLOW_L);
	m_msgStep.SetGradientColor(YELLOW_L);
	m_msgStep.SetTextColor(BLACK_C);

	m_msgStepData.SetFont(clsFunc.m_pFont[3]);
	m_msgStepData.SetWindowText(_T("-"));
	m_msgStepData.SetCenterText();
	m_msgStepData.SetColor(TEXT_BC);
	m_msgStepData.SetGradientColor(TEXT_BC);
	m_msgStepData.SetTextColor(BLACK_C);

	m_msgPartNo.SetFont(clsFunc.m_pFont[3]);
	m_msgPartNo.SetWindowText(_T("PART NO"));
	m_msgPartNo.SetCenterText();
	m_msgPartNo.SetColor(YELLOW_L);
	m_msgPartNo.SetGradientColor(YELLOW_L);
	m_msgPartNo.SetTextColor(BLACK_C);

	m_msgPartNoData.SetFont(clsFunc.m_pFont[3]);
	m_msgPartNoData.SetWindowText(_T("-"));
	m_msgPartNoData.SetCenterText();
	m_msgPartNoData.SetColor(TEXT_BC);
	m_msgPartNoData.SetGradientColor(TEXT_BC);
	m_msgPartNoData.SetTextColor(BLACK_C);

	m_msgLotNoM.SetFont(clsFunc.m_pFont[3]);
	m_msgLotNoM.SetWindowText(_T("MO LOT NO"));
	m_msgLotNoM.SetCenterText();
	m_msgLotNoM.SetColor(YELLOW_L);
	m_msgLotNoM.SetGradientColor(YELLOW_L);
	m_msgLotNoM.SetTextColor(BLACK_C);

	m_msgLotNoMData.SetFont(clsFunc.m_pFont[3]);
	m_msgLotNoMData.SetWindowText(_T("-"));
	m_msgLotNoMData.SetCenterText();
	m_msgLotNoMData.SetColor(TEXT_BC);
	m_msgLotNoMData.SetGradientColor(TEXT_BC);
	m_msgLotNoMData.SetTextColor(BLACK_C);

	m_msgQtyT.SetFont(clsFunc.m_pFont[3]);
	m_msgQtyT.SetWindowText(_T("QTY T"));
	m_msgQtyT.SetCenterText();
	m_msgQtyT.SetColor(YELLOW_L);
	m_msgQtyT.SetGradientColor(YELLOW_L);
	m_msgQtyT.SetTextColor(BLACK_C);

	m_msgQtyTData.SetFont(clsFunc.m_pFont[3]);
	m_msgQtyTData.SetWindowText(_T("-"));
	m_msgQtyTData.SetCenterText();
	m_msgQtyTData.SetColor(TEXT_BC);
	m_msgQtyTData.SetGradientColor(TEXT_BC);
	m_msgQtyTData.SetTextColor(BLACK_C);

	m_msgLotType.SetFont(clsFunc.m_pFont[3]);
	m_msgLotType.SetWindowText(_T("LOT TYPE"));
	m_msgLotType.SetCenterText();
	m_msgLotType.SetColor(YELLOW_L);
	m_msgLotType.SetGradientColor(YELLOW_L);
	m_msgLotType.SetTextColor(BLACK_C);

	m_msgLotTypeData.SetFont(clsFunc.m_pFont[3]);
	m_msgLotTypeData.SetWindowText(_T("-"));
	m_msgLotTypeData.SetCenterText();
	m_msgLotTypeData.SetColor(TEXT_BC);
	m_msgLotTypeData.SetGradientColor(TEXT_BC);
	m_msgLotTypeData.SetTextColor(BLACK_C);

	m_msgPkgCode.SetFont(clsFunc.m_pFont[3]);
	m_msgPkgCode.SetWindowText(_T("PKG CODE"));
	m_msgPkgCode.SetCenterText();
	m_msgPkgCode.SetColor(YELLOW_L);
	m_msgPkgCode.SetGradientColor(YELLOW_L);
	m_msgPkgCode.SetTextColor(BLACK_C);

	m_msgPkgCodeData.SetFont(clsFunc.m_pFont[3]);
	m_msgPkgCodeData.SetWindowText(_T("-"));
	m_msgPkgCodeData.SetCenterText();
	m_msgPkgCodeData.SetColor(TEXT_BC);
	m_msgPkgCodeData.SetGradientColor(TEXT_BC);
	m_msgPkgCodeData.SetTextColor(BLACK_C);

	m_msgCnt.SetFont(clsFunc.m_pFont[3]);
	m_msgCnt.SetWindowText(_T("CNT"));
	m_msgCnt.SetCenterText();
	m_msgCnt.SetColor(YELLOW_L);
	m_msgCnt.SetGradientColor(YELLOW_L);
	m_msgCnt.SetTextColor(BLACK_C);

	m_msgCntData.SetFont(clsFunc.m_pFont[3]);
	m_msgCntData.SetWindowText(_T("-"));
	m_msgCntData.SetCenterText();
	m_msgCntData.SetColor(TEXT_BC);
	m_msgCntData.SetGradientColor(TEXT_BC);
	m_msgCntData.SetTextColor(BLACK_C);

	m_msgFlag.SetFont(clsFunc.m_pFont[3]);
	m_msgFlag.SetWindowText(_T("FLAG"));
	m_msgFlag.SetCenterText();
	m_msgFlag.SetColor(YELLOW_L);
	m_msgFlag.SetGradientColor(YELLOW_L);
	m_msgFlag.SetTextColor(BLACK_C);

	m_msgFlagData.SetFont(clsFunc.m_pFont[3]);
	m_msgFlagData.SetWindowText(_T("-"));
	m_msgFlagData.SetCenterText();
	m_msgFlagData.SetColor(TEXT_BC);
	m_msgFlagData.SetGradientColor(TEXT_BC);
	m_msgFlagData.SetTextColor(BLACK_C);

	m_msgReject.SetFont(clsFunc.m_pFont[3]);
	m_msgReject.SetWindowText(_T("REJECT"));
	m_msgReject.SetCenterText();
	m_msgReject.SetColor(YELLOW_L);
	m_msgReject.SetGradientColor(YELLOW_L);
	m_msgReject.SetTextColor(BLACK_C);

	m_msgRejectData.SetFont(clsFunc.m_pFont[3]);
	m_msgRejectData.SetWindowText(_T("-"));
	m_msgRejectData.SetCenterText();
	m_msgRejectData.SetColor(TEXT_BC);
	m_msgRejectData.SetGradientColor(TEXT_BC);
	m_msgRejectData.SetTextColor(BLACK_C);

	m_msgBin.SetFont(clsFunc.m_pFont[3]);
	m_msgBin.SetWindowText(_T("BIN"));
	m_msgBin.SetCenterText();
	m_msgBin.SetColor(YELLOW_L);
	m_msgBin.SetGradientColor(YELLOW_L);
	m_msgBin.SetTextColor(BLACK_C);

	m_msgBinData.SetFont(clsFunc.m_pFont[3]);
	m_msgBinData.SetWindowText(_T("-"));
	m_msgBinData.SetCenterText();
	m_msgBinData.SetColor(TEXT_BC);
	m_msgBinData.SetGradientColor(TEXT_BC);
	m_msgBinData.SetTextColor(BLACK_C);

	m_msgTrayQty.SetFont(clsFunc.m_pFont[3]);
	m_msgTrayQty.SetWindowText(_T("TRAY QTY"));
	m_msgTrayQty.SetCenterText();
	m_msgTrayQty.SetColor(YELLOW_L);
	m_msgTrayQty.SetGradientColor(YELLOW_L);
	m_msgTrayQty.SetTextColor(BLACK_C);

	m_msgTrayQtyData.SetFont(clsFunc.m_pFont[3]);
	m_msgTrayQtyData.SetWindowText(_T("-"));
	m_msgTrayQtyData.SetCenterText();
	m_msgTrayQtyData.SetColor(TEXT_BC);
	m_msgTrayQtyData.SetGradientColor(TEXT_BC);
	m_msgTrayQtyData.SetTextColor(BLACK_C);

	m_msgNone1.SetFont(clsFunc.m_pFont[3]);
	m_msgNone1.SetWindowText(_T("NONE 1"));
	m_msgNone1.SetCenterText();
	m_msgNone1.SetColor(YELLOW_L);
	m_msgNone1.SetGradientColor(YELLOW_L);
	m_msgNone1.SetTextColor(BLACK_C);

	m_msgNone1Data.SetFont(clsFunc.m_pFont[3]);
	m_msgNone1Data.SetWindowText(_T("-"));
	m_msgNone1Data.SetCenterText();
	m_msgNone1Data.SetColor(TEXT_BC);
	m_msgNone1Data.SetGradientColor(TEXT_BC);
	m_msgNone1Data.SetTextColor(BLACK_C);

	m_msgNone2.SetFont(clsFunc.m_pFont[3]);
	m_msgNone2.SetWindowText(_T("NONE 2"));
	m_msgNone2.SetCenterText();
	m_msgNone2.SetColor(YELLOW_L);
	m_msgNone2.SetGradientColor(YELLOW_L);
	m_msgNone2.SetTextColor(BLACK_C);

	m_msgNone2Data.SetFont(clsFunc.m_pFont[3]);
	m_msgNone2Data.SetWindowText(_T("-"));
	m_msgNone2Data.SetCenterText();
	m_msgNone2Data.SetColor(TEXT_BC);
	m_msgNone2Data.SetGradientColor(TEXT_BC);
	m_msgNone2Data.SetTextColor(BLACK_C);
}

void CWorkRfid::OnInitButton()
{
	m_btnClientIpRfid.SetBitmaps(IDC_BTN_CLIENT_IP_RFID, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
	m_btnClientIpRfid.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	m_btnClientIpRfid.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	m_btnClientIpRfid.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	m_btnClientIpRfid.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnClientIpRfid.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnClientIpRfid.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnClientIpRfid.SetFont(clsFunc.m_pFont[1]);
	m_btnClientIpRfid.SetTooltipText(_T("Rfid Client IP Address"));

	m_btnClientPortRfid.SetBitmaps(IDC_BTN_CLIENT_PORT_RFID, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
	m_btnClientPortRfid.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	m_btnClientPortRfid.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	m_btnClientPortRfid.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	m_btnClientPortRfid.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnClientPortRfid.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnClientPortRfid.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnClientPortRfid.SetFont(clsFunc.m_pFont[1]);
	m_btnClientPortRfid.SetTooltipText(_T("Rfid Client Port"));

	m_btnRfidRead.SetBitmaps(IDC_BUTTON_RFID_READ, IDB_BTN_DOWNLOAD_DN, WINDOW_DN1, IDB_BTN_DOWNLOAD_UP, WINDOW_UP1);
	m_btnRfidRead.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	m_btnRfidRead.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	m_btnRfidRead.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	m_btnRfidRead.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnRfidRead.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnRfidRead.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnRfidRead.SetFont(clsFunc.m_pFont[2]);
	m_btnRfidRead.SetTooltipText(_T("Rfid Read"));

	m_btnRfidWrite.SetBitmaps(IDC_BUTTON_RFID_WRITE, IDB_BTN_UPLOAD_DN, WINDOW_DN1, IDB_BTN_UPLOAD_UP, WINDOW_UP1);
	m_btnRfidWrite.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	m_btnRfidWrite.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	m_btnRfidWrite.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	m_btnRfidWrite.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnRfidWrite.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnRfidWrite.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnRfidWrite.SetFont(clsFunc.m_pFont[2]);
	m_btnRfidWrite.SetTooltipText(_T("Rfid Write"));

	m_btnApply.SetBitmaps(IDC_BTN_APPLY, IDB_BITMAP_APPLY_DN1, WINDOW_DN1, IDB_BITMAP_APPLY_UP1, WINDOW_UP1);
	m_btnApply.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	m_btnApply.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	m_btnApply.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	m_btnApply.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnApply.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnApply.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnApply.SetFont(clsFunc.m_pFont[2]);
	m_btnApply.SetTooltipText(_T("RFID Data Apply"));

	m_btnReLoad.SetBitmaps(IDC_BTN_RELOAD, IDB_BITMAP_RELOAD_DN1, WINDOW_DN1, IDB_BITMAP_RELOAD_UP1, WINDOW_UP1);
	m_btnReLoad.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	m_btnReLoad.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	m_btnReLoad.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	m_btnReLoad.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnReLoad.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnReLoad.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnReLoad.SetFont(clsFunc.m_pFont[2]);
	m_btnReLoad.SetTooltipText(_T("RFID Data ReLoad"));
}

void CWorkRfid::OnInterface_IP(CString str_ip)
{
	int nDot1, nDot2, nDot3, nStart;

	nStart = 0;
	nDot1 = str_ip.Find('.');
	nDot2 = str_ip.Find('.', nDot1 + 1);
	nDot3 = str_ip.Find('.', nDot2 + 1);

	m_byIp[0]	= _wtoi(str_ip.Mid(nStart, nDot1 - nStart));
	nStart		= nDot1 + 1;
	m_byIp[1]	= _wtoi(str_ip.Mid(nStart, nDot2 - nStart));
	nStart		= nDot2 + 1;
	m_byIp[2]	= _wtoi(str_ip.Mid(nStart, nDot3 - nStart));
	nStart		= nDot3 + 1;
	m_byIp[3]	= _wtoi(str_ip.Mid(nStart));
}

void CWorkRfid::OnInterface_Display()
{
	CString strTemp;

	OnInterface_IP(m_strRfidIp[1]);
	
	m_clientIpRfid.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	
	strTemp.Format(_T("%d"), m_nRfidPort[1]);
	
	m_editClientPortRfid.SetWindowText(strTemp);
}

int CWorkRfid::OnRfidRead()
{
	switch (m_nRfidStep)
	{
		case 0:
			if (st_basic_info.nModeRfid == YES)
			{
				m_nRetry = 0;
				m_nRfidStep = 100;
			}
			else
			{
				m_nRfidStep = 0;

				return RET_GOOD;
			}
			break;

		case 100:
			//if (clsRfid.OnMakeReadCodeAll(1))
			if (clsRfid.OnMakeReadCodeAll(2))
			{
				m_dwWaitTime[0] = GetTickCount();

				m_nRfidStep = 1000;
			}
			else 
			{
				m_dwWaitTime[0] = GetTickCount();

				m_nRfidStep = 200;
			}
			break;

		case 200:
			m_dwWaitTime[1] = GetTickCount();
			m_dwWaitTime[2] = m_dwWaitTime[1] - m_dwWaitTime[0];

			if (m_dwWaitTime[2] <= 0)
			{
				m_dwWaitTime[0] = GetTickCount();
				break;
			}

			if (m_dwWaitTime[2] > (DWORD)5000)
			{
				m_nRetry++;
				if (m_nRetry >= 3)
				{
					m_nRfidStep = 0;

					return RET_ERROR;
				}
				else
				{
					m_nRfidStep = 100;
				}
			}
			break;

		case 1000:
			// 데이터 송신 결과 확인 작업
			if (clsRfid.m_nCommBin == BIN_GOODBIN)
			{
				OnDisplayData();
				m_nRfidStep = 0;

				return RET_GOOD;
			}
			else if (clsRfid.m_nCommBin == BIN_REJECTBIN)
			{
				m_nRfidStep = 0;

				return RET_ERROR;
			}
			else
			{
				m_dwWaitTime[1] = GetTickCount();
				m_dwWaitTime[2] = m_dwWaitTime[1] - m_dwWaitTime[0];

				if (m_dwWaitTime[2] <= 0)
				{
					m_dwWaitTime[0] = GetTickCount();
					break;
				}

				if (m_dwWaitTime[2] > (DWORD)5000)
				{
					m_nRfidStep = 0;

					return RET_ERROR;
				}
			}
			break;
	}
	return RET_PROCEED;
}

int CWorkRfid::OnRfidWrite()
{
	switch (m_nRfidStep)
	{
		case 0:
			if (st_basic_info.nModeRfid == YES)
			{
				m_nRetry = 0;
				m_nRfidStep = 100;
			}
			else
			{
				m_nRfidStep = 0;

				return RET_GOOD;
			}
			break;

		case 100:
			//if (clsRfid.OnMakeWriteCodeAll(1, m_strRfidData))
			if (clsRfid.OnMakeWriteCodeAll(2, m_strRfidData))
			{
				m_dwWaitTime[0] = GetTickCount();
				m_nRfidStep = 1000;
			}
			else 
			{
				m_dwWaitTime[0] = GetTickCount();

				m_nRfidStep = 200;
			}
			break;

		case 200:
			m_dwWaitTime[1] = GetTickCount();
			m_dwWaitTime[2] = m_dwWaitTime[1] - m_dwWaitTime[0];

			if (m_dwWaitTime[2] <= 0)
			{
				m_dwWaitTime[0] = GetTickCount();
				break;
			}

			if (m_dwWaitTime[2] > (DWORD)5000)
			{
				m_nRetry++;
				if (m_nRetry >= 3)
				{
					m_nRfidStep = 0;

					return RET_ERROR;
				}
				else
				{
					m_nRfidStep = 100;
				}
			}
			break;

		case 1000:
			// 데이터 송신 결과 확인 작업
			if (clsRfid.m_nCommBin == BIN_GOODBIN)
			{
				m_nRfidStep = 0;

				return RET_GOOD;
			}
			else if (clsRfid.m_nCommBin == BIN_REJECTBIN)
			{
				m_nRfidStep = 0;

				return RET_ERROR;
			}
			else
			{
				m_dwWaitTime[1] = GetTickCount();
				m_dwWaitTime[2] = m_dwWaitTime[1] - m_dwWaitTime[0];

				if (m_dwWaitTime[2] <= 0)
				{
					m_dwWaitTime[0] = GetTickCount();
					break;
				}

				if (m_dwWaitTime[2] > (DWORD)5000)
				{
					m_nRfidStep = 0;

					return RET_ERROR;
				}
			}
			break;
	}
/*	switch (m_nRfidStep)
	{
		case 0:
			if (st_client_info[RFID_NETWORK].nConnect == NO)
			{
				// 상위서버와 연결중이면
				m_nRfidStep = 100;
			}
			else
			{
				// 상위서버와 연결이 안된 상태이면
				m_nRfidStep = 300;
			}
			break;

		case 100:
			// 상위 서버에 클라이언트 연결 요청
			m_dwWaitTime[0] = GetTickCount();

			//::PostMessage(st_handler_info.hWnd, WM_CLIENT_MSG + RFID_NETWORK, CLIENT_CONNECT, 0);
			::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + RFID_NETWORK, CLIENT_CONNECT, 0);

			m_nRfidStep = 200;
			break;

		case 200:
			if (st_client_info[RFID_NETWORK].nConnect == YES)
			{
				// 상위서버와 클라이언트가 연결 되었으면 
				m_nRfidStep = 300;
			}
			else
			{
				m_dwWaitTime[1] = GetTickCount();
				m_dwWaitTime[2] = m_dwWaitTime[1] - m_dwWaitTime[0];

				if (m_dwWaitTime[2] <= 0)
				{
					m_dwWaitTime[0] = GetTickCount();
					break;
				}

				if (m_dwWaitTime[2] > (DWORD)5000)
				{
					// 5초동안 연결되지 않으면 
					m_nRfidStep = 0;

					return RET_ERROR;
				}
			}
			break;

		case 300:
			m_dwWaitTime[0]								= GetTickCount();

			clsRfid.m_nRfidReq							= BIN_NOTUESD;
			st_client_info[RFID_NETWORK].strSend		= clsRfid.OnMakeWriteCodeAll(1, m_strRfidData);
			//			st_client_info[RFID_NETWORK].strSend		= clsRfid.OnMakeWriteCode(1, 1, _T("AAAA"));

			//::PostMessage(st_handler_info.hWnd, WM_CLIENT_MSG + RFID_NETWORK, CLIENT_SEND, 0);
			::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + RFID_NETWORK, CLIENT_SEND, 0);

			m_nRfidStep = 400;
			break;

		case 400:
			// 데이터 송신 결과 확인 작업
			if (clsRfid.m_nRfidReq == BIN_GOODBIN)
			{
				m_nRfidStep = 0;

				return RET_GOOD;
			}
			else if (clsRfid.m_nRfidReq == BIN_REJECTBIN)
			{
				m_nRfidStep = 0;

				return RET_ERROR;
			}
			else
			{
				m_dwWaitTime[1] = GetTickCount();
				m_dwWaitTime[2] = m_dwWaitTime[1] - m_dwWaitTime[0];

				if (m_dwWaitTime[2] <= 0)
				{
					m_dwWaitTime[0] = GetTickCount();
					break;
				}

				if (m_dwWaitTime[2] > (DWORD)10000)
				{
					m_nRfidStep = 0;

					return RET_ERROR;
				}
			}
			break;
	}*/
	return RET_PROCEED;
}

void CWorkRfid::OnDataInit()
{
//	m_strRfidIp[1]	=	st_client_info[RFID_NETWORK].strIp;
//	m_nRfidPort[1]	=	st_client_info[RFID_NETWORK].nPort;
}

void CWorkRfid::OnDataApply()
{
//	st_client_info[RFID_NETWORK].strIp	=	m_strRfidIp[1];
//	st_client_info[RFID_NETWORK].nPort	=	m_nRfidPort[1];
}

void CWorkRfid::OnDataBackup()
{
	m_strRfidIp[0]	=	m_strRfidIp[1];
	m_nRfidPort[0]	=	m_nRfidPort[1];

}

void CWorkRfid::OnDataRecovery()
{
	m_strRfidIp[1]	=	m_strRfidIp[0];
	m_nRfidPort[1]	=	m_nRfidPort[0];
}

int CWorkRfid::OnDataComparison()
{
	if (m_strRfidIp[1] != m_strRfidIp[0]) return RET_ERROR;
	if (m_nRfidPort[1] != m_nRfidPort[0]) return RET_ERROR;

	return RET_GOOD;
}

void CWorkRfid::OnDataHistoryLog()
{
	CString strMsg;

	if (m_strRfidIp[1] != m_strRfidIp[0])
	{
		strMsg.Format(_T("[RFID] Ip Change %s -> %s"), m_strRfidIp[0], m_strRfidIp[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	if (m_nRfidPort[1] != m_nRfidPort[0])
	{
		strMsg.Format(_T("[RFID] Port Change %d -> %d"), m_nRfidPort[0], m_nRfidPort[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}
}

void CWorkRfid::OnBnClickedBtnClientIpRfid()
{
	CDialog_KeyBoard dlgKeyBoard;
	CString strTmp;

	dlgKeyBoard.m_strKeyBoardVal = _T("");

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		// TODO: Add your control notification handler code here
		m_strRfidIp[1] = dlgKeyBoard.m_strKeyBoardVal;

		m_strRfidIp[1].MakeUpper();
		m_strRfidIp[1].TrimLeft(' ');               
		m_strRfidIp[1].TrimRight(' ');

		OnInterface_IP(m_strRfidIp[1]);

		m_clientIpRfid.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);

//		::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + RFID_NETWORK, CLIENT_CLOSE, 0);
	}
}

void CWorkRfid::OnBnClickedBtnClientPortRfid()
{
	CDialog_KeyPad dlgKeyPad;
	CString strTmp;

	dlgKeyPad.m_nKeypadMode			= 0;
	dlgKeyPad.m_strKeypadLowLimit	= _T("0");
	dlgKeyPad.m_strKeypadHighLimit	= _T("100000");
	dlgKeyPad.m_strKeypadVal.Format(_T("%d"),m_nRfidPort[1]);

	if (dlgKeyPad.DoModal() == IDOK)
	{
		strTmp = dlgKeyPad.m_strNewVal;

		m_nRfidPort[1] = _wtoi(strTmp);	
		m_editClientPortRfid.SetWindowText(strTmp);

//		::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + RFID_NETWORK, CLIENT_CLOSE, 0);
	}
}




void CWorkRfid::OnTimer(UINT_PTR nIDEvent)
{
	int nRet;
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch(nIDEvent)
	{
	case TM_RFID_READ:
		nRet = OnRfidRead();

		if (nRet == RET_GOOD)
		{
			KillTimer(TM_RFID_READ);

			if (st_handler_info.nMenuLock != FALSE)
			{
				st_handler_info.nMenuLock = FALSE;
			}
		}
		else if (nRet == RET_ERROR)
		{
			KillTimer(TM_RFID_READ);

			if (st_handler_info.nMenuLock != FALSE)
			{
				st_handler_info.nMenuLock = FALSE;
			}
		}
		break;

	case TM_RFID_WRITE:
		nRet = OnRfidWrite();

		if (nRet == RET_GOOD)
		{
			KillTimer(TM_RFID_WRITE);

			if (st_handler_info.nMenuLock != FALSE)
			{
				st_handler_info.nMenuLock = FALSE;
			}
		}
		else if (nRet == RET_ERROR)
		{
			KillTimer(TM_RFID_WRITE);

			if (st_handler_info.nMenuLock != FALSE)
			{
				st_handler_info.nMenuLock = FALSE;
			}
		}
		break;
	}
	CDialog::OnTimer(nIDEvent);
}


void CWorkRfid::OnBnClickedButtonRfidRead()
{
	if (st_handler_info.nMenuLock) return;

	st_handler_info.nMenuLock = TRUE;

	m_dwWaitTime[0] = GetTickCount();
	m_nRfidStep		= 0;

	SetTimer(TM_RFID_READ, 100, NULL);
}


void CWorkRfid::OnBnClickedButtonRfidWrite()
{
	if (st_handler_info.nMenuLock) return;

	st_handler_info.nMenuLock = TRUE;

	m_dwWaitTime[0] = GetTickCount();
	m_nRfidStep		= 0;

	SetTimer(TM_RFID_WRITE, 100, NULL);
}


BOOL CWorkRfid::OnEraseBkgnd(CDC* pDC)
{
	CDC  memDC;                     // 메모리 DC
	CBitmap * pOldBitmap = NULL;    // m_BackBitmap으로 새로운 그림을 DC에 그릴 때, 이전 그려졌던 DC(즉, Bitmap)을 저장.
	BITMAP bitmapInfo;              // 그림의 정보(m_BackBitmap)
	st_handler_info.m_BackDlgBitmap.GetBitmap(&bitmapInfo); // Bitmap 크기 구함.

	memDC.CreateCompatibleDC(pDC);  // 메모리 DC 생성
	pOldBitmap = memDC.SelectObject(&st_handler_info.m_BackDlgBitmap);
	pDC->BitBlt(0, 0, bitmapInfo.bmWidth, bitmapInfo.bmHeight, &memDC, 0, 0, SRCCOPY);   // 메모리 DC에 그림을 그림

	memDC.SelectObject(pOldBitmap);
	memDC.DeleteDC();
	return true; // 흰 부러쉬가 아닌 배경을 그려야 하기 때문에 true
}


void CWorkRfid::OnBnClickedBtnReload()
{
	OnDataRecovery();
	OnInterface_Display();
}


void CWorkRfid::OnBnClickedBtnApply()
{
	CDialog_Message		dlgMsg;
	int					nResponse;

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
}


void CWorkRfid::OnStnClickedMsgCarrierIdData()
{
	CDialog_KeyBoard dlgKeyBoard;
	CString	strTemp;

	dlgKeyBoard.m_strKeyBoardVal = clsRfid.m_strRfidData[RF_CARRIER_ID];

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		strTemp = dlgKeyBoard.m_strKeyBoardVal;

		if (strTemp.GetLength() > 8)
		{
			m_strRfidData[RF_CARRIER_ID] = strTemp.Mid(0, 8);
		}
		else
		{
			m_strRfidData[RF_CARRIER_ID] = strTemp;
		}
		m_msgCarrierData.SetWindowText(m_strRfidData[RF_CARRIER_ID]);
	}
}


void CWorkRfid::OnStnClickedMsgEqpNoData()
{
	CDialog_KeyBoard dlgKeyBoard;
	CString strTemp;

	dlgKeyBoard.m_strKeyBoardVal = clsRfid.m_strRfidData[RF_EQP_ID];

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		strTemp = dlgKeyBoard.m_strKeyBoardVal;

		if (strTemp.GetLength() > 8)
		{
			m_strRfidData[RF_EQP_ID] = strTemp.Mid(0, 8);
		}
		else
		{
			m_strRfidData[RF_EQP_ID] = strTemp;
		}

		m_msgEqpNoData.SetWindowText(m_strRfidData[RF_EQP_ID]);
	}
}


void CWorkRfid::OnStnClickedMsgQtyData()
{
	CDialog_KeyBoard dlgKeyBoard;
	CString strTemp;

	dlgKeyBoard.m_strKeyBoardVal = clsRfid.m_strRfidData[RF_QTY];

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		strTemp = dlgKeyBoard.m_strKeyBoardVal;

		if (strTemp.GetLength() > 6)
		{
			m_strRfidData[RF_QTY] = strTemp.Mid(0, 6);
		}
		else
		{
			m_strRfidData[RF_QTY] = strTemp;
		}

		m_msgQtyData.SetWindowText(m_strRfidData[RF_QTY]);
	}
}


void CWorkRfid::OnStnClickedMsgLotNoJData()
{
	CDialog_KeyBoard dlgKeyBoard;
	CString strTemp;

	dlgKeyBoard.m_strKeyBoardVal = clsRfid.m_strRfidData[RF_LOT_ID_J];

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		strTemp = dlgKeyBoard.m_strKeyBoardVal;

		if (strTemp.GetLength() > 12)
		{
			m_strRfidData[RF_LOT_ID_J] = strTemp.Mid(0, 12);
		}
		else
		{
			m_strRfidData[RF_LOT_ID_J] = strTemp;
		}
		m_msgLotNoJData.SetWindowText(m_strRfidData[RF_LOT_ID_J]);
	}
}


void CWorkRfid::OnStnClickedMsgStepData()
{
	CDialog_KeyBoard dlgKeyBoard;
	CString strTemp;

	dlgKeyBoard.m_strKeyBoardVal = clsRfid.m_strRfidData[RF_STEP];

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		strTemp = dlgKeyBoard.m_strKeyBoardVal;

		if (strTemp.GetLength() > 6)
		{
			m_strRfidData[RF_STEP] = strTemp.Mid(0, 6);
		}
		else
		{
			m_strRfidData[RF_STEP] = strTemp;
		}

		m_msgStepData.SetWindowText(m_strRfidData[RF_STEP]);
	}
}


void CWorkRfid::OnStnClickedMsgPartNoData()
{
	CDialog_KeyBoard dlgKeyBoard;
	CString strTemp;

	dlgKeyBoard.m_strKeyBoardVal = clsRfid.m_strRfidData[RF_PART_ID];

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		strTemp = dlgKeyBoard.m_strKeyBoardVal;

		if (strTemp.GetLength() > 28)
		{
			m_strRfidData[RF_PART_ID] = strTemp.Mid(0, 28);
		}
		else
		{
			m_strRfidData[RF_PART_ID] = strTemp;
		}

		m_msgPartNoData.SetWindowText(m_strRfidData[RF_PART_ID]);
	}
}


void CWorkRfid::OnStnClickedMsgLotNoMData()
{
	CDialog_KeyBoard dlgKeyBoard;
	CString strTemp;

	dlgKeyBoard.m_strKeyBoardVal = clsRfid.m_strRfidData[RF_LOT_ID_M];

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		strTemp = dlgKeyBoard.m_strKeyBoardVal;

		if (strTemp.GetLength() > 12)
		{
			m_strRfidData[RF_LOT_ID_M] = strTemp.Mid(0, 12);
		}
		else
		{
			m_strRfidData[RF_LOT_ID_M] = strTemp;
		}
		m_msgLotNoMData.SetWindowText(m_strRfidData[RF_LOT_ID_M]);
	}
}


void CWorkRfid::OnStnClickedMsgQtyTData()
{
	CDialog_KeyBoard dlgKeyBoard;
	CString strTemp;

	dlgKeyBoard.m_strKeyBoardVal = clsRfid.m_strRfidData[RF_QTY_T];

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		strTemp = dlgKeyBoard.m_strKeyBoardVal;

		if (strTemp.GetLength() > 6)
		{
			m_strRfidData[RF_QTY_T] = strTemp.Mid(0, 6);
		}
		else
		{
			m_strRfidData[RF_QTY_T] = strTemp;
		}
		m_msgQtyTData.SetWindowText(m_strRfidData[RF_QTY_T]);
	}
}


void CWorkRfid::OnStnClickedMsgLotTypeData()
{
	CDialog_KeyBoard dlgKeyBoard;
	CString strTemp;

	dlgKeyBoard.m_strKeyBoardVal = clsRfid.m_strRfidData[RF_LOT_TYPE];

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		strTemp = dlgKeyBoard.m_strKeyBoardVal;

		if (strTemp.GetLength() > 2)
		{
			m_strRfidData[RF_LOT_TYPE] = strTemp.Mid(0, 2);
		}
		else
		{
			m_strRfidData[RF_LOT_TYPE] = strTemp;
		}
		m_msgLotTypeData.SetWindowText(m_strRfidData[RF_LOT_TYPE]);
	}
}


void CWorkRfid::OnStnClickedMsgPkgCodeData()
{
	CDialog_KeyBoard dlgKeyBoard;
	CString strTemp;

	dlgKeyBoard.m_strKeyBoardVal = clsRfid.m_strRfidData[RF_PKG_CODE];

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		strTemp = dlgKeyBoard.m_strKeyBoardVal;

		if (strTemp.GetLength() > 1)
		{
			m_strRfidData[RF_PKG_CODE] = strTemp.Mid(0, 1);
		}
		else
		{
			m_strRfidData[RF_PKG_CODE] = strTemp;
		}
		m_msgPkgCodeData.SetWindowText(m_strRfidData[RF_PKG_CODE]);
	}
}


void CWorkRfid::OnStnClickedMsgCntData()
{
	CDialog_KeyBoard dlgKeyBoard;
	CString strTemp;

	dlgKeyBoard.m_strKeyBoardVal = clsRfid.m_strRfidData[RF_CNT];

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		strTemp = dlgKeyBoard.m_strKeyBoardVal;

		if (strTemp.GetLength() > 8)
		{
			m_strRfidData[RF_CNT] = strTemp.Mid(0, 8);
		}
		else
		{
			m_strRfidData[RF_CNT] = strTemp;
		}
		m_msgCntData.SetWindowText(m_strRfidData[RF_CNT]);
	}
}


void CWorkRfid::OnStnClickedMsgLotFlagData()
{
	CDialog_KeyBoard dlgKeyBoard;
	CString strTemp;

	dlgKeyBoard.m_strKeyBoardVal = clsRfid.m_strRfidData[RF_FLAG];

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		strTemp = dlgKeyBoard.m_strKeyBoardVal;

		if (strTemp.GetLength() > 2)
		{
			m_strRfidData[RF_FLAG] = strTemp.Mid(0, 2);
		}
		else
		{
			m_strRfidData[RF_FLAG] = strTemp;
		}
		m_msgFlagData.SetWindowText(m_strRfidData[RF_FLAG]);
	}
}


void CWorkRfid::OnStnClickedMsgRejectData()
{
	CDialog_KeyBoard dlgKeyBoard;
	CString strTemp;

	dlgKeyBoard.m_strKeyBoardVal = clsRfid.m_strRfidData[RF_REJECT];

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		strTemp = dlgKeyBoard.m_strKeyBoardVal;

		if (strTemp.GetLength() > 2)
		{
			m_strRfidData[RF_REJECT] = strTemp.Mid(0, 2);
		}
		else
		{
			m_strRfidData[RF_REJECT] = strTemp;
		}
		m_msgRejectData.SetWindowText(m_strRfidData[RF_REJECT]);
	}
}


void CWorkRfid::OnStnClickedMsgBinData()
{
	CDialog_KeyBoard dlgKeyBoard;
	CString strTemp;

	dlgKeyBoard.m_strKeyBoardVal = clsRfid.m_strRfidData[RF_BIN];

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		strTemp = dlgKeyBoard.m_strKeyBoardVal;

		if (strTemp.GetLength() > 2)
		{
			m_strRfidData[RF_BIN] = strTemp.Mid(0, 2);
		}
		else
		{
			m_strRfidData[RF_BIN] = strTemp;
		}
		m_msgBinData.SetWindowText(m_strRfidData[RF_BIN]);
	}
}


void CWorkRfid::OnStnClickedMsgTrayQtyData()
{
	CDialog_KeyBoard dlgKeyBoard;
	CString strTemp;

	dlgKeyBoard.m_strKeyBoardVal = clsRfid.m_strRfidData[RF_TRAY_QTY];

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		strTemp = dlgKeyBoard.m_strKeyBoardVal;

		if (strTemp.GetLength() > 4)
		{
			m_strRfidData[RF_TRAY_QTY] = strTemp.Mid(0, 4);
		}
		else
		{
			m_strRfidData[RF_TRAY_QTY] = strTemp;
		}
		m_msgTrayQtyData.SetWindowText(m_strRfidData[RF_TRAY_QTY]);
	}
}


void CWorkRfid::OnStnClickedMsgNone1Data()
{
	CDialog_KeyBoard dlgKeyBoard;
	CString strTemp;

	dlgKeyBoard.m_strKeyBoardVal = clsRfid.m_strRfidData[RF_NONE_1];

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		strTemp = dlgKeyBoard.m_strKeyBoardVal;

		if (strTemp.GetLength() > 3)
		{
			m_strRfidData[RF_NONE_1] = strTemp.Mid(0, 3);
		}
		else
		{
			m_strRfidData[RF_NONE_1] = strTemp;
		}
		m_msgNone1Data.SetWindowText(m_strRfidData[RF_NONE_1]);
	}
}


void CWorkRfid::OnStnClickedMsgNone2Data()
{
	CDialog_KeyBoard dlgKeyBoard;
	CString strTemp;

	dlgKeyBoard.m_strKeyBoardVal = clsRfid.m_strRfidData[RF_NONE_2];

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		strTemp = dlgKeyBoard.m_strKeyBoardVal;

		if (strTemp.GetLength() > 2)
		{
			m_strRfidData[RF_NONE_2] = strTemp.Mid(0, 2);
		}
		else
		{
			m_strRfidData[RF_NONE_2] = strTemp;
		}
		m_msgNone2Data.SetWindowText(m_strRfidData[RF_NONE_2]);
	}
}

void CWorkRfid::OnDisplayData()
{
	m_strRfidData[RF_CARRIER_ID] = clsRfid.m_strRfidData[RF_CARRIER_ID];
	m_msgCarrierData.SetWindowText(m_strRfidData[RF_CARRIER_ID]);

	m_strRfidData[RF_EQP_ID] = clsRfid.m_strRfidData[RF_EQP_ID];
	m_msgEqpNoData.SetWindowText(m_strRfidData[RF_EQP_ID]);
	
	m_strRfidData[RF_QTY] = clsRfid.m_strRfidData[RF_QTY];
	m_msgQtyData.SetWindowText(m_strRfidData[RF_QTY]);
	
	m_strRfidData[RF_QTY_T] = clsRfid.m_strRfidData[RF_QTY_T];
	m_msgQtyTData.SetWindowText(m_strRfidData[RF_QTY_T]);

	m_strRfidData[RF_LOT_ID_J] = clsRfid.m_strRfidData[RF_LOT_ID_J];
	m_msgLotNoJData.SetWindowText(m_strRfidData[RF_LOT_ID_J]);
	
	m_strRfidData[RF_STEP] = clsRfid.m_strRfidData[RF_STEP];
	m_msgStepData.SetWindowText(m_strRfidData[RF_STEP]);
	
	m_strRfidData[RF_PART_ID] = clsRfid.m_strRfidData[RF_PART_ID];
	m_msgPartNoData.SetWindowText(m_strRfidData[RF_PART_ID]);
	
	m_strRfidData[RF_LOT_ID_M] = clsRfid.m_strRfidData[RF_LOT_ID_M];
	m_msgLotNoMData.SetWindowText(m_strRfidData[RF_LOT_ID_M]);
	
	m_strRfidData[RF_LOT_TYPE] = clsRfid.m_strRfidData[RF_LOT_TYPE];
	m_msgLotTypeData.SetWindowText(m_strRfidData[RF_LOT_TYPE]);
	
	m_strRfidData[RF_PKG_CODE] = clsRfid.m_strRfidData[RF_PKG_CODE];
	m_msgPkgCodeData.SetWindowText(m_strRfidData[RF_PKG_CODE]);
	
	m_strRfidData[RF_CNT] = clsRfid.m_strRfidData[RF_CNT];
	m_msgCntData.SetWindowText(m_strRfidData[RF_CNT]);
	
	m_strRfidData[RF_FLAG] = clsRfid.m_strRfidData[RF_FLAG];
	m_msgFlagData.SetWindowText(m_strRfidData[RF_FLAG]);
	
	m_strRfidData[RF_REJECT] = clsRfid.m_strRfidData[RF_REJECT];
	m_msgRejectData.SetWindowText(m_strRfidData[RF_REJECT]);
	
	m_strRfidData[RF_BIN] = clsRfid.m_strRfidData[RF_BIN];
	m_msgBinData.SetWindowText(m_strRfidData[RF_BIN]);
	
	m_strRfidData[RF_TRAY_QTY] = clsRfid.m_strRfidData[RF_TRAY_QTY];
	m_msgTrayQtyData.SetWindowText(m_strRfidData[RF_TRAY_QTY]);
	
	m_strRfidData[RF_NONE_1] = clsRfid.m_strRfidData[RF_NONE_1];
	m_msgNone1Data.SetWindowText(m_strRfidData[RF_NONE_1]);
	
	m_strRfidData[RF_NONE_2] = clsRfid.m_strRfidData[RF_NONE_2];
	m_msgNone2Data.SetWindowText(m_strRfidData[RF_NONE_2]);
}
