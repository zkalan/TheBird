// TheBirdDlg.cpp : implementation file
//

#include <string>
using std::string;

#include "stdafx.h"
#include "TheBird.h"
#include "TheBirdDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//线程函数：接收心跳包，刷新上线客户端详情界面
UINT updateThreadProc(LPVOID p);
//线程函数：发送心跳包
UINT sendHBeatThreadProc(LPVOID p);
//线程函数：刷新在线用户数
UINT updateUserNumProc(LPVOID p);
//线程函数：接收文件
UINT receiveFileProc(LPVOID p);
//解析心跳包
struct Peer retrieveMessage(CString& message, CString& hostname, CString& ip, CString& state);
//通过文件路径获取文件名
CString getFileName(CString filePath);


UINT SendMsgThread(LPVOID lpParam);

void analysis(char * strMessage);

CString analysis_result;

bool FrushFlag = false;

// Dialog Data
	//{{AFX_DATA(CTheBirdDlg)
	enum { IDD = IDD_THEBIRD_DIALOG };
	CListBox	m_listbox_ip;
	CListCtrl	m_list;
	CString	m_edit1;
	CString	m_edit2;
	CString	m_edit3;
	CString	m_edit4;
	//}}AFX_DATA



/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTheBirdDlg dialog

CTheBirdDlg::CTheBirdDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD, pParent)
{
	//{{AFX_DATA_INIT(CTheBirdDlg)
	m_edit1 = _T("");
	m_edit2 = _T("");
	m_edit3 = _T("");
	m_edit4 = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	confirm_selects = false;
	file_selected = false;
}

void CTheBirdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTheBirdDlg)
	DDX_Control(pDX, IDC_LIST2, m_listbox_ip);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Text(pDX, IDC_EDIT1, m_edit1);
	DDX_Text(pDX, IDC_EDIT2, m_edit2);
	DDX_Text(pDX, IDC_EDIT3, m_edit3);
	DDX_Text(pDX, IDC_EDIT4, m_edit4);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTheBirdDlg, CDialog)
	//{{AFX_MSG_MAP(CTheBirdDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnClearScreen)
	ON_BN_CLICKED(IDC_BUTTON3, OnSendMessage)
	ON_BN_CLICKED(IDC_BUTTON4, OnConcelSend)
	ON_NOTIFY(NM_SETFOCUS, IDC_LIST1, OnSetfocusList1)
	ON_NOTIFY(NM_KILLFOCUS, IDC_LIST1, OnKillfocusList1)
	ON_BN_CLICKED(IDC_BUTTON7, OnButton7)
	ON_BN_CLICKED(IDC_BUTTON5, OnButton5)
	ON_MESSAGE(WM_MY_UPDATE, FrushUI)
	ON_BN_CLICKED(IDC_BUTTON6, OnButton6)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTheBirdDlg message handlers

BOOL CTheBirdDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	DWORD dwStyle = m_list.GetExtendedStyle();
	//设置当鼠标点击某一行时，整行选中
	dwStyle |= LVS_EX_FULLROWSELECT;
	//添加网格线
	dwStyle |= LVS_EX_GRIDLINES;
	m_list.SetExtendedStyle(dwStyle);
	m_list.InsertColumn(0, _T("主机名"), LVCFMT_CENTER, 170);
	m_list.InsertColumn(1, _T("IP地址"), LVCFMT_CENTER, 200);

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	

	//******************************************************************************

	// ***TODO: Add extra initialization here
	thisClient.listCtrl = &m_list;
	thisClient.userNum = &m_edit1;
	thisClient.finished = false;
	//my client is online, two tasks need to be done
	//1)create a working thread to update m_List
	//2)send hearting-beating message to all the others every 5s
	
	//1)update thread
	AfxBeginThread(updateThreadProc, &thisClient, 0, 0, 0, NULL);

	//2)send heart-beating thread
	AfxBeginThread(sendHBeatThreadProc, &thisClient, 0, 0, 0, NULL);

	//3)update user number
	AfxBeginThread(updateUserNumProc, this, 0, 0, 0, NULL);

	//4)receive file thread
	AfxBeginThread(receiveFileProc, this, 0, 0, 0, NULL);

	//****************************************************************************

	// TODO: Add extra initialization here

	GetDlgItem(IDC_EDIT3)->SetFocus();
	
	//UpdateData(FALSE);
	
	AfxBeginThread(ListenService, this, 0, 0, 0, NULL);
	//创建刷新线程
	//AfxBeginThread(FreshUI, this, 0, 0, 0, NULL);
	//pThread = AfxBeginThread(SendMsgThread, this);
	AfxBeginThread(SendMsgThread, this, 0, 0, 0, NULL);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}
