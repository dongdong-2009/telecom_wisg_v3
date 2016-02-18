#ifndef CBEARMODULE_H
#define CBEARMODULE_H
#include "uacstask.h"
#include "msgdef_uninet.h"
#include "psa_sip_inf.h"
#include "int_msg_def.h"
#include "CBearModule_sm.h"
#include "CPropertiesManager.h"
#include "CTUniNetMsgHelper.h"
#include "CMessageXmlParse.h"
_CLASSDEF(CBearModule)
_DECLARE_CREATE_COMP(CBearModule)
;

//注意下面仅仅是定时器ID，不是超时时间 在timer.cfg配置其具体信息:名字,时延,重发次数等
const int BEAR_200OK_TIMEOUT = 40;

class CBearModule: public CUACSTask {
public:
	CBearModule(PCGFSM afsm);
	~CBearModule();

	void sendInviteToMS(TUniNetMsg * msg);
	void sendAckToMS(TUniNetMsg * msg);
	void sendJoinToMS(TUniNetMsg * msg);
	void sendCancelToMS();
	void sendByeToMS();

	void sendErrorToCall(const int errorType);
	void sendSdpAnswerToCall(TUniNetMsg * msg);
	void sendCloseToCall();

	bool isResp1xx(TUniNetMsg * msg);
	bool isResp3xx_6xx(TUniNetMsg * msg);
	bool isResp2xx(TUniNetMsg * msg);

	bool isWithSDP(TUniNetMsg * msg);

	void setConnId(TUniNetMsg * msg);

	string checkRespCSeqMethod(TUniNetMsg * msg);


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
	CBearModuleContext m_fsmContext;

	PTSipCtrlMsg m_MSSipCtrlMsg;
	string m_MSConnId;
	PTIntCtrlMsg m_IntCtrlMsg;

	string MS_NAME;
	string MS_IP;
	string MS_PORT;
	string HOST_NAME;
	string HOST_IP;
	string HOST_PORT;

	CMessageXmlParse* m_parser;

	INT LOGADDR_DISPATCHER; // 231

	void sendMsgToDispatcher(TUniNetMsgName msgName, TUniNetMsgType msgType, TDialogType dialog,
			PTCtrlMsg pCtrlMsg, PTMsgBody pMsgBody);

};

#endif
