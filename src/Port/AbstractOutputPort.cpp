#include <Port/AbstractOutputPort.hpp>

namespace MidiPatcher {

  void AbstractOutputPort::sendMessage(unsigned char * message, size_t len){
      if (getDeviceState() != DeviceStateConnected){
        return;
      }

      sendMessageImpl( message, len );
  }

}
