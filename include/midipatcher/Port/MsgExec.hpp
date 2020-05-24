#ifndef MSG_EXEC_H
#define MSG_EXEC_H

#include "RawExec.hpp"
#include "AbstractMessageOutputPort.hpp"

namespace MidiPatcher {
  namespace Port {

    class MsgExec : public virtual RawExec, public virtual AbstractMessageOutputPort {

        using AbstractMessageOutputPort::sendMessageImpl;

        public:

          static const constexpr char * PortClass = "MsgExec";

          std::string getPortClass(){
            return PortClass;
          }

          static AbstractPort* factory(PortDescriptor * portDescriptor);

          static PortClassRegistryInfo * getPortClassRegistryInfo() {
            return new PortClassRegistryInfo(PortClass, factory, nullptr, nullptr, nullptr);
          }

          PortDescriptor * getPortDescriptor(){
            return new PortDescriptor(PortClass, Name);
          }

          MsgExec(std::string portName, std::vector<std::string> argv, std::string baseDir = "");
          ~MsgExec();

          // void registerPort(PortRegistry &portRegistry);

        protected:

          void sendMessageImpl(unsigned char * message, size_t len);
          void writeStringMessage(unsigned char * stringMessage, size_t len);

          // void readFromFile(unsigned char * buffer, size_t len );
    };

  }
}

#endif /* MSG_EXEC_H */
