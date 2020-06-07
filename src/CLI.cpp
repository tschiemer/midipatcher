#include <CLI.hpp>

namespace MidiPatcher {

  //https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
  // trim from start (in place)
  static inline std::string & ltrim(std::string &s) {
      s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
          return !std::isspace(ch);
      }));
      return s;
  }

  // trim from end (in place)
  static inline std::string & rtrim(std::string &s) {
      s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
          return !std::isspace(ch);
      }).base(), s.end());
      return s;
  }

  // trim from both ends (in place)
  static inline std::string & trim(std::string &s) {
      return ltrim(rtrim(s));
      // rtrim(s);
  }

 //
 //  CLI::CLI(std::string delimiter = " ") : CLI(std::cin, std::cout, delimiter) {
 //  }
 //
 //  CLI::CLI(std::istream &in, std::ostream &out, std::string delimiter = " ") : In(in), Out(out){
 //    assert( delimiter.size() > 0 );
 //
 //    Delimiter = delimiter;
 //  }
 //
 // std::string CLI::getArgvDelimiter(){
 //   return Delimiter;
 // }
 //
 // void CLI::setArgvDelimiter(std::string delimiter){
 //   assert( delimiter.size() > 0 );
 //   Delimiter = delimiter;
 // }

  void CLI::printHelp(){
    Out <<
    #include "CLI.help.in"
    << std::endl;
  }

  bool CLI::runloop(){

    static bool once = true;

    if (once){
      Out << "Started" << std::endl;
      once = false;
    }

    std::string line;

    int c = In.peek();

    if (c == EOF){
      return true;
    }

    std::getline(In,line);


    trim(line);

    std::vector<std::string> argv;

    while(line.size() > 0){

      size_t pos = line.find(Delimiter);

      if (pos == std::string::npos){
        argv.push_back(line);
        line.clear();
      } else {
        std::string arg = line.substr(0,pos);
        argv.push_back(arg);
        line.erase(0, pos + Delimiter.size());

        trim(line);
      }
    }

    if (argv.size() == 0){
      return true;
    }

    if (argv.size() == 1){
      if (argv[0] == "help" || argv[0] == "?" || argv[0] == "h"){
        printHelp();
        return true;
      }
      if (argv[0] == "bye" || argv[0] == "quit" || argv[0] == "exit" || argv[0] == "x"){
        Out << "Good bye!" << std::endl;
        return false;
      }
    }

    if (argv[0] == "delim-reset"){
      // reset if called without arguments

      std::vector<std::string> response;

      if (argv.size() != 1){
        response.push_back("ERROR");
        response.push_back("Expected: delim-reset");
      } else {
        setArgvDelimiter(" ");
        response.push_back("OK");
      }

      receivedResponse(response);

      return true;
    }
    if (argv[0] == "delim"){

      std::vector<std::string> response;

      if (argv.size() != 2){
        response.push_back("ERROR");
        response.push_back("Expected: delim<delimiter><new-delimiter>");
      } else {
        setArgvDelimiter(argv[1]);
        response.push_back("OK");
      }

      receivedResponse(response);

      return true;
    }

    sendCommand(argv);

    return true;
  }

  void CLI::receivedResponse(std::vector<std::string> &argv){
    if (argv.size() == 0){
      Out << "* Empty response" << std::endl;
      return;
    }

    Out << argv[0];

    for(int i = 1; i < argv.size(); i++){
      Out << Delimiter << argv[i];
    }

    Out << std::endl;
  }

}
