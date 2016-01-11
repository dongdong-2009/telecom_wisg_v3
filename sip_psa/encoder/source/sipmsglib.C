/*********************************************************************
 * Copyright (c)2010-2012, by BUPT
 * All rights reserved.
 * FileName：		sipmsglib.C
 * System：         MCF
 * SubSystem:		PSA SIP
 * Author：			Huang Haiping
 * Description：
 *		SIP 中间消息编解码器
 *
 * Last Modified:
 *      2010-9-27: First Edition
 *********************************************************************/
#include "sipmsglib.h"

CLONE_COMP(TSipMsgLib)
CREATE_COMP(TSipMsgLib)

TSipMsgLib::TSipMsgLib(PCGFSM afsm) : TMsgLib(afsm)
{

}

TSipMsgLib::~TSipMsgLib()
{

}

BOOL TSipMsgLib::GenMsg(PTUniNetMsg msg, CHAR* &buf)
{
	switch (msg->msgType)
	{
	case SIP_TYPE:
		return GenSipMsg(msg, buf);
		return TRUE;
	default:
		fsm->print("ERROR when GenMsg: no such msgtype %d in this msglib", msg->msgType);
		fsm->errorLog("ERROR when GenMsg: no such msgtype %d in this msglib", msg->msgType);
		return FALSE;
	}
}

BOOL TSipMsgLib::GenSipMsg(PTUniNetMsg msg, CHAR* &buf)
{
	DECODE_CTRLHDR(TSipCtrlMsg);

	switch (msg->msgName)
	{
	case SIP_RESPONSE:
		DECODE_MSGBODY(TSipResp);
		break;
	case SIP_REGISTER:
		DECODE_MSGBODY(TSipRegister);
		break;
	case SIP_INVITE:
		DECODE_MSGBODY(TSipInvite);
		break;
	case SIP_MESSAGE:
		DECODE_MSGBODY(TSipMessage);
		break;
	case SIP_ACK:
		DECODE_MSGBODY(TSipReq);
		break;
	case SIP_CANCEL:
		DECODE_MSGBODY(TSipCancel);
		break;
	case SIP_BYE:
		DECODE_MSGBODY(TSipBye);
		break;
	default:
		fsm->errorLog("Unrecognized msg name %u", msg->msgName);
		return FALSE;
	}
	return TRUE;
}

BOOL TSipMsgLib::GenMsg(PTUniNetMsg msg, TiXmlHandle& xmlParser)
{
// TODO: impl TSipMsgLib::GenMsg from XML
	return true;
}

BOOL TSipMsgLib::PolicyMsg(PCTUniNetMsg rmsg, PTUniNetMsg msg, UINT policy)
{
//TODO: impl TSipMsgLib::PolicyMsg
	return true;
}

