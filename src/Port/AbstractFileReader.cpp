#include <Port/AbstractFileReader.hpp>

#include <Log.hpp>

#include <unistd.h>
#include <fcntl.h>

#include <iostream>

namespace MidiPatcher {
  namespace Port {

    unsigned int AbstractFileReader::SleepMillisec = 50;

    void AbstractFileReader::startFileReader(int fd){

      if (FileReaderState != FileReaderStateStopped){
        return;
      }
      FileReaderState = FileReaderStateWillStart;

      FileReaderFD = fd;

      Log::debug("FileReader (fd = " + std::to_string(FileReaderFD) + ")", "starting");

      // set non-blocking
      int flags = fcntl(FileReaderFD, F_GETFL, 0);
      fcntl(FileReaderFD, F_SETFL, flags | O_NONBLOCK);

      FileReaderThread = std::thread([this](){

        FileReaderState = FileReaderStateStarted;

        Log::debug("FileReader (fd = " + std::to_string(FileReaderFD) + ")", "started");

        while(FileReaderState == FileReaderStateStarted){
          size_t len = 0;

          len = read(FileReaderFD, FileReaderBuffer, FileReaderBufferSize);
          if (len == -1){
            // std::cout << "ERROR" << std::endl;
          }
          else if (len == 0){
            // std::cout << "nothing to read" << std::endl;
          }
          else if (len > 0){
            Log::debug("FileReader (fd = " + std::to_string(FileReaderFD) + ")", "read", FileReaderBuffer, len );

            if (len == FileReaderBufferSize){
              Log::warning("FileReader (fd = " + std::to_string(FileReaderFD) + ")", "filled buffer! " + std::to_string(len) );
            }


            readFromFile(FileReaderBuffer,len);
          }

          std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
      });
    }


    void AbstractFileReader::stopFileReader(){
      if (FileReaderState != FileReaderStateStarted){
        return;
      }

      FileReaderState = FileReaderStateWillStop;

      Log::debug("FileReader (fd = " + std::to_string(FileReaderFD) + ")", "stopping");

      FileReaderThread.join();

      FileReaderState = FileReaderStateStopped;

      Log::debug("FileReader (fd = " + std::to_string(FileReaderFD) + ")", "stopped");
    }

  }
}
