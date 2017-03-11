// NetTCPClient.h: interface for the NetTCPClient class.
//
//////////////////////////////////////////////////////////////////////
#include <string>
using std::string;

#include <winsock2.h>
#pragma comment(lib, "WS2_32.lib")

#if !defined(AFX_NETTCPCLIENT_H__EE87EF11_F4B0_4A31_B3F0_DE6BFDC7A7C4__INCLUDED_)
#define AFX_NETTCPCLIENT_H__EE87EF11_F4B0_4A31_B3F0_DE6BFDC7A7C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class NetTCPClient  
{
public:
	NetTCPClient(CString server_ip, CString name);
	virtual ~NetTCPClient();
	BOOL Connect(LPCTSTR pszRemoteAddr);
	SOCKET m_socket;
	BOOL sendContent(CString content);
	CString ServerIP;
	CString remote_name;
	void ReleaseResource();
	string FormatContent(string _content);

};

#endif // !defined(AFX_NETTCPCLIENT_H__EE87EF11_F4B0_4A31_B3F0_DE6BFDC7A7C4__INCLUDED_)
