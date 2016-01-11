/*********************************************************************
 * Copyright (c)2010-2012, by BUPT
 * All rights reserved.

 * FileName:        CTUniNetMsgHelper.h
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
#ifndef CTUNINETMSGHELPER_H_
#define CTUNINETMSGHELPER_H_
#include "UtilsCommon.h"
#include "mcf/msgdef_uninet.h"
#include "mcf/infoext.h"
/**
 * Helper to deal with TUniNetMsg.
 * @ingroup utils
 */
class CTUniNetMsgHelper {
public:
	static void print(TUniNetMsg * msg);
	static string toString(TUniNetMsg * msg);
private:
	CTUniNetMsgHelper();
	virtual ~CTUniNetMsgHelper();
};

#endif /* CTUNINETMSGHELPER_H_ */
