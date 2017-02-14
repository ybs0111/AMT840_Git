// FastechPublic_IO.cpp: implementation of the CFastechPublic_IO class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FastechPublic_IO.h"
#include "math.h"
#include "Variable.h"
#include "PublicFunction.h"
//#include "io.h"     // 파일 존재 유무 검사 함수 호출하기 위해서는 반드시 INCLUDE 필요 
// jtkim 20130625
#ifdef _WIN64
	#include ".\\64bit\\FAS_HSSI.h"
#else
	#include ".\\32bit\\FAS_HSSI.h"
#endif
#include "AlgMemory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CFastechPublic_IO  FAS_IO;  

CFastechPublic_IO::CFastechPublic_IO()
{
}

CFastechPublic_IO::~CFastechPublic_IO()
{

}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
//2011.1019 james lee WORD 단위로 I/O를 제어한다
//Ver 0.0.1_2011.1019
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////


//2011.1019 james //Word 단위로 제어시 사용하는 함수 
BOOL CFastechPublic_IO::get_in_bit_wait(int nNum, int nIO, int nOnOff, int nTime)  //2011.1019 Simulation 동작구현 목적으로 onoff 추가 
{
	if(st_work_info.nSimulationMode == 1) //Simulation 모드이면 프로그램이 동작해야 하니 I/O 상태를 원하는 것으로 셋팅해 준다 
	{		
		return n_in[nIO] = nOnOff;
	}

	m_dwWaitTime[nNum][nIO][1] = GetTickCount();
	m_dwWaitTime[nNum][nIO][2] = m_dwWaitTime[nNum][nIO][1] -  m_dwWaitTime[nNum][nIO][0];

	if (m_dwWaitTime[nNum][nIO][2] <= (DWORD)0 || m_dwWaitTime[nNum][nIO][0] <= (DWORD)0)
	{
		m_nWaitIO[nNum][nIO]		= n_in[nIO];		// jtkim 20160125
		m_dwWaitTime[nNum][nIO][0]	= GetTickCount();

		return -1;
	}

	if (m_bWaitRetry[nNum][nIO])
	{
		m_dwWaitTime[nNum][nIO][0]	= 0;
		m_bWaitRetry[nNum][nIO]		= FALSE;

		return n_in[nIO]; 
	}
	else
	{
		if (m_dwWaitTime[nNum][nIO][2] > (DWORD)nTime)
		{
			if (m_nWaitIO[nNum][nIO] == n_in[nIO])
			{
				m_dwWaitTime[nNum][nIO][0] = 0;

				return n_in[nIO]; 
			}
			m_bWaitRetry[nNum][nIO] = TRUE;
		}
	}

	return -1;
}


//2011.1019 james //Word 단위로 제어시 사용하는 함수 
BOOL CFastechPublic_IO::get_in_bit(int n_IONo, int n_OnOff)  //2011.1019 Simulation 동작구현 목적으로 onoff 추가 
{
/*
	CString sMsg;
	int     nRet=0;
	INT     nMasterID=0, nPortNo=0, nSlaveNo=0, nBitNo = 0;
	BOOL    bData=0;
*/
	if(st_work_info.nSimulationMode == 1) //Simulation 모드이면 프로그램이 동작해야 하니 I/O 상태를 원하는 것으로 셋팅해 준다 
	{		
		return n_out[n_IONo] = n_OnOff;
	}

	return n_in[n_IONo]; 
}

int CFastechPublic_IO::set_out_bit(int n_IONo, BOOL b_OnOff) 
{
/*
	CString sMsg;
	int     nRet=0;
	INT     nMasterID=0, nPortNo=0, nSlaveNo=0, nBitNo=0;
*/

	n_out[n_IONo] = b_OnOff;
//	return HSSI_STATUS_SUCCESS;
	//if (n_IONo == 2)//304)
	//{
	//	return STATUS_SUCCESS;
	//}
// jtkim 20130625
#ifdef _WIN64
	return HSSI_STATUS_SUCCESS;
#else
	return STATUS_SUCCESS;
#endif


	//이하는 사용하지 않는다 
	/*
	nMasterID = Search_IO_BoardInformation(0, n_IONo);  // HSSI 보드 데이터 확인 및 검사 함수
	nPortNo   = Search_IO_BoardInformation(1, n_IONo);
	nSlaveNo  = Search_IO_BoardInformation(2, n_IONo);
	nBitNo    = Search_IO_BoardInformation(3, n_IONo);

	nRet = HSSI_WriteOutputBit(nMasterID, nPortNo, nSlaveNo, nBitNo, b_OnOff);  // SLAVE I/O 모듈 출력 신호 발생 함수 [BIT 단위]
	if (nRet == STATUS_SUCCESS)
	{
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
	
	return nRet;*/
}

BOOL CFastechPublic_IO::get_out_bit(int n_IONo, int n_OnOff)
{
/*
	CString sMsg;
	int     nRet=0;	
	INT     nMasterID=0, nPortNo=0, nSlaveNo=0,nBitNo=0;
	BOOL    bData;
*/

	if(st_work_info.nSimulationMode == 1) //Simulation 모드이면 프로그램이 동작해야 하니 I/O 상태를 원하는 것으로 셋팅해 준다 
	{		
		return n_out[n_IONo] = n_OnOff;
	}

	return n_out[n_IONo];
	//이하는 사용하지 않는다 
	/*
	nMasterID = Search_IO_BoardInformation(0, n_IONo);  // HSSI 보드 데이터 확인 및 검사 함수
	nPortNo   = Search_IO_BoardInformation(1, n_IONo);
	nSlaveNo  = Search_IO_BoardInformation(2, n_IONo);
	nBitNo    = Search_IO_BoardInformation(3, n_IONo);
	
	nRet = HSSI_ReadOutputBit(nMasterID, nPortNo, nSlaveNo, nBitNo, &bData);  // SLAVE I/O 모듈 입력 신호 READ 함수 [BIT 단위]
	if (nRet == STATUS_SUCCESS)
	{
	}
	else
	{
		bData = BD_ERROR;
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}	
	return bData;*/
}

