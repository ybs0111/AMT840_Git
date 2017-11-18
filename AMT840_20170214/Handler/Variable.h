#include <afxtempl.h>

#ifndef _VARIABLE_h__
#define _VARIABLE_h__

#include "Alg_Mysql.h"

// amt8562
//	Console application ���α׷��� ���ؼ� �Ʒ� �κ��� ������
typedef unsigned long int				DWORD;
typedef unsigned short int				WORD;
typedef unsigned char					BYTE;
typedef int								BOOL;
typedef void							*HANDLE;

#ifndef PASCAL
#define PASCAL						__stdcall
#endif	// PASCAL

// jtkim 20130520
/*+
#ifndef EXPORT
//#define EXPORT
#endif	// EXPORT
*/
//typedef int							BOOL;			// 0(FALSE), 1(TRUE)
typedef signed char						INT8;			// -128 .. 127
typedef unsigned char					UINT8;			// 0 .. 255
typedef signed short int				INT16;			// -32768 .. 32767
typedef unsigned short int				UINT16;			// 0 .. 65535
typedef int								INT32;			// -2,147,483,648 .. 2,147,483,647
typedef unsigned int					UINT32;			// 0 .. 4,294,967,295

// *****************************************************************************
// ����� ���� �޽��� ID ����                                                   
// *****************************************************************************
#define WM_FORM_CHANGE			WM_USER+200   // ȭ���� ��ȯ�� �� �ش� ��ȯ ���� ������ �����ϱ� ���� ����� ���� �޽��� 

#define WM_STATUS_CHANGE		WM_USER+201   // Ÿ��Ʋ �� ȭ������ ���۵� �޽��� ��½�Ű�� ���� ����� ���� �޽��� �߰� 

#define WM_LIST_DATA			WM_USER + 6   // ��� �޽��� ��� ����Ʈ ��ȭ ���ڿ� 
                                              // �ø��� ��Ʈ�� ���� ��/���ŵ� �޽����� ����ϱ� ���� ����� ���� �޽��� 
#define WM_DATA_SEND			WM_USER + 7   // Data�� �����ϱ� ���� ȭ�鿡�� ������ �����ϱ� ���� ������ ����� ���� �Լ�

#define WM_WORK_END				WM_USER + 8   // �׽�Ʈ �۾� �Ϸ� �� ������ ���� ȭ�鿡 ��½�Ű�� ���� ����� ���� �޽��� �߰� 

#define WM_BASIC_APPLY			WM_USER + 10  // BASIC DATA�� APPLY ��Ű�� ���� ����� ���� �޽��� �߰� 

#define WM_MAINTENANCE_APPLY	WM_USER + 11  // Maintenance DATA�� APPLY ��Ű�� ���� ����� ���� �޽��� �߰�
#define WM_INTERFACE_APPLY		WM_USER + 13
#define	WM_WAITTIME_APPLY		WM_USER + 15  // Wait Time DATA�� APPLY ��Ű�� ���� ����� ���� �޽��� �߰� 

#define	WM_MODULESITE_APPLY		WM_USER + 16
#define	WM_HEATSINKSITE_APPLY	WM_USER + 17
#define	WM_MODULEROBOT_APPLY	WM_USER + 18
#define	WM_HEATSINKROBOT_APPLY	WM_USER + 19
#define	WM_DISPENSER_APPLY		WM_USER + 20
#define	WM_MOTORSPEED_APPLY		WM_USER + 21
#define WM_SERIAL_PORT			WM_USER + 25
#define	WM_TESTREFERENCE_MANUAL		WM_USER + 21
#define WM_MAINFRAME_WORK		WM_USER + 28 //kwlee 2016.0913
#define	WM_LOTOPEN_APPLY		WM_USER + 26
#define	WM_LOTSTART_APPLY		WM_USER + 27

#define WM_DATA_INIT_SAVE       WM_USER + 300 // apply��ư�� �������, �ش� ȭ���� ������ ����(Init)
#define WM_DATA_CHANGE          WM_USER + 301 // apply��ư�� �������, �ش� ȭ���� ������ ����(Init)

#define WM_WORK_DISPLAY			WM_USER + 400
#define WM_WORK_COMMAND			WM_USER + 401

#define WM_CLIENT_MSG			WM_USER + 500
#define WM_CLIENT_MSG_1			WM_USER + 500
#define WM_CLIENT_MSG_2			WM_USER + 501
#define WM_CLIENT_MSG_3			WM_USER + 502
#define WM_CLIENT_MSG_4			WM_USER + 503
#define WM_CLIENT_MSG_5			WM_USER + 504
#define WM_CLIENT_MSG_6			WM_USER + 505
#define WM_CLIENT_MSG_7			WM_USER + 506
#define WM_CLIENT_MSG_8			WM_USER + 507
#define WM_CLIENT_MSG_9			WM_USER + 508
#define WM_CLIENT_MSG_10		WM_USER + 509
#define WM_CLIENT_MSG_11		WM_USER + 510
#define WM_CLIENT_MSG_12		WM_USER + 511

#define WM_SERVER_MSG			WM_USER + 550
#define WM_SERVER_MSG_1			WM_USER + 550
#define WM_SERVER_MSG_2			WM_USER + 551
#define WM_SERVER_MSG_3			WM_USER + 552
#define WM_SERVER_MSG_4			WM_USER + 553
#define WM_SERVER_MSG_5			WM_USER + 554
#define WM_SERVER_MSG_6			WM_USER + 555
#define WM_SERVER_MSG_7			WM_USER + 556
#define WM_SERVER_MSG_8			WM_USER + 557
#define WM_SERVER_MSG_9			WM_USER + 558
#define WM_SERVER_MSG_10		WM_USER + 559

#define WM_GMS_MSG				WM_USER + 600

#define WM_MAIN_EVENT			WM_USER + 601

#define WM_BARCODE_MSG_1		WM_USER + 10615
#define WM_BARCODE_MSG_2		WM_USER + 10616
#define WM_BARCODE_MSG_3		WM_USER + 10617
#define WM_BARCODE_MSG_4		WM_USER + 10618
#define WM_BARCODE_MSG_5		WM_USER + 10619

#define WM_REGULATOR_MSG		WM_USER + 40000
#define WM_RFID_MSG				WM_USER + 31121

#define RFID_DATA				100
#define RFID_ERROR				101
#define RFID_READ				102
#define RFID_WRITE				103
#define RFID_WRITE_GOOD			104
#define RFID_WRITE_NG			105
#define RFID_READ_GOOD			106
#define RFID_READ_NG			107
#define RFID_DELETE				120

#define BARCODE_DATA			100
#define BARCODE_ERROR			101
#define BARCODE_TRIGGER_1		102
#define BARCODE_TRIGGER_2		103
#define BARCODE_DELETE			120
//////////////////////////////////////////////

#define WM_SLOTINFO_MSG				WM_USER + 700
// #define WM_LD_ROBOT_MOVE_MSG		WM_USER + 800
// #define WM_ULD_ROBOT_MOVE_MSG		WM_USER + 810
#define WM_LDBUFFER_MOVE_MSG			WM_USER + 800
#define WM_ULDBUFFER_MOVE_MSG			WM_USER + 810


#define LD_LOAD_TO_UNLOAD                0
#define LD_UNLOAD_TO_LOAD                1
#define ULD_LOAD_TO_UNLOAD               3
#define ULD_UNLOAD_TO_LOAD               4
//kwlee 2017.0104
#define MAX_TSITE                        8
#define MAX_MARK_POS                     4
//2016.1116
#define WM_SEND_COPY_DATA	   1
#define WM_MDL_PART_DATA       2

#define	WM_PARTID_DATA		   1

//////////////////////////////////////////////

#define CLIENT_CONNECT			0
#define CLIENT_CLOSE			1
#define CLIENT_SEND				2
#define CLIENT_REV				3
#define CLIENT_ACCEPT			4

#define SERVER_CONNECT			0
#define SERVER_CLOSE			1
#define SERVER_SEND				2
#define SERVER_REV				3
#define SERVER_ACCEPT			4

#define INITIAL_INDEX_DUMP		0	
#define INITIAL_TEST_DUMP		1
#define INITIAL_INDEX_TABLE		2
// *****************************************************************************

// *****************************************************************************
//  ��Ʈ�� �޼��� ���� ����ʿ��� BPC�� ������ �޼���.
// *****************************************************************************
#define NW_LOT_START_RPY				1
#define NW_LOT_CANCEL_RPY				2
#define NW_PCBOX_START_RPY				3
#define NW_PCBOX_END_RPY				4
#define NW_PCBOX_CHANGE_RPY				5
#define NW_ENABLE_RPY					6
#define NW_DISABLE_RPY					7
#define NW_SLOT_MASK_RPY				8
#define NW_LOT_START_ENABLE_RPY			9
#define NW_PCBOX_REMOVE_RPY				10
#define NW_PCBOX_ERROR_RPY				11

#define NW_PCBOX_OFF					12
#define NW_UNLOAD_COMPT					13
#define NW_CANCEL_END					14
#define NW_LOT_END						15
#define NW_PCBOX_DISABLE				16
#define NW_VIDEO_TIMEOUT				17
#define NW_HOLD_TIMEOUT					18
#define NW_PCBOX_LOT_INFO				19
#define NW_FAIL_INFO_SEND				20
#define NW_FAIL_REGNUMBER_REQUEST		21
#define NW_FAIL_INFO_SERACH				22
#define NW_FAIL_INFO_SAVE				23
#define NW_FAIL_INFO_DELETE				24
// ���� �߰� ��.
#define NW_LOT_DISPLAY					25
#define NW_EQP_LOT_START				26
#define NW_EQP_LOT_CANCEL				27
#define NW_EQP_LOT_END					28

// *****************************************************************************
//  ��Ʈ�� �޼��� ���� BPC�ʿ��� ���� ������ �޼���.
// *****************************************************************************
#define NW_LOT_START					29
#define NW_LOT_CANCEL					30
#define NW_PCBOX_START					31
#define NW_PCBOX_END					32
#define NW_PCBOX_CHANGE					33
#define NW_ENABLE						34
#define NW_DISABLE						35
#define NW_SLOT_MASK					36
#define NW_LOT_START_ENABLE				37
#define NW_PCBOX_REMOVE					38
#define NW_PCBOX_ERROR					39

#define NW_PCBOX_LOT_INFO_RPY			40
#define NW_FAIL_INFO_SEND_RPY			41
#define NW_FAIL_REGNUMBER_REQUEST_RPY	42
#define NW_FAIL_INFO_SERACH_RPY			43
#define NW_FAIL_INFO_SAVE_RPY			44
#define NW_FAIL_INFO_DELETE_RPY			45

// ���� �߰� ��
#define NW_LOT_DISPLAY_RPY				46
#define NW_EQP_LOT_START_RPY			47
#define NW_EQP_LOT_CANCEL_RPY			48
#define NW_EQP_LOT_END_RPY				49
// *****************************************************************************

#define NW_PCBOX_ENABLE					50
#define NW_PGM_START					60
//#define NW_FAIL_REGNUMBER_REQUEST		61
// *****************************************************************************
//  List Box ��¿� �޼��� ����                                                 
// *****************************************************************************
#define RECEIVE_MSG			1
#define SEND_MSG			2
#define NORMAL_MSG			3
#define ABNORMAL_MSG		4
#define MACHINE_INFO		5
#define DEVICE_INFO			6

// *****************************************************************************
// *****************************************************************************
//  SCREEN_MAIN ��¿� �޼��� ����                                                
// *****************************************************************************
#define MAIN_TIMEINFO				100
#define MAIN_COUNT_DISPLAY			101
#define MAIN_LF_INTERFACE_DISPLAY	102 //(left �׽�Ʈ ���� ǥ��)
#define MAIN_RI_INTERFACE_DISPLAY	103 //(right �׽�Ʈ ���� ǥ��)
#define MAIN_LD_PICKER_DISPLAY 		104 //(loader picker ���� ǥ��)
#define MAIN_LD_BUF_PICK_DISPLAY	105 //(loader buffer pick ���� ǥ��)
#define MAIN_LD_BUF_PLACE_DISPLAY	106 //(loader buffer place ����ǥ��)
#define MAIN_LD_TRAY_DISPLAY		107 //(loader tray ����ǥ��)
#define MAIN_UNLD_PICKER_DISPLAY	108 //(unloader picker ���� ǥ��)
#define MAIN_UNLD_BUF_PICK_DISPLAY	109 //(unloader buffer pick ����ǥ��)
#define MAIN_UNLD_BUF_PLACE_DISPLAY	110 //(unloader buffer place ����ǥ��)
#define MAIN_UNLD_TRAY_DISPLAY		111 //(unloader tray ���� ǥ��)
#define MAIN_RETEST_BUF_DISPLAY		112 //(retest buffer ���� ǥ��)
#define MAIN_REJECT_BUF_DISPLAY		113 //(reject buffer ���� ǥ��)
#define MAIN_PCB_TRANS_DISPLAY		114 //(pcb transfer ����ǥ��)
#define MAIN_CONVEYOR_DISPLAY		115 //(conveyor ����ǥ��)
#define MAIN_LOT_DISPLAY			116 //(conveyor ����ǥ��)
#define MAIN_BARCODE_DISPLAY		117 //(conveyor ����ǥ��) // jtkim 20150316

#define MAIN_PCB_INFO				200
#define MAIN_PCB_INFO_BACK			201
#define MAIN_PCB_INFO_NEXT			202

#define MAIN_PCB_PICKER_CREATE_MSG	300
#define MAIN_PCB_PICKER_DELETE_MSG	301
#define MAIN_PCB_BUFFER_CREATE_MSG	302
#define MAIN_PCB_BUFFER_DELETE_MSG	303
#define MAIN_PCB_TRAY_CREATE_MSG	304
#define MAIN_PCB_TRAY_DELETE_MSG	305
#define MAIN_PCB_INTER_CREATE_MSG	306
#define MAIN_PCB_INTER_DELETE_MSG	307

#define MAIN_BD_ERR_CREATE_MSG		400
#define MAIN_BD_ERR_DELETE_MSG		401

#define MAIN_MESSAGE_BOX_CREATE_REQ	100
#define MAIN_MESSAGE_BOX_DELETE_REQ	101

#define MAIN_BARCODE_BOX_CREATE_REQ	110
#define MAIN_BARCODE_BOX_DELETE_REQ	111

#define	MAIN_LOT_END_CREATE_REQ		500
#define	MAIN_LOT_END_DELETE_REQ		501
#define	MAIN_LOT_START_CREATE_REQ	510
#define	MAIN_LOT_START_DELETE_REQ	511

#define MAIN_LOT_HISTORY_CREATE_REQ 600
#define MAIN_LOT_HISTORY_DELETE_REQ 601

#define MAIN_PCB_DATA_CREATE_REQ	700
#define MAIN_PCB_DATA_DELETE_REQ	701

#define MAIN_NOTICE_CREATE_REQ		800
#define MAIN_NOTICE_DELETE_REQ		801

#define MOTOR_IO_CREATE_REQ			900
#define MOTOR_IO_DELETE_REQ			901

#define MAIN_COK_CREATE_REQ			902

#define MAIN_MESSAGE_VIEW_CREATE_REQ 1000
#define MAIN_MESSAGE_VIEW_DELETE_REQ 1001

#define MAIN_UNLD_CVY_CREATE_REQ	1100
#define MAIN_UNLD_CVY_DELETE_REQ    1101

#define MAIN_SOCKET_COMMENT_CREATE_REQ	1200
#define MAIN_SOCKET_COMMENT_DELETE_REQ	1201

#define ALARM_IO_CREATE_REQ 100
#define ALARM_IO_DELETE_REQ 101
#define ALARM_MOTOR_CREATE_REQ 200
#define ALARM_MOTOR_DELETE_REQ 201

#define INIT_RECOVERY_COMMAND		0
#define INIT_INITIALIZE_COMMAND		1
#define INIT_COK_EXCHANGE_COMMAND	2 //2015.0407 james COK ��ü �۾����� �۾��ڿ��� Ȯ���� �޴´� 
// *****************************************************************************
//  Title Bar ��¿� �޼��� ����                                                
// *****************************************************************************
#define FORM_NAME			1
#define MACHINE_STATUS		2
#define DEVICE_MODE			3
#define INTERFACE_MODE		4
#define LEVEL_MODE			5
#define FILE_MODE			6
#define MODEL_MODE			7
#define COMMUNICATION_MODE	8
#define XGEM_MODE			9
//kwlee 2016.0825
#define BCR_1D              10
#define BCR_2D              11
// *****************************************************************************

// *****************************************************************************
// ��� ���� ���� �� �ʿ��� �޽��� ����                                         
// *****************************************************************************
#define TIMECOUNT			5
#define ELEVATOR_STRATUMS	6
// *****************************************************************************

// *************************************************************************
// ��� ���� ���� �� �ʿ��� �޽��� ����                                          
// *************************************************************************
#define dSTOP				0
#define dRUN				1
#define dIDLE				2
#define dJAM				3
#define dLOTEND				4
#define dINIT				5
#define dWARNING			6
#define dLOCK				7
#define dSELFCHECK			8
#define dMAINT				9
#define dREINSTATE			10	//���͵�����
#define dRECOVERY			11
#define dMOTOR_WARNING		12 //�̶��� �˶��� �߻���Ű�� �ʴ´�(run status�� �ٲ��� �ʴ´�).thread ������ �Ǵ��Ͽ� �˶� �߻���Ų�� 

#define eWARNING			0
#define eRUNNING			1
#define eSTOP				2
#define eERROR				3

#define IO_CLEAR			-1
#define NO					0
#define YES					1
#define COVER_YES			2
#define READY				2
#define PROCEED				3


#define IO_ON				1
#define IO_OFF				0
#define IO_ERROR			2
#define IO_CHK_ERROR		4

#define IO_NONE				0
#define IO_IN				1
#define IO_OUT				2

#define IO_CLAMP			1
#define IO_UNCLAMP			0

#define IO_HOLD				0
#define IO_RELEASE			1

#define IO_DOWN				1
#define IO_UP				0

#define IO_PROCEED			2
#define IO_TIMEOUT			3

#define IO_LEFT				0
#define IO_RIGHT			1

#define IO_CLOSE			0
#define IO_OPEN				1

#define IO_GOOD				10
#define IO_FAIL				11

#define FUN_ING				0
#define FUN_END				1
#define FUN_ERR				2
#define FUN_CTU				3

#define PICKER				0
#define FINGER				1

//kwlee 2017.0904
#define START               0
#define END                 1
//2015.0324 james 
#define SOCKET_CONTACT_ON			100
#define SOCKET_CONTACT_OFF			200
///////////////////////////////////////////////


#define TIMECOUNT			5
#define ELEVATOR_STRATUMS	6
// *************************************************************************

// *************************************************************************
// RS-232C �ø��� ��� �� ���Ǵ� �޽��� ����                                   
// -> MAX_PORT : �⺻ ��Ʈ 2���� �����Ǿ� �ִ�						             
// *************************************************************************
#define MAX_PORT				2	// ������ �ø��� ��Ʈ ����
#define COM_ERROR				-1	// ���� �޽���
#define CRI_BARCODE_PORT		0		// ���� BCR READ ��Ʈ ��ȣ
#define MPCB_BARCODE_PORT		1		// ���� BCR READ ��Ʈ ��ȣ
#define LIGHT_PORT				2
#define A_LIGHT_PORT			3

#define COM_PORT			0
#define COM_BAUDRATE		1
#define COM_DATA			2
#define COM_STOP			3
#define COM_PARITY			4

//#define TESTER_PORT	2		// ���� Tester Communication ��Ʈ ��ȣ
//#define COM_PORT   2	// ���� ��� ���� �ø��� ��Ʈ ��ȣ
 
#define BUF_SIZE  1000	// ���� ũ��
// *************************************************************************

// *************************************************************************
// *************************************************************************
// I/O ��� ���� �� ��Ʈ �ִ� ���� �����Ѵ�                                      
// *************************************************************************
#define MAX_IO				1315

#define MAX_MASTER_IO		1
#define MAX_PORT_IO			4
#define MAX_SLAVE_IO		8
// *************************************************************************
#define M_BARCODE_PORT		0
#define C_BARCODE_PORT		1
// *************************************************************************
//  MOTOR ���� ���� ���� �����Ѵ�                                                
// *************************************************************************
#define MOT_GOOD			1
#define MOT_PROGRESS		2
#define MOT_RETRY			3
#define MOT_ERROR			4

#define PLUS				1
#define MINUS				0
#define RUN_MODE			1
#define MANUAL_MODE			2

#define POWER_OFF			0
#define POWER_ON			1

#define ELM					0
#define ELP					1
#define HOME				2

#define MOT_ACC				0
#define MOT_DEC				1
#define MOT_JOG				2
#define MOT_ORG				3
#define MOT_FEEDRATE		4
#define MOT_INTERACC		5
#define MOT_ORGMETHOD		6
#define MOT_ORGACCDEC		7

#define MOT_FAST			10
#define MOT_SLOW			20
#define TSITE_SOCKET_CNT	48
// *************************************************************************

// *****************************************************************************
//  Main Screen ��¿� �޼��� ����                                              
// *****************************************************************************

// *****************************************************************************
///// test bin  result
#define BIN_NOTUESD			0
#define BIN_LDBUFFERBIN		1
#define BIN_RETESTBIN		2
#define BIN_GOODBIN			3
#define BIN_REJECTBIN		4
#define BIN_REJECTBIN_1		5
#define BIN_REJECTBIN_2		6
#define BIN_REJECTBIN_3		7
#define BIN_REJECTBIN_4		8

///////////////////
#define RET_NOWHERE			-1000		// �ش� �����Ͱ� �������� ������ 
#define RET_NOSPACE			-2000		// ������ ������ 
#define RET_EMPTYDEVICE		-3000		// ����̽��� ��� ������ 
#define RET_FULLDEVICE		-4000		// ����̽��� FULL
#define RET_LOADTRAYSKIP	-10000		// loader key skip

#define RET_PICKERALLYES 	-5000	// ��Ŀ ������ �����϶� 
#define RET_PICKERALLNO 	-6000	// ��Ŀ ������ �����϶� 
#define RET_PICKERNOTFIND	-7000	// �� �ش��ϴ� ������ ����

#define BUTTON_MOUSE_DN		1100
#define BUTTON_MOUSE_UP		1000

// GRID TYPE
#define DFC_NOUSE       -1//�����ý��� �׸��� �����(0���÷��� ������� ����Ҷ� �̿��ϸ��)
#define DFC_STATIC       0
#define DFC_EDIT         1
#define DFC_COMBO        2
#define DFC_CHECK        3
#define DFC_DATE         4
#define DFC_TIME         5
#define DFC_PROGRESS     6
#define DFC_IMAGE        7

#define IMG_LISTHDCTRL_BG _T("���2.jpg")//_T("smflower.jpg")
#define IMG_PICCTRL_BG _T("���2.jpg") //_T("smflower.jpg")
#define IMG_LISTHDCTRL_BG1 _T("���25.gif")//_T("smflower.jpg")
#define IMG_PICCTRL_BG1 _T("���25.gif") //_T("smflower.jpg")

// Į�� ����
#define SKY_C					RGB(187, 250, 255)
#define NOR_C					RGB(164, 164, 164)
#define NOR_L					RGB(190, 190, 190)
#define RED_C					RGB(255, 0, 0)
#define RED_L					RGB(255, 150, 150)
#define RED_D					RGB(80, 20, 0)
#define BLUE_C					RGB(0, 0, 255)
#define WHITE_C					RGB(255, 255, 255)
#define BLACK_C					RGB(25, 1, 1)
#define BLACK_L					RGB(62, 55, 55)
#define GRAY					RGB(90, 80, 80)
#define YELLOW_C				RGB(255,255,0)
#define YELLOW_D				RGB(128, 128, 0)
#define YELLOW_L				RGB(255, 255, 193)
#define GREEN_C					RGB(0,255,0)
#define GREEN_L					RGB(150,255,150)
#define GREEN_D					RGB(0,100,20)
#define DSERBLE_BC				RGB(210,210,230)
#define TEXT_BC					RGB(220,230,210)
#define OK_BC					RGB(0, 0, 255)
#define NG_BC					RGB(255, 0, 0)
#define RESULT_BC				RGB(40, 1, 1)
#define ORANGE_C				RGB(0xFF, 0x68, 0x20)
#define BLACK					RGB(0, 0, 0)
#define BLUE_D					RGB(0, 0, 128)
#define BLUE				    RGB(0, 0, 255)
#define PINK					RGB(255, 125, 225)
#define TEXT_BC_1				RGB(240, 240, 240210)
#define BLUE_L					RGB(124, 139, 252)
#define ON_C					RGB(0, 255, 0)
#define OFF_C					RGB(0, 150, 0)
#define NG_C					0x0000FF
#define OK_C					0xFF00
#define BLACK_GC				0x00
#define YELLOW_GC				0xFFFF
#define TEST_BC					RGB(194, 231, 150)
#define IN_BC					RGB(204, 153, 255)

#define DVC_READY_C				RGB(153, 153, 255)
#define DVC_LOAD_C				RGB(239, 214, 198)
#define DVC_NOVIDEO_C			RGB(0,100,20)
#define DVC_PGM_C				RGB(255, 255, 0)
#define DVC_UNLOAD_C			RGB(255, 255, 204)
#define DVC_NONE_C				RGB(150, 150, 150)
#define DVC_START_C				RGB(150, 255, 150)
#define DVC_NO_C				RGB(128, 128, 0)

