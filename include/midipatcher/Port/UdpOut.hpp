#ifndef PORT_UDP_OUT_H
#define PORT_UDP_OUT_H

#include "AbstractStreamOutputPort.hpp"

#include "asio.hpp"

namespace MidiPatcher {
  namespace Port {

    class UdpOut : AbstractStreamOutputPort {

      public:

        static const constexpr char * PortClass = "UdpOut";

        std::string getPortClass(){
          return PortClass;
        }

        static AbstractPort* factory(PortDescriptor * portDescriptor);

        static PortClassRegistryInfo * getPortClassRegistryInfo() {
          return new PortClassRegistryInfo(PortClass, factory, nullptr);
        }

        PortDescriptor * getPortDescriptor(){
          return new PortDescriptor(PortClass, Name);
        }

        UdpOut(std::string portName, std::string remoteAddress, short port, std::string multicastAddress = "", bool runningStatusEnabled = false);
        ~UdpOut();

        void registerPort(PortRegistry &portRegistry);


      protected:

        asio::io_context IOContext;
        asio::ip::udp::socket Socket;
        asio::ip::udp::endpoint Endpoint;

        void writeToStream(unsigned char * data, size_t len);


    };

  }
}

#endif /* PORT_UDP_OUT_H */
