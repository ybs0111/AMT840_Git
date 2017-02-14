#pragma once


#include "VLabel.h"
#include "GradientStatic.h"
// CDialog_Event_Msg 대화 상자입니다.

class CDialog_Event_Msg : public CDialog
{
	DECLARE_DYNAMIC(CDialog_Event_Msg)

public:
	CDialog_Event_Msg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialog_Event_Msg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EVENT_MSG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
