#include "CRoapParser.h"

const char* CRoapParser::TYPE = "type";
const char* CRoapParser::OFFER_SESSION_ID = "offerSessionId";
const char* CRoapParser::ANSWER_SESSION_ID = "answerSessionId";
const char* CRoapParser::SEQ = "seq";
const char* CRoapParser::SDP = "sdp";
const char* CRoapParser::ERROR = "error";
const char* CRoapParser::LABEL = "label";
const char* CRoapParser::TIEBREAKER = "tiebreaker";
const char* CRoapParser::MORE_COMING_FLAG = "moreComingFlag";
const char* CRoapParser::MSG_SIZE = "msgSize";
const char* CRoapParser::MSG_CONTENT = "msgContent";

CRoapParser::CRoapParser(){

}
CRoapParser::CRoapParser(Json::Value roapJson)
{
	m_roapJson = roapJson;
//	if(m_roapJson[TYPE] == ROAP_OFFER || m_roapJson[TYPE] == ROAP_ANSWER)
	if(getType() == ROAP_OFFER || getType() == ROAP_ANSWER)
		m_sdpJson = roapJson[SDP];
}

CRoapParser* CRoapParser::create(int type, string offerSessionId, string answerSessionId, int seq, string sdp,
		int label, int errorType, string tieBreaker, bool moreComing, int msgSize, string msgContent){
	Json::Value item, sdpJson;
	item[TYPE] = type;
	item[OFFER_SESSION_ID] = offerSessionId;
	item[ANSWER_SESSION_ID] = answerSessionId;
	item[SEQ] = seq;
	// webrtc的sdp包括type（只有offer和answer两个值）和sdp
	if(type == ROAP_ANSWER || type == ROAP_OFFER){
		if(type == ROAP_ANSWER)
			sdpJson[TYPE] = "answer";
		else
			sdpJson[TYPE] = "offer";
		sdpJson[SDP] = sdp;
		item[SDP] = sdpJson;
	}else{
		item[SDP] = sdp;
	}
	item[ERROR] = errorType;
	item[LABEL] = label;
	item[TIEBREAKER] = tieBreaker;
	item[MORE_COMING_FLAG] = moreComing;
	item[MSG_SIZE] = msgSize;
	item[MSG_CONTENT] = msgContent;
	return new CRoapParser(item);
}

int CRoapParser::getType(){
//	return m_roapJson[TYPE].asInt();
	if(!m_roapJson.isMember(TYPE))
		throw noKey_inJson_error(TYPE, m_roapJson.toStyledString());
	return m_roapJson[TYPE].asInt();
}

void CRoapParser::setType(int type){
	if(!m_roapJson.isMember(TYPE))
			throw noKey_inJson_error(TYPE, m_roapJson.toStyledString());
	m_roapJson[TYPE] = type;
}

string CRoapParser::getOfferSessionId(){
//	return m_roapJson[OFFER_SESSION_ID].asString();
	if(!m_roapJson.isMember(OFFER_SESSION_ID))
		throw noKey_inJson_error(OFFER_SESSION_ID, m_roapJson.toStyledString());
	return m_roapJson[OFFER_SESSION_ID].asString();
}

string CRoapParser::getAnswerSessionId(){
//	return m_roapJson[ANSWER_SESSION_ID].asString();
	if(!m_roapJson.isMember(ANSWER_SESSION_ID))
	{		
		//throw noKey_inJson_error(ANSWER_SESSION_ID, m_roapJson.toStyledString());
		return "";	
	}
	return m_roapJson[ANSWER_SESSION_ID].asString();
}

int CRoapParser::getSeq(){
//	return m_roapJson[SEQ].asInt();
	if(!m_roapJson.isMember(SEQ))
	{	
		//throw noKey_inJson_error(SEQ, m_roapJson.toStyledString());
		return -1;
	}
	return m_roapJson[SEQ].asInt();
}

string CRoapParser::getSdp(){
//	if(m_roapJson[TYPE] == ROAP_OFFER || m_roapJson[TYPE] == ROAP_ANSWER)
//		return m_sdpJson[SDP].asString();
//	else return m_roapJson[SDP].asString();
	Json::Value tmp;
	if(getType() == ROAP_OFFER || getType() == ROAP_ANSWER){
		if(!m_sdpJson.isMember(SDP))
			throw noKey_inJson_error(SDP, m_sdpJson.toStyledString());
		return m_sdpJson[SDP].asString();
	}else{
		if(!m_roapJson.isMember(SDP))
		{	
			//throw noKey_inJson_error(SDP, m_roapJson.toStyledString());
			return "";
		}
		return m_roapJson[SDP].asString();
	}
}


void CRoapParser::setSdp(string sdp){
	if(getType() == ROAP_OFFER || getType() == ROAP_ANSWER){
		if(!m_sdpJson.isMember(SDP))
			throw noKey_inJson_error(SDP, m_sdpJson.toStyledString());
		m_sdpJson[SDP] = sdp;
		m_roapJson[SDP] = m_sdpJson;
	}else{
		if(!m_roapJson.isMember(SDP))
			throw noKey_inJson_error(SDP, m_roapJson.toStyledString());
		m_roapJson[SDP] = sdp;
	}
}

