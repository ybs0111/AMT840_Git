#pragma once
#include "afxwin.h"
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
#include ".\\Ctrl\\KeyBoard.h"


// CWorkVisionTeach 대화 상자입니다.
#define TM_VISION_FCOUS		7874
#define	TM_VISION_TEACH		7875
#define	TM_VISION_DISPLAY	7876
#define	TM_VISION_POS_SET	7877
class CWorkVisionTeach : public CDialogEx
{
	DECLARE_DYNAMIC(CWorkVisionTeach)

public:
	CWorkVisionTeach(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CWorkVisionTeach();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_WORK_TEACH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	void OnInitLabel();
	void OnInitBtn();
	void OnInitDgt();
	
	void OnInitDataLoad();
	void OnInitTestSitePos();

	void OnInitRightOffsetGrid();
	
	//////////////
	void OnInitLeftMarkTeachGrid();
	void OnInitRightMarkTeachGrid();

	void OnInitLeftVisionTeachGrid();
	void OnInitRightVisionTeachGrid();


	void OnInitLeftVisionTeachValue();
	void OnInitRightVisionTeachValue();

	void OnInitLeftReadGrid();
	void OnInitRightReadGrid();
	////////////

	void OnMainLeftVisionTeach();
	void OnMainRightVisionTeach();

	void OnMaintLeftRead();
	void OnMainRightRead();

	void OnMainLeftMarkTeach();
	void OnMainRightMarkTeach();

	void OnMainVisionLeftTeachValue();
	void OnMainVisionRightTeachValue();

	void OnInitGroup();
	int	VisionFocus_Move( int nSite );
	int	VisionTeach_Move();
	int ValueSetMove();

	//kwlee 2017.0111
	void VisionMarkSave();
	void VisionTeachSave();
	void OnVisionReadSave();
	void VisionRefSave();
	void OnVisionOffsetValSave();
	void OnVisionDataApply();
	int OnVisionOffsetCheck();
	
	
public:
	CListBox m_listVisionResponse;
	CGradientStatic m_msg_TsiteRbt_Site;
	CGradientStatic m_msg_TsiteRbt_Mark;

	CGradientStatic m_msg_RightTsiteRbt_Site;
	CGradientStatic m_msg_RightTsiteRbt_Mark;

	CDigit m_dgt_TsiteRbt_Site;
	CDigit m_dgt_TsiteRbt_Mark;

	CDigit m_dgt_RightTsiteRbt_Site;
	CDigit m_dgt_RightTsiteRbt_Mark;
	
	CXPGroupBox m_group_Vision_Pos;

	CGridCtrl m_pGridLeftOffset;
	CGridCtrl m_pGridRightOffset;

	CGridCtrl m_pGridLeftRead;
	CGridCtrl m_GridTeachPos;
	CGridCtrl m_pGridTestSitePos;
	CGridCtrl m_pGridTsiteLeftOffset;
	CGridCtrl m_pGridTsiteRightOffset;
	CGridCtrl m_pGridLeftVisionTeach;
	CGridCtrl m_pGridRightVisionTeach;


	CButtonST m_btnDataSave;
	CButtonST m_btnDataLoad;
	CButton m_btnAutoTeachUse;
	CGridCtrl m_pGridLeftTeach;
	CGridCtrl m_pGridRightTeach;

	CGridCtrl m_pGridLeftValue;
	CGridCtrl m_pGridRightValue;
	///////////////////////////////////
	//int			m_nIO_No; //사이트 번호.
	int			m_nSite_No; //사이트 번호.
	int			m_nVisFocusStep;
	int			m_nVisTeachStep;
	int			m_npPicker_YesNo[4];
	bool		m_bVisWaitChk_Falg[8];
	DWORD		m_dwWaitTimeVis[8][3];
	DWORD		m_dwWaitTime[3];
	double		m_dTempFcsZ;
	double		m_dFocusZ;
	double      m_dwXOffset;
	double      m_dwYOffset;
	int			m_nRtyCount;
	int			m_iVisionPos; //마크번호.
	int         m_nSimul;
	int         m_nValueSetStep;
	int         m_nAutoTeachModeUse;
	///////////////////////////////////

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnLeftTsiteRbtFocusGo();
	virtual BOOL OnInitDialog();
	afx_msg void OnStnClickedDgtLeftTsiteRbtSite();
	afx_msg void OnBnClickedBtnLeftTsiteRbtTeachGo();
	afx_msg void OnStnClickedDgtTsiteRbtMark();
	
	afx_msg void OnBnClickedBtnVisionShow();
	afx_msg void OnBnClickedBtnVisionHide();
	afx_msg void OnBnClickedBtnOffset();
	afx_msg void OnBnClickedBtnEdge();
	afx_msg void OnBnClickedBtnLoad();
	afx_msg void OnBnClickedBtnSave();

	afx_msg void OnMarkLeftTeachCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnMarkRightTeachCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnBnClickedBtnLeftTsiteRbtTeachGo2();
	afx_msg void OnBnClickedBtnTestSiteAddOffset();
	afx_msg void OnBnClickedBtnTsiteRbtSetGo();
	
	afx_msg void OnBnClickedBtnTestSiteValueSet();
};
