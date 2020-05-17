#include <Port/ControlPort.hpp>

#include <PortRegistry.hpp>
#include <version.h>

#include <cassert>
#include <future>

namespace MidiPatcher {

  namespace Port {

    static const constexpr unsigned char MessageHeader[] = {0xF0, 0x7D, 0x01, 0x03, 0x03, 0x07};

    static const constexpr unsigned char MessageTail[] = {0xF7};


    ControlPort::ControlPort(PortRegistry * portRegistry) : AbstractInputOutputPort("ControlPort"){
      assert(portRegistry != nullptr);

      PortRegistryRef = portRegistry;

      PortRegistryRef->subscribePortUpdateReveicer( this );
    }

    uint8_t ControlPort::packMidiMessage(unsigned char * midi, unsigned char * data, uint8_t dataLen){
      assert( midi != nullptr );
      assert( data != nullptr );
      assert( dataLen <= 0x7F );

      uint8_t len = 0;

      // header (sysex experimental and marker bytes)
      std::memcpy(midi, (uint8_t*)MessageHeader, sizeof(MessageHeader));
      len += sizeof(MessageHeader);

      midi[len++] = dataLen;

      for(int i = 0; i < dataLen; i++){
        midi[len++] = (data[i] >> 4) & 0x0F;
        midi[len++] = (data[i] & 0x0F);
      }

      std::memcpy(&midi[len], (uint8_t*)MessageTail, sizeof(MessageTail));
      len += sizeof(MessageTail);

      return len;
    }

    uint8_t ControlPort::unpackMidiMessage(unsigned char * data, unsigned char * midi, uint8_t midiLen){
      assert( midi != nullptr );
      assert( data != nullptr );

      // std::cout << (int)midiLen << " " << sizeof(MessageHeader) << " " << sizeof(MessageTail) << std::endl;

      // basic validation
      if (midiLen < (sizeof(MessageHeader) + sizeof(MessageTail))){
        // std::cerr << "l0 "  << std::endl;
        return 0;
      }

      uint8_t len = midi[sizeof(MessageHeader)];

      // std::cout << (int)len << std::endl;

      if (midiLen != sizeof(MessageHeader) + 1 + 2*len + sizeof(MessageTail) ){
        // std::cerr << "l1" << std::endl;
        return 0;
      }

      if (std::memcmp(midi, (uint8_t*)MessageHeader, sizeof(MessageHeader)) != 0){
        // std::cerr << "l2" << std::endl;
        return 0;
      }
      if (std::memcmp(&midi[midiLen - sizeof(MessageTail)], (uint8_t*)MessageTail, sizeof(MessageTail)) != 0){
        // std::cerr << "l3" << std::endl;
        return 0;
      }

      for(int d = 0, m = sizeof(MessageHeader) + 1; d < len; d++){
        data[d] = midi[m++] << 4;
        data[d] |= midi[m++] & 0x0F;
        // std::cout << std::hex << (int)data[d] << " ";
      }
      // std::cout << std::endl;

      return len;
    }

    uint8_t ControlPort::packArguments(unsigned char * data, std::vector<std::string> &argv){
        assert(data != nullptr);

        int len = 0;

        data[len++] = argv.size();

        for(std::vector<std::string>::iterator it = argv.begin(); it != argv.end(); it++){
          std::memcpy( &data[len], it->c_str(), it->size() );
          len += it->size();
          data[len++] = '\0';
        }

        return len;
    }

    void ControlPort::unpackArguments(std::vector<std::string> &argv, unsigned char * data, uint8_t dataLen){
      assert(data != nullptr);

      int pos = 0;

      int argc = data[pos++];

      for(int i = 0, l = 0; i < argc; i++){
        char tmp[128];

        // std::cout << "a(" << i << ") ";
        for(l = 0; data[pos] != '\0'; l++, pos++){
          tmp[l] = data[pos];
          // std::cout << tmp[l] << " ";
        }
        // std::cout << "(len = " << l << ")" << std::endl;

        tmp[l] = '\0';
        pos++;

        // std::cout << tmp << std::endl;



        argv.push_back(std::string(tmp));
      }
    }

    uint8_t ControlPort::packMessage(unsigned char * midi, std::vector<std::string> &argv){
        assert( midi != nullptr );

        unsigned char tmp[128];

        int len = packArguments( tmp, argv );

        // std::cout << "data(" << len << ") ";
        // for(int i = 0; i < len; i++){
        //   std::cout << std::hex << (int)tmp[i] << " ";
        // }
        // std::cout << std::endl;

        return packMidiMessage( midi, tmp, len );
    }

