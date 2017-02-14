#pragma once


// CSlotInfo 대화 상자입니다.
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


class CSlotInfo : public CDialogEx
{
	DECLARE_DYNAMIC(CSlotInfo)

public:
	CSlotInfo(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSlotInfo();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_SLOT_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	
	void     OnInitSlotInfo();
	void     OnInitSiteInfo();
	void     OnInitSiteSel();
	void     OnInitGroupBox();
	void     OnInitBtn();
	
	CString  m_strLotNo;
	CString  m_strPartNo;
	CString  m_strNextLotNo;
	CString  m_strCurrLotNo;
	CString  m_str1D[2];
	CString  m_str2D;
	CString  m_strAmount;
	CString  m_strTestTime;
	int		 m_nRetestCnt;
	CString  m_strCurrPos;

	CGridCtrl m_Grid_SiteInfo;
	CGridCtrl m_GridSlotInfo;

	CXPGroupBox m_group_SlotInfo;

	CButtonST m_Btn_Slot_1;
	CButtonST m_Btn_Slot_2;
	CButtonST m_Btn_Slot_3;
	CButtonST m_Btn_Slot_4;
	CButtonST m_Btn_Slot_5;
	CButtonST m_Btn_Slot_6;
	CButtonST m_Btn_Slot_7;
	CButtonST m_Btn_Slot_8;
	
	CGridCtrl m_Grid_SiteSel;
	CXPGroupBox m_group_Site_Sel;
	CXPGroupBox m_group_Site_Info;

	afx_msg void OnBnClickedBtnSlot1();
	afx_msg void OnBnClickedBtnSlot2();
	afx_msg void OnBnClickedBtnSlot3();
	afx_msg void OnBnClickedBtnSlot4();
	afx_msg void OnBnClickedBtnSlot5();
	afx_msg void OnBnClickedBtnSlot6();
	afx_msg void OnBnClickedBtnSlot7();
	afx_msg void OnBnClickedBtnSlot8();
	afx_msg void OnCellClickSiteSel(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnCellClickSiteInfo(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnCellClickSlotInfo(NMHDR *pNotifyStruct, LRESULT* pResult);
	
	afx_msg void OnBnClickedBtnUpdate();
};
