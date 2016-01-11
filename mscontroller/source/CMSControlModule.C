#include "CMSControlModule.h"

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/xml/domconfigurator.h>

using namespace log4cxx::xml;
using namespace log4cxx;

log4cxx::LoggerPtr logger;

CLONE_COMP(CMSControlModule)
CREATE_COMP( CMSControlModule)

CMSControlModule::CMSControlModule(PCGFSM afsm) :
	CUACSTask(afsm), m_fsmContext(*this) {
	m_callType = NOCALL;
	m_joinFinished = 0;
	m_bothCallFinished = 0;

	m_MSWebSipCtrlMsg = NULL;
	m_MSIMSSipCtrlMsg = NULL;

	m_parser = NULL;

	m_SipModCtrlMsg = new TSipCtrlMsg();

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

	log4cxx::xml::DOMConfigurator::configureAndWatch("etc/log4cxx.xml", 5000);

	logger = log4cxx::Logger::getLogger("SgFileAppender");
}

PTUACData CMSControlModule::createData() {
	return new TUACData();
}

void CMSControlModule::initState() {
	m_fsmContext.enterStartState();
}

void CMSControlModule::procMsg(PTUniNetMsg * msg) {
	LOG4CXX_DEBUG(logger, "procMsg: current state: "
			<< m_fsmContext.getState().getName() << ", recv msgName "
			<< msg->getMsgNameStr());
	LOG4CXX_DEBUG(logger, "procMsg: recv Msg:\n"
			<< CTUniNetMsgHelper::toString(msg));
	switch (msg->msgName) {
	case INTERNAL_INVITE:
		*m_SipModCtrlMsg = *((PTSipCtrlMsg) msg->ctrlMsgHdr);
		string from = m_SipModCtrlMsg->from.url.username.c_str();
		string to = m_SipModCtrlMsg->to.url.username.c_str();
		if (from == "SIPMODULE") {
			if (to == "WEBXMS") {
				m_callType = WEBCALL;
			} else if (to == "IMSXMS") {
				m_callType = IMSCALL;
			}
		}

		m_fsmContext.onInvite(msg);
		break;
	case INTERNAL_RESPONSE:
		msg->msgName = SIP_RESPONSE;
	case SIP_RESPONSE:
		m_fsmContext.onResponse(msg);
		break;
	case SIP_BYE:
		m_fsmContext.onBye(msg);
		break;
	default:
		LOG4CXX_ERROR(logger, "procMsg:Unknown msgName: "
				<< msg->getMsgNameStr());
		break;
	}

	LOG4CXX_DEBUG(logger, "procMsg:After procMsg state: "
			<< m_fsmContext.getState().getName());
}

