#ifndef PORT_REGISTRY_H
#define PORT_REGISTRY_H

#include <string>
#include <vector>
#include <map>
#include <utility>

#include <iostream>

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

      // std::vector<AbstractPort*> * findPortsByType( AbstractPort::Type_t portType );
      //
      // std::vector<AbstractPort*> * findPortsByKey( std::string key );

      void registerPort(AbstractPort * port){
        // std::cout << "register " << portf << std::endl;
        port->Id = ++EntryIncrement;
        Ports.push_back(port);
      }

      void rescan();

      void connectPorts(AbstractInputPort * input, AbstractOutputPort * output);

      void connectPortsById(unsigned int inputId, unsigned int outputId);

      void connectPortsByName(std::string inputName, std::string outputName);
  };

}

#endif
