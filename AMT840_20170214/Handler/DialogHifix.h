#pragma once


// CDialogHifix ��ȭ �����Դϴ�.
#include ".\\Union\\Grid\\GridCtrl.h"
#include ".\\Union\\Grid\\GridURLCell.h"
#include ".\\Union\\Grid\\GridCellCombo.h"
#include ".\\Union\\Grid\\GridCellCheck.h"
#include ".\\Union\\Grid\\GridCellNumeric.h"
#include ".\\Union\\Grid\\GridCellDateTime.h"
#include "GradientStatic.h"  
#include "Digit.h"
#include "BtnST.h"

class CDialogHifix : public CDialog
{
	DECLARE_DYNAMIC(CDialogHifix)
public:
	int					m_nHifix;

	CString				m_strHifix[50];
	CString				m_strHifixData;

	CGridCtrl			m_pGridHifix;

	CButtonST			m_btnExit;

	void				OnInitGrid();
	void				OnInitButton();
	void				OnInitHifix();

	CBitmap				m_bmGround;
public:
	CDialogHifix(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDialogHifix();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_HIFIX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL OnInitDialog();
	afx_msg void OnClickedBtnHifixExit();
	afx_msg void OnCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
};
