#include <Port/UdpIn.hpp>
#include <PortRegistry.hpp>

namespace MidiPatcher {
  namespace Port {

    UdpIn::UdpIn(PortRegistry * portRegistry, std::string portName, short port, std::string listenAddress, std::string multicastAddress)
      :  AbstractPort(portRegistry, TypeInput, portName), Socket(IOContext)
      {

// ;

      std::cout << "UdpIn port = " << port << std::endl;

      std::cout << "UdpIn listenAddress = " << listenAddress << std::endl;
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

      portRegistry->registerPort( this );


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

          if (count == -1){
            // std::cout << "ERROR" << std::endl;
          }
          else if (count == 0){
            // std::cout << "nothing to read" << std::endl;
          }
          else if (count > 0){
            // std::cout << "read (" << count << ") ";
            // for(int i = 0; i < count; i++){
            //   std::cout << std::hex << (int)buffer[i];
            // }
            // std::cout << std::endl;


            // this->send(buffer,count);
            send(buffer, count);
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


    // void UdpIn::start(){
    //   std::cout << "UdpIn.start" << std::endl;
    //   doReceive();
    // }
//
    // void UdpIn::doReceive(){
    //   std::cout << "UdpIn.doReceive" << std::endl;
    //   Socket.async_receive_from(
    //     asio::buffer(DataBuffer, sizeof(DataBuffer)), Endpoint, [this](std::error_code ec, std::size_t nRx)
    //       {
    //         std::cout << "received??" << std::endl;
    //         if (!ec && nRx > 0)
    //         {
    //           std::cout << "rx " << nRx << std::endl;
    //           this->send((unsigned char *)DataBuffer, nRx);
    //         }
    //         else
    //         {
    //           doReceive();
    //         }
    //     });
    // }


  }
}
