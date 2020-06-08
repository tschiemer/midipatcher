#ifndef MIDIPATCHER_PORT_MIDI_OUT_H
#define MIDIPATCHER_PORT_MIDI_OUT_H

#include "AbstractOutputPort.hpp"

#include <RtMidi.h>

#include <cassert>
#include <map>

namespace MidiPatcher {
  namespace Port {

    class MidiOut : AbstractOutputPort {

      public:

        static const constexpr char * PortClass = "MidiOut";

        std::string getPortClass(){
          return PortClass;
        }

        PortDescriptor * getPortDescriptor();

        static std::vector<AbstractPort*>  * scan(PortRegistry * portRegistry);

        static AbstractPort* factory(PortDescriptor * portDescriptor);

        static PortClassRegistryInfo * getPortClassRegistryInfo() {
          return new PortClassRegistryInfo(PortClass, factory, scan);
        }

      protected:

        static std::map<std::string, MidiOut*> * KnownPorts;

        static inline bool isKnown(std::string &key){
          if (KnownPorts == nullptr){
            return false;
          }
          return KnownPorts->count(key) > 0;
        }

        RtMidi::Api Api = RtMidi::UNSPECIFIED;

        unsigned int PortNumber = 0;
        RtMidiOut * MidiPort = nullptr;

      public:

        MidiOut(std::string portName, RtMidi::Api api = RtMidi::UNSPECIFIED, unsigned int portNumber = 0);

        ~MidiOut();

        void registerPort(PortRegistry &portRegistry);

      protected:

        void start();
        void stop();

      // public:

        void sendMessageImpl(unsigned char * message, size_t len);

    };

  }
}


#endif /* MIDIPATCHER_PORT_MIDI_OUT_H */
