#pragma once
class CRunHandlerCheck
{
public:
	// jtkim 20160929
	BOOL				m_bFrontMuteFlag;
	BOOL				m_bRightMuteFlag;

	DWORD				m_dwFrontWaitTime[3];
	DWORD				m_dwRightWaitTime[3];
	DWORD				m_dwFrontMuteWaitTime[2][3];
	DWORD				m_dwRightMuteWaitTime[2][3];

	int					m_nFrontMuteStep[2];
	int					m_nRightMuteStep[2];

	void				OnFrontMuteOn();
	void				OnFrontMuteOff();
	void				OnRightMuteOn();
	void				OnRightMuteOff();

	int					m_nLeftMpStep;
	int					m_nRightMpStep;

	int					m_nOldHifixMode;
	int					m_nHifixYesNo;
	int					m_nStartStep;
	int					m_nStopStep;
	int					m_nResetStep;
	int					m_nBuzzerStep;
	int					m_nAutoStep;
	int					m_nManualStep;
	int					m_nLdUldMuteStep;
	int					m_nRejMuteStep;

	DWORD				m_dwWaitTime[2][3];
	DWORD				m_dwStartWaitTime[3];
	DWORD				m_dwStopWaitTime[3];
	DWORD				m_dwResetWaitTime[3];
	DWORD				m_dwBuzzerWaitTime[3];
	DWORD				m_dwAutoWaitTime[3];
	DWORD				m_dwManualWaitTime[3];
	DWORD				m_dwOhtLdWaitTime[3];
	DWORD				m_dwOhtUnLdWaitTime[3];
	DWORD				m_dwJigStkWaitTime[3];
	DWORD				m_dwLdUldMuteWaitTime[3];
	DWORD				m_dwRejMuteWaitTime[3];

	void				OnStartCheck();
	void				OnStopCheck();
	void				OnResetCheck();
	void				OnBuzzerCheck();
	void				OnLdUldMuteOnOff();
	void				OnRejectMuteOnOff();
	void				OnAutoCheck();
	void				OnManualCheck();
	void				OnLightCurtainCheck();
	int					OnRunPossibleCheck();

public:
	CRunHandlerCheck(void);
	~CRunHandlerCheck(void);
};
extern CRunHandlerCheck clsHdChk;