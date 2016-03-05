/*
 * ex: set ro:
 * DO NOT EDIT.
 * generated by smc (http://smc.sourceforge.net/)
 * from file : CSipTermCall_sm.sm
 */

/*********************************************************************
 * Copyright (c)2010-2012, by BUPT
 * All rights reserved.

 * FileName:       CSipTermCall.sm
 * System:         webrtc
 * SubSystem:      gateway
 * Author:         Yadong Zhang
 * Date:           2016-1-16
 * Version:        1.0
 * Description:
 Sip_terminal_call module, WebRTC???????��????

 *******************************************************************************/

#include "CR2SCallModule.h"
#include "CSipTermCall_sm.h"

using namespace statemap;

// Static class declarations.
CSipTermCallState_IDLE CSipTermCallState::IDLE("CSipTermCallState::IDLE", 0);
CSipTermCallState_CALLPROC CSipTermCallState::CALLPROC(
		"CSipTermCallState::CALLPROC", 1);
CSipTermCallState_BEAR_GATEWAY_READY CSipTermCallState::BEAR_GATEWAY_READY(
		"CSipTermCallState::BEAR_GATEWAY_READY", 2);
CSipTermCallState_BEAR_CONFIRMING CSipTermCallState::BEAR_CONFIRMING(
		"CSipTermCallState::BEAR_CONFIRMING", 3);
CSipTermCallState_BEAR_CONFIRMED CSipTermCallState::BEAR_CONFIRMED(
		"CSipTermCallState::BEAR_CONFIRMED", 4);
CSipTermCallState_BEAR_CLIENT_READY CSipTermCallState::BEAR_CLIENT_READY(
		"CSipTermCallState::BEAR_CLIENT_READY", 5);
CSipTermCallState_BEAR_MODYFYING CSipTermCallState::BEAR_MODYFYING(
		"CSipTermCallState::BEAR_MODYFYING", 6);
CSipTermCallState_ACTIVE CSipTermCallState::ACTIVE("CSipTermCallState::ACTIVE",
		7);
CSipTermCallState_ACTIVE_WAIT_BEAR_MODIFYING
		CSipTermCallState::ACTIVE_WAIT_BEAR_MODIFYING(
				"CSipTermCallState::ACTIVE_WAIT_BEAR_MODIFYING", 8);
CSipTermCallState_ACTIVE_WAIT_ACK CSipTermCallState::ACTIVE_WAIT_ACK(
		"CSipTermCallState::ACTIVE_WAIT_ACK", 9);
CSipTermCallState_CLOSED CSipTermCallState::CLOSED("CSipTermCallState::CLOSED",
		10);

void CR2SCallModuleState_Sip::onAck(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	Default(context);
	return;
}

void CR2SCallModuleState_Sip::onBye(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	Default(context);
	return;
}

void CR2SCallModuleState_Sip::onClose(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	Default(context);
	return;
}

void CR2SCallModuleState_Sip::onError(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	Default(context);
	return;
}

void CR2SCallModuleState_Sip::onInvite(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	Default(context);
	return;
}

void CR2SCallModuleState_Sip::onNotify(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	Default(context);
	return;
}

void CR2SCallModuleState_Sip::onResponse(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	Default(context);
	return;
}

void CR2SCallModuleState_Sip::onSdpAnswer(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	Default(context);
	return;
}


void CR2SCallModuleState_Sip::onTimeOut(CSipTermCallContext& context,
		TTimeMarkExt timerMark) {
	Default(context);
	return;
}

void CR2SCallModuleState_Sip::onUpdate(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	Default(context);
	return;
}

void CR2SCallModuleState_Sip::Default(CSipTermCallContext& context) {
	throw(TransitionUndefinedException(context.getState().getName(),
			context.getTransition()));

	return;
}

void CSipTermCallState_Default::onSdpAnswer(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	CR2SCallModuleState_Sip& endState = context.getState();

	context.clearState();
	try {
		ctxt.handleUnexpectedMsg(msg);
		context.setState(endState);
	} catch (...) {
		context.setState(endState);
		throw;
	}

	return;
}

void CSipTermCallState_Default::onNotify(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	CR2SCallModuleState_Sip& endState = context.getState();

	context.clearState();
	try {
		ctxt.handleUnexpectedMsg(msg);
		context.setState(endState);
	} catch (...) {
		context.setState(endState);
		throw;
	}

	return;
}

void CSipTermCallState_Default::onResponse(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	CR2SCallModuleState_Sip& endState = context.getState();

	context.clearState();
	try {
		ctxt.handleUnexpectedMsg(msg);
		context.setState(endState);
	} catch (...) {
		context.setState(endState);
		throw;
	}

	return;
}

void CSipTermCallState_Default::onClose(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	CR2SCallModuleState_Sip& endState = context.getState();

	context.clearState();
	try {
		ctxt.handleUnexpectedMsg(msg);
		context.setState(endState);
	} catch (...) {
		context.setState(endState);
		throw;
	}

	return;
}

