#include "CMsgDispatcher.h"

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/xml/domconfigurator.h>

using namespace log4cxx::xml;
using namespace log4cxx;

log4cxx::LoggerPtr logger;

CLONE_COMP(CMsgDispatcher)
CREATE_COMP(CMsgDispatcher)

CMsgDispatcher::CMsgDispatcher(PCGFSM afsm) :
	CUACTask(afsm), m_isrtcPsaAddrSet(FALSE), m_issipPsaAddrSet(FALSE) {
	// get value from conf
	CProperties* properties = CPropertiesManager::getInstance()->getProperties(
			"rtc.env");
	if ((this->LOGADDR_SIP_PSA = properties->getIntProperty("LOGADDR_SIP_PSA"))
			== -1) {
		this->LOGADDR_SIP_PSA = 11;
	}
	if ((this->LOGADDR_RTC_PSA = properties->getIntProperty("LOGADDR_RTC_PSA"))
			== -1) {
		this->LOGADDR_RTC_PSA = 18;
	}
	if ((this->LOGADDR_RTC_SIP_CALL = properties->getIntProperty(
			"LOGADDR_RTC_SIP_CALL")) == -1) {
		this->LOGADDR_RTC_SIP_CALL = 232;
	}
	if ((this->LOGADDR_BEAR_MOD
			= properties->getIntProperty("LOGADDR_BEAR_MOD")) == -1) {
		this->LOGADDR_BEAR_MOD = 233;
	}

	if ((this->LOGADDR_DISPATCHER = properties->getIntProperty(
			"LOGADDR_DISPATCHER")) == -1) {
		this->LOGADDR_DISPATCHER = 231;
	}
	m_pDialogCtrl = new CDialogController();

	DOMConfigurator::configureAndWatch("etc/log4cxx.xml", 5000);

	logger = log4cxx::Logger::getLogger("SgFileAppender");
}

CMsgDispatcher::~CMsgDispatcher() {
	if (m_pDialogCtrl != NULL) {
		delete m_pDialogCtrl;
		m_pDialogCtrl = NULL;
	}
}

void CMsgDispatcher::procMsg(TUniNetMsg* msg) {
	LOG4CXX_DEBUG(logger, "dispatcher get msg from "<<msg->oAddr.logAddr);
	LOG4CXX_DEBUG(logger, "dispatcher recv msg\n "<<CTUniNetMsgHelper::toString(msg));
	if (msg->oAddr.logAddr == (UINT) LOGADDR_SIP_PSA) {
		handleMsgFromSipPSA(msg);
	} else if (msg->oAddr.logAddr == (UINT) LOGADDR_RTC_PSA) {
		handleMsgFromRtcPSA(msg);
	} else if (msg->oAddr.logAddr == (UINT) LOGADDR_RTC_SIP_CALL) {
		handleMsgFromRtcSipCall(msg);
	} else if (msg->oAddr.logAddr == (UINT) LOGADDR_BEAR_MOD) {
		handleMsgFromBear(msg);
	} else {
		LOG4CXX_ERROR(logger, "###CMsgDispatcher: received an unexpected message from logaddr " << msg->oAddr.logAddr << ",This task will be ended.\n"
		);
		end();
	}

}

void CMsgDispatcher::justPassRegister(TUniNetMsg* msg) {
	printf("pass register, from: ");
	TSipVia sipVia = ((TSipCtrlMsg*) msg->ctrlMsgHdr)->via;
	printf("addr: %s, port: %s\n", sipVia.url.host.c_str(),
			sipVia.url.port.c_str());
	PTUniNetMsg newMsg = new TUniNetMsg();
	//1.通用消息头
	newMsg->tAddr = msg->oAddr;
	newMsg->msgType = SIP_TYPE;
	newMsg->msgName = SIP_RESPONSE;
	newMsg->dialogType = DIALOG_CONTINUE;
	//2 控制消息头
	newMsg->ctrlMsgHdr = msg->ctrlMsgHdr->clone();
	newMsg->setCtrlMsgHdr();
	//3 消息体
	PTSipResp ans = new TSipResp();
	ans->statusCode = 200;
	newMsg->msgBody = ans;
	newMsg->setMsgBody();
	sendMsg(newMsg);
}

