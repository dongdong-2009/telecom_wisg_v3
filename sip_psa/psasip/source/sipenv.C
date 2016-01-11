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
    配置参数.如果没有则使用默认值

 *
 * Last Modified:
	  2010.04.20, 完成初始版本定义
		 By Huang Haiping


*************************************************************************/
#include "sipenv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "info.h"
#include "func.h"

PCSipEnv CSipEnv::s_instance = NULL;

CSipEnv::CSipEnv()
{
	isInit = FALSE;
	SIP_PORT = 5060;
	DIS_LOG_ADDR = 205;

}

PCSipEnv CSipEnv::instance()
{
	if (NULL == s_instance)
	{
		s_instance = new CSipEnv();
		s_instance->init();
	}

	return s_instance;
}

void CSipEnv::init()
{
	if (FALSE == isInit)
	{
		 // set sip.env path
		 char* uniDir = getenv("UNIDIR");
		 char envpath[8192] = {0};
		 if (uniDir == NULL)
		 {
		    uniDir = ".";
		 }
		 strcat(envpath, uniDir);
		 strcat(envpath, "/etc/sip.env");
		 FILE* fpSipEnv = fopen(envpath, "r");
		 if (NULL == fpSipEnv)
		 {
			 // File not exists use defaults
			 UniINFO("sip.env not exists. Use default values");
		 }
		 else {
			 UniINFO("Reading parameters from sip.env.");

			 char tmp[256];

			 USHORT uSipPort = 0;
			 while (fscanf(fpSipEnv, "%s", tmp) != EOF) {
				 if ('#' == tmp[0]) continue;

				 if (0 == strncasecmp(tmp, "sipPort"))
				 {
					 fscanf(fpSipEnv, "%hu", &uSipPort);
					 if (0 != uSipPort)
					 {
						 this->SIP_PORT = uSipPort;
					 }
				 }
				 else if (0 == strncasecmp(tmp, "disLogAddr"))
				 {
					 fscanf(fpSipEnv, "%u", &this->DIS_LOG_ADDR);
				 }
			 }

			 fclose(fpSipEnv);
		 } // if file exists

		 isInit = TRUE;
	}  // if FALSE == isInit

}



