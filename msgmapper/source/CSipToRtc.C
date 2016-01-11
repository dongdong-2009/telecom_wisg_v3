#include "CSipToRtc.h"
#include "CTUniNetMsgHelper.h"

CSipToRtc* CSipToRtc::_instance = NULL;
CSipToRtc* CSipToRtc::instance(){
	if(_instance == NULL)
		_instance = new CSipToRtc();
	return _instance;
}
CSipToRtc::CSipToRtc(){
	CProperties* properties = CPropertiesManager::getInstance()->getProperties("gateway.env");
	int accessMode = properties->getIntProperty("accessMode");
	if(accessMode == -1){
		this->m_accessMode = 0;
	}
	else{
		this->m_accessMode = accessMode;
	}
}

void CSipToRtc::mapToRtcOffer(TUniNetMsg* pSrcMsg, TUniNetMsg* pDestMsg,
		TRtcCtrlMsg* pRtcCtrl, TSipCtrlMsg* pSipCtrl, bool isReCall) {

//	pRtcCtrl->from = pSipCtrl->from.url.username;
//	pRtcCtrl->to = pSipCtrl->to.url.username;

	pDestMsg->msgName = RTC_OFFER;
	if(m_accessMode == 1 || m_accessMode == 2){
		CVarChar128 oldname = combineUserNameAndHost(pSipCtrl->to.url);
		CVarChar128 newname = CUserMapHelper::getMapRtcUser(oldname);
		pRtcCtrl->to = newname;
	}
	else{
		pRtcCtrl->to = combineUserNameAndHost(pSipCtrl->to.url);
	}
	pRtcCtrl->from = combineUserNameAndHost(pSipCtrl->from.url);
	
	if(!isReCall){
		pRtcCtrl->offerSessionId = pSipCtrl->from.tag;

	}
	else{

		pRtcCtrl->offerSessionId = pSipCtrl->to.tag;
		pRtcCtrl->answerSessionId = pSipCtrl->from.tag;

		CMsgMapHelper::clearRtcToSipMap(pSipCtrl->sip_callId.number);
	}

	CMsgMapHelper::storeSipCallID(pRtcCtrl->offerSessionId, pSipCtrl->sip_callId.number);

	//		pRtcCtrl->answerSessionId = pSipCtrl->to.tag;
	pDestMsg->ctrlMsgHdr = pRtcCtrl;
	pDestMsg->setCtrlMsgHdr();

	TSipInvite* pSipInvite = (TSipInvite*) (pSrcMsg->msgBody);
	TRtcOffer* pRtcOffer = new TRtcOffer();
	pRtcOffer->seq = atoi(pSipCtrl->cseq_number.c_str());
	pRtcOffer->sdp = pSipInvite->body.content;
	pDestMsg->msgBody = pRtcOffer;
	pDestMsg->setMsgBody();
}

void CSipToRtc::mapToRtcOk(TUniNetMsg* pSrcMsg, TUniNetMsg* pDestMsg, TRtcCtrlMsg* pRtcCtrl,
		TSipCtrlMsg* pSipCtrl, bool isReCall) {
	pDestMsg->msgName = RTC_OK;

//	pRtcCtrl->from = pSipCtrl->from.url.username;
//	pRtcCtrl->to = pSipCtrl->to.url.username;
	if(m_accessMode == 1 || m_accessMode == 2)
	{
		CVarChar128 sipname = combineUserNameAndHost(pSipCtrl->to.url);
		CVarChar128 rtcname = CUserMapHelper::getMapRtcUser(sipname);
		pRtcCtrl->to = rtcname;
	}
	else
	{
		pRtcCtrl->to = combineUserNameAndHost(pSipCtrl->to.url);
	}


	pRtcCtrl->from = combineUserNameAndHost(pSipCtrl->from.url);
	if(!isReCall){
		pRtcCtrl->offerSessionId = pSipCtrl->from.tag;
		pRtcCtrl->answerSessionId = pSipCtrl->to.tag;
	}else{
		pRtcCtrl->offerSessionId = pSipCtrl->to.tag;
		pRtcCtrl->answerSessionId = pSipCtrl->from.tag;
	}




	pDestMsg->ctrlMsgHdr = pRtcCtrl;
	pDestMsg->setCtrlMsgHdr();

	TSipReq* pSipACK = (TSipReq*) (pSrcMsg->msgBody);
	TRtcOK* pRtcOk = new TRtcOK();
	pRtcOk->seq = atoi(pSipCtrl->cseq_number.c_str());
	pRtcOk->sdp = pSipACK->body.content;

	pDestMsg->msgBody = pRtcOk;
	pDestMsg->setMsgBody();
}

