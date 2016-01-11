#include "CSipCallModule.h"

CLONE_COMP(CSipCallModule)
CREATE_COMP(CSipCallModule)

CSipCallModule::CSipCallModule(PCGFSM afsm) :
	CUACSTask(afsm), m_fsmContext(*this) {

	m_parser = NULL;
	m_UACsipCtrlMsg = new TSipCtrlMsg();
	m_UASsipCtrlMsg = new TSipCtrlMsg();
	m_MSsipCtrlMsg_1 = new TSipCtrlMsg();
	m_MSsipCtrlMsg_2 = new TSipCtrlMsg();

	m_sipInviteBody = new TSipInvite();
	m_webrtcBody = new TSipBody();
	m_imsBody = new TSipBody();
	m_xmsImsBody = new TSipBody();


	isColorRingStatus = false;


	CProperties* properties = CPropertiesManager::getInstance()->getProperties(
			"rtc.env");
	int LOGADDR_DISPATCHER = properties->getIntProperty("LOGADDR_DISPATCHER");
	if (LOGADDR_DISPATCHER == -1) {
		print(
				"LOGADDR_DISPATCHER not correctly set in rtc.env,Use DEFAULT 231.\n");
		this->LOGADDR_DISPATCHER = 231;
	} else {
		this->LOGADDR_DISPATCHER = LOGADDR_DISPATCHER;
	}
	//m_SipCtrlMsg = new TSipCtrlMsg();
	properties = CPropertiesManager::getInstance()->getProperties(
			"gateway.env");
	string MS_NAME = properties->getProperty("MS_NAME");
	if (MS_NAME.empty()) {
		print("MS_NAME not correctly set in gateway.env,Use DEFAULT.\n");
		this->MS_NAME = "msml";
	} else {
		this->MS_NAME = MS_NAME;
	}

	string MS_IP = properties->getProperty("MS_IP");
	if (MS_IP.empty()) {
		print("MS_IP not correctly set in gateway.env,Use DEFAULT.\n");
		this->MS_IP = "10.109.247.143";
	} else {
		this->MS_IP = MS_IP;
	}

	string MS_PORT = properties->getProperty("MS_PORT");
	if (MS_PORT.empty()) {
		print("MS_PORT not correctly set in gateway.env,Use DEFAULT.\n");
		this->MS_PORT = "5060";
	} else {
		this->MS_PORT = MS_PORT;
	}

	string HOST_NAME = properties->getProperty("HOST_NAME");
	if (HOST_NAME.empty()) {
		print("HOST_NAME not correctly set in gateway.env,Use DEFAULT.\n");
		this->HOST_NAME = "1";
	} else {
		this->HOST_NAME = HOST_NAME;
	}

	string HOST_IP = properties->getProperty("HOST_IP");
	if (HOST_IP.empty()) {
		print("HOST_IP not correctly set in gateway.env,Use DEFAULT.\n");
		this->HOST_IP = "10.109.247.143";
	} else {
		this->HOST_IP = HOST_IP;
	}

	string HOST_PORT = properties->getProperty("HOST_PORT");
	if (HOST_PORT.empty()) {
		print("HOST_PORT not correctly set in gateway.env,Use DEFAULT.\n");
		this->HOST_PORT = "5060";
	} else {
		this->HOST_PORT = HOST_PORT;
	}

	m_isReCall = false;
}

CSipCallModule::~CSipCallModule() {
	if (m_UACsipCtrlMsg) {
		delete m_UACsipCtrlMsg;
		m_UACsipCtrlMsg = NULL;
	}

	if (m_UASsipCtrlMsg) {
		delete m_UASsipCtrlMsg;
		m_UASsipCtrlMsg = NULL;
	}

	if (m_MSsipCtrlMsg_1) {
		delete m_MSsipCtrlMsg_1;
		m_MSsipCtrlMsg_1 = NULL;
	}

	if (m_MSsipCtrlMsg_2) {
		delete m_MSsipCtrlMsg_2;
		m_MSsipCtrlMsg_2 = NULL;
	}

	if (m_webrtcBody) {
		delete m_webrtcBody;
		m_webrtcBody = NULL;
	}
	if (m_imsBody) {
		delete m_imsBody;
		m_imsBody = NULL;
	}

	if (m_xmsImsBody) {
		delete m_xmsImsBody;
		m_xmsImsBody = NULL;
	}


	if (m_sipInviteBody) {
		delete m_sipInviteBody;
		m_sipInviteBody = NULL;
	}

	if (m_parser) {
		delete m_parser;
		m_parser = NULL;
	}

	//	if(m_SipCtrlMsg){
	//		delete m_SipCtrlMsg;
	//		m_SipCtrlMsg = NULL;
	//	}
}

PTUACData CSipCallModule::createData() {
	return new TUACData();
}

void CSipCallModule::initState() {
	m_fsmContext.enterStartState();
}



