#ifndef ABSTRACT_MESSAGE_INPUT_PORT_H
#define ABSTRACT_MESSAGE_INPUT_PORT_H

#include "AbstractInputPort.hpp"

namespace MidiPatcher {
  namespace Port {

    class AbstractMessageInputPort : public virtual AbstractInputPort {

      protected:

        void readLine(unsigned char * line, size_t len);

    };

  }
}

#endif /* ABSTRACT_MESSAGE_INPUT_PORT_H */