void CSipTermCallState_Default::onError(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	CR2SCallModuleState_Sip& endState = context.getState();

	context.clearState();
	try {
		ctxt.handleUnexpectedMsg(msg);
		context.setState(endState);
	} catch (...) {
		context.setState(endState);
		throw;
	}

	return;
}

void CSipTermCallState_Default::onAck(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	CR2SCallModuleState_Sip& endState = context.getState();

	context.clearState();
	try {
		ctxt.handleUnexpectedMsg(msg);
		context.setState(endState);
	} catch (...) {
		context.setState(endState);
		throw;
	}

	return;
}

void CSipTermCallState_Default::onUpdate(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	CR2SCallModuleState_Sip& endState = context.getState();

	context.clearState();
	try {
		ctxt.handleUnexpectedMsg(msg);
		context.setState(endState);
	} catch (...) {
		context.setState(endState);
		throw;
	}

	return;
}

void CSipTermCallState_Default::onInvite(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	CR2SCallModuleState_Sip& endState = context.getState();

	context.clearState();
	try {
		ctxt.handleUnexpectedMsg(msg);
		context.setState(endState);
	} catch (...) {
		context.setState(endState);
		throw;
	}

	return;
}

void CSipTermCallState_Default::onBye(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	CR2SCallModuleState_Sip& endState = context.getState();

	context.clearState();
	try {
		ctxt.handleUnexpectedMsg(msg);
		context.setState(endState);
	} catch (...) {
		context.setState(endState);
		throw;
	}

	return;
}

void CSipTermCallState_Default::onTimeOut(CSipTermCallContext& context,
		TTimeMarkExt timerMark) {

	return;
}

void CSipTermCallState_IDLE::onNotify(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	(context.getState()).Exit(context);
	context.clearState();
	try {
		ctxt.setTimer(SIP_200OK_TIMEOUT);
		ctxt.sendNoSdpInviteToIMS();
		context.setState(CSipTermCallState::CALLPROC);
	} catch (...) {
		context.setState(CSipTermCallState::CALLPROC);
		throw;
	}
	(context.getState()).Entry(context);

	return;
}

void CSipTermCallState_CALLPROC::onClose(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	(context.getState()).Exit(context);
	context.clearState();
	try {
		ctxt.stopTimer_Sip();
		ctxt.sendCancelToIMS();
		context.setState(CSipTermCallState::CLOSED);
	} catch (...) {
		context.setState(CSipTermCallState::CLOSED);
		throw;
	}
	(context.getState()).Entry(context);

	return;
}

void CSipTermCallState_CALLPROC::onResponse(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	if (true == ctxt.isResp1xx(msg) && false == ctxt.isWithSDP(msg)) {
		CR2SCallModuleState_Sip& endState = context.getState();

		context.clearState();
		try {
			ctxt.stopTimer_Sip();
			ctxt.setTimer(SIP_CONNECTING_TIMEOUT);
			context.setState(endState);
		} catch (...) {
			context.setState(endState);
			throw;
		}
	} else if (true == ctxt.isResp3xx_6xx(msg))

	{
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.stopTimer_Sip();
			ctxt.notifyRtcOrigCallError(msg);
			context.setState(CSipTermCallState::CLOSED);
		} catch (...) {
			context.setState(CSipTermCallState::CLOSED);
			throw;
		}
		(context.getState()).Entry(context);
	} else if (true == ctxt.isResp1xx(msg) && true == ctxt.isWithSDP(msg)
			&& true == ctxt.compAndModifySdpWithRtc(msg))

	{
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.stopTimer_Sip();
			ctxt.setUACTag(msg);
			ctxt.notifyRtcOrigCallSdp();
			ctxt.sendReqToBear_Sip();
			ctxt.setTimer(SIP_WAITBEAR_TIMEOUT);
			context.setState(CSipTermCallState::BEAR_GATEWAY_READY);
		} catch (...) {
			context.setState(CSipTermCallState::BEAR_GATEWAY_READY);
			throw;
		}
		(context.getState()).Entry(context);
	} else if (true == ctxt.isResp1xx(msg) && true == ctxt.isWithSDP(msg)
			&& false == ctxt.compAndModifySdpWithRtc(msg))

	{
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.stopTimer_Sip();
			ctxt.setUACTag(msg);
			ctxt.notifyRtcOrigCallClose();
			context.setState(CSipTermCallState::CLOSED);
		} catch (...) {
			context.setState(CSipTermCallState::CLOSED);
			throw;
		}
		(context.getState()).Entry(context);
	} else if (true == ctxt.isResp2xx(msg) && true == ctxt.isWithSDP(msg)
			&& true == ctxt.compAndModifySdpWithRtc(msg)) {
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.stopTimer_Sip();
			ctxt.setUACTag(msg);
			ctxt.notifyRtcOrigCallSdp();
			ctxt.sendReqToBear_Sip();
			ctxt.setTimer(SIP_WAITBEAR_TIMEOUT);
			context.setState(CSipTermCallState::BEAR_CLIENT_READY);
		} catch (...) {
			context.setState(CSipTermCallState::BEAR_CLIENT_READY);
			throw;
		}
		(context.getState()).Entry(context);
	} else if (true == ctxt.isResp2xx(msg) && true == ctxt.isWithSDP(msg)
			&& false == ctxt.compAndModifySdpWithRtc(msg)) {
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.stopTimer_Sip();
			ctxt.setUACTag(msg);
			ctxt.notifyRtcOrigCallClose();
			context.setState(CSipTermCallState::CLOSED);
		} catch (...) {
			context.setState(CSipTermCallState::CLOSED);
			throw;
		}
		(context.getState()).Entry(context);

	} else if (true == ctxt.isResp2xx(msg) && false == ctxt.isWithSDP(msg))

	{
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.stopTimer_Sip();
			ctxt.setUACTag(msg);
			ctxt.notifyRtcOrigCallClose();
			ctxt.sendCancelToIMS();
			context.setState(CSipTermCallState::CLOSED);
		} catch (...) {
			context.setState(CSipTermCallState::CLOSED);
			throw;
		}
		(context.getState()).Entry(context);
	} else {
		CSipTermCallState_Default::onResponse(context, msg);
	}

	return;
}

