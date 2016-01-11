#include "CSipMessageModule.h"
CLONE_COMP(CSipMessageModule)
CREATE_COMP(CSipMessageModule)

CSipMessageModule::CSipMessageModule(PCGFSM afsm):CUACSTask(afsm), m_fsmContext(*this){
	CProperties* properties = CPropertiesManager::getInstance()->getProperties("rtc.env");
	int LOGADDR_DISPATCHER = properties->getIntProperty("LOGADDR_DISPATCHER");
	if (LOGADDR_DISPATCHER == -1){
		print("LOGADDR_DISPATCHER not correctly set in samples.env,Use DEFAULT 231.\n");
		this->LOGADDR_DISPATCHER = 231;
	}
	else {
		this->LOGADDR_DISPATCHER = LOGADDR_DISPATCHER;
	}
	m_SipCtrlMsg = new TSipCtrlMsg();
}

CSipMessageModule::~CSipMessageModule(){
	if(m_SipCtrlMsg){
		delete m_SipCtrlMsg;
		m_SipCtrlMsg = NULL;
	}
}

void CSipMessageModule::sendToDispatcher(TUniNetMsg *pMsg){
	// 也许该记录下dispatcher的地址
	pMsg->tAddr.phyAddr = 0;
	pMsg->tAddr.taskInstID = 0;
	pMsg->tAddr.logAddr = LOGADDR_DISPATCHER;
	if(pMsg->dialogType == DIALOG_END){
		sendMsg(pMsg);
		return;
	}

	if(pMsg->msgType == SIP_TYPE){
		TUniNetMsg *pCopyMsg = (TUniNetMsg *)pMsg->cloneMsg();
		sendMsg(pCopyMsg);
		return;
	}

	if(pMsg->msgType == RTC_TYPE){
		TUniNetMsg *pNewMsg = new TUniNetMsg();
		msgMap(pMsg, pNewMsg);
		sendMsg(pNewMsg);
	}
}

void CSipMessageModule::endTask(){
	printf("endTask current state: %s %d\n",m_fsmContext.getState().getName(),m_fsmContext.getState().getId());
	// 产生一条DIALOG——END消息，发给dispatcher，清除会话信息
	TUniNetMsg *pMsg = new TUniNetMsg();
	pMsg->dialogType = DIALOG_END;
	pMsg->msgType = SIP_TYPE;	// 无所谓，先判断dialogType
	pMsg->msgName = SIP_RESPONSE;
	pMsg->ctrlMsgHdr = m_SipCtrlMsg->clone();
	pMsg->setCtrlMsgHdr();

	sendToDispatcher(pMsg);
	end();
	printf("endTask finish\n");
}

void CSipMessageModule::sendBack200OK(TUniNetMsg *msg){
	 TUniNetMsg *pNewMsg = new TUniNetMsg();
	 pNewMsg->dialogType = DIALOG_CONTINUE;
	 pNewMsg->msgType = SIP_TYPE;
	 pNewMsg->msgName = SIP_RESPONSE;
	 pNewMsg->tAddr = msg->oAddr;

	 pNewMsg->ctrlMsgHdr = (PTSipCtrlMsg)msg->cloneCtrlMsg();
	 if(TSipCtrlMsg* pSipCtrl = dynamic_cast<TSipCtrlMsg*>(pNewMsg->ctrlMsgHdr)){
		 CHAR buf[33];
		 CSipMsgHelper::generateRandomNumberStr(buf);
		 pSipCtrl->to.tag = buf;
	 }

	 pNewMsg->setCtrlMsgHdr();

	 TSipResp *pSipResp = new TSipResp();
	 pSipResp->statusCode = 200;
	 pSipResp->reason_phase = "OK";
	 pNewMsg->msgBody = pSipResp;
	 pNewMsg->setMsgBody();

	 sendToDispatcher(pNewMsg);
}

bool CSipMessageModule::isResponse200OK(TUniNetMsg *msg){
	//如果该消息不是sip的response消息
	if(msg->msgType != SIP_TYPE && msg->msgName != SIP_RESPONSE)
		return false;

	if(((PTSipResp)msg->msgBody)->statusCode == 200)
		return true;
	else
		return false;
}

void CSipMessageModule::procMsg(PTUniNetMsg msg){
	printf("CSipMessageModule: current state: %s %d\n",m_fsmContext.getState().getName(),m_fsmContext.getState().getId());
	printf("recv msg from %d, msgName is %s\n", msg->oAddr.logAddr, msg->getMsgNameStr());
	switch(msg->msgName){
	case SIP_MESSAGE:
		*m_SipCtrlMsg = *((PTSipCtrlMsg)msg->ctrlMsgHdr);
		m_fsmContext.onMessage(msg);
		break;
	case SIP_RESPONSE:
		m_fsmContext.onMessage(msg);
		break;
	default:
		printf("###CSipCallModule:unknow msgName %s\n",msg->getMsgNameStr());
		break;
	}
	printf("CSipMessageModule: after procMsg state: %s %d\n",m_fsmContext.getState().getName(),m_fsmContext.getState().getId());
}

void CSipMessageModule::onTimeOut (TTimeMarkExt timerMark){
	print("The CSipMessageModule task received a timeout event: %d!!!\n", timerMark.timerId);
	errorLog("[CSipMessageModule]ERROR: The CSipMessageModule task received a timeout event: %d!!!\n", timerMark.timerId);
	m_fsmContext.onTimeOut(timerMark);
}

PTUACData CSipMessageModule::createData(){
	return new TUACData();
}

void CSipMessageModule::initState(){
	m_fsmContext.enterStartState();
}


//change by guoxun
BOOL CSipMessageModule::msgMap(TUniNetMsg *pSrcMsg, TUniNetMsg *pDestMsg){
	CVarChar128 caller;
	CVarChar128 callerHost;
	return CSipToRtc::instance()->msgMap(pSrcMsg, pDestMsg, caller, callerHost, false);
}

// 判断Xlite发过来IM的是不是Text Html
BOOL CSipMessageModule::isTextHtml(TUniNetMsg *msg){
	TSipMessage* pSipMessage = (TSipMessage*) (msg->msgBody);
	return (strcmp(pSipMessage->content_type.type.c_str(), "text") == 0)
			&& (strcmp(pSipMessage->content_type.subtype.c_str(), "html") == 0);
}

BOOL CSipMessageModule::isTextPlain(TUniNetMsg *msg){
	TSipMessage* pSipMessage = (TSipMessage*) (msg->msgBody);
	return (strcmp(pSipMessage->content_type.type.c_str(), "text") == 0)
			&& (strcmp(pSipMessage->content_type.subtype.c_str(), "plain") == 0);
}

// 将接收到的来自XLIte的Html格式的短信转换成plain格式 xlite3.0
// 对于Xlite4 Xlite发送的可能就是plain
void CSipMessageModule::convertToPlain(TUniNetMsg *msg){
	TSipMessage* pSipMessage = (TSipMessage*) (msg->msgBody);
	string plain = "";
	if(isTextHtml(msg)){
		string html = pSipMessage->body.content.c_str();
		size_t start = html.find_first_of('>') + 1;
		size_t length = html.find_last_of('<') - start;
		plain = html.substr(start, length);
	}else if(isTextPlain(msg)){
		plain = pSipMessage->body.content.c_str();
	}
	pSipMessage->content_type.subtype = "plain";
	pSipMessage->body.content = plain.c_str();
	pSipMessage->body.content_length = plain.size();
}
