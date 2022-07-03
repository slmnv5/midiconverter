#ifndef MIDICLIENT_H
#define MIDICLIENT_H
#include "pch.hpp"

#include "RuleMapper.hpp"

using namespace std;

class MidiClient {
protected:
	int client = -1;
	int inport = -1;
	int outport = -1;
	snd_seq_t* seq_handle = nullptr;
	const string& clientName;

public:
	MidiClient(const string& clentName) :
		clientName(clentName) {
	}
	virtual ~MidiClient() {
	}

	void send_event(snd_seq_event_t* event) const;
	void send_new(const MidiEvent& ev) const;
	void open_alsa_connection();
	void process_events(long count);
	virtual string toString() const {
		return "";
	}
	virtual void process_one_event(snd_seq_event_t* event, MidiEvent& ev) {
	}
};
//=============== class that maps in event to out events ============================

class MidiConverter : public MidiClient {
private:
	RuleMapper rule_mapper;

public:
	MidiConverter(const string& ruleFile, MidiClient& mc) :
		MidiClient(mc), rule_mapper(ruleFile, *this) {
	}
	virtual ~MidiConverter() {
	}
	virtual string toString() const {
		return rule_mapper.toString();
	}
	virtual void process_one_event(snd_seq_event_t* event, MidiEvent& ev);

};

#endif
