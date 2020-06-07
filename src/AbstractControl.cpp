#include <AbstractControl.hpp>

#include <Port/AbstractPort.hpp>
#include <Port/AbstractInputOutputPort.hpp>

#include <version.h>
#include <Log.hpp>
#include <Error.hpp>


namespace MidiPatcher {
  // 
  // void AbstractControl::escapeArgv(std::string &str){
  //   // " -> \"
  //   // if has SPACE -> "<argument>"
  //   // std::replace(str.begin(), str.end(), " ", "");
  // }
  //
  // void AbstractControl::unescapeArgv(std::string &str){
  //   // std::replace(str.begin(), str.end(), "%20", " ");
  // }

  AbstractControl::AbstractControl(PortRegistry  * portRegistry){
    assert(portRegistry != nullptr);

    PortRegistryRef = portRegistry;

    PortRegistryRef->subscribePortRegistryUpdateReveicer( this );
  }

  AbstractPort * AbstractControl::getPortByIdOrKey( std::string &idOrKey ){

    if (std::all_of(idOrKey.begin(), idOrKey.end(), ::isdigit)){
      return PortRegistryRef->getPortById( std::stoi(idOrKey) );
    }

    return PortRegistryRef->getPortByKey( idOrKey );
  }

  void AbstractControl::handleCommand(std::vector<std::string> &argv){

    // std::cout << "(" << argv.size() << ") ";
    // for(int i = 0; i < argv.size(); i++){
    //   std::cout << argv[i] << " ";
    // }
    // std::cout << std::endl;

    PortRegistryRef->runloop();


    if (argv[0] == "ping"){
      respond("s","pong");
      return ok();
    }

    if (argv[0] == "version"){
      respond("ss", "version", VERSION.c_str());
      return ok();
    }

    if (argv[0] == "option"){
      if (1 == argv.size() || argv.size() > 3){
        return error("Expected: option <option-key> [<option-value>]");
      }

      if (argv[1] == "return-ids"){
        if (argv.size() == 3){
          OptReturnIds = std::stoi(argv[2]);
        }
        respond("ssi", "option", "return-ids", OptReturnIds ? 1 : 0);
        return ok();
      }

      if (argv[1] == "notifications"){
        if (argv.size() == 3){
          OptNotificationsEnabled = std::stoi(argv[2]);
        }
        respond("ssi", "option", "notifications", OptNotificationsEnabled ? 1 : 0);

        return ok();
      }

      if (argv[1] == "autoscan-enabled"){
          if (argv.size() == 3){
            try {
              bool enabled = std::stoi(argv[2]);
              if (enabled){
                PortRegistryRef->startAutoscan();
              } else {
                PortRegistryRef->stopAutoscan();
              }
            } catch (Error &e){
              return error(e.what());
            }
          }

          respond("ssi", "option", "autoscan-enabled", PortRegistryRef->isAutoscanEnabled() ? 1 : 0);

          return ok();
      }

      if (argv[1] == "autoscan-interval"){
        if (argv.size() == 3){
          try {
            PortRegistryRef->setAutoscanInterval(std::stoi(argv[2]));
          } catch (Error &e){
            return error(e.what());
          }
        }

        respond("ssi", "option", "autoscan-interval", PortRegistryRef->getAutoscanInterval());

        return ok();
      }

      return error("Unknown option: " + argv[1]);
    }

    if (argv[0] == "scan"){
      if (argv.size() != 1){
        return error("Expected: scan");
      }
      PortRegistryRef->rescan();
      return ok();
    }

    if (argv[0] == "portclasses"){
      if (argv.size() > 1){
        return error("Expected: portclasses");
      }

      std::vector<MidiPatcher::AbstractPort::PortClassRegistryInfo*> * classes = PortRegistryRef->getPortClassRegistryInfoList();

      respond("si", "portclasses", classes->size());

      for(int i = 0; i < classes->size(); i++){
        respond("ss", "portclasses", classes->at(i)->Key.c_str() );
      }

      delete classes;

      return ok();
    }

    if (argv[0] == "ports"){
      if (argv.size() > 3){
        return error("Expected: ports [<port-id>|<port-key>]");
      }
      if (argv.size() == 2){

        if (argv[1] == "#"){
          respond("si", "ports", PortRegistryRef->getPortCount());
          return ok();
        }

        AbstractPort * port = getPortByIdOrKey(argv[1]);

        if (port == nullptr){
          return error("No such port: " + argv[1]);
        }

        PortDescriptor * desc = port->getPortDescriptor();

        respond("siis", "ports", port->getId(), port->getType(), desc->toString().c_str());

        delete desc;

        return ok();

      } else {

        std::vector<AbstractPort*> * ports = PortRegistryRef->getAllPorts();

        respond("si", "ports", ports->size());

        for(int i = 0; i < ports->size(); i++){
          AbstractPort * port = ports->at(i);
          PortDescriptor * desc = port->getPortDescriptor();
          respond("siis","ports", port->getId(), port->getType(), desc->toString().c_str());
          delete desc;
        }

        delete ports;

        return ok();
      }
    }

    if (argv[0] == "devstate"){

      if (argv.size() > 2){
        return error("Expected: devstate [<port-id>|<port-key>]");
      }

      AbstractPort * port;

      if (argv.size() == 2){

        port = getPortByIdOrKey(argv[1]);

        if (port == nullptr){
          return error("No such port: " + argv[1]);
        }


        int connected = port->getDeviceState() == AbstractPort::DeviceStateConnected ? 1 : 0;

        if (OptReturnIds){
          respond("sii", "devstate", port->getId(), connected  );
        } else {
          respond("ssi", "devstate", port->getKey().c_str(), connected  );
        }

        return ok();

      } else {
        std::vector<AbstractPort*> * ports = PortRegistryRef->getAllPorts();

        respond("si", "devstate", ports->size() );

        for(int i = 0; i < ports->size(); i++){
          AbstractPort * port = ports->at(i);
          int connected = port->getDeviceState() == AbstractPort::DeviceStateConnected ? 1 : 0;
          if (OptReturnIds){
            respond("sii", "devstate", port->getId(), connected );
          } else {
            respond("ssi", "devstate", port->getKey().c_str(), connected );
          }
        }

        delete ports;

        return ok();
      }


    }

    if (argv[0] == "register"){
      if (argv.size() != 2){
        return error("Expected: register <port-descriptor>");
      }

      PortDescriptor * desc = nullptr;

      try {
        desc = PortDescriptor::fromString( argv[1] );
      } catch(Error &e){
        return error(e.what());
      }

      AbstractPort * port = PortRegistryRef->getPortByKey( desc->getKey() );

      if (port != nullptr){
        error("Already registered key: " + desc->getKey());
      } else {

        try {
          port = PortRegistryRef->registerPortFromDescriptor( desc );

          respond("sis", "ports", port->getId(), port->getKey().c_str());

          ok();

        } catch (Error &e) {
          error(e.what());
        }

      }

      delete desc;

      return;
    }

    if (argv[0] == "unregister"){
      if (argv.size() != 2){
        return error("Expected: unregister (<port-id>|<port-key>)");
      }

      AbstractPort * port = getPortByIdOrKey( argv[1] );

      if (port == nullptr){
        return error("No such port: " + argv[1]);
      }

      PortRegistryRef->unregisterPort( port );

      // if (OptReturnIds){
      //   respond("si", "unregister", port->getId());
      // } else {
      //   respond("ss", "unregister", port->getKey().c_str());
      // }

      delete port;

      return ok();
    }

    if (argv[0] == "constate"){
      if (argv.size() > 3){
        return error("Expected: constate [(<port1-id>|<port1-key>) [(<port2-id>|<port2-key>)]]");
      }
      if (argv.size() == 3){
        AbstractPort * inport = getPortByIdOrKey(argv[1]);
        AbstractPort * outport = getPortByIdOrKey(argv[2]);

        if (inport == nullptr){
          return error("No such port: " + argv[1]);
        }
        if (outport == nullptr){
          return error("No such port: " + argv[2]);
        }

        int constate = inport->isConnectedTo(outport) ? 1 : 0;

        if (OptReturnIds){
          respond("siii", "constate", inport->getId(), outport->getId(), constate );
        } else {
          respond("sssi", "constate", argv[1].c_str(), argv[2].c_str(), constate );
        }

        return ok();
      }
      else if (argv.size() == 2){

        AbstractPort * port = getPortByIdOrKey(argv[1]);

        if (port == nullptr){
          return error("No such port: " + argv[1]);
        }

        std::vector<AbstractPort*> * connections = port->getConnections();

        for(int i = 0; i < connections->size(); i++){
          AbstractPort * other = connections->at(i);

          int constate = port->isConnectedTo(other);

          if (OptReturnIds){
            respond("siii", "constate", port->getId(), other->getId(), constate );
          } else {
            respond("sssi", "constate", argv[1].c_str(), other->getKey().c_str(), constate );
          }
        }

        delete connections;

        return ok();
      }
      else {

          std::vector<AbstractPort*> * ports = PortRegistryRef->getAllPorts();

          // respond("si", "constate", ports->size() );

          for(int i = 0; i < ports->size(); i++){
            AbstractPort * port = ports->at(i);
            AbstractInputPort * inport = dynamic_cast<AbstractInputPort*>(port);

            if (inport != nullptr){

              std::vector<AbstractPort*> * connections = port->getConnections();

              for(int j = 0; j < connections->size(); j++){
                AbstractPort * other = connections->at(j);

                int constate = port->isConnectedTo(other);

                if (OptReturnIds){
                  respond("siii", "constate", port->getId(), other->getId(), constate );
                } else {
                  respond("sssi", "constate", port->getKey().c_str(), other->getKey().c_str(), constate );
                }
              }

              delete connections;
            }
          }

          delete ports;

          return ok();
      }

    }

    if (argv[0] == "connect"){
      if (argv.size() != 3){
        return error("Expected: connect (<in-port-id>|<in-port-key>) (<in-port-id>|<out-port-key>)");
      }

      AbstractInputPort * inport = dynamic_cast<AbstractInputPort*>(getPortByIdOrKey(argv[1]));
      AbstractOutputPort * outport = dynamic_cast<AbstractOutputPort*>(getPortByIdOrKey(argv[2]));

      if (inport == nullptr){
        return error("No such input-port: " + argv[1]);
      }
      if (outport == nullptr){
        return error("No such output-port: " + argv[2]);
      }

      if (!inport->isConnectedTo(outport)){
        PortRegistryRef->connectPorts(inport, outport);
      }

      if (OptReturnIds){
        respond("siii", "constate", inport->getId(), outport->getId(), 1);
      } else {
        respond("sssi", "constate", argv[1].c_str(), argv[2].c_str(), 1);
      }

      return ok();
    }

    if (argv[0] == "disconnect"){
      if (argv.size() != 3){
        return error("Expected: disconnect (<in-port-id>|<in-port-key>) (<in-port-id>|<out-port-key>)");
      }

      AbstractInputPort * inport = dynamic_cast<AbstractInputPort*>(getPortByIdOrKey(argv[1]));
      AbstractOutputPort * outport = dynamic_cast<AbstractOutputPort*>(getPortByIdOrKey(argv[2]));

      if (inport == nullptr){
        return error("No such input-port: " + argv[1]);
      }
      if (outport == nullptr){
        return error("No such output-port: " + argv[2]);
      }

      if (inport->isConnectedTo(outport)){
        PortRegistryRef->disconnectPorts(inport, outport);
      }

      if (OptReturnIds){
        respond("sssi", "constate", inport->getId(), outport->getId(), 0);
      } else {
        respond("sssi", "constate", argv[1].c_str(), argv[2].c_str(), 0);
      }

      return ok();
    }

    error("Unknown command");
  }

