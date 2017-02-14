// Dialog_Door_Open.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "Dialog_Door_Open.h"
#include "afxdialogex.h"
#include "FastechPublic_IO.h"
#include "Variable.h"
#include "PublicFunction.h"


// CDialog_Door_Open 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialog_Door_Open, CDialog)

CDialog_Door_Open::CDialog_Door_Open(CWnd* pParent /*=NULL*/)
	: CDialog(CDialog_Door_Open::IDD, pParent)
{
	m_bmGround.LoadBitmap(IDB_BITMAP_BACK_GROUND);   
}

CDialog_Door_Open::~CDialog_Door_Open()
{
}

void CDialog_Door_Open::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MSG_TOP_DOOR_1,	m_msgTopDoor[0]);
	DDX_Control(pDX, IDC_MSG_TOP_DOOR_2,	m_msgTopDoor[1]);
	DDX_Control(pDX, IDC_MSG_TOP_DOOR_3,	m_msgTopDoor[2]);
	DDX_Control(pDX, IDC_MSG_TOP_DOOR_4,	m_msgTopDoor[3]);
	DDX_Control(pDX, IDC_MSG_TOP_DOOR_5,	m_msgTopDoor[4]);
	DDX_Control(pDX, IDC_MSG_TOP_DOOR_6,	m_msgTopDoor[5]);
	DDX_Control(pDX, IDC_MSG_BOTTOM_DOOR_1,	m_msgBottomDoor[0]);
	DDX_Control(pDX, IDC_MSG_BOTTOM_DOOR_2,	m_msgBottomDoor[1]);
	DDX_Control(pDX, IDC_MSG_BOTTOM_DOOR_3,	m_msgBottomDoor[2]);
	DDX_Control(pDX, IDC_MSG_BOTTOM_DOOR_4,	m_msgBottomDoor[3]);
	DDX_Control(pDX, IDC_MSG_BOTTOM_DOOR_5,	m_msgBottomDoor[4]);
	DDX_Control(pDX, IDC_DIALOG_DOOR_OPEN, m_btnDoorOpen);
	DDX_Control(pDX, IDC_DIALOG_DOOR_CLOSE, m_btnDoorClose);
	DDX_Control(pDX, IDC_DIALOG_DOOR_EXIT, m_btnDoorExit);
	DDX_Control(pDX, IDC_GROUP_LD_ULD_DOOR, m_groupLdUldDoor);
	DDX_Control(pDX, IDC_TEST_DOOR, m_groupTestDoor);
	DDX_Control(pDX, IDC_DIALOG_LD_ULD_DOOR_OPEN, m_btnLdUldDoorOpen);
	DDX_Control(pDX, IDC_DIALOG_LD_ULD_DOOR_CLOSE, m_btnLdUldDoorClose);
	DDX_Control(pDX, IDC_DIALOG_TEST_DOOR_OPEN, m_btnTestDoorOpen);
	DDX_Control(pDX, IDC_DIALOG_TEST_DOOR_CLOSE, m_btnTestDoorClose);
}


BEGIN_MESSAGE_MAP(CDialog_Door_Open, CDialog)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_DIALOG_DOOR_OPEN, &CDialog_Door_Open::OnBnClickedDialogDoorOpen)
	ON_BN_CLICKED(IDC_DIALOG_DOOR_CLOSE, &CDialog_Door_Open::OnBnClickedDialogDoorClose)
	ON_BN_CLICKED(IDC_DIALOG_DOOR_EXIT, &CDialog_Door_Open::OnBnClickedDialogDoorExit)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_DIALOG_LD_ULD_DOOR_OPEN, &CDialog_Door_Open::OnBnClickedDialogLdUldDoorOpen)
	ON_BN_CLICKED(IDC_DIALOG_LD_ULD_DOOR_CLOSE, &CDialog_Door_Open::OnBnClickedDialogLdUldDoorClose)
	ON_BN_CLICKED(IDC_DIALOG_TEST_DOOR_OPEN, &CDialog_Door_Open::OnBnClickedDialogTestDoorOpen)
	ON_BN_CLICKED(IDC_DIALOG_TEST_DOOR_CLOSE, &CDialog_Door_Open::OnBnClickedDialogTestDoorClose)
	
END_MESSAGE_MAP()


// CDialog_Door_Open 메시지 처리기입니다.

