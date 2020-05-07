#ifndef PORT_REGISTRY_H
#define PORT_REGISTRY_H

#include <string>
#include <vector>
#include <map>
#include <utility>

#include <RtMidi.h>

#include "AbstractPort.hpp"

namespace MidiPatcher {

  // Forward declarations
  // class AbstractPort;

  class PortRegistry {

    protected:

      static std::map<std::string, PortScanner> * PortScanners;

      std::vector<AbstractPort*> Ports;

      unsigned int EntryIncrement = 0;

    public:

      static void init();

      static void deinit();

      PortRegistry();

      ~PortRegistry();

      std::vector<AbstractPort*> * getAllPorts(){
        return &Ports;
      }

      AbstractPort * getPortById( unsigned int id );

      AbstractPort * findPortByName( std::string needle, AbstractPort::Type_t portType );

      void registerPort(AbstractPort * port){
        port->Id = ++EntryIncrement;
        Ports.push_back(port);
      }

      void rescan();

      void connectPorts(AbstractPort * input, AbstractPort * output);

      inline void connectPortsById(unsigned int inputId, unsigned int outputId){
        connectPorts( getPortById(inputId), getPortById(outputId) );
      }

      inline void connectPortsByName(std::string inputName, std::string outputName){
        connectPorts( findPortByName(inputName, AbstractPort::TypeInput), findPortByName(outputName, AbstractPort::TypeOutput) );
      }
  };

}

#endif
