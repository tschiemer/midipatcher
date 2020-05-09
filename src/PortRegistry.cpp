#include <PortRegistry.hpp>

#include <AbstractPort.hpp>
#include <Port/MidiIn.hpp>
#include <Port/MidiOut.hpp>

#include <cassert>
#include <utility>

#include <iostream>


namespace MidiPatcher {

  PortRegistry::PortRegistry(std::vector<AbstractPort::PortClassRegistryInfo *> &pcriList){

      for (int i = 0; i < pcriList.size(); i++){
         PortClassRegistryInfoMap[pcriList[i]->Key] = pcriList[i];
      }
  }

  PortRegistry::~PortRegistry(){
    // disableAutoscan();

    // properly delete all ports
    std::for_each(Ports.begin(), Ports.end(), [](AbstractPort * port){
      //
      std::vector<AbstractPort *> * connections = port->getConnections();

      std::for_each(connections->begin(), connections->end(), [port](AbstractPort*other){
        port->removeConnection(other);
        other->removeConnection(port);
      });

      delete port;
    });

    std::for_each(PortClassRegistryInfoMap.begin(),PortClassRegistryInfoMap.end(), [](std::pair<std::string,AbstractPort::PortClassRegistryInfo *> pair){
      delete pair.second;
    });
  }


  void PortRegistry::init(){

    for (std::map<std::string, AbstractPort::PortClassRegistryInfo*>::iterator it = PortClassRegistryInfoMap.begin(); it != PortClassRegistryInfoMap.end(); ++it)
    {
      if (it->second->Init != NULL){
        it->second->Init();
      }
    }
  }

  void PortRegistry::deinit(){

    for (std::map<std::string, AbstractPort::PortClassRegistryInfo*>::iterator it = PortClassRegistryInfoMap.begin(); it != PortClassRegistryInfoMap.end(); ++it)
    {
      if (it->second->Deinit != NULL){
        it->second->Deinit();
      }
    }

  }

  void PortRegistry::rescan(){

    for (std::map<std::string, AbstractPort::PortClassRegistryInfo*>::iterator it = PortClassRegistryInfoMap.begin(); it != PortClassRegistryInfoMap.end(); ++it)
    {
      // std::cout << "Scanning " << it->first << std::endl;
      if (it->second->Scanner != NULL){
        it->second->Scanner(this);
      }
    }

  }

  AbstractPort * PortRegistry::getPortById( unsigned int id ){
    AbstractPort * result = NULL;

    // assuming we never delete Ports this works...
    result = Ports.at(id - 1);

    return result;
  }

  AbstractPort * PortRegistry::findPortByName( std::string needle, AbstractPort::Type_t portType){
    AbstractPort * result = NULL;

    for(int i = 0; result == NULL && i < Ports.size(); i++){
      AbstractPort * port = Ports.at(i);
      if (port->Type == portType && port->Name == needle){
        result = port;
      }
    }

    return result;
  }

  void PortRegistry::registerPort(AbstractPort * port){
    // std::cout << "register " << portf << std::endl;
    port->Id = ++EntryIncrement;
    Ports.push_back(port);

    port->subscribePortUpdateReveicer( this );
  }

  AbstractPort* PortRegistry::registerPortFromDescriptor(PortDescriptor * portDescriptor){
    assert( portDescriptor != NULL );
    assert( PortClassRegistryInfoMap.count(portDescriptor->Key) > 0 );

    return PortClassRegistryInfoMap[portDescriptor->Key]->Factory(this, portDescriptor);
  }


  void PortRegistry::connectPorts(AbstractPort *input, AbstractPort *output){
    assert(input != NULL);
    assert(output != NULL);

    input->addConnection(output);
    output->addConnection(input);
  }

  // void connectPortsByDescriptor(PortDescriptor * indesc, PortDescriptor * outdesc){
  //
  // }

  void PortRegistry::disconnectPorts(AbstractPort *input, AbstractPort *output){
    assert(input != NULL);
    assert(output != NULL);


    input->removeConnection(output);
    output->removeConnection(input);
  }


  void PortRegistry::enableAutoscan(unsigned int intervalMsec){
    if (AutoscanEnabled == true){
      return;
    }

    setAutoscanInterval(intervalMsec);

    AutoscanEnabled = true;

    AutoscanThread = std::thread([this]() {
      while (AutoscanEnabled)
      {
        this->rescan();
        std::this_thread::sleep_for(std::chrono::milliseconds(AutoscanIntervalMsec));
      }
    });
    AutoscanThread.detach();
  }

  void PortRegistry::disableAutoscan(){
    AutoscanEnabled = false;
    AutoscanThread.join();
  }

  void PortRegistry::deviceDiscovered(AbstractPort * port){
      std::cout << "deviceDiscovered " << port->Name << std::endl;
  }

  void PortRegistry::deviceStateChanged(AbstractPort * port, AbstractPort::DeviceState_t newState){
    std::cout << "deviceStateChanged for " << port->Name << " to " << (newState == AbstractPort::DeviceStateConnected ? "CONNECTED" : "NOT CONNECTED") << std::endl;
  }

}
