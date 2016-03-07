#include "CR2SCallModule.h"
#include "MyLogger.h"

CLONE_COMP(CR2SCallModule)
CREATE_COMP(CR2SCallModule)

using namespace log4cxx::xml;
using namespace log4cxx;

static MyLogger& mLogger = MyLogger::getInstance("etc/log4cxx.xml",
		"SgFileAppender");

timers_poll * my_timers;
timer * timer_rtc;
TimerType * timerType_rtc;

timer * timer_sip;
TimerType * timerType_sip;
pthread_t thread_id;

void * thread_fun(void *data) {
	timers_poll *timers = (timers_poll *) data;
	timers->run();
	return NULL;
}

int timeout_callback(timer * ptimer) {
	TimerType* myType = (TimerType *) ptimer->timer_get_userdata();
	CR2SCallModule* myMod = (CR2SCallModule*) myType->currMod;
	myMod->timeOut(ptimer);
	return 0;
}

CR2SCallModule::CR2SCallModule(PCGFSM afsm) :
	CUACSTask(afsm), m_rtcContext(*this), m_sipContext(*this),
			m_isDispatcherAddrSet(FALSE) {
	// get LOGADDR_DISPATCHER from conf
	CProperties* properties = CPropertiesManager::getInstance()->getProperties(
			"rtc.env");
	int LOGADDR_DISPATCHER = properties->getIntProperty("LOGADDR_DISPATCHER");
	if (LOGADDR_DISPATCHER == -1) {
		print(
				"LOGADDR_DISPATCHER not correctly set in samples.env,Use DEFAULT 231.\n");
		this->LOGADDR_DISPATCHER = 231;
	} else {
		this->LOGADDR_DISPATCHER = LOGADDR_DISPATCHER;
	}

	CProperties* properties2 =
			CPropertiesManager::getInstance()->getProperties("gateway.env");
	int accessMode = properties2->getIntProperty("accessMode");
	if (accessMode == -1) {
		accessMode = 0;
	} else {
		this->m_accessMode = accessMode;
	}
	m_rtcCtrlMsg = new TRtcCtrlMsg();
	m_intCtrlMsg_Rtc = NULL;
	m_intCtrlMsg_Sip = NULL;

	m_isSdpConfirmed = false;
	m_sipCtrlMsg = NULL;
	m_sdpModifyFlag = 0;

	m_joinFlag = 0;
	m_endFlag = 0;
	m_joinSend = false;
	m_sessionFlag = false;
	m_isImsVideoValid = true;

	/**
	 * 设置定时器，因为MCF一个task只支持一个定时器，下面采用第三方的定时器，自定义
	 */
	my_timers = new timers_poll(128);
	if (pthread_create(&thread_id, NULL, thread_fun, (void *) my_timers) != 0) {
		LOG4CXX_ERROR(mLogger.getLogger(), "Create timer thread failed");
		exit(-1);
	}

	timerType_rtc = new TimerType();
	timerType_rtc->timer_id = 1;
	timerType_rtc->currMod = this;
	timer_rtc = new timer(0, timeout_callback, timerType_rtc, 1);
	my_timers->timers_poll_add_timer(timer_rtc);

	timerType_sip = new TimerType();
	timerType_sip->timer_id = 2;
	timerType_sip->currMod = this;
	timer_sip = new timer(0, timeout_callback, timerType_sip, 1);
	my_timers->timers_poll_add_timer(timer_sip);

}

CR2SCallModule::~CR2SCallModule() {
	if (m_rtcCtrlMsg) {
		delete m_rtcCtrlMsg;
		m_rtcCtrlMsg = NULL;
	}

	if (m_sipCtrlMsg) {
		delete m_rtcCtrlMsg;
		m_rtcCtrlMsg = NULL;
	}

	if (m_intCtrlMsg_Rtc) {
		delete m_intCtrlMsg_Rtc;
		m_intCtrlMsg_Rtc = NULL;
	}

	if (m_intCtrlMsg_Sip) {
		delete m_intCtrlMsg_Sip;
		m_intCtrlMsg_Sip = NULL;
	}

	my_timers->timers_poll_deactive();//终止定时器

	delete timerType_rtc;
	delete timer_rtc;
	timer_rtc = NULL;

	delete timerType_sip;
	delete timer_sip;
	timer_sip = NULL;

	delete my_timers;
	my_timers = NULL;

	//mLogger.getLogger() = 0;

}

PTUACData CR2SCallModule::createData() {
	return new TUACData();
}

void CR2SCallModule::initState() {
	m_sipContext.enterStartState();
	m_rtcContext.enterStartState();
}

bool CR2SCallModule::selectSipUser(string rtcname) {
	CHAR pcSQLStatement[512];
	sprintf(
			pcSQLStatement,
			"SELECT sipname FROM user_map_table WHERE isRegistered = 1 AND isCalling = 0 order by lastUsedTime asc");
	CDB::instance()->execSQL(pcSQLStatement);
	PTSelectResult result = CDB::instance()->getSelectResult();

	if (result->rowNum > 0) {
		m_sipName = result->pRows->arrayField[0].value.stringValue;
		sprintf(
				pcSQLStatement,
				"UPDATE user_map_table SET rtcname = '%s', isCalling = 1 WHERE sipname = '%s'",
				rtcname.c_str(), m_sipName.c_str());
		CDB::instance()->execSQL(pcSQLStatement);

		LOG4CXX_INFO(mLogger.getLogger(), "selectSipUser: rtcname: "<<rtcname<<" map to sipname: "<<m_sipName);
		return true;
	} else {
		LOG4CXX_ERROR(mLogger.getLogger(), "selectSipUser: No IMS sip code idle.");
		return false;
	}

}

