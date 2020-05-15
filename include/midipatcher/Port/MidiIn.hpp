#ifndef PORT_MIDI_IN_H
#define PORT_MIDI_IN_H

#include "AbstractInputPort.hpp"

#include <RtMidi.h>

#include <map>

namespace MidiPatcher {
  namespace Port {

    class MidiIn : AbstractInputPort {

      public:

        static const constexpr char * PortClass = "MidiIn";

        static AbstractPort* factory(PortDescriptor * portDescriptor){
          assert( portDescriptor->PortClass == PortClass );
          return new MidiIn(portDescriptor->Name);
        }

        static void init(){
          if (KnownPorts == NULL){
            KnownPorts =  new std::map<std::string,MidiIn*>();
          }
        }

        static void deinit(){
          if (KnownPorts != NULL){
            delete KnownPorts;
            KnownPorts = NULL;
          }
        }

        std::string getPortClass(){
          return PortClass;
        }

        PortDescriptor * getPortDescriptor() {
            return new PortDescriptor(PortClass, Name);
        };

        static std::vector<AbstractPort*>  * scan(PortRegistry * portRegistry);


        static PortClassRegistryInfo * getPortClassRegistryInfo(){
          return new PortClassRegistryInfo(PortClass, factory, init, deinit, scan);
        }

      protected:

        static std::map<std::string, MidiIn*> * KnownPorts;

        unsigned int PortNumber = 0;
        RtMidiIn * MidiPort = NULL;

        // DeviceState_t DeviceState = DeviceStateNotConnected;

      public:

        // MidiIn(std::string)

        MidiIn(std::string portName, unsigned int portNumber = 0);

        ~MidiIn();

        void registerPort(PortRegistry &portRegistry);

        // DeviceState_t getDeviceState(){
        //   return DeviceState;
        // }

      protected:

        static void rtMidiCallback( double timeStamp, std::vector<unsigned char> *message, void *midiInRef );

        void start();
        void stop();
    };

  }
}


#endif /* PORT_MIDI_IN_H */
