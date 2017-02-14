#pragma once

typedef struct _INTER_SV_QUEUE_3_
{
	int			m_nRear;
	int			m_nFront;
	int			m_nCount;

	char		m_chMsg[10][5000];
} INTER_SV_QUEUE_3, *P_INTER_SV_QUEUE_3;

class CServerInterface3
{
public:
	HANDLE				m_hMemMapInter3;
	P_INTER_SV_QUEUE_3	m_pQueueInter3;

	int					m_nHeader;
	int					m_nHeaderLength;

	CString				m_strHeader;
	CString				m_strData;

	void				OnCommunication();
	void				OnMessageWrite(CString strMsg);
	CString				OnMessageRead();
	CString				OnCharToString(char chData[], int nLength);
	void				OnStringToChar(CString strData, char chData[]);
	void				OnDataReceive(CString strMsg);
	void				OnDataAnalysis(CString strMsg);
	CString				OnDataItemAnalysis(int nOpt, int nPos, CString strMsg, CString strFind);

public:
	CServerInterface3(void);
	~CServerInterface3(void);
};

extern CServerInterface3 clsInterS3;

