#pragma once


// CDialogHifix 대화 상자입니다.
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
	CDialogHifix(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogHifix();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_HIFIX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL OnInitDialog();
	afx_msg void OnClickedBtnHifixExit();
	afx_msg void OnCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
};
