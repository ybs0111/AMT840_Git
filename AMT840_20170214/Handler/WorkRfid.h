#pragma once


// CWorkRfid 대화 상자입니다.
#include "Variable.h"
#include "XPGroupBox.h"
#include "xShadeButton.h"	// 조그 버튼 생성 클래스 추가
#include "GradientStatic.h"  // 그라데이션 라벨 클래스
#include "BtnST.h"

class CWorkRfid : public CDialog
{
	DECLARE_DYNAMIC(CWorkRfid)

public:
	CWorkRfid(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CWorkRfid();
	
	void	OnDisplayData();
	void	OnInitGroupBox();
	void	OnInitLabel();
	void	OnInitButton();
	void	OnInterface_Display();
	int		OnRfidRead();
	int		OnRfidWrite();
	void	OnDataHistoryLog();
	void	OnDataRecovery();
	int		OnDataComparison();
	void	OnDataApply();
	void	OnDataBackup();
	void	OnDataInit();
	void	OnInterface_IP(CString strIp);

public:
	int			m_nRfidStep;
	int			m_nRetry;

	DWORD		m_dwWaitTime[3];

	CString		m_strRfidIp[2];
	CString		m_strRfidData[20];

	int			m_nRfidPort[2];

	BYTE		m_byIp[4];		// 0: FTP, 1: TAMS
public:
	CXPGroupBox		m_groupRfid;
	CXPGroupBox		m_groupRfidInfo;

	CGradientStatic m_msgClientIpRfid;
	CGradientStatic m_msgClientPortRfid;

	CGradientStatic m_msgCarrier;
	CGradientStatic m_msgCarrierData;
	CGradientStatic m_msgEqpNo;
	CGradientStatic m_msgEqpNoData;
	CGradientStatic m_msgQty;
	CGradientStatic m_msgQtyData;
	CGradientStatic m_msgLotNoJ;
	CGradientStatic m_msgLotNoJData;
	CGradientStatic m_msgStep;
	CGradientStatic m_msgStepData;
	CGradientStatic m_msgPartNo;
	CGradientStatic m_msgPartNoData;
	CGradientStatic m_msgLotNoM;
	CGradientStatic m_msgLotNoMData;
	CGradientStatic m_msgQtyT;
	CGradientStatic m_msgQtyTData;
	CGradientStatic m_msgLotType;
	CGradientStatic m_msgLotTypeData;
	CGradientStatic m_msgPkgCode;
	CGradientStatic m_msgPkgCodeData;
	CGradientStatic m_msgCnt;
	CGradientStatic m_msgCntData;
	CGradientStatic m_msgFlag;
	CGradientStatic m_msgFlagData;
	CGradientStatic m_msgReject;
	CGradientStatic m_msgRejectData;
	CGradientStatic m_msgBin;
	CGradientStatic m_msgBinData;
	CGradientStatic m_msgTrayQty;
	CGradientStatic m_msgTrayQtyData;
	CGradientStatic m_msgNone1;
	CGradientStatic m_msgNone1Data;
	CGradientStatic m_msgNone2;
	CGradientStatic m_msgNone2Data;

	CButtonST		m_btnClientIpRfid;
	CButtonST		m_btnClientPortRfid;
	CButtonST		m_btnRfidRead;
	CButtonST		m_btnRfidWrite;
	CButtonST		m_btnApply;
	CButtonST		m_btnReLoad;

	CIPAddressCtrl	m_clientIpRfid;

	CEdit			m_editClientPortRfid;
// 대화 상자 데이터입니다.
	enum { IDD = IDD_WORK_RFID };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	afx_msg void OnBnClickedBtnClientIpRfid();
	afx_msg void OnBnClickedBtnClientPortRfid();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonRfidRead();
	afx_msg void OnBnClickedButtonRfidWrite();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedBtnReload();
	afx_msg void OnBnClickedBtnApply();
	afx_msg void OnStnClickedMsgCarrierIdData();
	afx_msg void OnStnClickedMsgEqpNoData();
	afx_msg void OnStnClickedMsgQtyData();
	afx_msg void OnStnClickedMsgLotNoJData();
	afx_msg void OnStnClickedMsgStepData();
	afx_msg void OnStnClickedMsgPartNoData();
	afx_msg void OnStnClickedMsgLotNoMData();
	afx_msg void OnStnClickedMsgQtyTData();
	afx_msg void OnStnClickedMsgLotTypeData();
	afx_msg void OnStnClickedMsgPkgCodeData();
	afx_msg void OnStnClickedMsgCntData();
	afx_msg void OnStnClickedMsgLotFlagData();
	afx_msg void OnStnClickedMsgRejectData();
	afx_msg void OnStnClickedMsgBinData();
	afx_msg void OnStnClickedMsgTrayQtyData();
	afx_msg void OnStnClickedMsgNone1Data();
	afx_msg void OnStnClickedMsgNone2Data();
};
