#ifndef PORT_MIDI_IN_H
#define PORT_MIDI_IN_H

#include <AbstractInputPort.hpp>

#include <RtMidi.h>

#include <map>

namespace MidiPatcher {
  namespace Port {

    class MidiIn : AbstractInputPort {

      public:

        static const constexpr char * Key = "MidiIn";

        static AbstractPort* factory(PortRegistry * portRegistry, PortDescriptor * portDescriptor){
           return new MidiIn(portRegistry, portDescriptor->Name);
        }

        static void init(){
          // if (RtMidiInRef == NULL){
          //   RtMidiInRef = new RtMidiIn;
          // }
          if (KnownPorts == NULL){
            KnownPorts =  new std::map<std::string,MidiIn*>();
          }
        }

        static void deinit(){
          if (KnownPorts != NULL){
            delete KnownPorts;
            KnownPorts = NULL;
          }
          // if (RtMidiInRef != NULL){
          //   delete RtMidiInRef;
          //   RtMidiInRef = NULL;
          // }
        }

        std::string getKey(){
          return Key;
        }

        static std::vector<AbstractPort*>  * scan(PortRegistry * portRegistry);


        static PortClassRegistryInfo * getPortClassRegistryInfo(){
          return new PortClassRegistryInfo(Key, factory, init, deinit, scan);
        }

      protected:

        // static RtMidiIn * RtMidiInRef;

        static std::map<std::string, MidiIn*> * KnownPorts;

        unsigned int PortNumber = 0;
        RtMidiIn * MidiPort = NULL;

        DeviceState_t DeviceState = DeviceStateNotConnected;

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

        DeviceState_t getDeviceState(){
          return DeviceState;
        }

      protected:

        static void rtMidiCallback( double timeStamp, std::vector<unsigned char> *message, void *midiInRef );

        void start();
        void stop();
    };

  }
}


#endif /* PORT_MIDI_IN_H */
