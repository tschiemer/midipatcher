#ifndef CONTROL_PORT_H
#define CONTROL_PORT_H

#include "AbstractInputOutputPort.hpp"
#include "PortRegistry.hpp"

#include <cassert>
#include <vector>

namespace MidiPatcher {

  // class PortRegistry;
  // class PortRegistry::PortRegistryUpdateReceiver;

  namespace Port {

    class ControlPort : public virtual AbstractInputOutputPort, public virtual PortRegistry::PortRegistryUpdateReceiver {

      public:

        static const constexpr char * PortClass = "ControlPort";

        std::string getPortClass(){
          return PortClass;
        }

        // std::string getKey(){
        //   return PortClass;
        // }

        PortDescriptor * getPortDescriptor(){
          return new PortDescriptor(PortClass, Name);
        }

        ControlPort(PortRegistry * portRegistry);

        void sendMessage(unsigned char * message, size_t len);

      protected:

        PortRegistry * PortRegistryRef;

        bool OptReturnIds = false;

      public:

        // header (sysex experimental and marker bytes)
        // static const constexpr unsigned char MessageHeader[] = {0xF0, 0x7D, 0x01, 0x03, 0x03, 0x07};

        // tail (end of sysex)
        // static const constexpr char * MessageTail = "\xF7";

        static uint8_t packMidiMessage(unsigned char * midi, unsigned char * data, uint8_t dataLen);
        static uint8_t unpackMidiMessage(unsigned char * data, unsigned char * midi, uint8_t midiLen);

        static uint8_t packArguments(unsigned char * data, std::vector<std::string> &argv);
        static void unpackArguments(std::vector<std::string> &argv, unsigned char * data, uint8_t dataLen);

        static uint8_t packMessage(unsigned char * midi, std::vector<std::string> &argv);
        static void unpackMessage(std::vector<std::string> &argv, unsigned char * midi, uint8_t midiLen);

      protected:

        AbstractPort * getPortByIdOrKey( std::string &idOrKey );

        void handleCommand(std::vector<std::string> &argv);

        void send(std::vector<std::string> &argv);
        void send(const char * fmt, ...);

        void ok();

        void error(std::string msg = "");

      public:

        // void deviceDiscovered(AbstractPort * port);
        void deviceStateChanged(AbstractPort * port, DeviceState_t newState);
        void portRegistered( AbstractPort * port );
        void portUnregistered( AbstractPort * port );
    };

  }

}

#endif /* CONTROL_PORT_H */
