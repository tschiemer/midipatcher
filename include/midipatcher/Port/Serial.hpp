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

        static AbstractPort* factory(PortDescriptor &portDescriptor);

        static std::vector<AbstractPort*>  * scanner();

        Serial(std::string portName, std::string device, bool runningStatusEnabled = true);
        ~Serial();


      protected:

        asio::io_service IOService;
        asio::serial_port SerialPort;

        std::string Device;
        bool RunningStatusEnabled;

        void writeToStream(unsigned char * data, size_t len);

    };

  }
}

#endif /* MIDIPATCHER_PORT_SERIAL_H */