void CSipTermCallState_CALLPROC::onTimeOut(CSipTermCallContext& context,
		TTimeMarkExt timerMark) {
	CR2SCallModule& ctxt(context.getOwner());

	(context.getState()).Exit(context);
	context.clearState();
	try {
		ctxt.stopTimer_Sip();
		ctxt.notifyRtcOrigCallClose();
		ctxt.sendCancelToIMS();
		context.setState(CSipTermCallState::CLOSED);
	} catch (...) {
		context.setState(CSipTermCallState::CLOSED);
		throw;
	}
	(context.getState()).Entry(context);

	return;
}

void CSipTermCallState_BEAR_GATEWAY_READY::onClose(//from bear
		CSipTermCallContext& context, TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	(context.getState()).Exit(context);
	context.clearState();
	try {
		ctxt.stopTimer_Sip();
		ctxt.sendCloseToBear_Sip();
		ctxt.sendCancelToIMS();
		//notify rtc
		context.setState(CSipTermCallState::CLOSED);
	} catch (...) {
		context.setState(CSipTermCallState::CLOSED);
		throw;
	}
	(context.getState()).Entry(context);

	return;
}

void CSipTermCallState_BEAR_GATEWAY_READY::onResponse(
		CSipTermCallContext& context, TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	if (true == ctxt.isResp3xx_6xx(msg)) {
		//IMS return error. Example: timeout, 480
		printf("llllll\n");
		(context.getState()).Exit(context);
		context.clearState();
		printf("lll\n");
		try {
			ctxt.stopTimer_Sip();
			ctxt.notifyRtcOrigCallError(msg);
			ctxt.sendCloseToBear_Sip();
			context.setState(CSipTermCallState::CLOSED);
		} catch (...) {
			context.setState(CSipTermCallState::CLOSED);
			throw;
		}
		(context.getState()).Entry(context);

	}
}

void CSipTermCallState_BEAR_GATEWAY_READY::onError(
		CSipTermCallContext& context, TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	(context.getState()).Exit(context);
	context.clearState();
	try {
		ctxt.stopTimer_Sip();
		ctxt.sendCancelToIMS();
		ctxt.notifyRtcOrigCallClose();
		context.setState(CSipTermCallState::CLOSED);
	} catch (...) {
		context.setState(CSipTermCallState::CLOSED);
		throw;
	}
	(context.getState()).Entry(context);

	return;
}

void CSipTermCallState_BEAR_GATEWAY_READY::onSdpAnswer(
		CSipTermCallContext& context, TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	(context.getState()).Exit(context);
	context.clearState();
	try {
		ctxt.stopTimer_Sip();
		ctxt.setIMSConnId(msg);
		ctxt.sendPrackToIMS(msg);
		ctxt.setTimer(SIP_200OK_TIMEOUT);
		context.setState(CSipTermCallState::BEAR_CONFIRMING);
	} catch (...) {
		context.setState(CSipTermCallState::BEAR_CONFIRMING);
		throw;
	}
	(context.getState()).Entry(context);

	return;
}

void CSipTermCallState_BEAR_GATEWAY_READY::onTimeOut(CSipTermCallContext& context, TTimeMarkExt timerMark){
	CR2SCallModule& ctxt(context.getOwner());

	(context.getState()).Exit(context);
	context.clearState();
	try {
		ctxt.stopTimer_Sip();
		ctxt.sendCancelToIMS();
		ctxt.notifyRtcOrigCallError(ERROR_TIMEOUT);
		ctxt.notifyRtcOrigCallClose();
		context.setState(CSipTermCallState::CLOSED);
	} catch (...) {
		context.setState(CSipTermCallState::CLOSED);
		throw;
	}
	(context.getState()).Entry(context);

	return;
}

