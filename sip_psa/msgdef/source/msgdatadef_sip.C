/*****************************************************************************
 * msgdatadef_sip.C
 * It is an implementation file of message definition.
 * 
 * Note: This file is created automatically by msg compiler tool. 
 *       Please do not modify it.
 * 
 * Created at Tue Feb 21 11:46:04 2012
.
 * 
 ******************************************************************************/
#include "msgdatadef_sip.h"
#include "info.h"


/////////////////////////////////////////////
//           for class TSipContentType
/////////////////////////////////////////////
PTMsgPara TSipContentType::clone()
{
	PTSipContentType amsg = new TSipContentType();
	amsg->type                      = type;
	amsg->subtype                   = subtype;
	return amsg;
}
TSipContentType& TSipContentType::operator=(const TSipContentType &r)
{
	type                      = r.type;
	subtype                   = r.subtype;
	return *this;
}

BOOL TSipContentType::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TSipContentType,msg)

	COMPARE_FORCE_VCHAR(TSipContentType,type)
	COMPARE_FORCE_VCHAR(TSipContentType,subtype)

	COMPARE_END
}

INT TSipContentType::size()
{
	INT tmpSize = 0;

	tmpSize += type.size();
	tmpSize += subtype.size();

	return tmpSize;
}

INT TSipContentType::encode(CHAR* &buf)
{
	type.encode(buf);
	subtype.encode(buf);

	return size();
}

INT TSipContentType::decode(CHAR* &buf)
{
	type.decode(buf);
	subtype.decode(buf);

	return size();
}

BOOL TSipContentType::decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_VCHAR(TSipContentType,type)
	FILL_FORCE_VCHAR(TSipContentType,subtype)

	FILL_FIELD_END
}

void TSipContentType::print(ostrstream& st)
{
	st<<"==| TSipContentType =="<<endl;
	st<<"$type                        = "<<type.GetVarCharContentPoint()<<endl;
	st<<"$subtype                     = "<<subtype.GetVarCharContentPoint()<<endl;

}

/////////////////////////////////////////////
//           for class TSipBody
/////////////////////////////////////////////
PTMsgPara TSipBody::clone()
{
	PTSipBody amsg = new TSipBody();
	amsg->content_length            = content_length;
	amsg->content                   = content;
	return amsg;
}
TSipBody& TSipBody::operator=(const TSipBody &r)
{
	content_length            = r.content_length;
	content                   = r.content;
	return *this;
}

BOOL TSipBody::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TSipBody,msg)

	COMPARE_FORCE_INT(TSipBody,content_length)
	COMPARE_FORCE_VCHAR(TSipBody,content)

	COMPARE_END
}

INT TSipBody::size()
{
	INT tmpSize = 0;

	tmpSize += sizeof(INT);
	tmpSize += content.size();

	return tmpSize;
}

INT TSipBody::encode(CHAR* &buf)
{
	ENCODE_INT( buf , content_length )
	content.encode(buf);

	return size();
}

INT TSipBody::decode(CHAR* &buf)
{
	DECODE_INT( content_length, buf )
	content.decode(buf);

	return size();
}

BOOL TSipBody::decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_INT(TSipBody,content_length)
	FILL_FORCE_VCHAR(TSipBody,content)

	FILL_FIELD_END
}

void TSipBody::print(ostrstream& st)
{
	st<<"==| TSipBody =="<<endl;
	st<<"$content_length              = "<<content_length<<endl;
	st<<"$content                     = "<<content.GetVarCharContentPoint()<<endl;

}

/////////////////////////////////////////////
//           for class TSipHeader
/////////////////////////////////////////////
PTMsgPara TSipHeader::clone()
{
	PTSipHeader amsg = new TSipHeader();
	amsg->hname                     = hname;
	amsg->hvalue                    = hvalue;
	return amsg;
}
TSipHeader& TSipHeader::operator=(const TSipHeader &r)
{
	hname                     = r.hname;
	hvalue                    = r.hvalue;
	return *this;
}

