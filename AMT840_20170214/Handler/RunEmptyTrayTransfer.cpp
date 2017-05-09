// EmptyTrayTransfer.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Handler.h"
#include "RunEmptyTrayTransfer.h"
#include "Variable.h"
#include "FastechPublic_IO.h"
#include "CtlBd_Library.h"

//james 2016.0802 
#include "CtlBd_Variable.h"
#include "PublicFunction.h"
#include "ComizoaPublic.h"
#include "math.h"
#include "LogCtrl.h"
#include "LogFromat.h"

// clsRunEmptyTrayTransfer
CRunEmptyTrayTransfer clsRunEmptyTrayTransfer;
CRunEmptyTrayTransfer::CRunEmptyTrayTransfer()
{
	m_nRunStep	= -1;
	m_nInitStep	= -1;	
	m_nEmptyToReject = 0;

	m_nAxisNum = M_WORKTRAY_TRANSFER;
}

CRunEmptyTrayTransfer::~CRunEmptyTrayTransfer()
{
}


// clsRunEmptyTrayTransfer ��� �Լ�
void CRunEmptyTrayTransfer::OnThreadRun(void)
{
	//james 2016.0804
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

void CRunEmptyTrayTransfer::OnRunInit(void)
{
	int nRet_1, nRet_2=0;

	if (st_sync_info.nInit_Flag[THD_WORK_TRANSFER] != INIT_CLEAR)		return;		//INIT_CLEAR �϶��� �ʱ�ȭ �۾��� �Ѵ�. �ʱ�ȭ�� ������ INIT_CLEAR -> INIT_READY�� �Ǳ� ������...
	 
	switch (m_nInitStep)
	{
		case -1:		

			m_nInitStep = 0;
			break;

		case 0:		
			//kwlee 2016.1221 Empty Stacker Elevator �ʱ�ȭ �� Empty Transfer  �ʱ�ȭ ����
			//if( st_sync_info.nInit_Flag[THD_LD_TRAY_PLATE] < INIT_READY) break;
			if( st_sync_info.nInit_Flag[THD_LD_TRAY_PLATE] < INIT_READY || st_sync_info.nInit_Flag[THD_EMPTY_STACKER] < INIT_READY) break;
			if(COMI.mn_homechk_flag[M_WORK_RBT_Z] != CTL_YES) break;//2015.0915

			m_nInitStep = 100;
			break;

		case 100: //up
			Set_EmptyTrayTransfer_UpDown(IO_OFF); 
			m_nInitStep = 110;
			break;

		case 110:
			nRet_1 = Chk_EmptyTrayTransfer_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nInitStep = 200;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(4500, dWARNING, m_strAlarmCode);
				m_nInitStep = 100;
			}
			break;

		case 200:
			nRet_1 = COMI.HomeCheck_Mot(m_nAxisNum, MOT_SENS_ELM, MOT_TIMEOUT) ; //
			if(nRet_1 == BD_GOOD)
			{
				COMI.mn_homechk_flag[m_nAxisNum] = CTL_YES;
				m_nInitStep = 300;
			}
			else if(nRet_1 == BD_ERROR)
			{
				CTL_Lib.mn_run_status = CTL_dWARNING;
				CTL_Lib.Alarm_Error_Occurrence(4510, CTL_Lib.mn_run_status, COMI.strCode);
				m_nInitStep = 200;
			}			
			break;

		case 300:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_TrayTrsfVacChk,	IO_OFF);  
			if(nRet_1 == IO_OFF)
			{
				m_nInitStep = 400;
			}
			else
			{//Ʈ���� ���� ���� 
				m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_TrayTrsfVacChk); 
				CTL_Lib.Alarm_Error_Occurrence(4520, dWARNING, m_strAlarmCode);
			}
			break;

		case 400:
			Set_EmptyTrayVacuum_OnOff(IO_OFF );
			Set_EmptyTransferEjectorOnOff(IO_OFF );
			m_nInitStep = 900;
			break;

		case 900:
			st_sync_info.nInit_Flag[THD_WORK_TRANSFER] = INIT_READY; 
			 
			m_nInitStep = 0;
			break;	

		default :
			break;

	}
}

void CRunEmptyTrayTransfer::OnRunMove(void)
{
	int nRet_1, nRet_2=0, nRet_3=0;
	int nCount = 0, nJobFlag=0;

	clsFunc.OnThreadFunctionStepTrace(4, m_nRunStep);	

	switch (m_nRunStep)
	{ 
		case -1: 

			m_nRunStep = 0;
			break;

		case 0:
			if(st_lot_info[LOT_CURR].nLotStatus == LOT_START || st_lot_info[LOT_NEXT].nLotStatus == LOT_START )
			{
//				st_handler_info.mn_removetray = NO; //kwlee 2017.0117
				m_nRunStep = 100;
			}

			//////////////////////////////////////////////////////////////////////////////////////////////
			//���� ���԰� ���ÿ� �ڵ����� empty Ʈ���̸� ��������.
			//2017.0430
			if( ( st_lot_info[LOT_CURR].strLotNo !=_T("") && st_lot_info[LOT_NEXT].strLotNo ==_T("") )  || ( st_lot_info[LOT_CURR].strLotNo ==_T("") && st_lot_info[LOT_NEXT].strLotNo ==_T("") ) )
			{
				if( st_handler_info.mn_removetray == CTL_REQ && st_handler_info.mn_uldnum == 1 )
				{
					if( st_lot_info[LOT_CURR].strLotNo ==_T("") && st_lot_info[LOT_NEXT].strLotNo ==_T("") )
					{
						m_nRunStep = 100;	
					}
					else if( st_count_info.nInCount[0][0] <= 0 && st_lot_info[LOT_CURR].strLotNo !=_T("") && st_lot_info[LOT_NEXT].strLotNo ==_T("") && st_handler_info.mn_uldnum == 1)
					{
						m_nRunStep = 100;
					}
					else
					{
						st_handler_info.mn_removetray = CTL_NO;
					}
				}				
			}

			//////////////////////////////////////////////////////////////////////////////////////////////
			//2017.0116
			//if( st_lot_info[LOT_CURR].strLotNo !=_T("") || st_lot_info[LOT_NEXT].strLotNo !=_T("")	)
			if( st_lot_info[LOT_CURR].strLotNo ==_T("") && st_lot_info[LOT_NEXT].strLotNo ==_T(""))
			{
				if( st_handler_info.mn_removetray == CTL_REQ && st_handler_info.mn_uldnum == 1 )
				{
					m_nRunStep = 100;	
				}
			}
			break;

		case 100://up
			Set_EmptyTrayTransfer_UpDown(IO_OFF); 
			m_nRunStep = 110;
			break;

		case 110:
			nRet_1 = Chk_EmptyTrayTransfer_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 200;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(4600, dWARNING, m_strAlarmCode);
				m_nRunStep = 100;
			}
			break;

		case 200:
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_TrayTrsfVacChk,	IO_OFF);  
			if(nRet_1 == IO_OFF)
			{
				m_nRunStep = 300;
			}
			else
			{//Ʈ���� ���� ���� 
				m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_TrayTrsfVacChk); 
				CTL_Lib.Alarm_Error_Occurrence(4610, dWARNING, m_strAlarmCode);
			}
			break;

		case 300:
			Set_EmptyTrayVacuum_OnOff(IO_OFF );
			Set_EmptyTransferEjectorOnOff(IO_OFF );

			m_nRunStep = 400;
			break;

		case 400:
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nAxisNum, st_motor_info[m_nAxisNum].d_pos[P_WORKTRANS_EMPTY_STACKER], COMI.mn_runspeed_rate);  //left�� �̸� �̵��� �ִ´� 
			
			if (nRet_1 == BD_GOOD) //�������� �̵�  
			{			
				m_nRunStep = 900;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nRunStep = 400;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�  
				CTL_Lib.Alarm_Error_Occurrence(4620, dWARNING, st_alarm_info.strCode);
				m_nRunStep = 400;
			}
			break; 


		case 900: //�⺻���� �ʱ� ������ �Ϸ�� ���� 
			st_tray_info[THD_WORK_TRANSFER].nTrayExist = CTL_NO; //james 2016.0915
			m_nTransfer_WaitPosMove_Flag = CTL_NO;
			m_nRunStep = 1000;
			break;

		//////////////////////////////////////////////////////////////////////////
		// Tray ���� �� ��ü ���
		//////////////////////////////////////////////////////////////////////////
		case 1000:
// 			nJobFlag = 0;
// 			if(st_sync_info.nWorkTransfer_Req[THD_LD_TRAY_PLATE][0] == CTL_REQ) //Load Tray PlateƮ������ �� Ʈ,���� ��ü�� ��û 
// 			{
// 				nJobFlag += (int) pow((double)2, 0);// �۾��� ��ġ ���; //1
// 			}
// 			if(st_sync_info.nWorkTransfer_Req[THD_ULD_1_STACKER][0] == CTL_REQ) //��ε� #1 �� �� Ʈ���� ������ ��û �޾Ҵ� 
// 			{
// 				nJobFlag += (int) pow((double)2, 1);// �۾��� ��ġ ���; //2
// 			}
// 			if(st_sync_info.nWorkTransfer_Req[THD_ULD_2_STACKER][0] == CTL_REQ) //��ε� #2 �� �� Ʈ���� ������ ��û �޾Ҵ� 
// 			{
// 				nJobFlag += (int) pow((double)2, 2);// �۾��� ��ġ ���; //4
// 			}
			//2017.0116
			if( st_handler_info.mn_removetray == CTL_REQ || st_handler_info.mn_removetray == CTL_CHANGE)
			{
				//if( st_lot_info[LOT_CURR].strLotNo ==_T("") && st_lot_info[LOT_NEXT].strLotNo ==_T("")	)//kwlee 2017.0125
				//{
					if(st_sync_info.nWorkTransfer_Req[THD_ULD_1_STACKER][0] == CTL_REQ) //��ε� #1 �� �� Ʈ���� ������ ��û �޾Ҵ� 
					{
						nJobFlag += (int) pow((double)2, 1);// �۾��� ��ġ ���; //2
					}
					if(st_sync_info.nWorkTransfer_Req[THD_ULD_2_STACKER][0] == CTL_REQ) //��ε� #2 �� �� Ʈ���� ������ ��û �޾Ҵ� 
					{
						nJobFlag += (int) pow((double)2, 2);// �۾��� ��ġ ���; //4
					}
				//}
			}
			else
			{			
				if(st_sync_info.nWorkTransfer_Req[THD_LD_TRAY_PLATE][0] == CTL_REQ) //Load Tray PlateƮ������ �� Ʈ,���� ��ü�� ��û 
				{
					nJobFlag += (int) pow((double)2, 0);// �۾��� ��ġ ���; //1
				}
				if(st_sync_info.nWorkTransfer_Req[THD_ULD_1_STACKER][0] == CTL_REQ) //��ε� #1 �� �� Ʈ���� ������ ��û �޾Ҵ� 
				{
					nJobFlag += (int) pow((double)2, 1);// �۾��� ��ġ ���; //2
				}
				if(st_sync_info.nWorkTransfer_Req[THD_ULD_2_STACKER][0] == CTL_REQ) //��ε� #2 �� �� Ʈ���� ������ ��û �޾Ҵ� 
				{
					nJobFlag += (int) pow((double)2, 2);// �۾��� ��ġ ���; //4
				}				
			}
			////

			//if(nJobFlag == 0 || st_lot_info[LOT_CURR].nLot_THD_Status[THD_WORK_RBT] ==  LOT_END) //james 2016.0926 	
			//2017.0116
			if( st_handler_info.mn_removetray == CTL_NO && (nJobFlag == 0 || st_lot_info[LOT_CURR].nLot_THD_Status[THD_WORK_RBT] ==  LOT_END) )
			{
				//lot end Ȯ�� 
				if(st_lot_info[LOT_CURR].nLotStatus >= LOT_START && st_lot_info[LOT_NEXT].nLotStatus >= LOT_START )
				{
					if(st_lot_info[LOT_CURR].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END && st_lot_info[LOT_NEXT].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END)
					{
						if(st_tray_info[THD_WORK_TRANSFER].nTrayExist == CTL_YES) 
						{
							m_nTransferJobFlag[0] = THD_EMPTY_STACKER; 
							m_nTransferJobFlag[1] = WORK_PLACE; 

							//�̹� ��� �ִ� Ʈ���̸� ������ empty site�� ���� �غ��϶�� ��û�Ѵ� 
							st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] = CTL_REQ;
							st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][1] = WORK_PLACE;

							m_nRunStep = 2000; //Ʈ���̸� ������ empty site�� �̵��Ѵ� 
							break;
						}
						else
						{
							st_lot_info[LOT_CURR].nLot_THD_Status[THD_WORK_TRANSFER] =  LOT_END;
							st_lot_info[LOT_NEXT].nLot_THD_Status[THD_WORK_TRANSFER] =  LOT_END;
							m_nRunStep = 0; 
							break;
						}
					}
					else
					{
						if(m_nTransfer_WaitPosMove_Flag == CTL_NO)
						{
							m_nRunStep = 1100;
							break;
						} 
					}
				}
				else if(st_lot_info[LOT_CURR].nLotStatus >= LOT_START
					&& st_lot_info[LOT_NEXT].nLotStatus < LOT_START ) //james 2016.0924 
				{
					if(st_lot_info[LOT_CURR].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END)
					{
						if(st_tray_info[THD_WORK_TRANSFER].nTrayExist == CTL_YES) 
						{
							m_nTransferJobFlag[0] = THD_EMPTY_STACKER; 
							m_nTransferJobFlag[1] = WORK_PLACE; 

							//�̹� ��� �ִ� Ʈ���̸� ������ empty site�� ���� �غ��϶�� ��û�Ѵ� 
							st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] = CTL_REQ;
							st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][1] = WORK_PLACE;

							m_nRunStep = 2000; //Ʈ���̸� ������ empty site�� �̵��Ѵ� 
							break;
						}
						else
						{
							st_lot_info[LOT_CURR].nLot_THD_Status[THD_WORK_TRANSFER] =  LOT_END;
							m_nRunStep = 0; 
							break;
						}
					}
					else
					{
						if(m_nTransfer_WaitPosMove_Flag == CTL_NO)
						{
							m_nRunStep = 1100;
							break;
						} 
					}
				}
				else if(st_lot_info[LOT_NEXT].nLotStatus >= LOT_START )
				{
					if(st_lot_info[LOT_NEXT].nLot_THD_Status[THD_WORK_RBT] >=  LOT_END)
					{
						if(st_tray_info[THD_WORK_TRANSFER].nTrayExist == CTL_YES) 
						{
							m_nTransferJobFlag[0] = THD_EMPTY_STACKER; 
							m_nTransferJobFlag[1] = WORK_PLACE; 

							//�̹� ��� �ִ� Ʈ���̸� ������ empty site�� ���� �غ��϶�� ��û�Ѵ� 
							st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] = CTL_REQ;
							st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][1] = WORK_PLACE;

							m_nRunStep = 2000; //Ʈ���̸� ������ empty site�� �̵��Ѵ� 
							break;
						}
						else
						{
							st_lot_info[LOT_NEXT].nLot_THD_Status[THD_WORK_TRANSFER] =  LOT_END;
							m_nRunStep = 0; 
							break;
						}
					}
					else
					{
						if(m_nTransfer_WaitPosMove_Flag == CTL_NO)
						{
							m_nRunStep = 1100;
							break;
						} 
					}
				}//else if				 
			}//if
					
			/////////////////////////////////////////////////////////////////////////////////////////////////
			//���ո��� ������ �����ϱ� ���� �۾����� Ʈ���̸� ������ �ִ��� / ������ Ȯ���� ������ �����Ѵ� 
			/////////////////////////////////////////////////////////////////////////////////////////////////
			if(st_tray_info[THD_WORK_TRANSFER].nTrayExist == CTL_YES) //Ʈ���̸� ������ ������, �ε���  
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_TrayTrsfVacChk,	IO_ON);  
				if(nRet_1 == IO_OFF)
				{	//Ʈ���̰� ��� �����߻�  
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_TrayTrsfVacChk); 
					CTL_Lib.Alarm_Error_Occurrence(4630, dWARNING, m_strAlarmCode);
					break;
				}	

				if(nJobFlag == 1) //���� �ε�Ʈ���� ����� �Ѵٸ� Ʈ���̸� ���� ���� �� 
				{ //�����ִ� Ʈ���̸� empty stacker�� ���� �ε� �� Ʈ���̸� ������ �̵��Ѵ� 
					//m_nTransferJobFlag[2]; //work Ʈ�����ۿ� ������ ��û�Ѵ� , [0]:��û����, [1]:1:Ʈ���̸� ����, 2:Ʈ���̸� ����	
					m_nTransferJobFlag[0] = THD_EMPTY_STACKER; 
					m_nTransferJobFlag[1] = WORK_PLACE; 

					//�̹� ��� �ִ� Ʈ���̸� ������ empty site�� ���� �غ��϶�� ��û�Ѵ� 
					st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] = CTL_REQ;
					st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][1] = WORK_PLACE;
				}
				else if(nJobFlag == 2 || nJobFlag == 3|| nJobFlag == 4 || nJobFlag == 5 || nJobFlag == 6 || nJobFlag == 7)
				{//���� �ִ� Ʈ���̸� ��ε� �۾� ��û���� ����Ŀ�� �����Ѵ� 
					if(nJobFlag == 2 || nJobFlag == 3 || nJobFlag == 6 || nJobFlag == 7) //unload #1 �Ǵ� #2 ����Ʈ�� Ʈ���� ���� ��û ���� ����
					{
						m_nTransferJobFlag[0] = THD_ULD_1_STACKER; 
						m_nTransferJobFlag[1] = WORK_PLACE; 
					}
					else if(nJobFlag == 4 || nJobFlag == 5)
					{
						m_nTransferJobFlag[0] = THD_ULD_2_STACKER; 
						m_nTransferJobFlag[1] = WORK_PLACE; 
					}
				}
			}
			else //if(st_tray_info[THD_WORK_TRANSFER].nTrayExist == CTL_NO) //�÷��װ� Ʈ���̸� �������� ���� ���·� traty PICK �۾��� �����Ѵ�  
			{
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_TrayTrsfVacChk,	IO_OFF);  				
				if(nRet_1 == IO_ON)
				{
					//Ʈ���� ������ �˶� 
					m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_TrayTrsfVacChk); 
					CTL_Lib.Alarm_Error_Occurrence(4640, dWARNING, m_strAlarmCode);
					break;
				}

