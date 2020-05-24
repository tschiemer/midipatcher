#include <Port/RawExec.hpp>

#include <PortRegistry.hpp>

#include <Log.hpp>
#include <Error.hpp>

#include <cassert>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>

namespace MidiPatcher {
  namespace Port {

    AbstractPort* RawExec::factory(PortDescriptor * portDescriptor){

      std::string baseDir = "";
      std::vector<std::string> argv;

      std::for_each(portDescriptor->Options.begin(), portDescriptor->Options.end(), [&argv, &baseDir](std::pair<std::string, std::string> pair){

        std::string arg = pair.first;

        if (arg == "basedir" && pair.second.size() > 0){
            baseDir = pair.second;
        } else {
            if (pair.second.size() > 0){
              arg += "=" + pair.second;
            }
            argv.push_back(arg);
        }

      });

      return new RawExec( portDescriptor->Name, argv, baseDir );
    }

    RawExec::RawExec(std::string name, std::vector<std::string> argv, std::string baseDir) : AbstractInputOutputPort(name) {

      BaseDir = baseDir;

      // validate exec
      struct stat st;

      // std::string file = execpath();
      Log::debug(getKey(), execpath());
      // std::cout << "ep = " << execpath() << std::endl;

      if (stat(execpath().c_str(), &st)){
        throw Error(getKey(), "failed stat: " + std::to_string(errno));
      }
      if ((st.st_mode & S_IEXEC) == 0){
        throw Error(getKey(), "not executable");
      }

      Argv = argv;

      setDeviceState( DeviceStateConnected );
    }

    RawExec::~RawExec(){
      stop();
    }

    void RawExec::registerPort(PortRegistry &portRegistry){
      portRegistry.registerPort(this);
    }

    void RawExec::start(){
      if (State != StateStopped){
        return;
      }
      State = StateWillStart;

      Log::info(getKey(), "starting");

      int r;

      r = pipe(ToExecFDs);
      if( r < 0 ){
        throw Error(getKey(), "failed to create to pipe");
      }

      r = pipe(FromExecFDs);
      if( r < 0 ){
        throw Error(getKey(), "failed to create from pipe");
      }


      int pid = fork();

      if( pid < 0 ){
        throw Error(getKey(), "failed to fork");
      }

      // char ** argv = nullptr;

      // child
      if (pid == 0){

        close(ToExecFDs[1]);
        close(FromExecFDs[0]);

        // adjust file descriptors
        dup2(ToExecFDs[0], STDIN_FILENO);
        dup2(FromExecFDs[1], STDOUT_FILENO);

        char * argv[1] = {NULL};

        int exec_result = execvp( execpath().c_str(), argv );

        throw Error(getKey(), "failed to exec");
      }

      PID = pid;

      close(ToExecFDs[0]);
      close(FromExecFDs[1]);

      startFileReader(FromExecFDs[0]);

      Log::info(getKey(), "started");
    }

    void RawExec::stop(){
      if (State != StateStarted){
        return;
      }

      assert(PID > 0);

      State = StateWillStop;

      Log::info(getKey(), "stopping");

      // stop file reader
      stopFileReader();

      // close the program's stdin/out hoping it will quit by itself
      close(ToExecFDs[1]);
      close(FromExecFDs[0]);

      // to make (most likely) sure, send it the kill signal
      kill(PID, SIGINT);

      int status = 0;
      int r = waitpid(PID, &status, 0);

      State = StateStopped;

      Log::info(getKey(), "stopped");

    }

    void RawExec::sendMessageImpl(unsigned char * message, size_t len){
      write(ToExecFDs[1], message, len);
    }

    void RawExec::readFromFile(unsigned char * buffer, size_t len ){
      readFromStream(buffer, len);
    }
  }
}
