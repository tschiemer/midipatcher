#include <log.hpp>

#include <iostream>

namespace MidiPatcher {
  namespace Log {

    static int Level = 0;
    static std::ostream &Out = std::cerr;

    int getLevel(){
      return Level;
    }

    void setLevel( int level ){
      Level = level;
    }

    void setOutstream( std::ostream &out ){
      // Out = out;
    }

    void print( int level, std::string str ){
      if (level >= Level){
        return;
      }

      Out << str << std::endl;
    }

  }
}
