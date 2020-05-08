#include <Port/MidiIn.hpp>

#include <PortRegistry.hpp>
#include <AbstractOutputPort.hpp>

#include <iostream>

namespace MidiPatcher {

  namespace Port {

    std::map<std::string, MidiIn*> * MidiIn::KnownPorts = NULL;

    AbstractPort* MidiIn::factory(PortRegistry * portRegistry, int argc, char * argv[]){
      return NULL;
    }

    std::vector<AbstractPort*> * MidiIn::scan(PortRegistry * portRegistry){
      RtMidiIn *midiin = 0;

      std::vector< AbstractPort * > * result = new std::vector< AbstractPort* >();

      try {

        midiin = new RtMidiIn();

        // Check inputs.
        unsigned int nPorts = midiin->getPortCount();

        for ( unsigned i=0; i<nPorts; i++ ) {

          std::string name = midiin->getPortName(i);
          if (KnownPorts->count(name)){
            KnownPorts->at(name)->PortNumber = i;
          } else {
            new MidiIn(portRegistry, i, name);
          }
          result->push_back(KnownPorts->at(name));

        }

      } catch ( RtMidiError &error ) {
        error.printMessage();
      }

      delete midiin;

      return result;
    }


    void MidiIn::rtMidiCallback( double timeStamp, std::vector<unsigned char> *message, void * midiInRef ){
      assert(midiInRef != NULL);

      MidiIn * midiIn = (MidiIn*)midiInRef;

      std::for_each(midiIn->Connections.begin(), midiIn->Connections.end(), [message](AbstractPort* port){
        dynamic_cast<AbstractOutputPort*>(port)->send(message);
      });
    }
  }

}