// 				///////////////////////////////////////////////////////////////////////////////////////////////////////////
// 				//2017.0430
				if( st_handler_info.mn_auto_empty_tray == YES  && st_handler_info.mn_out_empty == YES )
				{
					m_nTransferJobFlag[0] = THD_EMPTY_STACKER; 
					m_nTransferJobFlag[1] = WORK_PICK; 
					st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] = CTL_REQ;
					st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][1] = WORK_PICK;
					nJobFlag = 100;
				}
				///////////////////////////////////////////////////////////////////////////////////////////////////////////
				else if(nJobFlag == 1 || nJobFlag == 3 || nJobFlag == 5 || nJobFlag == 7) //���� �ε�Ʈ���� ���� ����� ó���� �� �����۾��Ѵ�  
				{ // �ε� �� Ʈ���̸� ������ �̵��Ѵ� 
					//m_nTransferJobFlag[2]; //work Ʈ�����ۿ� ������ ��û�Ѵ� , [0]:��û����, [1]:1:Ʈ���̸� ����, 2:Ʈ���̸� ����	
					m_nTransferJobFlag[0] = THD_LD_TRAY_PLATE; 
					m_nTransferJobFlag[1] = WORK_PICK; 
				}
				else //if((nJobFlag == 2 || nJobFlag == 4 || nJobFlag == 6)// ��ε� ����Ŀ 1,2 ��ġ   nJobFlag == 8
				{//��ε� ����Ŀ ����Ʈ���� Ʈ���� ������ ��û�� ����
 
					m_nTransferJobFlag[0] = THD_EMPTY_STACKER; 
					m_nTransferJobFlag[1] = WORK_PICK; 

					//Ʈ���̸� �������� ������,  ������ empty site�� ���� ��ƾ���� Ʈ���̸� ������ ����
					st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] = CTL_REQ;
					st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][1] = WORK_PICK;

					nJobFlag = 100; //james 2018.0828 

				}
			}

			if(nJobFlag > 0) //�۾� �� ��ġ�� �ִ�
			{
				m_nTransfer_WaitPosMove_Flag = CTL_NO;
				m_nRunStep = 2000;
			}
			break;

	 
		//////////////////////////////////////////////////////////////////////
		// Ʈ�����۸� ��� ��ġ�� �̼� ��Ų�� 
		//////////////////////////////////////////////////////////////////////
		case 1100: //Ʈ������ �� üũ 
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_TrayTrsfUpChk,	IO_ON); //Ʈ�����۰� ��Ŀ�� ��� �ִ� ���� üũ
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_TrayTrsfDnChk,	IO_OFF); //james 2016.0921 
			if(nRet_1 == IO_ON && nRet_2 == IO_OFF)
			{ //��Ŀ�� ��� ���� 
				m_nRunStep = 1200;		 //james 2016.0921 		
			}
			else 
			{
				m_nRunStep = 1110;		
			}
			break;

		case 1110://transfer up 
			Set_EmptyTrayTransfer_UpDown(IO_OFF); 
			m_nRunStep = 1111;
			break;

		case 1111:
			nRet_1 = Chk_EmptyTrayTransfer_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 1200;
			}
			else if(nRet_1 == RET_ERROR)
			{
// 				CTL_Lib.Alarm_Error_Occurrence(4650, dWARNING, m_strAlarmCode);
// 				m_nRunStep = 1100;
				//2017.0116
				if( st_handler_info.mn_removetray == CTL_CHANGE )//st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] = CTL_FREE;
				{
					m_nRunStep = 0;
				}
				else
				{
					CTL_Lib.Alarm_Error_Occurrence(4650, dWARNING, m_strAlarmCode);
					m_nRunStep = 1100;
				}
			}

			break;

		case 1200:
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nAxisNum, st_motor_info[m_nAxisNum].d_pos[P_WORKTRANS_EMPTY_STACKER], COMI.mn_runspeed_rate);  //left�� �̸� �̵��� �ִ´� 
			if (nRet_1 == BD_GOOD) //�������� �̵�  
			{			
				m_nTransfer_WaitPosMove_Flag = CTL_YES;

				m_nRunStep = 1000;

				//2017.0109
				if( st_tray_info[THD_WORK_TRANSFER].nStartStatus  == 1)
				{
					st_tray_info[THD_WORK_TRANSFER].nStartStatus = 0;

					clsLog.LogTransfer(_T("EMPTY_TRAY_TRANSFER"),
										_T("MOVE_EMPTY_TRAY"),
										1,
										st_tray_info[THD_WORK_TRANSFER].strMaterial,
										_T("EMPTY_TRAY"),
										st_tray_info[THD_WORK_TRANSFER].strMovePos[1], 
										st_tray_info[THD_WORK_TRANSFER].strMovePos[0], 
										1, 
										st_tray_info[THD_WORK_TRANSFER].strLogKey,
										st_tray_info[THD_WORK_TRANSFER].strLogData);
				}
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nRunStep = 1200;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�  
				CTL_Lib.Alarm_Error_Occurrence(4660, dWARNING, st_alarm_info.strCode);
				m_nRunStep = 1200;
			}
			break; 


		//////////////////////////////////////////////////////////////////////
		// �ش� �۾� ��ġ�� ���� pick & place �ϴ� ��ƾ 
		//////////////////////////////////////////////////////////////////////
		case 2000: //Ʈ������ �� üũ 
			nRet_1 = FAS_IO.get_in_bit(st_io_info.i_TrayTrsfUpChk,	IO_ON); //Ʈ�����۰� ��Ŀ�� ��� �ִ� ���� üũ
			nRet_2 = FAS_IO.get_in_bit(st_io_info.i_TrayTrsfDnChk,	IO_OFF); //james 2016.0921 
			if(nRet_1 == IO_ON && nRet_2 == IO_OFF)
			{ //��Ŀ�� ��� ���� 
				m_nRunStep = 2100;			
			}
			else 
			{
				m_nRunStep = 2010;			
			}
			break;

		case 2010://transfer up 
			Set_EmptyTrayTransfer_UpDown(IO_OFF); 
			m_nRunStep = 2011;
			break;

		case 2011:
			nRet_1 = Chk_EmptyTrayTransfer_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 2100;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(4670, dWARNING, m_strAlarmCode);
				m_nRunStep = 2000;
			}
			break;

		case 2100: //�۾� ��ġ�� �̵��ϴ� ��ƾ
			if(m_nTransferJobFlag[0] == THD_LD_TRAY_PLATE)
			{
				m_dpTargetPos = st_motor_info[m_nAxisNum].d_pos[P_WORKTRANS_LD_STACKER];
			}
			else if(m_nTransferJobFlag[0] == THD_EMPTY_STACKER)
			{
				m_dpTargetPos = st_motor_info[m_nAxisNum].d_pos[P_WORKTRANS_EMPTY_STACKER];
				///////////////////////////////////////////////////////////////////////
				//2017.0430
				if( st_handler_info.mn_auto_empty_tray == YES  && st_handler_info.mn_out_empty == YES )
				{
					m_dpTargetPos = st_motor_info[m_nAxisNum].d_pos[P_WORKTRANS_REJECT_STACK];
				}
				///////////////////////////////////////////////////////////////////////
			}
			else if(m_nTransferJobFlag[0] == THD_ULD_1_STACKER)
			{
				m_dpTargetPos = st_motor_info[m_nAxisNum].d_pos[P_WORKTRANS_ULD_1_STACKER];
				//20161203
				st_work_info.dwRearSmema[0][0] = GetTickCount();
			}
			else if(m_nTransferJobFlag[0] == THD_ULD_2_STACKER)
			{
				m_dpTargetPos = st_motor_info[m_nAxisNum].d_pos[P_WORKTRANS_ULD_2_STACKER];
				//20161203
				st_work_info.dwRearSmema[1][0] = GetTickCount();
			}
			nRet_1 = CTL_Lib.Single_Move(BOTH_MOVE_FINISH, m_nAxisNum, m_dpTargetPos, COMI.mn_runspeed_rate);  //left�� �̸� �̵��� �ִ´� 
			if (nRet_1 == BD_GOOD) //�������� �̵�  
			{			
				m_nRunStep = 3000;
			}
			else if (nRet_1 == BD_RETRY)
			{
				m_nRunStep = 2000;
			}
			else if (nRet_1 == BD_ERROR || nRet_1 == BD_SAFETY)
			{//���� �˶��� �̹� ó�������� �̰������� �� ���¸� �ٲٸ� �ȴ�  
				CTL_Lib.Alarm_Error_Occurrence(4680, dWARNING, st_alarm_info.strCode);
				m_nRunStep = 2000;
			}
			break; 

		case 3000: //���� �۾��ϴ� ����
			if(m_nTransferJobFlag[0] == THD_LD_TRAY_PLATE)
			{
				nRet_1 = COMI.Get_MotIOSensor(M_LD_ELV, MOT_SENS_SD); 	
				if(nRet_1 != BD_GOOD) //�δ� �÷���Ʈ�� Ʈ���̰� ���� �� ���� 
				{
					m_strAlarmCode.Format(_T("900001")); //900003 1 0 "LD_STACKER_PLATE_SD_TRAY_OFF_CHECK_ERROR" 
					CTL_Lib.Alarm_Error_Occurrence(4690, dWARNING, m_strAlarmCode);
					break;
				}
				//����Ŀ�� ���¸� üũ�� �ʿ䵵 ���� 					
			}
			else if(m_nTransferJobFlag[0] == THD_EMPTY_STACKER)
			{	 //����Ŀ�� ���¸� üũ�� �ʿ䵵 ���� 	
				if(m_nTransferJobFlag[1] == WORK_PICK)
				{
					// jtkim 20160902
					if(st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] == CTL_FREE) //empty���� Ʈ���̸� �����Ͽ� ��� �� �ְ� �Ѵ� 
					{
						//m_nRunStep = 1000;
						//kwlee 2017.0126
						if( st_handler_info.mn_removetray == CTL_CHANGE )//st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] = CTL_FREE;
						{
							st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] = CTL_CLEAR;
							m_nRunStep = 0;
						}
						else
						{
							m_nRunStep = 1000;
						}
						break;
					}

					if(st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] != CTL_READY) //empty���� Ʈ���̸� �����Ͽ� ��� �� �ְ� �Ѵ� 
					{//empty stacker���� Ʈ���� �����ϱ���� ��ٸ��� 
						break;
					}

					nRet_1 = COMI.Get_MotIOSensor(M_EMPTY_ELV, MOT_SENS_SD); 	
					if(nRet_1 != BD_GOOD) //�δ� �÷���Ʈ�� Ʈ���̰� ���� �� ���� 
					{
						m_strAlarmCode.Format(_T("900003")); //900003 1 0 "EMPTY_STACKER_PLATE_SD_TRAY_OFF_CHECK_ERROR"
						CTL_Lib.Alarm_Error_Occurrence(4700, dWARNING, m_strAlarmCode);
						break;
					}
				} 
				else if(m_nTransferJobFlag[1] == WORK_PLACE)
				{
					if(st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] != CTL_READY) //empty���� Ʈ���̸� ���� �� �� �غ�Ǿ����� ��ٸ��� 
					{//empty stacker���� Ʈ���� �����ϱ���� ��ٸ��� 
						break;
					}
				}
			}
			else if(m_nTransferJobFlag[0] == THD_ULD_1_STACKER)
			{ //����Ŀ�� ���¸� üũ�� �ʿ䵵 ���� 	
				if(st_sync_info.nWorkTransfer_Req[THD_ULD_1_STACKER][0] != CTL_REQ) //�� Ʈ���� ������ �C���߾�� �Ѵ� 
				{
					break;
				}
			}
			else if(m_nTransferJobFlag[0] == THD_ULD_2_STACKER)
			{
				 //����Ŀ�� ���¸� üũ�� �ʿ䵵 ���� 	
				if(st_sync_info.nWorkTransfer_Req[THD_ULD_2_STACKER][0] != CTL_REQ) //�� Ʈ���� ������ �C���߾�� �Ѵ� 
				{
					break;
				}
			}
			m_nRunStep = 3100;
			break;		

		case 3100:
			nRet_1 = COMI.Check_MotPosRange(m_nAxisNum, m_dpTargetPos, st_motor_info[m_nAxisNum].d_allow); 			
			if (nRet_1 != BD_GOOD)  //��� ������ ��ġ 
			{//��ġ�� ���� ������ �ٽ� Ȯ���Ͽ� ��������
				m_nRunStep = 2000;
				break;
			}

			Set_EmptyTrayTransfer_UpDown(IO_ON); //down 

			if(m_nTransferJobFlag[1] == WORK_PICK)
			{  //pick ���� , �̸����� 
				Set_EmptyTransferEjectorOnOff(IO_OFF);
				Set_EmptyTrayVacuum_OnOff(IO_ON); //vacuum�� �̸� Ű�� �������� 
			}
			else //if(m_nTransferJobFlag[1] == TRAY_PLACE)
			{  //place ���� 
				// ������ 
				//Set_EmptyTrayVacuum_OnOff(IO_OFF);
				//Set_EmptyTransferEjectorOnOff(IO_ON);
			}

			m_nRunStep = 3110;
			break;

		case 3110:
			nRet_1 = Chk_EmptyTrayTransfer_UpDown(IO_ON); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 3200;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(4710, dWARNING, m_strAlarmCode);
				m_nRunStep = 3100;
			}
			break;

		case 3200:
			if(m_nTransferJobFlag[1] == WORK_PICK)
			{  //pick ���� 
				Set_EmptyTransferEjectorOnOff(IO_OFF);
				Set_EmptyTrayVacuum_OnOff(IO_ON);
			}
			else //if(m_nTransferJobFlag[1] == TRAY_PLACE)
			{  //place ���� 
				Set_EmptyTrayVacuum_OnOff(IO_OFF);
				Set_EmptyTransferEjectorOnOff(IO_ON);
			}
			m_nRunStep = 3210;
			break;

		case 3210: //üũ
			if(m_nTransferJobFlag[1] == WORK_PICK)
			{  //pick ���� 				 
				nRet_1 = Chk_EmptyTrayVacuum_OnOff(IO_ON);
				if(nRet_1 == RET_GOOD)
				{
					m_nRunStep = 3300;
				}
				else if(nRet_1 == RET_ERROR)
				{
					Set_EmptyTrayTransfer_UpDown(IO_OFF); //up
					CTL_Lib.Alarm_Error_Occurrence(4720, dWARNING, m_strAlarmCode);
					m_nRunStep = 2000;
				}
			}
			else //if(m_nTransferJobFlag[1] == TRAY_PLACE)
			{  //place ���� 
				nRet_1 = Chk_EmptyTrayVacuum_OnOff(IO_OFF);
				if(nRet_1 == RET_GOOD)
				{
					Set_EmptyTransferEjectorOnOff(IO_OFF); //ejector off
					m_nRunStep = 3300;
				}
				else if(nRet_1 == RET_ERROR)
				{
					Set_EmptyTrayTransfer_UpDown(IO_OFF); //up
					CTL_Lib.Alarm_Error_Occurrence(4730, dWARNING, m_strAlarmCode);
					m_nRunStep = 2000;
				}
			} 
			break;

		case 3300: //�������� �� 
			Set_EmptyTrayTransfer_UpDown(IO_OFF); 
			m_nRunStep = 3310;
			break;

		case 3310:
			nRet_1 = Chk_EmptyTrayTransfer_UpDown(IO_OFF); 
			if(nRet_1 == RET_GOOD)
			{
				m_nRunStep = 3400;
			}
			else if(nRet_1 == RET_ERROR)
			{
				CTL_Lib.Alarm_Error_Occurrence(4740, dWARNING, m_strAlarmCode);
				m_nRunStep = 3300;
			}
			break;

		case 3400:
			if(m_nTransferJobFlag[1] == WORK_PICK)
			{  //pick ���� 				 
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_TrayTrsfVacChk,	IO_ON);  
				if(nRet_1 == IO_ON)
				{	//���� 					 
					m_nRunStep = 4000;
				}
				else
				{	//Ʈ���� �־�� �Ѵ� 
					m_strAlarmCode.Format(_T("8%d%04d"), IO_ON, st_io_info.i_TrayTrsfVacChk); 
					CTL_Lib.Alarm_Error_Occurrence(4750, dWARNING, m_strAlarmCode);
					break;
				}	
			}
			else //if(m_nTransferJobFlag[1] == TRAY_PLACE)
			{  //place ���� 
				nRet_1 = FAS_IO.get_in_bit(st_io_info.i_TrayTrsfVacChk,	IO_OFF); //CV tray pass check  ����� �Ѵ� 
				if(nRet_1 == IO_OFF)
				{//���� 					
					m_nRunStep = 4000;
				}
				else
				{
					//Ʈ���� ����� �Ѵ� 
					m_strAlarmCode.Format(_T("8%d%04d"), IO_OFF, st_io_info.i_TrayTrsfVacChk); 
					CTL_Lib.Alarm_Error_Occurrence(4760, dWARNING, m_strAlarmCode); 
				}	
			} 
			break;			

		case 4000: //Ʈ���� ���� ���� 
			if(m_nTransferJobFlag[1] == WORK_PICK)
			{
				if(m_nTransferJobFlag[0] == THD_LD_TRAY_PLATE)//Ʈ������ Ʈ���� ���� 
				{
					clsFunc.Handler_Tray_DataInfo_Shift(1, CTL_YES, THD_LD_TRAY_PLATE, THD_WORK_TRANSFER); //Ʈ���� ������ load plate -> transfer�� ����Ʈ �Ѵ� // Handler_Tray_DataInfo_Shift(int nMode, int nDvc_Yes_No, int nSend_SIte, int nRcv_Site)

					st_sync_info.nWorkTransfer_Req[THD_LD_TRAY_PLATE][0] = CTL_READY;
					st_tray_info[THD_WORK_TRANSFER].nTrayExist = CTL_YES;

					//2017.0109
					st_tray_info[THD_WORK_TRANSFER].strLogKey[0]	= _T("EMPRT_TRAY_TRANSFER");
					if(st_lot_info[LOT_CURR].strLotNo != _T("") && st_lot_info[LOT_CURR].nLotStatus >= LOT_START && st_lot_info[LOT_CURR].nLotStatus < LOT_END_START)
					{
						st_tray_info[THD_WORK_TRANSFER].strLogData[0]	= st_lot_info[LOT_CURR].strLotNo;
					}
					else if(st_lot_info[LOT_NEXT].strLotNo != _T("") && st_lot_info[LOT_NEXT].nLotStatus >= LOT_START && st_lot_info[LOT_NEXT].nLotStatus < LOT_END_START)
					{
						st_tray_info[THD_WORK_TRANSFER].strLogData[0]	= st_lot_info[LOT_NEXT].strLotNo;
					}
					else
					{
						st_tray_info[THD_WORK_TRANSFER].strLogData[0]	= _T("$");
					}

					st_tray_info[THD_WORK_TRANSFER].strMaterial = _T("$");
					st_tray_info[THD_WORK_TRANSFER].nStartStatus = 1;
					st_tray_info[THD_WORK_TRANSFER].strMovePos[0] = clsLog.GetDeviceID( THD_LD_TRAY_PLATE );
					st_tray_info[THD_WORK_TRANSFER].strMovePos[1] = clsLog.GetDeviceID( THD_EMPTY_STACKER );


					clsLog.LogTransfer(_T("EMPTY_TRAY_TRANSFER"),
										_T("MOVE_EMPTY_TRAY"),
										0,
										st_tray_info[THD_WORK_TRANSFER].strMaterial,
										_T("EMPTY_TRAY"),
										st_tray_info[THD_WORK_TRANSFER].strMovePos[0], 
										st_tray_info[THD_WORK_TRANSFER].strMovePos[1], 
										1, 
										st_tray_info[THD_WORK_TRANSFER].strLogKey,
										st_tray_info[THD_WORK_TRANSFER].strLogData);


				}
				else if(m_nTransferJobFlag[0] == THD_EMPTY_STACKER)
				{
					clsFunc.Handler_Tray_DataInfo_Shift(1, CTL_YES, THD_EMPTY_STACKER, THD_WORK_TRANSFER); //Ʈ���� ������ empty stacker -> transfer�� ����Ʈ �Ѵ� 
					st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] = CTL_FREE;
					st_tray_info[THD_WORK_TRANSFER].nTrayExist = CTL_YES;
				}
			}
			else //if(m_nTransferJobFlag[1] == TRAY_PLACE) //Ʈ������ Ʈ���� ���� 
			{				
				if(m_nTransferJobFlag[0] == THD_EMPTY_STACKER)
				{
// 					clsFunc.Handler_Tray_DataInfo_Shift(1, CTL_YES, THD_WORK_TRANSFER, THD_EMPTY_STACKER); //Ʈ���� ������ transfer -> empty stacker�� ����Ʈ �Ѵ� 
// 
// 					st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] = CTL_FREE; //���Ҵ� 
// 					st_tray_info[THD_WORK_TRANSFER].nTrayExist =  CTL_NO;

					////////////////////////////////////////////////////////////////////////////////////////////////////////
					//2017.0430
					if( st_handler_info.mn_auto_empty_tray == YES && st_handler_info.mn_out_empty == YES)
					{
						clsFunc.Handler_Tray_DataInfo_Shift(1, CTL_YES, THD_WORK_TRANSFER, THD_REJECT_BUFFER_STACK);

						st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] = CTL_FREE; //���Ҵ� 
						st_tray_info[THD_WORK_TRANSFER].nTrayExist =  CTL_NO;

						m_nEmptyToReject++;
						if( m_nEmptyToReject > 6 )//6�������� �̸� ������. �ѹ��� empty tray stacker�� ���� ���� �ִ�.
						{
							st_handler_info.mn_out_empty = NO;
							m_nEmptyToReject = 0;
						}
					}
					////////////////////////////////////////////////////////////////////////////////////////////////////////	
					else
					{
						clsFunc.Handler_Tray_DataInfo_Shift(1, CTL_YES, THD_WORK_TRANSFER, THD_EMPTY_STACKER); //Ʈ���� ������ transfer -> empty stacker�� ����Ʈ �Ѵ� 
						st_sync_info.nWorkTransfer_Req[THD_EMPTY_STACKER][0] = CTL_FREE; //���Ҵ� 
						st_tray_info[THD_WORK_TRANSFER].nTrayExist =  CTL_NO;	
					}
				}
				else if(m_nTransferJobFlag[0] == THD_ULD_1_STACKER)  
				{
					clsFunc.Handler_Tray_DataInfo_Shift(1, CTL_YES, THD_WORK_TRANSFER, THD_ULD_1_STACKER); //Ʈ���� ������ transfer -> uld 1�� ����Ʈ �Ѵ� 

					st_sync_info.nWorkTransfer_Req[THD_ULD_1_STACKER][0] = CTL_READY;
					st_tray_info[THD_WORK_TRANSFER].nTrayExist =  CTL_NO;

					//2016.1203
					st_work_info.dwRearSmema[0][1] = GetTickCount();

					//2017.0109
					st_tray_info[THD_WORK_TRANSFER].strLogKey[0]	= _T("EMPRT_TRAY_TRANSFER");
					if(st_lot_info[LOT_CURR].strLotNo != _T("") && st_lot_info[LOT_CURR].nLotStatus >= LOT_START && st_lot_info[LOT_CURR].nLotStatus < LOT_END_START)
					{
						st_tray_info[THD_WORK_TRANSFER].strLogData[0]	= st_lot_info[LOT_CURR].strLotNo;
					}
					else if(st_lot_info[LOT_NEXT].strLotNo != _T("") && st_lot_info[LOT_NEXT].nLotStatus >= LOT_START && st_lot_info[LOT_NEXT].nLotStatus < LOT_END_START)
					{
						st_tray_info[THD_WORK_TRANSFER].strLogData[0]	= st_lot_info[LOT_NEXT].strLotNo;
					}
					else
					{
						st_tray_info[THD_WORK_TRANSFER].strLogData[0]	= _T("$");
					}

					st_tray_info[THD_WORK_TRANSFER].strMaterial = _T("$");
					st_tray_info[THD_WORK_TRANSFER].nStartStatus = 1;
					st_tray_info[THD_WORK_TRANSFER].strMovePos[0] = clsLog.GetDeviceID( THD_EMPTY_STACKER );
					st_tray_info[THD_WORK_TRANSFER].strMovePos[1] = clsLog.GetDeviceID( THD_ULD_1_STACKER );

					clsLog.LogTransfer(_T("EMPTY_TRAY_TRANSFER"),
										_T("MOVE_EMPTY_TRAY"),
										0,
										st_tray_info[THD_WORK_TRANSFER].strMaterial,
										_T("EMPTY_TRAY"),
										st_tray_info[THD_WORK_TRANSFER].strMovePos[0], 
										st_tray_info[THD_WORK_TRANSFER].strMovePos[1], 
										1, 
										st_tray_info[THD_WORK_TRANSFER].strLogKey,
										st_tray_info[THD_WORK_TRANSFER].strLogData);
				}
				else if(m_nTransferJobFlag[0] == THD_ULD_2_STACKER) //�̰����� ���� ���� ���� ���� 
				{
					clsFunc.Handler_Tray_DataInfo_Shift(1, CTL_YES, THD_WORK_TRANSFER, THD_ULD_2_STACKER); //Ʈ���� ������ transfer -> uld 2 �� ����Ʈ �Ѵ� 

					st_sync_info.nWorkTransfer_Req[THD_ULD_2_STACKER][0] = CTL_READY;
					st_tray_info[THD_WORK_TRANSFER].nTrayExist =  CTL_NO;

					//2016.1203
					st_work_info.dwRearSmema[1][1] = GetTickCount();

					//2017.0109
					st_tray_info[THD_WORK_TRANSFER].strLogKey[0]	= _T("EMPRT_TRAY_TRANSFER");
					if(st_lot_info[LOT_CURR].strLotNo != _T("") && st_lot_info[LOT_CURR].nLotStatus >= LOT_START && st_lot_info[LOT_CURR].nLotStatus < LOT_END_START)
					{
						st_tray_info[THD_WORK_TRANSFER].strLogData[0]	= st_lot_info[LOT_CURR].strLotNo;
					}
					else if(st_lot_info[LOT_NEXT].strLotNo != _T("") && st_lot_info[LOT_NEXT].nLotStatus >= LOT_START && st_lot_info[LOT_NEXT].nLotStatus < LOT_END_START)
					{
						st_tray_info[THD_WORK_TRANSFER].strLogData[0]	= st_lot_info[LOT_NEXT].strLotNo;
					}
					else
					{
						st_tray_info[THD_WORK_TRANSFER].strLogData[0]	= _T("$");
					}

					st_tray_info[THD_WORK_TRANSFER].strMaterial = _T("$");
					st_tray_info[THD_WORK_TRANSFER].nStartStatus = 1;
					st_tray_info[THD_WORK_TRANSFER].strMovePos[0] = clsLog.GetDeviceID( THD_EMPTY_STACKER );
					st_tray_info[THD_WORK_TRANSFER].strMovePos[1] = clsLog.GetDeviceID( THD_ULD_2_STACKER );

					clsLog.LogTransfer(_T("EMPTY_TRAY_TRANSFER"),
										_T("MOVE_EMPTY_TRAY"),
										0,
										st_tray_info[THD_WORK_TRANSFER].strMaterial,
										_T("EMPTY_TRAY"),
										st_tray_info[THD_WORK_TRANSFER].strMovePos[0], 
										st_tray_info[THD_WORK_TRANSFER].strMovePos[1], 
										1, 
										st_tray_info[THD_WORK_TRANSFER].strLogKey,
										st_tray_info[THD_WORK_TRANSFER].strLogData);
				}	
			}
			m_nRunStep = 1000; 			 
			break;

		default:
			break;
	}
}




