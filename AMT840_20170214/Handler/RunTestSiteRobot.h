#pragma once


// CRunTestSiteRobot

class CRunTestSiteRobot : public CObject
{
public:
	CRunTestSiteRobot();
	virtual ~CRunTestSiteRobot();

public:
	void			OnThreadRun(void);
	void			OnRunInit(void);
	void			OnRunMove(void);

	int Chk_TestSiteRobotAllGlipper( int nOnOff ); //��Ŀ Glipper�� ���� ON�Ǿ� �ִ��� ���� OFF�Ǿ� �ִ��� üũ�Ѵ�
	void Set_TestSiteRobotGlipper_OnOff( int *npPickerInfo, int nOnOff ); //james 2016.0807 void Set_TestSiteRobotGlipper_OnOff( int nPick, int nOnOff );//nPick : ��Ŀ �ѹ�(���° ��Ŀ����)
	int Chk_TestSiteRobotGlipper_OnOff( int *npPickerInfo, int nOnOff ); //james 2016.0807 int Chk_TestSiteRobotGlipper_OnOff( int nPick, int nOnOff );//nPick : ��Ŀ �ѹ�

	int TestSite_PickPlace_SocketData_LoadBuff_Map_Combine(int nMode, int nTestSite_THD_Num, int nTestSocket_0_4_Start_Pos); //���ɿ��� ������ Ȯ���� �ε� ���ۿ� �ش� ������ ���� �ε� ���� ������ ��û �Ѵ� 

	int Process_DVC_Pick(int nMode, int nWork_Site, int nTest_Site, int nStart_FixPos); //int nStart_FixPos = Left(0), Right(4) �ΰ��� ��ġ�� �ϳ��̴� , ������ ���۱��� �������� �۾� 
	int Process_DVC_Place(int nMode, int nWork_Site, int nTest_Site, int nStart_FixPos, int Opt=0);//2016.1128 Process_DVC_Place(int nMode, int nWork_Site, int nTest_Site, int nStart_FixPos);

	int Process_BinSort_Place(int nMode, int nWork_Site, int nSortBin); //james 2016.0814 retest bin ���� 

public:	
	int				m_nRobot_Z_Stop_Flag;
	int				m_nRetestBuffer_PickPlace_Mode; //0:�ѹ��� ���´�, 1:�ļ��϶� �� ã�� ���� �Ǵ� ���� �� �ִ� 
	int				 m_npPicker_OutputStatus[4];//james 2016.0919 �׸����� output ���¸� üũ 

	DWORD			m_dwWaitTime[3];

	int				m_nInitStep;
	int				m_nRunStep;  

	int				m_nPick_Step;
	int				m_nPlace_Step;
	int				m_nSortPick_Step; //james 2016.0814 ��ε� ���׽�Ʈ �� ó��
	int				m_nSortPlace_Step;

	int				m_nLinearMove_Index;   //���� map index 
	long			m_lAxisCnt;            //������ IndexNum���� ��� ������ ���� ���� �ִ� 4��  
	long			m_lpAxisNum[4];        //������ IndexNum���� ����ϴ� ���� ���� ��ȣ�� ������ 
	double			m_dpTargetPosList[4];		   //�ִ� 4 ����Ʈ�� �ѹ��� �̵������ϴ� 
 	double			m_dpSpdRatio[4];			//�ӵ����� [0]:Vel, [1]:Acc, [2]:Dec
	int				m_nRobot_X;
	int				m_nRobot_Y;
	int				m_nRobot_Z; 
	int				m_nRobot_Ejector; 

	int				m_nSyncTestCount;//�ѹ��� �ε� ��ε��� �۾� ���� 
	int				m_npGetSiteInfo[10]; 

	int				m_nDvcRepickCount;
	int				m_nPartialPickCount;
	bool			m_bDvcWaitChk_Falg;
	int				m_nPickRetry_Cnt;
	int				m_nDirRetry_Cnt;//2016.1201
	double			m_dwDirOffset;//2016.1201

	int				m_nBuffer_Work_Cnt;//james 2016.0929
	int				m_nFirstPicker_Num;
	int				m_nFirstTray_X_Num;
	int				m_nFirstTray_Y_Num;

