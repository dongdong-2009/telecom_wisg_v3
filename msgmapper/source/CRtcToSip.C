#include "CRtcToSip.h"
CRtcToSip* CRtcToSip::_instance = 0;

CRtcToSip* CRtcToSip::instance(){
	if(_instance == 0){
		_instance = new CRtcToSip();
	}
	return _instance;
}

CRtcToSip::CRtcToSip(){
	CProperties* properties = CPropertiesManager::getInstance()->getProperties("gateway.env");
	int accessMode = properties->getIntProperty("accessMode");
	if(accessMode == -1){
		accessMode = 0;
	}
	else{
		this->accessMode = accessMode;
	}

}

void CRtcToSip::mapToSipInvite(PTRtcCtrlMsg pRtcCtrl, TUniNetMsg* pDestMsg,
		TUniNetMsg* pSrcMsg) {
	pDestMsg->msgName = SIP_INVITE;

	// 消息体
	TRtcOffer* pRtcOffer = (TRtcOffer*) (pSrcMsg->msgBody);
	TSipInvite* pSipInvite = new TSipInvite();

	// (1) sip req_url,注意是to

	pSipInvite->req_uri = CSipMsgHelper::createSipURI("tel",
			getUserName(pRtcCtrl->to).c_str(), getHost(pRtcCtrl->to).c_str(), NULL);
	
	// (2) sip content_type
	pSipInvite->content_type = CSipMsgHelper::createSipContentType(
			"application", "sdp");
	// (3) sip body
	pSipInvite->body = CSipMsgHelper::createSipBody(pRtcOffer->sdp);
	pDestMsg->msgBody = pSipInvite;
	pDestMsg->setMsgBody();
	// 消息头
	PTSipCtrlMsg pSipCtrl = new TSipCtrlMsg();
	// (1)sip call id
	pSipCtrl->sip_callId.number = CMsgMapHelper::generateSipCallIDNumber(pRtcCtrl->offerSessionId);
	
	//pSipCtrl->sip_callId.host =
	// (2)sip from and to

	if(accessMode == 1 || accessMode == 2){
		CVarChar128 sipName = CUserMapHelper::getMapSipUser(pRtcCtrl->from);
		//CVarChar128  newSipName = CUserMapHelper::getMapNewSipUser(pRtcCtrl->from);
		pSipCtrl->from.displayname = getUserName(sipName).c_str();
		pSipCtrl->from.url = CSipMsgHelper::createSipURI("sip", getUserName(sipName).c_str(),
					getHost(sipName).c_str(), NULL);
	}
	else{
		pSipCtrl->from.displayname = getUserName(pRtcCtrl->from).c_str();
		pSipCtrl->from.url = CSipMsgHelper::createSipURI("sip", getUserName(pRtcCtrl->from).c_str(),
			getHost(pRtcCtrl->from).c_str(), NULL);
	}
	pSipCtrl->from.tag = pRtcCtrl->offerSessionId;
	
	CMsgMapHelper::storeSipCallID(pRtcCtrl->offerSessionId, pSipCtrl->sip_callId.number);

	pSipCtrl->to.displayname = getUserName(pRtcCtrl->to).c_str();
	pSipCtrl->to.url = CSipMsgHelper::createSipURI("tel", getUserName(pRtcCtrl->to).c_str(),
			getHost(pRtcCtrl->to).c_str(), NULL);

	// （3）via
	// （4）route
	// (5) CSeq_number and CSeq_method;
	char str[64];
	sprintf(str, "%d", pRtcOffer->seq);
	pSipCtrl->cseq_number = str;
	pSipCtrl->cseq_method = "INVITE";

	pDestMsg->ctrlMsgHdr = pSipCtrl;
	pDestMsg->setCtrlMsgHdr();
}

