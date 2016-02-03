/************************************************************************

 * Copyright (c)2010-2012, by BUPT
 * All rights reserved.

 * FileName：       exosipstack.h
 * System：         MCF
 * SubSystem：      PSA
 * Author：         Huang Haiping
 * Date：           2010.04.07
 * Version：        1.0
 * Description：
        控制eXosip2协议栈

 *
 * Last Modified:
	  2010.04.07, 完成初始版本定义
		 By Huang Haiping


*************************************************************************/

#ifndef _EXOSIPSTACK_H
#define _EXOSIPSTACK_H

#include <eXosip2/eXosip.h>
#include <osip2/osip_mt.h>
#include <osipparser2/headers/osip_www_authenticate.h>
#include <netinet/in.h>
#include <map>
#include <algorithm>


#include "MD5Digest.h"
#include "CPropertiesManager.h"

#include "comtypedef.h"
#include "pachook.h"
#include "unihashtable.h"
#include "exosiptranslator.h"
#include "timerpoll.h"

//#include "MyLogger.h"

typedef struct{
	string username;
	int reg_id;
	string cnonce;
	int noncecount;
} UserData;

class MyMap{
private:
	map<string, INT> _map;
public:
	void put(string key, INT value);
	bool get(string key, INT & val);
	void remove(string key);

};

_CLASSDEF(CExosipStack)
class CExosipStack {
public:
	CExosipStack(INT psaid);
	virtual ~CExosipStack();

	// INTERFACE
public:
	BOOL init(USHORT port);
	void doActive();
	BOOL doSendMsg(PTMsg msg);
	// INTERFACE END

private:
	INT m_psaid;
	MyMap m_map_branch_tid; // Transaction map
	MyMap m_map_callid; // call id map
	MyMap m_map_dialogid; // dialog id map


	map<string, vector<char *> > m_service_route;



	pthread_t thread_id;

	int accessMode;
	string confType;
	string confServer;
	string confRealm;

private:
	static void * thread_fun(void * data);
	static int sendInitRegister(timer *ptimer);
	static int sendAuthRegister(timer *ptimer);
	static int getSipUserFromDB(timer *ptimer);

	void register_process_401(eXosip_event_t * je);
	void register_process_200(eXosip_event_t * je);
	char * remove_quotes(char * text);
	char * add_quotes(string text);

	static string generateRand();


private:
	/* Id mapping function */
	void storeDid(RCTSipAddress from, RCTSipAddress to, RCTSipCallId callId, INT dialogId);
	void storeCid(RCTSipAddress from, RCTSipAddress to, RCTSipCallId callId, INT cid);

	void removeDid(RCTSipAddress from, RCTSipAddress to, RCTSipCallId callId);
	void removeCid(RCTSipAddress from, RCTSipAddress to, RCTSipCallId callId);

	/* -1 means not exist */
	INT getDid(RCTSipAddress from, RCTSipAddress to, RCTSipCallId callId);
	INT getCid(RCTSipAddress from, RCTSipAddress to, RCTSipCallId callId);

	/* Message handlers */
private:
	BOOL onSend_SIP_INVITE(PCTUniNetMsg uniMsg);
	BOOL onSend_SIP_REGISTER(PCTUniNetMsg uniMsg);
	BOOL onSend_SIP_MESSAGE(PCTUniNetMsg uniMsg);
	BOOL onSend_SIP_CANCEL(PCTUniNetMsg uniMsg);
	BOOL onSend_SIP_RESPONSE(PCTUniNetMsg uniMsg);
	BOOL onSend_SIP_ACK(PCTUniNetMsg uniMsg);
	BOOL onSend_SIP_BYE(PCTUniNetMsg uniMsg);
	BOOL onSend_SIP_INFO(PCTUniNetMsg uniMsg);
	BOOL onSend_SIP_PRACK(PCTUniNetMsg uniMsg);
	BOOL onSend_SIP_UPDATE(PCTUniNetMsg uniMsg);
};

#endif /* _EXOSIPSTACK_H */
