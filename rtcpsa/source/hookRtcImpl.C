#include "pachook.h"
#include "infoext.h"
#include "CPsaRtc.h"
#include "dyncomploader.h"
#include "MyLogger.h"

using namespace log4cxx::xml;
using namespace log4cxx;


static MyLogger& mLogger = MyLogger::getInstance("etc/log4cxx.xml", "SgFileAppender");

static PCPsaRtc s_pRtc = NULL;

void release(){
	LOG4CXX_INFO(mLogger.getLogger(), "rtcpsa exit call release\n");
	if(s_pRtc != NULL)
	{
		delete s_pRtc;
		s_pRtc = NULL;
	}
}

//initPsaTest的声明必须在宏INIT_PSA_COMP之前。
void initPsaRtc(int);
//这个宏是必须的。compLoader需要利用它来加载组件。
INIT_PSA_COMP(PsaRtc)
void hookActivePsaRtcImpl()
{
	 if (NULL != s_pRtc)
	 {
		 s_pRtc->doActive();
	 }
}

BOOL hookSendMsgPsaRtcImpl(PTMsg msg)
{
	if (NULL != s_pRtc)	{
		return s_pRtc->doSendMsg(msg);
	}else	{
		return FALSE;
	}

}

void initPsaRtc(int psaid){

	atexit(release);

	setHookActive(psaid, hookActivePsaRtcImpl);
	setHookSendMsg(psaid, hookSendMsgPsaRtcImpl);

	s_pRtc = new CPsaRtc(psaid);
	if(s_pRtc->init()){
		LOG4CXX_INFO(mLogger.getLogger(), "Rtc init success, psaid is "<<psaid);
	}else{
		LOG4CXX_ERROR(mLogger.getLogger(), "Rtc init failed");
		delete s_pRtc;
		s_pRtc = NULL;
	}

}
