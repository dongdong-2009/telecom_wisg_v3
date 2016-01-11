#ifndef CMSCONTROLMODULE_H
#define CMSCONTROLMODULE_H
#include "uacstask.h"
#include "msgdef_uninet.h"
#include "psa_sip_inf.h"
#include "rtc_msg_def.h"
#include "CSipToRtc.h"
#include "CSipCallModule_sm.h"
#include "CPropertiesManager.h"
#include "CTUniNetMsgHelper.h"
#include "CMessageXmlParse.h"
_CLASSDEF(CMSControlModule)
_DECLARE_CREATE_COMP( CMSControlModule)
;

//注意下面仅仅是定时器ID，不是超时时间 在timer.cfg配置其具体信息:名字,时延,重发次数等
const int SIPCALL_200OK_TIMEOUT = 30;
const int SIPCALL_ACK_TIMEOUT = 31;

enum CallType {
	NOCALL, WEBCALL, IMSCALL
};

class CMSControlModule: public CUACSTask {
public:
	CMSControlModule(PCGFSM afsm);
	~CMSControlModule();

	void sendInviteToMS(TUniNetMsg * msg);
	void sendAckToMS(TUniNetMsg * msg);
	void sendInfoToMS();

	void sendRespToSipCall(TUniNetMsg * msg);
	void sendByeToSipCall();
	void sendByeToMS();

	bool isResp1xx(TUniNetMsg * msg);
	bool isResp3xx_6xx(TUniNetMsg * msg);
	bool isResp2xx(TUniNetMsg * msg);

	bool isJoinFinished();
	bool isBothCallFinished();
	bool isFromMS(TUniNetMsg * msg);

	// method from CUACSTask
	//处理消息
	virtual void procMsg(PTUniNetMsg msg);
	//处理定时器超时
	virtual void onTimeOut(TTimeMarkExt timerMark);
	void list() {
	}
	DECLARE_CLONE();

	/*结束状态机
	 * */
	void endTask();

protected:
	virtual PTUACData createData();

	virtual void initState();

private:
	CMSControlModuleContext m_fsmContext;
	CallType m_callType;
	bool m_joinFinished;
	UINT m_bothCallFinished;

	PTSipCtrlMsg m_MSWebSipCtrlMsg;
	PTSipCtrlMsg m_MSIMSSipCtrlMsg;

	string m_WebConnId;
	string m_IMSConnId;

	PTSipCtrlMsg m_SipModCtrlMsg;

	string MS_NAME;
	string MS_IP;
	string MS_PORT;
	string HOST_NAME;
	string HOST_IP;
	string HOST_PORT;

	CMessageXmlParse* m_parser;

	INT LOGADDR_DISPATCHER; // 231

	void sendMessageToSip(TUniNetMsgName msgName, TDialogType dialog,
			PTCtrlMsg pCtrlMsg, PTMsgBody pMsgBody);

};

#endif
