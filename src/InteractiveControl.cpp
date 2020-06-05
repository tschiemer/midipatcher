#include <InteractiveControl.hpp>

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

  InteractiveControl::InteractiveControl(PortRegistry * portRegistry) : InteractiveControl(portRegistry, std::cin, std::cout){
  }

  InteractiveControl::InteractiveControl(PortRegistry * portRegistry, std::istream &in, std::ostream &out) : AbstractControl(portRegistry), In(in), Out(out) {
  }

  bool InteractiveControl::runloop(){

    static bool once = true;

    if (once){
      Out << "Ahoi landlubber!" << std::endl;
      once = false;
    }

    std::string line;

    int c = In.peek();

    if (c == EOF){
      return true;
    }

    std::getline(In,line);

    // if (getline_async(In,line) == false){
    //   return;
    // }

    trim(line);

    std::vector<std::string> argv;

    while(line.size() > 0){

      size_t pos;

      do {
        pos = line.find(" ");
      } while( pos != std::string::npos && (pos > 0 && line[pos-1] == '\\'));

      if (pos == std::string::npos){
        argv.push_back(line);
        line.clear();
      } else {
        argv.push_back(line.substr(0,pos));
        line.erase(0, pos+1);

        trim(line);
      }
    }

    if (argv.size() == 1 && (argv[0] == "bye" || argv[0] == "quit" || argv[0] == "exit" || argv[0] == "x")){
      Out << "Good bye!" << std::endl;
      return false;
    }



    handleCommand(argv);

    return true;
  }

  void InteractiveControl::respond(std::vector<std::string> &argv){
    if (argv.size() == 0){
      Out << "* Empty response" << std::endl;
      return;
    }

    Out << argv[0];

    for(int i = 1; i < argv.size(); i++){
      Out << " " << argv[i];
    }

    Out << std::endl;
  }

}
