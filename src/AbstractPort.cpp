#include <AbstractPort.hpp>

#include "PortRegistry.hpp"

namespace MidiPatcher {


  AbstractPort::AbstractPort(PortRegistry * portRegistry, Type_t type, std::string name){

    Type = type;
    Name = name;

    // portRegistry->registerPort( this );

  };

  AbstractPort::~AbstractPort(){
    // all connections must first be removed
    // yea base destructor is called last - whereas it should be called first here really.
    // but this assertion will fail either way if destruction was handled wrong (assuming
    // no other error related to this occurring first....)
    assert( Connections.size() == 0);
  };


  void AbstractPort::addConnection(AbstractPort * port){
    std::cout << Name << ".addConnection " << port->Name << std::endl;

    addConnectionImpl(port);

    // using a map to make sure that connection is set only once.
    Connections[port->getKey()] = port;
    // Connections.push_back(port);

    // std::cout << "#con = " << Connections.size() << std::endl;

    if (getDeviceState() == DeviceStateConnected){
      start();
    }
  }

  void AbstractPort::removeConnection(AbstractPort * port){
    // std::cout << Name << ".removeConnection " << port->Name << std::endl;

    removeConnectionImpl(port);
    // Connections.erase(std::remove(Connections.begin(), Connections.end(), port));
    Connections.erase(port->getKey());

    // std::cout << "#con = " << Connections.size() << std::endl;

    if (Connections.size() == 0){
      stop();
    }
  }

  void AbstractPort::onDeviceConnected(){
    // std::cout << "onDeviceConnected " << Connections.size() << std::endl;
    if (Connections.size() > 0){
      start();
    }
  }

  void AbstractPort::onDeviceDisconnected(){
    // if (Connections.size() == 0){
      stop();
    // }
  }

}
