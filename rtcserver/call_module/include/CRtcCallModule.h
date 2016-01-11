#ifndef CRTC_CALL_MODULE_H
#define CRTC_CALL_MODULE_H


#include <algorithm>
#include <map>

#include "uacstask.h"
#include "rtc_msg_def.h"
#include "CPropertiesManager.h"
#include "CRtcToSip.h"
#include "CRtcCallModule_sm.h"
#include "CMsgDispatcher.h"

#include "db.h"




_CLASSDEF(CRtcCallModule)
_DECLARE_CREATE_COMP(CRtcCallModule);

//注意下面仅仅是定时器ID，不是超时时间 在timer.cfg配置其具体信息:名字,时延,重发次数等
const int RTC_CONNECTION_TIMEOUT = 20;
const int RTC_SHUTDOWN_TIMEOUT = 21;


class CRtcCallModule: public CUACSTask {
public:
	CRtcCallModule(PCGFSM afsm);
	virtual ~CRtcCallModule();

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
	CRtcCallModuleContext m_fsmContext;
	INT LOGADDR_DISPATCHER;	// 231

	TMsgAddress m_dispatcherAddr;
	BOOL m_isDispatcherAddrSet;

	TRtcCtrlMsg* m_RtcCtrlMsg;
	CVarChar128 m_caller;
	CVarChar128 m_callee;
	CVarChar64 m_offerSessionId;
	BOOL m_isCaller;
	UINT m_seq;
	bool selectSipUser(string rtcname);

	int accessMode;

	bool m_isReCall;
public:
	// map rtc message to sip message
	BOOL msgMap(TUniNetMsg *pSrcMsg, TUniNetMsg *pDestMsg);

	// 收到rtc发过来的Bye消息直接回ok，不用等sip的200 ok
	void sendBackOK(TUniNetMsg *msg);
	void sendBackError(UINT errorType);
	// 判断bye消息是否来自rtc，是的话调用sendBackOK函数
	bool isByeFromRtc(TUniNetMsg *msg){
		return msg->msgType == SIP_TYPE;
	}

	bool isByeFromSip(TUniNetMsg *msg){
		return msg->msgType == RTC_TYPE;
	}

	bool checkSipUserAvailable(TUniNetMsg *msg);


};
#endif
