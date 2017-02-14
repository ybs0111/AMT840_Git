#pragma once

// CRunDvcLdBuffer ��� ����Դϴ�.

class CRunDvcLdBuffer : public CObject
{
public:
	CRunDvcLdBuffer();
	virtual ~CRunDvcLdBuffer();

public:
	void			OnThreadRun(void);
	void			OnRunInit(void);
	void			OnRunMove(void);

	int				m_nLotProcessNum; //james 2016.0911 lot�� LOT_CURR, LOT_NEXT ���� �ϳ��̴� 

	///////////////////////////////////////////////////////////////////////
	//2016.0806 ������ ���
	///////////////////////////////////////////////////////////////////////
	int				m_nInitStep;
	int				m_nRunStep;
	int				m_nInsertStep;


	CString         m_strAlarmCode;	
	DWORD			m_dwWaitTime[3];
	
	int				m_nAxisNum_Y;        //������ IndexNum���� ����ϴ� ���� ���� ��ȣ�� ������ 
	int				m_nAxisNum_R; 
	double			m_dpTargetPos_Y;
	double			m_dpTargetPos_R;

	int				m_nLinearMove_Index;   //���� map index 
	long			m_lAxisCnt;            //������ IndexNum���� ��� ������ ���� ���� �ִ� 4��  
	long			m_lpAxisNum[4];        //������ IndexNum���� ����ϴ� ���� ���� ��ȣ�� ������ 
	double			m_dpTargetPosList[4];		   //�ִ� 4 ����Ʈ�� �ѹ��� �̵������ϴ� 
 	double			m_dpSpdRatio[3];			//�ӵ����� [0]:Vel, [1]:Acc, [2]:Dec

	int				m_nWork_Site; // 0:Work robot, 1:Test Robot site 

	int				m_npFindWorkPosYXCPB[5]; //james 2015.0807 [0]first tray Y, [1]first tray X, [2]:�۾����� ���� �Ǵ� ���� ����, [3]first picker num, [4]first buff num

	int				m_nWork_Pos; //���� �۾��� ��ġ

	bool            m_bPickerFlag;
	DWORD			m_dwPickerWaitTime[3];

	int				m_npBuff_Info[4];
	int				m_npBuff_Status[4];
	int				 m_npBuff_OutputStatus[4];//james 2016.0919 �׸����� output ���¸� üũ 

	int				m_nErrorInfoStatus[10]; //clash ������ Ȯ������ ����Ǵ� ���� 

	void			Set_Gripper_OnOff(int nMode, int nOnOff, int *npPickerInfo);
	int				Chk_Gripper_OnOff(int nMode, int nOnOff, int *npPickerInfo);

 	//2016.1203
	CString			m_strLotNo;
	CString			m_strPartNo;	
	DWORD			m_dwCycleTime[3];
};
extern CRunDvcLdBuffer clsRunDvcLdBuffer;