//change by guoxun
void CSipToRtc::maoToRtcShutdown(TUniNetMsg* pDestMsg, TRtcCtrlMsg* pRtcCtrl,
		TSipCtrlMsg* pSipCtrl, const CVarChar128& caller,
		const CVarChar128& callerHost, bool isReCall) {
	pDestMsg->msgName = RTC_SHUTDOWN;

//	pRtcCtrl->from = pSipCtrl->from.url.username;
//	pRtcCtrl->to = pSipCtrl->to.url.username;
	pRtcCtrl->from = combineUserNameAndHost(pSipCtrl->from.url);
	if(m_accessMode == 1 || m_accessMode == 2){
		CVarChar128 sipname = combineUserNameAndHost(pSipCtrl->to.url);
		CVarChar128 rtcname = CUserMapHelper::getMapRtcUser(sipname);
		pRtcCtrl->to = rtcname;
		CUserMapHelper::resetCalling(sipname);
	}
	else{
		pRtcCtrl->to = combineUserNameAndHost(pSipCtrl->to.url);
	}

	if(!isReCall){
		// 需要区分挂断电话者是不是会话的发起者
		//change by guoxun
		if ((pSipCtrl->from.url.username == caller)&&(pSipCtrl->from.url.host==callerHost)) {
			pRtcCtrl->offerSessionId = pSipCtrl->from.tag;
			// 注意：cancel的时候没有answerSessionId，需要考虑
			pRtcCtrl->answerSessionId = pSipCtrl->to.tag;
		} else {
			pRtcCtrl->offerSessionId = pSipCtrl->to.tag;
			pRtcCtrl->answerSessionId = pSipCtrl->from.tag;
		}
	}
	else{
		//change by guoxun
		if ((pSipCtrl->from.url.username == caller)&&(pSipCtrl->from.url.host==callerHost)) {
			pRtcCtrl->offerSessionId = pSipCtrl->to.tag;
			// 注意：cancel的时候没有answerSessionId，需要考虑
			pRtcCtrl->answerSessionId = pSipCtrl->from.tag;
		} else {
			pRtcCtrl->offerSessionId = pSipCtrl->from.tag;
			pRtcCtrl->answerSessionId = pSipCtrl->to.tag;
		}

	}

	pDestMsg->ctrlMsgHdr = pRtcCtrl;
	pDestMsg->setCtrlMsgHdr();
	TRtcShutdown* pRtcShutdown = new TRtcShutdown;
	pRtcShutdown->seq = atoi(pSipCtrl->cseq_number.c_str());
	pDestMsg->msgBody = pRtcShutdown;
	pDestMsg->setMsgBody();
}

