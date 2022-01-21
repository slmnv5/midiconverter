#ifndef MIDIFILTER_H
#define MIDIFILTER_H
#include "pch.hpp"
#include "RuleMapper.hpp"
#include "NoteCounter.hpp"

#include <chrono>

using namespace std;

class MidiClient {
protected:
	int client = -1;
	int inport = -1;
	int outport = -1;
	snd_seq_t *seq_handle = nullptr;
	const string &clientName;
public:
	MidiClient(const string &clentName) :
			clientName(clentName) {
	}
	virtual ~MidiClient() {
	}

	void send_event(snd_seq_event_t *event) const;
	void send_new111(const MidiEvent &ev) const;
	void open_alsa_connection();
	void process_events(long count);
	virtual void process_one_event(snd_seq_event_t *event, MidiEvent &ev) = 0;
};
//=============== class that maps in event to out events ============================
class MidiFilterRule: public MidiClient {
private:
	const RuleMapper rule_mapper;

public:
	MidiFilterRule(const string &clientName, const string &ruleFile) :
			MidiClient(clientName), rule_mapper(ruleFile) {
	}
	virtual ~MidiFilterRule() {
	}

	virtual void process_one_event(snd_seq_event_t *event, MidiEvent &ev);
};

//========= class that convert CC to notes and count in events =======================
class MidiFilterCount: public MidiClient {
	/*Same as parent but converts CC to notes.
	 * When  CC value increases first time, it makes note ON and ignores next increases.
	 * When CC value decrease it makes note OFF and ignores next decreases.
	 * When new channel or CC number comes all is reset.
	 * Generated note number is same as CC number */

private:

	bool similar_and_fast(const MidiEvent &ev);
	void process_note(const MidiEvent &ev);

public:
	MidiFilterCount(const string &clientName, const string &fileName) :
			MidiClient(clientName), note_counter(fileName) {
	}
	~MidiFilterCount() {
	}
	virtual void process_one_event(snd_seq_event_t *event, MidiEvent &ev);
	void send_event_delayed(const MidiEvent &ev, int c_on);
};

#endif
