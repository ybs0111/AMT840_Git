// WorkBarcode.cpp : 구현 파일입니다.

#include "stdafx.h"
#include "Handler.h"
#include "WorkBarcode.h"
#include "afxdialogex.h"
#include "PublicFunction.h"
#include "Dialog_KeyBoard.h"
#include "Dialog_KeyPad.h"
#include "Dialog_Message.h"
#include "CtlBd_Variable.h"
#include "Comm.h"
#include "Barcode_1D.h"
#include "Barcode_2D.h"
#include ".\\Ctrl\\KeyBoard.h"
#include "LogFromat.h"

// CWorkBarcode 대화 상자입니다.
#define TM_BARCODE		100
#define TM_BARCODE_2D       200

IMPLEMENT_DYNAMIC(CWorkBarcode, CDialogEx)

CWorkBarcode::CWorkBarcode(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWorkBarcode::IDD, pParent)
{
	m_nBarcodePos = 0;
}

CWorkBarcode::~CWorkBarcode()
{
}

void CWorkBarcode::OnInitLabel()
{
	for(int i=0; i<2; i++)
	{
		m_msgPort[i].SetFont(clsFunc.m_pFont[1]);
		m_msgPort[i].SetWindowText(_T("Port"));
		m_msgPort[i].SetCenterText();
		m_msgPort[i].SetColor(WHITE_C);
		m_msgPort[i].SetGradientColor(ORANGE_C);
		m_msgPort[i].SetTextColor(BLACK_C);

		m_msgRate[i].SetFont(clsFunc.m_pFont[1]);
		m_msgRate[i].SetWindowText(_T("BaudRate"));
		m_msgRate[i].SetCenterText();
		m_msgRate[i].SetColor(WHITE_C);
		m_msgRate[i].SetGradientColor(ORANGE_C);
		m_msgRate[i].SetTextColor(BLACK_C);

		m_msgData[i].SetFont(clsFunc.m_pFont[1]);
		m_msgData[i].SetWindowText(_T("Data"));
		m_msgData[i].SetCenterText();
		m_msgData[i].SetColor(WHITE_C);
		m_msgData[i].SetGradientColor(ORANGE_C);
		m_msgData[i].SetTextColor(BLACK_C);

		m_msgStop[i].SetFont(clsFunc.m_pFont[1]);
		m_msgStop[i].SetWindowText(_T("Stop"));
		m_msgStop[i].SetCenterText();
		m_msgStop[i].SetColor(WHITE_C);
		m_msgStop[i].SetGradientColor(ORANGE_C);
		m_msgStop[i].SetTextColor(BLACK_C);

		m_msgParity[i].SetFont(clsFunc.m_pFont[1]);
		m_msgParity[i].SetWindowText(_T("Parity"));
		m_msgParity[i].SetCenterText();
		m_msgParity[i].SetColor(WHITE_C);
		m_msgParity[i].SetGradientColor(ORANGE_C);
		m_msgParity[i].SetTextColor(BLACK_C);
	}

	m_msgClientIp1.SetFont(clsFunc.m_pFont[1]);
	m_msgClientIp1.SetWindowText(_T("IP"));
	m_msgClientIp1.SetCenterText();
	m_msgClientIp1.SetColor(WHITE_C);
	m_msgClientIp1.SetGradientColor(ORANGE_C);
	m_msgClientIp1.SetTextColor(BLACK_C);

	m_msgClientPort1.SetFont(clsFunc.m_pFont[1]);
	m_msgClientPort1.SetWindowText(_T("Port"));
	m_msgClientPort1.SetCenterText();
	m_msgClientPort1.SetColor(WHITE_C);
	m_msgClientPort1.SetGradientColor(ORANGE_C);
	m_msgClientPort1.SetTextColor(BLACK_C);

	m_msgClientIp2.SetFont(clsFunc.m_pFont[1]);
	m_msgClientIp2.SetWindowText(_T("IP"));
	m_msgClientIp2.SetCenterText();
	m_msgClientIp2.SetColor(WHITE_C);
	m_msgClientIp2.SetGradientColor(ORANGE_C);
	m_msgClientIp2.SetTextColor(BLACK_C);

	m_msgClientPort2.SetFont(clsFunc.m_pFont[1]);
	m_msgClientPort2.SetWindowText(_T("Port"));
	m_msgClientPort2.SetCenterText();
	m_msgClientPort2.SetColor(WHITE_C);
	m_msgClientPort2.SetGradientColor(ORANGE_C);
	m_msgClientPort2.SetTextColor(BLACK_C);

	m_msgClientIp3.SetFont(clsFunc.m_pFont[1]);
	m_msgClientIp3.SetWindowText(_T("IP"));
	m_msgClientIp3.SetCenterText();
	m_msgClientIp3.SetColor(WHITE_C);
	m_msgClientIp3.SetGradientColor(ORANGE_C);
	m_msgClientIp3.SetTextColor(BLACK_C);

	m_msgClientPort3.SetFont(clsFunc.m_pFont[1]);
	m_msgClientPort3.SetWindowText(_T("Port"));
	m_msgClientPort3.SetCenterText();
	m_msgClientPort3.SetColor(WHITE_C);
	m_msgClientPort3.SetGradientColor(ORANGE_C);
	m_msgClientPort3.SetTextColor(BLACK_C);

	m_msgClientIp6.SetFont(clsFunc.m_pFont[1]);
	m_msgClientIp6.SetWindowText(_T("IP"));
	m_msgClientIp6.SetCenterText();
	m_msgClientIp6.SetColor(WHITE_C);
	m_msgClientIp6.SetGradientColor(ORANGE_C);
	m_msgClientIp6.SetTextColor(BLACK_C);

	m_msgClientPort6.SetFont(clsFunc.m_pFont[1]);
	m_msgClientPort6.SetWindowText(_T("Port"));
	m_msgClientPort6.SetCenterText();
	m_msgClientPort6.SetColor(WHITE_C);
	m_msgClientPort6.SetGradientColor(ORANGE_C);
	m_msgClientPort6.SetTextColor(BLACK_C);


	m_msg_Barcode_1d_client_ip.SetFont(clsFunc.m_pFont[1]);
	m_msg_Barcode_1d_client_ip.SetWindowText(_T("IP"));
	m_msg_Barcode_1d_client_ip.SetCenterText();
	m_msg_Barcode_1d_client_ip.SetColor(WHITE_C);
	m_msg_Barcode_1d_client_ip.SetGradientColor(ORANGE_C);
	m_msg_Barcode_1d_client_ip.SetTextColor(BLACK_C);

	m_msg_Barcode_1d_client_port.SetFont(clsFunc.m_pFont[1]);
	m_msg_Barcode_1d_client_port.SetWindowText(_T("Port"));
	m_msg_Barcode_1d_client_port.SetCenterText();
	m_msg_Barcode_1d_client_port.SetColor(WHITE_C);
	m_msg_Barcode_1d_client_port.SetGradientColor(ORANGE_C);
	m_msg_Barcode_1d_client_port.SetTextColor(BLACK_C);

}
void CWorkBarcode::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GROUP_SERIAL_1,			m_groupSerial1);
	DDX_Control(pDX, IDC_MSG_PORT_1,				m_msgPort[0]);
	DDX_Control(pDX, IDC_MSG_RATE_1,				m_msgRate[0]);
	DDX_Control(pDX, IDC_MSG_DATA_1,				m_msgData[0]);
	DDX_Control(pDX, IDC_MSG_STOP_1,				m_msgStop[0]);
	DDX_Control(pDX, IDC_MSG_PARITY_1,				m_msgParity[0]);
	DDX_Control(pDX, IDC_MSG_PORT_2,				m_msgPort[1]);
	DDX_Control(pDX, IDC_MSG_RATE_2,				m_msgRate[1]);
	DDX_Control(pDX, IDC_MSG_DATA_2,				m_msgData[1]);
	DDX_Control(pDX, IDC_MSG_STOP_2,				m_msgStop[1]);
	DDX_Control(pDX, IDC_MSG_PARITY_2,				m_msgParity[1]);
	DDX_Control(pDX, IDC_BTN_SERIAL_APPLY_1,		m_btnSerialApply[0]);
	DDX_Control(pDX, IDC_BTN_SERIAL_CONNECT_1,		m_btnSerialConvet[0]);
	DDX_Control(pDX, IDC_BTN_BCODE_TRIGGER_ON,		m_btnBcodeOn[0]);
	DDX_Control(pDX, IDC_GROUP_UNLOAD_BARCODE_A,	m_groupUnldBcrA);
	DDX_Control(pDX, IDC_BTN_SERIAL_APPLY_2,		m_btnSerialApply[1]);
	DDX_Control(pDX, IDC_BTN_SERIAL_CONNECT_2,		m_btnSerialConvet[1]);
	DDX_Control(pDX, IDC_BTN_BCODE_TRIGGER_ON2,		m_btnBcodeOn[1]);
	DDX_Control(pDX, IDC_COMBO_PORT_1,				m_cbPort[0]);
	DDX_Control(pDX, IDC_COMBO_RATE_1,				m_cbRate[0]);
	DDX_Control(pDX, IDC_COMBO_DATA_1,				m_cbData[0]);
	DDX_Control(pDX, IDC_COMBO_STOP_1,				m_cbStop[0]);
	DDX_Control(pDX, IDC_COMBO_PARITY_1,			m_cbParity[0]);
	DDX_Control(pDX, IDC_COMBO_PORT_2,				m_cbPort[1]);
	DDX_Control(pDX, IDC_COMBO_RATE_2,				m_cbRate[1]);
	DDX_Control(pDX, IDC_COMBO_DATA_2,				m_cbData[1]);
	DDX_Control(pDX, IDC_COMBO_STOP_2,				m_cbStop[1]);
	DDX_Control(pDX, IDC_COMBO_PARITY_2,			m_cbParity[1]);
	DDX_Control(pDX, IDC_GROUP_LOAD_BARCODE,		m_groupLdBcr);
	DDX_Control(pDX, IDC_LIST_RESPONSE_BARCODE,		m_listBarcode);
	DDX_Control(pDX, IDC_BTN_APPLY,					m_btnApply);
	DDX_Control(pDX, IDC_BTN_RELOAD,				m_btnReLoad);
	DDX_Control(pDX, IDC_GROUP_NETWORK_1,			m_groupClient1);
	DDX_Control(pDX, IDC_GROUP_NETWORK_2,			m_groupClient2);
	DDX_Control(pDX, IDC_GROUP_NETWORK_3,			m_groupClient3);
	DDX_Control(pDX, IDC_GROUP_NETWORK_6,			m_groupClient6);
	DDX_Control(pDX, IDC_MSG_CLIENT_IP_1,			m_msgClientIp1);
	DDX_Control(pDX, IDC_MSG_CLIENT_PORT_1,			m_msgClientPort1);
	DDX_Control(pDX, IDC_MSG_CLIENT_IP_2,			m_msgClientIp2);
	DDX_Control(pDX, IDC_MSG_CLIENT_PORT_2,			m_msgClientPort2);
	DDX_Control(pDX, IDC_MSG_CLIENT_IP_3,			m_msgClientIp3);
	DDX_Control(pDX, IDC_MSG_CLIENT_PORT_3,			m_msgClientPort3);
	DDX_Control(pDX, IDC_MSG_CLIENT_IP_6,			m_msgClientIp6);
	DDX_Control(pDX, IDC_MSG_CLIENT_PORT_6,			m_msgClientPort6);
	DDX_Control(pDX, IDC_EDIT_CLIENT_PORT_1,		m_editClientPort1);
	DDX_Control(pDX, IDC_EDIT_CLIENT_PORT_2,		m_editClientPort2);
	DDX_Control(pDX, IDC_EDIT_CLIENT_PORT_3,		m_editClientPort3);
	DDX_Control(pDX, IDC_EDIT_CLIENT_PORT_6,		m_editClientPort6);
	DDX_Control(pDX, IDC_BTN_CLIENT_IP_1,			m_btnClientIpSet[0]);
	DDX_Control(pDX, IDC_BTN_CLIENT_PORT_1,			m_btnClientPortSet[0]);
	DDX_Control(pDX, IDC_BTN_CLIENT_IP_2,			m_btnClientIpSet[1]);
	DDX_Control(pDX, IDC_BTN_CLIENT_PORT_2,			m_btnClientPortSet[1]);
	DDX_Control(pDX, IDC_BTN_CLIENT_IP_3,			m_btnClientIpSet[2]);
	DDX_Control(pDX, IDC_BTN_CLIENT_PORT_3,			m_btnClientPortSet[2]);
	DDX_Control(pDX, IDC_BTN_CLIENT_IP_6,			m_btnClientIpSet[5]);
	DDX_Control(pDX, IDC_BTN_CLIENT_PORT_6,			m_btnClientPortSet[5]);

	DDX_Control(pDX, IDC_CLIENT_IP_1,				m_clientIp1);
	DDX_Control(pDX, IDC_CLIENT_IP_2,				m_clientIp2);
	DDX_Control(pDX, IDC_CLIENT_IP_3,				m_clientIp3);
	DDX_Control(pDX, IDC_CLIENT_IP_6,				m_clientIp6);
	DDX_Control(pDX, IDC_MSG_BARCODE_1D_CLIENT_IP, m_msg_Barcode_1d_client_ip);
	DDX_Control(pDX, IDC_MSG_BARCODE_1D_CLIENT_PORT, m_msg_Barcode_1d_client_port);
	DDX_Control(pDX, IDC_BARCODE_1D_CLIENT_IP, m_Barcode_1d_client_ip);
	DDX_Control(pDX, IDC_EDIT_BARCODE_1D_CLIENT_PORT, m_edit_Barcode_1d_client_port);
}


