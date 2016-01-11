#ifndef _RTC_PROTOCOL_PARSER_
#define _RTC_PROTOCOL_PARSER_
#include "json/json.h"
#include "CRoapParser.h"
#include <string>

using namespace std;
class CRtcProtocolParser{
private:
	Json::Value m_rtcProtocolJson;
	CRoapParser m_roapParser;


public:
	CRtcProtocolParser(const string& rtcProtocolStr);
	CRtcProtocolParser(int type, const string& from, const string& to, const CRoapParser& roapParser);
	int getType();
	string getFrom();
	string getTo();

	void setRoap(Json::Value roapJson);
	CRoapParser getRoapParser();

	string toPlainString();
	static const char* TYPE;
	static const char* FROM;
	static const char* TO;
	static const char* ROAP;
	/*
	static const char* TYPE = "type";
	static const char* FROM = "from";
	static const char* TO = "to";
	static const char* ROAP = "roap";
	*/
};

#endif
