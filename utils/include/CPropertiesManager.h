/*********************************************************************
 * Copyright (c)2010-2012, by BUPT
 * All rights reserved.

 * FileName:        CPropertiesManager.h
 * System:          samples
 * SubSystem:       utils
 * Author:          wangchao
 * Date:            2010.11.1
 * Version:         1.0
 * Description:
      Manager of all properties.

 *
 * Last Modified:
     2010-11-01  初始版本
            By Wang Chao.

*******************************************************************************/
#ifndef CPROPERTIESMANAGER_H_
#define CPROPERTIESMANAGER_H_
#include "UtilsCommon.h"
#include "CProperties.h"
_CLASSDEF(CPropertiesManager)
/**
 * Manager of Properties from config files.
 * @ingroup utils
 */
class CPropertiesManager {
public:
	/*
	 * get the singleton instance of PropertiesManager
	 */
	static CPropertiesManager * getInstance();
	/*
	 * get a instance of Properties
	 *
	 */
	CProperties* getProperties(const char* fileName);

	/*
	 * destroy Properties
	 */
	virtual ~CPropertiesManager();
private:
	CPropertiesManager();

	map<string,CProperties*> m_map;
};

#endif /* CPROPERTIESMANAGER_H_ */
