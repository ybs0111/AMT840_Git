#pragma once


// CRunLdUldRobot

class CRunLdUldRobot :  public CObject
{
public:
	CRunLdUldRobot();
	virtual ~CRunLdUldRobot();

public:
	void			OnThreadRun(void);
	void			OnRunInit(void);
	void			OnRunMove(void);

	void Set_WorkRbtPicker_UpDown( int nOnOff );
	int Chk_WorkRbtPicker_UpDown( int nOnOff );
	void Set_WorkRbtRotator_OnOff( int nOnOff );
	int Chk_WorkRbtRotator_OnOff( int nOnOff );// On : 90�� ȸ�� Off : 0��
	void Set_Glipper_OnOff( int nOnOff );
	int Chk_Glipper_OnOff( int nOnOff );

public:
	CString m_strAlarmCode;
	bool m_bPickerDownFlag;
	DWORD m_dwWaitPickerUpDown[3];
	bool m_bRotatorOnOffFlag;
	DWORD m_dwWaitRotatorOnOff[3];
	bool m_bGlipperOnOffFlag;
	DWORD m_dwGlipperOnOff[3];

	///////////////////////////////////////////////////////////////////////
	//james 2016.0804 ������ ���
	///////////////////////////////////////////////////////////////////////

	int Process_DVC_Pick(int nMode, int nWork_Site);
	int Process_DVC_Place(int nMode, int nWork_Site, int nTestSitePos);

	//james 2016.0919 �߰� 
	int				 m_npPicker_OutputStatus[4];//james 2016.0919 �׸����� output ���¸� üũ 
	//james 2016.0912 lot�� LOT_CURR, LOT_NEXT ���� �̳��̴� 
	int				m_nLotProcessNum; 
	CString			m_strFindLotNo;
	int				m_nFindLotNo_Flag;
	///////// 2016.0912 
	DWORD			m_dwBarcodeWaitTime[3];
	int				m_nRetry;
	int				m_nBarcode1dStep;
	int				m_nBarcode2dStep;
	int				Process_Barcode1D(int nWork_Site, int nX, int nY);
	int				Process_Barcode2D(int nWork_site, int nX, int nY);
	
	DWORD			m_dwWaitTime[3];

	int				m_nInitStep;
	int				m_nRunStep;  

	int				m_nPick_Step;
	int				m_nPlace_Step;

	double			m_dCurrentPos[100];//x,y,z

	int				m_nDvcRepickCount;
	int				m_nPartialPickCount;
	bool			m_bDvcWaitChk_Falg;
	int				m_nPickRetry_Cnt;

	int				m_npTemp_Picker_YesNo[4];  //�ӽ� �۾�������Ŀ ���� 
	int				m_npPicker_YesNo[4];		//�۾� ���� ��Ŀ ���� 
	int				m_npPicker_Vacuum_Status[4];//���� ��Ũ ���� ���� 
	int				m_npPicker_Data_Status[4]; //����̽� ������ ���� 
	int				m_npPicker_Error_Status[4];//��Ŀ down�� Ʈ���̿��� ������ ��Ũ ���� ���� 

	int				m_nClash_ErrorInfoStatus[4];

 

	int				m_nWorkRobot_uldSite_SortJob_THD_Pos;	 //THD_REJECT_OUT_1_STACKER �� THD ������ 
	int				m_nWorkRobot_Current_SortBin;    //���� �κ��� ������ �� ����


	DWORD			m_dwWaitUntil[3];
	DWORD			m_dwVacuumWaitTime[3];

	int				m_n_FirstPicker_Num;
	int				m_n_FirstTray_X_Num;
	int				m_n_FirstTray_Y_Num;

	//int				m_n_FirstBuff_X_Num;
	//int				m_n_FirstBuff_Y_Num;

	
	int				m_nPickerPara;
	int				m_nRobotSite;

	int				m_npFindWorkPosYXCPB[5]; //james 2015.0807 ó�� ã������ ����,  [0]first tray Y, [1]first tray X, [2]:�۾����� ���� �Ǵ� ���� ����, [3]first picker num, [4]first buff num
	int				m_npSet_WorkPosYXCPB[5]; //james 2015.0807 �۾� �Ϸ��� �۾� ���� ��ġ ����,  [0]first tray Y, [1]first tray X, [2]:�۾����� ���� �Ǵ� ���� ����, [3]first picker num, [4]first buff num

	int				m_nLdRobot_Work_Pos_Num; //load1 = 0, load2 tray = 1 , load buffer = 2

	int				m_nMove_Flag[5]; //���� �̵� �÷��� 
	int				m_nRotator_Flag;
	int				m_nXYRobot_PreMove_Flag;//�̸� �۾� ��ġ�� �̵��ϴ� �÷��� 

	int				m_nLoadBuff_Job_Continue_Flag; //�ε� �÷���Ʈ���� ���� ���ۿ� ���������� Ȯ���ϴ� �÷���, �ε� �۾��� �̾ ��� �����ϱ� ���� �߰�, ��ε� ���簡 �͵� �ε��� ������ ó���Ѵ� 

	int				m_nAxisNum;        //������ IndexNum���� ����ϴ� ���� ���� ��ȣ�� ������ 
	double			m_dpTargetPos;
	int				m_nWork_Site_Pos; //�۾� ���, pick or place 

	double          m_dTemp_CalTargetPos_Y;
	double          m_dTemp_CalTargetPos_X;

	int				m_nLinearMove_Index;   //���� map index 
	long			m_lAxisCnt;            //������ IndexNum���� ��� ������ ���� ���� �ִ� 4��  
	long			m_lpAxisNum[4];        //������ IndexNum���� ����ϴ� ���� ���� ��ȣ�� ������ 
	double			m_dpTargetPosList[4];		   //�ִ� 4 ����Ʈ�� �ѹ��� �̵������ϴ� 
 	double			m_dpSpdRatio[3];			//�ӵ����� [0]:Vel, [1]:Acc, [2]:Dec
	int				m_nRobot_X;
	int				m_nRobot_Y;
	int				m_nRobot_Z; 

	int				mn_BcrFirst;
	int				mn_BcrFirstChk;
	int				mn_BcrFirstFinal;

	//2016.1203
	CString			m_strLotNo[2];
	CString			m_strPartNo[2];	
	DWORD			m_dwCycleTime[2][3];

	//kwlee 2017.0905
	int            m_nProduct_Cnt;
	 
};
extern CRunLdUldRobot clsRunLdUldRobot;

