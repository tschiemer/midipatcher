#include <Port/Serial.hpp>

namespace MidiPatcher {
  namespace Port {

    std::vector<AbstractPort*>  * Serial::scanner(){
      return new std::vector<AbstractPort*>();
    }

    Serial::Serial(std::string portName, bool runningStatusEnabled){

    }

    Serial::~Serial(){

    }

    void Serial::writeToStream(unsigned char * data, size_t len){

    }
  }
}
