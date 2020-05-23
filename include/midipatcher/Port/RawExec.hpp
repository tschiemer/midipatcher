#ifndef PORT_RAW_EXEC_H
#define PORT_RAW_EXEC_H

#include "AbstractInputOutputPort.hpp"

#include <vector>
#include <thread>

namespace MidiPatcher {

  class PortRegistry;

  namespace Port {

    class RawExec : public virtual AbstractInputOutputPort {

      public:

        static const constexpr char * PortClass = "RawExec";

        std::string getPortClass(){
          return PortClass;
        }

        static AbstractPort* factory(PortDescriptor * portDescriptor);

        static PortClassRegistryInfo * getPortClassRegistryInfo() {
          return new PortClassRegistryInfo(PortClass, factory, nullptr, nullptr, nullptr);
        }

        PortDescriptor * getPortDescriptor(){
          return new PortDescriptor(PortClass, Name);
        }

        RawExec(std::string portName, std::vector<std::string> argv);
        ~RawExec();

        void registerPort(PortRegistry &portRegistry);

        void sendMessage(unsigned char * message, size_t len);

      protected:

        // std::string ExecPath;
        std::vector<std::string> Argv;

        int PID;
        int ToExecFDs[2];
        int FromExecFDs[2];

        enum State_t{
          StateStopped, StateWillStart, StateStarted, StateWillStop
        } ;

        volatile State_t State = StateStopped;

        std::thread ReaderThread;

        void startReader();
        void stopReader();

        void start();
        void stop();

        void setNonBlocking(int fd);
    };

  }
}

#endif /* PORT_RAW_EXEC_H */
