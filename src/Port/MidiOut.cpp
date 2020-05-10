#include <Port/MidiOut.hpp>

#include <PortRegistry.hpp>

#include <cstdlib>
#include <ctime>

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

      static RtMidiOut *midiout = NULL;

      std::vector< AbstractPort * > * result = new std::vector< AbstractPort* >();

      try {

        midiout = new RtMidiOut();

        // Check inputs.
        unsigned int nPorts = midiout->getPortCount();

        for ( unsigned i=0; i<nPorts; i++ ) {

          MidiOut * mi;
          std::string name = midiout->getPortName(i);

          // catch RtMidi Error case :/
          if (name == ""){
            continue;
          }

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

    MidiOut::MidiOut(PortRegistry * portRegistry, std::string portName, unsigned int portNumber) : AbstractPort(portRegistry, TypeOutput, portName) {
      PortNumber = portNumber;

      (*KnownPorts)[portName] = this;

      portRegistry->registerPort( this );
    }

    MidiOut::~MidiOut() {
      if (MidiPort != NULL){
        MidiPort->closePort();
        delete MidiPort;
      }
    }

    void MidiOut::start(){

      // if (getDeviceState() == DeviceStateNotConnected){
      //   return;
      // }

      if (MidiPort != NULL){
        return;
      }
      // std::cout << "MidiOut:" << Name << " START (PortNumber = " << PortNumber << ")" << std::endl;

      MidiPort = new RtMidiOut();

      MidiPort->openPort(PortNumber);

      assert(MidiPort->isPortOpen());
    }

    void MidiOut::stop(){

      if (MidiPort == NULL){
        return;
      }
      // std::cout << "MidiOut:" << Name << " STOP (PortNumber = " << PortNumber << ")" << std::endl;

      MidiPort->closePort();

      delete MidiPort;

      MidiPort = NULL;
    }

    void MidiOut::send(unsigned char *message, size_t len){
      // std::cout << "send.." << std::endl;


      if (MidiPort == NULL){
        // std::cout << "port is NULL?!?! " << Name << " # " << PortNumber << std::endl;
        return;
      }
      if (MidiPort->isPortOpen() == false){
        // std::cout << "port not open! " << Name << " # " << PortNumber << std::endl;
        return;
      }

                  // if (len == 3){
                  //   std::cout << "tx [" << Name << "](" << len << ") ";
                  //   for(int i = 0; i < len; i++){
                  //     std::cout << std::hex << (int)message[i] << " ";
                  //   };
                  //   std::cout << std::endl;
                  // }

      MidiPort->sendMessage(message, len);
    }
  }

}
