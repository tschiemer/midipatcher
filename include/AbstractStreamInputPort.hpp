#ifndef ABSTRACT_STREAM_INPUT_PORT_H
#define ABSTRACT_STREAM_INPUT_PORT_H

// #include <cstdlib>

#include "AbstractInputPort.hpp"

// #include <midimessage.h>
#include <midimessage/parser.h>

namespace MidiPatcher {

  class AbstractStreamInputPort : public virtual AbstractInputPort {

    protected:

        uint8_t ParserBuffer[128];
        uint8_t MsgBuffer[128];
        MidiMessage::Parser_t Parser;
        bool RunningStatusEnabled;
        MidiMessage::Message_t MidiMessageMem = {
          .Data = {
            .SysEx = {
              .ByteData = MsgBuffer
            }
          }
        };

        AbstractStreamInputPort(bool runningStatusEnabled);

        void receivedStreamData(uint8_t &data, size_t len);

        static void midiMessageHandler(MidiMessage::Message_t * message, void * context);
        static void midiMessageDiscardHandler(uint8_t *bytes, uint8_t length, void *context);

  };

}

#endif /* ABSTRACT_STREAM_INPUT_PORT_H */
