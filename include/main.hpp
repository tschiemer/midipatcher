#ifndef MAIN_H
#define MAIN_H

#include <Port/MidiIn.hpp>
#include <Port/MidiOut.hpp>
#include <Port/VirtMidiIn.hpp>
#include <Port/VirtMidiOut.hpp>
#include <Port/FileIn.hpp>
#include <Port/FileOut.hpp>
#include <Port/UdpIn.hpp>

MidiPatcher::AbstractPort::PortClassRegistryInfo* PortClassRegistryInfoConfig[] = {
  MidiPatcher::Port::MidiIn::getPortClassRegistryInfo(),
  MidiPatcher::Port::MidiOut::getPortClassRegistryInfo(),
  MidiPatcher::Port::VirtMidiIn::getPortClassRegistryInfo(),
  MidiPatcher::Port::VirtMidiOut::getPortClassRegistryInfo(),
  MidiPatcher::Port::FileIn::getPortClassRegistryInfo(),
  MidiPatcher::Port::FileOut::getPortClassRegistryInfo(),
  MidiPatcher::Port::UdpIn::getPortClassRegistryInfo()
};

#define VERSION "v1.0.0"

#endif /* MAIN_H */