#define THICKNESS_FAIL			RGB(255,255,0)
#define LENGTH_FAIL				RGB(0xFF, 0x68, 0x20)
#define BARCODE_FAIL			RGB(255, 0, 0)
#define CONTINUE_FAIL			RGB(255, 0, 100) //kwlee 2016.1201
#define OPT_FAIL				RGB(255, 50, 100) //kwlee 2016.1201


#define WINDOW_DN				RGB(219, 255, 210)
#define WINDOW_UP				RGB(250, 255, 244)

#define WINDOW_DN1				RGB(219, 255, 210)
#define WINDOW_UP1				RGB(250, 250, 250)

#define BLUE_L					RGB(124, 139, 252)
#define BLACK_GC				0x00
#define YELLOW_GC				0xFFFF
#define PINK_D					RGB(252, 233, 218)
#define COL_PUPLE				RGB(204,102,204)

// board ���� ����
#define BIN_OFF					RGB(164, 164, 164)
#define BIN_NONE				RGB(62,	55, 55)
#define BIN_LOAD				RGB(187, 250, 255)
#define BIN_READY				RGB(150, 255, 150)
#define BIN_START				RGB(255, 255, 0)
#define BIN_ABORT				RGB(204, 102, 204)
#define BIN_GOOD				RGB(124, 139, 252)
#define BIN_FAIL				RGB(255, 0, 0)
#define BIN_RELOAD				RGB(255, 150, 150)

#define NORMAL_MODE				0
#define THRESHOLD_MODE			1

#define HOMING					1
#define MOVING					2
#define MULTI_MOVING			3
#define POS_MOVING				4

#define RET_ERROR				-1
#define RET_READY				0
#define RET_PROCEED				1
#define RET_GOOD				2
#define RET_RETRY				3
#define RET_SAFETY				4
#define RET_ABORT				5
#define RET_SKIP				6
#define RET_RACKSKIP			7

#define RET_PICKER_NOT_FIND		100  //��Ŀ�� ���̻� �۾��� ������ ������ 
#define RET_TRAY_NOT_FIND		200  //Ʈ���̿� ���̻� �۾��� ������ ������ 

/* ***************************************************************************** */
/* ȭ�� �� Ŭ���� ID ���� �κ�                                                   */
/* ***************************************************************************** */
#define IDW_SCREEN_MAIN					101
#define IDW_SCREEN_INIT					102

#define IDW_SCREEN_BASIC		 		201

#define IDW_SCREEN_WORK_INFO			301
#define IDW_SCREEN_SET_MAINTENANCE		302
#define IDW_SCREEN_SET_INTERFACE		303
#define IDW_SCREEN_SET_MEASURE			304
#define IDW_SCREEN_SET_RECIPE			305
#define IDW_SCREEN_SET_RFID				306
#define IDW_SCREEN_SET_FTP				307

#define IDW_SCREEN_SET_COK				308
#define IDW_SCREEN_SET_PART_NO			309
#define IDW_SCREEN_IO_FUNCTION			310
#define IDW_SCREEN_TEST_SITE			311
#define  IDW_SCREEN_SET_BARCODE         312
#define IDW_SCREEN_WAIT_TIME			401
#define IDW_SCREEN_VISION_TEACH			402 //kwlee 2016.0913

#define IDW_SCREEN_MOTOR				501
#define IDW_SCREEN_MOTOR_SPEED			502

#define IDW_SCREEN_LIST					601
#define IDW_SCREEN_LIST_WORK			602
#define IDW_SCREEN_LIST_ALARM			603
#define IDW_SCREEN_LIST_STEP			605
#define IDW_SCREEN_LIST_ERROR			606
#define IDW_SCREEN_LIST_DATA_VIEW		607

#define IDW_SCREEN_IO_MAP				701

#define IDW_SCREEN_LOCK					801

#define IDW_SCREEN_ALARM				901

#define IDW_SCREEN_PGM_INFO				1001

#define SCREEN_MAIN						0
#define SCREEN_INITIAL					1
#define SCREEN_BASIC					2
#define SCREEN_SETTING					3
#define SCREEN_MOTOR					4
#define SCREEN_IO						5
#define SCREEN_LIST						6
#define SCREEN_ALARM					7
#define SCREEN_LOCK						8

// Yasunaga ���� Define
#define Y_INITIALIZE					0//�ʱ�ȭ 0�̿Ϸ�: , 1:�Ϸ����
#define Y_MODELSELECT					1//�� ���� 0:�̿Ϸ�, 1: �Ϸ����
#define Y_RETURNTOORIGIN				2//�������� 0:�̿Ϸ�,1:�Ϸ����
#define Y_AUTOMODE						3//�ڵ� ��� 0:OFF, 1:ON
#define Y_CALIBRATION					4//calibration  0:�̿Ϸ�, 1:�Ϸ����
#define Y_PACKAGEDATA					5//��Ű�� ������0:�̿Ϸ�, 1:�۽ſϷ����
#define Y_AXISDATA						6//�൥����0:�̿Ϸ�, 1: �����Ϸ����

#define L_BUTTON_DOWN					0
#define L_BUTTON_UP						1

#define MACHINE_MANUAL					0
#define MACHINE_AUTO					1

#define LOT_CLEAR						0 //2015.0303 james -1
#define LOT_START						1
#define LOT_END_START					2
#define LOT_END							3

#define HOST_WAIT						0
#define HOST_STOP						1
#define HOST_RUN						2

#define MAXMOTOR						29

//2015.1005
#define  WM_DAY1_DATA					1
#define  WM_DAY3_DATA					2
#define  WM_DAY7_DATA					3
#define  WM_MONTH1_DATA					4
#define  WM_MONTH2_DATA					5
#define  WM_MONTH3_DATA					6
#define  WM_MONTHCAL_DATA				7
#define  WM_FILLRATIO_DATA				8
#define  WM_NOW_DATA					9
//kwlee 2016.0913
enum ENUM_MAIN_SCR_MSG
{
	MAIN_TEACH_VISION = 0,
};

//
struct tagHANDLER_INFO
{
	CBitmap m_TitleBackBitmap;
	CBitmap m_BackBitmap;
	CBitmap m_BackDlgBitmap;

	BOOL bProgramExit;				//���α׷��� ���������°��� Ȯ���Ѵ�.
	// **************************************************************************
	// LOT ó�� ���� �÷��� ���� ��� ����                                       
	// **************************************************************************
	bool bLoaderOnFlag;				// �δ� On : True    �δ� Off : False
	bool bAllLotStartFlag;			// LOT ó�� ���� �� ���� ��
	// **************************************************************************

	int nMenuNum;					// �� ȭ�鿡 ������ ȭ�� ��ȣ ���� ���� 
	int nAlarmScreen;				// ���� �˶� ȭ���� ��µǾ��� �ִ����� ���� ���� ���� ���� 
	int nLoadState;					// ���� �ε� ��ư ���� ��ȭ ���� ��� ���� �÷��� ���� ���� 
	int nRetestRequest;				// ���� ����� ���׽�Ʈ �ϴ°��� �����.
	// *************************************************************************
	// ���� ��ġ ���� ��� ��� �� ���Ǵ� ���� ����                           
	// *************************************************************************
	int nHomingRequest[MAXMOTOR];	// ���� Home Check ��û �÷��� ���� ���� 
	int nMenuLock;					// �޴� ��� ���� ���� �÷��� 
	int nSystemLock;				// SYSTEM LOCK ���� �÷���
	int nSwitchLock;				// Dialog�� �� �ִ� ���� �÷��� (Start Switch Lock)	
	int nMaintMode;					// ����Ʈ ��� ���� �÷���
	int nLevelMaint;				// ����Ʈ ����(Level 1) ���� �÷���
	int nLevelTeach;				// ƼĪ ����(Level 2) ���� �÷���
	int nLevelAdmin;				// ������ ����(Level 3) ���� �÷���
	int nLevelSpeed;
	int nIoBoardInitial;			// IO ���� �ʱ�ȭ �۾� �Ϸ� �÷���
	int nMotorBoardInitial;			// ��� ���� �ʱ�ȭ �۾� �Ϸ� �÷���
	int nMatroxBoardInitial;		// matrox �ʱ�ȭ
	int nIsAllSiteInitEnd;			// ���� �ʱ�ȭ �۾� �Ϸ� Ȯ�� �÷���
	int nRunStatus;
	int nIdleStatus;
	int nManualSpeed;
	int nRunSpeed;
	int nMachineMode;
	int nLotStatus;					// Lot start ��������....
	int	nInitRetry;					// initialize retry �۾� ����.......
	int nInitialSuccess;			// Initialize �۾� ��������.......	
	
	bool bEqpStatus;
	bool bInOutCheck[2]; //kwlee 2015.1126

	double dCurrCmdPos[MAXMOTOR];	// ���� ���� ��ġ ���� ���� ���� 
	// *************************************************************************

	// *************************************************************************
	// ȭ�鿡 ���� �ڵ� ���� ���� ���� ����                                     
	// *************************************************************************
	HWND hWnd;						// ���� ������ �ڵ� ���� ����

	CWnd *cWndMain;					// ���� ȭ�� �ڵ� ���� ����
	CWnd *cWndBasic;				// BASIC ȭ�� �ڵ� ���� ����
	CWnd *cWndInitial;
	CWnd *cWndMaintenance;			// Maintenance ȭ�� �ڵ� ���� ����	
	CWnd *cWndWaitTime;				// Wait Time ȭ�� �ڵ� ���� ����
	CWnd *cWndMotorSpeed;			// Motor Speed ȭ�� �ڵ� ���� ����
	CWnd *cWndIO;					// IO ȭ�� �ڵ� ���� ����
	CWnd *cWndMotor;
	CWnd *cWndTitle;				// Ÿ��Ʋ ��� ȭ�� �ڵ� ���� ����
	CWnd *cWndList;					// ����Ʈ ��� ȭ�� �ڵ� ���� ����
	CWnd *cWndAlarm;
	CWnd *cWndLock;
	CWnd *cWndWork;
	CWnd *cWndFtp;
	// *************************************************************************

	CString	strUserId;
	
	CTime		tRef;
	
	CTimeSpan	tRun;				// Run Time
	CTimeSpan	tJam;				// Jam Time
	CTimeSpan	tStop;				// Stop Time
	CTimeSpan	tMaint;				// Maint Time
	CTimeSpan	tDRun;				// Run Time
	CTimeSpan	tDJam;				// Jam Time
	CTimeSpan	tDStop;				// Stop Time
	CTimeSpan	tDMaint;			// Maint Time
	CTimeSpan	tWork[20][2];		// Work Time
	// jtkim 20150529
	CTimeSpan	tRunUph;
	CString		strHourUph;

	COleDateTime tCreate;
	COleDateTime tUph;

	int			nPgmVer;
	int			nFtpVer;			// jtkim 20150530 
	int			nVerUpdate;			// jtkim 20150604
	int			nAutoLine;			// jtkim 20150627

	//2015.1005
	CTimeSpan m_tDRef;
	CTimeSpan m_tDRdown[TSITE_SOCKET_CNT];
	CTimeSpan m_tDUserStop[TSITE_SOCKET_CNT];
	CTimeSpan m_tDStop[TSITE_SOCKET_CNT];
	CTimeSpan m_tDInStandby[TSITE_SOCKET_CNT];
	///////
	CTimeSpan m_tDtest[TSITE_SOCKET_CNT];
	CTimeSpan m_tDSokOff[TSITE_SOCKET_CNT];
	CTimeSpan m_tDOutStandby[TSITE_SOCKET_CNT];
	//kwlee 2015.1126
	CTimeSpan m_tStopTime[2][TSITE_SOCKET_CNT];

	int				mn_Pass[TSITE_SOCKET_CNT];
	int				mn_Fail[TSITE_SOCKET_CNT];
	int				mn_Input[TSITE_SOCKET_CNT];
	int             mn_Output[TSITE_SOCKET_CNT];
	int             mn_Yield[TSITE_SOCKET_CNT];


	int mn_allnewlot;
// 	int mn_InstandyBy[TSITE_SOCKET_CNT];
// 	int mn_InstandyBy_1[TSITE_SOCKET_CNT];
// 	int mn_OutstandyBy[TSITE_SOCKET_CNT];

	int mn_userstop;
	bool isTestRun[TSITE_SOCKET_CNT];
	CString mstr_DirPath;


	// jtkim 20160111
	int nDlgLock;
	int	m_iVisionOffSetAutoMode; //kwlee 2016.0913
	CString mstr_event_msg[3];

	int nAutoDirectionUseChk; //kwlee 2016.1124
	int nModelChangeCheck; //kwlee 2016.1220
	// jtkim 20160927
	int nAutoMode;
	//kwlee 2016.1020
	long m_tlDooropen[8][3];

	int mn_NoInput_test;//2016.1228

	//2017.0116
	int mn_removetray;
	int mn_uldnum;

	///////////////////////////////////////////////////
	//2017.0430
	int mn_auto_empty_tray;
	int mn_out_empty; //�ϴ� ������ 5���� ������. ������ ���� ���
	///////////////////////////////////////////////////
};
extern  tagHANDLER_INFO  st_handler_info;
// *****************************************************************************
// *************************************************************************
// RS-232C �ø��� ��� ���� ���� ����ü ����                                     
// *************************************************************************
struct tagSERIAL_INFO
{
	int	nConnect[MAX_PORT];
	int nSndChk[MAX_PORT];				// �޽��� �۽� �Ϸ� �÷���
	int nRecChk[MAX_PORT];				// �޽��� ���� �Ϸ� �÷���
	
	CString strPortChk[MAX_PORT];		// �ø��� ��Ʈ ���� ���� ���� ����

	CString strSnd[MAX_PORT];			// �۽� �޽��� ���� ����
	CString strRec[MAX_PORT];			// ���� �޽��� ���� ����

	CString strCommSnd[MAX_PORT];		// ����Ʈ ȭ�� ��� �۽� �޽��� ���� ����
	CString strCommRec[MAX_PORT];		// ����Ʈ ȭ�� ��� ���� �޽��� ���� ����
	CString strCommErr[MAX_PORT];		// ����Ʈ ȭ�� ��� ���� �޽��� ���� ����

	int nSerialPort[10];
	int nSerialBaudrate[10];
	int nSerialData[10];
	int nSerialParity[10];
	int nSerialStop[10];
};
extern  tagSERIAL_INFO  st_serial_info;
// *************************************************************************

#define _MAX_ALARM_CNT		2000

// *************************************************************************
// �߻��� �˶� ���� ������ ���� ����ü ����                                      
// *************************************************************************
struct tagALARM_INFO
{
	// **************************************************************************
	// �˶� ���� ǥ�� ��ũ [��� ��ġ], [ũ��], [Į��] ���� ����                 
	// **************************************************************************
	int nOffset;
	int nCy;
	int nCx;
	int nLineWidth;
	int nR;
	int nG;
	int nB;
	// **************************************************************************

	// **************************************************************************
	// ���Ͽ� ����� �˶� ���� ���� ����                                         
	// **************************************************************************
	CString strEContent[_MAX_ALARM_CNT];  // ���Ͽ��� �ε��� �� ���� ���� ���� ����
	CString strECode[_MAX_ALARM_CNT];     // �˶� �ڵ� ���� ����
	CString strEState[_MAX_ALARM_CNT];    // �˶� ���� ���� ����
	CString strEPart[_MAX_ALARM_CNT];     // �˶� �߻� ���� ���� ����
	CString strEMsg[_MAX_ALARM_CNT];      // ���� ���� �˶� �޽��� ���� ����
	// **************************************************************************

	// **************************************************************************
	// ȭ�鿡 ����� �˶� ���� ���� ����                                         
	// **************************************************************************
	int nActionCnt;            // ����� �˶� ��ġ ���� ���� ���� ����
	CString strStyle;           // �˶� ��� ���� ���� ����
	CString strBmpFile;        // �˶� ǥ�� �׸� ���� ���� ����
	CString strRepairMsg;      // �˶� ��ġ ���� ���� ����
	// **************************************************************************

	CStringList strListJam;		// �߻��� Jam Code�� ����Ʈ
	CString strCode;			// �߻��� �˶� �ڵ� ���� ����
	CString strPcode;			// �߻��� �˶� �ڵ� ���� ����
	CString strCurrMsg;
	CString strCurPart;
	CString strContent;

	int nCountMode;				// �˶� ī��Ʈ �÷��� (0:�˶� ī��Ʈ ������    1:�˶� ī��Ʈ ����)
	int nTypeMode;				// �˶� ���� ���� ���� (0:���    1:���༺    2:��� ����)

	long stl_cur_alarm_time;	// �˶� �߻� �ð� ���� ����
	int stl_prev_code;			// �ٷ� �� �߻��� �˶� �ڵ� ���� ����
	long stl_prev_alarm_time;	// �ٷ� �� �߻��� �˶� �߻� �ð� ���� ����

	int nAlarmOccured;			// �˶� �߻� Ƚ�� ���� ����
	int nAlarmOccuredDay;		// �˶� �߻� Ƚ�� ���� ����
	int nArmLotContinue;		// LOT ��� ���� �÷���
	int nResetStatus;			// Alarm Reset Status
	int nSkipMode;
	int nMotorAlarm;			// MOTOR���� �˶��� �ѹ��� �߻��ϵ��� �ϱ� ����!!  --> run�õ������� �ٽ� �︮�� ����!
	int nAlarmAssignSection;	//�˶��� �߻��� ���� ���ݴ� ���� ã�� ����
	int nCurState;
	int nAlarmClear;
	int nAlarmNum;

	int nAlarmCountClear;			// alarm ���� clear
};	
extern  tagALARM_INFO  st_alarm_info;
// *************************************************************************

// *************************************************************************
// �˶� ������ ���� ����ü ����                                                  
// *************************************************************************
struct tagALARM_DISPLAY_INFO
{
	// **************************************************************************
	// �˶� ���� ǥ�� ��ũ [��� ��ġ], [ũ��], [Į��] ���� ����                 
	// **************************************************************************
	int nOffset;
	int nCy;
	int nCx;
	int nLineWidth;
	int nR;
	int nG;
	int nB;
	// **************************************************************************

	// **************************************************************************
	// ȭ�鿡 ����� �˶� ���� ���� ����                                         
	// **************************************************************************
	int nActionCnt;            // ����� �˶� ��ġ ���� ���� ���� ����
	CString strStyle;           // �˶� ��� ���� ���� ����
	CString strBmpFile;        // �˶� ǥ�� �׸� ���� ���� ����
	CString strRepairMsg;      // �˶� ��ġ ���� ���� ����
	// **************************************************************************

	// **************************************************************************
	// ���Ͽ� ����� �˶� ���� ���� ����                                         
	// **************************************************************************
	CString strContent[2000];  // ���Ͽ��� �ε��� �� ���� ���� ���� ����
	CString strCode[2000];     // �˶� �ڵ� ���� ����
	CString strState[2000];    // �˶� ���� ���� ����
	CString strPart[2000];     // �˶� �߻� ���� ���� ����
	CString strMsg[2000];      // ���� ���� �˶� �޽��� ���� ����
	// **************************************************************************

	// **************************************************************************
	// ���� �߻��� �˶� ���� ����                                                
	// **************************************************************************
	int nCurrState;             // �˶� ���� ���� �÷���
	CString strCurrMsg;         // �߻��� �˶� �޽��� ���� ����
	// **************************************************************************
};
extern  tagALARM_DISPLAY_INFO  st_alarm_display_info;


// **********************************************************************************
// ������ ȭ�� ȯ�� ���� ���� ���� ����ü ����                                   
// **********************************************************************************
struct tagBASIC_INFO
{
	//int		nModeTesterDisable; //teser �����/��(0:tester���, 1:������� ����(���Ͽ������� ��������)
	//kwlee 2016.0930
	int		nUldGoodTrayStack_Count; //��ε� ���¤�Ŀ�� �����ϴ� ���� ���� (�ܵ� ��� ���� ����)
	//int		nStackerTray_Count; //��ε� ���¤�Ŀ�� �����ϴ� ���� ���� (�ܵ� ��� ���� ����)
	//

	int     nPickerErrorSkipMode; //0:��ŵ���� ����, 1:�ڵ����� ��ŵ�ϸ鼭 �̵� 
	int		nCtrlMode;				// [Title Bar ���� ǥ��] < ONLINE, REMOTE, LOCAL.. ǥ��	> 
	int		nModeDevice;			// [Title Bar ���� ǥ��] < WHIT/WHIT OUT �� MODE ǥ��	>
	int		nModeWork;				// [Title Bar ���� ǥ��] < �۾� ��� ���� ǥ��			>
	int		nModeXgem;
	int		nModeInterface;
	int		nModeJigStack;			// unloader jig stack ���.....
	int		nModeRfid;				// rfid �������.....
	int		nModeBarcode;			// barcode read ����....
	int		nMode1DBcr;				// Load Barcode Read ����...
	int		nMode2DBcr;			// unloader barcode read ����.....
	int		nModeLdUldLightCutainSkip; //Ld/Uld Light Cutain Skip ����..
	int 	nModeRejLightCutainSkip;  //Rej Light Cutain Skip ����...
	int		nRetry;					// retry ����....
	int		nPartialPickCount;
	int		nModeJigLevel;
	int		nModeFrontSmema;
	int		nModeRearSmema;
	int		nModeFtpGms;			// GMS FTP ��������.....
	int		nVnRTime;
	int		nPcbArray;				// ���迭 ����.....
	int		nModeHook;
	int		nModeXgemRunChk;		// 20140806 jtkim..... rcmd check.....
	int		nModeXgemInterface;		// xgem interface [0] off line [1] on line local [2] on line remote.....
	int		nPartNoDgt;
	bool	bEnableFlag;

	// jtkim 20160927
	int		nModeTestInterface;		// tester �������̽�.......

	CString strDeviceName;			// [Title Bar ���� ǥ��] < ��񿡼� ������ DEVICE ǥ��	>
	CString strModelName;			// [Title Bar ���� ǥ��] < �������� ������ JOB ���� ǥ�� >

	CString strNotice;				// Notice...

	CString	strEqp;					// eqp id.........
	CString strOperID;				// operator id.....
	CString strToolName;			// tool name.....
	CString strFtpUser;				// ftp ����ڸ�......
	CString strFtpPass;				// ftp ��ȣ......
	CString strDevice;
	CString strVnrSpec[2][8];
	CString strVnrUcl[2][8];
	CString strVnrLcl[2][8];

	//2015.0302 ���� �߰��� 
	int		nMp_Docking_Sensor_Disable; //2015.0302 CTL_YES: M/P�� ������ ���� ���·� ���� 
	int		nRotatorBuffer_IDCheck_Sensor_Disable; //2015.0302 CTL_YES: M/P�� ������ ���� ���·� ���� 

	CString strHifixType[10];

	int nLastMdlCount;	// jtkim 20150501 last mdl count....
	int nBcrRead;		// jtkim 20150508
	int nBcrFail;		// jtkim 20150514 ��â�ƾ� ��û

	CString strCodePath;
	// jtkim 20150529
	CString strScrapName;
	CString strScrapHead;

	CString strProcess;	// jtkim 20150623 in-line

	// jtkim 20150721
	int nSocketCount;

	int nRowCnt;
	int nColCnt;
	int nGmsTime;

	int nRejectStackerTray_Cnt; //kwlee 2017.0613

	// jtkim 20150930
	int nRecipeYesNo;	// recipe download.....
	int nBarcodeBlock;
	int nAlarmDelayCnt; //kwlee 2016.1124
	int nDirectionCheckSkip; //kwlee 2016.1124
	// jtkim 20151229
	int nRetestCount;
	int nAutoLoadMode; //kwlee 2016.1201
	int nLotEndSkipMode;//kwlee 2017.0224
	/////////////////////
	CString strGmeMsg[10];
	int nPort[10];
	int nMCID;
	double dVolt[10];
	double dResistance[10];

} ;
extern  tagBASIC_INFO  st_basic_info;
// ******************************************************************************

// **********************************************************************************
// ������ ȭ�� ȯ�� ���� ���� ���� ����ü ����                                   
// **********************************************************************************
struct tagRECIPE_INFO
{
	////////////////////////////////////////
	// ����ϴ� ���� 
	///////////////////////////////////////
	int	nTrayY;						// tray Y ����.......
	int nTrayX;						// tray X ����.......
	int nRetestBuffer_Num; //2016.0810
	int nLdUldBuffer_Num;//2016.0810
	int nTestSocket_Num;				//james 2016.0808,tester �� ���� ����
	double dLdUldPickerPitch;		//picker pitch 
	double dTestPickerPitch;		//picker pitch 
	double dTrayYOffset;			// tray Y pitch.......
	double dTrayXOffset;			// tray X pitch.......

	double dTrayPitch_Y;			//Ʈ���� ��ġ���� 
	double dTrayPitch_X;			//Ʈ���� ��ġ���� 

	double dLdUldBufferPitch;			//���� ��ġ���� 
	double dRetestBufferPitch;	//retest buffer pitch ��

	double dVisionXOffset; // Vision X Offset
	double dVisionYOffset; // Vision Y Offset

	int	nTestRetest_Count;
	int nBarcodeCnt;

	////////////////////////////////////////
	// ���� ������� �ʴ� ���� 
	///////////////////////////////////////

