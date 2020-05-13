#ifndef ABSTRACT_OUTPUT_PORT_H
#define ABSTRACT_OUTPUT_PORT_H

#include "AbstractPort.hpp"

namespace MidiPatcher {
  class AbstractOutputPort : public virtual AbstractPort {

    protected:
      
      AbstractOutputPort(){};
      AbstractOutputPort(std::string name){
          Type = TypeOutput;
          Name = name;
      };

    public:

      virtual void send(unsigned char * message, size_t len) = 0;
      virtual void send(std::vector<unsigned char> * message){
        send(&message->at(0), message->size());
      }
  };
}

#endif /* ABSTRACT_OUTPUT_PORT_H */