BOOL TSipHeader::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TSipHeader,msg)

	COMPARE_FORCE_VCHAR(TSipHeader,hname)
	COMPARE_FORCE_VCHAR(TSipHeader,hvalue)

	COMPARE_END
}

INT TSipHeader::size()
{
	INT tmpSize = 0;

	tmpSize += hname.size();
	tmpSize += hvalue.size();

	return tmpSize;
}

INT TSipHeader::encode(CHAR* &buf)
{
	hname.encode(buf);
	hvalue.encode(buf);

	return size();
}

INT TSipHeader::decode(CHAR* &buf)
{
	hname.decode(buf);
	hvalue.decode(buf);

	return size();
}

BOOL TSipHeader::decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_VCHAR(TSipHeader,hname)
	FILL_FORCE_VCHAR(TSipHeader,hvalue)

	FILL_FIELD_END
}

void TSipHeader::print(ostrstream& st)
{
	st<<"==| TSipHeader =="<<endl;
	st<<"$hname                       = "<<hname.GetVarCharContentPoint()<<endl;
	st<<"$hvalue                      = "<<hvalue.GetVarCharContentPoint()<<endl;

}

/////////////////////////////////////////////
//           for class TSipURIParam
/////////////////////////////////////////////
PTMsgPara TSipURIParam::clone()
{
	PTSipURIParam amsg = new TSipURIParam();
	amsg->gname                     = gname;
	amsg->gvalue                    = gvalue;
	return amsg;
}
TSipURIParam& TSipURIParam::operator=(const TSipURIParam &r)
{
	gname                     = r.gname;
	gvalue                    = r.gvalue;
	return *this;
}

BOOL TSipURIParam::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TSipURIParam,msg)

	COMPARE_FORCE_VCHAR(TSipURIParam,gname)
	COMPARE_FORCE_VCHAR(TSipURIParam,gvalue)

	COMPARE_END
}

INT TSipURIParam::size()
{
	INT tmpSize = 0;

	tmpSize += gname.size();
	tmpSize += gvalue.size();

	return tmpSize;
}

INT TSipURIParam::encode(CHAR* &buf)
{
	gname.encode(buf);
	gvalue.encode(buf);

	return size();
}

INT TSipURIParam::decode(CHAR* &buf)
{
	gname.decode(buf);
	gvalue.decode(buf);

	return size();
}

BOOL TSipURIParam::decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_VCHAR(TSipURIParam,gname)
	FILL_FORCE_VCHAR(TSipURIParam,gvalue)

	FILL_FIELD_END
}

void TSipURIParam::print(ostrstream& st)
{
	st<<"==| TSipURIParam =="<<endl;
	st<<"$gname                       = "<<gname.GetVarCharContentPoint()<<endl;
	st<<"$gvalue                      = "<<gvalue.GetVarCharContentPoint()<<endl;

}

/////////////////////////////////////////////
//           for class TSipURI
/////////////////////////////////////////////
PTMsgPara TSipURI::clone()
{
	PTSipURI amsg = new TSipURI();
	amsg->scheme                    = scheme;
	amsg->username                  = username;
	amsg->host                      = host;
	amsg->port                      = port;
	return amsg;
}
TSipURI& TSipURI::operator=(const TSipURI &r)
{
	scheme                    = r.scheme;
	username                  = r.username;
	host                      = r.host;
	port                      = r.port;
	return *this;
}

BOOL TSipURI::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TSipURI,msg)

	COMPARE_FORCE_VCHAR(TSipURI,scheme)
	COMPARE_FORCE_VCHAR(TSipURI,username)
	COMPARE_FORCE_VCHAR(TSipURI,host)
	COMPARE_FORCE_VCHAR(TSipURI,port)

	COMPARE_END
}

INT TSipURI::size()
{
	INT tmpSize = 0;

	tmpSize += scheme.size();
	tmpSize += username.size();
	tmpSize += host.size();
	tmpSize += port.size();

	return tmpSize;
}

INT TSipURI::encode(CHAR* &buf)
{
	scheme.encode(buf);
	username.encode(buf);
	host.encode(buf);
	port.encode(buf);

	return size();
}

