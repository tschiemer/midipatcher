#ifndef PORT_MIDI_IN_H
#define PORT_MIDI_IN_H

#include <AbstractPort.hpp>

#include <RtMidi.h>

#include <map>

namespace MidiPatcher {
  namespace Port {

    class MidiIn : AbstractPort {

      protected:

        static std::map<std::string, MidiIn*> * KnownPorts;

      public:

        static void init(){
          if (KnownPorts == NULL){
            KnownPorts =  new std::map<std::string,MidiIn*>();
          }
        }

        static void deinit(){
          if (KnownPorts != NULL){
            delete KnownPorts;
          }
        }

        static const constexpr char * Key = "MidiIn";

        static std::vector<AbstractPort*>  * scan(PortRegistry * portRegistry);

        std::string getKey(){
          return Key;
        }

      protected:

        unsigned int PortNumber;
        RtMidiIn * MidiPort;

      public:

        MidiIn(PortRegistry * portRegistry, unsigned int portNumber, std::string portName) : AbstractPort(portRegistry) {
          PortNumber = portNumber;
          Name = portName;

          (*KnownPorts)[portName] = this;
        }

      protected:

        void destructorImpl() {
          if (MidiPort != NULL){
            MidiPort->closePort();
            delete MidiPort;
          }
        }

        // ~MidiIn(){
        //   destructorImpl();
        // }
    };

  }
}


#endif /* PORT_MIDI_IN_H */
