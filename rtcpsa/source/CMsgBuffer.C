#include "CMsgBuffer.h"

CMsgBuffer::CMsgBuffer():toStop(false){
	 pthread_mutex_init(&mutex, NULL);		//初始化互斥量
}

CMsgBuffer::~CMsgBuffer(){
	pthread_mutex_destroy(&mutex);
}

void CMsgBuffer::stop(){
	toStop = true;
}

bool CMsgBuffer::storeMsg(int sockfd, string msg){
	pthread_mutex_lock(&mutex);
	if(toStop){
	    pthread_mutex_unlock(&mutex);
	    return false;
	}
	buffer.push(make_pair(sockfd, msg));
	pthread_mutex_unlock(&mutex);
	return true;
}

bool CMsgBuffer::getMsg(int& sockfd, string& msg){
	pthread_mutex_lock(&mutex);
	if(buffer.empty() || toStop){
	    pthread_mutex_unlock(&mutex);
	    return false;
	}
	sockfdAndMsg sa= buffer.front();
	sockfd = sa.first;
	msg = sa.second;
	buffer.pop();
	printf("getMsg current size %d\n", buffer.size());
	pthread_mutex_unlock(&mutex);
	return true;
}
