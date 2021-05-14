#include <string>

void setLogLevel(unsigned char level);
bool wouldLog(unsigned char level);
void log(unsigned char level, std::string msg, bool error = false);
