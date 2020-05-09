#include <PortDescriptor.hpp>

namespace MidiPatcher {

    PortDescriptor::PortDescriptor(std::string str){
      size_t pos = str.find(":");

      assert( pos != std::string::npos );

      Key = str.substr(0, pos);

      // assert( PortRegistry::getPortFamilyDeclarations()->count(Key) > 0 );

      str.erase(0,pos + 1);

      pos = str.find(",");

      if( pos == std::string::npos ){
        Name = str;
        str.erase(0,pos);
      } else {
        Name = str.substr(0,pos);
        str.erase(0, pos+1);
      }

      if (str.size() > 0){
        do {
          pos = str.find(",");
          std::string opt;
          if ( pos == std::string::npos ){
            opt = str.substr(0,pos);
            str.erase(0,pos);
          } else {
             opt = str.substr(0,pos);
             str.erase(0,pos+1);
          }

          // std::cout << "Opt " << opt << std::endl;

          pos = opt.find("=");
          std::string key, value;
          if (pos == std::string::npos){
            key = opt.substr(0, pos);
            opt.erase(0,pos);
            value = "1";
          }  else {
            key = opt.substr(0, pos);
            opt.erase(0,pos+1);
            value = opt.substr(0,opt.size());
          }

          Options[key] = value;

        } while(str.size() > 0);
      }

    }

    std::string PortDescriptor::toString(){
      std::string str = Key + ":" + Name;
      if (Options.size() > 0){
        std::for_each(Options.begin(), Options.end(), [&str](std::pair<std::string,std::string> opt){
          str += "," + opt.first;
          if (opt.second.size() > 0){
            str += "=" + opt.second;
          }
        });
      }
      return str;
    }

}
