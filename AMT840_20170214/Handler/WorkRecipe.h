#pragma once


// CWorkRecipe 대화 상자입니다.
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



class CDialogSocketComment;		//SOCKET COMMENT 2015.08.04
class CWorkRecipe : public CDialog
{
	DECLARE_DYNAMIC(CWorkRecipe)

public:
	CString				m_strLogKey[10];
	CString				m_strLogData[10];

	int					m_nSocketCount[2];
	int					m_nTsiteUse[2][8];
	int					m_nTsiteSocketUse[2][8][12];
	
	int					m_nRetry;
	int					m_nTrayYCnt[2];
	int					m_nTrayXCnt[2];
	int					m_nRejectTrayXCnt[2];
	int					m_nRetestTrayXCnt[2];
	int					m_nDegree[3][2];
	
	int					m_nRetCnt[2];
	int					m_nFailCnt[2];
	
	//kwlee 2016.0823
	int					m_nRetestBufferNum[2];
	int					m_nLdUldBufferNum[2];

	double              m_dLdUldPickerPitch[2];
	double              m_dLdUldBufferPitch[2];
	double              m_dTestPickerPitch[2];
	double              m_dTrayPitchX[2];
	double              m_dTrayPitchY[2];
	double              m_dRetestBufferPitch[2];
	//////

	double				m_dTrayYOffset[2];
	double				m_dTrayXOffset[2];
	double				m_dTSiteOffset[2];
	double				m_dBufXOffset[2];
	double				m_dBufYOffset[2];
	double				m_dCokXOffset[2];
	double				m_dRejectFullOffset[2];
	double				m_dRejectEmptyOffset[2];
	double				m_d15TOffset[2];

	double              m_dVisionXOffset[2];
	double              m_dVisionYOffset[2];


	CButtonST			m_btnApply;
	CButtonST			m_btnReLoad;
//	CButtonST			m_btnBufSenUse;

	CGradientStatic		m_msgTrayRowCnt;
	CGradientStatic		m_msgTrayColCnt;
	CGradientStatic		m_msgRejectTrayXCnt;
	CGradientStatic		m_msgRetestTrayXCnt;
	CGradientStatic		m_msgTrayRowOffset;
	CGradientStatic		m_msgTrayColOffset;
	CGradientStatic		m_msgBufRowOffset;
	CGradientStatic		m_msgBufColOffset;
	CGradientStatic		m_msgTSiteOffset;
	CGradientStatic		m_msgCokXOffset;
	CGradientStatic		m_msgRetCnt;
	CGradientStatic		m_msgFailCnt;
	CGradientStatic		m_msgRejectFullOffset;
	CGradientStatic		m_msgRejectEmptyOffset;
	CGradientStatic		m_msgSocketCount;
	CGradientStatic		m_msg15TOffset;
	
	CGradientStatic		m_msgRetestBufferCnt;
	CGradientStatic		m_msgWorkBufferCnt;
	CGradientStatic		m_msgWorkPickerPitch;
	CGradientStatic		m_msgTestPickerPitch;
	CGradientStatic		m_msgWorkBufferPitch;
	CGradientStatic		m_msgRetestBufferPitch;
	CGradientStatic		m_msgTrayPitchX;
	CGradientStatic		m_msgTrayPitchY;

	//kwlee 2016.0901
	CGradientStatic		m_msgVisionXOffset;
	CGradientStatic		m_msgVisionYOffset;
	CDigit				m_dgtVisionXOffset;
	CDigit				m_dgtVisionYOffset;

	CDigit				m_dgtTrayRowCnt;
	CDigit				m_dgtTrayColCnt;
	CDigit				m_dgtRejectTrayXCnt;
	CDigit				m_dgtRetestTrayXCnt;
	CDigit				m_dgtTrayRowOffset;
	CDigit				m_dgtTrayColOffset;
	CDigit				m_dgtBufRowOffset;
	CDigit				m_dgtBufColOffset;
	CDigit				m_dgtTSiteOffset;
	CDigit				m_dgtCokXOffset;
	CDigit				m_dgtRetCnt;
	CDigit				m_dgtFailCnt;
	CDigit				m_dgtRejectFullOffset;
	CDigit				m_dgtRejectEmptyOffset;
	CDigit				m_dgtSocketCount;
	CDigit				m_dgt15TOffset;

