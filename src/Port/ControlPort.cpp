#include <Port/ControlPort.hpp>

#include <cassert>

namespace MidiPatcher {

  namespace Port {

    static const constexpr unsigned char MessageHeader[] = {0xF0, 0x7D, 0x01, 0x03, 0x03, 0x07};

    static const constexpr unsigned char MessageTail[] = {0xF7};

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

      std::vector<std::string> argv;

      unpackArguments(argv, data, dataLen);

      if (argv.size() == 0){
        return;
      }

      // for(int i = 0; i < argv.size(); i++){
      //   std::cout << "arg[" << i << "] = " << argv[i] << std::endl;
      // }

      handleCommand(argv);
    }

    void ControlPort::handleCommand(std::vector<std::string> &argv){


      if (argv[0] == "ping"){
        send(1,"pong");
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

    void ControlPort::send(int argc, ...){
      assert( 0 < argc && argc < 128 );

      va_list args;
      va_start(args, argc);

      std::vector<std::string> argv;

      for(;argc > 0; argc--){

        char * str = va_arg(args, char *);
        argv.push_back( str );

      }

      va_end(args);

      send(argv);
    }


  }

}
