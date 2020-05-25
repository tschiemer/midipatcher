#include <Port/AbstractStreamInputPort.hpp>

#include <Log.hpp>

#include <cstdlib>
#include <cassert>

namespace MidiPatcher {

  AbstractStreamInputPort::AbstractStreamInputPort(bool runningStatusEnabled, size_t  bufferSize){

    assert( 3 <= bufferSize );

    ParserBufferSize = bufferSize;
    ParserBuffer = (uint8_t*)malloc(bufferSize);

    simpleparser_init(&Parser, runningStatusEnabled, ParserBuffer, bufferSize, midiMessageHandler, midiMessageDiscardHandler, this);
  }

  AbstractStreamInputPort::~AbstractStreamInputPort(){
    free(ParserBuffer);
  }

  void AbstractStreamInputPort::readFromStream(uint8_t * data, size_t len){
    simpleparser_receivedData(&Parser, data, len);
  }

  void AbstractStreamInputPort::midiMessageHandler(uint8_t * data, uint8_t len, void * context){
    assert(context != NULL);

    AbstractStreamInputPort * self = (AbstractStreamInputPort*)context;

    self->receivedMessage(data, len);
  }

  void AbstractStreamInputPort::midiMessageDiscardHandler(uint8_t *bytes, uint8_t length, void *context){

    AbstractStreamInputPort * self = (AbstractStreamInputPort*)context;

    Log::notice(self->getKey(), "discarding", bytes, length);
  }

}
