// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "Handler.h"
#include "MainFrm.h"
#include "Variable.h"
#include "ScreenMain.h"
#include "ScreenBasic.h"
#include "ScreenInitial.h"
#include "ScreenIO.h"
#include "ScreenLock.h"
#include "ScreenMotor.h"
#include "ScreenList.h"
#include "ScreenWork.h"
#include "ScreenAlarm.h"
#include "ScreenPrgInfo.h"
#include "Dialog_Exit.h"
#include "Ctlbd_Variable.h"
#include "FastechPublic_IO.h"
#include "Dialog_Pass_Check.h"
#include "PublicFunction.h"
#include "MyBasicData.h"
#include "MyJamData.h"
#include "Dialog_Message.h"
#include "Dialog_About.h"
#include "Dialog_Door_Open.h"
#include "ClientSocket.h"
#include "ServerSocket.h"
//#include "ClientGms.h"
#include "XgemClient.h"
#include "AlgMemory.h"
#include "ClientEcFirst.h"
#include "Interface.h"
#include "LogCtrl.h"
#include "ClientRfid.h"
#include "ServerVnR.h"
#include "ComizoaPublic.h"
#include "io.h"	
#include "ACriticalSection.h"
#include "ClientInterface1.h"
#include "ClientInterface2.h"
#include "ClientInterface3.h"
#include "ClientInterface4.h"
#include "ClientInterface5.h"
#include "ClientInterface6.h"
#include "ClientInterface7.h"
#include "ClientInterface8.h"
#include "ServerInterface1.h"
#include "ServerInterface2.h"
#include "ServerInterface3.h"
#include "ServerInterface4.h"
#include "ServerInterface5.h"
#include "ServerInterface6.h"
#include "ServerInterface7.h"
#include "ServerInterface8.h"
#include "Barcode_1D.h"
#include "Barcode_2D.h"
//kwlee 2016.0905
#include "RunTestSitePart1.h"
#include "RunTestSitePart2.h"
#include "RunTestSitePart3.h"
#include "RunTestSitePart4.h"
#include "RunTestSitePart5.h"
#include "RunTestSitePart6.h"
#include "RunTestSitePart7.h"
#include "RunTestSitePart8.h"
#include "AMTVClassWrapper.h" //kwlee 2016.0913 Vision 관련 임시 막아 놓음
#include "ClientEcSecond.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame
tagCLIENT_INFO				st_client_info[20];
tagSERVER_INFO				st_server_info[20];

//tagCLIENT_INFO				st_Testerclient_info[10];
tagSERVER_INFO				st_Testerserver_info[10];

tagHANDLER_INFO				st_handler_info;
tagSERIAL_INFO				st_serial_info;
tagALARM_INFO				st_alarm_info;
tagALARM_DISPLAY_INFO		st_alarm_display_info;
tagBASIC_INFO				st_basic_info;
tagLAMP_INFO				st_lamp_info;
tagIO_INFO					st_io_info;
tagPATH_INFO				st_path_info;
tagWORK_INFO				st_work_info;
tagOTHER_INFO				st_other_info;
tagWAIT_INFO				st_wait_info;
tagSYNC_INFO				st_sync_info = {0};		//구조체 0으로 초기화
tagRS232_INFO				st_rs232_info;
tagMOTOR_INFO				st_motor_info[MOT_MAXMOTOR];
tagRECIPE_INFO				st_recipe_info;
tagRECOVERY_INFO			st_recovery_info;
tagANIMATE_INFO				st_animate_info	= {0};	//구조체 0으로 초기화
tagCOUNT_INFO				st_count_info;
tagWORKER_INFO				st_worker_info;
tagGMS_INFO					st_gms_info;
tagLOT_HISTORY_INFO			st_lot_history_info;
tagPART_INFO				st_part_info;
tagTYPE_INFO				st_type_info;
st_teach_param              st_teach; //kwlee 2016.0913
/////////////////////////////////////////////////////////////////////////////////////////////////
//james 2016.0802
tagALL_TRAY_INFO			st_stacker_info[THREAD_MAX_SITE]; //모든 트레이가 이동하는 스태커 구조의 위치에는 사용한다
st_copy_data			*st_copydata;


/////////////////////////////////////////////////////////////////////////////////////////////////

/* 2015.0108
// loader picker 구조체
tagLD_PICKER_INFO			st_ld_picker_info[MAX_PICKER];
// loader buffer 구조체
// [0] load picker -> buffer [1] buffer -> test site robot
tagLD_BUF_INFO				st_ld_buf_info[2];
// unloader picker 구조체
tagUNLD_PICKER_INFO			st_unld_picker_info[MAX_PICKER];
// unloader buffer 구조체
// [0] unload picker <-> buffer [1] buffer <-> test site robot
tagUNLD_BUF_INFO			st_unld_buf_info[2];
// reject buffer 구조체
tagREJECT_BUFFER_INFO		st_reject_buf_info[2];
// retest buffer 구조체
tagRETEST_BUFFER_INFO		st_retest_buf_info;
// test site robot 구조체
tagTEST_SITE_RBT_INFO		st_test_site_rbt_info;


tagLD_TRAY_INFO				st_ld_tray_info[2];
// loader tray 구조체
tagUNLD_TRAY_INFO			st_unld_tray_info[2];

tagBOARD_INFO				st_bd_info[2][2];
*/
//2015.0102 
tagVARIABLE_INFO			st_var;
 

tagBOARD_INFO				st_bd_info[2][2];

// test site 구조체
//[0][0] left bottom [0][1] left top [1][0] right bottom [1][1] right top
tagTEST_SITE_INFO			st_test_site_info[THREAD_MAX_SITE]; //2015.0216 [2][2];

tagALL_TRAY_INFO			st_tray_info[THREAD_MAX_SITE];

tag_PICKER_INFO				st_picker[THREAD_MAX_SITE]; //2015.0107 

tag_BUFFER_INFO				st_buffer_info[THREAD_MAX_SITE]; //james 2016.0806

tagLOT_INFO					st_lot_info[3];		// 2015.0115
tagCOKBUFF_SITE_INFO		st_Cok_Buff_info[4]; //cok 4개 사이트 정보 //2015.0124

tagLOT_DISPLAY_INFO			st_lot_display_info[2]; // 2015.01.29 lot 전산처리 구조체
tagSAFETY_INFO				st_safety_info;

//tagTEST_SITE_SOCKET_FIND_INFO st_find_testsite_info[THREAD_MAX_SITE]; //0~47까지의 소켓을 12가지로 구간을 나눠 관리 
tagTEST_SITE_SOCKET_FIND_INFO st_find_testsite_info[THREAD_MAX_SITE][10];
tagTEST_SITE_SOCKET_FIND_INFO st_find_testsite_status[THREAD_MAX_SITE];//소켓의 상태를 확인할때 사용. 고유 소켓정보 작업 변경까지 유지  //0~47까지의 소켓을 12가지로 구간을 나눠 관리 

//james 2016.0807 tagREJECT_TRAY_INFO			st_reject_info[THREAD_MAX_SITE];
//james 2016.0807tagTEST_SITE_SOCKET_FIND_INFO st_test_socket_status[12]; //2015.0222 james 소켓의 상태를 확인할때 사용. 고유 소켓정보 작업 변경까지 유지  //0~47까지의 소켓을 12가지로 구간을 나눠 관리,
tagTRACK_INFO				st_track_info;
tagSCRAP_CODE				st_code_info[2];
//20151005
tagDB_time_param			st_DB_time;
tagdbtime_database			st_dbtime;
tagdbTimeTmp_database		*st_dbTimeTmp;
tagdbTimeTmp_database       *st_dbTimeDate;
tagBuffer_DB_database		st_BufferTemp;
tagPRODUCT_INFO				st_product_info;
//kwlee 2016.0615
OFFSET_INFO                 st_Offset_info;

#define TM_MAIN_REFRESH	100
#define TM_FILE_CREATE	200
#define TM_GMS			300
#define TM_XGEM			400

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_BASIC, &CMainFrame::OnBasic)
	ON_COMMAND(ID_ALARM, &CMainFrame::OnAlarm)
	ON_COMMAND(ID_DOOR, &CMainFrame::OnDoor)
	ON_COMMAND(ID_EXIT, &CMainFrame::OnExit)
	ON_COMMAND(ID_IO, &CMainFrame::OnIo)
	ON_COMMAND(ID_LEVEL, &CMainFrame::OnLevel)
	ON_COMMAND(ID_LIST, &CMainFrame::OnList)
	ON_COMMAND(ID_LOCK, &CMainFrame::OnLock)
	ON_COMMAND(ID_MAIN, &CMainFrame::OnMain)
	ON_COMMAND(ID_Menu, &CMainFrame::OnMenu)
	ON_COMMAND(ID_MOTOR, &CMainFrame::OnMotor)
	ON_COMMAND(ID_RESET, &CMainFrame::OnReset)
	ON_COMMAND(ID_SAVE, &CMainFrame::OnSave)
	ON_COMMAND(ID_SETTING, &CMainFrame::OnSetting)
	ON_COMMAND(ID_VER, &CMainFrame::OnVer)
	ON_MESSAGE(WM_FORM_CHANGE, OnViewChangeMode)					// Post Message에 대한 화면 전환 사용자 사용자 정의 메시지 선언 
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_COMMAND(ID_PGMINFO, &CMainFrame::OnPgminfo)
	ON_MESSAGE(WM_CLIENT_MSG_1, OnClientEcFirst)					// Network관련된 작업을 담당한다.
	ON_MESSAGE(WM_CLIENT_MSG_2, OnClientXgem)						// Network관련된 작업을 담당한다.
	ON_MESSAGE(WM_CLIENT_MSG_3, OnClientInterface1)						// Network관련된 작업을 담당한다.
	ON_MESSAGE(WM_CLIENT_MSG_4, OnClientInterface2)						// Network관련된 작업을 담당한다.
	ON_MESSAGE(WM_CLIENT_MSG_5, OnClientInterface3)						// Network관련된 작업을 담당한다.
	ON_MESSAGE(WM_CLIENT_MSG_6, OnClientInterface4)						// Network관련된 작업을 담당한다.
	ON_MESSAGE(WM_CLIENT_MSG_7, OnClientInterface5)						// Network관련된 작업을 담당한다.
	ON_MESSAGE(WM_CLIENT_MSG_8, OnClientInterface6)						// Network관련된 작업을 담당한다.
	ON_MESSAGE(WM_CLIENT_MSG_9, OnClientInterface7)						// Network관련된 작업을 담당한다.
	ON_MESSAGE(WM_CLIENT_MSG_10, OnClientInterface8)					// Network관련된 작업을 담당한다.
	ON_MESSAGE(WM_CLIENT_MSG_11,OnClientEcSecond )						// Network관련된 작업을 담당한다.
	ON_MESSAGE(WM_CLIENT_MSG_12,OnClientFtp)						// Network관련된 작업을 담당한다.
	ON_MESSAGE(WM_WORK_COMMAND, OnXgemCommand)						// Network관련된 작업을 담당한다. 
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_NCLBUTTONDOWN()
	ON_MESSAGE(WM_RFID_MSG,			OnRfidMessage)
	ON_MESSAGE(WM_BARCODE_MSG_1,OnBarcodeMessage1)
	ON_MESSAGE(WM_BARCODE_MSG_2,OnBarcodeMessage2)
	ON_MESSAGE(WM_BARCODE_MSG_3,OnBarcodeMessage3)
	ON_MESSAGE(WM_BARCODE_MSG_4,OnBarcodeMessage4)
	ON_MESSAGE(WM_BARCODE_MSG_5,OnBarcodeMessage5)
	ON_MESSAGE(WM_SERVER_MSG_3, OnServerInterface1)					// Network관련된 작업을 담당한다.
	ON_MESSAGE(WM_SERVER_MSG_4, OnServerInterface2)					// Network관련된 작업을 담당한다.
	ON_MESSAGE(WM_SERVER_MSG_5, OnServerInterface3)					// Network관련된 작업을 담당한다.
	ON_MESSAGE(WM_SERVER_MSG_6, OnServerInterface4)					// Network관련된 작업을 담당한다.
	ON_MESSAGE(WM_SERVER_MSG_7, OnServerInterface5)					// Network관련된 작업을 담당한다.
	ON_MESSAGE(WM_SERVER_MSG_8, OnServerInterface6)					// Network관련된 작업을 담당한다.
	ON_MESSAGE(WM_SERVER_MSG_9, OnServerInterface7)					// Network관련된 작업을 담당한다.
	ON_MESSAGE(WM_SERVER_MSG_10, OnServerInterface8)					// Network관련된 작업을 담당한다.
	ON_MESSAGE(WM_MAIN_EVENT, OnMainMessageEvent)	
	ON_WM_COPYDATA()
	ON_MESSAGE(WM_MAINFRAME_WORK, OnMainframe_Work) 
	ON_WM_COPYDATA()
END_MESSAGE_MAP()

// CMainFrame 생성/소멸


CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
	// File 경로 지정 함수.....
	int i,j;

	st_handler_info.nPgmVer = 1;
	m_nCount = 0;
	for (i=0; i<20; i++)
	{
		m_pClient[i] = NULL;
		m_pServer[i] = NULL;
	}

	st_dbTimeTmp = NULL;
	st_dbTimeDate = NULL;
	
	m_pNewActiveView = NULL;
	m_pOldActiveView = NULL;

	OnFilePath();	
	// Excel Map 정보 읽기....
	OnExcelRead();

	//kwlee 2016.1014
	OnConfigLoad();

	for (i=0; i<10000; i++)
	{
		//kwlee 2016.1018
		//st_work_info.nScrapRetest[i] = st_basic_info.nRetestCount;
		st_work_info.nScrapRetest[i] = st_recipe_info.nTestRetest_Count;
	}

	// jtkim 20161006 나중에 Retest Count 코드별 파일 읽어서 처리
// 	for (i=0; i<10000; i++)
// 	{
// 		st_work_info.nScrapRetest[i] = st_basic_info.nRetestCount;
// 	}
// 	OnConfigLoad();

	// 변수 및 HANDLE 초기화
	OnMainVarDefaultSet();
	st_work_info.nPgmInfo = 0;
	

	clsMyJamData.OnAlarmInfoLoad();	// 파일에 저장된 모든 알람 정보 전역 변수에 설정하는 함수

	// I/O Board Initialize
	if (FAS_IO.Initialize_Board() == RET_GOOD)
	{
		FAS_IO.Initialize_Map();

		FAS_IO.set_out_bit( st_io_info.o_LdUldMuteOn, IO_OFF);
		FAS_IO.set_out_bit( st_io_info.o_LdUldMuteOff, IO_ON);

		FAS_IO.set_out_bit( st_io_info.o_RejMuteOn, IO_OFF);
		FAS_IO.set_out_bit( st_io_info.o_RejMuteOff, IO_ON);

		//kwlee 2016.1215
		for (int i =0; i<4; i++)
		{
			FAS_IO.set_out_bit( st_io_info.o_LdBuffGripOnOffCyl[i], IO_OFF);
			FAS_IO.set_out_bit( st_io_info.o_UldBuffGripOnOffCyl[i], IO_OFF);
		}
		//
	}

	for (i=0; i<10000; i++)
	{
		for (j=0; j<2; j++)
		{
			st_code_info[0].m_nScrapCode[i][j] = st_recipe_info.nTestRetest_Count;
			st_code_info[1].m_nScrapCode[i][j] = st_recipe_info.nTestRetest_Count;
		}
	}
	st_lot_info[LOT_CURR].nTrayRunMode_StandAlone = YES;
	st_lot_info[LOT_NEXT].nTrayRunMode_StandAlone = YES;

	// jtkim 20150721
//	st_basic_info.nSocketCount = 100000;
	st_work_info.nCycleYesNo		= NO;
	st_work_info.nTsiteCycleYesNo	= YES;
	st_handler_info.nInitialSuccess = NO;
	st_handler_info.nAutoMode = YES;
	st_count_info.nHourSocket = st_count_info.nSocketStart;
	if (st_count_info.nHourSocket <= 0)
	{
		st_count_info.nHourSocket = 0;
		st_count_info.dHourPer = 0.0f;
	}
	else
	{
		st_count_info.dHourPer = ((double)st_count_info.nHourSocket / (double)48) * (double)100;
	}
	//kwlee 2016.0913 Vision 관련 임시 막아 놓음
		m_pAMTVClassWrapper = new AMTVClassWrapper();
	OpenDll();
	clsEcFirst.OnEqpID(st_basic_info.strEqp);
	clsEcSecond.OnEqpID(st_basic_info.strEqp);
	OnTestSiteDn(); //kwlee 20160905 Test Site 무조건 Down

//	st_basic_info.nModeTestInterface = EQP_ON_LINE;

	st_handler_info.mn_NoInput_test = CTL_NO;//2016.1228
}

