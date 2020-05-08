#include <PortRegistry.hpp>

#include <AbstractPort.hpp>
#include <Port/MidiIn.hpp>
#include <Port/MidiOut.hpp>

#include <cassert>
#include <utility>

#include <iostream>


namespace MidiPatcher {

  std::map<std::string, AbstractPort::PortDeclaration*> * PortRegistry::PortDeclarations = NULL;

  void PortRegistry::init(){
    PortDeclarations = new std::map<std::string, AbstractPort::PortDeclaration*>();

    /** ADD NEW PORT DECLARATIONS HERE **/
    (*PortDeclarations)[Port::MidiIn::Key] = Port::MidiIn::getDeclaration();
    (*PortDeclarations)[Port::MidiOut::Key] = Port::MidiOut::getDeclaration();
    /** ADD NEW PORT DECLARATIONS HERE **/

    for (std::map<std::string, AbstractPort::PortDeclaration*>::iterator it = PortDeclarations->begin(); it != PortDeclarations->end(); ++it)
    {
      it->second->Init();
    }
  }

  void PortRegistry::deinit(){

    for (std::map<std::string, AbstractPort::PortDeclaration*>::iterator it = PortDeclarations->begin(); it != PortDeclarations->end(); ++it)
    {
      it->second->Deinit();
    }

    delete PortDeclarations;
  }

  PortRegistry::PortRegistry(){
    // do nothing
  }

  PortRegistry::~PortRegistry(){
    // disableAutoscan();

    // properly delete all ports
    for_each(Ports.begin(), Ports.end(), [](AbstractPort * port){
      //
      std::vector<AbstractPort *> * connections = port->getConnections();

      std::for_each(connections->begin(), connections->end(), [port](AbstractPort*other){
        port->removeConnection(other);
        other->removeConnection(port);
      });

      delete port;
    });

  }

  AbstractPort * PortRegistry::getPortById( unsigned int id ){
    AbstractPort * result = NULL;

    // assuming we never delete Ports this works...
    result = Ports.at(id - 1);

    return result;
  }

  AbstractPort * PortRegistry::findPortByName( std::string needle, AbstractPort::Type_t portType){
    AbstractPort * result = NULL;

    for(int i = 0; result == NULL && i < Ports.size(); i++){
      AbstractPort * port = Ports.at(i);
      if (port->Type == portType && port->Name == needle){
        result = port;
      }
    }

    return result;
  }


  void PortRegistry::enableAutoscan(){
    if (AutoscanEnabled == true){
      return;
    }

    AutoscanEnabled = true;

    static const constexpr unsigned int interval = 1000;

    AutoscanThread = std::thread([this]() {
      while (AutoscanEnabled)
      {
        // std::cout << "rescan" << std::endl;
        this->rescan();
        std::this_thread::sleep_for(std::chrono::milliseconds(interval));
      }
    });
    AutoscanThread.detach();
  }

  void PortRegistry::disableAutoscan(){
    AutoscanEnabled = false;
    AutoscanThread.join();
  }

    void PortRegistry::rescan(){

      for (std::map<std::string, AbstractPort::PortDeclaration*>::iterator it = PortDeclarations->begin(); it != PortDeclarations->end(); ++it)
      {
        // std::cout << "Scanning " << it->first << std::endl;
        if (it->second->Scanner != NULL){
          it->second->Scanner(this);
        }
      }

    }

    void PortRegistry::connectPorts(AbstractPort *input, AbstractPort *output){
      assert(input != NULL);
      assert(output != NULL);

      input->addConnection(output);
      output->addConnection(input);
    }


    void PortRegistry::disconnectPorts(AbstractPort *input, AbstractPort *output){
      assert(input != NULL);
      assert(output != NULL);


      input->removeConnection(output);
      output->removeConnection(input);
    }


    // void PortRegistry::connectPorts(AbstractInputPort * input, AbstractOutputPort * output){
    //   assert( input != NULL );
    //   assert( output != NULL );
    //
    //   input->addConnection(output);
    //   output->addConnection(input);
    // }

    // void PortRegistry::connectPortsById(unsigned int inputId, unsigned int outputId){
    //   connectPorts( dynamic_cast<AbstractInputPort*>(getPortById(inputId)), dynamic_cast<AbstractOutputPort*>(getPortById(outputId)) );
    // }
    //
    // void PortRegistry::connectPortsByName(std::string inputName, std::string outputName){
    //   connectPorts( dynamic_cast<AbstractInputPort*>(findPortByName(inputName, AbstractPort::TypeInput)), dynamic_cast<AbstractOutputPort*>(findPortByName(outputName, AbstractPort::TypeOutput)) );
    // }
}
