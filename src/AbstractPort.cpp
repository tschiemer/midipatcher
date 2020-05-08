#include <AbstractPort.hpp>

#include "PortRegistry.hpp"

namespace MidiPatcher {

  AbstractPort::AbstractPort(PortRegistry * portRegistry, Type_t type, std::string name){
    // Connections = new std::vector<AbstractPort *>();
// std::cout << "mingle" << std::endl;
    Type = type;
    Name = name;

    portRegistry->registerPort( this );

  };

  AbstractPort::~AbstractPort(){
    // all connections must first be removed
    // yea base destructor is called last - whereas it should be called first here really.
    // but this assertion will fail either way if destruction was handled wrong (assuming
    // no other error related to this occurring first....)
    assert( Connections.size() == 0);
  };

}