bool CSipCallModule::isResp100_101(TUniNetMsg* msg) {
	//如果该消息不是sip的response消息
	if (msg->msgType != SIP_TYPE && msg->msgName != SIP_RESPONSE)
		return false;

	//获得响应消息的状态码
	INT statusCode = ((PTSipResp) msg->msgBody)->statusCode;
	if (100 == statusCode || 101 == statusCode)
		return true;
	else
		return false;
}

bool CSipCallModule::isRespWithSdp(TUniNetMsg* msg){
	//判断是否是IMS返回的彩铃的sdp
	printf("%d %d\n", msg->msgType, SIP_TYPE);	
	if(msg->msgType != SIP_TYPE && msg->msgName != SIP_RESPONSE){
		return false;
	}

	printf("content\n");
	if(((PTSipResp) msg->msgBody)->body.content_length != 0){
		m_UACsipCtrlMsg->via = ((PTSipCtrlMsg) msg->ctrlMsgHdr)->via;
		isColorRingStatus = true;
		return true;
	}

	return false;


}


bool CSipCallModule::isResp1xx(TUniNetMsg* msg) {
	//如果该消息不是sip的response消息
	if (msg->msgType != SIP_TYPE && msg->msgName != SIP_RESPONSE)
		return false;

	//获得响应消息的状态码
	INT statusCode = ((PTSipResp) msg->msgBody)->statusCode;
	if (statusCode / 100 == 1)
		return true;
	else
		return false;
}

bool CSipCallModule::isResp2xx(TUniNetMsg* msg) {
	//如果该消息不是sip的response消息
	if (msg->msgType != SIP_TYPE && msg->msgName != SIP_RESPONSE)
		return false;

	if (((PTSipResp) msg->msgBody)->statusCode == 200)
		return true;
	else
		return false;
}

bool CSipCallModule::isResp3xx_6xx(TUniNetMsg* msg) {
	//如果该消息不是sip的response消息
	if (msg->msgType != SIP_TYPE && msg->msgName != SIP_RESPONSE)
		return false;

	//获得响应消息的状态码
	INT statusCode = ((PTSipResp) msg->msgBody)->statusCode;
	if (statusCode < 700 && statusCode > 300)
		return true;
	else
		return false;
}

bool CSipCallModule::isConfReINVITE(TUniNetMsg *pMsg) {
	if (pMsg->msgType != SIP_TYPE && pMsg->msgName != SIP_INVITE)
		return false;

	//TSipInvite* pSipInvite = (TSipInvite*) (pMsg->msgBody);

	//	if(pSipInvite->body.content.length() == 0 && m_SipCtrlMsg->via.branch.size() > 0){
	//		//由会议服务器发出的Re-Invite，sdp为空，会议服务器发出的消息via的长度大于0
	//		m_isReCall = true;
	//		return true;
	//	}
	//	else{
	return false;
	//}

}

/** 在这里填充dispatcher的地址
 *  如果DIALOG_END，则通知dispatcher，消除会话信息
 *  如果消息最终发到sip（type = sip）那么直接转发，否则需要进行转换再发到dispatcher。见文档
 */
void CSipCallModule::sendToDispatcher(TUniNetMsg *pMsg) {
	// 也许该记录下dispatcher的地址
	pMsg->tAddr.phyAddr = 0;
	pMsg->tAddr.taskInstID = 0;
	pMsg->tAddr.logAddr = LOGADDR_DISPATCHER;
	if (pMsg->dialogType == DIALOG_END) {
		sendMsg(pMsg);
		return;
	}

	if (pMsg->msgType == SIP_TYPE) {
		// just copy the message
		TUniNetMsg *pCopyMsg = (TUniNetMsg *) pMsg->cloneMsg();
		sendMsg(pCopyMsg);
		return;
	}

	if (pMsg->msgType == RTC_TYPE) {
		TUniNetMsg *pNewMsg = new TUniNetMsg();
		msgMap(pMsg, pNewMsg);
		sendMsg(pNewMsg);
	}
}

/*
 * 结束状态机
 * 需要向Dispatcher模块发送信息,通知这个处理状态机实例的结束
 * 删除Dispatcher模块存储的会话状态信息
 */
