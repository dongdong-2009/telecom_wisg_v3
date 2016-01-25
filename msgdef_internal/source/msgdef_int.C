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
#include "msgdef_int.h"
#include "info.h"


/////////////////////////////////////////////
//           for class TIntCtrlMsg
/////////////////////////////////////////////
PTCtrlMsg TIntCtrlMsg::clone()
{
	PTIntCtrlMsg amsg = new TIntCtrlMsg();
	amsg->optionSet                 = optionSet;

	amsg->orgAddr                   = orgAddr;

	amsg->intType                   = intType;
	amsg->from                      = from;
	amsg->to                        = to;
	amsg->sessionId            = sessionId;
	return amsg;
}
TIntCtrlMsg& TIntCtrlMsg::operator=(const TIntCtrlMsg &r)
{
	intType                   = r.intType;
	from                      = r.from;
	to                        = r.to;
	sessionId            = r.sessionId;
	return *this;
}

BOOL TIntCtrlMsg::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TIntCtrlMsg,msg)

	COMPARE_FORCE_INT(TIntCtrlMsg,intType)
	COMPARE_FORCE_VCHAR(TIntCtrlMsg,from)
	COMPARE_FORCE_VCHAR(TIntCtrlMsg,to)
	COMPARE_FORCE_VCHAR(TIntCtrlMsg,sessionId)

	COMPARE_END
}

INT TIntCtrlMsg::size()
{
	INT tmpSize = 0;

	tmpSize += sizeof(UINT); //for optionSet

	if( optionSet & orgAddr_flag )	tmpSize += orgAddr.size();

	tmpSize += sizeof(INT);
	tmpSize += from.size();
	tmpSize += to.size();
	tmpSize += sessionId.size();

	return tmpSize;
}

INT TIntCtrlMsg::encode(CHAR* &buf)
{
	ENCODE_INT( buf , optionSet )

	if( optionSet & orgAddr_flag )	orgAddr.encode(buf);

	ENCODE_INT( buf , intType )
	from.encode(buf);
	to.encode(buf);
	sessionId.encode(buf);

	return size();
}

INT TIntCtrlMsg::decode(CHAR* &buf)
{
	DECODE_INT( optionSet , buf )

	if( optionSet & orgAddr_flag )	orgAddr.decode(buf);

	DECODE_INT( intType, buf )
	from.decode(buf);
	to.decode(buf);
	sessionId.decode(buf);

	return size();
}

BOOL TIntCtrlMsg::decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_INT(TIntCtrlMsg,intType)
	FILL_FORCE_VCHAR(TIntCtrlMsg,from)
	FILL_FORCE_VCHAR(TIntCtrlMsg,to)
	FILL_FORCE_VCHAR(TIntCtrlMsg,sessionId)

	FILL_FIELD_END
}

void TIntCtrlMsg::print(ostrstream& st)
{
	st<<"==| TIntCtrlMsg =="<<endl;
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

	st<<"$intType                     = "<<intType<<endl;
	st<<"$from                        = "<<from.GetVarCharContentPoint()<<endl;
	st<<"$to                          = "<<to.GetVarCharContentPoint()<<endl;
	st<<"$sessionId              = "<<sessionId.GetVarCharContentPoint()<<endl;

}

/////////////////////////////////////////////
//           for class TIntRequest
/////////////////////////////////////////////
PTMsgBody TIntRequest::clone()
{
	PTIntRequest amsg = new TIntRequest();
	amsg->body                       = body;
	return amsg;
}
TIntRequest& TIntRequest::operator=(const TIntRequest &r)
{
	body                       = r.body;
	return *this;
}

BOOL TIntRequest::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TIntRequest,msg)

	COMPARE_FORCE_VCHAR(TIntRequest,body)

	COMPARE_END
}

INT TIntRequest::size()
{
	INT tmpSize = 0;

	tmpSize += body.size();

	return tmpSize;
}

INT TIntRequest::encode(CHAR* &buf)
{
	body.encode(buf);

	return size();
}

INT TIntRequest::decode(CHAR* &buf)
{
	body.decode(buf);

	return size();
}

BOOL TIntRequest::decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_VCHAR(TIntRequest,body)

	FILL_FIELD_END
}

void TIntRequest::print(ostrstream& st)
{
	st<<"==| TIntRequest =="<<endl;
	st<<"$body                         = "<<body.GetVarCharContentPoint()<<endl;

}

/////////////////////////////////////////////
//           for class TIntResponse
/////////////////////////////////////////////
PTMsgBody TIntResponse::clone()
{
	PTIntResponse amsg = new TIntResponse();
	amsg->body                       = body;
	return amsg;
}
TIntResponse& TIntResponse::operator=(const TIntResponse &r)
{
	body                       = r.body;
	return *this;
}

BOOL TIntResponse::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TIntResponse,msg)

	COMPARE_FORCE_VCHAR(TIntResponse,body)

	COMPARE_END
}

INT TIntResponse::size()
{
	INT tmpSize = 0;

	tmpSize += body.size();

	return tmpSize;
}

INT TIntResponse::encode(CHAR* &buf)
{
	body.encode(buf);

	return size();
}

INT TIntResponse::decode(CHAR* &buf)
{
	body.decode(buf);

	return size();
}

BOOL TIntResponse::decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_VCHAR(TIntResponse,body)

	FILL_FIELD_END
}

void TIntResponse::print(ostrstream& st)
{
	st<<"==| TIntResponse =="<<endl;
	st<<"$body                         = "<<body.GetVarCharContentPoint()<<endl;

}

/////////////////////////////////////////////
//           for class TIntError
/////////////////////////////////////////////
PTMsgBody TIntError::clone()
{
	PTIntError amsg = new TIntError();
	amsg->errorType						= errorType;
	return amsg;
}
TIntError& TIntError::operator=(const TIntError &r)
{
	errorType						  = r.errorType;
	return *this;
}

BOOL TIntError::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TIntError,msg)
	COMPARE_FORCE_INT(TIntError, errorType);

	COMPARE_END
}

INT TIntError::size()
{
	INT tmpSize = 0;

	tmpSize += sizeof(UINT);

	return tmpSize;
}

INT TIntError::encode(CHAR* &buf)
{
	ENCODE_INT( buf , errorType )
	return size();
}

INT TIntError::decode(CHAR* &buf)
{
	DECODE_INT(errorType, buf)

	return size();
}

BOOL TIntError::decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_INT(TIntError, errorType)

	FILL_FIELD_END
}

void TIntError::print(ostrstream& st)
{
	st<<"==| TIntError =="<<endl;
	st<<"$error						  = "<<errorType<<endl;

}

/////////////////////////////////////////////
//           for class TIntClose
/////////////////////////////////////////////
PTMsgBody TIntClose::clone()
{
	PTIntClose amsg = new TIntClose();
	return amsg;
}
TIntClose& TIntClose::operator=(const TIntClose &r)
{
	return *this;
}

BOOL TIntClose::operator == (TMsgPara& msg)
{
//	COMPARE_MSG_BEGIN(TIntClose,msg)

	COMPARE_END
}

INT TIntClose::size()
{
	INT tmpSize = 0;


	return tmpSize;
}

INT TIntClose::encode(CHAR* &buf)
{

	return size();
}

INT TIntClose::decode(CHAR* &buf)
{

	return size();
}

BOOL TIntClose::decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm)
{
	FILL_FIELD_BEGIN

	FILL_FIELD_END
}

void TIntClose::print(ostrstream& st)
{
	st<<"==| TIntClose =="<<endl;
	st<<"empty"<<endl;

}