	int		nBufferRotator_TestSite_Degrees; // ld/unld ���� ��� . 0:0��, 1:90��, 2:180��, 3:270�� ���� ���� ���� 
	int		nBufferRotator_PlateSite_Degrees; // ld/uld ���� ��� . ������ 0���� �����̴� 
	int		nFailCnt;							//���� Fail�϶� Socket Off �Ǵ� ī��Ʈ.
	//kwlee 2016.0809
	int		nTestSite_Top_Degrees; // CW/CCW
	int		nTestSite_Btm_Degrees; // CW/CCW

	// jtkim 20151017
	int		nBufferRotator_Barcode_Degrees; 
	int Buffer_DvcCheckSensor_Mode;    //���� ����̽�üũ ���� ��� ��/�� 
	int nCOKBuff_IDNum_Type;			////2015.0305 ������ 1�� ���� 4.:2.5" reverse, 3:2.5" Front, 2:Msata, 1:pogo type //���� �۾��� COK ID ����, buffer id�� �����Ѵ�, (COK Buff, Rotator Buff, Test site COK ������ �� ��ȣ�� ���� �����ؾ� �Ѵ� 	

	int nTsiteUse[2][2][12];		// test site socket on/off ����....[0]����(0)/����[1] [0]�ϴ�(0)���(1) [0]SLOT
	

	int nRetestTray_X; //2015.0222 retest tray�� ���� X �������� ����� Ʈ������ ����
	int nRejectTray_X; //2015.0222 reject tray�� ���� X �������� ����� Ʈ������ ����

	int nBufferY;					//james 2016.0808 ������� ���� 
	int nBufferX;					//james 2016.0808 ������� ���� 

	int nRetestBufferY;					//buffer y ���� //james 2016.0808 ������� ���� 
	int nRetestBufferX;					//buffer x ����

	int nHifix;						// hifix ��ȣ.....
	int	nAbortTime;					// test timeout......
	int nCokType[4];

	
	double dBufferPitch_Y;			//���� ��ġ���� 
	double dBufferPitch_X;			//���� ��ġ���� 
	double dTestPitch_X;			//���� ��ġ���� 

	//james 2016.0822
	

	double dCOKBuffPitch_X;			//COK ���� ��ġ���� 

	//2015.0206 ���͸� X �������� ���� double dLdUldPickerPitch_Y;		//picker pitch 
	

	
	double dTSiteOffsetX;			// test site x offset.....

	double dRejTrayPick;			// Reject Tray Pick Offset...
	double dRejTrayPlace;			// Reject Tray Place Offset...

	CString strHifix;				// hifix name.....
	CString strCokTypeName[8];

	bool	bCokChk[8][3];

	BOOL	bUnldBcrFlag[2];		// Unload �� Barcode A, B ������� �÷���

	int		nBcrBank;				// barcode bank......

	double	dTsiteOffsetY[12];	// test site offset y.......

	CString strPartNo;

	// jtkim
	int		nAbortPer;
	int		nIndividualPick;

	double	d15TOffset;
 
	// jtkim 20160901
	int		nBarcodeCount;
};
extern tagRECIPE_INFO	st_recipe_info;
// ******************************************************************************

// *****************************************************************************
//  Tower Lamp ȭ�鿡 ���� ���� ���� ����ü ���� ����                           
// *****************************************************************************
struct tagLAMP_INFO
{
	int nTFlick;
	int nLampR[9];				// RED ���� ���� ���� ����
	int nLampY[9];				// YELLOW ���� ���� ���� ����
	int nLampG[9];				// GREEN ���� ���� ���� ����
	int nLampB[9];				// GREEN ���� ���� ���� ����

	int nInterest;				// BLUE ���� ���� ���� ����

	CString strEquipNo;			// ��� ȣ�� ���� ����
	CString strEquipCode;		// ��� �ڵ� ���� ����

	int nBuzzerMode;			// ���� ��� ��� ���� ����
	int nStatusBuzzerRing;		// ���� �︮�� �ִ� ���� ���� 
	long lBuzzerOffWait[3];		// ���� ���� �ð� �︮�� ���ߵ��� �ϴ� ���� 

	int nLampWaitTime;			// ���� FLACKER �ð� ���� ����

	int nModeLampBlue;			// ������� ���� ���� ��뿩��
	int nModeLampBlueFlicker;	// ������� ���� ���� FLACKER ����

	CString str_ftp_path;
	CString str_ftp_ip;
	CString str_ftp_user;
	CString str_ftp_pw;
	CString str_ftp_scrap_code_name;
	CString str_ftp_recipe_rule_name;
	CString str_mode_ftp;
	
	int n_ftp_port;
	int n_mode_ftp;

};
extern  tagLAMP_INFO st_lamp_info;
// *****************************************************************************

enum LAMP_CTRL
{
	LAMP_ON		= 1,
	LAMP_OFF	= 2,
	LAMP_FLICK	= 3,
};

struct tagIO_INFO
{
	int o_ReadyLd1CvyStkStopUpDnCyl;		// S0000
	int o_ReadyLd2CvyStkStopUpDnCyl;		// S0001
	int o_ReadyLd1CvyAcMotOnOff;			// S0002
	int o_ReadyLd2CvyAcMotOnOff;			// S0003
	// S0004
	// S0005
	// S0006
	int i_ReadyLd1CvyInTrayChk;				//PS0000
	int i_ReadyLd1TrayExistChk;				//PS0001
	int i_ReadyLd1CvyStopUpChk;				//PS0002
	int i_ReadyLd1CvyStopDnChk;				//PS0003
	int i_ReadyLd2CvyInTrayChk;				//PS0004
	int i_ReadyLd2TrayExistChk;				//PS0005
	int i_ReadyLd2CvyStopUpChk;				//PS0006
	int i_ReadyLd2CvyStopDnChk;				//PS0007
	int i_ReadyCvyLd12OutTrayPassChk;		//PS0008

	int o_ReadyLdCvyStkStopUpDnCyl[2];
	int i_ReadyLdCvyStkStopUpDnCyl[2][2];

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 01
	///////////////////////////////////////////////////////////////////////////////////////////////
	int o_LdTrayPlateLfOnOffCyl;			// S0100
	int o_LdTrayPlateRiOnOffCyl;			// S0101
	int o_LdStkUpCyl;						// S0102		
	int o_LdStkDnCyl;						// S0103		
	int o_LdStkAcMotOnOff;					// S0104	
	// S0105
	// S0106
	// S0107
	int i_LdTrayPlateLfOnChk; 				//PS0100
	int i_LdTrayPlateLfOffChk;				//PS0101
	int i_LdTrayPlateRiOnChk;				//PS0102
	int i_LdTrayPlateRiOffChk;				//PS0103
	int i_LdStkUpChk;						//PS0104		
	int i_LdStkDnChk;						//PS0105		
	int i_LdStkTrayChk;						//PS0106
	int i_LdStkTrayArrivedChk;				//PS0107
	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 02
	///////////////////////////////////////////////////////////////////////////////////////////////
	int o_EmptyStkUpCyl;					// S0200	
	int o_EmptyStkDnCyl;					// S0201	
	int o_ReadyUld1CvyStopCyl;				// S0202
	int o_ReadyUld2CvyStopCyl;				// S0203
	// S0204
	int i_EmptyStkUpChk;					//PS0200	
	int i_EmptyStkDnChk;					//PS0201
	int i_EmptyStkTrayChk;					//PS0202
	int i_ReadyUld1CvyPassTrayChk;			//PS0203
	int i_ReadyUld1CvyOutTrayChk;			//PS0204
	int i_ReadyUld1CvyStopUpChk;			//PS0205
	int i_ReadyUld1CvyStopDnChk;			//PS0206
	int i_ReadyUld2CvyPassTrayChk;			//PS0207
	int i_ReadyUld2CvyOutTrayChk;			//PS0208
	int i_ReadyUld2CvyStopUpChk;			//PS0209
	int i_ReadyUld2CvyStopDnChk;			//PS0210

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 03
	///////////////////////////////////////////////////////////////////////////////////////////////
	int o_Uld1StkUpCyl;						// S0300		
	int o_Uld1StkDnCyl;						// S0301	
	int o_Uld2StkUpCyl;						// S0302	
	int o_Uld2StkDnCyl;						// S0303	
	int o_Uld1StkAcMotOnOff;				// S0304
	int o_Uld2StkAcMotOnOff;				// S0305
	// S0306
	// S0307
	int i_Uld1StkUpChk;						//PS0300	
	int i_Uld1StkDnChk;						//PS0301	
	int i_Uld2StkUpChk;						//PS0302	
	int i_Uld2StkDnChk;						//PS0303	
	int i_Uld1StkTrayExistChk;				//PS0304		
	int i_Uld2StkTrayExistChk;				//PS0305


	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 04
	///////////////////////////////////////////////////////////////////////////////////////////////
	int o_TrayTrsfUpCyl;					// S0400
	int o_TrayTrsfDnCyl;					// S0401
	int o_TrayTrsfVacOnOff;					// S0402
	int o_TrayTrsfEjtOnOff;					// S0403
	int o_RejTrsfUpCyl;						// S0404
	int o_RejTrsfDnCyl;						// S0405
	int o_RejTrsfVacOnOff;					// S0406
	int o_RejTrsfEjtOnOff;					// S0407
	int i_TrayTrsfUpChk;					//PS0400
	int i_TrayTrsfDnChk;					//PS0401
	int i_TrayTrsfVacChk;					//PS0402
	int i_RejTrsfUpChk;						//PS0403
	int i_RejTrsfDnChk;						//PS0404
	int i_RejTrsfVacChk;					//PS0405
	//PS0406
	//PS0407

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 05
	///////////////////////////////////////////////////////////////////////////////////////////////
	int o_Rej1UldCvyStopUpDnCyl;			// S0500
	int o_Rej2UldCvyStopUpDnCyl;			// S0501
	int o_RejEmptyStkUpCyl;					// S0502
	int o_RejEmptyStkDnCyl;					// S0503
	int o_Rej1StkUpCyl;						// S0504
	int o_Rej1StkDnCyl;						// S0505
	int i_Rej1UldCvyStopUpChk;				//PS0500
	int i_Rej1UldCvyStopDnChk;				//PS0501
	int i_Rej2UldCvyStopUpChk;				//PS0502
	int i_Rej2UldCvyStopDnChk;				//PS0503
	int i_RejEmptyStkUpChk;					//PS0504
	int i_RejEmptyStkDnChk;					//PS0505
	int	i_Rej1StkUpChk;						//PS0506
	int	i_Rej1StkDnChk;						//PS0507

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 06
	///////////////////////////////////////////////////////////////////////////////////////////////
	int o_RejEmptyCvyStkAcMotOnOff;			// S0600
	int o_Rej1UldCvyStkAcMotOnOff;			// S0601
	int o_Rej2UldCvyStkAcMotOnOff;			// S0602
	int	o_Rej2StkUpCyl;						// S0603
	int o_Rej2StkDnCyl;						// S0604
	// S0605
	int i_RejEmptyStkTrayExistChk;			// PS0600
	int i_RejEmptyCvyInTrayExistChk;		// PS0601
	int i_RejEmptyTrayArrivedChk;			// PS0602
	int i_Rej1UldCvyTrayExistChk;			// PS0603
	int i_Rej1StkTrayExistChk;				// PS0604
	int i_Rej2UldCvyTrayExistChk;			// PS0605
	int i_Rej2StkTrayExistChk;				// PS0606
	int	i_Rej2StkUpChk;						// PS0607
	int	i_Rej2StkDnChk;						// PS0608
	//PS0609

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 07
	///////////////////////////////////////////////////////////////////////////////////////////////
	int o_LdUldPickUpCyl;						// S0700
	int o_LdUldPickDnCyl;						// S0701
	int o_LdUldPickRot_0_Cyl;					// S0702
	int o_LdUldPickRot_90_Cyl;					// S0703
	int o_LdUldPickGripCyl;						// S0704
	// S0705
	int i_LdUldPickUpChk;						//PS0700
	int i_LdUldPickDnChk;						//PS0701
	int i_LdUldPickRot_0_Chk;					//PS0702
	int i_LdUldPickRot_90_Chk;					//PS0703
	int i_LdUldPickDvcChk;						//PS0704
	//PS0705
	//PS0706
	//PS0707


	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 10
	///////////////////////////////////////////////////////////////////////////////////////////////
	int o_LdBuff1GripOnOffCyl;				// S1000
	int o_LdBuff2GripOnOffCyl;				// S1001
	int o_LdBuff3GripOnOffCyl;				// S1002
	int o_LdBuff4GripOnOffCyl;				// S1003
	int o_UldBuff1GripOnOffCyl;				// S1004
	int o_UldBuff2GripOnOffCyl;				// S1005
	int o_UldBuff3GripOnOffCyl;				// S1006
	int o_UldBuff4GripOnOffCyl;				// S1007
	int i_LdBuff1GripDvcExistChk;			//PS1000
	int i_LdBuff2GripDvcExistChk;			//PS1001
	int i_LdBuff3GripDvcExistChk;			//PS1002
	int i_LdBuff4GripDvcExistChk;			//PS1003
	int i_UldBuff1GripDvcExistChk;			//PS1004
	int i_UldBuff2GripDvcExistChk;			//PS1005
	int i_UldBuff3GripDvcExistChk;			//PS1006
	int i_UldBuff4GripDvcExistChk;			//PS1007

	int o_LdBuffGripOnOffCyl[4];
	int o_UldBuffGripOnOffCyl[4];
	int i_LdBuffGripDvcExistChk[4];
	int i_UldBuffGripDvcExistChk[4];

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 11
	///////////////////////////////////////////////////////////////////////////////////////////////
	int i_Ret1BuffDvcExistChk;				//PS1100
	//PS1101
	//PS1102
	//PS1103
	//PS1104
	//PS1105
	//PS1106
	//PS1107
	int i_Ret2BuffDvcExistChk;				//PS1108
	//PS1109
	//PS1110
	//PS1111
	//PS1112
	//PS1113
	//PS1114
	//PS1115

	//int i_Ret1BuffDvcExistChk[8];
	//int i_Ret2BuffDvcExistChk[8];

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 12
	///////////////////////////////////////////////////////////////////////////////////////////////
	int o_LdUldMuteOn;						// S1200
	int o_LdUldMuteOff;						// S1201
	int o_RejMuteOn;						// S1202
	int o_RejMuteOff;						// S1203
	// S1204
	// S1205
	// S1206
	// S1207
	int i_LdUldMuteOnChk;					//PS1200
	int i_LdUldMuteOffChk;					//PS1201
	int i_RejMuteOnChk;						//PS1202
	int i_RejMuteOffChk;					//PS1203
	int i_LdUldStkLctChk;					//PS1204
	int i_RejStkLctChk;						//PS1205
	int i_LdUldLightChk;					//PS1206
	int i_RejectLightChk;					//PS1207
	
	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 13
	///////////////////////////////////////////////////////////////////////////////////////////////
	int o_StartLamp;						// S1300
	int o_StopLamp;							// S1301
	int o_AlarmLamp;						// S1302
	int o_BuzzLamp;							// S1303
	//S1304
	//S1305									
	//S1306									
	//S1307
	int i_StartChk;							//PS1300
	int i_StopChk;							//PS1301
	int i_AlarmChk;							//PS1302
	int i_BuzzChk;							//PS1303
	int i_AutoModeChk;						//PS1304
	int i_ManualModeChk;					//PS1305
	int i_FrontSelectSwChk;					//PS1306
	int i_RearSelectSwChk;					//PS1307
	

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 14
	///////////////////////////////////////////////////////////////////////////////////////////////
	// S1400
	int o_TwGreenLamp;						// S1401
	int o_TwYellowLamp;						// S1402
	int o_TwRedLamp;						// S1403
	int o_Buzz1OnOff;						// S1404
	int o_Buzz2OnOff;						// S1405
	int o_Buzz3OnOff;						// S1406
	int o_DoorLockLdUld;					// S1407
	//PS1400	
	int i_MainAirChk;						//PS1401	
	int i_Emo1SwChk;						//PS1402
	int i_Emo2SwChk;						//PS1403
	int i_Emo3SwChk;						//PS1404	
	int i_Emo4SwChk;						//PS1405	
	//PS1406	
	//PS1407	

	int o_TwLamp[4];
	int o_BuzzOnOff[4];
	int i_EmoChk[4];

	int oMotPwr[10];

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 15
	///////////////////////////////////////////////////////////////////////////////////////////////	
	int i_TopLdUldDoor1Chk;						//PS1500
	int i_TopLdUldDoor2Chk;						//PS1501
	int i_TopLdUldDoor3Chk;						//PS1502
	int i_TopLdUldDoor4Chk;						//PS1503
	int i_TopLdUldDoor5Chk;						//PS1504
	int i_TopLdUldDoor6Chk;						//PS1505
	int i_TopLdUldDoor7Chk;						//PS1506
	int i_TopLdUldDoor8Chk;						//PS1507
	int i_TopLdUldDoor9Chk;						//PS1508
	int i_TopLdUldDoor10Chk;					//PS1509
	int i_BtmLdUldDoor1Chk;						//PS1510
	int i_BtmLdUldDoor2Chk;						//PS1511
	int i_BtmLdUldDoor3Chk;						//PS1512
	int i_BtmLdUldDoor4Chk;						//PS1513
	//PS2014
	//PS2015

	int i_TopLdUldDoorChk[10];
	int i_BtmLdUldDoorChk[4];


	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 16
	///////////////////////////////////////////////////////////////////////////////////////////////
	int i_Fan_1StopChk;							//PS1600
	int i_Fan_2StopChk;							//PS1601
	int i_Fan_3StopChk;						//PS1602
	int i_Fan_4StopChk;							//PS1603
	int i_Fan_5StopChk;							//PS1604
	int i_Fan_6StopChk;							//PS1605
	int i_Fan_7StopChk;							//PS1606
	int i_Fan_8StopChk;							//PS1607
	int i_NonContect_DoorChk_1;				//PS1608
	int i_NonContect_DoorChk_2;				//PS1609
	//PS1610
	//PS1611
	int i_MainBoxSmokeChk;						//PS1612
	int i_ServoBox_2SmokeChk;					//PS1613



	int i_Fan_StopChk[8];


	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 17
	///////////////////////////////////////////////////////////////////////////////////////////////

	//spaere

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 20
	///////////////////////////////////////////////////////////////////////////////////////////////
	int o_Chamber1AcMotClose;				// S2000
	int o_Chamber1AcMotOpen;				// S2001
	int o_Chamber2AcMotClose;				// S2002
	int o_Chamber2AcMotOpen;				// S2003
	int o_Chamber3AcMotClose;				// S2004
	int o_Chamber3AcMotOpen;				// S2005
	int o_Chamber4AcMotClose;				// S2006
	int o_Chamber4AcMotOpen;				// S2007
	int o_Chamber5AcMotClose;				// S2008
	int o_Chamber5AcMotOpen;				// S2009
	int o_Chamber6AcMotClose;				// S2010
	int o_Chamber6AcMotOpen;				// S2011
	int o_Chamber7AcMotClose;				// S2012
	int o_Chamber7AcMotOpen;				// S2013
	int o_Chamber8AcMotClose;				// S2014
	int o_Chamber8AcMotOpen;				// S2015

	int o_ChamberAcMotOpen[8];
	int o_ChamberAcMotCose[8];


	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 21
	///////////////////////////////////////////////////////////////////////////////////////////////
	int o_Tsite1UpCyl;						// S2100
	int o_Tsite1DnCyl;						// S2101
	int o_Tsite2UpCyl;						// S2102
	int o_Tsite2DnCyl;						// S2103
	int o_Tsite3UpCyl;						// S2104
	int o_Tsite3DnCyl;						// S2105
	int o_Tsite4UpCyl;						// S2106
	int o_Tsite4DnCyl;						// S2107

	int i_Tsite1LockOnChk;					//PS2100
	int i_Tsite1ExistChk;					//PS2101
	int i_Tsite1UpChk;						//PS2102
	int i_Tsite1DnChk;						//PS2103
	int i_Chamber1OpenChk;					//PS2104
	int i_Chamber1CloseChk;					//PS2105
	int i_Tsite2LockOnChk;					//PS2106
	int i_Tsite2ExistChk;					//PS2107

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 22
	///////////////////////////////////////////////////////////////////////////////////////////////
	int i_Tsite2UpChk;						//PS2200
	int i_Tsite2DnChk;						//PS2201
	int i_Chamber2OpenChk;					//PS2202
	int i_Chamber2CloseChk;					//PS2203
	int i_Tsite3LockOnChk;					//PS2204
	int i_Tsite3ExistChk;					//PS2205
	int i_Tsite3UpChk;						//PS2206
	int i_Tsite3DnChk;						//PS2207
	int i_Chamber3OpenChk;					//PS2208
	int i_Chamber3CloseChk;					//PS2209
	int i_Tsite4LockOnChk;					//PS2210
	int i_Tsite4ExistChk;					//PS2211
	int i_Tsite4UpChk;						//PS2212
	int i_Tsite4DnChk;						//PS2213
	int i_Chamber4OpenChk;					//PS2214
	int i_Chamber4CloseChk;					//PS2215


	int i_ChamberOpenChk[8];
	int i_ChamberCloseChk[8];
	int i_TsiteLockOnChk[8];
	int i_TsiteExistChk[8];	

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 23
	///////////////////////////////////////////////////////////////////////////////////////////////
	int o_Tsite5UpCyl;						// S2300
	int o_Tsite5DnCyl;						// S2301
	int o_Tsite6UpCyl;						// S2302
	int o_Tsite6DnCyl;						// S2303
	int o_Tsite7UpCyl;						// S2304
	int o_Tsite7DnCyl;						// S2305
	int o_Tsite8UpCyl;						// S2306
	int o_Tsite8DnCyl;						// S2307
	int i_Tsite5LockOnChk;					//PS2300
	int i_Tsite5ExistChk;					//PS2301
	int i_Tsite5UpChk;						//PS2302
	int i_Tsite5DnChk;						//PS2303
	int i_Chamber5OpenChk;					//PS2304
	int i_Chamber5CloseChk;					//PS2305
	int i_Tsite6LockOnChk;					//PS2306
	int i_Tsite6ExistChk;					//PS2307

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 24
	///////////////////////////////////////////////////////////////////////////////////////////////
	int i_Tsite6UpChk;						//PS2400
	int i_Tsite6DnChk;						//PS2401
	int i_Chamber6OpenChk;					//PS2402
	int i_Chamber6CloseChk;					//PS2403
	int i_Tsite7LockOnChk;					//PS2404
	int i_Tsite7ExistChk;					//PS2405
	int i_Tsite7UpChk;						//PS2406
	int i_Tsite7DnChk;						//PS2407
	int i_Chamber7OpenChk;					//PS2408
	int i_Chamber7CloseChk;					//PS2409
	int i_Tsite8LockOnChk;					//PS2410
	int i_Tsite8ExistChk;					//PS2411
	int i_Tsite8UpChk;						//PS2412
	int i_Tsite8DnChk;						//PS2413
	int i_Chamber8OpenChk;					//PS2414
	int i_Chamber8CloseChk;					//PS2415

	int o_TsiteUpCyl[8];
	int o_TsiteDnCyl[8];
	int i_TsiteUpChk[8];
	int i_TsiteDnChk[8];

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 25
	///////////////////////////////////////////////////////////////////////////////////////////////
	int o_TestRbtPicker1GripCyl;			// S2500
	int o_TestRbtPicker2GripCyl;			// S2501
	int o_TestRbtPicker3GripCyl;			// S2502
	int o_TestRbtPicker4GripCyl;			// S2503
	//S2504
	//S2505
	//S2506
	//S2507
	int i_TestRbtPicker1GripDvcExistChk;	//PS2500
	int i_TestRbtPicker2GripDvcExistChk;	//PS2501
	int i_TestRbtPicker3GripDvcExistChk;	//PS2502
	int i_TestRbtPicker4GripDvcExistChk;	//PS2503
	// 2017.1119 MISLOADING CHECK SENSOR
	int i_MissLoadingChk1;//PS2504
	int i_MissLoadingChk2;//PS2505
	//PS2506
	//PS2507

	int o_TestRbtPickerGripCyl[4];
	int i_TestRbtPickerGripDvcExistChk[4];


	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 26
	///////////////////////////////////////////////////////////////////////////////////////////////
	int o_MainAir;							//S2600
	//S2601
	//S2602
	//S2603
	//S2604
	//S2605
	//S2606
	//S2607
	int i_DvcDir1Chk;						//PS2600
	int i_DvcDir2Chk;						//PS2601
	int i_MainMcChk;						//PS2602
	int	 i_CBox3SmokeChk;				//PS2603
	int	 i_CBox4SmokeChk;				//PS2604
	int	 i_Emo5SwChk;					//PS2605
	int	 i_Emo6SwChk;					//PS2606
	int i_NonContect_DoorChk_3;				//PS2607
	int i_NonContect_DoorChk_4;				//PS2608


	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 27
	///////////////////////////////////////////////////////////////////////////////////////////////

