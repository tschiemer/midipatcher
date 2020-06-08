#ifndef VIRT_MIDI_OUT_H
#define VIRT_MIDI_OUT_H

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

      static AbstractPort* factory(PortDescriptor * portDescriptor){
        assert( portDescriptor->PortClass == PortClass );
        return new VirtMidiOut(portDescriptor->Name);
      }

      static PortClassRegistryInfo * getPortClassRegistryInfo() {
        return new PortClassRegistryInfo(PortClass, factory, nullptr);
      }

      PortDescriptor * getPortDescriptor(){
        return new PortDescriptor(PortClass, Name);
      }

      VirtMidiOut(std::string portName);
      ~VirtMidiOut();

      void registerPort(PortRegistry &portRegistry);

      protected:

        RtMidiOut * MidiPort = NULL;

        void sendMessageImpl(unsigned char * message, size_t len);
    };

  }
}

#endif /* VIRT_MIDI_OUT_H */
