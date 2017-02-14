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
#include "Variable.h"
#include "GradientStatic.h"
// CDialogManualIO 대화 상자입니다.

class CDialogManualIO : public CDialog
{
	DECLARE_DYNAMIC(CDialogManualIO)

public:
	CButtonST		m_btnExit;
	CButtonST		m_btnDoor;
	CButtonST		m_btnLeftMpClamp;
	CButtonST		m_btnLeftMpUnClamp;
	CButtonST		m_btnRightMpClamp;
	CButtonST		m_btnRightMpUnClamp;

	CGridCtrl		m_pGridTsite;
	CGridCtrl		m_pGridTRbt;
	CGridCtrl		m_pGridLdRbt;
	CGridCtrl		m_pGridUldRbt;
	CGridCtrl		m_pGridLfContact;
	CGridCtrl		m_pGridRiContact;
	CGridCtrl		m_pGridBuf;

	int				m_nTsiteNum;

	CBitmap			m_bmGround;

	DWORD			m_dwWaitTime[3];



	//kwlee 2016.0818
	//Ld Conveyor out
	int m_nReadyLd1CvyStkStopUpDnCylOut;		// S0000
	int m_nReadyLd2CvyStkStopUpDnCylOut;		// S0001
	
	//Ld Conveyor In
	int m_nReadyLd1CvyStopUpChkIn;			//PS0003
	int m_nReadyLd1CvyStopDnChkIn;			//PS0004
	int m_nReadyLd2CvyStopUpChkIn;			//PS0007
	int m_nReadyLd2CvyStopDnChkIn;			//PS0008

	//Uld Conveyor out
	int m_nReadyUld1CvyStopCylOut;				// S0202
	int m_nReadyUld2CvyStopCylOut;				// S0203

	//Uld Conveyor In
	int m_nReadyUld1CvyStopUpChkIn;			//PS0205
	int m_nReadyUld1CvyStopDnChkIn;			//PS0206
	int m_nReadyUld2CvyStopUpChkIn;			//PS0209
	int m_nReadyUld2CvyStopDnChkIn;			//PS0210
	
	//Reject Conveyor Out
	int m_nRej1UldCvyStopUpDnCylOut;			// S0500
	int m_nRej2UldCvyStopUpDnCylOut;			// S0501

	//Reject 1 Conveyor In
	int m_nRej1UldCvyStopUpChkIn;				//PS0500
	int m_nRej1UldCvyStopDnChkIn;				//PS0501
	int m_nRej2UldCvyStopUpChkIn;				//PS0502
	int m_nRej2UldCvyStopDnChkIn;				//PS0503

	//Out
	int m_nLd1CvyStopUpDnOut[2];
	int m_nLd2CvyStopUpDnOut[2];
	int m_nULd1CvyStopUpDnOut[2];
	int m_nULd2CvyStopUpDnOut[2];
	int m_nRej1CvyStopUpDnOut[2];
	int m_nRej2CvyStopUpDnOut[2];

	//Dn In
	int m_nLd1CvyStopDnIn[2];
	int m_nLd2CvyStopDnIn[2];
	int m_nUld1CvyStopDnIn[2];
	int m_nUld2CvyStopDnIn[2];
	int m_nRej1CvyStopDnIn[2];
	int m_nRej2CvyStopDnIn[2];

	//Up In
	int m_nLd1CvyStopUpIn[2];
	int m_nLd2CvyStopUpIn[2];
	int m_nUld1CvyStopUpIn[2];
	int m_nUld2CvyStopUpIn[2];
	int m_nRej1CvyStopUpIn[2];
	int m_nRej2CvyStopUpIn[2];
	
	//LdStacker
	int m_nLdTrayPlateLfOnOffCylOut;			// S0100
	int m_nLdTrayPlateRiOnOffCylOut;			// S0101
	int m_nLdStkUpCylOut;						// S0102		
	int m_nLdStkDnCylOut;						// S0103		

	int m_nLdTrayPlateLfOnChkIn; 				//PS0100
	int m_nLdTrayPlateLfOffChkIn;				//PS0101
	int m_nLdTrayPlateRiOnChkIn;				//PS0102
	int m_nLdTrayPlateRiOffChkIn;				//PS0103
	int m_nLdStkUpChkIn;						//PS0104		
	int m_nLdStkDnChkIn;						//PS0105		
	
	int m_nLdTrayPlateLfOnOffCyl[2];
	int m_nLdTrayPlateRiOnOffCyl[2];
	int m_nLdStkUpCyl[2];
	int m_nLdStkDnCyl[2];

