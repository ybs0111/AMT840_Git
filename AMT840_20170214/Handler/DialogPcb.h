#pragma once

#include "Variable.h"
#include "XPGroupBox.h"
#include "xShadeButton.h"	// 조그 버튼 생성 클래스 추가
#include "GradientStatic.h"  // 그라데이션 라벨 클래스
#include "BtnST.h"
#include "Digit.h"
#include ".\\Union\\Grid\\GridCtrl.h"
#include ".\\Union\\Grid\\GridURLCell.h"
#include ".\\Union\\Grid\\GridCellCombo.h"
#include ".\\Union\\Grid\\GridCellCheck.h"
#include ".\\Union\\Grid\\GridCellNumeric.h"
#include ".\\Union\\Grid\\GridCellDateTime.h"

// CDialogPcb 대화 상자입니다.

class CDialogPcb : public CDialog
{
	DECLARE_DYNAMIC(CDialogPcb)

public:
	CDialogPcb(CWnd* pParent = NULL);   // 표준 생성자입니다.
	CDialogPcb(tagPCB_INFO &st_pcb_info);
	virtual ~CDialogPcb();

// 대화 상자 데이터입니다.
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
