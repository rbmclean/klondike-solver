#include <iostream>
#include "logger.h"

static unsigned char logLevel = 0;

void setLogLevel(unsigned char level){
	#ifndef DISABLE_LOGGING
	logLevel = level;
	#endif
}

//if producing the msg might be expensive, this can be called
//to determine whether to produce the msg, followed by calling log()
bool wouldLog(unsigned char level){
	#ifndef DISABLE_LOGGING
	return level <= logLevel;
	#else
	return false;
	#endif
}

void log(unsigned char level, std::string msg, bool error){
	#ifndef DISABLE_LOGGING
	if(level <= logLevel){
		if(error)
			std::cerr << msg;
		else
			std::cout << msg;
	}
	#endif
}
