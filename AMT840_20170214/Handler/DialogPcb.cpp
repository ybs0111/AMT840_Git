// DialogPcb.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "DialogPcb.h"
#include "afxdialogex.h"
#include "PublicFunction.h"
#include "Dialog_KeyBoard.h"
#include "Dialog_KeyPad.h"
#include "Dialog_Message.h"
#include "AlgMemory.h"
#include ".\\Ctrl\\KeyBoard.h"
// CDialogPcb 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialogPcb, CDialog)

CDialogPcb::CDialogPcb(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogPcb::IDD, pParent)
{

}

CDialogPcb::CDialogPcb(tagPCB_INFO &st_pcb_info)
{
	m_pPcb_info = &st_pcb_info;
	m_pcb_info	= *m_pPcb_info;
}

CDialogPcb::~CDialogPcb()
{
}

void CDialogPcb::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GROUP_PCB_DIALOG_COMMAND_INFO,			m_groupCommand);
	DDX_Control(pDX, IDC_BTN_PCB_DIALOG_START,					m_btnStart);
	DDX_Control(pDX, IDC_BTN_PCB_DIALOG_ABORT,					m_btnAbort);
	DDX_Control(pDX, IDC_BTN_PCB_DIALOG_PASS,					m_btnPass);
	DDX_Control(pDX, IDC_BTN_PCB_DIALOG_FAIL,					m_btnFail);
	DDX_Control(pDX, IDC_BTN_PCB_DIALOG_APPLY,					m_btnApply);
	DDX_Control(pDX, IDC_BTN_PCB_DIALOG_CANCEL,					m_btnCancel);
	DDX_Control(pDX, IDC_MSG_PCB_DIALOG_MESSAGE,				m_msgMsg);
	DDX_Control(pDX, IDC_CUSTOM_PCB_DATA,						m_pGridPcb);
}


BEGIN_MESSAGE_MAP(CDialogPcb, CDialog)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_PCB_DATA, &CDialogPcb::OnPcbCellClick)
	ON_BN_CLICKED(IDCANCEL, &CDialogPcb::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_PCB_DIALOG_START, &CDialogPcb::OnBnClickedBtnPcbDialogStart)
	ON_BN_CLICKED(IDC_BTN_PCB_DIALOG_ABORT, &CDialogPcb::OnBnClickedBtnPcbDialogAbort)
	ON_BN_CLICKED(IDC_BTN_PCB_DIALOG_PASS, &CDialogPcb::OnBnClickedBtnPcbDialogPass)
	ON_BN_CLICKED(IDC_BTN_PCB_DIALOG_FAIL, &CDialogPcb::OnBnClickedBtnPcbDialogFail)
	ON_BN_CLICKED(IDC_BTN_PCB_DIALOG_APPLY, &CDialogPcb::OnBnClickedBtnPcbDialogApply)
	ON_BN_CLICKED(IDC_BTN_PCB_DIALOG_CANCEL, &CDialogPcb::OnBnClickedBtnPcbDialogCancel)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


