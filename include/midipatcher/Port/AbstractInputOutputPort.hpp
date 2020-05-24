#ifndef ABSTRACT_INPUT_OUTPUT_PORT_H
#define ABSTRACT_INPUT_OUTPUT_PORT_H

#include "AbstractInputPort.hpp"
#include "AbstractOutputPort.hpp"

namespace MidiPatcher {

  class AbstractInputOutputPort : public virtual AbstractInputPort, public virtual AbstractOutputPort {

    protected:

      AbstractInputOutputPort(){}
      AbstractInputOutputPort(std::string name){
        Type = TypeInputOutput;
        Name = name;
      }

  };

}

#endif /* ABSTRACT_INPUT_OUTPUT_PORT_H */