INT TSipURI::decode(CHAR* &buf)
{
	scheme.decode(buf);
	username.decode(buf);
	host.decode(buf);
	port.decode(buf);

	return size();
}

BOOL TSipURI::decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_VCHAR(TSipURI,scheme)
	FILL_FORCE_VCHAR(TSipURI,username)
	FILL_FORCE_VCHAR(TSipURI,host)
	FILL_FORCE_VCHAR(TSipURI,port)

	FILL_FIELD_END
}

void TSipURI::print(ostrstream& st)
{
	st<<"==| TSipURI =="<<endl;
	st<<"$scheme                      = "<<scheme.GetVarCharContentPoint()<<endl;
	st<<"$username                    = "<<username.GetVarCharContentPoint()<<endl;
	st<<"$host                        = "<<host.GetVarCharContentPoint()<<endl;
	st<<"$port                        = "<<port.GetVarCharContentPoint()<<endl;

}

/////////////////////////////////////////////
//           for class TSipAddress
/////////////////////////////////////////////
PTMsgPara TSipAddress::clone()
{
	PTSipAddress amsg = new TSipAddress();
	amsg->displayname               = displayname;
	amsg->url                       = url;
	amsg->tag                       = tag;
	return amsg;
}
TSipAddress& TSipAddress::operator=(const TSipAddress &r)
{
	displayname               = r.displayname;
	url                       = r.url;
	tag                       = r.tag;
	return *this;
}

BOOL TSipAddress::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TSipAddress,msg)

	COMPARE_FORCE_VCHAR(TSipAddress,displayname)
	COMPARE_FORCE_NEST(TSipAddress,TSipURI,url)
	COMPARE_FORCE_VCHAR(TSipAddress,tag)

	COMPARE_END
}

INT TSipAddress::size()
{
	INT tmpSize = 0;

	tmpSize += displayname.size();
	tmpSize += url.size();
	tmpSize += tag.size();

	return tmpSize;
}

INT TSipAddress::encode(CHAR* &buf)
{
	displayname.encode(buf);
	url.encode(buf);
	tag.encode(buf);

	return size();
}

INT TSipAddress::decode(CHAR* &buf)
{
	displayname.decode(buf);
	url.decode(buf);
	tag.decode(buf);

	return size();
}

BOOL TSipAddress::decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_VCHAR(TSipAddress,displayname)
	FILL_FORCE_NEST(TSipAddress,TSipURI,url)
	FILL_FORCE_VCHAR(TSipAddress,tag)

	FILL_FIELD_END
}

void TSipAddress::print(ostrstream& st)
{
	st<<"==| TSipAddress =="<<endl;
	st<<"$displayname                 = "<<displayname.GetVarCharContentPoint()<<endl;
	st<<"$url : ";
	url.print(st);
	st<<"$tag                         = "<<tag.GetVarCharContentPoint()<<endl;

}

/////////////////////////////////////////////
//           for class TSipCallId
/////////////////////////////////////////////
PTMsgPara TSipCallId::clone()
{
	PTSipCallId amsg = new TSipCallId();
	amsg->number                    = number;
	amsg->host                      = host;
	return amsg;
}
TSipCallId& TSipCallId::operator=(const TSipCallId &r)
{
	number                    = r.number;
	host                      = r.host;
	return *this;
}

BOOL TSipCallId::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TSipCallId,msg)

	COMPARE_FORCE_VCHAR(TSipCallId,number)
	COMPARE_FORCE_VCHAR(TSipCallId,host)

	COMPARE_END
}

INT TSipCallId::size()
{
	INT tmpSize = 0;

	tmpSize += number.size();
	tmpSize += host.size();

	return tmpSize;
}

INT TSipCallId::encode(CHAR* &buf)
{
	number.encode(buf);
	host.encode(buf);

	return size();
}

INT TSipCallId::decode(CHAR* &buf)
{
	number.decode(buf);
	host.decode(buf);

	return size();
}

