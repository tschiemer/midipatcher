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

        std::string getKey(){
          return Key;
        }

        static std::vector<AbstractPort*>  * scan(PortRegistry * portRegistry);

        static AbstractPort* factory(PortRegistry * portRegistry, PortDescriptor * portDescriptor){
           return new MidiIn(portRegistry, portDescriptor->Name);
        }

        static PortDeclaration * getDeclaration(){
          return new PortDeclaration(Key, factory, init, deinit, scan);
        }

      protected:

        unsigned int PortNumber = 0;
        RtMidiIn * MidiPort = NULL;

      public:

        MidiIn(PortRegistry * portRegistry, std::string portName, unsigned int portNumber = 0) : AbstractPort(portRegistry, TypeInput, portName) {
          PortNumber = portNumber;

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

      // protected:
        void addConnectionImpl(AbstractPort * port);
        void removeConnectionImpl(AbstractPort * port);
    };

  }
}


#endif /* PORT_MIDI_IN_H */
