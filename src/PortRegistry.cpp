#include <PortRegistry.hpp>

#include <AbstractPort.hpp>
#include <Port/MidiIn.hpp>
#include <Port/MidiOut.hpp>

#include <cassert>


namespace MidiPatcher {

  PortRegistry::PortRegistry(){
    Port::MidiIn::Scanner(this);
    // PortScanners[Port::MidiIn::Key] = Port::MidiIn::Scanner;
    // PortScanners[Port::MidiOut::Key] = Port::MidiOut::Scanner;
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

    void PortRegistry::refresh(){
      //
      // std::vector<std::pair<unsigned int, std::string>> * ports;
      //
      // ports = scanPorts(TypeInput);
      //
      // for(int i = 0; i < ports->size(); i++){
      //   std::pair<unsigned int, std::string> p = ports->at(i);
      //
      //   Entry * entry = findEntryByName(p.second, TypeInput);
      //
      //   // if found, update RtMidi port number
      //   if (entry != NULL){
      //     entry->RtMidiId = p.first;
      //     //TODO add last seen timestamp
      //   }
      //   // if not found/registered, just register it
      //   else {
      //     registerEntry(new Entry(TypeInput, p.first, p.second));
      //   }
      // }
      //
      // delete ports;
      //
      //
      // ports = scanPorts(TypeOutput);
      //
      // for(int i = 0; i < ports->size(); i++){
      //   std::pair<unsigned int, std::string> p = ports->at(i);
      //
      //   Entry * entry = findEntryByName(p.second, TypeOutput);
      //
      //   // if found, update RtMidi port number
      //   if (entry != NULL){
      //     entry->RtMidiId = p.first;
      //     //TODO add last seen timestamp
      //   }
      //   // if not found/registered, just register it
      //   else {
      //     registerEntry(new Entry(TypeOutput, p.first, p.second));
      //
      //   }
      // }
      //
      // delete ports;

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
