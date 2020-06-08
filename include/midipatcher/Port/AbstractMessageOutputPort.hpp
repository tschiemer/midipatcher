#ifndef MIDIPATCHER_PORT_ABSTRACT_MSG_OUTPUT_PORT_H
#define MIDIPATCHER_PORT_ABSTRACT_MSG_OUTPUT_PORT_H

#include "AbstractOutputPort.hpp"

#include <midimessage.h>

namespace MidiPatcher {
  namespace Port {

    class AbstractMessageOutputPort : public virtual AbstractOutputPort {

      protected:

        size_t OutMsgBufferSize;
        uint8_t * OutMsgTmpBuffer;
        MidiMessage::Message_t OutMsg;

        AbstractMessageOutputPort(size_t bufferSize = 256){
          assert( bufferSize >= 64 );

          OutMsgBufferSize = bufferSize;
          OutMsgTmpBuffer = (uint8_t*)malloc(bufferSize);
          OutMsg.Data.SysEx.ByteData = (uint8_t*)malloc(bufferSize);
        }

        ~AbstractMessageOutputPort(){
          std::free(OutMsgTmpBuffer);
          std::free(OutMsg.Data.SysEx.ByteData);
        }

        virtual void writeStringMessage(unsigned char * stringMessage, size_t len) = 0;

        virtual void sendMessageImpl(unsigned char * message, size_t len);

    };

  }
}

#endif /* MIDIPATCHER_PORT_ABSTRACT_MSG_OUTPUT_PORT_H */