  void AbstractControl::ok(){
    std::vector<std::string> argv;
    argv.push_back("OK");
    respond(argv);
  }

  void AbstractControl::error(std::string msg){
    std::vector<std::string> argv;

    argv.push_back("ERROR");

    if (msg.size() > 0){
      argv.push_back(msg);
    }

    respond(argv);
  }

  void AbstractControl::respond(const char * fmt, ...){
    assert( fmt != nullptr );

    va_list args;
    va_start(args, fmt);

    std::vector<std::string> argv;

    for(int i = 0; fmt[i] != '\0'; i++){

      if (fmt[i] == 's'){
        char * str = va_arg(args, char *);
        argv.push_back( str );
      }
      else if (fmt[i] == 'i'){
        int j = va_arg(args, int);
        argv.push_back(std::to_string(j));
      }


    }

    va_end(args);

    if (argv[0][0] == '+'){
      publishNotification(argv);
    } else {
      respond(argv);
    }
  }

  void AbstractControl::publishNotification(std::vector<std::string> &argv){
    respond(argv);
  }


  // void AbstractControl::deviceDiscovered(AbstractPort * port){
  //   // PortDescriptor * desc = port->getPortDescriptor();
  //   // respond("sis", "+ports", port->getId(), desc->toString().c_str());
  //   // delete desc;
  // }

