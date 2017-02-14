#pragma once


// CDialogBarcode ��ȭ �����Դϴ�.
#include "Variable.h"
#include "xShadeButton.h"	// ���� ��ư ���� Ŭ���� �߰�
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
	CDialogBarcode(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDialogBarcode();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_BARCODE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
