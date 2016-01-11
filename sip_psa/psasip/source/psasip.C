/************************************************************************

 * Copyright (c)2010-2012, by BUPT
 * All rights reserved.

 * FileName：       psasip.C
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
#include "psasip.h"
#include "exosipstack.h"
#include "sipenv.h"

static PCExosipStack s_sipStack;

CPsaSip::CPsaSip(INT psaid) : psaid(psaid)
{
}

CPsaSip::~CPsaSip()
{
	delete s_sipStack;
	s_sipStack = NULL;
}

BOOL CPsaSip::init(void)
{
	// 初始化具体协议栈控制对象
	s_sipStack = new CExosipStack(psaid);
	// 设置端口
	if (FALSE == s_sipStack->init(CSipEnv::instance()->SIP_PORT))
	{
		delete s_sipStack;
		s_sipStack = NULL;

		return FALSE;
	}

	return TRUE;
}

void CPsaSip::doActive(void)
{
	if (NULL != s_sipStack)
	{
		s_sipStack->doActive();
	}
}

BOOL CPsaSip::doSendMsg(PTMsg msg)
{
	if (NULL != s_sipStack)
	{
		return s_sipStack->doSendMsg(msg);
	}
	else
	{
		return FALSE;
	}
}
