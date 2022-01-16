#ifndef LOG_H
#define LOG_H

#include <iostream>

using namespace std;

enum class loglevel {
	DEBUG, INFO, WARN, ERROR
};

class LOG {
public:
	static loglevel gl_level;
	LOG() :
			msglevel(loglevel::DEBUG) {
	}
	LOG(loglevel lvl) :
			msglevel(lvl) {
	}
	~LOG() {
		cout << endl;
	}
	template<class T>
	LOG& operator<<(const T &msg) {
		if (msglevel >= LOG::gl_level) {
			cout << msg;
		}
		return *this;
	}
private:
	loglevel msglevel;
};

#endif  /* LOG_H */
