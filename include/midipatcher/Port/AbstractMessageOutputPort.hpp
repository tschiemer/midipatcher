#ifndef ABSTRACT_STREAM_MESSAGE_H
#define ABSTRACT_STREAM_MESSAGE_H

#include "AbstractOutputPort.hpp"

namespace MidiPatcher {
  namespace Port {

    class AbstractMessageOutputPort : public virtual AbstractOutputPort {

      protected:

        virtual void writeStringMessage(unsigned char * stringMessage, size_t len) = 0;

        virtual void sendMessageImpl(unsigned char * message, size_t len);

    };

  }
}

#endif /* ABSTRACT_MSG_GENERATOR_H */