// 仅仅考虑的是对于invite消息的response，其他消息不考虑
void CRtcToSip::mapToSipResponse(TUniNetMsg* pDestMsg, TUniNetMsg* pSrcMsg, bool isReCall) {
	pDestMsg->msgName = SIP_RESPONSE;
	// 消息体
	TRtcAnswer* pRtcAnswer = (TRtcAnswer*) (pSrcMsg->msgBody);
	TSipResp* pSipResponse = new TSipResp();
	// (1)response statusCode and status_phase
	if (pRtcAnswer->moreComing) {
		pSipResponse->statusCode = 180;
		pSipResponse->reason_phase = "Ringing";
	} else {
		pSipResponse->statusCode = 200;
		pSipResponse->reason_phase = "OK";
	}
	// (2)response content_type
	pSipResponse->content_type = CSipMsgHelper::createSipContentType(
			"application", "sdp");
	// (3) body
	pSipResponse->body = CSipMsgHelper::createSipBody(pRtcAnswer->sdp);
	pDestMsg->msgBody = pSipResponse;
	pDestMsg->setMsgBody();

	// 消息头
	PTRtcCtrlMsg pRtcCtrl = (PTRtcCtrlMsg) pSrcMsg->ctrlMsgHdr;
	PTSipCtrlMsg pSipCtrl = new TSipCtrlMsg();

	// (1)sip call id and tag
	pSipCtrl->sip_callId.number = CMsgMapHelper::getSipCallID(pRtcCtrl->offerSessionId);

	// (2)sip from and to
	pSipCtrl->from.displayname = getUserName(pRtcCtrl->to).c_str();
	pSipCtrl->from.url = CSipMsgHelper::createSipURI("sip", getUserName(pRtcCtrl->to).c_str(),
			getHost(pRtcCtrl->to).c_str(), NULL);
			
	

	if(accessMode == 1 || accessMode == 2){
		CVarChar128  newSipName = CUserMapHelper::getMapSipUser(pRtcCtrl->from);
		pSipCtrl->to.displayname = getUserName(newSipName).c_str();
		pSipCtrl->to.url = CSipMsgHelper::createSipURI("tel", getUserName(newSipName).c_str(),
				getHost(newSipName).c_str(), NULL);
	}
	else{
		pSipCtrl->to.displayname = getUserName(pRtcCtrl->from).c_str();
		pSipCtrl->to.url = CSipMsgHelper::createSipURI("tel", getUserName(pRtcCtrl->from).c_str(),
			getHost(pRtcCtrl->from).c_str(), NULL);
	}
	
	// whether need?
	if(!isReCall){
		pSipCtrl->from.tag = pRtcCtrl->offerSessionId;
		pSipCtrl->to.tag = pRtcCtrl->answerSessionId;
	}
	else{
		pSipCtrl->from.tag = pRtcCtrl->answerSessionId;
		pSipCtrl->to.tag = pRtcCtrl->offerSessionId;
	}


//	pSipCtrl->to.tag = CMsgMapHelper::getToTagByFromTag(pSipCtrl->from.tag);
//	CMsgMapHelper::storeFromTagAndAnswerId(pSipCtrl->from.tag, pRtcCtrl->answerSessionId);
//	CMsgMapHelper::storeAnswerSessionIdToFromtag(pSipCtrl->from.tag, pRtcCtrl->answerSessionId);
	// （3）via
	// （4）route
	// (5) CSeq_number and CSeq_method;
	char str[64];
	sprintf(str, "%d", pRtcAnswer->seq);
	pSipCtrl->cseq_number = str;
	pSipCtrl->cseq_method = "INVITE";

	pDestMsg->ctrlMsgHdr = pSipCtrl;
	pDestMsg->setCtrlMsgHdr();
}

