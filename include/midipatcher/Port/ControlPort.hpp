#ifndef CONTROL_PORT_H
#define CONTROL_PORT_H

#include "../PortRegistry.hpp"
#include "../AbstractControl.hpp"
#include "AbstractInputOutputPort.hpp"


#include <cassert>
#include <vector>

namespace MidiPatcher {

  // class PortRegistry;
  // class PortRegistry::PortRegistryUpdateReceiver;

  namespace Port {

    class ControlPort : public virtual AbstractControl, public virtual AbstractInputOutputPort {

      public:

        static const constexpr char * PortClass = "ControlPort";

        std::string getPortClass(){
          return PortClass;
        }

        // std::string getKey(){
        //   return PortClass;
        // }

        PortDescriptor * getPortDescriptor(){
          return new PortDescriptor(PortClass, Name);
        }

        ControlPort(PortRegistry * portRegistry, std::string portName = "Default");

        // void sendMessage(unsigned char * message, size_t len);

        class Message {

          protected:

            uint8_t Missing;

            std::vector<uint8_t> Data;

            std::vector<std::string> Argv;

          public:

            Message(){

            }

            Message& fromArgv(std::vector<std::string> &argv){
              Message * msg = new Message();
              msg->Argv = argv;
              return *msg;
            }

            std::string toString(){
              if (Argv.size() == 0){
                return "\n";
              }
              std::string result = Argv[0];
              for(int i = 1; i < Argv.size(); i++ ){
                result += ' ' + Argv[i];
              }
              return result;
            }

            void sendTo(AbstractInputPort * port){

              if (Argv.size() == 0){
                assert(false); // for now
                return;
              }

              Data.clear();

              packArguments(Data, Argv);

              uint8_t requiredMessages = 0;

              for(; requiredMessages > 0; requiredMessages--){

                uint8_t * data = &Data[0];

                uint8_t dataLen;
                if (Data.size() > 123){
                  dataLen = 123;
                } else {
                  dataLen = Data.size();
                }

                uint8_t midi[128];
                uint8_t midiLen = packMidiMessage(midi, data, dataLen, requiredMessages - 1);

                port->receivedMessage(midi, midiLen);

                Data.erase(0,dataLen);
              }

            }

            bool receivedPart(unsigned char * midi, uint8_t midiLen){

              if (midiLen > 128){
                assert(false); // for now...
              }

              uint8_t remainingMessages = 0;
              unsigned char data[128];
              int dataLen = unpackMidiMessage(data, midi,  midiLen,  &remainingMessages);

              if (empty() == false && Missing != remainingMessages + 1){
                clear();
              }

              Missing = remainingMessages;

              Data.push_back( data, &data[dataLen] );

              if (Missing > 0){
                return false;
              }

              unpackArguments(Argv, Data);

              return true;
            }

            bool empty(){
              return Data.empty() && Argv.empty();
            }

            void clear(){
              Data.clear();
              Argv.clear();
            }

          };




        public:

          // header (sysex experimental and marker bytes)
          // static const constexpr unsigned char MessageHeader[] = {0xF0, 0x7D, 0x01, 0x03, 0x03, 0x07};

          // tail (end of sysex)
          // static const constexpr char * MessageTail = "\xF7";

          static uint8_t packMidiMessage(unsigned char * midi, unsigned char * data, uint8_t dataLen, uint8_t remainingMessages = 0);
          static uint8_t unpackMidiMessage(unsigned char * data, unsigned char * midi, uint8_t midiLen, uint8_t &remainingMessages);

          static void packArguments(std::vector<uint8_t> &data, std::vector<std::string> &argv);
          static void unpackArguments(std::vector<std::string> &argv, std::vector<uint8_t> &data);

          // static uint8_t packMessage(unsigned char * midi, std::vector<std::string> &argv);
          // static void unpackMessage(std::vector<std::string> &argv, unsigned char * midi, uint8_t midiLen);

          static void packMessageParts(std::vector<std::string> &parts, std::vector<std::string> &argv);
          static void unpackMessageParts(std::vector<std::string> &argv, std::vector<std::string> &parts);

          // static uint8_t requiredMessages(std::vector<std::string> &argv);
          // static uint8_t requiredMessage(std::string &packedArgv);

      protected:

        std::vector<std::string> InBundle;

        void sendMessageImpl(unsigned char * message, size_t len);

      protected:

        void respond(std::vector<std::string> &argv);


      //   AbstractPort * getPortByIdOrKey( std::string &idOrKey );
      //
      //   void handleCommand(std::vector<std::string> &argv);
      //
      //   void send(std::vector<std::string> &argv);
      //   void send(const char * fmt, ...);
      //
      //   void ok();
      //
      //   void error(std::string msg = "");
      //
      // public:
      //
      //   // void deviceDiscovered(AbstractPort * port);
      //   void deviceStateChanged(AbstractPort * port, DeviceState_t newState);
      //   void portRegistered( AbstractPort * port );
      //   void portUnregistered( AbstractPort * port );
      //   void portsConnected( AbstractPort * inport, AbstractPort * outport );
      //   void portsDisconnected( AbstractPort * inport, AbstractPort * outport );
    };

  }

}

#endif /* CONTROL_PORT_H */
