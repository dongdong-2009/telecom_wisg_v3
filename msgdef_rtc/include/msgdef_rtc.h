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



_CLASSDEF(TRtcCtrlMsg)
class TRtcCtrlMsg:public TCtrlMsg
{
	public:
		UINT           rtcType;
		CVarChar128    from;
		CVarChar128    to;
		CVarChar64     offerSessionId;
		CVarChar64     answerSessionId;

		inline         TRtcCtrlMsg();

		CHAR*          getMsgName(){ return "TRtcCtrlMsg";};
		TRtcCtrlMsg    &operator=(const TRtcCtrlMsg &r);
		PTCtrlMsg      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
};

_CLASSDEF(TRtcOffer)
class TRtcOffer:public TMsgBody
{
	public:
		UINT           seq;
		CVarChar       sdp;
		CVarChar128    tieBreaker;

		inline         TRtcOffer();

		CHAR*          getMsgName(){ return "TRtcOffer";};
		TRtcOffer      &operator=(const TRtcOffer &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
};

_CLASSDEF(TRtcAnswer)
class TRtcAnswer:public TMsgBody
{
	public:
		UINT           seq;
		CVarChar       sdp;
		BOOL           moreComing;

		inline         TRtcAnswer();

		CHAR*          getMsgName(){ return "TRtcAnswer";};
		TRtcAnswer     &operator=(const TRtcAnswer &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
};

_CLASSDEF(TRtcOK)
class TRtcOK:public TMsgBody
{
	public:
		UINT           seq;
		CVarChar       sdp;

		inline         TRtcOK();

		CHAR*          getMsgName(){ return "TRtcOK";};
		TRtcOK         &operator=(const TRtcOK &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
};

_CLASSDEF(TRtcShutdown)
class TRtcShutdown:public TMsgBody
{
	public:
		UINT           seq;

		inline         TRtcShutdown();

		CHAR*          getMsgName(){ return "TRtcShutdown";};
		TRtcShutdown   &operator=(const TRtcShutdown &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
};

_CLASSDEF(TRtcError)
class TRtcError:public TMsgBody
{
	public:
		UINT           seq;
		UINT           errorType;

		inline         TRtcError();

		CHAR*          getMsgName(){ return "TRtcError";};
		TRtcError      &operator=(const TRtcError &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
};

_CLASSDEF(TRtcCandidate)
class TRtcCandidate:public TMsgBody
{
	public:
		UINT           seq;
		UINT           label;
		CVarChar       sdp;

		inline         TRtcCandidate();

		CHAR*          getMsgName(){ return "TRtcCandidate";};
		TRtcCandidate  &operator=(const TRtcCandidate &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
};

_CLASSDEF(TRtcInfo)
class TRtcInfo:public TMsgBody
{
public:
	UINT seq;
	UINT content_length;
	CVarChar content;

	inline         TRtcInfo();

	CHAR*          getMsgName(){ return "TRtcInfo";};
	TRtcInfo    &operator=(const TRtcInfo &r);
	PTMsgBody      clone();
	BOOL           operator == (TMsgPara&);

	INT            size();
	INT            encode(CHAR* &buf);
	INT            decode(CHAR* &buf);
	BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

	void           print(ostrstream& st);
};

_CLASSDEF(TRtcUpdate)
class TRtcUpdate:public TMsgBody
{
public:
	UINT seq;
	UINT content_length;
	CVarChar content;

	inline         TRtcUpdate();

	CHAR*          getMsgName(){ return "TRtcUpdate";};
	TRtcUpdate    &operator=(const TRtcUpdate &r);
	PTMsgBody      clone();
	BOOL           operator == (TMsgPara&);

	INT            size();
	INT            encode(CHAR* &buf);
	INT            decode(CHAR* &buf);
	BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

	void           print(ostrstream& st);
};



_CLASSDEF(TRtcMessage)
class TRtcMessage:public TMsgBody
{
	public:
		UINT           seq;
		UINT           msgSize;
		CVarChar       msgContent;

		inline         TRtcMessage();

		CHAR*          getMsgName(){ return "TRtcMessage";};
		TRtcMessage    &operator=(const TRtcMessage &r);
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
inline TRtcCtrlMsg::TRtcCtrlMsg()
{
	rtcType                   = 0;
}

inline TRtcOffer::TRtcOffer()
{
	seq                       = 0;
}

inline TRtcAnswer::TRtcAnswer()
{
	seq                       = 0;
	moreComing                = FALSE;
}

inline TRtcOK::TRtcOK()
{
	seq                       = 0;
}

inline TRtcShutdown::TRtcShutdown()
{
	seq                       = 0;
}

inline TRtcError::TRtcError()
{
	seq                       = 0;
	errorType                 = 0;
}

inline TRtcCandidate::TRtcCandidate()
{
	seq                       = 0;
	label                     = 0;
}

inline TRtcInfo::TRtcInfo()
{
	seq 					  = 0;
	content_length 			  = 0;
}

inline TRtcUpdate::TRtcUpdate()
{
	seq 					  = 0;
	content_length 			  = 0;
}

inline TRtcMessage::TRtcMessage()
{
	seq                       = 0;
	msgSize                   = 0;
}


#endif
 
