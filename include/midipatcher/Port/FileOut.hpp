#ifndef MIDIPATCHER_PORT_FILE_OUT_H
#define MIDIPATCHER_PORT_FILE_OUT_H

#include "AbstractStreamOutputPort.hpp"


#include <thread>

namespace MidiPatcher {
  namespace Port {

    class FileOut : AbstractStreamOutputPort {

      public:

        static const constexpr char * FILE_STDOUT = "STDOUT";
        static const constexpr char * FILE_STDERR = "STDERR";

      public:

        static const constexpr char * PortClass = "FileOut";

        std::string getPortClass(){
          return PortClass;
        }

        PortDescriptor * getPortDescriptor(){
          return new PortDescriptor(PortClass, Name);
        }

        static PortClassRegistryInfo * getPortClassRegistryInfo() {
          return new PortClassRegistryInfo(PortClass, factory, nullptr);
        }

        static AbstractPort* factory(PortDescriptor * portDescriptor){
          assert( portDescriptor->PortClass == PortClass );
          return new FileOut(portDescriptor->Name);
        }

        FileOut(std::string portName);
        ~FileOut();


      protected:

        volatile bool OpenThreadRunning = false;
        std::thread OpenThread;

        int FD = -1;

        void writeToStream(unsigned char * data, size_t len);

    };

  }
}

#endif /* MIDIPATCHER_PORT_FILE_OUT_H */
