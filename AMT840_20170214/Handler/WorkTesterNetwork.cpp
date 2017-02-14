// WorkTestNetwork.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "WorkTesterNetwork.h"
#include "afxdialogex.h"
#include "PublicFunction.h"
#include "Dialog_KeyBoard.h"
#include "Dialog_KeyPad.h"
#include "Dialog_Message.h"
#include "CtlBd_Variable.h"
#include "Comm.h"
#include ".\\Ctrl\\KeyBoard.h"
#include "LogFromat.h"


// CWorkInterface 대화 상자입니다.
//#define TM_BARCODE_1D		100
//#define TM_BARCODE_2D       200
// #define TM_RFID_READ		100
// #define TM_RFID_WRITE		200

IMPLEMENT_DYNAMIC(CWorkTesterNetwork, CDialog)

CWorkTesterNetwork::CWorkTesterNetwork(CWnd* pParent /*=NULL*/)
	: CDialog(CWorkTesterNetwork::IDD, pParent)
{
//	int i;

	
}

CWorkTesterNetwork::~CWorkTesterNetwork()
{
}

void CWorkTesterNetwork::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GROUP_SERVER1,			m_groupServer1);
	DDX_Control(pDX, IDC_GROUP_SERVER2,			m_groupServer2);
	DDX_Control(pDX, IDC_GROUP_SERVER3,			m_groupServer3);
	DDX_Control(pDX, IDC_GROUP_SERVER4,			m_groupServer4);
	DDX_Control(pDX, IDC_GROUP_SERVER5,			m_groupServer5);
	DDX_Control(pDX, IDC_GROUP_SERVER6,			m_groupServer6);
	DDX_Control(pDX, IDC_GROUP_SERVER7,			m_groupServer7);
	DDX_Control(pDX, IDC_GROUP_SERVER8,			m_groupServer8);

	DDX_Control(pDX, IDC_GROUP_CLIENT_1,			m_groupClient1);
	DDX_Control(pDX, IDC_GROUP_CLIENT_2,			m_groupClient2);
	DDX_Control(pDX, IDC_GROUP_CLIENT_3,			m_groupClient3);
	DDX_Control(pDX, IDC_GROUP_CLIENT_4,			m_groupClient4);
	DDX_Control(pDX, IDC_GROUP_CLIENT_5,			m_groupClient5);
	DDX_Control(pDX, IDC_GROUP_CLIENT_6,			m_groupClient6);
	DDX_Control(pDX, IDC_GROUP_CLIENT_7,			m_groupClient7);
	DDX_Control(pDX, IDC_GROUP_CLIENT_8,			m_groupClient8);

	DDX_Control(pDX, IDC_GROUP_COMMAND,			m_groupCommand);

// 	DDX_Control(pDX, IDC_MSG_SERVER_IP_1,			m_msgServerIp1);
// 	DDX_Control(pDX, IDC_MSG_SERVER_IP_2,			m_msgServerIp2);
// 	DDX_Control(pDX, IDC_MSG_SERVER_IP_3,			m_msgServerIp3);
// 	DDX_Control(pDX, IDC_MSG_SERVER_IP_4,			m_msgServerIp4);
// 	DDX_Control(pDX, IDC_MSG_SERVER_IP_5,			m_msgServerIp5);
// 	DDX_Control(pDX, IDC_MSG_SERVER_IP_6,			m_msgServerIp6);
// 	DDX_Control(pDX, IDC_MSG_SERVER_IP_7,			m_msgServerIp7);
// 	DDX_Control(pDX, IDC_MSG_SERVER_IP_8,			m_msgServerIp8);

	DDX_Control(pDX, IDC_MSG_CLIENT_IP_1,			m_msgClientIp1);
	DDX_Control(pDX, IDC_MSG_CLIENT_IP_2,			m_msgClientIp2);
	DDX_Control(pDX, IDC_MSG_CLIENT_IP_3,			m_msgClientIp3);
	DDX_Control(pDX, IDC_MSG_CLIENT_IP_4,			m_msgClientIp4);
	DDX_Control(pDX, IDC_MSG_CLIENT_IP_5,			m_msgClientIp5);
	DDX_Control(pDX, IDC_MSG_CLIENT_IP_6,			m_msgClientIp6);
	DDX_Control(pDX, IDC_MSG_CLIENT_IP_7,			m_msgClientIp7);
	DDX_Control(pDX, IDC_MSG_CLIENT_IP_8,			m_msgClientIp8);

	DDX_Control(pDX, IDC_MSG_SERVER_PORT1,			m_msgServerPort1);
	DDX_Control(pDX, IDC_MSG_SERVER_PORT2,			m_msgServerPort2);
	DDX_Control(pDX, IDC_MSG_SERVER_PORT3,			m_msgServerPort3);
	DDX_Control(pDX, IDC_MSG_SERVER_PORT4,			m_msgServerPort4);
	DDX_Control(pDX, IDC_MSG_SERVER_PORT5,			m_msgServerPort5);
	DDX_Control(pDX, IDC_MSG_SERVER_PORT6,			m_msgServerPort6);
	DDX_Control(pDX, IDC_MSG_SERVER_PORT7,			m_msgServerPort7);
	DDX_Control(pDX, IDC_MSG_SERVER_PORT8,			m_msgServerPort8);

	DDX_Control(pDX, IDC_MSG_CLIENT_PORT_1,			m_msgClientPort1);
	DDX_Control(pDX, IDC_MSG_CLIENT_PORT_2,			m_msgClientPort2);
	DDX_Control(pDX, IDC_MSG_CLIENT_PORT_3,			m_msgClientPort3);
	DDX_Control(pDX, IDC_MSG_CLIENT_PORT_4,			m_msgClientPort4);
	DDX_Control(pDX, IDC_MSG_CLIENT_PORT_5,			m_msgClientPort5);
	DDX_Control(pDX, IDC_MSG_CLIENT_PORT_6,			m_msgClientPort6);
	DDX_Control(pDX, IDC_MSG_CLIENT_PORT_7,			m_msgClientPort7);
	DDX_Control(pDX, IDC_MSG_CLIENT_PORT_8,			m_msgClientPort8);

// 	DDX_Control(pDX, IDC_MSG_SERVER_NAME_1,			m_msgServerName1);
// 	DDX_Control(pDX, IDC_MSG_SERVER_NAME_2,			m_msgServerName2);
// 	DDX_Control(pDX, IDC_MSG_SERVER_NAME_3,			m_msgServerName3);
// 	DDX_Control(pDX, IDC_MSG_SERVER_NAME_4,			m_msgServerName4);
// 	DDX_Control(pDX, IDC_MSG_SERVER_NAME_5,			m_msgServerName5);
// 	DDX_Control(pDX, IDC_MSG_SERVER_NAME_6,			m_msgServerName6);
// 	DDX_Control(pDX, IDC_MSG_SERVER_NAME_7,			m_msgServerName7);
// 	DDX_Control(pDX, IDC_MSG_SERVER_NAME_8,			m_msgServerName8);

	DDX_Control(pDX, IDC_MSG_CLIENT_NAME_1,			m_msgClientName1);
	DDX_Control(pDX, IDC_MSG_CLIENT_NAME_2,			m_msgClientName2);
	DDX_Control(pDX, IDC_MSG_CLIENT_NAME_3,			m_msgClientName3);
	DDX_Control(pDX, IDC_MSG_CLIENT_NAME_4,			m_msgClientName4);
	DDX_Control(pDX, IDC_MSG_CLIENT_NAME_5,			m_msgClientName5);
	DDX_Control(pDX, IDC_MSG_CLIENT_NAME_6,			m_msgClientName6);
	DDX_Control(pDX, IDC_MSG_CLIENT_NAME_7,			m_msgClientName7);
	DDX_Control(pDX, IDC_MSG_CLIENT_NAME_8,			m_msgClientName8);

	DDX_Control(pDX, IDC_CLIENT_IP_1,				m_clientIp1);
	DDX_Control(pDX, IDC_CLIENT_IP_2,				m_clientIp2);
	DDX_Control(pDX, IDC_CLIENT_IP_3,				m_clientIp3);
	DDX_Control(pDX, IDC_CLIENT_IP_4,				m_clientIp4);
	DDX_Control(pDX, IDC_CLIENT_IP_5,				m_clientIp5);
	DDX_Control(pDX, IDC_CLIENT_IP_6,				m_clientIp6);
	DDX_Control(pDX, IDC_CLIENT_IP_7,				m_clientIp7);
	DDX_Control(pDX, IDC_CLIENT_IP_8,				m_clientIp8);

// 	DDX_Control(pDX, IDC_SERVER_IP_1,				m_ServerIp1);
// 	DDX_Control(pDX, IDC_SERVER_IP_2,				m_ServerIp2);
// 	DDX_Control(pDX, IDC_SERVER_IP_3,				m_ServerIp3);
// 	DDX_Control(pDX, IDC_SERVER_IP_4,				m_ServerIp4);
// 	DDX_Control(pDX, IDC_SERVER_IP_5,				m_ServerIp5);
// 	DDX_Control(pDX, IDC_SERVER_IP_6,				m_ServerIp6);
// 	DDX_Control(pDX, IDC_SERVER_IP_7,				m_ServerIp7);
// 	DDX_Control(pDX, IDC_SERVER_IP_8,				m_ServerIp8);


// 	DDX_Control(pDX, IDC_EDIT_CLIENT_PORT_1,	m_editClientPort1);
// 	DDX_Control(pDX, IDC_EDIT_CLIENT_PORT_2,	m_editClientPort2);
// 	DDX_Control(pDX, IDC_EDIT_CLIENT_PORT_3,	m_editClientPort3);
// 	DDX_Control(pDX, IDC_EDIT_CLIENT_PORT_4,	m_editClientPort4);
// 	DDX_Control(pDX, IDC_EDIT_CLIENT_PORT_5,	m_editClientPort5);
// 	DDX_Control(pDX, IDC_EDIT_CLIENT_PORT_6,	m_editClientPort6);
// 	DDX_Control(pDX, IDC_EDIT_CLIENT_PORT_7,	m_editClientPort7);
// 	DDX_Control(pDX, IDC_EDIT_CLIENT_PORT_8,	m_editClientPort8);


// 	DDX_Control(pDX, IDC_EDIT_SERVER_PORT1,		m_editServerPort1);
// 	DDX_Control(pDX, IDC_EDIT_SERVER_PORT2,		m_editServerPort2);
// 	DDX_Control(pDX, IDC_EDIT_SERVER_PORT3,		m_editServerPort3);
// 	DDX_Control(pDX, IDC_EDIT_SERVER_PORT4,		m_editServerPort4);
// 	DDX_Control(pDX, IDC_EDIT_SERVER_PORT5,		m_editServerPort5);
// 	DDX_Control(pDX, IDC_EDIT_SERVER_PORT6,		m_editServerPort6);
// 	DDX_Control(pDX, IDC_EDIT_SERVER_PORT7,		m_editServerPort7);
// 	DDX_Control(pDX, IDC_EDIT_SERVER_PORT8,		m_editServerPort8);


// 	DDX_Control(pDX, IDC_EDIT_CLIENT_NAME_1,	m_editClientName1);
// 	DDX_Control(pDX, IDC_EDIT_CLIENT_NAME_2,	m_editClientName2);
// 	DDX_Control(pDX, IDC_EDIT_CLIENT_NAME_3,	m_editClientName3);
// 	DDX_Control(pDX, IDC_EDIT_CLIENT_NAME_4,	m_editClientName4);
// 	DDX_Control(pDX, IDC_EDIT_CLIENT_NAME_5,	m_editClientName5);
// 	DDX_Control(pDX, IDC_EDIT_CLIENT_NAME_6,	m_editClientName6);
// 	DDX_Control(pDX, IDC_EDIT_CLIENT_NAME_7,	m_editClientName7);
// 	DDX_Control(pDX, IDC_EDIT_CLIENT_NAME_8,	m_editClientName8);
// 
// 
// 	DDX_Control(pDX, IDC_EDIT_SERVER_NAME_1,		m_editServerName1);
// 	DDX_Control(pDX, IDC_EDIT_SERVER_NAME_2,		m_editServerName2);
// 	DDX_Control(pDX, IDC_EDIT_SERVER_NAME_3,		m_editServerName3);
// 	DDX_Control(pDX, IDC_EDIT_SERVER_NAME_4,		m_editServerName4);
// 	DDX_Control(pDX, IDC_EDIT_SERVER_NAME_5,		m_editServerName5);
// 	DDX_Control(pDX, IDC_EDIT_SERVER_NAME_6,		m_editServerName6);
// 	DDX_Control(pDX, IDC_EDIT_SERVER_NAME_7,		m_editServerName7);
// 	DDX_Control(pDX, IDC_EDIT_SERVER_NAME_8,		m_editServerName8);


	DDX_Control(pDX, IDC_BTN_APPLY,					m_btnApply);
	DDX_Control(pDX, IDC_BTN_RELOAD,				m_btnReLoad);

 	DDX_Control(pDX, IDC_BTN_CLIENT_IP_1,			m_btnClientIpSet[0]);
// 	DDX_Control(pDX, IDC_BTN_CLIENT_PORT_1,			m_btnClientPortSet[0]);
 	DDX_Control(pDX, IDC_BTN_CLIENT_IP_2,			m_btnClientIpSet[1]);
// 	DDX_Control(pDX, IDC_BTN_CLIENT_PORT_2,			m_btnClientPortSet[1]);
 	DDX_Control(pDX, IDC_BTN_CLIENT_IP_3,			m_btnClientIpSet[2]);
// 	DDX_Control(pDX, IDC_BTN_CLIENT_PORT_3,			m_btnClientPortSet[2]);
 	DDX_Control(pDX, IDC_BTN_CLIENT_IP_4,			m_btnClientIpSet[3]);
// 	DDX_Control(pDX, IDC_BTN_CLIENT_PORT_4,			m_btnClientPortSet[3]);
 	DDX_Control(pDX, IDC_BTN_CLIENT_IP_5,			m_btnClientIpSet[4]);
// 	DDX_Control(pDX, IDC_BTN_CLIENT_PORT_5,			m_btnClientPortSet[4]);
 	DDX_Control(pDX, IDC_BTN_CLIENT_IP_6,			m_btnClientIpSet[5]);
// 	DDX_Control(pDX, IDC_BTN_CLIENT_PORT_6,			m_btnClientPortSet[5]);
 	DDX_Control(pDX, IDC_BTN_CLIENT_IP_7,			m_btnClientIpSet[6]);
// 	DDX_Control(pDX, IDC_BTN_CLIENT_PORT_7,			m_btnClientPortSet[6]);
 	DDX_Control(pDX, IDC_BTN_CLIENT_IP_8,			m_btnClientIpSet[7]);
// 	DDX_Control(pDX, IDC_BTN_CLIENT_PORT_8,			m_btnClientPortSet[7]);
 	DDX_Control(pDX, IDC_BTN_ECSERVER_IP,			m_btnClientIpSet[8]);
 	DDX_Control(pDX, IDC_BTN_XGEM_IP,				m_btnClientIpSet[9]);

// 	DDX_Control(pDX, IDC_BTN_SERVER_IP_1,			m_btnServerIpSet[0]);
// 	DDX_Control(pDX, IDC_BTN_SERVER_PORT1,			m_btnServerPortSet[0]);
// 	DDX_Control(pDX, IDC_BTN_SERVER_IP_2,			m_btnServerIpSet[1]);
// 	DDX_Control(pDX, IDC_BTN_SERVER_PORT2,			m_btnServerPortSet[1]);
// 	DDX_Control(pDX, IDC_BTN_SERVER_IP_3,			m_btnServerIpSet[2]);
// 	DDX_Control(pDX, IDC_BTN_SERVER_PORT3,			m_btnServerPortSet[2]);
// 	DDX_Control(pDX, IDC_BTN_SERVER_IP_4,			m_btnServerIpSet[3]);
// 	DDX_Control(pDX, IDC_BTN_SERVER_PORT4,			m_btnServerPortSet[3]);
// 	DDX_Control(pDX, IDC_BTN_SERVER_IP_5,			m_btnServerIpSet[4]);
// 	DDX_Control(pDX, IDC_BTN_SERVER_PORT5,			m_btnServerPortSet[4]);
// 	DDX_Control(pDX, IDC_BTN_SERVER_IP_6,			m_btnServerIpSet[5]);
// 	DDX_Control(pDX, IDC_BTN_SERVER_PORT6,			m_btnServerPortSet[5]);
// 	DDX_Control(pDX, IDC_BTN_SERVER_IP_7,			m_btnServerIpSet[6]);
// 	DDX_Control(pDX, IDC_BTN_SERVER_PORT7,			m_btnServerPortSet[6]);
// 	DDX_Control(pDX, IDC_BTN_SERVER_IP_8,			m_btnServerIpSet[7]);
// 	DDX_Control(pDX, IDC_BTN_SERVER_PORT8,			m_btnServerPortSet[7]);
	//DDX_Control(pDX, IDC_BTN_SERVER_PORT_1,			m_btnServerPortSet[0]);
	//	DDX_Control(pDX, IDC_CHECK_UNLOAD_BARCODE_A,	m_btnUnldBcr[0]);
	//DDX_Control(pDX, IDC_CHECK_UNLOAD_BARCODE_B,	m_btnUnldBcr[1]);



	DDX_Control(pDX, IDC_MSG_SERVER_PORT_DATA_1, m_msgServerPortData_1);
	DDX_Control(pDX, IDC_MSG_SERVER_PORT_DATA_2, m_msgServerPortData_2);
	DDX_Control(pDX, IDC_MSG_SERVER_PORT_DATA_3, m_msgServerPortData_3);
	DDX_Control(pDX, IDC_MSG_SERVER_PORT_DATA_4, m_msgServerPortData_4);
	DDX_Control(pDX, IDC_MSG_SERVER_PORT_DATA_5, m_msgServerPortData_5);
	DDX_Control(pDX, IDC_MSG_SERVER_PORT_DATA_6, m_msgServerPortData_6);
	DDX_Control(pDX, IDC_MSG_SERVER_PORT_DATA_7, m_msgServerPortData_7);
	DDX_Control(pDX, IDC_MSG_SERVER_PORT_DATA_8, m_msgServerPortData_8);
// 	DDX_Control(pDX, IDC_MSG_SERVER_ID_1, m_msgServerID_1);
// 	DDX_Control(pDX, IDC_MSG_SERVER_ID_2, m_msgServerID_2);
// 	DDX_Control(pDX, IDC_MSG_SERVER_ID_3, m_msgServerID_3);
// 	DDX_Control(pDX, IDC_MSG_SERVER_ID_4, m_msgServerID_4);
// 	DDX_Control(pDX, IDC_MSG_SERVER_ID_5, m_msgServerID_5);
// 	DDX_Control(pDX, IDC_MSG_SERVER_ID_6, m_msgServerID_6);
// 	DDX_Control(pDX, IDC_MSG_SERVER_ID_7, m_msgServerID_7);
	DDX_Control(pDX, IDC_MSG_CLIENT_PORT_DATA_1, m_msgClientPortData_1);
	DDX_Control(pDX, IDC_MSG_CLIENT_PORT_DATA_2, m_msgClientPortData_2);
	DDX_Control(pDX, IDC_MSG_CLIENT_PORT_DATA_3, m_msgClientPortData_3);
	DDX_Control(pDX, IDC_MSG_CLIENT_PORT_DATA_4, m_msgClientPortData_4);
	DDX_Control(pDX, IDC_MSG_CLIENT_PORT_DATA_5, m_msgClientPortData_5);
	DDX_Control(pDX, IDC_MSG_CLIENT_PORT_DATA_6, m_msgClientPortData_6);
	DDX_Control(pDX, IDC_MSG_CLIENT_PORT_DATA_7, m_msgClientPortData_7);
	DDX_Control(pDX, IDC_MSG_CLIENT_PORT_DATA_8, m_msgClientPortData_8);
	DDX_Control(pDX, IDC_MSG_CLIENT_ID_1, m_msgClientID_1);
	DDX_Control(pDX, IDC_MSG_CLIENT_ID_3, m_msgClientID_3);
	DDX_Control(pDX, IDC_MSG_CLIENT_ID_2, m_msgClientID_2);
	DDX_Control(pDX, IDC_MSG_CLIENT_ID_4, m_msgClientID_4);
	DDX_Control(pDX, IDC_MSG_CLIENT_ID_5, m_msgClientID_5);
	DDX_Control(pDX, IDC_MSG_CLIENT_ID_6, m_msgClientID_6);
	DDX_Control(pDX, IDC_MSG_CLIENT_ID_7, m_msgClientID_7);
	DDX_Control(pDX, IDC_MSG_CLIENT_ID_8, m_msgClientID_8);
	DDX_Control(pDX, IDC_MSG_SERVER_ID_8, m_msgServerID_8);
	DDX_Control(pDX, IDC_GROUP_ECSERVER, m_groupEcServer);
	DDX_Control(pDX, IDC_GROUP_XGEM, m_groupXgem);
	DDX_Control(pDX, IDC_MSG_ECSERVER_IP, m_msgEcServerIP);
	DDX_Control(pDX, IDC_ECSERVER_IP, m_EcServerIp);
	DDX_Control(pDX, IDC_MSG_ECSERVER_PORT_DATA, m_msgEcServerPortData);
	DDX_Control(pDX, IDC_MSG_XGEM_IP, m_msgXgemIp);
	DDX_Control(pDX, IDC_MSG_ECSERVER_PORT, m_msgEcServerPort);
	DDX_Control(pDX, IDC_MSG_XGEM_PORT, m_msgXgemPort);
	DDX_Control(pDX, IDC_MSG_XGEM_PORT_DATA, m_msgXgemPortData);
	DDX_Control(pDX, IDC_XGEM_IP, m_XgemIp);
	DDX_Control(pDX, IDC_GROUP_TSITE_SERIAL, m_groupTsiteSerial);
	DDX_Control(pDX, IDC_GROUP_TSITE_1, m_groupTsiteSerial_1);
	DDX_Control(pDX, IDC_GROUP_TSITE_2, m_groupTsiteSerial_2);
	DDX_Control(pDX, IDC_GROUP_TSITE_3, m_groupTsiteSerial_3);
	DDX_Control(pDX, IDC_GROUP_TSITE_4, m_groupTsiteSerial_4);
	DDX_Control(pDX, IDC_GROUP_TSITE_5, m_groupTsiteSerial_5);
	DDX_Control(pDX, IDC_GROUP_TSITE_6, m_groupTsiteSerial_6);
	DDX_Control(pDX, IDC_GROUP_TSITE_7, m_groupTsiteSerial_7);
	DDX_Control(pDX, IDC_GROUP_TSITE_8, m_groupTsiteSerial_8);
	DDX_Control(pDX, IDC_MSG_SERIAL_NAME_1_1, m_msgSerialName_1_1);
	DDX_Control(pDX, IDC_MSG_SERIAL_NAME_1_2, m_msgSerialName_1_2);
	DDX_Control(pDX, IDC_MSG_SERIAL_NAME_2_1, m_msgSerialName_2_1);
	DDX_Control(pDX, IDC_MSG_SERIAL_NAME_2_2, m_msgSerialName_2_2);
	DDX_Control(pDX, IDC_MSG_SERIAL_NAME_3_1, m_msgSerialName_3_1);
	DDX_Control(pDX, IDC_MSG_SERIAL_NAME_3_2, m_msgSerialName_3_2);
	DDX_Control(pDX, IDC_MSG_SERIAL_NAME_4_1, m_msgSerialName_4_1);
	DDX_Control(pDX, IDC_MSG_SERIAL_NAME_4_2, m_msgSerialName_4_2);
	DDX_Control(pDX, IDC_MSG_SERIAL_NAME_5_1, m_msgSerialName_5_1);
	DDX_Control(pDX, IDC_MSG_SERIAL_NAME_5_2, m_msgSerialName_5_2);
	DDX_Control(pDX, IDC_MSG_SERIAL_NAME_6_1, m_msgSerialName_6_1);
	DDX_Control(pDX, IDC_MSG_SERIAL_NAME_6_2, m_msgSerialName_6_2);
	DDX_Control(pDX, IDC_MSG_SERIAL_NAME_7_1, m_msgSerialName_7_1);
	DDX_Control(pDX, IDC_MSG_SERIAL_NAME_7_2, m_msgSerialName_7_2);
	DDX_Control(pDX, IDC_MSG_SERIAL_NAME_8_1, m_msgSerialName_8_1);
	DDX_Control(pDX, IDC_MSG_SERIAL_NAME_8_2, m_msgSerialName_8_2);
	DDX_Control(pDX, IDC_MSG_SERIAL_ID_1_1, m_msgSerialID_1_1);
	DDX_Control(pDX, IDC_MSG_SERIAL_ID_1_2, m_msgSerialID_1_2);
	DDX_Control(pDX, IDC_MSG_SERIAL_ID_2_1, m_msgSerialID_2_1);
	DDX_Control(pDX, IDC_MSG_SERIAL_ID_2_2, m_msgSerialID_2_2);
	DDX_Control(pDX, IDC_MSG_SERIAL_ID_3_1, m_msgSerialID_3_1);
	DDX_Control(pDX, IDC_MSG_SERIAL_ID_3_2, m_msgSerialID_3_2);
	DDX_Control(pDX, IDC_MSG_SERIAL_ID_4_1, m_msgSerialID_4_1);
	DDX_Control(pDX, IDC_MSG_SERIAL_ID_4_2, m_msgSerialID_4_2);
	DDX_Control(pDX, IDC_MSG_SERIAL_ID_5_1, m_msgSerialID_5_1);
	DDX_Control(pDX, IDC_MSG_SERIAL_ID_5_2, m_msgSerialID_5_2);
	DDX_Control(pDX, IDC_MSG_SERIAL_ID_6_1, m_msgSerialID_6_1);
	DDX_Control(pDX, IDC_MSG_SERIAL_ID_6_2, m_msgSerialID_6_2);
	DDX_Control(pDX, IDC_MSG_SERIAL_ID_7_1, m_msgSerialID_7_1);
	DDX_Control(pDX, IDC_MSG_SERIAL_ID_7_2, m_msgSerialID_7_2);
	DDX_Control(pDX, IDC_MSG_SERIAL_ID_8_1, m_msgSerialID_8_1);
	DDX_Control(pDX, IDC_MSG_SERIAL_ID_8_2, m_msgSerialID_8_2);
}


