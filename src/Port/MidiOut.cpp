#include <Port/MidiOut.hpp>

namespace MidiPatcher {

  namespace Port {

    std::map<std::string, MidiOut*> * MidiOut::KnownPorts = new std::map<std::string, MidiOut*>();

    std::vector<AbstractPort*> * MidiOut::scan(PortRegistry * portRegistry){

      // set dirty (seen)
      std::map<std::string,bool> seen;
      std::map<std::string,DeviceState_t> previousState;

      std::for_each(KnownPorts->begin(), KnownPorts->end(), [&seen, &previousState](std::pair<std::string,MidiOut*> pair){
        seen[pair.first] = false;
        previousState[pair.first] = pair.second->getDeviceState();
      });

      RtMidiOut *midiout = 0;

      std::vector< AbstractPort * > * result = new std::vector< AbstractPort* >();

      try {

        midiout = new RtMidiOut();

        // Check inputs.
        unsigned int nPorts = midiout->getPortCount();

        for ( unsigned i=0; i<nPorts; i++ ) {

          MidiOut * mi;
          std::string name = midiout->getPortName(i);

          if (KnownPorts->count(name)){
            mi = KnownPorts->at(name);
            mi->PortNumber = i;

            seen[name] = true;
          } else {
            mi = new MidiOut(portRegistry, name, i);

            mi->publishDeviceDiscovered();
          }

          // this may trigger device connected notifications
          mi->setDeviceState(DeviceStateConnected);

          result->push_back(mi);
        }

      } catch ( RtMidiError &error ) {
        error.printMessage();
      }

      delete midiout;

      //detect devices gone offline
      std::for_each(seen.begin(), seen.end(), [](std::pair<std::string,bool> pair){
        if (pair.second == false){
          KnownPorts->at(pair.first)->setDeviceState(DeviceStateNotConnected);
        }
      });

      return result;
    }

    void MidiOut::start(){

      if (MidiPort != NULL){
        return;
      }

      MidiPort = new RtMidiOut();

      MidiPort->openPort(PortNumber);
    }

    void MidiOut::stop(){

      if (MidiPort == NULL){
        return;
      }

      MidiPort->closePort();

      delete MidiPort;

      MidiPort = NULL;
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
