/************************************************************************

 * Copyright (c)2010-2012, by BUPT
 * All rights reserved.

 * FileName：       exosipstack.C
 * System：         MCF
 * SubSystem：      PSA
 * Author：         Huang Haiping
 * Date：           2010.04.07
 * Version：        1.0
 * Description：
 控制eXosip2协议栈

 *
 * Last Modified:
 2010.04.07, 完成初始版本定义
 By Huang Haiping
 2010.07.20, fix sent out SIP message without route
 By Li Ling
 2010.09.09, fix several bug when building SIP message failed
 By Huang Haiping
 2010.09.15, fix not answering 200OK insubscription message
 By Huang Haiping

 *************************************************************************/

#include "pachook.h"
#include "msgconvertor.h"
#include "psa_sip_inf.h"
#include "sipenv.h"
#include "CSipUserManager.h"
#include "CTUniNetMsgHelper.h"
#include <stdio.h>
#include <sstream>
#include <time.h>
#include "MyLogger.h"
//#include <boost/thread/mutex.hpp>
//#include <log4cxx/mLogger.getLogger().h>
//#include <log4cxx/basicconfigurator.h>
//#include <log4cxx/propertyconfigurator.h>
//#include <log4cxx/helpers/exception.h>
//#include <log4cxx/xml/domconfigurator.h>
#include "exosipstack.h"

using namespace log4cxx;
using namespace log4cxx::xml;

static MyLogger& mLogger = MyLogger::getInstance("etc/log4cxx.xml",
		"SgFileAppender");

//log4cxx::LoggerPtr mLogger.getLogger();

set<string> m_name_pool;
map<string, timer *> m_map_timers;
map<string, UserData *> m_map_userdata; //save for heartbeat
timers_poll * ptimer_poll;

string proxy, icscf, _realmAddr, realmAddr;
//int count = 0;
static CHAR* __generateUniqueCallId(CHAR* dst, size_t size, RCTSipAddress from,
		RCTSipAddress to, RCTSipCallId callId) {
	// <from-sipuri><to-sipuri><callid>
	strncat(dst, from.url.username.c_str(), size);
	strncat(dst, to.url.username.c_str(), size);
	strncat(dst, CSipMsgHelper::toString(callId).c_str(), size);

	return dst;
}

static CHAR* __generateUniqueDialogId(CHAR* dst, size_t size,
		RCTSipAddress from, RCTSipAddress to, RCTSipCallId callId) {
	// <from-tag><to-tag><callid>
	//	strncat(dst, from.tag.c_str(), size);
	//	strncat(dst, to.tag.c_str(), size);
	//	strncat(dst, CSipMsgHelper::toString(callId).c_str(), size);
	strncat(dst, from.url.username.c_str(), size);
	strncat(dst, to.url.username.c_str(), size);
	strncat(dst, CSipMsgHelper::toString(callId).c_str(), size);
	return dst;
}

CExosipStack::CExosipStack(INT psaid) :
	m_psaid(psaid) {

}

CExosipStack::~CExosipStack() {

	//LOG4CXX_INFO(mLogger.getLogger(), "call CExosipStack distructure")

	delete ptimer_poll;
}

BOOL CExosipStack::init(USHORT port) {

	if (eXosip_init() != 0) {
		// 初始化失败
		LOG4CXX_ERROR(mLogger.getLogger(), "eXosip init failed");

		return FALSE;
	}
	int i = 1;
	eXosip_set_option(EXOSIP_OPT_DONT_SEND_101, &i);
	// 打开监听端口
	if (eXosip_listen_addr(IPPROTO_UDP, NULL, port, AF_INET, FALSE) != 0) {
		LOG4CXX_ERROR(mLogger.getLogger(), "eXosip listen on port "<<port<<" failed");
		eXosip_quit();
		return FALSE;
	}
	LOG4CXX_INFO(mLogger.getLogger(), "eXosip listening on port "<<port);

	CProperties* properties = CPropertiesManager::getInstance()->getProperties(
			"gateway.env");
	int accessMode = properties->getIntProperty("accessMode");

	if (accessMode == -1) {
		LOG4CXX_INFO(mLogger.getLogger(), "AccessMode not set in mcf.env, Use DEFAULT 0");
		accessMode = 0;
	} else {
		this->accessMode = accessMode;
	}

	confType = properties->getProperty("confType");
	if (confType != "") {
		confServer = "sip:" + properties->getProperty("confServer");
		if (confServer == "") {
			confServer = "sip:10.109.247.115:5060";
		}
		confRealm = properties->getIntProperty("confRealm");
		if (confRealm == "") {
			confRealm = "conf.com";
		}
	}

	if (accessMode == 2) {
		ptimer_poll = new timers_poll(128);
		thread_id = 0;

		if (pthread_create(&thread_id, NULL, thread_fun, (void *) ptimer_poll)
				!= 0) {
			LOG4CXX_ERROR(mLogger.getLogger(), "Create timer thread failed");
			return FALSE;
		}
	}

	if (1 == accessMode) {
		proxy = properties->getProperty("proxy");
		proxy = "sip:" + proxy;
		CSipUserManager::setAllRegistered(true);
	} else if (2 == accessMode) {
		proxy = properties->getProperty("proxy");
		proxy = "sip:" + proxy;

		_realmAddr = properties->getProperty("realmAddr");
		realmAddr = "sip:" + _realmAddr;

		CSipUserManager::init();
		timer * ptimer = new timer(1, getSipUserFromDB, NULL, 1);
		ptimer_poll->timers_poll_add_timer(ptimer);

	} else {
		icscf = properties->getProperty("icscf");
		icscf = "sip:" + icscf;
	}

	return TRUE;
}

void * CExosipStack::thread_fun(void *data) {
	timers_poll *my_timers = (timers_poll *) data;
	my_timers->run();

	return NULL;
}

string CExosipStack::generateRand() {
	srand((unsigned int) time(NULL));
	unsigned long int ra = rand();
	stringstream ss;
	ss << ra;
	string ran = ss.str();

	MD5 md5;
	md5.init();
	md5.UpdateMd5(ran.c_str(), ran.length());
	md5.Finalize();
	return md5.GetMd5().substr(0, 16);
}

int CExosipStack::sendInitRegister(timer * ptimer) {
	//ptimer->timer_modify_internal(10.0);
	ptimer->timer_modify_internal(0);

	UserData * ud = (UserData *) ptimer->timer_get_userdata();

	string name = ud->username;
	name.insert(0, "sip:");

	int reg_id = ud->reg_id;
	osip_message_t *reg = NULL;
	osip_message_init(&reg);

	eXosip_lock ();
	if (-1 == reg_id) {

		reg_id = eXosip_register_build_initial_register(name.c_str(),
				realmAddr.c_str(), NULL, 3600, &reg);

		if (reg_id < 0) {
			eXosip_unlock ();
			//printf("reg_id<0\n");
			return -1;
		}

		ud->reg_id = reg_id;

		osip_route_t *rt;
		osip_route_init(&rt);
		char * response = new char[128];

		if (osip_route_parse(rt, proxy.c_str()) != 0) {
			LOG4CXX_ERROR(mLogger.getLogger(), "proxy not correctly set");
			return 0;
		} else {
			osip_uri_uparam_add(rt->url,osip_strdup("lr"),NULL);
			osip_route_to_str(rt, &response);
		}

		osip_route_free(rt);

		osip_message_set_route(reg, response);
		delete[] response;
		response = NULL;

		osip_message_set_supported(reg, "100rel");
		osip_contact_t *contact_header;
		osip_message_get_contact(reg, 0, &contact_header);
		char * buf;
		osip_contact_to_str(contact_header, &buf);
		string contactStr = buf;
		size_t pos = contactStr.find(";");
		//contactStr.substr(1, pos);
		string str = contactStr.substr(1, pos - 1);
		osip_uri_t* url = osip_contact_get_url(contact_header);
		//osip_uri_free(url);
		//osip_uri_parse(url, str.c_str());
		osip_uri_param_freelist(&url->url_params);
		osip_contact_param_add(contact_header, strdup("expires"), strdup("3600"));
		osip_contact_param_add(contact_header, strdup("+g.oma.sip-im"), NULL);
		osip_contact_param_add(contact_header, strdup("+g.oma.sip-im.large-message"), NULL);

		osip_contact_to_str(contact_header, &buf);
	} else {
		eXosip_register_build_register(reg_id, 1800, &reg);
	}

	//	eXosip_unlock();

	//eXosip_lock();
	char * buf = NULL;
	size_t len;
	osip_message_to_str(reg, &buf, &len);
	LOG4CXX_DEBUG(mLogger.getLogger(), "SIPPSA send register:\n"<<buf);
	osip_free(buf);
	if (eXosip_register_send_register(reg_id, reg) != 0) {
		eXosip_unlock();
		LOG4CXX_ERROR(mLogger.getLogger(), "send register failed");
		return -1;
	}

	eXosip_unlock();

	return 1;
}

