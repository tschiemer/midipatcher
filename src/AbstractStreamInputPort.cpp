#include <AbstractStreamInputPort.hpp>

#include <cstdlib>
#include <cassert>

namespace MidiPatcher {

  AbstractStreamInputPort::AbstractStreamInputPort(bool runningStatusEnabled, int bufferSize){

    assert( 3 <= bufferSize );

    ParserBuffer = (uint8_t*)malloc(bufferSize);
    MsgBuffer = (uint8_t*)malloc(bufferSize);

    MidiMessageMem.Data.SysEx.ByteData = MsgBuffer;

    parser_init(&Parser, runningStatusEnabled, ParserBuffer, bufferSize, &MidiMessageMem, midiMessageHandler, midiMessageDiscardHandler, this);
  }

  AbstractStreamInputPort::~AbstractStreamInputPort(){
    free(ParserBuffer);
    free(MsgBuffer);
  }

  void AbstractStreamInputPort::readFromStream(uint8_t * data, size_t len){
    // std::cout << "readFromStream " << len << std::endl;
    parser_receivedData(&Parser, data, len);
  }

  void AbstractStreamInputPort::midiMessageHandler(MidiMessage::Message_t * message, void * context){
    assert(context != NULL);

    AbstractStreamInputPort * self = (AbstractStreamInputPort*)context;

    unsigned char bytes[128];
    size_t len = pack( (uint8_t*)bytes, message );

    self->receivedMessage(bytes, len);
  }

  void AbstractStreamInputPort::midiMessageDiscardHandler(uint8_t *bytes, uint8_t length, void *context){
    // std::cout << "Discarding ";
    // for(int i = 0; i < length; i++){
    //   std::cout << std::hex << (int)bytes[i] << " ";
    // }
    // std::cout << std::endl;
  }

}
