#include <Port/AbstractMessageOutputPort.hpp>

#include <Log.hpp>

#include <midimessage/stringifier.h>

namespace MidiPatcher {
  namespace Port {

    void AbstractMessageOutputPort::sendMessageImpl(unsigned char * message, size_t len){

      if (len > OutMsgBufferSize){
        Log::warning(getKey(), "discarding! message " + std::to_string(len) + " already exceeds buffer "+std::to_string(OutMsgBufferSize));
        return;
      }

      if (MidiMessage::unpack(message, len, &OutMsg) == false){
        return;
      }

      int outLen = MidiMessage::MessagetoString(OutMsgTmpBuffer, &OutMsg);

      if (outLen <= 0){
        return;
      }

      // outBuffer[outLen++] = '\n';

      writeStringMessage(OutMsgTmpBuffer, outLen);
    }

  }
}