BEGIN_MESSAGE_MAP(CWorkTesterNetwork, CDialog)
	ON_WM_ERASEBKGND()


	ON_BN_CLICKED(IDC_BTN_APPLY,&CWorkTesterNetwork::OnBnClickedBtnApply)
	ON_BN_CLICKED(IDC_BTN_RELOAD,&CWorkTesterNetwork::OnBnClickedBtnReload)
	ON_BN_CLICKED(IDC_BTN_SERVER_IP_1, &CWorkTesterNetwork::OnBnClickedBtnServerIp1)
	ON_BN_CLICKED(IDC_BTN_SERVER_PORT1, &CWorkTesterNetwork::OnBnClickedBtnServerPort1)
	ON_BN_CLICKED(IDC_BTN_SERVER_IP_2, &CWorkTesterNetwork::OnBnClickedBtnServerIp2)
	ON_BN_CLICKED(IDC_BTN_SERVER_PORT2, &CWorkTesterNetwork::OnBnClickedBtnServerPort2)
	ON_BN_CLICKED(IDC_BTN_SERVER_IP_3, &CWorkTesterNetwork::OnBnClickedBtnServerIp3)
	ON_BN_CLICKED(IDC_BTN_SERVER_PORT3, &CWorkTesterNetwork::OnBnClickedBtnServerPort3)
	ON_BN_CLICKED(IDC_BTN_SERVER_IP_4, &CWorkTesterNetwork::OnBnClickedBtnServerIp4)
	ON_BN_CLICKED(IDC_BTN_SERVER_PORT4, &CWorkTesterNetwork::OnBnClickedBtnServerPort4)
	ON_BN_CLICKED(IDC_BTN_SERVER_IP_5, &CWorkTesterNetwork::OnBnClickedBtnServerIp5)
	ON_BN_CLICKED(IDC_BTN_SERVER_PORT5, &CWorkTesterNetwork::OnBnClickedBtnServerPort5)
	ON_BN_CLICKED(IDC_BTN_SERVER_IP_6, &CWorkTesterNetwork::OnBnClickedBtnServerIp6)
	ON_BN_CLICKED(IDC_BTN_SERVER_PORT6, &CWorkTesterNetwork::OnBnClickedBtnServerPort6)
	ON_BN_CLICKED(IDC_BTN_SERVER_IP_7, &CWorkTesterNetwork::OnBnClickedBtnServerIp7)
	ON_BN_CLICKED(IDC_BTN_SERVER_PORT7, &CWorkTesterNetwork::OnBnClickedBtnServerPort7)
	ON_BN_CLICKED(IDC_BTN_SERVER_IP_8, &CWorkTesterNetwork::OnBnClickedBtnServerIp8)
	ON_BN_CLICKED(IDC_BTN_SERVER_PORT8, &CWorkTesterNetwork::OnBnClickedBtnServerPort8)
	ON_BN_CLICKED(IDC_BTN_CLIENT_IP_1, &CWorkTesterNetwork::OnBnClickedBtnClientIp1)
	ON_BN_CLICKED(IDC_BTN_CLIENT_PORT_1, &CWorkTesterNetwork::OnBnClickedBtnClientPort1)
	ON_BN_CLICKED(IDC_BTN_CLIENT_IP_2, &CWorkTesterNetwork::OnBnClickedBtnClientIp2)
	ON_BN_CLICKED(IDC_BTN_CLIENT_PORT_2, &CWorkTesterNetwork::OnBnClickedBtnClientPort2)
	ON_BN_CLICKED(IDC_BTN_CLIENT_IP_3, &CWorkTesterNetwork::OnBnClickedBtnClientIp3)
	ON_BN_CLICKED(IDC_BTN_CLIENT_PORT_3, &CWorkTesterNetwork::OnBnClickedBtnClientPort3)
	ON_BN_CLICKED(IDC_BTN_CLIENT_IP_4, &CWorkTesterNetwork::OnBnClickedBtnClientIp4)
	ON_BN_CLICKED(IDC_BTN_CLIENT_PORT_4, &CWorkTesterNetwork::OnBnClickedBtnClientPort4)
	ON_BN_CLICKED(IDC_BTN_CLIENT_IP_5, &CWorkTesterNetwork::OnBnClickedBtnClientIp5)
	ON_BN_CLICKED(IDC_BTN_CLIENT_PORT_5, &CWorkTesterNetwork::OnBnClickedBtnClientPort5)
	ON_BN_CLICKED(IDC_BTN_CLIENT_IP_6, &CWorkTesterNetwork::OnBnClickedBtnClientIp6)
	ON_BN_CLICKED(IDC_BTN_CLIENT_PORT_6, &CWorkTesterNetwork::OnBnClickedBtnClientPort6)
	ON_BN_CLICKED(IDC_BTN_CLIENT_IP_7, &CWorkTesterNetwork::OnBnClickedBtnClientIp7)
	ON_BN_CLICKED(IDC_BTN_CLIENT_PORT_7, &CWorkTesterNetwork::OnBnClickedBtnClientPort7)
	ON_BN_CLICKED(IDC_BTN_CLIENT_IP_8, &CWorkTesterNetwork::OnBnClickedBtnClientIp8)
	ON_BN_CLICKED(IDC_BTN_CLIENT_PORT_8, &CWorkTesterNetwork::OnBnClickedBtnClientPort8)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_EDIT_SERVER_PORT_4, &CWorkTesterNetwork::OnEnChangeEditServerPort4)
	ON_EN_CHANGE(IDC_EDIT_SERVER_NAME_1, &CWorkTesterNetwork::OnEnChangeEditServerName1)
	ON_STN_CLICKED(IDC_MSG_SERVER_PORT_DATA_1, &CWorkTesterNetwork::OnStnClickedMsgServerPortData1)
	ON_STN_CLICKED(IDC_MSG_SERVER_PORT_DATA_2, &CWorkTesterNetwork::OnStnClickedMsgServerPortData2)
	ON_STN_CLICKED(IDC_MSG_SERVER_PORT_DATA_3, &CWorkTesterNetwork::OnStnClickedMsgServerPortData3)
	ON_STN_CLICKED(IDC_MSG_SERVER_PORT_DATA_4, &CWorkTesterNetwork::OnStnClickedMsgServerPortData4)
	ON_STN_CLICKED(IDC_MSG_SERVER_PORT_DATA_5, &CWorkTesterNetwork::OnStnClickedMsgServerPortData5)
	ON_STN_CLICKED(IDC_MSG_SERVER_PORT_DATA_6, &CWorkTesterNetwork::OnStnClickedMsgServerPortData6)
	ON_STN_CLICKED(IDC_MSG_SERVER_PORT_DATA_7, &CWorkTesterNetwork::OnStnClickedMsgServerPortData7)
	ON_STN_CLICKED(IDC_MSG_SERVER_PORT_DATA_8, &CWorkTesterNetwork::OnStnClickedMsgServerPortData8)
	ON_STN_CLICKED(IDC_MSG_SERVER_ID_1, &CWorkTesterNetwork::OnStnClickedMsgServerId1)
	ON_STN_CLICKED(IDC_MSG_SERVER_ID_2, &CWorkTesterNetwork::OnStnClickedMsgServerId2)
	ON_STN_CLICKED(IDC_MSG_SERVER_ID_3, &CWorkTesterNetwork::OnStnClickedMsgServerId3)
	ON_STN_CLICKED(IDC_MSG_SERVER_ID_4, &CWorkTesterNetwork::OnStnClickedMsgServerId4)
	ON_STN_CLICKED(IDC_MSG_SERVER_ID_5, &CWorkTesterNetwork::OnStnClickedMsgServerId5)
	ON_STN_CLICKED(IDC_MSG_SERVER_ID_6, &CWorkTesterNetwork::OnStnClickedMsgServerId6)
	ON_STN_CLICKED(IDC_MSG_SERVER_ID_7, &CWorkTesterNetwork::OnStnClickedMsgServerId7)
	ON_STN_CLICKED(IDC_MSG_SERVER_ID_8, &CWorkTesterNetwork::OnStnClickedMsgServerId8)
	ON_STN_CLICKED(IDC_MSG_CLIENT_PORT_DATA_1, &CWorkTesterNetwork::OnStnClickedMsgClientPortData1)
	ON_STN_CLICKED(IDC_MSG_CLIENT_ID_1, &CWorkTesterNetwork::OnStnClickedMsgClientId1)
	ON_STN_CLICKED(IDC_MSG_CLIENT_PORT_DATA_2, &CWorkTesterNetwork::OnStnClickedMsgClientPortData2)
	ON_STN_CLICKED(IDC_MSG_CLIENT_PORT_DATA_3, &CWorkTesterNetwork::OnStnClickedMsgClientPortData3)
	ON_STN_CLICKED(IDC_MSG_CLIENT_PORT_DATA_4, &CWorkTesterNetwork::OnStnClickedMsgClientPortData4)
	ON_STN_CLICKED(IDC_MSG_CLIENT_PORT_DATA_5, &CWorkTesterNetwork::OnStnClickedMsgClientPortData5)
	ON_STN_CLICKED(IDC_MSG_CLIENT_PORT_DATA_6, &CWorkTesterNetwork::OnStnClickedMsgClientPortData6)
	ON_STN_CLICKED(IDC_MSG_CLIENT_PORT_DATA_7, &CWorkTesterNetwork::OnStnClickedMsgClientPortData7)
	ON_STN_CLICKED(IDC_MSG_CLIENT_PORT_DATA_8, &CWorkTesterNetwork::OnStnClickedMsgClientPortData8)
	ON_STN_CLICKED(IDC_MSG_CLIENT_ID_2, &CWorkTesterNetwork::OnStnClickedMsgClientId2)
	ON_STN_CLICKED(IDC_MSG_CLIENT_ID_3, &CWorkTesterNetwork::OnStnClickedMsgClientId3)
	ON_STN_CLICKED(IDC_MSG_CLIENT_ID_4, &CWorkTesterNetwork::OnStnClickedMsgClientId4)
	ON_STN_CLICKED(IDC_MSG_CLIENT_ID_5, &CWorkTesterNetwork::OnStnClickedMsgClientId5)
	ON_STN_CLICKED(IDC_MSG_CLIENT_ID_6, &CWorkTesterNetwork::OnStnClickedMsgClientId6)
	ON_STN_CLICKED(IDC_MSG_CLIENT_ID_7, &CWorkTesterNetwork::OnStnClickedMsgClientId7)
	ON_STN_CLICKED(IDC_MSG_CLIENT_ID_8, &CWorkTesterNetwork::OnStnClickedMsgClientId8)
	ON_STN_CLICKED(IDC_MSG_ECSERVER_PORT_DATA, &CWorkTesterNetwork::OnStnClickedMsgEcserverPortData)
	ON_STN_CLICKED(IDC_MSG_XGEM_PORT_DATA, &CWorkTesterNetwork::OnStnClickedMsgXgemPortData)
	ON_BN_CLICKED(IDC_BTN_ECSERVER_IP, &CWorkTesterNetwork::OnBnClickedBtnEcserverIp)
	ON_BN_CLICKED(IDC_BTN_XGEM_IP, &CWorkTesterNetwork::OnBnClickedBtnXgemIp)
	ON_STN_CLICKED(IDC_MSG_SERIAL_ID_1_1, &CWorkTesterNetwork::OnStnClickedMsgSerialId11)
	ON_STN_CLICKED(IDC_MSG_SERIAL_ID_1_2, &CWorkTesterNetwork::OnStnClickedMsgSerialId12)
	ON_STN_CLICKED(IDC_MSG_SERIAL_ID_2_1, &CWorkTesterNetwork::OnStnClickedMsgSerialId21)
	ON_STN_CLICKED(IDC_MSG_SERIAL_ID_2_2, &CWorkTesterNetwork::OnStnClickedMsgSerialId22)
	ON_STN_CLICKED(IDC_MSG_SERIAL_ID_3_1, &CWorkTesterNetwork::OnStnClickedMsgSerialId31)
	ON_STN_CLICKED(IDC_MSG_SERIAL_ID_3_2, &CWorkTesterNetwork::OnStnClickedMsgSerialId32)
	ON_STN_CLICKED(IDC_MSG_SERIAL_ID_4_1, &CWorkTesterNetwork::OnStnClickedMsgSerialId41)
	ON_STN_CLICKED(IDC_MSG_SERIAL_ID_4_2, &CWorkTesterNetwork::OnStnClickedMsgSerialId42)
	ON_STN_CLICKED(IDC_MSG_SERIAL_ID_5_1, &CWorkTesterNetwork::OnStnClickedMsgSerialId51)
	ON_STN_CLICKED(IDC_MSG_SERIAL_ID_5_2, &CWorkTesterNetwork::OnStnClickedMsgSerialId52)
	ON_STN_CLICKED(IDC_MSG_SERIAL_ID_6_1, &CWorkTesterNetwork::OnStnClickedMsgSerialId61)
	ON_STN_CLICKED(IDC_MSG_SERIAL_ID_6_2, &CWorkTesterNetwork::OnStnClickedMsgSerialId62)
	ON_STN_CLICKED(IDC_MSG_SERIAL_ID_7_1, &CWorkTesterNetwork::OnStnClickedMsgSerialId71)
	ON_STN_CLICKED(IDC_MSG_SERIAL_ID_7_2, &CWorkTesterNetwork::OnStnClickedMsgSerialId72)
	ON_STN_CLICKED(IDC_MSG_SERIAL_ID_8_1, &CWorkTesterNetwork::OnStnClickedMsgSerialId81)
	ON_STN_CLICKED(IDC_MSG_SERIAL_ID_8_2, &CWorkTesterNetwork::OnStnClickedMsgSerialId82)
END_MESSAGE_MAP()

BOOL CWorkTesterNetwork::DestroyWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
/*
	if (OnDataComparison() == RET_ERROR)
	{
		OnDataHistoryLog();
		OnDataApply();
	}
*/
	return CDialog::DestroyWindow();
}


BOOL CWorkTesterNetwork::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	OnDataInit();
	OnInitGroupBox();
	OnInitLabel();
	OnInitButton();
	OnInitName();
	OnInterface_Display();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CWorkTesterNetwork::OnInitGroupBox()
{
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

	m_groupClient4.SetFont(clsFunc.OnLogFont(16));
	m_groupClient4.SetCatptionTextColor(BLUE_C);
	m_groupClient4.SetBorderColor(ORANGE_C);
	m_groupClient4.SetBackgroundColor(WINDOW_UP1);
	m_groupClient4.SetFontBold(TRUE);

	m_groupClient5.SetFont(clsFunc.OnLogFont(16));
	m_groupClient5.SetCatptionTextColor(BLUE_C);
	m_groupClient5.SetBorderColor(ORANGE_C);
	m_groupClient5.SetBackgroundColor(WINDOW_UP1);
	m_groupClient5.SetFontBold(TRUE);
	
	m_groupClient6.SetFont(clsFunc.OnLogFont(16));
	m_groupClient6.SetCatptionTextColor(BLUE_C);
	m_groupClient6.SetBorderColor(ORANGE_C);
	m_groupClient6.SetBackgroundColor(WINDOW_UP1);
	m_groupClient6.SetFontBold(TRUE);

	m_groupClient7.SetFont(clsFunc.OnLogFont(16));
	m_groupClient7.SetCatptionTextColor(BLUE_C);
	m_groupClient7.SetBorderColor(ORANGE_C);
	m_groupClient7.SetBackgroundColor(WINDOW_UP1);
	m_groupClient7.SetFontBold(TRUE);

	m_groupClient8.SetFont(clsFunc.OnLogFont(16));
	m_groupClient8.SetCatptionTextColor(BLUE_C);
	m_groupClient8.SetBorderColor(ORANGE_C);
	m_groupClient8.SetBackgroundColor(WINDOW_UP1);
	m_groupClient8.SetFontBold(TRUE);

	m_groupServer1.SetFont(clsFunc.OnLogFont(16));
	m_groupServer1.SetCatptionTextColor(BLUE_C);
	m_groupServer1.SetBorderColor(ORANGE_C);
	m_groupServer1.SetBackgroundColor(WINDOW_UP1);
	m_groupServer1.SetFontBold(TRUE);

	m_groupServer2.SetFont(clsFunc.OnLogFont(16));
	m_groupServer2.SetCatptionTextColor(BLUE_C);
	m_groupServer2.SetBorderColor(ORANGE_C);
	m_groupServer2.SetBackgroundColor(WINDOW_UP1);
	m_groupServer2.SetFontBold(TRUE);

	m_groupServer3.SetFont(clsFunc.OnLogFont(16));
	m_groupServer3.SetCatptionTextColor(BLUE_C);
	m_groupServer3.SetBorderColor(ORANGE_C);
	m_groupServer3.SetBackgroundColor(WINDOW_UP1);
	m_groupServer3.SetFontBold(TRUE);

	m_groupServer4.SetFont(clsFunc.OnLogFont(16));
	m_groupServer4.SetCatptionTextColor(BLUE_C);
	m_groupServer4.SetBorderColor(ORANGE_C);
	m_groupServer4.SetBackgroundColor(WINDOW_UP1);
	m_groupServer4.SetFontBold(TRUE);

	m_groupServer5.SetFont(clsFunc.OnLogFont(16));
	m_groupServer5.SetCatptionTextColor(BLUE_C);
	m_groupServer5.SetBorderColor(ORANGE_C);
	m_groupServer5.SetBackgroundColor(WINDOW_UP1);
	m_groupServer5.SetFontBold(TRUE);

	m_groupServer6.SetFont(clsFunc.OnLogFont(16));
	m_groupServer6.SetCatptionTextColor(BLUE_C);
	m_groupServer6.SetBorderColor(ORANGE_C);
	m_groupServer6.SetBackgroundColor(WINDOW_UP1);
	m_groupServer6.SetFontBold(TRUE);

	m_groupServer7.SetFont(clsFunc.OnLogFont(16));
	m_groupServer7.SetCatptionTextColor(BLUE_C);
	m_groupServer7.SetBorderColor(ORANGE_C);
	m_groupServer7.SetBackgroundColor(WINDOW_UP1);
	m_groupServer7.SetFontBold(TRUE);

	m_groupServer8.SetFont(clsFunc.OnLogFont(16));
	m_groupServer8.SetCatptionTextColor(BLUE_C);
	m_groupServer8.SetBorderColor(ORANGE_C);
	m_groupServer8.SetBackgroundColor(WINDOW_UP1);
	m_groupServer8.SetFontBold(TRUE);

	m_groupCommand.SetFont(clsFunc.OnLogFont(16));
	m_groupCommand.SetCatptionTextColor(BLUE_C);
	m_groupCommand.SetBorderColor(ORANGE_C);
	m_groupCommand.SetBackgroundColor(WINDOW_UP1);
	m_groupCommand.SetFontBold(TRUE);

	m_groupEcServer.SetFont(clsFunc.OnLogFont(16));
	m_groupEcServer.SetCatptionTextColor(BLUE_C);
	m_groupEcServer.SetBorderColor(ORANGE_C);
	m_groupEcServer.SetBackgroundColor(WINDOW_UP1);
	m_groupEcServer.SetFontBold(TRUE);

	m_groupXgem.SetFont(clsFunc.OnLogFont(16));
	m_groupXgem.SetCatptionTextColor(BLUE_C);
	m_groupXgem.SetBorderColor(ORANGE_C);
	m_groupXgem.SetBackgroundColor(WINDOW_UP1);
	m_groupXgem.SetFontBold(TRUE);

	//kwlee 2016.1124
	m_groupTsiteSerial.SetFont(clsFunc.OnLogFont(16));
	m_groupTsiteSerial.SetCatptionTextColor(BLUE_C);
	m_groupTsiteSerial.SetBorderColor(ORANGE_C);
	m_groupTsiteSerial.SetBackgroundColor(WINDOW_UP1);
	m_groupTsiteSerial.SetFontBold(TRUE);

	m_groupTsiteSerial_1.SetFont(clsFunc.OnLogFont(16));
	m_groupTsiteSerial_1.SetCatptionTextColor(BLUE_C);
	m_groupTsiteSerial_1.SetBorderColor(ORANGE_C);
	m_groupTsiteSerial_1.SetBackgroundColor(WINDOW_UP1);
	m_groupTsiteSerial_1.SetFontBold(TRUE);

	m_groupTsiteSerial_2.SetFont(clsFunc.OnLogFont(16));
	m_groupTsiteSerial_2.SetCatptionTextColor(BLUE_C);
	m_groupTsiteSerial_2.SetBorderColor(ORANGE_C);
	m_groupTsiteSerial_2.SetBackgroundColor(WINDOW_UP1);
	m_groupTsiteSerial_2.SetFontBold(TRUE);

	m_groupTsiteSerial_3.SetFont(clsFunc.OnLogFont(16));
	m_groupTsiteSerial_3.SetCatptionTextColor(BLUE_C);
	m_groupTsiteSerial_3.SetBorderColor(ORANGE_C);
	m_groupTsiteSerial_3.SetBackgroundColor(WINDOW_UP1);
	m_groupTsiteSerial_3.SetFontBold(TRUE);

	m_groupTsiteSerial_4.SetFont(clsFunc.OnLogFont(16));
	m_groupTsiteSerial_4.SetCatptionTextColor(BLUE_C);
	m_groupTsiteSerial_4.SetBorderColor(ORANGE_C);
	m_groupTsiteSerial_4.SetBackgroundColor(WINDOW_UP1);
	m_groupTsiteSerial_4.SetFontBold(TRUE);

	m_groupTsiteSerial_5.SetFont(clsFunc.OnLogFont(16));
	m_groupTsiteSerial_5.SetCatptionTextColor(BLUE_C);
	m_groupTsiteSerial_5.SetBorderColor(ORANGE_C);
	m_groupTsiteSerial_5.SetBackgroundColor(WINDOW_UP1);
	m_groupTsiteSerial_5.SetFontBold(TRUE);

	m_groupTsiteSerial_6.SetFont(clsFunc.OnLogFont(16));
	m_groupTsiteSerial_6.SetCatptionTextColor(BLUE_C);
	m_groupTsiteSerial_6.SetBorderColor(ORANGE_C);
	m_groupTsiteSerial_6.SetBackgroundColor(WINDOW_UP1);
	m_groupTsiteSerial_6.SetFontBold(TRUE);

	m_groupTsiteSerial_7.SetFont(clsFunc.OnLogFont(16));
	m_groupTsiteSerial_7.SetCatptionTextColor(BLUE_C);
	m_groupTsiteSerial_7.SetBorderColor(ORANGE_C);
	m_groupTsiteSerial_7.SetBackgroundColor(WINDOW_UP1);
	m_groupTsiteSerial_7.SetFontBold(TRUE);

	m_groupTsiteSerial_8.SetFont(clsFunc.OnLogFont(16));
	m_groupTsiteSerial_8.SetCatptionTextColor(BLUE_C);
	m_groupTsiteSerial_8.SetBorderColor(ORANGE_C);
	m_groupTsiteSerial_8.SetBackgroundColor(WINDOW_UP1);
	m_groupTsiteSerial_8.SetFontBold(TRUE);


}

