#pragma once

#include "XPGroupBox.h"
#include "xShadeButton.h"	// 조그 버튼 생성 클래스 추가
#include "GradientStatic.h"  // 그라데이션 라벨 클래스
#include "BtnST.h"
#include "Digit.h"
#include <afxtempl.h>
#include ".\\Union\\Grid\\GridCtrl.h"
#include ".\\Union\\Grid\\GridURLCell.h"
#include ".\\Union\\Grid\\GridCellCombo.h"
#include ".\\Union\\Grid\\GridCellCheck.h"
#include ".\\Union\\Grid\\GridCellNumeric.h"
#include ".\\Union\\Grid\\GridCellDateTime.h"
#include "FtpClient.h"
// CWorkFtp 대화 상자입니다.

class CWorkFtp : public CDialog
{
	DECLARE_DYNAMIC(CWorkFtp)

public:
	CString				m_strLogKey[10];
	CString				m_strLogData[10];

	CFtpClient			m_pFtp;

	CBitmap				m_bmGround;

	BYTE				m_byIp[4];		// 0: FTP, 1: TAMS

	int					m_nFtpGmsUse[2];
	int					m_nFtpPort[2];
	int					m_nVnRTime[2];
	int					m_nRecipeYesNo[2];

	CString				m_strFtpIP[2];
	CString				m_strFtpUser[2];
	CString				m_strFtpPassword[2];
	CString				m_strFtpGmsPath[2];
	CString				m_strScrapCodeName[2];		// jtkim 20150529
	CString				m_strScrapEqpName[2];		// jtkim 20150529

	CString				m_strVnrSpec[2][2][8];		//Backup, 전압&저항, 채널.
	CString				m_strVnrUcl[2][2][8];		//Backup, 전압&저항, 채널.
	CString				m_strVnrLcl[2][2][8];		//Backup, 전압&저항, 채널.

	CStringArray		m_ArValVolt[8];			//전압데이터 저장배열.
	CStringArray		m_ArValRes[8];			//저항데이터 저장배열.

	CButtonST			m_btnFtpGmsUse;
	CButtonST			m_btnApply;
	CButtonST			m_btnReLoad;
	CButtonST			m_btnFtpIP;
	CButtonST			m_btnFtpPort;
	CButtonST			m_btnGmsSend;
	CButtonST			m_btnRead;
	CButtonST			m_btnRecipe;
	CButtonST			m_btnRecipeYesNo;

	CXPGroupBox			m_groupFtp;
	CXPGroupBox			m_groupFtpGms;

	CGradientStatic		m_msgFtpIP;
	CGradientStatic		m_msgFtpPort;
	CGradientStatic		m_msgUser;
	CGradientStatic		m_msgUserData;
	CGradientStatic		m_msgPass;
	CGradientStatic		m_msgPassData;
	CGradientStatic		m_msgGmsPath;
	CGradientStatic		m_msgGmsPathData;
	CGradientStatic		m_msgVnRTime;
	CGradientStatic		m_msgScrapCodeName;
	CGradientStatic		m_msgScrapCodeNameData;
	CGradientStatic		m_msgScrapEqpName;
	CGradientStatic		m_msgScrapEqpNameData;

	CGridCtrl			m_pGridVnR;

	CDigit				m_dgtVnRTime;
	
	CGradientStatic		m_msgGmsCh[8];
	CGradientStatic		m_msgGmsVoltData[8];
	CGradientStatic		m_msgGmsResData[8];

	CIPAddressCtrl		m_aFtpIP;

	CEdit				m_editFtpPort;

	void				OnInterfaceIpAddress(CString strIP);
	void				OnInitGroupBox();
	void				OnInitLabel();
	void				OnInitButton();
	void				OnInitDigit();
	void				OnInitGridVnR();
	void				OnInitArray();
	void				OnDataHistoryLog();
	void				OnDataRecovery();
	int					OnDataComparison();
	void				OnDataApply();
	void				OnDataBackup();
	void				OnDataInit();
	void				OnVnRDisplay();

public:
	CWorkFtp(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CWorkFtp();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_WORK_FTP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL DestroyWindow();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual void PostNcDestroy();
	afx_msg void OnClickedBtnClientIpFtp();
	afx_msg void OnClickedBtnClientPortFtp();
	afx_msg void OnClickedCheckWorkGmsUse();
	afx_msg void OnClickedMsgUserData();
	afx_msg void OnClickedMsgPasswordData();
	afx_msg void OnClickedMsgGmsFtpPathData();
	afx_msg void OnClickedBtnGmsSend();
	afx_msg void OnStnClickedDgtVnrTime();
	afx_msg void OnBnClickedBtnApply();
	afx_msg LRESULT OnDataDisplay(WPARAM wParam, LPARAM lParam);
	afx_msg void OnVnRCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnStnClickedMsgScrapCodeNameData();
	afx_msg void OnStnClickedMsgScrapEqpNameData();
	afx_msg void OnBnClickedRadioNotUsed();
	afx_msg void OnBnClickedRadioLocal();
	afx_msg void OnBnClickedRadioFtp();
	afx_msg void OnBnClickedBtnFtpRead();
	int m_nFtpMode;
	afx_msg void OnBnClickedBtnFtpRecipeDownload();
	afx_msg void OnBnClickedChkFtpRecipeDownload();
};
