#ifndef CRTCMESSAGEMODULE_H
#define CRTCMESSAGEMODULE_H
#include "uacstask.h"
#include "rtc_msg_def.h"
#include "CPropertiesManager.h"
#include "CRtcToSip.h"
#include "CRtcMessageModule_sm.h"
#include "CMsgDispatcher.h"

_CLASSDEF(CRtcMessageModule)
_DECLARE_CREATE_COMP(CRtcMessageModule);

class CRtcMessageModule: public CUACSTask {
public:
	CRtcMessageModule(PCGFSM afsm);
	virtual ~CRtcMessageModule();
	void sendToDispatcher(TUniNetMsg *pMsg);
	/*
	 * 结束状态机
	 * 需要向Dispatcher模块发送信息,通知这个处理状态机实例的结束
	 * 删除Dispatcher模块存储的会话状态信息
	 */
	void endTask();

	// method from CUACSTask
	//处理消息
	virtual  void procMsg(PTUniNetMsg msg);
	//处理定时器超时
	virtual  void onTimeOut (TTimeMarkExt timerMark);
	void list(){}
	DECLARE_CLONE();
protected:
	// method from CUACSTask
	virtual PTUACData createData();//需要子类实例化一个TUACData对象。
	virtual void initState(); //需要子类实例化一个状态类对象。通常是转到初始状态。
private:
	CRtcMessageModuleContext m_fsmContext;
	INT LOGADDR_DISPATCHER;	// 231

	TMsgAddress m_dispatcherAddr;
	BOOL m_isDispatcherAddrSet;

	TRtcCtrlMsg* m_RtcCtrlMsg;

	// map rtc message to sip message
	BOOL msgMap(TUniNetMsg *pSrcMsg, TUniNetMsg *pDestMsg);
};

#endif