void CRunEmptyTrayTransfer::Set_EmptyTrayTransfer_UpDown( int nOnOff )
{

	CString strLogKey[10];
	CString	strLogData[10];

	strLogKey[0] = _T("Mode Start");
	strLogData[0].Format(_T("%d"),0);

	m_bTransferUpDownFlag = false;
	m_dwWaitTransferUpDown[0]	= GetCurrentTime();
	if(nOnOff == IO_OFF) //up
	{
		FAS_IO.set_out_bit( st_io_info.o_TrayTrsfUpCyl, IO_ON);
		FAS_IO.set_out_bit( st_io_info.o_TrayTrsfDnCyl, IO_OFF);
	}
	else //if(nOnOff == IO_ON)//down
	{
		FAS_IO.set_out_bit( st_io_info.o_TrayTrsfUpCyl, IO_OFF);
		FAS_IO.set_out_bit( st_io_info.o_TrayTrsfDnCyl, IO_ON);
	}

	

	///kwlee 2016.0812
	if (nOnOff == IO_ON)
	{
		//LogFunction(_T("EMPTY_TRAY_TRANSFER"),_T("UP"),_T("TRAY_TRANSFER"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("EMPTY_TRAY_TRANSFER"),_T("UP"),0,_T("TRAY_TRANSFER"),_T("CYLINDER"),1,strLogKey,strLogData);
	}
	else
	{
		//LogFunction(_T("EMPTY_TRAY_TRANSFER"),_T("DOWN"),_T("TRAY_TRANSFER"),_T("CYLINDER"),1,strLogKey,strLogData);
		//2017.0109
		clsLog.LogFunction(_T("EMPTY_TRAY_TRANSFER"),_T("DOWN"),0,_T("TRAY_TRANSFER"),_T("CYLINDER"),1,strLogKey,strLogData);
	}

}

