#include <Port/FileOut.hpp>
#include <PortRegistry.hpp>

#include <Log.hpp>
#include <Error.hpp>

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace MidiPatcher {
  namespace Port {

    FileOut::FileOut(std::string portName) : AbstractOutputPort(portName){

        if (portName == FILE_STDOUT){

          Log::info(getKey(), "using STDOUT");

          FD = 1;

          setDeviceState(DeviceStateConnected);

        } else if (portName == FILE_STDERR){

          Log::info(getKey(), "using STDERR");

          FD = 2;

          setDeviceState(DeviceStateConnected);

        } else {

          int flags = O_WRONLY | O_CREAT | O_APPEND | O_NONBLOCK;

          // Log::info(getKey() + " opening " + portName);
          this->FD = open(portName.c_str(), flags);
          if (this->FD == -1){
            throw Error(getKey(), " failed to open: " + std::to_string(errno));
          }
          Log::info(getKey(), "opened with FD = " + std::to_string(FD));

          this->setDeviceState(DeviceStateConnected);

        }

    }

    FileOut::~FileOut(){

      stop();

      if (FD > 2){
        close(FD);
      }
    }

    void FileOut::writeToStream(unsigned char *data, size_t len){
      write( FD, data, len );
    }

  }
}