CMainFrame::~CMainFrame()
{
	int i;

	for (i=0; i<20; i++)
	{
		if (m_pClient[i] != NULL)
		{
			delete m_pClient[i];
			m_pClient[i] = NULL;
		}


		if (m_pServer[i] != NULL)
		{
			delete m_pServer[i];
			m_pServer[i] = NULL;
		}
	}

	

	if(mp_msg_dlg != NULL)   delete mp_msg_dlg;

	if( m_pAMTVClassWrapper != NULL ) delete m_pAMTVClassWrapper;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	st_handler_info.hWnd = GetSafeHwnd();  // 메인 프레임 핸들 설정

	//메모리 DC에 사용할 Background Bitmap
	st_handler_info.m_TitleBackBitmap.LoadBitmap(/*IDB_BITMAP_BACK_GROUND1*/IDB_BITMAP_TITLE_BACKGROUND); 
	st_handler_info.m_BackBitmap.LoadBitmap(/*IDB_BITMAP_BACK_GROUND1*/IDB_BITMAP_BACK_GROUND);   
	st_handler_info.m_BackDlgBitmap.LoadBitmap(/*IDB_BITMAP_BACK_GROUND1*/IDB_BITMAP_DLG_BACK_GROUND);   

	for (int i =0; i<4; i++)
	{
		st_work_info.mn_Motor_Pos_X[i] = 0;
		st_work_info.mn_Motor_Pos_Y[i] = 0;
	}
	
	m_wndToolBar.SetBorders(1, 1, 1, 1);
	if (!m_wndToolBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	m_wndToolBar.ModifyStyle(0, TBSTYLE_FLAT);

	m_wndToolBar.LoadTrueColorToolBar(TOOLBAR_DRAW_BUTTON_WIDTH,
									  IDB_TOOLBAR_DRAW,
		                              IDB_TOOLBAR_DRAW_HOT,
								      IDB_TOOLBAR_DRAW_DISABLED);

	m_wndToolBar.EnableDocking(CBRS_ALIGN_TOP);
	EnableDocking(CBRS_ALIGN_TOP);

//	m_wndToolBar.AddDropDownButton(this, ID_PGMINFO, IDR_MENU_YASUNAGA);

	// **************************************************************************
    // 타이틀 정보 출력 Initial Dialog bar 생성
    // **************************************************************************
	if (!m_wndTitleBar.Create(this, IDD_DIALOG_TITLE, CBRS_ALIGN_TOP, IDD_DIALOG_TITLE))
	{
		TRACE0("Failed to create my init bar\n");
		return -1;      // fail to create
	}
	st_handler_info.cWndTitle = &m_wndTitleBar;
	//==============================================================//
	// 메시지 출력용 리스트바 생성
	// : 상태 바 밑에 위치하도록 하려면 이 부분을 상태 바 생성 부분 위로 올린다
	//==============================================================//
	//if (!m_wndListBar.Create(this, IDD_DIALOG_LIST, CBRS_ALIGN_BOTTOM, IDD_DIALOG_LIST))
	if (!m_wndListBar.Create(this, IDD_DIALOG_LIST,
		WS_CHILD | WS_VISIBLE | CBRS_ALIGN_BOTTOM | WS_BORDER | CBRS_GRIPPER, IDD_DIALOG_LIST))
	{
		TRACE0("Failed to create my list bar\n");
		return -1;      // fail to create
	}
	st_handler_info.cWndList = &m_wndListBar;
	// =============================================================================
    // 텍스트 툴바 스타일 설정 (드롭 다운 메뉴로 설정)                            //
    // =============================================================================
	m_wndToolBar.GetToolBarCtrl().SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);

	OnThreadCreate();
#ifdef _WIN64
//	OnVisionInitialize();
#endif
	/* ************************************************************************** */
    /* 초기화 진행 화면으로 전환하여 장비 초기화 작업 진행한다                    */
    /* ************************************************************************** */
	::PostMessage(st_handler_info.hWnd, WM_FORM_CHANGE, 1, 3);  // 초기화 진행 화면 전환
	/* ************************************************************************** */

	SendMessage(WM_SERVER_MSG_3, SERVER_CONNECT);	// test site 1 server open
	SendMessage(WM_SERVER_MSG_4, SERVER_CONNECT);	// test site 2 server open
	SendMessage(WM_SERVER_MSG_5, SERVER_CONNECT);	// test site 3 server open
	SendMessage(WM_SERVER_MSG_6, SERVER_CONNECT);	// test site 4 server open
	SendMessage(WM_SERVER_MSG_7, SERVER_CONNECT);	// test site 5 server open
	SendMessage(WM_SERVER_MSG_8, SERVER_CONNECT);	// test site 6 server open
	SendMessage(WM_SERVER_MSG_9, SERVER_CONNECT);	// test site 7 server open
	SendMessage(WM_SERVER_MSG_10, SERVER_CONNECT);	// test site 8 server open
	

	
	SetTimer(TM_MAIN_REFRESH, 500, NULL);  // 리스트 파일 생성 타이머
	SetTimer(TM_FILE_CREATE, 500, NULL);

	if (st_handler_info.nVerUpdate == 1)
	{
	}

	m_nXgemStep = 0;
	SetTimer(TM_XGEM, 5000, NULL); //kwlee 2017.0906

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

//	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
//		  | WS_MAXIMIZE | WS_SYSMENU;

//	cs.style =  WS_MAXIMIZE| WS_THICKFRAME ;
	// jtkim 20150901
	//cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE | WS_SYSMENU;
	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
	cs.style &= ~FWS_ADDTOTITLE;								// 화면 타이틀 정보 제어 가능하도록 스타일 설정 
	//cs.style &= ~WS_MAXIMIZEBOX;

//	cs.lpszClass=((CHANDLERApp*)AfxGetApp())->GetClassName();	// 현재 응용 프로그램에 대한 클래스 이름을 얻는다(Need for Single Instance)

	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기
LRESULT CMainFrame::OnViewChangeMode(WPARAM wParam,LPARAM lParam)
{
	if (wParam != 8)  
	{
		if (st_handler_info.nMenuLock	!= FALSE)  return 0;
		if (st_handler_info.nSystemLock	!= FALSE)  return 0;  // 현재 시스템 Lock 상태인 경우 리턴 
	}
	/* ************************************************************************** */

	switch(wParam)
	{
		case SCREEN_MAIN:
			OnMain();
			break;

		case SCREEN_INITIAL:
			OnMcInitial();
			break;

		case SCREEN_BASIC:
			OnBasic();
			break;

		case SCREEN_SETTING:
			OnSetting();
			break;

		case SCREEN_MOTOR:
			OnMotor();
			break;

		case SCREEN_IO:
			OnIo();
			break;

		case SCREEN_LIST:
			OnList();
			break;

		case SCREEN_ALARM:
			OnAlarm();
			break;

		case SCREEN_LOCK:
			OnLock();
			break;

		default:
			break;
	}

	return 0 ;
}

void CMainFrame::OnMcInitial()
{
	if ( OnMenuChangeChecking() == RET_ERROR ) return ;
	if ( GetActiveView()->IsKindOf(RUNTIME_CLASS(CScreenInitial)) )   return;

	// **************************************************************************
	// 메뉴 사용 불가능한 경우에 대한 조건 검사한다                              
	// -> 메인트 모드 또는 티칭 모드가 설정되어 있는지 검사                      
	// **************************************************************************

	if (st_handler_info.nLotStatus == LOT_START) return;

	if (st_handler_info.nInitialSuccess == YES)
	{
		if (st_handler_info.nLevelTeach !=  TRUE)
		{
			if (st_handler_info.nLevelMaint !=  TRUE) 
			{
				OnLevel();
				if (st_handler_info.nLevelTeach !=  TRUE)
				{
					if (st_handler_info.nLevelMaint !=  TRUE)  return;
				}
			}
		}
	}

	OnSwitchToForm(IDW_SCREEN_INIT);
}

void CMainFrame::OnBasic()
{
	if (st_handler_info.nMachineMode	!= MACHINE_MANUAL) return;
	if ( OnMenuChangeChecking() == RET_ERROR ) return ;
	if ( GetActiveView()->IsKindOf(RUNTIME_CLASS(CScreenBasic)) )   return;

	OnSwitchToForm(IDW_SCREEN_BASIC);
}


void CMainFrame::OnAlarm()
{
	/* ************************************************************************** */
    /* 화면 뷰 전환 불가능한 정보 검사한다.                                       */
    /* ************************************************************************** */
	if (st_handler_info.nMenuLock != FALSE)		return;
	if (st_handler_info.nSystemLock != FALSE)	return;  // 현재 시스템 Lock 상태인 경우 리턴 

	if (st_handler_info.nRunStatus == dRUN || st_handler_info.nRunStatus == dINIT) return;

	//if ( OnMenuChangeChecking() == RET_ERROR ) return ;
//	if ( OnMenuChangeChecking() == RET_ERROR ) return ;
	if ( GetActiveView()->IsKindOf(RUNTIME_CLASS(CScreenAlarm)) )   return;

	OnSwitchToForm(IDW_SCREEN_ALARM);
}


void CMainFrame::OnDoor()
{
	int nResponse;
	CDialog_Door_Open dlgDoor;

	if (st_handler_info.nMachineMode	!= MACHINE_MANUAL) return;

	if ( OnMenuChangeChecking() == RET_ERROR ) return ;

	nResponse = (int)dlgDoor.DoModal();
	if (nResponse == IDOK)
	{
	}
}

void CMainFrame::OnExit()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CDialog_Message dlgMsg;

	int nResponse;

	if (st_handler_info.nRunStatus != dSTOP)
	{
		dlgMsg.m_nMessageType	= 0;
		dlgMsg.m_strMessage		= _T("Now Machine is working.");
		
		dlgMsg.DoModal();
		return ;
	} 

	if (st_handler_info.nSystemLock) return;

	dlgMsg.m_nMessageType	= 1;
	dlgMsg.m_strMessage		= _T("AMT840 Exit the program.");

	nResponse = (int)dlgMsg.DoModal();
	if (nResponse == IDOK)
	{
		KillTimer(TM_MAIN_REFRESH);
		KillTimer(TM_FILE_CREATE);
		KillTimer(TM_GMS);
		KillTimer(TM_XGEM);

		// jtkim 20150405
		// //////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 현재 선택된 View가 NULL인 경우 마지막 View를 가져온다....
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (m_pNewActiveView == NULL)
		{
			m_pNewActiveView = GetActiveView();			// 현재 화면에 출력된 뷰 정보 설정 (save old view)
		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		m_pNewActiveView->DestroyWindow();
		m_pNewActiveView = NULL;
		/*clsReg.WriteCreateTime(st_handler_info.tCreate);
		clsReg.WriteMcTime(dRUN, st_handler_info.tRun);
		clsReg.WriteMcTime(dSTOP, st_handler_info.tStop);
		clsReg.WriteMcTime(dJAM, st_handler_info.tJam);
		clsReg.WriteAlarmInfo();*/

		AllStop[0] = 1;
		AllStop[1] = 1;

		OnThreadDelete();

		OnConfigSave();

		CloseDll();

		CFrameWnd::OnClose();
	}
}


void CMainFrame::OnIo()
{
	if (st_handler_info.nMachineMode	!= MACHINE_MANUAL) return;
	if ( OnMenuChangeChecking() == RET_ERROR ) return ;
	if ( GetActiveView()->IsKindOf(RUNTIME_CLASS(CScreenIO)) )   return;

	// **************************************************************************
	// 메뉴 사용 불가능한 경우에 대한 조건 검사한다                              
	// -> 메인트 모드 또는 티칭 모드가 설정되어 있는지 검사                      
	// **************************************************************************
	if (st_handler_info.nLevelTeach !=  TRUE)
	{
		if (st_handler_info.nLevelMaint !=  TRUE) 
		{
			OnLevel();
			if (st_handler_info.nLevelTeach !=  TRUE)
			{
				if (st_handler_info.nLevelMaint !=  TRUE)  return;
			}
		}
	}

	OnSwitchToForm(IDW_SCREEN_IO_MAP);
}


void CMainFrame::OnLevel()
{
	CDialog_Pass_Check  pass_chk_dlg;
	int nResponse;

	if (st_handler_info.nMachineMode	!= MACHINE_MANUAL) return;
	if ( OnMenuChangeChecking() == RET_ERROR ) return ;
	if ( GetActiveView()->IsKindOf(RUNTIME_CLASS(CScreenLock)) )   return;

	pass_chk_dlg.m_nPassLevel = 5;
	nResponse = (int)pass_chk_dlg.DoModal();

	if (nResponse == IDOK)
	{
		if (st_handler_info.cWndList != NULL)
		{
			//wsprintfA(st_other_info.cNormalMsg, "%S", _T("Level Change Success.."));
			//clsFunc.OnStringToChar(_T("Level Change Success.."), st_other_info.cNormalMsg);
			clsMem.OnNormalMessageWrite(_T("Level Change Success.."));
			//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
		}
	}
	else
	{
		if (st_handler_info.cWndList != NULL)
		{
			//wsprintfA(st_other_info.cNormalMsg, "%S", _T("Level Change Fail.."));
			//clsFunc.OnStringToChar(_T("Level Change Success.."), st_other_info.cNormalMsg);
			clsMem.OnNormalMessageWrite(_T("Level Change Fail.."));
			//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
		}
	}
}


void CMainFrame::OnList()
{
	if (st_handler_info.nMachineMode	!= MACHINE_MANUAL) return;
	if ( OnMenuChangeChecking() == RET_ERROR ) return ;
	if ( GetActiveView()->IsKindOf(RUNTIME_CLASS(CScreenList)) )   return;

	OnSwitchToForm(IDW_SCREEN_LIST);
}

void CMainFrame::OnLock()
{
	CDialog_Pass_Check  pass_chk_dlg;
	int nResponse;

	if (st_handler_info.nMachineMode	!= MACHINE_MANUAL) return;
	if ( OnMenuChangeChecking() == RET_ERROR ) return ;
	if ( GetActiveView()->IsKindOf(RUNTIME_CLASS(CScreenLock)) )   return;

	pass_chk_dlg.m_nPassLevel = 2;
	nResponse = (int)pass_chk_dlg.DoModal();

	if (nResponse == IDOK)
	{
		//::PostMessage(st_handler_info.hWnd, WM_FORM_CHANGE, SCREEN_LOCK, 2);  // 메인 프레임에 SYSTEM LOCK 화면 출력 요청
		OnSwitchToForm(IDW_SCREEN_LOCK);

		if (st_handler_info.cWndList != NULL)
		{
			//wsprintfA(st_other_info.cNormalMsg, "%S", _T("System Locking."));
			//clsFunc.OnStringToChar(_T("System Locking"), st_other_info.cNormalMsg);
			clsMem.OnNormalMessageWrite(_T("System Locking.."));
			//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);
		}
	}
}

void CMainFrame::OnMain()
{
	if ( OnMenuChangeChecking() == RET_ERROR ) return ;
	if ( GetActiveView()->IsKindOf(RUNTIME_CLASS(CScreenMain)) )   return;

	OnSwitchToForm(IDW_SCREEN_MAIN);
}


void CMainFrame::OnMenu()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CMainFrame::OnMotor()
{
	if (st_handler_info.nMachineMode	!= MACHINE_MANUAL) return;
	if ( OnMenuChangeChecking() == RET_ERROR ) return ;
	if ( GetActiveView()->IsKindOf(RUNTIME_CLASS(CScreenMotor)) )   return;

	// **************************************************************************
	// 메뉴 사용 불가능한 경우에 대한 조건 검사한다                              
	// -> 메인트 모드 또는 티칭 모드가 설정되어 있는지 검사                      
	// **************************************************************************
	if (st_handler_info.nLevelTeach !=  TRUE)
	{
		if (st_handler_info.nLevelMaint !=  TRUE) 
		{
			OnLevel();
			if (st_handler_info.nLevelTeach !=  TRUE)
			{
				if (st_handler_info.nLevelMaint !=  TRUE)  return;
			}
		}
	}

	OnSwitchToForm(IDW_SCREEN_MOTOR);
}


//void CMainFrame::OnPgminfo()
//{
//	if ( OnMenuChangeChecking() == RET_ERROR ) return ;
//	if ( GetActiveView()->IsKindOf(RUNTIME_CLASS(CScreenPrgInfo)) )   return;
//
//	OnSwitchToForm(IDW_SCREEN_PGM_INFO);
//}


void CMainFrame::OnReset()
{
	CDialog_Pass_Check  pass_chk_dlg;

	int nResponse;

	if ( OnMenuChangeChecking() == RET_ERROR ) return ;
	if ( GetActiveView()->IsKindOf(RUNTIME_CLASS(CScreenInitial)) )   return;

	// **************************************************************************
	// 메뉴 사용 불가능한 경우에 대한 조건 검사한다                              
	// -> 메인트 모드 또는 티칭 모드가 설정되어 있는지 검사                      
	// **************************************************************************
	if (st_lot_info[LOT_CURR].nLotStatus == LOT_START || st_lot_info[LOT_CURR].nLotStatus == LOT_END_START) return;
	if (st_lot_info[LOT_NEXT].nLotStatus == LOT_START || st_lot_info[LOT_NEXT].nLotStatus == LOT_END_START) return;

	
	pass_chk_dlg.m_nPassLevel = 7;
	nResponse = (int)pass_chk_dlg.DoModal();

	if (nResponse == IDOK)
	{
		OnSwitchToForm(IDW_SCREEN_INIT);
	}
}


void CMainFrame::OnSave()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CDialog_Message  dlgMsg;
	int nResponse;
	if (st_handler_info.nMenuLock	!= FALSE)  return;
	if (st_handler_info.nSwitchLock	!= FALSE)  return;  // 현재 시스템 Lock 상태인 경우 리턴 
	
	dlgMsg.m_nMessageType	= 1;
	dlgMsg.m_strMessage		= _T("Do you want to save the changed information?");

	nResponse = (int)dlgMsg.DoModal();
	
	if(nResponse == IDOK)
	{
		st_handler_info.nMenuLock = TRUE;
		
		OnConfigSave();					// 화면에 설정된 모든 데이터 저장 함수
		
		st_handler_info.nMenuLock = FALSE;
	}
}


void CMainFrame::OnSetting()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if (st_handler_info.nMachineMode	!= MACHINE_MANUAL) return;
	if ( OnMenuChangeChecking() == RET_ERROR ) return ;
	if ( GetActiveView()->IsKindOf(RUNTIME_CLASS(CScreenWork)) )   return;

	// **************************************************************************
	// 메뉴 사용 불가능한 경우에 대한 조건 검사한다                              
	// -> 메인트 모드 또는 티칭 모드가 설정되어 있는지 검사                      
	// **************************************************************************
	if (st_handler_info.nLevelTeach !=  TRUE)
	{
		if (st_handler_info.nLevelMaint !=  TRUE) 
		{
			OnLevel();
			if (st_handler_info.nLevelTeach !=  TRUE)
			{
				if (st_handler_info.nLevelMaint !=  TRUE)  return;
			}
		}
	}

	OnSwitchToForm(IDW_SCREEN_WORK_INFO);
}


void CMainFrame::OnVer()
{
	CDialog_About dlgABOUT;
	int nResponse;
	
	if (st_handler_info.nRunStatus != dSTOP) return;

	nResponse = (int)dlgABOUT.DoModal();
	if (nResponse == IDOK)
	{
	}
}

void CMainFrame::OnSwitchToForm(int nForm)
{
	if (st_handler_info.nMenuLock	!= FALSE) return;
	//if (st_handler_info.nSystemLock	!= FALSE) return;		// 현재 시스템 Lock 상태인 경우 리턴 

	m_pOldActiveView = GetActiveView();			// 현재 화면에 출력된 뷰 정보 설정 (save old view)

	m_pNewActiveView = (CView*)GetDlgItem(nForm);  // 새로 출력할 뷰 정보 설정 (get new view)
	
	if(m_pNewActiveView == NULL)							// 새로 출력할 뷰가 아직 출력되지 않았는지 검사 (if it hasn't been created yet)
	{
		switch(nForm)			// 뷰에 설정된 ID 정보 비교 (these IDs are the dialog IDs of the view but can use anything)
		{
			case IDW_SCREEN_MAIN:				// 메인 화면 
				m_pNewActiveView = (CView*)new CScreenMain;
				break;

			case IDW_SCREEN_INIT:
				m_pNewActiveView = (CView*)new CScreenInitial;
				break;

			case IDW_SCREEN_BASIC:
				m_pNewActiveView = (CView*)new CScreenBasic;
				break;

			case IDW_SCREEN_WORK_INFO:
				m_pNewActiveView = (CView*)new CScreenWork;
				break;

			case IDW_SCREEN_SET_MAINTENANCE:
				break;

			case IDW_SCREEN_SET_BARCODE:
				break;

			case IDW_SCREEN_SET_INTERFACE:
				break;

			case IDW_SCREEN_SET_MEASURE:
				break;

			case IDW_SCREEN_WAIT_TIME:
				break;

			case IDW_SCREEN_MOTOR:
				m_pNewActiveView = (CView*)new CScreenMotor;
				break;

			case IDW_SCREEN_MOTOR_SPEED:
				break;

			case IDW_SCREEN_LIST:
				m_pNewActiveView = (CView*)new CScreenList;
				break;

			case IDW_SCREEN_LIST_WORK:
				break;

			case IDW_SCREEN_LIST_ALARM:
				break;

			case IDW_SCREEN_LIST_STEP:
				break;

			case IDW_SCREEN_LIST_ERROR:
				break;

			case IDW_SCREEN_IO_MAP:
				m_pNewActiveView = (CView*)new CScreenIO;
				break;

			case IDW_SCREEN_LOCK:
				m_pNewActiveView = (CView*)new CScreenLock;
				break;

			case IDW_SCREEN_ALARM:
				m_pNewActiveView = (CView*)new CScreenAlarm;
				break;

			case IDW_SCREEN_PGM_INFO:
				m_pNewActiveView = (CView*)new CScreenPrgInfo;
				break;
		}

		CCreateContext context;  // 새로운 뷰에 도큐먼트 Attach ( attach the document to the new view)
		context.m_pCurrentDoc = m_pOldActiveView->GetDocument();

		m_pNewActiveView->Create(NULL, NULL, 0L, CFrameWnd::rectDefault, this, nForm, &context);
		m_pNewActiveView->OnInitialUpdate();  		
	}

	SetActiveView(m_pNewActiveView);			// 새로 출력된 뷰를 액티브 뷰로 설정 (change the active view)
	m_pNewActiveView->ShowWindow(SW_SHOW);	// 새로운 뷰를 출력 (show the new window)

	m_pOldActiveView->ShowWindow(SW_HIDE);	// 기존에 출력된 뷰를 감춘다 (hide the old)
	m_pOldActiveView->DestroyWindow();		// 기존에 출력된 뷰를 파괴시킨다 (Old View Destroy 시키기 위하여 추가)
	m_pOldActiveView = NULL;					// 기존에 출력된 뷰에 대한 정보 저장 변수 초기화 

	::SetWindowLong(m_pNewActiveView->m_hWnd, GWL_ID, AFX_IDW_PANE_FIRST);  // gotta have it
	RecalcLayout();							// 프레임을 조율한다 (adjust frame)
	
	delete	m_pOldActiveView;					// 기존에 출력된 뷰 정보를 제거한다 (kill old view)
	
	st_handler_info.nMenuNum = nForm;

	/* ************************************************************************** */
	/* 현재 화면 정보 타이틀바에 출력한다.                                        */
	/* ************************************************************************** */
	if (st_handler_info.cWndTitle != NULL)
	{
		st_handler_info.cWndTitle->PostMessage(WM_STATUS_CHANGE, FORM_NAME, nForm);
		st_handler_info.cWndTitle->PostMessage(WM_STATUS_CHANGE, MODEL_MODE, 0);
//		st_handler_info.cWndTitle->PostMessage(WM_STATUS_CHANGE, INTERFACE_MODE, st_basic_info.nModeInterface);
		st_handler_info.cWndTitle->PostMessage(WM_STATUS_CHANGE, INTERFACE_MODE, st_handler_info.nFtpVer);
		st_handler_info.cWndTitle->PostMessage(WM_STATUS_CHANGE, XGEM_MODE, clsXgem.m_nXgemInterface);
	}
	/* ************************************************************************** */
}


int CMainFrame::OnMenuChangeChecking(void)
{
	/* ************************************************************************** */
    /* 화면 뷰 전환 불가능한 정보 검사한다.                                       */
    /* ************************************************************************** */
	if (st_handler_info.nMenuLock != FALSE)		return RET_ERROR;
	if (st_handler_info.nSystemLock != FALSE)	return RET_ERROR;  // 현재 시스템 Lock 상태인 경우 리턴 

	/* ************************************************************************** */
	/* 현재 장비 상태가 STOP 상태인지 검사                                        */
	/* ************************************************************************** */
	if (st_handler_info.nRunStatus != dSTOP && st_handler_info.nRunStatus != dLOCK)
	{
//		sprintf(st_other_info.cOpMsg, _T("[Now RUNNING] do first STOP Switch"));
//		_snprintf(st_other_info.cNormalMsg, sizeof(st_other_info.cNormalMsg), "%S", _T("[Now RUNNING] do first STOP Switch"));
		//wsprintfA(st_other_info.cNormalMsg, "%S", _T("[Now RUNNING] do first STOP Switch"));
		
		if (st_handler_info.cWndList != NULL)  
		{
			//clsFunc.OnStringToChar(_T("[Now RUNNING] do first STOP Switch"), st_other_info.cNormalMsg);
			clsMem.OnNormalMessageWrite(_T("[Now RUNNING] do first STOP Switch"));
			//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG); 
		}

		return RET_ERROR;
	}
	/* ************************************************************************** */

	return RET_GOOD;
}
//kwlee 20160905 TestSite 프로그램 시작시 무조건 다운.
 void CMainFrame::OnTestSiteDn()
 {
	clsTestSite1.Set_TestSite_UpDown(IO_ON);
	clsTestSite2.Set_TestSite_UpDown(IO_ON);
	clsTestSite3.Set_TestSite_UpDown(IO_ON);
	clsTestSite4.Set_TestSite_UpDown(IO_ON);
	clsTestSite5.Set_TestSite_UpDown(IO_ON);
	clsTestSite6.Set_TestSite_UpDown(IO_ON);
	clsTestSite7.Set_TestSite_UpDown(IO_ON);
	clsTestSite8.Set_TestSite_UpDown(IO_ON);
 }


void CMainFrame::OnMainVarDefaultSet()
{
	int i,j;
	// 핸들러 시뮬레이션 임시로 TRUE를 준다. 
	st_work_info.nSimulationMode	= 0;
	st_handler_info.nLotStatus		= LOT_CLEAR;

	st_handler_info.cWndTitle		= NULL;
	st_handler_info.cWndList		= NULL;
	st_handler_info.cWndMain		= NULL;
/*
	st_animate_info.nImgNum[ANI_PCB_LF_ELV]	= 1;
	st_animate_info.nImgNum[ANI_MGZ_ELV]	= 1;
	st_animate_info.nImgNum[ANI_JIG_IN_CVY]	= 1;
	st_animate_info.nImgNum[ANI_JIG_OUT_CVY]= 4;
	st_animate_info.nImgNum[ANI_JIG_ELV]	= 1;
	st_animate_info.nImgNum[ANI_PCB_ROBOT]	= 0;
	st_animate_info.nImgNum[ANI_MGZ_IN]		= 1;
	st_animate_info.nImgNum[ANI_MGZ_OUT]	= 1;*/
	ZeroMemory(st_other_info.nSocket, sizeof(st_other_info.nSocket));

	//kwlee 2015.1126
	for (i =0; i<2; i++)
	{
		for (j = 0; j<48; j++)
		{
			st_handler_info.m_tStopTime[i][j] = 0;
		}
		
	}

	for (i=0; i<MAX_GMS_CHANNEL; i++)
	{
		st_gms_info.ArValVolt[i].SetSize(60);
		st_gms_info.ArValRes[i].SetSize(60);
	}

	mp_msg_dlg = NULL;
	FAS_IO.set_out_bit( st_io_info.o_LdUldMuteOff, IO_ON);
	FAS_IO.set_out_bit( st_io_info.o_LdUldMuteOn, IO_OFF);

	//2016.1022
	for(int nTHD_i = THD_TESTSITE_1; nTHD_i <= THD_TESTSITE_8; nTHD_i++)
	{
		for(int nSocketNo = 0; nSocketNo < TEST_SOCKET_PARA; nSocketNo++)
		{
			st_test_site_info[nTHD_i].st_pcb_info[nSocketNo].nTestBdStart = BD_NONE; //2015.0313 추가
			st_test_site_info[nTHD_i].st_pcb_info[nSocketNo].nYesNo = CTL_NO;
		}
	}
	st_handler_info.bLoaderOnFlag = CTL_NO;
	//st_sync_info.nUnLdRbtDownZ = CTL_NO;//Direction 2016.1117
	//kwlee 2016.1124
	//st_sync_info.nSmema_Tray_Input_Req = CTL_NO;
	st_handler_info.nAutoDirectionUseChk = CTL_NO;

	st_sync_info.nAutoBufferIn = CTL_NO;

//	char chr_data[50];

	//2015.1005
// 	:: GetPrivateProfileString(_T("FILLATIO"), _T("DIR"), _T("0"), (LPWSTR)chr_data, 100, st_path_info.strFileBasic); 
// 	st_handler_info.mstr_DirPath.Format(_T("%s"),chr_data);
// 	st_handler_info.mstr_DirPath.TrimRight();
// 	st_handler_info.mstr_DirPath.TrimLeft();
// 
// 	if (st_handler_info.mstr_DirPath == _T(""))
// 	{
// 		st_handler_info.mstr_DirPath = _T("D:\\AMT840\\Fillratio\\");
// 	}
// 	clsFunc.OnCreateFolder(st_handler_info.mstr_DirPath);
}

