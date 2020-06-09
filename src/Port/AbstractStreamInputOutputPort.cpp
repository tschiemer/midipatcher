#include <Port/AbstractStreamInputOutputPort.hpp>

namespace MidiPatcher {
  namespace Port {

    std::string AbstractStreamInputOutputPort::getOption(std::string key){

      if (key == "runningstatus" || key == "irunningstatus"){
        return AbstractStreamInputPort::getOption("runningstatus");
      }

      if (key == "orunningstatus"){
        return AbstractStreamOutputPort::getOption("runningstatus");
      }

      return "";
    }

    void AbstractStreamInputOutputPort::setOption(std::string key, std::string value){

      if (key == "runningstatus"){
        AbstractStreamInputPort::setOption("runningstatus",value);
        AbstractStreamOutputPort::setOption("runningstatus",value);
        return;
      }

      if (key == "irunningstatus"){
        return AbstractStreamInputPort::setOption("runningstatus",value);
      }

      if (key == "orunningstatus"){
        return AbstractStreamOutputPort::setOption("runningstatus",value);
      }
    }
  }
}
