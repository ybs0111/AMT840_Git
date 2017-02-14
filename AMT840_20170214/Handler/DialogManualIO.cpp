// DialogManualIO.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "DialogManualIO.h"
#include "afxdialogex.h"
#include "Variable.h"
#include "PublicFunction.h"
#include "Dialog_KeyBoard.h"
#include "Dialog_KeyPad.h"
#include "Dialog_Message.h"
#include "Ctlbd_Variable.h"
#include "Dialog_Password_Change.h"
#include "ComizoaPublic.h"
#include "FastechPublic_IO.h"

// CDialogManualIO 대화 상자입니다.
#define TM_IO		100
IMPLEMENT_DYNAMIC(CDialogManualIO, CDialog)

CDialogManualIO::CDialogManualIO(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogManualIO::IDD, pParent)
{
	m_bmGround.LoadBitmap(IDB_BITMAP_BACK_GROUND);   

	m_nTsiteNum = 0;
}

CDialogManualIO::~CDialogManualIO()
{
}

void CDialogManualIO::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_CUSTOM_IO_FUNCTION_TSITE,		m_pGridTsite);
	///DDX_Control(pDX, IDC_CUSTOM_IO_FUNCTION_TSITE_RBT,	m_pGridTRbt);
	//DDX_Control(pDX, IDC_CUSTOM_IO_FUNCTION_LD_RBT,		m_pGridLdRbt);
	//DDX_Control(pDX, IDC_CUSTOM_IO_FUNCTION_ULD_RBT,	m_pGridUldRbt);
//	DDX_Control(pDX, IDC_CUSTOM_IO_FUNCTION_LF_CONTACT,	m_pGridLfContact);
//	DDX_Control(pDX, IDC_CUSTOM_IO_FUNCTION_RI_CONTACT,	m_pGridRiContact);
	
	DDX_Control(pDX, IDC_BTN_EXIT,						m_btnExit);
	DDX_Control(pDX, IDC_BTN_REFRESH,					m_btnDoor);
	DDX_Control(pDX, IDC_BTN_LEFT_CLAMP,				m_btnLeftMpClamp);
	DDX_Control(pDX, IDC_BTN_LEFT_UNCLAMP,				m_btnLeftMpUnClamp);
	DDX_Control(pDX, IDC_BTN_RIGHT_CLAMP,				m_btnRightMpClamp);
	DDX_Control(pDX, IDC_BTN_RIGHT_UNCLAMP,				m_btnRightMpUnClamp);
	DDX_Control(pDX, IDC_CUSTOM_IO_FUNCTION_CONV, m_pGridConveyor);
	DDX_Control(pDX, IDC_CUSTOM_IO_FUNCTION_TRANSFER, m_pGridTransfer);
	DDX_Control(pDX, IDC_CUSTOM_IO_FUNCTION_STACKER, m_pGridStacker);
	DDX_Control(pDX, IDC_CUSTOM_IO_FUNCTION_ROBOT, m_pGridWorkRobot);
	DDX_Control(pDX, IDC_CUSTOM_IO_FUNCTION_BUFFER,		m_pGridBuf);
	DDX_Control(pDX, IDC_CUSTOM_IO_FUNCTION_TSITE_ROBOT, m_pGridTSiteRobot);
	DDX_Control(pDX, IDC_CUSTOM_IO_FUNCTION_TESTSITE, m_pGridTestSite);
}


BEGIN_MESSAGE_MAP(CDialogManualIO, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_IO_FUNCTION_TSITE,		&CDialogManualIO::OnTsiteCellClick)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_IO_FUNCTION_TSITE_RBT,	&CDialogManualIO::OnTRbtCellClick)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_IO_FUNCTION_LD_RBT,		&CDialogManualIO::OnLdRbtCellClick)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_IO_FUNCTION_ULD_RBT,		&CDialogManualIO::OnUldRbtCellClick)
	//ON_NOTIFY(NM_CLICK, IDC_CUSTOM_IO_FUNCTION_LF_CONTACT,	&CDialogManualIO::OnLeftContactCellClick)
	//ON_NOTIFY(NM_CLICK, IDC_CUSTOM_IO_FUNCTION_RI_CONTACT,	&CDialogManualIO::OnRightContactCellClick)
	//ON_NOTIFY(NM_CLICK, IDC_CUSTOM_IO_FUNCTION_BUFFER,		&CDialogManualIO::OnBufferCellClick)
	ON_BN_CLICKED(IDC_BTN_EXIT,								&CDialogManualIO::OnBnClickedBtnExit)
	ON_BN_CLICKED(IDC_BTN_REFRESH, &CDialogManualIO::OnBnClickedBtnRefresh)
	ON_BN_CLICKED(IDC_BTN_LEFT_CLAMP, &CDialogManualIO::OnBnClickedBtnLeftClamp)
	ON_BN_CLICKED(IDC_BTN_LEFT_UNCLAMP, &CDialogManualIO::OnBnClickedBtnLeftUnclamp)
	ON_BN_CLICKED(IDC_BTN_RIGHT_CLAMP, &CDialogManualIO::OnBnClickedBtnRightClamp)
	ON_BN_CLICKED(IDC_BTN_RIGHT_UNCLAMP, &CDialogManualIO::OnBnClickedBtnRightUnclamp)
END_MESSAGE_MAP()


// CDialogManualIO 메시지 처리기입니다.

BOOL CDialogManualIO::Create()
{
	return CDialog::Create( CDialogManualIO::IDD );
}

BOOL CDialogManualIO::OnEraseBkgnd(CDC* pDC)
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

//	return CFormView::OnEraseBkgnd(pDC);
	return true;
}


BOOL CDialogManualIO::DestroyWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	KillTimer(100);
	KillTimer(200);
	KillTimer(300);
	KillTimer(400);
	KillTimer(500);
	KillTimer(600);
	KillTimer(700);
	return CDialog::DestroyWindow();
}


BOOL CDialogManualIO::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	OnInitButton();

	OnInitGridConveyor();
	OnInitGridStacker();
// 	OnInitGridTransfer();
// 	OnInitGridLdRbt();
// 	OnInitGridBuffer();
// 	OnInitGridTsiteRbt();
// 	OnInitGridTsite();
	
	//OnInitGridTsite();
	//OnInitGridTsiteRbt();
	//OnInitGridLdRbt();
// 	OnInitGridUldRbt();
// 	OnInitGridLfContact();
// 	OnInitGridRiContact();
	//OnInitGridBuffer();

// 	SetTimer(100, 500, NULL);
// 	SetTimer(200, 500, NULL);
// 	SetTimer(300, 500, NULL);
// 	SetTimer(400, 500, NULL);
// 	SetTimer(500, 500, NULL);
// 	SetTimer(600, 500, NULL);
// 	SetTimer(700, 500, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDialogManualIO::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch (nIDEvent)
	{
		case 100:
			OnDisplayLdRobot();
			break;

// 		case 200:
// 			OnDisplayUnLdRobot();
// 			break;
// 
// 		case 300:
// 			OnDisplayTestSiteRobot();
// 			break;
// 
// 		case 400:
// 			OnDisplayLeftContact();
// 			break;
// 
// 		case 500:
// 			OnDisplayRightContact();
// 			break;

		case 600:
			OnDisplayTestSite();
			break;

		case 700:
			OnDisplayBuffer();
			break;
	}
	
	CDialog::OnTimer(nIDEvent);
}


//kwlee 2016.0819
void CDialogManualIO::OnInitGridConveyor()
{
	int   i, j;
	int	  nMaxRow, nMaxCol;
	 
	CString strTmp;
	 
	nMaxRow = 7; // unloader tray y count
	nMaxCol = 5;
	 
	m_pGridConveyor.SetFrameFocusCell(FALSE);
	m_pGridConveyor.SetTrackFocusCell(FALSE);
	m_pGridConveyor.EnableSelection(FALSE);
	 
	m_pGridConveyor.SetGridLineColor(BLACK_C);
	m_pGridConveyor.SetGridLines(1);
	 
	m_pGridConveyor.SetRowCount(nMaxRow);
	m_pGridConveyor.SetColumnCount(nMaxCol);
	 
	m_pGridConveyor.SetFixedRowCount(0);
	m_pGridConveyor.SetFixedColumnCount(0);
	m_pGridConveyor.SetFixedBkColor(RGB(0,0,200));
	m_pGridConveyor.SetFixedBkColor(RGB(200,200,255));
	m_pGridConveyor.SetTextBkColor(RGB(150,150,200));
	 
	for (i=0; i<nMaxRow; i++)
	{
	 	m_pGridConveyor.SetRowHeight(i, 35);
	 
	 	for (j=0; j<nMaxCol; j++)
	 	{
	 		if (j == 0)
	 		{
	 			m_pGridConveyor.SetColumnWidth(j, 95);
	 		}
	 		else 
	 		{
	 			m_pGridConveyor.SetColumnWidth(j, 70);
	 		}
	 
	 		m_pGridConveyor.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	 		m_pGridConveyor.SetItemState(i, j, GVIS_READONLY);
	 		m_pGridConveyor.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
	 	}
	}
	//Ld_1 Out / in
	m_nReadyLd1CvyStkStopUpDnCylOut = st_io_info.o_ReadyLd1CvyStkStopUpDnCyl;
	
	m_nReadyLd1CvyStopUpChkIn = st_io_info.i_ReadyLd1CvyStopUpChk;
	m_nReadyLd1CvyStopDnChkIn = st_io_info.i_ReadyLd1CvyStopDnChk;

	//Ld_2 Out /in
	m_nReadyLd2CvyStkStopUpDnCylOut = st_io_info.o_ReadyLd2CvyStkStopUpDnCyl;

	m_nReadyLd2CvyStopUpChkIn = st_io_info.i_ReadyLd2CvyStopUpChk;
	m_nReadyLd2CvyStopDnChkIn = st_io_info.i_ReadyLd2CvyStopDnChk;

	//Uld_1 Out / in
	m_nReadyUld1CvyStopCylOut = st_io_info.o_ReadyUld1CvyStopCyl;

	m_nReadyUld1CvyStopUpChkIn = st_io_info.i_ReadyUld1CvyStopUpChk;
	m_nReadyUld1CvyStopDnChkIn = st_io_info.i_ReadyUld1CvyStopDnChk;
		
	//Uld_2 Out / in
	m_nReadyUld2CvyStopCylOut = st_io_info.o_ReadyUld2CvyStopCyl;

	m_nReadyUld2CvyStopUpChkIn = st_io_info.i_ReadyUld2CvyStopUpChk;
	m_nReadyUld2CvyStopDnChkIn = st_io_info.i_ReadyUld2CvyStopDnChk;

	//Rej Out / In
	m_nRej1UldCvyStopUpDnCylOut = st_io_info.o_Rej1UldCvyStopUpDnCyl;

	m_nRej1UldCvyStopUpChkIn	=	st_io_info.i_Rej1UldCvyStopUpChk;
	m_nRej1UldCvyStopDnChkIn	=	st_io_info.i_Rej1UldCvyStopDnChk;

	//Rej Out / In
	m_nRej2UldCvyStopUpDnCylOut = st_io_info.o_Rej2UldCvyStopUpDnCyl;

	m_nRej2UldCvyStopUpChkIn	=	st_io_info.i_Rej2UldCvyStopUpChk;
	m_nRej2UldCvyStopDnChkIn	=	st_io_info.i_Rej2UldCvyStopDnChk;


	m_pGridConveyor.MergeCells(0, 0, 0, 4);
	m_pGridConveyor.SetItemBkColour(0, 0, BLUE_D, WHITE_C);
	m_pGridConveyor.SetItemFont(0, 0, &clsFunc.OnLogFont(17));
	m_pGridConveyor.SetItemText(0, 0, _T("Conveyor Stopper"));

	// LD_1 Conv
	m_pGridConveyor.SetItemBkColour(1, 0, YELLOW_L, BLACK_C);
	m_pGridConveyor.SetItemFont(1, 0, &clsFunc.OnLogFont(15));
	m_pGridConveyor.SetItemText(1, 0, _T("LD_1 Conv"));

	m_nLd1CvyStopUpDnOut[0]	= 1;
	m_nLd1CvyStopUpDnOut[1]	= 1;
	m_pGridConveyor.MergeCells(m_nLd1CvyStopUpDnOut[0], m_nLd1CvyStopUpDnOut[1], m_nLd1CvyStopUpDnOut[0], m_nLd1CvyStopUpDnOut[1] + 1);
	m_pGridConveyor.SetItemBkColour(m_nLd1CvyStopUpDnOut[0], m_nLd1CvyStopUpDnOut[1], GREEN_D, BLACK_C);
	m_pGridConveyor.SetItemText(m_nLd1CvyStopUpDnOut[0], m_nLd1CvyStopUpDnOut[1], _T("Up/Dn"));

	m_nLd1CvyStopUpIn[0]	= 1;
	m_nLd1CvyStopUpIn[1]	= 3;
	m_pGridConveyor.SetItemBkColour(m_nLd1CvyStopUpIn[0], m_nLd1CvyStopUpIn[1], YELLOW_D, BLACK_C);
	m_pGridConveyor.SetItemText(m_nLd1CvyStopUpIn[0], m_nLd1CvyStopUpIn[1], _T("Up Chk"));

	m_nLd1CvyStopDnIn[0]	= 1;
	m_nLd1CvyStopDnIn[1]	= 4;
	m_pGridConveyor.SetItemBkColour(m_nLd1CvyStopDnIn[0], m_nLd1CvyStopDnIn[1], YELLOW_D, BLACK_C);
	m_pGridConveyor.SetItemText(m_nLd1CvyStopDnIn[0], m_nLd1CvyStopDnIn[1], _T("Dn Chk"));


	// LD_2 Conv
	m_pGridConveyor.SetItemBkColour(2, 0, YELLOW_L, BLACK_C);
	m_pGridConveyor.SetItemFont(2, 0, &clsFunc.OnLogFont(15));
	m_pGridConveyor.SetItemText(2, 0, _T("LD_2 Conv"));

	m_nLd2CvyStopUpDnOut[0]	= 2;
	m_nLd2CvyStopUpDnOut[1]	= 1;
	m_pGridConveyor.MergeCells(m_nLd2CvyStopUpDnOut[0], m_nLd2CvyStopUpDnOut[1], m_nLd2CvyStopUpDnOut[0], m_nLd2CvyStopUpDnOut[1] + 1);
	m_pGridConveyor.SetItemBkColour(m_nLd2CvyStopUpDnOut[0], m_nLd2CvyStopUpDnOut[1], GREEN_D, BLACK_C);
	m_pGridConveyor.SetItemText(m_nLd2CvyStopUpDnOut[0], m_nLd2CvyStopUpDnOut[1], _T("Up/Dn"));

	m_nLd2CvyStopUpIn[0]	= 2;
	m_nLd2CvyStopUpIn[1]	= 3;
	m_pGridConveyor.SetItemBkColour(m_nLd2CvyStopUpIn[0], m_nLd2CvyStopUpIn[1], YELLOW_D, BLACK_C);
	m_pGridConveyor.SetItemText(m_nLd2CvyStopUpIn[0], m_nLd2CvyStopUpIn[1], _T("Up Chk"));

	m_nLd2CvyStopDnIn[0]	= 2;
	m_nLd2CvyStopDnIn[1]	= 4;
	m_pGridConveyor.SetItemBkColour(m_nLd2CvyStopDnIn[0], m_nLd2CvyStopDnIn[1], YELLOW_D, BLACK_C);
	m_pGridConveyor.SetItemText(m_nLd2CvyStopDnIn[0], m_nLd2CvyStopDnIn[1], _T("Dn Chk"));

	// Uld_1 Conv
	m_pGridConveyor.SetItemBkColour(3, 0, YELLOW_L, BLACK_C);
	m_pGridConveyor.SetItemFont(3, 0, &clsFunc.OnLogFont(15));
	m_pGridConveyor.SetItemText(3, 0, _T("ULD_1 Conv"));

	m_nULd1CvyStopUpDnOut[0]	= 3;
	m_nULd1CvyStopUpDnOut[1]	= 1;
	m_pGridConveyor.MergeCells(m_nULd1CvyStopUpDnOut[0], m_nULd1CvyStopUpDnOut[1], m_nULd1CvyStopUpDnOut[0], m_nULd1CvyStopUpDnOut[1] + 1);
	m_pGridConveyor.SetItemBkColour(m_nULd1CvyStopUpDnOut[0], m_nULd1CvyStopUpDnOut[1], GREEN_D, BLACK_C);
	m_pGridConveyor.SetItemText(m_nULd1CvyStopUpDnOut[0], m_nULd1CvyStopUpDnOut[1], _T("Up/Dn"));

	m_nUld1CvyStopUpIn[0]	= 3;
	m_nUld1CvyStopUpIn[1]	= 3;
	m_pGridConveyor.SetItemBkColour(m_nUld1CvyStopUpIn[0], m_nUld1CvyStopUpIn[1], YELLOW_D, BLACK_C);
	m_pGridConveyor.SetItemText(m_nUld1CvyStopUpIn[0], m_nUld1CvyStopUpIn[1], _T("Up Chk"));

	m_nUld1CvyStopDnIn[0]	= 3;
	m_nUld1CvyStopDnIn[1]	= 4;
	m_pGridConveyor.SetItemBkColour(m_nUld1CvyStopDnIn[0], m_nUld1CvyStopDnIn[1], YELLOW_D, BLACK_C);
	m_pGridConveyor.SetItemText(m_nUld1CvyStopDnIn[0], m_nUld1CvyStopDnIn[1], _T("Dn Chk"));

	
	m_pGridConveyor.SetItemBkColour(4, 0, YELLOW_L, BLACK_C);
	m_pGridConveyor.SetItemFont(4, 0, &clsFunc.OnLogFont(15));
	m_pGridConveyor.SetItemText(4, 0, _T("ULD_2 Conv"));

	m_nULd2CvyStopUpDnOut[0]	= 4;
	m_nULd2CvyStopUpDnOut[1]	= 1;
	m_pGridConveyor.MergeCells(m_nULd2CvyStopUpDnOut[0], m_nULd2CvyStopUpDnOut[1], m_nULd2CvyStopUpDnOut[0], m_nULd2CvyStopUpDnOut[1] + 1);
	m_pGridConveyor.SetItemBkColour(m_nULd2CvyStopUpDnOut[0], m_nULd2CvyStopUpDnOut[1], GREEN_D, BLACK_C);
	m_pGridConveyor.SetItemText(m_nULd2CvyStopUpDnOut[0], m_nULd2CvyStopUpDnOut[1], _T("Up/Dn"));

	m_nUld2CvyStopUpIn[0]	= 4;
	m_nUld2CvyStopUpIn[1]	= 3;
	m_pGridConveyor.SetItemBkColour(m_nUld2CvyStopUpIn[0], m_nUld2CvyStopUpIn[1], YELLOW_D, BLACK_C);
	m_pGridConveyor.SetItemText(m_nUld2CvyStopUpIn[0], m_nUld2CvyStopUpIn[1], _T("Up Chk"));

	m_nUld2CvyStopDnIn[0]	= 4;
	m_nUld2CvyStopDnIn[1]	= 4;
	m_pGridConveyor.SetItemBkColour(m_nUld2CvyStopDnIn[0], m_nUld2CvyStopDnIn[1], YELLOW_D, BLACK_C);
	m_pGridConveyor.SetItemText(m_nUld2CvyStopDnIn[0], m_nUld2CvyStopDnIn[1], _T("Dn Chk"));

	// reject_1 Conv
	m_pGridConveyor.SetItemBkColour(5, 0, YELLOW_L, BLACK_C);
	m_pGridConveyor.SetItemFont(5, 0, &clsFunc.OnLogFont(15));
	m_pGridConveyor.SetItemText(5, 0, _T("Rej_1 Conv"));

	m_nRej1CvyStopUpDnOut[0]	= 5;
	m_nRej1CvyStopUpDnOut[1]	= 1;
	m_pGridConveyor.MergeCells(m_nRej1CvyStopUpDnOut[0], m_nRej1CvyStopUpDnOut[1], m_nRej1CvyStopUpDnOut[0], m_nRej1CvyStopUpDnOut[1] + 1);
	m_pGridConveyor.SetItemBkColour(m_nRej1CvyStopUpDnOut[0], m_nRej1CvyStopUpDnOut[1], GREEN_D, BLACK_C);
	m_pGridConveyor.SetItemText(m_nRej1CvyStopUpDnOut[0], m_nRej1CvyStopUpDnOut[1], _T("Up/Dn"));

	m_nRej1CvyStopUpIn[0]	= 5;
	m_nRej1CvyStopUpIn[1]	= 3;
	m_pGridConveyor.SetItemBkColour(m_nRej1CvyStopUpIn[0], m_nRej1CvyStopUpIn[1], YELLOW_D, BLACK_C);
	m_pGridConveyor.SetItemText(m_nRej1CvyStopUpIn[0], m_nRej1CvyStopUpIn[1], _T("Up Chk"));

	m_nRej1CvyStopDnIn[0]	= 5;
	m_nRej1CvyStopDnIn[1]	= 4;
	m_pGridConveyor.SetItemBkColour(m_nRej1CvyStopDnIn[0], m_nRej1CvyStopDnIn[1], YELLOW_D, BLACK_C);
	m_pGridConveyor.SetItemText(m_nRej1CvyStopDnIn[0], m_nRej1CvyStopDnIn[1], _T("Dn Chk"));

	// reject_2 Conv
	m_pGridConveyor.SetItemBkColour(6, 0, YELLOW_L, BLACK_C);
	m_pGridConveyor.SetItemFont(6, 0, &clsFunc.OnLogFont(15));
	m_pGridConveyor.SetItemText(6, 0, _T("Rej_2 Conv"));

	m_nRej2CvyStopUpDnOut[0]	= 6;
	m_nRej2CvyStopUpDnOut[1]	= 1;
	m_pGridConveyor.MergeCells(m_nRej2CvyStopUpDnOut[0], m_nRej2CvyStopUpDnOut[1], m_nRej2CvyStopUpDnOut[0], m_nRej2CvyStopUpDnOut[1] + 1);
	m_pGridConveyor.SetItemBkColour(m_nRej2CvyStopUpDnOut[0], m_nRej2CvyStopUpDnOut[1], GREEN_D, BLACK_C);
	m_pGridConveyor.SetItemText(m_nRej2CvyStopUpDnOut[0], m_nRej2CvyStopUpDnOut[1], _T("Up/Dn"));

	m_nRej2CvyStopUpIn[0]	= 6;
	m_nRej2CvyStopUpIn[1]	= 3;
	m_pGridConveyor.SetItemBkColour(m_nRej2CvyStopUpIn[0], m_nRej2CvyStopUpIn[1], YELLOW_D, BLACK_C);
	m_pGridConveyor.SetItemText(m_nRej2CvyStopUpIn[0], m_nRej2CvyStopUpIn[1], _T("Up Chk"));

	m_nRej2CvyStopDnIn[0]	= 6;
	m_nRej2CvyStopDnIn[1]	= 4;
	m_pGridConveyor.SetItemBkColour(m_nRej2CvyStopDnIn[0], m_nRej2CvyStopDnIn[1], YELLOW_D, BLACK_C);
	m_pGridConveyor.SetItemText(m_nRej2CvyStopDnIn[0], m_nRej2CvyStopDnIn[1], _T("Dn Chk"));

	m_pGridConveyor.Invalidate(FALSE);

}

void CDialogManualIO::OnInitGridTransfer()
{
	int   i, j;
	int	  nMaxRow, nMaxCol;

	CString strTmp;

	nMaxRow = 7; // unloader tray y count
	nMaxCol = 5;

	m_pGridTransfer.SetFrameFocusCell(FALSE);
	m_pGridTransfer.SetTrackFocusCell(FALSE);
	m_pGridTransfer.EnableSelection(FALSE);

	m_pGridTransfer.SetGridLineColor(BLACK_C);
	m_pGridTransfer.SetGridLines(1);

	m_pGridTransfer.SetRowCount(nMaxRow);
	m_pGridTransfer.SetColumnCount(nMaxCol);

	m_pGridTransfer.SetFixedRowCount(0);
	m_pGridTransfer.SetFixedColumnCount(0);
	m_pGridTransfer.SetFixedBkColor(RGB(0,0,200));
	m_pGridTransfer.SetFixedBkColor(RGB(200,200,255));
	m_pGridTransfer.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<nMaxRow; i++)
	{
		m_pGridTransfer.SetRowHeight(i, 35);

		for (j=0; j<nMaxCol; j++)
		{
			if (j == 0)
			{
				m_pGridTransfer.SetColumnWidth(j, 95);
			}
			else 
			{
				m_pGridTransfer.SetColumnWidth(j, 70);
			}

			m_pGridTransfer.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridTransfer.SetItemState(i, j, GVIS_READONLY);
			m_pGridTransfer.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
		}
	}
	


	m_pGridTransfer.MergeCells(0, 0, 0, 4);
	m_pGridTransfer.SetItemBkColour(0, 0, BLUE_D, WHITE_C);
	m_pGridTransfer.SetItemFont(0, 0, &clsFunc.OnLogFont(17));
	m_pGridTransfer.SetItemText(0, 0, _T("Tray Transfer"));

	
	m_pGridTransfer.SetItemBkColour(1, 0, YELLOW_L, BLACK_C);
	m_pGridTransfer.SetItemFont(1, 0, &clsFunc.OnLogFont(15));
	m_pGridTransfer.SetItemText(1, 0, _T("LD_1 Conv"));

	m_pGridLdRbt.MergeCells(0, 0, 0, 4);
	m_pGridLdRbt.SetItemBkColour(0, 0, BLUE_D, WHITE_C);
	m_pGridLdRbt.SetItemFont(0, 0, &clsFunc.OnLogFont(15));
	m_pGridLdRbt.SetItemText(0, 0, _T("Loader Robot"));

	m_pGridLdRbt.MergeCells(1, 1, 1, 2);
	m_pGridLdRbt.SetItemBkColour(1, 1, YELLOW_L, BLACK_C);
	m_pGridLdRbt.SetItemFont(1, 1, &clsFunc.OnLogFont(15));
	m_pGridLdRbt.SetItemText(1, 1, _T("Picker #1"));

	m_pGridLdRbt.MergeCells(1, 3, 1, 4);
	m_pGridLdRbt.SetItemBkColour(1, 3, YELLOW_L, BLACK_C);
	m_pGridLdRbt.SetItemFont(1, 3, &clsFunc.OnLogFont(15));
	m_pGridLdRbt.SetItemText(1, 3, _T("Picker #2"));

	m_pGridLdRbt.MergeCells(2, 0, 3, 0);
	m_pGridLdRbt.SetItemBkColour(2, 0, BLUE_D, WHITE_C);
	m_pGridLdRbt.SetItemFont(2, 0, &clsFunc.OnLogFont(15));
	m_pGridLdRbt.SetItemText(2, 0, _T("Picker"));

	m_pGridLdRbt.MergeCells(4, 0, 5, 0);
	m_pGridLdRbt.SetItemBkColour(4, 0, BLUE_D, WHITE_C);
	m_pGridLdRbt.SetItemFont(4, 0, &clsFunc.OnLogFont(15));
	m_pGridLdRbt.SetItemText(4, 0, _T("Vaccum"));

// 	for (i=0; i<2; i++)
// 	{
// 		// down input
// 		m_nLdDnIn[0][i] = 2;
// 		m_nLdDnIn[1][i] = (i * 2) + 1;
// 		m_pGridTransfer.SetItemBkColour(m_nLdDnIn[0][i], m_nLdDnIn[1][i], YELLOW_D, BLACK_C);
// 		m_pGridTransfer.SetItemText(m_nLdDnIn[0][i], m_nLdDnIn[1][i], _T("Dn"));
// 
// 		// down output
// 		m_nLdDnOut[0][i] = 2;
// 		m_nLdDnOut[1][i] = (i * 2) + 2;
// 		m_pGridTransfer.MergeCells(m_nLdDnOut[0][i], m_nLdDnOut[1][i], m_nLdDnOut[0][i] + 1, m_nLdDnOut[1][i]);
// 		m_pGridTransfer.SetItemBkColour(m_nLdDnOut[0][i], m_nLdDnOut[1][i], GREEN_D, BLACK_C);
// 		m_pGridTransfer.SetItemText(m_nLdDnOut[0][i], m_nLdDnOut[1][i], _T("Dn/Up"));
// 
// 		// up input
// 		m_nLdUpIn[0][i] = 3;
// 		m_nLdUpIn[1][i] = (i * 2) + 1;
// 		m_pGridTransfer.SetItemBkColour(m_nLdUpIn[0][i], m_nLdUpIn[1][i], YELLOW_D, BLACK_C);
// 		m_pGridTransfer.SetItemText(m_nLdUpIn[0][i], m_nLdUpIn[1][i], _T("Up"));
/*
		// up output
		m_nLdUpOut[0][i] = 3;
		m_nLdUpOut[1][i] = (i * 2) + 2;
		m_pGridLdRbt.SetItemBkColour(m_nLdUpOut[0][i], m_nLdUpOut[1][i], GREEN_D, BLACK_C);
		m_pGridLdRbt.SetItemText(m_nLdUpOut[0][i], m_nLdUpOut[1][i], _T("Up"));
*/
		////////////////////////////////////////////////////////////////////////////////////////
		// vaccum input
