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

      // virtual void receivedMessage(unsigned char * message, size_t len );

    // public:
    //
    //   virtual void sendMessage(unsigned char * message, size_t len);

  };

}

#endif /* ABSTRACT_INPUT_OUTPUT_PORT_H */
