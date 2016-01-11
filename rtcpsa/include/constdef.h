#ifndef _CONSTDEF_H
#define _CONSTDEF_H
// for roap_type
const int ROAP_OFFER = 1;
const int ROAP_ANSWER = 2;
const int ROAP_OK = 3;
const int ROAP_CANDIDATE = 4;
const int ROAP_SHUTDOWN = 5;
const int ROAP_ERROR = 6;
const int ROAP_MESSAGE = 7;
const int ROAP_INFO	= 8;
const int ROAP_UPDATE = 9;


// for error type
const int ERROR_OFFLINE = 1;
const int ERROR_TIMEOUT = 2;
const int ERROR_NOMATCH = 3;
const int ERROR_REFUSED = 4;
const int ERROR_CONFLICT = 5;
const int ERROR_DOUBLECONFLICT = 6;
const int ERROR_MEDIAFAILED = 7;
const int ERROR_FAILED = 8;
const int ERROR_NOSIP = 9;
#endif
