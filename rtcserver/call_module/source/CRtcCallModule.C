#include "CRtcCallModule.h"

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/xml/domconfigurator.h>

using namespace log4cxx::xml;
using namespace log4cxx;


log4cxx::LoggerPtr logger;

CLONE_COMP(CRtcCallModule)
CREATE_COMP(CRtcCallModule)

CRtcCallModule::CRtcCallModule(PCGFSM afsm) :CUACSTask(afsm),
	m_fsmContext(*this), m_isDispatcherAddrSet(FALSE), m_isReCall(false){
	// get LOGADDR_DISPATCHER from conf
	CProperties* properties = CPropertiesManager::getInstance()->getProperties("rtc.env");
	int LOGADDR_DISPATCHER = properties->getIntProperty("LOGADDR_DISPATCHER");
	if (LOGADDR_DISPATCHER == -1){
		print("LOGADDR_DISPATCHER not correctly set in samples.env,Use DEFAULT 231.\n");
		this->LOGADDR_DISPATCHER = 231;
	}
	else {
		this->LOGADDR_DISPATCHER = LOGADDR_DISPATCHER;
	}

	CProperties* properties2 = CPropertiesManager::getInstance()->getProperties("gateway.env");
	int accessMode = properties2->getIntProperty("accessMode");
	if(accessMode == -1){
		accessMode = 0;
	}
	else{
		this->accessMode = accessMode;
	}


	log4cxx::xml::DOMConfigurator::configureAndWatch("etc/log4cxx.xml", 5000);

	logger = log4cxx::Logger::getLogger("SgFileAppender");

	m_RtcCtrlMsg = new TRtcCtrlMsg();
	m_isReCall = false;
}

CRtcCallModule::~CRtcCallModule(){
	if(m_RtcCtrlMsg){
		delete m_RtcCtrlMsg;
		m_RtcCtrlMsg = NULL;
	}
}

PTUACData CRtcCallModule::createData(){
	return new TUACData();
}

void CRtcCallModule::initState(){
	m_fsmContext.enterStartState();
}

bool CRtcCallModule::selectSipUser(string rtcname){
	CHAR pcSQLStatement[512];
	sprintf(pcSQLStatement,"SELECT sipname FROM user_map_table WHERE isRegistered = 1 AND isCalling = 0 order by lastUsedTime asc");
	CDB::instance()->execSQL(pcSQLStatement);
	PTSelectResult result = CDB::instance()->getSelectResult();

	if(result->rowNum > 0){
		string sipname = result->pRows->arrayField[0].value.stringValue;
		sprintf(pcSQLStatement,"UPDATE user_map_table SET rtcname = '%s', isCalling = 1 WHERE sipname = '%s'", rtcname.c_str(), sipname.c_str());
		CDB::instance()->execSQL(pcSQLStatement);

		LOG4CXX_INFO(logger, "selectSipUser: rtcname: "<<rtcname<<" map to sipname: "<<sipname);
		return true;
	}
	else{
		LOG4CXX_ERROR(logger, "selectSipUser: No IMS sip code idle.");
		return false;
	}

}


bool CRtcCallModule::checkSipUserAvailable(TUniNetMsg * msg){
	if(accessMode == 1 || accessMode == 2)
	{
		PTRtcCtrlMsg pRtcCtrl = (PTRtcCtrlMsg) msg->ctrlMsgHdr;
		return selectSipUser(pRtcCtrl->from.c_str());
	}
	else{
		return true;
	}

}

/** 在这里填充dispatcher的地址
 *  如果DIALOG_END，则通知dispatcher，消除会话信息
 *  如果消息最终发到sip（type = sip）那么需要进行转换再发到dispatcher，否则直接转发。见文档
 */
void CRtcCallModule::sendToDispatcher(TUniNetMsg *pMsg){
	LOG4CXX_DEBUG(logger, "CRtcCallModule::sendToDispatcher\n"<<CTUniNetMsgHelper::toString(pMsg));

	if(m_isDispatcherAddrSet){
		pMsg->tAddr = m_dispatcherAddr;
	}else{
		pMsg->tAddr.phyAddr = 0;
		pMsg->tAddr.taskInstID = 0;
		pMsg->tAddr.logAddr = LOGADDR_DISPATCHER;
	}

	if(pMsg->dialogType == DIALOG_END){
		sendMsg(pMsg);
		return;
	}

	if(pMsg->msgType == RTC_TYPE){

		// just copy the message
		TUniNetMsg *pCopyMsg = (TUniNetMsg *)pMsg->cloneMsg();

		LOG4CXX_DEBUG(logger, "sendToDispatcher: RTC_TYPE, just send to dispatcher\n");
		sendMsg(pCopyMsg);

		return;
	}

	if(pMsg->msgType == SIP_TYPE){
		TUniNetMsg *pNewMsg = new TUniNetMsg();
		msgMap(pMsg, pNewMsg);
		LOG4CXX_DEBUG(logger, "sendToDispatcher: SIP_TYPE, map to sip msg\n"<<CTUniNetMsgHelper::toString(pNewMsg));
		sendMsg(pNewMsg);
	}
	
}

