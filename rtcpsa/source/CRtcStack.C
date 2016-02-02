#include "CRtcStack.h"

//default PSAID
INT CRtcStack::m_PSAID = 18;
CRtcStack::CRtcStack(INT psaid){
	m_PSAID = psaid;
	LOGADDR_DISPATCHER = CPropertiesManager::getInstance()->getProperties("rtc.env")->getIntProperty("LOGADDR_DISPATCHER");
	
	if(LOGADDR_DISPATCHER == -1){
		psaErrorLog(m_PSAID,"ERROR(%s:%d): LOGADDR_DISPATCHER is not set in file rtc.env, use 231\n",__FILE__, __LINE__);
		LOGADDR_DISPATCHER = 231;
	}


	string tmp = CPropertiesManager::getInstance()->getProperties("gateway.env")->getProperty("RTC_DOMAIN");
	if(tmp == ""){
		tmp = "webrtcdomain.com";
	}
	strcpy(RTC_DOMAIN,tmp.c_str());

	log4cxx::xml::DOMConfigurator::configureAndWatch("etc/log4cxx.xml", 5000);
	m_logger = log4cxx::Logger::getLogger("SgFileAppender");

	m_pSocket = new CPracticalSocket();
}

CRtcStack::~CRtcStack(){
	delete m_pSocket;
	m_pSocket = NULL;

	LOG4CXX_INFO(m_logger, "call CRtcStack distructure")

}

BOOL CRtcStack::init(string& wcsIp, int wcsPort){
	return m_pSocket->init(wcsIp,wcsPort);
}

BOOL CRtcStack::convertMsgToUniNetMsg(string strMsg, PTUniNetMsg pMsg){
	try{
		CRtcProtocolParser rtcParser(strMsg);
		CRoapParser roapParser = rtcParser.getRoapParser();

		pMsg->msgType = RTC_TYPE;
		// always BEGIN ?
		pMsg->dialogType = DIALOG_BEGIN;
		pMsg->oAddr.logAddr = m_PSAID;
		pMsg->tAddr.logAddr = LOGADDR_DISPATCHER;

		// set ctrl msg
		PTRtcCtrlMsg pCtrlMsg = new TRtcCtrlMsg();
		pCtrlMsg->rtcType = rtcParser.getType();

		string fromStr = rtcParser.getFrom().c_str();
		size_t pos = fromStr.find("@");
		if(pos != string::npos){
			fromStr.replace(pos, 1, "~");
		}
		fromStr.append("@"+(string)RTC_DOMAIN);
		

		string toStr = rtcParser.getTo().c_str();
//		if((toStr[0] != '0' && toStr.length() == 11) || toStr.length() == 12){
//			//为手机号
//		//	toStr = "+86"+toStr;
//		}

		pCtrlMsg->from = fromStr.c_str();
		pCtrlMsg->to = toStr.c_str();
		pCtrlMsg->offerSessionId = roapParser.getOfferSessionId().c_str();
		pCtrlMsg->answerSessionId = roapParser.getAnswerSessionId().c_str();

		pMsg->ctrlMsgHdr = pCtrlMsg;
		pMsg->setCtrlMsgHdr();

		int roapType = roapParser.getType();
		if(roapType == ROAP_OFFER){
			pMsg->msgName = RTC_OFFER;
			PTRtcOffer pMsgBody = new TRtcOffer();
			pMsgBody->seq = roapParser.getSeq();
			pMsgBody->sdp = roapParser.getSdp().c_str();

			pMsgBody->tieBreaker = roapParser.getTieBreaker().c_str();
			pMsg->msgBody = pMsgBody;
			pMsg->setMsgBody();
		}else if(roapType == ROAP_ANSWER){
			pMsg->msgName = RTC_ANSWER;
			PTRtcAnswer pMsgBody = new TRtcAnswer();
			pMsgBody->seq = roapParser.getSeq();
			pMsgBody->sdp = roapParser.getSdp().c_str();
			pMsgBody->moreComing = roapParser.isMoreComing();
			pMsg->msgBody = pMsgBody;
			pMsg->setMsgBody();
		}else if(roapType == ROAP_OK){
			pMsg->msgName = RTC_OK;
			PTRtcOK pMsgBody = new TRtcOK();
			pMsgBody->seq = roapParser.getSeq();
			pMsgBody->sdp = roapParser.getSdp().c_str();	//OK sdp,  re-offer->answer->OK
			pMsg->msgBody = pMsgBody;
			pMsg->setMsgBody();
		}else if(roapType == ROAP_SHUTDOWN){
			pMsg->msgName = RTC_SHUTDOWN;
			PTRtcShutdown pMsgBody = new TRtcShutdown();
			pMsgBody->seq = roapParser.getSeq();
			pMsg->msgBody = pMsgBody;
			pMsg->setMsgBody();
		}else if(roapType == ROAP_ERROR){
			pMsg->msgName = RTC_ERROR;
			PTRtcError pMsgBody = new TRtcError();
			pMsgBody->errorType = roapParser.getErrorType();
			pMsgBody->seq = roapParser.getSeq();
			pMsg->msgBody = pMsgBody;
			pMsg->setMsgBody();
		}else if(roapType == ROAP_CANDIDATE){
			pMsg->msgName = RTC_CANDIDATE;
			PTRtcCandidate pMsgBody = new TRtcCandidate();
			pMsgBody->seq = roapParser.getSeq();
			pMsgBody->label = roapParser.getLabel();
			pMsgBody->sdp = roapParser.getSdp().c_str();
			pMsg->msgBody = pMsgBody;
			pMsg->setMsgBody();
		}else if(roapType == ROAP_MESSAGE){
			pMsg->msgName = RTC_IM;
			PTRtcMessage pMsgBody = new TRtcMessage();
			pMsgBody->seq = roapParser.getSeq();
			pMsgBody->msgSize = roapParser.getMsgSize();
			pMsgBody->msgContent = roapParser.getMsgContent().c_str();
			pMsg->msgBody = pMsgBody;
			pMsg->setMsgBody();
		}else{
			LOG4CXX_ERROR(m_logger, "unhandle roaptype:"<<roapType);
			return false;
		}
	}catch(std::runtime_error & e){
		LOG4CXX_ERROR(m_logger, "catch exception:"<<e.what());
		return false;
	}
	return true;
}

