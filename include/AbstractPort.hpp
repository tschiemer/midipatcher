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

    protected:

      unsigned int Id;

      Type_t Type;

      std::string Name;

    public:

      unsigned int getId(){
        return Id;
      }

      Type_t getType(){
        return Type;
      }

      virtual std::string getPortClass() = 0;

      virtual std::string getKey() {
        return getPortClass() + ":" + Name;
      };


      virtual PortDescriptor * getPortDescriptor() {
        return new PortDescriptor("AbstractPort","Corona Milkbar");
      };

      virtual bool operator==(const AbstractPort& rhs){
        std::cout << "cmp " << Id << " ?== " << rhs.Id << std::endl;
        return Id == rhs.Id;
      }

    protected:

      AbstractPort(PortRegistry * portRegistry, Type_t type, std::string name);

    public:

      virtual ~AbstractPort();

    private:

      volatile DeviceState_t DeviceState = DeviceStateNotConnected;

    protected:

      void setDeviceState(DeviceState_t newState){
        if (DeviceState == newState){
          return;
        }

        DeviceState = newState;

        if (DeviceState == DeviceStateConnected){
          onDeviceConnected();
        } else {
          onDeviceDisconnected();
        }

        publishDeviceStateChanged();
      }

    public:

      DeviceState_t getDeviceState(){
        return DeviceState;
      }

    protected:

      std::map<std::string,AbstractPort *> Connections;

    public:

      std::vector<AbstractPort *> * getConnections(){
        return NULL;
      }

    protected:

      virtual void onDeviceConnected();
      virtual void onDeviceDisconnected();

      void addConnection(AbstractPort * port);
      void removeConnection(AbstractPort * port);

    protected:

      virtual void addConnectionImpl(AbstractPort * port) {};

      virtual void removeConnectionImpl(AbstractPort * port) {};

      virtual void start(){};
      virtual void stop(){};

    public:

      class PortUpdateReceiver {
        public:
          virtual void deviceDiscovered(AbstractPort * port) = 0;
          virtual void deviceStateChanged(AbstractPort * port, DeviceState_t newState) = 0;
      };

    protected:

      std::vector<PortUpdateReceiver *> PortUpdateReceiverList;

      void publishDeviceDiscovered(){
        // std::thread([this]() {
        //   while (AutoscanEnabled)
        //   {
        //     this->rescan();
        //     std::this_thread::sleep_for(std::chrono::milliseconds(AutoscanIntervalMsec));
        //   }
        // });
        // AutoscanThread.detach()
        std::for_each(PortUpdateReceiverList.begin(), PortUpdateReceiverList.end(), [this](PortUpdateReceiver* receiver){
          receiver->deviceDiscovered( this );
        });
      }

      void publishDeviceStateChanged(){
        std::for_each(PortUpdateReceiverList.begin(), PortUpdateReceiverList.end(), [this](PortUpdateReceiver* receiver){
          receiver->deviceStateChanged( this, this->getDeviceState() );
        });
      }

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
