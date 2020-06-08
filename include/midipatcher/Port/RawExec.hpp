#ifndef MIDIPATCHER_PORT_RAW_EXEC_H
#define MIDIPATCHER_PORT_RAW_EXEC_H

#include "AbstractExecPort.hpp"
#include "AbstractStreamInputPort.hpp"
#include "AbstractStreamOutputPort.hpp"

namespace MidiPatcher {

  class PortRegistry;

  namespace Port {

    class RawExec : public virtual AbstractExecPort, public virtual AbstractStreamInputPort, public virtual AbstractStreamOutputPort {

      public:

        static const constexpr char * PortClass = "RawExec";

        std::string getPortClass(){
          return PortClass;
        }

        PortDescriptor * getPortDescriptor(){
          return new PortDescriptor(PortClass, Name);
        }

        static PortClassRegistryInfo * getPortClassRegistryInfo() {
          return new PortClassRegistryInfo(PortClass, factory, nullptr);
        }

        static AbstractPort* factory(PortDescriptor * portDescriptor);

        RawExec(std::string portName, std::string execpath, std::string argvStr = "");
        ~RawExec();

        void registerPort(PortRegistry &portRegistry);

      protected:


        void writeToStream(unsigned char * data, size_t len);

        void readFromExec(unsigned char * buffer, size_t len );
    };

  }
}

#endif /* MIDIPATCHER_PORT_RAW_EXEC_H */
