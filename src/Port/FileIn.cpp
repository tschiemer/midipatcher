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

          // OpenThread = std::thread([this,portName](){
          //   OpenThreadRunning = true;

            Log::info(getKey(), "opening " + portName);

            this->FD = open(portName.c_str(), O_RDONLY | O_NONBLOCK);
            if (this->FD == -1){
              throw Error(getKey(), "failed to open: " + std::to_string(errno));
            }
            Log::info(getKey(), "opened " + portName + " (FD = " + std::to_string(FD) + ")");

            this->setDeviceState(DeviceStateConnected);

          //   OpenThreadRunning = false;
          // });
          // OpenThread.detach();

        }


      }

      FileIn::~FileIn(){

        // if (OpenThreadRunning){
        //   Log::debug("FileIn[" + Name + "] OpenThread still running..");
        //
        // }
        //
        // OpenThread.join();

        stop();

        if (FD > 0){
          close(FD);
        }
      }


      void FileIn::registerPort(PortRegistry &portRegistry){
        portRegistry.registerPort(this);
      }

      void FileIn::setNonBlocking(){

          // set non-blocking
          int flags = fcntl(FD, F_GETFL, 0);
          fcntl(FD, F_SETFL, flags | O_NONBLOCK);
      }

      void FileIn::start(){
        if (State != StateStopped){
          return;
        }
        State = StateWillStart;

        Log::debug(getKey(), "starting");

        this->setNonBlocking();


        ReaderThread = std::thread([this](){
          State = StateStarted;

          Log::debug(getKey(), "started");

          while(State == StateStarted && this->getDeviceState() == DeviceStateConnected){
            unsigned char buffer[128];
            size_t count = 0;

            // std::cout << Name << ".fread" << std::endl;
            // count = fread( buffer, 1, sizeof(buffer), this->FD);
            count = read(this->FD, buffer, sizeof(buffer));
            if (count == -1){
              // std::cout << "ERROR" << std::endl;
            }
            else if (count == 0){
              // std::cout << "nothing to read" << std::endl;
            }
            else if (count > 0){
              Log::debug(getKey(), "read " + std::to_string(count) + " bytes");
              // std::cout << "FileIn[" << Name << "] read (" << count << ") ";
              // for(int i = 0; i < count; i++){
              //   std::cout << std::hex << (int)buffer[i] << " ";
              // }
              // std::cout << std::endl;


              // this->send(buffer,count);
              readFromStream(buffer, count);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(50));

          }

          // this->Running = false;
          // close(this->FD);
          State = StateStopped;
        });
        // ReaderThread.detach();

      }

      void FileIn::stop(){
        if (State != StateStarted){
          return;
        }
        State = StateWillStop;

        setDeviceState(DeviceStateNotConnected);

        Log::debug(getKey(), "stopping");

        ReaderThread.join();

        Log::debug(getKey(), "stopped");

        State = StateStopped;
      }

  }
}
