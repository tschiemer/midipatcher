#ifndef PORT_REGISTRY_H
#define PORT_REGISTRY_H

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

      void init();
      void deinit();

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

      std::vector<AbstractPort*> * getAllPorts();

      AbstractPort * getPortById( unsigned int id );
      AbstractPort * getPortByKey(std::string key);

      void registerPort(AbstractPort * port);
      void unregisterPort(AbstractPort * port);

      AbstractPort * registerPortFromDescriptor(PortDescriptor * portDescriptor);


      void connectPorts(AbstractPort *input, AbstractPort *output);
      void connectPortsByKey(std::string inputKey, std::string outputKey);

      void disconnectPorts(AbstractPort *input, AbstractPort *output);
      void disconnectPortsByKey(std::string inputKey, std::string outputKey);

    protected:

      volatile unsigned int AutoscanIntervalMsec;
      volatile bool AutoscanEnabled = false;
      std::thread AutoscanThread;

    public:


      bool autoscanEnabled(){
        return AutoscanEnabled;
      }
      unsigned int getAutoscanInterval(){
        return AutoscanIntervalMsec;
      }

      void setAutoscanInterval(unsigned int intervalMsec){
        assert( intervalMsec >= 1000 );
        AutoscanIntervalMsec = intervalMsec;
      }

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