void CSipTermCallState_BEAR_CONFIRMING::onClose(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	if (true == ctxt.isFromRtc(msg)) {
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.stopTimer_Sip();
			ctxt.sendCloseToBear_Sip();
			ctxt.sendCancelToIMS();
			context.setState(CSipTermCallState::CLOSED);
		} catch (...) {
			context.setState(CSipTermCallState::CLOSED);
			throw;
		}
		(context.getState()).Entry(context);
	} else if (false == ctxt.isFromRtc(msg))

	{
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.stopTimer_Sip();
			ctxt.notifyRtcOrigCallClose();
			ctxt.sendCancelToIMS();
			context.setState(CSipTermCallState::CLOSED);
		} catch (...) {
			context.setState(CSipTermCallState::CLOSED);
			throw;
		}
		(context.getState()).Entry(context);
	} else {
		CSipTermCallState_Default::onClose(context, msg);
	}

	return;
}

void CSipTermCallState_BEAR_CONFIRMING::onResponse(
		CSipTermCallContext& context, TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	if ("PRACK" != ctxt.checkRespCseqMothod(msg)) {
//		(context.getState()).Exit(context);
//		context.clearState();
//		try {
//			ctxt.sendCancelToIMS();
//			ctxt.sendCloseToBear_Sip();
//			ctxt.notifyRtcOrigCallClose();
//			context.setState(CSipTermCallState::CLOSED);
//		} catch (...) {
//			context.setState(CSipTermCallState::CLOSED);
//			throw;
//		}
//		(context.getState()).Entry(context);
	} else if (true == ctxt.isResp2xx(msg))

	{
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.stopTimer_Sip();
			ctxt.setTimer(SIP_RING_TIMEOUT);
			printf("set ring timeout\n");
			context.setState(CSipTermCallState::BEAR_CONFIRMED);
		} catch (...) {
			context.setState(CSipTermCallState::BEAR_CONFIRMED);
			throw;
		}
		(context.getState()).Entry(context);
	} else if (false == ctxt.isResp2xx(msg))

	{
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.stopTimer_Sip();
			ctxt.sendCloseToBear_Sip();
			ctxt.sendCancelToIMS();
			ctxt.notifyRtcOrigCallClose();
			context.setState(CSipTermCallState::CLOSED);
		} catch (...) {
			context.setState(CSipTermCallState::CLOSED);
			throw;
		}
		(context.getState()).Entry(context);
	} else {
		CSipTermCallState_Default::onResponse(context, msg);
	}

	return;
}

void CSipTermCallState_BEAR_CONFIRMING::onTimeOut(CSipTermCallContext& context,
		TTimeMarkExt timerMark) {
	CR2SCallModule& ctxt(context.getOwner());

	(context.getState()).Exit(context);
	context.clearState();
	try {
		ctxt.stopTimer_Sip();
		ctxt.sendCancelToIMS();
		ctxt.sendCloseToBear_Sip();
		ctxt.notifyRtcOrigCallClose();
		context.setState(CSipTermCallState::CLOSED);
	} catch (...) {
		context.setState(CSipTermCallState::CLOSED);
		throw;
	}
	(context.getState()).Entry(context);

	return;
}

void CSipTermCallState_BEAR_CONFIRMED::onClose(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	if (true == ctxt.isFromRtc(msg)) {
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.stopTimer_Sip();
			ctxt.sendCancelToIMS();
			ctxt.sendCloseToBear_Sip();
			context.setState(CSipTermCallState::CLOSED);
		} catch (...) {
			context.setState(CSipTermCallState::CLOSED);
			throw;
		}
		(context.getState()).Entry(context);
	} else if (false == ctxt.isFromRtc(msg))

	{
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.stopTimer_Sip();
			ctxt.sendCancelToIMS();
			ctxt.notifyRtcOrigCallClose();
			context.setState(CSipTermCallState::CLOSED);
		} catch (...) {
			context.setState(CSipTermCallState::CLOSED);
			throw;
		}
		(context.getState()).Entry(context);
	} else {
		CSipTermCallState_Default::onClose(context, msg);
	}

	return;
}