int CFastechPublic_IO::IO_ReadWrite(int n_bd_num, int n_port_num, int n_slave_num, int n_rw_mode)
{//n_rw_mode : -1:변수값 초기화, 0=> initialize i/o 상태 read, 1:input, 2:output
	WORD	wData = 0;
	int     i=0, nRet=0, nBoardID=-1, nPortNo=0, nSlaveNo=0, nIO_Num = 0;
 
//#define MAX_PCI_SLOT 8	// 0~7 8개
//#define MAX_PORT_NO 4	// 0~3 4개
//#define MAX_SLAVE_NO 8	// 0~7 8개


	//보드 정보를 이용하여 io port 번호로 변경한다 
	if(n_bd_num < 0 || n_bd_num > MAX_PCI_SLOT - 6) //MAX_PCI_SLOT - 1)
	{//	보드는 실제 8장까지 사용가능하나 당사는 MAX를 2장으로 한다 
		AfxMessageBox(_T("IO Board [0 ~ 1] Max Num Over"));//[0 ~ 7] Max Num Over");
		return BD_ERROR;
	}
	else nBoardID = n_bd_num * 4000;

	if(n_port_num < 0 || n_port_num > MAX_PORT_NO - 1)
	{
		AfxMessageBox(_T("IO Board Port [0 ~ 3] Max Num Over"));
		return BD_ERROR;
	}
	else nPortNo   = n_port_num * 1000;

	if(n_slave_num < 0 || n_slave_num > MAX_SLAVE_NO - 1)
	{
		AfxMessageBox(_T("IO Board Slave [0 ~ 7] Max Num Over"));
		return BD_ERROR;
	}
	else nSlaveNo   = n_slave_num * 100;
	
	
    nIO_Num = nBoardID + nPortNo + nSlaveNo;
	if(nIO_Num > HSSI_MAX_IO - 1) //#define HSSI_MAX_IO		8716 //max io board 2장 까지만 선언하여 사용하자 //max io board 4장 까지만 처리한다 
	{
		AfxMessageBox(_T("IO Num [8715] Max Num Over"));
		return BD_ERROR;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//장비 I/O port를 define 및 통신포트 초기화 후 I/O 실제 제어전에 각 포트의 상태를 읽어 변수에 셋팅하여 준다 
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if(n_rw_mode == -1) //input, output port 사용유무 처음 한번 초기화 할 것 
	{
		for(i=0; i<HSSI_MAX_IO; i++) //포트 사용 유무 초기화 
		{
			n_in[i]  = 0;//-1;
			n_out[i] = -1;
		}

		////////////////////////////////////////////////////////////////
		// 아래 내용은 AMT8312 I/O define 관련 정의 내용에 따라 변수를 셋팅했다 
		// 이 부분은 장비별로 틀리니 셋팅이 틀요할 것 임 
		////////////////////////////////////////////////////////////////

		//output 사용하는 것은 0 으로 재 초기화 하여 사용가능하게 한다 
		n_out[0]=0; n_out[1]=0;  n_out[15]=0;	//slave 0 0x8003);
		n_out[0]=0; n_out[1]=0;   //			//slave 1 0x0003)
		n_out[0]=0; n_out[1]=0; n_out[2]=0; n_out[3]=0; //slave 2 0x000f)
		n_out[0]=0; n_out[1]=0; n_out[2]=0; n_out[3]=0; //slave 3 0x000f)
		n_out[0]=0; n_out[1]=0; n_out[2]=0; n_out[3]=0; n_out[4]=0; n_out[5]=0; n_out[6]=0; n_out[7]=0; //slave 4 0x00ff)
		n_out[0]=0; n_out[1]=0; n_out[2]=0; n_out[3]=0; n_out[4]=0; n_out[5]=0; n_out[6]=0; n_out[7]=0; n_out[8]=0; n_out[9]=0; //slave 5 0x03ff)
		n_out[0]=0; n_out[1]=0;   //slave 6 0x0003)
		n_out[0]=0; n_out[1]=0; n_out[2]=0; n_out[3]=0; n_out[4]=0; n_out[5]=0; n_out[6]=0; n_out[7]=0; n_out[8]=0; n_out[9]=0; //slave 7 0x03ff)

	}

	if(st_work_info.nSimulationMode == 1) //Simulation 모드이면 프로그램이 동작해야 하니 I/O 상태를 원하는 것으로 셋팅해 준다 
	{//장비없이 스뮬레이션 모드로 프로그램 테스트시 밑으로 내려가 읽으면 변수가 클리어 되니 리턴한다 
		return BD_GOOD; 
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// 장비 초기화시 처음한번 Input 과 output port의 상태를 받는다 
	//////////////////////////////////////////////////////////////////////////////////////////////////
	if(n_rw_mode == 0) //initialize total input/ output read mode 
	{
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		//선된 변수 최초 한번 초기화
		// in/out 변수가 -1이면 사용하지 않는 포트이고, 0 또는 1이면 정상적으로 장비에서 시용하는 변수이다 
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		

		// ************************************************************************** //
		// 입력(모듈 번호 00번)								                          //
		// ************************************************************************** //
		nRet = HSSI_ReadInputWord(n_bd_num, n_port_num, n_slave_num, &wData); //(iMasterID, iPortNo, iSlaveNo, &wData); 
//		if(nRet != HSSI_STATUS_SUCCESS){return BD_ERROR;};//error
// jtkim 20130625
#ifdef _WIN64
		if(nRet != HSSI_STATUS_SUCCESS){return BD_ERROR;};//error
#else
		if(nRet != STATUS_SUCCESS){return BD_ERROR;};//error
#endif
		n_in[nIO_Num + 0]	= (wData & DBIT0)   ? 1 : 0 ;		// PS000 (모듈: 0	포트: 0) 
		n_in[nIO_Num + 1]	= (wData & DBIT1)   ? 1 : 0 ;		// PS001 (모듈: 0	포트: 1) 
		n_in[nIO_Num + 2]	= (wData & DBIT2)   ? 1 : 0 ;		// PS002 (모듈: 0	포트: 2) 
		n_in[nIO_Num + 3]	= (wData & DBIT3)   ? 1 : 0 ;		// PS003 (모듈: 0	포트: 3) 
		n_in[nIO_Num + 4]	= (wData & DBIT4)   ? 1 : 0 ;		// PS004 (모듈: 0	포트: 4)
		n_in[nIO_Num + 5]	= (wData & DBIT5)   ? 1 : 0 ;		// PS005 (모듈: 0	포트: 5) 
		n_in[nIO_Num + 6]	= (wData & DBIT6)   ? 1 : 0 ;		// PS006 (모듈: 0	포트: 6) 
		n_in[nIO_Num + 7]	= (wData & DBIT7)   ? 1 : 0 ;		// PS007 (모듈: 0	포트: 7) 
		n_in[nIO_Num + 8]	= (wData & DBIT8)   ? 1 : 0 ;		// PS008 (모듈: 0	포트: 8) 
		n_in[nIO_Num + 9]	= (wData & DBIT9)   ? 1 : 0 ;		// PS009 (모듈: 0	포트: 9) 
		n_in[nIO_Num + 10]	= (wData & DBIT10)  ? 1 : 0 ;		// PS010 (모듈: 0	포트: 10) 
		n_in[nIO_Num + 11]	= (wData & DBIT11)  ? 1 : 0 ;		// PS011 (모듈: 0	포트: 11) 
		n_in[nIO_Num + 12]	= (wData & DBIT12)  ? 1 : 0 ;		// PS012 (모듈: 0	포트: 12) 
		n_in[nIO_Num + 13]	= (wData & DBIT13)  ? 1 : 0 ;		// PS013 (모듈: 0	포트: 13) 
		n_in[nIO_Num + 14]	= (wData & DBIT14)  ? 1 : 0 ;		// PS014 (모듈: 0	포트: 14) 
		n_in[nIO_Num + 15]	= (wData & DBIT15)  ? 1 : 0 ;		// PS015 (모듈: 0	포트: 15) 

		// ************************************************************************** //
		// 출력(모듈 번호 00번)								                          //
		// ************************************************************************** //
		nRet = HSSI_ReadOutputWord(n_bd_num, n_port_num, n_slave_num, &wData); //(iMasterID, iPortNo, iSlaveNo, &wData); 
// jtkim 20130625
//		if(nRet != HSSI_STATUS_SUCCESS){return BD_ERROR;};//error
#ifdef _WIN64
		if(nRet != HSSI_STATUS_SUCCESS){return BD_ERROR;};//error
#else
		if(nRet != STATUS_SUCCESS){return BD_ERROR;};//error
#endif
		for(i=0; i<16; i++)//io port 16개
		{
			if(n_out[nIO_Num + i] == 0 || n_out[nIO_Num + i] == 1) //0:off, 1:on
			{
				if     (i == 0)	n_out[nIO_Num + 0]		= (wData & DBIT0)   ? 1 : 0 ;		// S000 (모듈: 0	포트: 0) 
				else if(i == 1)	n_out[nIO_Num + 1]		= (wData & DBIT1)   ? 1 : 0 ;		// S001 (모듈: 0	포트: 1) 
				else if(i == 2) n_out[nIO_Num + 2]		= (wData & DBIT2)   ? 1 : 0 ;		// S002 (모듈: 0	포트: 2) 
				else if(i == 3) n_out[nIO_Num + 3]		= (wData & DBIT3)   ? 1 : 0 ;		// S003 (모듈: 0	포트: 3) 
				else if(i == 4) n_out[nIO_Num + 4]		= (wData & DBIT4)   ? 1 : 0 ;		// S004 (모듈: 0	포트: 4)
				else if(i == 5) n_out[nIO_Num + 5]		= (wData & DBIT5)   ? 1 : 0 ;		// S005 (모듈: 0	포트: 5) 
				else if(i == 6) n_out[nIO_Num + 6]		= (wData & DBIT6)   ? 1 : 0 ;		// S006 (모듈: 0	포트: 6) 
				else if(i == 7) n_out[nIO_Num + 7]		= (wData & DBIT7)   ? 1 : 0 ;		// S007 (모듈: 0	포트: 7) 
				else if(i == 8) n_out[nIO_Num + 8]		= (wData & DBIT8)   ? 1 : 0 ;		// S008 (모듈: 0	포트: 8) 
				else if(i == 9) n_out[nIO_Num + 9]		= (wData & DBIT9)   ? 1 : 0 ;		// S009 (모듈: 0	포트: 9) 
				else if(i == 10) n_out[nIO_Num + 10]	= (wData & DBIT10)  ? 1 : 0 ;		// S010 (모듈: 0	포트: 10) 
				else if(i == 11) n_out[nIO_Num + 11]	= (wData & DBIT11)  ? 1 : 0 ;		// S011 (모듈: 0	포트: 11) 
				else if(i == 12) n_out[nIO_Num + 12]	= (wData & DBIT12)  ? 1 : 0 ;		// S012 (모듈: 0	포트: 12) 
				else if(i == 13) n_out[nIO_Num + 13]	= (wData & DBIT13)  ? 1 : 0 ;		// S013 (모듈: 0	포트: 13) 
				else if(i == 14) n_out[nIO_Num + 14]	= (wData & DBIT14)  ? 1 : 0 ;		// S014 (모듈: 0	포트: 14) 
				else if(i == 15) n_out[nIO_Num + 15]	= (wData & DBIT15)  ? 1 : 0 ;		// S015 (모듈: 0	포트: 15) 
			}
		}
	}	

	if(n_rw_mode == 1 || n_rw_mode == 2) //input read mode
	{
		// ************************************************************************** //
		// 입력(모듈 번호 00번)								                          //
		// ************************************************************************** //
		nRet = HSSI_ReadInputWord(n_bd_num, n_port_num, n_slave_num, &wData); //(iMasterID, iPortNo, iSlaveNo, &wData); 
// jtkim 20130625
//		if(nRet != HSSI_STATUS_SUCCESS){return BD_ERROR;};//error
#ifdef _WIN64
		if(nRet != HSSI_STATUS_SUCCESS){return BD_ERROR;};//error
#else
		if(nRet != STATUS_SUCCESS){return BD_ERROR;};//error
#endif
		n_in[nIO_Num + 0]	= (wData & DBIT0)   ? 1 : 0 ;		// PS000 (모듈: 0	포트: 0) 
		n_in[nIO_Num + 1]	= (wData & DBIT1)   ? 1 : 0 ;		// PS001 (모듈: 0	포트: 1) 
		n_in[nIO_Num + 2]	= (wData & DBIT2)   ? 1 : 0 ;		// PS002 (모듈: 0	포트: 2) 
		n_in[nIO_Num + 3]	= (wData & DBIT3)   ? 1 : 0 ;		// PS003 (모듈: 0	포트: 3) 
		n_in[nIO_Num + 4]	= (wData & DBIT4)   ? 1 : 0 ;		// PS004 (모듈: 0	포트: 4)
		n_in[nIO_Num + 5]	= (wData & DBIT5)   ? 1 : 0 ;		// PS005 (모듈: 0	포트: 5) 
		n_in[nIO_Num + 6]	= (wData & DBIT6)   ? 1 : 0 ;		// PS006 (모듈: 0	포트: 6) 
		n_in[nIO_Num + 7]	= (wData & DBIT7)   ? 1 : 0 ;		// PS007 (모듈: 0	포트: 7) 
		n_in[nIO_Num + 8]	= (wData & DBIT8)   ? 1 : 0 ;		// PS008 (모듈: 0	포트: 8) 
		n_in[nIO_Num + 9]	= (wData & DBIT9)   ? 1 : 0 ;		// PS009 (모듈: 0	포트: 9) 
		n_in[nIO_Num + 10]	= (wData & DBIT10)  ? 1 : 0 ;		// PS010 (모듈: 0	포트: 10) 
		n_in[nIO_Num + 11]	= (wData & DBIT11)  ? 1 : 0 ;		// PS011 (모듈: 0	포트: 11) 
		n_in[nIO_Num + 12]	= (wData & DBIT12)  ? 1 : 0 ;		// PS012 (모듈: 0	포트: 12) 
		n_in[nIO_Num + 13]	= (wData & DBIT13)  ? 1 : 0 ;		// PS013 (모듈: 0	포트: 13) 
		n_in[nIO_Num + 14]	= (wData & DBIT14)  ? 1 : 0 ;		// PS014 (모듈: 0	포트: 14) 
		n_in[nIO_Num + 15]	= (wData & DBIT15)  ? 1 : 0 ;		// PS015 (모듈: 0	포트: 15) 
	}

	if(n_rw_mode == 1 || n_rw_mode == 2) //output mode
	{
		// ************************************************************************** //
		// 입력(모듈 번호 00번)								                          //
		// ************************************************************************** //
		wData = 0;
		for(i=0; i<16; i++)//io port 16개
		{
			if(n_out[nIO_Num + i] == 0 || n_out[nIO_Num + i] == 1) //0:off, 1:on
			{
				if     (i == 0)  wData += n_out[nIO_Num + i]		* DBIT0;  // S000 (모듈: 0	포트: 0)
				else if(i == 1)  wData += n_out[nIO_Num + i]		* DBIT1;  // S001 (모듈: 0	포트: 1)
				else if(i == 2)  wData += n_out[nIO_Num + i]		* DBIT2;  // S002 (모듈: 0	포트: 2)
				else if(i == 3)  wData += n_out[nIO_Num + i]		* DBIT3;  // S003 (모듈: 0	포트: 3)
				else if(i == 4)  wData += n_out[nIO_Num + i]		* DBIT4;  // S004 (모듈: 0	포트: 4)
				else if(i == 5)  wData += n_out[nIO_Num + i]		* DBIT5;  // S005 (모듈: 0	포트: 5)
				else if(i == 6)  wData += n_out[nIO_Num + i]		* DBIT6;  // S006 (모듈: 0	포트: 6)
				else if(i == 7)  wData += n_out[nIO_Num + i]		* DBIT7;  // S007 (모듈: 0	포트: 7)
				else if(i == 8)  wData += n_out[nIO_Num + i]		* DBIT8;  // S008 (모듈: 0	포트: 8)
				else if(i == 9)  wData += n_out[nIO_Num + i]		* DBIT9;  // S009 (모듈: 0	포트: 9)
				else if(i == 10) wData += n_out[nIO_Num + i]		* DBIT10; // S010 (모듈: 0	포트: 10)
				else if(i == 11) wData += n_out[nIO_Num + i]		* DBIT11; // S011 (모듈: 0	포트: 11)
				else if(i == 12) wData += n_out[nIO_Num + i]		* DBIT12; // S012 (모듈: 0	포트: 12)
				else if(i == 13) wData += n_out[nIO_Num + i]		* DBIT13; // S013 (모듈: 0	포트: 13)
				else if(i == 14) wData += n_out[nIO_Num + i]		* DBIT14; // S014 (모듈: 0	포트: 14)
				else if(i == 15) wData += n_out[nIO_Num + i]		* DBIT15; // S015 (모듈: 0	포트: 15)
				

			}
		}

		nRet = HSSI_WriteOutputWord(n_bd_num, n_port_num, n_slave_num, wData);//(iMasterID, iPortNo, iSlaveNo, wData); //TRUE = enable, FALSE = Dasible
// jtkim 20130625
//		if(nRet != HSSI_STATUS_SUCCESS){return BD_ERROR;};//error 
#ifdef _WIN64
		if(nRet != HSSI_STATUS_SUCCESS){return BD_ERROR;};//error
#else
		if(nRet != STATUS_SUCCESS){return BD_ERROR;};//error
#endif
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//2011.1019 james lee WORD 단위로 I/O를 제어한다
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





// ******************************************************************************
// I/O 보드 라이브러리 초기화 함수                                                   
// ******************************************************************************
// ******************************************************************************
// I/O 보드 오픈 함수                                                            
// ******************************************************************************
int CFastechPublic_IO::Set_IO_BoardOpen(INT* pn_MasterNo) 
{
	CString sMsg;
	int     nRet=0;
	INT     nMasterNo=0;

#ifdef _WIN64
	nRet = HSSI_Open(&nMasterNo);  // HSSI 초기화 함수 
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		*pn_MasterNo = nMasterNo;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
#else
	nRet = HSSI_Open(&nMasterNo, START_NOTHING);  // HSSI 초기화 함수 
	if (nRet == STATUS_SUCCESS)
	{
		*pn_MasterNo = nMasterNo;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
#endif

	return nRet;
}
// ******************************************************************************

// ******************************************************************************
// I/O 보드 해제 함수                                                            
// ******************************************************************************
int CFastechPublic_IO::Set_IO_BoardClose(bool b_CloseMethod) 
{
	CString sMsg;
	int     nRet=0;

#ifdef _WIN64
	nRet = HSSI_Close();  // HSSI 리소스 해제 함수 
	if (nRet == HSSI_STATUS_SUCCESS)
	{
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
#else
	nRet = HSSI_Close(TRUE);  // HSSI 리소스 해제 함수 
	if (nRet == STATUS_SUCCESS)
	{
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
#endif
	return nRet;
}
// ******************************************************************************

// ******************************************************************************
// SLAVE I/O 모듈의 입출력 설정 함수 [WORD 단위]                                 
// ******************************************************************************
int CFastechPublic_IO::Set_IO_DefineWord(int n_MasterID, int n_PortNo, int n_SlaveNo, WORD w_Data) 
{
	CString sMsg;
	int     nRet=0;

	nRet = HSSI_WriteDirectionWord(n_MasterID, n_PortNo, n_SlaveNo, w_Data);  // SLAVE I/O 모듈의 입출력 설정 함수 [WORD 단위]
// jtkim 20130625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
#endif
	
	return nRet;
}
// ******************************************************************************

// ******************************************************************************
// SLAVE I/O 모듈의 입출력 설정 함수 [BYTE 단위]                                 
// ******************************************************************************
int CFastechPublic_IO::Set_IO_DefineByte(int n_MasterID, int n_PortNo, int n_SlaveNo, int n_Type, BYTE bt_Data) 
{
	CString sMsg;
	int     nRet=0;

	nRet = HSSI_WriteDirectionByte(n_MasterID, n_PortNo, n_SlaveNo, n_Type, bt_Data);  // SLAVE I/O 모듈의 입출력 설정 함수 [BYTE 단위]
// jtkim 20130625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
#endif
	return nRet;
}
// ******************************************************************************

// ******************************************************************************
// SLAVE I/O 모듈의 입출력 설정 함수 [BIT 단위]                                  
// ******************************************************************************
int CFastechPublic_IO::Set_IO_DefineBit(int n_IONo, BOOL b_Data) 
{
	CString sMsg;
	int     nRet=0;
	INT     nMasterID=0, nPortNo=0, nSlaveNo=0, nBitNo=0;

	nMasterID = Search_IO_BoardInformation(0, n_IONo);  // HSSI 보드 데이터 확인 및 검사 함수
	nPortNo   = Search_IO_BoardInformation(1, n_IONo);
	nSlaveNo  = Search_IO_BoardInformation(2, n_IONo);
	nBitNo    = Search_IO_BoardInformation(3, n_IONo);

	nRet = HSSI_WriteDirectionBit(nMasterID, nPortNo, nSlaveNo, nBitNo, b_Data);  // SLAVE I/O 모듈의 입출력 설정 함수 [BIT 단위]
// jtkim 20130625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
#endif
	return nRet;
}
// ******************************************************************************

// ******************************************************************************
// SLAVE I/O 모듈의 입출력 설정 상태 리턴 [WORD 단위]                            
// ******************************************************************************
int CFastechPublic_IO::Get_IO_DefineWord(int n_MasterID, int n_PortNo, int n_SlaveNo, WORD* pw_Data) 
{
	CString sMsg;
	int     nRet=0;
	WORD    wData=0;

	nRet = HSSI_ReadDirectionWord(n_MasterID, n_PortNo, n_SlaveNo, &wData);  // SLAVE I/O 모듈의 입출력 설정 상태 리턴 [WORD 단위]
// jtkim 20130625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		*pw_Data = wData;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		*pw_Data = wData;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
		*pw_Data = wData;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
#endif
	return nRet;
}
// ******************************************************************************

// ******************************************************************************
// SLAVE I/O 모듈의 입출력 설정 상태 리턴 [BYTE 단위]                            
// ******************************************************************************
int CFastechPublic_IO::Get_IO_DefineByte(int n_MasterID, INT n_PortNo, int n_SlaveNo, int n_Type, BYTE* pb_Data) 
{
	CString sMsg;
	int     nRet=0;
	BYTE    bData=0;

	nRet = HSSI_ReadDirectionByte(n_MasterID, n_PortNo, n_SlaveNo, n_Type, &bData);  // SLAVE I/O 모듈의 입출력 설정 상태 리턴 [BYTE 단위]
// jtkim 20130625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		*pb_Data = bData;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		*pb_Data = bData;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
		*pb_Data = bData;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
#endif
	return nRet;
}
// ******************************************************************************

// ******************************************************************************
// SLAVE I/O 모듈의 입출력 설정 상태 리턴 [BIT 단위]                             
// ******************************************************************************
int CFastechPublic_IO::Get_IO_DefineBit(int n_IONo, BOOL* pb_Data) 
{
	CString sMsg;
	int     nRet=0;
	BOOL    bData;
	INT     nMasterID=0, nPortNo=0, nSlaveNo=0, nBitNo=0;

	nMasterID = Search_IO_BoardInformation(0, n_IONo);  // HSSI 보드 데이터 확인 및 검사 함수
	nPortNo   = Search_IO_BoardInformation(1, n_IONo);
	nSlaveNo  = Search_IO_BoardInformation(2, n_IONo);
	nBitNo    = Search_IO_BoardInformation(3, n_IONo);

	nRet = HSSI_ReadDirectionBit(nMasterID, nPortNo, nSlaveNo, nBitNo, &bData);  // SLAVE I/O 모듈의 입출력 설정 상태 리턴 [BIT 단위]
// jtkim 20130625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		*pb_Data = bData;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		*pb_Data = bData;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
		*pb_Data = bData;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
#endif
	return nRet;
}
// ******************************************************************************

// ******************************************************************************
// PC에 연결된 마스터 보드 갯수 검사 함수                                        
// ******************************************************************************
int CFastechPublic_IO::Search_IO_Master(INT* pn_MasterNo) 
{
	CString sMsg;
	int     nRet=0;
	INT     nMasterNo=0;

	nRet = HSSI_GetAttachedMasterBoard(&nMasterNo);  // 현재 PC에 연결되어 있는 마스터 보드 갯수 리턴 함수
// jtkim 20130625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		*pn_MasterNo = nMasterNo;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		*pn_MasterNo = nMasterNo;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
		*pn_MasterNo = nMasterNo;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
#endif
	return nRet;
}
// ******************************************************************************


// ******************************************************************************
// 마스터 보드에 연결된 PORT 갯수 검사 함수                                      
// ******************************************************************************
int CFastechPublic_IO::Search_IO_Port(int n_MasterID, INT* pn_PortNo) 
{
	CString sMsg;
	int     nRet=0;
	INT     nPortNo=0;

	nRet = HSSI_GetAttachedPort(n_MasterID, &nPortNo);  // 마스터 보드의 정상 동작하는 PORT 갯수 리턴 함수
// jtkim 20130625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		*pn_PortNo = nPortNo;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		*pn_PortNo = nPortNo;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
		*pn_PortNo = nPortNo;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
#endif
	return nRet;
}
// ******************************************************************************

// ******************************************************************************
// PORT에 연결된 SLAVE 검사 함수                                                 
// ******************************************************************************
int CFastechPublic_IO::Search_IO_Slave(int n_MasterID, int n_PortNo, INT* pn_SlaveNo) 
{
	CString sMsg;
	int     nRet=0;	
	INT     nSlaveNo=0;

	nRet = HSSI_GetAttachedSlaveModule(n_MasterID, n_PortNo, &nSlaveNo);  // PORT에 연결된 SLAVE I/O 모듈 검사 함수
// jtkim 20130625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		*pn_SlaveNo = nSlaveNo;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		*pn_SlaveNo = nSlaveNo;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
		*pn_SlaveNo = nSlaveNo;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
#endif
	return nRet;
}
// ******************************************************************************

// ******************************************************************************
// SLAVE I/O MODULE이 ENABLE 상태로 마스터 보드에 연결되어 있는지 검사 함수      
// ******************************************************************************
int CFastechPublic_IO::Check_IO_SlaveStatus(int n_MasterID, int n_PortNo, int n_SlaveNo) 
{
	CString sMsg;
	int     nRet=0;

	nRet = HSSI_IsExistSlaveModule(n_MasterID, n_PortNo, n_SlaveNo);  // SLAVE I/O MODULE이 ENABLE 상태로 마스터 보드에 연결되어 있는지 검사 함수
// jtkim 20130625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
#endif
	return nRet;
}
// ******************************************************************************

// ******************************************************************************
// PORT에 연결된 SLAVE [ENABLE/DISABLE] 상태 검사 함수                           
// ******************************************************************************
BOOL CFastechPublic_IO::Check_IO_Slave(int n_MasterID, int n_PortNo, int n_SlaveID) 
{
	int nSlaveInfo;

	FAS_STATUS  fasResult;

	// **************************************************************************
	// nSlaveInfo : 최하위로부터 상위 비트 방향으로 8개의 BIT까지 각 SLAVE들의   
	//              연결 상태가 저장된 BCD가 들어있다                            
	// -> BIT 1   : SLAVE I/O 모듈이 연결되어 ENABLE된 상태                      
	// -> BIT 0   : SLAVE I/O 모듈이 연결되지 않아 DISABLE된 상태                
	// **************************************************************************
	fasResult = HSSI_GetAttachedSlaveModule(n_MasterID, n_PortNo, &nSlaveInfo);  // PORT에 연결된 SLAVE I/O 모듈 검사 함수
	if (((nSlaveInfo >> n_SlaveID) & 0x1) != 1)
	{
		return BD_ERROR;
	}
	// **************************************************************************

	return BD_GOOD;
}
// ******************************************************************************

// ******************************************************************************
// PORT와 SLAVE I/O 모듈과의 통신 속도 설정 함수                                 
// -> PORT_SPEED_05 : 0.5 Mbps                                                   
// -> PORT_SPEED_10 : 1.0 Mbps                                                   
// -> PORT_SPEED_20 : 2.0 Mbps                                                   
// ******************************************************************************
int CFastechPublic_IO::Set_IO_HSSISpeed(int n_MasterID, int n_PortNo, int n_Type) 
{
	CString sMsg;
	int     nRet=0, nFlag = BD_ERROR;

	nRet = HSSI_SetCommSpeed(n_MasterID, n_PortNo, n_Type);  // 해당 PORT와 I/O 모듈 사이의 통신 속도 설정 함수
// jtkim 20130625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
#endif
	return nRet;
}
// ******************************************************************************

// ******************************************************************************
// PORT와 SLAVE I/O 모듈과의 통신 속도 리턴 함수                                 
// -> PORT_SPEED_05 : 0.5 Mbps                                                   
// -> PORT_SPEED_10 : 1.0 Mbps                                                   
// -> PORT_SPEED_20 : 2.0 Mbps                                                   
// ******************************************************************************
int CFastechPublic_IO::Get_IO_HSSISpeed(int n_MasterID, int n_PortNo, INT* pn_Type) 
{
	CString sMsg;
	int     nRet=0;
	INT     nType;

	nRet = HSSI_GetCommSpeed(n_MasterID, n_PortNo, &nType);  // 해당 PORT와 I/O 모듈 사이의 통신 속도 리턴 함수
// jtkim 20130625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		*pn_Type = nType;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		*pn_Type = nType;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
		*pn_Type = nType;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
#endif
	return nRet;
}
// ******************************************************************************

// ******************************************************************************
// 보드 장착 상태 검사 함수                                                      
// ******************************************************************************
int CFastechPublic_IO::Check_IO_BoardExist(int n_MaxSlot, int n_MaxPort, int n_MaxSlave) 
{
	int nMasterID, nPortNo, nSlaveID;
	int nRet=0;

	while(1)
	{
		for(nMasterID=0; nMasterID<n_MaxSlot; nMasterID++)
		{
			nRet = Check_IO_MasterStatus(nMasterID);  // 마스터 보드 장착 유무 검사 함수
			if (nRet != BD_GOOD)
			{
				continue ;
			}

			for(nPortNo=0; nPortNo<n_MaxPort; nPortNo)
			{
				nRet = Check_IO_PortStatus(nMasterID, nPortNo);  // PORT 존재 유무 및 정상 동작 검사 함수
				if (nRet != BD_GOOD)
				{
					continue ;
				}

				for(nSlaveID=0; nSlaveID<n_MaxSlave; nSlaveID++)
				{
					nRet = Check_IO_SlaveStatus(nMasterID, nPortNo, nSlaveID);  // SLAVE I/O MODULE이 ENABLE 상태로 마스터 보드에 연결되어 있는지 검사 함수
					if (nRet != BD_GOOD)
					{
						continue ;
					}
				}
			}
		}
	}

	return 0;
}
// ******************************************************************************

// ******************************************************************************
// PORT와 연결된 모든 SLAVE I/O 모듈 자동 ENABLE 함수                            
// ******************************************************************************
int CFastechPublic_IO::Set_IO_PortAutoEnable(int n_MasterID, int n_PortNo) 
{
	CString sMsg;
	int     nRet=0;

	nRet = HSSI_PortAutoEnable(n_MasterID, n_PortNo);  // 해당 PORT에 연결된 모든 SLAVE I/O 모듈 자동 ENABLE 함수
// jtkim 20130625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		nRet = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		nRet = BD_ERROR;
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		nRet = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		nRet = BD_ERROR;
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
		nRet = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		nRet = BD_ERROR;
	}
#endif
	return nRet;
}
// ******************************************************************************

// ******************************************************************************
// SLAVE I/O 모듈 동작 ENABLE/DISABLE 설정 함수                                  
// ******************************************************************************
int CFastechPublic_IO::Set_IO_SlaveEnable(int n_MasterID, int n_PortNo, int n_SlaveNo, BOOL b_Enable) 
{
	CString sMsg;
	int     nRet=0;

	nRet = HSSI_EnableSlaveModule(n_MasterID, n_PortNo, n_SlaveNo, b_Enable);  // 해당 SLAVE I/O 모듈 동작 ENABLE/DISABLE 설정 함수
// jtkim 20130625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		nRet = BD_GOOD;
	}
	else
	{
		nRet = BD_ERROR;
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		nRet = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		nRet = BD_ERROR;
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
		nRet = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		nRet = BD_ERROR;
	}
#endif
	return nRet;
}
// ******************************************************************************


// ******************************************************************************

// ******************************************************************************
// 특정 위치 BIT값 검사 함수                                                     
// ******************************************************************************
BOOL CFastechPublic_IO::Check_Bit(long n_Value, int n_BitPos) 
{
	unsigned long nMask = 0x0001;  // 마스크 BIT 설정 
	nMask <<= n_BitPos;  // 변경할 BIT 위치로 이동 [SHIFT] 

	return (n_Value & nMask);  // 해당 BIT 변경 후 리턴 
}
// ******************************************************************************

// ******************************************************************************
// 특정 위치 BIT 변경 함수                                                       
// ******************************************************************************
void CFastechPublic_IO::Set_Bit(long &n_Value, int n_Bit, BOOL b_On) 
{
	unsigned long nMask = 0x0001;  // 마스크 BIT 설정 
	nMask <<= n_Bit;  // 변경할 BIT 위치로 이동 [SHIFT] 

	if ( b_On )
	{
		n_Value |= nMask;
	}
	else
	{
		n_Value &= ~nMask;
	}
}
// ******************************************************************************

// ******************************************************************************
// HSSI 보드 데이터 확인 및 검사 함수                                            
// -> 입력 파라메터는 일덩한 포맷으로 입력되어야 한다                            
//  : I/O MAP 정의 참조한다                                                      
//  : I/O MAP 구성은 4자리로 구성된다                                            
// ******************************************************************************
int CFastechPublic_IO::Search_IO_BoardInformation(int n_Type, int n_Data) 
{
	int nFlag     = BD_ERROR;
	int nBardData = 0;

	// **************************************************************************
	// I/O PORT에 대한 최대 입출력은 7127로 제한한다                             
	// -> 최대 사용 : 마스터 보드 2개로 제한 [I/O PORT는 2048]                   
	// **************************************************************************
	if (n_Data > HSSI_MAX_IO) //HSSI_MAX_IO = 7715
		return BD_ERROR;
	// **************************************************************************

	switch(n_Type)
	{
	case 0 :  // 마스터 보드 
		// **********************************************************************
		// 0[0, 1, 2, 3]    1[4, 5, 6, 7]    2[8, 9, 10, 11]    3[12, 13, 14, 15]
		// -> 마스터 보드 1장에 최대 4 PORT 연결된다                             
		// **********************************************************************
		nBardData = n_Data / 4000;
		// **********************************************************************
		break;

	case 1 :  // PORT 
		// **********************************************************************
		// I/O MAP 번호를 1000으로 나누면 몫이 PORT 번호가 되도록 정의되어 있다  
		// -> PORT 설정 범위는 최대 [0 - 15] 이다                                
		// -> 마스터 보드는 최대 2장으로 제한한다                                
		// **********************************************************************
		nBardData = n_Data / 1000;

		if (nBardData>3)
			nBardData -= 4;
		// **********************************************************************
		break;

	case 2 :  // SLAVE 
		// **********************************************************************
		// 한 PORT에 연결된 SLAVE 번호를 설정한다                                
		// -> PORT 당 최대 [0 - 7] 연결된다                                      
		// **********************************************************************
		nBardData = n_Data     % 1000;
		nBardData = nBardData / 100;
		// **********************************************************************
		break;

	case 3 :  // I/O BIT 
		// **********************************************************************
		// 해당 I/O BIT 위치 설정한다                                            
		// -> I/O BIT는 최대 [0 - 15] 설정할 수 있다                             
		//  : IN/OUT 상태는 임의로 조정 가능한다                                 
		// **********************************************************************
		nBardData = n_Data     % 1000;
		nBardData = nBardData % 100;
		// **********************************************************************
		break;

	default :
		return BD_ERROR;
		break;
	}

	return nBardData;
}
// ******************************************************************************

// ******************************************************************************
// SLAVE I/O 모듈의 입력을 LATCH 모드로 설정 함수 [WORD 단위]                    
// ******************************************************************************
int CFastechPublic_IO::Set_IO_LatchWord(int n_MasterID, int n_PortNo, int n_SlaveNo, WORD w_Data) 
{
	CString sMsg;
	int     nRet=0;

	nRet = HSSI_SetLatchWord(n_MasterID, n_PortNo, n_SlaveNo, w_Data);  // SLAVE I/O 모듈의 입력을 LATCH 모드로 설정 함수 [WORD 단위]
// jtkim 20130625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		nRet = BD_GOOD;
	}
	else
	{
		nRet = BD_ERROR;
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		nRet = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		nRet = BD_ERROR;
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
		nRet = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		nRet = BD_ERROR;
	}
#endif
	return nRet;
}
// ******************************************************************************

// ******************************************************************************
// SLAVE I/O 모듈의 입력을 LATCH 모드로 설정 함수 [BYTE 단위]                    
// ******************************************************************************
int CFastechPublic_IO::Set_IO_LatchByte(int n_MasterID, int n_PortNo, int n_SlaveNo, int n_Type, BYTE bt_Data) 
{
	CString sMsg;
	int     nRet=0;

	nRet = HSSI_SetLatchByte(n_MasterID, n_PortNo, n_SlaveNo, n_Type, bt_Data);  // SLAVE I/O 모듈의 입력을 LATCH 모드로 설정 함수 [BYTE 단위]
// jtkim 20130625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		nRet = BD_GOOD;
	}
	else
	{
		nRet = BD_ERROR;
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		nRet = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		nRet = BD_ERROR;
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
		nRet = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		nRet = BD_ERROR;
	}
#endif
	return nRet;
}
// ******************************************************************************

// ******************************************************************************
// SLAVE I/O 모듈의 입력을 LATCH 모드로 설정 함수 [BIT 단위]                     
// ******************************************************************************
int CFastechPublic_IO::Set_IO_LatchBit(int n_IONo, BOOL b_Data) 
{
	CString sMsg;
	int     nRet=0;	
	INT     nMasterID=0, nPortNo=0, nSlaveNo=0, nBitNo=0;

	nMasterID = Search_IO_BoardInformation(0, n_IONo);  // HSSI 보드 데이터 확인 및 검사 함수
	nPortNo   = Search_IO_BoardInformation(1, n_IONo);
	nSlaveNo  = Search_IO_BoardInformation(2, n_IONo);
	nBitNo    = Search_IO_BoardInformation(3, n_IONo);

	nRet = HSSI_SetLatchBit(nMasterID, nPortNo, nSlaveNo, nBitNo, b_Data);  // SLAVE I/O 모듈의 입력을 LATCH 모드로 설정 함수 [BIT 단위]
// jtkim 20130625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		nRet = BD_GOOD;
	}
	else
	{
		nRet = BD_ERROR;
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		nRet = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		nRet = BD_ERROR;
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
		nRet = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		nRet = BD_ERROR;
	}
#endif
	return nRet;
}
// ******************************************************************************

// ******************************************************************************
// SLAVE I/O 모듈의 LATCH 상태 리턴 함수 [WORD 단위]                             
// ******************************************************************************
WORD CFastechPublic_IO::Get_IO_LatchWord(int n_MasterID, int n_PortNo, int n_SlaveNo) 
{
	CString sMsg;
	int     nRet=0;
	
	WORD    wData=0;

	nRet = HSSI_ReadLatchWord(n_MasterID, n_PortNo, n_SlaveNo, &wData);  // SLAVE I/O 모듈의 LATCH 상태 리턴 함수 [WORD 단위]
// jtkim 20130625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		//wData는 정상값이 들어갔다 //wData = BD_GOOD;
	}
	else
	{
		wData = BD_ERROR;
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		wData = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		wData = BD_ERROR;
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
		wData = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		wData = BD_ERROR;
	}
#endif
	return wData;
}
// ******************************************************************************

// ******************************************************************************
// SLAVE I/O 모듈의 LATCH 상태 리턴 함수 [BYTE 단위]                             
// ******************************************************************************
BYTE CFastechPublic_IO::Get_IO_LatchByte(int n_MasterID, INT n_PortNo, int n_SlaveNo, int n_Type) 
{
	CString sMsg;
	int     nRet=0;	
	BYTE    bData=0;

	nRet = HSSI_ReadLatchByte(n_MasterID, n_PortNo, n_SlaveNo, n_Type, &bData);  // SLAVE I/O 모듈의 LATCH 상태 리턴 함수 [BYTE 단위]
// jtkim 20130625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{	 
	}
	else
	{
		bData = BD_ERROR;
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		bData = BD_ERROR;
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		bData = BD_ERROR;
	}
#endif
	return bData;
}
// ******************************************************************************

