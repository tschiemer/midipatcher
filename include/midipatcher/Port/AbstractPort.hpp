#ifndef MIDIPATCHER_PORT_ABSTRACT_PORT_H
#define MIDIPATCHER_PORT_ABSTRACT_PORT_H

#include "../PortDescriptor.hpp"
#include "../Error.hpp"

// #include <utility>
#include <string>
#include <vector>
#include <map>
#include <cassert>
#include <algorithm>

// #include <iostream>

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

      typedef std::vector<AbstractPort*>  * (*PortScanner)();
      typedef AbstractPort * (*PortFactory)(PortDescriptor &portDescriptor);

      struct PortClassRegistryInfo {
        std::string Key;
        // void (*Init)(void);
        // void (*Deinit)(void);
        PortScanner Scanner;
        PortFactory Factory;
        PortClassRegistryInfo(std::string key, PortFactory factory, PortScanner scanner = NULL){
          assert( key.size() > 0 );
          assert( factory != NULL );

          Key = key;
          // Init = init;
          // Deinit = deinit;
          Scanner = scanner;
          Factory = factory;
        }
        bool operator==(const PortClassRegistryInfo& rhs){
          return Key == rhs.Key;
        }
      };


      typedef enum {
        TypeAny          = 0,
        TypeInput        = 1,
        TypeOutput       = 2,
        TypeInputOutput  = TypeInput | TypeOutput
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

      virtual PortDescriptor * getPortDescriptor() {
        return new PortDescriptor("AbstractPort","Corona Milkbar");
      };

      virtual std::string getKey() {
        return getPortClass() + ":" + Name;
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

       virtual void registerPort(PortRegistry &portRegistry);
       virtual void unregisterPort(PortRegistry &portRegistry);

    private:

      volatile DeviceState_t DeviceState = DeviceStateNotConnected;

    protected:

      void setDeviceState(DeviceState_t newState);

    public:

      inline DeviceState_t getDeviceState(){
        return DeviceState;
      }

      struct Connection {
        Type_t Type;
        AbstractPort * Port;

        Connection(Type_t type, AbstractPort * port){
          Type = type;
          Port = port;
        }

        Connection(const Connection &other){
          Type = other.Type;
          Port = other.Port;
        }

        bool operator==(const Connection &other){
          return (Type == other.Type && Port == other.Port);
        }
      };


      virtual bool hasOption(std::string key){
        return false;
      }

      virtual std::string getOption(std::string key){
        throw Error("Method getOption not implemented for " + getPortClass());
      }

      virtual void setOption(std::string key, std::string value){
        throw Error("Method setOption not implemented for " + getPortClass());
      }

      void setOptions(std::map<std::string,std::string> & options){
        std::for_each(options.begin(), options.end(), [this](std::pair<std::string, std::string> opt){
          if (hasOption(opt.first)){
            setOption(opt.first, opt.second);
          }
        });
      }

    protected:

      std::vector<Connection> Connections;

    public:

      std::vector<AbstractPort *> * getConnections(Type_t type = TypeAny);

      // inline bool isConnectedToType(std::string portKey, Type_t type){
      //   assert( type == TypeInput || type == TypeOutput );
      //   return Connections.count(portKey) > 0;
      // }

      bool isConnectedToType(AbstractPort * port, Type_t type);
      // {
      //   assert( type == TypeInput || type == TypeOutput );
      //   return isConnectedToType(port->getKey(), type);
      // }

    protected:

      virtual void onDeviceConnected();
      virtual void onDeviceDisconnected();

      void addConnectionAs(AbstractPort * port, Type_t type);
      void removeConnectionAs(AbstractPort * port, Type_t type);

    protected:

      virtual void addConnectionImpl(Connection &connection) {};

      virtual void removeConnectionImpl(Connection &connection) {};

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

#endif /* MIDIPATCHER_PORT_ABSTRACT_PORT_H */