int CExosipStack::getSipUserFromDB(timer *ptimer) {

	set<string> name_pool;
	CSipUserManager::getSipUser(name_pool);
	//printf("username unregistered: %d\n", name_pool.size());
	double interval = 1;
	double starttime = 0;

	for (set<string>::iterator iter = m_name_pool.begin(); iter
			!= m_name_pool.end(); ++iter) {
		if (name_pool.find(*iter) == name_pool.end()) {
			string uri = (*iter);
			uri.insert(0, "sip:");
			m_name_pool.erase(iter);
			timer * ptimer = m_map_timers[uri];
			ptimer_poll->timers_poll_del_timer(ptimer);
			//ptimer->timer_stop();
		}
	}

	for (set<string>::iterator iter = name_pool.begin(); iter
			!= name_pool.end(); ++iter) {
		if (m_name_pool.find(*iter) == m_name_pool.end()) {
			m_name_pool.insert(*iter);
			starttime += interval;
			UserData * ud = new UserData;
			ud->username = (*iter);
			ud->reg_id = -1;
			ud->noncecount = 1;
			ud->cnonce = generateRand();
			string uri = (*iter);
			uri.insert(0, "sip:");
			m_map_userdata.insert(make_pair<string, UserData *> (uri, ud));
			timer * ptimer = new timer(starttime, sendInitRegister, ud, 1);
			m_map_timers.insert(make_pair<string, timer *> (uri, ptimer));
			ptimer_poll->timers_poll_add_timer(ptimer);
		}
	}
	ptimer->timer_modify_internal(10);

	return 0;
}

void CExosipStack::register_process_401(eXosip_event_t * je) {
	string name;

	name.append(osip_from_get_url(je->response->from)->scheme);
	name.append(":");
	name.append(osip_from_get_url(je->response->from)->username);
	name.append("@");
	name.append(osip_from_get_url(je->response->from)->host);
	UserData * ud = m_map_userdata[name];

	string cnonce = ud->cnonce;
	char * nc = new char[10];
	snprintf(nc, 9, "%.8x", ud->noncecount);
	string noncecount(nc);

	osip_www_authenticate_t * www_header;
	osip_message_get_www_authenticate(je->response, 0, &www_header);

	char * opaque64 = osip_www_authenticate_get_opaque(www_header);
	char * nonce64 = osip_www_authenticate_get_nonce(www_header);
	char * nonce64_no_quotes = remove_quotes(nonce64);

	string nonce = nonce64_no_quotes;

	string username;
	username.append(osip_from_get_url(je->response->from)->username);
	username.append("@");
	username.append(osip_from_get_url(je->response->from)->host);

	string password = CSipUserManager::getSipPassword(username);

	string realm = osip_from_get_url(je->response->from)->host;

	//aka_version = 1;
	char * alg;
	alg = osip_www_authenticate_get_algorithm(www_header);

	if (strcmp(alg, "MD5") != 0 /* && strcmp(alg, "AKAv1-MD5") != 0 && strcmp(alg, "AKAv2-MD5") != 0*/) {
		LOG4CXX_ERROR(mLogger.getLogger(), "Authentication scheme "<<alg<< " not supported");
		return;
	}
	string algorithm = alg;

	MD5Digest md5Digest(password, username, realm, nonce, cnonce, noncecount,
			algorithm);
	string response = md5Digest.calcResponse();

	LOG4CXX_DEBUG(mLogger.getLogger(), "password:"<<password<<", username:"<<username<<", realm:"<<realm<<",\n"
			<<"nonce:"<<nonce<<", cnonce: "<<cnonce<<", noncecount:"<<noncecount<<
			",\nalgorithm:"<<algorithm<<", response:"<<response);

	osip_message_t * reg2 = NULL;
	int expires = 100;

	map<string, UserData *>::iterator iter = m_map_userdata.find(name);
	if (iter == m_map_userdata.end()) {
		return;
	}

	int reg_id = iter->second->reg_id;

	eXosip_lock();
	int r = eXosip_register_build_register(reg_id, expires, &reg2);
	eXosip_unlock();

	if (r < 0) {
		LOG4CXX_ERROR(mLogger.getLogger(), "process 401: build register failed");
		return;
	}

	osip_header_t *expires_header;
	osip_message_get_expires(reg2, 0, &expires_header);
	char reg_expires_str[10];
	sprintf(reg_expires_str, "%d", expires);

	if (!strcmp(expires_header->hvalue, "100")) {
		osip_header_set_value(expires_header, osip_strdup(reg_expires_str));
	}

	osip_authorization_t *auth_header;
	osip_authorization_init(&auth_header);

	osip_authorization_set_auth_type(auth_header, "Digest");
	osip_authorization_set_realm(auth_header, add_quotes(realm));

	osip_authorization_set_algorithm(auth_header, alg);

	osip_authorization_set_message_qop(auth_header, "auth");

	osip_authorization_set_nonce(auth_header, add_quotes(nonce));
	osip_authorization_set_opaque(auth_header, opaque64);
	osip_authorization_set_username(auth_header, add_quotes(username));

	osip_authorization_set_uri(auth_header, add_quotes(string("sip:") + realm));
	osip_authorization_set_response(auth_header, add_quotes(response));
	osip_authorization_set_cnonce(auth_header, add_quotes(cnonce));
	osip_authorization_set_nonce_count(auth_header, nc);

	char * h_value;
	osip_authorization_to_str(auth_header, &h_value);

	if (osip_message_set_authorization(reg2, h_value) != 0)
		LOG4CXX_ERROR(mLogger.getLogger(), "Cannot set authorization\n");

	eXosip_lock();
	int i = eXosip_register_send_register(reg_id, reg2);
	eXosip_unlock();

	if (i != 0) {
		LOG4CXX_ERROR(mLogger.getLogger(), "Error sending REGISTER\n");
		return;
	}

	return;
}

void CExosipStack::register_process_200(eXosip_event_t * je) {
	string username;
	username.append(osip_from_get_url(je->response->from)->username);
	username.append("@");
	username.append(osip_from_get_url(je->response->from)->host);

	CSipUserManager::setRegistered(username, true);

	// get list of service routes from 200 OK response
	osip_header_t *service_route;
	int num_routes = 0;
	int num_service_routes = 0;
	vector<char *> ims_service_route;
	while (osip_message_header_get_byname(je->response, "Service-Route",
			num_routes, &service_route) >= 0) {
		if ((num_service_routes == 0)
				|| (strcmp(osip_header_get_value(service_route),
						ims_service_route[num_service_routes - 1]) != 0)) {

			ims_service_route.push_back(osip_header_get_value(service_route));
			++num_service_routes;
		}
		++num_routes;
	}

	int expires = 1800;
	osip_contact_t * contact;

	if (osip_message_get_contact(je->response, 0, &contact) >= 0) {
		osip_generic_param_t * params;
		if (osip_contact_param_get_byname(contact, "Expires", &params) >= 0) {
			expires = atoi(params->gvalue);
			expires = expires >> 1;
		}
	}

	osip_header_t * exp;
	if (osip_message_get_expires(je->response, 0, &exp) >= 0) {
		expires = atoi(exp->hvalue);
		expires = expires >> 1;
	}

	string uri = username;
	uri.insert(0, "sip:");

	if (!ims_service_route.empty()) {
		m_service_route.insert(make_pair<string, vector<char *> > (uri,
				ims_service_route));
	}

	timer * ptimer = m_map_timers[uri];
	ptimer->timer_modify_internal(expires);

	return;
}

char * CExosipStack::remove_quotes(char * text) {
	char * rep = strtok(text, "\"");
	return osip_strdup(rep);
}

char * CExosipStack::add_quotes(string text) {
	text.insert(0, "\"", 1);
	text.append("\"");

	char * res = new char[text.length() + 1];
	strcpy(res, text.c_str());
	return res;
}