// ******************************************************************************
// SLAVE I/O 모듈의 LATCH 상태 리턴 함수 [BIT 단위]                              
// ******************************************************************************
BOOL CFastechPublic_IO::Get_IO_LatchBit(int n_IONo) 
{
	CString sMsg;
	int     nRet=0;
	BOOL    bData=0;
	INT     nMasterID=0, nPortNo=0, nSlaveNo=0, nBitNo=0;

	nMasterID = Search_IO_BoardInformation(0, n_IONo);  // HSSI 보드 데이터 확인 및 검사 함수
	nPortNo   = Search_IO_BoardInformation(1, n_IONo);
	nSlaveNo  = Search_IO_BoardInformation(2, n_IONo);
	nBitNo    = Search_IO_BoardInformation(3, n_IONo);

	nRet = HSSI_ReadLatchBit(nMasterID, nPortNo, nSlaveNo, nBitNo, &bData);  // SLAVE I/O 모듈의 LATCH 상태 리턴 함수 [BIT 단위]
// jtkim 20130625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{
	}
	else
	{
		bData = BD_ERROR;
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		bData = BD_ERROR;
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		bData = BD_ERROR;
	}
#endif
	return bData;
}
// ******************************************************************************

// ******************************************************************************
// SLAVE I/O 모듈의 LATCH 동작 정지 함수 [WORD 단위]                             
// ******************************************************************************
int CFastechPublic_IO::Reset_IO_LatchWord(int n_MasterID, int n_PortNo, int n_SlaveNo, WORD w_Data) 
{
	CString sMsg;
	int     nRet=0;

	nRet = HSSI_ResetLatchWord(n_MasterID, n_PortNo, n_SlaveNo, w_Data);  // SLAVE I/O 모듈의 LATCH 동작 정지 함수 [WORD 단위]
// jtkim 20130625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		nRet = BD_GOOD;
	}
	else
	{
		nRet = BD_ERROR;
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		nRet = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		nRet = BD_ERROR;
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
		nRet = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		nRet = BD_ERROR;
	}
