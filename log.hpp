#ifndef __LOG1_H__
#define __LOG1_H__

#include <iostream>

using std::cout;

enum class loglevel {
	ERROR, WARN, INFO, DEBUG
};

class Log {
public:
	Log();
	virtual ~Log();
	std::ostream& Get(loglevel level = loglevel::INFO);
public:
	static loglevel& ReportingLevel();
	static std::string ToString(loglevel level);
	static loglevel FromString(const std::string &level);
protected:

private:
	Log(const Log&);
	Log& operator =(const Log&);
};

inline Log::Log() {
}

inline std::ostream& Log::Get(loglevel level) {
	cout << ToString(level) << ": ";
	int k = (int) level > (int) loglevel::DEBUG ?
			(int) level - (int) loglevel::DEBUG : 0;
	cout << std::string(k, '\t');
	return cout;
}

inline Log::~Log() {
	cout << std::endl;
	cout.flush();
}

inline loglevel& Log::ReportingLevel() {
	static loglevel reportingLevel = loglevel::DEBUG;
	return reportingLevel;
}

inline std::string Log::ToString(loglevel level) {
	static const char *const buffer[] = { "ERROR", "WARN", "INFO", "DEBUG" };
	return buffer[(int) level];
}

typedef Log LOG;

#define LOG(level) \
    if (level > LOG::ReportingLevel()) ; \
    else Log().Get(level)

#endif
