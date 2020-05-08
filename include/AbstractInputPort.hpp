#ifndef ABSTRACT_INPUT_PORT_H
#define ABSTRACT_INPUT_PORT_H

#include "AbstractPort.hpp"

#include <iostream>

namespace MidiPatcher {
   class AbstractInputPort : public virtual AbstractPort {

    public:
      AbstractInputPort(){
        // Type = TypeInput;
      }
      // AbstractInputPort(PortRegistry * portRegistry, std::string name) : AbstractPort(portRegistry, TypeInput, name) {
      //   std::cout << "muuu" << std::endl;
      // };


      // virtual void foo(AbstractInputPort*p){};
  };
}

#endif /* ABSTRACT_INPUT_PORT_H */