BOOL TSipCallId::decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_VCHAR(TSipCallId,number)
	FILL_FORCE_VCHAR(TSipCallId,host)

	FILL_FIELD_END
}

void TSipCallId::print(ostrstream& st)
{
	st<<"==| TSipCallId =="<<endl;
	st<<"$number                      = "<<number.GetVarCharContentPoint()<<endl;
	st<<"$host                        = "<<host.GetVarCharContentPoint()<<endl;

}

/////////////////////////////////////////////
//           for class TSipVia
/////////////////////////////////////////////
PTMsgPara TSipVia::clone()
{
	PTSipVia amsg = new TSipVia();
	amsg->branch                   = branch;
	amsg->url                     = url;
	return amsg;
}
TSipVia& TSipVia::operator=(const TSipVia &r)
{
	branch                    = r.branch;
	url                      = r.url;
	return *this;
}

BOOL TSipVia::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TSipVia,msg)

	COMPARE_FORCE_VCHAR(TSipVia,branch)
	COMPARE_FORCE_NEST(TSipVia,TSipURI,url)

	COMPARE_END
}

INT TSipVia::size()
{
	INT tmpSize = 0;

	tmpSize += branch.size();
	tmpSize += url.size();

	return tmpSize;
}

INT TSipVia::encode(CHAR* &buf)
{
	branch.encode(buf);
	url.encode(buf);

	return size();
}

INT TSipVia::decode(CHAR* &buf)
{
	branch.decode(buf);
	url.decode(buf);

	return size();
}

BOOL TSipVia::decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_VCHAR(TSipVia,branch)
	FILL_FORCE_NEST(TSipVia, TSipURI, url);

	FILL_FIELD_END
}

void TSipVia::print(ostrstream& st)
{
	st<<"==| TSipVia =="<<endl;
	st<<"$branch                      = "<<branch.GetVarCharContentPoint()<<endl;
	st<<"$url : ";
	url.print(st);

}



/////////////////////////////////////////////
//           for class TSipSubscriptionState
/////////////////////////////////////////////
PTMsgPara TSipSubscriptionState::clone()
{
	PTSipSubscriptionState amsg = new TSipSubscriptionState();
	amsg->state                     = state;
	amsg->expires                   = expires;
	amsg->reason                    = reason;
	return amsg;
}
TSipSubscriptionState& TSipSubscriptionState::operator=(const TSipSubscriptionState &r)
{
	state                     = r.state;
	expires                   = r.expires;
	reason                    = r.reason;
	return *this;
}

BOOL TSipSubscriptionState::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TSipSubscriptionState,msg)

	COMPARE_FORCE_VCHAR(TSipSubscriptionState,state)
	COMPARE_FORCE_INT(TSipSubscriptionState,expires)
	COMPARE_FORCE_VCHAR(TSipSubscriptionState,reason)

	COMPARE_END
}

INT TSipSubscriptionState::size()
{
	INT tmpSize = 0;

	tmpSize += state.size();
	tmpSize += sizeof(INT);
	tmpSize += reason.size();

	return tmpSize;
}

INT TSipSubscriptionState::encode(CHAR* &buf)
{
	state.encode(buf);
	ENCODE_INT( buf , expires )
	reason.encode(buf);

	return size();
}

INT TSipSubscriptionState::decode(CHAR* &buf)
{
	state.decode(buf);
	DECODE_INT( expires, buf )
	reason.decode(buf);

	return size();
}

BOOL TSipSubscriptionState::decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_VCHAR(TSipSubscriptionState,state)
	FILL_FORCE_INT(TSipSubscriptionState,expires)
	FILL_FORCE_VCHAR(TSipSubscriptionState,reason)

	FILL_FIELD_END
}

void TSipSubscriptionState::print(ostrstream& st)
{
	st<<"==| TSipSubscriptionState =="<<endl;
	st<<"$state                       = "<<state.GetVarCharContentPoint()<<endl;
	st<<"$expires                     = "<<expires<<endl;
	st<<"$reason                      = "<<reason.GetVarCharContentPoint()<<endl;

}
