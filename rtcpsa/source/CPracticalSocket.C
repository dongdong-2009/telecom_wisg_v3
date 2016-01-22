#include "CPracticalSocket.h"
#include "rtc_msg_def.h"

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/xml/domconfigurator.h>

using namespace log4cxx::xml;
using namespace log4cxx;

log4cxx::LoggerPtr logger;


#define MAXCONN 10


UINT selectTimeoutCount;
UINT sendHeartBeatCount;



timers_poll * ptimer_poll = NULL;
timer * heartBeatPTimer = NULL;
timer * reConnectPTimer = NULL;

bool isJson(string& strMsg) {
	Json::Reader reader;
	Json::Value jsonValue;
	if (reader.parse(strMsg, jsonValue) == false || jsonValue.type()
			!= Json::objectValue)
		return false;
	return true;
}



void* handleThread(void *pParam) {
	CPracticalSocket& currSock = *(CPracticalSocket*) pParam;
	fd_set listenSet;
	char buf[BUFFER_SIZE + 1];
	struct timeval timeout;

	while (!currSock.stop) {
		int wcsfd = currSock.wcsfd;
		int maxfd = wcsfd;

		FD_ZERO(&listenSet);
		FD_SET(wcsfd, &listenSet);
		memset(buf, 0, sizeof(buf));

		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		int ret = select(maxfd + 1, &listenSet, NULL, NULL, &timeout);
		if (ret == -1) {
			LOG4CXX_DEBUG(logger, "handleThread:: select() failed!");
		} else if (ret == 0) {
			//timeout start send heartbeatTimer
			++ selectTimeoutCount;
			if(selectTimeoutCount == 22){//20s for one heartbeat msg
				selectTimeoutCount = 0;
				string str = "HeartBeat";
				currSock.sendMsg(wcsfd, str);

				heartBeatPTimer->timer_modify_internal(20);

			}

		} else {
			//if seted, need not check other fd

			if (FD_ISSET(wcsfd, &listenSet)) {
				selectTimeoutCount = 0;
				//test if data from wcs received
				// recv message length
				u_int32_t packetlen = 0;
				if (recv(wcsfd, (char *) &packetlen,
						sizeof(u_int32_t), 0) <= 0) {
					LOG4CXX_ERROR(logger, "handleThread:: can not get the size of the msg");
					close(wcsfd);
					currSock.stop = true;
					reConnectPTimer->timer_modify_internal(30);
					break;
				}
				packetlen = ntohl(packetlen);
				LOG4CXX_DEBUG(logger, "handleThread:: to recv"<<packetlen);
				// recv message content
				u_int32_t bufferSize = sizeof(buf) - 1;
				string strMsg;
				while (packetlen > 0) {
					int toRead = packetlen > bufferSize ? bufferSize
							: packetlen;

					int bytes = recv(wcsfd, buf, toRead, 0);
					if (bytes == -1) {
						LOG4CXX_ERROR(logger, "handleThread:: receive failed.");
						close(wcsfd);
						currSock.stop = true;
						reConnectPTimer->timer_modify_internal(30);
						break;
					} else if (bytes == 0) {
						LOG4CXX_ERROR(logger,
								"RecvThread: socket closed by the other side.");
						close(wcsfd);
						currSock.stop = true;
						reConnectPTimer->timer_modify_internal(30);
						break;
					} else {
						buf[bytes] = '\0';
						strMsg += buf;
//								printf("%s, %d\n", buf, strlen(buf));
						if(strlen(buf) == 0)
							bytes = 0;
					}
					packetlen -= bytes;
				}

				LOG4CXX_DEBUG(logger, "CPracticalSocket recv strMsg"<<strMsg.c_str());

				if(strMsg.find("heartBeatResponse") != string::npos){
					heartBeatPTimer->timer_modify_internal(0);
				}


				if (isJson(strMsg) == true) {
					currSock.handleMsgFromWebrtc(strMsg, wcsfd);
				}

			}
		}
	}
	return NULL;
}

void * CPracticalSocket::runTimers(void *data) {
	timers_poll *my_timers = (timers_poll *) data;
	my_timers->run();

	return NULL;
}

