#include <Port/VirtMidiIn.hpp>

#include <AbstractOutputPort.hpp>

#include <iostream>

namespace MidiPatcher {
  namespace Port {

    VirtMidiIn::VirtMidiIn(PortRegistry * portRegistry, std::string portName) : AbstractPort(portRegistry, TypeInput, portName){

      std::cout << "Opening " << portName << std::endl;

      try {

        MidiPort = new RtMidiIn();

        MidiPort->setCallback( rtMidiCallback, this );
        // Don't ignore sysex, timing, or active sensing messages.
        MidiPort->ignoreTypes( false, false, false );

        MidiPort->openVirtualPort( Name );

        // assert(MidiPort->isPortOpen());

      } catch ( RtMidiError &error ) {
        // std::cerr << "ERROR foooo" << std::endl;
        error.printMessage();
      }

      publishDeviceDiscovered();

      setDeviceState(DeviceStateConnected);
    }

    VirtMidiIn::~VirtMidiIn(){
      setDeviceState(DeviceStateNotConnected);
      
      delete MidiPort;
    }


    void VirtMidiIn::rtMidiCallback( double timeStamp, std::vector<unsigned char> *message, void * midiInRef ){
      assert(midiInRef != NULL);

      VirtMidiIn * midiIn = (VirtMidiIn*)midiInRef;

      if (midiIn->getDeviceState() == DeviceStateNotConnected){
        return;
      }

      std::for_each(midiIn->Connections.begin(), midiIn->Connections.end(), [message](AbstractPort* port){
        dynamic_cast<AbstractOutputPort*>(port)->send(message);
      });
    }

  }
}