	CDigit				m_dgtRetestBuffCnt;
	CDigit				m_dgtWorkBufferCnt;
	CDigit				m_dgtWorkPickerPitch;
	CDigit				m_dgtTestPickerPitch;
	CDigit				m_dgtWorkBuffPitch;
	CDigit				m_dgtRetestBuffPitch;
	CDigit				m_dgtTrayPitchX;
	CDigit				m_dgtTrayPitchY;

	CXPGroupBox			m_groupTrayInfo;
	CXPGroupBox			m_groupRotator;
//	CXPGroupBox			m_groupSocket;
	CXPGroupBox m_groupTsiteSocketInfo;
	
	
	CGridCtrl			m_pGridLfInterface;
	CGridCtrl			m_pGridRiInterface;
	CGridCtrl			m_pGridDegree;
	CGridCtrl			m_pGridInterface;

	void				OnInitGridLeftInterface();
	void				OnInitGridRightInterface();
	void                OnInitGridInterface(); //kwlee 2016.0729
	void				OnInitGridDegree();
	void				OnInitGroupBox();
	void				OnInitLabel();
	void				OnInitButton();
	void				OnInitDigit();
	void				OnDataHistoryLog();
	void				OnDataRecovery();
	int					OnDataComparison();
	void				OnDataApply();
	void				OnDataBackup();
	void				OnDataInit();
	void				OnDataDisplay();

public:
	CWorkRecipe(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CWorkRecipe();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_WORK_RECIPE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL DestroyWindow();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedBtnApply();
	afx_msg void OnBnClickedBtnReload();
	virtual void PostNcDestroy();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClickedDgtTrayRowCnt();
	afx_msg void OnClickedDgtTrayColCnt();
	afx_msg void OnClickedDgtTrayRowOffset();
	afx_msg void OnClickedDgtTrayColOffset();
	afx_msg void OnClickedDgtTestsiteOffset();
	//afx_msg void OnLeftInterfaceCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	//afx_msg void OnRightInterfaceCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnInterfaceCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnDegreeCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnStnClickedDgtRetestCount();
	afx_msg void OnStnClickedDgtFailCount();
	afx_msg void OnClickedDgtBufferRowOffset();
	afx_msg void OnClickedDgtBufferColOffset();
	afx_msg void OnClickedDgtCokXOffset();
	afx_msg void OnStnClickedDgtRejectTrayXCnt();
	afx_msg void OnStnClickedDgtRetestTrayXCnt();
	afx_msg void OnStnClickedDgtRejectFullOffset();
	afx_msg void OnStnClickedDgtRejectEmptyOffset();
//	afx_msg void OnStnClickedDgtCokXOffset();
	afx_msg void OnStnClickedDgtCokXOffset();
	afx_msg void OnStnClickedDgtSocketCount();
	afx_msg void OnStnClickedDgt15tOffset();
	afx_msg void OnStnClickedDgtRetestBuffCnt();
	afx_msg void OnStnClickedDgtWorkBuffCnt();
	afx_msg void OnStnClickedDgtWorkPickerPitch();
	afx_msg void OnStnClickedDgtTestPickerPitch();
	afx_msg void OnStnClickedDgtWorkBuffPitch();
	afx_msg void OnStnClickedDgtRetestBuffPitch();
	afx_msg void OnStnClickedDgtTrayPitchX();
	afx_msg void OnStnClickedDgtTrayPitchY();
	CXPGroupBox m_group_Offset_Info;
	afx_msg void OnStnClickedDgtVisionYOffset();
	afx_msg void OnStnClickedDgtVisionXOffset();
};
