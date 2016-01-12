/*****************************************************************************
 * msgdef_rtc.C
 * It is an implementation file of message definition.
 * 
 * Note: This file is created automatically by msg compiler tool. 
 *       Please do not modify it.
 * 
 * Created at Mon Mar 11 15:13:55 2013
.
 * 
 ******************************************************************************/
#include "msgdef_rtc.h"
#include "info.h"


/////////////////////////////////////////////
//           for class TRtcCtrlMsg
/////////////////////////////////////////////
PTCtrlMsg TRtcCtrlMsg::clone()
{
	PTRtcCtrlMsg amsg = new TRtcCtrlMsg();
	amsg->optionSet                 = optionSet;

	amsg->orgAddr                   = orgAddr;

	amsg->rtcType                   = rtcType;
	amsg->from                      = from;
	amsg->to                        = to;
	amsg->offerSessionId            = offerSessionId;
	amsg->answerSessionId           = answerSessionId;
	return amsg;
}
TRtcCtrlMsg& TRtcCtrlMsg::operator=(const TRtcCtrlMsg &r)
{
	rtcType                   = r.rtcType;
	from                      = r.from;
	to                        = r.to;
	offerSessionId            = r.offerSessionId;
	answerSessionId           = r.answerSessionId;
	return *this;
}

BOOL TRtcCtrlMsg::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TRtcCtrlMsg,msg)

	COMPARE_FORCE_INT(TRtcCtrlMsg,rtcType)
	COMPARE_FORCE_VCHAR(TRtcCtrlMsg,from)
	COMPARE_FORCE_VCHAR(TRtcCtrlMsg,to)
	COMPARE_FORCE_VCHAR(TRtcCtrlMsg,offerSessionId)
	COMPARE_FORCE_VCHAR(TRtcCtrlMsg,answerSessionId)

	COMPARE_END
}

INT TRtcCtrlMsg::size()
{
	INT tmpSize = 0;

	tmpSize += sizeof(UINT); //for optionSet

	if( optionSet & orgAddr_flag )	tmpSize += orgAddr.size();

	tmpSize += sizeof(INT);
	tmpSize += from.size();
	tmpSize += to.size();
	tmpSize += offerSessionId.size();
	tmpSize += answerSessionId.size();

	return tmpSize;
}

INT TRtcCtrlMsg::encode(CHAR* &buf)
{
	ENCODE_INT( buf , optionSet )

	if( optionSet & orgAddr_flag )	orgAddr.encode(buf);

	ENCODE_INT( buf , rtcType )
	from.encode(buf);
	to.encode(buf);
	offerSessionId.encode(buf);
	answerSessionId.encode(buf);

	return size();
}

INT TRtcCtrlMsg::decode(CHAR* &buf)
{
	DECODE_INT( optionSet , buf )

	if( optionSet & orgAddr_flag )	orgAddr.decode(buf);

	DECODE_INT( rtcType, buf )
	from.decode(buf);
	to.decode(buf);
	offerSessionId.decode(buf);
	answerSessionId.decode(buf);

	return size();
}

BOOL TRtcCtrlMsg::decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_INT(TRtcCtrlMsg,rtcType)
	FILL_FORCE_VCHAR(TRtcCtrlMsg,from)
	FILL_FORCE_VCHAR(TRtcCtrlMsg,to)
	FILL_FORCE_VCHAR(TRtcCtrlMsg,offerSessionId)
	FILL_FORCE_VCHAR(TRtcCtrlMsg,answerSessionId)

	FILL_FIELD_END
}

void TRtcCtrlMsg::print(ostrstream& st)
{
	st<<"==| TRtcCtrlMsg =="<<endl;
	CHAR temp[30];
	sprintf(temp,"0x%x",optionSet);
	st<<"optionSet                    = "<<temp<<endl;

	if( optionSet & orgAddr_flag )
	{
		st<<"orgAddr : ";
		orgAddr.print(st);
	}
	else
		st<<"orgAddr                      = (not present)"<<endl;

	st<<"$rtcType                     = "<<rtcType<<endl;
	st<<"$from                        = "<<from.GetVarCharContentPoint()<<endl;
	st<<"$to                          = "<<to.GetVarCharContentPoint()<<endl;
	st<<"$offerSessionId              = "<<offerSessionId.GetVarCharContentPoint()<<endl;
	st<<"$answerSessionId             = "<<answerSessionId.GetVarCharContentPoint()<<endl;

}