void CRtcToSip::mapToSipAck(TUniNetMsg* pDestMsg, TUniNetMsg* pSrcMsg) {
	pDestMsg->msgName = SIP_ACK;
	PTRtcCtrlMsg pRtcCtrl = (PTRtcCtrlMsg) pSrcMsg->ctrlMsgHdr;

	TRtcOK* pRtcOk = (TRtcOK*) (pSrcMsg->msgBody);
	TSipReq* pSipReq = new TSipReq();
	pSipReq->req_uri = CSipMsgHelper::createSipURI("tel",
			getUserName(pRtcCtrl->to).c_str(), getHost(pRtcCtrl->to).c_str(), NULL);
	pSipReq->content_type = CSipMsgHelper::createSipContentType(
			"application", "sdp");
	pSipReq->body = CSipMsgHelper::createSipBody(pRtcOk->sdp);

	pDestMsg->msgBody = pSipReq;
	pDestMsg->setMsgBody();


	PTSipCtrlMsg pSipCtrl = new TSipCtrlMsg();
	// (1)sip call id and tag
	
	
	pSipCtrl->sip_callId.number = CMsgMapHelper::getSipCallID(pRtcCtrl->offerSessionId);


	// (2)sip from and to

	if(accessMode == 1 || accessMode == 2){
		CVarChar128  newSipName = CUserMapHelper::getMapSipUser(pRtcCtrl->from);
		pSipCtrl->from.displayname = getUserName(newSipName).c_str();
		pSipCtrl->from.url = CSipMsgHelper::createSipURI("sip", getUserName(newSipName).c_str(),
				getHost(newSipName).c_str(), NULL);
	}
	else{
		pSipCtrl->from.displayname = getUserName(pRtcCtrl->from).c_str();
		pSipCtrl->from.url = CSipMsgHelper::createSipURI("sip", getUserName(pRtcCtrl->from).c_str(),
				getHost(pRtcCtrl->from).c_str(), NULL);

	}

	pSipCtrl->from.tag = pRtcCtrl->offerSessionId;
	pSipCtrl->to.displayname = getUserName(pRtcCtrl->to).c_str();
	pSipCtrl->to.url = CSipMsgHelper::createSipURI("tel", getUserName(pRtcCtrl->to).c_str(),
			getHost(pRtcCtrl->to).c_str(), NULL);
	pSipCtrl->to.tag = pRtcCtrl->answerSessionId;



	// （3）via
	// （4）route
	// (5) CSeq_number and CSeq_method;
	char str[64];
	sprintf(str, "%d", pRtcOk->seq);
	pSipCtrl->cseq_number = str;
	pSipCtrl->cseq_method = "ACK";

	pDestMsg->ctrlMsgHdr = pSipCtrl;
	pDestMsg->setCtrlMsgHdr();
}

void CRtcToSip::mapToSipBye(PTRtcCtrlMsg pRtcCtrl, TUniNetMsg* pDestMsg,
		TUniNetMsg* pSrcMsg, CVarChar128 caller/*CRtcCallModule* pRtcCall*/, bool isReCall) {
	// caller from
//	CVarChar128 caller = pRtcCall->getCallerOfferSessionIdPair().first;
	pDestMsg->msgName = SIP_BYE;

	TRtcShutdown* pRtcShutdown = (TRtcShutdown*) (pSrcMsg->msgBody);
	TSipBye* pSipBye = new TSipBye();
	pSipBye->req_uri = CSipMsgHelper::createSipURI("sip",
			getUserName(pRtcCtrl->to).c_str(), getHost(pRtcCtrl->to).c_str(), NULL);
	pDestMsg->msgBody = pSipBye;
	pDestMsg->setMsgBody();

	PTSipCtrlMsg pSipCtrl = new TSipCtrlMsg();
	// (1)sip call id and tag
	pSipCtrl->sip_callId.number = CMsgMapHelper::getSipCallID(pRtcCtrl->offerSessionId);
	// (2)sip from and to

	if(accessMode == 1 || accessMode == 2){
		CVarChar128  newSipName = CUserMapHelper::getMapSipUser(pRtcCtrl->from);
		CUserMapHelper::resetCalling(newSipName);
		pSipCtrl->from.displayname = getUserName(newSipName).c_str();
		pSipCtrl->from.url = CSipMsgHelper::createSipURI("sip", getUserName(newSipName).c_str(),
				getHost(newSipName).c_str(), NULL);
	}
	else{
		pSipCtrl->from.displayname = getUserName(pRtcCtrl->from).c_str();
		pSipCtrl->from.url = CSipMsgHelper::createSipURI("sip", getUserName(pRtcCtrl->from).c_str(),
				getHost(pRtcCtrl->from).c_str(), NULL);
	}



	pSipCtrl->to.displayname = getUserName(pRtcCtrl->to).c_str();
	pSipCtrl->to.url = CSipMsgHelper::createSipURI("tel", getUserName(pRtcCtrl->to).c_str(),
			getHost(pRtcCtrl->to).c_str(), NULL);
	if(!isReCall){
		if(pRtcCtrl->from == caller){
			pSipCtrl->from.tag = pRtcCtrl->offerSessionId;
			pSipCtrl->to.tag = pRtcCtrl->answerSessionId;
		}else{
			pSipCtrl->to.tag = pRtcCtrl->offerSessionId;
			pSipCtrl->from.tag= pRtcCtrl->answerSessionId;
		}
	}
	else{
		if(pRtcCtrl->from == caller){
			pSipCtrl->to.tag = pRtcCtrl->offerSessionId;
			pSipCtrl->from.tag= pRtcCtrl->answerSessionId;
		}else{
			pSipCtrl->from.tag = pRtcCtrl->offerSessionId;
			pSipCtrl->to.tag = pRtcCtrl->answerSessionId;
		}
	}
	// （3）via
	// （4）route
	// (5) CSeq_number and CSeq_method;
	char str[64];
	sprintf(str, "%d", pRtcShutdown->seq);
	pSipCtrl->cseq_number = str;
	pSipCtrl->cseq_method = "BYE";

	pDestMsg->ctrlMsgHdr = pSipCtrl;
	pDestMsg->setCtrlMsgHdr();
}