void CSipTermCallState_BEAR_CONFIRMED::onResponse(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());
	printf("BEAR_CONFIRMED: onResponse\n");

	if ("INVITE" != ctxt.checkRespCseqMothod(msg)) {
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.stopTimer_Sip();
			ctxt.sendCancelToIMS();
			ctxt.sendCloseToBear_Sip();
			ctxt.notifyRtcOrigCallClose();
			context.setState(CSipTermCallState::CLOSED);
		} catch (...) {
			context.setState(CSipTermCallState::CLOSED);
			throw;
		}
		(context.getState()).Entry(context);
	} else if (true == ctxt.isResp2xx(msg) && true == ctxt.isWithSDP(msg)
			&& true == ctxt.compAndModifySdpWithRtc(msg))

	{
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.stopTimer_Sip();
			ctxt.sendReqToBear_Sip();
			ctxt.setTimer(SIP_WAITBEAR_TIMEOUT);
			context.setState(CSipTermCallState::BEAR_CLIENT_READY);
		} catch (...) {
			context.setState(CSipTermCallState::BEAR_CLIENT_READY);
			throw;
		}
		(context.getState()).Entry(context);
	} else if (true == ctxt.isResp2xx(msg) && true == ctxt.isWithSDP(msg)
			&& false == ctxt.compAndModifySdpWithRtc(msg))

	{
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.stopTimer_Sip();
			ctxt.sendCloseToBear_Sip();
			ctxt.notifyRtcOrigCallClose();
			ctxt.sendCancelToIMS();
			context.setState(CSipTermCallState::CLOSED);
		} catch (...) {
			context.setState(CSipTermCallState::CLOSED);
			throw;
		}
		(context.getState()).Entry(context);
	} else if (true == ctxt.isResp2xx(msg) && false == ctxt.isWithSDP(msg))

	{
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.stopTimer_Sip();
			ctxt.sendAckToIMS(NULL);
			context.setState(CSipTermCallState::ACTIVE);
		} catch (...) {
			context.setState(CSipTermCallState::ACTIVE);
			throw;
		}
		(context.getState()).Entry(context);
	} else if (true == ctxt.isResp3xx_6xx(msg)) {
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.stopTimer_Sip();
			ctxt.sendCloseToBear_Sip();
			ctxt.notifyRtcOrigCallClose();
			context.setState(CSipTermCallState::CLOSED);
		} catch (...) {
			context.setState(CSipTermCallState::CLOSED);
			throw;
		}
		(context.getState()).Entry(context);
	} else {
		CSipTermCallState_Default::onResponse(context, msg);
	}

	return;
}

void CSipTermCallState_BEAR_CONFIRMED::onTimeOut(CSipTermCallContext& context,
		TTimeMarkExt timerMark) {
	CR2SCallModule& ctxt(context.getOwner());

	(context.getState()).Exit(context);
	context.clearState();
	try {

		printf("BEAR_CONFIRMED::onTimeOut\n");
		ctxt.stopTimer_Sip();
		ctxt.sendCancelToIMS();
		ctxt.sendCloseToBear_Sip();
		ctxt.notifyRtcOrigCallClose();
		context.setState(CSipTermCallState::CLOSED);
	} catch (...) {
		context.setState(CSipTermCallState::CLOSED);
		throw;
	}
	(context.getState()).Entry(context);

	return;
}

void CSipTermCallState_BEAR_CONFIRMED::onUpdate(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	if (false == ctxt.isWithSDP(msg)) {
		CR2SCallModuleState_Sip& endState = context.getState();

		context.clearState();
		try {
			ctxt.stopTimer_Sip();
			ctxt.send200OKForUpdateToIMS(NULL);
			context.setState(endState);
		} catch (...) {
			context.setState(endState);
			throw;
		}
	} else if (true == ctxt.compSdpWithOld(msg)) {//新sdp与原sdp 媒体类型相同
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.stopTimer_Sip();
			ctxt.sendReqToBear_Sip();
			ctxt.setTimer(SIP_WAITBEAR_TIMEOUT);
			context.setState(CSipTermCallState::BEAR_MODYFYING);
		} catch (...) {
			context.setState(CSipTermCallState::BEAR_MODYFYING);
			throw;
		}
		(context.getState()).Entry(context);
	} else if (false == ctxt.compSdpWithOld(msg)) {//新sdp与原sdp 媒体类型不同，需要告知web端，sdp发生修改
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.stopTimer_Sip();
			ctxt.notifyRtcOrigCallSdp();
			ctxt.sendReqToBear_Sip();
			ctxt.setTimer(SIP_WAITBEAR_TIMEOUT);
			context.setState(CSipTermCallState::BEAR_MODYFYING);
		} catch (...) {
			context.setState(CSipTermCallState::BEAR_MODYFYING);
			throw;
		}
		(context.getState()).Entry(context);
	} else {
		CSipTermCallState_Default::onUpdate(context, msg);
	}

	return;
}

void CSipTermCallState_BEAR_CLIENT_READY::onClose(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	(context.getState()).Exit(context);
	context.clearState();
	try {
		ctxt.stopTimer_Sip();
		ctxt.sendCloseToBear_Sip();
		ctxt.sendCancelToIMS();
		context.setState(CSipTermCallState::CLOSED);
	} catch (...) {
		context.setState(CSipTermCallState::CLOSED);
		throw;
	}
	(context.getState()).Entry(context);

	return;
}

void CSipTermCallState_BEAR_CLIENT_READY::onError(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	(context.getState()).Exit(context);
	context.clearState();
	try {
		ctxt.stopTimer_Sip();
		ctxt.sendCancelToIMS();
		ctxt.notifyRtcOrigCallClose();
		context.setState(CSipTermCallState::CLOSED);
	} catch (...) {
		context.setState(CSipTermCallState::CLOSED);
		throw;
	}
	(context.getState()).Entry(context);

	return;
}

void CSipTermCallState_BEAR_CLIENT_READY::onSdpAnswer(
		CSipTermCallContext& context, TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	(context.getState()).Exit(context);
	context.clearState();
	try {
		ctxt.stopTimer_Sip();
		ctxt.setIMSConnId(msg);
		ctxt.sendAckToIMS(msg);
		context.setState(CSipTermCallState::ACTIVE);
	} catch (...) {
		context.setState(CSipTermCallState::ACTIVE);
		throw;
	}
	(context.getState()).Entry(context);

	return;
}

