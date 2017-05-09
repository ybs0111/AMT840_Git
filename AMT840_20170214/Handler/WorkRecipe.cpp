// WorkRecipe.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "WorkRecipe.h"
#include "afxdialogex.h"
#include "Dialog_KeyBoard.h"
#include "Dialog_KeyPad.h"
#include "Dialog_Message.h"
#include "PublicFunction.h"
#include "CtlBd_Variable.h"
#include "Dialog_Message.h"
#include "XgemClient.h"
#include "AlgMemory.h"
#include ".\\Ctrl\\KeyBoard.h"
#include "LogFromat.h"

//#include "DialogSocketComment.h"
// CWorkRecipe 대화 상자입니다.
#define TM_INTERFACE	100

IMPLEMENT_DYNAMIC(CWorkRecipe, CDialog)

CWorkRecipe::CWorkRecipe(CWnd* pParent /*=NULL*/)
	: CDialog(CWorkRecipe::IDD, pParent)
{
	
}

CWorkRecipe::~CWorkRecipe()
{
}

void CWorkRecipe::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_APPLY,							m_btnApply);
	DDX_Control(pDX, IDC_BTN_RELOAD,						m_btnReLoad);
	DDX_Control(pDX, IDC_MSG_TRAY_ROW_CNT,					m_msgTrayRowCnt);
	DDX_Control(pDX, IDC_MSG_TRAY_COL_CNT,					m_msgTrayColCnt);
	DDX_Control(pDX, IDC_MSG_REJECT_TRAY_X_CNT,				m_msgRejectTrayXCnt);
	DDX_Control(pDX, IDC_MSG_RETEST_TRAY_X_CNT,				m_msgRetestTrayXCnt);
	DDX_Control(pDX, IDC_MSG_TRAY_ROW_OFFSET,				m_msgTrayRowOffset);
	DDX_Control(pDX, IDC_MSG_TRAY_COL_OFFSET,				m_msgTrayColOffset);
	DDX_Control(pDX, IDC_MSG_BUFFER_ROW_OFFSET,				m_msgBufRowOffset);
	DDX_Control(pDX, IDC_MSG_BUFFER_COL_OFFSET,				m_msgBufColOffset);
	DDX_Control(pDX, IDC_MSG_TESTSITE_OFFSET,				m_msgTSiteOffset);
	DDX_Control(pDX, IDC_MSG_COK_X_OFFSET,					m_msgCokXOffset);
	DDX_Control(pDX, IDC_MSG_RETEST_COUNT,					m_msgRetCnt);
	DDX_Control(pDX, IDC_MSG_FAIL_COUNT,					m_msgFailCnt);
	DDX_Control(pDX, IDC_MSG_REJECT_FULL_OFFSET,			m_msgRejectFullOffset);
	DDX_Control(pDX, IDC_MSG_REJECT_EMPTY_OFFSET,			m_msgRejectEmptyOffset);
	DDX_Control(pDX, IDC_MSG_SOCKET_COUNT,					m_msgSocketCount);
	DDX_Control(pDX, IDC_MSG_15T_OFFSET,					m_msg15TOffset);
	DDX_Control(pDX, IDC_DGT_TRAY_ROW_CNT,					m_dgtTrayRowCnt);
	DDX_Control(pDX, IDC_DGT_TRAY_COL_CNT,					m_dgtTrayColCnt);
	DDX_Control(pDX, IDC_DGT_REJECT_TRAY_X_CNT,				m_dgtRejectTrayXCnt);
	DDX_Control(pDX, IDC_DGT_RETEST_TRAY_X_CNT,				m_dgtRetestTrayXCnt);
	DDX_Control(pDX, IDC_DGT_TRAY_ROW_OFFSET,				m_dgtTrayRowOffset);
	DDX_Control(pDX, IDC_DGT_TRAY_COL_OFFSET,				m_dgtTrayColOffset);
	DDX_Control(pDX, IDC_DGT_BUFFER_ROW_OFFSET,				m_dgtBufRowOffset);
	DDX_Control(pDX, IDC_DGT_BUFFER_COL_OFFSET,				m_dgtBufColOffset);
	DDX_Control(pDX, IDC_DGT_TESTSITE_OFFSET,				m_dgtTSiteOffset);
	DDX_Control(pDX, IDC_DGT_COK_X_OFFSET,					m_dgtCokXOffset);
	DDX_Control(pDX, IDC_DGT_RETEST_COUNT,					m_dgtRetCnt);
	DDX_Control(pDX, IDC_DGT_FAIL_COUNT,					m_dgtFailCnt);
	DDX_Control(pDX, IDC_DGT_REJECT_FULL_OFFSET,			m_dgtRejectFullOffset);
	DDX_Control(pDX, IDC_DGT_REJECT_EMPTY_OFFSET,			m_dgtRejectEmptyOffset);
	DDX_Control(pDX, IDC_DGT_SOCKET_COUNT,					m_dgtSocketCount);
	DDX_Control(pDX, IDC_DGT_15T_OFFSET,					m_dgt15TOffset);
	DDX_Control(pDX, IDC_GROUP_RECIPE_TRAY,					m_groupTrayInfo);
	DDX_Control(pDX, IDC_GROUP_RECIPE_ROTATOR,				m_groupRotator);
	//	DDX_Control(pDX, IDC_GROUP_RECIPE_SOCKET,				m_groupSocket);
	//DDX_Control(pDX, IDC_CUSTOM_RECIPE_LEFT_INTERFACE,		m_pGridLfInterface);
	DDX_Control(pDX, IDC_CUSTOM_RECIPE_RIGHT_INTERFACE,		m_pGridRiInterface);
	DDX_Control(pDX, IDC_CUSTOM_RECIPE_DEGREE,				m_pGridDegree);
	DDX_Control(pDX, IDC_CUSTOM_RECIPE_INTERFACE, m_pGridInterface);
	DDX_Control(pDX, IDC_GROUP_RECIPE_TSITE_INFO, m_groupTsiteSocketInfo);
	DDX_Control(pDX, IDC_MSG_RETEST_BUFF_CNT, m_msgRetestBufferCnt);
	DDX_Control(pDX, IDC_MSG_WORK_BUFF_CNT, m_msgWorkBufferCnt);
	DDX_Control(pDX, IDC_MSG_WORK_PICKER_PITCH, m_msgWorkPickerPitch);
	DDX_Control(pDX, IDC_MSG_TEST_PICKER_PITCH, m_msgTestPickerPitch);
	DDX_Control(pDX, IDC_MSG_WORK_BUFFER_PITCH, m_msgWorkBufferPitch);
	DDX_Control(pDX, IDC_MSG_RETEST_BUFFER_PITCH, m_msgRetestBufferPitch);
	DDX_Control(pDX, IDC_MSG_TRAY_PITCH_X, m_msgTrayPitchX);
	DDX_Control(pDX, IDC_MSG_TRAY_PITCH_Y, m_msgTrayPitchY);
	DDX_Control(pDX, IDC_DGT_RETEST_BUFF_CNT, m_dgtRetestBuffCnt);
	DDX_Control(pDX, IDC_DGT_WORK_BUFF_CNT, m_dgtWorkBufferCnt);
	DDX_Control(pDX, IDC_DGT_WORK_PICKER_PITCH, m_dgtWorkPickerPitch);
	DDX_Control(pDX, IDC_DGT_TEST_PICKER_PITCH, m_dgtTestPickerPitch);
	DDX_Control(pDX, IDC_DGT_WORK_BUFF_PITCH, m_dgtWorkBuffPitch);
	DDX_Control(pDX, IDC_DGT_RETEST_BUFF_PITCH, m_dgtRetestBuffPitch);
	DDX_Control(pDX, IDC_DGT_TRAY_PITCH_X, m_dgtTrayPitchX);
	DDX_Control(pDX, IDC_DGT_TRAY_PITCH_Y, m_dgtTrayPitchY);
	DDX_Control(pDX, IDC_GROUP_OFFSET_CNT, m_group_Offset_Info);

	DDX_Control(pDX, IDC_MSG_VISON_X_OFFSET, m_msgVisionXOffset);
	DDX_Control(pDX, IDC_MSG_VISION_Y_OFFSET, m_msgVisionYOffset);
	DDX_Control(pDX, IDC_DGT_VISION_X_OFFSET, m_dgtVisionXOffset);
	DDX_Control(pDX, IDC_DGT_VISION_Y_OFFSET, m_dgtVisionYOffset);
}


BEGIN_MESSAGE_MAP(CWorkRecipe, CDialog)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BTN_APPLY,							&CWorkRecipe::OnBnClickedBtnApply)
	ON_BN_CLICKED(IDC_BTN_RELOAD,							&CWorkRecipe::OnBnClickedBtnReload)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_STN_CLICKED(IDC_DGT_TRAY_ROW_CNT,					&CWorkRecipe::OnClickedDgtTrayRowCnt)
	ON_STN_CLICKED(IDC_DGT_TRAY_COL_CNT,					&CWorkRecipe::OnClickedDgtTrayColCnt)
	ON_STN_CLICKED(IDC_DGT_TRAY_ROW_OFFSET,					&CWorkRecipe::OnClickedDgtTrayRowOffset)
	ON_STN_CLICKED(IDC_DGT_TRAY_COL_OFFSET,					&CWorkRecipe::OnClickedDgtTrayColOffset)
	ON_STN_CLICKED(IDC_DGT_TESTSITE_OFFSET,					&CWorkRecipe::OnClickedDgtTestsiteOffset)
	//ON_NOTIFY(NM_CLICK, IDC_CUSTOM_RECIPE_LEFT_INTERFACE,	OnLeftInterfaceCellClick)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_RECIPE_INTERFACE,	OnInterfaceCellClick)
	//ON_NOTIFY(NM_CLICK, IDC_CUSTOM_RECIPE_RIGHT_INTERFACE,	OnRightInterfaceCellClick)
	//ON_NOTIFY(NM_CLICK, IDC_CUSTOM_RECIPE_INTERFACE,	OnInterfaceCellClick)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_RECIPE_DEGREE,			OnDegreeCellClick)
	ON_STN_CLICKED(IDC_DGT_RETEST_COUNT,					&CWorkRecipe::OnStnClickedDgtRetestCount)
	ON_STN_CLICKED(IDC_DGT_FAIL_COUNT,						&CWorkRecipe::OnStnClickedDgtFailCount)
	ON_STN_CLICKED(IDC_DGT_BUFFER_ROW_OFFSET,				&CWorkRecipe::OnClickedDgtBufferRowOffset)
	ON_STN_CLICKED(IDC_DGT_BUFFER_COL_OFFSET,				&CWorkRecipe::OnClickedDgtBufferColOffset)
	ON_STN_CLICKED(IDC_DGT_REJECT_TRAY_X_CNT,				&CWorkRecipe::OnStnClickedDgtRejectTrayXCnt)
	ON_STN_CLICKED(IDC_DGT_RETEST_TRAY_X_CNT,				&CWorkRecipe::OnStnClickedDgtRetestTrayXCnt)
	ON_STN_CLICKED(IDC_DGT_REJECT_FULL_OFFSET, &CWorkRecipe::OnStnClickedDgtRejectFullOffset)
	ON_STN_CLICKED(IDC_DGT_REJECT_EMPTY_OFFSET, &CWorkRecipe::OnStnClickedDgtRejectEmptyOffset)
//	ON_STN_CLICKED(IDC_DGT_COK_X_OFFSET, &CWorkRecipe::OnStnClickedDgtCokXOffset)
	ON_STN_CLICKED(IDC_DGT_COK_X_OFFSET, &CWorkRecipe::OnStnClickedDgtCokXOffset)
	ON_STN_CLICKED(IDC_DGT_SOCKET_COUNT, &CWorkRecipe::OnStnClickedDgtSocketCount)
	ON_STN_CLICKED(IDC_DGT_15T_OFFSET, &CWorkRecipe::OnStnClickedDgt15tOffset)
	ON_STN_CLICKED(IDC_DGT_RETEST_BUFF_CNT, &CWorkRecipe::OnStnClickedDgtRetestBuffCnt)
	ON_STN_CLICKED(IDC_DGT_WORK_BUFF_CNT, &CWorkRecipe::OnStnClickedDgtWorkBuffCnt)
	ON_STN_CLICKED(IDC_DGT_WORK_PICKER_PITCH, &CWorkRecipe::OnStnClickedDgtWorkPickerPitch)
	ON_STN_CLICKED(IDC_DGT_TEST_PICKER_PITCH, &CWorkRecipe::OnStnClickedDgtTestPickerPitch)
	ON_STN_CLICKED(IDC_DGT_WORK_BUFF_PITCH, &CWorkRecipe::OnStnClickedDgtWorkBuffPitch)
	ON_STN_CLICKED(IDC_DGT_RETEST_BUFF_PITCH, &CWorkRecipe::OnStnClickedDgtRetestBuffPitch)
	ON_STN_CLICKED(IDC_DGT_TRAY_PITCH_X, &CWorkRecipe::OnStnClickedDgtTrayPitchX)
	ON_STN_CLICKED(IDC_DGT_TRAY_PITCH_Y, &CWorkRecipe::OnStnClickedDgtTrayPitchY)
	ON_STN_CLICKED(IDC_DGT_VISION_Y_OFFSET, &CWorkRecipe::OnStnClickedDgtVisionYOffset)
	ON_STN_CLICKED(IDC_DGT_VISION_X_OFFSET, &CWorkRecipe::OnStnClickedDgtVisionXOffset)
END_MESSAGE_MAP()


// CWorkRecipe 메시지 처리기입니다.


BOOL CWorkRecipe::DestroyWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
/*	if (OnDataComparison() == RET_ERROR)
	{
		OnDataHistoryLog();
		OnDataApply();
	}*/
	return CDialog::DestroyWindow();
}


BOOL CWorkRecipe::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	OnDataInit();
	OnInitGroupBox();
	OnInitLabel();
	OnInitButton();
	OnInitDigit();
	//OnInitGridLeftInterface();
	//OnInitGridRightInterface();
	OnInitGridInterface();//kwlee 2016.0729
	OnInitGridDegree();

	OnDataDisplay();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CWorkRecipe::OnInitGroupBox()
{
	m_groupTrayInfo.SetFont(clsFunc.OnLogFont(16));
	m_groupTrayInfo.SetCatptionTextColor(BLUE_C);
	m_groupTrayInfo.SetBorderColor(ORANGE_C);
	m_groupTrayInfo.SetBackgroundColor(WINDOW_UP1);
	m_groupTrayInfo.SetFontBold(TRUE);

	m_groupRotator.SetFont(clsFunc.OnLogFont(16));
	m_groupRotator.SetCatptionTextColor(BLUE_C);
	m_groupRotator.SetBorderColor(ORANGE_C);
	m_groupRotator.SetBackgroundColor(WINDOW_UP1);
	m_groupRotator.SetFontBold(TRUE);

	m_groupTsiteSocketInfo.SetFont(clsFunc.OnLogFont(16));
	m_groupTsiteSocketInfo.SetCatptionTextColor(BLUE_C);
	m_groupTsiteSocketInfo.SetBorderColor(ORANGE_C);
	m_groupTsiteSocketInfo.SetBackgroundColor(WINDOW_UP1);
	m_groupTsiteSocketInfo.SetFontBold(TRUE);

	m_group_Offset_Info.SetFont(clsFunc.OnLogFont(16));
	m_group_Offset_Info.SetCatptionTextColor(BLUE_C);
	m_group_Offset_Info.SetBorderColor(ORANGE_C);
	m_group_Offset_Info.SetBackgroundColor(WINDOW_UP1);
	m_group_Offset_Info.SetFontBold(TRUE);

}