/////////////////////////////////////////////
//           for class TRtcOffer
/////////////////////////////////////////////
PTMsgBody TRtcOffer::clone()
{
	PTRtcOffer amsg = new TRtcOffer();
	amsg->seq                       = seq;
	amsg->sdp                       = sdp;
	amsg->tieBreaker                = tieBreaker;
	return amsg;
}
TRtcOffer& TRtcOffer::operator=(const TRtcOffer &r)
{
	seq                       = r.seq;
	sdp                       = r.sdp;
	tieBreaker                = r.tieBreaker;
	return *this;
}

BOOL TRtcOffer::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TRtcOffer,msg)

	COMPARE_FORCE_INT(TRtcOffer,seq)
	COMPARE_FORCE_VCHAR(TRtcOffer,sdp)
	COMPARE_FORCE_VCHAR(TRtcOffer,tieBreaker)

	COMPARE_END
}

INT TRtcOffer::size()
{
	INT tmpSize = 0;

	tmpSize += sizeof(INT);
	tmpSize += sdp.size();
	tmpSize += tieBreaker.size();

	return tmpSize;
}

INT TRtcOffer::encode(CHAR* &buf)
{
	ENCODE_INT( buf , seq )
	sdp.encode(buf);
	tieBreaker.encode(buf);

	return size();
}

INT TRtcOffer::decode(CHAR* &buf)
{
	DECODE_INT( seq, buf )
	sdp.decode(buf);
	tieBreaker.decode(buf);

	return size();
}

BOOL TRtcOffer::decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_INT(TRtcOffer,seq)
	FILL_FORCE_VCHAR(TRtcOffer,sdp)
	FILL_FORCE_VCHAR(TRtcOffer,tieBreaker)

	FILL_FIELD_END
}

void TRtcOffer::print(ostrstream& st)
{
	st<<"==| TRtcOffer =="<<endl;
	st<<"$seq                         = "<<seq<<endl;
	st<<"$sdp                         = "<<sdp.GetVarCharContentPoint()<<endl;
	st<<"$tieBreaker                  = "<<tieBreaker.GetVarCharContentPoint()<<endl;

}

/////////////////////////////////////////////
//           for class TRtcAnswer
/////////////////////////////////////////////
PTMsgBody TRtcAnswer::clone()
{
	PTRtcAnswer amsg = new TRtcAnswer();
	amsg->seq                       = seq;
	amsg->sdp                       = sdp;
	amsg->moreComing                = moreComing;
	return amsg;
}
TRtcAnswer& TRtcAnswer::operator=(const TRtcAnswer &r)
{
	seq                       = r.seq;
	sdp                       = r.sdp;
	moreComing                = r.moreComing;
	return *this;
}

BOOL TRtcAnswer::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TRtcAnswer,msg)

	COMPARE_FORCE_INT(TRtcAnswer,seq)
	COMPARE_FORCE_VCHAR(TRtcAnswer,sdp)
	COMPARE_FORCE_BOOL(TRtcAnswer,moreComing)

	COMPARE_END
}

INT TRtcAnswer::size()
{
	INT tmpSize = 0;

	tmpSize += sizeof(INT);
	tmpSize += sdp.size();
	tmpSize += sizeof(BOOL);

	return tmpSize;
}

INT TRtcAnswer::encode(CHAR* &buf)
{
	ENCODE_INT( buf , seq )
	sdp.encode(buf);
	ENCODE_BOOL( buf , moreComing )

	return size();
}

INT TRtcAnswer::decode(CHAR* &buf)
{
	DECODE_INT( seq, buf )
	sdp.decode(buf);
	DECODE_BOOL( moreComing, buf )

	return size();
}

BOOL TRtcAnswer::decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_INT(TRtcAnswer,seq)
	FILL_FORCE_VCHAR(TRtcAnswer,sdp)
	FILL_FORCE_BOOL(TRtcAnswer,moreComing)

	FILL_FIELD_END
}

