#include <Port/AbstractControlPort.hpp>

#include <cassert>

namespace MidiPatcher {

  uint8_t AbstractControlPort::packMessage(unsigned char * sysex, unsigned char * data, uint8_t dataLen){
    assert( sysex != nullptr );
    assert( data != nullptr );
    assert( dataLen <= 0x7F );

    uint8_t len = 0;

    // header (sysex experimental and marker bytes)
    std::memcpy(sysex, ControlPortHeader, sizeof(ControlPortHeader));
    len += sizeof(ControlPortHeader);

    sysex[len++] = dataLen;

    for(int i = 0; i < dataLen; i++){
      sysex[len++] = (data[i] >> 4) & 0x0F;
      sysex[len++] = (data[i] & 0x0F);
    }

    std::memcpy(sysex, ControlPortTail, sizeof(ControlPortTail));
    len += sizeof(ControlPortTail);

    return len;
  }

  uint8_t AbstractControlPort::unpackMessage(unsigned char * data, unsigned char * sysex, uint8_t sysexLen){
    assert( sysex != nullptr );
    assert( data != nullptr );

    // basic validation
    if (sysexLen < (sizeof(ControlPortHeader) + sizeof(ControlPortTail))){
      return 0;
    }

    uint8_t len = sysex[sizeof(ControlPortHeader)];

    if (sysexLen != 2*len + sizeof(ControlPortHeader) + sizeof(ControlPortTail) ){
      return 0;
    }

    if (std::memcmp(sysex, ControlPortHeader, sizeof(ControlPortHeader)) != 0){
      return 0;
    }
    if (std::memcmp(&sysex[sysexLen - sizeof(ControlPortTail)], ControlPortTail, sizeof(ControlPortTail)) != 0){
      return 0;
    }

    for(int i = 0, p = sizeof(ControlPortHeader) + 1; i < len; i++){
      data[i] = sysex[p++] << 4;
      data[i] = sysex[p++] & 0x0F;
    }

    return len;
  }

}