// 		m_nLdVacIn[0][i] = 4;
// 		m_nLdVacIn[1][i] = (i * 2) + 1;
// 		m_pGridTransfer.MergeCells(m_nLdVacIn[0][i], m_nLdVacIn[1][i], m_nLdVacIn[0][i] + 1, m_nLdVacIn[1][i]);
// 		m_pGridTransfer.SetItemBkColour(m_nLdVacIn[0][i], m_nLdVacIn[1][i], YELLOW_D, BLACK_C);
// 		m_pGridTransfer.SetItemText(m_nLdVacIn[0][i], m_nLdVacIn[1][i], _T("Off"));
// 
// 		// vaccum output
// 		m_nLdVacOut[0][i] = 4;
// 		m_nLdVacOut[1][i] = (i * 2) + 2;
// 		m_pGridTransfer.SetItemBkColour(m_nLdVacOut[0][i], m_nLdVacOut[1][i], GREEN_D, BLACK_C);
// 		m_pGridTransfer.SetItemText(m_nLdVacOut[0][i], m_nLdVacOut[1][i], _T("Vac"));
// 
// 		// eject output
// 		m_nLdEjcOut[0][i] = 5;
// 		m_nLdEjcOut[1][i] = (i * 2) + 2;
// 		m_pGridTransfer.SetItemBkColour(m_nLdEjcOut[0][i], m_nLdEjcOut[1][i], GREEN_D, BLACK_C);
// 		m_pGridTransfer.SetItemText(m_nLdEjcOut[0][i], m_nLdEjcOut[1][i], _T("Ejc"));
//	}

	

	m_pGridTransfer.Invalidate(FALSE);

}

void CDialogManualIO::OnInitGridStacker()
{
	int   i, j;
	int	  nMaxRow, nMaxCol;
	//	int   nRow, nCol;
	CString strTmp;

	nMaxRow = 20; // unloader tray y count
	nMaxCol = 5;

	m_pGridStacker.SetFrameFocusCell(FALSE);
	m_pGridStacker.SetTrackFocusCell(FALSE);
	m_pGridStacker.EnableSelection(FALSE);

	m_pGridStacker.SetGridLineColor(BLACK_C);
	m_pGridStacker.SetGridLines(1);

	m_pGridStacker.SetRowCount(nMaxRow);
	m_pGridStacker.SetColumnCount(nMaxCol);

	m_pGridStacker.SetFixedRowCount(0);
	m_pGridStacker.SetFixedColumnCount(0);
	m_pGridStacker.SetFixedBkColor(RGB(0,0,200));
	m_pGridStacker.SetFixedBkColor(RGB(200,200,255));
	m_pGridStacker.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<nMaxRow; i++)
	{
		m_pGridStacker.SetRowHeight(i, 35);

		for (j=0; j<nMaxCol; j++)
		{
			if (j == 0)
			{
				m_pGridStacker.SetColumnWidth(j, 100);
			}
			else 
			{
				m_pGridStacker.SetColumnWidth(j, 70);
			}

			m_pGridStacker.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridStacker.SetItemState(i, j, GVIS_READONLY);
			m_pGridStacker.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
		}
	}
	//LdStacker
	m_nLdTrayPlateLfOnOffCylOut = st_io_info.o_LdTrayPlateLfOnOffCyl;
	m_nLdTrayPlateRiOnOffCylOut = st_io_info.o_LdTrayPlateRiOnOffCyl;
	m_nLdStkUpCylOut = st_io_info.o_LdStkUpCyl;			
	m_nLdStkDnCylOut =st_io_info.o_LdStkDnCyl;			

	m_nLdTrayPlateLfOnChkIn = st_io_info.i_LdTrayPlateLfOnChk; 	
	m_nLdTrayPlateLfOffChkIn = st_io_info.i_LdTrayPlateLfOffChk;	
	m_nLdTrayPlateRiOnChkIn = st_io_info.i_LdTrayPlateRiOnChk;	
	m_nLdTrayPlateRiOffChkIn = st_io_info.i_LdTrayPlateRiOffChk;	
	m_nLdStkUpChkIn = st_io_info.i_LdStkUpChk;			
	m_nLdStkDnChkIn = st_io_info.i_LdStkDnChk;	

	//Empty Stacker
	m_nEmptyStkUpCylOut = st_io_info.o_EmptyStkUpCyl;
	m_nEmptyStkDnCylOut = st_io_info.o_EmptyStkDnCyl;

	m_nEmptyStkUpChkIn = st_io_info.i_EmptyStkUpChk;	
	m_nEmptyStkDnChkIn = st_io_info.i_EmptyStkDnChk;	

	//Uld Stacker
	m_nUld1StkUpCylOut = st_io_info.o_Uld1StkUpCyl;
	m_nUld1StkDnCylOut = st_io_info.o_Uld1StkDnCyl;
	m_nUld2StkUpCylOut = st_io_info.o_Uld2StkUpCyl;
	m_nUld2StkDnCylOut = st_io_info.o_Uld2StkDnCyl;

	m_nUld1StkUpChkIn = st_io_info.i_Uld1StkUpChk; 
	m_nUld1StkDnChkIn = st_io_info.i_Uld1StkDnChk;
	m_nUld2StkUpChkIn = st_io_info.i_Uld2StkUpChk;
	m_nUld2StkDnChkIn = st_io_info.i_Uld2StkDnChk;

	//Rej Empty Stacker
	m_nRejEmptyStkUpCylOut = st_io_info.o_RejEmptyStkUpCyl;	
	m_nRejEmptyStkDnCylOut = st_io_info.o_RejEmptyStkDnCyl;	
	m_nRej1StkUpCylOut	   = st_io_info.o_Rej1StkUpCyl;		
	m_nRej1StkDnCylOut	   = st_io_info.o_Rej1StkDnCyl;		
	m_nRej2StkUpCylOut	   = st_io_info.o_Rej2StkUpCyl;		
	m_nRej2StkDnCylOut	   = st_io_info.o_Rej2StkDnCyl;

	m_nRejEmptyStkUpChkIn = st_io_info.i_RejEmptyStkUpChk;		
	m_nRejEmptyStkDnChkIn = st_io_info.i_RejEmptyStkDnChk;		
	m_nRej1StkUpChkIn     =	st_io_info.i_Rej1StkUpChk;		
	m_nRej1StkDnChkIn	  = st_io_info.i_Rej1StkDnChk;		
	m_nRej2StkUpChkIn	  = st_io_info.i_Rej2StkUpChk;	
	m_nRej2StkDnChkIn	  = st_io_info.i_Rej2StkDnChk;



	m_pGridStacker.MergeCells(0, 0, 0, 4);
	m_pGridStacker.SetItemBkColour(0, 0, BLUE_D, WHITE_C);
	m_pGridStacker.SetItemFont(0, 0, &clsFunc.OnLogFont(17));
	m_pGridStacker.SetItemText(0, 0, _T("Stacker Cylinder"));

	// LD Left Plate
	m_pGridStacker.SetItemBkColour(1, 0, YELLOW_L, BLACK_C);
	m_pGridStacker.SetItemFont(1, 0, &clsFunc.OnLogFont(15));
	m_pGridStacker.SetItemText(1, 0, _T("LD Left Plate"));

	m_nLdTrayPlateLfOnOffCyl[0]	= 1;
	m_nLdTrayPlateLfOnOffCyl[1]	= 1;
	m_pGridStacker.MergeCells(m_nLdTrayPlateLfOnOffCyl[0], m_nLdTrayPlateLfOnOffCyl[1], m_nLdTrayPlateLfOnOffCyl[0], m_nLdTrayPlateLfOnOffCyl[1] + 1);
	m_pGridStacker.SetItemBkColour(m_nLdTrayPlateLfOnOffCyl[0], m_nLdTrayPlateLfOnOffCyl[1], GREEN_D, BLACK_C);
	m_pGridStacker.SetItemText(m_nLdTrayPlateLfOnOffCyl[0], m_nLdTrayPlateLfOnOffCyl[1], _T("OFF/On"));

	m_nLdTrayPlateLfOnChk[0]	= 1;
	m_nLdTrayPlateLfOnChk[1]	= 3;
	m_pGridStacker.SetItemBkColour(m_nLdTrayPlateLfOnChk[0], m_nLdTrayPlateLfOnChk[1], YELLOW_D, BLACK_C);
	m_pGridStacker.SetItemText(m_nLdTrayPlateLfOnChk[0], m_nLdTrayPlateLfOnChk[1], _T("On Chk"));
	
	m_nLdTrayPlateLfOffChk[0]	= 1;
	m_nLdTrayPlateLfOffChk[1]	= 4;
	m_pGridStacker.SetItemBkColour(m_nLdTrayPlateLfOffChk[0], m_nLdTrayPlateLfOffChk[1], YELLOW_D, BLACK_C);
	m_pGridStacker.SetItemText(m_nLdTrayPlateLfOffChk[0], m_nLdTrayPlateLfOffChk[1], _T("Off Chk"));


	// LD Rigt Plate
	m_pGridStacker.SetItemBkColour(2, 0, YELLOW_L, BLACK_C);
	m_pGridStacker.SetItemFont(2, 0, &clsFunc.OnLogFont(15));
	m_pGridStacker.SetItemText(2, 0, _T("LD Right Plate"));

	m_nLdTrayPlateRiOnOffCyl[0]	= 2;
	m_nLdTrayPlateRiOnOffCyl[1]	= 1;
	m_pGridStacker.MergeCells(m_nLdTrayPlateRiOnOffCyl[0], m_nLdTrayPlateRiOnOffCyl[1], m_nLdTrayPlateRiOnOffCyl[0], m_nLdTrayPlateRiOnOffCyl[1] + 1);
	m_pGridStacker.SetItemBkColour(m_nLdTrayPlateRiOnOffCyl[0], m_nLdTrayPlateRiOnOffCyl[1], GREEN_D, BLACK_C);
	m_pGridStacker.SetItemText(m_nLdTrayPlateRiOnOffCyl[0], m_nLdTrayPlateRiOnOffCyl[1], _T("OFF/On"));

	m_nLdTrayPlateRiOnOffCyl[0]	= 2;
	m_nLdTrayPlateRiOnOffCyl[1]	= 3;
	m_pGridStacker.SetItemBkColour(m_nLdTrayPlateRiOnOffCyl[0], m_nLdTrayPlateRiOnOffCyl[1], YELLOW_D, BLACK_C);
	m_pGridStacker.SetItemText(m_nLdTrayPlateRiOnOffCyl[0], m_nLdTrayPlateRiOnOffCyl[1], _T("On Chk"));

	m_nLdTrayPlateRiOnOffCyl[0]	= 2;
	m_nLdTrayPlateRiOnOffCyl[1]	= 4;
	m_pGridStacker.SetItemBkColour(m_nLdTrayPlateRiOnOffCyl[0], m_nLdTrayPlateRiOnOffCyl[1], YELLOW_D, BLACK_C);
	m_pGridStacker.SetItemText(m_nLdTrayPlateRiOnOffCyl[0], m_nLdTrayPlateRiOnOffCyl[1], _T("Off Chk"));
	
	// Ld Stacker
	m_pGridStacker.SetItemBkColour(3, 0, YELLOW_L, BLACK_C);
	m_pGridStacker.SetItemFont(3, 0, &clsFunc.OnLogFont(15));
	m_pGridStacker.SetItemText(3, 0, _T("Ld Stacker"));

	m_nLdStkUpCyl[0]	= 3;
	m_nLdStkUpCyl[1]	= 1;
	//m_pGridStacker.MergeCells(m_nULd1CvyStopUpDnOut[0], m_nULd1CvyStopUpDnOut[1], m_nULd1CvyStopUpDnOut[0], m_nULd1CvyStopUpDnOut[1] + 1);
	m_pGridStacker.SetItemBkColour(m_nLdStkUpCyl[0], m_nLdStkUpCyl[1], GREEN_D, BLACK_C);
	m_pGridStacker.SetItemText(m_nLdStkUpCyl[0], m_nLdStkUpCyl[1], _T("Up"));

	m_nLdStkDnCyl[0]	= 3;
	m_nLdStkDnCyl[1]	= 2;
	//m_pGridStacker.MergeCells(m_nULd1CvyStopUpDnOut[0], m_nULd1CvyStopUpDnOut[1], m_nULd1CvyStopUpDnOut[0], m_nULd1CvyStopUpDnOut[1] + 1);
	m_pGridStacker.SetItemBkColour(m_nLdStkDnCyl[0], m_nLdStkDnCyl[1], GREEN_D, BLACK_C);
	m_pGridStacker.SetItemText(m_nLdStkDnCyl[0], m_nLdStkDnCyl[1], _T("Dn"));

	m_nLdStkUpChk[0]	= 3;
	m_nLdStkUpChk[1]	= 3;
	m_pGridStacker.SetItemBkColour(m_nLdStkUpChk[0], m_nLdStkUpChk[1], YELLOW_D, BLACK_C);
	m_pGridStacker.SetItemText(m_nLdStkUpChk[0], m_nLdStkUpChk[1], _T("Up Chk"));
	
	m_nLdStkDnChk[0]	= 3;
	m_nLdStkDnChk[1]	= 4;
	m_pGridStacker.SetItemBkColour(m_nLdStkDnChk[0], m_nLdStkDnChk[1], YELLOW_D, BLACK_C);
	m_pGridStacker.SetItemText(m_nLdStkDnChk[0], m_nLdStkDnChk[1], _T("Dn Chk"));

	
	
	m_pGridStacker.SetItemBkColour(4, 0, YELLOW_L, BLACK_C);
	m_pGridStacker.SetItemFont(4, 0, &clsFunc.OnLogFont(15));
	m_pGridStacker.SetItemText(4, 0, _T("Empty Stacker"));

	m_nULd2CvyStopUpDnOut[0]	= 4;
	m_nULd2CvyStopUpDnOut[1]	= 1;
	//m_pGridStacker.MergeCells(m_nULd2CvyStopUpDnOut[0], m_nULd2CvyStopUpDnOut[1], m_nULd2CvyStopUpDnOut[0], m_nULd2CvyStopUpDnOut[1] + 1);
	m_pGridStacker.SetItemBkColour(m_nULd2CvyStopUpDnOut[0], m_nULd2CvyStopUpDnOut[1], GREEN_D, BLACK_C);
	m_pGridStacker.SetItemText(m_nULd2CvyStopUpDnOut[0], m_nULd2CvyStopUpDnOut[1], _T("Up"));

	m_pGridStacker.SetItemBkColour(m_nULd2CvyStopUpDnOut[0], m_nULd2CvyStopUpDnOut[1], GREEN_D, BLACK_C);
	m_pGridStacker.SetItemText(m_nULd2CvyStopUpDnOut[0], m_nULd2CvyStopUpDnOut[1], _T("Dn"));

	m_nUld2CvyStopUpIn[0]	= 4;
	m_nUld2CvyStopUpIn[1]	= 3;
	m_pGridStacker.SetItemBkColour(m_nUld2CvyStopUpIn[0], m_nUld2CvyStopUpIn[1], YELLOW_D, BLACK_C);
	m_pGridStacker.SetItemText(m_nUld2CvyStopUpIn[0], m_nUld2CvyStopUpIn[1], _T("Up Chk"));

	m_nUld2CvyStopDnIn[0]	= 4;
	m_nUld2CvyStopDnIn[1]	= 4;
	m_pGridStacker.SetItemBkColour(m_nUld2CvyStopDnIn[0], m_nUld2CvyStopDnIn[1], YELLOW_D, BLACK_C);
	m_pGridStacker.SetItemText(m_nUld2CvyStopDnIn[0], m_nUld2CvyStopDnIn[1], _T("Dn Chk"));

	// reject_1 Conv
	m_pGridStacker.SetItemBkColour(5, 0, YELLOW_L, BLACK_C);
	m_pGridStacker.SetItemFont(5, 0, &clsFunc.OnLogFont(15));
	m_pGridStacker.SetItemText(5, 0, _T("Rej_1 Conv"));

	m_nRej1CvyStopUpDnOut[0]	= 5;
	m_nRej1CvyStopUpDnOut[1]	= 1;
	m_pGridStacker.MergeCells(m_nRej1CvyStopUpDnOut[0], m_nRej1CvyStopUpDnOut[1], m_nRej1CvyStopUpDnOut[0], m_nRej1CvyStopUpDnOut[1] + 1);
	m_pGridStacker.SetItemBkColour(m_nRej1CvyStopUpDnOut[0], m_nRej1CvyStopUpDnOut[1], GREEN_D, BLACK_C);
	m_pGridStacker.SetItemText(m_nRej1CvyStopUpDnOut[0], m_nRej1CvyStopUpDnOut[1], _T("Up/Dn"));

	m_nRej1CvyStopUpIn[0]	= 5;
	m_nRej1CvyStopUpIn[1]	= 3;
	m_pGridStacker.SetItemBkColour(m_nRej1CvyStopUpIn[0], m_nRej1CvyStopUpIn[1], YELLOW_D, BLACK_C);
	m_pGridStacker.SetItemText(m_nRej1CvyStopUpIn[0], m_nRej1CvyStopUpIn[1], _T("Up Chk"));

	m_nRej1CvyStopDnIn[0]	= 5;
	m_nRej1CvyStopDnIn[1]	= 4;
	m_pGridConveyor.SetItemBkColour(m_nRej1CvyStopDnIn[0], m_nRej1CvyStopDnIn[1], YELLOW_D, BLACK_C);
	m_pGridConveyor.SetItemText(m_nRej1CvyStopDnIn[0], m_nRej1CvyStopDnIn[1], _T("Dn Chk"));

	// reject_2 Conv
	m_pGridStacker.SetItemBkColour(6, 0, YELLOW_L, BLACK_C);
	m_pGridStacker.SetItemFont(6, 0, &clsFunc.OnLogFont(15));
	m_pGridStacker.SetItemText(6, 0, _T("Rej_2 Conv"));

	m_nRej2CvyStopUpDnOut[0]	= 6;
	m_nRej2CvyStopUpDnOut[1]	= 1;
	m_pGridStacker.MergeCells(m_nRej2CvyStopUpDnOut[0], m_nRej2CvyStopUpDnOut[1], m_nRej2CvyStopUpDnOut[0], m_nRej2CvyStopUpDnOut[1] + 1);
	m_pGridStacker.SetItemBkColour(m_nRej2CvyStopUpDnOut[0], m_nRej2CvyStopUpDnOut[1], GREEN_D, BLACK_C);
	m_pGridStacker.SetItemText(m_nRej2CvyStopUpDnOut[0], m_nRej2CvyStopUpDnOut[1], _T("Up/Dn"));

	m_nRej2CvyStopUpIn[0]	= 6;
	m_nRej2CvyStopUpIn[1]	= 3;
	m_pGridConveyor.SetItemBkColour(m_nRej2CvyStopUpIn[0], m_nRej2CvyStopUpIn[1], YELLOW_D, BLACK_C);
	m_pGridConveyor.SetItemText(m_nRej2CvyStopUpIn[0], m_nRej2CvyStopUpIn[1], _T("Up Chk"));

	m_nRej2CvyStopDnIn[0]	= 6;
	m_nRej2CvyStopDnIn[1]	= 4;
	m_pGridStacker.SetItemBkColour(m_nRej2CvyStopDnIn[0], m_nRej2CvyStopDnIn[1], YELLOW_D, BLACK_C);
	m_pGridStacker.SetItemText(m_nRej2CvyStopDnIn[0], m_nRej2CvyStopDnIn[1], _T("Dn Chk"));


	m_pGridConveyor.Invalidate(FALSE);

}

void CDialogManualIO::OnInitGridTsite()
{
	int   i, j;
	int	  nMaxRow, nMaxCol;
//	int   nRow, nCol;
	CString strTemp;

	nMaxRow = 8;
	nMaxCol = 13;

	m_pGridTsite.SetFrameFocusCell(FALSE);
	m_pGridTsite.SetTrackFocusCell(FALSE);
	m_pGridTsite.EnableSelection(FALSE);

	m_pGridTsite.SetGridLineColor(BLACK_C);
	m_pGridTsite.SetGridLines(1);

	m_pGridTsite.SetRowCount(nMaxRow);
	m_pGridTsite.SetColumnCount(nMaxCol);

	m_pGridTsite.SetFixedRowCount(0);
	m_pGridTsite.SetFixedColumnCount(0);
	m_pGridTsite.SetFixedBkColor(RGB(0,0,200));
	m_pGridTsite.SetFixedBkColor(RGB(200,200,255));
	m_pGridTsite.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<nMaxRow; i++)
	{
		m_pGridTsite.SetRowHeight(i, 25);
		
		for (j=0; j<nMaxCol; j++)
		{
			if (j == 0)
			{
				m_pGridTsite.SetColumnWidth(j, 160);
			}
			else
			{
				m_pGridTsite.SetColumnWidth(j, 83);
			}

			m_pGridTsite.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridTsite.SetItemState(i, j, GVIS_READONLY);
			m_pGridTsite.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
		}
	}

	m_pGridTsite.MergeCells(0, 0, 1, 0);
	m_pGridTsite.SetItemBkColour(0, 0, ORANGE_C, YELLOW_C);
	m_pGridTsite.SetItemFont(0, 0, &clsFunc.OnLogFont(14));
	m_pGridTsite.SetItemText(0, 0, _T("Left Bottom"));

	m_pGridTsite.MergeCells(2, 0, 3, 0);
	m_pGridTsite.SetItemBkColour(2, 0, BLACK_L, YELLOW_C);
	m_pGridTsite.SetItemFont(2, 0, &clsFunc.OnLogFont(14));
	m_pGridTsite.SetItemText(2, 0, _T("Left Top"));

	m_pGridTsite.MergeCells(4, 0, 5, 0);
	m_pGridTsite.SetItemBkColour(4, 0, BLACK_L, YELLOW_C);
	m_pGridTsite.SetItemFont(4, 0, &clsFunc.OnLogFont(14));
	m_pGridTsite.SetItemText(4, 0, _T("Right Bottom"));

	m_pGridTsite.MergeCells(6, 0, 7, 0);
	m_pGridTsite.SetItemBkColour(6, 0, BLACK_L, YELLOW_C);
	m_pGridTsite.SetItemFont(6, 0, &clsFunc.OnLogFont(14));
	m_pGridTsite.SetItemText(6, 0, _T("Right Top"));

// 	for (i=0; i<3; i++)
// 	{
// 		m_nTsiteInsOut[0][i]	= 0;
// 		m_nTsiteInsOut[1][i]	= (i * 4) + 1;
// 		m_pGridTsite.MergeCells(m_nTsiteInsOut[0][i], m_nTsiteInsOut[1][i], m_nTsiteInsOut[0][i], m_nTsiteInsOut[1][i] + 3);
// 		m_pGridTsite.SetItemBkColour(m_nTsiteInsOut[0][i], m_nTsiteInsOut[1][i], GREEN_D, BLACK_C);
// 		m_pGridTsite.SetItemFont(m_nTsiteInsOut[0][i], m_nTsiteInsOut[1][i], &clsFunc.OnLogFont(14));
// 		strTemp.Format(_T("Insert_%02d"), (m_nTsiteNum * 12) + (i + 1));
// 		m_pGridTsite.SetItemText(m_nTsiteInsOut[0][i], m_nTsiteInsOut[1][i], strTemp);
// 
// 		m_nTsiteInsFwd[0][i]	= 1;
// 		m_nTsiteInsFwd[1][i]	= (i * 4) + 1;
// 		m_pGridTsite.MergeCells(m_nTsiteInsFwd[0][i], m_nTsiteInsFwd[1][i], m_nTsiteInsFwd[0][i], m_nTsiteInsFwd[1][i] + 1);
// 		m_pGridTsite.SetItemBkColour(m_nTsiteInsFwd[0][i], m_nTsiteInsFwd[1][i], YELLOW_D, BLACK_C);
// 		m_pGridTsite.SetItemFont(m_nTsiteInsFwd[0][i], m_nTsiteInsFwd[1][i], &clsFunc.OnLogFont(14));
// 		m_pGridTsite.SetItemText(m_nTsiteInsFwd[0][i], m_nTsiteInsFwd[1][i], _T("Fwd"));
// 
// 		m_nTsiteInsBwd[0][i]	= 1;
// 		m_nTsiteInsBwd[1][i]	= (i * 4) + 3;
// 		m_pGridTsite.MergeCells(m_nTsiteInsBwd[0][i], m_nTsiteInsBwd[1][i], m_nTsiteInsBwd[0][i], m_nTsiteInsBwd[1][i] + 1);
// 		m_pGridTsite.SetItemBkColour(m_nTsiteInsBwd[0][i], m_nTsiteInsBwd[1][i], YELLOW_D, BLACK_C);
// 		m_pGridTsite.SetItemFont(m_nTsiteInsBwd[0][i], m_nTsiteInsBwd[1][i], &clsFunc.OnLogFont(14));
// 		m_pGridTsite.SetItemText(m_nTsiteInsBwd[0][i], m_nTsiteInsBwd[1][i], _T("Bwd"));
// 
// 		m_nTsiteInsOut[0][i + 3]	= 4;
// 		m_nTsiteInsOut[1][i + 3]	= (i * 4) + 1;
// 		m_pGridTsite.MergeCells(m_nTsiteInsOut[0][i + 3], m_nTsiteInsOut[1][i + 3], m_nTsiteInsOut[0][i + 3], m_nTsiteInsOut[1][i + 3] + 3);
// 		m_pGridTsite.SetItemBkColour(m_nTsiteInsOut[0][i + 3], m_nTsiteInsOut[1][i + 3], GREEN_D, BLACK_C);
// 		m_pGridTsite.SetItemFont(m_nTsiteInsOut[0][i + 3], m_nTsiteInsOut[1][i + 3], &clsFunc.OnLogFont(14));
// 		strTemp.Format(_T("Insert_%02d"), (m_nTsiteNum * 12) + (i + 4));
// 		m_pGridTsite.SetItemText(m_nTsiteInsOut[0][i + 3], m_nTsiteInsOut[1][i + 3], strTemp);
// 
// 		m_nTsiteInsFwd[0][i + 3]	= 5;
// 		m_nTsiteInsFwd[1][i + 3]	= (i * 4) + 1;
// 		m_pGridTsite.MergeCells(m_nTsiteInsFwd[0][i + 3], m_nTsiteInsFwd[1][i + 3], m_nTsiteInsFwd[0][i + 3], m_nTsiteInsFwd[1][i + 3] + 1);
// 		m_pGridTsite.SetItemBkColour(m_nTsiteInsFwd[0][i + 3], m_nTsiteInsFwd[1][i + 3], YELLOW_D, BLACK_C);
// 		m_pGridTsite.SetItemFont(m_nTsiteInsFwd[0][i + 3], m_nTsiteInsFwd[1][i + 3], &clsFunc.OnLogFont(14));
// 		m_pGridTsite.SetItemText(m_nTsiteInsFwd[0][i + 3], m_nTsiteInsFwd[1][i + 3], _T("Fwd"));
// 
// 		m_nTsiteInsBwd[0][i + 3]	= 5;
// 		m_nTsiteInsBwd[1][i + 3]	= (i * 4) + 3;
// 		m_pGridTsite.MergeCells(m_nTsiteInsBwd[0][i + 3], m_nTsiteInsBwd[1][i + 3], m_nTsiteInsBwd[0][i + 3], m_nTsiteInsBwd[1][i + 3] + 1);
// 		m_pGridTsite.SetItemBkColour(m_nTsiteInsBwd[0][i + 3], m_nTsiteInsBwd[1][i + 3], YELLOW_D, BLACK_C);
// 		m_pGridTsite.SetItemFont(m_nTsiteInsBwd[0][i + 3], m_nTsiteInsBwd[1][i + 3], &clsFunc.OnLogFont(14));
// 		m_pGridTsite.SetItemText(m_nTsiteInsBwd[0][i + 3], m_nTsiteInsBwd[1][i + 3], _T("Bwd"));
// 	}
// 
// 	for (i=0; i<6; i++)
// 	{
// 		m_nTsitePogoOut[0][i]	= 2;
// 		m_nTsitePogoOut[1][i]	= (i * 2) + 1;
// 		m_pGridTsite.MergeCells(m_nTsitePogoOut[0][i], m_nTsitePogoOut[1][i], m_nTsitePogoOut[0][i], m_nTsitePogoOut[1][i] + 1);
// 		m_pGridTsite.SetItemBkColour(m_nTsitePogoOut[0][i], m_nTsitePogoOut[1][i], GREEN_D, BLACK_C);
// 		m_pGridTsite.SetItemFont(m_nTsitePogoOut[0][i], m_nTsitePogoOut[1][i], &clsFunc.OnLogFont(14));
// 		strTemp.Format(_T("Pogo_%02d"), (m_nTsiteNum * 12) + (i + 1));
// 		m_pGridTsite.SetItemText(m_nTsitePogoOut[0][i], m_nTsitePogoOut[1][i], strTemp);
// 
// 		m_nTsitePogoIn[0][i]	= 3;
// 		m_nTsitePogoIn[1][i]	= (i * 2) + 1;
// 		m_pGridTsite.MergeCells(m_nTsitePogoIn[0][i], m_nTsitePogoIn[1][i], m_nTsitePogoIn[0][i], m_nTsitePogoIn[1][i] + 1);
// 		m_pGridTsite.SetItemBkColour(m_nTsitePogoIn[0][i], m_nTsitePogoIn[1][i], YELLOW_D, BLACK_C);
// 		m_pGridTsite.SetItemFont(m_nTsitePogoIn[0][i], m_nTsitePogoIn[1][i], &clsFunc.OnLogFont(14));
// 		m_pGridTsite.SetItemText(m_nTsitePogoIn[0][i], m_nTsitePogoIn[1][i], _T("Dn"));
// 
// 		m_nTsitePogoOut[0][i + 6]	= 6;
// 		m_nTsitePogoOut[1][i + 6]	= (i * 2) + 1;
// 		m_pGridTsite.MergeCells(m_nTsitePogoOut[0][i + 6], m_nTsitePogoOut[1][i + 6], m_nTsitePogoOut[0][i + 6], m_nTsitePogoOut[1][i + 6] + 1);
// 		m_pGridTsite.SetItemBkColour(m_nTsitePogoOut[0][i + 6], m_nTsitePogoOut[1][i + 6], GREEN_D, BLACK_C);
// 		m_pGridTsite.SetItemFont(m_nTsitePogoOut[0][i + 6], m_nTsitePogoOut[1][i + 6], &clsFunc.OnLogFont(14));
// 		strTemp.Format(_T("Pogo_%02d"), (m_nTsiteNum * 12) + (i + 7));
// 		m_pGridTsite.SetItemText(m_nTsitePogoOut[0][i + 6], m_nTsitePogoOut[1][i + 6], strTemp);
// 
// 		m_nTsitePogoIn[0][i + 6]	= 7;
// 		m_nTsitePogoIn[1][i + 6]	= (i * 2) + 1;
// 		m_pGridTsite.MergeCells(m_nTsitePogoIn[0][i + 6], m_nTsitePogoIn[1][i + 6], m_nTsitePogoIn[0][i + 6], m_nTsitePogoIn[1][i + 6] + 1);
// 		m_pGridTsite.SetItemBkColour(m_nTsitePogoIn[0][i + 6], m_nTsitePogoIn[1][i + 6], YELLOW_D, BLACK_C);
// 		m_pGridTsite.SetItemFont(m_nTsitePogoIn[0][i + 6], m_nTsitePogoIn[1][i + 6], &clsFunc.OnLogFont(14));
// 		m_pGridTsite.SetItemText(m_nTsitePogoIn[0][i + 6], m_nTsitePogoIn[1][i + 6], _T("Dn"));
// 	}

	OnDisplayTestSite(m_nTsiteNum);
}

