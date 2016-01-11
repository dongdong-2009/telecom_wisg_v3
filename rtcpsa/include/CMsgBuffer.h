#ifndef CMSGBUFFER_H
#define CMSGBUFFER_H
#include <string>
#include <queue>
#include <stdio.h>
#include <pthread.h>
using namespace std;


class CMsgBuffer{
private:
	pthread_mutex_t mutex;
	bool toStop;
	typedef pair<int, string> sockfdAndMsg;
	queue<sockfdAndMsg> buffer;
public:
	void stop();
	CMsgBuffer();
	~CMsgBuffer();
	bool storeMsg(int sockfd, string msg);
	bool getMsg(int &sockfd, string &msg);
};


#endif
