#include "CDialogController.h"

CDialogController::CDialogController(){
	m_mapDialog.clear();
	m_mapBear.clear();
}

CDialogController::~CDialogController(){
	m_mapDialog.clear();
	m_mapBear.clear();
}

BOOL CDialogController::storeDialog(const string &uniqID, TMsgAddress addr){
	pair<map<string, TMsgAddress>::iterator, bool> ret;
	ret = m_mapDialog.insert(pair<string, TMsgAddress>(uniqID, addr));
	//DubugOut();
	return ret.second;
}

BOOL CDialogController::getDialogAddr(const string &uniqID, TMsgAddress& addr){
	//DubugOut();
	map<string, TMsgAddress>::iterator it = m_mapDialog.find(uniqID);
	if(it != m_mapDialog.end()){
		addr = it->second;
		return TRUE;
	}
	return FALSE;
}

BOOL CDialogController::clearDialog(const string &uniqID){
	int n = m_mapDialog.erase(uniqID);
	return n == 1;
}


BOOL CDialogController::storeBear(const string &uniqID, TMsgAddress addr){
	pair<map<string, TMsgAddress>::iterator, bool> ret;
	m_mapBear[uniqID] = addr;
	return TRUE;
	//ret = m_mapBear.insert(pair<string, TMsgAddress>(uniqID, addr));
	//DubugOut();
	//return ret.second;
}

BOOL CDialogController::getBearAddr(const string &uniqID, TMsgAddress& addr){
	//DubugOut();
	map<string, TMsgAddress>::iterator it = m_mapBear.find(uniqID);
	if(it != m_mapBear.end()){
		addr = it->second;
		return TRUE;
	}
	return FALSE;
}

BOOL CDialogController::clearBear(const string &uniqID){
	int n = m_mapBear.erase(uniqID);
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



