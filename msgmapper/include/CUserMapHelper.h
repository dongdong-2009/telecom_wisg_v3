#ifndef CUSERMAPHELPER_H
#define CUSERMAPHELPER_H
#include "db.h"
#include "psa_sip_inf.h"
class CUserMapHelper{
public:
	CUserMapHelper();
	virtual ~CUserMapHelper();
	static CVarChar128 getMapNewSipUser(CVarChar128 rtcname);
	static CVarChar128 getMapNewRtcUser(CVarChar128 sipname);

	static CVarChar128 getMapSipUser(CVarChar128 rtcname);
	static CVarChar128 getMapRtcUser(CVarChar128 sipname);
	static void resetCalling(CVarChar128 sipname);
};


#endif
