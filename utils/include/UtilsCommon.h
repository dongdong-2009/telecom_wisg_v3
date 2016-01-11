/*
 * UtilsCommon.h
 *
 *  Created on: 2010-11-4
 *      Author: wangchao
 */

#ifndef UTILSCOMMON_H_
#define UTILSCOMMON_H_
#include<stdlib.h>
#include<string.h>
#include<time.h>

#include<iostream>
#include<fstream>
#include<string>
#include<map>
#include<vector>

#include "mcf/defs.h"
using namespace std;

#define CHAR_MAX_LEN 8192    // 8kB the max of socket udp default
#define ADDR_MAX_LEN 16     // 255.255.255.255 could only use 16 chars
#define COMMAND_MAX_LEN CHAR_MAX_LEN

#endif /* UTILSCOMMON_H_ */
