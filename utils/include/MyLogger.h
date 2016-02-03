#ifndef _MYLOGGER_H_
#define _MYLOGGER_H_

#include <boost/thread/mutex.hpp>
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/xml/domconfigurator.h>


class MyLogger{
private:
	MyLogger(const char * configPath, const char * appender);
	log4cxx::LoggerPtr logger;


public:
	static MyLogger& getInstance(const char * configPath, const char * appender);
	~MyLogger(){
		LOG4CXX_INFO(logger, "destroy MyLogger!");
	}

	inline log4cxx::LoggerPtr getLogger()
	{
		return logger;
	}

};

#endif