void CExosipStack::doActive(void) {
	BOOL parsed = FALSE; // 标记消息是否被处理


	eXosip_event_t* event = eXosip_event_wait(0, 0);
	if (NULL == event)
		return;

	LOG4CXX_DEBUG(mLogger.getLogger(), "eXosip receive message type:"<<event->type);
	LOG4CXX_DEBUG(mLogger.getLogger(), "eXosip textinfo: "<<event->textinfo);
	LOG4CXX_DEBUG(mLogger.getLogger(), "eXosip tid: "<<event->tid);
	LOG4CXX_DEBUG(mLogger.getLogger(), "eXosip cid: "<<event->cid);
	//LOG4CXX_DEBUG(mLogger.getLogger(), "eXosip ACK: %d\n", (int) event->ack);

	if (event->request == NULL && event->response == NULL) {
		return;
	}
	char * buf = NULL;
	size_t len;
	if (event->request != NULL) {
		osip_message_to_str(event->request, &buf, &len);
		LOG4CXX_DEBUG(mLogger.getLogger(), "eXosip request:\n"<<buf);
		osip_free(buf);
		buf = NULL;
	}

	if (event->response != NULL) {
		osip_message_to_str(event->response, &buf, &len);
		LOG4CXX_DEBUG(mLogger.getLogger(), "eXosip response:\n"<<buf);
		osip_free(buf);
		buf = NULL;
	}
	// Set Ctrl Msg

	PTSipCtrlMsg pCtrlMsg = new TSipCtrlMsg();
	PTUniNetMsg pMsg = new TUniNetMsg();

	pMsg->msgType = SIP_TYPE;

	pMsg->oAddr.logAddr = m_psaid;
	pMsg->tAddr.logAddr = CSipEnv::instance()->DIS_LOG_ADDR;

	pMsg->dialogType = DIALOG_BEGIN;

	// Build header
	pMsg->ctrlMsgHdr = pCtrlMsg;
	pMsg->setCtrlMsgHdr();

	if (event->request && MSG_IS_REGISTER(event->request)) {
		if (event->response == NULL) {
			// a request message

			parsed = TRUE;

			osip_message_t* ans = NULL;
			int ret;

			eXosip_lock();
			ret = eXosip_message_build_answer(event->tid, 501, &ans);
			if (OSIP_SUCCESS != ret) {
				LOG4CXX_ERROR(mLogger.getLogger(),
						"eXosip_message_build_answer error for register!!!\n");
			} else {

				ans->reason_phrase = osip_strdup("Not Implemented");
				ret = eXosip_message_send_answer(event->tid, 501, ans);
				if (OSIP_SUCCESS != ret) {
					LOG4CXX_ERROR(mLogger.getLogger(), "error send answer for register!!!\n");
				}
			}eXosip_unlock();
			delete pMsg;

			eXosip_event_free(event);

			return;

		}
	} else if (event->request && MSG_IS_OPTIONS(event->request)) {
		//OPTIONS message
		if (event->response == NULL) {
			parsed = TRUE;
			//	osip_message_t * opt = event->request;

			osip_message_t * ans;
			eXosip_options_build_answer(event->tid, 200, &ans);

			osip_message_set_allow(ans,
					"ACK, BYE, CANCEL, INFO, INVITE, MESSAGE, OPTIONS, PRACK, REGISTER, UPDATE");
			osip_message_set_accept(ans, "application/sdp");
			osip_message_set_accept_language(ans, "da, en-gb; q= 0.8, en;q=0.7");
			eXosip_options_send_answer(event->tid, 200, ans);
		}
		return;
	} else {
		switch (event->type) {
		case EXOSIP_REGISTRATION_FAILURE: {
			LOG4CXX_DEBUG(mLogger.getLogger(), "Register failure");
			break;
		}
		case EXOSIP_REGISTRATION_SUCCESS: {
			LOG4CXX_DEBUG(mLogger.getLogger(), "Register success");
			break;
		}
		case EXOSIP_CALL_INVITE: {
			parsed = TRUE;

			osip_message_t *invite = event->request;
			PTSipInvite mcfInvite = new TSipInvite();

			ExosipTranslator::convertOsipInvite2MCF(invite, *mcfInvite);

			pMsg->msgName = SIP_INVITE;
			pMsg->msgBody = mcfInvite;
			pMsg->setMsgBody();

			ExosipTranslator::convertCtrlMsg2MCF(event->request, pCtrlMsg);

			// add to tag to invite since upper app can't control eXosip of outgoing msg
			// retrive it from 101 response generated by eXosip
			ExosipTranslator::convertOsipTo2MCF(event->response->to, pCtrlMsg->to);

			this->m_map_branch_tid.put(pCtrlMsg->via.branch.c_str(), event->tid);

			this->storeCid(pCtrlMsg->from, pCtrlMsg->to, pCtrlMsg->sip_callId,
					event->cid);

			osip_message_t * ans = NULL;
			eXosip_lock();
			eXosip_call_build_answer(event->tid, 180, &ans);
			eXosip_call_send_answer(event->tid, 180, ans);
			eXosip_unlock();

			//char * buf = NULL;
			//size_t len;
			//osip_message_to_str(ans, &buf, &len);

			break;
		}
		case EXOSIP_CALL_REINVITE: {
			parsed = TRUE;

			osip_message_t *invite = event->request;
			PTSipInvite mcfInvite = new TSipInvite();
			ExosipTranslator::convertOsipInvite2MCF(invite, *mcfInvite);

			pMsg->msgName = SIP_INVITE;
			pMsg->msgBody = mcfInvite;
			pMsg->setMsgBody();

			ExosipTranslator::convertCtrlMsg2MCF(event->request, pCtrlMsg);
			// add to tag to invite since upper app can't control eXosip of outgoing msg
			// retrive it from 101 response generated by eXosip
			// re-invite msg have to-tag, no need to attach
			//			ExosipTranslator::convertOsipTo2MCF(event->response->to, pCtrlMsg->to);

			// needed, branch changed
			this->m_map_branch_tid.put(pCtrlMsg->via.branch.c_str(), event->tid);

			//			this->storeCid(pCtrlMsg->from, pCtrlMsg->to, pCtrlMsg->sip_callId,
			//					event->cid);
			break;
		}
		case EXOSIP_CALL_PROCEEDING: {
			//			if (100 == event->response->status_code)
			//			{
			//				// store call id for outgoing invite
			//				this->storeCid(pCtrlMsg->from, pCtrlMsg->to, pCtrlMsg->sip_callId,
			//						event->cid);
			//			}
			//			else if (101 == event->response->status_code)
			//			{
			//				// Dialog established
			//				this->storeDid(pCtrlMsg->from, pCtrlMsg->to, pCtrlMsg->sip_callId,
			//									event->did);
			//			}
			break;
		}
		case EXOSIP_CALL_ANSWERED: {
			parsed = TRUE;

			osip_message_t *resp = event->response;

			PTSipResp mcfResp = new TSipResp();

			ExosipTranslator::convertOsipResp2MCF(resp, *mcfResp);

			pMsg->msgName = SIP_RESPONSE;
			pMsg->msgBody = mcfResp;
			pMsg->setMsgBody();

			ExosipTranslator::convertCtrlMsg2MCF(event->response, pCtrlMsg);

			this->storeCid(pCtrlMsg->from, pCtrlMsg->to, pCtrlMsg->sip_callId,
					event->cid);
			this->storeDid(pCtrlMsg->from, pCtrlMsg->to, pCtrlMsg->sip_callId,
					event->did);

			break;
		}
		case EXOSIP_CALL_ACK: {
			parsed = TRUE;

			osip_message_t *ack = event->ack;

			PTSipReq mcfAck = new TSipReq();

			ExosipTranslator::convertOsipReq2MCF(ack, *mcfAck);

			pMsg->msgName = SIP_ACK;
			pMsg->msgBody = mcfAck;
			pMsg->setMsgBody();

			ExosipTranslator::convertCtrlMsg2MCF(event->ack, pCtrlMsg);

			this->storeDid(pCtrlMsg->from, pCtrlMsg->to, pCtrlMsg->sip_callId,
					event->did);

			break;
		}
		case EXOSIP_CALL_MESSAGE_NEW: {
			if (!strcmp(event->request->sip_method, "BYE")) {
				// It will be handled in EXOSIP_CALL_CLOSED
				parsed = TRUE;

				delete pMsg;

				eXosip_event_free(event);

				return;
			} else if (!strcmp(event->request->sip_method, "NOTIFY")) {
				// It will be handled in EXOSIP_CALL_CLOSED
				parsed = TRUE;

				delete pMsg;

				DEBUG0(m_psaid, "eXosip event free!");
				eXosip_event_free(event);

				return;
			} else if (!strcmp(event->request->sip_method, "INFO")) {
				parsed = TRUE;

				osip_message_t *info = event->request;

				PTSipInfo mcfInfo = new TSipInfo();

				ExosipTranslator::convertOsipInfo2MCF(info, *mcfInfo);

				pMsg->msgName = SIP_INFO;
				pMsg->msgBody = mcfInfo;
				pMsg->setMsgBody();
				ExosipTranslator::convertCtrlMsg2MCF(info, pCtrlMsg);
				this->m_map_branch_tid.put(pCtrlMsg->via.branch.c_str(),
						event->tid);
				this->storeDid(pCtrlMsg->from, pCtrlMsg->to,
						pCtrlMsg->sip_callId, event->did);

				break;
			} else if (!strcmp(event->request->sip_method, "UPDATE")) {
				parsed = TRUE;
				osip_message_t * update = event->request;
				PTSipUpdate mcfUpdate = new TSipUpdate();
				ExosipTranslator::convertOsipUpdate2MCF(update, *mcfUpdate);

				pMsg->msgName = SIP_UPDATE;
				pMsg->msgBody = mcfUpdate;

				pMsg->setMsgBody();
				ExosipTranslator::convertCtrlMsg2MCF(update, pCtrlMsg);
				this->m_map_branch_tid.put(pCtrlMsg->via.branch.c_str(),
						event->tid);
				this->storeDid(pCtrlMsg->from, pCtrlMsg->to,
						pCtrlMsg->sip_callId, event->did);
				break;

			}

		}
		case EXOSIP_CALL_RELEASED: {
			parsed = TRUE;

			delete pMsg;

			eXosip_event_free(event);

			return;
		}
		case EXOSIP_CALL_CANCELLED: {

			PTSipCancel mcfCancel = new TSipCancel();

			ExosipTranslator::convertOsipCancel2MCF(event->request, *mcfCancel);
			ExosipTranslator::convertCtrlMsg2MCF(event->response, pCtrlMsg);
			pCtrlMsg->cseq_method = "CANCEL";
			pMsg->msgName = SIP_CANCEL;
			pMsg->msgBody = mcfCancel;
			pMsg->setMsgBody();

		}
		case EXOSIP_CALL_CLOSED: {
			// Receive BYE or CANCEL
			parsed = TRUE;
			ExosipTranslator::convertCtrlMsg2MCF(event->request, pCtrlMsg);

			osip_cseq_t* cseq = osip_message_get_cseq(event->request);
			if (NULL != event->response && 487 != event->response->status_code
					&& strcmp("BYE", cseq->method)) {
				// A non-487 is sent out! by psa, ACK is received
				PTSipReq declineAck = new TSipReq();
				declineAck->req_uri = pCtrlMsg->to.url;
				pMsg->msgName = SIP_ACK;
				pMsg->msgBody = declineAck;
				pMsg->setMsgBody();

				ExosipTranslator::convertCtrlMsg2MCF(event->response, pCtrlMsg);
				// Change Cseq Method to ACK
				pCtrlMsg->cseq_method = "ACK";
				// TODO: uncorect Cseq number
			} else if (NULL != event->response && 487
					== event->response->status_code) {

				osip_message_t *resp = event->response;

				PTSipResp mcfResp = new TSipResp();

				ExosipTranslator::convertOsipResp2MCF(resp, *mcfResp);

				pMsg->msgName = SIP_RESPONSE;
				pMsg->msgBody = mcfResp;
				pMsg->setMsgBody();

				ExosipTranslator::convertCtrlMsg2MCF(event->response, pCtrlMsg);

			}
			//			else if (!strcmp(event->request->sip_method, "CANCEL"))
			//			{
			// CANCEL is received
			//				printf("received CANCEL\n");

			//				PTSipCancel mcfCancel = new TSipCancel();

			//				ExosipTranslator::convertOsipCancel2MCF(event->request, *mcfCancel);
			//				ExosipTranslator::convertCtrlMsg2MCF(event->response, pCtrlMsg);
			//				pCtrlMsg->cseq_method = "CANCEL";
			//				pMsg->msgName = SIP_CANCEL;
			//				pMsg->msgBody = mcfCancel;
			//				pMsg->setMsgBody();
			//			}
			else {
				// BYE received
				PTSipBye mcfBye = new TSipBye();

				ExosipTranslator::convertOsipBye2MCF(event->request, *mcfBye);
				pMsg->msgName = SIP_BYE;
				pMsg->msgBody = mcfBye;
				pMsg->setMsgBody();
			}

			this->removeCid(pCtrlMsg->from, pCtrlMsg->to, pCtrlMsg->sip_callId);
			this->removeDid(pCtrlMsg->from, pCtrlMsg->to, pCtrlMsg->sip_callId);

			// in case the callee send bye
			this->removeCid(pCtrlMsg->to, pCtrlMsg->from, pCtrlMsg->sip_callId);
			this->removeDid(pCtrlMsg->to, pCtrlMsg->from, pCtrlMsg->sip_callId);
			break;
		}
		case EXOSIP_MESSAGE_NEW: {
			parsed = TRUE;

			osip_message_t *msg = event->request;

			if (!strcmp(event->request->sip_method, "NOTIFY")) {
				osip_message_t * ans = NULL;
				eXosip_lock();
				eXosip_message_build_answer(event->tid, 200, &ans);
				eXosip_message_send_answer(event->tid, 200, ans);
				delete pMsg;
				eXosip_unlock();
				pMsg = NULL;
				return;

			} else {

				PTSipMessage mcfMsg = new TSipMessage();

				ExosipTranslator::convertOsipMessage2MCF(msg, *mcfMsg);
				ExosipTranslator::convertOsipContentType2MCF(msg->content_type,
						mcfMsg->content_type);

				pMsg->msgName = SIP_MESSAGE;
				pMsg->msgBody = mcfMsg;
				pMsg->setMsgBody();

				ExosipTranslator::convertCtrlMsg2MCF(event->request, pCtrlMsg);

				this->m_map_branch_tid.put(pCtrlMsg->via.branch.c_str(),
						event->tid);
			}
			break;
		}

		case EXOSIP_IN_SUBSCRIPTION_NEW: {
			// TODO: WARNING end subscription message here. ERRRRRRRRRRRRRR
			parsed = TRUE;
			eXosip_lock();
			osip_message_t* ans = NULL;
			eXosip_insubscription_build_answer(event->tid, 200, &ans);
			eXosip_insubscription_send_answer(event->tid, 200, ans);
			eXosip_unlock();
			break;
		}
		default:
			psaPrint(m_psaid, "UNHANDLED exosip event: %d\n", event->type);
			break;
		} // switch event->type
	} // is REGISTER or not

	if (!parsed) {
		if (event->response && MSG_IS_RESPONSE(event->response)) {
			parsed = TRUE;
			if (0 == strcmp(event->response->cseq->method, "REGISTER")) {
				delete pMsg;
				pMsg = NULL;
				if (2 == accessMode) {
					if (200 == event->response->status_code) {
						// REGISTER response 200OK, clear rid map
						register_process_200(event);
						return;
					} else if (401 == event->response->status_code) {
						register_process_401(event);
						return;
					} else {
						return;
					}
				} else {
					//不处理注册消息
					return;
				}
			}

			if (0 == strcmp(event->response->cseq->method, "BYE")) {
				delete pMsg;
				pMsg = NULL;
				LOG4CXX_DEBUG(mLogger.getLogger(), "Receive 200 OK for BYE");
				return;

			}
			//		if (0 == strcmp(event->response->cseq->method, "PRACK")) {
			//			return;
			//		}

			osip_message_t *resp = event->response;

			PTSipResp mcfResp = new TSipResp();

			ExosipTranslator::convertOsipResp2MCF(resp, *mcfResp);

			pMsg->msgName = SIP_RESPONSE;
			pMsg->msgBody = mcfResp;
			pMsg->setMsgBody();

			ExosipTranslator::convertCtrlMsg2MCF(event->response, pCtrlMsg);

			if (MSG_IS_STATUS_1XX(event->response)) {
				// keep cid here
				this->storeCid(pCtrlMsg->from, pCtrlMsg->to,
						pCtrlMsg->sip_callId, event->cid);

				this->m_map_branch_tid.put(pCtrlMsg->via.branch.c_str(),
						event->tid);
			}

			//100 trying event did = 0
			if (100 < event->response->status_code && 199
					>= event->response->status_code) {

				this->storeDid(pCtrlMsg->from, pCtrlMsg->to,
						pCtrlMsg->sip_callId, event->did);
			}
			//osip_header_t * requireHd/*, * supportedHd*/;
			/*	if(osip_message_get_supported(event->response, 0, &supportedHd) >= 0){
			 string supportedStr = supportedHd->hvalue;
			 printf("supportedStr %s\n", supportedStr.c_str());
			 if(supportedStr.find("100rel") != string::npos){
			 osip_message_t *prack;
			 eXosip_call_build_prack(event->tid, &prack);
			 eXosip_call_send_prack(event->tid, prack);
			 }
			 }
			 else*/
			//  if (osip_message_get_require(event->response, 0, &requireHd)
			// 		>= 0) {
			// 	string requireStr = requireHd->hvalue;
			// 	if (requireStr.find("100rel") != string::npos) {
			// 		osip_message_t *prack;
			// 		eXosip_call_build_prack(event->tid, &prack);
			// 		eXosip_call_send_prack(event->tid, prack);

			// 	}
			// }
			//}
		}
	} // if not parsed

	if (!parsed) {
		delete pMsg;
		pMsg = NULL;
		LOG4CXX_DEBUG(mLogger.getLogger(), "Unkown sip message type");

		eXosip_event_free(event);

		return;
	}

	PTMsg mcfMsg = new TMsg();
	// Send out message
	CMsgConvertor::convertMsg(pMsg, mcfMsg);
	sendMsgToPACM(mcfMsg);

	eXosip_event_free(event);
}

