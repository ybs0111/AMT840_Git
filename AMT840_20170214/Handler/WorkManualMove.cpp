// WorkManualMove.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "WorkManualMove.h"
#include "afxdialogex.h"
#include "PublicFunction.h"
#include "CtlBd_Variable.h"
#include "Variable.h"
#include "ComizoaPublic.h"
#include "Dialog_KeyPad.h"
#include "AlgMemory.h"
#include "FastechPublic_IO.h"
#include "Variable.h"
#include "RunLdUldRobot.h"
#include "CtlBd_Library.h"
// CWorkManualMove 대화 상자입니다.

IMPLEMENT_DYNAMIC(CWorkManualMove, CDialogEx)

CWorkManualMove::CWorkManualMove(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWorkManualMove::IDD, pParent)
	
{
	m_nTrayX = 0;
	m_nTrayY = 0;
	m_nWorkLdBuff = 0;
	m_nWorkUldBuff = 0;
	m_nMode = 0;

	m_nTsite_Pos = 0;
	m_nTsite_Socket = 0;

	m_nRetest1_Pos = 0;
	m_nRetest2_Pos = 0;
	m_nTsiteBuffer_PickNo = 0;
	m_nTsiteBuffer_Pos = 0;

	m_nStackTray = 0;
	m_nWorkBuffer = 0;
	m_nRetest = 0;
	m_nTsiteBuffer = 0;
}

CWorkManualMove::~CWorkManualMove()
{
}

void CWorkManualMove::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CUSTOM_TEST_SITE, m_pGridTestSite);
	DDX_Control(pDX, IDC_MSG_LD_RBT_TRAY_X, m_msg_LdRbt_Tray_x);
	DDX_Control(pDX, IDC_MSG_LD_RBT_TRAY_Y, m_msg_LdRbt_Tray_y);
	DDX_Control(pDX, IDC_DGT_LD_RBT_TRAY_X, m_dgt_LdRbt_Tray_x);
	DDX_Control(pDX, IDC_DGT_LD_RBT_TRAY_Y, m_dgt_LdRbt_Tray_y);
	DDX_Control(pDX, IDC_MSG_LD_BUFFER, m_msg_LdRbt_Buff);
	DDX_Control(pDX, IDC_MSG_ULD_BUFFER, m_msg_UldRbt_Buff);
	DDX_Control(pDX, IDC_DGT_LD_BUFFER, m_dgt_LdRbt_Buff);
	DDX_Control(pDX, IDC_DGT_ULD_BUFFER, m_dgt_UldRbt_Buff);
	DDX_Control(pDX, IDC_MSG_TSITE_RBT_SITE, m_msg_TsiteRbt_Pos);
	DDX_Control(pDX, IDC_MSG_TSITE_RBT_SOCKET, m_msg_TsiteRbt_Socket);
	DDX_Control(pDX, IDC_DGT_TSITE_RBT_SITE, m_dgt_TsiteRbt_Site_Pos);
	DDX_Control(pDX, IDC_DGT_TSITE_RBT_SOCKET, m_dgt_TsiteRbt_Socket);
	DDX_Control(pDX, IDC_MSG_TSITE_RETEST_1, m_msg_Retest_1);
	DDX_Control(pDX, IDC_MSG_TSITE_RETEST_2, m_msg_Retest_2);
	DDX_Control(pDX, IDC_DGT_TSITE_RETEST_1, m_dgt_Retest_1);
	DDX_Control(pDX, IDC_DGT_TSITE_RETEST_2, m_dgt_Retest_2);
	DDX_Control(pDX, IDC_MSG_TSITE_RBT_BUFFER_PICK, m_msg_TsiteRbt_Buff_Pick);
	DDX_Control(pDX, IDC_MSG_TSITE_RBT_BUFFER_POS, m_msg_TsiteRbt_Buff_Pos);
	DDX_Control(pDX, IDC_DGT_TSITE_RBT_BUFFER_PICK, m_dgt_TsiteRbt_Buff_Pick);
	DDX_Control(pDX, IDC_DGT_TSITE_RBT_BUFFER_POS, m_dgt_TsiteRbt_Buff_Pos);

	DDX_Radio(pDX, IDC_RADIO_LD_STK,		m_nStackTray);
	DDX_Control(pDX, IDC_RADIO_LD_STK,		m_btnStackTray[0]);
	DDX_Control(pDX, IDC_RADIO_ULD_STK_1,	m_btnStackTray[1]);
	DDX_Control(pDX, IDC_RADIO_ULD_STK_2,	m_btnStackTray[2]);
	DDX_Control(pDX, IDC_RADIO_REJ_STK_1,	m_btnStackTray[3]);
	DDX_Control(pDX, IDC_RADIO_REJ_STK_2,	m_btnStackTray[4]);

	DDX_Radio(pDX, IDC_RADIO_LD_BUFFER,		m_nWorkBuffer);
	DDX_Control(pDX, IDC_RADIO_LD_BUFFER,	m_btnWorkBuffer[0]);
	DDX_Control(pDX, IDC_RADIO_ULD_BUFFER,	m_btnWorkBuffer[1]);

	DDX_Radio(pDX, IDC_RADIO_RETEST1,		m_nRetest);
	DDX_Control(pDX, IDC_RADIO_RETEST1,		m_btnRetest[0]);
	DDX_Control(pDX, IDC_RADIO_RETEST2,		m_btnRetest[1]);

	DDX_Radio(pDX, IDC_RADIO_TSITE_LD_BUFF,		m_nTsiteBuffer);
	DDX_Control(pDX, IDC_RADIO_TSITE_LD_BUFF,		m_btnTsiteBuff[0]);
	DDX_Control(pDX, IDC_RADIO_TSITE_ULD_BUFF,	m_btnTsiteBuff[1]);
	DDX_Control(pDX, IDC_BTN_WORK_RBT_TRAY_GO, m_btnTraygo);
	DDX_Control(pDX, IDC_BTN_WORK_RBT_BUF_GO, m_btnWorkBuffergo);
	DDX_Control(pDX, IDC_BTN_TSITE_RBT_BUFFER_GO, m_btnTsiteBuffergo);
	DDX_Control(pDX, IDC_BTN_RETEST_GO, m_btnTsiteRetestgo);
	DDX_Control(pDX, IDC_BTN_TSITE_RBT_TSITE_GO, m_btnTsiteSocketgo);
	DDX_Control(pDX, IDC_BTN_DIALOG_MANUAL_EXIT, m_btnMMoveExit);
}