//创建窗口的监听服务
UINT CTheBirdDlg::ListenService(LPVOID lParam)
{
	StartService();
	return 0;
}
//刷新UI
UINT SendMsgThread(LPVOID lpParam)
{
	CTheBirdDlg *dlg = (CTheBirdDlg*)lpParam;
	while (TRUE)
	{
		//CString temp = m_edit2;
		Sleep(200);
		if (FrushFlag)
		{
			SendMessageTimeout(dlg->m_hWnd, WM_MY_UPDATE, FALSE,NULL, SMTO_BLOCK, 1000, NULL);
			FrushFlag = FALSE;
		}
	}
	return 0;
}
LRESULT CTheBirdDlg::FrushUI(WPARAM wParam, LPARAM lParam)
{
	//GetDlgItem(IDC_EDIT2)->SetWindowText(m_edit2);
	UpdateData(FALSE);
	//::PostMessage(m_hWnd, WM_VSCROLL, SB_BOTTOM, 0);
	//CEdit* pedit = (CEdit*)GetDlgItem(IDC_EDIT3);
	//pedit->LineScroll(pedit->GetLineCount());
	return 1;
}

void CTheBirdDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTheBirdDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTheBirdDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTheBirdDlg::OnClearScreen() 
{
	// TODO: Add your control notification handler code here
	m_edit2 = "";
	m_edit3 = "";
	UpdateData(FALSE);
	
}


CTheBirdDlg::~CTheBirdDlg()
{
	delete netTCPClient;
}

//单击发送按钮
//建立socket连接
//将所有内容发送完毕后，断开连接
void CTheBirdDlg::OnSendMessage() 
{
	set<CString>::iterator it;
	for(it = items_selected.begin(); it != items_selected.end(); it++)
	{
		CString hostname;
		
		//find the hostname of this ip
		set<struct Peer> peerlist = thisClient.getPeerList();
		set<struct Peer>::iterator iterate;
		for(iterate = peerlist.begin(); iterate != peerlist.end(); iterate++)
		{
			if(strcmp(iterate->ip, *it) == 0)
			{
				hostname = iterate->hostname;
				break;
			}
		}

		netTCPClient = new NetTCPClient(*it, hostname);
		//将窗口的值更新到变量
		UpdateData(TRUE);

		string m = m_edit3.GetBuffer(0);
		int flag = 0;

		for (int i = 0;i < m.length(); i ++)
		{
			if (m[i] == ' ')
			{
				flag+=1;
			}
		}
		if (m_edit3 == "" || flag == m.length())
		{
			::MessageBox(m_hWnd, "请检查发送内容", "提示", MB_OK);
		}
		else
		{
			//根据地址栏中的地址，建立socket连接
			if (!netTCPClient->Connect(*it))
			{
				MessageBox("服务连接失败", "错误", MB_OK);
				return;
			}
			//将制定内容发送到对方
			netTCPClient->sendContent(m_edit3);
			//释放资源
			netTCPClient->ReleaseResource();
			//更新edit窗口的内容
			CleanEdit();
		}

	}
}

