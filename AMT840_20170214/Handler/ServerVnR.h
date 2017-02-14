#pragma once
class CServerVnR
{
public:
	int		m_nDataCount;
	int     m_Cnt;
	double	m_dVolt[10][60];
	double  m_dResistance[10][60];
	CString m_strMsg[10];

	void	OnMakeTime();
	void	OnMakeAsd();
	void	OnMakePngok();
	//void    OnMakeData(int nMcID,int nPort, double dVolt, double dResistance,CString strMsg);
	void    OnMakeData(CString strMsg);
	void OnDataReceive(CString strRev);

public:
	CServerVnR(void);
	~CServerVnR(void);
};
extern CServerVnR clsVnR;

