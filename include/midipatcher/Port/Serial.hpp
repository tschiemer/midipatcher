#ifndef MIDIPATCHER_PORT_SERIAL_H
#define MIDIPATCHER_PORT_SERIAL_H

#include "AbstractStreamInputPort.hpp"
#include "AbstractStreamOutputPort.hpp"

#include <asio.hpp>

namespace MidiPatcher {

  namespace Port {

    class Serial : public virtual AbstractStreamInputPort, public virtual AbstractStreamOutputPort {

      public:

        enum FlowControl_t {
          FlowControlNone,
          FlowControlSoftware,
          FlowControlHardware
        };

        enum Parity_t {
          ParityNone,
          ParityOdd,
          ParityEven
        };

        enum StopBits_t {
          StopBitsOne,
          StopBitsOnePointFive,
          StopBitsTwo
        };


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

        Serial(std::string portName, std::string device);
        ~Serial();


        bool hasOption(std::string key);

        std::string getOption(std::string key);

        void setOption(std::string key, std::string value);

      protected:

        asio::io_service IOService;
        asio::serial_port SerialPort;

        std::string Device;

        void writeToStream(unsigned char * data, size_t len);

        void start();
        void stop();

    };

  }
}

#endif /* MIDIPATCHER_PORT_SERIAL_H */
