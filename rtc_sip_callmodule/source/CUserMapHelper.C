#include "CUserMapHelper.h"

CUserMapHelper::CUserMapHelper()
{

}

CUserMapHelper::~CUserMapHelper()
{

}

CVarChar128 CUserMapHelper::getMapSipUser(CVarChar128 rtcname)
{
	CHAR pcSQLStatement[512];
	sprintf(pcSQLStatement,"SELECT sipname FROM user_map_table WHERE isRegistered = 1 AND rtcname = '%s' ", rtcname.c_str());
	CDB::instance()->execSQL(pcSQLStatement);
	PTSelectResult result = CDB::instance()->getSelectResult();
	CVarChar128 sipname;
	if (result->rowNum > 0){
		sipname = result->pRows->arrayField[0].value.stringValue;
	}
	return sipname;

}

CVarChar128 CUserMapHelper::getMapRtcUser(CVarChar128 sipname)
{
	CHAR pcSQLStatement[512];
	sprintf(pcSQLStatement,"SELECT rtcname FROM user_map_table WHERE isRegistered = 1 AND sipname = '%s' ", sipname.c_str());
	CDB::instance()->execSQL(pcSQLStatement);
	PTSelectResult result = CDB::instance()->getSelectResult();
	CVarChar128 rtcname;
	if (result->rowNum > 0){
		rtcname = result->pRows->arrayField[0].value.stringValue;
	}

	return rtcname;
}


void CUserMapHelper::resetCalling(string& sipname)
{
	CHAR pcSQLStatement[512];
	
	sprintf(pcSQLStatement,"UPDATE user_map_table SET isCalling = 0, lastUsedTime=NOW() WHERE sipname = '%s'", sipname.c_str());
	//printf("#####RESET: %s", sipname.c_str());
	CDB::instance()->execSQL(pcSQLStatement);
	return;
}

CVarChar128 CUserMapHelper::getMapNewSipUser(CVarChar128 rtcname)
{
	CHAR pcSQLStatement[512];
	sprintf(pcSQLStatement,"SELECT sipname FROM user_map_table WHERE isRegistered = 1 AND rtcname = '%s' ", rtcname.c_str());
	CDB::instance()->execSQL(pcSQLStatement);
	PTSelectResult result = CDB::instance()->getSelectResult();
	CVarChar128 sipname;
	//printf("result->rowNum %d\n", result->rowNum);
	if (result->rowNum > 0){
		sipname = result->pRows->arrayField[0].value.stringValue;
		sprintf(pcSQLStatement,"UPDATE user_map_table SET isCalling = 1 WHERE sipname = '%s'", sipname.c_str());
		CDB::instance()->execSQL(pcSQLStatement);
	}
	else{
		sprintf(pcSQLStatement,"SELECT sipname FROM user_map_table WHERE isRegistered = 1 AND rtcname = '' ");
		CDB::instance()->execSQL(pcSQLStatement);
		PTSelectResult result = CDB::instance()->getSelectResult();

		if(result->rowNum > 0){
			sipname = result->pRows->arrayField[0].value.stringValue;
			sprintf(pcSQLStatement,"UPDATE user_map_table SET rtcname = '%s', isCalling = 1 WHERE sipname = '%s'", rtcname.c_str(), sipname.c_str());
			CDB::instance()->execSQL(pcSQLStatement);
		}
		else{
			//ToDo 从数据库中获取不再通话的sipuser
			sprintf(pcSQLStatement,"SELECT sipname FROM user_map_table WHERE isRegistered = 1 AND isCalling = 0");
			CDB::instance()->execSQL(pcSQLStatement);
			PTSelectResult result = CDB::instance()->getSelectResult();

			if(result->rowNum > 0){
				sipname = result->pRows->arrayField[0].value.stringValue;
				//printf("sipname = %s\n", sipname.c_str());
				sprintf(pcSQLStatement,"UPDATE user_map_table SET rtcname = '%s', isCalling = 1 WHERE sipname = '%s'", rtcname.c_str(), sipname.c_str());
				CDB::instance()->execSQL(pcSQLStatement);
			}


		}
	}

	return sipname;
}

CVarChar128 CUserMapHelper::getMapNewRtcUser(CVarChar128 sipname)
{

	CHAR pcSQLStatement[512];
	sprintf(pcSQLStatement,"SELECT rtcname FROM user_map_table WHERE isRegistered = 1 AND sipname = '%s' ", sipname.c_str());
	CDB::instance()->execSQL(pcSQLStatement);
	PTSelectResult result = CDB::instance()->getSelectResult();
	CVarChar128 rtcname;
	if (result->rowNum > 0){
		rtcname = result->pRows->arrayField[0].value.stringValue;
	}

	return rtcname;

}