void TRtcAnswer::print(ostrstream& st)
{
	st<<"==| TRtcAnswer =="<<endl;
	st<<"$seq                         = "<<seq<<endl;
	st<<"$sdp                         = "<<sdp.GetVarCharContentPoint()<<endl;
	st<<"$moreComing                  = "<<getBoolValueStr(moreComing)<<endl;

}

/////////////////////////////////////////////
//           for class TRtcOK
/////////////////////////////////////////////
PTMsgBody TRtcOK::clone()
{
	PTRtcOK amsg = new TRtcOK();
	amsg->seq                       = seq;
	amsg->sdp						= sdp;
	return amsg;
}
TRtcOK& TRtcOK::operator=(const TRtcOK &r)
{
	seq                       = r.seq;
	sdp						  = r.sdp;
	return *this;
}

BOOL TRtcOK::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TRtcOK,msg)

	COMPARE_FORCE_INT(TRtcOK,seq)
	COMPARE_FORCE_VCHAR(TRtcOK,sdp);

	COMPARE_END
}

INT TRtcOK::size()
{
	INT tmpSize = 0;

	tmpSize += sizeof(INT);
	tmpSize += sdp.size();

	return tmpSize;
}

INT TRtcOK::encode(CHAR* &buf)
{
	ENCODE_INT( buf , seq )
	sdp.encode(buf);

	return size();
}

INT TRtcOK::decode(CHAR* &buf)
{
	DECODE_INT( seq, buf )
	sdp.decode(buf);

	return size();
}

BOOL TRtcOK::decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_INT(TRtcOK,seq)
	FILL_FORCE_VCHAR(TRtcAnswer,sdp)

	FILL_FIELD_END
}

void TRtcOK::print(ostrstream& st)
{
	st<<"==| TRtcOK =="<<endl;
	st<<"$seq                         = "<<seq<<endl;
	st<<"$sdp						  = "<<sdp.GetVarCharContentPoint()<<endl;

}

/////////////////////////////////////////////
//           for class TRtcShutdown
/////////////////////////////////////////////
PTMsgBody TRtcShutdown::clone()
{
	PTRtcShutdown amsg = new TRtcShutdown();
	amsg->seq                       = seq;
	return amsg;
}
TRtcShutdown& TRtcShutdown::operator=(const TRtcShutdown &r)
{
	seq                       = r.seq;
	return *this;
}

BOOL TRtcShutdown::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TRtcShutdown,msg)

	COMPARE_FORCE_INT(TRtcShutdown,seq)

	COMPARE_END
}

INT TRtcShutdown::size()
{
	INT tmpSize = 0;

	tmpSize += sizeof(INT);

	return tmpSize;
}

INT TRtcShutdown::encode(CHAR* &buf)
{
	ENCODE_INT( buf , seq )

	return size();
}

INT TRtcShutdown::decode(CHAR* &buf)
{
	DECODE_INT( seq, buf )

	return size();
}

BOOL TRtcShutdown::decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_INT(TRtcShutdown,seq)

	FILL_FIELD_END
}

void TRtcShutdown::print(ostrstream& st)
{
	st<<"==| TRtcShutdown =="<<endl;
	st<<"$seq                         = "<<seq<<endl;

}

/////////////////////////////////////////////
//           for class TRtcError
/////////////////////////////////////////////
PTMsgBody TRtcError::clone()
{
	PTRtcError amsg = new TRtcError();
	amsg->seq                       = seq;
	amsg->errorType                 = errorType;
	return amsg;
}
TRtcError& TRtcError::operator=(const TRtcError &r)
{
	seq                       = r.seq;
	errorType                 = r.errorType;
	return *this;
}

BOOL TRtcError::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TRtcError,msg)

	COMPARE_FORCE_INT(TRtcError,seq)
	COMPARE_FORCE_INT(TRtcError,errorType)

	COMPARE_END
}

INT TRtcError::size()
{
	INT tmpSize = 0;

	tmpSize += sizeof(INT);
	tmpSize += sizeof(INT);

	return tmpSize;
}

INT TRtcError::encode(CHAR* &buf)
{
	ENCODE_INT( buf , seq )
	ENCODE_INT( buf , errorType )

	return size();
}

INT TRtcError::decode(CHAR* &buf)
{
	DECODE_INT( seq, buf )
	DECODE_INT( errorType, buf )

	return size();
}

