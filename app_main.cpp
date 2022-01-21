#include "pch.hpp"
#include "MidiEvent.hpp"
#include "RuleMapper.hpp"
#include "utils.hpp"
#include "log.hpp"
#include "MidiClient.hpp"

using namespace std;

void help();

int main(int argc, char *argv[]) {

	char *ruleFile = nullptr;
	char *countFile = nullptr;
	char *clientName = nullptr;

	LOG::ReportingLevel() = LogLvl::ERROR;
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-r") == 0 && i + 1 <= argc) {
			ruleFile = argv[i + 1];
			LOG(LogLvl::INFO) << "Loaded file: " << ruleFile;
		} else if (strcmp(argv[i], "-c") == 0) {
			countFile = argv[i + 1];
		} else if (strcmp(argv[i], "-n") == 0) {
			clientName = argv[i + 1];
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
		MidiClient *mf = nullptr;
		const char *clName = clientName == nullptr ? "mimap" : clientName;
		if (ruleFile != nullptr) {
			LOG(LogLvl::INFO) << "Start rule processing";
			mf = new MidiFilterRule(clName, ruleFile);
		} else if (countFile != nullptr) {
			LOG(LogLvl::INFO) << "Start count processing";
			mf = new MidiFilterCount(clName, countFile);
		}
		LOG(LogLvl::INFO)
				<< "Opening MIDI ports. Use 'aconnect' to see ports and connect to them";
		mf->open_alsa_connection();
		LOG(LogLvl::INFO) << "Starting MIDI messages processing";
		mf->process_events(88888888);
	} catch (exception &err) {
		LOG(LogLvl::ERROR) << "! Completed with error !" << err.what();
	}
}

void help() {
	cout
			<< "Usage: mimap3 <-r | -c file> [options] \n"
					"  -r <file> load file for MIDI mapping, see rules.txt for details\n"
					"  -c <file> load file for MIDI counting, see count.txt for details\n"
					"	options:\n"
					"  -h displays this info\n"
					"  -n [name] MIDI client name\n"
					"  -v verbose output\n"
					"  -vv more verbose\n"
					"  -vvv even more verbose\n";
	exit(0);
}

