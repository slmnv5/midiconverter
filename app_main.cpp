#include "pch.hpp"
#include "MidiEvent.hpp"
#include "MidiFilter.hpp"
#include "RuleMapper.hpp"
#include "utils.hpp"
#include "log.hpp"

using namespace std;

void help();

int main(int argc, char *argv[]) {

	char *ruleFile = nullptr;
	char *countFile = nullptr;
	LOG::ReportingLevel() = LogLvl::ERROR;
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-r") == 0 && i + 1 <= argc) {
			ruleFile = argv[i + 1];
			LOG(LogLvl::INFO) << "Loaded file: " << ruleFile;
		} else if (strcmp(argv[i], "-c") == 0) {
			countFile = argv[i + 1];
		} else if (strcmp(argv[i], "-v") == 0) {
			LOG::ReportingLevel() = LogLvl::WARN;
		} else if (strcmp(argv[i], "-vv") == 0) {
			LOG::ReportingLevel() = LogLvl::INFO;
		} else if (strcmp(argv[i], "-vvv") == 0) {
			LOG::ReportingLevel() = LogLvl::DEBUG;
		} else if (strcmp(argv[i], "-h") == 0) {
			help();
			exit(0);
		}
	}
	if ((ruleFile == nullptr && countFile == nullptr)
			|| (ruleFile != nullptr && countFile != nullptr)) {
		help();
		exit(1);
	}

	try {
		MidiFilter *mf = nullptr;
		if (ruleFile != nullptr) {
			LOG(LogLvl::INFO) << "Start rule processing";
			mf = new MidiFilterRule(ruleFile);
		} else if (countFile != nullptr) {
			LOG(LogLvl::INFO) << "Start count processing";
			mf = new MidiFilterCount(countFile);
		}
		LOG(LogLvl::INFO) << "Open MIDI ports";
		mf->open_alsa_connection();
		LOG(LogLvl::INFO) << "Start MIDI messages processing";
		mf->process_events(88888888);
	} catch (exception &err) {
		LOG(LogLvl::ERROR) << "! Completed with error !" << err.what();
	}
}

void help() {
	cout
			<< "Usage: mimap3 -f <rules_file> [options] \n"
					"  -r <rules_file> load file for MIDI mapping, see file_rules.txt for details\n"
					"  -c <count_file> load file for MIDI count, see file_count.txt for details\n"
					"  -h displays this info\n"
					"  -v verbose output\n"
					"  -vv more verbose\n"
					"  -vvv even more verbose\n";
	exit(0);
}

