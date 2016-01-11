/*****************************************************************************
 * msgdef_sip.h
 * It is a message declaration head file.
 * 
 * Note: This file is created automatically by msg compiler tool. 
 *       Please do not modify it.
 * 
 * Created at Tue Feb 21 11:46:04 2012
.
 * 
 ******************************************************************************/
#ifndef __MSGDEF_SIP_H_
#define __MSGDEF_SIP_H_

#include "comtypedef.h"
#include "comtypedef_uninet.h"
#include "comtypedef_vchar.h"
#include "comtypedef_fchar.h"
#include "msg.h"
#include "msgdef_uninet.h"
#include "gfsm.h"
#include "tinyxml.h"
#include "xmlmsgconvertor.h"

#include "msgdatadef_sip.h"
#include "comtypedef_fchar.h"

#include <time.h>
#include <strstream>
using namespace std;



_CLASSDEF(TSipCtrlMsg)
class TSipCtrlMsg:public TCtrlMsg
{
	public:
		TSipCallId     sip_callId;
		TSipAddress    from;
		TSipAddress    to;
		CVarChar16     cseq_method;
		CVarChar64     cseq_number;
		TSipVia		   via;
		TSipAddress    route;

		inline         TSipCtrlMsg();

		CHAR*          getMsgName(){ return "TSipCtrlMsg";};
		TSipCtrlMsg    &operator=(const TSipCtrlMsg &r);
		PTCtrlMsg      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
};

_CLASSDEF(TSipResp)
class TSipResp:public TMsgBody
{
	public:
		INT            statusCode;
		CVarChar64     reason_phase;
		INT            expires;
		TSipAddress    contact;
		CVarChar64     sip_etag;
		TSipContentType content_type;
		CVarChar16     authScheme;
		CVarChar64     authRealm;
		CVarChar       authNonce;
		CVarChar16     authStale;
		CVarChar16     authAlgor;
		CVarChar       authQop;
		CVarChar       authOpaque;
		TSipBody       body;

		inline         TSipResp();

		CHAR*          getMsgName(){ return "TSipResp";};
		TSipResp       &operator=(const TSipResp &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
};

_CLASSDEF(TSipReq)
class TSipReq:public TMsgBody
{
	public:
		TSipURI        req_uri;
		TSipContentType content_type;
		TSipBody       body;

		inline         TSipReq();

		CHAR*          getMsgName(){ return "TSipReq";};
		TSipReq        &operator=(const TSipReq &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
};




_CLASSDEF(TSipBye)
class TSipBye:public TMsgBody
{
	public:
		TSipURI        req_uri;

		inline         TSipBye();

		CHAR*          getMsgName(){ return "TSipBye";};
		TSipBye        &operator=(const TSipBye &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
};

_CLASSDEF(TSipCancel)
class TSipCancel:public TMsgBody
{
	public:
		TSipURI        req_uri;

		inline         TSipCancel();

		CHAR*          getMsgName(){ return "TSipCancel";};
		TSipCancel     &operator=(const TSipCancel &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
};

_CLASSDEF(TSipRegister)
class TSipRegister:public TMsgBody
{
	public:
		TSipURI        req_uri;
		TSipAddress    contact;
		INT            expires;
		CVarChar16     authScheme;
		CVarChar       authUserName;
		CVarChar64     authRealm;
		CVarChar       authNonce;
		CVarChar       authUri;
		CVarChar       authResponse;
		CVarChar16     authAlgor;
		CVarChar64     authCnonce;
		CVarChar       authOpaque;
		CVarChar       authQop;
		CVarChar       authNc;

		inline         TSipRegister();

		CHAR*          getMsgName(){ return "TSipRegister";};
		TSipRegister   &operator=(const TSipRegister &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
};

_CLASSDEF(TSipInvite)
class TSipInvite:public TMsgBody
{
	public:
		TSipURI        req_uri;
		TSipContentType content_type;
		TSipBody       body;

		inline         TSipInvite();

