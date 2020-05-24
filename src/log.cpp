#include <Log.hpp>

// #include <iostream>

namespace MidiPatcher {

  std::vector<Log::Logger> Log::LoggerList;

  void Log::registerLogger(Logger logger){
    LoggerList.push_back(logger);
  }

  void Log::log( Level_t level, std::string message ){
    for(std::vector<Logger>::iterator it = LoggerList.begin(); it != LoggerList.end(); it++){
      (*it)(level, message);
    }
  }

}