void CRtcToSip::mapToSipCancel(PTRtcCtrlMsg pRtcCtrl, TUniNetMsg* pDestMsg,
		TUniNetMsg* pSrcMsg) {
	pDestMsg->msgName = SIP_CANCEL;
	TRtcShutdown* pRtcShutdown = (TRtcShutdown*) (pSrcMsg->msgBody);
	TSipCancel* pSipCancel = new TSipCancel();
	pSipCancel->req_uri = CSipMsgHelper::createSipURI("tel",
			getUserName(pRtcCtrl->to).c_str(), getHost(pRtcCtrl->to).c_str(), NULL);
	pDestMsg->msgBody = pSipCancel;
	pDestMsg->setMsgBody();

	PTSipCtrlMsg pSipCtrl = new TSipCtrlMsg();
	// (1)sip call id and tag
	pSipCtrl->sip_callId.number = CMsgMapHelper::getSipCallID(pRtcCtrl->offerSessionId);

	// (2)sip from and to
	if(accessMode == 1 || accessMode == 2){
		CVarChar128  newSipName = CUserMapHelper::getMapSipUser(pRtcCtrl->from);
		pSipCtrl->from.displayname = getUserName(newSipName).c_str();
		CUserMapHelper::resetCalling(newSipName);
		pSipCtrl->from.url = CSipMsgHelper::createSipURI("sip", getUserName(newSipName).c_str(),
			getHost(newSipName).c_str(), NULL);
	}
	else{
		pSipCtrl->from.displayname = getUserName(pRtcCtrl->from).c_str();
		pSipCtrl->from.url = CSipMsgHelper::createSipURI("sip", getUserName(pRtcCtrl->from).c_str(),
			getHost(pRtcCtrl->from).c_str(), NULL);
	}

	pSipCtrl->from.tag = pRtcCtrl->offerSessionId;
	pSipCtrl->to.displayname = getUserName(pRtcCtrl->to).c_str();
	pSipCtrl->to.url = CSipMsgHelper::createSipURI("tel", getUserName(pRtcCtrl->to).c_str(),
			 getHost(pRtcCtrl->to).c_str(), NULL);
	// pSipCtrl->to.tag = CMsgMapHelper::getSipTag((CVarChar)pRtcCtrl->answerSessionId);
	// （3）via
	// （4）route
	// (5) CSeq_number and CSeq_method;
	char str[64];
	sprintf(str, "%d", pRtcShutdown->seq);
	pSipCtrl->cseq_number = str;
	pSipCtrl->cseq_method = "CANCEL";	// cancel? need reconsider

	pDestMsg->ctrlMsgHdr = pSipCtrl;
	pDestMsg->setCtrlMsgHdr();
}

