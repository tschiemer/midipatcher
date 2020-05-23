#include <Port/FileOut.hpp>
#include <PortRegistry.hpp>

#include <log.hpp>

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace MidiPatcher {
  namespace Port {


    FileOut::FileOut(std::string portName) : AbstractOutputPort(portName){

        if (portName == FILE_STDOUT){

          Log::print(1, "FileOut using STDOUT");

          FD = 1;

          setDeviceState(DeviceStateConnected);

        } else if (portName == FILE_STDERR){

          Log::print(1, "FileOut using STDERR");

          FD = 2;

          setDeviceState(DeviceStateConnected);

        } else {

          OpenThread = std::thread([this,portName](){
            OpenThreadRunning = true;

            int flags = O_WRONLY | O_CREAT | O_APPEND | O_NONBLOCK;

            Log::print(1, "FileOut opening " + portName);
            this->FD = open(portName.c_str(), flags);
            Log::print(1, "FileOut opened " + portName + " (FD = " + std::to_string(FD) + ")");


            this->setDeviceState(DeviceStateConnected);

            OpenThreadRunning = false;
          });

        }

    }

    FileOut::~FileOut(){
      if (OpenThreadRunning){

        Log::print(2, "FileIn[" + Name + "] OpenThread still running..");

        // OpenThread.~thread();
        OpenThread.join();

      } else {
        stop();
      }
    }


    void FileOut::registerPort(PortRegistry &portRegistry){
      portRegistry.registerPort(this);
    }

    void FileOut::writeToStream(unsigned char *data, size_t len){
// std::cout << "writeToStream (" << len << ")" << std::endl;

      if (getDeviceState() != DeviceStateConnected){
        return;
      }

      write( FD, data, len );
    }

  }
}
