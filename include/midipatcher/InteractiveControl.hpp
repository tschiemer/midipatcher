#ifndef MIDIPATCHER_INTERACTIVE_CONTROL_H
#define MIDIPATCHER_INTERACTIVE_CONTROL_H

#include "AbstractControl.hpp"

#include <iostream>

namespace MidiPatcher {

  class InteractiveControl : public virtual AbstractControl {

    protected:

      std::istream &In;
      std::ostream &Out;

      void printHelp();

    public:

     InteractiveControl(PortRegistry * portRegistry);
     InteractiveControl(PortRegistry * portRegistry, std::istream &in, std::ostream &out);

     bool runloop();

    protected:

      void respond(std::vector<std::string> &argv);

  };

}

#endif /* MIDIPATCHER_INTERACTIVE_CONTROL_H */
