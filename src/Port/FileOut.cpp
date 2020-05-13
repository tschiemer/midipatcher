#include <Port/FileOut.hpp>
#include <PortRegistry.hpp>

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace MidiPatcher {
  namespace Port {


    FileOut::FileOut(std::string portName) : AbstractPort(TypeOutput, portName){

        if (portName == FILE_STDOUT){

          FD = 1;

          setDeviceState(DeviceStateConnected);

        } else if (portName == FILE_STDERR){

          FD = 2;

          setDeviceState(DeviceStateConnected);

        } else {

          OpenThread = std::thread([this,portName](){

            int flags = O_WRONLY | O_CREAT | O_APPEND;

            this->FD = open(portName.c_str(), flags);

            // std::cout << "FileOut.FD = " << this->FD << std::endl;

            this->setDeviceState(DeviceStateConnected);
          });
          // OpenThread.detach();

        }


      // portRegistry->registerPort( this );
    }

    FileOut::~FileOut(){

    }


    void FileOut::registerPort(PortRegistry &portRegistry){
      portRegistry.registerPort(this);
    }

    void FileOut::send(unsigned char *message, size_t len){
      if (getDeviceState() != DeviceStateConnected){
        return;
      }

      write( FD, message, len );
    }

  }
}
