#ifndef MOUSEPORT_H
#define MOUSEPORT_H

#include "pch.hpp"
#include "MidiEvent.hpp"

std::string findMouseEvent();
std::string getInputDevicePath();


class MousePort {
private:
  std::map<int, int> kbdMap;
  std::string dev;
  int fd;

public:
  MousePort(const char* kbdMapFile);
  virtual ~MousePort() {
  }
  bool get_input_event(MidiEvent& ev);
private:
  void parse_string(const std::string& s);
  void parse_file(const char* kbdMapFile);


};

#endif