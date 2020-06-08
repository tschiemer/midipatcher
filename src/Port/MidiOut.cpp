#include <Port/MidiOut.hpp>

#include <PortRegistry.hpp>

#include <Log.hpp>

#include <cstdlib>
#include <ctime>

namespace MidiPatcher {

  namespace Port {

    std::map<std::string, MidiOut*> * MidiOut::KnownPorts = new std::map<std::string, MidiOut*>();

    AbstractPort* MidiOut::factory(PortDescriptor * portDescriptor){
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

      return new MidiOut(name, api);
    }

    PortDescriptor * MidiOut::getPortDescriptor() {
      std::map<std::string, std::string> opt;
      opt["api"] = std::to_string(Api);
      return new PortDescriptor(PortClass, Name, opt);
    };

    std::vector<AbstractPort*> * MidiOut::scan(PortRegistry * portRegistry){

      // set dirty (seen)
      std::map<std::string,bool> seen;
      std::map<std::string,DeviceState_t> previousState;

      std::for_each(KnownPorts->begin(), KnownPorts->end(), [&seen, &previousState](std::pair<std::string,MidiOut*> pair){
        seen[pair.first] = false;
        previousState[pair.first] = pair.second->getDeviceState();
      });

      RtMidiOut *midiout = NULL;

      std::vector< AbstractPort * > * result = new std::vector< AbstractPort* >();

      std::vector<RtMidi::Api> apis;
      RtMidi::getCompiledApi( apis );

      for(int a = 0; a < apis.size(); a++){
        try {

          midiout = new RtMidiOut(apis[a]);

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
              mi = new MidiOut(name, midiout->getCurrentApi(), i);

              // mi->publishDeviceDiscovered();
            }

            // this may trigger device connected notifications
            mi->setDeviceState(DeviceStateConnected);

            result->push_back(mi);
          }

        } catch ( RtMidiError &error ) {
          Log::error(error);
        }

        delete midiout;
      }

      //detect devices gone offline
      std::for_each(seen.begin(), seen.end(), [](std::pair<std::string,bool> pair){
        if (pair.second == false){
          KnownPorts->at(pair.first)->setDeviceState(DeviceStateNotConnected);
        }
      });

      return result;
    }

    MidiOut::MidiOut(std::string portName, RtMidi::Api api, unsigned int portNumber) : AbstractOutputPort(portName) {
      Api = api;
      PortNumber = portNumber;

      if (KnownPorts == nullptr){
        KnownPorts =  new std::map<std::string,MidiOut*>();
      }

      (*KnownPorts)[portName] = this;

      // portRegistry->registerPort( this );
    }

    MidiOut::~MidiOut() {
      if (MidiPort != NULL){
        MidiPort->closePort();
        delete MidiPort;
      }
      // remove from known ports
      if (KnownPorts->count(Name) > 0){
        KnownPorts->erase(Name);
      }
      // if no ports left, deinit know ports map
      if (KnownPorts->size() == 0){
        delete KnownPorts;
        KnownPorts = nullptr;
      }
    }

    void MidiOut::registerPort(PortRegistry &portRegistry){
      portRegistry.registerPort(this);
    }

    void MidiOut::start(){

      if (MidiPort != NULL){
        return;
      }

      Log::debug(getKey(), "starting");

      try {

        MidiPort = new RtMidiOut();

        MidiPort->openPort(PortNumber);

      } catch (RtMidiError &e){
        Log::error(e);

        stop();
      }

    }

    void MidiOut::stop(){

      if (MidiPort == NULL){
        return;
      }

      Log::debug(getKey(), "stopping");

      MidiPort->closePort();

      delete MidiPort;

      MidiPort = NULL;
    }

    void MidiOut::sendMessageImpl(unsigned char * message, size_t len){

      if (MidiPort == NULL){
        Log::error(getKey(), "not started, but trying to send!");
        return;
      }
      if (MidiPort->isPortOpen() == false){
        Log::error(getKey(), "not open, but trying to send!");
        return;
      }

      Log::debug(getKey(), "sending", message, len);

      MidiPort->sendMessage(message, len);
    }
  }

}
