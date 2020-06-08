#ifndef MIDIPATCHER_PORT_SERIAL_H
#define MIDIPATCHER_PORT_SERIAL_H

#include "AbstractStreamInputPort.hpp"
#include "AbstractStreamOutputPort.hpp"

#include <asio.hpp>

namespace MidiPatcher {

  namespace Port {

    class Serial : public virtual AbstractStreamInputPort, public virtual AbstractStreamOutputPort {

      public:

        static const constexpr char * PortClass = "Serial";

        std::string getPortClass(){
          return PortClass;
        }

        PortDescriptor * getPortDescriptor(){
          return new PortDescriptor(PortClass, Name);
        }

        static PortClassRegistryInfo * getPortClassRegistryInfo() {
          return new PortClassRegistryInfo(PortClass, factory, scanner);
        }

        static AbstractPort* factory(PortDescriptor * portDescriptor){
          assert( portDescriptor->PortClass == PortClass );
          return new Serial(portDescriptor->Name);
        }

        static std::vector<AbstractPort*>  * scanner(PortRegistry * portRegistry);

        Serial(std::string portName, bool runningStatusEnabled = true);
        ~Serial();

        void registerPort(PortRegistry &portRegistry);

      protected:

        void writeToStream(unsigned char * data, size_t len);

    };

  }
}

#endif /* MIDIPATCHER_PORT_SERIAL_H */
