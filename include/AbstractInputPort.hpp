#ifndef ABSTRACT_INPUT_PORT_H
#define ABSTRACT_INPUT_PORT_H

#include "AbstractPort.hpp"

#include <iostream>

namespace MidiPatcher {
   class AbstractInputPort : public virtual AbstractPort {
    public:
      AbstractInputPort(){}
      // AbstractInputPort(PortRegistry * portRegistry) : AbstractPort(portRegistry){};
  };
}

#endif /* ABSTRACT_INPUT_PORT_H */
