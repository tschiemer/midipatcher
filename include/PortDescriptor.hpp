#ifndef PORT_DESCRIPTOR_H
#define PORT_DESCRIPTOR_H

#include <string>
#include <map>

namespace MidiPatcher {

  struct PortDescriptor {

    std::string Key;
    std::string Name;
    std::map<std::string,std::string> Options;

    PortDescriptor(std::string key, std::string name, std::map<std::string,std::string> options = std::map<std::string, std::string>() ){
      Key = key;
      Name = name;
      Options = options;
    }

    PortDescriptor(std::string str);

    static PortDescriptor * fromString(std::string str){
      return new PortDescriptor(str);
    }

    std::string toString();
  };

}

#endif /* PORT_DESCRIPTOR_H */
