// WorkVisionTeach.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "WorkVisionTeach.h"
#include "afxdialogex.h"
#include "FastechPublic_IO.h"
#include "Dialog_Message.h"
#include "PublicFunction.h"
#include "CtlBd_Variable.h"
#include "Variable.h"
#include "ComizoaPublic.h"
#include "Dialog_KeyPad.h"
#include "AlgMemory.h"
#include "FastechPublic_IO.h"
#include "Variable.h"
#include "RunLdUldRobot.h"
#include "CtlBd_Library.h"
#include "RunTestSiteRobot.h"
// CWorkVisionTeach 대화 상자입니다.

IMPLEMENT_DYNAMIC(CWorkVisionTeach, CDialogEx)

CWorkVisionTeach::CWorkVisionTeach(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWorkVisionTeach::IDD, pParent)
{
	//SetTimer(TM_VISION_DISPLAY);

	m_dFocusZ = 0;
	m_nSimul = 1;
	
}

CWorkVisionTeach::~CWorkVisionTeach()
{
}

void CWorkVisionTeach::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RESPONSE_DATA, m_listVisionResponse);

	DDX_Control(pDX, IDC_MSG_TSITE_RBT_SITE, m_msg_TsiteRbt_Site);
	DDX_Control(pDX, IDC_MSG_TSITE_RBT_MARK, m_msg_TsiteRbt_Mark);

	DDX_Control(pDX, IDC_DGT_TSITE_RBT_MARK, m_dgt_TsiteRbt_Mark);
	DDX_Control(pDX, IDC_DGT_TSITE_RBT_SITE, m_dgt_TsiteRbt_Site);

	// 	DDX_Control(pDX, IDC_MSG_RIGHT_TSITE_RBT_SITE, m_msg_RightTsiteRbt_Site);
	// 	DDX_Control(pDX, IDC_MSG_RIGHT_TSITE_RBT_MARK, m_msg_RightTsiteRbt_Mark);
	// 
	// 	DDX_Control(pDX, IDC_DGT_RIGHT_TSITE_RBT_SITE, m_dgt_RightTsiteRbt_Site);
	// 	DDX_Control(pDX, IDC_DGT_RIGHT_TSITE_RBT_MARK, m_dgt_RightTsiteRbt_Mark);

	DDX_Control(pDX, IDC_GROUP_LEFT_TSITE_POS, m_group_Vision_Pos);
	DDX_Control(pDX, IDC_CUSTOM_LEFT_READ, m_pGridLeftRead);
	//DDX_Control(pDX, IDC_CUSTOM_RIGHT_READ, m_pGridRightRead);

	DDX_Control(pDX, IDC_CUSTOM_LEFT_TEACH, m_pGridLeftTeach);
	DDX_Control(pDX, IDC_CUSTOM_RIGHT_TEACH, m_pGridRightTeach);
	DDX_Control(pDX, IDC_BTN_SAVE, m_btnDataSave);
	DDX_Control(pDX, IDC_BTN_LOAD, m_btnDataLoad);
	//DDX_Control(pDX, IDC_CUSTOM_TESTSITE, m_pGridTestSitePos);

	DDX_Control(pDX, IDC_CUSTOM_TEACH_LEFT_POS, m_pGridTsiteLeftOffset);
	DDX_Control(pDX, IDC_CUSTOM_TEACH_RIGHT_POS, m_pGridTsiteRightOffset);
	DDX_Control(pDX, IDC_CUSTOM_LEFT_VISION_TEACH, m_pGridLeftVisionTeach);
	DDX_Control(pDX, IDC_CUSTOM_RIGHT_VISION_TEACH, m_pGridRightVisionTeach);
	DDX_Control(pDX, IDC_CUSTOM_TESTSITE_LEFT_VALUE, m_pGridLeftValue);
	DDX_Control(pDX, IDC_CUSTOM_TESTSITE_RIGHT_VALUE, m_pGridRightValue);
	/*DDX_Control(pDX, IDC_BTN_TEST_SITE_ADD_OFFSET, m_btnAutoTeacingUse);*/
	DDX_Control(pDX, IDC_BTN_TEST_SITE_ADD_OFFSET, m_btnAutoTeachUse);
}


BEGIN_MESSAGE_MAP(CWorkVisionTeach, CDialogEx)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_LEFT_TSITE_RBT_FOCUS_GO, &CWorkVisionTeach::OnBnClickedBtnLeftTsiteRbtFocusGo)
	ON_STN_CLICKED(IDC_DGT_TSITE_RBT_SITE, &CWorkVisionTeach::OnStnClickedDgtLeftTsiteRbtSite)
	ON_BN_CLICKED(IDC_BTN_LEFT_TSITE_RBT_TEACH_GO, &CWorkVisionTeach::OnBnClickedBtnLeftTsiteRbtTeachGo)
	ON_STN_CLICKED(IDC_DGT_TSITE_RBT_MARK, &CWorkVisionTeach::OnStnClickedDgtTsiteRbtMark)
	ON_BN_CLICKED(IDC_BTN_VISION_SHOW, &CWorkVisionTeach::OnBnClickedBtnVisionShow)
	ON_BN_CLICKED(IDC_BTN_VISION_HIDE, &CWorkVisionTeach::OnBnClickedBtnVisionHide)
	ON_BN_CLICKED(IDC_BTN_OFFSET, &CWorkVisionTeach::OnBnClickedBtnOffset)
	ON_BN_CLICKED(IDC_BTN_EDGE, &CWorkVisionTeach::OnBnClickedBtnEdge)
	ON_BN_CLICKED(IDC_BTN_LOAD, &CWorkVisionTeach::OnBnClickedBtnLoad)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CWorkVisionTeach::OnBnClickedBtnSave)
	//kwlee 2016.1013
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_LEFT_TEACH,			OnMarkLeftTeachCellClick)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_RIGHT_TEACH,			OnMarkRightTeachCellClick)
	ON_BN_CLICKED(IDC_BTN_LEFT_TSITE_RBT_TEACH_GO2, &CWorkVisionTeach::OnBnClickedBtnLeftTsiteRbtTeachGo2)
	ON_BN_CLICKED(IDC_BTN_TEST_SITE_ADD_OFFSET, &CWorkVisionTeach::OnBnClickedBtnTestSiteAddOffset)
	ON_BN_CLICKED(IDC_BTN_TSITE_RBT_SET_GO, &CWorkVisionTeach::OnBnClickedBtnTsiteRbtSetGo)
	ON_BN_CLICKED(IDC_BTN_TEST_SITE_VALUE_SET, &CWorkVisionTeach::OnBnClickedBtnTestSiteValueSet)
END_MESSAGE_MAP()


// CWorkVisionTeach 메시지 처리기입니다.


void CWorkVisionTeach::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnTimer(nIDEvent);
	int nRet;
	switch (nIDEvent)
	{
	case TM_VISION_FCOUS:
		nRet = VisionFocus_Move( m_nSite_No );//테스트
		if( nRet == RET_GOOD )
		{
			KillTimer(TM_VISION_FCOUS);
			//OnBtnEnable(true);
		}
		else if(nRet == RET_ERROR)
		{
			KillTimer(TM_VISION_FCOUS);
			//OnBtnEnable(true);
		}
		break;
	case TM_VISION_TEACH:
		nRet = VisionTeach_Move();
		if( nRet == RET_GOOD )
		{
			KillTimer(TM_VISION_TEACH);
			//OnBtnEnable(true);
		}
		else if(nRet == RET_ERROR)
		{
			KillTimer(TM_VISION_TEACH);
			//OnBtnEnable(true);
		}
		break;
	case TM_VISION_POS_SET:
		nRet = ValueSetMove();
		if( nRet == RET_GOOD )
		{
			KillTimer(TM_VISION_POS_SET);
			//OnBtnEnable(true);
		}
		else if(nRet == RET_ERROR)
		{
			KillTimer(TM_VISION_POS_SET);
			//OnBtnEnable(true);
		}
		break;

		//kwlee 2016.1007
	case TM_VISION_DISPLAY:
		OnMainLeftVisionTeach();
		OnMainRightVisionTeach();

		OnMaintLeftRead();
		OnMainRightRead();

		OnMainLeftMarkTeach();
		OnMainRightMarkTeach();

		OnMainVisionLeftTeachValue();
		OnMainVisionRightTeachValue();
		break;
	}
}


////////
//kwlee 2016.0107
void CWorkVisionTeach::OnMainRightVisionTeach()
{
	CString strRect;

	for (int i = 0; i<4; i++)
	{
		for (int j =0; j<4; j++)
		{
			if (st_teach.m_dRightVisionTeachXPos[i][j] > 0)
			{
				strRect.Format(_T("%.3f"),st_teach.m_dRightVisionTeachXPos[i][j]);
				m_pGridRightVisionTeach.SetItemFont(i+2, j +1, &clsFunc.OnLogFont(12));
				m_pGridRightVisionTeach.SetItemText(i+2, j +1, strRect);
			}

			if (st_teach.m_dRightVisionTeachYPos[i][j] > 0)
			{
				strRect.Format(_T("%.3f"),st_teach.m_dRightVisionTeachYPos[i][j]);
				m_pGridRightVisionTeach.SetItemFont(i+2, j +5, &clsFunc.OnLogFont(12));
				m_pGridRightVisionTeach.SetItemText(i+2, j +5, strRect);
			}
		}		
	}
	m_pGridRightVisionTeach.Invalidate(FALSE);
}


void CWorkVisionTeach::OnMainLeftVisionTeach()
{
	//m_dRightVisionOffSetXPos[4][4];
	//m_dRightVisionOffSetYPos[4][4];

	CString strRect;

	for (int i = 0; i<4; i++)
	{
		for (int j =0; j<4; j++)
		{
			if (st_teach.m_dLeftVisionTeachXPos[i][j] > 0)
			{
				strRect.Format(_T("%.3f"),st_teach.m_dLeftVisionTeachXPos[i][j]);
				m_pGridLeftVisionTeach.SetItemFont(i+2, j +1, &clsFunc.OnLogFont(12));
				m_pGridLeftVisionTeach.SetItemText(i+2, j +1, strRect);
				
			}

			if (st_teach.m_dLeftVisionTeachYPos[i][j] > 0)
			{
				strRect.Format(_T("%.3f"),st_teach.m_dLeftVisionTeachYPos[i][j]);
				m_pGridLeftVisionTeach.SetItemFont(i+2, j +5, &clsFunc.OnLogFont(12));
				m_pGridLeftVisionTeach.SetItemText(i+2, j +5, strRect);
			}
		}		
	}
	m_pGridLeftVisionTeach.Invalidate(FALSE);
}

//////////////////////
void CWorkVisionTeach::OnMaintLeftRead()
{

	CString strRect;

	for (int i = 0; i<4; i++)
	{
		for (int j =0; j<4; j++)
		{
			if (st_teach.m_dLeftVisionReadXPos[i][j] != 0)
			{
				strRect.Format(_T("%3.3f"),st_teach.m_dLeftVisionReadXPos[i][j]);
				m_pGridTsiteLeftOffset.SetItemText(i+2, j +1, strRect);
			}

 			if (st_teach.m_dLeftVisionReadYPos[i][j] != 0)
 			{
				strRect.Format(_T("%3.3f"),st_teach.m_dLeftVisionReadYPos[i][j]);
				m_pGridTsiteLeftOffset.SetItemText(i+2, j +5, strRect);
			}
		}		
	}
	m_pGridTsiteLeftOffset.Invalidate(FALSE);
}

void CWorkVisionTeach::OnMainRightRead()
{
	//m_dRightVisionOffSetXPos[4][4];
	//m_dRightVisionOffSetYPos[4][4];

	CString strRect;

	for (int i = 0; i<4; i++)
	{
		for (int j =0; j<4; j++)
		{
			if (st_teach.m_dRightVisionReadXPos[i][j] != 0)
			{
				strRect.Format(_T("%3.3f"),st_teach.m_dRightVisionReadXPos[i][j]);
				m_pGridTsiteRightOffset.SetItemText(i+2, j +1, strRect);
			}

			if (st_teach.m_dRightVisionReadYPos[i][j] != 0)
			{
				strRect.Format(_T("%3.3f"),st_teach.m_dRightVisionReadYPos[i][j]);
				m_pGridTsiteRightOffset.SetItemText(i+2, j +5, strRect);
			}
		}		
	}
	m_pGridTsiteRightOffset.Invalidate(FALSE);
}

/////

void CWorkVisionTeach::OnMainLeftMarkTeach()
{

	//	m_dLeftVisionOffSetXPos[4][4];
	//	m_dLeftVisionOffSetYPos[4][4];

	CString strRect;

	for (int i = 0; i<4; i++)
	{

		for (int j =0; j<4; j++)
		{
			//	st_teach.m_dLeftVisionOffSetXPos[i][j] = 123.45;
			//	st_teach.m_dLeftVisionOffSetYPos[i][j] = 321.54;

			//st_teach.m_dLeftVisionTeachXPos[i][j] = 1.55;
			//st_teach.m_dLeftVisionTeachYPos[i][j] = 1.66;
			if (st_teach.m_dLeftVisionMarkXPos[i][j] > 0)
			{
				strRect.Format(_T("%.3f"),st_teach.m_dLeftVisionMarkXPos[i][j]);
				m_pGridLeftTeach.SetItemText(i+2, j +1, strRect);
			}

			if (st_teach.m_dLeftVisionMarkYPos[i][j] > 0)
			{
				strRect.Format(_T("%.3f"),st_teach.m_dLeftVisionMarkYPos[i][j]);
				m_pGridLeftTeach.SetItemText(i+2, j +5, strRect);
			}

			if (st_teach.m_dFocusTeachZPos[i] > 0)
			{
				strRect.Format(_T("%.3f"),st_teach.m_dFocusTeachZPos[i]);
				m_pGridLeftTeach.SetItemText(i+2, 9, strRect);
			}
		}		
	}
	m_pGridLeftTeach.Invalidate(FALSE);
}

void CWorkVisionTeach::OnMainRightMarkTeach()
{
	//m_dRightVisionOffSetXPos[4][4];
	//m_dRightVisionOffSetYPos[4][4];

	CString strRect;

	for (int i = 0; i<4; i++)
	{
		for (int j =0; j<4; j++)
		{
			//Offset Right
			//st_teach.m_dRightVisionTeachXPos[i][j] = 1.88;
			//st_teach.m_dRightVisionTeachYPos[i][j] = 1.99;
			if (st_teach.m_dRightVisionMarkXPos[i][j] > 0)
			{
				strRect.Format(_T("%.3f"),st_teach.m_dRightVisionMarkXPos[i][j]);
				m_pGridRightTeach.SetItemText(i+2, j +1, strRect);
			}

			if (st_teach.m_dRightVisionMarkYPos[i][j] > 0)
			{
				strRect.Format(_T("%.3f"),st_teach.m_dRightVisionMarkYPos[i][j]);
				m_pGridRightTeach.SetItemText(i+2, j +5, strRect);
			}

			if (st_teach.m_dFocusTeachZPos[i + 4] > 0)
			{
				strRect.Format(_T("%.3f"),st_teach.m_dFocusTeachZPos[i + 4]);
				m_pGridRightTeach.SetItemText(i+2, 9, strRect);
			}
		}		
	}
	m_pGridRightTeach.Invalidate(FALSE);
}
//2017.0109
void CWorkVisionTeach::OnMainVisionLeftTeachValue()
{
	CString strRect;

	for (int i = 0; i<4; i++)
	{
		for (int j =0; j<4; j++)
		{
// 			if (st_teach.m_dLeftVisionTeachXPos[i][j] > 0)
// 			{
				//strRect.Format(_T("%.3f"),st_teach.m_dLeftVisionTeachXPos[i][j] + st_teach.m_dLeftVisionOffSetXPos[i][j]);
				strRect.Format(_T("%.3f"),st_teach.m_dLeftVisionMarkXPos[i][j] - st_teach.m_dLeftVisionReadXPos[i][j]);
				m_pGridLeftValue.SetItemFont(i+2, j +1, &clsFunc.OnLogFont(12));
				m_pGridLeftValue.SetItemText(i+2, j +1, strRect);
			//}

// 			if (st_teach.m_dLeftVisionTeachYPos[i][j] > 0)
// 			{
				//strRect.Format(_T("%.3f"),st_teach.m_dLeftVisionTeachYPos[i][j] + st_teach.m_dLeftVisionOffSetYPos[i][j]);
				strRect.Format(_T("%.3f"),st_teach.m_dLeftVisionMarkYPos[i][j] - st_teach.m_dLeftVisionReadYPos[i][j]);
				m_pGridLeftValue.SetItemFont(i+2, j +5, &clsFunc.OnLogFont(12));
				m_pGridLeftValue.SetItemText(i+2, j +5, strRect);
			//}
		}		
	}
	m_pGridLeftValue.Invalidate(FALSE);
}

void CWorkVisionTeach::OnMainVisionRightTeachValue()
{
	CString strRect;

	for (int i = 0; i<4; i++)
	{
		for (int j =0; j<4; j++)
		{
// 			if (st_teach.m_dRightVisionTeachXPos[i][j] > 0)
// 			{
				//strRect.Format(_T("%.3f"),st_teach.m_dRightVisionTeachXPos[i][j] + st_teach.m_dRightVisionOffSetXPos[i][j]);
				strRect.Format(_T("%.3f"),st_teach.m_dRightVisionMarkXPos[i][j] - st_teach.m_dRightVisionReadXPos[i][j]);
				m_pGridRightValue.SetItemFont(i+2, j +1, &clsFunc.OnLogFont(12));
				m_pGridRightValue.SetItemText(i+2, j +1, strRect);
			//}

// 			if (st_teach.m_dRightVisionTeachYPos[i][j] > 0)
// 			{
				//strRect.Format(_T("%.3f"),st_teach.m_dRightVisionTeachYPos[i][j] + st_teach.m_dRightVisionOffSetYPos[i][j]);
				strRect.Format(_T("%.3f"),st_teach.m_dRightVisionMarkYPos[i][j] - st_teach.m_dRightVisionReadYPos[i][j]);
				m_pGridRightValue.SetItemFont(i+2, j +5, &clsFunc.OnLogFont(12));
				m_pGridRightValue.SetItemText(i+2, j +5, strRect);
		//	}

		}		
	}
	m_pGridRightValue.Invalidate(FALSE);
}

/////////////

void CWorkVisionTeach::OnInitRightOffsetGrid()
{
	int   i, j;
	int	  nMaxRow, nMaxCol;
	int   nRow;
	CString strTmp;
	
	nMaxRow = 5; // unloader tray y count
	nMaxCol = 9;

	m_pGridRightOffset.SetFrameFocusCell(FALSE);
	m_pGridRightOffset.SetTrackFocusCell(FALSE);
	m_pGridRightOffset.EnableSelection(FALSE);

	m_pGridRightOffset.SetGridLineColor(BLACK_C);
	m_pGridRightOffset.SetGridLines(1);

	m_pGridRightOffset.SetRowCount(nMaxRow);
	m_pGridRightOffset.SetColumnCount(nMaxCol);

	m_pGridRightOffset.SetFixedRowCount(0);
	m_pGridRightOffset.SetFixedColumnCount(0);
	m_pGridRightOffset.SetFixedBkColor(RGB(0,0,200));
	m_pGridRightOffset.SetFixedBkColor(RGB(200,200,255));
	m_pGridRightOffset.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<nMaxRow; i++) 
	{
		m_pGridRightOffset.SetRowHeight(i, 26);

		for (j=0; j<nMaxCol; j++) 
		{
			m_pGridRightOffset.SetColumnWidth(j, 49);

			m_pGridRightOffset.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridRightOffset.SetItemState(i, j, GVIS_READONLY);
			m_pGridRightOffset.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
		}
	}

	nRow = 0;
	m_pGridRightOffset.SetItemBkColour(nRow, 0, BLUE_D, WHITE_C);
	m_pGridRightOffset.SetItemFont(nRow, 0, &clsFunc.OnLogFont(13));
	m_pGridRightOffset.SetItemText(nRow, 0, _T("Site Num"));

	m_pGridRightOffset.MergeCells(nRow, 1, nRow,4);
	m_pGridRightOffset.SetItemBkColour(nRow, 1, BLUE_D, WHITE_C);
	m_pGridRightOffset.SetItemFont(nRow, 1, &clsFunc.OnLogFont(13));
	m_pGridRightOffset.SetItemText(nRow, 1, _T("Right Offset X Pos "));

	m_pGridRightOffset.MergeCells(nRow, 5, nRow,8);
	m_pGridRightOffset.SetItemBkColour(nRow, 5, BLUE_D, WHITE_C);
	m_pGridRightOffset.SetItemFont(nRow, 5, &clsFunc.OnLogFont(13));
	m_pGridRightOffset.SetItemText(nRow, 5, _T("Right Offset Y Pos "));

	for (int i =0; i<4; i++)
	{
		strTmp.Format(_T("%d"),i+1);
		m_pGridRightOffset.SetItemBkColour(i+1, 0, RGB(219, 229, 241), BLACK_C);
		m_pGridRightOffset.SetItemFont(i+1, 0, &clsFunc.OnLogFont(13));
		m_pGridRightOffset.SetItemText(i+1, 0, strTmp);
	}
}