void CSipToRtc::mapToRtcAnswerOrError(TUniNetMsg* pSrcMsg, TUniNetMsg* pDestMsg,
		TSipCtrlMsg* pSipCtrl, TRtcCtrlMsg* pRtcCtrl, bool isReCall) {
	// 只处理接收到invite后的response，不处理其他情况（如bye之后的response）
//	pRtcCtrl->from = pSipCtrl->to.url.username;
//	pRtcCtrl->to = pSipCtrl->from.url.username;

	//	CTUniNetMsgHelper::print(pSrcMsg);

	pRtcCtrl->from = combineUserNameAndHost(pSipCtrl->to.url);
	CVarChar128 sipname;
	if(m_accessMode == 1 || m_accessMode == 2){
		sipname = combineUserNameAndHost(pSipCtrl->from.url);
		CVarChar128 rtcname = CUserMapHelper::getMapRtcUser(sipname);
		pRtcCtrl->to = rtcname;
	}
	else{
		pRtcCtrl->to = combineUserNameAndHost(pSipCtrl->from.url);
	}

	if(!isReCall){
		pRtcCtrl->offerSessionId = pSipCtrl->from.tag;
		pRtcCtrl->answerSessionId = pSipCtrl->to.tag;
	}
	else{
		pRtcCtrl->offerSessionId = pSipCtrl->to.tag;
		pRtcCtrl->answerSessionId = pSipCtrl->from.tag;
	}

//	CMsgMapHelper::storeRtcToSipMap(pRtcCtrl->answerSessionId,
//			pSipCtrl->sip_callId.number, pSipCtrl->to.tag);
	pDestMsg->ctrlMsgHdr = pRtcCtrl;
	pDestMsg->setCtrlMsgHdr();

	TSipResp* pSipResp = (TSipResp*) (pSrcMsg->msgBody);
	if (pSipResp->statusCode == 180 || pSipResp->statusCode == 200) {
		pDestMsg->msgName = RTC_ANSWER;

		TRtcAnswer* pRtcAnswer = new TRtcAnswer();
		pRtcAnswer->seq = atoi(pSipCtrl->cseq_number.c_str());
		pRtcAnswer->moreComing = pSipResp->statusCode == 180 ? TRUE : FALSE;
		pRtcAnswer->sdp = pSipResp->body.content;
		pDestMsg->msgBody = pRtcAnswer;
		pDestMsg->setMsgBody();
	} else {
		pDestMsg->msgName = RTC_ERROR;
		if(m_accessMode == 1 || m_accessMode == 2){
			CUserMapHelper::resetCalling(sipname);
		}
		
		TRtcError* pRtcError = new TRtcError();
		pRtcError->seq = atoi(pSipCtrl->cseq_number.c_str());
		if (pSipResp->statusCode == 481) {
			pRtcError->errorType = ERROR_NOMATCH;
		} else if (pSipResp->statusCode == 486 || pSipResp->statusCode == 600) {
			pRtcError->errorType = ERROR_REFUSED;
		} else if (pSipResp->statusCode == 408) {
			pRtcError->errorType = ERROR_TIMEOUT;
		} else if (pSipResp->statusCode == 491) {
			pRtcError->errorType = ERROR_CONFLICT;
		} else if (pSipResp->statusCode == 404){
			pRtcError->errorType = ERROR_OFFLINE;
		} else if (pSipResp->statusCode < 700 && pSipResp->statusCode > 300) {
			pRtcError->errorType = ERROR_FAILED;
		}
		pDestMsg->msgBody = pRtcError;
		pDestMsg->setMsgBody();
	}
}


void CSipToRtc::mapToRtcInfoOrUpdate(TUniNetMsg* pSrcMsg, TUniNetMsg* pDestMsg, TRtcCtrlMsg* pRtcCtrl,
		TSipCtrlMsg* pSipCtrl,   const CVarChar128& caller,
		const CVarChar128& callerHost, bool isReCall){

	if(pSrcMsg->msgName == SIP_INFO)
		pDestMsg->msgName = RTC_INFO;
	else
		pDestMsg->msgName = RTC_UPDATE;
	if(m_accessMode == 1 || m_accessMode == 2){
		CVarChar128 oldname = combineUserNameAndHost(pSipCtrl->to.url);
		CVarChar128 newname = CUserMapHelper::getMapRtcUser(oldname);
		pRtcCtrl->to = newname;
	}
	else{
		pRtcCtrl->to = combineUserNameAndHost(pSipCtrl->to.url);
	}
	pRtcCtrl->from = combineUserNameAndHost(pSipCtrl->from.url);

	if(!isReCall){
		// 需要区分挂断电话者是不是会话的发起者
		//change by guoxun
		if ((pSipCtrl->from.url.username == caller)&&(pSipCtrl->from.url.host==callerHost)) {
			pRtcCtrl->offerSessionId = pSipCtrl->from.tag;
			// 注意：cancel的时候没有answerSessionId，需要考虑
			pRtcCtrl->answerSessionId = pSipCtrl->to.tag;
		} else {
			pRtcCtrl->offerSessionId = pSipCtrl->to.tag;
			pRtcCtrl->answerSessionId = pSipCtrl->from.tag;
		}
	}
	else{
		//change by guoxun
		if ((pSipCtrl->from.url.username == caller)&&(pSipCtrl->from.url.host==callerHost)) {
			pRtcCtrl->offerSessionId = pSipCtrl->to.tag;
			// 注意：cancel的时候没有answerSessionId，需要考虑
			pRtcCtrl->answerSessionId = pSipCtrl->from.tag;
		} else {
			pRtcCtrl->offerSessionId = pSipCtrl->from.tag;
			pRtcCtrl->answerSessionId = pSipCtrl->to.tag;
		}

	}

	pDestMsg->ctrlMsgHdr = pRtcCtrl;
	pDestMsg->setCtrlMsgHdr();

	if(pDestMsg->msgName == RTC_INFO){
		TSipInfo* pSipInfo = (TSipInfo*) (pSrcMsg->msgBody);
		TRtcInfo* pRtcInfo = new TRtcInfo();
		pRtcInfo->seq = atoi(pSipCtrl->cseq_number.c_str());
		pRtcInfo->content_length = pSipInfo->body.content_length;
		pRtcInfo->content = pSipInfo->body.content;

		//		pRtcOffer->tieBreaker
		pDestMsg->msgBody = pRtcInfo;
		pDestMsg->setMsgBody();
	}
	else{
		TSipUpdate* pSipUpdate = (TSipUpdate*) (pSrcMsg->msgBody);
		TRtcUpdate* pRtcUpdate = new TRtcUpdate();
		pRtcUpdate->seq = atoi(pSipCtrl->cseq_number.c_str());
		pRtcUpdate->content_length = pSipUpdate->body.content_length;
		pRtcUpdate->content = pSipUpdate->body.content;

		//		pRtcOffer->tieBreaker
		pDestMsg->msgBody = pRtcUpdate;
		pDestMsg->setMsgBody();
	}

}




