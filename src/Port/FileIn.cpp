#include <Port/FileIn.hpp>

// #include <Port/AbstractOutputPort.hpp>

#include <PortRegistry.hpp>

#include <midimessage.h>

#include <cassert>
#include <cstdio>

#include <unistd.h>
#include <fcntl.h>

#include <iostream>

namespace MidiPatcher {
  namespace Port {

      FileIn::FileIn(std::string portName, bool runningStatusEnabled) : AbstractInputPort(portName), AbstractStreamInputPort(runningStatusEnabled) {


        if (portName == FILE_STDIN){

          FD = 0;

          setDeviceState(DeviceStateConnected);

        } else {

          OpenThread = std::thread([this,portName](){
            OpenThreadRunning = true;

            this->FD = open(portName.c_str(), O_RDONLY);

            this->setDeviceState(DeviceStateConnected);

            OpenThreadRunning = false;
          });
          // OpenThread.detach();

        }


      }

      FileIn::~FileIn(){

        // if (OpenThreadRunning){
        //   OpenThread.join();
        // }

        stop();

        // setDeviceState(DeviceStateNotConnected);


        if (Name == FILE_STDIN){
          // do nothing
        } else {
            // fclose(FD);
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
        if (Running){
          return;
        }

        this->setNonBlocking();


        ReaderThread = std::thread([this](){
          this->Running = true;

          while(this->getDeviceState() == DeviceStateConnected){
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
              std::cout << "FileIn[" << Name << "] read (" << count << ") ";
              for(int i = 0; i < count; i++){
                std::cout << std::hex << (int)buffer[i] << " ";
              }
              std::cout << std::endl;


              // this->send(buffer,count);
              readFromStream(buffer, count);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(50));

          }

          this->Running = false;
          // close(this->FD);
        });
        // ReaderThread.detach();

      }

      void FileIn::stop(){
        if (Running == false){
          return;
        }
        // Running = false;
        // ReaderThread.join();
      }

  }
}