/** 在这里填充dispatcher的地址
 *  如果DIALOG_END，则通知dispatcher，消除会话信息
 *  如果消息最终发到sip（type = sip）那么需要进行转换再发到dispatcher，否则直接转发。见文档
 */
void CR2SCallModule::sendToDispatcher(TUniNetMsgName msgName,
		TUniNetMsgType msgType, TDialogType dialog, PTCtrlMsg pCtrlMsg,
		PTMsgBody pMsgBody) {
	PTUniNetMsg newMsg = new TUniNetMsg();
	newMsg->tAddr.phyAddr = 0;
	newMsg->tAddr.logAddr = LOGADDR_DISPATCHER;
	newMsg->msgType = msgType;
	newMsg->msgName = msgName;
	newMsg->dialogType = dialog;

	newMsg->ctrlMsgHdr = pCtrlMsg;
	newMsg->setCtrlMsgHdr();
	if (pMsgBody != NULL) {
		newMsg->msgBody = pMsgBody;
		newMsg->setMsgBody();
	}
	sendMsg(newMsg);
}

void CR2SCallModule::endTask_Rtc() {
	LOG4CXX_DEBUG(mLogger.getLogger(), "endTask finished");
	// 产生一条DIALOG——END消息，发给dispatcher，清除会话信息
	if(m_rtcCtrlMsg != NULL)
		sendToDispatcher(RTC_OK, RTC_TYPE, DIALOG_END, m_rtcCtrlMsg->clone(), NULL);
	if(m_intCtrlMsg_Rtc != NULL)
		sendToDispatcher(INT_CLOSE, INT_TYPE, DIALOG_END, m_intCtrlMsg_Rtc->clone(), NULL);
	m_endFlag = m_endFlag | 0x2;
	if ((m_endFlag & 0x3) == 0x3) {
		LOG4CXX_DEBUG(mLogger.getLogger(), "endTask_Rtc end()");
		end();

	}
}

void CR2SCallModule::endTask_Sip() {
	LOG4CXX_DEBUG(mLogger.getLogger(), "endTask finished");
	if (m_accessMode == 1 || m_accessMode == 2) {
		CUserMapHelper::resetCalling(m_sipName);
	}

	// 产生一条DIALOG——END消息，发给dispatcher，清除会话信息
	if(m_sipCtrlMsg != NULL)
		sendToDispatcher(SIP_BYE, SIP_TYPE, DIALOG_END, m_sipCtrlMsg->clone(), NULL);
	if(m_intCtrlMsg_Sip != NULL)
		sendToDispatcher(INT_CLOSE, INT_TYPE, DIALOG_END, m_intCtrlMsg_Sip->clone(), NULL);
	m_endFlag = m_endFlag | 0x1;
	if ((m_endFlag & 0x3) == 0x3) {
		LOG4CXX_DEBUG(mLogger.getLogger(), "endTask_Sip end()");
		end();
	}

}

