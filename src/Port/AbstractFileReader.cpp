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

      Log::debug("AbstractFileReader", "starting");

      FileReaderFD = fd;

      // set non-blocking
      int flags = fcntl(FileReaderFD, F_GETFL, 0);
      fcntl(FileReaderFD, F_SETFL, flags | O_NONBLOCK);

      FileReaderThread = std::thread([this](){

        FileReaderState = FileReaderStateStarted;

        Log::debug("AbstractFileReader", "started");

        while(FileReaderState == FileReaderStateStarted){
          unsigned char buffer[128];
          size_t count = 0;

          count = read(FileReaderFD, buffer, sizeof(buffer));
          if (count == -1){
            // std::cout << "ERROR" << std::endl;
          }
          else if (count == 0){
            // std::cout << "nothing to read" << std::endl;
          }
          else if (count > 0){
            Log::debug("AbstractFileReader", "read " + std::to_string(count) );
            // std::cout << "RawExec[" << Name << "] read (" << count << ") ";
            // for(int i = 0; i < count; i++){
            //   std::cerr << buffer[i];
            // }
            // std::cerr << std::endl;


            readFromFile(buffer,count);
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

      Log::debug("AbstractFileReader", "stopping");

      FileReaderThread.join();

      FileReaderState = FileReaderStateStopped;

      Log::debug("AbstractFileReader", "stopped");
    }

  }
}
