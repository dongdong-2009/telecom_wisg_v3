#ifndef _ROAP_PARSER_
#define _ROAP_PARSER_
#include "json/json.h"
#include "CNoKeyInJsonError.h"
#include <string>
#include "constdef.h"
using namespace std;

class CRoapParser{
private:
	Json::Value m_roapJson;
	Json::Value m_sdpJson;

public:
	CRoapParser();
	CRoapParser(Json::Value roapJson);
	static CRoapParser* create(int type, string offerSessionId, string answerSessionId, int seq, string sdp,
			int label, int errorType, string tieBreaker, bool moreComing, int msgSize, string msgContent);
	static CRoapParser* createOffer(string offerSessionId, string answerSessionId,
			int seq, string sdp, string tieBreaker);

//	static CRoapParser* createReOffer(string offerSessionId, string answerSessionId,
//				int seq, string sdp, string tieBreaker);

	static CRoapParser* createAnswer(string offerSessionId, string answerSessionId,
			int seq, string sdp, bool moreComing);
	static CRoapParser* createOK(string offerSessionId, string answerSessionId, int seq, string sdp);
	static CRoapParser* createShutdown(string offerSessionId, string answerSessionId, int seq);
	static CRoapParser* createInfo(string offerSessionId, string answerSessionId,
			int seq, int content_length, string content);
	static CRoapParser* createUpdate(string offerSessionId, string answerSessionId,
			int seq, int content_length, string content);
	static CRoapParser* createNotify(string offerSessionId, string answerSessionId,
				int seq, int content_length, string content);

	static CRoapParser* createError(string offerSessionId, string answerSessionId,
				int seq, int errorType);
	static CRoapParser* createCandidate(string offerSessionId, string answerSessionId,
				int seq, string sdp, int label);
	static CRoapParser* createMessage(string offerSessionId, string answerSessionId,
				int seq, int msgSize, string msgContent);

	int getType();
	void setType(int type);
	string getOfferSessionId();
	string getAnswerSessionId();
	int getSeq();
	string getSdp();
	void setSdp(string sdp);


	int getLabel();
	int getErrorType();
	string getTieBreaker();
	bool isMoreComing();

	// add for im
	int getMsgSize();
	string getMsgContent();

	Json::Value getRoapJson(){
		return m_roapJson;
	}
	static const char* TYPE;
	static const char* OFFER_SESSION_ID;
	static const char* ANSWER_SESSION_ID;
	static const char* SEQ;
	static const char* SDP;
	static const char* ERROR;
	static const char* LABEL;
	static const char* TIEBREAKER;
	static const char* MORE_COMING_FLAG;
	static const char* MSG_SIZE;
	static const char* MSG_CONTENT;
};
#endif
