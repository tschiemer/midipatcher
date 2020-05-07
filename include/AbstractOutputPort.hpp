#ifndef ABSTRACT_OUTPUT_PORT_H
#define ABSTRACT_OUTPUT_PORT_H

#include "AbstractPort.hpp"

namespace MidiPatcher {
  class AbstractOutputPort : public virtual AbstractPort {

    public:
      AbstractOutputPort(){};
      virtual void send(std::vector<unsigned char> *message){};
  };
}

#endif /* ABSTRACT_OUTPUT_PORT_H */
