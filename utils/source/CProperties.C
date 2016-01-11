/*********************************************************************
 * Copyright (c)2010-2012, by BUPT
 * All rights reserved.

 * FileName:        CProperties.C
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

#include "CProperties.h"
#include "UtilsCommon.h"
/*
 * private initialise func
 * get properties from $UNIDIR/etc/fileName.env
 * without this file, program will exit
 */
CProperties::CProperties(const char *fileName)
{
    pMap.clear();
    char* uniDir = getenv("UNIDIR");
    char path[CHAR_MAX_LEN] = {0};

    //output(3, "UNIDIR is %s\n", uniDir);

    if (uniDir == NULL){
        strcat(path, "."); // use "." as UNIDIR
    }
    else{
        strcat(path, uniDir);
    }
    strcat(path,"/etc/");
    strcat(path, fileName);

    FILE *fp;
    char line[CHAR_MAX_LEN];
    char key[CHAR_MAX_LEN];
    char value[CHAR_MAX_LEN];
    if ((fp = fopen(path, "r")) > 0)
    {// $UNIDIR/etc/m_fileName is existed
        while (fgets(line, CHAR_MAX_LEN-1, fp))
        {
        	if(line[0]=='#') continue;
            sscanf(line, "%s %s", key, value);
            pMap[key] = value;
        }

        fclose(fp);
    }
    else
    {// $UNIDIR/m_fileName is not existed
        printf("ERROR: config file not existed at %s!\n", path);
    	exit(0);
    }
}

CProperties::~CProperties()
{
    pMap.clear();
}
/**
 * get property with the key .
 */
string CProperties::getProperty(const char* key)
{
    if (pMap.find(key) == pMap.end())
        return "";
    else
        return pMap[key];
}

/*
 * get integer property with the key
 * when the value is not a int
 * or the key is not existed, return -1
 */
int CProperties::getIntProperty(const char* key)
{
    if (pMap.find(key) == pMap.end())
    {// the key-value pair not existed
        return -1;
    }
    else
    {// existed
        string str = pMap[key];
        int num = 0;
        for (unsigned int i = 0; i < str.length(); i++)
        {
            if (str[i] >= '0' && str[i] <= '9')
                num = num * 10 + str[i] - '0';
            else
            {// the value contains not digit
                return -1;
            }
        }
        return num;
    }
}

/*
 * get all the properties
 * format is
 * "key1\t value1\n key2\t value2\n..."
 */
string CProperties::getProperties()
{
    string ps = "";
    for (map<string, string>::iterator iter = pMap.begin(); iter != pMap.end(); iter++)
    {
        ps += iter->first + '\t' + iter->second + '\n';
    }
    return ps;
}

/*
 * set property
 * if key existed, cover the value;
 * if key not existed, it will make a
 */
void CProperties::setProperty(const char* key, const char* value)
{
    pMap[key] = value;
}

