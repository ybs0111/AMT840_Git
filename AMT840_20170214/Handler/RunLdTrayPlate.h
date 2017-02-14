#pragma once

// CRunLdTrayPlate 명령 대상입니다.

class CRunLdTrayPlate : public CObject
{
public:
	CRunLdTrayPlate();
	virtual ~CRunLdTrayPlate();

public:
	int				m_nLotProcessNum; //james 2016.0911 lot이 LOT_CURR, LOT_NEXT 둘중 하나이다 

	///////////////////////////////////////////////////////////////////////
	//james 2016.0803 저장할 목록
	///////////////////////////////////////////////////////////////////////
	int				m_nInitStep;
	int				m_nRunStep; 
	DWORD			m_dwWaitTime[3];

	int				m_npFindWorkPosYXCPB[5]; //james 2015.0807 처음 찾을때의 정보,  [0]first tray Y, [1]first tray X, [2]:작업가능 수량 또는 공간 정보, [3]first picker num, [4]first buff num
	int				m_npSet_WorkPosYXCPB[5]; //james 2015.0807 작업 완료후 작업 셋팅 위치 정보,  [0]first tray Y, [1]first tray X, [2]:작업가능 수량 또는 공간 정보, [3]first picker num, [4]first buff num

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


