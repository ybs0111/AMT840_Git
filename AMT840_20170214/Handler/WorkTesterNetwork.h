#pragma once


// CWorkTesterNetwork 대화 상자입니다.
#include "Variable.h"
#include "XPGroupBox.h"
#include "xShadeButton.h"	// 조그 버튼 생성 클래스 추가
#include "GradientStatic.h"  // 그라데이션 라벨 클래스
#include "BtnST.h"
#include "afxcmn.h"

class CWorkTesterNetwork : public CDialog
{
	DECLARE_DYNAMIC(CWorkTesterNetwork)
public:
	CString				m_strLogKey[10];
	CString				m_strLogData[10];


	DWORD	m_dwWaitTime[3];

	BYTE	m_byIp[4];		// 0: FTP, 1: TAMS

	int		m_nRfidStep;
	int		m_nLightValue;
	int		m_nClientCnt;
	int		m_nServerCnt;
	int		m_nClientPort[10][2];
	int		m_nServerPort[10][2];
	int		m_nPort[10][2];
	int		m_nData[10][2];
	int		m_nParity[10][2];
	int		m_nStop[10][2];
	int		m_nRate[10][2];
	BOOL	m_bUnldBcrFlag[2][2];

	CString	m_strClientIp[10][2];
	CString	m_strServerIp[10][2];

	CString	m_strClientName[10][2];
	CString	m_strServerName[10][2];

	CString	m_strRfidData[20];

	CString	m_strTsiteSerial[16][2]; //kwlee 2016.1125

	void	OnInterfaceIpAddress(CString strIp);

