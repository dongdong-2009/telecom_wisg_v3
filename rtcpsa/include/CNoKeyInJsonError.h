/*********************************************************************
 * FileName:       CNoKeyInJsonError.h
 * System:         signal_gateway
 * SubSystem:	   rtc_psa
 * Author:         liumingshuan
 * Date:           2013-04-01
 * Version:        1.0
 * Description:	   we use json to present ROAP, if cannot find some "key" in json, throw this kind of exception
 * Last Modified:
 	 	 	 	   2013-04-03
*******************************************************************************/
#ifndef _NO_KEY_IN_JSON_ERROR_H
#define _NO_KEY_IN_JSON_ERROR_H
#include <iostream>
#include <stdexcept>
#include <exception>
using namespace std;

class noKey_inJson_error: public std::runtime_error{
public:
	explicit noKey_inJson_error(const std::string& msg):
		std::runtime_error(msg){
			m_result.append(runtime_error::what());
		}

	noKey_inJson_error(const std::string& msg, const std::string& key, const std::string& plainStr):
		std::runtime_error(msg), m_key(key), m_plainStr(plainStr){
			m_result.append(runtime_error::what());
			m_result.append("(key = ").append(m_key).append(", json = ").append(m_plainStr).append(")");
		}

	noKey_inJson_error(const std::string& key, const std::string& json):
		std::runtime_error("can not find key in json"), m_key(key), m_plainStr(json){
			m_result.append(runtime_error::what());
			m_result.append("(key = ").append(m_key).append(", json = ").append(m_plainStr).append(")");
		}

	friend ostream& operator<<(ostream& out, const noKey_inJson_error& e){
		out<<e.what();
		return out;
	}

 	const char *what() const throw (){
		return m_result.c_str();
 	}

	virtual ~noKey_inJson_error() throw(){}

private:
	const std::string m_key, m_plainStr;
	std::string m_result;
};

#endif
