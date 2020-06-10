#include <Port/AbstractPort.hpp>
#include <Port/AbstractInputPort.hpp>
#include <Port/AbstractOutputPort.hpp>

#include <PortRegistry.hpp>

#include <Log.hpp>

namespace MidiPatcher {

  AbstractPort::Message::Message(std::vector<uint8_t> &data) : Data(data){

  }

  bool AbstractPort::Message::empty(){
    return Data.empty();
  }

  void AbstractPort::Message::clear(){
    Data.clear();
  }

  void AbstractPort::Message::transmit(AbstractPort * port, void (*tx)(AbstractPort * port, unsigned char * message, size_t len)){
    if (Data.size() == 0){
      return;
    }

    tx(port, &Data[0], Data.size());
  }

  void AbstractPort::Message::sendFrom(AbstractInputPort * port){
    assert( port != nullptr );

    transmit(port, [](AbstractPort * port, unsigned char * message, size_t len){
      dynamic_cast<AbstractInputPort*>(port)->receivedMessage(message, len);
    });
  }

  void AbstractPort::Message::sendTo(AbstractOutputPort * port){
    assert( port != nullptr );

    transmit(port, [](AbstractPort * port, unsigned char * message, size_t len){
      dynamic_cast<AbstractOutputPort*>(port)->sendMessage(message, len);
    });
  }

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
  };

   void AbstractPort::registerPort(PortRegistry &portRegistry){
    portRegistry.registerPort(this);
  }

   void AbstractPort::unregisterPort(PortRegistry &portRegistry){
    portRegistry.unregisterPort(this);
  }


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


  std::vector<AbstractPort *> * AbstractPort::getConnections(Type_t type){
    // assert( type == TypeAny || type == TypeInput || type == TypeOutput );

    std::vector<AbstractPort *> * connections = new std::vector<AbstractPort*>();

    for(std::vector<Connection>::iterator it = Connections.begin(); it != Connections.end(); it++){
      if (type == TypeAny || (it->Type & type) == type){
        connections->push_back( it->Port );
      }
    }

    return connections;
  }

  void AbstractPort::addConnectionAs(AbstractPort * port, Type_t type){
    assert( type == TypeInput || type == TypeOutput );
    assert( port != NULL );
    assert( (port->Type & type) == type ); // can port actually act as what it is supposed to?

    Connection con(type, port);

    // already in list? (do not allow duplicates)
    std::vector<Connection>::iterator it = std::find(Connections.begin(), Connections.end(), con);

    if (it != Connections.end()){
      return;
    }

    // Connections[port->getKey()] = port;
    Connections.push_back(con);

    addConnectionImpl(con);

    if (getDeviceState() == DeviceStateConnected){
      start();
    }
  }

  void AbstractPort::removeConnectionAs(AbstractPort * port, Type_t type){
    assert( type == TypeInput || type == TypeOutput );
    assert( port != NULL );
    assert( (port->Type & type) == type ); // can port actually act as what it is supposed to?

    Connection con(type, port);

    Connections.erase(std::remove(Connections.begin(), Connections.end(), con));

    removeConnectionImpl(con);

    if (Connections.size() == 0){
      stop();
    }

  }

  bool AbstractPort::isConnectedToType(AbstractPort * port, Type_t type){

    for(std::vector<Connection>::iterator it = Connections.begin(); it != Connections.end(); it++){
      if (it->Port != port){
        continue;
      }
      if (type == TypeAny || (it->Type & type) == type){
        return true;
      }
    }

    return false;
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
