#include "CRtcProtocolParser.h"
#include <stdio.h>

const char* CRtcProtocolParser::TYPE = "type";
const char* CRtcProtocolParser::FROM = "from";
const char* CRtcProtocolParser::TO = "to";
const char* CRtcProtocolParser::ROAP = "roap";

CRtcProtocolParser::CRtcProtocolParser(const string& rtcProtocolStr){
	Json::Reader reader;
	if(reader.parse(rtcProtocolStr, m_rtcProtocolJson) == false || m_rtcProtocolJson.type() != Json::objectValue){
		throw runtime_error(rtcProtocolStr + " cannot be parsed as json");
	}
	Json::Value roap = m_rtcProtocolJson[ROAP];
	m_roapParser = CRoapParser(roap);
}


CRtcProtocolParser::CRtcProtocolParser(int type, const string& from, const string& to, const CRoapParser& roapParser)
	:m_roapParser(roapParser){
	Json::Value item;
	item[TYPE] = type;
	item[FROM] = from;
	item[TO] = to;
	item[ROAP] = m_roapParser.getRoapJson();
	m_rtcProtocolJson = item;
}

string CRtcProtocolParser::getTo(){
//	return m_rtcProtocolJson[TO].asString();
	if(!m_rtcProtocolJson.isMember(TO))
		throw noKey_inJson_error(TO, m_rtcProtocolJson.toStyledString());
	return m_rtcProtocolJson[TO].asString();
}

string CRtcProtocolParser::getFrom(){
//	return m_rtcProtocolJson[FROM].asString();
	if(!m_rtcProtocolJson.isMember(FROM))
		throw noKey_inJson_error(FROM, m_rtcProtocolJson.toStyledString());
	return m_rtcProtocolJson[FROM].asString();
}

int CRtcProtocolParser::getType(){
//	return m_rtcProtocolJson[TYPE].asInt();
	if(!m_rtcProtocolJson.isMember(TYPE))
		throw noKey_inJson_error(TYPE, m_rtcProtocolJson.toStyledString());
	return m_rtcProtocolJson[TYPE].asInt();
}

void CRtcProtocolParser::setRoap(Json::Value roapJson){
	m_rtcProtocolJson[ROAP] = roapJson;
}


CRoapParser CRtcProtocolParser::getRoapParser(){
	return m_roapParser;
}

string CRtcProtocolParser::toPlainString(){
	Json::FastWriter writer;

	return writer.write(m_rtcProtocolJson);
}
