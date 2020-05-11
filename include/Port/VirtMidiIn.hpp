#ifndef VIRT_MIDI_IN_H
#define VIRT_MIDI_IN_H

#include "../AbstractInputPort.hpp"

#include <RtMidi.h>

namespace MidiPatcher {
  namespace Port {

    class VirtMidiIn : AbstractInputPort {

    public:

      static const constexpr char * PortClass = "VirtMidiIn";

      std::string getPortClass(){
        return PortClass;
      }

      static AbstractPort* factory(PortRegistry * portRegistry, PortDescriptor * portDescriptor){
        assert( portDescriptor->PortClass == PortClass );
        return new VirtMidiIn(portRegistry, portDescriptor->Name);
      }

      static PortClassRegistryInfo * getPortClassRegistryInfo() {
        return new PortClassRegistryInfo(PortClass, factory, nullptr, nullptr, nullptr);
      }

      VirtMidiIn(PortRegistry * portRegistry, std::string portName);
      ~VirtMidiIn();


      protected:

        RtMidiIn * MidiPort = NULL;

        static void rtMidiCallback( double timeStamp, std::vector<unsigned char> *message, void *midiInRef );


    };

  }
}

#endif /* VIRT_MIDI_IN_H */
