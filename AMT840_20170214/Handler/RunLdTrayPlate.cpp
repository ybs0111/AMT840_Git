// LdTrayPlate.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Handler.h"
#include "RunLdTrayPlate.h"
#include "Variable.h"
#include "FastechPublic_IO.h"
#include "CtlBd_Library.h"

//james 2016.0802 
#include "CtlBd_Variable.h"
#include "PublicFunction.h"
#include "ComizoaPublic.h"
#include "LogCtrl.h"
#include "LogFromat.h"

// CLdTrayPlate
CRunLdTrayPlate clsRunLdTrayPlate;
CRunLdTrayPlate::CRunLdTrayPlate()
{
	m_nRunStep	= -1;
	m_nInitStep	= -1;
}

CRunLdTrayPlate::~CRunLdTrayPlate()
{
}


// CLdTrayPlate ��� �Լ�
void CRunLdTrayPlate::OnThreadRun(void)
{
	//james 2016.0803
	if (st_handler_info.nSystemLock != FALSE)  return;	// ���� �ý��� Lock ������ ��� ����

	switch(st_handler_info.nRunStatus)
	{
		case dINIT:
			OnRunInit();
			break;

		case dRUN:
			OnRunMove();					
			break;

		case dSTOP:
			m_dwWaitTime[0] = GetCurrentTime();
			break;

		case dWARNING:
			m_dwWaitTime[0] = GetCurrentTime();
			break;

		case dSELFCHECK:
			break;

		case dREINSTATE:
			break;

		case dRECOVERY:
			break;

		default:
			break;
	}
}

void CRunLdTrayPlate::OnRunInit(void)
{
int nRet_1, nRet_2 = 0;

	if (st_sync_info.nInit_Flag[THD_LD_TRAY_PLATE] != INIT_CLEAR)		return;		//INIT_CLEAR �϶��� �ʱ�ȭ �۾��� �Ѵ�. �ʱ�ȭ�� ������ INIT_CLEAR -> INIT_READY�� �Ǳ� ������...
	 
	switch (m_nInitStep)
	{
		case -1:			 
			m_nInitStep = 0;
			break;

		case 0:		

			m_nInitStep = 100;
			break;

		case 100:
			Set_LdTrayPlater_FwdBwd(IO_OFF); 
			m_nInitStep = 110;
			break;

		case 110:
			nRet_1 = Chk_LdTrayPlater_FwdBwd(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nInitStep = 200;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(5500, dWARNING, m_strAlarmCode);
				m_nInitStep = 100;
			}
			break;

		case 200:
			m_nInitStep = 300;
			break;

		case 300:
			st_sync_info.nInit_Flag[THD_LD_TRAY_PLATE] = INIT_READY; 
			 
			m_nInitStep = 0;
			break;	

		default :
			break;

	}
}

