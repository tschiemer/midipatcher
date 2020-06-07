#ifndef MIDIPATCHER_INTERACTIVE_CONTROL_H
#define MIDIPATCHER_INTERACTIVE_CONTROL_H

#include "AbstractControl.hpp"
#include "CLI.hpp"

namespace MidiPatcher {

  class InteractiveControl : public virtual AbstractControl, public virtual CLI {

    public:

     // InteractiveControl(PortRegistry * portRegistry, std::string delimiter = " ");
     // InteractiveControl(PortRegistry * portRegistry, std::istream &in, std::ostream &out, std::string delimiter = " ");

       InteractiveControl(PortRegistry * portRegistry, std::string delimiter = " ") : InteractiveControl(portRegistry, std::cin, std::cout, delimiter){
       }

       InteractiveControl(PortRegistry * portRegistry, std::istream &in, std::ostream &out, std::string delimiter = " ") : AbstractControl(portRegistry), CLI(in,out,delimiter) {
       }

     // void sendCommand(std::vector<std::string> &argv);

     void sendCommand(std::vector<std::string> &argv){
       handleCommand(argv);
     }

    protected:

      // void respond(std::vector<std::string> &argv);

      void respond(std::vector<std::string> &argv){
        receivedResponse(argv);
      }

  };

}

#endif /* MIDIPATCHER_INTERACTIVE_CONTROL_H */