BOOL CDialogPcb::OnInitDialog()
{
	CDialog::OnInitDialog();

	OnInitGridPcb();
	OnInitGroupBox();
	OnInitButton();
	OnInitLabel();

	if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
	{
		clsMem.OnNormalMessageWrite(_T("PCB VIEW Display"));
		st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDialogPcb::OnInitGridPcb()
{
	int   i, j;
	int	  max_row, max_col;
	int   row;

	CString strTmp;

	max_row = 6;
	max_col = 4;

	m_pGridPcb.SetFrameFocusCell(FALSE);
	m_pGridPcb.SetTrackFocusCell(FALSE);
	m_pGridPcb.EnableSelection(FALSE);

	m_pGridPcb.SetGridLineColor(BLACK_C);
	m_pGridPcb.SetGridLines(1);

	m_pGridPcb.SetRowCount(max_row);
	m_pGridPcb.SetColumnCount(max_col);

	m_pGridPcb.SetFixedRowCount(0);
	m_pGridPcb.SetFixedColumnCount(0);
	m_pGridPcb.SetFixedBkColor(RGB(0,0,200));
	m_pGridPcb.SetFixedBkColor(RGB(200,200,255));
	m_pGridPcb.SetTextBkColor(RGB(150,150,200));
	
	for (i=0; i<max_row; i++)
	{
		m_pGridPcb.SetRowHeight(i, 42);

		for (j=0; j<max_col; j++)
		{
			if ((j+1) % 2)
			{
				m_pGridPcb.SetColumnWidth(j, 90);
				m_pGridPcb.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
				m_pGridPcb.SetItemState(i, j, GVIS_READONLY);
				m_pGridPcb.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
			}
			else
			{
				m_pGridPcb.SetColumnWidth(j, 205);
				m_pGridPcb.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
				m_pGridPcb.SetItemState(i, j, GVIS_READONLY);
				m_pGridPcb.SetItemBkColour(i, j, WHITE_C, CLR_DEFAULT);
			}
		}
	}

	row = 0;
	m_pGridPcb.MergeCells(row, 0, row, 3);
	m_pGridPcb.SetItemBkColour(row, 0, ORANGE_C, BLACK_C);
	m_pGridPcb.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridPcb.SetItemText(row, 0, _T("PCB INFO"));

	row = 1;
	m_pGridPcb.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
	m_pGridPcb.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridPcb.SetItemText(row, 0, _T("Lot No"));

	m_pGridPcb.SetItemBkColour(row, 2, BLUE_D, WHITE_C);
	m_pGridPcb.SetItemFont(row, 2, &clsFunc.OnLogFont(16));
	m_pGridPcb.SetItemText(row, 2, _T("Retest Flag"));
		
	row = 2;
	m_pGridPcb.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
	m_pGridPcb.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridPcb.SetItemText(row, 0, _T("Part No"));

	m_pGridPcb.SetItemBkColour(row, 2, BLUE_D, WHITE_C);
	m_pGridPcb.SetItemFont(row, 2, &clsFunc.OnLogFont(16));
	m_pGridPcb.SetItemText(row, 2, _T("Retest Count"));

	row = 3;
	m_pGridPcb.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
	m_pGridPcb.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridPcb.SetItemText(row, 0, _T("1D"));

	m_pGridPcb.SetItemBkColour(row, 2, BLUE_D, WHITE_C);
	m_pGridPcb.SetItemFont(row, 2, &clsFunc.OnLogFont(16));
	m_pGridPcb.SetItemText(row, 2, _T("Start Time"));

	row = 4;
	m_pGridPcb.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
	m_pGridPcb.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridPcb.SetItemText(row, 0, _T("2D"));

	m_pGridPcb.SetItemBkColour(row, 2, BLUE_D, WHITE_C);
	m_pGridPcb.SetItemFont(row, 2, &clsFunc.OnLogFont(16));
	m_pGridPcb.SetItemText(row, 2, _T("Test Time"));

	row = 5;
	m_pGridPcb.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
	m_pGridPcb.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
	m_pGridPcb.SetItemText(row, 0, _T("2D"));

	m_pGridPcb.SetItemBkColour(row, 2, BLUE_D, WHITE_C);
	m_pGridPcb.SetItemFont(row, 2, &clsFunc.OnLogFont(16));
	m_pGridPcb.SetItemText(row, 2, _T("PCB Y/N"));

// 	row = 6;
// 	m_pGridPcb.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
// 	m_pGridPcb.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
// 	m_pGridPcb.SetItemText(row, 0, _T("PPID"));
// 
// 	m_pGridPcb.SetItemBkColour(row, 2, BLUE_D, WHITE_C);
// 	m_pGridPcb.SetItemFont(row, 2, &clsFunc.OnLogFont(16));
// 	m_pGridPcb.SetItemText(row, 2, _T("Board Num"));
// 
// 	row = 7;
// 	m_pGridPcb.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
// 	m_pGridPcb.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
// 	m_pGridPcb.SetItemText(row, 0, _T("WWN"));
// 
// 	m_pGridPcb.SetItemBkColour(row, 2, BLUE_D, WHITE_C);
// 	m_pGridPcb.SetItemFont(row, 2, &clsFunc.OnLogFont(16));
// 	m_pGridPcb.SetItemText(row, 2, _T("Result"));
// 
// 	row = 8;
// 	m_pGridPcb.SetItemBkColour(row, 0, BLUE_D, WHITE_C);
// 	m_pGridPcb.SetItemFont(row, 0, &clsFunc.OnLogFont(16));
// 	m_pGridPcb.SetItemText(row, 0, _T("Enable"));
// 
// 	m_pGridPcb.SetItemBkColour(row, 2, BLUE_D, WHITE_C);
// 	m_pGridPcb.SetItemFont(row, 2, &clsFunc.OnLogFont(16));
// 	m_pGridPcb.SetItemText(row, 2, _T("PCB Y/N"));
		
	OnPcbDataDisplay();
}

void CDialogPcb::OnPcbDataDisplay()
{
	CString			strTemp;

	m_pGridPcb.SetItemFont(1, 1, &clsFunc.OnLogFont(14));
	m_pGridPcb.SetItemText(1, 1, m_pcb_info.strLotNo);

	if (m_pcb_info.nRetest == YES) 
	{
		strTemp.Format(_T("YES"));
		m_pGridPcb.SetItemBkColour(1, 3, GREEN_L, BLACK_C);
	}
	else if (m_pcb_info.nRetest == NO)
	{
		strTemp.Format(_T("NO"));
		m_pGridPcb.SetItemBkColour(1, 3, RED_L, BLACK_C);
	}
	else
	{
		strTemp.Format(_T("-"));
		m_pGridPcb.SetItemBkColour(1, 3, WHITE_C, BLACK_C);
	}

	m_pGridPcb.SetItemFont(1, 3, &clsFunc.OnLogFont(14));
	m_pGridPcb.SetItemText(1, 3, strTemp);
	
	m_pGridPcb.SetItemFont(2, 1, &clsFunc.OnLogFont(14));
	m_pGridPcb.SetItemText(2, 1, m_pcb_info.strPartNo);

	strTemp.Format(_T("%d"), m_pcb_info.nRetestCnt);
	m_pGridPcb.SetItemFont(2, 3, &clsFunc.OnLogFont(14));
	m_pGridPcb.SetItemText(2, 3, strTemp);

	m_pGridPcb.SetItemFont(3, 1, &clsFunc.OnLogFont(14));
	m_pGridPcb.SetItemText(3, 1, m_pcb_info.strBarcode1D[0]);

	strTemp.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"), m_pcb_info.tStart.GetYear(), m_pcb_info.tStart.GetMonth(), m_pcb_info.tStart.GetDay()
												, m_pcb_info.tStart.GetHour(), m_pcb_info.tStart.GetMinute(), m_pcb_info.tStart.GetSecond());
	m_pGridPcb.SetItemFont(3, 3, &clsFunc.OnLogFont(14));
	m_pGridPcb.SetItemText(3, 3, strTemp);

	m_pGridPcb.SetItemFont(4, 1, &clsFunc.OnLogFont(14));
	// jtkim 20160929
	m_pGridPcb.SetItemText(4, 1, m_pcb_info.strBarcode2D[0]);

	strTemp.Format(_T("%d"), m_pcb_info.dwBdTime);
	m_pGridPcb.SetItemFont(4, 3, &clsFunc.OnLogFont(14));
	m_pGridPcb.SetItemText(4, 3, strTemp);

	m_pGridPcb.SetItemFont(5, 1, &clsFunc.OnLogFont(14));
	// jtkim 20160929
	m_pGridPcb.SetItemText(5, 1, m_pcb_info.strBarcode2D[0]);


	if (m_pcb_info.nYesNo == YES)
	{
		strTemp.Format(_T("YES"));
		m_pGridPcb.SetItemBkColour(5, 3, GREEN_L, BLACK_C);
	}
	else if (m_pcb_info.nYesNo == NO)
	{
		strTemp.Format(_T("NO"));
		m_pGridPcb.SetItemBkColour(5, 3, RED_L, BLACK_C);
	}

	m_pGridPcb.SetItemFont(5, 3, &clsFunc.OnLogFont(14));
	m_pGridPcb.SetItemText(5, 3, strTemp);

	//strTemp.Format(_T("%d"), m_pcb_info.nCOK_IDNum);
// 	m_pGridPcb.SetItemFont(5, 3, &clsFunc.OnLogFont(14));
// 	m_pGridPcb.SetItemText(5, 3, m_pcb_info.strCSerialNo2);

// 	m_pGridPcb.SetItemFont(6, 1, &clsFunc.OnLogFont(14));
// 	m_pGridPcb.SetItemText(6, 1, m_pcb_info.strPPID);
// 
// 	strTemp.Format(_T("%d"), m_pcb_info.nBdNum);
// 	m_pGridPcb.SetItemFont(6, 3, &clsFunc.OnLogFont(14));
// 	m_pGridPcb.SetItemText(6, 3, strTemp);
// 
// 	m_pGridPcb.SetItemFont(7, 1, &clsFunc.OnLogFont(14));
// 	m_pGridPcb.SetItemText(7, 1, m_pcb_info.strWWN);
// 
// 	if (m_pcb_info.nBin == BD_DATA_GOOD) 
// 	{
// 		strTemp.Format(_T("GOOD"));
// 		m_pGridPcb.SetItemBkColour(7, 3, GREEN_L, BLACK_C);
// 	}
// 	else if (m_pcb_info.nBin == BD_DATA_REJECT) 
// 	{	
// 		strTemp.Format(_T("REJECT"));
// 		m_pGridPcb.SetItemBkColour(7, 3, RED_L, BLACK_C);
// 	}
// 	else if (m_pcb_info.nBin == BD_DATA_ABORT) 
// 	{	
// 		strTemp.Format(_T("ABORT"));
// 		m_pGridPcb.SetItemBkColour(7, 3, ORANGE_C, BLACK_C);
// 	}
// 	else 
// 	{
// 		strTemp.Format(_T("No Data"));
// 		m_pGridPcb.SetItemBkColour(7, 3, WHITE_C, BLACK_C);
// 	}
// 
// 	m_pGridPcb.SetItemFont(7, 3, &clsFunc.OnLogFont(14));
// 	m_pGridPcb.SetItemText(7, 3, strTemp);
// 
// 	if (m_pcb_info.nEnable == YES)
// 	{
// 		strTemp.Format(_T("YES"));
// 		m_pGridPcb.SetItemBkColour(8, 1, GREEN_L, BLACK_C);
// 	}
// 	else if (m_pcb_info.nEnable == NO)
// 	{
// 		strTemp.Format(_T("NO"));
// 		m_pGridPcb.SetItemBkColour(8, 1, RED_L, BLACK_C);
// 	}
// 
// 	m_pGridPcb.SetItemFont(8, 1, &clsFunc.OnLogFont(14));
// 	m_pGridPcb.SetItemText(8, 1, strTemp);
// 
// 	if (m_pcb_info.nYesNo == YES)
// 	{
// 		strTemp.Format(_T("YES"));
// 		m_pGridPcb.SetItemBkColour(8, 3, GREEN_L, BLACK_C);
// 	}
// 	else if (m_pcb_info.nYesNo == NO)
// 	{
// 		strTemp.Format(_T("NO"));
// 		m_pGridPcb.SetItemBkColour(8, 3, RED_L, BLACK_C);
// 	}
// 
// 	m_pGridPcb.SetItemFont(8, 3, &clsFunc.OnLogFont(14));
// 	m_pGridPcb.SetItemText(8, 3, strTemp);

	m_pGridPcb.Invalidate(FALSE);
}

void CDialogPcb::OnPcbCellClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CDialog_KeyBoard	dlgKeyBoard;
	CDialog_KeyPad		dlgKeyPad;
	CString				strTemp;
	int nRow, nCol;
	
	NM_GRIDVIEW* pItem	= (NM_GRIDVIEW*) pNotifyStruct;

	nRow				= pItem->iRow;
	nCol				= pItem->iColumn;

	//kwlee 2016.1216
	return;

	dlgKeyPad.m_nKeypadMode = 0;
	dlgKeyPad.m_strKeypadLowLimit	=	_T("0");
	
	if (nRow == 0 || nCol == 0 || nCol == 2) return;

	if (nCol == 1)
	{
		switch(nRow)
		{
			case 1:
				dlgKeyBoard.m_strKeyBoardVal = m_pcb_info.strLotNo;
				
				if (dlgKeyBoard.DoModal() == IDOK)
				{
					m_pcb_info.strLotNo = dlgKeyBoard.m_strKeyBoardVal;
				}
				break;

			case 2:
				dlgKeyBoard.m_strKeyBoardVal = m_pcb_info.strPartNo;

				if (dlgKeyBoard.DoModal() == IDOK)
				{
					m_pcb_info.strPartNo = dlgKeyBoard.m_strKeyBoardVal;
				}
				break;
				
			case 3:
				dlgKeyBoard.m_strKeyBoardVal = m_pcb_info.strBarcode1D[0];

				if (dlgKeyBoard.DoModal() == IDOK)
				{
					m_pcb_info.strBarcode1D[0] = dlgKeyBoard.m_strKeyBoardVal;
				}
				break;

			case 4:
				dlgKeyBoard.m_strKeyBoardVal = m_pcb_info.strBarcode1D[1];

				if (dlgKeyBoard.DoModal() == IDOK)
				{
					m_pcb_info.strBarcode1D[1] = dlgKeyBoard.m_strKeyBoardVal;
				}
				break;

			case 5:
//				dlgKeyBoard.m_strKeyBoardVal = m_pcb_info.strBarcode2D;

				if (dlgKeyBoard.DoModal() == IDOK)
				{
//					m_pcb_info.strBarcode2D = dlgKeyBoard.m_strKeyBoardVal;
				}
				break;
// 
// 			case 6:
// 				dlgKeyBoard.m_strKeyBoardVal = m_pcb_info.strPPID;
// 
// 				if (dlgKeyBoard.DoModal() == IDOK)
// 				{
// 					m_pcb_info.strPPID = dlgKeyBoard.m_strKeyBoardVal;
// 				}
// 				break;
// 
// 			case 7:
// 				dlgKeyBoard.m_strKeyBoardVal = m_pcb_info.strWWN;
// 
// 				if (dlgKeyBoard.DoModal() == IDOK)
// 				{
// 					m_pcb_info.strWWN = dlgKeyBoard.m_strKeyBoardVal;
// 				}
// 				break;

			case 8:
/*
				if (m_pcb_info.nEnable == YES)
				{
					m_pcb_info.nEnable = NO;
				}
				else if (m_pcb_info.nEnable == NO)
				{
					m_pcb_info.nEnable = YES;
				}
*/
				break;
		}
	}

	else if (nCol == 3)
	{
		switch(nRow)
		{
			case 1:
				if (m_pcb_info.nRetest == 1)
				{
					m_pcb_info.nRetest = 0;
				}
				else if (m_pcb_info.nRetest == 0)
				{
					m_pcb_info.nRetest = 1;
				}
				break;

			case 2:
				dlgKeyPad.m_strKeypadHighLimit.Format(_T("%d"), st_recipe_info.nTestRetest_Count);
				dlgKeyPad.m_strKeypadVal.Format(_T("%d"), m_pcb_info.nRetestCnt);

				if (dlgKeyPad.DoModal() == IDOK)
				{
					m_pcb_info.nRetestCnt = _wtoi(dlgKeyPad.m_strNewVal);
				}
				break;

			case 3:
				break;

			case 4:
				break;

			case 5:
/*
				dlgKeyPad.m_strKeypadHighLimit	=	_T("8");
				dlgKeyPad.m_strKeypadVal.Format(_T("%d"), m_pcb_info.nCOK_IDNum);

				if (dlgKeyPad.DoModal() == IDOK)
				{
					m_pcb_info.nCOK_IDNum = _wtoi(dlgKeyPad.m_strNewVal);
				}
				*/		
				if (m_pcb_info.nYesNo == YES)
				{
					m_pcb_info.nYesNo = NO;
				}
				else if (m_pcb_info.nYesNo == NO)
				{
					m_pcb_info.nYesNo = YES;
				}
				break;

// 			case 6:
// 				dlgKeyPad.m_strKeypadHighLimit	=	_T("48");
// 				dlgKeyPad.m_strKeypadVal.Format(_T("%d"), m_pcb_info.nBdNum);
// 
// 				if (dlgKeyPad.DoModal() == IDOK)
// 				{
// 					m_pcb_info.nBdNum = _wtoi(dlgKeyPad.m_strNewVal);
// 				}
// 				break;
// 
// 			case 7:
// 				if (m_pcb_info.nBin == BD_DATA_GOOD || m_pcb_info.nBin == BD_DATA_REJECT || m_pcb_info.nBin == BD_DATA_ABORT)
// 				{
// 					if (m_pcb_info.nBin == BD_DATA_GOOD) 
// 					{
// 						m_pcb_info.nBin = BD_DATA_REJECT;
// 					}
// 					else if (m_pcb_info.nBin == BD_DATA_REJECT) 
// 					{	
// 						m_pcb_info.nBin = BD_DATA_ABORT;
// 					}
// 					else if (m_pcb_info.nBin == BD_DATA_ABORT) 
// 					{	
// 						m_pcb_info.nBin = BD_DATA_GOOD;
// 					}
// 				}
// 				break;
// 
// 			case 8:
// 				if (m_pcb_info.nYesNo == YES)
// 				{
// 					m_pcb_info.nYesNo = NO;
// 				}
// 				else if (m_pcb_info.nYesNo == NO)
// 				{
// 					m_pcb_info.nYesNo = YES;
// 				}
// 				break;
		}
	}
	OnPcbDataDisplay();
}

