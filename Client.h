#include "StdAfx.h"
#include <Winsock2.h>
#include <iostream>
#include <set>

//this port receives heart beatings
#define UDPRECEIVEPORT 2550 //not required to set a udp send port
//this port receives text messages
#define MESSAGERECVPORT 2551

#pragma once

struct Peer{
	CString hostname;
	CString ip;

	bool operator<(const struct Peer& other) const
	{
		return ip < other.ip;
	}
};


using namespace std;
//Client class stores all the attributes belonging to a client, 
//and handles all the send and receive functions (UDP).

class Client
{
private:
	CString local_ip;
	CString hostname;

	CString broadcast_address;

	sockaddr_in chatAddr;
	sockaddr_in serverAddr;

	SOCKET udp_send;
	SOCKET chat_send;
	SOCKET udp_receive;
	SOCKET chat_receive;
	//load SOCKET dll
	WORD sockVersion;
	WSADATA wsaData;

	//peer list
	set<struct Peer> peerList;

public :
	CListCtrl *listCtrl; //reference to m_List
	CListBox *message_list;// reference to m_List
	CString *userNum;
	
	bool finished;
	bool chatfinished;
	Client();//constructor
	~Client();
	void inform_state(CString state); //send heart-beating package
	CString make_message(CString state);
	
	void send_heartBeat(CString to_ip, CString message); //send UDP heart beat message
	
	void receive_heartBeat(sockaddr_in& sender, CString& message); //receive udp message(heart beatings)

	void send_message(CString ip,CString message); //send edit message
	void receive_message(sockaddr_in& sender, CString& message); //receive edit message

	bool addPeer(struct Peer peer);
	bool deletePeer(struct Peer peer);
	set<struct Peer>& getPeerList();
	int getLineIndex(CString ip);
};
