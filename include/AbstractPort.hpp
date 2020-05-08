#ifndef ABSTRACT_PORT_H
#define ABSTRACT_PORT_H

// #include <utility>
#include <string>
#include <vector>
#include <map>
#include <cassert>

#include <iostream>

#include <PortDescriptor.hpp>
// #include <PortDeclaration.hpp>

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

      struct PortDeclaration {
        std::string Key;
        void (*Init)(void);
        void (*Deinit)(void);
        PortScanner Scanner;
        PortFactory Factory;
        PortDeclaration(std::string key, PortFactory factory, void (*init)(void) = NULL, void (*deinit)(void) = NULL, PortScanner scanner = NULL){
          assert( key.size() > 0 );
          assert( factory != NULL );

          Key = key;
          Init = init;
          Deinit = deinit;
          Scanner = scanner;
          Factory = factory;
        }
        bool operator==(const PortDeclaration& rhs){
          return Key == rhs.Key;
        }
      } ;


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

      DeviceState_t DeviceState = DeviceStateNotConnected;

      std::string Name;

    protected:

      PortRegistry * PortRegistryRef;

      std::vector<AbstractPort *> Connections;

      // AbstractPort(){
      //   std::cout << "abstract!" << std::endl;
      // }

      AbstractPort(PortRegistry * portRegistry, Type_t type, std::string name);

    public:

      virtual ~AbstractPort();



      PortRegistry * getPortRegistry(){
        return PortRegistryRef;
      }

    public:

      virtual std::string getKey() = 0;

      virtual PortDescriptor * getPortDescriptor() {
          return new PortDescriptor(getKey(), Name);
      };

      // virtual PortDeclaration * getDeclaration() = 0;

      virtual bool operator==(const AbstractPort& rhs){
        std::cout << "cmp " << Id << " ?== " << rhs.Id << std::endl;
        return Id == rhs.Id;
      }

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
  };

}

#endif /* ABSTRACT_PORT_H */
