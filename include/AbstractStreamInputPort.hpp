#ifndef ABSTRACT_STREAM_INPUT_PORT_H
#define ABSTRACT_STREAM_INPUT_PORT_H

#include "AbstractInputPort.hpp"

#include <midimessage/parser.h>

namespace MidiPatcher {

  class AbstractStreamInputPort : public virtual AbstractInputPort {

    protected:

        AbstractStreamInputPort(bool runningStatusEnabled = true, int bufferSize = 128);
        ~AbstractStreamInputPort();

        uint8_t * ParserBuffer;
        uint8_t * MsgBuffer;
        MidiMessage::Parser_t Parser;
        MidiMessage::Message_t MidiMessageMem;

        void readFromStream(unsigned char * data, size_t len);

        static void midiMessageHandler(MidiMessage::Message_t * message, void * context);
        static void midiMessageDiscardHandler(uint8_t *bytes, uint8_t length, void *context);

      public:

        bool getRunningStatusEnabled(){
          return Parser.RunningStatusEnabled;
        }

        void setRunningStatusEnabled(bool enabled){
          Parser.RunningStatusEnabled = enabled;

          // MidiMessage::parser_reset( &Parser );
        }

  };

}

#endif /* ABSTRACT_STREAM_INPUT_PORT_H */
