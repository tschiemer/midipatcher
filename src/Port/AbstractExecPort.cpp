#include <Port/AbstractExecPort.hpp>

#include <Log.hpp>
#include <Error.hpp>

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>

namespace MidiPatcher {
  namespace Port {

    AbstractExecPort::AbstractExecPort(std::string execpath, std::string argvStr){

      ExecPath = execpath;

      while (argvStr.size() > 0) {
          size_t pos = argvStr.find(" ");

          if (pos == 0){
            argvStr.erase(0,1);
            continue;
          }

          std::string arg;
          if (pos == std::string::npos){
            arg = argvStr;
          } else {
            arg = argvStr.substr(0,pos);
          }
          ExecArgv.push_back(arg);

          argvStr.erase(0,arg.size()+1);

      }


      // validate exec
      struct stat st;

      // std::string file = execpath();
      // Log::debug(getKey(), execpath());
      // std::cout << "ep = " << execpath() << std::endl;

      if (stat(ExecPath.c_str(), &st)){
        throw Error(getKey(), "failed stat: " + std::to_string(errno));
      }
      if ((st.st_mode & S_IEXEC) == 0){
        throw Error(getKey(), "not executable");
      }
    }

    AbstractExecPort::~AbstractExecPort(){
      AbstractExecPort::stop();
    }

    void AbstractExecPort::start(){
      if (ExecState != ExecStateStopped){
        return;
      }
      ExecState = ExecStateWillStart;

      Log::info(getKey(), "(exec) starting");

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


      // child
      if (pid == 0){

        close(ToExecFDs[1]);
        close(FromExecFDs[0]);

        // adjust file descriptors
        dup2(ToExecFDs[0], STDIN_FILENO);
        dup2(FromExecFDs[1], STDOUT_FILENO);

        //TODO
        char * argv[12];

        assert( ExecArgv.size() < 11 );

        int a = 0;

        argv[a++] = (char*)ExecPath.c_str();

        for( int b = 0; b < ExecArgv.size(); b++, a++){
          argv[a] = (char*)ExecArgv[b].c_str();
        }
        argv[a] = NULL;

        int exec_result = execvp( ExecPath.c_str(), argv );

        throw Error(getKey(), "failed to exec");
      }

      ExecPID = pid;

      close(ToExecFDs[0]);
      close(FromExecFDs[1]);

      startFileReader(FromExecFDs[0]);

      ExecState = ExecStateStarted;

      Log::info(getKey(), "(exec) started");
    }

    void AbstractExecPort::stop(){

        if (ExecState != ExecStateStarted){
          return;
        }

        assert(ExecPID > 0);

        ExecState = ExecStateWillStop;

        Log::info(getKey(), "(exec) stopping");

        // stop file reader before closing FDs
        stopFileReader();

        // close the program's stdin/out hoping it will quit by itself
        close(ToExecFDs[1]);
        close(FromExecFDs[0]);

        // to make (most likely) sure, send it the kill signal
        kill(ExecPID, SIGINT);

        int status = 0;
        int r = waitpid(ExecPID, &status, 0);

        ExecState = ExecStateStopped;

        Log::info(getKey(), "(exec) stopped");
    }

    void AbstractExecPort::readFromFile(unsigned char * buffer, size_t len ){
      readFromExec(buffer, len);
    }

    void AbstractExecPort::writeToExec(unsigned char * bytes, size_t len){

      Log::debug(getKey(), "writing to exec", bytes, len);

      write(ToExecFDs[1], bytes, len);
    }

  }
}
