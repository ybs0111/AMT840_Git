// RunReadIO.cpp: implementation of the CRunReadIO class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RunReadIO.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRunReadIO	clsReadIO;

CRunReadIO::CRunReadIO()
{

}

CRunReadIO::~CRunReadIO()
{

}

//==================================================================//
// 쓰레드 동작
//==================================================================//
void CRunReadIO::OnThreadRun() 
{
	OnThreadExecute();  // 쓰레드 제어
}

//==================================================================//
// 쓰레드 관련 변수 초기화
//==================================================================//
void CRunReadIO::OnThreadInit() 
{
	if (m_bInitFlag == true)	
	{
		return;
	}
	m_bInitFlag = true;
}

//==================================================================//
// 쓰레드 제어d
//==================================================================//
void CRunReadIO::OnThreadExecute() 
{
	// I/O 보드가 열리지 않는 경우 강제 리턴
 	if (FAS_IO.m_nIoBoardInitialFlag == NO)	
	{
		return;
	}

	FAS_IO.IO_ReadWrite(0, 0, 0, 1);
	FAS_IO.IO_ReadWrite(0, 0, 1, 1);
	FAS_IO.IO_ReadWrite(0, 0, 2, 1);
	FAS_IO.IO_ReadWrite(0, 0, 3, 1);
	FAS_IO.IO_ReadWrite(0, 0, 4, 1);
	FAS_IO.IO_ReadWrite(0, 0, 5, 1);
	FAS_IO.IO_ReadWrite(0, 0, 6, 1);
	FAS_IO.IO_ReadWrite(0, 0, 7, 1);

	FAS_IO.IO_ReadWrite(0, 1, 0, 1);
	FAS_IO.IO_ReadWrite(0, 1, 1, 1);
	FAS_IO.IO_ReadWrite(0, 1, 2, 1);
	FAS_IO.IO_ReadWrite(0, 1, 3, 1);
	FAS_IO.IO_ReadWrite(0, 1, 4, 1);
	FAS_IO.IO_ReadWrite(0, 1, 5, 1);
	FAS_IO.IO_ReadWrite(0, 1, 6, 1);
	FAS_IO.IO_ReadWrite(0, 1, 7, 1);

	FAS_IO.IO_ReadWrite(0, 2, 0, 1);
	FAS_IO.IO_ReadWrite(0, 2, 1, 1);
	FAS_IO.IO_ReadWrite(0, 2, 2, 1);
	FAS_IO.IO_ReadWrite(0, 2, 3, 1);
	FAS_IO.IO_ReadWrite(0, 2, 4, 1);
	FAS_IO.IO_ReadWrite(0, 2, 5, 1);
	FAS_IO.IO_ReadWrite(0, 2, 6, 1);
	FAS_IO.IO_ReadWrite(0, 2, 7, 1);

	FAS_IO.IO_ReadWrite(0, 3, 0, 1);
	FAS_IO.IO_ReadWrite(0, 3, 1, 1);
	FAS_IO.IO_ReadWrite(0, 3, 2, 1);
	FAS_IO.IO_ReadWrite(0, 3, 3, 1);
	FAS_IO.IO_ReadWrite(0, 3, 4, 1);
	FAS_IO.IO_ReadWrite(0, 3, 5, 1);
	FAS_IO.IO_ReadWrite(0, 3, 6, 1);
	FAS_IO.IO_ReadWrite(0, 3, 7, 1);

	FAS_IO.IO_ReadWrite(1, 0, 0, 1);
	FAS_IO.IO_ReadWrite(1, 0, 1, 1);
	FAS_IO.IO_ReadWrite(1, 0, 2, 1);
	FAS_IO.IO_ReadWrite(1, 0, 3, 1);
/*	FAS_IO.IO_ReadWrite(1, 0, 4, 1);
	FAS_IO.IO_ReadWrite(1, 0, 5, 1);
	FAS_IO.IO_ReadWrite(1, 0, 6, 1);
	FAS_IO.IO_ReadWrite(1, 0, 7, 1);

	FAS_IO.IO_ReadWrite(1, 1, 0, 1);
	FAS_IO.IO_ReadWrite(1, 1, 1, 1);
	FAS_IO.IO_ReadWrite(1, 1, 2, 1);
	FAS_IO.IO_ReadWrite(1, 1, 3, 1);
	FAS_IO.IO_ReadWrite(1, 1, 4, 1);
	FAS_IO.IO_ReadWrite(1, 1, 5, 1);
	FAS_IO.IO_ReadWrite(1, 1, 6, 1);
	FAS_IO.IO_ReadWrite(1, 1, 7, 1);*/

	FAS_IO.IO_ReadWrite(2, 0, 0, 1);
	FAS_IO.IO_ReadWrite(2, 0, 1, 1);
	FAS_IO.IO_ReadWrite(2, 0, 2, 1);
	FAS_IO.IO_ReadWrite(2, 0, 3, 1);
	FAS_IO.IO_ReadWrite(2, 0, 4, 1);
	FAS_IO.IO_ReadWrite(2, 0, 5, 1);
	FAS_IO.IO_ReadWrite(2, 0, 6, 1);
	FAS_IO.IO_ReadWrite(2, 0, 7, 1);
/*
	FAS_IO.IO_ReadWrite(2, 1, 0, 1);
	FAS_IO.IO_ReadWrite(2, 1, 1, 1);
	FAS_IO.IO_ReadWrite(2, 1, 2, 1);
	FAS_IO.IO_ReadWrite(2, 1, 3, 1);
	FAS_IO.IO_ReadWrite(2, 1, 4, 1);
	FAS_IO.IO_ReadWrite(2, 1, 5, 1);
	FAS_IO.IO_ReadWrite(2, 1, 6, 1);
	FAS_IO.IO_ReadWrite(2, 1, 7, 1);*/
}