  void AbstractControl::deviceStateChanged(AbstractPort * port, AbstractPort::DeviceState_t newState){
    if (OptNotificationsEnabled == false){
      return;
    }
    int connected = newState == AbstractPort::DeviceStateConnected ? 1 : 0;
    if (OptReturnIds){
      respond("sii", "+devstate", port->getId(), connected);
    } else {
      respond("ssi", "+devstate", port->getKey().c_str(), connected);
    }
  }


  void AbstractControl::portRegistered( AbstractPort * port ){
    if (OptNotificationsEnabled == false){
      return;
    }
    PortDescriptor * desc = port->getPortDescriptor();
    respond("siis", "+ports", port->getId(), port->getType(),  desc->toString().c_str());
    delete desc;
  }

  void AbstractControl::portUnregistered( AbstractPort * port ){
    if (OptNotificationsEnabled == false){
      return;
    }
    PortDescriptor * desc = port->getPortDescriptor();
    respond("ssiis", "+ports", "-", port->getId(), port->getType(),  desc->toString().c_str());
    delete desc;
  }

  void AbstractControl::portsConnected( AbstractPort * inport, AbstractPort * outport ){
    if (OptNotificationsEnabled == false){
      return;
    }
    if (OptReturnIds){
      respond("siii","+constate",inport->getId(), outport->getId(), 1);
    } else {
      respond("sssi","+constate",inport->getKey().c_str(), outport->getKey().c_str(), 1);
    }
  }

  void AbstractControl::portsDisconnected( AbstractPort * inport, AbstractPort * outport ){
    if (OptNotificationsEnabled == false){
      return;
    }
    if (OptReturnIds){
      respond("siii","+constate",inport->getId(), outport->getId(), 0);
    } else {
      respond("sssi","+constate",inport->getKey().c_str(), outport->getKey().c_str(), 0);
    }
  }
}
