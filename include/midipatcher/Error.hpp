#ifndef MIDIPATCHER_ERROR_H
#define MIDIPATCHER_ERROR_H

#include <exception>

namespace MidiPatcher {

  class Error : virtual public std::exception {

    protected:

      std::string Message;

    public:

      Error(std::string message){
        Message = message;
      }

      Error(std::string context, std::string message){
        Message = context + ": " + message;
      }

      Error(const std::exception &e){
        Message = e.what();
      }

      const char* what() const noexcept {
        return Message.c_str();
      }

  };

}

#endif /* MIDIPATCHER_ERROR_H */
