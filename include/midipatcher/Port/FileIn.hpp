#ifndef MIDIPATCHER_PORT_FILE_IN_H
#define MIDIPATCHER_PORT_FILE_IN_H


#include "AbstractStreamInputPort.hpp"
#include "AbstractFileReader.hpp"

#include <midimessage/parser.h>

#include <thread>

#include <stdio.h>


namespace MidiPatcher {
  namespace Port {

    class FileIn : public virtual AbstractStreamInputPort, public virtual AbstractFileReader {

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
          return new PortClassRegistryInfo(PortClass, factory, nullptr);
        }

        PortDescriptor * getPortDescriptor(){
          return new PortDescriptor(PortClass, Name);
        }

        FileIn(std::string portName, bool runningStatusEnabled = true);
        ~FileIn();

        void registerPort(PortRegistry &portRegistry);

      public:

        static const constexpr char * FILE_STDIN = "STDIN";

      protected:

        int FD = -1;

        enum State_t{
          StateStopped, StateWillStart, StateStarted, StateWillStop
        };

        volatile State_t State = StateStopped;

        void start();
        void stop();

        void readFromFile(unsigned char * buffer, size_t len );

    };

  }
}

#endif /* MIDIPATCHER_PORT_FILE_IN_H */
