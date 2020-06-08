#ifndef MIDIPATCHER_PORT_UDP_OUT_H
#define MIDIPATCHER_PORT_UDP_OUT_H

#include "AbstractStreamOutputPort.hpp"

#include <asio.hpp>

namespace MidiPatcher {
  namespace Port {

    class UdpOut : AbstractStreamOutputPort {

      public:

        static const constexpr char * PortClass = "UdpOut";

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

        UdpOut(std::string portName, std::string remoteAddress, short port, std::string multicastAddress = "", bool runningStatusEnabled = false);
        ~UdpOut();


      protected:

        asio::io_context IOContext;
        asio::ip::udp::socket Socket;
        asio::ip::udp::endpoint Endpoint;

        void writeToStream(unsigned char * data, size_t len);


    };

  }
}

#endif /* MIDIPATCHER_PORT_UDP_OUT_H */
