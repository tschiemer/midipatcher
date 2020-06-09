#ifndef MIDIPATCHER_PORT_ABSTRACT_STREAM_INPUT_OUTPUT_PORT_H
#define MIDIPATCHER_PORT_ABSTRACT_STREAM_INPUT_OUTPUT_PORT_H

#include "AbstractStreamInputPort.hpp"
#include "AbstractStreamOutputPort.hpp"

namespace MidiPatcher {
  namespace Port {
    class AbstractStreamInputOutputPort : public virtual AbstractStreamInputPort, public virtual AbstractStreamOutputPort {

      public:

        virtual bool hasOption(std::string key){
            if (key == "runningstatus") return true;
            if (key == "irunningstatus") return true;
            if (key == "orunningstatus") return true;

            return false;
        }

        virtual std::string getOption(std::string key);
        virtual void setOption(std::string key, std::string value);

    };
  }
}

#endif /* MIDIPATCHER_PORT_ABSTRACT_STREAM_INPUT_OUTPUT_PORT_H */
