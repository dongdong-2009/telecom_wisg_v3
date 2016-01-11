/************************************************************************

 * Copyright (c)2010-2012, by BUPT
 * All rights reserved.

 * FileName：       hooksipimpl.C
 * System：         MCF
 * SubSystem：      PSA
 * Author：         Huang Haiping
 * Date：           2010.04.07
 * Version：        1.0
 * Description：
        SIP PSA(Protocol Stack Adaptor) 协议适配器。用于接入SIP协议栈
		初始化钩子函数和PSA
 *
 * Last Modified:
	  2010.04.07, 完成初始版本定义
		 By Huang Haiping


*************************************************************************/
#include "info.h"
#include "pachook.h"
#include "pacm.h"
#include "dyncomploader.h"
#include "psasip.h"

static PCPsaSip s_psasip = NULL;

//initPsaTest的声明必须在宏INIT_PSA_COMP之前。
void initPsaSip(int);
//这个宏是必须的。compLoader需要利用它来加载组件。
INIT_PSA_COMP(PsaSip)

void hookActivePsaSipImpl()
{
	 if (NULL != s_psasip)
	 {
		 s_psasip->doActive();
	 }
}

BOOL hookSendMsgPsaSipImpl(PTMsg msg)
{
	if (NULL != s_psasip)
	{
		return s_psasip->doSendMsg(msg);
	}
	else
	{
		return FALSE;
	}

}


void initPsaSip(int psaid)
{
	//设置挂钩函数
	setHookActive(psaid, hookActivePsaSipImpl);
//	DEBUG2(SIP, psaid,"set hookActive succeed.\n");

	setHookSendMsg(psaid, hookSendMsgPsaSipImpl);
	//psaPrint(psaid, "set hookSendMsg succeed.\n");
//	DEBUG2(SIP, psaid,"set hookSendMsg succeed.\n");

    //初始化PSA
	s_psasip = new CPsaSip(psaid);
	if (s_psasip->init())
	{
		UniINFO("init psa sip succeed. psaid: %d\n", psaid);
	}
	else
	{
		UniERROR("init psa sip succeed. psaid: %d\n", psaid);
		delete s_psasip;
		s_psasip = NULL;
	}

}
