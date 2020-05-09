#ifndef MAIN_H
#define MAIN_H

#include <Port/MidiIn.hpp>
#include <Port/MidiOut.hpp>

MidiPatcher::AbstractPort::PortClassRegistryInfo* PortClassRegistryInfoConfig[] = {
  MidiPatcher::Port::MidiIn::getPortClassRegistryInfo(),
  MidiPatcher::Port::MidiOut::getPortClassRegistryInfo()
};

#endif /* MAIN_H */