void CDialogManualIO::OnInitGridTsiteRbt()
{
	int   i, j;
	int	  nMaxRow, nMaxCol;
//	int   nRow, nCol;
	CString strTmp;

	nMaxRow = 6; // unloader tray y count
	nMaxCol = 9;

	m_pGridTRbt.SetFrameFocusCell(FALSE);
	m_pGridTRbt.SetTrackFocusCell(FALSE);
	m_pGridTRbt.EnableSelection(FALSE);

	m_pGridTRbt.SetGridLineColor(BLACK_C);
	m_pGridTRbt.SetGridLines(1);

	m_pGridTRbt.SetRowCount(nMaxRow);
	m_pGridTRbt.SetColumnCount(nMaxCol);

	m_pGridTRbt.SetFixedRowCount(0);
	m_pGridTRbt.SetFixedColumnCount(0);
	m_pGridTRbt.SetFixedBkColor(RGB(0,0,200));
	m_pGridTRbt.SetFixedBkColor(RGB(200,200,255));
	m_pGridTRbt.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<nMaxRow; i++)
	{
		m_pGridTRbt.SetRowHeight(i, 25);

		for (j=0; j<nMaxCol; j++)
		{
			m_pGridTRbt.SetColumnWidth(j, 60);

			m_pGridTRbt.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridTRbt.SetItemState(i, j, GVIS_READONLY);
			m_pGridTRbt.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
		}
	}

	m_pGridTRbt.MergeCells(0, 0, 0, 8);
	m_pGridTRbt.SetItemBkColour(0, 0, BLUE_D, WHITE_C);
	m_pGridTRbt.SetItemFont(0, 0, &clsFunc.OnLogFont(15));
	m_pGridTRbt.SetItemText(0, 0, _T("Test Site Robot"));

	m_pGridTRbt.MergeCells(1, 1, 1, 2);
	m_pGridTRbt.SetItemBkColour(1, 1, YELLOW_L, BLACK_C);
	m_pGridTRbt.SetItemFont(1, 1, &clsFunc.OnLogFont(15));
	m_pGridTRbt.SetItemText(1, 1, _T("Picker #1"));

	m_pGridTRbt.MergeCells(1, 3, 1, 4);
	m_pGridTRbt.SetItemBkColour(1, 3, YELLOW_L, BLACK_C);
	m_pGridTRbt.SetItemFont(1, 3, &clsFunc.OnLogFont(15));
	m_pGridTRbt.SetItemText(1, 3, _T("Picker #2"));

	m_pGridTRbt.MergeCells(1, 5, 1, 6);
	m_pGridTRbt.SetItemBkColour(1, 5, YELLOW_L, BLACK_C);
	m_pGridTRbt.SetItemFont(1, 5, &clsFunc.OnLogFont(15));
	m_pGridTRbt.SetItemText(1, 5, _T("Picker #3"));

	m_pGridTRbt.MergeCells(1, 7, 1, 8);
	m_pGridTRbt.SetItemBkColour(1, 7, YELLOW_L, BLACK_C);
	m_pGridTRbt.SetItemFont(1, 7, &clsFunc.OnLogFont(15));
	m_pGridTRbt.SetItemText(1, 7, _T("Picker #4"));

	m_pGridTRbt.MergeCells(2, 0, 3, 0);
	m_pGridTRbt.SetItemBkColour(2, 0, BLUE_D, WHITE_C);
	m_pGridTRbt.SetItemFont(2, 0, &clsFunc.OnLogFont(15));
	m_pGridTRbt.SetItemText(2, 0, _T("Picker"));

	m_pGridTRbt.MergeCells(4, 0, 5, 0);
	m_pGridTRbt.SetItemBkColour(4, 0, BLUE_D, WHITE_C);
	m_pGridTRbt.SetItemFont(4, 0, &clsFunc.OnLogFont(15));
	m_pGridTRbt.SetItemText(4, 0, _T("Vaccum"));

// 	m_nTsiteCyl[0]		= st_io_info.o_TsiteRbtPicker1Cyl;
// 	m_nTsiteFwdChk[0]	= st_io_info.i_TsiteRbtPicker1FwdChk;
// 	m_nTsiteBwdChk[0]	= st_io_info.i_TsiteRbtPicker1BwdChk;
// 
// 	m_nTsiteCyl[1]		= st_io_info.o_TsiteRbtPicker2Cyl;
// 	m_nTsiteFwdChk[1]	= st_io_info.i_TsiteRbtPicker2FwdChk;
// 	m_nTsiteBwdChk[1]	= st_io_info.i_TsiteRbtPicker2BwdChk;
// 
// 	m_nTsiteCyl[2]		= st_io_info.o_TsiteRbtPicker3Cyl;
// 	m_nTsiteFwdChk[2]	= st_io_info.i_TsiteRbtPicker3FwdChk;
// 	m_nTsiteBwdChk[2]	= st_io_info.i_TsiteRbtPicker3BwdChk;
// 
// 	m_nTsiteCyl[3]		= st_io_info.o_TsiteRbtPicker4Cyl;
// 	m_nTsiteFwdChk[3]	= st_io_info.i_TsiteRbtPicker4FwdChk;
// 	m_nTsiteBwdChk[3]	= st_io_info.i_TsiteRbtPicker4BwdChk;
// 
// 	m_nTsiteVac[0]		= st_io_info.o_TsiteRbtPicker1VacuumOnOff;
// 	m_nTsiteEjc[0]		= st_io_info.o_TsiteRbtPicker1EjectOnOff;
// 	m_nTsiteVacChk[0]	= st_io_info.i_TsiteRbtPicker1VacuumChk;
// 
// 	m_nTsiteVac[1]		= st_io_info.o_TsiteRbtPicker2VacuumOnOff;
// 	m_nTsiteEjc[1]		= st_io_info.o_TsiteRbtPicker2EjectOnOff;
// 	m_nTsiteVacChk[1]	= st_io_info.i_TsiteRbtPicker2VacuumChk;
// 
// 	m_nTsiteVac[2]		= st_io_info.o_TsiteRbtPicker3VacuumOnOff;
// 	m_nTsiteEjc[2]		= st_io_info.o_TsiteRbtPicker3EjectOnOff;
// 	m_nTsiteVacChk[2]	= st_io_info.i_TsiteRbtPicker3VacuumChk;
// 
// 	m_nTsiteVac[3]		= st_io_info.o_TsiteRbtPicker4VacuumOnOff;
// 	m_nTsiteEjc[3]		= st_io_info.o_TsiteRbtPicker4EjectOnOff;
// 	m_nTsiteVacChk[3]	= st_io_info.i_TsiteRbtPicker4VacuumChk;

// 	for (i=0; i<4; i++)
// 	{
// 		// down input
// 		m_nTsiteDnIn[0][i] = 2;
// 		m_nTsiteDnIn[1][i] = (i * 2) + 1;
// 		m_pGridTRbt.SetItemBkColour(m_nTsiteDnIn[0][i], m_nTsiteDnIn[1][i], YELLOW_D, BLACK_C);
// 		m_pGridTRbt.SetItemText(m_nTsiteDnIn[0][i], m_nTsiteDnIn[1][i], _T("Dn"));
// 
// 		// down output
// 		m_nTsiteDnOut[0][i] = 2;
// 		m_nTsiteDnOut[1][i] = (i * 2) + 2;
// 		m_pGridTRbt.MergeCells(m_nTsiteDnOut[0][i], m_nTsiteDnOut[1][i], m_nTsiteDnOut[0][i] + 1, m_nTsiteDnOut[1][i]);
// 		m_pGridTRbt.SetItemBkColour(m_nTsiteDnOut[0][i], m_nTsiteDnOut[1][i], GREEN_D, BLACK_C);
// 		m_pGridTRbt.SetItemText(m_nTsiteDnOut[0][i], m_nTsiteDnOut[1][i], _T("On/Off"));
// 
// 		// up input
// 		m_nTsiteUpIn[0][i] = 3;
// 		m_nTsiteUpIn[1][i] = (i * 2) + 1;
// 		m_pGridTRbt.SetItemBkColour(m_nTsiteUpIn[0][i], m_nTsiteUpIn[1][i], YELLOW_D, BLACK_C);
// 		m_pGridTRbt.SetItemText(m_nTsiteUpIn[0][i], m_nTsiteUpIn[1][i], _T("Up"));
/*
		// up output
		m_nTsiteUpOut[0][i] = 3;
		m_nTsiteUpOut[1][i] = (i * 2) + 2;
		m_pGridTRbt.SetItemBkColour(m_nTsiteUpOut[0][i], m_nTsiteUpOut[1][i], GREEN_D, BLACK_C);
		m_pGridTRbt.SetItemText(m_nTsiteUpOut[0][i], m_nTsiteUpOut[1][i], _T("Up"));
*/
		////////////////////////////////////////////////////////////////////////////////////////
		// vaccum input
// 		m_nTsiteVacIn[0][i] = 4;
// 		m_nTsiteVacIn[1][i] = (i * 2) + 1;
// 		m_pGridTRbt.MergeCells(m_nTsiteVacIn[0][i], m_nTsiteVacIn[1][i], m_nTsiteVacIn[0][i] + 1, m_nTsiteVacIn[1][i]);
// 		m_pGridTRbt.SetItemBkColour(m_nTsiteVacIn[0][i], m_nTsiteVacIn[1][i], YELLOW_D, BLACK_C);
// 		m_pGridTRbt.SetItemText(m_nTsiteVacIn[0][i], m_nTsiteVacIn[1][i], _T("Off"));

		// vaccum output
// 		m_nTsiteVacOut[0][i] = 4;
// 		m_nTsiteVacOut[1][i] = (i * 2) + 2;
// 		m_pGridTRbt.SetItemBkColour(m_nTsiteVacOut[0][i], m_nTsiteVacOut[1][i], GREEN_D, BLACK_C);
// 		m_pGridTRbt.SetItemText(m_nTsiteVacOut[0][i], m_nTsiteVacOut[1][i], _T("Vac"));

		// eject output
// 		m_nTsiteEjcOut[0][i] = 5;
// 		m_nTsiteEjcOut[1][i] = (i * 2) + 2;
// 		m_pGridTRbt.SetItemBkColour(m_nTsiteEjcOut[0][i], m_nTsiteEjcOut[1][i], GREEN_D, BLACK_C);
// 		m_pGridTRbt.SetItemText(m_nTsiteEjcOut[0][i], m_nTsiteEjcOut[1][i], _T("Ejc"));
	//}

	//m_pGridTRbt.Invalidate(FALSE);
}

void CDialogManualIO::OnInitGridLdRbt()
{
	int   i, j;
	int	  nMaxRow, nMaxCol;
//	int   nRow, nCol;
	CString strTmp;

	nMaxRow = 6; // unloader tray y count
	nMaxCol = 5;

	m_pGridLdRbt.SetFrameFocusCell(FALSE);
	m_pGridLdRbt.SetTrackFocusCell(FALSE);
	m_pGridLdRbt.EnableSelection(FALSE);

	m_pGridLdRbt.SetGridLineColor(BLACK_C);
	m_pGridLdRbt.SetGridLines(1);

	m_pGridLdRbt.SetRowCount(nMaxRow);
	m_pGridLdRbt.SetColumnCount(nMaxCol);

	m_pGridLdRbt.SetFixedRowCount(0);
	m_pGridLdRbt.SetFixedColumnCount(0);
	m_pGridLdRbt.SetFixedBkColor(RGB(0,0,200));
	m_pGridLdRbt.SetFixedBkColor(RGB(200,200,255));
	m_pGridLdRbt.SetTextBkColor(RGB(150,150,200));

// 	m_nLdCyl[0]		= st_io_info.o_LdRbtFrontPickerCyl;
// 	m_nLdFwdChk[0]	= st_io_info.i_LdRbtFrontPickerFwdChk;
// 	m_nLdBwdChk[0]	= st_io_info.i_LdRbtFrontPickerBwdChk;
// 
// 	m_nLdCyl[1]		= st_io_info.o_LdRbtRearPickerCyl;
// 	m_nLdFwdChk[1]	= st_io_info.i_LdRbtRearPickerFwdChk;
// 	m_nLdBwdChk[1]	= st_io_info.i_LdRbtRearPickerBwdChk;
// 
// 	m_nLdVac[0]		= st_io_info.o_LdRbtFrontPickerVacuumOnOff;
// 	m_nLdEjc[0]		= st_io_info.o_LdRbtFrontPickerEjectOnOff;
// 	m_nLdVacChk[0]	= st_io_info.i_LdRbtFrontPickerVacuumChk;
// 
// 	m_nLdVac[1]		= st_io_info.o_LdRbtRearPickerVacuumOnOff;
// 	m_nLdEjc[1]		= st_io_info.o_LdRbtRearPickerEjectOnOff;
// 	m_nLdVacChk[1]	= st_io_info.i_LdRbtRearPickerVacuumChk;

	for (i=0; i<nMaxRow; i++)
	{
		m_pGridLdRbt.SetRowHeight(i, 25);

		for (j=0; j<nMaxCol; j++)
		{
			m_pGridLdRbt.SetColumnWidth(j, 60);

			m_pGridLdRbt.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridLdRbt.SetItemState(i, j, GVIS_READONLY);
			m_pGridLdRbt.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
		}
	}

	m_pGridLdRbt.MergeCells(0, 0, 0, 4);
	m_pGridLdRbt.SetItemBkColour(0, 0, BLUE_D, WHITE_C);
	m_pGridLdRbt.SetItemFont(0, 0, &clsFunc.OnLogFont(15));
	m_pGridLdRbt.SetItemText(0, 0, _T("Loader Robot"));

	m_pGridLdRbt.MergeCells(1, 1, 1, 2);
	m_pGridLdRbt.SetItemBkColour(1, 1, YELLOW_L, BLACK_C);
	m_pGridLdRbt.SetItemFont(1, 1, &clsFunc.OnLogFont(15));
	m_pGridLdRbt.SetItemText(1, 1, _T("Picker #1"));

	m_pGridLdRbt.MergeCells(1, 3, 1, 4);
	m_pGridLdRbt.SetItemBkColour(1, 3, YELLOW_L, BLACK_C);
	m_pGridLdRbt.SetItemFont(1, 3, &clsFunc.OnLogFont(15));
	m_pGridLdRbt.SetItemText(1, 3, _T("Picker #2"));

	m_pGridLdRbt.MergeCells(2, 0, 3, 0);
	m_pGridLdRbt.SetItemBkColour(2, 0, BLUE_D, WHITE_C);
	m_pGridLdRbt.SetItemFont(2, 0, &clsFunc.OnLogFont(15));
	m_pGridLdRbt.SetItemText(2, 0, _T("Picker"));

	m_pGridLdRbt.MergeCells(4, 0, 5, 0);
	m_pGridLdRbt.SetItemBkColour(4, 0, BLUE_D, WHITE_C);
	m_pGridLdRbt.SetItemFont(4, 0, &clsFunc.OnLogFont(15));
	m_pGridLdRbt.SetItemText(4, 0, _T("Vaccum"));

// 	for (i=0; i<2; i++)
// 	{
// 		// down input
// 		m_nLdDnIn[0][i] = 2;
// 		m_nLdDnIn[1][i] = (i * 2) + 1;
// 		m_pGridLdRbt.SetItemBkColour(m_nLdDnIn[0][i], m_nLdDnIn[1][i], YELLOW_D, BLACK_C);
// 		m_pGridLdRbt.SetItemText(m_nLdDnIn[0][i], m_nLdDnIn[1][i], _T("Dn"));
// 
// 		// down output
// 		m_nLdDnOut[0][i] = 2;
// 		m_nLdDnOut[1][i] = (i * 2) + 2;
// 		m_pGridLdRbt.MergeCells(m_nLdDnOut[0][i], m_nLdDnOut[1][i], m_nLdDnOut[0][i] + 1, m_nLdDnOut[1][i]);
// 		m_pGridLdRbt.SetItemBkColour(m_nLdDnOut[0][i], m_nLdDnOut[1][i], GREEN_D, BLACK_C);
// 		m_pGridLdRbt.SetItemText(m_nLdDnOut[0][i], m_nLdDnOut[1][i], _T("Dn/Up"));
// 
// 		// up input
// 		m_nLdUpIn[0][i] = 3;
// 		m_nLdUpIn[1][i] = (i * 2) + 1;
// 		m_pGridLdRbt.SetItemBkColour(m_nLdUpIn[0][i], m_nLdUpIn[1][i], YELLOW_D, BLACK_C);
// 		m_pGridLdRbt.SetItemText(m_nLdUpIn[0][i], m_nLdUpIn[1][i], _T("Up"));
// /*
// 		// up output
// 		m_nLdUpOut[0][i] = 3;
// 		m_nLdUpOut[1][i] = (i * 2) + 2;
// 		m_pGridLdRbt.SetItemBkColour(m_nLdUpOut[0][i], m_nLdUpOut[1][i], GREEN_D, BLACK_C);
// 		m_pGridLdRbt.SetItemText(m_nLdUpOut[0][i], m_nLdUpOut[1][i], _T("Up"));
// */
// 		////////////////////////////////////////////////////////////////////////////////////////
// 		// vaccum input
// 		m_nLdVacIn[0][i] = 4;
// 		m_nLdVacIn[1][i] = (i * 2) + 1;
// 		m_pGridLdRbt.MergeCells(m_nLdVacIn[0][i], m_nLdVacIn[1][i], m_nLdVacIn[0][i] + 1, m_nLdVacIn[1][i]);
// 		m_pGridLdRbt.SetItemBkColour(m_nLdVacIn[0][i], m_nLdVacIn[1][i], YELLOW_D, BLACK_C);
// 		m_pGridLdRbt.SetItemText(m_nLdVacIn[0][i], m_nLdVacIn[1][i], _T("Off"));
// 
// 		// vaccum output
// 		m_nLdVacOut[0][i] = 4;
// 		m_nLdVacOut[1][i] = (i * 2) + 2;
// 		m_pGridLdRbt.SetItemBkColour(m_nLdVacOut[0][i], m_nLdVacOut[1][i], GREEN_D, BLACK_C);
// 		m_pGridLdRbt.SetItemText(m_nLdVacOut[0][i], m_nLdVacOut[1][i], _T("Vac"));
// 
// 		// eject output
// 		m_nLdEjcOut[0][i] = 5;
// 		m_nLdEjcOut[1][i] = (i * 2) + 2;
// 		m_pGridLdRbt.SetItemBkColour(m_nLdEjcOut[0][i], m_nLdEjcOut[1][i], GREEN_D, BLACK_C);
// 		m_pGridLdRbt.SetItemText(m_nLdEjcOut[0][i], m_nLdEjcOut[1][i], _T("Ejc"));
// 	}

	m_pGridLdRbt.Invalidate(FALSE);
}

// void CDialogManualIO::OnInitGridUldRbt()
// {
// 	int   i, j;
// 	int	  nMaxRow, nMaxCol;
// //	int   nRow, nCol;
// 	CString strTmp;
// 
// 	nMaxRow = 6; // unloader tray y count
// 	nMaxCol = 5;
// 
// 	m_pGridUldRbt.SetFrameFocusCell(FALSE);
// 	m_pGridUldRbt.SetTrackFocusCell(FALSE);
// 	m_pGridUldRbt.EnableSelection(FALSE);
// 
// 	m_pGridUldRbt.SetGridLineColor(BLACK_C);
// 	m_pGridUldRbt.SetGridLines(1);
// 
// 	m_pGridUldRbt.SetRowCount(nMaxRow);
// 	m_pGridUldRbt.SetColumnCount(nMaxCol);
// 
// 	m_pGridUldRbt.SetFixedRowCount(0);
// 	m_pGridUldRbt.SetFixedColumnCount(0);
// 	m_pGridUldRbt.SetFixedBkColor(RGB(0,0,200));
// 	m_pGridUldRbt.SetFixedBkColor(RGB(200,200,255));
// 	m_pGridUldRbt.SetTextBkColor(RGB(150,150,200));
// 
// 	for (i=0; i<nMaxRow; i++)
// 	{
// 		m_pGridUldRbt.SetRowHeight(i, 25);
// 
// 		for (j=0; j<nMaxCol; j++)
// 		{
// 			m_pGridUldRbt.SetColumnWidth(j, 60);
// 
// 			m_pGridUldRbt.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
// 			m_pGridUldRbt.SetItemState(i, j, GVIS_READONLY);
// 			m_pGridUldRbt.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
// 		}
// 	}
// 
// 	m_pGridUldRbt.MergeCells(0, 0, 0, 4);
// 	m_pGridUldRbt.SetItemBkColour(0, 0, BLUE_D, WHITE_C);
// 	m_pGridUldRbt.SetItemFont(0, 0, &clsFunc.OnLogFont(15));
// 	m_pGridUldRbt.SetItemText(0, 0, _T("UnLoader Robot"));
// 
// 	m_pGridUldRbt.MergeCells(1, 1, 1, 2);
// 	m_pGridUldRbt.SetItemBkColour(1, 1, YELLOW_L, BLACK_C);
// 	m_pGridUldRbt.SetItemFont(1, 1, &clsFunc.OnLogFont(15));
// 	m_pGridUldRbt.SetItemText(1, 1, _T("Picker #1"));
// 
// 	m_pGridUldRbt.MergeCells(1, 3, 1, 4);
// 	m_pGridUldRbt.SetItemBkColour(1, 3, YELLOW_L, BLACK_C);
// 	m_pGridUldRbt.SetItemFont(1, 3, &clsFunc.OnLogFont(15));
// 	m_pGridUldRbt.SetItemText(1, 3, _T("Picker #2"));
// 
// 	m_pGridUldRbt.MergeCells(2, 0, 3, 0);
// 	m_pGridUldRbt.SetItemBkColour(2, 0, BLUE_D, WHITE_C);
// 	m_pGridUldRbt.SetItemFont(2, 0, &clsFunc.OnLogFont(15));
// 	m_pGridUldRbt.SetItemText(2, 0, _T("Picker"));
// 
// 	m_pGridUldRbt.MergeCells(4, 0, 5, 0);
// 	m_pGridUldRbt.SetItemBkColour(4, 0, BLUE_D, WHITE_C);
// 	m_pGridUldRbt.SetItemFont(4, 0, &clsFunc.OnLogFont(15));
// 	m_pGridUldRbt.SetItemText(4, 0, _T("Vaccum"));
// 
// // 	m_nUnLdCyl[0]		= st_io_info.o_UnldRbtFrontPickerCyl;
// // 	m_nUnLdFwdChk[0]	= st_io_info.i_UnldRbtFrontPickerFwdChk;
// // 	m_nUnLdBwdChk[0]	= st_io_info.i_UnldRbtFrontPickerBwdChk;
// // 
// // 	m_nUnLdCyl[1]		= st_io_info.o_UnldRbtRearPickerCyl;
// // 	m_nUnLdFwdChk[1]	= st_io_info.i_UnldRbtRearPickerFwdChk;
// // 	m_nUnLdBwdChk[1]	= st_io_info.i_UnldRbtRearPickerBwdChk;
// // 
// // 	m_nUnLdVac[0]		= st_io_info.o_UnldRbtFrontPickerVacuumOnOff;
// // 	m_nUnLdEjc[0]		= st_io_info.o_UnldRbtFrontPickerEjectOnOff;
// // 	m_nUnLdVacChk[0]	= st_io_info.i_UnldRbtFrontPickerVacuumChk;
// // 
// // 	m_nUnLdVac[1]		= st_io_info.o_UnldRbtRearPickerVacuumOnOff;
// // 	m_nUnLdEjc[1]		= st_io_info.o_UnldRbtRearPickerEjectOnOff;
// // 	m_nUnLdVacChk[1]	= st_io_info.i_UnldRbtRearPickerVacuumChk;
// 
// 	for (i=0; i<2; i++)
// 	{
// 		// down input
// 		m_nUnLdDnIn[0][i] = 2;
// 		m_nUnLdDnIn[1][i] = (i * 2) + 1;
// 		m_pGridUldRbt.SetItemBkColour(m_nUnLdDnIn[0][i], m_nUnLdDnIn[1][i], YELLOW_D, BLACK_C);
// 		m_pGridUldRbt.SetItemText(m_nUnLdDnIn[0][i], m_nUnLdDnIn[1][i], _T("Dn"));
// 
// 		// down output
// 		m_nUnLdDnOut[0][i] = 2;
// 		m_nUnLdDnOut[1][i] = (i * 2) + 2;
// 		m_pGridUldRbt.MergeCells(m_nUnLdDnOut[0][i], m_nUnLdDnOut[1][i], m_nUnLdDnOut[0][i] + 1, m_nUnLdDnOut[1][i]);
// 		m_pGridUldRbt.SetItemBkColour(m_nUnLdDnOut[0][i], m_nUnLdDnOut[1][i], GREEN_D, BLACK_C);
// 		m_pGridUldRbt.SetItemText(m_nUnLdDnOut[0][i], m_nUnLdDnOut[1][i], _T("Dn"));
// 
// 		// up input
// 		m_nUnLdUpIn[0][i] = 3;
// 		m_nUnLdUpIn[1][i] = (i * 2) + 1;
// 		m_pGridUldRbt.SetItemBkColour(m_nUnLdUpIn[0][i], m_nUnLdUpIn[1][i], YELLOW_D, BLACK_C);
// 		m_pGridUldRbt.SetItemText(m_nUnLdUpIn[0][i], m_nUnLdUpIn[1][i], _T("Up"));
// /*
// 		// up output
// 		m_nLdUpOut[0][i] = 3;
// 		m_nLdUpOut[1][i] = (i * 2) + 2;
// 		m_pGridUldRbt.SetItemBkColour(m_nLdUpOut[0][i], m_nLdUpOut[1][i], GREEN_D, BLACK_C);
// 		m_pGridUldRbt.SetItemText(m_nLdUpOut[0][i], m_nLdUpOut[1][i], _T("Up"));
// */
// 		////////////////////////////////////////////////////////////////////////////////////////
// 		// vaccum input
// 		m_nUnLdVacIn[0][i] = 4;
// 		m_nUnLdVacIn[1][i] = (i * 2) + 1;
// 		m_pGridUldRbt.MergeCells(m_nUnLdVacIn[0][i], m_nUnLdVacIn[1][i], m_nUnLdVacIn[0][i] + 1, m_nUnLdVacIn[1][i]);
// 		m_pGridUldRbt.SetItemBkColour(m_nUnLdVacIn[0][i], m_nUnLdVacIn[1][i], YELLOW_D, BLACK_C);
// 		m_pGridUldRbt.SetItemText(m_nUnLdVacIn[0][i], m_nUnLdVacIn[1][i], _T("Off"));
// 
// 		// vaccum output
// 		m_nUnLdVacOut[0][i] = 4;
// 		m_nUnLdVacOut[1][i] = (i * 2) + 2;
// 		m_pGridUldRbt.SetItemBkColour(m_nUnLdVacOut[0][i], m_nUnLdVacOut[1][i], GREEN_D, BLACK_C);
// 		m_pGridUldRbt.SetItemText(m_nUnLdVacOut[0][i], m_nUnLdVacOut[1][i], _T("Vac"));
// 
// 		// eject output
// 		m_nUnLdEjcOut[0][i] = 5;
// 		m_nUnLdEjcOut[1][i] = (i * 2) + 2;
// 		m_pGridUldRbt.SetItemBkColour(m_nUnLdEjcOut[0][i], m_nUnLdEjcOut[1][i], GREEN_D, BLACK_C);
// 		m_pGridUldRbt.SetItemText(m_nUnLdEjcOut[0][i], m_nUnLdEjcOut[1][i], _T("Ejc"));
// 	}
// 
// 	m_pGridUldRbt.Invalidate(FALSE);
// }

