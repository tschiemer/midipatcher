#include <Port/MidiIn.hpp>

#include <PortRegistry.hpp>
#include <Port/AbstractOutputPort.hpp>

#include <Log.hpp>

#include <cstdlib>
#include <ctime>

namespace MidiPatcher {

  namespace Port {


    std::map<std::string, MidiIn*> * MidiIn::KnownPorts = NULL;

    AbstractPort* MidiIn::factory(PortDescriptor * portDescriptor){
      assert( portDescriptor->PortClass == PortClass );

      RtMidi::Api api = portDescriptor->Options.count("api") ? static_cast<RtMidi::Api>(std::stoi(portDescriptor->Options["api"])) : RtMidi::UNSPECIFIED;

      std::string name = portDescriptor->Name;

      //
      //
      // size_t pos = name.find(":");
      //
      // if (pos == std::string::npos){
      //   api = RtMidi::UNSPECIFIED;
      // } else {
      //   api = static_cast<RtMidi::Api>(std::stoi(name.substr(0,pos).c_str()));
      //   name.erase(0,pos+1);
      // }

      return new MidiIn(name, api);
    }

    PortDescriptor * MidiIn::getPortDescriptor() {
      std::map<std::string, std::string> opt;
      opt["api"] = std::to_string(Api);
      return new PortDescriptor(PortClass, Name, opt);
    };

    std::vector<AbstractPort*> * MidiIn::scan(PortRegistry * portRegistry){

      // set dirty (seen)
      std::map<std::string,bool> seen;
      std::map<std::string,DeviceState_t> previousState;

      std::for_each(KnownPorts->begin(), KnownPorts->end(), [&seen, &previousState](std::pair<std::string,MidiIn*> pair){
        seen[pair.first] = false;
        previousState[pair.first] = pair.second->getDeviceState();
      });

      RtMidiIn *midiin = NULL;

      std::vector< AbstractPort * > * result = new std::vector< AbstractPort* >();

      // static int counter = 0;

      std::vector<RtMidi::Api> apis;
      RtMidi::getCompiledApi( apis );

      for(int a = 0; a < apis.size(); a++){

        try {

          // std::cout << "SCAN " << counter++ << std::endl;

          midiin = new RtMidiIn(apis[a]);

          // Check inputs.

          unsigned int nPorts = midiin->getPortCount();

          // std::cout << "n = " << nPorts << std::endl;

          for ( unsigned i=0; i<nPorts; i++ ) {

            MidiIn * mi;

            std::string name = midiin->getPortName(i);

            // std::cout << i << " " << name << std::endl;

            // catch RtMidi Error case :/
            if (name == ""){
              continue;
            }

            if (KnownPorts->count(name)){
              mi = KnownPorts->at(name);
              mi->PortNumber = i;

              seen[name] = true;

              // std::cout << "recon " << name << std::endl;
            } else {
              mi = new MidiIn(name, midiin->getCurrentApi(), i);

              // if (portRegistry != nullptr){
              //   portRegistry->register(mi);
              // }

              // mi->publishDeviceDiscovered();

              // std::cout << "disco " << name << std::endl;
            }

            // this may trigger device connected notifications
            mi->setDeviceState(DeviceStateConnected);

            result->push_back(mi);
          }

        } catch ( RtMidiError &error ) {
          Log::error(error);
        }

        delete midiin;
      }

      //detect devices gone offline
      std::for_each(seen.begin(), seen.end(), [](std::pair<std::string,bool> pair){
        if (pair.second == false){
          KnownPorts->at(pair.first)->setDeviceState(DeviceStateNotConnected);
        }
      });

      return result;
    }


    MidiIn::MidiIn(std::string portName, RtMidi::Api api, unsigned int portNumber) : AbstractInputPort(portName) {
      Api = api;
      PortNumber = portNumber;

      (*KnownPorts)[portName] = this;
    }

    MidiIn::~MidiIn(){
      if (MidiPort != NULL){
        MidiPort->closePort();
        delete MidiPort;
      }
      if (KnownPorts->count(Name) > 0){
        KnownPorts->erase(Name);
      }
    }


    void MidiIn::registerPort(PortRegistry &portRegistry){
      portRegistry.registerPort(this);
    }

    void MidiIn::rtMidiCallback( double timeStamp, std::vector<unsigned char> *message, void * midiInRef ){
      assert(midiInRef != NULL);

      MidiIn * midiIn = (MidiIn*)midiInRef;

      if (midiIn->getDeviceState() == DeviceStateNotConnected){
        return;
      }

      Log::debug(midiIn->getKey(), "received", &message->at(0), message->size());

      // if (message->size() != 7){
      //
      //   std::cout << "rx [" << midiIn->Name << "](" << message->size() << ") to (" << midiIn->Connections.size() << ") ";
      //   std::for_each(message->begin(), message->end(), [](unsigned char c){
      //     std::cout << std::hex << (int)c << " ";
      //   });
      //   std::cout << std::endl;
      // }

      // std::cerr << "MidiIn.rx (" << message->size() << ")" << std::endl;

      midiIn->receivedMessage(&message->at(0), message->size());
    }

    void MidiIn::start(){

      if (MidiPort != NULL){
        return;
      }

      Log::debug(getKey(), "starting");

      try {

        MidiPort = new RtMidiIn(Api);

        MidiPort->setCallback( rtMidiCallback, this );
        // Don't ignore sysex, timing, or active sensing messages.
        MidiPort->ignoreTypes( false, false, false );

        MidiPort->openPort(PortNumber);

      } catch( RtMidiError &e ){

        Log::error(e);

        stop();
      }
    }

    void MidiIn::stop(){

      if (MidiPort == NULL){
        return;
      }

      Log::debug(getKey(), "stopping");

      MidiPort->closePort();

      delete MidiPort;

      MidiPort = NULL;
    }

  }

}