BEGIN_MESSAGE_MAP(CWorkBarcode, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_SERIAL_APPLY_1, &CWorkBarcode::OnBnClickedBtnSerialApply1)
	ON_BN_CLICKED(IDC_BTN_SERIAL_CONNECT_1, &CWorkBarcode::OnBnClickedBtnSerialConnect1)
	ON_BN_CLICKED(IDC_BTN_SERIAL_CONNECT_2, &CWorkBarcode::OnBnClickedBtnSerialConnect2)
	ON_BN_CLICKED(IDC_BTN_BCODE_TRIGGER_ON, &CWorkBarcode::OnBnClickedBtnBcodeTriggerOn)
	ON_BN_CLICKED(IDC_BTN_SERIAL_APPLY_2, &CWorkBarcode::OnBnClickedBtnSerialApply2)
	ON_BN_CLICKED(IDC_BTN_BCODE_TRIGGER_ON2, &CWorkBarcode::OnBnClickedBtnBcodeTriggerOn2)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_APPLY, &CWorkBarcode::OnBnClickedBtnApply)
	ON_BN_CLICKED(IDC_BTN_RELOAD, &CWorkBarcode::OnBnClickedBtnReload)
	ON_BN_CLICKED(IDC_BTN_CLIENT_IP_1, &CWorkBarcode::OnBnClickedBtnClientIp1)
	ON_BN_CLICKED(IDC_BTN_CLIENT_PORT_1, &CWorkBarcode::OnBnClickedBtnClientPort1)
	ON_BN_CLICKED(IDC_BTN_CLIENT_IP_2, &CWorkBarcode::OnBnClickedBtnClientIp2)
	ON_BN_CLICKED(IDC_BTN_CLIENT_PORT_2, &CWorkBarcode::OnBnClickedBtnClientPort2)
	ON_BN_CLICKED(IDC_BTN_CLIENT_IP_3, &CWorkBarcode::OnBnClickedBtnClientIp3)
	ON_BN_CLICKED(IDC_BTN_CLIENT_PORT_3, &CWorkBarcode::OnBnClickedBtnClientPort3)
	ON_BN_CLICKED(IDC_BTN_CLIENT_IP_6, &CWorkBarcode::OnBnClickedBtnClientIp6)
	ON_BN_CLICKED(IDC_BTN_CLIENT_PORT_6, &CWorkBarcode::OnBnClickedBtnClientPort6)
	ON_BN_CLICKED(IDC_BTN_BARCODE_1D_CLIENT_IP, &CWorkBarcode::OnBnClickedBtnBarcode1dClientIp)
	ON_BN_CLICKED(IDC_BTN_BARCODE_1D_CLIENT_PORT, &CWorkBarcode::OnBnClickedBtnBarcode1dClientPort)