BOOL TRtcError::decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_INT(TRtcError,seq)
	FILL_FORCE_INT(TRtcError,errorType)

	FILL_FIELD_END
}

void TRtcError::print(ostrstream& st)
{
	st<<"==| TRtcError =="<<endl;
	st<<"$seq                         = "<<seq<<endl;
	st<<"$errorType                   = "<<errorType<<endl;

}

/////////////////////////////////////////////
//           for class TRtcCandidate
/////////////////////////////////////////////
PTMsgBody TRtcCandidate::clone()
{
	PTRtcCandidate amsg = new TRtcCandidate();
	amsg->seq                       = seq;
	amsg->label                     = label;
	amsg->sdp                       = sdp;
	return amsg;
}
TRtcCandidate& TRtcCandidate::operator=(const TRtcCandidate &r)
{
	seq                       = r.seq;
	label                     = r.label;
	sdp                       = r.sdp;
	return *this;
}

BOOL TRtcCandidate::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TRtcCandidate,msg)

	COMPARE_FORCE_INT(TRtcCandidate,seq)
	COMPARE_FORCE_INT(TRtcCandidate,label)
	COMPARE_FORCE_VCHAR(TRtcCandidate,sdp)

	COMPARE_END
}

INT TRtcCandidate::size()
{
	INT tmpSize = 0;

	tmpSize += sizeof(INT);
	tmpSize += sizeof(INT);
	tmpSize += sdp.size();

	return tmpSize;
}

INT TRtcCandidate::encode(CHAR* &buf)
{
	ENCODE_INT( buf , seq )
	ENCODE_INT( buf , label )
	sdp.encode(buf);

	return size();
}

INT TRtcCandidate::decode(CHAR* &buf)
{
	DECODE_INT( seq, buf )
	DECODE_INT( label, buf )
	sdp.decode(buf);

	return size();
}

BOOL TRtcCandidate::decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_INT(TRtcCandidate,seq)
	FILL_FORCE_INT(TRtcCandidate,label)
	FILL_FORCE_VCHAR(TRtcCandidate,sdp)

	FILL_FIELD_END
}

void TRtcCandidate::print(ostrstream& st)
{
	st<<"==| TRtcCandidate =="<<endl;
	st<<"$seq                         = "<<seq<<endl;
	st<<"$label                       = "<<label<<endl;
	st<<"$sdp                         = "<<sdp.GetVarCharContentPoint()<<endl;

}

/////////////////////////////////////////////
//           for class TRtcInfo
/////////////////////////////////////////////
PTMsgBody TRtcInfo::clone()
{
	PTRtcInfo amsg = new TRtcInfo();
	amsg->seq 					 = seq;
	amsg->content_length		 = content_length;
	amsg->content				 = content;

	return

	amsg;
}
TRtcInfo& TRtcInfo::operator =(const TRtcInfo &r)
{
	seq 				= r.seq;
	content_length		= r.content_length;
	content				= r.content;

	return *this;
}
BOOL TRtcInfo::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TRtcInfo, msg)

	COMPARE_FORCE_INT(TRtcInfo, seq)
	COMPARE_FORCE_INT(TRtcInfo, content_length)
	COMPARE_FORCE_VCHAR(TRtcInfo, content)

	COMPARE_END
}
INT TRtcInfo::size()
{
	INT tmpSize = 0;

	tmpSize += sizeof(INT);
	tmpSize += sizeof(INT);
	tmpSize += content.size();

	return tmpSize;
}

INT TRtcInfo::encode(CHAR* &buf)
{
	ENCODE_INT( buf , seq )
	ENCODE_INT( buf , content_length )
	content.encode(buf);

	return size();
}

INT TRtcInfo::decode(CHAR* &buf)
{
	DECODE_INT( seq, buf )
	DECODE_INT( content_length, buf )
	content.decode(buf);

	return size();
}

BOOL TRtcInfo::decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_INT(TRtcInfo,seq)
	FILL_FORCE_INT(TRtcInfo,content_length)
	FILL_FORCE_VCHAR(TRtcInfo,content)

	FILL_FIELD_END
}

void TRtcInfo::print(ostrstream& st)
{
	st<<"==| TRtcInfo =="<<endl;
	st<<"$seq                         = "<<seq<<endl;
	st<<"$content_length                     = "<<content_length<<endl;
	st<<"$content                  = "<<content.GetVarCharContentPoint()<<endl;

}

