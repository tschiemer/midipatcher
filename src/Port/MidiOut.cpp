#include <Port/MidiOut.hpp>

namespace MidiPatcher {

  namespace Port {

    std::map<std::string, MidiOut*> * MidiOut::KnownPorts = new std::map<std::string, MidiOut*>();

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
            new MidiOut(portRegistry, name, i);
          }
          result->push_back(KnownPorts->at(name));

        }

      } catch ( RtMidiError &error ) {
        error.printMessage();
      }

      delete midiout;

      return result;

    }


    void MidiOut::addConnectionImpl(AbstractPort * port){
      //
      if (MidiPort != NULL){
        return;
      }

      MidiPort = new RtMidiOut();

      MidiPort->openPort(PortNumber);
    }

    void MidiOut::removeConnectionImpl(AbstractPort * port){

      if (MidiPort == NULL){
        return;
      }

      // if there will not be any connections after this, just close and deallocate the port
      if (Connections.size() <= 1){
        MidiPort->closePort();

        delete MidiPort;

        MidiPort = NULL;
      }
    }

    void MidiOut::send(unsigned char *message, size_t len){
      if (MidiPort == NULL){
        return;
      }
      if (MidiPort->isPortOpen() == false){
        return;
      }

      std::cout << "sending (" << Name << ") ";
      for(int i = 0; i < len; i++){
        std::cout << std::hex << (int)message[i] << " ";
      };
      std::cout << std::endl;

      MidiPort->sendMessage(message, len);
    }
  }

}