//kwlee 2017.0106
void CWorkVisionTeach::OnInitLeftVisionTeachGrid()
{
	int   i, j;
	int	  nMaxRow, nMaxCol;
	int   nRow;
	CString strTmp;

	nMaxRow = 6; // unloader tray y count
	nMaxCol = 9;

	m_pGridLeftVisionTeach.SetFrameFocusCell(FALSE);
	m_pGridLeftVisionTeach.SetTrackFocusCell(FALSE);
	m_pGridLeftVisionTeach.EnableSelection(FALSE);

	m_pGridLeftVisionTeach.SetGridLineColor(BLACK_C);
	m_pGridLeftVisionTeach.SetGridLines(1);

	m_pGridLeftVisionTeach.SetRowCount(nMaxRow);
	m_pGridLeftVisionTeach.SetColumnCount(nMaxCol);

	m_pGridLeftVisionTeach.SetFixedRowCount(0);
	m_pGridLeftVisionTeach.SetFixedColumnCount(0);
	m_pGridLeftVisionTeach.SetFixedBkColor(RGB(0,0,200));
	m_pGridLeftVisionTeach.SetFixedBkColor(RGB(200,200,255));
	m_pGridLeftVisionTeach.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<nMaxRow; i++) 
	{
		m_pGridLeftVisionTeach.SetRowHeight(i, 19);

		for (j=0; j<nMaxCol; j++) 
		{
			m_pGridLeftVisionTeach.SetColumnWidth(j, 51);

			m_pGridLeftVisionTeach.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridLeftVisionTeach.SetItemState(i, j, GVIS_READONLY);
			m_pGridLeftVisionTeach.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
		}
	}

	nRow = 0;
	m_pGridLeftVisionTeach.MergeCells(0, 0, 1,0);
	m_pGridLeftVisionTeach.SetItemBkColour(nRow, 0, BLUE_D, WHITE_C);
	m_pGridLeftVisionTeach.SetItemFont(nRow, 0, &clsFunc.OnLogFont(13));

	m_pGridLeftVisionTeach.MergeCells(nRow, 1, nRow,4);
	m_pGridLeftVisionTeach.SetItemBkColour(nRow, 1, BLUE_D, WHITE_C);
	m_pGridLeftVisionTeach.SetItemFont(nRow, 1, &clsFunc.OnLogFont(13));
	m_pGridLeftVisionTeach.SetItemText(nRow, 1, _T("Left Vision Teach X Pos "));

	m_pGridLeftVisionTeach.MergeCells(nRow, 5, nRow,8);
	m_pGridLeftVisionTeach.SetItemBkColour(nRow, 5, BLUE_D, WHITE_C);
	m_pGridLeftVisionTeach.SetItemFont(nRow, 5, &clsFunc.OnLogFont(13));
	m_pGridLeftVisionTeach.SetItemText(nRow, 5, _T("Left Vision Teach Y Pos "));

	nRow = 1;
	m_pGridLeftVisionTeach.SetItemBkColour(nRow, 1, RGB(219, 229, 241), BLACK_C);
	m_pGridLeftVisionTeach.SetItemFont(nRow, 1, &clsFunc.OnLogFont(13));
	m_pGridLeftVisionTeach.SetItemText(nRow, 1, _T("Mark_1"));

	m_pGridLeftVisionTeach.SetItemBkColour(nRow, 2, RGB(219, 229, 241), BLACK_C);
	m_pGridLeftVisionTeach.SetItemFont(nRow, 2, &clsFunc.OnLogFont(13));
	m_pGridLeftVisionTeach.SetItemText(nRow, 2, _T("Mark_2"));

	m_pGridLeftVisionTeach.SetItemBkColour(nRow, 3, RGB(219, 229, 241), BLACK_C);
	m_pGridLeftVisionTeach.SetItemFont(nRow, 3, &clsFunc.OnLogFont(13));
	m_pGridLeftVisionTeach.SetItemText(nRow, 3, _T("Mark_3"));

	m_pGridLeftVisionTeach.SetItemBkColour(nRow, 4, RGB(219, 229, 241), BLACK_C);
	m_pGridLeftVisionTeach.SetItemFont(nRow, 4, &clsFunc.OnLogFont(13));
	m_pGridLeftVisionTeach.SetItemText(nRow, 4, _T("Mark_4"));

	m_pGridLeftVisionTeach.SetItemBkColour(nRow, 5, RGB(219, 229, 241), BLACK_C);
	m_pGridLeftVisionTeach.SetItemFont(nRow, 5, &clsFunc.OnLogFont(13));
	m_pGridLeftVisionTeach.SetItemText(nRow, 5, _T("Mark_1"));

	m_pGridLeftVisionTeach.SetItemBkColour(nRow, 6, RGB(219, 229, 241), BLACK_C);
	m_pGridLeftVisionTeach.SetItemFont(nRow, 6, &clsFunc.OnLogFont(13));
	m_pGridLeftVisionTeach.SetItemText(nRow, 6, _T("Mark_2"));

	m_pGridLeftVisionTeach.SetItemBkColour(nRow, 7, RGB(219, 229, 241), BLACK_C);
	m_pGridLeftVisionTeach.SetItemFont(nRow, 7, &clsFunc.OnLogFont(13));
	m_pGridLeftVisionTeach.SetItemText(nRow, 7, _T("Mark_3"));

	m_pGridLeftVisionTeach.SetItemBkColour(nRow, 8, RGB(219, 229, 241), BLACK_C);
	m_pGridLeftVisionTeach.SetItemFont(nRow, 8, &clsFunc.OnLogFont(13));
	m_pGridLeftVisionTeach.SetItemText(nRow, 8, _T("Mark_4"));


	for (int i =0; i<4; i++)
	{
		strTmp.Format(_T("Site_%d"),i+1);
		m_pGridLeftVisionTeach.SetItemBkColour(i+2, 0, BLUE_D, WHITE_C);
		m_pGridLeftVisionTeach.SetItemFont(i+2, 0, &clsFunc.OnLogFont(13));
		m_pGridLeftVisionTeach.SetItemText(i+2, 0, strTmp);
	}
}
void CWorkVisionTeach::OnInitRightVisionTeachGrid()
{
	int   i, j;
	int	  nMaxRow, nMaxCol;
	int   nRow;
	CString strTmp;

	nMaxRow = 6; // unloader tray y count
	nMaxCol = 9;

	m_pGridRightVisionTeach.SetFrameFocusCell(FALSE);
	m_pGridRightVisionTeach.SetTrackFocusCell(FALSE);
	m_pGridRightVisionTeach.EnableSelection(FALSE);

	m_pGridRightVisionTeach.SetGridLineColor(BLACK_C);
	m_pGridRightVisionTeach.SetGridLines(1);

	m_pGridRightVisionTeach.SetRowCount(nMaxRow);
	m_pGridRightVisionTeach.SetColumnCount(nMaxCol);

	m_pGridRightVisionTeach.SetFixedRowCount(0);
	m_pGridRightVisionTeach.SetFixedColumnCount(0);
	m_pGridRightVisionTeach.SetFixedBkColor(RGB(0,0,200));
	m_pGridRightVisionTeach.SetFixedBkColor(RGB(200,200,255));
	m_pGridRightVisionTeach.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<nMaxRow; i++) 
	{
		m_pGridRightVisionTeach.SetRowHeight(i, 19);

		for (j=0; j<nMaxCol; j++) 
		{
			m_pGridRightVisionTeach.SetColumnWidth(j, 51);

			m_pGridRightVisionTeach.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridRightVisionTeach.SetItemState(i, j, GVIS_READONLY);
			m_pGridRightVisionTeach.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
		}
	}

	nRow = 0;
	m_pGridRightVisionTeach.MergeCells(0, 0, 1,0);
	m_pGridRightVisionTeach.SetItemBkColour(nRow, 0, BLUE_D, WHITE_C);
	m_pGridRightVisionTeach.SetItemFont(nRow, 0, &clsFunc.OnLogFont(13));
	//m_pGridRightTeach.SetItemText(nRow, 0, _T("Site Num"));

	m_pGridRightVisionTeach.MergeCells(nRow, 1, nRow,4);
	m_pGridRightVisionTeach.SetItemBkColour(nRow, 1, BLUE_D, WHITE_C);
	m_pGridRightVisionTeach.SetItemFont(nRow, 1, &clsFunc.OnLogFont(13));
	m_pGridRightVisionTeach.SetItemText(nRow, 1, _T("Right Vision Teach X Pos "));

	m_pGridRightVisionTeach.MergeCells(nRow, 5, nRow,8);
	m_pGridRightVisionTeach.SetItemBkColour(nRow, 5, BLUE_D, WHITE_C);
	m_pGridRightVisionTeach.SetItemFont(nRow, 5, &clsFunc.OnLogFont(13));
	m_pGridRightVisionTeach.SetItemText(nRow, 5, _T("Right Vision Teach Y Pos "));

	nRow = 1;
	m_pGridRightVisionTeach.SetItemBkColour(nRow, 1, RGB(219, 229, 241), BLACK_C);
	m_pGridRightVisionTeach.SetItemFont(nRow, 1, &clsFunc.OnLogFont(13));
	m_pGridRightVisionTeach.SetItemText(nRow, 1, _T("Mark_1"));

	m_pGridRightVisionTeach.SetItemBkColour(nRow, 2, RGB(219, 229, 241), BLACK_C);
	m_pGridRightVisionTeach.SetItemFont(nRow, 2, &clsFunc.OnLogFont(13));
	m_pGridRightVisionTeach.SetItemText(nRow, 2, _T("Mark_2"));

	m_pGridRightVisionTeach.SetItemBkColour(nRow, 3, RGB(219, 229, 241), BLACK_C);
	m_pGridRightVisionTeach.SetItemFont(nRow, 3, &clsFunc.OnLogFont(13));
	m_pGridRightVisionTeach.SetItemText(nRow, 3, _T("Mark_3"));

	m_pGridRightVisionTeach.SetItemBkColour(nRow, 4, RGB(219, 229, 241), BLACK_C);
	m_pGridRightVisionTeach.SetItemFont(nRow, 4, &clsFunc.OnLogFont(13));
	m_pGridRightVisionTeach.SetItemText(nRow, 4, _T("Mark_4"));

	m_pGridRightVisionTeach.SetItemBkColour(nRow, 5, RGB(219, 229, 241), BLACK_C);
	m_pGridRightVisionTeach.SetItemFont(nRow, 5, &clsFunc.OnLogFont(13));
	m_pGridRightVisionTeach.SetItemText(nRow, 5, _T("Mark_1"));

	m_pGridRightVisionTeach.SetItemBkColour(nRow, 6, RGB(219, 229, 241), BLACK_C);
	m_pGridRightVisionTeach.SetItemFont(nRow, 6, &clsFunc.OnLogFont(13));
	m_pGridRightVisionTeach.SetItemText(nRow, 6, _T("Mark_2"));

	m_pGridRightVisionTeach.SetItemBkColour(nRow, 7, RGB(219, 229, 241), BLACK_C);
	m_pGridRightVisionTeach.SetItemFont(nRow, 7, &clsFunc.OnLogFont(13));
	m_pGridRightVisionTeach.SetItemText(nRow, 7, _T("Mark_3"));

	m_pGridRightVisionTeach.SetItemBkColour(nRow, 8, RGB(219, 229, 241), BLACK_C);
	m_pGridRightVisionTeach.SetItemFont(nRow, 8, &clsFunc.OnLogFont(13));
	m_pGridRightVisionTeach.SetItemText(nRow, 8, _T("Mark_4"));


	for (int i =0; i<4; i++)
	{
		strTmp.Format(_T("Site_%d"),i+5);
		m_pGridRightVisionTeach.SetItemBkColour(i+2, 0, BLUE_D, WHITE_C);
		m_pGridRightVisionTeach.SetItemFont(i+2, 0, &clsFunc.OnLogFont(13));
		m_pGridRightVisionTeach.SetItemText(i+2, 0, strTmp);
	}
}
///////////
void CWorkVisionTeach::OnInitLeftReadGrid()
{
	int   i, j;
	int	  nMaxRow, nMaxCol;
	int   nRow;
	CString strTmp;

	nMaxRow = 6; // unloader tray y count
	nMaxCol = 9;

	m_pGridTsiteLeftOffset.SetFrameFocusCell(FALSE);
	m_pGridTsiteLeftOffset.SetTrackFocusCell(FALSE);
	m_pGridTsiteLeftOffset.EnableSelection(FALSE);

	m_pGridTsiteLeftOffset.SetGridLineColor(BLACK_C);
	m_pGridTsiteLeftOffset.SetGridLines(1);

	m_pGridTsiteLeftOffset.SetRowCount(nMaxRow);
	m_pGridTsiteLeftOffset.SetColumnCount(nMaxCol);

	m_pGridTsiteLeftOffset.SetFixedRowCount(0);
	m_pGridTsiteLeftOffset.SetFixedColumnCount(0);
	m_pGridTsiteLeftOffset.SetFixedBkColor(RGB(0,0,200));
	m_pGridTsiteLeftOffset.SetFixedBkColor(RGB(200,200,255));
	m_pGridTsiteLeftOffset.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<nMaxRow; i++) 
	{
		m_pGridTsiteLeftOffset.SetRowHeight(i, 19);

		for (j=0; j<nMaxCol; j++) 
		{
			m_pGridTsiteLeftOffset.SetColumnWidth(j, 51);

			m_pGridTsiteLeftOffset.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridTsiteLeftOffset.SetItemState(i, j, GVIS_READONLY);
			m_pGridTsiteLeftOffset.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
		}
	}

	nRow = 0;
	m_pGridTsiteLeftOffset.MergeCells(0, 0, 1,0);
	m_pGridTsiteLeftOffset.SetItemBkColour(nRow, 0, BLUE_D, WHITE_C);
	m_pGridTsiteLeftOffset.SetItemFont(nRow, 0, &clsFunc.OnLogFont(13));
	//m_pGridLeftTeach.SetItemText(nRow, 0, _T("Site Num"));

	m_pGridTsiteLeftOffset.MergeCells(nRow, 1, nRow,4);
	m_pGridTsiteLeftOffset.SetItemBkColour(nRow, 1, BLUE_D, WHITE_C);
	m_pGridTsiteLeftOffset.SetItemFont(nRow, 1, &clsFunc.OnLogFont(13));
	m_pGridTsiteLeftOffset.SetItemText(nRow, 1, _T("Left Mark X Val "));

	m_pGridTsiteLeftOffset.MergeCells(nRow, 5, nRow,8);
	m_pGridTsiteLeftOffset.SetItemBkColour(nRow, 5, BLUE_D, WHITE_C);
	m_pGridTsiteLeftOffset.SetItemFont(nRow, 5, &clsFunc.OnLogFont(13));
	m_pGridTsiteLeftOffset.SetItemText(nRow, 5, _T("Left Mark Y Val "));

	nRow = 1;
	m_pGridTsiteLeftOffset.SetItemBkColour(nRow, 1, RGB(219, 229, 241), BLACK_C);
	m_pGridTsiteLeftOffset.SetItemFont(nRow, 1, &clsFunc.OnLogFont(13));
	m_pGridTsiteLeftOffset.SetItemText(nRow, 1, _T("Mark_1"));

	m_pGridTsiteLeftOffset.SetItemBkColour(nRow, 2, RGB(219, 229, 241), BLACK_C);
	m_pGridTsiteLeftOffset.SetItemFont(nRow, 2, &clsFunc.OnLogFont(13));
	m_pGridTsiteLeftOffset.SetItemText(nRow, 2, _T("Mark_2"));

	m_pGridTsiteLeftOffset.SetItemBkColour(nRow, 3, RGB(219, 229, 241), BLACK_C);
	m_pGridTsiteLeftOffset.SetItemFont(nRow, 3, &clsFunc.OnLogFont(13));
	m_pGridTsiteLeftOffset.SetItemText(nRow, 3, _T("Mark_3"));

	m_pGridTsiteLeftOffset.SetItemBkColour(nRow, 4, RGB(219, 229, 241), BLACK_C);
	m_pGridTsiteLeftOffset.SetItemFont(nRow, 4, &clsFunc.OnLogFont(13));
	m_pGridTsiteLeftOffset.SetItemText(nRow, 4, _T("Mark_4"));

	m_pGridTsiteLeftOffset.SetItemBkColour(nRow, 5, RGB(219, 229, 241), BLACK_C);
	m_pGridTsiteLeftOffset.SetItemFont(nRow, 5, &clsFunc.OnLogFont(13));
	m_pGridTsiteLeftOffset.SetItemText(nRow, 5, _T("Mark_1"));

	m_pGridTsiteLeftOffset.SetItemBkColour(nRow, 6, RGB(219, 229, 241), BLACK_C);
	m_pGridTsiteLeftOffset.SetItemFont(nRow, 6, &clsFunc.OnLogFont(13));
	m_pGridTsiteLeftOffset.SetItemText(nRow, 6, _T("Mark_2"));

	m_pGridTsiteLeftOffset.SetItemBkColour(nRow, 7, RGB(219, 229, 241), BLACK_C);
	m_pGridTsiteLeftOffset.SetItemFont(nRow, 7, &clsFunc.OnLogFont(13));
	m_pGridTsiteLeftOffset.SetItemText(nRow, 7, _T("Mark_3"));

	m_pGridTsiteLeftOffset.SetItemBkColour(nRow, 8, RGB(219, 229, 241), BLACK_C);
	m_pGridTsiteLeftOffset.SetItemFont(nRow, 8, &clsFunc.OnLogFont(13));
	m_pGridTsiteLeftOffset.SetItemText(nRow, 8, _T("Mark_4"));


	for (int i =0; i<4; i++)
	{
		strTmp.Format(_T("Site_%d"),i+1);
		m_pGridTsiteLeftOffset.SetItemBkColour(i+2, 0, BLUE_D, WHITE_C);
		m_pGridTsiteLeftOffset.SetItemFont(i+2, 0, &clsFunc.OnLogFont(13));
		m_pGridTsiteLeftOffset.SetItemText(i+2, 0, strTmp);
	}
}
void CWorkVisionTeach::OnInitRightReadGrid()
{
	int   i, j;
	int	  nMaxRow, nMaxCol;
	int   nRow;
	CString strTmp;

	nMaxRow = 6; // unloader tray y count
	nMaxCol = 9;

	m_pGridTsiteRightOffset.SetFrameFocusCell(FALSE);
	m_pGridTsiteRightOffset.SetTrackFocusCell(FALSE);
	m_pGridTsiteRightOffset.EnableSelection(FALSE);

	m_pGridTsiteRightOffset.SetGridLineColor(BLACK_C);
	m_pGridTsiteRightOffset.SetGridLines(1);

	m_pGridTsiteRightOffset.SetRowCount(nMaxRow);
	m_pGridTsiteRightOffset.SetColumnCount(nMaxCol);

	m_pGridTsiteRightOffset.SetFixedRowCount(0);
	m_pGridTsiteRightOffset.SetFixedColumnCount(0);
	m_pGridTsiteRightOffset.SetFixedBkColor(RGB(0,0,200));
	m_pGridTsiteRightOffset.SetFixedBkColor(RGB(200,200,255));
	m_pGridTsiteRightOffset.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<nMaxRow; i++) 
	{
		m_pGridTsiteRightOffset.SetRowHeight(i, 19);

		for (j=0; j<nMaxCol; j++) 
		{
			m_pGridTsiteRightOffset.SetColumnWidth(j, 51);

			m_pGridTsiteRightOffset.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridTsiteRightOffset.SetItemState(i, j, GVIS_READONLY);
			m_pGridTsiteRightOffset.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
		}
	}

	nRow = 0;
	m_pGridTsiteRightOffset.MergeCells(0, 0, 1,0);
	m_pGridTsiteRightOffset.SetItemBkColour(nRow, 0, BLUE_D, WHITE_C);
	m_pGridTsiteRightOffset.SetItemFont(nRow, 0, &clsFunc.OnLogFont(13));
	//m_pGridLeftTeach.SetItemText(nRow, 0, _T("Site Num"));

	m_pGridTsiteRightOffset.MergeCells(nRow, 1, nRow,4);
	m_pGridTsiteRightOffset.SetItemBkColour(nRow, 1, BLUE_D, WHITE_C);
	m_pGridTsiteRightOffset.SetItemFont(nRow, 1, &clsFunc.OnLogFont(13));
	m_pGridTsiteRightOffset.SetItemText(nRow, 1, _T("Right Mark X Val "));

	m_pGridTsiteRightOffset.MergeCells(nRow, 5, nRow,8);
	m_pGridTsiteRightOffset.SetItemBkColour(nRow, 5, BLUE_D, WHITE_C);
	m_pGridTsiteRightOffset.SetItemFont(nRow, 5, &clsFunc.OnLogFont(13));
	m_pGridTsiteRightOffset.SetItemText(nRow, 5, _T("Right Mark Y Val "));

	nRow = 1;
	m_pGridTsiteRightOffset.SetItemBkColour(nRow, 1, RGB(219, 229, 241), BLACK_C);
	m_pGridTsiteRightOffset.SetItemFont(nRow, 1, &clsFunc.OnLogFont(13));
	m_pGridTsiteRightOffset.SetItemText(nRow, 1, _T("Mark_1"));

	m_pGridTsiteRightOffset.SetItemBkColour(nRow, 2, RGB(219, 229, 241), BLACK_C);
	m_pGridTsiteRightOffset.SetItemFont(nRow, 2, &clsFunc.OnLogFont(13));
	m_pGridTsiteRightOffset.SetItemText(nRow, 2, _T("Mark_2"));

	m_pGridTsiteRightOffset.SetItemBkColour(nRow, 3, RGB(219, 229, 241), BLACK_C);
	m_pGridTsiteRightOffset.SetItemFont(nRow, 3, &clsFunc.OnLogFont(13));
	m_pGridTsiteRightOffset.SetItemText(nRow, 3, _T("Mark_3"));

	m_pGridTsiteRightOffset.SetItemBkColour(nRow, 4, RGB(219, 229, 241), BLACK_C);
	m_pGridTsiteRightOffset.SetItemFont(nRow, 4, &clsFunc.OnLogFont(13));
	m_pGridTsiteRightOffset.SetItemText(nRow, 4, _T("Mark_4"));

	m_pGridTsiteRightOffset.SetItemBkColour(nRow, 5, RGB(219, 229, 241), BLACK_C);
	m_pGridTsiteRightOffset.SetItemFont(nRow, 5, &clsFunc.OnLogFont(13));
	m_pGridTsiteRightOffset.SetItemText(nRow, 5, _T("Mark_1"));

	m_pGridTsiteRightOffset.SetItemBkColour(nRow, 6, RGB(219, 229, 241), BLACK_C);
	m_pGridTsiteRightOffset.SetItemFont(nRow, 6, &clsFunc.OnLogFont(13));
	m_pGridTsiteRightOffset.SetItemText(nRow, 6, _T("Mark_2"));

	m_pGridTsiteRightOffset.SetItemBkColour(nRow, 7, RGB(219, 229, 241), BLACK_C);
	m_pGridTsiteRightOffset.SetItemFont(nRow, 7, &clsFunc.OnLogFont(13));
	m_pGridTsiteRightOffset.SetItemText(nRow, 7, _T("Mark_3"));

	m_pGridTsiteRightOffset.SetItemBkColour(nRow, 8, RGB(219, 229, 241), BLACK_C);
	m_pGridTsiteRightOffset.SetItemFont(nRow, 8, &clsFunc.OnLogFont(13));
	m_pGridTsiteRightOffset.SetItemText(nRow, 8, _T("Mark_4"));


	for (int i =0; i<4; i++)
	{
		strTmp.Format(_T("Site_%d"),i+5);
		m_pGridTsiteRightOffset.SetItemBkColour(i+2, 0, BLUE_D, WHITE_C);
		m_pGridTsiteRightOffset.SetItemFont(i+2, 0, &clsFunc.OnLogFont(13));
		m_pGridTsiteRightOffset.SetItemText(i+2, 0, strTmp);
	}

}
void CWorkVisionTeach::OnInitLeftMarkTeachGrid()
{
	int   i, j;
	int	  nMaxRow, nMaxCol;
	int   nRow;
	CString strTmp;

	nMaxRow = 6; // unloader tray y count
	nMaxCol = 10;

	m_pGridLeftTeach.SetFrameFocusCell(FALSE);
	m_pGridLeftTeach.SetTrackFocusCell(FALSE);
	m_pGridLeftTeach.EnableSelection(FALSE);

	m_pGridLeftTeach.SetGridLineColor(BLACK_C);
	m_pGridLeftTeach.SetGridLines(1);

	m_pGridLeftTeach.SetRowCount(nMaxRow);
	m_pGridLeftTeach.SetColumnCount(nMaxCol);

	m_pGridLeftTeach.SetFixedRowCount(0);
	m_pGridLeftTeach.SetFixedColumnCount(0);
	m_pGridLeftTeach.SetFixedBkColor(RGB(0,0,200));
	m_pGridLeftTeach.SetFixedBkColor(RGB(200,200,255));
	m_pGridLeftTeach.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<nMaxRow; i++) 
	{
		m_pGridLeftTeach.SetRowHeight(i, 19);

		for (j=0; j<nMaxCol; j++) 
		{
			m_pGridLeftTeach.SetColumnWidth(j, 62);

			m_pGridLeftTeach.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridLeftTeach.SetItemState(i, j, GVIS_READONLY);
			m_pGridLeftTeach.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
		}
	}

	nRow = 0;
	m_pGridLeftTeach.MergeCells(0, 0, 1,0);
	m_pGridLeftTeach.SetItemBkColour(nRow, 0, BLUE_D, WHITE_C);
	m_pGridLeftTeach.SetItemFont(nRow, 0, &clsFunc.OnLogFont(13));
	//m_pGridLeftTeach.SetItemText(nRow, 0, _T("Site Num"));

	m_pGridLeftTeach.MergeCells(nRow, 1, nRow,4);
	m_pGridLeftTeach.SetItemBkColour(nRow, 1, BLUE_D, WHITE_C);
	m_pGridLeftTeach.SetItemFont(nRow, 1, &clsFunc.OnLogFont(13));
	m_pGridLeftTeach.SetItemText(nRow, 1, _T("Left Teach X Pos "));

	m_pGridLeftTeach.MergeCells(nRow, 5, nRow,8);
	m_pGridLeftTeach.SetItemBkColour(nRow, 5, BLUE_D, WHITE_C);
	m_pGridLeftTeach.SetItemFont(nRow, 5, &clsFunc.OnLogFont(13));
	m_pGridLeftTeach.SetItemText(nRow, 5, _T("Left Teach Y Pos "));

	m_pGridLeftTeach.SetItemBkColour(nRow, 9, BLUE_D, WHITE_C);
	m_pGridLeftTeach.SetItemFont(nRow, 9, &clsFunc.OnLogFont(13));
	m_pGridLeftTeach.SetItemText(nRow, 9, _T("Z Pos "));

	nRow = 1;
	m_pGridLeftTeach.SetItemBkColour(nRow, 1, RGB(219, 229, 241), BLACK_C);
	m_pGridLeftTeach.SetItemFont(nRow, 1, &clsFunc.OnLogFont(13));
	m_pGridLeftTeach.SetItemText(nRow, 1, _T("Mark_1"));

	m_pGridLeftTeach.SetItemBkColour(nRow, 2, RGB(219, 229, 241), BLACK_C);
	m_pGridLeftTeach.SetItemFont(nRow, 2, &clsFunc.OnLogFont(13));
	m_pGridLeftTeach.SetItemText(nRow, 2, _T("Mark_2"));

	m_pGridLeftTeach.SetItemBkColour(nRow, 3, RGB(219, 229, 241), BLACK_C);
	m_pGridLeftTeach.SetItemFont(nRow, 3, &clsFunc.OnLogFont(13));
	m_pGridLeftTeach.SetItemText(nRow, 3, _T("Mark_3"));

	m_pGridLeftTeach.SetItemBkColour(nRow, 4, RGB(219, 229, 241), BLACK_C);
	m_pGridLeftTeach.SetItemFont(nRow, 4, &clsFunc.OnLogFont(13));
	m_pGridLeftTeach.SetItemText(nRow, 4, _T("Mark_4"));

	m_pGridLeftTeach.SetItemBkColour(nRow, 5, RGB(219, 229, 241), BLACK_C);
	m_pGridLeftTeach.SetItemFont(nRow, 5, &clsFunc.OnLogFont(13));
	m_pGridLeftTeach.SetItemText(nRow, 5, _T("Mark_1"));

	m_pGridLeftTeach.SetItemBkColour(nRow, 6, RGB(219, 229, 241), BLACK_C);
	m_pGridLeftTeach.SetItemFont(nRow, 6, &clsFunc.OnLogFont(13));
	m_pGridLeftTeach.SetItemText(nRow, 6, _T("Mark_2"));

	m_pGridLeftTeach.SetItemBkColour(nRow, 7, RGB(219, 229, 241), BLACK_C);
	m_pGridLeftTeach.SetItemFont(nRow, 7, &clsFunc.OnLogFont(13));
	m_pGridLeftTeach.SetItemText(nRow, 7, _T("Mark_3"));

	m_pGridLeftTeach.SetItemBkColour(nRow, 8, RGB(219, 229, 241), BLACK_C);
	m_pGridLeftTeach.SetItemFont(nRow, 8, &clsFunc.OnLogFont(13));
	m_pGridLeftTeach.SetItemText(nRow, 8, _T("Mark_4"));

	m_pGridLeftTeach.SetItemBkColour(nRow, 9, RGB(219, 229, 241), BLACK_C);
	m_pGridLeftTeach.SetItemFont(nRow, 9, &clsFunc.OnLogFont(13));
	m_pGridLeftTeach.SetItemText(nRow, 9, _T(""));

	for (int i =0; i<4; i++)
	{
		strTmp.Format(_T("Site_%d"),i+1);
		m_pGridLeftTeach.SetItemBkColour(i+2, 0, BLUE_D, WHITE_C);
		m_pGridLeftTeach.SetItemFont(i+2, 0, &clsFunc.OnLogFont(13));
		m_pGridLeftTeach.SetItemText(i+2, 0, strTmp);
	}
}

