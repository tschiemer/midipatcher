#ifndef MIDIPATCHER_PORT_ABSTRACT_EXEC_PORT_H
#define MIDIPATCHER_PORT_ABSTRACT_EXEC_PORT_H

#include "AbstractInputOutputPort.hpp"
#include "AbstractFileReader.hpp"

#include <vector>
#include <map>

namespace MidiPatcher {
  namespace Port {

    class AbstractExecPort : public virtual AbstractInputOutputPort, public virtual AbstractFileReader {

      protected:

        // static void extractFactoryOptions(std::string &baseDir, std::vector<std::string> &argv, std::map<std::string,std::string> &options );

        AbstractExecPort(std::string execpath, std::string argvStr = "");
        ~AbstractExecPort();

        std::string ExecPath;
        std::vector<std::string> ExecArgv;

        enum ExecState_t {
          ExecStateStopped, ExecStateWillStart, ExecStateStarted, ExecStateWillStop
        };

        ExecState_t ExecState = ExecStateStopped;

        int ExecPID;
        int ToExecFDs[2];
        int FromExecFDs[2];

        virtual void start();
        virtual void stop();

        virtual void writeToExec(unsigned char * bytes, size_t len);
        virtual void readFromExec(unsigned char * bytes, size_t len) = 0;

        void readFromFile(unsigned char * buffer, size_t len );
    };

  }
}

#endif /* MIDIPATCHER_PORT_ABSTRACT_EXEC_PORT_H */
