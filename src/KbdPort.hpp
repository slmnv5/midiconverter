#ifndef KBDPORT_H
#define KBDPORT_H

#include "pch.hpp"
#include "MidiClient.hpp"
 

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

std::string getInputDevicePath() {
  return "/dev/input/event" + findKbdEvent();
}

class KbdPort {
private:
  std::map<int, midi_byte_t> kbdMap;
  std::string dev;
  int fd;
  MidiClient* midi_client;

public:
  KbdPort(const char* kbdMapFile);
  virtual ~KbdPort() {}
  void start(MidiClient* mc);
private:
  void parse_string(const string& s);
  void parse_file(const char* kbdMapFile);
  void readKbd();

};

#endif