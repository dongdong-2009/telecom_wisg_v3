#ifndef _RTC_TO_SIP_H
#define _RTC_TO_SIP_H
#include "msgdef_uninet.h"
#include "rtc_msg_def.h"
#include "psa_sip_inf.h"
#include "CMsgMapHelper.h"
#include "constdef.h"
#include "CPropertiesManager.h"
#include "CUserMapHelper.h"
#include <map>

class CRtcCallModule;
class CRtcToSip{
private:
	CRtcToSip();
	static CRtcToSip* _instance;


	BOOL msgMapToSipError(TUniNetMsg *pSrcMsg, TUniNetMsg *pDestMsg, bool isReCall);
	BOOL msgMapToSipCtrl(TUniNetMsg *pSrcMsg, TUniNetMsg *pDestMsg,  CRtcCallModule* pRtcCall, UINT cSeq_number, CVarChar16 cSeq_method);
//	TSipURI generateSipUrl(CVarChar64 user);
	void mapToSipInvite(PTRtcCtrlMsg pRtcCtrl, TUniNetMsg* pDestMsg,
			TUniNetMsg* pSrcMsg);
	void mapToSipResponse(TUniNetMsg* pDestMsg, TUniNetMsg* pSrcMsg, bool isReCall);
	void mapToSipAck(TUniNetMsg* pDestMsg, TUniNetMsg* pSrcMsg);
	void mapToSipBye(PTRtcCtrlMsg pRtcCtrl, TUniNetMsg* pDestMsg,
			TUniNetMsg* pSrcMsg, CVarChar128 caller, bool isReCall);
	void mapToSipCancel(PTRtcCtrlMsg pRtcCtrl, TUniNetMsg* pDestMsg,
				TUniNetMsg* pSrcMsg);
	void mapToSipMessage(TUniNetMsg* pSrcMsg, TUniNetMsg* pDestMsg,
			PTRtcCtrlMsg pRtcCtrl);

	// 在sip中以@符号区分用户名和域名，webrtc是否也应该这样？
	string getUserName(const CVarChar128& user);
	string getHost(const CVarChar128& user);

private:
	int accessMode;

public:
	static CRtcToSip* instance();
	BOOL msgMap(TUniNetMsg *pSrcMsg, TUniNetMsg *pDestMsg, CVarChar128 caller, bool isReCall);
};

#endif