void CSipCallModule::endTask() {
	// 产生一条DIALOG——END消息，发给dispatcher，清除会话信息
	printf("end task UAC\n");
	TUniNetMsg *pMsg = new TUniNetMsg();
	pMsg->dialogType = DIALOG_END;
	pMsg->msgType = SIP_TYPE; // 无所谓，先判断dialogType
	pMsg->msgName = SIP_RESPONSE;
	pMsg->ctrlMsgHdr = m_UACsipCtrlMsg->clone();
	pMsg->setCtrlMsgHdr();

	sendToDispatcher(pMsg);

	printf("end task MS_1\n");
	pMsg = NULL;
	pMsg = new TUniNetMsg();
	pMsg->dialogType = DIALOG_END;
	pMsg->msgType = SIP_TYPE; // 无所谓，先判断dialogType
	pMsg->msgName = SIP_RESPONSE;
	pMsg->msgName = SIP_RESPONSE;
	pMsg->ctrlMsgHdr = m_MSsipCtrlMsg_1->clone();
	pMsg->setCtrlMsgHdr();
	sendToDispatcher(pMsg);

	pMsg = NULL;
	pMsg = new TUniNetMsg();
	pMsg->dialogType = DIALOG_END;
	pMsg->msgType = SIP_TYPE; // 无所谓，先判断dialogType
	pMsg->msgName = SIP_RESPONSE;
	printf("end task MS_2\n");
	pMsg->ctrlMsgHdr = m_MSsipCtrlMsg_2->clone();
	pMsg->setCtrlMsgHdr();
	sendToDispatcher(pMsg);

	end();
	printf("endTask finish");
}
void CSipCallModule::handleTimeoutAtCallProcState() {
	if (m_UACsipCtrlMsg->via.branch.size() < 1) // 对于webrtc发起的呼叫是没有via字段的
		generateCancel();
	else
		generateAndSendTimeOutMsg();
}
//产生超时消息，使用场景：sipCSipCallModule::发出invite，超时
void CSipCallModule::generateAndSendTimeOutMsg() {
	printf("CSipCallModule::generateAndSendTimeOutMsg\n");
	TUniNetMsg *pMsg = new TUniNetMsg();
	pMsg->dialogType = DIALOG_CONTINUE;
	pMsg->msgType = SIP_TYPE;
	pMsg->msgName = SIP_RESPONSE;

	PTSipResp response = new TSipResp();
	response->statusCode = 408;
	response->reason_phase = "Request Timeout";
	pMsg->msgBody = response;
	pMsg->setMsgBody();

	pMsg->ctrlMsgHdr = m_UACsipCtrlMsg->clone();
	pMsg->setCtrlMsgHdr();

	sendToDispatcher(pMsg);
}

BOOL CSipCallModule::msgMap(TUniNetMsg *pSrcMsg, TUniNetMsg *pDestMsg) {
	CSipToRtc::instance()->msgMap(pSrcMsg, pDestMsg, m_caller, m_callerHost,
			m_isReCall);//change by guoxun
	return TRUE;
}

//处理消息
void CSipCallModule::procMsg(PTUniNetMsg msg) {
	printf("procMsg\n");
	CTUniNetMsgHelper::print(msg);
	
	printf("current state: %s %d\n", m_fsmContext.getState().getName(),
			m_fsmContext.getState().getId());
	printf("recv msg from %d, msgName is %s\n", msg->oAddr.logAddr,
			msg->getMsgNameStr());
	printf("info %d, update %d\n", SIP_INFO, SIP_UPDATE);
	printf("msgName %d\n", msg->msgName);
	switch (msg->msgName) {
	case SIP_INVITE:
		*m_UACsipCtrlMsg = *((PTSipCtrlMsg) msg->ctrlMsgHdr);
		*m_sipInviteBody = *((PTSipInvite) msg->msgBody);
		*m_webrtcBody = m_sipInviteBody->body;

		m_caller = m_UACsipCtrlMsg->from.url.username;
		m_callerHost = m_UACsipCtrlMsg->from.url.host;//add by guoxun
		recordVia(msg);

		m_fsmContext.onInvite(msg);
		break;
	case SIP_RESPONSE:
		if (msg->msgType == SIP_TYPE) // webrtc回的answer
			attachRecordedViaToMsg(msg);
		m_fsmContext.onResponse(msg);
		break;
	case SIP_ACK:
		printf("CSipCallModule: has got ack\n");
		m_fsmContext.onAck(msg);
		break;
	case SIP_CANCEL:
		m_fsmContext.onCancel(msg);
		break;
	case SIP_BYE:
		m_fsmContext.onBye(msg);
		//		sendBack200OK(pCloneMsg);
		break;
	case SIP_INFO:
		m_fsmContext.onInfo(msg);
		break;
	case SIP_UPDATE:
		m_fsmContext.onUpdate(msg);
		break;
	default:
		printf("###CSipCallModule:unknow msgName %s\n", msg->getMsgNameStr());
		//收到非法消息,忽略.等待超时.
		//endTask();
		break;
	}
	printf("after procMsg state: %s %d\n", m_fsmContext.getState().getName(),
			m_fsmContext.getState().getId());
	//	delete pCloneMsg;
}

void CSipCallModule::forwardNoSdpInviteToIMS() {
	PTSipInvite pInvite = new TSipInvite();
	pInvite->req_uri = m_UACsipCtrlMsg->to.url;

	//	PTUniNetMsg newMsg = new TUniNetMsg();
	//	newMsg->tAddr.phyAddr = 0;
	//	newMsg->tAddr.logAddr = LOGADDR_DISPATCHER;
	//	newMsg->msgType = SIP_TYPE;
	//	newMsg->msgName = SIP_INVITE;
	//	newMsg->dialogType = DIALOG_BEGIN;
	//	newMsg->ctrlMsgHdr = m_UACsipCtrlMsg->clone();
	//	newMsg->setCtrlMsgHdr();
	//
	//	newMsg->msgBody = pInvite;
	//	newMsg->setMsgBody();

	sendMessageToSip(SIP_INVITE, DIALOG_BEGIN, m_UACsipCtrlMsg->clone(),
			pInvite);
}

