#ifndef CSIPMESSAGEMODULE_H
#define CSIPMESSAGEMODULE_H
#include "uacstask.h"
#include "msgdef_uninet.h"
#include "psa_sip_inf.h"
#include "rtc_msg_def.h"
#include "CSipToRtc.h"
#include "CSipMessageModule_sm.h"
#include "CPropertiesManager.h"

const int SIPMESSAGE_200OK_TIMEOUT = 32;

_CLASSDEF(CSipMessageModule)
_DECLARE_CREATE_COMP(CSipMessageModule);
class CSipMessageModule: public CUACSTask {
public:
	CSipMessageModule(PCGFSM afsm);
	~CSipMessageModule();

	void sendToDispatcher(TUniNetMsg *pMsg);
	/*
	 * 结束状态机
	 * 需要向Dispatcher模块发送信息,通知这个处理状态机实例的结束
	 * 删除Dispatcher模块存储的会话状态信息
	 */
	void endTask();

	// 收到SIP发过来的Message消息直接回200ok，不用等RTC的ok
	void sendBack200OK(TUniNetMsg *msg);

	// 判断Message消息是否来自sip，是的话调用sendBack200OK函数
	bool isFromSip(TUniNetMsg *msg){
		return msg->msgType == RTC_TYPE;
	}

	bool isFromRtc(TUniNetMsg *msg){
		return msg->msgType == SIP_TYPE;
	}

	bool isResponse200OK(TUniNetMsg *msg);

	//处理消息
	virtual  void procMsg(PTUniNetMsg msg);
	//处理定时器超时
	virtual  void onTimeOut (TTimeMarkExt timerMark);
	DECLARE_CLONE();
protected:
	// method from CUACSTask
	virtual PTUACData createData();//需要子类实例化一个TUACData对象。
	virtual void initState(); //需要子类实例化一个状态类对象。通常是转到初始状态。
private:
	CSipMessageModuleContext m_fsmContext;
	INT LOGADDR_DISPATCHER;	// 231
	TSipCtrlMsg* m_SipCtrlMsg;

	BOOL msgMap(TUniNetMsg *pSrcMsg, TUniNetMsg *pDestMsg);
	// 判断Xlite发过来IM的是不是Text Html
	BOOL isTextHtml(TUniNetMsg *msg);
	// 判断Xlite发过来IM的是不是Text plain
	BOOL isTextPlain(TUniNetMsg *msg);
public:
	// 将接收到的来自XLIte的Html格式的短信转换成plain格式
	void convertToPlain(TUniNetMsg *msg);
};

#endif
