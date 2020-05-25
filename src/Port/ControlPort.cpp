#include <Port/ControlPort.hpp>

#include <PortRegistry.hpp>
#include <version.h>
#include <Log.hpp>
#include <Error.hpp>

#include <cassert>
#include <future>

namespace MidiPatcher {

  namespace Port {

    static const constexpr unsigned char MessageHeader[] = {0xF0, 0x7D, 0x01, 0x03, 0x03, 0x07};

    static const constexpr unsigned char MessageTail[] = {0xF7};

    static const constexpr size_t MaxLen = 128 - (sizeof(MessageHeader) + 2 + sizeof(MessageTail));

    ControlPort::ControlPort(PortRegistry * portRegistry, std::string portName) : AbstractControl(portRegistry), AbstractInputOutputPort(portName){
      setDeviceState(DeviceStateConnected);
    }

    uint8_t ControlPort::packMidiMessage(unsigned char * midi, unsigned char * data, uint8_t dataLen, uint8_t remainingMessages){
      assert( midi != nullptr );
      assert( data != nullptr );
      assert( dataLen <= 0x7F );
      assert( dataLen <= sizeof(MessageHeader) + sizeof(MessageTail) + 2);
      assert( remainingMessages <= 0x7F );

      uint8_t len = 0;

      // header (sysex experimental and marker bytes)
      std::memcpy(midi, (uint8_t*)MessageHeader, sizeof(MessageHeader));
      len += sizeof(MessageHeader);

      midi[len++] = dataLen;
      midi[len++] = remainingMessages;

      for(int i = 0; i < dataLen; i++){
        midi[len++] = (data[i] >> 4) & 0x0F;
        midi[len++] = (data[i] & 0x0F);
      }

      std::memcpy(&midi[len], (uint8_t*)MessageTail, sizeof(MessageTail));
      len += sizeof(MessageTail);

      return len;
    }

    uint8_t ControlPort::unpackMidiMessage(unsigned char * data, unsigned char * midi, uint8_t midiLen, uint8_t &remainingMessages){
      assert( midi != nullptr );
      assert( data != nullptr );

      // std::cout << (int)midiLen << " " << sizeof(MessageHeader) << " " << sizeof(MessageTail) << std::endl;

      // basic validation
      if (midiLen < (sizeof(MessageHeader) + 2 + sizeof(MessageTail))){
        // std::cerr << "l0 "  << std::endl;
        return 0;
      }

      uint8_t dataLen = midi[sizeof(MessageHeader)];
      remainingMessages = midi[sizeof(MessageHeader)+1];

      // std::cout << (int)len << std::endl;
      // header dataLen(1) remainingMessages(1) data(2*dataLen) tail
      if (midiLen != sizeof(MessageHeader) + 2 + 2*dataLen + sizeof(MessageTail) ){
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

      for(int d = 0, m = sizeof(MessageHeader) + 2; d < len; d++){
        data[d] = midi[m++] << 4;
        data[d] |= midi[m++] & 0x0F;
        // std::cout << std::hex << (int)data[d] << " ";
      }
      // std::cout << std::endl;

      return len;
    }

    void ControlPort::packArguments(std::vector<uint8_t> &data, std::vector<std::string> &argv){

      assert( argv.size() <= 0x7F ); // too many arguments (128??)

      int len = 0;

      data[len++] = argv.size();

      for(std::vector<std::string>::iterator it = argv.begin(); it != argv.end(); it++){
        data += *it + '\0';
      }
    }

    void ControlPort::unpackArguments(std::vector<std::string> &argv, std::vector<uint8_t> &data){

      int pos = 0;

      int argc = data[pos++];

      for(int i = 0; i < argc && pos < data.size(); i++, pos++){

        std::string arg = "";

        for(; data[pos] != '\0' && pos < data.size(); pos++){
          arg += data[pos];
        }

        argv.push_back(arg);
      }
    }

    // uint8_t ControlPort::packMessage(unsigned char * midi, std::vector<std::string> &argv){
    //     assert( midi != nullptr );
    //
    //     unsigned char tmp[128];
    //
    //     int len = packArguments( tmp, argv );
    //
    //     // std::cout << "data(" << len << ") ";
    //     // for(int i = 0; i < len; i++){
    //     //   std::cout << std::hex << (int)tmp[i] << " ";
    //     // }
    //     // std::cout << std::endl;
    //
    //     return packMidiMessage( midi, tmp, len );
    // }
    //
    // void ControlPort::unpackMessage(std::vector<std::string> &argv, unsigned char * midi, uint8_t midiLen){
    //     assert( midi != nullptr );
    //
    //     unsigned char tmp[128];
    //
    //     int len = unpackMidiMessage(tmp, midi, midiLen);
    //
    //     unpackArguments(argv, tmp, len);
    // }


    void ControlPort::packMessageParts(std::vector<std::string> &parts, std::vector<std::string> &argv){

      std::string data;

      packArguments( data, argv );

      while (data.size() > 0){
        if (data.size() > MaxLen){
          parts.push_back( data.substr(0, MaxLen) );
          data.erase(0, MaxLen);
        } else {
          parts.push_back(data);
          data.clear();
        }
      }

    }

    void ControlPort::unpackMessageParts(std::vector<std::string> &parts, std::vector<std::string> &argv){

      std::string data;

      for(std::vector<std::string>::iterator it = parts.begin(); it != parts.end(); it++){
        data += *it;
      }
    }
    //
    // uint8_t ControlPort::requiredMessages(std::vector<std::string> &argv){
    //
    //   // all arguments are null-terminated
    //   size_t len = argv.size();
    //
    //   for(std::vector<std::string>::iterator it = argv.begin(); it != argv.end(); it++){
    //     len += it->size();
    //   }
    //
    //   // length change according to transfer format (nibblization!)
    //   len *= 2;
    //
    //   return len / MaxLen;
    // }
    //
    // uint8_t ControlPort::requiredMessage(std::string &packedArgv){
    //     return packedArgv.size() / MaxLen;
    // }

    void ControlPort::sendMessageImpl(unsigned char * message, size_t len){
    //
    //   Log::warning(getKey(), "called sendMessageImpl but standard caller (sendMessage) overriden");
    // }
    //
    // void ControlPort::sendMessage(unsigned char * message, size_t len){

      // std::cout << "rx (" << len << ") ";
      // for(int i = 0; i < len; i++){
      //   std::cout << std::hex << (int)message[i] << " ";
      // }
      // std::cout << std::endl;

      uint8_t data[128];

      if (len > 128){
        Log::warning(getKey(), "received msg too big", data, len );
        return;
      }

      uint8_t remainingMessages = 0;
      int dataLen = unpackMidiMessage(data, message, len, &remainingMessages);

      if (dataLen == 0){
        InBundle.clear();
        return;
      }

      std::string msg = "";

      for(int i = 0; i < dataLen; i++){
        msg[i] = data[i];
      }

      if (remainingMessages > 0){
        InBundle
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


    void ControlPort::respond(std::vector<std::string> &argv){
      if (getDeviceState() == AbstractPort::DeviceStateNotConnected){
        return;
      }

      unsigned char midi[128];

      int len = packMessage( midi, argv );

      assert( len > 0 );

      // std::cout << "cp.send (" << len << ") ";
      //
      // for(int i = 0; i < len; i++){
      //   std::cout << std::hex << (int)midi[i] << " ";
      // }
      // std::cout << std::endl;

      receivedMessage( midi, len );
    }

  }

}
