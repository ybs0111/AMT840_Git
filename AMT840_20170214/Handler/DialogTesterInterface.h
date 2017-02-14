#pragma once

#include ".\\Union\\Grid\\GridCtrl.h"
#include ".\\Union\\Grid\\GridURLCell.h"
#include ".\\Union\\Grid\\GridCellCombo.h"
#include ".\\Union\\Grid\\GridCellCheck.h"
#include ".\\Union\\Grid\\GridCellNumeric.h"
#include ".\\Union\\Grid\\GridCellDateTime.h"
#include "BtnST.h"
#include "GradientStatic.h"  
// CDialogPcbInterface ��ȭ �����Դϴ�.

class CDialogTesterInterface : public CDialog
{
	DECLARE_DYNAMIC(CDialogTesterInterface)

public:
	int					m_nSite;
	int					m_nRSite;
	int                 m_nCnt;
	int                 m_nNum;

	CButtonST			m_btnExit;

	CGridCtrl			m_pGridItemData;
	CGridCtrl			m_pGridBottomItemData;

	CGradientStatic		m_msgTitle;

	CBitmap				m_bmGround;

	BOOL				Create();
	void				OnInitButton();
	void				OnInitGroupBox();
	void				OnInitLabel();
	void				OnInitDigit();
	void				OnInitGridItemData();
	void				OnInitGridBottomItemData();
	void				OnDisplayData();

public:
	CDialogTesterInterface(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDialogTesterInterface();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_TESTER_INTERFACE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedBtnPcbDialogExit();
};