BEGIN_MESSAGE_MAP(CWorkManualMove, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_TSITE_RBT_BUFFER_GO, &CWorkManualMove::OnBnClickedBtnTsiteRbtBufferGo)
	ON_BN_CLICKED(IDC_BTN_RETEST_GO, &CWorkManualMove::OnBnClickedBtnRetestGo)
	ON_BN_CLICKED(IDC_BTN_TSITE_RBT_TSITE_GO, &CWorkManualMove::OnBnClickedBtnTsiteRbtTsiteGo)
	ON_BN_CLICKED(IDC_BTN_WORK_RBT_TRAY_GO, &CWorkManualMove::OnBnClickedBtnWorkRbtTrayGo)
	ON_BN_CLICKED(IDC_BTN_WORK_RBT_BUF_GO, &CWorkManualMove::OnBnClickedBtnWorkRbtBufGo)
	ON_STN_CLICKED(IDC_DGT_LD_RBT_TRAY_X, &CWorkManualMove::OnStnClickedDgtLdRbtTrayX)
	ON_STN_CLICKED(IDC_DGT_LD_RBT_TRAY_Y, &CWorkManualMove::OnStnClickedDgtLdRbtTrayY)
	ON_STN_CLICKED(IDC_DGT_LD_BUFFER, &CWorkManualMove::OnStnClickedDgtLdBuffer)
	ON_STN_CLICKED(IDC_DGT_ULD_BUFFER, &CWorkManualMove::OnStnClickedDgtUldBuffer)
	ON_STN_CLICKED(IDC_DGT_TSITE_RBT_SITE, &CWorkManualMove::OnStnClickedDgtTsiteRbtSite)
	ON_STN_CLICKED(IDC_DGT_TSITE_RBT_SOCKET, &CWorkManualMove::OnStnClickedDgtTsiteRbtSocket)
	ON_STN_CLICKED(IDC_DGT_TSITE_RETEST_1, &CWorkManualMove::OnStnClickedDgtTsiteRetest)
	ON_STN_CLICKED(IDC_DGT_TSITE_RBT_BUFFER_PICK, &CWorkManualMove::OnStnClickedDgtTsiteRbtBufferPick)
	ON_STN_CLICKED(IDC_DGT_TSITE_RBT_BUFFER_POS, &CWorkManualMove::OnStnClickedDgtTsiteRbtBufferPos)
	ON_STN_CLICKED(IDC_DGT_TSITE_RETEST_2, &CWorkManualMove::OnStnClickedDgtTsiteRetest2)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_DIALOG_MANUAL_EXIT, &CWorkManualMove::OnBnClickedBtnDialogManualExit)
	ON_BN_CLICKED(IDC_RADIO_LD_STK, &CWorkManualMove::OnBnClickedRadioLdStk)
	ON_BN_CLICKED(IDC_RADIO_ULD_STK_1, &CWorkManualMove::OnBnClickedRadioUldStk1)
	ON_BN_CLICKED(IDC_RADIO_ULD_STK_2, &CWorkManualMove::OnBnClickedRadioUldStk2)
	ON_BN_CLICKED(IDC_RADIO_REJ_STK_1, &CWorkManualMove::OnBnClickedRadioRejStk1)
	ON_BN_CLICKED(IDC_RADIO_REJ_STK_2, &CWorkManualMove::OnBnClickedRadioRejStk2)
	ON_BN_CLICKED(IDC_RADIO_LD_BUFFER, &CWorkManualMove::OnBnClickedRadioLdBuffer)
	ON_BN_CLICKED(IDC_RADIO_ULD_BUFFER, &CWorkManualMove::OnBnClickedRadioUldBuffer)
	ON_BN_CLICKED(IDC_RADIO_RETEST1, &CWorkManualMove::OnBnClickedRadioRetest1)
	ON_STN_CLICKED(IDC_MSG_TSITE_RBT_SITE, &CWorkManualMove::OnStnClickedMsgTsiteRbtSite)
	ON_BN_CLICKED(IDC_RADIO_RETEST2, &CWorkManualMove::OnBnClickedRadioRetest2)
	ON_BN_CLICKED(IDC_RADIO_TSITE_LD_BUFF, &CWorkManualMove::OnBnClickedRadioTsiteLdBuff)
	ON_BN_CLICKED(IDC_RADIO_TSITE_ULD_BUFF, &CWorkManualMove::OnBnClickedRadioTsiteUldBuff)
	ON_BN_CLICKED(IDC_BTN_TSITE_RBT__TSITE_Z_PICK, &CWorkManualMove::OnBnClickedBtnTsiteRbt)
	ON_BN_CLICKED(IDC_BTN_TSITE1_OPEN_CLOSE, &CWorkManualMove::OnBnClickedBtnTsite1OpenClose)
	ON_BN_CLICKED(IDC_BTN_TSITE2_OPEN_CLOSE, &CWorkManualMove::OnBnClickedBtnTsite2OpenClose)
	ON_BN_CLICKED(IDC_BTN_TSITE3_OPEN_CLOSE, &CWorkManualMove::OnBnClickedBtnTsite3OpenClose)
	ON_BN_CLICKED(IDC_BTN_TSITE4_OPEN_CLOSE, &CWorkManualMove::OnBnClickedBtnTsite4OpenClose)
	ON_BN_CLICKED(IDC_BTN_TSITE5_OPEN_CLOSE, &CWorkManualMove::OnBnClickedBtnTsite5OpenClose)
	ON_BN_CLICKED(IDC_BTN_TSITE6_OPEN_CLOSE, &CWorkManualMove::OnBnClickedBtnTsite6OpenClose)
	ON_BN_CLICKED(IDC_BTN_TSITE7_OPEN_CLOSE, &CWorkManualMove::OnBnClickedBtnTsite7OpenClose)
	ON_BN_CLICKED(IDC_BTN_TSITE8_OPEN_CLOSE, &CWorkManualMove::OnBnClickedBtnTsite8OpenClose)
	ON_BN_CLICKED(IDC_BTN_TSITE1_UP_DN, &CWorkManualMove::OnBnClickedBtnTsite1UpDn)
	ON_BN_CLICKED(IDC_BTN_TSITE2_UP_DN, &CWorkManualMove::OnBnClickedBtnTsite2UpDn)
	ON_BN_CLICKED(IDC_BTN_TSITE3_UP_DN, &CWorkManualMove::OnBnClickedBtnTsite3UpDn)
	ON_BN_CLICKED(IDC_BTN_TSITE4_UP_DN, &CWorkManualMove::OnBnClickedBtnTsite4UpDn)
	ON_BN_CLICKED(IDC_BTN_TSITE5_UP_DN, &CWorkManualMove::OnBnClickedBtnTsite5UpDn)
	ON_BN_CLICKED(IDC_BTN_TSITE6_UP_DN, &CWorkManualMove::OnBnClickedBtnTsite6UpDn)
	ON_BN_CLICKED(IDC_BTN_TSITE7_UP_DN, &CWorkManualMove::OnBnClickedBtnTsite7UpDn)
	ON_BN_CLICKED(IDC_BTN_TSITE8_UP_DN, &CWorkManualMove::OnBnClickedBtnTsite8UpDn)
END_MESSAGE_MAP()


// CWorkManualMove 메시지 처리기입니다.
void CWorkManualMove::OnBtnEnable(bool nEnable)
{
	m_btnTraygo.EnableWindow(nEnable);
	m_btnWorkBuffergo.EnableWindow(nEnable);
	m_btnTsiteBuffergo.EnableWindow(nEnable);
	m_btnTsiteRetestgo.EnableWindow(nEnable);
	m_btnTsiteSocketgo.EnableWindow(nEnable);
	m_btnMMoveExit.EnableWindow(nEnable);
}


void CWorkManualMove::OnBnClickedBtnTsiteRbtBufferGo()
{
	if( m_nTsiteBuffer >=0 && m_nTsiteBuffer <= 1)
	{
		if( m_nTsiteBuffer == 0)//RETEST1
		{
			if( m_nTsiteBuffer_PickNo >=0 && m_nTsiteBuffer_PickNo < 4)
			{
				m_nWorkSite = THD_LD_BUFF + m_nTsiteBuffer;
				m_nTrayX = m_nTsiteBuffer_PickNo;
				OnBtnEnable(false);
				SetTimer( TM_TMOR_MOVE, 100, NULL);	
				m_nTest_Step = 0;
			}
			else
			{
				AfxMessageBox( _T(" Retest Retest1_Pos error ") );
			}
		}
		else
		{
			if( m_nTsiteBuffer_Pos >=0 && m_nTsiteBuffer_Pos < 4)
			{
				m_nWorkSite = THD_LD_BUFF + m_nTsiteBuffer;
				m_nTrayX = m_nTsiteBuffer_Pos;
				SetTimer( TM_TMOR_MOVE, 100, NULL);	
				m_nTest_Step = 0;
			}
			else
			{
				AfxMessageBox( _T(" Retest Retest2_Pos error ") );
			}
		}
	}
}

void CWorkManualMove::OnBnClickedBtnRetestGo()
{
	if( m_nRetest >=0 && m_nRetest <= 1)
	{
		if( m_nRetest == 0)//RETEST1
		{
			if( m_nRetest1_Pos >=0 && m_nRetest1_Pos < 15)
			{
				m_nWorkSite = THD_RETEST_1_BUFF + m_nRetest;
				m_nTrayX = m_nRetest1_Pos;
				OnBtnEnable(false);
				SetTimer( TM_TMOR_MOVE, 100, NULL);	
			}
			else
			{
				AfxMessageBox( _T(" Retest Retest1_Pos error ") );
			}
		}
		else//RETEST2
		{
			if( m_nRetest2_Pos >=0 && m_nRetest2_Pos < 15)
			{
				m_nWorkSite = THD_RETEST_1_BUFF + m_nRetest;
				m_nTrayX = m_nRetest2_Pos;
				SetTimer( TM_TMOR_MOVE, 100, NULL);	
			}
			else
			{
				AfxMessageBox( _T(" Retest Retest2_Pos error ") );
			}
		}
	}
	else
	{
		AfxMessageBox( _T(" Retest Pos error ") );
	}
}


void CWorkManualMove::OnBnClickedBtnTsiteRbtTsiteGo()
{
	if( m_nTsite_Pos >=0 && m_nTsite_Pos <= 7)
	{
		if( m_nTsite_Socket >=0 && m_nTsite_Socket <= 7)
		{
				m_nMode = 0;
				m_nWorkSite = THD_TESTSITE_1 + m_nTsite_Pos;

				if( m_nTsite_Socket< 4 ) m_nFirstPos = 0;
				else							m_nFirstPos = 4;
				OnBtnEnable(false);
				SetTimer( TM_TMOR_MOVE, 100, NULL);	
		}
		else
		{
			AfxMessageBox( _T(" Test Socket Pos error ") );
		}
	}
	else
	{
		AfxMessageBox( _T(" Test Site Pos error ") );
	}
}


void CWorkManualMove::OnBnClickedBtnWorkRbtTrayGo()
{
	if( m_nStackTray >=0 && m_nStackTray <= 4)
	{
		if( m_nTrayX >=0 && m_nTrayX < st_recipe_info.nTrayX)
		{
			if( m_nTrayY >=0 && m_nTrayY < st_recipe_info.nTrayY)
			{
				if	   ( m_nStackTray == 0 ) m_nWorkSite = THD_LD_TRAY_PLATE;
				else if( m_nStackTray == 1 ) m_nWorkSite = THD_ULD_1_STACKER;
				else if( m_nStackTray == 2 ) m_nWorkSite = THD_ULD_2_STACKER;
				else if( m_nStackTray == 3 ) m_nWorkSite = THD_REJECT_OUT_1_STACKER;
				else                                m_nWorkSite = THD_REJECT_OUT_2_STACKER;

				m_nMode = 0;
				m_nMove_Step = 0;
				OnBtnEnable(false);
				SetTimer( TM_MOR_MOVE, 100, NULL);
			}
			else
			{
				AfxMessageBox( _T(" Tray Y error ") );
			}
		}
		else
		{
			AfxMessageBox( _T(" Tray X error ") );
		}
	}
	else
	{
		AfxMessageBox( _T(" Stack Pos error ") );
	}
}


void CWorkManualMove::OnBnClickedBtnWorkRbtBufGo()
{
	if( m_nWorkBuffer >=0 && m_nWorkBuffer <= 1)
	{
		if( m_nWorkBuffer ==0 )
		{
			if( m_nWorkLdBuff >=0 && m_nWorkLdBuff <= 4)
			{
				m_nMode = 1;
				m_nWorkSite = THD_LD_BUFF;
				m_nTrayY = m_nWorkLdBuff;
				m_nMove_Step = 0;
				OnBtnEnable(false);
				SetTimer( TM_MOR_MOVE, 100, NULL);
			}
			else
			{
				AfxMessageBox( _T(" Load Work Buffer Pos error ") );
			}
		}
		else
		{
			if( m_nWorkUldBuff >=0 && m_nWorkUldBuff <= 4)
			{
				m_nMode = 1;
				m_nWorkSite = THD_ULD_BUFF;
				m_nTrayY = m_nWorkUldBuff;
				m_nMove_Step = 0;
				SetTimer( TM_MOR_MOVE, 100, NULL);
			}
			else
			{
				AfxMessageBox( _T(" Unload Work Buffer Pos error ") );
			}
		}
	}
	else
	{
		AfxMessageBox( _T(" Work Buffer Pos error ") );
	}
}


