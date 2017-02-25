// MyBasicData.cpp: implementation of the CMyBasicData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "handler.h"
#include "MyBasicData.h"
#include "Ctlbd_Variable.h"
#include "PublicFunction.h"
#include "FastechPublic_IO.h"
#include "Alg_Excel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CMyBasicData clsBasic;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMyBasicData::CMyBasicData()
{

}

CMyBasicData::~CMyBasicData()
{

}

void CMyBasicData::OnPassWord_Load(CString strLoadLevel)
{
	CString strPass;
	CString strLoadFile;
	TCHAR chPass[25];

	(st_other_info.strPassword).Empty();		// ��ȣ ���� ���� �ʱ�ȭ

	strLoadFile = st_path_info.strBasic + st_basic_info.strDeviceName;

	if(strLoadLevel == _T("SysLock"))			// SYSTEM LOCK ��ȣ
	{
		:: GetPrivateProfileString(_T("Password"), _T("SysLock"), _T("M"), chPass, 20, strLoadFile);

	}
	else if (strLoadLevel == _T("Level1"))	// ����Ʈ ��ȣ
	{
		:: GetPrivateProfileString(_T("Password"), _T("Level_1"), _T("M"), chPass, 20, strLoadFile);
	}
	else if (strLoadLevel == _T("Level2"))	// ƼĪ ��ȣ
	{
		:: GetPrivateProfileString(_T("Password"), _T("Level_2"), _T("T"), chPass, 20, strLoadFile);
	}
	else  
	{
		return;
	}

	strPass.Format(_T("%s"), chPass);
	strPass.MakeUpper();     // ���ڿ� �빮�ڷ� ����
	strPass.TrimLeft(' ');   // ���� ���ڿ� ���� ����              
	strPass.TrimRight(' ');  // ���� ���ڿ� ���� ����

	st_other_info.strPassword = strPass;  // ��ȣ ���� ������ ����
}

void CMyBasicData::OnPassWord_Save(CString strSaveLevel, CString strPassData)
{
	CString strSaveFile;
	
	strSaveFile = st_path_info.strBasic + st_basic_info.strDeviceName;

	if (strSaveLevel=="SysLock")         // SYSTEM LOCK ��ȣ
	{
		:: WritePrivateProfileString(_T("Password"), _T("SysLock"), LPCTSTR(strPassData), strSaveFile);
	}
	else if (strSaveLevel=="Level1")  // ����Ʈ ��ȣ
	{
		:: WritePrivateProfileString(_T("Password"), _T("Level_1"), LPCTSTR(strPassData), strSaveFile);
	}
	else if (strSaveLevel=="Level2")  // ƼĪ ��ȣ
	{
		:: WritePrivateProfileString(_T("Password"), _T("Level_2"), LPCTSTR(strPassData), strSaveFile);
	}
}

CString CMyBasicData::OnStep_File_Index_Load()
{
	CString strLoadFile;
	char chLoad[20];

	strLoadFile = st_path_info.strBasic + st_basic_info.strDeviceName;

	:: GetPrivateProfileString(_T("Thread_Step_file"), _T("File_Index"), _T("00"), (LPWSTR)chLoad, 20, strLoadFile);
//	sprintf(chLoad,"%S", chLoad);
	strLoadFile = chLoad;
	strLoadFile.TrimLeft(' ');               
	strLoadFile.TrimRight(' ');

	return strLoadFile;  // ���� �ε��� ����
}

void CMyBasicData::OnStep_File_Index_Save(CString strIndex)
{
	CString strSaveFile;

	strSaveFile = st_path_info.strBasic + st_basic_info.strDeviceName;

	:: WritePrivateProfileString(_T("Thread_Step_file"), _T("File_Index"), LPCTSTR(strIndex), strSaveFile);
}

CString CMyBasicData::OnStep_File_Name_Load()
{
	CString strFileName;  // ���ϸ� ���� ����
	CString strLoadFile;
	char chLoad[20];

	strLoadFile = st_path_info.strBasic + st_basic_info.strDeviceName;

	:: GetPrivateProfileString(_T("Thread_Step_file"), _T("File_Name"), _T("DEFAULT"),(LPWSTR)chLoad, 20, strLoadFile);
//	sprintf(chLoad,"%S", chLoad);
	strFileName = chLoad;
	strFileName.TrimLeft(' ');               
	strFileName.TrimRight(' ');

	return strFileName;  // ���ϸ� ����
}

void CMyBasicData::OnStep_File_Name_Save(CString strFile)
{
	:: WritePrivateProfileString(_T("Thread_Step_file"), _T("File_Name"), LPCTSTR(strFile), st_path_info.strBasic);
}