BOOL CDialog_Door_Open::OnInitDialog()
{
	CDialog::OnInitDialog();

	OnInitButton();
	OnInitLabel();

	SetTimer(TIM_DOOR_OPEN, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDialog_Door_Open::OnInitGroup()
{
	m_groupLdUldDoor.SetFont(clsFunc.OnLogFont(16));
	m_groupLdUldDoor.SetCatptionTextColor(BLUE_C);
	m_groupLdUldDoor.SetBorderColor(ORANGE_C);
	m_groupLdUldDoor.SetFontBold(TRUE);
	m_groupLdUldDoor.SetBackgroundColor(WINDOW_UP);

	m_groupTestDoor.SetFont(clsFunc.OnLogFont(16));
	m_groupTestDoor.SetCatptionTextColor(BLUE_C);
	m_groupTestDoor.SetBorderColor(ORANGE_C);
	m_groupTestDoor.SetFontBold(TRUE);
	m_groupTestDoor.SetBackgroundColor(WINDOW_UP);
}
void CDialog_Door_Open::OnInitLabel()
{
	m_msgTopDoor[0].SetTextFont(clsFunc.m_pFont[1]);
	m_msgTopDoor[0].SetColor(YELLOW_D, RGB(0, 0, 0));
	m_msgTopDoor[0].SetTextAlign(DT_CENTER);

	m_msgTopDoor[1].SetTextFont(clsFunc.m_pFont[1]);
	m_msgTopDoor[1].SetColor(YELLOW_D, RGB(0, 0, 0));
	m_msgTopDoor[1].SetTextAlign(DT_CENTER);

	m_msgTopDoor[2].SetTextFont(clsFunc.m_pFont[1]);
	m_msgTopDoor[2].SetColor(YELLOW_D, RGB(0, 0, 0));
	m_msgTopDoor[2].SetTextAlign(DT_CENTER);

	m_msgTopDoor[3].SetTextFont(clsFunc.m_pFont[1]);
	m_msgTopDoor[3].SetColor(YELLOW_D, RGB(0, 0, 0));
	m_msgTopDoor[3].SetTextAlign(DT_CENTER);

	m_msgTopDoor[4].SetTextFont(clsFunc.m_pFont[1]);
	m_msgTopDoor[4].SetColor(YELLOW_D, RGB(0, 0, 0));
	m_msgTopDoor[4].SetTextAlign(DT_CENTER);

	m_msgTopDoor[5].SetTextFont(clsFunc.m_pFont[1]);
	m_msgTopDoor[5].SetColor(YELLOW_D, RGB(0, 0, 0));
	m_msgTopDoor[5].SetTextAlign(DT_CENTER);

	m_msgBottomDoor[0].SetTextFont(clsFunc.m_pFont[1]);
	m_msgBottomDoor[0].SetColor(YELLOW_D, RGB(0, 0, 0));
	m_msgBottomDoor[0].SetTextAlign(DT_CENTER);

	m_msgBottomDoor[1].SetTextFont(clsFunc.m_pFont[1]);
	m_msgBottomDoor[1].SetColor(YELLOW_D, RGB(0, 0, 0));
	m_msgBottomDoor[1].SetTextAlign(DT_CENTER);

	m_msgBottomDoor[2].SetTextFont(clsFunc.m_pFont[1]);
	m_msgBottomDoor[2].SetColor(YELLOW_D, RGB(0, 0, 0));
	m_msgBottomDoor[2].SetTextAlign(DT_CENTER);

	m_msgBottomDoor[3].SetTextFont(clsFunc.m_pFont[1]);
	m_msgBottomDoor[3].SetColor(YELLOW_D, RGB(0, 0, 0));
	m_msgBottomDoor[3].SetTextAlign(DT_CENTER);

	m_msgBottomDoor[4].SetTextFont(clsFunc.m_pFont[1]);
	m_msgBottomDoor[4].SetColor(YELLOW_D, RGB(0, 0, 0));
	m_msgBottomDoor[4].SetTextAlign(DT_CENTER);
}


void CDialog_Door_Open::OnInitButton()
{
	m_btnLdUldDoorOpen.SetBitmaps(IDC_DIALOG_LD_ULD_DOOR_OPEN, IDB_BITMAP_DOOR_OPEN, WINDOW_DN, IDB_BITMAP_DOOR_OPEN, WINDOW_UP);
	m_btnLdUldDoorOpen.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnLdUldDoorOpen.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnLdUldDoorOpen.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnLdUldDoorOpen.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnLdUldDoorOpen.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnLdUldDoorOpen.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnLdUldDoorOpen.SetFont(clsFunc.m_pFont[3]);
	m_btnLdUldDoorOpen.SetTooltipText(_T("Door Open"));

	m_btnLdUldDoorClose.SetBitmaps(IDC_DIALOG_LD_ULD_DOOR_CLOSE, IDB_BITMAP_DOOR_CLOSE, WINDOW_DN, IDB_BITMAP_DOOR_CLOSE, WINDOW_UP);
	m_btnLdUldDoorClose.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnLdUldDoorClose.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnLdUldDoorClose.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnLdUldDoorClose.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnLdUldDoorClose.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnLdUldDoorClose.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnLdUldDoorClose.SetFont(clsFunc.m_pFont[3]);
	m_btnLdUldDoorClose.SetTooltipText(_T("Door Close"));

	m_btnTestDoorOpen.SetBitmaps(IDC_DIALOG_TEST_DOOR_OPEN, IDB_BITMAP_DOOR_OPEN, WINDOW_DN, IDB_BITMAP_DOOR_OPEN, WINDOW_UP);
	m_btnTestDoorOpen.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnTestDoorOpen.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnTestDoorOpen.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnTestDoorOpen.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnTestDoorOpen.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnTestDoorOpen.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnTestDoorOpen.SetFont(clsFunc.m_pFont[3]);
	m_btnTestDoorOpen.SetTooltipText(_T("Door Open"));

	m_btnTestDoorClose.SetBitmaps(IDC_DIALOG_TEST_DOOR_CLOSE, IDB_BITMAP_DOOR_CLOSE, WINDOW_DN, IDB_BITMAP_DOOR_CLOSE, WINDOW_UP);
	m_btnTestDoorClose.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnTestDoorClose.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnTestDoorClose.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnTestDoorClose.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnTestDoorClose.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnTestDoorClose.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnTestDoorClose.SetFont(clsFunc.m_pFont[3]);
	m_btnTestDoorClose.SetTooltipText(_T("Door Close"));


	m_btnDoorExit.SetBitmaps(IDC_DIALOG_DOOR_EXIT, IDB_BITMAP_EXIT, WINDOW_DN, IDB_BITMAP_EXIT, WINDOW_UP);
	m_btnDoorExit.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnDoorExit.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnDoorExit.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnDoorExit.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnDoorExit.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnDoorExit.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnDoorExit.SetFont(clsFunc.m_pFont[3]);
	m_btnDoorExit.SetTooltipText(_T("Door Exit"));
}

void CDialog_Door_Open::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
		case TIM_DOOR_OPEN:
			OnDoorOpenCheck();									
		break;
	}
	CDialog::OnTimer(nIDEvent);
}