void CRunLdTrayPlate::OnRunMove(void) //0816
{
	int nRet_1, nRet_2=0, nRet_3=0;
	int nCount = 0, i=0;
	// jtkim 20160907
	int x, y;
	clsFunc.OnThreadFunctionStepTrace(6, m_nRunStep);

	st_count_info.nLoadTray_Cnt = 1000000;

	

	switch (m_nRunStep)
	{ 
		case -1: 
			st_lot_info[LOT_CURR].nLdTrayCnt = 0;
			st_lot_info[LOT_NEXT].nLdTrayCnt = 0;
			m_nRunStep = 0;
			break;

		case 0:
			if(st_lot_info[LOT_CURR].nLotStatus == LOT_START)//james 2016.0929 
			{
				m_nLotProcessNum = LOT_CURR;
				m_nRunStep = 100;
				
			}
			else if(st_lot_info[LOT_NEXT].nLotStatus == LOT_START)
			{
				m_nLotProcessNum = LOT_NEXT;
				m_nRunStep = 100;				
			}	
			break;

		case 100://������ �ʱ� ���� Ȯ�� 
			nRet_1 = COMI.Get_MotIOSensor(M_LD_ELV, MOT_SENS_SD); 	
			if(nRet_1 == BD_GOOD) //�δ� �÷���Ʈ�� Ʈ���̰� ���� �� ���� 
			{
				m_strAlarmCode.Format(_T("900001")); //900000 1 0 "LOAD_STACKER_PLATE_SD_TRAY_ON_CHECK_ERROR" //kwlee 2016.0902 "900000" ->"900001"
				CTL_Lib.Alarm_Error_Occurrence(5510, dWARNING, m_strAlarmCode); 

				m_nRunStep = 100;
			}
			else 
			{
				m_nRunStep = 200;
			}  
			break;

		case 200:
			Set_LdTrayPlater_FwdBwd(IO_OFF); 
			m_nRunStep = 210;
			break;

		case 210:
			nRet_1 = Chk_LdTrayPlater_FwdBwd(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 300;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(5520, dWARNING, m_strAlarmCode);
				m_nRunStep = 200;
			}
			break; 

		case 300:
			
			m_nRunStep = 1000;
			break;

		//////////////////////////////////////////////////////////////////////////////////
		//work robot�� �����Ͽ� �����Ѵ�
		//////////////////////////////////////////////////////////////////////////////////
		case 1000:  
			st_sync_info.nLdPlate_Tray_Supply_Req[THD_LD_STACKER] = CTL_REQ;
			m_nRunStep = 1100;
			break;

		case 1100:
			if(st_sync_info.nLdPlate_Tray_Supply_Req[THD_LD_STACKER] == CTL_READY)
			{//Ʈ���̸� plate�� ���� ���� ���� 
				////////////////////////////////////////
				// Ʈ���� ���� �������� �̹� ���� ���� 
				////////////////////////////////////////						
				
				//st_count_info.nLoadTray_Cnt++; //Ʈ���� �۾����� 
				if(st_lot_info[m_nLotProcessNum].strLotNo == st_tray_info[THD_LD_STACKER].strLotNo) //2016.0907 
				{
					st_tray_info[THD_LD_TRAY_PLATE].nTrayExist = CTL_YES;		
					st_lot_info[m_nLotProcessNum].nLdTrayCnt++;
					m_nRunStep = 2000; 
				}		

				if (st_handler_info.cWndMain != NULL)
				{
					st_handler_info.cWndMain->SendMessage(WM_WORK_DISPLAY, MAIN_LOT_DISPLAY, 0);
				}
			}
			else  
			{				
				if(st_lot_info[m_nLotProcessNum].nLotStatus >= LOT_END_START)
				{
					if(st_lot_info[m_nLotProcessNum].strLotNo == st_tray_info[THD_LD_STACKER].strLotNo
						&& st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_TRAY_PLATE] == LOT_START) //james 2016.0924
					{
						if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END_START || st_lot_info[m_nLotProcessNum].nLotStatus >= LOT_END_START)
						{
							st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_TRAY_PLATE] =  LOT_END_START;

							st_lot_info[m_nLotProcessNum].nLotStatus = LOT_END_START; //lot end ������ �Ѵ� 
							m_nRunStep = 9000;
						}
					}
				}	
			}
			break;

		case 2000:
			nRet_1 = COMI.Get_MotIOSensor(M_LD_ELV, MOT_SENS_SD); 	
			if(nRet_1 == BD_GOOD) //�δ� �÷���Ʈ�� Ʈ���̰� ���� �� ���� 
			{
				m_nRunStep = 2100;
			}
			else 
			{
				m_strAlarmCode.Format(_T("900001")); //900002 1 0 "LOAD_STACKER_PLATE_SD_TRAY_ON_CHECK_ERROR."
				CTL_Lib.Alarm_Error_Occurrence(5530, dWARNING, m_strAlarmCode); 
			}  
			break;

		case 2100:
			Set_LdTrayPlater_FwdBwd(IO_ON); 
			m_nRunStep = 2110;
			break;

		case 2110:
			nRet_1 = Chk_LdTrayPlater_FwdBwd(IO_ON); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 2200;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(5540, dWARNING, m_strAlarmCode);
				m_nRunStep = 2100;
			}
			break; 

		case 2200: //�� �˻� 
			nRet_1 = COMI.Get_MotIOSensor(M_LD_ELV, MOT_SENS_SD); 	
			if(nRet_1 == BD_GOOD) //�δ� �÷���Ʈ�� Ʈ���̰� ���� �� ���� 
			{
				st_sync_info.nLdPlate_Tray_Supply_Req[THD_LD_TRAY_PLATE] = CTL_CLEAR;
				m_nRunStep = 5000;
			}
			else 
			{
				m_strAlarmCode.Format(_T("900001")); //900001 1 0 "LOAD_STACKER_PLATE_SD_TRAY_ON_CHECK_ERROR."
				CTL_Lib.Alarm_Error_Occurrence(5550, dWARNING, m_strAlarmCode);			 
			} 
			break;


		///////////////////////////////////////////////////////////////////////////////////////////
		// work robot�� Ʈ���� ��û ������ �ް� �۾��Ѵ� 
		///////////////////////////////////////////////////////////////////////////////////////////
		case 5000:
			if(st_lot_info[LOT_NEXT].nLotStatus >= LOT_START) //james 2016.0929 ���� �߰�, Next Lot�� �׽��ͱ��� LotNo�� ���ǵ���LOT_NEXT ���縦 ��ŷ�κ��� ���� �� �ִ� 
			{
				if(st_lot_info[LOT_NEXT].nLotNoApply_FirstTestStart < LOT_START)
				{
					if(st_lot_info[LOT_CURR].nLotStatus == LOT_START)
					{

					}
					else
					{
						break;
					}
				}
				else
				{
					if(st_lot_info[LOT_NEXT].strLotNo == st_tray_info[THD_LD_STACKER].strLotNo)
					{
						nCount = 0;
						//2016.1110
						//for(i = THD_TESTSITE_1; i <= THD_TESTSITE_4; i++) //james 206.0924 
						for(i = THD_TESTSITE_1; i <= THD_TESTSITE_8; i++) //james 206.0924 
						{

							if(st_lot_info[LOT_NEXT].strLotNo ==  st_test_site_info[i].strLotNo)
							{
								nCount++;
							}
						}
						if(nCount == 0)
						{
							break;
						}
					}
				}
			}//2016.0926 

			if(st_sync_info.nWorkRbt_Dvc_Req[THD_LD_TRAY_PLATE][0] == CTL_REQ) //���� ���� ���� Ȯ�� 
			{
				if(st_tray_info[THD_LD_TRAY_PLATE].nTrayExist == CTL_YES) //Ʈ���� ������ �ִ� ���� 
				{
					nRet_1 = COMI.Get_MotIOSensor(M_LD_ELV, MOT_SENS_SD); 	
					if(nRet_1 == BD_GOOD) //�δ� �÷���Ʈ�� Ʈ���̰� ���� �� ���� 
					{
						m_nRunStep = 5100;
					}
					else 
					{
						m_strAlarmCode.Format(_T("900001")); //900001 1 0 "LOAD_STACKER_PLATE_SD_TRAY_ON_CHECK_ERROR."
						CTL_Lib.Alarm_Error_Occurrence(5560, dWARNING, m_strAlarmCode);			 
					} 					
				}
				else //if(st_tray_info[THD_LD_TRAY_PLATE].nTrayExist == CTL_NO)
				{//2016.0908 
					nRet_1 = COMI.Get_MotIOSensor(M_LD_ELV, MOT_SENS_SD); 	
					if(nRet_1 == BD_GOOD) //�δ� �÷���Ʈ�� Ʈ���̰� ���� �� ���� 
					{
						m_strAlarmCode.Format(_T("900001")); //900001 1 0 "LOAD_STACKER_PLATE_SD_TRAY_ON_CHECK_ERROR."
						CTL_Lib.Alarm_Error_Occurrence(5570, dWARNING, m_strAlarmCode); 
					}
					else 
					{
						 m_nRunStep = 1000; //ó�� ���� �ٽ� ũ���̸� ����Ŀ���� ���޹޴´� 
					}
				}
			}  
			else
			{
				//2016.1027 
				//���ص��� �ϴ����� ���� Ʈ���̰� �ִٸ� ���簡 �ִ��� Ȯ���� �ʿ��ϴ�.
				//���⼭ Ȯ�� ������ �ص�����

				
				
				
				//2015.0912 james 
				if(st_lot_info[m_nLotProcessNum].strLotNo == st_tray_info[THD_LD_STACKER].strLotNo) //james 2016.0912 
				{
					if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END_START
						&& st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_TRAY_PLATE] ==  LOT_START)
					{
						st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_TRAY_PLATE] =  LOT_END_START;						
						st_lot_info[m_nLotProcessNum].nLotStatus = LOT_END_START; //lot end ������ �Ѵ� 
						m_nRunStep = 9000;
					}
				}		 
			}
			break; 

		case 5100: //Ʈ���� ������ ã�´� 
			nRet_1 = clsFunc.Find_TrayWork_Pos(0, THD_LD_TRAY_PLATE, CTL_YES, m_npFindWorkPosYXCPB, THD_WORK_RBT, st_worker_info.strLotNo_Empty);
			if(nRet_1 == RET_GOOD) //�۾� ������ ���簡 �ִ�
			{
				// jtkim 20160831
				for (x=0; x<st_recipe_info.nTrayX; x++)
				{
					for (y=0; y<st_recipe_info.nTrayY; y++)
					{
						st_tray_info[THD_LD_TRAY_PLATE].st_pcb_info[y][x].strLotNo	= st_tray_info[THD_LD_STACKER].strLotNo;
						st_tray_info[THD_LD_TRAY_PLATE].st_pcb_info[y][x].strPartNo	= st_tray_info[THD_LD_STACKER].strPartNo;

						// jtkim 20160906
						st_tray_info[THD_LD_TRAY_PLATE].st_pcb_info[y][x].nTestBdStart	= BD_NONE;
						st_tray_info[THD_LD_TRAY_PLATE].st_pcb_info[y][x].nBin			= BD_NONE;
						st_tray_info[THD_LD_TRAY_PLATE].st_pcb_info[y][x].nRetestCnt	= 0;
					}
				}

 				st_sync_info.nWorkRbt_Dvc_Req[THD_LD_TRAY_PLATE][0] = CTL_READY; //���� Ʈ������ �۾��� �����ϴ� 
 				m_nRunStep = 5000;
			}
			else if(nRet_1 == RET_SKIP) //���̻� ���簡 ����
			{	//��� ������ �� ����
				m_nRunStep = 6000;
			}
			break;

		case 6000: //Ʈ�����۰� �۾��� ���� Ʈ���̸� empty�� ������ 
			Set_LdTrayPlater_FwdBwd(IO_OFF); 
			m_nRunStep = 6010;
			break;

		case 6010:
			nRet_1 = Chk_LdTrayPlater_FwdBwd(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 6100;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(5580, dWARNING, m_strAlarmCode);
				m_nRunStep = 6000;
			}
			break; 

		//load plate Ʈ���� ��ü�� ��û 
		case 6100:			
			st_sync_info.nWorkTransfer_Req[THD_LD_TRAY_PLATE][0] = CTL_REQ; //Ʈ���� ��ü�� ��û 
			st_sync_info.nWorkTransfer_Req[THD_LD_TRAY_PLATE][1] = WORK_PICK; //Ʈ���� ����� ��û 
			m_nRunStep = 6200;
			break;

		case 6200:
			if(st_sync_info.nWorkTransfer_Req[THD_LD_TRAY_PLATE][0] == CTL_READY) //Ʈ�����۰� Ʈ���̸� ���� ���� 
			{
				nRet_1 = COMI.Get_MotIOSensor(M_LD_ELV, MOT_SENS_SD); 	
				if(nRet_1 == BD_ERROR) //Ʈ���̰� ���� �ȵ� ���� 
				{
					st_sync_info.nWorkTransfer_Req[THD_LD_TRAY_PLATE][0] = CTL_CLEAR;
					st_sync_info.nWorkTransfer_Req[THD_LD_TRAY_PLATE][1] = CTL_CLEAR; 
					m_nRunStep = 6300;
				}
				else 
				{
					m_strAlarmCode.Format(_T("900002")); //900002 1 0 "LOAD_STACKER_PLATE_SD_TRAY_OFF_CHECK_ERROR."
					CTL_Lib.Alarm_Error_Occurrence(5590, dWARNING, m_strAlarmCode);			 
				} 
			}
			break;

		case 6300:
			if(st_lot_info[m_nLotProcessNum].strLotNo == st_tray_info[THD_LD_STACKER].strLotNo) //james 2016.0912 
			{
				if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END_START)
				{
					st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_TRAY_PLATE] =  LOT_END_START;						
					st_lot_info[m_nLotProcessNum].nLotStatus = LOT_END_START; //lot end ������ �Ѵ� 
					m_nRunStep = 9000;
					break;
				}
			}

			//lot end ������ �ε� ī��Ʈ ���� üũ�� ��ũ �κ����� �����Ѵ�.
			
			m_nRunStep = 1000; //����Ŀ�� Ʈ���� ������ ��û���� ó������ ���� 
			break;



		case 9000: //lot end ó�� ��ƾ  // 
			Set_LdTrayPlater_FwdBwd(IO_OFF); 
			m_nRunStep = 9010;
			break;

		case 9010:
			nRet_1 = Chk_LdTrayPlater_FwdBwd(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 9100;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(5600, dWARNING, m_strAlarmCode);
				m_nRunStep = 9000;
			}
			break; 

		case 9100:

			/*
			nRet_1 = COMI.Get_MotIOSensor(M_LD_ELV, MOT_SENS_SD); 	
			if(nRet_1 == BD_GOOD) //�δ� �÷���Ʈ�� Ʈ���̰� ���� �� ���� 
			{
				m_strAlarmCode.Format(_T("900002")); //900002 1 0 "LOAD_STACKER_PLATE_SD_TRAY_OFF_CHECK_ERROR."
				CTL_Lib.Alarm_Error_Occurrence(m_nRunStep, dWARNING, m_strAlarmCode);
			}
			else 
			{
				st_sync_info.nLdPlate_Tray_Supply_Req[THD_LD_TRAY_PLATE] = CTL_CLEAR; 
				m_nRunStep = 0;		 
			} 
			*/

			if(st_lot_info[m_nLotProcessNum].strLotNo == st_tray_info[THD_LD_STACKER].strLotNo) //james 2016.0912 
			{
				if(st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END_START)
				{
					st_lot_info[m_nLotProcessNum].nLot_THD_Status[THD_LD_TRAY_PLATE] =  LOT_END; 
					m_nRunStep = 0;
					break;
				}
			}		
			break; 


		default :
			break;
		}
}


	

