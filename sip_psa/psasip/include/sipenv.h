/************************************************************************

 * Copyright (c)2010-2012, by BUPT
 * All rights reserved.

 * FileName：      sip_env.h
 * System：         MCF
 * SubSystem：      PSA
 * Author：         Huang Haiping
 * Date：           2010.04.20
 * Version：        1.0
 * Description：
        SIP协议栈环境配置，比如指定端口号等。从$UNIDIR/etc/sip.env中读取
    配置参数。如果没有则使用默认值

 *
 * Last Modified:
	  2010.04.20, 完成初始版本定义
		 By Huang Haiping


*************************************************************************/

#ifndef SIP_ENV_H_
#define SIP_ENV_H_

#include "comtypedef.h"

_CLASSDEF(CSipEnv)
class CSipEnv
{
public:
	USHORT SIP_PORT; // sipPort
	UINT DIS_LOG_ADDR; // disLogAddr
	static PCSipEnv instance();
private:
	CSipEnv();
	static PCSipEnv s_instance;
	void init();
	BOOL isInit;
};


#endif /* SIP_ENV_H_ */