void CWorkTesterNetwork::OnInitLabel()
{
	
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

	m_msgClientIp4.SetFont(clsFunc.m_pFont[1]);
	m_msgClientIp4.SetWindowText(_T("IP"));
	m_msgClientIp4.SetCenterText();
	m_msgClientIp4.SetColor(WHITE_C);
	m_msgClientIp4.SetGradientColor(ORANGE_C);
	m_msgClientIp4.SetTextColor(BLACK_C);

	m_msgClientPort4.SetFont(clsFunc.m_pFont[1]);
	m_msgClientPort4.SetWindowText(_T("Port"));
	m_msgClientPort4.SetCenterText();
	m_msgClientPort4.SetColor(WHITE_C);
	m_msgClientPort4.SetGradientColor(ORANGE_C);
	m_msgClientPort4.SetTextColor(BLACK_C);

	m_msgClientIp5.SetFont(clsFunc.m_pFont[1]);
	m_msgClientIp5.SetWindowText(_T("IP"));
	m_msgClientIp5.SetCenterText();
	m_msgClientIp5.SetColor(WHITE_C);
	m_msgClientIp5.SetGradientColor(ORANGE_C);
	m_msgClientIp5.SetTextColor(BLACK_C);

	m_msgClientPort5.SetFont(clsFunc.m_pFont[1]);
	m_msgClientPort5.SetWindowText(_T("Port"));
	m_msgClientPort5.SetCenterText();
	m_msgClientPort5.SetColor(WHITE_C);
	m_msgClientPort5.SetGradientColor(ORANGE_C);
	m_msgClientPort5.SetTextColor(BLACK_C);

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

	m_msgClientIp7.SetFont(clsFunc.m_pFont[1]);
	m_msgClientIp7.SetWindowText(_T("IP"));
	m_msgClientIp7.SetCenterText();
	m_msgClientIp7.SetColor(WHITE_C);
	m_msgClientIp7.SetGradientColor(ORANGE_C);
	m_msgClientIp7.SetTextColor(BLACK_C);

	m_msgClientPort7.SetFont(clsFunc.m_pFont[1]);
	m_msgClientPort7.SetWindowText(_T("Port"));
	m_msgClientPort7.SetCenterText();
	m_msgClientPort7.SetColor(WHITE_C);
	m_msgClientPort7.SetGradientColor(ORANGE_C);
	m_msgClientPort7.SetTextColor(BLACK_C);

	m_msgClientIp8.SetFont(clsFunc.m_pFont[1]);
	m_msgClientIp8.SetWindowText(_T("IP"));
	m_msgClientIp8.SetCenterText();
	m_msgClientIp8.SetColor(WHITE_C);
	m_msgClientIp8.SetGradientColor(ORANGE_C);
	m_msgClientIp8.SetTextColor(BLACK_C);

	m_msgClientPort8.SetFont(clsFunc.m_pFont[1]);
	m_msgClientPort8.SetWindowText(_T("Port"));
	m_msgClientPort8.SetCenterText();
	m_msgClientPort8.SetColor(WHITE_C);
	m_msgClientPort8.SetGradientColor(ORANGE_C);
	m_msgClientPort8.SetTextColor(BLACK_C);

// 	m_msgServerIp1.SetFont(clsFunc.m_pFont[1]);
// 	m_msgServerIp1.SetWindowText(_T("IP"));
// 	m_msgServerIp1.SetCenterText();
// 	m_msgServerIp1.SetColor(WHITE_C);
// 	m_msgServerIp1.SetGradientColor(ORANGE_C);
// 	m_msgServerIp1.SetTextColor(BLACK_C);

	m_msgServerPort1.SetFont(clsFunc.m_pFont[1]);
	m_msgServerPort1.SetWindowText(_T("Port"));
	m_msgServerPort1.SetCenterText();
	m_msgServerPort1.SetColor(WHITE_C);
	m_msgServerPort1.SetGradientColor(ORANGE_C);
	m_msgServerPort1.SetTextColor(BLACK_C);

// 	m_msgServerIp2.SetFont(clsFunc.m_pFont[1]);
// 	m_msgServerIp2.SetWindowText(_T("IP"));
// 	m_msgServerIp2.SetCenterText();
// 	m_msgServerIp2.SetColor(WHITE_C);
// 	m_msgServerIp2.SetGradientColor(ORANGE_C);
// 	m_msgServerIp2.SetTextColor(BLACK_C);

	m_msgServerPort2.SetFont(clsFunc.m_pFont[1]);
	m_msgServerPort2.SetWindowText(_T("Port"));
	m_msgServerPort2.SetCenterText();
	m_msgServerPort2.SetColor(WHITE_C);
	m_msgServerPort2.SetGradientColor(ORANGE_C);
	m_msgServerPort2.SetTextColor(BLACK_C);

// 	m_msgServerIp3.SetFont(clsFunc.m_pFont[1]);
// 	m_msgServerIp3.SetWindowText(_T("IP"));
// 	m_msgServerIp3.SetCenterText();
// 	m_msgServerIp3.SetColor(WHITE_C);
// 	m_msgServerIp3.SetGradientColor(ORANGE_C);
// 	m_msgServerIp3.SetTextColor(BLACK_C);

	m_msgServerPort3.SetFont(clsFunc.m_pFont[1]);
	m_msgServerPort3.SetWindowText(_T("Port"));
	m_msgServerPort3.SetCenterText();
	m_msgServerPort3.SetColor(WHITE_C);
	m_msgServerPort3.SetGradientColor(ORANGE_C);
	m_msgServerPort3.SetTextColor(BLACK_C);

// 	m_msgServerIp4.SetFont(clsFunc.m_pFont[1]);
// 	m_msgServerIp4.SetWindowText(_T("IP"));
// 	m_msgServerIp4.SetCenterText();
// 	m_msgServerIp4.SetColor(WHITE_C);
// 	m_msgServerIp4.SetGradientColor(ORANGE_C);
// 	m_msgServerIp4.SetTextColor(BLACK_C);

	m_msgServerPort4.SetFont(clsFunc.m_pFont[1]);
	m_msgServerPort4.SetWindowText(_T("Port"));
	m_msgServerPort4.SetCenterText();
	m_msgServerPort4.SetColor(WHITE_C);
	m_msgServerPort4.SetGradientColor(ORANGE_C);
	m_msgServerPort4.SetTextColor(BLACK_C);

// 	m_msgServerIp5.SetFont(clsFunc.m_pFont[1]);
// 	m_msgServerIp5.SetWindowText(_T("IP"));
// 	m_msgServerIp5.SetCenterText();
// 	m_msgServerIp5.SetColor(WHITE_C);
// 	m_msgServerIp5.SetGradientColor(ORANGE_C);
// 	m_msgServerIp5.SetTextColor(BLACK_C);

	m_msgServerPort5.SetFont(clsFunc.m_pFont[1]);
	m_msgServerPort5.SetWindowText(_T("Port"));
	m_msgServerPort5.SetCenterText();
	m_msgServerPort5.SetColor(WHITE_C);
	m_msgServerPort5.SetGradientColor(ORANGE_C);
	m_msgServerPort5.SetTextColor(BLACK_C);

// 	m_msgServerIp6.SetFont(clsFunc.m_pFont[1]);
// 	m_msgServerIp6.SetWindowText(_T("IP"));
// 	m_msgServerIp6.SetCenterText();
// 	m_msgServerIp6.SetColor(WHITE_C);
// 	m_msgServerIp6.SetGradientColor(ORANGE_C);
// 	m_msgServerIp6.SetTextColor(BLACK_C);

	m_msgServerPort6.SetFont(clsFunc.m_pFont[1]);
	m_msgServerPort6.SetWindowText(_T("Port"));
	m_msgServerPort6.SetCenterText();
	m_msgServerPort6.SetColor(WHITE_C);
	m_msgServerPort6.SetGradientColor(ORANGE_C);
	m_msgServerPort6.SetTextColor(BLACK_C);

// 	m_msgServerIp7.SetFont(clsFunc.m_pFont[1]);
// 	m_msgServerIp7.SetWindowText(_T("IP"));
// 	m_msgServerIp7.SetCenterText();
// 	m_msgServerIp7.SetColor(WHITE_C);
// 	m_msgServerIp7.SetGradientColor(ORANGE_C);
// 	m_msgServerIp7.SetTextColor(BLACK_C);

	m_msgServerPort7.SetFont(clsFunc.m_pFont[1]);
	m_msgServerPort7.SetWindowText(_T("Port"));
	m_msgServerPort7.SetCenterText();
	m_msgServerPort7.SetColor(WHITE_C);
	m_msgServerPort7.SetGradientColor(ORANGE_C);
	m_msgServerPort7.SetTextColor(BLACK_C);

// 	m_msgServerIp8.SetFont(clsFunc.m_pFont[1]);
// 	m_msgServerIp8.SetWindowText(_T("IP"));
// 	m_msgServerIp8.SetCenterText();
// 	m_msgServerIp8.SetColor(WHITE_C);
// 	m_msgServerIp8.SetGradientColor(ORANGE_C);
// 	m_msgServerIp8.SetTextColor(BLACK_C);

	m_msgServerPort8.SetFont(clsFunc.m_pFont[1]);
	m_msgServerPort8.SetWindowText(_T("Port"));
	m_msgServerPort8.SetCenterText();
	m_msgServerPort8.SetColor(WHITE_C);
	m_msgServerPort8.SetGradientColor(ORANGE_C);
	m_msgServerPort8.SetTextColor(BLACK_C);

	m_msgEcServerIP.SetFont(clsFunc.m_pFont[1]);
	m_msgEcServerIP.SetWindowText(_T("IP"));
	m_msgEcServerIP.SetCenterText();
	m_msgEcServerIP.SetColor(WHITE_C);
	m_msgEcServerIP.SetGradientColor(ORANGE_C);
	m_msgEcServerIP.SetTextColor(BLACK_C);

	m_msgEcServerPort.SetFont(clsFunc.m_pFont[1]);
	m_msgEcServerPort.SetWindowText(_T("Port"));
	m_msgEcServerPort.SetCenterText();
	m_msgEcServerPort.SetColor(WHITE_C);
	m_msgEcServerPort.SetGradientColor(ORANGE_C);
	m_msgEcServerPort.SetTextColor(BLACK_C);

	m_msgXgemIp.SetFont(clsFunc.m_pFont[1]);
	m_msgXgemIp.SetWindowText(_T("IP"));
	m_msgXgemIp.SetCenterText();
	m_msgXgemIp.SetColor(WHITE_C);
	m_msgXgemIp.SetGradientColor(ORANGE_C);
	m_msgXgemIp.SetTextColor(BLACK_C);

	m_msgXgemPort.SetFont(clsFunc.m_pFont[1]);
	m_msgXgemPort.SetWindowText(_T("Port"));
	m_msgXgemPort.SetCenterText();
	m_msgXgemPort.SetColor(WHITE_C);
	m_msgXgemPort.SetGradientColor(ORANGE_C);
	m_msgXgemPort.SetTextColor(BLACK_C);
	
	////////
// 	m_msgServerName1.SetFont(clsFunc.m_pFont[1]);
// 	m_msgServerName1.SetWindowText(_T("Name"));
// 	m_msgServerName1.SetCenterText();
// 	m_msgServerName1.SetColor(WHITE_C);
// 	m_msgServerName1.SetGradientColor(ORANGE_C);
// 	m_msgServerName1.SetTextColor(BLACK_C);

	m_msgClientName1.SetFont(clsFunc.m_pFont[1]);
	m_msgClientName1.SetWindowText(_T("Name"));
	m_msgClientName1.SetCenterText();
	m_msgClientName1.SetColor(WHITE_C);
	m_msgClientName1.SetGradientColor(ORANGE_C);
	m_msgClientName1.SetTextColor(BLACK_C);

// 	m_msgServerName2.SetFont(clsFunc.m_pFont[1]);
// 	m_msgServerName2.SetWindowText(_T("Name"));
// 	m_msgServerName2.SetCenterText();
// 	m_msgServerName2.SetColor(WHITE_C);
// 	m_msgServerName2.SetGradientColor(ORANGE_C);
// 	m_msgServerName2.SetTextColor(BLACK_C);

	m_msgClientName2.SetFont(clsFunc.m_pFont[1]);
	m_msgClientName2.SetWindowText(_T("Name"));
	m_msgClientName2.SetCenterText();
	m_msgClientName2.SetColor(WHITE_C);
	m_msgClientName2.SetGradientColor(ORANGE_C);
	m_msgClientName2.SetTextColor(BLACK_C);

// 	m_msgServerName3.SetFont(clsFunc.m_pFont[1]);
// 	m_msgServerName3.SetWindowText(_T("Name"));
// 	m_msgServerName3.SetCenterText();
// 	m_msgServerName3.SetColor(WHITE_C);
// 	m_msgServerName3.SetGradientColor(ORANGE_C);
// 	m_msgServerName3.SetTextColor(BLACK_C);

	m_msgClientName3.SetFont(clsFunc.m_pFont[1]);
	m_msgClientName3.SetWindowText(_T("Name"));
	m_msgClientName3.SetCenterText();
	m_msgClientName3.SetColor(WHITE_C);
	m_msgClientName3.SetGradientColor(ORANGE_C);
	m_msgClientName3.SetTextColor(BLACK_C);

// 	m_msgServerName4.SetFont(clsFunc.m_pFont[1]);
// 	m_msgServerName4.SetWindowText(_T("Name"));
// 	m_msgServerName4.SetCenterText();
// 	m_msgServerName4.SetColor(WHITE_C);
// 	m_msgServerName4.SetGradientColor(ORANGE_C);
// 	m_msgServerName4.SetTextColor(BLACK_C);

	m_msgClientName4.SetFont(clsFunc.m_pFont[1]);
	m_msgClientName4.SetWindowText(_T("Name"));
	m_msgClientName4.SetCenterText();
	m_msgClientName4.SetColor(WHITE_C);
	m_msgClientName4.SetGradientColor(ORANGE_C);
	m_msgClientName4.SetTextColor(BLACK_C);

// 	m_msgServerName5.SetFont(clsFunc.m_pFont[1]);
// 	m_msgServerName5.SetWindowText(_T("Name"));
// 	m_msgServerName5.SetCenterText();
// 	m_msgServerName5.SetColor(WHITE_C);
// 	m_msgServerName5.SetGradientColor(ORANGE_C);
// 	m_msgServerName5.SetTextColor(BLACK_C);

	m_msgClientName5.SetFont(clsFunc.m_pFont[1]);
	m_msgClientName5.SetWindowText(_T("Name"));
	m_msgClientName5.SetCenterText();
	m_msgClientName5.SetColor(WHITE_C);
	m_msgClientName5.SetGradientColor(ORANGE_C);
	m_msgClientName5.SetTextColor(BLACK_C);

// 	m_msgServerName6.SetFont(clsFunc.m_pFont[1]);
// 	m_msgServerName6.SetWindowText(_T("Name"));
// 	m_msgServerName6.SetCenterText();
// 	m_msgServerName6.SetColor(WHITE_C);
// 	m_msgServerName6.SetGradientColor(ORANGE_C);
// 	m_msgServerName6.SetTextColor(BLACK_C);

	m_msgClientName6.SetFont(clsFunc.m_pFont[1]);
	m_msgClientName6.SetWindowText(_T("Name"));
	m_msgClientName6.SetCenterText();
	m_msgClientName6.SetColor(WHITE_C);
	m_msgClientName6.SetGradientColor(ORANGE_C);
	m_msgClientName6.SetTextColor(BLACK_C);

// 	m_msgServerName7.SetFont(clsFunc.m_pFont[1]);
// 	m_msgServerName7.SetWindowText(_T("Name"));
// 	m_msgServerName7.SetCenterText();
// 	m_msgServerName7.SetColor(WHITE_C);
// 	m_msgServerName7.SetGradientColor(ORANGE_C);
// 	m_msgServerName7.SetTextColor(BLACK_C);

	m_msgClientName7.SetFont(clsFunc.m_pFont[1]);
	m_msgClientName7.SetWindowText(_T("Name"));
	m_msgClientName7.SetCenterText();
	m_msgClientName7.SetColor(WHITE_C);
	m_msgClientName7.SetGradientColor(ORANGE_C);
	m_msgClientName7.SetTextColor(BLACK_C);

// 	m_msgServerName8.SetFont(clsFunc.m_pFont[1]);
// 	m_msgServerName8.SetWindowText(_T("Name"));
// 	m_msgServerName8.SetCenterText();
// 	m_msgServerName8.SetColor(WHITE_C);
// 	m_msgServerName8.SetGradientColor(ORANGE_C);
// 	m_msgServerName8.SetTextColor(BLACK_C);

	m_msgClientName8.SetFont(clsFunc.m_pFont[1]);
	m_msgClientName8.SetWindowText(_T("Name"));
	m_msgClientName8.SetCenterText();
	m_msgClientName8.SetColor(WHITE_C);
	m_msgClientName8.SetGradientColor(ORANGE_C);
	m_msgClientName8.SetTextColor(BLACK_C);

	///////////////////Server////////////////////
	
	m_msgServerPortData_1.SetFont(clsFunc.m_pFont[1]);
	m_msgServerPortData_1.SetCenterText();
	m_msgServerPortData_1.SetColor(WHITE_C);
	m_msgServerPortData_1.SetGradientColor(WHITE_C);
	m_msgServerPortData_1.SetTextColor(BLACK_C);

	m_msgServerPortData_2.SetFont(clsFunc.m_pFont[1]);
	m_msgServerPortData_2.SetCenterText();
	m_msgServerPortData_2.SetColor(WHITE_C);
	m_msgServerPortData_2.SetGradientColor(WHITE_C);
	m_msgServerPortData_2.SetTextColor(BLACK_C);

	m_msgServerPortData_3.SetFont(clsFunc.m_pFont[1]);
	m_msgServerPortData_3.SetCenterText();
	m_msgServerPortData_3.SetColor(WHITE_C);
	m_msgServerPortData_3.SetGradientColor(WHITE_C);
	m_msgServerPortData_3.SetTextColor(BLACK_C);

	m_msgServerPortData_4.SetFont(clsFunc.m_pFont[1]);
	m_msgServerPortData_4.SetCenterText();
	m_msgServerPortData_4.SetColor(WHITE_C);
	m_msgServerPortData_4.SetGradientColor(WHITE_C);
	m_msgServerPortData_4.SetTextColor(BLACK_C);

	m_msgServerPortData_5.SetFont(clsFunc.m_pFont[1]);
	m_msgServerPortData_5.SetCenterText();
	m_msgServerPortData_5.SetColor(WHITE_C);
	m_msgServerPortData_5.SetGradientColor(WHITE_C);
	m_msgServerPortData_5.SetTextColor(BLACK_C);

	m_msgServerPortData_6.SetFont(clsFunc.m_pFont[1]);
	m_msgServerPortData_6.SetCenterText();
	m_msgServerPortData_6.SetColor(WHITE_C);
	m_msgServerPortData_6.SetGradientColor(WHITE_C);
	m_msgServerPortData_6.SetTextColor(BLACK_C);

	m_msgServerPortData_7.SetFont(clsFunc.m_pFont[1]);
	m_msgServerPortData_7.SetCenterText();
	m_msgServerPortData_7.SetColor(WHITE_C);
	m_msgServerPortData_7.SetGradientColor(WHITE_C);
	m_msgServerPortData_7.SetTextColor(BLACK_C);

	m_msgServerPortData_8.SetFont(clsFunc.m_pFont[1]);
	m_msgServerPortData_8.SetCenterText();
	m_msgServerPortData_8.SetColor(WHITE_C);
	m_msgServerPortData_8.SetGradientColor(WHITE_C);
	m_msgServerPortData_8.SetTextColor(BLACK_C);

	////////////Server_Name////////////////////////
	m_msgServerID_1.SetFont(clsFunc.m_pFont[1]);
	m_msgServerID_1.SetCenterText();
	m_msgServerID_1.SetColor(WHITE_C);
	m_msgServerID_1.SetGradientColor(WHITE_C);
	m_msgServerID_1.SetTextColor(BLACK_C);

	m_msgServerID_2.SetFont(clsFunc.m_pFont[1]);
	m_msgServerID_2.SetCenterText();
	m_msgServerID_2.SetColor(WHITE_C);
	m_msgServerID_2.SetGradientColor(WHITE_C);
	m_msgServerID_2.SetTextColor(BLACK_C);

	m_msgServerID_3.SetFont(clsFunc.m_pFont[1]);
	m_msgServerID_3.SetCenterText();
	m_msgServerID_3.SetColor(WHITE_C);
	m_msgServerID_3.SetGradientColor(WHITE_C);
	m_msgServerID_3.SetTextColor(BLACK_C);

	m_msgServerID_4.SetFont(clsFunc.m_pFont[1]);
	m_msgServerID_4.SetCenterText();
	m_msgServerID_4.SetColor(WHITE_C);
	m_msgServerID_4.SetGradientColor(WHITE_C);
	m_msgServerID_4.SetTextColor(BLACK_C);

	m_msgServerID_5.SetFont(clsFunc.m_pFont[1]);
	m_msgServerID_5.SetCenterText();
	m_msgServerID_5.SetColor(WHITE_C);
	m_msgServerID_5.SetGradientColor(WHITE_C);
	m_msgServerID_5.SetTextColor(BLACK_C);

	m_msgServerID_6.SetFont(clsFunc.m_pFont[1]);
	m_msgServerID_6.SetCenterText();
	m_msgServerID_6.SetColor(WHITE_C);
	m_msgServerID_6.SetGradientColor(WHITE_C);
	m_msgServerID_6.SetTextColor(BLACK_C);

	m_msgServerID_7.SetFont(clsFunc.m_pFont[1]);
	m_msgServerID_7.SetCenterText();
	m_msgServerID_7.SetColor(WHITE_C);
	m_msgServerID_7.SetGradientColor(WHITE_C);
	m_msgServerID_7.SetTextColor(BLACK_C);

	m_msgServerID_8.SetFont(clsFunc.m_pFont[1]);
	m_msgServerID_8.SetCenterText();
	m_msgServerID_8.SetColor(WHITE_C);
	m_msgServerID_8.SetGradientColor(WHITE_C);
	m_msgServerID_8.SetTextColor(BLACK_C);

	///////////////////Serial Name//////////////
	m_msgSerialName_1_1.SetFont(clsFunc.m_pFont[1]);
	m_msgSerialName_1_1.SetWindowText(_T("1"));
	m_msgSerialName_1_1.SetCenterText();
	m_msgSerialName_1_1.SetColor(WHITE_C);
	m_msgSerialName_1_1.SetGradientColor(ORANGE_C);
	m_msgSerialName_1_1.SetTextColor(BLACK_C);

	m_msgSerialName_1_2.SetFont(clsFunc.m_pFont[1]);
	m_msgSerialName_1_2.SetWindowText(_T("2"));
	m_msgSerialName_1_2.SetCenterText();
	m_msgSerialName_1_2.SetColor(WHITE_C);
	m_msgSerialName_1_2.SetGradientColor(ORANGE_C);
	m_msgSerialName_1_2.SetTextColor(BLACK_C);

	m_msgSerialName_2_1.SetFont(clsFunc.m_pFont[1]);
	m_msgSerialName_2_1.SetWindowText(_T("1"));
	m_msgSerialName_2_1.SetCenterText();
	m_msgSerialName_2_1.SetColor(WHITE_C);
	m_msgSerialName_2_1.SetGradientColor(ORANGE_C);
	m_msgSerialName_2_1.SetTextColor(BLACK_C);

	m_msgSerialName_2_2.SetFont(clsFunc.m_pFont[1]);
	m_msgSerialName_2_2.SetWindowText(_T("2"));
	m_msgSerialName_2_2.SetCenterText();
	m_msgSerialName_2_2.SetColor(WHITE_C);
	m_msgSerialName_2_2.SetGradientColor(ORANGE_C);
	m_msgSerialName_2_2.SetTextColor(BLACK_C);

	m_msgSerialName_3_1.SetFont(clsFunc.m_pFont[1]);
	m_msgSerialName_3_1.SetWindowText(_T("1"));
	m_msgSerialName_3_1.SetCenterText();
	m_msgSerialName_3_1.SetColor(WHITE_C);
	m_msgSerialName_3_1.SetGradientColor(ORANGE_C);
	m_msgSerialName_3_1.SetTextColor(BLACK_C);

	m_msgSerialName_3_2.SetFont(clsFunc.m_pFont[1]);
	m_msgSerialName_3_2.SetWindowText(_T("2"));
	m_msgSerialName_3_2.SetCenterText();
	m_msgSerialName_3_2.SetColor(WHITE_C);
	m_msgSerialName_3_2.SetGradientColor(ORANGE_C);
	m_msgSerialName_3_2.SetTextColor(BLACK_C);

	m_msgSerialName_4_1.SetFont(clsFunc.m_pFont[1]);
	m_msgSerialName_4_1.SetWindowText(_T("1"));
	m_msgSerialName_4_1.SetCenterText();
	m_msgSerialName_4_1.SetColor(WHITE_C);
	m_msgSerialName_4_1.SetGradientColor(ORANGE_C);
	m_msgSerialName_4_1.SetTextColor(BLACK_C);

	m_msgSerialName_4_2.SetFont(clsFunc.m_pFont[1]);
	m_msgSerialName_4_2.SetWindowText(_T("2"));
	m_msgSerialName_4_2.SetCenterText();
	m_msgSerialName_4_2.SetColor(WHITE_C);
	m_msgSerialName_4_2.SetGradientColor(ORANGE_C);
	m_msgSerialName_4_2.SetTextColor(BLACK_C);

	m_msgSerialName_5_1.SetFont(clsFunc.m_pFont[1]);
	m_msgSerialName_5_1.SetWindowText(_T("1"));
	m_msgSerialName_5_1.SetCenterText();
	m_msgSerialName_5_1.SetColor(WHITE_C);
	m_msgSerialName_5_1.SetGradientColor(ORANGE_C);
	m_msgSerialName_5_1.SetTextColor(BLACK_C);

	m_msgSerialName_5_2.SetFont(clsFunc.m_pFont[1]);
	m_msgSerialName_5_2.SetWindowText(_T("2"));
	m_msgSerialName_5_2.SetCenterText();
	m_msgSerialName_5_2.SetColor(WHITE_C);
	m_msgSerialName_5_2.SetGradientColor(ORANGE_C);
	m_msgSerialName_5_2.SetTextColor(BLACK_C);

	m_msgSerialName_6_1.SetFont(clsFunc.m_pFont[1]);
	m_msgSerialName_6_1.SetWindowText(_T("1"));
	m_msgSerialName_6_1.SetCenterText();
	m_msgSerialName_6_1.SetColor(WHITE_C);
	m_msgSerialName_6_1.SetGradientColor(ORANGE_C);
	m_msgSerialName_6_1.SetTextColor(BLACK_C);

	m_msgSerialName_6_2.SetFont(clsFunc.m_pFont[1]);
	m_msgSerialName_6_2.SetWindowText(_T("2"));
	m_msgSerialName_6_2.SetCenterText();
	m_msgSerialName_6_2.SetColor(WHITE_C);
	m_msgSerialName_6_2.SetGradientColor(ORANGE_C);
	m_msgSerialName_6_2.SetTextColor(BLACK_C);

	m_msgSerialName_7_1.SetFont(clsFunc.m_pFont[1]);
	m_msgSerialName_7_1.SetWindowText(_T("1"));
	m_msgSerialName_7_1.SetCenterText();
	m_msgSerialName_7_1.SetColor(WHITE_C);
	m_msgSerialName_7_1.SetGradientColor(ORANGE_C);
	m_msgSerialName_7_1.SetTextColor(BLACK_C);

	m_msgSerialName_7_2.SetFont(clsFunc.m_pFont[1]);
	m_msgSerialName_7_2.SetWindowText(_T("2"));
	m_msgSerialName_7_2.SetCenterText();
	m_msgSerialName_7_2.SetColor(WHITE_C);
	m_msgSerialName_7_2.SetGradientColor(ORANGE_C);
	m_msgSerialName_7_2.SetTextColor(BLACK_C);

	m_msgSerialName_8_1.SetFont(clsFunc.m_pFont[1]);
	m_msgSerialName_8_1.SetWindowText(_T("1"));
	m_msgSerialName_8_1.SetCenterText();
	m_msgSerialName_8_1.SetColor(WHITE_C);
	m_msgSerialName_8_1.SetGradientColor(ORANGE_C);
	m_msgSerialName_8_1.SetTextColor(BLACK_C);

	m_msgSerialName_8_2.SetFont(clsFunc.m_pFont[1]);
	m_msgSerialName_8_2.SetWindowText(_T("2"));
	m_msgSerialName_8_2.SetCenterText();
	m_msgSerialName_8_2.SetColor(WHITE_C);
	m_msgSerialName_8_2.SetGradientColor(ORANGE_C);
	m_msgSerialName_8_2.SetTextColor(BLACK_C);

	///////////////////Serial ID///////////
	m_msgSerialID_1_1.SetFont(clsFunc.m_pFont[1]);
	m_msgSerialID_1_1.SetCenterText();
	m_msgSerialID_1_1.SetColor(WHITE_C);
	m_msgSerialID_1_1.SetGradientColor(WHITE_C);
	m_msgSerialID_1_1.SetTextColor(BLACK_C);

	m_msgSerialID_1_2.SetFont(clsFunc.m_pFont[1]);
	m_msgSerialID_1_2.SetCenterText();
	m_msgSerialID_1_2.SetColor(WHITE_C);
	m_msgSerialID_1_2.SetGradientColor(WHITE_C);
	m_msgSerialID_1_2.SetTextColor(BLACK_C);

	m_msgSerialID_2_1.SetFont(clsFunc.m_pFont[1]);
	m_msgSerialID_2_1.SetCenterText();
	m_msgSerialID_2_1.SetColor(WHITE_C);
	m_msgSerialID_2_1.SetGradientColor(WHITE_C);
	m_msgSerialID_2_1.SetTextColor(BLACK_C);

	m_msgSerialID_2_2.SetFont(clsFunc.m_pFont[1]);
	m_msgSerialID_2_2.SetCenterText();
	m_msgSerialID_2_2.SetColor(WHITE_C);
	m_msgSerialID_2_2.SetGradientColor(WHITE_C);
	m_msgSerialID_2_2.SetTextColor(BLACK_C);

	m_msgSerialID_3_1.SetFont(clsFunc.m_pFont[1]);
	m_msgSerialID_3_1.SetCenterText();
	m_msgSerialID_3_1.SetColor(WHITE_C);
	m_msgSerialID_3_1.SetGradientColor(WHITE_C);
	m_msgSerialID_3_1.SetTextColor(BLACK_C);

	m_msgSerialID_3_2.SetFont(clsFunc.m_pFont[1]);
	m_msgSerialID_3_2.SetCenterText();
	m_msgSerialID_3_2.SetColor(WHITE_C);
	m_msgSerialID_3_2.SetGradientColor(WHITE_C);
	m_msgSerialID_3_2.SetTextColor(BLACK_C);

	m_msgSerialID_4_1.SetFont(clsFunc.m_pFont[1]);
	m_msgSerialID_4_1.SetCenterText();
	m_msgSerialID_4_1.SetColor(WHITE_C);
	m_msgSerialID_4_1.SetGradientColor(WHITE_C);
	m_msgSerialID_4_1.SetTextColor(BLACK_C);

	m_msgSerialID_4_2.SetFont(clsFunc.m_pFont[1]);
	m_msgSerialID_4_2.SetCenterText();
	m_msgSerialID_4_2.SetColor(WHITE_C);
	m_msgSerialID_4_2.SetGradientColor(WHITE_C);
	m_msgSerialID_4_2.SetTextColor(BLACK_C);

	m_msgSerialID_5_1.SetFont(clsFunc.m_pFont[1]);
	m_msgSerialID_5_1.SetCenterText();
	m_msgSerialID_5_1.SetColor(WHITE_C);
	m_msgSerialID_5_1.SetGradientColor(WHITE_C);
	m_msgSerialID_5_1.SetTextColor(BLACK_C);

	m_msgSerialID_5_2.SetFont(clsFunc.m_pFont[1]);
	m_msgSerialID_5_2.SetCenterText();
	m_msgSerialID_5_2.SetColor(WHITE_C);
	m_msgSerialID_5_2.SetGradientColor(WHITE_C);
	m_msgSerialID_5_2.SetTextColor(BLACK_C);

	m_msgSerialID_6_1.SetFont(clsFunc.m_pFont[1]);
	m_msgSerialID_6_1.SetCenterText();
	m_msgSerialID_6_1.SetColor(WHITE_C);
	m_msgSerialID_6_1.SetGradientColor(WHITE_C);
	m_msgSerialID_6_1.SetTextColor(BLACK_C);

	m_msgSerialID_6_2.SetFont(clsFunc.m_pFont[1]);
	m_msgSerialID_6_2.SetCenterText();
	m_msgSerialID_6_2.SetColor(WHITE_C);
	m_msgSerialID_6_2.SetGradientColor(WHITE_C);
	m_msgSerialID_6_2.SetTextColor(BLACK_C);

	m_msgSerialID_7_1.SetFont(clsFunc.m_pFont[1]);
	m_msgSerialID_7_1.SetCenterText();
	m_msgSerialID_7_1.SetColor(WHITE_C);
	m_msgSerialID_7_1.SetGradientColor(WHITE_C);
	m_msgSerialID_7_1.SetTextColor(BLACK_C);

	m_msgSerialID_7_2.SetFont(clsFunc.m_pFont[1]);
	m_msgSerialID_7_2.SetCenterText();
	m_msgSerialID_7_2.SetColor(WHITE_C);
	m_msgSerialID_7_2.SetGradientColor(WHITE_C);
	m_msgSerialID_7_2.SetTextColor(BLACK_C);

	m_msgSerialID_8_1.SetFont(clsFunc.m_pFont[1]);
	m_msgSerialID_8_1.SetCenterText();
	m_msgSerialID_8_1.SetColor(WHITE_C);
	m_msgSerialID_8_1.SetGradientColor(WHITE_C);
	m_msgSerialID_8_1.SetTextColor(BLACK_C);

	m_msgSerialID_8_2.SetFont(clsFunc.m_pFont[1]);
	m_msgSerialID_8_2.SetCenterText();
	m_msgSerialID_8_2.SetColor(WHITE_C);
	m_msgSerialID_8_2.SetGradientColor(WHITE_C);
	m_msgSerialID_8_2.SetTextColor(BLACK_C);

	///////////////////Client///////////////

	m_msgClientPortData_1.SetFont(clsFunc.m_pFont[1]);
	m_msgClientPortData_1.SetCenterText();
	m_msgClientPortData_1.SetColor(WHITE_C);
	m_msgClientPortData_1.SetGradientColor(WHITE_C);
	m_msgClientPortData_1.SetTextColor(BLACK_C);

	m_msgClientPortData_2.SetFont(clsFunc.m_pFont[1]);
	m_msgClientPortData_2.SetCenterText();
	m_msgClientPortData_2.SetColor(WHITE_C);
	m_msgClientPortData_2.SetGradientColor(WHITE_C);
	m_msgClientPortData_2.SetTextColor(BLACK_C);

	m_msgClientPortData_3.SetFont(clsFunc.m_pFont[1]);
	m_msgClientPortData_3.SetCenterText();
	m_msgClientPortData_3.SetColor(WHITE_C);
	m_msgClientPortData_3.SetGradientColor(WHITE_C);
	m_msgClientPortData_3.SetTextColor(BLACK_C);

	m_msgClientPortData_4.SetFont(clsFunc.m_pFont[1]);
	m_msgClientPortData_4.SetCenterText();
	m_msgClientPortData_4.SetColor(WHITE_C);
	m_msgClientPortData_4.SetGradientColor(WHITE_C);
	m_msgClientPortData_4.SetTextColor(BLACK_C);

	m_msgClientPortData_5.SetFont(clsFunc.m_pFont[1]);
	m_msgClientPortData_5.SetCenterText();
	m_msgClientPortData_5.SetColor(WHITE_C);
	m_msgClientPortData_5.SetGradientColor(WHITE_C);
	m_msgClientPortData_5.SetTextColor(BLACK_C);

	m_msgClientPortData_6.SetFont(clsFunc.m_pFont[1]);
	m_msgClientPortData_6.SetCenterText();
	m_msgClientPortData_6.SetColor(WHITE_C);
	m_msgClientPortData_6.SetGradientColor(WHITE_C);
	m_msgClientPortData_6.SetTextColor(BLACK_C);

	m_msgClientPortData_7.SetFont(clsFunc.m_pFont[1]);
	m_msgClientPortData_7.SetCenterText();
	m_msgClientPortData_7.SetColor(WHITE_C);
	m_msgClientPortData_7.SetGradientColor(WHITE_C);
	m_msgClientPortData_7.SetTextColor(BLACK_C);

	m_msgClientPortData_8.SetFont(clsFunc.m_pFont[1]);
	m_msgClientPortData_8.SetCenterText();
	m_msgClientPortData_8.SetColor(WHITE_C);
	m_msgClientPortData_8.SetGradientColor(WHITE_C);
	m_msgClientPortData_8.SetTextColor(BLACK_C);

	m_msgEcServerPortData.SetFont(clsFunc.m_pFont[1]);
	m_msgEcServerPortData.SetCenterText();
	m_msgEcServerPortData.SetColor(WHITE_C);
	m_msgEcServerPortData.SetGradientColor(WHITE_C);
	m_msgEcServerPortData.SetTextColor(BLACK_C);


	m_msgXgemPortData.SetFont(clsFunc.m_pFont[1]);
	m_msgXgemPortData.SetCenterText();
	m_msgXgemPortData.SetColor(WHITE_C);
	m_msgXgemPortData.SetGradientColor(WHITE_C);
	m_msgXgemPortData.SetTextColor(BLACK_C);


	m_msgEcServerPortData.SetFont(clsFunc.m_pFont[1]);
	m_msgEcServerPortData.SetCenterText();
	m_msgEcServerPortData.SetColor(WHITE_C);
	m_msgEcServerPortData.SetGradientColor(WHITE_C);
	m_msgEcServerPortData.SetTextColor(BLACK_C);

	m_msgXgemPortData.SetFont(clsFunc.m_pFont[1]);
	m_msgXgemPortData.SetCenterText();
	m_msgXgemPortData.SetColor(WHITE_C);
	m_msgXgemPortData.SetGradientColor(WHITE_C);
	m_msgXgemPortData.SetTextColor(BLACK_C);

	////////////Client_Name////////////////////////
	m_msgClientID_1.SetFont(clsFunc.m_pFont[1]);
	m_msgClientID_1.SetCenterText();
	m_msgClientID_1.SetColor(WHITE_C);
	m_msgClientID_1.SetGradientColor(WHITE_C);
	m_msgClientID_1.SetTextColor(BLACK_C);

	m_msgClientID_2.SetFont(clsFunc.m_pFont[1]);
	m_msgClientID_2.SetCenterText();
	m_msgClientID_2.SetColor(WHITE_C);
	m_msgClientID_2.SetGradientColor(WHITE_C);
	m_msgClientID_2.SetTextColor(BLACK_C);

	m_msgClientID_3.SetFont(clsFunc.m_pFont[1]);
	m_msgClientID_3.SetCenterText();
	m_msgClientID_3.SetColor(WHITE_C);
	m_msgClientID_3.SetGradientColor(WHITE_C);
	m_msgClientID_3.SetTextColor(BLACK_C);

	m_msgClientID_4.SetFont(clsFunc.m_pFont[1]);
	m_msgClientID_4.SetCenterText();
	m_msgClientID_4.SetColor(WHITE_C);
	m_msgClientID_4.SetGradientColor(WHITE_C);
	m_msgClientID_4.SetTextColor(BLACK_C);

	m_msgClientID_5.SetFont(clsFunc.m_pFont[1]);
	m_msgClientID_5.SetCenterText();
	m_msgClientID_5.SetColor(WHITE_C);
	m_msgClientID_5.SetGradientColor(WHITE_C);
	m_msgClientID_5.SetTextColor(BLACK_C);

	m_msgClientID_6.SetFont(clsFunc.m_pFont[1]);
	m_msgClientID_6.SetCenterText();
	m_msgClientID_6.SetColor(WHITE_C);
	m_msgClientID_6.SetGradientColor(WHITE_C);
	m_msgClientID_6.SetTextColor(BLACK_C);

	m_msgClientID_7.SetFont(clsFunc.m_pFont[1]);
	m_msgClientID_7.SetCenterText();
	m_msgClientID_7.SetColor(WHITE_C);
	m_msgClientID_7.SetGradientColor(WHITE_C);
	m_msgClientID_7.SetTextColor(BLACK_C);

	m_msgClientID_8.SetFont(clsFunc.m_pFont[1]);
	m_msgClientID_8.SetCenterText();
	m_msgClientID_8.SetColor(WHITE_C);
	m_msgClientID_8.SetGradientColor(WHITE_C);
	m_msgClientID_8.SetTextColor(BLACK_C);



}