	//S2700
	//S2701
	//S2702
	int	 o_DoorLockTestSite;					//S2703
	int i_Tsite1_LeftDoor15Chk;				//PS2700
	int i_Tsite2_LeftDoor16Chk;				//PS2701
	int i_Tsite3_LeftDoor17Chk;				//PS2702
	int i_Tsite4_LeftDoor18Chk;				//PS2703
	int i_Tsite_Top12SmokeChk;				//PS2704
	int i_Tsite_Top34SmokeChk;				//PS2705
	int i_Tsite5_LeftDoor19Chk;				//PS2706
	int i_Tsite6_LeftDoor20Chk;				//PS2707
	int i_Tsite7_LeftDoor21Chk;				//PS2708
	int i_Tsite8_LeftDoor22Chk;				//PS2709
	int i_Tsite_Top56SmokeChk;				//PS2710
	int i_Tsite_Top78SmokeChk;				//PS2711


	int i_TsiteDoorChk[8];
	int i_TsiteSmokeChk[4];

	//////////////////////////////////////////////////////////////////////
	//2017.0430
	int i_EmptyRejFull;
	//////////////////////////////////////////////////////////////////////

};
extern  tagIO_INFO  st_io_info;
// *****************************************************************************

// *************************************************************************
// ���� ��θ� ���� ����ü ���� ����                                             
// *************************************************************************
struct tagPATH_INFO
{
	CString strBasic;			// �⺻ ���� ���� ���� ����+���ϸ� ���� ����

	CString strPathDvc;			// ����̽��� ƼĪ ���� ���� ���� ���� ���� ���� ����
	CString strSocket;			// ���Ϻ�...
	CString strPathAlarm;		// ��� �˶� ���� ���� ���� ���� ����
	CString strPathOp;			// ���۷����� ���� ���� ���� ���� ����
	CString strPathTray;		// Tray�� ���� ���� ���� ���� ���� ���� ����

	CString strPathFile;		// ���� ���� ���� ���� ����
	CString strPathBack;		// BACKUP ���� ���� ���� ���� ����

	CString strPathRecipe;
	CString strPathFtpGms;		// ftp path ���.....

	CString strPathRecovery;	// recoverydata........

	CString strFileBasic;		// �⺻ ���� ���� ���� ���ϸ� ���� ����
	CString strFileWait;		// ��� �ð� ���� ���� ���ϸ� ���� ����
	CString strFileMotor;
	CString str_scrap_code;

	CString strOperation;		//Operation Log ���� ���� ����.
	CString strInterface;		//Interface Log ���� ���� ����.
	CString strMachine;			//Machine Log ���� ���� ����.
	CString strTotal;			//Total Log ���� ���� ����.
	CString strGms;
	CString strData;			//Data Log ���� ���� ����.
	CString strAlarmDay;
	CString strAlarmMonth;

	CString strIoMap;
	CString strPartIoMap;
	CString strIoPartMap;
	CString strMotorAxisMap;
	CString strMotorPartMap;
	CString strWaitTimeMap;

	CString strTool; // 20140811 jtkim

	// jtkim 20150414
	CString strBarcode;
	CString strBarcodeNg;

	// jtkim 20150607
	CString strPathLotWork;
	CString strPathLotPcb;

	// jtkim 20150721
	CString strPathCycle;

	// jtkim 20160107
	CString strPathTestSite;
};
extern  tagPATH_INFO st_path_info;
// *************************************************************************

struct tagWORK_INFO
{
	int		nPgmInfo;
	int		nSimulationMode;
	int		nMessageInfo;
	int		nStatusEqp;
	int		nRearSafetyChk;
	int		nTowerStatus[5];

	int		nRemoveInfo[10];		// alarm skip ����.....
	int		nRemoveYesNo[10];		// alarm skip ����.....

	int		nLfTsiteBin[2][12];
	int		nRiTsiteBin[2][12];

	int		nBcrX;					// barcode x ��ǥ.....
	int     nBcrY;					// barcode y ��ǥ.....
	int		nBdReadyChk;			//

	//kwlee 2016.0613
	int     mn_Motor_Pos_X[4];
	int     mn_Motor_Pos_Y[4];

	// jtkim 20150625 tray skip
	int		nUnLdCvyTrayRemove;

	CString strBarcode[2];

	int		nOldHifix;

	DWORD	dwCycleTime[2][3];

	double	dCurrPos[100];

	// jtkim 20150719
	int		nFtpWork;

	// jtkim 20150721
	DWORD	dwTrayCycle;
	DWORD   dwLotCycle;
	int		nCycleCount;
	int		nCycleYesNo;
	int		nTsiteCycleYesNo;

	// jtkim 20150818 cycle time���� �߰�
	DWORD	dwFrontTime[3];
	DWORD	dwFrontSmema[3];

	DWORD	dwRearTime[2][3];
	DWORD	dwRearSmema[2][3];

	// jtkim 20150902
	CString strHifix[10];

	// daily cycle time
	double	dDailyCycle;

	CTime	tLotStart;
	CTime   tLotEnd;

	CString strSocketOff;
	CString strSocketOn;

	int		nLdCvySite;							// loader tray ���� left/right

	// jtkim 20161006
	int		nScrapRetest[10000];	// ���߿� scrap code�� ���� ����......

	//2016.1116
	int m_nMdlTotal;
	int m_nCboSelModel;
	CString m_strModelName[100];
	CString m_strCurModel;
	int m_nNmModel;//ahepfaud
	CStringList m_strModel[50][200];//[m_nNmModel][Partnum]

	int nXgemChk;
	int	nMpChk;
};
extern  tagWORK_INFO  st_work_info;

// *************************************************************************
// ��Ÿ ���� ���� ����ü ����                                                    
// *************************************************************************
struct tagOTHER_INFO
{
	CString strOpMsg;			// ����Ʈ ȭ�� ��� ���� ���� �޽��� ���� ����
	CString strNormalMsg;		// ����Ʈ ȭ�� ��� ���� ���� �޽��� ���� ����
	CString strAbnormalMsg;		// ����Ʈ ȭ�� ��� ���� ���� �޽��� ���� ����
	CString strSndMsg;			// ����Ʈ ȭ�� �׽��� SEND �޽��� ���� ����
	CString strRcvMsg;			// ����Ʈ ȭ�� �׽��� RECIVE �޽��� ���� ����
	CString strBcrRcvMsg;		// ����Ʈ ȭ�� ���ڵ� RECIVE �޽��� ���� ����
	CString strBoxMsg;

	CString strFallacyMsg;		// ���� �޽��� ��ȭ ���� Ÿ��Ʋ ���� ����
	CString strConfirmMsg;		// Ȯ�� �޽��� ��ȭ ���� Ÿ��Ʋ ���� ����
	CString strTypingMsg;		// Ű���� ��ȭ ���� Ÿ��Ʋ ���� ����

	CString strKeypadMsg;		// Ű�е� ��ȭ ���� Ÿ��Ʋ ���� ����
	CString strPadVal;			// Ű�е� ��ȭ ���ڿ� ����� ������ ���� ����

	CString strPadHighLimit;
	CString strPadLowLimit;

	CString strPassLevel;		// ��ȣ ���� ���� ����
	CString strPassword;		// ��ȣ ���� ����
	
	CString strParentMenu;		// �ָ޴� �̸� ���� ����

	CString strMsgBuf[10];

	char cNormalMsg[4096];
	char cAbnormalMsg[4096];
	char cOpMsg[4096];

	int nBuzYesNo;				// jtkim 20150410
	int nConfirmMsg;
	int nConfirmType;

	int nSocket[3];				// Socket Comment 20150804...
	////////////////////////////////2K10/02/09/ViboX////////////////////////////////////////
	int		nPrevStep[100];			// ���� step (���� �Ǿ����� ���� ����)
	CString	strHistoryStep[100];		// step_history �� 50���� �ش��ϴ� step�� ����
	CString strLastChangeDate[100];	// ���� step���� ����Ǵ� ������ �ð�
};
extern  tagOTHER_INFO  st_other_info;
// *************************************************************************

// *************************************************************************

#define INIT_CLEAR					0
#define INIT_READY					1
#define INIT_COMPLETE				2

#define WORK_CLEAR					0



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum SITE_WORK_INFO
{
	WORK_LD_STACKER_INFO = 0,
	WORK_EMPTY_STACKER_INFO,
	WORK_ULD_STACKER_INFO_1,
	WORK_ULD_STACKER_INFO_2,
	WORK_REJECT_STACKER_INFO_1,
	WORK_REJECT_STACKER_INFO_2,
	WORK_LD_LOAD_BUFFER_INFO,
	WORK_ULD_UNLOAD_BUFFER_INFO,
	WORK_LD_UNLOAD_BUFFER_INFO,
	WORK_ULD_LOAD_BUFFER_INFO,
	WORK_RETEST_BUFFER_INFO_1,
	WORK_RETEST_BUFFER_INFO_2,
	WORK_TEST_SITE_INFO_1,
	WORK_TEST_SITE_INFO_2,
	WORK_TEST_SITE_INFO_3,
	WORK_TEST_SITE_INFO_4,
	WORK_TEST_SITE_INFO_5,
	WORK_TEST_SITE_INFO_6,
	WORK_TEST_SITE_INFO_7,
	WORK_TEST_SITE_INFO_8,
	
};

enum THREAD_SYNC_VARIBLE_SITE_INFO  //��ġ�� Ʈ���� ���� ������ ��ġ���� ������ ����  
{
//james 2016.0730 
//���� ������ �� ����Ʈ�� ������ ������ �����Ͽ� ���� ������ ���� 
//lot ������ ���δװ�, END �ÿ��� �� ���е� ������ ��� ��� �Ϸ������� lot end  ���� 
	
	THD_LD_CV_STACKER_LEFT						 = 0,//load conveyor Left shifer site
	THD_LD_CV_STACKER_RIGHT							,
	THD_LD_STACKER									,
	THD_LD_TRAY_PLATE								,

	THD_EMPTY_STACKER								,

	THD_ULD_1_STACKER								,
//	THD_ULD_1_CV									,

	THD_ULD_2_STACKER								,	
//	THD_ULD_2_CV									,

	THD_REJECT_EMPTY_STACKER						,
	//THD_REJECT_EMPTY_CV								,

	THD_REJECT_OUT_1_STACKER						,
 	//THD_REJECT_OUT_1_CV								,

	THD_REJECT_OUT_2_STACKER						,	
	//THD_REJECT_OUT_2_CV								,
		 
	THD_WORK_RBT									,
	THD_TEST_RBT									,

	THD_LD_BUFF										,
	THD_ULD_BUFF									,

	THD_RETEST_1_BUFF								,
	THD_RETEST_2_BUFF								,

	THD_WORK_TRANSFER								,
	THD_REJECT_TRANSFER								,

	THD_TESTSITE_1									,
	THD_TESTSITE_2									,
	THD_TESTSITE_3									,
	THD_TESTSITE_4									,
	THD_TESTSITE_5									,
	THD_TESTSITE_6									,
	THD_TESTSITE_7									,
	THD_TESTSITE_8									,

	THD_SERVER_START_NETWORK_SITE					,	// lot ������ ó�� �������� �޾ƿ� ���� 
	THD_SERVER_END_NETWORK_SITE						,   // lot end�� ������ �����ϴ� ����


	THD_REJECT_BUFFER_STACK        ,//2017.0430 reject tray

	THREAD_MAX_SITE									,
//




/* james 2016.0730 �ּ�
	
	// tray site
	THD_PLATE_CV_LDTRAY_INPUT_SITE		=  0, //ld smema ���ó�� , //2015.0303 james ������� ���� 

	THD_PLATE_CV_BCR_TRAY_SITE				, //2015.0303 ���� ����񿡼� Ʈ���� ���� //ld smema ���ó��
	THD_PLATE_LD_TRAY_1_SITE				,
	THD_PLATE_LD_TRAY_2_SITE				,
	THD_PLATE_EMPTY_TRAY_SITE				,
	THD_PLATE_ULD_TRAY_1_SITE				,
	THD_PLATE_ULD_TRAY_2_SITE				,
	THD_PLATE_ULD_CV_MOVING_TRAY_SITE		, //7 //uld smema ���ó��  

	//bottom site ���� 
	THD_UNDER_EMPTY_LD_TRAY_2_SITE			, //8
	THD_UNDER_EMPTY_TRAY_STACKER_SITE		, //9
	THD_UNDER_UNLOAD_TRAY_STACKER_SITE		, //10

	THD_LOAD_TRAY_MOVE_INDEX_SITE			,
	THD_UNLOAD_TRAY_MOVE_INDEX_SITE			,
	//////////// tray site

	//robot site
	THD_LD_ROBOT_SITE						,
	THD_ULD_ROBOT_SITE						,
	THD_TEST_ROBOT_SITE						,
	THD_BCR_ROBOT_SITE					    , //2015.0119 

	//Etc
	THD_LD_ROTATOR_BUFF_SITE				,
	THD_ULD_ROTATOR_BUFF_SITE				,

	THD_RETEST_TRAY_BUFF_SITE				,
	THD_REJECT_TRAY_BUFF_SITE				,
	THD_REJECT_TRAY_UP_BUFF_SITE			,
	THD_REJECT_TRAY_DOWN_BUFF_SITE			,

	THD_COK_EXCHANGE_SITE					,
	THD_LEFT_TEST_SITE						,
	THD_RIGHT_TEST_SITE						,
	THD_LEFT_MP_SITE						,
	THD_RIGHT_MP_SITE						,

	THD_MACHINE_INPUT_NETWORK_SITE			,
	THD_MACHINE_OUTPUT_NETWORK_SITE			,

	THD_REJECT_TRAY_BUFF_SITE_1				,
	THD_REJECT_TRAY_BUFF_SITE_2				,
	THD_REJECT_TRAY_BUFF_SITE_3				,
	THD_REJECT_TRAY_BUFF_SITE_4				,
	THD_REJECT_TRAY_BUFF_SITE_5				,
	THREAD_MAX_SITE							,
	*/
};




//#define LOADTRAY_PALTE_NUM			2
//#define UNLOADTRAY_PLATE_NUM       2

//#define TESTROBOT_INFO_REQ			2	//0:req ����, 1:�۾� ������ġ  

//
//#define MAX_ROBOT_SITE				3 //0:load robot, 1:unload robot, 2:test robot 
//#define PICKER_DATA_INFO			5//[0]:dvc ��/��, [1]:������, [2]:retest cnt, [3]:screen �� ����, [4]:���� ����̽� ��/�� ���� �ӽ� ����, �Ǵ� �������� Ȯ��
//#define MAX_PICKER_PARA				4



struct tagSYNC_INFO
{
//james 2016.0802 
	int			nInit_Flag[50]; //�ʱ�ȭ ���� 


//////////////////

	int			nInitializeError;
	int			nLotEnd[50];					// lot end flag ����.....
///* james 2016.0801 
	//////////////////////////////
//james 2016.0801
////////////////////////////// 
	int nTestSiteChamber_AcMotorCtl_Req[THREAD_MAX_SITE][3]; //test site chamber ac motor�� �����Ѵ� 

	///////////////////////////////////////////////////////////////////////////////////////////
	// Load/Unload Tray ��� ����Ʈ 
	///////////////////////////////////////////////////////////////////////////////////////////

	int nTray_Exist_Data[THREAD_MAX_SITE]; //CTL_YES:tray ����. CTL_NO:tray ����,  //Ʈ���̸� ����ϴ� ������ ����

	int nConveyor_AcMotorCtl_Req[THREAD_MAX_SITE]; //ac conveyor�� �����Ѵ� 
 
	//load l/r input conveyor 
	int nLdCV_LR_TrayInStack_Flag[THREAD_MAX_SITE]; //�۾��ڰ� Ʈ���̸� �����ϸ� �� �÷��װ� �� �ȴ�(CTL_YES:tray ����. CTL_NO:tray ����) ( THD_LD_CV_SATCKER_LEFT	, THD_LD_CV_STACKER_RIGHT)

	//load elevator stacker
	int nLdElv_Tray_Req[THREAD_MAX_SITE];   //load elevator stacke���� load conveyor�� Ʈ���� ������ ��û�Ѵ� 

	//load plate���� ��û 
	int nLdPlate_Tray_Supply_Req[THREAD_MAX_SITE]; //load plate���� load elevator�� Ʈ���� ������ ��û�Ѵ� 

	//load plate, empty ele, unload #1,#2 elevator stacker �� work transfer�� �����Ͽ� �����Ѵ� 
	int nWorkTransfer_Req[THREAD_MAX_SITE][2]; //2016.0915 ������� ���� work Ʈ�����ۿ� ������ ��û�Ѵ� , [0]:��û����, [1]:1:Ʈ���̸� ����, 2:Ʈ���̸� ����

	//unload elevator stacker 
	int nUldElv_Tray_Req[THREAD_MAX_SITE]; //2016.0915 ������� ���� uld ele#1, #2 ���� ��� 


	///////////////////////////////////////////////////////////////////////////////////////////
	// Reject tray ��� ����Ʈ 
	///////////////////////////////////////////////////////////////////////////////////////////

	//reject empty, reject #1, #2 elevator stacker �� reject Ʈ�����۰� �����Ͽ� �����Ѵ� 
	int nRejectTransfer_Req[THREAD_MAX_SITE][2]; //reject Ʈ�����ۿ� ������ ��û�Ѵ� , [0]:��û����, [1]:1:Ʈ���̸� ����, 2:Ʈ���̸� ����

	//reject empty�� �� Ʈ���̸� �����ϴ� ���� 
	int nRejectCV_EmptyInStack_Flag[THREAD_MAX_SITE]; //2016.0915 ������� ���� //reject �� Ʈ���̸� ����� �� �ְ� �۾��ڰ� ���� ������ ����, �۾��ڰ� Ʈ���̸� �����ϸ� �� �÷��װ� �� �ȴ�

	//reject empty tray�� empty stacker�� ������ �� �ֵ��� ��û 
	int nRejectEmptyElv_Tray_Req[THREAD_MAX_SITE]; //2016.0915 ������� ���� //reject �� Ʈ���̸� stacker�� ���� �� �ֵ��� ��û 
	//reject elevator stacker
	int nRejectElv_Tray_Req[THREAD_MAX_SITE]; //2016.0915 ������� ���� //reject ele #1, #2 ���� ��� 

	///////////////////////////////////////////////////////////////////////////////////////////
	// Work Robot ��� ����Ʈ 
	///////////////////////////////////////////////////////////////////////////////////////////
	//load plate, load buff#1,#2 ����Ʈ�� ���� , Retest�ÿ��� ��밡��  
	//load buff#1/#2, unload #1/#2 Elevator stacker�� ���縦 ������ ���, Retest�ÿ��� ��밡��   
	int nWorkRbt_Dvc_Req[THREAD_MAX_SITE][5]; //[*][1]: WORK_PICK(����̽��� ���� ������ ���� ����), [*][1]:WORK_PLACE(/����̽��� ���� ������ ���� ����)
											  //[*][2]: �ε�/��ε� ������ ����(left turn((right test site �۾�����), right turn(left test �۾�����) ���),  -> �߿��� ������ ���Ƿ� �ٸ������� �ٲ㼭�� �ȵȴ� 
											  //[*][3]: �۾��� �׽�Ʈ ����Ʈ ��ȣ ��ġ ���� -> �߿��� ������ ���Ƿ� �ٸ������� �ٲ㼭�� �ȵȴ� 

											
	///////////////////////////////////////////////////////////////////////////////////////////
	// Test Robot ��� ����Ʈ 
	///////////////////////////////////////////////////////////////////////////////////////////
	//test site #1 ~ #8, Retest Buff #1/#2, Load buffer #1/#2(Retest�ÿ� ��밡��) ��� ,  
	//test site #1 ~ #8, Retest Buff #1/#2, Load buffer #1/#2 ��� , Retest�ÿ��� ��밡�� 
	int nTestRbt_Dvc_Req[THREAD_MAX_SITE][5]; //[*][1]: WORK_PICK(����̽��� ���� ������ ���� ����), [*][1]:WORK_PLACE(/����̽��� ���� ������ ���� ����)
											  //[*][2]: �ε�/��ε� ������ ����(left turn((right test site �۾�����), right turn(left test �۾�����) ���) ->�߿��� ������ ���Ƿ� �ٸ������� �ٲ㼭�� �ȵȴ�                                            	
											  //[*][3]: �۾��� �׽�Ʈ ����Ʈ Fix ��ȣ ��ġ ���� -> �߿��� ������ ���Ƿ� �ٸ������� �ٲ㼭�� �ȵȴ� 
										      //[*][4]:test site num
	// jtkim 20160901
	//int nTestSite_Target_THD_Work_Site; //�۾��� �׽�Ʈ ����Ʈ ���� THD_TESTSITE_1 ~ THD_TESTSITE_8
	int nTestSite_Target_THD_Work_Site[10]; //1 pick 2 place �۾��� �׽�Ʈ ����Ʈ ���� THD_TESTSITE_1 ~ THD_TESTSITE_8
	int nRetestBuff_Traget_THD_Work_Site; //�۾��� ���׽�Ʈ ������ ����, THD_RETEST_1_BUFF, THD_RETEST_2_BUFF  
    int nGoodStacker_Target_THD_Work_Site;//�۾��� ���׽�Ʈ ������ ����, THD_ULD_1_STACKER, THD_ULD_2_STACKER  
	int RejectStacker_Target_THD_Work_Site;//�۾��� ���׽�Ʈ ������ ����, THD_REJECT_OUT_1_STACKER, THD_REJECT_OUT_2_STACKER 

	// jtkim 20160831
	int nTestRbt_Dvc_Ld_Req[THREAD_MAX_SITE][10];//[*][1]: WORK_PICK(����̽��� ���� ������ ���� ����), [*][1]:WORK_PLACE(/����̽��� ���� ������ ���� ����)
												//[*][2]: �ε�/��ε� ������ ����(left turn((right test site �۾�����), right turn(left test �۾�����) ���) ->�߿��� ������ ���Ƿ� �ٸ������� �ٲ㼭�� �ȵȴ�                                            	
												//[*][3]: �۾��� �׽�Ʈ ����Ʈ Fix ��ȣ ��ġ ���� -> �߿��� ������ ���Ƿ� �ٸ������� �ٲ㼭�� �ȵȴ� 
												//[*][4]:test site num
												//[*][5]:test site start��ȣ
	int nTestRbt_Dvc_UnLd_Req[THREAD_MAX_SITE][10];//[*][1]: WORK_PICK(����̽��� ���� ������ ���� ����), [*][1]:WORK_PLACE(/����̽��� ���� ������ ���� ����)
												//[*][2]: �ε�/��ε� ������ ����(left turn((right test site �۾�����), right turn(left test �۾�����) ���) ->�߿��� ������ ���Ƿ� �ٸ������� �ٲ㼭�� �ȵȴ�                                            	
												//[*][3]: �۾��� �׽�Ʈ ����Ʈ Fix ��ȣ ��ġ ���� -> �߿��� ������ ���Ƿ� �ٸ������� �ٲ㼭�� �ȵȴ� 
												//[*][4]:test site num

	int nTestRbt_LdBuff_Enable[4]; //�׽�Ʈ �κ����� �ε� ���ۿ� ���� ����̽� ������ �����Ͽ� �ش�. �ε� ������ �̿� ���� ���縦 �����Ѵ�, �۾��� �׽�Ʈ ���� ������ ���� �׽�Ʈ�κ����� ���÷� ����ȴ�.
	
	//test site dut #1 ~ #8 ������ è���� ���� ������ test robot�� ���� 
	//1. �κ��� ���縦 ���� �̵��ϸ�, chamber �̵��Ͽ� �����ǰ� ���� �ڵ� �׽�Ʈ�� �����Ѵ�.
	//2. test�� ������ ��ȣ�� �ް� test robot�� ���� ���µ� ������ ������ è���� �ڵ����� �̵� ��Ų�� 
	//int nTestRbt_Chamber_Action_Req[THREAD_MAX_SITE];




	//james 2016.0823 ���� ������� ���� 
	
	int			nInitBcrRbt;
	int			nInitLdTrayCvy;
	int			nInitLdTrayPlate;
	int			nInitLdTrayShifter;
	int			nInitLdRbt;
	int			nInitLdBuff;
	int			nInitEmptyStk;
	int			nInitCokRbt;
	int			nInitRetBuff;
	int			nInitRejBuff;
	int			nInitUnldTrayPlate;
	int			nInitUnldTrayShifter;
	int			nInitUnldRbt;
	int			nInitUnldBuff;
	int			nInitUnldTrayStk;
	int			nInitTSiteRbt;
	int			nInitLfBtmInterface;
	int			nInitLfMP;
	int			nInitLfTopInterface;
	int			nInitRiBtmInterface;
	int			nInitRiMP;
	int			nInitRiTopInterface;
	int			nInitUnldTrayCvy;

	int			nInitializeSuccess;
//*/
	//2015.0104
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//������ ó������� ������ ����Ʈ�� DATA_SET, �޴� ����Ʈ�� DATA_CLEAR ������ �����͸� ó���Ѵ� 
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	



///* james 2016.0810 �ּ� 

//////////////////////////////
	
//	int nTray_Exist_Data[THREAD_MAX_SITE];		//���� ����� 0 ~ 10������ ����� ����
	int nPlate_Tray_Supply_Req[THREAD_MAX_SITE];		//Ʈ���� ����Ʈ�� ld/uld index shift�� ������ ����Ʈ�� �̼��� ��û�ϴ� ���� 

