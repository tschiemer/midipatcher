#include <Port/AbstractInputPort.hpp>
#include <Port/AbstractOutputPort.hpp>

namespace MidiPatcher {

  void AbstractInputPort::receivedMessage(unsigned char * message, size_t len ){

    std::for_each(this->Connections.begin(), this->Connections.end(), [message,len](std::pair<std::string,AbstractPort*> pair){
      AbstractOutputPort* output = dynamic_cast<AbstractOutputPort*>(pair.second);

      if (output != NULL){
        output->sendMessage(message,len);
      }

    });
  }

}
