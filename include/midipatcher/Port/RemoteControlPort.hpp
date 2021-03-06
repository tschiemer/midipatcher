#ifndef MIDIPATCHER_PORT_REMOTE_CONTROL_PORT_H
#define MIDIPATCHER_PORT_REMOTE_CONTROL_PORT_H

#include "../CLI.hpp"
#include "AbstractInputOutputPort.hpp"
#include <Port/ControlPort.hpp>

#include <iostream>

namespace MidiPatcher {

  class PortRegistry;

  namespace Port {

    class RemoteControlPort : public virtual CLI, public virtual AbstractInputOutputPort {

      public:

        static const constexpr char * PortClass = "RemoteControlPort";

        virtual std::string getPortClass(){
          return PortClass;
        }

        PortDescriptor * getPortDescriptor(){
          return new PortDescriptor(PortClass, Name);
        }

        RemoteControlPort(std::string name = "Default", std::string delimiter = " ") : RemoteControlPort(std::cin, std::cout, name, delimiter) {

        }

        RemoteControlPort(std::istream &in, std::ostream &out, std::string name = "Default", std::string delimiter = " ") : CLI(in,out,delimiter), AbstractInputOutputPort(name){
          setDeviceState(DeviceStateConnected);
        }

        void sendCommand(std::vector<std::string> &argv);

      protected:

        ControlPort::Message InMessage;

        // void receivedMessage(unsigned char * message, size_t len );

        void sendMessageImpl(unsigned char * message, size_t len);

    };

  }
}

#endif /* MIDIPATCHER_PORT_REMOTE_CONTROL_PORT_H */
