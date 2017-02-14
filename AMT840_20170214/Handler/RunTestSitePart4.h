#pragma once
class CRunTestSitePart4
{
public:
	int				m_nOpenDoor;
	int				m_nCloseDoor;

	int				m_nInitStep;
	int				m_nRunStep;  
	int				m_nTHD_ID;
	int				m_nIO_No;
	CString			m_strAlarmCode;

	int				m_nDoorStep;
	DWORD			m_dwDoorWaitTime[3];

	DWORD			m_dwWaitTime[3];
	bool			m_bWaitChk_Falg;

	int				m_nBdNum;

	void			OnRunDoor();
	void			OnRunInit(void);
	void			OnRunMove(void);
	void			OnThreadRun();
	void			OnCommunication();

	// jtkim 20160904
	DWORD			m_dwWaitTestSiteUpDown[3];
	bool			m_bTestSiteUpDownFlag;

	void			Set_TestSite_UpDown( int nOnOff );
	int				Chk_TestSite_UpDown( int nOnOff );

public:
	CRunTestSitePart4(void);
	~CRunTestSitePart4(void);
};

extern CRunTestSitePart4 clsTestSite4;