void CDialog_Door_Open::OnDoorOpenCheck()
{
/*	int nRet = 0;

	nRet = FAS_IO.get_in_bit_wait(0, st_io_info.iTopDoor1Chk, IO_OFF, 100);

	if (nRet == IO_OFF)
	{
		m_msgTopDoor[0].SetColor(YELLOW_D, RGB(0, 0, 0));
	}
	else
	{
		m_msgTopDoor[0].SetColor(YELLOW_C, RGB(0, 0, 0));
	}

	nRet = FAS_IO.get_in_bit_wait(0, st_io_info.iTopDoor2Chk, IO_OFF, 100);

	if (nRet == IO_OFF)
	{
		m_msgTopDoor[1].SetColor(YELLOW_D, RGB(0, 0, 0));
	}
	else
	{
		m_msgTopDoor[1].SetColor(YELLOW_C, RGB(0, 0, 0));
	}

	nRet = FAS_IO.get_in_bit_wait(0, st_io_info.iTopDoor3Chk, IO_OFF, 100);

	if (nRet == IO_OFF)
	{
		m_msgTopDoor[2].SetColor(YELLOW_D, RGB(0, 0, 0));
	}
	else
	{
		m_msgTopDoor[2].SetColor(YELLOW_C, RGB(0, 0, 0));
	}

	nRet = FAS_IO.get_in_bit_wait(0, st_io_info.iTopDoor4Chk, IO_OFF, 100);

	if (nRet == IO_OFF)
	{
		m_msgTopDoor[3].SetColor(YELLOW_D, RGB(0, 0, 0));
	}
	else
	{
		m_msgTopDoor[3].SetColor(YELLOW_C, RGB(0, 0, 0));
	}

	nRet = FAS_IO.get_in_bit_wait(0, st_io_info.iTopDoor5Chk, IO_OFF, 100);

	if (nRet == IO_OFF)
	{
		m_msgTopDoor[4].SetColor(YELLOW_D, RGB(0, 0, 0));
	}
	else
	{
		m_msgTopDoor[4].SetColor(YELLOW_C, RGB(0, 0, 0));
	}

	nRet = FAS_IO.get_in_bit_wait(0, st_io_info.iTopDoor6Chk, IO_OFF, 100);

	if (nRet == IO_OFF)
	{
		m_msgTopDoor[5].SetColor(YELLOW_D, RGB(0, 0, 0));
	}
	else
	{
		m_msgTopDoor[5].SetColor(YELLOW_C, RGB(0, 0, 0));
	}

	nRet = FAS_IO.get_in_bit_wait(0, st_io_info.iBtmDoor1Chk, IO_OFF, 100);

	if (nRet == IO_OFF)
	{
		m_msgBottomDoor[0].SetColor(YELLOW_D, RGB(0, 0, 0));
	}
	else
	{
		m_msgBottomDoor[0].SetColor(YELLOW_C, RGB(0, 0, 0));
	}

	nRet = FAS_IO.get_in_bit_wait(0, st_io_info.iBtmDoor2Chk, IO_OFF, 100);

	if (nRet == IO_OFF)
	{
		m_msgBottomDoor[1].SetColor(YELLOW_D, RGB(0, 0, 0));
	}
	else
	{
		m_msgBottomDoor[1].SetColor(YELLOW_C, RGB(0, 0, 0));
	}

	nRet = FAS_IO.get_in_bit_wait(0, st_io_info.iBtmDoor3Chk, IO_OFF, 100);

	if (nRet == IO_OFF)
	{
		m_msgBottomDoor[2].SetColor(YELLOW_D, RGB(0, 0, 0));
	}
	else
	{
		m_msgBottomDoor[2].SetColor(YELLOW_C, RGB(0, 0, 0));
	}

	nRet = FAS_IO.get_in_bit_wait(0, st_io_info.iBtmDoor4Chk, IO_OFF, 100);

	if (nRet == IO_OFF)
	{
		m_msgBottomDoor[3].SetColor(YELLOW_D, RGB(0, 0, 0));
	}
	else
	{
		m_msgBottomDoor[3].SetColor(YELLOW_C, RGB(0, 0, 0));
	}

	nRet = FAS_IO.get_in_bit_wait(0, st_io_info.iBtmDoor5Chk, IO_OFF, 100);

	if (nRet == IO_OFF)
	{
		m_msgBottomDoor[4].SetColor(YELLOW_D, RGB(0, 0, 0));
	}
	else
	{
		m_msgBottomDoor[4].SetColor(YELLOW_C, RGB(0, 0, 0));
	}*/
}


void CDialog_Door_Open::OnDestroy()
{
	CDialog::OnDestroy();

	KillTimer(TIM_DOOR_OPEN);
}


void CDialog_Door_Open::OnBnClickedDialogDoorOpen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	clsFunc.OnDoorOpen();
}


void CDialog_Door_Open::OnBnClickedDialogDoorClose()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	clsFunc.OnDoorClose();
}


void CDialog_Door_Open::OnBnClickedDialogDoorExit()
{
	CDialog::OnOK();
}


BOOL CDialog_Door_Open::OnEraseBkgnd(CDC* pDC)
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


void CDialog_Door_Open::OnBnClickedDialogLdUldDoorOpen()
{
	
	clsFunc.OnLdUldDoorOpen();
}


void CDialog_Door_Open::OnBnClickedDialogLdUldDoorClose()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	clsFunc.OnLdUldDoorClose();
}


void CDialog_Door_Open::OnBnClickedDialogTestDoorOpen()
{
	clsFunc.OnTestDoorOpen();
}


void CDialog_Door_Open::OnBnClickedDialogTestDoorClose()
{
	clsFunc.OnTestDoorClose();
}


