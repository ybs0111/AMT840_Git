#pragma once

#include ".\\Controls\\Picture.h"  // 이미지 로딩 클래스
#include "XPGroupBox.h"
#include "xShadeButton.h"	// 조그 버튼 생성 클래스 추가
#include ".\\Union\\Grid\\GridCtrl.h"
#include ".\\Union\\Grid\\GridURLCell.h"
#include ".\\Union\\Grid\\GridCellCombo.h"
#include ".\\Union\\Grid\\GridCellCheck.h"
#include ".\\Union\\Grid\\GridCellNumeric.h"
#include ".\\Union\\Grid\\GridCellDateTime.h"
#include "Digit.h"
#include "BtnST.h"
#include "GradientStatic.h"
#include "MyBasicData.h"
#include "SlotInfo.h"
#include "Animate.h"

class CDialogLotDisplay;
class CDialogLotStart;
class CDialogMessageBox;
class CDialogMessageView;
class CDialogTesterInterface;
class CDialogPcbTray;
class CDialogPcbPicker;
class CDialogPcbBuffer;
class CDialogBarcode;

class CScreenMain : public CFormView
{
	DECLARE_DYNCREATE(CScreenMain)

public:
	int						m_nTime;

	CDialogMessageBox		*m_pMsgBox;
	CDialogLotDisplay		*m_pLotDisplay;
	CDialogMessageView		*m_pMsgView;
	CDialogLotStart			*m_pLotStart;
	CDialogTesterInterface	*m_pTesterInterface;
	CDialogPcbTray			*m_pPcbTray;
	CDialogPcbPicker		*m_pPcbPicker;
	CDialogPcbBuffer		*m_pPcbBuffer;
	CDialogBarcode			*m_pBarcode;

public:
	CScreenMain();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CScreenMain();

public:
	enum { IDD = IDD_SCREEN_MAIN };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	CXPGroupBox	m_groupTimeInfo;
	CXPGroupBox	m_groupWorkInfo;
	CXPGroupBox m_groupLdTray;
	CXPGroupBox m_groupUnLdTrayFirst;
	CXPGroupBox m_groupUnLdTraySecond;
	CXPGroupBox m_groupRejectTrayFirst;
	CXPGroupBox m_groupRejectTraySecond;
	CXPGroupBox m_groupTrayRobot;
	CXPGroupBox m_groupLdBuf;
	CXPGroupBox m_groupUnLdBuf;
	CXPGroupBox m_groupTestSiteRobot;
	CXPGroupBox m_groupRetestBufFirst;
	CXPGroupBox m_groupRetestBufSecond;
	CXPGroupBox m_groupTestSiteTop;
	CXPGroupBox m_groupTestSiteBottom;

	CAnimate	m_Animate;

	CGridCtrl	m_pGridWorkInfoC;
	CGridCtrl	m_pGridWorkInfoN;
	CGridCtrl	m_pGridTimeInfo;
	CGridCtrl	m_pGridLdTray;
	CGridCtrl	m_pGridUnLdTrayFirst;
	CGridCtrl	m_pGridUnLdTraySecond;
	CGridCtrl	m_pGridRejectTrayFirst;
	CGridCtrl	m_pGridRejectTraySecond;
	CGridCtrl	m_pGridTrayRobot;
	CGridCtrl	m_pGridLdBuf;
	CGridCtrl	m_pGridUnLdBuf;
	CGridCtrl	m_pGridTestSiteRobot;
	CGridCtrl	m_pGridRetestBufFirst[2];
	CGridCtrl	m_pGridRetestBufSecond[2];
	CGridCtrl	m_pGridTestSiteTop[4];
	CGridCtrl	m_pGridTestSiteBottom[4];
	CGridCtrl	m_pGridRefernce;
	CGridCtrl	m_pGridYieldLot;
	CGridCtrl	m_pGridYieldDay;

	CImageList	*m_pImagelist;

	CBitmap		m_bmGround;

	CButtonST	m_btnUnLdOutputFirst;
	CButtonST	m_btnUnLdOutputSecond;
	CButtonST	m_btnRejectOutputFirst;
	CButtonST	m_btnRejectOutputSecond;
	CButtonST	m_btnLotDisplay;
	CButtonST	m_btnLdCvyLotInfo1;
	CButtonST	m_btnLdCvyLotInfo2;
	CButtonST	m_btnLotEnd;
	CButtonST m_btnDoorOpen;
	CButtonST m_btnDoorClose;

	CGradientStatic		m_msgRetestBufferFirst;;
	CGradientStatic		m_msgRetestBufferSecond;
	int					m_nLotYieldDisplayChange;
	int                 m_nSkipUse; //kwlee 2016.0912

	// jtkim 20150316
	void				OnMainCountDisplay();
	void				OnMainTimeDisplay();

	void				OnMainLdTray();
	void				OnMainUnLdTrayFirst();
	void				OnMainUnLdTraySecond();
	void				OnMainRejectTrayFirst();
	void				OnMainRejectTraySecond();
	void                OnMainWorkRobot();
	void				OnMainTrayRobot();
	void				OnMainLdBuf();
	void				OnMainUnLdBuf();
	void				OnMainTestSiteRobot();

