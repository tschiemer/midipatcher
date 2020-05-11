#ifndef VIRT_MIDI_OUT_H
#define VIRT_MIDI_OUT_H

#include "../AbstractOutputPort.hpp"

#include <RtMidi.h>

namespace MidiPatcher {
  namespace Port {

    class VirtMidiOut : AbstractOutputPort {

    public:

      static const constexpr char * PortClass = "VirtMidiOut";

      std::string getPortClass(){
        return PortClass;
      }

      static AbstractPort* factory(PortRegistry * portRegistry, PortDescriptor * portDescriptor){
        assert( portDescriptor->PortClass == PortClass );
        return new VirtMidiOut(portRegistry, portDescriptor->Name);
      }

      static PortClassRegistryInfo * getPortClassRegistryInfo() {
        return new PortClassRegistryInfo(PortClass, factory, nullptr, nullptr, nullptr);
      }

      VirtMidiOut(PortRegistry * portRegistry, std::string portName);
      ~VirtMidiOut();

      void send(unsigned char *message, size_t len);

      protected:

        RtMidiOut * MidiPort = NULL;

    };

  }
}

#endif /* VIRT_MIDI_OUT_H */
