#pragma once

#include "Variable.h"
#include "XPGroupBox.h"
#include "xShadeButton.h"	// ���� ��ư ���� Ŭ���� �߰�
#include "GradientStatic.h"  // �׶��̼� �� Ŭ����
#include "BtnST.h"
#include "Digit.h"
#include ".\\Union\\Grid\\GridCtrl.h"
#include ".\\Union\\Grid\\GridURLCell.h"
#include ".\\Union\\Grid\\GridCellCombo.h"
#include ".\\Union\\Grid\\GridCellCheck.h"
#include ".\\Union\\Grid\\GridCellNumeric.h"
#include ".\\Union\\Grid\\GridCellDateTime.h"

// CDialogPcb ��ȭ �����Դϴ�.

class CDialogPcb : public CDialog
{
	DECLARE_DYNAMIC(CDialogPcb)

public:
	CDialogPcb(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	CDialogPcb(tagPCB_INFO &st_pcb_info);
	virtual ~CDialogPcb();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_PCB };

public:	
	CGradientStatic	m_msgMsg;

	CXPGroupBox		m_groupCommand;

	CButtonST		m_btnStart;
	CButtonST		m_btnAbort;
	CButtonST		m_btnPass;
	CButtonST		m_btnFail;
	CButtonST		m_btnApply;
	CButtonST		m_btnCancel;

	int				m_nX;
	int				m_nY;

	int				m_nMode;

	tagPCB_INFO		*m_pPcb_info;
	tagPCB_INFO		m_pcb_info;

	CGridCtrl		m_pGridPcb;

public:
	void			OnPcbDataDisplay();
	void			OnInitGridPcb();
	void			OnInitButton();
	void			OnInitGroupBox();
	void			OnInitLabel();
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPcbCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnPcbDialogStart();
	afx_msg void OnBnClickedBtnPcbDialogAbort();
	afx_msg void OnBnClickedBtnPcbDialogPass();
	afx_msg void OnBnClickedBtnPcbDialogFail();
	afx_msg void OnBnClickedBtnPcbDialogApply();
	afx_msg void OnBnClickedBtnPcbDialogCancel();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
