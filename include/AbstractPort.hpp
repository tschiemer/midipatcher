#ifndef ABSTRACT_PORT_H
#define ABSTRACT_PORT_H

// #include <utility>
#include <string>
#include <vector>
#include <cassert>

#include <iostream>

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
      typedef AbstractPort * (*PortFactory)(PortRegistry * portRegistry, int argc, char * argv[]);

      struct PortDeclaration {
        std::string Key;
        void (*Init)(void);
        void (*Deinit)(void);
        PortScanner Scanner;
        PortFactory Factory;
        PortDeclaration(std::string key, void (*init)(void), void (*deinit)(void), PortScanner scanner, PortFactory factory){
          assert( key.size() > 0 );
          assert( init != NULL );
          assert( deinit != NULL );
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

      unsigned int Id;

      Type_t Type;

      std::string Name;

    protected:

      std::vector<AbstractPort *> Connections;

      // AbstractPort(){
      //   std::cout << "abstract!" << std::endl;
      // }

      AbstractPort(PortRegistry * portRegistry, Type_t type, std::string name);

    public:

      virtual ~AbstractPort();

    public:

      virtual std::string getKey() = 0;

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
