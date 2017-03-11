// TheBirdDlg.h : header file
//
#define WM_MY_UPDATE (WM_USER + 1)

#include "NetTCPClient.h"
#include "Client.h"

#include <winsock2.h>
#pragma comment(lib, "WS2_32.lib")

#if !defined(AFX_THEBIRDDLG_H__969C9041_475B_4B96_A190_0FFEC4CE9535__INCLUDED_)
#define AFX_THEBIRDDLG_H__969C9041_475B_4B96_A190_0FFEC4CE9535__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define FILERECVPORT 8888
/////////////////////////////////////////////////////////////////////////////
// CTheBirdDlg dialog

class CTheBirdDlg : public CDialog
{
// Construction
public:
	CTheBirdDlg(CWnd* pParent = NULL);	// standard constructor

	NetTCPClient *netTCPClient;
	
	void CleanEdit();
	
	~CTheBirdDlg();
	
	CString getLocalNameAndIP();
	
	static UINT ListenService(LPVOID lParam);

	static UINT FreshUI(LPVOID lParam);

	CWinThread* pThread;
	/***********************************/
	static void StartService();
	string ServerAnalysisMessage(char *strMessage);
	
	/********************************/
	afx_msg LRESULT FrushUI(WPARAM wParam, LPARAM lParam);

	//********************************************************************
	Client thisClient;
	set<CString> items_selected;
	bool confirm_selects;

	CString filePath;
	long fileSize;
	bool file_selected;
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTheBirdDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTheBirdDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton1();
	afx_msg void OnClearScreen();
	afx_msg void OnSendMessage();
	afx_msg void OnConcelSend();
	afx_msg void OnSetfocusList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButton7();
	afx_msg void OnButton5();
	afx_msg void OnButton6();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_THEBIRDDLG_H__969C9041_475B_4B96_A190_0FFEC4CE9535__INCLUDED_)