void CWorkRecipe::OnInitLabel()
{
	m_msgTrayRowCnt.SetFont(clsFunc.m_pFont[2]);
	m_msgTrayRowCnt.SetWindowText(_T("Tray Y Count" ));
	m_msgTrayRowCnt.SetCenterText();
	m_msgTrayRowCnt.SetColor(WHITE_C);
	m_msgTrayRowCnt.SetGradientColor( GREEN_C);
	m_msgTrayRowCnt.SetTextColor(BLACK_C);

	m_msgTrayColCnt.SetFont(clsFunc.m_pFont[2]);
	m_msgTrayColCnt.SetWindowText(_T("Tray X Count"));
	m_msgTrayColCnt.SetCenterText();
	m_msgTrayColCnt.SetColor(WHITE_C);
	m_msgTrayColCnt.SetGradientColor(RED_C);
	m_msgTrayColCnt.SetTextColor(BLACK_C);

	m_msgRejectTrayXCnt.SetFont(clsFunc.m_pFont[2]);
	m_msgRejectTrayXCnt.SetWindowText(_T("Reject Tray X Count"));
	m_msgRejectTrayXCnt.SetCenterText();
	m_msgRejectTrayXCnt.SetColor(WHITE_C);
	m_msgRejectTrayXCnt.SetGradientColor(GREEN_C);
	m_msgRejectTrayXCnt.SetTextColor(BLACK_C);

	m_msgRetestTrayXCnt.SetFont(clsFunc.m_pFont[2]);
	m_msgRetestTrayXCnt.SetWindowText(_T("Retest Tray X Count"));
	m_msgRetestTrayXCnt.SetCenterText();
	m_msgRetestTrayXCnt.SetColor(WHITE_C);
	m_msgRetestTrayXCnt.SetGradientColor(RED_C);
	m_msgRetestTrayXCnt.SetTextColor(BLACK_C);

	m_msgTrayRowOffset.SetFont(clsFunc.m_pFont[2]);
	m_msgTrayRowOffset.SetWindowText(_T("Tray Y Offset"));
	m_msgTrayRowOffset.SetCenterText();
	m_msgTrayRowOffset.SetColor(WHITE_C);
	m_msgTrayRowOffset.SetGradientColor(GREEN_C);
	m_msgTrayRowOffset.SetTextColor(BLACK_C);

	m_msgTrayColOffset.SetFont(clsFunc.m_pFont[2]);
	m_msgTrayColOffset.SetWindowText(_T("Tray X Offset"));
	m_msgTrayColOffset.SetCenterText();
	m_msgTrayColOffset.SetColor(WHITE_C);
	m_msgTrayColOffset.SetGradientColor(RED_C);
	m_msgTrayColOffset.SetTextColor(BLACK_C);

	m_msgBufRowOffset.SetFont(clsFunc.m_pFont[2]);
	m_msgBufRowOffset.SetWindowText(_T("Buffer Y Offset"));
	m_msgBufRowOffset.SetCenterText();
	m_msgBufRowOffset.SetColor(WHITE_C);
	m_msgBufRowOffset.SetGradientColor(GREEN_C);
	m_msgBufRowOffset.SetTextColor(BLACK_C);

	m_msgBufColOffset.SetFont(clsFunc.m_pFont[2]);
	m_msgBufColOffset.SetWindowText(_T("Buffer X Offset"));
	m_msgBufColOffset.SetCenterText();
	m_msgBufColOffset.SetColor(WHITE_C);
	m_msgBufColOffset.SetGradientColor(RED_C);
	m_msgBufColOffset.SetTextColor(BLACK_C);

	m_msgTSiteOffset.SetFont(clsFunc.m_pFont[2]);
	m_msgTSiteOffset.SetWindowText(_T("Testsite Offset"));
	m_msgTSiteOffset.SetCenterText();
	m_msgTSiteOffset.SetColor(WHITE_C);
	m_msgTSiteOffset.SetGradientColor(BLUE_C);
	m_msgTSiteOffset.SetTextColor(BLACK_C);

	m_msgCokXOffset.SetFont(clsFunc.m_pFont[2]);
	m_msgCokXOffset.SetWindowText(_T("Cok X Offset"));
	m_msgCokXOffset.SetCenterText();
	m_msgCokXOffset.SetColor(WHITE_C);
	m_msgCokXOffset.SetGradientColor(BLUE_C);
	m_msgCokXOffset.SetTextColor(BLACK_C);

	m_msgRetCnt.SetFont(clsFunc.m_pFont[2]);
	m_msgRetCnt.SetWindowText(_T("Retest Count"));
	m_msgRetCnt.SetCenterText();
	m_msgRetCnt.SetColor(WHITE_C);
	m_msgRetCnt.SetGradientColor(BLUE_C);
	m_msgRetCnt.SetTextColor(BLACK_C);

	m_msgFailCnt.SetFont(clsFunc.m_pFont[2]);
	m_msgFailCnt.SetWindowText(_T("Socket Fail Count"));
	m_msgFailCnt.SetCenterText();
	m_msgFailCnt.SetColor(WHITE_C);
	m_msgFailCnt.SetGradientColor(BLUE_C);
	m_msgFailCnt.SetTextColor(BLACK_C);

	m_msgRejectFullOffset.SetFont(clsFunc.m_pFont[2]);
	m_msgRejectFullOffset.SetWindowText(_T("Reject Full Offset"));
	m_msgRejectFullOffset.SetCenterText();
	m_msgRejectFullOffset.SetColor(WHITE_C);
	m_msgRejectFullOffset.SetGradientColor(GREEN_C);
	m_msgRejectFullOffset.SetTextColor(BLACK_C);

	m_msgRejectEmptyOffset.SetFont(clsFunc.m_pFont[2]);
	m_msgRejectEmptyOffset.SetWindowText(_T("Reject Empty Offset"));
	m_msgRejectEmptyOffset.SetCenterText();
	m_msgRejectEmptyOffset.SetColor(WHITE_C);
	m_msgRejectEmptyOffset.SetGradientColor(RED_C);
	m_msgRejectEmptyOffset.SetTextColor(BLACK_C);

	m_msgSocketCount.SetFont(clsFunc.m_pFont[2]);
	m_msgSocketCount.SetWindowText(_T("Socket Count"));
	m_msgSocketCount.SetCenterText();
	m_msgSocketCount.SetColor(WHITE_C);
	m_msgSocketCount.SetGradientColor(BLUE_C);
	m_msgSocketCount.SetTextColor(BLACK_C);

	m_msg15TOffset.SetFont(clsFunc.m_pFont[2]);
	m_msg15TOffset.SetWindowText(_T("15T Tray Offset"));
	m_msg15TOffset.SetCenterText();
	m_msg15TOffset.SetColor(WHITE_C);
	m_msg15TOffset.SetGradientColor(GREEN_C);
	m_msg15TOffset.SetTextColor(BLACK_C);
	/////////////////////////////////
	//kwlee 2016.0823
	m_msgRetestBufferCnt.SetFont(clsFunc.m_pFont[2]);
	m_msgRetestBufferCnt.SetWindowText(_T("Retest Buffer Cnt"));
	m_msgRetestBufferCnt.SetCenterText();
	m_msgRetestBufferCnt.SetColor(WHITE_C);
	m_msgRetestBufferCnt.SetGradientColor(BLUE_C);
	m_msgRetestBufferCnt.SetTextColor(BLACK_C);
	
	m_msgWorkBufferCnt.SetFont(clsFunc.m_pFont[2]);
	m_msgWorkBufferCnt.SetWindowText(_T("Ld/Uld Buffer Cnt"));
	m_msgWorkBufferCnt.SetCenterText();
	m_msgWorkBufferCnt.SetColor(WHITE_C);
	m_msgWorkBufferCnt.SetGradientColor(BLUE_C);
	m_msgWorkBufferCnt.SetTextColor(BLACK_C);

	m_msgWorkPickerPitch.SetFont(clsFunc.m_pFont[2]);
	m_msgWorkPickerPitch.SetWindowText(_T("Ld/Uld Picker Pitch"));
	m_msgWorkPickerPitch.SetCenterText();
	m_msgWorkPickerPitch.SetColor(WHITE_C);
	m_msgWorkPickerPitch.SetGradientColor(GREEN_C);
	m_msgWorkPickerPitch.SetTextColor(BLACK_C);

	m_msgTestPickerPitch.SetFont(clsFunc.m_pFont[2]);
	m_msgTestPickerPitch.SetWindowText(_T("Test Picker Pitch"));
	m_msgTestPickerPitch.SetCenterText();
	m_msgTestPickerPitch.SetColor(WHITE_C);
	m_msgTestPickerPitch.SetGradientColor(GREEN_C);
	m_msgTestPickerPitch.SetTextColor(BLACK_C);

	m_msgWorkBufferPitch.SetFont(clsFunc.m_pFont[2]);
	m_msgWorkBufferPitch.SetWindowText(_T("Ld/Uld Buffer Pitch"));
	m_msgWorkBufferPitch.SetCenterText();
	m_msgWorkBufferPitch.SetColor(WHITE_C);
	m_msgWorkBufferPitch.SetGradientColor(GREEN_C);
	m_msgWorkBufferPitch.SetTextColor(BLACK_C);

	m_msgRetestBufferPitch.SetFont(clsFunc.m_pFont[2]);
	m_msgRetestBufferPitch.SetWindowText(_T("Retest Buffer Pitch"));
	m_msgRetestBufferPitch.SetCenterText();
	m_msgRetestBufferPitch.SetColor(WHITE_C);
	m_msgRetestBufferPitch.SetGradientColor(GREEN_C);
	m_msgRetestBufferPitch.SetTextColor(BLACK_C);

	m_msgTrayPitchX.SetFont(clsFunc.m_pFont[2]);
	m_msgTrayPitchX.SetWindowText(_T("Tray Pitch X"));
	m_msgTrayPitchX.SetCenterText();
	m_msgTrayPitchX.SetColor(WHITE_C);
	m_msgTrayPitchX.SetGradientColor(RED_C);
	m_msgTrayPitchX.SetTextColor(BLACK_C);

	m_msgTrayPitchY.SetFont(clsFunc.m_pFont[2]);
	m_msgTrayPitchY.SetWindowText(_T("Tray Pitch Y"));
	m_msgTrayPitchY.SetCenterText();
	m_msgTrayPitchY.SetColor(WHITE_C);
	m_msgTrayPitchY.SetGradientColor(GREEN_C);
	m_msgTrayPitchY.SetTextColor(BLACK_C);

	//kwlee 2016.0901
	m_msgVisionXOffset.SetFont(clsFunc.m_pFont[2]);
	m_msgVisionXOffset.SetWindowText(_T("Vision Offset X"));
	m_msgVisionXOffset.SetCenterText();
	m_msgVisionXOffset.SetColor(WHITE_C);
	m_msgVisionXOffset.SetGradientColor(RED_C);
	m_msgVisionXOffset.SetTextColor(BLACK_C);

	m_msgVisionYOffset.SetFont(clsFunc.m_pFont[2]);
	m_msgVisionYOffset.SetWindowText(_T("Vision Offset Y"));
	m_msgVisionYOffset.SetCenterText();
	m_msgVisionYOffset.SetColor(WHITE_C);
	m_msgVisionYOffset.SetGradientColor(GREEN_C);
	m_msgVisionYOffset.SetTextColor(BLACK_C);

}

