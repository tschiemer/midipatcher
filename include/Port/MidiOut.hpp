#ifndef PORT_MIDI_OUT_H
#define PORT_MIDI_OUT_H

#include <AbstractOutputPort.hpp>

#include <RtMidi.h>

#include <cassert>
#include <map>

namespace MidiPatcher {
  namespace Port {

    class MidiOut : AbstractOutputPort {

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

        static std::vector<AbstractPort*>  * scan(PortRegistry * portRegistry);

        std::string getKey(){
          return Key;
        }

      protected:

        unsigned int PortNumber = 0;
        RtMidiOut * MidiPort = NULL;

      public:

        MidiOut(PortRegistry * portRegistry, unsigned int portNumber, std::string portName) : AbstractPort(portRegistry), AbstractOutputPort() {
          PortNumber = portNumber;
          Name = portName;

          (*KnownPorts)[portName] = this;
        }

        ~MidiOut() {
          if (MidiPort != NULL){
            MidiPort->closePort();
            delete MidiPort;
          }
      }
    };

  }
}


#endif /* PORT_MIDI_OUT_H */
