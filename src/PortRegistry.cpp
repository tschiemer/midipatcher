#include <PortRegistry.hpp>

#include <AbstractPort.hpp>
#include <Port/MidiIn.hpp>
#include <Port/MidiOut.hpp>

#include <cassert>
#include <utility>


namespace MidiPatcher {

  std::map<std::string, PortScanner> * PortRegistry::PortScanners = NULL;

  void PortRegistry::init(){
    PortScanners = new std::map<std::string, PortScanner>();

    PortScanners->at(Port::MidiIn::Key) = Port::MidiIn::scan;
    PortScanners->at(Port::MidiOut::Key) = Port::MidiOut::scan;
  }

  void PortRegistry::deinit(){
    delete PortScanners;
  }

  PortRegistry::PortRegistry(){
    // do nothing
  }

  PortRegistry::~PortRegistry(){
    for_each(Ports.begin(), Ports.end(), [](AbstractPort * e){
      delete e;
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

    void PortRegistry::rescan(){

      for (std::map<std::string, PortScanner>::iterator it = PortScanners->begin(); it != PortScanners->end(); ++it)
      {
        it->second(this);
      }

    }


    void PortRegistry::connectPorts(AbstractPort * input, AbstractPort * output){
      assert( input != NULL );
      assert( input->Type == AbstractPort::TypeInput );
      assert( output != NULL );
      assert( output->Type == AbstractPort::TypeOutput );

      input->addConnection(output);
      output->addConnection(input);
    }
}
