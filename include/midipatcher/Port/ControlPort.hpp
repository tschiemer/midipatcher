#ifndef MIDIPATCHER_PORT_CONTROL_PORT_H
#define MIDIPATCHER_PORT_CONTROL_PORT_H

#include "../PortRegistry.hpp"
#include "../AbstractControl.hpp"
#include "AbstractInputOutputPort.hpp"


#include <cassert>
#include <vector>

namespace MidiPatcher {

  namespace Port {

    class ControlPort : public virtual AbstractControl, public virtual AbstractInputOutputPort {

      public:

        class Message : public virtual AbstractPort::Message {

          protected:

            uint8_t Missing = 1;

            std::vector<uint8_t> Data;

            std::vector<std::string> Argv;

          public:

            Message(){}

            Message(int argc, char * argv[]){
              Argv.insert(Argv.end(), argv, &argv[argc]);
            }

            Message(std::vector<std::string> &argv){
              Argv = argv;
            }

            uint8_t getMissing(){
              return Missing;
            }

            std::vector<std::string> & getArgv(){
              return Argv;
            }

            std::string toString();

            bool complete();
            bool empty();
            void clear();

            bool receivedPart(unsigned char * midi, uint8_t midiLen);

          protected:

            void transmit(AbstractPort * port, void (*tx)(AbstractPort * port, unsigned char * message, size_t len));

          public:

            static uint8_t packMidiMessage(unsigned char * midi, unsigned char * data, uint8_t dataLen, uint8_t remainingMessages = 0);
            static uint8_t unpackMidiMessage(unsigned char * data, unsigned char * midi, uint8_t midiLen, uint8_t &remainingMessages);

            static void packArguments(std::vector<uint8_t> &data, std::vector<std::string> &argv);
            static void unpackArguments(std::vector<std::string> &argv, std::vector<uint8_t> &data);

          };


      public:

        static const constexpr char * PortClass = "ControlPort";

        std::string getPortClass(){
          return PortClass;
        }

        PortDescriptor * getPortDescriptor(){
          return new PortDescriptor(PortClass, Name);
        }

        ControlPort(PortRegistry * portRegistry, std::string portName = "Default");

        // void sendMessage(unsigned char * message, size_t len);

      protected:

        Message InMessage;

        void sendMessageImpl(unsigned char * message, size_t len);

      protected:

        void respond(std::vector<std::string> &argv);

    };

  }

}

#endif /* MIDIPATCHER_PORT_CONTROL_PORT_H */
