#ifndef CONTROL_PORT_H
#define CONTROL_PORT_H

#include "AbstractControlPort.hpp"

#include <cassert>

namespace MidiPatcher {

  class PortRegistry;

  class ControlPort : public virtual AbstractControlPort {

    public:

      static const constexpr char PortClass[] = "ControlPort";

      std::string getKey(){
        return PortClass;
      }

      ControlPort(PortRegistry * portRegistry) : AbstractInputOutputPort("ControlPort"){
        assert(portRegistry != nullptr);

        PortRegistryRef = portRegistry;
      }

      void sendMessage(unsigned char * message, size_t len);

    protected:

      PortRegistry * PortRegistryRef;

  };

}

#endif /* CONTROL_PORT_H */
