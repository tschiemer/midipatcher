#include <Port/MsgExec.hpp>

#include <PortRegistry.hpp>

#include <unistd.h>

namespace MidiPatcher {
  namespace Port {


    AbstractPort* MsgExec::factory(PortDescriptor * portDescriptor){

      std::string execpath;
      std::string argvStr = "";

      // has exec been specifically specified?
      if (portDescriptor->Options.count("exec") > 0){
        execpath = portDescriptor->Options["exec"];
      } else {
        // otherwise assume the name is the execpath
        execpath = portDescriptor->Name;
      }

      if (portDescriptor->Options.count("argv") > 0){
        argvStr = portDescriptor->Options["argv"];
      }


      return new MsgExec( portDescriptor->Name, execpath, argvStr );
    }

    MsgExec::MsgExec(std::string portName, std::string execpath, std::string argvStr) : AbstractInputOutputPort(portName), AbstractExecPort(execpath, argvStr) {
      setDeviceState( DeviceStateConnected );
    }

    MsgExec::~MsgExec(){
      // stop();
    }

    void MsgExec::writeStringMessage(unsigned char * stringMessage, size_t len){
      stringMessage[len++] = '\n';
      // stringMessage[len] = '\0';

      // std::cerr << "writing: " << stringMessage << std::endl;

      writeToExec(stringMessage, len);
    }

    void MsgExec::readFromExec(unsigned char * buffer, size_t len ){
      // readFromStream(buffer, len);
      readLine(buffer, len);
    }

  }
}