/////////////////////////////////////////////
//           for class TRtcUpdate
/////////////////////////////////////////////
PTMsgBody TRtcUpdate::clone()
{
	PTRtcUpdate amsg = new TRtcUpdate();
	amsg->seq 					 = seq;
	amsg->content_length		 = content_length;
	amsg->content				 = content;

	return

	amsg;
}
TRtcUpdate& TRtcUpdate::operator =(const TRtcUpdate &r)
{
	seq 				= r.seq;
	content_length		= r.content_length;
	content				= r.content;

	return *this;
}
BOOL TRtcUpdate::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TRtcUpdate, msg)

	COMPARE_FORCE_INT(TRtcUpdate, seq)
	COMPARE_FORCE_INT(TRtcUpdate, content_length)
	COMPARE_FORCE_VCHAR(TRtcUpdate, content)

	COMPARE_END
}
INT TRtcUpdate::size()
{
	INT tmpSize = 0;

	tmpSize += sizeof(INT);
	tmpSize += sizeof(INT);
	tmpSize += content.size();

	return tmpSize;
}

INT TRtcUpdate::encode(CHAR* &buf)
{
	ENCODE_INT( buf , seq )
	ENCODE_INT( buf , content_length )
	content.encode(buf);

	return size();
}

INT TRtcUpdate::decode(CHAR* &buf)
{
	DECODE_INT( seq, buf )
	DECODE_INT( content_length, buf )
	content.decode(buf);

	return size();
}

BOOL TRtcUpdate::decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_INT(TRtcUpdate,seq)
	FILL_FORCE_INT(TRtcUpdate,content_length)
	FILL_FORCE_VCHAR(TRtcUpdate,content)

	FILL_FIELD_END
}

void TRtcUpdate::print(ostrstream& st)
{
	st<<"==| TRtcUpdate =="<<endl;
	st<<"$seq                         = "<<seq<<endl;
	st<<"$content_length                     = "<<content_length<<endl;
	st<<"$content                  = "<<content.GetVarCharContentPoint()<<endl;

}


/////////////////////////////////////////////
//           for class TRtcMessage
/////////////////////////////////////////////
PTMsgBody TRtcMessage::clone()
{
	PTRtcMessage amsg = new TRtcMessage();
	amsg->seq                       = seq;
	amsg->msgSize                   = msgSize;
	amsg->msgContent                = msgContent;
	return amsg;
}
TRtcMessage& TRtcMessage::operator=(const TRtcMessage &r)
{
	seq                       = r.seq;
	msgSize                   = r.msgSize;
	msgContent                = r.msgContent;
	return *this;
}

BOOL TRtcMessage::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TRtcMessage,msg)

	COMPARE_FORCE_INT(TRtcMessage,seq)
	COMPARE_FORCE_INT(TRtcMessage,msgSize)
	COMPARE_FORCE_VCHAR(TRtcMessage,msgContent)

	COMPARE_END
}

INT TRtcMessage::size()
{
	INT tmpSize = 0;

	tmpSize += sizeof(INT);
	tmpSize += sizeof(INT);
	tmpSize += msgContent.size();

	return tmpSize;
}

INT TRtcMessage::encode(CHAR* &buf)
{
	ENCODE_INT( buf , seq )
	ENCODE_INT( buf , msgSize )
	msgContent.encode(buf);

	return size();
}

INT TRtcMessage::decode(CHAR* &buf)
{
	DECODE_INT( seq, buf )
	DECODE_INT( msgSize, buf )
	msgContent.decode(buf);

	return size();
}

BOOL TRtcMessage::decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_INT(TRtcMessage,seq)
	FILL_FORCE_INT(TRtcMessage,msgSize)
	FILL_FORCE_VCHAR(TRtcMessage,msgContent)

	FILL_FIELD_END
}

void TRtcMessage::print(ostrstream& st)
{
	st<<"==| TRtcMessage =="<<endl;
	st<<"$seq                         = "<<seq<<endl;
	st<<"$msgSize                     = "<<msgSize<<endl;
	st<<"$msgContent                  = "<<msgContent.GetVarCharContentPoint()<<endl;

}
