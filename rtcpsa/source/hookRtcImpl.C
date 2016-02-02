#include "pachook.h"
#include "infoext.h"
#include "CPsaRtc.h"
#include "dyncomploader.h"


#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/xml/domconfigurator.h>

using namespace log4cxx::xml;
using namespace log4cxx;




static PCPsaRtc s_pRtc = NULL;

void release(){
	printf("rtcpsa exit call release\n");
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
	log4cxx::xml::DOMConfigurator::configureAndWatch("etc/log4cxx.xml", 5000);
	log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger("SgFileAppender");

	setHookActive(psaid, hookActivePsaRtcImpl);
	setHookSendMsg(psaid, hookSendMsgPsaRtcImpl);

	s_pRtc = new CPsaRtc(psaid);
	if(s_pRtc->init()){
		LOG4CXX_INFO(logger, "Rtc init success, psaid is "<<psaid);
	}else{
		LOG4CXX_ERROR(logger, "Rtc init failed");
		delete s_pRtc;
		s_pRtc = NULL;
	}

}
