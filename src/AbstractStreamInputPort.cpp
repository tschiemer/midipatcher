#include <AbstractStreamInputPort.hpp>

namespace MidiPatcher {

  void AbstractStreamInputPort::receivedStreamData(uint8_t &data, size_t len){
    parser_receivedData(&Parser, &data, len);
  }

  void AbstractStreamInputPort::midiMessageHandler(MidiMessage::Message_t * message, void * context){
    assert(context != NULL);

    AbstractStreamInputPort * self = (AbstractStreamInputPort*)context;

    unsigned char bytes[128];
    size_t len = pack( (uint8_t*)bytes, message );

    self->received(bytes, len);
  }

  void AbstractStreamInputPort::midiMessageDiscardHandler(uint8_t *bytes, uint8_t length, void *context){
    // std::cout << "Discarding ";
    // for(int i = 0; i < length; i++){
    //   std::cout << std::hex << (int)bytes[i] << " ";
    // }
    // std::cout << std::endl;
  }

}
