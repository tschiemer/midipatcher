#ifndef ABSTRACT_PORT_H
#define ABSTRACT_PORT_H

// #include <utility>
#include <string>
#include <vector>
#include <map>
#include <cassert>
#include <algorithm>

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
      typedef AbstractPort * (*PortFactory)(PortDescriptor * portDescriptor);

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
        TypeInputOutput  = 3
      } Type_t;


      typedef enum {
        DeviceStateNotConnected = 0,
        DeviceStateConnected    = 1
      } DeviceState_t;

      class Message {

        protected:

          std::vector<uint8_t> Data;

        public:

          Message(){}

          Message(std::vector<uint8_t> &data);

          std::vector<uint8_t> & getData(){
            return Data;
          }

          virtual bool empty();
          virtual void clear();

        protected:

          virtual void transmit(AbstractPort * port, void (*tx)(AbstractPort * port, unsigned char * message, size_t len));

        public:

          virtual void sendFrom(AbstractInputPort * port);
          virtual void sendTo(AbstractOutputPort * port);

      };

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

      // virtual std::map<std::string,std::string> getOptions() {
      //   std::map<std::string,std::string> opt;
      //   return opt;
      // }

      virtual PortDescriptor * getPortDescriptor() {
        return new PortDescriptor("AbstractPort","Corona Milkbar");
      };

      virtual bool operator==(const AbstractPort& rhs){
        // std::cout << "cmp " << Id << " ?== " << rhs.Id << std::endl;
        return Id == rhs.Id;
      }

    protected:

      AbstractPort(){};
      AbstractPort(Type_t type, std::string name);

    public:

      virtual ~AbstractPort();

      virtual void registerPort(PortRegistry &portRegistry){}

    private:

      volatile DeviceState_t DeviceState = DeviceStateNotConnected;

    protected:

      void setDeviceState(DeviceState_t newState);

    public:

      inline DeviceState_t getDeviceState(){
        return DeviceState;
      }

    protected:

      std::map<std::string,AbstractPort *> Connections;

    public:

      std::vector<AbstractPort *> * getConnections();

      inline bool isConnectedTo(std::string portKey){
        return Connections.count(portKey) > 0;
      }

      inline bool isConnectedTo(AbstractPort * port){
        return isConnectedTo(port->getKey());
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
          // virtual void deviceDiscovered(AbstractPort * port) = 0;
          virtual void deviceStateChanged(AbstractPort * port, DeviceState_t newState) = 0;
      };

    protected:

      std::vector<PortUpdateReceiver *> PortUpdateReceiverList;

      // void publishDeviceDiscovered();
      void publishDeviceStateChanged();

    public:

      void subscribePortUpdateReveicer(PortUpdateReceiver *receiver);
      void unsubscribePortUpdateReveicer(PortUpdateReceiver *receiver);

  };

}

#endif /* ABSTRACT_PORT_H */
