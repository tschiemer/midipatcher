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

    void VirtMidiOut::sendMessage(unsigned char * message, size_t len){

      if (getDeviceState() == DeviceStateNotConnected){
        return;
      }

      if (MidiPort == NULL){
        return;
      }

      MidiPort->sendMessage(message, len);
    }

  }
}