void CSipCallModule::forwardCancelToIMS(){
	TSipCancel * pSipCancel = new TSipCancel();
		pSipCancel->req_uri = m_UACsipCtrlMsg->to.url;

		sendMessageToSip(SIP_CANCEL, DIALOG_CONTINUE, m_MSsipCtrlMsg_1->clone(),
				pSipCancel);
}





void CSipCallModule::setUACTag(TUniNetMsg * msg) {
	m_UACsipCtrlMsg->to.tag = ((PTSipCtrlMsg) msg->ctrlMsgHdr)->to.tag;
}

void CSipCallModule::setConnId_Web(TUniNetMsg * msg) {
	//	string userName = string(m_UACsipCtrlMsg->from.url.username.c_str());
	//	size_t pos = userName.find('-', 0);
	//	if(pos != string::npos){
	//		userName = userName.substr(0, pos);
	//	}
	m_MSsipCtrlMsg_1->to.tag = ((PTSipCtrlMsg) msg->ctrlMsgHdr)->to.tag;
	m_connId_Web = string("conn:") + m_MSsipCtrlMsg_1->to.tag.c_str();
}

void CSipCallModule::setConnId_IMS(TUniNetMsg * msg) {
	m_MSsipCtrlMsg_2->to.tag = ((PTSipCtrlMsg) msg->ctrlMsgHdr)->to.tag;
	m_connId_IMS = string("conn:") + m_MSsipCtrlMsg_2->to.tag.c_str();
	TSipResp* pSipResp = (TSipResp*) (msg->msgBody);
	*(m_xmsImsBody) = pSipResp->body;

}

void CSipCallModule::forwardJoinInfoToMS() {
	printf("m_connId_IMS: %s\n, m_connId_Web:%s\n", m_connId_IMS.c_str(), m_connId_Web.c_str());
	string infoStr;
	if(m_parser == NULL){
		m_parser = new CMessageXmlParse();
		infoStr = m_parser->createConfJoinXml(m_connId_IMS, m_connId_Web);
		m_parser = NULL;
	}
	PTSipInfo info = new TSipInfo();
	info->body.content = infoStr.c_str();
	info->body.content_length = (UINT) infoStr.length();

	info->content_type.type = "application";
	info->content_type.subtype = "xml";
	info->req_uri = m_MSsipCtrlMsg_1->to.url;

	sendMessageToSip(SIP_INFO, DIALOG_CONTINUE, m_MSsipCtrlMsg_1->clone(), info);

}

void CSipCallModule::forwardInviteToMS_Web() {
	m_MSsipCtrlMsg_1->from.url.scheme = "sip";
	m_MSsipCtrlMsg_1->from.url.username = HOST_NAME.c_str();
	m_MSsipCtrlMsg_1->from.url.host = HOST_IP.c_str();
	m_MSsipCtrlMsg_1->from.url.port = HOST_PORT.c_str();

	CHAR buf[33];
	CSipMsgHelper::generateRandomNumberStr(buf);
	m_MSsipCtrlMsg_1->from.tag = buf;

	m_MSsipCtrlMsg_1->to.url.scheme = "sip";
	m_MSsipCtrlMsg_1->to.url.username = MS_NAME.c_str();
	m_MSsipCtrlMsg_1->to.url.host = MS_IP.c_str();
	m_MSsipCtrlMsg_1->to.url.port = MS_PORT.c_str();

	m_MSsipCtrlMsg_1->cseq_method = m_UACsipCtrlMsg->cseq_method;
	m_MSsipCtrlMsg_1->cseq_number = m_UACsipCtrlMsg->cseq_number;

	string str = buf;
	str += buf;
	m_MSsipCtrlMsg_1->sip_callId.number = str.c_str();

	PTSipInvite pInvite = new TSipInvite();
	pInvite->req_uri = m_MSsipCtrlMsg_1->to.url;

	pInvite->content_type.type = "application";
	pInvite->content_type.subtype = "sdp";

	pInvite->body = *m_webrtcBody;
	sendMessageToSip(SIP_INVITE, DIALOG_BEGIN, m_MSsipCtrlMsg_1->clone(),
			pInvite);
}

