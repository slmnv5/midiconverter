#ifndef KBDPORT_H
#define KBDPORT_H

#include "pch.hpp"
#include "MidiEvent.hpp"

std::string findKbdEvent();
std::string getInputDevicePath();


class KbdPort {
private:
  std::map<int, int> kbdMap;
  std::string dev;
  int fd;

public:
  KbdPort(const char* kbdMapFile);
  virtual ~KbdPort() {
  }
  int get_input_event(MidiEvent& ev);
private:
  void parse_string(const std::string& s);
  void parse_file(const char* kbdMapFile);


};

#endif