void CMainFrame::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	return;
	CFrameWnd::OnClose();
}

void CMainFrame::OnExcelRead()
{
	clsExcel.OnOpenIOMap(st_path_info.strIoMap);
	clsExcel.OnCloseIOMap();

	clsExcel.OnOpenPartIOMap(st_path_info.strPartIoMap);
	clsExcel.OnClosePartIOMap();

	clsExcel.OnOpenMotorPart(st_path_info.strMotorPartMap);
	clsExcel.OnCloseMotorPart();

	clsExcel.OnOpenMotorPosition(st_path_info.strMotorAxisMap);
	clsExcel.OnCloseMotorPosition();

	clsExcel.OnOpenWaitTime(st_path_info.strWaitTimeMap);
	clsExcel.OnCloseWaitTime();
}


void CMainFrame::OnFilePath()
{
	CString strPath				= _T("C:\\AMT840\\");
	CString strCreate;

	clsFunc.OnCreateFolder(strPath);

	st_path_info.strBasic			=	strPath + _T("Setting\\");				// 환경 설정 정보 저장 파일에 대한 폴더 설정 
	clsFunc.OnCreateFolder(st_path_info.strBasic);
	
	st_path_info.strPathDvc			= strPath + _T("File\\");					// 디바이스별 티칭 정보 저장 파일 생성 폴더 설정
	clsFunc.OnCreateFolder(st_path_info.strPathDvc);
	
	st_path_info.strPathAlarm		= strPath + _T("Bmp\\");					// 출력 알람 정보 존재 폴더 설정
	clsFunc.OnCreateFolder(st_path_info.strPathAlarm);
	
	st_path_info.strPathOp			= strPath + _T("Operator\\");				// 오퍼레이터 정보 저장 폴더 설정
	clsFunc.OnCreateFolder(st_path_info.strPathOp);

	st_path_info.strPathBack		= strPath + _T("BackUp\\");					// BACKUP 파일 생성 폴더 설정
	clsFunc.OnCreateFolder(st_path_info.strPathBack);

	st_path_info.strFileBasic		= strPath + _T("File\\File.TXT");			// 기본 셋팅 정보 저장 파일명 설정
	st_path_info.strFileWait			= strPath + _T("WaitTime");					// 대기 시간 정보 저장 파일명 설정
	st_path_info.str_scrap_code   = strPath + _T("RETEST_RULE.TXT");		// 환경 설정 정보 저장 파일에 대한 폴더 설정 

	st_path_info.strOperation		= strPath + _T("Log\\Operation\\");			// Operation Log 파일에 대한 폴더 설정 
	clsFunc.OnCreateFolder(st_path_info.strOperation);

	st_path_info.strMachine			= strPath + _T("Log\\Machine\\");				// Machine Log 파일에 대한 폴더 설정 
	clsFunc.OnCreateFolder(st_path_info.strMachine);
	
	st_path_info.strTotal			= strPath + _T("Log\\Total\\");				// Total Log 파일에 대한 폴더 설정 
	clsFunc.OnCreateFolder(st_path_info.strTotal);

	//kwlee 2016.0612
	st_path_info.strGms			= strPath + _T("Log\\GMS\\");				// Total Log 파일에 대한 폴더 설정 
	clsFunc.OnCreateFolder(st_path_info.strGms);

	st_path_info.strData			= strPath + _T("Log\\Data\\");				// Total Log 파일에 대한 폴더 설정 
	clsFunc.OnCreateFolder(st_path_info.strData);

	st_path_info.strInterface		= strPath + _T("Log\\Interface\\");	
	clsFunc.OnCreateFolder(st_path_info.strInterface);

	st_path_info.strBarcode		= strPath + _T("Log\\Barcode\\");	
	clsFunc.OnCreateFolder(st_path_info.strBarcode);

	st_path_info.strBarcodeNg		= strPath + _T("Log\\BarcodeNg\\");	
	clsFunc.OnCreateFolder(st_path_info.strBarcodeNg);

	st_path_info.strFileMotor		= strPath + _T("Motor\\");
	clsFunc.OnCreateFolder(st_path_info.strFileMotor);

	st_path_info.strAlarmMonth		= strPath + _T("Alarm\\Monthly\\");
	clsFunc.OnCreateFolder(st_path_info.strAlarmMonth);

	st_path_info.strAlarmDay		= strPath + _T("Alarm\\Daily\\");
	clsFunc.OnCreateFolder(st_path_info.strAlarmDay);

	strCreate						= strPath + _T("Excel\\");
	clsFunc.OnCreateFolder(strCreate);

	// jtkim 20150607
	st_path_info.strPathLotWork		= strPath + _T("Log\\LOT_DATA\\WORK\\");	
	clsFunc.OnCreateFolder(st_path_info.strPathLotWork);

	st_path_info.strPathLotPcb		= strPath + _T("Log\\LOT_DATA\\RESULT\\");	
	clsFunc.OnCreateFolder(st_path_info.strPathLotPcb);

	strCreate = strPath + _T("Log\\LOT_DATA\\LD_TRAY\\");
	clsFunc.OnCreateFolder(strCreate);

	strCreate = strPath + _T("Log\\LOT_DATA\\LD_ROBOT\\");
	clsFunc.OnCreateFolder(strCreate);

	strCreate = strPath + _T("Log\\LOT_DATA\\LD_BUFFER\\");
	clsFunc.OnCreateFolder(strCreate);

	strCreate = strPath + _T("Log\\LOT_DATA\\UNLD_ROBOT\\");
	clsFunc.OnCreateFolder(strCreate);

	strCreate = strPath + _T("Log\\LOT_DATA\\UNLD_BUFFER\\");
	clsFunc.OnCreateFolder(strCreate);

	strCreate = strPath + _T("Log\\LOT_DATA\\TEST_SITE_ROBOT\\");
	clsFunc.OnCreateFolder(strCreate);

	strCreate = strPath + _T("Log\\LOT_DATA\\UNLD_TRAY\\");
	clsFunc.OnCreateFolder(strCreate);

	st_path_info.strPathCycle		= strPath + _T("Log\\LOT_DATA\\");	
	clsFunc.OnCreateFolder(st_path_info.strPathCycle);

	st_path_info.strPathTestSite	= strPath + _T("Log\\LOT_DATA\\TEST_SITE_CYCLE\\");	
	clsFunc.OnCreateFolder(st_path_info.strPathTestSite);

	st_path_info.strIoMap			= strPath + _T("Excel\\AMT_IO_MAP.xls");
	st_path_info.strPartIoMap		= strPath + _T("Excel\\AMT_IO_PART_MAP.xls");
	st_path_info.strIoPartMap		= strPath + _T("Excel\\AMT_IO_MAP.xls");
	st_path_info.strMotorPartMap	= strPath + _T("Excel\\AMT_MOTOR_PART_MAP.xls");
	st_path_info.strMotorAxisMap	= strPath + _T("Excel\\AMT_MOTOR_AXIS_MAP.xls");
	st_path_info.strWaitTimeMap		= strPath + _T("Excel\\AMT_WAITTIME_MAP.xls");

	// jtkim 20150408
	st_path_info.strPathRecipe		= _T("C:\\XGEM\\AMT840\\AMT8562\\AMT8562\\XWork\\Recipe\\");

	// 20140811 jtkim
	st_path_info.strTool			= strPath + _T("Tool.Ini");

	st_path_info.strPathRecovery	= strPath + _T("Recovery\\");
	clsFunc.OnCreateFolder(st_path_info.strPathRecovery);
}

void CMainFrame::OnConfigSave()
{
	/* **************************************************************************** */
	/* 화면에 설정된 모든 데이터 파일에 저장한다										*/
	/* -> 프로그램 종료 시점에서 오퍼레이터가 데이터 저장 파일 삭제하는 경우 대비		*/
	/* -> 장비 동작 중에 파일 삭제되더라도 현재 설정 정보 유지된다						*/
	/* **************************************************************************** */
	clsBasic.OnBasic_Data_Save();				// 전역 변수에 저장된 Basic 셋팅 정보를 파일에 저장하는 함수

	clsBasic.OnWaitTime_Data_Save();			// 전역 변수에 저장된 Wait Time 셋팅 정보를 파일에 저장하는 함수
	clsBasic.OnMotorSpeed_Set_Data_Save();		// 전역 변수에 저장된 Motor Speed 셋팅 정보를 파일에 저장하는 함수.

	clsBasic.OnModuleRobot_Teach_Data_Save();
	clsBasic.OnMaintenance_Data_Save();
	clsBasic.OnInterface_Data_Save();
	clsBasic.OnRecipe_Data_Save();					// 레시피 설정 정보를 파일에 저장
	clsBasic.OnRecoveryDataSave();
}

void CMainFrame::OnConfigLoad()
{
	/* **************************************************************************** */
	/* 화면에 설정된 모든 데이터 파일에 읽는다										*/
	/* -> 프로그램 종료 시점에서 오퍼레이터가 데이터 저장 파일 삭제하는 경우 대비		*/
	/* -> 장비 동작 중에 파일 삭제되더라도 현재 설정 정보 유지된다						*/
	/* **************************************************************************** */
/*
	int nNum = 0;
	// pcb left elevator
	// 거리
	st_animate_info.nDistance[ANI_PCB_LF_ELV]		= 100;
	// 현재 위치
	st_animate_info.dMovePos[ANI_PCB_LF_ELV]		= 0.0;
	// motor 최대위치
	st_animate_info.dMaxPos[ANI_PCB_LF_ELV]			= 200.0;
	// image 번호
	st_animate_info.nImgNum[ANI_PCB_LF_ELV]			= 0;

	// magazine elevator
	// 거리
	st_animate_info.nDistance[ANI_MGZ_ELV]			= 100;
	// 현재 위치
	st_animate_info.dMovePos[ANI_MGZ_ELV]			= 0.0;
	// motor 최대위치
	st_animate_info.dMaxPos[ANI_MGZ_ELV]			= 200.0;
	// image 번호
	st_animate_info.nImgNum[ANI_MGZ_ELV]			= 0;

	// pcb shuttle
	// 거리
	st_animate_info.nDistance[ANI_PCB_SHUTT]		= 100;
	// 현재 위치
	st_animate_info.dMovePos[ANI_PCB_SHUTT]			= 0.0;
	// motor 최대위치
	st_animate_info.dMaxPos[ANI_PCB_SHUTT]			= 200.0;
	// image 번호
	st_animate_info.nImgNum[ANI_PCB_SHUTT]			= 0;

	// pcb robot
	// 거리
	st_animate_info.nDistance[ANI_PCB_ROBOT]		= 100;
	// 현재 위치
	st_animate_info.dMovePos[ANI_PCB_ROBOT]			= 0.0;
	// motor 최대위치
	st_animate_info.dMaxPos[ANI_PCB_ROBOT]			= 200.0;
	// image 번호
	st_animate_info.nImgNum[ANI_PCB_ROBOT]			= 0;

	// jig in conveyor
	// 거리
	st_animate_info.nDistance[ANI_JIG_IN_CVY]		= 100;
	// 현재 위치
	st_animate_info.dMovePos[ANI_JIG_IN_CVY]		= 0.0;
	// motor 최대위치
	st_animate_info.dMaxPos[ANI_JIG_IN_CVY]			= 200.0;
	// image 번호
	st_animate_info.nImgNum[ANI_JIG_IN_CVY]			= 1;

	// jig out conveyor
	// 거리
	st_animate_info.nDistance[ANI_JIG_OUT_CVY]		= 100;
	// 현재 위치
	st_animate_info.dMovePos[ANI_JIG_OUT_CVY]		= 0.0;
	// motor 최대위치
	st_animate_info.dMaxPos[ANI_JIG_OUT_CVY]		= 200.0;
	// image 번호
	st_animate_info.nImgNum[ANI_JIG_OUT_CVY]		= 0;

	// jig elevator
	// 거리
	st_animate_info.nDistance[ANI_JIG_ELV]			= 100;
	// 현재 위치
	st_animate_info.dMovePos[ANI_JIG_ELV]			= 0.0;
	// motor 최대위치
	st_animate_info.dMaxPos[ANI_JIG_ELV]			= 200.0;
	// image 번호
	st_animate_info.nImgNum[ANI_JIG_ELV]			= 0;

	// jig elevator
	// 거리
	st_animate_info.nDistance[ANI_JIG_ROBOT]		= 100;
	// 현재 위치
	st_animate_info.dMovePos[ANI_JIG_ROBOT]			= 0.0;
	// motor 최대위치
	st_animate_info.dMaxPos[ANI_JIG_ROBOT]			= 200.0;
	// image 번호
	st_animate_info.nImgNum[ANI_JIG_ROBOT]			= 0;
*/
	clsBasic.OnBasic_Data_Load(0);				// 전역 변수에 저장된 Basic 셋팅 정보를 파일에 읽는 함수

	clsBasic.OnWaitTime_Data_Load();			// 전역 변수에 저장된 Wait Time 셋팅 정보를 파일에 읽는 함수
	clsBasic.OnMotorSpeed_Set_Data_Load();		// 전역 변수에 저장된 Motor Speed 셋팅 정보를 파일에 읽는 함수.

	clsBasic.OnModuleRobot_Teach_Data_Load();
	clsBasic.OnMaintenance_Data_Load();
	clsBasic.OnInterface_Data_Load();
	clsBasic.OnRecipe_Data_Load();


	clsBasic.OnRecoveryDataLoad();
	clsBasic.OnAnimateDataLoad();

	clsBasic.OnVision_Data_Load(); //kwlee 2017.0111


	//2017.0309
	//초기화 후에는 무조건 interface,with 타임아웃 설정 시간 모드로 
	st_basic_info.nModeDevice = WITH_DVC;
	st_basic_info.nModeInterface = EQP_ON_LINE;
	st_basic_info.nModeTestInterface = EQP_ON_LINE;
	//처음 로딩시에는 무조건 타임아웃설정
	st_recipe_info.nAbortTime = 15000;

}

void CMainFrame::OnThreadDelete()
{
	while(WaitForSingleObject(m_thrHandle[0], 5000) != WAIT_OBJECT_0)
	{
		if (m_thrHandle[0] == NULL) break;
		Sleep(1);
	}

	while(WaitForSingleObject(m_thrHandle[1], 5000) != WAIT_OBJECT_0)
	{
		if (m_thrHandle[1] == NULL) break;
		Sleep(1);
	}

	while(WaitForSingleObject(m_thrHandle[2], 5000) != WAIT_OBJECT_0)
	{
		if (m_thrHandle[2] == NULL) break;
		Sleep(1);
	}

	while(WaitForSingleObject(m_thrHandle[3], 5000) != WAIT_OBJECT_0)
	{
		if (m_thrHandle[3] == NULL) break;
		Sleep(1);
	}

	while(WaitForSingleObject(m_thrHandle[4], 5000) != WAIT_OBJECT_0)
	{
		if (m_thrHandle[4] == NULL) break;
		Sleep(0);
	}

	while(WaitForSingleObject(m_thrHandle[5], 5000) != WAIT_OBJECT_0)
	{
		if (m_thrHandle[5] == NULL) break;
		Sleep(0);
	}

	while(WaitForSingleObject(m_thrHandle[6], 5000) != WAIT_OBJECT_0)
	{
		if (m_thrHandle[6] == NULL) break;
		Sleep(0);
	}

	while(WaitForSingleObject(m_thrHandle[7], 5000) != WAIT_OBJECT_0)
	{
		if (m_thrHandle[7] == NULL) break;
		Sleep(0);
	}

	while(WaitForSingleObject(m_thrHandle[8], 5000) != WAIT_OBJECT_0)
	{
		if (m_thrHandle[8] == NULL) break;
		Sleep(0);
	}

	while(WaitForSingleObject(m_thrHandle[9], 5000) != WAIT_OBJECT_0)
	{
		if (m_thrHandle[9] == NULL) break;
		Sleep(0);
	}

	while(WaitForSingleObject(m_thrHandle[10], 5000) != WAIT_OBJECT_0)
	{
		if (m_thrHandle[10] == NULL) break;
		Sleep(0);
	}

	while(WaitForSingleObject(m_thrHandle[11], 5000) != WAIT_OBJECT_0)
	{
		if (m_thrHandle[11] == NULL) break;
		Sleep(0);
	}

	while(WaitForSingleObject(m_thrHandle[12], 5000) != WAIT_OBJECT_0)
	{
		if (m_thrHandle[12] == NULL) break;
		Sleep(0);
	}

	while(WaitForSingleObject(m_thrHandle[13], 5000) != WAIT_OBJECT_0)
	{
		if (m_thrHandle[13] == NULL) break;
		Sleep(0);
	}

	while(WaitForSingleObject(m_thrHandle[14], 5000) != WAIT_OBJECT_0)
	{
		if (m_thrHandle[14] == NULL) break;
		Sleep(0);
	}

	while(WaitForSingleObject(m_thrHandle[15], 5000) != WAIT_OBJECT_0)
	{
		if (m_thrHandle[15] == NULL) break;
		Sleep(0);
	}

	while(WaitForSingleObject(m_thrHandle[16], 5000) != WAIT_OBJECT_0)
	{
		if (m_thrHandle[16] == NULL) break;
		Sleep(0);
	}

	while(WaitForSingleObject(m_thrHandle[17], 5000) != WAIT_OBJECT_0)
	{
		if (m_thrHandle[17] == NULL) break;
		Sleep(0);
	}

	while(WaitForSingleObject(m_thrHandle[18], 5000) != WAIT_OBJECT_0)
	{
		if (m_thrHandle[18] == NULL) break;
		Sleep(0);
	}

	while(WaitForSingleObject(m_thrHandle[19], 5000) != WAIT_OBJECT_0)
	{
		if (m_thrHandle[19] == NULL) break;
		Sleep(0);
	}

	while(WaitForSingleObject(m_thrHandle[20], 5000) != WAIT_OBJECT_0)
	{
		if (m_thrHandle[20] == NULL) break;
		Sleep(0);
	}

	while(WaitForSingleObject(m_thrHandle[21], 5000) != WAIT_OBJECT_0)
	{
		if (m_thrHandle[21] == NULL) break;
		Sleep(0);
	}

	while(WaitForSingleObject(m_thrHandle[22], 5000) != WAIT_OBJECT_0)
	{
		if (m_thrHandle[22] == NULL) break;
		Sleep(0);
	}

	while(WaitForSingleObject(m_thrHandle[23], 5000) != WAIT_OBJECT_0)
	{
		if (m_thrHandle[23] == NULL) break;
		Sleep(0);
	}

	while(WaitForSingleObject(m_thrHandle[24], 5000) != WAIT_OBJECT_0)
	{
		if (m_thrHandle[24] == NULL) break;
		Sleep(0);
	}

	while(WaitForSingleObject(m_thrHandle[25], 5000) != WAIT_OBJECT_0)
	{
		if (m_thrHandle[25] == NULL) break;
		Sleep(0);
	}

	while(WaitForSingleObject(m_thrHandle[26], 5000) != WAIT_OBJECT_0)
	{
		if (m_thrHandle[26] == NULL) break;
		Sleep(0);
	}

	while(WaitForSingleObject(m_thrHandle[27], 5000) != WAIT_OBJECT_0)
	{
		if (m_thrHandle[27] == NULL) break;
		Sleep(0);
	}

	while(WaitForSingleObject(m_thrHandle[28], 5000) != WAIT_OBJECT_0)
	{
		if (m_thrHandle[28] == NULL) break;
		Sleep(0);
	}

	while(WaitForSingleObject(m_thrHandle[29], 5000) != WAIT_OBJECT_0)
	{
		if (m_thrHandle[29] == NULL) break;
		Sleep(0);
	}
}