    void ControlPort::unpackMessage(std::vector<std::string> &argv, unsigned char * midi, uint8_t midiLen){
        assert( midi != nullptr );

        unsigned char tmp[128];

        int len = unpackMidiMessage(tmp, midi, midiLen);

        unpackArguments(argv, tmp, len);
    }

    void ControlPort::sendMessage(unsigned char * message, size_t len){

      // std::cout << "rx (" << len << ") ";
      // for(int i = 0; i < len; i++){
      //   std::cout << std::hex << (int)message[i] << " ";
      // }
      // std::cout << std::endl;

      uint8_t data[128];
      int dataLen = unpackMidiMessage(data, message, len);

      if (dataLen == 0){
        return;
      }

      // std::cout << "received control port message (" << dataLen << ") " << std::endl;

      std::vector<std::string> * argv = new std::vector<std::string>();

      unpackArguments(*argv, data, dataLen);

      if (argv->size() == 0){
        return;
      }

      // for(int i = 0; i < argv.size(); i++){
      //   std::cout << "arg[" << i << "] = " << argv[i] << std::endl;
      // }

      new std::thread([this, argv](){
        handleCommand(*argv);
        delete argv;
      });

    }

    AbstractPort * ControlPort::getPortByIdOrKey( std::string &idOrKey ){

      if (std::all_of(idOrKey.begin(), idOrKey.end(), ::isdigit)){
        return PortRegistryRef->getPortById( std::stoi(idOrKey) );
      }

      return PortRegistryRef->getPortByKey( idOrKey );
    }

    void ControlPort::handleCommand(std::vector<std::string> &argv){

      std::cout << "(" << argv.size() << ") ";
      for(int i = 0; i < argv.size(); i++){
        std::cout << argv[i] << " ";
      }
      std::cout << std::endl;


      if (argv[0] == "ping"){
        send("s","pong");
        return ok();
      }

      if (argv[0] == "version"){
        send("ss", "version", VERSION.c_str());
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
          send("ssi", "option", "return-ids", OptReturnIds ? 1 : 0);
          return ok();
        }

        return error("Unknown option: " + argv[1]);
      }

      if (argv[0] == "portclasses"){
        if (argv.size() > 1){
          return error("Expected: portclasses");
        }

        std::vector<MidiPatcher::AbstractPort::PortClassRegistryInfo*> * classes = PortRegistryRef->getPortClassRegistryInfoList();

        send("si", "classes", classes->size());

        for(int i = 0; i < classes->size(); i++){
          send("ss", "classes", classes->at(i)->Key.c_str() );
        }

        delete classes;

        return ok();
      }

