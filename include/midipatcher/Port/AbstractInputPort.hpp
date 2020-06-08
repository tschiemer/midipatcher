#ifndef MIDIPATCHER_PORT_ABSTRACT_INPUT_PORT_H
#define MIDIPATCHER_PORT_ABSTRACT_INPUT_PORT_H

#include "AbstractPort.hpp"

namespace MidiPatcher {
   class AbstractInputPort : public virtual AbstractPort {

    friend class Message;

    protected:

      AbstractInputPort(){}
      AbstractInputPort(std::string name){
        Type = TypeInput;
        Name = name;
      }
      ~AbstractInputPort(){
        // std::cout << "~AbstractInputPort" << std::endl;
      }

      virtual void receivedMessage(unsigned char * message, size_t len );

  };
}

#endif /* MIDIPATCHER_PORT_ABSTRACT_INPUT_PORT_H */
