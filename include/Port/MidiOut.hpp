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


        std::string getKey(){
          return Key;
        }

        static std::vector<AbstractPort*>  * scan(PortRegistry * portRegistry);
        static AbstractPort* factory(PortRegistry * portRegistry, int argc, char * argv[]);

        static PortDeclaration * getDeclaration() {
          return new PortDeclaration(Key, init, deinit, scan, factory);
        }

      protected:

        unsigned int PortNumber = 0;
        RtMidiOut * MidiPort = NULL;

      public:

        MidiOut(PortRegistry * portRegistry, unsigned int portNumber, std::string portName) : AbstractPort(portRegistry, TypeOutput, portName) {
          PortNumber = portNumber;

          (*KnownPorts)[portName] = this;
        }

        ~MidiOut() {
          if (MidiPort != NULL){
            MidiPort->closePort();
            delete MidiPort;
          }
        }

      protected:
        void addConnectionImpl(AbstractPort * port);
        void removeConnectionImpl(AbstractPort * port);

      public:
        void send(unsigned char *message, size_t len);

    };

  }
}


#endif /* PORT_MIDI_OUT_H */
