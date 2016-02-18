#include "CBearModule.h"
#include "MyLogger.h"
using namespace log4cxx::xml;
using namespace log4cxx;

static MyLogger& mLogger = MyLogger::getInstance("etc/log4cxx.xml", "SgFileAppender");

CLONE_COMP( CBearModule)
CREATE_COMP( CBearModule)

CBearModule::CBearModule(PCGFSM afsm) :
	CUACSTask(afsm), m_fsmContext(*this) {

	m_MSSipCtrlMsg = NULL;

	m_parser = NULL;

	m_IntCtrlMsg = new TIntCtrlMsg();

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
	properties
			= CPropertiesManager::getInstance()->getProperties("gateway.env");
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
}

PTUACData CBearModule::createData() {
	return new TUACData();
}

void CBearModule::initState() {
	m_fsmContext.enterStartState();
}

void CBearModule::procMsg(PTUniNetMsg msg) {
	LOG4CXX_DEBUG(mLogger.getLogger(), "procMsg: current state: "
			<< m_fsmContext.getState().getName() << ", recv msgName "
			<< msg->getMsgNameStr());
	LOG4CXX_DEBUG(mLogger.getLogger(), "procMsg: recv Msg:\n"
			<< CTUniNetMsgHelper::toString(msg));
	switch (msg->msgName) {
	case INTERNAL_REQUEST:
		*m_IntCtrlMsg = *((PTIntCtrlMsg) msg->ctrlMsgHdr);
		swap(m_IntCtrlMsg->from, m_IntCtrlMsg->to);
		m_fsmContext.onSdpRequest(msg);
		break;
	case SIP_RESPONSE:
		m_fsmContext.onResponse(msg);
		break;
	case SIP_BYE:
		m_fsmContext.onBye(msg);
		break;
	default:
		LOG4CXX_ERROR(mLogger.getLogger(), "procMsg:Unknown msgName: "
				<< msg->getMsgNameStr())
		;
		break;
	}

	LOG4CXX_DEBUG(mLogger.getLogger(), "procMsg:After procMsg state: "
			<< m_fsmContext.getState().getName());
}

void CBearModule::sendInviteToMS(TUniNetMsg * msg) {
	//WEB CALL XMS
	if (NULL == m_MSSipCtrlMsg) {
		m_MSSipCtrlMsg = new TSipCtrlMsg();
		m_MSSipCtrlMsg->from.url.scheme = "sip";
		m_MSSipCtrlMsg->from.url.username = HOST_NAME.c_str();
		m_MSSipCtrlMsg->from.url.host = HOST_IP.c_str();
		m_MSSipCtrlMsg->from.url.port = HOST_PORT.c_str();

		CHAR buf[33];
		CSipMsgHelper::generateRandomNumberStr(buf);
		m_MSSipCtrlMsg->from.tag = buf;

		m_MSSipCtrlMsg->to.url.scheme = "sip";
		m_MSSipCtrlMsg->to.url.username = MS_NAME.c_str();
		m_MSSipCtrlMsg->to.url.host = MS_IP.c_str();
		m_MSSipCtrlMsg->to.url.port = MS_PORT.c_str();

		string str = buf;
		str += buf;
		m_MSSipCtrlMsg->sip_callId.number = str.c_str();

		m_MSSipCtrlMsg->cseq_method = "INVITE";
		m_MSSipCtrlMsg->cseq_number = 21;
	}

	PTSipInvite pInvite = new TSipInvite();
	pInvite->req_uri = m_MSSipCtrlMsg->to.url;

	pInvite->content_type.type = "application";
	pInvite->content_type.subtype = "sdp";

	pInvite->body.content = ((PTIntRequest) msg->msgBody)->body;
	pInvite->body.content_length = pInvite->body.content.length();
	sendMsgToDispatcher(SIP_INVITE, SIP_TYPE, DIALOG_BEGIN,
			m_MSSipCtrlMsg->clone(), pInvite);

}

void CBearModule::setConnId(TUniNetMsg * msg) {
	m_MSSipCtrlMsg->to.tag = ((PTSipCtrlMsg) msg->ctrlMsgHdr)->to.tag;
	m_MSConnId = string("conn:") + m_MSSipCtrlMsg->to.tag.c_str();
}

void CBearModule::sendAckToMS(TUniNetMsg * msg) {

	PTSipCtrlMsg pCtrlMsg = (PTSipCtrlMsg) m_MSSipCtrlMsg->clone();
	pCtrlMsg->cseq_method = "ACK";
	TSipReq * pSipReq = new TSipReq();
	pSipReq->req_uri = pCtrlMsg->to.url;

	sendMsgToDispatcher(SIP_ACK, SIP_TYPE, DIALOG_CONTINUE, pCtrlMsg, pSipReq);

}


void CBearModule::sendCancelToMS(){
	PTSipCtrlMsg pCtrlMsg = (PTSipCtrlMsg) m_MSSipCtrlMsg->clone();
	pCtrlMsg->cseq_method = "CANCEL";
	TSipCancel * pSipReq = new TSipCancel();
	pSipReq->req_uri = pCtrlMsg->to.url;

	sendMsgToDispatcher(SIP_CANCEL, SIP_TYPE, DIALOG_CONTINUE, pCtrlMsg, pSipReq);

}

