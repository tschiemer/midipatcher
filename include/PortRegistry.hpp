#ifndef PORT_REGISTRY_H
#define PORT_REGISTRY_H

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <thread>

#include <iostream>

#include <RtMidi.h>

#include "AbstractPort.hpp"

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

      std::vector<AbstractPort::PortClassRegistryInfo*> * getPortClassRegistryInfoList();

    protected:

      std::map<std::string, AbstractPort*> Ports;

      unsigned int EntryIncrement = 0;

    public:

      void rescan();

      std::vector<AbstractPort*> * getAllPorts();

      // AbstractPort * getPortById( unsigned int id );

      // AbstractPort * findPortByName( std::string needle, AbstractPort::Type_t portType );

      // std::vector<AbstractPort*> * findPortsByType( AbstractPort::Type_t portType );
      //
      // std::vector<AbstractPort*> * findPortsByKey( std::string key );

      void registerPort(AbstractPort * port);

      AbstractPort * registerPortFromDescriptor(PortDescriptor * portDescriptor);


      void connectPorts(AbstractPort *input, AbstractPort *output);
      // void connectPortsByDescriptor(PortDescriptor * indesc, PortDescriptor * outdesc);

      void disconnectPorts(AbstractPort *input, AbstractPort *output);

    protected:

      volatile unsigned int AutoscanIntervalMsec;
      volatile bool AutoscanEnabled = false;
      std::thread AutoscanThread;

    public:

      void setAutoscanInterval(unsigned int intervalMsec){
        assert( intervalMsec >= 1000 );
        AutoscanIntervalMsec = intervalMsec;
      }

      void enableAutoscan(unsigned int intervalMsec = 1000);
      void disableAutoscan();

      void deviceDiscovered(AbstractPort * port);
      void deviceStateChanged(AbstractPort * port, AbstractPort::DeviceState_t newState);
  };

}

#endif
