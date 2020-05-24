#ifndef ABSTRACT_FILE_READER_H
#define ABSTRACT_FILE_READER_H

#include <thread>

namespace MidiPatcher {

  namespace Port {

    class AbstractFileReader {

      static unsigned int SleepMillisec;

      protected:

          ~AbstractFileReader(){
            stopFileReader();
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
