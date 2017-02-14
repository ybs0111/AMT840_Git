// DialogXgem.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "DialogXgem.h"
#include "afxdialogex.h"
#include "XgemClient.h"
#include "Variable.h"
#include "ClientEcFirst.h"
#include "ClientInterface1.h"
#include "ClientInterface5.h"

// CDialogXgem 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialogXgem, CDialog)

CDialogXgem::CDialogXgem(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogXgem::IDD, pParent)
{

	m_strAlarm = _T("");
	m_strLotNo = _T("");
	m_strPartNo = _T("");
	m_strLotNoEc = _T("");
	m_strScrapCodeEc = _T("");
	m_strSerialEc = _T("");
	m_strPartNoEc = _T("");
}

CDialogXgem::~CDialogXgem()
{
}

void CDialogXgem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ALARM, m_strAlarm);
	DDX_Text(pDX, IDC_EDIT_LOT_NO, m_strLotNo);
	DDX_Text(pDX, IDC_EDIT_PART_NO, m_strPartNo);
	DDX_Text(pDX, IDC_EDIT_LOT_NO_EC, m_strLotNoEc);
	DDX_Text(pDX, IDC_EDIT_SCRAP_CODE, m_strScrapCodeEc);
	DDX_Text(pDX, IDC_EDIT_SERIAL, m_strSerialEc);
	DDX_Text(pDX, IDC_EDIT_PART_NO_EC, m_strPartNoEc);
}


BEGIN_MESSAGE_MAP(CDialogXgem, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_START, &CDialogXgem::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_END, &CDialogXgem::OnBnClickedButtonEnd)
	ON_BN_CLICKED(IDC_BUTTON_ALARM_ON, &CDialogXgem::OnBnClickedButtonAlarmOn)
	ON_BN_CLICKED(IDC_BUTTON_ALARM_OFF, &CDialogXgem::OnBnClickedButtonAlarmOff)
	ON_BN_CLICKED(IDC_BUTTON_RECIPE, &CDialogXgem::OnBnClickedButtonRecipe)
	ON_BN_CLICKED(IDC_BUTTON_BOARD, &CDialogXgem::OnBnClickedButtonBoard)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CDialogXgem::OnBnClickedButtonExit)
	ON_BN_CLICKED(IDC_BUTTON_START_EC, &CDialogXgem::OnBnClickedButtonStartEc)
	ON_BN_CLICKED(IDC_BUTTON_END_EC, &CDialogXgem::OnBnClickedButtonEndEc)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL_EC, &CDialogXgem::OnBnClickedButtonCancelEc)
	ON_BN_CLICKED(IDC_BUTTON_SCRAP_INFO_EC, &CDialogXgem::OnBnClickedButtonScrapInfoEc)
	ON_BN_CLICKED(IDC_BUTTON_ONLINE_EC, &CDialogXgem::OnBnClickedButtonOnlineEc)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_SCRAP_EC, &CDialogXgem::OnBnClickedButtonDeleteScrapEc)
	ON_BN_CLICKED(IDC_BUTTON_READY_INTER, &CDialogXgem::OnBnClickedButtonReadyInter)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_INTER, &CDialogXgem::OnBnClickedButtonLoadInter)
	ON_BN_CLICKED(IDC_BUTTON_MODULE_INTER, &CDialogXgem::OnBnClickedButtonModuleInter)
	ON_BN_CLICKED(IDC_BUTTON_START_INTER, &CDialogXgem::OnBnClickedButtonStartInter)
	ON_BN_CLICKED(IDC_BUTTON_TOTAL_INTER, &CDialogXgem::OnBnClickedButtonTotalInter)
	ON_BN_CLICKED(IDC_BUTTON_ABORT_INTER, &CDialogXgem::OnBnClickedButtonAbortInter)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL_EC2, &CDialogXgem::OnBnClickedButtonCancelEc2)
END_MESSAGE_MAP()


// CDialogXgem 메시지 처리기입니다.


BOOL CDialogXgem::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ¿©±â¿¡ Ãß°¡ ÃÊ±âÈ­ ÀÛ¾÷À» Ãß°¡ÇÕ´Ï´Ù.

	return TRUE;  // return TRUE unless you set the focus to a control
	// ¿¹¿Ü: OCX ¼Ó¼º ÆäÀÌÁö´Â FALSE¸¦ ¹ÝÈ¯ÇØ¾ß ÇÕ´Ï´Ù.
}