#endif
	return nRet;
}
// ******************************************************************************

// ******************************************************************************
// SLAVE I/O 모듈의 LATCH 동작 정지 함수 [BYTE 단위]                             
// ******************************************************************************
int CFastechPublic_IO::Reset_IO_LatchByte(int n_MasterID, int n_PortNo, int n_SlaveNo, int n_Type, BYTE bt_Data) 
{
	CString sMsg;
	int     nRet=0;

	nRet = HSSI_ResetLatchByte(n_MasterID, n_PortNo, n_SlaveNo, n_Type, bt_Data);  // SLAVE I/O 모듈의 LATCH 동작 정지 함수 [BYTE 단위]
// jtkim 20130625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		nRet = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		nRet = BD_ERROR;
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
		nRet = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		nRet = BD_ERROR;
	}
#endif
	return nRet;
}
// ******************************************************************************

// ******************************************************************************
// SLAVE I/O 모듈의 LATCH 동작 정지 함수 [BIT 단위]                              
// ******************************************************************************
int CFastechPublic_IO::Reset_IO_LatchBit(int n_IONo, BOOL b_Data) 
{
	CString sMsg;
	int     nRet=0;
	INT     nMasterID=0, nPortNo=0, nSlaveNo=0, nBitNo=0;

	nMasterID = Search_IO_BoardInformation(0, n_IONo);  // HSSI 보드 데이터 확인 및 검사 함수
	nPortNo   = Search_IO_BoardInformation(1, n_IONo);
	nSlaveNo  = Search_IO_BoardInformation(2, n_IONo);
	nBitNo    = Search_IO_BoardInformation(3, n_IONo);

	nRet = HSSI_SetLatchBit(nMasterID, nPortNo, nSlaveNo, nBitNo, b_Data);  // SLAVE I/O 모듈의 LATCH 동작 정지 함수 [BIT 단위]
// jtkim 20310625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		nRet = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		nRet = BD_ERROR;
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
		nRet = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		nRet = BD_ERROR;
	}
#endif
	return nRet;
}
// ******************************************************************************

// ******************************************************************************
// 마스터 보드 장착 유무 검사 함수                                               
// ******************************************************************************
int CFastechPublic_IO::Check_IO_MasterStatus(INT n_MasterID) 
{
	CString sMsg;
	int     nRet=0;

	nRet = HSSI_IsExistMasterBoard(n_MasterID);  // 마스터 보드 장착 유무 검사 함수
// jtkim 20130625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		nRet = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		nRet = BD_ERROR;
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
		nRet = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		nRet = BD_ERROR;
	}
#endif
	return nRet;
}
// ******************************************************************************

// ******************************************************************************
// PORT 존재 유무 및 정상 동작 검사 함수                                         
// ******************************************************************************
int CFastechPublic_IO::Check_IO_PortStatus(int n_MasterID, int n_PortNo) 
{
	CString sMsg;
	int     nRet=0;

	nRet = HSSI_IsExistPort(n_MasterID, n_PortNo);  // PORT 존재 유무 및 정상 동작 검사 함수
// jtkim 20130625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		nRet = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		nRet = BD_ERROR;
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
		nRet = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		nRet = BD_ERROR;
	}
#endif
	return nRet;
}
// ******************************************************************************


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//I/O를 직접 제어한다  james Lee 2008.9.5
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// ******************************************************************************
// SLAVE I/O 모듈 출력 신호 발생 함수 [WORD 단위]                                
// ******************************************************************************
int CFastechPublic_IO::Set_Out_Word(int n_IONo, WORD w_Data) 
{
	CString sMsg;
	int     nRet=0;	
	INT     nMasterID=0, nPortNo=0, nSlaveNo=0;

	nMasterID = Search_IO_BoardInformation(0, n_IONo * 100);  // HSSI 보드 데이터 확인 및 검사 함수
	nPortNo   = Search_IO_BoardInformation(1, n_IONo * 100);
	nSlaveNo  = Search_IO_BoardInformation(2, n_IONo * 100);

	nRet = HSSI_WriteOutputWord(nMasterID, nPortNo, nSlaveNo, w_Data);  // SLAVE I/O 모듈 출력 신호 발생 함수 [WORD 단위]
// jtkim 20130625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		nRet = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		nRet = BD_ERROR;
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
		nRet = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		nRet = BD_ERROR;
	}
#endif
	return nRet;
}
// ******************************************************************************

// ******************************************************************************
// SLAVE I/O 모듈 출력 신호 발생 함수 [BYTE 단위]                                
// ******************************************************************************
int CFastechPublic_IO::Set_Out_Byte(int n_IONo, int n_Type, BYTE bt_Data) 
{
	CString sMsg;
	int     nRet=0;	
	INT     nMasterID=0, nPortNo=0, nSlaveNo=0;

	nMasterID = Search_IO_BoardInformation(0, n_IONo * 100);  // HSSI 보드 데이터 확인 및 검사 함수
	nPortNo   = Search_IO_BoardInformation(1, n_IONo * 100);
	nSlaveNo  = Search_IO_BoardInformation(2, n_IONo * 100);

	nRet = HSSI_WriteOutputByte(nMasterID, nPortNo, nSlaveNo, n_Type, bt_Data);  // SLAVE I/O 모듈 출력 신호 발생 함수 [BYTE 단위]
// jtkim 20130625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		nRet = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		nRet = BD_ERROR;
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
		nRet = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		nRet = BD_ERROR;
	}
#endif
	return nRet;
}
// ******************************************************************************

// ******************************************************************************
// SLAVE I/O 모듈 출력 신호 발생 함수 [BIT 단위]                                 
// ******************************************************************************
int CFastechPublic_IO::Set_Out_Bit(int n_IONo, BOOL b_OnOff) 
{
	
	CString sMsg;
	int     nRet=0;
	INT     nMasterID=0, nPortNo=0, nSlaveNo=0, nBitNo=0;

	nMasterID = Search_IO_BoardInformation(0, n_IONo);  // HSSI 보드 데이터 확인 및 검사 함수
	nPortNo   = Search_IO_BoardInformation(1, n_IONo);
	nSlaveNo  = Search_IO_BoardInformation(2, n_IONo);
	nBitNo    = Search_IO_BoardInformation(3, n_IONo);

	nRet = HSSI_WriteOutputBit(nMasterID, nPortNo, nSlaveNo, nBitNo, b_OnOff);  // SLAVE I/O 모듈 출력 신호 발생 함수 [BIT 단위]
// jtkim 20130625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		nRet = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		nRet = BD_ERROR;
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
		nRet = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		nRet = BD_ERROR;
	}
#endif
	return nRet;
}
// ******************************************************************************

// ******************************************************************************
// SLAVE I/O 모듈 입력 신호 READ 함수 [WORD 단위]                                
// ******************************************************************************
WORD CFastechPublic_IO::Get_In_Word(int n_IONo) 
{
	CString sMsg;
	int     nRet=0;
	INT     nMasterID=0, nPortNo=0, nSlaveNo=0;
	WORD    wData=0;

	nMasterID = Search_IO_BoardInformation(0, n_IONo * 100);  // HSSI 보드 데이터 확인 및 검사 함수
	nPortNo   = Search_IO_BoardInformation(1, n_IONo * 100);
	nSlaveNo  = Search_IO_BoardInformation(2, n_IONo * 100);

	nRet = HSSI_ReadInputWord(nMasterID, nPortNo, nSlaveNo, &wData);  // SLAVE I/O 모듈 입력 신호 READ 함수 [WORD 단위]
// jtkim 20130625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{
	}
	else
	{
		wData = BD_ERROR;
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		wData = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		wData = BD_ERROR;
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
		wData = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		wData = BD_ERROR;
	}
#endif
	return wData;
}
// ******************************************************************************

// ******************************************************************************
// SLAVE I/O 모듈 출력 신호 READ 함수 [WORD 단위]                                
// ******************************************************************************
WORD CFastechPublic_IO::Get_Out_Word(int n_IONo)
{
	CString sMsg;
	int     nRet=0;
	INT     nMasterID=0, nPortNo=0, nSlaveNo=0;
	WORD    wData=0;
	
	nMasterID = Search_IO_BoardInformation(0, n_IONo * 100);  // HSSI 보드 데이터 확인 및 검사 함수
	nPortNo   = Search_IO_BoardInformation(1, n_IONo * 100);
	nSlaveNo  = Search_IO_BoardInformation(2, n_IONo * 100);
	
	nRet = HSSI_ReadOutputWord(nMasterID, nPortNo, nSlaveNo, &wData);  // SLAVE I/O 모듈 입력 신호 READ 함수 [WORD 단위]
// jtkim 20130625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{
	}
	else
	{
		wData = BD_ERROR;
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		wData = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		wData = BD_ERROR;
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
		wData = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		wData = BD_ERROR;
	}
#endif
	return wData;	
}

// ******************************************************************************
// SLAVE I/O 모듈 입력 신호 READ 함수 [BYTE 단위]                                
// ******************************************************************************
BYTE CFastechPublic_IO::Get_In_Byte(int n_IONo, int n_Type) 
{
	CString sMsg;
	int     nRet=0;
	INT     nMasterID=0, nPortNo=0, nSlaveNo=0;
	BYTE    btData=0;

	nMasterID = Search_IO_BoardInformation(0, n_IONo * 100);  // HSSI 보드 데이터 확인 및 검사 함수
	nPortNo   = Search_IO_BoardInformation(1, n_IONo * 100);
	nSlaveNo  = Search_IO_BoardInformation(2, n_IONo * 100);

	nRet = HSSI_ReadInputByte(nMasterID, nPortNo, nSlaveNo, n_Type, &btData);  // SLAVE I/O 모듈 입력 신호 READ 함수 [BYTE 단위]
// jtkim 20130625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{
	}
	else
	{
		btData = BD_ERROR;
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		btData = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		btData = BD_ERROR;
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
		btData = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		btData = BD_ERROR;
	}
#endif
	return btData;
}
// ******************************************************************************

// ******************************************************************************
// SLAVE I/O 모듈 출력 신호 READ 함수 [BYTE 단위]                                
// ******************************************************************************
BYTE CFastechPublic_IO::Get_Out_Byte(int n_IONo, int n_Type)
{
	CString sMsg;
	int     nRet=0;
	INT     nMasterID=0, nPortNo=0, nSlaveNo=0;
	BYTE    btData=0;
	
	nMasterID = Search_IO_BoardInformation(0, n_IONo * 100);  // HSSI 보드 데이터 확인 및 검사 함수
	nPortNo   = Search_IO_BoardInformation(1, n_IONo * 100);
	nSlaveNo  = Search_IO_BoardInformation(2, n_IONo * 100);
	
	nRet = HSSI_ReadOutputByte(nMasterID, nPortNo, nSlaveNo, n_Type, &btData);  // SLAVE I/O 모듈 입력 신호 READ 함수 [BYTE 단위]
// jtkim 20130625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{
	}
	else
	{
		btData = BD_ERROR;
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		btData = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		btData = BD_ERROR;
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
		btData = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		btData = BD_ERROR;
	}
#endif
	return btData;
}

// ******************************************************************************
// SLAVE I/O 모듈 입력 신호 READ 함수 [BIT 단위]                                 
// ******************************************************************************
BOOL CFastechPublic_IO::Get_In_Bit(int n_IONo) 
{
	CString sMsg;
	int     nRet=0;
	INT     nMasterID=0, nPortNo=0, nSlaveNo=0, nBitNo = 0;
	BOOL    bData;

	nMasterID = Search_IO_BoardInformation(0, n_IONo);  // HSSI 보드 데이터 확인 및 검사 함수
	nPortNo   = Search_IO_BoardInformation(1, n_IONo);
	nSlaveNo  = Search_IO_BoardInformation(2, n_IONo);
	nBitNo    = Search_IO_BoardInformation(3, n_IONo);

	nRet = HSSI_ReadInputBit(nMasterID, nPortNo, nSlaveNo, nBitNo, &bData);  // SLAVE I/O 모듈 입력 신호 READ 함수 [BIT 단위]
// jtkim 20130625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{
	}
	else
	{
		bData = BD_ERROR;
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		bData = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		bData = BD_ERROR;
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
		bData = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		bData = BD_ERROR;
	}
#endif
	return bData;
}




// ******************************************************************************
// SLAVE I/O 모듈 출력 신호 READ 함수 [BIT 단위]                                 
// ******************************************************************************
BOOL CFastechPublic_IO::Get_Out_Bit(int n_IONo)
{
	CString sMsg;
	int     nRet=0;	
	INT     nMasterID=0, nPortNo=0, nSlaveNo=0,nBitNo=0;
	BOOL    bData;
	
	nMasterID = Search_IO_BoardInformation(0, n_IONo);  // HSSI 보드 데이터 확인 및 검사 함수
	nPortNo   = Search_IO_BoardInformation(1, n_IONo);
	nSlaveNo  = Search_IO_BoardInformation(2, n_IONo);
	nBitNo    = Search_IO_BoardInformation(3, n_IONo);
	
	nRet = HSSI_ReadOutputBit(nMasterID, nPortNo, nSlaveNo, nBitNo, &bData);  // SLAVE I/O 모듈 입력 신호 READ 함수 [BIT 단위]
// jtkim 20130625
/*
	if (nRet == HSSI_STATUS_SUCCESS)
	{
	}
	else
	{
		bData = BD_ERROR;
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
	}
*/
#ifdef _WIN64
	if (nRet == HSSI_STATUS_SUCCESS)
	{
		bData = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		bData = BD_ERROR;
	}
#else
	if (nRet == STATUS_SUCCESS)
	{
		bData = BD_GOOD;
	}
	else
	{
		sMsg = ErrorOccurInfo(nRet);  // HSSI DLL 함수 호출 관련 에러 파싱 함수
		if (mn_errormsg_debug_mode)	AfxMessageBox(sMsg);
		bData = BD_ERROR;
	}
#endif
	return bData;
}
// ******************************************************************************




