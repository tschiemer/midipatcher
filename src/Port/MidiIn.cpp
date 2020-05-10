#include <Port/MidiIn.hpp>

#include <PortRegistry.hpp>
#include <AbstractOutputPort.hpp>

#include <iostream>

#include <cstdlib>
#include <ctime>

namespace MidiPatcher {

  namespace Port {

    // RtMidiIn * MidiIn::RtMidiInRef = NULL;

    std::map<std::string, MidiIn*> * MidiIn::KnownPorts = NULL;

    std::vector<AbstractPort*> * MidiIn::scan(PortRegistry * portRegistry){

      // set dirty (seen)
      std::map<std::string,bool> seen;
      std::map<std::string,DeviceState_t> previousState;

      std::for_each(KnownPorts->begin(), KnownPorts->end(), [&seen, &previousState](std::pair<std::string,MidiIn*> pair){
        seen[pair.first] = false;
        previousState[pair.first] = pair.second->getDeviceState();
      });

      static RtMidiIn *midiin = NULL;

      std::vector< AbstractPort * > * result = new std::vector< AbstractPort* >();

      try {

        midiin = new RtMidiIn();

        // Check inputs.
        unsigned int nPorts = midiin->getPortCount();

        for ( unsigned i=0; i<nPorts; i++ ) {

          MidiIn * mi;
          std::string name = midiin->getPortName(i);

          // catch RtMidi Error case :/
          if (name == ""){
            continue;
          }

          if (KnownPorts->count(name)){
            mi = KnownPorts->at(name);
            mi->PortNumber = i;

            seen[name] = true;
          } else {
            mi = new MidiIn(portRegistry, name, i);

            mi->publishDeviceDiscovered();
          }

          // this may trigger device connected notifications
          mi->setDeviceState(DeviceStateConnected);

          result->push_back(mi);
        }

      } catch ( RtMidiError &error ) {
        std::cerr << "ERROR foooo" << std::endl;
        error.printMessage();
      }

      delete midiin;

      //detect devices gone offline
      std::for_each(seen.begin(), seen.end(), [](std::pair<std::string,bool> pair){
        if (pair.second == false){
          KnownPorts->at(pair.first)->setDeviceState(DeviceStateNotConnected);
        }
      });

      return result;
    }


    MidiIn::MidiIn(PortRegistry * portRegistry, std::string portName, unsigned int portNumber) : AbstractPort(portRegistry, TypeInput, portName) {
      PortNumber = portNumber;

      (*KnownPorts)[portName] = this;

      portRegistry->registerPort( this );
    }

    MidiIn::~MidiIn(){
      if (MidiPort != NULL){
        MidiPort->closePort();
        delete MidiPort;
      }
    }

    void MidiIn::rtMidiCallback( double timeStamp, std::vector<unsigned char> *message, void * midiInRef ){
      assert(midiInRef != NULL);

      MidiIn * midiIn = (MidiIn*)midiInRef;

      // if (message->size() == 3){
      //
      //   std::cout << "rx [" << midiIn->Name << "](" << message->size() << ") to (" << midiIn->Connections.size() << ") ";
      //   std::for_each(message->begin(), message->end(), [](unsigned char c){
      //     std::cout << std::hex << (int)c << " ";
      //   });
      //   std::cout << std::endl;
      // }

      std::for_each(midiIn->Connections.begin(), midiIn->Connections.end(), [message](AbstractPort* port){
        dynamic_cast<AbstractOutputPort*>(port)->send(message);
      });
    }

    void MidiIn::start(){
      //
      // if (getDeviceState() == DeviceStateNotConnected){
      //   return;
      // }

      if (MidiPort != NULL){
        return;
      }

      // std::cout << "MidiIn:" << Name << " start (PortNumber = " << PortNumber << ")" << std::endl;

      MidiPort = new RtMidiIn();

      MidiPort->setCallback( rtMidiCallback, this );
      // Don't ignore sysex, timing, or active sensing messages.
      MidiPort->ignoreTypes( false, false, false );

      MidiPort->openPort(PortNumber);

      assert(MidiPort->isPortOpen());
    }

    void MidiIn::stop(){

      if (MidiPort == NULL){
        return;
      }

      MidiPort->closePort();

      delete MidiPort;

      MidiPort = NULL;
    }

  }

}
