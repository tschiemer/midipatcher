#ifndef ABSTRACT_FILE_READER_H
#define ABSTRACT_FILE_READER_H

#include "AbstractPort.hpp"

#include <thread>

namespace MidiPatcher {

  namespace Port {

    class AbstractFileReader {

      static unsigned int SleepMillisec;

      protected:

          size_t FileReaderBufferSize;
          unsigned char * FileReaderBuffer;

          AbstractFileReader(size_t bufferSize = 128){
            FileReaderBufferSize = bufferSize;
            FileReaderBuffer = (unsigned char *)malloc(bufferSize);
          }

          ~AbstractFileReader(){
            stopFileReader();
            free(FileReaderBuffer);
          }

          enum FileReaderState_t {
            FileReaderStateStopped, FileReaderStateWillStart, FileReaderStateStarted, FileReaderStateWillStop
          };

          volatile FileReaderState_t FileReaderState = FileReaderStateStopped;

          int FileReaderFD;
          std::thread FileReaderThread;

          void startFileReader(int fd);
          void stopFileReader();

          virtual void readFromFile(unsigned char * buffer, size_t len ) = 0;
    };

  }

}

#endif /* ABSTRACT_FILE_READER_H */