BOOL CExosipStack::doSendMsg(PTMsg msg) {
	PTUniNetMsg uniMsg = (PTUniNetMsg) msg->pMsgPara;
	BOOL flag = FALSE;

	if (NULL == uniMsg) {
		return flag;
	}

	switch (uniMsg->msgName) {
	case SIP_RESPONSE: {
		flag = this->onSend_SIP_RESPONSE(uniMsg);
		break;
	} // case SIP_RESPONSE
	case SIP_ACK: {
		flag = this->onSend_SIP_ACK(uniMsg);
		break;
	} // case SIP_ACK
	case SIP_BYE: {
		flag = this->onSend_SIP_BYE(uniMsg);
		break;
	} // SIP_BYE
	case SIP_INVITE: {
		//CTUniNetMsgHelper::print(uniMsg);
		flag = this->onSend_SIP_INVITE(uniMsg);
		break;
	} // SIP_INVITE
	case SIP_MESSAGE: {
		flag = this->onSend_SIP_MESSAGE(uniMsg);
		break;
	}
	case SIP_CANCEL: {
		flag = this->onSend_SIP_CANCEL(uniMsg);
		break;
	}
	case SIP_REGISTER: {
		flag = this->onSend_SIP_REGISTER(uniMsg);
		break;
	}
	case SIP_INFO: {
		flag = this->onSend_SIP_INFO(uniMsg);
		break;
	}
	case SIP_PRACK: {
		flag = this->onSend_SIP_PRACK(uniMsg);
		break;
	}
	case SIP_UPDATE: {
		flag = this->onSend_SIP_UPDATE(uniMsg);
		break;
	}
	default:
		break;
	}

	return flag;
}