void CWorkVisionTeach::OnInitRightMarkTeachGrid()
{
	int   i, j;
	int	  nMaxRow, nMaxCol;
	int   nRow;
	CString strTmp;

	nMaxRow = 6; // unloader tray y count
	nMaxCol = 10;

	m_pGridRightTeach.SetFrameFocusCell(FALSE);
	m_pGridRightTeach.SetTrackFocusCell(FALSE);
	m_pGridRightTeach.EnableSelection(FALSE);

	m_pGridRightTeach.SetGridLineColor(BLACK_C);
	m_pGridRightTeach.SetGridLines(1);

	m_pGridRightTeach.SetRowCount(nMaxRow);
	m_pGridRightTeach.SetColumnCount(nMaxCol);

	m_pGridRightTeach.SetFixedRowCount(0);
	m_pGridRightTeach.SetFixedColumnCount(0);
	m_pGridRightTeach.SetFixedBkColor(RGB(0,0,200));
	m_pGridRightTeach.SetFixedBkColor(RGB(200,200,255));
	m_pGridRightTeach.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<nMaxRow; i++) 
	{
		m_pGridRightTeach.SetRowHeight(i, 19);

		for (j=0; j<nMaxCol; j++) 
		{
			m_pGridRightTeach.SetColumnWidth(j, 62);

			m_pGridRightTeach.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridRightTeach.SetItemState(i, j, GVIS_READONLY);
			m_pGridRightTeach.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
		}
	}

	nRow = 0;
	m_pGridRightTeach.MergeCells(0, 0, 1,0);
	m_pGridRightTeach.SetItemBkColour(nRow, 0, BLUE_D, WHITE_C);
	m_pGridRightTeach.SetItemFont(nRow, 0, &clsFunc.OnLogFont(13));
	//m_pGridRightTeach.SetItemText(nRow, 0, _T("Site Num"));

	m_pGridRightTeach.MergeCells(nRow, 1, nRow,4);
	m_pGridRightTeach.SetItemBkColour(nRow, 1, BLUE_D, WHITE_C);
	m_pGridRightTeach.SetItemFont(nRow, 1, &clsFunc.OnLogFont(13));
	m_pGridRightTeach.SetItemText(nRow, 1, _T("Right Teach X Pos "));

	m_pGridRightTeach.MergeCells(nRow, 5, nRow,8);
	m_pGridRightTeach.SetItemBkColour(nRow, 5, BLUE_D, WHITE_C);
	m_pGridRightTeach.SetItemFont(nRow, 5, &clsFunc.OnLogFont(13));
	m_pGridRightTeach.SetItemText(nRow, 5, _T("Right Teach Y Pos "));

	//
	m_pGridRightTeach.SetItemBkColour(nRow, 9, BLUE_D, WHITE_C);
	m_pGridRightTeach.SetItemFont(nRow, 9, &clsFunc.OnLogFont(13));
	m_pGridRightTeach.SetItemText(nRow, 9, _T("Z Pos "));

	nRow = 1;
	m_pGridRightTeach.SetItemBkColour(nRow, 1, RGB(219, 229, 241), BLACK_C);
	m_pGridRightTeach.SetItemFont(nRow, 1, &clsFunc.OnLogFont(13));
	m_pGridRightTeach.SetItemText(nRow, 1, _T("Mark_1"));

	m_pGridRightTeach.SetItemBkColour(nRow, 2, RGB(219, 229, 241), BLACK_C);
	m_pGridRightTeach.SetItemFont(nRow, 2, &clsFunc.OnLogFont(13));
	m_pGridRightTeach.SetItemText(nRow, 2, _T("Mark_2"));

	m_pGridRightTeach.SetItemBkColour(nRow, 3, RGB(219, 229, 241), BLACK_C);
	m_pGridRightTeach.SetItemFont(nRow, 3, &clsFunc.OnLogFont(13));
	m_pGridRightTeach.SetItemText(nRow, 3, _T("Mark_3"));

	m_pGridRightTeach.SetItemBkColour(nRow, 4, RGB(219, 229, 241), BLACK_C);
	m_pGridRightTeach.SetItemFont(nRow, 4, &clsFunc.OnLogFont(13));
	m_pGridRightTeach.SetItemText(nRow, 4, _T("Mark_4"));

	m_pGridRightTeach.SetItemBkColour(nRow, 5, RGB(219, 229, 241), BLACK_C);
	m_pGridRightTeach.SetItemFont(nRow, 5, &clsFunc.OnLogFont(13));
	m_pGridRightTeach.SetItemText(nRow, 5, _T("Mark_1"));

	m_pGridRightTeach.SetItemBkColour(nRow, 6, RGB(219, 229, 241), BLACK_C);
	m_pGridRightTeach.SetItemFont(nRow, 6, &clsFunc.OnLogFont(13));
	m_pGridRightTeach.SetItemText(nRow, 6, _T("Mark_2"));

	m_pGridRightTeach.SetItemBkColour(nRow, 7, RGB(219, 229, 241), BLACK_C);
	m_pGridRightTeach.SetItemFont(nRow, 7, &clsFunc.OnLogFont(13));
	m_pGridRightTeach.SetItemText(nRow, 7, _T("Mark_3"));

	m_pGridRightTeach.SetItemBkColour(nRow, 8, RGB(219, 229, 241), BLACK_C);
	m_pGridRightTeach.SetItemFont(nRow, 8, &clsFunc.OnLogFont(13));
	m_pGridRightTeach.SetItemText(nRow, 8, _T("Mark_4"));

	m_pGridRightTeach.SetItemBkColour(nRow, 9, RGB(219, 229, 241), BLACK_C);
	m_pGridRightTeach.SetItemFont(nRow, 9, &clsFunc.OnLogFont(13));
	m_pGridRightTeach.SetItemText(nRow, 9, _T(""));

	for (int i =0; i<4; i++)
	{
		strTmp.Format(_T("Site_%d"),i+5);
		m_pGridRightTeach.SetItemBkColour(i+2, 0, BLUE_D, WHITE_C);
		m_pGridRightTeach.SetItemFont(i+2, 0, &clsFunc.OnLogFont(13));
		m_pGridRightTeach.SetItemText(i+2, 0, strTmp);
	}
}
//kwlee 2017.0109
void CWorkVisionTeach::OnInitLeftVisionTeachValue()
{
	int   i, j;
	int	  nMaxRow, nMaxCol;
	int   nRow;
	CString strTmp;

	nMaxRow = 6; // unloader tray y count
	nMaxCol = 9;

	m_pGridLeftValue.SetFrameFocusCell(FALSE);
	m_pGridLeftValue.SetTrackFocusCell(FALSE);
	m_pGridLeftValue.EnableSelection(FALSE);

	m_pGridLeftValue.SetGridLineColor(BLACK_C);
	m_pGridLeftValue.SetGridLines(1);

	m_pGridLeftValue.SetRowCount(nMaxRow);
	m_pGridLeftValue.SetColumnCount(nMaxCol);

	m_pGridLeftValue.SetFixedRowCount(0);
	m_pGridLeftValue.SetFixedColumnCount(0);
	m_pGridLeftValue.SetFixedBkColor(RGB(0,0,200));
	m_pGridLeftValue.SetFixedBkColor(RGB(200,200,255));
	m_pGridLeftValue.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<nMaxRow; i++) 
	{
		m_pGridLeftValue.SetRowHeight(i, 19);

		for (j=0; j<nMaxCol; j++) 
		{
			m_pGridLeftValue.SetColumnWidth(j, 51);

			m_pGridLeftValue.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridLeftValue.SetItemState(i, j, GVIS_READONLY);
			m_pGridLeftValue.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
		}
	}

	nRow = 0;
	m_pGridLeftValue.MergeCells(0, 0, 1,0);
	m_pGridLeftValue.SetItemBkColour(nRow, 0, BLUE_D, WHITE_C);
	m_pGridLeftValue.SetItemFont(nRow, 0, &clsFunc.OnLogFont(13));
	//m_pGridRightTeach.SetItemText(nRow, 0, _T("Site Num"));

	m_pGridLeftValue.MergeCells(nRow, 1, nRow,4);
	m_pGridLeftValue.SetItemBkColour(nRow, 1, BLUE_D, WHITE_C);
	m_pGridLeftValue.SetItemFont(nRow, 1, &clsFunc.OnLogFont(13));
	//m_pGridLeftValue.SetItemText(nRow, 1, _T("Teach Value Left X Pos "));
	m_pGridLeftValue.SetItemText(nRow, 1, _T("Mark Center Left X Pos "));

	m_pGridLeftValue.MergeCells(nRow, 5, nRow,8);
	m_pGridLeftValue.SetItemBkColour(nRow, 5, BLUE_D, WHITE_C);
	m_pGridLeftValue.SetItemFont(nRow, 5, &clsFunc.OnLogFont(13));
	//m_pGridLeftValue.SetItemText(nRow, 5, _T("Teach Value Left Y Pos "));
	m_pGridLeftValue.SetItemText(nRow, 5, _T("Mark Center Left Y Pos "));

	nRow = 1;
	m_pGridLeftValue.SetItemBkColour(nRow, 1, RGB(219, 229, 241), BLACK_C);
	m_pGridLeftValue.SetItemFont(nRow, 1, &clsFunc.OnLogFont(13));
	m_pGridLeftValue.SetItemText(nRow, 1, _T("Mark_1"));

	m_pGridLeftValue.SetItemBkColour(nRow, 2, RGB(219, 229, 241), BLACK_C);
	m_pGridLeftValue.SetItemFont(nRow, 2, &clsFunc.OnLogFont(13));
	m_pGridLeftValue.SetItemText(nRow, 2, _T("Mark_2"));

	m_pGridLeftValue.SetItemBkColour(nRow, 3, RGB(219, 229, 241), BLACK_C);
	m_pGridLeftValue.SetItemFont(nRow, 3, &clsFunc.OnLogFont(13));
	m_pGridLeftValue.SetItemText(nRow, 3, _T("Mark_3"));

	m_pGridLeftValue.SetItemBkColour(nRow, 4, RGB(219, 229, 241), BLACK_C);
	m_pGridLeftValue.SetItemFont(nRow, 4, &clsFunc.OnLogFont(13));
	m_pGridLeftValue.SetItemText(nRow, 4, _T("Mark_4"));

	m_pGridLeftValue.SetItemBkColour(nRow, 5, RGB(219, 229, 241), BLACK_C);
	m_pGridLeftValue.SetItemFont(nRow, 5, &clsFunc.OnLogFont(13));
	m_pGridLeftValue.SetItemText(nRow, 5, _T("Mark_1"));

	m_pGridLeftValue.SetItemBkColour(nRow, 6, RGB(219, 229, 241), BLACK_C);
	m_pGridLeftValue.SetItemFont(nRow, 6, &clsFunc.OnLogFont(13));
	m_pGridLeftValue.SetItemText(nRow, 6, _T("Mark_2"));

	m_pGridLeftValue.SetItemBkColour(nRow, 7, RGB(219, 229, 241), BLACK_C);
	m_pGridLeftValue.SetItemFont(nRow, 7, &clsFunc.OnLogFont(13));
	m_pGridLeftValue.SetItemText(nRow, 7, _T("Mark_3"));

	m_pGridLeftValue.SetItemBkColour(nRow, 8, RGB(219, 229, 241), BLACK_C);
	m_pGridLeftValue.SetItemFont(nRow, 8, &clsFunc.OnLogFont(13));
	m_pGridLeftValue.SetItemText(nRow, 8, _T("Mark_4"));


	for (int i =0; i<4; i++)
	{
		strTmp.Format(_T("Site_%d"),i + 1);
		m_pGridLeftValue.SetItemBkColour(i+2, 0, BLUE_D, WHITE_C);
		m_pGridLeftValue.SetItemFont(i+2, 0, &clsFunc.OnLogFont(13));
		m_pGridLeftValue.SetItemText(i+2, 0, strTmp);
	}
}