void CMsgDispatcher::handleMsgFromBear(TUniNetMsg * msg) {
	// sip call module调用endTask时发送过来，不用再转发

	string uniqID = generateIntUniqID(msg);

	if (msg->dialogType == DIALOG_BEGIN) {
		if (!m_pDialogCtrl->storeBear(uniqID, msg->oAddr)) {
			LOG4CXX_ERROR(logger, "handleMsgFromSipCall can not store "<<uniqID.c_str()<< "logdaddr: "<< msg->oAddr.logAddr);
		}
	}

	if (msg->dialogType == DIALOG_END) {
		m_pDialogCtrl->clearBear(uniqID);
		m_pDialogCtrl->clearDialog(uniqID);
		return;
	}

	switch (msg->msgName) {
	case INT_CLOSE:
	case INT_RESPONSE: {
		TMsgAddress tAddr;
		if (m_pDialogCtrl->getDialogAddr(uniqID, tAddr)) {
			sendMsgtoInstance(msg, tAddr, DIALOG_CONTINUE);
		} else {
			LOG4CXX_ERROR(logger ,"handleMsgFromMSControl can not get addr by "<< uniqID);
		}
		break;
	}
	default:
		LOG4CXX_ERROR(logger, "handleMsgFromMSControl: received unknown msgName "<<msg->getMsgName())
		;
		break;
	}
}

void CMsgDispatcher::handleMsgFromSipPSA(TUniNetMsg* msg) {
	//CTUniNetMsgHelper::print(msg);

	// 记录sippsa地址
	if (!m_issipPsaAddrSet) {
		m_sipPsaAddr = msg->oAddr;
		m_issipPsaAddrSet = TRUE;
	}

	string uniqID = generateSipUniqID(msg);

	TMsgAddress tAddr;
	switch (msg->msgName) {
	case SIP_REGISTER:
		justPassRegister(msg);
		break;
	case SIP_INVITE:
		if (m_pDialogCtrl->getDialogAddr(uniqID, tAddr)) {
			LOG4CXX_DEBUG(logger, "CMsgDispatcher::handleMsgFromSipPSA this is a reinvite\n");
			sendMsgtoInstance(msg, tAddr, DIALOG_CONTINUE);
		} else {
			tAddr.logAddr = LOGADDR_RTC_SIP_CALL;
			tAddr.phyAddr = 0;
			tAddr.taskInstID = 0;
			sendMsgtoInstance(msg, tAddr, DIALOG_BEGIN);
		}
		break;
	case SIP_ACK:
	case SIP_CANCEL:
	case SIP_INFO:
	case SIP_UPDATE:
	case SIP_BYE:
	case SIP_RESPONSE: {
		if (m_pDialogCtrl->getDialogAddr(uniqID, tAddr)) {
			sendMsgtoInstance(msg, tAddr, DIALOG_CONTINUE);
		} else {
			LOG4CXX_ERROR(logger, "handleMsgFromSipPSA can not get addr by "<< uniqID);
		}
		break;
	}
	default:
		LOG4CXX_ERROR(logger, "received unknown msgName "<<msg->getMsgNameStr())
		;
		break;
	}
}

void CMsgDispatcher::handleMsgFromRtcSipCall(TUniNetMsg* msg) {
	//call module调用endTask时发送过来，不用再转发
	//LOG4CXX_DEBUG(logger, "handleMsgFromRtcSipCall recv Msg:\n"<<CTUniNetMsgHelper::toString(msg));
	string uniqID;
	switch (msg->msgType) {
	case SIP_TYPE:
		uniqID = generateSipUniqID(msg);
		break;
	case RTC_TYPE:
		uniqID = generateRtcUniqID(msg);
		break;
	case INT_TYPE:
		uniqID = generateIntUniqID(msg);
		break;
	default:
		LOG4CXX_ERROR(logger, "received unknown msgType: "<<msg->msgType)
		;
		break;
	}

	if (msg->dialogType == DIALOG_BEGIN) {

		if (!m_pDialogCtrl->storeDialog(uniqID, msg->oAddr)) {
			LOG4CXX_ERROR(logger, "handleMsgFromSipCall can not store "<<uniqID.c_str()<< "logdaddr: "<< msg->oAddr.logAddr);
			return;
		}
	}

	if (msg->dialogType == DIALOG_END) {
		m_pDialogCtrl->clearDialog(uniqID);
		return;
	}

	if (msg->msgType == INT_TYPE) {
		TMsgAddress tAddr;
		if (m_pDialogCtrl->getBearAddr(uniqID, tAddr)) {
			sendMsgtoInstance(msg, tAddr, DIALOG_CONTINUE);
		} else if (msg->msgName == INT_REQUEST) {
			//发送给BEAR模块
			TMsgAddress tAddr;
			tAddr.logAddr = LOGADDR_BEAR_MOD;
			tAddr.phyAddr = 0;
			tAddr.taskInstID = 0;
			sendMsgtoInstance(msg, tAddr, DIALOG_BEGIN);
			return;
		} else {
			LOG4CXX_ERROR(logger, "handleMsgFromSipCall can not get addr by " << uniqID);
			return;
		}

	}

	if (msg->msgType == SIP_TYPE) {
		//发送给SIP-PSA
		sendMsgtoInstance(msg, getSipPsaAddr(), DIALOG_CONTINUE);
		return;
	}

	// 原始消息来自sip，发给rtc psa
	if (msg->msgType == RTC_TYPE) {
		sendMsgtoInstance(msg, getRtcPsaAddr(), DIALOG_CONTINUE);

	}
}

