#ifndef MIDIPATCHER_PORT_FILE_OUT_H
#define MIDIPATCHER_PORT_FILE_OUT_H

#include "AbstractStreamOutputPort.hpp"


#include <thread>

namespace MidiPatcher {
  namespace Port {

    class FileOut : AbstractStreamOutputPort {

      public:
        static const constexpr char * PortClass = "FileOut";

        std::string getPortClass(){
          return PortClass;
        }

        static AbstractPort* factory(PortDescriptor * portDescriptor){
          assert( portDescriptor->PortClass == PortClass );
          return new FileOut(portDescriptor->Name);
        }

        static PortClassRegistryInfo * getPortClassRegistryInfo() {
          return new PortClassRegistryInfo(PortClass, factory, nullptr);
        }

        PortDescriptor * getPortDescriptor(){
          return new PortDescriptor(PortClass, Name);
        }

        FileOut(std::string portName);
        ~FileOut();

        void registerPort(PortRegistry &portRegistry);

      public:

          static const constexpr char * FILE_STDOUT = "STDOUT";
          static const constexpr char * FILE_STDERR = "STDERR";

      protected:

        volatile bool OpenThreadRunning = false;
        std::thread OpenThread;

        int FD = -1;

        void writeToStream(unsigned char * data, size_t len);

    };

  }
}

#endif /* MIDIPATCHER_PORT_FILE_OUT_H */
