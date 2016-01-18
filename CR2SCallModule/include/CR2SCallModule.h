#ifndef CRTC_CALL_MODULE_H
#define CRTC_CALL_MODULE_H


#include <algorithm>
#include <map>

#include "uacstask.h"
#include "rtc_msg_def.h"
#include "int_msg_def.h"
#include "CPropertiesManager.h"
#include "CRtcToSip.h"
#include "CR2SCallModule_sm.h"
#include "CMsgDispatcher.h"

#include "db.h"


_CLASSDEF(CR2SCallModule)
_DECLARE_CREATE_COMP(CR2SCallModule);

//注意下面仅仅是定时器ID，不是超时时间 在timer.cfg配置其具体信息:名字,时延,重发次数等
const int RTC_CONNECTION_TIMEOUT = 20;
const int RTC_SHUTDOWN_TIMEOUT = 21;
const int RTC_WAITSIP_TIMEOUT = 22;
const int RTC_WAITBEAR_TIMEOUT = 23;



class CR2SCallModule: public CUACSTask {
public:
	CR2SCallModule(PCGFSM afsm);
	virtual ~CR2SCallModule();

	void sendToDispatcher(TUniNetMsgName msgName,
			TUniNetMsgType msgType, TDialogType dialog, PTCtrlMsg pCtrlMsg,
			PTMsgBody pMsgBody);

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
	CR2SCallModuleContext m_fsmContext;
	INT LOGADDR_DISPATCHER;	// 231

	TMsgAddress m_dispatcherAddr;
	BOOL m_isDispatcherAddrSet;

	string m_webSdp;

	string m_sipName;

	TRtcCtrlMsg* m_rtcCtrlMsg;

	TIntCtrlMsg* m_intCtrlMsg_Rtc;

	CVarChar64 m_offerSessionId;

	TSipCtrlMsg * m_sipCtrlMsg;

	UINT m_seq;
	int m_accessMode;

	bool m_isSdpConfirmed;

	bool selectSipUser(string rtcname);
public:
	// map rtc message to sip message
	BOOL msgMap(TUniNetMsg *pSrcMsg, TUniNetMsg *pDestMsg);

//	// 收到rtc发过来的Bye消息直接回ok，不用等sip的200 ok
//	void sendBackOK(TUniNetMsg *msg);
//	void sendBackError(UINT errorType);
//	// 判断bye消息是否来自rtc，是的话调用sendBackOK函数
//	bool isByeFromRtc(TUniNetMsg *msg){
//		return msg->msgType == SIP_TYPE;
//	}
//
//	bool isByeFromSip(TUniNetMsg *msg){
//		return msg->msgType == RTC_TYPE;
//	}

	bool checkSipUserAvailable(TUniNetMsg *msg);

	void notifySipTermCallSdp(TUniNetMsg *msg);
	void notifySipTermClose();

	void sendAnswerToWeb(TUniNetMsg *msg);
	void sendErrorToWeb(const int errorType);
	void forwardErrorToWeb(TUniNetMsg * msg);
	void sendShutdownToWeb();
	void sendNotifyToWeb(TUniNetMsg *msg);
	bool isSdpConfirmed();
	void sendReqToBear_Rtc();
	void sendCloseToBear_Rtc();


	void sendNoSdpInviteToIMS();
	void sendCancelToIMS();
	void sendPrackToIMS(TUniNetMsg * msg);
	void sendAckToIMS(TUniNetMsg * msg);
	void send200OKForUpdateToIMS(TUniNetMsg * msg);
	void send200OKForInviteToIMS(TUniNetMsg * msg);
	void sendByeToIMS();


	void notifyRtcOrigCallSdp();
	void notifyRtcOrigCallError(TUniNetMsg * msg);
	void notifyRtcOrigCallClose();
	void notifyRtcOrigCallError(const int errorType);


	void sendReqToBear_Sip();
	void sendCloseToBear_Sip();

	bool isWithSDP(TUniNetMsg * msg);
	bool isResp1xx(TUniNetMsg * msg);
	bool isResp3xx_6xx(TUniNetMsg * msg);
	bool isResp2xx(TUniNetMsg * msg);

	bool setUpdateFlag();
	bool setInviteFlag();

	bool compAndModifySdpWithRtc(TUniNetMsg * msg);

	string getUserName(const string& user);
	string getHost(const string& user);



};
#endif
