#pragma once

// CRunRejectTrayInputConveyor ��� ����Դϴ�.

class CRunRejectTrayInputConveyor : public CObject
{
public:
	CRunRejectTrayInputConveyor();
	virtual ~CRunRejectTrayInputConveyor();

public:
	void			OnThreadRun(void);
	void			OnRunInit(void);
	void			OnRunMove(void);

};

extern CRunRejectTrayInputConveyor clsRunRejectTrayInputConveyor;
