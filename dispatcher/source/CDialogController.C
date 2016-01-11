#include "CDialogController.h"

CDialogController::CDialogController(){
	m_mapDialog.clear();
}

CDialogController::~CDialogController(){
	m_mapDialog.clear();
}

BOOL CDialogController::storeDialog(const string &uniqID, TMsgAddress addr){
	pair<map<string, TMsgAddress>::iterator, bool> ret;
	ret = m_mapDialog.insert(pair<string, TMsgAddress>(uniqID, addr));
	//DubugOut();
	return ret.second;
}

BOOL CDialogController::getDialogAddr(const string &uniqID, TMsgAddress& addr){
	DubugOut();
	map<string, TMsgAddress>::iterator it = m_mapDialog.find(uniqID);
	if(it != m_mapDialog.end()){
		addr = it->second;
		return TRUE;
	}
	return FALSE;
}

BOOL CDialogController::clearDialog(const string &uniqID){
	int n = m_mapDialog.erase(uniqID);

	//erase msgmapper sessionIdtoCallIdTag, just work if the uniqid is sip callid number
	CVarChar128 Id128;
	Id128 = uniqID.c_str();
	CMsgMapHelper::clearRtcToSipMap(Id128);
	//erase msgmapper FromTagTotoTagWithAnswerId, just work if the uniqid is offersessionid
	//	(Note: fromtag is same with offersessionid)
//	CVarChar32 Id32;
//	Id32 = uniqID.c_str();
//	CMsgMapHelper::clearMapFromTagTotoTagWithAnswerId(Id32);

	DubugOut();
	return n == 1;
}

void CDialogController::DubugOut(){
	if(true){
	  cout << "CDialogController: map uniqID to addr "<<" size: "<<m_mapDialog.size()<<" contains: ";
	  for (map<string, TMsgAddress>::iterator it=m_mapDialog.begin(); it!=m_mapDialog.end(); ++it)
		cout << it->first<<"->LogAddr:"<<it->second.logAddr<<endl;
	  cout << endl;
	}
}



