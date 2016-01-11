#ifndef _DBHANDLER_H
#define _DBHANDLER_H
#include <stdio.h>
#include "db.h"
#include "comtypedef.h"
#include "comtypedef_vchar.h"
INT getServerIdByUserId(CHAR* userID);
INT getServerIdByServerAddr(CHAR* ip, INT port);

// 操作idmap表，该表存储rtc中offersessionId（answersessionId）到sip中callid.number和tag的映射
CVarChar32 dbgetRtcSessionID(CHAR*  callIDNumber, CHAR*  tag);
CVarChar32 dbgetSipTag(CHAR*  sessionID);
CVarChar64 dbgetSipCallIDNumber(CHAR*  sessionID);

void dbstoreRtcToSipMap(CHAR*  sessionID, CHAR*  callIDNumber, CHAR*  tag);
void dbclearRtcToSipMap(CHAR*  sessionID);

#endif
