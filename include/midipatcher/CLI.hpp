#ifndef MIDIPATCHER_CLI_H
#define MIDIPATCHER_CLI_H

#include <iostream>
#include <vector>

namespace MidiPatcher {

  class CLI {

    protected:

      std::istream &In;
      std::ostream &Out;
      std::string Delimiter;

    public:

      CLI(std::string delimiter = " ") : CLI(std::cin, std::cout, delimiter){}
      CLI(std::istream &in, std::ostream &out, std::string delimiter = " ") : In(in), Out(out){
         assert( delimiter.size() > 0 );

         Delimiter = delimiter;
       }

      // std::string getArgvDelimiter();
      // void setArgvDelimiter(std::string delimiter);

     inline std::string getArgvDelimiter(){
       return Delimiter;
     }

     inline void setArgvDelimiter(std::string delimiter){
       assert( delimiter.size() > 0 );
       Delimiter = delimiter;
     }

     virtual bool runloop();

     virtual void sendCommand(std::vector<std::string> &argv) = 0;

   protected:

     virtual void printHelp();

     virtual void receivedResponse(std::vector<std::string> &argv);

  };

}

#endif /* MIDIPATCHER_CLI_H */
