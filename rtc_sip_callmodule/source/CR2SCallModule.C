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
		this->m_accessMode = accessMode;
	}

	log4cxx::xml::DOMConfigurator::configureAndWatch("etc/log4cxx.xml", 5000);

	logger = log4cxx::Logger::getLogger("SgFileAppender");

	m_rtcCtrlMsg = new TRtcCtrlMsg();
	m_intCtrlMsg_Rtc = NULL;
	m_intCtrlMsg_Sip = NULL;

	m_isSdpConfirmed = false;
	m_sipCtrlMsg = NULL;
	m_sdpModifyFlag = 0;
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
		m_sipName = result->pRows->arrayField[0].value.stringValue;
		sprintf(
				pcSQLStatement,
				"UPDATE user_map_table SET rtcname = '%s', isCalling = 1 WHERE sipname = '%s'",
				rtcname.c_str(), m_sipName.c_str());
		CDB::instance()->execSQL(pcSQLStatement);

		LOG4CXX_INFO(logger, "selectSipUser: rtcname: "<<rtcname<<" map to sipname: "<<m_sipName);
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
	case SIP_RESPONSE:{
		PTSipCtrlMsg ctrlMsg = (PTSipCtrlMsg) msg->ctrlMsgHdr;

		break;
	}
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
	if(m_intCtrlMsg_Rtc == NULL){
		m_intCtrlMsg_Rtc->from = "rtc_call";
		m_intCtrlMsg_Rtc->to = "bear";
		m_intCtrlMsg_Rtc->sessionId = m_offerSessionId;
		m_intCtrlMsg_Rtc->intType = INT_REQUEST;
	}

	PTIntRequest pReq = new TIntRequest();
	pReq->body = m_webSdp.c_str();

	sendToDispatcher(INT_REQUEST, INT_TYPE, DIALOG_CONTINUE, m_intCtrlMsg_Rtc->clone(), pReq);
}

void CR2SCallModule::sendCloseToBear_Rtc()
{
	if(m_intCtrlMsg_Rtc != NULL)
	{
		PTIntClose pClose = new TIntClose();
		sendToDispatcher(INT_CLOSE, INT_TYPE, DIALOG_CONTINUE, m_intCtrlMsg_Rtc->clone(), pReq);
	}
}




char * CR2SCallModule::getUserName(const string& user){

	int i = user.find('@');
	if(i != -1)
		return user.substr(0, i).c_str();
	else
		return user.c_str();
}

char * CR2SCallModule::getHost(const string& user){
	int i = user.find('@');
	if(i != -1)
		return user.substr(i + 1).c_str();
	else
		return NULL;
}

void CR2SCallModule::sendNoSdpInviteToIMS(){
	if(m_sipCtrlMsg == NULL){
		m_sipCtrlMsg = new TSipCtrlMsg();
		m_sipCtrlMsg->sip_callId.number = CMsgMapHelper::generateSipCallIDNumber(m_rtcCtrlMsg->offerSessionId);

		if(m_accessMode == 1 || m_accessMode == 2){
			m_sipCtrlMsg->from.displayname = getUserName(m_sipName);
			if(getHost(m_sipName) != NULL)
			{
				m_sipCtrlMsg->from.url = CSipMsgHelper::createSipURI("sip", getUserName(m_sipName), getHost(m_sipName), NULL);
			}
			else{
				m_sipCtrlMsg->from.url = CSipMsgHelper::createSipURI("tel", getUserName(m_sipName), NULL, NULL);
			}
		}
		else{
			string from = m_rtcCtrlMsg->from.c_str();
			m_sipCtrlMsg->from.displayname = getUserName(from);
			m_sipCtrlMsg->from.url = CSipMsgHelper::createSipURI("sip", getUserName(from), getHost(from), NULL);
		}

		m_sipCtrlMsg->from.tag = m_rtcCtrlMsg->offerSessionId;

		string toStr  = m_rtcCtrlMsg->to.c_str();
		m_sipCtrlMsg->to.displayname = getUserName(toStr);
		m_sipCtrlMsg->to.url = CSipMsgHelper::createSipURI("tel", getUserName(m_sipName), NULL, NULL);//tel后不能有域名
	}

	//协议栈自动添加
//	char str[64];
//	sprintf(str, "%d", pRtcOffer->seq);
//	m_sipCtrlMsg->cseq_number = str;
//	m_sipCtrlMsg->cseq_method = "INVITE";

	PTSipInvite pInvite = new TSipInvite();
	pInvite->req_uri = m_sipCtrlMsg->to.url;

	sendToDispatcher(SIP_INVITE, SIP_TYPE, DIALOG_BEGIN, m_sipCtrlMsg, pInvite);
}
//	void sendNoSdpInviteToIMS();
//	void sendCancelToIMS();
//	void sendPrackToIMS(TUniNetMsg * msg);
//	void sendAckToIMS(TUniNetMsg * msg);
//	void send200OKForUpdateToIMS(TUniNetMsg * msg);
//	void send200OKForInviteToIMS(TUniNetMsg * msg);
//	void sendByeToIMS();