// void CDialogManualIO::OnInitGridLfContact()
// {
// 	int   i, j;
// 	int	  nMaxRow, nMaxCol;
// //	int   nRow, nCol;
// 	CString strTmp;
// 
// 	nMaxRow = 14; // unloader tray y count
// 	nMaxCol = 5;
// 
// 	m_pGridLfContact.SetFrameFocusCell(FALSE);
// 	m_pGridLfContact.SetTrackFocusCell(FALSE);
// 	m_pGridLfContact.EnableSelection(FALSE);
// 
// 	m_pGridLfContact.SetGridLineColor(BLACK_C);
// 	m_pGridLfContact.SetGridLines(1);
// 
// 	m_pGridLfContact.SetRowCount(nMaxRow);
// 	m_pGridLfContact.SetColumnCount(nMaxCol);
// 
// 	m_pGridLfContact.SetFixedRowCount(0);
// 	m_pGridLfContact.SetFixedColumnCount(0);
// 	m_pGridLfContact.SetFixedBkColor(RGB(0,0,200));
// 	m_pGridLfContact.SetFixedBkColor(RGB(200,200,255));
// 	m_pGridLfContact.SetTextBkColor(RGB(150,150,200));
// 
// 	for (i=0; i<nMaxRow; i++)
// 	{
// 		m_pGridLfContact.SetRowHeight(i, 25);
// 
// 		for (j=0; j<nMaxCol; j++)
// 		{
// 			if (j == 0)
// 			{
// 				m_pGridLfContact.SetColumnWidth(j, 100);
// 			}
// 			else 
// 			{
// 				m_pGridLfContact.SetColumnWidth(j, 70);
// 			}
// 
// 			m_pGridLfContact.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
// 			m_pGridLfContact.SetItemState(i, j, GVIS_READONLY);
// 			m_pGridLfContact.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
// 		}
// 	}

// 	m_nLfCokCyl[0]				= st_io_info.o_LfTsiteBtmCokClampCyl;
// 	m_nLfCokFwdChk[0][0]		= st_io_info.i_LfTsiteBtmCokLfClampFwdChk;
// 	m_nLfCokFwdChk[0][1]		= st_io_info.i_LfTsiteBtmCokRiClampFwdChk;
// 	m_nLfCokBwdChk[0][0]		= st_io_info.i_LfTsiteBtmCokLfClampBwdChk;
// 	m_nLfCokBwdChk[0][1]		= st_io_info.i_LfTsiteBtmCokRiClampBwdChk;
// 
// 	m_nLfCokCyl[1]				= st_io_info.o_LfTsiteTopCokClampCyl;
// 	m_nLfCokFwdChk[1][0]		= st_io_info.i_LfTsiteTopCokLfClampFwdChk;
// 	m_nLfCokFwdChk[1][1]		= st_io_info.i_LfTsiteTopCokRiClampFwdChk;
// 	m_nLfCokBwdChk[1][0]		= st_io_info.i_LfTsiteTopCokLfClampBwdChk;
// 	m_nLfCokBwdChk[1][1]		= st_io_info.i_LfTsiteTopCokRiClampBwdChk;
// 
// 	m_nLfContactFwdCyl[0]		= st_io_info.o_LfTsiteBtmTesterContactFwdCyl;
// 	m_nLfContactBwdCyl[0]		= st_io_info.o_LfTsiteBtmTesterContactBwdCyl;;
// 	m_nLfContactFwdChk[0][0]	= st_io_info.i_LfTsiteBtmTesterLfContactFwdChk;
// 	m_nLfContactFwdChk[0][1]	= st_io_info.i_LfTsiteBtmTesterRiContactFwdChk;
// 	m_nLfContactBwdChk[0][0]	= st_io_info.i_LfTsiteBtmTesterLfContactBwdChk;
// 	m_nLfContactBwdChk[0][1]	= st_io_info.i_LfTsiteBtmTesterRiContactBwdChk;
// 
// 	m_nLfContactFwdCyl[1]		= st_io_info.o_LfTsiteTopTesterContactFwdCyl;
// 	m_nLfContactBwdCyl[1]		= st_io_info.o_LfTsiteTopTesterContactBwdCyl;;
// 	m_nLfContactFwdChk[1][0]	= st_io_info.i_LfTsiteTopTesterLfContactFwdChk;
// 	m_nLfContactFwdChk[1][1]	= st_io_info.i_LfTsiteTopTesterRiContactFwdChk;
// 	m_nLfContactBwdChk[1][0]	= st_io_info.i_LfTsiteTopTesterLfContactBwdChk;
// 	m_nLfContactBwdChk[1][1]	= st_io_info.i_LfTsiteTopTesterRiContactBwdChk;
// 
// 	m_nLfHifixFwdCyl			= st_io_info.o_LfTsiteLfHiFixContactFwdCyl;
// 	m_nLfHifixBwdCyl			= st_io_info.o_LfTsiteLfHiFixContactBwdCyl;
// 	m_nLfHifixFwdChk[0]			= st_io_info.i_LfTsiteLfBtmHiFixContactFwdChk;
// 	m_nLfHifixFwdChk[1]			= st_io_info.i_LfTsiteLfTopHiFixContactFwdChk;
// 	m_nLfHifixFwdChk[2]			= st_io_info.i_LfTsiteRiTopHiFixContactFwdChk;
// 	m_nLfHifixBwdChk[0]			= st_io_info.i_LfTsiteLfBtmHiFixContactBwdChk;
// 	m_nLfHifixBwdChk[1]			= st_io_info.i_LfTsiteLfTopHiFixContactBwdChk;
// 	m_nLfHifixBwdChk[2]			= st_io_info.i_LfTsiteRiTopHiFixContactBwdChk;
// 
// 	m_nLfMpFwdCyl				= st_io_info.o_LfManipulatorClampFwdCyl;
// 	m_nLfMpBwdCyl				= st_io_info.o_LfManipulatorClampBwdCyl;
// 	m_nLfMpFwdChk[0]			= st_io_info.i_LfManipulatorLfClampFwdChk;
// 	m_nLfMpFwdChk[1]			= st_io_info.i_LfManipulatorRiClampFwdChk;
// 	m_nLfMpBwdChk[0]			= st_io_info.i_LfManipulatorLfClampBwdChk;
// 	m_nLfMpBwdChk[1]			= st_io_info.i_LfManipulatorLfClampBwdChk;

// 	m_pGridLfContact.MergeCells(0, 0, 0, 4);
// 	m_pGridLfContact.SetItemBkColour(0, 0, BLUE_D, WHITE_C);
// 	m_pGridLfContact.SetItemFont(0, 0, &clsFunc.OnLogFont(15));
// 	m_pGridLfContact.SetItemText(0, 0, _T("Left Hifix"));
// 
// 	// cok clamp
// 	m_pGridLfContact.MergeCells(1, 0, 3, 0);
// 	m_pGridLfContact.SetItemBkColour(1, 0, YELLOW_L, BLACK_C);
// 	m_pGridLfContact.SetItemFont(1, 0, &clsFunc.OnLogFont(15));
// 	m_pGridLfContact.SetItemText(1, 0, _T("Left COK Clamp"));
// 	// bottom
// 	m_nLfBCokClampOut[0]	= 1;
// 	m_nLfBCokClampOut[1]	= 1;
// 	m_pGridLfContact.MergeCells(m_nLfBCokClampOut[0], m_nLfBCokClampOut[1], m_nLfBCokClampOut[0], m_nLfBCokClampOut[1] + 1);
// 	m_pGridLfContact.SetItemBkColour(m_nLfBCokClampOut[0], m_nLfBCokClampOut[1], GREEN_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfBCokClampOut[0], m_nLfBCokClampOut[1], _T("Fwd/Bwd"));
// 
// 	m_nLfBCokClampFwd[0][0]	= 2;
// 	m_nLfBCokClampFwd[1][0]	= 1;
// 	m_pGridLfContact.SetItemBkColour(m_nLfBCokClampFwd[0][0], m_nLfBCokClampFwd[1][0], YELLOW_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfBCokClampFwd[0][0], m_nLfBCokClampFwd[1][0], _T("Lf Fwd"));
// 
// 	m_nLfBCokClampFwd[0][1]	= 3;
// 	m_nLfBCokClampFwd[1][1]	= 1;
// 	m_pGridLfContact.SetItemBkColour(m_nLfBCokClampFwd[0][1], m_nLfBCokClampFwd[1][1], YELLOW_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfBCokClampFwd[0][1], m_nLfBCokClampFwd[1][1], _T("Ri Fwd"));
// 
// 	m_nLfBCokClampBwd[0][0]	= 2;
// 	m_nLfBCokClampBwd[1][0]	= 2;
// 	m_pGridLfContact.SetItemBkColour(m_nLfBCokClampBwd[0][0], m_nLfBCokClampBwd[1][0], YELLOW_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfBCokClampBwd[0][0], m_nLfBCokClampBwd[1][0], _T("Lf Bwd"));
// 
// 	m_nLfBCokClampBwd[0][1]	= 3;
// 	m_nLfBCokClampBwd[1][1]	= 2;
// 	m_pGridLfContact.SetItemBkColour(m_nLfBCokClampBwd[0][1], m_nLfBCokClampBwd[1][1], YELLOW_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfBCokClampBwd[0][1], m_nLfBCokClampBwd[1][1], _T("Ri Bwd"));
// 	// top
// 	m_nLfTCokClampOut[0]	= 1;
// 	m_nLfTCokClampOut[1]	= 3;
// 	m_pGridLfContact.MergeCells(m_nLfTCokClampOut[0], m_nLfTCokClampOut[1], m_nLfTCokClampOut[0], m_nLfTCokClampOut[1] + 1);
// 	m_pGridLfContact.SetItemBkColour(m_nLfTCokClampOut[0], m_nLfTCokClampOut[1], GREEN_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfTCokClampOut[0], m_nLfTCokClampOut[1], _T("Fwd/Bwd"));
// 
// 	m_nLfTCokClampFwd[0][0]	= 2;
// 	m_nLfTCokClampFwd[1][0]	= 3;
// 	m_pGridLfContact.SetItemBkColour(m_nLfTCokClampFwd[0][0], m_nLfTCokClampFwd[1][0], YELLOW_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfTCokClampFwd[0][0], m_nLfTCokClampFwd[1][0], _T("Lf Fwd"));
// 
// 	m_nLfTCokClampFwd[0][1]	= 3;
// 	m_nLfTCokClampFwd[1][1]	= 3;
// 	m_pGridLfContact.SetItemBkColour(m_nLfTCokClampFwd[0][1], m_nLfTCokClampFwd[1][1], YELLOW_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfTCokClampFwd[0][1], m_nLfTCokClampFwd[1][1], _T("Ri Fwd"));
// 
// 	m_nLfTCokClampBwd[0][0]	= 2;
// 	m_nLfTCokClampBwd[1][0]	= 4;
// 	m_pGridLfContact.SetItemBkColour(m_nLfTCokClampBwd[0][0], m_nLfTCokClampBwd[1][0], YELLOW_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfTCokClampBwd[0][0], m_nLfTCokClampBwd[1][0], _T("Lf Bwd"));
// 
// 	m_nLfTCokClampBwd[0][1]	= 3;
// 	m_nLfTCokClampBwd[1][1]	= 4;
// 	m_pGridLfContact.SetItemBkColour(m_nLfTCokClampBwd[0][1], m_nLfTCokClampBwd[1][1], YELLOW_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfTCokClampBwd[0][1], m_nLfTCokClampBwd[1][1], _T("Ri Bwd"));
// 
// 	// left contact
// 	m_pGridLfContact.MergeCells(4, 0, 6, 0);
// 	m_pGridLfContact.SetItemBkColour(4, 0, YELLOW_L, BLACK_C);
// 	m_pGridLfContact.SetItemFont(4, 0, &clsFunc.OnLogFont(15));
// 	m_pGridLfContact.SetItemText(4, 0, _T("Left Contact"));
// 
// 	m_nLfBContactOut[0][0]	= 4;
// 	m_nLfBContactOut[1][0]	= 1;
// 	m_pGridLfContact.SetItemBkColour(m_nLfBContactOut[0][0], m_nLfBContactOut[1][0], GREEN_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfBContactOut[0][0], m_nLfBContactOut[1][0], _T("Fwd"));
// 
// 	m_nLfBContactOut[0][1]	= 4;
// 	m_nLfBContactOut[1][1]	= 2;
// 	m_pGridLfContact.SetItemBkColour(m_nLfBContactOut[0][1], m_nLfBContactOut[1][1], GREEN_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfBContactOut[0][1], m_nLfBContactOut[1][1], _T("Bwd"));
// 
// 	m_nLfBContactFwd[0][0]	= 5;
// 	m_nLfBContactFwd[1][0]	= 1;
// 	m_pGridLfContact.SetItemBkColour(m_nLfBContactFwd[0][0], m_nLfBContactFwd[1][0], YELLOW_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfBContactFwd[0][0], m_nLfBContactFwd[1][0], _T("Lf Fwd"));
// 
// 	m_nLfBContactFwd[0][1]	= 6;
// 	m_nLfBContactFwd[1][1]	= 1;
// 	m_pGridLfContact.SetItemBkColour(m_nLfBContactFwd[0][1], m_nLfBContactFwd[1][1], YELLOW_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfBContactFwd[0][1], m_nLfBContactFwd[1][1], _T("Ri Fwd"));
// 
// 	m_nLfBContactBwd[0][0]	= 5;
// 	m_nLfBContactBwd[1][0]	= 2;
// 	m_pGridLfContact.SetItemBkColour(m_nLfBContactBwd[0][0], m_nLfBContactBwd[1][0], YELLOW_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfBContactBwd[0][0], m_nLfBContactBwd[1][0], _T("Lf Bwd"));
// 
// 	m_nLfBContactBwd[0][1]	= 6;
// 	m_nLfBContactBwd[1][1]	= 2;
// 	m_pGridLfContact.SetItemBkColour(m_nLfBContactBwd[0][1], m_nLfBContactBwd[1][1], YELLOW_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfBContactBwd[0][1], m_nLfBContactBwd[1][1], _T("Ri Bwd"));
// 
// 	////top
// 	m_nLfTContactOut[0][0]	= 4;
// 	m_nLfTContactOut[1][0]	= 3;
// 	m_pGridLfContact.SetItemBkColour(m_nLfTContactOut[0][0], m_nLfTContactOut[1][0], GREEN_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfTContactOut[0][0], m_nLfTContactOut[1][0], _T("Fwd"));
// 
// 	m_nLfTContactOut[0][1]	= 4;
// 	m_nLfTContactOut[1][1]	= 4;
// 	m_pGridLfContact.SetItemBkColour(m_nLfTContactOut[0][1], m_nLfTContactOut[1][1], GREEN_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfTContactOut[0][1], m_nLfTContactOut[1][1], _T("Bwd"));
// 
// 	m_nLfTContactFwd[0][0]	= 5;
// 	m_nLfTContactFwd[1][0]	= 3;
// 	m_pGridLfContact.SetItemBkColour(m_nLfTContactFwd[0][0], m_nLfTContactFwd[1][0], YELLOW_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfTContactFwd[0][0], m_nLfTContactFwd[1][0], _T("Lf Fwd"));
// 
// 	m_nLfTContactFwd[0][1]	= 6;
// 	m_nLfTContactFwd[1][1]	= 3;
// 	m_pGridLfContact.SetItemBkColour(m_nLfTContactFwd[0][1], m_nLfTContactFwd[1][1], YELLOW_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfTContactFwd[0][1], m_nLfTContactFwd[1][1], _T("Ri Fwd"));
// 
// 	m_nLfTContactBwd[0][0]	= 5;
// 	m_nLfTContactBwd[1][0]	= 4;
// 	m_pGridLfContact.SetItemBkColour(m_nLfTContactBwd[0][0], m_nLfTContactBwd[1][0], YELLOW_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfTContactBwd[0][0], m_nLfTContactBwd[1][0], _T("Lf Bwd"));
// 
// 	m_nLfTContactBwd[0][1]	= 6;
// 	m_nLfTContactBwd[1][1]	= 4;
// 	m_pGridLfContact.SetItemBkColour(m_nLfTContactBwd[0][1], m_nLfTContactBwd[1][1], YELLOW_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfTContactBwd[0][1], m_nLfTContactBwd[1][1], _T("Ri Bwd"));
// 
// 	// left hifix
// 	m_pGridLfContact.MergeCells(7, 0, 10, 0);
// 	m_pGridLfContact.SetItemBkColour(7, 0, YELLOW_L, BLACK_C);
// 	m_pGridLfContact.SetItemFont(7, 0, &clsFunc.OnLogFont(15));
// 	m_pGridLfContact.SetItemText(7, 0, _T("Left Hifix"));
// 
// 	m_nLfHifixOut[0][0]	= 7;
// 	m_nLfHifixOut[1][0]	= 1;
// 	m_pGridLfContact.SetItemBkColour(m_nLfHifixOut[0][0], m_nLfHifixOut[1][0], GREEN_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfHifixOut[0][0], m_nLfHifixOut[1][0], _T("Fwd"));
// 
// 	m_nLfHifixOut[0][1]	= 7;
// 	m_nLfHifixOut[1][1]	= 2;
// 	m_pGridLfContact.SetItemBkColour(m_nLfHifixOut[0][1], m_nLfHifixOut[1][1], GREEN_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfHifixOut[0][1], m_nLfHifixOut[1][1], _T("Bwd"));
// 
// 	m_nLfHifixFwd[0][0]	= 8;
// 	m_nLfHifixFwd[1][0]	= 1;
// 	m_pGridLfContact.SetItemBkColour(m_nLfHifixFwd[0][0], m_nLfHifixFwd[1][0], YELLOW_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfHifixFwd[0][0], m_nLfHifixFwd[1][0], _T("Lf Top Fwd"));
// 
// 	m_nLfHifixFwd[0][1]	= 9;
// 	m_nLfHifixFwd[1][1]	= 1;
// 	m_pGridLfContact.SetItemBkColour(m_nLfHifixFwd[0][1], m_nLfHifixFwd[1][1], YELLOW_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfHifixFwd[0][1], m_nLfHifixFwd[1][1], _T("Lf Btm Fwd"));
// 
// 	m_nLfHifixFwd[0][2]	= 10;
// 	m_nLfHifixFwd[1][2]	= 1;
// 	m_pGridLfContact.SetItemBkColour(m_nLfHifixFwd[0][2], m_nLfHifixFwd[1][2], YELLOW_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfHifixFwd[0][2], m_nLfHifixFwd[1][2], _T("Ri Fwd"));
// 
// 	m_nLfHifixBwd[0][0]	= 8;
// 	m_nLfHifixBwd[1][0]	= 2;
// 	m_pGridLfContact.SetItemBkColour(m_nLfHifixBwd[0][0], m_nLfHifixBwd[1][0], YELLOW_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfHifixBwd[0][0], m_nLfHifixBwd[1][0], _T("Lf Top Bwd"));
// 
// 	m_nLfHifixBwd[0][1]	= 9;
// 	m_nLfHifixBwd[1][1]	= 2;
// 	m_pGridLfContact.SetItemBkColour(m_nLfHifixBwd[0][1], m_nLfHifixBwd[1][1], YELLOW_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfHifixBwd[0][1], m_nLfHifixBwd[1][1], _T("Lf Btm Bwd"));
// 
// 	m_nLfHifixBwd[0][2]	= 10;
// 	m_nLfHifixBwd[1][2]	= 2;
// 	m_pGridLfContact.SetItemBkColour(m_nLfHifixBwd[0][2], m_nLfHifixBwd[1][2], YELLOW_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfHifixBwd[0][2], m_nLfHifixBwd[1][2], _T("Ri Bwd"));
// 
// 	m_pGridLfContact.SetItemBkColour(7, 3, BLACK_L, YELLOW_C);
// 	m_pGridLfContact.SetItemText(7, 3, _T("-"));
// 
// 	m_pGridLfContact.SetItemBkColour(7, 4, BLACK_L, YELLOW_C);
// 	m_pGridLfContact.SetItemText(7, 4, _T("-"));
// 
// 	m_pGridLfContact.SetItemBkColour(8, 3, BLACK_L, YELLOW_C);
// 	m_pGridLfContact.SetItemText(8, 3, _T("-"));
// 
// 	m_pGridLfContact.SetItemBkColour(8, 4, BLACK_L, YELLOW_C);
// 	m_pGridLfContact.SetItemText(8, 4, _T("-"));
// 
// 	m_pGridLfContact.SetItemBkColour(9, 3, BLACK_L, YELLOW_C);
// 	m_pGridLfContact.SetItemText(9, 3, _T("-"));
// 
// 	m_pGridLfContact.SetItemBkColour(9, 4, BLACK_L, YELLOW_C);
// 	m_pGridLfContact.SetItemText(9, 4, _T("-"));
// 
// 	m_pGridLfContact.SetItemBkColour(10, 3, BLACK_L, YELLOW_C);
// 	m_pGridLfContact.SetItemText(10, 3, _T("-"));
// 
// 	m_pGridLfContact.SetItemBkColour(10, 4, BLACK_L, YELLOW_C);
// 	m_pGridLfContact.SetItemText(10, 4, _T("-"));
// 
// 	// left mp
// 	m_pGridLfContact.MergeCells(11, 0, 13, 0);
// 	m_pGridLfContact.SetItemBkColour(11, 0, YELLOW_L, BLACK_C);
// 	m_pGridLfContact.SetItemFont(11, 0, &clsFunc.OnLogFont(15));
// 	m_pGridLfContact.SetItemText(11, 0, _T("Left Mp"));
// 
// 	m_nLfMpOut[0][0]	= 11;
// 	m_nLfMpOut[1][0]	= 1;
// 	m_pGridLfContact.SetItemBkColour(m_nLfMpOut[0][0], m_nLfMpOut[1][0], GREEN_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfMpOut[0][0], m_nLfMpOut[1][0], _T("Fwd"));
// 
// 	m_nLfMpOut[0][1]	= 11;
// 	m_nLfMpOut[1][1]	= 2;
// 	m_pGridLfContact.SetItemBkColour(m_nLfMpOut[0][1], m_nLfMpOut[1][1], GREEN_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfMpOut[0][1], m_nLfMpOut[1][1], _T("Bwd"));
// 
// 	m_nLfMpFwd[0][0]	= 12;
// 	m_nLfMpFwd[1][0]	= 1;
// 	m_pGridLfContact.SetItemBkColour(m_nLfMpFwd[0][0], m_nLfMpFwd[1][0], YELLOW_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfMpFwd[0][0], m_nLfMpFwd[1][0], _T("Lf Fwd"));
// 
// 	m_nLfMpFwd[0][1]	= 13;
// 	m_nLfMpFwd[1][1]	= 1;
// 	m_pGridLfContact.SetItemBkColour(m_nLfMpFwd[0][1], m_nLfMpFwd[1][1], YELLOW_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfMpFwd[0][1], m_nLfMpFwd[1][1], _T("Ri Fwd"));
// 
// 	m_nLfMpBwd[0][0]	= 12;
// 	m_nLfMpBwd[1][0]	= 2;
// 	m_pGridLfContact.SetItemBkColour(m_nLfMpBwd[0][0], m_nLfMpBwd[1][0], YELLOW_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfMpBwd[0][0], m_nLfMpBwd[1][0], _T("Lf Bwd"));
// 
// 	m_nLfMpBwd[0][1]	= 13;
// 	m_nLfMpBwd[1][1]	= 2;
// 	m_pGridLfContact.SetItemBkColour(m_nLfMpBwd[0][1], m_nLfMpBwd[1][1], YELLOW_D, BLACK_C);
// 	m_pGridLfContact.SetItemText(m_nLfMpBwd[0][1], m_nLfMpBwd[1][1], _T("Ri Bwd"));
// 
// 	m_pGridLfContact.SetItemBkColour(11, 3, BLACK_L, YELLOW_C);
// 	m_pGridLfContact.SetItemText(11, 3, _T("-"));
// 
// 	m_pGridLfContact.SetItemBkColour(11, 4, BLACK_L, YELLOW_C);
// 	m_pGridLfContact.SetItemText(11, 4, _T("-"));
// 
// 	m_pGridLfContact.SetItemBkColour(12, 3, BLACK_L, YELLOW_C);
// 	m_pGridLfContact.SetItemText(12, 3, _T("-"));
// 
// 	m_pGridLfContact.SetItemBkColour(12, 4, BLACK_L, YELLOW_C);
// 	m_pGridLfContact.SetItemText(12, 4, _T("-"));
// 
// 	m_pGridLfContact.SetItemBkColour(13, 3, BLACK_L, YELLOW_C);
// 	m_pGridLfContact.SetItemText(13, 3, _T("-"));
// 
// 	m_pGridLfContact.SetItemBkColour(13, 4, BLACK_L, YELLOW_C);
// 	m_pGridLfContact.SetItemText(13, 4, _T("-"));
// 
// 	m_pGridLfContact.Invalidate(FALSE);
// }

// void CDialogManualIO::OnInitGridRiContact()
// {
// 	int   i, j;
// 	int	  nMaxRow, nMaxCol;
// //	int   nRow, nCol;
// 	CString strTmp;
// 
// 	nMaxRow = 14; // unloader tray y count
// 	nMaxCol = 5;
// 
// 	m_pGridRiContact.SetFrameFocusCell(FALSE);
// 	m_pGridRiContact.SetTrackFocusCell(FALSE);
// 	m_pGridRiContact.EnableSelection(FALSE);
// 
// 	m_pGridRiContact.SetGridLineColor(BLACK_C);
// 	m_pGridRiContact.SetGridLines(1);
// 
// 	m_pGridRiContact.SetRowCount(nMaxRow);
// 	m_pGridRiContact.SetColumnCount(nMaxCol);
// 
// 	m_pGridRiContact.SetFixedRowCount(0);
// 	m_pGridRiContact.SetFixedColumnCount(0);
// 	m_pGridRiContact.SetFixedBkColor(RGB(0,0,200));
// 	m_pGridRiContact.SetFixedBkColor(RGB(200,200,255));
// 	m_pGridRiContact.SetTextBkColor(RGB(150,150,200));
// 
// 	for (i=0; i<nMaxRow; i++)
// 	{
// 		m_pGridRiContact.SetRowHeight(i, 25);
// 
// 		for (j=0; j<nMaxCol; j++)
// 		{
// 			if (j == 0)
// 			{
// 				m_pGridRiContact.SetColumnWidth(j, 100);
// 			}
// 			else 
// 			{
// 				m_pGridRiContact.SetColumnWidth(j, 70);
// 			}
// 
// 			m_pGridRiContact.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
// 			m_pGridRiContact.SetItemState(i, j, GVIS_READONLY);
// 			m_pGridRiContact.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
// 		}
// 	}

// 	m_nRiCokCyl[0]				= st_io_info.o_RiTsiteBtmCokClampCyl;
// 	m_nRiCokFwdChk[0][0]		= st_io_info.i_RiTsiteBtmCokLfClampFwdChk;
// 	m_nRiCokFwdChk[0][1]		= st_io_info.i_RiTsiteBtmCokRiClampFwdChk;
// 	m_nRiCokBwdChk[0][0]		= st_io_info.i_RiTsiteBtmCokLfClampBwdChk;
// 	m_nRiCokBwdChk[0][1]		= st_io_info.i_RiTsiteBtmCokRiClampBwdChk;
// 
// 	m_nRiCokCyl[1]				= st_io_info.o_RiTsiteTopCokClampCyl;
// 	m_nRiCokFwdChk[1][0]		= st_io_info.i_RiTsiteTopCokLfClampFwdChk;
// 	m_nRiCokFwdChk[1][1]		= st_io_info.i_RiTsiteTopCokRiClampFwdChk;
// 	m_nRiCokBwdChk[1][0]		= st_io_info.i_RiTsiteTopCokLfClampBwdChk;
// 	m_nRiCokBwdChk[1][1]		= st_io_info.i_RiTsiteTopCokRiClampBwdChk;
// 
// 	m_nRiContactFwdCyl[0]		= st_io_info.o_RiTsiteBtmTesterContactFwdCyl;
// 	m_nRiContactBwdCyl[0]		= st_io_info.o_RiTsiteBtmTesterContactBwdCyl;;
// 	m_nRiContactFwdChk[0][0]	= st_io_info.i_RiTsiteBtmTesterLfContactFwdChk;
// 	m_nRiContactFwdChk[0][1]	= st_io_info.i_RiTsiteBtmTesterRiContactFwdChk;
// 	m_nRiContactBwdChk[0][0]	= st_io_info.i_RiTsiteBtmTesterLfContactBwdChk;
// 	m_nRiContactBwdChk[0][1]	= st_io_info.i_RiTsiteBtmTesterRiContactBwdChk;
// 
// 	m_nRiContactFwdCyl[1]		= st_io_info.o_RiTsiteTopTesterContactFwdCyl;
// 	m_nRiContactBwdCyl[1]		= st_io_info.o_RiTsiteTopTesterContactBwdCyl;;
// 	m_nRiContactFwdChk[1][0]	= st_io_info.i_RiTsiteTopTesterLfContactFwdChk;
// 	m_nRiContactFwdChk[1][1]	= st_io_info.i_RiTsiteTopTesterRiContactFwdChk;
// 	m_nRiContactBwdChk[1][0]	= st_io_info.i_RiTsiteTopTesterLfContactBwdChk;
// 	m_nRiContactBwdChk[1][1]	= st_io_info.i_RiTsiteTopTesterRiContactBwdChk;
// 
// 	m_nRiHifixFwdCyl			= st_io_info.o_RiTsiteLfHiFixContactFwdCyl;
// 	m_nRiHifixBwdCyl			= st_io_info.o_RiTsiteLfHiFixContactBwdCyl;
// 	m_nRiHifixFwdChk[0]			= st_io_info.i_RiTsiteLfBtmHiFixContactFwdChk;
// 	m_nRiHifixFwdChk[1]			= st_io_info.i_RiTsiteLfTopHiFixContactFwdChk;
// 	m_nRiHifixFwdChk[2]			= st_io_info.i_RiTsiteRiTopHiFixContactFwdChk;
// 	m_nRiHifixBwdChk[0]			= st_io_info.i_RiTsiteLfBtmHiFixContactBwdChk;
// 	m_nRiHifixBwdChk[1]			= st_io_info.i_RiTsiteLfTopHiFixContactBwdChk;
// 	m_nRiHifixBwdChk[2]			= st_io_info.i_RiTsiteRiTopHiFixContactBwdChk;
// 
// 	m_nRiMpFwdCyl				= st_io_info.o_RiManipulatorClampFwdCyl;
// 	m_nRiMpBwdCyl				= st_io_info.o_RiManipulatorClampBwdCyl;
// 	m_nRiMpFwdChk[0]			= st_io_info.i_RiManipulatorLfClampFwdChk;
// 	m_nRiMpFwdChk[1]			= st_io_info.i_RiManipulatorRiClampFwdChk;
// 	m_nRiMpBwdChk[0]			= st_io_info.i_RiManipulatorLfClampBwdChk;
// 	m_nRiMpBwdChk[1]			= st_io_info.i_RiManipulatorLfClampBwdChk;

