#include "CMsgMapHelper.h"

pthread_mutex_t CMsgMapHelper::mutex = PTHREAD_MUTEX_INITIALIZER;
map<string, string> CMsgMapHelper::s_SessionIdMapSipCallId;
//map<string, toTagWithAnswerId> CMsgMapHelper::s_FromTagTotoTagWithAnswerId;

CVarChar64 CMsgMapHelper::generateRtcSessionID(CVarChar128 callIDNumber, CVarChar64 tag){
	return tag;
}

CVarChar128 CMsgMapHelper::generateSipCallIDNumber(CVarChar64 sessionID){
	string str(sessionID.c_str());
	str += str;
	printf("generateSipCallIDNumber: %s\n", str.c_str());
	CVarChar128 temp;
	temp = str.c_str();
	return temp;
}

CVarChar64 CMsgMapHelper::generateSipTag(CVarChar64 sessionID){
	return sessionID;
}

/*
CVarChar32 CMsgMapHelper::getRtcSessionID(CVarChar64 callIDNumber, CVarChar32 tag){
	pthread_mutex_lock(&mutex);
	string str = "";
	map<string, sipPair>::iterator it = s_RtcSessionIdMapSipCallIdTag.begin();
	for(; it != s_RtcSessionIdMapSipCallIdTag.end(); ++it){
		if(it->second._callIdNumber == callIDNumber && it->second._tag == tag){
			str = it->first;
			break;
		}
	}
	CVarChar32 temp;
	temp = str.c_str();
	pthread_mutex_unlock(&mutex);
	return temp;
}

CVarChar32 CMsgMapHelper::getSipTag(CVarChar32 sessionID){
	pthread_mutex_lock(&mutex);
	string strSessionID(sessionID.c_str());
	CVarChar32 tag;
	map<string, sipPair>::iterator it = s_RtcSessionIdMapSipCallIdTag.find(strSessionID);
	if(it != s_RtcSessionIdMapSipCallIdTag.end())
		tag = it->second._tag;
	pthread_mutex_unlock(&mutex);
	return tag;
}

CVarChar64 CMsgMapHelper::getSipCallIDNumber(CVarChar32 sessionID){
	pthread_mutex_lock(&mutex);
	string strSessionID(sessionID.c_str());
	CVarChar64 callIdNumber;
	map<string, sipPair>::iterator it = s_RtcSessionIdMapSipCallIdTag.find(strSessionID);
	if(it != s_RtcSessionIdMapSipCallIdTag.end())
		callIdNumber = it->second._callIdNumber;
	pthread_mutex_unlock(&mutex);
	return callIdNumber;
}

void CMsgMapHelper::storeRtcToSipMap(CVarChar32 sessionID, CVarChar64 callIDNumber, CVarChar32 tag){
	pthread_mutex_lock(&mutex);
	s_RtcSessionIdMapSipCallIdTag.insert(pair<string, sipPair>(sessionID.c_str(),
			sipPair(callIDNumber, tag)));
	DubugOut();
	pthread_mutex_unlock(&mutex);
}*/

void CMsgMapHelper::storeSipCallID(CVarChar64 sessionID, CVarChar128 callIDNumber){
	pthread_mutex_lock(&mutex);
	s_SessionIdMapSipCallId.insert(pair<string, string>(sessionID.c_str(),
			callIDNumber.c_str()));
	DubugOut();
	pthread_mutex_unlock(&mutex);
}


CVarChar128 CMsgMapHelper::getSipCallID(CVarChar64 sessionID){
	pthread_mutex_lock(&mutex);
	string strSessionID(sessionID.c_str());
	CVarChar128 callIdNumber;
	map<string, string>::iterator it = s_SessionIdMapSipCallId.find(strSessionID);
	if(it != s_SessionIdMapSipCallId.end())
		callIdNumber = it->second.c_str();
	pthread_mutex_unlock(&mutex);
	return callIdNumber;
	

}

void CMsgMapHelper::clearRtcToSipMap(CVarChar128 callIDNumber){
	pthread_mutex_lock(&mutex);
//	s_RtcSessionIdMapSipCallIdTag.erase(sessionID.c_str());
	map<string, string>::iterator it = s_SessionIdMapSipCallId.begin();
	for(; it != s_SessionIdMapSipCallId.end(); ++it){
		if(it->second == callIDNumber.c_str()){
			s_SessionIdMapSipCallId.erase(it--);
		}
	}
	pthread_mutex_unlock(&mutex);
}

void CMsgMapHelper::DubugOut(){
	if(true){
	  cout << "RtcSessionId to SipCallIdTag size: "<<s_SessionIdMapSipCallId.size()<<" contains: ";
	  for (map<string, string>::iterator it=s_SessionIdMapSipCallId.begin();
			  it!=s_SessionIdMapSipCallId.end(); ++it)
		 cout << it->first<<"----->callId:"<<it->second<<endl;
	  cout << endl;
	 /* cout<<"SipFromTag To toTagWithAnswerId size: "<<s_FromTagTotoTagWithAnswerId.size()<<endl;
	  for (map<string, toTagWithAnswerId>::iterator it1=s_FromTagTotoTagWithAnswerId.begin();
			  it1!=s_FromTagTotoTagWithAnswerId.end(); ++it1)
		 cout << it1->first<<"----->toTag:"<<it1->second.first.c_str()<<";answerId:"<<it1->second.second.c_str()<<endl;
	*/
	}
}
/*
void CMsgMapHelper::storeFromTagAndToTag(CVarChar32 fromTag, CVarChar32 toTag){
	pthread_mutex_lock(&mutex);
	CVarChar32 nullanswerId;
	s_FromTagTotoTagWithAnswerId.insert(pair<string, toTagWithAnswerId>(fromTag.c_str(), make_pair(toTag, nullanswerId)));
	pthread_mutex_unlock(&mutex);
}

void CMsgMapHelper::storeFromTagAndAnswerId(CVarChar32 fromTag, CVarChar32 answerSessionID){
	pthread_mutex_lock(&mutex);
	s_FromTagTotoTagWithAnswerId[fromTag.c_str()].second = answerSessionID;
	pthread_mutex_unlock(&mutex);
}

CVarChar32 CMsgMapHelper::getToTagByFromTag(CVarChar32 fromTag){
	pthread_mutex_lock(&mutex);
	CVarChar32 ret = s_FromTagTotoTagWithAnswerId[fromTag.c_str()].first;
	pthread_mutex_unlock(&mutex);
	return ret;
}

CVarChar32 CMsgMapHelper::getAnswerIdByFromTag(CVarChar32 fromTag){
	pthread_mutex_lock(&mutex);
	CVarChar32 ret = s_FromTagTotoTagWithAnswerId[fromTag.c_str()].second;
	pthread_mutex_unlock(&mutex);
	return ret;
}

void CMsgMapHelper::clearMapFromTagTotoTagWithAnswerId(CVarChar32 fromTag){
	pthread_mutex_lock(&mutex);
	s_FromTagTotoTagWithAnswerId.erase(fromTag.c_str());
	pthread_mutex_unlock(&mutex);
}*/