//处理消息
void CR2SCallModule::procMsg(PTUniNetMsg msg) {
	if (!m_isDispatcherAddrSet) {
		m_dispatcherAddr = msg->oAddr;
		m_isDispatcherAddrSet = TRUE;
	}
	LOG4CXX_DEBUG(mLogger.getLogger(), "procMsg: RTC current state: "<< m_rtcContext.getState().getName() << ", recv msgName "<<msg->getMsgNameStr());
	LOG4CXX_DEBUG(mLogger.getLogger(), "procMsg: SIP current state: "<< m_sipContext.getState().getName() << ", recv msgName "<<msg->getMsgNameStr());
	LOG4CXX_DEBUG(mLogger.getLogger(), "procMsg: recv Msg: \n"<<CTUniNetMsgHelper::toString(msg));
	switch (msg->msgName) {
	case RTC_OFFER: {
		*m_rtcCtrlMsg = *((PTRtcCtrlMsg) msg->ctrlMsgHdr);

		m_offerSessionId = m_rtcCtrlMsg->offerSessionId;
		m_seq = ((TRtcOffer *) msg->msgBody)->seq;

		CHAR buf[33];
		CSipMsgHelper::generateRandomNumberStr(buf);
		string str = buf;
		str += buf;
		m_rtcCtrlMsg->answerSessionId = str.c_str();
		swap(m_rtcCtrlMsg->from, m_rtcCtrlMsg->to);
		LOG4CXX_DEBUG(mLogger.getLogger(),"rtcContext onOffer");
		m_rtcContext.onOffer(msg);
		break;
	}
	case RTC_ANSWER:
		m_rtcContext.onAnswer(msg);
		break;
	case RTC_OK:
		m_rtcContext.onOK(msg);
		break;
	case RTC_SHUTDOWN:
		m_rtcContext.onShutDown(msg);
		break;
	case RTC_ERROR:
		m_rtcContext.onShutDown(msg);
		break;

	case INT_RESPONSE: {
		PTIntCtrlMsg intCtrlMsg = (PTIntCtrlMsg) msg->ctrlMsgHdr;
		if (!strcmp(intCtrlMsg->to.c_str(), "sip_call")) {
			m_sipContext.onSdpAnswer(msg);
		} else if (!strcmp(intCtrlMsg->to.c_str(), "rtc_call")) {
			m_rtcContext.onSdpAnswer(msg);
		}
		break;
	}

	case INT_ERROR: {
		PTIntCtrlMsg intCtrlMsg = (PTIntCtrlMsg) msg->ctrlMsgHdr;
		if (!strcmp(intCtrlMsg->to.c_str(), "sip_call")) {
			m_sipContext.onError(msg);
		} else if (!strcmp(intCtrlMsg->to.c_str(), "rtc_call")) {
			m_rtcContext.onError(msg);
		}
		break;
	}

	case INT_CLOSE: {
		PTIntCtrlMsg intCtrlMsg = (PTIntCtrlMsg) msg->ctrlMsgHdr;
		if (!strcmp(intCtrlMsg->to.c_str(), "sip_call")) {
			m_sipContext.onClose(msg);
		} else if (!strcmp(intCtrlMsg->to.c_str(), "rtc_call")) {
			m_rtcContext.onClose(msg);
		}
		break;
	}

	case SIP_RESPONSE: {
		m_sipCtrlMsg->via = ((PTSipCtrlMsg) msg->ctrlMsgHdr)->via;
		m_sipContext.onResponse(msg);
		break;
	}

	case SIP_ACK: {
		m_sipContext.onAck(msg);
		break;
	}

	case SIP_INVITE: {
		m_sipContext.onInvite(msg);
	}
	case SIP_BYE: {
		m_sipContext.onBye(msg);
		break;
	}
	case SIP_UPDATE: {
		m_sipContext.onUpdate(msg);
		break;
	}
	default:
		LOG4CXX_ERROR(mLogger.getLogger(), "procMsg:unknow msgName "<<msg->getMsgNameStr())
		;
		//收到非法消息,忽略.等待超时.
		//endTask();
		break;
	}
	LOG4CXX_DEBUG(mLogger.getLogger(), "after procMsg state: "<<m_rtcContext.getState().getName());
	LOG4CXX_DEBUG(mLogger.getLogger(), "after procMsg state: "<< m_sipContext.getState().getName());
}

//处理定时器超时,不是mcf
void CR2SCallModule::timeOut(timer* ptimer) {
	TimerType * myType = (TimerType *) ptimer->timer_get_userdata();
	UINT timer_id = myType->timer_id;
	LOG4CXX_INFO(mLogger.getLogger(), "The CR2SCallModule task received a timeout event:"<<myType->timer_id);

	TTimeMarkExt timerMark;
	getTimeMarkExt(timer_id, timerMark);
//	timerMark.timerDelay = 0;

	switch (timer_id) {
	//	case SIP_200OK_TIMEOUT:
	//	case SIP_ACK_TIMEOUT:
	//	case SIP_ACTIVE_TIMEOUT:
	//	case SIP_CONNECTING_TIMEOUT:
	//	case SIP_WAITBEAR_TIMEOUT:
	//	case SIP_RING_TIMEOUT:
	case 2:
		m_sipContext.onTimeOut(timerMark);
		break;
		//	case RTC_CONNECTION_TIMEOUT:
		//	case RTC_SHUTDOWN_TIMEOUT:
		//	case RTC_WAITSIP_TIMEOUT:
		//	case RTC_WAITBEAR_TIMEOUT:
	case 1:
		m_rtcContext.onTimeOut(timerMark);
		break;
	default:
		LOG4CXX_ERROR(mLogger.getLogger(), "Timeout: unknown Type")
		;
		break;
	}

}

void CR2SCallModule::setTimer(UINT timer_id) {
	TTimeMarkExt timerMark;
	getTimeMarkExt(timer_id, timerMark);
	switch (timer_id) {
	case SIP_200OK_TIMEOUT:
	case SIP_ACK_TIMEOUT:
	case SIP_ACTIVE_TIMEOUT:
	case SIP_CONNECTING_TIMEOUT:
	case SIP_WAITBEAR_TIMEOUT:
	case SIP_RING_TIMEOUT: {
		//printf("setTimer_sip:: %d\n", timerMark.timerDelay);
		//TimerType* myType = (TimerType *) timer_sip->timer_get_userdata();
		//myType->timer_id = timer_id;
		timer_sip->timer_modify_internal(timerMark.timerDelay);

	}
		break;
	case RTC_CONNECTION_TIMEOUT:
	case RTC_SHUTDOWN_TIMEOUT:
	case RTC_WAITSIP_TIMEOUT:
	case RTC_WAITBEAR_TIMEOUT: {
		//printf("setTimer_rtc:: %d\n", timerMark.timerDelay);
		//TimerType* myType = (TimerType *) timer_sip->timer_get_userdata();
		//myType->timer_id = timer_id;
		timer_rtc->timer_modify_internal(timerMark.timerDelay);

	}
		break;
	default:
		LOG4CXX_ERROR(mLogger.getLogger(), "setTimer: unknown Type "<<timer_id)
		;
		break;
	}
}

