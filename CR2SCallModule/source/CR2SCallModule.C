#include "CR2SCallModule.h"

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/xml/domconfigurator.h>

using namespace log4cxx::xml;
using namespace log4cxx;

log4cxx::LoggerPtr logger;

CLONE_COMP(CR2SCallModule)
CREATE_COMP(CR2SCallModule)

CR2SCallModule::CR2SCallModule(PCGFSM afsm) :
	CUACSTask(afsm), m_fsmContext(*this), m_isDispatcherAddrSet(FALSE),
			m_isReCall(false) {
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
		this->accessMode = accessMode;
	}

	log4cxx::xml::DOMConfigurator::configureAndWatch("etc/log4cxx.xml", 5000);

	logger = log4cxx::Logger::getLogger("SgFileAppender");

	m_rtcCtrlMsg = new TRtcCtrlMsg();
	m_intCtrlMsg = NULL;
	m_isSdpConfirmed = false;
}

CR2SCallModule::~CR2SCallModule() {
	if (m_rtcCtrlMsg) {
		delete m_rtcCtrlMsg;
		m_rtcCtrlMsg = NULL;
	}
}

PTUACData CR2SCallModule::createData() {
	return new TUACData();
}

void CR2SCallModule::initState() {
	m_fsmContext.enterStartState();
}

bool CR2SCallModule::selectSipUser(string rtcname) {
	CHAR pcSQLStatement[512];
	sprintf(
			pcSQLStatement,
			"SELECT sipname FROM user_map_table WHERE isRegistered = 1 AND isCalling = 0 order by lastUsedTime asc");
	CDB::instance()->execSQL(pcSQLStatement);
	PTSelectResult result = CDB::instance()->getSelectResult();

	if (result->rowNum > 0) {
		string sipname = result->pRows->arrayField[0].value.stringValue;
		sprintf(
				pcSQLStatement,
				"UPDATE user_map_table SET rtcname = '%s', isCalling = 1 WHERE sipname = '%s'",
				rtcname.c_str(), sipname.c_str());
		CDB::instance()->execSQL(pcSQLStatement);

		LOG4CXX_INFO(logger, "selectSipUser: rtcname: "<<rtcname<<" map to sipname: "<<sipname);
		return true;
	} else {
		LOG4CXX_ERROR(logger, "selectSipUser: No IMS sip code idle.");
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

	//	if(pMsg->dialogType == DIALOG_END){
	//		sendMsg(pMsg);
	//		return;
	//	}

	//if(pMsg->msgType == RTC_TYPE){
	// just copy the message
	//TUniNetMsg *pCopyMsg = (TUniNetMsg *)pMsg->cloneMsg();

	//LOG4CXX_DEBUG(logger, "sendToDispatcher: RTC_TYPE, just send to dispatcher\n");

	//}

	//	if(pMsg->msgType == SIP_TYPE){
	//		TUniNetMsg *pNewMsg = new TUniNetMsg();
	//		msgMap(pMsg, pNewMsg);
	//		LOG4CXX_DEBUG(logger, "sendToDispatcher: SIP_TYPE, map to sip msg\n"<<CTUniNetMsgHelper::toString(pNewMsg));
	//		sendMsg(pNewMsg);
	//	}

}

void CR2SCallModule::endTask() {
	LOG4CXX_DEBUG(logger, "endTask current state:" <<m_fsmContext.getState().getName());
	// 产生一条DIALOG——END消息，发给dispatcher，清除会话信息
	TUniNetMsg *pMsg = new TUniNetMsg();
	pMsg->dialogType = DIALOG_END;
	pMsg->msgType = RTC_TYPE; // 无所谓，先判断dialogType
	pMsg->msgName = RTC_OK;
	pMsg->tAddr.logAddr = LOGADDR_DISPATCHER;
	pMsg->ctrlMsgHdr = m_rtcCtrlMsg->clone();
	pMsg->setCtrlMsgHdr();

	sendToDispatcher(pMsg);
	end();
	LOG4CXX_DEBUG(logger, "endTask finished");
}

//处理消息
void CR2SCallModule::procMsg(PTUniNetMsg msg) {
	if (!m_isDispatcherAddrSet) {
		m_dispatcherAddr = msg->oAddr;
		m_isDispatcherAddrSet = TRUE;
	}
	LOG4CXX_DEBUG(logger, "procMsg: current state: "<< m_fsmContext.getState().getName() << ", recv msgName "<<msg->getMsgNameStr());
	LOG4CXX_DEBUG(logger, "procMsg: recv Msg:\n"<<CTUniNetMsgHelper::toString(msg));
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
		break;
	}
	case RTC_ANSWER:
	case RTC_INFO:
	case RTC_OK:
	case RTC_SHUTDOWN:
	case RTC_ERROR:
		m_fsmContext.onMessage(msg);
		break;
	default:
		LOG4CXX_ERROR(logger, "procMsg:unknow msgName "<<msg->getMsgNameStr())
		;
		//收到非法消息,忽略.等待超时.
		//endTask();
		break;
	}
	LOG4CXX_DEBUG(logger, "after procMsg state: %s"<<m_fsmContext.getState().getName());
}

