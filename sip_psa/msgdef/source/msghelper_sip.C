/************************************************************************

 * Copyright (c)2010-2012, by BUPT
 * All rights reserved.

 * FileName：       msghelper_sip.h
 * System：         MCF
 * SubSystem：      PSA
 * Author：         Huang Haiping
 * Date：           2010.04.27
 * Version：        1.0
 * Description：
 中间消息辅助类，负责将SIP中间消息转换成标准字符串格式

 *
 * Last Modified:
 2010.04.27, 完成初始版本定义
 By Huang Haiping


 *************************************************************************/
#include "msghelper_sip.h"
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#define HAVE_LRAND48

static BOOL s_g_random_seed_set = FALSE;
CVarChar CSipMsgHelper::toString(RCTSipSubscriptionState ss) {
	CVarChar result;
	CHAR tmp[256];

	if (!strcmp("terminated", ss.state.c_str())) {
		sprintf(tmp, "%s;reason=%s", ss.state.c_str(), ss.reason.c_str());
	} else {
		sprintf(tmp, "%s;expires=%d", ss.state.c_str(), ss.expires);
	}

	result = tmp;

	return result;
}

CVarChar CSipMsgHelper::toString(RCTSipAddress sipAddress) {
	CVarChar result;
	CVarChar url;
	CHAR tmp[256];
	CHAR* p;

	url = CSipMsgHelper::toString(sipAddress.url);

	if (url.length() == 0) {
		return result;
	}

	if (sipAddress.displayname.length() != 0) {
		sprintf(tmp, "%s <%s>", sipAddress.displayname.c_str(), url.c_str());
	} else {
		sprintf(tmp, "<%s>", url.c_str());
	}
	p = tmp + strlen(tmp);

	if (sipAddress.tag.length() > 0) {
		sprintf(p, ";tag=%s", sipAddress.tag.c_str());
	}

	result = tmp;

	return result;
}

CVarChar CSipMsgHelper::toString(RCTSipURI sipUri) {
	CVarChar result;
	CHAR tmp[256];
	CHAR* p;

	if (sipUri.scheme.length() != 0) {
		sprintf(tmp, "%s:", sipUri.scheme.c_str());
	} else {
		sprintf(tmp, "sip:");
	}
	p = tmp + strlen(tmp);

	if (sipUri.username.length() != 0) {
		sprintf(p, "%s", sipUri.username.c_str());
		p = p + strlen(p);
	}
	if (sipUri.host.length() != 0) {
		sprintf(p, "@%s", sipUri.host.c_str());
		p = p + strlen(p);
	}
	if (sipUri.port.length() != 0) {
		sprintf(p, ":%s", sipUri.port.c_str());
		p = p + strlen(p);
	}

	result = tmp;

	return result;
}

CVarChar CSipMsgHelper::toString(RCTSipCallId sipCallId) {
	CVarChar result;
	CHAR tmp[256];

	if (sipCallId.number.length() == 0) {
		return result;
	}

	if (sipCallId.host.length() != 0) {
		sprintf(tmp, "%s@%s", sipCallId.number.c_str(), sipCallId.host.c_str());
	} else {
		sprintf(tmp, "%s", sipCallId.number.c_str());
	}

	result = tmp;

	return result;
}

CVarChar CSipMsgHelper::toString(RCTSipContentType sipContentType) {
	CVarChar result;
	CHAR tmp[32];

	if (sipContentType.type.length() == 0 || sipContentType.subtype.length()
			== 0) {
		return result;
	}
	sprintf(tmp, "%s/%s", sipContentType.type.c_str(),
			sipContentType.subtype.c_str());

	result = tmp;

	return result;
}

CVarChar CSipMsgHelper::toStringRoute(RCTSipAddress sipRoute) {
	CVarChar result;
	CVarChar url;
	CHAR tmp[256];

	url = CSipMsgHelper::toString(sipRoute.url);

	if (url.length() == 0) {
		return result;
	}
	sprintf(tmp, "%s;lr", url.c_str());

	result = tmp;

	return result;
}

UINT CSipMsgHelper::generateRandomNumberUINT() {
	if (!s_g_random_seed_set) {
		unsigned int ticks;
		struct timeval tv;
		int fd;

		gettimeofday(&tv, NULL);
		ticks = tv.tv_sec + tv.tv_usec;
		fd = open("/dev/urandom", O_RDONLY);
		if (fd > 0) {
			unsigned int r;
			int i;

			for (i = 0; i < 512; i++) {
				read(fd, &r, sizeof(r));
				ticks += r;
			}
			close(fd);
		}
#ifdef HAVE_LRAND48
		srand48(ticks);
#else
		srand (ticks);
#endif
		s_g_random_seed_set = TRUE;
	} // s_g_random_seed_set not set
#ifdef HAVE_LRAND48
	{
		int val = lrand48();
		if (val == 0) {
			unsigned int ticks;
			struct timeval tv;
			gettimeofday(&tv, NULL);
			ticks = tv.tv_sec + tv.tv_usec;
			srand48(ticks);
			return lrand48();
		}

		return val;
	}
#else
	return rand ();
#endif
}

void CSipMsgHelper::generateRandomNumberStr(CHAR buf[33]) {
	UINT randid = CSipMsgHelper::generateRandomNumberUINT();

	sprintf(buf, "%u", randid);
}

TSipURI CSipMsgHelper::createSipURI(const string& scheme,
		const string & userName, const string & host, const string& port) {
	TSipURI sipUri;
	if (!scheme.empty())
		sipUri.scheme = scheme.c_str();
	else
		sipUri.scheme = "sip";
	if (!userName.empty())
		sipUri.username = userName.c_str();
	if (!host.empty())
		sipUri.host = host.c_str();
	if (!port.empty())
		sipUri.port = port.c_str();
	return sipUri;
}

TSipContentType CSipMsgHelper::createSipContentType(const CHAR* type,
		const CHAR* subtype) {
	TSipContentType contentType;

	if (type)
		contentType.type = type;
	if (subtype)
		contentType.subtype = subtype;

	return contentType;
}

TSipBody CSipMsgHelper::createSipBody(CVarChar content) {
	TSipBody bdy;

	if (content.length() > 0) {
		bdy.content = content;
		bdy.content_length = content.length();
	}

	return bdy;
}