void CWorkTesterNetwork::OnInitButton()
{

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

	  m_btnClientIpSet[3].SetBitmaps(IDC_BTN_CLIENT_IP_4, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
	  m_btnClientIpSet[3].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	  m_btnClientIpSet[3].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	  m_btnClientIpSet[3].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	  m_btnClientIpSet[3].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	  m_btnClientIpSet[3].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	  m_btnClientIpSet[3].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	  m_btnClientIpSet[3].SetFont(clsFunc.m_pFont[1]);
	  m_btnClientIpSet[3].SetTooltipText(_T("4 Client IP Address"));
 
	  m_btnClientIpSet[4].SetBitmaps(IDC_BTN_CLIENT_IP_5, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
	  m_btnClientIpSet[4].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	  m_btnClientIpSet[4].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	  m_btnClientIpSet[4].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	  m_btnClientIpSet[4].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	  m_btnClientIpSet[4].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	  m_btnClientIpSet[4].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	  m_btnClientIpSet[4].SetFont(clsFunc.m_pFont[1]);
	  m_btnClientIpSet[4].SetTooltipText(_T("5 Client IP Address"));
 
	  m_btnClientIpSet[5].SetBitmaps(IDC_BTN_CLIENT_IP_6, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
	  m_btnClientIpSet[5].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	  m_btnClientIpSet[5].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	  m_btnClientIpSet[5].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	  m_btnClientIpSet[5].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	  m_btnClientIpSet[5].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	  m_btnClientIpSet[5].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	  m_btnClientIpSet[5].SetFont(clsFunc.m_pFont[1]);
	  m_btnClientIpSet[5].SetTooltipText(_T("6 Client IP Address"));
 
	  m_btnClientIpSet[6].SetBitmaps(IDC_BTN_CLIENT_IP_7, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
	  m_btnClientIpSet[6].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	  m_btnClientIpSet[6].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	  m_btnClientIpSet[6].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	  m_btnClientIpSet[6].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	  m_btnClientIpSet[6].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	  m_btnClientIpSet[6].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	  m_btnClientIpSet[6].SetFont(clsFunc.m_pFont[1]);
	  m_btnClientIpSet[6].SetTooltipText(_T("7 Client IP Address"));

	 m_btnClientIpSet[7].SetBitmaps(IDC_BTN_CLIENT_IP_8, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
	 m_btnClientIpSet[7].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	 m_btnClientIpSet[7].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	 m_btnClientIpSet[7].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	 m_btnClientIpSet[7].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	 m_btnClientIpSet[7].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	 m_btnClientIpSet[7].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	 m_btnClientIpSet[7].SetFont(clsFunc.m_pFont[1]);
	 m_btnClientIpSet[7].SetTooltipText(_T("8 Client IP Address"));

	 m_btnClientIpSet[8].SetBitmaps(IDC_BTN_ECSERVER_IP, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
	 m_btnClientIpSet[8].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	 m_btnClientIpSet[8].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	 m_btnClientIpSet[8].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	 m_btnClientIpSet[8].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	 m_btnClientIpSet[8].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	 m_btnClientIpSet[8].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	 m_btnClientIpSet[8].SetFont(clsFunc.m_pFont[1]);
	 m_btnClientIpSet[8].SetTooltipText(_T("ECSERVER IP Address"));

	 m_btnClientIpSet[9].SetBitmaps(IDC_BTN_XGEM_IP, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
	 m_btnClientIpSet[9].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
	 m_btnClientIpSet[9].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
	 m_btnClientIpSet[9].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
	 m_btnClientIpSet[9].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	 m_btnClientIpSet[9].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	 m_btnClientIpSet[9].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	 m_btnClientIpSet[9].SetFont(clsFunc.m_pFont[1]);
	m_btnClientIpSet[9].SetTooltipText(_T("XGEM IP Address"));


// 	m_btnServerIpSet[0].SetBitmaps(IDC_BTN_SERVER_IP_1, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
// 	m_btnServerIpSet[0].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
// 	m_btnServerIpSet[0].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
// 	m_btnServerIpSet[0].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
// 	m_btnServerIpSet[0].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
// 	m_btnServerIpSet[0].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
// 	m_btnServerIpSet[0].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
// 	m_btnServerIpSet[0].SetFont(clsFunc.m_pFont[1]);
// 	m_btnServerIpSet[0].SetTooltipText(_T("1 Server IP Address"));
// 
// 	m_btnServerIpSet[1].SetBitmaps(IDC_BTN_SERVER_IP_2, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
// 	m_btnServerIpSet[1].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
// 	m_btnServerIpSet[1].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
// 	m_btnServerIpSet[1].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
// 	m_btnServerIpSet[1].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
// 	m_btnServerIpSet[1].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
// 	m_btnServerIpSet[1].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
// 	m_btnServerIpSet[1].SetFont(clsFunc.m_pFont[1]);
// 	m_btnServerIpSet[1].SetTooltipText(_T("2 Server IP Address"));

// 	m_btnServerIpSet[2].SetBitmaps(IDC_BTN_SERVER_IP_3, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
// 	m_btnServerIpSet[2].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
// 	m_btnServerIpSet[2].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
// 	m_btnServerIpSet[2].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
// 	m_btnServerIpSet[2].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
// 	m_btnServerIpSet[2].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
// 	m_btnServerIpSet[2].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
// 	m_btnServerIpSet[2].SetFont(clsFunc.m_pFont[1]);
// 	m_btnServerIpSet[2].SetTooltipText(_T("3 Server IP Address"));

// 	m_btnServerIpSet[3].SetBitmaps(IDC_BTN_SERVER_IP_4, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
// 	m_btnServerIpSet[3].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
// 	m_btnServerIpSet[3].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
// 	m_btnServerIpSet[3].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
// 	m_btnServerIpSet[3].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
// 	m_btnServerIpSet[3].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
// 	m_btnServerIpSet[3].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
// 	m_btnServerIpSet[3].SetFont(clsFunc.m_pFont[1]);
// 	m_btnServerIpSet[3].SetTooltipText(_T("4 Server IP Address"));
	
// 	m_btnServerIpSet[4].SetBitmaps(IDC_BTN_SERVER_IP_5, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
// 	m_btnServerIpSet[4].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
// 	m_btnServerIpSet[4].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
// 	m_btnServerIpSet[4].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
// 	m_btnServerIpSet[4].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
// 	m_btnServerIpSet[4].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
// 	m_btnServerIpSet[4].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
// 	m_btnServerIpSet[4].SetFont(clsFunc.m_pFont[1]);
// 	m_btnServerIpSet[4].SetTooltipText(_T("5 Server IP Address"));

// 	m_btnServerIpSet[5].SetBitmaps(IDC_BTN_SERVER_IP_6, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
// 	m_btnServerIpSet[5].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
// 	m_btnServerIpSet[5].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
// 	m_btnServerIpSet[5].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
// 	m_btnServerIpSet[5].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
// 	m_btnServerIpSet[5].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
// 	m_btnServerIpSet[5].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
// 	m_btnServerIpSet[5].SetFont(clsFunc.m_pFont[1]);
// 	m_btnServerIpSet[5].SetTooltipText(_T("6 Server IP Address"));

// 	m_btnServerIpSet[6].SetBitmaps(IDC_BTN_SERVER_IP_7, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
// 	m_btnServerIpSet[6].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
// 	m_btnServerIpSet[6].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
// 	m_btnServerIpSet[6].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
// 	m_btnServerIpSet[6].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
// 	m_btnServerIpSet[6].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
// 	m_btnServerIpSet[6].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
// 	m_btnServerIpSet[6].SetFont(clsFunc.m_pFont[1]);
// 	m_btnServerIpSet[6].SetTooltipText(_T("7 Server IP Address"));

// 	m_btnServerIpSet[7].SetBitmaps(IDC_BTN_SERVER_IP_8, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
// 	m_btnServerIpSet[7].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
// 	m_btnServerIpSet[7].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
// 	m_btnServerIpSet[7].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
// 	m_btnServerIpSet[7].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
// 	m_btnServerIpSet[7].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
// 	m_btnServerIpSet[7].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
// 	m_btnServerIpSet[7].SetFont(clsFunc.m_pFont[1]);
// 	m_btnServerIpSet[7].SetTooltipText(_T("8 Server IP Address"));

//  m_btnClientPortSet[0].SetBitmaps(IDC_BTN_CLIENT_PORT_1, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
//  m_btnClientPortSet[0].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
//  m_btnClientPortSet[0].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
//  m_btnClientPortSet[0].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
//  m_btnClientPortSet[0].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
//  m_btnClientPortSet[0].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
//  m_btnClientPortSet[0].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
//  m_btnClientPortSet[0].SetFont(clsFunc.m_pFont[1]);
//  m_btnClientPortSet[0].SetTooltipText(_T("1 Client Port"));
//  
//  m_btnClientPortSet[1].SetBitmaps(IDC_BTN_CLIENT_PORT_2, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
//  m_btnClientPortSet[1].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
//  m_btnClientPortSet[1].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
//  m_btnClientPortSet[1].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
//  m_btnClientPortSet[1].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
//  m_btnClientPortSet[1].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
//  m_btnClientPortSet[1].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
//  m_btnClientPortSet[1].SetFont(clsFunc.m_pFont[1]);
//  m_btnClientPortSet[1].SetTooltipText(_T("2 Client Port"));

//  m_btnClientPortSet[2].SetBitmaps(IDC_BTN_CLIENT_PORT_3, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
//  m_btnClientPortSet[2].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
//  m_btnClientPortSet[2].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
//	m_btnClientPortSet[2].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
//  m_btnClientPortSet[2].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
//  m_btnClientPortSet[2].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
//	m_btnClientPortSet[2].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
//	m_btnClientPortSet[2].SetFont(clsFunc.m_pFont[1]);
//	m_btnClientPortSet[2].SetTooltipText(_T("3 Client Port"));

// 	m_btnClientPortSet[3].SetBitmaps(IDC_BTN_CLIENT_PORT_4, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
// 	m_btnClientPortSet[3].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
// 	m_btnClientPortSet[3].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
// 	m_btnClientPortSet[3].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
// 	m_btnClientPortSet[3].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
// 	m_btnClientPortSet[3].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
// 	m_btnClientPortSet[3].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
// 	m_btnClientPortSet[3].SetFont(clsFunc.m_pFont[1]);
// 	m_btnClientPortSet[3].SetTooltipText(_T("4 Client Port"));

// 	m_btnClientPortSet[4].SetBitmaps(IDC_BTN_CLIENT_PORT_5, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
// 	m_btnClientPortSet[4].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
// 	m_btnClientPortSet[4].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
// 	m_btnClientPortSet[4].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
// 	m_btnClientPortSet[4].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
// 	m_btnClientPortSet[4].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
// 	m_btnClientPortSet[4].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
// 	m_btnClientPortSet[4].SetFont(clsFunc.m_pFont[1]);
// 	m_btnClientPortSet[4].SetTooltipText(_T("5 Client Port"));

// 	m_btnClientPortSet[5].SetBitmaps(IDC_BTN_CLIENT_PORT_6, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
// 	m_btnClientPortSet[5].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
// 	m_btnClientPortSet[5].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
// 	m_btnClientPortSet[5].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
// 	m_btnClientPortSet[5].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
// 	m_btnClientPortSet[5].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
// 	m_btnClientPortSet[5].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
// 	m_btnClientPortSet[5].SetFont(clsFunc.m_pFont[1]);
// 	m_btnClientPortSet[5].SetTooltipText(_T("6 Client Port"));

// 	m_btnClientPortSet[6].SetBitmaps(IDC_BTN_CLIENT_PORT_7, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
// 	m_btnClientPortSet[6].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
// 	m_btnClientPortSet[6].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
// 	m_btnClientPortSet[6].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
// 	m_btnClientPortSet[6].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
// 	m_btnClientPortSet[6].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
// 	m_btnClientPortSet[6].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
// 	m_btnClientPortSet[6].SetFont(clsFunc.m_pFont[1]);
// 	m_btnClientPortSet[6].SetTooltipText(_T("7 Client Port"));
	
// 	m_btnClientPortSet[7].SetBitmaps(IDC_BTN_CLIENT_PORT_7, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
// 	m_btnClientPortSet[7].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
// 	m_btnClientPortSet[7].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
// 	m_btnClientPortSet[7].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
// 	m_btnClientPortSet[7].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
// 	m_btnClientPortSet[7].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
// 	m_btnClientPortSet[7].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
// 	m_btnClientPortSet[7].SetFont(clsFunc.m_pFont[1]);
// 	m_btnClientPortSet[7].SetTooltipText(_T("8 Client Port"));

// 	m_btnServerPortSet[0].SetBitmaps(IDC_BTN_SERVER_PORT1, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
// 	m_btnServerPortSet[0].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
// 	m_btnServerPortSet[0].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
// 	m_btnServerPortSet[0].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
// 	m_btnServerPortSet[0].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
// 	m_btnServerPortSet[0].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
// 	m_btnServerPortSet[0].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
// 	m_btnServerPortSet[0].SetFont(clsFunc.m_pFont[1]);
// 	m_btnServerPortSet[0].SetTooltipText(_T("1 Server Port"));

// 	m_btnServerPortSet[1].SetBitmaps(IDC_BTN_SERVER_PORT2, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
// 	m_btnServerPortSet[1].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
// 	m_btnServerPortSet[1].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
// 	m_btnServerPortSet[1].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
// 	m_btnServerPortSet[1].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
// 	m_btnServerPortSet[1].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
// 	m_btnServerPortSet[1].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
// 	m_btnServerPortSet[1].SetFont(clsFunc.m_pFont[1]);
// 	m_btnServerPortSet[1].SetTooltipText(_T("2 Server Port"));

// 	m_btnServerPortSet[2].SetBitmaps(IDC_BTN_SERVER_PORT3, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
// 	m_btnServerPortSet[2].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
// 	m_btnServerPortSet[2].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
// 	m_btnServerPortSet[2].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
// 	m_btnServerPortSet[2].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
// 	m_btnServerPortSet[2].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
// 	m_btnServerPortSet[2].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
// 	m_btnServerPortSet[2].SetFont(clsFunc.m_pFont[1]);
// 	m_btnServerPortSet[2].SetTooltipText(_T("3 Server Port"));

// 	m_btnServerPortSet[3].SetBitmaps(IDC_BTN_SERVER_PORT4, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
// 	m_btnServerPortSet[3].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
// 	m_btnServerPortSet[3].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
// 	m_btnServerPortSet[3].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
// 	m_btnServerPortSet[3].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
// 	m_btnServerPortSet[3].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
// 	m_btnServerPortSet[3].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
// 	m_btnServerPortSet[3].SetFont(clsFunc.m_pFont[1]);
// 	m_btnServerPortSet[3].SetTooltipText(_T("4 Server Port"));

// 	m_btnServerPortSet[4].SetBitmaps(IDC_BTN_SERVER_PORT5, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
// 	m_btnServerPortSet[4].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
// 	m_btnServerPortSet[4].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
// 	m_btnServerPortSet[4].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
// 	m_btnServerPortSet[4].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
// 	m_btnServerPortSet[4].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
// 	m_btnServerPortSet[4].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
// 	m_btnServerPortSet[4].SetFont(clsFunc.m_pFont[1]);
// 	m_btnServerPortSet[4].SetTooltipText(_T("5 Server Port"));

// 	m_btnServerPortSet[5].SetBitmaps(IDC_BTN_SERVER_PORT6, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
// 	m_btnServerPortSet[5].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
// 	m_btnServerPortSet[5].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
// 	m_btnServerPortSet[5].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
// 	m_btnServerPortSet[5].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
// 	m_btnServerPortSet[5].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
// 	m_btnServerPortSet[5].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
// 	m_btnServerPortSet[5].SetFont(clsFunc.m_pFont[1]);
// 	m_btnServerPortSet[5].SetTooltipText(_T("6 Server Port"));

// 	m_btnServerPortSet[6].SetBitmaps(IDC_BTN_SERVER_PORT7, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
// 	m_btnServerPortSet[6].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
// 	m_btnServerPortSet[6].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
// 	m_btnServerPortSet[6].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
// 	m_btnServerPortSet[6].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
// 	m_btnServerPortSet[6].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
// 	m_btnServerPortSet[6].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
// 	m_btnServerPortSet[6].SetFont(clsFunc.m_pFont[1]);
// 	m_btnServerPortSet[6].SetTooltipText(_T("7 Server Port"));

// 	m_btnServerPortSet[7].SetBitmaps(IDC_BTN_SERVER_PORT8, IDB_BITMAP_RECIPE1, WINDOW_DN1, IDB_BITMAP_RECIPE1, WINDOW_UP1);
// 	m_btnServerPortSet[7].SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
// 	m_btnServerPortSet[7].SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
// 	m_btnServerPortSet[7].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
// 	m_btnServerPortSet[7].SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
// 	m_btnServerPortSet[7].SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
// 	m_btnServerPortSet[7].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
// 	m_btnServerPortSet[7].SetFont(clsFunc.m_pFont[1]);
// 	m_btnServerPortSet[7].SetTooltipText(_T("8 Server Port"));

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



// 	if (st_recipe_info.bUnldBcrFlag[0]) m_btnUnldBcr[0].SetWindowTextW(_T("Unload Barcode A Enable"));
// 	else m_btnUnldBcr[0].SetWindowTextW(_T("Unload Barcode A Disable"));
// 
// 	if (st_recipe_info.bUnldBcrFlag[1]) m_btnUnldBcr[1].SetWindowTextW(_T("Unload Barcode B Enable"));
// 	else m_btnUnldBcr[1].SetWindowTextW(_T("Unload Barcode B Disable"));
}			

void CWorkTesterNetwork::OnInterface_Display()
{
	CString str_tmp;

	OnInterfaceIpAddress(m_strClientIp[INTERFACE_NETWORK_1][1]);
	m_clientIp1.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	str_tmp.Format(_T("%d"), m_nClientPort[INTERFACE_NETWORK_1][1]);
	m_msgClientPortData_1.SetWindowText(str_tmp);

	OnInterfaceIpAddress(m_strClientIp[INTERFACE_NETWORK_2][1]);
	m_clientIp2.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	str_tmp.Format(_T("%d"), m_nClientPort[INTERFACE_NETWORK_2][1]);
	m_msgClientPortData_2.SetWindowText(str_tmp);

	OnInterfaceIpAddress(m_strClientIp[INTERFACE_NETWORK_3][1]);
	m_clientIp3.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	str_tmp.Format(_T("%d"), m_nClientPort[INTERFACE_NETWORK_3][1]);
	m_msgClientPortData_3.SetWindowText(str_tmp);

	OnInterfaceIpAddress(m_strClientIp[INTERFACE_NETWORK_4][1]);
	m_clientIp4.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	str_tmp.Format(_T("%d"), m_nClientPort[INTERFACE_NETWORK_4][1]);
	m_msgClientPortData_4.SetWindowText(str_tmp);

	OnInterfaceIpAddress(m_strClientIp[INTERFACE_NETWORK_5][1]);
	m_clientIp5.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	str_tmp.Format(_T("%d"), m_nClientPort[INTERFACE_NETWORK_5][1]);
	m_msgClientPortData_5.SetWindowText(str_tmp);

	OnInterfaceIpAddress(m_strClientIp[INTERFACE_NETWORK_6][1]);
	m_clientIp6.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	str_tmp.Format(_T("%d"), m_nClientPort[INTERFACE_NETWORK_6][1]);
	m_msgClientPortData_6.SetWindowText(str_tmp);

	OnInterfaceIpAddress(m_strClientIp[INTERFACE_NETWORK_7][1]);
	m_clientIp7.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	str_tmp.Format(_T("%d"), m_nClientPort[INTERFACE_NETWORK_7][1]);
	m_msgClientPortData_7.SetWindowText(str_tmp);

	OnInterfaceIpAddress(m_strClientIp[INTERFACE_NETWORK_8][1]);
	m_clientIp8.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	str_tmp.Format(_T("%d"), m_nClientPort[INTERFACE_NETWORK_8][1]);
	m_msgClientPortData_8.SetWindowText(str_tmp);

	OnInterfaceIpAddress(m_strClientIp[EC_FIRST_NETWORK][1]); //ECSERVER
	m_EcServerIp.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	str_tmp.Format(_T("%d"), m_nClientPort[EC_FIRST_NETWORK][1]);
	m_msgEcServerPortData.SetWindowText(str_tmp);

	OnInterfaceIpAddress(m_strClientIp[XGEM_NETWORK][1]); //XGEM
	m_XgemIp.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	str_tmp.Format(_T("%d"), m_nClientPort[XGEM_NETWORK][1]);
	m_msgXgemPortData.SetWindowText(str_tmp);

	/////
// 	OnInterfaceIpAddress(m_strServerIp[0][1]);
// 	m_ServerIp1.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	str_tmp.Format(_T("%d"), m_nServerPort[INTERFACE_NETWORK_1][1]);
	m_msgServerPortData_1.SetWindowText(str_tmp);

//	OnInterfaceIpAddress(m_strServerIp[1][1]);
	//m_ServerIp2.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	str_tmp.Format(_T("%d"), m_nServerPort[INTERFACE_NETWORK_2][1]);
	m_msgServerPortData_2.SetWindowText(str_tmp);

	//OnInterfaceIpAddress(m_strServerIp[2][1]);
	//m_ServerIp3.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	str_tmp.Format(_T("%d"), m_nServerPort[INTERFACE_NETWORK_3][1]);
	m_msgServerPortData_3.SetWindowText(str_tmp);

	//OnInterfaceIpAddress(m_strServerIp[3][1]);
	//m_ServerIp4.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	str_tmp.Format(_T("%d"), m_nServerPort[INTERFACE_NETWORK_4][1]);
	m_msgServerPortData_4.SetWindowText(str_tmp);

	//OnInterfaceIpAddress(m_strServerIp[4][1]);
	//m_ServerIp5.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	str_tmp.Format(_T("%d"), m_nServerPort[INTERFACE_NETWORK_5][1]);
	m_msgServerPortData_5.SetWindowText(str_tmp);

	//OnInterfaceIpAddress(m_strServerIp[5][1]);
	//m_ServerIp6.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	str_tmp.Format(_T("%d"), m_nServerPort[INTERFACE_NETWORK_6][1]);
	m_msgServerPortData_6.SetWindowText(str_tmp);

	//OnInterfaceIpAddress(m_strServerIp[6][1]);
	//m_ServerIp7.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	str_tmp.Format(_T("%d"), m_nServerPort[INTERFACE_NETWORK_7][1]);
	m_msgServerPortData_7.SetWindowText(str_tmp);

//	OnInterfaceIpAddress(m_strServerIp[7][1]);
//	m_ServerIp8.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	str_tmp.Format(_T("%d"), m_nServerPort[INTERFACE_NETWORK_8][1]);
	m_msgServerPortData_8.SetWindowText(str_tmp);
}

void CWorkTesterNetwork::OnInterfaceIpAddress(CString str_ip)
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

void CWorkTesterNetwork::OnDataHistoryLog()
{
	CString strMsg;
	int i = 0;

	for(i = 0; i < 10; i++)
	{
		if (m_strClientIp[i][1]			!= m_strClientIp[i][0])
		{
			strMsg.Format(_T("[Interface] %d Client IP Change %s -> %s"), i + 1, m_strClientIp[i][0], m_strClientIp[i][1]);
			clsFunc.OnLogFileAdd(0, strMsg);
		}

		if (m_nClientPort[i][1]			!= m_nClientPort[i][0])
		{
			strMsg.Format(_T("[Interface] %d Client Port Change %d -> %d"), i + 1, m_nClientPort[i][0], m_nClientPort[i][1]);
			clsFunc.OnLogFileAdd(0, strMsg);
		}

		if (m_nServerPort[i][1]			!= m_nServerPort[i][0])
		{
			strMsg.Format(_T("[Interface] %d Server Port Change %d -> %d"), i + 1, m_nServerPort[i][0], m_nServerPort[i][1]);
			clsFunc.OnLogFileAdd(0, strMsg);
		}

		if (m_nPort[i][1]				!= m_nPort[i][0])
		{
			strMsg.Format(_T("[Interface] %d Serial Port Change %d -> %d"), i + 1, m_nPort[i][0], m_nPort[i][1]);
			clsFunc.OnLogFileAdd(0, strMsg);
		}

		if (m_nRate[i][1]				!= m_nRate[i][0])
		{
			strMsg.Format(_T("[Interface] %d Baudrate Change %d -> %d"), i + 1, m_nRate[i][0], m_nRate[i][1]);
			clsFunc.OnLogFileAdd(0, strMsg);
		}

		if (m_nData[i][1]				!= m_nData[i][0])
		{
			strMsg.Format(_T("[Interface] %d Data Bit Change %d -> %d"), i + 1, m_nData[i][0], m_nData[i][1]);
			clsFunc.OnLogFileAdd(0, strMsg);
		}

		if (m_nParity[i][1]				!= m_nParity[i][0])
		{
			strMsg.Format(_T("[Interface] %d Parity Bit Change %d -> %d"), i + 1, m_nParity[i][0], m_nParity[i][1]);
			clsFunc.OnLogFileAdd(0, strMsg);
		}

		if (m_nStop[i][1]				!= m_nStop[i][0])
		{
			strMsg.Format(_T("[Interface] %d Stop Bit Change %d -> %d"), i + 1, m_nStop[i][0], m_nStop[i][1]);
			clsFunc.OnLogFileAdd(0, strMsg);
		}
	}
	if (m_bUnldBcrFlag[0][1]			!= m_bUnldBcrFlag[0][0])
	{
		strMsg.Format(_T("[Interface]  Unload Barcode A Flag Change %d -> %d"), m_bUnldBcrFlag[0][1], m_bUnldBcrFlag[0][0]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}
	if (m_bUnldBcrFlag[1][1]			!= m_bUnldBcrFlag[1][0])
	{
		strMsg.Format(_T("[Interface]  Unload Barcode B Flag Change %d -> %d"), m_bUnldBcrFlag[1][1], m_bUnldBcrFlag[1][0]);
		clsFunc.OnLogFileAdd(0, strMsg);
	}
}


void CWorkTesterNetwork::OnDataRecovery()
{
	int i = 0;

	for(i = 0; i < 10; i++)
	{
		m_strClientIp[i][1]			= m_strClientIp[i][0];
		m_nClientPort[i][1]			= m_nClientPort[i][0];

		m_nServerPort[i][1]			= m_nServerPort[i][0];
		m_strServerIp[i][1]			= m_strServerIp[i][0];
// 		m_nPort[i][1]				= m_nPort[i][0];
// 		m_nRate[i][1]				= m_nRate[i][0];
// 		m_nData[i][1]				= m_nData[i][0];
// 		m_nParity[i][1]				= m_nParity[i][0];
// 		m_nStop[i][1]				= m_nStop[i][0];
	}
//	m_bUnldBcrFlag[0][1]			= m_bUnldBcrFlag[0][0];
	//m_bUnldBcrFlag[1][1]			= m_bUnldBcrFlag[1][0];
}


int CWorkTesterNetwork::OnDataComparison()
{
	int i = 0;

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

		if (m_nServerPort[i][1]			!= m_nServerPort[i][0])
		{
			return RET_ERROR;
		}

		if (m_strServerIp[i][1]			!= m_strServerIp[i][0])
		{
			return RET_ERROR;
		}

		if (m_strClientName[i][1]			!= m_strClientName[i][0])
		{
			return RET_ERROR;
		}

		if (m_strServerName[i][1]			!= m_strServerName[i][0])
		{
			return RET_ERROR;
		}
	}
	
	for (i = 0; i<16; i++)
	{
		if (m_strTsiteSerial[i][1] != m_strTsiteSerial[i][0])
		{
			return RET_ERROR;
		}
	}

	return RET_GOOD;
}


void CWorkTesterNetwork::OnDataApply()
{
	//2017.0109
	int i = 0;
	for(i = 0; i < 10; i++)
	{
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

			clsLog.LogConfig(_T("WORKFTP"), _T("CHANGE"), st_basic_info.strDeviceName, 4, m_strLogKey, m_strLogData);
		}

	}

	for(i = 0; i < 8; i++)
	{
		if (st_server_info[i].nPort	!= m_nServerPort[i][1])
		{
			m_strLogKey[0]	= _T("TYPE");
			m_strLogData[0]	= _T("SERVER PORT");
			m_strLogKey[2]	= _T("OLD");
			m_strLogData[2].Format(_T("%d"), st_server_info[i].nPort);
			m_strLogKey[3]	= _T("NEW");
			m_strLogData[3].Format(_T("%d"), m_nServerPort[i][1]);

			clsLog.LogConfig(_T("WORKFTP"), _T("CHANGE"), st_basic_info.strDeviceName, 4, m_strLogKey, m_strLogData);
		}

		if (st_test_site_info[i].strEqpID	!= m_strClientName[i][1])
		{
			m_strLogKey[0]	= _T("TYPE");
			m_strLogData[0]	= _T("TESTER CLIENT NAME");
			m_strLogKey[1]	= _T("CLIENT NAME");
			m_strLogData[1].Format(_T("%d"), i + 1);
			m_strLogKey[2]	= _T("OLD");
			m_strLogData[2].Format(_T("%s"), st_test_site_info[i].strEqpID);
			m_strLogKey[3]	= _T("NEW");
			m_strLogData[3].Format(_T("%s"), m_strClientName[i][1]);

			clsLog.LogConfig(_T("WORKFTP"), _T("CHANGE"), st_basic_info.strDeviceName, 4, m_strLogKey, m_strLogData);
		}
		if (st_client_info[i].strName	!= st_test_site_info[i].strEqpID)
		{
			m_strLogKey[0]	= _T("TYPE");
			m_strLogData[0]	= _T("CLIENT IP");
			m_strLogKey[1]	= _T("CLIENT NUM");
			m_strLogData[1].Format(_T("%d"), i + 1);
			m_strLogKey[2]	= _T("OLD");
			m_strLogData[2].Format(_T("%s"), st_client_info[i].strName);
			m_strLogKey[3]	= _T("NEW");
			m_strLogData[3].Format(_T("%s"), st_test_site_info[i].strEqpID);

			clsLog.LogConfig(_T("WORKFTP"), _T("CHANGE"), st_basic_info.strDeviceName, 4, m_strLogKey, m_strLogData);
		}

		if (st_test_site_info[i].strTsiteSerial[0]	!= m_strTsiteSerial[i][0])
		{
			m_strLogKey[0]	= _T("TYPE");
			m_strLogData[0]	= _T("CLIENT IP");
			m_strLogKey[1]	= _T("CLIENT NUM");
			m_strLogData[1].Format(_T("%d"), i + 1);
			m_strLogKey[2]	= _T("OLD");
			m_strLogData[2].Format(_T("%s"), st_test_site_info[i].strTsiteSerial[0]);
			m_strLogKey[3]	= _T("NEW");
			m_strLogData[3].Format(_T("%s"), m_strTsiteSerial[i][0]);

			clsLog.LogConfig(_T("WORKFTP"), _T("CHANGE"), st_basic_info.strDeviceName, 4, m_strLogKey, m_strLogData);
		}
		if (st_test_site_info[i].strTsiteSerial[1]	!= m_strTsiteSerial[i][1])
		{
			m_strLogKey[0]	= _T("TYPE");
			m_strLogData[0]	= _T("CLIENT IP");
			m_strLogKey[1]	= _T("CLIENT NUM");
			m_strLogData[1].Format(_T("%d"), i + 1);
			m_strLogKey[2]	= _T("OLD");
			m_strLogData[2].Format(_T("%s"), st_test_site_info[i].strTsiteSerial[1]);
			m_strLogKey[3]	= _T("NEW");
			m_strLogData[3].Format(_T("%s"), m_strTsiteSerial[i][1]);

			clsLog.LogConfig(_T("WORKFTP"), _T("CHANGE"), st_basic_info.strDeviceName, 4, m_strLogKey, m_strLogData);
		}
	}


	st_client_info[EC_FIRST_NETWORK].strIp			=	m_strClientIp[EC_FIRST_NETWORK][1];
	st_client_info[EC_FIRST_NETWORK].nPort			=	m_nClientPort[EC_FIRST_NETWORK][1];

	st_client_info[XGEM_NETWORK].strIp				=	m_strClientIp[XGEM_NETWORK][1];
	st_client_info[XGEM_NETWORK].nPort				=	m_nClientPort[XGEM_NETWORK][1];

	st_client_info[INTERFACE_NETWORK_1].strIp		=	m_strClientIp[INTERFACE_NETWORK_1][1];
	st_client_info[INTERFACE_NETWORK_1].nPort		=	m_nClientPort[INTERFACE_NETWORK_1][1];

	st_client_info[INTERFACE_NETWORK_2].strIp		=	m_strClientIp[INTERFACE_NETWORK_2][1];
	st_client_info[INTERFACE_NETWORK_2].nPort		=	m_nClientPort[INTERFACE_NETWORK_2][1];

	st_client_info[INTERFACE_NETWORK_3].strIp		=	m_strClientIp[INTERFACE_NETWORK_3][1];
	st_client_info[INTERFACE_NETWORK_3].nPort		=	m_nClientPort[INTERFACE_NETWORK_3][1];

	st_client_info[INTERFACE_NETWORK_4].strIp		=	m_strClientIp[INTERFACE_NETWORK_4][1];
	st_client_info[INTERFACE_NETWORK_4].nPort		=	m_nClientPort[INTERFACE_NETWORK_4][1];

	st_client_info[INTERFACE_NETWORK_5].strIp		=	m_strClientIp[INTERFACE_NETWORK_5][1];
	st_client_info[INTERFACE_NETWORK_5].nPort		=	m_nClientPort[INTERFACE_NETWORK_5][1];

	st_client_info[INTERFACE_NETWORK_6].strIp		=	m_strClientIp[INTERFACE_NETWORK_6][1];
	st_client_info[INTERFACE_NETWORK_6].nPort		=	m_nClientPort[INTERFACE_NETWORK_6][1];

	st_client_info[INTERFACE_NETWORK_7].strIp		=	m_strClientIp[INTERFACE_NETWORK_7][1];
	st_client_info[INTERFACE_NETWORK_7].nPort		=	m_nClientPort[INTERFACE_NETWORK_7][1];

	st_client_info[INTERFACE_NETWORK_8].strIp		=	m_strClientIp[INTERFACE_NETWORK_8][1];
	st_client_info[INTERFACE_NETWORK_8].nPort		=	m_nClientPort[INTERFACE_NETWORK_8][1];

	//Server Port
	st_server_info[INTERFACE_NETWORK_1].nPort		=	m_nServerPort[INTERFACE_NETWORK_1][1];
	st_server_info[INTERFACE_NETWORK_2].nPort		=	m_nServerPort[INTERFACE_NETWORK_2][1];
	st_server_info[INTERFACE_NETWORK_3].nPort		=	m_nServerPort[INTERFACE_NETWORK_3][1];
	st_server_info[INTERFACE_NETWORK_4].nPort		=	m_nServerPort[INTERFACE_NETWORK_4][1];
	st_server_info[INTERFACE_NETWORK_5].nPort		=	m_nServerPort[INTERFACE_NETWORK_5][1];
	st_server_info[INTERFACE_NETWORK_6].nPort		=	m_nServerPort[INTERFACE_NETWORK_6][1];
	st_server_info[INTERFACE_NETWORK_7].nPort		=	m_nServerPort[INTERFACE_NETWORK_7][1];
	st_server_info[INTERFACE_NETWORK_8].nPort		=	m_nServerPort[INTERFACE_NETWORK_8][1];

	//Test Site Eqp
	st_test_site_info[THD_TESTSITE_1].strEqpID		=	m_strClientName[INTERFACE_NETWORK_1][1];
	st_test_site_info[THD_TESTSITE_2].strEqpID		=	m_strClientName[INTERFACE_NETWORK_2][1];
	st_test_site_info[THD_TESTSITE_3].strEqpID		=	m_strClientName[INTERFACE_NETWORK_3][1];
	st_test_site_info[THD_TESTSITE_4].strEqpID		=	m_strClientName[INTERFACE_NETWORK_4][1];
	st_test_site_info[THD_TESTSITE_5].strEqpID		=	m_strClientName[INTERFACE_NETWORK_5][1];
	st_test_site_info[THD_TESTSITE_6].strEqpID		=	m_strClientName[INTERFACE_NETWORK_6][1];
	st_test_site_info[THD_TESTSITE_7].strEqpID		=	m_strClientName[INTERFACE_NETWORK_7][1];
	st_test_site_info[THD_TESTSITE_8].strEqpID		=	m_strClientName[INTERFACE_NETWORK_8][1];

	st_client_info[INTERFACE_NETWORK_1].strName		=	st_test_site_info[THD_TESTSITE_1].strEqpID;
	st_client_info[INTERFACE_NETWORK_2].strName		=	st_test_site_info[THD_TESTSITE_2].strEqpID;
	st_client_info[INTERFACE_NETWORK_3].strName		=	st_test_site_info[THD_TESTSITE_3].strEqpID;
	st_client_info[INTERFACE_NETWORK_4].strName		=	st_test_site_info[THD_TESTSITE_4].strEqpID;
	st_client_info[INTERFACE_NETWORK_5].strName		=	st_test_site_info[THD_TESTSITE_5].strEqpID;
	st_client_info[INTERFACE_NETWORK_6].strName		=	st_test_site_info[THD_TESTSITE_6].strEqpID;
	st_client_info[INTERFACE_NETWORK_7].strName		=	st_test_site_info[THD_TESTSITE_7].strEqpID;
	st_client_info[INTERFACE_NETWORK_8].strName		=	st_test_site_info[THD_TESTSITE_8].strEqpID;

	//kwlee 2016.1125 Tsite Serial
	st_test_site_info[THD_TESTSITE_1].strTsiteSerial[0]		=	m_strTsiteSerial[TSITE_SERIAL_1_1][1];
	st_test_site_info[THD_TESTSITE_2].strTsiteSerial[0]		=	m_strTsiteSerial[TSITE_SERIAL_2_1][1];
	st_test_site_info[THD_TESTSITE_3].strTsiteSerial[0]		=	m_strTsiteSerial[TSITE_SERIAL_3_1][1];
	st_test_site_info[THD_TESTSITE_4].strTsiteSerial[0]		=	m_strTsiteSerial[TSITE_SERIAL_4_1][1];
	st_test_site_info[THD_TESTSITE_5].strTsiteSerial[0]		=	m_strTsiteSerial[TSITE_SERIAL_5_1][1];
	st_test_site_info[THD_TESTSITE_6].strTsiteSerial[0]		=	m_strTsiteSerial[TSITE_SERIAL_6_1][1];
	st_test_site_info[THD_TESTSITE_7].strTsiteSerial[0]		=	m_strTsiteSerial[TSITE_SERIAL_7_1][1];
	st_test_site_info[THD_TESTSITE_8].strTsiteSerial[0]		=	m_strTsiteSerial[TSITE_SERIAL_8_1][1];

	st_test_site_info[THD_TESTSITE_1].strTsiteSerial[1]		=	m_strTsiteSerial[TSITE_SERIAL_1_2][1];
	st_test_site_info[THD_TESTSITE_2].strTsiteSerial[1]		=	m_strTsiteSerial[TSITE_SERIAL_2_2][1];
	st_test_site_info[THD_TESTSITE_3].strTsiteSerial[1]		=	m_strTsiteSerial[TSITE_SERIAL_3_2][1];
	st_test_site_info[THD_TESTSITE_4].strTsiteSerial[1]		=	m_strTsiteSerial[TSITE_SERIAL_4_2][1];
	st_test_site_info[THD_TESTSITE_5].strTsiteSerial[1]		=	m_strTsiteSerial[TSITE_SERIAL_5_2][1];
	st_test_site_info[THD_TESTSITE_6].strTsiteSerial[1]		=	m_strTsiteSerial[TSITE_SERIAL_6_2][1];
	st_test_site_info[THD_TESTSITE_7].strTsiteSerial[1]		=	m_strTsiteSerial[TSITE_SERIAL_7_2][1];
	st_test_site_info[THD_TESTSITE_8].strTsiteSerial[1]		=	m_strTsiteSerial[TSITE_SERIAL_8_2][1];
	//



	//for(i = 0; i < 10; i++)
	//{
//		st_client_info[i].strIp		= m_strClientIp[i][1];
//		st_client_info[i].nPort		= m_nClientPort[i][1];

	//	st_server_info[i].nPort		= m_nServerPort[i][1];
	//	st_server_info[i].strIp		= m_strServerIp[i][1];

		//st_client_info[i].strName	= m_strClientName[i][1]; 
		//st_server_info[i].strName	= m_strServerName[i][1];

		//st_client_info[i].strName	= m_strClientName[i][1]; 

		
	//}
}


void CWorkTesterNetwork::OnDataBackup()
{
	int i = 0;

	for(i = 0; i < 20; i++)
	{
		m_strClientIp[i][0]			= m_strClientIp[i][1];
		m_nClientPort[i][0]			= m_nClientPort[i][1];

		m_nServerPort[i][0]			= m_nServerPort[i][1];
		m_strServerIp[i][0]			= m_strServerIp[i][1];

		m_strClientName[i][0]	    = m_strClientName[i][1];
		m_strServerName[i][0]	    = m_strServerName[i][1];	
	}

	//kwlee 2016.1125 
	for (i = 0; i<16; i++)
	{
		m_strTsiteSerial[i][0]      = m_strTsiteSerial[i][1]; 
	}
}


void CWorkTesterNetwork::OnDataInit()
{

	m_strClientIp[EC_FIRST_NETWORK][1]		=	st_client_info[EC_FIRST_NETWORK].strIp;
	m_nClientPort[EC_FIRST_NETWORK][1]		=	st_client_info[EC_FIRST_NETWORK].nPort;

	m_strClientIp[XGEM_NETWORK][1]			=	st_client_info[XGEM_NETWORK].strIp;
	m_nClientPort[XGEM_NETWORK][1]			=	st_client_info[XGEM_NETWORK].nPort;

	m_strClientIp[INTERFACE_NETWORK_1][1]	=	st_client_info[INTERFACE_NETWORK_1].strIp;
	m_nClientPort[INTERFACE_NETWORK_1][1]	=	st_client_info[INTERFACE_NETWORK_1].nPort;

	m_strClientIp[INTERFACE_NETWORK_2][1]	=	st_client_info[INTERFACE_NETWORK_2].strIp;
	m_nClientPort[INTERFACE_NETWORK_2][1]	=	st_client_info[INTERFACE_NETWORK_2].nPort;

	m_strClientIp[INTERFACE_NETWORK_3][1]	=	st_client_info[INTERFACE_NETWORK_3].strIp;
	m_nClientPort[INTERFACE_NETWORK_3][1]	=	st_client_info[INTERFACE_NETWORK_3].nPort;

	m_strClientIp[INTERFACE_NETWORK_4][1]	=	st_client_info[INTERFACE_NETWORK_4].strIp;
	m_nClientPort[INTERFACE_NETWORK_4][1]	=	st_client_info[INTERFACE_NETWORK_4].nPort;

	m_strClientIp[INTERFACE_NETWORK_5][1]	=	st_client_info[INTERFACE_NETWORK_5].strIp;
	m_nClientPort[INTERFACE_NETWORK_5][1]	=	st_client_info[INTERFACE_NETWORK_5].nPort;

	m_strClientIp[INTERFACE_NETWORK_6][1]	=	st_client_info[INTERFACE_NETWORK_6].strIp;
	m_nClientPort[INTERFACE_NETWORK_6][1]	=	st_client_info[INTERFACE_NETWORK_6].nPort;

	m_strClientIp[INTERFACE_NETWORK_7][1]	=	st_client_info[INTERFACE_NETWORK_7].strIp;
	m_nClientPort[INTERFACE_NETWORK_7][1]	=	st_client_info[INTERFACE_NETWORK_7].nPort;

	m_strClientIp[INTERFACE_NETWORK_8][1]	=	st_client_info[INTERFACE_NETWORK_8].strIp;
	m_nClientPort[INTERFACE_NETWORK_8][1]	=	st_client_info[INTERFACE_NETWORK_8].nPort;

	//Server Port
	m_nServerPort[INTERFACE_NETWORK_1][1]	=	st_server_info[INTERFACE_NETWORK_1].nPort;
	m_nServerPort[INTERFACE_NETWORK_2][1]	=	st_server_info[INTERFACE_NETWORK_2].nPort;
	m_nServerPort[INTERFACE_NETWORK_3][1]	=	st_server_info[INTERFACE_NETWORK_3].nPort;
	m_nServerPort[INTERFACE_NETWORK_4][1]	=	st_server_info[INTERFACE_NETWORK_4].nPort;
	m_nServerPort[INTERFACE_NETWORK_5][1]	=	st_server_info[INTERFACE_NETWORK_5].nPort;
	m_nServerPort[INTERFACE_NETWORK_6][1]	=	st_server_info[INTERFACE_NETWORK_6].nPort;
	m_nServerPort[INTERFACE_NETWORK_7][1]	=	st_server_info[INTERFACE_NETWORK_7].nPort;
	m_nServerPort[INTERFACE_NETWORK_8][1]	=	st_server_info[INTERFACE_NETWORK_8].nPort;


	//Test Site Eqp
	st_test_site_info[THD_TESTSITE_1].strEqpID	=	st_client_info[INTERFACE_NETWORK_1].strName;
	st_test_site_info[THD_TESTSITE_2].strEqpID	=	st_client_info[INTERFACE_NETWORK_2].strName;
	st_test_site_info[THD_TESTSITE_3].strEqpID	=	st_client_info[INTERFACE_NETWORK_3].strName;
	st_test_site_info[THD_TESTSITE_4].strEqpID	=	st_client_info[INTERFACE_NETWORK_4].strName;
	st_test_site_info[THD_TESTSITE_5].strEqpID	=	st_client_info[INTERFACE_NETWORK_5].strName;
	st_test_site_info[THD_TESTSITE_6].strEqpID	=	st_client_info[INTERFACE_NETWORK_6].strName;
	st_test_site_info[THD_TESTSITE_7].strEqpID	=	st_client_info[INTERFACE_NETWORK_7].strName;
	st_test_site_info[THD_TESTSITE_8].strEqpID	=	st_client_info[INTERFACE_NETWORK_8].strName;

	m_strClientName[INTERFACE_NETWORK_1][1]		=	st_test_site_info[THD_TESTSITE_1].strEqpID;
	m_strClientName[INTERFACE_NETWORK_2][1]		=	st_test_site_info[THD_TESTSITE_2].strEqpID;
	m_strClientName[INTERFACE_NETWORK_3][1]		=	st_test_site_info[THD_TESTSITE_3].strEqpID;
	m_strClientName[INTERFACE_NETWORK_4][1]		=	st_test_site_info[THD_TESTSITE_4].strEqpID;
	m_strClientName[INTERFACE_NETWORK_5][1]		=	st_test_site_info[THD_TESTSITE_5].strEqpID;
	m_strClientName[INTERFACE_NETWORK_6][1]		=	st_test_site_info[THD_TESTSITE_6].strEqpID;
	m_strClientName[INTERFACE_NETWORK_7][1]		=	st_test_site_info[THD_TESTSITE_7].strEqpID;
	m_strClientName[INTERFACE_NETWORK_8][1]		=	st_test_site_info[THD_TESTSITE_8].strEqpID;


	//kwlee 2016.1125 Tsite Serial
	m_strTsiteSerial[TSITE_SERIAL_1_1][1]		=	st_test_site_info[THD_TESTSITE_1].strTsiteSerial[0];
	m_strTsiteSerial[TSITE_SERIAL_2_1][1]		=	st_test_site_info[THD_TESTSITE_2].strTsiteSerial[0];
	m_strTsiteSerial[TSITE_SERIAL_3_1][1]		=	st_test_site_info[THD_TESTSITE_3].strTsiteSerial[0];
	m_strTsiteSerial[TSITE_SERIAL_4_1][1]		=	st_test_site_info[THD_TESTSITE_4].strTsiteSerial[0];
	m_strTsiteSerial[TSITE_SERIAL_5_1][1]		=	st_test_site_info[THD_TESTSITE_5].strTsiteSerial[0];
	m_strTsiteSerial[TSITE_SERIAL_6_1][1]		=	st_test_site_info[THD_TESTSITE_6].strTsiteSerial[0];
	m_strTsiteSerial[TSITE_SERIAL_7_1][1]		=	st_test_site_info[THD_TESTSITE_7].strTsiteSerial[0];
	m_strTsiteSerial[TSITE_SERIAL_8_1][1]		=	st_test_site_info[THD_TESTSITE_8].strTsiteSerial[0];

	m_strTsiteSerial[TSITE_SERIAL_1_2][1]	=		st_test_site_info[THD_TESTSITE_1].strTsiteSerial[1];
	m_strTsiteSerial[TSITE_SERIAL_2_2][1]	=		st_test_site_info[THD_TESTSITE_2].strTsiteSerial[1];
	m_strTsiteSerial[TSITE_SERIAL_3_2][1]	=		st_test_site_info[THD_TESTSITE_3].strTsiteSerial[1];
	m_strTsiteSerial[TSITE_SERIAL_4_2][1]	=		st_test_site_info[THD_TESTSITE_4].strTsiteSerial[1];
	m_strTsiteSerial[TSITE_SERIAL_5_2][1]	=		st_test_site_info[THD_TESTSITE_5].strTsiteSerial[1];
	m_strTsiteSerial[TSITE_SERIAL_6_2][1]	=		st_test_site_info[THD_TESTSITE_6].strTsiteSerial[1];
	m_strTsiteSerial[TSITE_SERIAL_7_2][1]	=		st_test_site_info[THD_TESTSITE_7].strTsiteSerial[1];
	m_strTsiteSerial[TSITE_SERIAL_8_2][1]	=		st_test_site_info[THD_TESTSITE_8].strTsiteSerial[1];
// 
// 	for(int i = 0; i < 10; i++)
// 	{
// //		m_strClientIp[i][1] = st_client_info[i].strIp;
// //		m_nClientPort[i][1] = st_client_info[i].nPort;
// 
// 		m_nServerPort[i][1] = st_server_info[i].nPort;
// 		m_strServerIp[i][1] = st_server_info[i].strIp;
// 
// 		m_strServerName[i][1] = st_server_info[i].strName;
// 		m_strClientName[i][1] = st_client_info[i].strName;
// 
// 	}
// 

	OnDataBackup();
}
void CWorkTesterNetwork::OnWriteName()
{
	CString strClient[10];
	CString strServer[10];
	
// 	m_editClientName1.GetWindowText(strClient[0]);
// 	m_editClientName2.GetWindowText(strClient[1]);
// 	m_editClientName3.GetWindowText(strClient[2]);
// 	m_editClientName4.GetWindowText(strClient[3]);
// 	m_editClientName5.GetWindowText(strClient[4]);
// 	m_editClientName6.GetWindowText(strClient[5]);
// 	m_editClientName7.GetWindowText(strClient[6]);
// 	m_editClientName8.GetWindowText(strClient[7]);
// 
// 	m_editServerName1.GetWindowText(strServer[0]);
// 	m_editServerName2.GetWindowText(strServer[1]);
// 	m_editServerName3.GetWindowText(strServer[2]);
// 	m_editServerName4.GetWindowText(strServer[3]);
// 	m_editServerName5.GetWindowText(strServer[4]);
// 	m_editServerName6.GetWindowText(strServer[5]);
// 	m_editServerName7.GetWindowText(strServer[6]);
// 	m_editServerName8.GetWindowText(strServer[7]);
// 
// 	for (int i =0; i<10; i++)
// 	{
// 		m_strClientName[i][1] = strClient[i];
// 		m_strServerName[i][1] = strServer[i];
// 	}
}

void CWorkTesterNetwork::OnInitName()
{
	CString str_tmp;


	//m_msgServerID_1.SetWindowText(m_strServerName[0][1]);
	//m_msgServerID_2.SetWindowText(m_strServerName[1][1]);
	//m_msgServerID_3.SetWindowText(m_strServerName[2][1]);
	//m_msgServerID_4.SetWindowText(m_strServerName[3][1]);
	//m_msgServerID_5.SetWindowText(m_strServerName[4][1]);
	//m_msgServerID_6.SetWindowText(m_strServerName[5][1]);
	//m_msgServerID_7.SetWindowText(m_strServerName[6][1]);
	//m_msgServerID_8.SetWindowText(m_strServerName[7][1]);

	
	m_msgClientID_1.SetWindowText(m_strClientName[INTERFACE_NETWORK_1][1]);
	m_msgClientID_2.SetWindowText(m_strClientName[INTERFACE_NETWORK_2][1]);
	m_msgClientID_3.SetWindowText(m_strClientName[INTERFACE_NETWORK_3][1]);
	m_msgClientID_4.SetWindowText(m_strClientName[INTERFACE_NETWORK_4][1]);
	m_msgClientID_5.SetWindowText(m_strClientName[INTERFACE_NETWORK_5][1]);
	m_msgClientID_6.SetWindowText(m_strClientName[INTERFACE_NETWORK_6][1]);
	m_msgClientID_7.SetWindowText(m_strClientName[INTERFACE_NETWORK_7][1]);
	m_msgClientID_8.SetWindowText(m_strClientName[INTERFACE_NETWORK_8][1]);

	//kwlee 2016.1125
	m_msgSerialID_1_1.SetWindowText(m_strTsiteSerial[TSITE_SERIAL_1_1][1]);
	m_msgSerialID_2_1.SetWindowText(m_strTsiteSerial[TSITE_SERIAL_2_1][1]);
	m_msgSerialID_3_1.SetWindowText(m_strTsiteSerial[TSITE_SERIAL_3_1][1]);
	m_msgSerialID_4_1.SetWindowText(m_strTsiteSerial[TSITE_SERIAL_4_1][1]);
	m_msgSerialID_5_1.SetWindowText(m_strTsiteSerial[TSITE_SERIAL_5_1][1]);
	m_msgSerialID_6_1.SetWindowText(m_strTsiteSerial[TSITE_SERIAL_6_1][1]);
	m_msgSerialID_7_1.SetWindowText(m_strTsiteSerial[TSITE_SERIAL_7_1][1]);
	m_msgSerialID_8_1.SetWindowText(m_strTsiteSerial[TSITE_SERIAL_8_1][1]);

	m_msgSerialID_1_2.SetWindowText(m_strTsiteSerial[TSITE_SERIAL_1_2][1]);
	m_msgSerialID_2_2.SetWindowText(m_strTsiteSerial[TSITE_SERIAL_2_2][1]);
	m_msgSerialID_3_2.SetWindowText(m_strTsiteSerial[TSITE_SERIAL_3_2][1]);
	m_msgSerialID_4_2.SetWindowText(m_strTsiteSerial[TSITE_SERIAL_4_2][1]);
	m_msgSerialID_5_2.SetWindowText(m_strTsiteSerial[TSITE_SERIAL_5_2][1]);
	m_msgSerialID_6_2.SetWindowText(m_strTsiteSerial[TSITE_SERIAL_6_2][1]);
	m_msgSerialID_7_2.SetWindowText(m_strTsiteSerial[TSITE_SERIAL_7_2][1]);
	m_msgSerialID_8_2.SetWindowText(m_strTsiteSerial[TSITE_SERIAL_8_2][1]);
	/////

}
BOOL CWorkTesterNetwork::OnEraseBkgnd(CDC* pDC)
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


//void CWorkInterface::OnBnClickedBtnClientIp1()
//{
// 	CString strKey;
// 
// 	KeyBoard(&strKey);

// 	if (strKey == _T("")) return;
// 
// 	m_strClientIp[0][1].MakeUpper();
// 	m_strClientIp[0][1].TrimLeft(' ');               
// 	m_strClientIp[0][1].TrimRight(' ');
// 
// 	OnInterfaceIpAddress(m_strClientIp[0][1]);
// 
// 	m_clientIp1.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
//}


//void CWorkInterface::OnBnClickedBtnClientPort1()
//{
// 	CString strTemp;
// 
// 	int nKey = m_nClientPort[0][1];
// 
// 	KeyPadI(1, 100000, &nKey);
// 
// 	m_nClientPort[0][1] = nKey;
// 	strTemp.Format(_T("%d"), nKey);
// 
// 	m_editClientPort1.SetWindowText(strTemp);
//}

// 
// void CWorkInterface::OnBnClickedBtnClientIp2()
// {
// 	CString strKey;
// 
// 	KeyBoard(&strKey);
// 
// 	if (strKey == _T("")) return;
// 
// 	m_strClientIp[1][1].MakeUpper();
// 	m_strClientIp[1][1].TrimLeft(' ');               
// 	m_strClientIp[1][1].TrimRight(' ');
// 
// 	OnInterfaceIpAddress(m_strClientIp[1][1]);
// 
// 	m_clientIp2.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
// }


//void CWorkInterface::OnBnClickedBtnClientPort2()
//{
// 	CString strTemp;
// 
// 	int nKey = m_nClientPort[1][1];
// 
// 	KeyPadI(1, 100000, &nKey);
// 
// 	m_nClientPort[1][1] = nKey;
// 	strTemp.Format(_T("%d"), nKey);
// 
// 	m_editClientPort2.SetWindowText(strTemp);
//}


//void CWorkInterface::OnBnClickedBtnClientIp3()
//{
// 	CString strKey;
// 
// 	KeyBoard(&strKey);
// 
// 	if (strKey == _T("")) return;
// 
// 	m_strClientIp[2][1] = strKey;
// 
// 	m_strClientIp[2][1].MakeUpper();
// 	m_strClientIp[2][1].TrimLeft(' ');               
// 	m_strClientIp[2][1].TrimRight(' ');
// 
// 	OnInterfaceIpAddress(m_strClientIp[2][1]);
// 
// 	m_clientIp3.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
//}


//void CWorkInterface::OnBnClickedBtnClientPort3()
//{
// 	CString strTemp;
// 
// 	int nKey = m_nClientPort[2][1];
// 
// 	KeyPadI(1, 100000, &nKey);
// 
// 	m_nClientPort[2][1] = nKey;
// 	strTemp.Format(_T("%d"), nKey);
// 
// 	m_editClientPort3.SetWindowText(strTemp);
//}

//void CWorkInterface::OnBnClickedBtnClientIp4()
//{
// 	CString strKey;
// 
// 	KeyBoard(&strKey);
// 
// 	if (strKey == _T("")) return;
// 
// 	m_strClientIp[3][1] = strKey;
// 
// 	m_strClientIp[3][1].MakeUpper();
// 	m_strClientIp[3][1].TrimLeft(' ');               
// 	m_strClientIp[3][1].TrimRight(' ');
// 
// 	OnInterfaceIpAddress(m_strClientIp[3][1]);
// 
// 	m_clientIp4.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
//}


//void CWorkInterface::OnBnClickedBtnClientPort4()
//{
// 	CString strTemp;
// 
// 	int nKey = m_nServerPort[3][1];
// 
// 	KeyPadI(1, 100000, &nKey);
// 
// 	m_nServerPort[3][1] = nKey;
// 	strTemp.Format(_T("%d"), nKey);
// 
// 	m_editClientPort4.SetWindowText(strTemp);
//}


//void CWorkInterface::OnBnClickedBtnClientIp5()
//{
// 	CString strKey;
// 
// 	KeyBoard(&strKey);
// 
// 	if (strKey == _T("")) return;
// 
// 	m_strClientIp[4][1] = strKey;
// 
// 	m_strClientIp[4][1].MakeUpper();
// 	m_strClientIp[4][1].TrimLeft(' ');               
// 	m_strClientIp[4][1].TrimRight(' ');
// 
// 	OnInterfaceIpAddress(m_strClientIp[4][1]);
// 
// 	m_clientIp5.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
//}


//void CWorkInterface::OnBnClickedBtnClientPort5()
//{
// 	CString strTemp; 
// 	int nKey = m_nClientPort[4][1];
// 	KeyPadI(1, 100000, &nKey);
// 	m_nClientPort[4][1] = nKey;
// 	strTemp.Format(_T("%d"), nKey);

// 	m_editClientPort5.SetWindowText(strTemp);
////}

//void CWorkInterface::OnBnClickedBtnServerPort1()
//{
// 	CString strTemp;

// 	int nKey = m_nServerPort[2][1];
 
// 	KeyPadI(1, 100000, &nKey);

// 	m_nServerPort[2][1] = nKey;
// 	strTemp.Format(_T("%d"), nKey);

// 	m_editServerPort1.SetWindowText(strTemp);
//}

//void CWorkInterface::OnBnClickedBtnClientIp6()
//{
// 	CString strKey;
// 
// 	KeyBoard(&strKey);
// 
// 	if (strKey == _T("")) return;
// 
// 	m_strClientIp[6][1] = strKey;
// 
// 	m_strClientIp[6][1].MakeUpper();
// 	m_strClientIp[6][1].TrimLeft(' ');               
// 	m_strClientIp[6][1].TrimRight(' ');
// 
// 	OnInterfaceIpAddress(m_strClientIp[6][1]);
// 
// 	m_clientIp6.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
//}


///void CWorkInterface::OnBnClickedBtnClientPort6()
//{
// 	CString strTemp;
// 
// 	int nKey = m_nClientPort[6][1];
// 
// 	KeyPadI(1, 100000, &nKey);
// 
// 	m_nClientPort[6][1] = nKey;
// 	strTemp.Format(_T("%d"), nKey);
// 
// 	m_editClientPort6.SetWindowText(strTemp);
//}

//void CWorkInterface::OnBnClickedBtnClientIp7()
//{
// 	CString strKey;
// 
// 	KeyBoard(&strKey);
// 
// 	if (strKey == _T("")) return;
// 
// 	m_strClientIp[7][1] = strKey;
// 
// 	m_strClientIp[7][1].MakeUpper();
// 	m_strClientIp[7][1].TrimLeft(' ');               
// 	m_strClientIp[7][1].TrimRight(' ');
// 
// 	OnInterfaceIpAddress(m_strClientIp[7][1]);
// 
// 	m_clientIp7.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
//}


//void CWorkInterface::OnBnClickedBtnClientPort7()
//{
// 	CString strTemp;
// 
// 	int nKey = m_nServerPort[7][1];
// 
// 	KeyPadI(1, 100000, &nKey);
// 
// 	m_nServerPort[7][1] = nKey;
// 	strTemp.Format(_T("%d"), nKey);
// 
// 	m_editClientPort7.SetWindowText(strTemp);
//}


void CWorkTesterNetwork::OnBnClickedBtnApply()
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


void CWorkTesterNetwork::OnBnClickedBtnReload()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnDataRecovery();
	OnInterface_Display();
}


void CWorkTesterNetwork::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (OnDataComparison() == RET_ERROR)
	{
		OnDataHistoryLog();
		OnDataApply();
	}
	CDialog::PostNcDestroy();
}


void CWorkTesterNetwork::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (OnDataComparison() == RET_ERROR)
	{
		OnDataHistoryLog();
		OnDataApply();
	}
}



int	CWorkTesterNetwork::OnRfidRead()
{
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
			st_client_info[RFID_NETWORK].strSend		= clsRfid.OnMakeReadCodeAll(1);
			
			//::PostMessage(st_handler_info.hWnd, WM_CLIENT_MSG + RFID_NETWORK, CLIENT_SEND, 0);
			::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + RFID_NETWORK, CLIENT_SEND, 0);

			m_nRfidStep = 400;
			break;

		case 400:
			// 데이터 송신 결과 확인 작업
			if (clsRfid.m_nRfidReq == BIN_GOODBIN)
			{

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

				if (m_dwWaitTime[2] > (DWORD)5000)
				{
					m_nRfidStep = 0;

					return RET_ERROR;
				}
			}
			break;
	}
	*/
	return RET_PROCEED;
}


int	CWorkTesterNetwork::OnRfidWrite()
{
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

void CWorkTesterNetwork::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialog::OnTimer(nIDEvent);
}


void CWorkTesterNetwork::OnBnClickedBtnServerIp1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_KeyBoard dlgKeyBoard;
	CString strTmp;

	dlgKeyBoard.m_strKeyBoardVal = _T("");

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		m_strServerIp[0][1] = dlgKeyBoard.m_strKeyBoardVal;

		m_strServerIp[0][1].MakeUpper();
		m_strServerIp[0][1].TrimLeft(' ');               
		m_strServerIp[0][1].TrimRight(' ');

		OnInterfaceIpAddress(m_strServerIp[0][1]);

		m_ServerIp1.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	}
	
}


