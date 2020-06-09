#include <Port/AbstractStreamOutputPort.hpp>

#include <midimessage.h>
#include <Log.hpp>

namespace MidiPatcher {

  AbstractStreamOutputPort::AbstractStreamOutputPort(bool runningStatusEnabled){

    OutRunningStatusEnabled = runningStatusEnabled;
    OutRunningStatusState = MidiMessage::MidiMessage_RunningStatusNotSet;

  }


  std::string AbstractStreamOutputPort::getOption(std::string key){
    if (key == "runningstatus") {
      return std::to_string(getOutRunningStatusEnabled());
    }

    throw Error(getKey(), "Unrecognized option " + key);
  }

  void AbstractStreamOutputPort::setOption(std::string key, std::string value){
    if (key == "runningstatus") {
      bool enabled = value.size() == 0 || std::stoi(value);
      setOutRunningStatusEnabled(enabled);
      return;
    }

    throw Error(getKey(), "Unrecognized option " + key);
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