void CWorkVisionTeach::OnInitRightVisionTeachValue()
{
	int   i, j;
	int	  nMaxRow, nMaxCol;
	int   nRow;
	CString strTmp;

	nMaxRow = 6; // unloader tray y count
	nMaxCol = 9;

	m_pGridRightValue.SetFrameFocusCell(FALSE);
	m_pGridRightValue.SetTrackFocusCell(FALSE);
	m_pGridRightValue.EnableSelection(FALSE);

	m_pGridRightValue.SetGridLineColor(BLACK_C);
	m_pGridRightValue.SetGridLines(1);

	m_pGridRightValue.SetRowCount(nMaxRow);
	m_pGridRightValue.SetColumnCount(nMaxCol);

	m_pGridRightValue.SetFixedRowCount(0);
	m_pGridRightValue.SetFixedColumnCount(0);
	m_pGridRightValue.SetFixedBkColor(RGB(0,0,200));
	m_pGridRightValue.SetFixedBkColor(RGB(200,200,255));
	m_pGridRightValue.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<nMaxRow; i++) 
	{
		m_pGridRightValue.SetRowHeight(i, 19);

		for (j=0; j<nMaxCol; j++) 
		{
			m_pGridRightValue.SetColumnWidth(j, 51);

			m_pGridRightValue.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridRightValue.SetItemState(i, j, GVIS_READONLY);
			m_pGridRightValue.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
		}
	}

	nRow = 0;
	m_pGridRightValue.MergeCells(0, 0, 1,0);
	m_pGridRightValue.SetItemBkColour(nRow, 0, BLUE_D, WHITE_C);
	m_pGridRightValue.SetItemFont(nRow, 0, &clsFunc.OnLogFont(13));
	//m_pGridRightTeach.SetItemText(nRow, 0, _T("Site Num"));

	m_pGridRightValue.MergeCells(nRow, 1, nRow,4);
	m_pGridRightValue.SetItemBkColour(nRow, 1, BLUE_D, WHITE_C);
	m_pGridRightValue.SetItemFont(nRow, 1, &clsFunc.OnLogFont(13));
	//m_pGridRightValue.SetItemText(nRow, 1, _T("Teach Value Right X Pos "));
	m_pGridRightValue.SetItemText(nRow, 1, _T("Mark Center Right X Pos "));

	m_pGridRightValue.MergeCells(nRow, 5, nRow,8);
	m_pGridRightValue.SetItemBkColour(nRow, 5, BLUE_D, WHITE_C);
	m_pGridRightValue.SetItemFont(nRow, 5, &clsFunc.OnLogFont(13));
	//m_pGridRightValue.SetItemText(nRow, 5, _T("Teach Value Right Y Pos "));
	m_pGridRightValue.SetItemText(nRow, 5, _T("Mark Center Right Y Pos "));

	nRow = 1;
	m_pGridRightValue.SetItemBkColour(nRow, 1, RGB(219, 229, 241), BLACK_C);
	m_pGridRightValue.SetItemFont(nRow, 1, &clsFunc.OnLogFont(13));
	m_pGridRightValue.SetItemText(nRow, 1, _T("Mark_1"));

	m_pGridRightValue.SetItemBkColour(nRow, 2, RGB(219, 229, 241), BLACK_C);
	m_pGridRightValue.SetItemFont(nRow, 2, &clsFunc.OnLogFont(13));
	m_pGridRightValue.SetItemText(nRow, 2, _T("Mark_2"));

	m_pGridRightValue.SetItemBkColour(nRow, 3, RGB(219, 229, 241), BLACK_C);
	m_pGridRightValue.SetItemFont(nRow, 3, &clsFunc.OnLogFont(13));
	m_pGridRightValue.SetItemText(nRow, 3, _T("Mark_3"));

	m_pGridRightValue.SetItemBkColour(nRow, 4, RGB(219, 229, 241), BLACK_C);
	m_pGridRightValue.SetItemFont(nRow, 4, &clsFunc.OnLogFont(13));
	m_pGridRightValue.SetItemText(nRow, 4, _T("Mark_4"));

	m_pGridRightValue.SetItemBkColour(nRow, 5, RGB(219, 229, 241), BLACK_C);
	m_pGridRightValue.SetItemFont(nRow, 5, &clsFunc.OnLogFont(13));
	m_pGridRightValue.SetItemText(nRow, 5, _T("Mark_1"));

	m_pGridRightValue.SetItemBkColour(nRow, 6, RGB(219, 229, 241), BLACK_C);
	m_pGridRightValue.SetItemFont(nRow, 6, &clsFunc.OnLogFont(13));
	m_pGridRightValue.SetItemText(nRow, 6, _T("Mark_2"));

	m_pGridRightValue.SetItemBkColour(nRow, 7, RGB(219, 229, 241), BLACK_C);
	m_pGridRightValue.SetItemFont(nRow, 7, &clsFunc.OnLogFont(13));
	m_pGridRightValue.SetItemText(nRow, 7, _T("Mark_3"));

	m_pGridRightValue.SetItemBkColour(nRow, 8, RGB(219, 229, 241), BLACK_C);
	m_pGridRightValue.SetItemFont(nRow, 8, &clsFunc.OnLogFont(13));
	m_pGridRightValue.SetItemText(nRow, 8, _T("Mark_4"));


	for (int i =0; i<4; i++)
	{
		strTmp.Format(_T("Site_%d"),i + 5);
		m_pGridRightValue.SetItemBkColour(i+2, 0, BLUE_D, WHITE_C);
		m_pGridRightValue.SetItemFont(i+2, 0, &clsFunc.OnLogFont(13));
		m_pGridRightValue.SetItemText(i+2, 0, strTmp);
	}
}
///////////////////////////////
int CWorkVisionTeach::ValueSetMove()
{
	int nFuncRet = RET_PROCEED;
	int nRet_1;	
	int nFind_JobPos =0; //작업할 위치를 찾는다

	int m_nRobot_Z = M_TEST_RBT_Z; 
	int m_nRobot_X = M_TEST_RBT_X;
	int m_nRobot_Y = M_TEST_RBT_Y;
	int m_nRobot_Ejector = M_TEST_DVC_EJECT_Z;

	CString strLogX, strLogY;

	double m_dpTargetPosXY[2];
	double m_dCurrentPos;
	long m_lpAxisNum[4];
	int m_nLinearMove_Index = 2;   //현재 map index
	int m_lAxisCnt	= 2; 

	m_lpAxisNum[0] = M_TEST_RBT_Y; //m_lpAxisNum[4];        //현재의 IndexNum에서 사용하는 모터 실재 번호를 가진다
	m_lpAxisNum[1] = M_TEST_RBT_X;
	m_lpAxisNum[2] = M_TEST_RBT_Z;
	m_lpAxisNum[3] = M_TEST_DVC_EJECT_Z;

	m_dCurrentPos = 0;
	m_dpTargetPosXY[0] = 0;
	m_dpTargetPosXY[1] = 0;

	switch(m_nValueSetStep)
	{
	case 0:	
		m_nValueSetStep = 100;
		break;

	case 100:
		m_nValueSetStep = 900;
		break;

	case 900:
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, M_TEST_RBT_Z, st_motor_info[M_TEST_RBT_Z].d_pos[P_TESTRBT_Z_SAFETY], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_nValueSetStep = 910;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nValueSetStep = 900;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(m_nValueSetStep, dWARNING, st_alarm_info.strCode);
			clsMem.OnAbNormalMessagWrite(st_alarm_info.strCode);
			nFuncRet = RET_ERROR;
		}
		break;


	case 910:
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, M_TEST_DVC_EJECT_Z, st_motor_info[M_TEST_DVC_EJECT_Z].d_pos[P_DVC_EJECTOR_Z_SAFETY], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_nValueSetStep = 920;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nValueSetStep = 910;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(m_nValueSetStep, dWARNING, st_alarm_info.strCode);
			clsMem.OnAbNormalMessagWrite(st_alarm_info.strCode);
			nFuncRet = RET_ERROR;
		}
		break;

	case 920:
		m_nValueSetStep = 1000;
		break; 

		///////////////////////////////////////////////////////////////////
		// 작업 조건을 확인한다 
		///////////////////////////////////////////////////////////////////
	case 1000:
		m_dCurrentPos = COMI.Get_MotCurrentPos(M_TEST_RBT_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos > st_motor_info[M_TEST_RBT_Z].d_pos[P_TESTRBT_Z_SAFETY] + st_motor_info[M_TEST_RBT_Z].d_allow) //안전 위치를 벗어나 있으면 정위치 시킨다
		{
			m_nValueSetStep = 900;
			break;
		}

		m_dCurrentPos = COMI.Get_MotCurrentPos(M_TEST_DVC_EJECT_Z);  // 모터 특정 축의 현재 위치 리턴 함수
		if(m_dCurrentPos > st_motor_info[M_TEST_DVC_EJECT_Z].d_pos[P_DVC_EJECTOR_Z_SAFETY] + st_motor_info[M_TEST_DVC_EJECT_Z].d_allow) //안전 위치를 벗어나 있으면 정위치 시킨다
		{
			m_nValueSetStep = 910;
			break;
		}
		m_nValueSetStep = 2000;		
		break;

	case 2000:

		// 		//kwlee 2017.0104
		if( ( m_nSite_No == 1 && m_iVisionPos >= 2 ) ||
			( m_nSite_No == 2 && m_iVisionPos >= 2 ) ||
			( m_nSite_No == 3 && m_iVisionPos >= 2 ) ||
			( m_nSite_No == 5 && m_iVisionPos >= 2 ) ||
			( m_nSite_No == 6 && m_iVisionPos >= 2 ) ||
			( m_nSite_No == 7 && m_iVisionPos >= 2 ) )
		{
			m_nValueSetStep = 0;
			break;
		}

		if( m_nSite_No < 4)//LEFT 사이트
		{//마크가 있는 위치

			m_dpTargetPosXY[1] = st_teach.m_dLeftVisionTeachXPos[m_nSite_No][m_iVisionPos] + st_teach.m_dLeftVisionOffSetXPos[m_nSite_No][m_iVisionPos]; //X Pos
			m_dpTargetPosXY[0] = st_teach.m_dLeftVisionTeachYPos[m_nSite_No][m_iVisionPos] + st_teach.m_dLeftVisionOffSetYPos[m_nSite_No][m_iVisionPos];//Y Pos


		}
		else//Right 사이트
		{
			m_dpTargetPosXY[1] = st_teach.m_dRightVisionTeachXPos[m_nSite_No - 4][m_iVisionPos] + st_teach.m_dRightVisionOffSetXPos[m_nSite_No - 4][m_iVisionPos]; //X Pos
			m_dpTargetPosXY[0] = st_teach.m_dRightVisionTeachYPos[m_nSite_No - 4][m_iVisionPos] + st_teach.m_dRightVisionOffSetYPos[m_nSite_No - 4][m_iVisionPos]; //Y Pos
		}


		//kwlee 2017.0105
		if (m_nSimul == 1)
		{
			nRet_1 = BD_GOOD;
		}
		else
		{
			nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosXY, COMI.mn_runspeed_rate);
		}
		if(nRet_1 == BD_GOOD)
		{
			strLogX.Format( _T("[Site %d_%02d] X Pos : [%3.3f]"), m_nSite_No+1, m_iVisionPos + 1,  m_dpTargetPosXY[0]);
			strLogY.Format( _T("[Site %d_%02d] Y Pos : [%3.3f]"), m_nSite_No+1,  m_iVisionPos + 1, m_dpTargetPosXY[1]);

			clsMem.OnAbNormalMessagWrite(strLogX);
			clsMem.OnAbNormalMessagWrite(strLogY);
			m_nValueSetStep = 2100;
		}
		else if(nRet_1 == BD_ERROR)
		{			
			CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);
			nFuncRet = RET_ERROR;
			m_nValueSetStep = 2000;
		}
		break; 	

	case 2100:
		m_nValueSetStep = 0;
		nFuncRet = RET_GOOD;
		break;
	default : 
		break;
	}

	return nFuncRet;
}
int CWorkVisionTeach::VisionFocus_Move( int nSite )
{

	//kwlee 2016.0913 Vision 관련 
	//return RET_GOOD;
	int nFuncRet = RET_PROCEED;
	int nRet_1=0, nRet_2=0;
	int iXPos=0, iYPos=0, nFlag=0,nPos=0;
	
	CString str,str_1;
	int nTemp = 0;
	double m_dpTargetPosXY[2];
	long m_lpAxisNum[4];
	//double m_dpSpdRatio[3];

	int m_nRobot_Z = M_TEST_RBT_Z; 
	int m_nRobot_X = M_TEST_RBT_X;
	int m_nRobot_Y = M_TEST_RBT_Y;
	int m_nRobot_Ejector = M_TEST_DVC_EJECT_Z;

	int m_nLinearMove_Index = 2;   //현재 map index
	int m_lAxisCnt	= 2; 

	m_lpAxisNum[0] = M_TEST_RBT_Y; //m_lpAxisNum[4];        //현재의 IndexNum에서 사용하는 모터 실재 번호를 가진다
	m_lpAxisNum[1] = M_TEST_RBT_X;
	m_lpAxisNum[2] = M_TEST_RBT_Z;
	m_lpAxisNum[3] = M_TEST_DVC_EJECT_Z;

	m_dpTargetPosXY[0] = 0;
	m_dpTargetPosXY[1] = 0;

	switch(m_nVisFocusStep)
	{
	case 0:
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD)
		{
			m_nVisFocusStep = 100;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nVisFocusStep = 0;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);
			m_nVisFocusStep = 0;
			nFuncRet = RET_ERROR;
		}
		break;

	case 100:
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Ejector, st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_nVisFocusStep = 900;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nVisFocusStep = 100;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);
			nFuncRet = RET_ERROR;
			m_nVisFocusStep = 100;
		}
		break;

	case 900:
		m_nRtyCount = 0;
		m_dFocusZ = 0;
		st_teach.m_dVisionZValue = 0.0;
		//m_nVisFocusStep = 1000;
		m_nVisFocusStep = 910;
		break;

	case 910:
		if (st_teach.m_dFocusTeachZPos[nSite] > 0)
		{
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_teach.m_dFocusTeachZPos[nSite], COMI.mn_runspeed_rate);
			if (nRet_1 == BD_GOOD)
			{
				m_nVisFocusStep = 1000;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nVisFocusStep = 0;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
				CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);
				m_nVisFocusStep = 0;
				nFuncRet = RET_ERROR;
			}
		}
		else
		{
			m_nVisFocusStep = 0;
			nFuncRet = RET_ERROR;
		}
		break;
		/////

	case 1000:
		::SendMessage( st_handler_info.hWnd, WM_MAINFRAME_WORK, MAIN_TEACH_VISION, 5 );

		//m_nVisFocusStep = 1100;
		m_nVisFocusStep = 1010;
		break;

	case 1010:
		if (st_teach.m_dVisionZValue <= 0.0)
		{
			m_nRtyCount++;
			if (m_nRtyCount >5)
			{
				m_nRtyCount = 0;
				m_dwXOffset = 0.0;
				m_dwYOffset = 0.0;

				m_nVisFocusStep = 0;
				nFuncRet = RET_ERROR;
			}
			else
			{
				if (m_nRtyCount == 1)m_dwXOffset = 2; break;
				if (m_nRtyCount == 2)m_dwXOffset = -2;break;
				if (m_nRtyCount == 3)m_dwYOffset = 2; break;
				if (m_nRtyCount == 4)m_dwYOffset = -2;break;

				m_nVisFocusStep = 1020;
			}
		}
		else
		{
			m_nVisFocusStep = 1100;
		}
		break;

	case 1020:
		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY], COMI.mn_runspeed_rate);
		if (nRet_1 == BD_GOOD)
		{
			m_nVisFocusStep = 1030;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nVisFocusStep = 1020;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);
			m_nVisFocusStep = 1020;
			nFuncRet = RET_ERROR;
		}
		break;

	case 1030:
		if( m_nSite_No < 4)//LEFT 사이트
		{//마크가 있는 위치
			m_dpTargetPosXY[0] = st_teach.m_dLeftVisionMarkXPos[m_nSite_No][m_iVisionPos] + m_dwXOffset; //X Pos
			m_dpTargetPosXY[1] = st_teach.m_dLeftVisionMarkYPos[m_nSite_No][m_iVisionPos] + m_dwYOffset; //Y Pos
		}
		else//Right 사이트
		{
			m_dpTargetPosXY[0] = st_teach.m_dRightVisionMarkXPos[m_nSite_No - 4][m_iVisionPos]+ m_dwXOffset; //X Pos
			m_dpTargetPosXY[1] = st_teach.m_dRightVisionMarkYPos[m_nSite_No - 4][m_iVisionPos]+ m_dwYOffset; //Y Pos
		}

		//kwlee 2017.0105
		if (m_nSimul == 1)
		{
			nRet_1 = BD_GOOD;
		}
		else
		{
			nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosXY, COMI.mn_runspeed_rate);
		}
		if(nRet_1 == BD_GOOD)
		{
			m_nVisFocusStep = 0;
		}
		else if(nRet_1 == BD_ERROR)
		{			
			CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);
			nFuncRet = RET_ERROR;
			
		}
		break;

	case 1100:
		//비젼에서 넘겨준 값이 넘어왔는지 체크
// 		str.Format(_T("%.3f"),st_teach.m_dVisionZValue);
// 		SetDlgItemText(IDC_EDIT_EDGE,str);
// 
// 		//kwlee 2016.1018
 	//	if(st_teach.m_dVisionZValue <= 0.0)
 	//	{
		//	break;
// 			m_nRtyCount++;
// 			m_nVisFocusStep = 1000; //kwlee 2016.1018
// 
// 			if (m_nRtyCount >3)
// 			{
// 				m_nRtyCount = 0;
// 				m_nVisFocusStep = -1;
// 				nFuncRet = RET_ERROR;
// 			
// 				AfxMessageBox(_T("Edge Fail"));
// 			}
 //			break;
 		//}
		
		//값이 제일 큰값을 찾는데 맞는지 체크 필요함.(비젼에z축이 내려가면서 포커스 체크할 때 제일 큰값을 찾는게 맞는지 확인 필요)
// 		if( m_dTempFcsZ < st_teach.m_dVisionZValue )
// 		{
// 			m_dFocusZ++;
// 			m_nVisFocusStep = 1200;
// 		}
// 		else
// 		{
// 			st_teach.m_dFocusTeachZPos[nSite] = m_dFocusZ;//저장
// 			m_nVisFocusStep = 2000;
// 			break;
// 		}
		//kwlee 2016.1124
		str.Format(_T("%.2f"),st_teach.m_dVisionZValue);
		str_1 = str.Left(2);
		nTemp = _wtoi(str_1);

		if (nSite < 4)
		{
			nRet_1 = COMI.Check_MotPosRange(M_TEST_RBT_X, st_teach.m_dLeftVisionMarkXPos[m_nSite_No][m_iVisionPos], 10);
			nRet_2 = COMI.Check_MotPosRange(M_TEST_RBT_Y, st_teach.m_dLeftVisionMarkYPos[m_nSite_No][m_iVisionPos], 10);
			nPos = P_TESTRBT_Z_AUTOTEACH_VISION_TESTSITE_LEFT;
		}
		else
		{
			nRet_1 = COMI.Check_MotPosRange(M_TEST_RBT_X, st_teach.m_dRightVisionMarkXPos[m_nSite_No - 4][m_iVisionPos], 10);
			nRet_2 = COMI.Check_MotPosRange(M_TEST_RBT_Y, st_teach.m_dRightVisionMarkYPos[m_nSite_No - 4][m_iVisionPos], 10);
			nPos = P_TESTRBT_Z_AUTOTEACH_VISION_TESTSITE_RIGHT;
		}
		
		if (nRet_1 == BD_GOOD && nRet_2 == BD_GOOD )  // 이동할 위치에 이미 존재함
		{
			nRet_1 = COMI.Check_MotPosRange(M_TEST_RBT_Z,st_motor_info[m_nRobot_Z].d_pos[nPos] , 10);

			if (nRet_1 == BD_GOOD)
			{
				if (st_teach.m_dVisionZValue > 0)
				{
					if( nTemp < 43 || 48 < nTemp )
					{
// 						if (m_nRtyCount >5)
// 						{
// 							nFuncRet = RET_ERROR;
// 							m_nRtyCount = 0;
// 							m_nVisFocusStep = 0;
// 						}
// 						else
// 						{
							if (nTemp <43)
							{
								m_dFocusZ++;
							}
							else if(nTemp > 48)
							{
								m_dFocusZ--;
							}

							m_dwWaitTime[0] = GetCurrentTime();
							m_nVisFocusStep = 1200;
							
							//m_nRtyCount++;
						//}
					}
					else
					{
						//st_teach.m_dFocusTeachZPos[nSite] = m_dFocusZ;//저장
						//st_teach.m_dFocusTeachZPos[nSite] = st_teach.m_dFocusTeachZPos[nSite]+ m_dFocusZ;//저장
						m_nVisFocusStep = 2000;
						//break;
					}
					st_teach.m_dFocusTeachZPos[nSite] = st_teach.m_dFocusTeachZPos[nSite]+ m_dFocusZ;
				}
			}
			else
			{
				m_nVisFocusStep = 0;
				nFuncRet = RET_ERROR;
			}
		}
		else
		{
			m_nVisFocusStep = 0;
			nFuncRet = RET_ERROR;
		}
// 		if(m_nRtyCount == 0)
// 		{//처음 동작시에 z축 너무 높으면 m_dFocusZ에 값을 m_dFocusZ = 10 많이 준뒤에 1씩 증가 시킨다. 티칭시간 조절위함
// 			// 나중에는 좀더 세밀히 해야 됨
// 			m_dFocusZ++;
// 			m_nVisFocusStep = 1200;
// 		}
		break;

	case 1200:
		m_dwWaitTime[1] = GetCurrentTime();
		m_dwWaitTime[2] = m_dwWaitTime[1] - m_dwWaitTime[0];

		if (m_dwWaitTime[2] <= 0)
		{
			m_dwWaitTime[0] = GetCurrentTime();						 
		}
		if (m_dwWaitTime[2] >  3000)
		{
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_teach.m_dFocusTeachZPos[nSite], COMI.mn_runspeed_rate);
		
			if (nRet_1 == BD_GOOD)
			{
				m_nVisFocusStep = 1000;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nVisFocusStep = 0;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
				CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);
				m_nVisFocusStep = 0;
				nFuncRet = RET_ERROR;
			}
		}
		break;

	case 2000:

		nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY], COMI.mn_runspeed_rate);

		if (nRet_1 == BD_GOOD)
		{
			m_nVisFocusStep = 0;
			nFuncRet = RET_GOOD;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nVisFocusStep = 0;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);
			m_nVisFocusStep = 0;
			nFuncRet = RET_ERROR;
		}
		break;
	}
	return nFuncRet;
}
int CWorkVisionTeach::VisionTeach_Move()
{
	int nFuncRet = RET_PROCEED;
	int nRet_1=0, nRet_2=0, i, j;
	int iXPos=0, iYPos=0, nFlag=0;
	bool bEnable = false;
	CString strDebug=_T("");
	CString strLogX, strLogY;
	//int m_iVisionPos = 0;

	int m_nLinearMove_Index = 2;   //현재 map index
	int m_lAxisCnt	= 2; 

	long m_lpAxisNum[4];
	double m_dpTargetPosList[4], m_dpSpdRatio[3];
	double m_dpTargetPosZ;
//	double m_dPos;
	m_lpAxisNum[0] = M_TEST_RBT_Y; //m_lpAxisNum[4];        //현재의 IndexNum에서 사용하는 모터 실재 번호를 가진다
	m_lpAxisNum[1] = M_TEST_RBT_X;
	m_lpAxisNum[2] = M_TEST_RBT_Z;
	m_lpAxisNum[3] = M_TEST_DVC_EJECT_Z;

	int m_nRobot_Z = M_TEST_RBT_Z; 
	int m_nRobot_Ejector = M_TEST_DVC_EJECT_Z;
	int m_nRobot_X = M_TEST_RBT_X;
	int m_nRobot_Y = M_TEST_RBT_Y;

	m_dpTargetPosList[0]  = 0; //m_dpTargetPosList[4];		   //최대 4 포인트를 한번에 이동가능하다
	m_dpTargetPosList[1]  = 0;
	m_dpTargetPosList[2]  = 0;

	m_dpSpdRatio[0] = 100; 		//	m_dpSpdRatio[3];        //속도비율 [0]:Vel, [1]:Acc, [2]:Dec
	m_dpSpdRatio[1] = 100;
	m_dpSpdRatio[2] = 100;

	for( j = 0; j < 8; j++)
	{
		CTL_Lib.Process_TestChamber_AcMotor(0, THD_TESTSITE_1+j, 0);
	}

	switch(m_nVisTeachStep)
	{
	case 0:
		m_nVisTeachStep = 100;
		break;

	case 100://피커 상태를 파악한다//디바이스를 들고 있거나 테스트사이트에 디바이스가 있으면 에러이다.
		for(i=0; i<TEST_PICKER_PARA; i++)
		{
			m_npPicker_YesNo[i] = CTL_YES;
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_TestRbtPickerGripDvcExistChk[i],	IO_OFF);
			if(nRet_1 == IO_OFF) 
			{
				nFlag++;					
			}
			else
			{
				strDebug.Format(_T("8%d%04d"), IO_ON, st_io_info.i_LdUldPickDvcChk); 
				CTL_Lib.Alarm_Error_Occurrence(m_nVisTeachStep, dWARNING, strDebug);	
				break;
			}
		}
		m_nVisTeachStep = 200;
		break; 

	case 200:
		clsRunTestSiteRobot.Set_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON);
		m_nVisTeachStep = 210;
		break;

	case 210:
		nRet_1 = clsRunTestSiteRobot.Chk_TestSiteRobotGlipper_OnOff(m_npPicker_YesNo, IO_ON); 
		if(nRet_1 == RET_GOOD)
		{
			m_nVisTeachStep = 300;
		}
		else if(nRet_1 == RET_ERROR)
		{
			CTL_Lib.Alarm_Error_Occurrence(m_nVisTeachStep, dWARNING, clsRunTestSiteRobot.m_strAlarmCode);
			nFuncRet = RET_ERROR;
			m_nVisTeachStep = 600;
		}
		break;

	case 300:
		//kwlee 2017.0105

		if (m_nSimul == 1)
		{
			nRet_1 = BD_GOOD;
		}
		else
		{
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY], COMI.mn_runspeed_rate);
		}

		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_nVisTeachStep = 400;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nVisTeachStep = 300;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);
			m_nVisTeachStep = 300;
			nFuncRet = RET_ERROR;
		}
		break;

	case 400:
		//kwlee 2017.0105
		if (m_nSimul == 1)
		{
			nRet_1 = BD_GOOD;
		}
		else
		{
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Ejector, st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY], COMI.mn_runspeed_rate);
		}
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_nVisTeachStep = 500;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nVisTeachStep = 400;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);
			nFuncRet = RET_ERROR;
			m_nVisTeachStep = 300;
		}
		break;

	case 500:
		m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_SAFETY];
		m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_SAFETY];

		//kwlee 2017.0105
		if (m_nSimul == 1)
		{
			nRet_1 = BD_GOOD;
		}
		else
		{
			nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate);
		}
		if(nRet_1 == BD_GOOD) //정상적으로 완료된 상태
		{
			m_nVisTeachStep = 600;
		}
		else if(nRet_1 == BD_ERROR)
		{			
			CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);
			nFuncRet = RET_ERROR;
			m_nVisTeachStep = 400;
		}
		break;	

	case 600:
		//테스터에자재가 있으면 비젼을 체크할 수 없다.
		if(st_lot_info[LOT_CURR].nLotStatus == LOT_START || st_lot_info[LOT_NEXT].nLotStatus == LOT_START )
		{
			strDebug = _T(" 랏이 존재하면 비젼을 테스트 할 수 없습니다.");
			CTL_Lib.Alarm_Error_Occurrence(271, dWARNING,strDebug);
			clsMem.OnAbNormalMessagWrite(strDebug);
			nFuncRet = RET_ERROR;
			m_nVisTeachStep = 0;
		}
		else
		{
			m_nVisTeachStep = 700;
		}
		break;

	case 700:
		m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_SAFETY];
		m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_SAFETY];

		//kwlee 2017.0105
		if (m_nSimul == 1)
		{
			nRet_1 = BD_GOOD;
		}
		else
		{
			nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate);
		}
		if(nRet_1 == BD_GOOD) //정상적으로 완료된 상태
		{
			m_nVisTeachStep = 1000;
		}
		else if(nRet_1 == BD_ERROR)
		{
			CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);
			nFuncRet = RET_ERROR;
			m_nVisTeachStep = 0;
		}
		break;

	case 1000://테스트 사이트의 AC 모터를 OPEN 한다
		for( i = 0; i < 8; i++)
		{
			st_test_site_info[THD_TESTSITE_1+i].nTester_Req[0] = CTL_REQ;
			st_test_site_info[THD_TESTSITE_1+i].nTester_Req[1] = WORK_PICK;

			st_sync_info.nTestSiteChamber_AcMotorCtl_Req[THD_TESTSITE_1+i][0] = CTL_REQ;
			st_sync_info.nTestSiteChamber_AcMotorCtl_Req[THD_TESTSITE_1+i][1] = WORK_PICK;

			m_bVisWaitChk_Falg[i] = false;
		}
		m_nVisTeachStep = 1100;
		break;

	case 1100:
		nFlag = 0;
		for( i = 0; i < 8; i++ )
		{
			if(st_test_site_info[THD_TESTSITE_1+i].nTester_Req[0] == CTL_READY)
			{
				if(st_test_site_info[THD_TESTSITE_1+i].nTester_Req[1] == WORK_PICK) //open chamber, 자재 test site 에서 집기전 실행 
				{
					nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ChamberOpenChk[i],	IO_ON);  
					nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ChamberCloseChk[i],	IO_OFF);  
					if(nRet_1 == IO_ON && nRet_2 == IO_OFF )
					{
						nFlag++;
					}
					else
					{	// 알람은 로보트에서 체크하여 처리 하자, 이곳에서 하면 안됨 
						strDebug.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ChamberOpenChk[i]); 
						CTL_Lib.Alarm_Error_Occurrence(m_nVisTeachStep, dWARNING, strDebug);
						nFuncRet = RET_ERROR;
						m_nVisTeachStep = 0;	
					}
				}
			}
			else
			{
				if(m_bVisWaitChk_Falg[i] == false)
				{
					m_bVisWaitChk_Falg[i] = true;
					m_dwWaitTimeVis[i][0]	= GetCurrentTime();
				}
				else 
				{
					//limit time 설정 
					m_dwWaitTimeVis[i][1] = GetCurrentTime();
					m_dwWaitTimeVis[i][2] = m_dwWaitTimeVis[i][1] - m_dwWaitTimeVis[i][0];
					if (m_dwWaitTimeVis[i][2] <= 0)
					{
						m_dwWaitTimeVis[i][0] = GetCurrentTime();						 
					}
					if (m_dwWaitTimeVis[i][2] >  IO_CONVEYOR_WAIT_LIMIT)
					{
						if (	st_test_site_info[THD_TESTSITE_1+i].nTester_Req[1] == WORK_PICK) //open chamber, 자재 test site 에서 집기전 실행 
						{
							strDebug.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ChamberOpenChk[i]); 
						}
						else if(st_test_site_info[THD_TESTSITE_1+i].nTester_Req[1] == WORK_PLACE) //close chamber, 자재 test site에 insert 후 진행
						{
							strDebug.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ChamberCloseChk[i]); 
						}
						CTL_Lib.Alarm_Error_Occurrence(m_nVisTeachStep, dWARNING, strDebug);
						m_bVisWaitChk_Falg[i] = false;
						nFuncRet = RET_ERROR;
						m_nVisTeachStep = 0;
						break;
					}

					////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					//센서도 확인하자 혹시 플레그가 문제가 될 경우 대비하여, 조건이 충족되면 플레그를 강제 셋팅하여 해당 작업 종료  
					////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					if(st_test_site_info[THD_TESTSITE_1+i].nTester_Req[1] == WORK_PICK) //open chamber, 자재 test site 에서 집기전 실행 
					{
						nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ChamberOpenChk[i],	IO_ON);  
						nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ChamberCloseChk[i],	IO_OFF);  
						if(nRet_1 == IO_ON && nRet_2 == IO_OFF )
						{
							st_test_site_info[THD_TESTSITE_1+i].nTester_Req[0] = CTL_FREE; //강제 종료 
							nFlag++;
						}
					}
				}
			}
		}

		if( nFlag < 8 ) break;

		m_nVisTeachStep = 1200;
		break;

	case 1200:
		nFlag = 0;
		for( i = 0; i < 8; i++ )
		{
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_ChamberOpenChk[i],	IO_ON);  
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_ChamberCloseChk[i],	IO_OFF);  
			//kwlee 2016.1012
			//if(nRet_1 == IO_ON && nRet_1 == IO_OFF )
			if(nRet_1 == IO_ON && nRet_2 == IO_OFF )
			{	
				nFlag++;
			}
			else
			{
				break;
			}
		}
		if( nFlag != 8 )
		{
			strDebug.Format(_T("8%d%04d"), IO_ON, st_io_info.i_ChamberOpenChk[i]); 
			CTL_Lib.Alarm_Error_Occurrence(m_nVisTeachStep, dWARNING, strDebug);
			nFuncRet = RET_ERROR;
			m_nVisTeachStep = 0;	
		}
		else
		{
			m_nVisTeachStep = 1300;
		}
		break;

		//테스터가 모두 장착되어 있는지 체크한다.
		//테스터가 장착되어 있는 장비만 체크한다.
		//디바이스 사용 유무로 체크한다.
	case 1300:
		//m_nIO_No = 0;
		//m_iVisionPos = 0;
		m_nVisTeachStep = 1400;
		break;

	case 1400:
		//kwlee 2017.0105
		if (m_nSimul == 1)
		{
			nRet_1 = BD_GOOD;
		}
		else
		{
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY], COMI.mn_runspeed_rate);
		}
		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_nVisTeachStep = 1500;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nVisTeachStep = 1400;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);
			m_nVisTeachStep = 1400;
			nFuncRet = RET_ERROR;
		}
		break;

	case 1500:
		//kwlee 2017.0105
		if (m_nSimul == 1)
		{
			nRet_1 = BD_GOOD;
		}
		else
		{
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Ejector, st_motor_info[m_nRobot_Ejector].d_pos[P_DVC_EJECTOR_Z_SAFETY], COMI.mn_runspeed_rate);
		}

		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_nVisTeachStep = 2000;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nVisTeachStep = 1500;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);
			nFuncRet = RET_ERROR;
			m_nVisTeachStep = 1500;
		}
		break;

	case 2000:
		if( m_nSite_No >= MAX_TSITE )
		{
			m_nVisTeachStep = 9000;
			break;
		}

		bEnable = false;
		for( i = 0 ;i < 8; i++ )
		{
			//if(st_buffer_info[m_nIO_No].st_pcb_info[i].nEnable == CTL_YES)  
			if(st_test_site_info[THD_TESTSITE_1 + m_nSite_No].st_pcb_info[i].nEnable == CTL_YES)  
			{
				bEnable = true;
				break;
			}
		}

		//사용 가능한 소켓이 있을 때의 체크
		if(  bEnable == true  )
		{
			//kwlee 2016.1018
			//if(FAS_IO.get_in_bit( st_io_info.i_TsiteLockOnChk[m_nIO_No] , IO_ON ) == IO_ON )//테스트가 장착되어 있다
			if(FAS_IO.get_in_bit( st_io_info.i_TsiteLockOnChk[m_nSite_No] , IO_OFF ) == IO_OFF )//테스트가 장착되어 있다
			{
				if(  FAS_IO.get_in_bit( st_io_info.i_TsiteExistChk[m_nSite_No] , IO_ON ) == IO_ON )
				{
					if( FAS_IO.get_in_bit(st_io_info.i_TsiteUpChk[m_nSite_No],	IO_OFF)	== IO_OFF &&
						FAS_IO.get_in_bit(st_io_info.i_TsiteDnChk[m_nSite_No],	IO_ON)	== IO_ON)
					{ 
						m_nVisTeachStep = 2100;
					}
					else
					{
						strDebug.Format( _T("TESTER : It doesn't check UpDn-Cylinser-Sensor on TESTER %d th" ), m_nSite_No + 1 );
						CTL_Lib.Alarm_Error_Occurrence(m_nVisTeachStep, dWARNING, strDebug);
						nFuncRet = RET_ERROR;
					}
				}
				else
				{
					//kwlee 2016.1219
					m_nVisTeachStep = 2100;
				
// 					strDebug.Format( _T("TESTER : There is not exist TESTER %d th, Please check socket enable."), m_nIO_No + 1 );
// 					CTL_Lib.Alarm_Error_Occurrence(m_nVisTeachStep, dWARNING, strDebug);
// 					nFuncRet = RET_ERROR;
				}
			}	 
			else
			{//2000;
				m_nVisTeachStep = 2000;
				m_nSite_No++;
			}
		}
		//kwlee 2016.1018
		else
		{
			m_nVisTeachStep = 2000;
			m_nSite_No++;
		}
		break;

		//사이트 중에 사용할 수 있는 소켓이 있으면 그 소켓만 비젼 티칭한다.
		//나머지는 원래 메뉴얼로 티칭된 값으로만 사용한다.
		//각 테스트 사이트에서 4군데에서 비젼 마크를 읽는다.
		//단, 4,8번 사이트는 기구 간섭으로 왼쪽 2군데만 티칭한다.여기서는 m_iVisionPos 1,2만 티칭한다.3,4,는 못함
	case 2100:
