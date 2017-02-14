#pragma once


// CWorkTab
class CWorkMaintenance;
class CWorkMotorSpeed;
class CWorkRecipe;
class CWorkWaitTime;
class CWorkTesterNetwork;
class CWorkRfid;
class CWorkFtp;
class CWorkCok;
class CWorkPartNo;
class CWorkManualFunction;
class CWorkTestSiteOffset;
class CWorkCustomIoFunction;
class CWorkBarcode;
class CWorkVisionTeach; //kwlee 2016.0913
class CWorkTab : public CTabCtrl
{
	DECLARE_DYNAMIC(CWorkTab)
public:
	int						m_nViewMode;
	CWorkMaintenance		*m_pMaintenance;
	CWorkMotorSpeed			*m_pMotorSpeed;
	CWorkRecipe				*m_pRecipe;
	CWorkWaitTime			*m_pWaitTime;
	CWorkTesterNetwork		*m_pTesterNetwork;
	CWorkRfid				*m_pRfid;
	CWorkFtp				*m_pFtp;
	CWorkCok				*m_pCok;
	CWorkPartNo				*m_pPartNo;
	CWorkManualFunction		*m_pManualFunc;
	CWorkTestSiteOffset		*m_pTsiteOffset;
	CWorkCustomIoFunction	*m_pCustomIoFunc;
	CWorkBarcode            *m_pBarcode;
	//kwlee 2016.0913
	CWorkVisionTeach		*m_pVisionTeach;

	CRect					m_rectArea;
	void					OnInitTab(CWnd *pWnd, CRect rectArea);
	void					OnViewChange(int nView);
public:
	CWorkTab();
	virtual ~CWorkTab();

protected:
	DECLARE_MESSAGE_MAP()
};


