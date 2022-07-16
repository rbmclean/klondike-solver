#include <string>

#ifndef DISABLE_LOGGING

void setLogLevel(unsigned char level);
bool wouldLog(unsigned char level);
void log(unsigned char level, std::string msg, bool error = false);

#else

#define setLogLevel(level) (void)0
#define wouldLog(level) false
#define log(...) (void)0

#endif
