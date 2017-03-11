#include "StdAfx.h"
#include "Client.h"
#include <Winsock2.h>
#include <stdio.h>
#include <set>

using namespace std;

Client::Client()
{
	//load socket dll
	sockVersion = MAKEWORD(2, 0);
	WSAStartup(sockVersion, &wsaData);

	//initialize local_ip, hostname
	char name[256];
	gethostname(name, sizeof(name));
	hostent *hostInfo = gethostbyname(name);
	local_ip = inet_ntoa(*(in_addr *)hostInfo->h_addr_list[0]);
	hostname = name;

	//initialize broadcast_address
	int n = local_ip.ReverseFind('.');
	broadcast_address = local_ip.Left(n+1) + "255";

	//set Sockets
	udp_send = ::socket(AF_INET, SOCK_DGRAM, 0);
	chat_send = ::socket(AF_INET, SOCK_DGRAM, 0);
	udp_receive = ::socket(AF_INET, SOCK_DGRAM, 0);
	chat_receive = ::socket(AF_INET, SOCK_DGRAM, 0);
	u_long iMode = 0;
	ioctlsocket(chat_receive, FIONBIO, &iMode);
	//bind server addr(serverAddr) to heart-beating receive socket(udp_receive)
	//this udp_receive socket is used to receive heart beatings.
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(UDPRECEIVEPORT);
	serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(udp_receive, (LPSOCKADDR)&serverAddr, sizeof(serverAddr));

	//this udp socket is used to receive chat message
	chatAddr.sin_family = AF_INET;
	chatAddr.sin_port = htons(MESSAGERECVPORT);
	chatAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(chat_receive, (LPSOCKADDR)&chatAddr, sizeof(chatAddr));
}

Client::~Client()
{
	closesocket(udp_send);
	WSACleanup();
} 

void Client::inform_state(CString state)
{
	send_heartBeat(broadcast_address, make_message(state));
}

CString Client::make_message(CString state)
{
	
	return hostname + " " + local_ip + " " + state;
}


void Client::send_heartBeat(CString to_ip, CString message)
{
	//construct receiving side
	sockaddr_in receiver;
	receiver.sin_family = AF_INET;
	receiver.sin_port = htons(UDPRECEIVEPORT);
	receiver.sin_addr.S_un.S_addr = inet_addr((LPSTR)(LPCTSTR)to_ip);
	
	//send udp message
	char *buffer = (LPSTR)(LPCTSTR)message;
	sendto(udp_send, buffer, strlen(buffer), 0, (LPSOCKADDR)&receiver, sizeof(receiver));
}

void Client::receive_heartBeat(sockaddr_in& sender, CString& message)
{
	char buffer[256] = {0}; //receiving buffer
	int senderSize = sizeof(sender);
	//get udp message from other clients
	recvfrom(udp_receive, buffer, 256, 0, (LPSOCKADDR)&sender, &senderSize);
	//
	message = buffer;
}


void Client::send_message(CString ip,CString message){
	
	//receiver's address
	sockaddr_in receiver;
	receiver.sin_family = AF_INET;
	receiver.sin_port = htons(MESSAGERECVPORT);
	receiver.sin_addr.S_un.S_addr = inet_addr((LPSTR)(LPCTSTR)ip);
	
	char* buffer = (LPSTR)(LPCTSTR)message;
	sendto(chat_send, buffer, strlen(buffer), 0, (LPSOCKADDR)&receiver, sizeof(receiver));
}

void Client::receive_message(sockaddr_in& sender, CString& message)
{
	char buffer[256] = {0};
	int senderSize = sizeof(sender);

	recvfrom(chat_receive, buffer, 256, 0, (LPSOCKADDR)&sender, &senderSize);
	
	message.Format("%d", strlen(buffer));
}

bool Client::addPeer(struct Peer peer)
{

	peerList.insert(peer);
	return true;
}

bool Client::deletePeer(struct Peer peer)
{
	//traverse set
	set<struct Peer>::iterator it;
	for( it = peerList.begin(); it != peerList.end(); it++)
	{
		if(strcmp(it->ip, peer.ip) == 0)
		{
			peerList.erase(it); //delete
			return true;
		}
	}

	return false;
}

set<struct Peer>& Client::getPeerList()
{
	return peerList;
}

int Client:: getLineIndex(CString ip)
{
	
	int count = listCtrl->GetItemCount();
	
	for(int i = 0; i < count; i++)
	{
		CString item_ip = listCtrl->GetItemText(i, 1);
		
		if(strcmp(item_ip, ip) == 0)
		{
			return i;
		}
	}
}