int	CRunEmptyTrayTransfer::Chk_EmptyTrayTransfer_UpDown( int nOnOff )
{
//800408 1 0 "TRAY_TRANSFER_TRAY_UP_OFF_CHECK_ERROR"
//800409 1 0 "TRAY_TRANSFER_TRAY_DOWN_OFF_CHECK_ERROR"
//810408 1 0 "TRAY_TRANSFER_TRAY_UP_ON_CHECK_ERROR"
//810409 1 0 "TRAY_TRANSFER_TRAY_DOWN_ON_CHECK_ERROR"
	int nWaitTime = WAIT_TRANSFER_UP_DN;

	CString strLogKey[10];
	CString	strLogData[10];


	strLogKey[0] = _T("Mode End");
	strLogData[0].Format(_T("%d"),0);

	if (nOnOff == IO_OFF)//UP
	{
		if (m_bTransferUpDownFlag == false &&
			FAS_IO.get_in_bit(st_io_info.i_TrayTrsfUpChk, IO_ON) == IO_ON &&
			FAS_IO.get_in_bit(st_io_info.i_TrayTrsfDnChk, IO_OFF) == IO_OFF)
		{
			m_bTransferUpDownFlag = true;
			m_dwWaitTransferUpDown[0] = GetCurrentTime();
		}
		else if (m_bTransferUpDownFlag == true &&
			FAS_IO.get_in_bit(st_io_info.i_TrayTrsfUpChk, IO_ON) == IO_ON &&
			FAS_IO.get_in_bit(st_io_info.i_TrayTrsfDnChk, IO_OFF) == IO_OFF)
		{
			m_dwWaitTransferUpDown[1] = GetCurrentTime();
			m_dwWaitTransferUpDown[2] = m_dwWaitTransferUpDown[1] - m_dwWaitTransferUpDown[0];

			if (m_dwWaitTransferUpDown[2] <= 0)
			{
				m_dwWaitTransferUpDown[0] = GetCurrentTime();
				return RET_PROCEED;
			}

			if(m_dwWaitTransferUpDown[2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
			{
				//LogFunction(_T("EMPTY_TRAY_TRANSFER"),_T("UP"),_T("TRAY_TRANSFER"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("EMPTY_TRAY_TRANSFER"),_T("UP"),1,_T("TRAY_TRANSFER"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_GOOD;
			}
		}
		else
		{
			m_dwWaitTransferUpDown[1] = GetCurrentTime();
			m_dwWaitTransferUpDown[2] = m_dwWaitTransferUpDown[1] - m_dwWaitTransferUpDown[0];

			if (m_dwWaitTransferUpDown[2] <= 0)
			{
				m_dwWaitTransferUpDown[0]	= GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwWaitTransferUpDown[2] > (DWORD)st_wait_info.nLimitWaitTime[nWaitTime])
			{
				//m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_TrayTrsfUpChk); 
				//2017.0109
				clsLog.LogFunction(_T("EMPTY_TRAY_TRANSFER"),_T("UP"),1,_T("TRAY_TRANSFER"),_T("CYLINDER"),1,strLogKey,strLogData);
				return RET_ERROR;
			}
		}
		
	}
	else
	{
		if (m_bTransferUpDownFlag == false &&
			FAS_IO.get_in_bit(st_io_info.i_TrayTrsfUpChk, IO_OFF) == IO_OFF &&
			FAS_IO.get_in_bit(st_io_info.i_TrayTrsfDnChk, IO_ON) == IO_ON)
		{
			m_bTransferUpDownFlag		= true;
			m_dwWaitTransferUpDown[0]	= GetCurrentTime();
		}
		else if (m_bTransferUpDownFlag == true &&
			FAS_IO.get_in_bit(st_io_info.i_TrayTrsfUpChk, IO_OFF) == IO_OFF &&
			FAS_IO.get_in_bit(st_io_info.i_TrayTrsfDnChk, IO_ON) == IO_ON)
		{
			m_dwWaitTransferUpDown[1] = GetCurrentTime();
			m_dwWaitTransferUpDown[2] = m_dwWaitTransferUpDown[1] - m_dwWaitTransferUpDown[0];

			if (m_dwWaitTransferUpDown[2] <= 0)
			{
				m_dwWaitTransferUpDown[0] = GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwWaitTransferUpDown[2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
			{
				//LogFunction(_T("EMPTY_TRAY_TRANSFER"),_T("DOWN"),_T("TRAY_TRANSFER"),_T("CYLINDER"),1,strLogKey,strLogData); //kwlee 2016.0812
				//2017.0109
				clsLog.LogFunction(_T("EMPTY_TRAY_TRANSFER"),_T("DOWN"),1,_T("TRAY_TRANSFER"),_T("CYLINDER"),1,strLogKey,strLogData); 
				return RET_GOOD;
			}
		}
		else//DOWN
		{
			m_dwWaitTransferUpDown[1] = GetCurrentTime();
			m_dwWaitTransferUpDown[2] = m_dwWaitTransferUpDown[1] - m_dwWaitTransferUpDown[0];

			if (m_dwWaitTransferUpDown[2] <= 0)
			{
				m_dwWaitTransferUpDown[0] = GetCurrentTime();
				return RET_PROCEED;
			}

			if (m_dwWaitTransferUpDown[2] > (DWORD)st_wait_info.nLimitWaitTime[nWaitTime])
			{
				m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_TrayTrsfDnChk); 
				//2017.0109
				clsLog.LogFunction(_T("EMPTY_TRAY_TRANSFER"),_T("DOWN"),1,_T("TRAY_TRANSFER"),_T("CYLINDER"),1,strLogKey,strLogData); 
				return RET_ERROR;
			}
		}
	}

	return RET_PROCEED;
}

void CRunEmptyTrayTransfer::Set_EmptyTransferEjectorOnOff( int nOnOff )
{

	FAS_IO.set_out_bit(st_io_info.o_TrayTrsfEjtOnOff, nOnOff);
}

void CRunEmptyTrayTransfer::Set_EmptyTrayVacuum_OnOff( int nOnOff )
{
	m_bVacuumOnOffFlag = false;
	m_dwWaitVacuulOnOff[0]	= GetCurrentTime();

	if( nOnOff == IO_ON )
	{
		FAS_IO.set_out_bit( st_io_info.o_TrayTrsfVacOnOff, IO_ON);
		FAS_IO.set_out_bit( st_io_info.o_TrayTrsfEjtOnOff, IO_OFF);
	}
	else
	{
		FAS_IO.set_out_bit( st_io_info.o_TrayTrsfVacOnOff, IO_OFF);
		FAS_IO.set_out_bit( st_io_info.o_TrayTrsfEjtOnOff, IO_ON);
	}


}

//	800410 1 0 "TRAY_TRANSFER_TRAY_VACUUM_ON_OFF_CHECK_ERROR"
//	810410 1 0 "TRAY_TRANSFER_TRAY_VACUUM_ON_CHECK_ERROR"
int	CRunEmptyTrayTransfer::Chk_EmptyTrayVacuum_OnOff( int nOnOff )
{
	int nWaitTime= WAIT_TRANSFER_VACUUM_ON_OFF;


	if (nOnOff == IO_OFF)
	{
		if (st_basic_info.nModeDevice == WITHOUT_TRAY || 1)
		{
			if (m_bVacuumOnOffFlag == false)
			{
				m_bVacuumOnOffFlag		= true;
				m_dwWaitVacuulOnOff[0]	= GetCurrentTime();
			}
			else if (m_bVacuumOnOffFlag == true)
			{
				m_dwWaitVacuulOnOff[1] = GetCurrentTime();
				m_dwWaitVacuulOnOff[2] = m_dwWaitVacuulOnOff[1] - m_dwWaitVacuulOnOff[0];

				if (m_dwWaitVacuulOnOff[2] <= 0)
				{
					m_dwWaitVacuulOnOff[0] = GetCurrentTime();
					return RET_PROCEED;
				}

				if (m_dwWaitVacuulOnOff[2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
				{
					return RET_GOOD;
				}
			}

		}
		else
		{
			if (m_bVacuumOnOffFlag == false && FAS_IO.get_in_bit(st_io_info.i_TrayTrsfVacChk,	IO_OFF)	== IO_OFF)
			{
				m_bVacuumOnOffFlag		= true;
				m_dwWaitVacuulOnOff[0]	= GetCurrentTime();
			}
			else if (m_bVacuumOnOffFlag == true && FAS_IO.get_in_bit(st_io_info.i_TrayTrsfVacChk,	IO_OFF)	== IO_OFF)
			{
				m_dwWaitVacuulOnOff[1] = GetCurrentTime();
				m_dwWaitVacuulOnOff[2] = m_dwWaitVacuulOnOff[1] - m_dwWaitVacuulOnOff[0];

				if (m_dwWaitVacuulOnOff[2] <= 0)
				{
					m_dwWaitVacuulOnOff[0] = GetCurrentTime();
					return RET_PROCEED;
				}

				if (m_dwWaitVacuulOnOff[2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
				{
					return RET_GOOD;
				}
			}
			else
			{
				m_dwWaitVacuulOnOff[1] = GetCurrentTime();
				m_dwWaitVacuulOnOff[2] = m_dwWaitVacuulOnOff[1] - m_dwWaitVacuulOnOff[0];

				if (m_dwWaitVacuulOnOff[2] <= 0)
				{
					m_dwWaitVacuulOnOff[0] = GetCurrentTime();
					return RET_PROCEED;
				}

				if (m_dwWaitVacuulOnOff[2] > (DWORD)st_wait_info.nLimitWaitTime[nWaitTime])
				{
					m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_TrayTrsfVacChk);
					return RET_ERROR;
				}
			}
		}
	}
	else
	{
		if (st_basic_info.nModeDevice == WITH_DVC || 1)
		{
			if (m_bVacuumOnOffFlag == false && FAS_IO.get_in_bit(st_io_info.i_TrayTrsfVacChk,	IO_ON)	== IO_ON)
			{
				m_bVacuumOnOffFlag		= true;
				m_dwWaitVacuulOnOff[0]	= GetCurrentTime();
			}
			else if (m_bVacuumOnOffFlag == true && FAS_IO.get_in_bit(st_io_info.i_TrayTrsfVacChk,	IO_ON)	== IO_ON)
			{
				m_dwWaitVacuulOnOff[1]	= GetCurrentTime();
				m_dwWaitVacuulOnOff[2]	= m_dwWaitVacuulOnOff[1] - m_dwWaitVacuulOnOff[0];

				if (m_dwWaitVacuulOnOff[2] <= 0)
				{
					m_dwWaitVacuulOnOff[0]	= GetCurrentTime();
					return RET_PROCEED;
				}

				if(m_dwWaitVacuulOnOff[2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
				{
					return RET_GOOD;
				}
			}
			else
			{
				m_dwWaitVacuulOnOff[1]	= GetCurrentTime();
				m_dwWaitVacuulOnOff[2]	= m_dwWaitVacuulOnOff[1] - m_dwWaitVacuulOnOff[0];

				if (m_dwWaitVacuulOnOff[2] <= 0)
				{
					m_dwWaitVacuulOnOff[0]	= GetCurrentTime();
					return RET_PROCEED;
				}

				if (m_dwWaitVacuulOnOff[2] > (DWORD)st_wait_info.nLimitWaitTime[nWaitTime])
				{
					m_strAlarmCode.Format(_T("8%d%04d"), nOnOff, st_io_info.i_TrayTrsfVacChk);
					return RET_ERROR;
				}
			}
		}
		else
		{
			if (m_bVacuumOnOffFlag == false)
			{
				m_bVacuumOnOffFlag			= true;
				m_dwWaitVacuulOnOff[0]	= GetCurrentTime();
			}
			else if (m_bVacuumOnOffFlag == true)
			{
				m_dwWaitVacuulOnOff[1]	= GetCurrentTime();
				m_dwWaitVacuulOnOff[2]	= m_dwWaitVacuulOnOff[1] - m_dwWaitVacuulOnOff[0];


				if (m_dwWaitVacuulOnOff[2] <= 0)
				{
					m_dwWaitVacuulOnOff[0]	= GetCurrentTime();
					return RET_PROCEED;
				}

				else if(m_dwWaitVacuulOnOff[2] > (DWORD)st_wait_info.nOnWaitTime[nWaitTime])
				{
					RET_GOOD;
				}
			}
		}
	}

	return RET_PROCEED;
}