#include <Port/VirtMidiIn.hpp>

#include <PortRegistry.hpp>
#include <Port/AbstractOutputPort.hpp>
#include <Log.hpp>

namespace MidiPatcher {
  namespace Port {

    VirtMidiIn::VirtMidiIn(std::string portName) : AbstractInputPort(portName){

      try {

        MidiPort = new RtMidiIn();

        MidiPort->setCallback( rtMidiCallback, this );
        // Don't ignore sysex, timing, or active sensing messages.
        MidiPort->ignoreTypes( false, false, false );

        MidiPort->openVirtualPort( Name );

      } catch ( RtMidiError &error ) {
        Log::error(error);

        throw;
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

      Log::debug(midiIn->getKey(), "received", &message->at(0), message->size());

      midiIn->receivedMessage(&message->at(0), message->size());
    }

  }
}