		CHAR*          getMsgName(){ return "TSipInvite";};
		TSipInvite     &operator=(const TSipInvite &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
};

_CLASSDEF(TSipMessage)
class TSipMessage:public TMsgBody
{
	public:
		TSipURI        req_uri;
		TSipContentType content_type;
		TSipBody       body;

		inline         TSipMessage();

		CHAR*          getMsgName(){ return "TSipMessage";};
		TSipMessage    &operator=(const TSipMessage &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
};

_CLASSDEF(TSipInfo)
class TSipInfo:public TMsgBody
{
	public:
		TSipURI        req_uri;
		TSipContentType content_type;
		TSipBody       body;

		inline         TSipInfo();

		CHAR*          getMsgName(){ return "TSipInfo";};
		TSipInfo       &operator=(const TSipInfo &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
};


_CLASSDEF(TSipPrack)
class TSipPrack:public TMsgBody
{
public:
		TSipURI        req_uri;
		TSipContentType content_type;
		TSipBody       body;

		inline         TSipPrack();

		CHAR*          getMsgName(){ return "TSipPrack";};
		TSipPrack       &operator=(const TSipPrack &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
};

_CLASSDEF(TSipUpdate)
class TSipUpdate:public TMsgBody
{
public:
	TSipURI        req_uri;
	TSipContentType content_type;
	TSipBody       body;

	inline         TSipUpdate();

	CHAR*          getMsgName(){ return "TSipUpdate";};
	TSipUpdate       &operator=(const TSipUpdate &r);
	PTMsgBody      clone();
	BOOL           operator == (TMsgPara&);

	INT            size();
	INT            encode(CHAR* &buf);
	INT            decode(CHAR* &buf);
	BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

	void           print(ostrstream& st);
};


_CLASSDEF(TSipPublish)
class TSipPublish:public TMsgBody
{
	public:
		TSipURI        req_uri;
		INT            expires;
		CVarChar64     sip_if_match;
		CVarChar64     event_type;
		TSipContentType content_type;
		TSipBody       body;

		inline         TSipPublish();

		CHAR*          getMsgName(){ return "TSipPublish";};
		TSipPublish    &operator=(const TSipPublish &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
};

_CLASSDEF(TSipSubscribe)
class TSipSubscribe:public TMsgBody
{
	public:
		TSipURI        req_uri;
		INT            expires;
		CVarChar64     event_type;
		TSipContentType content_type;
		TSipBody       body;

		inline         TSipSubscribe();

		CHAR*          getMsgName(){ return "TSipSubscribe";};
		TSipSubscribe  &operator=(const TSipSubscribe &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
};

_CLASSDEF(TSipNotify)
class TSipNotify:public TMsgBody
{
	public:
		TSipURI        req_uri;
		CVarChar64     event_type;
		TSipSubscriptionState subscription_state;
		TSipContentType content_type;
		TSipBody       body;

		inline         TSipNotify();

		CHAR*          getMsgName(){ return "TSipNotify";};
		TSipNotify     &operator=(const TSipNotify &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
};

//////////////////////////////////////////////////////
//         the implementations of inline function      
/////////////////////////////////////////////////////////
inline TSipCtrlMsg::TSipCtrlMsg()
{
}

inline TSipResp::TSipResp()
{
	statusCode                = 0;
	expires                   = 0;
}

inline TSipReq::TSipReq()
{
}

inline TSipBye::TSipBye()
{
}

inline TSipCancel::TSipCancel()
{
}

inline TSipRegister::TSipRegister()
{
	expires                   = 0;
}

inline TSipInvite::TSipInvite()
{
}

inline TSipMessage::TSipMessage()
{
}

inline TSipInfo::TSipInfo()
{

}


inline TSipPrack::TSipPrack(){

}

inline TSipUpdate::TSipUpdate()
{
}


inline TSipPublish::TSipPublish()
{
	expires                   = 0;
}

inline TSipSubscribe::TSipSubscribe()
{
	expires                   = 0;
}

inline TSipNotify::TSipNotify()
{
}


#endif
 