void CWorkManualMove::OnStnClickedDgtLdRbtTrayX()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_KeyPad dlgKeyPad;
	CString strTmp;
	int nVal;

	dlgKeyPad.m_nKeypadMode				= 0;
	dlgKeyPad.m_strKeypadLowLimit		= _T("0");
	dlgKeyPad.m_strKeypadHighLimit		= _T("5");
	dlgKeyPad.m_strKeypadVal.Format(_T("%d"), m_nTrayX);

	if (dlgKeyPad.DoModal() == IDOK)
	{
		strTmp							= dlgKeyPad.m_strNewVal;
		nVal							= _wtoi(strTmp);
		m_nTrayX					= nVal;

		m_dgt_LdRbt_Tray_x.SetVal(m_nTrayX);
	}
}


void CWorkManualMove::OnStnClickedDgtLdRbtTrayY()
{
	CDialog_KeyPad dlgKeyPad;
	CString strTmp;
	int nVal;

	dlgKeyPad.m_nKeypadMode				= 0;
	dlgKeyPad.m_strKeypadLowLimit		= _T("0");
	dlgKeyPad.m_strKeypadHighLimit		= _T("5");
	dlgKeyPad.m_strKeypadVal.Format(_T("%d"), m_nTrayY);

	if (dlgKeyPad.DoModal() == IDOK)
	{
		strTmp							= dlgKeyPad.m_strNewVal;
		nVal							= _wtoi(strTmp);
		m_nTrayY					= nVal;

		m_dgt_LdRbt_Tray_y.SetVal(m_nTrayY);
	}
}


void CWorkManualMove::OnStnClickedDgtLdBuffer()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_KeyPad dlgKeyPad;
	CString strTmp;
	int nVal;

	dlgKeyPad.m_nKeypadMode				= 0;
	dlgKeyPad.m_strKeypadLowLimit		= _T("0");
	dlgKeyPad.m_strKeypadHighLimit		= _T("4");
	dlgKeyPad.m_strKeypadVal.Format(_T("%d"), m_nWorkLdBuff);

	if (dlgKeyPad.DoModal() == IDOK)
	{
		strTmp							= dlgKeyPad.m_strNewVal;
		nVal							= _wtoi(strTmp);
		m_nWorkLdBuff					= nVal;

		m_dgt_LdRbt_Buff.SetVal(m_nWorkLdBuff);
	}
}


void CWorkManualMove::OnStnClickedDgtUldBuffer()
{
	CDialog_KeyPad dlgKeyPad;
	CString strTmp;
	int nVal;

	dlgKeyPad.m_nKeypadMode				= 0;
	dlgKeyPad.m_strKeypadLowLimit		= _T("0");
	dlgKeyPad.m_strKeypadHighLimit		= _T("4");
	dlgKeyPad.m_strKeypadVal.Format(_T("%d"), m_nWorkUldBuff);

	if (dlgKeyPad.DoModal() == IDOK)
	{
		strTmp							= dlgKeyPad.m_strNewVal;
		nVal							= _wtoi(strTmp);
		m_nWorkUldBuff					= nVal;

		m_dgt_UldRbt_Buff.SetVal(m_nWorkUldBuff);
	}
}


void CWorkManualMove::OnStnClickedDgtTsiteRbtSite()
{
	CDialog_KeyPad dlgKeyPad;
	CString strTmp;
	int nVal;

	dlgKeyPad.m_nKeypadMode				= 0;
	dlgKeyPad.m_strKeypadLowLimit		= _T("0");
	dlgKeyPad.m_strKeypadHighLimit		= _T("8");
	dlgKeyPad.m_strKeypadVal.Format(_T("%d"), m_nTsite_Pos);

	if (dlgKeyPad.DoModal() == IDOK)
	{
		strTmp							= dlgKeyPad.m_strNewVal;
		nVal							= _wtoi(strTmp);
		m_nTsite_Pos					= nVal;

		m_dgt_TsiteRbt_Site_Pos.SetVal(m_nTsite_Pos);
	}
}


void CWorkManualMove::OnStnClickedDgtTsiteRbtSocket()
{
	CDialog_KeyPad dlgKeyPad;
	CString strTmp;
	int nVal;

	dlgKeyPad.m_nKeypadMode				= 0;
	dlgKeyPad.m_strKeypadLowLimit		= _T("0");
	dlgKeyPad.m_strKeypadHighLimit		= _T("8");
	dlgKeyPad.m_strKeypadVal.Format(_T("%d"), m_nTsite_Socket);

	if (dlgKeyPad.DoModal() == IDOK)
	{
		strTmp							= dlgKeyPad.m_strNewVal;
		nVal							= _wtoi(strTmp);
		m_nTsite_Socket					= nVal;

		m_dgt_TsiteRbt_Socket.SetVal(m_nTsite_Socket);
	}
}


void CWorkManualMove::OnStnClickedDgtTsiteRetest()
{
	CDialog_KeyPad dlgKeyPad;
	CString strTmp;
	int nVal;

	dlgKeyPad.m_nKeypadMode				= 0;
	dlgKeyPad.m_strKeypadLowLimit		= _T("0");
	dlgKeyPad.m_strKeypadHighLimit		= _T("15");
	dlgKeyPad.m_strKeypadVal.Format(_T("%d"), m_nRetest1_Pos);

	if (dlgKeyPad.DoModal() == IDOK)
	{
		strTmp							= dlgKeyPad.m_strNewVal;
		nVal							= _wtoi(strTmp);
		m_nRetest1_Pos					= nVal;

		m_dgt_Retest_1.SetVal(m_nRetest1_Pos);
	}
}
void CWorkManualMove::OnStnClickedDgtTsiteRetest2()
{
	CDialog_KeyPad dlgKeyPad;
	CString strTmp;
	int nVal;

	dlgKeyPad.m_nKeypadMode				= 0;
	dlgKeyPad.m_strKeypadLowLimit		= _T("0");
	dlgKeyPad.m_strKeypadHighLimit		= _T("15");
	dlgKeyPad.m_strKeypadVal.Format(_T("%d"), m_nRetest2_Pos);

	if (dlgKeyPad.DoModal() == IDOK)
	{
		strTmp							= dlgKeyPad.m_strNewVal;
		nVal							= _wtoi(strTmp);
		m_nRetest2_Pos					= nVal;

		m_dgt_Retest_2.SetVal(m_nRetest2_Pos);
	}
}

void CWorkManualMove::OnStnClickedDgtTsiteRbtBufferPick()
{
	CDialog_KeyPad dlgKeyPad;
	CString strTmp;
	int nVal;

	dlgKeyPad.m_nKeypadMode				= 0;
	dlgKeyPad.m_strKeypadLowLimit		= _T("0");
	dlgKeyPad.m_strKeypadHighLimit		= _T("4");
	dlgKeyPad.m_strKeypadVal.Format(_T("%d"), m_nTsiteBuffer_PickNo);

	if (dlgKeyPad.DoModal() == IDOK)
	{
		strTmp							= dlgKeyPad.m_strNewVal;
		nVal							= _wtoi(strTmp);
		m_nTsiteBuffer_PickNo					= nVal;

		m_dgt_TsiteRbt_Buff_Pick.SetVal(m_nTsiteBuffer_PickNo);
	}
}


void CWorkManualMove::OnStnClickedDgtTsiteRbtBufferPos()
{
	CDialog_KeyPad dlgKeyPad;
	CString strTmp;
	int nVal;

	dlgKeyPad.m_nKeypadMode				= 0;
	dlgKeyPad.m_strKeypadLowLimit		= _T("0");
	dlgKeyPad.m_strKeypadHighLimit		= _T("4");
	dlgKeyPad.m_strKeypadVal.Format(_T("%d"), m_nTsiteBuffer_Pos);

	if (dlgKeyPad.DoModal() == IDOK)
	{
		strTmp							= dlgKeyPad.m_strNewVal;
		nVal							= _wtoi(strTmp);
		m_nTsiteBuffer_Pos					= nVal;

		m_dgt_TsiteRbt_Buff_Pos.SetVal(m_nTsiteBuffer_Pos);
	}
}


