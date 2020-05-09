#ifndef ABSTRACT_PORT_H
#define ABSTRACT_PORT_H

// #include <utility>
#include <string>
#include <vector>
#include <map>
#include <cassert>

#include <iostream>

#include <PortDescriptor.hpp>
// #include <PortClassRegistryInfo.hpp>

namespace MidiPatcher {

  // Forward declarations
  class AbstractPort;
  class AbstractInputPort;
  class AbstractOutputPort;
  class PortRegistry;


  class AbstractPort {

    friend class PortRegistry;

    public:

      typedef std::vector<AbstractPort*>  * (*PortScanner)(PortRegistry * portRegistry);
      typedef AbstractPort * (*PortFactory)(PortRegistry * portRegistry, PortDescriptor * portDescriptor);

      struct PortClassRegistryInfo {
        std::string Key;
        void (*Init)(void);
        void (*Deinit)(void);
        PortScanner Scanner;
        PortFactory Factory;
        PortClassRegistryInfo(std::string key, PortFactory factory, void (*init)(void) = NULL, void (*deinit)(void) = NULL, PortScanner scanner = NULL){
          assert( key.size() > 0 );
          assert( factory != NULL );

          Key = key;
          Init = init;
          Deinit = deinit;
          Scanner = scanner;
          Factory = factory;
        }
        bool operator==(const PortClassRegistryInfo& rhs){
          return Key == rhs.Key;
        }
      };


      typedef enum {
        TypeInput        = 1,
        TypeOutput       = 2,
      } Type_t;


      typedef enum {
        DeviceStateNotConnected = 0,
        DeviceStateConnected    = 1
      } DeviceState_t;


      unsigned int Id;

      Type_t Type;

      std::string Name;

      virtual std::string getKey() = 0;

      virtual PortDescriptor * getPortDescriptor() {
          return new PortDescriptor(getKey(), Name);
      };

      virtual bool operator==(const AbstractPort& rhs){
        std::cout << "cmp " << Id << " ?== " << rhs.Id << std::endl;
        return Id == rhs.Id;
      }

    protected:

      AbstractPort(PortRegistry * portRegistry, Type_t type, std::string name);

    public:

      virtual ~AbstractPort();

    protected:

      DeviceState_t DeviceState = DeviceStateNotConnected;

    public:

      DeviceState_t getDeviceState(){
        return DeviceState;
      }

    protected:

      std::vector<AbstractPort *> Connections;

    public:

      std::vector<AbstractPort *> * getConnections(){
        return &Connections;
      }

    protected:

      void addConnection(AbstractPort * port){
        addConnectionImpl(port);
        Connections.push_back(port);
      }

      void removeConnection(AbstractPort * port){
        removeConnectionImpl(port);
        Connections.erase(std::remove(Connections.begin(), Connections.end(), port));
      }

    protected:

      virtual void addConnectionImpl(AbstractPort * port) {};

      virtual void removeConnectionImpl(AbstractPort * port) {};

    public:

      class PortUpdateReceiver {
        public:
          virtual void deviceStateChanged(AbstractPort * port, DeviceState_t newState) = 0;
      };

    protected:

      std::vector<PortUpdateReceiver *> PortUpdateReceiverList;

    public:

      void subscribePortUpdateReveicer(PortUpdateReceiver *receiver){
        PortUpdateReceiverList.push_back(receiver);
      }

      void unsubscribePortUpdateReveicer(PortUpdateReceiver *receiver){
        PortUpdateReceiverList.erase(std::remove(PortUpdateReceiverList.begin(), PortUpdateReceiverList.end(), receiver));
      }

  };

}

#endif /* ABSTRACT_PORT_H */
