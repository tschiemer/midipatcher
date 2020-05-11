#ifndef MAIN_H
#define MAIN_H

#include <Port/MidiIn.hpp>
#include <Port/MidiOut.hpp>
#include <Port/VirtMidiIn.hpp>
#include <Port/VirtMidiOut.hpp>
#include <Port/FileIn.hpp>

MidiPatcher::AbstractPort::PortClassRegistryInfo* PortClassRegistryInfoConfig[] = {
  MidiPatcher::Port::MidiIn::getPortClassRegistryInfo(),
  MidiPatcher::Port::MidiOut::getPortClassRegistryInfo(),
  MidiPatcher::Port::VirtMidiIn::getPortClassRegistryInfo(),
  MidiPatcher::Port::VirtMidiOut::getPortClassRegistryInfo(),
  MidiPatcher::Port::FileIn::getPortClassRegistryInfo()
};

#endif /* MAIN_H */
