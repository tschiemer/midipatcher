#ifndef PORT_MIDI_IN_H
#define PORT_MIDI_IN_H

#include <AbstractInputPort.hpp>

#include <RtMidi.h>

#include <map>

namespace MidiPatcher {
  namespace Port {

    class MidiIn : AbstractInputPort {

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

        unsigned int PortNumber = 0;
        RtMidiIn * MidiPort = NULL;



      public:

        MidiIn(PortRegistry * portRegistry, unsigned int portNumber, std::string portName) : AbstractPort(portRegistry) {
          PortNumber = portNumber;
          Name = portName;

          (*KnownPorts)[portName] = this;
        }

        ~MidiIn(){
          if (MidiPort != NULL){
            MidiPort->closePort();
            delete MidiPort;
          }
        }

      protected:

        // ~MidiIn(){
        //   destructorImpl();
        // }

        static void rtMidiCallback( double timeStamp, std::vector<unsigned char> *message, void *midiInRef );
    };

  }
}


#endif /* PORT_MIDI_IN_H */
