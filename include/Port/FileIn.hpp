#ifndef PORT_PIPE_IN
#define PORT_PIPE_IN


#include <AbstractInputPort.hpp>

#include <thread>

#include <stdio.h>


namespace MidiPatcher {
  namespace Port {

    class FileIn : AbstractInputPort {

      public:

        static const constexpr char * PortClass = "FileIn";

        std::string getPortClass(){
          return PortClass;
        }

        static AbstractPort* factory(PortRegistry * portRegistry, PortDescriptor * portDescriptor){
          assert( portDescriptor->PortClass == PortClass );
          return new FileIn(portRegistry, portDescriptor->Name);
        }

        static PortClassRegistryInfo * getPortClassRegistryInfo() {
          return new PortClassRegistryInfo(PortClass, factory, nullptr, nullptr, nullptr);
        }

        FileIn(PortRegistry * portRegistry, std::string portName);
        ~FileIn();


      protected:

        std::thread OpenThread;

        int FD = -1;

        void setNonBlocking();

        volatile bool Running = false;
        std::thread ReaderThread;

        void start();
        void stop();

      public:

          static const constexpr char * FILE_STDIN = "STDIN";




    };

  }
}

#endif