void CSipCallModule::forwardInviteToMS_IMS() {
	m_MSsipCtrlMsg_2->from.url.scheme = "sip";
	m_MSsipCtrlMsg_2->from.url.username = HOST_NAME.c_str();
	m_MSsipCtrlMsg_2->from.url.host = HOST_IP.c_str();
	m_MSsipCtrlMsg_2->from.url.port = HOST_PORT.c_str();

	CHAR buf[33];
	CSipMsgHelper::generateRandomNumberStr(buf);
	m_MSsipCtrlMsg_2->from.tag = buf;
	m_MSsipCtrlMsg_2->to.url.scheme = "sip";
	m_MSsipCtrlMsg_2->to.url.username = MS_NAME.c_str();
	m_MSsipCtrlMsg_2->to.url.host = MS_IP.c_str();
	m_MSsipCtrlMsg_2->to.url.port = MS_PORT.c_str();

	m_MSsipCtrlMsg_2->cseq_method = m_UACsipCtrlMsg->cseq_method;
	m_MSsipCtrlMsg_2->cseq_number = m_UACsipCtrlMsg->cseq_number;
	string str = buf;
	str += buf;
	m_MSsipCtrlMsg_2->sip_callId.number = str.c_str();

	PTSipInvite pInvite = new TSipInvite();
	pInvite->req_uri = m_MSsipCtrlMsg_2->to.url;

	pInvite->content_type.type = "application";
	pInvite->content_type.subtype = "sdp";

	pInvite->body = *m_imsBody;
	sendMessageToSip(SIP_INVITE, DIALOG_BEGIN, m_MSsipCtrlMsg_2->clone(),
			pInvite);
}

void CSipCallModule::forwardReinviteToMS_IMS(TUniNetMsg * msg){
	PTSipInvite pInvite = new TSipInvite();
	pInvite->content_type.type = "application";
	pInvite->content_type.subtype = "sdp";

	if(msg->msgName == SIP_RESPONSE){
		pInvite->body = ((PTSipResp) msg->msgBody)->body;
	}
	else{
		pInvite->body = ((PTSipUpdate) msg->msgBody)->body;
		*m_UASsipCtrlMsg = *((PTSipCtrlMsg) msg->ctrlMsgHdr);
	}

	pInvite->req_uri = m_MSsipCtrlMsg_2->to.url;

	sendMessageToSip(SIP_INVITE, DIALOG_CONTINUE, m_MSsipCtrlMsg_2->clone(),
			pInvite);

}

void CSipCallModule::forwardACKToIMS(TUniNetMsg * msg) {
	PTSipReq pReq = new TSipReq();
	pReq->content_type.type = "application";
	pReq->content_type.subtype = "sdp";

	pReq->body = ((PTSipResp) msg->msgBody)->body;
	pReq->req_uri = m_UACsipCtrlMsg->to.url;
	printf("ACK req_uri %s %s\n", pReq->req_uri.username.c_str(), pReq->req_uri.scheme.c_str());

	sendMessageToSip(SIP_ACK, DIALOG_CONTINUE, m_UACsipCtrlMsg->clone(), pReq);
}



void CSipCallModule::forwardPrackToIMS(TUniNetMsg * msg) {
	printf("send PRACK with XMS-IMS sdp to IMS\n");
	PTSipReq pReq = new TSipReq();
	pReq->content_type.type = "application";
	pReq->content_type.subtype = "sdp";

	
	pReq->body = *m_xmsImsBody;

	pReq->req_uri = m_UACsipCtrlMsg->to.url;

	//printf("ACK req_uri %s %s\n", pReq->req_uri.username.c_str(), pReq->req_uri.scheme.c_str());

	sendMessageToSip(SIP_PRACK, DIALOG_CONTINUE, m_UACsipCtrlMsg->clone(), pReq);
}


void CSipCallModule::forward200OKToIMS(TUniNetMsg *msg){
	PTSipCtrlMsg ctrlMsg = (PTSipCtrlMsg) m_UASsipCtrlMsg->clone();
	//ctrlMsg->cseq_method = "UPDATE";

	//swap(ctrlMsg->from, ctrlMsg->to);

	PTSipResp pResp = (PTSipResp) msg->msgBody->clone();

	string s1 = pResp->body.content.c_str();
	string s2 = "191.8.2.130";
	string s3 = "183.62.12.7";

	printf("s1:%s\n  s2:%s\n %d\n", s1.c_str(), s2.c_str(), s1.find(s2));
	if(s1.find(s2) != string::npos){
		s1 = replaceSdpStr(s1, s2, s3);
		pResp->body.content = s1.c_str();
		pResp->body.content_length = (UINT)s1.length(); 
	}

	sendMessageToSip(SIP_RESPONSE, DIALOG_CONTINUE, ctrlMsg, pResp);
	

}


void CSipCallModule::forward200OKToWeb(TUniNetMsg *msg) {
//	CHAR buf[33];
//	CSipMsgHelper::generateRandomNumberStr(buf);
//	m_UACsipCtrlMsg->to.tag = buf;

	PTUniNetMsg newMsg = new TUniNetMsg();
	newMsg->tAddr.phyAddr = 0;
	newMsg->tAddr.logAddr = LOGADDR_DISPATCHER;
	newMsg->msgType = RTC_TYPE;
	newMsg->msgName = SIP_RESPONSE;
	newMsg->dialogType = DIALOG_CONTINUE;
	newMsg->ctrlMsgHdr = m_UACsipCtrlMsg->clone();
	newMsg->setCtrlMsgHdr();

	//PTSipResp pResp =(PTSipResp) msg->msgBody->clone();

	newMsg->msgBody = msg->msgBody->clone();
	newMsg->setMsgBody();

	sendToDispatcher(newMsg);
	delete newMsg;
	newMsg = NULL;
}

