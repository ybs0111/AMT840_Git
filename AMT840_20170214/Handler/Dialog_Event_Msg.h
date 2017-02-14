#pragma once


#include "VLabel.h"
#include "GradientStatic.h"
// CDialog_Event_Msg ��ȭ �����Դϴ�.

class CDialog_Event_Msg : public CDialog
{
	DECLARE_DYNAMIC(CDialog_Event_Msg)

public:
	CDialog_Event_Msg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDialog_Event_Msg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EVENT_MSG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	CGradientStatic	 m_label_event_msg;

	void OnEventMsg_Button_Set();
	void OnEventMsg_Text_Set();
	void OnEventMsg_Label_Set();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL Create();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedOk();
};