// 	m_pGridRiContact.MergeCells(0, 0, 0, 4);
// 	m_pGridRiContact.SetItemBkColour(0, 0, BLUE_D, WHITE_C);
// 	m_pGridRiContact.SetItemFont(0, 0, &clsFunc.OnLogFont(15));
// 	m_pGridRiContact.SetItemText(0, 0, _T("Right Hifix"));
// 
// 	// cok clamp
// 	m_pGridRiContact.MergeCells(1, 0, 3, 0);
// 	m_pGridRiContact.SetItemBkColour(1, 0, YELLOW_L, BLACK_C);
// 	m_pGridRiContact.SetItemFont(1, 0, &clsFunc.OnLogFont(15));
// 	m_pGridRiContact.SetItemText(1, 0, _T("Right COK Clamp"));
// 
// 	// btm
// 	m_nRiBCokClampOut[0]	= 1;
// 	m_nRiBCokClampOut[1]	= 1;
// 	m_pGridRiContact.MergeCells(m_nRiBCokClampOut[0], m_nRiBCokClampOut[1], m_nRiBCokClampOut[0], m_nRiBCokClampOut[1] + 1);
// 	m_pGridRiContact.SetItemBkColour(m_nRiBCokClampOut[0], m_nRiBCokClampOut[1], GREEN_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiBCokClampOut[0], m_nRiBCokClampOut[1], _T("Fwd/Bwd"));
// 
// 	m_nRiBCokClampFwd[0][0]	= 2;
// 	m_nRiBCokClampFwd[1][0]	= 1;
// 	m_pGridRiContact.SetItemBkColour(m_nRiBCokClampFwd[0][0], m_nRiBCokClampFwd[1][0], YELLOW_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiBCokClampFwd[0][0], m_nRiBCokClampFwd[1][0], _T("Lf Fwd"));
// 
// 	m_nRiBCokClampFwd[0][1]	= 3;
// 	m_nRiBCokClampFwd[1][1]	= 1;
// 	m_pGridRiContact.SetItemBkColour(m_nRiBCokClampFwd[0][1], m_nRiBCokClampFwd[1][1], YELLOW_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiBCokClampFwd[0][1], m_nRiBCokClampFwd[1][1], _T("Ri Fwd"));
// 
// 	m_nRiBCokClampBwd[0][0]	= 2;
// 	m_nRiBCokClampBwd[1][0]	= 2;
// 	m_pGridRiContact.SetItemBkColour(m_nRiBCokClampBwd[0][0], m_nRiBCokClampBwd[1][0], YELLOW_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiBCokClampBwd[0][0], m_nRiBCokClampBwd[1][0], _T("Lf Bwd"));
// 
// 	m_nRiBCokClampBwd[0][1]	= 3;
// 	m_nRiBCokClampBwd[1][1]	= 2;
// 	m_pGridRiContact.SetItemBkColour(m_nRiBCokClampBwd[0][1], m_nRiBCokClampBwd[1][1], YELLOW_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiBCokClampBwd[0][1], m_nRiBCokClampBwd[1][1], _T("Ri Bwd"));
// 	//top
// 	m_nRiTCokClampOut[0]	= 1;
// 	m_nRiTCokClampOut[1]	= 3;
// 	m_pGridRiContact.MergeCells(m_nRiTCokClampOut[0], m_nRiTCokClampOut[1], m_nRiTCokClampOut[0], m_nRiTCokClampOut[1] + 1);
// 	m_pGridRiContact.SetItemBkColour(m_nRiTCokClampOut[0], m_nRiTCokClampOut[1], GREEN_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiTCokClampOut[0], m_nRiTCokClampOut[1], _T("Fwd/Bwd"));
// 
// 	m_nRiTCokClampFwd[0][0]	= 2;
// 	m_nRiTCokClampFwd[1][0]	= 3;
// 	m_pGridRiContact.SetItemBkColour(m_nRiTCokClampFwd[0][0], m_nRiTCokClampFwd[1][0], YELLOW_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiTCokClampFwd[0][0], m_nRiTCokClampFwd[1][0], _T("Lf Fwd"));
// 
// 	m_nRiTCokClampFwd[0][1]	= 3;
// 	m_nRiTCokClampFwd[1][1]	= 3;
// 	m_pGridRiContact.SetItemBkColour(m_nRiTCokClampFwd[0][1], m_nRiTCokClampFwd[1][1], YELLOW_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiTCokClampFwd[0][1], m_nRiTCokClampFwd[1][1], _T("Ri Fwd"));
// 
// 	m_nRiTCokClampBwd[0][0]	= 2;
// 	m_nRiTCokClampBwd[1][0]	= 4;
// 	m_pGridRiContact.SetItemBkColour(m_nRiTCokClampBwd[0][0], m_nRiTCokClampBwd[1][0], YELLOW_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiTCokClampBwd[0][0], m_nRiTCokClampBwd[1][0], _T("Lf Bwd"));
// 
// 	m_nRiTCokClampBwd[0][1]	= 3;
// 	m_nRiTCokClampBwd[1][1]	= 4;
// 	m_pGridRiContact.SetItemBkColour(m_nRiTCokClampBwd[0][1], m_nRiTCokClampBwd[1][1], YELLOW_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiTCokClampBwd[0][1], m_nRiTCokClampBwd[1][1], _T("Ri Bwd"));
// 
// 	// left contact
// 	m_pGridRiContact.MergeCells(4, 0, 6, 0);
// 	m_pGridRiContact.SetItemBkColour(4, 0, YELLOW_L, BLACK_C);
// 	m_pGridRiContact.SetItemFont(4, 0, &clsFunc.OnLogFont(15));
// 	m_pGridRiContact.SetItemText(4, 0, _T("Right Contact"));
// 	// bottom
// 	m_nRiBContactOut[0][0]	= 4;
// 	m_nRiBContactOut[1][0]	= 1;
// 	m_pGridRiContact.SetItemBkColour(m_nRiBContactOut[0][0], m_nRiBContactOut[1][0], GREEN_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiBContactOut[0][0], m_nRiBContactOut[1][0], _T("Fwd"));
// 
// 	m_nRiBContactOut[0][1]	= 4;
// 	m_nRiBContactOut[1][1]	= 2;
// 	m_pGridRiContact.SetItemBkColour(m_nRiBContactOut[0][1], m_nRiBContactOut[1][1], GREEN_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiBContactOut[0][1], m_nRiBContactOut[1][1], _T("Bwd"));
// 
// 	m_nRiBContactFwd[0][0]	= 5;
// 	m_nRiBContactFwd[1][0]	= 1;
// 	m_pGridRiContact.SetItemBkColour(m_nRiBContactFwd[0][0], m_nRiBContactFwd[1][0], YELLOW_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiBContactFwd[0][0], m_nRiBContactFwd[1][0], _T("Lf Fwd"));
// 
// 	m_nRiBContactFwd[0][1]	= 6;
// 	m_nRiBContactFwd[1][1]	= 1;
// 	m_pGridRiContact.SetItemBkColour(m_nRiBContactFwd[0][1], m_nRiBContactFwd[1][1], YELLOW_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiBContactFwd[0][1], m_nRiBContactFwd[1][1], _T("Ri Fwd"));
// 
// 	m_nRiBContactBwd[0][0]	= 5;
// 	m_nRiBContactBwd[1][0]	= 2;
// 	m_pGridRiContact.SetItemBkColour(m_nRiBContactBwd[0][0], m_nRiBContactBwd[1][0], YELLOW_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiBContactBwd[0][0], m_nRiBContactBwd[1][0], _T("Lf Bwd"));
// 
// 	m_nRiBContactBwd[0][1]	= 6;
// 	m_nRiBContactBwd[1][1]	= 2;
// 	m_pGridRiContact.SetItemBkColour(m_nRiBContactBwd[0][1], m_nRiBContactBwd[1][1], YELLOW_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiBContactBwd[0][1], m_nRiBContactBwd[1][1], _T("Ri Bwd"));
// 	//TOP
// 	m_nRiTContactOut[0][0]	= 4;
// 	m_nRiTContactOut[1][0]	= 3;
// 	m_pGridRiContact.SetItemBkColour(m_nRiTContactOut[0][0], m_nRiTContactOut[1][0], GREEN_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiTContactOut[0][0], m_nRiTContactOut[1][0], _T("Fwd"));
// 
// 	m_nRiTContactOut[0][1]	= 4;
// 	m_nRiTContactOut[1][1]	= 4;
// 	m_pGridRiContact.SetItemBkColour(m_nRiTContactOut[0][1], m_nRiTContactOut[1][1], GREEN_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiTContactOut[0][1], m_nRiTContactOut[1][1], _T("Bwd"));
// 
// 	m_nRiTContactFwd[0][0]	= 5;
// 	m_nRiTContactFwd[1][0]	= 3;
// 	m_pGridRiContact.SetItemBkColour(m_nRiTContactFwd[0][0], m_nRiTContactFwd[1][0], YELLOW_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiTContactFwd[0][0], m_nRiTContactFwd[1][0], _T("Lf Fwd"));
// 
// 	m_nRiTContactFwd[0][1]	= 6;
// 	m_nRiTContactFwd[1][1]	= 3;
// 	m_pGridRiContact.SetItemBkColour(m_nRiTContactFwd[0][1], m_nRiTContactFwd[1][1], YELLOW_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiTContactFwd[0][1], m_nRiTContactFwd[1][1], _T("Ri Fwd"));
// 
// 	m_nRiTContactBwd[0][0]	= 5;
// 	m_nRiTContactBwd[1][0]	= 4;
// 	m_pGridRiContact.SetItemBkColour(m_nRiTContactBwd[0][0], m_nRiTContactBwd[1][0], YELLOW_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiTContactBwd[0][0], m_nRiTContactBwd[1][0], _T("Lf Bwd"));
// 
// 	m_nRiTContactBwd[0][1]	= 6;
// 	m_nRiTContactBwd[1][1]	= 4;
// 	m_pGridRiContact.SetItemBkColour(m_nRiTContactBwd[0][1], m_nRiTContactBwd[1][1], YELLOW_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiTContactBwd[0][1], m_nRiTContactBwd[1][1], _T("Ri Bwd"));
// 
// 	// left hifix
// 	m_pGridRiContact.MergeCells(7, 0, 10, 0);
// 	m_pGridRiContact.SetItemBkColour(7, 0, YELLOW_L, BLACK_C);
// 	m_pGridRiContact.SetItemFont(7, 0, &clsFunc.OnLogFont(15));
// 	m_pGridRiContact.SetItemText(7, 0, _T("Right Hifix"));
// 
// 	m_nRiHifixOut[0][0]	= 7;
// 	m_nRiHifixOut[1][0]	= 1;
// 	m_pGridRiContact.SetItemBkColour(m_nRiHifixOut[0][0], m_nRiHifixOut[1][0], GREEN_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiHifixOut[0][0], m_nRiHifixOut[1][0], _T("Fwd"));
// 
// 	m_nRiHifixOut[0][1]	= 7;
// 	m_nRiHifixOut[1][1]	= 2;
// 	m_pGridRiContact.SetItemBkColour(m_nRiHifixOut[0][1], m_nRiHifixOut[1][1], GREEN_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiHifixOut[0][1], m_nRiHifixOut[1][1], _T("Bwd"));
// 
// 	m_nRiHifixFwd[0][0]	= 8;
// 	m_nRiHifixFwd[1][0]	= 1;
// 	m_pGridRiContact.SetItemBkColour(m_nRiHifixFwd[0][0], m_nRiHifixFwd[1][0], YELLOW_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiHifixFwd[0][0], m_nRiHifixFwd[1][0], _T("Lf Top Fwd"));
// 
// 	m_nRiHifixFwd[0][1]	= 9;
// 	m_nRiHifixFwd[1][1]	= 1;
// 	m_pGridRiContact.SetItemBkColour(m_nRiHifixFwd[0][1], m_nRiHifixFwd[1][1], YELLOW_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiHifixFwd[0][1], m_nRiHifixFwd[1][1], _T("Lf Btm Fwd"));
// 
// 	m_nRiHifixFwd[0][2]	= 10;
// 	m_nRiHifixFwd[1][2]	= 1;
// 	m_pGridRiContact.SetItemBkColour(m_nRiHifixFwd[0][2], m_nRiHifixFwd[1][2], YELLOW_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiHifixFwd[0][2], m_nRiHifixFwd[1][2], _T("Ri Fwd"));
// 
// 	m_nRiHifixBwd[0][0]	= 8;
// 	m_nRiHifixBwd[1][0]	= 2;
// 	m_pGridRiContact.SetItemBkColour(m_nRiHifixBwd[0][0], m_nRiHifixBwd[1][0], YELLOW_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiHifixBwd[0][0], m_nRiHifixBwd[1][0], _T("Lf Top Bwd"));
// 
// 	m_nRiHifixBwd[0][1]	= 9;
// 	m_nRiHifixBwd[1][1]	= 2;
// 	m_pGridRiContact.SetItemBkColour(m_nRiHifixBwd[0][1], m_nRiHifixBwd[1][1], YELLOW_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiHifixBwd[0][1], m_nRiHifixBwd[1][1], _T("Lf Btm Bwd"));
// 
// 	m_nRiHifixBwd[0][2]	= 10;
// 	m_nRiHifixBwd[1][2]	= 2;
// 	m_pGridRiContact.SetItemBkColour(m_nRiHifixBwd[0][2], m_nRiHifixBwd[1][2], YELLOW_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiHifixBwd[0][2], m_nRiHifixBwd[1][2], _T("Ri Bwd"));
// 
// 	m_pGridRiContact.SetItemBkColour(7, 3, BLACK_L, YELLOW_C);
// 	m_pGridRiContact.SetItemText(7, 3, _T("-"));
// 
// 	m_pGridRiContact.SetItemBkColour(7, 4, BLACK_L, YELLOW_C);
// 	m_pGridRiContact.SetItemText(7, 4, _T("-"));
// 
// 	m_pGridRiContact.SetItemBkColour(8, 3, BLACK_L, YELLOW_C);
// 	m_pGridRiContact.SetItemText(8, 3, _T("-"));
// 
// 	m_pGridRiContact.SetItemBkColour(8, 4, BLACK_L, YELLOW_C);
// 	m_pGridRiContact.SetItemText(8, 4, _T("-"));
// 
// 	m_pGridRiContact.SetItemBkColour(9, 3, BLACK_L, YELLOW_C);
// 	m_pGridRiContact.SetItemText(9, 3, _T("-"));
// 
// 	m_pGridRiContact.SetItemBkColour(9, 4, BLACK_L, YELLOW_C);
// 	m_pGridRiContact.SetItemText(9, 4, _T("-"));
// 
// 	m_pGridRiContact.SetItemBkColour(10, 3, BLACK_L, YELLOW_C);
// 	m_pGridRiContact.SetItemText(10, 3, _T("-"));
// 
// 	m_pGridRiContact.SetItemBkColour(10, 4, BLACK_L, YELLOW_C);
// 	m_pGridRiContact.SetItemText(10, 4, _T("-"));
// 
// 	// left mp
// 	m_pGridRiContact.MergeCells(11, 0, 13, 0);
// 	m_pGridRiContact.SetItemBkColour(11, 0, YELLOW_L, BLACK_C);
// 	m_pGridRiContact.SetItemFont(11, 0, &clsFunc.OnLogFont(15));
// 	m_pGridRiContact.SetItemText(11, 0, _T("Right Mp"));
// 
// 	m_nRiMpOut[0][0]	= 11;
// 	m_nRiMpOut[1][0]	= 1;
// 	m_pGridRiContact.SetItemBkColour(m_nRiMpOut[0][0], m_nRiMpOut[1][0], GREEN_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiMpOut[0][0], m_nRiMpOut[1][0], _T("Fwd"));
// 
// 	m_nRiMpOut[0][1]	= 11;
// 	m_nRiMpOut[1][1]	= 2;
// 	m_pGridRiContact.SetItemBkColour(m_nRiMpOut[0][1], m_nRiMpOut[1][1], GREEN_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiMpOut[0][1], m_nRiMpOut[1][1], _T("Bwd"));
// 
// 	m_nRiMpFwd[0][0]	= 12;
// 	m_nRiMpFwd[1][0]	= 1;
// 	m_pGridRiContact.SetItemBkColour(m_nRiMpFwd[0][0], m_nRiMpFwd[1][0], YELLOW_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiMpFwd[0][0], m_nRiMpFwd[1][0], _T("Lf Fwd"));
// 
// 	m_nRiMpFwd[0][1]	= 13;
// 	m_nRiMpFwd[1][1]	= 1;
// 	m_pGridRiContact.SetItemBkColour(m_nRiMpFwd[0][1], m_nRiMpFwd[1][1], YELLOW_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiMpFwd[0][1], m_nRiMpFwd[1][1], _T("Ri Fwd"));
// 
// 	m_nRiMpBwd[0][0]	= 12;
// 	m_nRiMpBwd[1][0]	= 2;
// 	m_pGridRiContact.SetItemBkColour(m_nRiMpBwd[0][0], m_nRiMpBwd[1][0], YELLOW_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiMpBwd[0][0], m_nRiMpBwd[1][0], _T("Lf Bwd"));
// 
// 	m_nRiMpBwd[0][1]	= 13;
// 	m_nRiMpBwd[1][1]	= 2;
// 	m_pGridRiContact.SetItemBkColour(m_nRiMpBwd[0][1], m_nRiMpBwd[1][1], YELLOW_D, BLACK_C);
// 	m_pGridRiContact.SetItemText(m_nRiMpBwd[0][1], m_nRiMpBwd[1][1], _T("Ri Bwd"));
// 
// 	m_pGridRiContact.SetItemBkColour(11, 3, BLACK_L, YELLOW_C);
// 	m_pGridRiContact.SetItemText(11, 3, _T("-"));
// 
// 	m_pGridRiContact.SetItemBkColour(11, 4, BLACK_L, YELLOW_C);
// 	m_pGridRiContact.SetItemText(11, 4, _T("-"));
// 
// 	m_pGridRiContact.SetItemBkColour(12, 3, BLACK_L, YELLOW_C);
// 	m_pGridRiContact.SetItemText(12, 3, _T("-"));
// 
// 	m_pGridRiContact.SetItemBkColour(12, 4, BLACK_L, YELLOW_C);
// 	m_pGridRiContact.SetItemText(12, 4, _T("-"));
// 
// 	m_pGridRiContact.SetItemBkColour(13, 3, BLACK_L, YELLOW_C);
// 	m_pGridRiContact.SetItemText(13, 3, _T("-"));
// 
// 	m_pGridRiContact.SetItemBkColour(13, 4, BLACK_L, YELLOW_C);
// 	m_pGridRiContact.SetItemText(13, 4, _T("-"));
// 
// 	m_pGridRiContact.Invalidate(FALSE);
// }

void CDialogManualIO::OnInitGridBuffer()
{
	int   i, j;
	int	  nMaxRow, nMaxCol;
//	int   nRow, nCol;
	CString strTmp;

	nMaxRow = 6; // unloader tray y count
	nMaxCol = 5;

	m_pGridBuf.SetFrameFocusCell(FALSE);
	m_pGridBuf.SetTrackFocusCell(FALSE);
	m_pGridBuf.EnableSelection(FALSE);

	m_pGridBuf.SetGridLineColor(BLACK_C);
	m_pGridBuf.SetGridLines(1);

	m_pGridBuf.SetRowCount(nMaxRow);
	m_pGridBuf.SetColumnCount(nMaxCol);

	m_pGridBuf.SetFixedRowCount(0);
	m_pGridBuf.SetFixedColumnCount(0);
	m_pGridBuf.SetFixedBkColor(RGB(0,0,200));
	m_pGridBuf.SetFixedBkColor(RGB(200,200,255));
	m_pGridBuf.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<nMaxRow; i++)
	{
		m_pGridBuf.SetRowHeight(i, 25);

		for (j=0; j<nMaxCol; j++)
		{
			if (j == 0)
			{
				m_pGridBuf.SetColumnWidth(j, 100);
			}
			else 
			{
				m_pGridBuf.SetColumnWidth(j, 70);
			}

			m_pGridBuf.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridBuf.SetItemState(i, j, GVIS_READONLY);
			m_pGridBuf.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
		}
	}

// 	m_nRetestMvFwdCyl			= st_io_info.o_RetestBuffFwdCyl;
// 	m_nRetestMvBwdCyl			= st_io_info.o_RetestBuffBwdCyl;
// 	m_nRetestMvFwdChk			= st_io_info.i_RetestBuffAlignFwdChk;
// 	m_nRetestMvBwdChk			= st_io_info.i_RetestBuffAlignBwdChk;
// 
// 	m_nRetestAlignCyl			= st_io_info.o_RetestBuffAlignCyl;
// 	m_nRetestAlignFwdChk		= st_io_info.i_RetestBuffAlignFwdChk;
// 	m_nRetestAlignBwdChk		= st_io_info.i_RetestBuffAlignBwdChk;
// 
// 	m_nRejectClampCyl			= st_io_info.o_RejGripperClampCyl;
// 	m_nRejectClampFwdChk		= st_io_info.i_RejGripperFwdChk;
// 	m_nRejectClampBwdChk		= st_io_info.i_RejGripperBwdChk;
// 
// 	m_nRejectUpDnCyl			= st_io_info.o_RejGripperUpDnCyl;
// 	m_nRejectUpChk				= st_io_info.i_RejGripperUpDnFwdChk;
// 	m_nRejectDnChk				= st_io_info.i_RejGripperUpDnBwdChk;
// 
// 	m_nRejectMvFwdCyl			= st_io_info.o_RejPushFwdCyl;
// 	m_nRejectMvBwdCyl			= st_io_info.o_RejPushBwdCyl;
// 	m_nRejectMvFwdChk			= st_io_info.i_RejPushFwdChk;
// 	m_nRejectMvBwdChk			= st_io_info.i_RejPushBwdChk;

	m_pGridBuf.MergeCells(0, 0, 0, 4);
	m_pGridBuf.SetItemBkColour(0, 0, BLUE_D, WHITE_C);
	m_pGridBuf.SetItemFont(0, 0, &clsFunc.OnLogFont(15));
	m_pGridBuf.SetItemText(0, 0, _T("Buffer Tray"));

	// retest move
	m_pGridBuf.SetItemBkColour(1, 0, YELLOW_L, BLACK_C);
	m_pGridBuf.SetItemFont(1, 0, &clsFunc.OnLogFont(15));
	m_pGridBuf.SetItemText(1, 0, _T("Retest Move"));

// 	m_nRetestMvFwdOut[0]	= 1;
// 	m_nRetestMvFwdOut[1]	= 1;
// 	m_pGridBuf.SetItemBkColour(m_nRetestMvFwdOut[0], m_nRetestMvFwdOut[1], GREEN_D, BLACK_C);
// 	m_pGridBuf.SetItemText(m_nRetestMvFwdOut[0], m_nRetestMvFwdOut[1], _T("Fwd"));
// 
// 	m_nRetestMvBwdOut[0]	= 1;
// 	m_nRetestMvBwdOut[1]	= 2;
// 	m_pGridBuf.SetItemBkColour(m_nRetestMvBwdOut[0], m_nRetestMvBwdOut[1], GREEN_D, BLACK_C);
// 	m_pGridBuf.SetItemText(m_nRetestMvBwdOut[0], m_nRetestMvBwdOut[1], _T("Bwd"));
// 
// 	m_nRetestMvFwd[0]		= 1;
// 	m_nRetestMvFwd[1]		= 3;
// 	m_pGridBuf.SetItemBkColour(m_nRetestMvFwd[0], m_nRetestMvFwd[1], YELLOW_D, BLACK_C);
// 	m_pGridBuf.SetItemText(m_nRetestMvFwd[0], m_nRetestMvFwd[1], _T("Fwd Chk"));
// 
// 	m_nRetestMvBwd[0]	= 1;
// 	m_nRetestMvBwd[1]	= 4;
// 	m_pGridBuf.SetItemBkColour(m_nRetestMvBwd[0], m_nRetestMvBwd[1], YELLOW_D, BLACK_C);
// 	m_pGridBuf.SetItemText(m_nRetestMvBwd[0], m_nRetestMvBwd[1], _T("Bwd Chk"));
// 
// 	// Align
// 	m_pGridBuf.SetItemBkColour(2, 0, YELLOW_L, BLACK_C);
// 	m_pGridBuf.SetItemFont(2, 0, &clsFunc.OnLogFont(15));
// 	m_pGridBuf.SetItemText(2, 0, _T("Retest Align"));
// 
// 	m_nRetestAlignOut[0]	= 2;
// 	m_nRetestAlignOut[1]	= 1;
// 	m_pGridBuf.MergeCells(m_nRetestAlignOut[0], m_nRetestAlignOut[1], m_nRetestAlignOut[0], m_nRetestAlignOut[1] + 1);
// 	m_pGridBuf.SetItemBkColour(m_nRetestAlignOut[0], m_nRetestAlignOut[1], GREEN_D, BLACK_C);
// 	m_pGridBuf.SetItemText(m_nRetestAlignOut[0], m_nRetestAlignOut[1], _T("Fwd/Bwd"));
// 
// 	m_nRetestAlignFwd[0]	= 2;
// 	m_nRetestAlignFwd[1]	= 3;
// 	m_pGridBuf.SetItemBkColour(m_nRetestAlignFwd[0], m_nRetestAlignFwd[1], YELLOW_D, BLACK_C);
// 	m_pGridBuf.SetItemText(m_nRetestAlignFwd[0], m_nRetestAlignFwd[1], _T("Fwd Chk"));
// 
// 	m_nRetestAlignBwd[0]	= 2;
// 	m_nRetestAlignBwd[1]	= 4;
// 	m_pGridBuf.SetItemBkColour(m_nRetestAlignBwd[0], m_nRetestAlignBwd[1], YELLOW_D, BLACK_C);
// 	m_pGridBuf.SetItemText(m_nRetestAlignBwd[0], m_nRetestAlignBwd[1], _T("Bwd Chk"));
// 
// 	// reject clamp
// 	m_pGridBuf.SetItemBkColour(3, 0, YELLOW_L, BLACK_C);
// 	m_pGridBuf.SetItemFont(3, 0, &clsFunc.OnLogFont(15));
// 	m_pGridBuf.SetItemText(3, 0, _T("Reject Clamp"));
// 
// 	m_nRejectClampOut[0]	= 3;
// 	m_nRejectClampOut[1]	= 1;
// 	m_pGridBuf.MergeCells(m_nRejectClampOut[0], m_nRejectClampOut[1], m_nRejectClampOut[0], m_nRejectClampOut[1] + 1);
// 	m_pGridBuf.SetItemBkColour(m_nRejectClampOut[0], m_nRejectClampOut[1], GREEN_D, BLACK_C);
// 	m_pGridBuf.SetItemText(m_nRejectClampOut[0], m_nRejectClampOut[1], _T("Clamp/UnClamp"));
// 
// 	m_nRejectClampFwd[0]		= 3;
// 	m_nRejectClampFwd[1]		= 3;
// 	m_pGridBuf.SetItemBkColour(m_nRejectClampFwd[0], m_nRejectClampFwd[1], YELLOW_D, BLACK_C);
// 	m_pGridBuf.SetItemText(m_nRejectClampFwd[0], m_nRejectClampFwd[1], _T("Clamp"));
// 
// 	m_nRejectClampBwd[0]	= 3;
// 	m_nRejectClampBwd[1]	= 4;
// 	m_pGridBuf.SetItemBkColour(m_nRejectClampBwd[0], m_nRejectClampBwd[1], YELLOW_D, BLACK_C);
// 	m_pGridBuf.SetItemText(m_nRejectClampBwd[0], m_nRejectClampBwd[1], _T("UnClamp"));
// 
// 	// Up / Dn
// 	m_pGridBuf.SetItemBkColour(4, 0, YELLOW_L, BLACK_C);
// 	m_pGridBuf.SetItemFont(4, 0, &clsFunc.OnLogFont(15));
// 	m_pGridBuf.SetItemText(4, 0, _T("Reject Up/Dn"));
// 
// 	m_nRejectUpDnOut[0]	= 4;
// 	m_nRejectUpDnOut[1]	= 1;
// 	m_pGridBuf.MergeCells(m_nRejectUpDnOut[0], m_nRejectUpDnOut[1], m_nRejectUpDnOut[0], m_nRejectUpDnOut[1] + 1);
// 	m_pGridBuf.SetItemBkColour(m_nRejectUpDnOut[0], m_nRejectUpDnOut[1], GREEN_D, BLACK_C);
// 	m_pGridBuf.SetItemText(m_nRejectUpDnOut[0], m_nRejectUpDnOut[1], _T("Up/Dn"));
// 
// 	m_nRejectUp[0]	= 4;
// 	m_nRejectUp[1]	= 3;
// 	m_pGridBuf.SetItemBkColour(m_nRejectUp[0], m_nRejectUp[1], YELLOW_D, BLACK_C);
// 	m_pGridBuf.SetItemText(m_nRejectUp[0], m_nRejectUp[1], _T("Up Chk"));
// 
// 	m_nRejectDn[0]	= 4;
// 	m_nRejectDn[1]	= 4;
// 	m_pGridBuf.SetItemBkColour(m_nRejectDn[0], m_nRejectDn[1], YELLOW_D, BLACK_C);
// 	m_pGridBuf.SetItemText(m_nRejectDn[0], m_nRejectDn[1], _T("Dn Chk"));
// 
// 	// reject move
// 	m_pGridBuf.SetItemBkColour(5, 0, YELLOW_L, BLACK_C);
// 	m_pGridBuf.SetItemFont(5, 0, &clsFunc.OnLogFont(15));
// 	m_pGridBuf.SetItemText(5, 0, _T("Reject Move"));
// 
// 	m_nRejectMvFwdOut[0]	= 5;
// 	m_nRejectMvFwdOut[1]	= 1;
// 	m_pGridBuf.SetItemBkColour(m_nRejectMvFwdOut[0], m_nRejectMvFwdOut[1], GREEN_D, BLACK_C);
// 	m_pGridBuf.SetItemText(m_nRejectMvFwdOut[0], m_nRejectMvFwdOut[1], _T("Fwd"));
// 
// 	m_nRejectMvBwdOut[0]	= 5;
// 	m_nRejectMvBwdOut[1]	= 2;
// 	m_pGridBuf.SetItemBkColour(m_nRejectMvBwdOut[0], m_nRejectMvBwdOut[1], GREEN_D, BLACK_C);
// 	m_pGridBuf.SetItemText(m_nRejectMvBwdOut[0], m_nRejectMvBwdOut[1], _T("Bwd"));
// 
// 	m_nRejectMvFwd[0]		= 5;
// 	m_nRejectMvFwd[1]		= 3;
// 	m_pGridBuf.SetItemBkColour(m_nRejectMvFwd[0], m_nRejectMvFwd[1], YELLOW_D, BLACK_C);
// 	m_pGridBuf.SetItemText(m_nRejectMvFwd[0], m_nRejectMvFwd[1], _T("Fwd Chk"));
// 
// 	m_nRejectMvBwd[0]	= 5;
// 	m_nRejectMvBwd[1]	= 4;
// 	m_pGridBuf.SetItemBkColour(m_nRejectMvBwd[0], m_nRejectMvBwd[1], YELLOW_D, BLACK_C);
// 	m_pGridBuf.SetItemText(m_nRejectMvBwd[0], m_nRejectMvBwd[1], _T("Bwd Chk"));

	m_pGridBuf.Invalidate(FALSE);
}