// ******************************************************************************
// HSSI DLL 함수 호출 관련 에러 파싱 함수                                        
// -> nErrorNum                : 발생한 에러 번호                                
// -> STATUS_SUCCESS = 0x0     : 보드 존재                                       
// -> STATUS_ERROR_NO_DEVICE   : 동작 중인 보드 미존재                           
// -> STATUS_ERROR_READ        : READ 동작 실패                                  
// -> STATUS_ERROR_WRITE       : WRITE 동작 실패                                 
// -> STATUS_ERROR_INITIALIZE  : 초기화 실패 [RESET 포함]                        
// -> STATUS_ERROR_COMMAND     : CMD 사용에 문제 발생                            
// -> STATUS_ERROR_FILE        : FILE에 문제 발생                                
// -> STATUS_ERROR_PORT        : PORT 미존재                                     
// -> STATUS_ERROR_SLAVE       : SLAVE가 ON 아님                                 
// -> STATUS_NOTUSE_COMMAND    : 지원하지 않는 명령어                            
// -> STATUS_INVALID_BOARDID   : 해당 보드 미존재                                
// -> STATUS_INVALID_PORTNO    : 해당 PORT 미존재                                
// -> STATUS_INVALID_SLAVEID   : 해당 SLAVE 미존재                               
// -> STATUS_INVALID_PARAMETER : 파라메터 입력 실패                              
// -> STATUS_INVALID_DLL       : 잘못된 DLL 사용                                 
// ******************************************************************************
char* CFastechPublic_IO::ErrorOccurInfo(int nErrorNum) 
{
	CString sErrorMsg;  // 에러 메시지 임시 저장 변수 

	static char cMsg[60]={0,};
// jtkim 20130625
/*
	if (nErrorNum == HSSI_STATUS_ERROR_NO_DEVICE)
	{
		sErrorMsg = _T("Board not EXIST");
	}
	else if (nErrorNum == HSSI_STATUS_ERROR_READ)
	{
		sErrorMsg = _T("READ failure");
	}
	else if (nErrorNum == HSSI_STATUS_ERROR_WRITE)
	{
		sErrorMsg = _T("WRITE failure");
	}
	else if (nErrorNum == HSSI_STATUS_ERROR_INITIALIZE)
	{
		sErrorMsg = _T("INIT Failure [RESET Inclusion]");
	}
	else if (nErrorNum == HSSI_STATUS_ERROR_COMMAND)
	{
		sErrorMsg = _T("Err Occured [CMD]");
	}
	else if (nErrorNum == HSSI_STATUS_ERROR_FILE)
	{
		sErrorMsg = _T("Err Occured [FILE]");
	}
	else if (nErrorNum == HSSI_STATUS_ERROR_PORT)
	{
		sErrorMsg = _T("PORT not EXIST");
	}
	else if (nErrorNum == HSSI_STATUS_ERROR_SLAVE)
	{
		sErrorMsg = _T("SLAVE not [ON]");
	}
	else if (nErrorNum == HSSI_STATUS_NOTUSE_COMMAND)
	{
		sErrorMsg = _T("Unknown Command");
	}
	else if (nErrorNum == HSSI_STATUS_INVALID_BOARDID)
	{
		sErrorMsg = _T("Board not EXIST");
	}
	else if (nErrorNum == HSSI_STATUS_INVALID_PORTNO)
	{
		sErrorMsg = _T("PORT not EXIST");
	}
	else if (nErrorNum == HSSI_STATUS_INVALID_SLAVEID)
	{
		sErrorMsg = _T("SLAVE not EXIST");
	}
	else if (nErrorNum == HSSI_STATUS_INVALID_PARAMETER)
	{
		sErrorMsg = _T("Input ERR [Parameter]");
	}
	else if (nErrorNum == HSSI_STATUS_INVALID_DLL)
	{
		sErrorMsg = _T("Wrong DLL Used");
	}
	else
	{
		sErrorMsg.Format(_T("Unknown HSSI Err Code = %d"), nErrorNum);
	}
*/
#ifdef _WIN64
	if (nErrorNum == HSSI_STATUS_ERROR_NO_DEVICE)
	{
		sErrorMsg = _T("Board not EXIST");
	}
	else if (nErrorNum == HSSI_STATUS_ERROR_READ)
	{
		sErrorMsg = _T("READ failure");
	}
	else if (nErrorNum == HSSI_STATUS_ERROR_WRITE)
	{
		sErrorMsg = _T("WRITE failure");
	}
	else if (nErrorNum == HSSI_STATUS_ERROR_INITIALIZE)
	{
		sErrorMsg = _T("INIT Failure [RESET Inclusion]");
	}
	else if (nErrorNum == HSSI_STATUS_ERROR_COMMAND)
	{
		sErrorMsg = _T("Err Occured [CMD]");
	}
	else if (nErrorNum == HSSI_STATUS_ERROR_FILE)
	{
		sErrorMsg = _T("Err Occured [FILE]");
	}
	else if (nErrorNum == HSSI_STATUS_ERROR_PORT)
	{
		sErrorMsg = _T("PORT not EXIST");
	}
	else if (nErrorNum == HSSI_STATUS_ERROR_SLAVE)
	{
		sErrorMsg = _T("SLAVE not [ON]");
	}
	else if (nErrorNum == HSSI_STATUS_NOTUSE_COMMAND)
	{
		sErrorMsg = _T("Unknown Command");
	}
	else if (nErrorNum == HSSI_STATUS_INVALID_BOARDID)
	{
		sErrorMsg = _T("Board not EXIST");
	}
	else if (nErrorNum == HSSI_STATUS_INVALID_PORTNO)
	{
		sErrorMsg = _T("PORT not EXIST");
	}
	else if (nErrorNum == HSSI_STATUS_INVALID_SLAVEID)
	{
		sErrorMsg = _T("SLAVE not EXIST");
	}
	else if (nErrorNum == HSSI_STATUS_INVALID_PARAMETER)
	{
		sErrorMsg = _T("Input ERR [Parameter]");
	}
	else if (nErrorNum == HSSI_STATUS_INVALID_DLL)
	{
		sErrorMsg = _T("Wrong DLL Used");
	}
	else
	{
		sErrorMsg.Format(_T("Unknown HSSI Err Code = %d"), nErrorNum);
	}
#else
	if (nErrorNum == STATUS_ERROR_NO_DEVICE)
	{
		sErrorMsg = _T("Board not EXIST");
	}
	else if (nErrorNum == STATUS_ERROR_READ)
	{
		sErrorMsg = _T("READ failure");
	}
	else if (nErrorNum == STATUS_ERROR_WRITE)
	{
		sErrorMsg = _T("WRITE failure");
	}
	else if (nErrorNum == STATUS_ERROR_INITIALIZE)
	{
		sErrorMsg = _T("INIT Failure [RESET Inclusion]");
	}
	else if (nErrorNum == STATUS_ERROR_COMMAND)
	{
		sErrorMsg = _T("Err Occured [CMD]");
	}
	else if (nErrorNum == STATUS_ERROR_FILE)
	{
		sErrorMsg = _T("Err Occured [FILE]");
	}
	else if (nErrorNum == STATUS_ERROR_PORT)
	{
		sErrorMsg = _T("PORT not EXIST");
	}
	else if (nErrorNum == STATUS_ERROR_SLAVE)
	{
		sErrorMsg = _T("SLAVE not [ON]");
	}
	else if (nErrorNum == STATUS_NOTUSE_COMMAND)
	{
		sErrorMsg = _T("Unknown Command");
	}
	else if (nErrorNum == STATUS_INVALID_BOARDID)
	{
		sErrorMsg = _T("Board not EXIST");
	}
	else if (nErrorNum == STATUS_INVALID_PORTNO)
	{
		sErrorMsg = _T("PORT not EXIST");
	}
	else if (nErrorNum == STATUS_INVALID_SLAVEID)
	{
		sErrorMsg = _T("SLAVE not EXIST");
	}
	else if (nErrorNum == STATUS_INVALID_PARAMETER)
	{
		sErrorMsg = _T("Input ERR [Parameter]");
	}
	else if (nErrorNum == STATUS_INVALID_DLL)
	{
		sErrorMsg = _T("Wrong DLL Used");
	}
	else
	{
		sErrorMsg.Format(_T("Unknown HSSI Err Code = %d"), nErrorNum);
	}
#endif
//	strncpy(cMsg, sErrorMsg, sizeof(cMsg)-1);
//	_snprintf(cMsg, sizeof(cMsg), "%S", sErrorMsg);
	//wsprintfA(cMsg, "%S", sErrorMsg);
	clsFunc.OnStringToChar(sErrorMsg, cMsg);
	cMsg[sizeof(cMsg)-1] = '\0'; //2011.0518

	return cMsg;
}
// ******************************************************************************

/*
int CFastechPublic_IO::Check_IO_Port(int n_io, int n_time, int n_flag)
{
	int Ret = CTLBD_READY;	// 함수 리턴 플래그 설정 
	
	if (n_time == 0)	// 대기 시간없이 바로 리턴값을 받아야 한다.
	{
		if (Get_IO_InportBit(n_io) == TRUE)
		{
			Ret = CTLBD_YES;
		}
		else	Ret = CTLBD_NO;
	}
	else				// 설정 시간동안 확인하고, 리턴값을 받는다.
	{
		if (IOFlag[n_io] != true)
		{
			IOTime[n_io][0]	= GetCurrentTime();
			IOFlag[n_io] = true;
		}
		else if (Get_IO_InportBit(n_io) == TRUE && IOFlag[n_io] == true)
		{
			IOTime[n_io][1] = GetCurrentTime();
			IOTime[n_io][2] = IOTime[n_io][1] - IOTime[n_io][0];
			
			if (IOTime[n_io][2] >= n_time)
			{
				IOFlag[n_io] = false;
				Ret = CTLBD_YES;
			}
		}
		else if (Get_IO_InportBit(n_io) == FALSE && IOFlag[n_io] == true)
		{
			IOTime[n_io][1] = GetCurrentTime();
			IOTime[n_io][2] = IOTime[n_io][1] - IOTime[n_io][0];
			
			if (IOTime[n_io][2] >= n_time)
			{
				IOFlag[n_io] = false;
				Ret = CTLBD_NO;
			}
		}
	}
	
	return Ret;
}

int CFastechPublic_IO::Check_IO_Port_Yield(int n_io, int n_time, int n_yield)
{
	int Ret = CTLBD_READY;	// 함수 리턴 플래그 설정 
	
	if (IOYFlag[n_io] != true)
	{
		IOYTime[n_io][0] = GetCurrentTime();
		IOYFlag[n_io] = true;
		// Yield Check 초기화. 2K6/05/18/ViboX
		IOYCount[n_io][0] = 0;					// false
		IOYCount[n_io][1] = 0;					// true
		IOYCount[n_io][2] = 0;					// total
	}
	else if (Get_IO_InportBit(n_io) == TRUE && IOYFlag[n_io] == true)
	{
		IOYCount[n_io][1]++;
	}
	else if (Get_IO_InportBit(n_io) == FALSE && IOYFlag[n_io] == true)
	{
		IOYCount[n_io][0]++;
	}
	
	if (IOYFlag[n_io] == true)
	{
		IOYTime[n_io][1] = GetCurrentTime();
		IOYTime[n_io][2] = IOYTime[n_io][1] - IOYTime[n_io][0];
		
		if (IOYTime[n_io][2] >= n_time)
		{
			
			IOYCount[n_io][2] = IOYCount[n_io][1] + IOYCount[n_io][0];
			
			if (IOYCount[n_io][2] <= 0)
			{
				return CTLBD_NO;
			}
			IOYYield[n_io] = (float)IOYCount[n_io][1] / IOYCount[n_io][2];
			
			if (IOYYield[n_io] >= n_yield)
			{
				Ret = CTLBD_YES;
			}
			else
			{
				Ret = CTLBD_NO;
			}
			IOYFlag[n_io] = false;
		}
	}
	return Ret;
}

*/

int CFastechPublic_IO::Initialize_Board()
{
	int i = 0,j = 0, k = 0;
	INT HSSI_speed = 0;
	int Ret = 0;
	INT pMasterNo=0;

	int nBd = 2;
	int nPort[5] = {3};
	int nSlave[1][4];
	
	nSlave[0][0] = 8;
	nSlave[0][1] = 8;
	nSlave[0][2] = 4;
//	nSlave[0][3] = 8;

// 	nSlave[1][0] = 4;
// 	nSlave[1][1] = 0;
// 
// 	nSlave[2][0] = 8;
// 	nSlave[2][1] = 0;

	CString strMsg;
	/////////////////초기에 초기화시 //////////////////////////////////////////////////////////////////////
	Ret = FAS_IO.Set_IO_BoardOpen(&pMasterNo); //START_LOADDATA); //START_NOTHING); //START_CLEAR);
	
#ifdef _WIN64
	if (Ret != HSSI_STATUS_SUCCESS)
	{
		m_nIoBoardInitialFlag	= BD_NO;
		
		return RET_ERROR;
	}
#else 
	if (st_work_info.nSimulationMode == 1) //Simulation 모드이면 프로그램이 동작해야 하니 I/O 상태를 원하는 것으로 셋팅해 준다 
	{
		m_nIoBoardInitialFlag	= BD_YES;
		
		return RET_GOOD;
	}
	else
	{
		if (Ret != STATUS_SUCCESS)
		{
			m_nIoBoardInitialFlag	= BD_NO;
			
			return RET_ERROR;
		}
	}
#endif
/*	for(i=0; i<2; i++)
	{
		Ret = FAS_IO.Set_IO_PortAutoEnable(0, i);
		
		if (Ret != BD_GOOD)
		{
			if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
			{
//				st_other_info.strAbnormalMsg.Format("#0 Card, %d Port, not enable", i);
				strMsg.Format(_T("#0 Card, %d Port, not enable"), i);

				//clsFunc.OnStringToChar(strMsg, st_other_info.cAbnormalMsg);
				clsMem.OnAbNormalMessagWrite(strMsg);
				st_handler_info.cWndList->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
			}
			
			m_nIoBoardInitialFlag	= BD_NO;
			return RET_ERROR;
		}
	}*/

	for(i=0; i<nBd; i++)
	{
		for(j=0; j<nPort[i]; j++)
		{
			Ret = FAS_IO.Set_IO_PortAutoEnable(i, j);
		
			if (Ret != BD_GOOD)
			{
				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					strMsg.Format(_T("#%d Card, %d Port, not enable"), i, j);

					clsMem.OnAbNormalMessagWrite(strMsg);
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
				}
			
				m_nIoBoardInitialFlag	= BD_NO;
				return RET_ERROR;
			}
		}
	}
	
	
	for(i = 0; i < nBd; i++)
	{
		for(j = 0; j < nPort[i]; j++)
		{
			Ret = FAS_IO.Set_IO_HSSISpeed(i, j, PORT_SPEED_05);
			Ret = FAS_IO.Get_IO_HSSISpeed(i, j, &HSSI_speed);

//			for(k = 0; k < 8; k++)
			for(k = 0; k < nSlave[i][j]; k++)
			{
				Ret = FAS_IO.Set_IO_SlaveEnable( i, j, k, TRUE);
			
				if(Ret != BD_GOOD)
				{
					if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
					{
	//					st_other_info.strAbnormalMsg.Format("#0 Card, Port%d, Slave #%d, not enable", i, j);
						strMsg.Format(_T("#%d Card, Port%d, Slave #%d, not enable"), i, j, k);

						//clsFunc.OnStringToChar(strMsg, st_other_info.cAbnormalMsg);
						clsMem.OnAbNormalMessagWrite(strMsg);
						//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
					}
					i = -1;
				
					if (j < 5)
					{
						j++;
					}
					else
					{
	//					st_handler.n_io_board_initial = FALSE;
						m_nIoBoardInitialFlag	= BD_NO;
						return RET_ERROR;
					}
					continue;
				}
			
				if (st_handler_info.cWndList != NULL)  // 리스트 바 화면 존재
				{
					strMsg.Format(_T("#%d Card, Port %d, Slave #%d, enable success"), i, j, k);

	//				clsFunc.OnStringToChar(strMsg, st_other_info.cAbnormalMsg);
					clsMem.OnNormalMessageWrite(strMsg);
					//st_handler_info.cWndList->SendMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 성공 출력 요청
				}
				Sleep(100);
			}
		}
	}
	
/*	
	for(i = 0; i < 4; i++)
	{
		// KILEE [2012.07.02]
		//Ret = FAS_IO.Set_IO_HSSISpeed(0, i, PORT_SPEED_10);
		Ret = FAS_IO.Set_IO_HSSISpeed(0, i, PORT_SPEED_05);
		Ret = FAS_IO.Get_IO_HSSISpeed(0, i, &HSSI_speed);
	}
*/	
	// port 0
	FAS_IO.Set_IO_DefineWord(0, 0, 0, 0x00ff); //0번 Master, 0번 포트, 0번 슬레이브 , 16bit 셋팅			-- Part I/O 검증 완료
	FAS_IO.Set_IO_DefineWord(0, 0, 1, 0x00ff); //0번 Master, 0번 포트, 1번 슬레이브 , 16bit 셋팅			-- Part I/O 검증 완료
	FAS_IO.Set_IO_DefineWord(0, 0, 2, 0x001f); //0번 Master, 0번 포트, 2번 슬레이브 , 16bit 셋팅			-- Part I/O 검증 완료
	FAS_IO.Set_IO_DefineWord(0, 0, 3, 0x00ff); //0번 Master, 0번 포트, 3번 슬레이브 , 16bit 셋팅			-- Part I/O 검증 완료
	FAS_IO.Set_IO_DefineWord(0, 0, 4, 0x00ff); //0번 Master, 0번 포트, 4번 슬레이브 , 16bit 셋팅			-- Part I/O 검증 완료
	FAS_IO.Set_IO_DefineWord(0, 0, 5, 0x003f); //0번 Master, 0번 포트, 5번 슬레이브 , 16bit 셋팅			-- Part I/O 검증 완료
	FAS_IO.Set_IO_DefineWord(0, 0, 6, 0x003f); //0번 Master, 0번 포트, 6번 슬레이브 , 16bit 셋팅			-- Part I/O 검증 완료
	FAS_IO.Set_IO_DefineWord(0, 0, 7, 0x00ff); //0번 Master, 0번 포트, 7번 슬레이브 , 16bit 셋팅			-- Part I/O 검증 완료	
	
	// port 1
	FAS_IO.Set_IO_DefineWord(0, 1, 0, 0x00ff); //0번 Master, 1번 포트, 0번 슬레이브 , 16bit 셋팅			
	FAS_IO.Set_IO_DefineWord(0, 1, 1, 0x0000); //0번 Master, 1번 포트, 1번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 1, 2, 0x00ff); //0번 Master, 1번 포트, 2번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 1, 3, 0x000f); //0번 Master, 1번 포트, 3번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 1, 4, 0x00ff); //0번 Master, 1번 포트, 4번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 1, 5, 0x0000); //0번 Master, 1번 포트, 5번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 1, 6, 0x0000); //0번 Master, 1번 포트, 6번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 1, 7, 0x0000); //0번 Master, 1번 포트, 7번 슬레이브 , 16bit 셋팅

	// port 2
	FAS_IO.Set_IO_DefineWord(0, 2, 0, 0xffff); //0번 Master, 2번 포트, 0번 슬레이브 , 16bit 셋팅			
	FAS_IO.Set_IO_DefineWord(0, 2, 1, 0x00ff); //0번 Master, 2번 포트, 1번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 2, 2, 0x0000); //0번 Master, 2번 포트, 2번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 2, 3, 0x00ff); //0번 Master, 2번 포트, 3번 슬레이브 , 16bit 셋팅
 	FAS_IO.Set_IO_DefineWord(0, 2, 4, 0x0000); //0번 Master, 2번 포트, 4번 슬레이브 , 16bit 셋팅
 	FAS_IO.Set_IO_DefineWord(0, 2, 5, 0x00ff); //0번 Master, 2번 포트, 5번 슬레이브 , 16bit 셋팅
 	FAS_IO.Set_IO_DefineWord(0, 2, 6, 0x00ff); //0번 Master, 2번 포트, 6번 슬레이브 , 16bit 셋팅
 	FAS_IO.Set_IO_DefineWord(0, 2, 7, 0x000f); //0번 Master, 2번 포트, 7번 슬레이브 , 16bit 셋팅