void CR2SCallModule::stopTimer_Rtc() {
	timer_rtc->timer_modify_internal(0);
}

void CR2SCallModule::stopTimer_Sip() {
	timer_sip->timer_modify_internal(0);
}

void CR2SCallModule::onTimeOut(TTimeMarkExt timerMark) {
	LOG4CXX_INFO(mLogger.getLogger(), "The CR2SCallModule can't handle mcf timeout event");
}

//
BOOL CR2SCallModule::msgMap(TUniNetMsg *pSrcMsg, TUniNetMsg *pDestMsg) {
	//return CRtcToSip::instance()->msgMap(pSrcMsg, pDestMsg, m_caller,
	//	m_isReCall);
	return false;
}

bool CR2SCallModule::checkSipUserAvailable(TUniNetMsg * msg) {
	if (m_accessMode == 1 || m_accessMode == 2) {
		PTRtcCtrlMsg pRtcCtrl = (PTRtcCtrlMsg) msg->ctrlMsgHdr;
		return selectSipUser(pRtcCtrl->from.c_str());
	} else {
		return true;
	}

}

//void sendAnswerToWeb(TUniNetMsg *msg);
//void sendErrorToWeb(const int errorType);
//void forwardErrorToWeb(TUniNetMsg * msg);
//void sendShutDownToWeb();
//void sendNotifyToWeb(TUniNetMsg *msg);
//bool isSdpConfirmed(TUniNetMsg *msg);
//void sendReqToBear_Rtc(TUniNetMsg * msg);
//void sendCloseToBear_Rtc();

void CR2SCallModule::sendAnswerToWeb(TUniNetMsg * msg) {
	PTIntResponse pResp = (PTIntResponse) msg->msgBody;

	PTRtcAnswer pAnswer = new TRtcAnswer();
	pAnswer->seq = m_seq;
	pAnswer->moreComing = false;


	if(m_isImsVideoValid == false){
		LOG4CXX_DEBUG(mLogger.getLogger(), "sendAnswerToWeb:: set video = 0 in XMS return sdp");
		string str = pResp->body.c_str();
		//str.append("");
		int pos;
		if((pos = m_videoSdp.find("m=video")) != string::npos){
			pos += 8;
			unsigned int pos2 = m_videoSdp.find(" ", pos);
			m_videoSdp.erase(pos, pos2 - pos);
			m_videoSdp.insert(pos, "0");
		}
		str += m_videoSdp;
		pAnswer->sdp = str.c_str();
		m_isImsVideoValid = true;
	}
	else{
		pAnswer->sdp = pResp->body;
	}

	//tell dispatcher record map of the addr
	sendToDispatcher(RTC_ANSWER, RTC_TYPE, DIALOG_BEGIN, m_rtcCtrlMsg->clone(),
			pAnswer);
}

void CR2SCallModule::sendErrorToWeb(int errorType) {

	TRtcError * pError = new TRtcError();
	pError->seq = m_seq;
	pError->errorType = errorType;

	sendToDispatcher(RTC_ERROR, RTC_TYPE, DIALOG_CONTINUE,
			m_rtcCtrlMsg->clone(), pError);

}

void CR2SCallModule::forwardErrorToWeb(TUniNetMsg * msg) {
	sendToDispatcher(RTC_ERROR, RTC_TYPE, DIALOG_CONTINUE, msg->ctrlMsgHdr,
			msg->msgBody);
}

void CR2SCallModule::sendShutdownToWeb() {

	TRtcShutdown * pShutdown = new TRtcShutdown();
	pShutdown->seq = m_seq;

	sendToDispatcher(RTC_SHUTDOWN, RTC_TYPE, DIALOG_CONTINUE,
			m_rtcCtrlMsg->clone(), pShutdown);
}

void CR2SCallModule::sendNotifyToWeb(TUniNetMsg * msg) {
	PTIntRequest pReq = (PTIntRequest) msg->msgBody;

	PTRtcNotify pNotify = new TRtcNotify();
	pNotify->seq = m_seq;
	pNotify->content = pReq->body;
	pNotify->content_length = pNotify->content.length();

	sendToDispatcher(RTC_NOTIFY, RTC_TYPE, DIALOG_CONTINUE,
			m_rtcCtrlMsg->clone(), pNotify);
}

bool CR2SCallModule::isSdpConfirmed() {
	return m_isSdpConfirmed;
}

void CR2SCallModule::sendReqToBear_Rtc() {
	if (m_intCtrlMsg_Rtc == NULL) {
		m_intCtrlMsg_Rtc = new TIntCtrlMsg();
		m_intCtrlMsg_Rtc->from = "rtc_call";
		m_intCtrlMsg_Rtc->to = "bear";
		CHAR buf[33];
		CSipMsgHelper::generateRandomNumberStr(buf);
		string str = buf;
		str += buf;
		m_intCtrlMsg_Rtc->sessionId = str.c_str();
		m_intCtrlMsg_Rtc->intType = INT_REQUEST;
	}

	PTIntRequest pReq = new TIntRequest();
	pReq->body = m_webSdp.c_str();

	sendToDispatcher(INT_REQUEST, INT_TYPE, DIALOG_BEGIN,
			m_intCtrlMsg_Rtc->clone(), pReq);
}

