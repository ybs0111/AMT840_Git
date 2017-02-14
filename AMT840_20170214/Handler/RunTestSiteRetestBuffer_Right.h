#pragma once

// CRunTestSiteRetestBuffer_Right 명령 대상입니다.

class CRunTestSiteRetestBuffer_Right : public CObject
{
public:
	CRunTestSiteRetestBuffer_Right();
	virtual ~CRunTestSiteRetestBuffer_Right();

public:
	void			OnThreadRun(void);
	void			OnRunInit(void);
	void			OnRunMove(void);

	int				m_nInitStep;
	int				m_nRunStep;  
	int				m_nTHD_ID;
	int				m_nIO_No;
	CString			m_strAlarmCode;

	double			m_dTargetPos;


	DWORD			m_dwWaitTime[3];
	bool			m_bWaitChk_Falg;

	int				m_nRetestBuff_R_Motor; 
};
extern CRunTestSiteRetestBuffer_Right clsRunTestSiteRetestBuffer_Right;