void CWorkRecipe::OnInitDigit()
{
	m_dgtTrayRowCnt.SetStyle(CDigit::DS_INT, 1, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgtTrayRowCnt.SetVal(m_nTrayYCnt[1]);
	
	m_dgtTrayColCnt.SetStyle(CDigit::DS_INT, 1, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgtTrayColCnt.SetVal(m_nTrayXCnt[1]);

	m_dgtRejectTrayXCnt.SetStyle(CDigit::DS_INT, 1, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgtRejectTrayXCnt.SetVal(m_nRejectTrayXCnt[1]);
	
	m_dgtRetestTrayXCnt.SetStyle(CDigit::DS_INT, 1, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgtRetestTrayXCnt.SetVal(m_nRetestTrayXCnt[1]);
	
	m_dgtRetCnt.SetStyle(CDigit::DS_INT, 2, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgtRetCnt.SetVal(m_nRetCnt[1]);

	m_dgtFailCnt.SetStyle(CDigit::DS_INT, 2, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgtFailCnt.SetVal(m_nFailCnt[1]);

	m_dgtTrayRowOffset.SetStyle(CDigit::DS_FLOAT3, 7, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgtTrayRowOffset.SetFloatVal((float)m_dTrayYOffset[1]);
	
	m_dgtTrayColOffset.SetStyle(CDigit::DS_FLOAT3, 7, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgtTrayColOffset.SetFloatVal((float)m_dTrayXOffset[1]);
	
	m_dgtTSiteOffset.SetStyle(CDigit::DS_FLOAT3, 6, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgtTSiteOffset.SetFloatVal((float)m_dTSiteOffset[1]);

	m_dgtCokXOffset.SetStyle(CDigit::DS_FLOAT3, 6, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgtCokXOffset.SetFloatVal((float)m_dCokXOffset[1]);

	m_dgtBufRowOffset.SetStyle(CDigit::DS_FLOAT3, 7, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgtBufRowOffset.SetFloatVal((float)m_dBufYOffset[1]);
	
	m_dgtBufColOffset.SetStyle(CDigit::DS_FLOAT3, 7, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgtBufColOffset.SetFloatVal((float)m_dBufXOffset[1]);

	m_dgtRejectFullOffset.SetStyle(CDigit::DS_FLOAT3, 6, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgtRejectFullOffset.SetFloatVal((float)m_dRejectFullOffset[1]);
	
	m_dgtRejectEmptyOffset.SetStyle(CDigit::DS_FLOAT3, 6, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgtRejectEmptyOffset.SetFloatVal((float)m_dRejectEmptyOffset[1]);

	m_dgtSocketCount.SetStyle(CDigit::DS_INT, 2, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgtSocketCount.SetVal(m_nSocketCount[1]);

	m_dgt15TOffset.SetStyle(CDigit::DS_FLOAT3, 7, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgt15TOffset.SetFloatVal((float)m_d15TOffset[1]);

	//kwlee 2016.0823
	m_dgtRetestBuffCnt.SetStyle(CDigit::DS_INT, 2, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgtRetestBuffCnt.SetVal(m_nRetestBufferNum[1]);

	m_dgtWorkBufferCnt.SetStyle(CDigit::DS_INT, 2, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgtWorkBufferCnt.SetVal(m_nLdUldBufferNum[1]);

	m_dgtWorkPickerPitch.SetStyle(CDigit::DS_FLOAT3, 6, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgtWorkPickerPitch.SetFloatVal((float)m_dLdUldPickerPitch[1]);

	m_dgtTestPickerPitch.SetStyle(CDigit::DS_FLOAT3, 6, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgtTestPickerPitch.SetFloatVal((float)m_dTestPickerPitch[1]);

	m_dgtWorkBuffPitch.SetStyle(CDigit::DS_FLOAT3, 6, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgtWorkBuffPitch.SetFloatVal((float)m_dLdUldBufferPitch[1]);

	m_dgtRetestBuffPitch.SetStyle(CDigit::DS_FLOAT3, 6, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgtRetestBuffPitch.SetFloatVal((float)m_dRetestBufferPitch[1]);

	m_dgtTrayPitchX.SetStyle(CDigit::DS_FLOAT3, 7, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgtTrayPitchX.SetFloatVal((float)m_dTrayPitchX[1]);

	m_dgtTrayPitchY.SetStyle(CDigit::DS_FLOAT3, 7, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgtTrayPitchY.SetFloatVal((float)m_dTrayPitchY[1]);
	
	//kwlee 2016.0901
	m_dgtVisionXOffset.SetStyle(CDigit::DS_FLOAT3, 7, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgtVisionXOffset.SetFloatVal((float)m_dVisionXOffset[1]);

	m_dgtVisionYOffset.SetStyle(CDigit::DS_FLOAT3, 7, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgtVisionYOffset.SetFloatVal((float)m_dVisionYOffset[1]);
}


void CWorkRecipe::OnInitButton()
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
}


void CWorkRecipe::OnDataHistoryLog()
{
	int i, j;
	CString strMsg;

	if (m_nTrayYCnt[0]	!=	m_nTrayYCnt[1])
	{
		strMsg.Format(_T("Tray Row Count Change %d -> %d"), m_nTrayYCnt[0], m_nTrayYCnt[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}
	if (m_nTrayXCnt[0]	!=	m_nTrayXCnt[1])		
	{
		strMsg.Format(_T("Tray Col Count Change %d -> %d"), m_nTrayXCnt[0], m_nTrayXCnt[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}
	if (m_nRetCnt[0]	!=	m_nRetCnt[1])		
	{
		strMsg.Format(_T("Retest Count Change %d -> %d"), m_nRetCnt[0], m_nRetCnt[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}
	if (m_nFailCnt[0]	!=	m_nFailCnt[1])		
	{
		strMsg.Format(_T("Fail Count Change %d -> %d"), m_nFailCnt[0], m_nFailCnt[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}
	if (m_dTrayYOffset[0]	!=	m_dTrayYOffset[1])	
	{
		strMsg.Format(_T("Tray Row Offset Change %.3f -> %.3f"), m_dTrayYOffset[0], m_dTrayYOffset[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}
	if (m_dTrayXOffset[0]	!=	m_dTrayXOffset[1])	
	{
		strMsg.Format(_T("Tray Col Offset Change %.3f -> %.3f"), m_dTrayXOffset[0], m_dTrayXOffset[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}
	if (m_dTSiteOffset[0]	!=	m_dTSiteOffset[1])		
	{
		strMsg.Format(_T("Testsite Offset Change %.3f -> %.3f"), m_dTSiteOffset[0], m_dTSiteOffset[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	if (m_dCokXOffset[0]	!=	m_dCokXOffset[1])	//2015.0406 추가 	
	{
		strMsg.Format(_T("Cok Buffer Offset Change %.3f -> %.3f"), m_dCokXOffset[0], m_dCokXOffset[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}
	
	if (m_dBufYOffset[0]	!=	m_dBufYOffset[1])		
	{
		strMsg.Format(_T("Buffer Y Offset Change %.3f -> %.3f"), m_dBufYOffset[0], m_dBufYOffset[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	if (m_dBufXOffset[0]	!=	m_dBufXOffset[1])		
	{
		strMsg.Format(_T("Buffer X Offset Change %.3f -> %.3f"), m_dBufXOffset[0], m_dBufXOffset[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	if (m_nRejectTrayXCnt[0]	!=	m_nRejectTrayXCnt[1])		
	{
		strMsg.Format(_T("Reject Tray X Count Change %d -> %d"), m_nRejectTrayXCnt[0], m_nRejectTrayXCnt[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	if (m_nRetestTrayXCnt[0]	!=	m_nRetestTrayXCnt[1])		
	{
		strMsg.Format(_T("Retest Tray X Count Change %d -> %d"), m_nRetestTrayXCnt[0], m_nRetestTrayXCnt[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	for (i=0; i<8; i++)
	{
		for (j=0; j<8; j++)
		{
			if (m_nTsiteSocketUse[0][i][j]	!= m_nTsiteSocketUse[1][i][j])
			{
				strMsg.Format(_T("Testsite Socket [%d][%d] Use Change  %d -> %d"), i, j, m_nTsiteSocketUse[0][i][j], m_nTsiteSocketUse[1][i][j]);
				clsFunc.OnLogFileAdd(0, strMsg);
			}
		}
	}
	//kwlee 2016.0809
	if (m_nDegree[0][0]	!= m_nDegree[0][1])
	{
		strMsg.Format(_T("Test Site Top(1~4) Change %d -> %d"), m_nDegree[0][0], m_nDegree[0][1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	if (m_nDegree[1][0]	!= m_nDegree[1][1])
	{
		strMsg.Format(_T("Test Site Top(5~8) Change %d -> %d"), m_nDegree[1][0], m_nDegree[1][1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}
	
// 	if (m_nDegree[2][0]	!= m_nDegree[2][1])
// 	{
// 		strMsg.Format(_T("Barcode Rotator Change %d -> %d"), m_nDegree[2][0], m_nDegree[2][1]);
// 		clsFunc.OnLogFileAdd(0, strMsg);
// 	}
	/////
	if (m_dRejectEmptyOffset[0]	!=	m_dRejectEmptyOffset[1])		
	{
		strMsg.Format(_T("Reject Empty Offset Change %.3f -> %.3f"), m_dRejectEmptyOffset[0], m_dRejectEmptyOffset[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	if (m_dRejectFullOffset[0]	!=	m_dRejectFullOffset[1])		
	{
		strMsg.Format(_T("Reject Full Offset Change %.3f -> %.3f"), m_dRejectFullOffset[0], m_dRejectFullOffset[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	if (m_nSocketCount[0]	!= m_nSocketCount[1])
	{
		strMsg.Format(_T("Socket Count Change %d -> %d"), m_nSocketCount[0], m_nSocketCount[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	if (m_d15TOffset[0]	!= m_d15TOffset[1])
	{
		strMsg.Format(_T("15T Offset Change %.3f -> %.3f"), m_d15TOffset[0], m_d15TOffset[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	//kwlee 2016.0823
	if (m_nRetestBufferNum[0]	!= m_nRetestBufferNum[1])
	{
		strMsg.Format(_T("Retest Buffer Num %.3f -> %.3f"), m_nRetestBufferNum[0], m_nRetestBufferNum[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}
	if (m_nLdUldBufferNum[0]	!= m_nLdUldBufferNum[1])
	{
		strMsg.Format(_T("Load Unload Buffer Num Change %.3f -> %.3f"), m_nLdUldBufferNum[0], m_nLdUldBufferNum[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}
	if (m_dLdUldPickerPitch[0]	!= m_dLdUldPickerPitch[1])
	{
		strMsg.Format(_T("Load Unload Picker Pitch Change %.3f -> %.3f"), m_dLdUldPickerPitch[0], m_dLdUldPickerPitch[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}
	if (m_dLdUldBufferPitch[0]	!= m_dLdUldBufferPitch[1])
	{
		strMsg.Format(_T("Load Unload Buffer Pitch Change %.3f -> %.3f"), m_dLdUldBufferPitch[0], m_dLdUldBufferPitch[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}
	if (m_dTestPickerPitch[0]	!= m_dTestPickerPitch[1])
	{
		strMsg.Format(_T("Test Picker Pitch Change %.3f -> %.3f"), m_dTestPickerPitch[0], m_dTestPickerPitch[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}
	if (m_dTrayPitchX[0]	!= m_dTrayPitchX[1])
	{
		strMsg.Format(_T("Tray Pitch X Change %.3f -> %.3f"), m_dTrayPitchX[0], m_dTrayPitchX[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}
	if (m_dTrayPitchY[0]	!= m_dTrayPitchY[1])
	{
		strMsg.Format(_T("Tray Pitch Y Change %.3f -> %.3f"), m_dTrayPitchY[0], m_dTrayPitchY[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}
	if (m_dRetestBufferPitch[0]	!= m_dRetestBufferPitch[1])
	{
		strMsg.Format(_T("Retest Buffer Change %.3f -> %.3f"), m_dRetestBufferPitch[0], m_dRetestBufferPitch[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}

	//kwlee 2016.0901
	if (m_dVisionXOffset[0]	!= m_dVisionXOffset[1])
	{
		strMsg.Format(_T("Vision X Offset Change %.3f -> %.3f"), m_dVisionXOffset[0], m_dVisionXOffset[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}
	if (m_dVisionYOffset[0]	!= m_dVisionYOffset[1])
	{
		strMsg.Format(_T("Vision Y Offset Change %.3f -> %.3f"), m_dVisionYOffset[0], m_dVisionYOffset[1]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}
}


void CWorkRecipe::OnDataRecovery()
{
	int i, j;

	m_nTrayYCnt[1]			=	m_nTrayYCnt[0];	
	m_nTrayXCnt[1]			=	m_nTrayXCnt[0];	
	m_nRetCnt[1]			=	m_nRetCnt[0];
	m_nFailCnt[1]			=	m_nFailCnt[0];
	m_dTrayYOffset[1]		=	m_dTrayYOffset[0];	
	m_dTrayXOffset[1]		=	m_dTrayXOffset[0];
	m_dTSiteOffset[1]		=	m_dTSiteOffset[0];	

	m_dCokXOffset[1]		=	m_dCokXOffset[0];	 //2015.0406 james 

	m_dBufYOffset[1]		=	m_dBufYOffset[0];	
	m_dBufXOffset[1]		=	m_dBufXOffset[0];

	m_nRejectTrayXCnt[1]	=	m_nRejectTrayXCnt[0];
	m_nRetestTrayXCnt[1]	=	m_nRetestTrayXCnt[0];

	m_dRejectEmptyOffset[1]	=	m_dRejectEmptyOffset[0];
	m_dRejectFullOffset[1]	=	m_dRejectFullOffset[0];

	for (i=0; i<8; i++)
	{
		for (j=0; j<8; j++)
		{
			m_nTsiteSocketUse[1][i][j]	= m_nTsiteSocketUse[0][i][j];
		}
	}

	m_nDegree[0][1]	= m_nDegree[0][0];
	m_nDegree[1][1]	= m_nDegree[1][0];
	//m_nDegree[2][1] = m_nDegree[2][0]; //kwlee 2016.0809

	m_nSocketCount[1] = m_nSocketCount[0];

	m_d15TOffset[1]	= m_d15TOffset[0];
}


int CWorkRecipe::OnDataComparison()
{
	int i, j;

	if (m_nTrayYCnt[0]		!=	m_nTrayYCnt[1])		return RET_ERROR;
	if (m_nTrayXCnt[0]		!=	m_nTrayXCnt[1])		return RET_ERROR;
	if (m_nRetCnt[0]		!=	m_nRetCnt[1])		return RET_ERROR;
	if (m_nFailCnt[0]		!=	m_nFailCnt[1])		return RET_ERROR;
	if (m_dTrayYOffset[0]	!=	m_dTrayYOffset[1])	return RET_ERROR;
	if (m_dTrayXOffset[0]	!=	m_dTrayXOffset[1])	return RET_ERROR;
	if (m_dTSiteOffset[0]	!=	m_dTSiteOffset[1])	return RET_ERROR;

	if (m_dCokXOffset[0]	!=	m_dCokXOffset[1])	return RET_ERROR; //2015.0406 james 

	if (m_dBufYOffset[0]	!=	m_dBufYOffset[1])	return RET_ERROR;
	if (m_dBufXOffset[0]	!=	m_dBufXOffset[1])	return RET_ERROR;

	if (m_nRejectTrayXCnt[0]	!=	m_nRejectTrayXCnt[1])return RET_ERROR;
	if (m_nRetestTrayXCnt[0]	!=	m_nRetestTrayXCnt[1])return RET_ERROR;

	if (m_dRejectEmptyOffset[0]	!=	m_dRejectEmptyOffset[1])return RET_ERROR;
	if (m_dRejectFullOffset[0]	!=	m_dRejectFullOffset[1])	return RET_ERROR;

	for (i=0; i<8; i++)
	{
		for (j=0; j<8; j++)
		{
			if (m_nTsiteSocketUse[0][i][j]	!= m_nTsiteSocketUse[1][i][j]) return RET_ERROR;
		}
	}

	if (m_nDegree[0][0]	!= m_nDegree[0][1])	return RET_ERROR;
	if (m_nDegree[1][0]	!= m_nDegree[1][1]) return RET_ERROR;
	//if (m_nDegree[2][0]	!= m_nDegree[2][1]) return RET_ERROR; //kwlee 2016.0809

	if (m_nSocketCount[0]	!= m_nSocketCount[1]) return RET_ERROR;
	if (m_d15TOffset[0]	!= m_d15TOffset[1]) return RET_ERROR;
	//kwlee 2016.0823
	if (m_nRetestBufferNum[0]	!= m_nRetestBufferNum[1]) return RET_ERROR;
	if (m_nLdUldBufferNum[0]	!= m_nLdUldBufferNum[1]) return RET_ERROR;
	if (m_dLdUldPickerPitch[0]	!= m_dLdUldPickerPitch[1]) return RET_ERROR;
	if (m_dLdUldBufferPitch[0]	!= m_dLdUldBufferPitch[1]) return RET_ERROR;
	if (m_dTestPickerPitch[0]	!= m_dTestPickerPitch[1]) return RET_ERROR;
	if (m_dTrayPitchX[0]	!= m_dTrayPitchX[1]) return RET_ERROR;
	if (m_dTrayPitchY[0]	!= m_dTrayPitchY[1]) return RET_ERROR;
	if (m_dRetestBufferPitch[0]	!= m_dRetestBufferPitch[1]) return RET_ERROR;
	//kwlee 2016.0901
	if (m_dVisionXOffset[0]	!= m_dVisionXOffset[1]) return RET_ERROR;
	if (m_dVisionYOffset[0]	!= m_dVisionYOffset[1]) return RET_ERROR;
	return RET_GOOD;
}


void CWorkRecipe::OnDataApply()
{
	//2017.0109
	if (st_recipe_info.nTrayY				!= m_nTrayYCnt[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("TRAY Y COUNT");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%d"), st_recipe_info.nTrayY);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%d"), m_nTrayYCnt[1]);

		clsLog.LogConfig(_T("WORKRECIPE"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_recipe_info.nTrayX				!= m_nTrayXCnt[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("TRAY X COUNT");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%d"), st_recipe_info.nTrayX);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%d"), m_nTrayXCnt[1]);

		clsLog.LogConfig(_T("WORKRECIPE"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_recipe_info.nTestRetest_Count				!= m_nRetCnt[1])
	{
		if (st_work_info.nXgemChk == NO)
		{
			st_work_info.nXgemChk = YES;
		}
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("TEST SITE RETEST COUNT");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%d"), st_recipe_info.nTestRetest_Count);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%d"), m_nRetCnt[1]);

		clsLog.LogConfig(_T("WORKRECIPE"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);

		//////////////////////////////////////////////////////////////////////////
		//2017.0321
		int i = 0, j = 0;
		for (i=0; i<10000; i++)
		{
			for (j=0; j<2; j++)
			{
				st_code_info[0].m_nScrapCode[i][j] = st_recipe_info.nTestRetest_Count;
				st_code_info[1].m_nScrapCode[i][j] = st_recipe_info.nTestRetest_Count;
			}
		}
		if( st_recipe_info.nTestRetest_Count > 0)
			clsFunc.OnScrapCodeFind(0, _T("C:\\AMT840\\File\\RETEST_RULE.TXT"));
		//////////////////////////////////////////////////////////////////////////

	}
	if (st_recipe_info.nFailCnt				!= m_nFailCnt[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("FAIL COUNT");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%d"), st_recipe_info.nFailCnt);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%d"), m_nFailCnt[1]);

		clsLog.LogConfig(_T("WORKRECIPE"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_recipe_info.dTrayYOffset				!= m_dTrayYOffset[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("TRAY Y OFFSET");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%.3f"), st_recipe_info.dTrayYOffset);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%.3f"), m_dTrayYOffset[1]);

		clsLog.LogConfig(_T("WORKRECIPE"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_recipe_info.dTrayXOffset				!= m_dTrayXOffset[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("TRAY X OFFSET");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%.3f"), st_recipe_info.dTrayXOffset);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%.3f"), m_dTrayXOffset[1]);

		clsLog.LogConfig(_T("WORKRECIPE"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_recipe_info.dTSiteOffsetX				!= m_dTSiteOffset[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("TEST SITE X OFFSET");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%.3f"), st_recipe_info.dTSiteOffsetX);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%.3f"), m_dTSiteOffset[1]);

		clsLog.LogConfig(_T("WORKRECIPE"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_recipe_info.dCOKBuffPitch_X				!= m_dCokXOffset[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("COK BUFFER PITCH X");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%.3f"), st_recipe_info.dCOKBuffPitch_X);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%.3f"), m_dCokXOffset[1]);

		clsLog.LogConfig(_T("WORKRECIPE"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_recipe_info.dBufferPitch_Y				!= m_dBufYOffset[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("COK BUFFER PITCH Y");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%.3f"), st_recipe_info.dBufferPitch_Y);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%.3f"), m_dBufYOffset[1]);

		clsLog.LogConfig(_T("WORKRECIPE"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_recipe_info.dBufferPitch_X				!= m_dBufXOffset[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("BUFFER PITCH X");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%.3f"), st_recipe_info.dBufferPitch_X);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%.3f"), m_dBufXOffset[1]);

		clsLog.LogConfig(_T("WORKRECIPE"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_recipe_info.nRejectTray_X				!= m_nRejectTrayXCnt[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("REJECT X");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%d"), st_recipe_info.nRejectTray_X);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%d"), m_nRejectTrayXCnt[1]);

		clsLog.LogConfig(_T("WORKRECIPE"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_recipe_info.nRetestTray_X				!= m_nRetestTrayXCnt[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("RETEST X");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%d"), st_recipe_info.nRetestTray_X);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%d"), m_nRetestTrayXCnt[1]);

		clsLog.LogConfig(_T("WORKRECIPE"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_recipe_info.dRejTrayPick				!= m_dRejectEmptyOffset[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("REJECT TRAY PICK");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%.3f"), st_recipe_info.dRejTrayPick);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%.3f"), m_dRejectEmptyOffset[1]);

		clsLog.LogConfig(_T("WORKRECIPE"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_recipe_info.dRejTrayPlace				!= m_dRejectFullOffset[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("REJECT TRAY PLACE");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%.3f"), st_recipe_info.dRejTrayPlace);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%.3f"), m_dRejectFullOffset[1]);

		clsLog.LogConfig(_T("WORKRECIPE"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}


	if (st_recipe_info.nTestSocket_Num				!= m_nSocketCount[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("TESTSOCKET NUM");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%d"), st_recipe_info.nTestSocket_Num);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%d"), m_nSocketCount[1]);

		clsLog.LogConfig(_T("WORKRECIPE"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_recipe_info.nRetestBuffer_Num				!= m_nRetestBufferNum[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("RETESTBUFFER NUM");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%d"), st_recipe_info.nRetestBuffer_Num);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%d"), m_nRetestBufferNum[1]);

		clsLog.LogConfig(_T("WORKRECIPE"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_recipe_info.nLdUldBuffer_Num				!= m_nLdUldBufferNum[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("LDULD BUFFER NUM");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%.3f"), st_recipe_info.nLdUldBuffer_Num);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%.3f"), m_nLdUldBufferNum[1]);

		clsLog.LogConfig(_T("WORKRECIPE"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_recipe_info.dLdUldPickerPitch				!= m_dLdUldPickerPitch[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("LDULD PICKER PITCH");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%.3f"), st_recipe_info.dLdUldPickerPitch);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%.3f"), m_dLdUldPickerPitch[1]);

		clsLog.LogConfig(_T("WORKRECIPE"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_recipe_info.dTestPickerPitch				!= m_dTestPickerPitch[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("TESTER PITCH");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%d"), st_recipe_info.dTestPickerPitch);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%d"), m_dTestPickerPitch[1]);

		clsLog.LogConfig(_T("WORKRECIPE"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_recipe_info.dLdUldBufferPitch				!= m_dLdUldBufferPitch[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("LDULD BUFFER PITCH");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%d"), st_recipe_info.dLdUldBufferPitch);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%d"), m_dLdUldBufferPitch[1]);

		clsLog.LogConfig(_T("WORKRECIPE"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_recipe_info.dRetestBufferPitch				!= m_dRetestBufferPitch[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("RETEST BUFFER PITCH");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%.3f"), st_recipe_info.dRetestBufferPitch);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%.3f"), m_dRetestBufferPitch[1]);

		clsLog.LogConfig(_T("WORKRECIPE"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_recipe_info.dTrayPitch_Y				!= m_dTrayPitchY[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("TRAYPITCH Y");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%.3f"), st_recipe_info.dTrayPitch_Y);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%.3f"), m_dTrayPitchY[1]);

		clsLog.LogConfig(_T("WORKRECIPE"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_recipe_info.dTrayPitch_X				!= m_dTrayPitchX[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("TRAYPITCH X");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%.3f"), st_recipe_info.dTrayPitch_X);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%.3f"), m_dTrayPitchX[1]);

		clsLog.LogConfig(_T("WORKRECIPE"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_recipe_info.dVisionXOffset				!= m_dVisionXOffset[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("VISION X OFFSET");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%.3f"), st_recipe_info.dVisionXOffset);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%.3f"), m_dVisionXOffset[1]);

		clsLog.LogConfig(_T("WORKRECIPE"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_recipe_info.dVisionYOffset				!= m_dVisionYOffset[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("VISION Y OFFSET");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%.3f"), st_recipe_info.dVisionYOffset);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%.3f"), m_dVisionYOffset[1]);

		clsLog.LogConfig(_T("WORKRECIPE"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}
	if (st_recipe_info.d15TOffset				!= m_d15TOffset[1])
	{
		m_strLogKey[0]	= _T("TYPE");
		m_strLogData[0]	= _T("15T OFFSET");
		m_strLogKey[1]	= _T("OLD");
		m_strLogData[1].Format(_T("%.3f"), st_recipe_info.d15TOffset);
		m_strLogKey[2]	= _T("NEW");
		m_strLogData[2].Format(_T("%.3f"), m_d15TOffset[1]);

		clsLog.LogConfig(_T("WORKRECIPE"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	st_recipe_info.nTrayY				=	m_nTrayYCnt[1];	
	st_recipe_info.nTrayX				=	m_nTrayXCnt[1];	
	st_recipe_info.nTestRetest_Count	=	m_nRetCnt[1];
	st_recipe_info.nFailCnt				=	m_nFailCnt[1];
	st_recipe_info.dTrayYOffset			=	m_dTrayYOffset[1];
	st_recipe_info.dTrayXOffset			=	m_dTrayXOffset[1];
	st_recipe_info.dTSiteOffsetX		=	m_dTSiteOffset[1];	

	st_recipe_info.dCOKBuffPitch_X		=	m_dCokXOffset[1];	 //2015.0406 james 

	st_recipe_info.dBufferPitch_Y		=	m_dBufYOffset[1];
	st_recipe_info.dBufferPitch_X		=	m_dBufXOffset[1];

	st_recipe_info.nRejectTray_X		=	m_nRejectTrayXCnt[1];
	st_recipe_info.nRetestTray_X		=	m_nRetestTrayXCnt[1];

	st_recipe_info.dRejTrayPick			=	m_dRejectEmptyOffset[1];
	st_recipe_info.dRejTrayPlace		=	m_dRejectFullOffset[1];

	st_count_info.nHourSocket = 0;

	for (int i=0; i<8; i++)
	{
		for (int j=0; j<8; j++)
		{
			st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].nEnable = m_nTsiteSocketUse[1][i][j];
		}
	}
	//kwlee 2016.1106
	for (int i=0; i<10000; i++)
	{
		st_work_info.nScrapRetest[i] = st_recipe_info.nTestRetest_Count;
	}
	///kwlee2016.0809
// 	st_recipe_info.nBufferRotator_PlateSite_Degrees	= m_nDegree[0][1];
// 	st_recipe_info.nBufferRotator_TestSite_Degrees	= m_nDegree[1][1];
	// jtkim 20151017 
	//st_recipe_info.nBufferRotator_Barcode_Degrees	= m_nDegree[2][1];

	st_recipe_info.nTestSite_Top_Degrees	= m_nDegree[0][1];
	st_recipe_info.nTestSite_Btm_Degrees	= m_nDegree[1][1];
	////
	//kwlee 2016.0823
	//st_basic_info.nSocketCount = m_nSocketCount[1];
	st_recipe_info.nTestSocket_Num = m_nSocketCount[1];
	st_recipe_info.nRetestBuffer_Num = m_nRetestBufferNum[1];
	st_recipe_info.nLdUldBuffer_Num = m_nLdUldBufferNum[1];

	st_recipe_info.dLdUldPickerPitch  = m_dLdUldPickerPitch[1];
	st_recipe_info.dTestPickerPitch	  = m_dTestPickerPitch[1];
	st_recipe_info.dLdUldBufferPitch  = m_dLdUldBufferPitch[1];
	st_recipe_info.dRetestBufferPitch = m_dRetestBufferPitch[1];
	st_recipe_info.dTrayPitch_Y		  = m_dTrayPitchY[1];
	st_recipe_info.dTrayPitch_X		  = m_dTrayPitchX[1];

	st_recipe_info.dVisionXOffset     = m_dVisionXOffset[1];
	st_recipe_info.dVisionYOffset     = m_dVisionYOffset[1];
	///////////////////

	if (st_count_info.nHourSocket <= 0)
	{
		st_count_info.nHourSocket = 0;
		st_count_info.dHourPer = 0.0f;
	}
	else
	{
		st_count_info.dHourPer = ((double)st_count_info.nHourSocket / (double)48) * (double)100;
	}

	st_recipe_info.d15TOffset	= m_d15TOffset[1];
	

}


void CWorkRecipe::OnDataBackup()
{
	int i, j;

	m_nTrayYCnt[0]			=	m_nTrayYCnt[1];	
	m_nTrayXCnt[0]			=	m_nTrayXCnt[1];	
	m_nRetCnt[0]			=	m_nRetCnt[1];
	m_nFailCnt[0]			=	m_nFailCnt[1];
	m_dTrayYOffset[0]		=	m_dTrayYOffset[1];	
	m_dTrayXOffset[0]		=	m_dTrayXOffset[1];
	m_dTSiteOffset[0]		=	m_dTSiteOffset[1];	

	m_dCokXOffset[0]		=	m_dCokXOffset[1];	//2015.0406 james 

	m_dBufYOffset[0]		=	m_dBufYOffset[1];	
	m_dBufXOffset[0]		=	m_dBufXOffset[1];

	m_nRejectTrayXCnt[0]	=	m_nRejectTrayXCnt[1];
	m_nRetestTrayXCnt[0]	=	m_nRetestTrayXCnt[1];

	m_dRejectEmptyOffset[0]	=	m_dRejectEmptyOffset[1];
	m_dRejectFullOffset[0]	=	m_dRejectFullOffset[1];

	for (i=0; i<8; i++)
	{
		for (j=0; j<12; j++)
		{
			m_nTsiteSocketUse[0][i][j]	= m_nTsiteSocketUse[1][i][j];
		}
	}

	m_nDegree[0][0]	= m_nDegree[0][1];
	m_nDegree[1][0]	= m_nDegree[1][1];
	//m_nDegree[2][0] = m_nDegree[2][1]; //kwlee 2016.0809

	m_nSocketCount[0] = m_nSocketCount[1];
	m_d15TOffset[0]	= m_d15TOffset[1];

	//kwlee 2016.0823
	m_nRetestBufferNum[0]	= m_nRetestBufferNum[1];
	m_nLdUldBufferNum[0]	= m_nLdUldBufferNum[1];
	m_dLdUldPickerPitch[0]  = m_dLdUldPickerPitch[1];
	m_dLdUldBufferPitch[0]	= m_dLdUldBufferPitch[1];	
	m_dTestPickerPitch[0]	= m_dTestPickerPitch[1]	;
	m_dTrayPitchX[0]		= m_dTrayPitchX[1];
	m_dTrayPitchY[0]		= m_dTrayPitchY[1];
	m_dRetestBufferPitch[0]	= m_dRetestBufferPitch[1];

	m_dVisionXOffset[0]		= m_dVisionXOffset[1];
	m_dVisionYOffset[0]		= m_dVisionYOffset[1];
}


void CWorkRecipe::OnDataInit()
{
	m_nTrayYCnt[1]		=	st_recipe_info.nTrayY;
	m_nTrayXCnt[1]		=	st_recipe_info.nTrayX;
	m_nRetCnt[1]		=	st_recipe_info.nTestRetest_Count;
	m_nFailCnt[1]		=	st_recipe_info.nFailCnt;
	m_dTrayYOffset[1]	=	st_recipe_info.dTrayYOffset;
	m_dTrayXOffset[1]	=	st_recipe_info.dTrayXOffset;
	m_dTSiteOffset[1]	=	st_recipe_info.dTSiteOffsetX;

	m_dCokXOffset[1]	=	st_recipe_info.dCOKBuffPitch_X; //2015.0406 james 

	m_dBufYOffset[1]	=	st_recipe_info.dBufferPitch_Y;	
	m_dBufXOffset[1]	=	st_recipe_info.dBufferPitch_X;

	m_nRejectTrayXCnt[1]=	st_recipe_info.nRejectTray_X;
	m_nRetestTrayXCnt[1]=	st_recipe_info.nRetestTray_X;

	m_dRejectEmptyOffset[1]	= st_recipe_info.dRejTrayPick;
	m_dRejectFullOffset[1]	= st_recipe_info.dRejTrayPlace;

	for (int i =0; i<8; i++)
	{
		for (int j =0; j<8; j++)
		{
			m_nTsiteSocketUse[1][i][j] = st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].nEnable;
		}
	}
	//kwlee 2016.0809
	m_nDegree[0][1]	= st_recipe_info.nTestSite_Top_Degrees;
	m_nDegree[1][1]	= st_recipe_info.nTestSite_Btm_Degrees;
// 	m_nDegree[0][1]	= st_recipe_info.nBufferRotator_PlateSite_Degrees;
// 	m_nDegree[1][1]	= st_recipe_info.nBufferRotator_TestSite_Degrees;
// 	m_nDegree[2][1] = st_recipe_info.nBufferRotator_Barcode_Degrees;
	//
	//kwlee 2016.0823
	//m_nSocketCount[1] = st_basic_info.nSocketCount;
	m_nSocketCount[1]		= st_recipe_info.nTestSocket_Num;
	m_nRetestBufferNum[1]	= st_recipe_info.nRetestBuffer_Num;
	m_nLdUldBufferNum[1]	= st_recipe_info.nLdUldBuffer_Num ;
	m_dLdUldPickerPitch[1]	= st_recipe_info.dLdUldPickerPitch ;
	m_dTestPickerPitch[1]	=  st_recipe_info.dTestPickerPitch; 
	m_dLdUldBufferPitch[1]	= st_recipe_info.dLdUldBufferPitch;
	m_dRetestBufferPitch[1] = st_recipe_info.dRetestBufferPitch;
	m_dTrayPitchY[1]		= st_recipe_info.dTrayPitch_Y; 
	m_dTrayPitchX[1]        = st_recipe_info.dTrayPitch_X; 
						
	m_dVisionXOffset[1]		= st_recipe_info.dVisionXOffset;												 
	m_dVisionYOffset[1]		= st_recipe_info.dVisionYOffset;
	/////////////////////
	m_d15TOffset[1]	= st_recipe_info.d15TOffset;

	OnDataBackup();  // 현재 클래스 변수 설정 상태 백업
}


BOOL CWorkRecipe::OnEraseBkgnd(CDC* pDC)
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


void CWorkRecipe::OnBnClickedBtnApply()
{
	CDialog_Message dlgMsg;
	int nResponse;

	dlgMsg.m_nMessageType	= 1;
	dlgMsg.m_strMessage		= _T("Do you want to change the existing value to the current value?");

	nResponse = (int)dlgMsg.DoModal();

	if (nResponse == IDOK)
	{
		if (OnDataComparison() == RET_ERROR)
		{
			OnDataHistoryLog();
			OnDataApply();
			OnDataBackup();  // 현재 클래스 변수 설정 상태 백업
		}
	}
}


void CWorkRecipe::OnBnClickedBtnReload()
{
	OnDataBackup();  // 현재 클래스 변수 설정 상태 백업
}


void CWorkRecipe::OnDataDisplay()
{
	
}


void CWorkRecipe::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (OnDataComparison() == RET_ERROR)
	{
		OnDataHistoryLog();
		OnDataApply();
	}
	CDialog::PostNcDestroy();
}


void CWorkRecipe::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (OnDataComparison() == RET_ERROR)
	{
		OnDataHistoryLog();
		OnDataApply();
	}
}

void CWorkRecipe::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	
	
	CDialog::OnTimer(nIDEvent);
}


void CWorkRecipe::OnClickedDgtTrayRowCnt()
{
	int nKey = m_nTrayYCnt[1];

	KeyPadI(1, 5, &nKey);
	m_nTrayYCnt[1] = nKey;
	m_dgtTrayRowCnt.SetVal(nKey);
}


void CWorkRecipe::OnClickedDgtTrayColCnt()
{
	int nKey = m_nTrayXCnt[1];

	KeyPadI(1, 5, &nKey);
	m_nTrayXCnt[1] = nKey;
	m_dgtTrayColCnt.SetVal(nKey);
}


void CWorkRecipe::OnClickedDgtTrayRowOffset()
{
	double dKey = m_dTrayYOffset[1];

	KeyPadD(0.0f, 10000.0f, &dKey);

	m_dTrayYOffset[1] = dKey;
	m_dgtTrayRowOffset.SetFloatVal((float)dKey);
}


void CWorkRecipe::OnClickedDgtTrayColOffset()
{
	double dKey = m_dTrayXOffset[1];

	KeyPadD(0.0f, 1000.0f, &dKey);

	m_dTrayXOffset[1] = dKey;
	m_dgtTrayColOffset.SetFloatVal((float)dKey);
}


void CWorkRecipe::OnClickedDgtTestsiteOffset()
{
	double dKey = m_dTSiteOffset[1];

	KeyPadD(0.0f, 1000.0f, &dKey);

	m_dTSiteOffset[1] = dKey;
	m_dgtTSiteOffset.SetFloatVal((float)dKey);
}

void CWorkRecipe::OnStnClickedDgtRetestCount()
{
	int nKey = m_nRetCnt[1];

	KeyPadI(0, 110, &nKey);

	m_nRetCnt[1] = nKey;
	m_dgtRetCnt.SetVal(nKey);
}

void CWorkRecipe::OnStnClickedDgtFailCount()
{
	int nKey = m_nFailCnt[1];

	KeyPadI(0, 110, &nKey);

	m_nFailCnt[1] = nKey;
	m_dgtFailCnt.SetVal(nKey);
}

// void CWorkRecipe::OnLeftInterfaceCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
// {
// 	int nRow, nCol;
// 	int nSite = 0;
// 
// 	CString strMsg;
// 
// 	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;
// 
// 	nRow				= pItem->iRow;
// 	nCol				= pItem->iColumn;
// 
// 	if (nRow < 1) return;
// 
// 
// 	m_pGridLfInterface.Invalidate(FALSE);
// }

// void CWorkRecipe::OnRightInterfaceCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
// {
// 	int nRow, nCol;
// 	int nSite = 0;
// 
// 	CString strMsg;
// 
// 	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;
// 
// 	nRow				= pItem->iRow;
// 	nCol				= pItem->iColumn;
// 
// 	if (nRow < 1) return;
// }

void CWorkRecipe::OnInterfaceCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{

	int nRow, nCol;
	int nRowTemp = 0;
	int nColTemp = 0;
	CDialog_Message			dlgMsg;
	//	CDialogSocketComment	dlgComment;
	CString strMsg;
	CString strTemp;
	int i,j,nSite;
	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	if (nRow <1 ) return;
	if (nCol == 0)
	{
		if (nRow>= 2 && nRow <10) return;
		if(nRow >10) return;
	}
		

	if (nRow > 10) 
	{
		//Bottom
		nRowTemp = nRow - 10;
		nColTemp = nCol +4;
		
	}
	else 
	{
		//Top
		nRowTemp = 10 - nRow ;
		nColTemp = nCol;
	}

	//2016.1121
	//return;

	if (nCol == 0 && nRow == 1 || nCol == 0 && nRow == 10)
	{
		// Left All Socket
		if (nRow == 1)
		{
			dlgMsg.m_nMessageType	= 1;
			dlgMsg.m_strMessage.Format(_T("Do you want a Board All On?"));

			if (dlgMsg.DoModal() == IDOK)
			{
				for (i=0; i<4; i++)
				{
					for(j =0; j< 8; j++ )
					{
						if (m_nTsiteSocketUse[1][i][j] == NO)
						{
							dlgMsg.m_nMessageType	= 1;
							dlgMsg.m_strMessage.Format(_T("LEft [%02d]Board Count Reset?...."), i + 1);
							if (dlgMsg.DoModal() == IDOK)
							{
								//kwlee 2017.0124
								st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].nFailCount = 0;
							}
						}
						//Test Site Socket Use;
						m_nTsiteSocketUse[1][i][j] = YES;
						m_pGridInterface.SetItemBkColour(j+2, i + 1, YELLOW_C, BLACK_L);
						m_pGridInterface.SetItemFont(j+2, i + 1, &clsFunc.OnLogFont(18));
						m_pGridInterface.SetItemText(j+2, i + 1, _T("Y"));

						//Test Site Use;
// 						m_nTsiteUse[1][i] = YES;
// 						strTemp.Format(_T("%d"),i + 1);
// 						m_pGridInterface.SetItemText(nRow, i + 1,strTemp);
// 						m_pGridInterface.SetItemBkColour(nRow, i + 1, RGB(219, 229, 241), BLACK_L);
// 						m_pGridInterface.SetItemFont(nRow, i + 1, &clsFunc.OnLogFont(18));
					}
				}
			}
			if (st_handler_info.cWndList != NULL)
			{
				clsMem.OnNormalMessageWrite(_T("Left Board All On"));
			}
		}
		//Right All Socket
		else if (nRow == 10)
		{
			dlgMsg.m_nMessageType	= 1;
			dlgMsg.m_strMessage.Format(_T("Do you want a Right Board All On?"));

			if (dlgMsg.DoModal() == IDOK)
			{
				for (i=0; i<4; i++)
				{
					for (j =0; j<8; j++)
					{
						if (m_nTsiteSocketUse[1][i + 4][j] == NO)
						{
							dlgMsg.m_nMessageType	= 1;
							dlgMsg.m_strMessage.Format(_T("[%02d] Board Count Reset?"), i + 1);
							if (dlgMsg.DoModal() == IDOK)
							{
								//kwlee 2017.0124
								st_test_site_info[THD_TESTSITE_5 + i].st_pcb_info[j].nFailCount = 0;
							}
						}

						m_nTsiteSocketUse[1][i + 4][j] = YES;

						m_pGridInterface.SetItemBkColour(j+11, i + 1, YELLOW_C, BLACK_L);
						m_pGridInterface.SetItemFont(j+11, i + 1, &clsFunc.OnLogFont(18));
						m_pGridInterface.SetItemText(j+11, i + 1, _T("Y"));

						//Test Site Use;
// 						m_nTsiteUse[1][i +4] = YES;
// 						strTemp.Format(_T("%d"),i + 5);
// 						m_pGridInterface.SetItemText(nRow, i + 1,strTemp);
// 						m_pGridInterface.SetItemBkColour(nRow, i + 1, RGB(219, 229, 241), BLACK_L);
// 						m_pGridInterface.SetItemFont(nRow, i + 1, &clsFunc.OnLogFont(18));
					}
	
				}
			}
			if (st_handler_info.cWndList != NULL)
			{
				clsMem.OnNormalMessageWrite(_T(" Right Board All On"));
			}

		}
	}
	else
	{
		//if (m_nTsiteUse[1][nRow-1][nCol-1] == YES)
		//Left Tsite Socket 
		if (m_nTsiteSocketUse[1][nColTemp - 1][nRowTemp - 1] == YES)
		{
			if (nRow>= 2 && nRow <10)
			{
				dlgMsg.m_nMessageType	= 1;
				dlgMsg.m_strMessage.Format(_T("Do you want a Left [%02d] Board Off?"), (10 - nRow));
				if (dlgMsg.DoModal() == IDOK)
				{
					m_nTsiteSocketUse[1][nColTemp - 1][nRowTemp - 1] = NO;
					m_pGridInterface.SetItemBkColour(nRow, nCol, YELLOW_D, BLACK_L);
					m_pGridInterface.SetItemFont(nRow, nCol, &clsFunc.OnLogFont(18));
					m_pGridInterface.SetItemText(nRow, nCol, _T("N"));

					if (st_handler_info.cWndList != NULL)
					{
						strTemp.Format(_T("[Recipe] %d site %d th Board Off"),nColTemp, 10 - nRow);
						clsMem.OnNormalMessageWrite(strTemp);
					}
				}
			}
			else
			{
				dlgMsg.m_nMessageType	= 1;
				dlgMsg.m_strMessage.Format(_T("Do you want a Right [%02d] Board Off?"), (nRow - 10));
				if (dlgMsg.DoModal() == IDOK)
				{
					m_nTsiteSocketUse[1][nColTemp - 1][nRowTemp - 1] = NO;
					m_pGridInterface.SetItemBkColour(nRow, nCol, YELLOW_D, BLACK_L);
					m_pGridInterface.SetItemFont(nRow, nCol, &clsFunc.OnLogFont(18));
					m_pGridInterface.SetItemText(nRow, nCol, _T("N"));
	
				}
			}
		}
		//Right Tsite Socket
		else
		{
			//if (nRow >= 2 && nRow != 10)
			if (nRow>= 2 && nRow <10)
			{
				dlgMsg.m_nMessageType	= 1;
				dlgMsg.m_strMessage.Format(_T("Do you want a Left [%02d] Board On?"), (10 - nRow) );

				if (dlgMsg.DoModal() == IDOK)
				{
					m_nTsiteSocketUse[1][nColTemp - 1][nRowTemp - 1] = YES;
					//kwlee 2017.0124
					st_test_site_info[THD_TESTSITE_1 + nColTemp - 1].st_pcb_info[nRowTemp - 1].nFailCount = 0;

					m_pGridInterface.SetItemBkColour(nRow, nCol, YELLOW_C, BLACK_L);
					m_pGridInterface.SetItemText(nRow, nCol, _T("Y"));

					dlgMsg.m_nMessageType	= 1;
					dlgMsg.m_strMessage.Format(_T("Do you want a Left [%02d] Board Count Reset?"), (10 - nRow));

// 					//Tsite Use
// 					m_nTsiteUse[1][nCol - 1] = YES;
// 					strTemp.Format(_T("%d"),nCol);
// 					m_pGridInterface.SetItemText(nRow, nCol,strTemp);
// 					m_pGridInterface.SetItemBkColour(nRow, nCol, RGB(219, 229, 241), BLACK_L);
// 					m_pGridInterface.SetItemFont(nRow, nCol, &clsFunc.OnLogFont(18));

					if (st_handler_info.cWndList != NULL)
					{
						strTemp.Format(_T("[Recipe] %d site %d th Board On"),nColTemp, 10 - nRow);
						clsMem.OnNormalMessageWrite(strTemp);
					}
					if (dlgMsg.DoModal() == IDOK)
					{
						
					}
				}
			}
			else if(nRow >10)
			{
				dlgMsg.m_nMessageType	= 1;
				dlgMsg.m_strMessage.Format(_T("Do you want a Right [%02d] Board On?"), (nRow - 10));
				if (dlgMsg.DoModal() == IDOK)
				{
					m_nTsiteSocketUse[1][nColTemp - 1][nRowTemp - 1] = YES;
					//kwlee 2017.0124
					st_test_site_info[THD_TESTSITE_5 + nCol - 1].st_pcb_info[nRowTemp - 1].nFailCount = 0;

					m_pGridInterface.SetItemBkColour(nRow, nCol, YELLOW_C, BLACK_L);
					m_pGridInterface.SetItemText(nRow, nCol, _T("Y"));

					dlgMsg.m_nMessageType	= 1;
					dlgMsg.m_strMessage.Format(_T("Do you want a Right [%02d] Board Count Reset?"), (nRow - 10) );
					if (dlgMsg.DoModal() == IDOK)
					{

					}
				}
			}
			//Tsite Use
			else
			{
				if (nRow == 1)
				{
					if (m_nTsiteUse[1][nCol - 1]  == NO)
					{
						dlgMsg.m_nMessageType	= 1;
						dlgMsg.m_strMessage.Format(_T("Left [%02d]Board Use?...."), nCol);

						if (dlgMsg.DoModal() == IDOK)
						{
							m_nTsiteUse[1][nCol - 1] = YES;
							strTemp.Format(_T("%d"),nCol);
							m_pGridInterface.SetItemText(nRow, nCol,strTemp);
							m_pGridInterface.SetItemBkColour(nRow, nCol, RGB(219, 229, 241), BLACK_L);
							m_pGridInterface.SetItemFont(nRow, nCol, &clsFunc.OnLogFont(18));

							for(j =0; j< 8; j++ )
							{
								m_nTsiteSocketUse[1][nCol - 1][j] = YES;
								m_pGridInterface.SetItemBkColour(j+2, nCol, YELLOW_C, BLACK_L);
								m_pGridInterface.SetItemFont(j+2, nCol, &clsFunc.OnLogFont(18));
								m_pGridInterface.SetItemText(j+2, nCol, _T("Y"));

								//kwlee 2017.0124
								st_test_site_info[THD_TESTSITE_1 + nCol - 1].st_pcb_info[j].nFailCount = 0;
							}

							if (st_handler_info.cWndList != NULL)
							{
								strTemp.Format(_T("[Recipe] %d th Board All On"), nCol);
								clsMem.OnNormalMessageWrite(strTemp);
							}

						}


					}
					else
					{
						dlgMsg.m_nMessageType	= 1;
						dlgMsg.m_strMessage.Format(_T("LEft [%02d]Board Not Use?...."), nCol);
						if (dlgMsg.DoModal() == IDOK)
						{
							
							m_nTsiteUse[1][nCol - 1] = NO;
							m_pGridInterface.SetItemBkColour(nRow, nCol, RGB(219, 229, 241), BLACK_L);
							m_pGridInterface.SetItemFont(nRow, nCol, &clsFunc.OnLogFont(18));
							//m_pGridInterface.SetItemText(nRow, nCol, _T("NOT USE"));


							for(j =0; j< 8; j++ )
							{
								m_nTsiteSocketUse[1][nCol - 1][j] = NO;
								m_pGridInterface.SetItemBkColour(j+2, nCol, YELLOW_D, BLACK_L);
								m_pGridInterface.SetItemFont(j+2, nCol, &clsFunc.OnLogFont(18));
								m_pGridInterface.SetItemText(j+2, nCol, _T("N"));
							}
						}
						/////////////////
						
					}
				}
				else if(nRow == 10)
				{
					if (m_nTsiteUse[1][nCol +3]  == NO)
					{
						dlgMsg.m_nMessageType	= 1;
						dlgMsg.m_strMessage.Format(_T("Right [%02d]Board Use?...."), nCol + 4);
						if (dlgMsg.DoModal() == IDOK)
						{
							m_nTsiteUse[1][nCol +3] = YES;
							strTemp.Format(_T("%d"),nCol + 4);
							m_pGridInterface.SetItemText(nRow, nCol,strTemp);
							m_pGridInterface.SetItemBkColour(nRow, nCol, RGB(219, 229, 241), BLACK_L);
							m_pGridInterface.SetItemFont(nRow, nCol, &clsFunc.OnLogFont(18));

							for(j =0; j< 8; j++ )
							{
								m_nTsiteSocketUse[1][nCol +3][j] = YES;
								m_pGridInterface.SetItemBkColour(j+11, nCol, YELLOW_C, BLACK_L);
								m_pGridInterface.SetItemFont(j+11, nCol, &clsFunc.OnLogFont(18));
								m_pGridInterface.SetItemText(j+11, nCol, _T("Y"));

								//kwlee 2017.0124
								st_test_site_info[THD_TESTSITE_5 + nCol - 1].st_pcb_info[j].nFailCount = 0;
							}
						}
					}
					else
					{

						dlgMsg.m_nMessageType	= 1;
						dlgMsg.m_strMessage.Format(_T("Left [%02d]Board Not Use?...."), nCol +4);

						if (dlgMsg.DoModal() == IDOK)
						{
							m_nTsiteUse[1][nCol +3] = NO;
							m_pGridInterface.SetItemBkColour(nRow, nCol, RGB(219, 229, 241), BLACK_L);
							m_pGridInterface.SetItemFont(nRow, nCol, &clsFunc.OnLogFont(18));
							//m_pGridInterface.SetItemText(nRow, nCol, _T("NOT USE"));


							for(j =0; j< 8; j++ )
							{
								m_nTsiteSocketUse[1][nCol +3][j] = NO;
								m_pGridInterface.SetItemBkColour(j+11, nCol, YELLOW_D, BLACK_L);
								m_pGridInterface.SetItemFont(j+11, nCol, &clsFunc.OnLogFont(18));
								m_pGridInterface.SetItemText(j+11, nCol, _T("N"));
							}
						}
						/////////////////////////
						
					}
				}	
			}
		}
	}

	m_pGridInterface.Invalidate(FALSE);
}


void CWorkRecipe::OnDegreeCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CDialog_Message dlgMsg;

	int nRow, nCol;
	CString strTemp;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	if (nRow <= 0) return;
	if (m_nDegree[nCol][1] == (nRow-1)) return;

	dlgMsg.m_nMessageType	= 1;
	switch (nRow-1)
	{
		case 0:
			strTemp = _T("CW");
			break;

		case 1:
			strTemp = _T("CCW");
			break;

// 		case 2:
// 			strTemp = _T("180'");
// 			break;
// 
// 		case 3:
// 			strTemp = _T("270'");
// 			break;
	}
	dlgMsg.m_strMessage.Format(_T("Do you want a select [%s] degree?"), strTemp);

	if (dlgMsg.DoModal() == IDOK)
	{
		m_pGridDegree.SetItemBkColour(1, nCol, GREEN_D, BLACK_C);
		m_pGridDegree.SetItemBkColour(2, nCol, GREEN_D, BLACK_C);
		//kwlee 2016.0809
// 		m_pGridDegree.SetItemBkColour(3, nCol, GREEN_D, BLACK_C);
// 		m_pGridDegree.SetItemBkColour(4, nCol, GREEN_D, BLACK_C);

		m_nDegree[nCol][1] = nRow-1;
		m_pGridDegree.SetItemBkColour(nRow, nCol, GREEN_C, BLACK_C);
	}

	m_pGridDegree.Invalidate(FALSE);
}

void CWorkRecipe::OnInitGridLeftInterface()
{
	int   i, j;
	int	  nMaxRow, nMaxCol;
	int   nRow;
	CString strTmp;

	nMaxRow = 5; // unloader tray y count
	nMaxCol = 13;

	m_pGridLfInterface.SetFrameFocusCell(FALSE);
	m_pGridLfInterface.SetTrackFocusCell(FALSE);
	m_pGridLfInterface.EnableSelection(FALSE);

	m_pGridLfInterface.SetGridLineColor(BLACK_C);
	m_pGridLfInterface.SetGridLines(1);

	m_pGridLfInterface.SetRowCount(nMaxRow);
	m_pGridLfInterface.SetColumnCount(nMaxCol);

	m_pGridLfInterface.SetFixedRowCount(0);
	m_pGridLfInterface.SetFixedColumnCount(0);
	m_pGridLfInterface.SetFixedBkColor(RGB(0,0,200));
	m_pGridLfInterface.SetFixedBkColor(RGB(200,200,255));
	m_pGridLfInterface.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<nMaxRow; i++) 
	{
		m_pGridLfInterface.SetRowHeight(i, 27);

		for (j=0; j<nMaxCol; j++) 
		{
			m_pGridLfInterface.SetColumnWidth(j, 35);

			m_pGridLfInterface.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridLfInterface.SetItemState(i, j, GVIS_READONLY);
			m_pGridLfInterface.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
		}
	}

	nRow = 0;
	m_pGridLfInterface.MergeCells(nRow, 0, nRow, 12);
	m_pGridLfInterface.SetItemBkColour(nRow, 0, BLUE_D, WHITE_C);
	m_pGridLfInterface.SetItemFont(nRow, 0, &clsFunc.OnLogFont(18));
	m_pGridLfInterface.SetItemText(nRow, 0, _T("Left Interface"));

	nRow = 1;
	m_pGridLfInterface.SetItemBkColour(nRow, 0, RGB(219, 229, 241), BLACK_L);
	m_pGridLfInterface.SetItemFont(nRow, 0, &clsFunc.OnLogFont(18));

	m_pGridLfInterface.SetItemBkColour(nRow, 1, RGB(219, 229, 241), BLACK_L);
	m_pGridLfInterface.SetItemFont(nRow, 1, &clsFunc.OnLogFont(18));
	m_pGridLfInterface.SetItemText(nRow, 1, _T("13"));

	m_pGridLfInterface.SetItemBkColour(nRow, 2, RGB(219, 229, 241), BLACK_L);
	m_pGridLfInterface.SetItemFont(nRow, 2, &clsFunc.OnLogFont(18));
	m_pGridLfInterface.SetItemText(nRow, 2, _T("14"));

	m_pGridLfInterface.SetItemBkColour(nRow, 3, RGB(219, 229, 241), BLACK_L);
	m_pGridLfInterface.SetItemFont(nRow, 3, &clsFunc.OnLogFont(18));
	m_pGridLfInterface.SetItemText(nRow, 3, _T("15"));

	m_pGridLfInterface.SetItemBkColour(nRow, 4, RGB(219, 229, 241), BLACK_L);
	m_pGridLfInterface.SetItemFont(nRow, 4, &clsFunc.OnLogFont(18));
	m_pGridLfInterface.SetItemText(nRow, 4, _T("16"));

	m_pGridLfInterface.SetItemBkColour(nRow, 5, RGB(219, 229, 241), BLACK_L);
	m_pGridLfInterface.SetItemFont(nRow, 5, &clsFunc.OnLogFont(18));
	m_pGridLfInterface.SetItemText(nRow, 5, _T("17"));

	m_pGridLfInterface.SetItemBkColour(nRow, 6, RGB(219, 229, 241), BLACK_L);
	m_pGridLfInterface.SetItemFont(nRow, 6, &clsFunc.OnLogFont(18));
	m_pGridLfInterface.SetItemText(nRow, 6, _T("18"));

	m_pGridLfInterface.SetItemBkColour(nRow, 7, RGB(219, 229, 241), BLACK_L);
	m_pGridLfInterface.SetItemFont(nRow, 7, &clsFunc.OnLogFont(18));
	m_pGridLfInterface.SetItemText(nRow, 7, _T("19"));

	m_pGridLfInterface.SetItemBkColour(nRow, 8, RGB(219, 229, 241), BLACK_L);
	m_pGridLfInterface.SetItemFont(nRow, 8, &clsFunc.OnLogFont(18));
	m_pGridLfInterface.SetItemText(nRow, 8, _T("20"));

	m_pGridLfInterface.SetItemBkColour(nRow, 9, RGB(219, 229, 241), BLACK_L);
	m_pGridLfInterface.SetItemFont(nRow, 9, &clsFunc.OnLogFont(18));
	m_pGridLfInterface.SetItemText(nRow, 9, _T("21"));

	m_pGridLfInterface.SetItemBkColour(nRow, 10, RGB(219, 229, 241), BLACK_L);
	m_pGridLfInterface.SetItemFont(nRow, 10, &clsFunc.OnLogFont(18));
	m_pGridLfInterface.SetItemText(nRow, 10, _T("22"));

	m_pGridLfInterface.SetItemBkColour(nRow, 11, RGB(219, 229, 241), BLACK_L);
	m_pGridLfInterface.SetItemFont(nRow, 11, &clsFunc.OnLogFont(18));
	m_pGridLfInterface.SetItemText(nRow, 11, _T("23"));

	m_pGridLfInterface.SetItemBkColour(nRow, 12, RGB(219, 229, 241), BLACK_L);
	m_pGridLfInterface.SetItemFont(nRow, 12, &clsFunc.OnLogFont(18));
	m_pGridLfInterface.SetItemText(nRow, 12, _T("24"));

	nRow = 2;
	m_pGridLfInterface.SetItemBkColour(nRow, 0, RGB(219, 229, 241), BLACK_L);
	m_pGridLfInterface.SetItemFont(nRow, 0, &clsFunc.OnLogFont(18));
	m_pGridLfInterface.SetItemText(nRow, 0, _T("T"));

	nRow = 3;
	m_pGridLfInterface.SetItemBkColour(nRow, 0, RGB(219, 229, 241), BLACK_L);
	m_pGridLfInterface.SetItemFont(nRow, 0, &clsFunc.OnLogFont(18));

	m_pGridLfInterface.SetItemBkColour(nRow, 1, RGB(219, 229, 241), BLACK_L);
	m_pGridLfInterface.SetItemFont(nRow, 1, &clsFunc.OnLogFont(18));
	m_pGridLfInterface.SetItemText(nRow, 1, _T("1"));

	m_pGridLfInterface.SetItemBkColour(nRow, 2, RGB(219, 229, 241), BLACK_L);
	m_pGridLfInterface.SetItemFont(nRow, 2, &clsFunc.OnLogFont(18));
	m_pGridLfInterface.SetItemText(nRow, 2, _T("2"));

	m_pGridLfInterface.SetItemBkColour(nRow, 3, RGB(219, 229, 241), BLACK_L);
	m_pGridLfInterface.SetItemFont(nRow, 3, &clsFunc.OnLogFont(18));
	m_pGridLfInterface.SetItemText(nRow, 3, _T("3"));

	m_pGridLfInterface.SetItemBkColour(nRow, 4, RGB(219, 229, 241), BLACK_L);
	m_pGridLfInterface.SetItemFont(nRow, 4, &clsFunc.OnLogFont(18));
	m_pGridLfInterface.SetItemText(nRow, 4, _T("4"));

	m_pGridLfInterface.SetItemBkColour(nRow, 5, RGB(219, 229, 241), BLACK_L);
	m_pGridLfInterface.SetItemFont(nRow, 5, &clsFunc.OnLogFont(18));
	m_pGridLfInterface.SetItemText(nRow, 5, _T("5"));

	m_pGridLfInterface.SetItemBkColour(nRow, 6, RGB(219, 229, 241), BLACK_L);
	m_pGridLfInterface.SetItemFont(nRow, 6, &clsFunc.OnLogFont(18));
	m_pGridLfInterface.SetItemText(nRow, 6, _T("6"));

	m_pGridLfInterface.SetItemBkColour(nRow, 7, RGB(219, 229, 241), BLACK_L);
	m_pGridLfInterface.SetItemFont(nRow, 7, &clsFunc.OnLogFont(18));
	m_pGridLfInterface.SetItemText(nRow, 7, _T("7"));

	m_pGridLfInterface.SetItemBkColour(nRow, 8, RGB(219, 229, 241), BLACK_L);
	m_pGridLfInterface.SetItemFont(nRow, 8, &clsFunc.OnLogFont(18));
	m_pGridLfInterface.SetItemText(nRow, 8, _T("8"));

	m_pGridLfInterface.SetItemBkColour(nRow, 9, RGB(219, 229, 241), BLACK_L);
	m_pGridLfInterface.SetItemFont(nRow, 9, &clsFunc.OnLogFont(18));
	m_pGridLfInterface.SetItemText(nRow, 9, _T("9"));

	m_pGridLfInterface.SetItemBkColour(nRow, 10, RGB(219, 229, 241), BLACK_L);
	m_pGridLfInterface.SetItemFont(nRow, 10, &clsFunc.OnLogFont(18));
	m_pGridLfInterface.SetItemText(nRow, 10, _T("10"));

	m_pGridLfInterface.SetItemBkColour(nRow, 11, RGB(219, 229, 241), BLACK_L);
	m_pGridLfInterface.SetItemFont(nRow, 11, &clsFunc.OnLogFont(18));
	m_pGridLfInterface.SetItemText(nRow, 11, _T("11"));

	m_pGridLfInterface.SetItemBkColour(nRow, 12, RGB(219, 229, 241), BLACK_L);
	m_pGridLfInterface.SetItemFont(nRow, 12, &clsFunc.OnLogFont(18));
	m_pGridLfInterface.SetItemText(nRow, 12, _T("12"));

	nRow = 4;
	m_pGridLfInterface.SetItemBkColour(nRow, 0, RGB(219, 229, 241), BLACK_L);
	m_pGridLfInterface.SetItemFont(nRow, 0, &clsFunc.OnLogFont(18));
	m_pGridLfInterface.SetItemText(nRow, 0, _T("B"));

// 	for (i=0; i<2; i++)
// 	{
// 		for (j=0; j<12; j++)
// 		{
// 			if (m_nTsiteUse[1][0][i][j] == YES)
// 			{
// 				m_pGridLfInterface.SetItemBkColour(4-(i*2), j + 1, YELLOW_C, BLACK_L);
// 				m_pGridLfInterface.SetItemFont(4-(i*2), j + 1, &clsFunc.OnLogFont(18));
// 				m_pGridLfInterface.SetItemText(4-(i*2), j + 1, _T("Y"));
// 			}
// 			else
// 			{
// 				m_pGridLfInterface.SetItemBkColour(4-(i*2), j + 1, YELLOW_D, BLACK_L);
// 				m_pGridLfInterface.SetItemFont(4-(i*2), j + 1, &clsFunc.OnLogFont(18));
// 				m_pGridLfInterface.SetItemText(4-(i*2), j + 1, _T("N"));
// 			}
// 		}
// 	}

	m_pGridLfInterface.Invalidate(FALSE);
}

void CWorkRecipe::OnInitGridRightInterface()
{
	int   i, j;
	int	  nMaxRow, nMaxCol;
	int   nRow;
	CString strTmp;

	nMaxRow = 5; // unloader tray y count
	nMaxCol = 13;

	m_pGridRiInterface.SetFrameFocusCell(FALSE);
	m_pGridRiInterface.SetTrackFocusCell(FALSE);
	m_pGridRiInterface.EnableSelection(FALSE);

	m_pGridRiInterface.SetGridLineColor(BLACK_C);
	m_pGridRiInterface.SetGridLines(1);

	m_pGridRiInterface.SetRowCount(nMaxRow);
	m_pGridRiInterface.SetColumnCount(nMaxCol);

	m_pGridRiInterface.SetFixedRowCount(0);
	m_pGridRiInterface.SetFixedColumnCount(0);
	m_pGridRiInterface.SetFixedBkColor(RGB(0,0,200));
	m_pGridRiInterface.SetFixedBkColor(RGB(200,200,255));
	m_pGridRiInterface.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<nMaxRow; i++) 
	{
		m_pGridRiInterface.SetRowHeight(i, 27);

		for (j=0; j<nMaxCol; j++) 
		{
			m_pGridRiInterface.SetColumnWidth(j, 35);

			m_pGridRiInterface.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridRiInterface.SetItemState(i, j, GVIS_READONLY);
			m_pGridRiInterface.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
		}
	}

	nRow = 0;
	m_pGridRiInterface.MergeCells(nRow, 0, nRow, 12);
	m_pGridRiInterface.SetItemBkColour(nRow, 0, BLUE_D, WHITE_C);
	m_pGridRiInterface.SetItemFont(nRow, 0, &clsFunc.OnLogFont(18));
	m_pGridRiInterface.SetItemText(nRow, 0, _T("Right Interface"));

	nRow = 1;
	m_pGridRiInterface.SetItemBkColour(nRow, 0, RGB(219, 229, 241), BLACK_L);
	m_pGridRiInterface.SetItemFont(nRow, 0, &clsFunc.OnLogFont(18));

	m_pGridRiInterface.SetItemBkColour(nRow, 1, RGB(219, 229, 241), BLACK_L);
	m_pGridRiInterface.SetItemFont(nRow, 1, &clsFunc.OnLogFont(18));
	m_pGridRiInterface.SetItemText(nRow, 1, _T("37"));

	m_pGridRiInterface.SetItemBkColour(nRow, 2, RGB(219, 229, 241), BLACK_L);
	m_pGridRiInterface.SetItemFont(nRow, 2, &clsFunc.OnLogFont(18));
	m_pGridRiInterface.SetItemText(nRow, 2, _T("38"));

	m_pGridRiInterface.SetItemBkColour(nRow, 3, RGB(219, 229, 241), BLACK_L);
	m_pGridRiInterface.SetItemFont(nRow, 3, &clsFunc.OnLogFont(18));
	m_pGridRiInterface.SetItemText(nRow, 3, _T("39"));

	m_pGridRiInterface.SetItemBkColour(nRow, 4, RGB(219, 229, 241), BLACK_L);
	m_pGridRiInterface.SetItemFont(nRow, 4, &clsFunc.OnLogFont(18));
	m_pGridRiInterface.SetItemText(nRow, 4, _T("40"));

	m_pGridRiInterface.SetItemBkColour(nRow, 5, RGB(219, 229, 241), BLACK_L);
	m_pGridRiInterface.SetItemFont(nRow, 5, &clsFunc.OnLogFont(18));
	m_pGridRiInterface.SetItemText(nRow, 5, _T("41"));

	m_pGridRiInterface.SetItemBkColour(nRow, 6, RGB(219, 229, 241), BLACK_L);
	m_pGridRiInterface.SetItemFont(nRow, 6, &clsFunc.OnLogFont(18));
	m_pGridRiInterface.SetItemText(nRow, 6, _T("42"));

	m_pGridRiInterface.SetItemBkColour(nRow, 7, RGB(219, 229, 241), BLACK_L);
	m_pGridRiInterface.SetItemFont(nRow, 7, &clsFunc.OnLogFont(18));
	m_pGridRiInterface.SetItemText(nRow, 7, _T("43"));

	m_pGridRiInterface.SetItemBkColour(nRow, 8, RGB(219, 229, 241), BLACK_L);
	m_pGridRiInterface.SetItemFont(nRow, 8, &clsFunc.OnLogFont(18));
	m_pGridRiInterface.SetItemText(nRow, 8, _T("44"));

	m_pGridRiInterface.SetItemBkColour(nRow, 9, RGB(219, 229, 241), BLACK_L);
	m_pGridRiInterface.SetItemFont(nRow, 9, &clsFunc.OnLogFont(18));
	m_pGridRiInterface.SetItemText(nRow, 9, _T("45"));

	m_pGridRiInterface.SetItemBkColour(nRow, 10, RGB(219, 229, 241), BLACK_L);
	m_pGridRiInterface.SetItemFont(nRow, 10, &clsFunc.OnLogFont(18));
	m_pGridRiInterface.SetItemText(nRow, 10, _T("46"));

	m_pGridRiInterface.SetItemBkColour(nRow, 11, RGB(219, 229, 241), BLACK_L);
	m_pGridRiInterface.SetItemFont(nRow, 11, &clsFunc.OnLogFont(18));
	m_pGridRiInterface.SetItemText(nRow, 11, _T("47"));

	m_pGridRiInterface.SetItemBkColour(nRow, 12, RGB(219, 229, 241), BLACK_L);
	m_pGridRiInterface.SetItemFont(nRow, 12, &clsFunc.OnLogFont(18));
	m_pGridRiInterface.SetItemText(nRow, 12, _T("48"));

	nRow = 2;
	m_pGridRiInterface.SetItemBkColour(nRow, 0, RGB(219, 229, 241), BLACK_L);
	m_pGridRiInterface.SetItemFont(nRow, 0, &clsFunc.OnLogFont(18));
	m_pGridRiInterface.SetItemText(nRow, 0, _T("T"));

	nRow = 3;
	m_pGridRiInterface.SetItemBkColour(nRow, 0, RGB(219, 229, 241), BLACK_L);
	m_pGridRiInterface.SetItemFont(nRow, 0, &clsFunc.OnLogFont(18));

	m_pGridRiInterface.SetItemBkColour(nRow, 1, RGB(219, 229, 241), BLACK_L);
	m_pGridRiInterface.SetItemFont(nRow, 1, &clsFunc.OnLogFont(18));
	m_pGridRiInterface.SetItemText(nRow, 1, _T("25"));

	m_pGridRiInterface.SetItemBkColour(nRow, 2, RGB(219, 229, 241), BLACK_L);
	m_pGridRiInterface.SetItemFont(nRow, 2, &clsFunc.OnLogFont(18));
	m_pGridRiInterface.SetItemText(nRow, 2, _T("26"));

	m_pGridRiInterface.SetItemBkColour(nRow, 3, RGB(219, 229, 241), BLACK_L);
	m_pGridRiInterface.SetItemFont(nRow, 3, &clsFunc.OnLogFont(18));
	m_pGridRiInterface.SetItemText(nRow, 3, _T("27"));

	m_pGridRiInterface.SetItemBkColour(nRow, 4, RGB(219, 229, 241), BLACK_L);
	m_pGridRiInterface.SetItemFont(nRow, 4, &clsFunc.OnLogFont(18));
	m_pGridRiInterface.SetItemText(nRow, 4, _T("28"));

	m_pGridRiInterface.SetItemBkColour(nRow, 5, RGB(219, 229, 241), BLACK_L);
	m_pGridRiInterface.SetItemFont(nRow, 5, &clsFunc.OnLogFont(18));
	m_pGridRiInterface.SetItemText(nRow, 5, _T("29"));

	m_pGridRiInterface.SetItemBkColour(nRow, 6, RGB(219, 229, 241), BLACK_L);
	m_pGridRiInterface.SetItemFont(nRow, 6, &clsFunc.OnLogFont(18));
	m_pGridRiInterface.SetItemText(nRow, 6, _T("30"));

	m_pGridRiInterface.SetItemBkColour(nRow, 7, RGB(219, 229, 241), BLACK_L);
	m_pGridRiInterface.SetItemFont(nRow, 7, &clsFunc.OnLogFont(18));
	m_pGridRiInterface.SetItemText(nRow, 7, _T("31"));

	m_pGridRiInterface.SetItemBkColour(nRow, 8, RGB(219, 229, 241), BLACK_L);
	m_pGridRiInterface.SetItemFont(nRow, 8, &clsFunc.OnLogFont(18));
	m_pGridRiInterface.SetItemText(nRow, 8, _T("32"));

	m_pGridRiInterface.SetItemBkColour(nRow, 9, RGB(219, 229, 241), BLACK_L);
	m_pGridRiInterface.SetItemFont(nRow, 9, &clsFunc.OnLogFont(18));
	m_pGridRiInterface.SetItemText(nRow, 9, _T("33"));

	m_pGridRiInterface.SetItemBkColour(nRow, 10, RGB(219, 229, 241), BLACK_L);
	m_pGridRiInterface.SetItemFont(nRow, 10, &clsFunc.OnLogFont(18));
	m_pGridRiInterface.SetItemText(nRow, 10, _T("34"));

	m_pGridRiInterface.SetItemBkColour(nRow, 11, RGB(219, 229, 241), BLACK_L);
	m_pGridRiInterface.SetItemFont(nRow, 11, &clsFunc.OnLogFont(18));
	m_pGridRiInterface.SetItemText(nRow, 11, _T("35"));

	m_pGridRiInterface.SetItemBkColour(nRow, 12, RGB(219, 229, 241), BLACK_L);
	m_pGridRiInterface.SetItemFont(nRow, 12, &clsFunc.OnLogFont(18));
	m_pGridRiInterface.SetItemText(nRow, 12, _T("36"));

	nRow = 4;
	m_pGridRiInterface.SetItemBkColour(nRow, 0, RGB(219, 229, 241), BLACK_L);
	m_pGridRiInterface.SetItemFont(nRow, 0, &clsFunc.OnLogFont(18));
	m_pGridRiInterface.SetItemText(nRow, 0, _T("B"));

// 	for (i=0; i<2; i++)
// 	{
// 		for (j=0; j<12; j++)
// 		{
// 			if (m_nTsiteUse[1][1][i][j] == YES)
// 			{
// 				m_pGridRiInterface.SetItemBkColour(4-(i*2), j + 1, YELLOW_C, BLACK_L);
// 				m_pGridRiInterface.SetItemFont(4-(i*2), j + 1, &clsFunc.OnLogFont(18));
// 				m_pGridRiInterface.SetItemText(4-(i*2), j + 1, _T("Y"));
// 			}
// 			else
// 			{
// 				m_pGridRiInterface.SetItemBkColour(4-(i*2), j + 1, YELLOW_D, BLACK_L);
// 				m_pGridRiInterface.SetItemFont(4-(i*2), j + 1, &clsFunc.OnLogFont(18));
// 				m_pGridRiInterface.SetItemText(4-(i*2), j + 1, _T("N"));
// 			}
// 		}
// 	}

	m_pGridRiInterface.Invalidate(FALSE);
}
//kwlee 2016.0729
void CWorkRecipe::OnInitGridInterface()
{
	int   i, j;
	int	  nMaxRow, nMaxCol;
	int   nRow;
	CString strTmp;

	nMaxRow = 19; // unloader tray y count
	nMaxCol = 5;

	m_pGridInterface.SetFrameFocusCell(FALSE);
	m_pGridInterface.SetTrackFocusCell(FALSE);
	m_pGridInterface.EnableSelection(FALSE);

	m_pGridInterface.SetGridLineColor(BLACK_C);
	m_pGridInterface.SetGridLines(1);

	m_pGridInterface.SetRowCount(nMaxRow);
	m_pGridInterface.SetColumnCount(nMaxCol);

	m_pGridInterface.SetFixedRowCount(0);
	m_pGridInterface.SetFixedColumnCount(0);
	m_pGridInterface.SetFixedBkColor(RGB(0,0,200));
	m_pGridInterface.SetFixedBkColor(RGB(200,200,255));
	m_pGridInterface.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<nMaxRow; i++) 
	{
		m_pGridInterface.SetRowHeight(i, 19);

		for (j=0; j<nMaxCol; j++) 
		{
			m_pGridInterface.SetColumnWidth(j, 79);

			m_pGridInterface.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridInterface.SetItemState(i, j, GVIS_READONLY);
			m_pGridInterface.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
		}
	}

	nRow = 0;
	m_pGridInterface.MergeCells(nRow, 0, nRow,4);
	m_pGridInterface.SetItemBkColour(nRow, 0, BLUE_D, WHITE_C);
	m_pGridInterface.SetItemFont(nRow, 0, &clsFunc.OnLogFont(18));
	m_pGridInterface.SetItemText(nRow, 0, _T("TestSite Interface"));

	nRow = 1;
	m_pGridInterface.SetItemBkColour(nRow, 0, RGB(219, 229, 241), BLACK_L);
	m_pGridInterface.SetItemFont(nRow, 0, &clsFunc.OnLogFont(18));

	m_pGridInterface.SetItemBkColour(nRow, 1, RGB(219, 229, 241), BLACK_L);
	m_pGridInterface.SetItemFont(nRow, 1, &clsFunc.OnLogFont(18));
	m_pGridInterface.SetItemText(nRow, 1, _T("1"));

	m_pGridInterface.SetItemBkColour(nRow, 2, RGB(219, 229, 241), BLACK_L);
	m_pGridInterface.SetItemFont(nRow, 2, &clsFunc.OnLogFont(18));
	m_pGridInterface.SetItemText(nRow, 2, _T("2"));

	m_pGridInterface.SetItemBkColour(nRow, 3, RGB(219, 229, 241), BLACK_L);
	m_pGridInterface.SetItemFont(nRow, 3, &clsFunc.OnLogFont(18));
	m_pGridInterface.SetItemText(nRow, 3, _T("3"));

	m_pGridInterface.SetItemBkColour(nRow, 4, RGB(219, 229, 241), BLACK_L);
	m_pGridInterface.SetItemFont(nRow, 4, &clsFunc.OnLogFont(18));
	m_pGridInterface.SetItemText(nRow, 4, _T("4"));

	m_pGridInterface.SetItemBkColour(nRow, 0, RGB(219, 229, 241), BLACK_L);
	m_pGridInterface.SetItemFont(nRow, 0, &clsFunc.OnLogFont(18));
	m_pGridInterface.SetItemText(nRow, 0, _T("LEFT"));

	nRow = 2;
	m_pGridInterface.SetItemBkColour(nRow, 0, BLUE_D, WHITE_C);
	m_pGridInterface.SetItemFont(nRow, 0, &clsFunc.OnLogFont(18));
	m_pGridInterface.SetItemText(nRow, 0, _T("8"));

	nRow = 3;
	m_pGridInterface.SetItemBkColour(nRow, 0, BLUE_D, WHITE_C);
	m_pGridInterface.SetItemFont(nRow, 0, &clsFunc.OnLogFont(18));
	m_pGridInterface.SetItemText(nRow, 0, _T("7"));

	nRow = 4;
	m_pGridInterface.SetItemBkColour(nRow, 0, BLUE_D, WHITE_C);
	m_pGridInterface.SetItemFont(nRow, 0, &clsFunc.OnLogFont(18));
	m_pGridInterface.SetItemText(nRow, 0, _T("6"));

	nRow = 5;
	m_pGridInterface.SetItemBkColour(nRow, 0, BLUE_D, WHITE_C);
	m_pGridInterface.SetItemFont(nRow, 0, &clsFunc.OnLogFont(18));
	m_pGridInterface.SetItemText(nRow, 0, _T("5"));

	nRow = 6;
	m_pGridInterface.SetItemBkColour(nRow, 0, BLUE_D, WHITE_C);
	m_pGridInterface.SetItemFont(nRow, 0, &clsFunc.OnLogFont(18));
	m_pGridInterface.SetItemText(nRow, 0, _T("4"));

	nRow = 7;
	m_pGridInterface.SetItemBkColour(nRow, 0,BLUE_D, WHITE_C);
	m_pGridInterface.SetItemFont(nRow, 0, &clsFunc.OnLogFont(18));
	m_pGridInterface.SetItemText(nRow, 0, _T("3"));

	nRow = 8;
	m_pGridInterface.SetItemBkColour(nRow, 0, BLUE_D, WHITE_C);
	m_pGridInterface.SetItemFont(nRow, 0, &clsFunc.OnLogFont(18));
	m_pGridInterface.SetItemText(nRow, 0, _T("2"));

	nRow = 9;
	m_pGridInterface.SetItemBkColour(nRow, 0, BLUE_D, WHITE_C);
	m_pGridInterface.SetItemFont(nRow, 0, &clsFunc.OnLogFont(18));
	m_pGridInterface.SetItemText(nRow, 0, _T("1"));

	nRow = 10;

	m_pGridInterface.SetItemBkColour(nRow, 0, RGB(219, 229, 241), BLACK_L);
	m_pGridInterface.SetItemFont(nRow, 0, &clsFunc.OnLogFont(18));

	m_pGridInterface.SetItemBkColour(nRow,1 , RGB(219, 229, 241), BLACK_L);
	m_pGridInterface.SetItemFont(nRow, 1, &clsFunc.OnLogFont(18));
	m_pGridInterface.SetItemText(nRow, 1, _T("5"));

	m_pGridInterface.SetItemBkColour(nRow,2, RGB(219, 229, 241), BLACK_L);
	m_pGridInterface.SetItemFont(nRow, 2, &clsFunc.OnLogFont(18));
	m_pGridInterface.SetItemText(nRow, 2, _T("6"));

	m_pGridInterface.SetItemBkColour(nRow, 3, RGB(219, 229, 241), BLACK_L);
	m_pGridInterface.SetItemFont(nRow, 3, &clsFunc.OnLogFont(18));
	m_pGridInterface.SetItemText(nRow, 3, _T("7"));

	m_pGridInterface.SetItemBkColour(nRow, 4, RGB(219, 229, 241), BLACK_L);
	m_pGridInterface.SetItemFont(nRow, 4, &clsFunc.OnLogFont(18));
	m_pGridInterface.SetItemText(nRow, 4, _T("8"));

	m_pGridInterface.SetItemBkColour(nRow, 0, RGB(219, 229, 241), BLACK_L);
	m_pGridInterface.SetItemFont(nRow, 0, &clsFunc.OnLogFont(18));
	m_pGridInterface.SetItemText(nRow, 0, _T("RIGHT"));

	nRow = 11;
	m_pGridInterface.SetItemBkColour(nRow, 0, BLUE_D, WHITE_C);
	m_pGridInterface.SetItemFont(nRow, 0, &clsFunc.OnLogFont(18));
	m_pGridInterface.SetItemText(nRow, 0, _T("1"));

	nRow = 12;
	m_pGridInterface.SetItemBkColour(nRow, 0, BLUE_D, WHITE_C);
	m_pGridInterface.SetItemFont(nRow, 0, &clsFunc.OnLogFont(18));
	m_pGridInterface.SetItemText(nRow, 0, _T("2"));

	nRow = 13;
	m_pGridInterface.SetItemBkColour(nRow, 0, BLUE_D, WHITE_C);
	m_pGridInterface.SetItemFont(nRow, 0, &clsFunc.OnLogFont(18));
	m_pGridInterface.SetItemText(nRow, 0, _T("3"));

	nRow = 14;
	m_pGridInterface.SetItemBkColour(nRow, 0, BLUE_D, WHITE_C);
	m_pGridInterface.SetItemFont(nRow, 0, &clsFunc.OnLogFont(18));
	m_pGridInterface.SetItemText(nRow, 0, _T("4"));

	nRow = 15;
	m_pGridInterface.SetItemBkColour(nRow, 0, BLUE_D, WHITE_C);
	m_pGridInterface.SetItemFont(nRow, 0, &clsFunc.OnLogFont(18));
	m_pGridInterface.SetItemText(nRow, 0, _T("5"));

	nRow = 16;
	m_pGridInterface.SetItemBkColour(nRow, 0, BLUE_D, WHITE_C);
	m_pGridInterface.SetItemFont(nRow, 0, &clsFunc.OnLogFont(18));
	m_pGridInterface.SetItemText(nRow, 0, _T("6"));

	nRow = 17;
	m_pGridInterface.SetItemBkColour(nRow, 0, BLUE_D, WHITE_C);
	m_pGridInterface.SetItemFont(nRow, 0, &clsFunc.OnLogFont(18));
	m_pGridInterface.SetItemText(nRow, 0, _T("7"));

	nRow = 18;
	m_pGridInterface.SetItemBkColour(nRow, 0, BLUE_D, WHITE_C);
	m_pGridInterface.SetItemFont(nRow, 0, &clsFunc.OnLogFont(18));
	m_pGridInterface.SetItemText(nRow, 0, _T("8"));

	for (i=0; i<4; i++)
	{
		for (j=0; j<8; j++)
		{
			//Top
			if (m_nTsiteSocketUse[1][i][j] == YES)
			{
				m_pGridInterface.SetItemBkColour(9 -j, i + 1, YELLOW_C, BLACK_L);
				m_pGridInterface.SetItemFont(9 -j, i + 1, &clsFunc.OnLogFont(18));
				m_pGridInterface.SetItemText(9 -j, i + 1, _T("Y"));

				m_nTsiteUse[1][i] = YES; //kwlee 2016.1011
			}
			else
			{
				m_pGridInterface.SetItemBkColour(9 -j, i + 1, YELLOW_D, BLACK_L);
				m_pGridInterface.SetItemFont(9 -j, i + 1, &clsFunc.OnLogFont(18));
				m_pGridInterface.SetItemText(9 -j, i + 1, _T("N"));
				m_nTsiteUse[1][i] = NO; //kwlee 2016.1011
			}


			if (m_nTsiteSocketUse[1][i + 4][j] == YES)
			{
				m_pGridInterface.SetItemBkColour(j+11, i + 1, YELLOW_C, BLACK_L);
				m_pGridInterface.SetItemFont(j+11, i + 1, &clsFunc.OnLogFont(18));
				m_pGridInterface.SetItemText(j+11, i + 1, _T("Y"));
				m_nTsiteUse[1][i + 4] = YES; //kwlee 2016.1011
			}
			else
			{
				m_pGridInterface.SetItemBkColour(j+11, i + 1, YELLOW_D, BLACK_L);
				m_pGridInterface.SetItemFont(j+11, i + 1, &clsFunc.OnLogFont(18));
				m_pGridInterface.SetItemText(j+11, i + 1, _T("N"));
				m_nTsiteUse[1][i + 4] = NO; //kwlee 2016.1011
			}

			//Bottom
			// 			if (m_nTsiteUse[1][1][i][j] == YES)
			// 			{
			// 				m_pGridInterface.SetItemBkColour(i+11, j + 1, YELLOW_C, BLACK_L);
			// 				m_pGridInterface.SetItemFont(i+11, j + 1, &clsFunc.OnLogFont(18));
			// 				m_pGridInterface.SetItemText(i+11, j + 1, _T("Y"));
			// 			}
			// 			else
			// 			{
			// 				m_pGridInterface.SetItemBkColour(i+11, j + 1, YELLOW_D, BLACK_L);
			// 				m_pGridInterface.SetItemFont(i+11, j + 1, &clsFunc.OnLogFont(18));
			// 				m_pGridInterface.SetItemText(i+11, j + 1, _T("N"));
			// 			}
		}
	}
	m_pGridInterface.Invalidate(FALSE);
}

void CWorkRecipe::OnInitGridDegree()
{
	int   i, j;
	int	  nMaxRow, nMaxCol;
	int   nRow;
	CString strTmp;

	nMaxRow = 3; // unloader tray y count
	nMaxCol = 2;
	
	m_pGridDegree.SetFrameFocusCell(FALSE);
	m_pGridDegree.SetTrackFocusCell(FALSE);
	m_pGridDegree.EnableSelection(FALSE);

	m_pGridDegree.SetGridLineColor(BLACK_C);
	m_pGridDegree.SetGridLines(1);

	m_pGridDegree.SetRowCount(nMaxRow);
	m_pGridDegree.SetColumnCount(nMaxCol);

	m_pGridDegree.SetFixedRowCount(0);
	m_pGridDegree.SetFixedColumnCount(0);
	m_pGridDegree.SetFixedBkColor(RGB(0,0,200));
	m_pGridDegree.SetFixedBkColor(RGB(200,200,255));
	m_pGridDegree.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<nMaxRow; i++) 
	{
		if (i ==0)
		{
			m_pGridDegree.SetRowHeight(i, 30);
		}
		else
		{
			m_pGridDegree.SetRowHeight(i, 40);
		}
		
		for (j=0; j<nMaxCol; j++) 
		{
			m_pGridDegree.SetColumnWidth(j, 210);
			//m_pGridDegree.SetColumnWidth(j,98);

			m_pGridDegree.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridDegree.SetItemState(i, j, GVIS_READONLY);
			m_pGridDegree.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
		}
	}

	nRow = 0;
	m_pGridDegree.SetItemBkColour(nRow, 0, BLUE_D, WHITE_C);
	m_pGridDegree.SetItemFont(nRow, 0, &clsFunc.OnLogFont(17));
	m_pGridDegree.SetItemText(nRow, 0, _T("TestSite Left(1~4)"));

	m_pGridDegree.SetItemBkColour(nRow, 1, BLUE_D, WHITE_C);
	m_pGridDegree.SetItemFont(nRow, 1, &clsFunc.OnLogFont(17));
	m_pGridDegree.SetItemText(nRow, 1, _T("TestSite Right(5~8)"));

	nRow = 1;
	m_pGridDegree.SetItemBkColour(nRow, 0, GREEN_D, BLACK_C);
	m_pGridDegree.SetItemFont(nRow, 0, &clsFunc.OnLogFont(14));
	m_pGridDegree.SetItemText(nRow, 0, _T("CW"));

	m_pGridDegree.SetItemBkColour(nRow, 1, GREEN_D, BLACK_C);
	m_pGridDegree.SetItemFont(nRow, 1, &clsFunc.OnLogFont(14));
	m_pGridDegree.SetItemText(nRow, 1, _T("CW"));

	nRow = 2;
	m_pGridDegree.SetItemBkColour(nRow, 0, GREEN_D, BLACK_C);
	m_pGridDegree.SetItemFont(nRow, 0, &clsFunc.OnLogFont(14));
	m_pGridDegree.SetItemText(nRow, 0, _T("CCW"));

	m_pGridDegree.SetItemBkColour(nRow, 1, GREEN_D, BLACK_C);
	m_pGridDegree.SetItemFont(nRow, 1, &clsFunc.OnLogFont(14));
	m_pGridDegree.SetItemText(nRow, 1, _T("CCW"));

	m_pGridDegree.SetItemBkColour(m_nDegree[0][1] + 1, 0, GREEN_C, BLACK_C);
	m_pGridDegree.SetItemBkColour(m_nDegree[1][1] + 1, 1, GREEN_C, BLACK_C);
	//m_pGridDegree.SetItemBkColour(m_nDegree[2][1] + 1, 2, GREEN_C, BLACK_C); //kwlee 2016.0809

	m_pGridDegree.Invalidate(FALSE);
}




void CWorkRecipe::OnClickedDgtBufferRowOffset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	double dKey = m_dBufYOffset[1];

	KeyPadD(0.0f, 100.0f, &dKey);

	m_dBufYOffset[1] = dKey;
	m_dgtBufRowOffset.SetFloatVal((float)dKey);
}


void CWorkRecipe::OnClickedDgtBufferColOffset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	double dKey = m_dBufXOffset[1];

	KeyPadD(0.0f, 100.0f, &dKey);

	m_dBufXOffset[1] = dKey;
	m_dgtBufColOffset.SetFloatVal((float)dKey);
}


void CWorkRecipe::OnStnClickedDgtRejectTrayXCnt()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nKey = m_nRejectTrayXCnt[1];

	KeyPadI(1, 9, &nKey);

	m_nRejectTrayXCnt[1] = nKey;
	m_dgtRejectTrayXCnt.SetVal(nKey);
}


void CWorkRecipe::OnStnClickedDgtRetestTrayXCnt()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nKey = m_nRetestTrayXCnt[1];

	KeyPadI(1, 9, &nKey);

	m_nRetestTrayXCnt[1] = nKey;
	m_dgtRetestTrayXCnt.SetVal(nKey);
}


void CWorkRecipe::OnStnClickedDgtRejectFullOffset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	double dKey = m_dRejectFullOffset[1];

	KeyPadD(0.0f, 100.0f, &dKey);

	m_dRejectFullOffset[1] = dKey;
	m_dgtRejectFullOffset.SetFloatVal((float)dKey);
}


void CWorkRecipe::OnStnClickedDgtRejectEmptyOffset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	double dKey = m_dRejectEmptyOffset[1];

	KeyPadD(0.0f, 100.0f, &dKey);

	m_dRejectEmptyOffset[1] = dKey;
	m_dgtRejectEmptyOffset.SetFloatVal((float)dKey);
}


 

//2015.0406 james 적용이 안되어 추가 정리 
void CWorkRecipe::OnStnClickedDgtCokXOffset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	double dKey = m_dCokXOffset[1];

	KeyPadD(0.0f, 250.0f, &dKey);

	m_dCokXOffset[1] = dKey;
	m_dgtCokXOffset.SetFloatVal((float)dKey);
}

void CWorkRecipe::OnStnClickedDgtSocketCount()
{
	// TODO: ¿©±â¿¡ ÄÁÆ®·Ñ ¾Ë¸² Ã³¸®±â ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.
	int nKey = m_nSocketCount[1];

	KeyPadI(0, 8, &nKey);

	m_nSocketCount[1] = nKey;
	m_dgtSocketCount.SetVal(nKey);
}


void CWorkRecipe::OnStnClickedDgt15tOffset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_KeyPad dlgKeyPad;
	CString strTmp;
	double dVal;

	dlgKeyPad.m_nKeypadMode				= 1;
	dlgKeyPad.m_strKeypadLowLimit		= _T("-200");
	dlgKeyPad.m_strKeypadHighLimit		= _T("200");
	dlgKeyPad.m_strKeypadVal.Format(_T("%.3f"), m_d15TOffset[1]);

	if (dlgKeyPad.DoModal() == IDOK)
	{
		strTmp					= dlgKeyPad.m_strNewVal;
		dVal					= _wtof(strTmp);
		m_d15TOffset[1]			= dVal;

		m_dgt15TOffset.SetFloatVal((float)dVal);
	}

	double dKey = m_d15TOffset[1];

	KeyPadD(-200.f, 200.0f, &dKey);

	m_d15TOffset[1] = dKey;
	m_dgt15TOffset.SetFloatVal((float)dKey);
}


void CWorkRecipe::OnStnClickedDgtRetestBuffCnt()
{
	int nKey = m_nRetestBufferNum[1];

	KeyPadI(0, 15, &nKey);

	m_nRetestBufferNum[1] = nKey;
	m_dgtRetestBuffCnt.SetVal(nKey);
}


void CWorkRecipe::OnStnClickedDgtWorkBuffCnt()
{
	int nKey = m_nLdUldBufferNum[1];

	KeyPadI(0, 4, &nKey);

	m_nLdUldBufferNum[1] = nKey;
	m_dgtWorkBufferCnt.SetVal(nKey);
}


void CWorkRecipe::OnStnClickedDgtWorkPickerPitch()
{
	double dKey = m_dLdUldPickerPitch[1];

	KeyPadD(0.0f, 110.0f, &dKey);

	m_dLdUldPickerPitch[1] = dKey;
	m_dgtWorkPickerPitch.SetFloatVal((float)dKey);
}


void CWorkRecipe::OnStnClickedDgtTestPickerPitch()
{
	double dKey = m_dTestPickerPitch[1];

	KeyPadD(0.0f, 110.0f, &dKey);

	m_dTestPickerPitch[1] = dKey;
	m_dgtTestPickerPitch.SetFloatVal((float)dKey);
}


void CWorkRecipe::OnStnClickedDgtWorkBuffPitch()
{
	double dKey = m_dLdUldBufferPitch[1];

	KeyPadD(0.0f, 110.0f, &dKey);

	m_dLdUldBufferPitch[1] = dKey;
	m_dgtWorkBuffPitch.SetFloatVal((float)dKey);
}


void CWorkRecipe::OnStnClickedDgtRetestBuffPitch()
{
	double dKey = m_dRetestBufferPitch[1];

	KeyPadD(0.0f, 110.0f, &dKey);

	m_dRetestBufferPitch[1] = dKey;
	m_dgtRetestBuffPitch.SetFloatVal((float)dKey);
}


void CWorkRecipe::OnStnClickedDgtTrayPitchX()
{
	double dKey = m_dTrayPitchX[1];

	KeyPadD(0.0f, 1000.0f, &dKey);

	m_dTrayPitchX[1] = dKey;
	m_dgtTrayPitchX.SetFloatVal((float)dKey);
}


void CWorkRecipe::OnStnClickedDgtTrayPitchY()
{
	double dKey = m_dTrayPitchY[1];

	KeyPadD(0.0f, 1000.0f, &dKey);

	m_dTrayPitchY[1] = dKey;
	m_dgtTrayPitchY.SetFloatVal((float)dKey);
}



void CWorkRecipe::OnStnClickedDgtVisionYOffset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	double dKey = m_dVisionYOffset[1];

	KeyPadD(0.0f, 1000.0f, &dKey);

	m_dVisionYOffset[1] = dKey;
	m_dgtVisionYOffset.SetFloatVal((float)dKey);
}


void CWorkRecipe::OnStnClickedDgtVisionXOffset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	double dKey = m_dVisionXOffset[1];

	KeyPadD(0.0f, 1000.0f, &dKey);

	m_dVisionXOffset[1] = dKey;
	m_dgtVisionXOffset.SetFloatVal((float)dKey);
}