END_MESSAGE_MAP()


// CWorkBarcode 메시지 처리기입니다.


void CWorkBarcode::OnBnClickedBtnSerialApply1()
{

	UpdateData(TRUE);

	m_nPort[0][1]			= m_cbPort[0].GetCurSel();
	m_nRate[0][1]			= m_cbRate[0].GetCurSel();
	m_nData[0][1]			= m_cbData[0].GetCurSel();
	m_nStop[0][1]			= m_cbStop[0].GetCurSel();
	m_nParity[0][1]			= m_cbParity[0].GetCurSel();
	

}
void CWorkBarcode::OnInitGroupBox()
{
	m_groupLdBcr.SetFont(clsFunc.OnLogFont(16));
	m_groupLdBcr.SetCatptionTextColor(BLUE_C);
	m_groupLdBcr.SetBorderColor(ORANGE_C);
	m_groupLdBcr.SetBackgroundColor(WINDOW_UP1);
	m_groupLdBcr.SetFontBold(TRUE);

	m_groupSerial1.SetFont(clsFunc.OnLogFont(16));
	m_groupSerial1.SetCatptionTextColor(BLUE_C);
	m_groupSerial1.SetBorderColor(ORANGE_C);
	m_groupSerial1.SetBackgroundColor(WINDOW_UP1);
	m_groupSerial1.SetFontBold(TRUE);

	m_groupUnldBcrA.SetFont(clsFunc.OnLogFont(16));
	m_groupUnldBcrA.SetCatptionTextColor(BLUE_C);
	m_groupUnldBcrA.SetBorderColor(ORANGE_C);
	m_groupUnldBcrA.SetBackgroundColor(WINDOW_UP1);
	m_groupUnldBcrA.SetFontBold(TRUE);

	m_groupClient1.SetFont(clsFunc.OnLogFont(16));
	m_groupClient1.SetCatptionTextColor(BLUE_C);
	m_groupClient1.SetBorderColor(ORANGE_C);
	m_groupClient1.SetBackgroundColor(WINDOW_UP1);
	m_groupClient1.SetFontBold(TRUE);

	m_groupClient2.SetFont(clsFunc.OnLogFont(16));
	m_groupClient2.SetCatptionTextColor(BLUE_C);
	m_groupClient2.SetBorderColor(ORANGE_C);
	m_groupClient2.SetBackgroundColor(WINDOW_UP1);
	m_groupClient2.SetFontBold(TRUE);

	m_groupClient3.SetFont(clsFunc.OnLogFont(16));
	m_groupClient3.SetCatptionTextColor(BLUE_C);
	m_groupClient3.SetBorderColor(ORANGE_C);
	m_groupClient3.SetBackgroundColor(WINDOW_UP1);
	m_groupClient3.SetFontBold(TRUE);

	m_groupClient6.SetFont(clsFunc.OnLogFont(16));
	m_groupClient6.SetCatptionTextColor(BLUE_C);
	m_groupClient6.SetBorderColor(ORANGE_C);
	m_groupClient6.SetBackgroundColor(WINDOW_UP1);
	m_groupClient6.SetFontBold(TRUE);

	
}

void CWorkBarcode::OnBnClickedBtnSerialConnect1()
{
	
	// jtkim 20150626
	st_serial_info.nSerialPort[0]		= ConverterToData(COM_PORT, m_nPort[0][1]);
	st_serial_info.nSerialData[0]		= ConverterToData(COM_DATA, m_nData[0][1]);
	st_serial_info.nSerialStop[0]		= ConverterToData(COM_STOP, m_nStop[0][1]);
	st_serial_info.nSerialBaudrate[0]	= ConverterToData(COM_BAUDRATE, m_nRate[0][1]);
	st_serial_info.nSerialParity[0]		= ConverterToData(COM_PARITY, m_nParity[0][1]);
	
}


void CWorkBarcode::OnBnClickedBtnSerialConnect2()
{

	st_serial_info.nSerialPort[1]		= ConverterToData(COM_PORT, m_nPort[1][1]);
	st_serial_info.nSerialData[1]		= ConverterToData(COM_DATA, m_nData[1][1]);
	st_serial_info.nSerialStop[1]		= ConverterToData(COM_STOP, m_nStop[1][1]);
	st_serial_info.nSerialBaudrate[1]	= ConverterToData(COM_BAUDRATE, m_nRate[1][1]);
	st_serial_info.nSerialParity[1]		= ConverterToData(COM_PARITY, m_nParity[1][1]);

}


