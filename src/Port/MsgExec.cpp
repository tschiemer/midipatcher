#include <Port/MsgExec.hpp>

#include <unistd.h>

namespace MidiPatcher {
  namespace Port {


    AbstractPort* MsgExec::factory(PortDescriptor * portDescriptor){


      std::string baseDir = "";
      std::vector<std::string> argv;

      if (portDescriptor->Options.count("basedir") > 0){
        baseDir = portDescriptor->Options["basedir"];
      }
      if (portDescriptor->Options.count("argv") > 0){
        std::string argvStr = portDescriptor->Options["argv"];
        std::cerr << argvStr << std::endl;

        // std::exit(0);
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
            argv.push_back(arg);

            // std::cerr << "#" << argv.size() << arg << std::endl;

            argvStr.erase(0,arg.size()+1);

        }
      }

// std::exit(0);

      // std::string baseDir = "";
      // std::vector<std::string> argv;
      //
      // std::for_each(portDescriptor->Options.begin(), portDescriptor->Options.end(), [&argv, &baseDir](std::pair<std::string, std::string> pair){
      //
      //   std::string arg = pair.first;
      //
      //   if (arg == "basedir" && pair.second.size() > 0){
      //       baseDir = pair.second;
      //   } else {
      //       if (pair.second.size() > 0){
      //         arg += "=" + pair.second;
      //       }
      //       argv.push_back(arg);
      //   }
      //
      // });

      return new MsgExec( portDescriptor->Name, argv, baseDir );
    }


    MsgExec::MsgExec(std::string name, std::vector<std::string> argv, std::string baseDir) : AbstractInputOutputPort(name), RawExec(name, argv, baseDir) {

      // do nothing special
    }

    MsgExec::~MsgExec(){
      // stop();
    }

    void MsgExec::sendMessageImpl(unsigned char * message, size_t len){
      AbstractMessageOutputPort::sendMessageImpl(message, len);
    }

    void MsgExec::writeStringMessage(unsigned char * stringMessage, size_t len){
      stringMessage[len++] = '\n';
      // stringMessage[len] = '\0';

      // std::cerr << "writing: " << stringMessage << std::endl;

      writeToStream(stringMessage, len);
      // flush(ToExecFDs[1]);
    }

    // void MsgExec::readFromFile(unsigned char * buffer, size_t len ){
    //   readFromStream(buffer, len);
    // }

  }
}
