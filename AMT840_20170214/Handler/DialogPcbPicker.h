#pragma once

#include ".\\Union\\Grid\\GridCtrl.h"
#include ".\\Union\\Grid\\GridURLCell.h"
#include ".\\Union\\Grid\\GridCellCombo.h"
#include ".\\Union\\Grid\\GridCellCheck.h"
#include ".\\Union\\Grid\\GridCellNumeric.h"
#include ".\\Union\\Grid\\GridCellDateTime.h"
#include "BtnST.h"
#include "GradientStatic.h"  

class CDialogPcbPicker : public CDialog
{
	DECLARE_DYNAMIC(CDialogPcbPicker)

public:
	int					m_nSite;

	CButtonST			m_btnExit;

	CGridCtrl			m_pGridItemData;

	CGradientStatic		m_msgTitle;

	CBitmap				m_bmGround;

	BOOL				Create();
	void				OnInitButton();
	void				OnInitGroupBox();
	void				OnInitLabel();
	void				OnInitDigit();
	void				OnInitGridItem();
	void				OnInitGridItemData();

public:
	CDialogPcbPicker(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDialogPcbPicker();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_PCB_PICKER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedBtnPcbDialogExit();
};
