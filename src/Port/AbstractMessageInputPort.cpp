#include <Port/AbstractMessageInputPort.hpp>

#include <midimessage.h>
#include <midimessage/stringifier.h>

namespace MidiPatcher {
  namespace Port {

    void AbstractMessageInputPort::readLine(unsigned char * line, size_t len){

      uint8_t bytes[128];

      assert( len < sizeof(bytes) );

      std::memcpy(bytes, line, len);

      uint8_t msgBuffer[128];
      MidiMessage::Message_t msg = {
        .Data = {
          .SysEx = {
            .ByteData = msgBuffer
          }
        }
      };


      if (MidiMessage::StringifierResultOk != MidiMessage::MessagefromString(&msg, len, bytes)){
        return;
      }

      int midiLen = MidiMessage::pack( bytes, &msg );
      if (midiLen <= 0){
        return;
      }

      receivedMessage(bytes, midiLen);
    }

  }
}