void CSipTermCallState_BEAR_CLIENT_READY::onTimeOut(
		CSipTermCallContext& context, TTimeMarkExt timerMark) {
	CR2SCallModule& ctxt(context.getOwner());

	(context.getState()).Exit(context);
	context.clearState();
	try {//ToDO xms is shutdown, sg break
		printf("1\n");
		ctxt.stopTimer_Sip();
		printf("2\n");
		ctxt.notifyRtcOrigCallError(ERROR_TIMEOUT);
		printf("3\n");
		ctxt.sendCancelToIMS();
		printf("4\n");
		//ctxt.notifyRtcOrigCallClose();
		context.setState(CSipTermCallState::CLOSED);
	} catch (...) {
		context.setState(CSipTermCallState::CLOSED);
		throw;
	}
	(context.getState()).Entry(context);

	return;
}

void CSipTermCallState_BEAR_MODYFYING::onClose(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	(context.getState()).Exit(context);
	context.clearState();
	try {
		ctxt.stopTimer_Sip();
		ctxt.sendCloseToBear_Sip();
		ctxt.sendCancelToIMS();
		context.setState(CSipTermCallState::CLOSED);
	} catch (...) {
		context.setState(CSipTermCallState::CLOSED);
		throw;
	}
	(context.getState()).Entry(context);

	return;
}

void CSipTermCallState_BEAR_MODYFYING::onError(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	(context.getState()).Exit(context);
	context.clearState();
	try {
		ctxt.stopTimer_Sip();
		ctxt.sendCancelToIMS();
		ctxt.notifyRtcOrigCallClose();
		context.setState(CSipTermCallState::CLOSED);
	} catch (...) {
		context.setState(CSipTermCallState::CLOSED);
		throw;
	}
	(context.getState()).Entry(context);

	return;
}

void CSipTermCallState_BEAR_MODYFYING::onResponse(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	if ("INVITE" != ctxt.checkRespCseqMothod(msg) || false
			!= ctxt.isResp3xx_6xx(msg)) {
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.stopTimer_Sip();
			ctxt.sendCancelToIMS();
			ctxt.sendCloseToBear_Sip();
			ctxt.notifyRtcOrigCallClose();
			context.setState(CSipTermCallState::CLOSED);
		} catch (...) {
			context.setState(CSipTermCallState::CLOSED);
			throw;
		}
		(context.getState()).Entry(context);
	} else if (true == ctxt.isResp3xx_6xx(msg))

	{
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.stopTimer_Sip();
			ctxt.sendCloseToBear_Sip();
			ctxt.notifyRtcOrigCallClose();
			context.setState(CSipTermCallState::CLOSED);
		} catch (...) {
			context.setState(CSipTermCallState::CLOSED);
			throw;
		}
		(context.getState()).Entry(context);
	} else {
		CSipTermCallState_Default::onResponse(context, msg);
	}

	return;
}

void CSipTermCallState_BEAR_MODYFYING::onSdpAnswer(
		CSipTermCallContext& context, TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	(context.getState()).Exit(context);
	context.clearState();
	try {
		ctxt.stopTimer_Sip();
		ctxt.setIMSConnId(msg);
		ctxt.send200OKForUpdateToIMS(msg);
		context.setState(CSipTermCallState::BEAR_CONFIRMED);
	} catch (...) {
		context.setState(CSipTermCallState::BEAR_CONFIRMED);
		throw;
	}
	(context.getState()).Entry(context);

	return;
}

void CSipTermCallState_BEAR_MODYFYING::onTimeOut(CSipTermCallContext& context,
		TTimeMarkExt timerMark) {
	CR2SCallModule& ctxt(context.getOwner());

	(context.getState()).Exit(context);
	context.clearState();
	try {
		ctxt.stopTimer_Sip();
		ctxt.notifyRtcOrigCallError(ERROR_TIMEOUT);
		ctxt.sendCancelToIMS();
		ctxt.notifyRtcOrigCallClose();
		context.setState(CSipTermCallState::CLOSED);
	} catch (...) {
		context.setState(CSipTermCallState::CLOSED);
		throw;
	}
	(context.getState()).Entry(context);

	return;
}

void CSipTermCallState_ACTIVE::onBye(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	(context.getState()).Exit(context);
	context.clearState();
	try {
		ctxt.sendCloseToBear_Sip();
		ctxt.notifyRtcOrigCallClose();
		context.setState(CSipTermCallState::CLOSED);
	} catch (...) {
		context.setState(CSipTermCallState::CLOSED);
		throw;
	}
	(context.getState()).Entry(context);

	return;
}