void CDialogPcb::OnBnClickedCancel()
{
	CDialog_Message dlgMsg;
	BOOL bRet = FALSE;
	//kwlee 2016.1216
	return;
	if (m_pcb_info.strLotNo			!= m_pPcb_info->strLotNo) bRet = TRUE;
	if (m_pcb_info.strPartNo		!= m_pPcb_info->strPartNo) bRet = TRUE;
	if (m_pcb_info.strBarcode1D[0]	!= m_pPcb_info->strBarcode1D[0]) bRet = TRUE;
	if (m_pcb_info.strBarcode1D[1]	!= m_pPcb_info->strBarcode1D[1]) bRet = TRUE;
	if (m_pcb_info.strBarcode2D		!= m_pPcb_info->strBarcode2D) bRet = TRUE;
	if (m_pcb_info.nRetest			!= m_pPcb_info->nRetest) bRet = TRUE;
	if (m_pcb_info.nRetestCnt		!= m_pPcb_info->nRetestCnt) bRet = TRUE;
	if (m_pcb_info.nYesNo			!= m_pPcb_info->nYesNo) bRet = TRUE;

	if (bRet == TRUE)
	{
		dlgMsg.m_nMessageType = 1;
		dlgMsg.m_strMessage.Format(_T("Do you want to change the existing value to the current value?"));

		if (dlgMsg.DoModal() == IDOK)
		{
			m_pPcb_info->strLotNo			=	m_pcb_info.strLotNo;		
			m_pPcb_info->strPartNo			=	m_pcb_info.strPartNo;	
			m_pPcb_info->strBarcode1D[0]	=	m_pcb_info.strBarcode1D[0];	
			m_pPcb_info->strBarcode1D[1]	=	m_pcb_info.strBarcode1D[1];
			// jtkim 20160929
			m_pPcb_info->strBarcode2D[0]	=	m_pcb_info.strBarcode2D[0];
			m_pPcb_info->nRetest			=	m_pcb_info.nRetest;		
			m_pPcb_info->nRetestCnt			=	m_pcb_info.nRetestCnt;	
			m_pPcb_info->nYesNo				=	m_pcb_info.nYesNo;	
		}
	}

	if (st_handler_info.cWndMain != NULL)
	{
		st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_PCB_DATA_DELETE_REQ);
	}
	/*CDialog::OnCancel();*/
}


