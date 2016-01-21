#ifndef MSG_DISPATCHER_H
#define MSG_DISPATCHER_H
#include "uactask.h"
#include "CPropertiesManager.h"
#include "psa_sip_inf.h"
#include "rtc_msg_def.h"
#include "int_msg_def.h"
#include "utils_inf.h"
#include "CDialogController.h"
#include <stdio.h>
_CLASSDEF(CMsgDispatcher)
_DECLARE_CREATE_COMP(CMsgDispatcher);

class CMsgDispatcher :public CUACTask{
public:
	CMsgDispatcher(PCGFSM afsm);
	virtual ~CMsgDispatcher();
	// from CUACTask
	void procMsg(TUniNetMsg* msg);
	void onTimeOut ( TTimerKey timerKey,  TTimeMarkExt timerMark,  void* para);
	void onTimeUpdate( TTimeMarkExt timerMark,  TTimerKey oldTimerKey, TTimerKey  newTimerKey, void* para);

	DECLARE_CLONE();
private:
	void handleMsgFromSipPSA(TUniNetMsg* msg);

	void handleMsgFromRtcPSA(TUniNetMsg* msg);

	void handleMsgFromRtcSipCall(TUniNetMsg* msg);
	void handleMsgFromBear(TUniNetMsg * msg);


	void sendMsgtoInstance(TUniNetMsg* msg, TMsgAddress instAddr, TDialogType dialogtype);
	inline char* generateSipUniqID(TUniNetMsg* msg)
	{
		TSipCtrlMsg* pSipCtrl = (TSipCtrlMsg*) (msg->ctrlMsgHdr);
		return pSipCtrl->sip_callId.number.c_str();
	}
	inline char* generateRtcUniqID(TUniNetMsg* msg){
		PTRtcCtrlMsg pCtrl = (PTRtcCtrlMsg) msg->ctrlMsgHdr;
		return pCtrl->offerSessionId.c_str();
	}

	inline char* generateIntUniqID(TUniNetMsg* msg){
		PTIntCtrlMsg pCtrl = (PTIntCtrlMsg) msg->ctrlMsgHdr;
		return pCtrl->sessionId.c_str();
	}

	INT LOGADDR_SIP_PSA;	// default 11
	INT LOGADDR_RTC_PSA;    // default 18
	INT LOGADDR_RTC_SIP_CALL;	// default 232
	INT LOGADDR_BEAR_MOD;   // default 233

	INT LOGADDR_DISPATCHER;	// default 231


	TMsgAddress m_rtcPsaAddr;
	BOOL m_isrtcPsaAddrSet;
	TMsgAddress m_sipPsaAddr;
	BOOL m_issipPsaAddrSet;

	TMsgAddress getRtcPsaAddr();
	TMsgAddress getSipPsaAddr();

	CDialogController* m_pDialogCtrl;
private:
	void justPassRegister(TUniNetMsg* msg);
};

#endif
