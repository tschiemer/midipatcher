#ifndef MIDIPATCHER_PORT_UDP_IN_H
#define MIDIPATCHER_PORT_UDP_IN_H

#include "AbstractStreamInputPort.hpp"

#include <asio.hpp>

namespace MidiPatcher {
  namespace Port {

    class UdpIn : AbstractStreamInputPort {

      public:

        static const constexpr char * PortClass = "UdpIn";

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

        UdpIn(std::string portName, std::string listenAddress, short port, std::string multicastAddress = "", bool runningStatusEnabled = true, size_t bufferSize = 256);
        ~UdpIn();

        void registerPort(PortRegistry &portRegistry);

      protected:

        asio::io_context IOContext;
        asio::ip::udp::socket Socket;
        asio::ip::udp::endpoint ListenEndpoint;

        volatile bool Running = false;
        std::thread ReaderThread;

        size_t InUdpBufferSize;
        unsigned char * InUdpBuffer;

        void start();
        void stop();

        // std::array<char, 1024> data_;
        // asio::ip::udp::endpoint SenderEndpoint;
        // void doRx();

    };


  }
}

#endif /* MIDIPATCHER_PORT_UDP_IN_H */
