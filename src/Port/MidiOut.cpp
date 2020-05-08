#include <Port/MidiOut.hpp>

namespace MidiPatcher {

  namespace Port {

    std::map<std::string, MidiOut*> * MidiOut::KnownPorts = new std::map<std::string, MidiOut*>();

        AbstractPort* MidiOut::factory(PortRegistry * portRegistry, int argc, char * argv[]){
          return NULL;
        }

    std::vector<AbstractPort*>  * MidiOut::scan(PortRegistry * portRegistry){
      RtMidiOut *midiout = 0;

      std::vector< AbstractPort * > * result = new std::vector< AbstractPort* >();

      try {

        midiout = new RtMidiOut();

        // Check inputs.
        unsigned int nPorts = midiout->getPortCount();

        for ( unsigned i=0; i<nPorts; i++ ) {

          std::string name = midiout->getPortName(i);
          if (KnownPorts->count(name)){
            KnownPorts->at(name)->PortNumber = i;
          } else {
            new MidiOut(portRegistry, i, name);
          }
          result->push_back(KnownPorts->at(name));

        }

      } catch ( RtMidiError &error ) {
        error.printMessage();
      }

      delete midiout;

      return result;

    }

  }

}