void CSipCallModule::forwardByeToWeb(){
	TSipBye * pSipBye = new TSipBye();
	pSipBye->req_uri = m_UACsipCtrlMsg->from.url;
	PTSipCtrlMsg ctrlMsg = (PTSipCtrlMsg)m_UACsipCtrlMsg->clone();
	swap(ctrlMsg->from, ctrlMsg->to);

	PTUniNetMsg newMsg = new TUniNetMsg();
	newMsg->tAddr.phyAddr = 0;
	newMsg->tAddr.logAddr = LOGADDR_DISPATCHER;
	newMsg->msgType = RTC_TYPE;
	newMsg->msgName = SIP_BYE;
	newMsg->dialogType = DIALOG_CONTINUE;
	newMsg->ctrlMsgHdr = ctrlMsg;
	newMsg->setCtrlMsgHdr();

	newMsg->msgBody = pSipBye;
	newMsg->setMsgBody();

	sendToDispatcher(newMsg);
	delete newMsg;
	newMsg = NULL;
}

void CSipCallModule::forwardByeToIMS(){
	TSipBye * pSipBye = new TSipBye();
	pSipBye->req_uri = m_UACsipCtrlMsg->to.url;

	sendMessageToSip(SIP_BYE, DIALOG_CONTINUE, m_UACsipCtrlMsg->clone(),
			pSipBye);

}


void CSipCallModule::forwardACKToMS_Web() {
	PTSipCtrlMsg pCtrlMsg = (PTSipCtrlMsg) m_MSsipCtrlMsg_1->clone();
	pCtrlMsg->cseq_method = "ACK";
	TSipReq * pSipReq = new TSipReq();
	pSipReq->req_uri = pCtrlMsg->to.url;

	sendMessageToSip(SIP_ACK, DIALOG_CONTINUE, pCtrlMsg, pSipReq);
}

void CSipCallModule::forwardACKToMS_IMS() {
	PTSipCtrlMsg pCtrlMsg = (PTSipCtrlMsg) m_MSsipCtrlMsg_2->clone();
	pCtrlMsg->cseq_method = "ACK";
	TSipReq * pSipReq = new TSipReq();
	pSipReq->req_uri = pCtrlMsg->to.url;

	sendMessageToSip(SIP_ACK, DIALOG_CONTINUE, pCtrlMsg, pSipReq);

}

void CSipCallModule::forwardByeToMS_Web() {
	printf("forwardByeToMS_Web\n");
	TSipBye * pSipBye = new TSipBye();
	pSipBye->req_uri = m_MSsipCtrlMsg_1->to.url;

	sendMessageToSip(SIP_BYE, DIALOG_CONTINUE, m_MSsipCtrlMsg_1->clone(),
			pSipBye);
}

void CSipCallModule::forwardByeToMS_IMS() {
	printf("forwardByeToMS_IMS\n");
	TSipBye * pSipBye = new TSipBye();
	pSipBye->req_uri = m_MSsipCtrlMsg_2->to.url;

	sendMessageToSip(SIP_BYE, DIALOG_CONTINUE, m_MSsipCtrlMsg_2->clone(),
			pSipBye);

}

void CSipCallModule::forwardCancelToMS_Web() {
	TSipCancel * pSipCancel = new TSipCancel();
	pSipCancel->req_uri = m_MSsipCtrlMsg_1->to.url;

	sendMessageToSip(SIP_CANCEL, DIALOG_CONTINUE, m_MSsipCtrlMsg_1->clone(),
			pSipCancel);
}

void CSipCallModule::forwardCancelToMS_IMS() {
	TSipCancel * pSipCancel = new TSipCancel();
	pSipCancel->req_uri = m_MSsipCtrlMsg_2->to.url;

	sendMessageToSip(SIP_CANCEL, DIALOG_CONTINUE, m_MSsipCtrlMsg_2->clone(),
			pSipCancel);

}

bool CSipCallModule::checkColorRing(){
	return isColorRingStatus;
}


void CSipCallModule::setIMSbody(TUniNetMsg * pMsg) {
	if (pMsg->msgName != SIP_RESPONSE)
		return;
	m_UACsipCtrlMsg->to.tag = ((PTSipCtrlMsg)pMsg->ctrlMsgHdr)->to.tag;
	TSipResp* pSipResp = (TSipResp*) (pMsg->msgBody);
	*m_imsBody = pSipResp->body;
}