int heartBeatTimeout(timer * ptimer){
	//heartBeat timeout
	CPracticalSocket& currSock = *(CPracticalSocket*) (ptimer->timer_get_userdata());
	//currSock.connectToWCS();
	currSock.stop = true;//stop the handle thread
	close(currSock.wcsfd);
	currSock.wcsfd = -1;

	heartBeatPTimer->timer_modify_internal(0);
	reConnectPTimer->timer_modify_internal(30); //every 30s connect to WCS if failed
	return 1;
}

int reConnectTimeout(timer * ptimer){

	CPracticalSocket& currSock = *(CPracticalSocket*) (ptimer->timer_get_userdata());
	if(true == currSock.connectToWCS()){
		reConnectPTimer->timer_modify_internal(0);
		// create listenThread
		if (0 != pthread_create(&(currSock.m_tListenId), NULL, handleThread, &currSock)) {
			LOG4CXX_ERROR(logger, "reConnectTimeout: RtcPsa create handleThread failed");
			currSock.stop = true;
			exit(-1);
		}
		else{
			currSock.stop = false;
		}
	}
	else{
		double interval = reConnectPTimer->timer_get_timer_interval();
		if(interval < 600){
			interval += 30;
			reConnectPTimer->timer_modify_internal(interval);
		}

		LOG4CXX_DEBUG(logger, "Reconnect to wcs failed, will reconnect after "<<interval<<"s.");
	}

	return 1;
}


CPracticalSocket::CPracticalSocket() :
	m_tListenId(0), stop(false) {
	// should get from config
	// m_listenPort = 9870;
}

CPracticalSocket::~CPracticalSocket() {
	cout << "call desc" << endl;
	stopThread();
	pthread_join(m_tListenId, NULL);


	m_mapOfferOrAnswer.clear();

	close(wcsfd);
}