void CWorkTesterNetwork::OnBnClickedBtnServerPort1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
// 	CDialog_KeyPad dlgKeyPad;
// 	CString strTmp;
// 	CString str;

// 	dlgKeyPad.m_nKeypadMode			= 0;
// 	dlgKeyPad.m_strKeypadLowLimit	= _T("0");
// 	dlgKeyPad.m_strKeypadHighLimit	= _T("100000");
// 	dlgKeyPad.m_strKeypadVal.Format(_T("%d"),m_nServerPort[0][1]);
// 
// 	if (dlgKeyPad.DoModal() == IDOK)
// 	{
// 		strTmp = dlgKeyPad.m_strNewVal;
// 
// 		m_nServerPort[0][1] = _wtoi(strTmp);	
// 		m_editServerPort1.SetWindowText(strTmp);
// 		//m_editServerName1.GetWindowText(str);
// 
// 	}
	CString strKey;

	KeyBoard(&strKey);

	m_nServerPort[0][1] = _wtoi(strKey);
	m_editServerPort1.SetWindowTextW(strKey);

	
}


void CWorkTesterNetwork::OnBnClickedBtnServerIp2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_KeyBoard dlgKeyBoard;
	CString strTmp;

	dlgKeyBoard.m_strKeyBoardVal = _T("");

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		m_strServerIp[1][1] = dlgKeyBoard.m_strKeyBoardVal;

		m_strServerIp[1][1].MakeUpper();
		m_strServerIp[1][1].TrimLeft(' ');               
		m_strServerIp[1][1].TrimRight(' ');

		OnInterfaceIpAddress(m_strServerIp[1][1]);

		m_ServerIp2.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	}
}