void CRtcToSip::mapToSipMessage(TUniNetMsg* pSrcMsg, TUniNetMsg* pDestMsg,
		PTRtcCtrlMsg pRtcCtrl) {
	pDestMsg->msgName = SIP_MESSAGE;
	TRtcMessage* pRtcMsg = (TRtcMessage*) (pSrcMsg->msgBody);
	TSipMessage* pSipMsg = new TSipMessage();
	// (1) message req_url

	pSipMsg->req_uri = CSipMsgHelper::createSipURI("sip", getUserName(pRtcCtrl->to).c_str(),
			getHost(pRtcCtrl->to).c_str(), NULL);
	// (2)
	pSipMsg->content_type = CSipMsgHelper::createSipContentType("text",	"plain");
	// (3)
	pSipMsg->body = CSipMsgHelper::createSipBody(pRtcMsg->msgContent);
	pDestMsg->msgBody = pSipMsg;
	pDestMsg->setMsgBody();

	PTSipCtrlMsg pSipCtrl = new TSipCtrlMsg();
	// (1)sip call id and tag
	pSipCtrl->sip_callId.number = CMsgMapHelper::generateSipCallIDNumber(pRtcCtrl->offerSessionId);
	//pSipCtrl->sip_callId.host =

	// (2)sip from and to
	if(accessMode == 1 || accessMode == 2){
		CVarChar128  newSipName = CUserMapHelper::getMapNewSipUser(pRtcCtrl->from);
		pSipCtrl->from.displayname = getUserName(newSipName).c_str();
		pSipCtrl->from.url = CSipMsgHelper::createSipURI("sip", getUserName(newSipName).c_str(),
				getHost(newSipName).c_str(), NULL);
	}
	else{
		pSipCtrl->from.displayname = getUserName(pRtcCtrl->from).c_str();
		pSipCtrl->from.url = CSipMsgHelper::createSipURI("sip", getUserName(pRtcCtrl->from).c_str(),
				getHost(pRtcCtrl->from).c_str(), NULL);
	}

	pSipCtrl->from.tag = pRtcCtrl->offerSessionId;

	pSipCtrl->to.displayname = getUserName(pRtcCtrl->to).c_str();
	pSipCtrl->to.url = CSipMsgHelper::createSipURI("sip", getUserName(pRtcCtrl->to).c_str(),
			getHost(pRtcCtrl->to).c_str(), NULL);
	// （3）via
	// （4）route
	// (5) CSeq_number and CSeq_method;
	char str[64];
	sprintf(str, "%d", pRtcMsg->seq);
	pSipCtrl->cseq_number = str;
	pSipCtrl->cseq_method = "MESSAGE";

	pDestMsg->ctrlMsgHdr = pSipCtrl;
	pDestMsg->setCtrlMsgHdr();
}

/*
 * 注意：1 国帅说现在RTC的from to是不带域信息的，所以所有host都没有赋值，待确认 12.7
 */
BOOL CRtcToSip::msgMap(TUniNetMsg *pSrcMsg, TUniNetMsg *pDestMsg, CVarChar128 caller, bool isReCall){
	pDestMsg->msgType = SIP_TYPE;
	pDestMsg->dialogType = pSrcMsg->dialogType;
	pDestMsg->tAddr = pSrcMsg->tAddr;

	PTRtcCtrlMsg pRtcCtrl = (PTRtcCtrlMsg) pSrcMsg->ctrlMsgHdr;
	switch(pSrcMsg->msgName){
	case RTC_OFFER:
		mapToSipInvite(pRtcCtrl, pDestMsg, pSrcMsg);
		break;
	case RTC_ANSWER:
		mapToSipResponse(pDestMsg, pSrcMsg, isReCall);
		break;
	case RTC_OK:
		// sip 中是否没有ack对应的消息体？
		mapToSipAck(pDestMsg, pSrcMsg);
		break;
	case RTC_SHUTDOWN:
		// it is cancel
		if(pRtcCtrl->answerSessionId.length() == 0)
			mapToSipCancel(pRtcCtrl, pDestMsg, pSrcMsg);
		else
			mapToSipBye(pRtcCtrl, pDestMsg, pSrcMsg, caller, isReCall);
		break;
	case RTC_ERROR:
		msgMapToSipError(pSrcMsg, pDestMsg, isReCall);
		break;
	case RTC_CANDIDATE:
		break;
	case RTC_IM:
		mapToSipMessage(pSrcMsg, pDestMsg, pRtcCtrl);
		break;
	default:
		return FALSE;
	}

	return TRUE;
}


