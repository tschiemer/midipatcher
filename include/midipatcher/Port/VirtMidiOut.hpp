#ifndef MIDIPATCHER_PORT_VIRT_MIDI_OUT_H
#define MIDIPATCHER_PORT_VIRT_MIDI_OUT_H

#include "AbstractOutputPort.hpp"

#include <RtMidi.h>

namespace MidiPatcher {
  namespace Port {

    class VirtMidiOut : AbstractOutputPort {

    public:

      static const constexpr char * PortClass = "VirtMidiOut";

      std::string getPortClass(){
        return PortClass;
      }

      PortDescriptor * getPortDescriptor(){
        return new PortDescriptor(PortClass, Name);
      }

      static PortClassRegistryInfo * getPortClassRegistryInfo() {
        return new PortClassRegistryInfo(PortClass, factory, nullptr);
      }

      static AbstractPort* factory(PortDescriptor * portDescriptor){
        assert( portDescriptor->PortClass == PortClass );
        return new VirtMidiOut(portDescriptor->Name);
      }

      VirtMidiOut(std::string portName);
      ~VirtMidiOut();


      protected:

        RtMidiOut * MidiPort = NULL;

        void sendMessageImpl(unsigned char * message, size_t len);
    };

  }
}

#endif /* MIDIPATCHER_PORT_VIRT_MIDI_OUT_H */
