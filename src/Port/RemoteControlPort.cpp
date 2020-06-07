#include <Port/RemoteControlPort.hpp>

#include <Port/ControlPort.hpp>

namespace MidiPatcher {
  namespace Port {


    void RemoteControlPort::sendCommand(std::vector<std::string> &argv){
      ControlPort::Message message(argv);

      message.sendFrom(this);
    }

    void RemoteControlPort::sendMessageImpl(unsigned char * message, size_t len){
      InMessage.receivedPart(message, len);

      if (InMessage.complete() == false){
        return;
      }

      receivedResponse(InMessage.getArgv());

      InMessage.clear();
    }

  }
}
