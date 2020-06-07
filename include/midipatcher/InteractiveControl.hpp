#ifndef MIDIPATCHER_INTERACTIVE_CONTROL_H
#define MIDIPATCHER_INTERACTIVE_CONTROL_H

#include "AbstractControl.hpp"

#include <iostream>

namespace MidiPatcher {

  class InteractiveControl : public virtual AbstractControl {

    protected:

      std::istream &In;
      std::ostream &Out;
      std::string Delimiter = " ";

      void printHelp();

    public:

     InteractiveControl(PortRegistry * portRegistry);
     InteractiveControl(PortRegistry * portRegistry, std::istream &in, std::ostream &out);

     inline std::string getArgvDelimiter(){
       return Delimiter;
     }

     inline void setArgvDelimiter(std::string delimiter){
       assert( delimiter.size() > 0 );
       Delimiter = delimiter;
     }

     bool runloop();

    protected:

      void respond(std::vector<std::string> &argv);

  };

}

#endif /* MIDIPATCHER_INTERACTIVE_CONTROL_H */
