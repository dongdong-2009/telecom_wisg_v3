#include "CRtcMessageModule.h"

CLONE_COMP(CRtcMessageModule)
CREATE_COMP(CRtcMessageModule)

CRtcMessageModule::CRtcMessageModule(PCGFSM afsm) :CUACSTask(afsm),
	m_fsmContext(*this), m_isDispatcherAddrSet(FALSE){
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
	m_RtcCtrlMsg = new TRtcCtrlMsg();
}

CRtcMessageModule::~CRtcMessageModule(){
	if(m_RtcCtrlMsg){
		delete m_RtcCtrlMsg;
		m_RtcCtrlMsg = NULL;
	}
}

PTUACData CRtcMessageModule::createData(){
	return new TUACData();
}

void CRtcMessageModule::initState(){
	m_fsmContext.enterStartState();
}

void CRtcMessageModule::sendToDispatcher(TUniNetMsg *pMsg){
	printf("RTC MESSAGE MODULE send msg to dispatcher, dialogtype: %d, and msgType: %d\n",
			pMsg->dialogType, pMsg->msgType);
	// 也许该记录下dispatcher的地址
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
		sendMsg(pCopyMsg);
		return;
	}

	if(pMsg->msgType == SIP_TYPE){
		TUniNetMsg *pNewMsg = new TUniNetMsg();
		msgMap(pMsg, pNewMsg);
		sendMsg(pNewMsg);
	}
}

void CRtcMessageModule::endTask(){
	printf("endTask current state: %s %d\n",m_fsmContext.getState().getName(),m_fsmContext.getState().getId());
	// 产生一条DIALOG——END消息，发给dispatcher，清除会话信息
	TUniNetMsg *pMsg = new TUniNetMsg();
	pMsg->dialogType = DIALOG_END;
	pMsg->msgType = RTC_TYPE;	// 无所谓，先判断dialogType
	pMsg->msgName = RTC_OK;
	pMsg->ctrlMsgHdr = m_RtcCtrlMsg->clone();
	pMsg->setCtrlMsgHdr();

	sendToDispatcher(pMsg);
	end();
	printf("endTask finish\n");
}

void CRtcMessageModule::procMsg(PTUniNetMsg msg){
	if(!m_isDispatcherAddrSet){
		m_dispatcherAddr = msg->oAddr;
		m_isDispatcherAddrSet = TRUE;
	}
	printf("current state: %s %d\n",m_fsmContext.getState().getName(),m_fsmContext.getState().getId());
	printf("recv msg from %d, and msgName %s\n", msg->oAddr.logAddr, msg->getMsgNameStr());
	switch(msg->msgName){
	case RTC_IM:
		*m_RtcCtrlMsg = *((PTRtcCtrlMsg)msg->ctrlMsgHdr);
		m_fsmContext.onMessage(msg);
		break;
	default:
		printf("###CRtcCallModule:unknow msgName %s\n",msg->getMsgNameStr());
		//收到非法消息.
		endTask();
		break;
	}
	printf("after procMsg state: %s %d\n",m_fsmContext.getState().getName(),m_fsmContext.getState().getId());
}

void CRtcMessageModule::onTimeOut (TTimeMarkExt timerMark){
	print("The CRtcMessageModule task received a timeout event: %d!!!\n", timerMark.timerId);
	errorLog("[CRtcMessageModule]ERROR: The CRtcMessageModule task received a timeout event: %d!!!\n", timerMark.timerId);
//	m_fsmContext.onTimeOut(timerMark);
}

BOOL CRtcMessageModule::msgMap(TUniNetMsg *pSrcMsg, TUniNetMsg *pDestMsg){
	CVarChar128 nouse;
	return CRtcToSip::instance()->msgMap(pSrcMsg, pDestMsg, nouse, false);
}
