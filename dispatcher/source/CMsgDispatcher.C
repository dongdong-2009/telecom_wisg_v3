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

CMsgDispatcher::CMsgDispatcher(PCGFSM afsm) :CUACTask(afsm),
	m_isrtcPsaAddrSet(FALSE), m_issipPsaAddrSet(FALSE){
	// get value from conf
	CProperties* properties = CPropertiesManager::getInstance()->getProperties("rtc.env");
	if((this->LOGADDR_SIP_PSA = properties->getIntProperty("LOGADDR_SIP_PSA")) == -1){
		this->LOGADDR_SIP_PSA = 11;
	}
	if((this->LOGADDR_RTC_PSA = properties->getIntProperty("LOGADDR_RTC_PSA")) == -1){
		this->LOGADDR_RTC_PSA = 18;
	}
	if((this->LOGADDR_SIP_CALL = properties->getIntProperty("LOGADDR_SIP_CALL")) == -1){
		this->LOGADDR_SIP_CALL = 232;
	}
	if((this->LOGADDR_RTC_CALL = properties->getIntProperty("LOGADDR_RTC_CALL")) == -1){
		this->LOGADDR_RTC_CALL = 233;
	}
	if((this->LOGADDR_SIP_IM = properties->getIntProperty("LOGADDR_SIP_IM")) == -1){
		this->LOGADDR_SIP_IM = 234;
	}
	if((this->LOGADDR_RTC_IM = properties->getIntProperty("LOGADDR_RTC_IM")) == -1){
		this->LOGADDR_RTC_IM = 235;
	}
	if((this->LOGADDR_MS_CONTROL = properties->getIntProperty("LOGADDR_MS_CONTROL")) == -1){
		this->LOGADDR_MS_CONTROL = 236;
	}

	if((this->LOGADDR_DISPATCHER = properties->getIntProperty("LOGADDR_DISPATCHER")) == -1){
		this->LOGADDR_DISPATCHER = 231;
	}
	m_pDialogCtrl = new CDialogController();

	DOMConfigurator::configureAndWatch("etc/log4cxx.xml", 5000);

	logger = log4cxx::Logger::getLogger("SgFileAppender");
}

CMsgDispatcher::~CMsgDispatcher(){
	if(m_pDialogCtrl != NULL){
		delete m_pDialogCtrl;
		m_pDialogCtrl = NULL;
	}
}

void CMsgDispatcher::procMsg(TUniNetMsg* msg){
	LOG4CXX_DEBUG(logger, "dispatcher get msg from "<<msg->oAddr.logAddr);
	if(msg->oAddr.logAddr == (UINT)LOGADDR_SIP_PSA){
		handleMsgFromSipPSA(msg);
	}else if(msg->oAddr.logAddr == (UINT)LOGADDR_RTC_PSA){
		handleMsgFromRtcPSA(msg);
	}else if(msg->oAddr.logAddr == (UINT)LOGADDR_SIP_CALL){
		handleMsgFromSipCall(msg);
	}else if(msg->oAddr.logAddr == (UINT)LOGADDR_RTC_CALL){
		handleMsgFromRtcCall(msg);
	}else if(msg->oAddr.logAddr == (UINT)LOGADDR_MS_CONTROL){
		handleMsgFromMSControl(msg);
	}
	/*else if(msg->oAddr.logAddr ==  (UINT)LOGADDR_SIP_IM){
		handleMsgFromSipIM(msg);
	}else if(msg->oAddr.logAddr ==  (UINT)LOGADDR_RTC_IM){
		handleMsgFromRtcIM(msg);
	}*/
	else{
		LOG4CXX_ERROR(logger, "###CMsgDispatcher: received an unexpected message from logaddr " << msg->oAddr.logAddr << ",This task will be ended.\n"
				);
		end();
	}

}