BOOL CExosipStack::onSend_SIP_RESPONSE(PCTUniNetMsg uniMsg) {
	LOG4CXX_DEBUG(mLogger.getLogger(), "OnSend_SIP_RESPONSE");
	PTSipResp mcfRes = (PTSipResp) uniMsg->msgBody;
	PTSipCtrlMsg pCtrlMsg = (PTSipCtrlMsg) uniMsg->ctrlMsgHdr;

	LOG4CXX_DEBUG(mLogger.getLogger(), "Send out SIP RESPONSE: status ="<<mcfRes->statusCode);

	if (!strcmp(pCtrlMsg->cseq_method.c_str(), "INVITE") || !strcmp(
			pCtrlMsg->cseq_method.c_str(), "UPDATE")) {
		if (100 == mcfRes->statusCode) {
			// 100 trying is sent by eXosip automatically
			return TRUE;
		} else if (603 == mcfRes->statusCode) {
			// Decline
			//return onSend_SIP_BYE(uniMsg);
		}

		INT tid = -1;
		if (this->m_map_branch_tid.get(pCtrlMsg->via.branch.c_str(), tid)) {
			osip_message_t *ans = NULL;

			int ret;

			eXosip_lock();
			ret = eXosip_call_build_answer(tid, mcfRes->statusCode, &ans);
			if (OSIP_SUCCESS != ret) {
				LOG4CXX_ERROR(mLogger.getLogger(), "eXosip can't build answer:"<<ret);
			} else {
				// HHP: modified by Huang Haiping 2010-09-09 fix crash when build failed
				ExosipTranslator::convertMCF2OsipResp(*mcfRes, ans);

				string toStr = CSipMsgHelper::toString(pCtrlMsg->to).c_str();
				string fromStr =
						CSipMsgHelper::toString(pCtrlMsg->from).c_str();
				if (toStr.find("msml") != string::npos || fromStr.find("msml")
						!= string::npos) {
				} else if (accessMode == 0) {
					osip_route_t *rt;
					osip_route_init(&rt);
					string icscfAddr = (string) "<" + icscf + ";lr>";
					osip_message_set_route(ans, icscfAddr.c_str());
				} else {
					osip_route_t *rt;
					osip_route_init(&rt);
					string proxyAddr = (string) "<" + proxy + ";lr>";
					//		osip_message_set_route(ans, proxyAddr.c_str());
				}

				osip_message_set_allow(ans,
						"ACK,BYE,CANCEL,INFO,INVITE,MESSAGE,OPTIONS,PRACK,REGISTER,UPDATE");
				//osip_message_set_server(ans, "Server/1.0.3141M");
				//osip_message_set_require(ans, "timer");
				//osip_message_set_header(ans, "Min-SE", "600");
				//osip_message_set_header(ans, "Session-Expires", "600;refresher=uac");
				//	osip_message_set_header(ans, "P-Prefered-Identity", "<+8676926991030@gd.ctcims.cn>");
				osip_message_set_supported(ans, "100rel,join,timer");
				//osip_message_set_header(ans, "Max-Forwards", "70");	

				//osip_via_t * vias;
				//osip_message_get_via(ans, 0, &vias);
				//osip_via_param_add(vias, "received", "113.98.13.96");


				//ExosipTranslator::convertMCF2CtrlMsg(pCtrlMsg, ans);

				if (ans->to) {
					osip_to_t *to;
					osip_to_init(&to);
					osip_to_set_displayname(to, ans->to->displayname);

					osip_to_set_url(to, ans->to->url);

					if (pCtrlMsg->to.tag.length() != 0) {
						osip_to_set_tag(to, osip_strdup(pCtrlMsg->to.tag.c_str()));
					}

					ans->to = to;
				}
				//				osip_message_set_content_length(ans, "183");
				//	osip_content_length_t * pLen = osip_message_get_content_length(ans);
				//	osip_content_length_free(pLen);
				//	osip_content_length_init(&pLen);
				//	osip_message_set_content_length(ans, "183");
				char * buf;
				//	osip_content_length_to_str(pLen, &buf);
				//	printf("content-length: %s\n", buf);
				size_t len;
				osip_message_to_str(ans, &buf, &len);

				LOG4CXX_DEBUG(mLogger.getLogger(), "SIPPSA send answer"<<buf);
				osip_free(buf);
				buf = NULL;

				ret = eXosip_call_send_answer(tid, mcfRes->statusCode, ans);
				if (OSIP_SUCCESS != ret) {
					LOG4CXX_ERROR(mLogger.getLogger(),
							"eXosip_call_send_answer "<<mcfRes->statusCode<<" failed: "<<ret);
				} else {
					LOG4CXX_DEBUG(mLogger.getLogger(), "eXosip_call_send_answer success");
				}
			}

			eXosip_unlock();
			if (200 == mcfRes->statusCode) {
				// Final response terminates the transaction
				this->m_map_branch_tid.remove(pCtrlMsg->via.branch.c_str());
			}
		} else {
			LOG4CXX_ERROR(mLogger.getLogger(), "UNKNOW Transaction: "<<
					pCtrlMsg->via.branch.c_str());
		}
	} // RESPONSE for INVITE
	else if (!strcmp(pCtrlMsg->cseq_method.c_str(), "REGISTER") || !strcmp(
			pCtrlMsg->cseq_method.c_str(), "MESSAGE")) {
		INT tid = -1;
		if (this->m_map_branch_tid.get(pCtrlMsg->via.branch.c_str(), tid)) {
			osip_message_t* ans = NULL;
			int ret;

			eXosip_lock();
			ret = eXosip_message_build_answer(tid, mcfRes->statusCode, &ans);
			if (OSIP_SUCCESS != ret) {
				LOG4CXX_ERROR(mLogger.getLogger(), "eXosip build answer failed");
			} else {
				ExosipTranslator::convertMCF2OsipResp(*mcfRes, ans);

				ret = eXosip_message_send_answer(tid, mcfRes->statusCode, ans);
				if (OSIP_SUCCESS != ret) {
					LOG4CXX_ERROR(mLogger.getLogger(), "eXosip_call_send_answer "<<mcfRes->statusCode<<" failed: "<<ret);
				} else {
					LOG4CXX_DEBUG(mLogger.getLogger(), "eXosip_call_send_answer success");
				}
			}eXosip_unlock();

			this->m_map_branch_tid.remove(pCtrlMsg->via.branch.c_str());
		} else {
			LOG4CXX_ERROR(mLogger.getLogger(), "UNKNOW Transaction: "<<
					pCtrlMsg->via.branch.c_str());
		}
	} // RESPONSE for REGISTER/MESSAGE
	else if (!strcmp(pCtrlMsg->cseq_method.c_str(), "BYE")) {
		// 200 ok is required to be sent and have been sent automatically by eXosip
	} // RESPONSE for BYE
	else if (!strcmp(pCtrlMsg->cseq_method.c_str(), "INFO")) {
		INT tid = -1;
		if (this->m_map_branch_tid.get(pCtrlMsg->via.branch.c_str(), tid)) {
			osip_message_t* ans = NULL;
			int ret;

			eXosip_lock();
			ret = eXosip_call_build_answer(tid, mcfRes->statusCode, &ans);
			//ret = eXosip_message_build_answer();
			if (OSIP_SUCCESS != ret) {
				LOG4CXX_ERROR(mLogger.getLogger(), "eXosip build answer failed");
			} else {
				ExosipTranslator::convertMCF2OsipResp(*mcfRes, ans);
				char * buf = NULL;
				size_t len;
				osip_message_to_str(ans, &buf, &len);
				LOG4CXX_DEBUG(mLogger.getLogger(), "SIPPSA send answer:\n"<<buf);
				osip_free(buf);
				buf = NULL;
				//ret = eXosip_message_send_answer(tid, mcfRes->statusCode, ans);
				ret = eXosip_call_send_answer(tid, mcfRes->statusCode, ans);
				if (OSIP_SUCCESS != ret) {
					LOG4CXX_ERROR(mLogger.getLogger(), "eXosip_call_send_answer "<<mcfRes->statusCode<<" failed: "<<ret);
				}
			}eXosip_unlock();
			this->m_map_branch_tid.remove(pCtrlMsg->via.branch.c_str());
		}
	} else {
		psaPrint(m_psaid, "UNHANDLED sip method: %s\n",
				pCtrlMsg->cseq_method.c_str());
	}

	return TRUE;
}

