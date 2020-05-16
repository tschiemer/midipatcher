#ifndef MAIN_H
#define MAIN_H

#include <midipatcher.hpp>

MidiPatcher::AbstractPort::PortClassRegistryInfo* PortClassRegistryInfoConfig[] = {
  MidiPatcher::Port::MidiIn::getPortClassRegistryInfo(),
  MidiPatcher::Port::MidiOut::getPortClassRegistryInfo(),
  MidiPatcher::Port::VirtMidiIn::getPortClassRegistryInfo(),
  MidiPatcher::Port::VirtMidiOut::getPortClassRegistryInfo(),
  MidiPatcher::Port::FileIn::getPortClassRegistryInfo(),
  MidiPatcher::Port::FileOut::getPortClassRegistryInfo(),
  MidiPatcher::Port::UdpIn::getPortClassRegistryInfo(),
  MidiPatcher::Port::UdpOut::getPortClassRegistryInfo(),
  MidiPatcher::Port::RawExec::getPortClassRegistryInfo()
};

#endif /* MAIN_H */
