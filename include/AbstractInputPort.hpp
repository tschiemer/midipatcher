#ifndef ABSTRACT_INPUT_PORT_H
#define ABSTRACT_INPUT_PORT_H

#include "AbstractPort.hpp"

#include <iostream>

namespace MidiPatcher {
   class AbstractInputPort : public virtual AbstractPort {

    public:
      AbstractInputPort(){}
      AbstractInputPort(std::string name) : AbstractPort(TypeInput, name){}

    protected:
      void send(std::vector<unsigned char> *message );
      void send(unsigned char * message, size_t len );
  };
}

#endif /* ABSTRACT_INPUT_PORT_H */