void CR2SCallModule::sendCloseToBear_Rtc() {
	if (m_intCtrlMsg_Rtc != NULL) {
		PTIntClose pClose = new TIntClose();
		sendToDispatcher(INT_CLOSE, INT_TYPE, DIALOG_CONTINUE,
				m_intCtrlMsg_Rtc->clone(), pClose);
	}
}

void CR2SCallModule::sendJoinToBear_Rtc() {
	if (m_intCtrlMsg_Rtc != NULL) {
		PTIntJoin pJoin = new TIntJoin();
		pJoin->connId1 = m_imsConnId;
		pJoin->connId2 = m_webConnId;
		sendToDispatcher(INT_JOIN, INT_TYPE, DIALOG_CONTINUE,
				m_intCtrlMsg_Rtc->clone(), pJoin);
	}
}

string CR2SCallModule::getUserName(const string& user) {

	int i = user.find('@');
	if (i != -1) {
		//return user.substr(0, i).c_str();
		return user.substr(0, i);
		//return tmp.c_str();
	} else
		return user.c_str();
}

string CR2SCallModule::getHost(const string& user) {
	int i = user.find('@');
	if (i != -1) {
		//return user.substr(i + 1).c_str();
		return user.substr(i + 1);
	} else
		return "";
}

void CR2SCallModule::sendNoSdpInviteToIMS() {
	if (m_sipCtrlMsg == NULL) {
		m_sipCtrlMsg = new TSipCtrlMsg();
		string str = m_rtcCtrlMsg->offerSessionId.c_str();
		str += str;
		m_sipCtrlMsg->sip_callId.number = str.c_str();

		if (m_accessMode == 1 || m_accessMode == 2) {
			string userName = getUserName(m_sipName);
			string host = getHost(m_sipName);
			m_sipCtrlMsg->from.displayname = userName.c_str();
			//printf("userName: %s, host %s\n", userName, host);

			if (!host.empty()) {
				m_sipCtrlMsg->from.url = CSipMsgHelper::createSipURI("sip",
						userName, host, string(""));
			} else {
				m_sipCtrlMsg->from.url = CSipMsgHelper::createSipURI("tel",
						userName, string(""), string(""));
			}

			//printf("sipName: %s, host %s, %s\n", m_sipName.c_str(), getHost(m_sipName), m_sipCtrlMsg->from.url.host.c_str());
		} else {
			string from = m_rtcCtrlMsg->to.c_str();
			m_sipCtrlMsg->from.displayname = getUserName(from).c_str();
			m_sipCtrlMsg->from.url = CSipMsgHelper::createSipURI("sip",
					getUserName(from), getHost(from), string(""));
		}

		m_sipCtrlMsg->from.tag = m_rtcCtrlMsg->offerSessionId;

		string toStr = m_rtcCtrlMsg->from.c_str();
		m_sipCtrlMsg->to.displayname = getUserName(toStr).c_str();
		m_sipCtrlMsg->to.url = CSipMsgHelper::createSipURI("tel", getUserName(
				toStr), string(""), string(""));//tel后不能有域名
	}

	//协议栈自动添加
	//	char str[64];
	//	sprintf(str, "%d", pRtcOffer->seq);
	//	m_sipCtrlMsg->cseq_number = str;
	//	m_sipCtrlMsg->cseq_method = "INVITE";

	PTSipInvite pInvite = new TSipInvite();
	pInvite->req_uri = m_sipCtrlMsg->to.url;

	sendToDispatcher(SIP_INVITE, SIP_TYPE, DIALOG_BEGIN, m_sipCtrlMsg->clone(),
			pInvite);
}
//	void sendNoSdpInviteToIMS();
//	void sendCancelToIMS();
//	void sendPrackToIMS(TUniNetMsg * msg);
//	void sendAckToIMS(TUniNetMsg * msg);
//	void send200OKForUpdateToIMS(TUniNetMsg * msg);
//	void send200OKForInviteToIMS(TUniNetMsg * msg);
//	void sendByeToIMS();


void CR2SCallModule::sendCancelToIMS() {
	PTSipCancel pCancel = new TSipCancel();
	pCancel->req_uri = m_sipCtrlMsg->to.url;

	sendToDispatcher(SIP_CANCEL, SIP_TYPE, DIALOG_CONTINUE,
			m_sipCtrlMsg->clone(), pCancel);
}

void CR2SCallModule::sendPrackToIMS(TUniNetMsg * msg) {
	PTSipPrack pPrack = new TSipPrack();
	pPrack->req_uri = m_sipCtrlMsg->to.url;

	if (msg != NULL) {
		PTIntResponse pResp = (PTIntResponse) msg->msgBody;
		pPrack->content_type.type = "application";
		pPrack->content_type.subtype = "sdp";
		pPrack->body.content = pResp->body;
		pPrack->body.content_length = pResp->body.length();
	}

	sendToDispatcher(SIP_PRACK, SIP_TYPE, DIALOG_CONTINUE,
			m_sipCtrlMsg->clone(), pPrack);
}

