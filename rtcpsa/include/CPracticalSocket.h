/*
 * for recevie and send message
 */
#ifndef _PRACTICAL_SOCKET_H
#define _PRACTICAL_SOCKET_H
#include "dbHandler.h"
#include "CMsgBuffer.h"
#include "CRtcProtocolParser.h"

//#include <infoext.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <map>
#include <set>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include <netdb.h>
#include <arpa/inet.h>
#include <iostream>


#include "timerpoll.h"
#include "MyLogger.h"


#define BUFFER_SIZE 8192
using namespace std;


typedef struct Packet{
	u_int32_t len;
	char buf[BUFFER_SIZE];
} Packet, *PacketPtr;



class CPracticalSocket{
private:
	pthread_t m_tListenId;

	int m_wcsPort;
	string m_wcsIp;

	int wcsfd;

	//webrtc user name mapped to socketfd
	map<string, int> m_mapWcsSockets;

	//offerSessionId
	map<string, string> m_mapOfferOrAnswer;

	set<int> m_wcsClifds;

	bool stop;
	CMsgBuffer msgBuffer;


public:
	CPracticalSocket();
	virtual ~CPracticalSocket();
	bool init(string&, int);
	//bool recvMsg(vector<string> &vecMsg);
	bool recvMsg(int& sockfd, string& msg);
	bool sendMsg(int clientSock, string msg);
	void stopThread(){
		msgBuffer.stop();
		stop = true;
	}

	int getwcsFd(){
		return wcsfd;
	}

	bool sendMsgToRtc(const string& strMsg, int sockfd);


private:
	static void * runTimers(void * data);
	bool handleMsgFromWebrtc(const string& strMsg, int sockfd);
	string createNewOfferOrAnswer();
	bool connectToWCS();

friend void * handleThread(void *);
friend int heartBeatTimeout(timer *);
friend int reConnectTimeout(timer *);
};





//set<SocketLastUsedTime> mg_socket_set;
//
//int main(){
//	for(int i=0; i<10; i++){
//		mg_socket_set.insert(SocketLastUsedTime(i));
//		set<SocketLastUsedTime>::iterator it = mg_socket_set.begin();
////		SocketLastUsedTime tmp = *it;
//		printf("%d\n", it->get_sockfd());
//	}
//	return 0;
//}


#endif
