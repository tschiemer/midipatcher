#ifndef MIDIPATCHER_DEBUG_H
#define MIDIPATCHER_DEBUG_H

// #include <ostream>

#include <string>
#include <vector>
#include <exception>

namespace MidiPatcher {

  class Log {

    public:

      enum Level_t {
        NOTICE,
        INFO,
        DEBUG,
        WARNING,
        ERROR
      };

    typedef void (*Logger)(Level_t level, std::string message, unsigned char * bytes, size_t len);

    protected:

      static std::vector<Logger> LoggerList;

    public:

      static void registerLogger(Logger logger);

      static void log( Level_t level, std::string message, unsigned char * bytes = nullptr, size_t len = 0 );

      static inline void notice( std::string message, unsigned char * bytes = nullptr, size_t len = 0  ){
        log(NOTICE, message, bytes, len);
      }

      static inline void notice( std::string context, std::string message, unsigned char * bytes = nullptr, size_t len = 0  ){
        log(NOTICE, context + " " + message, bytes, len);
      }

      static inline void info( std::string message, unsigned char * bytes = nullptr, size_t len = 0  ){
        log(INFO, message, bytes, len);
      }

      static inline void info( std::string context, std::string message, unsigned char * bytes = nullptr, size_t len = 0  ){
        log(NOTICE, context + " " + message, bytes, len);
      }

      static inline void debug( std::string message, unsigned char * bytes = nullptr, size_t len = 0  ){
        log(DEBUG, message, bytes, len);
      }

      static inline void debug( std::string context, std::string message, unsigned char * bytes = nullptr, size_t len = 0 ){
        log(DEBUG, context + " " + message, bytes, len);
      }

      static inline void warning( std::string message, unsigned char * bytes = nullptr, size_t len = 0  ){
        log(WARNING, message, bytes, len);
      }

      static inline void warning( std::string context, std::string message, unsigned char * bytes = nullptr, size_t len = 0  ){
        log(WARNING, context + " " + message, bytes, len);
      }

      static inline void warning( std::exception &e ){
        log(WARNING, e.what());
      }

      static inline void error( std::string message, unsigned char * bytes = nullptr, size_t len = 0  ){
        log(ERROR, message, bytes, len);
      }

      static inline void error( std::string context, std::string message, unsigned char * bytes = nullptr, size_t len = 0  ){
        log(ERROR, context + " " + message, bytes, len);
      }

      static inline void error( std::exception &e ){
        error(e.what());
      }

  };

}

#endif /* MIDIPATCHER_DEBUG_H */