void CR2SCallModule::sendCancelToIMS(){
	PTSipCancel pCancel = new TSipCancel();
	pCancel->req_uri = m_sipCtrlMsg->to.url;

	sendToDispatcher(SIP_CANCEL,SIP_TYPE, DIALOG_CONTINUE, m_sipCtrlMsg->clone(), pCancel);
}

void CR2SCallModule::sendPrackToIMS(TUniNetMsg * msg){
	PTSipPrack pPrack = new TSipPrack();
	pPrack->req_uri = m_sipCtrlMsg->to.url;

	if(msg!= NULL){
		PTIntResponse pResp = (PTIntResponse)msg->msgBody;
		pPrack->content_type.type = "application";
		pPrack->content_type.subtype = "sdp";
		pPrack->body.content = pResp->body;
		pPrack->body.content_length = pResp->body.length();
	}

	sendToDispatcher(SIP_PRACK, SIP_TYPE, DIALOG_CONTINUE, m_sipCtrlMsg->clone(), pPrack);
}

void CR2SCallModule::sendAckToIMS(TUniNetMsg * msg){
	PTSipReq pAck = new TSipReq();
	pAck->req_uri = m_sipCtrlMsg->to.url;

	if(msg != NULL){
		PTIntResponse pResp = (PTIntResponse)msg->msgBody;
		pAck->content_type.type = "application";
		pAck->content_type.subtype = "sdp";
		pAck->body.content = pResp->body;
		pAck->body.content_length = pResp->body.length();
	}

	sendToDispatcher(SIP_ACK, SIP_TYPE, DIALOG_CONTINUE, m_sipCtrlMsg->clone(), pAck);
}

void CR2SCallModule::send200OKForUpdateToIMS(TUniNetMsg * msg){
	PTSipResp pAns = new PTSipResp();
	pAns->statusCode = 200;
	pAns->reason_phase = "OK";

	if(msg != NULL){
		PTIntResponse pResp = (PTIntResponse)msg->msgBody;
		pAns->content_type.type = "application";
		pAns->content_type.subtype = "sdp";
		pAns->body.content = pResp->body;
		pAns->body.content_length = pResp->body.length();
	}

	PTSipCtrlMsg ctrlMsg = m_sipCtrlMsg->clone();

	ctrlMsg->cseq_method = "UPDATE";

	sendToDispatcher(SIP_RESPONSE, SIP_TYPE, DIALOG_CONTINUE, m_sipCtrlMsg->clone(), pAns);
}


