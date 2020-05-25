#include <Port/UdpIn.hpp>
#include <PortRegistry.hpp>

#include <exception>

namespace MidiPatcher {
  namespace Port {

    AbstractPort* UdpIn::factory(PortDescriptor * portDescriptor){
      assert( portDescriptor->PortClass == PortClass );

      std::string str = portDescriptor->Name;

      short port;
      std::string listenAddress;

      size_t pos = str.find(":");

      // if no colon, assume just port and standard listen address
      if (pos == std::string::npos){
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

    UdpIn::UdpIn(std::string portName, std::string listenAddress, short port, std::string multicastAddress, bool runningStatusEnabled, size_t bufferSize)
      :  AbstractInputPort(portName),
        AbstractStreamInputPort(runningStatusEnabled),
        Socket(IOContext)
      {

      // try {
      InUdpBufferSize = bufferSize;
      InUdpBuffer = (unsigned char *)malloc(InUdpBufferSize);


        if (multicastAddress == ""){

          asio::ip::address listenAddress_ = asio::ip::make_address(listenAddress);

          std::cout << "UdpIn listenAddress = " << listenAddress << std::endl;
          std::cout << "UdpIn port = " << port << std::endl;
          // std::cout << "UdpIn multicastAddress = " << multicastAddress << std::endl;

          ListenEndpoint = asio::ip::udp::endpoint(
            listenAddress_, port
          );

          Socket.open(ListenEndpoint.protocol());
          Socket.set_option(asio::ip::udp::socket::reuse_address(true));
          Socket.bind(ListenEndpoint);

        } else {
          asio::ip::address listenAddress_ = asio::ip::make_address(listenAddress);
          asio::ip::address multicastAddress_ = asio::ip::make_address(multicastAddress);

          std::cout << "UdpIn interfaceAddress = " << listenAddress << std::endl;
          std::cout << "UdpIn port = " << port << std::endl;
          std::cout << "UdpIn multicastAddress = " << multicastAddress << std::endl;

          ListenEndpoint = asio::ip::udp::endpoint(
            // listenAddress_, port
            // multicastAddress_, port
            asio::ip::address_v4::any(), port
          );

          Socket.open(ListenEndpoint.protocol());
          Socket.set_option(asio::ip::udp::socket::reuse_address(true));
          Socket.bind(ListenEndpoint);

          if (listenAddress == "0.0.0.0"){
            Socket.set_option(
              asio::ip::multicast::join_group(multicastAddress_)
            );
          } else {
            Socket.set_option(
              // asio::ip::multicast::join_group(multicastAddress_.to_v4())
              asio::ip::multicast::join_group(multicastAddress_.to_v4(), listenAddress_.to_v4())
            );
          }

        }

      // } catch( const std::exception& e){
      //   std::cout << "EXCEPTION " << e.what() << std::endl;
      //
      //   throw e;
      // }

      this->setDeviceState(DeviceStateConnected);
    }

    UdpIn::~UdpIn(){
      Socket.close();
      free(InUdpBuffer);
    }

    void UdpIn::registerPort(PortRegistry &portRegistry){
      portRegistry.registerPort(this);
    }

    void UdpIn::start(){

      if (Running){
        return;
      }
      // std::cout << "UDPIN.START" << std::endl;
      // this->setNonBlocking();

      // IOContext.run();
      //
      // doRx();
      ReaderThread = std::thread([this](){
        this->Running = true;

        while(this->getDeviceState() == DeviceStateConnected){

          // std::cout << "recv" << std::endl;
          size_t count = 0;

          asio::ip::udp::endpoint senderEndpoint;
          count = Socket.receive_from(asio::buffer(InUdpBuffer, InUdpBufferSize), senderEndpoint);

          if (count > 0){
            readFromStream(InUdpBuffer, count);
          }

          // std::this_thread::sleep_for(std::chrono::milliseconds(50));

        }

        this->Running = false;
        // close(this->FD);
      });
      // ReaderThread.detach();

    }

//     void UdpIn::doRx(){
// std::cout << "doRx()" << std::endl;
//       Socket.async_receive_from(
//         asio::buffer(data_), SenderEndpoint,
//         [this](std::error_code ec, std::size_t length)
//         {
//           if (!ec)
//           {
//             // std::cout.write(data_, length);
//             // std::cout << std::endl;
//
//             std::cout << "rx " << length << std::endl;
//
//             doRx();
//           }
//         });
//     }

    void UdpIn::stop(){
      if (Running == false){
        return;
      }
      // Running = false;
      // ReaderThread.join();
    }

  }
}