void CDialogXgem::OnBnClickedButtonStart()
{
	// TODO: ¿©±â¿¡ ÄÁÆ®·Ñ ¾Ë¸² Ã³¸®±â ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.
	UpdateData(TRUE);

	if (m_strLotNo == _T("") || m_strPartNo == _T("")) return;

	clsXgem.OnMcLotStart(m_strLotNo, m_strPartNo);
}


void CDialogXgem::OnBnClickedButtonEnd()
{
	// TODO: ¿©±â¿¡ ÄÁÆ®·Ñ ¾Ë¸² Ã³¸®±â ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.
	COleDateTimeSpan time;
	UpdateData(TRUE);

	if (m_strLotNo == _T("") || m_strPartNo == _T("")) return;

	clsXgem.OnMcLotEnd(m_strLotNo, m_strPartNo, 100, 90, 10, time);
}


void CDialogXgem::OnBnClickedButtonAlarmOn()
{
	// TODO: ¿©±â¿¡ ÄÁÆ®·Ñ ¾Ë¸² Ã³¸®±â ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.
	UpdateData(TRUE);

	if (m_strAlarm == _T("")) return;

	clsXgem.OnMcAlarm(m_strAlarm, 1);
}


void CDialogXgem::OnBnClickedButtonAlarmOff()
{
	// TODO: ¿©±â¿¡ ÄÁÆ®·Ñ ¾Ë¸² Ã³¸®±â ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.
	UpdateData(TRUE);

	if (m_strAlarm == _T("")) return;

	clsXgem.OnMcAlarm(m_strAlarm, 1);
}


void CDialogXgem::OnBnClickedButtonRecipe()
{
	// TODO: ¿©±â¿¡ ÄÁÆ®·Ñ ¾Ë¸² Ã³¸®±â ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.
	clsXgem.OnMcRecipeCreate(0, st_basic_info.strDeviceName, st_path_info.strPathRecipe);
}


void CDialogXgem::OnBnClickedButtonBoard()
{
	// TODO: ¿©±â¿¡ ÄÁÆ®·Ñ ¾Ë¸² Ã³¸®±â ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.
	CString strTemp;

	UpdateData(TRUE);

	if (m_strLotNo == _T("") || m_strPartNo == _T("")) return;

	strTemp.Format(_T("TRACE_DATA_%s.TXT"), m_strLotNo);

	clsXgem.OnMcRecipeCreate(1, strTemp, st_path_info.strPathRecipe);
}


void CDialogXgem::OnBnClickedButtonExit()
{
	// TODO: ¿©±â¿¡ ÄÁÆ®·Ñ ¾Ë¸² Ã³¸®±â ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.
	CDialog::OnOK();
}


void CDialogXgem::OnBnClickedButtonStartEc()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if (m_strLotNoEc == _T("")) return;

	clsEcFirst.OnEcLotStart(m_strLotNoEc, _T(""), 10);
}


void CDialogXgem::OnBnClickedButtonEndEc()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if (m_strLotNoEc == _T("")) return;

	clsEcFirst.OnEcLotEnd(m_strLotNoEc, _T(""), _T(""), 10, 0);
}


void CDialogXgem::OnBnClickedButtonCancelEc()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if (m_strLotNoEc == _T("")) return;

	clsEcFirst.OnEcLotCancel(m_strLotNoEc, _T(""));
}


void CDialogXgem::OnBnClickedButtonScrapInfoEc()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if (m_strLotNoEc == _T("")) return;

	clsEcFirst.OnEcScrapInfo(m_strLotNoEc);
}


void CDialogXgem::OnBnClickedButtonOnlineEc()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if (m_strLotNoEc		== _T("")) return;
	if (m_strScrapCodeEc	== _T("")) return;
	if (m_strSerialEc		== _T("")) return;

	st_lot_display_info[0].strLotNo	= m_strLotNoEc;
	st_lot_display_info[0].nScrapCnt	= 1;

	st_lot_display_info[0].strScrapSerial[0]	= m_strSerialEc;
	st_lot_display_info[0].strScrapCode[0]		= m_strScrapCodeEc;
	
	clsEcFirst.OnEcOnlineEnd(_T(""), st_lot_display_info[0]);
}


void CDialogXgem::OnBnClickedButtonDeleteScrapEc()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if (m_strLotNoEc		== _T("")) return;
	if (m_strSerialEc		== _T("")) return;

	clsEcFirst.OnEcDeleteScrap(m_strLotNoEc, m_strSerialEc);
}


