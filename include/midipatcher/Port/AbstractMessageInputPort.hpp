#ifndef MIDIPATCHER_PORT_ABSTRACT_MESSAGE_INPUT_PORT_H
#define MIDIPATCHER_PORT_ABSTRACT_MESSAGE_INPUT_PORT_H

#include "AbstractInputPort.hpp"

#include <midimessage.h>

namespace MidiPatcher {
  namespace Port {

    class AbstractMessageInputPort : public virtual AbstractInputPort {

      protected:

        size_t InMsgBufferSize;
        uint8_t * InMsgTmpBuffer;
        MidiMessage::Message_t InMsg;

        AbstractMessageInputPort(size_t bufferSize = 256){
          assert( bufferSize > 64 );

          InMsgBufferSize = bufferSize;
          InMsgTmpBuffer = (uint8_t*)malloc(bufferSize);
          InMsg.Data.SysEx.ByteData = (uint8_t*)malloc(bufferSize);
        }

        ~AbstractMessageInputPort(){
          std::free(InMsgTmpBuffer);
          std::free(InMsg.Data.SysEx.ByteData);
        }

        void readLine(unsigned char * line, size_t len);

    };

  }
}

#endif /* MIDIPATCHER_PORT_ABSTRACT_MESSAGE_INPUT_PORT_H */
