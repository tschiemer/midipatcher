#ifndef PORT_UDP_IN_H
#define PORT_UDP_IN_H

#include "../AbstractInputPort.hpp"

#include "asio.hpp"

namespace MidiPatcher {
  namespace Port {

    class UdpIn : AbstractInputPort {

      public:

        static const constexpr char * PortClass = "UdpIn";

        std::string getPortClass(){
          return PortClass;
        }

        static AbstractPort* factory(PortDescriptor * portDescriptor){
          assert( portDescriptor->PortClass == PortClass );

          int port = std::atoi(portDescriptor->Name.c_str());
          std::string listenAddress = portDescriptor->Options.count("listen") ? portDescriptor->Options["listen"] : "0.0.0.0";
          std::string multicastAddress = portDescriptor->Options.count("multicast") ? portDescriptor->Options["multicast"] : "255.255.255.255";

          // std::cout << "Key " << portDescriptor->getKey() << std::endl;
          // std::cout << portDescriptor->toString() << std::endl;

          return new UdpIn(portDescriptor->Name, port, listenAddress, multicastAddress);
        }

        static PortClassRegistryInfo * getPortClassRegistryInfo() {
          return new PortClassRegistryInfo(PortClass, factory, nullptr, nullptr, nullptr);
        }

        UdpIn(std::string portName, short port, std::string listenAddress = "0.0.0.0", std::string multicastAddress = "255.255.255.255");
        ~UdpIn();

        void registerPort(PortRegistry &portRegistry);

      protected:

        asio::io_context IOContext;
        asio::ip::udp::socket Socket;
        asio::ip::udp::endpoint Endpoint;

        char DataBuffer[128];

        volatile bool Running = false;
        std::thread ReaderThread;

        void start();
        void stop();

        void doReceive();

    };


  }
}

#endif /* PORT_UDP_IN_H */