void CDialogPcb::OnBnClickedBtnPcbDialogStart()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_Message  dlgMsg;
	int nResponse;

	//kwlee 2016.1216
	return;
	dlgMsg.m_nMessageType	= 1;
	dlgMsg.m_strMessage.Format(_T("[%02d] Do you want a slot test start?"), m_nX);

	nResponse = (int)dlgMsg.DoModal();

	if(nResponse == IDOK)
	{
		m_pcb_info.nTestBdStart = 1;
	}
}


void CDialogPcb::OnBnClickedBtnPcbDialogAbort()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_Message  dlgMsg;
	int nResponse;

	CString strMsg;
	//kwlee 2016.1216
	return;
	dlgMsg.m_nMessageType	= 1;
	dlgMsg.m_strMessage.Format(_T("[%02d] Do you want a slot abort?"), m_nX);

	nResponse = (int)dlgMsg.DoModal();

	if(nResponse == IDOK)
	{
		m_pcb_info.nBin = BD_DATA_ABORT;

		if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
		{
			strMsg.Format(_T("[%02d] Slot Abort"), m_nX);
			clsMem.OnNormalMessageWrite(strMsg);
			st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
		}
	}
}


void CDialogPcb::OnBnClickedBtnPcbDialogPass()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_Message  dlgMsg;
	int nResponse;
	//kwlee 2016.1216
	return;

	dlgMsg.m_nMessageType	= 1;
	dlgMsg.m_strMessage.Format(_T("[%02d] Do you want a slot pass?"), m_nX);

	nResponse = (int)dlgMsg.DoModal();

	if(nResponse == IDOK)
	{
		m_pcb_info.nBin = BD_DATA_GOOD;
	}
}