BOOL CWorkManualMove::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	OnInitLabel();
	OnInitDigit();
	OnBtnEnable(true);
	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CWorkManualMove::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	int nRet;
	switch (nIDEvent)
	{
		case TM_MOR_MOVE:			
			nRet = WorkRbt_Move( m_nMode,  m_nWorkSite);
			if( nRet == RET_GOOD )
			{
				KillTimer(TM_MOR_MOVE);
				OnBtnEnable(true);
			}
			else if(nRet == RET_ERROR)
			{
				KillTimer(TM_MOR_MOVE);
				OnBtnEnable(true);
			}
			break;

		case TM_TMOR_MOVE:
			nRet = TestRbt_Move( m_nMode,  m_nWorkSite,  m_nFirstPos);
			if( nRet == RET_GOOD )
			{
				KillTimer(TM_TMOR_MOVE);
				OnBtnEnable(true);
				m_nTest_Step = 0;
			}
			else if(nRet == RET_ERROR)
			{
				KillTimer(TM_TMOR_MOVE);
				OnBtnEnable(true);
				m_nTest_Step = 0;
			}
			break;

		case TM_TDOOR_MOVE:
			nRet = OnTestSiteDoorOpenClose();
			if( nRet == RET_GOOD )
			{
				KillTimer(TM_TDOOR_MOVE);
				OnBtnEnable(true);
			}
			else if(nRet == RET_ERROR)
			{
				KillTimer(TM_TDOOR_MOVE);
				OnBtnEnable(true);
			}
			break;

		case TM_TSITE_UPDN:
			nRet = OnTestSiteUpDn();
			if( nRet == RET_GOOD )
			{
				KillTimer(TM_TSITE_UPDN);
				OnBtnEnable(true);
			}
			else if(nRet == RET_ERROR)
			{
				KillTimer(TM_TSITE_UPDN);
				OnBtnEnable(true);
			}
			break;
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CWorkManualMove::OnInitLabel()
{
	m_msg_LdRbt_Tray_x.SetFont(clsFunc.m_pFont[2]);
	m_msg_LdRbt_Tray_x.SetWindowText(_T("X"));
	m_msg_LdRbt_Tray_x.SetCenterText();
	m_msg_LdRbt_Tray_x.SetColor(WHITE_C);
	m_msg_LdRbt_Tray_x.SetGradientColor(BLUE_C);
	m_msg_LdRbt_Tray_x.SetTextColor(BLACK_C);

	m_msg_LdRbt_Tray_y.SetFont(clsFunc.m_pFont[2]);
	m_msg_LdRbt_Tray_y.SetWindowText(_T("Y"));
	m_msg_LdRbt_Tray_y.SetCenterText();
	m_msg_LdRbt_Tray_y.SetColor(WHITE_C);
	m_msg_LdRbt_Tray_y.SetGradientColor(BLUE_C);
	m_msg_LdRbt_Tray_y.SetTextColor(BLACK_C);

	m_msg_LdRbt_Buff.SetFont(clsFunc.m_pFont[2]);
	m_msg_LdRbt_Buff.SetWindowText(_T("LD_Buff"));
	m_msg_LdRbt_Buff.SetCenterText();
	m_msg_LdRbt_Buff.SetColor(WHITE_C);
	m_msg_LdRbt_Buff.SetGradientColor(BLUE_C);
	m_msg_LdRbt_Buff.SetTextColor(BLACK_C);

	m_msg_UldRbt_Buff.SetFont(clsFunc.m_pFont[2]);
	m_msg_UldRbt_Buff.SetWindowText(_T("ULD_Buff"));
	m_msg_UldRbt_Buff.SetCenterText();
	m_msg_UldRbt_Buff.SetColor(WHITE_C);
	m_msg_UldRbt_Buff.SetGradientColor(BLUE_C);
	m_msg_UldRbt_Buff.SetTextColor(BLACK_C);

	m_msg_TsiteRbt_Pos.SetFont(clsFunc.m_pFont[2]);
	m_msg_TsiteRbt_Pos.SetWindowText(_T("Site"));
	m_msg_TsiteRbt_Pos.SetCenterText();
	m_msg_TsiteRbt_Pos.SetColor(WHITE_C);
	m_msg_TsiteRbt_Pos.SetGradientColor(BLUE_C);
	m_msg_TsiteRbt_Pos.SetTextColor(BLACK_C);

	m_msg_TsiteRbt_Socket.SetFont(clsFunc.m_pFont[2]);
	m_msg_TsiteRbt_Socket.SetWindowText(_T("Socket"));
	m_msg_TsiteRbt_Socket.SetCenterText();
	m_msg_TsiteRbt_Socket.SetColor(WHITE_C);
	m_msg_TsiteRbt_Socket.SetGradientColor(BLUE_C);
	m_msg_TsiteRbt_Socket.SetTextColor(BLACK_C);

	m_msg_Retest_1.SetFont(clsFunc.m_pFont[2]);
	m_msg_Retest_1.SetWindowText(_T("Retest_1"));
	m_msg_Retest_1.SetCenterText();
	m_msg_Retest_1.SetColor(WHITE_C);
	m_msg_Retest_1.SetGradientColor(BLUE_C);
	m_msg_Retest_1.SetTextColor(BLACK_C);

	m_msg_Retest_2.SetFont(clsFunc.m_pFont[2]);
	m_msg_Retest_2.SetWindowText(_T("Retest_2"));
	m_msg_Retest_2.SetCenterText();
	m_msg_Retest_2.SetColor(WHITE_C);
	m_msg_Retest_2.SetGradientColor(BLUE_C);
	m_msg_Retest_2.SetTextColor(BLACK_C);

	m_msg_TsiteRbt_Buff_Pick.SetFont(clsFunc.m_pFont[2]);
	m_msg_TsiteRbt_Buff_Pick.SetWindowText(_T("PickNum"));
	m_msg_TsiteRbt_Buff_Pick.SetCenterText();
	m_msg_TsiteRbt_Buff_Pick.SetColor(WHITE_C);
	m_msg_TsiteRbt_Buff_Pick.SetGradientColor(BLUE_C);
	m_msg_TsiteRbt_Buff_Pick.SetTextColor(BLACK_C);

	m_msg_TsiteRbt_Buff_Pos.SetFont(clsFunc.m_pFont[2]);
	m_msg_TsiteRbt_Buff_Pos.SetWindowText(_T("Pos"));
	m_msg_TsiteRbt_Buff_Pos.SetCenterText();
	m_msg_TsiteRbt_Buff_Pos.SetColor(WHITE_C);
	m_msg_TsiteRbt_Buff_Pos.SetGradientColor(BLUE_C);
	m_msg_TsiteRbt_Buff_Pos.SetTextColor(BLACK_C);
}

void CWorkManualMove::OnInitDigit()
{
	m_dgt_LdRbt_Tray_x.SetStyle(CDigit::DS_INT, 2, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgt_LdRbt_Tray_x.SetVal(m_nTrayX);

	m_dgt_LdRbt_Tray_y.SetStyle(CDigit::DS_INT, 2, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgt_LdRbt_Tray_y.SetVal(m_nTrayY);

	m_dgt_LdRbt_Buff.SetStyle(CDigit::DS_INT, 2, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgt_LdRbt_Buff.SetVal(m_nWorkLdBuff);

	m_dgt_UldRbt_Buff.SetStyle(CDigit::DS_INT, 2, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgt_UldRbt_Buff.SetVal(m_nWorkUldBuff);

	m_dgt_TsiteRbt_Site_Pos.SetStyle(CDigit::DS_INT, 2, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgt_TsiteRbt_Site_Pos.SetVal(m_nTsite_Pos);

	m_dgt_TsiteRbt_Socket.SetStyle(CDigit::DS_INT, 2, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgt_TsiteRbt_Socket.SetVal(m_nTsite_Socket);

	m_dgt_Retest_1.SetStyle(CDigit::DS_INT, 2, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgt_Retest_1.SetVal(m_nRetest1_Pos);

	m_dgt_Retest_2.SetStyle(CDigit::DS_INT, 2, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgt_Retest_2.SetVal(m_nRetest2_Pos);

	m_dgt_TsiteRbt_Buff_Pick.SetStyle(CDigit::DS_INT, 2, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgt_TsiteRbt_Buff_Pick.SetVal(m_nTsiteBuffer_PickNo);

	m_dgt_TsiteRbt_Buff_Pos.SetStyle(CDigit::DS_INT, 2, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgt_TsiteRbt_Buff_Pos.SetVal(m_nTsiteBuffer_Pos);

	
}




void CWorkManualMove::OnBnClickedBtnDialogManualExit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnOK();
}


void CWorkManualMove::OnBnClickedRadioLdStk()
{
	if(m_nStackTray == 0) return;
	m_nStackTray = 0;
	OnChange_TrayType();
}

void CWorkManualMove::OnBnClickedRadioUldStk1()
{
	if(m_nStackTray == 1) return;
	m_nStackTray = 1;
	OnChange_TrayType();
}

void CWorkManualMove::OnBnClickedRadioUldStk2()
{
	if(m_nStackTray == 2) return;
	m_nStackTray = 2;
	OnChange_TrayType();
}

void CWorkManualMove::OnBnClickedRadioRejStk1()
{
	if(m_nStackTray == 3) return;
	m_nStackTray = 3;
	OnChange_TrayType();
}

void CWorkManualMove::OnBnClickedRadioRejStk2()
{
	if(m_nStackTray == 4) return;
	m_nStackTray = 4;
	OnChange_TrayType();
}

void CWorkManualMove::OnChange_TrayType()
{
	if (m_nStackTray < 0 || m_nStackTray > 4)
	{
		m_nStackTray = 0;
	}
	
	switch (m_nStackTray)
	{
	case 0:
		m_btnStackTray[0].SetCheck(TRUE);
		m_btnStackTray[1].SetCheck(FALSE);
		m_btnStackTray[2].SetCheck(FALSE);
		m_btnStackTray[3].SetCheck(FALSE);
		m_btnStackTray[4].SetCheck(FALSE);
		break;

	case 1:
		m_btnStackTray[0].SetCheck(FALSE);
		m_btnStackTray[1].SetCheck(TRUE);
		m_btnStackTray[2].SetCheck(FALSE);
		m_btnStackTray[3].SetCheck(FALSE);
		m_btnStackTray[4].SetCheck(FALSE);
		break;

	case 2:
		m_btnStackTray[0].SetCheck(FALSE);
		m_btnStackTray[1].SetCheck(FALSE);
		m_btnStackTray[2].SetCheck(TRUE);
		m_btnStackTray[3].SetCheck(FALSE);
		m_btnStackTray[4].SetCheck(FALSE);
		break;
		
	case 3:
		m_btnStackTray[0].SetCheck(FALSE);
		m_btnStackTray[1].SetCheck(FALSE);
		m_btnStackTray[2].SetCheck(FALSE);
		m_btnStackTray[3].SetCheck(TRUE);
		m_btnStackTray[4].SetCheck(FALSE);
		break;

	case 4:
		m_btnStackTray[0].SetCheck(FALSE);
		m_btnStackTray[1].SetCheck(FALSE);
		m_btnStackTray[2].SetCheck(FALSE);
		m_btnStackTray[3].SetCheck(FALSE);
		m_btnStackTray[4].SetCheck(TRUE);
		break;
	}
}

void CWorkManualMove::OnBnClickedRadioLdBuffer()
{
	if(m_nWorkBuffer == 0) return;
	m_nWorkBuffer = 0;
	OnChange_WorkBufferType();
}
void CWorkManualMove::OnBnClickedRadioUldBuffer()
{
	if(m_nWorkBuffer == 1) return;
	m_nWorkBuffer = 1;
	OnChange_WorkBufferType();
}

void CWorkManualMove::OnChange_WorkBufferType()
{
	if (m_nWorkBuffer < 0 || m_nWorkBuffer > 1)
	{
		m_nWorkBuffer = 0;
	}
	
	switch (m_nWorkBuffer)
	{
	case 0:
		m_btnWorkBuffer[0].SetCheck(TRUE);
		m_btnWorkBuffer[0].SetCheck(FALSE);
		break;
	case 1:
		m_btnWorkBuffer[1].SetCheck(FALSE);
		m_btnWorkBuffer[1].SetCheck(TRUE);
		break;
	}

}



void CWorkManualMove::OnBnClickedRadioRetest1()
{
	if(m_nRetest == 0) return;
	m_nRetest = 0;
	OnChange_RetestBufferType();
}


void CWorkManualMove::OnBnClickedRadioRetest2()
{
	if(m_nRetest == 1) return;
	m_nRetest = 1;
	OnChange_RetestBufferType();
}

void CWorkManualMove::OnChange_RetestBufferType()
{
	if (m_nRetest < 0 || m_nRetest > 1)
	{
		m_nRetest = 0;
	}
	
	switch (m_nRetest)
	{
	case 0:
		m_btnRetest[0].SetCheck(TRUE);
		m_btnRetest[0].SetCheck(FALSE);
		break;
	case 1:
		m_btnRetest[1].SetCheck(FALSE);
		m_btnRetest[1].SetCheck(TRUE);
		break;
	}

}


void CWorkManualMove::OnBnClickedRadioTsiteLdBuff()
{	
	if(m_nTsiteBuffer == 0) return;
	m_nTsiteBuffer = 0;
	OnChange_TestBufferType();
}


void CWorkManualMove::OnBnClickedRadioTsiteUldBuff()
{
	if(m_nTsiteBuffer == 1) return;
	m_nTsiteBuffer = 1;
	OnChange_TestBufferType();
}

void CWorkManualMove::OnChange_TestBufferType()
{
	if (m_nTsiteBuffer < 0 || m_nTsiteBuffer > 1)
	{
		m_nTsiteBuffer = 0;
	}
	
	switch (m_nTsiteBuffer)
	{
	case 0:
		m_btnTsiteBuff[0].SetCheck(TRUE);
		m_btnTsiteBuff[0].SetCheck(FALSE);
		break;
	case 1:
		m_btnTsiteBuff[1].SetCheck(FALSE);
		m_btnTsiteBuff[1].SetCheck(TRUE);
		break;
	}
}

int CWorkManualMove::WorkRbt_Move(int nMode, int nWork_Site)
{
	int nFuncRet = RET_PROCEED;
	int nRet_1, nRet_2;
//	int x, y;
	//if(nWork_Site == THD_LD_BUFF,THD_ULD_1_STACKER,THD_ULD_2_STACKER,THD_REJECT_OUT_1_STACKER,THD_REJECT_OUT_2_STACKER

	m_nLinearMove_Index	= 0;   //현재 map index
	m_lAxisCnt	= 2;            //현재의 IndexNum에서 사용 가능한 모터 수량 최대 4개
	m_lpAxisNum[0] = M_WORK_RBT_Y; //m_lpAxisNum[4];        //현재의 IndexNum에서 사용하는 모터 실재 번호를 가진다
	m_lpAxisNum[1] = M_WORK_RBT_X;
	m_lpAxisNum[2] = M_WORK_RBT_Z;

	switch(m_nMove_Step)
	{
	case 0:
		m_nMove_Step = 100;
		break;

	case 100:	 
		clsRunLdUldRobot.Set_WorkRbtPicker_UpDown(IO_OFF); //미리 피커를 강제로 올린다

		m_dCurrentPos[M_WORK_RBT_Z] = COMI.Get_MotCurrentPos(M_WORK_RBT_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[M_WORK_RBT_Z] > st_motor_info[M_WORK_RBT_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[M_WORK_RBT_Z].d_allow) //안전 위치를 벗어나 있으면 저위치 시킨다
		{
			m_nMove_Step = 900;
		}
		else
		{
			m_nMove_Step = 1000;
		}
		break;

	case 900:
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, M_WORK_RBT_Z, st_motor_info[M_WORK_RBT_Z].d_pos[P_WORKROBOT_Z_SAFETY], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_nMove_Step = 1000;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nMove_Step = 900;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);
			clsMem.OnAbNormalMessagWrite(st_alarm_info.strCode);
			nFuncRet = RET_ERROR;
		}
		break;

	////////////////////////////////////////////////////////////////
	//피커 상태를 확인 후 문제가 없으면 작업할 피커를 찾는다
	////////////////////////////////////////////////////////////////
	case 1000:
		m_dCurrentPos[2] = COMI.Get_MotCurrentPos(M_WORK_RBT_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[2] > st_motor_info[M_WORK_RBT_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[M_WORK_RBT_Z].d_allow) //안전 위치를 벅어나 있으면 저위치 시킨다
		{
			m_nMove_Step = 900;
			break;
		}
		nRet_1 = clsRunLdUldRobot.Chk_WorkRbtPicker_UpDown(IO_OFF); 
		if(nRet_1 == RET_GOOD)
		{
			if( m_nMode == 0 )
			{
				//실제 작업 위치를 셋팅
				m_npFindWorkPosYXCPB[0] = m_nTrayY;				
				m_npFindWorkPosYXCPB[1] = m_nTrayX;
			}
			else if( m_nMode == 1 )
			{
				//실제 작업 위치를 셋팅
				m_npFindWorkPosYXCPB[0] = m_nTrayY; 
				m_npFindWorkPosYXCPB[1] = m_nTrayX;
				m_npFindWorkPosYXCPB[3]= 0;
			}

			m_nMove_Step = 2000;
		}
		else if(nRet_1 == RET_ERROR)
		{
			CString m_strAlarmCode;
			m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_LdUldPickDnChk); 
			CTL_Lib.Alarm_Error_Occurrence(m_nMove_Step, dWARNING, m_strAlarmCode);
			clsMem.OnAbNormalMessagWrite(m_strAlarmCode);
			nFuncRet = RET_ERROR;
			m_nMove_Step = 100;
		}
		break;

	////////////////////////////////////////////////////////
	// 작업 할 위치로 X,Y robot 및 rotator 실린더 동작한다
	////////////////////////////////////////////////////////
	case 2000:
		m_dCurrentPos[2] = COMI.Get_MotCurrentPos(M_WORK_RBT_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[2] > st_motor_info[M_WORK_RBT_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[M_WORK_RBT_Z].d_allow) //안전 위치를 벅어나 있으면 저위치 시킨다
		{
			m_nMove_Step = 900;
			break;
		}

		nRet_1 = clsFunc.Calculate_MovePos_Find(0, M_WORK_RBT_X, nWork_Site, 0, m_npFindWorkPosYXCPB, &m_dTemp_CalTargetPos_X);
		nRet_2 = clsFunc.Calculate_MovePos_Find(0, M_WORK_RBT_Y, nWork_Site, 0, m_npFindWorkPosYXCPB, &m_dTemp_CalTargetPos_Y);             //Calculate_MovePos_Find(int nMotNum, int nSite, int nWorkPart, int *npFirstYXP_Pos, double *dpGetTargetPos)
		if(nRet_1 == RET_GOOD && nRet_2 == RET_GOOD)
		{//ok
			m_dpTargetPosList[1] = m_dTemp_CalTargetPos_X;
			m_dpTargetPosList[0] = m_dTemp_CalTargetPos_Y;
		}
		else
		{
			CTL_Lib.Alarm_Error_Occurrence(m_nMove_Step, dWARNING, clsFunc.m_strAlarmCode);
			clsMem.OnAbNormalMessagWrite(clsFunc.m_strAlarmCode);
			nFuncRet = RET_ERROR;
			break;
		}

		nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate); 
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_nMove_Step = 0;
			nFuncRet = RET_GOOD;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nMove_Step = 2000;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);
			clsMem.OnAbNormalMessagWrite(st_alarm_info.strCode);
			nFuncRet = RET_ERROR;
		}
		break; 	
	}

	return nFuncRet;
}

int CWorkManualMove::TestRbt_Move(int nMode, int nWork_Site, int nStart_FixPos)
{
	int nFuncRet = RET_PROCEED;
	int nRet_1, nRet_2;	
	int nFind_JobPos =0; //작업할 위치를 찾는다

	m_nLinearMove_Index	= 3;   //현재 map index
	m_lAxisCnt	= 2;            //현재의 IndexNum에서 사용 가능한 모터 수량 최대 4개
	m_lpAxisNum[0] = M_TEST_RBT_X; //m_lpAxisNum[4];        //현재의 IndexNum에서 사용하는 모터 실재 번호를 가진다
	m_lpAxisNum[1] = M_TEST_RBT_Y;
	m_lpAxisNum[2] = M_TEST_RBT_Z;
	m_lpAxisNum[3] = M_TEST_DVC_EJECT_Z;	

	switch(m_nTest_Step)
	{
	case 0:	
		m_nTest_Step = 100;
		break;

	case 100:
		m_nTest_Step = 900;
		break;

	case 900:
		m_dCurrentPos[M_TEST_RBT_Z] = COMI.Get_MotCurrentPos(M_TEST_RBT_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[M_TEST_RBT_Z] > st_motor_info[M_TEST_RBT_Z].d_pos[P_TESTRBT_Z_SAFETY] + st_motor_info[M_TEST_RBT_Z].d_allow) //안전 위치를 벗어나 있으면 저위치 시킨다
		{
			m_nTest_Step = 910;
		}
		else
		{
			m_nTest_Step = 920;
		}
		break;

	case 910:
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, M_TEST_RBT_Z, st_motor_info[M_TEST_RBT_Z].d_pos[P_TESTRBT_Z_SAFETY], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_nTest_Step = 920;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nTest_Step = 900;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(m_nTest_Step, dWARNING, st_alarm_info.strCode);
			clsMem.OnAbNormalMessagWrite(st_alarm_info.strCode);
			nFuncRet = RET_ERROR;
		}
		break;

	case 920:
		m_dCurrentPos[M_TEST_DVC_EJECT_Z] = COMI.Get_MotCurrentPos(M_TEST_DVC_EJECT_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[M_TEST_DVC_EJECT_Z] > st_motor_info[M_TEST_DVC_EJECT_Z].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[M_TEST_RBT_Z].d_allow) //안전 위치를 벗어나 있으면 저위치 시킨다
		{
			m_nTest_Step = 930;
		}
		else
		{
			m_nTest_Step = 940;
		}
		break;

	case 930:
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, M_TEST_DVC_EJECT_Z, st_motor_info[M_TEST_DVC_EJECT_Z].d_pos[P_DVC_EJECTOR_Z_SAFETY], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_nTest_Step = 940;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nTest_Step = 930;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(m_nTest_Step, dWARNING, st_alarm_info.strCode);
			clsMem.OnAbNormalMessagWrite(st_alarm_info.strCode);
			nFuncRet = RET_ERROR;
		}
		break;

	case 940:
		m_nTest_Step = 1000;
		break; 

	///////////////////////////////////////////////////////////////////
	// 작업 조건을 확인한다 
	///////////////////////////////////////////////////////////////////
	case 1000:
		m_dCurrentPos[M_TEST_RBT_Z] = COMI.Get_MotCurrentPos(M_TEST_RBT_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[M_TEST_RBT_Z] > st_motor_info[M_TEST_RBT_Z].d_pos[P_TESTRBT_Z_SAFETY] + st_motor_info[M_TEST_RBT_Z].d_allow) //안전 위치를 벗어나 있으면 정위치 시킨다
		{
			m_nTest_Step = 900;
			break;
		}
		m_dCurrentPos[M_TEST_DVC_EJECT_Z] = COMI.Get_MotCurrentPos(M_TEST_DVC_EJECT_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[M_TEST_DVC_EJECT_Z] > st_motor_info[M_TEST_DVC_EJECT_Z].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[M_TEST_DVC_EJECT_Z].d_allow) //안전 위치를 벗어나 있으면 정위치 시킨다
		{
			m_nTest_Step = 900;
			break;
		}

		if(nWork_Site == THD_RETEST_1_BUFF || nWork_Site == THD_RETEST_2_BUFF)
		{
			m_nFirstTray_Y_Num = 0; //사용하지 않음 m_npFindWorkPosYXCPB[0];
			m_nFirstTray_X_Num = m_nTrayX;
		}
		else if(nWork_Site == THD_LD_BUFF || nWork_Site == THD_ULD_BUFF) 
		{
			m_nFirstTray_Y_Num = 0;//사용
			m_nFirstTray_X_Num = 0;
		}
		else if(nWork_Site >= THD_TESTSITE_1 && nWork_Site <= THD_TESTSITE_8)
		{
			//사용하지 않음 m_nFirstTray_Y_Num = st_test_socket_info[st_sync_info.nTestSite_Target_THD_Work_Site].nWorkSocket_StartPos; 
			m_nFirstTray_X_Num = nStart_FixPos; //물리적 으로 정한 위치로, 0 또는 4 임  st_test_socket_info[st_sync_info.nTestSite_Target_THD_Work_Site].nWorkSocket_StartPos;
		}
		m_nTest_Step = 2000;		
		break;

	case 2000:
		m_dCurrentPos[2] = COMI.Get_MotCurrentPos(M_TEST_RBT_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[2] > st_motor_info[M_TEST_RBT_Z].d_pos[P_WORKROBOT_Z_SAFETY] + st_motor_info[M_TEST_RBT_Z].d_allow) //안전 위치를 벅어나 있으면 저위치 시킨다
		{
			m_nTest_Step = 900;
			break;
		}
		m_dCurrentPos[M_TEST_DVC_EJECT_Z] = COMI.Get_MotCurrentPos(M_TEST_DVC_EJECT_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos[M_TEST_DVC_EJECT_Z] > st_motor_info[M_TEST_DVC_EJECT_Z].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[M_TEST_RBT_Z].d_allow) //안전 위치를 벗어나 있으면 저위치 시킨다
		{
			m_nTest_Step = 900;
			break;
		}


		nRet_1 = clsFunc.Calculate_MovePos_Find(0, M_TEST_RBT_Y, nWork_Site, 0, m_npFindWorkPosYXCPB, &m_dTemp_CalTargetPos_Y);             //Calculate_MovePos_Find(int nMotNum, int nSite, int nWorkPart, int *npFirstYXP_Pos, double *dpGetTargetPos)
		nRet_2 = clsFunc.Calculate_MovePos_Find(0, M_TEST_RBT_X, nWork_Site, 0, m_npFindWorkPosYXCPB, &m_dTemp_CalTargetPos_X);
		if(nRet_1 == RET_GOOD && nRet_2 == RET_GOOD)
		{//ok
			m_dpTargetPosList[1] = m_dTemp_CalTargetPos_Y;
			m_dpTargetPosList[0] = m_dTemp_CalTargetPos_X;
		}
		else
		{
			CTL_Lib.Alarm_Error_Occurrence(m_nTest_Step, dWARNING, clsFunc.m_strAlarmCode);
			clsMem.OnAbNormalMessagWrite(clsFunc.m_strAlarmCode);
			nFuncRet = RET_ERROR;
			break;
		}
		
		nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate); 
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			nFuncRet = RET_GOOD;
			m_nTest_Step = 0;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nTest_Step = 2000;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);
			clsMem.OnAbNormalMessagWrite(st_alarm_info.strCode);
			nFuncRet = RET_ERROR;
		}
		break; 	

	default : 
		break;
		
	}

	return nFuncRet;

}



void CWorkManualMove::OnStnClickedMsgTsiteRbtSite()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CWorkManualMove::OnBnClickedBtnTsiteRbt()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CWorkManualMove::OnBnClickedBtnTsite1OpenClose()
{
	m_nDoorTestSite = 0;
	if( m_nOpenClose[m_nDoorTestSite] == 0)  m_nOpenClose[m_nDoorTestSite] = 1;//OPEN
	else														m_nOpenClose[m_nDoorTestSite] = 0;//CLOSE

	OnBtnEnable(false);
	m_nTDoorStep = 0;
	SetTimer( TM_TDOOR_MOVE, 100, NULL);	
}


void CWorkManualMove::OnBnClickedBtnTsite2OpenClose()
{
	m_nDoorTestSite = 1;
	if( m_nOpenClose[m_nDoorTestSite] == 0)  m_nOpenClose[m_nDoorTestSite] = 1;//OPEN
	else														m_nOpenClose[m_nDoorTestSite] = 0;//CLOSE

	OnBtnEnable(false);
	m_nTDoorStep = 0;
	SetTimer( TM_TDOOR_MOVE, 100, NULL);
}


void CWorkManualMove::OnBnClickedBtnTsite3OpenClose()
{
	m_nDoorTestSite = 2;
	if( m_nOpenClose[m_nDoorTestSite] == 0)  m_nOpenClose[m_nDoorTestSite] = 1;//OPEN
	else														m_nOpenClose[m_nDoorTestSite] = 0;//CLOSE

	OnBtnEnable(false);
	m_nTDoorStep = 0;
	SetTimer( TM_TDOOR_MOVE, 100, NULL);
}


void CWorkManualMove::OnBnClickedBtnTsite4OpenClose()
{
	m_nDoorTestSite = 3;
	if( m_nOpenClose[m_nDoorTestSite] == 0)  m_nOpenClose[m_nDoorTestSite] = 1;//OPEN
	else														m_nOpenClose[m_nDoorTestSite] = 0;//CLOSE

	OnBtnEnable(false);
	m_nTDoorStep = 0;
	SetTimer( TM_TDOOR_MOVE, 100, NULL);
}


void CWorkManualMove::OnBnClickedBtnTsite5OpenClose()
{
	m_nDoorTestSite = 4;
	if( m_nOpenClose[m_nDoorTestSite] == 0)  m_nOpenClose[m_nDoorTestSite] = 1;//OPEN
	else														m_nOpenClose[m_nDoorTestSite] = 0;//CLOSE

	OnBtnEnable(false);
	m_nTDoorStep = 0;
	SetTimer( TM_TDOOR_MOVE, 100, NULL);
}


void CWorkManualMove::OnBnClickedBtnTsite6OpenClose()
{
	m_nDoorTestSite = 5;
	if( m_nOpenClose[m_nDoorTestSite] == 0)  m_nOpenClose[m_nDoorTestSite] = 1;//OPEN
	else														m_nOpenClose[m_nDoorTestSite] = 0;//CLOSE

	OnBtnEnable(false);
	m_nTDoorStep = 0;
	SetTimer( TM_TDOOR_MOVE, 100, NULL);
}


void CWorkManualMove::OnBnClickedBtnTsite7OpenClose()
{
	m_nDoorTestSite = 6;
	if( m_nOpenClose[m_nDoorTestSite] == 0)  m_nOpenClose[m_nDoorTestSite] = 1;//OPEN
	else														m_nOpenClose[m_nDoorTestSite] = 0;//CLOSE

	OnBtnEnable(false);
	m_nTDoorStep = 0;
	SetTimer( TM_TDOOR_MOVE, 100, NULL);
}


void CWorkManualMove::OnBnClickedBtnTsite8OpenClose()
{
	m_nDoorTestSite = 7;
	if( m_nOpenClose[m_nDoorTestSite] == 0)  m_nOpenClose[m_nDoorTestSite] = 1;//OPEN
	else														m_nOpenClose[m_nDoorTestSite] = 0;//CLOSE

	OnBtnEnable(false);
	m_nTDoorStep = 0;
	SetTimer( TM_TDOOR_MOVE, 100, NULL);
}

int CWorkManualMove::OnTestSiteDoorOpenClose()
{
	int nRet_1, nRet_2=0, nRet_3=0;
	int nCount = 0, nBin = 0, nFlag = 0, nTemp =0;
	int nFuncRet = RET_PROCEED;

	switch (m_nTDoorStep)
	{ 
		case 0:			

			m_nTDoorStep = 100;
			break;

		case 100:// 
			if( FAS_IO.get_in_bit(st_io_info.i_TsiteUpChk[m_nDoorTestSite],	IO_OFF)	== IO_OFF &&
				FAS_IO.get_in_bit(st_io_info.i_TsiteDnChk[m_nDoorTestSite],	IO_ON)	== IO_ON)
			{
				m_nTDoorStep = 1000;
			}
			else
			{
				m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_TsiteUpChk[m_nDoorTestSite]);
				clsMem.OnAbNormalMessagWrite(m_strAlarmCode);
				nFuncRet = RET_ERROR;
			}
			break; 

		case 1000:
			if(st_test_site_info[m_nDoorTestSite].nTester_Req[0] == CTL_REQ)
			{
				if(st_test_site_info[m_nDoorTestSite].nTester_Req[1] == WORK_PLACE) //자재를 소켓에 놓았다//인서트 했다, 이제 테스트를 해야 하니 챔버를 close 한다   
				{
					st_sync_info.nTestSiteChamber_AcMotorCtl_Req[m_nDoorTestSite][0] = CTL_REQ;
					st_sync_info.nTestSiteChamber_AcMotorCtl_Req[m_nDoorTestSite][1] = WORK_PLACE;
					m_bWaitChk_Falg = false;
					m_nTDoorStep = 2000;
				}
				else if(st_test_site_info[m_nDoorTestSite].nTester_Req[1] == WORK_PICK) //테스트가 끝나 챔버를 open해야 한다  
				{
					st_sync_info.nTestSiteChamber_AcMotorCtl_Req[m_nDoorTestSite][0] = CTL_REQ;
					st_sync_info.nTestSiteChamber_AcMotorCtl_Req[m_nDoorTestSite][1] = WORK_PICK;

					m_bWaitChk_Falg = false;
					m_nTDoorStep = 2000;
				}
			}
			break;

		case 2000:
			if(st_test_site_info[m_nDoorTestSite].nTester_Req[0] == CTL_READY)
			{
				if(st_test_site_info[m_nDoorTestSite].nTester_Req[1] == WORK_PICK) //open chamber, 자재 test site 에서 집기전 실행 
				{
					nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ChamberOpenChk[m_nDoorTestSite],	IO_ON);  
					nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ChamberCloseChk[m_nDoorTestSite],	IO_OFF);  
					if(nRet_1 == IO_ON && nRet_1 == IO_OFF )
					{
						m_nTDoorStep = 3000;
					}
					else
					{	// 알람은 로보트에서 체크하여 처리 하자, 이곳에서 하면 안됨 
						m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ChamberOpenChk[m_nDoorTestSite]); 
						CTL_Lib.Alarm_Error_Occurrence(m_nTDoorStep, dWARNING, m_strAlarmCode);
						nFuncRet = RET_ERROR;

					}
				}
				else if(st_test_site_info[m_nDoorTestSite].nTester_Req[1] == WORK_PLACE) //close chamber, 자재 test site에 insert 후 진행
				{
					nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ChamberOpenChk[m_nDoorTestSite],	IO_OFF);  
					nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ChamberCloseChk[m_nDoorTestSite],	IO_ON);  
					if(nRet_1 == IO_ON && nRet_1 == IO_OFF )
					{
						  m_nTDoorStep = 3000;
					}
					else
					{// 알람은 로보트에서 체크하여 처리 하자, 이곳에서 하면 안됨 
						m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ChamberCloseChk[m_nDoorTestSite]); 
						CTL_Lib.Alarm_Error_Occurrence(m_nTDoorStep, dWARNING, m_strAlarmCode);
					}
				}
			}
			else
			{
				if(m_bWaitChk_Falg == false)
				{
					m_bWaitChk_Falg = true;
					m_dwWaitTime[0]	= GetCurrentTime();
				}
				else 
				{
					//limit time 설정 
					m_dwWaitTime[1] = GetCurrentTime();
					m_dwWaitTime[2] = m_dwWaitTime[1] - m_dwWaitTime[0];
					if (m_dwWaitTime[2] <= 0)
					{
						m_dwWaitTime[0] = GetCurrentTime();						 
					}
					if (m_dwWaitTime[2] >  IO_CONVEYOR_WAIT_LIMIT)
					{
						if(st_test_site_info[m_nDoorTestSite].nTester_Req[1] == WORK_PICK) //open chamber, 자재 test site 에서 집기전 실행 
						{
							m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ChamberOpenChk[m_nDoorTestSite]); 
						}
						else if(st_test_site_info[m_nDoorTestSite].nTester_Req[1] == WORK_PLACE) //close chamber, 자재 test site에 insert 후 진행
						{
							m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ChamberCloseChk[m_nDoorTestSite]); 
						}
						CTL_Lib.Alarm_Error_Occurrence(m_nTDoorStep, dWARNING, m_strAlarmCode);
						m_bWaitChk_Falg = false;
					}

					////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					//센서도 확인하자 혹시 플레그가 문제가 될 경우 대비하여, 조건이 충족되면 플레그를 강제 셋팅하여 해당 작업 종료  
					////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					if(st_test_site_info[m_nDoorTestSite].nTester_Req[1] == WORK_PICK) //open chamber, 자재 test site 에서 집기전 실행 
					{
						nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ChamberOpenChk[m_nDoorTestSite],	IO_ON);  
						nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ChamberCloseChk[m_nDoorTestSite],	IO_OFF);  
						if(nRet_1 == IO_ON && nRet_1 == IO_OFF )
						{
							st_test_site_info[m_nDoorTestSite].nTester_Req[0] = CTL_FREE; //강제 종료 
							m_nTDoorStep = 3000;
						}
					}
					else if(st_test_site_info[m_nDoorTestSite].nTester_Req[1] == WORK_PLACE) //close chamber, 자재 test site에 insert 후 진행
					{
						nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ChamberOpenChk[m_nDoorTestSite],	IO_OFF);  
						nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ChamberCloseChk[m_nDoorTestSite],	IO_ON);  
						if(nRet_1 == IO_ON && nRet_1 == IO_OFF )
						{
							st_test_site_info[m_nDoorTestSite].nTester_Req[0] = CTL_FREE; //강제 종료 
							  m_nTDoorStep = 3000;
						}
					}
				}
			}
			break;

		case 3000:
			if(st_test_site_info[m_nDoorTestSite].nTester_Req[1] == WORK_PICK) //open chamber, 자재 test site 에서 집기전 실행 
			{
				st_sync_info.nTestRbt_Dvc_Req[m_nDoorTestSite][0] = CTL_READY; //test site robot가 소켓에서 뺴 갈 수 있게 플레그 셋

				st_test_site_info[m_nDoorTestSite].nTester_Req[0] = CTL_CLEAR;
				m_nTDoorStep = 1000;
				nFuncRet = RET_GOOD;
			}
			else if(st_test_site_info[m_nDoorTestSite].nTester_Req[1] == WORK_PLACE) //close chamber, 자재 test site에 insert 후 진행
			{
				st_test_site_info[m_nDoorTestSite].nTestBdStart = BD_START;//해당 테스트 사이트 test start 시작한다

				st_test_site_info[m_nDoorTestSite].nTester_Req[0] = CTL_CLEAR;
				m_nTDoorStep = 1000;
				nFuncRet = RET_GOOD;
			}
			break;

		default : 
			break;
	}

	return nFuncRet;
}

