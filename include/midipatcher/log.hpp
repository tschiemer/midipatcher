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

    typedef void (*Logger)(Level_t level, std::string message);

    protected:

      static std::vector<Logger> LoggerList;

    public:

      static void registerLogger(Logger logger);

      static void log( Level_t level, std::string message );

      static inline void notice( std::string message ){
        log(NOTICE, message);
      }

      static inline void notice( std::string context, std::string message ){
        log(NOTICE, context + ": " + message);
      }

      static inline void info( std::string message ){
        log(INFO, message);
      }

      static inline void info( std::string context, std::string message ){
        log(NOTICE, context + ": " + message);
      }

      static inline void debug( std::string message ){
        log(DEBUG, message);
      }

      static inline void debug( std::string context, std::string message ){
        log(DEBUG, context + ": " + message);
      }

      static inline void warning( std::string message ){
        log(WARNING, message);
      }

      static inline void warning( std::string context, std::string message ){
        log(WARNING, context + ": " + message);
      }

      static inline void error( std::string message ){
        log(ERROR, message);
      }

      static inline void error( std::string context, std::string message ){
        log(ERROR, context + ": " + message);
      }

      static inline void error( std::exception &e ){
        error(e.what());
      }

  };

}

#endif /* MIDIPATCHER_DEBUG_H */
