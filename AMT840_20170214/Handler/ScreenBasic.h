#pragma once



// CScreenBasic 폼 뷰입니다.

#include "Variable.h"
#include "XPGroupBox.h"
#include ".\\Union\\Grid\\GridCtrl.h"
#include ".\\Union\\Grid\\GridURLCell.h"
#include ".\\Union\\Grid\\GridCellCombo.h"
#include ".\\Union\\Grid\\GridCellCheck.h"
#include ".\\Union\\Grid\\GridCellNumeric.h"
#include ".\\Union\\Grid\\GridCellDateTime.h"
#include "BtnST.h"
#include "GradientStatic.h"
#include "Digit.h"


class CScreenBasic : public CFormView
{
	DECLARE_DYNCREATE(CScreenBasic)

public:
	CString				m_strLogKey[10];
	CString				m_strLogData[10];

	int					m_nModeTestInterface[2];
	int					m_nDeviceName;					// 선택된 디바이스의 리스트 위치 저장 변수
	int					m_nOldDeviceName;
	int					m_nModeDevice[2];
	int					m_nModeInterface[2];
	int					m_nModeSecsGem[2];
	int					m_nModeWork[2];
	int					m_nModeFrontSmema[2];
	int					m_nModeRearSmema[2];
	int					m_nModeRfid[2];
	int					m_nColCnt[2];
	int					m_nRowCnt[2];
	int					m_nBcrBank[2];
	int                 m_nGmsTime[2];
	int                 m_nCount;
	int					m_nFtpUse[2];
	int                 m_n1DBarcodeUse[2];
	int                 m_n2DBarcodeUse[2];
	int					m_nAbortTime[2];
	int					m_nBarcodeCnt[2];
	int                 m_nBcr_1d_Len[2];
	int                 m_nBcr_2d_IBM_Len[2];
	int                 m_nBcr_2d_SAMSUNG_Len[2];
	int                 m_nStackerTrayCnt[2]; //kwlee 20160930
	//kwlee 2016.1024
	int                 m_nDirectionCheck[2];
	int                 m_nAlarmDelayCnt[2];
	//kwlee 2016.1201
	int                 m_nAutoLoadMode[2]; 
	//kwlee 2017.0225
	int                m_nLotEndMode[2];
	//

	bool                m_bTimerCheck;
	int                 m_nModeSel;
	int					m_nLdUldLightCutainSkip[2];
	int					m_nRejLightCutainSkip[2];
	CString				m_strTempDevice;				// 선택된 디바이스 종류 임시 저장 변수
	CString				m_strLoadFile;					// 로딩할 파일에 대한 [폴더]+[파일명] 저장 변수
	CString				m_strDeviceName[2];
	CString				m_strModelName[2];
	CString             m_strBarcodeData[2];
	CString             m_strHifixData[2];   

	CButtonST			m_btnModelCreate;
	CButtonST			m_btnModelDelete;
	CButtonST			m_btnApply;
	CButtonST			m_btnReLoad;
	CButtonST			m_btnTimer;
	CButtonST			m_btnAuto;

	CXPGroupBox			m_groupModelInfo;
	CXPGroupBox			m_groupDeviceMode;
	CXPGroupBox			m_group_barcode_info;
	CXPGroupBox			m_group_barcode_1d;
	CXPGroupBox			m_group_barcode_2d;
	CXPGroupBox			m_groupProcess;
	CXPGroupBox			m_groupCdimm_Tray;
	CXPGroupBox			m_groupBcrBank;
	CXPGroupBox			m_groupGmsTime;
	CXPGroupBox			m_groupPgmMode;
	CXPGroupBox			m_groupHifix;
	CXPGroupBox			m_groupAutoMode;
	CGridCtrl			m_pGridDevice;
	CGridCtrl			m_pGridModel;
	CGridCtrl			m_pGridHifix;
	CGridCtrl			m_pGridPgmMode;
	
	
	CGradientStatic		m_msgHifixData;
	CGradientStatic		m_msgHifixInfo;
	CGradientStatic		 m_msg_samsung_barcode_data;
	CGradientStatic		 m_msgBcrBank;
	CGradientStatic		 m_msg_length;
	CGradientStatic		 m_msgPcbRowCnt;
	CGradientStatic		 m_msgPcbColCnt;
	CGradientStatic		 m_msgGmsTime;
	CGradientStatic		 m_msg_IBM;
	CGradientStatic		 m_msg_SAMSUNG;
	CGradientStatic		 m_msg_barcode_data;

	CGradientStatic			m_msgAbortTime;

	CGradientStatic			m_msgBarcodeCnt;
	//kwlee 20160930
	CGradientStatic			m_msgStackerTrayCnt;
	CDigit					m_dgtStackTrayCnt;
	//
	CDigit					m_dgtBarcodeCnt;

	CDigit					m_dgtAbortTime;