// 		if( ( m_nSite_No == 3 && m_iVisionPos >= 2 ) ||
// 			( m_nSite_No == 7 && m_iVisionPos >= 2 ) )
		//kwlee 2017.0104
		if( ( m_nSite_No == 1 && m_iVisionPos >= 2 ) ||
			( m_nSite_No == 2 && m_iVisionPos >= 2 ) ||
			( m_nSite_No == 3 && m_iVisionPos >= 2 ) ||
			( m_nSite_No == 5 && m_iVisionPos >= 2 ) ||
			( m_nSite_No == 6 && m_iVisionPos >= 2 ) ||
			( m_nSite_No == 7 && m_iVisionPos >= 2 ) )
		{
			m_nVisTeachStep = 2700;
			break;
		}

		if( m_nSite_No >= MAX_TSITE)		
		{
			m_nVisTeachStep = 9000;
			break;
		}

		if(m_iVisionPos >= MAX_MARK_POS)
		{
			m_nVisTeachStep = 2700;
			break;
		}

		if( m_nSite_No < 4)//LEFT 사이트
		{//마크가 있는 위치
			m_dpTargetPosList[1] = st_teach.m_dLeftVisionMarkXPos[m_nSite_No][m_iVisionPos]; //X Pos
			m_dpTargetPosList[0] = st_teach.m_dLeftVisionMarkYPos[m_nSite_No][m_iVisionPos]; //Y Pos
		}
		else//Right 사이트
		{
			//kwlee 2016.1012
// 			m_dpTargetPosList[0] = st_teach.m_dRightVisionMarkYPos[m_nIO_No][m_iVisionPos];
// 			m_dpTargetPosList[1] = st_teach.m_dRightVisionMarkXPos[m_nIO_No][m_iVisionPos];

			m_dpTargetPosList[1] = st_teach.m_dRightVisionMarkXPos[m_nSite_No - 4][m_iVisionPos]; //X Pos
			m_dpTargetPosList[0] = st_teach.m_dRightVisionMarkYPos[m_nSite_No - 4][m_iVisionPos]; //Y Pos
		}

		//kwlee 2017.0105
		if (m_nSimul == 1)
		{
			nRet_1 = BD_GOOD;
		}
		else
		{
			nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate);
		}
		if(nRet_1 == BD_GOOD)
		{
			m_nVisTeachStep = 2200;
		}
		else if(nRet_1 == BD_ERROR)
		{			
			CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);
			nFuncRet = RET_ERROR;
			m_nVisTeachStep = 2100;
		}
		break;

	case 2200:
		//1번,5번 사이트만 처음 Mark에서만 티칭한다. -> 나중에 사이트 별로 할 수도 있다.
		if( ( m_nSite_No == 0 || m_nSite_No == 4) && m_iVisionPos == 0 )//z축 auto focus 티칭한다.	
		{
			m_nVisFocusStep = 0;
			m_nVisTeachStep = 2210;
		}
		else
		{
			m_nVisTeachStep = 2220;
		}
		break;

	case 2210:
		//kwlee 2017.0105
		if (m_nSimul == 1)
		{
			nRet_1 = RET_GOOD;
		}
		else
		{
			nRet_1 = VisionFocus_Move(m_nSite_No);
		}

		if(nRet_1 == RET_GOOD )
		{
			m_nVisTeachStep = 2220;
		}
		else if(nRet_1 == RET_ERROR )
		{
			m_nVisTeachStep = 0;
			nFuncRet = RET_ERROR;
		}
		break;

	case 2220:
// 		if( m_nIO_No < 4)//LEFT 사이트
// 		{
// 			m_dpTargetPosZ = st_teach.m_dFocusTeachZPos[0];//Z축 오토티칭으로 저장된값
// 		}
// 		else
// 		{
// 			m_dpTargetPosZ = st_teach.m_dFocusTeachZPos[1];
// 		}

		m_dpTargetPosZ = st_teach.m_dFocusTeachZPos[m_nSite_No];

		//kwlee 2017.0105
		if (m_nSimul == 1)
		{
			nRet_1 = BD_GOOD;
		}
		else
		{
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, m_dpTargetPosZ, COMI.mn_runspeed_rate);
		}

		if (nRet_1 == BD_GOOD)
		{
			m_nVisTeachStep = 2300;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nVisTeachStep = 2200;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);
			m_nVisTeachStep = 2200;
			nFuncRet = RET_ERROR;
		}
		break;

	case 2300:
		//kwlee 2016.1020
		//마크에서 소켓까지의 거리
		//st_teach.m_dMarkToSocketXPos = 30;
		//st_teach.m_dMarkToSocketYPos = 10;
// 		if( m_nIO_No < 4)//LEFT 사이트
// 		{   //소켓 까지 거리.
// 			st_teach.m_dMarkToSocketXPos =	st_motor_info[m_nRobot_X].d_pos[ P_TESTRBT_XY_TESTDUCT_1_LEFT_START +(m_nIO_No *2) ] - st_teach.m_dLeftVisionMarkXPos[m_nIO_No][m_iVisionPos]; //X Pos
// 			st_teach.m_dMarkToSocketYPos =	st_motor_info[m_nRobot_Y].d_pos[ P_TESTRBT_XY_TESTDUCT_1_LEFT_START + (m_nIO_No *2) ] - st_teach.m_dLeftVisionMarkYPos[m_nIO_No][m_iVisionPos]; //Y Pos
// 
// 		}
// 		else//Right 사이트
// 		{
// 			//kwlee 2016.1012
// 			// 			m_dpTargetPosList[0] = st_teach.m_dRightVisionMarkYPos[m_nIO_No][m_iVisionPos];
// 			// 			m_dpTargetPosList[1] = st_teach.m_dRightVisionMarkXPos[m_nIO_No][m_iVisionPos];
// 			//Right 소켓 까지 거리.
// 			st_teach.m_dMarkToSocketXPos = st_motor_info[m_nRobot_X].d_pos[ P_TESTRBT_XY_TESTDUCT_1_RIGHT_START + (m_nIO_No *2) ] - st_teach.m_dRightVisionMarkXPos[m_nIO_No - 4][m_iVisionPos]; //X Pos
// 			st_teach.m_dMarkToSocketYPos = st_motor_info[m_nRobot_Y].d_pos[ P_TESTRBT_XY_TESTDUCT_1_RIGHT_START + (m_nIO_No *2) ] -st_teach.m_dRightVisionMarkYPos[m_nIO_No - 4][m_iVisionPos]; //Y Pos
// 
// 		}
		
		//로봇Z에 붙어 있는 카메라 OffSet 쟤봐야 함.
// 		st_teach.m_dRobotCameraOffSetXPos = 10; 
// 		st_teach.m_dRobotCameraOffSetYPos = 5;
		
		//카메라 Offset