//处理定时器超时
void CSipCallModule::onTimeOut(TTimeMarkExt timerMark) {
	print("The CSipCallModule task received a timeout event: %d!!!\n",
			timerMark.timerId);
	errorLog(
			"[CSipCallModule]ERROR: The CSipCallModule task received a timeout event: %d!!!\n",
			timerMark.timerId);
	m_fsmContext.onTimeOut(timerMark);
}

// add(1 function) by zhangyadong on 2014.7.14
void CSipCallModule::sendBackACK(TUniNetMsg *msg) {
	printf("******send Back ACK\n");
	TUniNetMsg *pNewMsg = new TUniNetMsg();
	pNewMsg->dialogType = DIALOG_CONTINUE;
	pNewMsg->msgType = SIP_TYPE;
	pNewMsg->msgName = SIP_ACK;

	PTSipCtrlMsg pCtrlMsg = (PTSipCtrlMsg) msg->cloneCtrlMsg();
	pCtrlMsg->cseq_method = "ACK";
	pNewMsg->ctrlMsgHdr = pCtrlMsg;
	pNewMsg->setCtrlMsgHdr();
	TSipReq * pSipReq = new TSipReq();
	pSipReq->req_uri = ((PTSipCtrlMsg) pNewMsg->ctrlMsgHdr)->to.url;
	pNewMsg->msgBody = pSipReq;
	pNewMsg->setMsgBody();

	sendToDispatcher(pNewMsg);
	printf("*******send Back ACK END\n");
}

void CSipCallModule::sendByeToIMS() {
	printf("*******send bye to IMS\n");
	TUniNetMsg *pMsg = new TUniNetMsg();
	pMsg->dialogType = DIALOG_CONTINUE;
	pMsg->msgType = SIP_TYPE;
	pMsg->msgName = SIP_BYE;

	TSipCtrlMsg *pSipCtrl = new TSipCtrlMsg();
	
	if (m_UACsipCtrlMsg->via.branch.size() > 1) {
		
		pSipCtrl->from = m_UACsipCtrlMsg->to;
		pSipCtrl->to = m_UACsipCtrlMsg->from;
	} else {
		pSipCtrl->from = m_UACsipCtrlMsg->from;
		pSipCtrl->to = m_UACsipCtrlMsg->to;
	}
	pSipCtrl->sip_callId = m_UACsipCtrlMsg->sip_callId;
	pSipCtrl->cseq_method = "BYE";
	pMsg->ctrlMsgHdr = pSipCtrl;
	pMsg->setCtrlMsgHdr();

	TSipBye * pBye = new TSipBye();

	pBye->req_uri = pSipCtrl->to.url;

	pMsg->msgBody = pBye;
	pMsg->setMsgBody();

	sendToDispatcher(pMsg);

	delete pMsg;

}

void CSipCallModule::sendByeToWeb() {
	TUniNetMsg *pMsg = new TUniNetMsg();
	pMsg->dialogType = DIALOG_CONTINUE;
	pMsg->msgType = RTC_TYPE; //flag for send to WebRTC call module
	pMsg->msgName = SIP_BYE;

	TSipCtrlMsg *pSipCtrl = new TSipCtrlMsg();
	printf("from to : %s %s\n",m_UACsipCtrlMsg->from.url.username.c_str(), m_UACsipCtrlMsg->to.url.username.c_str());	
	printf("Web via %s\n",m_UACsipCtrlMsg->via.branch.c_str());
	printf("size: %d\n", m_UACsipCtrlMsg->via.branch.size());
	if (m_UACsipCtrlMsg->via.branch.size() > 1) {
		printf("11111\n");
		pSipCtrl->from = m_UACsipCtrlMsg->from;
		pSipCtrl->to = m_UACsipCtrlMsg->to;
	} else {
		printf("222222");
		pSipCtrl->from = m_UACsipCtrlMsg->to;
		pSipCtrl->to = m_UACsipCtrlMsg->from;
		printf("%s %s\n",m_UACsipCtrlMsg->from.url.username.c_str(),pSipCtrl->to.url.username.c_str());	
	}

	
	
	pSipCtrl->cseq_method = "BYE";
	pMsg->ctrlMsgHdr = pSipCtrl;
	pMsg->setCtrlMsgHdr();

	TSipBye * pBye = new TSipBye();

	pBye->req_uri = pSipCtrl->to.url;

	pMsg->msgBody = pBye;
	pMsg->setMsgBody();

	sendToDispatcher(pMsg);
}

void CSipCallModule::sendBackBYE() {
	printf("*******send back BYE\n");
	TUniNetMsg *pMsg = new TUniNetMsg();
	pMsg->dialogType = DIALOG_CONTINUE;
	pMsg->msgType = SIP_TYPE;
	pMsg->msgName = SIP_BYE;

	TSipCtrlMsg *pSipCtrl = new TSipCtrlMsg();

	if (m_UACsipCtrlMsg->via.branch.size() >= 1) {
		pSipCtrl->from = m_UACsipCtrlMsg->to;
		pSipCtrl->to = m_UACsipCtrlMsg->from;
	} else {
		pSipCtrl->from = m_UACsipCtrlMsg->from;
		pSipCtrl->to = m_UACsipCtrlMsg->to;
	}

	pSipCtrl->cseq_method = "BYE";
	pMsg->ctrlMsgHdr = pSipCtrl;
	pMsg->setCtrlMsgHdr();

	TSipBye * pBye = new TSipBye();

	pBye->req_uri = pSipCtrl->to.url;

	pMsg->msgBody = pBye;
	pMsg->setMsgBody();

	sendToDispatcher(pMsg);

}

