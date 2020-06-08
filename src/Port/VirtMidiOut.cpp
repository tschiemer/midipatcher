#include <Port/VirtMidiOut.hpp>

#include <PortRegistry.hpp>
#include <Log.hpp>
#include <Error.hpp>

namespace MidiPatcher {
  namespace Port {

    VirtMidiOut::VirtMidiOut(std::string portName) : AbstractOutputPort(portName){

      try {

        MidiPort = new RtMidiOut();

        MidiPort->openVirtualPort( Name );

        if (MidiPort->isPortOpen() == false){
          new Error("foo");
        }

      } catch ( std::exception &e ) {

        Log::error(e);

        throw;
      }

      // publishDeviceDiscovered();

      setDeviceState(DeviceStateConnected);
    }

    VirtMidiOut::~VirtMidiOut(){

      setDeviceState(DeviceStateNotConnected);

      delete MidiPort;
    }

    void VirtMidiOut::sendMessageImpl(unsigned char * message, size_t len){

      if (MidiPort == NULL){
        Log::error(getKey(), "not started, but trying to send!");
        return;
      }

      Log::debug(getKey(), "sending", message, len);

      MidiPort->sendMessage(message, len);
    }

  }
}