void CMainFrame::OnThreadCreate()
{
	for(int i=0; i<40; i++) 
	{
		m_thread[i] = NULL;  // 쓰레드 핸들 초기화
	}

	m_thread[0] = AfxBeginThread(OnThreadReadIO, this);  // I/O 입출력 상태 읽기 쓰레드
	if (m_thread[0] != NULL) 
	{
		m_thrHandle[0] = m_thread[0]->m_hThread;
	}
	else 
	{
		AfxMessageBox(_T("Cannot begin the thread [thrReadIO]"));
	}

	m_thread[1] = AfxBeginThread(OnThreadHandlerCheck, this);  // 타워 램프 상태 변경 쓰레드
	if (m_thread[1] != NULL) 
	{
		m_thrHandle[1] = m_thread[1]->m_hThread;
	}
	else 
	{
		AfxMessageBox(_T("Cannot begin the thread [OnThreadHandlerCheck]"));
	}

	m_thread[2] = AfxBeginThread(OnThreadTowerLamp, this);  // 타워 램프 상태 변경 쓰레드
	if (m_thread[2] != NULL) 
	{
		m_thrHandle[2] = m_thread[2]->m_hThread;
	}
	else 
	{
		AfxMessageBox(_T("Cannot begin the thread [thrTowerLamp]"));
	}

	m_thread[3] = AfxBeginThread(OnThreadAlarmDisplay, this);  // 알람 출력 쓰레드
	if (m_thread[3] != NULL) 
	{
		m_thrHandle[3] = m_thread[3]->m_hThread;
	}
	else 
	{
		AfxMessageBox(_T("Cannot begin the thread [thrAlarmDisplay]"));
	}

	m_thread[4] = AfxBeginThread(OnThreadInterface, this);  // 알람 출력 쓰레드
	if (m_thread[4] != NULL) 
	{
		m_thrHandle[4] = m_thread[4]->m_hThread;
	}
	else 
	{
		AfxMessageBox(_T("Cannot begin the thread [thrInterface]"));
	}

	m_thread[28] = AfxBeginThread(OnThreadTestSiteTop, this);
	if (m_thread[28] != NULL) 
	{
		m_thrHandle[28] = m_thread[28]->m_hThread;
	}
	else 
	{
		AfxMessageBox(_T("Cannot begin the thread [threadTestSiteTop]"));
	}

	m_thread[29] = AfxBeginThread(OnThreadTestSiteBottom, this);
	if (m_thread[29] != NULL) 
	{
		m_thrHandle[29] = m_thread[29]->m_hThread;
	}
	else 
	{
		AfxMessageBox(_T("Cannot begin the thread [threadTestSiteBottom]"));
	}
	
}


void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	COleDateTime cur_time;
	COleDateTime next_time;
	COleDateTimeSpan timeSpan;
	COleDateTimeSpan timeSpanUph;
	CTime bin_time;
	int nDay;
	int nHour;
	int nSecond;
	int nMinute;
	int nDiff[2];
	int i;

	CString strTemp;
	CString strLog,strlog_1;

	if (nIDEvent == TM_MAIN_REFRESH)
	{
 		CTime cur				= CTime::GetCurrentTime();
 		CTimeSpan diff			= cur - st_handler_info.tRef;

		st_handler_info.tRef	= cur;
		
		//20151005
		st_handler_info.m_tDRef = st_handler_info.m_tDRef + diff;


		//2016.1117
		if (FAS_IO.get_in_bit(st_io_info.i_TsiteDoorChk[0], IO_OFF) == IO_OFF || 
			FAS_IO.get_in_bit(st_io_info.i_TopLdUldDoorChk[0], IO_OFF) == IO_OFF )
		{
			//kwlee 2016.1124
			//st_sync_info.nSmema_Tray_Input_Req = CTL_YES;
			st_handler_info.nAutoDirectionUseChk = CTL_YES;
		}

		for (i =0; i<TSITE_SOCKET_CNT;i++)
		{
			st_DB_time.n_Db_time[REF_TIME][i] = st_handler_info.m_tDRef;	
		}

		if (st_handler_info.nMaintMode == YES)		// 메인트 모드이면 메인트 타임만 업데이트 된다.
		{
			// jtkim 20150307
			//if (st_handler_info.nLotStatus == LOT_START)
			if (st_lot_info[LOT_CURR].nLotStatus == LOT_START)
			{
				st_handler_info.tMaint = st_handler_info.tMaint + diff;
			}
			st_handler_info.tMaint = st_handler_info.tDMaint + diff;
			
			if (st_handler_info.cWndMain != NULL)
			{
				st_handler_info.cWndMain->PostMessage(WM_WORK_DISPLAY, MAIN_TIMEINFO, dMAINT);
			}
		}
		else
		{
			switch(st_handler_info.nRunStatus)
			{			
				case dRUN:			
					if (st_handler_info.nIdleStatus == NO)
					{
						st_handler_info.tRun = st_handler_info.tRun + diff;
						// jtkim 20150529
						st_handler_info.tRunUph = st_handler_info.tRunUph + diff;

						// jtkim 20160328 product run time
						st_product_info.nRunTime += (long)diff.GetTotalSeconds();
						if (st_product_info.nAlarmCount > 0)
						{
							st_product_info.dMtbi = ((double)st_product_info.nRunTime / (double)st_product_info.nAlarmCount) / (double)60;
						}
						else st_product_info.dMtbi = st_product_info.nRunTime / (double)60;
					}
					else
					{
						// jtkim 20150307
						//if (st_handler_info.nLotStatus == LOT_START)
						if (st_lot_info[LOT_CURR].nLotStatus == LOT_START)
						{
							// jtkim 20150307
							//if (st_lot_info.strLotNo != _T(""))
							if (st_lot_info[LOT_CURR].strLotNo != _T(""))
							{
								st_handler_info.tRun = st_handler_info.tRun + diff;
								// jtkim 20150529
								st_handler_info.tRunUph = st_handler_info.tRunUph + diff;
							}
							else
							{
								st_handler_info.tStop = st_handler_info.tStop + diff;
							}
						}
						else
						{
							// jtkim 20150307
							//if (st_handler_info.nLotStatus == LOT_START)
							if (st_lot_info[LOT_CURR].nLotStatus == LOT_START)
							{
								st_handler_info.tStop = st_handler_info.tStop + diff;
							}
							st_handler_info.tDStop = st_handler_info.tDStop + diff;
						}
						// jtkim 20160328 product idle time
						st_product_info.nIdleTime += (long)diff.GetTotalSeconds();
					}
					break;

				case dWARNING:
				case dJAM:		
					// jtkim 20150307
					//if (st_handler_info.nLotStatus == LOT_START)
					if (st_lot_info[LOT_CURR].nLotStatus == LOT_START)
					{
						st_handler_info.tJam = st_handler_info.tJam + diff;
					}
					st_handler_info.tDJam = st_handler_info.tDJam + diff;

					// jtkim 20160328 product alarm time
					st_product_info.nAlarmTime += (long)diff.GetTotalSeconds();
					break;

				case dLOCK:
					// jtkim 20150307
					//if (st_handler_info.nLotStatus == LOT_START)
					if (st_lot_info[LOT_CURR].nLotStatus == LOT_START)
					{
						st_handler_info.tMaint = st_handler_info.tMaint + diff;
					}
					st_handler_info.tDMaint = st_handler_info.tDMaint + diff;

					// jtkim 20160328 product stop time
					if (st_handler_info.nInitialSuccess == YES)
					{
						st_product_info.nStopTime += (long)diff.GetTotalSeconds();
					}
					break;
			
				case dLOTEND:												// Lot End 시에도 Stop으로 시간을 올린다.
				case dSTOP:
					// jtkim 20150307
					//if (st_handler_info.nLotStatus == LOT_START)
					if (st_lot_info[LOT_CURR].nLotStatus == LOT_START)
					{
						// jtkim 20150307
						//if (st_lot_info.strLotNo != _T(""))
						if (st_lot_info[LOT_CURR].strLotNo != _T(""))
						{
							st_handler_info.tStop = st_handler_info.tStop + diff;
						}
					}
					st_handler_info.tDStop = st_handler_info.tDStop + diff;

					// jtkim 20160328 product stop time
					if (st_handler_info.nInitialSuccess == YES)
					{
						st_product_info.nStopTime += (long)diff.GetTotalSeconds();
					}
					break;
				}

				// jtkim 20130627
				if (st_handler_info.cWndMain != NULL)
				{
					st_handler_info.cWndMain->PostMessage(WM_WORK_DISPLAY, MAIN_TIMEINFO, st_handler_info.nRunStatus);
				}
		}
	}
	else if (nIDEvent == TM_FILE_CREATE)
	{
		cur_time = COleDateTime::GetCurrentTime();
	
		if (cur_time.GetSecond() == 0)
		{
			timeSpan = cur_time - st_handler_info.tCreate;

			nDay	= (int)timeSpan.GetTotalDays();

			if (nDay >= 1)
			{
				if (cur_time.GetMinute() == 0)
				{
					//st_handler_info.tCreate.SetDateTime(st_handler_info.tCreate.GetYear(), st_handler_info.tCreate.GetMonth(), st_handler_info.tCreate.GetDay(), 22, 0, 0);
					next_time.SetDateTime(st_handler_info.tCreate.GetYear(), st_handler_info.tCreate.GetMonth(), st_handler_info.tCreate.GetDay(), 22, 0, 0); 
					st_handler_info.tCreate = next_time + COleDateTimeSpan(1, 0, 0, 0);
					// Daily Data 저장
					clsFunc.OnDailyDataSave();
					// 현재Main 화면에 표시되는 시간 초기화..k
					
					clsFunc.OnTimeReset(); 
				}
			}

			timeSpanUph = cur_time - st_handler_info.tUph;
			nHour	= (int)timeSpanUph.GetTotalHours();
			if (nHour >= 1)
			{
				// Daily Data 저장
				if (cur_time.GetMinute() == 0)
				{
					st_handler_info.tUph.SetDateTime(cur_time.GetYear(), cur_time.GetMonth(), cur_time.GetDay(), cur_time.GetHour(), 0, 0);

					if (cur_time.GetHour() == 23)
					{
						// jtkim 20150721
						st_count_info.nDailyUph = 0;
						st_count_info.nUphCnt	= 0;
						st_count_info.dDailyPer = 0.0f;

						next_time = cur_time + COleDateTimeSpan(1, 0, 0, 0);
						st_handler_info.strHourUph.Format(_T("HOUR_DATA%04d%02d%02d"), next_time.GetYear(), next_time.GetMonth(), next_time.GetDay());
					}
					else
					{
						if (st_handler_info.strHourUph == _T(""))
						{
							st_handler_info.strHourUph.Format(_T("HOUR_DATA%04d%02d%02d"), cur_time.GetYear(), cur_time.GetMonth(), cur_time.GetDay());
						}
					}

					clsFunc.OnHourDataSave(st_handler_info.strHourUph, 
										   st_count_info.nUph,
										   st_count_info.dHourPer,
										   st_count_info.nSocketStart, 
										   st_count_info.nSocketOn,
										   st_count_info.nSocketOff,
										   st_work_info.strSocketOff,
										   st_work_info.strSocketOn);

					st_count_info.nUph = 0;
					//jtkim 20160111
					st_count_info.nSocketStart	= 0;
					st_count_info.nSocketOn		= 0;
					st_count_info.nSocketOff	= 0;
					st_count_info.dHourPer		= 0.0f;

					st_work_info.strSocketOff	= _T("");
					st_work_info.strSocketOn	= _T("");

					// 생산량 관련 데이터 저장
					if (st_basic_info.nModeDevice == WITH_DVC)
					{
						OnProductData();
					}
					st_handler_info.tRunUph = 0;
				}
			}

// 			timeSpan = cur_time - m_tGms;
// 			nMinute = (int)timeSpan.GetTotalMinutes();
// 			if (st_basic_info.nGmsTime <= 0) return;
// 
// 			//nDiff[0] = nMinute % (st_basic_info.nVnRTime);
// 			nDiff[0] = nMinute % (st_basic_info.nGmsTime);
		
			nHour = cur_time.GetHour();
			nMinute = cur_time.GetMinute();
			nSecond = cur_time.GetSecond();

			nDiff[0] = nHour * 60 + nMinute;
			if (nDiff[0] % st_basic_info.nGmsTime == 0)
			{
				st_gms_info.strMsg[1] = st_gms_info.strMsg[0];
// 				if (st_client_info[GMS_NETWORK].nConnect == YES)
// 				{
//					SetTimer(TM_GMS,200,NULL);
//				}
//				OnWriteGmsLog();
			}
		}
	}
	else if (nIDEvent == TM_GMS)
	{
		//clsGms.OnCommunication();
		//OnGmsLog();

//		if (clsGms.OnCommunication() == RET_ERROR)
//		{
//			KillTimer(TM_GMS);
//		}
	}
	else if (nIDEvent == TM_XGEM)
	{
		if (OnXgemInterface() == RET_GOOD)
		{
			KillTimer(TM_XGEM);
		}
	}
	CFrameWnd::OnTimer(nIDEvent);
}


void CMainFrame::OnPgminfo()
{
}
/*
LRESULT CMainFrame::OnClientGms(WPARAM wParam, LPARAM lParam)
{
	CString strMsg;

	int nLength;
	
	switch(wParam)
	{
		case CLIENT_CONNECT:
			if (m_pClient[GMS_NETWORK] != NULL)
			{
				//				if (st_client_info[GMS_NETWORK].nConnect == YES) m_pClient[GMS_NETWORK]->Close();

				st_client_info[GMS_NETWORK].nConnect = NO;

				delete m_pClient[GMS_NETWORK];
				m_pClient[GMS_NETWORK] = NULL;

				m_pClient[GMS_NETWORK] = new CClientSocket();
				m_pClient[GMS_NETWORK]->Create();

				m_pClient[GMS_NETWORK]->OnOpen(this, GMS_NETWORK);
				if (m_pClient[GMS_NETWORK]->Connect(st_client_info[GMS_NETWORK].strIp, st_client_info[GMS_NETWORK].nPort))
				{
					delete m_pClient[GMS_NETWORK];
					m_pClient[GMS_NETWORK] = NULL;

					if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
					{
						//clsFunc.OnStringToChar(_T("[TCP/IP] RFID Client Connect Error."), st_other_info.cAbnormalMsg);
						clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] TTS_VNR_NETWORK Client Connect Error."));
						//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
					}
					return 0;
				}

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					//clsFunc.OnStringToChar(_T("[TCP/IP] RFID Client Connect Success."), st_other_info.cNormalMsg);
					clsMem.OnNormalMessageWrite(_T("[TCP/IP] TTS_VNR_NETWORK Client Connect Success."));
					st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			else
			{
				st_client_info[GMS_NETWORK].nConnect = NO;

				m_pClient[GMS_NETWORK] = new CClientSocket();
				m_pClient[GMS_NETWORK]->Create();

				m_pClient[GMS_NETWORK]->OnOpen(this, GMS_NETWORK);
				if (m_pClient[GMS_NETWORK]->Connect(st_client_info[GMS_NETWORK].strIp, st_client_info[GMS_NETWORK].nPort))
				{
					delete m_pClient[GMS_NETWORK];
					m_pClient[GMS_NETWORK] = NULL;

					if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
					{
						//clsFunc.OnStringToChar(_T("[TCP/IP] RFID Client Connect Error."), st_other_info.cAbnormalMsg);
						clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] TTS_VNR_NETWORK Client Connect Error."));
						st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
					}
					return 0;
				}

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					//clsFunc.OnStringToChar(_T("[TCP/IP] RFID Client Connect Success."), st_other_info.cNormalMsg);
					clsMem.OnNormalMessageWrite(_T("[TCP/IP] TTS_VNR_NETWORK Client Connect Success."));
					st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			break;

		case CLIENT_CLOSE:
			if (m_pClient[GMS_NETWORK] != NULL)
			{
				//				if (st_client_info[GMS_NETWORK].nConnect == YES) m_pClient[GMS_NETWORK]->Close();

				st_client_info[GMS_NETWORK].nConnect = NO;

				delete m_pClient[GMS_NETWORK];
				m_pClient[GMS_NETWORK] = NULL;
			}
			break;

		case CLIENT_SEND:
			if (st_client_info[GMS_NETWORK].nConnect == YES)
			{
				if (m_pClient[GMS_NETWORK] == NULL) return 0;

				clsFunc.OnStringToChar(st_client_info[GMS_NETWORK].strSend, st_client_info[GMS_NETWORK].chSend);
				nLength = st_client_info[GMS_NETWORK].strSend.GetLength();

				m_pClient[GMS_NETWORK]->Send(st_client_info[GMS_NETWORK].chSend, nLength);

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					strMsg.Format(_T("[TTS_VNR_NETWORK] %s Send"), st_client_info[GMS_NETWORK].strSend);
					//clsFunc.OnStringToChar(strMsg, st_other_info.cNormalMsg);
					clsMem.OnNormalMessageWrite(strMsg);
					st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			break;

		case CLIENT_REV:
			clsGms.OnDataReceive(st_client_info[GMS_NETWORK].strRev);
			break;

		case CLIENT_ACCEPT:
			if (lParam != 0)
			{
				st_client_info[GMS_NETWORK].nConnect = NO;

		
				delete m_pClient[GMS_NETWORK];
				m_pClient[GMS_NETWORK] = NULL;

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					//clsFunc.OnStringToChar(_T("[TCP/IP] RFID Client Accept Fail."), st_other_info.cAbnormalMsg);
					clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] TTS_VNR_NETWORK Client Accept Fail."));
					st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
				}
				return 0;
			}

			st_client_info[GMS_NETWORK].nConnect = YES;

			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
				//clsFunc.OnStringToChar(_T("[TCP/IP] RFID Client Accept Success."), st_other_info.cNormalMsg);
				clsMem.OnNormalMessageWrite(_T("[TCP/IP] TTS_VNR_NETWORK Client Accept Success."));
				st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
			}
			break;
		}

	return 0;
}
*/

void CMainFrame::OnWriteGmsLog()
{
	CString strLog;
	if (st_gms_info.strData ==	_T("EOS"))
	{	
		for (int i =0; i<8; i++)
		{
			strLog.Format(_T("채널 : %s Volt : %s"), st_gms_info.strChan[i], st_gms_info.strValVolt[i]);
			clsFunc.OnLogFileAdd(80, strLog);
		}
	}
}

