#include "dbHandler.h"

INT getServerIdByUserId(CHAR* userID){
	INT server_id;
	CHAR pcSQLStatement[100] ;
	sprintf(pcSQLStatement, "select server_id from user where user_id = '%s'", userID);
	CDB::instance()->execSQL(pcSQLStatement);
	PTSelectResult sltResult = CDB::instance()->getSelectResult();
	if(sltResult->rowNum != 1){
		printf("getServerID faild\n");
		return -1;
	}
	server_id = sltResult->pRows->arrayField[0].value.intValue;
	return server_id;
}

INT getServerIdByServerAddr(CHAR* ip, INT port){
	INT server_id;
	CHAR pcSQLStatement[100];
	sprintf(pcSQLStatement, "select id from server where server_addr = '%s' and port = '%d'", ip, port);
	CDB::instance()->execSQL(pcSQLStatement);
	PTSelectResult sltResult = CDB::instance()->getSelectResult();
	if(sltResult->rowNum != 1){
		printf("getServerID faild\n");
		return -1;
	}
	server_id = sltResult->pRows->arrayField[0].value.intValue;
	return server_id;
}

CVarChar32 dbgetRtcSessionID(CHAR*  callIDNumber, CHAR*  tag){
	CVarChar32 ret;
	CHAR pcSQLStatement[1024] ;
	sprintf(pcSQLStatement, "select rtc_session_id from idmap where sip_callid_num = '%s' and sip_tag = '%s'",
			callIDNumber, tag);
	CDB::instance()->execSQL(pcSQLStatement);
	PTSelectResult sltResult = CDB::instance()->getSelectResult();
	if(sltResult->rowNum < 1){
		printf("getRTCSessionID faild\n");
		return ret;
	}
	return ret = sltResult->pRows->arrayField[0].value.stringValue;
}

CVarChar32 dbgetSipTag(CHAR*  sessionID){
	CVarChar32 ret;
	CHAR pcSQLStatement[1024] ;
	sprintf(pcSQLStatement, "select sip_tag from idmap where rtc_session_id = '%s'", sessionID);
	CDB::instance()->execSQL(pcSQLStatement);
	PTSelectResult sltResult = CDB::instance()->getSelectResult();
	if(sltResult->rowNum < 1){
		printf("getSipTag faild\n");
		return ret;
	}
	return ret = sltResult->pRows->arrayField[0].value.stringValue;
}

CVarChar64 dbgetSipCallIDNumber(CHAR*  sessionID){
	CVarChar64 ret;
	CHAR pcSQLStatement[1024] ;
	sprintf(pcSQLStatement, "select sip_callid_num from idmap where rtc_session_id = '%s'", sessionID);
	CDB::instance()->execSQL(pcSQLStatement);
	PTSelectResult sltResult = CDB::instance()->getSelectResult();
	if(sltResult->rowNum < 1){
		printf("getSipCallIDNumber faild\n");
		return ret;
	}
	return ret = sltResult->pRows->arrayField[0].value.stringValue;
}

void dbstoreRtcToSipMap(CHAR*  sessionID, CHAR*  callIDNumber, CHAR*  tag){
	CHAR pcSQLStatement[1024] ;
	sprintf(pcSQLStatement, "insert into idmap values('%s', '%s', '%s')", sessionID, callIDNumber, tag);
	if(CDB::instance()->execSQL(pcSQLStatement) == 0){
		printf("store session map faild\n");
	}
}

void dbclearRtcToSipMap(CHAR*  sessionID){
	CHAR pcSQLStatement[1024] ;
	sprintf(pcSQLStatement, "delete from idmap where rtc_session_id = '%s'", sessionID);
	if(CDB::instance()->execSQL(pcSQLStatement) == 0){
		printf("clear session map faild\n");
	}
}