      if (argv[0] == "ports"){
        if (argv.size() > 3){
          return error("Expected: ports [<port-id>|<port-key>]");
        }
        if (argv.size() == 2){

          AbstractPort * port = getPortByIdOrKey(argv[1]);

          if (port == nullptr){
            return error("No such port: " + argv[1]);
          }

          send("sis", "ports", port->getId(), port->getKey().c_str());

          return ok();

        } else {

          std::vector<AbstractPort*> * ports = PortRegistryRef->getAllPorts();

          send("si", "ports", ports->size());

          for(int i = 0; i < ports->size(); i++){
            AbstractPort * port = ports->at(i);
            send("sis","ports", port->getId(), port->getKey().c_str());
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


          int connected = port->getDeviceState() == DeviceStateConnected ? 1 : 0;

          if (OptReturnIds){
            send("sii", "devstate", port->getId(), connected  );
          } else {
            send("ssi", "devstate", port->getKey().c_str(), connected  );
          }

          return ok();

        } else {
          std::vector<AbstractPort*> * ports = PortRegistryRef->getAllPorts();

          send("si", "devstate", ports->size() );

          for(int i = 0; i < ports->size(); i++){
            AbstractPort * port = ports->at(i);
            int connected = port->getDeviceState() == DeviceStateConnected ? 1 : 0;
            if (OptReturnIds){
              send("sii", "devstate", port->getId(), connected );
            } else {
              send("ssi", "devstate", port->getKey().c_str(), connected );
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
        } catch(const char * e){
          return error(e);
        }

        AbstractPort * port = PortRegistryRef->getPortByKey( desc->getKey() );

        if (port != nullptr){
          error("Already registered key: " + desc->getKey());
        } else {

          try {
            port = PortRegistryRef->registerPortFromDescriptor( desc );

            send("sis", "ports", port->getId(), port->getKey().c_str());

            ok();

          } catch (std::exception &e) {

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

        if (OptReturnIds){
          send("si", "unregister", port->getId());
        } else {
          send("ss", "unregister", port->getKey().c_str());
        }

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
            send("siii", "constate", inport->getId(), outport->getId(), constate );
          } else {
            send("sssi", "constate", argv[1].c_str(), argv[2].c_str(), constate );
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
              send("siii", "constate", port->getId(), other->getId(), constate );
            } else {
              send("sssi", "constate", argv[1].c_str(), other->getKey().c_str(), constate );
            }
          }

          delete connections;

          return ok();
        }
        else {

            std::vector<AbstractPort*> * ports = PortRegistryRef->getAllPorts();

            // send("si", "constate", ports->size() );

            for(int i = 0; i < ports->size(); i++){
              AbstractPort * port = ports->at(i);
              AbstractInputPort * inport = dynamic_cast<AbstractInputPort*>(port);

              if (inport != nullptr){

                std::vector<AbstractPort*> * connections = port->getConnections();

                for(int j = 0; j < connections->size(); j++){
                  AbstractPort * other = connections->at(j);

                  int constate = port->isConnectedTo(other);

                  if (OptReturnIds){
                    send("siii", "constate", port->getId(), other->getId(), constate );
                  } else {
                    send("sssi", "constate", port->getKey().c_str(), other->getKey().c_str(), constate );
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

        AbstractPort * inport = getPortByIdOrKey(argv[1]);
        AbstractPort * outport = getPortByIdOrKey(argv[2]);

        if (inport == nullptr){
          return error("No such port: " + argv[1]);
        }
        if (outport == nullptr){
          return error("No such port: " + argv[2]);
        }

        if (!inport->isConnectedTo(outport)){
          PortRegistryRef->connectPorts(inport, outport);
        }

        if (OptReturnIds){
          send("siii", "constate", inport->getId(), outport->getId(), 1);
        } else {
          send("sssi", "constate", argv[1].c_str(), argv[2].c_str(), 1);
        }

        return ok();
      }

      if (argv[0] == "disconnect"){
        if (argv.size() != 3){
          return error("Expected: disconnect (<in-port-id>|<in-port-key>) (<in-port-id>|<out-port-key>)");
        }

        AbstractPort * inport = getPortByIdOrKey(argv[1]);
        AbstractPort * outport = getPortByIdOrKey(argv[2]);

        if (inport == nullptr){
          return error("No such port: " + argv[1]);
        }
        if (outport == nullptr){
          return error("No such port: " + argv[2]);
        }

        if (inport->isConnectedTo(outport)){
          PortRegistryRef->disconnectPorts(inport, outport);
        }

        if (OptReturnIds){
          send("sssi", "constate", inport->getId(), outport->getId(), 0);
        } else {
          send("sssi", "constate", argv[1].c_str(), argv[2].c_str(), 0);
        }

        return ok();
      }

      error("Unknown command");
    }

    void ControlPort::ok(){
      std::vector<std::string> argv;
      argv.push_back("OK");
      send(argv);
    }

    void ControlPort::error(std::string msg){
      std::vector<std::string> argv;

      argv.push_back("ERROR");

      if (msg.size() > 0){
        argv.push_back(msg);
      }

      send(argv);
    }

    void ControlPort::send(std::vector<std::string> &argv){
      unsigned char midi[128];

      int len = MidiPatcher::Port::ControlPort::packMessage( midi, argv );

      assert( len > 0 );

      // std::cout << "cp.send (" << len << ") ";
      //
      // for(int i = 0; i < len; i++){
      //   std::cout << std::hex << (int)midi[i] << " ";
      // }
      // std::cout << std::endl;

      receivedMessage( midi, len );
    }

    void ControlPort::send(const char * fmt, ...){
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

      send(argv);
    }


    void ControlPort::deviceDiscovered(AbstractPort * port){
      send("sis", "+ports", port->getId(), port->getKey().c_str());
    }

    void ControlPort::deviceStateChanged(AbstractPort * port, DeviceState_t newState){
      int connected = newState == DeviceStateConnected ? 1 : 0;
      if (OptReturnIds){
        send("sii", "+devstate", port->getId(), connected);
      } else {
        send("ssi", "+devstate", port->getKey().c_str(), connected);
      }
    }

  }

}
