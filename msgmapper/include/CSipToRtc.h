#ifndef CSIP_TO_RTC_H
#define CSIP_TO_RTC_H
#include "msgdef_uninet.h"
#include "rtc_msg_def.h"
#include "psa_sip_inf.h"
#include "CMsgMapHelper.h"
#include "constdef.h"
#include "CPropertiesManager.h"
#include "CUserMapHelper.h"
class CSipToRtc{
public :
	static CSipToRtc* instance();
	BOOL msgMap(TUniNetMsg *pSrcMsg, TUniNetMsg *pDestMsg,
			CVarChar128 caller,CVarChar128 callerHost, bool isReCall);//change by guoxun

private:
	int m_accessMode;

private:
	CSipToRtc();
	void mapToRtcOffer(TUniNetMsg* pSrcMsg, TUniNetMsg* pDestMsg,
			TRtcCtrlMsg* pRtcCtrl, TSipCtrlMsg* pSipCtrl, bool isReCall);
	void mapToRtcOk(TUniNetMsg* pSrcMsg, TUniNetMsg* pDestMsg, TRtcCtrlMsg* pRtcCtrl,
			TSipCtrlMsg* pSipCtrl, bool isReCall);
	void maoToRtcShutdown(TUniNetMsg* pDestMsg, TRtcCtrlMsg* pRtcCtrl,
			TSipCtrlMsg* pSipCtrl, const CVarChar128& caller,
			const CVarChar128& callerHost, bool isReCall);//change by guoxun
	void mapToRtcAnswerOrError(TUniNetMsg* pSrcMsg, TUniNetMsg* pDestMsg,
			TSipCtrlMsg* pSipCtrl, TRtcCtrlMsg* pRtcCtrl, bool isReCall);
	void mapToRtcMessage(TUniNetMsg* pSrcMsg, TUniNetMsg* pDestMsg,
			TRtcCtrlMsg* pRtcCtrl, TSipCtrlMsg* pSipCtrl);
	void mapToRtcInfoOrUpdate(TUniNetMsg* pSrcMsg,TUniNetMsg* pDestMsg, TRtcCtrlMsg* pRtcCtrl,
			TSipCtrlMsg* pSipCtrl, const CVarChar128& caller,
			const CVarChar128& callerHost, bool isReCall);

	static CSipToRtc* _instance;

	CVarChar128 combineUserNameAndHost(const TSipURI& url){
		string tmp(url.username.c_str());
		if(url.host.length() != 0)
			tmp = tmp + "@" + url.host.c_str();
		CVarChar128 ret;
		ret = tmp.c_str();
		return ret;
	}

};

#endif
