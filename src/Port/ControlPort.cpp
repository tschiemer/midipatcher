#include <Port/ControlPort.hpp>

#include <cassert>

namespace MidiPatcher {

  void ControlPort::sendMessage(unsigned char * message, size_t len){

    uint8_t data[128];
    uint8_t dataLen = unpackMessage(data, message, len);

    if (dataLen == 0){
      return;
    }

    std::cout << "received control port message (" << dataLen << ")" << std::endl;

  }

}
