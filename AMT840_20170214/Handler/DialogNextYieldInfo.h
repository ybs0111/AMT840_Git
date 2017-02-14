#pragma once

#include "XPGroupBox.h"
#include "xShadeButton.h"	// 조그 버튼 생성 클래스 추가
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

// CDialogNextYieldInfo 대화 상자입니다.

class CDialogNextYieldInfo : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogNextYieldInfo)

public:
	CDialogNextYieldInfo(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogNextYieldInfo();

// 대화 상자 데이터입니다.
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnExit();
};
