#pragma once

// CRunTestSiteHead_8 ��� ����Դϴ�.

class CRunTestSiteHead_8 : public CObject
{
public:
	CRunTestSiteHead_8();
	virtual ~CRunTestSiteHead_8();

public:
	void			OnThreadRun(void);
	void			OnRunInit(void);
	void			OnRunMove(void);
};
extern CRunTestSiteHead_8 clsRunTestSiteHead_8;