	//int nSmema_Interface_Req[THREAD_MAX_SITE];	//���� ����� THD_PLATE_CV_LDTRAY_INPUT_SITE, THD_PLATE_ULD_CV_MOVING_TRAY_SITE 2���� ����Ѵ� 
	int nBCRRobot_Req[THREAD_MAX_SITE];			//Plate BCR Ʈ���� ����Ʈ�� BCR �۾��� ��� 

	//james 2016.0805 int nLdRobot_Req[THREAD_MAX_SITE];			//���� ����� THD_PLATE_LD_TRAY_1_SITE,THD_PLATE_LD_TRAY_2_SITE, THD_LD_ROTATOR_BUFF_SITE 3���� ��� 
	int nUnloadRobot_Req[THREAD_MAX_SITE];		//���� ����� THD_PLATE_ULD_TRAY_1_SITE,THD_PLATE_ULD_TRAY_2_SITE, THD_ULD_ROTATOR_BUFFER_SITE 3���� ��� 
	int nTestRobot_Req[THREAD_MAX_SITE];       //

	//index shifter�� �̿��Ͽ� �̵��� ����ϴ� ���� 
	int nLdIndex_Tray_Shifter_Req[THREAD_MAX_SITE];		//�۾��� ���� CVInput, Load1�� Ʈ���̸� Load1,Load2�� ����Ʈ�� Ʈ���̸� �ε��� ����Ʈ �Ѵ� 
	int nUnloadIndex_Tray_Shifter_Req[THREAD_MAX_SITE]; //�۾��� ���� empty, unload1�� Ʈ���̸� unload1,unload2�� ����Ʈ�� Ʈ���̸� �ε��� ����Ʈ �Ѵ�

	int nEmpty_Stacker_Req[THREAD_MAX_SITE];		//�� Ʈ���̸� �����Ѵ� 
	int nUnloader_Stacker_Req[THREAD_MAX_SITE];		//���簡 �ִ� Ʈ���̸� �����Ѵ� , inline  mode�� �ƴҶ��� ��� 

//2015.0127 	int nLdRotator_Buff_Req[THREAD_MAX_SITE];  //
//2015.0127	int nUnloadRotator_Buff_Req[THREAD_MAX_SITE];  //
	int nRetest_Tray_Req[THREAD_MAX_SITE];
	int nReject_Tray_Req[THREAD_MAX_SITE];
	//int nCOK_Exchange_Req[THREAD_MAX_SITE];
	int nLeft_Test_Req[THREAD_MAX_SITE];
	int nRight_Test_Req[THREAD_MAX_SITE];

//	int nConveyor_AcMotorCtl_Req[THREAD_MAX_SITE];

	int nSmema_Tray_Input_Req; //lable ��� 
	int nSmema_Tray_Output_Req; //AOI Vision ���

	int nBcrRbtSafetyZ;		// 20150203 jtkim barcode robot z ������ġ Ȯ�� 
	int nLdRbtSafetyZ;
	int nUnLdRbtSafetyZ;
	int nTSiteRbtSafetyZ;

	int nAutoBufferIn;
	int nFrontTrayIn;		// front ���񿡼� buffer in data �Ϸ�....

	int nBcrTestStart[4]; //[0]:test start(BD_START)/end(BD_END), [1] :bin ����(good(BD_DATA_GOOD)/reject(BD_DATA_REJECT)������(�ҷ��� retry ��񿡼� ����), [2]:tray Y pos, [3]:tray X pos. 

	//COK Buffer �۾� �����̴� 
	int nCOK_BufferExchange_Req[7]; //[0]:req(�۾� ��û), [1]:pick(0, COK buffer PICK)/place(1, COK Buffer Place), [2]:�۾� ��ġ#1(0~3), [3]:�۾� ��ġ#2(0~3) ===>2�� ������ �����ؾ� ��, [4]:#1�۾� �ϷῩ�� YES/NO, [5]:#2�� �۾��Ϸ� ����, [6]:���� Z �۾� ��ġ  

	////////////////////////////////////////////////////////
	// �Ʒ������� 2015.0225 ���� ���� 
	////////////////////////////////////////////////////////
	int nRejectTransfer_Safety_Flag; 
	//1:reject tray�� �۾� �� set, �����Ұ��̴�.
	//2:test site �κ��� ���� �����ϴٴ� ��ȣ�� ������ 
	//3:reject transfer ���� ��(�̋� test robot�� ��ε� ���� ������ ������ ������ ���ƾ� �Ѵ�.(Z���� ��� ������ �ȴ�)
	//0:reject transfer�� �۾� �Ϸ��� flag�� 0 ���� Ŭ�����Ͽ� ���´�.

	int nUnloadRbot_BcrRead_Flag; //unloading Robot ���ڵ� �б� ����� ��� 

	int nUnloadTray_LotEnd_TrayOut_Flag; //2015.0307 lot end�� ��ε� Ʈ���̿� ���簡 �������� ������ ���� ���� �����

	int nUnLdRbtDownZ;	// jtkim 20150608
//*/

	int nLdUld_LightChk; //LD ULD Light Curtain
	int nRej_LightChk;

	int mn_lduld_rbt;//2016.1114
};
extern tagSYNC_INFO	st_sync_info;




struct tagVARIABLE_INFO
{

	//james 2016.0823 ���� ������� ���� 


	int			nForce_LOTEND;//2015.0224 james ������ lot end�� ����ÿ� ����Ѵ� 

	int			nRetestBin_OutputCount_Full_Flag; //retest bin�� ó���ؾ��ϴ� �����̴�. (�ε� ��Ŀ�� �ε� ���縦 �̸� �������� ���� ���׽�Ʈ ���簡 ���޵Ǳ⸦ ��ٸ��� )

	int			nTestDvcOutFlag; //���� lot�� �׽�Ʈ�� ���� ��ε� ���̴�. �� Ʈ���̸� ��ε� �÷���Ʈ�� �����ؾ� �Ѵ� 
	
	int			nTester_MP_Docking_End_Flag[2]; //left, right 
	int			nCokBuffer_Change_End_Flag; //100: cok�� ��ü�� �����Ѵ�, 200: COK ��ü�� �Ϸ�� ����(������ �Ǹ��� �۾��� ����), 0:cok ��ü �۾� ��οϷ� 

	//2015.0104 int nTray_Exist_Info[TRAY_EXIST_POSITION_INFO_MAX_SITE]; //Ʈ���� ���� ������ �����Ѵ� 
	int			nEmptyStackerSite_LastMovePos_Num; //2015.0111 1:
	double       dEmptyStackerSite_LastMovePos_Backup; //�� ��ġ�� �������� ��� ��ġ 

	int			nUnloadStackerSite_LastMovePos_Num; //2015.0111 1:
	double       dUnloadStackerSite_LastMovePos_Backup; //�� ��ġ�� �������� ��� ��ġ 

	//int			nTrayRunMode_StandAlone; //Ʈ���̸� �� �������� �������ʰ� ��ε� ����Ŀ�� �����ϴ� ��� 

	int			nBuffer_TestSite_RotatorDegree; //�׽�Ʈ ����Ʈ���� �ʿ��� ���� ==> 0, 90, 18, 270�� ȸ�� ����
	int			nBuffer_TraySite_RotatorDegree; //Ʈ���� ����Ʈ���� �ʿ��� ���� ==> ������ 0�� �ϰ��̳� Ȥ�� ���� �߰� 

	//
	int			npRotatorBuffer_Mapping_Num_Y[10][10];
	int			npRotatorBuffer_Mapping_Num_X[10][10];

 
	//////
	//LOT ���� ���� ����, Max 10������ ������ ������ �ÿ��� �ִ� 3�� ������ ���� �����ϰ� �ϰ�, 3�� �̻��϶��� Input smema ����Ʈ���� ����ϰ� ����  
	//int			nLotWork_Count; //���� ��� ������ �������� lot�� ����, Max 10������ ������ ������ �ÿ��� �ִ� 3�� ������ ���� �����ϰ� �ϰ�, 3�� �̻��϶��� Input smema ����Ʈ���� ����ϰ� ����  
	//int			nLot_LastModule[10]; //�� LOT�� LAST MODULE ����
    //CString     strLot_Name[10];     //�� LOT�� NAME
	//CString		strCurrent_LotName; //���� ���ʷ� �۾����̴� �ֽ� lot �� ����(nLot_LastModule[0] ��° �迭�� ������ ���ƾ� �Ѵ� )
	
	//lot end�ϸ鼭 �� ������ ��ȯ�Ͽ� ���� lot�� ������ �ѹ��� �����Ѵ� 
	int			nLotExchange_CurrentLotNum; //0,1 �� ������ ������ 0 -> 1 -> 0 ���� �ݺ� (���� �۾����� lot�� ������ update �ϴ� ��Ȱ�� �� 

	//CString		strReservation_LotName; //���� lot ���� ����.

	////// �Ʒ������� 2015.0225 ������ ���� 
	int			nLdbuffer_TestSite_MoveReady_Flag; //�ε����ۿ�  ���簡 �غ�Ǿ� �׽�Ʈ ����Ʈ�� �̼��� ��� ���̴�. �̋� �׽�Ʈ �κ��� �ε��� ������������ ������ ������� ���� ó���Ѵ� 
	int			nRotatorBuffer_PlateTrayReferencePoint_Degrees; //205.0227 �׽�Ʈ ����Ʈ���� Turn�� ��ġ�� tray plate��ġ�� �������� (0��)�ٽ� �����µ� �ʿ��� degrees ��ġ�� (/0(0��), 1(90��),2(180��),3(270��) �϶� 0���� ���� �ִ� ��)

	int			nTestSite_LastSSDPlace_Pos; //2015.0302 ������ �׽�Ʈ ����Ʈ������ ��ġ (0 - 47���� ���� ����)

	int			nTestSiteRobot_Z_RejectTrayMove_Safety_Action_Flag[10]; //2015.0303 reject tray ��ü�� ���� ��ġ üũ �� �̵���ġ 

	int			nTemp_Spare_Flag[100]; //2015.0303 ���� ������ ���� ���α׷� ������ ����ϴ� �ӽ� ����  

	int			nTestSite_SSD_Place_OneCycle_Flag; //2015.0309 Current Lot �� ����Ʈ ��ü �۾��� Ƚ��(0- 11���� 12�� ����Ʈ)
	 
	// jtkim 20151017
	int			nRotatorBuffer_Barcode_Degrees;
	//2015.0312
	//st_var ���� 
	int n_Contact_Start_Flag[2][THREAD_MAX_SITE][2][12]; //�۾� ���� ����(test robot���� ����)
	int n_Contact_Start_Status[THREAD_MAX_SITE][2][12];//�۾� ���࿩��
	int n_Contact_WorkSite_Info[2][48];//�۾� ���� ��ġ����
	int n_Contact_Start_Pos[2];//�۾� ���� ��ġ
	int n_Contact_End_Pos[2];//�۾� �Ϸ� ��ġ
	int n_Contact_WorkCnt[2];//�۾� �� ���� ����
	int n_Contact_Site[2]; //left/right test site
	int n_Contact_TopBtm[2]; //left/right test site
	int n_Contact_XPos[2]; //left/right test site

	int n_Socket_Conatct_Work_Mode_Flag[2]; 

	//2015.0314 �߰� 
	int n_RetestBinTray_LoadSite_Supply_Count;
	int n_RetestBinTray_LoadSite_MoveTo_Start_Flag;

	int n_NextLot_TestSiteSocket_Work_Enable_Count;

	//2015.0322 �߰� 
	int n_LotEnd_LastRetetstCount_Dvc_Flag[2]; //[0]:������ retest ��������, [1]:retest ���� ������� 

	//2015.0323 
	int n_SocketOff_Seperator_Mode_Flag; //4�� ������ 0 �Ǵ� 2�� ���� ��ġ�� �� �� �ִ�, �� 2�� ������ ���� ����, 1:4���� �����߿��� �ϳ� ������ ���� ���� ����
	
	//2015.0324 james test socket auto socket on/off ó���� ��� 
	int n_Auto_Socket_Open_WorkPos_Num;
	int n_Auto_Socket_Close_WorkPos_Num;
	

	// jtkim 20150826 
	int nPlateFlag;			// unloading plate 1.....
	int nRejectFlag;		// reject stacker....
	int nRetestFlag;		// retest.....
};
extern tagVARIABLE_INFO	st_var;




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 2015.0102 
#define MAX_GMS_CHANNEL					8

struct tagCLIENT_INFO
{
	CString		strIp;			// ip address......
	
	int			nPort;			// port.....
	int			nConnect;		// client �������......
	int			nSendLength;	// �۽� ���� ����......
	int			nRevLength;		// ���� ���� ����......
	int			nClientReady;	// ������ ������...
	int			nRevInfo;		// client ���� ���..... (0:�ٷ� 1: ������)

	char		chRev[8196];	// ���ų���....
	CString     strRev;
	char		chSend[8196];   // �۽ų���....
	CString		strSend;
	CString     strName;
	
};
extern tagCLIENT_INFO st_client_info[20];
//extern tagCLIENT_INFO st_Testerclient_info[20];

struct tagSERVER_INFO
{
	CString		strIp;			// ip address......
	
	int			nPort;			// port.....
	int			nConnect;		// client �������......
	int			nSendLength;	// �۽� ���� ����......
	int			nRevLength;		// ���� ���� ����......
	
	char		chRev[8196];	// ���ų���....
	CString     strRev;
	char		chSend[8196];   // �۽ų���....
	CString		strSend;
	CString     strName;
};
extern tagSERVER_INFO st_server_info[20];
extern tagSERVER_INFO st_Testerserver_info[10];

struct tagRS232_INFO
{
	int			nSerialPort[10];		// serial port....
	int			nSerialBaudrate[10];	// serial baudrate....
	int			nSerialData[10];		// serial data....
	int			nSerialParity[10];		// serial parity.....
	int			nSerialStop[10];		// serial stop....
};
extern tagRS232_INFO st_rs232_info;

struct tagGMS_INFO
{
	CString		strData;
	CString     strMsg[2];
	CString     strChan[MAX_GMS_CHANNEL];
	CString		strValVolt[MAX_GMS_CHANNEL];
	CString		strValRes[MAX_GMS_CHANNEL];
	CString		strEqpID[MAX_GMS_CHANNEL];
	CString		strDateTime[MAX_GMS_CHANNEL];
	
	CStringArray	ArValVolt[MAX_GMS_CHANNEL];
	CStringArray	ArValRes[MAX_GMS_CHANNEL];
	
	COleDateTime oleTime[MAX_GMS_CHANNEL];
};
extern tagGMS_INFO	st_gms_info;

#define RCVR_CLEAR						0
#define RCVR_PICK_UP_COMMAND			1
#define RCVR_PICK_UP_COMPLETE			2
#define RCVR_MOVE_COMMAND				3
#define RCVR_MOVE_COMPLETE				4

#define MOVE_FORWARD					0
#define MOVE_BACKWARD					1

#define LOW_SPEED						0
#define HIGH_SPEED						1

struct	tagRECOVERY_INFO
{
	double	dRecoveryPos[100];	// ������ recovery position......

	int		nRecoveryStep[100];	// recovery step......
	int		nRecoveryFlag[100];	// recovery �Ϸ� ����........
	
	int		nMcRecoveryUsed;
	int		nMcDataRecovery;

	// jtkim �Ǹ��� recovery ����
	int		nRecoveryCyl[100];
};
extern tagRECOVERY_INFO st_recovery_info;

#define MAX_BUF_SIZE			20	//james 2016.0806 ���۴� �� �������� �ִ� 20���� ��� ���´� 

#define MAX_PICKER				4
#define MAX_BUF_ROW_Y				10
#define MAX_BUF_COL_X				10
#define MAX_TRAY_ROW			20
#define MAX_TRAY_COL			20
#define MAX_SITE_PICKER			4
#define MAX_SITE				12

#define LD_ULD_PICKER_PARA			1 //james 2016.0804 
#define TEST_PICKER_PARA			4
#define TESTSITE_PARA				8		//james 2016.0808 �׽�Ʈ ����Ʈ ��ü ���� 
#define TEST_SOCKET_PARA			8		//tester�� ���� �Ķ� �� 
#define MAX_LDULD_BUFF_PARA				4//james 2016.0810
#define MAX_RETEST_BUFF_PARA			15//james 2016.0816 


#define VACUUM_WAIT_TIME			100  //100ms ���� üũ�ð� Ȯ�� 

#define MAX_RETEST_COUNT			5
//kwlee 2016.0613
#define  LD_ROBOT                   0
#define  TRANSFER_ROBOT				1
#define  REJECT_ROBOT				2
#define  ULD_ROBOT				    3


struct tagPCB_INFO
{	
	int nTestBdStart;						//�׽�Ʈ ���۰� ���� �˼� �ִ� 
	int nEnable;							//�ش� ��ġ �����/�� 
	int nTempEnable;                      //james 2016.0816 //�׽�Ʈ����Ʈ�� ���Ͽ������� �ش� ��ġ�� �۾��� �� �� ���� ����, ��Ŀ�� ��� ������� ���ϰ� �Ѵ�, �ش� �׽�Ʈ ��ġ �۾� �Ϸ���, �ش� ���� Ŭ����. 

	int nYesNo;								// ssd ����.....
	int nBin;								// ssd �׽�Ʈ ���.....
	int nRetestCnt;							// retest �׽�Ʈ Ƚ��......
	int nRetest;
	int	nBinNum;							// ssd �׽�Ʈ ���.....
	int	nRetestSite[MAX_RETEST_COUNT];		// retest ������ site ��ȣ....
	int nBdNum;								// ������ �׽�Ʈ �׽�Ʈ Board ��ȣ.......
	int nScrCode;							// scrap code.....
	int nCOK_IDNum;							//cok ���� 1~7���� 
	int nCOK_YesNo;							//cok ���� ��/�� ���� 
	int nStatusChang;						// �������� ����.........
	int nOldBin;							// barcode....

	int nSkipMode;                        //kwlee 2016.0912 TestSite SkipMode
	DWORD dwBdTime;							// test time......

///////////////////////////////////////////////////////////////
//james 2016.0804 
	CString strSerialNo;					// Lot���� pcb�� ������ȣ......
	CString strBarcode;						// pcb 2D barcode......

	CString strArrNo;						// array serial....
	CString strPPID;						// ppid.....
	CString strPSID;						// psid....
	CString strWWN;							// wwn.....
	CString strCSerialNo;					// c_serial......
///// james ���� ������� ������ ������ 

	CString strLotNo;						// pcb Lot number.....
	CString strBarcode1D[2];				// pcb 1D barcode......
	// jtkim 20160929
	CString strBarcode2D[2];				// pcb 2D barcode......
	CString strPartNo;						// part no.....
	CString strBin;							// strBin......
	// jtkim 20150707
	CString strOldSerial;
	// jtkim 20160804
	CString strMsg;							// result message (h60 h55 c30 c40)....
	// jtkim 20150805
	CString strComment;

	COleDateTime tStart;					// �׽�Ʈ ���۽ð�...
	// jtkim 20151027
	CTime tStandBy;
	// jtkim 20151028
	int nReworkNg;							// rework mode check......

	// jtkim 20160106
	int nCntInfo;							// count info......

	// jtkim 20160108
	DWORD dwCycle[10];						// log add...

	// jtkim 20160929
	int nTemp[10];
	int nFailCount;                       //kwlee 2016.1124

	//kwlee 2016.1228
	int nBinFailCnt[150];
	int nTotalFail;
	int nBinGoodCnt;

	CString strBdTime;

	//kwlee 2017.0905
	int nTrayCnt;
	int nProductCnt;

	//2017.0109
	CString			strMaterial;			// matrial id.....
	CString			strLogKey[10];
	CString			strLogData[10];
	CString			strMovePos[2];	
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//james 2016.0806 

struct tag_BUFFER_INFO
{
	int nBuffer_Req[3];

	CString strLotNo;				// test site lot no....
	CString strPartNo;				// test site part no.....
	CString strEqpID;				// test site epq id.....
	CString strOptCode;				// opt code (lot display���� ���� ��������)

	//kwlee 2017.0905
	int nTrayCnt;
	int nProductCnt;
	//

	//james 2016.0813 
	int nTestSIte_No;	  //�۾��� ������ ����, Test Site #1,#2,#3,#4, #5,#6,#7,#8
	int nTestSite_LR_Pos; //�۾��� ������ ����, TESTSITE_LEFT(test #1,#2,#3,#4), TESTSITE_RIGHT(Test #5,#6,#7,#8)
	int nTestSite_Start_FixPos; //���������� �׽�Ʈ����Ʈ �۾�������ġ ���� (0 �Ǵ� 4 �̴�.)
	///////

	//2017.0109
	int				nLogCount;
	CString			strMaterial;			// matrial id.....
	CString			strLogKey[50];
	CString			strLogData[50];

	tagPCB_INFO st_pcb_info[MAX_BUF_SIZE]; // pcb ����ü ����.....
};
extern tag_BUFFER_INFO st_buffer_info[THREAD_MAX_SITE];
//////  2016.0806 



struct tag_PICKER_INFO
{
	tagPCB_INFO st_pcb_info[MAX_PICKER]; // pcb ����ü ����.....

	int nPicker_Workinfo[MAX_PICKER]; //�׽�Ʈ �κ����� PICK&PLACE �۾��� �� �۾��� ��Ŀ ������ �����Ѵ� 

	//2017.0109
	int				nLogCount;
	CString			strMaterial;			// matrial id.....
	CString			strLogKey[50];
	CString			strLogData[50];

	//kwlee 2017.0905
	int nTrayCnt;
	int nProductCnt;
	//
};
extern tag_PICKER_INFO st_picker[THREAD_MAX_SITE];

//////////////////////////////////////////////////////////////////////////////////////////////
//2015.0106 ��ü plate�� ����Ʈ�� Ʈ���� ���� ������ �����Ѵ� 
//////////////////////////////////////////////////////////////////////////////////////////////
struct tagALL_TRAY_INFO
{
	int nNewLotIn;            
	int nLastModule;				// ������ �������� üũ
	int nModuleCnt;					// tray �ȿ� �ִ� ������.
	int nTrayExist;					//tray ���� ��/�� �߰� 2015.0226 james
	// jtkim 20150811
	int nReworkMode;				// rework mode......

	//2015.0226 �߰� 
	CString strLotNo;				// lot no......
	CString strPartNo;				// part no......
	CString strProcess;				// process id.....
	CString strOptCode;				// opt code (lot display���� ���� ��������)
	CString strFabSite;	          //2017.0105
	// jtkim 2015127
	CTime tStandBy;

	// jtkim 20160812
	int		nTestMode;				// ONESHOT, HOT, COLD, GATE......
	int		nInterface;				// ftp / local......
	int		nRetry;					// retry count.....
	int		nBarcodeMode;			// barcode ���ۼ��� (1��/2��)
	int		nQty;					// lot ����.....
	int		nModuleCount;			//
	int		nPgmDown;				// pgm download �Ϸ�......

	//kwlee 2017.0905
	int nTrayCnt;
	int nProductCnt;
	//
	//2017.0109
	int				nLogCount;
	int				nStartStatus;
	CString			strMaterial;			// matrial id.....
	CString			strLogKey[50];
	CString			strLogData[50];
	CString			strMovePos[2];

	tagPCB_INFO st_pcb_info[MAX_TRAY_ROW][MAX_TRAY_COL]; // pcb ����ü ����..... 
};
extern tagALL_TRAY_INFO st_tray_info[THREAD_MAX_SITE];

//james 2016.0805 extern tagALL_TRAY_INFO st_buffer_info[THREAD_MAX_SITE]; //2015.0114 ������Ʈ ���۶����� �ӽ� ������ ������ ���� Ʈ���� ������ �����Ѵ�  

//james 2016.0730
//james 2016.0805 extern tagALL_TRAY_INFO st_stacker_info[THREAD_MAX_SITE]; //��� Ʈ���̰� �̵��ϴ� ����Ŀ ������ ��ġ���� ����Ѵ�

struct tagREJECT_TRAY_INFO
{
	int nYesNo;				// reject tray ����.....

	//kwlee 2017.0905
	int nTrayCnt;
	int nProductCnt;
	//
	tagPCB_INFO st_pcb_info[MAX_TRAY_ROW][MAX_TRAY_COL]; // pcb ����ü ����..... 
};
//james 2016.0805 extern tagREJECT_TRAY_INFO	st_reject_info[THREAD_MAX_SITE];
// test site ����ü
//[0][0] left bottom [0][1] left top [1][0] right bottom [1][1] rigth top
struct tagTEST_SITE_INFO
{
	CString strLotNo;				// test site lot no....
	CString strPartNo;				// test site part no.....
	CString strEqpID;				// test site epq id.....
	CString strOptCode;				// opt code (lot display���� ���� ��������)
	CString strTsiteSerial[2];      //Test site Duct Serial 0: left , 1 : Right
	CString strFabSite;	          //2017.0105

	int		nTestMode;				// ONESHOT, HOT, COLD, GATE......
	int		nInterface;				// ftp / local......
	int		nRetry;					// retry count.....
	int		nBarcodeMode;			// barcode ���ۼ��� (1��/2��)
	int		nQty;					// lot ����.....
	int		nModuleCount;			//
	int		nPgmDown;				// pgm download �Ϸ�......
	int		nChamberDoor;			// chanber door open......
	// jtkim 20160806
	int		nTestBdStart;			// test site ���üũ ���� �������........