void CTheBirdDlg::CleanEdit()
{
	//将发送的内容添加到消息窗口
	m_edit2 = m_edit2 + "\r\n\r\n" + getLocalNameAndIP() + m_edit3;
	//将编辑栏内容清空
	m_edit3 = "";
	//由变量到窗口更新
	UpdateData(FALSE);
}

//获得格式化的本机名和ip
CString CTheBirdDlg::getLocalNameAndIP()
{
	//自己计算机名\r\n
	char hostname[50];
	int ri = gethostname(hostname, sizeof(hostname));
	if (ri == SOCKET_ERROR)
	{
		return "";
	}
	string self_name;
	self_name = hostname;
	//自己ip地址\r\n
	hostent *host = gethostbyname(hostname);
	if (host == NULL)
	{
		return "";
	}
	ULONG uAddr = ((struct in_addr*)*(host->h_addr_list))->s_addr;
	in_addr *addr = (in_addr*)*(host->h_addr_list);
	string self_ip = inet_ntoa(addr[0]);
	
	string result = "[" + self_name +  "@" + self_ip + "] ";
	
	return result.c_str();
}

//创建服务线程
void CTheBirdDlg::StartService()
{
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 0);
	::WSAStartup(sockVersion, &wsaData);

	SOCKET soc;
	SOCKET clientsocket;
	
	sockaddr_in sa_in;

	soc = socket(AF_INET, SOCK_STREAM, 0);
	//如果套接字创建失败
	if (soc == INVALID_SOCKET)
	{
		::MessageBox(NULL, "监听服务创建失败", "错误", MB_OK);
		WSACleanup();
		return;
	}
	//填充sockadd_in结构
	sa_in.sin_family = AF_INET;
	sa_in.sin_port = htons(4399);
	sa_in.sin_addr.S_un.S_addr = INADDR_ANY;
	//绑定套接字到本地地址
	if (bind(soc, (LPSOCKADDR)&sa_in, sizeof(sa_in)) == SOCKET_ERROR)
	{
		WSACleanup();
		::MessageBox(NULL, "绑定套接字到地址失败", "错误", MB_OK);
		return;
	}
	//套接字进入监听状态
	if(listen(soc, 10) == SOCKET_ERROR)
	{
		::MessageBox(NULL, "套接字监听状态设置失败", "错误", MB_OK);
		WSACleanup();
		return ;
	}
	
	//循环接受对方的连接请求
	while (TRUE)
	{
		sockaddr_in remoteAddr;
		int nAddrLen = sizeof(remoteAddr);

		clientsocket = accept(soc, (SOCKADDR*)&remoteAddr, &nAddrLen);

		//AfxBeginThread(ChatThread, this, 0, 0, 0, NULL);

		char message[2048] = {0};
		//接收消息
		recv(clientsocket, message, 2048, 0);

		analysis(message);
		
		//::MessageBox(NULL, "  收到消息", "消息", MB_OK);

		shutdown(clientsocket, SD_RECEIVE);
		closesocket(clientsocket);
	}

	//关闭监听套接字
	closesocket(soc);
	//释放WS2_32库
	WSACleanup();
	return;
}

//解析收到的联络报文
void analysis(char * strMessage)
{
	int contentLength = strlen(strMessage);
	int i = 0;
	int rnNum = 0;
	string _remote_name = "";
	string _remote_ip = "";
	string _self_name = "";
	string _self_ip = "";
	string _content = "";

	for (i = 0; i < contentLength && rnNum < 5; )
	{
		if (rnNum != 4)
		{
			while (strMessage[i] != '\r')
			{
				if (rnNum == 0)
				{
					_remote_name += strMessage[i]; i++;
				}
				else if (rnNum == 1)
				{
					_remote_ip += strMessage[i]; i++;
				}
				else if (rnNum == 2)
				{
					_self_name += strMessage[i]; i++;
				}
				else if (rnNum == 3)
				{
					_self_ip += strMessage[i]; i++;
				}
			}
			rnNum++;
			i += 2;
		}
		else
		{
			_content += strMessage[i];
			i++;
		}
	}
	string editContent = "[" + _remote_name + "@" + _remote_ip + "]" + _content;

	m_edit2 = m_edit2 + "\r\n\r\n" + editContent.c_str();

	FrushFlag = TRUE;
}