void CMSControlModule::sendInviteToMS(TUniNetMsg * msg) {
	if (WEBCALL == m_callType) {
		//WEB CALL XMS
		if (NULL == m_MSWebSipCtrlMsg) {
			m_MSWebSipCtrlMsg = new TSipCtrlMsg();
			m_MSWebSipCtrlMsg->from.url.scheme = "sip";
			m_MSWebSipCtrlMsg->from.url.username = HOST_NAME.c_str();
			m_MSWebSipCtrlMsg->from.url.host = HOST_IP.c_str();
			m_MSWebSipCtrlMsg->from.url.port = HOST_PORT.c_str();

			CHAR buf[33];
			CSipMsgHelper::generateRandomNumberStr(buf);
			m_MSWebSipCtrlMsg->from.tag = buf;

			m_MSWebSipCtrlMsg->to.url.scheme = "sip";
			m_MSWebSipCtrlMsg->to.url.username = MS_NAME.c_str();
			m_MSWebSipCtrlMsg->to.url.host = MS_IP.c_str();
			m_MSWebSipCtrlMsg->to.url.port = MS_PORT.c_str();

			m_MSWebSipCtrlMsg->cseq_method = "INVITE";
			m_MSWebSipCtrlMsg->cseq_number = 21;
		}

		string str = buf;
		str += buf;
		m_MSWebSipCtrlMsg->sip_callId.number = str.c_str();

		PTSipInvite pInvite = new TSipInvite();
		pInvite->req_uri = m_MSWebSipCtrlMsg->to.url;

		pInvite->content_type.type = "application";
		pInvite->content_type.subtype = "sdp";

		pInvite->body = ((PTSipInvite) msg->msgBody)->body;
		sendMessageToSip(SIP_INVITE, DIALOG_BEGIN, m_MSWebSipCtrlMsg->clone(),
				pInvite);

	} else {
		//IMS CALL XMS
		if (NULL == m_MSIMSSipCtrlMsg) {
			m_MSIMSSipCtrlMsg = new TSipCtrlMsg();
			m_MSIMSSipCtrlMsg->from.url.scheme = "sip";
			m_MSIMSSipCtrlMsg->from.url.username = HOST_NAME.c_str();
			m_MSIMSSipCtrlMsg->from.url.host = HOST_IP.c_str();
			m_MSIMSSipCtrlMsg->from.url.port = HOST_PORT.c_str();

			CHAR buf[33];
			CSipMsgHelper::generateRandomNumberStr(buf);
			m_MSIMSSipCtrlMsg->from.tag = buf;

			m_MSIMSSipCtrlMsg->to.url.scheme = "sip";
			m_MSIMSSipCtrlMsg->to.url.username = MS_NAME.c_str();
			m_MSIMSSipCtrlMsg->to.url.host = MS_IP.c_str();
			m_MSIMSSipCtrlMsg->to.url.port = MS_PORT.c_str();

			m_MSIMSSipCtrlMsg->cseq_method = "INVITE";
			m_MSIMSSipCtrlMsg->cseq_number = 21;
		}

		string str = buf;
		str += buf;
		m_MSIMSSipCtrlMsg->sip_callId.number = str.c_str();

		PTSipInvite pInvite = new TSipInvite();
		pInvite->req_uri = m_MSIMSSipCtrlMsg->to.url;

		pInvite->content_type.type = "application";
		pInvite->content_type.subtype = "sdp";

		pInvite->body = ((PTSipInvite) msg->msgBody)->body;
		sendMessageToSip(SIP_INVITE, DIALOG_BEGIN, m_MSIMSSipCtrlMsg->clone(),
				pInvite);
	}
}

void CMSControlModule::sendAckToMS(TUniNetMsg * msg) {
	if (WEBCALL == m_callType) {
		if (m_bothCallFinished & 1 == 0) {
			m_MSWebSipCtrlMsg->to.tag
					= ((PTSipCtrlMsg) msg->ctrlMsgHdr)->to.tag;
			m_WebConnId = string("conn:") + m_MSWebSipCtrlMsg->to.tag.c_str();
		}

		PTSipCtrlMsg pCtrlMsg = (PTSipCtrlMsg) m_MSWebSipCtrlMsg->clone();
		pCtrlMsg->cseq_method = "ACK";
		TSipReq * pSipReq = new TSipReq();
		pSipReq->req_uri = pCtrlMsg->to.url;

		sendMessageToSip(SIP_ACK, DIALOG_CONTINUE, pCtrlMsg, pSipReq);

	} else {
		if (m_bothCallFinished & 2 == 0) {
			m_MSIMSSipCtrlMsg->to.tag
					= ((PTSipCtrlMsg) msg->ctrlMsgHdr)->to.tag;
			m_WebConnId = string("conn:") + m_MSIMSSipCtrlMsg->to.tag.c_str();
		}

		PTSipCtrlMsg pCtrlMsg = (PTSipCtrlMsg) m_MSIMSSipCtrlMsg->clone();
		pCtrlMsg->cseq_method = "ACK";
		TSipReq * pSipReq = new TSipReq();
		pSipReq->req_uri = pCtrlMsg->to.url;

		sendMessageToSip(SIP_ACK, DIALOG_CONTINUE, pCtrlMsg, pSipReq);
	}
}

