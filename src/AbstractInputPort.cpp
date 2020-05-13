#include <AbstractInputPort.hpp>
#include <AbstractOutputPort.hpp>

namespace MidiPatcher {

  void AbstractInputPort::received(std::vector<unsigned char> * message ){
      std::for_each(this->Connections.begin(), this->Connections.end(), [message](std::pair<std::string,AbstractPort*> pair){
        dynamic_cast<AbstractOutputPort*>(pair.second)->send(message);
      });
  }

  void AbstractInputPort::received(unsigned char * message, size_t len ){

    std::for_each(this->Connections.begin(), this->Connections.end(), [message,len](std::pair<std::string,AbstractPort*> pair){
      dynamic_cast<AbstractOutputPort*>(pair.second)->send(message,len);
    });
  }

}