void CSipToRtc::mapToRtcMessage(TUniNetMsg* pSrcMsg, TUniNetMsg* pDestMsg,
		TRtcCtrlMsg* pRtcCtrl, TSipCtrlMsg* pSipCtrl) {
	pDestMsg->msgName = RTC_IM;
//	pRtcCtrl->from = pSipCtrl->from.url.username;
//	pRtcCtrl->to = pSipCtrl->to.url.username;
	pRtcCtrl->from = combineUserNameAndHost(pSipCtrl->from.url);
	if(m_accessMode == 1 || m_accessMode == 2){
		CVarChar128 sipname = combineUserNameAndHost(pSipCtrl->to.url);
		CVarChar128 rtcname = CUserMapHelper::getMapRtcUser(sipname);
		pRtcCtrl->to = rtcname;
	}
	else{
		pRtcCtrl->to = combineUserNameAndHost(pSipCtrl->to.url);
	}
	pRtcCtrl->offerSessionId = pSipCtrl->from.tag;

	CMsgMapHelper::storeSipCallID(pRtcCtrl->offerSessionId, pSipCtrl->sip_callId.number);

	pDestMsg->ctrlMsgHdr = pRtcCtrl;
	pDestMsg->setCtrlMsgHdr();
	TSipMessage* pSipMessage = (TSipMessage*) (pSrcMsg->msgBody);
	TRtcMessage* pRtcMessage = new TRtcMessage;
	pRtcMessage->seq = atoi(pSipCtrl->cseq_number.c_str());
	pRtcMessage->msgSize = pSipMessage->body.content_length;
	pRtcMessage->msgContent = pSipMessage->body.content;
	pDestMsg->msgBody = pRtcMessage;
	pDestMsg->setMsgBody();
}

//change by guoxun
BOOL CSipToRtc::msgMap(TUniNetMsg *pSrcMsg, TUniNetMsg *pDestMsg,
		CVarChar128 caller,CVarChar128 callerHost, bool isReCall){
	// 会话的最初发起者，应该来自CsipCallModule
	//CVarChar caller;

	pDestMsg->msgType = RTC_TYPE;
	pDestMsg->dialogType = pSrcMsg->dialogType;
	pDestMsg->tAddr = pSrcMsg->tAddr;

	TSipCtrlMsg* pSipCtrl = (TSipCtrlMsg*)pSrcMsg->ctrlMsgHdr;
	TRtcCtrlMsg* pRtcCtrl = new TRtcCtrlMsg();
	switch(pSrcMsg->msgName){
	case SIP_INVITE:
		mapToRtcOffer(pSrcMsg, pDestMsg, pRtcCtrl, pSipCtrl, isReCall);
		break;
	case SIP_ACK:
		mapToRtcOk(pSrcMsg, pDestMsg, pRtcCtrl, pSipCtrl, isReCall);
		break;
	case SIP_BYE:
	case SIP_CANCEL:
		maoToRtcShutdown(pDestMsg, pRtcCtrl, pSipCtrl, caller, callerHost, isReCall);//change by guoxun
		break;
	case SIP_RESPONSE:
		// 只处理接收到invite后的response，不处理其他情况（如bye之后的response）
		mapToRtcAnswerOrError(pSrcMsg, pDestMsg, pSipCtrl, pRtcCtrl, isReCall);
		break;
	case SIP_MESSAGE:
		mapToRtcMessage(pSrcMsg, pDestMsg, pRtcCtrl, pSipCtrl);
		break;
	case SIP_INFO:
		mapToRtcInfoOrUpdate(pSrcMsg, pDestMsg, pRtcCtrl, pSipCtrl, caller, callerHost, isReCall);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

