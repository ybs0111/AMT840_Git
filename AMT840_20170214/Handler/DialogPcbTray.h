#pragma once

#include ".\\Union\\Grid\\GridCtrl.h"
#include ".\\Union\\Grid\\GridURLCell.h"
#include ".\\Union\\Grid\\GridCellCombo.h"
#include ".\\Union\\Grid\\GridCellCheck.h"
#include ".\\Union\\Grid\\GridCellNumeric.h"
#include ".\\Union\\Grid\\GridCellDateTime.h"
#include "BtnST.h"
#include "GradientStatic.h"  
// CDialogPcbTray ��ȭ �����Դϴ�.

class CDialogPcbTray : public CDialog
{
	DECLARE_DYNAMIC(CDialogPcbTray)

public:
	int					m_nSite;
	int					m_nTrayNum;

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
	CDialogPcbTray(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDialogPcbTray();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_PCB_TRAY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedBtnPcbDialogExit();
};