bool CPracticalSocket::connectToWCS(){
	int wcsSocket = 0;
	if ((wcsSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		LOG4CXX_ERROR(logger, "RtcPsa create socket failed");
		return false;
	}

	int opt = 1;
	setsockopt(wcsSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	struct sockaddr_in serverAddr;
	bzero(&serverAddr, sizeof(struct sockaddr_in));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(m_wcsPort);
	serverAddr.sin_addr.s_addr = inet_addr(m_wcsIp.c_str());

	if(-1 == connect(wcsSocket, (struct sockaddr *) &serverAddr, sizeof(struct sockaddr))){
		LOG4CXX_ERROR(logger, "RtcPsa connect to Server failed");
		return false;
	}

	sendMsg(wcsSocket, "WISG");

	wcsfd = wcsSocket;

	return true;
}

bool CPracticalSocket::init(string& wcsIp, int wcsPort) {
	selectTimeoutCount = 0;

	log4cxx::xml::DOMConfigurator::configureAndWatch("etc/log4cxx.xml", 5000);

	logger = log4cxx::Logger::getLogger("SgFileAppender");

	m_wcsIp = wcsIp;
	m_wcsPort = wcsPort;
	LOG4CXX_INFO(logger, "RtcPsa connect to wcs "<<wcsIp.c_str()<<":"<<wcsPort);

	wcsfd = -1;

//	if(false == connectToWCS()){
//		return false;
//	}


//	// create handleThread waiting wcs data
//	if (0 != pthread_create(&m_tListenId, NULL, handleThread, this)) {
//		LOG4CXX_ERROR(logger, "RtcPsa create handleThread failed");
//		stop = true;
//		return false;
//	}

	ptimer_poll = new timers_poll(2);
	pthread_t thread_id = 0;

	if (0 != pthread_create(&thread_id, NULL, runTimers, (void *)ptimer_poll)) {
		LOG4CXX_ERROR(logger, "RtcPsa create run_timers thread failed");
		exit(-1);
		return false;
	}
	else{
		heartBeatPTimer = new timer(0, heartBeatTimeout, this, 1); //heartbeat timer
		reConnectPTimer = new timer(5, reConnectTimeout, this, 1); //reconnect timer
		ptimer_poll->timers_poll_add_timer(reConnectPTimer);
		ptimer_poll->timers_poll_add_timer(heartBeatPTimer);
	}



	LOG4CXX_INFO(logger, "Socket initializes successfully")
	return true;
}

bool CPracticalSocket::recvMsg(int& sockfd, string& msg) {
	return msgBuffer.getMsg(sockfd, msg);
}

bool CPracticalSocket::sendMsg(int clientSock, string msg) {
	LOG4CXX_DEBUG(logger, "will send msg " << msg << " to " << clientSock);
	fd_set writeSet;
	FD_ZERO(&writeSet);
	FD_SET(clientSock, &writeSet);
	int ret = select(clientSock + 1, NULL, &writeSet, NULL, NULL);
	if (ret == -1) {
		LOG4CXX_DEBUG(logger, "sendMsg: select() failed!\n");
	} else if (ret == 0) {
		//printf("SendThread: select() time out.\n");
	} else {
		if (FD_ISSET(clientSock, &writeSet)) {
			Packet packet;
			strncpy(packet.buf, msg.c_str(), BUFFER_SIZE);
			int msgSize = strlen(packet.buf);
			packet.len = htonl(msgSize);
			int bytes = send(clientSock, (char *) &packet, msgSize
					+ sizeof(packet.len), 0);
			if (bytes == -1) {
				LOG4CXX_ERROR(logger, "sendMsg: send failed!");
				return false;
			} else if (bytes != msgSize
					+ sizeof(packet.len)) {
				LOG4CXX_ERROR(logger, "sendMsg: send msg truncked");
			} else {
				//do nothing
				LOG4CXX_INFO(logger, "send to webrtc server succesfully");
			}

		}
	}
	return true;
}


bool CPracticalSocket::sendMsgToRtc(const string& strMsg, int sockfd) {
	CRtcProtocolParser rtcParser(strMsg);
	CRoapParser roapParser = rtcParser.getRoapParser();
	string offerSessionId = roapParser.getOfferSessionId();

	if (sendMsg(sockfd, strMsg))
		return true;
	else
		return false;

}

bool CPracticalSocket::handleMsgFromWebrtc(const string& strMsg, int sockfd) {
	CRtcProtocolParser rtcParser(strMsg);
	CRoapParser roapParser = rtcParser.getRoapParser();
	string offerSessionId = roapParser.getOfferSessionId();

	m_mapWcsSockets.insert(make_pair<string, int> (offerSessionId, sockfd));
	try {
		if (roapParser.getType() == ROAP_CANDIDATE) {
			//last candidate
			if (roapParser.getSdp().length() != 0) {
				if (m_mapOfferOrAnswer.find(offerSessionId)
						!= m_mapOfferOrAnswer.end()) {
					LOG4CXX_DEBUG(logger, "receive candidate, send offer with one candidate");
					string offerOrAnswer = m_mapOfferOrAnswer[offerSessionId];
		
					CRtcProtocolParser rtcParser2(offerOrAnswer);
					CRoapParser roapParser2 = rtcParser2.getRoapParser();
					string sdp = roapParser2.getSdp();

					string candidateSdp = roapParser.getSdp();
					if(candidateSdp.find("a=") == string::npos)
					{
						sdp += "a=" + candidateSdp + "\r\n";
					}
					else{
						sdp += candidateSdp;
					}
					roapParser2.setSdp(sdp);

					rtcParser2.setRoap(roapParser2.getRoapJson());

					string newOfferOrAnswer = rtcParser2.toPlainString();
					m_mapOfferOrAnswer.erase(offerSessionId);

					msgBuffer.storeMsg(sockfd, newOfferOrAnswer);

					return true;
				} else {
					return false;
				}
			}
		} else if (ROAP_OFFER == roapParser.getType() || ROAP_ANSWER
				== roapParser.getType()) {
			m_mapOfferOrAnswer[offerSessionId] = strMsg;

			if (roapParser.getSdp().find("a=candidate") != string::npos) {
				m_mapOfferOrAnswer.erase(offerSessionId);

				msgBuffer.storeMsg(sockfd, strMsg);
				return true;
			}
		} else {
			//other type message
			msgBuffer.storeMsg(sockfd, strMsg);
			return true;
		}
	} catch (std::runtime_error e) {
		LOG4CXX_ERROR(logger, "CPracticalSocket::receive exception:"<<e.what());
		//psaErrorLog(m_PSAID, "catch exception: %s\n", e.what());

	}
	return false;
}