	int m_nLdTrayPlateLfOnChk[2];
	int m_nLdTrayPlateLfOffChk[2];
	int m_nLdTrayPlateRiOnChk[2];
	int m_nLdTrayPlateRiOffChk[2];
	int m_nLdStkUpChk[2];
	int m_nLdStkDnChk[2];

	//Empty Stacker
	int m_nEmptyStkUpCylOut;					// S0200	
	int m_nEmptyStkDnCylOut;					// S0201
	int m_nEmptyStkUpChkIn;					//PS0200	
	int m_nEmptyStkDnChkIn;					//PS0201

	int m_nEmptyStkUpCyl[2];
	int m_nEmptyStkDnCyl[2];
	int m_nEmptyStkUpChk[2];
	int m_nEmptyStkDnChk[2];

	//Uld Stacker
	int m_nUld1StkUpCylOut;						// S0300		
	int m_nUld1StkDnCylOut;						// S0301	
	int m_nUld2StkUpCylOut;						// S0302	
	int m_nUld2StkDnCylOut;						// S0303	

	int m_nUld1StkUpChkIn;						//PS0300	
	int m_nUld1StkDnChkIn;						//PS0301	
	int m_nUld2StkUpChkIn;						//PS0302	
	int m_nUld2StkDnChkIn;						//PS0303	
	
	int m_nUld1StkUpCyl[2];
	int m_nUld1StkDnCyl[2];
	int m_nUld2StkUpCyl[2];
	int m_nUld2StkDnCyl[2];

	int	m_nUld1StkUpChk[2];
	int	m_nUld1StkDnChk[2];
	int	m_nUld2StkUpChk[2];
	int	m_nUld2StkDnChk[2];

	//Rej Empty Stacker
	int m_nRejEmptyStkUpCylOut;					// S0502
	int m_nRejEmptyStkDnCylOut;					// S0503
	int m_nRej1StkUpCylOut;						// S0504
	int m_nRej1StkDnCylOut;						// S0505
	int	m_nRej2StkUpCylOut;						// S0603
	int m_nRej2StkDnCylOut;						// S0604

	//Rej empty Stk
	int m_nRejEmptyStkUpChkIn;					//PS0504
	int m_nRejEmptyStkDnChkIn;					//PS0505
	int	m_nRej1StkUpChkIn;						//PS0506
	int	m_nRej1StkDnChkIn;						//PS0507
	int	m_nRej2StkUpChkIn;						// PS0607
	int	m_nRej2StkDnChkIn;						// PS0608
	
	int m_nRejEmptyStkUpCyl[2];	
	int m_nRejEmptyStkDnCyl[2];	
	int m_nRej1StkUpCyl[2];		
	int m_nRej1StkDnCyl[2];		
	int	m_nRej2StkUpCyl[2];		
	int m_nRej2StkDnCyl[2];		

	int m_nRejEmptyStkUpChk[2];	
	int m_nRejEmptyStkDnChk[2];	
	int	m_nRej1StkUpChk[2];		
	int	m_nRej1StkDnChk[2];		
	int	m_nRej2StkUpChk[2];		
	int	m_nRej2StkDnChk[2];		
	
	//Tray Transfer
	int m_nTrayTrsfUpCylOut;					// S0400
	int m_nTrayTrsfDnCylOut;					// S0401
	int m_nTrayTrsfVacOnOffOut;					// S0402
	int m_nTrayTrsfEjtOnOffOut;					// S0403

	int m_nTrayTrsfUpChkIn;					//PS0400
	int m_nTrayTrsfDnChkIn;					//PS0401
	int m_nTrayTrsfVacChkIn;				//PS0402

	int	m_nTrayTrsfUpCyl[2];	
	int	m_nTrayTrsfDnCyl[2];	
	int	m_nTrayTrsfVacOnOff[2];
	int	m_nTrayTrsfEjtOnOff[2];

	int m_nTrayTrsfUpChk[2];	
	int m_nTrayTrsfDnChk[2];	
	int m_nTrayTrsfVacChk[2];

	//Reject Tray Transfer
	int m_nRejTrsfUpCylOut;						// S0404
	int m_nRejTrsfDnCylOut;						// S0405
	int m_nRejTrsfVacOnOffOut;					// S0406
	int m_nRejTrsfEjtOnOffOut;					// S0407

	int m_nRejTrsfUpChkIn;						//PS0403
	int m_nRejTrsfDnChkIn;						//PS0404
	int m_nRejTrsfVacChkIn;						//PS0405

	int m_nRejTrsfUpCyl[2];		
	int m_nRejTrsfDnCyl[2];		
	int m_nRejTrsfVacOnOff[2];	
	int m_nRejTrsfEjtOnOff[2];	

