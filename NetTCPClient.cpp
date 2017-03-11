// NetTCPClient.cpp: implementation of the NetTCPClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "thebird.h"
#include "NetTCPClient.h"
#include <string>

using std::string;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//构造函数
//NetTCPClient::NetTCPClient(CString server_ip, SOCKET so): m_socket(so)
NetTCPClient::NetTCPClient(CString server_ip, CString name)
{
	ServerIP = server_ip;
	remote_name = name;
	m_socket = NULL;
}

NetTCPClient::~NetTCPClient()
{

}

BOOL NetTCPClient::Connect(LPCTSTR pszRemoteAddr)
{
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 0);
	::WSAStartup(sockVersion, &wsaData);
	
	//创建套接字
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET)
	{
		return FALSE;
	}
	ULONG uAddr = inet_addr(pszRemoteAddr);
	if (uAddr == INADDR_NONE)
	{	
		//不是ip地址，则认为uAddr是主机名称
		//从主机名获得ip地址
		hostent *pHost = gethostbyname(pszRemoteAddr);
		if (pHost == NULL)
		{
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;
		}
		//得到以网络字节顺序排列的ip地址
		uAddr = ((struct in_addr*)*(pHost->h_addr_list))->s_addr;
	}
	//填写服务器地址信息
	sockaddr_in remote;
	remote.sin_addr.S_un.S_addr = uAddr;
	remote.sin_family = AF_INET;
	remote.sin_port = htons(4399);
	//连接到远程服务器
	if (connect(m_socket, (sockaddr*)&remote, sizeof(sockaddr)) == 0)
	{
		return TRUE;
	}
	return FALSE;
}

//发送指定内容
BOOL NetTCPClient::sendContent(CString _content)
{
	//为内容添加报文格式
	string sContent = FormatContent(_content.GetBuffer(0));
	const char *content = sContent.c_str();

	//发送内容
	send(m_socket, content, strlen(content), 0);

	return TRUE;
}

//释放资源
void NetTCPClient::ReleaseResource()
{

	//关闭socket连接和释放资源
	shutdown(m_socket, SD_SEND);
	closesocket(m_socket);
	//释放WS2_32库
	WSACleanup();
}

string NetTCPClient::FormatContent(string _content)
{
	//自己计算机名\r\n
	char hostname[50];
	int ri = gethostname(hostname, sizeof(hostname));
	if (ri == SOCKET_ERROR)
	{
		return NULL;
	}
	string self_name;
	self_name = hostname;
	//自己ip地址\r\n
	hostent *host = gethostbyname(hostname);
	if (host == NULL)
	{
		return NULL;
	}
	ULONG uAddr = ((struct in_addr*)*(host->h_addr_list))->s_addr;
	in_addr *addr = (in_addr*)*(host->h_addr_list);
	string self_ip = inet_ntoa(addr[0]);

	//对方计算机名\r\n
	string remotename = remote_name.GetBuffer(0);
	//对方ip地址\r\n
	string remoteip = ServerIP.GetBuffer(0);

	return self_name + "\r\n" + self_ip + "\r\n" + remotename + "\r\n" + remoteip + "\r\n" + _content + "\r\n";
}
