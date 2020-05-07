#ifndef PORT_MIDI_OUT_H
#define PORT_MIDI_OUT_H

#include <AbstractPort.hpp>

#include <RtMidi.h>

#include <cassert>
#include <map>

namespace MidiPatcher {
  namespace Port {

    class MidiOut : AbstractPort {

      protected:

        static std::map<std::string, MidiOut*> * KnownPorts;

      public:

        static void init(){
          if (KnownPorts == NULL){
            KnownPorts =  new std::map<std::string,MidiOut*>();
          }
        }

        static void deinit(){
          if (KnownPorts != NULL){
            delete KnownPorts;
          }
        }


        static const constexpr char * Key = "MidiOut";

        static std::vector<AbstractPort*>  * Scanner(PortRegistry * portRegistry);

        std::string getKey(){
          return Key;
        }

      protected:

        unsigned int PortNumber;
        RtMidiOut * MidiPort;

      public:

        MidiOut(PortRegistry * portRegistry, unsigned int portNumber, std::string portName) : AbstractPort(portRegistry) {
          PortNumber = portNumber;
          Name = portName;

          KnownPorts->at(portName) = this;
        }

      protected:

        void destructorImpl() {
          if (MidiPort != NULL){
            MidiPort->closePort();
            delete MidiPort;
          }
        }
    };

  }
}


#endif /* PORT_MIDI_OUT_H */
