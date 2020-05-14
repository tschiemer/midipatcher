#include <Port/UdpOut.hpp>

#include <PortRegistry.hpp>

#include <cassert>

namespace MidiPatcher {
  namespace Port {

    AbstractPort* UdpOut::factory(PortDescriptor * portDescriptor){
      assert( portDescriptor->PortClass == PortClass );

      std::string str = portDescriptor->Name;

      short port;
      std::string remoteAddress;

      size_t pos = str.find(":");

      // if no colon, assume just port and standard listen address
      assert(pos != std::string::npos);

      remoteAddress = str.substr(0, pos);
      str.erase(0,pos + 1);
      port = std::atoi(str.c_str());

      std::string multicastAddress = portDescriptor->Options.count("multicast") ? portDescriptor->Options["multicast"] : "";
      bool runningStatusEnabled = portDescriptor->Options.count("runningstatus") && portDescriptor->Options["runningstatus"][0] != '\0' ? (portDescriptor->Options["runningstatus"][0] == '1') : false;

      return new UdpOut(portDescriptor->Name, remoteAddress, port, multicastAddress, runningStatusEnabled);
    }

    UdpOut::UdpOut(std::string portName, std::string remoteAddress, short port, std::string multicastAddress, bool runningStatusEnabled)
      :  AbstractOutputPort(portName),
        AbstractStreamOutputPort(runningStatusEnabled),
        Socket(IOContext, asio::ip::udp::endpoint(asio::ip::udp::v4(), 0))
      {

      std::cout << "UdpIn remoteAddress = " << remoteAddress << std::endl;
      std::cout << "UdpIn port = " << port << std::endl;

      asio::ip::udp::resolver resolver(IOContext);
      asio::ip::udp::resolver::results_type endpoints = resolver.resolve(asio::ip::udp::v4(), remoteAddress, std::to_string(port));

      // std::cout << "UdpIn multicastAddress = " << multicastAddress << std::endl;
      //
      // asio::ip::address remoteAddress_ = asio::ip::make_address(remoteAddress);
      // // asio::ip::address multicastAddress_ = asio::ip::make_address(multicastAddress);
      //
      //
      Endpoint = asio::ip::udp::endpoint(
        *endpoints.begin()
      );


      // Socket.open(Endpoint.protocol());
      // Socket.set_option(asio::ip::udp::socket::reuse_address(true));
      // Socket.bind(Endpoint);

      // Join the multicast group.
      // socket_.set_option(
      //   asio::ip::multicast::join_group(multicast_address)
      // );

      // portRegistry->registerPort( this );


          // size_t length = Socket.receive_from(asio::buffer(DataBuffer, sizeof(DataBuffer)), Endpoint);

          // std::cout << "RX " << length <<std::endl;

      // IOContext.run();

      this->setDeviceState(DeviceStateConnected);

      // std::thread([this](){
      //
      //   setDeviceState(DeviceStateConnected);
      // });
    }

    UdpOut::~UdpOut(){

    }

    void UdpOut::registerPort(PortRegistry &portRegistry){
      portRegistry.registerPort(this);
    }

    void UdpOut::writeToStream(unsigned char * data, size_t len){

// std::cout << "writeToUdp " << len << std::endl;
      Socket.send_to(asio::buffer(data, len), Endpoint);
    }

  }
}