void CR2SCallModule::sendAckToIMS(TUniNetMsg * msg) {
	PTSipReq pAck = new TSipReq();
	pAck->req_uri = m_sipCtrlMsg->to.url;

	if (msg != NULL) {
		PTIntResponse pResp = (PTIntResponse) msg->msgBody;
		pAck->content_type.type = "application";
		pAck->content_type.subtype = "sdp";
		pAck->body.content = pResp->body;
		pAck->body.content_length = pResp->body.length();
	}

	sendToDispatcher(SIP_ACK, SIP_TYPE, DIALOG_CONTINUE, m_sipCtrlMsg->clone(),
			pAck);
}

void CR2SCallModule::send200OKForUpdateToIMS(TUniNetMsg * msg) {
	PTSipResp pAns = new TSipResp();
	pAns->statusCode = 200;
	pAns->reason_phase = "OK";

	PTSipCtrlMsg ctrlMsg =NULL;
	if (msg->msgType == INT_TYPE) {
		PTIntResponse pResp = (PTIntResponse) msg->msgBody;
		pAns->content_type.type = "application";
		pAns->content_type.subtype = "sdp";
		pAns->body.content = pResp->body;
		pAns->body.content_length = pResp->body.length();
		ctrlMsg = (PTSipCtrlMsg) m_sipCtrlMsg->clone();
		ctrlMsg->cseq_method = "UPDATE";
		if(m_sessionFlag == true){
			ctrlMsg->via = m_via;
			m_sessionFlag = false;
		}
	}
	else{
		ctrlMsg = (PTSipCtrlMsg) msg->ctrlMsgHdr->clone();
	}
	//ctrlMsg->cseq_method = "UPDATE";
	//ctrlMsg->via.branch = ((PTSipCtrlMsg)msg->ctrlMsgHdr)->clone();

	sendToDispatcher(SIP_RESPONSE, SIP_TYPE, DIALOG_CONTINUE,
			ctrlMsg->clone(), pAns);
}

void CR2SCallModule::getSessionUpdateVia(TUniNetMsg * msg){
	m_sessionFlag = true;
	m_via = ((PTSipCtrlMsg)msg->ctrlMsgHdr)->via;
}

void CR2SCallModule::send200OKForInviteToIMS(TUniNetMsg * msg) {
	PTSipResp pAns = new TSipResp();
	pAns->statusCode = 200;
	pAns->reason_phase = "OK";
	PTSipCtrlMsg ctrlMsg = NULL;
	if (msg->msgType == INT_TYPE) {
		PTIntResponse pResp = (PTIntResponse) msg->msgBody;
		pAns->content_type.type = "application";
		pAns->content_type.subtype = "sdp";
		pAns->body.content = pResp->body;
		pAns->body.content_length = pResp->body.length();
		ctrlMsg = (PTSipCtrlMsg)m_sipCtrlMsg->clone();
		ctrlMsg->cseq_method = "UPDATE";
		if(m_sessionFlag == true){
			ctrlMsg->via = m_via;
			m_sessionFlag = false;
		}
	}
	else{
		ctrlMsg = (PTSipCtrlMsg) msg->ctrlMsgHdr->clone();
		ctrlMsg->cseq_method = "INVITE";
	}


	sendToDispatcher(SIP_RESPONSE, SIP_TYPE, DIALOG_CONTINUE,
			m_sipCtrlMsg->clone(), pAns);
}

void CR2SCallModule::sendByeToIMS() {
	PTSipBye pBye = new TSipBye();
	pBye->req_uri = m_sipCtrlMsg->to.url;

	sendToDispatcher(SIP_BYE, SIP_TYPE, DIALOG_CONTINUE, m_sipCtrlMsg->clone(),
			pBye);
}

//void sendReqToBear_Sip();
//void sendCloseToBear_Sip();
void CR2SCallModule::sendReqToBear_Sip() {
	if (m_intCtrlMsg_Sip == NULL) {
		m_intCtrlMsg_Sip = new TIntCtrlMsg();
		m_intCtrlMsg_Sip->from = "sip_call";
		m_intCtrlMsg_Sip->to = "bear";
		CHAR buf[33];
		CSipMsgHelper::generateRandomNumberStr(buf);
		string str = buf;
		str += buf;

		m_intCtrlMsg_Sip->sessionId = str.c_str();
		m_intCtrlMsg_Sip->intType = INT_REQUEST;
	}

	PTIntRequest pReq = new TIntRequest();
	pReq->body = m_imsSdp.c_str();

	sendToDispatcher(INT_REQUEST, INT_TYPE, DIALOG_BEGIN,
			m_intCtrlMsg_Sip->clone(), pReq);
}

void CR2SCallModule::sendCloseToBear_Sip() {
	if (m_intCtrlMsg_Sip != NULL) {
		PTIntClose pClose = new TIntClose();
		sendToDispatcher(INT_CLOSE, INT_TYPE, DIALOG_CONTINUE,
				m_intCtrlMsg_Sip->clone(), pClose);
	}
}

