#ifndef MSG_DISPATCHER_H
#define MSG_DISPATCHER_H
#include "uactask.h"
#include "CPropertiesManager.h"
#include "psa_sip_inf.h"
#include "rtc_msg_def.h"
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
	void handleMsgFromSipCall(TUniNetMsg* msg);
	//void handleMsgFromSipIM(TUniNetMsg* msg);

	void handleMsgFromMSControl(TUniNetMsg * msg);

	void handleMsgFromRtcPSA(TUniNetMsg* msg);
	void handleMsgFromRtcCall(TUniNetMsg* msg);
	//void handleMsgFromRtcIM(TUniNetMsg* msg);


	void sendMsgtoInstance(TUniNetMsg* msg, TMsgAddress instAddr, TDialogType dialogtype);
	char* generateSipUniqID(TUniNetMsg* msg);
	char* generateRtcUniqID(TUniNetMsg* msg);

	INT LOGADDR_SIP_PSA;	// default 11
	INT LOGADDR_RTC_PSA;    // default 18
	INT LOGADDR_SIP_CALL;	// default 232
	INT LOGADDR_RTC_CALL;   // default 233
	INT LOGADDR_SIP_IM;		// default 234
	INT LOGADDR_RTC_IM;    	// default 235
	INT LOGADDR_MS_CONTROL;	// default 236

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
