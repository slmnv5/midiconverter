#include "pch.hpp"
#include "MidiEvent.hpp"
#include "RuleMapper.hpp"
#include "utils.hpp"
#include "log.hpp"
#include "MidiClient.hpp"

using namespace std;

void help();

int main(int argc, char* argv[]) {

	char* ruleFile = nullptr;

	char* clientName = nullptr;

	LOG::ReportingLevel() = LogLvl::ERROR;
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-r") == 0 && i + 1 <= argc) {
			ruleFile = argv[i + 1];
			LOG(LogLvl::INFO) << "Loaded file: " << ruleFile;
		}
		else if (strcmp(argv[i], "-n") == 0) {
			clientName = argv[i + 1];
		}
		else if (strcmp(argv[i], "-v") == 0) {
			LOG::ReportingLevel() = LogLvl::WARN;
		}
		else if (strcmp(argv[i], "-vv") == 0) {
			LOG::ReportingLevel() = LogLvl::INFO;
		}
		else if (strcmp(argv[i], "-vvv") == 0) {
			LOG::ReportingLevel() = LogLvl::DEBUG;
		}
		else if (strcmp(argv[i], "-h") == 0) {
			help();
			exit(0);
		}
	}
	if (ruleFile == nullptr) {
		help();
		exit(1);
	}

	try {
		MidiClient* mf = nullptr;
		string clName = clientName == nullptr ? "mimap" : clientName;

		LOG(LogLvl::INFO) << "Start rule processing";
		mf = new MidiConverter(clName.substr(0, 15), ruleFile);
		LOG(LogLvl::WARN) << endl << "Loaded rules:" << endl
			<< mf->toString();

		LOG(LogLvl::INFO)
			<< "Opening MIDI ports. Use 'aconnect' to see ports and connect to them";
		mf->open_alsa_connection();
		LOG(LogLvl::INFO) << "Starting MIDI messages processing";
		mf->process_events(88888888);
	}
	catch (exception& err) {
		LOG(LogLvl::ERROR) << "! Completed with error !" << err.what();
	}
}

void help() {
	cout << "Usage: mimap5 -r <file> [options] \n"
		"  -r <file> load file with rules, see rules.txt for details\n"
		"options:\n"
		"  -h displays this info\n"
		"  -n [name] MIDI client name\n"
		"  -v verbose output\n"
		"  -vv more verbose\n"
		"  -vvv even more verbose\n";
	exit(0);
}
