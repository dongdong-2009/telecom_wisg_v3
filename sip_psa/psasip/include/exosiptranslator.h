/************************************************************************

 * Copyright (c)2010-2012, by BUPT
 * All rights reserved.

 * FileName：       exosiptranslator.h
 * System：         MCF
 * SubSystem：      PSA
 * Author：         Huang Haiping
 * Date：           2010.04.07
 * Version：        1.0
 * Description：
       eXosip 中间消息转换器

 *
 * Last Modified:
	  2010.04.07, 完成初始版本定义
		 By Huang Haiping


*************************************************************************/

#ifndef EXOSIPTRANSLATOR_H_
#define EXOSIPTRANSLATOR_H_

#include "comtypedef.h"
#include "pachook.h"
#include <eXosip2/eXosip.h>
#include "msgdef_sip.h"

namespace ExosipTranslator {
	/* Convert ctrl msg */
	PTSipCtrlMsg convertCtrlMsg2MCF(osip_message_t *sip, PTSipCtrlMsg pCtrlMsg);
	osip_message_t* convertMCF2CtrlMsg(PCTSipCtrlMsg pCtrlMsg, osip_message_t *sip);

	/* Convert SIP request */
	RTSipReq convertOsipReq2MCF(osip_message_t *req, RTSipReq mcfReq);
	osip_message_t* convertMCF2OsipReq(RCTSipReq mcfReq, osip_message_t *req);

	/* Convert SIP response */
	RTSipResp convertOsipResp2MCF(osip_message_t *response, RTSipResp mcfResp);
	osip_message_t* convertMCF2OsipResp(RCTSipResp mcfResp, osip_message_t *response);

	/* Convert osip call-id */
	RTSipCallId convertOsipCallId2MCF(osip_call_id_t *callId, RTSipCallId mcfCallId);
	osip_call_id_t* convertMCF2OsipCallId(RCTSipCallId mcfCallId, osip_call_id_t *callId);

	/* Convert osip from */
	RTSipAddress convertOsipFrom2MCF(osip_from_t *osipFrom, RTSipAddress mcfFrom);
	osip_from_t* convertMCF2OsipFrom(RCTSipAddress mcfFrom, osip_from_t *osipFrom);

	/* Convert osip to */
#define convertOsipTo2MCF(osipTo, mcfTo) convertOsipFrom2MCF(osipTo, mcfTo)
#define convertMCF2OsipTo(mcfTo, osipTo) convertMCF2OsipFrom(mcfTo, osipTo)
	//RTSipAddress convertOsipTo2MCF(osip_to_t *osipTo, RTSipAddress mcfTo);
	//osip_to_t* convertMCF2OsipTo(RCTSipAddress mcfTo, osip_to_t *osipTo);

	/* Convert osip contact */
#define convertOsipContact2MCF(osipContact, mcfSipContact) convertOsipFrom2MCF(osipContact, mcfSipContact)
#define convertMCF2OsipContact(mcfSipContact, osipContact) convertMCF2OsipFrom(mcfSipContact, osipContact)
	//RTSipAddress convertOsipContact2MCF(osip_contact_t* osipContact, RTSipAddress mcfSipContact);
	//osip_contact_t* convertMCF2OsipContact(RCTSipAddress mcfSipContact, osip_contact_t* osipContact);

	/* Convert osip uri */
	RTSipURI convertOsipUri2MCF(osip_uri_t *osipUri, RTSipURI mcfSipUri);
	osip_uri_t* convertMCF2OsipUri(RCTSipURI mcfSipUri, osip_uri_t *osipUri);

	/* Convert osip via */
	RTSipVia convertOsipVia2MCF(osip_via_t *osipVia, RTSipVia mcfVia);
	osip_via_t* convertMCF2OsipVia(RCTSipVia mcfVia, osip_via_t* osipVia);

	/* Convert osip route */
	RTSipAddress convertOsipRoute2MCF(osip_route_t* osipRoute, RTSipAddress mcfRoute);
	osip_route_t* convertMCF2OsipRoute(RCTSipAddress mcfRoute, osip_route_t* osipRoute);

	/* Convert REGISTER */
	RTSipRegister convertOsipRegister2MCF(osip_message_t *registerMsg, RTSipRegister mcfReg);
	osip_message_t* convertMCF2OsipRegister(RCTSipRegister mcfReg, osip_message_t *registerMsg);

	/* Convert Sip body */
	RTSipBody convertOsipBody2MCF(osip_body_t *sipBody, RTSipBody mcfBody);
	osip_body_t* convertMCF2OsipBody(RCTSipBody mcfBody, osip_body_t *sipBody);
	osip_message_t* fillOsipBody(RCTSipBody mcfBody, RCTSipContentType mcfContentType,
			osip_message_t* sip);

	/* Convert Sip content type */
	RTSipContentType convertOsipContentType2MCF(osip_content_type_t *sipContentType,
			RTSipContentType mcfContentType);
	osip_content_type* convertMCF2OsipContentType(RCTSipContentType mcfContentType,
			osip_content_type_t *sipContentType);

	/* Convert Invite */
	RTSipInvite convertOsipInvite2MCF(osip_message_t *invite, RTSipInvite mcfInvite);
	osip_message_t* convertMCF2OsipInvite(RCTSipInvite mcfInvite, osip_message_t *invite);

	/* Convert Info */
	RTSipInfo convertOsipInfo2MCF(osip_message_t * info, RTSipInfo mcfInfo);
	osip_message_t* convertMCF2OsipInfo(RCTSipInfo mcfMessage, osip_message_t *message);


	/*Convert Update*/
	RTSipUpdate convertOsipUpdate2MCF(osip_message_t * info, RTSipUpdate mcfUpdate);
	osip_message_t* convertMCF2OsipUpdate(RCTSipUpdate mcfMessage, osip_message_t *message);


	/*Convert Prack*/
	RTSipPrack convertOsipPrack2MCF(osip_message_t * info, RTSipPrack mcfPrack);
	osip_message_t* convertMCF2OsipPrack(RCTSipPrack mcfMessage, osip_message_t *message);

	/* Convert Message */
	RTSipMessage convertOsipMessage2MCF(osip_message_t *message, RTSipMessage mcfMessage);
	osip_message_t* convertMCF2OsipMessage(RCTSipMessage mcfMessage, osip_message_t *message);

	/* Convert BYE */
	RTSipBye convertOsipBye2MCF(osip_message_t *bye, RTSipBye mcfBye);
	osip_message_t* convertMCF2OsipBye(RCTSipBye mcfBye, osip_message_t* bye);

	/* Convert CANCEL */
	RTSipCancel convertOsipCancel2MCF(osip_message_t* cancel, RTSipCancel mcfCancel);
	osip_message_t* convertMCF2OsipCancel(RCTSipCancel mcfCancel, osip_message_t* cancel);
} // namespace ExosipTranslator

#endif