BOOL CExosipStack::onSend_SIP_ACK(PCTUniNetMsg uniMsg) {
	LOG4CXX_DEBUG(mLogger.getLogger(), "OnSend_SIP_ACK");
	PTSipCtrlMsg pCtrlMsg = (PTSipCtrlMsg) uniMsg->ctrlMsgHdr;

	//	printf("Send out SIP ACK!!!!!!!!\n");
	INT did = -1;
	//this->m_map_dialogid.getNext(did);

	did = this->getDid(pCtrlMsg->from, pCtrlMsg->to, pCtrlMsg->sip_callId);
	//	printf("did   %d", did);
	if (did != -1) {
		osip_message_t* ack = NULL;
		int ret;

		eXosip_lock();
		ret = eXosip_call_build_ack(did, &ack);
		if (ret != OSIP_SUCCESS) {
			LOG4CXX_ERROR(mLogger.getLogger() ,"eXosip_call_build_ack failed: "<<ret<<
					"\t(-6:OSIP_NOTFOUND -2:OSIP_BADPARAMETER)\n");
		} else {
			// try to fix ack send failed
			osip_via_t * via = (osip_via_t *) osip_list_get(&ack->vias, 0);
			if (via == NULL || via->protocol == NULL) {
				//DEBUG0(m_psaid, "via == NULL || via->protocol == NULL\n");
			} else {
				//DEBUG0(m_psaid, "via OK\n");
			}
			//	}
			// try to fix ack send failed END
			//ExosipTranslator::convertMCF2CtrlMsg(pCtrlMsg, ack);

			//			if(isConfCall(pCtrlMsg->to.url.username.c_str()) == true){
			//				osip_route_t *rt;
			//				osip_route_init(&rt);
			//				string confServerAddr = (string)"<" + confServer + ";lr>";
			//				osip_message_set_route(ack, confServerAddr.c_str());
			//			}

			string toStr = CSipMsgHelper::toString(pCtrlMsg->to).c_str();
			string fromStr = CSipMsgHelper::toString(pCtrlMsg->from).c_str();
			if (toStr.find("msml") != string::npos || fromStr.find("msml")
					!= string::npos) {
			} else if (accessMode == 0) {
				//				osip_route_t *rt;
				//				osip_route_init(&rt);
				//				string icscfAddr = (string) "<" + icscf + ";lr>";
				//				osip_message_set_route(ack, icscfAddr.c_str());
			} else {
				osip_route_t *rt;
				osip_route_init(&rt);
				string proxyAddr = (string) "<" + proxy + ";lr>";
				osip_message_set_route(ack, proxyAddr.c_str());

			}

			PTSipReq sipAck = (PTSipReq) uniMsg->msgBody;
			sipAck->req_uri.scheme = "tel";
			ack = ExosipTranslator::convertMCF2OsipReq(*sipAck, ack);
			char * buf = NULL;
			size_t len = 0;
			osip_message_to_str(ack, &buf, &len);
			ret = eXosip_call_send_ack(did, ack);
			LOG4CXX_DEBUG(mLogger.getLogger(), "SIPPSA send ACK:\n "<<buf);
			osip_free(buf);
			if (ret != OSIP_SUCCESS) {
				LOG4CXX_ERROR(mLogger.getLogger(), "eXosip_call_send_ack failed: "<<ret<<
						"\t(-6:OSIP_NOTFOUND -2:OSIP_BADPARAMETER\n");
			}
		}eXosip_unlock();
		return TRUE;
	} else {
		LOG4CXX_ERROR(mLogger.getLogger(), "UNKNOW Dialog for sending ACK\n");
		return FALSE;
	}
}

BOOL CExosipStack::onSend_SIP_BYE(PCTUniNetMsg uniMsg) {
	LOG4CXX_DEBUG(mLogger.getLogger(), "OnSend_SIP_BYE");
	INT did = -1;
	INT cid = -1;
	BOOL revert = false; // revert from,to?

	PTSipCtrlMsg pCtrlMsg = (PTSipCtrlMsg) uniMsg->ctrlMsgHdr;

	did = this->getDid(pCtrlMsg->from, pCtrlMsg->to, pCtrlMsg->sip_callId);
	cid = this->getCid(pCtrlMsg->from, pCtrlMsg->to, pCtrlMsg->sip_callId);

	if (-1 == cid) {
		// revert from to
		did = this->getDid(pCtrlMsg->to, pCtrlMsg->from, pCtrlMsg->sip_callId);
		cid = this->getCid(pCtrlMsg->to, pCtrlMsg->from, pCtrlMsg->sip_callId);
		revert = TRUE;
	}
	if (cid != -1) {
		eXosip_lock();

		//		if(isConfCall(pCtrlMsg->from.url.username.c_str()) == true){
		//			osip_route_t *rt;
		//			osip_route_init(&rt);
		//			string confServerAddr = (string)"<" + confServer + ";lr>";
		//			osip_message_set_route(ans, confServerAddr.c_str());
		//		}
		char* fromTag = NULL;
		if (pCtrlMsg->from.tag.length() != 0) {
			fromTag = pCtrlMsg->from.tag.c_str();
		}
		char* toTag = NULL;
		if (pCtrlMsg->to.tag.length() != 0) {
			toTag = pCtrlMsg->to.tag.c_str();
		}

		int status;
		string toStr = CSipMsgHelper::toString(pCtrlMsg->to).c_str();
		string fromStr = CSipMsgHelper::toString(pCtrlMsg->from).c_str();
		if (toStr.find("msml") != string::npos || fromStr.find("msml")
				!= string::npos) {
			status = eXosip_call_terminate(cid, did, NULL, fromTag, toTag);
		} else if (accessMode == 0) {
			//			string icscfAddr = (string) "<" + icscf + ";lr>";
			//			const char * nexthop = icscfAddr.c_str();
			//			status = eXosip_call_terminate(cid, did, nexthop, fromTag, toTag);
			status = eXosip_call_terminate(cid, did, NULL, fromTag, toTag);
		} else {
			string proxyAddr = (string) "<" + proxy + ";lr>";
			const char * nexthop = proxyAddr.c_str();
			status = eXosip_call_terminate(cid, did, nexthop, fromTag, toTag);
		}

		eXosip_unlock();
		if (OSIP_SUCCESS != status) {
			psaPrint(m_psaid, "eXosip_call_terminate failed: %d!\n", status);
		}
		if (!revert) {
			this->removeCid(pCtrlMsg->from, pCtrlMsg->to, pCtrlMsg->sip_callId);
			this->removeDid(pCtrlMsg->from, pCtrlMsg->to, pCtrlMsg->sip_callId);
		} else {
			this->removeCid(pCtrlMsg->to, pCtrlMsg->from, pCtrlMsg->sip_callId);
			this->removeDid(pCtrlMsg->to, pCtrlMsg->from, pCtrlMsg->sip_callId);
		}
		if (pCtrlMsg->via.branch.length() > 0) {
			// remove transaction id map
			this->m_map_branch_tid.remove(pCtrlMsg->via.branch.c_str());
		}
		return TRUE;
	} else {
		LOG4CXX_ERROR(mLogger.getLogger(), "BYE to unknown dialog!!!!!\n");
		return FALSE;
	}
}

BOOL CExosipStack::onSend_SIP_CANCEL(PCTUniNetMsg uniMsg) {
	LOG4CXX_DEBUG(mLogger.getLogger(), "OnSend_SIP_CANCEL");
	return this->onSend_SIP_BYE(uniMsg);
}

BOOL CExosipStack::onSend_SIP_INVITE(PCTUniNetMsg uniMsg) {
	LOG4CXX_DEBUG(mLogger.getLogger(), "OnSend_SIP_INVITE");
	PTSipCtrlMsg pCtrlMsg = (PTSipCtrlMsg) uniMsg->ctrlMsgHdr;

	PTSipInvite mcfInvite = (PTSipInvite) uniMsg->msgBody;

	// TODO: 是否在新消息中增加via字段填充？

	//printf("sip-psa receive INVITE, \n");

	/*	CVarChar routeStr;
	 if (pCtrlMsg->route.url.host.length() > 0)
	 {
	 DEBUGV(m_psaid, "OUTGOING SIP INVITE route before convert: %s\n",
	 pCtrlMsg->route.url.host.c_str());

	 routeStr = CSipMsgHelper::toStringRoute(pCtrlMsg->route);

	 DEBUGV(m_psaid, "OUTGOING SIP INVITE route after convert: %s\n",
	 routeStr.c_str());
	 }
	 */

	int did = this->getDid(pCtrlMsg->from, pCtrlMsg->to, pCtrlMsg->sip_callId);
	osip_message_t * invite;
	if (did != -1) {
		LOG4CXX_DEBUG(mLogger.getLogger(), "SipPsa build re-invite");
		int ret = eXosip_call_build_request(did, "INVITE", &invite);
		if (OSIP_SUCCESS != ret) {
			printf("eXosip_call_build_initial_invite failed!\n");
			return FALSE;
		}
	} else {
		//	pCtrlMsg->to.url.scheme = "tel";
		string toStr = CSipMsgHelper::toString(pCtrlMsg->to).c_str();
		string fromStr = CSipMsgHelper::toString(pCtrlMsg->from).c_str();

		int ret = eXosip_call_build_initial_invite(&invite, toStr.c_str(),
				CSipMsgHelper::toString(pCtrlMsg->from).c_str(), NULL, NULL);
		if (OSIP_SUCCESS != ret) {
			LOG4CXX_ERROR(mLogger.getLogger(), "eXosip_call_build_initial_invite failed!\n");
			return FALSE;
		}

		if (toStr.find("msml") != string::npos || fromStr.find("msml")
				!= string::npos) {
		} else if (1 == accessMode) {
			osip_route_t *rt;
			osip_route_init(&rt);
			char * response = new char[128];

			if (osip_route_parse(rt, proxy.c_str()) != 0) {
				LOG4CXX_ERROR(mLogger.getLogger(), "Proxy cannot parse to route");
				return NULL;
			} else {
				osip_uri_uparam_add(rt->url,osip_strdup("lr"),NULL);
				osip_route_to_str(rt, &response);
			}

			osip_route_free(rt);
			osip_message_set_route(invite, response);

		} else if (2 == accessMode) {
			osip_route_t *rt;
			osip_route_init(&rt);
			char * response = new char[128];

			if (osip_route_parse(rt, proxy.c_str()) != 0) {
				LOG4CXX_ERROR(mLogger.getLogger(), "Proxy cannot parse to route");
				return NULL;
			} else {
				osip_uri_uparam_add(rt->url,osip_strdup("lr"),NULL);
				osip_route_to_str(rt, &response);
			}

			osip_route_free(rt);
			osip_message_set_route(invite, response);

			string url = "sip:";
			url.append(osip_from_get_url(invite->from)->username);
			url.append("@");
			url.append(osip_from_get_url(invite->from)->host);

			if (m_service_route.find(url) != m_service_route.end()) {
				vector<char *> vec = m_service_route[url];
				vector<char*>::reverse_iterator it = vec.rbegin();
				//printf("sippsa::::accessMode = 1; from url: %s, service route:\n", url.c_str());
				while (it != vec.rend()) {
					//printf("*****service route: %s\n", *it);
					osip_message_set_route(invite, (*it));
					it++;
				}
			}
		} else {
			osip_route_t *rt;
			osip_route_init(&rt);
			string icscfAddr = (string) "<" + icscf + ";lr>";
			osip_message_set_route(invite, icscfAddr.c_str());
		}
	}

	//char * buf1 = NULL;
	//size_t len1;
	//osip_message_to_str(invite, &buf1, &len1);

	ExosipTranslator::convertMCF2CtrlMsg(pCtrlMsg, invite);
	ExosipTranslator::fillOsipBody(mcfInvite->body, mcfInvite->content_type,
			invite);
	//added by liling 2010-7 to add ROUTE to out-going SIP message

	//invite->to->url->scheme = "tel";
	//printf("invite %s %s\n", invite->to->url->scheme, invite->to->url->username);
	osip_message_set_supported(invite, "100rel,timer");
	osip_message_set_header("Session-Expires", "600;refresher=uas");
	char* buf = NULL;
	size_t len;
	osip_message_to_str(invite, &buf, &len);

	LOG4CXX_DEBUG(mLogger.getLogger(), "SIPPSA send invite: \n"<<buf);
	osip_free(buf);

	eXosip_lock();
	eXosip_call_send_initial_invite(invite);
	eXosip_unlock();

	//	time_t timel;
	//	time(&timel);
	//	printf("\n %s ####sip-psa count= %d\n\n", ctime(&timel), ++count);

	return TRUE;
}

