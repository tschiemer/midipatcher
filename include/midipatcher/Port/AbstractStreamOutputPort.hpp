#ifndef MIDIPATCHER_PORT_ABSTRACT_STREAM_OUTPUT_PORT_H
#define MIDIPATCHER_PORT_ABSTRACT_STREAM_OUTPUT_PORT_H

#include "AbstractOutputPort.hpp"

namespace MidiPatcher {

  class AbstractStreamOutputPort : public virtual AbstractOutputPort {

    protected:

      AbstractStreamOutputPort(bool runningStatusEnabled = false);

      bool OutRunningStatusEnabled;
      uint8_t OutRunningStatusState;

      virtual void writeToStream(unsigned char * data, size_t len) = 0;

    public :


      virtual bool hasOption(std::string key){
        if (key == "runningstatus") return true;
        return false;
      }

      virtual std::string getOption(std::string key);

      virtual void setOption(std::string key, std::string value);

      inline bool getOutRunningStatusEnabled(){
        return OutRunningStatusEnabled;
      }

      void setOutRunningStatusEnabled(bool enabled);

    protected:

      virtual void sendMessageImpl(unsigned char * message, size_t len);
  };

}

#endif /* MIDIPATCHER_PORT_ABSTRACT_STREAM_OUTPUT_PORT_H */
