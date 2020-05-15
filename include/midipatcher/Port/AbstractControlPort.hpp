#ifndef ABSTRACT_CONTROL_PORT_H
#define ABSTRACT_CONTROL_PORT_H

#include "AbstractInputOutputPort.hpp"

#include <cassert>

namespace MidiPatcher {

  class PortRegistry;

  class AbstractControlPort : public virtual AbstractInputOutputPort {

    public:

      // header (sysex experimental and marker bytes)
      static const constexpr uint8_t ControlPortHeader[] = {0xF0, 0x7D, 0x01, 0x03, 0x03, 0x07};

      // tail (end of sysex)
      static const constexpr uint8_t ControlPortTail[] = {0xF7};

      static uint8_t packMessage(unsigned char * sysex, unsigned char * data, uint8_t dataLen);
      static uint8_t unpackMessage(unsigned char * data, unsigned char * sysex, uint8_t sysexLen);

  };

}

#endif /* ABSTRACT_CONTROL_PORT_H */
