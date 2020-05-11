#ifndef MAIN_H
#define MAIN_H

#include <Port/MidiIn.hpp>
#include <Port/MidiOut.hpp>
#include <Port/VirtMidiIn.hpp>
#include <Port/VirtMidiOut.hpp>

MidiPatcher::AbstractPort::PortClassRegistryInfo* PortClassRegistryInfoConfig[] = {
  MidiPatcher::Port::MidiIn::getPortClassRegistryInfo(),
  MidiPatcher::Port::MidiOut::getPortClassRegistryInfo(),
  MidiPatcher::Port::VirtMidiIn::getPortClassRegistryInfo(),
  MidiPatcher::Port::VirtMidiOut::getPortClassRegistryInfo()
};

#endif /* MAIN_H */