	int m_nRejTrsfUpChk[2];		
	int m_nRejTrsfDnChk[2];		
	int m_nRejTrsfVacChk[2];		
	//PS0406
	//PS0407

	//Work Robot
	int m_nLdUldPickUpCylOut;						// S0700
	int m_nLdUldPickDnCylOut;						// S0701
	int m_nLdUldPickRot_0_CylOut;					// S0702
	int m_nLdUldPickRot_90_CylOut;					// S0703
	int m_nLdUldPickGripCylOut;						// S0704

	// S0705
	int m_nLdUldPickUpChkIn;						//PS0700
	int m_nLdUldPickDnChkIn;						//PS0701
	int m_nLdUldPickRot_0_ChkIn;					//PS0702
	int m_nLdUldPickRot_90_ChkIn;					//PS0703
	int m_nLdUldPickDvcChkIn;						//PS0704

	int m_nLdUldPickUpCyl[2];		
	int m_nLdUldPickDnCyl[2];		
	int m_nLdUldPickRot_0_Cyl[2];	
	int m_nLdUldPickRot_90_Cyl[2];	
	int m_nLdUldPickGripCyl[2];		

	int m_nLdUldPickUpChk[2];		
	int m_nLdUldPickDnChk[2];		
	int m_nLdUldPickRot_0_Chk[2];	
	int m_nLdUldPickRot_90_Chk[2];	
	int m_nLdUldPickDvcChk[2];		

	
	//Ld Buffer
	int m_nLdBuff1GripOnOffCylOut;				// S1000
	int m_nLdBuff2GripOnOffCylOut;				// S1001
	int m_nLdBuff3GripOnOffCylOut;				// S1002
	int m_nLdBuff4GripOnOffCylOut;				// S1003

	int m_nLdBuff1GripOnOffCyl[2];
	int m_nLdBuff2GripOnOffCyl[2];
	int m_nLdBuff3GripOnOffCyl[2];
	int m_nLdBuff4GripOnOffCyl[2];

	//Uld Buffer
	int m_nUldBuff1GripOnOffCylOut;				// S1004
	int m_nUldBuff2GripOnOffCylOut;				// S1005
	int m_nUldBuff3GripOnOffCylOut;				// S1006
	int m_nUldBuff4GripOnOffCylOut;				// S1007

	int m_nUldBuff1GripOnOffCyl[2];
	int m_nUldBuff2GripOnOffCyl[2];
	int m_nUldBuff3GripOnOffCyl[2];
	int m_nUldBuff4GripOnOffCyl[2];


 	int m_nLdBuff1GripDvcExistChkIn;			//PS1000
 	int m_nLdBuff2GripDvcExistChkIn;			//PS1001
 	int m_nLdBuff3GripDvcExistChkIn;			//PS1002
 	int m_nLdBuff4GripDvcExistChkIn;			//PS1003

	int m_nUldBuff1GripDvcExistChkIn;			//PS1004
	int m_nUldBuff2GripDvcExistChkIn;			//PS1005
	int m_nUldBuff3GripDvcExistChkIn;			//PS1006
	int m_nUldBuff4GripDvcExistChkIn;			//PS1007


	//Test site Robot Out
	int m_nTestRbtPicker1GripCylOut;			// S2500
	int m_nTestRbtPicker2GripCylOut;			// S2501
	int m_nTestRbtPicker3GripCylOut;			// S2502
	int m_nTestRbtPicker4GripCylOut;			// S2503

	int m_nTestRbtPicker1GripDvcExistChkIn;	//PS2500
	int m_nTestRbtPicker2GripDvcExistChkIn;	//PS2501
	int m_nTestRbtPicker3GripDvcExistChkIn;	//PS2502
	int m_nTestRbtPicker4GripDvcExistChkIn;	//PS2503

	//Test Site Out
	int m_nTsite1UpCylOut;						// S2100
	int m_nTsite1DnCylOut;						// S2101
	int m_nTsite2UpCylOut;						// S2102
	int m_nTsite2DnCylOut;						// S2103
	int m_nTsite3UpCylOut;						// S2104
	int m_nTsite3DnCylOut;						// S2105
	int m_nTsite4UpCylOut;						// S2106
	int m_nTsite4DnCylOut;						// S2107


	int m_nTsite5UpCylOut;						// S2300
	int m_nTsite5DnCylOut;						// S2301
	int m_nTsite6UpCylOut;						// S2302
	int m_nTsite6DnCylOut;						// S2303
	int m_nTsite7UpCylOut;						// S2304
	int m_nTsite7DnCylOut;						// S2305
	int m_nTsite8UpCylOut;						// S2306
	int m_nTsite8DnCylOut;						// S2307