	int				m_nFirstBuff_X_Num;
	int				m_nFirstBuff_Y_Num;
	 
//	int				m_nSite_WorkSize_Num; //�������λ���ϱ� ���� �߰�  //����,�׽�Ʈ����Ʈ�� �۾������� �۾� ����Ʈ ���������� ���Ƿ� �����Ͽ� ���
	int				m_nFirst_Buff_Num; //�������λ���ϱ� ���� �߰� , //����, ���ϵ� ���� Ȯ�ο� ��� 

	int				m_nPartial_FirstRetestBuffPos_Select_No; //���׽�Ʈ ���� ��ġ�� �׽�Ʈ ����Ʈ�� ������ �´� ���簡 ������ ������ �۾��� ���� ��ġ�� �����ϰ� �̰��� ������ �κ��� ���縦 ä���� �� �׽�Ʈ ����Ʈ�� ���׽�Ʈ ���� �����Ѵ�  

	//2016.0814 
	int				m_npBuffer_YesNo[4];		//�۾� ���� ��Ŀ ���� [4];		//�۾� ���� ��Ŀ ���� 
	int				m_npBuff_Status[4];
	int				m_npBuff_OutputStatus[4];//james 2016.0919 �׸����� output ���¸� üũ 

	int				m_npTemp_Picker_YesNo[4];  //�ӽ� �۾�������Ŀ ���� 
	int				m_npPicker_YesNo[4];		//�۾� ���� ��Ŀ ���� 
	int				m_npPicker_Vacuum_Status[4];//���� ��Ũ ���� ���� 
	int				m_npPicker_Data_Status[4]; //����̽� ������ ���� 
	int				m_npPicker_Error_Status[4];//��Ŀ down�� Ʈ���̿��� ������ ��Ũ ���� ���� 

	double			m_dCurrentPos[100];//x,y,z, eject
	
	int				m_np_BuffInfo_Count[20]; //james 2016.0929 [16]:��Ŀ �۾� ���� ����, [17]: ���� �� ����  [15]; //
	int				m_nMove_Flag[5]; //���� �̵� �÷��� 
	int				m_npFindWorkPosYXCPB[5]; //james 2015.0807 ó�� ã������ ����,  [0]first tray Y, [1]first tray X, [2]:�۾����� ���� �Ǵ� ���� ����, [3]first picker num, [4]first buff num
	int				m_npSet_WorkPosYXCPB[5]; //james 2015.0807 �۾� �Ϸ��� �۾� ���� ��ġ ����,  [0]first tray Y, [1]first tray X, [2]:�۾����� ���� �Ǵ� ���� ����, [3]first picker num, [4]first buff num

	int				m_nTestSite_THD_Work_Site; //THD NUM, ��� �׽�Ʈ ����Ʈ�� �۾��ϱ� ���� �����̴��� Ȯ��
	int				m_nBinSort_THD_Work_Site; //�ך��� ���� �����ִ� ���縦 �����ϴ� ����

	double          m_dTemp_CalTargetPos_Y;
	double          m_dTemp_CalTargetPos_X;

	DWORD			m_dwWaitUntil[3];
	DWORD			m_dwVacuumWaitTime[3];
	int				m_nPickerPara;
	int				m_nRobotSite;


	int				m_nSortPickerInfo[4];
	//int				m_nSortPicker_First;
//	int				m_nSortBuffer_First;
	int				m_nSort_Count;

	int				m_nLotProcessNum; //james 2016.0911 lot�� LOT_CURR, LOT_NEXT ���� �̳��̴� 
	CString			m_strFindLotNo;
	int				m_nFindLotNo_Flag;

	CString			m_strSortDvc_LotNo; //���� �����Ϸ��� ������ lot ���� 

	int				m_nClash_ErrorInfoStatus[4];



	CString m_strAlarmCode;
	bool m_bGlipperOnOffFlag;
	DWORD m_dwGlipperOnOff[3];

	//2016.1203
	DWORD			m_dwPlaceTime[3];
	DWORD			m_dwCycleTime[2][3];
	CString			m_strLotNo[2];
	CString			m_strPartNo[2];
};
extern CRunTestSiteRobot clsRunTestSiteRobot;