void CWorkTesterNetwork::OnBnClickedBtnServerPort2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
// 	CDialog_KeyPad dlgKeyPad;
// 	CString strTmp;
// 	CString str;
// 
// 	dlgKeyPad.m_nKeypadMode			= 0;
// 	dlgKeyPad.m_strKeypadLowLimit	= _T("0");
// 	dlgKeyPad.m_strKeypadHighLimit	= _T("100000");
// 	dlgKeyPad.m_strKeypadVal.Format(_T("%d"),m_nServerPort[1][1]);
// 
// 	if (dlgKeyPad.DoModal() == IDOK)
// 	{
// 		strTmp = dlgKeyPad.m_strNewVal;
// 
// 		m_nServerPort[1][1] = _wtoi(strTmp);	
// 		m_editServerPort2.SetWindowText(strTmp);
// 		//m_editServerName1.GetWindowText(str);
// 
// 	}

	CString strKey;

	KeyBoard(&strKey);

	m_nServerPort[1][1] = _wtoi(strKey);
	m_editServerPort2.SetWindowTextW(strKey);

}


void CWorkTesterNetwork::OnBnClickedBtnServerIp3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_KeyBoard dlgKeyBoard;
	CString strTmp;

	dlgKeyBoard.m_strKeyBoardVal = _T("");

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		m_strServerIp[2][1] = dlgKeyBoard.m_strKeyBoardVal;

		m_strServerIp[2][1].MakeUpper();
		m_strServerIp[2][1].TrimLeft(' ');               
		m_strServerIp[2][1].TrimRight(' ');

		OnInterfaceIpAddress(m_strServerIp[2][1]);

		m_ServerIp3.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	}
}