void CDialogPcb::OnBnClickedBtnPcbDialogFail()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog_Message  dlgMsg;
	int nResponse;

	//kwlee 2016.1216
	return;

	dlgMsg.m_nMessageType	= 1;
	dlgMsg.m_strMessage.Format(_T("[%02d] Do you want a slot fail?"), m_nX);

	nResponse = (int)dlgMsg.DoModal();

	if(nResponse == IDOK)
	{
		m_pcb_info.nBin = BD_DATA_REJECT;
	}
}

void CDialogPcb::OnInitButton()
{
	m_btnStart.SetBitmaps(IDC_BTN_PCB_DIALOG_START, IDB_BITMAP_GEAR_UP, WINDOW_DN, IDB_BITMAP_GEAR_UP, WINDOW_UP);
	m_btnStart.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnStart.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnStart.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnStart.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnStart.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnStart.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnStart.SetFont(clsFunc.m_pFont[3]);

	m_btnAbort.SetBitmaps(IDC_BTN_PCB_DIALOG_ABORT, IDB_BITMAP_GEAR_UP, WINDOW_DN, IDB_BITMAP_GEAR_UP, WINDOW_UP);
	m_btnAbort.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnAbort.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnAbort.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnAbort.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnAbort.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnAbort.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnAbort.SetFont(clsFunc.m_pFont[3]);

	m_btnPass.SetBitmaps(IDC_BTN_PCB_DIALOG_PASS, IDB_BITMAP_GEAR_UP, WINDOW_DN, IDB_BITMAP_GEAR_UP, WINDOW_UP);
	m_btnPass.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnPass.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnPass.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnPass.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnPass.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnPass.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnPass.SetFont(clsFunc.m_pFont[3]);

	m_btnFail.SetBitmaps(IDC_BTN_PCB_DIALOG_START, IDB_BITMAP_GEAR_UP, WINDOW_DN, IDB_BITMAP_GEAR_UP, WINDOW_UP);
	m_btnFail.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnFail.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnFail.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnFail.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnFail.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnFail.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnFail.SetFont(clsFunc.m_pFont[3]);

	m_btnApply.SetBitmaps(IDC_BTN_PCB_DIALOG_APPLY, IDB_BITMAP_APPLY, WINDOW_DN, IDB_BITMAP_APPLY, WINDOW_UP);
	m_btnApply.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnApply.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnApply.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnApply.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnApply.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnApply.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnApply.SetFont(clsFunc.m_pFont[3]);

	m_btnCancel.SetBitmaps(IDC_BTN_PCB_DIALOG_CANCEL, IDB_BITMAP_CLEAR_DN, WINDOW_DN, IDB_BITMAP_CLEAR_UP, WINDOW_UP);
	m_btnCancel.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnCancel.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnCancel.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnCancel.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnCancel.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnCancel.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnCancel.SetFont(clsFunc.m_pFont[3]);

	// jtkim 20150316
// 	m_btnStart.ShowWindow(FALSE);
// 	m_btnAbort.ShowWindow(FALSE);
// 	m_btnPass.ShowWindow(FALSE);
// 	m_btnFail.ShowWindow(FALSE);
	m_btnStart.ShowWindow(TRUE);
	m_btnAbort.ShowWindow(TRUE);
	m_btnPass.ShowWindow(TRUE);
	m_btnFail.ShowWindow(TRUE);

	if (m_nMode == 0 || m_nMode == 1 || m_nMode == 2 || m_nMode == 3 ||
		m_nMode == 4 || m_nMode == 5 || m_nMode == 6 || m_nMode == 7 )
	{
		if (m_pcb_info.nEnable == YES)
		{
			if (m_pcb_info.nYesNo == YES)
			{
				if (m_pcb_info.nBin == BD_START_OK)
				{
					m_btnAbort.ShowWindow(TRUE);
				}
				else if(m_pcb_info.nBin == BD_READY_CHK)
				{
					m_btnAbort.ShowWindow(TRUE);
				}
				else if(m_pcb_info.nBin == BD_READY_OK)
				{
					m_btnAbort.ShowWindow(TRUE);
				}
				else if(m_pcb_info.nBin == BD_START_CHK)
				{
					m_btnAbort.ShowWindow(TRUE);
				}
				else if(m_pcb_info.nBin == BD_BIN_CHK)
				{
					m_btnAbort.ShowWindow(TRUE);
				}
				else if (m_pcb_info.nBin == BD_DATA_GOOD)
				{
					m_btnPass.ShowWindow(TRUE);
					m_btnFail.ShowWindow(TRUE);
				}
				else if (m_pcb_info.nBin == BD_DATA_REJECT)
				{
					m_btnPass.ShowWindow(TRUE);
					m_btnFail.ShowWindow(TRUE);
				}
				else if (m_pcb_info.nBin == BD_DATA_ABORT)
				{
					m_btnPass.ShowWindow(TRUE);
					m_btnFail.ShowWindow(TRUE);
				}
				else
				{
					m_btnStart.ShowWindow(TRUE);
				}
			}
		}
	}
// 	else if (m_nMode == 7 || m_nMode == 8)
// 	{
// 		m_btnPass.ShowWindow(TRUE);
// 		m_btnFail.ShowWindow(TRUE);
// 	}
	else
	{
		
		m_btnStart.ShowWindow(FALSE);
		m_btnAbort.ShowWindow(FALSE);
		m_btnPass.ShowWindow(FALSE);
		m_btnFail.ShowWindow(FALSE);
	}
}