// 	//port 3
// 		FAS_IO.Set_IO_DefineWord(0, 3, 0, 0x07ff); //0번 Master, 3번 포트, 0번 슬레이브 , 16bit 셋팅			
// 		FAS_IO.Set_IO_DefineWord(0, 3, 1, 0x0000); //0번 Master, 3번 포트, 1번 슬레이브 , 16bit 셋팅
// 		FAS_IO.Set_IO_DefineWord(0, 3, 2, 0x0000); //0번 Master, 3번 포트, 2번 슬레이브 , 16bit 셋팅
// 		FAS_IO.Set_IO_DefineWord(0, 3, 3, 0x00ff); //0번 Master, 3번 포트, 3번 슬레이브 , 16bit 셋팅
// 		FAS_IO.Set_IO_DefineWord(0, 3, 4, 0x07ff); //0번 Master, 3번 포트, 4번 슬레이브 , 16bit 셋팅
// 		FAS_IO.Set_IO_DefineWord(0, 3, 5, 0x0000); //0번 Master, 3번 포트, 5번 슬레이브 , 16bit 셋팅
// 		FAS_IO.Set_IO_DefineWord(0, 3, 6, 0x0000); //0번 Master, 3번 포트, 6번 슬레이브 , 16bit 셋팅
// 		FAS_IO.Set_IO_DefineWord(0, 3, 7, 0x00ff); //0번 Master, 3번 포트, 7번 슬레이브 , 16bit 셋팅


	m_nIoBoardInitialFlag	= BD_YES;

	return RET_GOOD;
}


