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
//���캯��
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
	
	//�����׽���
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET)
	{
		return FALSE;
	}
	ULONG uAddr = inet_addr(pszRemoteAddr);
	if (uAddr == INADDR_NONE)
	{	
		//����ip��ַ������ΪuAddr����������
		//�����������ip��ַ
		hostent *pHost = gethostbyname(pszRemoteAddr);
		if (pHost == NULL)
		{
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;
		}
		//�õ��������ֽ�˳�����е�ip��ַ
		uAddr = ((struct in_addr*)*(pHost->h_addr_list))->s_addr;
	}
	//��д��������ַ��Ϣ
	sockaddr_in remote;
	remote.sin_addr.S_un.S_addr = uAddr;
	remote.sin_family = AF_INET;
	remote.sin_port = htons(4399);
	//���ӵ�Զ�̷�����
	if (connect(m_socket, (sockaddr*)&remote, sizeof(sockaddr)) == 0)
	{
		return TRUE;
	}
	return FALSE;
}

//����ָ������
BOOL NetTCPClient::sendContent(CString _content)
{
	//Ϊ������ӱ��ĸ�ʽ
	string sContent = FormatContent(_content.GetBuffer(0));
	const char *content = sContent.c_str();

	//��������
	send(m_socket, content, strlen(content), 0);

	return TRUE;
}

//�ͷ���Դ
void NetTCPClient::ReleaseResource()
{

	//�ر�socket���Ӻ��ͷ���Դ
	shutdown(m_socket, SD_SEND);
	closesocket(m_socket);
	//�ͷ�WS2_32��
	WSACleanup();
}

string NetTCPClient::FormatContent(string _content)
{
	//�Լ��������\r\n
	char hostname[50];
	int ri = gethostname(hostname, sizeof(hostname));
	if (ri == SOCKET_ERROR)
	{
		return NULL;
	}
	string self_name;
	self_name = hostname;
	//�Լ�ip��ַ\r\n
	hostent *host = gethostbyname(hostname);
	if (host == NULL)
	{
		return NULL;
	}
	ULONG uAddr = ((struct in_addr*)*(host->h_addr_list))->s_addr;
	in_addr *addr = (in_addr*)*(host->h_addr_list);
	string self_ip = inet_ntoa(addr[0]);

	//�Է��������\r\n
	string remotename = remote_name.GetBuffer(0);
	//�Է�ip��ַ\r\n
	string remoteip = ServerIP.GetBuffer(0);

	return self_name + "\r\n" + self_ip + "\r\n" + remotename + "\r\n" + remoteip + "\r\n" + _content + "\r\n";
}
