#include <Port/VirtMidiOut.hpp>

#include <iostream>

namespace MidiPatcher {
  namespace Port {

    VirtMidiOut::VirtMidiOut(PortRegistry * portRegistry, std::string portName) : AbstractPort(portRegistry, TypeInput, portName){

      try {

        MidiPort = new RtMidiOut();

        MidiPort->openVirtualPort( Name );

      } catch ( RtMidiError &error ) {

        error.printMessage();
      }

      publishDeviceDiscovered();

      setDeviceState(DeviceStateConnected);
    }

    VirtMidiOut::~VirtMidiOut(){

      setDeviceState(DeviceStateNotConnected);

      delete MidiPort;
    }


    void VirtMidiOut::send(unsigned char *message, size_t len){

      std::cout << "send 1" << std::endl;

      if (getDeviceState() == DeviceStateNotConnected){
        return;
      }

      std::cout << "send 2" << std::endl;

      if (MidiPort == NULL){
        return;
      }

      std::cout << "send 3" << std::endl;


      MidiPort->sendMessage(message, len);
    }

  }
}