void CRtcCallModule::endTask(){
	LOG4CXX_DEBUG(logger, "endTask current state:" <<m_fsmContext.getState().getName());
	// 产生一条DIALOG——END消息，发给dispatcher，清除会话信息
	TUniNetMsg *pMsg = new TUniNetMsg();
	pMsg->dialogType = DIALOG_END;
	pMsg->msgType = RTC_TYPE;	// 无所谓，先判断dialogType
	pMsg->msgName = RTC_OK;
	pMsg->tAddr.logAddr = LOGADDR_DISPATCHER;
	pMsg->ctrlMsgHdr = m_RtcCtrlMsg->clone();
	pMsg->setCtrlMsgHdr();

	sendToDispatcher(pMsg);
	end();
	LOG4CXX_DEBUG(logger, "endTask finished");
}

//处理消息
void CRtcCallModule::procMsg(PTUniNetMsg msg){
	if(!m_isDispatcherAddrSet){
		m_dispatcherAddr = msg->oAddr;
		m_isDispatcherAddrSet = TRUE;
	}
	LOG4CXX_DEBUG(logger, "procMsg: current state: "<< m_fsmContext.getState().getName() << ", recv msgName "<<msg->getMsgNameStr());
	LOG4CXX_DEBUG(logger, "procMsg: recv Msg:\n"<<CTUniNetMsgHelper::toString(msg));
	switch(msg->msgName){
	case RTC_OFFER:
		*m_RtcCtrlMsg = *((PTRtcCtrlMsg)msg->ctrlMsgHdr);
		m_caller = m_RtcCtrlMsg->from;
		m_callee = m_RtcCtrlMsg->to;
		m_offerSessionId = m_RtcCtrlMsg->offerSessionId;
		if(msg->msgType == SIP_TYPE){
			m_isCaller = true;
		}	
		else{
			m_isCaller = false;
		}
		m_seq = ((TRtcOffer *)msg->msgBody)->seq;
	case RTC_ANSWER:
	case RTC_INFO:
	case RTC_OK:
	case RTC_SHUTDOWN:
	case RTC_ERROR:
		m_fsmContext.onMessage(msg);
		break;
	default:
		LOG4CXX_ERROR(logger, "procMsg:unknow msgName "<<msg->getMsgNameStr());
		//收到非法消息,忽略.等待超时.
		//endTask();
		break;
	}
	LOG4CXX_DEBUG(logger, "after procMsg state: %s"<<m_fsmContext.getState().getName());
}

//处理定时器超时
void CRtcCallModule::onTimeOut (TTimeMarkExt timerMark){
	sendBackError(ERROR_TIMEOUT);
	LOG4CXX_INFO(logger, "The CRtcCallModule task received a timeout event:"<<timerMark.timerId);
	m_fsmContext.onTimeOut(timerMark);
}

//
BOOL CRtcCallModule::msgMap(TUniNetMsg *pSrcMsg, TUniNetMsg *pDestMsg){
	return CRtcToSip::instance()->msgMap(pSrcMsg, pDestMsg, m_caller, m_isReCall);
}

void CRtcCallModule::sendBackError(UINT errorType){
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
	if(m_isCaller){
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


void CRtcCallModule::sendBackOK(TUniNetMsg *msg){ 
	 TUniNetMsg *pNewMsg = new TUniNetMsg();
	 pNewMsg->dialogType = DIALOG_CONTINUE;
	 pNewMsg->msgType =	RTC_TYPE;
	 pNewMsg->msgName = RTC_OK;
	 pNewMsg->tAddr = msg->oAddr;

	 TRtcCtrlMsg *pCtrl = (TRtcCtrlMsg*)msg->cloneCtrlMsg();
	 std::swap(pCtrl->from, pCtrl->to);
	 pCtrl->rtcType=ROAP_OK;
	 pNewMsg->ctrlMsgHdr = pCtrl;
	 pNewMsg->setCtrlMsgHdr();

	 TRtcOK* pOk = new TRtcOK();
	 pOk->seq = ((TRtcShutdown*)msg->msgBody)->seq ;
	 pNewMsg->msgBody = pOk;
	 pNewMsg->setMsgBody();

	 sendToDispatcher(pNewMsg);
}
