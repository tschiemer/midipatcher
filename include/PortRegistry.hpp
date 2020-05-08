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

  class PortRegistry {

    protected:

      static std::map<std::string, AbstractPort::PortDeclaration*> * PortDeclarations;

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

    protected:

      volatile bool AutoscanEnabled = false;
      std::thread AutoscanThread;

    public:

      void enableAutoscan();
      void disableAutoscan();

      void rescan();


      void connectPorts(AbstractPort *input, AbstractPort *output);

      void disconnectPorts(AbstractPort *input, AbstractPort *output);

    // public:
    //
    //   void connectPorts(AbstractInputPort * input, AbstractOutputPort * output);

      // void connectPortsById(unsigned int inputId, unsigned int outputId);
      //
      // void connectPortsByName(std::string inputName, std::string outputName);
  };

}

#endif