void CWorkBarcode::OnBnClickedBtnBcodeTriggerOn()
{
	if (st_handler_info.nMenuLock) return;
	st_handler_info.nMenuLock = TRUE;

	m_nBarcodePos	= 0;
	m_nBarcodeStep	= 0;

	SetTimer(TM_BARCODE, 100, NULL);
}


void CWorkBarcode::OnBnClickedBtnSerialApply2()
{

	UpdateData(TRUE);

	m_nPort[1][1]			= m_cbPort[1].GetCurSel();
	m_nRate[1][1]			= m_cbRate[1].GetCurSel();
	m_nData[1][1]			= m_cbData[1].GetCurSel();
	m_nStop[1][1]			= m_cbStop[1].GetCurSel();
	m_nParity[1][1]			= m_cbParity[1].GetCurSel();

}


void CWorkBarcode::OnBnClickedBtnBcodeTriggerOn2()
{

	if (st_handler_info.nMenuLock) return;

	st_handler_info.nMenuLock = TRUE;

	 	m_nBarcodePos	= 1;
	 	m_nBarcodeStep	= 0;

		SetTimer(TM_BARCODE, 100, NULL);
	

}
 int CWorkBarcode::OnBarcode_1D()
 {
	/* BYTE byData[100];

	 int nLength;

	 CString strTemp;
	 CString strBarcode;
	// st_serial_info.nSerialPort[BCR_1D_PORT] = 9004;
	 	switch (m_nBarcodeStep)
	 	{
	 	case 0:
	 		if (clsBcr1D.m_bConnect)
	 		{
	 			m_nBarcodeStep = 100;
	 		}
	 		else
	 		{
	 			clsBcr1D.OnClose();
	
	 			if (clsBcr1D.OnOpen(st_serial_info.nSerialPort[BCR_1D_PORT],		st_serial_info.nSerialBaudrate[BCR_1D_PORT],
	 				st_serial_info.nSerialParity[BCR_1D_PORT],	st_serial_info.nSerialData[BCR_1D_PORT],
	 				st_serial_info.nSerialStop[BCR_1D_PORT],		0x0d))				
	 			{
	 				m_nBarcodeStep = 100;
	 			}
	 			else
	 			{
	 				m_nBarcodeStep = 0;
	 				m_listBarcode.AddString(_T("Not Connected"));

	 				return RET_ERROR;
	 			}
	 		}
	 		break;
	 
	 	case 100:
	 		m_dwWaitTime[0] = GetTickCount();
	 		
			strTemp.Format(_T("LON%02d"), st_recipe_info.nBcrBank);
			clsBcr1D.OnDataSend(strTemp);
	 		m_nBarcodeStep = 200;
	 		break;
	 
	 	case 200:
	 		if (clsBcr1D.m_pCom->m_bRevFlag)
	 		{
				nLength = clsBcr1D.m_pCom->m_nLength;

				if (nLength > 0 )
				{
					if (nLength > 100)
					{
						clsBcr1D.m_pCom->Empty();
						m_nBarcodeStep = 100;
						break;
					}

					clsBcr1D.m_pCom->ReadData(byData, nLength);
					strTemp		= clsFunc.OnCharToString((char*)byData, sizeof(byData));
					strBarcode	= strTemp.Mid(0, nLength -1);

					m_listBarcode.AddString(strBarcode);
					m_nBarcodeStep = 0;
					return RET_GOOD;
				}
				else
				{
					m_nBarcodeStep = 0;
				}
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
	 
	 			if (m_dwWaitTime[2] > (DWORD) 10000)
	 			{
	 				m_nBarcodeStep = 0;
	 				m_listBarcode.AddString(_T("Barcode Receive Fail"));
	 
	 				return RET_ERROR;
	 			}
	 		}
	 		break;
	 	}
	*/
 	return RET_PROCEED;
 }


void CWorkBarcode::OnDataInit()
{
	int pos,i;

	//m_nGmsServerPort[1] = 	st_server_info[GMS_NETWORK].nPort;

	for (i =0; i<10; i++)
	{
		m_strClientIp[i][1] = st_client_info[i].strIp;
							
		m_nClientPort[i][1] = st_client_info[i].nPort;

	}
	
	for (i =0; i<2; i++)
	{
		pos = st_serial_info.nSerialPort[i];
		m_nPort[i][1] = ConverterToPos(COM_PORT, pos);

		pos = st_serial_info.nSerialBaudrate[i];
		m_nRate[i][1] = ConverterToPos(COM_BAUDRATE, pos);

		pos = st_serial_info.nSerialData[i];
		m_nData[i][1] = ConverterToPos(COM_DATA, pos);

		pos = st_serial_info.nSerialParity[i];
		m_nParity[i][1] = ConverterToPos(COM_PARITY, pos);

		pos = st_serial_info.nSerialStop[i];
		m_nStop[i][1] = ConverterToPos(COM_STOP, pos);
	}
	
}

int CWorkBarcode::ConverterToData(int mode, int pos)
{
	int Ret = 0;

	switch(mode)
	{
	case COM_PORT:
		switch(pos)
		{
		case 0:
			Ret = 1;
			break;
		case 1:
			Ret = 2;
			break;
		case 2:
			Ret = 3;
			break;
		case 3:
			Ret = 4;
			break;
		case 4:
			Ret = 5;
			break;
		case 5:
			Ret = 6;
			break;
		case 6:
			Ret = 7;
			break;
		case 7:
			Ret = 8;
			break;
		}
		break;
	case COM_BAUDRATE:
		switch(pos)
		{
		case 0:
			Ret = 2400;
			break;
		case 1:
			Ret = 4800;
			break;
		case 2:
			Ret = 9600;
			break;
		case 3:
			Ret = 14400;
			break;
		case 4:
			Ret = 19200;
			break;
		case 5:
			Ret = 38400;
			break;
		case 6:
			Ret = 57600;
			break;
		case 7:
			Ret = 115200;
			break;
		}
		break;
	case COM_DATA:
		switch(pos)
		{
		case 0:
			Ret = 5;
			break;
		case 1:
			Ret = 6;
			break;
		case 2:
			Ret = 7;
			break;
		case 3:
			Ret = 8;
			break;
		}
		break;
	case COM_STOP:
		switch(pos)
		{
		case 0:
			Ret = ONESTOPBIT;
			break;
		case 1:
			Ret = TWOSTOPBITS;
			break;
		}
		break;
	case COM_PARITY:
		switch(pos)
		{
		case 0:
			Ret = EVENPARITY;  // Even
			break;
		case 1:
			Ret = ODDPARITY;  // Odd
			break;
		case 2:
			Ret = NOPARITY;  // None
			break;
		}
		break;
	}

	return Ret;
}