void CSipCallModule::sendBack200OK(TUniNetMsg *pCloneMsg) {
	TUniNetMsg *pNewMsg = new TUniNetMsg();
	pNewMsg->dialogType = DIALOG_CONTINUE;
	pNewMsg->msgType = SIP_TYPE;
	pNewMsg->msgName = SIP_RESPONSE;
	pNewMsg->tAddr = pCloneMsg->oAddr;

	pNewMsg->ctrlMsgHdr = (PTSipCtrlMsg) pCloneMsg->cloneCtrlMsg();
	pNewMsg->setCtrlMsgHdr();

	TSipResp *pSipResp = new TSipResp();
	pSipResp->statusCode = 200;
	pSipResp->reason_phase = "ok";
	pNewMsg->msgBody = pSipResp;
	pNewMsg->setMsgBody();
	CTUniNetMsgHelper::print(pNewMsg);

	sendToDispatcher(pNewMsg);
}

void CSipCallModule::sendBack488NotAcceptableHere(PTUniNetMsg msg) {
	TUniNetMsg *pNewMsg = new TUniNetMsg();
	pNewMsg->dialogType = DIALOG_CONTINUE;
	pNewMsg->msgType = SIP_TYPE;
	pNewMsg->msgName = SIP_RESPONSE;
	pNewMsg->tAddr = msg->oAddr;

	pNewMsg->ctrlMsgHdr = (PTSipCtrlMsg) msg->cloneCtrlMsg();
	pNewMsg->setCtrlMsgHdr();

	TSipResp *pSipResp = new TSipResp();
	pSipResp->statusCode = 488;
	pSipResp->reason_phase = "Not Acceptable Here";
	pNewMsg->msgBody = pSipResp;
	pNewMsg->setMsgBody();

	sendToDispatcher(pNewMsg);
}

void CSipCallModule::generateCancel() {
	printf("CSipCallModule::generateCancel\n");
	TUniNetMsg *pMsg = new TUniNetMsg();
	pMsg->dialogType = DIALOG_CONTINUE;
	pMsg->msgType = SIP_TYPE;
	pMsg->msgName = SIP_CANCEL;

	TSipCtrlMsg *pSipCtrl = (TSipCtrlMsg *) m_UACsipCtrlMsg->clone();
	pSipCtrl->cseq_method = "CANCEL";
	pMsg->ctrlMsgHdr = pSipCtrl;
	pMsg->setCtrlMsgHdr();

	TSipCancel *pCancel = new TSipCancel();
	pCancel->req_uri = pSipCtrl->from.url;
	pMsg->msgBody = pCancel;
	pMsg->setMsgBody();

	sendToDispatcher(pMsg);
}

void CSipCallModule::recordVia(PTUniNetMsg msg) {
	TSipCtrlMsg* psipCtrl = (TSipCtrlMsg*) msg->ctrlMsgHdr;
	m_sipvia = psipCtrl->via;
}

void CSipCallModule::attachRecordedViaToMsg(PTUniNetMsg msg) {
	TSipCtrlMsg* psipCtrl = (TSipCtrlMsg*) msg->ctrlMsgHdr;
	psipCtrl->via = m_sipvia;
	msg->setCtrlMsgHdr();
}

void CSipCallModule::sendMessageToSip(TUniNetMsgName msgName,
		TDialogType dialog, PTCtrlMsg pCtrlMsg, PTMsgBody pMsgBody) {
	PTUniNetMsg newMsg = new TUniNetMsg();
	newMsg->tAddr.phyAddr = 0;
	newMsg->tAddr.logAddr = LOGADDR_DISPATCHER;
	newMsg->msgType = SIP_TYPE;
	newMsg->msgName = msgName;
	newMsg->dialogType = dialog;

	newMsg->ctrlMsgHdr = pCtrlMsg;
	newMsg->setCtrlMsgHdr();
	if (pMsgBody != NULL) {
		newMsg->msgBody = pMsgBody;
		newMsg->setMsgBody();
	}

	sendMsg(newMsg);
	return;

}

string CSipCallModule::replaceSdpStr(string &s1, string &s2, string &s3)
{
	string::size_type pos=0;
	string::size_type a=s2.size();
	string::size_type b=s3.size();
	while((pos=s1.find(s2,pos))!=string::npos)
	{
		s1.replace(pos,a,s3);
		printf("s1 %s\n", s1.c_str());
		pos+=b;
	}

	return s1;

}