void CBearModule::sendByeToMS(){
	PTSipCtrlMsg pCtrlMsg = (PTSipCtrlMsg) m_MSSipCtrlMsg->clone();
	pCtrlMsg->cseq_method = "BYE";
	TSipBye * pSipReq = new TSipBye();
	pSipReq->req_uri = pCtrlMsg->to.url;

	sendMsgToDispatcher(SIP_BYE, SIP_TYPE, DIALOG_CONTINUE, pCtrlMsg, pSipReq);

}

void CBearModule::sendJoinToMS(TUniNetMsg * msg) {
	string infoStr;

	string anotherConnId = ((PTIntRequest) msg->msgBody)->body.c_str();

	m_parser = new CMessageXmlParse();
	infoStr = m_parser->createConfJoinXml(m_MSConnId, anotherConnId);
	delete m_parser;
	m_parser = NULL;

	PTSipCtrlMsg pCtrlMsg = (PTSipCtrlMsg) m_MSSipCtrlMsg->clone();
	pCtrlMsg->cseq_method = "INFO";

	PTSipInfo info = new TSipInfo();
	info->body.content = infoStr.c_str();
	info->body.content_length = (UINT) infoStr.length();

	info->content_type.type = "application";
	info->content_type.subtype = "xml";
	info->req_uri = pCtrlMsg->to.url;

	sendMsgToDispatcher(SIP_INFO,  SIP_TYPE, DIALOG_CONTINUE, pCtrlMsg->clone(), info);

}

void CBearModule::sendSdpAnswerToCall(TUniNetMsg * msg) {
	//PTSipResp pResp = (PTSipResp) msg->msgBody->clone();
	PTIntCtrlMsg pCtrlMsg = (PTIntCtrlMsg) m_IntCtrlMsg->clone();

	PTIntResponse pResp = new TIntResponse();
	pResp->body = ((PTSipResp) msg->msgBody)->body.content;

	sendMsgToDispatcher(INTERNAL_RESPONSE, SIP_TYPE, DIALOG_BEGIN, pCtrlMsg,
			pResp);
}

void CBearModule::sendErrorToCall(const int errorType) {
	PTIntCtrlMsg pCtrlMsg = (PTIntCtrlMsg) m_IntCtrlMsg->clone();
	PTIntError pError = new TIntError();
	pError->errorType = errorType;

	sendMsgToDispatcher(INTERNAL_ERROR, INT_TYPE, DIALOG_CONTINUE, pCtrlMsg,
			pError);
}

void CBearModule::sendCloseToCall() {
	PTIntCtrlMsg pCtrlMsg = (PTIntCtrlMsg) m_IntCtrlMsg->clone();

	sendMsgToDispatcher(INTERNAL_CLOSE, INT_TYPE, DIALOG_CONTINUE, pCtrlMsg,
			NULL);
}

void CBearModule::sendMsgToDispatcher(TUniNetMsgName msgName,
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
	return;
}

void CBearModule::endTask() {
	LOG4CXX_DEBUG(mLogger.getLogger(), "endTask: end BearModule");
	sendMsgToDispatcher(SIP_RESPONSE, INT_TYPE, DIALOG_END,
			m_IntCtrlMsg->clone(), NULL);

	if (m_MSSipCtrlMsg != NULL)
		sendMsgToDispatcher(SIP_RESPONSE, SIP_TYPE, DIALOG_END,
				m_MSSipCtrlMsg->clone(), NULL);
}

bool CBearModule::isResp1xx(TUniNetMsg* msg) {
	if (msg->msgType != SIP_TYPE && msg->msgName != SIP_RESPONSE)
		return false;

	INT statusCode = ((PTSipResp) msg->msgBody)->statusCode;
	if (statusCode / 100 == 1)
		return true;
	else
		return false;
}

bool CBearModule::isResp2xx(TUniNetMsg* msg) {
	if (msg->msgType != SIP_TYPE && msg->msgName != SIP_RESPONSE)
		return false;

	if (((PTSipResp) msg->msgBody)->statusCode == 200)
		return true;
	else
		return false;
}

bool CBearModule::isResp3xx_6xx(TUniNetMsg* msg) {
	if (msg->msgType != SIP_TYPE && msg->msgName != SIP_RESPONSE)
		return false;

	INT statusCode = ((PTSipResp) msg->msgBody)->statusCode;
	if (statusCode < 700 && statusCode > 300)
		return true;
	else
		return false;
}
bool CBearModule::isWithSDP(TUniNetMsg * msg) {
	if (msg->msgType != SIP_TYPE && msg->msgName != SIP_RESPONSE)
		return false;

	PTSipResp pResp = (PTSipResp) msg->msgBody;
	return pResp->body.content_length != 0;
}

string CBearModule::checkRespCSeqMethod(TUniNetMsg * msg) {
	if (msg->msgType != SIP_TYPE && msg->msgName != SIP_RESPONSE)
		return NULL;

	return ((PTSipCtrlMsg) msg->ctrlMsgHdr)->cseq_method.c_str();
}

void CBearModule::onTimeOut(TTimeMarkExt timerMark) {
	LOG4CXX_DEBUG(mLogger.getLogger(),
			"[CBearModule]DEBUG: The CSipCallModule task received a timeout event: "
			<< timerMark.timerId);
	m_fsmContext.onTimeOut(timerMark);
	//CUACTask::setTimer(timerMark, NULL, NULL);
}