//处理定时器超时
void CR2SCallModule::onTimeOut(TTimeMarkExt timerMark) {
	sendBackError(ERROR_TIMEOUT);
	LOG4CXX_INFO(logger, "The CR2SCallModule task received a timeout event:"<<timerMark.timerId);
	m_fsmContext.onTimeOut(timerMark);
}

//
BOOL CR2SCallModule::msgMap(TUniNetMsg *pSrcMsg, TUniNetMsg *pDestMsg) {
	return CRtcToSip::instance()->msgMap(pSrcMsg, pDestMsg, m_caller,
			m_isReCall);
}

void CR2SCallModule::sendBackError(UINT errorType) {
	TUniNetMsg *pNewMsg = new TUniNetMsg();
	pNewMsg->dialogType = DIALOG_CONTINUE;
	pNewMsg->msgType = RTC_TYPE;
	pNewMsg->msgName = RTC_ERROR;
	//pNewMsg->tAddr = LOGADDR_DISPATCHER;
	TRtcCtrlMsg * pCtrl = new TRtcCtrlMsg();
	pCtrl->from = m_caller;
	pCtrl->to = m_callee;
	pCtrl->offerSessionId = m_offerSessionId;
	pCtrl->rtcType = ROAP_ERROR;
	if (m_isCaller) {
		//printf("from %s,to %s\n", pCtrl->from.c_str(), pCtrl->to.c_str());
		std::swap(pCtrl->from, pCtrl->to);
	}
	pCtrl->answerSessionId = "webrtc_4ff57274d9a89b47c415be9c1";
	pNewMsg->ctrlMsgHdr = pCtrl;
	pNewMsg->setCtrlMsgHdr();
	TRtcError * pError = new TRtcError();
	pError->seq = m_seq;
	pNewMsg->msgBody = pError;
	pError->errorType = errorType;
	pNewMsg->setMsgBody();
	sendToDispatcher(pNewMsg);

}

void CR2SCallModule::sendBackOK(TUniNetMsg *msg) {
	TUniNetMsg *pNewMsg = new TUniNetMsg();
	pNewMsg->dialogType = DIALOG_CONTINUE;
	pNewMsg->msgType = RTC_TYPE;
	pNewMsg->msgName = RTC_OK;
	pNewMsg->tAddr = msg->oAddr;

	TRtcCtrlMsg *pCtrl = (TRtcCtrlMsg*) msg->cloneCtrlMsg();
	std::swap(pCtrl->from, pCtrl->to);
	pCtrl->rtcType = ROAP_OK;
	pNewMsg->ctrlMsgHdr = pCtrl;
	pNewMsg->setCtrlMsgHdr();

	TRtcOK* pOk = new TRtcOK();
	pOk->seq = ((TRtcShutdown*) msg->msgBody)->seq;
	pNewMsg->msgBody = pOk;
	pNewMsg->setMsgBody();

	sendToDispatcher(pNewMsg);
}

