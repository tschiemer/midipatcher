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
  class PortRegistry;

  typedef std::vector<AbstractPort*>  * (*PortScanner)(PortRegistry * portRegistry);

  class AbstractPort {

    public:

      typedef enum {
        TypeInput        = 1,
        TypeOutput       = 2,
        // TypePassthrough  = 3,
      } Type_t;

      // typedef enum {
      //   ClassMidi        = 0x10,
      //   ClassVirtualMidi = 0x30,
      //   ClassStream      = 0x40,
      // } Family_t;

      unsigned int Id;

      Type_t Type;
      // Class_t Class;

      std::string Name;

    protected:

      std::vector<AbstractPort *> Connections;

    // Port(PortType_t portType, unsigned int rtMidiId, std::string name ){
    //   PortType = portType;
    //   RtMidiId = rtMidiId;
    //   Name = name;
    // }

      AbstractPort(){}

    public:

      AbstractPort(PortRegistry * portRegistry);

      virtual ~AbstractPort();

    public:

      virtual std::string getKey() = 0;

      virtual bool operator==(const AbstractPort& rhs){
        return Id == rhs.Id;
      }

      std::vector<AbstractPort *> * getConnections(){
        return &Connections;
      }

      virtual void foo(AbstractPort*p){};

      void addConnection(AbstractPort * port){
        addConnectionImpl(port);
        Connections.push_back(port);
      }

      void removeConnection(AbstractPort * port){
        removeConnectionImpl(port);
        std::remove(Connections.begin(), Connections.end(), port);
      }

    protected:

      virtual void addConnectionImpl(AbstractPort * port){};

      virtual void removeConnectionImpl(AbstractPort * port){};
  };

}

#endif /* ABSTRACT_PORT_H */