void CMyBasicData::OnMotorSpeed_Set_Data_Load()
{
	int  nChk=0, i=0;
	double dChk;
	CString strTemp;       // �ӽ� ���� ����
	CString strMotorName;
	CString strLoadFile;	
	char chData[20];
	
	for(i = 0; i < MAXMOTOR; i++)
	{
		strMotorName.Format(_T("%02d_AXIS_SPEED"), i+1);
		:: GetPrivateProfileString(strMotorName, _T("ACC"), _T("100"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
		strTemp.Format(_T("%s"), chData);
		dChk = _wtof(strTemp);
		st_motor_info[i].d_accel[0] = dChk;
		st_motor_info[i].d_spd_vel[1] = dChk;
		
		:: GetPrivateProfileString(strMotorName, _T("DEC"), _T("100"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
		strTemp.Format(_T("%s"), chData);
		dChk = _wtof(strTemp);
		st_motor_info[i].d_decel[0] = dChk;
		st_motor_info[i].d_spd_vel[2] = dChk;
		
		:: GetPrivateProfileString(strMotorName, _T("MAX"), _T("100000"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
		strTemp.Format(_T("%s"), chData);
		dChk = _wtof(strTemp);
		st_motor_info[i].d_spd_max[0] = dChk;
		
		:: GetPrivateProfileString(strMotorName, _T("VEL"), _T("1000"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
		strTemp.Format(_T("%s"), chData);
		dChk = _wtof(strTemp);
		st_motor_info[i].d_spd_vel[0] = dChk;
		
		:: GetPrivateProfileString(strMotorName, _T("HOME"), _T("500"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
		strTemp.Format(_T("%s"), chData);
		dChk = _wtof(strTemp);
		st_motor_info[i].d_spd_home[0] = dChk;
		
		:: GetPrivateProfileString(strMotorName, _T("JOG"), _T("300"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
		strTemp.Format(_T("%s"), chData);
		dChk = _wtof(strTemp);
		st_motor_info[i].d_spd_jog[0] = dChk;
		
		:: GetPrivateProfileString(strMotorName, _T("VEL_PER"), _T("50"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
		strTemp.Format(_T("%s"), chData);
		nChk = _wtoi(strTemp);
		st_motor_info[i].n_per_vel = nChk;
		
		:: GetPrivateProfileString(strMotorName, _T("HOME_PER"), _T("5"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
		strTemp.Format(_T("%s"), chData);
		nChk = _wtoi(strTemp);
		st_motor_info[i].n_per_home = nChk;
		
		:: GetPrivateProfileString(strMotorName, _T("JOG_PER"), _T("5"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
		strTemp.Format(_T("%s"), chData);
		nChk = _wtoi(strTemp);
		st_motor_info[i].n_per_jog = nChk;
		
		:: GetPrivateProfileString(strMotorName, _T("ALLOW"), _T("100"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
		strTemp.Format(_T("%s"), chData);
		dChk = _wtof(strTemp);
		st_motor_info[i].d_allow = dChk;

		:: GetPrivateProfileString(strMotorName, _T("LIMIT_M"), _T("0"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
		strTemp.Format(_T("%s"), chData);
		dChk = _wtof(strTemp);
		st_motor_info[i].d_limit_position[0] = dChk;

		:: GetPrivateProfileString(strMotorName, _T("LIMIT_P"), _T("3000"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
		strTemp.Format(_T("%s"), chData);
		dChk = _wtof(strTemp);
		st_motor_info[i].d_limit_position[1] = dChk;
	}
	
	//Speed Rate
	:: GetPrivateProfileString(_T("SPEED_RATE"), _T("RUN"), _T("100"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	st_handler_info.nRunSpeed = nChk;
	
	:: GetPrivateProfileString(_T("SPEED_RATE"), _T("MANUAL"), _T("80"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	st_handler_info.nManualSpeed = nChk;
}

void CMyBasicData::OnMotorSpeed_Set_Data_Save()
{
	CString strTemp;       // �ӽ� ���� ����
	CString strMotorName;
	CString strSaveFile;
//	char chBuf[20];

	for(int i = 0; i < MAXMOTOR; i++)
	{
		strMotorName.Format(_T("%02d_AXIS_SPEED"), i+1);
//		LPCTSTR(_itoa((int)(st_motor_info[i].d_accel[0]), chBuf, 10));
		strTemp.Format(_T("%.3f"), st_motor_info[i].d_accel[0]);
		:: WritePrivateProfileString(strMotorName, _T("ACC"), LPCTSTR(strTemp), st_path_info.strFileBasic);
		
//		LPCTSTR(_itoa((int)(st_motor_info[i].d_decel[0]), chBuf, 10));
//		strTemp = chBuf;
		strTemp.Format(_T("%.3f"), st_motor_info[i].d_decel[0]);
		:: WritePrivateProfileString(strMotorName, _T("DEC"), LPCTSTR(strTemp), st_path_info.strFileBasic);
		
//		LPCTSTR(_itoa((int)(st_motor_info[i].d_spd_max[0]), chBuf, 10));
//		strTemp = chBuf;
		strTemp.Format(_T("%.3f"), st_motor_info[i].d_spd_max[0]);
		:: WritePrivateProfileString(strMotorName, _T("MAX"), LPCTSTR(strTemp), st_path_info.strFileBasic);
		
//		LPCTSTR(_itoa((int)(st_motor_info[i].d_spd_vel[0]), chBuf, 10));
//		strTemp = chBuf;
		strTemp.Format(_T("%.3f"), st_motor_info[i].d_spd_vel[0]);
		:: WritePrivateProfileString(strMotorName, _T("VEL"), LPCTSTR(strTemp), st_path_info.strFileBasic);
		
//		LPCTSTR(_itoa((int)(st_motor_info[i].d_spd_home[0]), chBuf, 10));
//		strTemp = chBuf;
		strTemp.Format(_T("%.3f"), st_motor_info[i].d_spd_home[0]);
		:: WritePrivateProfileString(strMotorName, _T("HOME"), LPCTSTR(strTemp), st_path_info.strFileBasic);
		
//		LPCTSTR(_itoa((int)(st_motor_info[i].d_spd_jog[0]), chBuf, 10));
//		strTemp = chBuf;
		strTemp.Format(_T("%.3f"), st_motor_info[i].d_spd_jog[0]);
		:: WritePrivateProfileString(strMotorName, _T("JOG"), LPCTSTR(strTemp), st_path_info.strFileBasic);
		
//		LPCTSTR(_itoa((st_motor_info[i].n_per_vel), chBuf, 10));
//		strTemp = chBuf;
		strTemp.Format(_T("%d"), st_motor_info[i].n_per_vel);
		:: WritePrivateProfileString(strMotorName, _T("VEL_PER"), LPCTSTR(strTemp), st_path_info.strFileBasic);
		
//		LPCTSTR(_itoa((st_motor_info[i].n_per_home), chBuf, 10));
//		strTemp = chBuf;
		strTemp.Format(_T("%d"), st_motor_info[i].n_per_home);
		:: WritePrivateProfileString(strMotorName, _T("HOME_PER"), LPCTSTR(strTemp), st_path_info.strFileBasic);
		
//		LPCTSTR(_itoa((st_motor_info[i].n_per_jog), chBuf, 10));
//		strTemp = chBuf;
		strTemp.Format(_T("%d"), st_motor_info[i].n_per_jog);
		:: WritePrivateProfileString(strMotorName, _T("JOG_PER"), LPCTSTR(strTemp), st_path_info.strFileBasic);
		

		strTemp.Format(_T("%.2f"), st_motor_info[i].d_allow);
		:: WritePrivateProfileString(strMotorName, _T("ALLOW"), LPCTSTR(strTemp), st_path_info.strFileBasic);
		
//		LPCTSTR(_itoa((int)(st_motor_info[i].d_limit_position[0]), chBuf, 10));
//		strTemp = chBuf;
		strTemp.Format(_T("%.3f"), st_motor_info[i].d_limit_position[0]);
		:: WritePrivateProfileString(strMotorName, _T("LIMIT_M"), LPCTSTR(strTemp), st_path_info.strFileBasic);

//		LPCTSTR(_itoa((int)(st_motor_info[i].d_limit_position[1]), chBuf, 10));
//		strTemp = chBuf;
		strTemp.Format(_T("%.3f"), st_motor_info[i].d_limit_position[1]);
		:: WritePrivateProfileString(strMotorName, _T("LIMIT_P"), LPCTSTR(strTemp), st_path_info.strFileBasic);
	}
	
	// Speed Rate
//	LPCTSTR(_itoa((int)(st_handler_info.nRunSpeed), chBuf, 10));
//	strTemp = chBuf;
	strTemp.Format(_T("%d"), st_handler_info.nRunSpeed);
	:: WritePrivateProfileString(_T("SPEED_RATE"), _T("RUN"), LPCTSTR(strTemp), st_path_info.strFileBasic);
	
//	LPCTSTR(_itoa((int)(st_handler_info.nManualSpeed), chBuf, 10));
//	strTemp = chBuf;
	strTemp.Format(_T("%d"), st_handler_info.nManualSpeed);
	:: WritePrivateProfileString(_T("SPEED_RATE"), _T("MANUAL"), LPCTSTR(strTemp), st_path_info.strFileBasic);
}

void CMyBasicData::OnWaitTime_Data_Load()
{
	CString strTemp, strMsg, strHead;       // �ӽ� ���� ����
	CString strLoadFile, strOnName, strOffName, strLimitName;
	int nChk, i;
	char chData[20];

	for(i=0; i<MAX_WAIT_TIME; i++)
	{
		strHead.Format(_T("TIME_[%02d]"), i);
		
		strOnName.Format(_T("%s_ON"), strHead);
		:: GetPrivateProfileString(_T("ON_WAIT_TIME"), strOnName, _T("100"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
		strTemp.Format(_T("%s"), chData);
		nChk = _wtoi(strTemp);
		st_wait_info.nOnWaitTime[i] = nChk;
		
		strOffName.Format(_T("%s_OFF"), strHead);
		:: GetPrivateProfileString(_T("OFF_WAIT_TIME"), strOffName, _T("100"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
		strTemp.Format(_T("%s"), chData);
		nChk = _wtoi(strTemp);
		st_wait_info.nOffWaitTime[i] = nChk;
		
		strLimitName.Format(_T("%s_LIMIT"), strHead);
		:: GetPrivateProfileString(_T("LIMIT_TIME"), strLimitName, _T("100"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
		strTemp.Format(_T("%s"), chData);
		nChk = _wtoi(strTemp);
		st_wait_info.nLimitWaitTime[i] = nChk;
	}
}

void CMyBasicData::OnWaitTime_Data_Save()
{
	CString strTemp, strPart;  // �ε� ���� �ӽ� ���� ����
	CString strSaveFile;
	CString strOnName, strOffName, strLimitName;
	CString strData;
	int i;
	
	for(i=0; i<MAX_WAIT_TIME; i++)
	{
		strTemp.Format(_T("TIME_[%02d]"), i);
		
		strOnName.Format(_T("%s_ON"), strTemp);
		strData.Format(_T("%d"), st_wait_info.nOnWaitTime[i]);
		:: WritePrivateProfileString(_T("ON_WAIT_TIME"), strOnName, LPCTSTR(strData), st_path_info.strFileBasic);
		
		strOffName.Format(_T("%s_OFF"), strTemp);
		strData.Format(_T("%d"), st_wait_info.nOffWaitTime[i]);
		:: WritePrivateProfileString(_T("OFF_WAIT_TIME"), strOffName, LPCTSTR(strData), st_path_info.strFileBasic);
		
		strLimitName.Format(_T("%s_LIMIT"), strTemp);
		strData.Format(_T("%d"), st_wait_info.nLimitWaitTime[i]);
		:: WritePrivateProfileString(_T("LIMIT_TIME"), strLimitName, LPCTSTR(strData), st_path_info.strFileBasic);	
	}
}

void CMyBasicData::OnMaintenance_Data_Load()
{
	CString strTemp, strPart;  // �ε� ���� �ӽ� ���� ����
	int nChk;
	CString strLoadFile;
	
	char chData[20];
	
	// **************************************************************************
	// Ÿ�� ���� RED ���� �ε��Ͽ� ���� ������ �����Ѵ�                          
	// ->  STOP			= 0,
	// 		   RUN			= 1,
	// 		   ALARM		= 2,
	// 		   LOTEND		= 3,
	// 		   INIT			= 4,
	// 		   WARRING		= 5,
	// 		   LOCK			= 6,
	// 		   SELFCHECK	= 7														 
	// -> �ε� �� [0:OFF    1:ON    2:FLICKER]                                   
	// **************************************************************************
//	str_load_file = st_path.str_basic + st_basic.str_device_name;  // ƼĪ ������ ���� ���� ����
	strLoadFile = st_path_info.strBasic + _T("Maintenance.TXT");  // ƼĪ ������ ���� ���� ����

	:: GetPrivateProfileString(_T("TowerLampData_Red"), _T("Stop_State"), _T("0"), (LPWSTR)chData, 10, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk<0 || nChk>2)
	{
		st_lamp_info.nLampR[0] = 0;
	}
	else
	{
		st_lamp_info.nLampR[0] = nChk;
	}
	
	:: GetPrivateProfileString(_T("TowerLampData_Red"), _T("Run_State"), _T("0"), (LPWSTR)chData, 10, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk<0 || nChk>2)
	{
		st_lamp_info.nLampR[1] = 0;
	}
	else
	{
		st_lamp_info.nLampR[1] = nChk;
	}
	
	:: GetPrivateProfileString(_T("TowerLampData_Red"), _T("Alarm_State"), _T("0"), (LPWSTR)chData, 10, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk<0 || nChk>2)
	{
		st_lamp_info.nLampR[2] = 2;
	}
	else
	{
		st_lamp_info.nLampR[2] = nChk;
	}

	:: GetPrivateProfileString(_T("TowerLampData_Red"), _T("LotEnd_State"), _T("0"), (LPWSTR)chData, 10, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk<0 || nChk>2)
	{
		st_lamp_info.nLampR[3] = 2;
	}
	else
	{
		st_lamp_info.nLampR[3] = nChk;
	}

	:: GetPrivateProfileString(_T("TowerLampData_Red"), _T("Initial_State"), _T("0"), (LPWSTR)chData, 10, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk<0 || nChk>2)
	{
		st_lamp_info.nLampR[4] = 2;
	}
	else
	{
		st_lamp_info.nLampR[4] = nChk;
	}

	:: GetPrivateProfileString(_T("TowerLampData_Red"), _T("Warring_State"), _T("0"), (LPWSTR)chData, 10, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk<0 || nChk>2)
	{
		st_lamp_info.nLampR[5] = 2;
	}
	else
	{
		st_lamp_info.nLampR[5] = nChk;
	}

	:: GetPrivateProfileString(_T("TowerLampData_Red"), _T("Lock_State"), _T("0"), (LPWSTR)chData, 10, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk<0 || nChk>2)
	{
		st_lamp_info.nLampR[6] = 2;
	}
	else
	{
		st_lamp_info.nLampR[6] = nChk;
	}

	:: GetPrivateProfileString(_T("TowerLampData_Red"), _T("SelfCheck_State"), _T("0"), (LPWSTR)chData, 10, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk<0 || nChk>2)
	{
		st_lamp_info.nLampR[7] = 2;
	}
	else
	{
		st_lamp_info.nLampR[7] = nChk;
	}

	:: GetPrivateProfileString(_T("TowerLampData_Red"), _T("Idle_State"), _T("0"), (LPWSTR)chData, 10, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk<0 || nChk>2)
	{
		st_lamp_info.nLampR[8] = 2;
	}
	else
	{
		st_lamp_info.nLampR[8] = nChk;
	}
	// **************************************************************************


	// **************************************************************************
	// Ÿ�� ���� YELLOW ���� �ε��Ͽ� ���� ������ �����Ѵ�                       
	// ->  STOP			= 0,
// 		   RUN			= 1,
// 		   ALARM		= 2,
// 		   LOTEND		= 3,
// 		   INIT			= 4,
// 		   WARRING		= 5,
// 		   LOCK			= 6,
// 		   SELFCHECK	= 7														 
	// -> �ε� �� [0:OFF    1:ON    2:FLICKER]                                   
	// **************************************************************************
	:: GetPrivateProfileString(_T("TowerLampData_Yellow"), _T("Stop_State"), _T("0"), (LPWSTR)chData, 10, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk<0 || nChk>2)
	{
		st_lamp_info.nLampY[0] = 0;
	}
	else
	{
		st_lamp_info.nLampY[0] = nChk;
	}

	:: GetPrivateProfileString(_T("TowerLampData_Yellow"), _T("Run_State"), _T("0"), (LPWSTR)chData, 10, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk<0 || nChk>2)
	{
		st_lamp_info.nLampY[1] = 1;
	}
	else
	{
		st_lamp_info.nLampY[1] = nChk;
	}

	:: GetPrivateProfileString(_T("TowerLampData_Yellow"), _T("Alarm_State"), _T("0"), (LPWSTR)chData, 10, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk<0 || nChk>2)
	{
		st_lamp_info.nLampY[2] = 0;
	}
	else
	{
		st_lamp_info.nLampY[2] = nChk;
	}

	:: GetPrivateProfileString(_T("TowerLampData_Yellow"), _T("LotEnd_State"), _T("0"), (LPWSTR)chData, 10, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk<0 || nChk>2)
	{
		st_lamp_info.nLampY[3] = 2;
	}
	else
	{
		st_lamp_info.nLampY[3] = nChk;
	}

	:: GetPrivateProfileString(_T("TowerLampData_Yellow"), _T("Initial_State"), _T("0"), (LPWSTR)chData, 10, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk<0 || nChk>2)
	{
		st_lamp_info.nLampY[4] = 2;
	}
	else
	{
		st_lamp_info.nLampY[4] = nChk;
	}

	:: GetPrivateProfileString(_T("TowerLampData_Yellow"), _T("Warring_State"), _T("0"), (LPWSTR)chData, 10, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk<0 || nChk>2)
	{
		st_lamp_info.nLampY[5] = 2;
	}
	else
	{
		st_lamp_info.nLampY[5] = nChk;
	}

	:: GetPrivateProfileString(_T("TowerLampData_Yellow"), _T("Lock_State"), _T("0"), (LPWSTR)chData, 10, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk<0 || nChk>2)
	{
		st_lamp_info.nLampY[6] = 2;
	}
	else
	{
		st_lamp_info.nLampY[6] = nChk;
	}

	:: GetPrivateProfileString(_T("TowerLampData_Yellow"), _T("SelfCheck_State"), _T("0"), (LPWSTR)chData, 10, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk<0 || nChk>2)
	{
		st_lamp_info.nLampY[7] = 2;
	}
	else
	{
		st_lamp_info.nLampY[7] = nChk;
	}

	:: GetPrivateProfileString(_T("TowerLampData_Yellow"), _T("Idle_State"), _T("0"), (LPWSTR)chData, 10, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk<0 || nChk>2)
	{
		st_lamp_info.nLampY[8] = 2;
	}
	else
	{
		st_lamp_info.nLampY[8] = nChk;
	}
	// **************************************************************************

	// **************************************************************************
	// Ÿ�� ���� GREEN ���� �ε��Ͽ� ���� ������ �����Ѵ�                        
	// ->  STOP			= 0,
// 		   RUN			= 1,
// 		   ALARM		= 2,
// 		   LOTEND		= 3,
// 		   INIT			= 4,
// 		   WARRING		= 5,
// 		   LOCK			= 6,
// 		   SELFCHECK	= 7														 
	// -> �ε� �� [0:OFF    1:ON    2:FLICKER]                                   
	// **************************************************************************
	:: GetPrivateProfileString(_T("TowerLampData_Green"), _T("Stop_State"), _T("0"), (LPWSTR)chData, 10, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk<0 || nChk>2)
	{
		st_lamp_info.nLampG[0] = 1;
	}
	else
	{
		st_lamp_info.nLampG[0] = nChk;
	}

	:: GetPrivateProfileString(_T("TowerLampData_Green"), _T("Run_State"), _T("0"), (LPWSTR)chData, 10, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk<0 || nChk>2)
	{
		st_lamp_info.nLampG[1] = 0;
	}
	else
	{
		st_lamp_info.nLampG[1] = nChk;
	}

	:: GetPrivateProfileString(_T("TowerLampData_Green"), _T("Alarm_State"), _T("0"), (LPWSTR)chData, 10, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk<0 || nChk>2)
	{
		st_lamp_info.nLampG[2] = 0;
	}
	else
	{
		st_lamp_info.nLampG[2] = nChk;
	}

	:: GetPrivateProfileString(_T("TowerLampData_Green"), _T("LotEnd_State"), _T("0"), (LPWSTR)chData, 10, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk<0 || nChk>2)
	{
		st_lamp_info.nLampG[3] = 2;
	}
	else
	{
		st_lamp_info.nLampG[3] = nChk;
	}

	:: GetPrivateProfileString(_T("TowerLampData_Green"), _T("Initial_State"), _T("0"), (LPWSTR)chData, 10, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk<0 || nChk>2)
	{
		st_lamp_info.nLampG[4] = 2;
	}
	else
	{
		st_lamp_info.nLampG[4] = nChk;
	}

	:: GetPrivateProfileString(_T("TowerLampData_Green"), _T("Warring_State"), _T("0"), (LPWSTR)chData, 10, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk<0 || nChk>2)
	{
		st_lamp_info.nLampG[5] = 2;
	}
	else
	{
		st_lamp_info.nLampG[5] = nChk;
	}

	:: GetPrivateProfileString(_T("TowerLampData_Green"), _T("Lock_State"), _T("0"), (LPWSTR)chData, 10, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk<0 || nChk>2)
	{
		st_lamp_info.nLampG[6] = 2;
	}
	else
	{
		st_lamp_info.nLampG[6] = nChk;
	}

	:: GetPrivateProfileString(_T("TowerLampData_Green"), _T("SelfCheck_State"), _T("0"), (LPWSTR)chData, 10, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk<0 || nChk>2)
	{
		st_lamp_info.nLampG[7] = 2;
	}
	else
	{
		st_lamp_info.nLampG[7] = nChk;
	}

	:: GetPrivateProfileString(_T("TowerLampData_Green"), _T("Idle_State"), _T("0"), (LPWSTR)chData, 10, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk<0 || nChk>2)
	{
		st_lamp_info.nLampG[8] = 2;
	}
	else
	{
		st_lamp_info.nLampG[8] = nChk;
	}
	// **************************************************************************

	// **************************************************************************
	// ���� ��� ��� �ε��Ͽ� ���� ������ ����                                  
	// -> 0:���    1:�̻��                                                     
	// **************************************************************************
	:: GetPrivateProfileString(_T("TowerLampData"), _T("n_buzzer_mode"), _T("0"), (LPWSTR)chData, 10, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk<0 || nChk>1)
	{
		st_lamp_info.nBuzzerMode = 1;
	}
	else
	{
		st_lamp_info.nBuzzerMode = nChk;
	}
	// **************************************************************************

	// **************************************************************************
	// ��� ȣ�� �� ��� �ڵ� �ε��Ͽ� ���� ������ ����                          
	// -> �ε� �������� ��/�� ������ �����Ѵ�                                    
	// **************************************************************************
	:: GetPrivateProfileString(_T("TowerLampData"), _T("str_equip_no"), _T("AMT"), (LPWSTR)chData, 20, strLoadFile);
//	sprintf(chData,"%S", chData);
	st_lamp_info.strEquipNo.Format(_T("%s"), chData);
	(st_lamp_info.strEquipNo).TrimLeft(' ');               
	(st_lamp_info.strEquipNo).TrimRight(' ');

	:: GetPrivateProfileString(_T("TowerLampData"), _T("str_equip_code"), _T("AMT"), (LPWSTR)chData, 20, strLoadFile);
//	sprintf(chData,"%S", chData);
	st_lamp_info.strEquipCode.Format(_T("%s"), chData);
	(st_lamp_info.strEquipCode).TrimLeft(' ');               
	(st_lamp_info.strEquipCode).TrimRight(' ');
	// **************************************************************************

	// **************************************************************************
	// Ÿ�� ���� ON/OFF ��� �ð� �ε��Ͽ� ���� ������ ����                      
	// **************************************************************************
	:: GetPrivateProfileString(_T("TowerLampData"), _T("n_lamp_on_time_w"), _T("0"), (LPWSTR)chData, 10, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk < 1)
	{
		st_lamp_info.nLampWaitTime = 500;
	}
	else
	{
		st_lamp_info.nLampWaitTime = nChk;
	}
	// ***************************************************************************/

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//2016.1109
	:: GetPrivateProfileString(_T("FTP"), _T("ftp_path"), _T(""), (LPWSTR)chData, 200, strLoadFile);
	st_lamp_info.str_ftp_path = chData;
	(st_lamp_info.str_ftp_path).TrimLeft(' ');               
	(st_lamp_info.str_ftp_path).TrimRight(' ');

	:: GetPrivateProfileString(_T("FTP"), _T("ftp_ip"), _T(""), (LPWSTR)chData, 20, strLoadFile);
	st_lamp_info.str_ftp_ip = chData;
	(st_lamp_info.str_ftp_ip).TrimLeft(' ');               
	(st_lamp_info.str_ftp_ip).TrimRight(' ');

	:: GetPrivateProfileString(_T("FTP"), _T("ftp_user"), _T(""), (LPWSTR)chData, 20, strLoadFile);
	st_lamp_info.str_ftp_user = chData;
	(st_lamp_info.str_ftp_user).TrimLeft(' ');               
	(st_lamp_info.str_ftp_user).TrimRight(' ');

	:: GetPrivateProfileString(_T("FTP"), _T("ftp_pw"), _T(""), (LPWSTR)chData, 20, strLoadFile);
	st_lamp_info.str_ftp_pw = chData;
	(st_lamp_info.str_ftp_pw).TrimLeft(' ');               
	(st_lamp_info.str_ftp_pw).TrimRight(' ');

	:: GetPrivateProfileString(_T("FTP"), _T("ftp_port"), _T(""), (LPWSTR)chData, 20, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk < 0)
	{
		st_lamp_info.n_ftp_port = 21;
		//strTemp = LPCTSTR(_itoa(st_lamp_info.n_ftp_port, (LPWSTR)chr_buf, 10)) ;
		strTemp.Format(_T("%.d"), st_lamp_info.n_ftp_port);
		:: WritePrivateProfileString(_T("FTP"), _T("ftp_port"), LPCTSTR(strTemp), strLoadFile);
	}
	else
		st_lamp_info.n_ftp_port = nChk;

	:: GetPrivateProfileString(_T("FTP"), _T("ftp_scrap_code_name"), _T(""), (LPWSTR)chData, 20, strLoadFile);
	st_lamp_info.str_ftp_scrap_code_name = _T("RETEST_RULE.TXT");
	(st_lamp_info.str_ftp_scrap_code_name).TrimLeft(' ');               
	(st_lamp_info.str_ftp_scrap_code_name).TrimRight(' ');

	:: GetPrivateProfileString(_T("FTP"), _T("ftp_recipe_rule_name"), _T(""), (LPWSTR)chData, 20, strLoadFile);
	st_lamp_info.str_ftp_recipe_rule_name = _T("RECIPE_RULE.TXT");
	(st_lamp_info.str_ftp_recipe_rule_name).TrimLeft(' ');               
	(st_lamp_info.str_ftp_recipe_rule_name).TrimRight(' ');

	:: GetPrivateProfileString(_T("FTP"), _T("ftp_mode"), _T(""), (LPWSTR)chData, 20, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk < 0 || nChk > 2)
	{
		st_lamp_info.n_mode_ftp = 0;
		//strTemp = LPCTSTR(_itoa(st_basic.mn_mode_ftp, chr_buf, 10)) ;
		strTemp.Format(_T("%d"), st_lamp_info.n_mode_ftp);
		:: WritePrivateProfileString(_T("FTP"), _T("ftp_mode"), LPCTSTR(strTemp), strLoadFile);
	}
	else
		st_lamp_info.n_mode_ftp = nChk;

	:: GetPrivateProfileString(_T("FTP"), _T("ftp_mode_name"), _T(""), (LPWSTR)chData, 20, strLoadFile);
	st_lamp_info.str_mode_ftp = chData;
	(st_lamp_info.str_mode_ftp).TrimLeft(' ');               
	(st_lamp_info.str_mode_ftp).TrimRight(' ');

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

void CMyBasicData::OnMaintenance_Data_Save()
{
	CString strTemp, strPart;  // �ε� ���� �ӽ� ���� ����
	CString strSaveFile;;

	// **************************************************************************
	// Ÿ�� ���� RED ���� ������ ���Ͽ� �����Ѵ�                                 
	// ->  STOP			= 0,
// 		   RUN			= 1,
// 		   ALARM		= 2,
// 		   LOTEND		= 3,
// 		   INIT			= 4,
// 		   WARRING		= 5,
// 		   LOCK			= 6,
// 		   SELFCHECK	= 7														 
	// -> �ε� �� [0:OFF    1:ON    2:FLICKER]                                   
	// **************************************************************************
//	str_save_file = st_path.str_basic + st_basic.str_device_name;  // ƼĪ ������ ���� ���� ����
	strSaveFile = st_path_info.strBasic + _T("Maintenance.TXT");

	strTemp.Format(_T("%d"), st_lamp_info.nLampR[0]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Red"), _T("Stop_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampR[1]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Red"), _T("Run_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampR[2]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Red"), _T("Alarm_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampR[3]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Red"), _T("LotEnd_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampR[4]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Red"), _T("Initial_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampR[5]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Red"), _T("Warring_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampR[6]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Red"), _T("Lock_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampR[7]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Red"), _T("SelfCheck_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampR[8]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Red"), _T("Idle_State"), LPCTSTR(strTemp), strSaveFile);
	// **************************************************************************


	// **************************************************************************
	// Ÿ�� ���� YELLOW ���� ������ ���Ͽ� �����Ѵ�                              
	// ->  STOP			= 0,
// 		   RUN			= 1,
// 		   ALARM		= 2,
// 		   LOTEND		= 3,
// 		   INIT			= 4,
// 		   WARRING		= 5,
// 		   LOCK			= 6,
// 		   SELFCHECK	= 7														 
	// -> �ε� �� [0:OFF    1:ON    2:FLICKER]                                   
	// **************************************************************************
	strTemp.Format(_T("%d"),st_lamp_info.nLampY[0]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Yellow"), _T("Stop_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampY[1]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Yellow"), _T("Run_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampY[2]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Yellow"), _T("Alarm_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampY[3]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Yellow"), _T("LotEnd_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampY[4]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Yellow"), _T("Initial_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampY[5]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Yellow"), _T("Warring_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampY[6]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Yellow"), _T("Lock_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampY[7]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Yellow"), _T("SelfCheck_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampY[8]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Yellow"), _T("Idle_State"), LPCTSTR(strTemp), strSaveFile);

	// **************************************************************************

	// **************************************************************************
	// Ÿ�� ���� GREEN ���� ������ ���Ͽ� �����Ѵ�                               
	// ->  STOP			= 0,
// 		   RUN			= 1,
// 		   ALARM		= 2,
// 		   LOTEND		= 3,
// 		   INIT			= 4,
// 		   WARRING		= 5,
// 		   LOCK			= 6,
// 		   SELFCHECK	= 7														 
	// -> �ε� �� [0:OFF    1:ON    2:FLICKER]                                   
	// **************************************************************************
	strTemp.Format(_T("%d"),st_lamp_info.nLampG[0]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Green"), _T("Stop_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampG[1]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Green"), _T("Run_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampG[2]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Green"), _T("Alarm_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampG[3]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Green"), _T("LotEnd_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampG[4]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Green"), _T("Initial_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampG[5]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Green"), _T("Warring_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampG[6]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Green"), _T("Lock_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampG[7]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Green"), _T("SelfCheck_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampG[8]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Green"), _T("Idle_State"), LPCTSTR(strTemp), strSaveFile);
	// **************************************************************************

	// **************************************************************************
	// ���� ��� ��� �ε��Ͽ� ���� ������ ����                                  
	// -> 0:���    1:�̻��                                                     
	// **************************************************************************
	strTemp.Format(_T("%d"),st_lamp_info.nBuzzerMode) ;
	:: WritePrivateProfileString(_T("TowerLampData"), _T("n_buzzer_mode"), LPCTSTR(strTemp), strSaveFile);
	// **************************************************************************

	// **************************************************************************
	// ��� ȣ�� �� ��� �ڵ� �ε��Ͽ� ���� ������ ����                          
	// -> �ε� �������� ��/�� ������ �����Ѵ�                                    
	// **************************************************************************
	:: WritePrivateProfileString(_T("TowerLampData"), _T("str_equip_no"), LPCTSTR(st_lamp_info.strEquipNo), strSaveFile);
	:: WritePrivateProfileString(_T("TowerLampData"), _T("str_equip_code"), LPCTSTR(st_lamp_info.strEquipCode), strSaveFile);
	// **************************************************************************

	// **************************************************************************
	// Ÿ�� ���� ON/OFF ��� �ð� �ε��Ͽ� ���� ������ ����                      
	// **************************************************************************
	strTemp.Format(_T("%d"),st_lamp_info.nLampWaitTime);
	:: WritePrivateProfileString(_T("TowerLampData"), _T("n_lamp_on_time_w"), LPCTSTR(strTemp), strSaveFile);
	
}

void CMyBasicData::OnModuleRobot_Teach_Data_Save()
{
	CString strTemp, strMsg, strSaveFail;       // �ӽ� ���� ����
	CString strPos;
	CString strHead, strItem;
	int i, j;
	
	strSaveFail = st_path_info.strFileMotor + st_basic_info.strDeviceName;
	
	for(i=0; i<MAXMOTOR; i++)
	{
		strHead.Format(_T("Motor%d"),i+1);
		for(j=0; j<M_MAX_POS; j++)
		{
			strItem.Format(_T("%02d_Axis_[%02d]"), i+1, j+1); 
			strTemp.Format(_T("%.3f"), st_motor_info[i].d_pos[j]);
			//:: WritePrivateProfileString(strHead, strItem, LPCTSTR(strTemp), strSaveFail);
			if (clsExcel.m_nAxisPosData[i][j] == NO)
			{
				:: WritePrivateProfileString(strHead, strItem, LPCTSTR(strTemp), strSaveFail);
			}
			else
			{
				:: WritePrivateProfileString(strHead, strItem, LPCTSTR(strTemp), st_path_info.strFileBasic);
			}
		}
	}
}

void CMyBasicData::OnModuleRobot_Teach_Data_Load()
{
	CString strTemp, strMsg, strSaveFail;       // �ӽ� ���� ����
	CString strPos;
	CString strHead, strItem;
	int i, j;
	char chData[100];

	strSaveFail = st_path_info.strFileMotor + st_basic_info.strDeviceName;
	
	for(i=0; i<MAXMOTOR; i++)
	{
		strHead.Format(_T("Motor%d"),i+1);
		for(j=0; j<M_MAX_POS; j++)
		{
			strItem.Format(_T("%02d_Axis_[%02d]"), i+1, j+1); 
			if (clsExcel.m_nAxisPosData[i][j] == NO)
			{
				:: GetPrivateProfileString(strHead, strItem, _T("0.0"), (LPWSTR)chData, 10, strSaveFail);
			}
			else
			{
				:: GetPrivateProfileString(strHead, strItem, _T("0.0"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
			}
			strTemp.Format(_T("%s"), chData);
			st_motor_info[i].d_pos[j] = _wtof(strTemp);;
		}
	}
}

void CMyBasicData::OnModuleRobot_Teach_Data_Load(CString strDeviceName)
{
	CString strTemp, strMsg, strSaveFail;       // �ӽ� ���� ����
	CString strPos;
	CString strHead, strItem;
	int i, j;
	char chData[100];

	strSaveFail = st_path_info.strFileMotor + st_basic_info.strDeviceName;
	
	for(i=0; i<MAXMOTOR; i++)
	{
		strHead.Format(_T("Motor%d"),i+1);
		for(j=0; j<M_MAX_POS; j++)
		{
			strItem.Format(_T("%02d_Axis_[%02d]"), i+1, j+1); 
			if (clsExcel.m_nAxisPosData[i][j] == NO)
			{
				:: GetPrivateProfileString(strHead, strItem, _T("0.0"), (LPWSTR)chData, 10, strSaveFail);
			}
			else
			{
				:: GetPrivateProfileString(strHead, strItem, _T("0.0"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
			}
			strTemp.Format(_T("%s"), chData);
			st_motor_info[i].d_pos[j] = _wtof(strTemp);;
		}
	}
}

void CMyBasicData::OnBasic_Data_Load(int nMode)
{
	CString str_load_device;	// �ε� ����̽��� ���� ����
	CString str_load_pgm;		// �ε� ����̽��� ���� ����
	CString str_load_file;
	CString str_chk_ext;		// ���� Ȯ���� ���� ����
	CString str_temp, stemp;	// ������ ���� �ӽ� ���� ���� 
	CString strTemp;
	CString str_pos;
	TCHAR chr_data[50], chr_buf[20];
	TCHAR chData[50];
	int mn_chk, i, j, k;
	double md_chk;

	//////////////////////////////////////////////////////////
	//2016.1203
	CTime time;
	int nDay1[2];
	int nHour1[2];
	//////////////////////////////////////////////////////////


	memset(&chr_data, 0, sizeof(chr_data));
	memset(&chr_buf, 0, sizeof(chr_buf));

	// ���� ���ϸ� ��������
	if(st_basic_info.strDeviceName == "")
	{
		GetPrivateProfileString(_T("FILE_NAME"), _T("Device_Type"), _T("DEFAULT.TXT"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		st_basic_info.strDeviceName = (LPCTSTR)chr_data;
	}
	// ���� ��� 
	str_load_file = st_path_info.strBasic + st_basic_info.strDeviceName;  // ƼĪ ������ ���� ���� ����

	st_basic_info.nCtrlMode = EQP_OFF_LINE;

	GetPrivateProfileString(_T("BASIC_SCREEN"), _T("strEqp"), _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
	st_basic_info.strEqp = (LPCTSTR)chr_data;

	if (nMode == 0)
	{
		GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nModeInterface"), _T("1"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), chr_data);
		mn_chk = _wtoi(str_temp);
		if (mn_chk < 0)
		{
			st_basic_info.nModeInterface = 1;
		}
		else  st_basic_info.nModeInterface = mn_chk;


		GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nModeTestInterface"), _T("1"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), chr_data);
		mn_chk = _wtoi(str_temp);
		if (mn_chk < 0)
		{
			st_basic_info.nModeTestInterface = 1;
		}
		else  st_basic_info.nModeTestInterface = mn_chk;


		GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nModeXgem"), _T("1"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), chr_data);
		mn_chk = _wtoi(str_temp);
		if (mn_chk < 0)
		{
			st_basic_info.nModeXgem = 1;
		}
		else  st_basic_info.nModeXgem = mn_chk;

		GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nModeXgemRunChk"), _T("1"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), chr_data);
		mn_chk = _wtoi(str_temp);
		if (mn_chk < 0)
		{
			st_basic_info.nModeXgemRunChk = 1;
		}
		else  st_basic_info.nModeXgemRunChk = mn_chk;

		GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nModeXgemInterface"), _T("1"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), chr_data);
		mn_chk = _wtoi(str_temp);
		if (mn_chk < 0)
		{
			st_basic_info.nModeXgemInterface = 1;
		}
		else  st_basic_info.nModeXgemInterface = mn_chk;

		GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nModeDevice"), _T("1"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), chr_data);
		mn_chk = _wtoi(str_temp);
		if (mn_chk < 0)
		{
			st_basic_info.nModeDevice = 1;
		}
		else  st_basic_info.nModeDevice = mn_chk;

		GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nModeWork"), _T("1"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), chr_data);
		mn_chk = _wtoi(str_temp);
		if (mn_chk < 0)
		{
			st_basic_info.nModeWork = 1;
		}
		else  st_basic_info.nModeWork = mn_chk;
	}

	//kwlee 2016.0809
	for (int i =0; i<8; i++)
	{
		strTemp.Format(_T("TEST_SITE_MODE_%02d"), i+1);
		GetPrivateProfileString(_T("BASIC_SCREEN"), strTemp, _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), chr_data);
		mn_chk = _wtoi(str_temp);
		if (mn_chk < 0)
		{
			st_test_site_info[i].nInterface = 0;
		}
		else  st_test_site_info[i].nInterface = mn_chk;
		
	}


	GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nModeJigStack"), _T("1"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
	str_temp.Format(_T("%s"), chr_data);
	mn_chk = _wtoi(str_temp);
	if (mn_chk < 0)
	{
		st_basic_info.nModeJigStack = 1;
	}
	else  st_basic_info.nModeJigStack = mn_chk;

	GetPrivateProfileString(_T("BASIC_SCREEN"), _T("strLastOperatorName"), _T("MAINT"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
	st_worker_info.strWorker_Name.Format(_T("%s"), chr_data);

	GetPrivateProfileString(_T("BASIC_SCREEN"), _T("strLastOperatorCrem"), _T("A"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
	st_worker_info.strWorker_Crem.Format(_T("%s"), chr_data);

	GetPrivateProfileString(_T("BASIC_SCREEN"), _T("strLastOperatorId"), _T("AUTO"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
	st_worker_info.strWorker_Num.Format(_T("%s"), chr_data);

	GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nModeFrontSmema"), _T("1"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
	str_temp.Format(_T("%s"), chr_data);
	mn_chk = _wtoi(str_temp);
	if (mn_chk < 0)
	{
		st_basic_info.nModeFrontSmema = 1;
	}
	else  st_basic_info.nModeFrontSmema = mn_chk;

	GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nModeRearSmema"), _T("1"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
	str_temp.Format(_T("%s"), chr_data);
	mn_chk = _wtoi(str_temp);
	if (mn_chk < 0)
	{
		st_basic_info.nModeRearSmema = 1;
	}
	else  st_basic_info.nModeRearSmema = mn_chk;

	GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nModeJigLevel"), _T("1"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
	str_temp.Format(_T("%s"), chr_data);
	mn_chk = _wtoi(str_temp);
	if (mn_chk < 0)
	{
		st_basic_info.nModeJigLevel = 1;
	}
	else  st_basic_info.nModeJigLevel = mn_chk;

	GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nModeRfid"), _T("1"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
	str_temp.Format(_T("%s"), chr_data);
	mn_chk = _wtoi(str_temp);
	if (mn_chk < 0)
	{
		st_basic_info.nModeRfid = 1;
	}
	else  st_basic_info.nModeRfid = mn_chk;

	//kwlee 2016.0825
	//kwlee 2016.1007
	//GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nMode1DBcr"), _T("1"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
	GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nMode1DBcr"), _T("1"), chr_data, sizeof(chr_data), str_load_file);
	str_temp.Format(_T("%s"), chr_data);
	mn_chk = _wtoi(str_temp);
	if (mn_chk < 0)
	{
		st_basic_info.nMode1DBcr = 1;
	}
	else  st_basic_info.nMode1DBcr = mn_chk;

	//kwlee 2016.1007
	//GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nMode2DBcr"), _T("1"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
	GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nMode2DBcr"), _T("1"), chr_data, sizeof(chr_data), str_load_file);
	str_temp.Format(_T("%s"), chr_data);
	mn_chk = _wtoi(str_temp);
	if (mn_chk < 0)
	{
		st_basic_info.nMode2DBcr  = 1;
	}
	else  st_basic_info.nMode2DBcr  = mn_chk;
	///////

	//Ld Light Cutain
	GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nModeLdUldLightCutainSkip"), _T("1"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
	str_temp.Format(_T("%s"), chr_data);
	mn_chk = _wtoi(str_temp);
	if (mn_chk < 0)
	{
		st_basic_info.nModeLdUldLightCutainSkip  = 1;
	}
	else  st_basic_info.nModeLdUldLightCutainSkip  = mn_chk;

	//Rej Light Cutain
	GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nModeRejLightCutainSkip"), _T("1"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
	str_temp.Format(_T("%s"), chr_data);
	mn_chk = _wtoi(str_temp);
	if (mn_chk < 0)
	{
		st_basic_info.nModeRejLightCutainSkip     = 1;
	}
	else  st_basic_info.nModeRejLightCutainSkip     = mn_chk;


	GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nPcbRowCnt"), _T("1"), chr_data, sizeof(chr_data), str_load_file);
	str_temp.Format(_T("%s"), chr_data);
	mn_chk = _wtoi(str_temp);
	if (mn_chk < 0)
	{
		st_basic_info.nRowCnt = 1;
	}
	else  st_basic_info.nRowCnt = mn_chk;

	GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nPcbColCnt"), _T("1"), chr_data, sizeof(chr_data), str_load_file);
	str_temp.Format(_T("%s"), chr_data);
	mn_chk = _wtoi(str_temp);
	if (mn_chk < 0)
	{
		st_basic_info.nColCnt = 1;
	}
	else  st_basic_info.nColCnt = mn_chk;

	GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nGmsTime"), _T("1"), chr_data, sizeof(chr_data), str_load_file);
	str_temp.Format(_T("%s"), chr_data);
	mn_chk = _wtoi(str_temp);
	if (mn_chk < 10 || mn_chk < 1440)
	{
		st_basic_info.nGmsTime = 10;
	}
	else  st_basic_info.nGmsTime = mn_chk;
	
	GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nVnRTime"), _T("0"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
	str_temp.Format(_T("%s"), chr_data);
	st_basic_info.nVnRTime = _wtoi(str_temp);

	GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nStackerTray_Count"), _T("0"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
	str_temp.Format(_T("%s"), chr_data);
	st_basic_info.nUldGoodTrayStack_Count = _wtoi(str_temp);
//	st_basic_info.nStackerTray_Count = _wtoi(str_temp);
	//

	//kwlee 2016.1124
	GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nAlarmDelayCnt"), _T("0"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
	str_temp.Format(_T("%s"), chr_data);
	st_basic_info.nAlarmDelayCnt = _wtoi(str_temp);

	GetPrivateProfileString(_T("BASIC_SCREEN"), _T("ModelName"), _T("MDX_MSATA"), chr_data, sizeof(chr_data), str_load_file);
	st_basic_info.strModelName = (LPCTSTR)chr_data;

	GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nPcbArray"), _T("1"), chr_data, sizeof(chr_data), str_load_file);
	str_temp.Format(_T("%s"), chr_data);
	mn_chk = _wtoi(str_temp);
	if (mn_chk < 0)
	{
		st_basic_info.nPcbArray = 1;
	}
	else  st_basic_info.nPcbArray = mn_chk;

	GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nRetry"), _T("1"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
	str_temp.Format(_T("%s"), chr_data);
	mn_chk = _wtoi(str_temp);
	if (mn_chk < 0)
	{
		st_basic_info.nRetry = 1;
	}
	else  st_basic_info.nRetry = mn_chk;

	GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nModeHook"), _T("1"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
	str_temp.Format(_T("%s"), chr_data);
	mn_chk = _wtoi(str_temp);
	if (mn_chk < 0)
	{
		st_basic_info.nModeHook = 1;
	}
	else  st_basic_info.nModeHook = mn_chk;

	GetPrivateProfileString(_T("BASIC_SCREEN"), _T("bEnableFlag"), _T("1"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
	str_temp.Format(_T("%s"), chr_data);
	mn_chk = _wtoi(str_temp);
	if (mn_chk < 0)
	{
		st_basic_info.bEnableFlag = true;
	}
	else
	{
		if (mn_chk == false)
		{
			st_basic_info.bEnableFlag = false;
		}
		else
		{
			st_basic_info.bEnableFlag = true;
		}
	}

	:: GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nRetestCount"), _T("1"), (LPWSTR)chr_data, 30, st_path_info.strFileBasic);
	str_temp.Format(_T("%s"), chr_data);
	st_recipe_info.nTestRetest_Count = _wtoi(str_temp);

	:: GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nFailCount"), _T("1"), (LPWSTR)chr_data, 30, st_path_info.strFileBasic);
	str_temp.Format(_T("%s"), chr_data);
	st_recipe_info.nFailCnt = _wtoi(str_temp);

	:: GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nPartNoDgt"), _T("1"), (LPWSTR)chr_data, 30, st_path_info.strFileBasic);
	str_temp.Format(_T("%s"), chr_data);
	st_basic_info.nPartNoDgt = _wtoi(str_temp);

	:: GetPrivateProfileString(_T("BASIC_SCREEN"), _T("strDevice"), _T(""), (LPWSTR)chr_data, 30, st_path_info.strFileBasic);
	str_temp.Format(_T("%s"), chr_data);
	st_basic_info.strDevice = str_temp;
	
	if (nMode == 0)
	{
		GetPrivateProfileString(_T("ALARM"), _T("COUNT"), _T("1"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), chr_data);
		mn_chk = _wtoi(str_temp);
		if (mn_chk < 0)
		{
			st_alarm_info.nAlarmNum = 0;
		}
		else st_alarm_info.nAlarmNum = mn_chk;

		GetPrivateProfileString(_T("TIME"), _T("RUN_TIME"), _T("1"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), chr_data);
		mn_chk = _wtoi(str_temp);
		if (mn_chk < 0)
		{
			st_handler_info.tRun = 0;
		}
		st_handler_info.tRun = mn_chk;

		GetPrivateProfileString(_T("TIME"), _T("STOP_TIME"), _T("1"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), chr_data);
		mn_chk = _wtoi(str_temp);
		if (mn_chk < 0)
		{
			st_handler_info.tStop = 0;
		}
		st_handler_info.tStop = mn_chk;

		GetPrivateProfileString(_T("TIME"), _T("JAM_TIME"), _T("1"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), chr_data);
		mn_chk = _wtoi(str_temp);
		if (mn_chk < 0)
		{
			st_handler_info.tJam = 0;
		}
		st_handler_info.tJam = mn_chk;

	}

	int nYear, nMonth, nDay, nHour, nMinute, nSecond;

	if (nMode == 0)
	{
		GetPrivateProfileString(_T("TIME"), _T("CREATE_YEAR"), _T("2014"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), chr_data);
		mn_chk = _wtoi(str_temp);
		if (mn_chk < 0)
		{
			nYear = 2014;
		}
		else  nYear = mn_chk;

		GetPrivateProfileString(_T("TIME"), _T("CREATE_MONTH"), _T("1"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), chr_data);
		mn_chk = _wtoi(str_temp);
		if (mn_chk < 0)
		{
			nMonth = 1;
		}
		else  nMonth = mn_chk;

		GetPrivateProfileString(_T("TIME"), _T("CREATE_DAY"), _T("1"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), chr_data);
		mn_chk = _wtoi(str_temp);
		if (mn_chk < 0)
		{
			nDay = 1;
		}
		else  nDay = mn_chk;

		GetPrivateProfileString(_T("TIME"), _T("CREATE_HOUR"), _T("22"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), chr_data);
		mn_chk = _wtoi(str_temp);
		if (mn_chk < 0)
		{
			nHour = 22;
		}
		else  nHour = mn_chk;

		GetPrivateProfileString(_T("TIME"), _T("CREATE_MINUTE"), _T("0"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), chr_data);
		mn_chk = _wtoi(str_temp);
		if (mn_chk < 0)
		{
			nMinute = 0;
		}
		else  nMinute = mn_chk;

		GetPrivateProfileString(_T("TIME"), _T("CREATE_SECOND"), _T("0"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), chr_data);
		mn_chk = _wtoi(str_temp);
		if (mn_chk < 0)
		{
			nSecond = 0;
		}
		else  nSecond = mn_chk;

		st_handler_info.tCreate.SetDateTime(nYear, nMonth, nDay, nHour, nMinute, nSecond);
	}

	// 20140811 jtkim
	GetPrivateProfileString(_T("BASIC_SCREEN"), _T("ftp_strIp"), _T("MAINT"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
	st_client_info[FTP_NETWORK].strIp.Format(_T("%s"), chr_data);

	:: GetPrivateProfileString(_T("BASIC_SCREEN"), _T("ftp_nPort"), _T("21"), (LPWSTR)chData, 20, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_client_info[FTP_NETWORK].nPort = _wtoi(strTemp);

	GetPrivateProfileString(_T("BASIC_SCREEN"), _T("strToolName"), _T("MAINT"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
	st_basic_info.strToolName.Format(_T("%s"), chr_data);

	GetPrivateProfileString(_T("BASIC_SCREEN"), _T("strFtpUser"), _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
	st_basic_info.strFtpUser.Format(_T("%s"), chr_data);

	GetPrivateProfileString(_T("BASIC_SCREEN"), _T("strFtpPass"), _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
	st_basic_info.strFtpPass.Format(_T("%s"), chr_data);

	GetPrivateProfileString(_T("BASIC_SCREEN"), _T("strPathFtpGms"), _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
	st_path_info.strPathFtpGms.Format(_T("%s"), chr_data);

	for (i=0; i<8; i++)
	{
		strTemp.Format(_T("VnR_Voltage_Spec #%02d"), i+1);
		GetPrivateProfileString(_T("BASIC_SCREEN"), strTemp, _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		st_basic_info.strVnrSpec[1][i].Format(_T("%s"), chr_data);

		strTemp.Format(_T("VnR_Resistance_Spec #%02d"), i+1);
		GetPrivateProfileString(_T("BASIC_SCREEN"), strTemp, _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		st_basic_info.strVnrSpec[0][i].Format(_T("%s"), chr_data);

		strTemp.Format(_T("VnR_Voltage_Ucl #%02d"), i+1);
		GetPrivateProfileString(_T("BASIC_SCREEN"), strTemp, _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		st_basic_info.strVnrUcl[1][i].Format(_T("%s"), chr_data);

		strTemp.Format(_T("VnR_Resistance_Ucl #%02d"), i+1);
		GetPrivateProfileString(_T("BASIC_SCREEN"), strTemp, _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		st_basic_info.strVnrUcl[0][i].Format(_T("%s"), chr_data);

		strTemp.Format(_T("VnR_Voltage_Lcl #%02d"), i+1);
		GetPrivateProfileString(_T("BASIC_SCREEN"), strTemp, _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		st_basic_info.strVnrLcl[1][i].Format(_T("%s"), chr_data);

		strTemp.Format(_T("VnR_Resistance_Lcl #%02d"), i+1);
		GetPrivateProfileString(_T("BASIC_SCREEN"), strTemp, _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		st_basic_info.strVnrLcl[0][i].Format(_T("%s"), chr_data);
	}

	// safety����
	:: GetPrivateProfileString(_T("SAFETY"), _T("dTsiteTopPlaceY"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dTsiteTopPlaceY = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dTsiteBottomPlaceY"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dTsiteBottomPlaceY = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dTsiteTopPlaceZ"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dTsiteTopPlaceZ = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dTsiteTopPlaceZ"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dTsiteTopPlaceZ = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dTsiteBottomPlaceZ"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dTsiteBottomPlaceZ = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dTsiteBottomPlaceZ"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dTsiteBottomPlaceZ = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dTsitePickMinX"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dTsitePickMinX = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dTsitePickMaxX"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dTsitePickMaxX = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dTsitePickMinY"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dTsitePickMinY = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dTsitePickMaxY"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dTsitePickMaxY = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dTsitePickZ"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dTsitePickZ = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dLdBufPlace"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dLdBufPlace = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dLdBufPick"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dLdBufPick = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dLdRbtPlaceMinX"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dLdRbtPlaceMinX = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dLdRbtPlaceMaxX"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dLdRbtPlaceMaxX = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dLdRbtPlaceMinY"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dLdRbtPlaceMinY = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dLdRbtPlaceMaxY"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dLdRbtPlaceMaxY = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dLdRbtPickMinX"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dLdRbtPickMinX = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dLdRbtPickMaxX"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dLdRbtPickMaxX = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dLdRbtPickMinY"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dLdRbtPickMinY = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dLdRbtPickMaxY"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dLdRbtPickMaxY = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dTsitePlaceMinX"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dTsitePlaceMinX = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dTsitePlaceMaxX"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dTsitePlaceMaxX = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dTsitePlaceMinY"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dTsitePlaceMinY = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dTsitePlaceMaxY"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dTsitePlaceMaxY = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dTsitePlaceZ"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dTsitePlaceZ = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dUnLdBufPlace"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dUnLdBufPlace = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dUnLdBufPick"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dUnLdBufPick = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dUnLdRbtPlaceMinX"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dUnLdRbtPlaceMinX = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dUnLdRbtPlaceMaxX"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dUnLdRbtPlaceMaxX = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dUnLdRbtPlaceMinY"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dUnLdRbtPlaceMinY = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dUnLdRbtPlaceMaxY"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dUnLdRbtPlaceMaxY = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dUnLdRbtPickMinX"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dUnLdRbtPickMinX = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dUnLdRbtPickMaxX"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dUnLdRbtPickMaxX = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dUnLdRbtPickMinY"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dUnLdRbtPickMinY = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dUnLdRbtPickMaxY"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dUnLdRbtPickMaxY = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dUnLdRbtRejectMinX"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dUnLdRbtRejectMinX = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dUnLdRbtRejectMaxX"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dUnLdRbtRejectMaxX = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dUnLdRbtRejectMinY"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dUnLdRbtRejectMinY = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dUnLdRbtRejectMaxY"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dUnLdRbtRejectMaxY = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dLdShiftX"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dLdShiftX = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dUnLdShiftX"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dUnLdShiftX = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dBarcodeZ"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dBarcodeZ = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dLdRbtZ"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dLdRbtZ = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dUnLdRbtZ"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dUnLdRbtZ = _wtof(strTemp);

	:: GetPrivateProfileString(_T("SAFETY"), _T("dCokZ"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_safety_info.dCokZ = _wtof(strTemp);
	// jtkim 20150624
/*
	if (nMode == 0)
	{
		:: GetPrivateProfileString(_T("LOT_INFO"), _T("LOT_CURR_STATUS"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
		strTemp.Format(_T("%s"), chData);
		st_lot_info[LOT_CURR].nLotStatus = _wtoi(strTemp);

		:: GetPrivateProfileString(_T("LOT_INFO"), _T("LOT_NEXT_STATUS"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
		strTemp.Format(_T("%s"), chData);
		st_lot_info[LOT_NEXT].nLotStatus = _wtoi(strTemp);
	}
*/
	if (nMode == 0)
	{
		for (i=0; i<HSSI_MAX_IO; i++)
		{
			strTemp.Format(_T("OUT_STATUS_%06d"), i);
			GetPrivateProfileString(_T("IO_DATA"), strTemp, _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
			str_temp.Format(_T("%s"), chr_data);
			mn_chk = _wtoi(str_temp);
			if (mn_chk < 0)
			{
				FAS_IO.n_out[i] = 0;
			}
			else  FAS_IO.n_out[i] = mn_chk;
		}
	}

	:: GetPrivateProfileString(_T("BASIC"), _T("nUph"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_count_info.nUph = _wtoi(strTemp);
	if (st_count_info.nUph < 0) st_count_info.nUph = 0;

	:: GetPrivateProfileString(_T("BASIC"), _T("nOldHifix"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_work_info.nOldHifix = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("BASIC"), _T("nLastMdlCount"), _T(""), (LPWSTR)chData, 30, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_basic_info.nLastMdlCount = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("BASIC"), _T("nBcrRead"), _T(""), (LPWSTR)chData, 5, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_basic_info.nBcrRead = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("BASIC"), _T("nBcrFail"), _T(""), (LPWSTR)chData, 5, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_basic_info.nBcrFail = _wtoi(strTemp);

	//str_load_file]
	if (nMode == 0)
	{
		for (i=0; i<2; i++)
		{
			for (j=0; j<2; j++)
			{
				strTemp.Format(_T("COUNT_IN_%02d_%02d"), i+1, j+1);
				GetPrivateProfileString(_T("BASIC_SCREEN"), strTemp, _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
				str_temp.Format(_T("%s"), chr_data);
				mn_chk = _wtoi(str_temp);
				if (mn_chk < 0)
				{
					st_count_info.nInCount[i][j] = 0;
				}
				else  st_count_info.nInCount[i][j] = mn_chk;

				strTemp.Format(_T("COUNT_PRIME_%02d_%02d"), i+1, j+1);
				GetPrivateProfileString(_T("BASIC_SCREEN"), strTemp, _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
				str_temp.Format(_T("%s"), chr_data);
				mn_chk = _wtoi(str_temp);
				if (mn_chk < 0)
				{
					st_count_info.nPrimeCount[i][j] = 0;
				}
				else  st_count_info.nPrimeCount[i][j] = mn_chk;

				strTemp.Format(_T("COUNT_PASS_%02d_%02d"), i+1, j+1);
				GetPrivateProfileString(_T("BASIC_SCREEN"), strTemp, _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
				str_temp.Format(_T("%s"), chr_data);
				mn_chk = _wtoi(str_temp);
				if (mn_chk < 0)
				{
					st_count_info.nPassCount[i][j] = 0;
				}
				else  st_count_info.nPassCount[i][j] = mn_chk;

				strTemp.Format(_T("COUNT_PRIME_REJECT_%02d_%02d"), i+1, j+1);
				GetPrivateProfileString(_T("BASIC_SCREEN"), strTemp, _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
				str_temp.Format(_T("%s"), chr_data);
				mn_chk = _wtoi(str_temp);
				if (mn_chk < 0)
				{
					st_count_info.nPrimeRejectCount[i][j] = 0;
				}
				else  st_count_info.nPrimeRejectCount[i][j] = mn_chk;

				strTemp.Format(_T("COUNT_REJECT_%02d_%02d"), i+1, j+1);
				GetPrivateProfileString(_T("BASIC_SCREEN"), strTemp, _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
				str_temp.Format(_T("%s"), chr_data);
				mn_chk = _wtoi(str_temp);
				if (mn_chk < 0)
				{
					st_count_info.nRejectCount[i][j] = 0;
				}
				else  st_count_info.nRejectCount[i][j] = mn_chk;
			}
		}
	}

	if (nMode == 0)
	{
		// jtkim 20150709
		GetPrivateProfileString(_T("BASIC_SCREEN"), _T("UPH_COUNT"), _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), chr_data);
		mn_chk = _wtoi(str_temp);
		if (mn_chk < 0)
		{
			st_count_info.nUph = 0;
		}
		else  st_count_info.nUph = mn_chk;

		// jtkim 20150709
		GetPrivateProfileString(_T("BASIC_SCREEN"), _T("DAILY_UPH_COUNT"), _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), chr_data);
		mn_chk = _wtoi(str_temp);
		if (mn_chk < 0)
		{
			st_count_info.nDailyUph = 0;
		}
		else  st_count_info.nDailyUph = mn_chk;

		// jtkim 20150709
		GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nUphCnt"), _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), chr_data);
		mn_chk = _wtoi(str_temp);
		if (mn_chk < 0)
		{
			st_count_info.nUphCnt = 0;
		}
		else  st_count_info.nUphCnt = mn_chk;


		// jtkim 20150709
		GetPrivateProfileString(_T("BASIC_SCREEN"), _T("dHourPer"), _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), chr_data);
		md_chk = _wtof(str_temp);
		if (mn_chk < 0)
		{
			st_count_info.dHourPer = 0.0f;
		}
		else  st_count_info.dHourPer = md_chk;

		GetPrivateProfileString(_T("BASIC_SCREEN"), _T("dDailyPer"), _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), chr_data);
		md_chk = _wtof(str_temp);
		if (mn_chk < 0)
		{
			st_count_info.dDailyPer = 0.0f;
		}
		else  st_count_info.dDailyPer = md_chk;
	}

	for (i=0; i<12; i++)
	{
		strTemp.Format(_T("TSITE_OFFSET_Y_%02d"), i);
		GetPrivateProfileString(_T("TSITE_OFFSET"), strTemp, _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), chr_data);
		md_chk = _wtof(str_temp);
		if (md_chk < -1)
		{
			st_recipe_info.dTsiteOffsetY[i] = 0;
		}
		else  st_recipe_info.dTsiteOffsetY[i] = md_chk;
	}

	if (nMode == 0)
	{
		for (i=0; i<24; i++)
		{
			strTemp.Format(_T("UPH%d"), i);
			GetPrivateProfileString(_T("LATEST_UPH"), strTemp, _T("0"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
			str_temp.Format(_T("%s"), chr_data);
			mn_chk = _wtoi(str_temp);

			st_count_info.nLatestUph[i] = mn_chk;
		}
	}

	if (nMode == 0)
	{
		// jtkim 20150323
		for (i=0; i<2; i++)
		{
			for (j=0; j<2; j++)
			{
				for (k=0; k<12; k++)
				{
					strTemp.Format(_T("BdTestCnt_%02d_%02d_%02d"), i+1, j+1, k+1);
					GetPrivateProfileString(_T("BASIC_SCREEN"), strTemp, _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
					str_temp.Format(_T("%s"), chr_data);
					mn_chk = _wtoi(str_temp);
					if (mn_chk < 0)
					{
						st_bd_info[i][j].nBdTestCnt[k] = 0;
					}
					else  st_bd_info[i][j].nBdTestCnt[k] = mn_chk;

					strTemp.Format(_T("BdPassCnt_%02d_%02d_%02d"), i+1, j+1, k+1);
					GetPrivateProfileString(_T("BASIC_SCREEN"), strTemp, _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
					str_temp.Format(_T("%s"), chr_data);
					mn_chk = _wtoi(str_temp);
					if (mn_chk < 0)
					{
						st_bd_info[i][j].nBdPassCnt[k] = 0;
					}
					else  st_bd_info[i][j].nBdPassCnt[k] = mn_chk;

					strTemp.Format(_T("BdInfo_%02d_%02d_%02d"), i+1, j+1, k+1);
					GetPrivateProfileString(_T("BASIC_SCREEN"), strTemp, _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
					str_temp.Format(_T("%s"), chr_data);
					mn_chk = _wtoi(str_temp);
					if (mn_chk < 0)
					{
						st_bd_info[i][j].nBdInfo[k] = 0;
					}
					else  st_bd_info[i][j].nBdInfo[k] = mn_chk;

					strTemp.Format(_T("BdBin_%02d_%02d_%02d"), i+1, j+1, k+1);
					GetPrivateProfileString(_T("BASIC_SCREEN"), strTemp, _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
					str_temp.Format(_T("%s"), chr_data);
					mn_chk = _wtoi(str_temp);
					if (mn_chk < 0)
					{
						st_bd_info[i][j].nBdBin[k] = 0;
					}
					else  st_bd_info[i][j].nBdBin[k] = mn_chk;

					strTemp.Format(_T("BdFailContiCnt_%02d_%02d_%02d"), i+1, j+1, k+1);
					GetPrivateProfileString(_T("BASIC_SCREEN"), strTemp, _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
					str_temp.Format(_T("%s"), chr_data);
					mn_chk = _wtoi(str_temp);
					if (mn_chk < 0)
					{
						st_bd_info[i][j].nBdFailContiCnt[k] = 0;
					}
					else  st_bd_info[i][j].nBdFailContiCnt[k] = mn_chk;

					strTemp.Format(_T("strBinHistory_%02d_%02d_%02d"), i+1, j+1, k+1);
					GetPrivateProfileString(_T("BASIC_SCREEN"), strTemp, _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
					str_temp.Format(_T("%s"), chr_data);
					st_bd_info[i][j].strBinHistory[k] = str_temp;
				}
			}
		}
	}

	if (nMode == 0)
	{
		for (i=0; i<4; i++)
		{
			strTemp.Format(_T("BUFFER_COK_NUM_%02d"), i+1);
			GetPrivateProfileString(_T("BASIC_SCREEN"), strTemp, _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
			str_temp.Format(_T("%s"), chr_data);
			mn_chk = _wtoi(str_temp);
			if (mn_chk < 0)
			{
				st_Cok_Buff_info[i].st_pcb_info.nCOK_IDNum = 0;
			}
			else  st_Cok_Buff_info[i].st_pcb_info.nCOK_IDNum = mn_chk;

			strTemp.Format(_T("BUFFER_COK_COUNT_%02d"), i+1);
			GetPrivateProfileString(_T("BASIC_SCREEN"), strTemp, _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
			str_temp.Format(_T("%s"), chr_data);
			mn_chk = _wtoi(str_temp);
			if (mn_chk < 0)
			{
				st_Cok_Buff_info[i].nCokCount = 0;
			}
			else  st_Cok_Buff_info[i].nCokCount = mn_chk;

			for (j=0; j<24; j++)
			{
				strTemp.Format(_T("BUFFER_COK_%02d_%02d"), i+1, j+1);
				GetPrivateProfileString(_T("BASIC_SCREEN"), strTemp, _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
				str_temp.Format(_T("%s"), chr_data);
				mn_chk = _wtoi(str_temp);
				if (mn_chk < 0)
				{
					st_Cok_Buff_info[i].st_pcb_info.nYesNo[j] = NO;
				}
				else  st_Cok_Buff_info[i].st_pcb_info.nYesNo[j] = mn_chk;
			}
		}
		//kwlee 2016.0809
// 		for (i=0; i<5; i++)
// 		{
// 			strTemp.Format(_T("COK_MODE_%02d"), i+1);
// 			GetPrivateProfileString(_T("BASIC_SCREEN"), strTemp, _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
// 			str_temp.Format(_T("%s"), chr_data);
// 			st_basic_info.strHifixType[i] = str_temp;
// 		}
	
		GetPrivateProfileString(_T("BASIC_SCREEN"), _T("COK_MODE"), _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), chr_data);
		st_basic_info.strHifixType[0] = str_temp;

		
	}

	if (nMode == 0)
	{
		for (i=0; i<4; i++)
		{
			strTemp.Format(_T("BUFFER_COK_NUM_%02d"), i+1);
			GetPrivateProfileString(_T("BASIC_SCREEN"), strTemp, _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
			str_temp.Format(_T("%s"), chr_data);
			mn_chk = _wtoi(str_temp);
			if (mn_chk < 0)
			{
				st_Cok_Buff_info[i].st_pcb_info.nCOK_IDNum = 0;
			}
			else  st_Cok_Buff_info[i].st_pcb_info.nCOK_IDNum = mn_chk;
		}
	}
/*
	for (i=0; i<M_MAX_MOTOR_NUM; i++)
	{
		strTemp.Format(_T("MOTOR_LAST_%02d"), i+1);
		GetPrivateProfileString(_T("BASIC_SCREEN"), strTemp, _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), chr_data);
		st_work_info.dCurrPos[i] = _wtoi(str_temp);
	}
*/
	:: GetPrivateProfileString(_T("BASIC_SCREEN"), _T("strScrapName"), _T(""), (LPWSTR)chr_data, 30, st_path_info.strFileBasic);
	str_temp.Format(_T("%s"), chr_data);
	st_basic_info.strScrapName = str_temp;

	:: GetPrivateProfileString(_T("BASIC_SCREEN"), _T("strScrapHead"), _T(""), (LPWSTR)chr_data, 30, st_path_info.strFileBasic);
	str_temp.Format(_T("%s"), chr_data);
	st_basic_info.strScrapHead = str_temp;

	// jtkim 20150530 ftp ������
	:: GetPrivateProfileString(_T("BASIC"), _T("nFtpVer"), _T(""), (LPWSTR)chData, 5, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_handler_info.nFtpVer = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("BASIC"), _T("nVerUpdate"), _T(""), (LPWSTR)chData, 5, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_handler_info.nVerUpdate = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("BASIC"), _T("nPgmVer"), _T(""), (LPWSTR)chData, 5, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_handler_info.nPgmVer = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("BASIC"), _T("nAutoLine"), _T(""), (LPWSTR)chData, 5, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_handler_info.nAutoLine = _wtoi(strTemp);

	// 20150623 in-line
	GetPrivateProfileString(_T("BASIC_SCREEN"), _T("strProcess"), _T(""), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
	st_basic_info.strProcess.Format(_T("%s"), chr_data);

	// jtkim 20150530 socket ������
	//kwlee 2016.0823
// 	:: GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nSocketCount"), _T(""), (LPWSTR)chData, 20, st_path_info.strFileBasic);
// 	strTemp.Format(_T("%s"), chData);
// 	st_basic_info.nSocketCount = _wtoi(strTemp);

	// jtkim 20150902
	:: GetPrivateProfileString(_T("HIFIX"), _T("HIFIX_01"), _T(""), (LPWSTR)chData, 20, st_path_info.strFileBasic);
	st_work_info.strHifix[0].Format(_T("%s"), chData);

	:: GetPrivateProfileString(_T("HIFIX"), _T("HIFIX_02"), _T(""), (LPWSTR)chData, 20, st_path_info.strFileBasic);
	st_work_info.strHifix[1].Format(_T("%s"), chData);

	:: GetPrivateProfileString(_T("HIFIX"), _T("HIFIX_03"), _T(""), (LPWSTR)chData, 20, st_path_info.strFileBasic);
	st_work_info.strHifix[2].Format(_T("%s"), chData);

	:: GetPrivateProfileString(_T("HIFIX"), _T("HIFIX_04"), _T(""), (LPWSTR)chData, 20, st_path_info.strFileBasic);
	st_work_info.strHifix[3].Format(_T("%s"), chData);

	:: GetPrivateProfileString(_T("HIFIX"), _T("HIFIX_05"), _T(""), (LPWSTR)chData, 20, st_path_info.strFileBasic);
	st_work_info.strHifix[4].Format(_T("%s"), chData);

	// jtkim 20150930
	:: GetPrivateProfileString(_T("BASIC"), _T("nRecipeYesNo"), _T(""), (LPWSTR)chData, 5, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_basic_info.nRecipeYesNo = _wtoi(strTemp);

	// jtkim 20150930
	:: GetPrivateProfileString(_T("BASIC"), _T("nBarcodeBlock"), _T(""), (LPWSTR)chData, 5, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_basic_info.nBarcodeBlock = _wtoi(strTemp);

	
	//kwlee 2016.1124
	:: GetPrivateProfileString(_T("BASIC"), _T("nDirectionCheckSkip"), _T(""), (LPWSTR)chData, 5, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_basic_info.nDirectionCheckSkip = _wtoi(strTemp);


	//kwlee 2016.1201
	:: GetPrivateProfileString(_T("BASIC"), _T("nAutoLoadMode"), _T(""), (LPWSTR)chData, 5, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_basic_info.nAutoLoadMode = _wtoi(strTemp);

	
	//kwlee 2017.0225
	:: GetPrivateProfileString(_T("BASIC"), _T("nLotEndSkipMode"), _T(""), (LPWSTR)chData, 5, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_basic_info.nLotEndSkipMode = _wtoi(strTemp);

	//kwlee 2017.0110
	:: GetPrivateProfileString(_T("BASIC_SCREEN"), _T("m_iVisionOffSetAutoMode"), _T("0"), (LPWSTR)chData, 5, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_handler_info.m_iVisionOffSetAutoMode = _wtoi(strTemp);
	//

	// jtkim 20151229 ������
	:: GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nRetestCount"), _T("1"), (LPWSTR)chData, 20, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	//kwlee 2016.1021
	//st_basic_info.nRetestCount = _wtoi(strTemp);
	st_recipe_info.nTestRetest_Count = _wtoi(strTemp);
	
	

	// jtkim 20160124
	:: GetPrivateProfileString(_T("BASIC_SCREEN"), _T("nAbortPer"), _T("10"), (LPWSTR)chData, 20, st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.nAbortPer = _wtoi(strTemp);

	//2016.1203
	if (nMode == 0)
	{
		time = CTime::GetCurrentTime();
		nDay1[0]	= time.GetDay();
		nHour1[0]	= time.GetHour();

		GetPrivateProfileString(_T("PRODUCT"), _T("Day"), _T("0"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), chr_data);
		mn_chk = _wtoi(str_temp);
		if (mn_chk < 0)
		{
			nDay1[1] = 0;
		}
		else  nDay1[1] = mn_chk;

		GetPrivateProfileString(_T("PRODUCT"), _T("Hour"), _T("0"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), chr_data);
		mn_chk = _wtoi(str_temp);
		if (mn_chk < 0)
		{
			nHour1[1] = 0;
		}
		else  nHour1[1] = mn_chk;

	


		if (nDay1[0] == nDay1[1] && nHour1[0] == nHour1[1])
		{
			GetPrivateProfileString(_T("PRODUCT"), _T("nUph"), _T("0"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
			str_temp.Format(_T("%s"), chr_data);
			mn_chk = _wtoi(str_temp);
			if (mn_chk < 0)
			{
				st_product_info.nUph = 0;
			}
			else  st_product_info.nUph = mn_chk;

			GetPrivateProfileString(_T("PRODUCT"), _T("nSocketOff"), _T("0"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
			str_temp.Format(_T("%s"), chr_data);
			mn_chk = _wtoi(str_temp);
			if (mn_chk < 0)
			{
				st_product_info.nSocketOff = 0;
			}
			else  st_product_info.nSocketOff = mn_chk;

			GetPrivateProfileString(_T("PRODUCT"), _T("nCok"), _T("0"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
			str_temp.Format(_T("%s"), chr_data);
			mn_chk = _wtoi(str_temp);
			if (mn_chk < 0)
			{
				st_product_info.nCok = 0;
			}
			else  st_product_info.nCok = mn_chk;

			GetPrivateProfileString(_T("PRODUCT"), _T("nRunTime"), _T("0"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
			str_temp.Format(_T("%s"), chr_data);
			mn_chk = _wtoi(str_temp);
			if (mn_chk < 0)
			{
				st_product_info.nRunTime = 0;
			}
			else  st_product_info.nRunTime = mn_chk;

			GetPrivateProfileString(_T("PRODUCT"), _T("nStopTime"), _T("0"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
			str_temp.Format(_T("%s"), chr_data);
			mn_chk = _wtoi(str_temp);
			if (mn_chk < 0)
			{
				st_product_info.nStopTime = 0;
			}
			else  st_product_info.nStopTime = mn_chk;

			GetPrivateProfileString(_T("PRODUCT"), _T("nIdleTime"), _T("0"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
			str_temp.Format(_T("%s"), chr_data);
			mn_chk = _wtoi(str_temp);
			if (mn_chk < 0)
			{
				st_product_info.nIdleTime = 0;
			}
			else  st_product_info.nIdleTime = mn_chk;

			GetPrivateProfileString(_T("PRODUCT"), _T("nFrontTime"), _T("0"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
			str_temp.Format(_T("%s"), chr_data);
			mn_chk = _wtoi(str_temp);
			if (mn_chk < 0)
			{
				st_product_info.nFrontTime = 0;
			}
			else  st_product_info.nFrontTime = mn_chk;

			GetPrivateProfileString(_T("PRODUCT"), _T("nAlarmTime"), _T("0"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
			str_temp.Format(_T("%s"), chr_data);
			mn_chk = _wtoi(str_temp);
			if (mn_chk < 0)
			{
				st_product_info.nAlarmTime = 0;
			}
			else  st_product_info.nAlarmTime = mn_chk;

			GetPrivateProfileString(_T("PRODUCT"), _T("nAlarmCount"), _T("0"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
			str_temp.Format(_T("%s"), chr_data);
			mn_chk = _wtoi(str_temp);
			if (mn_chk < 0)
			{
				st_product_info.nAlarmCount = 0;
			}
			else  st_product_info.nAlarmCount = mn_chk;

			GetPrivateProfileString(_T("PRODUCT"), _T("nRetest"), _T("0"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
			str_temp.Format(_T("%s"), chr_data);
			mn_chk = _wtoi(str_temp);
			if (mn_chk < 0)
			{
				st_product_info.nRetest = 0;
			}
			else  st_product_info.nRetest = mn_chk;

			GetPrivateProfileString(_T("PRODUCT"), _T("dMtbi"), _T("0"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
			str_temp.Format(_T("%s"), chr_data);
			md_chk = _wtof(str_temp);
			if (md_chk < 0)
			{
				st_product_info.dMtbi = 0.0f;
			}
			else  st_product_info.dMtbi = md_chk;

			GetPrivateProfileString(_T("PRODUCT"), _T("dPrime"), _T("0"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
			str_temp.Format(_T("%s"), chr_data);
			md_chk = _wtof(str_temp);
			if (md_chk < 0)
			{
				st_product_info.dPrime = 0.0f;
			}
			else  st_product_info.dPrime = md_chk;

			GetPrivateProfileString(_T("PRODUCT"), _T("dCycleTime"), _T("0"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
			str_temp.Format(_T("%s"), chr_data);
			md_chk = _wtof(str_temp);
			if (md_chk < 0)
			{
				st_product_info.dCycleTime = 0.0f;
			}
			else  st_product_info.dCycleTime = md_chk;

			GetPrivateProfileString(_T("PRODUCT"), _T("dTestTime"), _T("0"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
			str_temp.Format(_T("%s"), chr_data);
			md_chk = _wtof(str_temp);
			if (md_chk < 0)
			{
				st_product_info.dTestTime = 0.0f;
			}
			else  st_product_info.dTestTime = md_chk;
		}
		else
		{
			st_product_info.nUph = 0;
			st_product_info.nSocketOff = 0;
			st_product_info.nCok = 0;
			st_product_info.nRunTime = 0;
			st_product_info.nStopTime = 0;
			st_product_info.nIdleTime = 0;
			st_product_info.nFrontTime = 0;
			st_product_info.nAlarmTime = 0;
			st_product_info.nAlarmCount = 0;
			st_product_info.nRetest = 0;
			st_product_info.dMtbi = 0.0f;
			st_product_info.dPrime = 0.0f;
			st_product_info.dCycleTime = 0.0f;
			st_product_info.dTestTime = 0.0f;
		}
	}

}

void CMyBasicData::OnBasic_Data_Load(CString strDeviceName)
{
	CString str_load_device;	// �ε� ����̽��� ���� ����
	CString str_load_pgm;		// �ε� ����̽��� ���� ����
	CString str_load_file;
	CString str_chk_ext;		// ���� Ȯ���� ���� ����
	CString str_temp, stemp;	// ������ ���� �ӽ� ���� ���� 
	CString str_pos;
	TCHAR chr_data[50], chr_buf[20];
//	int mn_chk;


	//2016.1203
	CTime time;

	memset(&chr_data, 0, sizeof(chr_data));
	memset(&chr_buf, 0, sizeof(chr_buf));

	

	// ���� ��� 
	str_load_file = st_path_info.strBasic + strDeviceName;  // ƼĪ ������ ���� ���� ����
}

void CMyBasicData::OnBasic_Data_Save()
{
	CString mstr_temp;  // ������ ���� �ӽ� ���� ���� 
	CString str_save_file;
	CString str_part, str_chk_ext;
	CString str_pos, str_tmp;
	COleDateTime time_cur;

	int i, j, k;
	CTime time;
	/* ************************************************************************** */
	/*  ������ ������ ���� �����Ѵ� [���� Ȯ���� �˻�]                               */
	/* ************************************************************************** */
	:: WritePrivateProfileString(_T("FILE_NAME"), _T("Device_Type"), LPCTSTR(st_basic_info.strDeviceName), st_path_info.strFileBasic);

	str_save_file = st_path_info.strBasic + st_basic_info.strDeviceName;  // ƼĪ ������ ���� ���� ����

	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("strEqp"), LPCTSTR(st_basic_info.strEqp), st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_basic_info.nModeInterface);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nModeInterface"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_basic_info.nModeTestInterface);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nModeTestInterface"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_basic_info.nModeXgem);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nModeXgem"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_basic_info.nModeXgemRunChk);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nModeXgemRunChk"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_basic_info.nModeXgemInterface);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nModeXgemInterface"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_basic_info.nCtrlMode);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nCtrlMode"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_basic_info.nModeDevice);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nModeDevice"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_basic_info.nModeWork);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nModeWork"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_basic_info.nModeJigStack);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nModeJigStack"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("strLastOperatorName"), st_worker_info.strWorker_Name, st_path_info.strFileBasic);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("strLastOperatorCrem"), st_worker_info.strWorker_Crem, st_path_info.strFileBasic);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("strLastOperatorId"), st_worker_info.strWorker_Num, st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_basic_info.nModeFrontSmema);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nModeFrontSmema"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_basic_info.nModeRearSmema);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nModeRearSmema"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_basic_info.nModeJigLevel);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nModeJigLevel"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_basic_info.nModeRfid);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nModeRfid"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	//kwlee 2016.0825
	mstr_temp.Format(_T("%d"), st_basic_info.nMode1DBcr);
	//kwlee 2016.1007
	//:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nMode1DBcr"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nMode1DBcr"), LPCTSTR(mstr_temp), str_save_file);

	//kwlee 2016.1007
	mstr_temp.Format(_T("%d"), st_basic_info.nMode2DBcr );
	//:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nMode2DBcr"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nMode2DBcr"), LPCTSTR(mstr_temp), str_save_file);
	///
	
	//Ld/Uld Light Cutain
	mstr_temp.Format(_T("%d"), st_basic_info.nModeLdUldLightCutainSkip);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nModeLdUldLightCutainSkip"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	//Rej Light Cutain
	mstr_temp.Format(_T("%d"), st_basic_info.nModeRejLightCutainSkip );
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nModeRejLightCutainSkip"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);


	mstr_temp.Format(_T("%d"), st_basic_info.nRowCnt);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nPcbRowCnt"), LPCTSTR(mstr_temp),str_save_file);

	mstr_temp.Format(_T("%d"), st_basic_info.nColCnt);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nPcbColCnt"), LPCTSTR(mstr_temp), str_save_file);

	mstr_temp.Format(_T("%d"), st_basic_info.nGmsTime);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nGmsTime"), LPCTSTR(mstr_temp), str_save_file);

	mstr_temp.Format(_T("%d"), st_basic_info.nVnRTime);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nVnRTime"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	//kwlee 2016.0930
	mstr_temp.Format(_T("%d"), st_basic_info.nUldGoodTrayStack_Count);
	//mstr_temp.Format(_T("%d"), st_basic_info.nStackerTray_Count);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nStackerTray_Count"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("ModelName"), LPCTSTR(st_basic_info.strModelName), str_save_file);
	
	mstr_temp.Format(_T("%d"), st_basic_info.nPcbArray);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nPcbArray"), LPCTSTR(mstr_temp), str_save_file);

	mstr_temp.Format(_T("%d"), st_basic_info.nRetry);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nRetry"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_basic_info.bEnableFlag);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("bEnableFlag"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	//kwlee 2016.1124
	mstr_temp.Format(_T("%d"), st_basic_info.nAlarmDelayCnt);
	//mstr_temp.Format(_T("%d"), st_basic_info.nStackerTray_Count);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nAlarmDelayCnt"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_basic_info.nModeHook);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nModeHook"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_recipe_info.nTestRetest_Count);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nRetestCount"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_recipe_info.nFailCnt);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nFailCount"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_basic_info.nPartNoDgt);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nPartNoDgt"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("strDevice"), LPCTSTR(st_basic_info.strDevice), st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_alarm_info.nAlarmNum);
	:: WritePrivateProfileString(_T("ALARM"), _T("COUNT"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_handler_info.tRun);
	:: WritePrivateProfileString(_T("TIME"), _T("RUN_TIME"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_handler_info.tStop);
	:: WritePrivateProfileString(_T("TIME"), _T("STOP_TIME"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_handler_info.tJam);
	:: WritePrivateProfileString(_T("TIME"), _T("JAM_TIME"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	////2015.0914
	mstr_temp.Format(_T("%d"),st_handler_info.m_tDRef);
	:: WritePrivateProfileString(_T("TIME"), _T("Daily_Ref_Time"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	CString str;
	for (j =0; j<TSITE_SOCKET_CNT; j++)
	{
		str.Format(_T("Daily_RunDown_Time[%d]"), j);
		mstr_temp.Format(_T("%d"),st_handler_info.m_tDRdown[j]);
		:: WritePrivateProfileString(_T("TIME"), str, LPCTSTR(mstr_temp), st_path_info.strFileBasic);

		str.Format(_T("m_tDStop[%d]"), j);
		mstr_temp.Format(_T("%d"),st_handler_info.m_tDStop[j]);
		:: WritePrivateProfileString(_T("TIME"), str, LPCTSTR(mstr_temp), st_path_info.strFileBasic);

		str.Format(_T("Daily_UserStop[%d]"), j);
		mstr_temp.Format(_T("%d"),st_handler_info.m_tDUserStop[j]);
		:: WritePrivateProfileString(_T("TIME"), str, LPCTSTR(mstr_temp), st_path_info.strFileBasic);
		
		//kwlee 2015.1127
// 		str.Format(_T("InStandBy_Time[%d]"), j);
// 		mstr_temp.Format(_T("%d"),st_handler_info.m_tDInStandby[j]);
// 		:: WritePrivateProfileString(_T("TIME"), str, LPCTSTR(mstr_temp), st_path_info.strFileBasic);

		str.Format(_T("InStandBy_Time[%d]"), j);
		mstr_temp.Format(_T("%d"),st_DB_time.n_Db_time[INSTANBY][j]);
		:: WritePrivateProfileString(_T("TIME"), str, LPCTSTR(mstr_temp), st_path_info.strFileBasic);
		///////////
	}

	for ( k = 0; k < TSITE_SOCKET_CNT; k++ )
	{
		str.Format(_T("Daily_Test_Time[%d]"), k);
		mstr_temp.Format(_T("%d"),st_handler_info.m_tDtest[k]);
		:: WritePrivateProfileString(_T("TIME"), str, LPCTSTR(mstr_temp), st_path_info.strFileBasic);

		str.Format(_T("Daily_SokOff_Time[%d]"), k);
		mstr_temp.Format(_T("%d"),st_handler_info.m_tDSokOff[k]);
		:: WritePrivateProfileString(_T("TIME"), str, LPCTSTR(mstr_temp), st_path_info.strFileBasic);
		//kwlee 2015.1127
// 		str.Format(_T("OutStandBy_Time[%d]"), k);
// 		mstr_temp.Format(_T("%d"),st_handler_info.m_tDOutStandby[k]);
// 		:: WritePrivateProfileString(_T("TIME"), str, LPCTSTR(mstr_temp), st_path_info.strFileBasic);

		str.Format(_T("OutStandBy_Time[%d]"), k);
		mstr_temp.Format(_T("%d"),st_DB_time.n_Db_time[OUTREADY][k]);
		:: WritePrivateProfileString(_T("TIME"), str, LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	}
	////////////////

	mstr_temp.Format(_T("%d"), st_handler_info.tCreate.GetYear());
	:: WritePrivateProfileString(_T("TIME"), _T("CREATE_YEAR"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_handler_info.tCreate.GetMonth());
	:: WritePrivateProfileString(_T("TIME"), _T("CREATE_MONTH"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_handler_info.tCreate.GetDay());
	:: WritePrivateProfileString(_T("TIME"), _T("CREATE_DAY"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_handler_info.tCreate.GetHour());
	:: WritePrivateProfileString(_T("TIME"), _T("CREATE_HOUR"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_handler_info.tCreate.GetMinute());
	:: WritePrivateProfileString(_T("TIME"), _T("CREATE_MINUTE"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_handler_info.tCreate.GetSecond());
	:: WritePrivateProfileString(_T("TIME"), _T("CREATE_SECOND"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	// 20140811 jtkim
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("ftp_strIp"), st_client_info[FTP_NETWORK].strIp, st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_client_info[FTP_NETWORK].nPort);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("ftp_nPort"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("strToolName"), st_basic_info.strToolName, st_path_info.strFileBasic);

	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("strFtpUser"), st_basic_info.strFtpUser, st_path_info.strFileBasic);

	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("strFtpPass"), st_basic_info.strFtpPass, st_path_info.strFileBasic);

	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("strPathFtpGms"), st_path_info.strPathFtpGms, st_path_info.strFileBasic);

	// safety����
	mstr_temp.Format(_T("%.3f"), st_safety_info.dTsiteTopPlaceY);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dTsiteTopPlaceY"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dTsiteBottomPlaceY);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dTsiteBottomPlaceY"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dTsiteTopPlaceZ);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dTsiteTopPlaceZ"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dTsiteTopPlaceZ);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dTsiteTopPlaceZ"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	mstr_temp.Format(_T("%.3f"), st_safety_info.dTsiteBottomPlaceZ);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dTsiteBottomPlaceZ"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dTsiteBottomPlaceZ);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dTsiteBottomPlaceZ"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dTsitePickMinX);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dTsitePickMinX"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dTsitePickMaxX);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dTsitePickMaxX"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dTsitePickMinY);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dTsitePickMinY"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dTsitePickMaxY);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dTsitePickMaxY"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dTsitePickZ);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dTsitePickZ"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dLdBufPlace);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dLdBufPlace"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dLdBufPick);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dLdBufPick"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dLdRbtPlaceMinX);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dLdRbtPlaceMinX"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dLdRbtPlaceMaxX);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dLdRbtPlaceMaxX"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dLdRbtPlaceMinY);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dLdRbtPlaceMinY"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dLdRbtPlaceMaxY);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dLdRbtPlaceMaxY"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dLdRbtPickMinX);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dLdRbtPickMinX"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dLdRbtPickMaxX);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dLdRbtPickMaxX"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dLdRbtPickMinY);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dLdRbtPickMinY"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dLdRbtPickMaxY);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dLdRbtPickMaxY"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dTsitePlaceMinX);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dTsitePlaceMinX"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dTsitePlaceMaxX);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dTsitePlaceMaxX"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dTsitePlaceMinY);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dTsitePlaceMinY"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dTsitePlaceMaxY);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dTsitePlaceMaxY"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dTsitePlaceZ);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dTsitePlaceZ"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dUnLdBufPlace);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dUnLdBufPlace"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dUnLdBufPick);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dUnLdBufPick"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dUnLdRbtPlaceMinX);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dUnLdRbtPlaceMinX"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dUnLdRbtPlaceMaxX);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dUnLdRbtPlaceMaxX"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dUnLdRbtPlaceMinY);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dUnLdRbtPlaceMinY"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dUnLdRbtPlaceMaxY);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dUnLdRbtPlaceMaxY"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dUnLdRbtPickMinX);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dUnLdRbtPickMinX"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dUnLdRbtPickMaxX);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dUnLdRbtPickMaxX"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dUnLdRbtPickMinY);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dUnLdRbtPickMinY"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dUnLdRbtPickMaxY);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dUnLdRbtPickMaxY"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dUnLdRbtRejectMinX);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dUnLdRbtRejectMinX"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dUnLdRbtRejectMaxX);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dUnLdRbtRejectMaxX"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dUnLdRbtRejectMinY);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dUnLdRbtRejectMinY"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dUnLdRbtRejectMaxY);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dUnLdRbtRejectMaxY"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dLdShiftX);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dLdShiftX"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dUnLdShiftX);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dUnLdShiftX"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dBarcodeZ);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dBarcodeZ"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dLdRbtZ);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dLdRbtZ"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dUnLdRbtZ);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dUnLdRbtZ"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);
	
	mstr_temp.Format(_T("%.3f"), st_safety_info.dCokZ);
	:: WritePrivateProfileString(_T("SAFETY"), _T("dCokZ"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_lot_info[LOT_CURR].nLotStatus);
	:: WritePrivateProfileString(_T("LOT_INFO"), _T("LOT_CURR_STATUS"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_lot_info[LOT_NEXT].nLotStatus);
	:: WritePrivateProfileString(_T("LOT_INFO"), _T("LOT_NEXT_STATUS"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	for (i=0; i<8; i++)
	{
		mstr_temp.Format(_T("VnR_Voltage_Spec #%02d"), i+1);
		:: WritePrivateProfileString(_T("BASIC_SCREEN"), mstr_temp, st_basic_info.strVnrSpec[1][i], st_path_info.strFileBasic);
		
		mstr_temp.Format(_T("VnR_Resistance_Spec #%02d"), i+1);
		:: WritePrivateProfileString(_T("BASIC_SCREEN"), mstr_temp, st_basic_info.strVnrSpec[0][i], st_path_info.strFileBasic);

		mstr_temp.Format(_T("VnR_Voltage_Ucl #%02d"), i+1);
		:: WritePrivateProfileString(_T("BASIC_SCREEN"), mstr_temp, st_basic_info.strVnrUcl[1][i], st_path_info.strFileBasic);

		mstr_temp.Format(_T("VnR_Resistance_Ucl #%02d"), i+1);
		:: WritePrivateProfileString(_T("BASIC_SCREEN"), mstr_temp, st_basic_info.strVnrUcl[0][i], st_path_info.strFileBasic);

		mstr_temp.Format(_T("VnR_Voltage_Lcl #%02d"), i+1);
		:: WritePrivateProfileString(_T("BASIC_SCREEN"), mstr_temp, st_basic_info.strVnrLcl[1][i], st_path_info.strFileBasic);

		mstr_temp.Format(_T("VnR_Resistance_Lcl #%02d"), i+1);
		:: WritePrivateProfileString(_T("BASIC_SCREEN"), mstr_temp, st_basic_info.strVnrLcl[0][i], st_path_info.strFileBasic);
	}

// 	for (i=0; i<HSSI_MAX_IO; i++)
// 	{
// 		str_tmp.Format(_T("OUT_STATUS_%06d"), i);
// 		mstr_temp.Format(_T("%d"), FAS_IO.n_out[i]);
// 		:: WritePrivateProfileString(_T("IO_DATA"), str_tmp, LPCTSTR(mstr_temp), st_path_info.strFileBasic);
// 	}

	mstr_temp.Format(_T("%d"), st_count_info.nUph);
	:: WritePrivateProfileString(_T("BASIC"), _T("nUph"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_work_info.nOldHifix);
	:: WritePrivateProfileString(_T("BASIC"), _T("nOldHifix"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_basic_info.nLastMdlCount);
	:: WritePrivateProfileString(_T("BASIC"), _T("nLastMdlCount"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_basic_info.nBcrRead);
	:: WritePrivateProfileString(_T("BASIC"), _T("nBcrRead"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	mstr_temp.Format(_T("%d"), st_basic_info.nBcrFail);
	:: WritePrivateProfileString(_T("BASIC"), _T("nBcrFail"), LPCTSTR(mstr_temp), st_path_info.strFileBasic);

	for (i=0; i<2; i++)
	{
		for (j=0; j<2; j++)
		{
/*
			mstr_temp.Format(_T("COUNT_IN_%02d_%02d"), i+1, j+1);
			str_tmp.Format(_T("%d"), st_count_info.nInCount[i][j]);
			:: WritePrivateProfileString(_T("BASIC_SCREEN"), mstr_temp, str_tmp, st_path_info.strFileBasic);

			mstr_temp.Format(_T("COUNT_PRIME_%02d_%02d"), i+1, j+1);
			str_tmp.Format(_T("%d"), st_count_info.nPrimeCount[i][j]);
			:: WritePrivateProfileString(_T("BASIC_SCREEN"), mstr_temp, str_tmp, st_path_info.strFileBasic);

			mstr_temp.Format(_T("COUNT_PASS_%02d_%02d"), i+1, j+1);
			str_tmp.Format(_T("%d"), st_count_info.nPassCount[i][j]);
			:: WritePrivateProfileString(_T("BASIC_SCREEN"), mstr_temp, str_tmp, st_path_info.strFileBasic);

			mstr_temp.Format(_T("COUNT_PRIME_REJECT_%02d_%02d"), i+1, j+1);
			str_tmp.Format(_T("%d"), st_count_info.nPrimeRejectCount[i][j]);
			:: WritePrivateProfileString(_T("BASIC_SCREEN"), mstr_temp, str_tmp, st_path_info.strFileBasic);

			mstr_temp.Format(_T("COUNT_REJECT_%02d_%02d"), i+1, j+1);
			str_tmp.Format(_T("%d"), st_count_info.nRejectCount[i][j]);
			:: WritePrivateProfileString(_T("BASIC_SCREEN"), mstr_temp, str_tmp, st_path_info.strFileBasic);
*/
			mstr_temp.Format(_T("COUNT_IN_%02d_%02d"), i+1, j+1);
			str_tmp.Format(_T("%d"), st_count_info.nInCount[i][j]);
			:: WritePrivateProfileString(_T("BASIC_SCREEN"), mstr_temp, str_tmp, st_path_info.strFileBasic);

			mstr_temp.Format(_T("COUNT_PRIME_%02d_%02d"), i+1, j+1);
			str_tmp.Format(_T("%d"), st_count_info.nPrimeCount[i][j]);
			:: WritePrivateProfileString(_T("BASIC_SCREEN"), mstr_temp, str_tmp, st_path_info.strFileBasic);

			mstr_temp.Format(_T("COUNT_PASS_%02d_%02d"), i+1, j+1);
			str_tmp.Format(_T("%d"), st_count_info.nPassCount[i][j]);
			:: WritePrivateProfileString(_T("BASIC_SCREEN"), mstr_temp, str_tmp, st_path_info.strFileBasic);

			mstr_temp.Format(_T("COUNT_PRIME_REJECT_%02d_%02d"), i+1, j+1);
			str_tmp.Format(_T("%d"), st_count_info.nPrimeRejectCount[i][j]);
			:: WritePrivateProfileString(_T("BASIC_SCREEN"), mstr_temp, str_tmp, st_path_info.strFileBasic);

			mstr_temp.Format(_T("COUNT_REJECT_%02d_%02d"), i+1, j+1);
			str_tmp.Format(_T("%d"), st_count_info.nRejectCount[i][j]);
			:: WritePrivateProfileString(_T("BASIC_SCREEN"), mstr_temp, str_tmp, st_path_info.strFileBasic);
		}
	}

	// jtkim 20150709
	str_tmp.Format(_T("%d"), st_count_info.nUph);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("UPH_COUNT"), str_tmp, st_path_info.strFileBasic);

	// jtkim 20150709
	str_tmp.Format(_T("%d"), st_count_info.nDailyUph);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("DAILY_UPH_COUNT"), str_tmp, st_path_info.strFileBasic);

	// jtkim 20150709
	str_tmp.Format(_T("%d"), st_count_info.nUphCnt);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nUphCnt"), str_tmp, st_path_info.strFileBasic);

	// jtkim 20150709
	str_tmp.Format(_T("%.2f"), st_count_info.dHourPer);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("dHourPer"), str_tmp, st_path_info.strFileBasic);

	str_tmp.Format(_T("%.2f"), st_count_info.dDailyPer);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("dDailyPer"), str_tmp, st_path_info.strFileBasic);

	for (i=0; i<12; i++)
	{
		mstr_temp.Format(_T("TSITE_OFFSET_Y_%02d"), i);
		str_tmp.Format(_T("%.3f"), st_recipe_info.dTsiteOffsetY[i]);
		:: WritePrivateProfileString(_T("TSITE_OFFSET"), mstr_temp, str_tmp, st_path_info.strFileBasic);
	}

	for (i=0; i<24; i++)		//2015.03.17 sjs
	{
		mstr_temp.Format(_T("UPH_%dh"), i);
		str_tmp.Format(_T("%d"), st_count_info.nLatestUph[i]);
		:: WritePrivateProfileString(_T("LATEST_UPH"), mstr_temp, str_tmp, st_path_info.strFileBasic);
	}

	// jtkim 20150323
	for (i=0; i<2; i++)
	{
		for (j=0; j<2; j++)
		{
			for (k=0; k<12; k++)
			{
				mstr_temp.Format(_T("BdTestCnt_%02d_%02d_%02d"), i+1, j+1, k+1);
				str_tmp.Format(_T("%d"), st_bd_info[i][j].nBdTestCnt[k]);
				:: WritePrivateProfileString(_T("BASIC_SCREEN"), mstr_temp, str_tmp, st_path_info.strFileBasic);

				mstr_temp.Format(_T("BdPassCnt_%02d_%02d_%02d"), i+1, j+1, k+1);
				str_tmp.Format(_T("%d"), st_bd_info[i][j].nBdPassCnt[k]);
				:: WritePrivateProfileString(_T("BASIC_SCREEN"), mstr_temp, str_tmp, st_path_info.strFileBasic);

				mstr_temp.Format(_T("BdInfo_%02d_%02d_%02d"), i+1, j+1, k+1);
				str_tmp.Format(_T("%d"), st_bd_info[i][j].nBdInfo[k]);
				:: WritePrivateProfileString(_T("BASIC_SCREEN"), mstr_temp, str_tmp, st_path_info.strFileBasic);

				mstr_temp.Format(_T("BdBin_%02d_%02d_%02d"), i+1, j+1, k+1);
				str_tmp.Format(_T("%d"), st_bd_info[i][j].nBdBin[k]);
				:: WritePrivateProfileString(_T("BASIC_SCREEN"), mstr_temp, str_tmp, st_path_info.strFileBasic);

				mstr_temp.Format(_T("BdFailContiCnt_%02d_%02d_%02d"), i+1, j+1, k+1);
				str_tmp.Format(_T("%d"), st_bd_info[i][j].nBdFailContiCnt[k]);
				:: WritePrivateProfileString(_T("BASIC_SCREEN"), mstr_temp, str_tmp, st_path_info.strFileBasic);

				mstr_temp.Format(_T("strBinHistory_%02d_%02d_%02d"), i+1, j+1, k+1);
				:: WritePrivateProfileString(_T("BASIC_SCREEN"), mstr_temp, st_bd_info[i][j].strBinHistory[k], st_path_info.strFileBasic);
			}
		}
	}


	for (i=0; i<4; i++)
	{
		mstr_temp.Format(_T("BUFFER_COK_NUM_%02d"), i+1);
		str_tmp.Format(_T("%d"), st_Cok_Buff_info[i].st_pcb_info.nCOK_IDNum);
		:: WritePrivateProfileString(_T("BASIC_SCREEN"), mstr_temp, str_tmp, st_path_info.strFileBasic);

		mstr_temp.Format(_T("BUFFER_COK_COUNT_%02d"), i+1);
		str_tmp.Format(_T("%d"), st_Cok_Buff_info[i].nCokCount);
		:: WritePrivateProfileString(_T("BASIC_SCREEN"), mstr_temp, str_tmp, st_path_info.strFileBasic);

		for (j=0; j<24; j++)
		{
			mstr_temp.Format(_T("BUFFER_COK_%02d_%02d"), i+1, j+1);
			str_tmp.Format(_T("%d"), st_Cok_Buff_info[i].st_pcb_info.nYesNo[j]);
			:: WritePrivateProfileString(_T("BASIC_SCREEN"), mstr_temp, str_tmp, st_path_info.strFileBasic);
		}
	}

	//kwlee 2016.0809
	for (i=0; i<8; i++)
	{
		mstr_temp.Format(_T("TEST_SITE_MODE_%02d"), i+1);
		str_tmp.Format(_T("%d"),st_test_site_info[i].nInterface);
		:: WritePrivateProfileString(_T("BASIC_SCREEN"), mstr_temp,str_tmp, st_path_info.strFileBasic);
	}

// 	for (i=0; i<5; i++)
// 	{
// 		mstr_temp.Format(_T("COK_MODE_%02d"), i+1);
// 		:: WritePrivateProfileString(_T("BASIC_SCREEN"), mstr_temp, st_basic_info.strHifixType[i], st_path_info.strFileBasic);
// 	}

	:: WritePrivateProfileString(_T("BASIC_SCREEN"),_T("COK_MODE"), st_basic_info.strHifixType[0], st_path_info.strFileBasic);

	////	
/*
	for (i=0; i<M_MAX_MOTOR_NUM; i++)
	{
		mstr_temp.Format(_T("MOTOR_LAST_%02d"), i+1);
		str_tmp.Format(_T("%.3f"), st_work_info.dCurrPos[i]);
		:: WritePrivateProfileString(_T("BASIC_SCREEN"), mstr_temp, str_tmp, st_path_info.strFileBasic);
	}
*/
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("strScrapName"), st_basic_info.strScrapName, st_path_info.strFileBasic);

	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("strScrapHead"), st_basic_info.strScrapHead, st_path_info.strFileBasic);

	// 20150623 in-line
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("strProcess"), st_basic_info.strProcess, st_path_info.strFileBasic);


	// jtkim 20150709
	str_tmp.Format(_T("%d"), st_handler_info.nFtpVer);
	:: WritePrivateProfileString(_T("BASIC"), _T("nFtpVer"), str_tmp, st_path_info.strFileBasic);

	// jtkim 20150709
	//kwlee 2016.0823
// 	str_tmp.Format(_T("%d"), st_basic_info.nSocketCount);
// 	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nSocketCount"), str_tmp, st_path_info.strFileBasic);

	// jtkim 20150902
	:: WritePrivateProfileString(_T("HIFIX"), _T("HIFIX_01"), st_work_info.strHifix[0], st_path_info.strFileBasic);

	:: WritePrivateProfileString(_T("HIFIX"), _T("HIFIX_02"), st_work_info.strHifix[1], st_path_info.strFileBasic);

	:: WritePrivateProfileString(_T("HIFIX"), _T("HIFIX_03"), st_work_info.strHifix[2], st_path_info.strFileBasic);

	:: WritePrivateProfileString(_T("HIFIX"), _T("HIFIX_04"), st_work_info.strHifix[3], st_path_info.strFileBasic);

	:: WritePrivateProfileString(_T("HIFIX"), _T("HIFIX_05"), st_work_info.strHifix[4], st_path_info.strFileBasic);

	// jtkim 20150930
	str_tmp.Format(_T("%d"), st_basic_info.nRecipeYesNo);
	:: WritePrivateProfileString(_T("BASIC"), _T("nRecipeYesNo"), str_tmp, st_path_info.strFileBasic);


	//kwlee 2016.1124
	str_tmp.Format(_T("%d"), st_basic_info.nDirectionCheckSkip);
	:: WritePrivateProfileString(_T("BASIC"), _T("nDirectionCheckSkip"), str_tmp, st_path_info.strFileBasic);

	//kwlee 2016.1201
	str_tmp.Format(_T("%d"), st_basic_info.nAutoLoadMode);
	:: WritePrivateProfileString(_T("BASIC"), _T("nAutoLoadMode"), str_tmp, st_path_info.strFileBasic);

	//kwlee 2017.0225
	str_tmp.Format(_T("%d"), st_basic_info.nLotEndSkipMode);
	:: WritePrivateProfileString(_T("BASIC"), _T("nLotEndSkipMode"), str_tmp, st_path_info.strFileBasic);
	
	// jtkim 20151229 ������
	//kwlee 2016.1021
	//str_tmp.Format(_T("%d"), st_basic_info.nRetestCount);
	str_tmp.Format(_T("%d"), st_recipe_info.nTestRetest_Count);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nRetestCount"), str_tmp, st_path_info.strFileBasic);

	// jtkim 20160124
	str_tmp.Format(_T("%d"), st_recipe_info.nAbortPer);
	:: WritePrivateProfileString(_T("BASIC_SCREEN"), _T("nAbortPer"), str_tmp, st_path_info.strFileBasic);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//2016.1203
	time = CTime::GetCurrentTime();

	str_tmp.Format(_T("%d"), time.GetDay());
	:: WritePrivateProfileString(_T("PRODUCT"), _T("Day"), str_tmp, st_path_info.strFileBasic);

	str_tmp.Format(_T("%d"), time.GetHour());
	:: WritePrivateProfileString(_T("PRODUCT"), _T("Hour"), str_tmp, st_path_info.strFileBasic);

	str_tmp.Format(_T("%d"), st_product_info.nUph);
	:: WritePrivateProfileString(_T("PRODUCT"), _T("nUph"), str_tmp, st_path_info.strFileBasic);

	str_tmp.Format(_T("%d"), st_product_info.nSocketOff);
	:: WritePrivateProfileString(_T("PRODUCT"), _T("nSocketOff"), str_tmp, st_path_info.strFileBasic);

	str_tmp.Format(_T("%d"), st_product_info.nCok);
	:: WritePrivateProfileString(_T("PRODUCT"), _T("nCok"), str_tmp, st_path_info.strFileBasic);

	str_tmp.Format(_T("%d"), st_product_info.nRunTime);
	:: WritePrivateProfileString(_T("PRODUCT"), _T("nRunTime"), str_tmp, st_path_info.strFileBasic);

	str_tmp.Format(_T("%d"), st_product_info.nStopTime);
	:: WritePrivateProfileString(_T("PRODUCT"), _T("nStopTime"), str_tmp, st_path_info.strFileBasic);

	str_tmp.Format(_T("%d"), st_product_info.nIdleTime);
	:: WritePrivateProfileString(_T("PRODUCT"), _T("nIdleTime"), str_tmp, st_path_info.strFileBasic);

	str_tmp.Format(_T("%d"), st_product_info.nFrontTime);
	:: WritePrivateProfileString(_T("PRODUCT"), _T("nFrontTime"), str_tmp, st_path_info.strFileBasic);

	str_tmp.Format(_T("%d"), st_product_info.nAlarmTime);
	:: WritePrivateProfileString(_T("PRODUCT"), _T("nAlarmTime"), str_tmp, st_path_info.strFileBasic);

	str_tmp.Format(_T("%d"), st_product_info.nAlarmCount);
	:: WritePrivateProfileString(_T("PRODUCT"), _T("nAlarmCount"), str_tmp, st_path_info.strFileBasic);

	str_tmp.Format(_T("%d"), st_product_info.nRetest);
	:: WritePrivateProfileString(_T("PRODUCT"), _T("nRetest"), str_tmp, st_path_info.strFileBasic);

	str_tmp.Format(_T("%.1f"), st_product_info.dMtbi);
	:: WritePrivateProfileString(_T("PRODUCT"), _T("dMtbi"), str_tmp, st_path_info.strFileBasic);

	str_tmp.Format(_T("%.1f"), st_product_info.dPrime );
	:: WritePrivateProfileString(_T("PRODUCT"), _T("dPrime"), str_tmp, st_path_info.strFileBasic);

	str_tmp.Format(_T("%.1f"), st_product_info.dCycleTime);
	:: WritePrivateProfileString(_T("PRODUCT"), _T("dCycleTime"), str_tmp, st_path_info.strFileBasic);

	str_tmp.Format(_T("%.1f"), st_product_info.dTestTime);
	:: WritePrivateProfileString(_T("PRODUCT"), _T("dTestTime"), str_tmp, st_path_info.strFileBasic);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

CString CMyBasicData::OnGet_File_Name()
{
	CString strTemp;       // �ӽ� ���� ����
	CString strSaveFile;  // ���� ���� �ӽ� ���� ����
	CString strChkExt;    // ���� Ȯ���� ���� ����
	CString strPart, strPart2;
	CString strNewSaveFile;
	int nPos;
	
	// **************************************************************************
    // Socket Contact Count ������ ������ ���� �����Ѵ� [���� Ȯ���� �˻�]        
    // **************************************************************************
	strSaveFile = st_path_info.strPathDvc + st_basic_info.strDeviceName;  // ƼĪ ������ ���� ���� ����
	
	nPos = strSaveFile.Find(_T("."));  // Ȯ���� ��ġ �˻�
	
	if (nPos == -1)
	{
		strSaveFile += _T(".DAT");  // Ȯ���� �߰�
	}
	else 
	{
		strChkExt = strSaveFile.Mid(nPos);  // ���� Ȯ���� ����

		if (strChkExt != _T(".TXT"))  
		{
			strSaveFile = st_path_info.strPathDvc + _T("DEFAULT.TXT");  // ƼĪ ������ ���� ���ο� ���� ����
/*			
			if (st_handler.cwnd_list != NULL)	// ����Ʈ �� ȭ�� ����
			{
				st_other.str_abnormal_msg = _T("[DEVICE FILE] The error happened at a file extension.");
				sprintf(st_other.c_abnormal_msg, st_other.str_abnormal_msg);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ��� ��û
			}*/
		}
	}
	// **************************************************************************
	
	return strSaveFile;  // ���ϸ� ���� 
}

void CMyBasicData::OnDeviec_Folder_Load()
{

}

void CMyBasicData::OnDevice_Folder_Save()
{
	
}

void CMyBasicData::OnInterface_Data_Load()
{
	int i;
	CString str_temp, str_name,str;
	char ch_data[100];
//	char ch_tmp;

	for(i=0; i<20; i++)
	{
		str_name.Format(_T("%02d_CLIENT_IP"), i);
		:: GetPrivateProfileString(_T("INTERFACE_SCREEN"), str_name, _T(""), (LPWSTR)ch_data, 100, st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), ch_data);
		st_client_info[i].strIp = str_temp;

		str_name.Format(_T("%02d_CLIENT_PORT"), i);
		:: GetPrivateProfileString(_T("INTERFACE_SCREEN"), str_name, _T(""), (LPWSTR)ch_data, 100, st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), ch_data);
		st_client_info[i].nPort = _wtoi(str_temp);

		str_name.Format(_T("%02d_SERVER_PORT"), i);
		:: GetPrivateProfileString(_T("INTERFACE_SCREEN"), str_name, _T(""), (LPWSTR)ch_data, 100, st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), ch_data);
		st_server_info[i].nPort = _wtoi(str_temp);

		//str.Format(_T("TC%d"),i +1);
		str_name.Format(_T("%02d_TESTER_CLIENT_NAME"), i);
		:: GetPrivateProfileString(_T("INTERFACE_SCREEN"), str_name,  _T(""), (LPWSTR)ch_data, 100, st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), ch_data);
		st_client_info[i].strName = str_temp;
		if (i >= INTERFACE_NETWORK_1 && i <= INTERFACE_NETWORK_8)
		{
			st_test_site_info[THD_TESTSITE_1 + (i - INTERFACE_NETWORK_1)].strEqpID = st_client_info[i].strName;
		}
		
		str_name.Format(_T("%02d_TESTER_SERVER_NAME"), i);
	//	str.Format(_T("TS%d"),i +1);
		:: GetPrivateProfileString(_T("INTERFACE_SCREEN"), str_name, _T(""), (LPWSTR)ch_data, 100, st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), ch_data);
		st_server_info[i].strName = str_temp;
		////////////////////////////////////
	}

	for(i=0; i<10; i++)
	{
		str_name.Format(_T("PORT_%02d"), i+1);
		:: GetPrivateProfileString(_T("SERIAL"), str_name, _T("1"), (LPWSTR)ch_data, 100, st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), ch_data);
		st_serial_info.nSerialPort[i] = _wtoi(str_temp);
		if(st_serial_info.nSerialPort[i] < 1)
			st_serial_info.nSerialPort[i] = 1;
		
		str_name.Format(_T("BAUDRATE_%02d"), i+1);
		:: GetPrivateProfileString(_T("SERIAL"), str_name, _T("9600"), (LPWSTR)&ch_data, 100, st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), ch_data);
		st_serial_info.nSerialBaudrate[i] = _wtoi(str_temp);

		
		str_name.Format(_T("DATA_%02d"), i+1);
		:: GetPrivateProfileString(_T("SERIAL"), str_name, _T("8"), (LPWSTR)ch_data, 100, st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), ch_data);
		st_serial_info.nSerialData[i] = _wtoi(str_temp);
		
		str_name.Format(_T("STOP_%02d"), i+1);
		:: GetPrivateProfileString(_T("SERIAL"), str_name, _T("1"), (LPWSTR)ch_data, 100, st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), ch_data);
		st_serial_info.nSerialStop[i] = _wtoi(str_temp);
		
		str_name.Format(_T("PARITY_%02d"), i+1);
		:: GetPrivateProfileString(_T("SERIAL"), str_name, _T("0"), (LPWSTR)ch_data, 100, st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), ch_data);
		st_serial_info.nSerialParity[i] = _wtoi(str_temp);	
	}
}

void CMyBasicData::OnInterface_Data_Save()
{
	int i;
	CString str_tmp, str_name;

	for(i=0; i<20; i++)
	{
		str_name.Format(_T("%02d_CLIENT_IP"), i);
		str_tmp.Format(_T("%s"), st_client_info[i].strIp);
		:: WritePrivateProfileString(_T("INTERFACE_SCREEN"), str_name, LPCTSTR(str_tmp), st_path_info.strFileBasic);

		str_name.Format(_T("%02d_CLIENT_PORT"), i);
		str_tmp.Format(_T("%d"), st_client_info[i].nPort);
		:: WritePrivateProfileString(_T("INTERFACE_SCREEN"), str_name, LPCTSTR(str_tmp), st_path_info.strFileBasic);
		///////////////////

		str_name.Format(_T("%02d_TESTER_CLIENT_NAME"), i);
		str_tmp.Format(_T("%s"), st_client_info[i].strName);
		:: WritePrivateProfileString(_T("INTERFACE_SCREEN"), str_name, LPCTSTR(str_tmp), st_path_info.strFileBasic);

// 		str_name.Format(_T("%02d_TESTER_SERVER_NAME"), i);
// 		str_tmp.Format(_T("%s"), st_server_info[i].strName);
// 		:: WritePrivateProfileString(_T("INTERFACE_SCREEN"), str_name, LPCTSTR(str_tmp), st_path_info.strFileBasic);
	}
		////////////
	for(i=0; i<20; i++)
	{
		str_name.Format(_T("%02d_SERVER_PORT"), i);
		str_tmp.Format(_T("%d"), st_server_info[i].nPort);
		:: WritePrivateProfileString(_T("INTERFACE_SCREEN"), str_name, LPCTSTR(str_tmp), st_path_info.strFileBasic);

		str_name.Format(_T("PORT_%02d"), i+1);
		str_tmp.Format(_T("%d"), st_serial_info.nSerialPort[i]);
		:: WritePrivateProfileString(_T("SERIAL"), str_name, LPCTSTR(str_tmp), st_path_info.strFileBasic);

		str_name.Format(_T("BAUDRATE_%02d"), i+1);
		str_tmp.Format(_T("%d"), st_serial_info.nSerialBaudrate[i]);
		:: WritePrivateProfileString(_T("SERIAL"), str_name, LPCTSTR(str_tmp), st_path_info.strFileBasic);

		str_name.Format(_T("DATA_%02d"), i+1);
		str_tmp.Format(_T("%d"), st_serial_info.nSerialData[i]);
		:: WritePrivateProfileString(_T("SERIAL"), str_name, LPCTSTR(str_tmp), st_path_info.strFileBasic);

		str_name.Format(_T("STOP_%02d"), i+1);
		str_tmp.Format(_T("%d"), st_serial_info.nSerialStop[i]);
		:: WritePrivateProfileString(_T("SERIAL"), str_name, LPCTSTR(str_tmp), st_path_info.strFileBasic);

		str_name.Format(_T("PARITY_%02d"), i+1);
		str_tmp.Format(_T("%d"), st_serial_info.nSerialParity[i]);
		:: WritePrivateProfileString(_T("SERIAL"), str_name, LPCTSTR(str_tmp), st_path_info.strFileBasic);
	}
}

void CMyBasicData::OnInterface_Data_Save_As(CString strDeviceName)
{
	int i;
	CString str_tmp, str_name;

	for(i=0; i<20; i++)
	{
		str_name.Format(_T("%02d_CLIENT_IP"), i);
		str_tmp.Format(_T("%s"), st_client_info[i].strIp);
		:: WritePrivateProfileString(_T("INTERFACE_SCREEN"), str_name, LPCTSTR(str_tmp), st_path_info.strFileBasic);

		str_name.Format(_T("%02d_CLIENT_PORT"), i);
		str_tmp.Format(_T("%d"), st_client_info[i].nPort);
		:: WritePrivateProfileString(_T("INTERFACE_SCREEN"), str_name, LPCTSTR(str_tmp), st_path_info.strFileBasic);
		///////////

		str_name.Format(_T("%02d_TESTER_CLIENT_NAME"), i);
		str_tmp.Format(_T("%s"), st_client_info[i].strName);
		:: WritePrivateProfileString(_T("INTERFACE_SCREEN"), str_name, LPCTSTR(str_tmp), st_path_info.strFileBasic);

		str_name.Format(_T("%02d_TESTER_SERVER_NAME"), i);
		str_tmp.Format(_T("%s"), st_server_info[i].strName);
		:: WritePrivateProfileString(_T("INTERFACE_SCREEN"), str_name, LPCTSTR(str_tmp), st_path_info.strFileBasic);
		//////////
		str_name.Format(_T("%02d_SERVER_PORT"), i);
		str_tmp.Format(_T("%d"), st_server_info[i].nPort);
		:: WritePrivateProfileString(_T("INTERFACE_SCREEN"), str_name, LPCTSTR(str_tmp), st_path_info.strFileBasic);
	}

	for(i=0; i<10; i++)
	{
		str_name.Format(_T("PORT_%02d"), i+1);
		str_tmp.Format(_T("%d"), st_serial_info.nSerialPort[i]);
		:: WritePrivateProfileString(_T("SERIAL"), str_name, LPCTSTR(str_tmp), st_path_info.strFileBasic);

		str_name.Format(_T("BAUDRATE_%02d"), i+1);
		str_tmp.Format(_T("%d"), st_serial_info.nSerialBaudrate[i]);
		:: WritePrivateProfileString(_T("SERIAL"), str_name, LPCTSTR(str_tmp), st_path_info.strFileBasic);

		str_name.Format(_T("DATA_%02d"), i+1);
		str_tmp.Format(_T("%d"), st_serial_info.nSerialData[i]);
		:: WritePrivateProfileString(_T("SERIAL"), str_name, LPCTSTR(str_tmp), st_path_info.strFileBasic);

		str_name.Format(_T("STOP_%02d"), i+1);
		str_tmp.Format(_T("%d"), st_serial_info.nSerialStop[i]);
		:: WritePrivateProfileString(_T("SERIAL"), str_name, LPCTSTR(str_tmp), st_path_info.strFileBasic);

		str_name.Format(_T("PARITY_%02d"), i+1);
		str_tmp.Format(_T("%d"), st_serial_info.nSerialParity[i]);
		:: WritePrivateProfileString(_T("SERIAL"), str_name, LPCTSTR(str_tmp), st_path_info.strFileBasic);
	}
}

void CMyBasicData::OnBasic_Data_Save_As(CString strDeviceName)
{
	CString mstr_temp;  // ������ ���� �ӽ� ���� ���� 
	CString str_save_file;
	CString str_part, str_chk_ext;
	CString str_pos;
	COleDateTime time_cur;

	/* ************************************************************************** */
	/*  ������ ������ ���� �����Ѵ� [���� Ȯ���� �˻�]                            */
	/* ************************************************************************** */
	:: WritePrivateProfileString(_T("FILE_NAME"), _T("Device_Type"), LPCWSTR(strDeviceName), st_path_info.strFileBasic);

	str_save_file = st_path_info.strBasic + strDeviceName;  // ƼĪ ������ ���� ���� ����

	mstr_temp.Format(_T("%d"), st_basic_info.nModeDevice);
	:: WritePrivateProfileString(_T("BASIC"), _T("DEVICE_MODE"), LPCWSTR(strDeviceName), str_save_file);
}

void CMyBasicData::OnMaintenance_Data_Save_As(CString strDevice)
{
	CString strTemp, strPart;  // �ε� ���� �ӽ� ���� ����
//	char chBuf[20] ;
	CString strSaveFile;;

	// **************************************************************************
	// Ÿ�� ���� RED ���� ������ ���Ͽ� �����Ѵ�                                 
	// ->  STOP			= 0,
	// 		   RUN			= 1,
	// 		   ALARM		= 2,
	// 		   LOTEND		= 3,
	// 		   INIT			= 4,
	// 		   WARRING		= 5,
	// 		   LOCK			= 6,
	// 		   SELFCHECK	= 7														 
	// -> �ε� �� [0:OFF    1:ON    2:FLICKER]                                   
	// **************************************************************************
//	str_save_file = st_path.str_basic + str_device;  // ƼĪ ������ ���� ���� ����
	strSaveFile = st_path_info.strBasic + _T("Maintenance.TXT");

	strTemp.Format(_T("%d"), st_lamp_info.nLampR[0]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Red"), _T("Stop_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampR[1]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Red"), _T("Run_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampR[2]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Red"), _T("Alarm_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampR[3]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Red"), _T("LotEnd_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampR[4]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Red"), _T("Initial_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampR[5]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Red"), _T("Warring_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampR[6]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Red"), _T("Lock_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampR[7]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Red"), _T("SelfCheck_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampR[8]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Red"), _T("Idle_State"), LPCTSTR(strTemp), strSaveFile);
	// **************************************************************************


	// **************************************************************************
	// Ÿ�� ���� YELLOW ���� ������ ���Ͽ� �����Ѵ�                              
	// ->  STOP			= 0,
// 		   RUN			= 1,
// 		   ALARM		= 2,
// 		   LOTEND		= 3,
// 		   INIT			= 4,
// 		   WARRING		= 5,
// 		   LOCK			= 6,
// 		   SELFCHECK	= 7														 
	// -> �ε� �� [0:OFF    1:ON    2:FLICKER]                                   
	// **************************************************************************
	strTemp.Format(_T("%d"),st_lamp_info.nLampY[0]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Yellow"), _T("Stop_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampY[1]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Yellow"), _T("Run_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampY[2]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Yellow"), _T("Alarm_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampY[3]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Yellow"), _T("LotEnd_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampY[4]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Yellow"), _T("Initial_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampY[5]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Yellow"), _T("Warring_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampY[6]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Yellow"), _T("Lock_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampY[7]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Yellow"), _T("SelfCheck_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampY[8]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Yellow"), _T("Idle_State"), LPCTSTR(strTemp), strSaveFile);

	// **************************************************************************

	// **************************************************************************
	// Ÿ�� ���� GREEN ���� ������ ���Ͽ� �����Ѵ�                               
	// ->  STOP			= 0,
// 		   RUN			= 1,
// 		   ALARM		= 2,
// 		   LOTEND		= 3,
// 		   INIT			= 4,
// 		   WARRING		= 5,
// 		   LOCK			= 6,
// 		   SELFCHECK	= 7														 
	// -> �ε� �� [0:OFF    1:ON    2:FLICKER]                                   
	// **************************************************************************
	strTemp.Format(_T("%d"),st_lamp_info.nLampG[0]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Green"), _T("Stop_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampG[1]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Green"), _T("Run_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampG[2]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Green"), _T("Alarm_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampG[3]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Green"), _T("LotEnd_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampG[4]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Green"), _T("Initial_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampG[5]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Green"), _T("Warring_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampG[6]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Green"), _T("Lock_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampG[7]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Green"), _T("SelfCheck_State"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"),st_lamp_info.nLampG[8]) ;
	:: WritePrivateProfileString(_T("TowerLampData_Green"), _T("Idle_State"), LPCTSTR(strTemp), strSaveFile);
	// **************************************************************************

	// **************************************************************************
	// ���� ��� ��� �ε��Ͽ� ���� ������ ����                                  
	// -> 0:���    1:�̻��                                                     
	// **************************************************************************
	strTemp.Format(_T("%d"),st_lamp_info.nBuzzerMode) ;
	:: WritePrivateProfileString(_T("TowerLampData"), _T("n_buzzer_mode"), LPCTSTR(strTemp), strSaveFile);
	// **************************************************************************

	// **************************************************************************
	// ��� ȣ�� �� ��� �ڵ� �ε��Ͽ� ���� ������ ����                          
	// -> �ε� �������� ��/�� ������ �����Ѵ�                                    
	// **************************************************************************
	:: WritePrivateProfileString(_T("TowerLampData"), _T("str_equip_no"), LPCTSTR(st_lamp_info.strEquipNo), strSaveFile);
	:: WritePrivateProfileString(_T("TowerLampData"), _T("str_equip_code"), LPCTSTR(st_lamp_info.strEquipCode), strSaveFile);
	// **************************************************************************

	// **************************************************************************
	// Ÿ�� ���� ON/OFF ��� �ð� �ε��Ͽ� ���� ������ ����                      
	// **************************************************************************
	strTemp.Format(_T("%d"),st_lamp_info.nLampWaitTime);
	:: WritePrivateProfileString(_T("TowerLampData"), _T("n_lamp_on_time_w"), LPCTSTR(strTemp), strSaveFile);
	// **************************************************************************
}

void CMyBasicData::OnModuleRobot_Teach_Data_Save_As(CString strDeviceName)
{
	CString strTemp, strMsg, strSaveFail;       // �ӽ� ���� ����
	CString strPos;
	CString strHead, strItem;
	int i, j;
	
	//strSaveFail = st_path_info.strFileMotor + st_basic_info.strDeviceName;
	strSaveFail = st_path_info.strFileMotor + strDeviceName;
	for(i=0; i<MAXMOTOR; i++)
	{
		strHead.Format(_T("Motor%d"),i+1);
		for(j=0; j<M_MAX_POS; j++)
		{
			strItem.Format(_T("%02d_Axis_[%02d]"), i+1, j+1); 
			strTemp.Format(_T("%.3f"), st_motor_info[i].d_pos[j]);
			//:: WritePrivateProfileString(strHead, strItem, LPCTSTR(strTemp), strSaveFail);
			if (clsExcel.m_nAxisPosData[i][j] == NO)
			{
				:: WritePrivateProfileString(strHead, strItem, LPCTSTR(strTemp), strSaveFail);
			}
			else
			{
				:: WritePrivateProfileString(strHead, strItem, LPCTSTR(strTemp), st_path_info.strFileBasic);
			}
		}
	}
}





CString CMyBasicData::GetWaitTimeName(int n_mode)
{
	CString strName;

	strName = "";

	switch(n_mode)
	{
		case 0:
			strName = "0";
			break;

		case 1:
			strName = "1";
			break;

		case 2:
			strName = "2";
			break;

		case 3:
			strName = "3";
			break;

		case 4:
			strName = "4";
			break;

		case 5:
			strName = "5";
			break;

		case 6:
			strName = "6";
			break;

		case 7:
			strName = "7";
			break;

		case 8:
			strName = "8";
			break;

		case 9:
			strName = "9";
			break;

		case 10:
			strName = "10";
			break;

		case 11:
			strName = "11";
			break;

		case 12:
			strName = "12";
			break;

		case 13:
			strName = "13";
			break;

		case 14:
			strName = "14";
			break;

		case 15:
			strName = "15";
			break;

		case 16:
			strName = "16";
			break;

		case 17:
			strName = "17";
			break;

		case 18:
			strName = "18";
			break;

		case 19:
			strName = "19";
			break;
	}

	return strName;
}

void CMyBasicData::OnRecipe_Data_Load()
{
	CString strTemp;  // �ε� ���� �ӽ� ���� ����
	CString strHead;
	CString strLoadFile;

	char chData[200];

	int i, j, nChk;
	
	// ���� ��� 
	strLoadFile = st_path_info.strBasic + st_basic_info.strDeviceName;  

	:: GetPrivateProfileString(_T("RECIPE"), _T("nTrayY"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.nTrayY = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("nTrayX"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.nTrayX = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("nRejectTray_X"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.nRejectTray_X = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("nRetestTray_X"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.nRetestTray_X = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("dTrayYOffset"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.dTrayYOffset = _wtof(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("dTrayXOffset"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.dTrayXOffset = _wtof(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("dBufferPitch_Y"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.dBufferPitch_Y = _wtof(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("dBufferPitch_X"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.dBufferPitch_X = _wtof(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("dTSiteOffsetX"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.dTSiteOffsetX = _wtof(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("dCOKBuffPitch_X"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.dCOKBuffPitch_X = _wtof(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("dRejTrayPick"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.dRejTrayPick = _wtof(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("dRejTrayPlace"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.dRejTrayPlace = _wtof(strTemp);



	:: GetPrivateProfileString(_T("RECIPE"), _T("nCOKBuff_IDNum_Type"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.nHifix = st_recipe_info.nCOKBuff_IDNum_Type = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("strHifix"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.strHifix = strTemp;

	:: GetPrivateProfileString(_T("RECIPE"), _T("Buffer_DvcCheckSensor_Mode"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.Buffer_DvcCheckSensor_Mode = _wtoi(strTemp);
/*
	for (i=0; i<5; i++)
	{
		strHead.Format(_T("Hifix_%02d_Name"), i);
		:: GetPrivateProfileString(_T("RECIPE"), strHead, _T(""), (LPWSTR)chData, 30, strLoadFile);
		strTemp.Format(_T("%s"), chData);
		st_recipe_info.strHifix[i] = strTemp;
	}*/
	//kwlee 2016.0809
// 	:: GetPrivateProfileString(_T("RECIPE"), _T("nBufferRotator_PlateSite_Degrees"), _T(""), (LPWSTR)chData, 30, strLoadFile);
// 	strTemp.Format(_T("%s"), chData);
// 	st_recipe_info.nBufferRotator_PlateSite_Degrees = _wtoi(strTemp);
// 
// 	:: GetPrivateProfileString(_T("RECIPE"), _T("nBufferRotator_TestSite_Degrees"), _T(""), (LPWSTR)chData, 30, strLoadFile);
// 	strTemp.Format(_T("%s"), chData);
// 	st_recipe_info.nBufferRotator_TestSite_Degrees = _wtoi(strTemp);
// 
// 	// jtkim 20151017
// 	:: GetPrivateProfileString(_T("RECIPE"), _T("nBufferRotator_Barcode_Degrees"), _T(""), (LPWSTR)chData, 30, strLoadFile);
// 	strTemp.Format(_T("%s"), chData);
// 	st_recipe_info.nBufferRotator_Barcode_Degrees = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("nTestSite_Top_Degrees"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.nTestSite_Top_Degrees = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("nTestSite_Btm_Degrees"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.nTestSite_Btm_Degrees = _wtoi(strTemp);


	
	//kwlee 2016.0817
	for (i=0; i<8; i++)
	{
		for (j=0; j<8; j++)
		{
			strHead.Format(_T("Tsite_%02d_%02d_use"), i+ 1, j + 1);
			:: GetPrivateProfileString(_T("RECIPE"), strHead, _T(""), (LPWSTR)chData, 30, strLoadFile);
			strTemp.Format(_T("%s"), chData);
			//st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].nEnable = _wtoi(strTemp);
			//kwlee 2017.0125
			int nTemp = _wtoi(strTemp);
			if(nTemp <=0)
			{
				st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].nEnable = 0;
			}
			else
			{
				st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].nEnable = _wtoi(strTemp);
			}
			//
		}
	}
	//////
	
	//kwlee 2016.0823
	:: GetPrivateProfileString(_T("RECIPE"), _T("nTestSocket_Num"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.nTestSocket_Num = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("nRetestBuffer_Num"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.nRetestBuffer_Num = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("nLdUldBuffer_Num"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.nLdUldBuffer_Num = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("dLdUldPickerPitch"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.dLdUldPickerPitch = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("dTestPickerPitch"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.dTestPickerPitch = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("dLdUldBufferPitch"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.dLdUldBufferPitch = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("dRetestBufferPitch"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.dRetestBufferPitch = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("dTrayPitch_Y"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.dTrayPitch_Y = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("dTrayPitch_X"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.dTrayPitch_X = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("dVisionXOffset"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.dVisionXOffset = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("dVisionYOffset"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.dVisionYOffset = _wtoi(strTemp);
	//////////////////////////////////////////////////

	:: GetPrivateProfileString(_T("RECIPE"), _T("nAbortTime"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.nAbortTime = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("nBcrBank"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.nBcrBank = _wtoi(strTemp);

	//kwlee 2016.0901
	:: GetPrivateProfileString(_T("RECIPE"), _T("nBarcodeCnt"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.nBarcodeCnt = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("bUnldFlagA"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk>=0 && nChk<2)
	{
		st_recipe_info.bUnldBcrFlag[0]	=	nChk;
	}
	else
	{
		st_recipe_info.bUnldBcrFlag[0]	=	FALSE;
	}

	:: GetPrivateProfileString(_T("RECIPE"), _T("bUnldFlagB"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk>=0 && nChk<2)
	{
		st_recipe_info.bUnldBcrFlag[1]	=	nChk;
	}
	else
	{
		st_recipe_info.bUnldBcrFlag[1]	=	FALSE;
	}

	

	for (i=0; i<4; i++)
	{
		strHead.Format(_T("COK_BUFF_#%d"), i+1);

		::GetPrivateProfileString(_T("RECIPE"), strHead, _T("-1"), (LPWSTR)chData, 30, strLoadFile);
		strTemp.Format(_T("%s"), chData);
		nChk = _wtoi(strTemp);

		if (nChk > 0 && nChk < 9)
		{
			st_recipe_info.nCokType[i]	=	nChk;
		}
		else
		{
			st_recipe_info.nCokType[i]	=	0;
		}
	}

	for (i=0; i<4; i++)
	{
		strHead.Format(_T("COK_TYPE_#%d_NAME"), i+1);

		::GetPrivateProfileString(_T("RECIPE"), strHead, _T(""), (LPWSTR)chData, 30, strLoadFile);
		strTemp.Format(_T("%s"), chData);
		st_recipe_info.strCokTypeName[i]	=	strTemp;

		for (j=0; j<3; j++)
		{
			strHead.Format(_T("COK_TYPE_#%d_CHECK_#%d"), i+1, j+1);

			::GetPrivateProfileString(_T("RECIPE"), strHead, _T("false"), (LPWSTR)chData, 30, strLoadFile);
			strTemp.Format(_T("%s"), chData);

			if (strTemp == _T("true")) st_recipe_info.bCokChk[i][j]	=	true;
			else	st_recipe_info.bCokChk[i][j]	=	false;
		}
	}

	:: GetPrivateProfileString(_T("RECIPE"), _T("strPartNo"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.strPartNo = strTemp;

/*
	for (i=0; i<2; i++)		//SOCKET COMMENT 2015.08.04
	{
		for (j=0; j<12; j++)
		{
			strHead.Format(_T("#%02d SOCKET_COMMENT"), (i*12) + (j+1));

			:: GetPrivateProfileString(_T("RECIPE"), strHead, _T(""), (LPWSTR)chData, 200, strLoadFile);

			st_bd_info[0][i].strSocketComment[j].Format(_T("%s"), chData);
		}
	}

	for (i=0; i<2; i++)
	{
		for (j=0; j<12; j++)
		{
			strHead.Format(_T("#%02d SOCKET_COMMENT"), (((i*12)  + (j+1)) + 24));

			:: GetPrivateProfileString(_T("RECIPE"), strHead, _T(""), (LPWSTR)chData, 200, strLoadFile);

			st_bd_info[1][i].strSocketComment[j].Format(_T("%s"), chData);
		}
	}
*/
	:: GetPrivateProfileString(_T("RECIPE"), _T("nIndividualPick"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.nIndividualPick = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("d15TOffset"), _T("0"), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.d15TOffset = _wtoi(strTemp);
}


void CMyBasicData::OnRecipe_Data_Load(CString strDeviceName)
{
	CString strTemp;  // �ε� ���� �ӽ� ���� ����
	CString strLoadFile;
	CString strHead;

	char chData[200];

	int i, j, nChk;
	
	// ���� ���  
	strLoadFile = st_path_info.strBasic + strDeviceName; 

	:: GetPrivateProfileString(_T("RECIPE"), _T("nTrayY"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.nTrayY = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("nTrayX"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.nTrayX = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("nRejectTray_X"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.nRejectTray_X = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("nRetestTray_X"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.nRetestTray_X = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("dTrayYOffset"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.dTrayYOffset = _wtof(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("dTrayXOffset"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.dTrayXOffset = _wtof(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("dBufferPitch_Y"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.dBufferPitch_Y = _wtof(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("dBufferPitch_X"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.dBufferPitch_X = _wtof(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("dTSiteOffsetX"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.dTSiteOffsetX = _wtof(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("dCOKBuffPitch_X"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.dCOKBuffPitch_X = _wtof(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("dRejTrayPick"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.dRejTrayPick = _wtof(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("dRejTrayPlace"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.dRejTrayPlace = _wtof(strTemp);
/*
	for (i=0; i<2; i++)
	{
		for (j=0; j<2; j++)
		{
			for (k=0; k<12; k++)
			{
				strHead.Format(_T("Tsite_%02d_%02d_%02d_use"), i, j, k);
				:: GetPrivateProfileString(_T("RECIPE"), strHead, _T(""), (LPWSTR)chData, 30, strLoadFile);
				strTemp.Format(_T("%s"), chData);
				st_recipe_info.nTsiteUse[i][j][k] = _wtoi(strTemp);
			}
		}
	}
*/

	//kwlee 2016.0823
	:: GetPrivateProfileString(_T("RECIPE"), _T("nTestSocket_Num"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.nTestSocket_Num = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("nRetestBuffer_Num"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.nRetestBuffer_Num = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("nLdUldBuffer_Num"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.nLdUldBuffer_Num = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("dLdUldPickerPitch"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.dLdUldPickerPitch = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("dTestPickerPitch"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.dTestPickerPitch = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("dLdUldBufferPitch"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.dLdUldBufferPitch = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("dRetestBufferPitch"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.dRetestBufferPitch = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("dTrayPitch_Y"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.dTrayPitch_Y = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("nSocketCdTrayPitch_Xount"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.dTrayPitch_X = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("dVisionXOffset"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.dVisionXOffset = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("dVisionYOffset"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.dVisionYOffset = _wtoi(strTemp);
	//////////////////////////////////////////////////
	:: GetPrivateProfileString(_T("RECIPE"), _T("nCOKBuff_IDNum_Type"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.nHifix = st_recipe_info.nCOKBuff_IDNum_Type = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("strHifix"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.strHifix = strTemp;

	:: GetPrivateProfileString(_T("RECIPE"), _T("Buffer_DvcCheckSensor_Mode"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.Buffer_DvcCheckSensor_Mode = _wtoi(strTemp);
/*
	for (i=0; i<5; i++)
	{
		strHead.Format(_T("Hifix_%02d_Name"), i);
		:: GetPrivateProfileString(_T("RECIPE"), strHead, _T(""), (LPWSTR)chData, 30, strLoadFile);
		strTemp.Format(_T("%s"), chData);
		st_recipe_info.strHifix[i] = strTemp;
	}
*/
	//kwlee 2016.0809
// 	:: GetPrivateProfileString(_T("RECIPE"), _T("nBufferRotator_PlateSite_Degrees"), _T(""), (LPWSTR)chData, 30, strLoadFile);
// 	strTemp.Format(_T("%s"), chData);
// 	st_recipe_info.nBufferRotator_PlateSite_Degrees = _wtoi(strTemp);
// 
// 	:: GetPrivateProfileString(_T("RECIPE"), _T("nBufferRotator_TestSite_Degrees"), _T(""), (LPWSTR)chData, 30, strLoadFile);
// 	strTemp.Format(_T("%s"), chData);
// 	st_recipe_info.nBufferRotator_TestSite_Degrees = _wtoi(strTemp);
// 
// 	// jtkim 20151017
// 	:: GetPrivateProfileString(_T("RECIPE"), _T("nBufferRotator_Barcode_Degrees"), _T(""), (LPWSTR)chData, 30, strLoadFile);
// 	strTemp.Format(_T("%s"), chData);
// 	st_recipe_info.nBufferRotator_Barcode_Degrees = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("nTestSite_Top_Degrees"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.nTestSite_Top_Degrees = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("nTestSite_Btm_Degrees"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.nTestSite_Btm_Degrees = _wtoi(strTemp);

	///////

	:: GetPrivateProfileString(_T("RECIPE"), _T("nAbortTime"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.nAbortTime = _wtoi(strTemp);

	//kwlee 2016.0901
	:: GetPrivateProfileString(_T("RECIPE"), _T("nBarcodeCnt"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.nBarcodeCnt = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("nBcrBank"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.nBcrBank = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("bUnldFlagA"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk>=0 && nChk<2)
	{
		st_recipe_info.bUnldBcrFlag[0]	=	nChk;
	}
	else
	{
		st_recipe_info.bUnldBcrFlag[0]	=	FALSE;
	}

	:: GetPrivateProfileString(_T("RECIPE"), _T("bUnldFlagB"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nChk = _wtoi(strTemp);
	if (nChk>=0 && nChk<2)
	{
		st_recipe_info.bUnldBcrFlag[1]	=	nChk;
	}
	else
	{
		st_recipe_info.bUnldBcrFlag[1]	=	FALSE;
	}

	for (i=0; i<4; i++)
	{
		strHead.Format(_T("COK_BUFF_#%d"), i+1);

		::GetPrivateProfileString(_T("RECIPE"), strHead, _T("-1"), (LPWSTR)chData, 30, strLoadFile);
		strTemp.Format(_T("%s"), chData);
		nChk = _wtoi(strTemp);

		if (nChk > 0 && nChk < 9)
		{
			st_recipe_info.nCokType[i]	=	nChk;
		}
		else
		{
			st_recipe_info.nCokType[i]	=	0;
		}
	}

	for (i=0; i<4; i++)
	{
		strHead.Format(_T("COK_TYPE_#%d_NAME"), i+1);

		::GetPrivateProfileString(_T("RECIPE"), strHead, _T(""), (LPWSTR)chData, 30, strLoadFile);
		strTemp.Format(_T("%s"), chData);
		st_recipe_info.strCokTypeName[i]	=	strTemp;

		for (j=0; j<3; j++)
		{
			strHead.Format(_T("COK_TYPE_#%d_CHECK_#%d"), i+1, j+1);

			::GetPrivateProfileString(_T("RECIPE"), strHead, _T("false"), (LPWSTR)chData, 30, strLoadFile);
			strTemp.Format(_T("%s"), chData);

			if (strTemp == _T("true")) st_recipe_info.bCokChk[i][j]	=	true;
			else	st_recipe_info.bCokChk[i][j]	=	false;
		}
	}

	:: GetPrivateProfileString(_T("RECIPE"), _T("strPartNo"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.strPartNo = strTemp;

	for (i=0; i<2; i++)		//SOCKET COMMENT 2015.08.04
	{
		for (j=0; j<12; j++)
		{
			strHead.Format(_T("#%02d SOCKET_COMMENT"), (((i+1)*12) + (j+1)));

			:: GetPrivateProfileString(_T("RECIPE"), strHead, _T(""), (LPWSTR)chData, 200, strLoadFile);

			st_bd_info[0][i].strSocketComment[j].Format(_T("%s"), chData);
		}
	}
	for (i=0; i<2; i++)
	{
		for (j=0; j<12; j++)
		{
			strHead.Format(_T("#%02d SOCKET_COMMENT"), (((i+1)*12  + (j+1)) + 24));

			:: GetPrivateProfileString(_T("RECIPE"), strHead, _T(""), (LPWSTR)chData, 200, strLoadFile);

			st_bd_info[1][i].strSocketComment[j].Format(_T("%s"), chData);
		}
	}

	:: GetPrivateProfileString(_T("RECIPE"), _T("nIndividualPick"), _T(""), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.nIndividualPick = _wtoi(strTemp);

	:: GetPrivateProfileString(_T("RECIPE"), _T("d15TOffset"), _T("0"), (LPWSTR)chData, 30, strLoadFile);
	strTemp.Format(_T("%s"), chData);
	st_recipe_info.d15TOffset = _wtoi(strTemp);
}


void CMyBasicData::OnRecipe_Data_Save()
{
	CString strTemp;  // ������ ���� �ӽ� ���� ���� 
	CString strSaveFile;
	CString strPart, strChkExt;
	CString strPos;
	CString strHead;

	int i, j;
	COleDateTime time_cur;

	strSaveFile = st_path_info.strBasic + st_basic_info.strDeviceName;  // ƼĪ ������ ���� ���� ����

	strTemp.Format(_T("%d"), st_recipe_info.nTrayY);
	:: WritePrivateProfileString(_T("RECIPE"), _T("nTrayY"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"), st_recipe_info.nTrayX);
	:: WritePrivateProfileString(_T("RECIPE"), _T("nTrayX"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"), st_recipe_info.nRejectTray_X);
	:: WritePrivateProfileString(_T("RECIPE"), _T("nRejectTray_X"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"), st_recipe_info.nRetestTray_X);
	:: WritePrivateProfileString(_T("RECIPE"), _T("nRetestTray_X"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.dTrayYOffset);
	:: WritePrivateProfileString(_T("RECIPE"), _T("dTrayYOffset"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.dTrayXOffset);
	:: WritePrivateProfileString(_T("RECIPE"), _T("dTrayXOffset"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.dBufferPitch_Y);
	:: WritePrivateProfileString(_T("RECIPE"), _T("dBufferPitch_Y"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.dBufferPitch_X);
	:: WritePrivateProfileString(_T("RECIPE"), _T("dBufferPitch_X"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.dRejTrayPick);
	:: WritePrivateProfileString(_T("RECIPE"), _T("dRejTrayPick"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.dRejTrayPlace);
	:: WritePrivateProfileString(_T("RECIPE"), _T("dRejTrayPlace"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.dTSiteOffsetX);
	:: WritePrivateProfileString(_T("RECIPE"), _T("dTSiteOffsetX"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.dCOKBuffPitch_X);
	:: WritePrivateProfileString(_T("RECIPE"), _T("dCOKBuffPitch_X"), LPCTSTR(strTemp), strSaveFile);

	//kwlee 2016.0823
	strTemp.Format(_T("%d"), st_recipe_info.nTestSocket_Num);
	:: WritePrivateProfileString(_T("RECIPE"), _T("nTestSocket_Num"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"), st_recipe_info.nRetestBuffer_Num);
	:: WritePrivateProfileString(_T("RECIPE"), _T("nRetestBuffer_Num"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"), st_recipe_info.nLdUldBuffer_Num);
	:: WritePrivateProfileString(_T("RECIPE"), _T("nLdUldBuffer_Num"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.dLdUldPickerPitch);
	:: WritePrivateProfileString(_T("RECIPE"), _T("dLdUldPickerPitch"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.dTestPickerPitch);
	:: WritePrivateProfileString(_T("RECIPE"), _T("dTestPickerPitch"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.dLdUldBufferPitch);
	:: WritePrivateProfileString(_T("RECIPE"), _T("dLdUldBufferPitch"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.dRetestBufferPitch);
	:: WritePrivateProfileString(_T("RECIPE"), _T("dRetestBufferPitch"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.dTrayPitch_Y);
	:: WritePrivateProfileString(_T("RECIPE"), _T("dTrayPitch_Y"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.dTrayPitch_X);
	:: WritePrivateProfileString(_T("RECIPE"), _T("dTrayPitch_X"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.dVisionXOffset);
	:: WritePrivateProfileString(_T("RECIPE"), _T("dVisionXOffset"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.dVisionYOffset);
	:: WritePrivateProfileString(_T("RECIPE"), _T("dVisionYOffset"), LPCTSTR(strTemp), strSaveFile);
	//////////////

	strTemp.Format(_T("%d"), st_recipe_info.nCOKBuff_IDNum_Type);
	:: WritePrivateProfileString(_T("RECIPE"), _T("nCOKBuff_IDNum_Type"), LPCTSTR(strTemp), strSaveFile);

//	:: WritePrivateProfileString(_T("RECIPE"), _T("nCOKBuff_IDNum_Type"), LPCTSTR(st_recipe_info.strHifix), strSaveFile);

	strTemp.Format(_T("%d"), st_recipe_info.Buffer_DvcCheckSensor_Mode);
	:: WritePrivateProfileString(_T("RECIPE"), _T("Buffer_DvcCheckSensor_Mode"), LPCTSTR(strTemp), strSaveFile);
/*
	for (i=0; i<5; i++)
	{
		strHead.Format(_T("Hifix_%02d_Name"), i);
		:: WritePrivateProfileString(_T("RECIPE"), strHead, LPCTSTR(st_recipe_info.strHifix[i]), strSaveFile);
	}
*/
	//kwlee 2016.0809
// 	strTemp.Format(_T("%d"), st_recipe_info.nBufferRotator_PlateSite_Degrees);
// 	:: WritePrivateProfileString(_T("RECIPE"), _T("nBufferRotator_PlateSite_Degrees"), LPCTSTR(strTemp), strSaveFile);
// 
// 	strTemp.Format(_T("%d"), st_recipe_info.nBufferRotator_TestSite_Degrees);
// 	:: WritePrivateProfileString(_T("RECIPE"), _T("nBufferRotator_TestSite_Degrees"), LPCTSTR(strTemp), strSaveFile);
// 
// 	// jtkim 20151017
// 	strTemp.Format(_T("%d"), st_recipe_info.nBufferRotator_Barcode_Degrees);
// 	:: WritePrivateProfileString(_T("RECIPE"), _T("nBufferRotator_Barcode_Degrees"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"), st_recipe_info.nTestSite_Top_Degrees);
	:: WritePrivateProfileString(_T("RECIPE"), _T("nTestSite_Top_Degrees"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"), st_recipe_info.nTestSite_Btm_Degrees);
	:: WritePrivateProfileString(_T("RECIPE"), _T("nBufferRotator_TestSite_Degrees"), LPCTSTR(strTemp), strSaveFile);

	
	for (i=0; i<8; i++)
	{
		for (j=0; j<8; j++)
		{
			strHead.Format(_T("Tsite_%02d_%02d_use"), i + 1, j + 1);
			strTemp.Format(_T("%d"), st_test_site_info[THD_TESTSITE_1 + i].st_pcb_info[j].nEnable);
			:: WritePrivateProfileString(_T("RECIPE"), strHead, LPCTSTR(strTemp), strSaveFile);	
		}
	}


	//////////
	//kwlee 2016.0901
	strTemp.Format(_T("%d"), st_recipe_info.nBarcodeCnt);
	:: WritePrivateProfileString(_T("RECIPE"), _T("nBarcodeCnt"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"), st_recipe_info.nAbortTime);
	:: WritePrivateProfileString(_T("RECIPE"), _T("nAbortTime"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"), st_recipe_info.bUnldBcrFlag[0]);
	:: WritePrivateProfileString(_T("RECIPE"), _T("bUnldFlagA"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"), st_recipe_info.bUnldBcrFlag[1]);
	:: WritePrivateProfileString(_T("RECIPE"), _T("bUnldFlagB"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"), st_recipe_info.nBcrBank);
	:: WritePrivateProfileString(_T("RECIPE"), _T("nBcrBank"), LPCTSTR(strTemp), strSaveFile);

	for (i=0; i<4; i++)
	{
		strHead.Format(_T("COK_BUFF_#%d"), i+1);
		strTemp.Format(_T("%d"), st_recipe_info.nCokType[i]);
		:: WritePrivateProfileString(_T("RECIPE"), strHead, LPCTSTR(strTemp), strSaveFile);
	}

	for (i=0; i<4; i++)
	{
		strHead.Format(_T("COK_TYPE_#%d_NAME"), i+1);
		:: WritePrivateProfileString(_T("RECIPE"), strHead, LPCTSTR(st_recipe_info.strCokTypeName[i]), strSaveFile);

		for (j=0; j<3; j++)
		{
			strHead.Format(_T("COK_TYPE_#%d_CHECK_#%d"), i+1, j+1);

			strTemp = _T("false");
			if (st_recipe_info.bCokChk[i][j]) strTemp = _T("true");

			:: WritePrivateProfileString(_T("RECIPE"), strHead, LPCTSTR(strTemp), strSaveFile);
		}
	}

	:: WritePrivateProfileString(_T("RECIPE"), _T("strPartNo"), LPCTSTR(st_recipe_info.strPartNo), strSaveFile);
/*
	for (i=0; i<2; i++)		//SOCKET COMMENT 2015.08.04
	{
		for (j=0; j<12; j++)
		{
			strHead.Format(_T("#%02d SOCKET_COMMENT"), ((i*12) + (j+1)));

			strTemp = st_bd_info[0][i].strSocketComment[j];

			::WritePrivateProfileString(_T("RECIPE"), strHead, LPCTSTR(strTemp), strSaveFile);
		}
	}
	for (i=0; i<2; i++)
	{
		for (j=0; j<12; j++)
		{
			strHead.Format(_T("#%02d SOCKET_COMMENT"), (((i*12) + (j+1)) + 24));

			strTemp = st_bd_info[1][i].strSocketComment[j];

			::WritePrivateProfileString(_T("RECIPE"), strHead, LPCTSTR(strTemp), strSaveFile);
		}
	}
*/
	strTemp.Format(_T("%d"), st_recipe_info.nIndividualPick);
	:: WritePrivateProfileString(_T("RECIPE"), _T("nIndividualPick"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.d15TOffset);
	:: WritePrivateProfileString(_T("RECIPE"), _T("d15TOffset"), LPCTSTR(strTemp), strSaveFile);
}

void CMyBasicData::OnRecipe_Data_Save_As(CString strDeviceName)
{
	CString strTemp;  // ������ ���� �ӽ� ���� ���� 
	CString strSaveFile;
	CString strPart, strChkExt;
	CString strPos;
	CString strHead;

	int i, j;

	COleDateTime time_cur;

	strSaveFile = st_path_info.strBasic + strDeviceName;  // ƼĪ ������ ���� ���� ����

	strTemp.Format(_T("%d"), st_recipe_info.nTrayY);
	:: WritePrivateProfileString(_T("RECIPE"), _T("nTrayY"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"), st_recipe_info.nTrayX);
	:: WritePrivateProfileString(_T("RECIPE"), _T("nTrayX"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"), st_recipe_info.nRejectTray_X);
	:: WritePrivateProfileString(_T("RECIPE"), _T("nRejectTray_X"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"), st_recipe_info.nRetestTray_X);
	:: WritePrivateProfileString(_T("RECIPE"), _T("nRetestTray_X"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.dTrayYOffset);
	:: WritePrivateProfileString(_T("RECIPE"), _T("dTrayYOffset"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.dTrayXOffset);
	:: WritePrivateProfileString(_T("RECIPE"), _T("dTrayXOffset"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.dBufferPitch_Y);
	:: WritePrivateProfileString(_T("RECIPE"), _T("dBufferPitch_Y"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.dBufferPitch_X);
	:: WritePrivateProfileString(_T("RECIPE"), _T("dBufferPitch_X"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.dRejTrayPick);
	:: WritePrivateProfileString(_T("RECIPE"), _T("dRejTrayPick"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.dRejTrayPlace);
	:: WritePrivateProfileString(_T("RECIPE"), _T("dRejTrayPlace"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.dTSiteOffsetX);
	:: WritePrivateProfileString(_T("RECIPE"), _T("dTSiteOffsetX"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.dCOKBuffPitch_X);
	:: WritePrivateProfileString(_T("RECIPE"), _T("dCOKBuffPitch_X"), LPCTSTR(strTemp), strSaveFile);
/*
	for (i=0; i<2; i++)
	{
		for (j=0; j<2; j++)
		{
			for (k=0; k<12; k++)
			{
				strHead.Format(_T("Tsite_%02d_%02d_%02d_use"), i, j, k);
				strTemp.Format(_T("%d"), st_recipe_info.nTsiteUse[i][j][k]);
				:: WritePrivateProfileString(_T("RECIPE"), strHead, LPCTSTR(strTemp), strSaveFile);
			}
		}
	}
*/


	strTemp.Format(_T("%d"), st_recipe_info.nCOKBuff_IDNum_Type);
	:: WritePrivateProfileString(_T("RECIPE"), _T("nCOKBuff_IDNum_Type"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"), st_recipe_info.Buffer_DvcCheckSensor_Mode);
	:: WritePrivateProfileString(_T("RECIPE"), _T("Buffer_DvcCheckSensor_Mode"), LPCTSTR(strTemp), strSaveFile);
/*
	for (i=0; i<5; i++)
	{
		strHead.Format(_T("Hifix_%02d_Name"), i);
		:: WritePrivateProfileString(_T("RECIPE"), strHead, LPCTSTR(st_recipe_info.strHifix[i]), strSaveFile);
	}
*/
	//kwlee 2016.0809
// 	strTemp.Format(_T("%d"), st_recipe_info.nBufferRotator_PlateSite_Degrees);
// 	:: WritePrivateProfileString(_T("RECIPE"), _T("nBufferRotator_PlateSite_Degrees"), LPCTSTR(strTemp), strSaveFile);
// 
// 	strTemp.Format(_T("%d"), st_recipe_info.nBufferRotator_TestSite_Degrees);
// 	:: WritePrivateProfileString(_T("RECIPE"), _T("nBufferRotator_TestSite_Degrees"), LPCTSTR(strTemp), strSaveFile);
// 
// 	// jtkim 20151017
// 	strTemp.Format(_T("%d"), st_recipe_info.nBufferRotator_Barcode_Degrees);
// 	:: WritePrivateProfileString(_T("RECIPE"), _T("nBufferRotator_Barcode_Degrees"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"), st_recipe_info.nTestSite_Top_Degrees);
	:: WritePrivateProfileString(_T("RECIPE"), _T("nTestSite_Top_Degrees"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"), st_recipe_info.nTestSite_Btm_Degrees);
	:: WritePrivateProfileString(_T("RECIPE"), _T("nBufferRotator_TestSite_Degrees"), LPCTSTR(strTemp), strSaveFile);

	//////////////////

	//kwlee 2016.0823
	strTemp.Format(_T("%d"), st_recipe_info.nTestSocket_Num);
	:: WritePrivateProfileString(_T("RECIPE"), _T("nTestSocket_Num"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"), st_recipe_info.nRetestBuffer_Num);
	:: WritePrivateProfileString(_T("RECIPE"), _T("nRetestBuffer_Num"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"), st_recipe_info.nLdUldBuffer_Num);
	:: WritePrivateProfileString(_T("RECIPE"), _T("nLdUldBuffer_Num"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.dLdUldPickerPitch);
	:: WritePrivateProfileString(_T("RECIPE"), _T("dLdUldPickerPitch"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.dTestPickerPitch);
	:: WritePrivateProfileString(_T("RECIPE"), _T("dTestPickerPitch"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.dLdUldBufferPitch);
	:: WritePrivateProfileString(_T("RECIPE"), _T("dLdUldBufferPitch"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.dRetestBufferPitch);
	:: WritePrivateProfileString(_T("RECIPE"), _T("dRetestBufferPitch"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.dTrayPitch_Y);
	:: WritePrivateProfileString(_T("RECIPE"), _T("dTrayPitch_Y"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.dTrayPitch_X);
	:: WritePrivateProfileString(_T("RECIPE"), _T("dTrayPitch_X"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.dVisionXOffset);
	:: WritePrivateProfileString(_T("RECIPE"), _T("dVisionXOffset"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.dVisionYOffset);
	:: WritePrivateProfileString(_T("RECIPE"), _T("dVisionYOffset"), LPCTSTR(strTemp), strSaveFile);
	//////////////
	//kwlee 2016.0901
	strTemp.Format(_T("%d"), st_recipe_info.nBarcodeCnt);
	:: WritePrivateProfileString(_T("RECIPE"), _T("nBarcodeCnt"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"), st_recipe_info.nAbortTime);
	:: WritePrivateProfileString(_T("RECIPE"), _T("nAbortTime"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"), st_recipe_info.bUnldBcrFlag[0]);
	:: WritePrivateProfileString(_T("RECIPE"), _T("bUnldFlagA"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"), st_recipe_info.bUnldBcrFlag[1]);
	:: WritePrivateProfileString(_T("RECIPE"), _T("bUnldFlagB"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%d"), st_recipe_info.nBcrBank);
	:: WritePrivateProfileString(_T("RECIPE"), _T("nBcrBank"), LPCTSTR(strTemp), strSaveFile);

	for (i=0; i<4; i++)
	{
		strHead.Format(_T("COK_BUFF_#%d"), i+1);
		strTemp.Format(_T("%d"), st_recipe_info.nCokType[i]);
		:: WritePrivateProfileString(_T("RECIPE"), strHead, LPCTSTR(strTemp), strSaveFile);
	}

	for (i=0; i<4; i++)
	{
		strHead.Format(_T("COK_TYPE_#%d_NAME"), i+1);
		:: WritePrivateProfileString(_T("RECIPE"), strHead, LPCTSTR(st_recipe_info.strCokTypeName[i]), strSaveFile);

		for (j=0; j<3; j++)
		{
			strHead.Format(_T("COK_TYPE_#%d_CHECK_#%d"), i+1, j+1);

			strTemp = _T("false");
			if (st_recipe_info.bCokChk[i][j]) strTemp = _T("true");

			:: WritePrivateProfileString(_T("RECIPE"), strHead, LPCTSTR(strTemp), strSaveFile);
		}
	}

	:: WritePrivateProfileString(_T("RECIPE"), _T("strPartNo"), LPCTSTR(st_recipe_info.strPartNo), strSaveFile);
/*
	for (i=0; i<2; i++)		//SOCKET COMMENT 2015.08.04
	{
		for (j=0; j<12; j++)
		{
			strHead.Format(_T("#%02d SOCKET_COMMENT"), ((i * 12) + (j+1)));

			strTemp = st_bd_info[0][i].strSocketComment[j];

			::WritePrivateProfileString(_T("RECIPE"), strHead, LPCTSTR(strTemp), strSaveFile);
		}
	}
	for (i=0; i<2; i++)
	{
		for (j=0; j<12; j++)
		{
			strHead.Format(_T("#%02d SOCKET_COMMENT"), ((i * 12) + (j+1) + 24));

			strTemp = st_bd_info[1][i].strSocketComment[j];

			::WritePrivateProfileString(_T("RECIPE"), strHead, LPCTSTR(strTemp), strSaveFile);
		}
	}
*/
	strTemp.Format(_T("%d"), st_recipe_info.nIndividualPick);
	:: WritePrivateProfileString(_T("RECIPE"), _T("nIndividualPick"), LPCTSTR(strTemp), strSaveFile);

	strTemp.Format(_T("%.3f"), st_recipe_info.d15TOffset);
	:: WritePrivateProfileString(_T("RECIPE"), _T("d15TOffset"), LPCTSTR(strTemp), strSaveFile);
}

void CMyBasicData::OnRecoveryDataLoad()
{

}


void CMyBasicData::OnRecoveryDataSave()
{
	
}


void CMyBasicData::OnAnimateDataLoad()
{
	CString str_load_device;	// �ε� ����̽��� ���� ����
	CString str_load_file;
	CString str_temp, stemp;	// ������ ���� �ӽ� ���� ���� 
	CString str_pos;
	TCHAR chr_data[50], chr_buf[20];
	int nVal;
	int i;
	double dVal;

	memset(&chr_data, 0, sizeof(chr_data));
	memset(&chr_buf, 0, sizeof(chr_buf));

	// ���� ���ϸ� ��������
	if(st_basic_info.strDeviceName == "")
	{
		GetPrivateProfileString(_T("FILE_NAME"), _T("Device_Type"), _T("DEFAULT.TXT"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		st_basic_info.strDeviceName = (LPCTSTR)chr_data;
	}
	// ���� ��� 
	str_load_file = st_path_info.strBasic + st_basic_info.strDeviceName;  // ƼĪ ������ ���� ���� ����

	for (i=0; i<20; i++)
	{
		str_pos.Format(_T("MAX_POS_%02d"), i);
		GetPrivateProfileString(_T("ANIMATE_MAX"), str_pos, _T("1"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), chr_data);
		dVal = _wtof(str_temp);
		if (dVal < 0.0)
		{
			st_animate_info.dMaxPos[i] = 0.0;
		}
		else
		{
			st_animate_info.dMaxPos[i] = dVal;
		}

		str_pos.Format(_T("LENGTH_POS_%02d"), i);
		GetPrivateProfileString(_T("ANIMATE_LENGTH"), str_pos, _T("1"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		str_temp.Format(_T("%s"), chr_data);
		nVal = _wtoi(str_temp);
		if (nVal < 0.0)
		{
			st_animate_info.nDistance[i] = 0;
		}
		else
		{
			st_animate_info.nDistance[i] = nVal;
		}
	}
}
//kwlee 2015.1116
void CMyBasicData::OnFillratio_Data_Load()
{
	CString str_load_device;	// �ε� ����̽��� ���� ����
	CString str_load_pgm;		// �ε� ����̽��� ���� ����
	CString str_load_file;
	CString str_temp, stemp;	// ������ ���� �ӽ� ���� ���� 

	TCHAR chr_data[50], chr_buf[20];
	//TCHAR chData[50];
	int mn_chk, j, k;
	//double md_chk;

	memset(&chr_data, 0, sizeof(chr_data));
	memset(&chr_buf, 0, sizeof(chr_buf));

	//2015.1020
	GetPrivateProfileString(_T("TIME"), _T("Daily_Ref_Time"), _T("0"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
	//mn_chk = _wtoi(chr_data);
	str_temp.Format(_T("%s"), chr_data);
	mn_chk = _wtoi(str_temp);

	if (mn_chk < 0)
	{
		st_handler_info.m_tDRef = 0;
	}
	else  st_handler_info.m_tDRef = mn_chk;

	GetPrivateProfileString(_T("TIME"), _T("RUN_TIME"), _T("1"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
	str_temp.Format(_T("%s"), chr_data);
	mn_chk = _wtoi(str_temp);
	if (mn_chk < 0)
	{
		st_handler_info.tRun = 0;
	}
	else st_handler_info.tRun = mn_chk;

	CString str;

	for (j =0; j<TSITE_SOCKET_CNT; j++)
	{
		str.Format(_T("Daily_RunDown_Time[%d]"), j);
		GetPrivateProfileString(_T("TIME"), str, _T("0"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		//mn_chk = _wtoi(chr_data);
		str_temp.Format(_T("%s"), chr_data);
		mn_chk = _wtoi(str_temp);

		if (mn_chk < 0)
		{
			st_handler_info.m_tDRdown[j] = 0;
		}
		else  st_handler_info.m_tDRdown[j] = mn_chk;

		str.Format(_T("Daily_Stop[%d]"), j);
		GetPrivateProfileString(_T("TIME"), str, _T("0"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		//mn_chk = _wtoi(chr_data);
		str_temp.Format(_T("%s"), chr_data);
		mn_chk = _wtoi(str_temp);

		if (mn_chk < 0)
		{
			st_handler_info.m_tDStop[j] = 0;
		}
		else  st_handler_info.m_tDStop[j] = mn_chk;

		str.Format(_T("Daily_UserStop[%d]"), j);
		GetPrivateProfileString(_T("TIME"), str, _T("0"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		//mn_chk = _wtoi(chr_data);

		str_temp.Format(_T("%s"), chr_data);
		mn_chk = _wtoi(str_temp);

		if (mn_chk < 0)
		{
			st_handler_info.m_tDUserStop[j] = 0;
		}
		else  st_handler_info.m_tDUserStop[j] = mn_chk;

		str.Format(_T("InStandBy_Time[%d]"), j);
		//mstr_temp.Format(_T("%d"),st_handler_info.m_tDInStandby[j]);
		GetPrivateProfileString(_T("TIME"), str, _T("0"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		//mn_chk = _wtoi(chr_data);
		str_temp.Format(_T("%s"), chr_data);
		mn_chk = _wtoi(str_temp);
		//kwlee 2015.1127
// 		if (mn_chk < 0)
// 		{
// 			st_handler_info.m_tDInStandby[j] = 0;
// 		}
// 		else  st_handler_info.m_tDInStandby[j] = mn_chk;

		if (mn_chk < 0)
		{
			st_DB_time.n_Db_time[INSTANBY][j] = 0;
		}
		else  st_DB_time.n_Db_time[INSTANBY][j] = mn_chk;

	}

	for ( k = 0; k < TSITE_SOCKET_CNT; k++ )
	{
		str.Format(_T("Daily_Test_Time[%d]"), k);
		GetPrivateProfileString(_T("TIME"), str, _T("0"), chr_data,sizeof(chr_data), st_path_info.strFileBasic);
		//mn_chk = _wtoi(chr_data);
		str_temp.Format(_T("%s"), chr_data);
		mn_chk = _wtoi(str_temp);
		if (mn_chk < 0)
		{
			st_handler_info.m_tDtest[k] = 0;
		}
		else  st_handler_info.m_tDtest[k] = mn_chk;

		str.Format(_T("Daily_SokOff_Time[%d]"), k);
		GetPrivateProfileString(_T("TIME"), str, _T("0"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		//mn_chk = _wtoi(chr_data);
		str_temp.Format(_T("%s"), chr_data);
		mn_chk = _wtoi(str_temp);
		if (mn_chk < 0)
		{
			st_handler_info.m_tDSokOff[k] = 0;
		}
		else  st_handler_info.m_tDSokOff[k] = mn_chk;

		str.Format(_T("OutStandBy_Time[%d]"), k);
		GetPrivateProfileString(_T("TIME"), str, _T("0"), chr_data,sizeof(chr_data), st_path_info.strFileBasic);
		//mn_chk = _wtoi(chr_data);
		str_temp.Format(_T("%s"), chr_data);
		mn_chk = _wtoi(str_temp);
		//kwlee 2015.1127
// 		if (mn_chk < 0)
// 		{
// 			st_handler_info.m_tDOutStandby[k] = 0;
// 		}
// 		else  st_handler_info.m_tDOutStandby[k] = mn_chk;

		if (mn_chk < 0)
		{
			st_DB_time.n_Db_time[OUTREADY][k] = 0;
		}
		else st_DB_time.n_Db_time[OUTREADY][k] = mn_chk;

		//////
		str.Format(_T("Fail_Device[%d]"), k);
		GetPrivateProfileString(_T("TIME"), str, _T("0"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		//mn_chk = _wtoi(chr_data);
		str_temp.Format(_T("%s"), chr_data);
		mn_chk = _wtoi(str_temp);

		str.Format(_T("Pass_Device[%d]"), k);
		GetPrivateProfileString(_T("TIME"), str, _T("0"), chr_data, sizeof(chr_data), st_path_info.strFileBasic);
		//mn_chk = _wtoi(chr_data);
		str_temp.Format(_T("%s"), chr_data);
		mn_chk = _wtoi(str_temp);
	}
	///////////////////////////	
}
void CMyBasicData::OnVision_Data_Load()
{
	
	OnVisionMarkLoad();
	OnVisionTeachLoad();
	OnVisionReadLoad();
	OnVisionRefLoad();
	OnVisionOffsetValLoad();
	
}

void CMyBasicData::OnVisionMarkLoad()
{
	CString strName,strTemp;
	double dChk;
	char chData[100];
	CString str_1;

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

void CMyBasicData::OnVisionTeachLoad()
{
	CString strName,strTemp;
	double dChk;
	char chData[100];
	CString str_1;

	for (int i =0 ; i<4; i++)
	{
		for (int j =0; j<4; j++)
		{
			//Left X
			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
			:: GetPrivateProfileString(_T("Vision Left X Teach"),strName, _T("0"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
			strTemp.Format(_T("%s"), chData);
			dChk = _wtof(strTemp);
			st_teach.m_dLeftVisionTeachXPos[i][j] = dChk;

			//Right X
			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
			:: GetPrivateProfileString(_T("Vision Right X Teach"),strName, _T("0"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
			strTemp.Format(_T("%s"), chData);
			dChk = _wtof(strTemp);
			st_teach.m_dRightVisionTeachXPos[i][j] = dChk;

			//Left Y
			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
			:: GetPrivateProfileString(_T("Vision Left Y Teach"),strName, _T("0"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
			strTemp.Format(_T("%s"), chData);
			dChk = _wtof(strTemp);
			st_teach.m_dLeftVisionTeachYPos[i][j] = dChk;

			//RIght Y
			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
			:: GetPrivateProfileString(_T("Vision Right Y Teach"),strName, _T("0"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
			strTemp.Format(_T("%s"), chData);
			dChk = _wtof(strTemp);
			st_teach.m_dRightVisionTeachYPos[i][j] = dChk;

		}
	}

}
void CMyBasicData::OnVisionReadLoad()
{
	CString strName,strTemp;
	double dChk;
	char chData[100];
	CString str_1;

	for (int i =0 ; i<4; i++)
	{
		for (int j =0; j<4; j++)
		{
			//Left X
			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
			:: GetPrivateProfileString(_T("Vision Read Left X Offset"),strName, _T("0"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
			strTemp.Format(_T("%s"), chData);
			dChk = _wtof(strTemp);
			st_teach.m_dLeftVisionReadXPos[i][j] = dChk;

			//Right X
			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
			:: GetPrivateProfileString(_T("Vision Read Right X Offset"),strName, _T("0"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
			strTemp.Format(_T("%s"), chData);
			dChk = _wtof(strTemp);
			st_teach.m_dRightVisionReadXPos[i][j] = dChk;

			//Left Y
			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
			:: GetPrivateProfileString(_T("Vision Read Left Y Offset"),strName, _T("0"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
			strTemp.Format(_T("%s"), chData);
			dChk = _wtof(strTemp);
			st_teach.m_dLeftVisionReadYPos[i][j] = dChk;

			//RIght Y
			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
			:: GetPrivateProfileString(_T("Vision Read Right Y Offset"),strName, _T("0"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
			strTemp.Format(_T("%s"), chData);
			dChk = _wtof(strTemp);
			st_teach.m_dRightVisionReadYPos[i][j] = dChk;

		}
	}

}
void CMyBasicData::OnVisionRefLoad()
{
	CString strName,strTemp;
	double dChk;
	char chData[100];

	/////kwlee 2017.0103
	:: GetPrivateProfileString(_T("BASIC_SCREEN"),_T("m_dRobotCameraOffSetXPos"), _T("0"), (LPWSTR)chData, sizeof(chData), st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	dChk = _wtof(strTemp);
	st_teach.m_dRobotCameraOffSetXPos = dChk;

	:: GetPrivateProfileString(_T("BASIC_SCREEN"),_T("m_dRobotCameraOffSetYPos"), _T("0"),(LPWSTR)chData, sizeof(chData), st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	dChk = _wtof(strTemp);
	st_teach.m_dRobotCameraOffSetYPos = dChk;

	:: GetPrivateProfileString(_T("BASIC_SCREEN"),_T("m_dMarkToSocketXPos"), _T("0"), (LPWSTR)chData, sizeof(chData), st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	dChk = _wtof(strTemp);
	st_teach.m_dMarkToSocketXPos = dChk;

	:: GetPrivateProfileString(_T("BASIC_SCREEN"),_T("m_dMarkToSocketYPos"), _T("0"), (LPWSTR)chData, sizeof(chData), st_path_info.strFileBasic);
	strTemp.Format(_T("%s"), chData);
	dChk = _wtof(strTemp);
	st_teach.m_dMarkToSocketYPos = dChk;
}

void CMyBasicData::OnVisionOffsetValLoad()
{
	CString strName,strTemp;
	double dChk;
	char chData[100];
	CString str_1;

	for (int i =0 ; i<4; i++)
	{
		for (int j =0; j<4; j++)
		{
			//Left X
			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
			:: GetPrivateProfileString(_T("Vision Left X Teach Offset Val"),strName, _T("0"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
			strTemp.Format(_T("%s"), chData);
			dChk = _wtof(strTemp);
			st_teach.m_dLeftVisionOffSetXPos[i][j] = dChk;

			//Right X
			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
			:: GetPrivateProfileString(_T("Vision Right X Teach Offset Val"),strName, _T("0"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
			strTemp.Format(_T("%s"), chData);
			dChk = _wtof(strTemp);
			st_teach.m_dRightVisionOffSetXPos[i][j] = dChk;

			//Left Y
			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
			:: GetPrivateProfileString(_T("Vision Left Y Teach Offset Val"),strName, _T("0"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
			strTemp.Format(_T("%s"), chData);
			dChk = _wtof(strTemp);
			st_teach.m_dLeftVisionOffSetYPos[i][j] = dChk;

			//RIght Y
			strName.Format(_T("%d_Site_%d_Pos"), i+1,j+1);
			:: GetPrivateProfileString(_T("Vision Right Y Teach Offset Val"),strName, _T("0"), (LPWSTR)chData, 10, st_path_info.strFileBasic);
			strTemp.Format(_T("%s"), chData);
			dChk = _wtof(strTemp);
			st_teach.m_dRightVisionOffSetYPos[i][j] = dChk;

		}
	}
}