	// jtkim 20160806
	COleDateTime tStart;			// �׽�Ʈ ���۽ð�...
	// jtkim 20160830
	DWORD	dwBdTime;

	tagPCB_INFO st_pcb_info[MAX_SITE]; // pcb ����ü ����.....

	int nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT   //james 2016.0809 
	int nStart_FixPos; //�۾� ������ ����Ʈ 0~7 ���� �� ���� ����(0��, 4�� ������ ���� ��ġ�̸�, üũ �� ���� ��ġ�� FIX�̴�) //james 2016.0813 

	int nTester_Req[3];
	//kwlee 2017.0905
	int nTrayCnt;
	int nProductCnt;
	//
};
// jtkim 20160803
//extern tagTEST_SITE_INFO st_test_site_info[THREAD_MAX_SITE][2]; //2015.0216 [2][2];
extern tagTEST_SITE_INFO st_test_site_info[THREAD_MAX_SITE]; //2015.0216 [2][2];
						 
enum CHAMBER_DOOR
{
	CHAM_OPEN			= 0,
	CHAM_CLOSE			,
};

//2015.0316 james COK
struct tagCOK_INFO
{ 
	int nWorkEnable;						//�ش� ��ġ ��� ��/��, ����ϸ� 1, ������� ������ 0��
	int nCOK_IDNum;							//cok ���� 1~7���� 
	int nYesNo[24];							// cok ����.....	
};

//2015.0316 james COK
struct tagCOKBUFF_SITE_INFO
{
	// jtkim 20150330
	int	nCokCount; // cok ����.......

	tagCOK_INFO st_pcb_info; // pcb ����ü ����.....
};
extern tagCOKBUFF_SITE_INFO st_Cok_Buff_info[4]; //cok 4�� ����Ʈ ���� 


struct tagTEST_SITE_SOCKET_FIND_INFO
{
	//james 2016.0807 
	CString strLotNo;				// test site lot no....
	int nRun_Mode; //0:�۾� �Ұ���, 1(WORK_PICK): Test site soket���� ��ε� �۾����� , 2(WORK_PLACE): test socket �� �ε� �۾����� 
	int nSite_Pos; //1:TESTSITE_LEFT, 2:TESTSITE_RIGHT  
	int nStart_FixPos; //�۾� ������ ����Ʈ 0~7 ���� �� ���� ����(0��, 4�� ������ ���� ��ġ�̸�, üũ �� ���� ��ġ�� FIX�̴�) 
	int nFix_Cnt; //�̹� ����Ʋ���� �۾� ������ ����(Max 4�� �۾����� (��Ŀ�� 4���̴�), 
					//�׽�Ʈ ����Ʈ ���� ���� ������, ���鿡�� 8�� ���� �� ������ ��ġ�� �������� 4���� �۾��ϸ�, ���� ���� 4���� �۾��Ѵ�
	int nTotal_Cnt; //�ش� ����Ʈ�� ��ü �۾������� ���� ���� 

	int n_Fix_03_Count; //���鿡�� ������ ���� ��ġ �������� 0 -> 7 �� 0 ~ 3 ���� ��ġ �۾����� ���� 
	int n_Fix_47_Count; //���鿡�� ������ ���� ��ġ �������� 0 -> 7 �� 4 ~ 7 ���� ��ġ �۾����� ���� 
};
// jtkim 20160901
//extern tagTEST_SITE_SOCKET_FIND_INFO st_find_testsite_info[THREAD_MAX_SITE]; //0~47������ ������ 12������ ������ ���� ����
extern tagTEST_SITE_SOCKET_FIND_INFO st_find_testsite_info[THREAD_MAX_SITE][10]; //0~47������ ������ 12������ ������ ���� ���� 
extern tagTEST_SITE_SOCKET_FIND_INFO st_find_testsite_status[THREAD_MAX_SITE];//������ ���¸� Ȯ���Ҷ� ���. ���� �������� �۾� ������� ����  //0~47������ ������ 12������ ������ ���� ���� 

//james 2016.0807 extern tagTEST_SITE_SOCKET_FIND_INFO st_test_socket_info[12]; //0~47������ ������ 12������ ������ ���� ���� 
//james 2016.0807 extern tagTEST_SITE_SOCKET_FIND_INFO st_test_socket_status[12]; //2015.0222 james ������ ���¸� Ȯ���Ҷ� ���. ���� �������� �۾� ������� ����  //0~47������ ������ 12������ ������ ���� ����, thj 

//////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* 2015.0108
// loader picker ����ü
struct tagLD_PICKER_INFO
{
	tagPCB_INFO st_pcb_info; // pcb ����ü ����.....
};
extern tagLD_PICKER_INFO st_ld_picker_info[MAX_PICKER];

// loader buffer ����ü
// [0] load picker -> buffer [1] buffer -> test site robot
struct tagLD_BUF_INFO
{
	tagPCB_INFO st_pcb_info[MAX_BUF_ROW][MAX_BUF_COL]; // pcb ����ü ����.....
};
extern tagLD_BUF_INFO st_ld_buf_info[2];

// unloader picker ����ü
struct tagUNLD_PICKER_INFO
{
	tagPCB_INFO st_pcb_info; // pcb ����ü ����.....
};
extern tagUNLD_PICKER_INFO st_unld_picker_info[MAX_PICKER];

// unloader buffer ����ü
// [0] unload picker <-> buffer [1] buffer <-> test site robot
struct tagUNLD_BUF_INFO
{
	tagPCB_INFO st_pcb_info[MAX_BUF_ROW][MAX_BUF_COL]; // pcb ����ü ����.....
};
extern tagUNLD_BUF_INFO st_unld_buf_info[2];



// loader tray ����ü
struct tagLD_TRAY_INFO
{
	tagPCB_INFO st_pcb_info[MAX_TRAY_ROW][MAX_TRAY_COL]; // pcb ����ü ����.....
};
extern tagLD_TRAY_INFO st_ld_tray_info[2];

// unloader tray ����ü
struct tagUNLD_TRAY_INFO
{
	tagPCB_INFO st_pcb_info[MAX_TRAY_ROW][MAX_TRAY_COL]; // pcb ����ü ����.....
};
extern tagUNLD_TRAY_INFO st_unld_tray_info[2];

// reject buffer ����ü
struct tagREJECT_BUFFER_INFO
{
	tagPCB_INFO st_pcb_info[MAX_TRAY_ROW][MAX_TRAY_COL]; // pcb ����ü ����.....
};
extern tagREJECT_BUFFER_INFO st_reject_buf_info[2];

// retest buffer ����ü
struct tagRETEST_BUFFER_INFO
{
	tagPCB_INFO st_pcb_info[MAX_TRAY_ROW][MAX_TRAY_COL]; // pcb ����ü ����.....
};
extern tagRETEST_BUFFER_INFO st_retest_buf_info;


// test site robot ����ü
struct tagTEST_SITE_RBT_INFO
{
	tagPCB_INFO st_pcb_info[MAX_SITE_PICKER]; // pcb ����ü ����.....
};
extern tagTEST_SITE_RBT_INFO st_test_site_rbt_info;
*/

struct tagTRACK_INFO
{
	CString	strLotNo;
	CString strPartNo;

	long	nIn;
	long	nPrimePass;
	long	nPrimeReject;
	long	nCumPass;
	long	nCumReject;

	int		nTestEndBin;
	int		nStatus;		// track ����........

	// jtkim 20161008 
	CTime	tStart;	
	int		nBdTime;
	int		nBdTimeCount;
};
extern struct tagTRACK_INFO st_track_info;

enum PICKER_PICKPLACE_INFO
{
	 PICKER_PICK_MODE	= 0,
	 PICKER_PLACE_MODE	   ,
};

#define	BD_READY_CHK_TIME	20		// board ready ����� �Ϸ�üũ �ð�

enum BOARD_INFO
{
	BD_NONE							= 0,
	BD_START						,
	BD_END							,
	BD_READY_CHK 					,
	BD_READY_OK						,
	BD_MODULE_CHK					,
	BD_MODULE_OK					,
	BD_PGM_CHK						,
	BD_PGM_OK						,
	BD_START_CHK					,
	BD_START_OK						,
	BD_BIN_CHK						,
	BD_BIN_OK						,
	BD_SLOT_OFF						,
	BD_SLOT_ON						,
	BD_DATA_GOOD					= 200,
	BD_DATA_REJECT					,
	BD_DATA_ABORT					,
	BD_DATA_RETEST					, //2015.0120
	BD_CONT_TIMEOUT					,
	BD_SEND_TIMEOUT					,
	BD_RCV_TIMEOUT					,
	BD_PCB_CHK_TIMEOUT				,
	BD_DATA_ABORT_CHK				, // 20150819
	
	
};
#define BD_DATA_CONTINUE_FAIL	-100
#define BD_DATA_OPT_FAIL		-200
// test site board ����
struct tagBOARD_INFO
{
	long nBdTestCnt[12];			// board �׽�Ʈ Ƚ��.....
	long nBdPassCnt[12];			// pass Ƚ��......
	long nBdInfo[12];				// board ����...... 0(Clear) 1(Start) 2.......
	long nBdBin[12];				// board ��� ����...... 
	long nBdFailContiCnt[12];		// ���� fail ����.....

	CString strBinHistory[12];		// jtkim 20150320......
	CString strSocketComment[12];	// Socket Off Comment 20150804
};
extern tagBOARD_INFO st_bd_info[2][2];

enum LOT_INFO
{
	LOT_CURR						= 0,
	LOT_NEXT						,
	LOT_CURR_BACKUP					, //2015.0302 james LOT_CURR�� lot end �� Ŭ�������� ��� �����͸� �޴´� 
};

struct tagLOT_DISPLAY_INFO
{
	CString strLotNo;
	CString strPartNo;
	CString strCycleID;
	CString strStepCnt;
	CString strRetestKind;
	CString strRetestRes;
	CString strOneShotFlag;
	CString strTier;
	CString strOptcode;
	CString strFabSite;
	CString strReworkCnt;
	CString strScrapSerial[1000];
	CString strScrapCode[1000];
	CString strArray[1000];
	
	int nQty;
	int nScrapCnt;

	int nCodeCount;
	int nCode[100];
	CString strCode[100];
};
extern struct tagLOT_DISPLAY_INFO st_lot_display_info[2];

//
// lot ����.....
struct tagLOT_INFO
{
	/*
	//////
	//LOT ���� ���� ����, Max 10������ ������ ������ �ÿ��� �ִ� 3�� ������ ���� �����ϰ� �ϰ�, 3�� �̻��϶��� Input smema ����Ʈ���� ����ϰ� ����  
	int			nLotWork_Count; //���� ��� ������ �������� lot�� ����, Max 10������ ������ ������ �ÿ��� �ִ� 3�� ������ ���� �����ϰ� �ϰ�, 3�� �̻��϶��� Input smema ����Ʈ���� ����ϰ� ����  
	int			nLot_LastModule[10]; //�� LOT�� LAST MODULE ����
    CString     strLot_Name[10];     //�� LOT�� NAME
	CString		strCurrent_LotName; //���� ���ʷ� �۾����̴� �ֽ� lot �� ����(nLot_LastModule[0] ��° �迭�� ������ ���ƾ� �Ѵ� )
	*/
	CString strLotNo;				// lot no......
	CString strPartNo;				// part no......
	CString strProcess;				// process id.....
	CString strProPerty;			// property end....
	CString strBypass;				// bypass.....
	// jtkim 20160929
	CString strOpCode;				// opcode

	CString strFabSite;	          //2017.0105

	int		nLotNoApply_FirstTestStart;	//james 2016.0926 Next Lot�� test site(#1~#4) ������ Tester �� ����ϱ� �����Ѵٴ� �÷��� 

	
	int     nLotStatus; //0:clear, 1:set
	int		nLot_THD_Status[THREAD_MAX_SITE]; //james 2016.0804 nLotEnd[THREAD_MAX_SITE]; //�ش� ����Ʈ lot end �÷��� 
	int		nNewLotIn;				//[0]Label ��� new lot in , [1]:AOI ��� ����
	int		nLastModule;			// last module....//[0]Label ��� new lot in , [1]:AOI ��� ����
	int		nNewLotComplete;		// new lot check ����.....
	int		nTotalCnt;				// total count....
	int		nLane;					// ......
	int		nLdTrayCnt;				// loading tray ����.....
	int		nUnLdTrayCnt;			// unloading tray 
	int		nBinSortCnt[3];			//0:good, 1:reject, 2:retest ���� //2015.0225 james 
	int		nLdCnt;
	int		nTrayRunMode_StandAlone; //Ʈ���̸� �� �������� �������ʰ� ��ε� ����Ŀ�� �����ϴ� ��� 
	int		nScrapCode;				// scrap code check.....

	COleDateTime	tStart;			// jtkim 20150408

	// jtkim 20160108
	CTime	tLotStart;

	// jtkim 20160124
	int		nAbortTime;
	int		nAbortTotal;
	int		nAbortCount;
	int		nAbortUsed;	

	 //james 2016.0805, lot ���� �Է��� �Ϸ�Ǹ鼭 ���� �ʿ� 
	int		nStacker_Good_THD_Num; //lot���� ����� ��δ� ����Ŀ�� ����(THD_ULD_1_STACKER,THD_ULD_2_STACKER)
	int		nStacker_Reject_THD_Num; //lot���� ����� ���� ����Ŀ�� ����(THD_REJECT_OUT_1_STACKER, THD_REJECT_OUT_2_STACKER)
	int		nBuffer_Retest_THD_Num; //THD_RETEST_1_BUFF:���� ���׽�Ʈ, THD_RETEST_2_BUFF:���� ���׽�Ʈ ���� 

	// jtkim 20160811
	int		nTestMode;				// ONESHOT, HOT, COLD, GATE......
	int		nInterface;				// ftp / local......
	int		nRetry;					// retry count.....
	int		nBarcodeMode;			// barcode ���ۼ��� (1��/2��)
	int		nQty;					// lot ����.....
	int		nModuleCount;			//
	int		nPgmDown;				// pgm download �Ϸ�......
	int		nChamberDoor;			// chanber door open......

	// jtkim 20161008 ��ǰ test �ð�......
	int		nBdTime;
	int		nBdTimeCount;
};
extern tagLOT_INFO st_lot_info[3]; //james 2016.0820 [0]:CURRENT LOT. [1]:NEXT LOT, [2]:BACKUP LOT(CURRENT Lot Backup) 

struct tagANIMATE_INFO
{
	double	dMaxPos[30];		// animate �ִ� pulse.....
	double  dMovePos[30];		// ���� motor pulse .......
	double	dMotorDistance[30];
	double	dMoveOffset[30];

	int		nMoveDistance[30];
	int		nMoveOffset[30];
	int		nImgNum[30];		// image ��ȣ......
	int		nDistance[30];

	bool	bTrayFlag[9];		// Conveyor Assy ���� Tray ǥ�� Flag 0=LdCvyFr 1=LdCvyRe 2=LdRailFr 3=LdRailRe 4=EmptyStk 5=UnldRail 6=UnldElv 7=UnldCvy 8=MvCvy;
};
extern tagANIMATE_INFO st_animate_info;

struct tagCOUNT_INFO
{
	int nCokBufferCount[4]; //cok buffer�� cok ���� ����
	int nPcbQty;
	int nBadPcbQty;
	int	nPcbLotMoveCount[10];		// ���޵� Pcb ����......
	int	nJigCount;					// jig ����......
	int	nPcbLotMove;				// Lot Count.......
	int	nLatestUph[24];				// Uph.......

	int nSSD_Input[2];				//loading tray [0]:lot ����, [1]:DAILY ���� 
	int nTest_Site[2][2][12];		//test site �۾� ���� 
	int nSSD_Output[2][20];			//��ε� �� ���� //[0]:lot ����, [1]:DAILY ���� 

	int nEmptyStackerTray;			//����� Ʈ���� ���� 
	int nUnldStackerTray;			//Ʈ���� ���� 
	int nRejectTray;				// reject tray����....

	// count ǥ���ϱ� ����
	long nInCount[2][2];			// [0][0]:lot ����, [0][1]:lot ���� [1][0]:DAILY ���� 
	long nPassCount[2][2];			// ��ǰ����.....
	long nPrimeCount[2][2];			// nPrimeCount........
	long nRejectCount[2][2];
	// jtkim 20150307
	long nPrimeRejectCount[2][2];

	int nUph;
	int nDailyUph;
	// jtkim 20160106
	int nUphCnt;

	// jtkim 20160111
	int nSocketStart;
	int nSocketOff;
	int nSocketOn;

	double dDailyPer;
	double dHourPer;

	int nHourSocket;

	//james  2016.0804 
	int     nLoadTray_Cnt; //2016.0817
	int		nUnLdStacker_Good_TrayCnt[10];;			// unloading stacker  tray count
	int		nUnLdStacker_Reject_TrayCnt[10];;			// unloading stacker tray count
};
extern struct tagCOUNT_INFO st_count_info;


struct tagWORKER_INFO
{
	CString strWorker_Name;
	CString strWorker_Crem;
	CString strWorker_Num;

	CString strLotNo_Empty; //2016.0909
};
extern struct tagWORKER_INFO st_worker_info;

struct tagPART_INFO
{
	CString strPartNo[30];
	CString strType[30];
	CString strDate[30];

	int nDataCnt;
};
extern struct tagPART_INFO st_part_info;

struct tagTYPE_INFO
{
	CString strType[30];
	CString strDate[30];

	int nDataCnt;
};
extern struct tagTYPE_INFO st_type_info;

struct tagLOT_HISTORY_INFO
{
	CString strLotNo;
	CString strPartNo;
	int		nTotalCnt;
	int		nPassCnt;
};
extern struct tagLOT_HISTORY_INFO st_lot_history_info;


enum ANI_PART_NUM
{
	ANI_LF_TSITE		=	0,
	ANI_RI_TSITE		,
	ANI_CVY_ASSY		,
	ANI_LD_BUFF			,
	ANI_LD_RBT_X		,
	ANI_LD_RBT_Y		,
	ANI_UNLD_BUFF		,
	ANI_UNLD_RBT_X		,
	ANI_UNLD_RBT_Y		,
	ANI_RETEST_BUFF		,
	ANI_REJECT_BUFF		,
	ANI_COK_BUFF		,
	ANI_TSITE_RBT_X		,
	ANI_TSITE_RBT_Y		,
	ANI_LD_CVY_FRONT	,	
	ANI_LD_CVY_REAR		,
	ANI_LD_RAIL_FRONT	,
	ANI_LD_RAIL_REAR	,
	ANI_EMPTY_STACKER	,
	ANI_UNLD_RAIL		,
	ANI_UNLD_ELV		,
	ANI_UNLD_CVY		,
	ANI_MV_CVY			,
};

enum WAIT_TIME
{
	WAIT_STOPPER_UP_DN						= 0,
	WAIT_STACKER_UP_DN						,
	WAIT_LDTRAYPLATE_FWD_BWD				,
	WAIT_RBTPICKER_UP_DN					,
	WAIT_RBTROTATOR_ON_OFF					,
	WAIT_RBTGLIPPER_ON_OFF					,
	WAIT_TRANSFER_UP_DN						,
	WAIT_TRANSFER_VACUUM_ON_OFF				,
	WAIT_TRANSFER_EJECT_ON_OFF				,
	WAIT_PUSHER_ON_OFF						,
	WAIT_TEST_SITE_UP_DN					,
	MAX_WAIT_TIME							,
};

// *************************************************************************
// I/O ���� ��� �ð� �� ����Ʈ �ð� ���� ����ü ����                            
// *************************************************************************

struct tagWAIT_INFO
{
	int	nOnWaitTime[MAX_WAIT_TIME];
	int nOffWaitTime[MAX_WAIT_TIME];
	int nLimitWaitTime[MAX_WAIT_TIME];


	//at loader
	int nGripOnOffWaitTime;
	int nPickUpDnWaitTime;
};
extern  tagWAIT_INFO  st_wait_info;

struct tagSAFETY_INFO
{
	// test site y�� ����üũ
	double dTsiteTopPlaceY;		// test site top y���� ������ġ üũ (z���� �������� ������ �ϰ��� �˶�)
	double dTsiteBottomPlaceY;  // test site bottom y�� ������ġ üũ (z���� �������� ������ �ϰ��� �˶�)
	double dTsiteTopPlaceZ;		// test site top �۾��� y���� �������� ���Խ� Z�� �������� üũ
	double dTsiteBottomPlaceZ;	// test site bottom �۾��� y���� �������� ���Խ� Z�� �������� üũ

	double dTsitePickMinX;		// test site pick min (buffer y, rotator ��������)
	double dTsitePickMaxX;		// test site pick max (buffer y, rotator ��������)
	double dTsitePickMinY;
	double dTsitePickMaxY;
	double dTsitePickZ;
	double dLdBufPlace;
	double dLdBufPick;
	double dLdRbtPlaceMinX;
	double dLdRbtPlaceMaxX;
	double dLdRbtPlaceMinY;
	double dLdRbtPlaceMaxY;
	double dLdRbtPickMinX;
	double dLdRbtPickMaxX;
	double dLdRbtPickMinY;
	double dLdRbtPickMaxY;

	double dTsitePlaceMinX;		// test site pick min (buffer y, rotator ��������)
	double dTsitePlaceMaxX;		// test site pick max (buffer y, rotator ��������)
	double dTsitePlaceMinY;
	double dTsitePlaceMaxY;
	double dTsitePlaceZ;
	double dUnLdBufPlace;
	double dUnLdBufPick;
	double dUnLdRbtPlaceMinX;
	double dUnLdRbtPlaceMaxX;
	double dUnLdRbtPlaceMinY;
	double dUnLdRbtPlaceMaxY;
	double dUnLdRbtPickMinX;
	double dUnLdRbtPickMaxX;
	double dUnLdRbtPickMinY;
	double dUnLdRbtPickMaxY;
	double dUnLdRbtRejectMinX;
	double dUnLdRbtRejectMaxX;
	double dUnLdRbtRejectMinY;
	double dUnLdRbtRejectMaxY;

	double dLdShiftX;
	double dUnLdShiftX;

	double dBarcodeZ;

	double dLdRbtZ;
	double dUnLdRbtZ;

	double dCokZ;
};
extern tagSAFETY_INFO st_safety_info;


struct tagSCRAP_CODE
{
	int		m_nScrapCode[10000][2]; //scrap code data.....
	int		m_nScrapBin;			// scrap code bin......
};
extern tagSCRAP_CODE st_code_info[2];

struct OFFSET_INFO
{
	double	md_Offset[5];
};
extern  OFFSET_INFO st_Offset_info;


//2015.1005
struct tagdbtime_database
{
	int				mndb;

	COleDateTime	mole_date;
	char            str_slot[100];

	CTimeSpan		mn_Ref_time[TSITE_SOCKET_CNT];
	CTimeSpan		mn_run_time[TSITE_SOCKET_CNT];
	CTimeSpan		mn_test_time[TSITE_SOCKET_CNT];
	float			mf_Run[TSITE_SOCKET_CNT];
	float           mf_test[TSITE_SOCKET_CNT];
	CTimeSpan       mn_lt_Rundown[TSITE_SOCKET_CNT];
	CTimeSpan       mn_lt_Socketoff[TSITE_SOCKET_CNT];
	CTimeSpan       mn_lt_instandby[TSITE_SOCKET_CNT];
	CTimeSpan       mn_lt_outReady[TSITE_SOCKET_CNT];
	CTimeSpan       mn_lt_stop[TSITE_SOCKET_CNT];
	CTimeSpan       mn_lt_User_stop[TSITE_SOCKET_CNT];

	float           mf_lr_Rd[TSITE_SOCKET_CNT];
	float           mf_lr_SocketOff[TSITE_SOCKET_CNT];
	float           mf_lr_instandby[TSITE_SOCKET_CNT];
	float           mf_lr_outReady[TSITE_SOCKET_CNT];
	float           mf_lr_stop[TSITE_SOCKET_CNT];
	float           mf_lr_userstop[TSITE_SOCKET_CNT];

	int             mn_Output[TSITE_SOCKET_CNT];
	double          md_Yield[TSITE_SOCKET_CNT];	
	int				mn_Pass[TSITE_SOCKET_CNT];
	int				mn_Fail[TSITE_SOCKET_CNT];
	int				mn_Input[TSITE_SOCKET_CNT];

};
extern struct tagdbtime_database	st_dbtime;

struct tagDB_time_param 
{
	CTimeSpan		n_Db_time[TSITE_SOCKET_CNT][TSITE_SOCKET_CNT];
	int				mn_Pass[TSITE_SOCKET_CNT];
	int				mn_Fail[TSITE_SOCKET_CNT];
	int				mn_Input[TSITE_SOCKET_CNT];
	int             mn_Output[TSITE_SOCKET_CNT];
	float           mn_Yield[TSITE_SOCKET_CNT];
};
extern  tagDB_time_param  st_DB_time;

struct tagBuffer_DB_database
{
	int				mndb;

	COleDateTime	mole_date;
	char            str_slot[100];

	CTimeSpan		mn_Ref_time[48];
	CTimeSpan		mn_run_time[48];
	CTimeSpan		mn_test_time[48];
	float			mf_Run[48];
	float           mf_test[48];
	CTimeSpan       mn_lt_Rundown[48];
	CTimeSpan       mn_lt_Socketoff[48];
	CTimeSpan       mn_lt_instandby[48];
	CTimeSpan       mn_lt_outReady[48];
	CTimeSpan       mn_lt_stop[48];
	CTimeSpan       mn_lt_User_stop[48];

