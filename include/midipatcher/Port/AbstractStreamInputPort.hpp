#ifndef MIDIPATCHER_PORT_ABSTRACT_STREAM_INPUT_PORT_H
#define MIDIPATCHER_PORT_ABSTRACT_STREAM_INPUT_PORT_H

#include "AbstractInputPort.hpp"

#include <midimessage/simpleparser.h>

namespace MidiPatcher {

  class AbstractStreamInputPort : public virtual AbstractInputPort {

    protected:

        AbstractStreamInputPort(bool runningStatusEnabled = true, size_t  bufferSize = 128);
        ~AbstractStreamInputPort();

        size_t ParserBufferSize;
        uint8_t * ParserBuffer;
        MidiMessage::SimpleParser_t Parser;

        void readFromStream(unsigned char * data, size_t len);

        static void midiMessageHandler(uint8_t * data, uint8_t len, void * context);
        static void midiMessageDiscardHandler(uint8_t *bytes, uint8_t length, void *context);

      public:

        inline bool getInRunningStatusEnabled(){
          return Parser.RunningStatusEnabled;
        }

        inline void setInRunningStatusEnabled(bool enabled){
          Parser.RunningStatusEnabled = enabled;
        }

  };

}

#endif /* MIDIPATCHER_PORT_ABSTRACT_STREAM_INPUT_PORT_H */
