#include <Port/AbstractStreamOutputPort.hpp>

#include <midimessage.h>
#include <Log.hpp>

namespace MidiPatcher {

  AbstractStreamOutputPort::AbstractStreamOutputPort(bool runningStatusEnabled){

    OutRunningStatusEnabled = runningStatusEnabled;
    OutRunningStatusState = MidiMessage::MidiMessage_RunningStatusNotSet;

  }

  void AbstractStreamOutputPort::setOutRunningStatusEnabled(bool enabled){
    OutRunningStatusEnabled = enabled;

    if (enabled){
      OutRunningStatusState = MidiMessage::MidiMessage_RunningStatusNotSet;
    }
  }

  void AbstractStreamOutputPort::sendMessageImpl(unsigned char * message, size_t len){


    if (OutRunningStatusEnabled && MidiMessage::updateRunningStatus( &OutRunningStatusState, message[0] )){

        Log::debug(getKey(), "writing to stream", &message[1], len-1);

        writeToStream( &message[1], len-1);
    } else {

        Log::debug(getKey(), "writing to stream", message, len);

        writeToStream(message, len);
    }
  }

}
