#ifndef MIDIPATCHER_PATCHFILE_H
#define MIDIPATCHER_PATCHFILE_H

#include "PortRegistry.hpp"

namespace MidiPatcher {

  namespace Patchfile {

    void loadFromPatchfile(PortRegistry * portRegistry, std::string fname);

    void saveToPatchfile(PortRegistry * portRegistry, std::string fname);

  }

}

#endif /* MIDIPATCHER_PATCHFILE_H */
