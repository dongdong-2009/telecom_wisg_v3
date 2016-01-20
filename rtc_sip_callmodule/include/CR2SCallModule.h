#ifndef CRTC_CALL_MODULE_H
#define CRTC_CALL_MODULE_H


#include <algorithm>
#include <map>
#include "db.h"
#include "uacstask.h"
#include "rtc_msg_def.h"
#include "int_msg_def.h"
#include "CPropertiesManager.h"
#include "CRtcToSip.h"
#include "CRtcOrigCall_sm.h"
#include "CSipTermCall_sm.h"
#include "CMsgDispatcher.h"
#include "timerpoll.h"

typedef struct
{
	UINT timer_id;
	void * currMod;
} TimerType;

_CLASSDEF(CR2SCallModule)
_DECLARE_CREATE_COMP(CR2SCallModule);

//注意下面仅仅是定时器ID，不是超时时间 在timer.cfg配置其具体信息:名字,时延,重发次数等
const int RTC_CONNECTION_TIMEOUT = 20;
const int RTC_SHUTDOWN_TIMEOUT = 21;
const int RTC_WAITSIP_TIMEOUT = 22;
const int RTC_WAITBEAR_TIMEOUT = 23;

const int SIP_200OK_TIMEOUT = 30;
const int SIP_ACK_TIMEOUT = 31;

const int SIP_CONNECTING_TIMEOUT = 32;
const int SIP_RING_TIMEOUT = 33;
const int SIP_ACTIVE_TIMEOUT = 34;
const int SIP_WAITBEAR_TIMEOUT = 35;



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
	void endTask_Rtc();
	void endTask_Sip();

	// method from CUACSTask
	//处理消息
	virtual  void procMsg(PTUniNetMsg msg);
	//mcf提供的定时器，没用到
	virtual void onTimeOut(TTimeMarkExt timerMark);

	//处理定时器超时,自定义的定时器
	void timeOut (timer * ptimer);

	void setTimer (UINT timer_id);

	void list(){}
	DECLARE_CLONE();
protected:
	// method from CUACSTask
	virtual PTUACData createData();//需要子类实例化一个TUACData对象。
	virtual void initState(); //需要子类实例化一个状态类对象。通常是转到初始状态。
private:
	CRtcOrigCallContext m_rtcContext;
	CSipTermCallContext m_sipContext;

	INT LOGADDR_DISPATCHER;	// 231

	TMsgAddress m_dispatcherAddr;
	BOOL m_isDispatcherAddrSet;

	string m_webSdp;
	string m_imsSdp;

	string m_sipName;


	TIntCtrlMsg* m_intCtrlMsg_Rtc;
	TIntCtrlMsg* m_intCtrlMsg_Sip;
	TRtcCtrlMsg* m_rtcCtrlMsg;

	CVarChar64 m_offerSessionId;

	TSipCtrlMsg * m_sipCtrlMsg;

	UINT m_seq;
	int m_accessMode;

	bool m_isSdpConfirmed;

	UINT m_sdpModifyFlag;

	bool m_isSipInit;
	bool m_isRtcInit;
	bool m_switchFlag;

	bool selectSipUser(string rtcname);
public:
	// map rtc message to sip message
	BOOL msgMap(TUniNetMsg *pSrcMsg, TUniNetMsg *pDestMsg);

	void handleUnexpectedMsg(TUniNetMsg * msg);

	bool checkSipUserAvailable(TUniNetMsg *msg);

	void notifySipTermCallSdp(TUniNetMsg *msg);
	void notifySipTermCallClose();

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

	bool compAndModifySdpWithRtc(TUniNetMsg * msg);
	bool compSdpWithOld(TUniNetMsg * msg);
	string checkRespCseqMothod(TUniNetMsg * msg);

	const char * getUserName(const string& user);
	const char * getHost(const string& user);

	inline void setUACTag(TUniNetMsg * msg){
		m_sipCtrlMsg->to.tag = ((PTSipCtrlMsg) msg->ctrlMsgHdr)->to.tag;
	}

	inline void resetFlag(){
		m_sdpModifyFlag = 0;
	}

	inline bool isInviteFlag(){
		return m_sdpModifyFlag == 1;
	}

	inline bool isUpdateFlag(){
		return m_sdpModifyFlag == 2;
	}

	inline void setUpdateFlag(){
		m_sdpModifyFlag = 2;
	}

	inline void setInviteFlag(){
		m_sdpModifyFlag = 1;
	}

	inline bool isFromRtc(TUniNetMsg * msg){
		return msg == NULL;
	}

	inline bool isFromSip(TUniNetMsg * msg){
		return msg == NULL;
	}

	inline bool isSwitch(){
		return m_switchFlag;
	}

	inline void resetSwitchFlag(){
		m_switchFlag = false;
	}

};
#endif
