#ifndef ABSTRACT_INPUT_PORT_H
#define ABSTRACT_INPUT_PORT_H

#include "AbstractPort.hpp"

#include <iostream>

namespace MidiPatcher {
   class AbstractInputPort : public virtual AbstractPort {

    protected:

      AbstractInputPort(){}
      AbstractInputPort(std::string name){
        Type = TypeInput;
        Name = name;
      }

      void received(unsigned char * message, size_t len );
      void received(std::vector<unsigned char> *message ){
        received(&message->at(0), message->size());
      }
  };
}

#endif /* ABSTRACT_INPUT_PORT_H */
