#ifndef MIDIPATCHER_ABSTRACT_CONTROL_H
#define MIDIPATCHER_ABSTRACT_CONTROL_H

#include "PortRegistry.hpp"
#include "Port/AbstractPort.hpp"

namespace MidiPatcher {

  class AbstractControl : public virtual PortRegistry::PortRegistryUpdateReceiver {

    public:

      AbstractControl(PortRegistry * portRegistry);

    protected:

      PortRegistry * PortRegistryRef;

      bool OptReturnIds = false;


      MidiPatcher::AbstractPort * getPortByIdOrKey( std::string &idOrKey );

      void handleCommand(std::vector<std::string> &argv);

      virtual void respond(std::vector<std::string> &argv) = 0;

      void respond(const char * fmt, ...);

      void ok();

      void error(std::string msg = "");

    public:

      // void deviceDiscovered(AbstractPort * port);
      void deviceStateChanged( MidiPatcher::AbstractPort * port, MidiPatcher::AbstractPort::DeviceState_t newState);
      void portRegistered( MidiPatcher::AbstractPort * port );
      void portUnregistered( MidiPatcher::AbstractPort * port );
      void portsConnected( MidiPatcher::AbstractPort * inport, MidiPatcher::AbstractPort * outport );
      void portsDisconnected( MidiPatcher::AbstractPort * inport, MidiPatcher::AbstractPort * outport );

  };

}

#endif /* MIDIPATCHER_ABSTRACT_CONTROL_H */