	float           mf_lr_Rd[20];
	float           mf_lr_SocketOff[48];
	float           mf_lr_instandby[48];
	float           mf_lr_outReady[48];
	float           mf_lr_stop[48];
	float           mf_lr_userstop[48];

	int             mn_Output[48];
	double          md_Yield[48];
	int				mn_Pass[48];
	int				mn_Fail[48];
	int				mn_Input[48];

};
extern struct tagBuffer_DB_database	st_BufferTemp;

struct tagdbTimeTmp_database
{
	char            str_startDate[100];
	char            str_EndDate[100];
	char            str_slot[100];

	CTimeSpan		mn_Ref_time;
	CTimeSpan		mn_run_time;
	CTimeSpan		mn_test_time;
	float			mf_Run;
	float           mf_test;
	CTimeSpan       mn_lt_Rundown;
	CTimeSpan       mn_lt_Socketoff;
	CTimeSpan       mn_lt_instandby;
	CTimeSpan       mn_lt_outReady;
	CTimeSpan       mn_lt_stop;
	CTimeSpan       mn_lt_User_stop;

	float           mf_lr_Rd;
	float           mf_lr_SocketOff;
	float           mf_lr_instandby;
	float           mf_lr_outReady;
	float           mf_lr_stop;
	float           mf_lr_userstop;

	int             mn_Output;
	int             mn_Pass;
	double          md_Yield;
	int             mn_Cnt;
	int             mn_Interval;

};
extern struct tagdbTimeTmp_database	*st_dbTimeTmp;
//////////////

struct tagPRODUCT_INFO
{
	int					nUph;				// �ð��� ���귮.....
	int					nSocketOff;			// �ð��� Socket Off......
	int					nCok;				// �ð��� Cok ��ü.....
	long				nRunTime;			// �ð��� ���� �ð�.......
	long				nStopTime;			// �ð��� stop �ð�......
	long				nIdleTime;			// �ð��� idle �ð�.....
	long				nFrontTime;			// �ð��� idle �ð�.....
	long				nAlarmTime;			// �ð��� alarm �ð�.....
	int					nAlarmCount;		// �ð��� alarm ����....
	int					nRetest;			// �ð��� retest �߻�Ƚ��.....
	double				dMtbi;				// �ð��� mtbi.....
	
	double				dCycleTime;
	int					nCycleCount;
	double				dTestTime;
	int					nTestCount;
	double				dPrime;
	int					nPrimeCount;
	int					nRejectCount;
};
extern tagPRODUCT_INFO st_product_info;

enum RECOVERY_CYL
{
	RECO_LD_CVY_BUFF_STOPPER					= 0,
	RECO_LD_CVY_WAIT_STOPPER					,
	RECO_LD_CVY_WAIT_ALIGN						,
	RECO_LD_TRAY_RAIL_ALIGN						,
	RECO_LD_TRAY_RAIL							,
	RECO_LD_TRAY_RAIL_LOW_TRAY_PUSHER			,
	RECO_LD_TRAY_INDEX_TRAY_CLAMP				,
	RECO_UNLD_TRAY_EMPTY_HI_RAIL				,
	RECO_UNLD_TRAY_EMPTY_LOW_RAIL				,
	RECO_UNLD_TRAY_RAIL_ALIGN					,
	RECO_UNLD_TRAY_RAIL							,
	RECO_UNLD_TRAY_INDEX_TRAY_CLAMP				,
	RECO_UNLD_MV_CVY_TRAY_STOPPER				,
	RECO_RETEST_BUFFER_MOVE						,
	RECO_RETEST_BUFFER_ALIGN					,
	RECO_LD_RBT_PICK_VAC_1						,
	RECO_LD_RBT_PICK_VAC_2						,
	RECO_LD_RBT_PICK_UPDN_1						,
	RECO_LD_RBT_PICK_UPDN_2						,
	RECO_TSITE_RBT_COK_UPDN						,
	RECO_TSITE_RBT_COK_GRIP						,
	RECO_TSITE_RBT_PICK_UPDN_1					,
	RECO_TSITE_RBT_PICK_UPDN_2					,
	RECO_TSITE_RBT_PICK_UPDN_3					,
	RECO_TSITE_RBT_PICK_UPDN_4					,
	RECO_TSITE_RBT_PICK_VAC_1					,
	RECO_TSITE_RBT_PICK_VAC_2					,
	RECO_TSITE_RBT_PICK_VAC_3					,
	RECO_TSITE_RBT_PICK_VAC_4					,
	RECO_TSITE_RBT_PICKER_EJECT					,
	RECO_UNLD_RBT_PICK_VAC_1					,
	RECO_UNLD_RBT_PICK_VAC_2					,
	RECO_UNLD_RBT_PICK_UPDN_1					,
	RECO_UNLD_RBT_PICK_UPDN_2					,
	RECO_LF_TEST_SITE_TOP_COK_CLAMP				,
	RECO_LF_TEST_SITE_BTM_COK_CLAMP				,
	RECO_LF_TEST_SITE_TOP_CONTACT				,
	RECO_LF_TEST_SITE_BTM_CONTACT				,
	RECO_LF_TEST_SITE_TOP_INSERT_1				,
	RECO_LF_TEST_SITE_TOP_INSERT_2				,
	RECO_LF_TEST_SITE_TOP_INSERT_3				,
	RECO_LF_TEST_SITE_TOP_INSERT_4				,
	RECO_LF_TEST_SITE_TOP_INSERT_5				,
	RECO_LF_TEST_SITE_TOP_INSERT_6				,
	RECO_LF_TEST_SITE_BTM_INSERT_1				,
	RECO_LF_TEST_SITE_BTM_INSERT_2				,
	RECO_LF_TEST_SITE_BTM_INSERT_3				,
	RECO_LF_TEST_SITE_BTM_INSERT_4				,
	RECO_LF_TEST_SITE_BTM_INSERT_5				,
	RECO_LF_TEST_SITE_BTM_INSERT_6				,
	RECO_RI_TEST_SITE_TOP_COK_CLAMP				,
	RECO_RI_TEST_SITE_BTM_COK_CLAMP				,
	RECO_RI_TEST_SITE_TOP_CONTACT				,
	RECO_RI_TEST_SITE_BTM_CONTACT				,
	RECO_RI_TEST_SITE_TOP_INSERT_1				,
	RECO_RI_TEST_SITE_TOP_INSERT_2				,
	RECO_RI_TEST_SITE_TOP_INSERT_3				,
	RECO_RI_TEST_SITE_TOP_INSERT_4				,
	RECO_RI_TEST_SITE_TOP_INSERT_5				,
	RECO_RI_TEST_SITE_TOP_INSERT_6				,
	RECO_RI_TEST_SITE_BTM_INSERT_1				,
	RECO_RI_TEST_SITE_BTM_INSERT_2				,
	RECO_RI_TEST_SITE_BTM_INSERT_3				,
	RECO_RI_TEST_SITE_BTM_INSERT_4				,
	RECO_RI_TEST_SITE_BTM_INSERT_5				,
	RECO_RI_TEST_SITE_BTM_INSERT_6				,
	RECO_LF_HI_FIX_CONTACT						,
	RECO_RI_HI_FIX_CONTACT						,
	RECO_REJECT_PUSHER							,
	RECO_REJECT_CLAMP							,
	RECO_REJECT_GRIPPER_UPDN					,
	RECO_REJECT_ALIGN							,
	RECO_LF_MP_CLAMP							,
	RECO_RI_MP_CLAMP							,
	RECO_MAX_COUNT								,
};

enum EQP_MCMD
{
	EQP_OFF_LINE					= 1,
	EQP_ON_LINE						= 4,
};

enum MES_MCMD
{
	MES_OFF_LINE					= 0,
	MES_ON_LINE_LOCAL				= 4,
	MES_ON_LINE_REMOTE				= 5,
};


enum LINK_STATUS
{
	LINK_DISCONNECT					= 0,
	LINK_CONNECT					= 1,
};


enum PROC_STATE
{
	PROC_UNKNOWN					= 0,
	PROC_IDLE						,
	PROC_RUN						,
	PROC_DOWN						,
	PROC_MAINT						,
};

enum  THREAD_PART
{
	PART_MGZ_ELV					= 0,
	PART_PCB_SHUTTLE				,
	PART_PCB_ROBOT					,
	PART_JIG_ROBOT					,
	PART_JIG_ELV					,
	PART_JIG_MOVE_CVY				,
	PART_JIG_SPRT_CVY				,
	PART_MGZ_IN_CVY					,
	PART_MGZ_OUT_CVY				,
	PART_REJECT_TH					,
};


enum EQ_WITH_MODE_STATE
{
	WITHOUT_DVC						= 0,
	WITH_DVC						= 1,
	WITHOUT_TRAY					= 3,
};

enum BCR_STATE
{
	BCR_USE							= 1,
	BCR_NOT_USE						= 0,
	
};




enum EQ_WORK_MODE_STATE
{
	WORK_BYPASS						= 0,
	WORK_JIG						= 1,
};

#define DN							0
#define UP							1

#define MOVE_FORWARD				0
#define MOVE_BACKWARD				1
//kwlee 2016.0614

///////////////////////////////////////////////////////////////
//2016.0631 james 
//////////////////////////////////////////////////////////////
enum MOTOR_NUM //���� ����
{
	///////////////////////////////////////////////////////////////////////////////////////////////	 
	// ��� ������ loadd/unload stacker ���� �� ���� ������ �������� ������ �����Ѵ�
	///////////////////////////////////////////////////////////////////////////////////////////////
	// 1��° ����(8��)
	M_LD_CV_LR_SHIFTER			 = 0,			//0: loading tray conveyor left/right shifter
	M_LD_ELV						,			//1: loading tray stacker elevator
	M_EMPTY_ELV						,			//2: empty tray stacker elevator 
	M_ULD_1_ELV						,			//3: unload tray #1(left) stacker elevator 
	M_ULD_2_ELV						,		    //4: unload tray #2(right) stacker elevator 
	M_RJ_EMPTY_ELV					,			//5: reject tray empty stacker elevator 
	M_RJ_ULD_1_ELV					,			//6: reject tray #1(front) stacker elevator
	M_RJ_ULD_2_ELV					,  			//7: reject tray #2(rear) stacker elevator
	// 2��° ����(8��)	
	M_WORK_RBT_Y					,			//8: work robot x(work robot == load/unload robot)
	M_WORK_RBT_X					,			//9: work robot y
	M_WORK_RBT_Z					,			//10: work robot z
	M_LD_BUF_SHIFTER_Y				,			//11: load buffer shifter Y
	M_LD_BUF_ROTATOR				,			//12: load buffer rotator
	M_ULD_BUF_SHIFTER_Y				,           //13: unload buffer shifter Y
	M_ULD_BUF_ROTATOR				,			//14: unload buffer rotator
	M_SPARE_1						,			//15: spare 1
	// 3��° ����(8��)	
	M_RETEST_BUF_1_ROTATOR			,			//16: retest buffer #1(left) rotator
	M_RETEST_BUF_2_ROTATOR			,			//17: retest buffer #2(right) rotator
	M_WORKTRAY_TRANSFER				,			//18: work tray transfer
	M_REJECTTRAY_TRANSFER			,			//19: reject tray transfer
	M_TEST_RBT_Y					,			//20: test robot x
	M_TEST_RBT_X					,			//21: test robot y
	M_TEST_RBT_Z					,			//22: test robot z
	M_TEST_DVC_EJECT_Z				,			//23: test robot device eject z

	M_MAX_MOT_NUM					, //24
};

enum M_LD_CV_LR_SHIFTER //0 //load conveyor Left/Right shifter
{
	P_LDCV_SFT_SAFETY		= 0,	
	P_LDCV_SFT_LEFT		   ,
	P_LDCV_SFT_RIGHT		   ,
};

enum M_TRAY_STACKER_ELEVATOR //1~7�� ���� (M_LD_ELV ~ M_RJ_ULD_2_ELV ����) //��� elevtor ���ʹ� ���� ������� ƼĪ
{
	P_ELV_SAFETY		  = 0,//���� ��ġ (down limit ��ġ�ϰ���)
	P_ELV_UP_LIMIT		    ,  //high rail ���� �� Ʈ���� ���� �Ǵ� Ʈ���̸� ������  ���� ���ʱ��� �ø��� üũ
	P_ELV_DN_LIMIT		    ,  //���ô� Ʈ���� ���� �� soflEo dncl tkdyd, �Ǵ� ���� �ظ�, Ʈ���� ���� üũ �Ǵ� Full�� ��ü�� �������� 	
	P_ELV_SD_SENSOR				, //sd sensor ���� ��ġ(Ʈ���̰� ���� �̻� ������ ������. �ϳ��� ������ �������� ���� 	

	P_ELV_TRAY_INOUT_POS,		//Ʈ���̸� ���� �Ǵ� �Է� ������ ��ġ(CONVEYOR ���� �����Ͽ� Ʈ���̸� �ְ�,  ���Ⱑ ������ ��ġ 

	//SD ���� ���������� ���� �̵��ϴ� ��ġ 
	P_ELV_SUPPLY_OFFSET	,	  // SD pos + Ʈ���̸� �۾� �����ϰ� �ϴ� ��ġ 
	P_ELV_RECEIVE_OFFSET	, // SD pos + Ʈ���̸� �޴� ��ġ 
};

enum M_WORK_RBT_X_Y //8,9�� ���� , loading/unload robot 
{
	P_WORKROBOT_SAFETY				= 0,	

	P_WORKROBOT_LDTRAY_START		   , 

	P_WORKROBOT_LD_BUFF_START		   , 
	P_WORKROBOT_ULD_BUFF_START		   , 

	P_WORKROBOT_ULDTRAY_1_START		   , 
	P_WORKROBOT_ULDTRAY_2_START		   , 

	P_WORKROBOT_REJECT_1_START		   , 
	P_WORKROBOT_REJECT_2_START		   , 

	P_WORKROBOT_BCR_1D_START_LD	   , //kwlee 2016.1007 P_WORKROBOT_BCR_1D_START -> P_WORKROBOT_BCR_1D_START_LD
	P_WORKROBOT_BCR_2D_START_LD		   ,
	P_WORKROBOT_BCR_2D_START_UNLD	   ,
	P_WORKROBOT_BCR_1D_START_UNLD	   ,
};

enum M_WORK_RBT_Z //10
{
	P_WORKROBOT_Z_SAFETY				= 0,	

	P_WORKROBOT_Z_LDTRAY_PICK		   ,
	P_WORKROBOT_Z_LDTRAY_PLACE		   ,

	P_WORKROBOT_Z_LDBUF_PICK		   ,
	P_WORKROBOT_Z_LDBUF_PLCAE		   ,

	P_WORKROBOT_Z_ULDBUF_PICK		   ,
	P_WORKROBOT_Z_ULDBUF_PLCAE		   ,

	P_WORKROBOT_Z_ULDTRAY_1_PLACE		,
	P_WORKROBOT_Z_ULDTRAY_2_PLACE		   ,

	P_WORKROBOT_Z_REJECTTRAY_1_PLACE		   ,
	P_WORKROBOT_Z_REJECTTRAY_2_PLACE		   ,

	P_WORKROBOT_Z_1D_LD_POS					, //kwlee 2016.1007 P_WORKROBOT_Z_1D_POS -> P_WORKROBOT_Z_1D_LD_POS
	P_WORKROBOT_Z_2D_LD_POS					  ,
	P_WORKROBOT_Z_2D_UNLD_POS					  ,
	P_WORKROBOT_Z_1D_UNLD_POS					, //kwlee 2016.1007 �߰�
};

enum M_LD_BUF_SHIFTER_Y //11
{
	P_LDBUFF_Y_SAFETY				= 0,	

	P_LDBUFF_Y_WORKRBT				,	
	P_LDBUFF_Y_TESTRBT				,	
};

enum M_LD_BUF_ROTATOR//12
{
	P_LDBUFF_RTR_SAFETY				= 0,	

	P_LDBUFF_RTR_WORKRBT		   ,	//�ε� ���� �޴� ��ġ�� ȸ�� �� 
	P_LDBUFF_RTR_TESTSITE_LEFT	   ,    //test site Left Site ��ġ�� �۾� ������ ȸ�� ��
	P_LDBUFF_RTR_TESTSITE_RIGHT	   ,	 //test site Right Site ��ġ�� �۾� ������ ȸ�� ��
};

enum M_ULD_BUF_SHIFTER_Y//13
{
	P_ULDBUFF_Y_SAFETY				= 0,	

	P_ULDBUFF_Y_WORKRBT				,	
	P_ULDBUFF_Y_TESTRBT				,	
};

enum M_ULD_BUF_ROTATOR//14�� ���� 
{
	P_ULDBUFF_RTR_SAFETY				= 0,
	P_ULDBUFF_RTR_WORKRBT		   ,	//workwobot�� ���縦 ���� �����Ѵ�  
	P_ULDBUFF_RTR_TEST_1_LEFT	   ,    //test site Left Site ��ġ���� ����� ���縦 ���� ��ġ  
	P_ULDBUFF_RTR_TEST_2_RIGHT	   ,	 //test site Right Site ��ġ���� ����� ���縦 ���� ��ġ  
};

//15�� ���� spare

enum M_RETEST_BUF_1_LEFT_ROTATOR	//16 �� ���� 
{
	P_RETEST_BUFF_1_SAFETY				= 0,	

	P_RETEST_BUFF_1_RTR_TESTSITE_LEFT	   ,    //test site Left Site ��ġ���� ����� ���縦 ���� ��ġ  
	P_RETEST_BUFF_1_RTR_TESTSITE_RIGHT	   ,	 //�������� ����, �ʱ���ġ , test site Right Site ��ġ���� ����� ���縦 ���� ��ġ  
};

enum M_RETEST_BUF_2_RIGHT_ROTATOR	//17 �� ���� 
{
	P_RETEST_BUFF_2_SAFETY				= 0,	

	P_RETEST_BUFF_2_RTR_TESTSITE_LEFT	   ,    //test site Left Site ��ġ���� ����� ���縦 ���� ��ġ  
	P_RETEST_BUFF_2_RTR_TESTSITE_RIGHT	   ,	 //�������� ����,  �ʱ���ġ , test site Right Site ��ġ���� ����� ���縦 ���� ��ġ  
};


enum M_WORKTRAY_TRANSFER //18
{
	P_WORKTRANS_SAFETY				= 0,	

	P_WORKTRANS_LD_STACKER		   ,
	P_WORKTRANS_EMPTY_STACKER	   , 
	P_WORKTRANS_ULD_1_STACKER	   ,
	P_WORKTRANS_ULD_2_STACKER	   ,
	P_WORKTRANS_REJECT_STACK	   ,//2017.0430
};

enum M_REJECTTRAY_TRANSFER //19
{
	P_REJECTTRANS_SAFETY				= 0,
	P_REJECTTRANS_EMPTY_STACKER		   , 
	P_REJECTTRANS_REJECT_1_STACKER	   ,
	P_REJECTTRANS_REJECT_2_STACKER	   ,	
};

enum M_TEST_RBT_X_Y //20~21 ���� 
{
	P_TESTRBT_XY_SAFETY				= 0,	

	P_TESTRBT_XY_LDBUF_START				, 
	P_TESTRBT_XY_ULDBUF_START				 ,

	//P_TESTRBT_XY_RETESTBUF_1_TESTSITE_1_LEFT_START				 ,
	//P_TESTRBT_XY_RETESTBUF_1_TESTSITE_2_RIGHT_START				 ,
	//P_TESTRBT_XY_RETESTBUF_1_ULDBUF_START				 ,
	P_TESTRBT_XY_RETESTBUF_1_START				 ,

	//P_TESTRBT_XY_RETESTBUF_2_TESTSITE_1_LEFT_START				 ,
	//P_TESTRBT_XY_RETESTBUF_2_TESTSITE_2_RIGHT_START				 ,
	//P_TESTRBT_XY_RETESTBUF_2_ULDBUF_START				 ,
	P_TESTRBT_XY_RETESTBUF_2_START				 ,

	P_TESTRBT_XY_TESTDUCT_1_LEFT_START				 ,
	P_TESTRBT_XY_TESTDUCT_1_RIGHT_START				 ,
	P_TESTRBT_XY_TESTDUCT_2_LEFT_START				 ,
	P_TESTRBT_XY_TESTDUCT_2_RIGHT_START				 ,
	P_TESTRBT_XY_TESTDUCT_3_LEFT_START				 ,
	P_TESTRBT_XY_TESTDUCT_3_RIGHT_START				 ,
	P_TESTRBT_XY_TESTDUCT_4_LEFT_START				 ,
	P_TESTRBT_XY_TESTDUCT_4_RIGHT_START				 ,
	P_TESTRBT_XY_TESTDUCT_5_LEFT_START				 ,
	P_TESTRBT_XY_TESTDUCT_5_RIGHT_START				 ,
	P_TESTRBT_XY_TESTDUCT_6_LEFT_START				 ,
	P_TESTRBT_XY_TESTDUCT_6_RIGHT_START				 ,
	P_TESTRBT_XY_TESTDUCT_7_LEFT_START				 ,
	P_TESTRBT_XY_TESTDUCT_7_RIGHT_START				 ,
	P_TESTRBT_XY_TESTDUCT_8_LEFT_START				 ,
	P_TESTRBT_XY_TESTDUCT_8_RIGHT_START				 ,

	//P_TESTRBT_XY_DVC_DIRECTION_CHK				 ,
	P_TESTRBT_XY_DVC_DIRECTION_LEFTSITE_CHK				 ,
	P_TESTRBT_XY_DVC_DIRECTION_RIGHTSITE_CHK				 ,
//kwlee 2016.1019
// 	P_TESTRBT_XY_AUTOTEACH_VISION_TEST_1_LEFT				 ,
// 	P_TESTRBT_XY_AUTOTEACH_VISION_TEST_2_RIGHT				 ,

	P_TESTRBT_XY_AUTOTEACH_VISION_TEST_1_LEFT				 ,
	P_TESTRBT_XY_AUTOTEACH_VISION_TEST_2_LEFT				 ,
	P_TESTRBT_XY_AUTOTEACH_VISION_TEST_3_LEFT				 ,
	P_TESTRBT_XY_AUTOTEACH_VISION_TEST_4_LEFT				 ,
	P_TESTRBT_XY_AUTOTEACH_VISION_TEST_1_RIGHT				 ,
	P_TESTRBT_XY_AUTOTEACH_VISION_TEST_2_RIGHT				 ,
	P_TESTRBT_XY_AUTOTEACH_VISION_TEST_3_RIGHT				 ,
	P_TESTRBT_XY_AUTOTEACH_VISION_TEST_4_RIGHT				 ,
	 /////
	
};
enum M_TEST_RBT_Z //22
{
	P_TESTRBT_Z_SAFETY				= 0,	

	P_TESTRBT_Z_LDBUF_PICK		   ,
	P_TESTRBT_Z_LDBUF_PLACE		   ,
	P_TESTRBT_Z_ULDBUF_PICK		   ,
	P_TESTRBT_Z_ULDBUF_PLACE		   ,

	P_TESTRBT_Z_RETESTBUF_1_PICK		   ,
	P_TESTRBT_Z_RETESTBUF_1_PLACE		   ,

	P_TESTRBT_Z_RETESTBUF_2_PICK		   ,
	P_TESTRBT_Z_RETESTBUF_2_PLACE		   ,

// 	P_TESTRBT_Z_TESTSITE_LEFT_PICK		   ,
// 	P_TESTRBT_Z_TESTSITE_LEFT_PLACE		   ,
// 	P_TESTRBT_Z_TESTSITE_LEFT_INSERT		   ,
// 	P_TESTRBT_Z_TESTSITE_LEFT_EJECT	   ,
// 
// 	P_TESTRBT_Z_TESTSITE_RIGHT_PICK		   ,
// 	P_TESTRBT_Z_TESTSITE_RIGHT_PLACE		   ,
// 	P_TESTRBT_Z_TESTSITE_RIGHT_INSERT		   ,
// 	P_TESTRBT_Z_TESTSITE_RIGHT_EJECT	   ,

	P_TESTRBT_Z_TESTSITE_1_LEFT_PICK		   ,
	P_TESTRBT_Z_TESTSITE_1_LEFT_PLACE		   ,
	P_TESTRBT_Z_TESTSITE_1_LEFT_INSERT		   ,
	P_TESTRBT_Z_TESTSITE_1_LEFT_EJECT	   ,

	P_TESTRBT_Z_TESTSITE_5_RIGHT_PICK		   ,
	P_TESTRBT_Z_TESTSITE_5_RIGHT_PLACE		   ,
	P_TESTRBT_Z_TESTSITE_5_RIGHT_INSERT		   ,
	P_TESTRBT_Z_TESTSITE_5_RIGHT_EJECT	   ,

	P_TESTRBT_Z_DVC_DIRECTION_CHK	   ,

	P_TESTRBT_Z_AUTOTEACH_VISION_TESTSITE_LEFT	   ,
	P_TESTRBT_Z_AUTOTEACH_VISION_TESTSITE_RIGHT	   ,

