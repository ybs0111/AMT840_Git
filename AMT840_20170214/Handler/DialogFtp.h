#pragma once

#include "FtpClient.h"
#include "BtnST.h"
// CDialogFtp ��ȭ �����Դϴ�.

class CDialogFtp : public CDialog
{
	DECLARE_DYNAMIC(CDialogFtp)
public:
	CBitmap				m_bmGround;

	CString				m_strPathRemote;

	CFtpClient			m_pFtp;

	CListCtrl			m_listRemote;

	CButtonST			m_btnExit;

	void				OnInitListRemote();
	void				OnUpdateRemote(CString strPath);
	void				OnFtpInfo();
	void				OnInitButton();
public:
	CDialogFtp(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDialogFtp();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_FTP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL OnInitDialog();
	afx_msg void OnClickedBtnExit();
	afx_msg void OnDblclkListDirectory(NMHDR *pNMHDR, LRESULT *pResult);
};