void CDialogManualIO::OnTsiteCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	int nRow, nCol;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	switch (nRow)
	{
		case 0:
			switch (nCol)
			{
				case 0:
					m_nTsiteNum = 0;
					OnDisplayTestSite(m_nTsiteNum);

					m_pGridTsite.SetItemBkColour(0, 0, ORANGE_C, YELLOW_C);
					m_pGridTsite.SetItemBkColour(2, 0, BLACK_L, YELLOW_C);
					m_pGridTsite.SetItemBkColour(4, 0, BLACK_L, YELLOW_C);
					m_pGridTsite.SetItemBkColour(6, 0, BLACK_L, YELLOW_C);
					break;

// 				case 1:
// 					FAS_IO.set_out_bit(m_nInsertCyl[m_nTsiteNum][0], !FAS_IO.get_out_bit(m_nInsertCyl[m_nTsiteNum][0], IO_ON));
// 					break;
// 
// 				case 2:
// 					FAS_IO.set_out_bit(m_nInsertCyl[m_nTsiteNum][0], !FAS_IO.get_out_bit(m_nInsertCyl[m_nTsiteNum][0], IO_ON));
// 					break;
// 
// 				case 3:
// 					FAS_IO.set_out_bit(m_nInsertCyl[m_nTsiteNum][0], !FAS_IO.get_out_bit(m_nInsertCyl[m_nTsiteNum][0], IO_ON));
// 					break;
// 
// 				case 4:
// 					FAS_IO.set_out_bit(m_nInsertCyl[m_nTsiteNum][0], !FAS_IO.get_out_bit(m_nInsertCyl[m_nTsiteNum][0], IO_ON));
// 					break;
// 
// 				case 5:
// 					FAS_IO.set_out_bit(m_nInsertCyl[m_nTsiteNum][1], !FAS_IO.get_out_bit(m_nInsertCyl[m_nTsiteNum][1], IO_ON));
// 					break;
// 
// 				case 6:
// 					FAS_IO.set_out_bit(m_nInsertCyl[m_nTsiteNum][1], !FAS_IO.get_out_bit(m_nInsertCyl[m_nTsiteNum][1], IO_ON));
// 					break;
// 
// 				case 7:
// 					FAS_IO.set_out_bit(m_nInsertCyl[m_nTsiteNum][1], !FAS_IO.get_out_bit(m_nInsertCyl[m_nTsiteNum][1], IO_ON));
// 					break;
// 
// 				case 8:
// 					FAS_IO.set_out_bit(m_nInsertCyl[m_nTsiteNum][1], !FAS_IO.get_out_bit(m_nInsertCyl[m_nTsiteNum][1], IO_ON));
// 					break;
// 
// 				case 9:
// 					FAS_IO.set_out_bit(m_nInsertCyl[m_nTsiteNum][2], !FAS_IO.get_out_bit(m_nInsertCyl[m_nTsiteNum][2], IO_ON));
// 					break;
// 
// 				case 10:
// 					FAS_IO.set_out_bit(m_nInsertCyl[m_nTsiteNum][2], !FAS_IO.get_out_bit(m_nInsertCyl[m_nTsiteNum][2], IO_ON));
// 					break;
// 
// 				case 11:
// 					FAS_IO.set_out_bit(m_nInsertCyl[m_nTsiteNum][2], !FAS_IO.get_out_bit(m_nInsertCyl[m_nTsiteNum][2], IO_ON));
// 					break;
// 
// 				case 12:
// 					FAS_IO.set_out_bit(m_nInsertCyl[m_nTsiteNum][2], !FAS_IO.get_out_bit(m_nInsertCyl[m_nTsiteNum][2], IO_ON));
// 					break;
			}
			break;

		case 1:
			switch (nCol)
			{
				case 0:
					m_nTsiteNum = 1;
					OnDisplayTestSite(m_nTsiteNum);

					m_pGridTsite.SetItemBkColour(0, 0, ORANGE_C, YELLOW_C);
					m_pGridTsite.SetItemBkColour(2, 0, BLACK_L, YELLOW_C);
					m_pGridTsite.SetItemBkColour(4, 0, BLACK_L, YELLOW_C);
					m_pGridTsite.SetItemBkColour(6, 0, BLACK_L, YELLOW_C);
					break;
			}
			break;

		case 2:
			switch (nCol)
			{
				case 0:
					m_nTsiteNum = 1;
					OnDisplayTestSite(m_nTsiteNum);

					m_pGridTsite.SetItemBkColour(0, 0, BLACK_L, YELLOW_C);
					m_pGridTsite.SetItemBkColour(2, 0, ORANGE_C, YELLOW_C);
					m_pGridTsite.SetItemBkColour(4, 0, BLACK_L, YELLOW_C);
					m_pGridTsite.SetItemBkColour(6, 0, BLACK_L, YELLOW_C);
					break;

// 				case 1:
// 					FAS_IO.set_out_bit(m_nPogoCyl[m_nTsiteNum][0], !FAS_IO.get_out_bit(m_nPogoCyl[m_nTsiteNum][0], IO_ON));
// 					break;
// 
// 				case 2:
// 					FAS_IO.set_out_bit(m_nPogoCyl[m_nTsiteNum][0], !FAS_IO.get_out_bit(m_nPogoCyl[m_nTsiteNum][0], IO_ON));
// 					break;
// 
// 				case 3:
// 					FAS_IO.set_out_bit(m_nPogoCyl[m_nTsiteNum][1], !FAS_IO.get_out_bit(m_nPogoCyl[m_nTsiteNum][1], IO_ON));
// 					break;
// 
// 				case 4:
// 					FAS_IO.set_out_bit(m_nPogoCyl[m_nTsiteNum][1], !FAS_IO.get_out_bit(m_nPogoCyl[m_nTsiteNum][1], IO_ON));
// 					break;
// 
// 				case 5:
// 					FAS_IO.set_out_bit(m_nPogoCyl[m_nTsiteNum][2], !FAS_IO.get_out_bit(m_nPogoCyl[m_nTsiteNum][2], IO_ON));
// 					break;
// 
// 				case 6:
// 					FAS_IO.set_out_bit(m_nPogoCyl[m_nTsiteNum][2], !FAS_IO.get_out_bit(m_nPogoCyl[m_nTsiteNum][2], IO_ON));
// 					break;
// 
// 				case 7:
// 					FAS_IO.set_out_bit(m_nPogoCyl[m_nTsiteNum][3], !FAS_IO.get_out_bit(m_nPogoCyl[m_nTsiteNum][3], IO_ON));
// 					break;
// 
// 				case 8:
// 					FAS_IO.set_out_bit(m_nPogoCyl[m_nTsiteNum][3], !FAS_IO.get_out_bit(m_nPogoCyl[m_nTsiteNum][3], IO_ON));
// 					break;
// 
// 				case 9:
// 					FAS_IO.set_out_bit(m_nPogoCyl[m_nTsiteNum][4], !FAS_IO.get_out_bit(m_nPogoCyl[m_nTsiteNum][4], IO_ON));
// 					break;
// 
// 				case 10:
// 					FAS_IO.set_out_bit(m_nPogoCyl[m_nTsiteNum][4], !FAS_IO.get_out_bit(m_nPogoCyl[m_nTsiteNum][4], IO_ON));
// 					break;
// 
// 				case 11:
// 					FAS_IO.set_out_bit(m_nPogoCyl[m_nTsiteNum][5], !FAS_IO.get_out_bit(m_nPogoCyl[m_nTsiteNum][5], IO_ON));
// 					break;
// 
// 				case 12:
// 					FAS_IO.set_out_bit(m_nPogoCyl[m_nTsiteNum][5], !FAS_IO.get_out_bit(m_nPogoCyl[m_nTsiteNum][5], IO_ON));
// 					break;
			}
			break;

		case 3:
			switch (nCol)
			{
				case 0:
					m_nTsiteNum = 1;
					OnDisplayTestSite(m_nTsiteNum);

					m_pGridTsite.SetItemBkColour(0, 0, BLACK_L, YELLOW_C);
					m_pGridTsite.SetItemBkColour(2, 0, ORANGE_C, YELLOW_C);
					m_pGridTsite.SetItemBkColour(4, 0, BLACK_L, YELLOW_C);
					m_pGridTsite.SetItemBkColour(6, 0, BLACK_L, YELLOW_C);
					break;
			}
			break;

		case 4:
			switch (nCol)
			{
				case 0:
					m_nTsiteNum = 2;
					OnDisplayTestSite(m_nTsiteNum);

					m_pGridTsite.SetItemBkColour(0, 0, BLACK_L, YELLOW_C);
					m_pGridTsite.SetItemBkColour(2, 0, BLACK_L, YELLOW_C);
					m_pGridTsite.SetItemBkColour(4, 0, ORANGE_C, YELLOW_C);
					m_pGridTsite.SetItemBkColour(6, 0, BLACK_L, YELLOW_C);
					break;

// 				case 1:
// 					FAS_IO.set_out_bit(m_nInsertCyl[m_nTsiteNum][3], !FAS_IO.get_out_bit(m_nInsertCyl[m_nTsiteNum][3], IO_ON));
// 					break;
// 
// 				case 2:
// 					FAS_IO.set_out_bit(m_nInsertCyl[m_nTsiteNum][3], !FAS_IO.get_out_bit(m_nInsertCyl[m_nTsiteNum][3], IO_ON));
// 					break;
// 
// 				case 3:
// 					FAS_IO.set_out_bit(m_nInsertCyl[m_nTsiteNum][3], !FAS_IO.get_out_bit(m_nInsertCyl[m_nTsiteNum][3], IO_ON));
// 					break;
// 
// 				case 4:
// 					FAS_IO.set_out_bit(m_nInsertCyl[m_nTsiteNum][3], !FAS_IO.get_out_bit(m_nInsertCyl[m_nTsiteNum][3], IO_ON));
// 					break;
// 
// 				case 5:
// 					FAS_IO.set_out_bit(m_nInsertCyl[m_nTsiteNum][4], !FAS_IO.get_out_bit(m_nInsertCyl[m_nTsiteNum][4], IO_ON));
// 					break;
// 
// 				case 6:
// 					FAS_IO.set_out_bit(m_nInsertCyl[m_nTsiteNum][4], !FAS_IO.get_out_bit(m_nInsertCyl[m_nTsiteNum][4], IO_ON));
// 					break;
// 
// 				case 7:
// 					FAS_IO.set_out_bit(m_nInsertCyl[m_nTsiteNum][4], !FAS_IO.get_out_bit(m_nInsertCyl[m_nTsiteNum][4], IO_ON));
// 					break;
// 
// 				case 8:
// 					FAS_IO.set_out_bit(m_nInsertCyl[m_nTsiteNum][4], !FAS_IO.get_out_bit(m_nInsertCyl[m_nTsiteNum][4], IO_ON));
// 					break;
// 
// 				case 9:
// 					FAS_IO.set_out_bit(m_nInsertCyl[m_nTsiteNum][5], !FAS_IO.get_out_bit(m_nInsertCyl[m_nTsiteNum][5], IO_ON));
// 					break;
// 
// 				case 10:
// 					FAS_IO.set_out_bit(m_nInsertCyl[m_nTsiteNum][5], !FAS_IO.get_out_bit(m_nInsertCyl[m_nTsiteNum][5], IO_ON));
// 					break;
// 
// 				case 11:
// 					FAS_IO.set_out_bit(m_nInsertCyl[m_nTsiteNum][5], !FAS_IO.get_out_bit(m_nInsertCyl[m_nTsiteNum][5], IO_ON));
// 					break;
// 
// 				case 12:
// 					FAS_IO.set_out_bit(m_nInsertCyl[m_nTsiteNum][5], !FAS_IO.get_out_bit(m_nInsertCyl[m_nTsiteNum][5], IO_ON));
// 					break;
			}
			break;

		case 5:
			switch (nCol)
			{
				case 0:
					m_nTsiteNum = 2;
					OnDisplayTestSite(m_nTsiteNum);

					m_pGridTsite.SetItemBkColour(0, 0, BLACK_L, YELLOW_C);
					m_pGridTsite.SetItemBkColour(2, 0, BLACK_L, YELLOW_C);
					m_pGridTsite.SetItemBkColour(4, 0, ORANGE_C, YELLOW_C);
					m_pGridTsite.SetItemBkColour(6, 0, BLACK_L, YELLOW_C);
					break;
			}
			break;

		case 6:
			switch (nCol)
			{
				case 0:
					m_nTsiteNum = 3;
					OnDisplayTestSite(m_nTsiteNum);

					m_pGridTsite.SetItemBkColour(0, 0, BLACK_L, YELLOW_C);
					m_pGridTsite.SetItemBkColour(2, 0, BLACK_L, YELLOW_C);
					m_pGridTsite.SetItemBkColour(4, 0, BLACK_L, YELLOW_C);
					m_pGridTsite.SetItemBkColour(6, 0, ORANGE_C, YELLOW_C);
					break;

// 				case 1:
// 					FAS_IO.set_out_bit(m_nPogoCyl[m_nTsiteNum][6], !FAS_IO.get_out_bit(m_nPogoCyl[m_nTsiteNum][6], IO_ON));
// 					break;
// 
// 				case 2:
// 					FAS_IO.set_out_bit(m_nPogoCyl[m_nTsiteNum][6], !FAS_IO.get_out_bit(m_nPogoCyl[m_nTsiteNum][6], IO_ON));
// 					break;
// 
// 				case 3:
// 					FAS_IO.set_out_bit(m_nPogoCyl[m_nTsiteNum][7], !FAS_IO.get_out_bit(m_nPogoCyl[m_nTsiteNum][7], IO_ON));
// 					break;
// 
// 				case 4:
// 					FAS_IO.set_out_bit(m_nPogoCyl[m_nTsiteNum][7], !FAS_IO.get_out_bit(m_nPogoCyl[m_nTsiteNum][7], IO_ON));
// 					break;
// 
// 				case 5:
// 					FAS_IO.set_out_bit(m_nPogoCyl[m_nTsiteNum][8], !FAS_IO.get_out_bit(m_nPogoCyl[m_nTsiteNum][8], IO_ON));
// 					break;
// 
// 				case 6:
// 					FAS_IO.set_out_bit(m_nPogoCyl[m_nTsiteNum][8], !FAS_IO.get_out_bit(m_nPogoCyl[m_nTsiteNum][8], IO_ON));
// 					break;
// 
// 				case 7:
// 					FAS_IO.set_out_bit(m_nPogoCyl[m_nTsiteNum][9], !FAS_IO.get_out_bit(m_nPogoCyl[m_nTsiteNum][9], IO_ON));
// 					break;
// 
// 				case 8:
// 					FAS_IO.set_out_bit(m_nPogoCyl[m_nTsiteNum][9], !FAS_IO.get_out_bit(m_nPogoCyl[m_nTsiteNum][9], IO_ON));
// 					break;
// 
// 				case 9:
// 					FAS_IO.set_out_bit(m_nPogoCyl[m_nTsiteNum][10], !FAS_IO.get_out_bit(m_nPogoCyl[m_nTsiteNum][10], IO_ON));
// 					break;
// 
// 				case 10:
// 					FAS_IO.set_out_bit(m_nPogoCyl[m_nTsiteNum][10], !FAS_IO.get_out_bit(m_nPogoCyl[m_nTsiteNum][10], IO_ON));
// 					break;
// 
// 				case 11:
// 					FAS_IO.set_out_bit(m_nPogoCyl[m_nTsiteNum][11], !FAS_IO.get_out_bit(m_nPogoCyl[m_nTsiteNum][11], IO_ON));
// 					break;
// 
// 				case 12:
// 					FAS_IO.set_out_bit(m_nPogoCyl[m_nTsiteNum][11], !FAS_IO.get_out_bit(m_nPogoCyl[m_nTsiteNum][11], IO_ON));
// 					break;
			}
			break;

		case 7:
			switch (nCol)
			{
				case 0:
					m_nTsiteNum = 3;
					OnDisplayTestSite(m_nTsiteNum);

					m_pGridTsite.SetItemBkColour(0, 0, BLACK_L, YELLOW_C);
					m_pGridTsite.SetItemBkColour(2, 0, BLACK_L, YELLOW_C);
					m_pGridTsite.SetItemBkColour(4, 0, BLACK_L, YELLOW_C);
					m_pGridTsite.SetItemBkColour(6, 0, ORANGE_C, YELLOW_C);
					break;
			}
			break;
	}

	m_pGridTsite.Invalidate(FALSE);
}

void CDialogManualIO::OnTRbtCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	int nRow, nCol;
	CString strTmp;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

//	switch (nRow)
	{
// 		case 2:
// 			switch (nCol)
// 			{
// // 				case 2:
// // 					FAS_IO.set_out_bit(m_nTsiteCyl[0], !FAS_IO.get_out_bit(m_nTsiteCyl[0], IO_ON));
// // 					break;
// // 
// // 				case 4:
// // 					FAS_IO.set_out_bit(m_nTsiteCyl[1], !FAS_IO.get_out_bit(m_nTsiteCyl[1], IO_ON));
// // 					break;
// // 
// // 				case 6:
// // 					FAS_IO.set_out_bit(m_nTsiteCyl[2], !FAS_IO.get_out_bit(m_nTsiteCyl[2], IO_ON));
// // 					break;
// // 
// // 				case 8:
// // 					FAS_IO.set_out_bit(m_nTsiteCyl[3], !FAS_IO.get_out_bit(m_nTsiteCyl[3], IO_ON));
// // 					break;
// 			}
// 			break;
// 
// 		case 3:
// 			switch (nCol)
// 			{
// // 				case 2:
// // 					FAS_IO.set_out_bit(m_nTsiteCyl[0], !FAS_IO.get_out_bit(m_nTsiteCyl[0], IO_ON));
// // 					break;
// // 
// // 				case 4:
// // 					FAS_IO.set_out_bit(m_nTsiteCyl[1], !FAS_IO.get_out_bit(m_nTsiteCyl[1], IO_ON));
// // 					break;
// // 
// // 				case 6:
// // 					FAS_IO.set_out_bit(m_nTsiteCyl[2], !FAS_IO.get_out_bit(m_nTsiteCyl[2], IO_ON));
// // 					break;
// // 
// // 				case 8:
// // 					FAS_IO.set_out_bit(m_nTsiteCyl[3], !FAS_IO.get_out_bit(m_nTsiteCyl[3], IO_ON));
// // 					break;
// 			}
// 			break;
// 
// 		case 4:
// 			switch (nCol)
// 			{
// // 				case 2:
// // 					FAS_IO.set_out_bit(m_nTsiteVac[0], !FAS_IO.get_out_bit(m_nTsiteVac[0], IO_ON));
// // 					break;
// // 
// // 				case 4:
// // 					FAS_IO.set_out_bit(m_nTsiteVac[1], !FAS_IO.get_out_bit(m_nTsiteVac[1], IO_ON));
// // 					break;
// // 
// // 				case 6:
// // 					FAS_IO.set_out_bit(m_nTsiteVac[2], !FAS_IO.get_out_bit(m_nTsiteVac[2], IO_ON));
// // 					break;
// // 
// // 				case 8:
// // 					FAS_IO.set_out_bit(m_nTsiteVac[3], !FAS_IO.get_out_bit(m_nTsiteVac[3], IO_ON));
// // 					break;
// 			}
// 			break;
// 
// 		case 5:
// 			switch (nCol)
// 			{
// // 				case 2:
// // 					FAS_IO.set_out_bit(m_nTsiteEjc[0], !FAS_IO.get_out_bit(m_nTsiteEjc[0], IO_ON));
// // 					break;
// // 
// // 				case 4:
// // 					FAS_IO.set_out_bit(m_nTsiteEjc[1], !FAS_IO.get_out_bit(m_nTsiteEjc[1], IO_ON));
// // 					break;
// // 
// // 				case 6:
// // 					FAS_IO.set_out_bit(m_nTsiteEjc[2], !FAS_IO.get_out_bit(m_nTsiteEjc[2], IO_ON));
// // 					break;
// // 
// // 				case 8:
// // 					FAS_IO.set_out_bit(m_nTsiteEjc[3], !FAS_IO.get_out_bit(m_nTsiteEjc[3], IO_ON));
// // 					break;
// 			}
// 			break;
	}
}

void CDialogManualIO::OnLdRbtCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	int nRow, nCol;
	CString strTmp;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	switch (nRow)
	{
		case 2:
	//		switch (nCol)
			{
// 				case 2:
// 					FAS_IO.set_out_bit(m_nLdCyl[0], !FAS_IO.get_out_bit(m_nLdCyl[0], IO_ON));
// 					break;
// 
// 				case 4:
// 					FAS_IO.set_out_bit(m_nLdCyl[1], !FAS_IO.get_out_bit(m_nLdCyl[1], IO_ON));
// 					break;
			}
			break;

		case 3:
//			switch (nCol)
			{
// 				case 2:
// 					FAS_IO.set_out_bit(m_nLdCyl[0], !FAS_IO.get_out_bit(m_nLdCyl[0], IO_ON));
// 					break;
// 
// 				case 4:
// 					FAS_IO.set_out_bit(m_nLdCyl[1], !FAS_IO.get_out_bit(m_nLdCyl[1], IO_ON));
// 					break;
			}
			break;

		case 4:
//			switch (nCol)
 			{
// 				case 2:
// 					FAS_IO.set_out_bit(m_nLdVac[0], !FAS_IO.get_out_bit(m_nLdVac[0], IO_ON));
// 					break;
// 
// 				case 4:
// 					FAS_IO.set_out_bit(m_nLdVac[1], !FAS_IO.get_out_bit(m_nLdVac[1], IO_ON));
// 					break;
			}
			break;

		case 5:
//			switch (nCol)
			{
// 				case 2:
// 					FAS_IO.set_out_bit(m_nLdEjc[0], !FAS_IO.get_out_bit(m_nLdEjc[0], IO_ON));
// 					break;
// 
// 				case 4:
// 					FAS_IO.set_out_bit(m_nLdEjc[1], !FAS_IO.get_out_bit(m_nLdEjc[1], IO_ON));
// 					break;
			}
			break;
	}
}

void CDialogManualIO::OnUldRbtCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	int nRow, nCol;
	CString strTmp;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	switch (nRow)
	{
		case 2:
//			switch (nCol)
//			{
// 				case 2:
// 					FAS_IO.set_out_bit(m_nUnLdCyl[0], !FAS_IO.get_out_bit(m_nUnLdCyl[0], IO_ON));
// 					break;
// 
// 				case 4:
// 					FAS_IO.set_out_bit(m_nUnLdCyl[1], !FAS_IO.get_out_bit(m_nUnLdCyl[1], IO_ON));
// 					break;
//			}
			break;

		case 3:
//			switch (nCol)
//			{
// 				case 2:
// 					FAS_IO.set_out_bit(m_nUnLdCyl[0], !FAS_IO.get_out_bit(m_nUnLdCyl[0], IO_ON));
// 					break;
// 
// 				case 4:
// 					FAS_IO.set_out_bit(m_nUnLdCyl[1], !FAS_IO.get_out_bit(m_nUnLdCyl[1], IO_ON));
// 					break;
//			}
			break;

		case 4:
//			switch (nCol)
//			{
// 				case 2:
// 					FAS_IO.set_out_bit(m_nUnLdVac[0], !FAS_IO.get_out_bit(m_nUnLdVac[0], IO_ON));
// 					break;
// 
// 				case 4:
// 					FAS_IO.set_out_bit(m_nUnLdVac[1], !FAS_IO.get_out_bit(m_nUnLdVac[1], IO_ON));
// 					break;
//			}
			break;

		case 5:
//			switch (nCol)
//			{
// 				case 2:
// 					FAS_IO.set_out_bit(m_nUnLdEjc[0], !FAS_IO.get_out_bit(m_nUnLdEjc[0], IO_ON));
// 					break;
// 
// 				case 4:
// 					FAS_IO.set_out_bit(m_nUnLdEjc[1], !FAS_IO.get_out_bit(m_nUnLdEjc[1], IO_ON));
// 					break;
//			}
			break;
	}
}

void CDialogManualIO::OnInitButton()
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

	m_btnDoor.SetBitmaps(IDC_BTN_REFRESH, IDB_BITMAP_DOOR_OPEN, WINDOW_DN, IDB_BITMAP_DOOR_OPEN, WINDOW_UP);
	m_btnDoor.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnDoor.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnDoor.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnDoor.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnDoor.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnDoor.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnDoor.SetFont(clsFunc.m_pFont[3]);
	m_btnDoor.SetTooltipText(_T("Door Open"));

	m_btnLeftMpClamp.SetBitmaps(IDC_BTN_LEFT_CLAMP, IDB_BITMAP_GEAR_DN, WINDOW_DN, IDB_BITMAP_GEAR_UP, WINDOW_UP);
	m_btnLeftMpClamp.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnLeftMpClamp.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnLeftMpClamp.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnLeftMpClamp.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnLeftMpClamp.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnLeftMpClamp.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnLeftMpClamp.SetFont(clsFunc.m_pFont[3]);
	m_btnLeftMpClamp.SetTooltipText(_T("Left Clamp"));

	m_btnLeftMpUnClamp.SetBitmaps(IDC_BTN_LEFT_UNCLAMP, IDB_BITMAP_GEAR_DN, WINDOW_DN, IDB_BITMAP_GEAR_UP, WINDOW_UP);
	m_btnLeftMpUnClamp.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnLeftMpUnClamp.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnLeftMpUnClamp.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnLeftMpUnClamp.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnLeftMpUnClamp.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnLeftMpUnClamp.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnLeftMpUnClamp.SetFont(clsFunc.m_pFont[3]);
	m_btnLeftMpUnClamp.SetTooltipText(_T("Left UnClamp"));

	m_btnRightMpClamp.SetBitmaps(IDC_BTN_RIGHT_CLAMP, IDB_BITMAP_GEAR_DN, WINDOW_DN, IDB_BITMAP_GEAR_UP, WINDOW_UP);
	m_btnRightMpClamp.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnRightMpClamp.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnRightMpClamp.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnRightMpClamp.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnRightMpClamp.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnRightMpClamp.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnRightMpClamp.SetFont(clsFunc.m_pFont[3]);
	m_btnRightMpClamp.SetTooltipText(_T("Right Clamp"));

	m_btnRightMpUnClamp.SetBitmaps(IDC_BTN_RIGHT_UNCLAMP, IDB_BITMAP_GEAR_DN, WINDOW_DN, IDB_BITMAP_GEAR_UP, WINDOW_UP);
	m_btnRightMpUnClamp.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnRightMpUnClamp.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnRightMpUnClamp.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnRightMpUnClamp.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnRightMpUnClamp.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnRightMpUnClamp.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnRightMpUnClamp.SetFont(clsFunc.m_pFont[3]);
	m_btnRightMpUnClamp.SetTooltipText(_T("Right UnClamp"));
}