/* 判断是否有已经建立会话，无则记录，DIALOG—BEGIN，否则DIALOG_CONTINUE
 */
void CMsgDispatcher::handleMsgFromRtcPSA(TUniNetMsg* msg) {
	LOG4CXX_DEBUG(logger, "Msg from RTC Psa, roap Type is %s\n" <<msg->getMsgNameStr());
	LOG4CXX_DEBUG(logger, "msg from RTC psa:\n"<<CTUniNetMsgHelper::toString(msg));

	// 记录rtcpsa地址
	if (!m_isrtcPsaAddrSet) {
		m_rtcPsaAddr = msg->oAddr;
		m_isrtcPsaAddrSet = TRUE;
	}

	string uniqID = generateRtcUniqID(msg);
	TMsgAddress tAddr;
	switch (msg->msgName) {
	case RTC_OFFER:
		if (m_pDialogCtrl->getDialogAddr(uniqID, tAddr)) {
			LOG4CXX_DEBUG(logger, "CMsgDispatcher::handleMsgFromRtcPsa: this is a reoffer\n");
			sendMsgtoInstance(msg, tAddr, DIALOG_CONTINUE);
		} else {
			tAddr.logAddr = LOGADDR_RTC_SIP_CALL;
			tAddr.phyAddr = 0;
			tAddr.taskInstID = 0;
			sendMsgtoInstance(msg, tAddr, DIALOG_BEGIN);
		}
		break;
	case RTC_OK:
	case RTC_ANSWER:
	case RTC_SHUTDOWN:
	case RTC_ERROR: {
		if (m_pDialogCtrl->getDialogAddr(uniqID, tAddr)) {
			sendMsgtoInstance(msg, tAddr, DIALOG_CONTINUE);
		} else {
			LOG4CXX_ERROR(logger, "handleMsgFromRtcPSA can not get addr by "<< uniqID);
		}
		break;
	}
	default:
		LOG4CXX_ERROR(logger, "received unknown msgName "<<msg->getMsgNameStr())
		;
		break;
	}
}

/** 填写消息的地址并发送到 instAddr 指定的模块去
 *
 */
void CMsgDispatcher::sendMsgtoInstance(TUniNetMsg* msg, TMsgAddress instAddr,
		TDialogType dialogtype) {
	//创建将被发送的通用消息实例
	PTUniNetMsg newMsg = new TUniNetMsg();

	//1.通用消息头
	newMsg->tAddr = instAddr;
	newMsg->msgType = msg->msgType;
	newMsg->msgName = msg->msgName;
	newMsg->dialogType = dialogtype;

	//2 控制消息头
	newMsg->ctrlMsgHdr = msg->ctrlMsgHdr->clone();
	newMsg->setCtrlMsgHdr();

	//3 消息体
	newMsg->msgBody = msg->msgBody->clone();
	newMsg->setMsgBody();

	sendMsg(newMsg);
}

void CMsgDispatcher::onTimeOut(TTimerKey timerKey, TTimeMarkExt timerMark,
		void* para) {

}
void CMsgDispatcher::onTimeUpdate(TTimeMarkExt timerMark,
		TTimerKey oldTimerKey, TTimerKey newTimerKey, void* para) {

}

TMsgAddress CMsgDispatcher::getRtcPsaAddr() {
	if (m_isrtcPsaAddrSet)
		return m_rtcPsaAddr;
	TMsgAddress tAddr;
	tAddr.phyAddr = 0;
	tAddr.taskInstID = 0;
	tAddr.logAddr = LOGADDR_RTC_PSA;
	return tAddr;
}

TMsgAddress CMsgDispatcher::getSipPsaAddr() {
	if (m_issipPsaAddrSet)
		return m_sipPsaAddr;
	TMsgAddress tAddr;
	tAddr.phyAddr = 0;
	tAddr.taskInstID = 0;
	tAddr.logAddr = LOGADDR_SIP_PSA;
	return tAddr;
}