BOOL CRtcStack::convertUniMsgToPlainMsg(PTUniNetMsg uniMsg, string& plainMsg){
	CRoapParser* roapParser = NULL;
	try{
		PTRtcCtrlMsg pCtrlMsg = (PTRtcCtrlMsg)uniMsg->ctrlMsgHdr;
		switch(uniMsg->msgName){
		case RTC_OFFER:{
			PTRtcOffer pMsgOffer = (PTRtcOffer)uniMsg->msgBody;
			roapParser = CRoapParser::createOffer(pCtrlMsg->offerSessionId.c_str(),
									pCtrlMsg->answerSessionId.c_str(),
									pMsgOffer->seq,
									pMsgOffer->sdp.c_str(),
									pMsgOffer->tieBreaker.c_str());
			break;
		}
		case RTC_ANSWER:{
			PTRtcAnswer pMsgAnswer = (PTRtcAnswer)uniMsg->msgBody;
			roapParser = CRoapParser::createAnswer(pCtrlMsg->offerSessionId.c_str(),
					pCtrlMsg->answerSessionId.c_str(),
					pMsgAnswer->seq,
					pMsgAnswer->sdp.c_str(),
					pMsgAnswer->moreComing);
			break;
		}
		case RTC_OK:{
			PTRtcOK pMsgOk = (PTRtcOK)uniMsg->msgBody;
			roapParser = CRoapParser::createOK(pCtrlMsg->offerSessionId.c_str(),
					pCtrlMsg->answerSessionId.c_str(),
					pMsgOk->seq, pMsgOk->sdp.c_str());
			break;
		}

		case RTC_INFO:
		{
			PTRtcInfo pMsgInfo = (PTRtcInfo) uniMsg->msgBody;
			roapParser = CRoapParser::createInfo(pCtrlMsg->offerSessionId.c_str(), pCtrlMsg->answerSessionId.c_str(),
					pMsgInfo->seq, pMsgInfo->content_length, pMsgInfo->content.c_str());
			break;
		}

		case RTC_UPDATE:
		{
			PTRtcUpdate pMsgUpdate = (PTRtcUpdate) uniMsg->msgBody;
			roapParser = CRoapParser::createUpdate(pCtrlMsg->offerSessionId.c_str(), pCtrlMsg->answerSessionId.c_str(),
					pMsgUpdate->seq, pMsgUpdate->content_length, pMsgUpdate->content.c_str());
			break;
		}

		case RTC_NOTIFY:
		{
			PTRtcNotify pMsgNotify = (PTRtcNotify) uniMsg->msgBody;
			roapParser = CRoapParser::createNotify(pCtrlMsg->offerSessionId.c_str(), pCtrlMsg->answerSessionId.c_str(),
					pMsgNotify->seq, pMsgNotify->content_length, pMsgNotify->content.c_str());
			break;
		}

		case RTC_SHUTDOWN:
		{
			PTRtcShutdown pMsgShutdown = (PTRtcShutdown)uniMsg->msgBody;
			roapParser = CRoapParser::createShutdown(pCtrlMsg->offerSessionId.c_str(),
					pCtrlMsg->answerSessionId.c_str(),
					pMsgShutdown->seq);
			break;
		}
		case RTC_ERROR:{
			PTRtcError pMsgError = (PTRtcError)uniMsg->msgBody;
			roapParser = CRoapParser::createError(pCtrlMsg->offerSessionId.c_str(),
					pCtrlMsg->answerSessionId.c_str(),
					pMsgError->seq,
					pMsgError->errorType);
			break;
		}
		case RTC_CANDIDATE:{
			PTRtcCandidate pMsgCand = (PTRtcCandidate)uniMsg->msgBody;
			roapParser = CRoapParser::createCandidate(pCtrlMsg->offerSessionId.c_str(),
					pCtrlMsg->answerSessionId.c_str(),
					pMsgCand->seq,
					pMsgCand->sdp.c_str(),
					pMsgCand->label);
			break;
		}
		case RTC_IM:{
			PTRtcMessage pMsgIM = (PTRtcMessage)uniMsg->msgBody;
			roapParser = CRoapParser::createMessage(pCtrlMsg->offerSessionId.c_str(),
					pCtrlMsg->answerSessionId.c_str(),
					pMsgIM->seq,
					pMsgIM->msgSize,
					pMsgIM->msgContent.c_str());
			break;
		}
		default:
			LOG4CXX_ERROR(m_logger, "can not convertUniMsgToPlainMsg msgName:"<<uniMsg->msgName);
			return FALSE;
		}

		string toStr = pCtrlMsg->to.c_str();

		size_t i = toStr.find("@");
		if(i != string::npos){
			toStr = toStr.substr(0,i);
			int pos = toStr.find("~");
			if(pos != string::npos){
				toStr.replace(pos,1, "@");
			}

		}

		string fromStr = pCtrlMsg->from.c_str();
		if(fromStr[0] == '+'){
			fromStr = fromStr.substr(0, 3);
		}

		CRtcProtocolParser rtcParser(pCtrlMsg->rtcType,
				fromStr.c_str(),
				toStr.c_str(),
				*roapParser);
		plainMsg = rtcParser.toPlainString();
		delete roapParser;
		return TRUE;
	}catch(std::runtime_error& e){
		if(roapParser){
			delete roapParser;
			roapParser = NULL;
		}
		LOG4CXX_ERROR(m_logger, "catch exception:"<<e.what());
		return false;
	}
	return TRUE;
}





