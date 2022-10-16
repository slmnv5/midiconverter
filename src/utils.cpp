#include "pch.hpp"
#include "utils.hpp"



//==================== utility functions ===================================


std::string findKbdEvent() {
	const char* cmd = "grep -E 'Handlers|EV=' /proc/bus/input/devices | "
		"grep -B1 'EV=120013' | grep -Eo 'event[0-9]+' | grep -Eo '[0-9]+' | tr -d '\n'";

	FILE* pipe = popen(cmd, "r");
	char buffer[128];
	std::string result = "";
	while (!feof(pipe))
		if (fgets(buffer, 128, pipe) != NULL)
			result += buffer;
	pclose(pipe);
	return result;
}

std::string findTouchScreenEvent() {
	const char* cmd = "grep -E 'Handlers|EV=' /proc/bus/input/devices | "
		"grep -B1 'EV=b' | grep -Eo 'event[0-9]+' | grep -Eo '[0-9]+' | tr -d '\n'";

	FILE* pipe = popen(cmd, "r");
	char buffer[128];
	std::string result = "";
	while (!feof(pipe))
		if (fgets(buffer, 128, pipe) != NULL)
			result += buffer;
	pclose(pipe);
	return result;
}




std::vector<std::string> split_string(const std::string& s, const std::string& delimiter) {
	std::vector<std::string> tokens;
	auto start = 0U;
	auto stop = s.find(delimiter);
	while (stop != std::string::npos) {
		tokens.push_back(s.substr(start, stop - start));
		start = stop + delimiter.length();
		stop = s.find(delimiter, start);
	}
	tokens.push_back(s.substr(start, stop));
	return tokens;
}

int replace_all(std::string& s, const std::string& del, const std::string& repl) {
	std::string::size_type delsz = del.size();
	if (delsz == 0)
		return 0;
	int count = 0;
	std::string::size_type n = 0;
	while ((n = s.find(del, n)) != std::string::npos) {
		s.replace(n, delsz, repl);
		n += repl.size();
		count++;
	}
	return count;
}

void remove_spaces(std::string& s) {
	s = s.substr(0, s.find(";"));
	replace_all(s, " ", "");
	replace_all(s, "\n", "");
	replace_all(s, "\t", "");
}

std::string exec_command(const std::string& cmd) {
	char buffer[128];
	std::string result = "";
	FILE* pipe = popen(cmd.c_str(), "r");
	if (!pipe)
		throw std::runtime_error("popen() failed!");
	try {
		while (fgets(buffer, sizeof buffer, pipe) != NULL) {
			result += buffer;
		}
	}
	catch (std::exception& e) {
		pclose(pipe);
		throw e;
	}
	pclose(pipe);
	return result;
}


