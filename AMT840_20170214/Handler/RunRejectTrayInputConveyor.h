#pragma once

// CRunRejectTrayInputConveyor 명령 대상입니다.

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
