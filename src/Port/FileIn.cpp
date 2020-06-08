#include <Port/FileIn.hpp>

#include <PortRegistry.hpp>

#include <midimessage.h>

#include <Log.hpp>
#include <Error.hpp>

#include <cassert>
#include <cstdio>

#include <unistd.h>
#include <fcntl.h>

#include <iostream>

namespace MidiPatcher {
  namespace Port {

      FileIn::FileIn(std::string portName, bool runningStatusEnabled) : AbstractInputPort(portName), AbstractStreamInputPort(runningStatusEnabled) {


        if (portName == FILE_STDIN){

          Log::info(getKey(), "using STDIN");

          FD = 0;

          // setNonBlocking();

          setDeviceState(DeviceStateConnected);

        } else {

          // Log::info(getKey(), "opening " + portName);

          this->FD = open(portName.c_str(), O_RDONLY | O_NONBLOCK);
          if (this->FD == -1){
            throw Error(getKey(), "failed to open: " + std::to_string(errno));
          }
          Log::info(getKey(), "opened " + portName + " (FD = " + std::to_string(FD) + ")");

          this->setDeviceState(DeviceStateConnected);
        }


      }

      FileIn::~FileIn(){

        // stopFileReader();
        stop();

        if (FD > 0){
          close(FD);
        }
      }

      // void FileIn::setNonBlocking(){
      //
      //     // set non-blocking
      //     int flags = fcntl(FD, F_GETFL, 0);
      //     fcntl(FD, F_SETFL, flags | O_NONBLOCK);
      // }

      void FileIn::start(){
        if (State != StateStopped){
          return;
        }
        State = StateWillStart;

        Log::debug(getKey(), "starting");

        startFileReader(FD);

        Log::debug(getKey(), "started");
      }

      void FileIn::stop(){
        if (State != StateStarted){
          return;
        }
        State = StateWillStop;

        Log::debug(getKey(), "stopping");

        setDeviceState(DeviceStateNotConnected);

        stopFileReader();

        Log::debug(getKey(), "stopped");

        State = StateStopped;
      }

      void FileIn::readFromFile(unsigned char * buffer, size_t len ){
        readFromStream(buffer, len);
      }

  }
}
