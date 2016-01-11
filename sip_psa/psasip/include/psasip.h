/************************************************************************

 * Copyright (c)2010-2012, by BUPT
 * All rights reserved.

 * FileName：       psasip.h
 * System：         MCF
 * SubSystem：      PSA
 * Author：         Huang Haiping
 * Date：           2010.04.07
 * Version：        1.0
 * Description：
        SIP PSA(Protocol Stack Adaptor) 协议适配器。用于接入SIP协议栈

 *
 * Last Modified:
	  2010.04.07, 完成初始版本定义
		 By Huang Haiping


*************************************************************************/
#ifndef _PSASIP_H
#define _PSASIP_H

#include "comtypedef.h"
#include "pachook.h"

_CLASSDEF(CPsaSip)
class CPsaSip {
public:
	CPsaSip(INT psaid);
	~CPsaSip();
	BOOL init();
	void doActive();
	BOOL doSendMsg(PTMsg msg);

private:
	INT psaid;
};

#endif
