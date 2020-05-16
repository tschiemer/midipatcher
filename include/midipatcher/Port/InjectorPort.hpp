#ifndef INJECTOR_PORT_H
#define INJECTOR_PORT_H

#include "AbstractInputOutputPort.hpp"

namespace MidiPatcher {
  namespace Port {


    class InjectorPort : public virtual AbstractInputOutputPort {

      public:

        static const constexpr char * PortClass = "InjectorPort";

        std::string getPortClass(){
          return PortClass;
        }

        typedef void (*ReceivedMessageHandler)(unsigned char * data, int len, InjectorPort * injectorPort, void * userData);

        InjectorPort(std::string name, ReceivedMessageHandler receivedMessageHandler, void * useData = NULL) : AbstractInputOutputPort(name){
          ReceivedMessageHandlerRef = receivedMessageHandler;

          setDeviceState(DeviceStateConnected);
        }

      protected:

        void * UserData = NULL;
        ReceivedMessageHandler ReceivedMessageHandlerRef;

        void receivedMessage(unsigned char * message, size_t len ){
          std::cout << "receivedMessage" << std::endl;
          ReceivedMessageHandlerRef(message, len, this, UserData);
        }

      public:

        void sendMessage(unsigned char * message, size_t len){
          // std::cout << "sendMessage" << std::endl;
          AbstractInputPort::receivedMessage(message, len);
        }

    };

  }
}

#endif /* INJECTOR_PORT_H */
