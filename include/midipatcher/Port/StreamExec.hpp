#ifndef MIDIPATCHER_PORT_RAW_EXEC_H
#define MIDIPATCHER_PORT_RAW_EXEC_H

#include "AbstractExecPort.hpp"
#include "AbstractStreamInputOutputPort.hpp"

namespace MidiPatcher {

  class PortRegistry;

  namespace Port {

    class StreamExec : public virtual AbstractExecPort, public virtual AbstractStreamInputOutputPort {

      public:

        static const constexpr char * PortClass = "StreamExec";

        std::string getPortClass(){
          return PortClass;
        }

        PortDescriptor * getPortDescriptor(){
          return new PortDescriptor(PortClass, Name);
        }

        static PortClassRegistryInfo * getPortClassRegistryInfo() {
          return new PortClassRegistryInfo(PortClass, factory, nullptr);
        }

        static AbstractPort* factory(PortDescriptor &portDescriptor);

        StreamExec(std::string portName, std::string execpath, std::string argvStr = "");
        ~StreamExec();

      protected:


        void writeToStream(unsigned char * data, size_t len);

        void readFromExec(unsigned char * buffer, size_t len );
    };

  }
}

#endif /* MIDIPATCHER_PORT_RAW_EXEC_H */