	int		OnRfidRead();
	int		OnRfidWrite();
	void	OnInitGroupBox();
	void	OnInitLabel();
	void	OnInitButton();
	void	OnInterface_Display();
	void	OnDataHistoryLog();
	void	OnDataRecovery();
	int		OnDataComparison();
	void	OnDataApply();
	void	OnDataBackup();
	void	OnDataInit();
	void    OnInitName();
	void    OnWriteName();
public:
	CWorkTesterNetwork(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CWorkTesterNetwork();

// 대화 상자 데이터입니다.
	//enum { IDD = IDD_WORK_INTERFACE };
	enum { IDD = IDD_WORK_TESTER_NETWORK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL DestroyWindow();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

 	CXPGroupBox		m_groupClient1;
 	CXPGroupBox		m_groupClient2;
 	CXPGroupBox		m_groupClient3;
 	CXPGroupBox		m_groupClient4;
 	CXPGroupBox		m_groupClient5;
 	CXPGroupBox		m_groupClient6;
 	CXPGroupBox		m_groupClient7;
	CXPGroupBox		m_groupClient8;

 	CXPGroupBox		m_groupServer1;
	CXPGroupBox		m_groupServer2;
	CXPGroupBox		m_groupServer3;
	CXPGroupBox		m_groupServer4;
	CXPGroupBox		m_groupServer5;
	CXPGroupBox		m_groupServer6;
	CXPGroupBox		m_groupServer7;
	CXPGroupBox		m_groupServer8;

	CXPGroupBox		m_groupCommand;
	CXPGroupBox		m_groupTsiteSerial;
	CXPGroupBox		m_groupTsiteSerial_1;
	CXPGroupBox		m_groupTsiteSerial_2;
	CXPGroupBox		m_groupTsiteSerial_3;
	CXPGroupBox		m_groupTsiteSerial_4;
	CXPGroupBox		m_groupTsiteSerial_5;
	CXPGroupBox		m_groupTsiteSerial_6;
	CXPGroupBox		m_groupTsiteSerial_7;
	CXPGroupBox		m_groupTsiteSerial_8;
	CGradientStatic m_msgSerialName_1_1;
	CGradientStatic m_msgSerialName_1_2;
	CGradientStatic m_msgSerialName_2_1;
	CGradientStatic m_msgSerialName_2_2;
	CGradientStatic m_msgSerialName_3_1;
	CGradientStatic m_msgSerialName_3_2;
	CGradientStatic m_msgSerialName_4_1;
	CGradientStatic m_msgSerialName_4_2;
	CGradientStatic m_msgSerialName_5_1;
	CGradientStatic m_msgSerialName_5_2;
	CGradientStatic m_msgSerialName_6_1;
	CGradientStatic m_msgSerialName_6_2;
	CGradientStatic m_msgSerialName_7_1;
	CGradientStatic m_msgSerialName_7_2;
	CGradientStatic m_msgSerialName_8_1;
	CGradientStatic m_msgSerialName_8_2;
	CGradientStatic m_msgSerialID_1_1;
	CGradientStatic m_msgSerialID_1_2;
	CGradientStatic m_msgSerialID_2_1;
	CGradientStatic m_msgSerialID_2_2;
	CGradientStatic m_msgSerialID_3_1;
	CGradientStatic m_msgSerialID_3_2;
	CGradientStatic m_msgSerialID_4_1;
	CGradientStatic m_msgSerialID_4_2;
	CGradientStatic m_msgSerialID_5_1;
	CGradientStatic m_msgSerialID_5_2;
	CGradientStatic m_msgSerialID_6_1;
	CGradientStatic m_msgSerialID_6_2;
	CGradientStatic m_msgSerialID_7_1;
	CGradientStatic m_msgSerialID_7_2;
	CGradientStatic m_msgSerialID_8_1;
	CGradientStatic m_msgSerialID_8_2;
	CGradientStatic m_msgClientName1;
	CGradientStatic m_msgClientName2;
	CGradientStatic m_msgClientName3;
	CGradientStatic m_msgClientName4;
	CGradientStatic m_msgClientName5;
	CGradientStatic m_msgClientName6;
	CGradientStatic m_msgClientName7;
	CGradientStatic m_msgClientName8;

	CGradientStatic m_msgServerName1;
	CGradientStatic m_msgServerName2;
	CGradientStatic m_msgServerName3;
	CGradientStatic m_msgServerName4;
	CGradientStatic m_msgServerName5;
	CGradientStatic m_msgServerName6;
	CGradientStatic m_msgServerName7;
	CGradientStatic m_msgServerName8;

	CGradientStatic m_msgClientIp1;
	CGradientStatic m_msgClientIp2;
	CGradientStatic m_msgClientIp3;
	CGradientStatic m_msgClientIp4;
	CGradientStatic m_msgClientIp5;
	CGradientStatic m_msgClientIp6;
	CGradientStatic m_msgClientIp7;
	CGradientStatic m_msgClientIp8;

	CGradientStatic m_msgClientPort1;
	CGradientStatic m_msgClientPort2;
	CGradientStatic m_msgClientPort3;
	CGradientStatic m_msgClientPort4;
	CGradientStatic m_msgClientPort5;
	CGradientStatic m_msgClientPort6;
	CGradientStatic m_msgClientPort7;
	CGradientStatic m_msgClientPort8;

	CGradientStatic m_msgServerIp1;
	CGradientStatic m_msgServerIp2;
	CGradientStatic m_msgServerIp3;
	CGradientStatic m_msgServerIp4;
	CGradientStatic m_msgServerIp5;
	CGradientStatic m_msgServerIp6;
	CGradientStatic m_msgServerIp7;
	CGradientStatic m_msgServerIp8;

	CGradientStatic m_msgServerPort1;
	CGradientStatic m_msgServerPort2;
	CGradientStatic m_msgServerPort3;
	CGradientStatic m_msgServerPort4;
	CGradientStatic m_msgServerPort5;
	CGradientStatic m_msgServerPort6;
	CGradientStatic m_msgServerPort7;
	CGradientStatic m_msgServerPort8;
	
	CGradientStatic m_msgServerPortData_1;
	CGradientStatic m_msgServerPortData_2;
	CGradientStatic m_msgServerPortData_3;
	CGradientStatic m_msgServerPortData_4;
	CGradientStatic m_msgServerPortData_5;
	CGradientStatic m_msgServerPortData_6;
	CGradientStatic m_msgServerPortData_7;
	CGradientStatic m_msgServerPortData_8;
	CGradientStatic m_msgServerID_1;
	CGradientStatic m_msgServerID_2;
	CGradientStatic m_msgServerID_3;
	CGradientStatic m_msgServerID_4;
	CGradientStatic m_msgServerID_5;
	CGradientStatic m_msgServerID_6;
	CGradientStatic m_msgServerID_7;
	CGradientStatic m_msgClientPortData_1;
	CGradientStatic m_msgClientPortData_2;
	CGradientStatic m_msgClientPortData_3;
	CGradientStatic m_msgClientPortData_4;
	CGradientStatic m_msgClientPortData_5;
	CGradientStatic m_msgClientPortData_6;
	CGradientStatic m_msgClientPortData_7;
	CGradientStatic m_msgClientPortData_8;
	CGradientStatic m_msgClientID_1;
	CGradientStatic m_msgClientID_3;
	CGradientStatic m_msgClientID_2;
	CGradientStatic m_msgClientID_4;
	CGradientStatic m_msgClientID_5;
	CGradientStatic m_msgClientID_6;
	CGradientStatic m_msgClientID_7;
	CGradientStatic m_msgClientID_8;
	CGradientStatic m_msgServerID_8;

	CIPAddressCtrl	m_clientIp1;
	CIPAddressCtrl	m_clientIp2;
	CIPAddressCtrl	m_clientIp3;
	CIPAddressCtrl	m_clientIp4;
	CIPAddressCtrl	m_clientIp5;
	CIPAddressCtrl	m_clientIp6;
	CIPAddressCtrl	m_clientIp7;
	CIPAddressCtrl	m_clientIp8;

	CIPAddressCtrl	m_ServerIp1;
	CIPAddressCtrl	m_ServerIp2;
	CIPAddressCtrl	m_ServerIp3;
	CIPAddressCtrl	m_ServerIp4;
	CIPAddressCtrl	m_ServerIp5;
	CIPAddressCtrl	m_ServerIp6;
	CIPAddressCtrl	m_ServerIp7;
	CIPAddressCtrl	m_ServerIp8;

	CEdit	m_editClientName1;
	CEdit	m_editClientName2;
	CEdit	m_editClientName3;
	CEdit	m_editClientName4;
	CEdit	m_editClientName5;
	CEdit	m_editClientName6;
	CEdit	m_editClientName7;
	CEdit	m_editClientName8;

	CEdit	m_editServerName1;
	CEdit	m_editServerName2;
	CEdit	m_editServerName3;
	CEdit	m_editServerName4;
	CEdit	m_editServerName5;
	CEdit	m_editServerName6;
	CEdit	m_editServerName7;
	CEdit	m_editServerName8;

	CEdit	m_editServerPort1;
	CEdit	m_editServerPort2;
	CEdit	m_editServerPort3;
	CEdit	m_editServerPort4;
	CEdit	m_editServerPort5;
	CEdit	m_editServerPort6;
	CEdit	m_editServerPort7;
	CEdit	m_editServerPort8;

	CEdit	m_editClientPort1;
	CEdit	m_editClientPort2;
	CEdit	m_editClientPort3;
	CEdit	m_editClientPort4;
	CEdit	m_editClientPort5;
	CEdit	m_editClientPort6;
	CEdit	m_editClientPort7;
	CEdit	m_editClientPort8;

	CButtonST	m_btnClientIpSet[10];
	CButtonST	m_btnClientPortSet[10];

	CButtonST	m_btnServerIpSet[8];
	CButtonST	m_btnServerPortSet[8];

	CButtonST	m_btnApply;
	CButtonST	m_btnReLoad;

	CXPGroupBox m_groupEcServer;
	CXPGroupBox m_groupXgem;
	CGradientStatic m_msgEcServerIP;
	CIPAddressCtrl m_EcServerIp;
	CGradientStatic m_msgEcServerPortData;
	CGradientStatic m_msgXgemIp;
	CGradientStatic m_msgEcServerPort;
	CGradientStatic m_msgXgemPort;
	CGradientStatic m_msgXgemPortData;
	CIPAddressCtrl m_XgemIp;

	afx_msg void OnBnClickedBtnApply();
	afx_msg void OnBnClickedBtnReload();
	virtual void PostNcDestroy();
	afx_msg void OnDestroy();
	//afx_msg void OnClickedBtnBcodeTriggerOff();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnServerIp1();
	afx_msg void OnBnClickedBtnServerPort1();
	afx_msg void OnBnClickedBtnServerIp2();
	afx_msg void OnBnClickedBtnServerPort2();
	afx_msg void OnBnClickedBtnServerIp3();
	afx_msg void OnBnClickedBtnServerPort3();
	afx_msg void OnBnClickedBtnServerIp4();
	afx_msg void OnBnClickedBtnServerPort4();
	afx_msg void OnBnClickedBtnServerIp5();
	afx_msg void OnBnClickedBtnServerPort5();
	afx_msg void OnBnClickedBtnServerIp6();
	afx_msg void OnBnClickedBtnServerPort6();
	afx_msg void OnBnClickedBtnServerIp7();
	afx_msg void OnBnClickedBtnServerPort7();
	afx_msg void OnBnClickedBtnServerIp8();
	afx_msg void OnBnClickedBtnServerPort8();
	afx_msg void OnBnClickedBtnClientIp1();
	afx_msg void OnBnClickedBtnClientPort1();
	afx_msg void OnBnClickedBtnClientIp2();
	afx_msg void OnBnClickedBtnClientPort2();
	afx_msg void OnBnClickedBtnClientIp3();
	afx_msg void OnBnClickedBtnClientPort3();
	afx_msg void OnBnClickedBtnClientIp4();
	afx_msg void OnBnClickedBtnClientPort4();
	afx_msg void OnBnClickedBtnClientIp5();
	afx_msg void OnBnClickedBtnClientPort5();
	afx_msg void OnBnClickedBtnClientIp6();
	afx_msg void OnBnClickedBtnClientPort6();
	afx_msg void OnBnClickedBtnClientIp7();
	afx_msg void OnBnClickedBtnClientPort7();
	afx_msg void OnBnClickedBtnClientIp8();
	afx_msg void OnBnClickedBtnClientPort8();
	afx_msg void OnEnChangeEditServerPort4();
	afx_msg void OnEnChangeEditServerName1();
	afx_msg void OnStnClickedMsgServerPortData1();
	afx_msg void OnStnClickedMsgServerPortData2();
	afx_msg void OnStnClickedMsgServerPortData3();
	afx_msg void OnStnClickedMsgServerPortData4();
	afx_msg void OnStnClickedMsgServerPortData5();
	afx_msg void OnStnClickedMsgServerPortData6();
	afx_msg void OnStnClickedMsgServerPortData7();
	afx_msg void OnStnClickedMsgServerPortData8();
	afx_msg void OnStnClickedMsgServerId1();
	afx_msg void OnStnClickedMsgServerId2();
	afx_msg void OnStnClickedMsgServerId3();
	afx_msg void OnStnClickedMsgServerId4();
	afx_msg void OnStnClickedMsgServerId5();
	afx_msg void OnStnClickedMsgServerId6();
	afx_msg void OnStnClickedMsgServerId7();
	afx_msg void OnStnClickedMsgServerId8();
	afx_msg void OnStnClickedMsgClientPortData1();
	afx_msg void OnStnClickedMsgClientId1();
	afx_msg void OnStnClickedMsgClientPortData2();
	afx_msg void OnStnClickedMsgClientPortData3();
	afx_msg void OnStnClickedMsgClientPortData4();
	afx_msg void OnStnClickedMsgClientPortData5();
	afx_msg void OnStnClickedMsgClientPortData6();
	afx_msg void OnStnClickedMsgClientPortData7();
	afx_msg void OnStnClickedMsgClientPortData8();
	afx_msg void OnStnClickedMsgClientId2();
	afx_msg void OnStnClickedMsgClientId3();
	afx_msg void OnStnClickedMsgClientId4();
	afx_msg void OnStnClickedMsgClientId5();
	afx_msg void OnStnClickedMsgClientId6();
	afx_msg void OnStnClickedMsgClientId7();
	afx_msg void OnStnClickedMsgClientId8();
	
	afx_msg void OnStnClickedMsgEcserverPortData();
	afx_msg void OnStnClickedMsgXgemPortData();
	afx_msg void OnBnClickedBtnEcserverIp();
	afx_msg void OnBnClickedBtnXgemIp();
	
	afx_msg void OnStnClickedMsgSerialId11();
	afx_msg void OnStnClickedMsgSerialId12();
	afx_msg void OnStnClickedMsgSerialId21();
	afx_msg void OnStnClickedMsgSerialId22();
	afx_msg void OnStnClickedMsgSerialId31();
	afx_msg void OnStnClickedMsgSerialId32();
	afx_msg void OnStnClickedMsgSerialId41();
	afx_msg void OnStnClickedMsgSerialId42();
	afx_msg void OnStnClickedMsgSerialId51();
	afx_msg void OnStnClickedMsgSerialId52();
	afx_msg void OnStnClickedMsgSerialId61();
	afx_msg void OnStnClickedMsgSerialId62();
	afx_msg void OnStnClickedMsgSerialId71();
	afx_msg void OnStnClickedMsgSerialId72();
	afx_msg void OnStnClickedMsgSerialId81();
	afx_msg void OnStnClickedMsgSerialId82();
};