void CTheBirdDlg::OnConcelSend() 
{
	// TODO: Add your control notification handler code here
	m_edit3 = "";
	UpdateData(FALSE);
	
}

BOOL CTheBirdDlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	thisClient.finished = true;
	thisClient.inform_state("OFFLINE");
	exit(0);
	
	return CDialog::DestroyWindow();
}

//*****************************线程函数：接收心跳包，刷新左侧上线客户端信息********************
UINT updateThreadProc(LPVOID p)
{
	Client *lpClient = (Client *)p; //current client
	sockaddr_in sender;
	CString message;
	//hostname, ip, state from message
	CString hostname;
	CString ip;
	CString state;
	struct Peer peer;

	//loop to listen heart beating
	while(true)
	{

		if(lpClient->finished == true)
		{
			break;
		}

		//receive message
		lpClient->receive_heartBeat(sender, message);
		CString senderAddr = inet_ntoa(sender.sin_addr);
		//retrieve message to get struct Peer
		peer = retrieveMessage(message, hostname, ip, state);
		
		//judge the state, "ONLINE" or "OFFLINE"
		if(strcmp(state, "ONLINE") == 0)
		{
			//check if the client already exists
			set<struct Peer>::iterator it;
			for(it = lpClient->getPeerList().begin(); it != lpClient->getPeerList().end(); it++)
			{
				if(strcmp(it->ip, peer.ip) == 0)
				{
					break;
				}
			}
			
			//if this senderAddr doesn't exist in list
			if(it == lpClient->getPeerList().end())
			{
				//add peer
				lpClient->addPeer(peer);

				//update list control window
				int row = lpClient->listCtrl->InsertItem(lpClient->getPeerList().size(), hostname);
				lpClient->listCtrl->SetItemText(row, 1, senderAddr);
			}

		}
		else
		{
			//delete the item in set
			lpClient->deletePeer(peer);
			
			//get the line index in list control
			int lineIndex = lpClient->getLineIndex(peer.ip);

			lpClient->listCtrl->DeleteItem(lineIndex);
		}
	}

	return 0;
}

//****************************线程函数：持续发送心跳包*****************************
UINT sendHBeatThreadProc(LPVOID p)
{
	Client *lpClient = (Client *)p;

	//loop to send heart beat every 5s
	while(true)
	{
		//this client left, here is wrong
		//after window closed, broadcast ineffective
		if(lpClient->finished == true)
		{
			lpClient->inform_state("OFFLINE");
			break;
		}
		//this client logs on
		else
		{
			lpClient->inform_state("ONLINE");
		}

		Sleep(50);
	}

	return 0;
}

//******************************线程函数：持续更新在线用户数*************************
UINT updateUserNumProc(LPVOID p)
{
	
	CTheBirdDlg *thisClass = (CTheBirdDlg *)p;

	while(true)
	{
		if(thisClass->thisClient.finished == true)
		{
			break;
		}

		//update m_EditUserNumber
		m_edit1.Format("%d", thisClass->thisClient.getPeerList().size());
		//update IDC_EDIT
		thisClass->GetDlgItem(IDC_EDIT1)->SetWindowText(m_edit1);

		Sleep(50);
	}
	return 0;
}

