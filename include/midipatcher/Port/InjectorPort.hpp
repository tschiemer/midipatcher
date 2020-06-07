#ifndef INJECTOR_PORT_H
#define INJECTOR_PORT_H

#include "AbstractInputOutputPort.hpp"

namespace MidiPatcher {
  namespace Port {


    class InjectorPort : public virtual AbstractInputOutputPort {

      public:

        static const constexpr char * PortClass = "InjectorPort";

        virtual std::string getPortClass(){
          return PortClass;
        }

        typedef void (*ReceivedMessageHandler)(unsigned char * data, int len, InjectorPort * injectorPort, void * userData);

        PortDescriptor * getPortDescriptor(){
          return new PortDescriptor(PortClass, Name);
        }

        InjectorPort(std::string name, ReceivedMessageHandler receivedMessageHandler, void * userData = NULL) : AbstractInputOutputPort(name){
          ReceivedMessageHandlerRef = receivedMessageHandler;
          UserData = userData;
          setDeviceState(DeviceStateConnected);
        }

      protected:

        void * UserData = NULL;
        ReceivedMessageHandler ReceivedMessageHandlerRef;

        void sendMessageImpl(unsigned char * message, size_t len){
          ReceivedMessageHandlerRef(message, len, this, UserData);
        }

      public:

        void send( unsigned char * message, size_t len){
          receivedMessage(message, len);
        }

    };

  }
}

#endif /* INJECTOR_PORT_H */