void CDialogManualIO::OnBnClickedBtnExit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	CDialog::OnOK();
	if (st_handler_info.cWndMotor != NULL)
	{
		st_handler_info.cWndMotor->SendMessage(WM_WORK_COMMAND, MOTOR_IO_DELETE_REQ, 0);
	}
}


void CDialogManualIO::OnDisplayTestSite(int nNum)
{
//	int i;
	CString strTemp;

// 	if (nNum == 0)
// 	{
// 		m_nInsertCyl[nNum][0]		= st_io_info.o_LfTsiteBtmSockIns1Cyl;
// 		m_nInsertFwdChk[nNum][0]	= st_io_info.i_LfTsiteBtmSockIns1FwdChk;
// 		m_nInsertBwdChk[nNum][0]	= st_io_info.i_LfTsiteBtmSockIns1BwdChk;
// 
// 		m_nInsertCyl[nNum][1]		= st_io_info.o_LfTsiteBtmSockIns2Cyl;
// 		m_nInsertFwdChk[nNum][1]	= st_io_info.i_LfTsiteBtmSockIns2FwdChk;
// 		m_nInsertBwdChk[nNum][1]	= st_io_info.i_LfTsiteBtmSockIns2BwdChk;
// 
// 		m_nInsertCyl[nNum][2]		= st_io_info.o_LfTsiteBtmSockIns3Cyl;
// 		m_nInsertFwdChk[nNum][2]	= st_io_info.i_LfTsiteBtmSockIns3FwdChk;
// 		m_nInsertBwdChk[nNum][2]	= st_io_info.i_LfTsiteBtmSockIns3BwdChk;
// 
// 		m_nInsertCyl[nNum][3]		= st_io_info.o_LfTsiteBtmSockIns4Cyl;
// 		m_nInsertFwdChk[nNum][3]	= st_io_info.i_LfTsiteBtmSockIns4FwdChk;
// 		m_nInsertBwdChk[nNum][3]	= st_io_info.i_LfTsiteBtmSockIns4BwdChk;
// 
// 		m_nInsertCyl[nNum][4]		= st_io_info.o_LfTsiteBtmSockIns5Cyl;
// 		m_nInsertFwdChk[nNum][4]	= st_io_info.i_LfTsiteBtmSockIns5FwdChk;
// 		m_nInsertBwdChk[nNum][4]	= st_io_info.i_LfTsiteBtmSockIns5BwdChk;
// 
// 		m_nInsertCyl[nNum][5]		= st_io_info.o_LfTsiteBtmSockIns6Cyl;
// 		m_nInsertFwdChk[nNum][5]	= st_io_info.i_LfTsiteBtmSockIns6FwdChk;
// 		m_nInsertBwdChk[nNum][5]	= st_io_info.i_LfTsiteBtmSockIns6BwdChk;
// 
// 		m_nPogoCyl[nNum][0]			= st_io_info.o_LfTSiteBtmPogoCyl[0];
// 		m_nPogoOnOffChk[nNum][0]	= st_io_info.i_LfTSiteBtmPogoChk[0];
// 
// 		m_nPogoCyl[nNum][1]			= st_io_info.o_LfTSiteBtmPogoCyl[1];
// 		m_nPogoOnOffChk[nNum][1]	= st_io_info.i_LfTSiteBtmPogoChk[1];
// 
// 		m_nPogoCyl[nNum][2]			= st_io_info.o_LfTSiteBtmPogoCyl[2];
// 		m_nPogoOnOffChk[nNum][2]	= st_io_info.i_LfTSiteBtmPogoChk[2];
// 
// 		m_nPogoCyl[nNum][3]			= st_io_info.o_LfTSiteBtmPogoCyl[3];
// 		m_nPogoOnOffChk[nNum][3]	= st_io_info.i_LfTSiteBtmPogoChk[3];
// 
// 		m_nPogoCyl[nNum][4]			= st_io_info.o_LfTSiteBtmPogoCyl[4];
// 		m_nPogoOnOffChk[nNum][4]	= st_io_info.i_LfTSiteBtmPogoChk[4];
// 
// 		m_nPogoCyl[nNum][5]			= st_io_info.o_LfTSiteBtmPogoCyl[5];
// 		m_nPogoOnOffChk[nNum][5]	= st_io_info.i_LfTSiteBtmPogoChk[5];
// 
// 		m_nPogoCyl[nNum][6]			= st_io_info.o_LfTSiteBtmPogoCyl[6];
// 		m_nPogoOnOffChk[nNum][6]	= st_io_info.i_LfTSiteBtmPogoChk[6];
// 
// 		m_nPogoCyl[nNum][7]			= st_io_info.o_LfTSiteBtmPogoCyl[7];
// 		m_nPogoOnOffChk[nNum][7]	= st_io_info.i_LfTSiteBtmPogoChk[7];
// 
// 		m_nPogoCyl[nNum][8]			= st_io_info.o_LfTSiteBtmPogoCyl[8];
// 		m_nPogoOnOffChk[nNum][8]	= st_io_info.i_LfTSiteBtmPogoChk[8];
// 
// 		m_nPogoCyl[nNum][9]			= st_io_info.o_LfTSiteBtmPogoCyl[9];
// 		m_nPogoOnOffChk[nNum][9]	= st_io_info.i_LfTSiteBtmPogoChk[9];
// 
// 		m_nPogoCyl[nNum][10]		= st_io_info.o_LfTSiteBtmPogoCyl[10];
// 		m_nPogoOnOffChk[nNum][10]	= st_io_info.i_LfTSiteBtmPogoChk[10];
// 
// 		m_nPogoCyl[nNum][11]		= st_io_info.o_LfTSiteBtmPogoCyl[11];
// 		m_nPogoOnOffChk[nNum][11]	= st_io_info.i_LfTSiteBtmPogoChk[11];

// 		for (i=0; i<3; i++)
// 		{
// 			strTemp.Format(_T("Insert_%02d ~ %02d"), (i * 2) + 1, (i * 2) + 2);
// 			m_pGridTsite.SetItemText(m_nTsiteInsOut[0][i], m_nTsiteInsOut[1][i], strTemp);
// 
// 			strTemp.Format(_T("IInsert_%02d ~ %02d"), (i * 2) + 7, (i * 2) + 8);
// 			m_pGridTsite.SetItemText(m_nTsiteInsOut[0][i + 3], m_nTsiteInsOut[1][i + 3], strTemp);
// 		}
// 
// 		for (i=0; i<6; i++)
// 		{
// 			strTemp.Format(_T("Pogo_%02d"),  i + 1);
// 			m_pGridTsite.SetItemText(m_nTsitePogoOut[0][i], m_nTsitePogoOut[1][i], strTemp);
// 
// 			strTemp.Format(_T("Pogo_%02d"), i + 7);
// 			m_pGridTsite.SetItemText(m_nTsitePogoOut[0][i + 6], m_nTsitePogoOut[1][i + 6], strTemp);
// 		}
// 	}
// 	else if (nNum == 1)
// 	{
// 		m_nInsertCyl[nNum][0]		= st_io_info.o_LfTsiteTopSockIns1Cyl;
// 		m_nInsertFwdChk[nNum][0]	= st_io_info.i_LfTsiteTopSockIns1FwdChk;
// 		m_nInsertBwdChk[nNum][0]	= st_io_info.i_LfTsiteTopSockIns1BwdChk;
// 														 
// 		m_nInsertCyl[nNum][1]		= st_io_info.o_LfTsiteTopSockIns2Cyl;
// 		m_nInsertFwdChk[nNum][1]	= st_io_info.i_LfTsiteTopSockIns2FwdChk;
// 		m_nInsertBwdChk[nNum][1]	= st_io_info.i_LfTsiteTopSockIns2BwdChk;
// 
// 		m_nInsertCyl[nNum][2]		= st_io_info.o_LfTsiteTopSockIns3Cyl;
// 		m_nInsertFwdChk[nNum][2]	= st_io_info.i_LfTsiteTopSockIns3FwdChk;
// 		m_nInsertBwdChk[nNum][2]	= st_io_info.i_LfTsiteTopSockIns3BwdChk;
// 
// 		m_nInsertCyl[nNum][3]		= st_io_info.o_LfTsiteTopSockIns4Cyl;
// 		m_nInsertFwdChk[nNum][3]	= st_io_info.i_LfTsiteTopSockIns4FwdChk;
// 		m_nInsertBwdChk[nNum][3]	= st_io_info.i_LfTsiteTopSockIns4BwdChk;
// 
// 		m_nInsertCyl[nNum][4]		= st_io_info.o_LfTsiteTopSockIns5Cyl;
// 		m_nInsertFwdChk[nNum][4]	= st_io_info.i_LfTsiteTopSockIns5FwdChk;
// 		m_nInsertBwdChk[nNum][4]	= st_io_info.i_LfTsiteTopSockIns5BwdChk;
// 
// 		m_nInsertCyl[nNum][5]		= st_io_info.o_LfTsiteTopSockIns6Cyl;
// 		m_nInsertFwdChk[nNum][5]	= st_io_info.i_LfTsiteTopSockIns6FwdChk;
// 		m_nInsertBwdChk[nNum][5]	= st_io_info.i_LfTsiteTopSockIns6BwdChk;
// 
// 		m_nPogoCyl[nNum][0]			= st_io_info.o_LfTSiteTopPogoCyl[0];
// 		m_nPogoOnOffChk[nNum][0]	= st_io_info.i_LfTSiteTopPogoChk[0];
// 
// 		m_nPogoCyl[nNum][1]			= st_io_info.o_LfTSiteTopPogoCyl[1];
// 		m_nPogoOnOffChk[nNum][1]	= st_io_info.i_LfTSiteTopPogoChk[1];
// 
// 		m_nPogoCyl[nNum][2]			= st_io_info.o_LfTSiteTopPogoCyl[2];
// 		m_nPogoOnOffChk[nNum][2]	= st_io_info.i_LfTSiteTopPogoChk[2];
// 
// 		m_nPogoCyl[nNum][3]			= st_io_info.o_LfTSiteTopPogoCyl[3];
// 		m_nPogoOnOffChk[nNum][3]	= st_io_info.i_LfTSiteTopPogoChk[3];
// 
// 		m_nPogoCyl[nNum][4]			= st_io_info.o_LfTSiteTopPogoCyl[4];
// 		m_nPogoOnOffChk[nNum][4]	= st_io_info.i_LfTSiteTopPogoChk[4];
// 
// 		m_nPogoCyl[nNum][5]			= st_io_info.o_LfTSiteTopPogoCyl[5];
// 		m_nPogoOnOffChk[nNum][5]	= st_io_info.i_LfTSiteTopPogoChk[5];
// 
// 		m_nPogoCyl[nNum][6]			= st_io_info.o_LfTSiteTopPogoCyl[6];
// 		m_nPogoOnOffChk[nNum][6]	= st_io_info.i_LfTSiteTopPogoChk[6];
// 
// 		m_nPogoCyl[nNum][7]			= st_io_info.o_LfTSiteTopPogoCyl[7];
// 		m_nPogoOnOffChk[nNum][7]	= st_io_info.i_LfTSiteTopPogoChk[7];
// 
// 		m_nPogoCyl[nNum][8]			= st_io_info.o_LfTSiteTopPogoCyl[8];
// 		m_nPogoOnOffChk[nNum][8]	= st_io_info.i_LfTSiteTopPogoChk[8];
// 
// 		m_nPogoCyl[nNum][9]			= st_io_info.o_LfTSiteTopPogoCyl[9];
// 		m_nPogoOnOffChk[nNum][9]	= st_io_info.i_LfTSiteTopPogoChk[9];
// 
// 		m_nPogoCyl[nNum][10]		= st_io_info.o_LfTSiteTopPogoCyl[10];
// 		m_nPogoOnOffChk[nNum][10]	= st_io_info.i_LfTSiteTopPogoChk[10];
// 
// 		m_nPogoCyl[nNum][11]		= st_io_info.o_LfTSiteTopPogoCyl[11];
// 		m_nPogoOnOffChk[nNum][11]	= st_io_info.i_LfTSiteTopPogoChk[11];

// 		for (i=0; i<3; i++)
// 		{
// 			strTemp.Format(_T("Insert_%02d ~ %02d"), (i * 2) + 13, (i * 2) + 14);
// 			m_pGridTsite.SetItemText(m_nTsiteInsOut[0][i], m_nTsiteInsOut[1][i], strTemp);
// 
// 			strTemp.Format(_T("IInsert_%02d ~ %02d"), (i * 2) + 19, (i * 2) + 20);
// 			m_pGridTsite.SetItemText(m_nTsiteInsOut[0][i + 3], m_nTsiteInsOut[1][i + 3], strTemp);
// 		}
// 
// 		for (i=0; i<6; i++)
// 		{
// 			strTemp.Format(_T("Pogo_%02d"),  i + 13);
// 			m_pGridTsite.SetItemText(m_nTsitePogoOut[0][i], m_nTsitePogoOut[1][i], strTemp);
// 
// 			strTemp.Format(_T("Pogo_%02d"), i + 19);
// 			m_pGridTsite.SetItemText(m_nTsitePogoOut[0][i + 6], m_nTsitePogoOut[1][i + 6], strTemp);
// 		}
// 	}
// 	else if (nNum == 2)
// 	{
// 		m_nInsertCyl[nNum][0]		= st_io_info.o_RiTsiteBtmSockIns1Cyl;
// 		m_nInsertFwdChk[nNum][0]	= st_io_info.i_RiTsiteBtmSockIns1FwdChk;
// 		m_nInsertBwdChk[nNum][0]	= st_io_info.i_RiTsiteBtmSockIns1BwdChk;
// 
// 		m_nInsertCyl[nNum][1]		= st_io_info.o_RiTsiteBtmSockIns2Cyl;
// 		m_nInsertFwdChk[nNum][1]	= st_io_info.i_RiTsiteBtmSockIns2FwdChk;
// 		m_nInsertBwdChk[nNum][1]	= st_io_info.i_RiTsiteBtmSockIns2BwdChk;
// 
// 		m_nInsertCyl[nNum][2]		= st_io_info.o_RiTsiteBtmSockIns3Cyl;
// 		m_nInsertFwdChk[nNum][2]	= st_io_info.i_RiTsiteBtmSockIns3FwdChk;
// 		m_nInsertBwdChk[nNum][2]	= st_io_info.i_RiTsiteBtmSockIns3BwdChk;
// 
// 		m_nInsertCyl[nNum][3]		= st_io_info.o_RiTsiteBtmSockIns4Cyl;
// 		m_nInsertFwdChk[nNum][3]	= st_io_info.i_RiTsiteBtmSockIns4FwdChk;
// 		m_nInsertBwdChk[nNum][3]	= st_io_info.i_RiTsiteBtmSockIns4BwdChk;
// 
// 		m_nInsertCyl[nNum][4]		= st_io_info.o_RiTsiteBtmSockIns5Cyl;
// 		m_nInsertFwdChk[nNum][4]	= st_io_info.i_RiTsiteBtmSockIns5FwdChk;
// 		m_nInsertBwdChk[nNum][4]	= st_io_info.i_RiTsiteBtmSockIns5BwdChk;
// 
// 		m_nInsertCyl[nNum][5]		= st_io_info.o_RiTsiteBtmSockIns6Cyl;
// 		m_nInsertFwdChk[nNum][5]	= st_io_info.i_RiTsiteBtmSockIns6FwdChk;
// 		m_nInsertBwdChk[nNum][5]	= st_io_info.i_RiTsiteBtmSockIns6BwdChk;
// 
// 		m_nPogoCyl[nNum][0]			= st_io_info.o_RiTSiteBtmPogoCyl[0];
// 		m_nPogoOnOffChk[nNum][0]	= st_io_info.i_RiTSiteBtmPogoChk[0];
// 
// 		m_nPogoCyl[nNum][1]			= st_io_info.o_RiTSiteBtmPogoCyl[1];
// 		m_nPogoOnOffChk[nNum][1]	= st_io_info.i_RiTSiteBtmPogoChk[1];
// 
// 		m_nPogoCyl[nNum][2]			= st_io_info.o_RiTSiteBtmPogoCyl[2];
// 		m_nPogoOnOffChk[nNum][2]	= st_io_info.i_RiTSiteBtmPogoChk[2];
// 
// 		m_nPogoCyl[nNum][3]			= st_io_info.o_RiTSiteBtmPogoCyl[3];
// 		m_nPogoOnOffChk[nNum][3]	= st_io_info.i_RiTSiteBtmPogoChk[3];
// 
// 		m_nPogoCyl[nNum][4]			= st_io_info.o_RiTSiteBtmPogoCyl[4];
// 		m_nPogoOnOffChk[nNum][4]	= st_io_info.i_RiTSiteBtmPogoChk[4];
// 
// 		m_nPogoCyl[nNum][5]			= st_io_info.o_RiTSiteBtmPogoCyl[5];
// 		m_nPogoOnOffChk[nNum][5]	= st_io_info.i_RiTSiteBtmPogoChk[5];
// 
// 		m_nPogoCyl[nNum][6]			= st_io_info.o_RiTSiteBtmPogoCyl[6];
// 		m_nPogoOnOffChk[nNum][6]	= st_io_info.i_RiTSiteBtmPogoChk[6];
// 
// 		m_nPogoCyl[nNum][7]			= st_io_info.o_RiTSiteBtmPogoCyl[7];
// 		m_nPogoOnOffChk[nNum][7]	= st_io_info.i_RiTSiteBtmPogoChk[7];
// 
// 		m_nPogoCyl[nNum][8]			= st_io_info.o_RiTSiteBtmPogoCyl[8];
// 		m_nPogoOnOffChk[nNum][8]	= st_io_info.i_RiTSiteBtmPogoChk[8];
// 
// 		m_nPogoCyl[nNum][9]			= st_io_info.o_RiTSiteBtmPogoCyl[9];
// 		m_nPogoOnOffChk[nNum][9]	= st_io_info.i_RiTSiteBtmPogoChk[9];
// 
// 		m_nPogoCyl[nNum][10]		= st_io_info.o_RiTSiteBtmPogoCyl[10];
// 		m_nPogoOnOffChk[nNum][10]	= st_io_info.i_RiTSiteBtmPogoChk[10];
// 
// 		m_nPogoCyl[nNum][11]		= st_io_info.o_RiTSiteBtmPogoCyl[11];
// 		m_nPogoOnOffChk[nNum][11]	= st_io_info.i_RiTSiteBtmPogoChk[11];

// 		for (i=0; i<3; i++)
// 		{
// 			strTemp.Format(_T("Insert_%02d ~ %02d"), (i * 2) + 25, (i * 2) + 26);
// 			m_pGridTsite.SetItemText(m_nTsiteInsOut[0][i], m_nTsiteInsOut[1][i], strTemp);
// 
// 			strTemp.Format(_T("IInsert_%02d ~ %02d"), (i * 2) + 31, (i * 2) + 32);
// 			m_pGridTsite.SetItemText(m_nTsiteInsOut[0][i + 3], m_nTsiteInsOut[1][i + 3], strTemp);
// 		}
// 
// 		for (i=0; i<6; i++)
// 		{
// 			strTemp.Format(_T("Pogo_%02d"),  i + 25);
// 			m_pGridTsite.SetItemText(m_nTsitePogoOut[0][i], m_nTsitePogoOut[1][i], strTemp);
// 
// 			strTemp.Format(_T("Pogo_%02d"), i + 31);
// 			m_pGridTsite.SetItemText(m_nTsitePogoOut[0][i + 6], m_nTsitePogoOut[1][i + 6], strTemp);
// 		}
// 	}
// 	else
// 	{
// 		m_nInsertCyl[nNum][0]		= st_io_info.o_RiTsiteTopSockIns1Cyl;
// 		m_nInsertFwdChk[nNum][0]	= st_io_info.i_RiTsiteTopSockIns1FwdChk;
// 		m_nInsertBwdChk[nNum][0]	= st_io_info.i_RiTsiteTopSockIns1BwdChk;
// 														 
// 		m_nInsertCyl[nNum][1]		= st_io_info.o_RiTsiteTopSockIns2Cyl;
// 		m_nInsertFwdChk[nNum][1]	= st_io_info.i_RiTsiteTopSockIns2FwdChk;
// 		m_nInsertBwdChk[nNum][1]	= st_io_info.i_RiTsiteTopSockIns2BwdChk;
// 
// 		m_nInsertCyl[nNum][2]		= st_io_info.o_RiTsiteTopSockIns3Cyl;
// 		m_nInsertFwdChk[nNum][2]	= st_io_info.i_RiTsiteTopSockIns3FwdChk;
// 		m_nInsertBwdChk[nNum][2]	= st_io_info.i_RiTsiteTopSockIns3BwdChk;
// 
// 		m_nInsertCyl[nNum][3]		= st_io_info.o_RiTsiteTopSockIns4Cyl;
// 		m_nInsertFwdChk[nNum][3]	= st_io_info.i_RiTsiteTopSockIns4FwdChk;
// 		m_nInsertBwdChk[nNum][3]	= st_io_info.i_RiTsiteTopSockIns4BwdChk;
// 
// 		m_nInsertCyl[nNum][4]		= st_io_info.o_RiTsiteTopSockIns5Cyl;
// 		m_nInsertFwdChk[nNum][4]	= st_io_info.i_RiTsiteTopSockIns5FwdChk;
// 		m_nInsertBwdChk[nNum][4]	= st_io_info.i_RiTsiteTopSockIns5BwdChk;
// 
// 		m_nInsertCyl[nNum][5]		= st_io_info.o_RiTsiteTopSockIns6Cyl;
// 		m_nInsertFwdChk[nNum][5]	= st_io_info.i_RiTsiteTopSockIns6FwdChk;
// 		m_nInsertBwdChk[nNum][5]	= st_io_info.i_RiTsiteTopSockIns6BwdChk;
// 
// 		m_nPogoCyl[nNum][0]			= st_io_info.o_RiTSiteTopPogoCyl[0];
// 		m_nPogoOnOffChk[nNum][0]	= st_io_info.i_RiTSiteTopPogoChk[0];
// 
// 		m_nPogoCyl[nNum][1]			= st_io_info.o_RiTSiteTopPogoCyl[1];
// 		m_nPogoOnOffChk[nNum][1]	= st_io_info.i_RiTSiteTopPogoChk[1];
// 
// 		m_nPogoCyl[nNum][2]			= st_io_info.o_RiTSiteTopPogoCyl[2];
// 		m_nPogoOnOffChk[nNum][2]	= st_io_info.i_RiTSiteTopPogoChk[2];
// 
// 		m_nPogoCyl[nNum][3]			= st_io_info.o_RiTSiteTopPogoCyl[3];
// 		m_nPogoOnOffChk[nNum][3]	= st_io_info.i_RiTSiteTopPogoChk[3];
// 
// 		m_nPogoCyl[nNum][4]			= st_io_info.o_RiTSiteTopPogoCyl[4];
// 		m_nPogoOnOffChk[nNum][4]	= st_io_info.i_RiTSiteTopPogoChk[4];
// 
// 		m_nPogoCyl[nNum][5]			= st_io_info.o_RiTSiteTopPogoCyl[5];
// 		m_nPogoOnOffChk[nNum][5]	= st_io_info.i_RiTSiteTopPogoChk[5];
// 
// 		m_nPogoCyl[nNum][6]			= st_io_info.o_RiTSiteTopPogoCyl[6];
// 		m_nPogoOnOffChk[nNum][6]	= st_io_info.i_RiTSiteTopPogoChk[6];
// 
// 		m_nPogoCyl[nNum][7]			= st_io_info.o_RiTSiteTopPogoCyl[7];
// 		m_nPogoOnOffChk[nNum][7]	= st_io_info.i_RiTSiteTopPogoChk[7];
// 
// 		m_nPogoCyl[nNum][8]			= st_io_info.o_RiTSiteTopPogoCyl[8];
// 		m_nPogoOnOffChk[nNum][8]	= st_io_info.i_RiTSiteTopPogoChk[8];
// 
// 		m_nPogoCyl[nNum][9]			= st_io_info.o_RiTSiteTopPogoCyl[9];
// 		m_nPogoOnOffChk[nNum][9]	= st_io_info.i_RiTSiteTopPogoChk[9];
// 
// 		m_nPogoCyl[nNum][10]		= st_io_info.o_RiTSiteTopPogoCyl[10];
// 		m_nPogoOnOffChk[nNum][10]	= st_io_info.i_RiTSiteTopPogoChk[10];
// 
// 		m_nPogoCyl[nNum][11]		= st_io_info.o_RiTSiteTopPogoCyl[11];
// 		m_nPogoOnOffChk[nNum][11]	= st_io_info.i_RiTSiteTopPogoChk[11];

// 		for (i=0; i<3; i++)
// 		{
// 			strTemp.Format(_T("Insert_%02d ~ %02d"), (i * 2) + 37, (i * 2) + 38);
// 			m_pGridTsite.SetItemText(m_nTsiteInsOut[0][i], m_nTsiteInsOut[1][i], strTemp);
// 
// 			strTemp.Format(_T("IInsert_%02d ~ %02d"), (i * 2) + 43, (i * 2) + 44);
// 			m_pGridTsite.SetItemText(m_nTsiteInsOut[0][i + 3], m_nTsiteInsOut[1][i + 3], strTemp);
// 		}
// 
// 		for (i=0; i<6; i++)
// 		{
// 			strTemp.Format(_T("Pogo_%02d"),  i + 37);
// 			m_pGridTsite.SetItemText(m_nTsitePogoOut[0][i], m_nTsitePogoOut[1][i], strTemp);
// 
// 			strTemp.Format(_T("Pogo_%02d"), i + 43);
// 			m_pGridTsite.SetItemText(m_nTsitePogoOut[0][i + 6], m_nTsitePogoOut[1][i + 6], strTemp);
// 		}
// 	}
}

// void CDialogManualIO::OnLeftContactCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
// {
// 	int nRow, nCol;
// 	CString strTmp;
// 
// 	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;
// 
// 	nRow				= pItem->iRow;
// 	nCol				= pItem->iColumn;
// 
// 	switch (nRow)
// 	{
// 		case 1:
// 			switch (nCol)
// 			{
// 				case 1:
// 					FAS_IO.set_out_bit(m_nLfCokCyl[0], !FAS_IO.get_out_bit(m_nLfCokCyl[0], IO_ON));
// 					break;
// 
// 				case 2:
// 					FAS_IO.set_out_bit(m_nLfCokCyl[0], !FAS_IO.get_out_bit(m_nLfCokCyl[0], IO_ON));
// 					break;
// 
// 				case 3:
// 					FAS_IO.set_out_bit(m_nLfCokCyl[1], !FAS_IO.get_out_bit(m_nLfCokCyl[1], IO_ON));
// 					break;
// 
// 				case 4:
// 					FAS_IO.set_out_bit(m_nLfCokCyl[1], !FAS_IO.get_out_bit(m_nLfCokCyl[1], IO_ON));
// 					break;
// 			}
// 			break;
// 
// 		case 4:
// 			switch (nCol)
// 			{
// 				case 1:
// 					FAS_IO.set_out_bit(m_nLfContactFwdCyl[0], !FAS_IO.get_out_bit(m_nLfContactFwdCyl[0], IO_ON));
// 					break;
// 
// 				case 2:
// 					FAS_IO.set_out_bit(m_nLfContactBwdCyl[0], !FAS_IO.get_out_bit(m_nLfContactBwdCyl[0], IO_ON));
// 					break;
// 
// 				case 3:
// 					FAS_IO.set_out_bit(m_nLfContactFwdCyl[1], !FAS_IO.get_out_bit(m_nLfContactFwdCyl[1], IO_ON));
// 					break;
// 
// 				case 4:
// 					FAS_IO.set_out_bit(m_nLfContactBwdCyl[1], !FAS_IO.get_out_bit(m_nLfContactBwdCyl[1], IO_ON));
// 					break;
// 			}
// 			break;
// 
// 		case 7:
// 			switch (nCol)
// 			{
// 				case 1:
// 					FAS_IO.set_out_bit(m_nLfHifixFwdCyl, !FAS_IO.get_out_bit(m_nLfHifixFwdCyl, IO_ON));
// 					break;
// 
// 				case 2:
// 					FAS_IO.set_out_bit(m_nLfHifixBwdCyl, !FAS_IO.get_out_bit(m_nLfHifixBwdCyl, IO_ON));
// 					break;
// 			}
// 			break;
// 
// 		case 11:
// 			switch (nCol)
// 			{
// 				case 1:
// 					FAS_IO.set_out_bit(m_nLfMpFwdCyl, !FAS_IO.get_out_bit(m_nLfMpFwdCyl, IO_ON));
// 					break;
// 
// 				case 2:
// 					FAS_IO.set_out_bit(m_nLfMpBwdCyl, !FAS_IO.get_out_bit(m_nLfMpBwdCyl, IO_ON));
// 					break;
// 			}
// 			break;
// 	}
// }