//*****************************线程函数：接收文件************************************
UINT receiveFileProc(LPVOID p)
{
	CTheBirdDlg *thisClass = (CTheBirdDlg *)p;
	
	//
	char *file_location;
	//*********set up listening socket**********

	//初始化socket dll
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 0);
	::WSAStartup(sockVersion, &wsaData);
	
	// listen socket
	SOCKET server_socket, receive_socket;
	sockaddr_in local_addr, remote_addr;
	char buffer[256] = {0};
	int size_remoteAddr = sizeof(remote_addr);
	
	server_socket = ::socket(AF_INET, SOCK_STREAM, 0);
	// local address
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(FILERECVPORT);
	local_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	//dind server  socket to local address
	::bind(server_socket, (sockaddr *)&local_addr, sizeof(local_addr));
	//start listening
	::listen(server_socket, 5);

	while(true)
	{
		if(thisClass->thisClient.finished == true)
		{
			break;
		}

		//accept a connection
		receive_socket = ::accept(server_socket, (sockaddr *)&remote_addr, &size_remoteAddr);

		if(receive_socket != NULL)
		{
			//first, receive file info 
			::recv(receive_socket, buffer, 256, 0);
			if(buffer != NULL)
			{
				MessageBox(NULL, buffer, "receiving to D:\\", MB_OK);
				//new a file
				char path[] = "D:\\";
				file_location = strcat(path, buffer);
				CFile newFile(file_location, CFile::modeCreate|CFile::modeReadWrite);

				int len;
				if(newFile != NULL)
				{
					//loop to receive file content
					while(len = ::recv(receive_socket, buffer, 256, 0))
					{
						newFile.Write(buffer, 256);
					}
							
				}

				newFile.Close();
			}
		}
		
		//notice: file receivedm_edit2.Format("%s", strcat("file saved successfully: ", file_location));
		CString file_loc;
		file_loc.Format("%s", file_location);
		m_edit2 += "file received successfully: ";
		m_edit2 += file_loc;
		
		FrushFlag = true;
		closesocket(receive_socket);
		
	}

	::closesocket(server_socket);
	::WSACleanup();

	return 0;
}

//*****************************解析心跳包*******************************
struct Peer retrieveMessage(CString& message, CString& hostname, CString& ip, CString& state)
{
	struct Peer peer;

	//hostname = message.Tokenize(_T(" "), 0);
	//ip = message.Tokenize(_T(" "), 0);
	//state = message.Tokenize(_T(" "), 0);

	AfxExtractSubString(hostname, message, 0,' ');

	AfxExtractSubString(ip, message, 1, ' ');

	AfxExtractSubString(state, message, 2, ' ');


	peer.hostname = hostname;
	peer.ip = ip;

	return peer;
}

//******************************刷新按钮的触发函数*******************************
void CTheBirdDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	//updateThreadProc(&thisClient);

	//delete all items first
	thisClient.listCtrl->DeleteAllItems();

	set<struct Peer>:: iterator it;
	int itemIndex = 0;
	for(it = thisClient.getPeerList().begin(); it != thisClient.getPeerList().end(); it++)
	{
		int row = thisClient.listCtrl->InsertItem(itemIndex, it->hostname);
		thisClient.listCtrl->SetItemText(row, 1, it->ip);

		itemIndex++;
	}
}

//***********************聚焦list control 某一项后的触发函数*******************
void CTheBirdDlg::OnSetfocusList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
			for(int i = 0; i < m_list.GetItemCount(); i++ )
		{
			   if( m_list.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED || m_list.GetCheck(i))
			   {
				   CString current_item = m_list.GetItemText(i, 1);
				   //insert item when this item doesn't exist in set
				   set<CString>::iterator it;
				   for(it = items_selected.begin(); it != items_selected.end(); it++)
				   {
						if(strcmp(*it, current_item) == 0)
							break;
				   }
					
				   if(it == items_selected.end())
				   {
						items_selected.insert(current_item); 
				   }	 
			   }
		  }

	*pResult = 0;
}