void CDialogXgem::OnBnClickedButtonReadyInter()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if (m_strLotNoEc		== _T("")) return;
	if (m_strPartNoEc		== _T("")) return;

	tagTEST_SITE_INFO inter;

	inter.strEqpID	= _T("ICBT01");
	inter.strLotNo	= m_strLotNoEc;
	inter.strPartNo	= m_strPartNoEc;
	inter.nTestMode = 0;
	inter.nPgmDown	= 0;
	inter.nQty		= 100;
	inter.nRetry	= 3;

	clsInterC1.OnTesterReady(inter);
}


void CDialogXgem::OnBnClickedButtonLoadInter()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if (m_strLotNoEc		== _T("")) return;
	if (m_strPartNoEc		== _T("")) return;

	tagTEST_SITE_INFO inter;

	inter.strEqpID		= _T("ICBT01");
	inter.strLotNo		= m_strLotNoEc;
	inter.strPartNo		= m_strPartNoEc;
	inter.nTestMode		= 0;
	inter.nPgmDown		= 0;
	inter.nQty			= 100;
	inter.nRetry		= 3;
	inter.nModuleCount	= 1;
	inter.st_pcb_info[0].strBarcode1D[0] = _T("11S00JA668YH10MS49114D");

	clsInterC1.OnPgmDownload(inter);
}


void CDialogXgem::OnBnClickedButtonModuleInter()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if (m_strLotNoEc		== _T("")) return;
	if (m_strPartNoEc		== _T("")) return;

	tagTEST_SITE_INFO inter;

	inter.strEqpID		= _T("ICBT01");
	inter.strLotNo		= m_strLotNoEc;
	inter.strPartNo		= m_strPartNoEc;
	inter.nTestMode		= 0;
	inter.nPgmDown		= 0;
	inter.nQty			= 100;
	inter.nRetry		= 3;
	inter.nModuleCount	= 1;
	inter.st_pcb_info[0].strBarcode1D[0] = _T("11S00JA668YH10MS49114D");

	clsInterC1.OnModuleInfo(inter);
}


void CDialogXgem::OnBnClickedButtonStartInter()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if (m_strLotNoEc		== _T("")) return;
	if (m_strPartNoEc		== _T("")) return;

	tagTEST_SITE_INFO inter;

	inter.strEqpID		= _T("ICBT01");
	inter.strLotNo		= m_strLotNoEc;
	inter.strPartNo		= m_strPartNoEc;
	inter.nTestMode		= 0;
	inter.nPgmDown		= 0;
	inter.nQty			= 100;
	inter.nRetry		= 3;
	inter.nModuleCount	= 1;
	inter.st_pcb_info[0].strBarcode1D[0] = _T("11S00JA668YH10MS49114D");
	clsInterC1.OnTesterStart(inter);
}


void CDialogXgem::OnBnClickedButtonTotalInter()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if (m_strLotNoEc		== _T("")) return;
	if (m_strPartNoEc		== _T("")) return;

	tagTEST_SITE_INFO inter;

	inter.strEqpID		= _T("ICBT05");
	inter.strLotNo		= m_strLotNoEc;
	inter.strPartNo		= m_strPartNoEc;
	inter.nTestMode		= 0;
	inter.nPgmDown		= 0;
	inter.nQty			= 100;
	inter.nRetry		= 3;
	inter.nModuleCount	= 1;
	inter.st_pcb_info[0].strBarcode1D[0] = _T("11S00JA668YH10MS49114D");

	clsInterC5.OnLotComplete(inter);
}


void CDialogXgem::OnBnClickedButtonAbortInter()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if (m_strLotNoEc		== _T("")) return;
	if (m_strPartNoEc		== _T("")) return;

	tagTEST_SITE_INFO inter;

	inter.strEqpID		= _T("ICBT01");
	inter.strLotNo		= m_strLotNoEc;
	inter.strPartNo		= m_strPartNoEc;
	inter.nTestMode		= 0;
	inter.nPgmDown		= 0;
	inter.nQty			= 100;
	inter.nRetry		= 3;
	inter.nModuleCount	= 1;
	inter.st_pcb_info[0].strBarcode1D[0] = _T("11S00JA668YH10MS49114D");

	clsInterC1.OnTesterAbort(inter);
}


void CDialogXgem::OnBnClickedButtonCancelEc2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if (m_strLotNoEc		== _T("")) return;

	clsEcFirst.OnEcLotDisplay(m_strLotNoEc, _T(""));
}
