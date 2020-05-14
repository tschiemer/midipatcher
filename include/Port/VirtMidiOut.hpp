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

      static AbstractPort* factory(PortDescriptor * portDescriptor){
        assert( portDescriptor->PortClass == PortClass );
        return new VirtMidiOut(portDescriptor->Name);
      }

      static PortClassRegistryInfo * getPortClassRegistryInfo() {
        return new PortClassRegistryInfo(PortClass, factory, nullptr, nullptr, nullptr);
      }

      VirtMidiOut(std::string portName);
      ~VirtMidiOut();

      void registerPort(PortRegistry &portRegistry);

      void sendMessage(unsigned char * message, size_t len);

      protected:

        RtMidiOut * MidiPort = NULL;

    };

  }
}

#endif /* VIRT_MIDI_OUT_H */
