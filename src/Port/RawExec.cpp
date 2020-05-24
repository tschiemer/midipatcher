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

      std::vector<std::string> argv;

      std::for_each(portDescriptor->Options.begin(), portDescriptor->Options.end(), [&argv](std::pair<std::string, std::string> pair){

        std::string arg = pair.first;

        if (pair.second.size() > 0){
          arg += "=" + pair.second;
        }

        argv.push_back(arg);
      });

      return new RawExec( portDescriptor->Name, argv );
    }

    RawExec::RawExec(std::string name, std::vector<std::string> argv) : AbstractInputOutputPort(name) {


      // validate exec
      struct stat st;

      if (stat(name.c_str(), &st)){
        throw Error(getKey(), "failed stat: " + std::to_string(errno));
      }
      if ((st.st_mode & S_IEXEC) == 0){
        throw Error(getKey(), "not executable");
      }

      Argv = argv;

      setDeviceState( DeviceStateConnected );
    }

    RawExec::~RawExec(){

    }

    void RawExec::registerPort(PortRegistry &portRegistry){
      portRegistry.registerPort(this);
    }

    void RawExec::start(){
      if (State != StateStopped){
        return;
      }
      State = StateWillStart;

      int r;

      r = pipe(ToExecFDs);
      assert( r >= 0 );

      r = pipe(FromExecFDs);
      assert( r >= 0 );



      int pid = fork();

      assert( pid >= 0 );

      // char ** argv = nullptr;

      // parent
      if (pid > 0){
        PID = pid;

        close(ToExecFDs[0]);
        close(FromExecFDs[1]);

        // wait for child to finish starting (or failing for that matter..)
        // while (State == StateWillStart){}

        // std::cout << "foobar" << std::endl;
        // while(1);

        // if (argv != nullptr){
        //   std::free(argv);
        // }

        startReader();

        return;
      }


      close(ToExecFDs[1]);
      close(FromExecFDs[0]);

      // adjust file descriptors
      dup2(ToExecFDs[0], STDIN_FILENO);
      dup2(FromExecFDs[1], STDOUT_FILENO);

      char * argv[1] = {NULL};

      int exec_result = execvp( Name.c_str(), argv );

      assert( exec_result != -1 );
    }

    void RawExec::startReader(){

      setNonBlocking(FromExecFDs[0]);

      Log::info(getKey(), "start");

      ReaderThread = std::thread([this](){
        State = StateStarted;

        Log::info(getKey(), "started");

        while(State == StateStarted){
          unsigned char buffer[128];
          size_t count = 0;

          // std::cout << Name << ".fread" << std::endl;
          // count = fread( buffer, 1, sizeof(buffer), this->FD);
          count = read(FromExecFDs[0], buffer, sizeof(buffer));
          if (count == -1){
            // std::cout << "ERROR" << std::endl;
          }
          else if (count == 0){
            // std::cout << "nothing to read" << std::endl;
          }
          else if (count > 0){
            // std::cout << "RawExec[" << Name << "] read (" << count << ") ";
            // for(int i = 0; i < count; i++){
            //   std::cout << std::hex << (int)buffer[i];
            // }
            // std::cout << std::endl;


            receivedMessage(buffer,count);
            // readFromStream(buffer, count);
          }

          std::this_thread::sleep_for(std::chrono::milliseconds(50));

        }

        this->State = StateStopped;
      });
    }

    void RawExec::stop(){
      if (State != StateStarted){
        return;
      }

      assert(PID > 0);

      State = StateWillStop;

      Log::info(getKey(), "stopping");

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

    void RawExec::stopReader(){

    }

    void RawExec::setNonBlocking(int fd){

        // set non-blocking
        int flags;
        // flags = fcntl(FDs[0], F_GETFL, 0);
        // fcntl(FDs[0], F_SETFL, flags | O_NONBLOCK);
        flags = fcntl(fd, F_GETFL, 0);
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    }

    void RawExec::sendMessage(unsigned char * message, size_t len){
      // std::cout << "sendMessage 1 " << len << std::endl;
      if (getDeviceState() != DeviceStateConnected){
        return;
      }
      // std::cout << "sendMessage 2" << std::endl;

      write(ToExecFDs[1], message, len);
    }
  }
}