BOOL CRtcToSip::msgMapToSipError(TUniNetMsg *pSrcMsg, TUniNetMsg *pDestMsg, bool isReCall){
	pDestMsg->msgName = SIP_RESPONSE;
	PTRtcError pRtcError = (PTRtcError) pSrcMsg->msgBody;
	TSipResp* pSipResponse = new TSipResp();
	switch(pRtcError->errorType){
	case ERROR_NOMATCH:
		pSipResponse->statusCode = 481;
		pSipResponse->reason_phase = "Call Leg/Transaction Does Not Exist";
		break;
	case ERROR_REFUSED:
		pSipResponse->statusCode = 486;
		pSipResponse->reason_phase = "Busy Here";
		break;
	case ERROR_TIMEOUT:
		pSipResponse->statusCode = 408;
		pSipResponse->reason_phase = "Request Timeout";
		break;
	case ERROR_CONFLICT:	// webrtc 通话中，491不会XLite挂断而是一直处于呼叫状态，改为486
//		pSipResponse->statusCode = 491;
//		pSipResponse->reason_phase = "Request Pending";
		pSipResponse->statusCode = 486;
		pSipResponse->reason_phase = "Busy Here";
		break;
	case ERROR_OFFLINE:
		pSipResponse->statusCode = 404;
		pSipResponse->reason_phase = "NOT FOUND";
		break;
	case ERROR_FAILED:
		pSipResponse->statusCode = 500;
		pSipResponse->reason_phase = "Internal Server Error";
		break;
	default:
		pSipResponse->statusCode = 500;
		pSipResponse->reason_phase = "Internal Server Error";
		break;
	//	return FALSE;
	}
//	pSipResponse->content_type = CSipMsgHelper::createSipContentType(
//				"application", "sdp");
	pDestMsg->msgBody = pSipResponse;
	pDestMsg->setMsgBody();


	PTRtcCtrlMsg pRtcCtrl = (PTRtcCtrlMsg) pSrcMsg->ctrlMsgHdr;
	PTSipCtrlMsg pSipCtrl = new TSipCtrlMsg();

	// (1)sip call id and tag
	pSipCtrl->sip_callId.number = CMsgMapHelper::getSipCallID(pRtcCtrl->offerSessionId);
	
	pSipCtrl->from.displayname = getUserName(pRtcCtrl->to).c_str();
	pSipCtrl->from.url = CSipMsgHelper::createSipURI("sip", getUserName(pRtcCtrl->to).c_str(),
				getHost(pRtcCtrl->to).c_str(), NULL);
	// (2)sip from and to
	if(accessMode == 1  || accessMode == 2){
		CVarChar128 newSipName = CUserMapHelper::getMapSipUser(pRtcCtrl->from);
		CUserMapHelper::resetCalling(newSipName);
		pSipCtrl->to.displayname = getUserName(newSipName).c_str();
		pSipCtrl->to.url = CSipMsgHelper::createSipURI("sip", getUserName(newSipName).c_str(),
			getHost(newSipName).c_str(), NULL);
	}
	else{
		pSipCtrl->to.displayname = getUserName(pRtcCtrl->from).c_str();
		pSipCtrl->to.url = CSipMsgHelper::createSipURI("sip", getUserName(pRtcCtrl->from).c_str(),
			getHost(pRtcCtrl->from).c_str(), NULL);
		
	}

	if(!isReCall){
		pSipCtrl->to.tag = pRtcCtrl->answerSessionId;
		pSipCtrl->from.tag = pRtcCtrl->offerSessionId;					
	}
	else{	
		pSipCtrl->to.tag = pRtcCtrl->offerSessionId;
		pSipCtrl->from.tag = pRtcCtrl->answerSessionId;					
	}


	// （3）via
	// （4）route
	// (5) CSeq_number and CSeq_method;
	char str[64];
	sprintf(str, "%d", pRtcError->seq);
	pSipCtrl->cseq_number = str;
	pSipCtrl->cseq_method = "INVITE";

	pDestMsg->ctrlMsgHdr = pSipCtrl;
	pDestMsg->setCtrlMsgHdr();
	return TRUE;
}

string CRtcToSip::getUserName(const CVarChar128& user){
	string temp = user.c_str();
	int i = temp.find('@');
	if(i != -1)
		return (temp.substr(0, i));
	else
		return user.c_str();
}

string CRtcToSip::getHost(const CVarChar128& user){
	string temp = user.c_str();
	int i = temp.find('@');
	if(i != -1)
		return temp.substr(i + 1);
	else
		return "";
}
