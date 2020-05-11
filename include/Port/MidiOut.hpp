#ifndef PORT_MIDI_OUT_H
#define PORT_MIDI_OUT_H

#include <AbstractOutputPort.hpp>

#include <RtMidi.h>

#include <cassert>
#include <map>

namespace MidiPatcher {
  namespace Port {

    class MidiOut : AbstractOutputPort {

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

        static const constexpr char * PortClass = "MidiOut";

        std::string getPortClass(){
          return PortClass;
        }

        PortDescriptor * getPortDescriptor() {
            return new PortDescriptor(PortClass, Name);
        };

        static std::vector<AbstractPort*>  * scan(PortRegistry * portRegistry);

        static AbstractPort* factory(PortRegistry * portRegistry, PortDescriptor * portDescriptor){
           return new MidiOut(portRegistry, portDescriptor->Name);
        }

        static PortClassRegistryInfo * getPortClassRegistryInfo() {
          return new PortClassRegistryInfo(PortClass, factory, init, deinit, scan);
        }

      protected:

        static std::map<std::string, MidiOut*> * KnownPorts;

        unsigned int PortNumber = 0;
        RtMidiOut * MidiPort = NULL;

      public:

        MidiOut(PortRegistry * portRegistry, std::string portName, unsigned int portNumber = 0);

        ~MidiOut();

      protected:

        void start();
        void stop();

      public:

        void send(unsigned char *message, size_t len);

    };

  }
}


#endif /* PORT_MIDI_OUT_H */
