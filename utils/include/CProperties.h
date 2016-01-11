/*********************************************************************
 * Copyright (c)2010-2012, by BUPT
 * All rights reserved.

 * FileName:        CProperties.h
 * System:          samples
 * SubSystem:       utils
 * Author:          wangchao
 * Date:            2010.11.1
 * Version:         1.0
 * Description:
      System properties of samples.

 *
 * Last Modified:
     2010-11-01  初始版本
            By Wang Chao.

*******************************************************************************/
#ifndef CPROPERTIES_H_
#define CPROPERTIES_H_

#include "UtilsCommon.h"

_CLASSDEF(CProperties)
/**
 * 读取一个以键值对方式存储的文件中的配置信息.
 * 一行中的第一个字符为#时,注释单行
 * @ingroup utils
 */
class CProperties {
public:
	friend class CPropertiesManager;
	/*
	 * get property with the key
	 */
	string getProperty(const char* key);

	/*
	 * get integer property with the key
	 * when the value is not a int
	 * or the key is not existed, return -1
	 */
	int getIntProperty(const char* key);

	/*
	 * get all the properties
	 * format is
	 * "key1\t value1\n key2\t value2\n..."
	 */
	string getProperties();

	/*
	 * set property
	 * if key existed, cover the value;
	 * if key not existed, it will make a
	 */
	void setProperty(const char* key, const char* value);

	/*
	 * get the singleton instance of Properties
	 * the Properties can only existed alone
	 * which used for the whole program
	 */
	virtual ~CProperties();

private:
	/*
	 * private initialise func
	 * get properties from $UNIDIR/etc/mpc.env
	 */
	CProperties();
	CProperties(const char * fileName);
	CProperties & operator = (const CProperties&);

	map<string, string> pMap;
};

#endif /* CPROPERTIES_H_ */
