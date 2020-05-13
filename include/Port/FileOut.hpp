#ifndef PORT_FILE_OUT_H
#define PORT_FILE_OUT_H

#include "../AbstractOutputPort.hpp"


#include <thread>

namespace MidiPatcher {
  namespace Port {

    class FileOut : AbstractOutputPort {

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
          return new PortClassRegistryInfo(PortClass, factory, nullptr, nullptr, nullptr);
        }

        FileOut(std::string portName);
        ~FileOut();

        void registerPort(PortRegistry &portRegistry);

        void send(unsigned char *message, size_t len);

      protected:

        std::thread OpenThread;

        int FD = -1;

      public:

          static const constexpr char * FILE_STDOUT = "STDOUT";
          static const constexpr char * FILE_STDERR = "STDERR";
    };

  }
}

#endif /* PORT_FILE_OUT_H */
