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
      bool OptNotificationsEnabled = false;


      MidiPatcher::AbstractPort * getPortByIdOrKey( std::string &idOrKey );

      void handleCommand(std::vector<std::string> &argv);

      virtual void respond(std::vector<std::string> &argv) = 0;
      virtual void publishNotification(std::vector<std::string> &argv);

      void respond(const char * fmt, ...);

      void ok();

      void error(std::string msg = "");

    public:

      virtual void deviceStateChanged( MidiPatcher::AbstractPort * port, MidiPatcher::AbstractPort::DeviceState_t newState);
      virtual void portRegistered( MidiPatcher::AbstractPort * port );
      virtual void portUnregistered( MidiPatcher::AbstractPort * port );
      virtual void portsConnected( MidiPatcher::AbstractPort * inport, MidiPatcher::AbstractPort * outport );
      virtual void portsDisconnected( MidiPatcher::AbstractPort * inport, MidiPatcher::AbstractPort * outport );

  };

}

#endif /* MIDIPATCHER_ABSTRACT_CONTROL_H */