void CDialogPcb::OnInitGroupBox()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_groupCommand.SetFont(clsFunc.OnLogFont(16));
	m_groupCommand.SetCatptionTextColor(BLUE_C);
	m_groupCommand.SetBorderColor(ORANGE_C);
	m_groupCommand.SetFontBold(TRUE);
	m_groupCommand.SetBackgroundColor(WINDOW_UP);
}

void CDialogPcb::OnInitLabel()
{
	CString strMsg;

	m_msgMsg.ShowWindow(SW_SHOW);
	m_msgMsg.SetFont(clsFunc.m_pFont[1]);

	switch (m_nMode)
	{
		case 0:
			strMsg.Format(_T("TestSite1: [%d]"), m_nY);
			break;
			 
		case 1:
			strMsg.Format(_T("TestSite2 : [%d]"), m_nY);
			break;

		case 2:
			strMsg.Format(_T("Test Site3 : [%d]"), m_nY);
			break;

		case 3:
			strMsg.Format(_T("TestSite4 : [%d]"), m_nY);
			break;

		case 4:
			strMsg.Format(_T("TestSite5 : [%d]"), m_nY);
			break;

		case 5:
			strMsg.Format(_T("TestSite6 : [%d]"), m_nY);
			break;

		case 6:
			strMsg.Format(_T("TestSite7 : [%d]"), m_nY);
			break;

		case 7:
			strMsg.Format(_T("TestSite8 : [%d]"), m_nY);
			break;

// 		case 8:
// 			strMsg.Format(_T("UnLoader Picker X : [%d] Y : [%d]"), m_nX, m_nY);
// 			break;
// 
// 		case 9:
// 			strMsg.Format(_T("UnLoader Tray X : [%d] Y : [%d]"), m_nX, m_nY);
// 			break;
// 
// 		case 10:
// 			strMsg.Format(_T("UnLoader Tray X : [%d] Y : [%d]"), m_nX, m_nY);
// 			break;
// 
// 		case 11:
// 			strMsg.Format(_T("Retest Tray X : [%d] Y : [%d]"), m_nX, m_nY);
// 			break;
	}
	
	m_msgMsg.SetWindowText(strMsg);
	m_msgMsg.SetCenterText();
	m_msgMsg.SetColor(BLUE_L);
	m_msgMsg.SetGradientColor(WHITE_C);
	m_msgMsg.SetTextColor(BLACK_C);
}

