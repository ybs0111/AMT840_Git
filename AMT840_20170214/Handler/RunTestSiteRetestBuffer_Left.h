#pragma once

// CrunTestSiteRetestBuffer_Left ��� ����Դϴ�.

class CRunTestSiteRetestBuffer_Left : public CObject
{
public:
	CRunTestSiteRetestBuffer_Left();
	virtual ~CRunTestSiteRetestBuffer_Left();

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
extern CRunTestSiteRetestBuffer_Left clsRunTestSiteRetestBuffer_Left;