void CWorkManualMove::Set_Push_UnDn( int nSite, int nOnOff )
{
	m_bPushUpDnFlag[nSite]	= false;
	m_dwPushOnOff[nSite][0]	= GetCurrentTime();

	FAS_IO.set_out_bit( st_io_info.o_TsiteUpCyl[nSite], nOnOff);
	FAS_IO.set_out_bit( st_io_info.o_TsiteDnCyl[nSite], !nOnOff);	
}

int CWorkManualMove::Chk_Push_UnDn( int nSite, int nOnOff )
{

//802110 1 0 "TESTSITE1_UP_OFF_CHECK_ERROR."
//802111 1 0 "TESTSITE1_DOWN_OFF_CHECK_ERROR."
//802200 1 0 "TESTSITE2_UP_OFF_CHECK_ERROR."
//802201 1 0 "TESTSITE2_DOWN_OFF_CHECK_ERROR."
//802206 1 0 "TESTSITE3_UP_OFF_CHECK_ERROR."
//802207 1 0 "TESTSITE3_DOWN_OFF_CHECK_ERROR."
//802212 1 0 "TESTSITE4_UP_OFF_CHECK_ERROR."
//802213 1 0 "TESTSITE4_DOWN_OFF_CHECK_ERROR."
//802310 1 0 "TESTSITE5_UP_OFF_CHECK_ERROR."
//802311 1 0 "TESTSITE5_DOWN_OFF_CHECK_ERROR."
//802400 1 0 "TESTSITE6_UP_OFF_CHECK_ERROR."
//802401 1 0 "TESTSITE6_DOWN_OFF_CHECK_ERROR."
//802406 1 0 "TESTSITE7_UP_OFF_CHECK_ERROR."
//802407 1 0 "TESTSITE7_DOWN_OFF_CHECK_ERROR."
//802412 1 0 "TESTSITE8_UP_OFF_CHECK_ERROR."
//802413 1 0 "TESTSITE8_DOWN_OFF_CHECK_ERROR."

	int nWaitTime = WAIT_PUSHER_ON_OFF;

	if (nOnOff == IO_OFF)
	{
		if (m_bPushUpDnFlag[nSite] == false &&
			FAS_IO.get_in_bit(st_io_info.i_TsiteUpChk[nSite],	IO_OFF)	== IO_OFF &&
			FAS_IO.get_in_bit(st_io_info.i_TsiteDnChk[nSite],	IO_ON)	== IO_ON)
		{
			m_bPushUpDnFlag[nSite]		= true;
			m_dwPushOnOff[nSite][0]	= GetCurrentTime();
		}
		else if (m_bPushUpDnFlag[nSite] == true &&
			FAS_IO.get_in_bit(st_io_info.i_TsiteUpChk[nSite],	IO_OFF)	== IO_OFF &&
			FAS_IO.get_in_bit(st_io_info.i_TsiteDnChk[nSite],	IO_ON)	== IO_ON)
		{
			m_dwPushOnOff[nSite][1] = GetCurrentTime();
			m_dwPushOnOff[nSite][2] = m_dwPushOnOff[nSite][1] - m_dwPushOnOff[nSite][0];

			if (m_dwPushOnOff[nSite][2] <= 0)
			{
				m_dwPushOnOff[nSite][0] = GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwPushOnOff[nSite][2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
			{
				return RET_GOOD;
			}
		}
		else
		{
			m_dwPushOnOff[nSite][1] = GetCurrentTime();
			m_dwPushOnOff[nSite][2] = m_dwPushOnOff[nSite][1] - m_dwPushOnOff[nSite][0];

			if (m_dwPushOnOff[nSite][2] <= 0)
			{
				m_dwPushOnOff[nSite][0] = GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwPushOnOff[nSite][2] > (DWORD)st_wait_info.nLimitWaitTime[nWaitTime])
			{
				m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_TsiteDnChk[nSite]); 
				return RET_ERROR;
			}
		}
	}
	else
	{
		if (m_bPushUpDnFlag[nSite] == false &&
			FAS_IO.get_in_bit(st_io_info.i_TsiteUpChk[nSite],	IO_ON)	== IO_ON &&
			FAS_IO.get_in_bit(st_io_info.i_TsiteDnChk[nSite],	IO_OFF)	== IO_OFF )
		{
			m_bPushUpDnFlag[nSite]			= true;
			m_dwPushOnOff[nSite][0]	= GetCurrentTime();
		}
		else if (m_bPushUpDnFlag[nSite] == true &&
				FAS_IO.get_in_bit(st_io_info.i_TsiteUpChk[nSite],	IO_ON)	== IO_ON &&
				FAS_IO.get_in_bit(st_io_info.i_TsiteDnChk[nSite],	IO_OFF)	== IO_OFF )
		{
			m_dwPushOnOff[nSite][1]	= GetCurrentTime();
			m_dwPushOnOff[nSite][2]	= m_dwPushOnOff[nSite][1] - m_dwPushOnOff[nSite][0];

			if (m_dwPushOnOff[nSite][2] <= 0)
			{
				m_dwPushOnOff[nSite][0]	= GetCurrentTime();
				return RET_PROCEED;
			}

			if(m_dwPushOnOff[nSite][2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
			{
				return RET_GOOD;
			}
		}
		else
		{
			m_dwPushOnOff[nSite][1]	= GetCurrentTime();
			m_dwPushOnOff[nSite][2]	= m_dwPushOnOff[nSite][1] - m_dwPushOnOff[nSite][0];

			if (m_dwPushOnOff[nSite][2] <= 0)
			{
				m_dwPushOnOff[nSite][0]	= GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwPushOnOff[nSite][2] > (DWORD)st_wait_info.nLimitWaitTime[nWaitTime])
			{
				m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_TsiteUpChk[nSite]); 
				return RET_ERROR;
			}
		}
	}

	return RET_PROCEED;
}

void CWorkManualMove::OnBnClickedBtnTsite1UpDn()
{
	m_nDoorTestSite = 0;
	if( m_nUpDn[m_nDoorTestSite] == 0)  m_nUpDn[m_nDoorTestSite] = 1;//UP
	else												 m_nUpDn[m_nDoorTestSite] = 0;//DN

	OnBtnEnable(false);
	m_nTUpDnStep = 0;
	SetTimer( TM_TSITE_UPDN, 100, NULL);	
}

void CWorkManualMove::OnBnClickedBtnTsite2UpDn()
{
	m_nDoorTestSite = 1;
	if( m_nUpDn[m_nDoorTestSite] == 0)  m_nUpDn[m_nDoorTestSite] = 1;//UP
	else												 m_nUpDn[m_nDoorTestSite] = 0;//DN

	OnBtnEnable(false);
	m_nTUpDnStep = 0;
	SetTimer( TM_TSITE_UPDN, 100, NULL);	
}


void CWorkManualMove::OnBnClickedBtnTsite3UpDn()
{
	m_nDoorTestSite = 2;
	if( m_nUpDn[m_nDoorTestSite] == 0)  m_nUpDn[m_nDoorTestSite] = 1;//UP
	else												 m_nUpDn[m_nDoorTestSite] = 0;//DN

	OnBtnEnable(false);
	m_nTUpDnStep = 0;
	SetTimer( TM_TSITE_UPDN, 100, NULL);
}


void CWorkManualMove::OnBnClickedBtnTsite4UpDn()
{
	m_nDoorTestSite = 3;
	if( m_nUpDn[m_nDoorTestSite] == 0)  m_nUpDn[m_nDoorTestSite] = 1;//UP
	else												 m_nUpDn[m_nDoorTestSite] = 0;//DN

	OnBtnEnable(false);
	m_nTUpDnStep = 0;
	SetTimer( TM_TSITE_UPDN, 100, NULL);
}


void CWorkManualMove::OnBnClickedBtnTsite5UpDn()
{
	m_nDoorTestSite = 4;
	if( m_nUpDn[m_nDoorTestSite] == 0)  m_nUpDn[m_nDoorTestSite] = 1;//UP
	else												 m_nUpDn[m_nDoorTestSite] = 0;//DN

	OnBtnEnable(false);
	m_nTUpDnStep = 0;
	SetTimer( TM_TSITE_UPDN, 100, NULL);
}


void CWorkManualMove::OnBnClickedBtnTsite6UpDn()
{
	m_nDoorTestSite = 5;
	if( m_nUpDn[m_nDoorTestSite] == 0)  m_nUpDn[m_nDoorTestSite] = 1;//UP
	else												 m_nUpDn[m_nDoorTestSite] = 0;//DN

	OnBtnEnable(false);
	m_nTUpDnStep = 0;
	SetTimer( TM_TSITE_UPDN, 100, NULL);
}


void CWorkManualMove::OnBnClickedBtnTsite7UpDn()
{
	m_nDoorTestSite = 6;
	if( m_nUpDn[m_nDoorTestSite] == 0)  m_nUpDn[m_nDoorTestSite] = 1;//UP
	else												 m_nUpDn[m_nDoorTestSite] = 0;//DN

	OnBtnEnable(false);
	m_nTUpDnStep = 0;
	SetTimer( TM_TSITE_UPDN, 100, NULL);
}


void CWorkManualMove::OnBnClickedBtnTsite8UpDn()
{
	m_nDoorTestSite = 7;
	if( m_nUpDn[m_nDoorTestSite] == 0)  m_nUpDn[m_nDoorTestSite] = 1;//UP
	else												 m_nUpDn[m_nDoorTestSite] = 0;//DN

	OnBtnEnable(false);
	m_nTUpDnStep = 0;
	SetTimer( TM_TSITE_UPDN, 100, NULL);
}


int	 CWorkManualMove::OnTestSiteUpDn()
{
	int nRet = 0;
	int nFuncRet = RET_PROCEED;
	switch(m_nTUpDnStep)
	{
	case 0:	
		m_nTUpDnStep = 1000;
		break;

	case 1000:
		Set_Push_UnDn( m_nDoorTestSite, m_nUpDn[m_nDoorTestSite] );
		m_nTUpDnStep = 2000;
		break;

	case 2000:

		nRet = Chk_Push_UnDn( m_nDoorTestSite, m_nUpDn[m_nDoorTestSite] );
		if( nRet == RET_GOOD )
		{
			nFuncRet = RET_GOOD;
			m_nTUpDnStep = 0;
		}
		else if(nRet == RET_ERROR)
		{
			nFuncRet = RET_ERROR;
			m_nTUpDnStep = 0;
		}		
		break;
	}

	return nFuncRet;
}