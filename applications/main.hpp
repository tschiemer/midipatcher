#ifndef MAIN_H
#define MAIN_H

#include <midipatcher.hpp>

#define DEFAULT_AUTOSCAN_INTERVAL       1000

#define DEFAULT_PATCHFILE               "default.patchfile"

#define DEFAULT_CLI_DELIMITER           " "

#define DEFAULT_CONTROL_PORT_IN         "VirtMidiIn:MidiPatcher-Control"
#define DEFAULT_CONTROL_PORT_OUT        "VirtMidiOut:MidiPatcher-Control"
#define DEFAULT_REMOTE_CONTROL_PORT_IN  "MidiIn:MidiPatcher-Control"
#define DEFAULT_REMOTE_CONTROL_PORT_OUT "MidiOut:MidiPatcher-Control"


MidiPatcher::AbstractPort::PortClassRegistryInfo* PortClassRegistryInfoConfig[] = {
  MidiPatcher::Port::MidiIn::getPortClassRegistryInfo(),
  MidiPatcher::Port::MidiOut::getPortClassRegistryInfo(),
  MidiPatcher::Port::VirtMidiIn::getPortClassRegistryInfo(),
  MidiPatcher::Port::VirtMidiOut::getPortClassRegistryInfo(),
  MidiPatcher::Port::FileIn::getPortClassRegistryInfo(),
  MidiPatcher::Port::FileOut::getPortClassRegistryInfo(),
  MidiPatcher::Port::UdpIn::getPortClassRegistryInfo(),
  MidiPatcher::Port::UdpOut::getPortClassRegistryInfo(),
  MidiPatcher::Port::RawExec::getPortClassRegistryInfo(),
  MidiPatcher::Port::MsgExec::getPortClassRegistryInfo(),
  MidiPatcher::Port::Serial::getPortClassRegistryInfo(),
};

#endif /* MAIN_H */
