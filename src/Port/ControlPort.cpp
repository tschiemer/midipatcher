#include <Port/ControlPort.hpp>

#include <PortRegistry.hpp>

#include <Log.hpp>
#include <Error.hpp>

#include <cassert>
#include <future>

namespace MidiPatcher {

  namespace Port {

    static const constexpr unsigned char MessageHeader[] = {0xF0, 0x7D, 0x01, 0x03, 0x03, 0x07};

    static const constexpr unsigned char MessageTail[] = {0xF7};

    static const constexpr size_t MaxMidiDataLen = 128 - (sizeof(MessageHeader) + 2 + sizeof(MessageTail));

    static const constexpr size_t MaxDataLen = MaxMidiDataLen / 2;



    std::string ControlPort::Message::toString(){
      if (Argv.size() == 0){
        return "";
      }
      std::string result = Argv[0];
      for(int i = 1; i < Argv.size(); i++ ){
        result += ' ' + Argv[i];
      }
      return result;
    }


    bool ControlPort::Message::complete(){
      return !Data.empty() && Missing == 0;
    }

    bool ControlPort::Message::empty(){
      return Data.empty() && Argv.empty();
    }

    void ControlPort::Message::clear(){
      Data.clear();
      Argv.clear();
      Missing = 1;
    }

    bool ControlPort::Message::receivedPart(unsigned char * midi, uint8_t midiLen){

      if (midiLen > 128){
        assert(false); // for now...
      }

      uint8_t remainingMessages = 0;
      uint8_t data[128];
      int dataLen = unpackMidiMessage(data, midi,  midiLen,  remainingMessages);

      if (dataLen == 0){
        return false;
      }

      if (empty() == false && Missing != remainingMessages + 1){
        clear();
      }

      Missing = remainingMessages;

      Data.insert( Data.end(), data, &data[dataLen] );

      if (Missing > 0){ // not complete
        return false;
      }

      unpackArguments(Argv, Data);

      return true; // same as "complete()"
    }

    void ControlPort::Message::transmit(AbstractPort * port, void (*tx)(AbstractPort * port, unsigned char * message, size_t len)){

      if (Argv.size() == 0){
        assert(false); // for now
        return;
      }

      Data.clear();

      packArguments(Data, Argv);

      assert( Data.size() > 0 );

      uint8_t requiredMessages = Data.size() / MaxDataLen + 1;

      // std::cerr << "requiredMessages " << (int)requiredMessages << std::endl;
      // std::cerr << "MaxDataLen " << std::dec << MaxDataLen << std::endl;

      for(; requiredMessages > 0; requiredMessages--){

        uint8_t dataLen;
        if (Data.size() > MaxDataLen){
          dataLen = MaxDataLen;
        } else {
          dataLen = Data.size();
        }

        uint8_t midi[128];
        uint8_t midiLen = packMidiMessage(midi, &Data[0], dataLen, requiredMessages - 1);


        // std::cerr << "midiLen " << std::dec << (int)midiLen << std::endl;

        tx(port, midi, midiLen);

        // std::cerr << "tx " << midiLen;
        // for(int i = 0; i < midiLen; i++){
        //   std::cerr << std::hex << (int)midi[i] << " ";
        // }
        // std::cerr << std::endl;

        Data.erase(Data.begin(), Data.begin() + dataLen);
      }

    }

    uint8_t ControlPort::Message::packMidiMessage(unsigned char * midi, unsigned char * data, uint8_t dataLen, uint8_t remainingMessages){
      assert( midi != nullptr );
      assert( data != nullptr );
      assert( dataLen <= MaxDataLen );
      // assert( dataLen >= sizeof(MessageHeader) + sizeof(MessageTail) + 2);
      assert( remainingMessages <= 0x7F );

      uint8_t midiLen = 0;

      // header (sysex experimental and marker bytes)
      std::memcpy(midi, (uint8_t*)MessageHeader, sizeof(MessageHeader));
      midiLen += sizeof(MessageHeader);

      midi[midiLen++] = dataLen;
      midi[midiLen++] = remainingMessages;

      for(int i = 0; i < dataLen; i++){
        midi[midiLen++] = (data[i] >> 4) & 0x0F;
        midi[midiLen++] = (data[i] & 0x0F);
      }

      std::memcpy(&midi[midiLen], (uint8_t*)MessageTail, sizeof(MessageTail));
      midiLen += sizeof(MessageTail);

      return midiLen;
    }

    uint8_t ControlPort::Message::unpackMidiMessage(unsigned char * data, unsigned char * midi, uint8_t midiLen, uint8_t &remainingMessages){
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

      for(int d = 0, m = sizeof(MessageHeader) + 2; d < midiLen; d++){
        data[d] = midi[m++] << 4;
        data[d] |= midi[m++] & 0x0F;
        // std::cout << std::hex << (int)data[d] << " ";
      }
      // std::cout << std::endl;

      return dataLen;
    }

    void ControlPort::Message::packArguments(std::vector<uint8_t> &data, std::vector<std::string> &argv){

      assert( argv.size() <= 0x7F ); // too many arguments (128??)

      data.push_back(argv.size());

      for(std::vector<std::string>::iterator it = argv.begin(); it != argv.end(); it++){
        data.insert(data.end(), it->begin(), it->end());
        data.push_back('\0');
      }
    }

    void ControlPort::Message::unpackArguments(std::vector<std::string> &argv, std::vector<uint8_t> &data){

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

    ControlPort::ControlPort(PortRegistry * portRegistry, std::string portName) : AbstractControl(portRegistry), AbstractInputOutputPort(portName){
      setDeviceState(DeviceStateConnected);
    }


    void ControlPort::sendMessageImpl(unsigned char * message, size_t len){

      if (len > 128){
        error("message > 128");
        return;
      }

      InMessage.receivedPart(message, (uint8_t)len);

      if (InMessage.complete() == false){
        return;
      }



      std::vector<std::string> * argv = new std::vector<std::string>(InMessage.getArgv());

      new std::thread([this, argv](){
        handleCommand(*argv);
        delete argv;
      });

    }


    void ControlPort::respond(std::vector<std::string> &argv){

      Message message(argv);

      message.sendFrom(this);

    }

  }

}