void CMsgDispatcher::justPassRegister(TUniNetMsg* msg) {
	printf("pass register, from: ");
	TSipVia sipVia = ((TSipCtrlMsg*)msg->ctrlMsgHdr)->via;
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

void CMsgDispatcher::handleMsgFromMSControl(TUniNetMsg * msg){
	// sip call module调用endTask时发送过来，不用再转发

	if (msg->dialogType == DIALOG_BEGIN) {
		string uniqID;
		if (msg->msgType == SIP_TYPE) {
			uniqID = generateSipUniqID(msg);
		} else { // rtc
			//			uniqID = getStoredUniqID();
			//			printf("CMsgDispatcher getSoredUniqID: %s\n", getStoredUniqID());

			PTRtcCtrlMsg pRtcCtrl = (PTRtcCtrlMsg) msg->ctrlMsgHdr;
			uniqID
					= CMsgMapHelper::getSipCallID(pRtcCtrl->offerSessionId).c_str();
		}
		if (!m_pDialogCtrl->storeDialog(uniqID, msg->oAddr)) {
			LOG4CXX_ERROR(logger, "handleMsgFromSipCall can not store "<<uniqID.c_str()<< "logdaddr: "<< msg->oAddr.logAddr);
		}
	}

	if(msg->dialogType == DIALOG_END){
		char* uniqID = generateSipUniqID(msg);
		m_pDialogCtrl->clearDialog(uniqID);
		return;
	}



	switch(msg->msgName){
	case INTERNAL_RESPONSE:{
		TMsgAddress tAddr;
		char* uniqID = generateSipUniqID(msg);
		if(m_pDialogCtrl->getDialogAddr(uniqID, tAddr)){
			sendMsgtoInstance(msg, tAddr, DIALOG_CONTINUE);
		}else{
			LOG4CXX_ERROR(logger ,"handleMsgFromMSControl can not get addr by "<< uniqID);
		}
		break;
	}
	case INT_CLOSE:{
		TMsgAddress tAddr;
		char* uniqID = generateSipUniqID(msg);
		if(m_pDialogCtrl->getDialogAddr(uniqID, tAddr)){
			sendMsgtoInstance(msg, tAddr, DIALOG_CONTINUE);
		}else{
			LOG4CXX_ERROR(logger ,"handleMsgFromMSControl: Can not get addr by "<< uniqID);
		}
		break;
	}
	case SIP_INVITE:
	case SIP_RESPONSE:
	case SIP_BYE:
	case SIP_INFO:
	case SIP_ACK:{
		//发送给SIP-PSA
		sendMsgtoInstance(msg, getSipPsaAddr(), DIALOG_CONTINUE);
		break;
	}

	default:
		LOG4CXX_ERROR(logger, "handleMsgFromMSControl: received unknown msgName "<<msg->getMsgName());
		break;
	}
}


void CMsgDispatcher::handleMsgFromSipPSA(TUniNetMsg* msg){
	//CTUniNetMsgHelper::print(msg);

	// 记录sippsa地址
	if(!m_issipPsaAddrSet){
		m_sipPsaAddr = msg->oAddr;
		m_issipPsaAddrSet = TRUE;
	}

	// 注意，实现需要这里修改type = rtc-type，参见文档
	msg->msgType = RTC_TYPE;

	TMsgAddress tAddr;
	switch(msg->msgName){
	case SIP_REGISTER:
		justPassRegister(msg);
		break;
	case SIP_INVITE:
		tAddr.logAddr = LOGADDR_SIP_CALL;
		tAddr.phyAddr = 0;
		tAddr.taskInstID = 0;
		// think about re-invite from xlite
		if(m_pDialogCtrl->getDialogAddr(generateSipUniqID(msg), tAddr)){
			LOG4CXX_DEBUG(logger, "CMsgDispatcher::handleMsgFromSipPSA this is a reinvite\n");
			sendMsgtoInstance(msg, tAddr, DIALOG_CONTINUE);
		}else{
//			storeUniqID(msg, SIP_TYPE);
			sendMsgtoInstance(msg, tAddr, DIALOG_BEGIN);
		}
		break;
	case SIP_MESSAGE:
		tAddr.logAddr = LOGADDR_SIP_IM;
		tAddr.phyAddr = 0;
		tAddr.taskInstID = 0;
//		storeUniqID(msg, SIP_TYPE);
		sendMsgtoInstance(msg, tAddr, DIALOG_BEGIN);
		break;
	case SIP_ACK:
	case SIP_CANCEL:
	case SIP_INFO:
	case SIP_UPDATE:
	case SIP_BYE:
	case SIP_RESPONSE:
	{
		char* uniqID = generateSipUniqID(msg);
		if(m_pDialogCtrl->getDialogAddr(uniqID, tAddr)){
			sendMsgtoInstance(msg, tAddr, DIALOG_CONTINUE);
		}else{
			LOG4CXX_ERROR(logger, "handleMsgFromSipPSA can not get addr by "<< uniqID);
		}
		break;
	}
	default:
		LOG4CXX_ERROR(logger, "received unknown msgName "<<msg->getMsgName());
		break;
	}
}

void CMsgDispatcher::handleMsgFromSipCall(TUniNetMsg* msg){

	if(msg->dialogType == DIALOG_BEGIN){
		string uniqID;
		if(msg->msgType == SIP_TYPE){
			uniqID = generateSipUniqID(msg);
		}else{	// rtc
//			uniqID = getStoredUniqID();
//			printf("CMsgDispatcher getSoredUniqID: %s\n", getStoredUniqID());

			PTRtcCtrlMsg pRtcCtrl = (PTRtcCtrlMsg)msg->ctrlMsgHdr;
			uniqID = CMsgMapHelper::getSipCallID(pRtcCtrl->offerSessionId).c_str();
		}
		if(!m_pDialogCtrl->storeDialog(uniqID, msg->oAddr)){
			LOG4CXX_ERROR(logger, "handleMsgFromSipCall can not store "<<uniqID.c_str()<< "logdaddr: "<< msg->oAddr.logAddr);
		}
	}

	// sip call module调用endTask时发送过来，不用再转发
	if(msg->dialogType == DIALOG_END){
		char* uniqID = generateSipUniqID(msg);
		m_pDialogCtrl->clearDialog(uniqID);
		return;
	}

	if(msg->msgType == SIP_TYPE){
		if(msg->msgName = INT_REQUEST)
		{//发送给MS CONTROL模块
			TMsgAddress tAddr;
			tAddr.logAddr = LOGADDR_MS_CONTROL;
			tAddr.phyAddr = 0;
			tAddr.taskInstID = 0;
			sendMsgtoInstance(msg, tAddr, DIALOG_BEGIN);
		}
		else{
			//发送给SIP-PSA
			sendMsgtoInstance(msg, getSipPsaAddr(), DIALOG_CONTINUE);
		}
		return;
	}

	// 原始消息来自sip，发给rtc call module
	if(msg->msgType == RTC_TYPE){
		TMsgAddress tAddr;
		char* uniqID = generateRtcUniqID(msg);
		if(m_pDialogCtrl->getDialogAddr(uniqID, tAddr))
		{
			sendMsgtoInstance(msg, tAddr, DIALOG_CONTINUE);
		}
		else if(msg->msgName == RTC_OFFER)
		{
			tAddr.phyAddr = 0;
			tAddr.taskInstID = 0;
			tAddr.logAddr = LOGADDR_RTC_CALL;
			sendMsgtoInstance(msg, tAddr, DIALOG_BEGIN);
		}else{
			printf("handleMsgFromSipCall can not get addr by %s", uniqID);
		}

	}
}

/* 判断是否有已经建立会话，无则记录，DIALOG—BEGIN，否则DIALOG_CONTINUE
 */
void CMsgDispatcher::handleMsgFromRtcPSA(TUniNetMsg* msg){
	printf("Msg from RTC Psa, roap Type is %s\n", msg->getMsgNameStr());
	CTUniNetMsgHelper::print(msg);

	// 记录rtcpsa地址
	if(!m_isrtcPsaAddrSet){
		m_rtcPsaAddr = msg->oAddr;
		m_isrtcPsaAddrSet = TRUE;
	}

	// 注意，实现需要这里修改type = sip-type，参见文档
	msg->msgType = SIP_TYPE;

	// 获取地址
	TMsgAddress tAddr;
	if(msg->msgName == RTC_OFFER || msg->msgName == RTC_IM){
		tAddr.phyAddr = 0;
		tAddr.taskInstID = 0;
		if(msg->msgName == RTC_OFFER){
			tAddr.logAddr = LOGADDR_RTC_CALL;
		}else{	// IM
			tAddr.logAddr = LOGADDR_RTC_IM;
		}
//		storeUniqID(msg, RTC_TYPE);
		sendMsgtoInstance(msg, tAddr, DIALOG_BEGIN);
	}else{
		char* uniqID = generateRtcUniqID(msg);
		if(!m_pDialogCtrl->getDialogAddr(uniqID, tAddr)){
			printf("handleMsgFromRtcPSA can not get addr by %s", uniqID);
			errorLog("handleMsgFromRtcPSA can not get addr by %s", uniqID);
		}else{
			sendMsgtoInstance(msg, tAddr, DIALOG_CONTINUE);
		}
	}
}


/** 处理来自rtc call module 的消息，需要决定发送给rtcPSA还是sip call(im) module
 * 如果dialogType 为 DIALOG_BEGIN，那么需要添加会话记录
 * 如果dialogType 为 DIALOG_END，那么需要清除会话记录
 */
void CMsgDispatcher::handleMsgFromRtcCall(TUniNetMsg* msg){
	printf("DISPATCHER, MSG FROM RTC CALL MODULE\n");
	CTUniNetMsgHelper::print(msg);

	if(msg->dialogType == DIALOG_BEGIN){
		string uniqID;
		if(msg->msgType == SIP_TYPE){
//			uniqID = getStoredUniqID();
//			printf("CMsgDispatcher getSoredUniqID: %s\n", getStoredUniqID());

			PTSipCtrlMsg pSipCtrl = (PTSipCtrlMsg)msg->ctrlMsgHdr;
			uniqID = pSipCtrl->from.tag.c_str();
			printf("CMsgDispatcher getRtcSessionID: %s\n", uniqID.c_str());
		}else {// msg->msgType == RTC_TYPE
			uniqID = generateRtcUniqID(msg);
		}
		if(!m_pDialogCtrl->storeDialog(uniqID, msg->oAddr)){
			printf("handleMsgFromRtcCall can not store %s, logdaddr: %d\n", uniqID.c_str(), msg->oAddr.logAddr);
			errorLog("handleMsgFromRtcCall can not store %s, logdaddr: %d\n", uniqID.c_str(), msg->oAddr.logAddr);
		}
	}

	// rtc call module调用endTask时发送过来，不用再转发
	if(msg->dialogType == DIALOG_END){
		char* uniqID = generateRtcUniqID(msg);
		m_pDialogCtrl->clearDialog(uniqID);
		return;
	}

	// 原始消息来自sip，发给RtcPSA
	if(msg->msgType == RTC_TYPE){
		sendMsgtoInstance(msg, getRtcPsaAddr(), DIALOG_CONTINUE);
		return;
	}

	// 原始消息来自rtc，发给sip call module
	if(msg->msgType == SIP_TYPE){
		TMsgAddress tAddr;
		if(msg->msgName == SIP_INVITE){
			tAddr.phyAddr = 0;
			tAddr.taskInstID = 0;
			tAddr.logAddr = LOGADDR_SIP_CALL;
			sendMsgtoInstance(msg, tAddr, DIALOG_BEGIN);
		}else{
			char* uniqID = generateSipUniqID(msg);
			if(m_pDialogCtrl->getDialogAddr(uniqID, tAddr)){
				sendMsgtoInstance(msg, tAddr, DIALOG_CONTINUE);
			}else{
				printf("handleMsgFromRtcCall can not get addr by %s", uniqID);
			}
		}
	}

}

/** 填写消息的地址并发送到 instAddr 指定的模块去
 *
 */
void CMsgDispatcher::sendMsgtoInstance(TUniNetMsg* msg, TMsgAddress instAddr, TDialogType dialogtype){
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

char* CMsgDispatcher::generateSipUniqID(TUniNetMsg* msg) {
	TSipCtrlMsg* pSipCtrl = (TSipCtrlMsg*) (msg->ctrlMsgHdr);
	return pSipCtrl->sip_callId.number.c_str();
}

char* CMsgDispatcher::generateRtcUniqID(TUniNetMsg* msg) {
	PTRtcCtrlMsg pCtrl = (PTRtcCtrlMsg) msg->ctrlMsgHdr;
	return pCtrl->offerSessionId.c_str();
}

void CMsgDispatcher::onTimeOut ( TTimerKey timerKey,  TTimeMarkExt timerMark,  void* para){

}
void CMsgDispatcher::onTimeUpdate( TTimeMarkExt timerMark,  TTimerKey oldTimerKey, TTimerKey  newTimerKey, void* para){

}

TMsgAddress CMsgDispatcher::getRtcPsaAddr(){
	if(m_isrtcPsaAddrSet)
		return m_rtcPsaAddr;
	TMsgAddress tAddr;
	tAddr.phyAddr = 0;
	tAddr.taskInstID = 0;
	tAddr.logAddr = LOGADDR_RTC_PSA;
	return tAddr;
}

TMsgAddress CMsgDispatcher::getSipPsaAddr(){
	if(m_issipPsaAddrSet)
		return m_sipPsaAddr;
	TMsgAddress tAddr;
	tAddr.phyAddr = 0;
	tAddr.taskInstID = 0;
	tAddr.logAddr = LOGADDR_SIP_PSA;
	return tAddr;
}


/*
void CMsgDispatcher::handleMsgFromSipIM(TUniNetMsg* msg){
	CTUniNetMsgHelper::print(msg);

	if(msg->dialogType == DIALOG_BEGIN){
		string uniqID;
		if(msg->msgType == SIP_TYPE){
			uniqID = generateSipUniqID(msg);
		}else{	// rtc
	//		uniqID = getStoredUniqID();
//			printf("CMsgDispatcher getSoredUniqID: %s\n", getStoredUniqID());

			PTRtcCtrlMsg pRtcCtrl = (PTRtcCtrlMsg)msg->ctrlMsgHdr;
			uniqID = CMsgMapHelper::getSipCallID(pRtcCtrl->offerSessionId).c_str();

			printf("CMsgDispatcher getSipCallIDNumber: %s\n", uniqID.c_str());
		}
		if(!m_pDialogCtrl->storeDialog(uniqID, msg->oAddr)){
			printf("handleMsgFromSipIM can not store %s, logdaddr: %d", uniqID.c_str(), msg->oAddr.logAddr);
			errorLog("handleMsgFromSipIM can not store %s, logdaddr: %d", uniqID.c_str(), msg->oAddr.logAddr);
		}
	}

	// sip module调用endTask时发送过来，不用再转发
	if(msg->dialogType == DIALOG_END){
		char* uniqID = generateSipUniqID(msg);
		m_pDialogCtrl->clearDialog(uniqID);
		return;
	}

	// 原始消息来自rtc，发给SipPSA
	if(msg->msgType == SIP_TYPE){
		sendMsgtoInstance(msg, getSipPsaAddr(), DIALOG_CONTINUE);
		return;
	}

	// 原始消息来自sip，发给rtc im module
	if(msg->msgType == RTC_TYPE){
		TMsgAddress tAddr;
		if(msg->msgName == RTC_IM){
			tAddr.phyAddr = 0;
			tAddr.taskInstID = 0;
			tAddr.logAddr = LOGADDR_RTC_IM;
			sendMsgtoInstance(msg, tAddr, DIALOG_BEGIN);
		}else{
			char* uniqID = generateRtcUniqID(msg);
			if(m_pDialogCtrl->getDialogAddr(uniqID, tAddr)){
				sendMsgtoInstance(msg, tAddr, DIALOG_CONTINUE);
			}else{
				printf("handleMsgFromSipIM can not get addr by %s", uniqID);
			}
		}
	}
}
*/

/*
void CMsgDispatcher::handleMsgFromRtcIM(TUniNetMsg* msg){
	CTUniNetMsgHelper::print(msg);

	if(msg->dialogType == DIALOG_BEGIN){
		string uniqID;
		if(msg->msgType == SIP_TYPE){
//			uniqID = getStoredUniqID();
//			printf("CMsgDispatcher getSoredUniqID: %s\n", getStoredUniqID());

			PTSipCtrlMsg pSipCtrl = (PTSipCtrlMsg)msg->ctrlMsgHdr;
			uniqID = pSipCtrl->from.tag.c_str();
			printf("CMsgDispatcher getRtcSessionID: %s\n", uniqID.c_str());
		}else {// msg->msgType == RTC_TYPE
			uniqID = generateRtcUniqID(msg);
		}
		if(!m_pDialogCtrl->storeDialog(uniqID, msg->oAddr)){
			printf("handleMsgFromRtcIM can not store %s, logdaddr: %d\n", uniqID.c_str(), msg->oAddr.logAddr);
			errorLog("handleMsgFromRtcIM can not store %s, logdaddr: %d\n", uniqID.c_str(), msg->oAddr.logAddr);
		}
	}

	// rtc call module调用endTask时发送过来，不用再转发
	if(msg->dialogType == DIALOG_END){
		char* uniqID = generateRtcUniqID(msg);
		m_pDialogCtrl->clearDialog(uniqID);
		return;
	}

	// 原始消息来自sip，发给RtcPSA
	if(msg->msgType == RTC_TYPE){
		sendMsgtoInstance(msg, getRtcPsaAddr(), DIALOG_CONTINUE);
		return;
	}

	// 原始消息来自rtc，发给sip call module
	if(msg->msgType == SIP_TYPE){
		TMsgAddress tAddr;
		if(msg->msgName == SIP_MESSAGE){
			tAddr.phyAddr = 0;
			tAddr.taskInstID = 0;
			tAddr.logAddr = LOGADDR_SIP_IM;
			sendMsgtoInstance(msg, tAddr, DIALOG_BEGIN);
		}else{
			char* uniqID = generateSipUniqID(msg);
			if(m_pDialogCtrl->getDialogAddr(uniqID, tAddr)){
				sendMsgtoInstance(msg, tAddr, DIALOG_CONTINUE);
			}else{
				printf("handleMsgFromRtcIM can not get addr by %s", uniqID);
			}
		}
	}
}
*/
