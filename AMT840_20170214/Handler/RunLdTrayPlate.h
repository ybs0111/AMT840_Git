#pragma once

// CRunLdTrayPlate ��� ����Դϴ�.

class CRunLdTrayPlate : public CObject
{
public:
	CRunLdTrayPlate();
	virtual ~CRunLdTrayPlate();

public:
	int				m_nLotProcessNum; //james 2016.0911 lot�� LOT_CURR, LOT_NEXT ���� �ϳ��̴� 

	///////////////////////////////////////////////////////////////////////
	//james 2016.0803 ������ ���
	///////////////////////////////////////////////////////////////////////
	int				m_nInitStep;
	int				m_nRunStep; 
	DWORD			m_dwWaitTime[3];

	int				m_npFindWorkPosYXCPB[5]; //james 2015.0807 ó�� ã������ ����,  [0]first tray Y, [1]first tray X, [2]:�۾����� ���� �Ǵ� ���� ����, [3]first picker num, [4]first buff num
	int				m_npSet_WorkPosYXCPB[5]; //james 2015.0807 �۾� �Ϸ��� �۾� ���� ��ġ ����,  [0]first tray Y, [1]first tray X, [2]:�۾����� ���� �Ǵ� ���� ����, [3]first picker num, [4]first buff num

	void			OnThreadRun(void);
	void			OnRunInit(void);
	void			OnRunMove(void);

	void Set_LdTrayPlater_FwdBwd( int nOnOff );
	int Chk_LdTrayPlater_FwdBwd( int nOnOff );

public:
	CString m_strAlarmCode;
	bool m_bTrayPlateFwdBwdFlag;
	DWORD m_dwWaitTrayPlateFwdBwd[3];
};
extern CRunLdTrayPlate clsRunLdTrayPlate;