void CWorkTesterNetwork::OnBnClickedBtnServerPort3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
// 	CDialog_KeyPad dlgKeyPad;
// 	CString strTmp;
// 	CString str;
// 
// 	dlgKeyPad.m_nKeypadMode			= 0;
// 	dlgKeyPad.m_strKeypadLowLimit	= _T("0");
// 	dlgKeyPad.m_strKeypadHighLimit	= _T("100000");
// 	dlgKeyPad.m_strKeypadVal.Format(_T("%d"),m_nServerPort[2][1]);
// 
// 	if (dlgKeyPad.DoModal() == IDOK)
// 	{
// 		strTmp = dlgKeyPad.m_strNewVal;
// 
// 		m_nServerPort[2][1] = _wtoi(strTmp);	
// 		m_editServerPort3.SetWindowText(strTmp);
// 		//m_editServerName1.GetWindowText(str);
// 
// 	}

	CString strKey;

	KeyBoard(&strKey);

	m_nServerPort[2][1] = _wtoi(strKey);
	m_editServerPort3.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnBnClickedBtnServerIp4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_KeyBoard dlgKeyBoard;
	CString strTmp;

	dlgKeyBoard.m_strKeyBoardVal = _T("");

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		m_strServerIp[3][1] = dlgKeyBoard.m_strKeyBoardVal;

		m_strServerIp[3][1].MakeUpper();
		m_strServerIp[3][1].TrimLeft(' ');               
		m_strServerIp[3][1].TrimRight(' ');

		OnInterfaceIpAddress(m_strServerIp[3][1]);

		m_ServerIp4.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	}
}


void CWorkTesterNetwork::OnBnClickedBtnServerPort4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
// 	CDialog_KeyPad dlgKeyPad;
// 	CString strTmp;
// 	CString str;
// 
// 	dlgKeyPad.m_nKeypadMode			= 0;
// 	dlgKeyPad.m_strKeypadLowLimit	= _T("0");
// 	dlgKeyPad.m_strKeypadHighLimit	= _T("100000");
// 	dlgKeyPad.m_strKeypadVal.Format(_T("%d"),m_nServerPort[3][1]);
// 
// 	if (dlgKeyPad.DoModal() == IDOK)
// 	{
// 		strTmp = dlgKeyPad.m_strNewVal;
// 
// 		m_nServerPort[3][1] = _wtoi(strTmp);	
// 		m_editServerPort4.SetWindowText(strTmp);
// 		//m_editServerName1.GetWindowText(str);
// 
// 	}

	CString strKey;

	KeyBoard(&strKey);

	m_nServerPort[3][1] = _wtoi(strKey);
	m_editServerPort4.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnBnClickedBtnServerIp5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_KeyBoard dlgKeyBoard;
	CString strTmp;

	dlgKeyBoard.m_strKeyBoardVal = _T("");

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		m_strServerIp[4][1] = dlgKeyBoard.m_strKeyBoardVal;

		m_strServerIp[4][1].MakeUpper();
		m_strServerIp[4][1].TrimLeft(' ');               
		m_strServerIp[4][1].TrimRight(' ');

		OnInterfaceIpAddress(m_strServerIp[4][1]);

		m_ServerIp5.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	}
}


void CWorkTesterNetwork::OnBnClickedBtnServerPort5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
// 	CDialog_KeyPad dlgKeyPad;
// 	CString strTmp;
// 	CString str;
// 
// 	dlgKeyPad.m_nKeypadMode			= 0;
// 	dlgKeyPad.m_strKeypadLowLimit	= _T("0");
// 	dlgKeyPad.m_strKeypadHighLimit	= _T("100000");
// 	dlgKeyPad.m_strKeypadVal.Format(_T("%d"),m_nServerPort[4][1]);
// 
// 	if (dlgKeyPad.DoModal() == IDOK)
// 	{
// 		strTmp = dlgKeyPad.m_strNewVal;
// 
// 		m_nServerPort[4][1] = _wtoi(strTmp);	
// 		m_editServerPort5.SetWindowText(strTmp);
// 		//m_editServerName1.GetWindowText(str);
// 	}
	CString strKey;

	KeyBoard(&strKey);

	m_nServerPort[4][1] = _wtoi(strKey);
	m_editServerPort5.SetWindowTextW(strKey);
	
}


void CWorkTesterNetwork::OnBnClickedBtnServerIp6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_KeyBoard dlgKeyBoard;
	CString strTmp;

	dlgKeyBoard.m_strKeyBoardVal = _T("");

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		m_strServerIp[5][1] = dlgKeyBoard.m_strKeyBoardVal;

		m_strServerIp[5][1].MakeUpper();
		m_strServerIp[5][1].TrimLeft(' ');               
		m_strServerIp[5][1].TrimRight(' ');

		OnInterfaceIpAddress(m_strServerIp[5][1]);

		m_ServerIp6.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	}
}


void CWorkTesterNetwork::OnBnClickedBtnServerPort6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
// 	CDialog_KeyPad dlgKeyPad;
// 	CString strTmp;
// 	CString str;
// 
// 	dlgKeyPad.m_nKeypadMode			= 0;
// 	dlgKeyPad.m_strKeypadLowLimit	= _T("0");
// 	dlgKeyPad.m_strKeypadHighLimit	= _T("100000");
// 	dlgKeyPad.m_strKeypadVal.Format(_T("%d"),m_nServerPort[5][1]);
// 
// 	if (dlgKeyPad.DoModal() == IDOK)
// 	{
// 		strTmp = dlgKeyPad.m_strNewVal;
// 
// 		m_nServerPort[5][1] = _wtoi(strTmp);	
// 		m_editServerPort6.SetWindowText(strTmp);
// 		//m_editServerName1.GetWindowText(str);
// 	}

	CString strKey;

	KeyBoard(&strKey);

	m_nServerPort[5][1] = _wtoi(strKey);
	m_editServerPort6.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnBnClickedBtnServerIp7()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_KeyBoard dlgKeyBoard;
	CString strTmp;

	dlgKeyBoard.m_strKeyBoardVal = _T("");

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		m_strServerIp[6][1] = dlgKeyBoard.m_strKeyBoardVal;

		m_strServerIp[6][1].MakeUpper();
		m_strServerIp[6][1].TrimLeft(' ');               
		m_strServerIp[6][1].TrimRight(' ');

		OnInterfaceIpAddress(m_strServerIp[6][1]);

		m_ServerIp7.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	}
}


void CWorkTesterNetwork::OnBnClickedBtnServerPort7()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
// 	CDialog_KeyPad dlgKeyPad;
// 	CString strTmp;
// 	CString str;
// 
// 	dlgKeyPad.m_nKeypadMode			= 0;
// 	dlgKeyPad.m_strKeypadLowLimit	= _T("0");
// 	dlgKeyPad.m_strKeypadHighLimit	= _T("100000");
// 	dlgKeyPad.m_strKeypadVal.Format(_T("%d"),m_nServerPort[6][1]);
// 
// 	if (dlgKeyPad.DoModal() == IDOK)
// 	{
// 		strTmp = dlgKeyPad.m_strNewVal;
// 		m_nServerPort[6][1] = _wtoi(strTmp);	
// 		m_editServerPort7.SetWindowText(strTmp);
// 		//m_editServerName1.GetWindowText(str);
// 	}

	CString strKey;

	KeyBoard(&strKey);

	m_nServerPort[6][1] = _wtoi(strKey);
	m_editServerPort7.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnBnClickedBtnServerIp8()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_KeyBoard dlgKeyBoard;
	CString strTmp;

	dlgKeyBoard.m_strKeyBoardVal = _T("");

	if (dlgKeyBoard.DoModal() == IDOK)
	{
		m_strServerIp[7][1] = dlgKeyBoard.m_strKeyBoardVal;

		m_strServerIp[7][1].MakeUpper();
		m_strServerIp[7][1].TrimLeft(' ');               
		m_strServerIp[7][1].TrimRight(' ');

		OnInterfaceIpAddress(m_strServerIp[7][1]);

		m_ServerIp8.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	}
}


void CWorkTesterNetwork::OnBnClickedBtnServerPort8()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
// 	CDialog_KeyPad dlgKeyPad;
// 	CString strTmp;
// 	CString str;
// 
// 	dlgKeyPad.m_nKeypadMode			= 0;
// 	dlgKeyPad.m_strKeypadLowLimit	= _T("0");
// 	dlgKeyPad.m_strKeypadHighLimit	= _T("100000");
// 	dlgKeyPad.m_strKeypadVal.Format(_T("%d"),m_nServerPort[7][1]);
// 
// 	if (dlgKeyPad.DoModal() == IDOK)
// 	{
// 		strTmp = dlgKeyPad.m_strNewVal;
// 
// 		m_nServerPort[7][1] = _wtoi(strTmp);	
// 		m_editServerPort8.SetWindowText(strTmp);
// 		//m_editServerName1.GetWindowText(str);
// 	}
	CString strKey;

	KeyBoard(&strKey);

	m_nServerPort[7][1] = _wtoi(strKey);
	m_editServerPort8.SetWindowTextW(strKey);

}


void CWorkTesterNetwork::OnBnClickedBtnClientIp1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
// 	CDialog_KeyBoard dlgKeyBoard;
// 	CString strTmp;
// 
// 	dlgKeyBoard.m_strKeyBoardVal = _T("");
// 
// 	if (dlgKeyBoard.DoModal() == IDOK)
// 	{
// 		m_strClientIp[0][1] = dlgKeyBoard.m_strKeyBoardVal;
// 
// 		m_strClientIp[0][1].MakeUpper();
// 		m_strClientIp[0][1].TrimLeft(' ');               
// 		m_strClientIp[0][1].TrimRight(' ');
// 
// 		OnInterfaceIpAddress(m_strClientIp[0][1]);
// 
// 		m_clientIp1.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
// 	}

	CString strKey;

	KeyBoard(&strKey);

	m_strClientIp[INTERFACE_NETWORK_1][1] = strKey;

	m_strClientIp[INTERFACE_NETWORK_1][1].MakeUpper();
	m_strClientIp[INTERFACE_NETWORK_1][1].TrimLeft(' ');               
	m_strClientIp[INTERFACE_NETWORK_1][1].TrimRight(' ');

	OnInterfaceIpAddress(m_strClientIp[INTERFACE_NETWORK_1][1]);

	m_clientIp1.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
}


void CWorkTesterNetwork::OnBnClickedBtnClientPort1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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


void CWorkTesterNetwork::OnBnClickedBtnClientIp2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
// 	CDialog_KeyBoard dlgKeyBoard;
// 	CString strTmp;
// 
// 	dlgKeyBoard.m_strKeyBoardVal = _T("");
// 
// 	if (dlgKeyBoard.DoModal() == IDOK)
// 	{
// 		m_strClientIp[1][1] = dlgKeyBoard.m_strKeyBoardVal;
// 
// 		m_strClientIp[1][1].MakeUpper();
// 		m_strClientIp[1][1].TrimLeft(' ');               
// 		m_strClientIp[1][1].TrimRight(' ');
// 
// 		OnInterfaceIpAddress(m_strClientIp[1][1]);
// 
// 		m_clientIp2.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
// 	}

	CString strKey;

	KeyBoard(&strKey);

	m_strClientIp[INTERFACE_NETWORK_2][1] = strKey;

	m_strClientIp[INTERFACE_NETWORK_2][1].MakeUpper();
	m_strClientIp[INTERFACE_NETWORK_2][1].TrimLeft(' ');               
	m_strClientIp[INTERFACE_NETWORK_2][1].TrimRight(' ');

	OnInterfaceIpAddress(m_strClientIp[INTERFACE_NETWORK_2][1]);

	m_clientIp2.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
}


void CWorkTesterNetwork::OnBnClickedBtnClientPort2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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


void CWorkTesterNetwork::OnBnClickedBtnClientIp3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
// 	CDialog_KeyBoard dlgKeyBoard;
// 	CString strTmp;
// 
// 	dlgKeyBoard.m_strKeyBoardVal = _T("");
// 
// 	if (dlgKeyBoard.DoModal() == IDOK)
// 	{
// 		m_strClientIp[2][1] = dlgKeyBoard.m_strKeyBoardVal;
// 
// 		m_strClientIp[2][1].MakeUpper();
// 		m_strClientIp[2][1].TrimLeft(' ');               
// 		m_strClientIp[2][1].TrimRight(' ');
// 
// 		OnInterfaceIpAddress(m_strClientIp[2][1]);
// 
// 		m_clientIp3.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
// 	}

	CString strKey;

	KeyBoard(&strKey);

	m_strClientIp[INTERFACE_NETWORK_3][1] = strKey;

	m_strClientIp[INTERFACE_NETWORK_3][1].MakeUpper();
	m_strClientIp[INTERFACE_NETWORK_3][1].TrimLeft(' ');               
	m_strClientIp[INTERFACE_NETWORK_3][1].TrimRight(' ');

	OnInterfaceIpAddress(m_strClientIp[INTERFACE_NETWORK_3][1]);

	m_clientIp3.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
}


void CWorkTesterNetwork::OnBnClickedBtnClientPort3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_KeyPad dlgKeyPad;
	CString strTmp;

	dlgKeyPad.m_nKeypadMode			= 0;
	dlgKeyPad.m_strKeypadLowLimit	= _T("0");
	dlgKeyPad.m_strKeypadHighLimit	= _T("100000");
	dlgKeyPad.m_strKeypadVal.Format(_T("%d"),m_nClientPort[2][1]);

	if (dlgKeyPad.DoModal() == IDOK)
	{
		strTmp = dlgKeyPad.m_strNewVal;
		m_nClientPort[2][1] = _wtoi(strTmp);	
		m_editClientPort3.SetWindowText(strTmp);
	}
}


void CWorkTesterNetwork::OnBnClickedBtnClientIp4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
// 	CDialog_KeyBoard dlgKeyBoard;
// 	CString strTmp;
// 
// 	dlgKeyBoard.m_strKeyBoardVal = _T("");
// 
// 	if (dlgKeyBoard.DoModal() == IDOK)
// 	{
// 		m_strClientIp[3][1] = dlgKeyBoard.m_strKeyBoardVal;
// 
// 		m_strClientIp[3][1].MakeUpper();
// 		m_strClientIp[3][1].TrimLeft(' ');               
// 		m_strClientIp[3][1].TrimRight(' ');
// 
// 		OnInterfaceIpAddress(m_strClientIp[3][1]);
// 
// 		m_clientIp4.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
	//}
	CString strKey;

	KeyBoard(&strKey);

	m_strClientIp[INTERFACE_NETWORK_4][1] = strKey;

	m_strClientIp[INTERFACE_NETWORK_4][1].MakeUpper();
	m_strClientIp[INTERFACE_NETWORK_4][1].TrimLeft(' ');               
	m_strClientIp[INTERFACE_NETWORK_4][1].TrimRight(' ');

	OnInterfaceIpAddress(m_strClientIp[INTERFACE_NETWORK_4][1]);

	m_clientIp4.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
}


void CWorkTesterNetwork::OnBnClickedBtnClientPort4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_KeyPad dlgKeyPad;
	CString strTmp;

	dlgKeyPad.m_nKeypadMode			= 0;
	dlgKeyPad.m_strKeypadLowLimit	= _T("0");
	dlgKeyPad.m_strKeypadHighLimit	= _T("100000");
	dlgKeyPad.m_strKeypadVal.Format(_T("%d"),m_nClientPort[3][1]);

	if (dlgKeyPad.DoModal() == IDOK)
	{
		strTmp = dlgKeyPad.m_strNewVal;
		m_nClientPort[3][1] = _wtoi(strTmp);	
		m_editClientPort4.SetWindowText(strTmp);
	}
}


void CWorkTesterNetwork::OnBnClickedBtnClientIp5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
// 	CDialog_KeyBoard dlgKeyBoard;
// 	CString strTmp;
// 
// 	dlgKeyBoard.m_strKeyBoardVal = _T("");
// 
// 	if (dlgKeyBoard.DoModal() == IDOK)
// 	{
// 		m_strClientIp[4][1] = dlgKeyBoard.m_strKeyBoardVal;
// 
// 		m_strClientIp[4][1].MakeUpper();
// 		m_strClientIp[4][1].TrimLeft(' ');               
// 		m_strClientIp[4][1].TrimRight(' ');
// 
// 		OnInterfaceIpAddress(m_strClientIp[4][1]);
// 
// 		m_clientIp5.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
// 	}

	CString strKey;

	KeyBoard(&strKey);

	m_strClientIp[INTERFACE_NETWORK_5][1] = strKey;

	m_strClientIp[INTERFACE_NETWORK_5][1].MakeUpper();
	m_strClientIp[INTERFACE_NETWORK_5][1].TrimLeft(' ');               
	m_strClientIp[INTERFACE_NETWORK_5][1].TrimRight(' ');

	OnInterfaceIpAddress(m_strClientIp[INTERFACE_NETWORK_5][1]);

	m_clientIp5.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
}


void CWorkTesterNetwork::OnBnClickedBtnClientPort5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_KeyPad dlgKeyPad;
	CString strTmp;

	dlgKeyPad.m_nKeypadMode			= 0;
	dlgKeyPad.m_strKeypadLowLimit	= _T("0");
	dlgKeyPad.m_strKeypadHighLimit	= _T("100000");
	dlgKeyPad.m_strKeypadVal.Format(_T("%d"),m_nClientPort[4][1]);

	if (dlgKeyPad.DoModal() == IDOK)
	{
		strTmp = dlgKeyPad.m_strNewVal;
		m_nClientPort[4][1] = _wtoi(strTmp);	
		m_editClientPort5.SetWindowText(strTmp);
	}
}


void CWorkTesterNetwork::OnBnClickedBtnClientIp6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
// 	CDialog_KeyBoard dlgKeyBoard;
// 	CString strTmp;
// 
// 	dlgKeyBoard.m_strKeyBoardVal = _T("");
// 
// 	if (dlgKeyBoard.DoModal() == IDOK)
// 	{
// 		m_strClientIp[5][1] = dlgKeyBoard.m_strKeyBoardVal;
// 
// 		m_strClientIp[5][1].MakeUpper();
// 		m_strClientIp[5][1].TrimLeft(' ');               
// 		m_strClientIp[5][1].TrimRight(' ');
// 
// 		OnInterfaceIpAddress(m_strClientIp[5][1]);
// 
// 		m_clientIp6.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
// 	}

	CString strKey;

	KeyBoard(&strKey);

	m_strClientIp[INTERFACE_NETWORK_6][1] = strKey;

	m_strClientIp[INTERFACE_NETWORK_6][1].MakeUpper();
	m_strClientIp[INTERFACE_NETWORK_6][1].TrimLeft(' ');               
	m_strClientIp[INTERFACE_NETWORK_6][1].TrimRight(' ');

	OnInterfaceIpAddress(m_strClientIp[INTERFACE_NETWORK_6][1]);

	m_clientIp6.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
}


void CWorkTesterNetwork::OnBnClickedBtnClientPort6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_KeyPad dlgKeyPad;
	CString strTmp;

	dlgKeyPad.m_nKeypadMode			= 0;
	dlgKeyPad.m_strKeypadLowLimit	= _T("0");
	dlgKeyPad.m_strKeypadHighLimit	= _T("100000");
	dlgKeyPad.m_strKeypadVal.Format(_T("%d"),m_nClientPort[5][1]);

	if (dlgKeyPad.DoModal() == IDOK)
	{
		strTmp = dlgKeyPad.m_strNewVal;
		m_nClientPort[5][1] = _wtoi(strTmp);	
		m_editClientPort6.SetWindowText(strTmp);
	}
}


void CWorkTesterNetwork::OnBnClickedBtnClientIp7()
{
// 	CDialog_KeyBoard dlgKeyBoard;
// 	CString strTmp;
// 
// 	dlgKeyBoard.m_strKeyBoardVal = _T("");
// 
// 	if (dlgKeyBoard.DoModal() == IDOK)
// 	{
// 		m_strClientIp[6][1] = dlgKeyBoard.m_strKeyBoardVal;
// 
// 		m_strClientIp[6][1].MakeUpper();
// 		m_strClientIp[6][1].TrimLeft(' ');               
// 		m_strClientIp[6][1].TrimRight(' ');
// 
// 		OnInterfaceIpAddress(m_strClientIp[6][1]);
// 
// 		m_clientIp7.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
// 	}
	
	CString strKey;

	KeyBoard(&strKey);

	m_strClientIp[INTERFACE_NETWORK_7][1] = strKey;

	m_strClientIp[INTERFACE_NETWORK_7][1].MakeUpper();
	m_strClientIp[INTERFACE_NETWORK_7][1].TrimLeft(' ');               
	m_strClientIp[INTERFACE_NETWORK_7][1].TrimRight(' ');

	OnInterfaceIpAddress(m_strClientIp[INTERFACE_NETWORK_7][1]);

	m_clientIp7.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
}


void CWorkTesterNetwork::OnBnClickedBtnClientPort7()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
		m_editClientPort7.SetWindowText(strTmp);
	}
}


void CWorkTesterNetwork::OnBnClickedBtnClientIp8()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
// 	CDialog_KeyBoard dlgKeyBoard;
// 	CString strTmp;
// 
// 	dlgKeyBoard.m_strKeyBoardVal = _T("");
// 
// 	if (dlgKeyBoard.DoModal() == IDOK)
// 	{
// 		m_strClientIp[7][1] = dlgKeyBoard.m_strKeyBoardVal;
// 
// 		m_strClientIp[7][1].MakeUpper();
// 		m_strClientIp[7][1].TrimLeft(' ');               
// 		m_strClientIp[7][1].TrimRight(' ');
// 
// 		OnInterfaceIpAddress(m_strClientIp[7][1]);
// 
// 		m_clientIp8.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
// 	}

	CString strKey;

	KeyBoard(&strKey);

	m_strClientIp[INTERFACE_NETWORK_8][1] = strKey;

	m_strClientIp[INTERFACE_NETWORK_8][1].MakeUpper();
	m_strClientIp[INTERFACE_NETWORK_8][1].TrimLeft(' ');               
	m_strClientIp[INTERFACE_NETWORK_8][1].TrimRight(' ');

	OnInterfaceIpAddress(m_strClientIp[INTERFACE_NETWORK_8][1]);

	m_clientIp8.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
}


void CWorkTesterNetwork::OnBnClickedBtnClientPort8()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
		m_editClientPort8.SetWindowText(strTmp);
	}
}