// 		st_teach.m_dRobotCameraOffSetXPos = 45;
// 		//카메라 Offset
// 		st_teach.m_dRobotCameraOffSetYPos = 160; 
// 		//Socket 거리
// 		st_teach.m_dMarkToSocketXPos = 45;
// 		st_teach.m_dMarkToSocketYPos = 237.5;

		//Socket Y 에서 Mark 거리 : 237.5 
		//Socket Y 이동 해야 할 거리 : 77.5
		//Socket X 에서 Mark 거리 : 45 

		st_teach.m_dVisionXValue = 0.0;
		st_teach.m_dVisionYValue = 0.0;
		::SendMessage( st_handler_info.hWnd, WM_MAINFRAME_WORK, MAIN_TEACH_VISION, 4 );
		m_nVisTeachStep = 2400;
		break;

	case 2400:
		//kwlee 2017.0105
		if (m_nSimul == 1)
		{
			nRet_1 = BD_GOOD;
			//st_teach.m_dVisionYValue = 1.3 ;
			//st_teach.m_dVisionXValue = 2.5;
		}
		else
		{
			if( st_teach.m_dVisionXValue == 0.0 || st_teach.m_dVisionYValue == 0.0 ) 
				break;
		}
		//실제 픽셀값을 계산새서 값을 대응한다. 지금은 일단 픽셀당 0.025를 곱했다 픽샐당 거리를 물어서(비젼에) 값을 저장한다.
		if( m_nSite_No < 4)//LEFT 사이트
		{
			//kwlee 2016.1012
			//st_teach.m_dLeftVisionReadXPos[m_nIO_No][m_iVisionPos] = st_teach.m_dVisionXValue * 0.025;
			//비젼에서 읽은 X축위치                                = 비젼에서의 리턴해준 픽셀값 * 1픽셀당 거리(비젼팀한테 물어본다)
			//st_teach.m_dLeftVisionReadYPos[m_nIO_No][m_iVisionPos] = st_teach.m_dVisionYValue * 0.025;
			//비젼에서 읽은 Y축위치                                = 비젼에서의 리턴해준 픽셀값 * 1픽셀당 거리(비젼팀한테 물어본다)
			
			//st_teach.m_dLeftVisionReadXPos[m_nIO_No][m_iVisionPos] = st_teach.m_dVisionXValue * 0.022;
			
			//st_teach.m_dLeftVisionReadYPos[m_nIO_No][m_iVisionPos] = st_teach.m_dVisionYValue * 0.022;

			//kwlee 2016.1212 
			if (m_nSimul == 1)
			{
				st_teach.m_dLeftVisionReadXPos[m_nSite_No][m_iVisionPos];
				st_teach.m_dLeftVisionReadYPos[m_nSite_No][m_iVisionPos];
			}
			else
			{
				st_teach.m_dLeftVisionReadXPos[m_nSite_No][m_iVisionPos] = st_teach.m_dVisionYValue * 0.022;
				//비젼에서 읽은 X축위치                                = 비젼에서의 리턴해준 픽셀값 * 1픽셀당 거리(비젼팀한테 물어본다)
				st_teach.m_dLeftVisionReadYPos[m_nSite_No][m_iVisionPos] = st_teach.m_dVisionXValue * 0.022;
				//비젼에서 읽은 Y축위치                                = 비젼에서의 리턴해준 픽셀값 * 1픽셀당 거리(비젼팀한테 물어본다)
			}	
		//	st_teach.m_dLeftVisionTeachXPos[m_nIO_No][m_iVisionPos] = st_teach.m_dLeftVisionMarkXPos[m_nIO_No][m_iVisionPos] + ( st_teach.m_dMarkToSocketXPos - st_teach.m_dLeftVisionMarkXPos[m_nIO_No][m_iVisionPos] - st_teach.m_dRobotCameraOffSetXPos );
			//비젼에서 읽은 티칭 X축위치                            = 물리적인 비젼에서 볼때의 X축 값[사이트][4군데중 한군데]+ ( 마크에서 X축 소켓 거리       - 물리적인 비젼에서 볼때의 X축 값[사이트][4군데중 한군데 - 로봇z축에 있는 카메라와 피커의X축 거리)
			//st_teach.m_dLeftVisionTeachYPos[m_nIO_No][m_iVisionPos] = st_teach.m_dLeftVisionMarkYPos[m_nIO_No][m_iVisionPos] + ( st_teach.m_dMarkToSocketYPos - st_teach.m_dLeftVisionMarkYPos[m_nIO_No][m_iVisionPos] - st_teach.m_dRobotCameraOffSetYPos);
			//////////

			//kwlee 2016.1212
			//가야할 위치.
			//st_teach.m_dLeftVisionTeachXPos[m_nSite_No][m_iVisionPos] = st_teach.m_dLeftVisionMarkXPos[m_nSite_No][m_iVisionPos] + ( st_teach.m_dMarkToSocketXPos - st_teach.m_dLeftVisionReadXPos[m_nSite_No][m_iVisionPos] - st_teach.m_dRobotCameraOffSetXPos );
			st_teach.m_dLeftVisionTeachXPos[m_nSite_No][m_iVisionPos] = (st_teach.m_dLeftVisionMarkXPos[m_nSite_No][m_iVisionPos] - st_teach.m_dLeftVisionReadXPos[m_nSite_No][m_iVisionPos]) +  st_teach.m_dMarkToSocketXPos  - st_teach.m_dRobotCameraOffSetXPos;
			
			if (m_iVisionPos < 2)
			{
				//st_teach.m_dLeftVisionTeachYPos[m_nSite_No][m_iVisionPos] = st_teach.m_dLeftVisionMarkYPos[m_nSite_No][m_iVisionPos] + ( st_teach.m_dMarkToSocketYPos - st_teach.m_dLeftVisionReadYPos[m_nSite_No][m_iVisionPos] - st_teach.m_dRobotCameraOffSetYPos);
				st_teach.m_dLeftVisionTeachYPos[m_nSite_No][m_iVisionPos] = (st_teach.m_dLeftVisionMarkYPos[m_nSite_No][m_iVisionPos] - st_teach.m_dLeftVisionReadYPos[m_nSite_No][m_iVisionPos]) + st_teach.m_dMarkToSocketYPos - st_teach.m_dRobotCameraOffSetYPos;
			}
			else
			{
				//st_teach.m_dLeftVisionTeachYPos[m_nSite_No][m_iVisionPos] = st_teach.m_dLeftVisionMarkYPos[m_nSite_No][m_iVisionPos] - ( st_teach.m_dMarkToSocketYPos + st_teach.m_dLeftVisionReadYPos[m_nSite_No][m_iVisionPos] + st_teach.m_dRobotCameraOffSetYPos);
				st_teach.m_dLeftVisionTeachYPos[m_nSite_No][m_iVisionPos] = (st_teach.m_dLeftVisionMarkYPos[m_nSite_No][m_iVisionPos] - st_teach.m_dLeftVisionReadYPos[m_nSite_No][m_iVisionPos]) - st_teach.m_dMarkToSocketYPos - st_teach.m_dRobotCameraOffSetYPos;
			}
			strLogX.Format( _T("[Site %d_%02d] X Pos : 마크에서 비전 거리 : [%3.3f] Camera Offset X : [%3.3f] Vision Read X : [%3.3f] Vision Teach X:[%3.3f]"), m_nSite_No+1, m_iVisionPos + 1,  st_teach.m_dMarkToSocketXPos, st_teach.m_dRobotCameraOffSetXPos,  st_teach.m_dLeftVisionReadXPos[m_nSite_No][m_iVisionPos], st_teach.m_dLeftVisionTeachXPos[m_nSite_No][m_iVisionPos] );
			strLogY.Format( _T("[Site %d_%02d] Y Pos : 마크에서 비전 거리 : [%3.3f] Camera Offset Y : [%3.3f] Vision Read Y : [%3.3f] Vision Teach Y:[%3.3f]"), m_nSite_No+1,  m_iVisionPos + 1,  st_teach.m_dMarkToSocketYPos, st_teach.m_dRobotCameraOffSetYPos,  st_teach.m_dLeftVisionReadYPos[m_nSite_No][m_iVisionPos], st_teach.m_dLeftVisionTeachYPos[m_nSite_No][m_iVisionPos] );
			
		}
		else
		{
			//kwlee 2016.1012
// 			st_teach.m_dRightVisionReadXPos[m_nIO_No][m_iVisionPos] = st_teach.m_dVisionXValue * 0.025;
// 			st_teach.m_dRightVisionReadYPos[m_nIO_No][m_iVisionPos] = st_teach.m_dVisionYValue * 0.025;

			//st_teach.m_dRightVisionReadXPos[m_nIO_No - 4][m_iVisionPos] = st_teach.m_dVisionXValue * 0.022;
			//st_teach.m_dRightVisionReadYPos[m_nIO_No - 4][m_iVisionPos] = st_teach.m_dVisionYValue * 0.022;
			if (m_nSimul == 1)
			{
				st_teach.m_dRightVisionReadXPos[m_nSite_No - 4][m_iVisionPos];
				st_teach.m_dRightVisionReadYPos[m_nSite_No - 4][m_iVisionPos];
			}
			else
			{
				//kwlee 2016.1219
				st_teach.m_dRightVisionReadXPos[m_nSite_No - 4][m_iVisionPos] = st_teach.m_dVisionYValue * 0.022;
				st_teach.m_dRightVisionReadYPos[m_nSite_No - 4][m_iVisionPos] = st_teach.m_dVisionXValue * 0.022;
			}
			//kwlee 2016.1012
			//st_teach.m_dRightVisionTeachXPos[m_nIO_No][m_iVisionPos] = st_teach.m_dRightVisionMarkXPos[m_nIO_No][m_iVisionPos] + ( st_teach.m_dMarkToSocketXPos - st_teach.m_dRightVisionMarkXPos[m_nIO_No][m_iVisionPos] - st_teach.m_dRobotCameraOffSetXPos );
			//st_teach.m_dRightVisionTeachYPos[m_nIO_No][m_iVisionPos] = st_teach.m_dRightVisionMarkYPos[m_nIO_No][m_iVisionPos] + ( st_teach.m_dMarkToSocketYPos - st_teach.m_dRightVisionMarkYPos[m_nIO_No][m_iVisionPos] - st_teach.m_dRobotCameraOffSetYPos);
			//strLogX.Format( _T("[Site %d_%02d] X Pos : [%3.2f][%3.2f][%3.2f][%3.2f]"), m_nIO_No+1, m_iVisionPos + 1,  st_teach.m_dMarkToSocketXPos, st_teach.m_dRobotCameraOffSetXPos,  st_teach.m_dRightVisionReadXPos[m_nIO_No][m_iVisionPos], st_teach.m_dRightVisionTeachXPos[m_nIO_No][m_iVisionPos] );
			//strLogY.Format( _T("[Site %d_%02d] Y Pos : [%3.2f][%3.2f][%3.2f][%3.2f]"), m_nIO_No+1,  m_iVisionPos + 1,  st_teach.m_dMarkToSocketYPos, st_teach.m_dRobotCameraOffSetYPos,  st_teach.m_dRightVisionReadYPos[m_nIO_No][m_iVisionPos], st_teach.m_dRightVisionTeachYPos[m_nIO_No][m_iVisionPos] );
			
			//가야할 위치.
			//st_teach.m_dRightVisionTeachXPos[m_nSite_No - 4][m_iVisionPos] = st_teach.m_dRightVisionMarkXPos[m_nSite_No - 4][m_iVisionPos] + ( st_teach.m_dMarkToSocketXPos - st_teach.m_dRightVisionReadXPos[m_nSite_No - 4][m_iVisionPos] - st_teach.m_dRobotCameraOffSetXPos );
			st_teach.m_dRightVisionTeachXPos[m_nSite_No - 4][m_iVisionPos] = (st_teach.m_dRightVisionMarkXPos[m_nSite_No - 4][m_iVisionPos]- st_teach.m_dRightVisionReadXPos[m_nSite_No - 4][m_iVisionPos]) +  st_teach.m_dMarkToSocketXPos - st_teach.m_dRobotCameraOffSetXPos ;

			if (m_iVisionPos < 2)
			{
				//st_teach.m_dRightVisionTeachYPos[m_nSite_No - 4][m_iVisionPos] = st_teach.m_dRightVisionMarkYPos[m_nSite_No - 4][m_iVisionPos] + ( st_teach.m_dMarkToSocketYPos - st_teach.m_dRightVisionReadYPos[m_nSite_No - 4][m_iVisionPos] - st_teach.m_dRobotCameraOffSetYPos);
				st_teach.m_dRightVisionTeachYPos[m_nSite_No - 4][m_iVisionPos] = (st_teach.m_dRightVisionMarkYPos[m_nSite_No - 4][m_iVisionPos] - st_teach.m_dRightVisionReadYPos[m_nSite_No - 4][m_iVisionPos]) + st_teach.m_dMarkToSocketYPos - st_teach.m_dRobotCameraOffSetYPos;
			}
			else
			{
				//st_teach.m_dRightVisionTeachYPos[m_nSite_No - 4][m_iVisionPos] = st_teach.m_dRightVisionMarkYPos[m_nSite_No - 4][m_iVisionPos] - ( st_teach.m_dMarkToSocketYPos + st_teach.m_dRightVisionReadYPos[m_nSite_No - 4][m_iVisionPos] + st_teach.m_dRobotCameraOffSetYPos);
				st_teach.m_dRightVisionTeachYPos[m_nSite_No - 4][m_iVisionPos] = (st_teach.m_dRightVisionMarkYPos[m_nSite_No - 4][m_iVisionPos] - st_teach.m_dRightVisionReadYPos[m_nSite_No - 4][m_iVisionPos]) -  st_teach.m_dMarkToSocketYPos - st_teach.m_dRobotCameraOffSetYPos;
			}
			strLogX.Format( _T("[Site %d_%02d] X Pos : 마크에서 비전 거리 : [%3.3f] Camera Offset X : [%3.3f] Vision Read X : [%3.3f] Vision Teach X:[%3.3f]"), m_nSite_No+1, m_iVisionPos + 1,  st_teach.m_dMarkToSocketXPos, st_teach.m_dRobotCameraOffSetXPos,  st_teach.m_dRightVisionReadXPos[m_nSite_No - 4][m_iVisionPos], st_teach.m_dRightVisionTeachXPos[m_nSite_No - 4][m_iVisionPos] );
			strLogY.Format( _T("[Site %d_%02d] Y Pos : 마크에서 비전 거리 : [%3.3f] Camera Offset Y : [%3.3f] Vision Read Y : [%3.3f] Vision Teach Y:[%3.3f]"), m_nSite_No+1,  m_iVisionPos + 1,  st_teach.m_dMarkToSocketYPos, st_teach.m_dRobotCameraOffSetYPos, st_teach.m_dRightVisionReadYPos[m_nSite_No - 4][m_iVisionPos], st_teach.m_dRightVisionTeachYPos[m_nSite_No - 4][m_iVisionPos] );
		}

		clsMem.OnAbNormalMessagWrite(strLogX);
		clsMem.OnAbNormalMessagWrite(strLogY);
		
		//if( st_handler_info.m_iVisionOffSetAutoMode == CTL_YES )
		if( st_handler_info.m_iVisionOffSetAutoMode == CTL_YES)
		{
			if( m_nSite_No < 4)//LEFT 사이트
			{
				//st_teach.m_dLeftVisionOffSetXPos[m_nIO_No][m_iVisionPos] = st_motor_info[m_nRobot_X].d_pos[ P_TESTRBT_XY_TESTDUCT_1_LEFT_START + m_iVisionPos ] - st_teach.m_dLeftVisionTeachXPos[m_nIO_No][m_iVisionPos];
				//st_teach.m_dLeftVisionOffSetYPos[m_nIO_No][m_iVisionPos] = st_motor_info[m_nRobot_Y].d_pos[ P_TESTRBT_XY_TESTDUCT_1_LEFT_START + m_iVisionPos ] - st_teach.m_dLeftVisionTeachYPos[m_nIO_No][m_iVisionPos];
				//kwlee 2016.1219
				if (m_iVisionPos%2)
				{
					st_teach.m_dLeftVisionOffSetXPos[m_nSite_No][m_iVisionPos] = st_motor_info[m_nRobot_X].d_pos[ P_TESTRBT_XY_TESTDUCT_1_RIGHT_START + (m_nSite_No * 2)] - st_teach.m_dLeftVisionTeachXPos[m_nSite_No][m_iVisionPos];
					st_teach.m_dLeftVisionOffSetYPos[m_nSite_No][m_iVisionPos] = st_motor_info[m_nRobot_Y].d_pos[ P_TESTRBT_XY_TESTDUCT_1_RIGHT_START + (m_nSite_No * 2)] - st_teach.m_dLeftVisionTeachYPos[m_nSite_No][m_iVisionPos];

					strLogX.Format( _T("[Site %02d Mark : %02d] X Pos : Vision Teach : [%3.3f]-> MotorTeach : [%3.3f] OffSet[%3.3f]"),  m_nSite_No+1, m_iVisionPos + 1,  st_teach.m_dLeftVisionTeachXPos[m_nSite_No][m_iVisionPos], st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_TESTDUCT_1_RIGHT_START + (m_nSite_No * 2) ], st_teach.m_dLeftVisionOffSetXPos[m_nSite_No][m_iVisionPos] );
					strLogY.Format( _T("[Site %02d Mark : %02d] Y Pos : Vision Teach : [%3.3f]-> MotorTeach : [%3.3f] OffSet[%3.3f]"),  m_nSite_No+1, m_iVisionPos + 1,  st_teach.m_dLeftVisionTeachYPos[m_nSite_No][m_iVisionPos], st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_TESTDUCT_1_RIGHT_START + (m_nSite_No * 2) ], st_teach.m_dLeftVisionOffSetYPos[m_nSite_No][m_iVisionPos] );
				}
				else
				{
					st_teach.m_dLeftVisionOffSetXPos[m_nSite_No][m_iVisionPos] = st_motor_info[m_nRobot_X].d_pos[ P_TESTRBT_XY_TESTDUCT_1_LEFT_START + (m_nSite_No * 2)] - st_teach.m_dLeftVisionTeachXPos[m_nSite_No][m_iVisionPos];
					st_teach.m_dLeftVisionOffSetYPos[m_nSite_No][m_iVisionPos] = st_motor_info[m_nRobot_Y].d_pos[ P_TESTRBT_XY_TESTDUCT_1_LEFT_START + (m_nSite_No * 2)] - st_teach.m_dLeftVisionTeachYPos[m_nSite_No][m_iVisionPos];

					strLogX.Format( _T("[Site %02d Mark : %02d] X Pos : Vision Teach : [%3.3f]-> MotorTeach : [%3.3f] OffSet[%3.3f]"),  m_nSite_No+1, m_iVisionPos + 1,  st_teach.m_dLeftVisionTeachXPos[m_nSite_No][m_iVisionPos], st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_TESTDUCT_1_LEFT_START + (m_nSite_No * 2) ], st_teach.m_dLeftVisionOffSetXPos[m_nSite_No][m_iVisionPos] );
					strLogY.Format( _T("[Site %02d Mark : %02d] Y Pos : Vision Teach : [%3.3f]-> MotorTeach : [%3.3f] OffSet[%3.3f]"),  m_nSite_No+1, m_iVisionPos + 1,  st_teach.m_dLeftVisionTeachYPos[m_nSite_No][m_iVisionPos], st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_TESTDUCT_1_LEFT_START + (m_nSite_No * 2) ], st_teach.m_dLeftVisionOffSetYPos[m_nSite_No][m_iVisionPos] );

				}
// 				strLogX.Format( _T("[Site %02d Mark : %02d] X Pos :[%3.2f]->[%3.2f] OffSet[%3.2f]"),  m_nIO_No+1, m_iVisionPos + 1,  st_teach.m_dLeftVisionTeachXPos[m_nIO_No][m_iVisionPos], st_motor_info[m_nRobot_X].d_pos[ P_TESTRBT_XY_TESTDUCT_1_LEFT_START + m_iVisionPos ], st_teach.m_dLeftVisionOffSetXPos[m_nIO_No][m_iVisionPos] );
// 				strLogY.Format( _T("[Site %02d Mark : %02d] Y Pos :[%3.2f]->[%3.2f] OffSet[%3.2f]"),  m_nIO_No+1, m_iVisionPos + 1,  st_teach.m_dLeftVisionTeachYPos[m_nIO_No][m_iVisionPos], st_motor_info[m_nRobot_Y].d_pos[ P_TESTRBT_XY_TESTDUCT_1_LEFT_START + m_iVisionPos ], st_teach.m_dLeftVisionOffSetYPos[m_nIO_No][m_iVisionPos] );
				//kwlee 2016.1219
// 				strLogX.Format( _T("[Site %02d Mark : %02d] X Pos :[%3.2f]->[%3.2f] OffSet[%3.2f]"),  m_nIO_No+1, m_iVisionPos + 1,  st_teach.m_dLeftVisionTeachXPos[m_nIO_No][m_iVisionPos], st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_TESTDUCT_1_LEFT_START + m_iVisionPos%2 ], st_teach.m_dLeftVisionOffSetXPos[m_nIO_No][m_iVisionPos] );
// 				strLogY.Format( _T("[Site %02d Mark : %02d] Y Pos :[%3.2f]->[%3.2f] OffSet[%3.2f]"),  m_nIO_No+1, m_iVisionPos + 1,  st_teach.m_dLeftVisionTeachYPos[m_nIO_No][m_iVisionPos], st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_TESTDUCT_1_LEFT_START +m_iVisionPos%2 ], st_teach.m_dLeftVisionOffSetYPos[m_nIO_No][m_iVisionPos] );
			}
			else
			{
				//kwlee 2016.1012
// 				st_teach.m_dRightVisionOffSetXPos[m_nIO_No][m_iVisionPos] = st_motor_info[m_nRobot_X].d_pos[ P_TESTRBT_XY_TESTDUCT_1_LEFT_START + m_iVisionPos ] - st_teach.m_dRightVisionTeachXPos[m_nIO_No][m_iVisionPos];
// 				st_teach.m_dRightVisionOffSetYPos[m_nIO_No][m_iVisionPos] = st_motor_info[m_nRobot_Y].d_pos[ P_TESTRBT_XY_TESTDUCT_1_LEFT_START + m_iVisionPos ] - st_teach.m_dRightVisionTeachYPos[m_nIO_No][m_iVisionPos];
// 
// 				strLogX.Format( _T("[Site %02d_%02d] X Pos :[%3.2f]->[%3.2f] OffSet[%3.2f]"),  m_nIO_No+1, m_iVisionPos + 1,  st_teach.m_dRightVisionTeachXPos[m_nIO_No][m_iVisionPos], st_motor_info[m_nRobot_X].d_pos[ P_TESTRBT_XY_TESTDUCT_5_LEFT_START + m_iVisionPos ], st_teach.m_dLeftVisionOffSetXPos[m_nIO_No][m_iVisionPos] );
// 				strLogY.Format( _T("[Site %02d_%02d] Y Pos :[%3.2f]->[%3.2f] OffSet[%3.2f]"),  m_nIO_No+1, m_iVisionPos + 1,  st_teach.m_dRightVisionTeachYPos[m_nIO_No][m_iVisionPos], st_motor_info[m_nRobot_Y].d_pos[ P_TESTRBT_XY_TESTDUCT_5_LEFT_START + m_iVisionPos ], st_teach.m_dLeftVisionOffSetYPos[m_nIO_No][m_iVisionPos] );

				//st_teach.m_dRightVisionOffSetXPos[m_nIO_No - 4][m_iVisionPos] = st_motor_info[m_nRobot_X].d_pos[ P_TESTRBT_XY_TESTDUCT_1_RIGHT_START + m_iVisionPos ] - st_teach.m_dRightVisionTeachXPos[m_nIO_No  - 4][m_iVisionPos];
				//st_teach.m_dRightVisionOffSetYPos[m_nIO_No - 4][m_iVisionPos] = st_motor_info[m_nRobot_Y].d_pos[ P_TESTRBT_XY_TESTDUCT_1_RIGHT_START + m_iVisionPos ] - st_teach.m_dRightVisionTeachYPos[m_nIO_No  - 4][m_iVisionPos];
				//kwlee 2016.1219
				if (m_iVisionPos%2)
				{
					st_teach.m_dRightVisionOffSetXPos[m_nSite_No - 4][m_iVisionPos] = st_motor_info[m_nRobot_X].d_pos[ P_TESTRBT_XY_TESTDUCT_5_RIGHT_START + ((m_nSite_No - 4) * 2)] - st_teach.m_dRightVisionTeachXPos[m_nSite_No  - 4][m_iVisionPos];
					st_teach.m_dRightVisionOffSetYPos[m_nSite_No - 4][m_iVisionPos] = st_motor_info[m_nRobot_Y].d_pos[ P_TESTRBT_XY_TESTDUCT_5_RIGHT_START + ((m_nSite_No - 4) * 2) ] - st_teach.m_dRightVisionTeachYPos[m_nSite_No  - 4][m_iVisionPos];

					strLogX.Format( _T("[Site %02d Mark : %02d] X Pos : Vision Teach : [%3.3f]-> MotorTeach : [%3.3f] OffSet[%3.3f]"),  m_nSite_No+1, m_iVisionPos + 1,  st_teach.m_dRightVisionTeachXPos[m_nSite_No  - 4][m_iVisionPos], st_motor_info[m_nRobot_X].d_pos[ P_TESTRBT_XY_TESTDUCT_5_RIGHT_START + ((m_nSite_No - 4) * 2) ], st_teach.m_dRightVisionOffSetXPos[m_nSite_No  - 4][m_iVisionPos] );
					strLogY.Format( _T("[Site %02d Mark : %02d] Y Pos : Vision Teach : [%3.3f]-> MotorTeach : [%3.3f] OffSet[%3.3f]"),  m_nSite_No+1, m_iVisionPos + 1,  st_teach.m_dRightVisionTeachYPos[m_nSite_No  - 4][m_iVisionPos], st_motor_info[m_nRobot_Y].d_pos[ P_TESTRBT_XY_TESTDUCT_5_RIGHT_START + ((m_nSite_No - 4) * 2) ], st_teach.m_dRightVisionOffSetYPos[m_nSite_No  - 4][m_iVisionPos] );
				}
				else
				{
					st_teach.m_dRightVisionOffSetXPos[m_nSite_No - 4][m_iVisionPos] = st_motor_info[m_nRobot_X].d_pos[ P_TESTRBT_XY_TESTDUCT_5_LEFT_START + ((m_nSite_No - 4) * 2) ] - st_teach.m_dRightVisionTeachXPos[m_nSite_No  - 4][m_iVisionPos];
					st_teach.m_dRightVisionOffSetYPos[m_nSite_No - 4][m_iVisionPos] = st_motor_info[m_nRobot_Y].d_pos[ P_TESTRBT_XY_TESTDUCT_5_LEFT_START + ((m_nSite_No - 4) * 2) ] - st_teach.m_dRightVisionTeachYPos[m_nSite_No  - 4][m_iVisionPos];

					strLogX.Format( _T("[Site %02d Mark : %02d] X Pos : Vision Teach : [%3.3f]-> MotorTeach : [%3.2f] OffSet[%3.3f]"),  m_nSite_No+1, m_iVisionPos + 1,  st_teach.m_dRightVisionTeachXPos[m_nSite_No  - 4][m_iVisionPos], st_motor_info[m_nRobot_X].d_pos[ P_TESTRBT_XY_TESTDUCT_5_LEFT_START + ((m_nSite_No - 4) * 2) ], st_teach.m_dRightVisionOffSetXPos[m_nSite_No  - 4][m_iVisionPos] );
					strLogY.Format( _T("[Site %02d Mark : %02d] Y Pos : Vision Teach : [%3.3f]-> MotorTeach : [%3.2f] OffSet[%3.3f]"),  m_nSite_No+1, m_iVisionPos + 1,  st_teach.m_dRightVisionTeachYPos[m_nSite_No  - 4][m_iVisionPos], st_motor_info[m_nRobot_Y].d_pos[ P_TESTRBT_XY_TESTDUCT_5_LEFT_START + ((m_nSite_No - 4) * 2) ], st_teach.m_dRightVisionOffSetYPos[m_nSite_No  - 4][m_iVisionPos] );
				}
				//strLogX.Format( _T("[Site %02d_%02d] X Pos :[%3.2f]->[%3.2f] OffSet[%3.2f]"),  m_nIO_No+1, m_iVisionPos + 1,  st_teach.m_dRightVisionTeachXPos[m_nIO_No  - 4][m_iVisionPos], st_motor_info[m_nRobot_X].d_pos[ P_TESTRBT_XY_TESTDUCT_5_LEFT_START + m_iVisionPos ], st_teach.m_dRightVisionOffSetXPos[m_nIO_No  - 4][m_iVisionPos] );
				//strLogY.Format( _T("[Site %02d_%02d] Y Pos :[%3.2f]->[%3.2f] OffSet[%3.2f]"),  m_nIO_No+1, m_iVisionPos + 1,  st_teach.m_dRightVisionTeachYPos[m_nIO_No  - 4][m_iVisionPos], st_motor_info[m_nRobot_Y].d_pos[ P_TESTRBT_XY_TESTDUCT_5_LEFT_START + m_iVisionPos ], st_teach.m_dRightVisionOffSetYPos[m_nIO_No  - 4][m_iVisionPos] );
				//kwlee 2016.1219
				//strLogX.Format( _T("[Site %02d_%02d] X Pos :[%3.2f]->[%3.2f] OffSet[%3.2f]"),  m_nIO_No+1, m_iVisionPos + 1,  st_teach.m_dRightVisionTeachXPos[m_nIO_No  - 4][m_iVisionPos], st_motor_info[m_nRobot_X].d_pos[ P_TESTRBT_XY_TESTDUCT_5_LEFT_START + m_iVisionPos%2 ], st_teach.m_dRightVisionOffSetXPos[m_nIO_No  - 4][m_iVisionPos] );
				//strLogY.Format( _T("[Site %02d_%02d] Y Pos :[%3.2f]->[%3.2f] OffSet[%3.2f]"),  m_nIO_No+1, m_iVisionPos + 1,  st_teach.m_dRightVisionTeachYPos[m_nIO_No  - 4][m_iVisionPos], st_motor_info[m_nRobot_Y].d_pos[ P_TESTRBT_XY_TESTDUCT_5_LEFT_START + m_iVisionPos%2 ], st_teach.m_dRightVisionOffSetYPos[m_nIO_No  - 4][m_iVisionPos] );
			}
			clsMem.OnAbNormalMessagWrite(strLogX);
			clsMem.OnAbNormalMessagWrite(strLogY);
			
		}
		else
		{
			if( m_nSite_No < 4)//LEFT 사이트
			{
				if (m_iVisionPos%2)
				{
					strLogX.Format( _T("[Site %02d Mark : %02d] X Pos : Vision Teach : [%3.3f]-> MotorTeach : [%3.3f] OffSet[%3.3f]"),  m_nSite_No+1, m_iVisionPos + 1,  st_teach.m_dLeftVisionTeachXPos[m_nSite_No][m_iVisionPos], st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_TESTDUCT_1_RIGHT_START +(m_nSite_No * 2)], st_teach.m_dLeftVisionOffSetXPos[m_nSite_No][m_iVisionPos] );
					strLogY.Format( _T("[Site %02d Mark : %02d] Y Pos : Vision Teach : [%3.3f]-> MotorTeach : [%3.3f] OffSet[%3.3f]"),  m_nSite_No+1, m_iVisionPos + 1,  st_teach.m_dLeftVisionTeachYPos[m_nSite_No][m_iVisionPos], st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_TESTDUCT_1_RIGHT_START +(m_nSite_No * 2)], st_teach.m_dLeftVisionOffSetYPos[m_nSite_No][m_iVisionPos] );
				}
				else
				{
					strLogX.Format( _T("[Site %02d Mark : %02d] X Pos : Vision Teach : [%3.3f]-> MotorTeach : [%3.3f] OffSet[%3.3f]"),  m_nSite_No+1, m_iVisionPos + 1,  st_teach.m_dLeftVisionTeachXPos[m_nSite_No][m_iVisionPos], st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_TESTDUCT_1_LEFT_START +(m_nSite_No * 2)], st_teach.m_dLeftVisionOffSetXPos[m_nSite_No][m_iVisionPos] );
					strLogY.Format( _T("[Site %02d Mark : %02d] Y Pos : Vision Teach : [%3.3f]-> MotorTeach : [%3.3f] OffSet[%3.3f]"),  m_nSite_No+1, m_iVisionPos + 1,  st_teach.m_dLeftVisionTeachYPos[m_nSite_No][m_iVisionPos], st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_TESTDUCT_1_LEFT_START +(m_nSite_No * 2)], st_teach.m_dLeftVisionOffSetYPos[m_nSite_No][m_iVisionPos] );
				}
			}
			else
			{
// 				strLogX.Format( _T("[Site %02d_%02d] X Pos :[%3.2f]->[%3.2f] OffSet[%3.2f]"),  m_nIO_No+1, m_iVisionPos + 1,  st_teach.m_dRightVisionTeachXPos[m_nIO_No][m_iVisionPos], st_motor_info[m_nRobot_X].d_pos[ P_TESTRBT_XY_TESTDUCT_5_LEFT_START + m_iVisionPos ], st_teach.m_dLeftVisionOffSetXPos[m_nIO_No][m_iVisionPos] );
// 				strLogY.Format( _T("[Site %02d_%02d] Y Pos :[%3.2f]->[%3.2f] OffSet[%3.2f]"),  m_nIO_No+1, m_iVisionPos + 1,  st_teach.m_dRightVisionTeachYPos[m_nIO_No][m_iVisionPos], st_motor_info[m_nRobot_Y].d_pos[ P_TESTRBT_XY_TESTDUCT_5_LEFT_START + m_iVisionPos ], st_teach.m_dLeftVisionOffSetYPos[m_nIO_No][m_iVisionPos] );
				if (m_iVisionPos%2)
				{
					strLogX.Format( _T("[Site %02d Mark : %02d] X Pos : Vision Teach : [%3.3f]-> MotorTeach : [%3.3f] OffSet[%3.3f]"),  m_nSite_No+1, m_iVisionPos + 1,  st_teach.m_dRightVisionTeachXPos[m_nSite_No - 4][m_iVisionPos], st_motor_info[m_nRobot_X].d_pos[ P_TESTRBT_XY_TESTDUCT_5_RIGHT_START + ((m_nSite_No - 4) * 2) ], st_teach.m_dRightVisionOffSetXPos[m_nSite_No  - 4][m_iVisionPos] );
					strLogY.Format( _T("[Site %02d Mark : %02d] Y Pos : Vision Teach : [%3.3f]-> MotorTeach : [%3.3f] OffSet[%3.3f]"),  m_nSite_No+1, m_iVisionPos + 1,  st_teach.m_dRightVisionTeachYPos[m_nSite_No - 4][m_iVisionPos], st_motor_info[m_nRobot_Y].d_pos[ P_TESTRBT_XY_TESTDUCT_5_RIGHT_START + ((m_nSite_No - 4) * 2) ], st_teach.m_dRightVisionOffSetYPos[m_nSite_No  - 4][m_iVisionPos] );
				}
				else
				{
					strLogX.Format( _T("[Site %02d Mark : %02d] X Pos : Vision Teach : [%3.3f]-> MotorTeach : [%3.3f] OffSet[%3.3f]"),  m_nSite_No+1, m_iVisionPos + 1,  st_teach.m_dRightVisionTeachXPos[m_nSite_No - 4][m_iVisionPos], st_motor_info[m_nRobot_X].d_pos[ P_TESTRBT_XY_TESTDUCT_5_LEFT_START + ((m_nSite_No - 4) * 2) ], st_teach.m_dRightVisionOffSetXPos[m_nSite_No  - 4][m_iVisionPos] );
					strLogY.Format( _T("[Site %02d Mark : %02d] Y Pos : Vision Teach : [%3.3f]-> MotorTeach : [%3.3f] OffSet[%3.3f]"),  m_nSite_No+1, m_iVisionPos + 1,  st_teach.m_dRightVisionTeachYPos[m_nSite_No - 4][m_iVisionPos], st_motor_info[m_nRobot_Y].d_pos[ P_TESTRBT_XY_TESTDUCT_5_LEFT_START + ((m_nSite_No - 4) * 2) ], st_teach.m_dRightVisionOffSetYPos[m_nSite_No  - 4][m_iVisionPos] );
				}
			}			
			clsMem.OnAbNormalMessagWrite(strLogX);
			clsMem.OnAbNormalMessagWrite(strLogY);
		}
		m_nVisTeachStep = 2500;
		break;

	case 2500:
		//kwlee 2017.0105
		if (m_nSimul == 1)
		{
			nRet_1 = BD_GOOD;
		}
		else
		{
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nRobot_Z, st_motor_info[m_nRobot_Z].d_pos[P_TESTRBT_Z_SAFETY], COMI.mn_runspeed_rate);
		}

		if (nRet_1 == BD_GOOD) //좌측으로 이동
		{
			m_nVisTeachStep = 2600;
		}
		else if (nRet_1 == BD_RETRY)
		{
			m_nVisTeachStep = 2500;
		}
		else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
		{//모터 알람은 이미 처리했으니 이곳에서는 런 상태만 바꾸면 된다
			CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);
			m_nVisTeachStep = 2500;
			nFuncRet = RET_ERROR;
		}
		break;

	case 2600:
		//m_nVisTeachStep = 2100;
		//m_nVisTeachStep = 9000;
		//kwlee 2017.0104
		m_nVisTeachStep = 2100;
		m_iVisionPos++;
		//kwlee 2016.1219
		//m_nVisTeachStep = 9000;

		break;

	case 2700:
		m_nVisTeachStep = 2100;
		//kwlee 2017.0104
		m_nSite_No++;
		m_iVisionPos = 0;
		break;

	case 9000:
		m_dpTargetPosList[0] = st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_SAFETY];
		m_dpTargetPosList[1] = st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_SAFETY];

		//kwlee 2017.0105
		if (m_nSimul == 1)
		{
			nRet_1 = BD_GOOD;
		}
		else
		{
			nRet_1 = CTL_Lib.Linear_Move(BOTH_MOVE_FINISH, m_nLinearMove_Index, m_lAxisCnt, m_lpAxisNum, m_dpTargetPosList, COMI.mn_runspeed_rate);
		}

		if(nRet_1 == BD_GOOD) //정상적으로 완료된 상태
		{
			m_nVisTeachStep = 9100;
		}
		else if(nRet_1 == BD_ERROR)
		{
			CTL_Lib.Alarm_Error_Occurrence(271, dWARNING, st_alarm_info.strCode);
			nFuncRet = RET_ERROR;
			m_nVisTeachStep = 9000;
		}
		break;

	case 9100:
		st_handler_info.m_iVisionOffSetAutoMode = CTL_NO;
		nFuncRet = RET_GOOD;
		m_nSite_No = 0;
		m_iVisionPos = 0;
		m_nVisTeachStep = 0;
		break;
	}

	return nFuncRet;
}

