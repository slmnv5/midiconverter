#ifndef MIDICLIENT_H
#define MIDICLIENT_H
#include "pch.hpp"



class MidiClient
{
protected:
	int client = -1;
	int inport = -1;
	int outport = -1;
	snd_seq_t* seq_handle = nullptr;

public:
	MidiClient(const char* clientName, const char* srcName, const char* dstName)
	{
		open_alsa_connections(clientName, srcName, dstName);
	}
	virtual ~MidiClient()
	{
	}
	void send_event(snd_seq_event_t* event) const;
	snd_seq_event_t* get_input_event() const;

protected:
	virtual void open_alsa_connections(const char* clientName, const char* srcName, const char* dstName);
	int find_midi_client(const std::string& name_part, unsigned int capability, int& cli_id, int& cli_port);
	void subscribe(const char* name_part, bool is_input);
};

#endif
