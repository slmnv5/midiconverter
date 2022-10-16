#include "pch.hpp"
#include "MidiEvent.hpp"
#include "RuleMapper.hpp"
#include "utils.hpp"
#include "MidiClient.hpp"
#include "MidiConverter.hpp"
#include "MousePort.hpp"

using namespace std;

void help();

int main(int argc, char* argv[]) {

	const char* ruleFile = nullptr;
	const char* clientName = nullptr;
	const char* kbdMapFile = nullptr;
	const char* sourceName = nullptr;
	LOG::ReportingLevel() = LogLvl::ERROR;

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
			sourceName = argv[i + 1];
		}
		else if (strcmp(argv[i], "-r") == 0 && i + 1 < argc) {
			ruleFile = argv[i + 1];
		}
		else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
			clientName = argv[i + 1];
		}
		else if (strcmp(argv[i], "-k") == 0 && i + 1 < argc) {
			kbdMapFile = argv[i + 1];
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
			return 0;
		}
	}
	if (ruleFile == nullptr) {
		help();
		return 2;
	}
	if (sourceName == nullptr && kbdMapFile == nullptr) {
		help();
		return 2;
	}
	if (clientName == nullptr)
		clientName = "mimap";

	LOG(LogLvl::INFO) << "MIDI client name: " << clientName;
	LOG(LogLvl::INFO) << "Rule file: " << ruleFile;
	RuleMapper* ruleMapper = nullptr;
	MidiClient* midiClient = nullptr;
	MousePort* mousePort = nullptr;


	try {
		if (kbdMapFile != nullptr) {
			midiClient = new MidiKbdClient(clientName, kbdMapFile);
			LOG(LogLvl::INFO) << "Using typing keyboard as source with map: " << kbdMapFile;
		}
		else {
			midiClient = new MidiClient(clientName, sourceName);
			LOG(LogLvl::INFO) << "Using midi port as source: " << sourceName;
		}

		ruleMapper = new RuleMapper(ruleFile, midiClient);
		mousePort = new MousePort();

		MidiConverter midiConverter = MidiConverter(ruleMapper, midiClient, mousePort);

		LOG(LogLvl::INFO) << "Starting MIDI messages processing";
		midiConverter.process_events();
	}
	catch (exception& e) {
		LOG(LogLvl::ERROR) << "Completed with error: " << e.what();
		return 1;
	}
}

void help() {
	cout << "Usage: mimap5 -r <file> [options] \n"
		"  -r <ruleFile> load file with rules, see rules.txt for details and example\n"
		"  -i <sourceName> MIDI source to connect to\n"
		"options:\n"
		"  -k <kbdMapFile> use typing keyboard for MIDI notes, needs sudo\n"
		"  -n [name] output MIDI port name to create\n"
		"  -v verbose output\n"
		"  -vv more verbose\n"
		"  -vvv even more verbose\n"
		"  -h displays this info\n";
}
