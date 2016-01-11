/*********************************************************************
 * Copyright (c)2010-2012, by BUPT
 * All rights reserved.

 * FileName:        CPropertiesManager.C
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
#include "CPropertiesManager.h"

CPropertiesManager::CPropertiesManager() {
	m_map.clear();
}
/*
 * 析构所有创建的Properties对象
 */
CPropertiesManager::~CPropertiesManager() {
	map<string,CProperties*>::iterator it= m_map.begin();
	for(;it !=m_map.end();it++){
		delete it->second;
	}
}
/*
 * get the singleton instance of PropertiesManager
 * the PropertiesManager can only exist one instance
 * which used for the whole program
 * static method
 */
CPropertiesManager * CPropertiesManager::getInstance()
{
    static CPropertiesManager *p ;
    if (p == NULL)
    {
        p = new CPropertiesManager();
    }
    return (p);
}
/*
 * 获得配置文件对象 Properties
 * 若请求的配置文件不存在，将导致程序退出
 * 参数fileName $UNIDIR/etc/目录下的配置文件名 如 samples.env
 */
CProperties * CPropertiesManager::getProperties(const char* fileName){
	map<string,CProperties*>::iterator it= m_map.find(fileName);
	CProperties * result =NULL;
	//不存在此配置文件
	if(it == m_map.end()){
		result = new CProperties(fileName);
		m_map[fileName] = result;
	}
	else{
		result = it->second;
	}
	return result;
}



