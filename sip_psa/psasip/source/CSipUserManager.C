#include "db.h"
#include <stdio.h>
#include "CSipUserManager.h"

#define LOOP_COUNT	10
int CSipUserManager::count = LOOP_COUNT;
map<string, string> CSipUserManager::user_map;


CSipUserManager::CSipUserManager()
{

}

CSipUserManager::~CSipUserManager()
{

}

string CSipUserManager::getSipPassword(string sipname)
{
	CHAR pcSQLStatement[512];
	string passwd;

	if(count == LOOP_COUNT){
		count = 0;
		if(CDB::instance()->ping()){
			sprintf(pcSQLStatement,"SELECT password FROM user_map_table WHERE sipname = '%s'", sipname.c_str());
			CDB::instance()->execSQL(pcSQLStatement);
			PTSelectResult result = CDB::instance()->getSelectResult();


			if (result->rowNum > 0){
				passwd = result->pRows[0].arrayField[0].value.stringValue;
				user_map[sipname] = passwd;
				return passwd;
			}
			else{
				if(user_map.find(sipname) != user_map.end()){
					return user_map[sipname];
				}
			}
		}
		else
		{
			if(user_map.find(sipname) != user_map.end()){
				return user_map[sipname];
			}

		}

	}
	else{
		++count;
		if(user_map.find(sipname) != user_map.end()){
			return user_map[sipname];
		}
	}


	return "";

}

INT CSipUserManager::getSipUser(set<string> &nameArr)
{
	CHAR pcSQLStatement[512];

	sprintf(pcSQLStatement,"SELECT sipname FROM user_map_table");
	CDB::instance()->execSQL(pcSQLStatement);
	PTSelectResult result = CDB::instance()->getSelectResult();

	Row * cur = result->pRows;
	for(int i = 0; i < result->rowNum; ++i)
	{
		nameArr.insert(cur->arrayField[0].value.stringValue);
		cur = cur->next;
	}

	return 1;
}

INT CSipUserManager::setRegistered(string sipname, INT value)
{
	CHAR pcSQLStatement[512];
	sprintf(pcSQLStatement, "UPDATE user_map_table SET isRegistered = '%d' WHERE sipname = '%s'", value, sipname.c_str());

	return CDB::instance()->execSQL(pcSQLStatement);
}

INT CSipUserManager::setAllRegistered(INT value)
{
	CHAR pcSQLStatement[512];
	sprintf(pcSQLStatement, "UPDATE user_map_table SET isRegistered = '%d'", value);

	return CDB::instance()->execSQL(pcSQLStatement);
}

INT CSipUserManager::init()
{
	CHAR pcSQLStatement[512];
	sprintf(pcSQLStatement, "UPDATE user_map_table SET isRegistered = 0, isCalling = 0");

	return CDB::instance()->execSQL(pcSQLStatement);
}
