/************************************************************************

 * Copyright (c)2010-2012, by BUPT
 * All rights reserved.

 * FileName：       msghelper_sip.h
 * System：         MCF
 * SubSystem：      PSA
 * Author：         Huang Haiping
 * Date：           2010.04.27
 * Version：        1.0
 * Description：
       中间消息辅助类，负责将SIP中间消息转换成标准字符串格式

 *
 * Last Modified:
	  2010.04.27, 完成初始版本定义
		 By Huang Haiping
	  2010.10.25, 将中间消息的toString方法实现为完全协议栈无关
		 By Huang Haiping


*************************************************************************/

#ifndef MSGHELPER_SIP_H_
#define MSGHELPER_SIP_H_

#include "psa_sip_inf.h"

_CLASSDEF(CSipMsgHelper);
class CSipMsgHelper
{
public:
	/* 生成32位随机数，用于from tag和call id */
	static UINT generateRandomNumberUINT();
	static void generateRandomNumberStr(CHAR buf[33]);

	static TSipURI createSipURI(const CHAR* scheme, const CHAR* userName,
			const CHAR* host, const CHAR* port);
	static TSipContentType createSipContentType(const CHAR* type, const CHAR* subtype);

	static TSipBody createSipBody(CVarChar content);

	static CVarChar toString(RCTSipSubscriptionState ss);
	static CVarChar toString(RCTSipAddress sipAddress);
	static CVarChar toString(RCTSipURI sipUri);
	static CVarChar toString(RCTSipCallId sipCallId);
	static CVarChar toString(RCTSipContentType sipContentType);
	static CVarChar toStringRoute(RCTSipAddress sipRoute);
};

#endif
