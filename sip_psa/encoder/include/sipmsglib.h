/*********************************************************************
 * Copyright (c)2010-2012, by BUPT
 * All rights reserved.
 * FileName：		sipmsglib.h
 * System：         MCF
 * SubSystem:		PSA SIP
 * Author：			Huang Haiping
 * Description：
 *		
 *
 * Last Modified:
 *      2010-9-27: First Edition
 *********************************************************************/

#ifndef SIPMSGLIB_H_
#define SIPMSGLIB_H_

#include "msglib.h"
#include "psa_sip_inf.h"

_CLASSDEF(TSipMsgLib)
class TSipMsgLib : public TMsgLib
{
public:
	TSipMsgLib(PCGFSM afsm);
	~TSipMsgLib();

	DECLARE_CLONE();

	void list() {}// this method implemented later.

	// decode from bin msg buffer to C++ TUniNetMsg
	virtual BOOL GenMsg(PTUniNetMsg msg, CHAR* &buf);

	//
	// decode from xml msg tree to C++ TUniNetMsg
	//
	// decode ctrlmsg and msgbody
	virtual BOOL GenMsg(PTUniNetMsg msg, TiXmlHandle& xmlParser);
	// fill some fields of the msg according to the policy of the session context.
	virtual BOOL PolicyMsg(PCTUniNetMsg rmsg, PTUniNetMsg msg, UINT policy);
protected:
	BOOL GenSipMsg(PTUniNetMsg msg, CHAR* &buf);
};

#endif /* SIPMSGLIB_H_ */
