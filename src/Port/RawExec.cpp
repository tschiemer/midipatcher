#include <Port/RawExec.hpp>

#include <PortRegistry.hpp>

#include <Log.hpp>
#include <Error.hpp>

#include <cassert>
#include <cstdlib>
#include <string>
#include <map>

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>

namespace MidiPatcher {
  namespace Port {

    AbstractPort* RawExec::factory(PortDescriptor &portDescriptor){
      assert( portDescriptor.PortClass == PortClass );

      std::string execpath;
      std::string argvStr = "";

      // has exec been specifically specified?
      if (portDescriptor.Options.count("exec") > 0){
        execpath = portDescriptor.Options["exec"];
      } else {
        // otherwise assume the name is the execpath
        execpath = portDescriptor.Name;
      }

      if (portDescriptor.Options.count("argv") > 0){
        argvStr = portDescriptor.Options["argv"];
      }


      return new RawExec( portDescriptor.Name, execpath, argvStr );
    }

    RawExec::RawExec(std::string portName, std::string execpath, std::string argvStr) : AbstractInputOutputPort(portName), AbstractExecPort(execpath, argvStr) {
      setDeviceState( DeviceStateConnected );
    }

    RawExec::~RawExec(){

    }

    void RawExec::writeToStream(unsigned char * message, size_t len){
      write(ToExecFDs[1], message, len);
    }

    void RawExec::readFromExec(unsigned char * buffer, size_t len ){
      readFromStream(buffer, len);
    }
  }
}
