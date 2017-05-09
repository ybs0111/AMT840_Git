#pragma once

// CRunRejectTrayBuffer 명령 대상입니다.

class CRunRejectTrayBuffer : public CObject
{
public:
	CRunRejectTrayBuffer();
	virtual ~CRunRejectTrayBuffer();


public:
	void			OnThreadRun();
	void			OnRunInit();
	void			OnRunMove();

public:
	int				m_nInitStep;
	int				m_nRunStep; 
	int				m_nEmptyToReject;
	int				m_nAxisNum;

	DWORD		m_dwWaitTime[3];
};

extern CRunRejectTrayBuffer clsRunRejectTrayBuffer;



