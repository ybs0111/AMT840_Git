#pragma once


// CDialogBarcode 대화 상자입니다.
#include "Variable.h"
#include "xShadeButton.h"	// 조그 버튼 생성 클래스 추가
#include "XPGroupBox.h"
#include "GradientStatic.h"  
#include "Digit.h"
#include "BtnST.h"


class CDialogBarcode : public CDialog
{
	DECLARE_DYNAMIC(CDialogBarcode)

public:
	int					m_nWorkSite;

	CGradientStatic		m_msgBarcodeWorkSite;
	CGradientStatic		m_msgBarcodeIbm;
	CGradientStatic		m_msgBarcodeIbmData;
	CGradientStatic		m_msgBarcodeVendor;
	CGradientStatic		m_msgBarcodeVendorData;

	CButtonST			m_btnBarcodeExit;
	CButtonST			m_btnBarcodeRetry;
	
	BOOL				Create();
	void				OnInitLabel();
	void				OnInitButton();

public:
	CDialogBarcode(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogBarcode();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_BARCODE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL DestroyWindow();
	virtual BOOL OnInitDialog();
	afx_msg void OnClickedBtnBarcodeRetry();
	afx_msg void OnClickedBtnBarcodeExit();
	afx_msg void OnStnClickedMsgBarcodeIbmData();
	afx_msg void OnStnClickedMsgBarcodeVendorData();
};