//*********************《《在确认选中列表之前》》，取消选中后，将该项从set中删除****************
void CTheBirdDlg::OnKillfocusList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	
	// TODO: Add your control notification handler code here
	if(confirm_selects == false)
	{
		for(int i = 0; i < m_list.GetItemCount(); i++)
		{
			if(m_list.GetItemState(i, LVIS_SELECTED) != LVIS_SELECTED)
			{
				CString current_item = m_list.GetItemText(i, 1);
				set<CString>::iterator it;
				for(it = items_selected.begin(); it != items_selected.end(); it++)
				{
					if(strcmp(*it, current_item) == 0)
					{
						items_selected.erase(it);
						break;
					}
				}

			}
		}
	}

	*pResult = 0;
}



void CTheBirdDlg::OnButton7()
{
	// TODO: Add your control notification handler code here
	
	confirm_selects = true; //告知其他函数已按下确认按钮

	//traverse set
	set<CString>::iterator it;
	CString ip;
	for(it = items_selected.begin() ; it != items_selected.end(); it++)
	{
		//check if has insert *it
		for(int i = 0; i < m_listbox_ip.GetCount(); i++)
		{
			m_listbox_ip.GetText(i, ip);
			if(strcmp(ip, *it) == 0)
			{
				break;
			}
		}
	
		//insert an item in list box 
		if(i == m_listbox_ip.GetCount())
		{
			m_listbox_ip.InsertString(m_listbox_ip.GetCount(), *it);
		}
		
	}

}

//****************************选择文件*****************************
void CTheBirdDlg::OnButton5() 
{
	if(items_selected.size() == 0)
	{
		::MessageBox(NULL, "请先选择发送对象", "alert", MB_OK);
		return;
	}
	// TODO: Add your control notification handler code here
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT, NULL, this);
	fileDlg.DoModal();
	filePath = fileDlg.GetPathName();
	
	FILE* fp;
	fp = fopen(filePath,"rb");
	fseek(fp,0,SEEK_END); //将文件位置指针移至文件首
	fileSize = ftell(fp); 
	fclose(fp);
	
	//change file_selected flag
	file_selected = true;
	
	//
	m_edit4 = "preparing to send file ";
	m_edit4 += filePath;
	UpdateData(FALSE);
}

//**************************发送文件*******************************
void CTheBirdDlg::OnButton6() 
{
	// TODO: Add your control notification handler code here
	
	//获取remote ip
	CString remote_ip;
	m_listbox_ip.GetText(0, remote_ip);
	
	//初始化socket dll
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 0);
	::WSAStartup(sockVersion, &wsaData);
	
	//send file socket
	SOCKET send_socket = ::socket(AF_INET, SOCK_STREAM, 0);
	//remote address
	sockaddr_in remote_addr;
	remote_addr.sin_family = AF_INET;
	remote_addr.sin_port = htons(FILERECVPORT);
	remote_addr.sin_addr.S_un.S_addr = inet_addr(remote_ip);
	
	//connect to remote client
	int result = ::connect(send_socket, (sockaddr *)&remote_addr, sizeof(remote_addr));
	if(result != -1)
	{
		// notice text
		m_edit4 += "\r\n";
		m_edit4 += "sending file ";
		m_edit4 += filePath;
		UpdateData(FALSE);

		//send a file name msg
		CString fileName = getFileName(filePath);
		::send(send_socket, fileName, fileName.GetLength(), 0);

		CFile file(filePath, CFile::modeReadWrite);
		char buffer[256] = {0};
		int len;

		//send file content
		while(len = file.Read(buffer, 256))
		{
			::send(send_socket, buffer, 256, 0);
		}

		//close file
		file.Close();
	}

	closesocket(send_socket);
	WSACleanup();
}

CString getFileName(CString filePath)
{
	//AfxExtractSubString(hostname, filePath, 0, '\');
	int index = filePath.ReverseFind('\\');
	
	CString fileName = filePath.Right(filePath.GetLength() - index - 1);

	return fileName;
}