void CWorkTesterNetwork::OnEnChangeEditServerPort4()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CWorkTesterNetwork::OnEnChangeEditServerName1()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CWorkTesterNetwork::OnStnClickedMsgServerPortData1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strKey;
	
	
	KeyBoard(&strKey);
	
	m_nServerPort[INTERFACE_NETWORK_1][1] = _wtoi(strKey);
	m_msgServerPortData_1.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgServerPortData2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	

	CString strKey;
	

	KeyBoard(&strKey);

	m_nServerPort[INTERFACE_NETWORK_2][1] = _wtoi(strKey);
	m_msgServerPortData_2.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgServerPortData3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strKey;
	

	KeyBoard(&strKey);
	
	m_nServerPort[INTERFACE_NETWORK_3][1] = _wtoi(strKey);
	m_msgServerPortData_3.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgServerPortData4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strKey;
	

	KeyBoard(&strKey);
	
	m_nServerPort[INTERFACE_NETWORK_4][1] = _wtoi(strKey);
	m_msgServerPortData_4.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgServerPortData5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strKey;
	

	KeyBoard(&strKey);
	
	m_nServerPort[INTERFACE_NETWORK_5][1] = _wtoi(strKey);
	m_msgServerPortData_5.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgServerPortData6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strKey;
	

	KeyBoard(&strKey);
	
	m_nServerPort[INTERFACE_NETWORK_6][1] = _wtoi(strKey);
	m_msgServerPortData_6.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgServerPortData7()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strKey;
	

	KeyBoard(&strKey);

	m_nServerPort[INTERFACE_NETWORK_7][1] = _wtoi(strKey);
	m_msgServerPortData_7.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgServerPortData8()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strKey;
	

	KeyBoard(&strKey);

	m_nServerPort[INTERFACE_NETWORK_8][1] = _wtoi(strKey);
	m_msgServerPortData_8.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgServerId1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strKey,strTemp;

	KeyBoard(&strKey);

	strTemp = m_msgServerID_1.GetWindowText();

	if (strTemp == strKey)
	{

	}
	else
	{
		for(int i =0; i<10; i++)
		{
			if (i != INTERFACE_NETWORK_1)
			{
				if (strKey == m_strServerName[i][1])
				{
					strTemp.Format(_T("Test[%d] Server Name Same Check"), i + 1);
					AfxMessageBox(strTemp);
					return;
				}	
			}
			if (strKey == m_strClientName[i][1])
			{
				strTemp.Format(_T("Test [%d] Client Name Same Check"), i + 1);
				AfxMessageBox(strTemp);

				return;
			}
		}
	}
	m_strServerName[INTERFACE_NETWORK_1][1] = strKey;
	m_msgServerID_1.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgServerId2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strKey,strTemp;

	KeyBoard(&strKey);

	strTemp = m_msgServerID_2.GetWindowText();

	if (strTemp == strKey)
	{

	}
	else
	{
		for(int i =0; i<10; i++)
		{
			if (i != INTERFACE_NETWORK_2)
			{
				if (strKey == m_strServerName[i][1])
				{
					strTemp.Format(_T("Test[%d] Server Name Same Check"), i + 1);
					AfxMessageBox(strTemp);
					return;
				}	
			}
			if (strKey == m_strClientName[i][1])
			{
				strTemp.Format(_T("Test [%d] Client Name Same Check"), i + 1);
				AfxMessageBox(strTemp);

				return;
			}
		}
	}
	m_strServerName[INTERFACE_NETWORK_2][1] = strKey;
	m_msgServerID_2.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgServerId3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString strKey,strTemp;

	KeyBoard(&strKey);

	strTemp = m_msgServerID_3.GetWindowText();

	if (strTemp == strKey)
	{

	}
	else
	{
		for(int i =0; i<10; i++)
		{
			if (i != INTERFACE_NETWORK_3)
			{
				if (strKey == m_strServerName[i][1])
				{
					strTemp.Format(_T("Test[%d] Server Name Same Check"), i + 1);
					AfxMessageBox(strTemp);
					return;
				}	
			}
			if (strKey == m_strClientName[i][1])
			{
				strTemp.Format(_T("Test [%d] Client Name Same Check"), i + 1);
				AfxMessageBox(strTemp);

				return;
			}
		}
	}

	m_strServerName[INTERFACE_NETWORK_3][1] = strKey;
	m_msgServerID_3.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgServerId4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strKey,strTemp;

	KeyBoard(&strKey);

	strTemp = m_msgServerID_4.GetWindowText();

	if (strTemp == strKey)
	{

	}
	else
	{
		for(int i =0; i<10; i++)
		{
			if (i != INTERFACE_NETWORK_4)
			{
				if (strKey == m_strServerName[i][1])
				{
					strTemp.Format(_T("Test[%d] Server Name Same Check"), i + 1);
					AfxMessageBox(strTemp);
					return;
				}	
			}
			if (strKey == m_strClientName[i][1])
			{
				strTemp.Format(_T("Test [%d] Client Name Same Check"), i + 1);
				AfxMessageBox(strTemp);

				return;
			}
		}
	}

	m_strServerName[INTERFACE_NETWORK_4][1] = strKey;
	m_msgServerID_4.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgServerId5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strKey,strTemp;

	KeyBoard(&strKey);

	strTemp = m_msgServerID_5.GetWindowText();

	if (strTemp == strKey)
	{

	}
	else
	{
		for(int i =0; i<10; i++)
		{
			if (i != INTERFACE_NETWORK_5)
			{
				if (strKey == m_strServerName[i][1])
				{
					strTemp.Format(_T("Test[%d] Server Name Same Check"), i + 1);
					AfxMessageBox(strTemp);
					return;
				}	
			}
			if (strKey == m_strClientName[i][1])
			{
				strTemp.Format(_T("Test [%d] Client Name Same Check"), i + 1);
				AfxMessageBox(strTemp);

				return;
			}
		}
	}

	m_strServerName[INTERFACE_NETWORK_5][1] = strKey;
	m_msgServerID_5.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgServerId6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strKey,strTemp;

	KeyBoard(&strKey);

	strTemp = m_msgServerID_6.GetWindowText();

	if (strTemp == strKey)
	{

	}
	else
	{
		for(int i =0; i<10; i++)
		{
			if (i != INTERFACE_NETWORK_6)
			{
				if (strKey == m_strServerName[i][1])
				{
					strTemp.Format(_T("Test[%d] Server Name Same Check"), i + 1);
					AfxMessageBox(strTemp);
					return;
				}	
			}
			if (strKey == m_strClientName[i][1])
			{
				strTemp.Format(_T("Test [%d] Client Name Same Check"), i + 1);
				AfxMessageBox(strTemp);

				return;
			}
		}
	}

	m_strServerName[INTERFACE_NETWORK_6][1] = strKey;
	m_msgServerID_6.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgServerId7()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strKey,strTemp;

	KeyBoard(&strKey);

	strTemp = m_msgServerID_7.GetWindowText();

	if (strTemp == strKey)
	{

	}
	else
	{
		for(int i =0; i<10; i++)
		{
			if (i != INTERFACE_NETWORK_7)
			{
				if (strKey == m_strServerName[i][1])
				{
					strTemp.Format(_T("Test[%d] Server Name Same Check"), i + 1);
					AfxMessageBox(strTemp);
					return;
				}	
			}
			if (strKey == m_strClientName[i][1])
			{
				strTemp.Format(_T("Test [%d] Client Name Same Check"), i + 1);
				AfxMessageBox(strTemp);

				return;
			}
		}
	}

	m_strServerName[INTERFACE_NETWORK_7][1] = strKey;
	m_msgServerID_7.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgServerId8()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString strKey,strTemp;

	KeyBoard(&strKey);

	strTemp = m_msgServerID_8.GetWindowText();

	if (strTemp == strKey)
	{

	}
	else
	{
		for(int i =0; i<10; i++)
		{
			if (i != INTERFACE_NETWORK_8)
			{
				if (strKey == m_strServerName[i][1])
				{
					strTemp.Format(_T("Test[%d] Server Name Same Check"), i + 1);
					AfxMessageBox(strTemp);
					return;
				}	
			}
			if (strKey == m_strClientName[i][1])
			{
				strTemp.Format(_T("Test [%d] Client Name Same Check"), i + 1);
				AfxMessageBox(strTemp);

				return;
			}
		}
	}

	m_strServerName[INTERFACE_NETWORK_8][1] = strKey;
	m_msgServerID_8.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgClientPortData1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strKey;
	

	KeyBoard(&strKey);
	
	m_nClientPort[INTERFACE_NETWORK_1][1] = _wtoi(strKey);
	m_msgClientPortData_1.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgClientId1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString strKey,strTemp;

	KeyBoard(&strKey);

	strTemp = m_msgClientID_1.GetWindowText();

	if (strTemp == strKey)
	{

	}
	else
	{
		for(int i =0; i<10; i++)
		{
			if (i != INTERFACE_NETWORK_1)
			{
				if (strKey == m_strClientName[i][1])
				{
					strTemp.Format(_T("Test [%d] Client Name Same Check"), i + 1);
					AfxMessageBox(strTemp);

					return;
				}
			}

// 			if (strKey == m_strServerName[i][1])
// 			{
// 				strTemp.Format(_T("Test[%d] Server Name Same Check"), i + 1);
// 				AfxMessageBox(strTemp);
// 				return;
// 			}
		}
	}

	m_strClientName[INTERFACE_NETWORK_1][1] = strKey;
	m_msgClientID_1.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgClientPortData2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strKey;
	

	KeyBoard(&strKey);

	m_nClientPort[INTERFACE_NETWORK_2][1] = _wtoi(strKey);
	m_msgClientPortData_2.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgClientPortData3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strKey;
	

	KeyBoard(&strKey);
	
	m_nClientPort[INTERFACE_NETWORK_3][1] = _wtoi(strKey);
	m_msgClientPortData_3.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgClientPortData4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strKey;
	

	KeyBoard(&strKey);
	
	m_nClientPort[INTERFACE_NETWORK_4][1] = _wtoi(strKey);
	m_msgClientPortData_4.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgClientPortData5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strKey;
	

	KeyBoard(&strKey);
	
	m_nClientPort[INTERFACE_NETWORK_5][1] = _wtoi(strKey);
	m_msgClientPortData_5.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgClientPortData6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strKey;
	

	KeyBoard(&strKey);
	
	m_nClientPort[INTERFACE_NETWORK_6][1] = _wtoi(strKey);
	m_msgClientPortData_6.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgClientPortData7()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strKey;
	

	KeyBoard(&strKey);

	m_nClientPort[INTERFACE_NETWORK_7][1] = _wtoi(strKey);
	m_msgClientPortData_7.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgClientPortData8()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strKey;
	

	KeyBoard(&strKey);

	m_nClientPort[INTERFACE_NETWORK_8][1] = _wtoi(strKey);
	m_msgClientPortData_8.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgClientId2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strKey,strTemp;

	KeyBoard(&strKey);

	strTemp = m_msgClientID_2.GetWindowText();

	if (strTemp == strKey)
	{

	}
	else
	{
		for(int i =0; i<10; i++)
		{
			if (i != INTERFACE_NETWORK_2)
			{
				if (strKey == m_strClientName[i][1])
				{
					strTemp.Format(_T("Test [%d] Client Name Same Check"), i + 1);
					AfxMessageBox(strTemp);

					return;
				}
			}

// 			if (strKey == m_strServerName[i][1])
// 			{
// 				strTemp.Format(_T("Test[%d] Server Name Same Check"), i + 1);
// 				AfxMessageBox(strTemp);
// 				return;
// 			}
		}
	}

	m_strClientName[INTERFACE_NETWORK_2][1] = strKey;
	m_msgClientID_2.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgClientId3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strKey,strTemp;

	KeyBoard(&strKey);

	strTemp = m_msgClientID_3.GetWindowText();

	if (strTemp == strKey)
	{

	}
	else
	{
		for(int i =0; i<10; i++)
		{
			if (i != INTERFACE_NETWORK_3)
			{
				if (strKey == m_strClientName[i][1])
				{
					strTemp.Format(_T("Test [%d] Client Name Same Check"), i + 1);
					AfxMessageBox(strTemp);

					return;
				}
			}

// 			if (strKey == m_strServerName[i][1])
// 			{
// 				strTemp.Format(_T("Test[%d] Server Name Same Check"), i + 1);
// 				AfxMessageBox(strTemp);
// 				return;
// 			}
		}
	}

	m_strClientName[INTERFACE_NETWORK_3][1] = strKey;
	m_msgClientID_3.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgClientId4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strKey,strTemp;

	KeyBoard(&strKey);

	strTemp = m_msgClientID_4.GetWindowText();

	if (strTemp == strKey)
	{

	}
	else
	{
		for(int i =0; i<10; i++)
		{
			if (i != INTERFACE_NETWORK_4)
			{
				if (strKey == m_strClientName[i][1])
				{
					strTemp.Format(_T("Test [%d] Client Name Same Check"), i + 1);
					AfxMessageBox(strTemp);

					return;
				}
			}

// 			if (strKey == m_strServerName[i][1])
// 			{
// 				strTemp.Format(_T("Test[%d] Server Name Same Check"), i + 1);
// 				AfxMessageBox(strTemp);
// 				return;
// 			}
		}
	}

	m_strClientName[INTERFACE_NETWORK_4][1] = strKey;
	m_msgClientID_4.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgClientId5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strKey,strTemp;

	KeyBoard(&strKey);

	strTemp = m_msgClientID_5.GetWindowText();

	if (strTemp == strKey)
	{

	}
	else
	{
		for(int i =0; i<10; i++)
		{
			if (i != INTERFACE_NETWORK_5)
			{
				if (strKey == m_strClientName[i][1])
				{
					strTemp.Format(_T("Test [%d] Client Name Same Check"), i + 1);
					AfxMessageBox(strTemp);

					return;
				}
			}

// 			if (strKey == m_strServerName[i][1])
// 			{
// 				strTemp.Format(_T("Test[%d] Server Name Same Check"), i + 1);
// 				AfxMessageBox(strTemp);
// 				return;
// 			}
		}
	}

	m_strClientName[INTERFACE_NETWORK_5][1] = strKey;
	m_msgClientID_5.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgClientId6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strKey,strTemp;

	KeyBoard(&strKey);

	strTemp = m_msgClientID_6.GetWindowText();

	if (strTemp == strKey)
	{

	}
	else
	{
		for(int i =0; i<10; i++)
		{
			if (i != INTERFACE_NETWORK_6)
			{
				if (strKey == m_strClientName[i][1])
				{
					strTemp.Format(_T("Test [%d] Client Name Same Check"), i + 1);
					AfxMessageBox(strTemp);

					return;
				}
			}

// 			if (strKey == m_strServerName[i][1])
// 			{
// 				strTemp.Format(_T("Test[%d] Server Name Same Check"), i + 1);
// 				AfxMessageBox(strTemp);
// 				return;
// 			}
		}
	}

	m_strClientName[INTERFACE_NETWORK_6][1] = strKey;
	m_msgClientID_6.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgClientId7()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strKey,strTemp;

	KeyBoard(&strKey);

	strTemp = m_msgClientID_7.GetWindowText();

	if (strTemp == strKey)
	{

	}
	else
	{
		for(int i =0; i<10; i++)
		{
			if (i != INTERFACE_NETWORK_7)
			{
				if (strKey == m_strClientName[i][1])
				{
					strTemp.Format(_T("Test [%d] Client Name Same Check"), i + 1);
					AfxMessageBox(strTemp);

					return;
				}
			}

// 			if (strKey == m_strServerName[i][1])
// 			{
// 				strTemp.Format(_T("Test[%d] Server Name Same Check"), i + 1);
// 				AfxMessageBox(strTemp);
// 				return;
// 			}
		}
	}

	m_strClientName[INTERFACE_NETWORK_7][1] = strKey;
	m_msgClientID_7.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgClientId8()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString strKey,strTemp;

	KeyBoard(&strKey);

	strTemp = m_msgClientID_8.GetWindowText();

	if (strTemp == strKey)
	{

	}
	else
	{
		for(int i =0; i<10; i++)
		{
			if (i != INTERFACE_NETWORK_8)
			{
				if (strKey == m_strClientName[i][1])
				{
					strTemp.Format(_T("Test [%d] Client Name Same Check"), i + 1);
					AfxMessageBox(strTemp);

					return;
				}
			}

// 			if (strKey == m_strServerName[i][1])
// 			{
// 				strTemp.Format(_T("Test[%d] Server Name Same Check"), i + 1);
// 				AfxMessageBox(strTemp);
// 				return;
// 			}
		}
	}

	m_strClientName[INTERFACE_NETWORK_8][1] = strKey;
	m_msgClientID_8.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgEcserverPortData()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strKey;
	
	KeyBoard(&strKey);

	m_nClientPort[EC_FIRST_NETWORK][1] = _wtoi(strKey);
	m_msgEcServerPortData.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgXgemPortData()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strKey;
	

	KeyBoard(&strKey);

	m_nClientPort[XGEM_NETWORK][1] = _wtoi(strKey);
	m_msgXgemPortData.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnBnClickedBtnEcserverIp()
{
	CString strKey;

	KeyBoard(&strKey);

	m_strClientIp[EC_FIRST_NETWORK][1] = strKey;

	m_strClientIp[EC_FIRST_NETWORK][1].MakeUpper();
	m_strClientIp[EC_FIRST_NETWORK][1].TrimLeft(' ');               
	m_strClientIp[EC_FIRST_NETWORK][1].TrimRight(' ');

	OnInterfaceIpAddress(m_strClientIp[EC_FIRST_NETWORK][1]);

	m_EcServerIp.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
}


void CWorkTesterNetwork::OnBnClickedBtnXgemIp()
{
	CString strKey;

	KeyBoard(&strKey);

	m_strClientIp[XGEM_NETWORK][1] = strKey;

	m_strClientIp[XGEM_NETWORK][1].MakeUpper();
	m_strClientIp[XGEM_NETWORK][1].TrimLeft(' ');               
	m_strClientIp[XGEM_NETWORK][1].TrimRight(' ');

	OnInterfaceIpAddress(m_strClientIp[XGEM_NETWORK][1]);

	m_XgemIp.SetAddress(m_byIp[0], m_byIp[1], m_byIp[2], m_byIp[3]);
}


void CWorkTesterNetwork::OnStnClickedMsgSerialId11()
{
	CString strKey,strTemp;

	KeyBoard(&strKey);

	strTemp = m_msgSerialID_1_1.GetWindowText();

	if (strTemp == strKey)
	{

	}
	else
	{
		for(int i =0; i<16; i++)
		{
			if (i != TSITE_SERIAL_1_1)
			{
				if (strKey == m_strTsiteSerial[i][1])
				{
					strTemp.Format(_T("%d Site %d Serial Name이 같습니다."), (i/2) + 1,(i%2) + 1);
					AfxMessageBox(strTemp);

					return;
				}
			}
		}
	}

	m_strTsiteSerial[TSITE_SERIAL_1_1][1] = strKey;
	m_msgSerialID_1_1.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgSerialId12()
{
	CString strKey,strTemp;

	KeyBoard(&strKey);

	strTemp = m_msgSerialID_1_2.GetWindowText();

	if (strTemp == strKey)
	{

	}
	else
	{
		for(int i =0; i<16; i++)
		{
			if (i != TSITE_SERIAL_1_2)
			{
				if (strKey == m_strTsiteSerial[i][1])
				{
					strTemp.Format(_T("%d Site %d Serial Name이 같습니다."), (i/2) + 1,(i%2) + 1);
					AfxMessageBox(strTemp);

					return;
				}
			}
		}
	}

	m_strTsiteSerial[TSITE_SERIAL_1_2][1] = strKey;
	m_msgSerialID_1_2.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgSerialId21()
{
	CString strKey,strTemp;

	KeyBoard(&strKey);

	strTemp = m_msgSerialID_2_1.GetWindowText();

	if (strTemp == strKey)
	{

	}
	else
	{
		for(int i =0; i<16; i++)
		{
			if (i != TSITE_SERIAL_2_1)
			{
				if (strKey == m_strTsiteSerial[i][1])
				{
					strTemp.Format(_T("%d Site %d Serial Name이 같습니다."), (i/2) + 1,(i%2) + 1);
					AfxMessageBox(strTemp);

					return;
				}
			}
		}
	}

	m_strTsiteSerial[TSITE_SERIAL_2_1][1] = strKey;
	m_msgSerialID_2_1.SetWindowTextW(strKey);
}

void CWorkTesterNetwork::OnStnClickedMsgSerialId22()
{
	CString strKey,strTemp;

	KeyBoard(&strKey);

	strTemp = m_msgSerialID_2_2.GetWindowText();

	if (strTemp == strKey)
	{

	}
	else
	{
		for(int i =0; i<16; i++)
		{
			if (i != TSITE_SERIAL_2_2)
			{
				if (strKey == m_strTsiteSerial[i][1])
				{
					strTemp.Format(_T("%d Site %d Serial Name이 같습니다."), (i/2) + 1,(i%2) + 1);
					AfxMessageBox(strTemp);

					return;
				}
			}
		}
	}

	m_strTsiteSerial[TSITE_SERIAL_2_2][1] = strKey;
	m_msgSerialID_2_2.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgSerialId31()
{
	CString strKey,strTemp;

	KeyBoard(&strKey);

	strTemp = m_msgSerialID_3_1.GetWindowText();

	if (strTemp == strKey)
	{

	}
	else
	{
		for(int i =0; i<16; i++)
		{
			if (i != TSITE_SERIAL_3_1)
			{
				if (strKey == m_strTsiteSerial[i][1])
				{
					strTemp.Format(_T("%d Site %d Serial Name이 같습니다."), (i/2) + 1,(i%2) + 1);
					AfxMessageBox(strTemp);

					return;
				}
			}
		}
	}

	m_strTsiteSerial[TSITE_SERIAL_3_1][1] = strKey;
	m_msgSerialID_3_1.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgSerialId32()
{
	CString strKey,strTemp;

	KeyBoard(&strKey);

	strTemp = m_msgSerialID_3_2.GetWindowText();

	if (strTemp == strKey)
	{

	}
	else
	{
		for(int i =0; i<16; i++)
		{
			if (i != TSITE_SERIAL_3_2)
			{
				if (strKey == m_strTsiteSerial[i][1])
				{
					strTemp.Format(_T("%d Site %d Serial Name이 같습니다."), (i/2) + 1,(i%2) + 1);
					AfxMessageBox(strTemp);

					return;
				}
			}
		}
	}

	m_strTsiteSerial[TSITE_SERIAL_3_2][1] = strKey;
	m_msgSerialID_3_2.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgSerialId41()
{
	CString strKey,strTemp;

	KeyBoard(&strKey);

	strTemp = m_msgSerialID_4_1.GetWindowText();

	if (strTemp == strKey)
	{

	}
	else
	{
		for(int i =0; i<16; i++)
		{
			if (i != TSITE_SERIAL_4_1)
			{
				if (strKey == m_strTsiteSerial[i][1])
				{
					strTemp.Format(_T("%d Site %d Serial Name이 같습니다."), (i/2) + 1,(i%2) + 1);
					AfxMessageBox(strTemp);

					return;
				}
			}
		}
	}

	m_strTsiteSerial[TSITE_SERIAL_4_1][1] = strKey;
	m_msgSerialID_4_1.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgSerialId42()
{
	CString strKey,strTemp;

	KeyBoard(&strKey);

	strTemp = m_msgSerialID_4_2.GetWindowText();

	if (strTemp == strKey)
	{

	}
	else
	{
		for(int i =0; i<16; i++)
		{
			if (i != TSITE_SERIAL_4_2)
			{
				if (strKey == m_strTsiteSerial[i][1])
				{
					strTemp.Format(_T("%d Site %d Serial Name이 같습니다."), (i/2) + 1,(i%2) + 1);
					AfxMessageBox(strTemp);

					return;
				}
			}
		}
	}

	m_strTsiteSerial[TSITE_SERIAL_4_2][1] = strKey;
	m_msgSerialID_4_2.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgSerialId51()
{
	CString strKey,strTemp;

	KeyBoard(&strKey);

	strTemp = m_msgSerialID_5_1.GetWindowText();

	if (strTemp == strKey)
	{

	}
	else
	{
		for(int i =0; i<16; i++)
		{
			if (i != TSITE_SERIAL_5_1)
			{
				if (strKey == m_strTsiteSerial[i][1])
				{
					strTemp.Format(_T("%d Site %d Serial Name이 같습니다."), (i/2) + 1,(i%2) + 1);
					AfxMessageBox(strTemp);

					return;
				}
			}
		}
	}

	m_strTsiteSerial[TSITE_SERIAL_5_1][1] = strKey;
	m_msgSerialID_5_1.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgSerialId52()
{
	CString strKey,strTemp;

	KeyBoard(&strKey);

	strTemp = m_msgSerialID_5_2.GetWindowText();

	if (strTemp == strKey)
	{

	}
	else
	{
		for(int i =0; i<16; i++)
		{
			if (i != TSITE_SERIAL_5_2)
			{
				if (strKey == m_strTsiteSerial[i][1])
				{
					strTemp.Format(_T("%d Site %d Serial Name이 같습니다."), (i/2) + 1,(i%2) + 1);
					AfxMessageBox(strTemp);

					return;
				}
			}
		}
	}

	m_strTsiteSerial[TSITE_SERIAL_5_2][1] = strKey;
	m_msgSerialID_5_2.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgSerialId61()
{
	CString strKey,strTemp;

	KeyBoard(&strKey);

	strTemp = m_msgSerialID_6_1.GetWindowText();

	if (strTemp == strKey)
	{

	}
	else
	{
		for(int i =0; i<16; i++)
		{
			if (i != TSITE_SERIAL_6_1)
			{
				if (strKey == m_strTsiteSerial[i][1])
				{
					strTemp.Format(_T("%d Site %d Serial Name이 같습니다."), (i/2) + 1,(i%2) + 1);
					AfxMessageBox(strTemp);

					return;
				}
			}
		}
	}

	m_strTsiteSerial[TSITE_SERIAL_6_1][1] = strKey;
	m_msgSerialID_6_1.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgSerialId62()
{
	CString strKey,strTemp;

	KeyBoard(&strKey);

	strTemp = m_msgSerialID_6_2.GetWindowText();

	if (strTemp == strKey)
	{

	}
	else
	{
		for(int i =0; i<16; i++)
		{
			if (i != TSITE_SERIAL_6_2)
			{
				if (strKey == m_strTsiteSerial[i][1])
				{
					strTemp.Format(_T("%d Site %d Serial Name이 같습니다."), (i/2) + 1,(i%2) + 1);
					AfxMessageBox(strTemp);

					return;
				}
			}
		}
	}

	m_strTsiteSerial[TSITE_SERIAL_6_2][1] = strKey;
	m_msgSerialID_6_2.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgSerialId71()
{
	CString strKey,strTemp;

	KeyBoard(&strKey);

	strTemp = m_msgSerialID_7_1.GetWindowText();

	if (strTemp == strKey)
	{

	}
	else
	{
		for(int i =0; i<16; i++)
		{
			if (i != TSITE_SERIAL_7_1)
			{
				if (strKey == m_strTsiteSerial[i][1])
				{
					strTemp.Format(_T("%d Site %d Serial Name이 같습니다."), (i/2) + 1,(i%2) + 1);
					AfxMessageBox(strTemp);

					return;
				}
			}
		}
	}
	m_strTsiteSerial[TSITE_SERIAL_7_1][1] = strKey;
	m_msgSerialID_7_1.SetWindowTextW(strKey);	
}


void CWorkTesterNetwork::OnStnClickedMsgSerialId72()
{
	CString strKey,strTemp;

	KeyBoard(&strKey);

	strTemp = m_msgSerialID_7_2.GetWindowText();

	if (strTemp == strKey)
	{

	}
	else
	{
		for(int i =0; i<16; i++)
		{
			if (i != TSITE_SERIAL_7_2)
			{
				if (strKey == m_strTsiteSerial[i][1])
				{
					strTemp.Format(_T("%d Site %d Serial Name이 같습니다."), (i/2) + 1,(i%2) + 1);
					AfxMessageBox(strTemp);

					return;
				}
			}
		}
	}

	m_strTsiteSerial[TSITE_SERIAL_7_2][1] = strKey;
	m_msgSerialID_7_2.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgSerialId81()
{
	CString strKey,strTemp;

	KeyBoard(&strKey);

	strTemp = m_msgSerialID_8_1.GetWindowText();

	if (strTemp == strKey)
	{

	}
	else
	{
		for(int i =0; i<16; i++)
		{
			if (i != TSITE_SERIAL_8_1)
			{
				if (strKey == m_strTsiteSerial[i][1])
				{
					strTemp.Format(_T("%d Site %d Serial Name이 같습니다."), (i/2) + 1,(i%2) + 1);
					AfxMessageBox(strTemp);

					return;
				}
			}
		}
	}

	m_strTsiteSerial[TSITE_SERIAL_8_1][1] = strKey;
	m_msgSerialID_8_1.SetWindowTextW(strKey);
}


void CWorkTesterNetwork::OnStnClickedMsgSerialId82()
{
	CString strKey,strTemp;

	KeyBoard(&strKey);

	strTemp = m_msgSerialID_8_2.GetWindowText();

	if (strTemp == strKey)
	{

	}
	else
	{
		for(int i =0; i<16; i++)
		{
			if (i != TSITE_SERIAL_8_2)
			{
				if (strKey == m_strTsiteSerial[i][1])
				{
					strTemp.Format(_T("%d Site %d Serial Name이 같습니다."), (i/2) + 1,(i%2) + 1);
					AfxMessageBox(strTemp);

					return;
				}
			}
		}
	}

	m_strTsiteSerial[TSITE_SERIAL_8_2][1] = strKey;
	m_msgSerialID_8_2.SetWindowTextW(strKey);
}
