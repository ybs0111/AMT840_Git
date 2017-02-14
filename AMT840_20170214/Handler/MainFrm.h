
// MainFrm.h : CMainFrame Ŭ������ �������̽�
//

#pragma once

#define TOOLBAR_DRAW_BUTTON_WIDTH 32

/* ****************************************************************************** */
/* Initial Dialog Bar ���� Ŭ���� �߰� �κ�                                       */
/* ****************************************************************************** */
#include "Dialog_Title.h"	// Ÿ��Ʋ ���� ��� ��ȭ ���� Ŭ���� �߰� 
#include "Dialog_List.h"
#include "TrueColorToolBar.h"
#include "Variable.h"
#include "Thread.h"			// ������ Ŭ����
#include "FtpComm.h"
#include "Dialog_Event_Msg.h"

// ******************************************************************************
// Initial Dialog Bar ���� Ŭ���� �߰� �κ�
// ******************************************************************************
#include "Alg_Excel.h"
//  ******************************************************************************

class CClientSocket;
class CServerSocket;
class AMTVClassWrapper;
class CMainFrame : public CFrameWnd
{
public:
	// ���귮 ���� DB
	void	OnProductData();

	void	OpenDll();
	void	CloseDll();
	void    OnMcInitial();
	int		OnXgemInterface();
	void	OnExcelRead();
	void	OnFilePath();
	void	OnSwitchToForm(int nForm);
	int		OnMenuChangeChecking();
	void	OnConfigSave();
	void	OnConfigLoad();
	void    OnWriteGmsLog();

	void	OnMainVarDefaultSet();
	void    OnTestSiteDn(); //kwlee 2016.0905 TestSite �ٿ�.
	void    OnThreadCreate();
	void    OnThreadDelete();
	void    OnMainFrame_TeachVision(int iCommand );
protected: // serialization������ ��������ϴ�.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Ư���Դϴ�.
public:
	CFtpComm		m_pFtp;			// scrap code���� 

	int				m_nXgemStep;
	int             m_nCount;
	DWORD			m_dwXgemTime[3];

	CClientSocket	*m_pClient[20];
	CServerSocket	*m_pServer[20];

	CView			*m_pNewActiveView;
	CView			*m_pOldActiveView;
	CDialog_Event_Msg*  mp_msg_dlg;

	COleDateTime	m_tCreate;
	COleDateTime	m_tGms;

	//2015.1005
	CString mstr_Data[49][49]; 
	int mn_Cnt;
	CString mstrHeaterWCommand;
	long lTempSerialTime[3];
	int mn_new_lot_in_step;

	int mn_Db_Cnt; 
	int mn_Count;

	bool mb_intervalCheck;
	bool mb_Ten;	
	///
private:
	AMTVClassWrapper *m_pAMTVClassWrapper;
// �۾��Դϴ�.
public:
	// **************************************************************************
	// Initial Dialog Bar ���� Ŭ���� ������ ���� ����
	// **************************************************************************
	CDialog_Title	m_wndTitleBar;  // Ÿ��Ʋ �� ��ȭ ���ڿ� ���� Ŭ���� ���� ���� 
	CDialog_List	m_wndListBar;   // ����Ʈ�� Ŭ����
	

// �������Դϴ�.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// �����Դϴ�.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // ��Ʈ�� ������ ���Ե� ����Դϴ�.
	CTrueColorToolBar    m_wndToolBar;
// ������ �޽��� �� �Լ�
protected:
	afx_msg int			OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT		OnViewChangeMode(WPARAM wParam,LPARAM lParam) ;		// Post Message�� ���� ȭ�� ��ȯ ����� ����� ���� �޽��� ���� 
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBasic();
	afx_msg void OnAlarm();
	afx_msg void OnDoor();
	afx_msg void OnExit();
	afx_msg void OnIo();
	afx_msg void OnLevel();
	afx_msg void OnList();
	afx_msg void OnLock();
	afx_msg void OnMain();
	afx_msg void OnMenu();
	afx_msg void OnMotor();
	afx_msg void OnReset();
	afx_msg void OnSave();
	afx_msg void OnSetting();
	afx_msg void OnVer();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPgminfo();
	afx_msg LRESULT OnClientEcFirst(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClientEcSecond(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClientXgem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClientInterface1(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClientInterface2(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClientInterface3(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClientInterface4(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClientInterface5(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClientInterface6(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClientInterface7(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClientInterface8(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClientFtp(WPARAM wParam, LPARAM lParam);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg LRESULT OnXgemCommand(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnRfidMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnBarcodeMessage1(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnBarcodeMessage2(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnBarcodeMessage3(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnBarcodeMessage4(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnBarcodeMessage5(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnServerInterface1(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnServerInterface2(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnServerInterface3(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnServerInterface4(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnServerInterface5(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnServerInterface6(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnServerInterface7(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnServerInterface8(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMainMessageEvent(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMainframe_Work(WPARAM wParam, LPARAM lParam);
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
};


