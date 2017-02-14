// WorkTab.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Handler.h"
#include "WorkTab.h"
#include "WorkMaintenance.h"
#include "WorkMotorSpeed.h"
#include "WorkRecipe.h"
#include "WorkWaitTime.h"
#include "WorkTesterNetwork.h"
#include "WorkRfid.h"
#include "WorkBarcode.h"
#include "WorkFtp.h"
#include "WorkVisionTeach.h"
// CWorkTab

IMPLEMENT_DYNAMIC(CWorkTab, CTabCtrl)

CWorkTab::CWorkTab()
{
	m_nViewMode			= -1;

	m_pMaintenance		= NULL;
	m_pMotorSpeed		= NULL;
	m_pRecipe			= NULL;
	m_pWaitTime			= NULL;
	m_pTesterNetwork	= NULL;
	m_pRfid				= NULL;
	m_pFtp				= NULL;
	m_pCok				= NULL;
	m_pPartNo			= NULL;
	m_pManualFunc		= NULL;
	m_pTsiteOffset		= NULL;
	m_pBarcode          = NULL;
	m_pVisionTeach      = NULL; //kwlee 2016.0913
	
//	m_pCustomIoFunc		= NULL;
}

CWorkTab::~CWorkTab()
{
	if(m_pMaintenance != NULL)
	{
		m_pMaintenance->DestroyWindow();
		delete m_pMaintenance;
		m_pMaintenance = NULL;
	}
	
	if(m_pMotorSpeed != NULL)
	{
		m_pMotorSpeed->DestroyWindow();
		delete m_pMotorSpeed;
		m_pMotorSpeed = NULL;
	}
	
	if(m_pRecipe != NULL)
	{
		m_pRecipe->DestroyWindow();
		delete m_pRecipe;
		m_pRecipe = NULL;
	}
	
	if(m_pWaitTime != NULL)
	{
		m_pWaitTime->DestroyWindow();
		delete m_pWaitTime;
		m_pWaitTime = NULL;
	}
	if(m_pRfid != NULL)
	{
		m_pRfid->DestroyWindow();

		delete m_pRfid;
		m_pRfid = NULL;
	}

	if(m_pTesterNetwork != NULL)
	{
		m_pTesterNetwork->DestroyWindow();
		delete m_pTesterNetwork;
		m_pTesterNetwork = NULL;
	}

	if(m_pFtp != NULL)
	{
		m_pFtp->DestroyWindow();
		delete m_pFtp;
		m_pFtp = NULL;
	}

	if(m_pVisionTeach != NULL)
	{
		m_pVisionTeach->DestroyWindow();
		delete m_pVisionTeach;
		m_pVisionTeach = NULL;
	}
}


BEGIN_MESSAGE_MAP(CWorkTab, CTabCtrl)
END_MESSAGE_MAP()



// CWorkTab 메시지 처리기입니다.
void CWorkTab::OnInitTab(CWnd *pWnd, CRect rectArea)
{
	m_rectArea = rectArea;

	m_pMaintenance		= new CWorkMaintenance;
	m_pMaintenance->Create(IDD_WORK_MAINTENANCE, this);

	m_pMotorSpeed		= new CWorkMotorSpeed;
	m_pMotorSpeed->Create(IDD_WORK_SPEED, this);

	m_pRecipe			= new CWorkRecipe;
	m_pRecipe->Create(IDD_WORK_RECIPE, this);

	m_pWaitTime			= new CWorkWaitTime;
	m_pWaitTime->Create(IDD_WORK_WAIT, this);

	m_pTesterNetwork		= new CWorkTesterNetwork;
//	m_pInterface->Create(IDD_WORK_INTERFACE, this);
	m_pTesterNetwork->Create(IDD_WORK_TESTER_NETWORK, this);

	//kwlee 2016.0729
// 	m_pRfid				= new CWorkRfid;
// 	m_pRfid->Create(IDD_WORK_RFID, this);

 	m_pVisionTeach         = new CWorkVisionTeach;
 	m_pVisionTeach->Create(IDD_WORK_TEACH,this);

	m_pFtp          = new CWorkFtp;
	m_pFtp->Create(IDD_WORK_FTP,this);

	OnViewChange(0);
}

void CWorkTab::OnViewChange(int nView)
{
	if (nView == m_nViewMode) return;

	m_nViewMode = nView;

	m_pMaintenance->ShowWindow(SW_HIDE);
	m_pMotorSpeed->ShowWindow(SW_HIDE);
	m_pRecipe->ShowWindow(SW_HIDE);
	m_pWaitTime->ShowWindow(SW_HIDE);
	m_pTesterNetwork->ShowWindow(SW_HIDE);
	//m_pRfid->ShowWindow(SW_HIDE);
	//m_pBarcode->ShowWindow(SW_HIDE);
	m_pFtp->ShowWindow(SW_HIDE);
	m_pVisionTeach->ShowWindow(SW_HIDE);
	switch (nView)
	{
		case 0:
			m_pMaintenance->ShowWindow(SW_SHOW);
			break;

		case 1:
			m_pTesterNetwork->ShowWindow(SW_SHOW);
			break;

		case 2:
			m_pWaitTime->ShowWindow(SW_SHOW);
			break;

		case 3:
			m_pMotorSpeed->ShowWindow(SW_SHOW);
			break;

		case 4:
			m_pRecipe->ShowWindow(SW_SHOW);
			break;

// 		case 5:
// 			m_pRfid->ShowWindow(SW_SHOW);
// 			break;
// 
 		case 6:
 			m_pVisionTeach->ShowWindow(SW_SHOW);
 			break;

		case 7:
			m_pFtp->ShowWindow(SW_SHOW);
			break;

		case 8:
			
			break;

		case 9:
		
			break;

		case 10:
		
			break;

		case 11:

			break;
	}
}

