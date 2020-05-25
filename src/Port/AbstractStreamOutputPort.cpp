#include <Port/AbstractStreamOutputPort.hpp>

#include <midimessage.h>
#include <Log.hpp>

namespace MidiPatcher {

  AbstractStreamOutputPort::AbstractStreamOutputPort(bool runningStatusEnabled){

    RunningStatusEnabled = runningStatusEnabled;
    RunningStatusState = MidiMessage::MidiMessage_RunningStatusNotSet;

  }

  void AbstractStreamOutputPort::setRunningStatusEnabled(bool enabled){
    RunningStatusEnabled = enabled;

    if (enabled){
      RunningStatusState = MidiMessage::MidiMessage_RunningStatusNotSet;
    }
  }

  void AbstractStreamOutputPort::sendMessageImpl(unsigned char * message, size_t len){


    if (RunningStatusEnabled && MidiMessage::updateRunningStatus( &RunningStatusState, message[0] )){

        Log::debug(getKey(), "writing to stream", &message[1], len-1);

        writeToStream( &message[1], len-1);
    } else {

        Log::debug(getKey(), "writing to stream", message, len);

        writeToStream(message, len);
    }
  }

}
