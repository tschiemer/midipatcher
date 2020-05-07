#include <AbstractPort.hpp>

#include "PortRegistry.hpp"

namespace MidiPatcher {

  AbstractPort::AbstractPort(PortRegistry * portRegistry){
    portRegistry->registerPort( this );
  };

  AbstractPort::~AbstractPort(){
    // all connections must first be removed
    // yea base destructor is called last - whereas it should be called first here really.
    // but this assertion will fail either way if destruction was handled wrong (assuming
    // no other error related to this occurring first....)
    assert( Connections.size() == 0);

    destructorImpl();
  };

}