bool CR2SCallModule::isResp1xx(TUniNetMsg * msg) {
	if (msg->msgType != SIP_TYPE && msg->msgName != SIP_RESPONSE)
		return false;

	INT statusCode = ((PTSipResp) msg->msgBody)->statusCode;
	if (statusCode / 100 == 1)
		return true;
	else
		return false;
}

bool CR2SCallModule::isResp2xx(TUniNetMsg * msg) {
	if (msg->msgType != SIP_TYPE && msg->msgName != SIP_RESPONSE)
		return false;

	if (((PTSipResp) msg->msgBody)->statusCode == 200)
		return true;
	else
		return false;
}

bool CR2SCallModule::isResp3xx_6xx(TUniNetMsg * msg) {
	if (msg->msgType != SIP_TYPE && msg->msgName != SIP_RESPONSE)
		return false;

	INT statusCode = ((PTSipResp) msg->msgBody)->statusCode;
	if (statusCode < 700 && statusCode > 300)
		return true;
	else
		return false;
}

bool CR2SCallModule::isWithSDP(TUniNetMsg *msg) {
	if (msg->msgType != SIP_TYPE)
		return false;

	if (msg->msgName == SIP_RESPONSE) {
		PTSipResp pResp = (PTSipResp) msg->msgBody;
		return pResp->body.content_length != 0;
	}

	if (msg->msgName == SIP_UPDATE) {
		PTSipUpdate pUpdate = (PTSipUpdate) msg->msgBody;
		return pUpdate->body.content_length != 0;
	}

	if (msg->msgName == SIP_INVITE) {
		PTSipInvite pInvite = (PTSipInvite) msg->msgBody;
		return pInvite->body.content_length != 0;
	}
	return false;
}

void CR2SCallModule::notifySipTermCallSdp(TUniNetMsg * msg) {
	//onNotify(NULL);
	PTRtcOffer pOffer = (PTRtcOffer) msg->msgBody;
	m_webSdp = pOffer->sdp.c_str();

	m_sipContext.onNotify(NULL); //通知SIP端发送INVITE
}

void CR2SCallModule::notifySipTermCallClose() {
	//onClose(NULL)
	if( (m_endFlag & 0x01) != 0x01)
		m_sipContext.onClose(NULL);
}

//void notifyRtcOrigCallSdp();
//void notifyRtcOrigCallError(TUniNetMsg * msg);
//void notifyRtcOrigCallClose();
//void notifyRtcOrigCallError(const int errorType);

void CR2SCallModule::notifyRtcOrigCallSdp() {
	LOG4CXX_DEBUG(mLogger.getLogger(), "notifyRtcOrigCallSdp");

	m_rtcContext.onNotify(NULL);
}

void CR2SCallModule::notifyRtcOrigCallError(TUniNetMsg * msg) {
	if((m_endFlag & 0x02) != 0x02){
		TRtcError * pError = new TRtcError();
		pError->seq = m_seq;

		PTSipResp pSipResp = (PTSipResp) msg->msgBody;

		if (pSipResp->statusCode == 481) {
				pError->errorType = ERROR_NOMATCH;
		} else if (pSipResp->statusCode == 486 || pSipResp->statusCode == 600) {
				pError->errorType = ERROR_REFUSED;
		} else if (pSipResp->statusCode == 408) {
				pError->errorType = ERROR_TIMEOUT;
		} else if (pSipResp->statusCode == 491) {
				pError->errorType = ERROR_CONFLICT;
		} else if (pSipResp->statusCode == 404) {
				pError->errorType = ERROR_OFFLINE;
		} else if (pSipResp->statusCode < 700 && pSipResp->statusCode > 300) {
				pError->errorType = ERROR_FAILED;
		}

		PTUniNetMsg newMsg = new TUniNetMsg();
		newMsg->ctrlMsgHdr = m_rtcCtrlMsg->clone();
		newMsg->setCtrlMsgHdr();
		newMsg->msgBody = pError;
		newMsg->setMsgBody();

		m_rtcContext.onError(newMsg);

	}

}

void CR2SCallModule::notifyRtcOrigCallClose() {
	if((m_endFlag & 0x02) != 0x02){
		m_rtcContext.onClose(NULL);
	}
}

void CR2SCallModule::notifyRtcOrigCallError(const int errorType) {
	if((m_endFlag&0x02) != 0x02){
		TRtcError * pError = new TRtcError();
		pError->seq = m_seq;

		pError->errorType = errorType;

		PTUniNetMsg msg = new TUniNetMsg();
		msg->ctrlMsgHdr = m_rtcCtrlMsg;
		msg->setCtrlMsgHdr();
		msg->msgBody = pError;
		msg->setMsgBody();

		m_rtcContext.onError(msg);

	}
}

void CR2SCallModule::handleUnexpectedMsg(TUniNetMsg * msg) {
	LOG4CXX_DEBUG(mLogger.getLogger(), "CR2SCallModule Recv Unexpected Msg!")
}