void CFastechPublic_IO::Initialize_Map()
{
	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 00
	///////////////////////////////////////////////////////////////////////////////////////////////
	st_io_info.o_ReadyLd1CvyStkStopUpDnCyl	=	0;		// S0000
	st_io_info.o_ReadyLd2CvyStkStopUpDnCyl	=	1;		// S0001
	st_io_info.o_ReadyLd1CvyAcMotOnOff		=	2;		// S0002
	st_io_info.o_ReadyLd2CvyAcMotOnOff		=	3;		// S0003
	// S0004
	// S0005
	// S0006
	st_io_info.i_ReadyLd1CvyInTrayChk		=	7;		//PS0001
	st_io_info.i_ReadyLd1TrayExistChk		=	8;		//PS0002
	st_io_info.i_ReadyLd1CvyStopUpChk		=	9;		//PS0003
	st_io_info.i_ReadyLd1CvyStopDnChk		=	10;		//PS0004
	st_io_info.i_ReadyLd2CvyInTrayChk		=	11;		//PS0005
	st_io_info.i_ReadyLd2TrayExistChk		=	12;		//PS0006
	st_io_info.i_ReadyLd2CvyStopUpChk		=	13;		//PS0007
	st_io_info.i_ReadyLd2CvyStopDnChk		=	14;		//PS0008
	st_io_info.i_ReadyCvyLd12OutTrayPassChk	=	15;		//PS0000

	st_io_info.o_ReadyLdCvyStkStopUpDnCyl[0] = st_io_info.o_ReadyLd1CvyStkStopUpDnCyl;
	st_io_info.o_ReadyLdCvyStkStopUpDnCyl[1] = st_io_info.o_ReadyLd2CvyStkStopUpDnCyl;
	st_io_info.i_ReadyLdCvyStkStopUpDnCyl[0][0] = st_io_info.i_ReadyLd1CvyStopUpChk;
	st_io_info.i_ReadyLdCvyStkStopUpDnCyl[0][1] = st_io_info.i_ReadyLd1CvyStopDnChk;
	st_io_info.i_ReadyLdCvyStkStopUpDnCyl[1][0] = st_io_info.i_ReadyLd2CvyStopUpChk;
	st_io_info.i_ReadyLdCvyStkStopUpDnCyl[1][1] = st_io_info.i_ReadyLd2CvyStopDnChk;

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 01
	///////////////////////////////////////////////////////////////////////////////////////////////
	st_io_info.o_LdTrayPlateLfOnOffCyl		=	100;	// S0100
	st_io_info.o_LdTrayPlateRiOnOffCyl		=	101;	// S0101
	st_io_info.o_LdStkUpCyl					=	102;	// S0102		
	st_io_info.o_LdStkDnCyl					=	103;	// S0103		
	st_io_info.o_LdStkAcMotOnOff			=	104;	// S0104	
	// S0105
	// S0106
	// S0107
	st_io_info.i_LdTrayPlateLfOnChk			=	108;	//PS0100
	st_io_info.i_LdTrayPlateLfOffChk		=	109;	//PS0101
	st_io_info.i_LdTrayPlateRiOnChk			=	110;	//PS0102
	st_io_info.i_LdTrayPlateRiOffChk		=	111;	//PS0103
	st_io_info.i_LdStkUpChk					=	112;	//PS0104		
	st_io_info.i_LdStkDnChk					=	113;	//PS0105		
	st_io_info.i_LdStkTrayChk				=	114;	//PS0106  // 스태커에 붙어서 트레이 유무감지하는센서
	st_io_info.i_LdStkTrayArrivedChk		=	115;	//PS0107  //컨베이어로 이동하여 로더 엘리베이터에 도착하는 센서. Reject Load에도 있음

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 02
	///////////////////////////////////////////////////////////////////////////////////////////////
	st_io_info.o_EmptyStkUpCyl				=	200;	// S0200	
	st_io_info.o_EmptyStkDnCyl				=	201;	// S0201	
	st_io_info.o_ReadyUld1CvyStopCyl		=	202;	// S0202
	st_io_info.o_ReadyUld2CvyStopCyl		=	203;	// S0203
	// S0204
	st_io_info.i_EmptyStkUpChk				=	205;	//PS0200	
	st_io_info.i_EmptyStkDnChk				=	206;	//PS0201	
	st_io_info.i_EmptyStkTrayChk			=	207;	//PS0202
	st_io_info.i_ReadyUld1CvyPassTrayChk	=	208;	//PS0203
	st_io_info.i_ReadyUld1CvyOutTrayChk		=	209;	//PS0205
	st_io_info.i_ReadyUld1CvyStopUpChk		=	210;	//PS0207
	st_io_info.i_ReadyUld1CvyStopDnChk		=	211;	//PS0208
	st_io_info.i_ReadyUld2CvyPassTrayChk	=	212;	//PS0204
	st_io_info.i_ReadyUld2CvyOutTrayChk		=	213;	//PS0206
	st_io_info.i_ReadyUld2CvyStopUpChk		=	214;	//PS0209
	st_io_info.i_ReadyUld2CvyStopDnChk		=	215;	//PS0210

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 03
	///////////////////////////////////////////////////////////////////////////////////////////////
	st_io_info.o_Uld1StkUpCyl				=	300;		// S0300		
	st_io_info.o_Uld1StkDnCyl				=	301;		// S0301	
	st_io_info.o_Uld2StkUpCyl				=	302;		// S0302	
	st_io_info.o_Uld2StkDnCyl				=	303;		// S0303	
	st_io_info.o_Uld1StkAcMotOnOff			=	304;		// S0304
	st_io_info.o_Uld2StkAcMotOnOff			=	305;		// S0305
	// S0306
	// S0307
	st_io_info.i_Uld1StkUpChk				=	308;		//PS0300	
	st_io_info.i_Uld1StkDnChk				=	309;		//PS0301	
	st_io_info.i_Uld2StkUpChk				=	310;		//PS0302	
	st_io_info.i_Uld2StkDnChk				=	311;		//PS0303	
	st_io_info.i_Uld1StkTrayExistChk		=	312;		//PS0304		
	st_io_info.i_Uld2StkTrayExistChk		=	313;		//PS0305
	//PS0306
	//PS0307


	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 04
	///////////////////////////////////////////////////////////////////////////////////////////////
	st_io_info.o_TrayTrsfUpCyl				=	400;		// S0400
	st_io_info.o_TrayTrsfDnCyl				=	401;		// S0401
	st_io_info.o_TrayTrsfVacOnOff			=	402;		// S0404
	st_io_info.o_TrayTrsfEjtOnOff			=	403;		// S0406
	st_io_info.o_RejTrsfUpCyl				=	404;		// S0402
	st_io_info.o_RejTrsfDnCyl				=	405;		// S0403
	st_io_info.o_RejTrsfVacOnOff			=	406;		// S0405
	st_io_info.o_RejTrsfEjtOnOff			=	407;		// S0407
	st_io_info.i_TrayTrsfUpChk				=	408;		//PS0400
	st_io_info.i_TrayTrsfDnChk				=	409;		//PS0401
	st_io_info.i_TrayTrsfVacChk				=	410;		//PS0404
	st_io_info.i_RejTrsfUpChk				=	411;		//PS0402
	st_io_info.i_RejTrsfDnChk				=	412;		//PS0403
	st_io_info.i_RejTrsfVacChk				=	413;		//PS0405
	//PS0406
	//PS0407

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 05
	///////////////////////////////////////////////////////////////////////////////////////////////
	st_io_info.o_Rej1UldCvyStopUpDnCyl		=	500;		// S0500
	st_io_info.o_Rej2UldCvyStopUpDnCyl		=	501;		// S0501
	st_io_info.o_RejEmptyStkUpCyl			=	502;		// S0502
	st_io_info.o_RejEmptyStkDnCyl			=	503;		// S0503
	st_io_info.o_Rej1StkUpCyl				=	504;		// S0504
	st_io_info.o_Rej1StkDnCyl				=	505;		// S0505
	st_io_info.i_Rej1UldCvyStopUpChk		=	506;		//PS0500
	st_io_info.i_Rej1UldCvyStopDnChk		=	507;		//PS0501
	st_io_info.i_Rej2UldCvyStopUpChk		=	508;		//PS0502
	st_io_info.i_Rej2UldCvyStopDnChk		=	509;		//PS0503
	st_io_info.i_RejEmptyStkUpChk			=	510;		//PS0504
	st_io_info.i_RejEmptyStkDnChk			=	511;		//PS0505
	st_io_info.i_Rej1StkUpChk				=	512;		//PS0506
	st_io_info.i_Rej1StkDnChk				=	513;		//PS0507

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 06
	///////////////////////////////////////////////////////////////////////////////////////////////
	st_io_info.o_RejEmptyCvyStkAcMotOnOff	=	600;		// S0600
	st_io_info.o_Rej1UldCvyStkAcMotOnOff	=	601;		// S0601
	st_io_info.o_Rej2UldCvyStkAcMotOnOff	=	602;		// S0602
	st_io_info.o_Rej2StkUpCyl				=	603;		// S0603
	st_io_info.o_Rej2StkDnCyl				=	604;		// S0604
	// S0605
	st_io_info.i_RejEmptyStkTrayExistChk	=	606;		// PS0600
	st_io_info.i_RejEmptyCvyInTrayExistChk	=	607;		// PS0601
	st_io_info.i_RejEmptyTrayArrivedChk		=	608;		// PS0602
	st_io_info.i_Rej1UldCvyTrayExistChk		=	609;		// PS0603
	st_io_info.i_Rej1StkTrayExistChk		=	610;		// PS0604
	st_io_info.i_Rej2UldCvyTrayExistChk		=	611;		// PS0605
	st_io_info.i_Rej2StkTrayExistChk		=	612;		// PS0606
	st_io_info.i_Rej2StkUpChk				=	613;		// PS0607
	st_io_info.i_Rej2StkDnChk				=	614;		// PS0608
	//PS0609


	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 07
	///////////////////////////////////////////////////////////////////////////////////////////////
	st_io_info.o_LdUldPickUpCyl				=	700;		// S0700
	st_io_info.o_LdUldPickDnCyl				=	701;		// S0701
	st_io_info.o_LdUldPickRot_0_Cyl			=	702;		// S0702
	st_io_info.o_LdUldPickRot_90_Cyl		=	703;		// S0703
	st_io_info.o_LdUldPickGripCyl			=	704;		// S0704
	//S0705
	st_io_info.i_LdUldPickUpChk				=	708;		//PS0700
	st_io_info.i_LdUldPickDnChk				=	709;		//PS0701
	st_io_info.i_LdUldPickRot_0_Chk			=	710;		//PS0702
	st_io_info.i_LdUldPickRot_90_Chk		=	711;		//PS0703
	st_io_info.i_LdUldPickDvcChk			=	712;		//PS0704
	//PS0705
	//PS0706
	//PS0707

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 10
	///////////////////////////////////////////////////////////////////////////////////////////////
	st_io_info.o_LdBuffGripOnOffCyl[0]		=	st_io_info.o_LdBuff1GripOnOffCyl		=	1000;		// S1000
	st_io_info.o_LdBuffGripOnOffCyl[1]		=	st_io_info.o_LdBuff2GripOnOffCyl		=	1001;		// S1001
	st_io_info.o_LdBuffGripOnOffCyl[2]		=	st_io_info.o_LdBuff3GripOnOffCyl		=	1002;		// S1002
	st_io_info.o_LdBuffGripOnOffCyl[3]		=	st_io_info.o_LdBuff4GripOnOffCyl		=	1003;		// S1003
	st_io_info.o_UldBuffGripOnOffCyl[0]		=	st_io_info.o_UldBuff1GripOnOffCyl		=	1004;		// S1004
	st_io_info.o_UldBuffGripOnOffCyl[1]		=	st_io_info.o_UldBuff2GripOnOffCyl		=	1005;		// S1005
	st_io_info.o_UldBuffGripOnOffCyl[2]		=	st_io_info.o_UldBuff3GripOnOffCyl		=	1006;		// S1006
	st_io_info.o_UldBuffGripOnOffCyl[3]		=	st_io_info.o_UldBuff4GripOnOffCyl		=	1007;		// S1007
	st_io_info.i_LdBuffGripDvcExistChk[0]	=	st_io_info.i_LdBuff1GripDvcExistChk		=	1008;		//PS1000
	st_io_info.i_LdBuffGripDvcExistChk[1]	=	st_io_info.i_LdBuff2GripDvcExistChk		=	1009;		//PS1001
	st_io_info.i_LdBuffGripDvcExistChk[2]	=	st_io_info.i_LdBuff3GripDvcExistChk		=	1010;		//PS1002
	st_io_info.i_LdBuffGripDvcExistChk[3]	=	st_io_info.i_LdBuff4GripDvcExistChk		=	1011;		//PS1003
	st_io_info.i_UldBuffGripDvcExistChk[0]	=	st_io_info.i_UldBuff1GripDvcExistChk	=	1012;		//PS1004
	st_io_info.i_UldBuffGripDvcExistChk[1]	=	st_io_info.i_UldBuff2GripDvcExistChk	=	1013;		//PS1005
	st_io_info.i_UldBuffGripDvcExistChk[2]	=	st_io_info.i_UldBuff3GripDvcExistChk	=	1014;		//PS1006
	st_io_info.i_UldBuffGripDvcExistChk[3]	=	st_io_info.i_UldBuff4GripDvcExistChk	=	1015;		//PS1007

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 11
	///////////////////////////////////////////////////////////////////////////////////////////////
	st_io_info.i_Ret1BuffDvcExistChk	=	1100;		//PS1100
	//PS1101
	//PS1102
	//PS1103
	//PS1104
	//PS1105
	//PS1106
	//PS1107
	st_io_info.i_Ret2BuffDvcExistChk	=	1108;		//PS1108
	//PS1109
	//PS1110
	//PS1111
	//PS1112
	//PS1113
	//PS1114
	//PS1115

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 12
	///////////////////////////////////////////////////////////////////////////////////////////////
	st_io_info.o_LdUldMuteOn				=	1200;		// S1200
	st_io_info.o_LdUldMuteOff				=	1201;		// S1201
	st_io_info.o_RejMuteOn					=	1202;		// S1202
	st_io_info.o_RejMuteOff					=	1203;		// S1203
	// S1204
	// S1205
	// S1206
	// S1207
	st_io_info.i_LdUldMuteOnChk				=	1208;		//PS1200
	st_io_info.i_LdUldMuteOffChk			=	1209;		//PS1201
	st_io_info.i_RejMuteOnChk				=	1210;		//PS1202
	st_io_info.i_RejMuteOffChk				=	1211;		//PS1203
	st_io_info.i_LdUldStkLctChk				=	1212;		//PS1204
	st_io_info.i_RejStkLctChk				=	1213;		//PS1205
	st_io_info.i_LdUldLightChk				=	1214;		//PS1206
	st_io_info.i_RejectLightChk				=	1215;		//PS1207

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 13
	///////////////////////////////////////////////////////////////////////////////////////////////
	st_io_info.o_StartLamp					= 1300;			// S1300
	st_io_info.o_StopLamp					= 1301;			// S1301	
	st_io_info.o_AlarmLamp					= 1302;			// S1302	
	st_io_info.o_BuzzLamp					= 1303;			// S1303
	//S1304
	//S1305									
	//S1306									
	//S1307
	st_io_info.i_StartChk					= 1308;			//PS1300
	st_io_info.i_StopChk					= 1309;			//PS1301
	st_io_info.i_AlarmChk					= 1310;			//PS1302
	st_io_info.i_BuzzChk					= 1311;			//PS1303
	st_io_info.i_AutoModeChk				= 1312;			//PS1304
	st_io_info.i_ManualModeChk				= 1313;			//PS1305
	st_io_info.i_FrontSelectSwChk			= 1314;			//PS1306
	st_io_info.i_RearSelectSwChk			= 1315;			//PS1307

	
	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 14
	///////////////////////////////////////////////////////////////////////////////////////////////
	// S1400
	st_io_info.o_TwLamp[0]		=		st_io_info.o_TwGreenLamp	= 1401;		// S1401
	st_io_info.o_TwLamp[1]		=		st_io_info.o_TwYellowLamp	= 1402;		// S1402
	st_io_info.o_TwLamp[2]		=		st_io_info.o_TwRedLamp		= 1403;		// S1403
	st_io_info.o_BuzzOnOff[0]	=		st_io_info.o_Buzz1OnOff		= 1404;		// S1404
	st_io_info.o_BuzzOnOff[1]	=		st_io_info.o_Buzz2OnOff		= 1405;		// S1405
	st_io_info.o_BuzzOnOff[2]	=		st_io_info.o_Buzz3OnOff		= 1406;		// S1406
	st_io_info.o_DoorLockLdUld												= 1407;		// S1407
	//PS1400	
	st_io_info.i_MainMcChk													= 1409;		//PS1401	
	st_io_info.i_EmoChk[0]		=		st_io_info.i_Emo1SwChk		= 1410;		//PS1402
	st_io_info.i_EmoChk[1]		=		st_io_info.i_Emo2SwChk		= 1411;		//PS1403
	st_io_info.i_EmoChk[2]		=		st_io_info.i_Emo3SwChk		= 1412;		//PS1404	
	st_io_info.i_EmoChk[3]		=		st_io_info.i_Emo4SwChk		= 1413;		//PS1405	
	//PS1406
	//PS1407

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 15
	///////////////////////////////////////////////////////////////////////////////////////////////
	st_io_info.i_TopLdUldDoorChk[0]		=	st_io_info.i_TopLdUldDoor1Chk		=	1500;		//PS1500
	st_io_info.i_TopLdUldDoorChk[1]		=	st_io_info.i_TopLdUldDoor2Chk		=	1501;		//PS1501
	st_io_info.i_TopLdUldDoorChk[2]		=	st_io_info.i_TopLdUldDoor3Chk		=	1502;		//PS1502
	st_io_info.i_TopLdUldDoorChk[3]		=	st_io_info.i_TopLdUldDoor4Chk		=	1503;		//PS1503
	st_io_info.i_TopLdUldDoorChk[4]		=	st_io_info.i_TopLdUldDoor5Chk		=	1504;		//PS1504
	st_io_info.i_TopLdUldDoorChk[5]		=	st_io_info.i_TopLdUldDoor6Chk		=	1505;		//PS1505
	st_io_info.i_TopLdUldDoorChk[6]		=	st_io_info.i_TopLdUldDoor7Chk		=	1506;		//PS1506
	st_io_info.i_TopLdUldDoorChk[7]		=	st_io_info.i_TopLdUldDoor8Chk		=	1507;		//PS1507
	st_io_info.i_TopLdUldDoorChk[8]		=	st_io_info.i_TopLdUldDoor9Chk		=	1508;		//PS1508
	st_io_info.i_TopLdUldDoorChk[9]		=	st_io_info.i_TopLdUldDoor10Chk		=	1509;		//PS1509
	st_io_info.i_BtmLdUldDoorChk[0]		=	st_io_info.i_BtmLdUldDoor1Chk		=	1510;		//PS1510
	st_io_info.i_BtmLdUldDoorChk[1]		=	st_io_info.i_BtmLdUldDoor2Chk		=	1511;		//PS1511
	st_io_info.i_BtmLdUldDoorChk[2]		=	st_io_info.i_BtmLdUldDoor3Chk		=	1512;		//PS1512
	st_io_info.i_BtmLdUldDoorChk[3]		=	st_io_info.i_BtmLdUldDoor4Chk		=	1513;		//PS1513
	//PS1514
	//PS1515

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 16
	///////////////////////////////////////////////////////////////////////////////////////////////
	st_io_info.i_Fan_StopChk[0]			=	st_io_info.i_Fan_1StopChk			=	1600;		//PS1600
	st_io_info.i_Fan_StopChk[1]			=	st_io_info.i_Fan_2StopChk			=	1601;		//PS1601
	st_io_info.i_Fan_StopChk[2]			=	st_io_info.i_Fan_3StopChk			=	1602;		//PS1602
	st_io_info.i_Fan_StopChk[3]			=	st_io_info.i_Fan_4StopChk			=	1603;		//PS1603
	st_io_info.i_Fan_StopChk[4]			=	st_io_info.i_Fan_5StopChk			=	1604;		//PS1604
	st_io_info.i_Fan_StopChk[5]			=	st_io_info.i_Fan_6StopChk			=	1605;		//PS1605
	st_io_info.i_Fan_StopChk[6]			=	st_io_info.i_Fan_7StopChk			=	1606;		//PS1606
	st_io_info.i_Fan_StopChk[7]			=	st_io_info.i_Fan_8StopChk			=	1607;		//PS1607
	st_io_info.i_NonContect_DoorChk_1												=  1608;		
	st_io_info.i_NonContect_DoorChk_2												=  1609;		

	st_io_info.i_MainBoxSmokeChk		=	1612;		//PS1612
	st_io_info.i_ServoBox_2SmokeChk		=	1613;		//PS1613


	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 17
	///////////////////////////////////////////////////////////////////////////////////////////////

	//spaere

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 20
	///////////////////////////////////////////////////////////////////////////////////////////////
/*	st_io_info.o_ChamberAcMotCose[0]	=	st_io_info.o_Chamber1AcMotClose		=	2000;		// S2200
	st_io_info.o_ChamberAcMotOpen[0]	=	st_io_info.o_Chamber1AcMotOpen		=	2001;		// S2201
	st_io_info.o_ChamberAcMotCose[1]	=	st_io_info.o_Chamber2AcMotClose		=	2002;		// S2202
	st_io_info.o_ChamberAcMotOpen[1]	=	st_io_info.o_Chamber2AcMotOpen		=	2003;		// S2203
	st_io_info.o_ChamberAcMotCose[2]	=	st_io_info.o_Chamber3AcMotClose		=	2004;		// S2204
	st_io_info.o_ChamberAcMotOpen[2]	=	st_io_info.o_Chamber3AcMotOpen		=	2005;		// S2205
	st_io_info.o_ChamberAcMotCose[3]	=	st_io_info.o_Chamber4AcMotClose		=	2006;		// S2206
	st_io_info.o_ChamberAcMotOpen[3]	=	st_io_info.o_Chamber4AcMotOpen		=	2007;		// S2207
	st_io_info.o_ChamberAcMotCose[4]	=	st_io_info.o_Chamber5AcMotClose		=	2008;		// S2208
	st_io_info.o_ChamberAcMotOpen[4]	=	st_io_info.o_Chamber5AcMotOpen		=	2009;		// S2209
	st_io_info.o_ChamberAcMotCose[5]	=	st_io_info.o_Chamber6AcMotClose		=	2010;		// S2210
	st_io_info.o_ChamberAcMotOpen[5]	=	st_io_info.o_Chamber6AcMotOpen		=	2011;		// S2211
	st_io_info.o_ChamberAcMotCose[6]	=	st_io_info.o_Chamber7AcMotClose		=	2012;		// S2212
	st_io_info.o_ChamberAcMotOpen[6]	=	st_io_info.o_Chamber7AcMotOpen		=	2013;		// S2213
	st_io_info.o_ChamberAcMotCose[7]	=	st_io_info.o_Chamber8AcMotClose		=	2014;		// S2214
	st_io_info.o_ChamberAcMotOpen[7]	=	st_io_info.o_Chamber8AcMotOpen		=	2015;		// S2215*/
	st_io_info.o_ChamberAcMotCose[0]	=	st_io_info.o_Chamber1AcMotClose		=	2001;		// S2200
	st_io_info.o_ChamberAcMotOpen[0]	=	st_io_info.o_Chamber1AcMotOpen		=	2000;		// S2201
	st_io_info.o_ChamberAcMotCose[1]	=	st_io_info.o_Chamber2AcMotClose		=	2003;		// S2202
	st_io_info.o_ChamberAcMotOpen[1]	=	st_io_info.o_Chamber2AcMotOpen		=	2002;		// S2203
	st_io_info.o_ChamberAcMotCose[2]	=	st_io_info.o_Chamber3AcMotClose		=	2005;		// S2204
	st_io_info.o_ChamberAcMotOpen[2]	=	st_io_info.o_Chamber3AcMotOpen		=	2004;		// S2205
	st_io_info.o_ChamberAcMotCose[3]	=	st_io_info.o_Chamber4AcMotClose		=	2007;		// S2206
	st_io_info.o_ChamberAcMotOpen[3]	=	st_io_info.o_Chamber4AcMotOpen		=	2006;		// S2207
	st_io_info.o_ChamberAcMotCose[4]	=	st_io_info.o_Chamber5AcMotClose		=	2009;		// S2208
	st_io_info.o_ChamberAcMotOpen[4]	=	st_io_info.o_Chamber5AcMotOpen		=	2008;		// S2209
	st_io_info.o_ChamberAcMotCose[5]	=	st_io_info.o_Chamber6AcMotClose		=	2011;		// S2210
	st_io_info.o_ChamberAcMotOpen[5]	=	st_io_info.o_Chamber6AcMotOpen		=	2010;		// S2211
	st_io_info.o_ChamberAcMotCose[6]	=	st_io_info.o_Chamber7AcMotClose		=	2013;		// S2212
	st_io_info.o_ChamberAcMotOpen[6]	=	st_io_info.o_Chamber7AcMotOpen		=	2012;		// S2213
	st_io_info.o_ChamberAcMotCose[7]	=	st_io_info.o_Chamber8AcMotClose		=	2015;		// S2214
	st_io_info.o_ChamberAcMotOpen[7]	=	st_io_info.o_Chamber8AcMotOpen		=	2014;		// S2215

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 21
	///////////////////////////////////////////////////////////////////////////////////////////////
	st_io_info.o_TsiteUpCyl[0]			=	st_io_info.o_Tsite1UpCyl			=	2100;		// S2100
	st_io_info.o_TsiteDnCyl[0]			=	st_io_info.o_Tsite1DnCyl			=	2101;		// S2101
	st_io_info.o_TsiteUpCyl[1]			=	st_io_info.o_Tsite2UpCyl			=	2102;		// S2102
	st_io_info.o_TsiteDnCyl[1]			=	st_io_info.o_Tsite2DnCyl			=	2103;		// S2103
	st_io_info.o_TsiteUpCyl[2]			=	st_io_info.o_Tsite3UpCyl			=	2104;		// S2104
	st_io_info.o_TsiteDnCyl[2]			=	st_io_info.o_Tsite3DnCyl			=	2105;		// S2105
	st_io_info.o_TsiteUpCyl[3]			=	st_io_info.o_Tsite4UpCyl			=	2106;		// S2106
	st_io_info.o_TsiteDnCyl[3]			=	st_io_info.o_Tsite4DnCyl			=	2107;		// S2107

	st_io_info.i_TsiteLockOnChk[0]		=	st_io_info.i_Tsite1LockOnChk		=	2108;		//PS2100
	st_io_info.i_TsiteExistChk[0]		=	st_io_info.i_Tsite1ExistChk			=	2109;		//PS2100
	st_io_info.i_TsiteUpChk[0]			=	st_io_info.i_Tsite1UpChk			=	2110;		//PS2100
	st_io_info.i_TsiteDnChk[0]			=	st_io_info.i_Tsite1DnChk			=	2111;		//PS2100
	st_io_info.i_ChamberOpenChk[0]		=	st_io_info.i_Chamber1OpenChk		=	2112;		//PS2100
	st_io_info.i_ChamberCloseChk[0]		=	st_io_info.i_Chamber1CloseChk		=	2113;		//PS2100
	st_io_info.i_TsiteLockOnChk[1]		=	st_io_info.i_Tsite2LockOnChk		=	2114;		//PS2100
	st_io_info.i_TsiteExistChk[1]		=	st_io_info.i_Tsite2ExistChk			=	2115;		//PS2100

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 22
	///////////////////////////////////////////////////////////////////////////////////////////////
	st_io_info.i_TsiteUpChk[1]			=	st_io_info.i_Tsite2UpChk			=	2200;		//PS2200
	st_io_info.i_TsiteDnChk[1]			=	st_io_info.i_Tsite2DnChk			=	2201;		//PS2201
	st_io_info.i_ChamberOpenChk[1]		=	st_io_info.i_Chamber2OpenChk		=	2202;		//PS2202
	st_io_info.i_ChamberCloseChk[1]		=	st_io_info.i_Chamber2CloseChk		=	2203;		//PS2203
	st_io_info.i_TsiteLockOnChk[2]		=	st_io_info.i_Tsite3LockOnChk		=	2204;		//PS2204
	st_io_info.i_TsiteExistChk[2]		=	st_io_info.i_Tsite3ExistChk			=	2205;		//PS2205
	st_io_info.i_TsiteUpChk[2]			=	st_io_info.i_Tsite3UpChk			=	2206;		//PS2206
	st_io_info.i_TsiteDnChk[2]			=	st_io_info.i_Tsite3DnChk			=	2207;		//PS2207
	st_io_info.i_ChamberOpenChk[2]		=	st_io_info.i_Chamber3OpenChk		=	2208;		//PS2208
	st_io_info.i_ChamberCloseChk[2]		=	st_io_info.i_Chamber3CloseChk		=	2209;		//PS2209
	st_io_info.i_TsiteLockOnChk[3]		=	st_io_info.i_Tsite4LockOnChk		=	2210;		//PS2210
	st_io_info.i_TsiteExistChk[3]		=	st_io_info.i_Tsite4ExistChk			=	2211;		//PS2211
	st_io_info.i_TsiteUpChk[3]			=	st_io_info.i_Tsite4UpChk			=	2212;		//PS2212
	st_io_info.i_TsiteDnChk[3]			=	st_io_info.i_Tsite4DnChk			=	2213;		//PS2213
	st_io_info.i_ChamberOpenChk[3]		=	st_io_info.i_Chamber4OpenChk		=	2214;		//PS2214
	st_io_info.i_ChamberCloseChk[3]		=	st_io_info.i_Chamber4CloseChk		=	2215;		//PS2215


	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 23
	///////////////////////////////////////////////////////////////////////////////////////////////	
	st_io_info.o_TsiteUpCyl[4]			=	st_io_info.o_Tsite5UpCyl			=	2300;		// S2300
	st_io_info.o_TsiteDnCyl[4]			=	st_io_info.o_Tsite5DnCyl			=	2301;		// S2300
	st_io_info.o_TsiteUpCyl[5]			=	st_io_info.o_Tsite6UpCyl			=	2302;		// S2300
	st_io_info.o_TsiteDnCyl[5]			=	st_io_info.o_Tsite6DnCyl			=	2303;		// S2300
	st_io_info.o_TsiteUpCyl[6]			=	st_io_info.o_Tsite7UpCyl			=	2304;		// S2300
	st_io_info.o_TsiteDnCyl[6]			=	st_io_info.o_Tsite7DnCyl			=	2305;		// S2300
	st_io_info.o_TsiteUpCyl[7]			=	st_io_info.o_Tsite8UpCyl			=	2306;		// S2300
	st_io_info.o_TsiteDnCyl[7]			=	st_io_info.o_Tsite8DnCyl			=	2307;		// S2300

	st_io_info.i_TsiteLockOnChk[4]		=	st_io_info.i_Tsite5LockOnChk		=	2308;		//PS2300
	st_io_info.i_TsiteExistChk[4]		=	st_io_info.i_Tsite5ExistChk			=	2309;		//PS2301
	st_io_info.i_TsiteUpChk[4]			=	st_io_info.i_Tsite5UpChk			=	2310;		//PS2302
	st_io_info.i_TsiteDnChk[4]			=	st_io_info.i_Tsite5DnChk			=	2311;		//PS2303
	st_io_info.i_ChamberOpenChk[4]		=	st_io_info.i_Chamber5OpenChk		=	2312;		//PS2304
	st_io_info.i_ChamberCloseChk[4]		=	st_io_info.i_Chamber5CloseChk		=	2313;		//PS2305
	st_io_info.i_TsiteLockOnChk[5]		=	st_io_info.i_Tsite6LockOnChk		=	2314;		//PS2306
	st_io_info.i_TsiteExistChk[5]		=	st_io_info.i_Tsite6ExistChk			=	2315;		//PS2307

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 24
	///////////////////////////////////////////////////////////////////////////////////////////////	
	st_io_info.i_TsiteUpChk[5]			=	st_io_info.i_Tsite6UpChk			=	2400;		//PS2400
	st_io_info.i_TsiteDnChk[5]			=	st_io_info.i_Tsite6DnChk			=	2401;		//PS2401
	st_io_info.i_ChamberOpenChk[5]		=	st_io_info.i_Chamber6OpenChk		=	2402;		//PS2402
	st_io_info.i_ChamberCloseChk[5]		=	st_io_info.i_Chamber6CloseChk		=	2403;		//PS2403
	st_io_info.i_TsiteLockOnChk[6]		=	st_io_info.i_Tsite7LockOnChk		=	2404;		//PS2404
	st_io_info.i_TsiteExistChk[6]		=	st_io_info.i_Tsite7ExistChk			=	2405;		//PS2405
	st_io_info.i_TsiteUpChk[6]			=	st_io_info.i_Tsite7UpChk			=	2406;		//PS2406
	st_io_info.i_TsiteDnChk[6]			=	st_io_info.i_Tsite7DnChk			=	2407;		//PS2407
	st_io_info.i_ChamberOpenChk[6]		=	st_io_info.i_Chamber7OpenChk		=	2408;		//PS2408
	st_io_info.i_ChamberCloseChk[6]		=	st_io_info.i_Chamber7CloseChk		=	2409;		//PS2409
	st_io_info.i_TsiteLockOnChk[7]		=	st_io_info.i_Tsite8LockOnChk		=	2410;		//PS2410
	st_io_info.i_TsiteExistChk[7]		=	st_io_info.i_Tsite8ExistChk			=	2411;		//PS2411
	st_io_info.i_TsiteUpChk[7]			=	st_io_info.i_Tsite8UpChk			=	2412;		//PS2412
	st_io_info.i_TsiteDnChk[7]			=	st_io_info.i_Tsite8DnChk			=	2413;		//PS2413
	st_io_info.i_ChamberOpenChk[7]		=	st_io_info.i_Chamber8OpenChk		=	2414;		//PS2414
	st_io_info.i_ChamberCloseChk[7]		=	st_io_info.i_Chamber8CloseChk		=	2415;		//PS2415	

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 25
	///////////////////////////////////////////////////////////////////////////////////////////////	
	st_io_info.o_TestRbtPickerGripCyl[0]			=	st_io_info.o_TestRbtPicker1GripCyl			=	2500;	// S2500
	st_io_info.o_TestRbtPickerGripCyl[1]			=	st_io_info.o_TestRbtPicker2GripCyl			=	2501;	// S2501
	st_io_info.o_TestRbtPickerGripCyl[2]			=	st_io_info.o_TestRbtPicker3GripCyl			=	2502;	// S2502
	st_io_info.o_TestRbtPickerGripCyl[3]			=	st_io_info.o_TestRbtPicker4GripCyl			=	2503;	// S2503
	//S2504
	//S2505
	//S2506
	//S2507
	st_io_info.i_TestRbtPickerGripDvcExistChk[0]	=	st_io_info.i_TestRbtPicker1GripDvcExistChk	=	2508;	//PS2500
	st_io_info.i_TestRbtPickerGripDvcExistChk[1]	=	st_io_info.i_TestRbtPicker2GripDvcExistChk	=	2509;	//PS2501
	st_io_info.i_TestRbtPickerGripDvcExistChk[2]	=	st_io_info.i_TestRbtPicker3GripDvcExistChk	=	2510;	//PS2502
	st_io_info.i_TestRbtPickerGripDvcExistChk[3]	=	st_io_info.i_TestRbtPicker4GripDvcExistChk	=	2511;	//PS2503
	//PS2504
	//PS2505
	//PS2506
	//PS2507

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 26
	///////////////////////////////////////////////////////////////////////////////////////////////
	st_io_info.o_MainAir																			= 2600;		
	st_io_info.i_DvcDir1Chk																			= 2605;	
	st_io_info.i_DvcDir2Chk																			= 2606;	
	st_io_info.i_MainAirChk																			= 2607;	
	st_io_info.i_CBox3SmokeChk																		= 2608;
	st_io_info.i_CBox4SmokeChk																		= 2609;//PS2604
	st_io_info.i_Emo5SwChk																			= 2610;//PS2605
	st_io_info.i_Emo6SwChk																			= 2611;//PS2606
	st_io_info.i_NonContect_DoorChk_3																= 2612;//PS2607
	st_io_info.i_NonContect_DoorChk_4																= 2613;//PS2608

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Module No 27
	///////////////////////////////////////////////////////////////////////////////////////////////
	// S2700
	// S2701
	st_io_info.o_DoorLockTestSite																	= 2703;
	st_io_info.i_TsiteDoorChk[0]					=   st_io_info.i_Tsite1_LeftDoor15Chk			= 2704;		//PS2700
	st_io_info.i_TsiteDoorChk[1]					=   st_io_info.i_Tsite2_LeftDoor16Chk			= 2705;		//PS2701
	st_io_info.i_TsiteDoorChk[2]					=   st_io_info.i_Tsite3_LeftDoor17Chk			= 2706;		//PS2702	
	st_io_info.i_TsiteDoorChk[3]					=   st_io_info.i_Tsite4_LeftDoor18Chk			= 2707;		//PS2703	
	st_io_info.i_TsiteDoorChk[4]					=   st_io_info.i_Tsite5_LeftDoor19Chk			= 2708;		//PS2706
	st_io_info.i_TsiteDoorChk[5]					=   st_io_info.i_Tsite6_LeftDoor20Chk			= 2709;		//PS2707	
	st_io_info.i_TsiteDoorChk[6]					=   st_io_info.i_Tsite7_LeftDoor21Chk			= 2710;		//PS2708	
	st_io_info.i_TsiteDoorChk[7]					=   st_io_info.i_Tsite8_LeftDoor22Chk			= 2711;		//PS2709	
	st_io_info.i_TsiteSmokeChk[0]					=	st_io_info.i_Tsite_Top12SmokeChk			= 2712;		//PS2704
	st_io_info.i_TsiteSmokeChk[1]					=	st_io_info.i_Tsite_Top34SmokeChk			= 2713;		//PS2705	
	st_io_info.i_TsiteSmokeChk[2]					=	st_io_info.i_Tsite_Top56SmokeChk			= 2714;		//PS2710
	st_io_info.i_TsiteSmokeChk[3]					=	st_io_info.i_Tsite_Top78SmokeChk			= 2715;		//PS2711

}