void CDialogPcb::OnBnClickedBtnPcbDialogApply()
{
	//kwlee 2017.0126
	return;
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnOK();
}


void CDialogPcb::OnBnClickedBtnPcbDialogCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnCancel();
}


BOOL CDialogPcb::OnEraseBkgnd(CDC* pDC)
{
	CDC  memDC;                     // 메모리 DC
	CBitmap * pOldBitmap = NULL;    // m_BackBitmap으로 새로운 그림을 DC에 그릴 때, 이전 그려졌던 DC(즉, Bitmap)을 저장.
	BITMAP bitmapInfo;              // 그림의 정보(m_BackBitmap)
	st_handler_info.m_BackDlgBitmap.GetBitmap(&bitmapInfo); // Bitmap 크기 구함.

	memDC.CreateCompatibleDC(pDC);  // 메모리 DC 생성
	pOldBitmap = memDC.SelectObject(&st_handler_info.m_BackDlgBitmap);
	pDC->BitBlt(0, 0, bitmapInfo.bmWidth, bitmapInfo.bmHeight, &memDC, 0, 0, SRCCOPY);   // 메모리 DC에 그림을 그림

	memDC.SelectObject(pOldBitmap);
	memDC.DeleteDC();
	return true; // 흰 부러쉬가 아닌 배경을 그려야 하기 때문에 true
}