bool CR2SCallModule::checkSipUserAvailable(TUniNetMsg * msg) {
	if (accessMode == 1 || accessMode == 2) {
		PTRtcCtrlMsg pRtcCtrl = (PTRtcCtrlMsg) msg->ctrlMsgHdr;
		return selectSipUser(pRtcCtrl->from.c_str());
	} else {
		return true;
	}

}

void CR2SCallModule::notifySipTermCallSdp(TUniNetMsg * msg) {
	//onNotify(NULL);
}

void CR2SCallModule::notifySipTermClose() {
	//onClose(NULL)
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
	pAnswer->sdp = pResp->body;

	TUniNetMsg * newMsg = new TUniNetMsg();
	sendToDispatcher(RTC_ANSER, RTC_TYPE, DIALOG_CONTINUE, m_rtcCtrlMsg->clone(), pAnswer);
	//m_rtcCtrlMsg
}

void CR2SCallModule::sendErrorToWeb(int errorType) {

	TRtcError * pError = new TRtcError();
	pError->seq = m_seq;
	pError->errorType = errorType;


	sendToDispatcher(RTC_ERROR, RTC_TYPE, DIALOG_CONTINUE, m_rtcCtrlMsg->clone(), pError);

}

void CR2SCallModule::forwardErrorToWeb(TUniNetMsg * msg) {
	sendToDispatcher(RTC_ERROR, RTC_TYPE, DIALOG_CONTINUE, msg->ctrlMsgHdr,
			msg->msgBody);
}

void CR2SCallModule::sendShutdownToWeb() {

	TRtcShutdown * pShutdown = new TRtcShutdown();
	pShutdown->seq = m_seq;


	sendToDispatcher(RTC_SHUTDOWN, RTC_TYPE, DIALOG_CONTINUE, m_rtcCtrlMsg->clone(), pShutdown);
}

void CR2SCallModule::sendNotifyToWeb(TUniNetMsg * msg){
	PTIntRequest pReq = (PTIntRequest)msg->msgBody;

	PTRtcNotify pNotify = new TRtcNotify();
	pNotify->seq = m_seq;
	pNotify->content = pReq->body;
	pNotify->content_length = pNotify->content.length();

	PTRtcCtrlMsg ctrlMsg = m_rtcCtrlMsg->clone();

	sendToDispatcher(RTC_NOTIFY, RTC_TYPE, DIALOG_CONTINUE, m_rtcCtrlMsg->clone(), pNotify);
}

bool CR2SCallModule::isSdpConfirmed(){
	return m_isSdpConfirmed;
}

void CR2SCallModule::sendReqToBear_Rtc(){
	if(m_intCtrlMsg == NULL){
		m_intCtrlMsg->from = "rtc_call";
		m_intCtrlMsg->to = "bear";
		m_intCtrlMsg->sessionId = m_offerSessionId;
		m_intCtrlMsg->intType = INT_REQUEST;
	}

	PTIntRequest pReq = new TIntRequest();
	pReq->body = m_webSdp.c_str();

	sendToDispatcher(INT_REQUEST, INT_TYPE, DIALOG_CONTINUE, m_intCtrlMsg->clone(), pReq);
}

void CR2SCallModule::sendCloseToBear_Rtc()
{
	if(m_intCtrlMsg != NULL)
	{
		PTIntClose pClose = new TIntClose();
		sendToDispatcher(INT_CLOSE, INT_TYPE, DIALOG_CONTINUE, m_intCtrlMsg->clone(), pReq);
	}
}

