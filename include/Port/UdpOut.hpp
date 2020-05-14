#ifndef PORT_UDP_OUT_H
#define PORT_UDP_OUT_H

#include "../AbstractOutputPort.hpp"

namespace MidiPatcher {
  namespace Port {

    class UdpOut : AbstractOutputPort {

      public:

        static const constexpr char * PortClass = "UdpOut";

        std::string getPortClass(){
          return PortClass;
        }

        static AbstractPort* factory(PortDescriptor * portDescriptor);

        static PortClassRegistryInfo * getPortClassRegistryInfo() {
          return new PortClassRegistryInfo(PortClass, factory, nullptr, nullptr, nullptr);
        }

        UdpOut(std::string portName, std::string remoteAddress, short port, std::string multicastAddress = "");
        ~UdpOut();

        void registerPort(PortRegistry &portRegistry);


    }

  }
}

#endif /* PORT_UDP_OUT_H */
