#include "pch.hpp"
#include "MidiClient.hpp"
#include "lib/utils.hpp"
#include <fcntl.h>
#include <linux/input.h>

void MidiClient::subscribe(const char* name_part, bool is_input)
{
	if (nullptr == name_part)
	{
		return;
	}

	int id, port, result;
	id = port = result = -1;
	unsigned int cp_read, cp_write;
	cp_read = SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ;
	cp_write = SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE;
	unsigned int capability = is_input ? cp_read : cp_write;
	result = find_midi_client(name_part, capability, id, port);
	if (result < 0)
	{
		throw std::runtime_error("Error finding source MIDI port: " + std::string(name_part));
	}

	result = is_input ? snd_seq_connect_from(seq_handle, inport, id, port) : snd_seq_connect_to(seq_handle, outport, id, port);
	if (result < 0)
	{
		throw new std::runtime_error("Cannot connect to port: " + std::to_string(id) + ":" + std::to_string(port));
	}
	LOG(LogLvl::INFO) << "Connected to MIDI port: " << name_part << (is_input ? " input " : " output ") << id << ":" << port;
}

int MidiClient::find_midi_client(const std::string& name_part, unsigned int capability, int& cli_id, int& cli_port)
{
	cli_id = cli_port = -1;
	snd_seq_client_info_t* cinfo;
	snd_seq_port_info_t* pinfo;

	snd_seq_client_info_alloca(&cinfo);
	snd_seq_port_info_alloca(&pinfo);

	snd_seq_client_info_set_client(cinfo, -1);
	while (snd_seq_query_next_client(seq_handle, cinfo) >= 0)
	{
		int cl = snd_seq_client_info_get_client(cinfo);
		std::string cl_name(snd_seq_client_info_get_name(cinfo));
		if (cl_name.find(name_part) == std::string::npos)
			continue;

		snd_seq_port_info_set_client(pinfo, cl);
		snd_seq_port_info_set_port(pinfo, -1);
		unsigned int capability = 0;
		while (snd_seq_query_next_port(seq_handle, pinfo) >= 0)
		{
			// port must understand MIDI messages
			capability = SND_SEQ_PORT_TYPE_MIDI_GENERIC;
			if ((snd_seq_port_info_get_type(pinfo) & capability) != capability)
				continue;
			// we need capability
			if ((snd_seq_port_info_get_capability(pinfo) & capability) != capability)
				continue;
			cli_id = snd_seq_port_info_get_client(pinfo);
			cli_port = snd_seq_port_info_get_port(pinfo);
			LOG(LogLvl::INFO) << "Found MIDI: " << name_part << " -- " << cli_id << ":" << cli_port;
			return 0;
		}
	}
	return -1;
}

void MidiClient::open_alsa_connections(const char* clientName, const char* srcName, const char* dstName)
{
	const std::string clName = std::string(clientName).substr(0, 15);
	const std::string inPortName = clName + "_in";
	const std::string outPortName = clName + "_out";

	if (snd_seq_open(&seq_handle, "default", SND_SEQ_OPEN_DUPLEX, 0) < 0)
		throw std::runtime_error("Error opening ALSA seq_handle");

	snd_seq_set_client_name(seq_handle, clName.c_str());
	client = snd_seq_client_id(seq_handle);

	inport = snd_seq_create_simple_port(seq_handle, inPortName.c_str(),
		SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE,
		SND_SEQ_PORT_TYPE_APPLICATION);
	if (inport < 0)
		throw std::runtime_error("Error creating virtual IN port");

	outport = snd_seq_create_simple_port(seq_handle, outPortName.c_str(),
		SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ,
		SND_SEQ_PORT_TYPE_APPLICATION);
	if (outport < 0)
		throw std::runtime_error("Error creating virtual OUT port");

	LOG(LogLvl::INFO) << "MIDI ports created: IN=" << client << ":" << inport << " OUT="
		<< client << ":" << outport;

	subscribe(srcName, true);
	subscribe(dstName, false);
}

void MidiClient::send_event(snd_seq_event_t* event) const
{
	snd_seq_ev_set_direct(event);
	snd_seq_ev_set_subs(event);
	snd_seq_ev_set_source(event, outport);
	snd_seq_event_output_direct(seq_handle, event);
}

snd_seq_event_t* MidiClient::get_input_event() const {
	snd_seq_event_t* event = nullptr;
	int result = snd_seq_event_input(seq_handle, &event);
	if (result < 0) {
		LOG(LogLvl::WARN) << "Possible loss of MIDI event";
	}
	return event;
}
