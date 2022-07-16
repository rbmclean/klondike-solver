#include <iostream>
#include "logger.h"

#ifndef DISABLE_LOGGING

static unsigned char logLevel = 0;

void setLogLevel(unsigned char level){
	logLevel = level;
}

//if producing the msg might be expensive, this can be called
//to determine whether to produce the msg, followed by calling log()
bool wouldLog(unsigned char level){
	return level <= logLevel;
}

void log(unsigned char level, std::string msg, bool error){
	if(level <= logLevel){
		if(error)
			std::cerr << msg;
		else
			std::cout << msg;
	}
}

#endif
