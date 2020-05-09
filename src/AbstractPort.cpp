#include <AbstractPort.hpp>

#include "PortRegistry.hpp"

namespace MidiPatcher {


  AbstractPort::AbstractPort(PortRegistry * portRegistry, Type_t type, std::string name){

    Type = type;
    Name = name;

    portRegistry->registerPort( this );

  };

  AbstractPort::~AbstractPort(){
    // all connections must first be removed
    // yea base destructor is called last - whereas it should be called first here really.
    // but this assertion will fail either way if destruction was handled wrong (assuming
    // no other error related to this occurring first....)
    assert( Connections.size() == 0);
  };


  void AbstractPort::addConnection(AbstractPort * port){
    addConnectionImpl(port);
    Connections.push_back(port);

    start();
  }

  void AbstractPort::removeConnection(AbstractPort * port){
    removeConnectionImpl(port);
    Connections.erase(std::remove(Connections.begin(), Connections.end(), port));

    if (Connections.size() == 0){
      stop();
    }
  }

  void AbstractPort::onDeviceConnected(){
    if (Connections.size() > 0){
      start();
    }
  }

  void AbstractPort::onDeviceDisconnected(){
    if (Connections.size() == 0){
      stop();
    }
  }

}