// ******************************************************************************
//  실린더 동작시키는  함수 (단동)                                               
// ******************************************************************************
void CFastechPublic_IO::OnCylinderAction(int n_cyl, int n_act)
{
	//Set_Out_Bit(n_cyl, n_act);
	set_out_bit(n_cyl, n_act);
	
	CYLTime[n_cyl][0] = GetCurrentTime();
	
	CYLFlag[n_cyl] = false;
}

// ******************************************************************************
//  실린더 동작시키는  함수 (복복동)                                             
// ******************************************************************************
void CFastechPublic_IO::OnCylinderAction(int n_cyl, int n_cyl2, int n_act)
{
	//Set_Out_Bit(n_cyl, n_act);
	//Set_Out_Bit(n_cyl2, !n_act);
	set_out_bit(n_cyl, n_act);
	set_out_bit(n_cyl2, !n_act);
	
	CYLTime[n_cyl][0] = GetCurrentTime();
	
	CYLFlag[n_cyl] = false;
}

// ******************************************************************************
//  실린더 동작 시킨후 확인하는 함수 (센서 하나용)                               
// ******************************************************************************
int CFastechPublic_IO::OnCylinderActionCheck(int n_cyl, int n_sen, int n_act, int n_wait, int n_limit)
{
	int Ret = RET_PROCEED;  // 함수 리턴 플래그 설정 
	
	if (get_in_bit(n_sen, n_act) == n_act && CYLFlag[n_cyl] == false)
	{
		CYLTime[n_cyl][0]	= GetCurrentTime();
		CYLFlag[n_cyl] = true;
	}
	else if (get_in_bit(n_sen, n_act) == n_act && CYLFlag[n_cyl] == true)
	{
		CYLTime[n_cyl][1] = GetCurrentTime();
		CYLTime[n_cyl][2] = CYLTime[n_cyl][1] - CYLTime[n_cyl][0];
		
		if (CYLTime[n_cyl][2] >= n_wait)
		{
			Ret = RET_GOOD;
		}
	}
	else
	{
		CYLTime[n_cyl][1] = GetCurrentTime();
		CYLTime[n_cyl][2] = CYLTime[n_cyl][1] - CYLTime[n_cyl][0];
		
		if (CYLTime[n_cyl][2] >= n_limit)
		{
			Ret = RET_ERROR;
		}
	}
	return Ret;
}

// ******************************************************************************
//  실린더 동작 시킨후 확인하는 함수 (센서 두개용)                               
// ******************************************************************************
int CFastechPublic_IO::OnCylinderActionCheck(int n_cyl, int n_sen1, int n_sen2, int n_act, int n_wait, int n_limit)
{
	int Ret = RET_PROCEED;  // 함수 리턴 플래그 설정 
	
	if (get_in_bit(n_sen1, n_act) == n_act && 
		get_in_bit(n_sen2, !n_act) == !n_act && 
		CYLFlag[n_cyl] == false)
	{
		CYLTime[n_cyl][0]	= GetCurrentTime();
		CYLFlag[n_cyl] = true;
	}
	else if (get_in_bit(n_sen1, n_act) == n_act && 
		get_in_bit(n_sen2, !n_act) == !n_act && 
		CYLFlag[n_cyl] == true)
	{
		CYLTime[n_cyl][1] = GetCurrentTime();
		CYLTime[n_cyl][2] = CYLTime[n_cyl][1] - CYLTime[n_cyl][0];
		
		if (CYLTime[n_cyl][2] >= n_wait)
		{
			Ret = RET_GOOD;
		}
	}
	else
	{
		CYLTime[n_cyl][1] = GetCurrentTime();
		CYLTime[n_cyl][2] = CYLTime[n_cyl][1] - CYLTime[n_cyl][0];
		
		if (CYLTime[n_cyl][2] >= n_limit)
		{
			Ret = RET_ERROR;
		}
	}
	return Ret;
}


//james 2016.0810 센서 없이 동작 시키는 실린더 등을 제어하기 위해 추가 
void CFastechPublic_IO::Set_IO_OnOff(int nIONo, int OnOff)
{
	 
	set_out_bit(nIONo, OnOff); 
	
	CYLTime[nIONo][0] = GetCurrentTime();
	
	CYLFlag[nIONo] = false;
}
//james 2016.0810 센서 없이 동작 시키는 실린더 등을 제어하기 위해 추가 
int CFastechPublic_IO::Chk_IO_OnOff(int nIONo, int OnOff, int nWaitTime, int nLimitTime)
{
	 
	int Ret = RET_PROCEED;  // 함수 리턴 플래그 설정 
	int nIO;

	nIO = get_in_bit(nIONo, OnOff);
	if (CYLFlag[nIONo] == false && nIO	== OnOff)
	{
		CYLTime[nIONo][0]	= GetCurrentTime();
		CYLFlag[nIONo] = true;
	}
	else if (CYLFlag[nIONo] == true && nIO	== OnOff)
	{
		CYLTime[nIONo][1] = GetCurrentTime();
		CYLTime[nIONo][2] = CYLTime[nIONo][1] - CYLTime[nIONo][0];
		//if(CYLTime[nIONo][2] >= 20000 || CYLTime[nIONo][2] < 0) //wait 시간 limit 설정 
		if(CYLTime[nIONo][2] < 0) //wait 시간 limit 설정 
		{
			CYLTime[nIONo][0] = GetCurrentTime();
			CYLFlag[nIONo] = false;
			return RET_PROCEED;
		}

		if (CYLTime[nIONo][2] >= nWaitTime)
		{
			Ret = RET_GOOD;
			CYLFlag[nIONo] = false;
		}
	} 
	else
	{
		CYLTime[nIONo][1] = GetCurrentTime();
		CYLTime[nIONo][2] = CYLTime[nIONo][1] - CYLTime[nIONo][0];
		//if(CYLTime[nIONo][2] >= 20000 || CYLTime[nIONo][2] < 0) //wait 시간 limit 설정 
		if(CYLTime[nIONo][2] <= 0) //wait 시간 limit 설정 
		{
			CYLFlag[nIONo] = false;
			return RET_PROCEED;
		}
		if (CYLTime[nIONo][2] >= nLimitTime)
		{
			Ret = RET_ERROR;
			CYLFlag[nIONo] = false;
		}
	}
	return Ret;
}
//james 2016.0810 센서 없이 동작 시키는 실린더 등을 제어하기 위해 추가 
int CFastechPublic_IO::Wait_IO_OnOff(int nIONo, int OnOff, int nWaitTime)
{
	 
	int Ret = RET_PROCEED;  // 함수 리턴 플래그 설정 
	
	if (CYLFlag[nIONo] == false)
	{
		CYLTime[nIONo][0]	= GetCurrentTime();
		CYLFlag[nIONo] = true;
	}
	else if (CYLFlag[nIONo] == true)
	{
		CYLTime[nIONo][1] = GetCurrentTime();
		CYLTime[nIONo][2] = CYLTime[nIONo][1] - CYLTime[nIONo][0];

		if(  CYLTime[nIONo][2] <= 0) //wait 시간 limit 설정 
		{
			CYLFlag[nIONo] = false;
			return RET_PROCEED;
		}
		
		if (CYLTime[nIONo][2] >= nWaitTime)
		{
			Ret = RET_GOOD;
			CYLFlag[nIONo] = false;
		}
	} 
	return Ret;
}