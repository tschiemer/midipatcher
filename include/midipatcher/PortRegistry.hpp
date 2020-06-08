#ifndef MIDIPATCHER_PORT_REGISTRY_H
#define MIDIPATCHER_PORT_REGISTRY_H

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <thread>

#include <iostream>

#include <RtMidi.h>

#include "Port/AbstractPort.hpp"

namespace MidiPatcher {

  // Forward declarations
  // class AbstractPort;

  class PortRegistry : AbstractPort::PortUpdateReceiver {

    protected:

      std::map<std::string, AbstractPort::PortClassRegistryInfo*> PortClassRegistryInfoMap;


    public:

      PortRegistry(std::vector<AbstractPort::PortClassRegistryInfo *> &pcriList);

      ~PortRegistry();

    public:

      void runloop();

      std::vector<AbstractPort::PortClassRegistryInfo*> * getPortClassRegistryInfoList();

      bool portClassExists( std::string portClass ){
        return PortClassRegistryInfoMap.count(portClass) > 0;
      }

    protected:

      std::map<std::string, AbstractPort*> Ports;

      unsigned int EntryIncrement = 0;

    public:

      void rescan();

      int getPortCount();
      std::vector<AbstractPort*> * getAllPorts(AbstractPort::Type_t type = AbstractPort::TypeAny);

      AbstractPort * getPortById( unsigned int id );
      AbstractPort * getPortByKey(std::string key);

      void registerPort(AbstractPort * port);
      void unregisterPort(AbstractPort * port);

      AbstractPort * registerPortFromDescriptor(PortDescriptor &portDescriptor);

      std::vector<std::pair<AbstractInputPort*,AbstractOutputPort*>> * getAllConnections();

      void connectPorts(AbstractInputPort *input, AbstractOutputPort *output);
      void connectPortsByKey(std::string inputKey, std::string outputKey);

      void disconnectPorts(AbstractInputPort *input, AbstractOutputPort *output);
      void disconnectPortsByKey(std::string inputKey, std::string outputKey);

    protected:

      volatile unsigned int AutoscanIntervalMsec = 1000;
      volatile bool AutoscanEnabled = false;
      std::thread AutoscanThread;

    public:


      bool isAutoscanEnabled(){
        return AutoscanEnabled;
      }
      unsigned int getAutoscanInterval(){
        return AutoscanIntervalMsec;
      }

      void setAutoscanInterval(unsigned int intervalMsec);

      void startAutoscan();
      void stopAutoscan();


      void deviceDiscovered(AbstractPort * port);
      void deviceStateChanged(AbstractPort * port, AbstractPort::DeviceState_t newState);

    public:

      class PortRegistryUpdateReceiver : public virtual PortUpdateReceiver {
        public:
          virtual void portRegistered( AbstractPort * port ) = 0;
          virtual void portUnregistered( AbstractPort * port ) = 0;
          virtual void portsConnected( AbstractPort * inport, AbstractPort * outport ) = 0;
          virtual void portsDisconnected( AbstractPort * inport, AbstractPort * outport ) = 0;
      };

    protected:

      std::vector<PortRegistryUpdateReceiver *> PortRegistryUpdateReceiverList;


      // void publishDeviceDiscovered(AbstractPort * port);
      void publishPortRegistered(AbstractPort * port);
      void publishPortUnregistered(AbstractPort * port);
      void publishDeviceStateChanged(AbstractPort * port, AbstractPort::DeviceState_t newState);
      void publishPortsConnected(  AbstractPort * inport, AbstractPort * outport );
      void publishPortsDisconnected(  AbstractPort * inport, AbstractPort * outport );

    public:

      void subscribePortRegistryUpdateReveicer(PortRegistryUpdateReceiver *receiver);
      void unsubscribePortRegistryUpdateReveicer(PortRegistryUpdateReceiver *receiver);

  };

}

#endif
