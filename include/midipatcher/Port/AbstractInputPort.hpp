#ifndef ABSTRACT_INPUT_PORT_H
#define ABSTRACT_INPUT_PORT_H

#include "AbstractPort.hpp"

namespace MidiPatcher {
   class AbstractInputPort : public virtual AbstractPort {

    protected:

      AbstractInputPort(){}
      AbstractInputPort(std::string name){
        Type = TypeInput;
        Name = name;
      }
      ~AbstractInputPort(){
        // std::cout << "~AbstractInputPort" << std::endl;
      }

    public:

      virtual void receivedMessage(unsigned char * message, size_t len );

  };
}

#endif /* ABSTRACT_INPUT_PORT_H */