int CRoapParser::getErrorType(){
//	return m_roapJson[ERROR].asInt();
	if(!m_roapJson.isMember(ERROR))
		throw noKey_inJson_error(ERROR, m_roapJson.toStyledString());
	return m_roapJson[ERROR].asInt();
}

int CRoapParser::getLabel(){
//	return m_roapJson[LABEL].asInt();
	if(!m_roapJson.isMember(LABEL))
	{	
		//throw noKey_inJson_error(LABEL, m_roapJson.toStyledString());
		return -1;
	}
	return m_roapJson[LABEL].asInt();
}

string CRoapParser::getTieBreaker(){
//	return m_roapJson[TIEBREAKER].asString();
	if(!m_roapJson.isMember(TIEBREAKER))
		throw noKey_inJson_error(TIEBREAKER, m_roapJson.toStyledString());
	return m_roapJson[TIEBREAKER].asString();
}

bool CRoapParser::isMoreComing(){
//	return m_roapJson[MORE_COMING_FLAG].asBool();
	if(!m_roapJson.isMember(MORE_COMING_FLAG))
		throw noKey_inJson_error(MORE_COMING_FLAG, m_roapJson.toStyledString());
	return m_roapJson[MORE_COMING_FLAG].asBool();
}

int CRoapParser::getMsgSize(){
//	return m_roapJson[MSG_SIZE].asInt();
	if(!m_roapJson.isMember(MSG_SIZE))
		throw noKey_inJson_error(MSG_SIZE, m_roapJson.toStyledString());
	return m_roapJson[MSG_SIZE].asInt();
}

string CRoapParser::getMsgContent(){
//	return m_roapJson[MSG_CONTENT].asString();
	if(!m_roapJson.isMember(MSG_CONTENT))
		throw noKey_inJson_error(MSG_CONTENT, m_roapJson.toStyledString());
	return m_roapJson[MSG_CONTENT].asString();
}

CRoapParser* CRoapParser::createOffer(string offerSessionId, string answerSessionId,
		int seq, string sdp, string tieBreaker){
	return create(ROAP_OFFER, offerSessionId, answerSessionId, seq, sdp,
			0, 0, tieBreaker, false, 0, "");
}

//CRoapParser* CRoapParser::createReOffer(string offerSessionId, string answerSessionId,
//		int seq, string sdp, string tieBreaker){
//	return create(ROAP_REOFFER, offerSessionId, answerSessionId, seq, sdp,
//			0, 0, tieBreaker, false, 0, "");
//}

CRoapParser* CRoapParser::createAnswer(string offerSessionId, string answerSessionId,
		int seq, string sdp, bool moreComing){
	return create(ROAP_ANSWER, offerSessionId, answerSessionId, seq, sdp,
			0, 0, "", moreComing, 0, "");
}
CRoapParser* CRoapParser::createOK(string offerSessionId, string answerSessionId, int seq, string sdp){
	return create(ROAP_OK, offerSessionId, answerSessionId, seq, sdp,
				0, 0, "", false, 0, "");
}
CRoapParser* CRoapParser::createShutdown(string offerSessionId, string answerSessionId, int seq){
	return create(ROAP_SHUTDOWN, offerSessionId, answerSessionId, seq, "",
			0, 0, "", false, 0, "");
}
CRoapParser* CRoapParser::createError(string offerSessionId, string answerSessionId,
			int seq, int errorType){
	return create(ROAP_ERROR, offerSessionId, answerSessionId, seq, "",
				0, errorType, "", false, 0, "");
}
CRoapParser* CRoapParser::createCandidate(string offerSessionId, string answerSessionId,
			int seq, string sdp, int label){
	return create(ROAP_CANDIDATE, offerSessionId, answerSessionId, seq, sdp,
				label, 0, "", false, 0, "");
}
CRoapParser* CRoapParser::createInfo(string offerSessionId, string answerSessionId,
			int seq, int content_length, string content){
	return create(ROAP_INFO, offerSessionId, answerSessionId, seq, "",
			0, 0, "", false, content_length, content);
}
CRoapParser* CRoapParser::createUpdate(string offerSessionId, string answerSessionId,
			int seq, int content_length, string content){
	return create(ROAP_UPDATE, offerSessionId, answerSessionId, seq, "",
			0, 0, "", false, content_length, content);
}

CRoapParser* CRoapParser::createNotify(string offerSessionId, string answerSessionId,
			int seq, int content_length, string content){
	return create(ROAP_NOTIFY, offerSessionId, answerSessionId, seq, "",
			0, 0, "", false, content_length, content);
}

CRoapParser* CRoapParser::createMessage(string offerSessionId, string answerSessionId,
			int seq, int msgSize, string msgContent){
	return create(ROAP_MESSAGE, offerSessionId, answerSessionId, seq, "",
				0, 0, "", false, msgSize, msgContent);
}
