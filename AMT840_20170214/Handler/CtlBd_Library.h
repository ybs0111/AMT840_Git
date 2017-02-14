// CtlBd_Library.h: interface for the CCtlBd_Library class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CTLBD_LIBRARY_H__95978C35_3D8D_44B9_A9DF_E9F5E1C39135__INCLUDED_)
#define AFX_CTLBD_LIBRARY_H__95978C35_3D8D_44B9_A9DF_E9F5E1C39135__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CCtlBd_Library  
{
public:
	CCtlBd_Library();
	virtual ~CCtlBd_Library();

public:		
	CString str_Motor_Msg;//20150428 james 
	
	CString m_strMotorAlarmCode;

	int  m_nMotRetry_Cnt[100];
	int  m_nStep_Single[100];
	int  m_nStep_Linear[100];
	int  m_nLinearRetry_Cnt[100];

	DWORD m_dwUntil_Wait[100][3];

	char mc_normal_msg[1000]; //��񿡼� �߻��ϴ� �޼����� �����Ѵ� 

	int mn_run_status;	//����� ���� ���� ���� 
	
	char mc_alarmcode[10]; //alarm code ����
	int mn_alarm_type;	//�˶��� ������ ����[��)operator error ����, maint �� error ���� ����]
	int mn_alarm_count; //�˶� ī��Ʈ ���� ���� [0:ī��Ʈ���� ����, 1:ī��Ʈ ��]
	int mn_jampos; //jam �� �߻��� ��ġ�� ������ �����Ѵ� 

	int m_nMcEmg;
	int	mn_empty_sd_flag_1;
	int	mn_empty_sd_flag_2;
	int	mn_reject_sd_flag;
	int mn_unload_sd_flag;
	int mn_cart_sd_flag;

	double md_empty_sd_pos_1;
	double md_empty_sd_pos_2;
	double md_reject_sd_pos;
	double md_unload_sd_pos;
	double md_cart_sd_pos;

	double m_dMvOldPos[100];	// motor safety old pos;
	int	   m_nRetryCnt[100];

	DWORD  m_dwJigElvChk[3];
	bool   m_bJigElvChk;

	///////////////////////////////////////////////////////////////////////
	//james 2016.0803 
	//////////////////////////////////////////////////////////////////////
	int		m_nSD_Elv_MoveStep[100]; //MAX 100 �� ��ȣ �迭 
	bool		m_bSD_MoveFlag[100];
	DWORD	m_dwSdWaitTime[100][3];
	bool	m_bSd_Move_Flag[100];

	int		m_nSd_Motor_MoveStart_Flag[100];

	double  m_dTargetPos[100]; //MAX 100 �� ��ȣ �迭 
	double  m_dSD_Supply_Pos_Backup[100]; //MAX 100 �� ��ȣ �迭 
	double  m_dSD_Receive_Pos_Backup[100]; //MAX 100 �� ��ȣ �迭 

	bool			m_bAcMotorFlag[100];
	DWORD			m_dwAcMotorWaitTime[100][3];

	int		m_npTestSitePos_Info[2]; //[0]:�׽�Ʈ ����Ʈ ��ġ����, [1]:�׽�Ʈ ���� 8����, left, right ����



public:
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 20150428 james
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	int HomeCheck_Mot(int n_Axis, int n_HomeMode, int n_TimeOut);
	int Debug_File_Create(int n_mode,  char* s_msg); //2012.1029 jamestest  CString s_msg)CString s_msg);
	int FileSizeCheck(CString s_filename, long l_size, int n_check);
	void OnStringToChar(CString strData, char chData[]);
	void Motor_Error_Occurrence(int n_Mode, long l_Axis, long l_ErrorIDNum, CString strErrorMsg); //20150428 james �˶����� ���� ���� 
	/////////////////////////// 


	
	int SD_Sensor_Enable(int n_Mode, int n_AxisNum, int n_Enable);

	int Linear_Move(int n_RunMethod, int n_LinearMapIndex, long l_AxisCnt, long *lp_AxisNum, double *dp_TargetPosList, int n_SpeedRate) ;
	int Single_Move(int n_RunMethod, int n_MotorNum, double d_MovePos, int n_SpeedRate); //2016.0911 james =100);	// �Էµ� ��ġ�� ���� �̵�
	///////////////////////////////////////////////////

	int IO_Safety(int n_io, int n_onoff);
	BOOL CreateFolder(LPCTSTR sz_path);  //folder ���� �ʿ�� ��� 
	int FileSizeCheck(char * cp_filename, int n_size, int n_check);
	int LogFile_Write(char * cp_dir_path, char * cp_msg);

	
	int Initialize_motor_board(int n_bd_type, CString s_filename);	//motor ���� �ʱ�ȭ�� ��� 
	int Initialize_io_Board(int n_bd_type);	//i/o ���� �ʱ�ȭ�� ��� 

	int Motor_SafetyCheck(int n_mode, int n_axis, double d_targetpos);	//���� ���� ���� ���ý� ��� 
	int Alarm_Error_Occurrence(int nJamPos, int nRunStatus, CString strCode); //��� �˶� �߻���ų�� ����ϴ� ���� 

	int OnMotorGoCheck(int n_motor, double d_pos);

	///////////////////////////////////////////////////////////////////////
	//james 2016.0803 
	//////////////////////////////////////////////////////////////////////
	int Elevator_Job_Move_Pos(int nMode, int n_AxisNum,  int n_TargetPos);
	void Process_Tray_Conveyor_AcMotor(int n_Mode, int n_THD_Site, int n_AcMotor_IO_Num);

	void Process_TestChamber_AcMotor(int n_Mode, int n_THD_Site, int n_AcMotor_IO_Num); //close,open chamber



	//	int alarm_func(int n_mode, char * cp_jamcode, char * cp_msg);
	//int char_msg_func(int n_mode, char * cp_source_msg); //2011.0910 

};
extern CCtlBd_Library CTL_Lib;


#endif // !defined(AFX_CTLBD_LIBRARY_H__95978C35_3D8D_44B9_A9DF_E9F5E1C39135__INCLUDED_)
