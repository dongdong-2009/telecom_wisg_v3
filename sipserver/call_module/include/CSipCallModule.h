#ifndef CSIPCALLMUDUAL_H
#define CSIPCALLMUDUAL_H
#include "uacstask.h"
#include "msgdef_uninet.h"
#include "psa_sip_inf.h"
#include "rtc_msg_def.h"
#include "CSipToRtc.h"
#include "CSipCallModule_sm.h"
#include "CPropertiesManager.h"
#include "CTUniNetMsgHelper.h"
#include "CMessageXmlParse.h"

_CLASSDEF(CSipCallModule)
_DECLARE_CREATE_COMP(CSipCallModule);

//注意下面仅仅是定时器ID，不是超时时间 在timer.cfg配置其具体信息:名字,时延,重发次数等
const int SIPCALL_200OK_TIMEOUT = 30;
const int SIPCALL_ACK_TIMEOUT = 31;


class CSipCallModule: public CUACSTask {
public:
	CSipCallModule(PCGFSM afsm);
	~CSipCallModule();



	void sendToDispatcher(TUniNetMsg *pMsg);
	/*
	 * 结束状态机
	 * 需要向Dispatcher模块发送信息,通知这个处理状态机实例的结束
	 * 删除Dispatcher模块存储的会话状态信息
	 */
	void endTask();

	// callproc状态时超时，根据呼叫方做不同的处理
	void handleTimeoutAtCallProcState();

	// 收到SIP发过来的Bye消息直接回200ok，不用等RTC的ok
	void sendBack200OK(TUniNetMsg *msg);

	//add 2 functions by zhangyadong on 2014.7.14
	void sendBackACK(TUniNetMsg *msg);
	void sendBackBYE();

	void sendByeToIMS();
	void sendByeToWeb();


	// 判断bye消息是否来自sip，是的话调用sendBack200OK函数
	bool isByeFromSip(TUniNetMsg *msg){
		return msg->msgType == RTC_TYPE;
	}
	bool isByeFromRtc(TUniNetMsg *msg){
		return msg->msgType == SIP_TYPE;
	}
	
	bool isSipCaller(){

		return m_UACsipCtrlMsg->via.branch.size() >= 1;
	}


	void forwardNoSdpInviteToIMS();
	void forwardCancelToIMS();
	void forwardACKToIMS(TUniNetMsg *msg);

	void forwardPrackToIMS(TUniNetMsg *msg);
	void forward200OKToIMS(TUniNetMsg *msg);

	void forwardByeToIMS();


	void forward200OKToWeb(TUniNetMsg *msg);
	void forwardByeToWeb();


	void forwardACKToMS_IMS();
	void forwardInviteToMS_IMS();
	void forwardReinviteToMS_IMS(TUniNetMsg * msg);

	void forwardInviteToMS_Web();
	void forwardACKToMS_Web();



	void forwardJoinInfoToMS();
	void forwardByeToMS_Web();
	void forwardByeToMS_IMS();

	void forwardCancelToMS_Web();
	void forwardCancelToMS_IMS();




	void setUACTag(TUniNetMsg * msg);
	void setConnId_Web(TUniNetMsg *msg);
	void setConnId_IMS(TUniNetMsg *msg);
	void setIMSbody(TUniNetMsg *msg);


	bool isConfReINVITE(TUniNetMsg * msg);

	bool checkColorRing();

	//判断是否是180或183携带彩铃的sdp --add by zhangyadong 2015-11-29
	bool isRespWithSdp(TUniNetMsg * msg);
	//消息是否是100/101响应消息
	static bool isResp100_101(TUniNetMsg* msg);
	//消息是否是1xx响应消息
	static bool isResp1xx(TUniNetMsg* msg);
	//消息是否是200 OK响应消息
	static bool isResp2xx(TUniNetMsg* msg);
	//消息是否是3xx~6xx响应
	static bool isResp3xx_6xx(TUniNetMsg* msg);

	//处理消息
	virtual  void procMsg(PTUniNetMsg msg);
	//处理定时器超时
	virtual  void onTimeOut (TTimeMarkExt timerMark);
	DECLARE_CLONE();
protected:
	//需要子类实例化一个TUACData对象。
	virtual PTUACData createData();
	//需要子类实例化一个状态类对象。通常是转到初始状态。
	virtual void initState();
private:
	string replaceSdpStr(string& s1, string& s2, string& s3);
	
	CSipCallModuleContext m_fsmContext;

	INT LOGADDR_DISPATCHER;	// 231
	//TSipCtrlMsg* m_SipCtrlMsg;
	CVarChar128 m_caller;

	CVarChar128 m_callerHost;//add by guoxun

	bool isColorRingStatus; //彩铃状态


	PTSipCtrlMsg 			m_UACsipCtrlMsg;
	PTSipCtrlMsg 			m_UASsipCtrlMsg;
	PTSipCtrlMsg 			m_MSsipCtrlMsg_1;
	PTSipCtrlMsg 			m_MSsipCtrlMsg_2;


	PTSipInvite 			m_sipInviteBody;

	PTSipBody				m_webrtcBody;
	PTSipBody				m_imsBody;
	PTSipBody				m_xmsImsBody;

	string					m_connId_Web;
	string 					m_connId_IMS;

	string					MS_NAME;
	string					MS_IP;
	string 					MS_PORT;
	string					HOST_NAME;
	string					HOST_IP;
	string 					HOST_PORT;

	CMessageXmlParse* 		m_parser;



	bool m_isReCall;

	BOOL msgMap(TUniNetMsg *pSrcMsg, TUniNetMsg *pDestMsg);
	// 当callproc状态超时，产生timeout消息发给xlite,作用于xlite呼叫，webrtc不接听
	void generateAndSendTimeOutMsg();
	// 当callproc状态超时，产生cancel消息发给xlite,作用于webrtc呼叫，xlite不接听
	void generateCancel();

	void sendMessageToSip(TUniNetMsgName msgName, TDialogType dialog, PTCtrlMsg pCtrlMsg, PTMsgBody pMsgBody);

	// 如果XLite发起呼叫，webrtc回复200ok时没有via字段，这样就不能发到xlite上去，所以需要记录和填充
	TSipVia m_sipvia;
public:
	void recordVia(PTUniNetMsg msg);
	void attachRecordedViaToMsg(PTUniNetMsg msg);

	// just ignore re-invite
	void sendBack488NotAcceptableHere(PTUniNetMsg msg);
};
#endif
