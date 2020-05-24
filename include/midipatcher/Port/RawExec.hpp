#ifndef PORT_RAW_EXEC_H
#define PORT_RAW_EXEC_H

#include "AbstractInputOutputPort.hpp"
#include "AbstractStreamInputPort.hpp"
#include "AbstractStreamOutputPort.hpp"
#include "AbstractFileReader.hpp"

#include <vector>
#include <thread>

namespace MidiPatcher {

  class PortRegistry;

  namespace Port {

    class RawExec : public virtual AbstractInputOutputPort, public virtual AbstractStreamInputPort, public virtual AbstractStreamOutputPort, public virtual AbstractFileReader {

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

        RawExec(std::string portName, std::vector<std::string> argv, std::string baseDir = "");
        ~RawExec();

        void registerPort(PortRegistry &portRegistry);

      protected:

        std::string BaseDir;
        std::vector<std::string> Argv;

        std::string execpath(){
          if (BaseDir.size() == 0){
            return Name;
          }
          return BaseDir + Name;
        }

        int PID;
        int ToExecFDs[2];
        int FromExecFDs[2];

        enum State_t{
          StateStopped, StateWillStart, StateStarted, StateWillStop
        } ;

        volatile State_t State = StateStopped;

        void start();
        void stop();

        void writeToStream(unsigned char * data, size_t len);

        virtual void readFromFile(unsigned char * buffer, size_t len );
    };

  }
}

#endif /* PORT_RAW_EXEC_H */
