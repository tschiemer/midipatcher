#ifndef PORT_PIPE_IN
#define PORT_PIPE_IN


#include <AbstractStreamInputPort.hpp>

#include <midimessage/parser.h>

#include <thread>

#include <stdio.h>


namespace MidiPatcher {
  namespace Port {

    class FileIn : AbstractStreamInputPort {

      public:

        static const constexpr char * PortClass = "FileIn";

        std::string getPortClass(){
          return PortClass;
        }

        static AbstractPort* factory(PortDescriptor * portDescriptor){
          assert( portDescriptor->PortClass == PortClass );
          return new FileIn(portDescriptor->Name);
        }

        static PortClassRegistryInfo * getPortClassRegistryInfo() {
          return new PortClassRegistryInfo(PortClass, factory, nullptr, nullptr, nullptr);
        }

        FileIn(std::string portName, bool runningStatusEnabled = true);
        ~FileIn();

        void registerPort(PortRegistry &portRegistry);

      public:

        static const constexpr char * FILE_STDIN = "STDIN";

      protected:

        std::thread OpenThread;

        int FD = -1;

        void setNonBlocking();

        volatile bool Running = false;
        std::thread ReaderThread;

        void start();
        void stop();

    };

  }
}

#endif