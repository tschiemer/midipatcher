#include <Port/AbstractInputPort.hpp>
#include <Port/AbstractOutputPort.hpp>

namespace MidiPatcher {

  void AbstractInputPort::receivedMessage(unsigned char * message, size_t len ){

    std::for_each(this->Connections.begin(), this->Connections.end(), [message,len](Connection &connection){

      if (connection.Type != TypeOutput){
        return;
      }

      AbstractOutputPort* output = dynamic_cast<AbstractOutputPort*>(connection.Port);

      output->sendMessage(message,len);

    });
  }

}
