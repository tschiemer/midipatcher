#include <Port/AbstractMessageInputPort.hpp>

#include <Log.hpp>

#include <midimessage/stringifier.h>


namespace MidiPatcher {
  namespace Port {

    void AbstractMessageInputPort::readLine(unsigned char * line, size_t len){

      if (len > InMsgBufferSize){
        Log::warning(getKey(), "discarding! line " + std::to_string(len) + " exceeds buffer "+std::to_string(InMsgBufferSize));
        return;
      }

      // copy to owned buffer because MessageFromString alters the given memory
      std::memcpy(InMsgTmpBuffer, line, len);

      if (MidiMessage::StringifierResultOk != MidiMessage::MessagefromString(&InMsg, len, InMsgTmpBuffer)){
        return;
      }

      int midiLen = MidiMessage::pack( InMsgTmpBuffer, &InMsg );
      if (midiLen <= 0){
        return;
      }

      receivedMessage(InMsgTmpBuffer, midiLen);
    }

  }
}