BOOL CExosipStack::onSend_SIP_REGISTER(PCTUniNetMsg uniMsg) {
	LOG4CXX_DEBUG(mLogger.getLogger(), "OnSend_SIP_REGISTER");
	PTSipCtrlMsg pCtrlMsg = (PTSipCtrlMsg) uniMsg->ctrlMsgHdr;

	PTSipRegister mcfRegister = (PTSipRegister) uniMsg->msgBody;

	osip_message_t *reg = NULL;
	INT rid = 0;
	rid = eXosip_register_build_initial_register(CSipMsgHelper::toString(
			pCtrlMsg->from).c_str(), CSipMsgHelper::toString(
			mcfRegister->req_uri).c_str(), CSipMsgHelper::toString(
			mcfRegister->contact).c_str(), mcfRegister->expires, &reg);

	ExosipTranslator::convertMCF2CtrlMsg(pCtrlMsg, reg);

	eXosip_lock();
	int ret = eXosip_register_send_register(rid, reg);
	eXosip_unlock();

	if (OSIP_SUCCESS != ret) {
		LOG4CXX_ERROR(mLogger.getLogger(), "SIPPSA send REGISTER failed!!!\n");
		return FALSE;
	} else {
		//DEBUGV(m_psaid, "keep exosip register rid: %d\n", rid);
		//m_map_registerid.put(CSipMsgHelper::toString(pCtrlMsg->from.url).c_str(), rid);
	}

	return TRUE;
}

BOOL CExosipStack::onSend_SIP_MESSAGE(PCTUniNetMsg uniMsg) {
	LOG4CXX_DEBUG(mLogger.getLogger(), "OnSend_SIP_MESSAGE");
	PTSipCtrlMsg pCtrlMsg = (PTSipCtrlMsg) uniMsg->ctrlMsgHdr;

	PTSipMessage mcfMsg = (PTSipMessage) uniMsg->msgBody;
	osip_message_t *msg = NULL;

	CVarChar routeStr;

	if (pCtrlMsg->route.url.host.length() > 0) {
		//DEBUGV(m_psaid, "OUTGOING SIP MSG route before convert: %s\n",
		//	pCtrlMsg->route.url.host.c_str());

		routeStr = CSipMsgHelper::toStringRoute(pCtrlMsg->route);

		//DEBUGV(m_psaid, "OUTGOING SIP MSG route after convert: %s\n", routeStr.c_str());
	}

	int ret = eXosip_message_build_request(&msg, "MESSAGE",
			CSipMsgHelper::toString(pCtrlMsg->to).c_str(),
			CSipMsgHelper::toString(pCtrlMsg->from).c_str(), (routeStr.length()
					> 0) ? routeStr.c_str() : NULL);

	if (OSIP_SUCCESS != ret) {
		psaPrint(m_psaid, "eXosip_message_build_request failed!\n");
		return FALSE;
	}

	string toStr = CSipMsgHelper::toString(pCtrlMsg->to).c_str();
	string fromStr = CSipMsgHelper::toString(pCtrlMsg->from).c_str();
	if (toStr.find("msml") != string::npos || fromStr.find("msml")
			!= string::npos) {
		//printf("send to XMS");
	} else if (1 == accessMode) {
		osip_route_t *rt;
		osip_route_init(&rt);
		char * response = new char[128];

		if (osip_route_parse(rt, proxy.c_str()) != 0) {
			LOG4CXX_ERROR(mLogger.getLogger(), "SIPPSA cannot parse to route");
			return NULL;
		} else {
			osip_uri_uparam_add(rt->url,osip_strdup("lr"),NULL);
			osip_route_to_str(rt, &response);
		}

		osip_route_free(rt);
		osip_message_set_route(msg, response);
	} else if (2 == accessMode) {
		osip_route_t *rt;
		osip_route_init(&rt);
		char * response = new char[128];

		if (osip_route_parse(rt, proxy.c_str()) != 0) {
			LOG4CXX_ERROR(mLogger.getLogger(), "SIPPSA cannot parse to route");
			return NULL;
		} else {
			osip_uri_uparam_add(rt->url,osip_strdup("lr"),NULL);
			osip_route_to_str(rt, &response);
		}

		osip_route_free(rt);
		osip_message_set_route(msg, response);

		string url = "sip:";
		url.append(osip_from_get_url(msg->from)->username);
		url.append("@");
		url.append(osip_from_get_url(msg->from)->host);

		if (m_service_route.find(url) != m_service_route.end()) {
			vector<char *> vec = m_service_route[url];
			vector<char*>::reverse_iterator it = vec.rbegin();
			while (it != vec.rend()) {
				osip_message_set_route(msg, (*it));
				it++;
			}
		}
	} else {
		osip_route_t *rt;
		osip_route_init(&rt);
		string icscfAddr = (string) "<" + icscf + ";lr>";
		osip_message_set_route(msg, icscfAddr.c_str());
	}

	ExosipTranslator::convertMCF2CtrlMsg(pCtrlMsg, msg);

	ExosipTranslator::convertMCF2OsipMessage(*mcfMsg, msg);

	//add by liling 2010-7 to add ROUTE to out-going SIP MESSAGE
	char* buf = NULL;
	size_t len;
	osip_message_to_str(msg, &buf, &len);
	LOG4CXX_DEBUG(mLogger.getLogger(), "SIPPSA send MESSAGE"<<buf);
	osip_free(buf);

	eXosip_lock();
	ret = eXosip_message_send_request(msg);
	eXosip_unlock();
	if (OSIP_SUCCESS != ret) {
		LOG4CXX_ERROR(mLogger.getLogger(), "SIPPSA send MESSAGE"<<buf);
		return FALSE;
	}

	return TRUE;
}

BOOL CExosipStack::onSend_SIP_INFO(PCTUniNetMsg uniMsg) {
	LOG4CXX_DEBUG(mLogger.getLogger(), "OnSend_SIP_INFO");
	PTSipCtrlMsg pCtrlMsg = (PTSipCtrlMsg) uniMsg->ctrlMsgHdr;

	PTSipInfo mcfMsg = (PTSipInfo) uniMsg->msgBody;
	int did = this->getDid(pCtrlMsg->from, pCtrlMsg->to, pCtrlMsg->sip_callId);
	if (did != -1) {
		osip_message_t *msg = NULL;

		//		CVarChar routeStr;
		//
		//		if (pCtrlMsg->route.url.host.length() > 0)
		//		{
		//			DEBUGV(m_psaid, "OUTGOING SIP MSG route before convert: %s\n",
		//					pCtrlMsg->route.url.host.c_str());
		//
		//			//routeStr = CSipMsgHelper::toStringRoute(pCtrlMsg->route);
		//			routeStr = toOsipRoute(pCtrlMsg->route);
		//
		//			printf("OUTGOING SIP MSG route after convert: %s\n", routeStr.c_str());
		//		}

		int ret = eXosip_call_build_info(did, &msg);

		if (OSIP_SUCCESS != ret) {
			psaPrint(m_psaid, "eXosip_message_build_request failed!\n");
			return FALSE;
		}
		//	ExosipTranslator::convertMCF2CtrlMsg(pCtrlMsg, msg);

		ExosipTranslator::convertMCF2OsipInfo(*mcfMsg, msg);

		//	ExosipTranslator::convertMCF2OsipUri(pCtrlMsg->to.url, msg->req_uri);


		//add by liling 2010-7 to add ROUTE to out-going SIP MESSAGE
		char* buf = NULL;
		size_t len;
		osip_message_to_str(msg, &buf, &len);
		LOG4CXX_DEBUG(mLogger.getLogger(), "SIPPSA send INFO:\n"<<buf);
		osip_free(buf);

		eXosip_lock();
		ret = eXosip_message_send_request(msg);
		eXosip_unlock();
		if (OSIP_SUCCESS != ret) {
			psaPrint(m_psaid, "Send MESSAGE failed!!!\n");
			return FALSE;
		}

		return TRUE;
	} else {
		psaPrint(m_psaid, "UNKNOW Dialog for sending ACK\n");
		return FALSE;
	}

}

