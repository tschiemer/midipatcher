#include <Port/Serial.hpp>

#include <Error.hpp>

namespace MidiPatcher {
  namespace Port {

    AbstractPort* Serial::factory(PortDescriptor &portDescriptor){
      assert( portDescriptor.PortClass == PortClass );

      // if (portDescriptor.Options.count("device") == 0){
      //   throw Error("Missing 'device' argument!")
      // }

      std::string device = (portDescriptor.Options.count("device") == 0) ? portDescriptor.Name : portDescriptor.Options["device"];
      bool runningStatusEnabled = portDescriptor.Options.count("runningstatus") && portDescriptor.Options["runningstatus"][0] != '\0' ? (portDescriptor.Options["runningstatus"][0] == '1') : true;

      return new Serial(portDescriptor.Name, device, runningStatusEnabled);
    }

    std::vector<AbstractPort*>  * Serial::scanner(){
      return new std::vector<AbstractPort*>();
    }

    Serial::Serial(std::string portName, std::string device, bool runningStatusEnabled) : SerialPort(IOService, device){
      Device = device;
      RunningStatusEnabled = runningStatusEnabled;
    }

    Serial::~Serial(){

    }

    void Serial::writeToStream(unsigned char * data, size_t len){

    }
  }
}