void CWorkVisionTeach::OnBnClickedBtnLeftTsiteRbtFocusGo()
{
	if (st_lot_info[LOT_CURR].nLotStatus >= LOT_START)
	{
		return;
	}

	if (st_handler_info.nRunStatus		!= dSTOP &&
		st_handler_info.nMachineMode	!= MACHINE_MANUAL)
	{
		return;
	}

	m_nVisFocusStep = 0;
	SetTimer( TM_VISION_FCOUS, 100, NULL);
}

void CWorkVisionTeach::OnInitBtn()
{
// 	m_btnDataSave.SetBitmaps(IDC_BTN_APPLY, IDB_BITMAP_APPLY_DN1, WINDOW_DN1, IDB_BITMAP_APPLY_UP1, WINDOW_UP1);
// 	m_btnDataSave.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
// 	m_btnDataSave.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
// 	m_btnDataSave.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
// 	m_btnDataSave.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
// 	m_btnDataSave.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
// 	m_btnDataSave.SetFont(clsFunc.m_pFont[2]);
// 	m_btnDataSave.SetTooltipText(_T("Data Save"));
// 
// 	m_btnDataLoad.SetBitmaps(IDC_BTN_RELOAD, IDB_BITMAP_RELOAD_DN1, WINDOW_DN1, IDB_BITMAP_RELOAD_UP1, WINDOW_UP1);
// 	m_btnDataLoad.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN1);
// 	m_btnDataLoad.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP1);
// 	m_btnDataLoad.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP1);
// 	m_btnDataLoad.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
// 	m_btnDataLoad.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
// 	m_btnDataLoad.SetFont(clsFunc.m_pFont[2]);
// 	m_btnDataLoad.SetTooltipText(_T("Data Load"));

	// kwlee 20160929
	if (st_handler_info.m_iVisionOffSetAutoMode == YES)
	{
		m_nAutoTeachModeUse = YES;
		m_btnAutoTeachUse.SetWindowTextW(_T("Auto Teach Use"));
	}
	else
	{
		m_nAutoTeachModeUse = NO;
		m_btnAutoTeachUse.SetWindowTextW(_T("Auto Teach Skip"));
	}

}
void CWorkVisionTeach::OnInitLabel()
{
	m_msg_TsiteRbt_Site.SetFont(clsFunc.m_pFont[2]);
	m_msg_TsiteRbt_Site.SetWindowText(_T("Test Site" ));
	m_msg_TsiteRbt_Site.SetCenterText();
	m_msg_TsiteRbt_Site.SetColor(WHITE_C);
	m_msg_TsiteRbt_Site.SetGradientColor( GREEN_C);
	m_msg_TsiteRbt_Site.SetTextColor(BLACK_C);

	m_msg_TsiteRbt_Mark.SetFont(clsFunc.m_pFont[2]);
	m_msg_TsiteRbt_Mark.SetWindowText(_T("TSite Mark" ));
	m_msg_TsiteRbt_Mark.SetCenterText();
	m_msg_TsiteRbt_Mark.SetColor(WHITE_C);
	m_msg_TsiteRbt_Mark.SetGradientColor( GREEN_C);
	m_msg_TsiteRbt_Mark.SetTextColor(BLACK_C);
}