bool CR2SCallModule::compAndModifySdpWithRtc(TUniNetMsg * msg) {
	m_isSdpConfirmed = true;
	if (msg->msgName == SIP_RESPONSE) {
		PTSipResp pResp = (PTSipResp) msg->msgBody;
		m_imsSdp = pResp->body.content.c_str();
	} else if (msg->msgName == SIP_UPDATE) {
		PTSipUpdate pUpdate = (PTSipUpdate) msg->msgBody;
		m_imsSdp = pUpdate->body.content.c_str();
	}


	unsigned int pos = 0, pos2 = 0;
//	while ((pos = m_imsSdp.find("m=", pos)) != string::npos) {
//		unsigned int pos2 = m_imsSdp.find(" ", pos);
//		pos += 2;
//		string mediaType = m_imsSdp.substr(pos, pos2 - pos);
//		if (m_webSdp.find("m=" + mediaType) == string::npos) {
//
//			pos = pos2 + 1;
//			pos2 = m_imsSdp.find(" ", pos);
//			m_imsSdp.erase(pos, pos2 - pos);
//			m_imsSdp.insert(pos, "0");
//		}
//	}
	if((pos = m_imsSdp.find("m=video", 0)) != string::npos && (pos2 = m_webSdp.find("m=video")) == string::npos ){
		LOG4CXX_INFO(mLogger.getLogger(), "WebRTC Do Not Support Video, set PORT 0 in IMS SDP");
		pos += 8;
		unsigned int pos2 = m_imsSdp.find(" ", pos);
		m_imsSdp.erase(pos, pos2 - pos);
		m_imsSdp.insert(pos, "0");
	}


	pos = 0;
	pos2 = 0;

	if((pos = m_webSdp.find("m=video", 0)) != string::npos && (pos2 = m_imsSdp.find("m=video")) == string::npos ){
		LOG4CXX_INFO(mLogger.getLogger(), "IMS Do Not Support Video, set PORT 0 in WebRTC SDP");
		m_videoSdp = m_webSdp.substr(pos);
		m_webSdp = m_webSdp.substr(0, pos);


//
//		pos = m_webSdp.find("a=group:BUNDLE",0);
//		pos2 = m_webSdp.find("\r\n", pos);
//		m_webSdp.erase(pos, pos2-pos);
//		m_webSdp.insert(pos, "a=group:BUNDLE audio");
		m_isImsVideoValid = false;

//		pos += 8;
//		unsigned int pos2 = m_webSdp.find(" ", pos);
//		m_webSdp.erase(pos, pos2 - pos);
//		m_webSdp.insert(pos, "0");
	}

//	while ((pos = m_webSdp.find("m=", pos)) != string::npos) {
//		unsigned pos2 = m_webSdp.find(" ", pos);
//		pos += 2;
//		string mediaType = m_webSdp.substr(pos, pos2 - pos);
//		if (m_imsSdp.find("m=" + mediaType) == string::npos) {
//			LOG4CXX_INFO(mLogger.getLogger(), "IMS Do Not Support "<<mediaType<<", set PORT 0 in WebRTC SDP");
//			pos = pos2 + 1;
//			pos2 = m_webSdp.find(" ", pos);
//			m_webSdp.erase(pos, pos2 - pos);
//			m_webSdp.insert(pos, "0");
//		}
//	}

	return true;
}
bool CR2SCallModule::compSdpWithOld(TUniNetMsg * msg) {
	//set ims rtc body
//	string imsBody;
//	if (msg->msgName == SIP_INVITE) {
//		PTSipInvite pInvite = (PTSipInvite) msg->msgBody;
//		imsBody = pInvite->body.content.c_str();
//	} else if (msg->msgName == SIP_UPDATE) {
//		PTSipUpdate pUpdate = (PTSipUpdate) msg->msgBody;
//		imsBody = pUpdate->body.content.c_str();
//	}

	return true;
}

string CR2SCallModule::checkRespCseqMothod(TUniNetMsg * msg) {
	PTSipCtrlMsg ctrlMsg = (PTSipCtrlMsg) msg->ctrlMsgHdr;

	return ctrlMsg->cseq_method.c_str();
}

void CR2SCallModule::setIMSConnId(TUniNetMsg * msg) {
	PTIntResponse pResp = (PTIntResponse) msg->msgBody;
	m_imsConnId = pResp->connId;
	m_joinFlag = m_joinFlag | 0x02;
	LOG4CXX_DEBUG(mLogger.getLogger(), "setIMSConnId: m_joinFlag="<<m_joinFlag);
	if (!m_joinSend && ((m_joinFlag & 0x03) == 0x03)) {
		sendJoinToBear_Rtc();
		m_joinSend = true;
	}
}

void CR2SCallModule::setWebConnId(TUniNetMsg * msg) {
	PTIntResponse pResp = (PTIntResponse) msg->msgBody;
	m_webConnId = pResp->connId;
	m_joinFlag = m_joinFlag | 0x01;
	LOG4CXX_DEBUG(mLogger.getLogger(), "setWebConnId: m_joinFlag="<<m_joinFlag);
	if (!m_joinSend && ((m_joinFlag & 0x03) == 0x03)) {
		sendJoinToBear_Rtc();
		m_joinSend = true;
	}
}
