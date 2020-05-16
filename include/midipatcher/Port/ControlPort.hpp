#ifndef CONTROL_PORT_H
#define CONTROL_PORT_H

#include "AbstractInputOutputPort.hpp"

#include <cassert>
#include <vector>

namespace MidiPatcher {

  class PortRegistry;

  namespace Port {

    class ControlPort : public virtual AbstractInputOutputPort {

      public:

        static const constexpr char * PortClass = "ControlPort";

        std::string getPortClass(){
          return PortClass;
        }

        ControlPort(PortRegistry * portRegistry) : AbstractInputOutputPort("ControlPort"){
          assert(portRegistry != nullptr);

          PortRegistryRef = portRegistry;
        }

        void sendMessage(unsigned char * message, size_t len);

      protected:

        PortRegistry * PortRegistryRef;

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
        //
        // enum Type_t {
        //   TypeRequest         = 1,
        //   TypeResponse        = 2,
        //   TypeUnsolicitedInfo = 3
        // };
        //
        // enum Status_t {
        //   StatusNone          = 0,
        //   StatusOK            = 1,
        //   StatusError         = 2,
        // };

        // enum Type_t {
        //   TypePing               = 01,
        //   TypePong               = 02,
        //
        //   TypeVersionReqquest    = 05,
        //   TypeVersionResponse    = 06,
        //
        //   TypeListPortsRequest   = 10,
        //   TypeListPortsResponse  = 10,
        //   TypeListPortsItem      = 11,
        //
        //   TypeConnectionStatusRequest   = 20,
        //   TypeConnectionStatusResponse  = 21,
        //   TypeConnectionStatusItem = 22,
        //
        //   TypeConnectPortsRequest       = 30,
        //   TypeConnectPortsResponse      = 31,
        //   TypeDisconnectPortsRequest    = 32,
        //   TypeDisconnectPortsResponse   = 33,
        //
        //   TypeRegisterPortRequest       = 40,
        //   TypeRegisterPortResponse      = 41,
        //   TypeUnregisterPortRequest     = 42,
        //   TypeUnregisterPortResponse    = 43,
        //
        //   TypeInformation        = 200,
        //   TypeWarning            = 201,
        //   TypeError              = 202
        //
        //   TypeKillRequest        = 254,
        //   TypeKillACK            = 255
        // };
        //
        // struct Message_t {
        //   Type_t Type;
        //   std::string Str1;
        //   std::string Str2;
        //   int Int1;
        // };

        // static uint8_t packMessage(uint8_t * sysex, Type_t type, ... );

      protected:



    };

  }

}

#endif /* CONTROL_PORT_H */
