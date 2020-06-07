#include <Port/RemoteControlPort.hpp>

namespace MidiPatcher {
  namespace Port {

    RemoteControlPort::RemoteControlPort(std::string name, std::string delimiter) : RemoteControlPort(std::cin, std::cout, name, delimiter) {

    }

    RemoteControlPort::RemoteControlPort(std::istream &in, std::ostream &out, std::string name, std::string delimiter) : CLI(in,out,delimiter), AbstractInputOutputPort(name){

    }

    void RemoteControlPort::sendCommand(std::vector<std::string> &argv){

    }

  }
}
