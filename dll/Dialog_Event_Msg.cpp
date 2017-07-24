// Dialog_Event_Msg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "Dialog_Event_Msg.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "Ctlbd_Variable.h"
#include "FastechPublic_IO.h"
#include "PublicFunction.h"

// CDialog_Event_Msg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialog_Event_Msg, CDialog)

CDialog_Event_Msg::CDialog_Event_Msg(CWnd* pParent /*=NULL*/)
	: CDialog(CDialog_Event_Msg::IDD, pParent)
{

}

CDialog_Event_Msg::~CDialog_Event_Msg()
{
}

void CDialog_Event_Msg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_EVENT_MSG, m_label_event_msg);
}


BEGIN_MESSAGE_MAP(CDialog_Event_Msg, CDialog)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDOK, &CDialog_Event_Msg::OnBnClickedOk)
END_MESSAGE_MAP()


// CDialog_Event_Msg 메시지 처리기입니다.

BOOL CDialog_Event_Msg::Create()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CDialog::Create(IDD);
}


BOOL CDialog_Event_Msg::OnInitDialog()
{
	CDialog::OnInitDialog();

	OnEventMsg_Label_Set();
	OnEventMsg_Text_Set();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDialog_Event_Msg::OnDestroy()
{
	CDialog::OnDestroy();

	FAS_IO.set_out_bit(st_io_info.o_Buzz1OnOff, IO_OFF);
}


void CDialog_Event_Msg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialog::OnTimer(nIDEvent);
}

void CDialog_Event_Msg::OnEventMsg_Label_Set()
{
	//m_label_event_msg.SetFont(clsFunc.m_pFont[4]);
	//m_label_event_msg.SetText(_T(""));
	//m_label_event_msg.SetTextAlign(DT_CENTER | DT_VCENTER);
	//m_label_event_msg.SetColor(RGB(160, 182, 140), RGB(250, 50, 50));
	//m_label_event_msg.SetTextColor(RGB(255,255,255));
	//m_label_event_msg.SetColor(RGB(160, 182, 140);

	m_label_event_msg.SetFont(clsFunc.m_pFont[4]);
	m_label_event_msg.SetCenterText();
	m_label_event_msg.SetColor(BLUE_D);
	m_label_event_msg.SetGradientColor(BLUE_D);
	m_label_event_msg.SetTextColor(RGB(255,255,255));
	
}

void CDialog_Event_Msg::OnEventMsg_Text_Set()
{
	int i;
	CString strMsg;
	
	for (i = 0; i < 3; i++)
	{
		if (st_handler_info.mstr_event_msg[i] != _T(""))
		{
			strMsg += st_handler_info.mstr_event_msg[i];
			strMsg += _T("\r\n");
		}
	}
	
	//m_label_event_msg.SetText(strMsg);
	m_label_event_msg.SetWindowTextW(strMsg);
}

BOOL CDialog_Event_Msg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
		if (pMsg->wParam == VK_ESCAPE)
			return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}


void CDialog_Event_Msg::OnBnClickedOk()
{
	::PostMessage(st_handler_info.hWnd, WM_MAIN_EVENT, CTL_NO, 0);

	CDialog::OnOK();
}
