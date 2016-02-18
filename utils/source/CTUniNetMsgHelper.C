/*********************************************************************
 * Copyright (c)2010-2012, by BUPT
 * All rights reserved.

 * FileName:        CTUniNetMsgHelper.C
 * System:          samples
 * SubSystem:       utils
 * Author:          wangchao
 * Date:            2010.11.1
 * Version:         1.0
 * Description:
      Helper to deal with TUniNetMsg

 *
 * Last Modified:
     2010-11-01  初始版本
            By Wang Chao.

*******************************************************************************/
#include "CTUniNetMsgHelper.h"

CTUniNetMsgHelper::CTUniNetMsgHelper() {


}

CTUniNetMsgHelper::~CTUniNetMsgHelper() {

}
/**
 * 输出一个TUniNetMsg的全部信息.
 * 这个方法仅建议调试时使用
 */
void CTUniNetMsgHelper::print(TUniNetMsg * msg){
	ostrstream st;
	msg->print(st);

	//printf("%s\n",st.str());
	//print("%s",st.str());
	//term("%s",st.str());
	st.freeze(0);
	delete st.str();
}

string CTUniNetMsgHelper::toString(TUniNetMsg * msg){
	ostrstream st;
	msg->print(st);

	string res =  st.str();
	//print("%s",st.str());
	//term("%s",st.str());
	st.freeze(0);
	delete st.str();
	return res;
}
