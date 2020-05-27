#include <Port/AbstractPort.hpp>

#include <PortRegistry.hpp>

#include <Log.hpp>

namespace MidiPatcher {


  AbstractPort::AbstractPort(Type_t type, std::string name){

    Type = type;
    Name = name;

  };

  AbstractPort::~AbstractPort(){
    // all connections must first be removed
    // yea base destructor is called last - whereas it should be called first here really.
    // but this assertion will fail either way if destruction was handled wrong (assuming
    // no other error related to this occurring first....)
    // assert( Connections.size() == 0);

    // stop();
    // std::cout << "~AbstractPort" << std::endl;
  };


  void AbstractPort::setDeviceState(DeviceState_t newState){

    if (DeviceState == newState){
      return;
    }

    DeviceState = newState;

    if (DeviceState == DeviceStateConnected){
      onDeviceConnected();
    } else {
      onDeviceDisconnected();
    }

    publishDeviceStateChanged();
  }


  std::vector<AbstractPort *> * AbstractPort::getConnections(){
    std::vector<AbstractPort *> * connections = new std::vector<AbstractPort*>();

    for(std::map<std::string,AbstractPort*>::iterator it = Connections.begin(); it != Connections.end(); it++){
      connections->push_back( it->second );
    }

    return connections;
  }

  void AbstractPort::addConnection(AbstractPort * port){

    Connections[port->getKey()] = port;

    addConnectionImpl(port);

    if (getDeviceState() == DeviceStateConnected){
      start();
    }
  }

  void AbstractPort::removeConnection(AbstractPort * port){

    Connections.erase(port->getKey());

    removeConnectionImpl(port);

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
    stop();
  }


  void AbstractPort::publishDeviceStateChanged(){

    std::for_each(PortUpdateReceiverList.begin(), PortUpdateReceiverList.end(), [this](PortUpdateReceiver* receiver){
      receiver->deviceStateChanged( this, this->getDeviceState() );
    });

  }

  void AbstractPort::subscribePortUpdateReveicer(PortUpdateReceiver *receiver){
    PortUpdateReceiverList.push_back(receiver);
  }

  void AbstractPort::unsubscribePortUpdateReveicer(PortUpdateReceiver *receiver){
    PortUpdateReceiverList.erase(std::remove(PortUpdateReceiverList.begin(), PortUpdateReceiverList.end(), receiver));
  }

}
