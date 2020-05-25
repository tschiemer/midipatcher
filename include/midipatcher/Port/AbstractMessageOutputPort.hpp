#ifndef ABSTRACT_STREAM_MESSAGE_H
#define ABSTRACT_STREAM_MESSAGE_H

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

#endif /* ABSTRACT_MSG_GENERATOR_H */
