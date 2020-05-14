#include <AbstractStreamOutputPort.hpp>

#include <midimessage.h>

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

  void AbstractStreamOutputPort::sendMessage(unsigned char * message, size_t len){

    if (RunningStatusEnabled && MidiMessage::updateRunningStatus( &RunningStatusState, message[0] )){
        writeToStream( &message[1], len-1);
    } else {
        writeToStream(message, len);
    }
  }

}
