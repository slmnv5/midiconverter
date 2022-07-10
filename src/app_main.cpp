#include "pch.hpp"
#include "MidiEvent.hpp"
#include "RuleMapper.hpp"
#include "utils.hpp"
#include "log.hpp"
#include "MidiClient.hpp"

using namespace std;

void help();

int main(int argc, char* argv[]) {

	char const* ruleFile = nullptr;
	char const* clientName = nullptr;
	char const* kbdMapFile = nullptr;

	LOG::ReportingLevel() = LogLvl::ERROR;
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-r") == 0) {
			ruleFile = argv[i + 1];
			LOG(LogLvl::INFO) << "Rule file: " << ruleFile;
		}
		else if (strcmp(argv[i], "-n") == 0) {
			clientName = argv[i + 1];
		}
		else if (strcmp(argv[i], "-k") == 0) {
			kbdMapFile = argv[i + 1];
			LOG(LogLvl::INFO) << "Keyboard map file: " << kbdMapFile;
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

		if (clientName == nullptr)
			clientName = "mimap";

		LOG(LogLvl::INFO) << "Start rule processing";
		MidiClient midiClient = MidiClient(clientName);
		MidiConverter midiConverter = MidiConverter(ruleFile, midiClient);
		LOG(LogLvl::WARN) << endl << "Loaded rules:" << endl
			<< midiConverter.toString();

		if (kbdMapFile != nullptr) {
			KbdPort kbdPort = KbdPort(kbdMapFile);
			kbdPort.start(&midiConverter);
			LOG(LogLvl::INFO) << "Using typing keyboard for MIDI input with map: " << kbdMapFile;
		}

		LOG(LogLvl::INFO) << "Starting MIDI messages processing";
		midiConverter.process_events();
	}
	catch (exception& err) {
		LOG(LogLvl::ERROR) << "! Completed with error !" << err.what();
	}
}

void help() {
	cout << "Usage: mimap5 -r <file> [options] \n"
		"  -r <file> load file with rules, see rules.txt for details\n"
		"options:\n"
		"  -k <kbdMapFile> -- use typing keyboard for MIDI notes\n"
		"  -n [name] -- MIDI port name (mimap if missing)\n"
		"  -v -- verbose output\n"
		"  -vv -- more verbose\n"
		"  -vvv -- even more verbose\n"
		"  -h -- displays this info\n";

	exit(0);
}
