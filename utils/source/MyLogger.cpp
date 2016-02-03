#include "MyLogger.h"

boost::mutex lock;

MyLogger::MyLogger(const char * configPath, const char * appender) {
	log4cxx::xml::DOMConfigurator::configureAndWatch(configPath, 5000);
	logger = log4cxx::Logger::getLogger(appender);
}

MyLogger& MyLogger::getInstance(const char * configPath,
		const char * appender) {
	lock.lock();
	static MyLogger instance(configPath, appender);
	lock.unlock();

	return instance;
}

