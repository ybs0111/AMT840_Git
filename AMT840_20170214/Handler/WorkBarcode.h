#pragma once

#include "Variable.h"
#include "XPGroupBox.h"
#include "xShadeButton.h"	// 조그 버튼 생성 클래스 추가
#include "GradientStatic.h"  // 그라데이션 라벨 클래스
#include "BtnST.h"
#include "afxcmn.h"
#include "afxwin.h"
// CWorkBarcode 대화 상자입니다.

class CWorkBarcode : public CDialogEx
{
	DECLARE_DYNAMIC(CWorkBarcode)
public:
	CString				m_strLogKey[10];
	CString				m_strLogData[10];

	int				OnBarcode_1D();
	int				OnBarcode_2D();
	
	int				ConverterToData(int nMode, int nPos);
	int				ConverterToPos(int nMode, int Ndata);
	void			OnInitLabel();
	void			OnDataInit();
	void            OnDataApply();
	void            OnInitGroupBox();
	void			OnInitButton();
	void            OnInt_Display();
	void            OnDataRecovery();
	int				OnDataComparison();
	void			OnInterfaceIpAddress(CString strIp);
	void            OnDataBackup();
public:

	DWORD			m_dwWaitTime[3];
	BYTE			m_byIp[4];	
	int				m_nBarcodePos;
	int				m_nBarcodeStep;

	int				m_nPort[10][2];
	int				m_nData[10][2];
	int				m_nParity[10][2];
	int				m_nStop[10][2];
	int				m_nRate[10][2];
	int				m_nClientPort[10][2];
	int             m_nGmsServerPort[2];
	CString			m_strClientIp[10][2];

	CButtonST		m_btnSerialApply[5];
	CButtonST		m_btnSerialConvet[5];
	CButtonST		m_btnClientIpSet[10];
	CButtonST		m_btnClientPortSet[7];
	CButtonST		m_btnApply;
	CButtonST		m_btnReLoad;
	CButtonST		m_btnBcodeOn[3];
	CButtonST		m_btnBcodeOff;
	CButtonST		m_btnUnldBcr[2];
	CListBox		m_listBarcode;

	CComboBox		m_cbPort[2];
	CComboBox		m_cbRate[2];
	CComboBox		m_cbData[2];
	CComboBox		m_cbStop[2];
	CComboBox		m_cbParity[2];

	CXPGroupBox		m_groupLdBcr;
	CXPGroupBox		m_groupUnldBcrA;
	CXPGroupBox		m_groupSerial1;

	CXPGroupBox		m_groupClient1;
	CXPGroupBox		m_groupClient2;
	CXPGroupBox		m_groupClient3;
	CXPGroupBox		m_groupClient6;

	CIPAddressCtrl	m_clientIp1;
	CIPAddressCtrl	m_clientIp2;
	CIPAddressCtrl	m_clientIp3;
	CIPAddressCtrl	m_clientIp6;

	CGradientStatic m_msgClientIp1;
	CGradientStatic m_msgClientPort1;
	CGradientStatic m_msgClientIp2;
	CGradientStatic m_msgClientPort2;
	CGradientStatic m_msgClientIp3;
	CGradientStatic m_msgClientPort3;
	CGradientStatic m_msgClientIp6;
	CGradientStatic m_msgClientPort6;
	CGradientStatic m_msgPort[2];
	CGradientStatic m_msgRate[2];
	CGradientStatic m_msgData[2];
	CGradientStatic m_msgStop[2];
	CGradientStatic m_msgParity[2];

	CEdit			m_editClientPort1;
	CEdit			m_editClientPort2;
	CEdit			m_editClientPort3;
	CEdit			m_editClientPort6;

public:
	CWorkBarcode(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CWorkBarcode();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_WORK_INTERFACE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnSerialApply1();
	afx_msg void OnBnClickedBtnSerialConnect1();
	afx_msg void OnBnClickedBtnSerialConnect2();
	afx_msg void OnBnClickedBtnBcodeTriggerOn();
	afx_msg void OnBnClickedBtnSerialApply2();
	afx_msg void OnBnClickedBtnBcodeTriggerOn2();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnApply();
	afx_msg void OnBnClickedBtnReload();
	afx_msg void OnBnClickedBtnClientIp1();
	afx_msg void OnBnClickedBtnClientPort1();
	afx_msg void OnBnClickedBtnClientIp2();
	afx_msg void OnBnClickedBtnClientPort2();
	afx_msg void OnBnClickedBtnClientIp3();
	afx_msg void OnBnClickedBtnClientPort3();
	afx_msg void OnBnClickedBtnClientIp6();
	afx_msg void OnBnClickedBtnClientPort6();
	CGradientStatic m_msg_Barcode_1d_client_ip;
	CGradientStatic m_msg_Barcode_1d_client_port;
	CIPAddressCtrl m_Barcode_1d_client_ip;
	CEdit m_edit_Barcode_1d_client_port;
	afx_msg void OnBnClickedBtnBarcode1dClientIp();
	afx_msg void OnBnClickedBtnBarcode1dClientPort();
};