BOOL CWorkVisionTeach::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	OnInitLabel();
	OnInitDgt();
	OnInitDataLoad();

	OnInitBtn();
	//OnInitLeftOffsetGrid();
	//OnInitRightOffsetGrid();
	//OnInitTestSitePos();
	/////////////////////
	OnInitLeftVisionTeachValue();
	OnInitRightVisionTeachValue();

	OnInitLeftReadGrid();
	OnInitRightReadGrid();
	OnInitLeftMarkTeachGrid();
	OnInitRightMarkTeachGrid();
	OnInitLeftVisionTeachGrid();
	OnInitRightVisionTeachGrid();
	SetTimer( TM_VISION_DISPLAY, 1000, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CWorkVisionTeach::OnInitDgt()
{
	m_iVisionPos = 0;
	m_nSite_No = 0;
	m_dgt_TsiteRbt_Mark.SetStyle(CDigit::DS_INT, 2, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgt_TsiteRbt_Mark.SetVal(m_iVisionPos);

	m_dgt_TsiteRbt_Site.SetStyle(CDigit::DS_INT, 2, CDigit::DC_BGREEN, CDigit::DC_BDISABLE);
	m_dgt_TsiteRbt_Site.SetVal(m_nSite_No);
}



void CWorkVisionTeach::OnStnClickedDgtLeftTsiteRbtSite()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_KeyPad dlgKeyPad;
	CString strTmp;
	int nVal;

	dlgKeyPad.m_nKeypadMode				= 0;
	dlgKeyPad.m_strKeypadLowLimit		= _T("0");
	dlgKeyPad.m_strKeypadHighLimit		= _T("7");
	dlgKeyPad.m_strKeypadVal.Format(_T("%d"), m_nSite_No);

	if (dlgKeyPad.DoModal() == IDOK)
	{
		strTmp			= dlgKeyPad.m_strNewVal;
		nVal			= _wtoi(strTmp);
		m_nSite_No		= nVal;

		if(m_nSite_No == 1 || m_nSite_No == 2 || m_nSite_No == 3  ||	m_nSite_No == 7 )
		{
			if (m_iVisionPos >=2)
			{
				m_iVisionPos = 1;
				m_dgt_TsiteRbt_Mark.SetVal(m_iVisionPos);
			}
		}
			  
		m_dgt_TsiteRbt_Site.SetVal(m_nSite_No);
	}
}


void CWorkVisionTeach::OnStnClickedDgtTsiteRbtMark()
{
	CDialog_KeyPad dlgKeyPad;
	CString strTmp;
	int nVal;

	dlgKeyPad.m_nKeypadMode				= 0;
	dlgKeyPad.m_strKeypadLowLimit		= _T("0");
	if (m_nSite_No == 1 ||m_nSite_No == 2 || m_nSite_No == 3  ||	m_nSite_No == 7)
	{
		dlgKeyPad.m_strKeypadHighLimit		= _T("1");
	}
	else
	{
		dlgKeyPad.m_strKeypadHighLimit		= _T("3");
	}
	
	dlgKeyPad.m_strKeypadVal.Format(_T("%d"), m_iVisionPos);

	if (dlgKeyPad.DoModal() == IDOK)
	{
		strTmp							= dlgKeyPad.m_strNewVal;
		nVal							= _wtoi(strTmp);
		m_iVisionPos					= nVal;
		if (m_nSite_No == 1 ||m_nSite_No == 2 || m_nSite_No == 3  ||	m_nSite_No == 7)
		{
			if (m_iVisionPos >= 2)
			{
				m_iVisionPos = 1;
			}
		}

		m_dgt_TsiteRbt_Mark.SetVal(m_iVisionPos);
	}
}
void CWorkVisionTeach::OnBnClickedBtnLeftTsiteRbtTeachGo()
{
	if (st_lot_info[LOT_CURR].nLotStatus >= LOT_START)
	{
		return;
	}
	if (st_handler_info.nRunStatus		!= dSTOP &&
		st_handler_info.nMachineMode	!= MACHINE_MANUAL)
	{
		return;
	}
	if (m_nSimul == 1)
	{
		m_nVisTeachStep =  2100;
	}
	else
	{
		m_nVisTeachStep = 0;
	}
	m_iVisionPos = 0;
	m_nSite_No = 0;
	SetTimer( TM_VISION_TEACH, 100, NULL);
}

void CWorkVisionTeach::OnBnClickedBtnVisionShow()
{
	::SendMessage( st_handler_info.hWnd, WM_MAINFRAME_WORK, MAIN_TEACH_VISION, 1 );
}


void CWorkVisionTeach::OnBnClickedBtnVisionHide()
{
	::SendMessage( st_handler_info.hWnd, WM_MAINFRAME_WORK, MAIN_TEACH_VISION, 0 );
}


void CWorkVisionTeach::OnBnClickedBtnOffset()
{
	CString str_1,str_2;

	::SendMessage( st_handler_info.hWnd, WM_MAINFRAME_WORK, MAIN_TEACH_VISION, 4 );

	str_1.Format(_T("%.3f"),st_teach.m_dVisionXValue);
	SetDlgItemText(IDC_OFFSET_X,str_1);

	str_2.Format(_T("%.3f"),st_teach.m_dVisionYValue);
	SetDlgItemText(IDC_OFFSET_Y,str_2);
}


void CWorkVisionTeach::OnBnClickedBtnEdge()
{
	CString str;
	::SendMessage( st_handler_info.hWnd, WM_MAINFRAME_WORK, MAIN_TEACH_VISION, 5 );
	
	str.Format(_T("%.3f"),st_teach.m_dVisionZValue);
	SetDlgItemText(IDC_EDIT_EDGE,str);
}

void CWorkVisionTeach::OnInitDataLoad()
{
	CString strName,strTemp;
//	double dChk;
//	char chData[100];
	CString str_1;

// 	for (int i =0 ; i<4; i++)
// 	{
// 		for (int j =0; j<4; j++)
// 		{
// 			//Left X
// 			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
// 			:: GetPrivateProfileString(_T("Mark Left X Teach"),strName, _T("0"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
// 			strTemp.Format(_T("%s"), chData);
// 			dChk = _wtof(strTemp);
// 			st_teach.m_dLeftVisionMarkXPos[i][j] = dChk;
// 
// 			//Right X
// 			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
// 			:: GetPrivateProfileString(_T("Mark Right X Teach"),strName, _T("0"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
// 			strTemp.Format(_T("%s"), chData);
// 			dChk = _wtof(strTemp);
// 			st_teach.m_dRightVisionMarkXPos[i][j] = dChk;
// 
// 			//Left Y
// 			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
// 			:: GetPrivateProfileString(_T("Mark Left Y Teach"),strName, _T("0"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
// 			strTemp.Format(_T("%s"), chData);
// 			dChk = _wtof(strTemp);
// 			st_teach.m_dLeftVisionMarkYPos[i][j] = dChk;
// 
// 			//RIght Y
// 			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
// 			:: GetPrivateProfileString(_T("Mark Right Y Teach"),strName, _T("0"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
// 			strTemp.Format(_T("%s"), chData);
// 			dChk = _wtof(strTemp);
// 			st_teach.m_dRightVisionMarkYPos[i][j] = dChk;
// 
// 			//Left Z Pos
// 			strName.Format(_T("%d_Left Site_Z_Pos"), i+1);
// 			:: GetPrivateProfileString(_T("Mark Left Z Teach"),strName, _T("0"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
// 			strTemp.Format(_T("%s"), chData);
// 			dChk = _wtof(strTemp);
// 			st_teach.m_dFocusTeachZPos[i] = dChk;
// 
// 			//Right Z Pos
// 			strName.Format(_T("%d_Right Site_Z_Pos"), i+5);
// 			:: GetPrivateProfileString(_T("Mark Right Z Teach"),strName, _T("0"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
// 			strTemp.Format(_T("%s"), chData);
// 			dChk = _wtof(strTemp);
// 			st_teach.m_dFocusTeachZPos[i + 4] = dChk;
// 
// 
// 		}
// 	}
// 
// 	/////kwlee 2017.0103
// 	:: GetPrivateProfileString(_T("BASIC_SCREEN"),_T("m_dRobotCameraOffSetXPos"), _T("0"), (LPWSTR)chData, sizeof(chData), st_path_info.strFileBasic);
// 	strTemp.Format(_T("%s"), chData);
// 	dChk = _wtof(strTemp);
// 	st_teach.m_dRobotCameraOffSetXPos = dChk;
// 
// 	:: GetPrivateProfileString(_T("BASIC_SCREEN"),_T("m_dRobotCameraOffSetYPos"), _T("0"),(LPWSTR)chData, sizeof(chData), st_path_info.strFileBasic);
// 	strTemp.Format(_T("%s"), chData);
// 	dChk = _wtof(strTemp);
// 	st_teach.m_dRobotCameraOffSetYPos = dChk;
// 
// 	:: GetPrivateProfileString(_T("BASIC_SCREEN"),_T("m_dMarkToSocketXPos"), _T("0"), (LPWSTR)chData, sizeof(chData), st_path_info.strFileBasic);
// 	strTemp.Format(_T("%s"), chData);
// 	dChk = _wtof(strTemp);
// 	st_teach.m_dMarkToSocketXPos = dChk;
// 
// 	:: GetPrivateProfileString(_T("BASIC_SCREEN"),_T("m_dMarkToSocketYPos"), _T("0"), (LPWSTR)chData, sizeof(chData), st_path_info.strFileBasic);
// 	strTemp.Format(_T("%s"), chData);
// 	dChk = _wtof(strTemp);
// 	st_teach.m_dMarkToSocketYPos = dChk;
	/////////////////////////////////////
	
	str_1.Format(_T("%.3f"),st_teach.m_dRobotCameraOffSetXPos);
	SetDlgItemText(IDC_VISION_X,str_1);

	str_1.Format(_T("%.3f"),st_teach.m_dRobotCameraOffSetYPos);
	SetDlgItemText(IDC_VISION_Y,str_1);

	str_1.Format(_T("%.3f"),st_teach.m_dMarkToSocketXPos);
	SetDlgItemText(IDC_SOCKET_X,str_1);

	str_1.Format(_T("%.3f"),st_teach.m_dMarkToSocketYPos);
	SetDlgItemText(IDC_SOCkET_Y,str_1);
	//////////////
}
void CWorkVisionTeach::OnBnClickedBtnLoad()
{
	CString strName,strTemp;
	double dChk;
	char chData[100];

	for (int i =0 ; i<4; i++)
	{
		for (int j =0; j<4; j++)
		{
			//Left X
			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
			:: GetPrivateProfileString(_T("Mark Left X Teach"),strName, _T("0"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
			strTemp.Format(_T("%s"), chData);
			dChk = _wtof(strTemp);
			st_teach.m_dLeftVisionMarkXPos[i][j] = dChk;

			//Right X
			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
			:: GetPrivateProfileString(_T("Mark Right X Teach"),strName, _T("0"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
			strTemp.Format(_T("%s"), chData);
			dChk = _wtof(strTemp);
			st_teach.m_dRightVisionMarkXPos[i][j] = dChk;

			//Left Y
			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
			:: GetPrivateProfileString(_T("Mark Left Y Teach"),strName, _T("0"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
			strTemp.Format(_T("%s"), chData);
			dChk = _wtof(strTemp);
			st_teach.m_dLeftVisionMarkYPos[i][j] = dChk;

			//RIght Y
			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
			:: GetPrivateProfileString(_T("Mark Right Y Teach"),strName, _T("0"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
			strTemp.Format(_T("%s"), chData);
			dChk = _wtof(strTemp);
			st_teach.m_dRightVisionMarkYPos[i][j] = dChk;

			//Left Z Pos
			strName.Format(_T("%d_Left Site_Z_Pos"), i+1);
			:: GetPrivateProfileString(_T("Mark Left Z Teach"),strName, _T("0"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
			strTemp.Format(_T("%s"), chData);
			dChk = _wtof(strTemp);
			st_teach.m_dFocusTeachZPos[i] = dChk;

			//Right Z Pos
			strName.Format(_T("%d_Right Site_Z_Pos"), i+5);
			:: GetPrivateProfileString(_T("Mark Right Z Teach"),strName, _T("0"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
			strTemp.Format(_T("%s"), chData);
			dChk = _wtof(strTemp);
			st_teach.m_dFocusTeachZPos[i + 4] = dChk;
		}
	}
}
//kwlee 2017.0111
void CWorkVisionTeach::VisionMarkSave()
{
	CString strName,strTemp;

	for (int i =0 ; i < 4; i++)
	{
		for (int j =0; j<4; j++)
		{
			//Left X
			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
			strTemp.Format(_T("%.3f"), st_teach.m_dLeftVisionMarkXPos[i][j]);
			:: WritePrivateProfileString(_T("Mark Left X Teach"),strName, LPCTSTR(strTemp), st_path_info.strFileBasic);

			//Right X
			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
			strTemp.Format(_T("%.3f"), st_teach.m_dRightVisionMarkXPos[i][j]);
			:: WritePrivateProfileString(_T("Mark Right X Teach"),strName, LPCTSTR(strTemp), st_path_info.strFileBasic);

			//Left Y
			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
			strTemp.Format(_T("%.3f"), st_teach.m_dLeftVisionMarkYPos[i][j]);
			:: WritePrivateProfileString(_T("Mark Left Y Teach"),strName, LPCTSTR(strTemp), st_path_info.strFileBasic);

			//Right Y
			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
			strTemp.Format(_T("%.3f"), st_teach.m_dRightVisionMarkYPos[i][j]);
			:: WritePrivateProfileString(_T("Mark Right Y Teach"),strName, LPCTSTR(strTemp), st_path_info.strFileBasic);

			//Left Z
			strName.Format(_T("%d_Left Site_Z_Pos"), i+1);
			strTemp.Format(_T("%.3f"), st_teach.m_dFocusTeachZPos[i]);
			:: WritePrivateProfileString(_T("Mark Left Z Teach"),strName, LPCTSTR(strTemp), st_path_info.strFileBasic);

			//Right Z
			strName.Format(_T("%d_Right Site_Z_Pos"), i+5);
			strTemp.Format(_T("%.3f"), st_teach.m_dFocusTeachZPos[i + 4]);
			:: WritePrivateProfileString(_T("Mark Right Z Teach"),strName, LPCTSTR(strTemp), st_path_info.strFileBasic);

		}
	}
}
void CWorkVisionTeach::VisionRefSave()
{

	CString strTemp;
	//kwlee 2017.0103
	GetDlgItemText(IDC_VISION_X,strTemp);
	st_teach.m_dRobotCameraOffSetXPos = _wtof(strTemp);

	GetDlgItemText(IDC_VISION_Y,strTemp);
	st_teach.m_dRobotCameraOffSetYPos = _wtof(strTemp);

	GetDlgItemText(IDC_SOCKET_X,strTemp);
	st_teach.m_dMarkToSocketXPos = _wtof(strTemp);

	GetDlgItemText(IDC_SOCkET_Y,strTemp);
	st_teach.m_dMarkToSocketYPos = _wtof(strTemp);

	//kwlee 2017.0110
	st_handler_info.m_iVisionOffSetAutoMode =  m_nAutoTeachModeUse;

	strTemp.Format(_T("%d"),st_handler_info.m_iVisionOffSetAutoMode);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"),_T("m_iVisionOffSetAutoMode"), LPCTSTR(strTemp), st_path_info.strFileBasic);
	////

	strTemp.Format(_T("%.3f"),st_teach.m_dRobotCameraOffSetXPos);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"),_T("m_dRobotCameraOffSetXPos"), LPCTSTR(strTemp), st_path_info.strFileBasic);

	strTemp.Format(_T("%.3f"), st_teach.m_dRobotCameraOffSetYPos);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"),_T("m_dRobotCameraOffSetYPos"), LPCTSTR(strTemp), st_path_info.strFileBasic);

	strTemp.Format(_T("%.3f"), st_teach.m_dMarkToSocketXPos);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"),_T("m_dMarkToSocketXPos"), LPCTSTR(strTemp), st_path_info.strFileBasic);

	strTemp.Format(_T("%.3f"), st_teach.m_dMarkToSocketYPos);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"),_T("m_dMarkToSocketYPos"), LPCTSTR(strTemp), st_path_info.strFileBasic);
}
void CWorkVisionTeach::OnVisionReadSave()
{
	CString strName,strTemp;

	for (int i =0 ; i < 4; i++)
	{
		for (int j =0; j<4; j++)
		{
			//Left X
			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
			strTemp.Format(_T("%.3f"), st_teach.m_dLeftVisionReadXPos[i][j]);
			:: WritePrivateProfileString(_T("Vision Read Left X Offset"),strName, LPCTSTR(strTemp), st_path_info.strFileBasic);

			//Right X
			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
			strTemp.Format(_T("%.3f"), st_teach.m_dRightVisionReadXPos[i][j]);
			:: WritePrivateProfileString(_T("Vision Read Right X Offset"),strName, LPCTSTR(strTemp), st_path_info.strFileBasic);

			//Left Y
			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
			strTemp.Format(_T("%.3f"), st_teach.m_dLeftVisionReadYPos[i][j]);
			:: WritePrivateProfileString(_T("Vision Read Left Y Offset"),strName, LPCTSTR(strTemp), st_path_info.strFileBasic);

			//Right Y
			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
			strTemp.Format(_T("%.3f"), st_teach.m_dRightVisionReadYPos[i][j]);
			:: WritePrivateProfileString(_T("Vision Read Right Y Offset"),strName, LPCTSTR(strTemp), st_path_info.strFileBasic);

		}
	}
}
void CWorkVisionTeach::VisionTeachSave()
{
	CString strName,strTemp;

	for (int i =0 ; i < 4; i++)
	{
		for (int j =0; j<4; j++)
		{
			//Left X
			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
			strTemp.Format(_T("%.3f"), st_teach.m_dLeftVisionTeachXPos[i][j]);
			:: WritePrivateProfileString(_T("Vision Left X Teach"),strName, LPCTSTR(strTemp), st_path_info.strFileBasic);

			//Right X
			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
			strTemp.Format(_T("%.3f"), st_teach.m_dRightVisionTeachXPos[i][j]);
			:: WritePrivateProfileString(_T("Vision Right X Teach"),strName, LPCTSTR(strTemp), st_path_info.strFileBasic);

			//Left Y
			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
			strTemp.Format(_T("%.3f"), st_teach.m_dLeftVisionTeachYPos[i][j]);
			:: WritePrivateProfileString(_T("Vision Left Y Teach"),strName, LPCTSTR(strTemp), st_path_info.strFileBasic);

			//Right Y
			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
			strTemp.Format(_T("%.3f"), st_teach.m_dRightVisionTeachYPos[i][j]);
			:: WritePrivateProfileString(_T("Vision Right Y Teach"),strName, LPCTSTR(strTemp), st_path_info.strFileBasic);

		}
	}
}
void CWorkVisionTeach::OnVisionOffsetValSave()
{
	CString strName,strTemp;

	for (int i =0 ; i < 4; i++)
	{
		for (int j =0; j<4; j++)
		{
			//Left X
			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
			strTemp.Format(_T("%.3f"),st_teach.m_dLeftVisionOffSetXPos[i][j]);
			:: WritePrivateProfileString(_T("Vision Left X Teach Offset Val"),strName, LPCTSTR(strTemp), st_path_info.strFileBasic);

			//Right X
			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
			strTemp.Format(_T("%.3f"),st_teach.m_dRightVisionOffSetXPos[i][j]);
			:: WritePrivateProfileString(_T("Vision Right X Teach Offset Val"),strName, LPCTSTR(strTemp), st_path_info.strFileBasic);

			//Left Y
			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
			strTemp.Format(_T("%.3f"),st_teach.m_dLeftVisionOffSetYPos[i][j]);
			:: WritePrivateProfileString(_T("Vision Left Y Teach Offset Val"),strName, LPCTSTR(strTemp), st_path_info.strFileBasic);

			//Right Y
			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
			strTemp.Format(_T("%.3f"),st_teach.m_dRightVisionOffSetYPos[i][j]);
			:: WritePrivateProfileString(_T("Vision Right Y Teach Offset Val"),strName, LPCTSTR(strTemp), st_path_info.strFileBasic);

		}
	}
}

void CWorkVisionTeach::OnBnClickedBtnSave()
{
	
	VisionMarkSave(); //vision mark
	VisionTeachSave(); //Vision Teach 가야할 곳.
	OnVisionReadSave(); //Vision Read 받아온 값
	VisionRefSave();
	OnVisionOffsetValSave();

}

void CWorkVisionTeach::OnMarkLeftTeachCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{

	int nRow, nCol;
	
	double dKey;

	CString strTemp;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	if (nRow <= 0 || nRow == 1) return;
	//kwlee 2016.1019
	if (nRow == 5 && nCol == 3) return; //Tsite_4 Mark3 X_Pos
	if (nRow == 5 && nCol == 4) return; //Tsite_4 Mark4 X_Pos

	if (nRow == 5 && nCol == 7) return;	//Tsite_4 Mark3 Y_Pos
	if (nRow == 5 && nCol == 8) return;	//Tsite_4 Mark4 Y_Pos
	if (nCol < 5)
	{
		dKey = st_teach.m_dLeftVisionMarkXPos[nRow - 2][nCol - 1];
	}
	else if(nCol < 9)
	{
		dKey = st_teach.m_dLeftVisionMarkYPos[nRow - 2][nCol - 5];
	}
	else
	{
		dKey = st_teach.m_dFocusTeachZPos[nRow - 2];
	}
	

	KeyPadD(0.0f, 10000.0f, &dKey);

	if (nCol < 5)
	{
		st_teach.m_dLeftVisionMarkXPos[nRow - 2][nCol - 1] = dKey; //4 //4
	}
	else if(nCol < 9)
	{
		st_teach.m_dLeftVisionMarkYPos[nRow - 2][nCol - 5] = dKey; //4 //4
	}
	else
	{
		st_teach.m_dFocusTeachZPos[nRow - 2] = dKey;
	}

	m_pGridLeftTeach.Invalidate(FALSE);
}

void CWorkVisionTeach::OnMarkRightTeachCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{

	int nRow, nCol;

	double dKey;

	CString strTemp;

	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	if (nRow <= 0 || nRow == 1) return;

	//kwlee 2016.1019
	if (nRow == 5 && nCol == 3) return; //Tsite_8 Mark3 X_Pos
	if (nRow == 5 && nCol == 4) return; //Tsite_8 Mark4 X_Pos

	if (nRow == 5 && nCol == 7) return;	//Tsite_8 Mark3 Y_Pos
	if (nRow == 5 && nCol == 8) return;	//Tsite_8 Mark4 Y_Pos

	if (nCol < 5)
	{
		dKey = st_teach.m_dRightVisionMarkXPos[nRow - 2][nCol - 1];
	}
	else if(nCol < 9)
	{
		dKey = st_teach.m_dRightVisionMarkYPos[nRow - 2][nCol - 5];
	}
	else
	{
		dKey = st_teach.m_dFocusTeachZPos[nRow + 2 ];
	}


	KeyPadD(0.0f, 10000.0f, &dKey);

	if (nCol < 5)
	{
		st_teach.m_dRightVisionMarkXPos[nRow - 2][nCol - 1] = dKey; //4 //4
	}
	else if (nCol < 9)
	{
		st_teach.m_dRightVisionMarkYPos[nRow - 2][nCol - 5] = dKey; //4 //4
	}
	else
	{
		st_teach.m_dFocusTeachZPos[nRow + 2 ] = dKey;
	}
	

	m_pGridRightTeach.Invalidate(FALSE);
}

void CWorkVisionTeach::OnBnClickedBtnLeftTsiteRbtTeachGo2()
{
	COMI.Set_MotStop(1, M_TEST_RBT_Z) ; //긴급정지 
	KillTimer(TM_VISION_TEACH);
	KillTimer(TM_VISION_FCOUS);
	KillTimer(TM_VISION_POS_SET);
}


void CWorkVisionTeach::OnBnClickedBtnTestSiteAddOffset()
{	
	if (m_nAutoTeachModeUse == YES)
	{	
		m_nAutoTeachModeUse = NO;
		m_btnAutoTeachUse.SetWindowText(_T("Auto Teach Skip"));

	}
	else
	{
		m_nAutoTeachModeUse = YES;
		m_btnAutoTeachUse.SetWindowText(_T("Auto Teach Use"));
	}
	Invalidate(FALSE);
}

void CWorkVisionTeach::OnBnClickedBtnTsiteRbtSetGo()
{
	if (st_lot_info[LOT_CURR].nLotStatus >= LOT_START)
	{
		return;
	}
	if (st_handler_info.nRunStatus		!= dSTOP &&
		st_handler_info.nMachineMode	!= MACHINE_MANUAL)
	{
		return;
	}
	
 	m_nValueSetStep = 0;
 	SetTimer(TM_VISION_POS_SET, 100, NULL);
}


void CWorkVisionTeach::OnBnClickedBtnTestSiteValueSet()
{
	CString strLogX, strLogY;	
	CDialog_Message		dlgMsg;
	int					nResponse;

	int m_nRobot_X = M_TEST_RBT_X;
	int m_nRobot_Y = M_TEST_RBT_Y;
	int nCnt = 0;

	//kwlee 2017.0118 현재 임시 막아놓음...
	return;

	nCnt = OnVisionOffsetCheck();

	if (nCnt > 0)
	{
		dlgMsg.m_nMessageType	= 1;
		dlgMsg.m_strMessage.Format(_T("Teaching 값 보다 1mm 이상 차이납니다. 적용 하시겠습니까? "));

		nResponse =  (int)dlgMsg.DoModal();

		if (nResponse == IDOK)
		{
			OnVisionDataApply();
		}
	}
	else
	{
		dlgMsg.m_nMessageType	= 1;
		dlgMsg.m_strMessage.Format(_T("적용 하시겠습니까?"));

		nResponse =  (int)dlgMsg.DoModal();

		if (nResponse == IDOK)
		{
			OnVisionDataApply();
		}
	}
}
int CWorkVisionTeach::OnVisionOffsetCheck()
{
	CString strLogX, strLogY;
	int m_nRobot_X = M_TEST_RBT_X;
	int m_nRobot_Y = M_TEST_RBT_Y;
//	int	nResponse;
	int nCnt = 0;

	for (int i =0; i<8;i++ ) //Testsite
	{
		for (int j = 0; j<4; j++)//Mark
		{
			if( ( i == 0 && j >= 2 ) || ( i == 1 && j >= 2 ) || ( i == 2 && j >= 2 ) ||( i == 3 && j >= 2 ) ||

				( i == 4 && j >= 2 ) ||( i == 5 && j >= 2 ) ||( i == 6 && j >= 2 ) ||( i == 7 && j >= 2 ) )	
			{
				break;
			}

			if( i < 4)//LEFT 사이트
			{
				if (j%2)
				{
					st_teach.m_dLeftVisionOffSetXPos[i][j] = st_motor_info[m_nRobot_X].d_pos[ P_TESTRBT_XY_TESTDUCT_1_RIGHT_START + (i * 2)] - st_teach.m_dLeftVisionTeachXPos[i][j];
					st_teach.m_dLeftVisionOffSetYPos[i][j] = st_motor_info[m_nRobot_Y].d_pos[ P_TESTRBT_XY_TESTDUCT_1_RIGHT_START + (i * 2)] - st_teach.m_dLeftVisionTeachYPos[i][j];
				}
				else
				{
					st_teach.m_dLeftVisionOffSetXPos[i][j] = st_motor_info[m_nRobot_X].d_pos[ P_TESTRBT_XY_TESTDUCT_1_LEFT_START + (i * 2)] - st_teach.m_dLeftVisionTeachXPos[i][j];
					st_teach.m_dLeftVisionOffSetYPos[i][j] = st_motor_info[m_nRobot_Y].d_pos[ P_TESTRBT_XY_TESTDUCT_1_LEFT_START + (i * 2)] - st_teach.m_dLeftVisionTeachYPos[i][j];

				}

				if (st_teach.m_dLeftVisionOffSetYPos[i][j] > 1 || st_teach.m_dLeftVisionOffSetXPos[i][j] > 1 )
				{
					if (j%2)
					{
						if (st_teach.m_dLeftVisionOffSetXPos[i][j] > 1)
						{
							strLogX.Format( _T("[Site %02d Mark : %02d] X Pos : Vision Teach : [%3.3f]-> MotorTeach : [%3.3f] OffSet[%3.3f]"),  i+1, j + 1,  st_teach.m_dLeftVisionTeachXPos[i][j], st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_TESTDUCT_1_RIGHT_START + (i * 2) ], st_teach.m_dLeftVisionOffSetXPos[i][j] );
							clsMem.OnAbNormalMessagWrite(strLogX);
						}
						else if (st_teach.m_dLeftVisionOffSetXPos[i][j] > 1)
						{
							strLogY.Format( _T("[Site %02d Mark : %02d] Y Pos : Vision Teach : [%3.3f]-> MotorTeach : [%3.3f] OffSet[%3.3f]"),  i+1, j + 1,  st_teach.m_dLeftVisionTeachYPos[i][j], st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_TESTDUCT_1_RIGHT_START + (i * 2) ], st_teach.m_dLeftVisionOffSetYPos[i][j] );
							clsMem.OnAbNormalMessagWrite(strLogY);
						}
						
						
					}
					else
					{
						if (st_teach.m_dLeftVisionOffSetXPos[i][j] > 1)
						{
							strLogX.Format( _T("[Site %02d Mark : %02d] X Pos : Vision Teach : [%3.3f]-> MotorTeach : [%3.3f] OffSet[%3.3f]"),  i+1, j + 1,  st_teach.m_dLeftVisionTeachXPos[i][j], st_motor_info[m_nRobot_X].d_pos[P_TESTRBT_XY_TESTDUCT_1_LEFT_START + (i * 2) ], st_teach.m_dLeftVisionOffSetXPos[i][j] );
							clsMem.OnAbNormalMessagWrite(strLogX);
						}
						else if (st_teach.m_dLeftVisionOffSetXPos[i][j] > 1)
						{
							strLogY.Format( _T("[Site %02d Mark : %02d] Y Pos : Vision Teach : [%3.3f]-> MotorTeach : [%3.3f] OffSet[%3.3f]"),  i+1, j + 1,  st_teach.m_dLeftVisionTeachYPos[i][j], st_motor_info[m_nRobot_Y].d_pos[P_TESTRBT_XY_TESTDUCT_1_LEFT_START + (i * 2) ], st_teach.m_dLeftVisionOffSetYPos[i][j] );
							clsMem.OnAbNormalMessagWrite(strLogY);
						}
						
					}
					nCnt++;
				}
			}
			else
			{
				if (j%2)
				{
					st_teach.m_dRightVisionOffSetXPos[i - 4][j] = st_motor_info[m_nRobot_X].d_pos[ P_TESTRBT_XY_TESTDUCT_5_RIGHT_START + ((i - 4) * 2)] - st_teach.m_dRightVisionTeachXPos[i  - 4][j];
					st_teach.m_dRightVisionOffSetYPos[i - 4][j] = st_motor_info[m_nRobot_Y].d_pos[ P_TESTRBT_XY_TESTDUCT_5_RIGHT_START + ((i - 4) * 2) ] - st_teach.m_dRightVisionTeachYPos[i  - 4][j];
				}
				else
				{
					st_teach.m_dRightVisionOffSetXPos[i - 4][j] = st_motor_info[m_nRobot_X].d_pos[ P_TESTRBT_XY_TESTDUCT_5_LEFT_START + ((i - 4) * 2) ] - st_teach.m_dRightVisionTeachXPos[i  - 4][j];
					st_teach.m_dRightVisionOffSetYPos[i - 4][j] = st_motor_info[m_nRobot_Y].d_pos[ P_TESTRBT_XY_TESTDUCT_5_LEFT_START + ((i - 4) * 2) ] - st_teach.m_dRightVisionTeachYPos[i  - 4][j];
				}

				if (st_teach.m_dRightVisionOffSetXPos[i - 4][j] > 1 || st_teach.m_dRightVisionOffSetYPos[i - 4][j] > 1 )
				{
					if (j%2)
					{
						if(st_teach.m_dRightVisionOffSetXPos[i - 4][j] > 1)
						{
							strLogX.Format( _T("[Site %02d Mark : %02d] X Pos : Vision Teach : [%3.3f]-> MotorTeach : [%3.3f] OffSet[%3.3f]"),  i+1, j + 1,  st_teach.m_dRightVisionTeachXPos[i  - 4][j], st_motor_info[m_nRobot_X].d_pos[ P_TESTRBT_XY_TESTDUCT_5_RIGHT_START + ((i - 4) * 2) ], st_teach.m_dRightVisionOffSetXPos[i  - 4][j] );
							clsMem.OnAbNormalMessagWrite(strLogX);
						}
						else /*if (st_teach.m_dRightVisionOffSetYPos[i - 4][j] > 1)*/ 
						{
							strLogY.Format( _T("[Site %02d Mark : %02d] Y Pos : Vision Teach : [%3.3f]-> MotorTeach : [%3.3f] OffSet[%3.3f]"),  i+1, j + 1,  st_teach.m_dRightVisionTeachYPos[i  - 4][j], st_motor_info[m_nRobot_Y].d_pos[ P_TESTRBT_XY_TESTDUCT_5_RIGHT_START + ((i - 4) * 2) ], st_teach.m_dRightVisionOffSetYPos[i  - 4][j] );
							clsMem.OnAbNormalMessagWrite(strLogY);
						}
					}
					else
					{
						if(st_teach.m_dRightVisionOffSetXPos[i - 4][j] > 1)
						{
							strLogX.Format( _T("[Site %02d Mark : %02d] X Pos : Vision Teach : [%3.3f]-> MotorTeach : [%3.2f] OffSet[%3.3f]"),  i+1, j + 1,  st_teach.m_dRightVisionTeachXPos[i  - 4][j], st_motor_info[m_nRobot_X].d_pos[ P_TESTRBT_XY_TESTDUCT_5_LEFT_START + ((i - 4) * 2) ], st_teach.m_dRightVisionOffSetXPos[i  - 4][j] );
							clsMem.OnAbNormalMessagWrite(strLogX);
						}
						else /*if(st_teach.m_dRightVisionOffSetYPos[i - 4][j] > 1)*/ 
						{
							strLogY.Format( _T("[Site %02d Mark : %02d] Y Pos : Vision Teach : [%3.3f]-> MotorTeach : [%3.2f] OffSet[%3.3f]"),  i+1, j + 1,  st_teach.m_dRightVisionTeachYPos[m_nSite_No  - 4][j], st_motor_info[m_nRobot_Y].d_pos[ P_TESTRBT_XY_TESTDUCT_5_LEFT_START + ((i - 4) * 2) ], st_teach.m_dRightVisionOffSetYPos[i  - 4][j] );
							clsMem.OnAbNormalMessagWrite(strLogY);
						}
					}
					nCnt++;
				}
			}
		}
	}
	return nCnt;

	
}

void CWorkVisionTeach::OnVisionDataApply()
{
	int m_nRobot_X = M_TEST_RBT_X;
	int m_nRobot_Y = M_TEST_RBT_Y;

	for (int i =0; i<8;i++ ) //Testsite
	{
		for (int j = 0; j<4; j++)//Mark
		{
			if( ( i == 0 && j >= 2 ) || ( i == 1 && j >= 2 ) || ( i == 2 && j >= 2 ) ||( i == 3 && j >= 2 ) ||

				( i == 4 && j >= 2 ) ||( i == 5 && j >= 2 ) ||( i == 6 && j >= 2 ) ||( i == 7 && j >= 2 ) )	
			{
				break;
			}

			if( i < 4)//LEFT 사이트
			{
				//kwlee 2016.1219
				if (j%2)
				{
					st_motor_info[m_nRobot_X].d_pos[ P_TESTRBT_XY_TESTDUCT_1_RIGHT_START + (i * 2)] = st_teach.m_dLeftVisionTeachXPos[i][j];
					st_motor_info[m_nRobot_Y].d_pos[ P_TESTRBT_XY_TESTDUCT_1_RIGHT_START + (i * 2)] = st_teach.m_dLeftVisionTeachYPos[i][j];
				}
				else
				{
					st_motor_info[m_nRobot_X].d_pos[ P_TESTRBT_XY_TESTDUCT_1_LEFT_START + (i * 2)] = st_teach.m_dLeftVisionTeachXPos[i][j];
					st_motor_info[m_nRobot_Y].d_pos[ P_TESTRBT_XY_TESTDUCT_1_LEFT_START + (i * 2)] = st_teach.m_dLeftVisionTeachYPos[i][j];
				}
			}
			else
			{
				if (j%2)
				{
					st_motor_info[m_nRobot_X].d_pos[ P_TESTRBT_XY_TESTDUCT_5_RIGHT_START + ((i - 4) * 2)] = st_teach.m_dRightVisionTeachXPos[i  - 4][j];
					st_motor_info[m_nRobot_Y].d_pos[ P_TESTRBT_XY_TESTDUCT_5_RIGHT_START + ((i - 4) * 2) ] = st_teach.m_dRightVisionTeachYPos[i  - 4][j];
				}
				else
				{
					st_motor_info[m_nRobot_X].d_pos[ P_TESTRBT_XY_TESTDUCT_5_LEFT_START + ((i - 4) * 2) ] = st_teach.m_dRightVisionTeachXPos[i  - 4][j];
					st_motor_info[m_nRobot_Y].d_pos[ P_TESTRBT_XY_TESTDUCT_5_LEFT_START + ((i - 4) * 2) ] = st_teach.m_dRightVisionTeachYPos[i  - 4][j];
				}
			}
		}
	}
}