LRESULT CMainFrame::OnClientEcFirst(WPARAM wParam, LPARAM lParam)
{
	CString strMsg;

	int nLength;

	switch(wParam)
	{
		case CLIENT_CONNECT:
			if (m_pClient[EC_FIRST_NETWORK] != NULL)
			{
				if (st_client_info[EC_FIRST_NETWORK].nConnect == YES) 
				{
					st_client_info[EC_FIRST_NETWORK].nConnect = NO;

					delete m_pClient[EC_FIRST_NETWORK];
					m_pClient[EC_FIRST_NETWORK] = NULL;
				}

				m_pClient[EC_FIRST_NETWORK] = new CClientSocket();
				m_pClient[EC_FIRST_NETWORK]->Create();
	
				m_pClient[EC_FIRST_NETWORK]->OnOpen(this, EC_FIRST_NETWORK);
				if (m_pClient[EC_FIRST_NETWORK]->Connect(st_client_info[EC_FIRST_NETWORK].strIp, st_client_info[EC_FIRST_NETWORK].nPort))
				{
					delete m_pClient[EC_FIRST_NETWORK];
					m_pClient[EC_FIRST_NETWORK] = NULL;

					if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
					{
						clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] EC_FIRST_NETWORK Client Connect Error."));
						//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
					}

					return 0;
				}

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnNormalMessageWrite(_T("[TCP/IP] EC_FIRST_NETWORK Client Connect Success."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			else
			{
				st_client_info[EC_FIRST_NETWORK].nConnect = NO;

				m_pClient[EC_FIRST_NETWORK] = new CClientSocket();
				m_pClient[EC_FIRST_NETWORK]->Create();
	
				m_pClient[EC_FIRST_NETWORK]->OnOpen(this, EC_FIRST_NETWORK);
				if (m_pClient[EC_FIRST_NETWORK]->Connect(st_client_info[EC_FIRST_NETWORK].strIp, st_client_info[EC_FIRST_NETWORK].nPort))
				{
					delete m_pClient[EC_FIRST_NETWORK];
					m_pClient[EC_FIRST_NETWORK] = NULL;

					if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
					{
						clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] EC_FIRST_NETWORK Client Connect Error."));
						//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
					}

					return 0;
				}

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnNormalMessageWrite(_T("[TCP/IP] EC_FIRST_NETWORK Client Connect Success."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			break;

		case CLIENT_CLOSE:
			if (m_pClient[EC_FIRST_NETWORK] != NULL)
			{
				st_client_info[EC_FIRST_NETWORK].nConnect = NO;

				delete m_pClient[EC_FIRST_NETWORK];
				m_pClient[EC_FIRST_NETWORK] = NULL;
			}
			break;

		case CLIENT_SEND:
			if (st_client_info[EC_FIRST_NETWORK].nConnect == YES)
			{
				if (m_pClient[EC_FIRST_NETWORK] == NULL) return 0;

				clsFunc.OnStringToChar(st_client_info[EC_FIRST_NETWORK].strSend, st_client_info[EC_FIRST_NETWORK].chSend);
				nLength = st_client_info[EC_FIRST_NETWORK].strSend.GetLength();

				m_pClient[EC_FIRST_NETWORK]->Send(st_client_info[EC_FIRST_NETWORK].chSend, nLength);

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					strMsg.Format(_T("[EC_FIRST_NETWORK] %s"), st_client_info[EC_FIRST_NETWORK].strSend);
					clsMem.OnNormalMessageWrite(strMsg);
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			break;

		case CLIENT_REV:
			clsEcFirst.OnDataReceive(st_client_info[EC_FIRST_NETWORK].strRev);
			break;

		case CLIENT_ACCEPT:
			if (lParam != 0)
			{
				st_client_info[EC_FIRST_NETWORK].nConnect = NO;

				delete m_pClient[EC_FIRST_NETWORK];
				m_pClient[EC_FIRST_NETWORK] = NULL;

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] EC_FIRST_NETWORK Client Accept Fail."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
				}

				return 0;
			}

			st_client_info[EC_FIRST_NETWORK].nConnect = YES;

			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
				clsMem.OnNormalMessageWrite(_T("[TCP/IP] EC_FIRST_NETWORK Client Accept Success."));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
			}
			break;
	}

	return 0;
}

LRESULT CMainFrame::OnClientEcSecond(WPARAM wParam, LPARAM lParam)
{
	CString strMsg;

	int nLength;

	switch(wParam)
	{
		case CLIENT_CONNECT:
			if (m_pClient[EC_SECOND_NETWORK] != NULL)
			{
				if (st_client_info[EC_SECOND_NETWORK].nConnect == YES) 
				{
					st_client_info[EC_SECOND_NETWORK].nConnect = NO;

					delete m_pClient[EC_SECOND_NETWORK];
					m_pClient[EC_SECOND_NETWORK] = NULL;
				}

				m_pClient[EC_SECOND_NETWORK] = new CClientSocket();
				m_pClient[EC_SECOND_NETWORK]->Create();
	
				m_pClient[EC_SECOND_NETWORK]->OnOpen(this, EC_SECOND_NETWORK);
				if (m_pClient[EC_SECOND_NETWORK]->Connect(st_client_info[EC_FIRST_NETWORK].strIp, st_client_info[EC_FIRST_NETWORK].nPort))
				{
					delete m_pClient[EC_SECOND_NETWORK];
					m_pClient[EC_SECOND_NETWORK] = NULL;

					if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
					{
						clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] EC_SECOND_NETWORK Client Connect Error."));
						//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
					}

					return 0;
				}

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnNormalMessageWrite(_T("[TCP/IP] EC_SECOND_NETWORK Client Connect Success."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			else
			{
				st_client_info[EC_SECOND_NETWORK].nConnect = NO;

				m_pClient[EC_SECOND_NETWORK] = new CClientSocket();
				m_pClient[EC_SECOND_NETWORK]->Create();
	
				m_pClient[EC_SECOND_NETWORK]->OnOpen(this, EC_SECOND_NETWORK);
				if (m_pClient[EC_SECOND_NETWORK]->Connect(st_client_info[EC_FIRST_NETWORK].strIp, st_client_info[EC_FIRST_NETWORK].nPort))
				{
					delete m_pClient[EC_SECOND_NETWORK];
					m_pClient[EC_SECOND_NETWORK] = NULL;

					if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
					{
						clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] EC_SECOND_NETWORK Client Connect Error."));
						//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
					}

					return 0;
				}

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnNormalMessageWrite(_T("[TCP/IP] EC_SECOND_NETWORK Client Connect Success."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			break;

		case CLIENT_CLOSE:
			if (m_pClient[EC_SECOND_NETWORK] != NULL)
			{
				st_client_info[EC_SECOND_NETWORK].nConnect = NO;

				delete m_pClient[EC_SECOND_NETWORK];
				m_pClient[EC_SECOND_NETWORK] = NULL;
			}
			break;

		case CLIENT_SEND:
			if (st_client_info[EC_SECOND_NETWORK].nConnect == YES)
			{
				if (m_pClient[EC_SECOND_NETWORK] == NULL) return 0;

				clsFunc.OnStringToChar(st_client_info[EC_SECOND_NETWORK].strSend, st_client_info[EC_SECOND_NETWORK].chSend);
				nLength = st_client_info[EC_SECOND_NETWORK].strSend.GetLength();

				m_pClient[EC_SECOND_NETWORK]->Send(st_client_info[EC_SECOND_NETWORK].chSend, nLength);

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					strMsg.Format(_T("[EC_SECOND_NETWORK] %s"), st_client_info[EC_SECOND_NETWORK].strSend);
					clsMem.OnNormalMessageWrite(strMsg);
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			break;

		case CLIENT_REV:
			clsEcSecond.OnDataReceive(st_client_info[EC_SECOND_NETWORK].strRev);
			break;

		case CLIENT_ACCEPT:
			if (lParam != 0)
			{
				st_client_info[EC_SECOND_NETWORK].nConnect = NO;

				delete m_pClient[EC_SECOND_NETWORK];
				m_pClient[EC_SECOND_NETWORK] = NULL;

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] EC_SECOND_NETWORK Client Accept Fail."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
				}

				return 0;
			}

			st_client_info[EC_SECOND_NETWORK].nConnect = YES;

			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
				clsMem.OnNormalMessageWrite(_T("[TCP/IP] EC_SECOND_NETWORK Client Accept Success."));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
			}
			break;
	}

	return 0;
}

LRESULT CMainFrame::OnClientXgem(WPARAM wParam, LPARAM lParam)
{
	int nLength;

	switch(wParam)
	{
		case CLIENT_CONNECT:
			if (m_pClient[XGEM_NETWORK] != NULL)
			{
				//if (st_client_info[XGEM_NETWORK].nConnect == YES) m_pClient[XGEM_NETWORK]->Close();

				st_client_info[XGEM_NETWORK].nConnect = NO;

				delete m_pClient[XGEM_NETWORK];
				m_pClient[XGEM_NETWORK] = NULL;

				m_pClient[XGEM_NETWORK] = new CClientSocket();
				m_pClient[XGEM_NETWORK]->Create();
	
				m_pClient[XGEM_NETWORK]->OnOpen(this, XGEM_NETWORK);
				if (m_pClient[XGEM_NETWORK]->Connect(st_client_info[XGEM_NETWORK].strIp, st_client_info[XGEM_NETWORK].nPort))
				{
					delete m_pClient[XGEM_NETWORK];
					m_pClient[XGEM_NETWORK] = NULL;

					if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
					{
						//clsFunc.OnStringToChar(_T("[TCP/IP] Front Client Connect Error."), st_other_info.cAbnormalMsg);
						clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] XGEM_NETWORK Client Connect Error."));
						//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
					}

					return 0;
				}

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					//clsFunc.OnStringToChar(_T("[TCP/IP] Front Client Connect Success."), st_other_info.cNormalMsg);
					clsMem.OnNormalMessageWrite(_T("[TCP/IP] XGEM_NETWORK Client Connect Success."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			else
			{
				st_client_info[XGEM_NETWORK].nConnect = NO;

				m_pClient[XGEM_NETWORK] = new CClientSocket();
				m_pClient[XGEM_NETWORK]->Create();
	
				m_pClient[XGEM_NETWORK]->OnOpen(this, XGEM_NETWORK);
				if (m_pClient[XGEM_NETWORK]->Connect(st_client_info[XGEM_NETWORK].strIp, st_client_info[XGEM_NETWORK].nPort))
				{
					delete m_pClient[XGEM_NETWORK];
					m_pClient[XGEM_NETWORK] = NULL;

					if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
					{
						//clsFunc.OnStringToChar(_T("[TCP/IP] Front Client Connect Error."), st_other_info.cAbnormalMsg);
						clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] XGEM_NETWORK Client Connect Error."));
						//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
					}

					return 0;
				}

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					//clsFunc.OnStringToChar(_T("[TCP/IP] Front Client Connect Success."), st_other_info.cNormalMsg);
					clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] XGEM_NETWORK Client Connect Success."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
				}
			}
			break;

		case CLIENT_CLOSE:
			if (m_pClient[XGEM_NETWORK] != NULL)
			{
				//if (st_client_info[XGEM_NETWORK].nConnect == YES) m_pClient[XGEM_NETWORK]->Close();

				st_client_info[XGEM_NETWORK].nConnect = NO;

				delete m_pClient[XGEM_NETWORK];
				m_pClient[XGEM_NETWORK] = NULL;

				clsXgem.m_nXgemInterface = MES_OFF_LINE;
				if(st_handler_info.cWndTitle != NULL)  
				{
					st_handler_info.cWndTitle->PostMessage(WM_STATUS_CHANGE, XGEM_MODE, clsXgem.m_nXgemInterface);
 				}

				m_nXgemStep = 0;
				SetTimer(TM_XGEM, 5000, NULL);
			}
			break;

		case CLIENT_SEND:
			if (st_client_info[XGEM_NETWORK].nConnect == YES)
			{
				if (m_pClient[XGEM_NETWORK] == NULL) return 0;

				clsFunc.OnStringToChar(st_client_info[XGEM_NETWORK].strSend, st_client_info[XGEM_NETWORK].chSend);
				nLength = st_client_info[XGEM_NETWORK].strSend.GetLength();

				m_pClient[XGEM_NETWORK]->Send(st_client_info[XGEM_NETWORK].chSend, nLength);
			}
			break;

		case CLIENT_REV:
			clsXgem.OnDataReceive(st_client_info[XGEM_NETWORK].strRev);
			break;

		case CLIENT_ACCEPT:
			if (lParam != 0)
			{
				st_client_info[XGEM_NETWORK].nConnect = NO;

				delete m_pClient[XGEM_NETWORK];
				m_pClient[XGEM_NETWORK] = NULL;

				return 0;
			}

			st_client_info[XGEM_NETWORK].nConnect = YES;
			break;
	}

	return 0;
}

LRESULT CMainFrame::OnClientInterface1(WPARAM wParam, LPARAM lParam)
{
	CString strMsg;

	int nLength;

	switch(wParam)
	{
		case CLIENT_CONNECT:
			if (m_pClient[INTERFACE_NETWORK_1] != NULL)
			{
				if (st_client_info[INTERFACE_NETWORK_1].nConnect == YES) 
				{
					st_client_info[INTERFACE_NETWORK_1].nConnect = NO;

					delete m_pClient[INTERFACE_NETWORK_1];
					m_pClient[INTERFACE_NETWORK_1] = NULL;
				}

				m_pClient[INTERFACE_NETWORK_1] = new CClientSocket();
				m_pClient[INTERFACE_NETWORK_1]->Create();
	
				m_pClient[INTERFACE_NETWORK_1]->OnOpen(this, INTERFACE_NETWORK_1);
				if (m_pClient[INTERFACE_NETWORK_1]->Connect(st_client_info[INTERFACE_NETWORK_1].strIp, st_client_info[INTERFACE_NETWORK_1].nPort))
				{
					delete m_pClient[INTERFACE_NETWORK_1];
					m_pClient[INTERFACE_NETWORK_1] = NULL;

					if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
					{
						clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] INTERFACE_NETWORK_1 Client Connect Error."));
						//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
					}

					return 0;
				}

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnNormalMessageWrite(_T("[TCP/IP] INTERFACE_NETWORK_1 Client Connect Success."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			else
			{
				st_client_info[INTERFACE_NETWORK_1].nConnect = NO;

				m_pClient[INTERFACE_NETWORK_1] = new CClientSocket();
				m_pClient[INTERFACE_NETWORK_1]->Create();
	
				m_pClient[INTERFACE_NETWORK_1]->OnOpen(this, INTERFACE_NETWORK_1);
				if (m_pClient[INTERFACE_NETWORK_1]->Connect(st_client_info[INTERFACE_NETWORK_1].strIp, st_client_info[INTERFACE_NETWORK_1].nPort))
				{
					delete m_pClient[INTERFACE_NETWORK_1];
					m_pClient[INTERFACE_NETWORK_1] = NULL;

					if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
					{
						clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] INTERFACE_NETWORK_1 Client Connect Error."));
						//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
					}

					return 0;
				}

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnNormalMessageWrite(_T("[TCP/IP] INTERFACE_NETWORK_1 Client Connect Success."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			break;

		case CLIENT_CLOSE:
			if (m_pClient[INTERFACE_NETWORK_1] != NULL)
			{
				st_client_info[INTERFACE_NETWORK_1].nConnect = NO;

				delete m_pClient[INTERFACE_NETWORK_1];
				m_pClient[INTERFACE_NETWORK_1] = NULL;
			}
			break;

		case CLIENT_SEND:
			if (st_client_info[INTERFACE_NETWORK_1].nConnect == YES)
			{
				if (m_pClient[INTERFACE_NETWORK_1] == NULL) return 0;

				clsFunc.OnStringToChar(st_client_info[INTERFACE_NETWORK_1].strSend, st_client_info[INTERFACE_NETWORK_1].chSend);
				nLength = st_client_info[INTERFACE_NETWORK_1].strSend.GetLength();

				m_pClient[INTERFACE_NETWORK_1]->Send(st_client_info[INTERFACE_NETWORK_1].chSend, nLength);

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					strMsg.Format(_T("[INTERFACE_NETWORK_1] %s"), st_client_info[INTERFACE_NETWORK_1].strSend);
					clsMem.OnNormalMessageWrite(strMsg);
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			break;

		case CLIENT_REV:
			clsInterC1.OnDataReceive(st_client_info[INTERFACE_NETWORK_1].strRev);
			break;

		case CLIENT_ACCEPT:
			if (lParam != 0)
			{
				st_client_info[INTERFACE_NETWORK_1].nConnect = NO;

				delete m_pClient[INTERFACE_NETWORK_1];
				m_pClient[INTERFACE_NETWORK_1] = NULL;

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] INTERFACE_NETWORK_1 Client Accept Fail."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
				}

				return 0;
			}

			st_client_info[INTERFACE_NETWORK_1].nConnect = YES;

			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
				clsMem.OnNormalMessageWrite(_T("[TCP/IP] INTERFACE_NETWORK_1 Client Accept Success."));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
			}
			break;
	}

	return 0;
}


LRESULT CMainFrame::OnClientInterface2(WPARAM wParam, LPARAM lParam)
{
	CString strMsg;

	int nLength;

	switch(wParam)
	{
		case CLIENT_CONNECT:
			if (m_pClient[INTERFACE_NETWORK_2] != NULL)
			{
				if (st_client_info[INTERFACE_NETWORK_2].nConnect == YES) 
				{
					st_client_info[INTERFACE_NETWORK_2].nConnect = NO;

					delete m_pClient[INTERFACE_NETWORK_2];
					m_pClient[INTERFACE_NETWORK_2] = NULL;
				}

				m_pClient[INTERFACE_NETWORK_2] = new CClientSocket();
				m_pClient[INTERFACE_NETWORK_2]->Create();
	
				m_pClient[INTERFACE_NETWORK_2]->OnOpen(this, INTERFACE_NETWORK_2);
				if (m_pClient[INTERFACE_NETWORK_2]->Connect(st_client_info[INTERFACE_NETWORK_2].strIp, st_client_info[INTERFACE_NETWORK_2].nPort))
				{
					delete m_pClient[INTERFACE_NETWORK_2];
					m_pClient[INTERFACE_NETWORK_2] = NULL;

					if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
					{
						clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] INTERFACE_NETWORK_2 Client Connect Error."));
						//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
					}

					return 0;
				}

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnNormalMessageWrite(_T("[TCP/IP] INTERFACE_NETWORK_2 Client Connect Success."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			else
			{
				st_client_info[INTERFACE_NETWORK_2].nConnect = NO;

				m_pClient[INTERFACE_NETWORK_2] = new CClientSocket();
				m_pClient[INTERFACE_NETWORK_2]->Create();
	
				m_pClient[INTERFACE_NETWORK_2]->OnOpen(this, INTERFACE_NETWORK_2);
				if (m_pClient[INTERFACE_NETWORK_2]->Connect(st_client_info[INTERFACE_NETWORK_2].strIp, st_client_info[INTERFACE_NETWORK_2].nPort))
				{
					delete m_pClient[INTERFACE_NETWORK_2];
					m_pClient[INTERFACE_NETWORK_2] = NULL;

					if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
					{
						clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] INTERFACE_NETWORK_2 Client Connect Error."));
						//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
					}

					return 0;
				}

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnNormalMessageWrite(_T("[TCP/IP] INTERFACE_NETWORK_2 Client Connect Success."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			break;

		case CLIENT_CLOSE:
			if (m_pClient[INTERFACE_NETWORK_2] != NULL)
			{
				st_client_info[INTERFACE_NETWORK_2].nConnect = NO;

				delete m_pClient[INTERFACE_NETWORK_2];
				m_pClient[INTERFACE_NETWORK_2] = NULL;
			}
			break;

		case CLIENT_SEND:
			if (st_client_info[INTERFACE_NETWORK_2].nConnect == YES)
			{
				if (m_pClient[INTERFACE_NETWORK_2] == NULL) return 0;

				clsFunc.OnStringToChar(st_client_info[INTERFACE_NETWORK_2].strSend, st_client_info[INTERFACE_NETWORK_2].chSend);
				nLength = st_client_info[INTERFACE_NETWORK_2].strSend.GetLength();

				m_pClient[INTERFACE_NETWORK_2]->Send(st_client_info[INTERFACE_NETWORK_2].chSend, nLength);

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					strMsg.Format(_T("[INTERFACE_NETWORK_2] %s"), st_client_info[INTERFACE_NETWORK_2].strSend);
					clsMem.OnNormalMessageWrite(strMsg);
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			break;

		case CLIENT_REV:
			clsInterC2.OnDataReceive(st_client_info[INTERFACE_NETWORK_2].strRev);
			break;

		case CLIENT_ACCEPT:
			if (lParam != 0)
			{
				st_client_info[INTERFACE_NETWORK_2].nConnect = NO;

				delete m_pClient[INTERFACE_NETWORK_2];
				m_pClient[INTERFACE_NETWORK_2] = NULL;

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] INTERFACE_NETWORK_2 Client Accept Fail."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
				}

				return 0;
			}

			st_client_info[INTERFACE_NETWORK_2].nConnect = YES;

			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
				clsMem.OnNormalMessageWrite(_T("[TCP/IP] INTERFACE_NETWORK_2 Client Accept Success."));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
			}
			break;
	}

	return 0;
}


LRESULT CMainFrame::OnClientInterface3(WPARAM wParam, LPARAM lParam)
{
	CString strMsg;

	int nLength;

	switch(wParam)
	{
		case CLIENT_CONNECT:
			if (m_pClient[INTERFACE_NETWORK_3] != NULL)
			{
				if (st_client_info[INTERFACE_NETWORK_3].nConnect == YES) 
				{
					st_client_info[INTERFACE_NETWORK_3].nConnect = NO;

					delete m_pClient[INTERFACE_NETWORK_3];
					m_pClient[INTERFACE_NETWORK_3] = NULL;
				}

				m_pClient[INTERFACE_NETWORK_3] = new CClientSocket();
				m_pClient[INTERFACE_NETWORK_3]->Create();
	
				m_pClient[INTERFACE_NETWORK_3]->OnOpen(this, INTERFACE_NETWORK_3);
				if (m_pClient[INTERFACE_NETWORK_3]->Connect(st_client_info[INTERFACE_NETWORK_3].strIp, st_client_info[INTERFACE_NETWORK_3].nPort))
				{
					delete m_pClient[INTERFACE_NETWORK_3];
					m_pClient[INTERFACE_NETWORK_3] = NULL;

					if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
					{
						clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] INTERFACE_NETWORK_3 Client Connect Error."));
						//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
					}

					return 0;
				}

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnNormalMessageWrite(_T("[TCP/IP] INTERFACE_NETWORK_3 Client Connect Success."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			else
			{
				st_client_info[INTERFACE_NETWORK_3].nConnect = NO;

				m_pClient[INTERFACE_NETWORK_3] = new CClientSocket();
				m_pClient[INTERFACE_NETWORK_3]->Create();
	
				m_pClient[INTERFACE_NETWORK_3]->OnOpen(this, INTERFACE_NETWORK_3);
				if (m_pClient[INTERFACE_NETWORK_3]->Connect(st_client_info[INTERFACE_NETWORK_3].strIp, st_client_info[INTERFACE_NETWORK_3].nPort))
				{
					delete m_pClient[INTERFACE_NETWORK_3];
					m_pClient[INTERFACE_NETWORK_3] = NULL;

					if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
					{
						clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] INTERFACE_NETWORK_3 Client Connect Error."));
						//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
					}

					return 0;
				}

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnNormalMessageWrite(_T("[TCP/IP] INTERFACE_NETWORK_3 Client Connect Success."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			break;

		case CLIENT_CLOSE:
			if (m_pClient[INTERFACE_NETWORK_3] != NULL)
			{
				st_client_info[INTERFACE_NETWORK_3].nConnect = NO;

				delete m_pClient[INTERFACE_NETWORK_3];
				m_pClient[INTERFACE_NETWORK_3] = NULL;
			}
			break;

		case CLIENT_SEND:
			if (st_client_info[INTERFACE_NETWORK_3].nConnect == YES)
			{
				if (m_pClient[INTERFACE_NETWORK_3] == NULL) return 0;

				clsFunc.OnStringToChar(st_client_info[INTERFACE_NETWORK_3].strSend, st_client_info[INTERFACE_NETWORK_3].chSend);
				nLength = st_client_info[INTERFACE_NETWORK_3].strSend.GetLength();

				m_pClient[INTERFACE_NETWORK_3]->Send(st_client_info[INTERFACE_NETWORK_3].chSend, nLength);

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					strMsg.Format(_T("[INTERFACE_NETWORK_3] %s"), st_client_info[INTERFACE_NETWORK_3].strSend);
					clsMem.OnNormalMessageWrite(strMsg);
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			break;

		case CLIENT_REV:
			clsInterC3.OnDataReceive(st_client_info[INTERFACE_NETWORK_3].strRev);
			break;

		case CLIENT_ACCEPT:
			if (lParam != 0)
			{
				st_client_info[INTERFACE_NETWORK_3].nConnect = NO;

				delete m_pClient[INTERFACE_NETWORK_3];
				m_pClient[INTERFACE_NETWORK_3] = NULL;

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] INTERFACE_NETWORK_3 Client Accept Fail."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
				}

				return 0;
			}

			st_client_info[INTERFACE_NETWORK_3].nConnect = YES;

			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
				clsMem.OnNormalMessageWrite(_T("[TCP/IP] INTERFACE_NETWORK_3 Client Accept Success."));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
			}
			break;
	}

	return 0;
}


LRESULT CMainFrame::OnClientInterface4(WPARAM wParam, LPARAM lParam)
{
	CString strMsg;

	int nLength;

	switch(wParam)
	{
		case CLIENT_CONNECT:
			if (m_pClient[INTERFACE_NETWORK_4] != NULL)
			{
				if (st_client_info[INTERFACE_NETWORK_4].nConnect == YES) 
				{
					st_client_info[INTERFACE_NETWORK_4].nConnect = NO;

					delete m_pClient[INTERFACE_NETWORK_4];
					m_pClient[INTERFACE_NETWORK_4] = NULL;
				}

				m_pClient[INTERFACE_NETWORK_4] = new CClientSocket();
				m_pClient[INTERFACE_NETWORK_4]->Create();
	
				m_pClient[INTERFACE_NETWORK_4]->OnOpen(this, INTERFACE_NETWORK_4);
				if (m_pClient[INTERFACE_NETWORK_4]->Connect(st_client_info[INTERFACE_NETWORK_4].strIp, st_client_info[INTERFACE_NETWORK_4].nPort))
				{
					delete m_pClient[INTERFACE_NETWORK_4];
					m_pClient[INTERFACE_NETWORK_4] = NULL;

					if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
					{
						clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] INTERFACE_NETWORK_4 Client Connect Error."));
						//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
					}

					return 0;
				}

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnNormalMessageWrite(_T("[TCP/IP] INTERFACE_NETWORK_4 Client Connect Success."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			else
			{
				st_client_info[INTERFACE_NETWORK_4].nConnect = NO;

				m_pClient[INTERFACE_NETWORK_4] = new CClientSocket();
				m_pClient[INTERFACE_NETWORK_4]->Create();
	
				m_pClient[INTERFACE_NETWORK_4]->OnOpen(this, INTERFACE_NETWORK_4);
				if (m_pClient[INTERFACE_NETWORK_4]->Connect(st_client_info[INTERFACE_NETWORK_4].strIp, st_client_info[INTERFACE_NETWORK_4].nPort))
				{
					delete m_pClient[INTERFACE_NETWORK_4];
					m_pClient[INTERFACE_NETWORK_4] = NULL;

					if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
					{
						clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] INTERFACE_NETWORK_4 Client Connect Error."));
						//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
					}

					return 0;
				}

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnNormalMessageWrite(_T("[TCP/IP] INTERFACE_NETWORK_4 Client Connect Success."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			break;

		case CLIENT_CLOSE:
			if (m_pClient[INTERFACE_NETWORK_4] != NULL)
			{
				st_client_info[INTERFACE_NETWORK_4].nConnect = NO;

				delete m_pClient[INTERFACE_NETWORK_4];
				m_pClient[INTERFACE_NETWORK_4] = NULL;
			}
			break;

		case CLIENT_SEND:
			if (st_client_info[INTERFACE_NETWORK_4].nConnect == YES)
			{
				if (m_pClient[INTERFACE_NETWORK_4] == NULL) return 0;

				clsFunc.OnStringToChar(st_client_info[INTERFACE_NETWORK_4].strSend, st_client_info[INTERFACE_NETWORK_4].chSend);
				nLength = st_client_info[INTERFACE_NETWORK_4].strSend.GetLength();

				m_pClient[INTERFACE_NETWORK_4]->Send(st_client_info[INTERFACE_NETWORK_4].chSend, nLength);

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					strMsg.Format(_T("[INTERFACE_NETWORK_4] %s"), st_client_info[INTERFACE_NETWORK_4].strSend);
					clsMem.OnNormalMessageWrite(strMsg);
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			break;

		case CLIENT_REV:
			clsInterC4.OnDataReceive(st_client_info[INTERFACE_NETWORK_4].strRev);
			break;

		case CLIENT_ACCEPT:
			if (lParam != 0)
			{
				st_client_info[INTERFACE_NETWORK_4].nConnect = NO;

				delete m_pClient[INTERFACE_NETWORK_4];
				m_pClient[INTERFACE_NETWORK_4] = NULL;

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] INTERFACE_NETWORK_4 Client Accept Fail."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
				}

				return 0;
			}

			st_client_info[INTERFACE_NETWORK_4].nConnect = YES;

			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
				clsMem.OnNormalMessageWrite(_T("[TCP/IP] INTERFACE_NETWORK_4 Client Accept Success."));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
			}
			break;
	}

	return 0;
}


LRESULT CMainFrame::OnClientInterface5(WPARAM wParam, LPARAM lParam)
{
	CString strMsg;

	int nLength;

	switch(wParam)
	{
		case CLIENT_CONNECT:
			if (m_pClient[INTERFACE_NETWORK_5] != NULL)
			{
				if (st_client_info[INTERFACE_NETWORK_5].nConnect == YES) 
				{
					st_client_info[INTERFACE_NETWORK_5].nConnect = NO;

					delete m_pClient[INTERFACE_NETWORK_5];
					m_pClient[INTERFACE_NETWORK_5] = NULL;
				}

				m_pClient[INTERFACE_NETWORK_5] = new CClientSocket();
				m_pClient[INTERFACE_NETWORK_5]->Create();
	
				m_pClient[INTERFACE_NETWORK_5]->OnOpen(this, INTERFACE_NETWORK_5);
				if (m_pClient[INTERFACE_NETWORK_5]->Connect(st_client_info[INTERFACE_NETWORK_5].strIp, st_client_info[INTERFACE_NETWORK_5].nPort))
				{
					delete m_pClient[INTERFACE_NETWORK_5];
					m_pClient[INTERFACE_NETWORK_5] = NULL;

					if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
					{
						clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] INTERFACE_NETWORK_5 Client Connect Error."));
						//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
					}

					return 0;
				}

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnNormalMessageWrite(_T("[TCP/IP] INTERFACE_NETWORK_5 Client Connect Success."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			else
			{
				st_client_info[INTERFACE_NETWORK_5].nConnect = NO;

				m_pClient[INTERFACE_NETWORK_5] = new CClientSocket();
				m_pClient[INTERFACE_NETWORK_5]->Create();
	
				m_pClient[INTERFACE_NETWORK_5]->OnOpen(this, INTERFACE_NETWORK_5);
				if (m_pClient[INTERFACE_NETWORK_5]->Connect(st_client_info[INTERFACE_NETWORK_5].strIp, st_client_info[INTERFACE_NETWORK_5].nPort))
				{
					delete m_pClient[INTERFACE_NETWORK_5];
					m_pClient[INTERFACE_NETWORK_5] = NULL;

					if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
					{
						clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] INTERFACE_NETWORK_5 Client Connect Error."));
						//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
					}

					return 0;
				}

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnNormalMessageWrite(_T("[TCP/IP] INTERFACE_NETWORK_5 Client Connect Success."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			break;

		case CLIENT_CLOSE:
			if (m_pClient[INTERFACE_NETWORK_5] != NULL)
			{
				st_client_info[INTERFACE_NETWORK_5].nConnect = NO;

				delete m_pClient[INTERFACE_NETWORK_5];
				m_pClient[INTERFACE_NETWORK_5] = NULL;
			}
			break;

		case CLIENT_SEND:
			if (st_client_info[INTERFACE_NETWORK_5].nConnect == YES)
			{
				if (m_pClient[INTERFACE_NETWORK_5] == NULL) return 0;

				clsFunc.OnStringToChar(st_client_info[INTERFACE_NETWORK_5].strSend, st_client_info[INTERFACE_NETWORK_5].chSend);
				nLength = st_client_info[INTERFACE_NETWORK_5].strSend.GetLength();

				m_pClient[INTERFACE_NETWORK_5]->Send(st_client_info[INTERFACE_NETWORK_5].chSend, nLength);

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					strMsg.Format(_T("[INTERFACE_NETWORK_5] %s"), st_client_info[INTERFACE_NETWORK_5].strSend);
					clsMem.OnNormalMessageWrite(strMsg);
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			break;

		case CLIENT_REV:
			
			clsInterC5.OnDataReceive(st_client_info[INTERFACE_NETWORK_5].strRev);
			break;

		case CLIENT_ACCEPT:
			if (lParam != 0)
			{
				st_client_info[INTERFACE_NETWORK_5].nConnect = NO;

				delete m_pClient[INTERFACE_NETWORK_5];
				m_pClient[INTERFACE_NETWORK_5] = NULL;

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] INTERFACE_NETWORK_5 Client Accept Fail."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
				}

				return 0;
			}

			st_client_info[INTERFACE_NETWORK_5].nConnect = YES;

			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
				clsMem.OnNormalMessageWrite(_T("[TCP/IP] INTERFACE_NETWORK_5 Client Accept Success."));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
			}
			break;
	}

	return 0;
}


LRESULT CMainFrame::OnClientInterface6(WPARAM wParam, LPARAM lParam)
{
	CString strMsg;

	int nLength;

	switch(wParam)
	{
		case CLIENT_CONNECT:
			if (m_pClient[INTERFACE_NETWORK_6] != NULL)
			{
				if (st_client_info[INTERFACE_NETWORK_6].nConnect == YES) 
				{
					st_client_info[INTERFACE_NETWORK_6].nConnect = NO;

					delete m_pClient[INTERFACE_NETWORK_6];
					m_pClient[INTERFACE_NETWORK_6] = NULL;
				}

				m_pClient[INTERFACE_NETWORK_6] = new CClientSocket();
				m_pClient[INTERFACE_NETWORK_6]->Create();
	
				m_pClient[INTERFACE_NETWORK_6]->OnOpen(this, INTERFACE_NETWORK_6);
				if (m_pClient[INTERFACE_NETWORK_6]->Connect(st_client_info[INTERFACE_NETWORK_6].strIp, st_client_info[INTERFACE_NETWORK_6].nPort))
				{
					delete m_pClient[INTERFACE_NETWORK_6];
					m_pClient[INTERFACE_NETWORK_6] = NULL;

					if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
					{
						clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] INTERFACE_NETWORK_6 Client Connect Error."));
						//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
					}

					return 0;
				}

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnNormalMessageWrite(_T("[TCP/IP] INTERFACE_NETWORK_6 Client Connect Success."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			else
			{
				st_client_info[INTERFACE_NETWORK_6].nConnect = NO;

				m_pClient[INTERFACE_NETWORK_6] = new CClientSocket();
				m_pClient[INTERFACE_NETWORK_6]->Create();
	
				m_pClient[INTERFACE_NETWORK_6]->OnOpen(this, INTERFACE_NETWORK_6);
				if (m_pClient[INTERFACE_NETWORK_6]->Connect(st_client_info[INTERFACE_NETWORK_6].strIp, st_client_info[INTERFACE_NETWORK_6].nPort))
				{
					delete m_pClient[INTERFACE_NETWORK_6];
					m_pClient[INTERFACE_NETWORK_6] = NULL;

					if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
					{
						clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] INTERFACE_NETWORK_6 Client Connect Error."));
						//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
					}

					return 0;
				}

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnNormalMessageWrite(_T("[TCP/IP] INTERFACE_NETWORK_6 Client Connect Success."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			break;

		case CLIENT_CLOSE:
			if (m_pClient[INTERFACE_NETWORK_6] != NULL)
			{
				st_client_info[INTERFACE_NETWORK_6].nConnect = NO;

				delete m_pClient[INTERFACE_NETWORK_6];
				m_pClient[INTERFACE_NETWORK_6] = NULL;
			}
			break;

		case CLIENT_SEND:
			if (st_client_info[INTERFACE_NETWORK_6].nConnect == YES)
			{
				if (m_pClient[INTERFACE_NETWORK_6] == NULL) return 0;

				clsFunc.OnStringToChar(st_client_info[INTERFACE_NETWORK_6].strSend, st_client_info[INTERFACE_NETWORK_6].chSend);
				nLength = st_client_info[INTERFACE_NETWORK_6].strSend.GetLength();

				m_pClient[INTERFACE_NETWORK_6]->Send(st_client_info[INTERFACE_NETWORK_6].chSend, nLength);

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					strMsg.Format(_T("[INTERFACE_NETWORK_6] %s"), st_client_info[INTERFACE_NETWORK_6].strSend);
					clsMem.OnNormalMessageWrite(strMsg);
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			break;

		case CLIENT_REV:
			clsInterC6.OnDataReceive(st_client_info[INTERFACE_NETWORK_6].strRev);
			break;

		case CLIENT_ACCEPT:
			if (lParam != 0)
			{
				st_client_info[INTERFACE_NETWORK_6].nConnect = NO;

				delete m_pClient[INTERFACE_NETWORK_6];
				m_pClient[INTERFACE_NETWORK_6] = NULL;

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] INTERFACE_NETWORK_6 Client Accept Fail."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
				}

				return 0;
			}

			st_client_info[INTERFACE_NETWORK_6].nConnect = YES;

			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
				clsMem.OnNormalMessageWrite(_T("[TCP/IP] INTERFACE_NETWORK_6 Client Accept Success."));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
			}
			break;
	}

	return 0;
}


LRESULT CMainFrame::OnClientInterface7(WPARAM wParam, LPARAM lParam)
{
	CString strMsg;

	int nLength;

	switch(wParam)
	{
		case CLIENT_CONNECT:
			if (m_pClient[INTERFACE_NETWORK_7] != NULL)
			{
				if (st_client_info[INTERFACE_NETWORK_7].nConnect == YES) 
				{
					st_client_info[INTERFACE_NETWORK_7].nConnect = NO;

					delete m_pClient[INTERFACE_NETWORK_7];
					m_pClient[INTERFACE_NETWORK_7] = NULL;
				}

				m_pClient[INTERFACE_NETWORK_7] = new CClientSocket();
				m_pClient[INTERFACE_NETWORK_7]->Create();
	
				m_pClient[INTERFACE_NETWORK_7]->OnOpen(this, INTERFACE_NETWORK_7);
				if (m_pClient[INTERFACE_NETWORK_7]->Connect(st_client_info[INTERFACE_NETWORK_7].strIp, st_client_info[INTERFACE_NETWORK_7].nPort))
				{
					delete m_pClient[INTERFACE_NETWORK_7];
					m_pClient[INTERFACE_NETWORK_7] = NULL;

					if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
					{
						clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] INTERFACE_NETWORK_7 Client Connect Error."));
						//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
					}

					return 0;
				}

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnNormalMessageWrite(_T("[TCP/IP] INTERFACE_NETWORK_7 Client Connect Success."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			else
			{
				st_client_info[INTERFACE_NETWORK_7].nConnect = NO;

				m_pClient[INTERFACE_NETWORK_7] = new CClientSocket();
				m_pClient[INTERFACE_NETWORK_7]->Create();
	
				m_pClient[INTERFACE_NETWORK_7]->OnOpen(this, INTERFACE_NETWORK_7);
				if (m_pClient[INTERFACE_NETWORK_7]->Connect(st_client_info[INTERFACE_NETWORK_7].strIp, st_client_info[INTERFACE_NETWORK_7].nPort))
				{
					delete m_pClient[INTERFACE_NETWORK_7];
					m_pClient[INTERFACE_NETWORK_7] = NULL;

					if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
					{
						clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] INTERFACE_NETWORK_7 Client Connect Error."));
						//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
					}

					return 0;
				}

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnNormalMessageWrite(_T("[TCP/IP] INTERFACE_NETWORK_7 Client Connect Success."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			break;

		case CLIENT_CLOSE:
			if (m_pClient[INTERFACE_NETWORK_7] != NULL)
			{
				st_client_info[INTERFACE_NETWORK_7].nConnect = NO;

				delete m_pClient[INTERFACE_NETWORK_7];
				m_pClient[INTERFACE_NETWORK_7] = NULL;
			}
			break;

		case CLIENT_SEND:
			if (st_client_info[INTERFACE_NETWORK_7].nConnect == YES)
			{
				if (m_pClient[INTERFACE_NETWORK_7] == NULL) return 0;

				clsFunc.OnStringToChar(st_client_info[INTERFACE_NETWORK_7].strSend, st_client_info[INTERFACE_NETWORK_7].chSend);
				nLength = st_client_info[INTERFACE_NETWORK_7].strSend.GetLength();

				m_pClient[INTERFACE_NETWORK_7]->Send(st_client_info[INTERFACE_NETWORK_7].chSend, nLength);

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					strMsg.Format(_T("[INTERFACE_NETWORK_7] %s"), st_client_info[INTERFACE_NETWORK_7].strSend);
					clsMem.OnNormalMessageWrite(strMsg);
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			break;

		case CLIENT_REV:
			clsInterC7.OnDataReceive(st_client_info[INTERFACE_NETWORK_7].strRev);
			break;

		case CLIENT_ACCEPT:
			if (lParam != 0)
			{
				st_client_info[INTERFACE_NETWORK_7].nConnect = NO;

				delete m_pClient[INTERFACE_NETWORK_7];
				m_pClient[INTERFACE_NETWORK_7] = NULL;

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] INTERFACE_NETWORK_7 Client Accept Fail."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
				}

				return 0;
			}

			st_client_info[INTERFACE_NETWORK_7].nConnect = YES;

			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
				clsMem.OnNormalMessageWrite(_T("[TCP/IP] INTERFACE_NETWORK_7 Client Accept Success."));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
			}
			break;
	}

	return 0;
}


LRESULT CMainFrame::OnClientInterface8(WPARAM wParam, LPARAM lParam)
{
	CString strMsg;

	int nLength;

	switch(wParam)
	{
		case CLIENT_CONNECT:
			if (m_pClient[INTERFACE_NETWORK_8] != NULL)
			{
				if (st_client_info[INTERFACE_NETWORK_8].nConnect == YES) 
				{
					st_client_info[INTERFACE_NETWORK_8].nConnect = NO;

					delete m_pClient[INTERFACE_NETWORK_8];
					m_pClient[INTERFACE_NETWORK_8] = NULL;
				}

				m_pClient[INTERFACE_NETWORK_8] = new CClientSocket();
				m_pClient[INTERFACE_NETWORK_8]->Create();
	
				m_pClient[INTERFACE_NETWORK_8]->OnOpen(this, INTERFACE_NETWORK_8);
				if (m_pClient[INTERFACE_NETWORK_8]->Connect(st_client_info[INTERFACE_NETWORK_8].strIp, st_client_info[INTERFACE_NETWORK_8].nPort))
				{
					delete m_pClient[INTERFACE_NETWORK_8];
					m_pClient[INTERFACE_NETWORK_8] = NULL;

					if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
					{
						clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] INTERFACE_NETWORK_8 Client Connect Error."));
						//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
					}

					return 0;
				}

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnNormalMessageWrite(_T("[TCP/IP] INTERFACE_NETWORK_8 Client Connect Success."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			else
			{
				st_client_info[INTERFACE_NETWORK_8].nConnect = NO;

				m_pClient[INTERFACE_NETWORK_8] = new CClientSocket();
				m_pClient[INTERFACE_NETWORK_8]->Create();
	
				m_pClient[INTERFACE_NETWORK_8]->OnOpen(this, INTERFACE_NETWORK_8);
				if (m_pClient[INTERFACE_NETWORK_8]->Connect(st_client_info[INTERFACE_NETWORK_8].strIp, st_client_info[INTERFACE_NETWORK_8].nPort))
				{
					delete m_pClient[INTERFACE_NETWORK_8];
					m_pClient[INTERFACE_NETWORK_8] = NULL;

					if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
					{
						clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] INTERFACE_NETWORK_8 Client Connect Error."));
						//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
					}

					return 0;
				}

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnNormalMessageWrite(_T("[TCP/IP] INTERFACE_NETWORK_8 Client Connect Success."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			break;

		case CLIENT_CLOSE:
			if (m_pClient[INTERFACE_NETWORK_8] != NULL)
			{
				st_client_info[INTERFACE_NETWORK_8].nConnect = NO;

				delete m_pClient[INTERFACE_NETWORK_8];
				m_pClient[INTERFACE_NETWORK_8] = NULL;
			}
			break;

		case CLIENT_SEND:
			if (st_client_info[INTERFACE_NETWORK_8].nConnect == YES)
			{
				if (m_pClient[INTERFACE_NETWORK_8] == NULL) return 0;

				clsFunc.OnStringToChar(st_client_info[INTERFACE_NETWORK_8].strSend, st_client_info[INTERFACE_NETWORK_8].chSend);
				nLength = st_client_info[INTERFACE_NETWORK_8].strSend.GetLength();

				m_pClient[INTERFACE_NETWORK_8]->Send(st_client_info[INTERFACE_NETWORK_8].chSend, nLength);

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					strMsg.Format(_T("[INTERFACE_NETWORK_8] %s"), st_client_info[INTERFACE_NETWORK_8].strSend);
					clsMem.OnNormalMessageWrite(strMsg);
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			break;

		case CLIENT_REV:
			clsInterC8.OnDataReceive(st_client_info[INTERFACE_NETWORK_8].strRev);
			break;

		case CLIENT_ACCEPT:
			if (lParam != 0)
			{
				st_client_info[INTERFACE_NETWORK_8].nConnect = NO;

				delete m_pClient[INTERFACE_NETWORK_8];
				m_pClient[INTERFACE_NETWORK_8] = NULL;

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] INTERFACE_NETWORK_8 Client Accept Fail."));
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
				}

				return 0;
			}

			st_client_info[INTERFACE_NETWORK_8].nConnect = YES;

			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
				clsMem.OnNormalMessageWrite(_T("[TCP/IP] INTERFACE_NETWORK_8 Client Accept Success."));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
			}
			break;
	}

	return 0;
}


LRESULT CMainFrame::OnServerInterface1(WPARAM wParam, LPARAM lParam)
{
	int i;
	int nLength;

	CString strMsg;

	switch(wParam)
	{
		case SERVER_CONNECT:
			if (m_pServer[INTERFACE_NETWORK_1] != NULL)
			{
				m_pServer[INTERFACE_NETWORK_1]->OnClose();

				delete m_pServer[INTERFACE_NETWORK_1];
				m_pServer[INTERFACE_NETWORK_1] = NULL;
			}

			// sevrer 생성
			m_pServer[INTERFACE_NETWORK_1] = new CServerSocket;
	
			// Tcp/IP server open
			if (m_pServer[INTERFACE_NETWORK_1]->OnOpen(this, st_server_info[INTERFACE_NETWORK_1].nPort, INTERFACE_NETWORK_1))
			{
				clsMem.OnNormalMessageWrite(_T("[TCP/IP] INTERFACE_NETWORK_1 Server Open Success."));
			}
			else
			{
				clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] INTERFACE_NETWORK_1 Server Open Error."));
					
				delete m_pServer[INTERFACE_NETWORK_1];
				m_pServer[INTERFACE_NETWORK_1] = NULL;
			}
			break;

		case SERVER_CLOSE:
			if (m_pServer[INTERFACE_NETWORK_1] != NULL)
			{
				m_pServer[INTERFACE_NETWORK_1]->OnCloseClient(lParam);
			}
			break;

		case SERVER_REV:
			clsInterS1.OnDataReceive(st_server_info[INTERFACE_NETWORK_1].strRev);
			break;

		case SERVER_SEND:
			for (i=0; i<MAX_USER; i++)
			{
				if (m_pServer[INTERFACE_NETWORK_1] != NULL)
				{
					// client가 server에 접속중이면 
					if (m_pServer[INTERFACE_NETWORK_1]->m_pClient[i] != NULL)
					{
						// message 전송
						//m_pServer[RFID_NETWORK]->m_pClient[i]->Send(st_server_info[RFID_NETWORK].strSend, st_server_info[RFID_NETWORK].strSend.GetLength() * 2, 0);
						clsFunc.OnStringToChar(st_server_info[INTERFACE_NETWORK_1].strSend, st_server_info[INTERFACE_NETWORK_1].chSend);
						nLength = st_server_info[INTERFACE_NETWORK_1].strSend.GetLength();

						m_pServer[INTERFACE_NETWORK_1]->m_pClient[i]->Send(st_server_info[INTERFACE_NETWORK_1].chSend, nLength);

						strMsg.Format(_T("[INTERFACE_SERVER_1] %s"), st_server_info[INTERFACE_NETWORK_1].strSend);
						clsMem.OnNormalMessageWrite(strMsg);
					}
				}
			}
			break;
	}

	return 0;
}


LRESULT CMainFrame::OnServerInterface2(WPARAM wParam, LPARAM lParam)
{
	int i;
	int nLength;

	CString strMsg;

	switch(wParam)
	{
		case SERVER_CONNECT:
			if (m_pServer[INTERFACE_NETWORK_2] != NULL)
			{
				m_pServer[INTERFACE_NETWORK_2]->OnClose();

				delete m_pServer[INTERFACE_NETWORK_2];
				m_pServer[INTERFACE_NETWORK_2] = NULL;
			}

			// sevrer 생성
			m_pServer[INTERFACE_NETWORK_2] = new CServerSocket;
	
			// Tcp/IP server open
			if (m_pServer[INTERFACE_NETWORK_2]->OnOpen(this, st_server_info[INTERFACE_NETWORK_2].nPort, INTERFACE_NETWORK_2))
			{
				clsMem.OnNormalMessageWrite(_T("[TCP/IP] INTERFACE_NETWORK_2 Server Open Success."));
			}
			else
			{
				clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] INTERFACE_NETWORK_2 Server Open Error."));
					
				delete m_pServer[INTERFACE_NETWORK_2];
				m_pServer[INTERFACE_NETWORK_2] = NULL;
			}
			break;

		case SERVER_CLOSE:
			if (m_pServer[INTERFACE_NETWORK_2] != NULL)
			{
				m_pServer[INTERFACE_NETWORK_2]->OnCloseClient(lParam);
			}
			break;

		case SERVER_REV:
			clsInterS2.OnDataReceive(st_server_info[INTERFACE_NETWORK_2].strRev);
			break;

		case SERVER_SEND:
			for (i=0; i<MAX_USER; i++)
			{
				if (m_pServer[INTERFACE_NETWORK_2] != NULL)
				{
					// client가 server에 접속중이면 
					if (m_pServer[INTERFACE_NETWORK_2]->m_pClient[i] != NULL)
					{
						// message 전송
						//m_pServer[RFID_NETWORK]->m_pClient[i]->Send(st_server_info[RFID_NETWORK].strSend, st_server_info[RFID_NETWORK].strSend.GetLength() * 2, 0);
						clsFunc.OnStringToChar(st_server_info[INTERFACE_NETWORK_2].strSend, st_server_info[INTERFACE_NETWORK_2].chSend);
						nLength = st_server_info[INTERFACE_NETWORK_2].strSend.GetLength();

						m_pServer[INTERFACE_NETWORK_2]->m_pClient[i]->Send(st_server_info[INTERFACE_NETWORK_2].chSend, nLength);

						strMsg.Format(_T("[INTERFACE_SERVER_2] %s"), st_server_info[INTERFACE_NETWORK_2].strSend);
						clsMem.OnNormalMessageWrite(strMsg);
					}
				}
			}
			break;
	}

	return 0;
}


LRESULT CMainFrame::OnServerInterface3(WPARAM wParam, LPARAM lParam)
{
	int i;
	int nLength;

	CString strMsg;

	switch(wParam)
	{
		case SERVER_CONNECT:
			if (m_pServer[INTERFACE_NETWORK_3] != NULL)
			{
				m_pServer[INTERFACE_NETWORK_3]->OnClose();

				delete m_pServer[INTERFACE_NETWORK_3];
				m_pServer[INTERFACE_NETWORK_3] = NULL;
			}

			// sevrer 생성
			m_pServer[INTERFACE_NETWORK_3] = new CServerSocket;
	
			// Tcp/IP server open
			if (m_pServer[INTERFACE_NETWORK_3]->OnOpen(this, st_server_info[INTERFACE_NETWORK_3].nPort, INTERFACE_NETWORK_3))
			{
				clsMem.OnNormalMessageWrite(_T("[TCP/IP] INTERFACE_NETWORK_3 Server Open Success."));
			}
			else
			{
				clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] INTERFACE_NETWORK_3 Server Open Error."));
					
				delete m_pServer[INTERFACE_NETWORK_3];
				m_pServer[INTERFACE_NETWORK_3] = NULL;
			}
			break;

		case SERVER_CLOSE:
			if (m_pServer[INTERFACE_NETWORK_3] != NULL)
			{
				m_pServer[INTERFACE_NETWORK_3]->OnCloseClient(lParam);
			}
			break;

		case SERVER_REV:
			clsInterS3.OnDataReceive(st_server_info[INTERFACE_NETWORK_3].strRev);
			break;

		case SERVER_SEND:
			for (i=0; i<MAX_USER; i++)
			{
				if (m_pServer[INTERFACE_NETWORK_3] != NULL)
				{
					// client가 server에 접속중이면 
					if (m_pServer[INTERFACE_NETWORK_3]->m_pClient[i] != NULL)
					{
						// message 전송
						//m_pServer[RFID_NETWORK]->m_pClient[i]->Send(st_server_info[RFID_NETWORK].strSend, st_server_info[RFID_NETWORK].strSend.GetLength() * 2, 0);
						clsFunc.OnStringToChar(st_server_info[INTERFACE_NETWORK_3].strSend, st_server_info[INTERFACE_NETWORK_3].chSend);
						nLength = st_server_info[INTERFACE_NETWORK_3].strSend.GetLength();

						m_pServer[INTERFACE_NETWORK_3]->m_pClient[i]->Send(st_server_info[INTERFACE_NETWORK_3].chSend, nLength);

						strMsg.Format(_T("[INTERFACE_SERVER_3] %s"), st_server_info[INTERFACE_NETWORK_3].strSend);
						clsMem.OnNormalMessageWrite(strMsg);
					}
				}
			}
			break;
	}

	return 0;
}


LRESULT CMainFrame::OnServerInterface4(WPARAM wParam, LPARAM lParam)
{
	int i;
	int nLength;

	CString strMsg;

	switch(wParam)
	{
		case SERVER_CONNECT:
			if (m_pServer[INTERFACE_NETWORK_4] != NULL)
			{
				m_pServer[INTERFACE_NETWORK_4]->OnClose();

				delete m_pServer[INTERFACE_NETWORK_4];
				m_pServer[INTERFACE_NETWORK_4] = NULL;
			}

			// sevrer 생성
			m_pServer[INTERFACE_NETWORK_4] = new CServerSocket;
	
			// Tcp/IP server open
			if (m_pServer[INTERFACE_NETWORK_4]->OnOpen(this, st_server_info[INTERFACE_NETWORK_4].nPort, INTERFACE_NETWORK_4))
			{
				clsMem.OnNormalMessageWrite(_T("[TCP/IP] INTERFACE_NETWORK_4 Server Open Success."));
			}
			else
			{
				clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] INTERFACE_NETWORK_4 Server Open Error."));
					
				delete m_pServer[INTERFACE_NETWORK_4];
				m_pServer[INTERFACE_NETWORK_4] = NULL;
			}
			break;

		case SERVER_CLOSE:
			if (m_pServer[INTERFACE_NETWORK_4] != NULL)
			{
				m_pServer[INTERFACE_NETWORK_4]->OnCloseClient(lParam);
			}
			break;

		case SERVER_REV:
			clsInterS4.OnDataReceive(st_server_info[INTERFACE_NETWORK_4].strRev);
			break;

		case SERVER_SEND:
			for (i=0; i<MAX_USER; i++)
			{
				if (m_pServer[INTERFACE_NETWORK_4] != NULL)
				{
					// client가 server에 접속중이면 
					if (m_pServer[INTERFACE_NETWORK_4]->m_pClient[i] != NULL)
					{
						// message 전송
						//m_pServer[RFID_NETWORK]->m_pClient[i]->Send(st_server_info[RFID_NETWORK].strSend, st_server_info[RFID_NETWORK].strSend.GetLength() * 2, 0);
						clsFunc.OnStringToChar(st_server_info[INTERFACE_NETWORK_4].strSend, st_server_info[INTERFACE_NETWORK_4].chSend);
						nLength = st_server_info[INTERFACE_NETWORK_4].strSend.GetLength();

						m_pServer[INTERFACE_NETWORK_4]->m_pClient[i]->Send(st_server_info[INTERFACE_NETWORK_4].chSend, nLength);

						strMsg.Format(_T("[INTERFACE_SERVER_4] %s"), st_server_info[INTERFACE_NETWORK_4].strSend);
						clsMem.OnNormalMessageWrite(strMsg);
					}
				}
			}
			break;
	}

	return 0;
}


LRESULT CMainFrame::OnServerInterface5(WPARAM wParam, LPARAM lParam)
{
	int i;
	int nLength;

	CString strMsg;

	switch(wParam)
	{
		case SERVER_CONNECT:
			if (m_pServer[INTERFACE_NETWORK_5] != NULL)
			{
				m_pServer[INTERFACE_NETWORK_5]->OnClose();

				delete m_pServer[INTERFACE_NETWORK_5];
				m_pServer[INTERFACE_NETWORK_5] = NULL;
			}

			// sevrer 생성
			m_pServer[INTERFACE_NETWORK_5] = new CServerSocket;
	
			// Tcp/IP server open
			if (m_pServer[INTERFACE_NETWORK_5]->OnOpen(this, st_server_info[INTERFACE_NETWORK_5].nPort, INTERFACE_NETWORK_5))
			{
				clsMem.OnNormalMessageWrite(_T("[TCP/IP] INTERFACE_NETWORK_5 Server Open Success."));
			}
			else
			{
				clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] INTERFACE_NETWORK_5 Server Open Error."));
					
				delete m_pServer[INTERFACE_NETWORK_5];
				m_pServer[INTERFACE_NETWORK_5] = NULL;
			}
			break;

		case SERVER_CLOSE:
			if (m_pServer[INTERFACE_NETWORK_5] != NULL)
			{
				m_pServer[INTERFACE_NETWORK_5]->OnCloseClient(lParam);
			}
			break;

		case SERVER_REV:
			clsInterS5.OnDataReceive(st_server_info[INTERFACE_NETWORK_5].strRev);
			break;

		case SERVER_SEND:
			for (i=0; i<MAX_USER; i++)
			{
				if (m_pServer[INTERFACE_NETWORK_5] != NULL)
				{
					// client가 server에 접속중이면 
					if (m_pServer[INTERFACE_NETWORK_5]->m_pClient[i] != NULL)
					{
						// message 전송
						//m_pServer[RFID_NETWORK]->m_pClient[i]->Send(st_server_info[RFID_NETWORK].strSend, st_server_info[RFID_NETWORK].strSend.GetLength() * 2, 0);
						clsFunc.OnStringToChar(st_server_info[INTERFACE_NETWORK_5].strSend, st_server_info[INTERFACE_NETWORK_5].chSend);
						nLength = st_server_info[INTERFACE_NETWORK_5].strSend.GetLength();

						m_pServer[INTERFACE_NETWORK_5]->m_pClient[i]->Send(st_server_info[INTERFACE_NETWORK_5].chSend, nLength);

						strMsg.Format(_T("[INTERFACE_SERVER_5] %s"), st_server_info[INTERFACE_NETWORK_5].strSend);
						clsMem.OnNormalMessageWrite(strMsg);
					}
				}
			}
			break;
	}

	return 0;
}


LRESULT CMainFrame::OnServerInterface6(WPARAM wParam, LPARAM lParam)
{
	int i;
	int nLength;

	CString strMsg;

	switch(wParam)
	{
		case SERVER_CONNECT:
			if (m_pServer[INTERFACE_NETWORK_6] != NULL)
			{
				m_pServer[INTERFACE_NETWORK_6]->OnClose();

				delete m_pServer[INTERFACE_NETWORK_6];
				m_pServer[INTERFACE_NETWORK_6] = NULL;
			}

			// sevrer 생성
			m_pServer[INTERFACE_NETWORK_6] = new CServerSocket;
	
			// Tcp/IP server open
			if (m_pServer[INTERFACE_NETWORK_6]->OnOpen(this, st_server_info[INTERFACE_NETWORK_6].nPort, INTERFACE_NETWORK_6))
			{
				clsMem.OnNormalMessageWrite(_T("[TCP/IP] INTERFACE_NETWORK_6 Server Open Success."));
			}
			else
			{
				clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] INTERFACE_NETWORK_6 Server Open Error."));
					
				delete m_pServer[INTERFACE_NETWORK_6];
				m_pServer[INTERFACE_NETWORK_6] = NULL;
			}
			break;

		case SERVER_CLOSE:
			if (m_pServer[INTERFACE_NETWORK_6] != NULL)
			{
				m_pServer[INTERFACE_NETWORK_6]->OnCloseClient(lParam);
			}
			break;

		case SERVER_REV:
			clsInterS6.OnDataReceive(st_server_info[INTERFACE_NETWORK_6].strRev);
			break;

		case SERVER_SEND:
			for (i=0; i<MAX_USER; i++)
			{
				if (m_pServer[INTERFACE_NETWORK_6] != NULL)
				{
					// client가 server에 접속중이면 
					if (m_pServer[INTERFACE_NETWORK_6]->m_pClient[i] != NULL)
					{
						// message 전송
						//m_pServer[RFID_NETWORK]->m_pClient[i]->Send(st_server_info[RFID_NETWORK].strSend, st_server_info[RFID_NETWORK].strSend.GetLength() * 2, 0);
						clsFunc.OnStringToChar(st_server_info[INTERFACE_NETWORK_6].strSend, st_server_info[INTERFACE_NETWORK_6].chSend);
						nLength = st_server_info[INTERFACE_NETWORK_6].strSend.GetLength();

						m_pServer[INTERFACE_NETWORK_6]->m_pClient[i]->Send(st_server_info[INTERFACE_NETWORK_6].chSend, nLength);

						strMsg.Format(_T("[INTERFACE_SERVER_6] %s"), st_server_info[INTERFACE_NETWORK_6].strSend);
						clsMem.OnNormalMessageWrite(strMsg);
					}
				}
			}
			break;
	}

	return 0;
}


LRESULT CMainFrame::OnServerInterface7(WPARAM wParam, LPARAM lParam)
{
	int i;
	int nLength;

	CString strMsg;

	switch(wParam)
	{
		case SERVER_CONNECT:
			if (m_pServer[INTERFACE_NETWORK_7] != NULL)
			{
				m_pServer[INTERFACE_NETWORK_7]->OnClose();

				delete m_pServer[INTERFACE_NETWORK_7];
				m_pServer[INTERFACE_NETWORK_7] = NULL;
			}

			// sevrer 생성
			m_pServer[INTERFACE_NETWORK_7] = new CServerSocket;
	
			// Tcp/IP server open
			if (m_pServer[INTERFACE_NETWORK_7]->OnOpen(this, st_server_info[INTERFACE_NETWORK_7].nPort, INTERFACE_NETWORK_7))
			{
				clsMem.OnNormalMessageWrite(_T("[TCP/IP] INTERFACE_NETWORK_7 Server Open Success."));
			}
			else
			{
				clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] INTERFACE_NETWORK_7 Server Open Error."));
					
				delete m_pServer[INTERFACE_NETWORK_7];
				m_pServer[INTERFACE_NETWORK_7] = NULL;
			}
			break;

		case SERVER_CLOSE:
			if (m_pServer[INTERFACE_NETWORK_7] != NULL)
			{
				m_pServer[INTERFACE_NETWORK_7]->OnCloseClient(lParam);
			}
			break;

		case SERVER_REV:
			clsInterS7.OnDataReceive(st_server_info[INTERFACE_NETWORK_7].strRev);
			break;

		case SERVER_SEND:
			for (i=0; i<MAX_USER; i++)
			{
				if (m_pServer[INTERFACE_NETWORK_7] != NULL)
				{
					// client가 server에 접속중이면 
					if (m_pServer[INTERFACE_NETWORK_7]->m_pClient[i] != NULL)
					{
						// message 전송
						//m_pServer[RFID_NETWORK]->m_pClient[i]->Send(st_server_info[RFID_NETWORK].strSend, st_server_info[RFID_NETWORK].strSend.GetLength() * 2, 0);
						clsFunc.OnStringToChar(st_server_info[INTERFACE_NETWORK_7].strSend, st_server_info[INTERFACE_NETWORK_7].chSend);
						nLength = st_server_info[INTERFACE_NETWORK_7].strSend.GetLength();

						m_pServer[INTERFACE_NETWORK_7]->m_pClient[i]->Send(st_server_info[INTERFACE_NETWORK_7].chSend, nLength);

						strMsg.Format(_T("[INTERFACE_SERVER_7] %s"), st_server_info[INTERFACE_NETWORK_7].strSend);
						clsMem.OnNormalMessageWrite(strMsg);
					}
				}
			}
			break;
	}

	return 0;
}


LRESULT CMainFrame::OnServerInterface8(WPARAM wParam, LPARAM lParam)
{
	int i;
	int nLength;

	CString strMsg;

	switch(wParam)
	{
		case SERVER_CONNECT:
			if (m_pServer[INTERFACE_NETWORK_8] != NULL)
			{
				m_pServer[INTERFACE_NETWORK_8]->OnClose();

				delete m_pServer[INTERFACE_NETWORK_8];
				m_pServer[INTERFACE_NETWORK_8] = NULL;
			}

			// sevrer 생성
			m_pServer[INTERFACE_NETWORK_8] = new CServerSocket;
	
			// Tcp/IP server open
			if (m_pServer[INTERFACE_NETWORK_8]->OnOpen(this, st_server_info[INTERFACE_NETWORK_8].nPort, INTERFACE_NETWORK_8))
			{
				clsMem.OnNormalMessageWrite(_T("[TCP/IP] INTERFACE_NETWORK_8 Server Open Success."));
			}
			else
			{
				clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] INTERFACE_NETWORK_8 Server Open Error."));
					
				delete m_pServer[INTERFACE_NETWORK_8];
				m_pServer[INTERFACE_NETWORK_8] = NULL;
			}
			break;

		case SERVER_CLOSE:
			if (m_pServer[INTERFACE_NETWORK_8] != NULL)
			{
				m_pServer[INTERFACE_NETWORK_8]->OnCloseClient(lParam);
			}
			break;

		case SERVER_REV:
			clsInterS8.OnDataReceive(st_server_info[INTERFACE_NETWORK_8].strRev);
			break;

		case SERVER_SEND:
			for (i=0; i<MAX_USER; i++)
			{
				if (m_pServer[INTERFACE_NETWORK_8] != NULL)
				{
					// client가 server에 접속중이면 
					if (m_pServer[INTERFACE_NETWORK_8]->m_pClient[i] != NULL)
					{
						// message 전송
						//m_pServer[RFID_NETWORK]->m_pClient[i]->Send(st_server_info[RFID_NETWORK].strSend, st_server_info[RFID_NETWORK].strSend.GetLength() * 2, 0);
						clsFunc.OnStringToChar(st_server_info[INTERFACE_NETWORK_8].strSend, st_server_info[INTERFACE_NETWORK_8].chSend);
						nLength = st_server_info[INTERFACE_NETWORK_8].strSend.GetLength();

						m_pServer[INTERFACE_NETWORK_8]->m_pClient[i]->Send(st_server_info[INTERFACE_NETWORK_8].chSend, nLength);

						strMsg.Format(_T("[INTERFACE_SERVER_8] %s"), st_server_info[INTERFACE_NETWORK_8].strSend);
						clsMem.OnNormalMessageWrite(strMsg);
					}
				}
			}
			break;
	}

	return 0;
}

/*
LRESULT CMainFrame::OnClientRfid(WPARAM wParam, LPARAM lParam)
{
	CString strMsg;

	int nLength;

	switch(wParam)
	{
		case CLIENT_CONNECT:
			if (m_pClient[RFID_NETWORK] != NULL)
			{
				//if (st_client_info[RFID_NETWORK].nConnect == YES) m_pClient[RFID_NETWORK]->Close();

				st_client_info[RFID_NETWORK].nConnect = NO;

				delete m_pClient[RFID_NETWORK];
				m_pClient[RFID_NETWORK] = NULL;

				m_pClient[RFID_NETWORK] = new CClientSocket();
				m_pClient[RFID_NETWORK]->Create();
	
				m_pClient[RFID_NETWORK]->OnOpen(this, RFID_NETWORK);
				if (m_pClient[RFID_NETWORK]->Connect(st_client_info[RFID_NETWORK].strIp, st_client_info[RFID_NETWORK].nPort))
				{
					delete m_pClient[RFID_NETWORK];
					m_pClient[RFID_NETWORK] = NULL;

					if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
					{
						//clsFunc.OnStringToChar(_T("[TCP/IP] RFID Client Connect Error."), st_other_info.cAbnormalMsg);
						clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] RFID_NETWORK Client Connect Error."));
						st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
					}

					return 0;
				}

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					//clsFunc.OnStringToChar(_T("[TCP/IP] RFID Client Connect Success."), st_other_info.cNormalMsg);
					clsMem.OnNormalMessageWrite(_T("[TCP/IP] RFID_NETWORK Client Connect Success."));
					st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			else
			{
				st_client_info[RFID_NETWORK].nConnect = NO;

				m_pClient[RFID_NETWORK] = new CClientSocket();
				m_pClient[RFID_NETWORK]->Create();
	
				m_pClient[RFID_NETWORK]->OnOpen(this, RFID_NETWORK);
				if (m_pClient[RFID_NETWORK]->Connect(st_client_info[RFID_NETWORK].strIp, st_client_info[RFID_NETWORK].nPort))
				{
					delete m_pClient[RFID_NETWORK];
					m_pClient[RFID_NETWORK] = NULL;

					if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
					{
						//clsFunc.OnStringToChar(_T("[TCP/IP] RFID Client Connect Error."), st_other_info.cAbnormalMsg);
						clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] RFID_NETWORK Client Connect Error."));
						st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
					}
					return 0;
				}

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					//clsFunc.OnStringToChar(_T("[TCP/IP] RFID Client Connect Success."), st_other_info.cNormalMsg);
					clsMem.OnNormalMessageWrite(_T("[TCP/IP] RFID_NETWORK Client Connect Success."));
					st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			break;

		case CLIENT_CLOSE:
			if (m_pClient[RFID_NETWORK] != NULL)
			{
				//if (st_client_info[RFID_NETWORK].nConnect == YES) m_pClient[RFID_NETWORK]->Close();

				st_client_info[RFID_NETWORK].nConnect = NO;

				delete m_pClient[RFID_NETWORK];
				m_pClient[RFID_NETWORK] = NULL;
			}
			break;

		case CLIENT_SEND:
			if (st_client_info[RFID_NETWORK].nConnect == YES)
			{
				if (m_pClient[RFID_NETWORK] == NULL) return 0;

				clsFunc.OnStringToChar(st_client_info[RFID_NETWORK].strSend, st_client_info[RFID_NETWORK].chSend);
				nLength = st_client_info[RFID_NETWORK].strSend.GetLength();

				m_pClient[RFID_NETWORK]->Send(st_client_info[RFID_NETWORK].chSend, nLength);

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					strMsg.Format(_T("[RFID_NETWORK] %s"), st_client_info[RFID_NETWORK].strSend);
					//clsFunc.OnStringToChar(strMsg, st_other_info.cNormalMsg);
					clsMem.OnNormalMessageWrite(strMsg);
					st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
				}
			}
			break;

		case CLIENT_REV:
			clsRfid.OnDataReceive(st_client_info[RFID_NETWORK].strRev);
			break;

		case CLIENT_ACCEPT:
			if (lParam != 0)
			{
				st_client_info[RFID_NETWORK].nConnect = NO;

				delete m_pClient[RFID_NETWORK];
				m_pClient[RFID_NETWORK] = NULL;

				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					//clsFunc.OnStringToChar(_T("[TCP/IP] RFID Client Accept Fail."), st_other_info.cAbnormalMsg);
					clsMem.OnAbNormalMessagWrite(_T("[TCP/IP] RFID_NETWORK Client Accept Fail."));
					st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력
				}
				return 0;
			}

			st_client_info[RFID_NETWORK].nConnect = YES;

			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
				//clsFunc.OnStringToChar(_T("[TCP/IP] RFID Client Accept Success."), st_other_info.cNormalMsg);
				clsMem.OnNormalMessageWrite(_T("[TCP/IP] RFID_NETWORK Client Accept Success."));
				st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
			}
			break;
	}

	return 0;
}
*/



LRESULT CMainFrame::OnClientFtp(WPARAM wParam, LPARAM lParam)
{
	BOOL bRet;

	if (wParam == 0)
	{
		bRet = m_pFtp.OnScrapCodeDownload(lParam, st_path_info.strPathFtpGms, st_basic_info.strScrapName);

		if (bRet)
		{
			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
				clsMem.OnNormalMessageWrite(_T("[FTP] Scrap Code Download Success."));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
			}

			// jtkim 20150719
			st_work_info.nFtpWork = 1;
			if (st_handler_info.cWndTitle != NULL)
			{
		//		st_handler_info.cWndTitle->PostMessage(WM_STATUS_CHANGE, INTERFACE_MODE, st_basic_info.nModeInterface);
				st_handler_info.cWndTitle->PostMessage(WM_STATUS_CHANGE, INTERFACE_MODE, st_handler_info.nFtpVer);
			}
			st_code_info[lParam].m_nScrapBin = BD_DATA_GOOD;
		}
		else
		{
			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
				clsMem.OnNormalMessageWrite(_T("[FTP] Scrap Code Download Fail."));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
			}

			// jtkim 20150719
			st_work_info.nFtpWork = 0;
			if (st_handler_info.cWndTitle != NULL)
			{
		//		st_handler_info.cWndTitle->PostMessage(WM_STATUS_CHANGE, INTERFACE_MODE, st_basic_info.nModeInterface);
				st_handler_info.cWndTitle->PostMessage(WM_STATUS_CHANGE, INTERFACE_MODE, st_handler_info.nFtpVer);
			}
			st_code_info[lParam].m_nScrapBin = BD_DATA_REJECT;
		}
	}
	else
	{
		bRet = m_pFtp.OnRecipeDownload(st_path_info.strPathFtpGms);

		if (bRet)
		{
			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
				clsMem.OnNormalMessageWrite(_T("[FTP] Recipe Download Success."));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
			}

			// jtkim 20150719
			st_work_info.nFtpWork = 1;
			if (st_handler_info.cWndTitle != NULL)
			{
		//		st_handler_info.cWndTitle->PostMessage(WM_STATUS_CHANGE, INTERFACE_MODE, st_basic_info.nModeInterface);
				st_handler_info.cWndTitle->PostMessage(WM_STATUS_CHANGE, INTERFACE_MODE, st_handler_info.nFtpVer);
			}
			st_code_info[lParam].m_nScrapBin = BD_DATA_GOOD;
		}
		else
		{
			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
				clsMem.OnNormalMessageWrite(_T("[FTP] Recipe Download Fail."));
				//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력
			}

			// jtkim 20150719
			st_work_info.nFtpWork = 0;
			if (st_handler_info.cWndTitle != NULL)
			{
		//		st_handler_info.cWndTitle->PostMessage(WM_STATUS_CHANGE, INTERFACE_MODE, st_basic_info.nModeInterface);
				st_handler_info.cWndTitle->PostMessage(WM_STATUS_CHANGE, INTERFACE_MODE, st_handler_info.nFtpVer);
			}
			st_code_info[lParam].m_nScrapBin = BD_DATA_REJECT;
		}
	}

	return 0;
}

void CMainFrame::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//return ;

	CFrameWnd::OnNcLButtonDblClk(nHitTest, point);
}


void CMainFrame::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//return ;

	CFrameWnd::OnNcLButtonDown(nHitTest, point);
}


int CMainFrame::OnXgemInterface()
{
	switch (m_nXgemStep)
	{
		case 0:
			// 작업중인 모드체크
			if (st_basic_info.nModeXgem == YES)
			{
				// 작업모드가 xgem 모드이면
				m_nXgemStep = 100;
			}
			break;

		case 100:
			// client 연결 요청 
			m_dwXgemTime[0] = GetTickCount();
			::SendMessage(st_handler_info.hWnd, WM_CLIENT_MSG + XGEM_NETWORK, CLIENT_CONNECT, 0);

			m_nXgemStep = 200;
			break;

		case 200:
			// client 연결상태 체크
			if (st_client_info[XGEM_NETWORK].nConnect == YES)
			{
				// 현재 연결중이면 데이터 전송으로 
				KillTimer(TM_XGEM);
				SetTimer(TM_XGEM, 100, NULL);

				m_nXgemStep = 300;
			}
			else 
			{
				m_dwXgemTime[1] = GetTickCount();
				m_dwXgemTime[2] = m_dwXgemTime[1] - m_dwXgemTime[0];

				if (m_dwXgemTime[2] <= (DWORD)0)
				{
					m_dwXgemTime[0] = GetTickCount();
					break;
				}

				if (m_dwXgemTime[2] > 5000)
				{
					m_nXgemStep = 0;

				}
			}
			break;

		case 300:
			m_dwXgemTime[0] = GetTickCount();
			clsXgem.OnXgemInitialize(_T("C:\\XGEM\\EqSample.cfg"));

			m_nXgemStep = 400;
			break;

		case 400:
			m_dwXgemTime[1] = GetTickCount();
			m_dwXgemTime[2] = m_dwXgemTime[1] - m_dwXgemTime[0];

			if (m_dwXgemTime[2] <= (DWORD)0)
			{
				m_dwXgemTime[0] = GetTickCount();
				break;
			}

			if (m_dwXgemTime[2] > 1000)
			{
				m_nXgemStep = 500;

			}
			break;

		case 500:
			clsXgem.OnMcInterface(st_basic_info.nModeXgemInterface);

			m_nXgemStep = 0;
			return RET_GOOD;

			break;
	}

	return RET_PROCEED;
}


LRESULT CMainFrame::OnXgemCommand(WPARAM wParam, LPARAM lParam)
{
	if (wParam == XGEM_MODE)
	{
		if (st_basic_info.nModeXgem == YES)
		{
			m_nXgemStep = 0;
			SetTimer(TM_XGEM, 5000, NULL);
		}
		else
		{
			KillTimer(TM_XGEM);
		}
	}

	return 0;
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다
	if( pMsg->message == WM_KEYDOWN) 
	{
/*		if( pMsg->wParam == 'T')
		{
			if (st_handler_info.cWndMain != NULL)
			{
				st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_PCB_INFO, 0);
			}
		}
		else if( pMsg->wParam == 'S')
		{
			if (st_handler_info.cWndMain != NULL)
			{
				st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_PCB_STATUS, 0);
			}
		}
		else if( pMsg->wParam == 'N')
		{
			if (st_handler_info.cWndMain != NULL)
			{
				st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_PCB_INFO_NEXT, 0);
			}
		}
		else if( pMsg->wParam == 'B')
		{
			if (st_handler_info.cWndMain != NULL)
			{
				st_handler_info.cWndMain->SendMessage(WM_WORK_COMMAND, MAIN_PCB_INFO_BACK, 0);
			}
		}*/
	}
	return CFrameWnd::PreTranslateMessage(pMsg);
}


void CMainFrame::OnProductData()
{
	CString	strTable[14];

	int i, j;

	for(i = 0; i < 8; i++)
	{
		for (j=0; j<8; j++)
		{
			if (st_test_site_info[i].st_pcb_info[j].nEnable == NO)
			{
				st_product_info.nSocketOff++;
			}
		}
	}

	strTable[2].Format(_T("%d"), st_product_info.nUph);
	st_product_info.nUph = 0;

	strTable[3].Format(_T("%d"), st_product_info.nSocketOff);
	st_product_info.nSocketOff = 0;

	strTable[4].Format(_T("%d"), st_product_info.nCok);
	st_product_info.nCok = 0;

	if (st_product_info.nRunTime <= 0) strTable[5] = _T("0");
	else
	{
		strTable[5].Format(_T("%.1f"), (double)st_product_info.nRunTime / (double)60);
	}
	st_product_info.nRunTime = 0;

	if (st_product_info.nStopTime <= 0) strTable[6] = _T("0");
	else
	{
		strTable[6].Format(_T("%.1f"), (double)st_product_info.nStopTime / (double)60);
	}
	st_product_info.nStopTime = 0;

	if (st_product_info.nIdleTime <= 0) strTable[7] = _T("0");
	else
	{
		strTable[7].Format(_T("%.1f"), (double)st_product_info.nIdleTime / (double)60);
	}
	st_product_info.nIdleTime = 0;

	if (st_product_info.nFrontTime <= 0) strTable[8] = _T("0");
	else
	{
		strTable[8].Format(_T("%.1f"), (double)st_product_info.nFrontTime / (double)60);
	}
	st_product_info.nFrontTime = 0;

	if (st_product_info.nAlarmTime <= 0) strTable[9] = _T("0");
	else
	{
		strTable[9].Format(_T("%.1f"), (double)st_product_info.nAlarmTime / (double)60);
	}
	st_product_info.nAlarmTime = 0;

	strTable[10].Format(_T("%d"), st_product_info.nAlarmCount);
	st_product_info.nAlarmCount = 0;

	strTable[11].Format(_T("%.1f"), st_product_info.dMtbi);
	st_product_info.dMtbi = 0.0f;

	strTable[12].Format(_T("%d"), st_product_info.nRetest);
	st_product_info.nRetest = 0;

	strTable[13].Format(_T("%.1f"), st_product_info.dPrime);
	st_product_info.dPrime			= 0.0f;
	st_product_info.nPrimeCount		= 0;
	st_product_info.nRejectCount	= 0;

	Mysql.WriteWork(strTable);
}


void CMainFrame::OpenDll()
{
	LogOpen();		// Log관련 dll open

	Open(this);		// interface dll open
	// Rfid open
	RfidCreate();
	// VnR Server Open
	VnRCreate();
	// Barcode open
	BarcodeCreate(BCR_1D_PORT);
	BarcodeCreate(BCR_2D_PORT);
}


void CMainFrame::CloseDll()
{
	LogClose();
	
	//Rfid Close
	RfidDelete();
	// VnR Server Close
	VnRDelete();
	// Barcode Close
	BarcodeDelete(BCR_1D_PORT);
	BarcodeDelete(BCR_2D_PORT);
// 		// Interface관련 dll close
	Close();
}



LRESULT CMainFrame::OnBarcodeMessage1(WPARAM wParam, LPARAM lParam)
{
	CString *strMsg;
	CString strTri;

	switch(wParam)
	{
		case BARCODE_DATA:
			strMsg = (CString*)lParam;	

			clsBcr1D.OnDataRevice(*strMsg);
			clsBcr1D.m_nCommand = BD_END;
			break;

		case BARCODE_ERROR:
			strMsg = (CString*)lParam;	

			clsBcr1D.OnDataRevice(*strMsg);
			clsBcr1D.m_nCommand = BD_END;
			break;
			

		case BARCODE_TRIGGER_1:
			st_basic_info.nModeDevice = 1;
			if (st_basic_info.nModeDevice == WITH_DVC)
			{
				// jtkim 20160903
				// BarcodeReadT(BCR_1D_PORT, clsBcr1D.m_strTrigger);
				BarcodeReadT(BCR_1D_PORT, _T("LON"));
			}
			else
			{
				clsBcr1D.OnDataRevice(_T("11S45D3223YH30MS972041,KR M351B4G73DB0-YK0 1411"));
				clsBcr1D.m_nCommand = BD_END;
			}
			break;

		case BARCODE_TRIGGER_2:
			break;

		case BARCODE_DELETE:
			break;
	}
	return 0;
}


LRESULT CMainFrame::OnBarcodeMessage2(WPARAM wParam, LPARAM lParam)
{
	CString *strMsg;
	CString strTri;

	switch(wParam)
	{
		case BARCODE_DATA:
			strMsg = (CString*)lParam;	

			clsBcr2D.OnDataRevice( *strMsg);
			clsBcr2D.m_nCommand = BD_END;
			break;

		case BARCODE_ERROR:
			strMsg = (CString*)lParam;	

			clsBcr2D.OnDataRevice(*strMsg);
			clsBcr2D.m_nCommand = BD_END;
			break;
		
		case BARCODE_TRIGGER_1:
			clsBcr2D.m_nCommand = clsBcr2D.m_nBin = BD_NONE;

			if (st_basic_info.nModeDevice == WITH_DVC)
			{
				// jtkim 20160903
				//BarcodeReadT(BCR_2D_PORT, clsBcr2D.m_strTrigger);
				BarcodeReadT(BCR_2D_PORT, _T("LON"));
			}
			else
			{
				clsBcr2D.OnDataRevice(_T("11S45D3223YH30MS972041,KR M351B4G73DB0-YK0 1411"));
				clsBcr2D.m_nCommand = BD_END;
			}
			break;

		case BARCODE_TRIGGER_2:
			break;

		case BARCODE_DELETE:
			break;
	}
	return 0;
}


LRESULT CMainFrame::OnBarcodeMessage3(WPARAM wParam, LPARAM lParam)
{
	CString *strMsg;
	CString strTri;

	switch(wParam)
	{
		case BARCODE_DATA:
			strMsg = (CString*)lParam;	

			//clsBcrUnLdB.OnDataRevice(clsBcrUnLdB.m_nX, clsBcrUnLdB.m_nY, *strMsg);
			//clsBcrUnLdB.m_nCommand = BD_END;
			break;

		case BARCODE_ERROR:
			strMsg = (CString*)lParam;	

			//clsBcrUnLdB.OnDataRevice(clsBcrUnLdB.m_nX, clsBcrUnLdB.m_nY, *strMsg);
			//clsBcrUnLdB.m_nCommand = BD_END;
			break;
		
		case BARCODE_TRIGGER_1:
			//clsBcrUnLdB.m_nCommand = clsBcrUnLdB.m_nBin = BD_NONE;

			///if (st_basic_info.nModeDevice == WITH_DVC)
			//{
			//	strTri.Format(_T("LON%02d"), st_recipe_info.nBcrBank);
			//	BarcodeReadT(UNLD_BCR_B_PORT, strTri);
			//}
			//else
			//{
			//	clsBcrUnLdB.OnDataRevice(clsBcrUnLdB.m_nX, clsBcrUnLdB.m_nY, _T("B_BARCODE"));
			//	clsBcrUnLdB.m_nCommand = BD_END;
			//}
			break;

		case BARCODE_TRIGGER_2:
			break;

		case BARCODE_DELETE:
			break;
	}
	return 0;
}


LRESULT CMainFrame::OnBarcodeMessage4(WPARAM wParam, LPARAM lParam)
{
//	int nData;
	CString *strMsg, strTri;

	switch(wParam)
	{
		case BARCODE_DATA:
			strMsg = (CString*)lParam;	

			
			break;

		case BARCODE_ERROR:
			strMsg = (CString*)lParam;	
			break;
		
		case BARCODE_TRIGGER_1:															//레귤레이터 공압설정 커맨드 전송.
			//st_recipe_info.strSend = clsFunc.OnSetAirPressure(st_recipe_info.dInPressVal);
			//BarcodeReadT(REGULATOR_PORT, strTri);
			break;

		case BARCODE_TRIGGER_2:															//레귤레이터에서 실제로 조절하고 있는 공압값 받아오기.
			//st_recipe_info.strSend = clsFunc.OnGetAirPressure(1);
			//BarcodeReadT(REGULATOR_PORT, strTri);
			break;

		case BARCODE_DELETE:
			break;
	}
	return 0;
}


LRESULT CMainFrame::OnBarcodeMessage5(WPARAM wParam, LPARAM lParam)
{
	CString *strMsg;

	switch(wParam)
	{
		case BARCODE_DATA:
			strMsg = (CString*)lParam;	
			break;

		case BARCODE_ERROR:
			strMsg = (CString*)lParam;	
			break;
		
		case BARCODE_TRIGGER_1:

			break;

		case BARCODE_TRIGGER_2:
			break;

		case BARCODE_DELETE:
			break;
	}
	return 0;
}

LRESULT CMainFrame::OnRfidMessage(WPARAM wParam, LPARAM lParam)
{
	int nChan;

	CString *strMsg;
	CString strTemp;
	switch(wParam)
	{
		case RFID_DATA:
			
			break;

		case RFID_ERROR:
			strMsg = (CString*)lParam;	
			break;

		case RFID_READ:
			nChan = (int)lParam;
			clsRfid.m_nCommBin = BD_NONE;
			RfidReadTag(nChan);
			
			break;

		case RFID_WRITE:
			nChan = (int)lParam;

			clsRfid.m_nCommBin = BD_NONE;
			RfidWriteAllData(nChan, clsRfid.m_strRfidData);
			break;

		case RFID_WRITE_GOOD:
			strMsg = (CString*)lParam;	

			clsRfid.m_nCommBin = BD_DATA_GOOD;
			break;
	
		case RFID_WRITE_NG:
			strMsg = (CString*)lParam;	

			clsRfid.m_nCommBin = BD_DATA_REJECT;
			break;
	
		case RFID_READ_GOOD:
			strMsg = (CString*)lParam;	
			clsRfid.OnDataReceive(*strMsg);//kwlee 2017.0123
			//clsRfid.m_nCommBin = BD_DATA_GOOD;
			break;
	
		case RFID_READ_NG:
			strMsg = (CString*)lParam;	
			
			clsRfid.m_nCommBin = BD_DATA_REJECT;
			break;	

		case RFID_DELETE:

			break;
	}

	return 0;
}

//kwlee 2016.0913 Vision 관련 임시 막아 놓음.
LRESULT CMainFrame::OnMainframe_Work(WPARAM wParam, LPARAM lParam)
{	
	int mn_command_num = wParam;  // 네트워크 작업을 할 구분 변수

	switch (mn_command_num)
	{		
		case MAIN_TEACH_VISION:
			OnMainFrame_TeachVision( lParam );
		break;

	}

	return 0;
}

void CMainFrame::OnMainFrame_TeachVision( int iCommand )
{
	double OffsetX = 0.0;
	double OffsetY = 0.0;
	double OffsetZ = 0.0;

	switch( iCommand )
	{
	case 0:
		m_pAMTVClassWrapper->HideForm();
		break;

	case 1:
		m_pAMTVClassWrapper->ShowForm();
		break;

	case 2:
		m_pAMTVClassWrapper->ChangeChannel( 0 );
		break;

	case 3:
		m_pAMTVClassWrapper->ChangeChannel( 1 );
		break;

	case 4:		
		m_pAMTVClassWrapper->Result(&OffsetX, &OffsetY);
		st_teach.m_dVisionXValue = OffsetX;
		st_teach.m_dVisionYValue = OffsetY;
		break;

	case 5:		
		m_pAMTVClassWrapper->EdgeScore(&OffsetZ);
		st_teach.m_dVisionZValue = OffsetZ;
		break;

	}
}
 LRESULT CMainFrame::OnMainMessageEvent(WPARAM wParam, LPARAM lParam)
 {
	int i = 0;
	CString strTemp;
	
	if (wParam == CTL_YES)
	{
		if (mp_msg_dlg != NULL && IsWindow(mp_msg_dlg->m_hWnd))
		{
			mp_msg_dlg->SetFocus();	// 대화상자를 활성화
			mp_msg_dlg->OnEventMsg_Text_Set();
		}
		else
		{
			mp_msg_dlg = new CDialog_Event_Msg;
			mp_msg_dlg->Create();
			mp_msg_dlg->ShowWindow(SW_SHOW);
		}
		FAS_IO.set_out_bit(st_io_info.o_Buzz1OnOff,  IO_ON);

	}
	else if (wParam == CTL_NO)
	{
		for (i = 0; i < 3; i++)
		{
			st_handler_info.mstr_event_msg[i] = "";
		}
		
		if (mp_msg_dlg != NULL && IsWindow(mp_msg_dlg->m_hWnd))
		{
			mp_msg_dlg->ShowWindow(SW_HIDE);
			mp_msg_dlg->DestroyWindow();
			delete mp_msg_dlg;
			mp_msg_dlg = NULL;
		}
		
		FAS_IO.set_out_bit(st_io_info.o_Buzz1OnOff, IO_OFF);
	}
	
	return 0;
 }

 BOOL CMainFrame::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
 {
	 // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	COPYDATASTRUCT cds;
	CString strMsg;
	
	switch( pCopyDataStruct->dwData)
	{ 
	case WM_PARTID_DATA:
		{					
			g_cs.EnterCriticalSection();
			st_copydata = new st_copy_data;			
			st_copydata->mn_mdl = 19;
			
			CString strPartID;
			strPartID.Format( _T("%s"),  (LPCTSTR) st_lot_info[LOT_CURR].strPartNo);
			strPartID.TrimLeft(' '); strPartID.TrimRight(' ');
			strPartID.TrimLeft(); strPartID.TrimRight();			
			
			memset(st_copydata->str_partid, 0x00, sizeof(st_copydata->str_partid));
			//sprintf(st_copydata->str_partid,"%s", (LPCTSTR)strPartID);

			clsFunc.OnStringToChar((LPCTSTR)strPartID,st_copydata->str_partid);
			
			cds.dwData = WM_MDL_PART_DATA;
			cds.cbData = sizeof(st_copy_data);
			cds.lpData = st_copydata;
			
			HWND hwnd = ::FindWindow(NULL,_T("ModelChange"));
			if (hwnd!= NULL)
			{
				HANDLE handle = AfxGetInstanceHandle();
				::SendMessage(hwnd, WM_COPYDATA, (WPARAM)handle, (LPARAM)&cds);
			}
			else
			{
				if (st_handler_info.cWndList != NULL) 
				{
					strMsg.Format( _T("ModelChange.exe can't find program(C:\\AMT840\\BIN\\ModelChange.exe") );
					clsMem.OnAbNormalMessagWrite(strMsg);	
				}
			}
			
			delete st_copydata;
			g_cs.LeaveCriticalSection();			
		}
		break;	
	}

	 return CFrameWnd::OnCopyData(pWnd, pCopyDataStruct);
 }
