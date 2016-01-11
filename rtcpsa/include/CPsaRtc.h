#ifndef _PSARTC_H
#define _PSARTC_H
#include "pachook.h"
#include "CPropertiesManager.h"
#include "CRtcStack.h"

_CLASSDEF(CPsaRtc)

 class CPsaRtc{
 public:
	 CPsaRtc(INT psaid);
	 virtual ~CPsaRtc();

	 BOOL init();
	 void doActive();
	 BOOL doSendMsg(PTMsg msg);
private:
	 INT m_psaid;
 };

#endif
