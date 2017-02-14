#pragma once

// CRunDvcLdBuffer 명령 대상입니다.

class CRunDvcLdBuffer : public CObject
{
public:
	CRunDvcLdBuffer();
	virtual ~CRunDvcLdBuffer();

public:
	void			OnThreadRun(void);
	void			OnRunInit(void);
	void			OnRunMove(void);

	int				m_nLotProcessNum; //james 2016.0911 lot이 LOT_CURR, LOT_NEXT 둘중 하나이다 

	///////////////////////////////////////////////////////////////////////
	//2016.0806 저장할 목록
	///////////////////////////////////////////////////////////////////////
	int				m_nInitStep;
	int				m_nRunStep;
	int				m_nInsertStep;


	CString         m_strAlarmCode;	
	DWORD			m_dwWaitTime[3];
	
	int				m_nAxisNum_Y;        //현재의 IndexNum에서 사용하는 모터 실재 번호를 가진다 
	int				m_nAxisNum_R; 
	double			m_dpTargetPos_Y;
	double			m_dpTargetPos_R;

	int				m_nLinearMove_Index;   //현재 map index 
	long			m_lAxisCnt;            //현재의 IndexNum에서 사용 가능한 모터 수량 최대 4개  
	long			m_lpAxisNum[4];        //현재의 IndexNum에서 사용하는 모터 실재 번호를 가진다 
	double			m_dpTargetPosList[4];		   //최대 4 포인트를 한번에 이동가능하다 
 	double			m_dpSpdRatio[3];			//속도비율 [0]:Vel, [1]:Acc, [2]:Dec

	int				m_nWork_Site; // 0:Work robot, 1:Test Robot site 

	int				m_npFindWorkPosYXCPB[5]; //james 2015.0807 [0]first tray Y, [1]first tray X, [2]:작업가능 수량 또는 공간 정보, [3]first picker num, [4]first buff num

	int				m_nWork_Pos; //현재 작업할 위치

	bool            m_bPickerFlag;
	DWORD			m_dwPickerWaitTime[3];

	int				m_npBuff_Info[4];
	int				m_npBuff_Status[4];
	int				 m_npBuff_OutputStatus[4];//james 2016.0919 그리퍼의 output 상태를 체크 

	int				m_nErrorInfoStatus[10]; //clash 정보를 확인한후 저장되는 정보 

	void			Set_Gripper_OnOff(int nMode, int nOnOff, int *npPickerInfo);
	int				Chk_Gripper_OnOff(int nMode, int nOnOff, int *npPickerInfo);

 	//2016.1203
	CString			m_strLotNo;
	CString			m_strPartNo;	
	DWORD			m_dwCycleTime[3];
};
extern CRunDvcLdBuffer clsRunDvcLdBuffer;


