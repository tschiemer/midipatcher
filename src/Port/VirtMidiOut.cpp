#include <Port/VirtMidiOut.hpp>

#include <PortRegistry.hpp>
#include <Log.hpp>

namespace MidiPatcher {
  namespace Port {

    VirtMidiOut::VirtMidiOut(std::string portName) : AbstractOutputPort(portName){

      try {

        MidiPort = new RtMidiOut();

        MidiPort->openVirtualPort( Name );

      } catch ( RtMidiError &error ) {

        Log::error(error);

        throw;
      }

      // publishDeviceDiscovered();

      setDeviceState(DeviceStateConnected);
    }

    VirtMidiOut::~VirtMidiOut(){

      setDeviceState(DeviceStateNotConnected);

      delete MidiPort;
    }

    void VirtMidiOut::registerPort(PortRegistry &portRegistry){
      portRegistry.registerPort(this);
    }

    void VirtMidiOut::sendMessageImpl(unsigned char * message, size_t len){

      if (MidiPort == NULL){
        Log::error(getKey(), "not started, but trying to send!");
        return;
      }
      if (MidiPort->isPortOpen() == false){
        Log::error(getKey(), "not open, but trying to send!");
        return;
      }

      Log::debug(getKey(), "sending " + std::to_string(len));
      
      MidiPort->sendMessage(message, len);
    }

  }
}
