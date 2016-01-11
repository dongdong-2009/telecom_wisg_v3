#ifndef DIALOG_CONTROLLER_H
#define DIALOG_CONTROLLER_H
#include "msgdef_uninet.h"
#include "CMsgMapHelper.h"
#include <iostream>
#include <string>
#include <map>
using namespace std;



/*
 * 假设一个RTC会话由offersessionID唯一标识
 * 一个sip会话由callID.number唯一标识（offersessionID和callID.number不能相同）
 * 需要保证新的会话或者IM都能生成新的标识
 * 考虑：（1）这些ID怎么相互转换
 */
class CDialogController{
public:
	CDialogController();
	virtual ~CDialogController();

	BOOL storeDialog(const string &uniqID, TMsgAddress addr);
	BOOL getDialogAddr(const string &uniqID, TMsgAddress& addr);
	BOOL clearDialog(const string &uniqID);

private:
	// 存储的是标识会话的uniqid到UAC地址的映射
	map<string, TMsgAddress> m_mapDialog;

	void DubugOut();

};

#endif