	void				OnMainRetestBufFirst();
	void				OnMainRetestBufSecond();

	void				OnMainTestSiteTop();
	void				OnMainTestSiteBottom();

	void				OnMainYieldDay();
	void				OnMainYieldLot();
	void				OnInitButton();
	void				OnInitGroupBox();
	void				OnInitLabel();
	void				OnInitDigit();
	void				OnInitGridWorkInfoCurr();
	void				OnInitGridWorkInfoNext();
	void				OnInitGridLdTray();
	void				OnInitGridUnLdTrayFirst();
	void				OnInitGridUnLdTraySecond();
	void				OnInitGridRejectTrayFirst();
	void				OnInitGridRejectTraySecond();
	void				OnInitGridTrayRobot();
	void				OnInitGridLdBuf();
	void				OnInitGridUnLdBuf();
	void				OnInitGridTestSiteRobot();

	void				OnInitGridRetestBufFirst1();
	//void				OnInitGridRetestBufFirst2();
	void				OnInitGridRetestBufSecond1();
	//void				OnInitGridRetestBufSecond2();

	void				OnInitGridTestSiteTop();
	void				OnInitGridTestSiteBottom();
	void				OnInitGridRefernce();
	void				OnInitGridYieldDay();
	void				OnInitGridYieldLot();
	void				OnInitGridTimeInfo(int nMode);
	void				OnMainDisplay();

	//kwlee 2016.0901
	void                OnMainNextLotDataDisplay();
	void				OnInitGridNextLotYield();

	void                OnMainLotDisplay();
	void                OnPcbHistory(int nNum, tagPCB_INFO PcbOld, tagPCB_INFO PcbNew);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	virtual BOOL DestroyWindow();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg LRESULT OnMainWorkInfoDisplay(WPARAM wParam, LPARAM lParam);  // 화면 데이터 갱신용 사용자 메시지
	afx_msg LRESULT	OnMainWorkInfoCommand(WPARAM wParam, LPARAM lParam);  // 화면 데이터 갱신용 사용자 메시지
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual void PostNcDestroy();
	afx_msg void OnDestroy();
	afx_msg void OnClickedBtnMainAnimate();
	afx_msg void OnClickedBtnMainData();
	afx_msg void OnBnClickedButtonTrackOut();
	afx_msg void OnTimeCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnWorkInfoCurrCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);

	afx_msg void OnLdTrayCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnTrayRobotCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnUnLdTrayFirstCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnUnLdTraySecondCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnLdBufCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnUnLdBufCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnRejectTrayFirstCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnRejectTraySecondCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnRetestBufFirstCellClick1(NMHDR *pNotifyStruct, LRESULT* pResult);
	//afx_msg void OnRetestBufFirstCellClick2(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnRetestBufSecondCellClick1(NMHDR *pNotifyStruct, LRESULT* pResult);
	//afx_msg void OnRetestBufSecondCellClick2(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnTestSiteRobotCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);


	afx_msg void OnTestSite1CellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnTestSite2CellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnTestSite3CellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnTestSite4CellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnTestSite5CellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnTestSite6CellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnTestSite7CellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnTestSite8CellClick(NMHDR *pNotifyStruct, LRESULT* pResult);

	//kwlee 2016.0912
	afx_msg void OnLdTrayDCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnUnLdTrayFirstDCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnUnLdTraySecondDCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnLdBufDCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnUnLdBufDCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnRejectTrayFirstDCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnRejectTraySecondDCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnRetestBufFirstDCellClick1(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnRetestBufFirstDCellClick2(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnRetestBufSecondDCellClick1(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnRetestBufSecondDCellClick2(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnTestSiteRobotDCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);

	afx_msg void OnTestSite1DCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnTestSite2DCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnTestSite3DCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnTestSite4DCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnTestSite5DCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnTestSite6DCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnTestSite7DCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnTestSite8DCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	////

	afx_msg void OnTestSite1CellRClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnTestSite2CellRClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnTestSite3CellRClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnTestSite4CellRClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnTestSite5CellRClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnTestSite6CellRClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnTestSite7CellRClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnTestSite8CellRClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnYieldCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);

	afx_msg void OnBnClickedBtnUnldTrayOutputFirst();
	afx_msg void OnBnClickedBtnUnldTrayOutputSecond();
	afx_msg void OnBnClickedBtnRejectTrayOutputFirst();
	afx_msg void OnBnClickedBtnRejectTrayOutputSecond();
	afx_msg void OnBnClickedBtnLotDisplay();
	afx_msg void OnBnClickedBtnLdCvyLotInfo1();
	afx_msg void OnBnClickedBtnLdCvyLotInfo2();
	afx_msg void OnBnClickedBtnLotEnd();
	afx_msg void OnBnClickedBtnDoorOpen();
	afx_msg void OnBnClickedBtnDoorClose();
};