void CRunLdTrayPlate::Set_LdTrayPlater_FwdBwd( int nOnOff )
{
	CString strLogKey[10];
	CString	strLogData[10];

	strLogKey[0] = _T("Mode Start");
	strLogData[0].Format(_T("%d"),0);

	m_bTrayPlateFwdBwdFlag	= false;
	m_dwWaitTrayPlateFwdBwd[0]	= GetCurrentTime();

	FAS_IO.set_out_bit( st_io_info.o_LdTrayPlateLfOnOffCyl, nOnOff);
	FAS_IO.set_out_bit( st_io_info.o_LdTrayPlateRiOnOffCyl, nOnOff);


	///kwlee 2016.0812
	if (nOnOff == IO_ON)
	{
		//LogFunction(_T("LD_TRAY_PLATE"),_T("FORWARD"),_T("TRAY_PLATE"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("LD_TRAY_PLATE"),_T("FORWARD"),0,_T("TRAY_PLATE"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
	else
	{
		//LogFunction(_T("LD_TRAY_PLATE"),_T("BACKWARD"),_T("TRAY_PLATE"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("LD_TRAY_PLATE"),_T("BACKWARD"),0,_T("TRAY_PLATE"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
}

int CRunLdTrayPlate::Chk_LdTrayPlater_FwdBwd( int nOnOff )
{
	CString strLogKey[10];
	CString	strLogData[10];

	strLogKey[0] = _T("Mode End");
	strLogData[0].Format(_T("%d"),0);
//	800108 1 0 "LOADING_TRAY_PLATE_LEFT_ON_CHECK"
//	800109 1 0 "LOADING_TRAY_PLATE_LEFT_OFF_CHECK"
//	800110 1 0 "LOADING_TRAY_PLATE_RIGHT_ON_CHECK"
//	800111 1 0 "LOADING_TRAY_PLATE_RIGHT_OFF_CHECK"

	int nWaitTime = WAIT_LDTRAYPLATE_FWD_BWD;

	if (nOnOff == IO_OFF)
	{
		if (m_bTrayPlateFwdBwdFlag == false &&
			FAS_IO.get_in_bit(st_io_info.i_LdTrayPlateLfOffChk,	IO_ON)	== IO_ON &&
			FAS_IO.get_in_bit(st_io_info.i_LdTrayPlateLfOnChk,	IO_OFF)	== IO_OFF &&
			FAS_IO.get_in_bit(st_io_info.i_LdTrayPlateRiOffChk,	IO_ON)	== IO_ON &&
			FAS_IO.get_in_bit(st_io_info.i_LdTrayPlateRiOnChk,	IO_OFF)	== IO_OFF )
		{
			m_bTrayPlateFwdBwdFlag		= true;
			m_dwWaitTrayPlateFwdBwd[0]	= GetCurrentTime();
		}
		else if (m_bTrayPlateFwdBwdFlag == true &&
			FAS_IO.get_in_bit(st_io_info.i_LdTrayPlateLfOffChk,	IO_ON)	== IO_ON &&
			FAS_IO.get_in_bit(st_io_info.i_LdTrayPlateLfOnChk,	IO_OFF)	== IO_OFF &&
			FAS_IO.get_in_bit(st_io_info.i_LdTrayPlateRiOffChk,	IO_ON)	== IO_ON &&
			FAS_IO.get_in_bit(st_io_info.i_LdTrayPlateRiOnChk,	IO_OFF)	== IO_OFF )
		{
			m_dwWaitTrayPlateFwdBwd[1] = GetCurrentTime();
			m_dwWaitTrayPlateFwdBwd[2] = m_dwWaitTrayPlateFwdBwd[1] - m_dwWaitTrayPlateFwdBwd[0];

			if (m_dwWaitTrayPlateFwdBwd[2] <= 0)
			{
				m_dwWaitTrayPlateFwdBwd[0] = GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwWaitTrayPlateFwdBwd[2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
			{
				//LogFunction(_T("LD_TRAY_PLATE"),_T("BACKWARD"),_T("TRAY_PLATE"),_T("CYLINDER"),1,strLogKey,strLogData);//kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("LD_TRAY_PLATE"),_T("BACKWARD"),1,_T("TRAY_PLATE"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_GOOD;
			}
		}
		else
		{
			m_dwWaitTrayPlateFwdBwd[1] = GetCurrentTime();
			m_dwWaitTrayPlateFwdBwd[2] = m_dwWaitTrayPlateFwdBwd[1] - m_dwWaitTrayPlateFwdBwd[0];

			if (m_dwWaitTrayPlateFwdBwd[2] <= 0)
			{
				m_dwWaitTrayPlateFwdBwd[0] = GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwWaitTrayPlateFwdBwd[2] > (DWORD)st_wait_info.nLimitWaitTime[nWaitTime])
			{
				if(	FAS_IO.get_in_bit(st_io_info.i_LdTrayPlateLfOffChk,	IO_ON)	!= IO_ON )
				{
					m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_LdTrayPlateLfOffChk); 
				}
				else if(	FAS_IO.get_in_bit(st_io_info.i_LdTrayPlateLfOnChk,	IO_OFF)	!= IO_OFF )
				{
					m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_LdTrayPlateLfOnChk); 
				}
				else if(	FAS_IO.get_in_bit(st_io_info.i_LdTrayPlateRiOffChk,	IO_ON)	!= IO_ON )
				{
					m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_LdTrayPlateRiOffChk); 
				}
				else
				{
					m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_LdTrayPlateRiOnChk); 
				}
				return RET_ERROR;
			}
		}
	}
	else
	{
		if (m_bTrayPlateFwdBwdFlag == false &&
			FAS_IO.get_in_bit(st_io_info.i_LdTrayPlateLfOffChk,	IO_OFF)	== IO_OFF &&
			FAS_IO.get_in_bit(st_io_info.i_LdTrayPlateLfOnChk,	IO_ON)	== IO_ON &&
			FAS_IO.get_in_bit(st_io_info.i_LdTrayPlateRiOffChk,	IO_OFF)	== IO_OFF &&
			FAS_IO.get_in_bit(st_io_info.i_LdTrayPlateRiOnChk,	IO_ON)	== IO_ON )
		{
			m_bTrayPlateFwdBwdFlag			= true;
			m_dwWaitTrayPlateFwdBwd[0]	= GetCurrentTime();
		}
		else if (m_bTrayPlateFwdBwdFlag == true &&
				FAS_IO.get_in_bit(st_io_info.i_LdTrayPlateLfOffChk,	IO_OFF)	== IO_OFF &&
				FAS_IO.get_in_bit(st_io_info.i_LdTrayPlateLfOnChk,	IO_ON)	== IO_ON &&
				FAS_IO.get_in_bit(st_io_info.i_LdTrayPlateRiOffChk,	IO_OFF)	== IO_OFF &&
				FAS_IO.get_in_bit(st_io_info.i_LdTrayPlateRiOnChk,	IO_ON)	== IO_ON )
		{
			m_dwWaitTrayPlateFwdBwd[1]	= GetCurrentTime();
			m_dwWaitTrayPlateFwdBwd[2]	= m_dwWaitTrayPlateFwdBwd[1] - m_dwWaitTrayPlateFwdBwd[0];

			if (m_dwWaitTrayPlateFwdBwd[2] <= 0)
			{
				m_dwWaitTrayPlateFwdBwd[0]	= GetCurrentTime();
				return RET_PROCEED;
			}

			if(m_dwWaitTrayPlateFwdBwd[2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
			{
				//LogFunction(_T("LD_TRAY_PLATE"),_T("FORWARD"),_T("TRAY_PLATE"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("LD_TRAY_PLATE"),_T("FORWARD"),1,_T("TRAY_PLATE"),_T("CYLINDER"),1,strLogKey,strLogData); 
				return RET_GOOD;
			}
		}
		else
		{
			m_dwWaitTrayPlateFwdBwd[1]	= GetCurrentTime();
			m_dwWaitTrayPlateFwdBwd[2]	= m_dwWaitTrayPlateFwdBwd[1] - m_dwWaitTrayPlateFwdBwd[0];

			if (m_dwWaitTrayPlateFwdBwd[2] <= 0)
			{
				m_dwWaitTrayPlateFwdBwd[0]	= GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwWaitTrayPlateFwdBwd[2] > (DWORD)st_wait_info.nLimitWaitTime[nWaitTime])
			{
				if(	FAS_IO.get_in_bit(st_io_info.i_LdTrayPlateLfOffChk,	IO_OFF)	!= IO_OFF )
				{
					m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_LdTrayPlateLfOffChk); 
				}
				else if(	FAS_IO.get_in_bit(st_io_info.i_LdTrayPlateLfOnChk,	IO_ON)	!= IO_ON )
				{
					m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_LdTrayPlateLfOnChk); 
				}
				else if(	FAS_IO.get_in_bit(st_io_info.i_LdTrayPlateRiOffChk,	IO_OFF)	!= IO_OFF )
				{
					m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_LdTrayPlateRiOffChk); 
				}
				else
				{
					m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_LdTrayPlateLfOnChk); 
				}
				//2017.0109
				clsLog.LogFunction(_T("LD_TRAY_PLATE"),_T("FORWARD"),1,_T("TRAY_PLATE"),_T("CYLINDER"),1,strLogKey,strLogData); 
				return RET_ERROR;
			}
		}
	}

	return RET_PROCEED;
}

