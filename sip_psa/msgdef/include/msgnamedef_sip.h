/************************************************************************

 * Copyright (c)2010-2012, by BUPT
 * All rights reserved.

 * FileName：       msgnamedef_sip.h
 * System：         MCF
 * SubSystem：      MSGDEF
 * Author：         Huang Haiping
 * Date：           2010.04.09
 * Version：        1.0
 * Description：
		定义SIP消息的名字，每个消息体(msgBody)定义都应该对应一个整数类型的msgName

 *
 * Last Modified:
	  2010.04.09, 完成初始版本定义
		 By Huang Haiping


*************************************************************************/
#ifndef MSGNAMEDEF_SIP_H_
#define MSGNAMEDEF_SIP_H_

#include "comtypedef_uninet.h"

const TUniNetMsgName SIP_RESPONSE     = 1000;
const TUniNetMsgName SIP_REGISTER     = SIP_RESPONSE + 1; // 1001
const TUniNetMsgName SIP_INVITE       = SIP_REGISTER + 1; // 1002
const TUniNetMsgName SIP_MESSAGE      = SIP_INVITE + 1;   // 1003
const TUniNetMsgName SIP_ACK          = SIP_MESSAGE + 1;  // 1004
const TUniNetMsgName SIP_CANCEL       = SIP_ACK + 1;      // 1005
const TUniNetMsgName SIP_BYE          = SIP_CANCEL + 1;   // 1006
const TUniNetMsgName SIP_PUBLISH      = SIP_BYE + 1;      // 1007
const TUniNetMsgName SIP_SUBSCRIBE    = SIP_PUBLISH + 1;  // 1008
const TUniNetMsgName SIP_NOTIFY       = SIP_SUBSCRIBE + 1;// 1009
const TUniNetMsgName SIP_INFO		  = SIP_NOTIFY + 1;	  //1010
const TUniNetMsgName SIP_UPDATE		  = SIP_INFO + 1;	//1011
const TUniNetMsgName SIP_PRACK		  = SIP_UPDATE + 1; //1012

const TUniNetMsgName INTERNAL_INVITE  = 1020;
const TUniNetMsgName INTERNAL_RESPONSE = INTERNAL_INVITE + 1; //1021
const TUniNetMsgName INTERNAL_BYE = INTERNAL_RESPONSE + 1;





#endif /* MSGNAMEDEF_SIP_H_ */
