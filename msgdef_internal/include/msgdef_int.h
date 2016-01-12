/*****************************************************************************
 * msgdef_rtc.h
 * It is a message declaration head file.
 * 
 * Note: This file is created automatically by msg compiler tool. 
 *       Please do not modify it.
 * 
 * Created at Mon Mar 11 15:13:55 2013
.
 * 
 ******************************************************************************/
#ifndef __MSGDEF_RTC_H_
#define __MSGDEF_RTC_H_

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



_CLASSDEF(TIntCtrlMsg)
class TIntCtrlMsg:public TCtrlMsg
{
	public:
		UINT           intType;
		CVarChar128    from;
		CVarChar128    to;
		CVarChar64     sessionId;

		inline         TIntCtrlMsg();

		CHAR*          getMsgName(){ return "TIntCtrlMsg";};
		TIntCtrlMsg    &operator=(const TIntCtrlMsg &r);
		PTCtrlMsg      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
};

_CLASSDEF(TIntRequest)
class TIntRequest:public TMsgBody
{
	public:

		CVarChar       body;


		inline         TIntRequest();

		CHAR*          getMsgName(){ return "TIntRequest";};
		TIntRequest      &operator=(const TIntRequest &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
};

_CLASSDEF(TIntResponse)
class TIntResponse:public TMsgBody
{
	public:
		CVarChar       body;

		inline         TIntResponse();

		CHAR*          getMsgName(){ return "TIntResponse";};
		TIntResponse     &operator=(const TIntResponse &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
};

_CLASSDEF(TIntError)
class TIntError:public TMsgBody
{
	public:
		CVarChar       reason_phase;

		inline         TIntError();

		CHAR*          getMsgName(){ return "TIntError";};
		TIntError         &operator=(const TIntError &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
};

_CLASSDEF(TIntClose)
class TIntClose:public TMsgBody
{
	public:

		inline         TIntClose();

		CHAR*          getMsgName(){ return "TIntClose";};
		TIntClose   &operator=(const TIntClose &r);
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
inline TIntCtrlMsg::TIntCtrlMsg()
{
	intType                   = 0;
}

inline TIntRequest::TIntRequest()
{
}

inline TIntResponse::TIntResponse()
{
}

inline TIntError::TIntError()
{
}

inline TIntClose::TIntClose()
{
}


#endif
 