	int m_nTsite1UpCyl[2];
	int m_nTsite1DnCyl[2];
	int m_nTsite2UpCyl[2];
	int m_nTsite2DnCyl[2];
	int m_nTsite3UpCyl[2];
	int m_nTsite3DnCyl[2];
	int m_nTsite4UpCyl[2];
	int m_nTsite4DnCyl[2];


	int m_nTsite5UpCyl[2];
	int m_nTsite5DnCyl[2];
	int m_nTsite6UpCyl[2];
	int m_nTsite6DnCyl[2];
	int m_nTsite7UpCyl[2];
	int m_nTsite7DnCyl[2];
	int m_nTsite8UpCyl[2];
	int m_nTsite8DnCyl[2];

	//Test Site In
	int m_nTsite1UpChkIn;						//PS2102
	int m_nTsite1DnChkIn;						//PS2103
	int m_nTsite2UpChkIn;						//PS2200
	int m_nTsite2DnChkIn;						//PS2201
	int m_nTsite3UpChkIn;						//PS2206
	int m_nTsite3DnChkIn;						//PS2207
	int m_nTsite4UpChkIn;						//PS2212
	int m_nTsite4DnChkIn;						//PS2213

	int m_nTsite5UpChkIn;						//PS2302
	int m_nTsite5DnChkIn;						//PS2303
	int m_nTsite6UpChkIn;						//PS2400
	int m_nTsite6DnChkIn;						//PS2401
	int m_nTsite7UpChkIn;						//PS2406
	int m_nTsite7DnChkIn;						//PS2407
	int m_nTsite8UpChkIn;						//PS2412
	int m_nTsite8DnChkIn;						//PS2413
	

	int m_nTsite1UpChk[2];
	int m_nTsite1DnChk[2];
	int m_nTsite2UpChk[2];
	int m_nTsite2DnChk[2];
	int m_nTsite3UpChk[2];
	int m_nTsite3DnChk[2];
	int m_nTsite4UpChk[2];
	int m_nTsite4DnChk[2];

	int m_nTsite5UpChk[2];
	int m_nTsite5DnChk[2];
	int m_nTsite6UpChk[2];
	int m_nTsite6DnChk[2];
	int m_nTsite7UpChk[2];
	int m_nTsite7DnChk[2];
	int m_nTsite8UpChk[2];
	int m_nTsite8DnChk[2];


	
	BOOL			Create();
	void            OnDisplayConveyor(); // LD1,2 /Uld/Rej V
	void            OnDisplayStacker(); //LD/Empty/Uld1,2/Rej 1,2 V
	void			OnDisplayTransfer();//Ld Transfer/ Rej Transfer V
	void			OnDisplayLdRobot();
	void			OnDisplayBuffer();
	void			OnDisplayTestSiteRobot();
	void			OnDisplayTestSite(int nNum);
	void			OnDisplayTestSite();
	
	void            OnInitGridConveyor();
	void            OnInitGridStacker();
	void            OnInitGridTransfer();
	void			OnInitGridLdRbt();
	void			OnInitGridBuffer();
	void			OnInitGridTsiteRbt();
	void			OnInitGridTsite();
	
// 	void			OnDisplayUnLdRobot();
// 	void			OnDisplayLeftContact();
// 	void			OnDisplayRightContact();
// 
// 	void			OnInitGridUldRbt();
// 	void			OnInitGridLfContact();
// 	void			OnInitGridRiContact();
	
	void			OnInitButton();


public:
	CDialogManualIO(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogManualIO();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_MANUAL_IO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL DestroyWindow();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLdRbtCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
		afx_msg void OnUldRbtCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
		afx_msg void OnTRbtCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
		afx_msg void OnTsiteCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
		afx_msg void OnLeftContactCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
		afx_msg void OnRightContactCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
		//afx_msg void OnBufferCellClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnBnClickedBtnExit();
	afx_msg void OnBnClickedBtnRefresh();
	afx_msg void OnBnClickedBtnLeftClamp();
	afx_msg void OnBnClickedBtnLeftUnclamp();
	afx_msg void OnBnClickedBtnRightClamp();
	afx_msg void OnBnClickedBtnRightUnclamp();
	CGridCtrl m_pGridConveyor;
	CGridCtrl m_pGridTransfer;
	CGridCtrl m_pGridStacker;
	CGridCtrl m_pGridWorkRobot;
	CGridCtrl m_pGridTSiteRobot;
	CGridCtrl m_pGridTestSite;
};
