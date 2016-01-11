#ifndef _CMSGMAPHELPER_H
#define _CMSGMAPHELPER_H
#include "msgdef_uninet.h"
#include <map>
#include <iostream>
#include "dbHandler.h"
using namespace std;
typedef struct _pair{
	CVarChar64 _callIdNumber;
	CVarChar32 _tag;
	_pair(CVarChar64 callIdNumber, CVarChar32 tag):_callIdNumber(callIdNumber), _tag(tag){}
	_pair(){}
	_pair(const _pair& another){
		_callIdNumber = another._callIdNumber;
		_tag = another._tag;
	}
}sipPair;

typedef pair<CVarChar32, CVarChar32> toTagWithAnswerId;
class CMsgMapHelper{
public :
	static CVarChar64 generateRtcSessionID(CVarChar128 callIDNumber, CVarChar64 tag);
	static CVarChar128 generateSipCallIDNumber(CVarChar64 sessionID);
	static CVarChar64 generateSipTag(CVarChar64 sessionID);

//	static CVarChar32 getRtcSessionID(CVarChar64 callIDNumber, CVarChar32 tag);


	static CVarChar64 getSipTag(CVarChar64 sessionID);
//	static CVarChar64 getSipCallIDNumber(CVarChar32 sessionID);

	static CVarChar128 getSipCallID(CVarChar64 sessionID);
	
	static void storeSipCallID(CVarChar64 sessionID, CVarChar128 callIDNumber);
//	static void storeRtcToSipMap(CVarChar32 sessionID, CVarChar64 callIDNumber, CVarChar32 tag);
	static void clearRtcToSipMap(CVarChar128 callIDNumber);
private:
	// rtc sessionID到Sip callid-number和tag的映射，全局，需要加锁
//	static map<string, sipPair> s_RtcSessionIdMapSipCallIdTag;

	static map<string, string> s_SessionIdMapSipCallId;
	static pthread_mutex_t mutex;

	static void DubugOut();

	// add lms 5-14 记录fromtag 到 《totag， answersessioId》的映射
	static map<string, toTagWithAnswerId> s_FromTagTotoTagWithAnswerId;
public:
	/*
	// invite from xlite
	static void storeFromTagAndToTag(CVarChar32 fromTag, CVarChar32 toTag);
	// answer from webrtc
	static void storeFromTagAndAnswerId(CVarChar32 fromTag, CVarChar32 answerSessionID);
	// before send answer/bye(if xlite call) from webrtc should use toTag not answersessionid
	static CVarChar32 getToTagByFromTag(CVarChar32 fromTag);
	// before send ack/bye(if xlite call) from xlite should use answersessionid not toTag
	static CVarChar32 getAnswerIdByFromTag(CVarChar32 fromTag);
	// must be called
	static void clearMapFromTagTotoTagWithAnswerId(CVarChar32 fromTag);
	*/
};

#endif
