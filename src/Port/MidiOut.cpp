#include <Port/MidiOut.hpp>

#include <PortRegistry.hpp>

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

      (*KnownPorts)[portName] = this;

      // portRegistry->registerPort( this );
    }

    MidiOut::~MidiOut() {
      if (MidiPort != NULL){
        MidiPort->closePort();
        delete MidiPort;
      }
    }

    void MidiOut::registerPort(PortRegistry &portRegistry){
      portRegistry.registerPort(this);
    }

    void MidiOut::start(){

      // if (getDeviceState() == DeviceStateNotConnected){
      //   return;
      // }
      // std::cout << "MidiOut:" << Name << " START request" << std::endl;

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

    void MidiOut::sendMessage(unsigned char * message, size_t len){
      // std::cout << "send 1" << std::endl;

      if (getDeviceState() == DeviceStateNotConnected){
        return;
      }

      // std::cout << "send 2" << std::endl;

      if (MidiPort == NULL){
        return;
      }
      // std::cout << "send 3" << std::endl;
      if (MidiPort->isPortOpen() == false){
        return;
      }
      // std::cout << "send 4" << std::endl;

                  // if (len == 3){
                  //   std::cout << "tx [" << Name << "](" << len << ") ";
                  //   for(int i = 0; i < len; i++){
                  //     std::cout << std::hex << (int)message[i] << " ";
                  //   };
                  //   std::cout << std::endl;
                  // }
// std::cout << "MO.sendMessage" << std::endl;
      MidiPort->sendMessage(message, len);
    }
  }

}