int CWorkBarcode::ConverterToPos(int mode, int data)
{
	int Ret = 0;

	switch(mode)
	{
	case COM_PORT:
		switch(data)
		{
		case 1:
			Ret = 0;
			break;
		case 2:
			Ret = 1;
			break;
		case 3:
			Ret = 2;
			break;
		case 4:
			Ret = 3;
			break;
		case 5:
			Ret = 4;
			break;
		case 6:
			Ret = 5;
			break;
		case 7:
			Ret = 6;
			break;
		case 8:
			Ret = 7;
			break;
		}
		break;
	case COM_BAUDRATE:
		switch(data)
		{
		case 2400:
			Ret = 0;
			break;
		case 4800:
			Ret = 1;
			break;
		case 9600:
			Ret = 2;
			break;
		case 14400:
			Ret = 3;
			break;
		case 19200:
			Ret = 4;
			break;
		case 38400:
			Ret = 5;
			break;
		case 57600:
			Ret = 6;
			break;
		case 115200:
			Ret = 7;
			break;
		}
		break;
	case COM_DATA:
		switch(data)
		{
		case 5:
			Ret = 0;
			break;
		case 6:
			Ret = 1;
			break;
		case 7:
			Ret = 2;
			break;
		case 8:
			Ret = 3;
			break;
		}
		break;
	case COM_STOP:
		switch(data)
		{
		case ONESTOPBIT:
			Ret = 0;
			break;
		case TWOSTOPBITS:
			Ret = 1;
			break;
		}
		break;
	case COM_PARITY:
		switch(data)
		{
		case EVENPARITY:
			Ret = 0;  // Even
			break;
		case ODDPARITY:
			Ret = 1;  // Odd
			break;
		case NOPARITY:
			Ret = 2;  // None
			break;
		}
		break;
	}

	return Ret;
}

int	CWorkBarcode::OnBarcode_2D()
{
	/*
	BYTE byData[100];

	int nLength;

	CString strTemp;
	CString strBarcode;

	switch (m_nBarcodeStep)
	{
	case 0:
		if (clsBcr2D.m_bConnect)
		{
			m_nBarcodeStep = 100;
		}
		else
		{
			clsBcr2D.OnClose();

			if (clsBcr2D.OnOpen(st_serial_info.nSerialPort[BCR_2D_PORT],		st_serial_info.nSerialBaudrate[BCR_2D_PORT],
				st_serial_info.nSerialParity[BCR_2D_PORT],	st_serial_info.nSerialData[BCR_2D_PORT],
				st_serial_info.nSerialStop[BCR_2D_PORT],		0x0d))
			{
				m_nBarcodeStep = 100;
			}
			else
			{
				m_nBarcodeStep = 0;
				m_listBarcode.AddString(_T("Not Connected"));

				return RET_ERROR;
			}
		}
		break;

	case 100:
		m_dwWaitTime[0] = GetTickCount();
		strTemp.Format(_T("LON%02d"), st_recipe_info.nBcrBank);
		clsBcr2D.OnDataSend(strTemp);

		m_nBarcodeStep = 200;
		break;

	case 200:
		if (clsBcr2D.m_pCom->m_bRevFlag)
		{
			nLength = clsBcr2D.m_pCom->m_nLength;

			if (nLength > 0 )
			{
				if (nLength > 100)
				{
					clsBcr2D.m_pCom->Empty();
					m_nBarcodeStep = 100;
					break;
				}

				clsBcr2D.m_pCom->ReadData(byData, nLength);
				strTemp		= clsFunc.OnCharToString((char*)byData, sizeof(byData));
				strBarcode	= strTemp.Mid(0, nLength -1);

				m_listBarcode.AddString(strBarcode);

				m_nBarcodeStep = 0;

				return RET_GOOD;
			}
			else
			{
				m_nBarcodeStep = 0;
			}
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

			if (m_dwWaitTime[2] > (DWORD) 10000)
			{
				m_nBarcodeStep = 0;
				m_listBarcode.AddString(_T("Barcode Receive Fail"));

				return RET_ERROR;
			}
		}
		break;
	}
	*/
	return RET_PROCEED;
}

BOOL CWorkBarcode::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	OnDataInit();

	OnInitGroupBox();
	OnInitLabel();
	OnInitButton();

	OnInt_Display();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CWorkBarcode::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	int nRet;

	switch (nIDEvent)
	{
		case TM_BARCODE:
			switch(m_nBarcodePos)
			{
				case 0:
				nRet = OnBarcode_1D();
				break;

				case 1:	
				nRet = OnBarcode_2D();
				break;
			}
		if (nRet == RET_GOOD)
		{
			KillTimer(TM_BARCODE);

			if (st_handler_info.nMenuLock != FALSE)
			{
				st_handler_info.nMenuLock = FALSE;
			}
		}
		else if (nRet == RET_ERROR)
		{
			KillTimer(TM_BARCODE);

			if (st_handler_info.nMenuLock != FALSE)
			{
				st_handler_info.nMenuLock = FALSE;
			}
		}
		break;
	}
	CDialogEx::OnTimer(nIDEvent);


}