void CSipTermCallState_ACTIVE::onClose(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	if (true == ctxt.isFromRtc(msg)) {
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.sendCloseToBear_Sip();
			ctxt.sendByeToIMS();
			context.setState(CSipTermCallState::CLOSED);
		} catch (...) {
			context.setState(CSipTermCallState::CLOSED);
			throw;
		}
		(context.getState()).Entry(context);
	} else if (false == ctxt.isFromRtc(msg))

	{
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.sendByeToIMS();
			ctxt.notifyRtcOrigCallClose();
			context.setState(CSipTermCallState::CLOSED);
		} catch (...) {
			context.setState(CSipTermCallState::CLOSED);
			throw;
		}
		(context.getState()).Entry(context);
	} else {
		CSipTermCallState_Default::onClose(context, msg);
	}

	return;
}

void CSipTermCallState_ACTIVE::onInvite(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	if (false == ctxt.isWithSDP(msg)) {
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.send200OKForInviteToIMS(NULL);
			ctxt.setTimer(SIP_ACK_TIMEOUT);
			context.setState(CSipTermCallState::ACTIVE_WAIT_ACK);
		} catch (...) {
			context.setState(CSipTermCallState::ACTIVE_WAIT_ACK);
			throw;
		}
		(context.getState()).Entry(context);
	} else if (true == ctxt.compSdpWithOld(msg))

	{
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.notifyRtcOrigCallSdp();
			ctxt.setInviteFlag();
			ctxt.sendReqToBear_Sip();
			ctxt.setTimer(SIP_WAITBEAR_TIMEOUT);
			context.setState(CSipTermCallState::ACTIVE_WAIT_BEAR_MODIFYING);
		} catch (...) {
			context.setState(CSipTermCallState::ACTIVE_WAIT_BEAR_MODIFYING);
			throw;
		}
		(context.getState()).Entry(context);
	} else if (false == ctxt.compSdpWithOld(msg))

	{
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.setInviteFlag();
			ctxt.sendReqToBear_Sip();
			ctxt.setTimer(SIP_WAITBEAR_TIMEOUT);
			context.setState(CSipTermCallState::ACTIVE_WAIT_BEAR_MODIFYING);
		} catch (...) {
			context.setState(CSipTermCallState::ACTIVE_WAIT_BEAR_MODIFYING);
			throw;
		}
		(context.getState()).Entry(context);
	} else {
		CSipTermCallState_Default::onInvite(context, msg);
	}

	return;
}

void CSipTermCallState_ACTIVE::onUpdate(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	if (false == ctxt.isWithSDP(msg)) {
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.send200OKForUpdateToIMS(NULL);
			context.setState(CSipTermCallState::ACTIVE);
		} catch (...) {
			context.setState(CSipTermCallState::ACTIVE);
			throw;
		}
		(context.getState()).Entry(context);
	} else if (true == ctxt.compSdpWithOld(msg))

	{
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.notifyRtcOrigCallSdp();
			ctxt.setUpdateFlag();
			ctxt.sendReqToBear_Sip();
			ctxt.setTimer(SIP_WAITBEAR_TIMEOUT);
			context.setState(CSipTermCallState::ACTIVE_WAIT_BEAR_MODIFYING);
		} catch (...) {
			context.setState(CSipTermCallState::ACTIVE_WAIT_BEAR_MODIFYING);
			throw;
		}
		(context.getState()).Entry(context);
	} else if (false == ctxt.compSdpWithOld(msg))

	{
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.setUpdateFlag();
			ctxt.sendReqToBear_Sip();
			ctxt.setTimer(SIP_WAITBEAR_TIMEOUT);
			context.setState(CSipTermCallState::ACTIVE_WAIT_BEAR_MODIFYING);
		} catch (...) {
			context.setState(CSipTermCallState::ACTIVE_WAIT_BEAR_MODIFYING);
			throw;
		}
		(context.getState()).Entry(context);
	} else {
		CSipTermCallState_Default::onUpdate(context, msg);
	}

	return;
}

void CSipTermCallState_ACTIVE_WAIT_BEAR_MODIFYING::onBye(
		CSipTermCallContext& context, TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	(context.getState()).Exit(context);
	context.clearState();
	try {
		ctxt.stopTimer_Sip();
		ctxt.sendCloseToBear_Sip();
		ctxt.notifyRtcOrigCallClose();
		context.setState(CSipTermCallState::CLOSED);
	} catch (...) {
		context.setState(CSipTermCallState::CLOSED);
		throw;
	}
	(context.getState()).Entry(context);

	return;
}

void CSipTermCallState_ACTIVE_WAIT_BEAR_MODIFYING::onClose(
		CSipTermCallContext& context, TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	if (true == ctxt.isFromRtc(msg)) {
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.stopTimer_Sip();
			ctxt.sendCloseToBear_Sip();
			ctxt.sendByeToIMS();
			context.setState(CSipTermCallState::CLOSED);
		} catch (...) {
			context.setState(CSipTermCallState::CLOSED);
			throw;
		}
		(context.getState()).Entry(context);
	} else if (false == ctxt.isFromRtc(msg))

	{
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.stopTimer_Sip();
			ctxt.sendByeToIMS();
			ctxt.notifyRtcOrigCallClose();
			context.setState(CSipTermCallState::CLOSED);
		} catch (...) {
			context.setState(CSipTermCallState::CLOSED);
			throw;
		}
		(context.getState()).Entry(context);
	} else {
		CSipTermCallState_Default::onClose(context, msg);
	}

	return;
}