// void CDialogManualIO::OnRightContactCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
// {
// 	int nRow, nCol;
// 	CString strTmp;
// 
// 	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;
// 
// 	nRow				= pItem->iRow;
// 	nCol				= pItem->iColumn;
// 
// 	switch (nRow)
// 	{
// 		case 1:
// 			switch (nCol)
// 			{
// 				case 1:
// 					FAS_IO.set_out_bit(m_nRiCokCyl[0], !FAS_IO.get_out_bit(m_nRiCokCyl[0], IO_ON));
// 					break;
// 
// 				case 2:
// 					FAS_IO.set_out_bit(m_nRiCokCyl[0], !FAS_IO.get_out_bit(m_nRiCokCyl[0], IO_ON));
// 					break;
// 
// 				case 3:
// 					FAS_IO.set_out_bit(m_nRiCokCyl[1], !FAS_IO.get_out_bit(m_nRiCokCyl[1], IO_ON));
// 					break;
// 
// 				case 4:
// 					FAS_IO.set_out_bit(m_nRiCokCyl[1], !FAS_IO.get_out_bit(m_nRiCokCyl[1], IO_ON));
// 					break;
// 			}
// 			break;
// 
// 		case 4:
// 			switch (nCol)
// 			{
// 				case 1:
// 					FAS_IO.set_out_bit(m_nRiContactFwdCyl[0], !FAS_IO.get_out_bit(m_nRiContactFwdCyl[0], IO_ON));
// 					break;
// 
// 				case 2:
// 					FAS_IO.set_out_bit(m_nRiContactBwdCyl[0], !FAS_IO.get_out_bit(m_nRiContactBwdCyl[0], IO_ON));
// 					break;
// 
// 				case 3:
// 					FAS_IO.set_out_bit(m_nRiContactFwdCyl[1], !FAS_IO.get_out_bit(m_nRiContactFwdCyl[1], IO_ON));
// 					break;
// 
// 				case 4:
// 					FAS_IO.set_out_bit(m_nRiContactBwdCyl[1], !FAS_IO.get_out_bit(m_nRiContactBwdCyl[1], IO_ON));
// 					break;
// 			}
// 			break;
// 
// 		case 7:
// 			switch (nCol)
// 			{
// 				case 1:
// 					FAS_IO.set_out_bit(m_nRiHifixFwdCyl, !FAS_IO.get_out_bit(m_nRiHifixFwdCyl, IO_ON));
// 					break;
// 
// 				case 2:
// 					FAS_IO.set_out_bit(m_nRiHifixBwdCyl, !FAS_IO.get_out_bit(m_nRiHifixBwdCyl, IO_ON));
// 					break;
// 			}
// 			break;
// 
// 		case 11:
// 			switch (nCol)
// 			{
// 				case 1:
// 					FAS_IO.set_out_bit(m_nRiMpFwdCyl, !FAS_IO.get_out_bit(m_nRiMpFwdCyl, IO_ON));
// 					break;
// 
// 				case 2:
// 					FAS_IO.set_out_bit(m_nRiMpBwdCyl, !FAS_IO.get_out_bit(m_nRiMpBwdCyl, IO_ON));
// 					break;
// 			}
// 			break;
// 	}
// }

// void CDialogManualIO::OnBufferCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
// {
// 	int nRow, nCol;
// 	CString strTmp;
// 
// 	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;
// 
// 	nRow				= pItem->iRow;
// 	nCol				= pItem->iColumn;
// 
// 	switch (nRow)
// 	{
// 		case 1:
// 			switch (nCol)
// 			{
// 				case 1:
// 					FAS_IO.set_out_bit(m_nRetestMvFwdCyl, !FAS_IO.get_out_bit(m_nRetestMvFwdCyl, IO_ON));
// 					break;
// 
// 				case 2:
// 					FAS_IO.set_out_bit(m_nRetestMvBwdCyl, !FAS_IO.get_out_bit(m_nRetestMvBwdCyl, IO_ON));
// 					break;
// 			}
// 			break;
// 
// 		case 2:
// 			switch (nCol)
// 			{
// 				case 1:
// 					FAS_IO.set_out_bit(m_nRetestAlignCyl, !FAS_IO.get_out_bit(m_nRetestAlignCyl, IO_ON));
// 					break;
// 
// 				case 2:
// 					FAS_IO.set_out_bit(m_nRetestAlignCyl, !FAS_IO.get_out_bit(m_nRetestAlignCyl, IO_ON));
// 					break;
// 			}
// 			break;
// 
// 		case 3:
// 			switch (nCol)
// 			{
// 				case 1:
// 					FAS_IO.set_out_bit(m_nRejectClampCyl, !FAS_IO.get_out_bit(m_nRejectClampCyl, IO_ON));
// 					break;
// 
// 				case 2:
// 					FAS_IO.set_out_bit(m_nRejectClampCyl, !FAS_IO.get_out_bit(m_nRejectClampCyl, IO_ON));
// 					break;
// 			}
// 			break;
// 
// 		case 4:
// 			switch (nCol)
// 			{
// 				case 1:
// 					FAS_IO.set_out_bit(m_nRejectUpDnCyl, !FAS_IO.get_out_bit(m_nRejectUpDnCyl, IO_ON));
// 					break;
// 
// 				case 2:
// 					FAS_IO.set_out_bit(m_nRejectUpDnCyl, !FAS_IO.get_out_bit(m_nRejectUpDnCyl, IO_ON));
// 					break;
// 			}
// 			break;
// 
// 		case 5:
// 			switch (nCol)
// 			{
// 				case 1:
// 					FAS_IO.set_out_bit(m_nRejectMvFwdCyl, !FAS_IO.get_out_bit(m_nRejectMvFwdCyl, IO_ON));
// 					break;
// 
// 				case 2:
// 					FAS_IO.set_out_bit(m_nRejectMvBwdCyl, !FAS_IO.get_out_bit(m_nRejectMvBwdCyl, IO_ON));
// 					break;
// 			}
// 			break;
// 	}
// }

void CDialogManualIO::OnDisplayLdRobot()
{
//	int i;

// 	for (i=0; i<2; i++)
// 	{
// 		if (FAS_IO.get_in_bit(m_nLdFwdChk[i], IO_OFF) == IO_OFF)
// 		{
// 			m_pGridLdRbt.SetItemBkColour(m_nLdDnIn[0][i], m_nLdDnIn[1][i], YELLOW_D, BLACK_C);
// 		}
// 		else
// 		{
// 			m_pGridLdRbt.SetItemBkColour(m_nLdDnIn[0][i], m_nLdDnIn[1][i], YELLOW_C, BLACK_C);
// 		}
// 
// 		if (FAS_IO.get_in_bit(m_nLdBwdChk[i], IO_OFF) == IO_OFF)
// 		{
// 			m_pGridLdRbt.SetItemBkColour(m_nLdUpIn[0][i], m_nLdUpIn[1][i], YELLOW_D, BLACK_C);
// 		}
// 		else
// 		{
// 			m_pGridLdRbt.SetItemBkColour(m_nLdUpIn[0][i], m_nLdUpIn[1][i], YELLOW_C, BLACK_C);
// 		}
// 
// 		if (FAS_IO.get_in_bit(m_nLdVacChk[i], IO_OFF) == IO_OFF)
// 		{
// 			m_pGridLdRbt.SetItemBkColour(m_nLdVacIn[0][i], m_nLdVacIn[1][i], YELLOW_D, BLACK_C);
// 		}
// 		else
// 		{
// 			m_pGridLdRbt.SetItemBkColour(m_nLdVacIn[0][i], m_nLdVacIn[1][i], YELLOW_C, BLACK_C);
// 		}
// 
// 		if (FAS_IO.get_out_bit(m_nLdCyl[i], IO_OFF) == IO_OFF)
// 		{
// 			m_pGridLdRbt.SetItemBkColour(m_nLdDnOut[0][i], m_nLdDnOut[1][i], GREEN_D, BLACK_C);
// 		}
// 		else
// 		{
// 			m_pGridLdRbt.SetItemBkColour(m_nLdDnOut[0][i], m_nLdDnOut[1][i], GREEN_C, BLACK_C);
// 		}
// 
// 		if (FAS_IO.get_out_bit(m_nLdVac[i], IO_OFF) == IO_OFF)
// 		{
// 			m_pGridLdRbt.SetItemBkColour(m_nLdVacOut[0][i], m_nLdVacOut[1][i], GREEN_D, BLACK_C);
// 		}
// 		else
// 		{
// 			m_pGridLdRbt.SetItemBkColour(m_nLdVacOut[0][i], m_nLdVacOut[1][i], GREEN_C, BLACK_C);
// 		}
// 
// 		if (FAS_IO.get_out_bit(m_nLdEjc[i], IO_OFF) == IO_OFF)
// 		{
// 			m_pGridLdRbt.SetItemBkColour(m_nLdEjcOut[0][i], m_nLdEjcOut[1][i], GREEN_D, BLACK_C);
// 		}
// 		else
// 		{
// 			m_pGridLdRbt.SetItemBkColour(m_nLdEjcOut[0][i], m_nLdEjcOut[1][i], GREEN_C, BLACK_C);
// 		}
// 	}

	m_pGridLdRbt.Invalidate(FALSE);
}

// void CDialogManualIO::OnDisplayUnLdRobot()
// {
// 	int i;
// 
// 	for (i=0; i<2; i++)
// 	{
// 		if (FAS_IO.get_in_bit(m_nUnLdFwdChk[i], IO_OFF) == IO_OFF)
// 		{
// 			m_pGridUldRbt.SetItemBkColour(m_nUnLdDnIn[0][i], m_nUnLdDnIn[1][i], YELLOW_D, BLACK_C);
// 		}
// 		else
// 		{
// 			m_pGridUldRbt.SetItemBkColour(m_nUnLdDnIn[0][i], m_nUnLdDnIn[1][i], YELLOW_C, BLACK_C);
// 		}
// 
// 		if (FAS_IO.get_in_bit(m_nUnLdBwdChk[i], IO_OFF) == IO_OFF)
// 		{
// 			m_pGridUldRbt.SetItemBkColour(m_nUnLdUpIn[0][i], m_nUnLdUpIn[1][i], YELLOW_D, BLACK_C);
// 		}
// 		else
// 		{
// 			m_pGridUldRbt.SetItemBkColour(m_nUnLdUpIn[0][i], m_nUnLdUpIn[1][i], YELLOW_C, BLACK_C);
// 		}
// 
// 		if (FAS_IO.get_in_bit(m_nUnLdVacChk[i], IO_OFF) == IO_OFF)
// 		{
// 			m_pGridUldRbt.SetItemBkColour(m_nUnLdVacIn[0][i], m_nUnLdVacIn[1][i], YELLOW_D, BLACK_C);
// 		}
// 		else
// 		{
// 			m_pGridUldRbt.SetItemBkColour(m_nUnLdVacIn[0][i], m_nUnLdVacIn[1][i], YELLOW_C, BLACK_C);
// 		}
// 
// 		if (FAS_IO.get_out_bit(m_nUnLdCyl[i], IO_OFF) == IO_OFF)
// 		{
// 			m_pGridUldRbt.SetItemBkColour(m_nUnLdDnOut[0][i], m_nUnLdDnOut[1][i], GREEN_D, BLACK_C);
// 		}
// 		else
// 		{
// 			m_pGridUldRbt.SetItemBkColour(m_nUnLdDnOut[0][i], m_nUnLdDnOut[1][i], GREEN_C, BLACK_C);
// 		}
// 
// 		if (FAS_IO.get_out_bit(m_nUnLdVac[i], IO_OFF) == IO_OFF)
// 		{
// 			m_pGridUldRbt.SetItemBkColour(m_nUnLdVacOut[0][i], m_nUnLdVacOut[1][i], GREEN_D, BLACK_C);
// 		}
// 		else
// 		{
// 			m_pGridUldRbt.SetItemBkColour(m_nUnLdVacOut[0][i], m_nUnLdVacOut[1][i], GREEN_C, BLACK_C);
// 		}
// 
// 		if (FAS_IO.get_out_bit(m_nUnLdEjc[i], IO_OFF) == IO_OFF)
// 		{
// 			m_pGridUldRbt.SetItemBkColour(m_nUnLdEjcOut[0][i], m_nUnLdEjcOut[1][i], GREEN_D, BLACK_C);
// 		}
// 		else
// 		{
// 			m_pGridUldRbt.SetItemBkColour(m_nUnLdEjcOut[0][i], m_nUnLdEjcOut[1][i], GREEN_C, BLACK_C);
// 		}
// 	}
// 
// 	m_pGridUldRbt.Invalidate(FALSE);
// }

void CDialogManualIO::OnDisplayTestSiteRobot()
{
//	int i;

// 	for (i=0; i<4; i++)
// 	{
// 		if (FAS_IO.get_in_bit(m_nTsiteFwdChk[i], IO_OFF) == IO_OFF)
// 		{
// 			m_pGridTRbt.SetItemBkColour(m_nTsiteDnIn[0][i], m_nTsiteDnIn[1][i], YELLOW_D, BLACK_C);
// 		}
// 		else
// 		{
// 			m_pGridTRbt.SetItemBkColour(m_nTsiteDnIn[0][i], m_nTsiteDnIn[1][i], YELLOW_C, BLACK_C);
// 		}
// 
// 		if (FAS_IO.get_in_bit(m_nTsiteBwdChk[i], IO_OFF) == IO_OFF)
// 		{
// 			m_pGridTRbt.SetItemBkColour(m_nTsiteUpIn[0][i], m_nTsiteUpIn[1][i], YELLOW_D, BLACK_C);
// 		}
// 		else
// 		{
// 			m_pGridTRbt.SetItemBkColour(m_nTsiteUpIn[0][i], m_nTsiteUpIn[1][i], YELLOW_C, BLACK_C);
// 		}
// 
// 		if (FAS_IO.get_in_bit(m_nTsiteVacChk[i], IO_OFF) == IO_OFF)
// 		{
// 			m_pGridTRbt.SetItemBkColour(m_nTsiteVacIn[0][i], m_nTsiteVacIn[1][i], YELLOW_D, BLACK_C);
// 		}
// 		else
// 		{
// 			m_pGridTRbt.SetItemBkColour(m_nTsiteVacIn[0][i], m_nTsiteVacIn[1][i], YELLOW_C, BLACK_C);
// 		}
// 
// 		if (FAS_IO.get_out_bit(m_nTsiteCyl[i], IO_OFF) == IO_OFF)
// 		{
// 			m_pGridTRbt.SetItemBkColour(m_nTsiteDnOut[0][i], m_nTsiteDnOut[1][i], GREEN_D, BLACK_C);
// 		}
// 		else
// 		{
// 			m_pGridTRbt.SetItemBkColour(m_nTsiteDnOut[0][i], m_nTsiteDnOut[1][i], GREEN_C, BLACK_C);
// 		}
// 
// 		if (FAS_IO.get_out_bit(m_nTsiteVac[i], IO_OFF) == IO_OFF)
// 		{
// 			m_pGridTRbt.SetItemBkColour(m_nTsiteVacOut[0][i], m_nTsiteVacOut[1][i], GREEN_D, BLACK_C);
// 		}
// 		else
// 		{
// 			m_pGridTRbt.SetItemBkColour(m_nTsiteVacOut[0][i], m_nTsiteVacOut[1][i], GREEN_C, BLACK_C);
// 		}
// 
// 		if (FAS_IO.get_out_bit(m_nTsiteEjc[i], IO_OFF) == IO_OFF)
// 		{
// 			m_pGridTRbt.SetItemBkColour(m_nTsiteEjcOut[0][i], m_nTsiteEjcOut[1][i], GREEN_D, BLACK_C);
// 		}
// 		else
// 		{
// 			m_pGridTRbt.SetItemBkColour(m_nTsiteEjcOut[0][i], m_nTsiteEjcOut[1][i], GREEN_C, BLACK_C);
// 		}
// 	}

	m_pGridTRbt.Invalidate(FALSE);
}

void CDialogManualIO::OnDisplayTestSite()
{
// 	int i;
// 
// 	for (i=0; i<3; i++)
// 	{
// 		if (FAS_IO.get_out_bit(m_nInsertCyl[m_nTsiteNum][i], IO_OFF) == IO_OFF)
// 		{
// 			m_pGridTsite.SetItemBkColour(m_nTsiteInsOut[0][i], m_nTsiteInsOut[1][i], GREEN_D, BLACK_C);
// 		}
// 		else
// 		{
// 			m_pGridTsite.SetItemBkColour(m_nTsiteInsOut[0][i], m_nTsiteInsOut[1][i], GREEN_C, BLACK_C);
// 		}
// 
// 		if (FAS_IO.get_in_bit(m_nInsertFwdChk[m_nTsiteNum][i], IO_OFF) == IO_OFF)
// 		{
// 			m_pGridTsite.SetItemBkColour(m_nTsiteInsFwd[0][i], m_nTsiteInsFwd[1][i], YELLOW_D, BLACK_C);
// 		}
// 		else
// 		{
// 			m_pGridTsite.SetItemBkColour(m_nTsiteInsFwd[0][i], m_nTsiteInsFwd[1][i], YELLOW_C, BLACK_C);
// 		}
// 		
// 		if (FAS_IO.get_in_bit(m_nInsertBwdChk[m_nTsiteNum][i], IO_OFF) == IO_OFF)
// 		{
// 			m_pGridTsite.SetItemBkColour(m_nTsiteInsBwd[0][i], m_nTsiteInsBwd[1][i], YELLOW_D, BLACK_C);
// 		}
// 		else
// 		{
// 			m_pGridTsite.SetItemBkColour(m_nTsiteInsBwd[0][i], m_nTsiteInsBwd[1][i], YELLOW_C, BLACK_C);
// 		}
// 		
// 		if (FAS_IO.get_out_bit(m_nInsertCyl[m_nTsiteNum][i + 3], IO_OFF) == IO_OFF)
// 		{
// 			m_pGridTsite.SetItemBkColour(m_nTsiteInsOut[0][i + 3], m_nTsiteInsOut[1][i + 3], GREEN_D, BLACK_C);
// 		}
// 		else
// 		{
// 			m_pGridTsite.SetItemBkColour(m_nTsiteInsOut[0][i + 3], m_nTsiteInsOut[1][i + 3], GREEN_C, BLACK_C);
// 		}
// 		
// 		if (FAS_IO.get_in_bit(m_nInsertFwdChk[m_nTsiteNum][i + 3], IO_OFF) == IO_OFF)
// 		{
// 			m_pGridTsite.SetItemBkColour(m_nTsiteInsFwd[0][i + 3], m_nTsiteInsFwd[1][i + 3], YELLOW_D, BLACK_C);
// 		}
// 		else
// 		{
// 			m_pGridTsite.SetItemBkColour(m_nTsiteInsFwd[0][i + 3], m_nTsiteInsFwd[1][i + 3], YELLOW_C, BLACK_C);
// 		}
// 		
// 		if (FAS_IO.get_in_bit(m_nInsertBwdChk[m_nTsiteNum][i + 3], IO_OFF) == IO_OFF)
// 		{
// 			m_pGridTsite.SetItemBkColour(m_nTsiteInsBwd[0][i + 3], m_nTsiteInsBwd[1][i + 3], YELLOW_D, BLACK_C);
// 		}
// 		else
// 		{
// 			m_pGridTsite.SetItemBkColour(m_nTsiteInsBwd[0][i + 3], m_nTsiteInsBwd[1][i + 3], YELLOW_C, BLACK_C);
// 		}
// 	}
// 
// 	for (i=0; i<6; i++)
// 	{
// 		if (FAS_IO.get_out_bit(m_nPogoCyl[m_nTsiteNum][i], IO_OFF) == IO_OFF)
// 		{
// 			m_pGridTsite.SetItemBkColour(m_nTsitePogoOut[0][i], m_nTsitePogoOut[1][i], GREEN_D, BLACK_C);
// 		}
// 		else
// 		{
// 			m_pGridTsite.SetItemBkColour(m_nTsitePogoOut[0][i], m_nTsitePogoOut[1][i], GREEN_C, BLACK_C);
// 		}
// 		
// 		if (FAS_IO.get_in_bit(m_nPogoOnOffChk[m_nTsiteNum][i], IO_OFF) == IO_OFF)
// 		{
// 			m_pGridTsite.SetItemBkColour(m_nTsitePogoIn[0][i], m_nTsitePogoIn[1][i], YELLOW_D, BLACK_C);
// 		}
// 		else
// 		{
// 			m_pGridTsite.SetItemBkColour(m_nTsitePogoIn[0][i], m_nTsitePogoIn[1][i], YELLOW_C, BLACK_C);
// 		}
// 		
// 		if (FAS_IO.get_out_bit(m_nPogoCyl[m_nTsiteNum][i + 6], IO_OFF) == IO_OFF)
// 		{
// 			m_pGridTsite.SetItemBkColour(m_nTsitePogoOut[0][i + 6], m_nTsitePogoOut[1][i + 6], GREEN_D, BLACK_C);
// 		}
// 		else
// 		{
// 			m_pGridTsite.SetItemBkColour(m_nTsitePogoOut[0][i + 6], m_nTsitePogoOut[1][i + 6], GREEN_C, BLACK_C);
// 		}
// 		
// 		if (FAS_IO.get_in_bit(m_nPogoOnOffChk[m_nTsiteNum][i + 6], IO_OFF) == IO_OFF)
// 		{
// 			m_pGridTsite.SetItemBkColour(m_nTsitePogoIn[0][i + 6], m_nTsitePogoIn[1][i + 6], YELLOW_D, BLACK_C);
// 		}
// 		else
// 		{
// 			m_pGridTsite.SetItemBkColour(m_nTsitePogoIn[0][i + 6], m_nTsitePogoIn[1][i + 6], YELLOW_C, BLACK_C);
// 		}
// 	}

	m_pGridTsite.Invalidate(FALSE);
}


void CDialogManualIO::OnDisplayBuffer()
{
	// retest move
// 	if (FAS_IO.get_out_bit(m_nRetestMvFwdCyl, IO_OFF) == IO_OFF)
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRetestMvFwdOut[0], m_nRetestMvFwdOut[1], GREEN_D, BLACK_C);
// 	}
// 	else
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRetestMvFwdOut[0], m_nRetestMvFwdOut[1], GREEN_C, BLACK_C);
// 	}
// 
// 	if (FAS_IO.get_out_bit(m_nRetestMvBwdCyl, IO_OFF) == IO_OFF)
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRetestMvBwdOut[0], m_nRetestMvBwdOut[1], GREEN_D, BLACK_C);
// 	}
// 	else
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRetestMvBwdOut[0], m_nRetestMvBwdOut[1], GREEN_C, BLACK_C);
// 	}
// 
// 	if (FAS_IO.get_in_bit(m_nRetestMvFwdChk, IO_OFF) == IO_OFF)
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRetestMvFwd[0], m_nRetestMvFwd[1], YELLOW_D, BLACK_C);
// 	}
// 	else
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRetestMvFwd[0], m_nRetestMvFwd[1], YELLOW_C, BLACK_C);
// 	}
// 
// 	if (FAS_IO.get_in_bit(m_nRetestMvBwdChk, IO_OFF) == IO_OFF)
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRetestMvBwd[0], m_nRetestMvBwd[1], YELLOW_D, BLACK_C);
// 	}
// 	else
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRetestMvBwd[0], m_nRetestMvBwd[1], YELLOW_C, BLACK_C);
// 	}
// 	
// 	// retest align
// 	if (FAS_IO.get_out_bit(m_nRetestAlignCyl, IO_OFF) == IO_OFF)
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRetestAlignOut[0], m_nRetestAlignOut[1], GREEN_D, BLACK_C);
// 	}
// 	else
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRetestAlignOut[0], m_nRetestAlignOut[1], GREEN_C, BLACK_C);
// 	}
// 
// 	if (FAS_IO.get_in_bit(m_nRetestAlignFwdChk, IO_OFF) == IO_OFF)
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRetestAlignFwd[0], m_nRetestAlignFwd[1], YELLOW_D, BLACK_C);
// 	}
// 	else
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRetestAlignFwd[0], m_nRetestAlignFwd[1], YELLOW_C, BLACK_C);
// 	}
// 
// 	if (FAS_IO.get_in_bit(m_nRetestAlignBwdChk, IO_OFF) == IO_OFF)
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRetestAlignBwd[0], m_nRetestAlignBwd[1], YELLOW_D, BLACK_C);
// 	}
// 	else
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRetestAlignBwd[0], m_nRetestAlignBwd[1], YELLOW_C, BLACK_C);
// 	}
// 
// 	// reject clamp
// 	if (FAS_IO.get_out_bit(m_nRejectClampCyl, IO_OFF) == IO_OFF)
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRejectClampOut[0], m_nRejectClampOut[1], GREEN_D, BLACK_C);
// 	}
// 	else
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRejectClampOut[0], m_nRejectClampOut[1], GREEN_C, BLACK_C);
// 	}
// 
// 	if (FAS_IO.get_in_bit(m_nRejectClampFwdChk, IO_OFF) == IO_OFF)
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRejectClampFwd[0], m_nRejectClampFwd[1], YELLOW_D, BLACK_C);
// 	}
// 	else
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRejectClampFwd[0], m_nRejectClampFwd[1], YELLOW_C, BLACK_C);
// 	}
// 
// 	if (FAS_IO.get_in_bit(m_nRejectClampBwdChk, IO_OFF) == IO_OFF)
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRejectClampBwd[0], m_nRejectClampBwd[1], YELLOW_D, BLACK_C);
// 	}
// 	else
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRejectClampBwd[0], m_nRejectClampBwd[1], YELLOW_C, BLACK_C);
// 	}
// 
// 	// reject up/dn
// 	if (FAS_IO.get_out_bit(m_nRejectUpDnCyl, IO_OFF) == IO_OFF)
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRejectUpDnOut[0], m_nRejectUpDnOut[1], GREEN_D, BLACK_C);
// 	}
// 	else
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRejectUpDnOut[0], m_nRejectUpDnOut[1], GREEN_C, BLACK_C);
// 	}
// 
// 	if (FAS_IO.get_in_bit(m_nRejectUpChk, IO_OFF) == IO_OFF)
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRejectUp[0], m_nRejectUp[1], YELLOW_D, BLACK_C);
// 	}
// 	else
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRejectUp[0], m_nRejectUp[1], YELLOW_C, BLACK_C);
// 	}
// 
// 	if (FAS_IO.get_in_bit(m_nRejectDnChk, IO_OFF) == IO_OFF)
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRejectDn[0], m_nRejectDn[1], YELLOW_D, BLACK_C);
// 	}
// 	else
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRejectDn[0], m_nRejectDn[1], YELLOW_C, BLACK_C);
// 	}
// 
// 	// rejectmove
// 	if (FAS_IO.get_out_bit(m_nRejectMvFwdCyl, IO_OFF) == IO_OFF)
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRejectMvFwdOut[0], m_nRejectMvFwdOut[1], GREEN_D, BLACK_C);
// 	}
// 	else
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRejectMvFwdOut[0], m_nRejectMvFwdOut[1], GREEN_C, BLACK_C);
// 	}
// 
// 	if (FAS_IO.get_out_bit(m_nRejectMvBwdCyl, IO_OFF) == IO_OFF)
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRejectMvBwdOut[0], m_nRejectMvBwdOut[1], GREEN_D, BLACK_C);
// 	}
// 	else
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRejectMvBwdOut[0], m_nRejectMvBwdOut[1], GREEN_C, BLACK_C);
// 	}
// 
// 	if (FAS_IO.get_in_bit(m_nRejectMvFwdChk, IO_OFF) == IO_OFF)
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRejectMvFwd[0], m_nRejectMvFwd[1], YELLOW_D, BLACK_C);
// 	}
// 	else
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRejectMvFwd[0], m_nRejectMvFwd[1], YELLOW_C, BLACK_C);
// 	}
// 
// 	if (FAS_IO.get_in_bit(m_nRejectMvBwdChk, IO_OFF) == IO_OFF)
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRejectMvBwd[0], m_nRejectMvBwd[1], YELLOW_D, BLACK_C);
// 	}
// 	else
// 	{
// 		m_pGridBuf.SetItemBkColour(m_nRejectMvBwd[0], m_nRejectMvBwd[1], YELLOW_C, BLACK_C);
// 	}

	m_pGridBuf.Invalidate(FALSE);
}

void CDialogManualIO::OnBnClickedBtnRefresh()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (st_handler_info.nRunStatus != dSTOP) return;

	CDialog_Message dlgMsg;

	//if (FAS_IO.get_in_bit(st_io_info.i_FrontSelectSwChk, IO_ON) == IO_OFF ||
	//	FAS_IO.get_in_bit(st_io_info.i_RearSelectSwChk, IO_ON)	== IO_OFF)
	if (FAS_IO.get_in_bit(st_io_info.i_ManualModeChk, IO_OFF) == IO_ON)
	{
		FAS_IO.set_out_bit(st_io_info.o_DoorLockLdUld, IO_OFF);
		FAS_IO.set_out_bit(st_io_info.o_DoorLockTestSite, IO_OFF);
	}
	else
	{
		dlgMsg.m_nMessageType	= 0;
		dlgMsg.m_strMessage		= _T("Manual Key Status Check");

		dlgMsg.DoModal();
	}
}


void CDialogManualIO::OnBnClickedBtnLeftClamp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (st_handler_info.nMenuLock != FALSE) return;
	st_handler_info.nMenuLock = TRUE;
}


void CDialogManualIO::OnBnClickedBtnLeftUnclamp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (st_handler_info.nMenuLock != FALSE) return;
	st_handler_info.nMenuLock = TRUE;
}


void CDialogManualIO::OnBnClickedBtnRightClamp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (st_handler_info.nMenuLock != FALSE) return;
	st_handler_info.nMenuLock = TRUE;
}


void CDialogManualIO::OnBnClickedBtnRightUnclamp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (st_handler_info.nMenuLock != FALSE) return;
	st_handler_info.nMenuLock = TRUE;
}
