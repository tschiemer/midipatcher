#include <Port/VirtMidiIn.hpp>

#include <PortRegistry.hpp>
#include <AbstractOutputPort.hpp>

#include <iostream>

namespace MidiPatcher {
  namespace Port {

    VirtMidiIn::VirtMidiIn(std::string portName) : AbstractPort(TypeInput, portName){

      try {

        MidiPort = new RtMidiIn();

        MidiPort->setCallback( rtMidiCallback, this );
        // Don't ignore sysex, timing, or active sensing messages.
        MidiPort->ignoreTypes( false, false, false );

        MidiPort->openVirtualPort( Name );

      } catch ( RtMidiError &error ) {
        error.printMessage();
      }

      // publishDeviceDiscovered();

      setDeviceState(DeviceStateConnected);
    }

    VirtMidiIn::~VirtMidiIn(){
      setDeviceState(DeviceStateNotConnected);

      delete MidiPort;
    }

    void VirtMidiIn::registerPort(PortRegistry &portRegistry){
      portRegistry.registerPort(this);
    }

    void VirtMidiIn::rtMidiCallback( double timeStamp, std::vector<unsigned char> *message, void * midiInRef ){
      assert(midiInRef != NULL);

      VirtMidiIn * midiIn = (VirtMidiIn*)midiInRef;

      if (midiIn->getDeviceState() == DeviceStateNotConnected){
        return;
      }

      midiIn->send(message);
      // std::for_each(midiIn->Connections.begin(), midiIn->Connections.end(), [message](AbstractPort* port){
      //   dynamic_cast<AbstractOutputPort*>(port)->send(message);
      // });
    }

  }
}