void CSipTermCallState_ACTIVE_WAIT_BEAR_MODIFYING::onSdpAnswer(
		CSipTermCallContext& context, TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	if (true == ctxt.isUpdateFlag()) {
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.resetFlag();
			ctxt.stopTimer_Sip();
			ctxt.setIMSConnId(msg);
			ctxt.send200OKForUpdateToIMS(msg);
			context.setState(CSipTermCallState::ACTIVE);
		} catch (...) {
			context.setState(CSipTermCallState::ACTIVE);
			throw;
		}
		(context.getState()).Entry(context);
	} else if (true == ctxt.isInviteFlag())

	{
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.resetFlag();
			ctxt.stopTimer_Sip();
			ctxt.setIMSConnId(msg);
			ctxt.send200OKForInviteToIMS(msg);
			ctxt.setTimer(SIP_ACK_TIMEOUT);
			context.setState(CSipTermCallState::ACTIVE_WAIT_ACK);
		} catch (...) {
			context.setState(CSipTermCallState::ACTIVE_WAIT_ACK);
			throw;
		}
		(context.getState()).Entry(context);
	} else {
		CSipTermCallState_Default::onSdpAnswer(context, msg);
	}

	return;
}

void CSipTermCallState_ACTIVE_WAIT_BEAR_MODIFYING::onTimeOut(
		CSipTermCallContext& context, TTimeMarkExt timerMark) {
	CR2SCallModule& ctxt(context.getOwner());

	(context.getState()).Exit(context);
	context.clearState();
	try {
		ctxt.stopTimer_Sip();
		ctxt.sendCloseToBear_Sip();
		ctxt.sendByeToIMS();
		ctxt.notifyRtcOrigCallClose();
		context.setState(CSipTermCallState::CLOSED);
	} catch (...) {
		context.setState(CSipTermCallState::CLOSED);
		throw;
	}
	(context.getState()).Entry(context);

	return;
}

void CSipTermCallState_ACTIVE_WAIT_ACK::onAck(CSipTermCallContext& context,
		TUniNetMsg* msg) {

	(context.getState()).Exit(context);
	context.setState(CSipTermCallState::ACTIVE);
	(context.getState()).Entry(context);

	return;
}

void CSipTermCallState_ACTIVE_WAIT_ACK::onBye(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	(context.getState()).Exit(context);
	context.clearState();
	try {
		ctxt.stopTimer_Sip();
		ctxt.sendCloseToBear_Sip();
		ctxt.notifyRtcOrigCallClose();
		context.setState(CSipTermCallState::CLOSED);
	} catch (...) {
		context.setState(CSipTermCallState::CLOSED);
		throw;
	}
	(context.getState()).Entry(context);

	return;
}

void CSipTermCallState_ACTIVE_WAIT_ACK::onClose(CSipTermCallContext& context,
		TUniNetMsg* msg) {
	CR2SCallModule& ctxt(context.getOwner());

	if (true == ctxt.isFromRtc(msg)) {
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.stopTimer_Sip();
			ctxt.sendCloseToBear_Sip();
			ctxt.sendByeToIMS();
			context.setState(CSipTermCallState::CLOSED);
		} catch (...) {
			context.setState(CSipTermCallState::CLOSED);
			throw;
		}
		(context.getState()).Entry(context);
	} else if (false == ctxt.isFromRtc(msg))

	{
		(context.getState()).Exit(context);
		context.clearState();
		try {
			ctxt.stopTimer_Sip();
			ctxt.sendByeToIMS();
			ctxt.notifyRtcOrigCallClose();
			context.setState(CSipTermCallState::CLOSED);
		} catch (...) {
			context.setState(CSipTermCallState::CLOSED);
			throw;
		}
		(context.getState()).Entry(context);
	} else {
		CSipTermCallState_Default::onClose(context, msg);
	}

	return;
}

void CSipTermCallState_ACTIVE_WAIT_ACK::onTimeOut(CSipTermCallContext& context,
		TTimeMarkExt timerMark) {
	CR2SCallModule& ctxt(context.getOwner());

	(context.getState()).Exit(context);
	context.clearState();
	try {
		ctxt.stopTimer_Sip();
		ctxt.sendCloseToBear_Sip();
		ctxt.sendByeToIMS();
		ctxt.notifyRtcOrigCallClose();
		context.setState(CSipTermCallState::CLOSED);
	} catch (...) {
		context.setState(CSipTermCallState::CLOSED);
		throw;
	}
	(context.getState()).Entry(context);

	return;
}

void CSipTermCallState_CLOSED::Entry(CSipTermCallContext& context)

{
	CR2SCallModule& ctxt(context.getOwner());

	ctxt.endTask_Sip();
	return;
}

/*
 * Local variables:
 *  buffer-read-only: t
 * End:
 */
