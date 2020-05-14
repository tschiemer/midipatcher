#ifndef ABSTRACT_STREAM_OUTPUT_PORT_H
#define ABSTRACT_STREAM_OUTPUT_PORT_H

#include "AbstractOutputPort.hpp"

namespace MidiPatcher {

  class AbstractStreamOutputPort : public virtual AbstractOutputPort {

    protected:

      AbstractStreamOutputPort(bool runningStatusEnabled = false);

      bool RunningStatusEnabled;
      uint8_t RunningStatusState;

      virtual void writeToStream(unsigned char * data, size_t len) = 0;

    public :

      bool getRunningStatusEnabled(){
        return RunningStatusEnabled;
      }

      void setRunningStatusEnabled(bool enabled);

      void sendMessage(unsigned char * message, size_t len);
  };

}

#endif /* ABSTRACT_STREAM_OUTPUT_PORT_H */
