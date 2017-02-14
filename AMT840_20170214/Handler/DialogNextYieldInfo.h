#pragma once

#include "XPGroupBox.h"
#include "xShadeButton.h"	// ���� ��ư ���� Ŭ���� �߰�
#include ".\\Union\\Grid\\GridCtrl.h"
#include ".\\Union\\Grid\\GridURLCell.h"
#include ".\\Union\\Grid\\GridCellCombo.h"
#include ".\\Union\\Grid\\GridCellCheck.h"
#include ".\\Union\\Grid\\GridCellNumeric.h"
#include ".\\Union\\Grid\\GridCellDateTime.h"
#include "Digit.h"
#include "BtnST.h"
#include "GradientStatic.h"
#include "MyBasicData.h"

// CDialogNextYieldInfo ��ȭ �����Դϴ�.

class CDialogNextYieldInfo : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogNextYieldInfo)

public:
	CDialogNextYieldInfo(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDialogNextYieldInfo();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_NEXT_LOTYIELD };

public:
	void InitGridNextLotYield();
	void InitGroupBox();
	void InitBtn();
	void InitData();

	CGridCtrl m_pGridNextYield;
	CXPGroupBox m_groupNextYield;
	CButtonST m_btnExit;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnExit();
};
