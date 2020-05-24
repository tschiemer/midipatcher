#include <Port/AbstractMessageOutputPort.hpp>

#include <midimessage.h>
#include <midimessage/stringifier.h>

namespace MidiPatcher {
  namespace Port {

    void AbstractMessageOutputPort::sendMessageImpl(unsigned char * message, size_t len){

      uint8_t msgBuffer[128];
      MidiMessage::Message_t msg = {
        .Data = {
          .SysEx = {
            .ByteData = msgBuffer
          }
        }
      };

      if (MidiMessage::unpack(message, len, &msg) == false){
        return;
      }

      unsigned char outBuffer[128];
      int outLen = MidiMessage::MessagetoString(outBuffer, &msg);

      if (outLen <= 0){
        return;
      }

      // outBuffer[outLen++] = '\n';

      writeStringMessage(outBuffer, outLen);
    }

  }
}