void CRtcStack::doActive(){
	int sockfd;	// 消息来自那个socket
	string plainMsg;
	if(m_pSocket->recvMsg(sockfd, plainMsg)){
		PTUniNetMsg pMsg = new TUniNetMsg();
		if(convertMsgToUniNetMsg(plainMsg, pMsg) ==  false){
			return ;
		}

		LOG4CXX_INFO(m_logger, "doActive:: recvMsg "<<plainMsg<<"\nmsgType "<<pMsg->getMsgNameStr());

		TRtcCtrlMsg *pCtrl = (TRtcCtrlMsg *)pMsg->ctrlMsgHdr;
		string offersessionId(pCtrl->offerSessionId.c_str());

		// 发送到mcf
		PTMsg mcfMsg = new TMsg();
		CMsgConvertor::convertMsg(pMsg, mcfMsg);
		sendMsgToPACM(mcfMsg);
	}
}

BOOL CRtcStack::doSendMsg(PTMsg pMsg){
	PTUniNetMsg uniMsg = (PTUniNetMsg) pMsg->pMsgPara;
	if(NULL == uniMsg){
		return FALSE;
	}

	string plainMsg;
	if(convertUniMsgToPlainMsg(uniMsg, plainMsg) == false){
		return FALSE;
	}

	TRtcCtrlMsg *pCtrl = (TRtcCtrlMsg *)uniMsg->ctrlMsgHdr;
	string offersessionId(pCtrl->offerSessionId.c_str());

	//the message need send to webrtc server
	return sendToWebRTCServer(offersessionId, plainMsg);
}

BOOL CRtcStack::sendToWebRTCServer(const string& offersessionId, const string& plainMsg){
	int clientSocket;

	if((clientSocket = m_pSocket->getwcsFd()) == -1){
		LOG4CXX_ERROR(m_logger, "no wcs is connected");
		return FALSE;
	}
	if(!m_pSocket->sendMsgToRtc(plainMsg, clientSocket)){
		LOG4CXX_ERROR(m_logger, "send msg to "<<clientSocket<<" failed");
		return FALSE;
	}
	return TRUE;
}

