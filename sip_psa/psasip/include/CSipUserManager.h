#ifndef CUSERMAPMANAGER_H_
#define CUSERMAPMANAGER_H_

#include <string>
#include <set>
#include <map>
using namespace std;
class CSipUserManager{
public:
	CSipUserManager();
	virtual ~CSipUserManager();
	static INT getSipUser(set<string> &nameArr);		//get all sipname pool
	static INT setRegistered(string sipname, INT value);
	static INT setAllRegistered(INT value);
	static string getSipPassword(string sipname);
	static INT init();		//init database, set isRegistered = 0 and rtcname = ''
	static map<string, string> user_map; //username -> password

	static int count;
};


#endif
