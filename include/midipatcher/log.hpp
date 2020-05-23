#ifndef MIDIPATCHER_DEBUG_H
#define MIDIPATCHER_DEBUG_H

#include <ostream>

namespace MidiPatcher {

  namespace Log {

    int getLevel();
    void setLevel( int level );

    void setOutstream( std::ostream &out );

    void print( int level, std::string str );

  }

}

#endif /* MIDIPATCHER_DEBUG_H */
