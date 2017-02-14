#pragma once


// CDialogLotDisplay 대화 상자입니다.
#include "Variable.h"
#include "xShadeButton.h"	// 조그 버튼 생성 클래스 추가
#include "XPGroupBox.h"
#include ".\\Union\\Grid\\GridCtrl.h"
#include ".\\Union\\Grid\\GridURLCell.h"
#include ".\\Union\\Grid\\GridCellCombo.h"
#include ".\\Union\\Grid\\GridCellCheck.h"
#include ".\\Union\\Grid\\GridCellNumeric.h"
#include ".\\Union\\Grid\\GridCellDateTime.h"
#include "GradientStatic.h"  
#include "Digit.h"
#include "BtnST.h"
#include <afxtempl.h>

class CDialogLotDisplay : public CDialog
{
	DECLARE_DYNAMIC(CDialogLotDisplay)
public:
	BOOL				Create();

	int					OnLotCancelReq();
	int					OnLotStatusReq();
	int					OnDelScrapReq();
	int					OnPcbInfoReq();
	int					OnTrackOutReq();
	int					OnOnLineEndReq();
	int					OnScrapReq();
	int					OnTestEndReq();
	int					OnLotEnd(); // jtkim lot end
	void				OnInitGridLotInfo();
	void				OnInitGridPcbInfo();
	void				OnInitGroupBox();
	void				OnInitLabel();
	void				OnInitButton();
	void				OnLotDataDisplay();
	void				OnMessageDisplay(int nBin, CString strMsg);
	// jtkim lot end
	CString				m_strRfidData[20]; // jtkim lot end

	int					m_nLotEndStep; // jtkim lot end

	DWORD				m_dwWaitTime[3];

	CString				m_strPcbBarcode;
	CString				m_strPcbBarcodeData;
	CString				m_strScrapCodeData;
	CString				m_strArraySnData;
	CString				m_strLotNo;
	CString				m_strPartNo;
	CString				m_strLotState;
	CString				m_strLotType;
	CString				m_strMainState;
	CString				m_strProCid;
	CString				m_strStep;
	CString				m_strComplot;
	CString				m_strLine;
	CString				m_strScrapCode[1000];
	CString				m_strAddScrap[1000];
	CString				m_strArray[1000];
	CString				m_strScrapSerial[1000];

	int					m_nTestEndBin;
	int					m_nDelScrapNum;

	CStringArray		m_ArScrapCode;
	CStringArray		m_ArScrapSerial;
	CStringArray		m_ArArraySn;

	int					m_nLotCancelStep;
	int					m_nLotStatusStep;
	int					m_nAddScrap;
	int					m_nXout;
	int					m_nLotQty;
	int					m_nPcbQty;
	int					m_nPcbArray;
	int					m_nScrapBin;
	int					m_nOnLineBin;
	int					m_nScrapCount;
	int					m_nOnLineStep;
	int					m_nScrapStep;
	int					m_nTrackOutStep;
	int					m_nPcbInfoStep;
	int					m_nDelStep;
	int					m_nTestEndStep;
	int					m_nRetry;

	CXPGroupBox			m_groupLotDisplay;

	CGradientStatic		m_msgPcbBarcode;
	CGradientStatic		m_msgPcbBarcodeData;
	CGradientStatic		m_msgScrapCode;
	CGradientStatic		m_msgScrapCodeData;
	CGradientStatic		m_msgPcbCnt;
	CGradientStatic		m_msgPcbCntData;
	CGradientStatic		m_msgArraySn;
	CGradientStatic		m_msgArraySnData;

	CGridCtrl			m_pGridPcbInfo;
	CGridCtrl			m_pGridLotInfo;

	CButtonST			m_btnExit;
	CButtonST			m_btnBadInform;
	CButtonST			m_btnLotEnd;
	CButtonST			m_btnAdd;
	CButtonST			m_btnDel;
	CButtonST			m_btnScrapInform;
	CButtonST			m_btnOnLineInform;
	CButtonST			m_btnLotClear;
	CButtonST			m_btnLotCancel;

	CBitmap				m_bmGround;
public:
	CDialogLotDisplay(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogLotDisplay();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_LOT_DISPLAY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnClickedBtnExit();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnPcbInform();
	afx_msg void OnBnClickedBtnTrackOut();
	afx_msg void OnBnClickedBtnDialogLotDisplayAdd();
	afx_msg void OnBnClickedBtnDialogLotDisplayDelete();
	afx_msg void OnStnClickedMsgDialogLotDisplayBarcodeData();
	afx_msg void OnStnClickedMsgDialogLotDisplayScrapData();
	afx_msg void OnScarpCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnLotCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnBnClickedBtnDialogLotDisplayScrap();
	afx_msg void OnBnClickedBtnDialogLotDisplayOnline();
	afx_msg void OnStnClickedMsgDialogLotDisplayPcbCntData();
	virtual BOOL DestroyWindow();
	afx_msg void OnBnClickedBtnDialogLotDisplayClear();
	afx_msg void OnStnClickedMsgDialogLotDisplayArraySnData();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnDialogLotDisplayEnd();
	afx_msg void OnBnClickedBtnDialogLotDisplayCancel1();
	afx_msg void OnBnClickedBtnDialogLotDisplayCancel();
	CButtonST m_btnLotDataClear;
	afx_msg void OnBnClickedBtnDialogLotDataClear();
};
