#ifndef PORT_DESCRIPTOR_H
#define PORT_DESCRIPTOR_H

#include <string>
#include <map>

namespace MidiPatcher {

  struct PortDescriptor {

    std::string PortClass;
    std::string Name;
    std::map<std::string,std::string> Options;

    PortDescriptor(std::string portClass, std::string name, std::map<std::string,std::string> options = std::map<std::string, std::string>() ){
      PortClass = portClass;
      Name = name;
      Options = options;
    }

    PortDescriptor(std::string str);

    static PortDescriptor * fromString(std::string str){
      return new PortDescriptor(str);
    }

    std::string toString();

    std::string getKey(){
      return PortClass + ":" + Name;
    }
  };

}

#endif /* PORT_DESCRIPTOR_H */
