#pragma once

#include "BtnST.h"
#include "VLabel.h"
#include "xpgroupbox.h"
// CDialog_Door_Open ��ȭ �����Դϴ�.
#define TIM_DOOR_OPEN	1000
class CDialog_Door_Open : public CDialog
{
	DECLARE_DYNAMIC(CDialog_Door_Open)
public:
	CVLabel				m_msgTopDoor[6];
	CVLabel				m_msgBottomDoor[5];

	CBitmap				m_bmGround;
public:
	CDialog_Door_Open(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDialog_Door_Open();


	int	nDoorOpenBack[12];

	CXPGroupBox m_groupLdUldDoor;
	CXPGroupBox m_groupTestDoor;
	CButtonST m_btnLdUldDoorOpen;
	CButtonST m_btnLdUldDoorClose;
	CButtonST m_btnTestDoorOpen;
	CButtonST m_btnTestDoorClose;
	CButtonST m_btnDoorOpen;
	CButtonST m_btnDoorClose;
	CButtonST m_btnDoorExit;

	void OnInitButton();
	void OnInitLabel();
	void OnInitGroup();

	void OnDoorOpenCheck();
// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_DOOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedDialogDoorOpen();
	afx_msg void OnBnClickedDialogDoorClose();
	afx_msg void OnBnClickedDialogDoorExit();
	virtual BOOL OnInitDialog();

	
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedDialogLdUldDoorOpen();
	afx_msg void OnBnClickedDialogLdUldDoorClose();
	afx_msg void OnBnClickedDialogTestDoorOpen();
	afx_msg void OnBnClickedDialogTestDoorClose();
	afx_msg void OnBnClickedDialogDoorExit2();
	
};