void CR2SCallModule::send200OKForInviteToIMS(TUniNetMsg * msg){
	PTSipResp pAns = new PTSipResp();
	pAns->statusCode = 200;
	pAns->reason_phase = "OK";

	if(msg != NULL){
		PTIntResponse pResp = (PTIntResponse)msg->msgBody;
		pAns->content_type.type = "application";
		pAns->content_type.subtype = "sdp";
		pAns->body.content = pResp->body;
		pAns->body.content_length = pResp->body.length();
	}

	PTSipCtrlMsg ctrlMsg = m_sipCtrlMsg->clone();
	ctrlMsg->cseq_method = "INVITE";
	sendToDispatcher(SIP_RESPONSE, SIP_TYPE, DIALOG_CONTINUE, m_sipCtrlMsg->clone(), pAns);
}

void CR2SCallModule::sendByeToIMS()
{
	PTSipBye pBye = new PTSipBye();
	pBye->req_uri = m_sipCtrlMsg->to.url;

	sendToDispatcher(SIP_BYE, SIP_TYPE, DIALOG_CONTINUE, m_sipCtrlMsg->clone(), pBye);
}


//void sendReqToBear_Sip();
//void sendCloseToBear_Sip();
void CR2SCallModule::sendReqToBear_Sip(){
	if(m_intCtrlMsg_Sip == NULL){
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

	sendToDispatcher(INT_REQUEST, INT_TYPE, DIALOG_CONTINUE, m_intCtrlMsg_Sip->clone(), pReq);
}

void CR2SCallModule::sendCloseToBear_Sip()
{
	if(m_intCtrlMsg_Sip != NULL)
	{
		PTIntClose pClose = new TIntClose();
		sendToDispatcher(INT_CLOSE, INT_TYPE, DIALOG_CONTINUE, m_intCtrlMsg_Sip->clone(), pReq);
	}
}


void CR2SCallModule::isResp1xx(TUniNetMsg * msg){
	if (msg->msgType != SIP_TYPE && msg->msgName != SIP_RESPONSE)
		return false;

	INT statusCode = ((PTSipResp) msg->msgBody)->statusCode;
	if (statusCode / 100 == 1)
		return true;
	else
		return false;
}

void CR2SCallModule::isResp2xx(TUniNetMsg * msg){
	if (msg->msgType != SIP_TYPE && msg->msgName != SIP_RESPONSE)
		return false;

	if (((PTSipResp) msg->msgBody)->statusCode == 200)
		return true;
	else
		return false;
}

void CR2SCallModule::isResp3xx_6xx(TUniNetMsg * msg){
	if (msg->msgType != SIP_TYPE && msg->msgName != SIP_RESPONSE)
		return false;

	INT statusCode = ((PTSipResp) msg->msgBody)->statusCode;
	if (statusCode < 700 && statusCode > 300)
		return true;
	else
		return false;
}

void CR2SCallModule::isWithSDP(TUniNetMsg *msg){
	if (msg->msgType != SIP_TYPE)
		return false;

	if(msg->msgName == SIP_RESPONSE)
	{
		PTSipResp pResp = (PTSipResp) msg->msgBody;
		return pResp->body.content_length == 0;
	}

	if(msg->msgName == SIP_UPDATE)
	{
		PTSipUpdate pUpdate = (PTSipUpdate) msg->msgBody;
		return pUpdate->body.content_length == 0;
	}

	if(msg->msgName == SIP_INVITE)
	{
		PTSipInvite pInvite = (PTSipInvite) msg->msgBody;
		return pInvite->body.content_length == 0;
	}
	return false;
}

void CR2SCallModule::notifySipTermCallSdp(TUniNetMsg * msg) {
	//onNotify(NULL);
}

void CR2SCallModule::notifySipTermClose() {
	//onClose(NULL)
}

//void notifyRtcOrigCallSdp();
//void notifyRtcOrigCallError(TUniNetMsg * msg);
//void notifyRtcOrigCallClose();
//void notifyRtcOrigCallError(const int errorType);

void CR2SCallModule::notifyRtcOrigCallSdp(){

}

void CR2SCallModule::notifyRtcOrigCallError(TUniNetMsg * msg){

}

void CR2SCallModule::notifyRtcOrigCallClose(){

}

void CR2SCallModule::notifyRtcOrigCallError(const int errorType){

}