BOOL CExosipStack::onSend_SIP_PRACK(PCTUniNetMsg uniMsg) {
	LOG4CXX_DEBUG(mLogger.getLogger(), "OnSend_SIP_PRACK");
	PTSipCtrlMsg pCtrlMsg = (PTSipCtrlMsg) uniMsg->ctrlMsgHdr;

	int tid = -1;
	if (this->m_map_branch_tid.get(pCtrlMsg->via.branch.c_str(), tid)) {
		osip_message_t *msg = NULL;
		int ret;
		eXosip_lock();
		ret = eXosip_call_build_prack(tid, &msg);

		if (OSIP_SUCCESS != ret) {
			LOG4CXX_ERROR(mLogger.getLogger(), "eXosip_prack_build_request failed!\n");
			return FALSE;
		}
		//	ExosipTranslator::convertMCF2CtrlMsg(pCtrlMsg, msg);


		PTSipPrack mcfMsg = (PTSipPrack) uniMsg->msgBody;
		ExosipTranslator::convertMCF2OsipPrack(*mcfMsg, msg);

		//	ExosipTranslator::convertMCF2OsipUri(pCtrlMsg->to.url, msg->req_uri);

		char* buf = NULL;
		size_t len;
		osip_message_to_str(msg, &buf, &len);
		LOG4CXX_DEBUG(mLogger.getLogger(), "SIPPSA send RACK:\n"<<msg);
		osip_free(buf);

		eXosip_lock();
		ret = eXosip_message_send_request(msg);
		eXosip_unlock();
		if (OSIP_SUCCESS != ret) {
			LOG4CXX_ERROR(mLogger.getLogger(), "send RACK failed");
			return FALSE;
		}

		return TRUE;
	} else {
		LOG4CXX_ERROR(mLogger.getLogger(), "UNKNOW Dialog for sending PRACK\n");
		return FALSE;
	}

}

BOOL CExosipStack::onSend_SIP_UPDATE(PCTUniNetMsg uniMsg) {
	LOG4CXX_DEBUG(mLogger.getLogger(), "OnSend_SIP_UPDATE");
	PTSipCtrlMsg pCtrlMsg = (PTSipCtrlMsg) uniMsg->ctrlMsgHdr;

	PTSipUpdate mcfMsg = (PTSipUpdate) uniMsg->msgBody;
	int did = this->getDid(pCtrlMsg->from, pCtrlMsg->to, pCtrlMsg->sip_callId);
	if (did != -1) {
		osip_message_t *msg = NULL;

		//		CVarChar routeStr;
		//
		//		if (pCtrlMsg->route.url.host.length() > 0)
		//		{
		//			DEBUGV(m_psaid, "OUTGOING SIP MSG route before convert: %s\n",
		//					pCtrlMsg->route.url.host.c_str());
		//
		//			//routeStr = CSipMsgHelper::toStringRoute(pCtrlMsg->route);
		//			routeStr = toOsipRoute(pCtrlMsg->route);
		//
		//			printf("OUTGOING SIP MSG route after convert: %s\n", routeStr.c_str());
		//		}

		int ret = eXosip_call_build_update(did, &msg);

		if (OSIP_SUCCESS != ret) {
			psaPrint(m_psaid, "eXosip_message_build_request failed!\n");
			return FALSE;
		}
		//	ExosipTranslator::convertMCF2CtrlMsg(pCtrlMsg, msg);

		ExosipTranslator::convertMCF2OsipUpdate(*mcfMsg, msg);

		//	ExosipTranslator::convertMCF2OsipUri(pCtrlMsg->to.url, msg->req_uri);


		//add by liling 2010-7 to add ROUTE to out-going SIP MESSAGE
		char* buf = NULL;
		size_t len;
		osip_message_to_str(msg, &buf, &len);
		LOG4CXX_DEBUG(mLogger.getLogger(), "SIPPSA send UPDATE:\n"<<buf);
		osip_free(buf);

		eXosip_lock();
		ret = eXosip_message_send_request(msg);
		eXosip_unlock();
		if (OSIP_SUCCESS != ret) {
			LOG4CXX_ERROR(mLogger.getLogger(), "send UPDATE failed");
			return FALSE;
		}

		return TRUE;
	} else {
		LOG4CXX_ERROR(mLogger.getLogger(), "UNKNOW Dialog for sending ACK\n");
		return FALSE;
	}

}

void CExosipStack::storeDid(RCTSipAddress from, RCTSipAddress to,
		RCTSipCallId callId, INT dialogId) {
	CHAR* uniqueDialogId = new CHAR[512];
	memset(uniqueDialogId, 0, 512);
	__generateUniqueDialogId(uniqueDialogId, 512, from, to, callId);

	this->m_map_dialogid.put(uniqueDialogId, dialogId);

	LOG4CXX_DEBUG(mLogger.getLogger(), "StoreDid: "<<uniqueDialogId<<" ["<<dialogId<<"]");

	delete[] uniqueDialogId;
}

void CExosipStack::storeCid(RCTSipAddress from, RCTSipAddress to,
		RCTSipCallId callId, INT cid) {
	CHAR* uniqueCallId = new CHAR[512];
	memset(uniqueCallId, 0, 512);
	__generateUniqueCallId(uniqueCallId, 512, from, to, callId);
	this->m_map_callid.put(uniqueCallId, cid);

	LOG4CXX_DEBUG(mLogger.getLogger(), "StoreCid: "<<uniqueCallId<<" ["<<cid<<"]");

	delete[] uniqueCallId;
}

void CExosipStack::removeDid(RCTSipAddress from, RCTSipAddress to,
		RCTSipCallId callId) {
	CHAR* uniqueDialogId = new CHAR[512];
	memset(uniqueDialogId, 0, 512);
	__generateUniqueDialogId(uniqueDialogId, 512, from, to, callId);

	this->m_map_dialogid.remove(uniqueDialogId);

	LOG4CXX_DEBUG(mLogger.getLogger(), "RmDid:"<<uniqueDialogId);

	delete[] uniqueDialogId;
}

void CExosipStack::removeCid(RCTSipAddress from, RCTSipAddress to,
		RCTSipCallId callId) {
	CHAR* uniqueCallId = new CHAR[512];
	memset(uniqueCallId, 0, 512);
	__generateUniqueCallId(uniqueCallId, 512, from, to, callId);
	this->m_map_callid.remove(uniqueCallId);

	LOG4CXX_DEBUG(mLogger.getLogger(), "RmCid: "<<uniqueCallId);

	delete[] uniqueCallId;
}

INT CExosipStack::getDid(RCTSipAddress from, RCTSipAddress to,
		RCTSipCallId callId) {
	INT did = -1;

	CHAR* uniqueDialogId = new CHAR[512];
	memset(uniqueDialogId, 0, 512);
	__generateUniqueDialogId(uniqueDialogId, 512, from, to, callId);

	if (FALSE == this->m_map_dialogid.get(uniqueDialogId, did)) {
		did = -1;
	}
	printf("getDid %s %d\n", uniqueDialogId, did);

	delete[] uniqueDialogId;

	return did;
}

INT CExosipStack::getCid(RCTSipAddress from, RCTSipAddress to,
		RCTSipCallId callId) {
	INT cid = -1;

	CHAR* uniqueCallId = new CHAR[512];
	memset(uniqueCallId, 0, 512);
	__generateUniqueCallId(uniqueCallId, 512, from, to, callId);
	if (FALSE == this->m_map_callid.get(uniqueCallId, cid)) {
		cid = -1;
	}

	delete[] uniqueCallId;

	return cid;
}

void MyMap::put(string key, INT val) {
	_map[key] = val;
	//_map.insert(pair<string, INT>(key, val));
}

bool MyMap::get(string key, INT& val) {
	LOG4CXX_DEBUG(mLogger.getLogger(),"MyMap: values");
	for (map<string, INT>::iterator it = _map.begin(); it != _map.end(); ++it) {
		LOG4CXX_DEBUG(mLogger.getLogger(), it->first<<" => "<<it->second);
	}

	if (_map.find(key) != _map.end()) {
		val = _map[key];
		return TRUE;
	}
	return FALSE;
}

void MyMap::remove(string key) {
	if (_map.find(key) != _map.end()) {
		_map.erase(key);
	}
}
