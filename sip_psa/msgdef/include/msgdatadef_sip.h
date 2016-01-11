/*****************************************************************************
 * msgdatadef_sip.h
 * It is a message declaration head file.
 * 
 * Note: This file is created automatically by msg compiler tool. 
 *       Please do not modify it.
 * 
 * Created at Tue Feb 21 11:46:04 2012
.
 * 
 ******************************************************************************/
#ifndef __MSGDATADEF_SIP_H_
#define __MSGDATADEF_SIP_H_

#include "comtypedef.h"
#include "comtypedef_uninet.h"
#include "comtypedef_vchar.h"
#include "comtypedef_fchar.h"
#include "msg.h"
#include "msgdef_uninet.h"
#include "gfsm.h"
#include "tinyxml.h"
#include "xmlmsgconvertor.h"


#include <time.h>
#include <strstream>
using namespace std;



_CLASSDEF(TSipContentType)
class TSipContentType:public TMsgPara
{
	public:
		CVarChar16     type;
		CVarChar16     subtype;

		inline         TSipContentType();

		CHAR*          getMsgName(){ return "TSipContentType";};
		TSipContentType &operator=(const TSipContentType &r);
		PTMsgPara      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
};

_CLASSDEF(TSipBody)
class TSipBody:public TMsgPara
{
	public:
		UINT           content_length;
		CVarChar       content;

		inline         TSipBody();

		CHAR*          getMsgName(){ return "TSipBody";};
		TSipBody       &operator=(const TSipBody &r);
		PTMsgPara      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
};

_CLASSDEF(TSipHeader)
class TSipHeader:public TMsgPara
{
	public:
		CVarChar128    hname;
		CVarChar128    hvalue;

		inline         TSipHeader();

		CHAR*          getMsgName(){ return "TSipHeader";};
		TSipHeader     &operator=(const TSipHeader &r);
		PTMsgPara      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
};

_CLASSDEF(TSipURIParam)
class TSipURIParam:public TMsgPara
{
	public:
		CVarChar128    gname;
		CVarChar128    gvalue;

		inline         TSipURIParam();

		CHAR*          getMsgName(){ return "TSipURIParam";};
		TSipURIParam   &operator=(const TSipURIParam &r);
		PTMsgPara      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
};

_CLASSDEF(TSipURI)
class TSipURI:public TMsgPara
{
	public:
		CVarChar16     scheme;
		CVarChar128    username;
		CVarChar128    host;
		CVarChar8      port;

		inline         TSipURI();

		CHAR*          getMsgName(){ return "TSipURI";};
		TSipURI        &operator=(const TSipURI &r);
		PTMsgPara      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
};

_CLASSDEF(TSipAddress)
class TSipAddress:public TMsgPara
{
	public:
		CVarChar256    displayname;
		TSipURI        url;
		CVarChar64     tag;

		inline         TSipAddress();

		CHAR*          getMsgName(){ return "TSipAddress";};
		TSipAddress    &operator=(const TSipAddress &r);
		PTMsgPara      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
};

_CLASSDEF(TSipCallId)
class TSipCallId:public TMsgPara
{
	public:
		CVarChar128     number;
		CVarChar64     host;

		inline         TSipCallId();

		CHAR*          getMsgName(){ return "TSipCallId";};
		TSipCallId     &operator=(const TSipCallId &r);
		PTMsgPara      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
};

_CLASSDEF(TSipSubscriptionState)
class TSipSubscriptionState:public TMsgPara
{
	public:
		CVarChar64     state;
		int            expires;
		CVarChar64     reason;

		inline         TSipSubscriptionState();

		CHAR*          getMsgName(){ return "TSipSubscriptionState";};
		TSipSubscriptionState &operator=(const TSipSubscriptionState &r);
		PTMsgPara      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
};

_CLASSDEF(TSipVia)
class TSipVia:public TMsgPara
{
	public:
		TSipURI        url;
		CVarChar64     branch;

		inline         TSipVia();

		CHAR*          getMsgName(){ return "TSipVia";};
		TSipVia        &operator=(const TSipVia &r);
		PTMsgPara      clone();
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
inline TSipContentType::TSipContentType()
{
}

inline TSipBody::TSipBody()
{
	content_length            = 0;
}

inline TSipHeader::TSipHeader()
{
}

inline TSipURIParam::TSipURIParam()
{
}

inline TSipURI::TSipURI()
{
}

inline TSipAddress::TSipAddress()
{
}

inline TSipCallId::TSipCallId()
{
}

inline TSipVia::TSipVia(){

}

inline TSipSubscriptionState::TSipSubscriptionState()
{
	expires                   = 0;
}


#endif
 
