#pragma once

#include "Variable.h"
#include "xShadeButton.h"	// 조그 버튼 생성 클래스 추가
#include "XPGroupBox.h"
#include ".\\Union\\Grid\\GridCtrl.h"
#include ".\\Union\\Grid\\GridURLCell.h"
#include ".\\Union\\Grid\\GridCellCombo.h"
#include ".\\Union\\Grid\\GridCellCheck.h"
#include ".\\Union\\Grid\\GridCellNumeric.h"
#include ".\\Union\\Grid\\GridCellDateTime.h"
#include "GradientStatic.h"  
#include "Digit.h"
#include "BtnST.h"
// CDialogLotStart 대화 상자입니다.

class CDialogLotStart : public CDialog
{
	DECLARE_DYNAMIC(CDialogLotStart)

public:
	DWORD				m_dwWaitTime[3];

	CString				m_strLotNo;
	CString				m_strPartNo;
	// jtkim 20160929
	CString				m_strOpCode;
	CString				m_strTmpLotNo;
	//2017.0105
	CString				m_strFabSite;

	int					m_nLdCvySite;
	int					m_nLotDisplayStep;
	int					m_nLotStartStep;
	int					m_nLotCancelStep;
	int					m_nRfidReadStep;

	int					OnLotDisplay();
	int					OnLotStart();
	int					OnLotCancel();
	int					OnRfidRead();
	
	CGridCtrl			m_pGridLotInfo;

	CButtonST			m_btnExit;
	CButtonST			m_btnLotStart;
	CButtonST			m_btnLotCancel;
	CButtonST			m_btnRfidRead;
	CButtonST			m_btnLotDisplay;
	CButtonST			m_btnCurrLot;
	CButtonST			m_btnNextLot;

	CBitmap				m_bmGround;

	BOOL				Create();
	void				OnInitGridLotInfo();
	void				OnInitButton();
	void                OnDataApply(); //kwlee 2016.1128

public:
	CDialogLotStart(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogLotStart();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_LOT_START };


public:
	CString			  OnRecipeExistCheck(CString strFileNo);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.



	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL DestroyWindow();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnExit();
	afx_msg void OnBnClickedBtnLotCancel();
	afx_msg void OnBnClickedBtnLotStart();
	afx_msg void OnBnClickedBtnRfidRead();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnLotDisplay1();
	afx_msg void OnLotCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnBnClickedBtnCurrLot();
	afx_msg void OnBnClickedBtnNextLot();
};
