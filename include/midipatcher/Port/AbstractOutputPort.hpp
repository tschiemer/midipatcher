#ifndef MIDIPATCHER_PORT_ABSTRACT_OUTPUT_PORT_H
#define MIDIPATCHER_PORT_ABSTRACT_OUTPUT_PORT_H

#include "AbstractPort.hpp"

namespace MidiPatcher {
  class AbstractOutputPort : public virtual AbstractPort {

    protected:

      AbstractOutputPort(){};
      AbstractOutputPort(std::string name){
        Type = TypeOutput;
        Name = name;
      };

      virtual void sendMessageImpl(unsigned char * message, size_t len) = 0;

    public:

      virtual void sendMessage(unsigned char * message, size_t len);
  };
}

#endif /* MIDIPATCHER_PORT_ABSTRACT_OUTPUT_PORT_H */
