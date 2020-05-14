#include <Port/UdpIn.hpp>
#include <PortRegistry.hpp>

namespace MidiPatcher {
  namespace Port {

    AbstractPort* UdpIn::factory(PortDescriptor * portDescriptor){
      assert( portDescriptor->PortClass == PortClass );

      std::string str = portDescriptor->Name;

      short port;
      std::string listenAddress;

      size_t pos = str.find(":");

      // if no colon, assume just port and standard listen address
      if (pos != std::string::npos){
        port = std::atoi(str.c_str());
        listenAddress = "0.0.0.0";
      } else {
        listenAddress = str.substr(0, pos);
        str.erase(0,pos + 1);
        port = std::atoi(str.c_str());
      }

      std::string multicastAddress = portDescriptor->Options.count("multicast") ? portDescriptor->Options["multicast"] : "";
      bool runningStatusEnabled = portDescriptor->Options.count("runningstatus") && portDescriptor->Options["runningstatus"][0] != '\0' ? (portDescriptor->Options["runningstatus"][0] == '1') : true;

      return new UdpIn(portDescriptor->Name, listenAddress, port, multicastAddress, runningStatusEnabled);
    }

    UdpIn::UdpIn(std::string portName, std::string listenAddress, short port, std::string multicastAddress, bool runningStatusEnabled)
      :  AbstractInputPort(portName),
        AbstractStreamInputPort(runningStatusEnabled),
        Socket(IOContext)
      {


      std::cout << "UdpIn listenAddress = " << listenAddress << std::endl;
      std::cout << "UdpIn port = " << port << std::endl;

      // std::cout << "UdpIn multicastAddress = " << multicastAddress << std::endl;

      asio::ip::address listenAddress_ = asio::ip::make_address(listenAddress);
      // asio::ip::address multicastAddress_ = asio::ip::make_address(multicastAddress);


      Endpoint = asio::ip::udp::endpoint(
        listenAddress_, port
      );


      Socket.open(Endpoint.protocol());
      Socket.set_option(asio::ip::udp::socket::reuse_address(true));
      Socket.bind(Endpoint);

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

    UdpIn::~UdpIn(){

    }

    void UdpIn::registerPort(PortRegistry &portRegistry){
      portRegistry.registerPort(this);
    }

    void UdpIn::start(){

      if (Running){
        return;
      }

      // this->setNonBlocking();


      ReaderThread = std::thread([this](){
        this->Running = true;

        while(this->getDeviceState() == DeviceStateConnected){
          unsigned char buffer[128];
          size_t count = 0;

          count = Socket.receive_from(asio::buffer(buffer, sizeof(buffer)), Endpoint);

          if (count > 0){
            readFromStream(buffer, count);
          }

          // std::this_thread::sleep_for(std::chrono::milliseconds(50));

        }

        this->Running = false;
        // close(this->FD);
      });
      // ReaderThread.detach();

    }

    void UdpIn::stop(){
      if (Running == false){
        return;
      }
      // Running = false;
      // ReaderThread.join();
    }

  }
}