	//kwlee 2016.1117
	P_TESTRBT_Z_TESTSITE_2_LEFT_PICK		   ,
	P_TESTRBT_Z_TESTSITE_2_LEFT_PLACE		   ,
	P_TESTRBT_Z_TESTSITE_2_LEFT_INSERT		   ,
	P_TESTRBT_Z_TESTSITE_2_LEFT_EJECT	   ,

	P_TESTRBT_Z_TESTSITE_3_LEFT_PICK		   ,
	P_TESTRBT_Z_TESTSITE_3_LEFT_PLACE		   ,
	P_TESTRBT_Z_TESTSITE_3_LEFT_INSERT		   ,
	P_TESTRBT_Z_TESTSITE_3_LEFT_EJECT	   ,

	P_TESTRBT_Z_TESTSITE_4_LEFT_PICK		   ,
	P_TESTRBT_Z_TESTSITE_4_LEFT_PLACE		   ,
	P_TESTRBT_Z_TESTSITE_4_LEFT_INSERT		   ,
	P_TESTRBT_Z_TESTSITE_4_LEFT_EJECT	   ,

	P_TESTRBT_Z_TESTSITE_6_RIGHT_PICK		   ,
	P_TESTRBT_Z_TESTSITE_6_RIGHT_PLACE		   ,
	P_TESTRBT_Z_TESTSITE_6_RIGHT_INSERT		   ,
	P_TESTRBT_Z_TESTSITE_6_RIGHT_EJECT	   ,

	P_TESTRBT_Z_TESTSITE_7_RIGHT_PICK		   ,
	P_TESTRBT_Z_TESTSITE_7_RIGHT_PLACE		   ,
	P_TESTRBT_Z_TESTSITE_7_RIGHT_INSERT		   ,
	P_TESTRBT_Z_TESTSITE_7_RIGHT_EJECT	   ,

	P_TESTRBT_Z_TESTSITE_8_RIGHT_PICK		   ,
	P_TESTRBT_Z_TESTSITE_8_RIGHT_PLACE		   ,
	P_TESTRBT_Z_TESTSITE_8_RIGHT_INSERT		   ,
	P_TESTRBT_Z_TESTSITE_8_RIGHT_EJECT	   ,

	P_TESTRBT_Z_TESTSITE_MISSLOAD_CHECK, //2017.1119
	P_TESTRBT_Z_RETEST_MISSLOAD_CHECK,	
};
enum M_TEST_DVC_EJECTOR_Z //23
{
	P_DVC_EJECTOR_Z_SAFETY				= 0,	

	P_TESTSITE_1_LEFT		   ,
	P_TESTSITE_2_RIGHT		   ,
};
//2016.0730 james

enum OFFSET_RESULT
{
	O_LD_TRAY_OFFSET = 0,
	O_LD_BUFF_OFFSET,
	O_TEST_X_OFFSET,
	O_TEST_Y_OFFSET,
	O_RETEST_X_OFFSET,
	O_TOTAL_OFFSET,
};

/* james 2016.0731 
//////////////////////////////////
//kwlee 2016.0614 ���� ����
enum MOTOR_NUM_OD
{
	M_BARCODE_X						= 0,
	M_BARCODE_Y						,
	M_BARCODE_Z						,
	M_LD_TRAY_SHIFTER				,
	M_UNLD_TRAY_SHIFTER				,
	M_EMPTY_TRAY_ELV				,
	M_LD_BUFF_ROTATOR				,
	M_LD_BUFF_Y						,
	M_UNLD_BUFF_ROTATOR				,
	M_UNLD_BUFF_Y					,
	M_COK_STK_X						,
	M_COK_STK_Z						,
	M_LD_RBT_X						,
	M_LD_RBT_Y						,
	M_LD_RBT_Z						,
	M_LD_RBT_P						,
	M_UNLD_RBT_X					,
	M_UNLD_RBT_Y					,
	M_UNLD_RBT_Z					,
	M_UNLD_RBT_P					,
	M_TSITE_RBT_X					,
	M_TSITE_RBT_Y					,
	M_TSITE_RBT_Z					,
	M_LF_MP_Z						,
	M_RI_MP_Z						,
	M_MV_TRAYOUT_CV_X				,
	M_GOOD_TRAY_ELV					,
	M_REJ_RBT_Z_EMPTY				,
	M_REJ_RBT_Z_FULL				,
	M_MAX_MOTOR_NUM					, //29
};


//kwlee 2016.0614 ���� ����
enum M_BARCODE_X_POS
{
	P_BARCODE_X_SAFETY				= 0,	
	P_BARCODE_X_TRAY_FIRST			   ,
};

enum M_BARCODE_Y_POS
{
	P_BARCODE_Y_SAFETY				= 0,
	P_BARCODE_Y_TRAY_FIRST			   ,
};

enum M_BARCODE_Z_POS
{
	P_BARCODE_Z_SAFETY				= 0,
	P_BARCODE_Z_WORK				  ,
									
};

enum M_LD_TRAY_SHIFTER_POS
{
	P_LD_TRAY_SHIFTER_SAFETY			= 0,	
	P_LD_TRAY_SHIFTER_PICK			,	
	P_LD_TRAY_SHIFTER_PLACE			   ,	
};

enum M_UNLD_TRAY_SHIFTER_POS
{
	P_UNLD_TRAY_SHIFTER_SAFETY		= 0,	
	P_UNLD_TRAY_SHIFTER_PICK		,	
	P_UNLD_TRAY_SHIFTER_PLACE		   ,	
};

enum M_EMPTY_TRAY_ELV_POS
{	
	P_EMPTY_TRAY_ELV_SAFETY		  = 0,//���� ��ġ (down limit ��ġ�ϰ���)
	P_EMPTY_TRAY_ELV_UP_LIMIT			,  //high rail ���� �� Ʈ���� ���� �Ǵ� Ʈ���̸� ������  ���� ���ʱ��� �ø��� üũ
	P_EMPTY_TRAY_ELV_DOWN_LIMIT		,  //���ô� Ʈ���� ���� �� soflEo dncl tkdyd, �Ǵ� ���� �ظ�, Ʈ���� ���� üũ �Ǵ� Full�� ��ü�� �������� 	
	P_EMPTY_TRAY_ELV_SDSENSOR		, //sd sensor ���� ��ġ(Ʈ���̰� ���� �̻� ������ ������. �ϳ��� ������ �������� ���� 	

	//SD ���� ���������� ���� �̵��ϴ� ��ġ 
	P_EMPTY_TRAY_ELV_RAIL_HIGH_SUPPLY_OFFSET	, // SD pos + high Up Ʈ���̸� �����ϴ� 
	P_EMPTY_TRAY_ELV_RAIL_HIGH_RECEIVE_OFFSET	, // SD pos + high Up Ʈ���̸� ����Ʈ���� �޴� ��ġ( �ʱ�ȭ�� Ʈ���̸� ����Ŀ�� ������ ���) 
	P_EMPTY_TRAY_ELV_RAIL_LOW_RECEIVE_OFFSET	, // SD pos + up + Low up Ʈ���̸� �޴� ��ġ, �⺻ ����ġ�� �̰����� ���� �������� ��� �̵��Ѵ� 
	P_EMPTY_TRAY_ELV_REFERENCE_POSITION_OFFSET	, //SD pos - down	��ġ�� �� ������ �۾��ϴ� �������̴�(SD ������ Down�Ͽ� SD�� ���� �ȵǴ� ��ġ�� �۾� �������̴�)

};

enum M_LD_BUFF_ROTATOR_POS
{
	P_LD_BUFF_ROTATOR_SAFETY		= 0,	
	P_LD_BUFF_ROTATOR_0				,
	P_LD_BUFF_ROTATOR_90			,
	P_LD_BUFF_ROTATOR_180			,
	P_LD_BUFF_ROTATOR_270			,
};

enum M_LD_BUFF_Y_POS
{
	P_LD_BUFF_Y_SAFETY				= 0,	
	P_LD_BUFF_Y_PLATE_LOAD			,	
	P_LD_BUFF_Y_TEST_UNLOAD			,	
	P_LD_BUFF_Y_ROTATOR				,
};

enum M_UNLD_BUFF_ROTATOR_POS
{
	P_UNLD_BUFF_ROTATOR_SAFETY			= 0,	
	P_UNLD_BUFF_ROTATOR_0				,
	P_UNLD_BUFF_ROTATOR_90				,
	P_UNLD_BUFF_ROTATOR_180				,
	P_UNLD_BUFF_ROTATOR_270				,
	P_UNLD_BUFF_BARCODE					,
};

enum M_UNLD_BUFF_Y_POS
{
	P_UNLD_BUFF_Y_SAFETY				= 0,	
	P_UNLD_BUFF_Y_TEST_LOAD				,	
	P_UNLD_BUFF_Y_PLATE_UNLOAD			,	
	P_UNLD_BUFF_Y_ROTATOR				,
};

enum M_COK_STK_X_POS
{
	P_COK_STK_X_SAFETY				= 0,
	P_COK_STK_X_FIRST				,
	P_COK_STK_X_SECOND				,
};

enum M_COK_STK_Z_POS
{
	P_COK_STK_Z_SAFETY				= 0,
	P_COK_STK_Z_UP_LAST_POS  		,			// Cok 1�� ��ġ
	P_COK_STK_Z_DOWN_FIRST_POS  	,			// Cok Full ��ġ
	P_COK_STK_Z_OFFSET_POS  		,			 // cok z�� ��ĭ���� GAP ����


	P_COK_STK_Z_UP_OFFSET			,
	P_COK_STK_Z_DOWN_OFFSET			,
	P_COK_STK_Z_LIMIT				,			// Cok Limit
};

enum M_LD_RBT_X_POS
{
	P_LD_RBT_X_SAFETY				= 0,
	P_LD_RBT_X_LD1TRAY_FIRST		,
	P_LD_RBT_X_LD2TRAY_FIRST		, 
	P_LD_RBT_X_BUFFER_FIRST			, 
	P_LD_RBT_X_RETESTTRAY_FIRST		, 
};

enum M_LD_RBT_Y_POS
{
	P_LD_RBT_Y_SAFETY				= 0,
	P_LD_RBT_Y_LD1TRAY_FIRST		,
	P_LD_RBT_Y_LD2TRAY_FIRST		, 
	P_LD_RBT_Y_BUFFER_FIRST			, 
	P_LD_RBT_Y_RETESTTRAY_FIRST		, 
};

enum M_LD_RBT_Z_POS
{
	P_LD_RBT_Z_SAFETY				= 0,
	P_LD_RBT_Z_LD1TRAY_PICK			,
	P_LD_RBT_Z_LD2TRAY_PICK			,
	P_LD_RBT_Z_BUFFER_PLACE			,
	P_LD_RBT_Z_RETESTTRAY_PICK		,

};

enum M_LD_RBT_P_POS
{
	P_LD_RBT_P_SAFETY				= 0,
	P_LD_RBT_P_TRAY					,
	P_LD_RBT_P_BUFFER				,
};

enum M_UNLD_RBT_X_POS
{
	P_UNLD_RBT_X_SAFETY				= 0,
	P_UNLD_RBT_X_ULD1TRAY_FIRST		,
	P_UNLD_RBT_X_ULD2TRAY_FIRST		,
	P_UNLD_RBT_X_BUFFER_FIRST		,
	P_UNLD_RBT_X_REJECT_FIRST		,
	P_UNLD_RBT_X_RETEST_FIRST		,
	P_UNLD_RBT_X_BARCODE_FIRST		,
};

enum M_UNLD_RBT_Y_POS
{
	P_UNLD_RBT_Y_SAFETY				= 0,
	P_UNLD_RBT_Y_ULD1TRAY_FIRST		,
	P_UNLD_RBT_Y_ULD2TRAY_FIRST		,
	P_UNLD_RBT_Y_BUFFER_FIRST		,
	P_UNLD_RBT_Y_REJECT_FIRST		,
	P_UNLD_RBT_Y_RETEST_FIRST		,
	P_UNLD_RBT_Y_BARCODE_FIRST		,
};
enum M_UNLD_RBT_Z_POS
{
	P_UNLD_RBT_Z_SAFETY				= 0,
	P_UNLD_RBT_Z_ULD1TRAY_PLACE		,
	P_UNLD_RBT_Z_ULD2TRAY_PLACE		,
	P_UNLD_RBT_Z_BUFFER_PICK		,
	P_UNLD_RBT_Z_RETESTTRAY_PLACE	,
	P_UNLD_RBT_Z_REJECTTRAY_PLACE	,
	P_UNLD_RBT_Z_BARCODE_READ		,
};

enum M_UNLD_RBT_P_POS
{
	P_UNLD_RBT_P_SAFETY				= 0,
	P_UNLD_RBT_P_TRAY				,
	P_UNLD_RBT_P_BUFFER				,
};

enum M_TSITE_RBT_X_POS
{
	P_TSITE_RBT_X_SAFETY						= 0,
	P_TSITE_RBT_X_LEFT_LOW_FIRST				,
	P_TSITE_RBT_X_LEFT_HIGH_FIRST				,
	P_TSITE_RBT_X_RIGHT_LOW_FIRST				,
	P_TSITE_RBT_X_RIGHT_HIGH_FIRST				,
	P_TSITE_RBT_X_LOADBUFF_FIRST				, 
	P_TSITE_RBT_X_UNLOADBUFF_FIRST				, 

	P_TSITE_RBT_X_COK_LEFT_LOW_FIRST			, 
	P_TSITE_RBT_X_COK_LEFT_HIGH_FIRST			, 
	P_TSITE_RBT_X_COK_RIGHT_LOW_FIRST			, 
	P_TSITE_RBT_X_COK_RIGHT_HIGH_FIRST			, 
	P_TSITE_RBT_X_COK_EXCHANGE_BUFFER_FIRST		, 
};

enum M_TSITE_RBT_Y_POS
{
	P_TSITE_RBT_Y_SAFETY						= 0,
	P_TSITE_RBT_Y_LEFT_LOW_FIRST				,
	P_TSITE_RBT_Y_LEFT_HIGH_FIRST				,
	P_TSITE_RBT_Y_RIGHT_LOW_FIRST				,
	P_TSITE_RBT_Y_RIGHT_HIGH_FIRST				, 
	P_TSITE_RBT_Y_LOADBUFF_FIRST				, 
	P_TSITE_RBT_Y_UNLOADBUFF_FIRST				, 	

	P_TSITE_RBT_Y_COK_LEFT_LOW_FIRST			, 
	P_TSITE_RBT_Y_COK_LEFT_HIGH_FIRST			, 
	P_TSITE_RBT_Y_COK_RIGHT_LOW_FIRST			, 
	P_TSITE_RBT_Y_COK_RIGHT_HIGH_FIRST			, 
	P_TSITE_RBT_Y_COK_EXCHANGE_BUFFER_FIRST		, 
};

enum M_TSITE_RBT_Z_POS
{
	P_TSITE_RBT_Z_SAFETY						= 0,
	P_TSITE_RBT_Z_BUFFER_PICK					,
	P_TSITE_RBT_Z_BUFFER_PLACE					,
	P_TSITE_RBT_Z_TESTLOW_PICK					,
	P_TSITE_RBT_Z_TESTLOW_PLACE					,
	P_TSITE_RBT_Z_TESTHIGH_PICK					,
	P_TSITE_RBT_Z_TESTHIGH_PLACE				,

	P_TSITE_RBT_Z_COK_LEFT_LOW_PICK				,
	P_TSITE_RBT_Z_COK_LEFT_HIGH_PICK			,
	P_TSITE_RBT_Z_COK_RIGHT_LOW_PICK			,
	P_TSITE_RBT_Z_COK_RIGHT_HIGH_PICK			,
	P_TSITE_RBT_Z_COK_LEFT_LOW_PLACE			,
	P_TSITE_RBT_Z_COK_LEFT_HIGH_PLACE			,
	P_TSITE_RBT_Z_COK_RIGHT_LOW_PLACE			,
	P_TSITE_RBT_Z_COK_RIGHT_HIGH_PLACE			,

	P_TSITE_RBT_Z_COK_EXCHANGE_BUFFER_PICK		, 
	P_TSITE_RBT_Z_COK_EXCHANGE_BUFFER_PLACE		, 
 
};

enum M_LF_MP_Z_POS
{
	P_LF_MP_Z_SAFETY					= 0, 
	P_LF_MP_Z_FLOOR_1						, //pogo tyoe
	P_LF_MP_Z_FLOOR_2						, //msata
	P_LF_MP_Z_FLOOR_3						, //2.5" front
	P_LF_MP_Z_FLOOR_4						, //2.5" reverse
	P_LF_MP_Z_FLOOR_5						,
};

enum M_RI_MP_Z_POS
{
	P_RI_MP_Z_SAFETY					= 0, 
	P_RI_MP_Z_FLOOR_1						,//pogo tyoe
	P_RI_MP_Z_FLOOR_2						,//msata
	P_RI_MP_Z_FLOOR_3						,//2.5" front
	P_RI_MP_Z_FLOOR_4						,//2.5" reverse
	P_RI_MP_Z_FLOOR_5						,
};

enum M_MVTRAYOUT_CV_X_POS
{
	P_MVOUT_CVY_SAFETY					= 0,
	P_MVOUT_CVY_IN					,
	P_MVOUT_CVY_OUT					,
};

enum M_GOOD_TRAY_ELV_POS
{
	P_GOOD_TRAY_ELV_SAFETY			= 0, //���� ��ġ (down limit ��ġ�ϰ���) 
	P_GOOD_TRAY_ELV_UP_LIMIT			,  //high rail ���� ���� ���� 
	P_GOOD_TRAY_ELV_DOWN_LIMIT		,  //���� �ظ� 	
	P_GOOD_TRAY_ELV_SDSENSOR		, //sd sensor ���� ��ġ 	

	//SD ���� ���������� ���� �̵��ϴ� ��ġ 
	//P_GOOD_TRAY_ELV_RAIL_HIGH_SUPPLY_OFFSET	, // SD pos + high Up Ʈ���̸� �����ϴ� 
	P_GOOD_TRAY_ELV_RAIL_HIGH_RECEIVE_OFFSET	, // SD pos + high Up Ʈ���̸� ����Ʈ���� �޴� ��ġ( �ʱ�ȭ�� Ʈ���̸� ����Ŀ�� ������ ���) 
	P_GOOD_TRAY_ELV_REFERENCE_POSITION_OFFSET	, //SD pos - down	��ġ�� �� ������ �۾��ϴ� �������̴�(SD ������ Down�Ͽ� SD�� ���� �ȵǴ� ��ġ�� �۾� �������̴�)
};

enum M_REJ_RBT_Z_EMPTY_POS
{
	P_REJ_RBT_Z_EMPTY_DN			= 0,
	P_REJ_RBT_Z_EMPTY_UP			, 
};

enum M_REJ_RBT_Z_FULL_POS
{
	P_REJ_RBT_Z_FULL_DN			= 0,
	P_REJ_RBT_Z_FULL_UP			, 
};
//kwlee 2016.0614  ���� ����
*/
///////////////////////////////////////////////
enum RFID_POS
{
	RF_CARRIER_ID					= 0,
	RF_EQP_ID						,
	RF_QTY							,
	RF_QTY_T						,
	RF_LOT_ID_J						, 
	RF_STEP							,
	RF_LOT_TYPE						,
	RF_PART_ID						,
	RF_PKG_CODE						,
	RF_NONE_1						,
	RF_LOT_ID_M						,
	RF_CNT							,
	RF_FLAG							,
	RF_REJECT						,
	RF_BIN							,
	RF_NONE_2						,
	RF_TRAY_QTY						,
};

enum COMM_PORT
{
	BCR_1D_PORT						= 1,
	BCR_2D_PORT					,
};

#define WAIT_CONNECT					5000
#define WAIT_RCV						30000
#define WAIT_CLOSE						300

enum NETWORK_NUM
{
	EC_FIRST_NETWORK				= 0,
	XGEM_NETWORK					,
	INTERFACE_NETWORK_1				,
	INTERFACE_NETWORK_2				,
	INTERFACE_NETWORK_3				,
	INTERFACE_NETWORK_4				,
	INTERFACE_NETWORK_5				,
	INTERFACE_NETWORK_6				,
	INTERFACE_NETWORK_7				,
	INTERFACE_NETWORK_8				,
	EC_SECOND_NETWORK				,
	FTP_NETWORK						,
};

//kwlee 2016.1124
enum SERIAL_NUM
{
	
	TSITE_SERIAL_1_1			 = 0,
	TSITE_SERIAL_1_2				,
	TSITE_SERIAL_2_1				,
	TSITE_SERIAL_2_2				,
	TSITE_SERIAL_3_1				,
	TSITE_SERIAL_3_2				,
	TSITE_SERIAL_4_1				,
	TSITE_SERIAL_4_2				,
	TSITE_SERIAL_5_1				,
	TSITE_SERIAL_5_2				,
	TSITE_SERIAL_6_1				,
	TSITE_SERIAL_6_2				,
	TSITE_SERIAL_7_1				,
	TSITE_SERIAL_7_2				,
	TSITE_SERIAL_8_1				,
	TSITE_SERIAL_8_2				,
};
/////////


enum CUSTOM_IO_FUNCTION_SITE
{
	ALL_TSITE			= -1,
	LEFT_TSITE			,
	RIGHT_TSITE			,
	TSITE_TOP			,
	TSITE_BTM			,
};

enum CUSTOM_IO_FUNCTION_PICKER
{
	ALL_PICKER			= -1,
	PICKER_1			,
	PICKER_2			,
	PICKER_3			,
	PICKER_4			,
};

enum CUSTOM_IO_FUNCTION_SOCKET
{
	ALL_SOCKET			= -1,
	SOCKET_1			,
	SOCKET_2			,
	SOCKET_3			,
	SOCKET_4			,
	SOCKET_5			,
	SOCKET_6			,
	SOCKET_7			,
	SOCKET_8			,
	SOCKET_9			,
	SOCKET_10			,
	SOCKET_11			,
	SOCKET_12			,
	SOCKET_13			,
	SOCKET_14			,
	SOCKET_15			,
	SOCKET_16			,
	SOCKET_17			,
	SOCKET_18			,
	SOCKET_19			,
	SOCKET_20			,
	SOCKET_21			,
	SOCKET_22			,
	SOCKET_23			,
	SOCKET_24			,

};

//2015.1005
enum ENUM_FILLRATIO
{

	REF_TIME,
	RUN_TIME,
	TEST_TIME,
	RATIO_RUN,
	RATIO_TEST,
	RUN_DOWN,
	SOCKETOFF,
	INSTANBY,
	OUTREADY,
	STOP,
	USERSTOP,
	OUTPUT,
	YIELD,		
};
///////////////////
// jtkim 20160804
enum INTER_RPY_RESULT
{
	INTER_READY			= 0,
	INTER_SOCKET		,
	INTER_MODULE		,
	INTER_PGM			,
	INTER_START			,
	INTER_TOTAL			,
	INTER_OPTION		,
	INTER_ABORT			,
	INTER_DOOR			,
};

#define SLOT_DISABLE				0
#define SLOT_ENABLE					1

///////////////////////////////////////////////////////////
//���� ƼĪ���� ���� ��
struct st_teach_param
{
	//Z�� focus
	//�� ����Ʈ���� z���� �� ���� �ϴϸ� left, right�� ���� ���� ����
	//�� �Ǹ� 2���� �ϸ� �ȴ��� �ȵǸ� ����Ʈ���� �ؾ� �ϴµ� �ð���
	//���� �ɸ��� ����.
	//�ϴ�, 2���� ��
	//double m_dFocusTeachZPos[2];//LEFT=0,RIGHT

	double m_dFocusTeachZPos[8];//LEFT=0,RIGHT

	// ��ũ�� �ִ� ��ġ ��. �̰� �������� ����Ѵ�.
	double m_dLeftVisionMarkXPos[4][4];//[4]Left����Ʈ [4]ƼĪ4����
	double m_dLeftVisionMarkYPos[4][4];
	double m_dRightVisionMarkXPos[4][4];
	double m_dRightVisionMarkYPos[4][4];

	// ��ũ�� ���� ��ġ ��. ���⼭���� ã�ư��� ��.
	double m_dLeftVisionReadXPos[4][4];
	double m_dLeftVisionReadYPos[4][4];
	double m_dRightVisionReadXPos[4][4];
	double m_dRightVisionReadYPos[4][4];

	// ���� ������ ���� ��ġ.
	double m_dLeftVisionTeachXPos[4][4];
	double m_dLeftVisionTeachYPos[4][4];
	double m_dRightVisionTeachXPos[4][4];
	double m_dRightVisionTeachYPos[4][4];

	// �� ��ġ�� OffSet
	double m_dLeftVisionOffSetXPos[4][4];
	double m_dLeftVisionOffSetYPos[4][4];
	double m_dRightVisionOffSetXPos[4][4];
	double m_dRightVisionOffSetYPos[4][4];

	// �κ��� �پ� �ִ� ī�޶� OffSet
	double m_dRobotCameraOffSetXPos;
	double m_dRobotCameraOffSetYPos;

	// ��ũ���� ���� ���ϱ����� �Ÿ�.
	double m_dMarkToSocketXPos;
	double m_dMarkToSocketYPos;

	//���� dll���� �о� �°�
	double m_dVisionXValue;
	double m_dVisionYValue;
	double m_dVisionZValue;
};
extern st_teach_param	st_teach;


struct st_copy_data
{
	int		mn_mdl;	
	char	str_partid[100];
};
extern struct st_copy_data	*st_copydata;
#endif