	CDigit				 m_dgt_length;
	CDigit				 m_dgt_ibm_length;
	CDigit				 m_dgt_samsung_length;
	CDigit				 m_dgtPcbRowCnt;
	CDigit				 m_dgtPcbColCnt;
		
	
	CDigit				m_dgtBcrBank;
	CDigit				 m_dgtGmsTime;
	
	void				OnInitGroupBox();
	void				OnInitLabel();
	void				OnInitButton();
	void				OnInitDigit();
	void				OnInitGridDeviceMode();
	void				OnInitGridPgmMode();
	void				OnInitGridModelList();
	void				OnDataHistoryLog();
	void				OnDeviceColorChange(int nSelectRow, int nSelectCol);
	
	void				OnDataRecovery();
	int					OnDataComparison();
	void				OnDataApply();
	void				OnDataBackup();
	void				OnDataInit();
	int					OnInitModelList(CString strFileName);
	int					OnAddItem(int nIndex, WIN32_FIND_DATA *pFd);
	void				OnDisplayModelList(int nMode, int nCount, CString strFileName);
	bool				OnModelDeviceCheck(CString strFileName);
	bool				OnModelCreateDeviceCheck(CString strFileName);
	

public:
	CScreenBasic();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CScreenBasic();

public:
	enum { IDD = IDD_SCREEN_BASIC };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	virtual BOOL DestroyWindow();
	afx_msg void OnDeviceCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	//afx_msg void OnWorkModeCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnModelCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnModelCellRClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnPgmModeCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnClickedBtnModelCreate();
	afx_msg void OnClickedBtnModelDelete();
	afx_msg void OnClickedBtnReload();
	afx_msg void OnClickedBtnApply();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void PostNcDestroy();
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnStnClickedDgtRowCnt();
	afx_msg void OnStnClickedDgtColCnt();
	afx_msg void OnStnClickedDgtGmsTime();
	afx_msg void OnBnClickedBtnTimer();
	afx_msg void OnStnClickedDgtBasicBarcodeBank();
	afx_msg void OnStnClickedDgtIbmLength();
	afx_msg void OnStnClickedDgtSamsungLength();
	afx_msg void OnStnClickedMsgBarcodeData();
	afx_msg void OnStnClickedDgtLength();
	
	afx_msg void OnBnClickedRadioLocal();
	afx_msg void OnBnClickedRadioFtp();
	
	afx_msg void OnStnClickedMsgHifixData();

	afx_msg void OnStnClickedDgtAbortTime();
	afx_msg void OnStnClickedDgtBarcodeCnt();
	afx_msg void OnBnClickedBtnAuto();
	
	afx_msg void OnStnClickedDgtStackerTrayCnt();

	CXPGroupBox m_groupManualStart;
	CButtonST m_btnTester1;
	CButtonST m_btnTester2;
	CButtonST m_btnTester3;
	CButtonST m_btnTester4;
	CButtonST m_btnTester6;
	CButtonST m_btnTester5;
	CButtonST m_btnTester7;
	CButtonST m_btnTester8;
	afx_msg void OnBnClickedBtnTest1();
	afx_msg void OnBnClickedBtnTest2();
	afx_msg void OnBnClickedBtnTest3();
	afx_msg void OnBnClickedBtnTest4();
	afx_msg void OnBnClickedBtnTest5();
	afx_msg void OnBnClickedBtnTest6();
	afx_msg void OnBnClickedBtnTest7();
	afx_msg void OnBnClickedBtnTest8();
	afx_msg void OnBnClickedBtnChamber1();
	afx_msg void OnBnClickedBtnChamber2();
	afx_msg void OnBnClickedBtnChamber3();
	afx_msg void OnBnClickedBtnChamber4();
	afx_msg void OnBnClickedBtnChamber5();
	afx_msg void OnBnClickedBtnChamber6();
	afx_msg void OnBnClickedBtnChamber7();
	afx_msg void OnBnClickedBtnChamber8();
	CButtonST m_btnChamber_1;
	CButtonST m_Chamber_2;
	CButtonST m_btnChamber_3;
	CButtonST m_btnChamber_4;
	CButtonST m_btnChamber_5;
	CButtonST m_btnChamber_6;
	CButtonST m_btnChamber_7;
	CButtonST m_btnChamber_8;
	CXPGroupBox m_groupManualChamberDoor;
	afx_msg void OnBnClickedBtnDirection();
	afx_msg void OnStnClickedDgtAlarmDelayCnt();
	CDigit m_dgtAlarmDelayCnt;
	CGradientStatic m_msgAlarmDelayCnt;
	CXPGroupBox m_groupDirection;
	CButtonST m_btnDirection;
	CXPGroupBox m_groupAutoLoad;
	CButtonST m_btnAutoLoad;
	afx_msg void OnBnClickedBtnAutoload();
	CXPGroupBox m_groupLotEndUse;
	afx_msg void OnBnClickedBtnLotendUse();
	CButtonST m_btnLotendUse;
};


