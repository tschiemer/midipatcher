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

        static AbstractPort* factory(PortDescriptor * portDescriptor);

        std::string getPortClass(){
          return PortClass;
        }

        // std::string getKey(){
        //   if (Api == RtMidi::UNSPECIFIED){
        //     return getPortClass() + ":" + Name;
        //   }
        //   return getPortClass() + ":" + Name + ":" + std::to_string(static_cast<int>(Api));
        // }

        PortDescriptor * getPortDescriptor();

        static std::vector<AbstractPort*>  * scan(PortRegistry * portRegistry);


        static PortClassRegistryInfo * getPortClassRegistryInfo(){
          return new PortClassRegistryInfo(PortClass, factory, scan);
        }

      protected:

        static std::map<std::string, MidiIn*> * KnownPorts;

        static inline bool isKnown(std::string &key){
          if (KnownPorts == nullptr){
            return false;
          }
          return KnownPorts->count(key) > 0;
        }

        RtMidi::Api Api = RtMidi::UNSPECIFIED;

        unsigned int PortNumber = 0;
        RtMidiIn * MidiPort = nullptr;

        // DeviceState_t DeviceState = DeviceStateNotConnected;

      public:

        // MidiIn(std::string)

        MidiIn(std::string portName, RtMidi::Api api = RtMidi::UNSPECIFIED, unsigned int portNumber = 0);

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
