#pragma once

#include ".\\Union\\Grid\\GridCtrl.h"
#include ".\\Union\\Grid\\GridURLCell.h"
#include ".\\Union\\Grid\\GridCellCombo.h"
#include ".\\Union\\Grid\\GridCellCheck.h"
#include ".\\Union\\Grid\\GridCellNumeric.h"
#include ".\\Union\\Grid\\GridCellDateTime.h"
#include "BtnST.h"
#include "GradientStatic.h"  

class CDialogPcbBuffer : public CDialog
{
	DECLARE_DYNAMIC(CDialogPcbBuffer)

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
	CDialogPcbBuffer(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDialogPcbBuffer();

	// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_PCB_BUFFER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedBtnPcbDialogExit();
};
