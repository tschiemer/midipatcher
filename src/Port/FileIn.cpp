#include <Port/FileIn.hpp>

#include <AbstractOutputPort.hpp>
#include <PortRegistry.hpp>

#include <midimessage.h>

#include <cassert>
#include <cstdio>

#include <unistd.h>
#include <fcntl.h>

#include <iostream>

namespace MidiPatcher {
  namespace Port {

      FileIn::FileIn(std::string portName) : AbstractInputPort(portName){


        parser_init(&Parser, RunningStatusEnabled, ParserBuffer, sizeof(ParserBuffer), &MidiMessageMem, midiMessageHandler, midiMessageDiscardHandler, this);


        if (portName == FILE_STDIN){
          // std::cout << "opening stdin" << std::endl;
          // FD = stdin;
          FD = 0;

          setDeviceState(DeviceStateConnected);

        } else {
          // FD = fopen( portName.c_str(), "r");
//
          // std::cout << "FD = " << (int)*FD << std::endl;

          // assert( FD != nullptr );

          OpenThread = std::thread([this,portName](){
            this->FD = open(portName.c_str(), O_RDONLY);
            // std::cout << "FD = " << this->FD << std::endl;

            this->setDeviceState(DeviceStateConnected);
          });
          // OpenThread.detach();

        }


      }

      FileIn::~FileIn(){

        // OpenThread.join();

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
              // std::cout << "read (" << count << ") ";
              // for(int i = 0; i < count; i++){
              //   std::cout << std::hex << (int)buffer[i];
              // }
              // std::cout << std::endl;


              // this->send(buffer,count);
              parser_receivedData(&Parser, buffer, count);
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

      void FileIn::midiMessageHandler(MidiMessage::Message_t * message, void * context){
        assert(context != NULL);

        FileIn * self = (FileIn*)context;

        unsigned char bytes[128];
        size_t len = pack( (uint8_t*)bytes, message );

        self->received(bytes, len);
      }

      void FileIn::midiMessageDiscardHandler(uint8_t *bytes, uint8_t length, void *context){
        // std::cout << "Discarding ";
        // for(int i = 0; i < length; i++){
        //   std::cout << std::hex << (int)bytes[i] << " ";
        // }
        // std::cout << std::endl;
      }

  }
}
