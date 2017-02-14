#pragma once
#include "gradientstatic.h"
#include "digit.h"
#include ".\\Union\\Grid\\GridCtrl.h"
#include ".\\Union\\Grid\\GridURLCell.h"
#include ".\\Union\\Grid\\GridCellCombo.h"
#include ".\\Union\\Grid\\GridCellCheck.h"
#include ".\\Union\\Grid\\GridCellNumeric.h"
#include ".\\Union\\Grid\\GridCellDateTime.h"
#include "afxwin.h"

#define TM_MOR_MOVE		7870
#define TM_TMOR_MOVE		7871
#define	TM_TDOOR_MOVE  7872
#define	TM_TSITE_UPDN		7873


enum WRBT_TRAY_POS
{
	LDTRAY=0,
	UNLOAD1,
	UNLOAD2,
	REJECT1,
	REJECT2,
};


// CWorkManualMove 대화 상자입니다.

class CWorkManualMove : public CDialogEx
{
	DECLARE_DYNAMIC(CWorkManualMove)

public:
	CWorkManualMove(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CWorkManualMove();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_WORK_MANUAL_MOVE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:

	CGridCtrl m_pGridTestSite;

	CGradientStatic m_msg_LdRbt_Tray_x;
	CGradientStatic m_msg_LdRbt_Tray_y;


	CGradientStatic m_msg_LdRbt_Buff;
	CGradientStatic m_msg_UldRbt_Buff;

	CGradientStatic m_msg_TsiteRbt_Pos;
	CGradientStatic m_msg_TsiteRbt_Socket;

	CGradientStatic m_msg_TsiteRbt_Buff_Pick;
	CGradientStatic m_msg_TsiteRbt_Buff_Pos;
	CGradientStatic m_msg_Retest_1;
	CGradientStatic m_msg_Retest_2;

	int	m_nStackTray;
	int	m_nWorkBuffer;
	int m_nRetest;
	int m_nTsiteBuffer;
	int m_nFirstPos;
	
	CButton	m_btnStackTray[5];
	CButton	m_btnWorkBuffer[2];
	CButton	m_btnRetest[2];
	CButton	m_btnTsiteBuff[2];	

	CDigit m_dgt_LdRbt_Tray_x;
	CDigit m_dgt_LdRbt_Tray_y;
	CDigit m_dgt_LdRbt_Buff;
	CDigit m_dgt_UldRbt_Buff;
	CDigit m_dgt_TsiteRbt_Site_Pos;
	CDigit m_dgt_TsiteRbt_Socket;
	CDigit m_dgt_Retest_1;
	CDigit m_dgt_Retest_2;
	CDigit m_dgt_TsiteRbt_Buff_Pick;
	CDigit m_dgt_TsiteRbt_Buff_Pos;

	int m_nTrayX;
	int m_nTrayY;
	int m_nWorkSite;
	/////////////////////////////////////
	int			m_nLinearMove_Index;   //현재 map index 
	long		m_lAxisCnt;            //현재의 IndexNum에서 사용 가능한 모터 수량 최대 4개  
	long		m_lpAxisNum[4];        //현재의 IndexNum에서 사용하는 모터 실재 번호를 가진다 
	double	m_dpTargetPosList[4];		   //최대 4 포인트를 한번에 이동가능하다 
 	double	m_dpSpdRatio[3];			//속도비율 [0]:Vel, [1]:Acc, [2]:Dec
	int			m_nMove_Step;
	int			m_nTest_Step;
	int			m_nTDoorStep;
	int			m_nTUpDnStep;
	bool		m_bWaitChk_Falg;
	CString	m_strAlarmCode;
	DWORD	m_dwWaitTime[3];
	double	m_dCurrentPos[100];//x,y,z
	int			m_npFindWorkPosYXCPB[5]; //james 2015.0807 처음 찾을때의 정보,  [0]first tray Y, [1]first tray X, [2]:작업가능 수량 또는 공간 정보, [3]first picker num, [4]first buff num
	int			m_npSet_WorkPosYXCPB[5]; //james 2015.0807 작업 완료후 작업 셋팅 위치 정보,  [0]first tray Y, [1]first tray X, [2]:작업가능 수량 또는 공간 정보, [3]first picker num, [4]first buff num
	double  m_dTemp_CalTargetPos_Y;
	double  m_dTemp_CalTargetPos_X;

	int			m_nWorkLdBuff;
	int			m_nWorkUldBuff;
	int			m_nTsite_Pos;
	int			m_nTsite_Socket;
	int			m_nMode;

	int			m_nFirstPicker_Num;
	int			m_nFirstTray_X_Num;
	int			m_nFirstTray_Y_Num;
	int			m_nFirst_Buff_Num; //공통으로사용하기 위해 추가 , //버퍼, 소켓등 정보 확인에 사용 

	int			m_nRetest1_Pos;
	int			m_nRetest2_Pos;
	int			m_nTsiteBuffer_PickNo;
	int			m_nTsiteBuffer_Pos;
	
	int			m_nOpenClose[8];
	int			m_nDoorTestSite;
	int			m_nUpDn[8];

	bool		m_bPushUpDnFlag[8];
	DWORD	m_dwPushOnOff[8][3];

	void		OnInitDigit();
	void		OnInitGridTsite();
	void		OnInitLabel();

	void		OnChange_TrayType();
	void		OnChange_WorkBufferType();
	void		OnChange_RetestBufferType();
	void		OnChange_TestBufferType();


	int			WorkRbt_Move(int nMode, int nWork_Site);
	int			TestRbt_Move(int nMode, int nWork_Site, int nStart_FixPos);
	int			OnTestSiteDoorOpenClose();
	int			OnTestSiteUpDn();
	void		OnBtnEnable(bool nEnable);

	void CWorkManualMove::Set_Push_UnDn( int nSite, int nOnOff );
	int CWorkManualMove::Chk_Push_UnDn( int nSite, int nOnOff );

	afx_msg void OnBnClickedBtnTsiteRbtBufferGo();
	afx_msg void OnBnClickedBtnRetestGo();
	afx_msg void OnBnClickedBtnTsiteRbtTsiteGo();
	afx_msg void OnBnClickedBtnWorkRbtTrayGo();
	afx_msg void OnBnClickedBtnWorkRbtBufGo();
	afx_msg void OnStnClickedDgtLdRbtTrayX();
	afx_msg void OnStnClickedDgtLdRbtTrayY();
	afx_msg void OnStnClickedDgtLdBuffer();
	afx_msg void OnStnClickedDgtUldBuffer();
	afx_msg void OnStnClickedDgtTsiteRbtSite();
	afx_msg void OnStnClickedDgtTsiteRbtSocket();
	afx_msg void OnStnClickedDgtTsiteRetest();
	afx_msg void OnStnClickedDgtTsiteRbtBufferPick();
	afx_msg void OnStnClickedDgtTsiteRbtBufferPos();
	virtual BOOL OnInitDialog();
	afx_msg void OnStnClickedDgtTsiteRetest2();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	
	afx_msg void OnBnClickedBtnDialogManualExit();
	afx_msg void OnBnClickedRadioLdStk();
	afx_msg void OnBnClickedRadioUldStk1();
	afx_msg void OnBnClickedRadioUldStk2();
	afx_msg void OnBnClickedRadioRejStk1();
	afx_msg void OnBnClickedRadioRejStk2();
	afx_msg void OnBnClickedRadioLdBuffer();
	afx_msg void OnBnClickedRadioUldBuffer();
	afx_msg void OnBnClickedRadioRetest1();
	afx_msg void OnStnClickedMsgTsiteRbtSite();
	afx_msg void OnBnClickedRadioRetest2();
	afx_msg void OnBnClickedRadioTsiteLdBuff();
	afx_msg void OnBnClickedRadioTsiteUldBuff();
	afx_msg void OnBnClickedBtnTsiteRbt();
	afx_msg void OnBnClickedBtnTsite1OpenClose();
	CButton m_btnTraygo;
	CButton m_btnWorkBuffergo;
	CButton m_btnTsiteBuffergo;
	CButton m_btnTsiteRetestgo;
	CButton m_btnTsiteSocketgo;
	CButton m_btnMMoveExit;
	afx_msg void OnBnClickedBtnTsite2OpenClose();
	afx_msg void OnBnClickedBtnTsite3OpenClose();
	afx_msg void OnBnClickedBtnTsite4OpenClose();
	afx_msg void OnBnClickedBtnTsite5OpenClose();
	afx_msg void OnBnClickedBtnTsite6OpenClose();
	afx_msg void OnBnClickedBtnTsite7OpenClose();
	afx_msg void OnBnClickedBtnTsite8OpenClose();
	afx_msg void OnBnClickedBtnTsite1UpDn();
	afx_msg void OnBnClickedBtnTsite2UpDn();
	afx_msg void OnBnClickedBtnTsite3UpDn();
	afx_msg void OnBnClickedBtnTsite4UpDn();
	afx_msg void OnBnClickedBtnTsite5UpDn();
	afx_msg void OnBnClickedBtnTsite6UpDn();
	afx_msg void OnBnClickedBtnTsite7UpDn();
	afx_msg void OnBnClickedBtnTsite8UpDn();
};