void CWorkBarcode::OnBnClickedBtnApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
				OnDataApply();
				OnDataBackup();
			}
		}
	}
}
int CWorkBarcode::OnDataComparison()
{
	int i = 0;

// 	if (m_nGmsServerPort[1]			!= m_nGmsServerPort[0])
// 	{
// 		return RET_ERROR;
// 	}

	for(i = 0; i < 10; i++)
	{
		if (m_strClientIp[i][1]			!= m_strClientIp[i][0])
		{
			return RET_ERROR;
		}

		if (m_nClientPort[i][1]			!= m_nClientPort[i][0])
		{
			return RET_ERROR;
		}


		if (m_nPort[i][1]				!= m_nPort[i][0])
		{
			return RET_ERROR;
		}

		if (m_nRate[i][1]				!= m_nRate[i][0])
		{
			return RET_ERROR;
		}

		if (m_nData[i][1]				!= m_nData[i][0])
		{
			return RET_ERROR;
		}

		if (m_nParity[i][1]				!= m_nParity[i][0])
		{
			return RET_ERROR;
		}

		if (m_nStop[i][1]				!= m_nStop[i][0])
		{
			return RET_ERROR;
		}
	}
	

	return RET_GOOD;
}
void CWorkBarcode::OnInitButton()
{
	m_btnSerialApply[0].SetBitmaps(IDC_BTN_SERIAL_APPLY_1, IDB_BITMAP_INTERFACE_APPLY_DN, WINDOW_DN1, IDB_BITMAP_INTERFACE_APPLY_UP, WINDOW_UP1);
	m_btnSerialApply[0].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	m_btnSerialApply[0].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	m_btnSerialApply[0].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	m_btnSerialApply[0].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnSerialApply[0].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnSerialApply[0].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnSerialApply[0].SetFont(clsFunc.m_pFont[1]);
	m_btnSerialApply[0].SetTooltipText(_T("1 Serial Data Apply"));

	m_btnSerialConvet[0].SetBitmaps(IDC_BTN_SERIAL_CONNECT_1, IDB_BITMAP_INTERFACE_CONVERT_DN, WINDOW_DN1, IDB_BITMAP_INTERFACE_CONVERT_UP, WINDOW_UP1);
	m_btnSerialConvet[0].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	m_btnSerialConvet[0].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	m_btnSerialConvet[0].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	m_btnSerialConvet[0].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnSerialConvet[0].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnSerialConvet[0].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnSerialConvet[0].SetFont(clsFunc.m_pFont[1]);
	m_btnSerialConvet[0].SetTooltipText(_T("1 Serial Data Converter"));

	m_btnSerialApply[1].SetBitmaps(IDC_BTN_SERIAL_APPLY_1, IDB_BITMAP_INTERFACE_APPLY_DN, WINDOW_DN1, IDB_BITMAP_INTERFACE_APPLY_UP, WINDOW_UP1);
	m_btnSerialApply[1].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	m_btnSerialApply[1].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	m_btnSerialApply[1].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	m_btnSerialApply[1].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnSerialApply[1].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnSerialApply[1].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnSerialApply[1].SetFont(clsFunc.m_pFont[1]);
	m_btnSerialApply[1].SetTooltipText(_T("1 Serial Data Apply"));

	m_btnSerialConvet[1].SetBitmaps(IDC_BTN_SERIAL_CONNECT_1, IDB_BITMAP_INTERFACE_CONVERT_DN, WINDOW_DN1, IDB_BITMAP_INTERFACE_CONVERT_UP, WINDOW_UP1);
	m_btnSerialConvet[1].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	m_btnSerialConvet[1].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	m_btnSerialConvet[1].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	m_btnSerialConvet[1].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnSerialConvet[1].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnSerialConvet[1].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnSerialConvet[1].SetFont(clsFunc.m_pFont[1]);
	m_btnSerialConvet[1].SetTooltipText(_T("1 Serial Data Converter"));


	m_btnApply.SetBitmaps(IDC_BTN_APPLY, IDB_BITMAP_APPLY_DN1, WINDOW_DN1, IDB_BITMAP_APPLY_UP1, WINDOW_UP1);
	m_btnApply.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	m_btnApply.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	m_btnApply.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	m_btnApply.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnApply.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnApply.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnApply.SetFont(clsFunc.m_pFont[2]);
	m_btnApply.SetTooltipText(_T("Interface Data Apply"));

	m_btnReLoad.SetBitmaps(IDC_BTN_RELOAD, IDB_BITMAP_RELOAD_DN1, WINDOW_DN1, IDB_BITMAP_RELOAD_UP1, WINDOW_UP1);
	m_btnReLoad.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	m_btnReLoad.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	m_btnReLoad.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	m_btnReLoad.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnReLoad.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnReLoad.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnReLoad.SetFont(clsFunc.m_pFont[2]);
	m_btnReLoad.SetTooltipText(_T("Interface Data ReLoad"));

	m_btnBcodeOn[0].SetBitmaps(IDC_BTN_BCODE_TRIGGER_ON, IDB_BTN_UPLOAD_DN, WINDOW_DN1, IDB_BTN_UPLOAD_UP, WINDOW_UP1);
	m_btnBcodeOn[0].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	m_btnBcodeOn[0].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	m_btnBcodeOn[0].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	m_btnBcodeOn[0].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnBcodeOn[0].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnBcodeOn[0].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnBcodeOn[0].SetFont(clsFunc.m_pFont[2]);
	m_btnBcodeOn[0].SetTooltipText(_T("Trigger On"));

	m_btnBcodeOn[1].SetBitmaps(IDC_BTN_BCODE_TRIGGER_ON2, IDB_BTN_UPLOAD_DN, WINDOW_DN1, IDB_BTN_UPLOAD_UP, WINDOW_UP1);
	m_btnBcodeOn[1].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	m_btnBcodeOn[1].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	m_btnBcodeOn[1].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	m_btnBcodeOn[1].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnBcodeOn[1].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnBcodeOn[1].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnBcodeOn[1].SetFont(clsFunc.m_pFont[2]);
	m_btnBcodeOn[1].SetTooltipText(_T("Trigger On"));

	m_btnClientIpSet[0].SetBitmaps(IDC_BTN_CLIENT_IP_1, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
	m_btnClientIpSet[0].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	m_btnClientIpSet[0].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	m_btnClientIpSet[0].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	m_btnClientIpSet[0].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnClientIpSet[0].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnClientIpSet[0].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnClientIpSet[0].SetFont(clsFunc.m_pFont[1]);
	m_btnClientIpSet[0].SetTooltipText(_T("1 Client IP Address"));

	m_btnClientIpSet[1].SetBitmaps(IDC_BTN_CLIENT_IP_2, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
	m_btnClientIpSet[1].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	m_btnClientIpSet[1].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	m_btnClientIpSet[1].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	m_btnClientIpSet[1].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnClientIpSet[1].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnClientIpSet[1].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnClientIpSet[1].SetFont(clsFunc.m_pFont[1]);
	m_btnClientIpSet[1].SetTooltipText(_T("2 Client IP Address"));

	m_btnClientIpSet[2].SetBitmaps(IDC_BTN_CLIENT_IP_3, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
	m_btnClientIpSet[2].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	m_btnClientIpSet[2].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	m_btnClientIpSet[2].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	m_btnClientIpSet[2].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnClientIpSet[2].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnClientIpSet[2].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnClientIpSet[2].SetFont(clsFunc.m_pFont[1]);
	m_btnClientIpSet[2].SetTooltipText(_T("3 Client IP Address"));

	m_btnClientIpSet[5].SetBitmaps(IDC_BTN_CLIENT_IP_6, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
	m_btnClientIpSet[5].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	m_btnClientIpSet[5].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	m_btnClientIpSet[5].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	m_btnClientIpSet[5].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnClientIpSet[5].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnClientIpSet[5].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnClientIpSet[5].SetFont(clsFunc.m_pFont[1]);
	m_btnClientIpSet[5].SetTooltipText(_T("FTP Client IP Address"));

	m_btnClientPortSet[0].SetBitmaps(IDC_BTN_CLIENT_PORT_1, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
	m_btnClientPortSet[0].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	m_btnClientPortSet[0].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	m_btnClientPortSet[0].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	m_btnClientPortSet[0].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnClientPortSet[0].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnClientPortSet[0].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnClientPortSet[0].SetFont(clsFunc.m_pFont[1]);
	m_btnClientPortSet[0].SetTooltipText(_T("1 Client Port"));

	m_btnClientPortSet[1].SetBitmaps(IDC_BTN_CLIENT_PORT_2, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
	m_btnClientPortSet[1].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	m_btnClientPortSet[1].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	m_btnClientPortSet[1].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	m_btnClientPortSet[1].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnClientPortSet[1].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnClientPortSet[1].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnClientPortSet[1].SetFont(clsFunc.m_pFont[1]);
	m_btnClientPortSet[1].SetTooltipText(_T("2 Client Port"));

	m_btnClientPortSet[2].SetBitmaps(IDC_BTN_CLIENT_PORT_3, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
	m_btnClientPortSet[2].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	m_btnClientPortSet[2].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	m_btnClientPortSet[2].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	m_btnClientPortSet[2].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnClientPortSet[2].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnClientPortSet[2].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnClientPortSet[2].SetFont(clsFunc.m_pFont[1]);
	m_btnClientPortSet[2].SetTooltipText(_T("3 Client Port"));

	m_btnClientPortSet[5].SetBitmaps(IDC_BTN_CLIENT_PORT_6, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
	m_btnClientPortSet[5].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	m_btnClientPortSet[5].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	m_btnClientPortSet[5].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	m_btnClientPortSet[5].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnClientPortSet[5].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnClientPortSet[5].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnClientPortSet[5].SetFont(clsFunc.m_pFont[1]);
	m_btnClientPortSet[5].SetTooltipText(_T("FTP Client Port"));
}

void CWorkBarcode::OnBnClickedBtnReload()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnInt_Display();
	OnDataRecovery();
}
void CWorkBarcode::OnDataRecovery()
{
	int i = 0;

	for(i = 0; i < 10; i++)
	{
		m_strClientIp[i][1]			= m_strClientIp[i][0];
		m_nClientPort[i][1]			= m_nClientPort[i][0];

		

		m_nPort[i][1]				= m_nPort[i][0];
		m_nRate[i][1]				= m_nRate[i][0];
		m_nData[i][1]				= m_nData[i][0];
		m_nParity[i][1]				= m_nParity[i][0];
		m_nStop[i][1]				= m_nStop[i][0];
	}
	
}
void CWorkBarcode::OnDataApply()
{
	int i;

	//st_server_info[GMS_NETWORK].nPort  =  m_nGmsServerPort[1];
	

	for(i = 0; i < 10; i++)
	{
		//2017.0109
		if (st_client_info[i].strIp	!= m_strClientIp[i][1])
		{
			m_strLogKey[0]	= _T("TYPE");
			m_strLogData[0]	= _T("CLIENT IP");
			m_strLogKey[1]	= _T("CLIENT NUM");
			m_strLogData[1].Format(_T("%d"), i + 1);
			m_strLogKey[2]	= _T("OLD");
			m_strLogData[2].Format(_T("%s"), st_client_info[i].strIp);
			m_strLogKey[3]	= _T("NEW");
			m_strLogData[3].Format(_T("%s"), m_strClientIp[i][1]);

			clsLog.LogConfig(_T("WORKFTP"), _T("CHANGE"), st_basic_info.strDeviceName, 4, m_strLogKey, m_strLogData);
		}
		if (st_client_info[i].nPort	!= m_nClientPort[i][1])
		{
			m_strLogKey[0]	= _T("TYPE");
			m_strLogData[0]	= _T("CLIENT PORT");
			m_strLogKey[1]	= _T("CLIENT NUM");
			m_strLogData[1].Format(_T("%d"), i + 1);
			m_strLogKey[2]	= _T("OLD");
			m_strLogData[2].Format(_T("%d"), st_client_info[i].nPort);
			m_strLogKey[3]	= _T("NEW");
			m_strLogData[3].Format(_T("%d"), m_nClientPort[i][1]);

			clsLog.LogConfig(_T("WORKFTP"), _T("CHANGE"), st_basic_info.strDeviceName, 3, m_strLogKey, m_strLogData);
		}


		st_client_info[i].strIp = m_strClientIp[i][1];
		st_client_info[i].nPort = m_nClientPort[i][1];


		st_serial_info.nSerialPort[i]		= ConverterToData(COM_PORT, m_nPort[i][1]);
		st_serial_info.nSerialData[i]		= ConverterToData(COM_DATA, m_nData[i][1]);
		st_serial_info.nSerialStop[i]		= ConverterToData(COM_STOP, m_nStop[i][1]);
		st_serial_info.nSerialBaudrate[i]	= ConverterToData(COM_BAUDRATE, m_nRate[i][1]);
		st_serial_info.nSerialParity[i]		= ConverterToData(COM_PARITY, m_nParity[i][1]);
	}
}
void CWorkBarcode::OnDataBackup()
{
	int i = 0;
	//m_nGmsServerPort[0] = m_nGmsServerPort[1];

	for(i = 0; i < 10; i++)
	{
		m_strClientIp[i][0]			= m_strClientIp[i][1];
		m_nClientPort[i][0]			= m_nClientPort[i][1];

		

		m_nPort[i][0]				= m_nPort[i][1];
		m_nRate[i][0]				= m_nRate[i][1];
		m_nData[i][0]				= m_nData[i][1];
		m_nParity[i][0]				= m_nParity[i][1];
		m_nStop[i][0]				= m_nStop[i][1];
	}
	
}
void CWorkBarcode::OnInt_Display()
{
	CString str_tmp;
	int i;

	for (i=0; i<3; i++)
	{
		m_cbPort[i].SetCurSel(m_nPort[i][1]);
		m_cbRate[i].SetCurSel(m_nRate[i][1]);
		m_cbData[i].SetCurSel(m_nData[i][1]);
		m_cbParity[i].SetCurSel(m_nParity[i][1]);
		m_cbStop[i].SetCurSel(m_nStop[i][1]);
	}

	OnInterfaceIpAddress(m_strClientIp[0][1]);
	m_clientIp1.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	str_tmp.Format(_T("%d"), m_nClientPort[0][1]);
	m_editClientPort1.SetWindowText(str_tmp);

	OnInterfaceIpAddress(m_strClientIp[1][1]);
	m_clientIp2.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	str_tmp.Format(_T("%d"), m_nClientPort[1][1]);
	m_editClientPort2.SetWindowText(str_tmp);

	OnInterfaceIpAddress(m_strClientIp[2][1]);
	m_clientIp3.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	str_tmp.Format(_T("%d"), m_nClientPort[2][1]);
	m_editClientPort3.SetWindowText(str_tmp);

// 	str_tmp.Format(_T("%d"), m_nGmsServerPort[1]);
// 	m_editClientPort3.SetWindowText(str_tmp);

	OnInterfaceIpAddress(m_strClientIp[6][1]);
	m_clientIp6.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	str_tmp.Format(_T("%d"), m_nClientPort[6][1]);
	m_editClientPort6.SetWindowText(str_tmp);

	//
	OnInterfaceIpAddress(m_strClientIp[7][1]);
	m_Barcode_1d_client_ip.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	str_tmp.Format(_T("%d"), m_nClientPort[7][1]);
	m_edit_Barcode_1d_client_port.SetWindowText(str_tmp);

	OnInterfaceIpAddress(m_strClientIp[8][1]); //ECSERVER
	m_Barcode_1d_client_ip.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	str_tmp.Format(_T("%d"), m_nClientPort[8][1]);
	m_edit_Barcode_1d_client_port.SetWindowText(str_tmp);

	OnInterfaceIpAddress(m_strClientIp[9][1]); //XGEM
	m_Barcode_1d_client_ip.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	str_tmp.Format(_T("%d"), m_nClientPort[9][1]);
	m_edit_Barcode_1d_client_port.SetWindowText(str_tmp);
}

void CWorkBarcode::OnBnClickedBtnClientIp1()
{
	CDialog_KeyBoard dlgKeyBoard;
	CString strTmp;

	dlgKeyBoard.m_strKeyBoardVal = _T("");

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		// TODO: Add your control notification handler code here
		m_strClientIp[0][1] = dlgKeyBoard.m_strKeyBoardVal;

		m_strClientIp[0][1].MakeUpper();
		m_strClientIp[0][1].TrimLeft(' ');               
		m_strClientIp[0][1].TrimRight(' ');

		OnInterfaceIpAddress(m_strClientIp[0][1]);

		m_clientIp1.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	}
}

void CWorkBarcode::OnInterfaceIpAddress(CString str_ip)
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

void CWorkBarcode::OnBnClickedBtnClientPort1()
{
	CDialog_KeyPad dlgKeyPad;
	CString strTmp;

	dlgKeyPad.m_nKeypadMode			= 0;
	dlgKeyPad.m_strKeypadLowLimit	= _T("0");
	dlgKeyPad.m_strKeypadHighLimit	= _T("100000");
	dlgKeyPad.m_strKeypadVal.Format(_T("%d"),m_nClientPort[0][1]);

	if (dlgKeyPad.DoModal() == IDOK)
	{
		strTmp = dlgKeyPad.m_strNewVal;

		m_nClientPort[0][1] = _wtoi(strTmp);	
		m_editClientPort1.SetWindowText(strTmp);
	}
}


void CWorkBarcode::OnBnClickedBtnClientIp2()
{
	CDialog_KeyBoard dlgKeyBoard;
	CString strTmp;

	dlgKeyBoard.m_strKeyBoardVal = _T("");

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		// TODO: Add your control notification handler code here
		m_strClientIp[1][1] = dlgKeyBoard.m_strKeyBoardVal;

		m_strClientIp[1][1].MakeUpper();
		m_strClientIp[1][1].TrimLeft(' ');               
		m_strClientIp[1][1].TrimRight(' ');

		OnInterfaceIpAddress(m_strClientIp[1][1]);

		m_clientIp2.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	}
}


void CWorkBarcode::OnBnClickedBtnClientPort2()
{
	CDialog_KeyPad dlgKeyPad;
	CString strTmp;

	dlgKeyPad.m_nKeypadMode			= 0;
	dlgKeyPad.m_strKeypadLowLimit	= _T("0");
	dlgKeyPad.m_strKeypadHighLimit	= _T("100000");
	dlgKeyPad.m_strKeypadVal.Format(_T("%d"),m_nClientPort[1][1]);

	if (dlgKeyPad.DoModal() == IDOK)
	{
		strTmp = dlgKeyPad.m_strNewVal;

		m_nClientPort[1][1] = _wtoi(strTmp);	
		m_editClientPort2.SetWindowText(strTmp);
	}
}


void CWorkBarcode::OnBnClickedBtnClientIp3()
{
	CDialog_KeyBoard dlgKeyBoard;
	CString strTmp;

	dlgKeyBoard.m_strKeyBoardVal = _T("");

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		// TODO: Add your control notification handler code here
		m_strClientIp[2][1] = dlgKeyBoard.m_strKeyBoardVal;

		m_strClientIp[2][1].MakeUpper();
		m_strClientIp[2][1].TrimLeft(' ');               
		m_strClientIp[2][1].TrimRight(' ');

		OnInterfaceIpAddress(m_strClientIp[2][1]);
		m_clientIp3.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	}
}


void CWorkBarcode::OnBnClickedBtnClientPort3()
{
	CDialog_KeyPad dlgKeyPad;
	CString strTmp;

	dlgKeyPad.m_nKeypadMode			= 0;
	dlgKeyPad.m_strKeypadLowLimit	= _T("0");
	dlgKeyPad.m_strKeypadHighLimit	= _T("100000");
	dlgKeyPad.m_strKeypadVal.Format(_T("%d"),m_nClientPort[2][1]);
	//dlgKeyPad.m_strKeypadVal.Format(_T("%d"),m_nGmsServerPort[1]);

	if (dlgKeyPad.DoModal() == IDOK)
	{
		strTmp = dlgKeyPad.m_strNewVal;

		m_nClientPort[2][1] = _wtoi(strTmp);	
		//m_nGmsServerPort[1] = _wtoi(strTmp);	
		m_editClientPort3.SetWindowText(strTmp);
	}
}


void CWorkBarcode::OnBnClickedBtnClientIp6()
{
	CDialog_KeyBoard dlgKeyBoard;
	CString strTmp;

	dlgKeyBoard.m_strKeyBoardVal = _T("");

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		// TODO: Add your control notification handler code here
		m_strClientIp[6][1] = dlgKeyBoard.m_strKeyBoardVal;

		m_strClientIp[6][1].MakeUpper();
		m_strClientIp[6][1].TrimLeft(' ');               
		m_strClientIp[6][1].TrimRight(' ');

		OnInterfaceIpAddress(m_strClientIp[6][1]);

		m_clientIp6.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	}
}


void CWorkBarcode::OnBnClickedBtnClientPort6()
{

	::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG_8, CLIENT_SEND, 0);
	return;
	CDialog_KeyPad dlgKeyPad;
	CString strTmp;

	dlgKeyPad.m_nKeypadMode			= 0;
	dlgKeyPad.m_strKeypadLowLimit	= _T("0");
	dlgKeyPad.m_strKeypadHighLimit	= _T("100000");
	dlgKeyPad.m_strKeypadVal.Format(_T("%d"),m_nClientPort[6][1]);

	if (dlgKeyPad.DoModal() == IDOK)
	{
		strTmp = dlgKeyPad.m_strNewVal;

		m_nClientPort[6][1] = _wtoi(strTmp);	
		m_editClientPort6.SetWindowText(strTmp);
	}
}


void CWorkBarcode::OnBnClickedBtnBarcode1dClientIp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	


	CDialog_KeyBoard dlgKeyBoard;
	CString strTmp;

	dlgKeyBoard.m_strKeyBoardVal = _T("");

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		// TODO: Add your control notification handler code here
		m_strClientIp[7][1] = dlgKeyBoard.m_strKeyBoardVal;

		m_strClientIp[7][1].MakeUpper();
		m_strClientIp[7][1].TrimLeft(' ');               
		m_strClientIp[7][1].TrimRight(' ');

		OnInterfaceIpAddress(m_strClientIp[7][1]);

		m_Barcode_1d_client_ip.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	}
}

void CWorkBarcode::OnBnClickedBtnBarcode1dClientPort()
{
	CDialog_KeyPad dlgKeyPad;
	CString strTmp;

	dlgKeyPad.m_nKeypadMode			= 0;
	dlgKeyPad.m_strKeypadLowLimit	= _T("0");
	dlgKeyPad.m_strKeypadHighLimit	= _T("100000");
	dlgKeyPad.m_strKeypadVal.Format(_T("%d"),m_nClientPort[7][1]);

	if (dlgKeyPad.DoModal() == IDOK)
	{
		strTmp = dlgKeyPad.m_strNewVal;
		m_nClientPort[7][1] = _wtoi(strTmp);	
		m_edit_Barcode_1d_client_port.SetWindowText(strTmp);
	}
}