void CMSControlModule::sendInfoToMS(TUniNetMsg * msg) {
	string infoStr;

	m_parser = new CMessageXmlParse();
	infoStr = m_parser->createConfJoinXml(m_connId_IMS, m_connId_Web);
	delete m_parser;
	m_parser = NULL;

	PTSipCtrlMsg pCtrlMsg = (PTSipCtrlMsg) m_MSWebSipCtrlMsg->clone();
	pCtrlMsg->cseq_method = "INFO";

	PTSipInfo info = new TSipInfo();
	info->body.content = infoStr.c_str();
	info->body.content_length = (UINT) infoStr.length();

	info->content_type.type = "application";
	info->content_type.subtype = "xml";
	info->req_uri = pCtrlMsg->to.url;

	sendMessageToSip(SIP_INFO, DIALOG_CONTINUE, pCtrlMsg->clone(), info);

}

void CMSControlModule::sendRespToSipCall(TUniNetMsg * msg){
	//PTSipResp pResp = (PTSipResp) msg->msgBody->clone();

	sendMessageToSip(INTERNAL_RESPONSE, DIALOG_CONTINUE, m_SipModCtrlMsg->clone(), msg->msgBody->clone());
}

void CMSControlModule::sendByeToSipCall(TUniNetMsg * msg){
	PTSipCtrlMsg pCtrlMsg = (PTSipCtrlMsg) m_MSIMSSipCtrlMsg->clone();
	swap(pCtrlMsg->from, pCtrlMsg->to);

	sendMessageToSip(INTERNAL_BYE, DIALOG_CONTINUE, pCtrlMsg, msg->msgBody->clone());
}


void CMSControlModule::sendMessageToSip(TUniNetMsgName msgName,
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

void CMSControlModule::endTask(){
	LOG4CXX_DEBUG(logger, "endTask: end SIPMODULE");
	sendMessageToSip(SIP_RESPONSE, DIALOG_END, m_SipModCtrlMsg->clone(), NULL);

	if(m_MSIMSSipCtrlMsg != NULL)
		sendMessageToSip(SIP_RESPONSE, DIALOG_END, m_MSIMSSipCtrlMsg->clone(), NULL);

	if(m_MSWebSipCtrlMsg != NULL)
		sendMessageToSip(SIP_RESPONSE, DIALOG_END, m_MSWebSipCtrlMsg->clone(), NULL);
}


bool CMSControlModule::isResp1xx(TUniNetMsg* msg) {
	if (msg->msgType != SIP_TYPE && msg->msgName != SIP_RESPONSE)
		return false;

	INT statusCode = ((PTSipResp) msg->msgBody)->statusCode;
	if (statusCode / 100 == 1)
		return true;
	else
		return false;
}

bool CMSControlModule::isResp2xx(TUniNetMsg* msg) {
	if (msg->msgType != SIP_TYPE && msg->msgName != SIP_RESPONSE)
		return false;

	if (((PTSipResp) msg->msgBody)->statusCode == 200)
		return true;
	else
		return false;
}

bool CMSControlModule::isResp3xx_6xx(TUniNetMsg* msg) {
	if (msg->msgType != SIP_TYPE && msg->msgName != SIP_RESPONSE)
		return false;

	INT statusCode = ((PTSipResp) msg->msgBody)->statusCode;
	if (statusCode < 700 && statusCode > 300)
		return true;
	else
		return false;
}

bool CMSControlModule::isJoinFinished() {
	return m_joinFinished == true;
}

bool CMSControlModule::isBothCallFinished() {
	return m_bothCallFinished == 3;
}

bool CMSControlModule::isFromMS(TUniNetMsg *msg) {
	PTSipCtrlMsg ctrlMsg = (PTSipCtrlMsg) msg->ctrlMsgHdr;
	if (ctrlMsg->from.url.username.c_str() == "msml") {
		return true;
	}
	return false;
}


void CMSControlModule::onTimeOut(TTimeMarkExt timerMark) {
	LOG4CXX_DEBUG(logger, "[CMSControlModule]DEBUG: The CSipCallModule task received a timeout event: "<< timerMark.timerId);
	m_fsmContext.onTimeOut(timerMark);
}



